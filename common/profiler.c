#include "profiler.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void profiler_init(Profiler *prof, const char *name)
{
	prof->name = strndup(name, 30);
	prof->runs = 0;
	prof->total = 0.0;
}

void profiler_free(Profiler *prof)
{
	free(prof->name);
}

void profiler_print(Profiler *prof)
{
	printf("%-15s %12"PRIu64" %ld\n",
	       prof->name, prof->total, prof->runs);
}
