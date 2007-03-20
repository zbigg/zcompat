/*
* File:		netio.c
*
* Id:           $Id: netio.c,v 1.4 2003/05/07 11:34:35 trurl Exp $
*
* Version:	0.1
* Date:		29.01.2002
*
* Project:	ZBIG Portable Input/Output library.
*
* Decription:

  Network byteorder safe input/output.

* Provided functions

    znwrite_l(ZSTREAM,const unsigned long* )
    znwrite_lv(ZSTREAM,const unsigned long*, size_t)

    znwrite_h(ZSTREAM,const unsigned long* )
    znwrite_hv(ZSTREAM,const unsigned long*, size_t)

    znwrite_s(ZSTREAM,const char**)


    znread_l(ZSTREAM,unsigned long* )
    znread_lv(ZSTREAM,unsigned long*,size_t)

    znread_h(ZSTREAM,unsigned long* )
    znread_hv(ZSTREAM,unsigned long*,size_t)

    znwrite_s(ZSTREAM,const char**)


    unsigned long  zntohl(unsigned long)
    unsigned short zntohs(unsigned short)
    unsigned long  zhtonl(unsigned long)
    unsigned short zhtons(unsigned short)

  This is file is under construction, and functions doesn't work
  properly.

*
* Author:	Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2001-2002,
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
#include <string.h>

#include "zcompat/zsystype.h"
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"


int	znwrite_l(ZSTREAM f,unsigned long* l)
{
    unsigned long x = *l;
    x = zhtonl(x);
    return zwrite(f,&x,sizeof(x));
}
int	znwrite_lv(ZSTREAM f,const unsigned long* v,size_t count)
{
    unsigned long x[50];
    size_t i = 0,written = 0,l;
    while( count > 0 ) {
	l = zmin(countof(x),count);
	for( i = 0; i < l; i++ )
	    x[i] = zhtonl(v[i + written]);
	i = zwrite(f,x,sizeof( x[0] ) * l );
	if( i < 0) return written > 0 ? written : -1;
	written += i;
	count -= i;
    }
    return written;
}

int	znwrite_h(ZSTREAM f,unsigned short* h)
{
    unsigned short x = *h;
    x = zhtons(x);
    return zwrite(f,&x,sizeof(x));
}
int	znwrite_hv(ZSTREAM f,const unsigned short* v,size_t count)
{
    unsigned short x[50];
    size_t i = 0,written = 0,l;
    while( count > 0 ) {
	l = zmin(countof(x),count);
	for( i = 0; i < l; i++ )
	    x[i] = zhtons(v[i + written]);
	i = zwrite(f,x,sizeof( x[0] ) * l );
	if( i < 0 ) return written > 0 ? written : -1;
	written += i;
	count -= i;
    }
    return written;
}

int	znwrite_s(ZSTREAM f,const char** s)
{
    unsigned long len = strlen(*s);
    return znwrite_l(f,&len) + zwrite(f,*s,len);
}

int	znread_l(ZSTREAM f,unsigned long* l)
{
    int e = zread(f,l,sizeof(*l));
    *l = zntohl(*l);
    return e;
}
int	znread_lv(ZSTREAM f,unsigned long* v,size_t count)
{
    size_t readed = zread(f,v, sizeof(v[0]) * count);
    size_t i;
    if( readed > 0 )
	for(i = 0; i < readed; i++ )
	    v[i] = zntohl(v[i]);
    return readed;
}

int	znread_h(ZSTREAM f,unsigned short* h)
{
    int e = zread(f,h,sizeof(*h)); 
    *h = zntohs(*h);
    return e;
}
int	znread_hv(ZSTREAM f,unsigned short* v,size_t count)
{
    size_t readed = zread(f,v, sizeof(v[0]) * count);
    size_t i;
    if( readed > 0 )
	for(i = 0; i < readed; i++ )
	    v[i] = zntohs(v[i]);
    return readed;
}


int	znread_s(ZSTREAM f,char** c)
{
    unsigned long l;
    int r;
    *c = NULL;
    if( (r = znread_l(f,&l)) > 0 ) {
	*c = zcompat_malloc(l+1);
	if( !c ) return -1;
	r += zread(f,*c,l);
	(*c)[l] = 0;
	return r;

    } else return -1;
}


#ifdef HAVE_ENDIAN_H
#include <endian.h>
#else
/*
    If you don't have this header on your system please
    specify the platform byte order by defining :
	__BYTE_ORDER to
	    1234 on Little Endian platforms : (i386, alpha, mips )
	    4321 on Big Endian platforms
    in
*/
#endif

