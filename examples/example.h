#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <leapsecondtable.h>

static void
panic(const char *msg)
{
	fprintf(stderr, "error: %s\n", msg);
	exit(2);
}

static void
panic_errno(const char *func)
{
	fprintf(stderr, "error: %s(): %s\n", func, strerror(errno));
	exit(2);
}

static void
print_table(const int64_t *leap, size_t n)
{
	for (size_t i = 0; i < n; i++)
		printf("%" PRId64 "\n", leap[i]);
	printf("%zu entries\n", n);
	fflush(stdout);
}
