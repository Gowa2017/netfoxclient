#include "Stdafx.h"
#include "Resource.h"
#include "DlgServerMatch.h"
#include "DlgMatchWizard.h"
#include "ModuleDBParameter.h"


//�б�����
#define LIST_STYTE LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgServerMatch, CDialog)

	//��ť��Ϣ
	ON_BN_CLICKED(IDC_LOAD_MATCH, OnBnClickedLoadMatch)
	ON_BN_CLICKED(IDC_CREATE_MATCH, OnBnClickedCreateMatch)
	ON_BN_CLICKED(IDC_DELETE_MATCH, OnBnClickedDeleteMatch)
	ON_BN_CLICKED(IDC_OPTION_MATCH, OnBnClickedOptionMatch)

	//�ؼ���Ϣ
	ON_NOTIFY(NM_DBLCLK, IDC_MATCH_LIST, OnNMDblclkServerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MATCH_LIST, OnLvnItemChangedServerList)

END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CDlgServerMatch::CDlgServerMatch() : CDialog(IDD_SERVER_MATCH)
{
	//ģ�����
	ZeroMemory(&m_GameMatchOption,sizeof(m_GameMatchOption));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));

	return;
}

//��������
CDlgServerMatch::~CDlgServerMatch()
{
}

//��ʼ����
BOOL CDlgServerMatch::OnInitDialog()
{
	__super::OnInitDialog();

	//��ѯ���ݿ�
	QueryMatchDataBase();

	//���ر���
	LoadDBMatchItem();

	return TRUE;  
}

//���ݽ���
void CDlgServerMatch::DoDataExchange(CDataExchange* pDX)
{	
	__super::DoDataExchange(pDX);

	//�����б�
	DDX_Control(pDX,IDC_MATCH_LIST,m_MatchListControl);
}

//�򿪱���
bool CDlgServerMatch::OpenGameMatch(tagGameServiceAttrib & GameServiceAttrib,tagGameServiceOption & GameServiceOption)
{
	//���ñ���
	m_GameServiceOption=GameServiceOption;
	m_GameServiceAttrib=GameServiceAttrib;

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//���÷���
	if (DoModal()==IDC_LOAD_MATCH)
	{
		return true;
	}

	return false;
}

//���ط���
bool CDlgServerMatch::LoadDBMatchItem()
{
	//�����б�
	m_MatchListControl.DeleteAllItems();

	//���ð�ť
	GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(FALSE);

	//��ȡ������
	TCHAR szServiceMachine[LEN_MACHINE_ID];
	CWHService::GetMachineID(szServiceMachine);

	//������Ϣ
	CMatchInfoManager MatchInfoManager;
	if (MatchInfoManager.LoadGameMatchOption(m_GameServiceAttrib.wKindID,szServiceMachine,m_MatchInfoBuffer)==true)
	{
		//��������
		tagGameMatchOption * pGameMatchOption=NULL;

		for(INT_PTR nIndex=0;nIndex<m_MatchInfoBuffer.m_GameMatchOptionArray.GetCount();nIndex++)
		{
			//��ȡ����
			pGameMatchOption=m_MatchInfoBuffer.m_GameMatchOptionArray[nIndex];

			//�����б�
			ASSERT(pGameMatchOption!=NULL);
			if (pGameMatchOption!=NULL) m_MatchListControl.InsertMatchOption(pGameMatchOption);
		}
		
		return true;
	}

	return false;
}

