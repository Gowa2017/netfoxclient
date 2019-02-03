#include "Stdafx.h"
#include "Resource.h"
#include "DlgServerMatch.h"
#include "DlgMatchWizard.h"
#include "ModuleDBParameter.h"


//列表属性
#define LIST_STYTE LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgServerMatch, CDialog)

	//按钮消息
	ON_BN_CLICKED(IDC_LOAD_MATCH, OnBnClickedLoadMatch)
	ON_BN_CLICKED(IDC_CREATE_MATCH, OnBnClickedCreateMatch)
	ON_BN_CLICKED(IDC_DELETE_MATCH, OnBnClickedDeleteMatch)
	ON_BN_CLICKED(IDC_OPTION_MATCH, OnBnClickedOptionMatch)

	//控件消息
	ON_NOTIFY(NM_DBLCLK, IDC_MATCH_LIST, OnNMDblclkServerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MATCH_LIST, OnLvnItemChangedServerList)

END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CDlgServerMatch::CDlgServerMatch() : CDialog(IDD_SERVER_MATCH)
{
	//模块参数
	ZeroMemory(&m_GameMatchOption,sizeof(m_GameMatchOption));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));

	return;
}

//析构函数
CDlgServerMatch::~CDlgServerMatch()
{
}

//初始窗口
BOOL CDlgServerMatch::OnInitDialog()
{
	__super::OnInitDialog();

	//查询数据库
	QueryMatchDataBase();

	//加载比赛
	LoadDBMatchItem();

	return TRUE;  
}

//数据交换
void CDlgServerMatch::DoDataExchange(CDataExchange* pDX)
{	
	__super::DoDataExchange(pDX);

	//比赛列表
	DDX_Control(pDX,IDC_MATCH_LIST,m_MatchListControl);
}

//打开比赛
bool CDlgServerMatch::OpenGameMatch(tagGameServiceAttrib & GameServiceAttrib,tagGameServiceOption & GameServiceOption)
{
	//设置变量
	m_GameServiceOption=GameServiceOption;
	m_GameServiceAttrib=GameServiceAttrib;

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//配置房间
	if (DoModal()==IDC_LOAD_MATCH)
	{
		return true;
	}

	return false;
}

//加载房间
bool CDlgServerMatch::LoadDBMatchItem()
{
	//重置列表
	m_MatchListControl.DeleteAllItems();

	//设置按钮
	GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(FALSE);

	//获取机器码
	TCHAR szServiceMachine[LEN_MACHINE_ID];
	CWHService::GetMachineID(szServiceMachine);

	//加载信息
	CMatchInfoManager MatchInfoManager;
	if (MatchInfoManager.LoadGameMatchOption(m_GameServiceAttrib.wKindID,szServiceMachine,m_MatchInfoBuffer)==true)
	{
		//变量定义
		tagGameMatchOption * pGameMatchOption=NULL;

		for(INT_PTR nIndex=0;nIndex<m_MatchInfoBuffer.m_GameMatchOptionArray.GetCount();nIndex++)
		{
			//获取对象
			pGameMatchOption=m_MatchInfoBuffer.m_GameMatchOptionArray[nIndex];

			//插入列表
			ASSERT(pGameMatchOption!=NULL);
			if (pGameMatchOption!=NULL) m_MatchListControl.InsertMatchOption(pGameMatchOption);
		}
		
		return true;
	}

	return false;
}

