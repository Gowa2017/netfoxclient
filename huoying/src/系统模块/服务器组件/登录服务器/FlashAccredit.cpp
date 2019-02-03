#include "StdAfx.h"
#include ".\flashaccredit.h"


//////////////////////////////////////////////////////////////////////////
//�궨��
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

//��������
bool CFlashAccredit::StartService()
{
	//��������
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2,2),&WSAData)!=0)
	{
		AfxMessageBox(TEXT("��������ʧ��!"));
		return false;
	}

	//��������
	SOCKADDR_IN SocketAddr;
	ZeroMemory(&SocketAddr,sizeof(SocketAddr));

	//��������
	SocketAddr.sin_family=AF_INET;
	SocketAddr.sin_addr.s_addr=INADDR_ANY;
	SocketAddr.sin_port=htons(m_wServicePort);
	m_hServerSocket=WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);

	//�����ж�
	if (m_hServerSocket==INVALID_SOCKET) 
	{
		LPCTSTR pszString=TEXT("ϵͳ��Դ������� TCP/IP Э��û�а�װ����������ʧ��");
		AfxMessageBox(pszString);
		return false;
	}

	//������
	if (bind(m_hServerSocket,(SOCKADDR*)&SocketAddr,sizeof(SocketAddr))==SOCKET_ERROR)
	{
		LPCTSTR pszString=TEXT("����󶨷���������������ʧ��");
		AfxMessageBox(pszString);
		return false;
	}

	//�����˿�
	if (listen(m_hServerSocket,200)==SOCKET_ERROR)
	{
		TCHAR szString[512]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("�˿�������������ռ�ã����� %d �˿�ʧ��"), m_wServicePort);
		AfxMessageBox(szString);
		return false;
	}

	//ϵͳ��Ϣ
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	//��ɶ˿�
	m_lIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,SystemInfo.dwNumberOfProcessors);
	if(m_lIOCPHandle == NULL)
	{
		LPCTSTR pszString=TEXT("����������Դʧ�ܣ���������ʧ��");
		AfxMessageBox(pszString);
		return false;
	}

	//��SOCKET
	if(NULL == CreateIoCompletionPort((HANDLE)m_hServerSocket, m_lIOCPHandle, 0, 0))
	{
		LPCTSTR pszString=TEXT("�󶨼���SOCKETʧ�ܣ���������ʧ��");
		AfxMessageBox(pszString);
		return false;
	}

	//���������߳�
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcceptWork, (LPVOID)this, 0, 0);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadWriteWork, (LPVOID)this, 0, 0);

	return true;
}

//ֹͣ����
bool CFlashAccredit::StopService()
{
	//�����¼
	POSITION pos=m_IORecvOperationItemList.GetHeadPosition();
	for (INT_PTR i=0;i<m_IORecvOperationItemList.GetCount();i++)
	{
		LPPER_IO_OPERATION_DATA pIOOperdata=m_IORecvOperationItemList.GetNext(pos);
		if(pIOOperdata->socket)
		{
			//�ر�����
			closesocket(pIOOperdata->socket);
			delete pIOOperdata;
			pIOOperdata = NULL;
		}
	}
	m_IORecvOperationItemList.RemoveAll();

	//�����¼
	pos=m_IOSendOperationItemList.GetHeadPosition();
	for (INT_PTR i=0;i<m_IOSendOperationItemList.GetCount();i++)
	{
		LPPER_IO_OPERATION_DATA pIOOperdata=m_IOSendOperationItemList.GetNext(pos);
		if(pIOOperdata->socket)
		{
			//�ر�����
			closesocket(pIOOperdata->socket);
			delete pIOOperdata;
			pIOOperdata = NULL;
		}
	}
	m_IOSendOperationItemList.RemoveAll();

	//�����¼
	pos=m_IdleIOOperationItemList.GetHeadPosition();
	for (INT_PTR i=0;i<m_IdleIOOperationItemList.GetCount();i++)
	{
		LPPER_IO_OPERATION_DATA pIOOperdata=m_IdleIOOperationItemList.GetNext(pos);
		if(pIOOperdata->socket)
		{
			//�ر�����
			closesocket(pIOOperdata->socket);
			delete pIOOperdata;
			pIOOperdata = NULL;
		}
	}
	m_IdleIOOperationItemList.RemoveAll();

	return true;
}

//���÷���
void CFlashAccredit::SetServiceParameter(WORD wServicePort)
{
	m_wServicePort=wServicePort;
}

