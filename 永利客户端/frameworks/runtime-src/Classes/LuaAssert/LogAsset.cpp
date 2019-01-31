//
//  LogAsset.cpp
//  GloryProject
//
//  Created by zhong on 16/8/19.
//
//

#include "LogAsset.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "json/filestream.h"
#include "json/prettywriter.h"

#if CC_ENABLE_SCRIPT_BINDING
#include "CCLuaEngine.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#endif

USING_NS_CC;

static LogAsset* _logInstance = nullptr;
volatile static bool g_bRunLog = false;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include <platform/android/jni/JniHelper.h>

extern "C"
{
    JNIEXPORT void JNICALL Java_org_cocos2dx_lua_nativeLogData(JNIEnv * env, jobject obj, jstring jStr)
    {
        const char* cStr = env->GetStringUTFChars(jStr,nullptr);
        std::string str(cStr);
        env->ReleaseStringUTFChars(jStr, cStr);
        
        LogAsset::getInstance()->logData(str);
    }
}
#endif

LogAsset::LogAsset():
m_strFilePath(""),
m_bEnable(false),
m_bPrettyWrite(false)
{
    m_vecQueue = new std::vector<tagLogData*>();
}

LogAsset::~LogAsset()
{
    for(auto k : *m_vecQueue)
    {
        delete k;
    }
    delete m_vecQueue;
}

LogAsset* LogAsset::getInstance()
{
    if (nullptr == _logInstance)
    {
        _logInstance = new LogAsset();
    }
    return _logInstance;
}

void LogAsset::init(const std::string &logPath, const bool &bEnable, const bool &bPrettyWrite /*= false*/)
{
    m_strFilePath = logPath;
    m_bEnable = bEnable;
    m_bPrettyWrite = bPrettyWrite;
}

void LogAsset::logData(const std::string &str, const bool &bExtraData /*= false*/)
{
    if (!m_bEnable)
    {
        return;
    }
    
    
    tagLogData *logdata = new tagLogData();
    logdata->strData = str;
    logdata->bExtraData = bExtraData;
    
    addToQueue(logdata);
}

void LogAsset::addToQueue(tagLogData *pData)
{
    m_logMutex.lock();
    m_vecQueue->push_back(pData);
    m_logMutex.unlock();
    
    if (!g_bRunLog)
    {
        g_bRunLog = true;
        std::thread thr(&LogAsset::logRun,this);
        thr.detach();
    }
}

void LogAsset::logRun()
{
    bool bQuit = false;
    do
    {
        m_logMutex.lock();
        if (m_vecQueue->empty())
        {
            bQuit = true;
            g_bRunLog = false;
            m_logMutex.unlock();
            break;
        }
        tagLogData* logdata = *(m_vecQueue->begin());
        m_vecQueue->erase(m_vecQueue->begin());
        m_logMutex.unlock();
        
        if (nullptr == logdata)
        {
            continue;
        }
        time_t t = time(nullptr);
        auto nt = localtime(&t);
        auto filepath = StringUtils::format("%s/log/%d-%02d-%02d/%02d-%02d",m_strFilePath.c_str(), (nt->tm_year + 1900),(nt->tm_mon + 1),nt->tm_mday,nt->tm_hour,nt->tm_min);
        if (!FileUtils::getInstance()->isDirectoryExist(filepath))
        {
            FileUtils::getInstance()->createDirectory(filepath);
        }
        filepath += "/data.log";

        std::string data = logdata->strData;
        rapidjson::Document doc;
        if (!FileUtils::getInstance()->isFileExist(filepath))
        {
            doc.SetArray();
        }
        else
        {
            auto str = FileUtils::getInstance()->getStringFromFile(filepath);
            doc.Parse<0>(str.c_str());
            if (doc.HasParseError() || !doc.IsArray())
            {
                backupFile(filepath);
                doc.SetArray();
            }
        }
        
        rapidjson::Document::AllocatorType& alloc = doc.GetAllocator();
        rapidjson::Document doc2;
        doc2.Parse<0>(data.c_str());
        if (logdata->bExtraData)
        {
            rapidjson::Value logValue(rapidjson::kObjectType);            
            auto stime = StringUtils::format("%d/%02d/%02d %02d:%02d:%02d",(nt->tm_year + 1900),(nt->tm_mon + 1),nt->tm_mday,nt->tm_hour,nt->tm_min,nt->tm_sec);
            logValue.AddMember("logtime", rapidjson::Value(stime.c_str(), alloc), alloc);
            if (!doc2.HasParseError())
            {
                logValue.AddMember("logdata", rapidjson::Value(doc2, alloc), alloc);
            }
            else
            {
                logValue.AddMember("logdata", rapidjson::Value(data.c_str(), alloc), alloc);
            }            
            doc.PushBack(logValue, alloc);
        }
        else
        {
            if (!doc2.HasParseError())
            {
                doc.PushBack(rapidjson::Value(doc2, alloc), alloc);
            }
            else
            {
                doc.PushBack(rapidjson::Value(data.c_str(), alloc), alloc);
            }            
        }       
        
        FILE* fp = fopen(filepath.c_str(),"wb");
        if (nullptr != fp)
        {
            if (m_bPrettyWrite)
            {
                rapidjson::FileStream s(fp);
                rapidjson::PrettyWriter<rapidjson::FileStream> writer(s);
                doc.Accept(writer);
            }
            else
            {
                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                doc.Accept(writer);
                
                std::string jsonStr = buffer.GetString();
                ssize_t nLen = jsonStr.length();
                
                fwrite(jsonStr.c_str(), 1, nLen, fp);
            }
            
            fclose(fp);
        }
        
        delete logdata;
    } while (!bQuit);
}

