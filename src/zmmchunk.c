/*
* File:		zmmchunk.c
*
* Id:           $Id$
*
* Project:	zcompat - zmm
*
* Description:  zmm - chunk memory manager
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
#include "zcompat/ztools.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct buffer_info {
    unsigned int    map;
    void*           buffer;
};

struct zmm_constsize_allocator {
    struct zmm_allocator_vt*   vt;
    size_t      chunk_size;
    sbuffer     memory_map;
};

#define     BUFCOUNT    (sizeof(int)*8)

static void*    const_alloc(struct zmm_constsize_allocator* al, size_t size,const char* file,int line);
static void*    const_realloc(struct zmm_constsize_allocator* al, void* old_ptr,size_t size,const char* file,int line);
static void     const_free(struct zmm_constsize_allocator* al, void* ptr);
static int      const_info(struct zmm_constsize_allocator* al, void* ptr,struct zmm_chunk_info* dest);
static void	const_delete(struct zmm_constsize_allocator* al);
static void	const_stats(struct zmm_constsize_allocator* al,ZSTREAM s,int flags);

static zmm_allocator_vt constsize_vt = {
    (zmm_vt_delete)const_delete,
    (zmm_vt_alloc)const_alloc,
    (zmm_vt_realloc)const_realloc,
    (zmm_vt_free)const_free,
    (zmm_vt_info)const_info,
    (zmm_vt_stats)const_stats
};
/*##
    =Function zmm_new_constsize_allocator
	create constatnt size chunk allocator
	
    =Synopsis
    |#include <zcompat/zmm.h>
    |zmm_allocator*  zmm_new_constsize_allocator(size_t size);
    
    =Description
    Create allocator for fixed chunks thet will be alloced and disposed 
    frequently.
    
    <size> is constant size of chunk that will be returned by
    <zmm_alloc> called on returned allocator.
*/
zmm_allocator*  zmm_new_constsize_allocator(size_t size) {

    struct zmm_constsize_allocator* x = malloc(sizeof(struct zmm_constsize_allocator));
    memset(x,0,sizeof(*x));
    x->vt = & constsize_vt;
    x->chunk_size = size;

    sbuffer_init(&x->memory_map,0);

    return (zmm_allocator*)x;
};

