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
	long mean_alloc = 0;
	long mean_free = 0;
	long curr_alloc = 0;
	long curr_free = 0;
	long inter_count = 0;

	printf("*--------------------------*malloc()*--------------------------*\n");
	printf("%-8s%-25s%-20s%-20s\n", "Power", "Buffer size (bytes)", "Allocation (ns)", "Freeing (ns)");
	size_t alloc_size = 1;
	do {
		clock_gettime(CLOCK_REALTIME, &ts1);
		pdata = malloc(alloc_size);
		clock_gettime(CLOCK_REALTIME, &ts2);

		if (!pdata)
			break;

		curr_alloc = get_interval(&ts1, &ts2);
		mean_alloc += curr_alloc;

		clock_gettime(CLOCK_REALTIME, &ts1);
		free(pdata);
		clock_gettime(CLOCK_REALTIME, &ts2);

		curr_free = get_interval(&ts1, &ts2);
		mean_free += curr_free;
		++inter_count;
		printf("%-8ld%-25lu%-20ld%-20ld\n", inter_count, alloc_size, curr_alloc, curr_free);
	} while(alloc_size *= 2);

	mean_alloc /= inter_count;
	mean_free /= inter_count;
	printf("\n%-33s%-20ld%-20ld\n", "MEAN", mean_alloc, mean_free);
	return 0;
}
