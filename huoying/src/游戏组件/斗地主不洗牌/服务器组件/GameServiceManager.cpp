#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#ifndef _DEBUG
	#define ANDROID_SERVICE_DLL_NAME	TEXT("LandNoShuffleAndroidService.dll")	//�������
#else
	#define ANDROID_SERVICE_DLL_NAME	TEXT("LandNoShuffleAndroidServiceD.dll")	//�������
#endif

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServiceManager::CGameServiceManager()
{
	//�ؼ�����
	m_hDllInstance=NULL;
	m_pDlgCustomRule=NULL;

	//�ں�����
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD|GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_EDUCATE|GAME_GENRE_PERSONAL);

	//���ܱ�־
	m_GameServiceAttrib.cbDynamicJoin=FALSE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=TRUE;

	//��������
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,TEXT("QPLandDB"),CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("LandNoShuffle.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("LandNoShuffleServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//��������
CGameServiceManager::~CGameServiceManager()
{
	//ɾ������
	SafeDelete(m_pDlgCustomRule);

	//�ͷ� DLL
	if (m_hDllInstance!=NULL)
	{
		AfxFreeLibrary(m_hDllInstance);
		m_hDllInstance=NULL;
	}

	return;
}

//�ӿڲ�ѯ
VOID * CGameServiceManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//��������
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������
	CTableFrameSink * pTableFrameSink=NULL;

	try
	{
		//��������
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("����ʧ��");

		//��ѯ�ӿ�
		VOID * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("�ӿڲ�ѯʧ��");

		return pObject;
	}
	catch (...) {}

	//ɾ������
	SafeDelete(pTableFrameSink);

	return NULL;
}

//��������
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	//try
	//{
	//	//����ģ��
	//	if (m_hDllInstance==NULL)
	//	{
	//		m_hDllInstance=AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
	//		if (m_hDllInstance==NULL) throw TEXT("�����˷���ģ�鲻����");
	//	}

	//	//Ѱ�Һ���
	//	ModuleCreateProc * CreateProc=(ModuleCreateProc *)GetProcAddress(m_hDllInstance,"CreateAndroidUserItemSink");
	//	if (CreateProc==NULL) throw TEXT("�����˷���ģ��������Ϸ�");

	//	//�������
	//	return CreateProc(Guid,dwQueryVer);
	//}
	//catch(...) {}


	try
	{
		//�������
		if( m_AndroidServiceHelper.GetInterface() == NULL )
		{
			m_AndroidServiceHelper.SetModuleCreateInfo(ANDROID_SERVICE_DLL_NAME,"CreateGameServiceManager");

			if( !m_AndroidServiceHelper.CreateInstance() ) throw 0;
			m_AndroidServiceHelper->RectifyParameter(m_GameServiceOption);
		}

		//����������
		VOID *pAndroidObject = m_AndroidServiceHelper->CreateAndroidUserItemSink(Guid,dwQueryVer);
		if( pAndroidObject == NULL ) throw TEXT("����������ʧ��");

		return pAndroidObject;
	}
	catch(...) {}

	return NULL;

	return NULL;
}

//��������
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

//�������
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	//���ñ���
	GameServiceAttrib=m_GameServiceAttrib;

	return true;
}

//��������
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//���淿��ѡ��
	m_GameServiceOption=GameServiceOption;

	//��Ԫ����
	//GameServiceOption.lCellScore=__max(1.0,GameServiceOption.lCellScore);

	tagCustomRule * pCustomRule=(tagCustomRule *)GameServiceOption.cbCustomRule;
	
	////�����Ϸ
	//if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|GAME_GENRE_EDUCATE|SCORE_GENRE_POSITIVE|GAME_GENRE_PERSONAL))
	//{		
	//	//�� 3 ����Ϊ ���Խ� 3 ��
	//	GameServiceOption.lMinTableScore=__max(GameServiceOption.lCellScore*pCustomRule->wMaxScoreTimes*2,GameServiceOption.lMinTableScore);
	//}

	//�����
	if(pCustomRule->wMaxScoreTimes < 96 ||  pCustomRule->wMaxScoreTimes > 512)
	{
		pCustomRule->wMaxScoreTimes = 96;
	}

	//���ܱ���
	if(pCustomRule->wFleeScoreTimes < 2 ||  pCustomRule->wFleeScoreTimes > 512)
	{
		pCustomRule->wFleeScoreTimes = 12;
	}
	//����ʱ��
	if(pCustomRule->cbTimeOutCard < 5 ||  pCustomRule->cbTimeOutCard > 60)
	{
		pCustomRule->cbTimeOutCard = 20;
	}
	//�з�ʱ��
	if(pCustomRule->cbTimeCallScore < 5 ||  pCustomRule->cbTimeCallScore > 60)
	{
		pCustomRule->cbTimeCallScore = 20;
	}
	//��ʼʱ��
	if(pCustomRule->cbTimeStartGame < 5 ||  pCustomRule->cbTimeStartGame > 60)
	{
		pCustomRule->cbTimeStartGame = 30;
	}
	//�׳�ʱ��
	if(pCustomRule->cbTimeHeadOutCard < 5 ||  pCustomRule->cbTimeHeadOutCard > 60)
	{
		pCustomRule->cbTimeHeadOutCard = 30;
	}

	return true;
}

//��ȡ����
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//Ч��״̬
	ASSERT(m_pDlgCustomRule!=NULL);
	if (m_pDlgCustomRule==NULL) return false;

	//��������
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//��ȡ����
	if (m_pDlgCustomRule->GetCustomRule(*pCustomRule)==false)
	{
		return false;
	}	

	return true;
}

//Ĭ������
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//��������
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//���ñ���
	pCustomRule->cbTimeOutCard=20;
	pCustomRule->cbTimeStartGame=30;
	pCustomRule->cbTimeCallScore=20;
	pCustomRule->cbTimeHeadOutCard=30;

	//��Ϸ����
	pCustomRule->wMaxScoreTimes=96;
	pCustomRule->wFleeScoreTimes=12;
	pCustomRule->cbFleeScorePatch=FALSE;

	return true;
}

//��������
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//��������
	if (m_pDlgCustomRule==NULL)
	{
		m_pDlgCustomRule=new CDlgCustomRule;
	}

	//��������
	if (m_pDlgCustomRule->m_hWnd==NULL)
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//��������
		m_pDlgCustomRule->Create(IDD_CUSTOM_RULE,pParentWnd);

		//��ԭ��Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//���ñ���
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	m_pDlgCustomRule->SetCustomRule(*((tagCustomRule *)pcbCustomRule));

	//��ʾ����
	m_pDlgCustomRule->SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	return m_pDlgCustomRule->GetSafeHwnd();
}

//////////////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////////////
