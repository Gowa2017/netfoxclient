//
//  AudioRecorder.h
//  GloryProject
//
//  Created by zhong on 16/11/24.
//
//

#ifndef AudioRecorder_h
#define AudioRecorder_h

#include <stdio.h>
#include "cocos2d.h"

int register_all_recorder();

class AudioRecorder
{
private:
    AudioRecorder();
    ~AudioRecorder();
    
public:
    static AudioRecorder* getInstance();
    
    static void destroy();
    
    /*
     * @brief 初始化
     * @param[recordPath]   录音文件存储路径
     * @param[downloadPath] 录音文件下载路径
     */
    void init(const std::string &recordPath, const std::string &downloadPath);
    
    /*
     * @brief 开始录音
     */
    void startRecord(const std::string &filename = "record.mp3");
    
    /*
     * @brief 结束录音
     */
    void endRecord();

    /*
     * @brief 取消录音
     */
     void cancelRecord();
    
    std::string m_strRecordPath;
    std::string m_strDownloadPath;
    std::string m_strRecordFile;
private:
    bool m_bInit;    
};
#endif /* AudioRecorder_h */
