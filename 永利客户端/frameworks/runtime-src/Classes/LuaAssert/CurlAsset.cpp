//
//  CurlAsset.cpp
//  GloryProject
//
//  Created by zhong on 16/8/16.
//
//

#include "CurlAsset.h"

#if CC_ENABLE_SCRIPT_BINDING
#include "CCLuaEngine.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#endif

USING_NS_CC;

CurlAsset::CurlAsset():
    m_pPost(nullptr),
    m_pLasePost(nullptr),
    m_pCurl(nullptr),
    m_strUrl(""),
    m_strFilePath(""),
    m_strFunName("")
{
    
}

CurlAsset::~CurlAsset()
{
    log("release CurlAsset");
    //release
    releaseAssert();
}

bool CurlAsset::initCurl()
{
    bool bRes = false;
    do
    {
        //初始化curl
        m_pCurl = curl_easy_init();
        CC_BREAK_IF(nullptr == m_pCurl);
        
        bRes = true;
    } while (false);
    return bRes;
}

//MARK:: 上传 begin -
CurlAsset* CurlAsset::createUploader(const std::string &url, const std::string &filepath)
{
    CurlAsset *pCurl = new CurlAsset();
    if(nullptr != pCurl && pCurl->initCurl())
    {
        pCurl->initUploader(url, filepath);
        pCurl->autorelease();
        return pCurl;
    }
    CC_SAFE_DELETE(pCurl);
    return nullptr;
}

bool CurlAsset::initUploader(const std::string &url,
                             const std::string &filepath)
{
    bool bRes = false;
    do
    {
        m_strUrl = url;
        m_strFilePath = filepath;
        m_pPost = nullptr;
        m_pLasePost = nullptr;
        
        bRes = true;
    }
    while (false);
    return bRes;
}

int CurlAsset::addToForm(const std::string &contentname,
                          const std::string &content,
                          const std::string &contenttype /*= ""*/)
{
    if (nullptr == m_pCurl)
    {
        cocos2d::log("curl is nullptr");
        return -1;
    }
    CURLFORMcode nRes = CURL_FORMADD_LAST;
    if (!contenttype.empty())
    {
        nRes = curl_formadd(&m_pPost,
                     &m_pLasePost,
                     CURLFORM_COPYNAME,contentname.c_str(),
                     CURLFORM_COPYCONTENTS,content.c_str(),
                     CURLFORM_CONTENTTYPE, contenttype.c_str(),
                     CURLFORM_END);
    }
    else
    {
        nRes = curl_formadd(&m_pPost,
                     &m_pLasePost,
                     CURLFORM_COPYNAME,contentname.c_str(),
                     CURLFORM_COPYCONTENTS,content.c_str(),
                     CURLFORM_END);
    }
    cocos2d::log("curl form add : %d", nRes);
    return nRes;
}

int CurlAsset::addToFileForm(const std::string &contentname,
                              const std::string &content,
                              const std::string &contenttype /*= ""*/)
{
    if (nullptr == m_pCurl)
    {
        cocos2d::log("curl is nullptr");
        return -1;
    }
    CURLFORMcode nRes = CURL_FORMADD_LAST;
    if (!contenttype.empty())
    {
        nRes = curl_formadd(&m_pPost,
                            &m_pLasePost,
                            CURLFORM_COPYNAME,contentname.c_str(),
                            CURLFORM_FILE,content.c_str(),
                            CURLFORM_CONTENTTYPE, contenttype.c_str(),
                            CURLFORM_END);
    }
    else
    {
        nRes = curl_formadd(&m_pPost,
                            &m_pLasePost,
                            CURLFORM_COPYNAME,contentname.c_str(),
                            CURLFORM_FILE,content.c_str(),
                            CURLFORM_END);
    }
    cocos2d::log("curl form add : %d", nRes);
    return nRes;
}

struct ResponseStruct
{
    ResponseStruct()
    {
        memory = nullptr;
        size = 0;
    }
    char *memory;
    size_t size;
};

