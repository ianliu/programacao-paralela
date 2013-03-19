#include <mpi.h>
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

int main(int argc, char *argv[])
{
	int rank;
	int n_ranks;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int *data;

	if (rank == 0)
		data = get_data(DATA_LEN);

	MPI_Finalize();
	return 0;
}
