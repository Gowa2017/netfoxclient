#ifndef PLATFORM_HEAD_FILE
#define PLATFORM_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//包含文件

//定义文件
#include "Macro.h"
#include "Define.h"

//结构文件
#include "Struct.h"
#include "Packet.h"
#include "Property.h"

//模板文件
#include "Array.h"
#include "Module.h"
#include "PacketAide.h"
#include "ServerRule.h"
#include "RightDefine.h"

//全局函数
#include "GlobalFunction.h"

//////////////////////////////////////////////////////////////////////////////////

//程序版本
#define VERSION_FRAME				PROCESS_VERSION(7,0,1)				//框架版本
#define VERSION_PLAZA				PROCESS_VERSION(7,0,1)				//大厅版本
#define VERSION_MOBILE_ANDROID		PROCESS_VERSION(7,0,1)				//手机版本
#define VERSION_MOBILE_IOS			PROCESS_VERSION(7,0,1)				//手机版本

//版本定义
#define VERSION_EFFICACY			0									//效验版本
#define VERSION_FRAME_SDK			INTERFACE_VERSION(7,1)				//框架版本

//////////////////////////////////////////////////////////////////////////////////
//发布版本

#ifndef _DEBUG

//平台常量
const TCHAR szProduct[]=TEXT("网狐棋牌平台");							//产品名字
const TCHAR szPlazaClass[]=TEXT("RYQPGamePlaza");						//广场类名
const TCHAR szProductKey[]=TEXT("RYQPGamePlatform");					//产品主键

//地址定义
const TCHAR szCookieUrl[]=TEXT("http://103.26.78.231");					//记录地址
const TCHAR szLogonServer[]=TEXT("103.26.78.231");						//登录地址
const TCHAR szPlatformLink[]=TEXT("http://103.26.78.231");				//平台网站
const TCHAR szValidateKey[]=TEXT("RYSyncLoginKey");						//验证密钥
const TCHAR szValidateLink[]=TEXT("SyncLogin.aspx?userid=%d&time=%d&signature=%s&url=/"); //验证地址 

#else

//////////////////////////////////////////////////////////////////////////////////
//内测版本

//平台常量
const TCHAR szProduct[]=TEXT("网狐棋牌平台");							//产品名字
const TCHAR szPlazaClass[]=TEXT("WHRYGamePlaza");						//广场类名
const TCHAR szProductKey[]=TEXT("WHRYGamePlatform");					//产品主键

//地址定义
const TCHAR szCookieUrl[]=TEXT("http://103.26.78.231");					//记录地址
const TCHAR szLogonServer[]=TEXT("103.26.78.231");						//登录地址
const TCHAR szPlatformLink[]=TEXT("http://103.26.78.231");				//平台网站
const TCHAR szValidateKey[]=TEXT("RYSyncLoginKey");						//验证密钥
const TCHAR szValidateLink[]=TEXT("SyncLogin.aspx?userid=%d&time=%d&signature=%s&url=/"); //验证地址 

#endif

//////////////////////////////////////////////////////////////////////////////////

//数据库名
const TCHAR szPlatformDB[]=TEXT("RYPlatformDB");						//平台数据库
const TCHAR szAccountsDB[]=TEXT("RYAccountsDB");						//用户数据库
const TCHAR szTreasureDB[]=TEXT("RYTreasureDB");						//财富数据库
const TCHAR szGameMatchDB[]=TEXT("RYGameMatchDB");						//比赛数据库
const TCHAR szExerciseDB[]=TEXT("RYEducateDB"); 						//练习数据库
const TCHAR szGameScoreDB[]=TEXT("RYGameScoreDB");						//练习数据库

//////////////////////////////////////////////////////////////////////////////////

//授权信息
const TCHAR szCompilation[]=TEXT("B3D44854-9C2F-4C78-807F-8C08E940166D");

//////////////////////////////////////////////////////////////////////////////////

#endif