#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Number of data points to calculate the histogram
 */
static int DATA_LEN = 10;

/**
 * The intervals of the bucket are:
 * [BUCKET_START, BUCKET_START + BUCKET_STEP,
 *    ..., BUCKET_START + (BUCKET_LEN - 1) * BUCKET_STEP]
 */
static int BUCKET_START = 0;
static int BUCKET_STEP = 10;
static int BUCKET_LEN = 50;

/**
 * Fetches data from stdin.
 */
int *get_data(int len)
{
	int *data = malloc(sizeof(int) * len);
	int *p = data;
	while (len--)
		scanf("%d", p++);
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

int main(int argc, char *argv[])
{
	int rank;
	int n_ranks;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int *datain;
	int displs[n_ranks];
	int sendcnts[n_ranks];
	int n_elements = DATA_LEN / n_ranks + 1;
	int dataout[n_elements];

	if (rank == 0)
		datain = get_data(DATA_LEN);

	get_scatter_info(sendcnts, displs, n_ranks, DATA_LEN);
	MPI_Scatterv(datain, sendcnts, displs, MPI_INT,
			dataout, n_elements, MPI_INT,
			0, MPI_COMM_WORLD);

	printf("[Rank %d]\n", rank);
	for (int i = 0; i < sendcnts[rank]; i++)
		printf("%d ", dataout[i]);
	puts("");

	MPI_Finalize();
	return 0;
}
