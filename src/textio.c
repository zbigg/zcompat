/*
* File: textio.c
*
* Id:           $Id: textio.c,v 1.3 2003/02/26 18:28:40 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  text input/output functions
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* std */
#include <stdlib.h>

#include "zcompat/zsystype.h"
#include <errno.h>
#include <string.h>
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

int	zungetc_grow(int** top, int** start, unsigned* cap);	/* zpio.c */
void	zungetc_shrink(int** top, int** start, unsigned* cap);  /* zpio.c */
#if 0
int    zfgets(ZFILE* fdesc,char* buf,int len)
{
    int i = 0,n = 0;
    char c = 0;
    if( buf == NULL ) {
        do {
	    int r = zread(fdesc,&c,1);
	    if( r <= 0 ) {
		if( n > 0 )
		    return n;
		fdesc->error = (r < 0) ? errno : 0;
		return -1;
	    }
	    n++;
	} while( c != '\n' );
        return n;
    }
    buf[0] = 0;
    do {
	int r = zread(fdesc,&c,1);
	if( r <= 0 ) {
	    if( n > 0 )
		return n;
	    fdesc->error = (r < 0) ? errno : 0;
	    return -1;
	}
	n++;
	if( c == '\n' )
	    break;
	buf[  i] = c;
	buf[++i] = 0;
    } while( i < len );
    if( buf[i-1] == '\r' ) {
	buf[i-1] = 0;
    }
    return n;
}
#else
int    zfgets(ZFILE* fdesc,char* buf,int len)
{
    int readed = 0,e;
    if( buf == NULL ) {
	errno = EINVAL;
	return -1;
    }
    buf[0] = 0;
    readed = _zread(fdesc,buf,len-1,ZRDT_CHAR,(void*) '\n' );
    if( readed <= 0 ) {
	return -1;
    }
    buf[readed] = '\0';

    if( buf[readed-1] == '\r' )
	buf[readed-1] = 0;

    e  = zfgetc(fdesc);
    if( e != '\n' ) {
	if( readed == 0 && e == EOF ) {
	    fdesc->error = 0;
	    return -1;
	}
	zungetc(fdesc,e);
    }
    else {
	readed++;
    }
    return readed;
}
#endif


int     zreads(char* buf,int len,ZFILE* fdesc)
{
    int i = 0;
    char c;
    int  r;

    r = zread(fdesc,&c,1);
    buf[0] = 0;
    len--;
    while(r > 0 && i<len && c!='0') {
        buf[i] = c;
        buf[++i] = 0;
        r = zread(fdesc,&c,1);
    }
    if( i > 0)
        return i;
    return r;
}

int	zputc(int c)
{
    unsigned char x = (unsigned char)c;
    return zwrite(zstdout,&x,sizeof(x));
}
int	zputs(const char* buf)
{
    int len = strlen(buf);
    return zwrite(zstdout,buf,len);
}
int	zfputc(ZSTREAM s,int c)
{
    unsigned char x = (unsigned char)c;
    return zwrite(s,&x,sizeof(x));
}
int	zfputs(ZSTREAM s,const char* buf)
{
    int len = strlen(buf);
    return zwrite(s,buf,len);
}

int    zgets(char* buf,int len)
{
    return zfgets(zstdin,buf,len);
}

int    zfgetc(ZSTREAM f)
{
    char x;
    int a;
    a = zread(f,&x,sizeof(x));
    if( a < 0 ) {
	return a;
    } else if( a > 0 ) {
	return (unsigned char)x;
    }
    return -1;
}

int	zungetc(ZSTREAM f,int c)
{
    if( f != NULL ) {
#ifndef ZPIO_USE_UNGETC_STACK
	if( (unsigned)(f->ungetc_top - f->ungetc_start) >= f->ungetc_capacity)
	    if( zungetc_grow(& f->ungetc_top, & f->ungetc_start, & f->ungetc_capacity) < 0 )
		return -1;
	*(f->ungetc_top++) = c;
#else
	sstack_push(& f->ungetc_stack,&c,sizeof(c));
#endif
    }
    return 0;
}
