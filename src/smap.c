/*
* File:		smap.c
*
* Id:           $Id: smap.c,v 1.4 2003/06/27 19:04:34 trurl Exp $
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

#include "zcompat/zcompat.h"
#include "zcompat/ztools.h"
#include "ztools_i.h"

/**
    PUBLIC INTERFACE
**/
smap*	smap_new(smap_cmp_f cmp_f,smap_del_f del_f,smap_dup_f dup_f);
void	smap_free(smap* m);

smap*	smap_init(smap* m,smap_cmp_f cmp_f,smap_del_f del_f,smap_dup_f dup_f);
void	smap_done(smap* m);	/* equal */
void	smap_frees(smap* m);    /* equal */

int	smap_delete(smap* m);

int	smap_add(smap* m,const void* key,void* data);
smapi	smap_add_i(smap* m,const void* key,void* data,smapi i);

int	smap_set(smap* m,const void* key,void* data);
smapi	smap_set_i(smap* m,const void* key,void* data,smapi i);

void*	smap_del(smap* bt,const void* key);
smapi	smap_del_i(smap* bt,smapi i);

void*	smap_find(smap* bt,const void* key);
smapi	smap_find_i(smap* bt,const void* key,smapi i);
void*   smap_find_cust(smap* bt, const void* cust_key, zt_cmp_f cust_cmp_f);
smapi   smap_find_cust_i(smap* bt, const void* cust_key, zt_cmp_f cust_cmp_f,smapi i);
smapi	smap_begin(smap* bt,smapi i);
smapi	smap_end(smap* bt,smapi i);

void	smapi_free(smapi i);

smapi	smapi_inc(smapi i);
smapi	smapi_dec(smapi i);
int	smapi_eq(smapi a,smapi b);

void*	smapi_get(smapi i);
void    smapi_set(smapi i, void* new_item);
const void* smapi_key(smapi i);


/**
    PRIVATE HELPERS
**/

#define KEY_DEL(m,k)	do if(m->del && (k) ) m->del((k)); while(0)
#define KEY_DUP(m,k)    ((m->dup) ? m->dup(k) : (void*)k )
#define KEY_CMP(m,a,b)	((m->cmp) ? m->cmp(a,b) : (unsigned long)a - (unsigned long)b )
#define ITEM_DEL(m,i)	do if( m->item_del ) m->item_del(i); while(0)

/** Deletes item without equalization */
//static void		del_item(smap* bt, smap_item* t);
//static void		cmp_item(smap* bt, smap_item* a,smap_item* b);
//static void		dup_key(smap* bt, smap_item* t);

/** Returns top right item */
static smap_item*	smap_max(smap * bt);
/** Returns top left item */
static smap_item*	smap_min(smap * bt);

/* 
*/
static void*		smap_eq_del_int(smap* m,
                                        smap_item** tree,
					const void *key,
					int *h);
/* */
static smap_item *	smap_eq_set_int(smap* m,
                                        smap_item ** tree,
					const void *key,
					int *h);
/* delete all from map
   fastest
*/
static void		smap_delete_all(smap* b);

/*  Change pointer in <x> from <w> to <to> 
*/
/*
static void		chptr(smap_item * x,
			    smap_item * w,
			    smap_item * to);
*/
#define suse(a)  (void)0
#define sfinish() (void)0

smap*	smap_new(smap_cmp_f cmp_f,smap_del_f del_f,smap_dup_f dup_f)
{
    smap *p = (smap *) zt_malloc(sizeof(smap));
    if (!p)
	return NULL;
    memset(p, 0, sizeof(*p));
    MDMF_FILL(p,"smap");

    return smap_init(p,cmp_f,del_f,dup_f);
}
void	smap_free(smap* m)
{
    if ( m ) {
	smap_done(m);
	zt_free(m);
    }
}

smap*	smap_init(smap* m,smap_cmp_f cmp_f,smap_del_f del_f,smap_dup_f dup_f)
{
    m->head = m->leftmost = m->rightmost = 0;
    m->cmp = cmp_f;
    m->dup = dup_f;
    m->del = del_f;
    m->item_del = 0;
    return m;
}
void	smap_done(smap* m)
{
    if( m )
	smap_delete_all(m);
}

