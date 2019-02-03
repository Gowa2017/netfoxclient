#include "Stdafx.h"
#include "TCPSocketService.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//��Ϣ����
#define WM_SOCKET_NOTIFY			WM_USER+100							//��Ϣ��ʶ
#define WM_SERVICE_REQUEST			WM_USER+101							//��������

//////////////////////////////////////////////////////////////////////////
//��������

#define REQUEST_CONNECT				1									//��������
#define REQUEST_SEND_DATA			2									//������
#define REQUEST_SEND_DATA_EX		3									//������
#define REQUEST_CLOSE_SOCKET		4									//����ر�

//��������
struct tagConnectRequest
{
	WORD							wPort;								//���Ӷ˿�
	DWORD							dwServerIP;							//���ӵ�ַ
};

//��������
struct tagSendDataRequest
{
	WORD							wMainCmdID;							//��������
	WORD							wSubCmdID;							//��������
};

//��������
struct tagSendDataExRequest
{
	WORD							wMainCmdID;							//��������
	WORD							wSubCmdID;							//��������
	WORD							wDataSize;							//���ݴ�С
	BYTE							cbSendBuffer[SOCKET_TCP_PACKET];		//���ͻ���
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CTCPSocketServiceThread::CTCPSocketServiceThread()
{
	//�ں˱���
	m_hWnd=NULL;
	m_hSocket=INVALID_SOCKET;
	m_TCPSocketStatus=SOCKET_STATUS_IDLE;

	//���ձ���
	m_wRecvSize=0;
	ZeroMemory(m_cbRecvBuf,sizeof(m_cbRecvBuf));

	//�������
	m_dwBufferData=0L;
	m_dwBufferSize=0L;
	m_bNeedBuffer=false;
	m_pcbDataBuffer=NULL;

	//��������
	m_cbSendRound=0;
	m_cbRecvRound=0;
	m_dwSendXorKey=0;
	m_dwRecvXorKey=0;

	//��������
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;
	m_dwSendPacketCount=0;
	m_dwRecvPacketCount=0;

	return;
}

//��������
CTCPSocketServiceThread::~CTCPSocketServiceThread()
{
	//�ر�����
	if (m_hSocket!=INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;
	}

	//ɾ������
	SafeDeleteArray(m_pcbDataBuffer);

	return;
}

//ֹͣ�߳�
bool CTCPSocketServiceThread::ConcludeThread(DWORD dwWaitSeconds)
{
	//�˳���Ϣ
	if (IsRuning()==true)
	{
		PostThreadMessage(WM_QUIT,0,0);
	}

	return __super::ConcludeThread(dwWaitSeconds);
}

//Ͷ������
bool CTCPSocketServiceThread::PostThreadRequest(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize)
{
	//Ͷ������
	CWHDataLocker ThreadLock(m_CriticalSection);
	m_DataQueue.InsertData(wIdentifier,pBuffer,wDataSize);

	//������Ϣ
	ASSERT(m_hWnd!=NULL);
	if (m_hWnd!=NULL) PostMessage(m_hWnd,WM_SERVICE_REQUEST,wDataSize,GetCurrentThreadId());

	return true;
}

//��ʼ�¼�
bool CTCPSocketServiceThread::OnEventThreadStrat()
{
	//��������
	WNDCLASS WndClass;
	ZeroMemory(&WndClass,sizeof(WndClass));

	//���ñ���
	WndClass.lpfnWndProc=DefWindowProc;
	WndClass.hInstance=AfxGetInstanceHandle();
	WndClass.lpszClassName=TEXT("TCPSocketStatusServiceThread");

	//ע�ᴰ��
	RegisterClass(&WndClass);

	//��������
	HWND hParentWnd=GetDesktopWindow();
	m_hWnd=CreateWindow(WndClass.lpszClassName,NULL,WS_CHILD,0,0,0,0,hParentWnd,NULL,WndClass.hInstance,NULL);

	return true;
}

//ֹͣ�¼�
bool CTCPSocketServiceThread::OnEventThreadConclude()
{
	//�رմ���
	if (m_hWnd!=NULL)
	{
		DestroyWindow(m_hWnd);
		m_hWnd=NULL;
	}

	//�ر�����
	PerformCloseSocket(SHUT_REASON_INSIDE);

	//�������
	CWHDataLocker ThreadLock(m_CriticalSection);
	m_DataQueue.RemoveData(false);

	return true;
}

//���к���
bool CTCPSocketServiceThread::OnEventThreadRun()
{
	//��ȡ��Ϣ
	MSG Message;
	if (GetMessage(&Message,NULL,0,0)==FALSE) return false;

	//��Ϣ����
	switch (Message.message)
	{
	case WM_SOCKET_NOTIFY:		//������Ϣ
		{
			OnSocketNotify(Message.wParam,Message.lParam);
			return true;
		}
	case WM_SERVICE_REQUEST:	//��������
		{
			OnServiceRequest(Message.wParam,Message.lParam);
			return true;
		}
	default:					//Ĭ�ϴ���
		{ 
			DefWindowProc(Message.hwnd,Message.message,Message.wParam,Message.lParam); 
			return true;
		}
	}

	return false;
}

//ִ������
DWORD CTCPSocketServiceThread::PerformConnect(DWORD dwServerIP, WORD wPort)
{
	//Ч�����
	ASSERT(m_hSocket==INVALID_SOCKET);
	ASSERT(m_TCPSocketStatus==SOCKET_STATUS_IDLE);
	ASSERT((dwServerIP!=INADDR_NONE)&&(dwServerIP!=0));
	
	try
	{
		//Ч��״̬
		if (m_hSocket!=INVALID_SOCKET) throw CONNECT_EXCEPTION;
		if (m_TCPSocketStatus!=SOCKET_STATUS_IDLE) throw CONNECT_EXCEPTION;
		if ((dwServerIP==INADDR_NONE)||(dwServerIP==0)) throw CONNECT_EXCEPTION;

		//���ò���
		m_wRecvSize=0;
		m_cbSendRound=0;
		m_cbRecvRound=0;
		m_dwSendXorKey=0xA8C543FF;
		m_dwRecvXorKey=0xA8C543FF;
		m_dwSendTickCount=GetTickCount()/1000L;
		m_dwRecvTickCount=GetTickCount()/1000L;

		//���� SOCKET
		m_hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (m_hSocket==INVALID_SOCKET) throw CONNECT_EXCEPTION;

		//��������
		SOCKADDR_IN SocketAddr;
		ZeroMemory(&SocketAddr,sizeof(SocketAddr));

		//���ñ���
		SocketAddr.sin_family=AF_INET;
		SocketAddr.sin_port=htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr=dwServerIP;

		//�󶨴���
		WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY,FD_READ|FD_CONNECT|FD_CLOSE|FD_WRITE);

		//���ӷ�����
		INT nErrorCode=connect(m_hSocket,(SOCKADDR *)&SocketAddr,sizeof(SocketAddr));
		if ((nErrorCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSAEWOULDBLOCK)) throw CONNECT_EXCEPTION;

		//���ñ���
		m_TCPSocketStatus=SOCKET_STATUS_WAIT;

		return CONNECT_SUCCESS;
	}
	catch (...)
	{
		//�ر�����
		PerformCloseSocket(SHUT_REASON_INSIDE);
	}

