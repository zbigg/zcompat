/*
* Header:	zdefs.h
*
*    Author Zbigniew Zagorski <zzbigg@o2.pl>
*
  Definitions

  *  countof(static_array)
     returns size of array in elements

  *  offsetof(struct,member)
     returns offset in bytes of structure member

  *  zmin(a,b)
     returns less value of {a,b}

  *  zmax(a,b)
     returns greater value of {a,b}

  *  ZFL_ISSET(val,flag)
     ZFL_SET(val,flag)
     ZFL_UNSET(val,flag)
     test, set, and unset flags in value

  *  zassert(cond)
     assertion on condition
*
* Copyright:
*
*   This program is Copyright(c) Zbigniew Zagorski 1999-2001,
*   All rights reserved, and is distributed as free software under the
*   license in the file "LICENSE", which is included in the distribution.
\* * * * * * * * */

#ifndef __zdefs_h_
#define __zdefs_h_
#define INC_ZDEFS

#ifndef INC_ZSYSTYPE
#include "zcompat/zsystype.h"
#endif
#include <stddef.h>
/*
    number of items in vector
*/
#ifndef countof
#define countof(a) ( sizeof(a) / sizeof((a)[0]) )
#endif

/*
    offset of field in struct
 (taken from Visual C++ stdlib.h)
*/
#ifndef offsetof
#define offsetof(s,m)   (size_t)&(((s *)0)->m)
#endif

/*
    min/max
 = zmin/zmax
*/

#ifndef __cplusplus

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

#endif

#define zmin(a,b)   (((a) < (b)) ? (a) : (b))
#define zmax(a,b)   (((a) > (b)) ? (a) : (b))

/* Flags operating routines.
*/
#define ZFL_ISSET(a,b)	(((a) & (b)) == (b))
#define ZFL_SET(a,b)	((a) |= (b))
#define ZFL_UNSET(a,b)	((a) &= ~(b))


#define Z_NOT_READY	fprintf(stderr,"not ready function called: %s:%i\n",__FILE__,__LINE__),exit(1)

#if (defined NDEBUG || defined ZNDEBUG || defined ZNOASSERT) && ! defined Z_FORCE_ASSERT
#define zassert(a)
#else
#include <stdio.h>
#define zassert(a) if( a ) { fprintf(stderr,"%s:%i: assertion failed: '%s'\n",__FILE__,__LINE__,#a);exit(1) } else
#endif


#ifdef ZCOMPAT_DLL
#  ifdef ZCOMPAT_MAKINGDLL
#    define	ZCEXPORT    ZEXPORT
#  else
#    define	ZCEXPORT    ZIMPORT
#  endif
#else
#  define	ZCEXPORT
#endif

#endif /* _zdefs.h */