static size_t ResponseWriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct ResponseStruct *mem = (struct ResponseStruct *)userp;
    
    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

void CurlAsset::uploadFile(const int &nHandler,
                            const int &connecttimeout /*= 10*/,
                            const int &uploadtimeout /*= 60*/)
{
    if (false == FileUtils::getInstance()->isFileExist(m_strFilePath))
    {
        std::string msg = StringUtils::format("upload error: %s not exist", m_strFilePath.c_str());
        uploadCallBack(nHandler, -1, msg);
        return;
    }
    if (nullptr == m_pCurl || nullptr == m_pPost)
    {
        std::string msg = "curl or post is nullptr";
        uploadCallBack(nHandler, -1, msg);
        return;
    }
    
    //引用计数
    if (1 == this->getReferenceCount())
    {
        retain();
    }
    std::thread thr([=]()
                    {
                        auto length = FileUtils::getInstance()->getFileSize(m_strFilePath);
                        ResponseStruct chunk;
                        chunk.memory = (char*)malloc(1);
                        chunk.size = 0;
                        
                        curl_easy_setopt(m_pCurl, CURLOPT_URL, m_strUrl.c_str());
                        curl_easy_setopt(m_pCurl, CURLOPT_INFILESIZE_LARGE, length);
                        curl_easy_setopt(m_pCurl, CURLOPT_HTTPPOST,m_pPost);
                        
                        curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)&chunk);
                        curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, ResponseWriteCallback);
                        curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, connecttimeout);
                        curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT,uploadtimeout);
                        curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1L);
                        
                        CURLcode nRes = curl_easy_perform(m_pCurl);
                        std::string msg = "";
                        int nMsg = nRes;
                        if (CURLE_OK == nRes)
                        {
                            msg = std::string(chunk.memory);
                        }
                        else
                        {
                            msg = StringUtils::format("curl form add error : %d", nRes);
                        }
                        if (nullptr != chunk.memory)
                        {
                            free(chunk.memory);
                        }                        
                        auto scheduler = Director::getInstance()->getScheduler();
                        scheduler->performFunctionInCocosThread([=]()
                                                                {
                                                                    uploadCallBack(nHandler, nMsg, msg);
                                                                    release();
                                                                });
                    });
    thr.detach();
}

void CurlAsset::uploadCallBack(const int &nHandler, const int &nRes, const std::string &msg)
{
#if CC_ENABLE_SCRIPT_BINDING
    if (0 != nHandler)
    {
        lua_State* tolua_S = LuaEngine::getInstance()->getLuaStack()->getLuaState();
        toluafix_get_function_by_refid(tolua_S, nHandler);
        if (lua_isfunction(tolua_S, -1))
        {
            toluafix_pushusertype_ccobject(tolua_S, this->_ID, &(this->_luaID), (void*)this, "cc.CurlAsset");
            lua_pushinteger(tolua_S, nRes);
            lua_pushstring(tolua_S, msg.c_str());
            LuaEngine::getInstance()->getLuaStack()->executeFunctionByHandler(nHandler, 3);
        }
    }
#endif
}

#if CC_ENABLE_SCRIPT_BINDING
static int toLua_CurlAsset_createUploader(lua_State *tolua_S)
{
    int argc = lua_gettop(tolua_S);
    CurlAsset *obj = nullptr;
    if (3 == argc)
    {
        std::string url = lua_tostring(tolua_S, 2);
        std::string filepath = lua_tostring(tolua_S, 3);
        obj = CurlAsset::createUploader(url, filepath);
    }
    int nID = (nullptr != obj) ? obj->_ID : -1;
    int *pLuaID = (nullptr != obj) ? &obj->_luaID : nullptr;
    toluafix_pushusertype_ccobject(tolua_S,nID,pLuaID,(void*)obj, "cc.CurlAsset");
    return 1;
}

