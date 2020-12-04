#include <mpi.h>   /* PROVIDES THE BASIC MPI DEFINITION AND TYPES */
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#define TEST_TIME (32)
double difftimeval(const struct timeval *start, const struct timeval *end)
{
	double d;
	time_t s;
	suseconds_t u;

	s = start->tv_sec - end->tv_sec;
	u = start->tv_usec - end->tv_usec;

	d = s;
	d *= 1000000.0;//1 秒 = 10^6 微秒
	d += u;

	return d;
}
struct Data {
	struct timeval tv;
	char data[4096*1024];
};

int main(int argc, char **argv) {

	int my_rank; 
	int partner;
	int size, i,t;
	MPI_Status stat;

	MPI_Init(&argc, &argv); /*START MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /*DETERMINE RANK OF THIS PROCESSOR*/
	MPI_Comm_size(MPI_COMM_WORLD, &size); /*DETERMINE TOTAL NUMBER OF PROCESSORS*/

	printf("begin, my rank:%d\n", my_rank);

	if (my_rank == 0) {
		for(int i=0; i<TEST_TIME; i++) {
			struct Data* data = malloc(sizeof(struct Data));
			struct timeval tv;
			MPI_Recv(data, sizeof(struct Data), MPI_BYTE, 1, 1, MPI_COMM_WORLD, &stat);
			gettimeofday(&tv, NULL);
			printf("time:%fus, receive:%s\n", difftimeval(&tv, &data->tv), data->data);
			free(data);
		}
	}
	else if(my_rank == 1){
		for(int i=0; i<TEST_TIME; i++) {
			struct Data* data = malloc(sizeof(struct Data));
			memset(data->data, 0, sizeof(data->data));
			snprintf(data->data, sizeof(data->data), "%s, num:%d", "hello world!", i);
			gettimeofday(&data->tv, NULL);
			MPI_Send(data, sizeof(struct Data), MPI_BYTE, 0, 1, MPI_COMM_WORLD);
			free(data);
		}
	}



	if (my_rank == 0) printf("That is all for now!\n");
	MPI_Finalize();  /* EXIT MPI */

}

