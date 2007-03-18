/*
* File:		sstack.c
*
* Id:           $Id: sstack.c,v 1.3 2003/06/27 19:04:34 trurl Exp $
*
* Project:	ztools
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*   This program is Copyright(c) Zbigniew Zagorski 1999-2001, 
*   All rights reserved, and is distributed as free software under the 
*   license in the file "LICENSE", which is included in the distribution.
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "zcompat/ztools.h"
#include "ztools_i.h"

sstack* sstack_new(size_t count)
{
    sstack* a = (sstack*)zt_calloc(1,sizeof(sstack));
    if( a == NULL )
	return NULL;
    a->buf.t = NULL;
    a->buf.size = 0;
    if( count > 0 )
	if( sbuffer_set_min_size(&a->buf,count) < 0 ) {
	    zt_free(a);
	    return NULL;
	}
    a->top = a->buf.t;
    a->ptr = 0;
    return a;
}
void	sstack_delete(sstack* s)
{
    if( s != NULL ) {
	if( s->buf.t != NULL )
	    zt_free(s->buf.t);
	zt_free(s);
    }
}
void	sstack_free(sstack* s)
{
    sstack_delete(s);
}

void	sstack_push(sstack* s,void* ptr,size_t size)
{
    if( s == NULL )
	return;
    if( sbuffer_set_min_size(&s->buf,s->ptr+size) < 0 )
	return;
    if( ptr != NULL )
	memcpy(s->buf.t+s->ptr,ptr,size);
    s->ptr += size;
    s->top = s->buf.t + s->ptr;
}
void	sstack_pop(sstack* s,void* ptr,size_t size)
{
    if( s == NULL )
	return;
    if( size > s->ptr )
	size = s->ptr;
    if( ptr != NULL )
	memcpy(ptr, (char*) ((int)(s->buf.t)+(int)(s->ptr-size)) ,size);
    s->ptr-=size;
    s->top-=size;
    if( s->ptr < ( s->buf.size / 2 ) ) {
	sbuffer_resize(& s->buf, ( s->buf.size / 2 ) );
    }
}
#undef sstack_empty
int	sstack_empty(sstack* s)
{
    return s ? ( s->ptr == 0 ) : 1;
}

#undef sstack_left
int	sstack_left(sstack* s)
{
    return s ? s->ptr : 0;
}
