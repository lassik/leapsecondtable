#include "example.h"

int
main(void)
{
	static int64_t leap[64];
	size_t n;

	n = sizeof(leap) / sizeof(leap[0]);
	if (getleapsecondtable(leap, &n) == -1)
		panic_errno("getleapsecondtable");
	print_table(leap, n);
	return 0;
}
