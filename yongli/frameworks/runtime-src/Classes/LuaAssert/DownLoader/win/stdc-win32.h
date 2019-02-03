#ifndef __STD_C_H__
#define __STD_C_H__

#include "../stdc.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#include <BaseTsd.h>
#ifndef __SSIZE_T
#define __SSIZE_T
typedef SSIZE_T ssize_t;
#endif // __SSIZE_T

typedef size_t sa_family_t;

#include <float.h>

#if _MSC_VER < 1900
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
  #define M_PI      3.14159265358
#endif
#ifndef M_PI_2
  #define M_PI_2    1.57079632679
#endif

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif


#if _MSC_VER >= 1600 || defined(__MINGW32__)
    #include <stdint.h>
#else
    #include "./compat/stdint.h"
#endif

#define _WINSOCKAPI_
#ifndef NOMINMAX
  #define NOMINMAX
#endif
// Structure timeval has define in winsock.h, include windows.h for it.
#include <Windows.h>

#include <WinSock2.h>

//struct timezone
//{
//    int tz_minuteswest;
//    int tz_dsttime;
//};

int gettimeofday(struct timeval *, struct timezone *);


#endif // TARGET_PLATFORM == PLATFORM_WIN32

#endif  // __STD_C_H__