void	smap_frees(smap* m)
{
    smap_done(m);
}

int	smap_delete(smap* m)
{
    smap_done(m);
    return 0;
}


int smap_add(smap * m, const void *key, void *data)
{
    smap_item *x;
    int h = 0;
    x = smap_eq_set_int(m,&m->head, key, &h);
    if (x == NULL)
	return -1;
    x->item = data;
    return 0;
}

smapi	smap_add_i(smap* m,const void* key,void* data,smapi i)
{
    smap_item *x;
    int h = 0;
    if (!i) {
	i = (smapi) zt_calloc(sizeof(*i), 1);
	MDMF_FILL(i,"smit");
    }
    i->item = 0;
    i->tree = m;
    x = smap_eq_set_int(m,&m->head, key, &h);
    if (x == NULL)
	return i;
    x->item = data;
    i->item = x;
    return i;
}

void *smap_del(smap * bt, const void *key)
{
    void *tmp;
    int h = 0;
    tmp = smap_eq_del_int(bt,&bt->head, key, &h);
    return tmp;
}
smapi smap_del_i(smap * bt, smapi i)
{
    smap_item *t = i->item;
    int h = 0;
    const void *nkey,*key;
    if (!t)
	return NULL;
    key = smapi_key(i);
    smapi_inc(i);
    nkey = smapi_key(i);
    smap_eq_del_int(bt,&bt->head, key, &h);
    
    if ( key ) {
	smap_find_i(bt, nkey, i);
    }
    return i;
}

void* smap_find(smap * bt, const void *key)
{
    smap_item *t = bt->head;
    while (t) {
	int c = KEY_CMP(bt,key, t->key);
	if (c == 0)
	    return t->item;
	else if (c < 0)
	    t = t->left;
	else if (c > 0)
	    t = t->right;
    }
    return NULL;
}

smapi smap_find_i(smap * bt, const void *key, smapi i)
{
    smap_item *t = bt->head;
    int c;
    if (!i) {
	i = (smapi) zt_calloc(sizeof(*i), 1);
	MDMF_FILL(i,"smit");
    }
    i->tree = bt;
    i->item = 0;
    while (t) {
	c = KEY_CMP(bt,key, t->key);
	if (c == 0) {
	    i->item = t;
	    return i;
	} else if (c < 0)
	    t = t->left;
	else if (c > 0)
	    t = t->right;
    }
    return i;
}

void*   smap_find_cust(smap* bt, const void* cust_key, zt_cmp_f cust_cmp_f)
{
    smap_item *t = bt->head;
    while (t) {
	int c = cust_cmp_f(cust_key, t->key);
	if (c == 0)
	    return t->item;
	else if (c < 0)
	    t = t->left;
	else if (c > 0)
	    t = t->right;
    }
    return NULL;
}
smapi   smap_find_cust_i(smap* bt, const void* cust_key, zt_cmp_f cust_cmp_f,smapi i)
{
    smap_item *t = bt->head;
    int c;
    if (!i) {
	i = (smapi) zt_calloc(sizeof(*i), 1);
	MDMF_FILL(i,"smit");
    }
    i->tree = bt;
    i->item = 0;
    while (t) {
	c = cust_cmp_f(cust_key, t->key);
	if (c == 0) {
	    i->item = t;
	    return i;
	} else if (c < 0)
	    t = t->left;
	else if (c > 0)
	    t = t->right;
    }
    return i;
}

smapi smapi_inc(smapi i)
{
    smap_item
	* node = i->item,
	*y;
    if (node == 0)
	return i;
    if (node->right != 0) {
	node = node->right;
	while (node->left != 0)
	    node = node->left;
    } else {
	y = node->up;
	if( !y )
	    node = NULL;
	else {
	    while (node == y->right) {
		node = y;
		y = y->up;
		if (y == 0)
		    break;
	    }
	    if (node->right != y)
		node = y;
	}
    }
    i->item = node;
    return i;
}

