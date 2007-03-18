/*
* File:		sbtree.c
*
* Id:           $Id: sbtree.c,v 1.3 2003/06/27 19:04:34 trurl Exp $
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
#include <stdio.h>

#include "zcompat/ztools.h"
#include "ztools_i.h"

static void* str_dup(const void* a)
{
    return (void*) zt_strdup( (const char*) a );
}
static void str_del(void* a)
{
    zt_free(a);    
}
static int str_cmp(const void* a,const void* b)
{
    return strcmp( (const char*) a,(const char*) b );
}
sbtree* sbtree_new(void)
{
    return smap_new(str_cmp,str_del,str_dup);    
}


void	sbtree_free(sbtree* b,sbtree_free_f free_callback )
{
    b->item_del = free_callback;
    smap_free(b);
}
void	sbtree_frees(sbtree* b,sbtree_free_f free_callback )
{
    b->item_del = free_callback;
    smap_done(b);
}

int	sbtree_add(sbtree* bt,const char* key,void* data)
{
    return smap_add(bt,key,data);
}

void*	sbtree_del(sbtree* bt,const char* key)
{
    return smap_del(bt,key);
}
sbti	sbtree_del_i(sbtree* bt,sbti i)
{
    return smap_del_i(bt,i);
}

void*	sbtree_find(sbtree* bt,const char* key)
{
    return smap_find(bt,key);
}
sbti	sbtree_find_i(sbtree* bt,const char* key,sbti i)
{
    return smap_find_i(bt,key,i);
}

int	sbtree_delete(sbtree* bt)
{
    smap_free(bt);
    return 0;
}

sbti	sbtree_begin(sbtree* bt,sbti i)
{
    return smap_begin(bt,i);
}
sbti	sbtree_end(sbtree* bt,sbti i)
{
    return smap_end(bt,i);
}

void	sbti_free(sbti i)
{
    smapi_free(i);
}

sbti	sbti_inc(sbti i)
{
    return smapi_inc(i);
}
sbti	sbti_dec(sbti i)
{
    return smapi_dec(i);
}
int	sbti_eq(sbti a,sbti b)
{
    return smapi_eq(a,b);
}

void*	sbti_get(sbti i)
{
    return smapi_get(i);
}
const char*
	sbti_key(sbti i)
{
    return (const char*) smapi_key(i);
}

static void _show(sbtree_item* tree,int level,char* o,int dir)
;

int sbtree_show(sbtree* bt )
{
    _show( bt->head, 0, "",0 );
    return 0;
}

static void _show(sbtree_item* tree,int level,char* o,int dir)
{
    char *x = o,*y,*z;
    if( tree == NULL )
	return;
    y = malloc(strlen(x)+ 7);
	strcpy(y,x);
	strcat(y,"    ");
	strcat(y, dir ? "|" : " " );

    z = malloc(strlen(x)+ 7);
	strcpy(z,x);
	strcat(z,"    ");
	strcat(z, !dir ? "|" : " " );

    if( tree->left != NULL)
	_show(tree->left,level+1,z, 1);
    else{
/*	printf("%s    ,-- L - NULL\n",z);*/
    }
    printf("%s     %c --: %s, w:%i, level: %i,up=%s\n",
	x,
	(!dir ? 0x60 : 0x2c),
	(char*)tree->key,
	tree->weight,
	level,
	tree->up ? ( tree->up->key ? (char*)tree->up->key : "(no name)"): "none");
    if( tree->right != NULL)
	_show(tree->right,level+1,y, 0);
    else{
/*	printf("%s    `-- R - NULL\n",y); */
    }
    fflush(stdout);
    if( y )
	free(y);
    if( z )
	free(z);
}
