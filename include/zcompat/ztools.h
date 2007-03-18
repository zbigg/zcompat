/*
* Header:	ztools.h
*
* Project:	ZCompat Containers Library
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*   This program is Copyright(c) Zbigniew Zagorski 2002,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifndef __ztools_h_
#define __ztools_h_

#define INC_ZTOOLS

#include <stddef.h>
#include "zdefs.h"
#ifdef __cplusplus
extern  "C" {
#endif

typedef int   (*zt_cmp_f)(const void* a,const void* b);
typedef void  (*zt_del_f)(void* a);
typedef void* (*zt_dup_f)(const void* a);
typedef void* (*zt_init_f)(void* a,const void* b);


struct ztools_obj_desc {
    unsigned size;
    int   (*cmp_f)(const void* a,const void* b);
    void  (*del_f)(void* a);
    void* (*dup_f)(const void* a);
    void* (*init_f)(void* a,const void* b);
};

typedef struct ztools_obj_desc ztools_obj_desc;

extern ztools_obj_desc ztools_int_desc;
extern ztools_obj_desc ztools_charp_desc;


ZCEXPORT  void    ztools_free(void*);
ZCEXPORT  void*    ztools_malloc(size_t size);
ZCEXPORT  void*    ztools_calloc(size_t nmemb, size_t size);

/****************************************************/
/*
    TURN ON/OFF memory debugging 
        (tracking memory allocation )
*/
/****************************************************/
#if 0 && !defined(NDEBUG)
#define MDMF_DEBUG
#endif
/****************************************************/

#ifdef MDMF_DEBUG
#define MDMF_DEF	char _mdmf_signature[5];
#define MDMF_FILL(a,sig)    memcpy((a)->_mdmf_signature,(sig),5)
#define MDMF_INIT(a)	a,
#else
#define MDMF_DEF	
#define MDMF_FILL(a,sig)    
#define MDMF_INIT(a)
#endif
ZCEXPORT void    mdmf_stats(void);
/*void	mdmf_fstats(FILE*);*/
/*void	mdmf_zfstats(ZSTREAM s);*/
ZCEXPORT void*      mdmf_malloc(size_t size);
ZCEXPORT void*      mdmf_calloc(size_t n,size_t size);
ZCEXPORT void*      mdmf_realloc(void* p,size_t s);
ZCEXPORT void       mdmf_free(void* p)	;
ZCEXPORT char*      mdmf_strdup(const char* p);

#ifndef __smap_h_
#define __smap_h_

#define INC_SMAP


struct smap_item {
	MDMF_DEF
	void*			key;
	int			weight;
	void*			item;
	struct smap_item
				*left,
				*right,
				*up;
};
typedef struct smap_item smap_item;

typedef int   (*smap_cmp_f)(const void* a,const void* b);
typedef void  (*smap_del_f)(void* a);
typedef void* (*smap_dup_f)(const void* a);

struct smap_t {
	MDMF_DEF
        int	(*cmp)(const void* a,const void* b);
        void	(*del)(void* a);
        void*	(*dup)(const void* a);
	void    (*item_del)(void* a);

	smap_item* head;
	smap_item* leftmost;
	smap_item* rightmost;
};
typedef struct smap_t smap;

struct smap_iterator {
    MDMF_DEF
    smap_item*   item;
    smap*	 tree;
};

typedef struct smap_iterator	smapi_t;

typedef smapi_t	*smapi;

ZCEXPORT smap*    smap_new(smap_cmp_f cmp_f,smap_del_f del_f,smap_dup_f dup_f);
ZCEXPORT void    smap_free(smap* m);

ZCEXPORT smap*    smap_init(smap* m,smap_cmp_f cmp_f,smap_del_f del_f,smap_dup_f dup_f);
ZCEXPORT void    smap_done(smap* m);	/* equal */
ZCEXPORT void    smap_frees(smap* m);    /* equal */

ZCEXPORT int    smap_delete(smap* m);

