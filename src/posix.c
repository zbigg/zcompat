/*
* File: posix.c
*
* Id:           $Id: posix.c,v 1.7 2003/05/07 12:20:36 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:	posix I/O routines
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2002,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zsystype.h"

#if( defined ZSYS_WIN || defined ZSYS_MSDOS) && !defined ZDEV_DJGPP
#include <io.h>
#include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

/* POSIX IO, based on descriptors */
ZSTREAM zposix_open	(ZSTREAM,const char*,int);
int	zposix_close	(ZSTREAM);
int	zposix_read	(ZSTREAM,void*,size_t);
int	zposix_write	(ZSTREAM,const void*,size_t);
zoff_t	zposix_seek	(ZSTREAM,zoff_t,int);
long	zposix_ctl	(ZSTREAM,int,void*);

int	zpio_openflags_2_posix_openflags(int zf);

zstream_vtable_t zvt_posix = {
    0,			/*__ id */
    1,			/*__ data_size */
    zposix_open,	/*__ open */
    zposix_close,	/*__ close */
    zposix_read,	/*__ read */
    zposix_write,	/*__ write */
    zposix_seek,	/*__ seek */
    zposix_ctl, 	/*__ ctl */
};

/* ------------------
	     PUBLIC
	    -----------------*/



/* ----------------------====*****	POSIX
*/

#define posix_d		(f->data[0])
ZSTREAM zposix_open	(ZSTREAM f,const char* name,int mode)
{
    if( name != NULL ) {
	posix_d = open(name,
		zpio_openflags_2_posix_openflags(mode)
#if 1
,
#ifdef ZSYS_UNIX
		00644
#else
		0
#endif
#endif
		 );
	f->name = zcompat_strdup(name);
    } else {
	posix_d = mode;
	f->name = 0;
	ZFL_SET(f->flags, ZSF_PUBLIC);
#if defined(HAVE_FCNTL) && defined (F_GETFL)
	{ /* get access mode */
	    int fl = fcntl(posix_d,F_GETFL);
	    int ofl = 0;
	    if( fl != -1 ) {
		if( ZFL_ISSET( fl, O_RDONLY) )
		    ofl |= ZO_READ;
		if( ZFL_ISSET( fl, O_WRONLY) )
		    ofl |= ZO_WRITE;
		if( ZFL_ISSET( fl, O_RDWR) )
		    ofl |= ZO_WRITE | ZO_READ;
#ifdef O_APPEND
		if( ZFL_ISSET( fl, O_APPEND) )
		    ofl |= ZO_APPEND;
#endif
#ifdef O_EXCL
		if( ZFL_ISSET( fl, O_EXCL) )
		    ofl |= ZO_EXCL;
#endif
#ifdef O_NONBLOCK
		if( ZFL_ISSET( fl, O_NONBLOCK) )
		    ofl |= ZO_NODELAY;
#endif
	    } else {
		ofl = ZO_RDWR;
	    }
	    f->mode = ofl;
	}
#else /* haven't got fcntl */
	f->mode = ZO_RDWR;
#endif
    }
    if( posix_d < 0 )
	return NULL;
    {
	struct stat st;
	int r = fstat( posix_d, &st);
	if( r < 0 ) {
	    ZFL_SET(f->flags,ZSF_FILE);
	} else {
#ifdef ZSYS_WIN
	    unsigned short m = st.st_mode;
#else
	    mode_t m = st.st_mode;
#endif
	    int is_file = 0;
	    int is_pipe = 0;
#if	defined S_ISREG
	    is_file = S_ISREG(m);
#elif	defined S_IFREG
	    is_file = ZFL_ISSET(m,S_IFREG);
#elif	defined _S_IFREG
	    is_file = ZFL_ISSET(m,_S_IFREG);
#endif

#if	defined S_ISFIFO
	    is_pipe |= S_ISFIFO(m);
#elif	defined S_IFIFO
	    is_pipe |= ZFL_ISSET(m,S_IFIFO);
#elif	defined _S_IFIFO
	    is_pipe |= ZFL_ISSET(m,_S_IFIFO);
#endif

	    if( ! is_pipe ) {
#if	defined S_ISCHR
		is_pipe |= S_ISCHR(m);
#elif	defined S_IFCHR
		is_pipe |= ZFL_ISSET(m,S_IFCHR);
#elif	defined _S_IFCHR
		is_pipe |= ZFL_ISSET(m,_S_IFCHR);
#endif
	    }

	    if( ! is_pipe ) {
#if	defined S_ISSOCK
		is_pipe |= S_ISSOCK(m);
#elif	defined S_IFSOCK
		is_pipe |= ZFL_ISSET(m,S_IFSOCK);
#elif	defined _S_IFSOCK
		is_pipe |= ZFL_ISSET(m,_S_IFSOCK);
#endif
	    }
	    if( is_pipe )
        	ZFL_UNSET(f->flags, ZSF_FILE);
	    else
		ZFL_SET(f->flags, ZSF_FILE);
	}
    }

    return f;
}
int	zposix_close	(ZSTREAM f)
{
    int e = 0;
    if( !ZFL_ISSET(f->flags, ZSF_PUBLIC) )
	e = close(posix_d);
    if( e < 0 )
	f->error = e;
    return e;
}
int	zposix_read	(ZSTREAM f,void* data,size_t size)
{
    size_t e = read(posix_d,data,size);
    if( e == (size_t)-1 )
	f->error = errno;
    if( e == 0 )
	f->eof = 1;
    return (int)e;
}
int	zposix_write	(ZSTREAM f,const void* data,size_t size)
{
    size_t e = write(posix_d,data,size);
    if( e == (size_t)-1 )
	f->error = errno;
    if( e == 0 )
	f->eof = 1;
    return (int)e;
}
zoff_t	zposix_seek	(ZSTREAM f,zoff_t offset,int whence)
{
    register off_t e = lseek(posix_d,offset,whence);
    if( e == (off_t)-1 )
	f->error = (int)errno;
    else if( whence == ZSEEK_END && offset == 0 )
	f->eof = 1;
    return (zoff_t)e;
}

