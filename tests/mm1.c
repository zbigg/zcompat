 /*
   * File:	mm1.c
   * Id:	$Id$
   * Project:	libzcompat
   *
   * Author Zbigniew Zagorski <zzbigg@o2.pl>
   *
   * Copyright:
   *
   *   This program is Copyright(c) Zbigniew Zagorski 2003,
   *   All rights reserved, and is distributed as free software under the
   *   license in the file "LICENSE", which is included in the distribution.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "zcompat/zsystype.h"

#include <errno.h>
#include <string.h>
#include <zcompat/zpio.h>
#include <stdlib.h>
#include "zcompat/zprintf.h"
#include "zcompat/zmm.h"

int test_allocator(const char* a, zmm_allocator* al, int r_b, int r_e, int n)
{
    int i;
    for( i=0; i < n; i++ ) {
	int s = r_b + rand() % (r_e-r_b+1);
	char* x = zmm_alloc( al, s);
	if( x )
	    memset(x,i % 256, s);
	else
	    zfprintf(zstderr,"%s %i: can't alloc %i bytes\n",a,i,s);
    }
    zmm_stats(al,zstderr,0);
    return n;
}

int main(int argc, char** argv)
{
    int result = 1;
    zmm_allocator* al;
    srand(time(NULL));
    al = zmm_new_constsize_allocator(16);
    test_allocator("const",al,1,16,1000);
    zmm_delete(al);
    
    al = zmm_new_incremental_allocator(8192*2);
    test_allocator("inc  ",al, 10, 200, 1000);
    zmm_delete(al);
    
    return result ? 0 : 1;
}
