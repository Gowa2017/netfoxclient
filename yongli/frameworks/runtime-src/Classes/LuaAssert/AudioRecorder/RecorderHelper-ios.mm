//
//  ClientUpdater-ios.mm
//  HLDouDiZhu
//
//  Created by zhong on 3/24/16.
//
//

#ifndef RecorderHelper_ios_h
#define RecorderHelper_ios_h

#include "RecorderHelper.h"
USING_NS_CC;
#import "AudioHelper.h"
static AudioHelper* g_audioHelper = nullptr;
void initRecordHelper(const std::string &path)
{
    g_audioHelper = new AudioHelper(path);
}

void destroyRecordHelper()
{
    CC_SAFE_DELETE(g_audioHelper);
}

void startRecordHelper(const std::string &filename)
{
    g_audioHelper->benginRecord(filename);
}

void endRecordHelper()
{
    g_audioHelper->endRecord();
}
#endif /* ClientUpdater_ios_h */