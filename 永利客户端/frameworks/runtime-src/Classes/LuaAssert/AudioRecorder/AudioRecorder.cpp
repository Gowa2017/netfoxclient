//
//  AudioRecorder.cpp
//  GloryProject
//
//  Created by zhong on 16/11/24.
//
//

#include "AudioRecorder.h"
#include "audio/include/AudioEngine.h"
#include "../CMD_Data.h"
static const int MAXT_VOICE_LENGTH = 15000;
static const int AUDIO_BUFFER_SIZE = 30;
static int m_nAudioCount = 0;
//用户语音
struct CMD_GF_C_UserVoice
{
    DWORD							dwTargetUserID;						//目标用户
    DWORD							dwVoiceLength;						//语音长度
    BYTE							byVoiceData[MAXT_VOICE_LENGTH];     //语音数据
};

//用户语音
struct CMD_GF_S_UserVoice
{
    DWORD							dwSendUserID;						//发送用户
    DWORD							dwTargetUserID;						//目标用户
    DWORD							dwVoiceLength;						//语音长度
    BYTE							byVoiceData[MAXT_VOICE_LENGTH];     //语音数据
};

#include "RecorderHelper.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
static const char* JAVA_CLASS = "org/cocos2dx/lua/AppActivity";
#endif

USING_NS_CC;
using namespace experimental;

#if CC_ENABLE_SCRIPT_BINDING
#include "CCLuaEngine.h"
#include "tolua_fix.h"
#endif

static AudioRecorder* _recorderInstance = nullptr;
AudioRecorder::AudioRecorder():
m_bInit(false)
{
    m_strRecordPath = FileUtils::getInstance()->getWritablePath();
    m_strDownloadPath = FileUtils::getInstance()->getWritablePath();
}

AudioRecorder::~AudioRecorder()
{
    
}

AudioRecorder* AudioRecorder::getInstance()
{
    if (nullptr == _recorderInstance)
    {
        _recorderInstance = new AudioRecorder();
        
    }
    return _recorderInstance;
}

void AudioRecorder::destroy()
{
    CC_SAFE_DELETE(_recorderInstance);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    destroyRecordHelper();
#endif
}

void AudioRecorder::init(const std::string &recordPath, const std::string &downloadPath)
{
    m_bInit = true;
    if (recordPath.length() > 0)
    {
        m_strRecordPath = recordPath;
        if (recordPath[recordPath.length()-1] != '/')
        {
            m_strRecordPath = recordPath + "/";
        }
    }
    if (!FileUtils::getInstance()->isDirectoryExist(m_strRecordPath))
    {
        FileUtils::getInstance()->createDirectory(m_strRecordPath);
    }
    if (downloadPath.length() > 0)
    {
        m_strDownloadPath = downloadPath;
        if (downloadPath[downloadPath.length()-1] != '/')
        {
            m_strDownloadPath = downloadPath + "/";
        }
    }
    if (!FileUtils::getInstance()->isDirectoryExist(m_strDownloadPath))
    {
        FileUtils::getInstance()->createDirectory(m_strDownloadPath);
    }
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    initRecordHelper(m_strRecordPath);
#endif
}

