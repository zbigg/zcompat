/*
 * File:      mfifo.c
 * Id:        $Id$
 * Created:   23-09-2004
 *
 * Copyright(C)  Zbigniew Zagorski 2004. All rights reserved.
 * 
 * This program is distributed as free software under the
 * license in the file "LICENSE", which is included in the distribution.
 */
 
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/ztools.h"
#include "zcompat/zthread.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
/* 
   mfifo methods 
*/
static ZSTREAM	mfifo_open	(ZSTREAM,const char*,int);
static int	mfifo_close	(ZSTREAM);
static int	mfifo_read	(ZSTREAM,void*,size_t);
static int	mfifo_write	(ZSTREAM,const void*,size_t);
static zoff_t	mfifo_seek	(ZSTREAM,zoff_t,int);
static long	mfifo_ctl	(ZSTREAM,int,void*);

static zstream_vtable_t vt_mfifo = {
    500,			/*__ id */
    1,			/*__ data_size */
    mfifo_open,	/*__ open */
    mfifo_close,	/*__ close */
    mfifo_read,	/*__ read */
    mfifo_write,		/*__ write */
    mfifo_seek,	/*__ seek */
    mfifo_ctl,		/*__ ctl */
};
typedef struct mfifo_data {
    sbuffer     buffer;
    int         refcnt;
    size_t      read_pos;
    size_t      write_pos; 
    size_t      capacity; /* capacity of buffer */
    size_t      size;   /* size of data currently in fifo */
    zmutex      lock;
    int         writers_count;
    zmutex      empty_lock;
} mfifo_data;

#define MFIFO_GET_DATA(f)	((mfifo_data *) (f)->data[0] )

#define LOCK() zmt_lock(mfifo->lock, ZMT_INFINITY);
#define UNLOCK() zmt_unlock(mfifo->lock);
static mfifo_data* mfifo_data_new(size_t size)
{
    mfifo_data* mfifo = (mfifo_data* )zpio_calloc(1,sizeof( mfifo_data ));
    sbuffer_init(&mfifo->buffer,size);
    mfifo->lock = zmt_new();
    mfifo->empty_lock = zmt_new();
    mfifo->refcnt = 1;
    mfifo->read_pos = 0;
    mfifo->write_pos = 0;
    mfifo->size = 0;
    mfifo->capacity = mfifo->buffer.size;
    mfifo->writers_count = 0;
    zmt_lock(mfifo->empty_lock,ZMT_INFINITY);
    zmt_unlock(mfifo->lock);
    return mfifo;
}
static void mfifo_data_free(mfifo_data* mfifo)
{
    zmt_free(mfifo->lock);
    zmt_free(mfifo->empty_lock);
    sbuffer_free(&mfifo->buffer);
    zpio_free(mfifo);
}
static ZSTREAM	mfifo_open	(ZSTREAM f,const char* name,int mode)
{
    mfifo_data* mfifo = (mfifo_data* )name;
    if( mfifo == NULL ) {
        mfifo = mfifo_data_new(mode);
    } else
        mfifo->refcnt++;
    f->mode = mode;
    if( mode & ZO_WRITE )
        mfifo->writers_count++;
    f->data[0] = (long)mfifo;
    return f;
}
static int	mfifo_close(ZSTREAM f)
{
    mfifo_data* mfifo = MFIFO_GET_DATA(f);
    if( f->mode & ZO_WRITE )
        mfifo->writers_count--;
    LOCK();
    mfifo->refcnt -= 1;
    if( mfifo->refcnt == 0 ) {
        UNLOCK();
        mfifo_data_free(mfifo);
    } else
        UNLOCK();
    return 0;
}

