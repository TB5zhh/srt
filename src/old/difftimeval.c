#include "difftimeval.h"
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

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