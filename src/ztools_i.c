/*
* File:		ztools_i.c
*
* Id:           $Id: ztools_i.c,v 1.5 2003/06/27 19:04:34 trurl Exp $
*
* Project:	ztools
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
#include "zcompat/ztools.h"

#include "zcompat/zlinkdll.h"

#if 0
ztools_obj_desc ztools_int_desc =  {
    /* size of object */	    sizeof(int),    
    /* comparator */		    NULL, /* so the memcmp is used */
    /* destructor */		    NULL, /* no destructor is used */
    /* new object from template*/   NULL, /* so bytes are copied directly */
    /* copy constructor */	    NULL, /* so bytes are copied directly */
};

struct ztools_obj_desc ztools_charp_desc = {
    /* size of object */	    sizeof(char*),    
    /* comparator */		    strcmp,
    /* destructor */		    ztools_free,
    /* new object from template*/   ztools_strdup,
    /* copy constructor */	    NULL, /* so bytes are copied directly */
};
#endif