#if 0
#	define __BYTE_ORDER your_byte_order
#endif

#ifndef __LITTLE_ENDIAN
#define	__LITTLE_ENDIAN	1234
#endif

#ifndef __BIG_ENDIAN
#define	__BIG_ENDIAN	4321
#endif

#ifndef __PDP_ENDIAN
#define	__PDP_ENDIAN	3412
#endif

#ifndef __BYTE_ORDER

#if defined(ZARCH_I386)	    || \
    defined(__i386__)	    || \
    defined(WIN32)	    || \
    defined(ZARCH_ALPHA)    || \
    defined(__alpha__)	    || \
    defined(__alpha)	    || \
    (defined(__mips__) && defined(__MIPSEL__)) || \
    defined(__arm__)	    || \
    defined(__ia64__)	    || \
    defined(__LITTLE_ENDIAN__)

#define __BYTE_ORDER __LITTLE_ENDIAN
#else
#define __BYTE_ORDER __BIG_ENDIAN
#endif

#endif /* ifndef __BYTE_ORDER */



#ifndef SYSDEP_NET2HOST

#define ZPIO_LIL_ENDIAN	1234
#define ZPIO_BIG_ENDIAN	4321

#ifndef __BYTEORDER
#if  defined(__i386__) || defined(__ia64__) || defined(WIN32) || \
    (defined(__alpha__) || defined(__alpha)) || \
     defined(__arm__) || \
    (defined(__mips__) && defined(__MIPSEL__)) || \
     defined(__LITTLE_ENDIAN__)
#define ZPIO_BYTEORDER	ZPIO_LIL_ENDIAN
#else
#define ZPIO_BYTEORDER	ZPIO_BIG_ENDIAN
#endif
#else
#define ZPIO_BYTEORDER __BYTEORDER
#endif

#define NET_BYTEORDER	ZPIO_BIG_ENDIAN

#undef zntohl
#undef zntohs
#undef zhtonl
#undef zhtons

#define MOVEBYTE(i,from,to) (( ((i) & (0xff << 8*from)) >> 8*from ) << 8*to )

unsigned long 	zntohl(unsigned long i)
{
#if ZPIO_BYTEORDER == ZPIO_LIL_ENDIAN
    /* byte nt */
    /* 4321 > 1234 */
    return  MOVEBYTE(i,3,0) |
	    MOVEBYTE(i,2,1) |
	    MOVEBYTE(i,1,2) |
	    MOVEBYTE(i,0,3);
#else
    return i;
#endif
}

unsigned short	zntohs(unsigned short i)
{
#if ZPIO_BYTEORDER == ZPIO_LIL_ENDIAN
    /* byte nt */
    /* 21 > 12 */
    return  (unsigned short)((
		MOVEBYTE(i,1,0) |
		MOVEBYTE(i,0,1))
	    );
#else
    return i;
#endif
}
unsigned long 	zhtonl(unsigned long i)
{
#if ZPIO_BYTEORDER == ZPIO_LIL_ENDIAN
    /* byte nt */
    /* 1234 > 4321 */
    return  MOVEBYTE(i,0,3) |
	    MOVEBYTE(i,1,2) |
	    MOVEBYTE(i,2,1) |
	    MOVEBYTE(i,3,0);
#else
    return i;
#endif
}
unsigned short	zhtons(unsigned short i)
{
#if ZPIO_BYTEORDER == ZPIO_LIL_ENDIAN
    /* byte nt */
    /* 12 > 21 */
    return  (unsigned short)(
		MOVEBYTE(i,0,1) |
		MOVEBYTE(i,1,0)
	    );
#else
    return i;
#endif
}

#endif /* #ifdef SYSDEP_NET2HOST */