//查询数据库
bool CDlgServerMatch::QueryMatchDataBase()
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//获取参数
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pMatchDBParameter=pModuleDBParameter->GetGameMatchDBParameter();
	tagDataBaseParameter * pPlatformDBParameter=pModuleDBParameter->GetPlatformDBParameter();	

	//设置连接
	PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
		pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

	//获取信息
	try
	{
		//发起连接
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//连接信息
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@strDataBaseAddr"),m_GameServiceOption.szDataBaseAddr);

		//执行查询
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadDataBaseInfo"),true)!=DB_SUCCESS)
		{
			//构造信息
			TCHAR szErrorDescribe[128]=TEXT("");
			PlatformDBAide.GetValue_String(TEXT("ErrorDescribe"),szErrorDescribe,CountArray(szErrorDescribe));

			//提示消息
			CTraceService::TraceString(szErrorDescribe,TraceLevel_Exception);

			return false;
		}

		//读取密文
		TCHAR szDBUserRead[512]=TEXT(""),szDBPassRead[512]=TEXT("");
		PlatformDBAide.GetValue_String(TEXT("DBUser"),szDBUserRead,CountArray(szDBUserRead));
		PlatformDBAide.GetValue_String(TEXT("DBPassword"),szDBPassRead,CountArray(szDBPassRead));

		//获取信息
		pMatchDBParameter->wDataBasePort=PlatformDBAide.GetValue_WORD(TEXT("DBPort"));
		lstrcpyn(pMatchDBParameter->szDataBaseAddr,m_GameServiceOption.szDataBaseAddr,CountArray(pMatchDBParameter->szDataBaseAddr));
		lstrcpyn(pMatchDBParameter->szDataBaseName,m_GameServiceOption.szDataBaseName,CountArray(pMatchDBParameter->szDataBaseName));

		//解密密文
		TCHAR szDataBaseUser[32]=TEXT(""),szDataBasePass[32]=TEXT("");
		CWHEncrypt::XorCrevasse(szDBUserRead,pMatchDBParameter->szDataBaseUser,CountArray(pMatchDBParameter->szDataBaseUser));
		CWHEncrypt::XorCrevasse(szDBPassRead,pMatchDBParameter->szDataBasePass,CountArray(pMatchDBParameter->szDataBasePass));
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//加载比赛
VOID CDlgServerMatch::OnBnClickedLoadMatch()
{
	//获取选择
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagGameMatchOption * pGameMatchOption=(tagGameMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//设置变量
	CopyMemory(&m_GameMatchOption,pGameMatchOption,sizeof(m_GameMatchOption));

	//结束对话框
	EndDialog(IDC_LOAD_MATCH);
}

//配置比赛
VOID CDlgServerMatch::OnBnClickedOptionMatch()
{
	//获取选择
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagGameMatchOption * pGameMatchOption=(tagGameMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//变量定义
	tagGameMatchOption GameMatchOption;
	CopyMemory(&GameMatchOption,pGameMatchOption,sizeof(GameMatchOption));

	//设置控件
	CDlgMatchWizard DlgMatchWizard;
	DlgMatchWizard.SetGameMatchOption(GameMatchOption);
	DlgMatchWizard.SetGameKindID(m_GameServiceAttrib.wKindID);	

	//配置比赛
	if (DlgMatchWizard.DoModal()==IDOK)
	{
		//获取配置
		tagGameMatchOption GameMatchOption;
		DlgMatchWizard.GetGameMatchOption(GameMatchOption);

		//修改配置
		m_MatchInfoBuffer.InsertMatchOption(&GameMatchOption);
		m_MatchListControl.UpdateMatchOption(pGameMatchOption);
	}
}

//删除比赛
VOID CDlgServerMatch::OnBnClickedDeleteMatch()
{
	//获取选择
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagGameMatchOption * pGameMatchOption=(tagGameMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//删除比赛
	CMatchInfoManager MatchInfoManager;
	if (MatchInfoManager.DeleteGameMatchOption(pGameMatchOption->dwMatchID))
	{		
		m_MatchListControl.DeleteItem(nListItem);
		m_MatchInfoBuffer.DeleteMatchOption(pGameMatchOption->dwMatchID);
	}
}

//创建比赛
VOID CDlgServerMatch::OnBnClickedCreateMatch()
{
	//设置控件
	CDlgMatchWizard DlgMatchWizard;
	DlgMatchWizard.SetGameKindID(m_GameServiceAttrib.wKindID);

	//配置房间
    if (DlgMatchWizard.DoModal()==IDOK)
	{
		//获取配置
		tagGameMatchOption GameMatchOption;
		DlgMatchWizard.GetGameMatchOption(GameMatchOption);

		//插入配置
		tagGameMatchOption * pGameMatchOption=m_MatchInfoBuffer.InsertMatchOption(&GameMatchOption);
		if (pGameMatchOption!=NULL)
		{
			m_MatchListControl.InsertMatchOption(pGameMatchOption);
		}		
	}
}

//双击列表
VOID CDlgServerMatch::OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//加载配置
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		OnBnClickedLoadMatch();
	}

	return;
}

//选择改变
VOID CDlgServerMatch::OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//获取选择
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();

	//设置列表
	if (Position!=NULL)
	{
		//设置按钮
		GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELETE_MATCH)->EnableWindow(TRUE);
	}
	else
	{
		//设置按钮
		GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE_MATCH)->EnableWindow(FALSE);
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
