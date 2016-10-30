#ifndef _BHEAP
#define _BHEAP

typedef struct {
	int dir;
	size_t foot, num, size;
	int (*cmp)(const void *, const void *);
	byte h[];
} bheap_t;

bheap_t *heapalloc (int dir, size_t nmemb, size_t size, int (*cmp)(const void *, const void *));
void heapfree (bheap_t *h);
void *heapheap (bheap_t *h);
size_t heapsize (bheap_t *h);
size_t heapcount (bheap_t *h);
int heappeek (bheap_t *h, void *p);
void heapup (bheap_t *h, void *el);
int heapdown (bheap_t *h, void *p);
int heapverify (bheap_t *h);
#ifndef NDEBUG
void heapdump (bheap_t *h, void (*out) (const void *, char *, size_t));
#endif
#endif