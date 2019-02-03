#ifndef __STD_C_H__
#define __STD_C_H__

#include "../stdc.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <cassert>

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#endif // TARGET_PLATFORM == PLATFORM_ANDROID

#endif  // __STD_C_H__
