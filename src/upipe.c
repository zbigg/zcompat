/*
* File: upipe.c
*
* Id:           $Id: upipe.c,v 1.5 2003/06/28 11:57:45 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  pipe I/O
		for UNIX-like systems
		using fork() and pipe() syscalls
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

#include "zcompat/zsystype.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(HAVE_FORK) && defined(HAVE_PIPE)

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

char*	zst_strdup2(const char* s1,const char* s2);

/* Different input & output stream */
extern int	zdstream_read	(ZSTREAM,void*,size_t);
extern int	zdstream_write	(ZSTREAM,const void*,size_t);
extern long	zdstream_ctl	(ZSTREAM,int,void*);

/* UNIX pipe. */
ZSTREAM	zpipe_open	(ZSTREAM,const char*,int);
int	zpipe_close	(ZSTREAM);

zstream_vtable_t zvt_pipe = {
    1,			/*__ id */
    3,			/*__ data_size */
    zpipe_open,		/*__ open */
    zpipe_close,	/*__ close */
    zdstream_read,	/*__ read */
    zdstream_write,	/*__ write */
    NULL,		/*__ seek */
    zdstream_ctl,	/*__ ctl */
};

/* ------------------
             PUBLIC
            -----------------*/
ZSTREAM zpopen(const char* program,int mode)
{
    return zcreat(& zvt_pipe, program, mode );
}

/* ----------------------====*****      UNIX PIPE
*/
#define pipe_in		((ZSTREAM) f->data[0])
#define pipe_out	((ZSTREAM) f->data[1])
#define pipe_pid	((int) f->data[2])

#define pipes_in(a)	(f->data[0] = (unsigned long) (a))
#define pipes_out(a)	(f->data[1] = (unsigned long) (a))
#define pipes_pid(a)	(f->data[2] = (unsigned long) (a))

/** Open pipe.
    Modes supported : ZO_RD | ZO_WR | ZO_RDWR
*/
ZSTREAM	zpipe_open	(ZSTREAM f,const char* program,int mode)
{
    int		out[2] = { -1,-1 };
    int 	in[2] = { -1,-1 };
    int		a;
    int		pid;

    int readf	= (mode & ZO_RD) == ZO_RD;
    int writef	= (mode & ZO_WR) == ZO_WR;

    if( (!readf && !writef) || program == NULL) {
	errno = EINVAL;
	return 0;
    }

    if( writef )
	if ( pipe(out) < 0 )
	    goto err_0; /* EXIT  */
    if( readf )
	if ( pipe(in) < 0 ) {
	    goto err_1; /* EXIT & free opened out handles */
	}

    switch ( (pid = fork()) ) {
    case 0:
	if( writef ) {
	    a = dup(out[0]);
	    dup2(a,0);
	    close(a);
	}
	if( readf ) {
	    a = dup(in[1]);
    	    dup2(a,1);
	    close(a);
	}

	if( writef ) {
	    close(out[0]);
	    close(out[1]);
	}

	if( readf ) {
	    close(in[0]);
	    close(in[1]);
	}
	_exit(system(program));
    case -1:
	goto err_2; /* EXIT & free opened handles */
    default:
	pipes_out(NULL);
	pipes_in(NULL);

	if( writef ) {
	    pipes_out(zdopen(out[1],"pipe out"));
	    ZFL_UNSET(pipe_out->flags, ZSF_PUBLIC);
	    close(out[0]);
	}
	if( readf ) {
	    pipes_in(zdopen(in[0],"pipe in"));
	    ZFL_UNSET(pipe_in->flags, ZSF_PUBLIC);
	    close(in[1]);
	}

	pipes_pid(pid);

        ZFL_UNSET(f->flags,ZSF_FILE);
	f->name = zst_strdup2("pipe://",program);
	return f;
    }
    return NULL;
err_2:
    if( in[0] != -1 )
	close(in[0]);
    if( in[1] != -1 )
	close(in[1]);
err_1:
    if( out[0] != -1 )
	close(out[0]);
    if( out[1] != -1 )
	close(out[1]);
err_0:
    return NULL;
}


int	zpipe_close	(ZSTREAM f)
{
    int e = 0;
    if( pipe_in )
	e |= zclose( pipe_in ) < 0;

    if( pipe_out )
	e |= zclose( pipe_out ) < 0;

    e  |= wait(NULL) < 0;

    return e;
}

#undef pipe_in
#undef pipe_out
#undef pipe_pid


#endif
