/*
* Header:	win/wutil.h
*
* Id:           $Id: wutil.h,v 1.3 2003/06/27 19:04:35 trurl Exp $
*
* Project:	zpio
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*   This program is Copyright(c) Zbigniew Zagorski 2002, 
*   All rights reserved, and is distributed as free software under the 
*   license in the file "LICENSE", which is included in the distribution.
*/



#ifndef __win_wutil_i_h_
#define __win_wutil_i_h_

extern ZSTREAM win_stdin,win_stdout,win_stderr;

/*
    Windows message box with printf-like 
    formatted text

  options is set of MB_xx constants 
  (see MessageBox in Win32 Online Help)

*/
int	_zpiowin_message_box(
    const char* title,
    int options,
    const char* fmt,
    ...);

int	_zpiowin_vmessage_box(
    const char* title,
    int options,
    const char* fmt,
    va_list ap);

/*

    Return bit set
	0   win_stdin available
	1   win_stdout available
	2   win_stderr available


*/
int	_zpiowin_init(void);
void	_zpiowin_done(void);

/*  zpiowin_error2errno

    convert Win32 specific error code into errno style value
*/
int     zpiowin_error2errno(DWORD er);
/*  zpiowin_get_last_errno

    equal to zpiowin_error2errno(GetLastError())
*/
int     zpiowin_get_last_errno();

/*  zpiowin_raise_io
 
    Raise an "signal" that will cause, that all blocking read operations
    will stop returning EINTR.
*/
void    zpiowin_raise_io(void);

#endif /* force single include of file */



