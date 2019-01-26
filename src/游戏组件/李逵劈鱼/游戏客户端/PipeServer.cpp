
#include "StdAfx.h"
#include "PipeServer.h"
#include <process.h>
#include <thread>
#include "..\..\..\..\ϵͳģ��\��Ϣ����\IPC_GameFrame.h"

#define PIPE_NAME_S		L"\\\\.\\pipe\\Pipe_FishConfig_S"
#define PIPE_NAME_C		L"\\\\.\\pipe\\Pipe_FishConfig_C"
#define PIPE_NAME_SS    L"\\\\.\\pipe\\Pipe_Fishbaojing_S"
#define PIPE_NAME_CC    L"\\\\.\\pipe\\Pipe_Fishbaojing_C"
PipeServer::PipeServer()
{
	m_hServerPipe = INVALID_HANDLE_VALUE;
	m_hClientPipe = INVALID_HANDLE_VALUE;
	m_lclientpipe = INVALID_HANDLE_VALUE;
	m_lserverpipe = INVALID_HANDLE_VALUE;
}

PipeServer::~PipeServer()
{
	StopPipe();
	StopPipe1();
}
bool PipeServer::StartPipe1()
{
	if (m_lserverpipe != INVALID_HANDLE_VALUE) return false;
	//���������ܵ�
	m_lserverpipe = CreateNamedPipe(PIPE_NAME_SS, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0, 1, 1024, 1024, 0, NULL);
	if (INVALID_HANDLE_VALUE == m_lserverpipe) //ע�ⷵ��ֵ
	{

		m_lserverpipe = INVALID_HANDLE_VALUE;
		int a = 3;
		OutputDebugString(L"���������ܵ�ʧ�ܣ�pip 32");
		return false;
	}
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //�����˹����õ��¼�����
	if (!hEvent)
	{
		OutputDebugString(L"�����¼�����ʧ�ܣ�pip 38");
		CloseHandle(m_lserverpipe);
		m_lserverpipe = INVALID_HANDLE_VALUE;
		return false;
	}
	OVERLAPPED ovlap;
	//�����������ܵ�����FILE_FLAG_OVERLAPPED,����ָ���˽ṹ��,����һ���˹����õ��¼�������
	ZeroMemory(&ovlap, sizeof(OVERLAPPED)); //���˽ṹ������
	ovlap.hEvent = hEvent; //ֻ��Ҫʹ�ô����ݳ�Ա
	if (!ConnectNamedPipe(m_lserverpipe, &ovlap)) //�ȴ��ͻ������������ܵ�
	{
		Sleep(100);
		if (ERROR_IO_PENDING != GetLastError())
		{
			OutputDebugString(L"�ȴ��ͻ�������ʧ�ܣ�pip 52");
			CloseHandle(m_lserverpipe);
			CloseHandle(hEvent);
			m_lserverpipe = INVALID_HANDLE_VALUE;
			return false;
		}
	}
	//�����¼����
	std::thread th([&, hEvent]()
	{
		if (WAIT_FAILED == WaitForSingleObject(hEvent, INFINITE)) //�ȴ��¼������Ϊ���ź�״̬
		{
			OutputDebugString(L"�ȴ�����ʧ�ܣ�pip64");
			CloseHandle(m_lserverpipe);
			CloseHandle(hEvent);
			m_lserverpipe = INVALID_HANDLE_VALUE;
			return;
		}
		CloseHandle(hEvent); //�Ѿ���һ���ͻ������ӵ������ܵ���ʵ����

		const int BUFSIZE = 8192;
		//�������ӵ�����ѭ����������	
		DWORD dwReadSize;
		char cbBuffer[BUFSIZE];
		for (;;)
		{
			memset(cbBuffer, 0, BUFSIZE);
			if (!ReadFile(m_lserverpipe, cbBuffer, BUFSIZE, &dwReadSize, nullptr))
			{
				StopPipe1();
				return;
			}

			if (strcmp(cbBuffer, "InitClient") == 0)
			{
				if (!WaitNamedPipe(PIPE_NAME_CC, NMPWAIT_WAIT_FOREVER)) //�ȴ������ܵ�
				{
					OutputDebugString(L"��ʼ��ʧ�� Error1��pip 89");
					return;
				}

				m_lclientpipe = CreateFile(PIPE_NAME_CC, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //�������ܵ�
				if (INVALID_HANDLE_VALUE == m_lclientpipe) //ע�ⷵ��ֵ
				{
					OutputDebugString(L"��ʼ��ʧ�� Error2��pip96");
					return;
				}
				continue;
			}
			///Ч�����
			if (dwReadSize == 4)
			{
				userIDD * pHead = (userIDD *)cbBuffer;
				//ASSERT((pHead->wVersion == IPC_VER) && (pHead->wPacketSize == dwReadSize));
				DWORD abc = pHead->UserID;
				///���ݴ���
				if (m_func1 != nullptr)	m_func1(pHead, sizeof(userIDD));


			}

			if (dwReadSize < sizeof(IPC_Head)) continue;

			///Ч������
			IPC_Head * pHead = (IPC_Head *)cbBuffer;
			//ASSERT((pHead->wVersion == IPC_VER) && (pHead->wPacketSize == dwReadSize));
			if ((pHead->wVersion != IPC_VER) || (pHead->wPacketSize != dwReadSize)) continue;
			///���ݴ���
			if (m_func != nullptr)	m_func(pHead->wMainCmdID, pHead->wSubCmdID, pHead + 1, pHead->wPacketSize - sizeof(IPC_Head));
		}

	});
	th.detach();
	return true;
}
bool PipeServer::StartPipe()
{
	if (m_hServerPipe != INVALID_HANDLE_VALUE) return false;
	//���������ܵ�
	m_hServerPipe = CreateNamedPipe(PIPE_NAME_S, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0, 1, 1024, 1024, 0, NULL);
	if (INVALID_HANDLE_VALUE == m_hServerPipe) //ע�ⷵ��ֵ
	{
	
		m_hServerPipe = INVALID_HANDLE_VALUE;
		int a = 3;
		OutputDebugString(L"���������ܵ�ʧ�ܣ�pip 32");
		return false;
	}
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //�����˹����õ��¼�����
	if (!hEvent)
	{
		OutputDebugString(L"�����¼�����ʧ�ܣ�pip 38");
		CloseHandle(m_hServerPipe);
		m_hServerPipe = INVALID_HANDLE_VALUE;
		return false;
	}
	OVERLAPPED ovlap;
	//�����������ܵ�����FILE_FLAG_OVERLAPPED,����ָ���˽ṹ��,����һ���˹����õ��¼�������
	ZeroMemory(&ovlap, sizeof(OVERLAPPED)); //���˽ṹ������
	ovlap.hEvent = hEvent; //ֻ��Ҫʹ�ô����ݳ�Ա
	if (!ConnectNamedPipe(m_hServerPipe, &ovlap)) //�ȴ��ͻ������������ܵ�
	{
		Sleep(100);
		if (ERROR_IO_PENDING != GetLastError())
		{
			OutputDebugString(L"�ȴ��ͻ�������ʧ�ܣ�pip 52");
			CloseHandle(m_hServerPipe);
			CloseHandle(hEvent);
			m_hServerPipe = INVALID_HANDLE_VALUE;
			return false;
		}
	}
	//�����¼����
	std::thread th([&, hEvent]()
	{
		if (WAIT_FAILED == WaitForSingleObject(hEvent, INFINITE)) //�ȴ��¼������Ϊ���ź�״̬
		{
			OutputDebugString(L"�ȴ�����ʧ�ܣ�pip64");
			CloseHandle(m_hServerPipe);
			CloseHandle(hEvent);
			m_hServerPipe = INVALID_HANDLE_VALUE;
			return;
		}
		CloseHandle(hEvent); //�Ѿ���һ���ͻ������ӵ������ܵ���ʵ����

		const int BUFSIZE = 8192;
		//�������ӵ�����ѭ����������	
		DWORD dwReadSize;
		char cbBuffer[BUFSIZE];
		for (;;)
		{
			memset(cbBuffer, 0, BUFSIZE);
			if (!ReadFile(m_hServerPipe, cbBuffer, BUFSIZE, &dwReadSize, nullptr))
			{
				StopPipe();
				return ;
			}

			if (strcmp(cbBuffer, "InitClient") == 0)
			{
				if (!WaitNamedPipe(PIPE_NAME_C, NMPWAIT_WAIT_FOREVER)) //�ȴ������ܵ�
				{
					OutputDebugString(L"��ʼ��ʧ�� Error1��pip 89");
					return;
				}

				m_hClientPipe = CreateFile(PIPE_NAME_C, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //�������ܵ�
				if (INVALID_HANDLE_VALUE == m_hClientPipe) //ע�ⷵ��ֵ
				{
					OutputDebugString(L"��ʼ��ʧ�� Error2��pip96");
					return;
				}
				continue;
			}
			///Ч�����
			if (dwReadSize == 4)
			{
				userIDD * pHead = (userIDD *)cbBuffer;
				//ASSERT((pHead->wVersion == IPC_VER) && (pHead->wPacketSize == dwReadSize));
				DWORD abc = pHead->UserID;
				///���ݴ���
				if (m_func1 != nullptr)	m_func1(pHead,sizeof(userIDD));


			}

			if (dwReadSize < sizeof(IPC_Head)) continue;

			///Ч������
			IPC_Head * pHead = (IPC_Head *)cbBuffer;
			//ASSERT((pHead->wVersion == IPC_VER) && (pHead->wPacketSize == dwReadSize));
			if ((pHead->wVersion != IPC_VER) || (pHead->wPacketSize != dwReadSize)) continue;
			///���ݴ���
			if (m_func != nullptr)	m_func(pHead->wMainCmdID, pHead->wSubCmdID, pHead + 1, pHead->wPacketSize - sizeof(IPC_Head));
		}

	});
	th.detach();
	return true;
}

bool PipeServer::StopPipe()
{
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
	return true;
}
bool PipeServer::StopPipe1()
{
	if (INVALID_HANDLE_VALUE != m_lserverpipe)
	{
		CloseHandle(m_lserverpipe);
		m_lserverpipe = INVALID_HANDLE_VALUE;
	}
	if (INVALID_HANDLE_VALUE != m_lclientpipe)
	{
		CloseHandle(m_lclientpipe);
		m_lclientpipe = INVALID_HANDLE_VALUE;
	}
	return true;
}
bool PipeServer::PipeSendData1(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if (m_lclientpipe == INVALID_HANDLE_VALUE) return false;

	///��������
	IPC_Buffer IPCBuffer;
	IPCBuffer.Head.wVersion = IPC_VER;
	IPCBuffer.Head.wSubCmdID = wSubCmdID;
	IPCBuffer.Head.wMainCmdID = wMainCmdID;
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

	///��������
	std::thread th([=]()
	{

		DWORD dwWrite;
		if (!WriteFile(m_lclientpipe, &IPCBuffer, IPCBuffer.Head.wPacketSize, &dwWrite, NULL))
		{
			OutputDebugString(L"д������ʧ�ܣ�pip170");
			return;
		}

	}
	);
	th.detach();

	return true;
}
bool PipeServer::PipeSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if (m_hClientPipe == INVALID_HANDLE_VALUE) return false;

	///��������
	IPC_Buffer IPCBuffer;
	IPCBuffer.Head.wVersion = IPC_VER;
	IPCBuffer.Head.wSubCmdID = wSubCmdID;
	IPCBuffer.Head.wMainCmdID = wMainCmdID;
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
	if (!WriteFile(m_hClientPipe, &IPCBuffer, IPCBuffer.Head.wPacketSize, &dwWrite, NULL))
	{
		OutputDebugString(L"д������ʧ�ܣ�pip170");
		return true;
	}
	return true;
}