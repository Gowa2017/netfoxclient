#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "Server_shzManager.h"
#include "AndroidUserItemSink.h"
//////////////////////////////////////////////////////////////////////////////////

////机器定义
//#ifndef _DEBUG
//	#define ANDROID_SERVICE_DLL_NAME	TEXT("SGLBnAndroid.dll")	//组件名字
//#else
//	#define ANDROID_SERVICE_DLL_NAME	TEXT("WaterMarginAndroid.dll")	//组件名字
//#endif

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
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_EDUCATE);

	//功能标志
	m_GameServiceAttrib.cbDynamicJoin=FALSE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;

	//服务属性
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	//lstrcpyn(m_GameServiceAttrib.szDataBaseName,TEXT("THWaterMarginDB"),CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("SGLB_Server.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("SGLB_Server.dll"),CountArray(m_GameServiceAttrib.szServerDLLName));

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
	//建立对象
	CAndroidUserItemSink * pAndroidUserItemSink = NULL;
	try
	{
		pAndroidUserItemSink = new CAndroidUserItemSink();
		if (pAndroidUserItemSink == NULL)
		{
			throw TEXT("创建失败");
		}
		void * pObject = pAndroidUserItemSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL)
		{
			throw TEXT("接口查询失败");
		}
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pAndroidUserItemSink);

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

	tagCustomRule * pCustomRule=(tagCustomRule *)GameServiceOption.cbCustomRule;
	
	//单元积分
	GameServiceOption.lCellScore = __max(1L, GameServiceOption.lCellScore);

	//积分下限
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{
		GameServiceOption.lMinTableScore = __max(pCustomRule->lBetScore[0] * 9, GameServiceOption.lMinTableScore);
	}

	//积分上限
	if (GameServiceOption.lRestrictScore != 0L)
	{
		GameServiceOption.lRestrictScore = __max(GameServiceOption.lRestrictScore, GameServiceOption.lMinTableScore);
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
	pCustomRule->lStorageStartTable = 100000;
	pCustomRule->lStorageDeductRoom = 0;
	pCustomRule->lStorageMax1Room = 1000000;
	pCustomRule->lStorageMul1Room = 50;
	pCustomRule->lStorageMax2Room = 5000000;
	pCustomRule->lStorageMul2Room = 75;
	pCustomRule->wGameTwo = INVALID_WORD;
	pCustomRule->wGameTwoDeduct = INVALID_WORD;
	pCustomRule->wGameThree = INVALID_WORD;
	pCustomRule->wGameThreeDeduct = INVALID_WORD;

	pCustomRule->cbBetCount = 9;
	pCustomRule->lBetScore[0] = 10;
	pCustomRule->lBetScore[1] = 20;
	pCustomRule->lBetScore[2] = 30;
	pCustomRule->lBetScore[3] = 40;
	pCustomRule->lBetScore[4] = 50;
	pCustomRule->lBetScore[5] = 60;
	pCustomRule->lBetScore[6] = 70;
	pCustomRule->lBetScore[7] = 80;
	pCustomRule->lBetScore[8] = 90;

	pCustomRule->TheFirstPass[0] = 100;
	pCustomRule->TheFirstPass[1] = 200;
	pCustomRule->TheFirstPass[2] = 300;
	pCustomRule->TheFirstPass[3] = 400;
	pCustomRule->TheFirstPass[4] = 500;

	pCustomRule->TheSecondPass[0] = 0;
	pCustomRule->TheSecondPass[1] = 200;
	pCustomRule->TheSecondPass[2] = 300;
	pCustomRule->TheSecondPass[3] = 400;
	pCustomRule->TheSecondPass[4] = 500;

	pCustomRule->TheThirdPass[0] = 0;
	pCustomRule->TheThirdPass[1] = 0;
	pCustomRule->TheThirdPass[2] = 300;
	pCustomRule->TheThirdPass[3] = 400;
	pCustomRule->TheThirdPass[4] = 500;

	pCustomRule->TheFourthPass[0] = 0;
	pCustomRule->TheFourthPass[1] = 0;
	pCustomRule->TheFourthPass[2] = 0;
	pCustomRule->TheFourthPass[3] = 400;
	pCustomRule->TheFourthPass[4] = 500;
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