static int	mfifo_read	(ZSTREAM f,void* xbuf,size_t request)
{
    mfifo_data* mfifo = MFIFO_GET_DATA(f);
    int result;
    int size = request;
    if( ! ( f->mode & ZO_READ) ) {
        f->error = errno = EBADF;
        return -1;
    }
    do {
        if( mfifo->size == 0 ) {
            do {
                if( mfifo->writers_count <= 0 ) {
                    f->eof = 1;
                    result = 0;
                    goto end_no_unlock;
                }
                if( zmt_lock(mfifo->empty_lock,100) == 0 ) {
                    /* 
                       if we've locked it, it mean that
                       somebody written to empty fifo,
                       we must unlock it just right after
                    */
                    zmt_unlock(mfifo->empty_lock);
                }
            } while( mfifo->size == 0 );
        }
        LOCK();

        if( mfifo->size > 0 ) 
             break;        
        /*
                It might happen that after we've locked
                this fifo, somebody already read all contens
                if so, then release lock and wait again until
                not empty.
        */
        UNLOCK();
    } while( 1 );
    if( size > mfifo->size )
        size = mfifo->size;
    result = size;
    if( result <= 0 )
        goto end_ok;
    if( mfifo->read_pos + size > mfifo->capacity ) {
        /* read in two chunks */
        int end_size = mfifo->capacity - mfifo->read_pos;
        int start_size = size - end_size;
        char* b = (char*)xbuf;
        if( end_size > 0 )
            memcpy(b           , mfifo->buffer.t + mfifo->read_pos, end_size );
        if( start_size > 0 )
            memcpy(b + end_size, mfifo->buffer.t                  , start_size);
        mfifo->size -= size;
        mfifo->read_pos = start_size;
    } else {
        /* read in one chunk */
        memcpy(xbuf,mfifo->buffer.t + mfifo->read_pos, size);
        mfifo->read_pos += size;
        mfifo->size -= size;
    }
    if( mfifo->size == 0 )
        zmt_lock(mfifo->empty_lock,ZMT_INFINITY);
end_ok:
    UNLOCK();
end_no_unlock:
    return result;
}

static int	mfifo_write(ZSTREAM f,const void* buf,size_t request)
{
    mfifo_data* mfifo = MFIFO_GET_DATA(f);
    int size = request;
    int result = size;
    if( ! ( f->mode & ZO_WRITE) ) {
        f->error = errno = EBADF;
        return -1;
    } 
    LOCK();
    {
        int free_capacity = mfifo->capacity - mfifo->size;
        int do_unlock_empty = 0;
        if( free_capacity < size ) {
            size_t delta = size - free_capacity;

            if( sbuffer_resize(&mfifo->buffer, mfifo->capacity + delta ) < 0 ) {
                UNLOCK();
                return -1;
            }
            mfifo->capacity = mfifo->buffer.size;
        }
        if( mfifo->size == 0 )
            do_unlock_empty = 1;
        if( mfifo->write_pos + size > mfifo->capacity ) {
            /* write in two chunks */
            int end_size = mfifo->capacity - mfifo->write_pos;
            int start_size = size - end_size;
            const char* b = (const char*)buf;
            
            if( end_size > 0 )
                memcpy( mfifo->buffer.t + mfifo->write_pos,b       , end_size );
            if( start_size > 0 )
                memcpy( mfifo->buffer.t                   ,b + size,start_size);
            mfifo->write_pos = start_size;
            mfifo->size += size;
        } else {
            /* write in one chunk */
            memcpy( mfifo->buffer.t + mfifo->write_pos, buf, size );
            mfifo->write_pos += size;
            mfifo->size += size;
        }
        if( do_unlock_empty  )
            zmt_unlock(mfifo->empty_lock);

    }
    UNLOCK();

    return result;
}

static zoff_t	mfifo_seek	(ZSTREAM f,zoff_t offset,int whence)
{
    errno = EBADF;
    return -1;
}

static long	mfifo_ctl	(ZSTREAM f,int what,void* data)
{
    mfifo_data* mfifo = MFIFO_GET_DATA(f);
    switch( what) {
    case ZFCTL_DUP:
        return (long)zcreat(&vt_mfifo, (const char*)mfifo, f->mode);
    default:
        errno = EINVAL;
        return -1;
    }
}


int     zpio_mfifo_new(size_t size,ZSTREAM* reader, ZSTREAM* writer)
{
    mfifo_data* mfifo = mfifo_data_new(size);
    if( mfifo == NULL )
        return -1;
    mfifo->refcnt--;
    
    *reader = zcreat(&vt_mfifo,(const char*)mfifo,ZO_READ);
    
    *writer = zcreat(&vt_mfifo,(const char*)mfifo,ZO_WRITE);
    if( ! *reader || ! *writer ) {
        zclose(*writer);
        zclose(*reader);
        mfifo_data_free(mfifo);
        return -1;
    }
    return 0;
}

