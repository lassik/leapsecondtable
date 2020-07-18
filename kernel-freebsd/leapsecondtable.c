#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/module.h>
#include <sys/sysctl.h>
#include <sys/kernel.h>

static struct sysctl_ctx_list ctx;
static size_t leapsecondcount;
static int64_t leapsecondtable[64] = {

        1483228836LL, // Last known good timestamp.

        78796810LL,
        94694411LL,
        126230412LL,
        157766413LL,
        189302414LL,
        220924815LL,
        252460816LL,
        283996817LL,
        315532818LL,
        362793619LL,
        394329620LL,
        425865621LL,
        489024022LL,
        567993623LL,
        631152024LL,
        662688025LL,
        709948826LL,
        741484827LL,
        773020828LL,
        820454429LL,
        867715230LL,
        915148831LL,
        1136073632LL,
        1230768033LL,
        1341100834LL,
        1435708835LL,
        1483228836LL,

};
static const size_t leapsecondmax =
        sizeof(leapsecondtable) / sizeof(leapsecondtable[0]);

static int
validate(int64_t *table, size_t n)
{
	int64_t last, value;
	size_t k;

	if (n < 1)
		return (EINVAL);
	if (table[0] < 0)
		return (EINVAL);
	last = 0;
	for (k = 1; k < n; k++) {
		value = table[k];
		if (value < 0) {
			if (value == INT64_MIN)
				return (EINVAL);
			value = -value;
		}
		if (value <= last)
			return (EINVAL);
		last = value;
	}
	return (0);
}

static int handle_sysctl(SYSCTL_HANDLER_ARGS)
{
	int64_t *newtable;
	size_t newcount, bytesize;
	int error;

	error = 0;
	bytesize = leapsecondcount * sizeof(int64_t);
	// req->oldlen = bytesize;
	if (req->oldptr != NULL) {
		error = SYSCTL_OUT(req, leapsecondtable, bytesize);
		if (error)
			return (error);
	}
	if (req->newptr != NULL) {
		bytesize = req->newlen;
		if (bytesize == 0)
			return (EINVAL);
		if (bytesize % sizeof(int64_t) != 0)
			return (EINVAL);
		newcount = bytesize / sizeof(int64_t);
		if (newcount > leapsecondmax)
			return (ERANGE);
		newtable = malloc(bytesize, M_TEMP, M_WAITOK);
		error = SYSCTL_IN(req, newtable, bytesize);
		if (!error)
			error = validate(newtable, newcount);
		if (error) {
			free(newtable, M_TEMP);
			return (error);
		}
		bzero(leapsecondtable, sizeof(leapsecondtable));
		memcpy(leapsecondtable, newtable, sizeof(int64_t) * newcount);
		leapsecondcount = newcount;
	}
	return (error);
}

static int
initialize(void)
{
	size_t n;

	for (n = 0; n < leapsecondmax; n++) {
		if (!leapsecondtable[n])
			break;
	}
	leapsecondcount = n;
	sysctl_ctx_init(&ctx);
	SYSCTL_ADD_PROC(&ctx, SYSCTL_STATIC_CHILDREN(_kern), OID_AUTO,
	        "leapsecondtable", CTLTYPE_OPAQUE | CTLFLAG_RW, NULL, 0,
	        handle_sysctl, "", "leap second table");
	return (0);
}

static int
finalize(void)
{
	sysctl_ctx_free(&ctx);
	return (0);
}

static int
load(module_t mod, int cmd, void *arg)
{
	switch (cmd) {
	case MOD_LOAD:
		return (initialize());
	case MOD_UNLOAD:
		return (finalize());
	default:
		return (EOPNOTSUPP);
	}
}

static moduledata_t mod_data = {"leapsecondtable", load, 0};

DECLARE_MODULE(leapsecondtable, mod_data, SI_SUB_EXEC, SI_ORDER_ANY);
