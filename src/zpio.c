/*
* File: zpio.c
*
* Id:           $Id: zpio.c,v 1.3 2003/02/26 18:28:40 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  library core
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2002,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* std */
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>

#include "zcompat/zsystype.h"

/*
	This file should be system independent
	so no system check
*/
#include <string.h>
#include <errno.h>
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

#if WANT_STDIO
#include <stdio.h>
#endif

#ifndef WITH_INPUT_BUFFER
#define WITH_INPUT_BUFFER	1
#endif

#ifndef WITH_OUTPUT_BUFFER
#define WITH_OUTPUT_BUFFER	0
#endif

#define _zpos_in_buf(buf)	( (buf).start - (buf).buffer )
#define _zpos_in_file(buf)	( _zpos_in_buf(buf) + (buf).start_offset )

/* ----------------------====*****      FORWARDS
*/
static int	zst_freerec(ZSTREAM fdesc);
char*		zst_strdup2(const char* s1,const char* s2);

#define ZRDT_NONE	0
#define ZRDT_CHAR	1
#define ZRDT_SPN	2
#define ZRDT_CSPN	3

/*
    This thre real read function.

    et:
	one of ZRDT_xxx
	    ZRDT_CHAR   end_test = (void*) ending_character
	    ZRDT_SPN    end_test = (void*) "accepted characters"
	    ZRDT_CSPN   end_test = (void*) "rejected characters"
*/
int     _zread(ZSTREAM f,char* db,int requested_size,int et,void* end_test);
static int zset_bufsize(struct zstream_buf* buf,int size);

ZSTREAM zstream_ref(ZSTREAM f)
{
    f->ref_count++;
    return f;
}
int     zclose(ZSTREAM f)
{
    f->ref_count--;
    if( f->ref_count > 0 ) return 0;
    /* close dependencies */
    {
	ZSTREAM c;
	while ((c = zf_pop(f)) != NULL )
            zclose(c);
    }
    /* close stream */
    {
	int err = 0;
	if( f != NULL )
	    if( f->vtable->vt_close != NULL  )
		err = f->vtable->vt_close(f);
	zst_freerec(f);
	return err;
    }
}

int	zclosesub(ZSTREAM s,int mode)
{
    int readf	= (mode & ZO_RD) == ZO_RD;
    int writef	= (mode & ZO_WR) == ZO_WR;
    int r = 0;
    if( readf ) {
	ZSTREAM rs = (ZSTREAM)zfctl(s,ZFCTL_GET_READ_STREAM,0);
	if ( rs ) r |= zclose(rs) < 0;
	zfctl(s,ZFCTL_SET_READ_STREAM,NULL);
    }
    if( writef ) {
	ZSTREAM ws = (ZSTREAM)zfctl(s,ZFCTL_GET_WRITE_STREAM,0);
	if ( ws ) r |= zclose(ws) < 0;
	zfctl(s,ZFCTL_SET_WRITE_STREAM,NULL);
    }
    return r ? -1 : 0;
}

ZSTREAM zcreat(zstream_vtable_t* vt,const char* name,int mode)
{
    ZSTREAM f = zcompat_calloc(1, sizeof( zstream_t ) + sizeof(unsigned long) * vt->vt_data_size );
    if( f == NULL )
	return NULL;
    f->vtable = vt;
#ifndef ZPIO_USE_UNGETC_STACK
    f->ungetc_top = NULL;
    f->ungetc_start = NULL;
    f->ungetc_capacity = 0;
#endif
    f->mode = mode;
    f->chain = NULL;
    f->parent = NULL;
    f->ref_count = 1;
    if( f->vtable->vt_open != NULL  ) {
	ZSTREAM nf = f->vtable->vt_open( f, name, mode );
	if( nf != f )
	    zst_freerec(f);
       	return nf;
    }
    return f;
}


