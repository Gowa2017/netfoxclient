
// Cfishbaojing.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Cfishbaojing.h"
#include "CfishbaojingDlg.h"
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PIPE_NAME_S		L"\\\\.\\pipe\\Pipe_Fishbaojing_S"
#define PIPE_NAME_C		L"\\\\.\\pipe\\Pipe_Fishbaojing_C"
// CCfishbaojingApp

BEGIN_MESSAGE_MAP(CCfishbaojingApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCfishbaojingApp ����

CCfishbaojingApp::CCfishbaojingApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��

}


// Ψһ��һ�� CCfishbaojingApp ����

CCfishbaojingApp theApp;


// CCfishbaojingApp ��ʼ��

BOOL CCfishbaojingApp::InitInstance()
{
	if (!PipeLoading())
	{
		return false;
	}
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO:  Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CCfishbaojingDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
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

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

//�󶨽���
bool CCfishbaojingApp::PipeLoading()
{
	if (!WaitNamedPipe(PIPE_NAME_S, NMPWAIT_WAIT_FOREVER)) //�ȴ������ܵ�
	{
		OutputDebugString(L"��ʼ��ʧ�� Error1�� fish_config150");
		return false;
	}
	//�� ������Ϣ�ܵ�
	m_hServerPipe = CreateFile(PIPE_NAME_S, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //�������ܵ�

	if (INVALID_HANDLE_VALUE == m_hServerPipe) //ע�ⷵ��ֵ
	{
		OutputDebugString(L"��ʼ��ʧ�� Error2 fish_config��160");
		m_hServerPipe = NULL;
		return false;
	}

	///��������
	DWORD dwWrite;
	if (!WriteFile(m_hServerPipe, "InitClient", sizeof("InitClient"), &dwWrite, NULL))
	{
		OutputDebugString(L"д������ʧ�ܣ� fish_config170");
		PostQuitMessage(-1);
		return false;
	}

	//���������ܵ�
	m_hClientPipe = CreateNamedPipe(PIPE_NAME_C, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0, 1, 1024, 1024, 0, NULL);
	if (INVALID_HANDLE_VALUE == m_hClientPipe) //ע�ⷵ��ֵ
	{
		OutputDebugString(L"���������ܵ�ʧ�ܣ� fish_config180");
		m_hClientPipe = NULL;
		return false;
	}
	HANDLE hEvent;
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //�����˹����õ��¼�����
	if (!hEvent)
	{
		OutputDebugString(L"�����¼�����ʧ�ܣ�fish_config190");
		CloseHandle(m_hClientPipe);
		m_hClientPipe = NULL;
		return false;
	}

	OVERLAPPED ovlap;
	//�����������ܵ�����FILE_FLAG_OVERLAPPED,����ָ���˽ṹ��,����һ���˹����õ��¼�������
	ZeroMemory(&ovlap, sizeof(OVERLAPPED)); //���˽ṹ������
	ovlap.hEvent = hEvent; //ֻ��Ҫʹ�ô����ݳ�Ա
	if (!ConnectNamedPipe(m_hClientPipe, &ovlap)) //�ȴ��ͻ������������ܵ�
	{
		if (ERROR_IO_PENDING != GetLastError())
		{
			OutputDebugString(L"�ȴ��ͻ�������ʧ�ܣ�fish_config200");
			CloseHandle(m_hClientPipe);
			CloseHandle(hEvent);
			m_hClientPipe = NULL;
			return false;
		}
	}

	//�����¼����
	std::thread th([&, hEvent]()
	{
		if (WAIT_FAILED == WaitForSingleObject(hEvent, INFINITE)) //�ȴ��¼������Ϊ���ź�״̬
		{
			OutputDebugString(L"�ȴ�����ʧ�ܣ�fish_config190");
			CloseHandle(m_hClientPipe);
			CloseHandle(hEvent);
			m_hClientPipe = NULL;
			return;
		}
		CloseHandle(hEvent); //�Ѿ���һ���ͻ������ӵ������ܵ���ʵ����





		const int BUFSIZE = 1024;
		//�������ӵ�����ѭ����������	
		DWORD dwReadSize;
		for (;;)
		{
			char cbBuffer[1024];
			memset(cbBuffer, 0, sizeof(cbBuffer[0])*BUFSIZE);
			if (!ReadFile(m_hClientPipe, cbBuffer, BUFSIZE, &dwReadSize, nullptr))
			{
				OutputDebugString(L"��ȡ����ʧ�ܣ�\n fish_config211");
				exit(0);
				break;
			}

			///Ч�����
			/*if (dwReadSize<sizeof(IPC_Head)) continue;*/

			///Ч������
			IPC_Head * pHead = (IPC_Head *)cbBuffer;
			//ASSERT((pHead->wVersion == IPC_VER) && (pHead->wPacketSize == dwReadSize));
			if ((pHead->wVersion != IPC_VER) || (pHead->wPacketSize != dwReadSize)) continue;
			///���ݴ���
			this->PipeMsgEvent(pHead->wSubCmdID, pHead + 1, pHead->wPacketSize - sizeof(IPC_Head));

		}
	});
	th.detach();
	return true;
}

///��������
bool CCfishbaojingApp::PipeSendData(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//ASSERT(m_hServerPipe != INVALID_HANDLE_VALUE);
	if (m_hServerPipe == INVALID_HANDLE_VALUE) return false;

	///��������
	IPC_Buffer IPCBuffer;
	IPCBuffer.Head.wVersion = IPC_VER;
	IPCBuffer.Head.wSubCmdID = wSubCmdID;
	IPCBuffer.Head.wMainCmdID = MDM_GF_GAME;
	IPCBuffer.Head.wPacketSize = sizeof(IPCBuffer.Head);

	///��������
	if (pData != NULL)
	{
		///Ч�鳤��
		//ASSERT(wDataSize <= sizeof(IPCBuffer.cbBuffer));
		if (wDataSize>sizeof(IPCBuffer.cbBuffer)) return false;

		///��������
		IPCBuffer.Head.wPacketSize += wDataSize;
		CopyMemory(IPCBuffer.cbBuffer, pData, wDataSize);
	}

	///��������
	DWORD dwWrite;
	if (!WriteFile(m_hServerPipe, &IPCBuffer, IPCBuffer.Head.wPacketSize, &dwWrite, NULL))
	{
		PostQuitMessage(0);
		return true;
	}
	return true;
}

bool CCfishbaojingApp::PipeMsgEvent(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	///Ч��ӿ�
	//ASSERT(m_hClientPipe != INVALID_HANDLE_VALUE);
	if (m_hClientPipe == INVALID_HANDLE_VALUE) return false;
	CCfishbaojingDlg* pDlg = dynamic_cast<CCfishbaojingDlg*>(m_pMainWnd);
	return pDlg->PipeMsgEvent(wSubCmdID, pData, wDataSize);
}

BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

CMyEdit::CMyEdit(void)
{
	m_bFocus = false;
};

void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString str;

	if ('.' == nChar)//С����ֻ������һ��  
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

	else if (('0' <= nChar && '9' >= nChar) || (0x08 == nChar) || (0x2E == nChar) || (0x2D == nChar))//���֡�backspace��delete������  
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}

void CMyEdit::OnSetFocus(CWnd* pCwnd)
{
	__super::OnSetFocus(pCwnd);
	m_bFocus = true;
}

void CMyEdit::OnKillFocus(CWnd *pCwnd)
{
	__super::OnKillFocus(pCwnd);
	m_bFocus = false;
}

