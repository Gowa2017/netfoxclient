#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"
#include "AndroidUserItemSink.h"
//////////////////////////////////////////////////////////////////////////////////

//机器定义
#ifndef _DEBUG
	#define ANDROID_SERVICE_DLL_NAME	TEXT("SparrowDZAndroidService.dll")	//组件名字
#else
	#define ANDROID_SERVICE_DLL_NAME	TEXT("SparrowDZAndroidServiceD.dll")	//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameServiceManager::CGameServiceManager()
{
	//控件变量
	m_hDllInstance=NULL;
	m_pDlgCustomRule=NULL;

	//内核属性
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_EDUCATE|GAME_GENRE_PERSONAL);
	//m_GameServiceAttrib.wSupporType=(GAME_GENRE_PERSONAL);//zadd

	//功能标志
	m_GameServiceAttrib.cbDynamicJoin=TRUE;
	m_GameServiceAttrib.cbAndroidUser=FALSE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;

	//服务属性
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,TEXT("QPSparrowDZDB"),CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("SparrowHZH.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("SparrowHZHServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//析构函数
CGameServiceManager::~CGameServiceManager()
{
	//删除对象
	SafeDelete(m_pDlgCustomRule);

	//释放 DLL
	if (m_hDllInstance!=NULL)
	{
		AfxFreeLibrary(m_hDllInstance);
		m_hDllInstance=NULL;
	}

	return;
}

//接口查询
VOID * CGameServiceManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//创建桌子
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//变量定义
	CTableFrameSink * pTableFrameSink=NULL;

	try
	{
		//建立对象
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("创建失败");

		//查询接口
		VOID * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("接口查询失败");

		return pObject;
	}
	catch (...) {}

	//删除对象
	SafeDelete(pTableFrameSink);

	return NULL;
}

//创建机器
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	//变量定义
	CAndroidUserItemSink * pAndroidUserItemSink = NULL;

	try
	{
		//建立对象
		pAndroidUserItemSink = new CAndroidUserItemSink();
		if (pAndroidUserItemSink == NULL) throw TEXT("创建失败");

		//查询接口
		void * pObject = pAndroidUserItemSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) throw TEXT("接口查询失败");

		return pObject;
	}
	catch (...) {}

	//删除对象
	SafeDelete(pAndroidUserItemSink);
}

//创建数据
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

//组件属性
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	//设置变量
	GameServiceAttrib=m_GameServiceAttrib;

	return true;
}

//调整参数
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//单元积分
	GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);
	
	tagCustomRule * pCustomRule=(tagCustomRule *)GameServiceOption.cbCustomRule;
	ASSERT(pCustomRule);
	if(NULL == pCustomRule) return true;

	//码数
	if(pCustomRule->cbMaCount < 1|| pCustomRule->cbMaCount > 6)
	{
		pCustomRule->cbMaCount = 2;
	}
	//人数
	if(pCustomRule->cbPlayerCount < 2|| pCustomRule->cbPlayerCount > 4)
	{
		pCustomRule->cbPlayerCount= 4;
	}
	//开始时间
	if(pCustomRule->cbTimeStartGame < 5|| pCustomRule->cbTimeStartGame > 60)
	{
		pCustomRule->cbTimeStartGame = 30;
	}
	//操作时间
	if(pCustomRule->cbTimeOperateCard < 5|| pCustomRule->cbTimeOperateCard > 60)
	{
		pCustomRule->cbTimeOperateCard = 10;
	}
	//出牌时间
	if(pCustomRule->cbTimeOutCard < 5|| pCustomRule->cbTimeOutCard > 60)
	{
		pCustomRule->cbTimeOutCard = 20;
	}

	//金币游戏
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{		
		GameServiceOption.lMinTableScore=__max(GameServiceOption.lCellScore,GameServiceOption.lMinTableScore);
	}

	return true;
}

//获取配置
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//效验状态
	ASSERT(m_pDlgCustomRule!=NULL);
	if (m_pDlgCustomRule==NULL) return false;

	//变量定义
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//获取配置
	if (m_pDlgCustomRule->GetCustomRule(*pCustomRule)==false)
	{
		return false;
	}

	return true;
}

//默认配置
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//变量定义
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//设置变量
	pCustomRule->cbTimeOutCard=20;
	pCustomRule->cbTimeStartGame=30;
	pCustomRule->cbTimeOperateCard=10;

	//游戏控制
	pCustomRule->cbMaCount=2;
	pCustomRule->cbPlayerCount=4;
	return true;
}

//创建窗口
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//创建窗口
	if (m_pDlgCustomRule==NULL)
	{
		m_pDlgCustomRule=new CDlgCustomRule;
	}

	//创建窗口
	if (m_pDlgCustomRule->m_hWnd==NULL)
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//创建窗口
		m_pDlgCustomRule->Create(IDD_CUSTOM_RULE,pParentWnd);

		//还原资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//设置变量
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	m_pDlgCustomRule->SetCustomRule(*((tagCustomRule *)pcbCustomRule));

	//显示窗口
	m_pDlgCustomRule->SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	return m_pDlgCustomRule->GetSafeHwnd();
}

//////////////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
