/*
* Header:	zlinkdll.h
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
* $Id: zlinkdll.h,v 1.3 2003/02/26 18:28:43 trurl Exp $
*
*/

#ifndef __zlinkdll_h_
#define __zlinkdll_h_
#define INC_ZLINKDLL
/* Not for use */

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#define DECL_DLL_ALLOC_VECT_FUNC(lib)		\
void lib ## _zlinkdll_set_alloc(	\
void* (*_malloc)(size_t),			\
void* (*_calloc)(size_t,size_t),		\
void* (*_realloc)(void*,size_t),		\
void  (*_free)(void*),				\
char* (*_strdup)(const char* )			\
)

/* Internal library header */
#define DECL_DLL_ALLOC_VECT(lib)		\
extern void* lib ## _malloc(size_t);		\
extern void* lib ## _calloc(size_t,size_t);	\
extern void* lib ## _realloc(void*,size_t);	\
extern void  lib ## _free(void*);		\
extern char* lib ## _strdup(const char* );	\


#define DEF_DLL_ALLOC_VECT_FUNC(lib)		\
ZEXPORT void* (*lib ## _malloc_ptr)(size_t) = 0;	\
ZEXPORT void* (*lib ## _calloc_ptr)(size_t,size_t) = 0;	\
ZEXPORT void* (*lib ## _realloc_ptr)(void*,size_t) = 0;	\
ZEXPORT void  (*lib ## _free_ptr)(void*) = 0;		\
ZEXPORT char* (*lib ## _strdup_ptr)(const char* ) = 0;	\
ZEXPORT void* lib ## _malloc(size_t size)		\
{						\
    if( lib ## _malloc_ptr )			\
	return lib ## _malloc_ptr(size);	\
    return (void*)malloc(size);			\
}						\
ZEXPORT void* lib ## _calloc(size_t n,size_t size)	\
{						\
    if( lib ## _calloc_ptr )			\
	return lib ## _calloc_ptr(n,size);	\
    return calloc(n,size);			\
}						\
ZEXPORT void* lib ## _realloc(void* p,size_t s)		\
{						\
    if( lib ## _realloc_ptr )			\
	return lib ## _realloc_ptr(p,s);	\
    return realloc(p,s);			\
}						\
ZEXPORT void  lib ## _free(void* p)			\
{						\
    if( lib ## _free_ptr )			\
	lib ## _free_ptr(p);			\
    else					\
	free(p);				\
}						\
ZEXPORT char* lib ## _strdup(const char* p)		\
{						\
    if( lib ## _strdup_ptr )			\
	return lib ## _strdup_ptr(p);		\
    return (char*)strdup(p);			\
}						\
ZEXPORT DECL_DLL_ALLOC_VECT_FUNC(lib)			\
{						\
    lib ## _malloc_ptr = _malloc;		\
    lib ## _calloc_ptr = _calloc;		\
    lib ## _realloc_ptr = _realloc;		\
    lib ## _free_ptr = _free;			\
    lib ## _strdup_ptr = _strdup;		\
    {

#define END_DLL_ALLOC_VECT_FUNC(lib)		\
    }						\
}

/* At start of program that imports symbols from lib */
#define SET_DLL_ALLOC_VECT(lib)		\
    if (1){				\
	char* strdup(const char*);	\
	ZIMPORT DECL_DLL_ALLOC_VECT_FUNC(lib);	\
	lib ## _zlinkdll_set_alloc(	\
	    malloc,calloc,realloc,free,	\
	    strdup);			\
    } else
/* At start of library `me` that imports symbols from `lib` */
#define SET_DLL_ALLOC_VECT_PR(me,lib)	\
    if (1){				\
	DECL_DLL_ALLOC_VECT_FUNC(lib);	\
	lib ## _zlinkdll_set_alloc(	\
	    me ## _malloc,		\
	    me ## _calloc,		\
	    me ## _realloc,		\
	    me ## _free,		\
	    me ## _strdup);		\
    } else

#endif /* force single include of file */

