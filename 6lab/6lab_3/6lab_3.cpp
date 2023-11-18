#include <iostream>
#include <omp.h>
#include <vector>

using matrix = std::vector<std::vector<int>>;
const int N = 250;
const int NUM_THREADS = 4;

void MatrixMultiplicationParallel(matrix& A, matrix& B, matrix& C)
{
	double startTime, endTime;
	startTime = omp_get_wtime();
#pragma omp parallel for num_threads(NUM_THREADS)
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			int sum = 0;
			for (int k = 0; k < N; k++)
			{
				sum += A[i][k] * B[k][j];
			}
#pragma omp critical
			C[i][j] = sum;
		}
	}
	endTime = omp_get_wtime();
	printf("Parrallel: Время выполнения = %.6f секунд\n", endTime - startTime);
}

void MatrixMultiplication(matrix& A, matrix& B, matrix& C)
{
	double startTime, endTime;
	startTime = omp_get_wtime();
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			int sum = 0;
			for (int k = 0; k < N; k++)
			{
				sum += A[i][k] * B[k][j];
			}
			C[i][j] = sum;
		}
	}
	endTime = omp_get_wtime();
	printf("Default: Время выполнения = %.6f секунд\n", endTime - startTime);
}

int main()
{

	setlocale(LC_ALL, "Russian");
	matrix A(N, std::vector<int>(N));
	matrix B(N, std::vector<int>(N));
	matrix C(N, std::vector<int>(N));

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			A[j][i] = i + j;
		}
	}

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			B[i][j] = j + i;
		}

	}

	MatrixMultiplication(A, B, C);
	MatrixMultiplicationParallel(A, B, C);


	return 0;
}