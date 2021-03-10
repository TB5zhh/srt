#ifndef _MY_MPI
#define _MY_MPI
#include <stdlib.h>

struct status
{
    char state;
    int offset;
};

int get_index();
int mpi_init(int argc, char **argv);
void *my_malloc(size_t size);
int mpi_send(void *start, size_t size, int tgt_index);
int mpi_receive(void *place, size_t size, int fd);
int mpi_finialize();

#endif