/*
* File:		zscanf.c
*
* Id:           $Id: zscanf.c,v 1.4 2003/02/26 18:28:40 trurl Exp $
*
* Version:	0.1
* Date:		29.01.2002
*
* Project:	ZBIG Portable Input/Output library.
*
* Decription:	zscanf function and derivates

  This is file is under construction, and functions doesn't work
  properly.

*
* Author:	Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2001-2003,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zpio.h"

#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* Exported functions */
int zscanf(const char* fmt,...);
int zfscanf(ZSTREAM in,const char* fmt,...);
int zsscanf(const char* buf,const char* fmt,...);

int zvscanf(const char* fmt,va_list ap);
int zvfscanf(ZSTREAM in,const char* fmt,va_list ap);
int zvsscanf(const char* buf,const char* fmt,va_list ap);

typedef struct zvscanner_rec {	/* TODO: -> to header */
    char*   s;
    char*   c;
    char*   e;

    void*   param;
    int	    (*read_func)(struct zvscanner_rec* r);
} zvscanner_rec;

int zvscanner(zvscanner_rec* vsr, const char* fmt,va_list ap);



/* Implementation */
int zscanf(const char* fmt,...)
{
    va_list ap;
    int r;

    va_start(ap,fmt);
    r = zvfscanf(zstdin,fmt,ap);
    va_end(ap);

    return r;
}

int zfscanf(ZSTREAM in,const char* fmt,...)
{
    va_list ap;
    int r;

    va_start(ap,fmt);
    r = zvfscanf(in,fmt,ap);
    va_end(ap);
    return r;
}
int zsscanf(const char* buf,const char* fmt,...)
{
    va_list ap;
    int r;

    va_start(ap,fmt);
    r = zvsscanf(buf,fmt,ap);
    va_end(ap);
    return r;
}


/* This is very simple version of getc.
*/
static int file_read_func(zvscanner_rec* r)
{
    char a = zfgetc( (ZSTREAM)r->param );
    if( a <= 0 ) return -1;
    r->s[0] = a;
    r->c = r->s;
    r->e = r->c + 1;
    return 1;
}

int zvscanf(const char* fmt,va_list ap)
{
    zvscanner_rec vsr;
    char buf[1];
    vsr.s = buf;
    vsr.c = buf;
    vsr.e = buf;
    vsr.param = (void*)zstdin;
    vsr.read_func = file_read_func;
    return zvscanner(&vsr,fmt,ap);
}
int zvfscanf(ZSTREAM in,const char* fmt,va_list ap)
{
    zvscanner_rec vsr;
    char buf[1];
    vsr.s = buf;
    vsr.c = buf;
    vsr.e = buf;
    vsr.param = (void*)zstdin;
    vsr.read_func = file_read_func;
    return zvscanner(&vsr,fmt,ap);
}
int zvsscanf(const char* buf,const char* fmt,va_list ap)
{
    zvscanner_rec vsr;
    vsr.s = (char*)buf;
    vsr.c = (char*)buf;
    vsr.e = (char*)buf + strlen(buf);
    vsr.param = 0;
    vsr.read_func = 0;
    return zvscanner(&vsr,fmt,ap);
}

#define INP_GET_CHAR(c,cp,ep,buf)	    \
{				    \
    if( cp >= ep ) {		    \
	if( buf->read_func && buf->read_func(buf) >= 0) { \
	    cp = buf->c;    \
	    ep = buf->e;    \
	    c = *cp++;	    \
	} else		    \
	    c = 0;	    \
    } else		    \
	c = *cp++;	    \
}

#define INP_UNGETC(c,cp,buf)\
{			    \
    *--cp  = c;		    \
}
#define NUM( c )			( c - '0' )

#define STR_TO_DEC( str, num )		\
    num = NUM( *(str)++ ) ;		\
    while ( isdigit( *(str) ) )		\
    {					\
	num *= 10 ;			\
	num += NUM( *(str)++ ) ;	\
    }


#ifdef ZSYS_WIN
typedef __int64		    longlong_t;
typedef unsigned __int64    ulonglong_t;
#else
typedef long long	    longlong_t;
typedef unsigned long long  ulonglong_t;
#endif

