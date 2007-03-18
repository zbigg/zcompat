/*
    Kopiec MIN - modul

    Autor Zbigniew Zagorski <zzbigg@o2.pl>
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdarg.h>
#include "heap.h"

struct _heap_node {
    int	    value;
    void*   param;
};

struct _heap_t {
    int	    qty;
    int	    nmax;
    struct _heap_node*    tab;

};


heap	heap_new(heap h,int size);
void	heap_free(heap h);

int	heap_empty(heap h);
int	heap_full(heap h);

void	heap_put(heap h,int x,void* param);
int	heap_get(heap h,void** Pparam);
int	heap_peek(heap h);
void*	heap_peek_param(heap h);

heap	heap_new(heap h,int size)
{
    h = h ? h : malloc(sizeof(struct _heap_t));
    memset(h,0,sizeof(*h));
    h->tab = (struct _heap_node*)malloc( sizeof(struct _heap_node) * size );
    h->nmax = size-1;
    return h;
}
void	heap_free(heap h) {
    free(h->tab);
    free(h);
}

int	heap_empty(heap h) {
    return h->qty > 0;
}
int	heap_full(heap h) {
    return h->qty == h->nmax;
}

static void	swap_node(struct _heap_node* a,struct _heap_node* b) {
    struct _heap_node t = *a;
    *a = *b;
    *b = t;
}
void	heap_put(heap h,int x,void* param) 
{
    if( h->qty == 0 ) {
	h->tab[1].value = x;
	h->tab[1].param = param;
	h->qty = 1;
	return;
    } else {
	int cp = ++(h->qty); /* aktualna pozycja */
	h->tab[cp].value = x;
	h->tab[cp].param = param;

	while( cp != 1 ) {
	    int p = cp / 2; /* pozycja "rodzica" */
	    if( h->tab[p].value > h->tab[cp].value ) {
		swap_node(h->tab + p , h->tab + cp );
		cp = p;
	    } else 
		break;
	}
    }
}

int	heap_get(heap h,void** Pparam)
{
    {
	if( h->qty == 0 ) {
	    if( Pparam )
		*Pparam = 0;
	    return 0;
	}
    }
    {
	int ret = h->tab[1].value;

	int cp = 1;
	if( Pparam )
	    *Pparam = h->tab[1].param;
	h->tab[1] = h->tab[h->qty--];
	while ( cp <= h->qty ) {
	    int r = cp*2;
	    int l = cp*2+1;

	    int rb = r <= h->qty 
			? (h->tab[cp].value > h->tab[r].value) 
			: 0;
	    int lb = l <= h->qty 
			? (h->tab[cp].value > h->tab[l].value )
			: 0;
	    
	    if( rb && lb ) {
		if( h->tab[r].value < h->tab[l].value )
		    lb = 0;
		else
		    rb = 0;
	    }
	    if( rb ) {/* parent is bigger than right child */
		swap_node(h->tab + cp , h->tab + r );
		cp = r;
	    } else if( lb ) {/* parent is bigger than right child */
		swap_node(h->tab + cp , h->tab + l );
		cp = l;
	    } else {/* heap structure is ok */
		return ret;
	    }
	} /* while */
	return ret;
    }
}
int	heap_peek(heap h)
{
    return (h->qty > 0) ? h->tab[1].value : 0;
}

void*	heap_peek_param(heap h)
{
    return (h->qty > 0) ? h->tab[1].param : NULL;
}