/** Get/set stream options.

    @param f		stream
    @param func		operation number (see ZFCTL_xxx macros)
    @param data		optional data for operation
*/
int	zfctl(ZSTREAM f,int func,void* data)
{
    if( f != NULL )
	if( f->vtable->vt_ctl != NULL  )
	    return f->vtable->vt_ctl(f,func,data);
    errno = EINVAL;
    return -1;
}
/** Update input buffer.

    return number of bytes in buffer
    or
    -1 on error
    or 0 on EOF
*/
static int	zupdate_ibuf(ZSTREAM f,struct zstream_buf* buf)
{
    int	is_file = f->flags & ZSF_FILE;
    /* start of buffer which is to be updated */
    void* upd_start = buf->buffer;
    /* size of buffer */
    int	upd_size = buf->buffer_size;

    int tmp = (int)buf->end - (int)buf->start;

    if( is_file ) {
	buf->start_offset = f->offset;
	tmp = 0;
    } else if( tmp > 0 && tmp < buf->buffer_size ) {
	memcpy(buf->start, upd_start, tmp);
	upd_start = (char*)upd_start + tmp;
	upd_size -= tmp;
	buf->start = buf->buffer;
	buf->end = (char*)buf->buffer + tmp;
    } else {
	tmp = 0;
    }

    if( f->vtable->vt_read  != NULL  ) {
	int r = f->vtable->vt_read(f,upd_start,upd_size);
	if( tmp == 0 ) {
	    if( r < 0 )
		return r;
	    if( r == 0 )
		f->eof = 1;
	}
	if( r >= 0) {
	    buf->end = (char*)upd_start + r;
	    buf->start = (char*)upd_start - tmp;
	    if( is_file ) {
		f->real_offset += r;
		buf->end_offset = buf->start_offset + r;
	    }
	}
	return r + tmp;
    } else {
	errno = EBADF;
	return -1;
    }
}
int	zflush(ZSTREAM f)
{
    errno = EBADF;
    return -1;
}

int	zungetc_set_min_size(int** top, int** start, unsigned* cap,int new_size)
{
    unsigned size = *top - *start;
    int* tmp;
    if( *cap != 0 ) {
	*cap = *cap * new_size;
	tmp = (int*)zcompat_realloc( *start, sizeof(int) * *cap );
    } else {
	*cap = new_size;
	tmp = (int*)zcompat_malloc( sizeof(int) * *cap);
    }
    if( tmp == NULL )
	return -1;
    *start = tmp;
    *top = *start + size;
    return 0;
}

int	zungetc_grow(int** top, int** start, unsigned* cap)
{
    unsigned size = *top - *start;
    int* tmp;
    if( *cap != 0 ) {
	*cap = *cap * 2;
	tmp = (int*)zcompat_realloc( *start, sizeof(int) * *cap );
    } else {
	*cap = 2;
	tmp = (int*)zcompat_malloc( sizeof(int) * *cap);
    }
    if( tmp == NULL )
	return -1;
    *start = tmp;
    *top = *start + size;
    return 0;
}

void zungetc_shrink(int** top, int** start, unsigned* cap)
{
    unsigned size = *top - *start;
    int* tmp;
    if( size*2 < *cap ) {
	*cap = *cap / 2;
	tmp = zcompat_realloc( *start, sizeof(int) * *cap );
	if( tmp == NULL ) {
	    *start = NULL;
	    *top = NULL;
	    *cap = 0;
	    return;
	}
	*start = tmp;
	*top = *start + size;
    }
}

int	zread(ZSTREAM f,void* db,int requested_size)
{
    return _zread(f,db,requested_size,ZRDT_NONE,NULL);
}

#define ZRDT_TEST(ch,et,ep) (						\
      (et) == ZRDT_CHAR ? ( (int)(ch) != (int)(ep))			\
    : (et) == ZRDT_SPN  ? ( strchr((const char*)(ep),(ch)) != NULL )	\
    : (et) == ZRDT_CSPN ? ( strchr((const char*)(ep),(ch)) == NULL )	\
    : 1									\
    )

#define ZRDT_MTEST(buf,len, et,ep,acc )	\
    { 					\
	int ix = 0;			\
	acc = 1; 			\
	while( ix < len ) {		\
	    if( !ZRDT_TEST((buf)[ix],et,ep)){ \
		acc = 0;		\
		break;			\
	    }				\
	    ++ix;			\
	}				\
	len = ix;			\
    }

