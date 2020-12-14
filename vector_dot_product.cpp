#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <cmath>
#include <iostream>
#include <ctime>
#include <algorithm> 
#include <vector> 

using namespace std;

void dotProduct(double *a,  double *b, int num_threads, int N)
{
	double sum_paral_red = 0.0;
	double sum_paral_atomic = 0.0;
	double sum_seq = 0.0;
	int i;

	double startReductionOMP;  
	double endReductionOMP;
	double startAtomic;
	double endAtomic;
	double startConseq;   
	double endConseq;

	omp_set_dynamic(0);

#pragma omp parallel num_threads(num_threads)
	if (omp_get_thread_num() == 0)
	{
		printf("\n\nParallel execution with %d threads and %d elements\n", omp_get_num_threads(), N);
	}

	startReductionOMP = omp_get_wtime();

	
#pragma omp parallel reduction(+:sum_paral_red) shared(a, b) private (i)  num_threads(num_threads)
	{	
		int id = omp_get_thread_num();
		int numtr = omp_get_num_threads();

		for (i = id; i < N; i = i + numtr)
		{
			sum_paral_red += a[i] * b[i];
		}
	}
	endReductionOMP = omp_get_wtime();
	printf("Reduction: %f seconds\n", endReductionOMP - startReductionOMP);

	// Atomic operations
	startAtomic = omp_get_wtime();
#pragma omp parallel shared(sum_paral_atomic, a, b) private (i) num_threads(num_threads)
	{	
		int id = omp_get_thread_num();
		int numtr = omp_get_num_threads();
		for (i = id; i < N; i = i + numtr)
		{
#pragma omp atomic
			sum_paral_atomic += a[i] * b[i];
		}
	}

	endAtomic = omp_get_wtime();
	printf("Atomic   : %f seconds\n", endAtomic - startAtomic);


	// Consequental computations
	startConseq = omp_get_wtime();
	for (i = 0;i < N; i++)
	{
		sum_seq += a[i] * b[i];
	}
	endConseq = omp_get_wtime();

	printf("Seq      : %f seconds\n\n", endConseq - startConseq);

	printf("Reduction result : %f\n", sum_paral_red);
	printf("Atomic add result: %f\n", sum_paral_atomic);
	printf("Sequence result  : %f\n", sum_seq);
	printf("\n=======================================================\n");
}

int main(int argc, char** argv)
{
	//const int N = atoi(argv[1]);
	const int N = 1000;
	double *a = (double*) malloc(N * sizeof(double));
        double *b = (double*) malloc(N * sizeof(double));
        int i;

        for (i = 0; i < N; i++)
        {
                a[i] = rand() % 10;
                b[i] = rand() % 10;
        }

	dotProduct(a, b, 1, N);
	dotProduct(a, b, 2, N);
	dotProduct(a, b, 4, N);
	dotProduct(a, b, 8, N);
	dotProduct(a, b, 16, N);
        dotProduct(a, b, 32, N);

}
