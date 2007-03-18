/*
* File:	    win/wutil.c
*
* Id:           $Id: wutil.c,v 1.5 2003/05/07 13:44:12 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  windows specific
		utility
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
#include "zcompat/zcompat.h"

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

#if 0 && defined(BUILD_XL)
#include "axl/xlstr.h"
#else
#include "zcompat/ztools.h"
#endif
ZSTREAM
    win_stdin	= 0,
    win_stdout	= 0,
    win_stderr	= 0;


int	_zpiowin_message_box(
    const char* title,
    int options,
    const char* fmt,
    ...)
{
    int r;
    va_list ap;
    va_start(ap,fmt);
    r = _zpiowin_vmessage_box(title,options,fmt,ap);
    va_end(ap);
    return r;
}
int	_zpiowin_vmessage_box(
    const char* title,
    int options,
    const char* fmt,
    va_list ap)

{
    char* a;
    int mbr;

    a = zvsaprintf(0,0,fmt,ap);

    mbr = MessageBox((HWND)0,a,title,(UINT)options);

    zpio_free(a);
    return mbr;
}
int	zwin32out_debug_write(ZSTREAM f,const void* p,size_t size)
{
#if 0 && defined(BUILD_XL)
    xl_str b = NULL;
    char buf[20];
    size_t i = 0;
    const char* s = (const char*)p;
    if( !p ) {
        errno = EINVAL;
        return -1;
    }
    while( i < size ) {
        if( isprint(*s) || iscntrl(*s) )
            b = xls_add_char(b,*s);
        else {
            sprintf(buf,"\\x%x",*s);
            b = xls_add(b,buf);
        }
        s++;
        i++;
    }
    OutputDebugString(xls_get(b));
    xls_free(b);
    return size;
#else
    sstr b = SSTR_EMPTY;
    char buf[20];
    size_t i = 0;
    const char* s = (const char*)p;
    if( !p ) {
        errno = EINVAL;
        return -1;
    }
    while( i < size ) {
        if( isprint(*s) || iscntrl(*s) )
            sstr_addc(&b,*s);
        else {
            sprintf(buf,"\\x%x",*s);
            sstr_add(&b,buf);
        }
        s++;
        i++;
    }
    OutputDebugString(b.buf);
    sstr_frees(&b);
    return size;
#endif
}

zstream_vtable_t zvt_zwin32out_debug = {
    6,			/*__ id */
    0,			/*__ data_size */
    NULL        ,	/*__ open */
    NULL        ,	/*__ close */
    NULL        ,	/*__ read */
    zwin32out_debug_write,	/*__ write */
    NULL        ,	/*__ seek */
    NULL        ,	/*__ ctl */
};

int	_zpiowin_init(void)
{
    HANDLE hIn,hOut,hErr;

    int i = 0;
    DWORD dwinfo;

    hIn  = GetStdHandle(STD_INPUT_HANDLE);
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hErr = GetStdHandle(STD_ERROR_HANDLE);

    if( hIn != INVALID_HANDLE_VALUE &&
	hIn != NULL &&
	GetHandleInformation(hIn,&dwinfo) ) {
	i |= 1;
	if( !win_stdin )
	    win_stdin = zwin32hopen(hIn);
    }
    if( hOut != INVALID_HANDLE_VALUE &&
	hOut != NULL &&
	GetHandleInformation(hOut,&dwinfo) ) {
	i |= 2;
	if( !win_stdout )
	    win_stdout = zwin32hopen(hOut);
    }
    if( hErr != INVALID_HANDLE_VALUE &&
	hErr != NULL &&
	GetHandleInformation(hErr,&dwinfo) ) {
	i |= 4;
	if( !win_stderr )
	    win_stderr = zwin32hopen(hErr);
    }
    if( !win_stderr ) {
        win_stderr = zcreat(&zvt_zwin32out_debug,NULL,ZO_WRITE);
    }
    zc_atexit(_zpiowin_done);
    return i;
}
void	_zpiowin_done(void)
{
    if( win_stdin )
	zclose(win_stdin);
    if( win_stdout )
	zclose(win_stdout);
    if( win_stderr )
	zclose(win_stderr);
    win_stderr = win_stdin = win_stdout = (ZSTREAM)0;
}
#endif
