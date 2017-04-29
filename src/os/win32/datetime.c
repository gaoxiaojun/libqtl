#ifdef _WIN32
void gettimeofday(struct timeval *tp, void *tz /* ignore */) {
  uint64_t intervals;
  FILETIME ft;

  GetSystemTimeAsFileTime(&ft);

  /*
   * A file time is a 64-bit value that represents the number
   * of 100-nanosecond intervals that have elapsed since
   * January 1, 1601 12:00 A.M. UTC.
   *
   * Between January 1, 1970 (Epoch) and January 1, 1601 there were
   * 134774 days,
   * 11644473600 seconds or
   * 11644473600,000,000,0 100-nanosecond intervals.
   *
   * See also MSKB Q167296.
   */

  intervals = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
  intervals -= 116444736000000000;

  tp->tv_sec = (long)(intervals / 10000000);
  tp->tv_usec = (long)((intervals % 10000000) / 10);
}

void libc_localtime(time_t s, struct tm *tm)
{
    struct tm *t;

    t = localtime(&s);
    *tm = *t;
}

void libc_gmtime(time_t s, struct tm *tm)
{
    struct tm *t;
    t = gmtime(&s);
    *tm = *t;
}

int32_t gettimezone(void)
{
    uin32_t n;
    TIME_ZONE_INFORMATION tz;

    n = GetTimeZoneInfomation(&tz);

    switch(n) {
    case TIME_ZONE_ID_UNKNOWN:
         return -tz.Bias;

     case TIME_ZONE_ID_STANDARD:
         return -(tz.Bias + tz.StandardBias);

     case TIME_ZONE_ID_DAYLIGHT:
         return -(tz.Bias + tz.DaylightBias);

     default: /* TIME_ZONE_ID_INVALID */
         return 0;
    }
}
#endif
