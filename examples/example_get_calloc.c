#include "example.h"

int
main(void)
{
	int64_t *leap;
	size_t n;

	if (getleapsecondtable(NULL, &n) == -1) {
		if (errno != ENOMEM)
			panic_errno("getleapsecondtable");
	}
	if ((leap = calloc(n, sizeof(*leap))) == NULL)
		panic_errno("calloc");
	if (getleapsecondtable(leap, &n) == -1)
		panic_errno("getleapsecondtable");
	print_table(leap, n);
	return 0;
}