int     _zread(ZSTREAM f,char* db,int requested_size,int et,void* ep)
{
    register int readed = 0;

    if	   ( f == NULL || db == NULL ) {
	errno = EINVAL;
	return -1;
    }

    if( f->chain != NULL && !ZFL_ISSET(f->flags,ZSF_CHAIN ) ) {
	int e;
	ZFL_SET(f->flags,ZSF_CHAIN);
	e = _zread(f->chain,db,requested_size,et,ep);
	ZFL_UNSET(f->flags,ZSF_CHAIN);
	return e;
    }
    /* Get data from "ungetc" buffer if not empty */
    {
	unsigned ungetc_size;
	while( (ungetc_size = f->ungetc_top - f->ungetc_start) > 0
		&& requested_size > 0 ) {
	    register int c = f->ungetc_top[-1];

	    if( et && !ZRDT_TEST(c,et,ep) )
		return readed;

	    --(f->ungetc_top);

	    if( c == EOF )
		return readed;
	    else
		*(db++) = (unsigned char)c;
	    requested_size-=1;
	    readed++;
	}
	if( f->ungetc_capacity > 4 && ungetc_size*2 < f->ungetc_capacity )
	    zungetc_shrink(& f->ungetc_top, & f->ungetc_start, & f->ungetc_capacity);
    } /* end of un"ungetc" */

#if WITH_INPUT_BUFFER
    /* if input BUFFERing is ENABLED then read using buffering */
    if( (f->flags & ZSF_FILE)
		? f->buf.file.enabled
		: f->buf.pipe.in_enabled
	) {
        if( (f->flags & ZSF_FILE) == ZSF_FILE ) {
	    /*##### FILE input buffering */
	    register struct zstream_buf* buf = &f->buf.file.io;
            while( requested_size > 0 ) {
                register int bd_size = (int) buf->end - (int) buf->start;
		int acc = 1;
                if( bd_size == 0 && requested_size*2 > buf->buffer_size ) {
                    /* Input buffer is empty, and we've got quite big block to transfer.
                        Let's do it without buffering. */
                    if( f->vtable->vt_read  != NULL  ) {
                        register int r = f->vtable->vt_read(f,db,requested_size);
                        if( r < 0 )
                            return (readed > 0 ) ? readed : r;
                        else {
			    if( et ) {
				int rr = r;
				ZRDT_MTEST((char*)db,r, et, ep, acc);
				if( r < rr ) {
				    /* We have readed some bytes that were not accepted, so
				       put them to  buffer
				        r  contains accepted length
					rr contains readed length
				    */
				    if( buf->buffer_size < rr-r) {
					if( zset_bufsize(buf,rr-r) < 0 ) {
					    f->error = errno = ENOMEM;
					    return -1;
					}
				    }
				    buf->start = buf->buffer;
				    buf->end = buf->start + rr - r;
				    memcpy(buf->start,(char*)db + r,rr - r);
				    buf->start_offset = f->real_offset + r;
				    buf->end_offset   = f->real_offset + rr;
				    f->real_offset += rr;
				    f->offset += r;
				    return readed + r;
				}
			    }
                            f->real_offset += r;
                            f->offset += r;
                            return r + readed;
                        }
                    } else
                        return readed > 0 ? readed : (errno = EBADF), -1;
                } /* END: Buffer is empty, skip buffering */

		/* Check if file pointer is inside buffered area
		    f->offset	    -	offset in file from where we want to read
		    buf->start
		    buf->end_offset -	start and end of beffered area
		    bd_size	    -	size of buffer
		*/
                if ( f->offset < buf->start_offset ||
                     f->offset > buf->end_offset || bd_size == 0 ) {
                    /* Update buffer. */
                    int r = zupdate_ibuf(f,buf);
                    if( r == 0 )
                        /* Nothing more to read. */
                        return readed;
                    if( r < 0 )
                        /* Error ocurred during updating buffer */
                        return (readed > 0) ? readed : r;
                    bd_size = (int)buf->end - (int)buf->start;
                }       /* END: updateing buffer */
                if( bd_size > requested_size )
                    bd_size = requested_size;
		if( et ) {
		    /* test how many of bytes are acceptable */
		    ZRDT_MTEST((char*)buf->start,bd_size, et, ep, acc);
		}
		switch( bd_size ) {
		case sizeof(char):
		    *(char* )db = *(char* )buf->start;
		    break;
                case sizeof(short):
		    *(short*)db = *(short*)buf->start;
		    break;
                case sizeof(long):
		    *(long* )db = *(long* )buf->start;
		    break;
                default:
		    memcpy(db,buf->start,bd_size);
        	    break;
		}
                requested_size          -= bd_size;
                buf->start              += bd_size;
                db                      += bd_size;
                readed                  += bd_size;
                f->offset               += bd_size;
		if( !acc )
		    return readed;
            } /* while requested_size > 0 */
            return readed;
        }        /* END of file input buffering */
        else
        {
	    /*##### PIPE input buffering */
	    register struct zstream_buf* buf = &f->buf.pipe.in;
            while( requested_size > 0 ) {
                register int bd_size = (int) buf->end - (int) buf->start;
		int acc = 1;
                if( bd_size == 0 ) {
                    /* Buffer is empty. */
                    if( requested_size*2 > buf->buffer_size ) {
                        /* Input buffer is empty, and we've got quite big block to transfer.
                            Let's do it without buffering. */
                        if( f->vtable->vt_read  != NULL  ) {
                            int r = f->vtable->vt_read(f,db,requested_size);
                            if( r < 0 )
                                return (readed > 0 ) ? readed : r;
                            else {
				if( et ) {
				    int rr = r;
				    ZRDT_MTEST((char*)db,r, et, ep, acc);
				    if( r < rr ) {
					/* We have readed some bytes that were not accepted, so
				    	    put them to  buffer
					    r  contains accepted length
					    rr contains readed length
					*/
					if( buf->buffer_size < rr-r) {
					    if( zset_bufsize(buf,rr-r) < 0 ) {
						f->error = errno = ENOMEM;
						return -1;
					    }
					}
					buf->start = buf->buffer;
					buf->end = buf->start + rr - r;
					memcpy(buf->start,(char*)db + r,rr-r);
					return readed + r;
				    }
				}
                                return r + readed;
			    }
                        } else
                            return readed > 0 ? readed : ((errno = EBADF), -1);
                    } else {
                        /* Update buffer. */
                        int r = zupdate_ibuf(f,buf);
                        if( r == 0 )
        	            /* Nothing more to read. */
                            return readed;
                        if( r < 0 )
                            /* Error ocurred during updating buffer */
                            return (readed > 0) ? readed : r;
                        bd_size = (int)buf->end - (int)buf->start;
                    }       /* END: updateing buffer */
                } /* END: Buffer is empty */
                if( bd_size > requested_size )
                    bd_size = requested_size;
		if( et ) {
		    /* test ho much of string is acceptable */
		    ZRDT_MTEST((char*)buf->start,bd_size, et, ep, acc);
		}
		switch( bd_size ) {
		case sizeof(char):
		    {
			char* a = (char*)db;
			*a = *(char* )buf->start;
		    }
		    break;
                case sizeof(short):
		    {
			short *a = (short*)db;
			*a  = *(short*)buf->start;
		    }
		    break;
                case sizeof(long):
		    {
			long* a = (long*)db;
			*a = *(long* )buf->start;
		    }
		    break;
                default:
		    memcpy(db,buf->start,bd_size);
        	    break;
		}
                requested_size  -= bd_size;
                buf->start      += bd_size;
                db              += bd_size;
                readed          += bd_size;
		if( !acc ) return readed;
            } /* while requested_size > 0 */
            return readed;
        } /* END of pipe input buffering */
    } else
#endif
    { /* unbuffered reading */
	if( requested_size > 0 ) {
	    if( f->vtable->vt_read  != NULL  ) {
		int r = f->vtable->vt_read(f,db,requested_size);
		if( r < 0 )
		    return r;
		if( r == 0 )
		    f->eof = 1;
		if( et ) {
		    int acc = 1,rr = r;
		    /* test how much of string is acceptable */
		    ZRDT_MTEST((char*)db,r, et, ep, acc);
		    if( r < rr ) {
			/*
			    We have readeed rr bytes and only r are acceptable
			    and we have buffering turned off. So we must push it
			    into ungetc stack in reverse order.
			*/
			/* Assure that stack is big enough */
			{
			    unsigned usize = f->ungetc_top - f->ungetc_start;
			    if( zungetc_set_min_size(
				& f->ungetc_top, & f->ungetc_start, & f->ungetc_capacity,
				usize + rr - r + 1 ) < 0 ) 
                            { 
                                f->error = errno = ENOMEM; 
                                return -1; 
                            }
			}
			/* copy it to stack */
			{
			    register char* p = (char*)db + rr;
			    while( p > (char*)db + r )
				*(f->ungetc_top)++ = (int) *(--p);
			}
			return readed + r;
		    }
		}
		readed += r;
	    } else {
		errno = EBADF;
		return -1;
	    }
	}
	return readed;
    }
    return -1;
}

