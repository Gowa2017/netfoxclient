#include "UnZipAsset.h"
#include "FileAsset.h"
#include <thread>
#include "MobileClientKernel.h"
#include "Define.h"
#ifdef MINIZIP_FROM_SYSTEM
#include <minizip/unzip.h>
#else
#include "unzip/unzip.h"
#endif

#define READ_SIZE  (1024*16)
static bool unzip(const char *zipPath, const char *dirpath, const char *passwd, bool bAsset)
{
	// Open the zip file
	CCLOG("unzip info:zippath[%s]\n dirpath[%s]", zipPath, dirpath);
	if (false == FileUtils::getInstance()->isFileExist(zipPath))
	{
		CCLOG("zipfile [%s] not exist", zipPath);
		return false;
	}
	unzFile zipfile;
	if (!bAsset)
	{
		zipfile = unzOpen(zipPath);
	}
	else
	{
		ssize_t len = 0;
		unsigned char *data = CCFileUtils::getInstance()->getFileData(zipPath, "rb", &len);
		zipfile = unzOpenBuffer(data, len);
	}
	if (!zipfile)
	{
		CCLOG("unzip error get zip file false");
		return false;
	}

	std::string szTmpDir = dirpath;
	if (szTmpDir[szTmpDir.length() - 1] != '/') {
		szTmpDir = szTmpDir + "/";
	}
	// Get info about the zip file
	unz_global_info global_info;
	if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
	{
		CCLOG("could not read file global info");
		unzClose(zipfile);
		return false;
	}

	//log("-------------------------------------- unzip file start");
	// Buffer to hold data read from the zip file.
	char read_buffer[READ_SIZE];

	// Loop to extract all files
	//log("-------------------------------------- unzip file num:%d",global_info.number_entry);
	uLong i;
	for (i = 0; i < global_info.number_entry; ++i)
	{
		// Get info about current file.
		unz_file_info file_info;
		char filename[512];
		if (unzGetCurrentFileInfo(
			zipfile,
			&file_info,
			filename,
			512,
			NULL, 0, NULL, 0) != UNZ_OK)
		{
			CCLOG("could not read file info");
			unzClose(zipfile);
			return false;
		}

		// Check if this entry is a directory or file.
		const size_t filename_length = strlen(filename);
		std::string newName = szTmpDir + filename;
		if (filename[filename_length - 1] == '/')
		{
			createDirectory(newName.c_str());

			bool bFilePathExist = FileUtils::getInstance()->isDirectoryExist(newName.c_str());
			//log("-------------------------------------- create dir:%s%d", newName.c_str(), bFilePathExist);
			
		}
		else
		{ 
			// Entry is a file, so extract it.
			//log("-------------------------------------- create zip file name:%s", newName.c_str());
 
			if (passwd)
			{ 
				int nRet = unzOpenCurrentFilePassword(zipfile, passwd);
				if (nRet != UNZ_OK)
				{
					log("could not open file of pass,Err%d", nRet);
					unzClose(zipfile);
					return false;
				}
			}
			else
			{ 
				if (unzOpenCurrentFile(zipfile) != UNZ_OK)
				{
					log("could not open file");
					unzClose(zipfile);
					return false;
				}
			}

			// Open a file to write out the data.
			FILE *out = fopen(newName.c_str(), "wb");
			if (out == NULL)
			{
				log("could not open destination file\n");
				unzCloseCurrentFile(zipfile);
				unzClose(zipfile);
				return false;
			}

			int error = UNZ_OK;
			do
			{
				error = unzReadCurrentFile(zipfile, read_buffer, READ_SIZE);
				if (error < 0)
				{
					log("unzReadCurrentFile error %d\n", error);
					unzCloseCurrentFile(zipfile);
					unzClose(zipfile);
					return false;
				}

				// Write data to file.
				if (error > 0)
				{
					fwrite(read_buffer, error, 1, out); // You should check return of fwrite...
				}
			} while (error > 0);

			fclose(out);
		}

		unzCloseCurrentFile(zipfile);

		// Go the the next entry listed in the zip file.
		if ((i + 1) < global_info.number_entry)
		{
			if (unzGoToNextFile(zipfile) != UNZ_OK)
			{
				CCLOG("cound not read next file\n");
				unzClose(zipfile);
				return false;
			}
		}
	}

	unzClose(zipfile);

	return true;
}

CUnZipAsset::CUnZipAsset(const char* szFilePath,const char* szUnZipPath, const char * szPassword, const UnZipAsyncCallBack & callback)
{
	m_szFilePath = szFilePath;
	m_szUnZipPath = szUnZipPath;
	m_szPassword = szPassword;
	m_callback = callback;
}

CUnZipAsset::~CUnZipAsset()
{

}

void CUnZipAsset::UnZipFile(const char* szFilePath,const char* szUnZipPath, const char * szPassword, const UnZipAsyncCallBack & callback)
{
	std::string strFilePath = szFilePath;
	std::string strUnZipPath = szUnZipPath;
	std::string strPassword = szPassword ? szPassword : "";
	std::thread thr([=]() {
		CUnZipAsset *pUnzipAsset = new CUnZipAsset(strFilePath.c_str(), strUnZipPath.c_str(), strPassword.c_str(),[callback](int code, const std::string parm) {
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([callback, code, parm]() {
				callback(code, parm);
			});
		});
		pUnzipAsset->UnZipRun();
	});
    thr.detach();
}

void CUnZipAsset::UnZipRun()
{
     //解压
    int result = 0;
     //创建解压
	int tryTimes = 0;
    if (createDirectory(m_szUnZipPath.c_str()))
    {
	retryUnZip:
        if(unzip(m_szFilePath.c_str(),m_szUnZipPath.c_str(),m_szPassword.c_str(),true))
      	{
			log("--------------------------------------%s:%d, after unzip path :%s", __FUNCTION__, __LINE__, m_szUnZipPath.c_str());
			if (m_callback)
				m_callback((int)UnZipType::UNZIP_COMPELETED, "");
      	}
		else
		{
			if (tryTimes < 3)
			{
                sleep_ms(1);
				++tryTimes;
				goto retryUnZip;
			}
			else
			{
				if (m_callback)
					m_callback((int)UnZipType::UNZIP_FILE_FAILED, "");
			}
		}
    }
	else
	{
		if (m_callback)
			m_callback((int)UnZipType::UNZIP_ERROR_CREATEFILE, "");
    }

	delete this;
}
