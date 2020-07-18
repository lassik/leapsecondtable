#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <leapsecondtable.h>

static const char filename[] = "/etc/leapsecs.dat";

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
getleapsecondtable(int64_t *buf, size_t *lenp)
{
	struct stat st;
	ssize_t nr;
	size_t n, i;
	size_t len;
	int fd, save;

	len = 0;
	if (buf != NULL) {
		len = *lenp;
		if ((len < 1) || (len > SIZE_MAX / sizeof(*buf))) {
			errno = EINVAL;
			return -1;
		}
	}
	if ((fd = open(filename, O_RDONLY | O_NDELAY)) == -1)
		return -1;
	if (fstat(fd, &st) == -1) {
		save = errno;
		close(fd);
		errno = save;
		return -1;
	}
	if (st.st_size % sizeof(*buf) != 0) {
		close(fd);
		errno = EINVAL;
		return -1;
	}
	n = st.st_size / sizeof(*buf);
	if ((buf != NULL) && (n >= len)) {
		close(fd);
		errno = ENOMEM;
		return -1;
	}
	n++;
	if (buf == NULL) {
		if (close(fd) == -1)
			return -1;
		*lenp = len = n;
		return 0;
	}
	memset(buf, 0, sizeof(*buf) * len);
	if ((nr = read(fd, &buf[1], st.st_size)) < 0) {
		save = errno;
		close(fd);
		errno = save;
		return -1;
	}
	if (close(fd) == -1)
		return -1;
	if ((size_t)nr != (n - 1) * sizeof(*buf)) {
		close(fd);
		errno = EINVAL;
		return -1;
	}
	buf[0] = 0;
	for (i = 1; i < n; i++)
		buf[i] = tai_unpack((unsigned char *)&buf[i]);
	*lenp = len = n;
	return 0;
}

int
setleapsecondtable(const int64_t *buf, size_t len)
{
	(void)buf;
	(void)len;
	errno = EOPNOTSUPP;
	return -1;
}
