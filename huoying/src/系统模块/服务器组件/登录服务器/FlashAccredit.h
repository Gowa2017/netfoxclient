#pragma once

#include <WinSock2.h>
#include <afxtempl.h>

//////////////////////////////////////////////////////////////////////////
//结构定义
typedef struct _PER_IO_OPERATION_DATA
{
	OVERLAPPED   Overlapped;
	WSABUF       WSABuff;
	SOCKET       socket;
	int          cbOperationType;
	char         cbDataBuff[1024];
}PER_IO_OPERATION_DATA,* LPPER_IO_OPERATION_DATA;

//////////////////////////////////////////////////////////////////////////

typedef CList<LPPER_IO_OPERATION_DATA> CIOOperationItemList;

//////////////////////////////////////////////////////////////////////////
class CFlashAccredit
{
public:
	SOCKET                          m_hServerSocket;                    //连接句柄
	HANDLE                          m_lIOCPHandle;                      //连接句柄
	WORD							m_wServicePort;						//监听端口
	CIOOperationItemList            m_IOSendOperationItemList;
	CIOOperationItemList            m_IORecvOperationItemList;
	CIOOperationItemList            m_IdleIOOperationItemList;

public:
	CFlashAccredit(void);
	~CFlashAccredit(void);

public:
	//启动服务
	bool StartService();
	//停止服务
	bool StopService();
	//设置服务
	void SetServiceParameter(WORD wServicePort);
	//获取缓存
	LPPER_IO_OPERATION_DATA GetIOOperationItem(SOCKET Socket, BYTE cbOperationType);
	//关闭连接
	bool CloseSocket(SOCKET Socket);

public:
	//接受线程
	static void AcceptWork(LPVOID pParam);
	//读写线程
	static void ReadWriteWork(LPVOID pParam);
};
