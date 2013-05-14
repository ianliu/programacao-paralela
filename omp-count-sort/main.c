#include <omp.h>
#include <profiler.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int NUM_POINTS = 0;
static bool USE_QSORT = false;

void count_sort(int a[], int n)
{
	int i, j, count;
	int *temp = malloc(n*sizeof(int));

	#pragma omp parallel for shared(a, n, temp) private(i, j, count)
	for (i = 0; i < n; i++) {
		count = 0;
		for (j = 0; j < n; j++)
			if (a[j] < a[i])
				count++;
			else if (a[j] == a[i] && j < i)
				count++;
		temp[count] = a[i];
	}

	#pragma omp parallel for shared(a, n, temp) private(i)
	for (i = 0; i < n; i++)
		a[i] = temp[i];

	free(temp);
}

void print_v(int a[], int n)
{
	for (int i = 0; i < n; i++)
		printf("%d\n", a[i]);
}

void usage(char *prog)
{
	fprintf(stderr, "Usage: %s -h | -n NUM_POINTS [-q]\n", prog);
	fprintf(stderr, "    -n      Number of points to read from stdin.\n");
	fprintf(stderr, "    -q      Use qsort method instead.\n");
	exit(1);
}

void get_options(int argc, char *argv[])
{
	int opt;
	while ((opt = getopt(argc, argv, "hn:q")) != -1) {
		switch (opt) {
			case 'h':
				usage(argv[0]);
				break;
			case 'n':
				NUM_POINTS = atoi(optarg);
				break;
			case 'q':
				USE_QSORT = true;
				break;
			default:
				usage(argv[0]);
				break;
		}
	}

	if (NUM_POINTS <= 0)
		usage(argv[0]);
}

int cmp(const void *aa, const void *bb)
{
	int a = *(int*)aa;
	int b = *(int*)bb;
	return a - b;
}

int main(int argc, char *argv[])
{
	int n, *a;
	Profiler prof_sort;
	profiler_init(&prof_sort, "sort");

	get_options(argc, argv);

	n = NUM_POINTS;
	a = malloc(n*sizeof(int));
	for (int i = 0; i < n; i++)
		scanf("%d", &a[i]);

	profiler_start(&prof_sort);
	if (USE_QSORT)
		qsort(a, n, sizeof(int), cmp);
	else
		count_sort(a, n);
	profiler_stop(&prof_sort);

	fprintf(stderr, "%" PRIu64 "\n", prof_sort.total);
	profiler_free(&prof_sort);

	print_v(a, n);

	return 0;
}
