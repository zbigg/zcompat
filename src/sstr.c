/*
* File:		sstr.c
*
* Id:           $Id: sstr.c,v 1.4 2003/06/27 19:04:34 trurl Exp $
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "zcompat/zcompat.h"
#include "zcompat/ztools.h"

#include "ztools_i.h"

int	sstr_resize(sstr* b,size_t new_size);
int	sstr_set_min_size(sstr* b,size_t min_size);

/** Alloc a new string
    Allocates a new structure for a new string. If a s parameter is not NULL
    function sets new strings contens to s.
    Returns a pointer to newly allocated structure, or NULL when an error occured.

*/
#define sstr_alloc(n) ((sstr*)(zt_calloc(n,sizeof(sstr))))
static char sstr_empty[20] = "";

sstr*	sstr_new(const char* s)
{
    sstr* a = sstr_alloc(1);
    if( a == NULL)
	return NULL;
    MDMF_FILL(a,"sstr");
    if( s != NULL)
	if( sstr_set(a,s) < 0 ) {
	    zt_free(a);
	    return NULL;
	} else ;
    else
	a->buf = sstr_empty;
    return a;
}
/** Alloc a new string
    Allocates a new structure for a new string. If a s parameter is not NULL
    function sets new strings contens to s.
    Returns a pointer to newly allocated structure, or NULL when an error occured.
*/
sstr*	sstr_news(const sstr* s)
{
    sstr* a = sstr_alloc(1);
    if( !a )
	return NULL;
    MDMF_FILL(a,"sstr");
    if( s )
	if( sstr_sets(a,s) < 0 ) {
	    zt_free(a);
	    return 0;
	}
    return a;
}

/** Free
    Frees strings buffer and structure containing string.
*/
void	sstr_free(sstr* s)
{
    if( s != NULL ) {
	if( s->buf )
	    if( s->buf != sstr_empty )
		zt_free(s->buf);
	s->buf = sstr_empty;
	zt_free(s);
    }
}

/** Free static
    Frees only strings buffer, doesn't touch sstr struct.
*/
void	sstr_frees(sstr* s)
{
    if( s ) {
	if( s->buf )
	    if( s->buf != sstr_empty )
		zt_free(s->buf);
	s->buf = sstr_empty;
    }
}


/** Returns lenght.
    Returns length of string (using strlen). Or zero if string is NULL
*/
int	sstr_len(const sstr* s)
{
    if( s == NULL )
	return 0;
    return (s->buf != NULL ) ? strlen(s->buf) : 0;
}

/** Returns capacity.
    Returns capacity of strings buffer or zero if string is NULL.
*/
int	sstr_capacity(const sstr* s)
{
    if( s == NULL )
	return 0;
    return s->cap;
}

/** Change contens.
    Change contens of string. Destroys old capacity. Resizes capacity
    only if new string is lenghter than old capacity.

    @returns less than zero when an error occurs, 0 when success
*/
int	sstr_set(sstr* s,const char* o)
{
    int a;
    if( s == NULL )
	return -1;
    if( o == NULL ) {
	if( s->buf != NULL )
	    s->buf[0] = 0;
	return 0;
    }
    a = strlen(o);
    if( sstr_set_min_size(s,a) < 0 )
	return -1;
    strcpy(s->buf,o);
    return 0;
}

int	sstr_safe_set(sstr* s,const char* o)
{
    int a;
    if( s == NULL )
	return -1;
    a = strlen(o);
    s->buf = 0;
    s->cap = 0;
    if( sstr_set_min_size(s,a) < 0 )
	return -1;
    strcpy(s->buf,o);
    return 0;
}

/** Change contens.
    Change contens of string. Destroys old capacity. Resizes capacity
    only if new string is lenghter than old capacity.

    @returns less than zero when an error occurs, 0 when success
*/
int	sstr_sets(sstr* s,const sstr* x)
{
    int a;
    char* o;
    if( s == NULL || x == NULL )
	return -1;
    o = x->buf;
    if( o == NULL ) {
	if( s->buf )
	    s->buf[0] = 0;
	return 0;
    }
    a = strlen(o);
    if( sstr_set_min_size(s,a) < 0 )
	return -1;
    strcpy(s->buf,o);
    return 0;
}

