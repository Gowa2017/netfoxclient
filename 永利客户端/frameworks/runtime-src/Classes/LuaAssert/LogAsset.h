//
//  LogAsset.h
//  GloryProject
//
//  Created by zhong on 16/8/19.
//
//

#ifndef ry_LogAsset_h
#define ry_LogAsset_h

#include <stdio.h>
#include "cocos2d.h"

int register_all_logasset();

struct tagLogData
{
    std::string strData = "";
    bool bExtraData = false;
};

class LogAsset
{
private:
    LogAsset();
    ~LogAsset();
    
public:
    static LogAsset* getInstance();
    
    static void destroy();
    
    void init(const std::string &logPath, const bool &bEnable, const bool &bPrettyWrite = false);
    
    void logData(const std::string &str, const bool &bExtraData = false);
    
private:
    void addToQueue(tagLogData* pData);
    
    void logRun();
    
    void backupFile(const std::string &filepath);
private:
    std::string m_strFilePath;
    bool m_bEnable;
    bool m_bPrettyWrite;
    std::mutex m_logMutex;
    std::vector<tagLogData*> *m_vecQueue;
};
#endif /* ry_LogAsset_h */
