/*
* File: chain.c
*
* Id:           $Id: chain.c,v 1.2 2003/06/28 11:57:44 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  chain I/O  routines
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2003
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "zcompat/zpio.h"
#include "zpio_i.h"


ZSTREAM	zpio_chain_new(int n,int mode);
int	zpio_chain_add(ZSTREAM chain, ZSTREAM s, size_t position);


/* chain streams - fopen ... */
static ZSTREAM	zchain_open	(ZSTREAM,const char*,int);
static int	zchain_close	(ZSTREAM);
static int	zchain_read	(ZSTREAM,void*,size_t);
/*static int	zchain_write	(ZSTREAM,const void*,size_t);*/
static zoff_t	zchain_seek	(ZSTREAM,zoff_t,int);
static long	zchain_ctl	(ZSTREAM,int,void*);

static zstream_vtable_t zvt_chain = {
    500,			/*__ id */
    1,			/*__ data_size */
    zchain_open,	/*__ open */
    zchain_close,	/*__ close */
    zchain_read,	/*__ read */
    NULL,		/*__ write */
    zchain_seek,	/*__ seek */
    zchain_ctl,		/*__ ctl */
};

typedef struct zstream_chain {
    ZSTREAM* table;
    size_t  count;
    size_t  capacity;

    unsigned	    tpos;
} zstream_chain;

#define VT_ID_CHAIN 500
#define CHECK_TYPE_INT \
    if( f->vtable->vt_id != VT_ID_CHAIN ) {  \
    f->error = EBADF;			    \
    return -1;		\
}
#define CHECK_TYPE_PTR \
    if( f->vtable->vt_id != VT_ID_CHAIN ) {  \
    f->error = EBADF;	\
    return NULL;    \
}

#define SET_CHAIN(a)  (f->data[0]) = (long)(void*)(a)
#define GET_CHAIN()   ((zstream_chain*)(f->data[0]))
static int  resize(zstream_chain* s,unsigned new_size)
{
    ZSTREAM* tmp;
    if( new_size == s->capacity ) return new_size;
    if( new_size < s->count ) {
	int i;
	for( i = s->count-1; i >= (int)new_size ; i-- ) 
	    if( s->table[i] ) {
		zclose(s->table[i]);
		s->count--;
	    }
	s->tpos = zmin(new_size,s->tpos);
    }
    tmp = zpio_realloc(s->table, new_size * sizeof(ZSTREAM));
    if( !tmp ) return -1;

    s->table = tmp;
    if( new_size > s->capacity ) {
	memset(s->table + s->capacity, 0, (new_size - s->capacity ) * sizeof( ZSTREAM ));
    }
    s->capacity = new_size;
    return new_size;
}

static ZSTREAM	zchain_open	(ZSTREAM f,const char* name,int mode)
{
    zstream_chain* s = zpio_calloc(1,sizeof( zstream_chain ));
    int n = (int)name;
    if( resize(s,n) < 0 )
	return NULL;
    SET_CHAIN(s);
    return f;
}
static int	zchain_close(ZSTREAM f)
{
    zstream_chain* chain = GET_CHAIN();
    if( chain ) 
	resize(chain,0);
    return 0;
}
static int	zchain_read	(ZSTREAM f,void* xbuf,size_t size)
{
    zstream_chain* chain = GET_CHAIN();
    size_t readed = 0;
    char* buf = (char*)xbuf;
    CHECK_TYPE_INT;
    while( chain->tpos < chain->count && size > 0 ) {
	int r = zread(chain->table[chain->tpos],buf,size);
	if( r < 0 )
	    f->error = chain->table[chain->tpos]->error;
	size -= r;
	readed += r;
	buf += r;
	if( size > 0 )
	    chain->tpos++;
    }
    if( size > 0 && chain->tpos == chain->count )
	f->eof = 1;
    return readed;
}
/*
static int	zchain_write(ZSTREAM f,const void* buf,size_t size)
{
    zstream_chain* chain = GET_CHAIN();
    CHECK_TYPE_INT;
    return -1;

}
*/
static zoff_t	zchain_seek	(ZSTREAM f,zoff_t offset,int whence)
{
    //zstream_chain* chain = GET_CHAIN();
    //CHECK_TYPE_INT;
    return -1;
}

static long	zchain_ctl	(ZSTREAM f,int what,void* data)
{
    //zstream_chain* chain = GET_CHAIN();
    //CHECK_TYPE_INT;
    return -1;
}

ZSTREAM	zpio_chain_new(int n,int mode)
{
    int readf	= (mode & ZO_RD) == ZO_RD;
    int writef	= (mode & ZO_WR) == ZO_WR;

    if( (!readf && !writef) || (readf && writef) ) {
	errno = EINVAL;
	return NULL;
    }
    return zcreat(&zvt_chain,(const char*)n,mode);
}

int	zpio_chain_add(ZSTREAM f, ZSTREAM s, size_t position)
{
    zstream_chain* chain = GET_CHAIN();    
    CHECK_TYPE_INT;
    if( chain->count == chain->capacity ) 
	if( resize(chain,chain->capacity + 2 ) < 0 )
	    return -1;
    chain->table[chain->count] = s;
    zseek(s,position,ZSEEK_SET);
    chain->count++;
    return 0;
}
