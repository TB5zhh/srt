#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define TEST_SIZE 4096

int main()
{
    printf("Page size: %dB\n", getpagesize());
    printf("PID: %d\n", getpid());
    int fd = shm_open("test_tb5", O_CREAT | O_RDWR, 0777);
    if (fd < 0)
    {
        printf("Fail with shm_open\n");
        return -1;
    }

    ftruncate(fd, TEST_SIZE);
    void *anon = mmap(NULL, TEST_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

    printf("pointer: %p\n", anon);
    getchar();
    munmap(anon, getpagesize());
    shm_unlink("test_tb5");

    return 0;
}