
#include "StdAfx.h"
#include "PipeServer.h"
#include <process.h>
#include <thread>
#include "..\..\..\..\系统模块\消息定义\IPC_GameFrame.h"

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
	//创建命名管道
	m_lserverpipe = CreateNamedPipe(PIPE_NAME_SS, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0, 1, 1024, 1024, 0, NULL);
	if (INVALID_HANDLE_VALUE == m_lserverpipe) //注意返回值
	{

		m_lserverpipe = INVALID_HANDLE_VALUE;
		int a = 3;
		OutputDebugString(L"创建命名管道失败！pip 32");
		return false;
	}
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //创建人工重置的事件对象
	if (!hEvent)
	{
		OutputDebugString(L"创建事件对象失败！pip 38");
		CloseHandle(m_lserverpipe);
		m_lserverpipe = INVALID_HANDLE_VALUE;
		return false;
	}
	OVERLAPPED ovlap;
	//若创建命名管道用了FILE_FLAG_OVERLAPPED,必须指定此结构体,包含一个人工重置的事件对象句柄
	ZeroMemory(&ovlap, sizeof(OVERLAPPED)); //将此结构体清零
	ovlap.hEvent = hEvent; //只需要使用此数据成员
	if (!ConnectNamedPipe(m_lserverpipe, &ovlap)) //等待客户端连接命名管道
	{
		Sleep(100);
		if (ERROR_IO_PENDING != GetLastError())
		{
			OutputDebugString(L"等待客户端连接失败！pip 52");
			CloseHandle(m_lserverpipe);
			CloseHandle(hEvent);
			m_lserverpipe = INVALID_HANDLE_VALUE;
			return false;
		}
	}
	//储存登录内容
	std::thread th([&, hEvent]()
	{
		if (WAIT_FAILED == WaitForSingleObject(hEvent, INFINITE)) //等待事件对象变为有信号状态
		{
			OutputDebugString(L"等待对象失败！pip64");
			CloseHandle(m_lserverpipe);
			CloseHandle(hEvent);
			m_lserverpipe = INVALID_HANDLE_VALUE;
			return;
		}
		CloseHandle(hEvent); //已经有一个客户端连接到命名管道的实例上

		const int BUFSIZE = 8192;
		//已有连接到来，循环接收数据	
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
				if (!WaitNamedPipe(PIPE_NAME_CC, NMPWAIT_WAIT_FOREVER)) //等待命名管道
				{
					OutputDebugString(L"初始化失败 Error1！pip 89");
					return;
				}

				m_lclientpipe = CreateFile(PIPE_NAME_CC, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //打开命名管道
				if (INVALID_HANDLE_VALUE == m_lclientpipe) //注意返回值
				{
					OutputDebugString(L"初始化失败 Error2！pip96");
					return;
				}
				continue;
			}
			///效验参数
			if (dwReadSize == 4)
			{
				userIDD * pHead = (userIDD *)cbBuffer;
				//ASSERT((pHead->wVersion == IPC_VER) && (pHead->wPacketSize == dwReadSize));
				DWORD abc = pHead->UserID;
				///数据处理
				if (m_func1 != nullptr)	m_func1(pHead, sizeof(userIDD));


			}

			if (dwReadSize < sizeof(IPC_Head)) continue;

			///效验数据
			IPC_Head * pHead = (IPC_Head *)cbBuffer;
			//ASSERT((pHead->wVersion == IPC_VER) && (pHead->wPacketSize == dwReadSize));
			if ((pHead->wVersion != IPC_VER) || (pHead->wPacketSize != dwReadSize)) continue;
			///数据处理
			if (m_func != nullptr)	m_func(pHead->wMainCmdID, pHead->wSubCmdID, pHead + 1, pHead->wPacketSize - sizeof(IPC_Head));
		}

	});
	th.detach();
	return true;
}
bool PipeServer::StartPipe()
{
	if (m_hServerPipe != INVALID_HANDLE_VALUE) return false;
	//创建命名管道
	m_hServerPipe = CreateNamedPipe(PIPE_NAME_S, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0, 1, 1024, 1024, 0, NULL);
	if (INVALID_HANDLE_VALUE == m_hServerPipe) //注意返回值
	{
	
		m_hServerPipe = INVALID_HANDLE_VALUE;
		int a = 3;
		OutputDebugString(L"创建命名管道失败！pip 32");
		return false;
	}
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); //创建人工重置的事件对象
	if (!hEvent)
	{
		OutputDebugString(L"创建事件对象失败！pip 38");
		CloseHandle(m_hServerPipe);
		m_hServerPipe = INVALID_HANDLE_VALUE;
		return false;
	}
	OVERLAPPED ovlap;
	//若创建命名管道用了FILE_FLAG_OVERLAPPED,必须指定此结构体,包含一个人工重置的事件对象句柄
	ZeroMemory(&ovlap, sizeof(OVERLAPPED)); //将此结构体清零
	ovlap.hEvent = hEvent; //只需要使用此数据成员
	if (!ConnectNamedPipe(m_hServerPipe, &ovlap)) //等待客户端连接命名管道
	{
		Sleep(100);
		if (ERROR_IO_PENDING != GetLastError())
		{
			OutputDebugString(L"等待客户端连接失败！pip 52");
			CloseHandle(m_hServerPipe);
			CloseHandle(hEvent);
			m_hServerPipe = INVALID_HANDLE_VALUE;
			return false;
		}
	}
	//储存登录内容
	std::thread th([&, hEvent]()
	{
		if (WAIT_FAILED == WaitForSingleObject(hEvent, INFINITE)) //等待事件对象变为有信号状态
		{
			OutputDebugString(L"等待对象失败！pip64");
			CloseHandle(m_hServerPipe);
			CloseHandle(hEvent);
			m_hServerPipe = INVALID_HANDLE_VALUE;
			return;
		}
		CloseHandle(hEvent); //已经有一个客户端连接到命名管道的实例上

		const int BUFSIZE = 8192;
		//已有连接到来，循环接收数据	
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
				if (!WaitNamedPipe(PIPE_NAME_C, NMPWAIT_WAIT_FOREVER)) //等待命名管道
				{
					OutputDebugString(L"初始化失败 Error1！pip 89");
					return;
				}

				m_hClientPipe = CreateFile(PIPE_NAME_C, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //打开命名管道
				if (INVALID_HANDLE_VALUE == m_hClientPipe) //注意返回值
				{
					OutputDebugString(L"初始化失败 Error2！pip96");
					return;
				}
				continue;
			}
			///效验参数
			if (dwReadSize == 4)
			{
				userIDD * pHead = (userIDD *)cbBuffer;
				//ASSERT((pHead->wVersion == IPC_VER) && (pHead->wPacketSize == dwReadSize));
				DWORD abc = pHead->UserID;
				///数据处理
				if (m_func1 != nullptr)	m_func1(pHead,sizeof(userIDD));


			}

			if (dwReadSize < sizeof(IPC_Head)) continue;

			///效验数据
			IPC_Head * pHead = (IPC_Head *)cbBuffer;
			//ASSERT((pHead->wVersion == IPC_VER) && (pHead->wPacketSize == dwReadSize));
			if ((pHead->wVersion != IPC_VER) || (pHead->wPacketSize != dwReadSize)) continue;
			///数据处理
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

	///构造数据
	IPC_Buffer IPCBuffer;
	IPCBuffer.Head.wVersion = IPC_VER;
	IPCBuffer.Head.wSubCmdID = wSubCmdID;
	IPCBuffer.Head.wMainCmdID = wMainCmdID;
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

	///发送数据
	std::thread th([=]()
	{

		DWORD dwWrite;
		if (!WriteFile(m_lclientpipe, &IPCBuffer, IPCBuffer.Head.wPacketSize, &dwWrite, NULL))
		{
			OutputDebugString(L"写入数据失败！pip170");
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

	///构造数据
	IPC_Buffer IPCBuffer;
	IPCBuffer.Head.wVersion = IPC_VER;
	IPCBuffer.Head.wSubCmdID = wSubCmdID;
	IPCBuffer.Head.wMainCmdID = wMainCmdID;
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
	if (!WriteFile(m_hClientPipe, &IPCBuffer, IPCBuffer.Head.wPacketSize, &dwWrite, NULL))
	{
		OutputDebugString(L"写入数据失败！pip170");
		return true;
	}
	return true;
}