void AudioRecorder::startRecord(const std::string &filename /*= "record.mp3"*/)
{
    if (false == m_bInit)
    {
        log(" do not init AudioRecorder ");
        return;
    }
    m_strRecordFile = filename;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    startRecordHelper(filename);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo minfo;
    std::string fullPath = m_strRecordPath + filename;
    if (JniHelper::getStaticMethodInfo(minfo, JAVA_CLASS, "startRecord", "(Ljava/lang/String;)V"))
    {
        jstring jFullPath = minfo.env->NewStringUTF(fullPath.c_str());
        minfo.env->CallStaticVoidMethod(minfo.classID,minfo.methodID,jFullPath);
        minfo.env->DeleteLocalRef(jFullPath);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
#endif
}

void AudioRecorder::endRecord()
{
    if (false == m_bInit)
    {
        log(" do not init AudioRecorder ");
        return;
    }
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    endRecordHelper();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo minfo;
    if (JniHelper::getStaticMethodInfo(minfo, JAVA_CLASS, "stopRecord", "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID,minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
#endif
}

void AudioRecorder::cancelRecord()
{
    if (false == m_bInit)
    {
        log(" do not init AudioRecorder ");
        return;
    }
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    endRecordHelper();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo minfo;
    if (JniHelper::getStaticMethodInfo(minfo, JAVA_CLASS, "cancelRecord", "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID,minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
#endif
}

#if CC_ENABLE_SCRIPT_BINDING
static int toLua_AudioRecorder_getInstance(lua_State *tolua_S)
{
    int argc = lua_gettop(tolua_S);
    if (1 == argc)
    {
        AudioRecorder *ret = AudioRecorder::getInstance();
        object_to_luaval<AudioRecorder>(tolua_S, "cc.AudioRecorder", (AudioRecorder*)ret);
        return 1;
    }
    return 0;
}

static int toLua_AudioRecorder_init(lua_State *tolua_S)
{
    AudioRecorder *ret = (AudioRecorder*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != ret)
    {
        int argc = lua_gettop(tolua_S);
        if (argc == 3)
        {
            std::string rPath = lua_tostring(tolua_S, 2);
            std::string dPath = lua_tostring(tolua_S, 3);
            ret->init(rPath, dPath);
            return 1;
        }
    }
    log(" init AudioRecorder error ");
    return 0;
}

static int toLua_AudioRecorder_startRecord(lua_State *tolua_S)
{
    AudioRecorder *ret = (AudioRecorder*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != ret)
    {
        int argc = lua_gettop(tolua_S);
        if (argc == 1)
        {
            ret->startRecord();
            return 1;
        }
        else if (argc == 2)
        {
            std::string filename = lua_tostring(tolua_S, 2);
            ret->startRecord(filename);
            return 1;
        }
    }
    return 0;
}

static int toLua_AudioRecorder_endRecord(lua_State *tolua_S)
{
    AudioRecorder *ret = (AudioRecorder*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != ret)
    {
        ret->endRecord();
        return 1;
    }
    return 0;
}

static int toLua_AudioRecorder_cancelRecord(lua_State *tolua_S)
{
    AudioRecorder *ret = (AudioRecorder*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != ret)
    {
        ret->cancelRecord();
        return 1;
    }
    return 0;
}

static int toLua_AudioRecorder_createSendBuffer(lua_State *tolua_S)
{
    AudioRecorder *ret = (AudioRecorder*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != ret)
    {
        std::string fileFullPath = ret->m_strRecordPath + ret->m_strRecordFile;
        if (FileUtils::getInstance()->isFileExist(fileFullPath))
        {
            unsigned char * pData = NULL;
            ssize_t size = 0;
            pData = FileUtils::getInstance()->getFileData(fileFullPath,"rb",&size);
            if (0 != size)
            {
                size = size > MAXT_VOICE_LENGTH ? MAXT_VOICE_LENGTH : size;
                CMD_GF_C_UserVoice cmd;
                cmd.dwTargetUserID = INVALID_USERID;
                cmd.dwVoiceLength = (DWORD)size;
                memcpy(cmd.byVoiceData,pData,size);
                free(pData);
                int nSize = sizeof(cmd) - sizeof(cmd.byVoiceData) + cmd.dwVoiceLength;
                
                CCmd_Data *buffer = CCmd_Data::create(nSize);
                buffer->PushByteData((BYTE*)&cmd.dwTargetUserID, 4);
                buffer->PushByteData((BYTE*)&size, 4);
                buffer->PushByteData((BYTE*)&cmd.byVoiceData, size);
                object_to_luaval<CCmd_Data>(tolua_S, "cc.CCmd_Data", (CCmd_Data*)buffer);
                return 1;
            }
        }
    }
    return 0;
}

static int toLua_AudioRecorder_saveRecordFile(lua_State *tolua_S)
{
    AudioRecorder *ret = (AudioRecorder*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != ret)
    {
        int argc = lua_gettop(tolua_S);
        if (argc == 3)
        {
            CCmd_Data *buffer = (CCmd_Data*)tolua_tousertype(tolua_S, 2, nullptr);
            if (nullptr != buffer)
            {
                WORD idx = buffer->GetCurrentIndex();
                if (idx + 4 > buffer->GetBufferLenght())
                {
                    return 0;
                }
                // CMD_GF_S_UserVoice
                BYTE tmp[4] = {0};
                memcpy(tmp, (void*)(buffer->m_pBuffer+idx), 4);
                buffer->SetCurrentIndex(idx + 4);
                DWORD dwSendUerId = *(DWORD*)tmp;
                
                idx = buffer->GetCurrentIndex();
                if (idx + 4 > buffer->GetBufferLenght())
                {
                    return 0;
                }
                memset(tmp, 0, 4);
                memcpy(tmp, (void*)(buffer->m_pBuffer+idx), 4);
                buffer->SetCurrentIndex(idx + 4);
                DWORD dwTargetUserID = *(DWORD*)tmp;
                
                idx = buffer->GetCurrentIndex();
                if (idx + 4 > buffer->GetBufferLenght())
                {
                    return 0;
                }
                memset(tmp, 0, 4);
                memcpy(tmp, (void*)(buffer->m_pBuffer+idx), 4);
                buffer->SetCurrentIndex(idx + 4);
                DWORD dwVoiceLength = *(DWORD*)tmp;
                
                idx = buffer->GetCurrentIndex();
                if (idx + dwVoiceLength > buffer->GetBufferLenght())
                {
                    return 0;
                }
                ++m_nAudioCount;
                std::string fileFullPath = ret->m_strDownloadPath + StringUtils::format("audio%d.mp3",m_nAudioCount % AUDIO_BUFFER_SIZE);
                FILE *fp = fopen(fileFullPath.c_str(), "wb+");
                if(fp)
                {
                    fwrite(buffer->m_pBuffer, sizeof(unsigned char), dwVoiceLength, fp);
                    fclose(fp);
                }
                buffer->SetCurrentIndex(idx + dwVoiceLength);
                
                auto nHandler = toluafix_ref_function(tolua_S, 3, 0);
                lua_pushinteger(tolua_S, dwSendUerId);
                lua_pushinteger(tolua_S, dwTargetUserID);
                lua_pushstring(tolua_S, fileFullPath.c_str());
                return LuaEngine::getInstance()->getLuaStack()->executeFunctionByHandler(nHandler, 3);
            }
        }
    }
    return 0;
}

static int toLua_AudioRecorder_setFinishCallBack(lua_State *tolua_S)
{
    AudioRecorder *ret = (AudioRecorder*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != ret)
    {
        int argc = lua_gettop(tolua_S);
        if (argc == 3)
        {
            int audioId = (int)lua_tointeger(tolua_S, 2);
            auto nHandler = toluafix_ref_function(tolua_S, 3, 0);
            AudioEngine::setFinishCallback(audioId,[nHandler, tolua_S](int aid, const std::string & name)
                                                         {
                                                             lua_pushinteger(tolua_S, aid);
                                                             lua_pushstring(tolua_S, name.c_str());
                                                             LuaEngine::getInstance()->getLuaStack()->executeFunctionByHandler(nHandler, 3);
                                                         });
            return 1;
        }
    }
    return 0;
}
#endif

int register_all_recorder()
{
#if CC_ENABLE_SCRIPT_BINDING
    lua_State* tolua_S = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    
    tolua_usertype(tolua_S, "cc.AudioRecorder");
    tolua_cclass(tolua_S,"AudioRecorder","cc.AudioRecorder","",nullptr);
    
    tolua_beginmodule(tolua_S,"AudioRecorder");
    tolua_function(tolua_S,"getInstance",toLua_AudioRecorder_getInstance);
    tolua_function(tolua_S,"init",toLua_AudioRecorder_init);
    tolua_function(tolua_S,"startRecord",toLua_AudioRecorder_startRecord);
    tolua_function(tolua_S,"endRecord",toLua_AudioRecorder_endRecord);
    tolua_function(tolua_S,"cancelRecord",toLua_AudioRecorder_cancelRecord);
    tolua_function(tolua_S,"createSendBuffer",toLua_AudioRecorder_createSendBuffer);
    tolua_function(tolua_S,"saveRecordFile",toLua_AudioRecorder_saveRecordFile);
    tolua_function(tolua_S,"setFinishCallBack",toLua_AudioRecorder_setFinishCallBack);
    tolua_endmodule(tolua_S);
    
    std::string typeName = typeid(AudioRecorder).name();
    g_luaType[typeName] = "cc.AudioRecorder";
    g_typeCast["AudioRecorder"] = "cc.AudioRecorder";
#endif
    return 1;
}
