#include <stdio.h>
#include <time.h>

int main( ) {
   time_t timeh_time;

   timeh_time = time(NULL);

   printf( "Time from time.h - time() :%lds from 1970y\n", timeh_time);

   return 0;
}
