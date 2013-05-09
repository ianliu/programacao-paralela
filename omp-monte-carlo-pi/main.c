#include <common.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Sets the data type.
 */
#define DATA_FMT "lf"
#define DATA_MAX DBL_MAX
#define DATA_MIN DBL_MIN
typedef float data_t;

/**
 * Sets the tosses type.
 */
#define TOSS_FMT "lld"
#define atotoss_t(s) atoll(s)
typedef unsigned long long int toss_t;

#define STATE_SIZE 8

/**
 * Number of random points to use in the Monte-Carlo simulation.
 */
static toss_t NUM_POINTS = 0;

/**
 * Prints the usage string to stderr and exits.
 */
void usage(const char *prog)
{
	fprintf(stderr, "Usage: %s -h | -n NUM POINTS\n", prog);
	exit(1);
}

/**
 * Get the command line arguments and change global variables.
 */
void get_options(int argc, char **argv)
{
	int op;
	while ((op = getopt(argc, argv, "hn:")) != -1) {
		switch (op) {
			case 'n':
				NUM_POINTS = atotoss_t(optarg);
				break;
			case 'h':
			default:
				usage(argv[0]);
		}
	}

	if (NUM_POINTS <= 0)
		usage(argv[0]);
}

int main(int argc, char *argv[])
{
	get_options(argc, argv);

	/* Profiler monte_carlo; */
	/* profiler_init(&monte_carlo, "monte_carlo"); */

	/* profiler_start(&monte_carlo); */
	toss_t sum = 0;
	#pragma omp parallel
	{
		char statebuf[STATE_SIZE];
		struct random_data rd;
		initstate_r(omp_get_thread_num(),
				statebuf, STATE_SIZE, &rd);

		#pragma omp for reduction(+:sum)
		for (int i = 0; i < NUM_POINTS; i++) {
			int32_t xx, yy;
			random_r(&rd, &xx);
			random_r(&rd, &yy);
			double x = 2.0 * (double)xx / RAND_MAX - 1.0;
			double y = 2.0 * (double)yy / RAND_MAX - 1.0;
			if (x*x + y*y < 1)
				sum += 1;
		}
	}
	/* profiler_stop(&monte_carlo); */

	/* profiler_print(&monte_carlo); */
	printf("%"DATA_FMT"\n", (data_t)sum/NUM_POINTS * 4.0);

	return 0;
}
