/*
* File: binio.c
*
* Id:           $Id: binio.c,v 1.3 2003/02/26 18:28:39 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  binary input/output functions
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2001-2002,
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

int	zbwrite_l(ZSTREAM f,long* l)
{
    return zwrite(f,l,sizeof(l[0]));
}
int	zbwrite_lv(ZSTREAM f,const long* v,size_t count)
{
    return zwrite(f,v, sizeof(long) * count);
}

int	zbwrite_i(ZSTREAM f,int* i)
{
    return zwrite(f,i,sizeof(i[0]));
}
int	zbwrite_iv(ZSTREAM f,const int* v,size_t count)
{
    return zwrite(f,v, sizeof(v[0]) * count);
}

int	zbwrite_h(ZSTREAM f,short* h)
{
    return zwrite(f,h,sizeof(h[0]));
}
int	zbwrite_hv(ZSTREAM f,const short* v,size_t count)
{
    return zwrite(f,v, sizeof(v[0]) * count);
}

int	zbwrite_c(ZSTREAM f,char* c)
{
    return zwrite(f,c,sizeof(c[0]));
}
int	zbwrite_cv(ZSTREAM f,const char* s,size_t count)
{
    return zwrite(f,s, sizeof(s[0]) * count);
}

int	zbwrite_s(ZSTREAM f,const char** s)
{
    long len = strlen(*s);
    return zbwrite_l(f,&len) + zbwrite_cv(f,*s,len);
}

int	zbread_l(ZSTREAM f,long* l)
{
    return zread(f,l,sizeof(*l));
}
int	zbread_lv(ZSTREAM f,long* v,size_t count)
{
    return zread(f,v, sizeof(v[0]) * count);
}

int	zbread_i(ZSTREAM f,int* i)
{
    return zread(f,i,sizeof(*i));
}
int	zbread_iv(ZSTREAM f,int* v,size_t count)
{
    return zread(f,v, sizeof(v[0]) * count);
}

int	zbread_h(ZSTREAM f,short* h)
{
    return zread(f,h,sizeof(*h));
}
int	zbread_hv(ZSTREAM f,short* v,size_t count)
{
    return zread(f,v, sizeof(v[0]) * count);
}

int	zbread_c(ZSTREAM f,char* c)
{
    return zread(f,c,sizeof(*c));
}

int	zbread_cv(ZSTREAM f,char* s,size_t count)
{
    return zread(f,s, sizeof(s[0]) * count);
}

int	zbread_s(ZSTREAM f,char** c)
{
    long l;
    int r;
    *c = NULL;
    if( (r = zbread_l(f,&l)) > 0 ) {
	*c = zpio_malloc(l+1);
	if( !c ) return -1;
	r += zread(f,*c,l);
	(*c)[l] = 0;
	return r;

    } else return -1;
}
