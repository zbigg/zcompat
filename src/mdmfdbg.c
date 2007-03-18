/*
* File:		smap.c
*
* Id:           $Id: smap.c,v 1.4 2003/06/27 19:04:34 trurl Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zcompat/zcompat.h"

#include "zcompat/ztools.h"
#include "zcompat/zpio.h"
void	mdmf_stats(void);
void	mdmf_fstats(FILE*);
void	mdmf_zfstats(ZSTREAM s);
void*   mdmf_malloc(size_t size);
void*   mdmf_calloc(size_t n,size_t size);
void*   mdmf_realloc(void* p,size_t s);
void    mdmf_free(void* p);
char*   mdmf_strdup(const char* p);



unsigned alloc_active = 0;
unsigned alloc_count = 0;

#undef malloc
#undef calloc
#undef realloc
#undef free
#undef strdup

static void	report_active_ptrs(ZSTREAM s,const char* prefix);
static void free_ptr_map(void);
void	mdmf_stats(void)
{
    mdmf_zfstats(zstderr);
}
void	mdmf_fstats(FILE* f)
{
    ZSTREAM x = zansiopen(f,"statstream");
    mdmf_zfstats(x);
    zclose(x);
}
void	mdmf_zfstats(ZSTREAM s)
{
    if( alloc_active == 0 ) return;
    report_active_ptrs(s,"MDMF:");
    free_ptr_map();
    zfprintf(s,
	"MDMF: Memory report\n"
        "MDMF:   axl allocation stats:\n"
	"MDMF:     total allocation operations:  %i\n"
	"MDMF:     currently allocated blocks:	%i\n"
	,alloc_count,alloc_active);
}

#define PALLOC(d,s,p)       zprintf("ALLOC   (%s): %i = 0x%08x\n",d,s,p)
#define PFREE(d,s,p)	    zprintf("FREE    (%s): %i   0x%08x\n",d,s,p)

smap* ptr_map = NULL;
int ptr_map_lock = 0;
#define PTR_LOCK()  ptr_map_lock++
#define PTR_UNLOCK() ptr_map_lock--

static void free_ptr_map(void)
{
    if( ptr_map ) {
	PTR_LOCK();
	smap_free(ptr_map);
	PTR_UNLOCK();
    }
    ptr_map_lock = 1;
}


static void init_ptr_map()
{
    if( ptr_map ) return;

    ptr_map = smap_new(NULL,NULL,NULL);
#ifndef MDMF_DEBUG
    zc_atexit(free_ptr_map);
#else
    zc_atexit(mdmf_stats);
#endif
}

static void	add_ptr(void* ptr,size_t size)
{
    if( ptr_map_lock ) return;
    PTR_LOCK();
    init_ptr_map();
    smap_add(ptr_map,ptr, (void*)size);
    alloc_active ++;
    PTR_UNLOCK();
}
static size_t	del_ptr(void* ptr)
{
    size_t x;
    if( ptr_map_lock ) return 0;
    PTR_LOCK();
    x = (size_t)smap_del(ptr_map,ptr);
    alloc_active --;
    PTR_UNLOCK();
    return x;
}

static size_t	ptr_size(void* ptr)
{
    return (size_t)smap_find(ptr_map,ptr);
}
void*   mdmf_malloc(size_t size)
{
    void* p = malloc(size);
    alloc_count++;
    add_ptr(p,size);
    memset(p,0,zmin(5,size));
    return p;
}
void*   mdmf_calloc(size_t n,size_t size)
{
    void* p = calloc(n,size);
    alloc_count++;
    add_ptr(p,size*n);
    return p;
}
void*   mdmf_realloc(void* p,size_t s)
{
    void* x;
    size_t ns = ptr_size(p);
    if( ns == 0 && p != NULL )
	zprintf("PTR: warning doing realloc on unknown pointer 0x%08x\n",p);

    x = realloc(p,s);
    if( x != NULL && p == NULL ) {
	add_ptr(x,s);
	memset(x,0,zmin(5,s));
    } else
    if( x == NULL && p != NULL ) {
	del_ptr(p);
    } else {
	del_ptr(p);
	add_ptr(x,s);
    }

    alloc_count++;
    return x;
}
void    mdmf_free(void* p)
{
    if( !ptr_map_lock ) {
	size_t ns = ptr_size(p);
	if( ns == 0 )
	    zprintf("PTR: warning doing free on unknown pointer 0x%08x\n",p);
    }
    del_ptr(p);
    free(p);
}
char*   mdmf_strdup(const char* p)
{
    char* x = strdup(p);
    int size = strlen(p)+1;
    alloc_count++;
    add_ptr(x,size);
    return x;
}

static void	report_active_ptrs(ZSTREAM s,const char* prefix)
{
    int i = 0;
    prefix = prefix ? prefix : "";
    if( !ptr_map || alloc_active == 0 ) return ;

    zfprintf(s,"%s active allocated blocks report: begin\n",prefix);
    SMAP_ENUM_BEGIN (ptr_map,size,ptr,void*,size_t) {
        zfprintf(s,"%s buffer at 0x%08x size %i\n'",prefix,ptr,size);
	i++;
    } SMAP_ENUM_END
    zfprintf(s,"%s counted %i blocks (should be %i)\n",i,alloc_active);
    zfprintf(s,"%s active allocated blocks report: end\n",prefix);
}
