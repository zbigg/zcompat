/*
* File:	    win/wgetcon.c
*
* Id:           $Id: wgetcon.c,v 1.5 2003/05/07 13:44:12 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  windows specific
		get/free console input
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

/*
    These are in error.c
*/

extern DWORD zwin32_last_error;
extern const char* win32_strerror(DWORD er);

static int con_alloc = 0;

ZSTREAM	zpio_getcon(int stream_id)
{
    DWORD   nStdHandle;
    HANDLE *hStream;

    int i;

    switch( stream_id ) {
    case ZCON_IN:
	nStdHandle = STD_INPUT_HANDLE;
	break;
    case ZCON_OUT:
	nStdHandle = STD_OUTPUT_HANDLE;
	break;
    case ZCON_ERR:
	nStdHandle = STD_ERROR_HANDLE;
	break;
    default:
	errno = EINVAL;
	return 0;
    }
    i = 0;
    do {
	DWORD dwinfo;
	hStream = GetStdHandle(nStdHandle);
	if( hStream == INVALID_HANDLE_VALUE ||
	    hStream == NULL ||
	    !GetHandleInformation(hStream,&dwinfo) ) {
	    if( i == 0 ) {
		BOOL r = AllocConsole();
		if( !r ) {
		    errno = ZE_FROM_WINDOWS;
		    zwin32_last_error = GetLastError();
		    return 0;
		}
		_zpiowin_init();
		con_alloc = 1;
		i++;
	    } else {
		break;
	    }
	} else break;
    } while( 1 );

    if( hStream == INVALID_HANDLE_VALUE ) {
	errno = ZE_FROM_WINDOWS;
	zwin32_last_error = GetLastError();
	return 0;
    }
    return zwin32hopen( hStream );
}

void zpio_freecon()
{
    if( con_alloc ) {
	FreeConsole();
	_zpiowin_done();
    }
    con_alloc = 0;
}
#endif
