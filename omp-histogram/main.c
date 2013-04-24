#include <common.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Sets the data type.
 */
#define DATA_FMT "f"
#define DATA_MAX FLT_MAX
#define DATA_MIN FLT_MIN
typedef float data_t;

/**
 * Number of data points to calculate the histogram.
 */
static int DATA_LEN = 10;

/**
 * The number of buckets in this histogram.
 */
static int BUCKET_LEN = 4;

/**
 * Fetches data from stdin and sets the @data_bounds, ie
 * data_bounds[0] = min(data),
 * data_bounds[1] = max(data).
 */
data_t *get_data(int len, data_t *data_bounds)
{
	data_t *data = malloc(sizeof(data_t) * len);
	data_t *p = data;
	while (len--) {
		scanf("%" DATA_FMT, p);
		if (*p < data_bounds[0])
			data_bounds[0] = *p;
		else if (*p > data_bounds[1])
			data_bounds[1] = *p;
		p++;
	}
	return data;
}

/**
 * Returns a zeroed vector of size @len to cache the partial result of the
 * histogram.
 */
int *get_bucket_cache(int len)
{
	int *cache = malloc(sizeof(int) * len);
	memset(cache, 0, sizeof(int) * len);
	return cache;
}

/**
 * Calculate the histogram for the local problem given by @local_data and place
 * the result in @bucket_cache.
 */
void histogram(data_t *local_data, int local_data_len, data_t *data_bounds,
		int *bucket_cache, int cache_len)
{
	double dmin = (double)data_bounds[0];
	double delt = (double)(data_bounds[1] - dmin);

	for (int i = 0; i < local_data_len; i++) {
		double r = cache_len * (local_data[i] - dmin) / delt;
		int b = (int)floor(r);
		if (b == cache_len)
			b--;
		if (b >= 0 && b < cache_len)
			bucket_cache[b]++;
	}
}

/**
 * Prints the usage string to stderr and exits.
 */
void usage(const char *prog)
{
	fprintf(stderr, "Usage: %s [-h] | [-n DATA SIZE] [-b BUCKET SIZE]\n", prog);
	exit(1);
}

/**
 * Get the command line arguments and change global variables.
 */
void get_options(int argc, char **argv)
{
	int op;
	while ((op = getopt(argc, argv, "hn:b:")) != -1) {
		switch (op) {
			case 'n':
				DATA_LEN = atoi(optarg);
				break;
			case 'b':
				BUCKET_LEN = atoi(optarg);
				break;
			case 'h':
			default:
				usage(argv[0]);
		}
	}
}

int main(int argc, char *argv[])
{
	int rank;
	int n_ranks;

	get_options(argc, argv);

	int n_elements = DATA_LEN / n_ranks + 1;
	data_t data_bounds[2] = {DATA_MAX, DATA_MIN};
	data_t *datain;
	data_t dataout[n_elements];
	int displs[n_ranks];
	int sendcnts[n_ranks];

	if (rank == 0)
		datain = get_data(DATA_LEN, data_bounds);

	int *bucket = get_bucket_cache(BUCKET_LEN);
	histogram(dataout, sendcnts[rank], data_bounds, bucket, BUCKET_LEN);

	int *result;
	if (rank == 0)
		result = get_bucket_cache(BUCKET_LEN);

	if (rank == 0) {
		double dmin = (double)data_bounds[0];
		double delt = (double)(data_bounds[1] - dmin);
		printf("len %d\n", BUCKET_LEN);
		printf("min %lf\n", dmin);
		printf("del %lf\n", delt);
		for (int i = 0; i < BUCKET_LEN; i++)
			printf("%d ", result[i]);
		printf("\n");
	}

	return 0;
}
