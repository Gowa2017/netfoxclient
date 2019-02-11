#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////

#define IDI_OUT_CARD			101								//���ƶ�ʱ
#define IDI_OPERATE				102								//������ʱ

#define TIME_LESS				5								//����ʱ��
#define TIME_OUT_CARD			5								//����ʱ��
#define TIME_OPERATE			5								//����ʱ��
//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
  
  //�ӿڱ���
  m_pIAndroidUserItem=NULL;
  srand((unsigned)time(NULL));



  return;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
  return NULL;
}

//��ʼ�ӿ�
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
  //��ѯ�ӿ�
  m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
  if(m_pIAndroidUserItem==NULL)
  {
    return false;
  }


  //�������
  //UINT nElapse=TIME_CHECK_BANKER+rand()%100;
  //m_pIAndroidUserItem->SetGameTimer(IDI_CHECK_BANKER_OPERATE,nElapse);

  return true;
}

//���ýӿ�
bool CAndroidUserItemSink::RepositionSink()
{
  

  return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
  try
  {
    switch(nTimerID)
    {
		case IDI_OUT_CARD:
		{
			CMD_C_OutCard OutCard;
			OutCard.cbCardData = m_cbHandCardData[MAX_COUNT - 1];

			//������Ϣ
			m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD, &OutCard, sizeof(OutCard));
			return true;
		}

    }

  }
  catch(...)
  {
    CString cs;
    cs.Format(TEXT("�쳣ID=%d"),nTimerID);
    CTraceService::TraceString(cs,TraceLevel_Debug);
  }

  return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
  switch(wSubCmdID)
  {
    case SUB_S_GAME_START:  //��Ϸ��ʼ
    {
		//��Ϣ����
		return OnSubGameStart(pData,wDataSize);
    }
	case SUB_S_OUT_CARD:	//��ҳ���
	{
		//��Ϣ����
		return OnSubOutCard(pData,wDataSize);
	}
    case SUB_S_SEND_CARD: //������Ϣ
    {
		//��Ϣ����
		return OnSubSendCard(pData,wDataSize);
    }
	case SUB_S_OPERATE_NOTIFY:
	{
		//��Ϣ����
		return OnSubOperateNotify(pData,wDataSize);
	}
  }

  return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
  return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
  switch(cbGameStatus)
  {
    case GAME_STATUS_FREE:    //����״̬
    {
      //Ч������
      if(wDataSize!=sizeof(CMD_S_StatusFree))
      {
        return false;
      }

      //��Ϣ����
      CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;


      //ReadConfigInformation(&(pStatusFree->RobotConfig));
	  m_pIAndroidUserItem->SendUserReady(NULL,0);

      return true;
    }
  }

  ASSERT(FALSE);

  return false;
}

//�û�����
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//�û��뿪
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//�û�����
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//�û�״̬
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//�û���λ
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const VOID * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_GameStart));
	if (wDataSize != sizeof(CMD_S_GameStart)) return false;

	//��Ϣ����
	CMD_S_GameStart * pGameStart = (CMD_S_GameStart *)pBuffer;

	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_PLAY);

	CopyMemory(m_cbHandCardData, pGameStart->cbCardData, sizeof(m_cbHandCardData));
	m_cbHandCardCount = MAX_COUNT-1;

	if (m_pIAndroidUserItem->GetChairID() == pGameStart->wBankerUser)
	{
		m_cbHandCardCount = MAX_COUNT;
		int nElapse = (rand() % TIME_OUT_CARD) + TIME_LESS;

		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD, nElapse);
	}

	return true;
}

//��ҳ���
bool CAndroidUserItemSink::OnSubOutCard(const VOID * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_OutCard));
	if (wDataSize != sizeof(CMD_S_OutCard)) return false;

	//��Ϣ����
	CMD_S_OutCard * pOutCard = (CMD_S_OutCard *)pBuffer;




	if (m_pIAndroidUserItem->GetChairID() == pOutCard->wOutCardUser)
	{
		BYTE cbTempCard[MAX_COUNT];
		CopyMemory(cbTempCard,m_cbHandCardData,sizeof(cbTempCard));

		for (int i = 0; i < MAX_COUNT; i++)
		{
			if (cbTempCard[i] == pOutCard->cbOutCardData)
			{
				cbTempCard[i] = 0;
				break;
			}
		}

		ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

		int restcount = 0;
		for (int i = 0; i < MAX_COUNT; i++)
		{
			if (cbTempCard[i] != 0)
			{
				m_cbHandCardData[restcount] = cbTempCard[i];
				restcount++;
			}
		}
		m_cbHandCardCount = restcount;
	}

	return true;
}

//������
bool CAndroidUserItemSink::OnSubSendCard(const VOID * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_SendCard));
	if (wDataSize != sizeof(CMD_S_SendCard)) return false;

	//��Ϣ����
	CMD_S_SendCard * pSendCard = (CMD_S_SendCard *)pBuffer;

	m_cbHandCardData[m_cbHandCardCount] = pSendCard->cbCardData;
	m_cbHandCardCount++;

	if (m_pIAndroidUserItem->GetChairID() == pSendCard->wCurrentUser)
	{
		int nElapse = (rand() % TIME_OUT_CARD) + TIME_LESS;

		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD, nElapse);
	}

	return true;
}
//������ʾ
bool CAndroidUserItemSink::OnSubOperateNotify(const VOID * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_OperateNotify));
	if (wDataSize != sizeof(CMD_S_OperateNotify)) return false;

	//��Ϣ����
	CMD_S_OperateNotify * pOperateNotify = (CMD_S_OperateNotify *)pBuffer;


	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCard[0] = 0;
	OperateCard.cbOperateCard[1] = 0;
	OperateCard.cbOperateCard[2] = 0;

	if ( (pOperateNotify->cbActionMask & WIK_CHI_HU) != 0)
		OperateCard.cbOperateCode = WIK_CHI_HU;
	else
		OperateCard.cbOperateCode = WIK_NULL;

	//������Ϣ
	m_pIAndroidUserItem->SendSocketData(SUB_C_OPERATE_CARD, &OperateCard, sizeof(OperateCard));


	return true;
}

//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
