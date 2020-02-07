#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long get_interval(struct timespec *ts1, struct timespec *ts2)
{
	return 1000000000 * (ts2->tv_sec - ts1->tv_sec) +
	       (ts2->tv_nsec - ts1->tv_nsec);
}

int main(void)
{
	struct timespec ts1, ts2;
	void *pdata = NULL;
	long inter = 0;

	clock_gettime(CLOCK_REALTIME, &ts1);
	pdata = malloc(1);
	clock_gettime(CLOCK_REALTIME, &ts2);
	inter = get_interval(&ts1, &ts2);
	printf("malloc(1) inter = %ld ns\n", inter);

	clock_gettime(CLOCK_REALTIME, &ts1);
	free(pdata);
	clock_gettime(CLOCK_REALTIME, &ts2);
	inter = get_interval(&ts1, &ts2);
	printf("free() inter = %ld ns\n", inter);

	return 0;
}
