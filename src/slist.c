/*
* File:		slist.c
*
* Id:           $Id: slist.c,v 1.4 2003/06/27 19:04:34 trurl Exp $
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

int	slist_empty(slist* l)
{
    return l->head == 0;
}

slist_item*	slist_new_item(void* item,slist_item* next)
{
    slist_item* a = (slist_item*)zt_calloc(1 , sizeof(slist_item) );
    MDMF_FILL(a,"slim");
    a->item = item;
    a->next = next;
    return a;
}
slist*	slist_new(void)
{
    slist* a = (slist*)zt_calloc(sizeof(slist),1);
    MDMF_FILL(a,"slis");
    a->head = 0;
    return a;
}
int     slist_add(slist* l,void* data)
{
    if( l->head ) {
	l->tail->next = slist_new_item(data,0);
	l->tail = l->tail->next;
    } else
	l->head = l->tail = slist_new_item(data,0);
    return 1;
}

void*   slist_del(slist* l,void* data)
{
    slist_item* a = l->head;
    slist_item* p = 0;
    while ( a ) {
	if( a->item == data ) {
	    if( a == l->head )
		l->head = a->next;
	    else
		p->next = a->next;
	    if( a == l->tail )
		l->tail = p;
	    zt_free(a);
	    return data;
	}
	p = a;
	a = a->next;
    }
    return 0;
}
void    slist_del_all(slist* l,void* data)
{
    slist_item* a = l->head;
    slist_item* p = 0;
    slist_item* next;
    while ( a ) {
	next = a->next;
	if( a->item == data ) {
/*	    if( p == 0 )
		l->head = a->next;
	    else 
		p->next = a->next;
	    zt_free(a);
*/
	    if( a == l->head )
		l->head = a->next;
	    else
		p->next = a->next;
	    if( a == l->tail )
		l->tail = p;
	    zt_free(a);

	} else
	    p = a;
	a = next;
    }
}
sli	slist_first(slist* l)
{
    sli a = (sli)zt_calloc(sizeof(*a),1);
    a->list = l;
    a->item = l->head;
    return a;
}
sli	slist_next(sli i) 
{
    i->item = i->item->next;
    return i;
}

int	sli_end(sli i)
{
    if( i->item == 0 ) {
	return 1;
    }
    return 0;
}

int	slist_delete(slist* bt)
{
    slist_item* a,*b;
    if( bt ) {
	a = bt->head;
	while( a ) {
	    b = a->next;
	    zt_free(a);
	    a = b;
	}
	bt->head = 0;
	zt_free(bt);
    }
    return 1;
}

sli	slist_begin(slist* l,sli_t* a)
{
    if( !a )
	a = (sli)zt_calloc(sizeof(*a),1);
    a->list = l;
    a->item = l->head;
    return a;
}
sli	slist_end(slist* l,sli_t* a)
{
    if( !a )
	a = (sli)zt_calloc(sizeof(*a),1);
    a->list = l;
    a->item = l->tail;
    return a;
}
sli	sli_inc(sli i)
{
    if( i->item )
	i->item = i->item->next;
    return i;
}
sli	sli_dec(sli a)
{
    return 0;
}
int	sli_eq(sli a,sli b)
{
    return a->item == b->item && a->list == b->list;
}
void*	sli_get(sli i)
{
    if( i->item)
	return i->item->item;
    else
	return 0;
}
void	sli_free(sli i)
{
    zt_free(i);
}
