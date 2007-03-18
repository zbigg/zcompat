/*
* File: new_filter.c
*
* Id:           $Id: filter.template.c,v 1.4 2003/06/28 11:57:44 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  new_filter Filter ...
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2000,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/
#include "zcompat/zpio.h"

ZSTREAM znew_filter(ZSTREAM s,unsigned long key);


int	znew_filter_read	(ZSTREAM f,void* buf,size_t size);
int	znew_filter_write	(ZSTREAM f,const void* buf,size_t size);
#define zxor_key	f->data[0]

ZSTREAM znew_filter(ZSTREAM f,unsigned long key)
{
    ZSTREAM a = zfilter_creat(znew_filter_read,znew_filter_write,1);
    if( a == NULL )
	return NULL;
    a->vtable->vt_seek = NULL;
    a->parent = f;
    a->data[0] = key;
    return a;
}

int	znew_filter_read	(ZSTREAM f,void* buf,size_t size)
{
    int e = zread(f->parent,buf,size);
    if( e < 0 )
	return e;
    else  {
	/* do something with input data */
    }
    return e;
}

int	znew_filter_write	(ZSTREAM f,const void* buf,size_t size)
{
    /* do something with output data */
    return zwrite(f->parent,buf,size);
}

