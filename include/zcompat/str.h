/*
* Header:	str.h
*
* Id:           $Id$
*
* Project:	ZCompat Reference Counted String
*
* Description:	zcompat ref-counted string definition
*
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2003,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifndef __zcompat_str_h_
#define __zcompat_str_h_

#define INC_STR
#include <string.h>
#ifndef INC_ZDEFS
#include "zcompat/zdefs.h"
#endif

/* #define WITHOUT_ZPIO*/

#ifndef WITHOT_ZPIO
#ifndef INC_ZPIO
#include "zcompat/zpio.h"
#endif
#endif

/* C Extern Identifiers. */
#ifdef __cplusplus
extern "C" {
#endif

struct _zc_str {
    /*MDMF_DEF*/
    int		ref_cnt;
    size_t	capacity;
    int		length;
    char	first[10];
};
typedef struct	_zc_str	 zc_str_t;

typedef	struct	_zc_str* zc_str;

typedef	const struct	 _zc_str* const_zc_str;

extern char* zc_str_empty;

extern zc_str	zcs_ref(const zc_str_t* s);
extern void	zcs_free(zc_str a);
extern zc_str   zcs_alloc(unsigned int len);
extern zc_str   zcs_realloc(zc_str s,unsigned int new_len);

extern zc_str	zcs_new(const char* s);
extern zc_str	zcs_newn(const char* s,unsigned int len);
extern zc_str	zcs_new_zcs(const zc_str_t* s);
/*
    zcs_cmp

    Compare strings. Bevaviour like strcmp().
*/
extern int	zcs_cmp(const zc_str_t* a, const zc_str_t* b);

/*
    zcs_eq

    Check if strings are equal. Optimized for hash tables where only
    equal/not equal condition is checked.
*/
extern int      zcs_eq(const zc_str_t* a, const zc_str_t* b);
/** Set contens.
*/
extern zc_str	zcs_set 	(zc_str dest,const char* src);
extern zc_str	zcs_setn	(zc_str dest,const char* src,unsigned int len);
extern zc_str	zcs_set_zcs	(zc_str dest,const zc_str_t* src);

extern zc_str	zcs_cat(zc_str dest, ...);
extern zc_str	zcs_catv(zc_str dest, va_list ap);

/** Concatenate with a string with known length.
*/
zc_str	zcs_addn 	(zc_str dest,const char* src,unsigned int len);
/** Concatenate with a string.
*/
extern zc_str	zcs_add 	(zc_str dest,const char* src);

/** Concatenate with a "zcs" string.
*/
extern zc_str	zcs_add_zcs	(zc_str dest,const zc_str_t* src);

/** Add a char to the string.
*/
extern zc_str	zcs_add_char	(zc_str dest,char src);
/** Add a integer to the string.
*/
extern zc_str	zcs_add_int	(zc_str dest,int i,const char* fmt);
/** Add a float to the string.
*/
extern zc_str	zcs_add_float	(zc_str dest,double f,const char* fmt);

#ifndef WITHOUT_ZPIO
extern int	zcs_bwrite	(ZSTREAM f,const zc_str_t* src);
extern zc_str	zcs_bread	(ZSTREAM f);
#endif

extern zc_str	zcs_erase_blank (zc_str src);

zc_str	zcs_printf(zc_str s,const char* fmt, ...);
zc_str	zcs_vprintf(zc_str s,const char* fmt, va_list ap);

#if !defined(HAVE_INLINE) && !defined(__cplusplus)
#define _zcs_get( a )	((a)->first)
#define _zcs_str( a )	((a)->first)
#define _zcs_ptr( a )	((a)->first)
#define _zcs_len( a )	((a)->length)
#define _zcs_empty( a ) (zcs_str(a)[0] == '\0' )
#define _zcs_cap( a )	((a)->capacity)


#define zcs_get( a ) (a != NULL ? _zcs_get(a) : NULL )
#define zcs_str( a ) (a != NULL ? _zcs_str(a) : zc_str_empty )
#define zcs_ptr( a ) (a != NULL ? _zcs_ptr(a) : NULL )
#define zcs_len( a ) (a != NULL ? _zcs_len(a) : 0 )
#define zcs_empty( a ) (a != NULL ?  _zcs_empty(a) : 1 )
#define zcs_cap( a ) (a != NULL ? _zcs_cap(a) : 0 )




#else

static inline char*	_zcs_get(const zc_str_t* a)
{
    return (char*)a->first;
}

static inline char*	_zcs_str(const zc_str_t* a)
{
    return (char*)a->first;
}
static inline int	_zcs_empty(const zc_str_t* a)
{
    return _zcs_str(a)[0] == '\0';
}
static inline char*	_zcs_ptr(const zc_str_t* a)
{
    return (char*)a->first;
}
static inline int	_zcs_len(const zc_str_t* a)
{
    return a->length;
}
static inline int	_zcs_cap(const zc_str_t* a)
{
    return a->capacity;
}

inline static char*	zcs_get(const zc_str_t* a)
{
    return (a != NULL ? (char*)((a)->first) : NULL );
}

static inline char*	zcs_str(const zc_str_t* a)
{
    return (a != NULL ? (char*)((a)->first) : zc_str_empty );
}
static inline int	zcs_empty(const zc_str_t* a)
{
    return (zcs_str(a)[0] == '\0' );
}
static inline char*	zcs_ptr(const zc_str_t* a)
{
    return (a != NULL ? (char*)((a)->first) : NULL );
}
static inline int	zcs_len(const zc_str_t* a)
{
    return (a != NULL ? (a)->length : 0 );
}
static inline int	zcs_cap(const zc_str_t* a)
{
    return (a != NULL ? (a)->capacity : 0 );
}


#endif


#ifdef __cplusplus
}
#endif

#endif /* force single include of file */
