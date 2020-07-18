#include <sys/types.h>
#include <sys/sysctl.h>

#include <stddef.h>

#include <leapsecondtable.h>

static const char name[] = "kern.leapsecondtable";

int
getleapsecondtable(int64_t *buf, size_t *lenp)
{
	size_t bytesize;
	int rv;

	bytesize = (*lenp) * sizeof(*buf);
	rv = sysctlbyname(name, buf, &bytesize, NULL, 0);
	*lenp = bytesize / sizeof(*buf);
	return rv;
}

int
setleapsecondtable(const int64_t *buf, size_t len)
{
	return sysctlbyname(
	        name, NULL, NULL, (int64_t *)buf, sizeof(*buf) * len);
}
