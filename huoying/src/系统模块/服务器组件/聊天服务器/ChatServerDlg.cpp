#include "Stdafx.h"
#include "Resource.h"
#include "ChatServerDlg.h"
//////////////////////////////////////////////////////////////////////////////////

//��Ϣ����
#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//��������

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChatServerDlg, CDialog)

	//ϵͳ��Ϣ
	ON_WM_QUERYENDSESSION()

	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	//�Զ���Ϣ
	ON_MESSAGE(WM_PROCESS_CMD_LINE,OnMessageProcessCmdLine)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CChatServerDlg::CChatServerDlg() : CDialog(IDD_DLG_GAME_SERVER)
{
	//���ò���
	m_bAutoControl=false;
	m_bOptionSuccess=false;

	return;
}

//��������
CChatServerDlg::~CChatServerDlg()
{
}

//�ؼ���
VOID CChatServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}

//��ʼ������
BOOL CChatServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("��������� -- [ ֹͣ ]"));

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//�������
	m_ServiceUnits.SetServiceUnitsSink(this);

	//�����
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]!=0) PostMessage(WM_PROCESS_CMD_LINE,0,(LPARAM)pszCmdLine);

	return TRUE;
}

//ȷ����Ϣ
VOID CChatServerDlg::OnOK()
{
	return;
}

//ȡ������
VOID CChatServerDlg::OnCancel()
{
	//�ر�ѯ��
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		LPCTSTR pszQuestion=TEXT("��Ϸ���������������У���ȷʵҪ�رշ�������");
		if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
	}

	//ֹͣ����
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//��Ϣ����
BOOL CChatServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//����״̬
VOID CChatServerDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	//״̬����
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//ֹͣ״̬
		{
			//���±���
			UpdateServerTitle(ServiceStatus);

			//����ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("����ֹͣ�ɹ�");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Config:	//����״̬
		{
			//���±���
			UpdateServerTitle(ServiceStatus);

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("���ڳ�ʼ�����...");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Service:	//����״̬
		{
			//���±���
			UpdateServerTitle(ServiceStatus);

			//����ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("���������ɹ�");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	}

	return;
}

//���±���
VOID CChatServerDlg::UpdateServerTitle(enServiceStatus ServiceStatus)
{
	//��������
	LPCTSTR pszStatusName=NULL;
	LPCTSTR pszServerName=NULL;

	//״̬�ַ�
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//ֹͣ״̬
		{
			pszStatusName=TEXT("ֹͣ");
			break;
		}
	case ServiceStatus_Config:	//����״̬
		{
			pszStatusName=TEXT("��ʼ��");
			break;
		}
	case ServiceStatus_Service:	//����״̬
		{
			pszStatusName=TEXT("����");
			break;
		}
	}

	//�������
	TCHAR szTitle[128]=TEXT("");
	_sntprintf(szTitle,CountArray(szTitle),TEXT("��������� -- [ %s ]"),pszStatusName);

	//���ñ���
	SetWindowText(szTitle);

	return;
}

//��������
bool CChatServerDlg::StartServerService(WORD wServerID)
{
	//��������
	try
	{
		m_ServiceUnits.StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return true;
}

//��������
VOID CChatServerDlg::OnBnClickedStartService()
{
	//��������
	try
	{
		m_ServiceUnits.StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//ֹͣ����
VOID CChatServerDlg::OnBnClickedStopService()
{
	//ֹͣ����
	try
	{
		m_ServiceUnits.ConcludeService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//�ر�ѯ��
BOOL CChatServerDlg::OnQueryEndSession()
{
	//��ʾ��Ϣ
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		CTraceService::TraceString(TEXT("�������������У�ϵͳҪ��ע���ػ�����ʧ��"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}

//�����
LRESULT CChatServerDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	//��������
	CWHCommandLine CommandLine;
	LPCTSTR pszCommandLine=(LPCTSTR)(lParam);

	//�����ʶ
	TCHAR szSrverID[32]=TEXT("");
	if (CommandLine.SearchCommandItem(pszCommandLine,TEXT("/ServerID:"),szSrverID,CountArray(szSrverID))==true)
	{
		//��ȡ����
		WORD wServerID=(WORD)(_tstol(szSrverID));

		//��������
		if (wServerID!=0)
		{
			//���ñ���
			m_bAutoControl=true;

			//��������
			StartServerService(wServerID);
		}
	}

	return 0L;
}
//////////////////////////////////////////////////////////////////////////////////
