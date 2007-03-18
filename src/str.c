/*
* File: 	str.c
*
* Id:           $Id$
*
* Project:	zcompat
*
* Description:
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2004,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "zcompat/str.h"
#include "zcompat/zprintf.h"

#define ZCSTR_REF_CNT 1

char* zc_str_empty = "";

zc_str    zcs_alloc(unsigned int len)
{
    zc_str a = (zc_str)malloc( sizeof( zc_str_t ) + len + 1 );
    a->capacity = len;
    a->length = 0;
    a->ref_cnt = 1;
    /*MDMF_FILL(a,"xlst");*/
    return a;
}

zc_str    zcs_realloc(zc_str s,unsigned int len)
{
    unsigned int olen = zcs_len(s);
    zc_str a = (zc_str)realloc( (void*)s,sizeof( zc_str_t ) + len + 1 );
    if( s != NULL ) {
        a->capacity = len;
        a->length = zmin(a->capacity,olen);
        zcs_get(a)[a->length] = '\0';
    }
    /*MDMF_FILL(a,"xlst");*/
    return a;
}

/** Create string.
*/
zc_str	zcs_new(const char* s)
{
    return zcs_newn(s,s !=NULL ? strlen(s) : 0);
}

zc_str	zcs_newn(const char* s,unsigned int len)
{
    zc_str a;
    a = zcs_alloc(len);
    a->capacity = len;
    a->length = len;
    a->ref_cnt = 1;
    if( s != NULL ) {
	strncpy( zcs_get(a), s,len);
	zcs_ptr(a)[len] = '\0';
    } else
	zcs_ptr(a) [0] = 0;
    return a;
}

zc_str	zcs_ref(const zc_str_t* s)
{
    if( s )
        ((zc_str_t*)s)->ref_cnt+=1;
    return (zc_str)s;
}

zc_str	zcs_new_zcs(const zc_str_t* s)
{
    if( s ) 
        ((zc_str_t*)s)->ref_cnt+=1;
    return (zc_str)s;
}

void	zcs_free(zc_str a)
{
    if( a == NULL ) return;
    a->ref_cnt-=1;
    if( a->ref_cnt <= 0 )
    	free((void*)a);
}

zc_str	zcs_set 	(zc_str dest,const char* src)
{
    if( dest && dest->ref_cnt == 1 )
	return zcs_setn(dest,src,strlen(src));
    else {
	zcs_free(dest);
	return zcs_new(src);
    }
}

zc_str	zcs_setn 	(zc_str dest,const char* src,unsigned int len)
{
    if( dest == NULL && src == NULL )
	return NULL;
    if( dest != NULL && dest->ref_cnt == 1 ) {
	if( dest == NULL && src != NULL )
	    return zcs_newn( src, len );
	else {
	    if( len > dest->capacity ) {
		dest = zcs_realloc(dest,len);
		if( dest == NULL )
		    return NULL;
	    }
	    if( src != NULL ) {
		strncpy( zcs_ptr( dest ), src, len );
		zcs_ptr(dest)[len] = '\0';

		dest->length = len;
	    } else {
		zcs_ptr( dest ) [0] = 0;
		dest->length = 0;
	    }
	    return dest;
	}
    } else {
	zcs_free(dest);
	return zcs_newn(src,len);
    }
}

zc_str	zcs_set_zcs	(zc_str dest,const zc_str_t* src)
{
    if( dest == NULL )
	return zcs_new_zcs(src);
    if( dest->ref_cnt == 1 )
	return zcs_setn(dest,zcs_ptr(src),zcs_len(src));
    else {
	((zc_str_t*)src)->ref_cnt+=1;
	zcs_free(dest);
	return (zc_str)src;
    }
}

extern zc_str	zcs_cat(zc_str dest, ...)
{
    va_list ap;
    zc_str ret;
    va_start(ap,dest);
    ret = zcs_catv(dest,ap);
    va_end(ap);
    return ret;
}
extern zc_str	zcs_catv(zc_str dest, va_list ap)
{
    char* s;

    while( (s = va_arg(ap,char*)) != NULL )
	dest = zcs_add(dest,s);
    return dest;
}

