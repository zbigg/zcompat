/*
* File: pipe.c
*
* Id:           $Id: pipe.c,v 1.6 2003/06/28 11:57:44 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  pipes I/O
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

#include "zcompat/zsystype.h"
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef ZSYS_UNIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#ifdef ZDEV_VC
#include <io.h>
#define HAVE__POPEN 1
#endif
#ifdef ZDEV_BORLAND
#include <io.h>
#define HAVE_POPEN 1
#endif

#if defined(HAVE_POPEN) || defined(HAVE__POPEN)
#define ANSI_PIPE 1
#else
#define ANSI_PIPE 0
#endif

#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"

char*	zst_strdup2(const char* s1,const char* s2);

	ZSTREAM	zansi_pipe_open	(ZSTREAM f,const char* program,int mode);
	int	zansi_pipe_close(ZSTREAM f);
extern int	zansi_read	(ZSTREAM,void*,size_t);
extern int	zansi_write	(ZSTREAM,const void*,size_t);
extern long	zansi_ctl	(ZSTREAM,int,void*);

zstream_vtable_t zvt_ansi_pipe = {
    1,			/*__ id */
    2,			/*__ data_size */
    zansi_pipe_open,	/*__ open */
    zansi_pipe_close,	/*__ close */
    zansi_read,		/*__ read */
    zansi_write,	/*__ write */
    NULL,		/*__ seek */
    zansi_ctl,		/*__ ctl */
};


/* ------------------
             PUBLIC
            -----------------*/
#ifndef SYSDEP_ZPOPEN
ZSTREAM zpopen(const char* program,int mode) {
#ifdef ANSI_PIPE
    return zcreat(& zvt_ansi_pipe, program, mode );
#else
    errno = ENODEV;
    return NULL;
#endif
}
#endif

#define file_d ((FILE*) (f->data[0]))
#define files_d(a) ((f->data[0]) = (unsigned long)a)

ZSTREAM	zansi_pipe_open	(ZSTREAM f,const char* program,int mode)
{
#if ANSI_PIPE
    int readf	= (mode & ZO_RD) == ZO_RD;
    int writef	= (mode & ZO_WR) == ZO_WR;
    FILE* af;
#if ANSI_PIPE
    char* type = readf ? "r" : "w";
#endif
    if( (!readf && !writef) || program == NULL) {
	errno = EINVAL;
	return 0;
    }
    if( readf && writef ) {
	errno = EINVAL;
	return NULL;
    }	/* popen supoprts only one way communication */
#if defined HAVE_POPEN
    af = popen(program,type);
#elif defined HAVE__POPEN
    af = _popen(program,type);
#else
    af = 0;
    errno = ENODEV;
#endif
    if( af == NULL )
	return NULL;
    files_d(af);
    f->name = zst_strdup2("pipe://",program);
    ZFL_UNSET(f->flags,ZSF_FILE);
    return f;
#else
    errno = ENODEV;
    return NULL;
#endif
}

int	zansi_pipe_close(ZSTREAM f)
{
#if defined HAVE_POPEN
    return pclose(file_d);
#elif defined HAVE__POPEN
    return _pclose(file_d);
#else
    errno = ENODEV;
    return 0;
#endif
}
#undef file_d


