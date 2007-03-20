/*
* File:		sbuffer.c
*
* Id:           $Id: sbuffer.c,v 1.4 2003/06/27 19:04:34 trurl Exp $
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


#include "zcompat/zcompat.h"
#include "zcompat/ztools.h"
#include "ztools_i.h"

sbuffer*	sbuffer_init(sbuffer* a,unsigned int size)
{
    if( a == NULL )
	a = (sbuffer*) zt_calloc(1,sizeof(sbuffer));
    if( a == NULL )
	return NULL;
    MDMF_FILL(a,"sbuf");

    a->t = NULL;
    a->size = 0;
    if( size > 0 && sbuffer_resize(a,size) < 0) {
	zt_free(a);
	return NULL;
    } else {
	return a;
    }
}
void		sbuffer_done(sbuffer* a)
{
    if( a->t != NULL )
	zt_free(a->t);
    a->t = 0;
}

sbuffer* sbuffer_new(unsigned int size)
{
    return sbuffer_init(NULL,size);
}
void sbuffer_free(sbuffer* b)
{
    if( b != NULL ) {
	sbuffer_done(b);
	zt_free(b);
    }
}

int	sbuffer_resize(sbuffer* b,unsigned int new_size)
{
    if( b->t == NULL ) {
	b->t = (char*)zt_malloc(new_size);
	if( b->t == NULL ) 
	    return -1;
	memset(b->t,'\0',new_size);
	b->size = new_size;
	return 0;
    } else if( b->t != NULL && b->size != new_size ) {
	char* x;
	x = zt_realloc(b->t,new_size);
	if( x == NULL && new_size > 0)
	    return -1;
	if( new_size > b->size )
	    memset(x + b->size , '\0' , new_size - b->size );
	b->t = x;
	b->size = new_size;
	return 0;
    }
    return -1;
}
int	sbuffer_set_min_size(sbuffer* b,unsigned int min_size)
{
    return b->size < min_size ? sbuffer_resize(b,min_size) : 0;
}
void	sbuffer_delete(sbuffer* b)
{
    sbuffer_free(b);
}