int     zwrite(ZSTREAM f,const void* db,int requested_size)
{
    if( f == NULL ) {
	errno = EBADF;
	return -1;
    } else if(db == NULL) {
	errno = EINVAL;
	return -1;
    }
    /* if stream is chained then post data to dependecies */
    if( f->chain != NULL && !ZFL_ISSET(f->flags,ZSF_CHAIN ) ) {
	int e;
	ZFL_SET(f->flags,ZSF_CHAIN);
	e = zwrite(f->chain,db,requested_size);
	ZFL_UNSET(f->flags,ZSF_CHAIN);
	return e;
    }
#if WITH_OUTPUT_BUFFER          /* not ready yet !!!!!!!! */
    if( (f->flags & ZSF_FILE)
	? f->buf.file.enabled
	: f->buf.pipe.out_enabled
	) {
	if( (f->flags & ZSF_FILE) ==  ZSF_FILE ) {
	    /*##### FILE output buffering */
	    register struct zstream_buf* buf = &f->buf.file.io;
	    register int written = 0;
	    while( requested_size > 0 ) {
		register int bd_size = (int) buf->end - (int) buf->start;
	    }
	    return written;
	}   /* END: file version of write buffering */
	else
	{
	    /*##### PIPE output buffering */
	    register struct zstream_buf* buf = &f->buf.pipe.out;
	    register int written = 0;
	    while( requested_size > 0 ) {
		register int bd_size = (int) buf->end - (int) buf->start;
	    }
	    return written;
	}   /* END: pipe version of write buffering */
    } else
#endif
    if( f->vtable->vt_write != NULL  ) {
	int r =  f->vtable->vt_write(f,db,requested_size);
	if( r == 0 )
	    f->eof = 1;
	if( r > 0 )
	    f->real_offset += r;
	return r;
    } else
	errno = EBADF;
    return -1;
}

