/*

* File:	    dstream.c
*
* Id:           $Id: dstream.c,v 1.4 2003/06/28 11:57:44 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  stream with which redirects output/input ti dirrefernt
		streams
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2000,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
* To do:
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zsystype.h"
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

char*	zst_strdup2(const char* s1,const char* s2);

/* Different input & output stream */
ZSTREAM	zdstream_open	(ZSTREAM,const char*,int);
int	zdstream_read	(ZSTREAM,void*,size_t);
int	zdstream_write	(ZSTREAM,const void*,size_t);
int	zdstream_close	(ZSTREAM);
long	zdstream_ctl	(ZSTREAM,int,void*);


zstream_vtable_t zvt_dstream = {
    2,			/*__ id */
    2,			/*__ data_size */
    zdstream_open,	/*__ open */
    zdstream_close,	/*__ close */
    zdstream_read,	/*__ read */
    zdstream_write,	/*__ write */
    0,			/*__ seek */
    zdstream_ctl,	/*__ ctl */
};

ZSTREAM zdscreat(ZSTREAM in,ZSTREAM out)
{
    ZSTREAM f[2];
    f[0] = in;
    f[1] = out;
    return zcreat(& zvt_dstream, (const char*) f, 0 );
}

/* ----------------------====*****      DSTREAM
*/
#define dstr_in		((ZSTREAM) f->data[0])
#define dstr_out	((ZSTREAM) f->data[1])

ZSTREAM	zdstream_open	(ZSTREAM f,const char* name,int mode)
{
    ZSTREAM* s = (ZSTREAM*)name;

    f->data[0] = (long) s[0];
    f->data[1] = (long) s[1];

    return f;
}
int	zdstream_read	(ZSTREAM f,void* buf,size_t size)
{
    int e = zread( dstr_in ,buf,size );
    if( e < 0 )
	f->error = ( dstr_in)-> error;
    return e;
}
int	zdstream_write	(ZSTREAM f,const void* buf,size_t size)
{
    int e =  zwrite( dstr_out ,buf,size);
    if( e < 0 )
	f->error = dstr_out->error;
    return e;
}
int	zdstream_close	(ZSTREAM f)
{
    int err1,
	err2;
    err1 = zclose( dstr_in );
    err2 = zclose( dstr_out );
    return err1 < err2 ? err1 : err2;
}

long	zdstream_ctl	(ZSTREAM f,int what,void* data)
{
    switch( what ) {
    case ZFCTL_DUP:
        {
            ZSTREAM r = NULL,w = NULL;
            if( dstr_in ) {
                r = zdup( dstr_in );
                if( !r ) {
                    f->error = errno;
                    return -1;
                }
            }
            if( dstr_out ) {
                w = zdup( dstr_out);
                if( !w ) {
                    f->error = errno;
                    if( r ) zclose(r);
                    return -1;
                }
            }
            return (long)zdscreat(r,w);
        }
    case ZFCTL_GET_READ_DESC:
	return (int) zfdesc_r ( dstr_in );
    case ZFCTL_GET_WRITE_DESC:
	return (int) zfdesc_w ( dstr_out );
    case ZFCTL_GET_READ_STREAM:
	return (long) dstr_in;
    case ZFCTL_GET_WRITE_STREAM:
	return (long) dstr_out;

    case ZFCTL_SET_READ_STREAM:
	f->data[0] = (long)(ZSTREAM)data;
	return 0;
    case ZFCTL_SET_WRITE_STREAM:
	f->data[1] = (long)(ZSTREAM)data;
	return 0;

    case ZFCTL_GET_BLOCKING:
    case ZFCTL_SET_BLOCKING:
    case ZFCTL_EOF:

	errno = EINVAL;

	return -1;

    }

    errno = EINVAL;

    return -1;

}



