#include "Stdafx.h"
#include "ChatServer.h"
#include "ChatServerDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//�������
CChatServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChatServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CChatServerApp::CChatServerApp()
{
}

//��������
BOOL CChatServerApp::InitInstance()
{
	__super::InitInstance();

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//����ע���
	SetRegistryKey(szProduct);

	//��ʾ����
	CChatServerDlg ChatServerDlg;
	m_pMainWnd=&ChatServerDlg;
	ChatServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
