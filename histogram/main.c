#include <limits.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Sets the data type.
 */
#define DATA_FMT "f"
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
 * Fetches data from stdin.
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
 * Fills the @sendcnts and @displs vectors to scatter the data.
 */
void get_scatter_info(int *sendcnts, int *displs, int n_ranks, int n)
{
	int sum = 0;
	int a = n / n_ranks;
	int b = n - a*n_ranks;
	for (int i = 0; i < n_ranks; i++) {
		sendcnts[i] = a + (b > 0? 1:0);
		displs[i] = sum;
		sum += sendcnts[i];
		b--;
	}
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

int main(int argc, char *argv[])
{
	int rank;
	int n_ranks;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int n_elements = DATA_LEN / n_ranks + 1;
	data_t data_bounds[2];
	data_t *datain;
	data_t dataout[n_elements];
	int displs[n_ranks];
	int sendcnts[n_ranks];

	if (rank == 0)
		datain = get_data(DATA_LEN, data_bounds);

	MPI_Bcast(data_bounds, 2, MPI_INT, 0, MPI_COMM_WORLD);

	get_scatter_info(sendcnts, displs, n_ranks, DATA_LEN);
	MPI_Scatterv(datain, sendcnts, displs, MPI_INT,
			dataout, n_elements, MPI_INT,
			0, MPI_COMM_WORLD);

	int *bucket = get_bucket_cache(BUCKET_LEN);
	histogram(dataout, sendcnts[rank], data_bounds, bucket, BUCKET_LEN);

	int *result;
	if (rank == 0)
		result = get_bucket_cache(BUCKET_LEN);

	MPI_Reduce(bucket, result, BUCKET_LEN,
			MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		printf("[Histogram result]\n");
		for (int i = 0; i < BUCKET_LEN; i++)
			printf("%d ", result[i]);
		puts("");
	}

	MPI_Finalize();
	return 0;
}