zoff_t     zseek(ZSTREAM f,zoff_t offset,int whence)
{
    if( f == NULL )
	errno = EBADF;
    else if( f->vtable->vt_seek != NULL ) {
	zoff_t r;
	if( ZFL_ISSET(f->flags, ZSF_FILE ) && f->buf.file.enabled ) {
	    switch( whence ) {
	    case ZSEEK_CUR:
		whence = ZSEEK_SET;
		offset = f->offset + offset;
		break;
	    }
	}
	r = f->vtable->vt_seek(f,offset,whence);
	if( r >= 0) {
	    f->offset = f->real_offset = r;
	    if( f->buf.file.enabled )
		/* give them invalid values */
		f->buf.file.io.start =
		    (char*)f->buf.file.io.buffer + (f->offset - f->buf.file.io.start_offset);
	}
	return r;
    } else
	errno = EBADF;
    return (zoff_t)-1;
}


static int zst_freerec(ZSTREAM fdesc)
{
    if( fdesc == NULL )
        return -1;
    if( fdesc->name != NULL )
	zpio_free(fdesc->name);
    if( fdesc->ungetc_start) {
	zpio_free(fdesc->ungetc_start);
    }
    zpio_free(fdesc);
    return 0;
}

char* zst_strdup2(const char* s1,const char* s2)
{
    char* s;
    int len;
    int a = strlen(s1);
    len = a+strlen(s2)+1;
    s =(char*)zcompat_malloc(len);
    if( s == NULL )
	return NULL;
    strcpy(s,s1);
    strcpy(s+a,s2);
    return s;
}



