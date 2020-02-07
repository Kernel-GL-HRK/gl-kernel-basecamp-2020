#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define NSEC_PER_SEC 1000000000L

void test_malloc();
void test_free();
void test_calloc();
void test_alloca();

void test_malloc()
{
	uint8_t i = 0;
	uint64_t p = 1;
	struct timespec start, end;
	void *m;
	printf("Starting malloc() test...\n");
	while(1) {
		if (clock_gettime(CLOCK_REALTIME, &start) < 0)
			exit(1);
		if (!(m = malloc(p)))
			break;
		if (clock_gettime(CLOCK_REALTIME, &end) < 0)
			exit(1);
		printf("malloc: 2^%u: %ld ns\n", i, (end.tv_sec - start.tv_sec) * NSEC_PER_SEC + (end.tv_nsec - start.tv_nsec));

		if (clock_gettime(CLOCK_REALTIME, &start) < 0)
			exit(1);
		free(m);
		if (clock_gettime(CLOCK_REALTIME, &end) < 0)
			exit(1);
		printf("free: 2^%u: %ld ns\n", i, (end.tv_sec - start.tv_sec) * NSEC_PER_SEC + (end.tv_nsec - start.tv_nsec));
		p = pow(2, ++i);
	}
	printf("Stopped at %lld bytes, %u power\nmalloc() test finished.\n", p, i);
}

int main(int argc, char **argv)
{
	test_malloc();
	return 0;
}