	return CONNECT_EXCEPTION;
}

//���ͺ���
DWORD CTCPSocketServiceThread::PerformSendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//Ч��״̬
	if (m_hSocket==INVALID_SOCKET) return false;
	if (m_TCPSocketStatus!=SOCKET_STATUS_CONNECT) return false;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_BUFFER];
	TCP_Head * pHead=(TCP_Head *)cbDataBuffer;

	//���ñ���
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;

	//��������
	WORD wSendSize=EncryptBuffer(cbDataBuffer,sizeof(TCP_Head),sizeof(cbDataBuffer));

	//��������
	return SendBuffer(cbDataBuffer,wSendSize);
}

//���ͺ���
DWORD CTCPSocketServiceThread::PerformSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч��״̬
	if (m_hSocket==INVALID_SOCKET) return 0L;
	if (m_TCPSocketStatus!=SOCKET_STATUS_CONNECT) return 0L;

	//Ч���С
	ASSERT(wDataSize<=SOCKET_TCP_PACKET);
	if (wDataSize>SOCKET_TCP_PACKET) return 0L;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_BUFFER];
	TCP_Head * pHead=(TCP_Head *)cbDataBuffer;

	//���ñ���
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;

	//��������
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(pHead+1,pData,wDataSize);
	}

	//��������
	WORD wSendSize=EncryptBuffer(cbDataBuffer,sizeof(TCP_Head)+wDataSize,sizeof(cbDataBuffer));

	//��������
	return SendBuffer(cbDataBuffer,wSendSize);
}

