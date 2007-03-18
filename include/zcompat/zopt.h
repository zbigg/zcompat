 /*
   * File:	zopt.h
   *
   * Id:	$Id: sgetopt.h,v 1.3 2003/02/26 18:28:43 trurl Exp $
   *
   * Project:	ZCompat
   *
   * Author Zbigniew Zagorski <zzbigg@o2.pl>
   *
   * Copyright:
   *
   *   This program is Copyright(c) Zbigniew Zagorski 2002,
   *   All rights reserved, and is distributed as free software under the
   *   license in the file "LICENSE", which is included in the distribution.
 */
#ifndef __zcompat_zopt_h_
#define __zcompat_zopt_h_

#ifndef INC_ZDEFS
#include "zdefs.h"
#endif

#define    ZOPT_ALLOW_POSTFIX		1
#define	   ZOPT_NO_PARAM		2

#define    ZOPT_BOOL			1
#define    ZOPT_NEGBOOL			2
#define    ZOPT_STRING			3
#define    ZOPT_INT			4
#define    ZOPT_CALLBACK	        5
#define    ZOPT_DOUBLE			6

struct zopt_rec {
   char*    name;
   char*    shortcut;
   int	    flags;
   int	    type;
   void*    param;
   long	    size;
   const char* info;
};

typedef struct zopt_rec zopt_rec;
/*##

 =Function zc_getopt

   parse commandline options

 =Synopsis
	|#include "zcompat/zopt.h"
	|
	|int zc_getopt(struct zopt_rec* rec,int no,
   |            int* argc,char*** argv,int flags);

 =Description

	This function gets table 
    |
    |struct zopt_rec {
    |   char*       name;
    |   char*       shortcut;
    |   int         flags;
    |   int         type;
    |   void*       param;
    |   long        size;
    |   const char* info;
    |};
	 
	 These constants are passed as flags:
    |  #define    ZOPT_ALLOW_POSTFIX  1
    |  #define    ZOPT_NO_PARAM       2
	
    These are possible types of data:
    * ZOPT_BOOL - argument is integer which has values 0 and not 0
	for the user there are following input 
    * ZOPT_STRING - argument is string that follows option argument
    * ZOPT_CALLBACK
	
	It hasn't default value.
    |  #define    ZOPT_NEGBOOL      2
    |  #define    ZOPT_STRING       3
    |  #define    ZOPT_INT          4
    |  #define    ZOPT_CALLBACK     5
    |  #define    ZOPT_DOUBLE       6

	 

 =Return value

    what is return value ????

*/
ZCEXPORT int    zc_getopt(struct zopt_rec* rec,int no,
            int* argc,char*** argv,int flags,
	    void (*usage)(const char* msg));

ZCEXPORT int    zc_genusage(struct zopt_rec* records,int no,FILE* output);

#endif




