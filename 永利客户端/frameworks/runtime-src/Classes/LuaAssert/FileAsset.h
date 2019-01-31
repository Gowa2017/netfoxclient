#ifndef _FILE_ASSET_H_
#define _FILE_ASSET_H_

#include "cocos2d.h"

#include <string>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <fstream>
#include <io.h>
#include <windows.h>
#include <direct.h>
#include <string.h>
#include <stdio.h>
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include <ftw.h>
#endif

#include "platform/CCFileUtils.h"

using namespace cocos2d;
using namespace std;

//文件辅助

//WIN32删除文件夹
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
static bool deleteDirectory(const char* pathName)  
{  
    struct _finddata_t fData;  
    memset(&fData, 0, sizeof(fData));  
  
    if (_chdir(pathName) != 0) //_chdir函数设置当前目录  
    {  
        printf("chdir failed: %s\n",pathName);  
        return true;  
    }  
  
    intptr_t hFile = _findfirst("*",&fData);  //参数1：char *类型，"*"表示通配符，可以查找文件、文件夹  
    if(hFile == -1)  
    {  
        return false;  
    }  
  
    do  
    {  
        if(fData.name[0] == '.')  
            continue;  
        if(fData.attrib == _A_SUBDIR) //子文件夹  
        {  
  
            char dirPath[MAX_PATH];  
            memset(dirPath,0,sizeof(pathName));  
            strcpy_s(dirPath,pathName);  
            strcat_s(dirPath,"\\");  
            strcat_s(dirPath,fData.name);  
  
            deleteDirectory(dirPath);  //recursion subdir  
            printf("remove dir: %s\n",dirPath);  
            _chdir("..");  
            _rmdir(dirPath);  
        }  
        else  
        {  
            char filePath[MAX_PATH];  
            memset(filePath,0,sizeof(filePath));  
            strcpy_s(filePath,pathName);  
            strcat_s(filePath,"\\");  
            strcat_s(filePath,fData.name);  
  
            remove(filePath);  
            printf("remove file: %s\n",filePath);  
        }  
    }while(_findnext(hFile,&fData) == 0);  
  
    _findclose(hFile);  //close  
  
    return true;  
}  
#endif


//删除文件夹
static bool removeDirectory(const std::string& path)
{
	return FileUtils::getInstance()->removeDirectory(path);
}


//创建目录
static bool createDirectory(const char *path)
{
    return FileUtils::getInstance()->createDirectory(path);
}

#endif