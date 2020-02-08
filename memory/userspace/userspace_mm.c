#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <alloca.h>
#include <math.h>

#define NSEC_PER_SEC 1000000000L

static void test_malloc(void);
static void test_calloc_1(void);
static void test_calloc_2(void);
static void test_alloca(void);

static void test_malloc(void)
{
	uint8_t i = 0;
	uint64_t p = 1;
	struct timespec start, end;
	void *m;

	printf("Starting malloc() test...\n");
	while (1) {
		if (clock_gettime(CLOCK_REALTIME, &start) < 0)
			exit(1);
		m = malloc(p);
		if (!m)
			break;
		if (clock_gettime(CLOCK_REALTIME, &end) < 0)
			exit(1);
		printf("malloc: 2^%u: %ld ns\n", i,
		(end.tv_sec - start.tv_sec) * NSEC_PER_SEC
		+ (end.tv_nsec - start.tv_nsec));

		if (clock_gettime(CLOCK_REALTIME, &start) < 0)
			exit(1);
		free(m);
		if (clock_gettime(CLOCK_REALTIME, &end) < 0)
			exit(1);
		printf("free: 2^%u: %ld ns\n", i,
		(end.tv_sec - start.tv_sec) * NSEC_PER_SEC
		+ (end.tv_nsec - start.tv_nsec));
		p = pow(2, ++i);
	}
	printf("Stopped at %lld bytes,"
	"%u power\nmalloc() test finished.\n", p, i);
}

static void test_calloc_1(void)
{
	uint8_t i = 0;
	uint64_t p = 1;
	struct timespec start, end;
	void *m;

	printf("Starting calloc() test #1...\n");
	while (1) {
		if (clock_gettime(CLOCK_REALTIME, &start) < 0)
			exit(1);
		m = calloc(p, 1);
		if (!m)
			break;
		if (clock_gettime(CLOCK_REALTIME, &end) < 0)
			exit(1);
		printf("calloc: 2^%u: %ld ns\n", i,
		(end.tv_sec - start.tv_sec) * NSEC_PER_SEC
		+ (end.tv_nsec - start.tv_nsec));

		if (clock_gettime(CLOCK_REALTIME, &start) < 0)
			exit(1);
		free(m);
		if (clock_gettime(CLOCK_REALTIME, &end) < 0)
			exit(1);
		printf("free: 2^%u: %ld ns\n", i,
		(end.tv_sec - start.tv_sec) * NSEC_PER_SEC
		+ (end.tv_nsec - start.tv_nsec));
		p = pow(2, ++i);
	}
	printf("Stopped at %lld bytes,"
	"%u power\ncalloc() test #1 finished.\n", p, i);
}

static void test_calloc_2(void)
{
	uint8_t i = 0;
	uint64_t p = 1;
	struct timespec start, end;
	void *m;

	printf("Starting calloc() test #2...\n");
	while (1) {
		if (clock_gettime(CLOCK_REALTIME, &start) < 0)
			exit(1);
		m = calloc(1, p);
		if (!m)
			break;
		if (clock_gettime(CLOCK_REALTIME, &end) < 0)
			exit(1);
		printf("calloc: 2^%u: %ld ns\n", i,
		(end.tv_sec - start.tv_sec) * NSEC_PER_SEC
		+ (end.tv_nsec - start.tv_nsec));

		if (clock_gettime(CLOCK_REALTIME, &start) < 0)
			exit(1);
		free(m);
		if (clock_gettime(CLOCK_REALTIME, &end) < 0)
			exit(1);
		printf("free: 2^%u: %ld ns\n", i,
		(end.tv_sec - start.tv_sec) * NSEC_PER_SEC
		+ (end.tv_nsec - start.tv_nsec));
		p = pow(2, ++i);
	}
	printf("Stopped at %lld bytes,"
	"%u power\ncalloc() test #2 finished.\n", p, i);
}

static void test_alloca(void)
{
	uint8_t i = 0;
	uint64_t p = 1;
	struct timespec start, end;
	void *m;

	printf("Starting alloca() test...\n");
	while (1) {
		if (clock_gettime(CLOCK_REALTIME, &start) < 0)
			exit(1);
		m = alloca(p);
		if (!m)
			break;
		if (clock_gettime(CLOCK_REALTIME, &end) < 0)
			exit(1);
		printf("alloca: 2^%u: %ld ns\n", i,
		(end.tv_sec - start.tv_sec) * NSEC_PER_SEC
		+ (end.tv_nsec - start.tv_nsec));

		p = pow(2, ++i);
	}
	printf("Stopped at %lld bytes,"
	"%u power\nalloca() test finished.\n", p, i);
}

int main(int argc, char **argv)
{
	test_malloc();
	putchar('\n');
	test_calloc_1();
	putchar('\n');
	test_calloc_2();
	putchar('\n');
	test_alloca();
	return 0;
}