smapi smapi_dec(smapi i)
{
    smap_item
	* node = i->item,
	*y;
    if (node == 0)
	return i;
    if (node->left != 0) {
	node = node->left;
	while (node->right != 0)
	    node = node->right;
    } else {
	y = node->up;
	if( !y )
	    node = NULL;
	else {
	    while (node == y->left) {
		node = y;
		y = y->up;
		if (y == 0)
		    break;
	    }
	    if (node->left != y)
		node = y;
	}
    }
    i->item = node;
    return i;
}
void  smapi_set(smapi i, void* new_item)
{
    if (i->item)
	i->item->item = new_item;
}
void *smapi_get(smapi i)
{
    if (i->item)
	return i->item->item;
    return 0;
}
const void *smapi_key(smapi i)
{
    if (i->item)
	return i->item->key;
    return 0;
}
int smapi_end(smapi i)
{
    if (i->item == 0)
	return 1;
    return 0;
}

smapi smap_begin(smap * l, smapi a)
{
    if (!a)
	a = (smapi) zt_calloc(sizeof(*a), 1);
    a->tree = l;
/*    a->item = l->leftmost; */
    a->item = smap_min(l);
    return a;
}
smapi smap_end(smap * l, smapi a)
{
    if (!a)
	a = (smapi) zt_calloc(sizeof(*a), 1);
    a->tree = l;
/*    a->item = l->rightmost; */
    a->item = smap_max(l);
    return a;
}

void smapi_free(smapi i)
{
    if (i)
	zt_free(i);
}

int smapi_eq(smapi a, smapi b)
{
    return
	a->item == b->item
	&&
	a->tree == b->tree;
}


/**
PRIVATE static helpers
*/
static void _del_2(smap* map, smap_item** x)
{
    smap_item* t = *x;
    if( !t ) 
	return;
    if( t->left && t->left != t)
	_del_2(map, & t->left);
    if( t->right && t->right != t )
	_del_2(map, & t->right);
    ITEM_DEL(map,t->item);
    KEY_DEL(map,t->key);
    zt_free(t);		/* delete node */
    *x = NULL;
}
static void smap_delete_all(smap* b)
{
    _del_2(b,& b->head);

}

#define ch_up(a,b) if( (a) ) (a)->up = (b)

static smap_item *smap_eq_set_int(smap* m,smap_item ** tree, const void *key, int *h)
{
    int cmp;
    register smap_item *t = (*tree);
    smap_item *x = NULL;
    if (t == NULL) {
	t = (smap_item *) zt_malloc(sizeof(smap_item));
	MDMF_FILL(t,"smim");
	t->key = KEY_DUP(m,key);
	t->weight = 0;
	t->left = NULL;
	t->up = NULL;
	t->right = NULL;
	*h = 1;
	return (*tree) = t;
    }
    cmp = KEY_CMP(m,key, t->key);
    if (cmp < 0) {
	x = smap_eq_set_int(m,&t->left, key, h);
	ch_up(t->left, t);
	if (*h) {		/* left branch has grown */
	    switch (t->weight) {
	    case 1:
		t->weight = 0;
		*h = 0;
		break;
	    case 0:
		t->weight = -1;
		break;
	    case -1:{
		    register smap_item *p1 = t->left;
		    if (p1->weight == -1) {
			/* single LL change */
			t->left = p1->right;
			ch_up(p1->right, t);

			p1->right = t;
			ch_up(p1, t->up);
			ch_up(t, p1);

			t->weight = 0;
			t = p1;
		    } else {
			/* double LR change */
			register smap_item *p2 = p1->right;

			p1->right = p2->left;
			ch_up(p2->left, p1);

			p2->left = p1;
			ch_up(p1, p2);

			t->left = p2->right;
			ch_up(p2->right, t);

			p2->right = t;
			ch_up(p2, t->up);
			ch_up(t, p2);

			if (p2->weight == -1)
			    t->weight = 1;
			else
			    t->weight = 0;

			if (p2->weight == 1)
			    p1->weight = -1;
			else
			    p1->weight = 0;
			t = p2;
		    }
		    t->weight = 0;
		    *h = 0;
		    break;
		}		/* case -1 */
	    }			/* switch t->weight */
	}			/* *h */
    } else if (cmp > 0) {
	x = smap_eq_set_int(m,&t->right, key, h);
	ch_up(t->right, t);
	if (*h) {		/* right branch has grown */
	    switch (t->weight) {
	    case -1:
		t->weight = 0;
		*h = 0;
		break;
	    case 0:
		t->weight = 1;
		break;
	    case 1:{
		    register smap_item *p1 = t->right;
		    if (p1->weight == 1) {
			/* single RR change */

			t->right = p1->left;
			ch_up(p1->left, t);

			p1->left = t;
			ch_up(p1, t->up);
			ch_up(t, p1);

			t->weight = 0;
			t = p1;
		    } else {
			/* double RL change */
			register smap_item *p2 = p1->left;
			p1->left = p2->right;
			ch_up(p2->right, p1);

			p2->right = p1;
			ch_up(p1, p2);

			t->right = p2->left;
			ch_up(p2->left, t);

			p2->left = t;
			ch_up(p2, t->up);
			ch_up(t, p2);

			if (p2->weight == 1)
			    t->weight = -1;
			else
			    t->weight = 0;
			if (p2->weight == -1)
			    p1->weight = 1;
			else
			    p1->weight = 0;
			t = p2;
		    }
		    t->weight = 0;
		    *h = 0;
		    break;
		}		/* case -1 */
	    }			/* switch */
	}			/* *h */
    } else {			/* we have found it ! */
	*h = 0;
	return *tree = t;
    }
    *tree = t;
    return x;
}

