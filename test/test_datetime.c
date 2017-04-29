#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>

#include <qtl.h>

char dst[100];

void print_datetime2(datetime_t dt)
{
	datetime_format(dst, sizeof(dst), dt);
	printf("%s\n", dst);
}

int main()
{
	char timestr[] = "2013-12-31T00:00:00Z";
 	datetime_t dt;	
       	datetime_from_iso8601(timestr, strlen(timestr), &dt);
	print_datetime2(dt);
	datetime_t dt2;
	datetime_from_iso8601(dst, strlen(dst), &dt2);
	assert(dt == dt2);	
}