//�ر�����
VOID CTCPSocketServiceThread::PerformCloseSocket(BYTE cbShutReason)
{
	//�ں˱���
	m_wRecvSize=0;
	m_dwBufferData=0L;
	m_bNeedBuffer=false;
	m_TCPSocketStatus=SOCKET_STATUS_IDLE;

	//��������
	m_cbSendRound=0;
	m_cbRecvRound=0;
	m_dwSendXorKey=0;
	m_dwRecvXorKey=0;

	//��������
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;
	m_dwSendPacketCount=0;
	m_dwRecvPacketCount=0;

	//�ر��ж�
	if (m_hSocket!=INVALID_SOCKET)
	{
		//�ر�����
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;

		//�ر�֪ͨ
		if (cbShutReason!=SHUT_REASON_INSIDE)
		{
			CTCPSocketService * pTCPSocketStatusService=CONTAINING_RECORD(this,CTCPSocketService,m_TCPSocketServiceThread);
			pTCPSocketStatusService->OnSocketShut(cbShutReason);
		}
	}

	return;
}

//��������
DWORD CTCPSocketServiceThread::SendBuffer(VOID * pBuffer, WORD wSendSize)
{
	//��������
	WORD wTotalCount=0;

	//���ñ���
	m_dwSendTickCount=GetTickCount()/1000L;

	//��������
	while ((m_bNeedBuffer==false)&&(wTotalCount<wSendSize))
	{
		//��������
		INT nSendCount=send(m_hSocket,(char *)pBuffer+wTotalCount,wSendSize-wTotalCount,0);

		//�����ж�
		if (nSendCount==SOCKET_ERROR)
		{
			//�����ж�
			if (WSAGetLastError()==WSAEWOULDBLOCK)
			{
				AmortizeBuffer((LPBYTE)pBuffer+wTotalCount,wSendSize-wTotalCount);
				return wSendSize;
			}

			//�ر�����
			PerformCloseSocket(SHUT_REASON_EXCEPTION);

			return 0L;
		}
		else
		{
			//���ñ���
			wTotalCount+=nSendCount;
		}
	}

	//��������
	if (wTotalCount>wSendSize)
	{
		AmortizeBuffer((LPBYTE)pBuffer+wTotalCount,wSendSize-wTotalCount);
	}

	return wSendSize;
}

//��������
VOID CTCPSocketServiceThread::AmortizeBuffer(VOID * pData, WORD wDataSize)
{
	//���뻺��
	if ((m_dwBufferData+wDataSize)>m_dwBufferSize)
	{
		//��������
		LPBYTE pcbDataBuffer=NULL;
		LPBYTE pcbDeleteBuffer=m_pcbDataBuffer;

		//�����С
		DWORD dwNeedSize=m_dwBufferData+wDataSize;
		DWORD dwApplySize=__max(dwNeedSize,m_dwBufferSize*2L);

		//���뻺��
		try
		{
			pcbDataBuffer=new BYTE [dwApplySize];
		}
		catch (...)	{ }

		//ʧ���ж�
		if (pcbDataBuffer==NULL)
		{
			PerformCloseSocket(SHUT_REASON_EXCEPTION);
			return;
		}

		//���ñ���
		m_dwBufferSize=dwApplySize;
		m_pcbDataBuffer=pcbDataBuffer;
		CopyMemory(m_pcbDataBuffer,pcbDeleteBuffer,m_dwBufferData);

		//ɾ������
		SafeDeleteArray(pcbDeleteBuffer);
	}

	//���ñ���
	m_bNeedBuffer=true;
	m_dwBufferData+=wDataSize;
	CopyMemory(m_pcbDataBuffer+m_dwBufferData-wDataSize,pData,wDataSize);

	return;
}

