/*

* File: error.c
*
* Id:           $Id: error.c,v 1.3 2003/02/26 18:28:39 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  error routines
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

int zerrno = 0;

#ifndef SYSDEP_ZPERROR
void    zperror(const char* msg)
{
    const char *str = zstrerror(errno);
    if( str == NULL ) {
	str = "unknown error";
    }
    zfprintf(zstderr,"%s : %s\n",msg,str);
}
#endif

#ifndef SYSDEP_ZSTRERROR
const char*  zstrerror(int errmsg)
{
    char* strerr;
    switch( errmsg ) {
    case ZEINHOST:
	strerr = "host name invalid or not found";
	break;
    default:
#ifdef HAVE_STRERROR
	strerr = strerror(errmsg);
	if( !strerr )
	    return 0;
#else
	{
	    static char buf[100];
	    sprintf(buf,"error code: %i", errmsg);
	    return buf;
	}
#endif
    }
    return strerr;
}
#endif

#ifndef SYSDEP_ZSPERROR
void zsperror(ZSTREAM f,const char* msg)
{
    zfprintf(zstderr,
	"%s : %s : %s\n",

	msg,

	f->name ? f->name : "unknown stream",

	zstrerror(f->error));

}

#endif