static void*   const_alloc(struct zmm_constsize_allocator* al, size_t size,const char* file,int line)
{
    struct buffer_info*    mm = (struct buffer_info*)al->memory_map.t;
    struct buffer_info*    mmi = mm;
    int             mm_size = al->memory_map.size/sizeof(struct buffer_info);
    int i;
    unsigned int    bits;
    unsigned int    mmap_bits;
    int             bi;
    for( i = 0; i < mm_size; i++, mmi++ ) {
        if( mmi->map == (unsigned int)-1 )
            continue; /* this one is full; proceed to next buffer */
        mmap_bits = mmi->map;
        bits = 1; /* bitmask */
        bi = 0; /* number of buffer in buffer */
        while( bi < BUFCOUNT && (mmap_bits & bits) ) {
            bits = bits << 1;
            ++bi;
        }
        if( bi < BUFCOUNT ) {
            /* we've found a free chunk of buffer at <bi> */
            mmi->map |= bits;   /* mark as used */
            if( !mmi->buffer )
                mmi->buffer = malloc(al->chunk_size * BUFCOUNT);
            return ((char*)mm->buffer) + al->chunk_size * bi;
        }
        /* proceed to next buffer, 
            this should never happen */
    }
    if( i == mm_size ) {
        /* 
            * increase size of memory_map by 2
            * allocate new buffer 
            * return first chunk 
        */
        if( sbuffer_set_min_size(&al->memory_map,
            al->memory_map.size + 2*sizeof(struct buffer_info)) < 0 ) {
            return NULL;
        }
        mmi = (struct buffer_info*)al->memory_map.t;
        mmi += mm_size;
        mmi->buffer = malloc(al->chunk_size * BUFCOUNT);
        mmi->map = 1;
        return mmi->buffer;
    }
    /* we shouldn't get here ! */
    return NULL;
}
static void*   const_realloc(struct zmm_constsize_allocator* al, void* old_ptr,size_t size,const char* file,int line)
{
    if( old_ptr == NULL )
        return const_alloc(al,size,file,line);
    if( old_ptr && size == 0 ) {
        const_free(al,old_ptr);
        return NULL;
    }
    /* do actual resize */
    if( size < al->chunk_size )
        return old_ptr;
    return NULL;

}
static void   const_free(struct zmm_constsize_allocator* al, void* ptr)
{
    struct buffer_info*    mm = (struct buffer_info*)al->memory_map.t;
    struct buffer_info*    mmi = mm;
    int                    mm_size = al->memory_map.size/sizeof(struct buffer_info);

    int i;

    for( i = 0; i < mm_size; i++, mmi++ ) {
        char* a = (char*)mmi->buffer;
        char* b = (char*)mmi->buffer + al->chunk_size * BUFCOUNT;
        if( (char*)ptr >= a && (char*)ptr < b ) {
            /* belongs to current buffer */
            size_t cn = ((char*)ptr-a) / al->chunk_size;
            unsigned int a = 1 << cn;
            mmi->map &= ~ a;
            if( mmi->map == 0 ) {
                /* this one is unused */
                free(mmi->buffer);
                mmi->buffer = NULL;
                if( i < mm_size -1 )
                    /* remove info about this buffer from
                        memory_map
                    */
                    memmove(mmi,mmi+1,(mm_size-i-1)*sizeof(struct buffer_info));
            }
            return;
        }
    }
}
static int     const_info(struct zmm_constsize_allocator* al, void* ptr,struct zmm_chunk_info* dest)
{
    dest->dbg_file = NULL;
    dest->dbg_line = 0;
    dest->owner = (zmm_allocator*)al;
    dest->size = al->chunk_size;
    return 0;
}

static void	const_delete(struct zmm_constsize_allocator* al)
{
    struct buffer_info*    mm = (struct buffer_info*)al->memory_map.t;
    struct buffer_info*    mmi = mm;
    int                    mm_size = al->memory_map.size/sizeof(struct buffer_info);
    int i;
    for(i = 0; i < mm_size; i++,mmi++ )    
	if( mmi->buffer)
	    free(mmi->buffer);
    sbuffer_done(&al->memory_map);
    free(al);
}

static void const_stats(struct zmm_constsize_allocator* al,ZSTREAM s,int flags)
{
    struct buffer_info*    mm = (struct buffer_info*)al->memory_map.t;
    struct buffer_info*    mmi = mm;
    int                    mm_size = al->memory_map.size/sizeof(struct buffer_info);
    int bal = 0;
    int bu = 0;
    int i = 0;
    int x;
    zfprintf(s,"const chunk size  allocator 0x%08x info ...\n",al);
    for(i = 0; i < mm_size && mmi->buffer; i++,mmi++) {
	int na = 0;
	unsigned int ba = 1;
	do { 
	    if( mmi->map & ba )
		na++;
	    ba = ba << 1;
	} while( ba );
	
	
	zfprintf(s,"block % 3i: used %i blocks of size %i(%i%%)\n", 
		i,
		na,
		na * al->chunk_size,
		(int) ((na * al->chunk_size) / (double)( al->chunk_size * BUFCOUNT) * 100.0) );
	bal += al->chunk_size * BUFCOUNT;
	bu += na * al->chunk_size;
    }
    x = (int)(100.0 * (double)bu/bal);
    zfprintf(s,
	"chunk size:        %i\n"
	"chunks count:      %i\n"
	"bytes allocated:   %i\n"
	"bytes really used: %i (%i%%)\n", 
	    al->chunk_size,
	    i,
	    bal,bu, x);
}
