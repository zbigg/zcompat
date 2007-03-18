/*
* File: zopen.c
*
* Id:           $Id: zopen.c,v 1.3 2003/02/26 18:28:40 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  zopen
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2000-2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* std */
#include <stdlib.h>

#include "zcompat/zsystype.h"
#include <errno.h>
#include <string.h>
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

ZSTREAM zsopen_func	(const char* name,int mode,int* err);
ZSTREAM zhttpopen_func	(const char* name,int mode,int* err);
ZSTREAM zpopen_func	(const char* name,int mode,int* err);
ZSTREAM zdpopen_func	(const char* name,int mode,int* err);
ZSTREAM zgzip_open_func	(const char* name,int mode,int* err);

extern zpio_user_open_f	user_open_t[ZPIO_USER_OPEN_MAX+5];

int zpio_open_sopen_flag = 1;
int zpio_open_popen_flag = 1;
int zpio_open_dpopen_flag = 1;
int zpio_open_http_flag =
#if !defined ZSYS_NET_SUPPORT
			    0;
#else
			    1;
#endif


int zpio_open_gzip_flag =
#if !defined ZSYS_UNIX
			    0;
#else
			    1;
#endif


ZSTREAM	zopen(const char* name,int mode)
{
    ZSTREAM file = NULL;
    zpio_user_open_f* of = user_open_t;
    if( name == NULL ) {
	errno = EINVAL;
	return NULL;
    }
    while( file == NULL && *of != NULL ) {
	int	err = 0;
	file = (*of++)(name,mode,&err);
	if( err == ZR_ERROR )
	    return NULL;
    }
    if( file != NULL )
        return file;
    if(! strncmp(name,"file://",7) ) {
	name += 7;
    }
    return zfopen(name,mode);
}

int	zpio_register_user_open_func(zpio_user_open_f func)
{
    int i = 0;
    while ( i < ZPIO_USER_OPEN_MAX-1 ) {
	if( user_open_t[i] == NULL ) {
	    user_open_t[i] = func;
	    user_open_t[i+1] = NULL;
	    return 0;
	}
	i++;
    }
    return -1;
}

ZSTREAM zsopen_func(const char* name,int mode,int* err)
{
    if( !zpio_open_sopen_flag ) return NULL;
    if(! strncmp(name,"socket://",9) ) {
	char	host[256];
	int	host_name_len;
	short	port_number;
	char*	port_number_str;
	name += 9;
	port_number_str = strchr(name,':');
	if( port_number_str == NULL ) {
	    errno = EINVAL;
	    *err = ZR_ERROR;
	    return NULL;
	}
	port_number_str++;

	host_name_len = (int)port_number_str - (int)name;
	strncpy(host,name,host_name_len);
	host[host_name_len-1] = 0;

	port_number = (short) atoi(port_number_str);
	if( port_number == 0 ) {
	    errno = EINVAL;
	    *err = ZR_ERROR;
	    return NULL;
	}
	return zsopen(host,port_number);
    }
    return NULL;
}
ZSTREAM zhttpopen_func(const char* name,int mode,int* err)
{
    if( !zpio_open_http_flag ) return NULL;
    if(! strncmp(name,"http://",7) ) {
	int http_result = 0;
	ZFILE* stream = zhttpopen(name,1,&http_result);
	if( stream != NULL ) {
	    stream->error = http_result;
	} else {
	    *err = ZR_ERROR;
	    return NULL;
	}
	if( http_result != 200 ) {
	    *err = ZR_ERROR;
	    switch( http_result ) {
	    case 401:
	    case 403:
		errno = EACCES;
		break;
	    default:
		errno = ENOENT;
		break;
	    }
	    zclose(stream);
	    return NULL;
	}
	zsetbuf(stream,512,0);
	return stream;
    }
    return NULL;
}

ZSTREAM zpopen_func(const char* name,int mode,int* err)
{
    if( !zpio_open_popen_flag ) return NULL;
    if(! strncmp(name,"pipe://",7) ) {
	ZSTREAM stream;
	name += 7;
	stream = zpopen(name,mode);
	if( stream == NULL )
	    *err = ZR_ERROR;
	zsetbuf(stream,512,0);
	return stream;
    }
    return NULL;
}

ZSTREAM zdpopen_func(const char* name,int mode,int* err)
{
    if( !zpio_open_dpopen_flag ) return NULL;
    if(! strncmp(name,"dpipe://",8) ) {
	ZSTREAM stream;
	name += 8;
	stream = zpopen(name,ZO_RDWR);
	if( stream == NULL )
	    *err = ZR_ERROR;
	return stream;
    }
    return NULL;
}

ZSTREAM zgzip_open_func(const char* name,int mode,int* err)
{
    char* t;
    if( !zpio_open_gzip_flag ) return NULL;
    t = strstr(name,".gz");
    if( t != NULL && t[3] == 0 ) {
	ZSTREAM f;
	char* cmd;
	if( (mode & ZO_READ) == ZO_READ)
	    cmd = zsaprintf(NULL,0,"gzip -cd %s",name);
	else
	    cmd = zsaprintf(NULL,0,"gzip -c > %s",name);
	if( cmd == NULL )
	    return NULL;
	f = zpopen(cmd,mode);
	zpio_free(cmd);
	if( f == NULL )
	    *err = ZR_ERROR;
	return f;
    }
    return NULL;
}

#if 0
ZSTREAM zsopen_func(const char* name,int mode,int* err)
{
    return NULL;
}
#endif

zpio_user_open_f	user_open_t[ZPIO_USER_OPEN_MAX+5] =
{
    zsopen_func,
    zhttpopen_func,
    zpopen_func,
    zdpopen_func,
    zgzip_open_func,
    NULL
};


