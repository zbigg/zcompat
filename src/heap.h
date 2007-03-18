#ifndef __heap_h__
#define __heap_h__

struct _heap_t;
typedef struct _heap_t* heap;

heap	heap_new(heap h,int size);
void	heap_free(heap h);

int	heap_empty(heap h);
int	heap_full(heap h);

void	heap_put(heap h,int x,void* param);
int	heap_get(heap h,void** Pparam);
int	heap_peek(heap h);
void*	heap_peek_param(heap h);

#endif
