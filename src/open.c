/*
* File: open.c
*
* Id:           $Id: open.c,v 1.3 2003/02/26 18:28:40 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  stream open functions
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2001,
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

#define ZPIO_WITH_POSIX 1

/* ----------------------====*****      DEFAULT
*/
#ifdef ZPIO_WITH_POSIX
extern zstream_vtable_t zvt_posix;
#endif

#ifdef ZSYS_WIN
extern zstream_vtable_t zvt_win32file;
#endif

extern zstream_vtable_t zvt_pipe;
extern zstream_vtable_t zvt_dstream;
extern zstream_vtable_t zvt_ansi;
extern zstream_vtable_t zvt_membuf;

#undef zfopen_preffered

int		zfopen_preffered =
#if   defined(ZSYS_WIN)
			    ZFOP_WIN32;
#elif defined(ZSYS_UNIX)
			    ZFOP_POSIX;
#else
			    ZFOP_ANSI;
#endif
int*		zpio_get_zfopen_preffered(void)
{
	return &zfopen_preffered;
}

ZSTREAM zfopen(const char* name,int mode)
{
    switch( zfopen_preffered ) {
    case ZFOP_POSIX:
	return zcreat(&zvt_posix,name,mode);

#ifdef ZSYS_WIN
    case ZFOP_WIN32:
	return zcreat(&zvt_win32file,name,mode);
#endif

    case ZFOP_ANSI:
    default:
	return zcreat(&zvt_ansi,name,mode);
    }
}

ZSTREAM zdopen(int fd,const char* filename)
{
    ZSTREAM a = zcreat(&zvt_posix,NULL,fd);
    if( a != NULL  ) {
	a->name = zpio_strdup(filename);
    }
    return a;
}
ZSTREAM zansiopen(FILE* f,const char* filename)
{
    ZSTREAM a = zcreat(&zvt_ansi,NULL,(int)f);
    if( a != NULL ) {
	a->name = zpio_strdup(filename);
    }
    return a;
}