long	zposix_ctl	(ZSTREAM f,int what,void* data)
{
    switch( what ) {
    case ZFCTL_DUP:
        {
            int fd = dup(posix_d);
            if( fd < 0 ) return -1;
            return (long)zdopen(fd,f->name);
        }
    case ZFCTL_GET_READ_DESC:
	return ZFL_ISSET(f->mode,ZO_READ) ? posix_d : -1;

    case ZFCTL_GET_WRITE_DESC:
	return ZFL_ISSET(f->mode,ZO_WRITE) ? posix_d : -1;

    case ZFCTL_GET_READ_STREAM:
	return ZFL_ISSET(f->mode,ZO_READ) ? (long)f : (long)NULL;

    case ZFCTL_GET_WRITE_STREAM:
	return ZFL_ISSET(f->mode,ZO_WRITE) ? (long)f : (long)NULL;

    case ZFCTL_GET_BLOCKING:
	{
#if defined(O_NONBLOCK) && defined(F_GETFL)
	    int fl = fcntl(posix_d, F_GETFL );
	    if( fl < 0 )
		return -1;
	    return ( fl & O_NONBLOCK ) == O_NONBLOCK;
#else
	    errno = EINVAL;
	    return 0;
#endif
	}
    case ZFCTL_SET_BLOCKING:
	{
#ifdef O_NDELAY
	    int fl = fcntl(posix_d, F_GETFL );
	    if( fl < 0 )
		return -1;
	    if( (int)data )
		fl |= O_NDELAY;
	    else
		fl &= ~(O_NDELAY);
	    if( fcntl(posix_d, F_SETFL ,fl ) < 0 )
		return -1;
	    return 0;
#else
	    errno = EINVAL;
	    return -1;
#endif
	}

    case ZFCTL_EOF:
	{
	    return f->eof;
	}
    }
    errno = EINVAL;
    return -1;
}

#undef posix_fd

int	zpio_openflags_2_posix_openflags(int zf)
{
    int r = 0;

    if	   ( ( zf & ZO_RDWR ) == ZO_RDWR )
	r = O_RDWR;
    else if( ( zf & ZO_RD ) == ZO_RD )
	r = O_RDONLY;
    else if( ( zf & ZO_WR ) == ZO_WR )
	r = O_WRONLY;

#ifdef O_APPEND
    if(( zf & ZO_APPEND ) == ZO_APPEND )
	r |= O_APPEND;
#endif

#ifdef O_CREAT
    if(( zf & ZO_CREAT ) == ZO_CREAT )
	r |= O_CREAT;
#endif

#ifdef O_NDELAY
    if(( zf & ZO_NODELAY ) == ZO_NODELAY )
	r |= O_NDELAY;
#endif

#ifdef O_EXCL
    if(( zf & ZO_EXCL ) == ZO_EXCL )
	r |= O_EXCL;
#endif

#ifdef O_TRUNC
    if(( zf & ZO_TRUNC ) == ZO_TRUNC )
	r |= O_TRUNC;
#endif

#ifdef O_TEXT
    if(( zf & ZO_TEXT ) == ZO_TEXT )
	r |= O_TEXT;
#endif

#ifdef O_BINARY
    if(( zf & ZO_BINARY )== ZO_BINARY )
	r |= O_BINARY;
#endif
    return r;
}
