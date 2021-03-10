#include <stdio.h>
#include <stdlib.h>
#include "my_mpi.h"
#include <unistd.h>
#include <string.h>

#define SIZE 4096
#define PAGE_SIZE (4096 * 4)
// * Command argument:
// * 1. count of processes
// * 2. index of current process

// * Init:
// * shm_open two shared memory area:
// * 1. shared area for data transfer
// * 2. an shared array of struct status to indicate send request
// * the two areas are identified by file descriptors

// * Malloc:
// * 1. find the suitable page offset in the 1st share area
// ?? how to find the suitable page offset?
// + for debugging, just set fd to 0
// * 2. mmap at that offset with given size

// * Send:
// * 1. find target status in 2nd shared area
// * 2. wait until available to write(0)
// ?? how to wait between processes?
// * 3. write status(0->1), source index and page offset in 1st area
// ?? and how to find the offset from address?
// * 4. wait until transfer is done (2)
// * 5. write status(2->0), then return

// * Receive:
// * 1. wait until available to read(1)
// * 2. mmap input pointer to the page offset of 1st area given by source index
// * 3. write status(1->2), then return

// * Finalize:
// * 1. shm_unlink the shared areas

int main(int argc, char **argv)
{
    mpi_init(argc, argv);
    int proc_id = get_index();
    printf("My index is %d: %d\n", proc_id, getpid());

    if (proc_id == 0)
    {
        // char *recv = (char *)0x100000000000;
        char *recv = (char *)malloc(1000 * sizeof(char));
        printf("Allocated: %x\n", recv);
        getchar();
        mpi_receive(recv, SIZE, 1);
        printf("%s\n", recv);
        getchar();
    }
    else if (proc_id == 1)
    {
        char *word = (char *)my_malloc(SIZE);
        memcpy(word, "hello! test\n", 13);
        mpi_send(word, SIZE, 0);
    }

    mpi_finialize();
    return 0;
}