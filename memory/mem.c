#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <alloca.h>
#include <time.h>


unsigned long long ticks;

unsigned long long rdtsc()
{
	unsigned int lo, hi;
	asm volatile("rdtsc\n" : "=a"(lo), "=d"(hi));
	return ((unsigned long long)hi << 32) | lo;
}

int tryMalloc()
{
	static int npow;
	void *pointer;
	size_t size = pow(2.0, (double)npow++);

	ticks = rdtsc();
	pointer = (void *)malloc(size);
	ticks = rdtsc() - ticks;

	printf("Malloc %ld byte lasts %d ticks\n", size, ticks);

	if (pointer == NULL) {
		printf("Bad malloc at %ld\n", size);
		return -1;
	}

	free(pointer);

	return 0;
}

int tryCalloc()
{
	static int npow;
	void *pointer;
	size_t size = pow(2.0, (double)npow++);

	ticks = rdtsc();
	pointer = (void *)calloc(1, size);
	ticks = rdtsc() - ticks;

	printf("Calloc %ld bytes lasts %d ticks\n", size, ticks);

	if (pointer == NULL) {
		printf("Bad calloc at %ld\n", size);
		return -1;
	}

	free(pointer);

	return 0;
}

int main()
{
    while (!tryMalloc());
    while (!tryCalloc());
	return 0;
}