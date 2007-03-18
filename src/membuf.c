/*
* File: membuf.c
*
* Id:           $Id: membuf.c,v 1.3 2003/02/26 18:28:40 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:	Memory buffer I/O
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2002,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
* To do:
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "zcompat/zpio.h"
#include "zcompat/zprintf.h"
#include "zpio_i.h"

/* Memory buffer */
ZSTREAM zmembuf_open	(ZSTREAM,const char*,int);
int	zmembuf_close	(ZSTREAM);
int	zmembuf_read	(ZSTREAM,void*,size_t);
int	zmembuf_write	(ZSTREAM,const void*,size_t);
zoff_t	zmembuf_seek	(ZSTREAM,zoff_t,int);
long	zmembuf_ctl	(ZSTREAM,int,void*);

zstream_vtable_t zvt_membuf = {
    6,			/*__ id */
    5,			/*__ data_size	TODO ! change this */
    zmembuf_open,	/*__ open */
    zmembuf_close,	/*__ close */
    zmembuf_read,	/*__ read */
    zmembuf_write,	/*__ write */
    zmembuf_seek,	/*__ seek */
    zmembuf_ctl,	/*__ ctl*/
};


typedef struct _zmb_init {
    char*	buffer;
    size_t	capacity;
    size_t	datasize;
    int 	flags;
} zmb_init_t;

/* ------------------
	     PUBLIC
	    -----------------*/

ZSTREAM zmbopen(int capacity,int datasize,void* buffer,int mode,int flags)
{
    zmb_init_t a;
    a.capacity = capacity;
    a.datasize = datasize;
    a.buffer = buffer;
    a.flags = flags;
    return zcreat(&zvt_membuf,(const char*)&a,mode);
}

ZSTREAM zstropen(char* str,int size,int mode,int flags)
{
    zmb_init_t a;
    a.capacity = size;
    a.datasize = strlen(str);
    a.buffer = str;
    a.flags = flags;
    return zcreat(&zvt_membuf,(const char*)&a,mode);
}

/* ----------------------====*****	MEMBUF
*/
#define zmb_buffer_s	( (sbuffer*)& (f->data[0]) )

#define zmb_buffer	( (char*)  f->data[0] )
#define zmb_capacity	( (size_t) f->data[1] )
#define zmb_datasize	( (size_t) f->data[2] )
#define zmb_flags	( (size_t) f->data[3] )
#define zmb_pointer	( (size_t) f->data[4] )

#define zmbs_buffer(a)	( f->data[0] ) = (unsigned long)(a)
#define zmbs_capacity(a)( f->data[1] ) = (unsigned long)(a)
#define zmbs_datasize(a)( f->data[2] ) = (unsigned long)(a)
#define zmbs_flags(a)	( f->data[3] ) = (unsigned long)(a)
#define zmbs_pointer(a)	( f->data[4] ) = (unsigned long)(a)

#define zmb_is_dynamic	( (zmb_flags & ZMB_DYNAMIC) == ZMB_DYNAMIC )
#define zmb_is_growable ( ( (zmb_flags & ZMB_GROWABLE) == ZMB_GROWABLE) && zmb_is_dynamic )


static int mb_count = 0;

ZSTREAM zmembuf_open	(ZSTREAM f,const char* name,int mode)
{
    zmb_init_t* is = (zmb_init_t*)name;
    f->mode = mode;

    zmbs_flags(is->flags);
    zmbs_buffer(is->buffer);
    zmbs_capacity(is->capacity);
    zmbs_datasize(is->datasize);
    zmbs_pointer(0);

    if( zmb_buffer == NULL ) {
	if( zmb_is_dynamic && zmb_capacity > 0 ) {
	    zmbs_buffer(zpio_malloc(zmb_capacity));
	    if( zmb_buffer == NULL )
		return NULL;
	    zmbs_datasize(0);
	} else {
	    errno = EINVAL;
	    return NULL;
	}
    }

    f->name = zsaprintf(NULL,0,"membuf://%i",mb_count++);
    ZFL_SET(f->flags,ZSF_FILE);

    if ( (f->mode & ZO_APPEND) == ZO_APPEND) {
	zmbs_pointer(zmb_datasize);
    }

    if ( (f->mode & ZO_TRUNC) == ZO_TRUNC) {
	zmbs_pointer(zmbs_datasize(0));
    }

    return f;
}

int	zmembuf_close	(ZSTREAM f)
{
    if( zmb_is_dynamic && zmb_buffer != NULL ) {
	zpio_free(zmb_buffer);
    }
    return 0;
}
int	zmembuf_read	(ZSTREAM f,void* buf,size_t size)
{
    if( buf == NULL ) {
	f->error = errno = EINVAL;
    } else {
	if( zmb_pointer + size > zmb_datasize ) {
	    size = zmb_datasize - zmb_pointer;
	    f->eof = 1;
	}
	if( size > 0 )
	    memcpy(buf,zmb_buffer+zmb_pointer,size);
	zmbs_pointer(zmb_pointer + size);
	return size;
    }
    return -1;
}

int	zmembuf_write	(ZSTREAM f,const void* buf,size_t size)
{
    if( buf == NULL ) {
	f->error = errno = EINVAL;
    } else {
	if ( ZFL_ISSET(f->mode,ZO_APPEND) ) {
	    zmbs_pointer(zmb_datasize);
	}
	if( zmb_pointer + size > zmb_capacity ) {
	    if( zmb_is_growable ) {
		do {
		    char* tmp = zpio_realloc( zmb_buffer, zmb_capacity * 2 );
		    if( tmp == NULL ) {
			size = zmb_capacity  - zmb_pointer;
			break;
		    } else {
			zmbs_buffer(tmp);
			memset(zmb_buffer + zmb_capacity  , 0 , zmb_capacity );
			zmbs_capacity(zmb_capacity*2);
		    }
		} while ( zmb_pointer + size > zmb_capacity );
	    } else
		size = zmb_capacity - zmb_pointer;
	}
	if( size > 0 ) {
	    memcpy(zmb_buffer+zmb_pointer,buf,size);
	} else {
	    f->error = errno = ENOSPC;
	    return -1;
	}
	zmbs_pointer(zmb_pointer + size);
	if( zmb_datasize <= zmb_pointer ) {
	    zmbs_datasize(zmb_pointer);
	    f->eof = 1;
	}
	return size;
    }
    return -1;
}
zoff_t	zmembuf_seek	(ZSTREAM f,zoff_t offset,int whence)
{
    switch( whence ) {
    case ZSEEK_CUR:
	zmbs_pointer( zmb_pointer + offset );
	break;
    case ZSEEK_END:
	zmbs_pointer( zmb_datasize + offset );
	break;
    case ZSEEK_SET:
	zmbs_pointer( offset );
	break;
    default:
	f->error = errno = EINVAL;
	return (zoff_t)-1;
    }
    return zmb_pointer;
}

long	zmembuf_ctl	(ZSTREAM f,int what,void* data)
{
    if( ZFCTL_EOF == what ) {
	return f->eof = (zmb_pointer == zmb_datasize);
    }
    f->error = errno = EINVAL;
    return -1;
}

extern int	zmbtrunc(ZSTREAM f)
{
    if( f == NULL )
	return ZR_ERROR;
    zmbs_datasize(zmb_pointer);
/*    zpio_realloc(zmb_buffer,zmb_datasize); */
    return 0;
}
