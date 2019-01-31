#include "DownAsset.h"

#include <thread>
#include "MobileClientKernel.h"
#include "FileAsset.h"

#include "Define.h"
#include <sys/stat.h>
#include <regex>
#include "ThreadPool.h"
#include "DownLoader/axel.h"
#include <float.h>

#ifndef LOW_SPEED_LIMIT
#define LOW_SPEED_LIMIT 1L
#endif
#ifndef LOW_SPEED_TIME
#define LOW_SPEED_TIME 5L
#endif

/************************************************************************/
/* 获取要下载的远程文件的大小                                            */
/************************************************************************/
ThreadPool tp(30);
std::atomic_bool bStop;

/* Convert a number of bytes to a human-readable form */
char* trimWhiteSpace(char *str)
{
	if (str == NULL)
	{
		return str;
	}

	char *end;

	// Trim leading space.
	while (isspace(*str))
		str++;

	// All spaces?
	if (*str == 0)
	{
		return str;
	}

	// Trim trailing space.
	end = str + strlen(str) - 1;
	while (end > str && isspace(*end))
		end--;

	// Write new null terminator.
	*(end + 1) = 0;

	return str;
}

std::vector<std::string> split(const  std::string& s, const std::string& delim)
{
	std::vector<std::string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			std::string result = s.substr(pos, len - pos);
			if (result.size())
				elems.push_back(result);
			break;
		}
		std::string result = s.substr(pos, find_pos - pos);
		if (result.size())
			elems.push_back(result);
		pos = find_pos + delim_len;
	}
	return elems;
}

void string_replace(std::string &strBig, const std::string &strsrc, const std::string &strdst)
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = strsrc.size();
	std::string::size_type dstlen = strdst.size();

	while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}

//************************************
// Method:    GetFileOrURLShortName
// FullName:  GetFileOrURLShortName
// Access:    public 
// Returns:   std::string
// Qualifier: 获取路径或URL的文件名（包括后缀，如 C:\Test\abc.xyz --> abc.xyz）
// Parameter: std::string strFullName
//************************************
std::string GetPathOrURLShortName(std::string strFullName)
{
	if (strFullName.empty())
	{
		return "";
	}

	string_replace(strFullName, "/", "\\");

	std::string::size_type iPos = strFullName.find_last_of('\\') + 1;

	return strFullName.substr(iPos, strFullName.length() - iPos);
}

//构造函数
CDownAsset::CDownAsset(const char* szUrl, const char* szSavePath, const char * szFileName, DoloadAsyncCallBack callback)
: m_szDownUrl (szUrl)
, m_szSavePath (szSavePath)
{
	m_szReName = szFileName ? szFileName : "";
	m_callBack = callback;
    m_nRetryTimes = 0;
}

//析构函数
CDownAsset::~CDownAsset()
{

}

