#ifndef DOWN_ASSETS
#define DWON_ASSETS

#include <string>
#include "cocos2d.h"
using namespace std;

class CDownAsset
{
public:
	typedef std::function<void(int code, const std::string parm)> DoloadAsyncCallBack;
	enum class DoloadType
	{
		DOWN_PRO_INFO = 1,				//下载进度
		DOWN_COMPELETED = 2,			//下载完成
		DOWN_ERROR_PATH = 3,			//目标文件不存�?
		DOWN_ERROR_CREATEFILE = 4,		//创建文件失败
		DOWN_ERROR_CREATEURL = 5,		//创建URL
		DOWN_ERROR_NET = 6,				//下载网络错误
		DOWN_FILE_ERROR = 7,			//目标文件下载异常
		DOWN_PRO_SUB_INFO = 8,				//下载进度
		DOWN_STOPED = 9,				//下载中断
	};
protected:
	std::string	m_szDownUrl;			//下载地址
	std::string	m_szSavePath;			//保存目录
	std::string m_szReName;
    int         m_nRetryTimes;          //

	DoloadAsyncCallBack	m_callBack;		//下载回调
protected:
	CDownAsset(const char* szUrl, const char* szSavePath, const char * szFileName, DoloadAsyncCallBack callback);
	
public:
	virtual ~CDownAsset();

public:
	//@szURL 下载网络地址
	//@szSavePath 下载保存地址
	//@callback 下载回调
	//单文件 下载需要的线程数量
	static void DownMulFile(const char* szUrl, const char* szSavePath, DoloadAsyncCallBack callback, int threads = 1);
	static void DownFile(const char* szUrl, const char* szSavePath, const char * szFileName, DoloadAsyncCallBack callback, int threads = 1);
	
	void DownRun(int threads = 1);
};
#endif
