#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_mpi.h"
#include <unistd.h>
#include <pthread.h>

#define SHM_SIZE 4096

#ifndef MYFD_DATA
#define MYFD_DATA "shared_data"
#endif

#ifndef MYFD_STATUS
#define MYFD_STATUS "shared_status"
#endif

static int _cnt_proc;
static int _index;
static int _data_fd;
static int _status_fd;
static struct status *_status_ptr;

int get_index() { return _index; }

int mpi_init(int argc, char **argv)
{
    int i, pid;
    if (argc != 2){
        printf("The argc must be 2\n");
        return -1;
    }
    _cnt_proc = atoi(argv[1]);
    printf("_cnt_proc is %d \n", _cnt_proc);

    for (i = 1; i < _cnt_proc; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            _index = i;
            break;
        }
    }

    // * First shared area
    _data_fd = shm_open(MYFD_DATA, O_CREAT | O_RDWR, 0777);
    if (_data_fd < 0)
        return -1;
    ftruncate(_data_fd, SHM_SIZE);

    // * Second shared area
    _status_fd = shm_open(MYFD_STATUS, O_CREAT | O_RDWR, 0777);
    if (_status_fd < 0)
        return -1;
    ftruncate(_status_fd, _cnt_proc * sizeof(struct status));

    _status_ptr = mmap(NULL, _cnt_proc * sizeof(struct status), PROT_READ | PROT_WRITE, MAP_SHARED, _status_fd, 0);
    return 0;
}

int mpi_finialize()
{
    munmap(_status_ptr, _cnt_proc * sizeof(struct status));
    shm_unlink(MYFD_DATA);
    shm_unlink(MYFD_STATUS);
    return 0;
}

void *my_malloc(size_t size)
{
    // ! for debug only
    int offset = 0;

    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, _data_fd, offset); // offset needs to be chosen carefully
}

int mpi_send(void *start, size_t size, int tgt_index)
{
    while (_status_ptr[tgt_index].state != 0)
        ;
    _status_ptr[tgt_index].state = 1;
    // _status_ptr[tgt_index].offset = 0;
    while (_status_ptr[tgt_index].state != 2)
        ;
    _status_ptr[tgt_index].state = 0;
    return 0;
}

int mpi_receive(void **place, size_t size, int tgt_index)
{
    while (_status_ptr[tgt_index].state != 0)
        ;
    int offset = _status_ptr[tgt_index].offset;
    *place = mmap(NULL, size, PROT_READ, MAP_SHARED, _data_fd, offset);
    _status_ptr[tgt_index].state = 2;
    return 0;
}