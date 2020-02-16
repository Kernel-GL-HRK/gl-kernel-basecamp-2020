#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <time.h>

int main( ) {
   int rtc_dev;
   time_t timeh_time;
   struct rtc_time tm;

   rtc_dev = open( "/dev/rtc", O_RDONLY );
   if( rtc_dev < 0 )
           printf( "Bad open : %m\n" );

   ioctl( rtc_dev, RTC_RD_TIME, &tm );
   timeh_time = time(NULL);

   printf( "Time from time.h - time() :%lds from 1970y\n", timeh_time);
   printf( "Time from RTC: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec );

   close( rtc_dev );
   return 0;
}

