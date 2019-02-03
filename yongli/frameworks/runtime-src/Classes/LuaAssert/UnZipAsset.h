#ifndef _UN_ZIP_ASSET_H_
#define _UN_ZIP_ASSET_H_

#include <string>
#include "cocos2d.h"

using namespace cocos2d;
using namespace std;


class CUnZipAsset
{
public:
	typedef std::function<void(int code, const std::string parm)> UnZipAsyncCallBack;
	enum class UnZipType
	{
		UNZIP_COMPELETED = 1,			//下载完成
		UNZIP_ERROR_PATH = 2,			//目标文件不存在
		UNZIP_FILE_FAILED = 3,			//目标文件不存在
		UNZIP_ERROR_CREATEFILE = 4,		//创建文件失败
	};

protected:
	std::string	m_szUnZipPath;			//解压目录
	std::string m_szFilePath;			//解压文件
	std::string m_szPassword;			//解压密码
	UnZipAsyncCallBack m_callback;

protected:
	CUnZipAsset(const char* szFilePath,const char* szUnZipPath, const char * szPassword, const UnZipAsyncCallBack & callback);

public:
	virtual ~CUnZipAsset();

public:
	static void UnZipFile(const char* szFilePath,const char* szUnZipPath, const char * szPassword, const UnZipAsyncCallBack & callback);

protected:
	void UnZipRun();
};

#endif