static void equalize_left /* 1 */ (smap_item ** tree, int *h) {
    register smap_item *t = (*tree);

    switch (t->weight) {
    case -1:
	t->weight = 0;
	break;
    case 0:
	t->weight = 1;
	*h = 0;
	break;
    case 1:
	{
	    register smap_item *p1 = t->right;
	    register int b1 = p1->weight;
	    if (b1 >= 0) {
		/* single RR change */
		t->right = p1->left;
		ch_up(p1->left, t);

		p1->left = t;
		ch_up(p1, t->up);
		ch_up(t, p1);

		if (b1 == 0) {
		    t->weight = 1;
		    p1->weight = -1;
		    *h = 0;
		} else {
		    t->weight = 0;
		    p1->weight = 0;
		}
		t = p1;
	    } else {
		/* double Rl change */
		register smap_item *p2 = p1->left;

		b1 = p2->weight;

		p1->left = p2->right;
		ch_up(p2->right, p1);

		p2->right = p1;
		ch_up(p1, p2);

		t->right = p2->left;
		ch_up(p2->left, t);

		p2->left = t;
		ch_up(p2, t->up);
		ch_up(t, p2);

		t->weight = (b1 == 1) ? -1 : 0;
		p1->weight = (b1 == -1) ? 1 : 0;
		t = p2;
		p2->weight = 0;
	    }
	    break;
	}
    }
    *tree = t;
}

static void equalize_right /* 2 */ (smap_item ** tree, int *h) {
    register smap_item *t = (*tree);

    switch (t->weight) {
    case 1:
	t->weight = 0;
	break;
    case 0:
	t->weight = -1;
	*h = 0;
	break;
    case -1:
	{
	    register smap_item *p1 = t->left;
	    register int b1 = p1->weight;
	    if (b1 <= 0) {
		/* single LL change */
		t->left = p1->right;
		ch_up(p1->right, t);

		p1->right = t;
		ch_up(p1, t->up);
		ch_up(t, p1);

		if (b1 == 0) {
		    t->weight = -1;
		    p1->weight = 1;
		    *h = 0;
		} else {
		    t->weight = 0;
		    p1->weight = 0;
		}
		t = p1;
	    } else {
		/* double LR change */
		register smap_item *p2 = p1->right;

		b1 = p2->weight;

		p1->right = p2->left;
		ch_up(p2->left, p1);

		p2->left = p1;
		ch_up(p1, p2);

		t->left = p2->right;
		ch_up(p2->right, t);

		p2->right = t;
		ch_up(p2, t->up);
		ch_up(t, p2);

		t->weight = (b1 == -1) ? 1 : 0;
		p1->weight = (b1 == 1) ? -1 : 0;
		t = p2;
		p2->weight = 0;
	    }
	    break;
	}
    }
    *tree = t;
}


