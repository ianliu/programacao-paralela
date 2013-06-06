#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>

static double dx = 0.0;
static double x0 = 0.0;
static long ns = 0;
static int nt = 1;
static double *samples = NULL;
static double global_sum = 0.0;

struct lock {
	enum lock_approach {
		BUSYWAIT,
		MUTEX,
		SEMAPHORE
	} ap;

	union {
		pthread_mutex_t mutex;
		sem_t sem;
		int busy;
	} l;
};

static struct lock lock;

void usage(char *prog) {
	fprintf(stderr, "Usage: %s -h|-d DELTA -0 X0 -n NUM_SAMPLES -a APPROACH\n", prog);
	fprintf(stderr, "   -d DELTA       The spacement between samples.\n");
	fprintf(stderr, "   -0 X0          The position for the first sample.\n");
	fprintf(stderr, "   -n NUM_SAMPLES The number of samples to read from stdin.\n");
	fprintf(stderr, "   -a APPROACH    One of BUSYWAIT, MUTEX or SEMAPHORE.\n");
	fprintf(stderr, "Environment variables:\n");
	fprintf(stderr, "   PTH_NUM_THREADS The number of threads to spawn (default 1).\n");
	exit(1);
}

void lock_init(struct lock *lock, enum lock_approach ap) {
	lock->ap = ap;
	switch (lock->ap) {
		case BUSYWAIT:
			lock->l.busy = 0;
			break;
		case MUTEX:
			pthread_mutex_init(&lock->l.mutex, NULL);
			break;
		case SEMAPHORE:
			sem_init(&lock->l.sem, 0, 1);
			break;
	}
}

void lock_destroy(struct lock *lock) {
	switch (lock->ap) {
		case BUSYWAIT:
			break;
		case MUTEX:
			pthread_mutex_destroy(&lock->l.mutex);
			break;
		case SEMAPHORE:
			sem_destroy(&lock->l.sem);
			break;
	}
}

void lock_lock(struct lock *lock) {
	switch (lock->ap) {
		case BUSYWAIT:
			while (lock->l.busy == 1);
			lock->l.busy = 1;
			break;
		case MUTEX:
			pthread_mutex_lock(&lock->l.mutex);
			break;
		case SEMAPHORE:
			sem_wait(&lock->l.sem);
			break;
	}
}

void lock_unlock(struct lock *lock) {
	switch (lock->ap) {
		case BUSYWAIT:
			lock->l.busy = 0;
			break;
		case MUTEX:
			pthread_mutex_unlock(&lock->l.mutex);
			break;
		case SEMAPHORE:
			sem_post(&lock->l.sem);
			break;
	}
}

void get_options(int argc, char *argv[]) {
	int opt;
	bool has_x0 = false;
	bool has_ap = false;
	while ((opt = getopt(argc, argv, "hd:0:n:a:")) != -1) {
		switch (opt) {
			case 'a':
				has_ap = true;
				if (strcmp(optarg, "BUSYWAIT") == 0)
					lock_init(&lock, BUSYWAIT);
				else if (strcmp(optarg, "MUTEX") == 0)
					lock_init(&lock, MUTEX);
				else if (strcmp(optarg, "SEMAPHORE") == 0)
					lock_init(&lock, SEMAPHORE);
				break;
			case 'd':
				dx = atof(optarg);
				break;
			case '0':
				has_x0 = true;
				x0 = atof(optarg);
				break;
			case 'n':
				ns = atoi(optarg);
				break;
			case 'h':
			default:
				usage(argv[0]);
		}
	}

	if (dx <= 0 || ns <= 0 || !has_x0 || !has_ap)
		usage(argv[0]);
}

void calc_indexes(long t, int *i0, int *iN) {
	int k = (ns - 1) / nt;
	int r = (ns - 1) % nt;
	if (t < r) {
		*i0 = t * (k + 1);
		*iN = *i0 + k + 1;
	} else {
		*i0 = (k + 1) * r + (t - r) * k;
		*iN = *i0 + k;
	}
	if (*iN >= ns - 1)
		*iN = ns - 1;
}

void *worker(void *arg) {
	long t = (long)arg;
	int i0, iN;
	double sum;
	calc_indexes(t, &i0, &iN);

	sum = samples[i0];
	for (int i = i0+1; i < iN-1; i++)
		sum += 2*samples[i];
	sum += samples[iN];

	lock_lock(&lock);
	global_sum += dx * sum / 2;
	lock_unlock(&lock);

	return NULL;
}

int main(int argc, char *argv[]) {
	char *nt_s = getenv("PTH_NUM_THREADS");
	if (nt_s)
		nt = atoi(nt_s);
	get_options(argc, argv);

	samples = malloc(sizeof(double)*ns);
	for (int i = 0; i < ns; i++)
		scanf("%lf", &samples[i]);

	pthread_t threads[nt];

	for (long i = 0; i < nt; i++)
		pthread_create(&threads[i], NULL, worker, (void*)i);

	for (int i = 0; i < nt; i++)
		pthread_join(threads[i], NULL);

	printf("%lf\n", global_sum);

	return 0;
}