//���ӳ��
WORD CTCPSocketServiceThread::SeedRandMap(WORD wSeed)
{
  DWORD dwHold = wSeed;
  return (WORD)((dwHold = dwHold * 241103L + 2533101L) >> 16);
}

//ӳ�䷢������
BYTE CTCPSocketServiceThread::MapSendByte(BYTE const cbData)
{
	BYTE cbMap;
	cbMap = g_SendByteMap[cbData];
	return cbMap;
}

//ӳ���������
BYTE CTCPSocketServiceThread::MapRecvByte(BYTE const cbData)
{
	BYTE cbMap;
	cbMap = g_RecvByteMap[cbData];
	return cbMap;
}

//��������
WORD CTCPSocketServiceThread::CrevasseBuffer(BYTE cbDataBuffer[], WORD wDataSize)
{
	//Ч�����
	//ASSERT(m_dwSendPacketCount > 0);
	ASSERT(wDataSize >= sizeof(TCP_Head));
	ASSERT(((TCP_Head *)cbDataBuffer)->TCPInfo.wPacketSize == wDataSize);
	
	//Ч�������ֽ�ӳ��
	TCP_Head * pHead = (TCP_Head *)cbDataBuffer;
	for (int i = sizeof(TCP_Info); i < wDataSize; i++)
	{
		cbDataBuffer[i] = MapRecvByte(cbDataBuffer[i]);
	}

	return wDataSize;
}

//��������
WORD CTCPSocketServiceThread::EncryptBuffer(BYTE cbDataBuffer[], WORD wDataSize, WORD wBufferSize)
{
	int i = 0;
	//Ч�����
	ASSERT(wDataSize >= sizeof(TCP_Head));
	ASSERT(wBufferSize >= (wDataSize + 2*sizeof(DWORD)));
	ASSERT(wDataSize <= (sizeof(TCP_Head) + SOCKET_TCP_BUFFER));

	//��д��Ϣͷ
	TCP_Head * pHead = (TCP_Head *)cbDataBuffer;
	pHead->TCPInfo.wPacketSize = wDataSize;
	pHead->TCPInfo.cbDataKind = DK_MAPPED;
	
	
	BYTE checkCode = 0;

	for (WORD i = sizeof(TCP_Info); i < wDataSize; i++)
	{
		checkCode += cbDataBuffer[i];
		cbDataBuffer[i] = MapSendByte(cbDataBuffer[i]);
	}
	pHead->TCPInfo.cbCheckCode = ~checkCode+1;

	//���ñ���
	m_dwSendPacketCount++;

	return wDataSize;
}

//������Ϣ
LRESULT CTCPSocketServiceThread::OnServiceRequest(WPARAM wParam, LPARAM lParam)
{
	//��������
	tagDataHead DataHead;
	CWHDataLocker ThreadLock(m_CriticalSection);

	//��ȡ����
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];
	if (m_DataQueue.DistillData(DataHead,cbBuffer,sizeof(cbBuffer))==false) return 0;

	//���ݴ���
	switch (DataHead.wIdentifier)
	{
	case REQUEST_CONNECT:		//��������
		{
			//Ч������
			ASSERT(DataHead.wDataSize==sizeof(tagConnectRequest));
			tagConnectRequest * pConnectRequest=(tagConnectRequest *)cbBuffer;

			//���ݴ���
			if (PerformConnect(pConnectRequest->dwServerIP,pConnectRequest->wPort)!=CONNECT_SUCCESS)
			{
				//�¼�֪ͨ
				CTCPSocketService * pTCPSocketStatusService=CONTAINING_RECORD(this,CTCPSocketService,m_TCPSocketServiceThread);
				pTCPSocketStatusService->OnSocketLink(WSAEADDRNOTAVAIL);
			}

			return 1;
		}
	case REQUEST_SEND_DATA:		//��������
		{
			//Ч������
			ASSERT(DataHead.wDataSize==sizeof(tagSendDataRequest));
			tagSendDataRequest * pSendDataRequest=(tagSendDataRequest *)cbBuffer;

			//���ݴ���
			PerformSendData(pSendDataRequest->wMainCmdID,pSendDataRequest->wSubCmdID);

			return 1;
		}
	case REQUEST_SEND_DATA_EX:	//��������
		{
			//Ч������
			tagSendDataExRequest * pSendDataExRequest=(tagSendDataExRequest *)cbBuffer;
			ASSERT(DataHead.wDataSize>=(sizeof(tagSendDataExRequest)-sizeof(pSendDataExRequest->cbSendBuffer)));
			ASSERT(DataHead.wDataSize==(pSendDataExRequest->wDataSize+sizeof(tagSendDataExRequest)-sizeof(pSendDataExRequest->cbSendBuffer)));

			//���ݴ���
			PerformSendData(pSendDataExRequest->wMainCmdID,pSendDataExRequest->wSubCmdID,pSendDataExRequest->cbSendBuffer,pSendDataExRequest->wDataSize);

			return 1;
		}
	case REQUEST_CLOSE_SOCKET:	//�ر�����
		{
			//�ر�����
			PerformCloseSocket(true);

			return 1;
		}
	}

	return 0;
}

