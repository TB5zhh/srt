#include <stdio.h>
#include <stdlib.h>
#include "difftimeval.h"
#include "config.h"
#include <sys/time.h>
#include "string.h"

struct Data
{
    struct timeval tv;
    char data[DATA_SIZE];
};

int main()
{
    struct timeval a;
    struct Data *data = malloc(sizeof(struct Data));
    memset(data, 1, sizeof(struct Data));
    struct Data *data1 = malloc(sizeof(struct Data));
    memset(data1, 1, sizeof(struct Data));
    int i;
    double sum = 0.;
    for (i = 0; i < TEST_TIME; i++)
    {
        memset(data->data, 0, sizeof(data->data));
        snprintf(data->data, sizeof(data->data), "%s, num:%d", "hello world!", i);
        gettimeofday(&data->tv, NULL);
        snprintf(data->data, sizeof(data->data), "%s, num:%d", "hello world!", i);
        memcpy(data1, data, sizeof(struct Data));
        gettimeofday(&a, NULL);
        printf("time:%fus, receive:%s\n", difftimeval(&a, &data1->tv), data1->data);
        sum += difftimeval(&a, &data1->tv);
    }
    printf("Avg time: %lf\n", sum / TEST_TIME);

    free(data);
    free(data1);
}