//��ѯ���ݿ�
bool CDlgServerMatch::QueryMatchDataBase()
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��ȡ����
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pMatchDBParameter=pModuleDBParameter->GetGameMatchDBParameter();
	tagDataBaseParameter * pPlatformDBParameter=pModuleDBParameter->GetPlatformDBParameter();	

	//��������
	PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
		pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

	//��ȡ��Ϣ
	try
	{
		//��������
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//������Ϣ
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@strDataBaseAddr"),m_GameServiceOption.szDataBaseAddr);

		//ִ�в�ѯ
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadDataBaseInfo"),true)!=DB_SUCCESS)
		{
			//������Ϣ
			TCHAR szErrorDescribe[128]=TEXT("");
			PlatformDBAide.GetValue_String(TEXT("ErrorDescribe"),szErrorDescribe,CountArray(szErrorDescribe));

			//��ʾ��Ϣ
			CTraceService::TraceString(szErrorDescribe,TraceLevel_Exception);

			return false;
		}

		//��ȡ����
		TCHAR szDBUserRead[512]=TEXT(""),szDBPassRead[512]=TEXT("");
		PlatformDBAide.GetValue_String(TEXT("DBUser"),szDBUserRead,CountArray(szDBUserRead));
		PlatformDBAide.GetValue_String(TEXT("DBPassword"),szDBPassRead,CountArray(szDBPassRead));

		//��ȡ��Ϣ
		pMatchDBParameter->wDataBasePort=PlatformDBAide.GetValue_WORD(TEXT("DBPort"));
		lstrcpyn(pMatchDBParameter->szDataBaseAddr,m_GameServiceOption.szDataBaseAddr,CountArray(pMatchDBParameter->szDataBaseAddr));
		lstrcpyn(pMatchDBParameter->szDataBaseName,m_GameServiceOption.szDataBaseName,CountArray(pMatchDBParameter->szDataBaseName));

		//��������
		TCHAR szDataBaseUser[32]=TEXT(""),szDataBasePass[32]=TEXT("");
		CWHEncrypt::XorCrevasse(szDBUserRead,pMatchDBParameter->szDataBaseUser,CountArray(pMatchDBParameter->szDataBaseUser));
		CWHEncrypt::XorCrevasse(szDBPassRead,pMatchDBParameter->szDataBasePass,CountArray(pMatchDBParameter->szDataBasePass));
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//���ر���
VOID CDlgServerMatch::OnBnClickedLoadMatch()
{
	//��ȡѡ��
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//��ȡ����
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagGameMatchOption * pGameMatchOption=(tagGameMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//���ñ���
	CopyMemory(&m_GameMatchOption,pGameMatchOption,sizeof(m_GameMatchOption));

	//�����Ի���
	EndDialog(IDC_LOAD_MATCH);
}

//���ñ���
VOID CDlgServerMatch::OnBnClickedOptionMatch()
{
	//��ȡѡ��
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//��ȡ����
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagGameMatchOption * pGameMatchOption=(tagGameMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//��������
	tagGameMatchOption GameMatchOption;
	CopyMemory(&GameMatchOption,pGameMatchOption,sizeof(GameMatchOption));

	//���ÿؼ�
	CDlgMatchWizard DlgMatchWizard;
	DlgMatchWizard.SetGameMatchOption(GameMatchOption);
	DlgMatchWizard.SetGameKindID(m_GameServiceAttrib.wKindID);	

	//���ñ���
	if (DlgMatchWizard.DoModal()==IDOK)
	{
		//��ȡ����
		tagGameMatchOption GameMatchOption;
		DlgMatchWizard.GetGameMatchOption(GameMatchOption);

		//�޸�����
		m_MatchInfoBuffer.InsertMatchOption(&GameMatchOption);
		m_MatchListControl.UpdateMatchOption(pGameMatchOption);
	}
}

//ɾ������
VOID CDlgServerMatch::OnBnClickedDeleteMatch()
{
	//��ȡѡ��
	ASSERT(m_MatchListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();
	
	//��ȡ����
	INT nListItem=m_MatchListControl.GetNextSelectedItem(Position);
	tagGameMatchOption * pGameMatchOption=(tagGameMatchOption *)m_MatchListControl.GetItemData(nListItem);

	//ɾ������
	CMatchInfoManager MatchInfoManager;
	if (MatchInfoManager.DeleteGameMatchOption(pGameMatchOption->dwMatchID))
	{		
		m_MatchListControl.DeleteItem(nListItem);
		m_MatchInfoBuffer.DeleteMatchOption(pGameMatchOption->dwMatchID);
	}
}

//��������
VOID CDlgServerMatch::OnBnClickedCreateMatch()
{
	//���ÿؼ�
	CDlgMatchWizard DlgMatchWizard;
	DlgMatchWizard.SetGameKindID(m_GameServiceAttrib.wKindID);

	//���÷���
    if (DlgMatchWizard.DoModal()==IDOK)
	{
		//��ȡ����
		tagGameMatchOption GameMatchOption;
		DlgMatchWizard.GetGameMatchOption(GameMatchOption);

		//��������
		tagGameMatchOption * pGameMatchOption=m_MatchInfoBuffer.InsertMatchOption(&GameMatchOption);
		if (pGameMatchOption!=NULL)
		{
			m_MatchListControl.InsertMatchOption(pGameMatchOption);
		}		
	}
}

//˫���б�
VOID CDlgServerMatch::OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��������
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		OnBnClickedLoadMatch();
	}

	return;
}

//ѡ��ı�
VOID CDlgServerMatch::OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//��ȡѡ��
	POSITION Position=m_MatchListControl.GetFirstSelectedItemPosition();

	//�����б�
	if (Position!=NULL)
	{
		//���ð�ť
		GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELETE_MATCH)->EnableWindow(TRUE);
	}
	else
	{
		//���ð�ť
		GetDlgItem(IDC_LOAD_MATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE_MATCH)->EnableWindow(FALSE);
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
