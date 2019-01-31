#ifdef WIN32

#include "stdc-win32.h"

int gettimeofday(struct timeval * val, struct timezone *)
{
    if (val)
    {
        LARGE_INTEGER liTime, liFreq;
        QueryPerformanceFrequency( &liFreq );
        QueryPerformanceCounter( &liTime );
        val->tv_sec     = (long)( liTime.QuadPart / liFreq.QuadPart );
        val->tv_usec    = (long)( liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0 );
    }
    return 0;
}

#endif // TARGET_PLATFORM == PLATFORM_WIN32
