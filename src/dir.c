/*
* File:		dir.c
*
* Id:           $Id: dir.c,v 1.4 2003/05/07 11:14:40 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  directory creating/removing/searching
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2002,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zsystype.h"

#ifdef ZSYS_WIN
#ifdef ZDEV_MSC
#include <direct.h>
#elif defined ZDEV_BORLAND
#include <dir.h>
#endif
#else
#ifdef ZSYS_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#else
	// TODO:  DOS-version needed ?
#endif
#endif

/* std */
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"


#ifndef SYSDEP_ZPIO_MKDIR
int	zpio_mkdir(const char* name)
{
#ifdef ZSYS_WIN
    return mkdir(name);
#else
    return mkdir(name,0755);
#endif
}

#endif /* SYSDEP_ZPIO_MKDIR */

#ifndef SYSDEP_ZPIO_MKPATH

static int	_zpio_mkpath(char* name);

int	zpio_mkpath(const char* name)
{
    char* xname = strdup(name);
    int r;
    int l = strlen(xname);
    while( xname[l-1] == Z_DIR_SEPARATOR ) { /* / or \ */
	xname[l-1] = 0;
	l--;
    }
    r = _zpio_mkpath(xname);
    free(xname);
    return r;
}

static int	_zpio_mkpath(char* name)
{
    if( zpio_mkdir(name) == 0 )
	return 0;
    else {
	char* l = strrchr(name,Z_DIR_SEPARATOR);
	if( l ) {
	    *l = '\0';
	    if( _zpio_mkpath(name) == 0 ) {
		*l = Z_DIR_SEPARATOR;
		return zpio_mkdir(name);
	    } else
		return -1;
	}
    }
    return -1;
}

#endif /* SYSDEP_ZPIO_MKPATH */

#ifndef SYSDEP_ZPIO_RMDIR
int	zpio_rmdir(const char* name,int recursive)
{
    return rmdir(name);
}
#endif /* SYSDEP_ZPIO_RMDIR */

