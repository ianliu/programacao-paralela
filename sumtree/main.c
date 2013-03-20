#include <common.h>
#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static int DATA_LEN = 10;

int *get_data(int len)
{
	int *data = malloc(sizeof(int) * len);
	int *p = data;
	while (len--)
		scanf("%d", p++);
	return data;
}

int calc_sum(int *v, int n)
{
	int s = 0;
	while (n--)
		s += *(v++);
	return s;
}

typedef enum {
	RECEIVER,
	SENDER,
	NONE
} ProcessType;

int main(int argc, char *argv[])
{
	int rank;
	int n_ranks;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int *data;
	int sendcnts[n_ranks];
	int displs[n_ranks];
	int outlen = DATA_LEN / n_ranks + 1;
	int dataout[outlen];

	if (rank == 0)
		data = get_data(DATA_LEN);

	get_scatter_info(sendcnts, displs, n_ranks, DATA_LEN);

	MPI_Scatterv(data, sendcnts, displs, MPI_INT,
			dataout, outlen, MPI_INT,
			0, MPI_COMM_WORLD);

	int rec;
	int sum = calc_sum(dataout, sendcnts[rank]);

	int twoT = 2;
	ProcessType type = NONE;
	bool done = false;
	int proc;

	while (!done) {
		if (rank % twoT == 0) {
			type = RECEIVER;
			proc = rank + twoT/2;
		} else if ((rank - twoT/2) % twoT == 0) {
			type = SENDER;
			proc = rank - twoT/2;
		} else {
			type = NONE;
		}

		switch (type) {
			case RECEIVER:
				if (proc < n_ranks) {
					MPI_Recv(&rec, 1, MPI_INT, proc,
							0, MPI_COMM_WORLD, NULL);
					sum += rec;
				}
				break;
			case SENDER:
				MPI_Send(&sum, 1, MPI_INT, proc,
						0, MPI_COMM_WORLD);
				done = true;
				break;
			default:
				done = true;
				break;
		}

		if (twoT >= n_ranks)
			done = true;

		twoT *= 2;
	}

	if (rank == 0)
		printf("%d\n", sum);

	MPI_Finalize();
	return 0;
}
