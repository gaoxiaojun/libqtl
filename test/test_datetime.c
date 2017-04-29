#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <qtl.h>

char dst[100];

void print_datetime2(datetime_t dt)
{
	datetime_format(dst, sizeof(dst), dt, 0);
	printf("%s\n", dst);
}

void print_timezone()
{
        struct  timeval    tv;
        struct  timezone   tz;
        gettimeofday(&tv,&tz);

        printf("tv_sec:%ld\n",tv.tv_sec);
        printf("tv_usec:%d\n",tv.tv_usec);
        printf("tz_minuteswest:%d\n",tz.tz_minuteswest);
        printf("tz_dsttime:%d\n",tz.tz_dsttime);
}

int main()
{
	char timestr[] = "2013-12-31T00:00:00Z";
 	datetime_t dt;	
       	datetime_from_iso8601(timestr, strlen(timestr), &dt);
	print_datetime2(dt);
	datetime_t dt2;
	datetime_from_iso8601(dst, strlen(dst), &dt2);
	print_datetime2(dt2);
	assert(dt == dt2);	
	datetime_t now = datetime_now();
	print_datetime2(now);
	print_timezone();
}