static int toLua_CurlAsset_addToForm(lua_State *tolua_S)
{
    CurlAsset *obj = (CurlAsset*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != obj)
    {
        int argc = lua_gettop(tolua_S);
        std::string name = "";
        std::string content = "";
        std::string sType = "";
        int nRes = 0;
        if (4 == argc)
        {
            name = lua_tostring(tolua_S, 2);
            content = lua_tostring(tolua_S, 3);
            sType = lua_tostring(tolua_S, 4);
            nRes = 1;
        }
        else if (3 == argc)
        {
            name = lua_tostring(tolua_S, 2);
            content = lua_tostring(tolua_S, 3);
            nRes = 1;
        }
        
        auto nOk = obj->addToForm(name, content, sType);
        lua_pushinteger(tolua_S, nOk);
        return nRes;
    }
    return 0;
}

static int toLua_CurlAsset_addToFileForm(lua_State *tolua_S)
{
    CurlAsset *obj = (CurlAsset*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != obj)
    {
        int argc = lua_gettop(tolua_S);
        std::string name = "";
        std::string content = "";
        std::string sType = "";
        int nRes = 0;
        if (4 == argc)
        {
            name = lua_tostring(tolua_S, 2);
            content = lua_tostring(tolua_S, 3);
            sType = lua_tostring(tolua_S, 4);
            nRes = 1;
        }
        else if (3 == argc)
        {
            name = lua_tostring(tolua_S, 2);
            content = lua_tostring(tolua_S, 3);
            nRes = 1;
        }
        
        auto nOk = obj->addToFileForm(name, content, sType);
        lua_pushinteger(tolua_S, nOk);
        return nRes;
    }
    return 0;
}

static int toLua_CurlAsset_uploadFile(lua_State *tolua_S)
{
    CurlAsset *obj = (CurlAsset*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != obj)
    {
        int argc = lua_gettop(tolua_S);
        int nHandler = 0;
        int timeout = 10;
        int utimeout = 60;
        int nRes = 0;
        if (4 == argc)
        {
            nHandler = toluafix_ref_function(tolua_S, 2, 0);
            timeout = lua_tointeger(tolua_S, 3);
            utimeout = lua_tointeger(tolua_S, 4);
            nRes = 1;
        }
        else if (3 == argc)
        {
            nHandler = toluafix_ref_function(tolua_S, 2, 0);
            timeout = lua_tointeger(tolua_S, 3);
            nRes = 1;
        }
        else if (2 == argc)
        {
            nHandler = toluafix_ref_function(tolua_S, 2, 0);
            nRes = 1;
        }
        obj->uploadFile(nHandler, timeout, utimeout);
        return nRes;
    }
    return 0;
}
#endif

//MARK:: 上传 end -

//MARK:: 下载 begin -
CurlAsset* CurlAsset::createDownloader(const std::string &funname,
                                         const std::string &url)
{
    CurlAsset *pd = new CurlAsset();
    if (nullptr != pd && pd->initCurl())
    {
        pd->initDownloader(funname, url);
        pd->autorelease();
        return pd;
    }
    CC_SAFE_DELETE(pd);
    return nullptr;
}

bool CurlAsset::initDownloader(const std::string &funname,
                                const std::string &url)
{
    bool bRes = false;
    do
    {
        m_strUrl = url;
        m_strFunName = funname;
        
        bRes = true;
    } while (false);
    return bRes;
}

static size_t downLoadPackage(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    FILE *fp = (FILE*)userdata;
    if (nullptr != fp)
    {
        size_t written = fwrite(ptr, size, nmemb, fp);
        return written;
    }
    return 0;
}

int downloaderProgressFunc(void *ptr, double totalToDownload, double nowDownloaded, double totalToUpLoad, double nowUpLoaded)
{
    //进度回调
    return 0;
}

