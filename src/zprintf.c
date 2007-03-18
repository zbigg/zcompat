/*
* File:		zprintf.c
*
* Id:           $Id: zprintf.c,v 1.4 2003/02/26 18:28:40 trurl Exp $
*
* Version:	0.1
* Date:		29.01.2002
*
* Project:	ZBIG Portable Input/Output library.
*
* Decription:

    zprintf	- stdout printf
    zfprintf	- stream printf
    zvfprintf
    zuprintf	- user printf
    zvuprintf

  This is file is under construction, and functions doesn't work
  properly.

*
* Author:	Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "zcompat/zsystype.h"
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

static int fprintf_flush(zvformatter_buff *vbuff)
{
    if( vbuff->param != NULL ) {
	int r = zwrite((ZSTREAM)vbuff->param,vbuff->bufpos,vbuff->buflen);
	if( r < 0 )
	    return -1;
	vbuff->curpos = vbuff->bufpos;
	return 0;
    } else {
	return -1;
    }
}

#ifndef SYSDEP_ZPRINTF
int    zprintf(const char* format,...)
{
    char buf[512];
    int cc;
    int r = 0;
    va_list ap;
    zvformatter_buff vbuff;

    vbuff.curpos = buf;
    vbuff.endpos = buf + sizeof(buf);
    vbuff.bufpos = buf;
    vbuff.buflen = 0;
    vbuff.param = zstdout;
    va_start(ap, format);
    cc = zvformatter(fprintf_flush, &vbuff, format, ap);
    va_end(ap);
    if( vbuff.bufpos != vbuff.curpos ) {
	register int x = vbuff.curpos - vbuff.bufpos;
	r = zwrite((ZSTREAM)vbuff.param,vbuff.bufpos,x);
    }
    return (r < 0) ? cc : r;
}

#endif

#ifndef SYSDEP_ZFPRINTF
int    zfprintf(ZSTREAM s,const char* format,...)
{
    char buf[512];
    int cc;
    int r = 0;
    va_list ap;
    zvformatter_buff vbuff;

    vbuff.curpos = buf;
    vbuff.endpos = buf + sizeof(buf);
    vbuff.bufpos = buf;
    vbuff.buflen = 0;
    vbuff.param = s;
    va_start(ap, format);
    cc = zvformatter(fprintf_flush, &vbuff, format, ap);
    va_end(ap);
    if( vbuff.bufpos != vbuff.curpos ) {
	register int x = vbuff.curpos - vbuff.bufpos;
	r = zwrite((ZSTREAM)vbuff.param,vbuff.bufpos,x);
    }
    return (r < 0) ? cc : r;
}
#endif

#ifndef SYSDEP_ZVFPRINTF
int    zvfprintf(ZSTREAM s,const char* format,va_list ap)

{
    char buf[512];
    int cc;
    int r = 0;
    zvformatter_buff vbuff;

    vbuff.curpos = buf;
    vbuff.endpos = buf + sizeof(buf);
    vbuff.bufpos = buf;
    vbuff.buflen = 0;
    vbuff.param = s;
    cc = zvformatter(fprintf_flush, &vbuff, format, ap);
    if( vbuff.bufpos != vbuff.curpos ) {
	register int x = vbuff.curpos - vbuff.bufpos;
	r = zwrite( (ZSTREAM)vbuff.param,vbuff.bufpos,x);
    }
    return (r < 0) ? cc : r;
}
#endif

#ifndef ZUMODE_OUT
#define ZUMODE_OUT   0
#define ZUMODE_ERR   1
#define ZUMODE_WARN  2
#endif

void    zuprintf(int umode,const char* fmt,...);
void    zvuprintf(int umode,const char* fmt,va_list ap);

#ifndef SYSDEP_ZUPRINTF
void    zuprintf(int umode,const char* fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    zvuprintf(umode,fmt,ap);
    va_end(ap);
}
#endif

#ifndef SYSDEP_ZVUPRINTF
void    zvuprintf(int umode,const char* fmt,va_list ap)
{
    switch( umode ) {
    case ZUMODE_OUT:
	zvfprintf(zstdout,fmt,ap);
	break;
    default:
	zvfprintf(zstderr,fmt,ap);
	break;
    }
}
#endif