/** Append string.
    Append text to existing contens. If its required new memory is allocated.

    @returns less than zero when an error occurs, 0 when success
*/

int	sstr_add_len(sstr* s,int len,const char* s1,int len1)
{
    if( s == NULL )
	return -1;
    if( s1 == NULL) {
	return 0;
    }
    if( s->buf == NULL )
	return sstr_set(s,s1);
    if( s->buf[0] == '\0')
	return sstr_set(s,s1);
    if( sstr_set_min_size(s,len+len1) < 0 )
	return -1;
    strcpy(s->buf+len,s1);
    return 0;
}

int	sstr_add(sstr* s,const char* o)
{
    int a,b;
    if( s == NULL )
	return -1;
    if( o == NULL) {
	return 0;
    }
    if( s->buf == NULL )
	return sstr_set(s,o);
    if( s->buf[0] == '\0')
	return sstr_set(s,o);
    a = strlen(s->buf);
    b = strlen(o);
    if( sstr_set_min_size(s,a+b) < 0 )
	return -1;
    strcpy(s->buf+a,o);
    return 0;
}
int	sstr_addc_len(sstr* s,int len,char c)
{
    if( s->cap == 0 || s->buf == NULL || s->buf == sstr_empty ) {
	if( sstr_set_min_size(s,2) < 0 )
	    return -1;
	s->buf[0] = c;
	s->buf[1] = 0;
    } else {
	register unsigned int i = len;
	if( i+1 >= s->cap )
	    if( sstr_set_min_size(s,s->cap+1) < 0 )
		return -1;
	s->buf[i++] = c;
	s->buf[i++] = 0;
    }
    return 0;
}

int	sstr_addc(sstr* s,char c)
{
    if( s->cap == 0 || s->buf == NULL || s->buf == sstr_empty ) {
	if( sstr_set_min_size(s,4) < 0 )
	    return -1;
	s->buf[0] = c;
	s->buf[1] = 0;
    } else {
	register unsigned int i = 0;
	while( s->buf[i] != '\0' ) i++;
	if( i+1 >= s->cap )
	    if( sstr_set_min_size(s,s->cap+4) < 0 )
		return -1;
	s->buf[i++] = c;
	s->buf[i++] = 0;
    }
    return 0;
/*
    char buf[2];
    buf[0] = c;
    buf[1] = 0;
    return sstr_add(s,buf);
*/
}

/** Append string.
    Append text to existing contens. If its required new memory is allocated.

    @returns less than zero when an error occurs, 0 when success
*/
int	sstr_adds(sstr* s,const sstr* o)
{
    if( o != NULL )
	return sstr_add(s,o->buf);
    return -1;
}
/** Append integer.
    Append text representation of integer number to existing contens.
    If its required new memory is allocated.

    @returns less than zero when an error occurs, 0 when success
*/
char sstr_tmpbuf[100];
int	sstr_addi(sstr* s,int x)
{
    sprintf(sstr_tmpbuf,"%i",x);
    return sstr_add(s,sstr_tmpbuf);
}
/** Append float.
    Append text representation of float number to existing contens.
    If its required new memory is allocated.

    @returns less than zero when an error occurs, 0 when success
*/
int	sstr_addf(sstr* s,float f)
{
    sprintf(sstr_tmpbuf,"%f",f);
    return sstr_add(s,sstr_tmpbuf);
}


int	sstr_resize(sstr* b,size_t new_size)
{
    if( b->buf == NULL || b->buf == sstr_empty ) {
	b->buf = (char*)zt_malloc(new_size);
	if( b->buf == NULL )
	    return -1;
	memset(b->buf,0,new_size);
	b->cap = new_size;
	return 0;
    } else if( b->buf != NULL &&
	    b->cap != new_size ) {
	char *x = zt_realloc(b->buf,new_size);
	if( x == NULL )
	    return -1;
	if( new_size > b->cap )
	    memset(&x[b->cap],0,new_size-b->cap);
	b->buf = x;
	b->cap = new_size;
	return 0;
    }
    return -1;
}
int	sstr_set_min_size(sstr* b,size_t min_size)
{
    int r = 0;
    while( b->cap < min_size+1 && r == 0)
	r = sstr_resize(b,(b->cap+1)*3);
    return r;
}
