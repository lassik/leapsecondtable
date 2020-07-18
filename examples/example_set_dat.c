#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>

#include "example.h"

// <http://cr.yp.to/libtai/tai64.html>
static int64_t
tai_unpack(unsigned char *bytes)
{
	const uint64_t two_to_the_power_of_62 = 4611686018427387904ULL;
	uint64_t val;

	val = bytes[0];
	val <<= 8;
	val |= bytes[1];
	val <<= 8;
	val |= bytes[2];
	val <<= 8;
	val |= bytes[3];
	val <<= 8;
	val |= bytes[4];
	val <<= 8;
	val |= bytes[5];
	val <<= 8;
	val |= bytes[6];
	val <<= 8;
	val |= bytes[7];
	return (int64_t)(val - two_to_the_power_of_62);
}

int
main(int argc, char **argv)
{
	static int64_t leap[64];
	struct stat st;
	size_t n, i;
	ssize_t nr;
	int fd;

	if (argc != 2)
		panic("usage");
	if ((fd = open(argv[1], O_RDONLY)) == -1)
		panic_errno("open");
	if (fstat(fd, &st) == -1)
		panic_errno("fstat");
	nr = read(fd, &leap[1], sizeof(leap) - sizeof(leap[0]));
	if (nr < 0)
		panic_errno("read");
	if (close(fd) == -1)
		panic_errno("close");
	n = 1 + (((size_t)nr) / sizeof(*leap));
	for (i = 1; i < n; i++)
		leap[i] = tai_unpack((unsigned char *)&leap[i]);
	print_table(leap, n);
	if (setleapsecondtable(leap, n) == -1)
		panic_errno("setleapsecondtable");
	return 0;
}
