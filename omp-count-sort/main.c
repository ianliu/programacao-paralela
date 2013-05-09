#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char *argv[])
{
	int n, *a;

	n = atoi(argv[1]);
	a = malloc(n*sizeof(int));
	for (int i = 0; i < n; i++)
		scanf("%d", &a[i]);

	count_sort(a, n);
	print_v(a, n);

	return 0;
}