void LogAsset::backupFile(const std::string &filepath)
{
    std::string backUpFile = filepath + ".bak";
    
    Data bData = FileUtils::getInstance()->getDataFromFile(filepath);
    if (bData.getSize() == 0)
    {
        FileUtils::getInstance()->removeFile(filepath);
        return;
    }
    FILE* fp = fopen(backUpFile.c_str(),"wb");
    if (!fp)
    {
        FileUtils::getInstance()->removeFile(filepath);
        return;
    }
    fwrite(bData.getBytes(),1,bData.getSize(),fp);
    fclose(fp);
    
    FileUtils::getInstance()->removeFile(filepath);
}

#if CC_ENABLE_SCRIPT_BINDING
static int toLua_LogAsset_getInstance(lua_State *tolua_S)
{
    int argc = lua_gettop(tolua_S);
    if (1 == argc)
    {
        LogAsset *ret = LogAsset::getInstance();
        object_to_luaval<LogAsset>(tolua_S, "cc.LogAsset", (LogAsset*)ret);
        return 1;
    }
    return 0;
}

static int toLua_LogAsset_init(lua_State *tolua_S)
{
    LogAsset *ret = (LogAsset*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != ret)
    {
        int argc = lua_gettop(tolua_S);
        std::string filepath = "";
        bool bEnable = false;
        bool bPretty = false;
        if (3 == argc)
        {
            filepath = lua_tostring(tolua_S, 2);
            bEnable = lua_toboolean(tolua_S, 3);
        }
        if (4 == argc)
        {
            filepath = lua_tostring(tolua_S, 2);
            bEnable = lua_toboolean(tolua_S, 3);
            bPretty = lua_toboolean(tolua_S, 4);
        }
        ret->init(filepath, bEnable, bPretty);
        return 1;
    }
    return 0;
}

static int toLua_LogAsset_logData(lua_State *tolua_S)
{
    LogAsset *ret = (LogAsset*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != ret)
    {
        int argc = lua_gettop(tolua_S);
        if (2 == argc)
        {
            std::string data = lua_tostring(tolua_S, 2);
            ret->logData(data);
            return 1;
        }
        if (3 == argc)
        {
            std::string data = lua_tostring(tolua_S, 2);
            bool bExtra = lua_toboolean(tolua_S, 3);
            ret->logData(data,bExtra);
            return 1;
        }
    }
    return 0;
}
#endif

int register_all_logasset()
{
#if CC_ENABLE_SCRIPT_BINDING
    lua_State* tolua_S = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    
    tolua_usertype(tolua_S, "cc.LogAsset");
    tolua_cclass(tolua_S,"LogAsset","cc.LogAsset","",nullptr);
    
    tolua_beginmodule(tolua_S,"LogAsset");
    tolua_function(tolua_S,"getInstance",toLua_LogAsset_getInstance);
    tolua_function(tolua_S,"init",toLua_LogAsset_init);
    tolua_function(tolua_S,"logData",toLua_LogAsset_logData);
    tolua_endmodule(tolua_S);
    
    std::string typeName = typeid(LogAsset).name();
    g_luaType[typeName] = "cc.LogAsset";
    g_typeCast["LogAsset"] = "cc.LogAsset";
#endif
    return 1;
}