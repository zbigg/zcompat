/*
* File:		win/wstdio.c
*
* Id:           $Id: wstdio.c,v 1.5 2003/05/07 13:44:12 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:  standard input/output/error
		for windows
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
#include <windows.h>
#include "zcompat/zpio.h"
#include "zpio_i.h"
#include "zcompat/zprintf.h"
#include "wutil.h"


ZSTREAM zpio_get_std_stream(int i)
{

    switch( i ) {
    case 0:
	if( !win_stdin )
	    _zpiowin_init();
	return win_stdin;
    case 1:
	if( !win_stdout )
	    _zpiowin_init();
	return win_stdout;
    case 2:
	if( !win_stderr )
	    _zpiowin_init();
	return win_stderr;
    default:
	return 0;
    }
    return 0;
}
#endif /* #ifdef ZSYS_WIN */
