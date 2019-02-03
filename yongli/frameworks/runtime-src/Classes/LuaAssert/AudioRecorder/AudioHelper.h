#pragma once
#include <string>

#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
class AudioHelper
{
public:
    AudioHelper(const std::string &recordPath);
    NSURL *urlPlay;
    NSTimer *timer;
    AVAudioRecorder *recorder;
    AVAudioPlayer *avPlay;
    
    double m_fVoice;
    void initAudio();
    void getVoice();
    void benginRecord(const std::string &filename);
    void endRecord();
    void audio_PCMtoMP3(std::string soundData = "");
private:
    std::string m_strRecordPath;
    std::string m_strFileName;
};