static void *_del(smap* m,smap_item ** r, smap_item * q, int *h)
{
    smap_item *x = *r;
    smap_item *tr;
    if (x->right != NULL) {
	tr = _del(m,&x->right, q, h);
	if (*h)
	    equalize_right(&x, h);
	*r = x;
    } else {
	/* q is deleted */
	KEY_DEL(m,q->key);
	q->key = x->key;
	tr = q->item;
	q->item = x->item;
	q->up = NULL;
	(*r) = x->left;
	if (*r)
	    (*r)->up = x->up;
	zt_free(x);
	*h = 1;
    }
    return tr;
}
static void *smap_eq_del_int(smap* m,smap_item ** tree, const void *key, int *h)
{
    smap_item *t = (*tree);
    void *r;
    if (t == NULL) {
	*h = 0;
	return NULL;
    } else {
	int cmp = KEY_CMP(m,key, t->key);
	if (cmp < 0) {
	    r = smap_eq_del_int(m,&t->left, key, h);
	    ch_up(t->left, t);
	    if (*h)
		equalize_left(&t, h);
	} else if (cmp > 0) {
	    r = smap_eq_del_int(m,&t->right, key, h);
	    ch_up(t->right, t);
	    if (*h)
		equalize_right(&t, h);
	} else {
	    smap_item *q = t/*, **r = tree*/;
	    if (q->left == NULL) {
		t = q->right;
		ch_up(t, NULL);

		r = q->item;

		KEY_DEL(m,q->key);
		q->key = NULL;
		zt_free(q);
		q = NULL;

		*h = 1;
	    } else if (q->right == NULL) {
		t = q->left;
		ch_up(t, NULL);

		r = q->item;

		KEY_DEL(m,q->key);
		
		q->key = NULL;
		zt_free(q);
		q = NULL;

		*h = 1;
	    } else {
		r = _del(m,&q->left, q, h);
		if (*h)
		    equalize_left(&t, h);
	    }
	}
    }
    *tree = t;
    return r;
}
/*
static void chptr(smap_item * x, smap_item * w, smap_item * to)
{
    if (!x)
    else if (x->up == w)
	x->up = to;
    else if (x->left == w)
	x->left = to;
    else if (x->right == w)
	x->right = to;
}
*/
#if 0 /* this is no longer used */
static void smap_del_item(smap * bt, smap_item * t)
{
    smap_item *l;
    if (!t->right && !t->left) {
	if (t->up) {
	    chptr(t->up, t, 0);
	    zt_free(t->key);
	    zt_free(t);
	    return;
	} else {
	    bt->head = NULL;
	    zt_free(t->key);
	    zt_free(t);
	    return;
	}
    }
    if (!t->left && t->right) {
	if (!t->up) {
	    bt->head = t->right;
	    bt->head->up = 0;
	    zt_free(t->key);
	    zt_free(t);
	    return;
	} else {
	    chptr(t->up, t, t->right);
	    t->right->up = t->up;
	    zt_free(t->key);
	    zt_free(t);
	    return;
	}
    }
    if (t->left && !t->right) {
	if (!t->up) {
	    bt->head = t->left;
	    bt->head->up = 0;
	    zt_free(t->key);
	    zt_free(t);
	    return;
	} else {
	    chptr(t->up, t, t->left);
	    t->left->up = t->up;
	    zt_free(t->key);
	    zt_free(t);
	    return;
	}
    }
    l = t->left;
    while (l->right != NULL)
	l = l->right;
    t->item = l->item;
    KEY_DEL(bt,t->key);
    t->key = l->key;
    chptr(l->up, l, l->left);
    chptr(l->left, l, l->up);
    zt_free(l);
}
#endif

static smap_item *smap_min(smap * bt)
{
    smap_item *a = bt->head;
    if (!a)
	return 0;
    while (a->left)
	a = a->left;
    return a;
}

static smap_item *smap_max(smap * bt)
{
    smap_item *a = bt->head;
    if (!a)
	return 0;
    while (a->right)
	a = a->right;
    return a;
}
