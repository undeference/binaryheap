#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <bheap.c>

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
			(int)((i + 1) & ~1), "",
			p[i], x,
			c, y,
			c + 1, z);
	}
}

int bhcmp (const void *a, const void *b) {
	return *(long *)a - *(long *)b;
}

#include <locale.h>
void printer (const void *in, char *out, size_t len) {
	snprintf (out, len, "%'ld", *(long *)in);
}

#define TOTAL 48
#define MAXCNT 32
int main (int argc, char **argv) {
	long nums[TOTAL], cur, prev;
	size_t i;
	bheap_t *heap = heapalloc (-1, MAXCNT, sizeof (long), bhcmp);
	setlocale (LC_NUMERIC, "");
	/* set up our random list */
	for (i = 0; i < TOTAL; i++)
		nums[i] = random ();
	/* check length */
	for (i = 0; i < MAXCNT; i++)
		heapup (heap, &nums[i]);
	assert (heapcount (heap) == MAXCNT);
	heapdump (heap, printer);
	/* search, delete */
	/* peek, remove, reinsert, reremove */
	heapfree (heap);
}
