#include <string.h>
#include <bheap.h>

#ifdef HEAPDEBUG
#include <stdio.h>
#define HEAPDBG(f,...) \
	fprintf (stderr, "%s:%d %s() " f "\n", \
		__FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define HEAPDBG(f,...)
#endif

bheap_t *heapalloc (int dir, size_t nmemb, size_t size, int (*cmp)(const void *, const void *)) {
	bheap_t *h = malloc (sizeof (bheap_t) + nmemb * size);
	h->dir = dir;
	h->num = nmemb;
	h->size = size;
	h->cmp = cmp;
	h->foot = 0;
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

/*
 * 1. add element to bottom
 * 2. compare with parent; if not in correct order…
 * 3. swap with parent and continue 2
 */
void heapup (bheap_t *h, void *el) {
	size_t i, p;
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
}

/*
 * 1. replace the root with the last element
 * 2. compare root with children; if not in correct order…
 * 3. swap with the correct child (based on dir)
 */
int heapdowni (bheap_t *h, void *p, size_t i) {
	size_t c;
	HEAPDBG ("copy to %p", p);

	if (h->foot == 0)
		return 0;

	/* copy the root element to output */
	memcpy (p, _heap_item (h, i), h->size);

	if (--h->foot == 0)
		return 1;
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
	HEAPDBG ("\tcopy last element [%zd] to %zd", h->foot, i);
	_heap_set_id (h, i, h->foot);
	return 1;
}

int heapdown (bheap_t *h, void *p) {
	return heapdowni (h, p, (size_t i)0);
}

#ifndef NDEBUG
int heapverify (bheap_t *h) {
	size_t i;
	if (h->foot < 2)
		return 1;
	for (i = h->foot - 1; i; i--) {
		if (_heap_cmp_id (h, _heap_parent (i), i) < 0) {
			HEAPDBG ("heap verify failed at id#%zd\n", i);
			return 0;
		}
	}
	return 1;
}

void heapdump (bheap_t *h, void (*out) (const void *, char *, size_t)) {
	// should be ceil(log2(h->foot))?
	size_t p[h->foot], n, i, c;
	char x[17], y[17], z[17];
	p[0] = 0;
	n = 1;
	for (i = 0; i < n && p[i] < h->foot; i++) {
		out (_heap_item (h, p[i]), x, sizeof (x));
		c = _heap_left (p[i]);
		if (c < h->foot) {
			p[n++] = c;
			out (_heap_item (h, c), y, sizeof (y));
			if (c + 1 < h->foot) {
				p[n++] = c + 1;
				out (_heap_item (h, c + 1), z, sizeof (z));
			} else
				z[0] = '\0';
		} else
			y[0] = z[0] = '\0';
		fprintf (stderr, "%*s[%zd] %s [[%zd] %s, [%zd] %s]\n",
		// 0; 1,2; 3,4; 5,6; 7,8
			(i + 1 & ~1), "",
			p[i], x,
			c, y,
			c + 1, z);
	}
}
#endif