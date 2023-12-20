#ifndef _BHEAP
#define _BHEAP

#ifdef HEAPDEBUG
#include <stdio.h>
#define HEAPDBG(f,...) \
	fprintf (stderr, "%s:%d %s() " f "\n", \
		__FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define HEAPDBG(f,...)
#endif

typedef struct {
	int dir, lock;
	size_t foot, num, size;
	int (*cmp)(const void *, const void *);
	unsigned char h[];
} bheap_t;

bheap_t *heapalloc (int dir, size_t nmemb, size_t size, int (*cmp)(const void *, const void *));
void heapfree (bheap_t *h);
void *heapheap (bheap_t *h);
size_t heapsize (bheap_t *h);
size_t heapcount (bheap_t *h);
int heappeek (bheap_t *h, void *p);
int heaplocked (bheap_t *h);
int heaplock (bheap_t *h);
int heapunlock (bheap_t *h);
int heapup (bheap_t *h, void *el);
int heapdownri (bheap_t *h, void *p, void *r, size_t i);
int heapdowni (bheap_t *h, void *p, size_t i);
int heapdown (bheap_t *h, void *p);
int heapsearch (bheap_t *h, void *p, size_t i, int (*match)(const void *, void *), void *arg);
int heapdelete (bheap_t *h, int (*match)(const void *, void *), void *arg);

#endif
