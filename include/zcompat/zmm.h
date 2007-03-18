/*
* File:		zcompat/zmm.h
*
* Id:           $Id$
*
* Project:	ZCompat Memory Management
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*   This program is Copyright(c) Zbigniew Zagorski 2004, 
*   All rights reserved, and is distributed as free software under the 
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifndef __zcompat_zmm_h_
#define __zcompat_zmm_h_

#define INC_ZMM


#ifndef INC_ZCOMPAT
#include "zcompat/zcompat.h"
#endif

struct zmm_allocator;
struct zmm_allocator_vt;

typedef struct zmm_allocator zmm_allocator;

typedef struct zmm_allocator_vt zmm_allocator_vt;

struct zmm_chunk_info {
    zmm_allocator*  owner;
    size_t          size;
    const char*     dbg_file;
    int             dbg_line;
};

struct zmm_allocator_vt {
    void    (*vt_delete)(void* al);
    void*   (*vt_alloc)(void* al, size_t size,const char* file,int line);
    void*   (*vt_realloc)(void* al, void* old_ptr,size_t size,const char* file,int line);
    void    (*vt_free)(void* al, void* ptr);
    int     (*vt_info)(void* al, void* ptr,struct zmm_chunk_info* dest);
    void    (*vt_stats)(void* al, ZSTREAM s, int flags);
};

typedef	void    (*zmm_vt_delete)(void* al);
typedef	void*   (*zmm_vt_alloc)(void* al, size_t size,const char* file,int line);
typedef	void*   (*zmm_vt_realloc)(void* al, void* old_ptr,size_t size,const char* file,int line);
typedef	void    (*zmm_vt_free)(void* al, void* ptr);
typedef	int     (*zmm_vt_info)(void* al, void* ptr,struct zmm_chunk_info* dest);
typedef	void    (*zmm_vt_stats)(void* al, ZSTREAM s, int flags);

struct zmm_allocator {
    struct zmm_allocator_vt*   vt;
};

ZCEXPORT void*      zmm_alloc(zmm_allocator* al, size_t size);
ZCEXPORT void*      zmm_alloc_dbg(zmm_allocator* al, size_t size,const char* file,int line);

ZCEXPORT void*      zmm_realloc(zmm_allocator* al, void* old_ptr,size_t size);
ZCEXPORT void*      zmm_realloc_dbg(zmm_allocator* al, void* old_ptr,size_t size,const char* file,int line);

ZCEXPORT void       zmm_free(zmm_allocator* al,void* ptr);

ZCEXPORT char*    zmm_strdup(zmm_allocator* al, const char* s);

ZCEXPORT int        zmm_info(zmm_allocator* al, void* ptr,struct zmm_chunk_info* dest);
ZCEXPORT void    zmm_stats(struct zmm_allocator* al,ZSTREAM s,int flags);

/* delete the allocator */
ZCEXPORT void    zmm_delete(zmm_allocator* al);

/* 
    create allocator for fixed chunks thet will be alloced and disposed 
    frequently.
*/
ZCEXPORT zmm_allocator*     zmm_new_constsize_allocator(size_t size);

/* 
    create allocator for memory which will be allocated incrementaly
    without need to deallocate, and free the whole memory
    at once at the end.
*/

ZCEXPORT zmm_allocator*     zmm_new_incremental_allocator(size_t size);

#endif