void CDownAsset::DownMulFile(const char* szUrl, const char* szSavePath, DoloadAsyncCallBack callback, int threads)
{
	std::string strUrl = szUrl;
	std::string strSavePath = szSavePath;
	std::string strdelim = "{0}";
	auto strUrlVec = split(szUrl, strdelim);
	auto strSaveFilePathVec = split(strSavePath, strdelim);

	if (strUrlVec.size() == 0 || strSaveFilePathVec.size() == 0)
	{
		callback((int)DoloadType::DOWN_FILE_ERROR, "");
		return;
	}

	if (strUrlVec.size() != strSaveFilePathVec.size())
	{
		callback((int)DoloadType::DOWN_FILE_ERROR, "");
		return;
	}

	static float curPercent = 0;
	static int curTaskCount = 0;
	static int totalTaskCounts = 0;

	totalTaskCounts += strUrlVec.size();

	//开启dns缓存
	tcp_dns_cache(true);

	std::function<void()> func = nullptr;
	for (int i = 0; i < strUrlVec.size(); ++i)
	{
		std::string & urlpath = strUrlVec[i];
		//assert(urlname.find(filename) != std::string::npos);
		std::string & savepath = strSaveFilePathVec[i];
		func = [urlpath, savepath, i, callback, threads]()
		{
			CDownAsset *pDownAsset = new CDownAsset(urlpath.c_str(), savepath.c_str(), NULL,[urlpath, callback](int code, std::string parm) {
				switch (code)
				{
				case (int)DoloadType::DOWN_PRO_INFO:
					Director::getInstance()->getScheduler()->performFunctionInCocosThread([code, parm, callback]() {
						callback((int)DoloadType::DOWN_PRO_SUB_INFO, parm);
					});
					break;
				case (int)DoloadType::DOWN_COMPELETED:
					//释放所有的dns缓存
					Director::getInstance()->getScheduler()->performFunctionInCocosThread([code, parm, callback]() {
						if (totalTaskCounts == ++curTaskCount)
						{
							bStop = false;
							curPercent = 0;
							curTaskCount = 0;
							totalTaskCounts = 0;
							callback((int)DoloadType::DOWN_COMPELETED, "");
							tcp_free_dns_cache();
						}
						else
						{
							auto percent = (int)((float)curTaskCount / (float)totalTaskCounts * 100);
							if (fabs(percent - curPercent) >= FLT_EPSILON)
							{
								callback((int)DoloadType::DOWN_PRO_INFO, StringUtils::format("%0.2f", curPercent));
								curPercent = percent;
							}
						}
						//printf("---------------------------------%d\n", curTaskCount);
					});
					break;
				case (int)DoloadType::DOWN_STOPED:
				{
					Director::getInstance()->getScheduler()->performFunctionInCocosThread([code, parm, callback, urlpath]() {
						auto percent = (int)((float)curTaskCount / (float)totalTaskCounts * 100);
						if (fabs(percent - curPercent) >= FLT_EPSILON)
						{
							curPercent = percent;
						}

						callback((int)DoloadType::DOWN_STOPED, urlpath.c_str());

						if (totalTaskCounts == ++curTaskCount)
						{
							bStop = false;
							curPercent = 0;
							curTaskCount = 0;
							totalTaskCounts = 0;

							callback((int)DoloadType::DOWN_COMPELETED, "");
							//释放所有的dns缓存
							tcp_free_dns_cache();
						}
						//printf("---------------------------------%d\n", curTaskCount);
					});

					break;
				}
				default:
				{
					Director::getInstance()->getScheduler()->performFunctionInCocosThread([code, parm, callback, urlpath]() {
						auto percent = (int)((float)curTaskCount / (float)totalTaskCounts * 100);
						if (fabs(percent - curPercent) >= FLT_EPSILON)
						{
							curPercent = percent;
						}

						callback((int)DoloadType::DOWN_FILE_ERROR, urlpath.c_str());

						if (totalTaskCounts == ++curTaskCount)
						{
							bStop = false;
							curPercent = 0;
							curTaskCount = 0;
							totalTaskCounts = 0;

							callback((int)DoloadType::DOWN_COMPELETED, "");
							//释放所有的dns缓存
							tcp_free_dns_cache();
						}
						//printf("---------------------------------%d\n", curTaskCount);
					});
					break;
				}
				}
			});

			pDownAsset->DownRun(threads);
		};
		tp.enqueue(func);
	}
}

//下载单个文件
void CDownAsset::DownFile(const char* szUrl,const char* szSavePath, const char * szFileName, DoloadAsyncCallBack callback, int threds)
{
	CDownAsset *pDownAsset = new CDownAsset(szUrl, szSavePath, szFileName, [callback](int code, std::string parm) {
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([code ,parm, callback]() {
			callback(code, parm);
		});
	});
	tp.enqueue(std::bind(&CDownAsset::DownRun, pDownAsset, threds));

	return;
}

