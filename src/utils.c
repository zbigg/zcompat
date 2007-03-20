/*
* File: utils.c
*
* Id:           $Id: utils.c,v 1.7 2003/06/28 11:57:44 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  Other functions.
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2004,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zdefs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#if defined(HAVE_SENDFILE) && defined(linux)
#include <sys/sendfile.h>
#endif
#if defined (HAVE_SENDFILE) && defined(ZSYSD_FREEBSD)
#include <sys/socket.h>
#include <sys/uio.h>
#endif

#if defined ZSYS_WIN || defined ZSYS_MSDOS

#include <io.h>

#else

#include <unistd.h>
#include <sys/types.h>

#endif

#include "zcompat/zcompat.h"
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"
char*	zst_strdup2(const char* s1,const char* s2);
#ifndef HAVE_STRCASECMP
int	strcasecmp(const char* s1,const char* s2)
{
    return zc_strcasecmp(s1,s2);
}
#endif

ZFILE*  zhttpopen(const char* httpfilename,int options,int* result)
{
    char hostname[0xff],
	filename[0xfff],
	request[0xfff],
	portnumber[0xff],
	*buf = (char*)httpfilename,
	*buf1;
    int	len;
    short port;
    ZSTREAM f;
    int a , c;
    if( strncmp(httpfilename,"http://",7) == 0) {
	httpfilename+=7;
    }
    c = strcspn(httpfilename,":/");
    if( c > (sizeof(hostname)-2) ) {
	errno = EINVAL;
        return 0;
    }
    strncpy(hostname,httpfilename,c);
    hostname[c] = 0;
    httpfilename+=c;
    if(httpfilename[0] == ':') {
        a = strcspn(httpfilename,"/");
        if( a > (sizeof(portnumber)-2) ) {
	    errno = EINVAL;
            return 0;
	}
        strncpy(portnumber,httpfilename,a);
        httpfilename += a;
    } else
        strcpy(portnumber,"80");
    strncpy(filename,httpfilename,sizeof(filename)-1);
    httpfilename = buf;
    if(filename[0] == '\0' )
        strcpy(filename,"/");
    zsnprintf(request,sizeof(request),
	"GET %s HTTP/1.0\r\n"
	"Host: %s:%s\r\n"
	"Accept: */*\r\n"
	"User-Agent: libzpio\r\n"
	"\r\n",filename,hostname,portnumber);
    port = (short)atoi(portnumber);
    f = zsopen(hostname,port);
    if( f == NULL )
        return NULL;
    if( f->name != NULL) {
        zpio_free(f->name);
    }
    f->name = zsaprintf(NULL,0,"http://%s",httpfilename);

    zwrite(f,request,strlen(request));

    if( options == 1) {
        len = zfgets(f,filename,sizeof(filename)-1);
        while(len > 0) {
            buf = filename;
            /*
	    Header is like this
               HTTP/1.1 200 OK  or
               HTTP/1.0 404 Not found etc etc
            */
            if( strncmp(buf,"HTTP/",5) == 0 ) {
                buf += 5;
                while(*buf !=' ')  /* skip word */
                    buf++;
                while(*buf == ' ') /* skip spaces */
                    buf++;
                buf1 = buf;
                while(*buf!=' ')   /* skip word */
                    buf++;
                *buf = 0;          /* in buf1 we have response code */
                if( result ) {
                    *result = atoi(buf1);
                }
            } else {
		/* bad header */
	    }
            len = zfgets(f,filename,sizeof(filename)-1);
        }
    }
    return f;
}

