#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define TEST_TARGETS 2

static struct timespec timespec_sub(struct timespec before,
				    struct timespec after)
{
	struct timespec temp;

	if ((after.tv_nsec - before.tv_nsec) < 0) {
		temp.tv_sec = after.tv_sec - before.tv_sec - 1;
		temp.tv_nsec = 1000000000 + after.tv_nsec - before.tv_nsec;
	} else {
		temp.tv_sec = after.tv_sec - before.tv_sec;
		temp.tv_nsec = after.tv_nsec - before.tv_nsec;
	}

	return temp;
}

struct test_data {
	unsigned long long buf_len;
	struct timespec alloc_duration;
	struct timespec free_duration;
};

struct test_target {
	char *name;
	void *(*alloc)(size_t, size_t);
	void (*free)(void *);
};

static bool test_memory_alloc(struct test_target *target,
			      struct test_data *data)
{
	void *buf;

	struct timespec before;
	struct timespec after;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &before);
	buf = target->alloc(data->buf_len, sizeof(*buf));
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &after);

	if (buf) {
		data->alloc_duration = timespec_sub(before, after);

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &before);
		free(buf);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &after);

		data->free_duration = timespec_sub(before, after);
	} else {
		return false;
	}

	return true;
}

static void *malloc_wrapper(size_t num, size_t size)
{
	return malloc(num * size);
}

static struct test_target test_targets[TEST_TARGETS] = {
	{ .name = "malloc", .alloc = malloc_wrapper, .free = free },
	{ .name = "calloc", .alloc = calloc, .free = free }
};

int main(void)
{
	struct test_data test_data;
	struct test_target *test_target;

	char alloc_duration_str[64];
	char free_duration_str[64];

	for (size_t i = 0; i < TEST_TARGETS; i++) {
		test_target = &test_targets[i];

		printf("TEST SUMMARY (%s)\n", test_target->name);
		printf("%-20s %-20s %-20s\n", "SIZE (BYTES)", "ALLOC (S.NS)",
		       "FREE (S.NS)");

		int power = 0;
		bool is_out_of_memory = false;
		while (!is_out_of_memory) {
			test_data.buf_len = pow(2, power);

			is_out_of_memory =
				!test_memory_alloc(test_target, &test_data);
			if (!is_out_of_memory) {
				sprintf(alloc_duration_str, "%lld.%.9ld",
					(long long)
						test_data.alloc_duration.tv_sec,
					test_data.alloc_duration.tv_nsec);

				sprintf(free_duration_str, "%lld.%.9ld",
					(long long)
						test_data.free_duration.tv_sec,
					test_data.free_duration.tv_nsec);

				printf("%-20lld %-20s %-20s\n",
				       test_data.buf_len, alloc_duration_str,
				       free_duration_str);
			} else {
				printf("%-20lld %-20s %-20s\n",
				       test_data.buf_len, "N/A", "N/A");
			}

			power++;
		}

		printf("\n");
	}

	return 0;
}