zc_str	zcs_addn 	(zc_str dest,const char* src,unsigned int len2)
{
    if( dest == NULL ) {
	if( src != NULL )
	    return zcs_newn(src,len2);
	else
	    return NULL;
    }
    if( dest->ref_cnt == 1 ) {
	if( src == NULL )
	    return dest;
	else {
	    unsigned len1 = zcs_len(dest);
	    /*  int len2 = length of src */
	    unsigned lens = len1 + len2;
	    if( lens > dest->capacity ) {
		dest = zcs_realloc(dest,lens);
		if( dest == NULL )
		    return NULL;
	    }
	    memcpy( zcs_get( dest ) + len1, src, len2);
	    zcs_get(dest)[lens] = '\0';
	    dest->length = lens;
	    return dest;
	}

    } else {
	zc_str x;
	unsigned len1 = zcs_len(dest);
	/*  int len2 = length of src */
	unsigned lens = len1 + len2;
	char* xs;
	if( src == NULL ) return NULL;
	x = zcs_alloc(lens);
	if( !x ) return NULL;
	xs = zcs_ptr(x);
	memcpy(xs,     zcs_get(dest), len1);
	memcpy(xs+len1,src, len2);
	xs[lens] = '\0';
	x->length = lens;
	zcs_free(dest);
	return x;
    }
}

zc_str	zcs_add 	(zc_str dest,const char* src)
{
    register int x = src ? strlen(src) : 0;
    return zcs_addn(dest,src,x);
}

zc_str	zcs_add_zcs	(zc_str dest,const zc_str_t* src)
{
    if( dest == NULL ) {
	if ( src != NULL )
	    return zcs_new_zcs(src);
	else
	    return NULL;
    } else
	if( src == NULL ) 
	    return dest;
    return zcs_addn(dest,zcs_ptr(src),zcs_len(src));
}

zc_str	zcs_add_char	(zc_str dest,char src)
{
    char x[2];
    x[0] = src;
    x[1] = 0;
    return zcs_addn(dest,x,1);
}

zc_str	zcs_add_int	(zc_str dest,int i,const char* fmt)
{
    char x[40];
    register int len = sprintf(x,fmt ? fmt : "%i", i);
    return zcs_addn(dest,x,len);
}

zc_str	zcs_add_float	(zc_str dest,double f,const char* fmt)
{
    char x[80];
    register int len = sprintf(x,fmt ? fmt : "%f", f);
    return zcs_addn(dest,x,len);
}
static void	zcs_str_erase_blank (char* src,int *len);

extern zc_str	zcs_erase_blank (zc_str src)
{
    char* es,*ee;
    char* s;
    int si = 0;
    if( zcs_empty(src) )
	return src;
    if( src->ref_cnt > 1 ) {
	zcs_free(src);
	src = zcs_newn(zcs_get(src),zcs_len(src));
    }
    s = zcs_str(src);
    for( es = s; *es && isspace(*es); es++,si++ ) ;

    for( ee = s + zcs_len(src)-1; isspace(*ee) ; ee-- ) {
	*ee = '\0';
	--(src->length);
    }
    if( es > s ) {
	memmove(s,es,src->length - si);
	src->length-=si;
	s[src->length] = '\0';
    }
    return src;
}

static void	zcs_str_erase_blank (char* src,int *len)
{
    char* es,*ee;
    char* s = src;
    int si = 0;
    if( !s || !*s ) return;
    for( es = s; *es && isspace(*es); es++,si++ ) ;

    for( ee = s + *len-1; isspace(*ee) ; ee-- ) {
	*ee = '\0';
	--(*len);
    }
    if( es > s ) {
	memmove(s,es,*len - si);
	*len-=si;
	s[*len] = '\0';
    }
    si = *len;
    while( *s )  {
	if( isspace(s[0]) && isspace(s[1]) ) {
	    int x = si-1;
	    zcs_str_erase_blank(s+1,&x);
	    *len -=(si-1-x);
	    return;
	}
	s++;
	si--;
    }
}

