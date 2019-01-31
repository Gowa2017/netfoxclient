//
//  CurlAsset.h
//  GloryProject
//
//  Created by zhong on 16/8/16.
//
//
// curl 上传、下载类

#ifndef ry_CurlAsset_h
#define ry_CurlAsset_h

#include <stdio.h>
#include "cocos2d.h"
#include "curl/curl.h"
#include "curl/easy.h"

int register_all_curlasset();

class CurlAsset : public cocos2d::Node
{
public:
    CurlAsset();
    ~CurlAsset();
    
    bool initCurl();
    
    /////////////////////////////////////
    //MARK:: 上传功能
    
    /*
     * @brief 创建上传
     * @param[url] url链接
     * @param[filepath] 文件路径(用于获取文件大小)
     */
    static CurlAsset* createUploader(const std::string &url,
                                      const std::string &filepath);
    
    bool initUploader(const std::string &url,
                      const std::string &filepath);
    
    /*
     * @brief 添加表单内容
     * @param[contentname] 内容名
     * @param[content] 内容
     * @param[contenttype] 内容类型
     * @return 0:ok
     */
    int addToForm(const std::string &contentname,
                  const std::string &content,
                  const std::string &contenttype = "");
    
    /*
     * @brief 添加文件表单
     * @param[contentname] 内容名
     * @param[content] 内容
     * @param[contenttype] 内容类型
     * @return 0:ok
     */
    int addToFileForm(const std::string &contentname,
                      const std::string &content,
                      const std::string &contenttype = "");
    
    /*
     * @brief 上传文件
     * @param[nHandler] lua回调
     * @param[connecttimeout] 连接超时时间设定
     * @param[uploadtimeout] 上传文件超时时间设定
     */
    void uploadFile(const int &nHandler,
                    const int &connecttimeout = 10,
                    const int &uploadtimeout = 60);
private:
    void uploadCallBack(const int &nHandler, const int &nRes, const std::string &msg);
private:
    curl_httppost* m_pPost;
    curl_httppost* m_pLasePost;
    std::string m_strFilePath;
    /////////////////////////////////////
    
public:
    /////////////////////////////////////
    //MARK:: 下载功能
    
    /*
     * @brief 创建下载
     * @param[notifykey] 下载完之后调用的lua全局函数名
     */
    static CurlAsset *createDownloader(const std::string &funname,
                                        const std::string &url);
    
    bool initDownloader(const std::string &funname,
                        const std::string &url);
    
    void downloadFile(const std::string &savepath,
                      const std::string &savename,
                      const int &connecttimeout = 10,
                      const int &downloadtimeout = 60);
private:
    void downloadNofity(const int &nCode, const std::string &strMsg, const std::string &filename);
    void safeNotify(const int &nCode, const std::string &strMsg, const std::string &filename);
    void safeNotifyWithRelease(const int &nCode, const std::string &strMsg, const std::string &filename);
private:
    std::string m_strFunName;
    /////////////////////////////////////
private:
    void releaseAssert();
private:
    std::string m_strUrl;
    CURL *m_pCurl;
};
#endif /* CurlAsset_h */
