/*
* File:		zcompzt.c
*
* Id:           $Id$
*
* Project:	zcompat
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*   This program is Copyright(c) Zbigniew Zagorski 2004,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "zcompat/zdefs.h"

void	zc_strncpy(char* dest, const char* src, size_t len)
{
    strncpy(dest,src,len-1);
    dest[len-1] = '\0';
}
void	zc_strncat(char* dest, const char* src, size_t len)
{
    strncat(dest,src,len-1);
    dest[len-1] = '\0';
}

int	zc_strcasecmp(const char* a, const char* b)
{
#if defined ( HAVE_STRCASECMP )
    return strcasecmp(a,b);
#elif defined( HAVE_STRICMP )
    return stricmp(a,b);
#else
    register int i;
    while( *a && *b ) {
	i = tolower(*a++) - tolower(*b++);
	if( i ) return i;
    }
    return 0;
#endif
}
int	zc_strncasecmp(const char* a, const char* b,size_t len)
{
#if defined ( HAVE_STRNCASECMP )
    return strncasecmp(a,b,len);
#elif defined( HAVE_STRNICMP )
    return strnicmp(a,b,len);
#else
    register int i,j=0;
    while( j++ < len && *a && *b ) {
	i = tolower(*a++) - tolower(*b++);
	if( i ) return i;
    }
    return 0;
#endif
}

void (*_zcompat_atexit)(void (*p)(void)) = NULL;

void zc_atexit(void (*p)(void))
{
    if( _zcompat_atexit != NULL )
        _zcompat_atexit(p);
    else
#if defined(BUILD_XL)
    {
        void    xl_atexit(void(*)(void));
        xl_atexit(zpio_free_std_streams);
    }
#else
    {
        atexit(p);
    }
#endif
}
void    zc_set_atexit(void (*p)(void (*)(void)))
{
    _zcompat_atexit = p;
}
