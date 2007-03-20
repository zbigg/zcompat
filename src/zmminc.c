/*
* File:		zmminc.c
*
* Id:           $Id$
*
* Project:	zcompat - zmm
*
* Description:  zmm - incremental memory manager
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*   This program is Copyright(c) Zbigniew Zagorski 2004, 
*   All rights reserved, and is distributed as free software under the 
*   license in the file "LICENSE", which is included in the distribution.
*/

#include "zcompat/zcompat.h"
#include "zcompat/zmm.h"
#include "zcompat/zpio.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*############################################################
 *#
 *#    zmm_incremental_allocator
 */
struct ia_buffer {
    struct ia_buffer*	next;
    char*	current;
    size_t	size;
    size_t	remaining;
};

struct zmm_incremental_allocator {
    struct zmm_allocator_vt*   vt;
    size_t      chunk_size;
    struct ia_buffer        *first,
			    *first_free,
			    *last;
    int ac,cc;
    
};

#define     BUFCOUNT    (sizeof(int)*8)

static void*    incremental_alloc(struct zmm_incremental_allocator* al, size_t size,const char* file,int line);
static void*    incremental_realloc(struct zmm_incremental_allocator* al, void* old_ptr,size_t size,const char* file,int line);
static void     incremental_free(struct zmm_incremental_allocator* al, void* ptr);
static int      incremental_info(struct zmm_incremental_allocator* al, void* ptr,struct zmm_chunk_info* dest);
static void	incremental_delete(struct zmm_incremental_allocator* al);
static void	incremental_stats(struct zmm_incremental_allocator* al,ZSTREAM s,int flags);

static zmm_allocator_vt incremental_vt = {
    (zmm_vt_delete)incremental_delete,
    (zmm_vt_alloc)incremental_alloc,
    (zmm_vt_realloc)incremental_realloc,
    (zmm_vt_free)incremental_free,
    (zmm_vt_info)incremental_info,
    (zmm_vt_stats)incremental_stats
};

static struct ia_buffer* new_ia_buffer(size_t size) {
    struct ia_buffer* a = malloc( sizeof(struct ia_buffer) + size + 4);
    a->next = NULL;
    a->current = ((char*)a) + sizeof( struct ia_buffer ) ;
    a->remaining = size;
    a->size = size;
    return a;
}

/*##
    =Function zmm_new_incremental_allocator
	create incremental allocator

    =Synopsis
    |#include <zcompat/zmm.h>
    |zmm_allocator*  zmm_new_incremental_allocator(size_t size);

    =Description
    
    Create allocator for memory which will be allocated incrementaly
    without need to deallocate, and free the whole memory
    at once at the end.
    
    <size> is default initial and grow size for allocator.
*/

zmm_allocator*  zmm_new_incremental_allocator(size_t size) {

    struct zmm_incremental_allocator* x = malloc(sizeof(struct zmm_incremental_allocator));
    memset(x,0,sizeof(*x));
    x->chunk_size = size;
    x->vt = & incremental_vt;

    return (zmm_allocator*)x;
};


static void*   incremental_alloc(struct zmm_incremental_allocator* al, size_t size,const char* file,int line)
{
    struct ia_buffer* i = al->first_free;
    int f = 1;
    al->ac++;

    while( i ) {
	 al->cc++;
         if( i->remaining >= size ) {
	    void* p = i->current;
	    i->current += size;
	    i->remaining -= size;
	    if( i->remaining < (al->chunk_size >> 8) ) {
		al->first_free = i->next;
		while( al->first_free )
		    if( al->first_free->remaining > (al->chunk_size >> 8 ) )
			break;
		    else
			al->first_free = al->first_free->next;
	    }
	    return p;
	 }
	 i = i->next;
	 if( !i && f ) {
	    i = al->first;
	    f = 0;
	 } else
	    if( !f && i == al->first_free )
		break;
    }
    al->cc++;
    // i = null
    if( al->chunk_size == 0 )
	al->chunk_size = 8192 - sizeof(struct ia_buffer);
    if( size > al->chunk_size )
	i = new_ia_buffer(size*2);
    else
	i = new_ia_buffer(al->chunk_size);
	
    if( !al->first ) {
	al->first = al->first_free = al->last = i;
    } else {
	al->last->next = i;
	al->last = i;
	if( al->first_free == NULL )
	    al->first_free = i;
    }
    
    if( i == NULL )
	return NULL;
    {
	void* p = i->current;
	i->current += size;
	i->remaining -= size;
	return p;
    }
}
static void*   incremental_realloc(struct zmm_incremental_allocator* al, void* old_ptr,size_t size,const char* file,int line)
{
    if( old_ptr == NULL )
        return incremental_alloc(al,size,file,line);
    if( old_ptr && size == 0 ) {
        incremental_free(al,old_ptr);
        return NULL;
    }
    // cannot be done !
    return NULL;

}
static void   incremental_free(struct zmm_incremental_allocator* al, void* ptr)
{
    // nothing is done ... !
    return;
}
static int     incremental_info(struct zmm_incremental_allocator* al, void* ptr,struct zmm_chunk_info* dest)
{
    dest->dbg_file = NULL;
    dest->dbg_line = 0;
    dest->owner = (zmm_allocator*)al;
    dest->size = 0;
    return 0;
}

static void	incremental_delete(struct zmm_incremental_allocator* al)
{
    struct ia_buffer* a = al->first,*b;
    while( a ) {
	b = a->next;
	free(a);
	a = b;
    }
    free(al);
}

static void incremental_stats(struct zmm_incremental_allocator* al,ZSTREAM s,int flags)
{
    struct ia_buffer* a = al->first;
    int ba = 0;
    int bu = 0;
    int i = 0;
    zfprintf(s, "incremental allocator 0x%08x info ...\n",al);
    while(a) {
	int u = a->size-a->remaining;
	int sz = a->size;
	zfprintf(s,"block % 3i: size % 8i used %i (%i%%) %s\n", 
		i,sz,
		u,
		(int)(u/((double)sz)*100.0),
		a == al->first_free ? "first free" : "" 
		);
	ba += sz;
	bu += u;
	i++;
	a = a->next;
    }
    zfprintf(s,
	"default chunk size: %i\n"
	"blocks count:       %i\n"
	"alloc count:        %i\n"
	"avg. search steps:  %02f\n"
	"bytes allocated:    %i\n"
	"bytes really used:  %i (%i%%)\n", al->chunk_size, i, al->ac, al->cc/(double)al->ac, ba, bu, (int)((bu/(float)ba)*100.0));
}
