#include "common.h"

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
