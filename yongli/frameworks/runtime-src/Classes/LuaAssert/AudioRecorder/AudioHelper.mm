#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "lame/include/lame/lame.h"
#import "AudioHelper.h"
#import <CoreTelephony/CTCarrier.h>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <AudioToolbox/AudioToolbox.h>
#import "util.h"
#define SAMPLE_RATE 110250.0

#include "cocos2d.h"
using namespace std;
extern "C"
{
    AudioHelper::AudioHelper(const std::string &recordPath)
    :m_strRecordPath(recordPath)
    {
        initAudio();
    }
    
    void AudioHelper::initAudio()
    {
      
        AVAudioSession *session = [AVAudioSession sharedInstance];
        NSError *setCategoryError = nil;
        [session setCategory:AVAudioSessionCategoryAmbient error:&setCategoryError];
                if(setCategoryError)
        {
            NSLog(@"%@", [setCategoryError description]);
        }
        [session setActive:YES error:&setCategoryError];
        
        NSError *audioError = nil;
        BOOL success = [session overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker error:&audioError];
        if(!success){
            NSLog(@"error doing outputaudioportoverride - %@", [audioError localizedDescription]);
        }
        
        //录音设置
        NSMutableDictionary *recordSetting = [[[NSMutableDictionary alloc]init] autorelease];
        //设置录音格式  AVFormatIDKey==kAudioFormatLinearPCM
        [recordSetting setValue:[NSNumber numberWithInt:kAudioFormatLinearPCM] forKey:AVFormatIDKey];
        //设置录音采样率(Hz) 如：AVSampleRateKey==8000/44100/96000（影响音频的质量）
        [recordSetting setValue:[NSNumber numberWithFloat:8000] forKey:AVSampleRateKey];
        //录音通道数  1 或 2
        [recordSetting setValue:[NSNumber numberWithInt:2] forKey:AVNumberOfChannelsKey];
        //线性采样位数  8、16、24、32
        [recordSetting setValue:[NSNumber numberWithInt:16] forKey:AVLinearPCMBitDepthKey];
        //录音的质量
        [recordSetting setValue:[NSNumber numberWithInt:AVAudioQualityMax] forKey:AVEncoderAudioQualityKey];

        std::string cafPath = m_strRecordPath + "res.caf";
        NSString *nsCafPath = [NSString stringWithUTF8String:cafPath.c_str()];
        NSURL *url = [NSURL fileURLWithPath:nsCafPath];
        
        urlPlay = url;
        NSError *error;
        //初始化
        recorder = [[AVAudioRecorder alloc]initWithURL:url settings:recordSetting error:&error];
        //开启音量检测
        recorder.meteringEnabled = YES;
    }
    
    void AudioHelper::getVoice()
    {
        [recorder updateMeters];//刷新音量数据
        
        //获取音量的平均值  [recorder averagePowerForChannel:0];
        //音量的最大值  [recorder peakPowerForChannel:0];
        double lowPassResults = pow(10, (0.05 * [recorder peakPowerForChannel:0]));
        NSLog(@"%lf",lowPassResults);
        //最大50  0
        m_fVoice = lowPassResults;
    }
    
    void AudioHelper::benginRecord(const std::string &filename)
    {
        m_strFileName = filename;
        initAudio();
        //创建录音文件，准备录音
        if ([recorder prepareToRecord])
        {
            //开始
            [recorder record];
        }
    }
    void AudioHelper::endRecord()
    {
        //[recorder deleteRecording];
        [recorder stop];
        audio_PCMtoMP3();
    }
    
    void AudioHelper::audio_PCMtoMP3(std::string soundData)
    {
        std::string cafPath = m_strRecordPath + "res.caf";
        std::string mp3Path = m_strRecordPath + m_strFileName;
        NSString *nsMp3Path = [NSString stringWithUTF8String:cafPath.c_str()];
        NSString *mp3FilePath = [NSHomeDirectory() stringByAppendingPathComponent:nsMp3Path];
        
        NSFileManager* fileManager = [NSFileManager defaultManager];
        if([fileManager removeItemAtPath:mp3FilePath error:nil])
        {
            NSLog(@"删除");
        }
        
        std::string strMp3 = "";
        @try {
            int read, write;
            
            FILE *pcm = fopen(cafPath.c_str(), "rb");  //source 被转换的音频文件位置
            if(!pcm)
                return;
            fseek(pcm, 4*1024, SEEK_CUR);              //skip file header
            FILE *mp3 = fopen(mp3Path.c_str(), "wb");  //output 输出生成的Mp3文件位置
            
            const int PCM_SIZE = 8192;
            short int pcm_buffer[PCM_SIZE*2];
            const int MP3_SIZE = 8192;
            unsigned char mp3_buffer[MP3_SIZE];
            
            lame_t lame = lame_init();
            lame_set_num_channels(lame,1);//设置1为单通道，默认为2双通道
            lame_set_in_samplerate(lame, 8000.0);//11025.0
            lame_set_brate(lame,8);
            lame_set_mode(lame,MONO);
            lame_set_quality(lame,9); /* 2=high 5 = medium 7=low 音质*/
            lame_set_scale(lame,6.0);
            lame_init_params(lame);
            do
            {
                read = (int)fread(pcm_buffer, 2*sizeof(short int), PCM_SIZE, pcm);
                if (read == 0)
                {
                    write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
                }
                else
                {
                    write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
                }
                
                fwrite(mp3_buffer, write, 1, mp3);
            } while (read != 0);
            lame_close(lame);
            fclose(mp3);
            fclose(pcm);
        }
        @catch (NSException *exception)
        {
            NSLog(@"%@",[exception description]);
        }
        @finally
        {
        }
        AVAudioSession *session = [AVAudioSession sharedInstance];
        [session setCategory:AVAudioSessionCategoryPlayback error:nil];
    }
}
#endif