ZCEXPORT int    smap_add(smap* m,const void* key,void* data);
ZCEXPORT smapi    smap_add_i(smap* m,const void* key,void* data,smapi i);

ZCEXPORT int    smap_set(smap* m,const void* key,void* data);
ZCEXPORT smapi    smap_set_i(smap* m,const void* key,void* data,smapi i);

ZCEXPORT void*    smap_del(smap* bt,const void* key);
ZCEXPORT smapi    smap_del_i(smap* bt,smapi i);

ZCEXPORT void*    smap_find(smap* bt,const void* key);
ZCEXPORT smapi    smap_find_i(smap* bt,const void* key,smapi i);

ZCEXPORT void*      smap_find_cust(smap* bt, const void* cust_key, zt_cmp_f cust_cmp_f);
ZCEXPORT smapi      smap_find_cust_i(smap* bt, const void* cust_key, zt_cmp_f cust_cmp_f,smapi i);

ZCEXPORT smapi    smap_begin(smap* bt,smapi i);
ZCEXPORT smapi    smap_end(smap* bt,smapi i);

ZCEXPORT void    smapi_free(smapi i);

ZCEXPORT smapi    smapi_inc(smapi i);
ZCEXPORT smapi    smapi_dec(smapi i);
ZCEXPORT int    smapi_eq(smapi a,smapi b);

ZCEXPORT void*    smapi_get(smapi i);
ZCEXPORT void       smapi_set(smapi i, void* new_item);
ZCEXPORT const void*    smapi_key(smapi i);

