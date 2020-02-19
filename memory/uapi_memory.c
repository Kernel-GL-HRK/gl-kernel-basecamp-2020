#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define ALLOC_COEF .9F //For controling the stack memory allocation

long get_interval(struct timespec *ts1, struct timespec *ts2)
{
	return 1000000000 * (ts2->tv_sec - ts1->tv_sec) +
	       (ts2->tv_nsec - ts1->tv_nsec);
}

static inline void malloc_test(void)
{
	struct timespec ts1, ts2;
	void *restrict pdata = NULL;
	long mean_alloc = 0;
	long mean_free = 0;
	long curr_alloc = 0;
	long curr_free = 0;
	long inter_count = 0;

	printf("\n*--------------------------*malloc()*--------------------------*\n");
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

		printf("%-8ld%-25lu%-20ld%-20ld\n", inter_count, alloc_size, curr_alloc, curr_free);

		++inter_count;
	} while(alloc_size *= 2);

	mean_alloc /= inter_count;
	mean_free /= inter_count;
	printf("\n%-33s%-20ld%-20ld\n", "MEAN", mean_alloc, mean_free);
}

static inline void calloc_test(void)
{
	struct timespec ts1, ts2;
	void *restrict pdata = NULL;
	long mean_alloc = 0;
	long mean_free = 0;
	long curr_alloc = 0;
	long curr_free = 0;
	long inter_count = 0;

	printf("\n*--------------------------*calloc()*--------------------------*\n");
	printf("%-8s%-25s%-20s%-20s\n", "Power", "Buffer size (bytes)", "Allocation (ns)", "Freeing (ns)");
	size_t alloc_size = 1;
	do {
		clock_gettime(CLOCK_REALTIME, &ts1);
		pdata = calloc(alloc_size, 1);
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

		printf("%-8ld%-25lu%-20ld%-20ld\n", inter_count, alloc_size, curr_alloc, curr_free);

		++inter_count;
	} while(alloc_size *= 2);

	mean_alloc /= inter_count;
	mean_free /= inter_count;
	printf("\n%-33s%-20ld%-20ld\n", "MEAN", mean_alloc, mean_free);
}

static inline void alloca_test(void)
{
	struct timespec ts1, ts2;
	void *restrict pdata = NULL;
	long mean_alloc = 0;
	long curr_alloc = 0;
	long inter_count = 0;
	ssize_t stack_rem = 0;
	pthread_attr_t pth_attr;

	pthread_attr_init(&pth_attr);
	pthread_attr_getstacksize(&pth_attr, &stack_rem);
	stack_rem *= ALLOC_COEF;

	printf("\n*--------------------------*alloca()*--------------------------*\n");
	printf("%-8s%-25s%-20s\n", "Power", "Buffer size (bytes)", "Allocation (ns)");
	size_t alloc_size = 1;
	do {
		stack_rem -= alloc_size;
		if (stack_rem < 0)
			break;

		clock_gettime(CLOCK_REALTIME, &ts1);
		pdata = alloca(alloc_size);
		clock_gettime(CLOCK_REALTIME, &ts2);

		curr_alloc = get_interval(&ts1, &ts2);
		mean_alloc += curr_alloc;

		printf("%-8ld%-25lu%-20ld\n", inter_count, alloc_size, curr_alloc);

		++inter_count;
	} while(alloc_size *= 2);

	mean_alloc /= inter_count;
	printf("\n%-33s%-20ld\n", "MEAN", mean_alloc);
}

int main(void)
{
	malloc_test();
	calloc_test();
	alloca_test();
	return 0;
}