int zeof(ZSTREAM f)
{
    if( f != NULL )
	return f->eof;
    else
	return -1;
}

const char* zfname(ZSTREAM f)
{
    return (f != NULL) ? f->name : NULL;
}

int	zf_push(ZSTREAM base,ZSTREAM next)
{
    if( base == NULL || next == NULL ) {
	errno = EINVAL;
	return -1;
    }
    if( base->chain == NULL ) {
	next->parent = base;
	base->chain = next;
    } else {
	next->parent = base->chain;
	base->chain = next;
    }
    return 0;
}

ZSTREAM	zf_pop(ZSTREAM base)
{
    if( base == NULL ) {
	errno = EINVAL;
	return NULL;
    }
    if( base->chain == NULL )
	return NULL;
    else {
	ZSTREAM tmp = base->chain;
	if( base->chain->parent != base )
	    base->chain = base->chain->parent;
	else
	    base->chain = NULL;
	tmp->parent = NULL;
	return tmp;
    }
}
/** Enable/disable/get/set input buffer size.
    If size > 0 buffer size is set to requested size (and enabled).
    If size == 0 buffer is disabled.
    If size == -1, nothing is done.

    - returned size of input buffer after call, 0 if is disabled
    or -1 on error.
*/

static int zset_bufsize(struct zstream_buf* buf,int size)
{
    switch( size ) {
    case -1:	/* do nothing */
	break;
    case 0:	/* disable */
	if( buf->buffer ) {
	    zpio_free(buf->buffer);
	    buf->buffer = 0;
	}
	buf->buffer_size = 0;
	buf->end = NULL;
	buf->start = NULL;
	buf->start_offset = 0;
	buf->end_offset = 0;
	break;
    default: {	/* enable and/or change size */
	void* tmp;
	    if( buf->buffer_size == 0 || buf->buffer == NULL ) {
		tmp = zcompat_malloc(size);
		if( !tmp )
		    return -1;
		buf->buffer = tmp;
		buf->buffer_size = size;
		buf->start = tmp;
		buf->end = tmp;
	    } else {
		int x;
		tmp = zcompat_realloc(buf->buffer, size);
		if( !tmp )
		    return -1;
		{
		    x = (int)buf->end - (int)buf->buffer;
		    if( x > size )
		        x = size;
		    buf->end = (char*)tmp + x;
		}
		{
		    x = (int)buf->start - (int)buf->buffer;
		    if( x > size )
		        x = size;
		    buf->start = (char*)tmp + x;
		}
		buf->buffer = tmp;
	        buf->buffer_size = size;
	    }
	break;
	}
    }
    return buf->buffer_size;
}
int	zsetbuf(ZSTREAM f,int isize,int osize)
{
    if( f->flags & ZSF_FILE ) {
	int size = zmax (isize,osize);
	int r = zset_bufsize(& f->buf.file.io, size );
	if( size >= 0 )
	    f->buf.file.enabled = size > 0;
	return r;
    } else {
	int r = zset_bufsize(& f->buf.pipe.in, isize );
	if( isize >= 0 )
	    f->buf.pipe.in_enabled = isize > 0;
	if( osize != -1 )
	    r = zset_bufsize(& f->buf.pipe.out, osize );
	if( osize >= 0 )
	    f->buf.pipe.out_enabled = osize > 0;
	return r;
    }
}
int	zsetbufi(ZSTREAM f,int isize,int osize)
{
    return zsetbuf(f,isize,osize);
}

ZSTREAM zdup(ZSTREAM s)
{
    long i = zfctl(s,ZFCTL_DUP,NULL);
    if( i == -1 ) return NULL;
    return (ZSTREAM)i;
}
