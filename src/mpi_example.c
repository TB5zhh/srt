#include <mpi.h> /* PROVIDES THE BASIC MPI DEFINITION AND TYPES */
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include "difftimeval.h"
#include "config.h"

struct Data
{
	struct timeval tv;
	char data[DATA_SIZE];
};

int main(int argc, char **argv)
{

	int my_rank;
	int partner;
	int size, i, t;
	MPI_Status stat;

	MPI_Init(&argc, &argv);					 /*START MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /*DETERMINE RANK OF THIS PROCESSOR*/
	MPI_Comm_size(MPI_COMM_WORLD, &size);	 /*DETERMINE TOTAL NUMBER OF PROCESSORS*/

	printf("begin, my rank:%d\n", my_rank);

	if (my_rank == 0)
	{
		struct Data *datas = malloc(TEST_TIME * sizeof(struct Data));
		memset(datas, 1, TEST_TIME * sizeof(struct Data));
		for (int i = 0; i < TEST_TIME; i++)
		{
			struct Data *data = datas + i;
			// printf("Address of data of rank0: %p, index: %d\n", data, i);
			struct timeval tv;
			MPI_Recv(data, sizeof(struct Data), MPI_BYTE, 1, 1, MPI_COMM_WORLD, &stat);
			snprintf(data->data, sizeof(data->data), "%s, num:%d", "hello world!", i);
			gettimeofday(&tv, NULL);
			printf("time:%fus, receive:%s\n", difftimeval(&tv, &data->tv), data->data);
			
			printf("Pause at 0\n");
			getchar();
		}
		free(datas);

		printf("=======================\n");

		for (int i = 0; i < TEST_TIME; i++)
		{
			struct Data *data = malloc(sizeof(struct Data));
			memset(data, 1, sizeof(struct Data));
			// printf("Address of data of rank0: %p, index: %d\n", data, i);
			struct timeval tv;
			MPI_Recv(data, sizeof(struct Data), MPI_BYTE, 1, 1, MPI_COMM_WORLD, &stat);
			snprintf(data->data, sizeof(data->data), "%s, num:%d", "hello world!", i);
			gettimeofday(&tv, NULL);
			printf("time:%fus, receive:%s\n", difftimeval(&tv, &data->tv), data->data);

			// printf("Pause at 0\n");
			// getchar();

			free(data);
		}
	}
	else if (my_rank == 1)
	{
		struct Data *datas = malloc(TEST_TIME * sizeof(struct Data));
		memset(datas, 1, TEST_TIME * sizeof(struct Data));
		for (int i = 0; i < TEST_TIME; i++)
		{
			struct Data *data = datas + i;
			// printf("Address of data of rank1: %p, index: %d\n", data, i);
			memset(data->data, 0, sizeof(data->data));
			snprintf(data->data, sizeof(data->data), "%s, num:%d", "hello world!", i);
			gettimeofday(&data->tv, NULL);
			MPI_Send(data, sizeof(struct Data), MPI_BYTE, 0, 1, MPI_COMM_WORLD);
		}
		free(datas);

		for (int i = 0; i < TEST_TIME; i++)
		{
			struct Data *data = malloc(sizeof(struct Data));
			memset(data, 1, sizeof(struct Data));
			// printf("Address of data of rank1: %p, index: %d\n", data, i);
			memset(data->data, 0, sizeof(data->data));
			snprintf(data->data, sizeof(data->data), "%s, num:%d", "hello world!", i);
			gettimeofday(&data->tv, NULL);
			MPI_Send(data, sizeof(struct Data), MPI_BYTE, 0, 1, MPI_COMM_WORLD);
			free(data);
		}
	}

	if (my_rank == 0)
		printf("That is all for now!\n");
	MPI_Finalize(); /* EXIT MPI */
}
