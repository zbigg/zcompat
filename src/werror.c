/*
* File:	    win/werror.c
*
* Project:	Portable I/O library
*
* Description:  windows specific error handling routines
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
#include "zcompat/zsystype.h"
#ifdef ZSYS_WIN

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <winsock.h>
#include <windows.h>
#pragma hdrstop

#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"
#include "wutil.h"

void    zvuprintf(int umode,const char* fmt,va_list ap)
{
    switch( umode ) {
    case ZUMODE_OUT:
	if( !win_stdout && !(_zpiowin_init() & 2) && !win_stdout ) {
	    _zpiowin_vmessage_box(
		"Message",MB_OK,
		fmt,ap);
	} else {
	    zvfprintf(win_stdout,fmt,ap);
	}
	break;
    case ZUMODE_DBG:
        {
            char* x = zvsaprintf(NULL,0,fmt,ap);
            OutputDebugString(x);
            zpio_free(x);
        }
        break;
    default:
	if( !win_stderr && !(_zpiowin_init() & 4) && !win_stderr ) {
	    _zpiowin_vmessage_box(
		(umode == ZUMODE_WARN
		    ? "Warning"
		    : "Error"),
		MB_OK |
		(umode == ZUMODE_WARN
		    ? MB_ICONWARNING
		    : MB_ICONERROR ),
		fmt,ap);
	} else {
	    zvfprintf(win_stderr,fmt,ap);
	}
	break;
    }
}

void    zperror(const char* msg)
{
    const char *str = zstrerror(errno);
    if( str == NULL ) {
	str = "unknown error";
    }

    zuprintf(ZUMODE_ERR,"%s : %s\n",msg,str);
}

void zsperror(ZSTREAM f,const char* msg)
{
    const char* str = zstrerror(f->error);
    char* fname = f->name ? f->name : "stream";
    zuprintf(ZUMODE_ERR,"%s : %s : %s\n",msg,fname,str);
}

/* win32f.c */
extern const char* win32_strerror(DWORD er);
extern DWORD zwin32_last_error;

#ifndef ZST_WSA_ERROR_COUNT
#define ZST_WSA_ERROR_COUNT 1100
#endif
#define ERR_IS_WSA(a) ( (a) >= WSABASEERR && (a) <=  WSABASEERR+ZST_WSA_ERROR_COUNT )

const char*  zstrerror(int errmsg)
{
    char* strerr;
    if( ERR_IS_WSA( errmsg ) ) {
	switch( errmsg ) {
#	include "wsockerr.h"
	}
    } else if( errmsg == ZE_FROM_WINDOWS ) {
        return win32_strerror(zwin32_last_error);
    } else
	switch( errmsg ) {
	case ZEINHOST:
    	    strerr = "host name invalid or not found";
    	    break;
	default:
    	    strerr = strerror(errmsg);
    	    if( !strerr || strcmp(strerr,"Unknown error") == 0 )
        	return win32_strerror(errmsg);
	}
    return strerr;
}

int     zpiowin_error2errno(DWORD er) 
{
    switch( er ) {
#   include "win32err.c"
    default:
        zwin32_last_error = er;
        return ZE_FROM_WINDOWS;
    }
}

int     zpiowin_get_last_errno()
{
    return zpiowin_error2errno(GetLastError());
}

static char win32_error_buf[200] = "(success)";
DWORD zwin32_last_error = 0;
const char* win32_strerror(DWORD er)
{
    LPVOID lpMsgBuf;
    if( FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	NULL,
	er,
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	(LPTSTR) &lpMsgBuf,
	0,
	NULL
	) < 0 || lpMsgBuf == NULL) {

	strncpy(win32_error_buf,"Very unknown Windows error !",sizeof(win32_error_buf));
	return win32_error_buf;
    }
    strncpy(win32_error_buf,(char*)lpMsgBuf,sizeof(win32_error_buf));
    win32_error_buf[sizeof(win32_error_buf)] = 0;
    LocalFree(lpMsgBuf);
    return win32_error_buf;
}

#endif


