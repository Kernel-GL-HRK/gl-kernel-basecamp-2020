#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <alloca.h>
#include <time.h>

unsigned long long rdtsc()
{
	unsigned int lo, hi;
	asm volatile("rdtsc\n" : "=a"(lo), "=d"(hi));
	return ((unsigned long long)hi << 32) | lo;
}


int main()
{
	return 0;
}