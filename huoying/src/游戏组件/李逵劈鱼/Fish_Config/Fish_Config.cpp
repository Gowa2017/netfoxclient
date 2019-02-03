
// Fish_Config.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Fish_Config.h"
#include "Fish_ConfigDlg.h"
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PIPE_NAME_S		L"\\\\.\\pipe\\Pipe_FishConfig_S"
#define PIPE_NAME_C		L"\\\\.\\pipe\\Pipe_FishConfig_C"
// CFish_ConfigApp

BEGIN_MESSAGE_MAP(CFish_ConfigApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFish_ConfigApp 构造

CFish_ConfigApp::CFish_ConfigApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中

	m_hServerPipe = INVALID_HANDLE_VALUE;
	m_hClientPipe = INVALID_HANDLE_VALUE;
	
}


// 唯一的一个 CFish_ConfigApp 对象

CFish_ConfigApp theApp;




// CFish_ConfigApp 初始化

BOOL CFish_ConfigApp::InitInstance()
{
	if (!PipeLoading())
	{
		return false;
	}
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CFish_ConfigDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	if (INVALID_HANDLE_VALUE != m_hServerPipe)
	{
		CloseHandle(m_hServerPipe);
		m_hServerPipe = INVALID_HANDLE_VALUE;
	}
	if (INVALID_HANDLE_VALUE != m_hClientPipe)
	{
		CloseHandle(m_hClientPipe);
		m_hClientPipe = INVALID_HANDLE_VALUE;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

//绑定进程
bool CFish_ConfigApp::PipeLoading()
{
	if (!WaitNamedPipe(PIPE_NAME_S, NMPWAIT_WAIT_FOREVER)) //等待命名管道
	{
		OutputDebugString(L"初始化失败 Error1！ fish_config129");
		return false;
	}
	  //打开 命名消息管道 连接管道
	m_hServerPipe = CreateFile(PIPE_NAME_S, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //打开命名管道
	
	if (INVALID_HANDLE_VALUE == m_hServerPipe) //注意返回值
	{
		OutputDebugString(L"初始化失败 Error2 fish_config！137");
		m_hServerPipe = NULL;
		return false;
	}

	///发送数据
	DWORD dwWrite;
	if (!WriteFile(m_hServerPipe, "InitClient", sizeof("InitClient"), &dwWrite, NULL))
	{
		OutputDebugString(L"写入数据失败！ fish_config146");
		PostQuitMessage(-1);
		return false;
	}

	//创建命名管道
	m_hClientPipe = CreateNamedPipe(PIPE_NAME_C, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0, 1, 1024, 1024, 0, NULL);
	if (INVALID_HANDLE_VALUE == m_hClientPipe) //注意返回值
	{
		OutputDebugString(L"创建命名管道失败！ fish_config155");
		m_hClientPipe = NULL;
		return false;
	}
	HANDLE hEvent;
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //创建人工重置的事件对象
	if (!hEvent)
	{
		OutputDebugString(L"创建事件对象失败！fish_config163");
		CloseHandle(m_hClientPipe);
		m_hClientPipe = NULL;
		return false;
	}

	OVERLAPPED ovlap;
	//若创建命名管道用了FILE_FLAG_OVERLAPPED,必须指定此结构体,包含一个人工重置的事件对象句柄
	ZeroMemory(&ovlap, sizeof(OVERLAPPED)); //将此结构体清零
	ovlap.hEvent = hEvent; //只需要使用此数据成员
	if (!ConnectNamedPipe(m_hClientPipe, &ovlap)) //等待客户端连接命名管道
	{
		if (ERROR_IO_PENDING != GetLastError())
		{
			OutputDebugString(L"等待客户端连接失败！fish_config177");
			CloseHandle(m_hClientPipe);
			CloseHandle(hEvent);
			m_hClientPipe = NULL;
			return false;
		}
	}

	//储存登录内容
	std::thread th([&, hEvent]()
	{
		if (WAIT_FAILED == WaitForSingleObject(hEvent, INFINITE)) //等待事件对象变为有信号状态
		{
			OutputDebugString(L"等待对象失败！fish_config190");
			CloseHandle(m_hClientPipe);
			CloseHandle(hEvent);
			m_hClientPipe = NULL;
			return;
		}
		CloseHandle(hEvent); //已经有一个客户端连接到命名管道的实例上


		


		const int BUFSIZE = 8192;
		//已有连接到来，循环接收数据	
		DWORD dwReadSize;
		char cbBuffer[BUFSIZE];
		for (;;)
		{
			memset(cbBuffer, 0, sizeof(cbBuffer[0])*BUFSIZE);
			if (!ReadFile(m_hClientPipe, cbBuffer, BUFSIZE, &dwReadSize, nullptr))
			{
				OutputDebugString(L"读取数据失败！\n fish_config211");
				exit(0);
				break;
			}

			///效验参数
			if (dwReadSize<sizeof(IPC_Head)) continue;

			///效验数据
			IPC_Head * pHead = (IPC_Head *)cbBuffer;
			//ASSERT((pHead->wVersion == IPC_VER) && (pHead->wPacketSize == dwReadSize));
			if ((pHead->wVersion != IPC_VER) || (pHead->wPacketSize != dwReadSize)) continue;
			///数据处理
			this->PipeMsgEvent(pHead->wSubCmdID, pHead + 1, pHead->wPacketSize - sizeof(IPC_Head));

		}
	});
	th.detach();
	return true;
}

///发送数据
bool CFish_ConfigApp::PipeSendData(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//ASSERT(m_hServerPipe != INVALID_HANDLE_VALUE);
	if (m_hServerPipe == INVALID_HANDLE_VALUE) return false;

	///构造数据
	IPC_Buffer IPCBuffer;
	IPCBuffer.Head.wVersion = IPC_VER;
	IPCBuffer.Head.wSubCmdID = wSubCmdID;
	IPCBuffer.Head.wMainCmdID = MDM_GF_GAME;
	IPCBuffer.Head.wPacketSize = sizeof(IPCBuffer.Head);

	///附加数据
	if (pData != NULL)
	{
		///效验长度
		//ASSERT(wDataSize <= sizeof(IPCBuffer.cbBuffer));
		if (wDataSize>sizeof(IPCBuffer.cbBuffer)) return false;

		///拷贝数据
		IPCBuffer.Head.wPacketSize += wDataSize;
		CopyMemory(IPCBuffer.cbBuffer, pData, wDataSize);
	}

	///发送数据
	DWORD dwWrite;
	if (!WriteFile(m_hServerPipe, &IPCBuffer, IPCBuffer.Head.wPacketSize, &dwWrite, NULL))
	{
		PostQuitMessage(0);
		return true;
	}
	return true;
}
bool CFish_ConfigApp::pipeSendDataTren(VOID * pData, WORD wDataSize)
{

	if (m_hServerPipe == INVALID_HANDLE_VALUE) return false;
	userIDD *f = static_cast<userIDD*>(pData);

	userIDD h = *f;
	DWORD dwWrite;
	if (!WriteFile(m_hServerPipe, &h, 4, &dwWrite, NULL))
	{
		PostQuitMessage(0);
		return true;
	}
	return true;

}

bool CFish_ConfigApp::PipeMsgEvent(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	///效验接口
	//ASSERT(m_hClientPipe != INVALID_HANDLE_VALUE);
	if (m_hClientPipe == INVALID_HANDLE_VALUE) return false;
	CFish_ConfigDlg* pDlg = dynamic_cast<CFish_ConfigDlg*>(m_pMainWnd);
	return pDlg->PipeMsgEvent(wSubCmdID, pData, wDataSize);
}

BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()
CMyEdit::CMyEdit(void)
{
	m_bFocus=false;
};

void CMyEdit::OnSetFocus(CWnd* pCwnd)
{
	__super::OnSetFocus(pCwnd);
	m_bFocus=true;
}
void CMyEdit::OnKillFocus(CWnd* pCwnd)
{
	__super::OnKillFocus(pCwnd);
	m_bFocus = false;

}

void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString str;

	if ('.' == nChar)//小数点只能输入一次  
	{
		GetWindowText(str);
		if ("" == str)
		{

		}
		else if (-1 != str.Find('.'))
		{

		}
		else
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
		}
	}
	
	else if (('0' <= nChar && '9' >= nChar) || (0x08 == nChar) || (0x2E == nChar)||(0x2D==nChar))//数字、backspace、delete键处理  
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}