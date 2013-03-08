#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define MAX_STRING 100

int main(int argc, char *argv[])
{
	char *greeting[MAX_STRING];
	int comm_sz;
	int my_rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	if (my_rank == 0) {
		printf("Greetings from process %d of %d!\n",
				my_rank, comm_sz);
		for (int i = 1; i < comm_sz; i++) {
			MPI_Recv(greeting, MAX_STRING, MPI_CHAR, i,
					0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("%s\n", greeting);
		}
	} else {
		sprintf(greeting, "Greetings from process %d of %d!",
				my_rank, comm_sz);
		MPI_Send(greeting, strlen(greeting) + 1, MPI_CHAR, 0, 0,
				MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