int	zpio_send(ZSTREAM dest,ZSTREAM src,int max,int flags)
{
    char buf[512];
    int r,already_sent = 0,to_send;
#if defined(HAVE_SENDFILE) && defined(linux)
    if( !dest->chain && !src->chain ) {
	int rd,wd;
	rd = zfdesc_r(src);
	wd = zfdesc_w(dest);
	if( rd >= 0 && wd >= 0 ) {
	    ssize_t sent_bytes;
	    if( max == -1 ) {
		if( ZFL_ISSET(src->flags,ZSF_FILE) ) {
		    struct stat st;
		    off_t	pos;
		    if( fstat(rd,&st) < 0 )
			goto sendfile_failed;
		    pos = lseek(rd,0,SEEK_CUR);
		    if( pos == (off_t) -1 )
			goto sendfile_failed;
		    max = st.st_size - pos;
		    sent_bytes = sendfile(wd,rd,NULL,(ssize_t)max);
		    if( sent_bytes > 0 )
			lseek(rd,sent_bytes,SEEK_CUR);
		    return sent_bytes;
		} else {
		    /*
			rd is a pipe like descriptor so we can't
			determine size.
		    */
		    goto sendfile_failed;

#if 0
		    ssize_t already_sent;
		    while( 1 )
#endif
		}
	    } else {
		    sent_bytes = sendfile(wd,rd,NULL,(ssize_t)max);
		    if( sent_bytes > 0 )
			lseek(rd,sent_bytes,SEEK_CUR);
		    return sent_bytes;
	    }
	}
    }
sendfile_failed:
#endif
#if defined (HAVE_SENDFILE) && defined(ZSYSD_FREEBSD)
    /*
	NOTE:
	    this code compiles but hasn't been tested against work
	    
    */
    if( !dest->chain && !src->chain ) {
	int rd,wd;
	struct stat sstat, dstat;
	off_t ofs;
	rd = zfdesc_r(src);
	wd = zfdesc_w(dest);
	if( fstat(rd, &sstat) < 0 || fstat(wd, &dstat) < 0 )
	    return -1;
	if( !(sstat.st_mode & S_IFREG) || !(dstat.st_mode & S_IFSOCK) ) 
	    goto sendfile_failed;
	ofs = lseek(rd,0,SEEK_CUR);
	{
	    struct sf_hdtr sf;
	    off_t written;
	    if( sendfile(rd,wd,ofs,max, &sf, &written,0) < 0 )
		return -1;
	}
sendfile_failed:
    }
#endif
    while( 1 ) {
	if( max >= 0 )
	    to_send = zmin(
			sizeof(buf),
			max - already_sent
			);
	else
	    to_send =	sizeof(buf);
	if( to_send <= 0 )
	    return already_sent;
	r = zread(src,buf,to_send);
	if( r <= 0 )
	    return already_sent;
	if( r > 0 ) {
	    r = zwrite(dest,buf,r);
	    if( r < 0 )
		return already_sent;
	    already_sent += r;
	}
    }
}

zstream_vtable_t* zpio_vt_dup(const zstream_vtable_t* vt)
{
    zstream_vtable_t* n = zcompat_calloc(1,sizeof (zstream_vtable_t) );
    if( n == NULL )
	return NULL;
    memcpy(n,vt,sizeof (zstream_vtable_t));
    return n;
}


int	zpio_find_file_by_path(
    const char* path,
    const char* file,
    int		type,
    char* 	buf,
    size_t 	max)
{
    char	tmp[Z_PATH_MAX];
    register char*	c;
    if( path == NULL || file == NULL )
	return 0;
    if( zpio_path_is_abs(file) )
	return zpio_access(file,type);
    c = (char*)path;
    while( *c != '\0' ) {
	int len = strcspn(c,Z_PATH_SEPARATOR_T);
	tmp[0] = 0;
	if( len > 0 ) {
	    strncpy(tmp,c,len);
	    if( tmp[len-1] != Z_DIR_SEPARATOR ) {
		tmp[len] = Z_DIR_SEPARATOR;
		tmp[len+1] = 0;
	    } else
		tmp[len] = 0;
	}
	strncat(tmp,file,Z_PATH_MAX-1);
	tmp[Z_PATH_MAX-1] = 0;

	if( zpio_access(tmp,type) == 0 ) {
	    if( buf != NULL ) {
		strncpy(buf,tmp,max);
		buf[max-1] = 0;
	    }
	    return 0;
	}
	c += len;
	if( *c++ == '\0' )
	    return -1;
    }
    return -1;
}

int	zpio_find_file_by_paths(
    const char* path[],
    const char* file,
    int		type,
    char* 	buf,
    size_t 	max)
{
    do {
	if( zpio_find_file_by_path(*path++,file,type,buf,max) == 0 )
	    return 0;
    } while( *path != NULL );
    return -1;
}

