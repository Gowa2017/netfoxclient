//
//  RecorderHelper.h
//  HLDouDiZhu
//
//  Created by zhong on 3/24/16.
//
//

#ifndef RecorderHelper_hpp
#define RecorderHelper_hpp

#include <stdio.h>
#include "cocos2d.h"

void initRecordHelper(const std::string &path);

void destroyRecordHelper();

/*
 * @brief 开始记录
 */
void startRecordHelper(const std::string &filename);

/*
 * @brief 结束记录
 */
void endRecordHelper();
#endif /* RecorderHelper_hpp */
