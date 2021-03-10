#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_mpi.h"
#include <unistd.h>
#include <pthread.h>

#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
using boost::interprocess::interprocess_mutex;
using boost::interprocess::named_condition;
using boost::interprocess::named_mutex;
using boost::interprocess::named_semaphore;
using boost::interprocess::open_or_create;

#define SHM_SIZE 4096

#ifndef MYFD_DATA
#define MYFD_DATA "shared_data"
#endif

#ifndef MYFD_STATUS
#define MYFD_STATUS "shared_status"
#endif

static named_mutex *mtx;

static int _cnt_proc;
static int _index;
static int _data_fd;
static int _status_fd;
static int _reach_fd;
static int *reach;
static struct status *_status_ptr;

int get_index() { return _index; }

int mpi_init(int argc, char **argv)
{
    int i, pid;
    if (argc != 2)
    {
        printf("The argc must be 2\n");
        return -1;
    }
    _cnt_proc = atoi(argv[1]);
    printf("_cnt_proc is %d \n", _cnt_proc);
    _reach_fd = shm_open("reach", O_CREAT | O_RDWR, 0777);
    ftruncate(_reach_fd, sizeof(int));
    reach = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, _reach_fd, 0);

    // * Create child process
    for (i = 1; i < _cnt_proc; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            _index = i;
            break;
        }
        _index = 0;
    }

    // * First shared area
    _data_fd = shm_open(MYFD_DATA, O_CREAT | O_RDWR, 0777);
    if (_data_fd < 0)
    {
        printf("shm open #1 failed\n");
        return -1;
    }
    ftruncate(_data_fd, SHM_SIZE);

    // * Second shared area
    _status_fd = shm_open(MYFD_STATUS, O_CREAT | O_RDWR, 0777);
    if (_status_fd < 0)
    {

        printf("shm open #1 failed\n");
        return -1;
    }
    ftruncate(_status_fd, _cnt_proc * sizeof(struct status));

    _status_ptr = (status *)mmap(NULL, _cnt_proc * sizeof(struct status), PROT_READ | PROT_WRITE, MAP_SHARED, _status_fd, 0);

    // * Third shared area
    if (_index != 0)
    {
        _reach_fd = shm_open("reach", O_CREAT | O_RDWR, 0777);
        ftruncate(_reach_fd, sizeof(int));
        reach = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, _reach_fd, 0);
    }
    mtx = new named_mutex(open_or_create, "mtx");
    // mtx->unlock();
    if (_index == 1)
    {
        // mtx->lock();
        // printf("Init lock\n");
    }
    // printf("DONE1\n");

    // named_mutex temp{open_or_create, "temp"};
    // // named_condition syn{open_or_create, "syn"};
    // named_semaphore syn{open_or_create, "syn", 0};
    // named_mutex barrier{open_or_create, "barrier"};
    // barrier.lock();
    // if (*reach < _cnt_proc)
    // {
    //     *reach += 1;
    //     syn.wait();
    //     barrier.unlock();
    //     printf("\tfirst freed, %d\n", _index);
    // }
    // else
    // {
    //     barrier.unlock();
    //     printf("\tsecond freed, %d\n", _index);
    //     syn.post();
    // }
    return 0;
}

int mpi_finialize()
{
    munmap(_status_ptr, _cnt_proc * sizeof(struct status));
    shm_unlink(MYFD_DATA);
    shm_unlink(MYFD_STATUS);
    delete mtx;
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
    if (mtx->try_lock())
        mtx->unlock();
    printf("Sender: unlocked\n");
    while (!mtx->try_lock())
        ;
    printf("Sender: relocked\n");

    return 0;
}

int mpi_receive(void *place, size_t size, int tgt_index)
{
    while (mtx->try_lock())
        ;
    printf("Receiver: locked\n");
    int offset = _status_ptr[tgt_index].offset;
    place = mmap(place, size, PROT_READ, MAP_SHARED, _data_fd, offset); //mremap()
    if (mtx->try_lock())
        mtx->unlock();
    printf("Receiver: unlocked\n");
    return 0;
}