#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "cocos2d.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "ios/stdc-ios.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "android/stdc-android.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#include "win/stdc-win32.h"
#endif

#endif /* __PLATFORM_H__*/
