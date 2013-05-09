#include <common.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

/**
 * Returns a uniformely random point in [-1, 1]x[-1, 1]
 */
void get_random_point(data_t point[])
{
	point[0] = 2*(data_t)random() / RAND_MAX - 1.0;
	point[1] = 2*(data_t)random() / RAND_MAX - 1.0;
}

bool is_point_in_circle(data_t point[])
{
	data_t x = point[0], y = point[1];
	return x*x + y*y < 1.0;
}

int main(int argc, char *argv[])
{
	int rank;
	int n_ranks;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	get_options(argc, argv);

	srandom(rank);
	int rest = NUM_POINTS % n_ranks;
	toss_t local_points = NUM_POINTS / n_ranks + (rank >= rest? 0:1);
	toss_t local_sum = 0;
	data_t point[2];

	for (int i = 0; i < local_points; i++) {
		get_random_point(point);
		if (is_point_in_circle(point))
			local_sum++;
	}

	toss_t global_sum = 0;
	MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		printf("Got %"TOSS_FMT" points of %"TOSS_FMT" inside the circle.\n",
				global_sum, NUM_POINTS);
		printf("Pi is estimated to be: %" DATA_FMT ".\n",
				(data_t)global_sum / NUM_POINTS * 4.0);
	}

	MPI_Finalize();
	return 0;
}