int	zcs_bwrite	(ZSTREAM f,const zc_str_t* src)
{
    unsigned long len = zcs_len(src);
    int x = 0;
/*    x += zwrite(f,&len,sizeof(len));*/
    x += znwrite_l(f,&len);
    if( x > 0 )
	x += zwrite (f,zcs_ptr(src),len);
    return x;
}
zc_str	zcs_bread	(ZSTREAM f)
{
    unsigned long len;
    zc_str s;
    if( znread_l(f, &len ) <= 0 ) return NULL;
    s = zcs_alloc(len);
    if( s == NULL ) return NULL;
    if( zread(f,zcs_ptr(s),len) < len ) { zcs_free(s); return NULL; }
    zcs_ptr(s)[len] = '\0';
    s->length = len;
    return s;
}

zc_str	zcs_sprintf(zc_str s,const char* fmt, ...)
{

    va_list ap;
    va_start(ap, fmt);
    s = zcs_vprintf(s,fmt,ap);
    va_end(ap);
    return s;
}

static int zcsprintf_flush(zvformatter_buff *vbuff)
{
    zc_str s = (zc_str) vbuff->param;
    zc_str ns;
    unsigned nc = s->capacity > 2 ? s->capacity*2 : 3 ;
    unsigned clen = vbuff->curpos - zcs_get(s);
    s->length = clen;
    ns = zcs_realloc(s,nc);
    if( !ns ) return -1;
    s = ns;
    s->capacity = nc;
    s->length = clen;
    vbuff->curpos = zcs_get(s) + clen;
    vbuff->endpos = zcs_get(s) + zcs_cap(s);
    vbuff->param = (void*)s;
    vbuff->curpos[0] = 0;
    vbuff->endpos[0] = 0;
    return 0;
}

zc_str	zcs_vprintf(zc_str s,const char* fmt, va_list ap)
{
    int cc;
    zvformatter_buff vbuff;
    if( s == NULL ) {
	s = zcs_alloc(10);
	if( s == NULL ) return NULL;

    }
    if( s->ref_cnt > 1 ) {
	int x = s->capacity;
	zcs_free(s);
	s = zcs_alloc(x);
    }

    vbuff.curpos = zcs_get(s);
    vbuff.endpos = zcs_get(s) + zcs_cap(s);
    vbuff.param = (void*)s;

    cc = zvformatter(zcsprintf_flush,&vbuff,fmt,ap);

    s = (zc_str)vbuff.param;
    *(vbuff.curpos) = '\0';
    s->length = vbuff.curpos - zcs_get(s);

    if( cc == -1 ) {
	zcs_free(s);
	return NULL;
    } else
	return s;
}
int zcs_cmp(const zc_str_t* a, const zc_str_t* b)
{

    register unsigned r;
    register const char* ac = zcs_get(a);
    register const char* bc = zcs_get(b);
    register size_t len;
    if( a == b ) return 0;
    if( a )
	if( !b )
	    return 1;
	else ;
    else
	if( !b )
	    return 0;
	else
	    return -1;
/*
    r = *ac - *bc;
    if( r ) return r;
*/
    len = zmin( _zcs_len(a),_zcs_len(b));
/*
    ac++, bc++, len--;
    if( len <= 0 ) return 0;
    r = *ac - *bc;
    if( r ) return r;

    ac++, bc++,len--;
    if( len <= 0 ) return 0;
*/
    r = memcmp(ac,bc,len);
    if( r ) return r;
    return _zcs_len(a) - _zcs_len(b);
}

int zcs_eq(const zc_str_t* a, const zc_str_t* b)
{
    register int r;
    r = (_zcs_len(a) - _zcs_len(b)) == 0;
    if( !r ) return 0;
    return zcs_cmp(a,b) == 0;
}