// 下载工作函数
void CDownAsset::DownRun(int threads)
{
	//EnterCriticalSection(&cs);
	FILE* fr = nullptr;

	if (bStop)
	{
		axel_log(fr, "axel this line end:%d\n", __LINE__);
		m_callBack((int)DoloadType::DOWN_STOPED, this->m_szReName);
		return;
	}

	conf_t conf[1] = {0};
	axel_t *axel = nullptr;
	int i, j, cur_head = 0;
	
	char s[256];

#ifdef _WIN32  
	WSADATA data;
	if (WSAStartup(MAKEWORD(1, 1), &data) != 0)
	{
		m_callBack((int)DoloadType::DOWN_ERROR_NET, "Could not initialise Winsock");
		assert(false);
	}
#endif

	std::string fileName = m_szReName.size() ? m_szReName : GetPathOrURLShortName(m_szDownUrl);
	std::string tempFileName = fileName + ".temp";

	axel_log(fr, "axel this line:%d\n", __LINE__);

retry_again:

	do 
	{
		//g_printf.lock();
		//printf("task create:%d\n", creatTaskCounts);
		//g_printf.unlock();

		//创建目录
		if (!FileUtils::getInstance()->isDirectoryExist(m_szSavePath))
		{
			if (createDirectory(m_szSavePath.c_str()) == false)
			{
				//Director::getInstance()->getScheduler()->performFunctionInCocosThread([callback]() {
				if (m_callBack)
					m_callBack((int)DoloadType::DOWN_ERROR_CREATEFILE, "");
				//});
				axel_log(fr, "createDirectory failed %s\n", m_szSavePath.c_str());
				break;
			}
		}
		/* Set up internationalization (i18n) */
		//setlocale(LC_ALL, "");

		if (!conf_init(conf)) {
			axel_log(fr, "axel->message:%d, %s\n", __LINE__, axel->message->text);
			break;
		}

		conf->add_header_count = cur_head;
		conf->num_connections = threads;

        assert(conf->num_connections);

		//m_szReName.size() ? m_szReName.c_str() : NULL
		axel = axel_new(conf, 0, m_szDownUrl.c_str(), m_szSavePath.c_str(), tempFileName.c_str(), fr);

		axel_log(fr, "axel this line:%d\n", __LINE__);

		//不存在上次断点续传文件
		if (!FileUtils::getInstance()->isFileExist(m_szSavePath + tempFileName + ".st"))
		{
			//上次已经完成下载的文件 移除
			if (FileUtils::getInstance()->isFileExist(m_szSavePath + tempFileName))
			{
				FileUtils::getInstance()->removeFile(m_szSavePath + tempFileName);
			}
		}


		axel_log(fr, "axel this line:%d\n", __LINE__);

		if (axel->ready == -1)
		{
			axel_log(fr, "axel->message:%d, %s\n", __LINE__, axel->message->text);

			break;
		}

		axel_log(fr, "axel this line:%d\n", __LINE__);
		if (!axel_open(axel)) {
			axel_log(fr, "axel->message:%d, %s\n", __LINE__, axel->message->text);
			break;
		}
        
		axel_log(fr, "axel this line:%d\n", __LINE__);
		axel_start(axel);

		axel_log(fr, "axel this line:%d\n", __LINE__);

		//sprintf(s, "--------%d\n", __LINE__);
		//fprintf(fr, s);
		//fflush(fr);

		long long int prev;

		prev = 0;

		axel_log(fr, "start task:%d-%s\n", __LINE__, axel->filename);

		while (!axel->ready)
		{
			if (bStop)
			{
				break;
			}
			axel_do(axel);

#ifdef WIN32
			Sleep(1);
#else
			usleep(1);
#endif
			float curPercent = 0.0f;

			if (axel->size == 0)
				curPercent = 100.0f;
			else
				curPercent = (float)axel->bytes_done / (float)axel->size * 100;

			if (fabs(prev - curPercent) >= FLT_EPSILON)
			{
				if (m_callBack)
				{
					m_callBack((int)DoloadType::DOWN_PRO_INFO, StringUtils::format("%0.2f", curPercent));
				}

				prev = curPercent;
			}
		}
		
	} while (0);
	axel_log(fr, "axel this line:%d\n", __LINE__);

	if (axel)
	{
		axel_log(fr, "code:%d Down Size:%d\n  Line:%d\n", axel->ready, axel->size, __LINE__);

		if (axel->ready == 1)
		{
			if (m_callBack)
			{
				m_callBack((int)DoloadType::DOWN_COMPELETED, axel->filename);
				axel_log(fr, "DOWN_COMPELETED %s down\n", axel->filename);
			}
		}
		else
		{
			if (m_callBack)
			{
				if (this->m_nRetryTimes <= 3)
				{
					axel_close(axel);

					if (conf)
						conf_free(conf);

					++this->m_nRetryTimes;

					std::string stFullPathName = m_szSavePath + tempFileName + ".st";
					std::string fullPathName = m_szSavePath + tempFileName;

					unlink(stFullPathName.c_str());
					unlink(fullPathName.c_str());

					goto retry_again;
				}
				else
				{
					std::string stFullPathName = m_szSavePath + tempFileName + ".st";
					std::string fullPathName = m_szSavePath + tempFileName;

					unlink(stFullPathName.c_str());
					unlink(fullPathName.c_str());

					if (!bStop)
					{
						bStop = true;
						m_callBack((int)DoloadType::DOWN_ERROR_NET, axel->filename);
					}
					else
					{
						m_callBack((int)DoloadType::DOWN_STOPED, this->m_szReName);
					}
				}
			}
		}
	}

	bool bSuccessed = axel->ready == 1;

	if (axel)
		axel_close(axel);

	if (conf)
		conf_free(conf);

#ifdef _WIN32  
	WSACleanup();
#endif

	if (!bStop)
	{
		if (bSuccessed)
		{
			if (m_szReName.size())
			{
				std::string targetName = m_szSavePath + tempFileName;
				std::string oldName = m_szSavePath + m_szReName;
				unlink(oldName.c_str());
				rename(targetName.c_str(), oldName.c_str());
			}
			else
			{
				std::string targetName = m_szSavePath + tempFileName;
				std::string oldName = m_szSavePath + fileName;
				unlink(oldName.c_str());
				rename(targetName.c_str(), oldName.c_str());
			}
		}
		else
		{
			std::string stFullPathName = m_szSavePath + tempFileName + ".st";
			unlink(stFullPathName.c_str());
			std::string fileFullPathName = m_szSavePath + tempFileName;
			unlink(fileFullPathName.c_str());

			axel_log(fr, "clean up \n");
			axel_log(fr, "axel this line end:%d\n", __LINE__);
		}
	}

	if (fr)
		fclose(fr);
		
	//释放自己
	delete this;
}
