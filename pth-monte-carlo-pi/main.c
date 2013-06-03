#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <inttypes.h>

static const int STATE_SIZE = 8;
static long R = 65535;
static long R2 = 4294836225;

void *worker(void *arg)
{
	long sum = 0;
	long k = (long)arg;
	char statebuf[STATE_SIZE];
	struct random_data rd;
	initstate_r(k, statebuf, STATE_SIZE, &rd);

	for (long i = 0; i < k; i++) {
		long x, y;
		random_r(&rd, &x);
		random_r(&rd, &y);
		x &= R;
		y &= R;
		if (x*x + y*y < R2)
			sum++;
	}

	return (void*)sum;
}

int main(int argc, char *argv[])
{
	int num_threads = atoi(argv[1]);
	long num_points = atoi(argv[2]);

	pthread_t threads[num_threads];

	long a = num_points / num_threads;
	long b = num_points % num_threads;

	for (int i = 0; i < num_threads; i++) {
		long k = a + (i < b? 1:0);
		pthread_create(&threads[i], NULL, worker, (void*)k);
	}

	long sum = 0;
	for (int i = 0; i < num_threads; i++) {
		long psum;
		pthread_join(threads[i], (void**)(&psum));
		sum += psum;
	}

	printf("Pi is %lf\n", 4.0 * (double)sum / num_points);

	return 0;
}

