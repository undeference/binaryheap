#include <stdlib.h>
#include <string.h>
#include <bheap.h>

bheap_t *heapalloc (int dir, size_t nmemb, size_t size, int (*cmp)(const void *, const void *)) {
	bheap_t *h = malloc (sizeof (bheap_t) + nmemb * size);
	h->dir = dir;
	h->num = nmemb;
	h->size = size;
	h->cmp = cmp;
	h->foot = 0;
	h->lock = 0;
	HEAPDBG ("%s-heap %p of up to %zd items of size %zd",
		h->dir < 0 ? "min" : "max", h, h->num, h->size);
	return h;
}

void heapfree (bheap_t *h) {
	free (h);
}

void *heapheap (bheap_t *h) {
	return h->h;
}

size_t heapsize (bheap_t *h) {
	return h->num * h->size;
}

size_t heapcount (bheap_t *h) {
	return h->foot;
}

#define _heap_left(i) (((i) << 1) + 1)
#define _heap_right(i) (((i) << 1) + 2)
#define _heap_parent(i) (((i) - 1) >> 1)
#define _heap_item(h,i) (void *)((h)->h + (i) * (h)->size)
#define _heap_cmp(h,a,b) ((h)->dir * (h)->cmp (_heap_item (h, a), b))
#define _heap_cmp_id(h,i,j) _heap_cmp (h, i, _heap_item (h, j))
#define _heap_set(h,i,s) memcpy (_heap_item (h, i), s, (h)->size)
#define _heap_set_id(h,i,j) _heap_set (h, i, _heap_item (h, j))

int heappeek (bheap_t *h, void *p) {
	if (h->foot <= 0)
		return 0;
	memcpy (p, _heap_item (h, 0), h->size);
	return 1;
}

int heaplocked (bheap_t *h) {
	return h->lock;
}

int heaplock (bheap_t *h) {
	return h->lock ? 0 : (h->lock = 1);
}

int heapunlock (bheap_t *h) {
	return h->lock ? !(h->lock = 0) : 0;
}

/*
 * 1. add element to bottom
 * 2. compare with parent; if not in correct order…
 * 3. swap with parent and continue 2
 */
int heapup (bheap_t *h, void *el) {
	size_t i, p;
	if (h->foot >= h->num)
		return 0;
	if (!heaplock (h))
		return 0;
	for (i = h->foot++; i; i = p) {
		p = _heap_parent (i);
		/* correct order, so we're done */
		if (_heap_cmp (h, p, el) >= 0)
			break;
		/* copy up */
		_heap_set_id (h, i, p);
	}
	/* now the order is correct, insert the item */
	_heap_set (h, i, el);
	return heapunlock (h);
}

/*
 * 1. replace the root with the last element
 * 2. compare root with children; if not in correct order…
 * 3. swap with the correct child (based on dir)
 */
int heapdownri (bheap_t *h, void *p, void *r, size_t i) {
	size_t c;
	HEAPDBG ("copy to %p", p);

	if (h->foot <= 0 || h->foot <= i)
		return 0;

	/* copy the root element to output */
	if (p)
		memcpy (p, _heap_item (h, i), h->size);

	for (; (c = _heap_left (i)) < h->foot; i = c) {
		/* find the bigger child */
		if (c + 1 < h->foot && _heap_cmp_id (h, c, c + 1) < 0)
			c++;
		HEAPDBG ("\tcompare last element [%zd] with [%zd] (larger child of [%zd])", h->foot, c, i);
		/* correct order, so we're done */
		if (_heap_cmp_id (h, h->foot, c) >= 0)
			break;
		/* copy up */
		HEAPDBG ("\t\tcopy [%zd] up to [%zd]", c, i);
		_heap_set_id (h, i, c);
	}
	/* now the order is correct, reinsert the last item */
	HEAPDBG ("\tcopy item [%zd] to %zd", r, i);
	_heap_set (h, i, r);
	return 1;
}

int heapdowni (bheap_t *h, void *p, size_t i) {
	int r = heapdownri (h, p, _heap_item (h, h->foot), i);
	h->foot--;
	return r;
}

int heapdown (bheap_t *h, void *p) {
	if (!heaplock (h) || !heapdowni (h, p, 0))
		return 0;
	heapunlock (h);
	return 1;
}

int heapsearch (bheap_t *h, void *p, size_t i, int (*match)(const void *, void *arg), void *arg) {
	for (; i < h->foot; i++) {
		if (match (_heap_item (h, i), arg)) {
			if (p)
				memcpy (p, _heap_item (h, i), h->size);
			return i + 1;
		}
	}
	return -1;
}

int heapdelete (bheap_t *h, int (*match)(const void *, void *), void *arg) {
	ssize_t i;
	size_t n = 0;
	if (h->foot < 1)
		return 0;
	if (!heaplock (h))
		return 0;
	for (i = h->foot - 1; i >= 0; i--) {
		if (match (_heap_item (h, i), arg)) {
			n++;
			heapdowni (h, NULL, i);
		}
	}
	heapunlock (h);
	return n;
}
