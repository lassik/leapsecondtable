#include "example.h"

static int64_t *
update(size_t *n)
{
	int64_t *leap;
	int64_t *newleap;
	size_t cap;

	leap = 0;
	for (cap = 16; cap <= 256; cap *= 2) {
		newleap = realloc(leap, sizeof(*leap) * cap);
		if (newleap == NULL)
			panic_errno("realloc");
		leap = newleap;
		memset(leap, 0, sizeof(*leap) * cap);
		*n = cap;
		if (getleapsecondtable(leap, n) == 0) {
			return leap;
		}
		if (errno != ENOMEM)
			panic_errno("getleapsecondtable");
	}
	free(leap);
	panic("too many");
	return NULL;
}

int
main(void)
{
	int64_t *leap;
	size_t n;

	leap = update(&n);
	print_table(leap, n);
	return 0;
}
