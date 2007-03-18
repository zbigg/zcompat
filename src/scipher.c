/*
* File: scipher.c
*
* Id:           $Id: scipher.c,v 1.3 2003/02/26 18:28:40 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  simple XORcipher
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

#include <string.h>

ZSTREAM zcipher_xor(ZSTREAM s,unsigned long key);


static int	zcipher_xor_read	(ZSTREAM f,void* buf,size_t size);
static int	zcipher_xor_write	(ZSTREAM f,const void* buf,size_t size);
#define zxor_key	f->data[0]

ZSTREAM zcipher_xor(ZSTREAM f,unsigned long key)
{
    ZSTREAM a = zfilter_creat(f,zcipher_xor_read,zcipher_xor_write,1);
    if( a == NULL )
	return NULL;
    a->vtable->vt_seek = NULL;
    a->data[0] = key;
    return a;
}

static void	do_xor(unsigned long key,char* buf,size_t size);

static int	zcipher_xor_read	(ZSTREAM f,void* buf,size_t size)
{
    int e = zread(f->parent,buf,size);
    if( e < 0 )
	return e;
    else
	do_xor(f->data[0],buf,e);
    return e;
}
static int	zcipher_xor_write	(ZSTREAM f,const void* buf,size_t size)
{
    size_t n = 0;
    while( n < size ) {
	char szbuf[200];
	int now = size-n < sizeof(szbuf) ? size-n : sizeof(szbuf);
	int r;
	memcpy(szbuf,(char*)buf + n,now);
	do_xor(f->data[0],szbuf,now);
	r = zwrite(f->parent,szbuf,now);
	if( r < 0 )
	    return r;
	n+=now;
    }
    return n;
}

static void	do_xor(unsigned long key,char* buf,size_t size)
{
    size_t i;
    for(i=0;i<size;i++)
	buf[i] ^= (unsigned char)key;
}
