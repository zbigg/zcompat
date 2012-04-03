/*
* File:		zpio_i.c
*
* Id:           $Id: zpio_m.c,v 1.3 2003/02/26 18:28:40 trurl Exp $
*
* Project:	zpio
*
* Author Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*   This program is Copyright(c) Zbigniew Zagorski 1999-2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "zcompat/zcompat.h"
#include "zcompat/zlinkdll.h"

ZCEXPORT void    zpio_realloc(void* mem, size_t size)
{
    return zcompat_realloc(mem, size);
}
ZCEXPORT void    zpio_free(void* mem)
{
    zcompat_free(mem);
}
