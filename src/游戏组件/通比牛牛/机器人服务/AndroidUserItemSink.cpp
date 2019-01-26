#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////

//����ʱ��
#define TIME_LESS           2                 //����ʱ��

//��Ϸʱ��
#define TIME_USER_CALL_BANKER     3                 //��ׯʱ��
#define TIME_USER_START_GAME      5                 //��ʼʱ��
#define TIME_USER_ADD_SCORE       3                 //��עʱ��
#define TIME_USER_OPEN_CARD       3                 //̯��ʱ��

#define TIME_CHECK_BANKER       30                  //̯��ʱ��

//��Ϸʱ��
#define IDI_START_GAME          (100)                 //��ʼ��ʱ��
#define IDI_CALL_BANKER         (101)                 //��ׯ��ʱ��
#define IDI_USER_ADD_SCORE        (102)                 //��ע��ʱ��
#define IDI_OPEN_CARD         (103)                 //���ƶ�ʱ��
#define IDI_DELAY_TIME          (105)                 //��ʱ��ʱ��



//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
  m_lTurnMaxScore = 0;
  ZeroMemory(m_HandCardData,sizeof(m_HandCardData));

  m_nRobotBankStorageMul=0;
  m_lRobotBankGetScore=0;
  m_lRobotBankGetScoreBanker=0;
  ZeroMemory(m_lRobotScoreRange,sizeof(m_lRobotScoreRange));

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
  m_lTurnMaxScore = 0;
  ZeroMemory(m_HandCardData,sizeof(m_HandCardData));

  //�������
  //UINT nElapse=TIME_CHECK_BANKER+rand()%100;
  //m_pIAndroidUserItem->SetGameTimer(IDI_CHECK_BANKER_OPERATE,nElapse);

  return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
  try
  {
    switch(nTimerID)
    {
      case IDI_DELAY_TIME:
      {
        //��ʼʱ��
        UINT nElapse= TIME_LESS + (rand() % TIME_USER_START_GAME);
        m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

        return true;
      }
      case IDI_START_GAME:    //��ʼ��ʱ��
      {
		  IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
		  if (pUserItem)
		  {
			  //��������
			  SCORE lRobotScore = pUserItem->GetUserScore();
			  //�жϴ�ȡ
			  if ((lRobotScore >m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)||(lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore))
			  {		
				  //���ͻ������˳�
				  m_pIAndroidUserItem->SendAndroidQuit(NULL, 0);
				  return true;
			  }
		  }
		  		  		//�뿪�ж�
		if(m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITLEAVE))
		{
			//��������
			//PerformStandUpAction(pIServerUserItem);
        //������Ϣ
        m_pIAndroidUserItem->SendSocketData(SUB_S_ANDROIDUP);
			//continue;
			return true;
		}

		//�����ж�
		if(m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP))
		{
			//��������
			//PerformStandUpAction(pIServerUserItem);
        m_pIAndroidUserItem->SendSocketData(SUB_S_ANDROIDUP);
			//continue;
			return true;
		}	


        //����׼��
        m_pIAndroidUserItem->SendUserReady(NULL,0);

        return true;
      }
      case IDI_CALL_BANKER:   //��ׯ��ʱ
      {
        //���ñ���
        CMD_C_CallBanker CallBanker;
        CallBanker.bBanker = rand()%2;

        //������Ϣ
        m_pIAndroidUserItem->SendSocketData(SUB_C_CALL_BANKER,&CallBanker,sizeof(CallBanker));

        return true;
      }
      case IDI_USER_ADD_SCORE:  //��ע��ʱ
      {
        //����ע����
        SCORE lUserMaxScore[MAX_JETTON_AREA];
        ZeroMemory(lUserMaxScore,sizeof(lUserMaxScore));
        SCORE lTemp=m_lTurnMaxScore;
        for(WORD i=0; i<MAX_JETTON_AREA; i++)
        {
          if(lTemp > 0)
            //lUserMaxScore[i] = __max(lTemp/((i+1)),1L);
          {
            lUserMaxScore[i]=__max(lTemp/(pow(2,i)),1L);
          }
          else
          {
            lUserMaxScore[i] = 1;
          }
        }

        //��ע���� 60%����ѡ���д�ע��
        BYTE cbAddScoreIndex = 0;
        BYTE cbRand1 = rand() % 100;
        BYTE cbRand2 = cbRand1 % 2;

        cbAddScoreIndex = (cbRand1 < 60) ? cbRand2 : (2 + cbRand2);

        //������Ϣ
        CMD_C_AddScore AddScore;
        AddScore.lScore=lUserMaxScore[cbAddScoreIndex%MAX_JETTON_AREA];
        m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

        return true;
      }
      case IDI_OPEN_CARD:     //���ƶ�ʱ
      {
        //������Ϣ
        CMD_C_OxCard OxCard;
        OxCard.bOX=(m_GameLogic.GetCardType(m_HandCardData,MAX_COUNT)>0)?TRUE:FALSE;
        m_pIAndroidUserItem->SendSocketData(SUB_C_OPEN_CARD,&OxCard,sizeof(OxCard));

        //BankOperate();

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
    case SUB_S_CALL_BANKER: //�û���ׯ
    {
      //��Ϣ����
      return OnSubCallBanker(pData,wDataSize);
    }
    case SUB_S_GAME_START:  //��Ϸ��ʼ
    {
      //��Ϣ����
      return OnSubGameStart(pData,wDataSize);
    }
    case SUB_S_ADD_SCORE: //�û���ע
    {
      //��Ϣ����
      return OnSubAddScore(pData,wDataSize);
    }
    case SUB_S_SEND_CARD: //������Ϣ
    {
      //��Ϣ����
      return OnSubSendCard(pData,wDataSize);
    }
    case SUB_S_OPEN_CARD: //�û�̯��
    {
      //��Ϣ����
      return OnSubOpenCard(pData,wDataSize);
    }
    case SUB_S_PLAYER_EXIT: //�û�ǿ��
    {
      //��Ϣ����
      return OnSubPlayerExit(pData,wDataSize);
    }
    case SUB_S_GAME_END:  //��Ϸ����
    {
      //��Ϣ����
      return OnSubGameEnd(pData,wDataSize);
    }
    case SUB_S_ALL_CARD:
    {
      return true;
    }
    case SUB_S_ANDROID_BANKOPERATOR:
    {
      //BankOperate(2);
      return true;
    }
    case SUB_S_ADMIN_STORAGE_INFO:
    {
      return true;
    }
  }

  //�������
  ASSERT(FALSE);

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

      memcpy(m_szRoomName, pStatusFree->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation(&(pStatusFree->RobotConfig));

      BankOperate(2);

      //��ʼʱ��
      UINT nElapse= TIME_LESS + (rand() % TIME_USER_START_GAME);
      m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

      return true;
    }
    case GS_TK_CALL:  // ��ׯ״̬
    {
      //Ч������
      if(wDataSize!=sizeof(CMD_S_StatusCall))
      {
        return false;
      }
      CMD_S_StatusCall * pStatusCall=(CMD_S_StatusCall *)pData;

      memcpy(m_szRoomName, pStatusCall->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation(&(pStatusCall->RobotConfig));

      BankOperate(2);

      //ʼ���û�
      if(pStatusCall->wCallBanker==m_pIAndroidUserItem->GetChairID())
      {
        //��ׯʱ��
        UINT nElapse = TIME_LESS + (rand() % TIME_USER_CALL_BANKER);
        m_pIAndroidUserItem->SetGameTimer(IDI_CALL_BANKER,nElapse);
      }

      return true;
    }
    case GS_TK_SCORE: //��ע״̬
    {
      //Ч������
      if(wDataSize!=sizeof(CMD_S_StatusScore))
      {
        return false;
      }
      CMD_S_StatusScore * pStatusScore=(CMD_S_StatusScore *)pData;

      memcpy(m_szRoomName, pStatusScore->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation(&(pStatusScore->RobotConfig));

      BankOperate(2);

      //���ñ���
      m_lTurnMaxScore=pStatusScore->lTurnMaxScore;
      WORD wMeChairId = m_pIAndroidUserItem->GetChairID();

      //���ó���
      if(pStatusScore->lTurnMaxScore>0L && pStatusScore->lTableScore[wMeChairId]==0L)
      {
        //��עʱ��
        UINT nElapse = TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
        m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
      }

      return true;
    }
    case GS_TK_PLAYING: //��Ϸ״̬
    {
      //Ч������
      if(wDataSize!=sizeof(CMD_S_StatusPlay))
      {
        return false;
      }
      CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

      memcpy(m_szRoomName, pStatusPlay->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation(&(pStatusPlay->RobotConfig));

      BankOperate(2);

      //���ñ���
      m_lTurnMaxScore=pStatusPlay->lTurnMaxScore;
      WORD wMeChiarID=m_pIAndroidUserItem->GetChairID();
      CopyMemory(m_HandCardData,pStatusPlay->cbHandCardData[wMeChiarID],MAX_COUNT);

      //�ؼ�����
      if(pStatusPlay->bOxCard[wMeChiarID]==0xff)
      {
        //����ʱ��
        UINT nElapse = TIME_LESS + 2 + (rand() % TIME_USER_OPEN_CARD);
        m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD,nElapse);
      }

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

//�û���ׯ
bool CAndroidUserItemSink::OnSubCallBanker(const void * pBuffer, WORD wDataSize)
{

  //Ч������
  if(wDataSize!=sizeof(CMD_S_CallBanker))
  {
    return false;
  }
  CMD_S_CallBanker * pCallBanker=(CMD_S_CallBanker *)pBuffer;

  //ʼ���û�
  if(pCallBanker->wCallBanker==m_pIAndroidUserItem->GetChairID())
  {
    //��ׯʱ��
    UINT nElapse = TIME_LESS + (rand() % TIME_USER_CALL_BANKER);
    m_pIAndroidUserItem->SetGameTimer(IDI_CALL_BANKER,nElapse);
  }

  return true;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  if(wDataSize!=sizeof(CMD_S_GameStart))
  {
    return false;
  }
  CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

  //���ñ���
  m_lTurnMaxScore=pGameStart->lTurnMaxScore;

  //���ó���
  if(pGameStart->lTurnMaxScore>0)
  {
    //��עʱ��
    UINT nElapse = TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
    m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
  }

  return true;
}

//�û���ע
bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  if(wDataSize!=sizeof(CMD_S_AddScore))
  {
    return false;
  }
  CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;

  return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{

  //Ч������
  if(wDataSize!=sizeof(CMD_S_SendCard))
  {
    return false;
  }
  CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

  //��������
  WORD wMeChiarID=m_pIAndroidUserItem->GetChairID();
  CopyMemory(m_HandCardData,pSendCard->cbCardData[wMeChiarID],sizeof(m_HandCardData));

  //����ʱ��
  UINT nElapse = TIME_LESS + 4 + (rand() % TIME_USER_OPEN_CARD);
  m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD,nElapse);

  return true;
}

//�û�̯��
bool CAndroidUserItemSink::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  if(wDataSize!=sizeof(CMD_S_Open_Card))
  {
    return false;
  }
  CMD_S_Open_Card * pOpenCard=(CMD_S_Open_Card *)pBuffer;

  return true;
}

//�û�ǿ��
bool CAndroidUserItemSink::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  if(wDataSize!=sizeof(CMD_S_PlayerExit))
  {
    return false;
  }
  CMD_S_PlayerExit * pPlayerExit=(CMD_S_PlayerExit *)pBuffer;

  return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
  //Ч�����
  if(wDataSize!=sizeof(CMD_S_GameEnd))
  {
    return false;
  }
  CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

  //ɾ����ʱ��
  m_pIAndroidUserItem->KillGameTimer(IDI_CALL_BANKER);
  m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
  m_pIAndroidUserItem->KillGameTimer(IDI_OPEN_CARD);

  //��ʼʱ��
  if(pGameEnd->cbDelayOverGame==0)
  {
    UINT nElapse= TIME_LESS + (rand() % TIME_USER_START_GAME) + 3;
    m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

    //BankOperate(3);
  }
  else
  {
    UINT nElapse=/*TIME_LESS +*/ pGameEnd->cbDelayOverGame;
    m_pIAndroidUserItem->SetGameTimer(IDI_DELAY_TIME,nElapse);
  }


  //�������
  m_lTurnMaxScore = 0;
  ZeroMemory(m_HandCardData,sizeof(m_HandCardData));


  return true;
}

