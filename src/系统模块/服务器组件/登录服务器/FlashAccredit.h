#pragma once

#include <WinSock2.h>
#include <afxtempl.h>

//////////////////////////////////////////////////////////////////////////
//�ṹ����
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
	SOCKET                          m_hServerSocket;                    //���Ӿ��
	HANDLE                          m_lIOCPHandle;                      //���Ӿ��
	WORD							m_wServicePort;						//�����˿�
	CIOOperationItemList            m_IOSendOperationItemList;
	CIOOperationItemList            m_IORecvOperationItemList;
	CIOOperationItemList            m_IdleIOOperationItemList;

public:
	CFlashAccredit(void);
	~CFlashAccredit(void);

public:
	//��������
	bool StartService();
	//ֹͣ����
	bool StopService();
	//���÷���
	void SetServiceParameter(WORD wServicePort);
	//��ȡ����
	LPPER_IO_OPERATION_DATA GetIOOperationItem(SOCKET Socket, BYTE cbOperationType);
	//�ر�����
	bool CloseSocket(SOCKET Socket);

public:
	//�����߳�
	static void AcceptWork(LPVOID pParam);
	//��д�߳�
	static void ReadWriteWork(LPVOID pParam);
};
