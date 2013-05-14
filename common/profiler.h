#ifndef __PROFILER_H__
#define __PROFILER_H__

#define __STDC_FORMAT_MACROS
#include <time.h>
#include <inttypes.h>

typedef struct _Profiler Profiler;

struct _Profiler {
	char *name;
	long runs;
	uint64_t total;
	uint64_t t0;
};

void profiler_init(Profiler *prof, const char *name);

void profiler_free(Profiler *prof);

void profiler_print(Profiler *prof);

static __inline__ uint64_t getticks(void)
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return tp.tv_sec*1000000000 + tp.tv_nsec;
}

static __inline__ void profiler_start(Profiler *prof)
{
	prof->t0 = getticks();
	prof->runs++;
}

static __inline__ void profiler_stop(Profiler *prof)
{
	prof->total += getticks() - prof->t0;
}

#endif /* end of include guard: __PROFILER_H__ */
