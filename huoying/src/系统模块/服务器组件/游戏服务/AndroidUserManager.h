#ifndef ANDROID_USER_MANAGER_HEAD_FILE
#define ANDROID_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "AndroidUserItem.h"
#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////////
//������չ
struct tagAndroidParameterEx
{
	//��������
	tagAndroidParameter					AndroidParameter;					//��������

	//��ʶ����
	bool								bIsValided;							//��Ч��ʶ
	bool								bIsLoadAndroid;						//���ػ���	
};

///////////////////////////////////////////////////////////////////////////////////////

//��������
typedef CWHArray<CAndroidUserItem *> CAndroidUserItemArray;
typedef CWHArray<tagAndroidParameterEx *> CAndroidParameterArray;
typedef CMap<DWORD,DWORD,CAndroidUserItem *, CAndroidUserItem *> CAndroidUserItemMap;
//typedef CMap<DWORD,DWORD,tagAndroidParameter *, tagAndroidParameter *> CAndroidParameterMap;

//////////////////////////////////////////////////////////////////////////////////

//�����˹���
class GAME_SERVICE_CLASS CAndroidUserManager : public IAndroidUserManager
{
	//���ñ���
protected:
	WORD							m_wAutoAndroidCount;				//�Զ���Ŀ

	//������
protected:
	WORD							m_wStockCount;						//�����Ŀ
	tagAndroidItemConfig			m_AndroidItemConfig[MAX_ANDROID];	//��������

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IServerUserManager *			m_pIServerUserManager;				//�û�����
	IGameServiceManager *			m_pIGameServiceManager;				//�������
	IGameServiceCustomTime *		m_pIGameServiceSustomTime;			//ʱ������
	ITCPNetworkEngineEvent *		m_pITCPNetworkEngineEvent;			//�¼��ӿ�
	IMainServiceFrame *				m_pIMainServiceFrame;				//���ȷ���

	//��������
protected:	
	bool							m_bServiceContinue;					//��������
	DWORD							m_dwMinSitInterval;					//��Ϣʱ��
	DWORD							m_dwMaxSitInterval;					//��Ϣʱ��	

	//ϵͳ����
protected:
	tagGameParameter *				m_pGameParameter;					//���ò���
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������
	tagGameMatchOption *			m_pGameMatchOption;					//��������

	//�û�����
protected:
	CAndroidUserItemMap				m_AndroidUserItemMap;				//�û�����
	CAndroidParameterArray			m_AndroidParameterArray;			//��������
	CAndroidUserItemArray			m_AndroidUserItemArray;				//�洢����
	CAndroidUserItemArray			m_AndroidUserItemBuffer;			//���ж���

	//��������
public:
	//���캯��
	CAndroidUserManager();
	//��������
	virtual ~CAndroidUserManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();

	//���ýӿ�
public:	
	//�������
	virtual bool InitAndroidUser(tagAndroidUserParameter & AndroidUserParameter);
	//�Ƴ�����
	virtual bool RemoveAndroidParameter(DWORD dwBatchID);
	//���ò���
	virtual bool AddAndroidParameter(tagAndroidParameter AndroidParameter[], WORD wParameterCount);
	//�������
	virtual bool InsertAndroidInfo(tagAndroidAccountsInfo AndroidAccountsInfo[],WORD wAndroidCount,DWORD dwBatchID);

	//����ӿ�
public:
	//ɾ������
	virtual bool DeleteAndroidUserItem(DWORD dwAndroidID,bool bStockRetrieve);
	//���һ���
	virtual IAndroidUserItem * SearchAndroidUserItem(DWORD dwUserID, DWORD dwContextID);
	//��������
	virtual IAndroidUserItem * CreateAndroidUserItem(tagAndroidItemConfig & AndroidItemConfig);
	//���ýӿ�
	VOID SetMainServiceFrame(IMainServiceFrame *pIMainServiceFrame);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);

	//״̬�ӿ�
public:
	//������Ŀ
	virtual WORD GetAndroidCount() { return (WORD)m_AndroidUserItemMap.GetCount(); }
	//�����Ŀ
	virtual WORD GetAndroidStock() { return m_wStockCount; }
	//���ػ���
	virtual bool GetAndroidLoadInfo(DWORD & dwBatchID,DWORD & dwLoadCount);
	//�û�״��
	virtual WORD GetAndroidUserInfo(tagAndroidUserInfo & AndroidUserInfo, DWORD dwServiceMode);
	//��ȡ��������
	virtual tagGameServiceOption* GetGameServiceOption() { return m_pGameServiceOption; }
	//��ȡ��Ϸ����
	virtual tagGameServiceAttrib* GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//��ȡ��������
	virtual tagGameMatchOption* GetGameMatchOption() { return m_pGameMatchOption; }

	//����ӿ�
public:
	//��������
	virtual bool SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendDataToClient(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendDataToServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�������
protected:
	//�ͷŶ���
	VOID FreeAndroidUserItem(DWORD dwUserID);	
	//��ȡ����
	CAndroidUserItem * GetAndroidUserItem(WORD wIndex);
	//�������
	CAndroidUserItem * ActiveAndroidUserItem(tagAndroidItemConfig & AndroidItemConfig);
	//ǿ������
	VOID ForceAndroidUserItemStandup(CAndroidUserItem * pAndroidUserItem);

public:
	//��ȡ����
	tagAndroidParameterEx * GetAndroidParameter(DWORD dwBatchID);
	//��������
protected:	 
	//��ȡ����
	WORD GetAndroidParameterIndex(tagAndroidParameterEx * pAndroidParameter);
	//ת��ʱ��
	DWORD GetTodayTickCount(SYSTEMTIME & SystemTime);
	//����ʱ��
	DWORD BatchServiceRemaindTime(tagAndroidParameter * pAndroidParameter,DWORD dwTodayTickCount);
	//�ȴ�ʱ��
	DWORD BatchServiceAwaitTime(tagAndroidParameter * pAndroidParameter,DWORD dwTodayTickCount);
};

//////////////////////////////////////////////////////////////////////////////////

#endif