//��ȡ����
LPPER_IO_OPERATION_DATA CFlashAccredit::GetIOOperationItem(SOCKET Socket, BYTE cbOperationType)
{
	//����Ч��
	ASSERT(cbOperationType==SEND_POSTED || cbOperationType==RECV_POSTED);
	if(cbOperationType!=SEND_POSTED && cbOperationType!=RECV_POSTED) return NULL;

	//�������
	LPPER_IO_OPERATION_DATA pIOOperdata = NULL;

	//��ȡBUFF
	if(m_IdleIOOperationItemList.GetCount() > 0)
	{
		pIOOperdata = m_IdleIOOperationItemList.RemoveHead();
	}
	else //���仺��
	{
		pIOOperdata = new PER_IO_OPERATION_DATA;
		if( pIOOperdata == NULL) return NULL;
	}

	//���û���
	pIOOperdata->socket = Socket;
	memset(&(pIOOperdata->Overlapped),0,sizeof(OVERLAPPED));
	memset(pIOOperdata->cbDataBuff,0,sizeof(pIOOperdata->cbDataBuff));
	pIOOperdata->WSABuff.len = sizeof(pIOOperdata->cbDataBuff);
	pIOOperdata->WSABuff.buf = pIOOperdata->cbDataBuff;
	pIOOperdata->cbOperationType = cbOperationType;

	//��¼����
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

//�ر�����
bool CFlashAccredit::CloseSocket(SOCKET Socket)
{
	//�ر�����
	closesocket(Socket);

	//�����¼
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

	//�����¼
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

//�����߳�
void CFlashAccredit::AcceptWork(LPVOID pParam)
{
	//��������
	CFlashAccredit * pFlashAccredit = (CFlashAccredit *)pParam;

	//�߳�ѭ��
	while(true)
	{
		//�����û����ӣ�������ɶ˿ڹ���
		SOCKET sockAccept = WSAAccept(pFlashAccredit->m_hServerSocket,NULL,NULL,NULL,0);

		//���仺��
		LPPER_IO_OPERATION_DATA pIOOperdata = pFlashAccredit->GetIOOperationItem(sockAccept, RECV_POSTED);
		if( pIOOperdata == NULL) continue;

		//����SOCKET����ɶ˿�
		CreateIoCompletionPort((HANDLE)sockAccept,pFlashAccredit->m_lIOCPHandle,NULL,1);

		//Ͷ�ݽ��ղ���
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

//��д�߳�
void CFlashAccredit::ReadWriteWork(LPVOID pParam)
{
	//��������
	CFlashAccredit * pFlashAccredit = (CFlashAccredit *)pParam;

	//�߳�ѭ��
	while(true)
	{
		//�ȴ���ɶ˿���SOCKET�����
		DWORD dwThancferred=0;
		DWORD  dwCompletionKey=0;
		LPPER_IO_OPERATION_DATA pIOOperdata=NULL;
		GetQueuedCompletionStatus(pFlashAccredit->m_lIOCPHandle,&dwThancferred,&dwCompletionKey,(LPOVERLAPPED *)&pIOOperdata,INFINITE);
		if(pIOOperdata==NULL) continue;

		//����Ƿ��д������
		if(dwThancferred == 0 && (pIOOperdata->cbOperationType == RECV_POSTED || pIOOperdata->cbOperationType == SEND_POSTED))
		{
			//�ر�SOCKET
			pFlashAccredit->CloseSocket(pIOOperdata->socket);
			continue;
		}

		//Ϊ�������
		if(pIOOperdata->cbOperationType == RECV_POSTED)
		{
			//���仺��
			LPPER_IO_OPERATION_DATA pSendIOOperdata = pFlashAccredit->GetIOOperationItem(pIOOperdata->socket, SEND_POSTED);
			if( pSendIOOperdata == NULL) continue;

			//��Ӧ�ͻ���
			CHAR buf[256]=("<?xml version=\"1.0\"?> \r\n<cross-domain-policy> \r\n<site-control permitted-cross-domain-policies=\"all\"/>\r\n<allow-access-from domain=\"*\" to-ports=\"*\" />\r\n</cross-domain-policy> ");
			CopyMemory(pSendIOOperdata->cbDataBuff,buf,sizeof(buf));

			//��������
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
			//�ر�SOCKET
			pFlashAccredit->CloseSocket(pIOOperdata->socket);
		}
	}
}

