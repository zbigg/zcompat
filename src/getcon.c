/*

* File: 	getcon.c
*
* Id:           $Id: getcon.c,v 1.3 2003/02/26 18:28:40 trurl Exp $
*
* Project:	Portable I/O library
*
* Description:	console/tty open function
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>

#include "zcompat/zpio.h"
#include "zpio_i.h"


#ifndef SYSDEP_ZPIO_GETCON
ZSTREAM zpio_getcon(int stream_id)
{

    return 0;

}

#endif



#ifndef SYSDEP_ZPIO_FREECON

void	zpio_freecon()

{

}

#endif