void CurlAsset::downloadFile(const std::string &savepath,
                              const std::string &savename,
                              const int &connecttimeout /*= 10*/,
                              const int &downloadtimeout /*= 60*/)
{
    if (nullptr == m_pCurl)
    {
        downloadNofity(-1,"curl init error", savename);
        return;
    }
    
    //引用计数
    if (1 == this->getReferenceCount())
    {
        retain();
    }
    //异步下载
    std::thread ths([=]
                    {
                        //目录
                        if(!FileUtils::getInstance()->isDirectoryExist(savepath))
                        {
                            FileUtils::getInstance()->createDirectory(savepath);
                        }
                        
                        //保存文件
                        std::string savefile = savepath + savename;
                        if (savepath[savepath.length() - 1] != '/')
                        {
                            savefile = savepath + "/" + savename;
                        }
                        FILE *fp = fopen(savefile.c_str(), "wb");
                        int nRes = -1;
                        if(nullptr == fp)
                        {
                            std::string str = StringUtils::format("open file:%s error!", savefile.c_str());
                            safeNotifyWithRelease(nRes, str, savename);
                            return;
                        }
                        //下载地址
                        curl_easy_setopt(m_pCurl, CURLOPT_URL, m_strUrl.c_str());
                        //写入函数
                        curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, downLoadPackage);
                        //写入文件
                        curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, fp);
                        //为了使CURLOPT_PROGRESSFUNCTION被调用. CURLOPT_NOPROGRESS必须被设置为false.
                        curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, false);
                        //下载进度回调
                        curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSFUNCTION, downloaderProgressFunc);
                        //进度回调参数
                        curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSDATA, this);
                        //屏蔽其它信号
                        curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1L);
                        //设置连接超时
                        curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, connecttimeout);
                        //设置下载超时
                        curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT,downloadtimeout);
                        //控制传送字节
                        curl_easy_setopt(m_pCurl, CURLOPT_LOW_SPEED_LIMIT, 1L);
                        //控制多少秒传送CURLOPT_LOW_SPEED_LIMIT
                        curl_easy_setopt(m_pCurl, CURLOPT_LOW_SPEED_TIME, 5L);
                        //设置支持302重定向
                        curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1 );
                        CURLcode res = curl_easy_perform(m_pCurl);
                        if (CURLE_OK == res)
                        {
                            double downsize = 0;
                            res = curl_easy_getinfo(m_pCurl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downsize);
                            if (CURLE_OK == res && downsize > 0)
                            {
                                nRes = 0;
                            }
                        }
                        else
                        {
                            nRes = res;
                        }
                        fclose(fp);
                        
                        safeNotifyWithRelease(nRes,"download over", savename);
                    });
    ths.detach();
}

void CurlAsset::downloadNofity(const int &nCode,
                                const std::string &strMsg,
                                const std::string &filename)
{
#if CC_ENABLE_SCRIPT_BINDING
    lua_State* tolua_S = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    lua_getglobal(tolua_S, m_strFunName.c_str());
    if (!lua_isfunction(tolua_S, -1))
    {
        CCLOG("value at stack [%d] is not function", -1);
        lua_pop(tolua_S, 1);
    }   
    else
    {
        lua_pushinteger(tolua_S, nCode);
        lua_pushstring(tolua_S, strMsg.c_str());
        lua_pushstring(tolua_S, filename.c_str());
        int iRet = lua_pcall(tolua_S, 3, 0, 0);
        if (iRet)
        {
            log("call lua fun error:%s", lua_tostring(tolua_S, -1));
            lua_pop(tolua_S, 1);
        }
    }
#endif
}

void CurlAsset::safeNotify(const int &nCode,
                            const std::string &strMsg,
                            const std::string &filename)
{
    auto scheduler = Director::getInstance()->getScheduler();
    scheduler->performFunctionInCocosThread([=]()
                                            {
                                                downloadNofity(nCode, strMsg, filename);
                                            });
}

