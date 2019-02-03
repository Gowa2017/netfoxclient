#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

//////////////////////////////////////////////////////////////////////////////////

//机器定义
#ifndef _DEBUG
	#define ANDROID_SERVICE_DLL_NAME	TEXT("LandNoShuffleAndroidService.dll")	//组件名字
#else
	#define ANDROID_SERVICE_DLL_NAME	TEXT("LandNoShuffleAndroidServiceD.dll")	//组件名字
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

	//功能标志
	m_GameServiceAttrib.cbDynamicJoin=FALSE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=TRUE;

	//服务属性
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,TEXT("QPLandDB"),CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("LandNoShuffle.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("LandNoShuffleServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

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
	//try
	//{
	//	//加载模块
	//	if (m_hDllInstance==NULL)
	//	{
	//		m_hDllInstance=AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
	//		if (m_hDllInstance==NULL) throw TEXT("机器人服务模块不存在");
	//	}

	//	//寻找函数
	//	ModuleCreateProc * CreateProc=(ModuleCreateProc *)GetProcAddress(m_hDllInstance,"CreateAndroidUserItemSink");
	//	if (CreateProc==NULL) throw TEXT("机器人服务模块组件不合法");

	//	//创建组件
	//	return CreateProc(Guid,dwQueryVer);
	//}
	//catch(...) {}


	try
	{
		//创建组件
		if( m_AndroidServiceHelper.GetInterface() == NULL )
		{
			m_AndroidServiceHelper.SetModuleCreateInfo(ANDROID_SERVICE_DLL_NAME,"CreateGameServiceManager");

			if( !m_AndroidServiceHelper.CreateInstance() ) throw 0;
			m_AndroidServiceHelper->RectifyParameter(m_GameServiceOption);
		}

		//创建机器人
		VOID *pAndroidObject = m_AndroidServiceHelper->CreateAndroidUserItemSink(Guid,dwQueryVer);
		if( pAndroidObject == NULL ) throw TEXT("创建机器人失败");

		return pAndroidObject;
	}
	catch(...) {}

	return NULL;

	return NULL;
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
	//保存房间选项
	m_GameServiceOption=GameServiceOption;

	//单元积分
	//GameServiceOption.lCellScore=__max(1.0,GameServiceOption.lCellScore);

	tagCustomRule * pCustomRule=(tagCustomRule *)GameServiceOption.cbCustomRule;
	
	////金币游戏
	//if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|GAME_GENRE_EDUCATE|SCORE_GENRE_POSITIVE|GAME_GENRE_PERSONAL))
	//{		
	//	//乘 3 是因为 可以叫 3 分
	//	GameServiceOption.lMinTableScore=__max(GameServiceOption.lCellScore*pCustomRule->wMaxScoreTimes*2,GameServiceOption.lMinTableScore);
	//}

	//最大倍数
	if(pCustomRule->wMaxScoreTimes < 96 ||  pCustomRule->wMaxScoreTimes > 512)
	{
		pCustomRule->wMaxScoreTimes = 96;
	}

	//逃跑倍数
	if(pCustomRule->wFleeScoreTimes < 2 ||  pCustomRule->wFleeScoreTimes > 512)
	{
		pCustomRule->wFleeScoreTimes = 12;
	}
	//出牌时间
	if(pCustomRule->cbTimeOutCard < 5 ||  pCustomRule->cbTimeOutCard > 60)
	{
		pCustomRule->cbTimeOutCard = 20;
	}
	//叫分时间
	if(pCustomRule->cbTimeCallScore < 5 ||  pCustomRule->cbTimeCallScore > 60)
	{
		pCustomRule->cbTimeCallScore = 20;
	}
	//开始时间
	if(pCustomRule->cbTimeStartGame < 5 ||  pCustomRule->cbTimeStartGame > 60)
	{
		pCustomRule->cbTimeStartGame = 30;
	}
	//首出时间
	if(pCustomRule->cbTimeHeadOutCard < 5 ||  pCustomRule->cbTimeHeadOutCard > 60)
	{
		pCustomRule->cbTimeHeadOutCard = 30;
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
	pCustomRule->cbTimeCallScore=20;
	pCustomRule->cbTimeHeadOutCard=30;

	//游戏控制
	pCustomRule->wMaxScoreTimes=96;
	pCustomRule->wFleeScoreTimes=12;
	pCustomRule->cbFleeScorePatch=FALSE;

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