#define SMAP_START_ENUM(tree,i,k,ktype,itype)	\
    {						\
	smapi_t sbt_e_begin,sbt_e_end;		\
	itype i; 				\
	ktype k;				\
	smap_begin((tree),&sbt_e_begin);	\
	smap_end((tree),&sbt_e_end);		\
	smapi_inc(&sbt_e_end);			\
	for(; !smapi_eq(&sbt_e_begin,&sbt_e_end); \
	      smapi_inc(&sbt_e_begin) ) {	\
	    i = (itype) smapi_get(&sbt_e_begin);	\
	    k = (ktype) smapi_key(&sbt_e_begin); {

#define SMAP_STOP_ENUM }}}

#define SMAP_ENUM_BEGIN SMAP_START_ENUM
#define SMAP_ENUM_END	SMAP_STOP_ENUM


#endif

#ifndef __sbuffer_h_
#define __sbuffer_h_

#define INC_SBUFFER

struct sbuffer {
    MDMF_DEF
    char*	t;
    size_t	size;
};
typedef struct sbuffer sbuffer;

/* Static buffer structures */
ZCEXPORT sbuffer*    sbuffer_init(sbuffer* buf,unsigned int size);
ZCEXPORT void	    sbuffer_done(sbuffer* buf);

/* Dynamic buffers */
ZCEXPORT sbuffer*    sbuffer_new(unsigned int size);
ZCEXPORT void	    sbuffer_free(sbuffer* b);

/** Resize buffer.

    Newly allocated memory is filled with zeros.

    Returns -1 on error, 0 on success.
*/
ZCEXPORT int	    sbuffer_resize(sbuffer* b,unsigned int new_size);

/** Request specified size.
    Check if size of buffer is sufficent and if not
    allocate memory.

    Newly allocated memory is filled with zeros.
    Returns -1 on error, 0 on success.
*/
ZCEXPORT int	    sbuffer_set_min_size(sbuffer* b,unsigned int min_size);

/** obsolete */
ZCEXPORT void	    sbuffer_delete(sbuffer* bt);

#define     DEFINE_SBUFFER_TYPE(T,C)            \
typedef sbuffer C;                              \
C*      C##_new(size_t size);                   \
void    C##_free(C* b);                         \
int     C##_set_size(C* b, size_t size);        \
size_t  C##_capacity(C* b);                     \
T*      C##_geti(C* b,int i);                   \
T*      C##_getp(C* b);                         \
int     C##_set_min_size(C* b,size_t size);

#define     IMPLEMENT_SBUFFER_TYPE(T,C)         \
C*       C##_new(size_t size)                   \
{                                               \
    return sbuffer_new(size*sizeof(T));         \
}                                               \
                                                \
void    C##_free(C* b)                          \
{                                               \
    sbuffer_free(b);                            \
}                                               \
                                                \
int     C##_set_size(C* b, size_t size)         \
{                                               \
    return sbuffer_resize(b,size*sizeof(T));    \
}                                               \
                                                \
size_t  C##_capacity(C* b)                      \
{                                               \
    return b->size / sizeof(T);                 \
}                                               \
                                                \
T*      C##_geti(C* b,int i)                    \
{                                               \
    return ((T*)(b->t))+i;                      \
}                                               \
                                                \
T*      C##_getp(C* b)                          \
{                                               \
    return (T*)(b->t);                          \
}                                               \
                                                \
int     C##_set_min_size(C* b,size_t size)      \
{                                               \
    return sbuffer_set_min_size(b,size*sizeof(T));\
}                                               


#define     DEFINE_VECTOR(T,C)                  \
typedef struct C {                              \
    sbuffer b;                                  \
    int count;                                  \
} C;                                            \
C*      C##_new(size_t size);                   \
void    C##_free(C* b);                         \
int     C##_set_size(C* b, size_t size);        \
size_t  C##_capacity(C* b);                     \
size_t  C##_count(C* b);                        \
T*      C##_geti(C* b,int i);                   \
T*      C##_getp(C* b);                         \
int     C##_push_back(C* b,const T* a);         \
int     C##_push_front(C* b,const T* a);        \
int     C##_delete(C* b,size_t i);              \
int     C##_set_min_size(C* b,size_t size);

#define     IMPLEMENT_VECTOR(T,C)               \
C*       C##_new(size_t size)                   \
{                                               \
    C* a = (C*)ztools_calloc(1,sizeof(C));      \
    sbuffer_init(&a->b,size*sizeof(T));         \
    return a;                                   \
}                                               \
                                                \
void    C##_free(C* b)                          \
{                                               \
    sbuffer_done(&b->b);                        \
    ztools_free(b);                             \
}                                               \
                                                \
int     C##_set_size(C* b, size_t size)         \
{                                               \
    return sbuffer_resize(&b->b,size*sizeof(T));\
}                                               \
                                                \
size_t  C##_capacity(C* b)                      \
{                                               \
    return b->b.size / sizeof(T);               \
}                                               \
size_t  C##_count(C* b)                         \
{                                               \
    return b->count;                            \
}                                               \
                                                \
T*      C##_geti(C* b,int i)                    \
{                                               \
    return ((T*)(b->b.t))+i;                    \
}                                               \
                                                \
T*      C##_getp(C* b)                          \
{                                               \
    return (T*)(b->b.t);                        \
}                                               \
                                                \
int     C##_set_min_size(C* b,size_t size)      \
{                                               \
    return sbuffer_set_min_size(&b->b,size*sizeof(T));\
}                                               \
int     C##_push_back(C* b,const T* a)          \
{                                               \
    C##_set_min_size(b,b->count+1);             \
    ((T*)(b->b.t))[b->count] = (T)*a;           \
    b->count++;                                 \
    return 0;                                   \
}                                               \
int     C##_push_front(C* b,const T* a)         \
{                                               \
    int i = b->count;                           \
    C##_set_min_size(b,b->count+1);             \
    for( ; i > 0; i-- ) {                       \
        ((T*)(b->b.t))[i] = ((T*)(b->b.t))[i-1];\
    }                                           \
    ((T*)(b->b.t))[0] = (T)*a;                  \
    b->count++;                                 \
    return 0;                                   \
}                                               \
int     C##_delete(C* b,size_t i)               \
{                                               \
    for( i+=1; (int)i < b->count; i++ ) {       \
        ((T*)(b->b.t))[i-1] = ((T*)(b->b.t))[i];\
    }                                           \
    b->count--;                                 \
    return 0;                                   \
}                                               

#endif


#ifndef __sbtree_h_
#define __sbtree_h_
#define INC_SBTREE

typedef struct smap_item sbtree_item;

typedef smap sbtree;

typedef smapi_t	sbti_t;

typedef sbti_t* sbti;



ZCEXPORT sbtree*    sbtree_new(void);

typedef void (*sbtree_free_f)(void*);

ZCEXPORT void    sbtree_free(sbtree* b,sbtree_free_f free_callback );
ZCEXPORT void    sbtree_frees(sbtree* b,sbtree_free_f free_callback );

ZCEXPORT int    sbtree_add(sbtree* bt,const char* key,void* data);

ZCEXPORT void*    sbtree_del(sbtree* bt,const char* key);
ZCEXPORT sbti    sbtree_del_i(sbtree* bt,sbti i);

ZCEXPORT void*    sbtree_find(sbtree* bt,const char* key);
ZCEXPORT sbti    sbtree_find_i(sbtree* bt,const char* key,sbti i);

ZCEXPORT int    sbtree_delete(sbtree* bt);

ZCEXPORT sbti    sbtree_begin(sbtree* bt,sbti i);
ZCEXPORT sbti    sbtree_end(sbtree* bt,sbti i);

ZCEXPORT void    sbti_free(sbti i);

ZCEXPORT sbti    sbti_inc(sbti i);
ZCEXPORT sbti    sbti_dec(sbti i);
ZCEXPORT int    sbti_eq(sbti a,sbti b);

ZCEXPORT void*    sbti_get(sbti i);
ZCEXPORT const char*
	    sbti_key(sbti i);

#define SBTI_START_ENUM(tree,i,k,type)		\
    {						\
	sbti_t sbt_e_begin,sbt_e_end;		\
	type i; 				\
	const char* k;				\
	sbtree_begin((tree),&sbt_e_begin);	\
	sbtree_end((tree),&sbt_e_end);		\
	sbti_inc(&sbt_e_end);			\
	for(; !sbti_eq(&sbt_e_begin,&sbt_e_end); \
	      sbti_inc(&sbt_e_begin) ) {	\
	    i = (type) sbti_get(&sbt_e_begin);	\
	    k = sbti_key(&sbt_e_begin); {

#define SBTI_STOP_ENUM }}}

#define SBTI_ENUM_BEGIN SBTI_START_ENUM
#define SBTI_ENUM_END	SBTI_STOP_ENUM

#endif

#ifndef __slist_h_
#define __slist_h_
#define INC_SLIST

struct slist_item {
	MDMF_DEF
	void*			item;
	struct slist_item
*	next;
};
typedef struct slist_item slist_item;

struct slist_a {
	MDMF_DEF
	slist_item* head;
	slist_item* tail;
};
typedef struct slist_a slist;

struct slist_iterator {
    MDMF_DEF
    slist_item*	item;
    slist*	list;
};

typedef struct slist_iterator* sli;

typedef struct slist_iterator sli_t;

/** Allocate new list.
*/
ZCEXPORT slist*    slist_new(void);

/** Free list.
*/
ZCEXPORT int    slist_delete(slist* bt);
ZCEXPORT int    slist_free(slist* bt);

/** Init list.
*/
ZCEXPORT slist*    slist_init(slist* l);

/** Free list data.

    Free all data contained by list.
    Don't free slist structure.
*/
ZCEXPORT int    slist_done(slist* bt);


/** Check if list is empty.
    Return 1 if empty 0 if not.
*/
ZCEXPORT int    slist_empty(slist* l);

/** Add data item to list.
*/
ZCEXPORT int        slist_add(slist* l,void* data);

/** Delete first occurence of data.
*/
ZCEXPORT void*      slist_del(slist* l,void* data);

/** Delete all occurences of data.
*/
ZCEXPORT void       slist_del_all(slist* l,void* data);

ZCEXPORT sli    slist_begin(slist* l,sli_t* a);
ZCEXPORT sli    slist_end(slist* l,sli_t* a);
ZCEXPORT sli    sli_inc(sli a);
ZCEXPORT int    sli_eq(sli a,sli b);
ZCEXPORT void*    sli_get(sli i);
ZCEXPORT void    sli_free(sli i);

ZCEXPORT sli    sli_first(slist* l);
ZCEXPORT sli    sli_next(sli i);
ZCEXPORT int    sli_end(sli i);

#endif

#ifndef __sstack_h_
#define __sstack_h_
#define INC_SSTACK

#ifndef INC_SBUFFER
#include "sbuffer.h"
#endif

struct _sstack {
    MDMF_DEF
    sbuffer	buf;
    char*	top;
    size_t	ptr;
};
typedef struct _sstack sstack;

struct sstack_iterator {
    MDMF_DEF
    sstack*	stack;
};

typedef struct sstack_iterator* ssti;

ZCEXPORT sstack*    sstack_new(size_t count);
ZCEXPORT void    sstack_delete(sstack* v);
ZCEXPORT void    sstack_free(sstack* v);

ZCEXPORT void    sstack_push(sstack* s,void* ptr,size_t size);
ZCEXPORT void    sstack_pop(sstack* s,void* ptr,size_t size);
ZCEXPORT int    sstack_empty(sstack* s);
ZCEXPORT int    sstack_left(sstack* s);

#define sstack_empty(s) ((s!=NULL) ? ( (s)->ptr == 0 ) : 1)

#define sstack_pushvar(s,v) ( sstack_push((s),&(v),sizeof(v)) )
#define sstack_popvar(s,v) ( sstack_pop((s),&(v),sizeof(v)) )

#endif

#ifndef __sstr_h_
#define __sstr_h_
#define INC_SSTR

struct sstr_tag {
    MDMF_DEF
    char*	buf; /* buffer	 */
    size_t	cap; /* capacity */
};

#define SSTR_EMPTY { MDMF_INIT("sstr") (char*)0,0 }
#define SSTR_INITIALIZER SSTR_EMPTY

typedef struct sstr_tag sstr;

/** Alloc a new string
    Allocates a new structure for a new string. If a s parameter is not NULL
    function sets new strings contens to s.
    Returns a pointer to newly allocated structure, or NULL when an error occured.

*/
ZCEXPORT sstr*    sstr_new(const char* s);
/** Alloc a new string
    Allocates a new structure for a new string. If a s parameter is not NULL
    function sets new strings contens to s.
    Returns a pointer to newly allocated structure, or NULL when an error occured.
*/
ZCEXPORT sstr*    sstr_news(const sstr* s);

/** Free
    Frees strings buffer and structure containing string.
*/
ZCEXPORT void    sstr_free(sstr* s);
/** Free static
    Frees only strings buffer, doesn't touch sstr struct.
*/
ZCEXPORT void    sstr_frees(sstr* s);

/** Returns lenght.
    Returns length of string (using strlen). Or zero if string is NULL
*/
ZCEXPORT int    sstr_len(const sstr* s);

#define sstr_length sstr_len
/** Returns capacity.
    Returns capacity of strings buffer or zero if string is NULL.
*/
ZCEXPORT int    sstr_capacity(const sstr* s);
#define sstr_cap sstr_capacity

/** Change contens.
    Change contens of string. Destroys old capacity. Resizes capacity
    only if new string is lenghter than old capacity.

    @returns less than zero when an error occurs, 0 when success
*/
ZCEXPORT int    sstr_set(sstr* s,const char* s1);

/** Set contens.
    Change contens of string. Destroys old capacity. Resizes capacity
    only if new string is lenghter than old capacity.
    Treat s as _invalid_ structure, overwrite it.

    @returns less than zero when an error occurs, 0 when success
*/
ZCEXPORT int    sstr_safe_set(sstr* s,const char* s1);

/** Change contens.
    Change contens of string. Destroys old capacity. Resizes capacity
    only if new string is lenghter than old capacity.

    @returns less than zero when an error occurs, 0 when success
*/
ZCEXPORT int    sstr_sets(sstr* s,const sstr* s1);

/** Append string.
    Append text to existing contens. If its required new memory is allocated.

    @returns less than zero when an error occurs, 0 when success
*/
ZCEXPORT int    sstr_add(sstr* s,const char* s1);
ZCEXPORT int    sstr_add_len(sstr* s,int len,const char* s1,int len1);
/** Append char.
    Append char to existing contens. If its required new memory is allocated.

    @returns less than zero when an error occurs, 0 when success
*/
ZCEXPORT int    sstr_addc(sstr* s,char c);
ZCEXPORT int    sstr_addc_len(sstr* s,int len,char c);
/** Append string.
    Append text to existing contens. If its required new memory is allocated.

    @returns less than zero when an error occurs, 0 when success
*/
ZCEXPORT int    sstr_adds(sstr* s,const sstr* s1);
/** Append integer.
    Append text representation of integer number to existing contens.
    If its required new memory is allocated.

    @returns less than zero when an error occurs, 0 when success
*/
ZCEXPORT int    sstr_addi(sstr* s,int x);
/** Append float.
    Append text representation of float number to existing contens.
    If its required new memory is allocated.

    @returns less than zero when an error occurs, 0 when success
*/
ZCEXPORT int    sstr_addf(sstr* s,float f);

#endif

#ifdef __cplusplus
};
#endif

#ifdef __cplusplus
#ifndef __sstr_h_cpp_
#define __sstr_h_cpp_

#include <stdlib.h>

class sstr_c : public sstr_tag {
public:
    sstr_c() {
	buf = 0;cap = 0;
    }
    sstr_c(const char* s) {
	buf = 0;cap = 0;
	sstr_set(this,s);
    }
    sstr_c(const sstr_c& s) {
	buf = 0;cap = 0;
	sstr_set(this,s.buf);
    }
    ~sstr_c() { 			sstr_frees(this); }

    void add(const char* s) {		sstr_add(this,s); }
    void add(const sstr* s) {		sstr_add(this,s->buf); }
    void add(const sstr_c& s) { 	sstr_add(this,s.buf); }
    void add(char c) {			sstr_addc(this,c); }
    void add(int i) {			sstr_addi(this,i); }
    void add(float f) { 		sstr_addf(this,f); }

    void set(const char* s) {		sstr_set(this,s); }
    void set(const sstr* s) {		sstr_set(this,s->buf); }
    void set(const sstr_c& s) { 	sstr_set(this,s.buf); }

    sstr_c& operator = (const char* s) { set(s); return *this; }
    sstr_c& operator = (const sstr* s) { set(s); return *this; }
    sstr_c& operator = (const sstr_c& s) { set(s); return *this; }

    sstr_c& operator += (const char* s) { add(s); return *this; }
    sstr_c& operator += (const sstr* s) { add(s); return *this; }
    sstr_c& operator += (const sstr_c& s) { add(s); return *this; }
    sstr_c& operator += (char  c) { add(c); return *this; }
    sstr_c& operator += (int   i) { add(i); return *this; }
    sstr_c& operator += (float f) { add(f); return *this; }

    sstr_c& operator << (const char* s) { add(s); return *this; }
    sstr_c& operator << (const sstr* s) { add(s); return *this; }
    sstr_c& operator << (const sstr_c& s) { add(s); return *this; }
    sstr_c& operator << (char  c) { add(c); return *this; }
    sstr_c& operator << (int   i) { add(i); return *this; }
    sstr_c& operator << (float f) { add(f); return *this; }
    /*int*/ operator int ()
            {

               if( buf ) return atoi(buf);
	       return 0;
            }
  /*double*/operator double ()
            {
               if( buf ) return atof(buf);
	       return 0.0;
            }

 /* char* */operator char* () { return buf; }

    int length() {
        size_t strlen(const char*);
        return strlen(buf);
    }
    int capacity() { return cap; }

    char* str() { return buf; }

};
#endif /* __sstr_h_cpp_ */
#endif /* c++ */

#endif /* force single include of file */

