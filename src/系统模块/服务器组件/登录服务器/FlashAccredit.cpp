#include "StdAfx.h"
#include ".\flashaccredit.h"


//////////////////////////////////////////////////////////////////////////
//宏定义
#define RECV_POSTED      101
#define SEND_POSTED      102

//////////////////////////////////////////////////////////////////////////
CFlashAccredit::CFlashAccredit(void)
{
	m_hServerSocket = INVALID_SOCKET;
	m_lIOCPHandle = INVALID_HANDLE_VALUE;
	m_wServicePort = 843;
	m_IOSendOperationItemList.RemoveAll();
	m_IORecvOperationItemList.RemoveAll();
	m_IdleIOOperationItemList.RemoveAll();
}

CFlashAccredit::~CFlashAccredit(void)
{
	StopService();
	WSACleanup();
}

//启动服务
bool CFlashAccredit::StartService()
{
	//加载网络
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2,2),&WSAData)!=0)
	{
		AfxMessageBox(TEXT("加载网络失败!"));
		return false;
	}

	//建立网络
	SOCKADDR_IN SocketAddr;
	ZeroMemory(&SocketAddr,sizeof(SocketAddr));

	//建立网络
	SocketAddr.sin_family=AF_INET;
	SocketAddr.sin_addr.s_addr=INADDR_ANY;
	SocketAddr.sin_port=htons(m_wServicePort);
	m_hServerSocket=WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);

	//错误判断
	if (m_hServerSocket==INVALID_SOCKET) 
	{
		LPCTSTR pszString=TEXT("系统资源不足或者 TCP/IP 协议没有安装，网络启动失败");
		AfxMessageBox(pszString);
		return false;
	}

	//绑定链接
	if (bind(m_hServerSocket,(SOCKADDR*)&SocketAddr,sizeof(SocketAddr))==SOCKET_ERROR)
	{
		LPCTSTR pszString=TEXT("网络绑定发生错误，网络启动失败");
		AfxMessageBox(pszString);
		return false;
	}

	//监听端口
	if (listen(m_hServerSocket,200)==SOCKET_ERROR)
	{
		TCHAR szString[512]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("端口正被其他服务占用，监听 %d 端口失败"), m_wServicePort);
		AfxMessageBox(szString);
		return false;
	}

	//系统信息
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	//完成端口
	m_lIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,SystemInfo.dwNumberOfProcessors);
	if(m_lIOCPHandle == NULL)
	{
		LPCTSTR pszString=TEXT("创建网络资源失败，网络启动失败");
		AfxMessageBox(pszString);
		return false;
	}

	//绑定SOCKET
	if(NULL == CreateIoCompletionPort((HANDLE)m_hServerSocket, m_lIOCPHandle, 0, 0))
	{
		LPCTSTR pszString=TEXT("绑定监听SOCKET失败，网络启动失败");
		AfxMessageBox(pszString);
		return false;
	}

	//创建工作线程
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcceptWork, (LPVOID)this, 0, 0);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadWriteWork, (LPVOID)this, 0, 0);

	return true;
}

//停止服务
bool CFlashAccredit::StopService()
{
	//清理记录
	POSITION pos=m_IORecvOperationItemList.GetHeadPosition();
	for (INT_PTR i=0;i<m_IORecvOperationItemList.GetCount();i++)
	{
		LPPER_IO_OPERATION_DATA pIOOperdata=m_IORecvOperationItemList.GetNext(pos);
		if(pIOOperdata->socket)
		{
			//关闭连接
			closesocket(pIOOperdata->socket);
			delete pIOOperdata;
			pIOOperdata = NULL;
		}
	}
	m_IORecvOperationItemList.RemoveAll();

	//清理记录
	pos=m_IOSendOperationItemList.GetHeadPosition();
	for (INT_PTR i=0;i<m_IOSendOperationItemList.GetCount();i++)
	{
		LPPER_IO_OPERATION_DATA pIOOperdata=m_IOSendOperationItemList.GetNext(pos);
		if(pIOOperdata->socket)
		{
			//关闭连接
			closesocket(pIOOperdata->socket);
			delete pIOOperdata;
			pIOOperdata = NULL;
		}
	}
	m_IOSendOperationItemList.RemoveAll();

	//清理记录
	pos=m_IdleIOOperationItemList.GetHeadPosition();
	for (INT_PTR i=0;i<m_IdleIOOperationItemList.GetCount();i++)
	{
		LPPER_IO_OPERATION_DATA pIOOperdata=m_IdleIOOperationItemList.GetNext(pos);
		if(pIOOperdata->socket)
		{
			//关闭连接
			closesocket(pIOOperdata->socket);
			delete pIOOperdata;
			pIOOperdata = NULL;
		}
	}
	m_IdleIOOperationItemList.RemoveAll();

	return true;
}

//设置服务
void CFlashAccredit::SetServiceParameter(WORD wServicePort)
{
	m_wServicePort=wServicePort;
}