//������Ϣ
LRESULT CTCPSocketServiceThread::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:		//���ݶ�ȡ
		{ 
			return OnSocketNotifyRead(wParam,lParam);
		}
	case FD_WRITE:		//���緢��
		{
			return OnSocketNotifyWrite(wParam,lParam);
		}
	case FD_CLOSE:		//����ر�
		{ 
			return OnSocketNotifyClose(wParam,lParam);	
		}
	case FD_CONNECT:	//��������
		{ 
			return OnSocketNotifyConnect(wParam,lParam); 
		}
	}

	return 0;
}

//�����ȡ
LRESULT CTCPSocketServiceThread::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	try
	{
		//��ȡ����
		INT iRetCode=recv(m_hSocket,(char *)m_cbRecvBuf+m_wRecvSize,sizeof(m_cbRecvBuf)-m_wRecvSize,0);
		if (iRetCode==SOCKET_ERROR) throw TEXT("�������ӹرգ���ȡ����ʧ��");
		ASSERT(m_dwSendPacketCount>0);
		m_wRecvSize+=iRetCode;
		m_dwRecvTickCount=GetTickCount()/1000L;

		//��������
		WORD wPacketSize=0;
		BYTE cbDataBuffer[SOCKET_TCP_PACKET+sizeof(TCP_Head)];
		TCP_Head * pHead=(TCP_Head *)m_cbRecvBuf;

		while (m_wRecvSize>=sizeof(TCP_Head))
		{
			//Ч�����
			wPacketSize=pHead->TCPInfo.wPacketSize;
			ASSERT(pHead->TCPInfo.cbDataKind==DK_MAPPED);
			ASSERT(wPacketSize<=(SOCKET_TCP_PACKET+sizeof(TCP_Head)));
			if (pHead->TCPInfo.cbDataKind!=DK_MAPPED) throw TEXT("���ݰ��汾����");
			if (wPacketSize>(SOCKET_TCP_PACKET+sizeof(TCP_Head))) throw TEXT("���ݰ�̫��");
			if (m_wRecvSize<wPacketSize) return 1;

			//��������
			m_dwRecvPacketCount++;
			CopyMemory(cbDataBuffer,m_cbRecvBuf,wPacketSize);
			m_wRecvSize-=wPacketSize;
			MoveMemory(m_cbRecvBuf,m_cbRecvBuf+wPacketSize,m_wRecvSize);

			//��������
			WORD wRealySize=CrevasseBuffer(cbDataBuffer,wPacketSize);
			ASSERT(wRealySize>=sizeof(TCP_Head));

			//��������
			WORD wDataSize=wRealySize-sizeof(TCP_Head);
			VOID * pDataBuffer=cbDataBuffer+sizeof(TCP_Head);
			TCP_Command Command=((TCP_Head *)cbDataBuffer)->CommandInfo;

			//�ں�����
			if (Command.wMainCmdID==MDM_KN_COMMAND)
			{
				switch (Command.wSubCmdID)
				{
				case SUB_KN_DETECT_SOCKET:		//������
					{
						//��Ӧ����
						PerformSendData(MDM_KN_COMMAND,SUB_KN_DETECT_SOCKET);

						break;
					}
				//case SUB_KN_SHUT_DOWN_SOCKET:	//�ж�����
				//	{
				//		//�ж�����
				//		PerformCloseSocket(true); 

				//		break;
				//	}
				}
			}
			else
			{
				//��������
				CTCPSocketService * pTCPSocketStatusService=CONTAINING_RECORD(this,CTCPSocketService,m_TCPSocketServiceThread);
				if (pTCPSocketStatusService->OnSocketRead(Command,pDataBuffer,wDataSize)==false) throw TEXT("�������ݰ�����ʧ��");
			}
		};
	}
	catch (...) 
	{
		//�ر�����
		PerformCloseSocket(true); 
	}

	return 1;
}