void CurlAsset::safeNotifyWithRelease(const int &nCode,
                                       const std::string &strMsg,
                                       const std::string &filename)
{
    auto scheduler = Director::getInstance()->getScheduler();
    scheduler->performFunctionInCocosThread([=]()
                                            {
                                                downloadNofity(nCode, strMsg, filename);
                                                release();
                                            });
}

#if CC_ENABLE_SCRIPT_BINDING
static int toLua_CurlAsset_createDownloader(lua_State *tolua_S)
{
    int argc = lua_gettop(tolua_S);
    CurlAsset *obj = nullptr;
    if (3 == argc)
    {
        std::string notifykey = lua_tostring(tolua_S, 2);
        std::string url = lua_tostring(tolua_S, 3);
        obj = CurlAsset::createDownloader(notifykey, url);
    }
    int nID = (nullptr != obj) ? obj->_ID : -1;
    int *pLuaID = (nullptr != obj) ? &obj->_luaID : nullptr;
    toluafix_pushusertype_ccobject(tolua_S,nID,pLuaID,(void*)obj, "cc.CurlAsset");
    return 1;
}

static int toLua_CurlAsset_downloadFile(lua_State *tolua_S)
{
    CurlAsset *obj = (CurlAsset*)tolua_tousertype(tolua_S, 1, nullptr);
    if (nullptr != obj)
    {
        int argc = lua_gettop(tolua_S);
        std::string strpath = "";
        std::string strname = "";
        int timeout = 10;
        int utimeout = 60;
        int nRes = 0;
        if (5 == argc)
        {
            strpath = lua_tostring(tolua_S, 2);
            strname = lua_tostring(tolua_S, 3);
            timeout = lua_tointeger(tolua_S, 4);
            utimeout = lua_tointeger(tolua_S, 5);
            nRes = 1;
        }
        else if (4 == argc)
        {
            strpath = lua_tostring(tolua_S, 2);
            strname = lua_tostring(tolua_S, 3);
            timeout = lua_tointeger(tolua_S, 4);
            nRes = 1;
        }
        else if (3 == argc)
        {
            strpath = lua_tostring(tolua_S, 2);
            strname = lua_tostring(tolua_S, 3);
            nRes = 1;
        }
        obj->downloadFile(strpath, strname, timeout, utimeout);
        return nRes;
    }
    return 0;
}
#endif
//MARK:: 下载 end -

void CurlAsset::releaseAssert()
{
    if (nullptr != m_pPost)
    {
        curl_formfree(m_pPost);
        m_pPost = nullptr;
    }
    if (nullptr != m_pCurl)
    {
        curl_easy_cleanup(m_pCurl);
        m_pCurl = nullptr;
    }
}

int register_all_curlasset()
{
#if CC_ENABLE_SCRIPT_BINDING
    lua_State* tolua_S = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    
    tolua_usertype(tolua_S, "cc.CurlAsset");
    tolua_cclass(tolua_S,"CurlAsset","cc.CurlAsset","cc.Node",nullptr);
    
    tolua_beginmodule(tolua_S,"CurlAsset");
    //上传
    tolua_function(tolua_S,"createUploader",toLua_CurlAsset_createUploader);
    tolua_function(tolua_S,"addToForm",toLua_CurlAsset_addToForm);
    tolua_function(tolua_S,"addToFileForm",toLua_CurlAsset_addToFileForm);
    tolua_function(tolua_S,"uploadFile",toLua_CurlAsset_uploadFile);
    //上传
    
    //下载
    tolua_function(tolua_S,"createDownloader",toLua_CurlAsset_createDownloader);
    tolua_function(tolua_S,"downloadFile",toLua_CurlAsset_downloadFile);
    //下载
    tolua_endmodule(tolua_S);
    
    std::string typeName = typeid(CurlAsset).name();
    g_luaType[typeName] = "cc.CurlAsset";
    g_typeCast["CurlAsset"] = "cc.CurlAsset";
#endif
    return 1;
}