int zvscanner(zvscanner_rec* vsr, const char* fmt,va_list ap)
{
    union {
	char	vchar;
	short	vshort;
	int	vint;
	long int vlong;
	longlong_t vlonglong;

    	unsigned char	vuchar;
	unsigned short int vushort;
	unsigned int	vuint;
	unsigned long int vulong;
	ulonglong_t vulonglong;

	float vfloat;
	double vdouble;
	long double vlongdouble;
#if defined(HAVE_FULL_INTMAX_IMPL)
	intmax_t    vintmax;
	uintmax_t    vuintmax;
#endif
    } t;
    union {
	char	*vchar;
	short	*vshort;
	int	*vint;
	long int *vlong;
	longlong_t *vlonglong;

    	unsigned char	    *vuchar;
	unsigned short int *vushort;
	unsigned int	    *vuint;
	unsigned long int   *vulong;
	ulonglong_t	    *vulonglong;

	float *vfloat;
	double *vdouble;
	long double *vlongdouble;

#if defined(HAVE_FULL_INTMAX_IMPL)
	intmax_t    *vintmax;
	uintmax_t    *vuintmax;
#endif
    } p;
    char    str_buf[256];
    char*   errp;
    char*   cp;
    char*   ep;
    int	    c;
    int	    npr = 0;
    int	    nchr = 0;

    int	    prec,
	large,
	no_assign,
	done_flag,
	unsigned_flag,
	base,
	intmax_flag,
	long_double_flag,
	type_flag;

    int	    int_val;
    int*    p_int;

    cp = vsr->c;
    ep = vsr->e;
#ifdef NDEBUG
    zuprintf(ZUMODE_ERR,"This program called not ready function zvscanner from zpio library\n");
    exit(1);
#endif
    while( *fmt ) {
	/* FMT: ignore initial whitespace */
	while( *fmt && isspace(*fmt) ) fmt++;
	if( *fmt == '%') {
	    fmt++;
	    prec = -1;
	    unsigned_flag =
		done_flag =
		no_assign =
		large =
		intmax_flag =
		long_double_flag =
		type_flag = 0;

	    while( !done_flag ) {
		if( isdigit(*fmt ) ) {
		    STR_TO_DEC(fmt,prec);
		} else {
		    switch( *fmt ) {
		    /* MODIFIERS */
		    case '%':
			{
			    INP_GET_CHAR(c,cp,ep,vsr);
			    nchr++;
			    if( c != *fmt) {
				INP_UNGETC(c,cp,vsr);
				return npr;
			    }
			}
			done_flag++;
			break;
		    case '[':
			type_flag = 4;
			done_flag++;
			break;
		    case '*':
			no_assign++;
			fmt++;
			break;
		    case 'h':	/* short */
			large--;
			break;
		    case 'l':	/* long */
			large++;
			fmt++;
			break;
		    case 'L':	/* long double */
			long_double_flag++;
			break;
		    case 'z':
			large=1;
			break;
		    case 't':
			large=1;
			break;
		    case 'j':	/* intmax_t || uintmax_t */
			intmax_flag++;
			break;

		    /* SPECIAL */
		    case 'n':
			done_flag++;
			break;
		    case 'p':
			done_flag++;
			break;

		    /* STRINGS */
		    case 'c':
			done_flag++;
			type_flag=2;
			break;
		    case 's':
			type_flag=3;
			done_flag++;
			break;

		    /* INTEGER */
		    case 'd':
			base = 10;
			done_flag++;
			break;
		    case 'i':
			base = 0;
			done_flag++;
			break;
		    case 'u':
			unsigned_flag++;
			done_flag++;
			break;
		    case 'o':
			unsigned_flag++;
			done_flag++;
			base = 8;
			break;
		    case 'X':
		    case 'x':
			unsigned_flag++;
			base = 16;
			done_flag++;
			break;
		    /* FLOATING */
		    case 'a':case 'A':
		    case 'e':case 'E':
		    case 'f':case 'F':
		    case 'g':case 'G':
			type_flag++;
			done_flag++;
			break;
		    default:
			break;
		    } /* END of: switch (*fmt ) */
		} /* END of: else of isidigit(*fmt) */
	    } /* END of:  while( !done_flag )	*/

	    switch( *fmt ) {
	    case '%':
		fmt++;
		/* '%' was matched and we can continue */
		continue;
	    case 'n':
		fmt++;
		p_int = va_arg(ap,int*);
		if( p_int )
		    *p_int = nchr;
		continue;
	    }

	    /*
		Flags valid and significant here:

		no_assign
		    no assignment is made
		large : integer of width
		    = -2	char
		    = -1	short
		    = 0		int
		    = 1		long
		    > =2	long long
		intmax_flag
		    it's intmax_t
		base =
		    the base arg for strtol
		float_flag
		    that it's float
		long_double_flag
		    then it's long double
		large = 1
		    then it's double
		large = 0
		    then it's float
	    */
	    switch( type_flag ) {
	    case '0': /* Integers */
		{
/*
Windows has :
    __int64 __cdecl _atoi64(const char *);
Others should have:
    long long int strtoll(const char*, char**, int base)
*/
		    /*
			This is hack because most of compilers
			currently doesn't support (u)intmax_t
			and strto(u)imax
		    */


#if !defined HAVE_FULL_INTMAX_IMPL
		    if( intmax_flag ) large = 2;
#else
		    if( intmax_flag ) {
			if( unsigned_flag)
			    p.vuintmax = av_arg(ap,uintmax_t)
			else
			    p.vintmax = va_arg(ap,inrmax_t);
		    }
#endif

		    if( unsigned_flag ) {
/*			t.vulonglong = strto*/
		    }
		}
		break;
	    case '1': /* Floats */
		{

		}
		break;
	    case '2': /* n characters */
		{
		}
		break;
	    case '3': /* string until first whitespace */
		{
		}
		break;
	    case '4': /* string matched with [ .. ] */
		{
		}
		break;
	    } /* END of : switch( type_flag ) */
	} /* END OF: if ( *fmt == '%' ) */
	else if( *fmt != '\0') {
	    INP_GET_CHAR(c,cp,ep,vsr);
	    nchr++;
	    if( !c || c != *fmt ) {
		INP_UNGETC(c,cp,vsr);
		return npr;
	    }
	}
	fmt++;
    }
    return npr;
}
