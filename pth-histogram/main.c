#include <common.h>
#include <float.h>
#include <math.h>
#include <profiler.h>
#include <pthread.h>
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
static int DATA_LEN;

/**
 * The number of buckets in this histogram.
 */
static int BUCKET_LEN;

/**
 * The number of threads to spawn.
 */
static int NUM_THREADS = 4;

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
 * Returns a zeroed vector of size @len to hold the result of the
 * histogram.
 */
int *get_bucket(int len)
{
	int *cache = malloc(sizeof(int) * len);
	memset(cache, 0, sizeof(int) * len);
	return cache;
}

struct workerdata {
	int tid, numt;
	data_t *data;
	int data_len;
	data_t *data_bounds;
	int bucket_len;
	int *global_bucket;
};

void *worker_method(void *arg) {
	struct workerdata *wd = arg;
	double dmin = (double)wd->data_bounds[0];
	double delt = (double)(wd->data_bounds[1] - dmin);

	int i0, iN; // Region to process
	int k = wd->data_len / wd->numt;
	int r = wd->data_len % wd->numt;

	if (wd->tid < r) {
		i0 = (k+1)*wd->tid;
		iN = i0 + k+1;
	} else {
		i0 = k*wd->tid + r;
		iN = i0 + k;
	}

	int *local_bucket = get_bucket(wd->bucket_len);
	for (int i = i0; i < iN; i++) {
		double r = wd->bucket_len * (wd->data[i] - dmin) / delt;
		int b = (int)floor(r);
		if (b == wd->bucket_len)
			b--;
		if (b >= 0 && b < wd->bucket_len)
			local_bucket[b]++;
	}

	for (int i = 0; i < wd->bucket_len; i++)
		__sync_fetch_and_add(&wd->global_bucket[i], local_bucket[i]);
}

/**
 * Calculate the histogram and place the result in @bucket.
 */
void histogram(data_t *data, int data_len, data_t *data_bounds,
		int *bucket, int bucket_len, int numt)
{
	pthread_t threads[numt];
	struct workerdata tdata[numt];

	for (int i = 0; i < numt; i++) {
		tdata[i].tid = i;
		tdata[i].numt = numt;
		tdata[i].data = data;
		tdata[i].data_len = data_len;
		tdata[i].data_bounds = data_bounds;
		tdata[i].bucket_len = bucket_len;
		tdata[i].global_bucket = bucket;

		pthread_create(&threads[i], NULL, worker_method, &tdata[i]);
	}

	for (int i = 0; i < numt; i++)
		pthread_join(threads[i], NULL);
}

/**
 * Prints the usage string to stderr and exits.
 */
void usage(const char *prog)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "  %s -h\n", prog);
	fprintf(stderr, "  %s -n DATA_SIZE -b BUCKET_SIZE [-t NUM_THREADS]\n", prog);
	exit(1);
}

/**
 * Get the command line arguments and change global variables.
 */
void get_options(int argc, char **argv)
{
	int op;
	while ((op = getopt(argc, argv, "hn:b:t:")) != -1) {
		switch (op) {
			case 'n':
				DATA_LEN = atoi(optarg);
				break;
			case 'b':
				BUCKET_LEN = atoi(optarg);
				break;
			case 't':
				NUM_THREADS = atoi(optarg);
				break;
			case 'h':
			default:
				usage(argv[0]);
		}
	}

	if (DATA_LEN <= 0 || BUCKET_LEN <= 0 || NUM_THREADS <= 0)
		usage(argv[0]);
}

int main(int argc, char *argv[])
{
	Profiler hist, main;
	profiler_init(&hist, "histogram");
	profiler_init(&main, "main");

	get_options(argc, argv);

	profiler_start(&main);

	data_t data_bounds[2] = {DATA_MAX, DATA_MIN};
	data_t *datain = get_data(DATA_LEN, data_bounds);
	int *bucket = get_bucket(BUCKET_LEN);

	profiler_start(&hist);
	histogram(datain, DATA_LEN, data_bounds, bucket, BUCKET_LEN, NUM_THREADS);
	profiler_stop(&hist);

	double dmin = (double)data_bounds[0];
	double delt = (double)(data_bounds[1] - dmin);
	printf("Histogram:");
	for (int i = 0; i < BUCKET_LEN; i++)
		printf(" %d", bucket[i]);
	printf("\n");

	profiler_stop(&main);

	printf("Profiling data\n");
	printf("==============\n");
	profiler_print(&main);
	profiler_print(&hist);

	return 0;
}