//获取缓存
LPPER_IO_OPERATION_DATA CFlashAccredit::GetIOOperationItem(SOCKET Socket, BYTE cbOperationType)
{
	//参数效验
	ASSERT(cbOperationType==SEND_POSTED || cbOperationType==RECV_POSTED);
	if(cbOperationType!=SEND_POSTED && cbOperationType!=RECV_POSTED) return NULL;

	//定义变量
	LPPER_IO_OPERATION_DATA pIOOperdata = NULL;

	//获取BUFF
	if(m_IdleIOOperationItemList.GetCount() > 0)
	{
		pIOOperdata = m_IdleIOOperationItemList.RemoveHead();
	}
	else //分配缓存
	{
		pIOOperdata = new PER_IO_OPERATION_DATA;
		if( pIOOperdata == NULL) return NULL;
	}

	//设置缓存
	pIOOperdata->socket = Socket;
	memset(&(pIOOperdata->Overlapped),0,sizeof(OVERLAPPED));
	memset(pIOOperdata->cbDataBuff,0,sizeof(pIOOperdata->cbDataBuff));
	pIOOperdata->WSABuff.len = sizeof(pIOOperdata->cbDataBuff);
	pIOOperdata->WSABuff.buf = pIOOperdata->cbDataBuff;
	pIOOperdata->cbOperationType = cbOperationType;

	//记录缓存
	if(cbOperationType == RECV_POSTED)
	{
		m_IORecvOperationItemList.AddTail(pIOOperdata);
	}
	else if(cbOperationType == SEND_POSTED)
	{
		m_IOSendOperationItemList.AddTail(pIOOperdata);
	}

	return pIOOperdata;
}

//关闭连接
bool CFlashAccredit::CloseSocket(SOCKET Socket)
{
	//关闭连接
	closesocket(Socket);

	//清理记录
	POSITION pos=m_IORecvOperationItemList.GetHeadPosition();
	for (INT_PTR i=0;i<m_IORecvOperationItemList.GetCount();i++)
	{
		POSITION tempPos=pos;
		LPPER_IO_OPERATION_DATA pIOOperdata=m_IORecvOperationItemList.GetNext(pos);
		if(pIOOperdata->socket == Socket)
		{
			m_IORecvOperationItemList.RemoveAt(tempPos);
			m_IdleIOOperationItemList.AddTail(pIOOperdata);
			break;
		}
	}

	//清理记录
	pos=m_IOSendOperationItemList.GetHeadPosition();
	for (INT_PTR i=0;i<m_IOSendOperationItemList.GetCount();i++)
	{
		POSITION tempPos=pos;
		LPPER_IO_OPERATION_DATA pIOOperdata=m_IOSendOperationItemList.GetNext(pos);
		if(pIOOperdata->socket == Socket)
		{
			m_IOSendOperationItemList.RemoveAt(tempPos);
			m_IdleIOOperationItemList.AddTail(pIOOperdata);
			break;
		}
	}

	return true;
}

//接受线程
void CFlashAccredit::AcceptWork(LPVOID pParam)
{
	//变量定义
	CFlashAccredit * pFlashAccredit = (CFlashAccredit *)pParam;

	//线程循环
	while(true)
	{
		//接收用户连接，被和完成端口关联
		SOCKET sockAccept = WSAAccept(pFlashAccredit->m_hServerSocket,NULL,NULL,NULL,0);

		//分配缓存
		LPPER_IO_OPERATION_DATA pIOOperdata = pFlashAccredit->GetIOOperationItem(sockAccept, RECV_POSTED);
		if( pIOOperdata == NULL) continue;

		//关联SOCKET和完成端口
		CreateIoCompletionPort((HANDLE)sockAccept,pFlashAccredit->m_lIOCPHandle,NULL,1);

		//投递接收操作
		DWORD dwRecvBytes=0;
		DWORD dwFlags=0;
		INT nRet = WSARecv(pIOOperdata->socket,&(pIOOperdata->WSABuff),1,&dwRecvBytes,&dwFlags,&(pIOOperdata->Overlapped),NULL);
		if(nRet==SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			pFlashAccredit->CloseSocket(sockAccept);
			continue;
		}
	}
}

//读写线程
void CFlashAccredit::ReadWriteWork(LPVOID pParam)
{
	//变量定义
	CFlashAccredit * pFlashAccredit = (CFlashAccredit *)pParam;

	//线程循环
	while(true)
	{
		//等待完成端口上SOCKET的完成
		DWORD dwThancferred=0;
		DWORD  dwCompletionKey=0;
		LPPER_IO_OPERATION_DATA pIOOperdata=NULL;
		GetQueuedCompletionStatus(pFlashAccredit->m_lIOCPHandle,&dwThancferred,&dwCompletionKey,(LPOVERLAPPED *)&pIOOperdata,INFINITE);
		if(pIOOperdata==NULL) continue;

		//检查是否有错误产生
		if(dwThancferred == 0 && (pIOOperdata->cbOperationType == RECV_POSTED || pIOOperdata->cbOperationType == SEND_POSTED))
		{
			//关闭SOCKET
			pFlashAccredit->CloseSocket(pIOOperdata->socket);
			continue;
		}

		//为请求服务
		if(pIOOperdata->cbOperationType == RECV_POSTED)
		{
			//分配缓存
			LPPER_IO_OPERATION_DATA pSendIOOperdata = pFlashAccredit->GetIOOperationItem(pIOOperdata->socket, SEND_POSTED);
			if( pSendIOOperdata == NULL) continue;

			//回应客户端
			CHAR buf[256]=("<?xml version=\"1.0\"?> \r\n<cross-domain-policy> \r\n<site-control permitted-cross-domain-policies=\"all\"/>\r\n<allow-access-from domain=\"*\" to-ports=\"*\" />\r\n</cross-domain-policy> ");
			CopyMemory(pSendIOOperdata->cbDataBuff,buf,sizeof(buf));

			//发送数据
			DWORD dwSendBytes=0;
			INT nRet = WSASend(pSendIOOperdata->socket,&(pSendIOOperdata->WSABuff),1,&dwSendBytes,0,&(pSendIOOperdata->Overlapped),NULL);
			if(nRet==SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				pFlashAccredit->CloseSocket(pSendIOOperdata->socket);
				continue;
			}
		}
		else if(pIOOperdata->cbOperationType == SEND_POSTED)
		{
			//关闭SOCKET
			pFlashAccredit->CloseSocket(pIOOperdata->socket);
		}
	}
}

