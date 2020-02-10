#include <time.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	return !(printf("Absolute time: %u s\n", time(0)));
}
