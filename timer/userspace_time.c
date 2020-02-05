#include <time.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	return !(printf("Absolute time: %u\n", time(0)));
}
