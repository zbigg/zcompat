/*
* File:		zmm.c
*
* Id:           $Id$
*
* Project:	ztools
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
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
/*##

    =Module ZMM Memory Menager
    
    memory manager based on allocaters (sometimes called pools)
*/
/* ############################################################
##  =Function zmm_alloc
##      allocate memory
##
##  =Synopsis
##  |void*   zmm_alloc(zmm_allocator* al, size_t size);
##  |void*   zmm_alloc_dbg(zmm_allocator* al, size_t size,const char* file,int line);
##
##  =Description
##      TODO: write documentation
##  =Return values
##      TODO: write documentation
##
*/
void*   zmm_alloc(zmm_allocator* al, size_t size)
{
    if( al && al->vt && al->vt->vt_alloc )
        return al->vt->vt_alloc(al,size,NULL,0);
    return NULL;
}
void*   zmm_alloc_dbg(zmm_allocator* al, size_t size,const char* file,int line)
{
    if( al && al->vt && al->vt->vt_alloc )
        return al->vt->vt_alloc(al,size,file,line);
    return NULL;
}

/* ############################################################
##  =Function zmm_realloc
##      short description of zmm_realloc
##
##  =Synopsis
##  |void*   zmm_realloc(zmm_allocator* al, void* old_ptr,size_t size);
##  |void*   zmm_realloc_dbg(zmm_allocator* al, void* old_ptr,size_t size,const char* file,int line);
##
##  =Description
##      TODO: write documentation
##  =Return values
##      TODO: write documentation
##
*/
void*   zmm_realloc(zmm_allocator* al, void* old_ptr,size_t size)
{
    if( al && al->vt && al->vt->vt_realloc )
        return al->vt->vt_realloc(al,old_ptr,size,NULL,size);
    return NULL;
}
void*   zmm_realloc_dbg(zmm_allocator* al, void* old_ptr,size_t size,const char* file,int line)
{
    if( al && al->vt && al->vt->vt_realloc )
        return al->vt->vt_realloc(al,old_ptr,size,file,line);
    return NULL;
}
/* ############################################################
##
##  =Function zmm_free
##      short description of zmm_free
##
##  =Synopsis
##  |void   zmm_free(zmm_allocator* al,void* ptr);
##
##  =Description
##      TODO: write documentation
##  =Return values
##      TODO: write documentation
##
*/
void   zmm_free(zmm_allocator* al,void* ptr)
{
    if( al && al->vt && al->vt->vt_free )
        al->vt->vt_free(al,ptr);
}
/* ############################################################
##  =Function zmm_info
##      get information about alocated memory
##
##  =Synopsis
##  |int     zmm_info(zmm_allocator* al, void* ptr,struct zmm_chunk_info* chunk_info);
##
##  =Description
##      Get information about <ptr> owned by allocator <al>.
##
##      struct zmm_chunk_info has the following structure:
##      |struct zmm_chunk_info {
##      |    zmm_allocator*  owner;
##      |    size_t          size;
##      |    const char*     dbg_file;
##      |    int             dbg_line;
##      |};
##
##      If some of information is not available, then field is set to 0 or
##      NULL for pointers.
##
##  =Return values
##      On success <zmm_info> returns 0 and fills <chunk_info> fields with 
##      all available information about <ptr>.
##      On error it returns -1 and sets errno to EINVAL.
*/
int     zmm_info(zmm_allocator* al, void* ptr,struct zmm_chunk_info* dest)
{
    if( al && al->vt && al->vt->vt_info )
        return al->vt->vt_info(al,ptr,dest);
    errno = EINVAL;
    return -1;
}

/*##
    =Function zmm_delete
	delete allocator and all it's buffers
	
    =Syopsis
    |#include <zcompat/zmm.h>
    |void	zmm_delete(zmm_allocator* al);
    
    =Description

    Delete allocator and all it's buffers.
    
    After calling this on allocator, all buffers that
    were allocated within it will be deleted.
*/
void	zmm_delete(zmm_allocator* al) 
{
    if( al && al->vt && al->vt->vt_delete )
	al->vt->vt_delete(al);
}

void zmm_stats(struct zmm_allocator* al,ZSTREAM s,int flags) 
{
    if( al && al->vt && al->vt->vt_stats )
	al->vt->vt_stats(al,s,flags);
}

char*	zmm_strdup(zmm_allocator* al, const char* s)
{
    size_t l = strlen(s);
    char* a = zmm_alloc(al,l+1);
    memcpy(a,s,l+1);
    return a;
}
