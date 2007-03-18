/*

* File:		stdio.c
*
* Id:           $Id: stdio.c,v 1.3 2003/02/26 18:28:40 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  standard input/output/error streams
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2002,
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
#include "zcompat/zcompat.h"

#ifndef SYSDEP_ZPIO_GET_STD_STREAM
/*
    This is UNIX version where all streams
    are accesible and inherited.
*/
static ZSTREAM*  zpio_streams	= NULL;
static int	zpio_streams_size = 0;

static void zpio_free_std_streams(void);
static int zpio_free_std_streams_atexit_set;

ZSTREAM zpio_get_std_stream(int i)
{
    if( i >= zpio_streams_size ) {
    	zpio_streams = (ZSTREAM*)
	    zpio_realloc(
	    zpio_streams,
	    (i+1) * sizeof( ZSTREAM ));
    	if( zpio_streams == NULL)
    	    return 0;
        memset(
	    zpio_streams + zpio_streams_size,
	    0,
	    (1+i-zpio_streams_size)*sizeof( ZSTREAM ));
        zpio_streams_size = i+1;

	/* atexit THIS */
	if( !zpio_free_std_streams_atexit_set ) {
            zc_atexit(zpio_free_std_streams);
	    zpio_free_std_streams_atexit_set = 1;
	}
    }
    if( zpio_streams[i] != NULL )
    	return zpio_streams[i];
    else {
    	static char buf[30];
        sprintf(buf,"posix://%i",i);
    	return zpio_streams[i] = zdopen(i,buf);
    }
}
void zpio_free_std_streams(void)
{
    int i;
    for(i=0;i < zpio_streams_size;i++)
	if( zpio_streams[i] != NULL )
	    zclose(zpio_streams[i]);
    zpio_free(zpio_streams);
    zpio_free_std_streams_atexit_set = 0;

}

#endif /*SYSDEP_ZPIO_GET_STD_STREAM*/

