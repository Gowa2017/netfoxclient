#ifndef TCP_SOCKET_SERVICE_HEAD_FILE
#define TCP_SOCKET_SERVICE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////

//�����߳�
class CTCPSocketServiceThread : public CWHThread
{
	//�ں˱���
protected:
	HWND							m_hWnd;								//���ھ��
	SOCKET							m_hSocket;							//���Ӿ��
	BYTE				            m_TCPSocketStatus;					//����״̬

	//�������
protected:
	CWHDataQueue					m_DataQueue;						//�������
	CCriticalSection				m_CriticalSection;					//��������

	//���ձ���
protected:
	WORD							m_wRecvSize;						//���ճ���
	BYTE							m_cbRecvBuf[SOCKET_TCP_BUFFER*10];		//���ջ���

	//�������
protected:
	bool							m_bNeedBuffer;						//����״̬
	DWORD							m_dwBufferData;						//��������
	DWORD							m_dwBufferSize;						//�����С
	LPBYTE							m_pcbDataBuffer;					//��������

	//��������
protected:
	BYTE							m_cbSendRound;						//�ֽ�ӳ��
	BYTE							m_cbRecvRound;						//�ֽ�ӳ��
	DWORD							m_dwSendXorKey;						//������Կ
	DWORD							m_dwRecvXorKey;						//������Կ

	//��������
protected:
	DWORD							m_dwSendTickCount;					//����ʱ��
	DWORD							m_dwRecvTickCount;					//����ʱ��
	DWORD							m_dwSendPacketCount;				//���ͼ���
	DWORD							m_dwRecvPacketCount;				//���ܼ���

	//��������
public:
	//���캯��
	CTCPSocketServiceThread();
	//��������
	virtual ~CTCPSocketServiceThread();

	//���غ���
public:
	//���к���
	virtual bool OnEventThreadRun();
	//��ʼ�¼�
	virtual bool OnEventThreadStrat();
	//ֹͣ�¼�
	virtual bool OnEventThreadConclude();
	//ֹͣ�߳�
	virtual bool ConcludeThread(DWORD dwWaitSeconds);

	//������
public:
	//Ͷ������
	bool PostThreadRequest(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize);

	//���ƺ���
private:
	//�ر�����
	VOID PerformCloseSocket(BYTE cbShutReason);
	//���ӷ�����
	DWORD PerformConnect(DWORD dwServerIP, WORD wPort);
	//���ͺ���
	DWORD PerformSendData(WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	DWORD PerformSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//��������
private:
	//��������
	DWORD SendBuffer(VOID * pBuffer, WORD wSendSize);
	//��������
	VOID AmortizeBuffer(VOID * pData, WORD wDataSize);
	//��������
	WORD CrevasseBuffer(BYTE cbDataBuffer[], WORD wDataSize);
	//��������
	WORD EncryptBuffer(BYTE cbDataBuffer[], WORD wDataSize, WORD wBufferSize);

	//������
private:
	//������Ϣ
	LRESULT OnSocketNotify(WPARAM wParam, LPARAM lParam);
	//������Ϣ
	LRESULT OnServiceRequest(WPARAM wParam, LPARAM lParam);

	//��������
private:
	//�����ȡ
	LRESULT OnSocketNotifyRead(WPARAM wParam, LPARAM lParam);
	//���緢��
	LRESULT OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam);
	//����ر�
	LRESULT OnSocketNotifyClose(WPARAM wParam, LPARAM lParam);
	//��������
	LRESULT OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam);

	//��������
private:
	//���ӳ��
	inline WORD SeedRandMap(WORD wSeed);
	//����ӳ��
	inline BYTE MapSendByte(BYTE cbData);
	//����ӳ��
	inline BYTE MapRecvByte(BYTE cbData);
};

//////////////////////////////////////////////////////////////////////////

//�������
class CTCPSocketService : public ITCPSocketService
{
	//��Ԫ˵��
	friend class CTCPSocketServiceThread;

	//�ں˱���
protected:
	bool							m_bService;							//�����־
	WORD							m_wServiceID;						//�����ʶ
	ITCPSocketEvent	*				m_pITCPSocketEvent;					//�¼��ӿ�

	//�������
protected:
	CTCPSocketServiceThread			m_TCPSocketServiceThread;			//�����߳�

	//��������
public:
	//���캯��
	CTCPSocketService();
	//��������
	virtual ~CTCPSocketService();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();

	//���ýӿ�
public:
	//���ú���
	virtual bool SetServiceID(WORD wServiceID);
	//���ýӿ�
	virtual bool SetTCPSocketEvent(IUnknownEx * pIUnknownEx);

	//���ܽӿ�
public:
	//�ر�����
	virtual bool CloseSocket();
	//���ӵ�ַ
	virtual bool Connect(DWORD dwServerIP, WORD wPort);
	//���ӵ�ַ
	virtual bool Connect(LPCTSTR szServerIP, WORD wPort);
	//���ͺ���
	virtual bool SendData(WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	virtual bool SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//��������
protected:
	//������Ϣ
	bool OnSocketLink(INT nErrorCode);
	//�ر���Ϣ
	bool OnSocketShut(BYTE cbShutReason);
	//��ȡ��Ϣ
	bool OnSocketRead(TCP_Command Command, VOID * pData, WORD wDataSize);

	//�ڲ�����
private:
	//��ַ����
	DWORD TranslateAddress(LPCTSTR szServerIP);
};

//////////////////////////////////////////////////////////////////////////

#endif