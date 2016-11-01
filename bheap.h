#ifndef _BHEAP
#define _BHEAP

typedef struct {
	int dir;
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
void heapup (bheap_t *h, void *el);
int heapdowni (bheap_t *h, void *p, size_t i);
int heapdown (bheap_t *h, void *p);
int heapsearch (bheap_t *h, void *p, size_t i, int (*match)(const void *));
int heapdelete (bheap_t *h, int (*match)(const void *));
#ifndef NDEBUG
int heapverify (bheap_t *h);
void heapdump (bheap_t *h, void (*out) (const void *, char *, size_t));
#endif
#endif