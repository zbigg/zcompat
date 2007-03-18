/*
* File: win/wpipe.c
*
* Id:           $Id: wpipe.c,v 1.7 2003/06/27 18:49:52 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:	windows specific pipe functions
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2002,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
* To do:
    1. Use STARTUPINFo fields instead of using Get/SetStdHandle for
       redirecting output.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* std */
#include "zcompat/zsystype.h"
#ifdef ZSYS_WIN

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <io.h>
#include <process.h>
#include <stdarg.h>
#include <winsock.h>
#include <windows.h>
#pragma hdrstop

#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"
#include "wutil.h"

char*	zst_strdup2(const char* s1,const char* s2);

/*
    Different input & output stream used
    as read/write wrapper.
*/
int	zdstream_read	(ZSTREAM,void*,size_t);
int	zdstream_write	(ZSTREAM,const void*,size_t);
int	zdstream_ctl	(ZSTREAM,int,void*);

static ZSTREAM  zwin32_pipe_open (ZSTREAM f,const char* program,int mode);
static int	zwin32_pipe_close(ZSTREAM);
static long	zwin32_pipe_ctl	(ZSTREAM f,int what,void* data);

zstream_vtable_t zvt_win32_pipe = {
    1,			/*__ id */
    3,			/*__ data_size */
    zwin32_pipe_open,	/*__ open */
    zwin32_pipe_close,	/*__ close */
    zdstream_read,	/*__ read */
    zdstream_write,	/*__ write */
    NULL,		/*__ seek */
    zwin32_pipe_ctl,	/*__ ctl */
};

/* ------------------
	     PUBLIC
	    -----------------*/
ZSTREAM zpopen(const char* program,int mode)
{
    return zcreat(& zvt_win32_pipe, program, mode );
}

extern DWORD zwin32_last_error;     /* werror.c */
/* ----------------------====*****	WIN32 Anonymoues Pipe
*/
#define w32_pipe_in	 ((ZSTREAM) f->data[0])
#define w32_pipe_out	 ((ZSTREAM) f->data[1])
#define w32_pipe_phandle ((HANDLE) f->data[2])

static ZSTREAM zwin32_pipe_open (ZSTREAM f,const char* program,int mode)
{
    HANDLE	out[2];
    HANDLE	in[2];

    HANDLE	o_stdin = INVALID_HANDLE_VALUE;
    int 	stdin_changed = 0;
    HANDLE	o_stdout = INVALID_HANDLE_VALUE;
    int 	stdout_changed = 0;

    int readf	= (mode & ZO_RD) == ZO_RD;
    int writef	= (mode & ZO_WR) == ZO_WR;

    if( (!readf && !writef) || program == NULL) {
	errno = EINVAL;
	return 0;
    }
    w32_pipe_in = w32_pipe_out = (ZSTREAM)0;
    w32_pipe_phandle = (HANDLE)0;

    if( writef ) {
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
	if ( CreatePipe(&out[0],&out[1],NULL,4096) == 0 ) {
	    zwin32_last_error = GetLastError();
	    goto err_0;
	}
        SetHandleInformation(out[0],HANDLE_FLAG_INHERIT,HANDLE_FLAG_INHERIT);
    }
    if( readf ) {
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
	if ( CreatePipe(&in[0],&in[1],NULL,4096) == 0 ) {
	    zwin32_last_error = GetLastError();
	    goto err_1;
	}
        SetHandleInformation(in[1],HANDLE_FLAG_INHERIT,HANDLE_FLAG_INHERIT);
    }
    {	    /* spawn process */
	PROCESS_INFORMATION processi;
	STARTUPINFO si;
        DWORD creationFlags = 0;

        memset(&si,0,sizeof(si));
        si.cb = sizeof(si);
	/* GetStartupInfo(&si);*/

        si.hStdError = readf ? in[1] : GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdOutput = readf ? in[1] : GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdInput  = writef ? out[0] : GetStdHandle(STD_INPUT_HANDLE);
        si.dwFlags |= STARTF_USESTDHANDLES;
        if( readf && writef ) {
            creationFlags |= DETACHED_PROCESS;
            si.dwFlags |= STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_HIDE;
        }
	if( CreateProcess(
	    NULL,
	    (char*)program,
	    NULL,
	    NULL,
	    TRUE,
	    creationFlags,
	    NULL,
	    NULL,
	    &si,
	    &processi) == 0 )
	{
	    zwin32_last_error = GetLastError();
	    goto err_2;
	}
	w32_pipe_phandle = processi.hProcess;
    /*    CloseHandle(w32_pipe_phandle);*/
    }

    w32_pipe_out = NULL;
    w32_pipe_in = NULL;


    if( writef ) {
	w32_pipe_out = zwin32hopen(out[1]);
	ZFL_UNSET(w32_pipe_out->flags, ZSF_PUBLIC);
	CloseHandle(out[0]);
    }
    if( readf ) {
	w32_pipe_in = zwin32hopen(in[0]);
	ZFL_UNSET(w32_pipe_in->flags, ZSF_PUBLIC);
	CloseHandle(in[1]);
    }
    return f;

err_2:
    CloseHandle(in[0]);
    CloseHandle(in[1]);
err_1:
    CloseHandle(out[0]);
    CloseHandle(out[1]);
err_0:
    errno = ZE_FROM_WINDOWS;
    return NULL;
}
static int	zwin32_pipe_close(ZSTREAM f)
{
    int r = 0;
    if( w32_pipe_in  && zclose( w32_pipe_in ) < 0 )
	r = -1;
    if( w32_pipe_out && zclose( w32_pipe_out ) < 0 )
	r = -1;
    switch( WaitForSingleObject(w32_pipe_phandle,0) ) {
    case WAIT_FAILED:
	zwin32_last_error = GetLastError();
	errno = ZE_FROM_WINDOWS;
	return -1;
    case WAIT_TIMEOUT:
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED:
	return r;
    }
    return r;
}

static long	zwin32_pipe_ctl	(ZSTREAM f,int what,void* data)
{
    switch( what ) {
    case ZFCTL_GET_PID:
        return (long)w32_pipe_phandle;
    default:
        return zdstream_ctl(f,what,data);
    }
}
#undef w32_pipe_in
#undef w32_pipe_out
#undef w32_pipe_phandle
#endif