int	zpio_path_is_abs(const char* path)
{
    if( path == NULL )
	return 0;
#ifdef ZSYS_MAC
    if( (path[0] >= 'A' && path[0] <= 'Z' ||
	    path[0] >= 'a' && path[0] <= 'z' ) &&
	path[1] == ':' && path[2] == ':' )
#else
    if( path[0] == Z_DIR_SEPARATOR )
	return 1;
#if defined(ZSYS_DOS) || defined(ZSYS_WIN)
    if( (path[0] >= 'A' && path[0] <= 'Z' ||
	    path[0] >= 'a' && path[0] <= 'z' ) &&
	path[1] == ':' && path[2] == Z_DIR_SEPARATOR )
	return 1;
#endif
#endif
    return 0;
}

int	zpio_file_has_ext(const char* name)
{
    return name != NULL && strrchr(name,'.') > strrchr(name,Z_DIR_SEPARATOR);
}

int	zpio_file_set_ext(const char* name,const char* ext,char* buf,int max)
{
    int len;
    char* x = strrchr(name,Z_DIR_SEPARATOR);
    if( x != NULL )
	x++;
    else
	x = (char*)name;
    len = strcspn(x,".") + x-name;
    len = len>max ? max : len;
    strncpy(buf,name,len);
    buf[len] = 0;
    strncat(buf,ext,max);
    buf[max-1] = 0;
    return 0;
}

/** Convert path to current system format.
*/
void	zpio_path_to_sys(char* name,int len)
{
    for( ; len > 0; name++,len-- ) {
	switch( *name ) {
	case '\\':
	case '/':
	case ':':
	    *name = Z_DIR_SEPARATOR;
	    break;
	default:
	    /* nothing */;
	}
    }
}

/** Convert path to portable format.
*/
void	zpio_path_to_portable(char* name,int len)
{
    for( ; len > 0; name++,len-- ) {
	switch( *name ) {
	case '\\':
	case '/':
	case ':':
	    *name = '/';
	    break;
	default:
	    /* nothing */;
	}
    }
}

#ifndef SYSDEP_ZPIO_ACCESS
int	zpio_access(const char* path,int type)
{
#if 1
    return access(path,type);
#else	/* we have got access */
    return 0;
#endif  /* we haven't got access, we return true */
}
#endif /* SYSDEP_ZPIO_ACCESS */

#ifndef SYSDEP_ZPIO_RM
int	zpio_rm(const char* name)
{
    return remove(name);
}
#endif /* SYSDEP_ZPIO_RM */


#ifndef SYSDEP_ZPIO_ACCESS
int	zpio_chmod_ex(const char* path,int permit,char* who,int what)
{
#ifdef ZSYS_UNIX
    struct stat ss;
    mode_t	m;
    mode_t	user = 0,global = 0;
    if( stat(path,&ss) == -1 )
	return -1;
    m = ss.st_mode;
    if( ZFL_ISSET(what,Z_ACCESS_READ) )
	user |= 04;
    if( ZFL_ISSET(what,Z_ACCESS_WRITE) )
	user |= 02;
    if( ZFL_ISSET(what,Z_ACCESS_EXEC) )
	user |= 01;

    if( ZFL_ISSET(what,Z_ACCESS_SUID) )
	global |= S_ISUID;
    if( ZFL_ISSET(what,Z_ACCESS_SGID) )
	global |= S_ISGID;

    if( who != NULL ) {
	if( !strcmp(who,Z_PERSON_ALL  ) )
	    user = user | (user << 3) | (user << 6);
	else if( !strcmp(who,Z_PERSON_GROUP) )
	    user = (user << 3);
	else if( !strcmp(who,Z_PERSON_OTHER) )
	    user = (user << 6);
    } else
	user = 0;
    if( permit == ZPIO_ALLOW )
	m |= user | global;
    else
	m &= ~(user | global);
    return chmod(path,m);
#elif defined ZSYS_MSDOS || defined ZSYS_WIN
        int amode = 0;
        if( ZFL_ISSET(what,Z_ACCESS_READ) )
	        amode |= S_IREAD;
        if( ZFL_ISSET(what,Z_ACCESS_WRITE) )
	        amode |= S_IWRITE;
        return chmod(path,amode);
#else
    errno = ENODEV;
    return -1;
#endif
}
#endif /* SYSDEP_ZPIO_CHMODEX */
