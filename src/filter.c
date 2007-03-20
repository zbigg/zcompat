/*
* File: filter.c
*
* Id:           $Id: filter.c,v 1.4 2003/06/28 11:57:44 trurl Exp $
*
*************************************************

    !!!!      !!!!   !!!!!!!   !!!!!!!!!!!!
    !!!        !!!     !!!       !!!     !!!
    !!!        !!!     !!!       !!!     !!!
    !!!   !!   !!!     !!!       !!!!!!!!!!
    !!! !!!!!! !!!     !!!       !!!
    !!!!!!  !!!!!!     !!!       !!!
    !!!!      !!!!   !!!!!!!   !!!!!!!

	Work	      In	 Progress
*************************************************
*
* Project:	Portable I/O library
*
* Description:  Filters .. fbsdjkhflhsdfghsdkghjk wrrrrrrrr
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

#include <stdlib.h>
#include "zcompat/zsystype.h"
#include "zcompat/zpio.h"
#include "zpio_i.h"

#include <stdlib.h>
#include <string.h>

/* FILTERS streams - fopen ... */
ZSTREAM	zfilter_open	(ZSTREAM,const char*,int);
int	zfilter_close	(ZSTREAM);
int	zfilter_read	(ZSTREAM,void*,size_t);
int	zfilter_write	(ZSTREAM,const void*,size_t);
zoff_t	zfilter_seek	(ZSTREAM,zoff_t,int);
long	zfilter_ctl	(ZSTREAM,int,void*);

zstream_vtable_t zvt_filter =
{
    3,			/*__ id		*/
    1,			/*__ data_size	*/
    NULL,		/*__ open	*/
    zfilter_close,	/*__ close	*/
    zfilter_read,	/*__ read	*/
    zfilter_write,	/*__ write	*/
    zfilter_seek,	/*__ seek	*/
    zfilter_ctl,	/*__ ctl	*/
};

ZSTREAM zfilter_creat(
    ZSTREAM f,
    int (*f_read)(ZSTREAM,void*,size_t),
    int (*f_write)(ZSTREAM,const void*,size_t),
    int data_size
    )
{
    zstream_vtable_t* t = (zstream_vtable_t*) zcompat_malloc(sizeof(zstream_vtable_t));
    if( t == NULL )
	return NULL;
    memcpy(t,&zvt_filter,sizeof(zstream_vtable_t));

    if( f_read != NULL )
	t->vt_read = f_read;
    if( f_write != NULL )
	t->vt_write = f_write;
    t->vt_data_size = data_size;

    {
	ZSTREAM x = zcreat(t,NULL,0);
	if( x != NULL )
	    x->parent = f;
	return x;
    }
}

int	zfilter_close	(ZSTREAM f)
{
    if( f->vtable != NULL )
	zpio_free(f->vtable);
    return 0;
}
int	zfilter_read	(ZSTREAM f,void* buf,size_t size)
{
    return zread(f->parent,buf,size);
}
int	zfilter_write	(ZSTREAM f,const void* buf,size_t size)
{
    return zwrite(f->parent,buf,size);
}
zoff_t	zfilter_seek	(ZSTREAM f,zoff_t offset,int whence)
{
    return zseek(f->parent,offset,whence);
}
long	zfilter_ctl	(ZSTREAM f,int what,void* data)
{
    return zfctl(f->parent,what,data);
}
