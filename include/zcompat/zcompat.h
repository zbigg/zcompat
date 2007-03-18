/*
* File:		zcompat.h
*
* Project:	ZCompat
*
* Decription:	Library main header
*
* Author:	Zbigniew Zagorski <zzbigg@o2.pl>
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2002,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
*
* To do:
    * add ztruncate(ZSTREAM f, ?pos or current?)
*/

#ifndef __zcompat_h_
#define __zcompat_h_

#define INC_ZCOMPAT

#include "zsystype.h"
#include "zdefs.h"

/* ZPIO typedefs ... */
struct	zstream_s;
typedef struct zstream_s zstream_t;
typedef zstream_t	ZFILE;
typedef zstream_t*	ZSTREAM;

/* OTHER typedefs */


/*
 * #include "zpio.h"
 * #include "zprintf.h"
 * #include "ztools.h"
 */

#ifdef ZCOMPAT_MAKINGDLL
#   define	ZCEXPORT    ZEXPORT
#else
#   define	ZCEXPORT    ZIMPORT
#endif

void	ZCEXPORT    zc_strncpy(char* dest, const char* src, size_t dest_len);
void	ZCEXPORT    zc_strncat(char* dest, const char* src, size_t dest_len);

int	ZCEXPORT    zc_strcasecmp(const char* a, const char* b);
int	ZCEXPORT    zc_strncasecmp(const char* a, const char* b,size_t len);

void    ZCEXPORT    zc_atexit(void (*p)(void));
void    ZCEXPORT    zc_set_atexit(void (*p)(void (*)(void)));


#endif /* __zcompat_h_ */











