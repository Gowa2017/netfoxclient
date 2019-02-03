#include "Stdafx.h"
#include "ChatServer.h"
#include "ChatServerDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//程序对象
CChatServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChatServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CChatServerApp::CChatServerApp()
{
}

//启动函数
BOOL CChatServerApp::InitInstance()
{
	__super::InitInstance();

	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szProduct);

	//显示窗口
	CChatServerDlg ChatServerDlg;
	m_pMainWnd=&ChatServerDlg;
	ChatServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