////���в���
//void CAndroidUserItemSink::BankOperate(BYTE cbType)
//{
//  if(cbType == 3)
//  {
//    if(rand() % 100 > 33)
//    {
//      return;
//    }
//  }
//  IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
//  if(pUserItem->GetUserStatus()>=US_SIT)
//  {
//    if(cbType==1)
//    {
//      CString strInfo;
//      strInfo.Format(TEXT("������״̬���ԣ���ִ�д�ȡ��"));
//      NcaTextOut(strInfo);
//
//      return;
//
//    }
//  }
//
//  //��������
//  SCORE lRobotScore = pUserItem->GetUserScore();
//
//  {
//    CString strInfo;
//    strInfo.Format(TEXT("[%s] ����(%I64d)"),pUserItem->GetNickName(),lRobotScore);
//
//    if(lRobotScore > m_lRobotScoreRange[1])
//    {
//      CString strInfo1;
//      strInfo1.Format(TEXT("����������(%I64d)"),m_lRobotScoreRange[1]);
//      strInfo+=strInfo1;
//      //if(cbType==1)
//      NcaTextOut(strInfo);
//    }
//    else if(lRobotScore < m_lRobotScoreRange[0])
//    {
//      CString strInfo1;
//      strInfo1.Format(TEXT("����ȡ������(%I64d)"),m_lRobotScoreRange[0]);
//      strInfo+=strInfo1;
//      //if(cbType==1)
//      NcaTextOut(strInfo);
//    }
//
//    //�жϴ�ȡ
//    if(lRobotScore > m_lRobotScoreRange[1])
//    {
//      SCORE lSaveScore=0L;
//
//      lSaveScore = SCORE(lRobotScore*m_nRobotBankStorageMul/100);
//      if(lSaveScore > lRobotScore)
//      {
//        lSaveScore = lRobotScore;
//      }
//
//      if(lSaveScore > 0)
//      {
//        m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
//      }
//
//      SCORE lRobotNewScore = pUserItem->GetUserScore();
//
//      CString strInfo;
//      strInfo.Format(TEXT("[%s] ִ�д����ǰ���(%I64d)��������(%I64d)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);
//
//      //if(cbType==1)
//      NcaTextOut(strInfo);
//
//    }
//    else if(lRobotScore < m_lRobotScoreRange[0])
//    {
//
//      CString strInfo;
//      strInfo.Format(TEXT("������Ϣ��ȡ����Сֵ(%I64d)��ȡ�����ֵ(%I64d)"),m_lRobotBankGetScore,m_lRobotBankGetScoreBanker);
//
//      if(cbType==1)
//        NcaTextOut(strInfo);
//
//      SCORE lScore=rand()%m_lRobotBankGetScoreBanker+m_lRobotBankGetScore;
//      if(lScore > 0)
//      {
//        m_pIAndroidUserItem->PerformTakeScore(lScore);
//      }
//
//      SCORE lRobotNewScore = pUserItem->GetUserScore();
//
//      //CString strInfo;
//      strInfo.Format(TEXT("[%s] ִ��ȡ�ȡ��ǰ���(%I64d)��ȡ�����(%I64d)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);
//
//      //if(cbType==1)
//      NcaTextOut(strInfo);
//
//    }
//  }
//}
//���в���
void CAndroidUserItemSink::BankOperate(BYTE cbType)
{
	IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
	if(pUserItem->GetUserStatus()>=US_SIT)
	{
		if(cbType==1)
		{
			CString strInfo;
			strInfo.Format(TEXT("������״̬���ԣ���ִ�д�ȡ��"));
			NcaTextOut(strInfo);
			return;
		}
	}

	//��������
	SCORE lRobotScore = pUserItem->GetUserScore();

	{
		CString strInfo;
		strInfo.Format(TEXT("[%s] ����(%.2lf)"),pUserItem->GetNickName(),lRobotScore);

		if (lRobotScore > m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)
		{
			CString strInfo1;
			strInfo1.Format(TEXT("����������(%.2lf)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore);
			strInfo+=strInfo1;

			NcaTextOut(strInfo);
		}
		else if (lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)
		{
			CString strInfo1;
			strInfo1.Format(TEXT("����ȡ������(%.2lf)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore);
			strInfo+=strInfo1;

			NcaTextOut(strInfo);
		}

		//�жϴ�ȡ
		if (lRobotScore >m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)
		{			
			SCORE lSaveScore=0L;
			lSaveScore = lRobotScore - m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore;

			if (lSaveScore > 0) 
			{
				lSaveScore += (rand()%(int)(m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore - m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore));
				m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
			}
			SCORE lRobotNewScore = pUserItem->GetUserScore();

			CString strInfo;
			strInfo.Format(TEXT("[%s] ִ�д����ǰ���(%.2lf)��������(%.2lf)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			NcaTextOut(strInfo);
		}
		else if (lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)
		{
			CString strInfo;
			SCORE lScore=m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore-lRobotScore;
			if (lScore > 0)
			{
				lScore += (rand()%(int)(m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore - m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore));
				m_pIAndroidUserItem->PerformTakeScore(lScore);
			}

			SCORE lRobotNewScore = pUserItem->GetUserScore();
			strInfo.Format(TEXT("[%s] ִ��ȡ�ȡ��ǰ���(%.2lf)��ȡ�����(%.2lf)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			NcaTextOut(strInfo);
		}
	}
}
//��ȡ����
void CAndroidUserItemSink::ReadConfigInformation(tagRobotConfig *pConfig)
{
  m_lRobotScoreRange[0] = pConfig->lRobotScoreMin;
  m_lRobotScoreRange[1] = pConfig->lRobotScoreMax;


  if(m_lRobotScoreRange[1] < m_lRobotScoreRange[0])
  {
    m_lRobotScoreRange[1] = m_lRobotScoreRange[0];
  }

  m_lRobotBankGetScore = pConfig->lRobotBankGet;

  m_lRobotBankGetScoreBanker = pConfig->lRobotBankGetBanker;

  m_nRobotBankStorageMul = pConfig->lRobotBankStoMul;


  if(m_nRobotBankStorageMul < 0 || m_nRobotBankStorageMul > 100)
  {
    m_nRobotBankStorageMul = 20;
  }
}


//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