//���緢��
LRESULT CTCPSocketServiceThread::OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam)
{
	//�����ж�
	if ((m_bNeedBuffer==true)&&(m_dwBufferData>0L))
	{
		//��������
		DWORD dwTotalCount=0;
		DWORD dwPacketSize=4096;

		//���ñ���
		m_dwSendTickCount=GetTickCount()/1000L;

		//��������
		while (dwTotalCount<m_dwBufferData)
		{
			//��������
			WORD wSendSize=(WORD)__min(dwPacketSize,m_dwBufferData-dwTotalCount);
			INT nSendCount=send(m_hSocket,(char *)m_pcbDataBuffer+dwTotalCount,wSendSize,0);

			//�����ж�
			if (nSendCount==SOCKET_ERROR)
			{
				//�����ж�
				if (WSAGetLastError()==WSAEWOULDBLOCK)
				{
					//���ñ���
					m_bNeedBuffer=false;
					m_dwBufferData-=dwTotalCount;

					//�ƶ��ڴ�
					if (m_dwBufferData>0L)
					{
						m_bNeedBuffer=true;
						MoveMemory(m_pcbDataBuffer,m_pcbDataBuffer+dwTotalCount,m_dwBufferData);
					}

					return 1L;
				}

				//�ر�����
				PerformCloseSocket(SHUT_REASON_EXCEPTION);

				return 1L;
			}

			//���ñ���
			dwTotalCount+=nSendCount;
		}

		//���ñ���
		m_dwBufferData=0L;
		m_bNeedBuffer=false;
	}

	return 1L;
}

//����ر�
LRESULT CTCPSocketServiceThread::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	//�ر�����
	PerformCloseSocket(true);

	return 1;
}

//��������
LRESULT CTCPSocketServiceThread::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����
	INT nErrorCode=WSAGETSELECTERROR(lParam);

	//�¼�֪ͨ
	CTCPSocketService * pTCPSocketStatusService=CONTAINING_RECORD(this,CTCPSocketService,m_TCPSocketServiceThread);
	pTCPSocketStatusService->OnSocketLink(nErrorCode);

	//�ر��ж�
	if (nErrorCode!=0)
	{
		PerformCloseSocket(SHUT_REASON_INSIDE);
		return 0;
	}

	//����״̬
	m_TCPSocketStatus=SOCKET_STATUS_CONNECT;

	return 1;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CTCPSocketService::CTCPSocketService()
{
	//�ں˱���
	m_wServiceID=0;
	m_bService=false;
	m_pITCPSocketEvent=NULL;

	return;
}

//��������
CTCPSocketService::~CTCPSocketService()
{
	//ֹͣ����
	ConcludeService();

	return;
}

//�ӿڲ�ѯ
VOID * CTCPSocketService::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPSocketService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocketService,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CTCPSocketService::StartService()
{
	//�����ж�
	ASSERT((m_bService==false)&&(m_pITCPSocketEvent!=NULL));
	if ((m_bService==true)||(m_pITCPSocketEvent==NULL)) return false;

	//�����߳�
	if (m_TCPSocketServiceThread.StartThread()==false) return false;

	//���ñ���
	m_bService=true;

	return true;
}

//ֹͣ����
bool CTCPSocketService::ConcludeService()
{
	//���ñ���
	m_bService=false;

	//ֹͣ�߳�
	m_TCPSocketServiceThread.ConcludeThread(INFINITE);

	return true;
}

//���ú���
bool CTCPSocketService::SetServiceID(WORD wServiceID)
{
	//״̬Ч��
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//���ñ���
	m_wServiceID=wServiceID;

	return true;
}

//���ýӿ�
bool CTCPSocketService::SetTCPSocketEvent(IUnknownEx * pIUnknownEx)
{
	//״̬Ч��
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//��ѯ�ӿ�
	m_pITCPSocketEvent=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPSocketEvent);

	//�����ж�
	if (m_pITCPSocketEvent==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

    return true;
}

//�ر�����
bool CTCPSocketService::CloseSocket()
{
	//״̬Ч��
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//Ͷ������
	return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_CLOSE_SOCKET,NULL,0);
}

//���ӷ�����
bool CTCPSocketService::Connect(DWORD dwServerIP, WORD wPort)
{
	//״̬Ч��
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//��������
	tagConnectRequest ConnectRequest;
	ZeroMemory(&ConnectRequest,sizeof(ConnectRequest));

	//���ñ���
	ConnectRequest.wPort=wPort;
	ConnectRequest.dwServerIP=htonl(dwServerIP);

	//Ͷ������
	return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_CONNECT,&ConnectRequest,sizeof(ConnectRequest));
}

//���ӵ�ַ
bool CTCPSocketService::Connect(LPCTSTR szServerIP, WORD wPort)
{
	//״̬Ч��
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//��������
	tagConnectRequest ConnectRequest;
	ZeroMemory(&ConnectRequest,sizeof(ConnectRequest));

	//���ñ���
	ConnectRequest.wPort=wPort;
	ConnectRequest.dwServerIP=TranslateAddress(szServerIP);

	//Ͷ������
	return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_CONNECT,&ConnectRequest,sizeof(ConnectRequest));
}

//���ͺ���
bool CTCPSocketService::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//״̬Ч��
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//��������
	tagSendDataRequest SendDataRequest;
	ZeroMemory(&SendDataRequest,sizeof(SendDataRequest));

	//���ñ���
	SendDataRequest.wSubCmdID=wSubCmdID;
	SendDataRequest.wMainCmdID=wMainCmdID;

	//Ͷ������
	return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_SEND_DATA,&SendDataRequest,sizeof(SendDataRequest));
}

//���ͺ���
bool CTCPSocketService::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT(m_bService==true);
	if (m_bService==false) return false;

	//��������
	tagSendDataExRequest SendRequestEx;
	ZeroMemory(&SendRequestEx,sizeof(SendRequestEx));

	//���ñ���
	SendRequestEx.wDataSize=wDataSize;
	SendRequestEx.wSubCmdID=wSubCmdID;
	SendRequestEx.wMainCmdID=wMainCmdID;

	//��������
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(SendRequestEx.cbSendBuffer,pData,wDataSize);
	}

	//Ͷ������
	WORD wSendSize=sizeof(SendRequestEx)-sizeof(SendRequestEx.cbSendBuffer)+wDataSize;
	return m_TCPSocketServiceThread.PostThreadRequest(REQUEST_SEND_DATA_EX,&SendRequestEx,wSendSize);
}

//������Ϣ
bool CTCPSocketService::OnSocketLink(INT nErrorCode)
{
	//Ͷ���¼�
	ASSERT(m_pITCPSocketEvent!=NULL);
	return m_pITCPSocketEvent->OnEventTCPSocketLink(m_wServiceID,nErrorCode);
}

//�ر���Ϣ
bool CTCPSocketService::OnSocketShut(BYTE cbShutReason)
{
	//Ͷ���¼�
	ASSERT(m_pITCPSocketEvent!=NULL);
	return m_pITCPSocketEvent->OnEventTCPSocketShut(m_wServiceID,cbShutReason);
}

//��ȡ��Ϣ
bool CTCPSocketService::OnSocketRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Ͷ���¼�
	ASSERT(m_pITCPSocketEvent!=NULL);
	return m_pITCPSocketEvent->OnEventTCPSocketRead(m_wServiceID,Command,pData,wDataSize);
}

//��ַ����
DWORD CTCPSocketService::TranslateAddress(LPCTSTR szServerIP)
{
	//ת����ַ
	CT2CA ServerAddr(szServerIP);
	DWORD dwServerIP=inet_addr(ServerAddr);

	//��������
	if (dwServerIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(ServerAddr);
		if (lpHost==NULL) return INADDR_NONE;
		dwServerIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}

	return dwServerIP;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(TCPSocketService);

//////////////////////////////////////////////////////////////////////////
