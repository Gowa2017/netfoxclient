#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_BANK_OPERATE      3                 //���ж�ʱ
#define IDI_PLACE_JETTON1     103                 //��ע��ʱ
#define IDI_PLACE_JETTON2     104                 //��ע��ʱ
#define IDI_PLACE_JETTON3     105                 //��ע��ʱ
#define IDI_PLACE_JETTON4     106                 //��ע��ʱ
#define IDI_PLACE_JETTON5     107                 //��ע��ʱ
#define IDI_CHECK_BANKER      108                 //�����ׯ
#define IDI_REQUEST_BANKER      101                 //���붨ʱ
#define IDI_GIVEUP_BANKER     102                 //��ׯ��ʱ
#define IDI_PLACE_JETTON      110                 //��ע���� (Ԥ��110-160)

//////////////////////////////////////////////////////////////////////////

int CAndroidUserItemSink::m_stlApplyBanker = 0L;
int CAndroidUserItemSink::m_stnApplyCount = 0L;

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
  //��Ϸ����
  m_lMaxChipBanker = 0;
  m_lMaxChipUser = 0;
  m_wCurrentBanker = 0;
  m_nChipTime = 0;
  m_nChipTimeCount = 0;
  m_cbTimeLeave = 0;
  m_nListUserCount=0;
  ZeroMemory(m_lAreaChip, sizeof(m_lAreaChip));
  ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));
  m_nRobotBetTimeLimit[0]=3;
  m_nRobotBetTimeLimit[1]=8;
  m_lRobotJettonLimit[0]=1;
  m_lRobotJettonLimit[1]=1000;

  //��ׯ����
  m_bMeApplyBanker=false;
  m_nWaitBanker=0;
  m_nBankerCount=0;
  m_nRobotBankerCount=3;

  //��ׯ����
  m_nRobotListMaxCount = 5;
  m_nRobotListMinCount = 2;

  return;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
VOID *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
  return NULL;
}

//��ʼ�ӿ�
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
  //��ѯ�ӿ�
  m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
  if(m_pIAndroidUserItem==NULL)
  {
    return false;
  }

  return true;
}

//���ýӿ�
bool  CAndroidUserItemSink::RepositionSink()
{
  //��Ϸ����
  m_lMaxChipBanker = 0;
  m_lMaxChipUser = 0;
  m_wCurrentBanker = 0;
  m_nChipTime = 0;
  m_nChipTimeCount = 0;
  m_cbTimeLeave = 0;
  ZeroMemory(m_lAreaChip, sizeof(m_lAreaChip));
  ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));

  //��ׯ����
  m_bMeApplyBanker=false;
  m_nWaitBanker=0;
  m_nBankerCount=0;

  return true;
}

//ʱ����Ϣ
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
  switch(nTimerID)
  {
    case IDI_CHECK_BANKER:    //�����ׯ
    {
      m_pIAndroidUserItem->KillGameTimer(nTimerID);

      if(m_wCurrentBanker == m_pIAndroidUserItem->GetChairID())
      {
        return false;
      }

      int nMinCount = m_nRobotListMaxCount;
      if(m_nRobotListMaxCount > m_nRobotListMinCount)
      {
        nMinCount = (rand()%(m_nRobotListMaxCount - m_nRobotListMinCount+1)) + m_nRobotListMinCount;
      }

      //��ׯ
      m_nWaitBanker++;

      //��������ׯ
      if(m_bRobotBanker
         && !m_bMeApplyBanker
         && m_nWaitBanker >= m_nRobotWaitBanker
         && m_nListUserCount < m_nRobotListMaxCount
         && m_stlApplyBanker < m_nRobotListMaxCount
         && m_stlApplyBanker < nMinCount
         && m_stnApplyCount < 2)
      {
        m_nWaitBanker = 0;

        //�Ϸ��ж�
        IServerUserItem *pIUserItemBanker = m_pIAndroidUserItem->GetMeUserItem();
        if(pIUserItemBanker->GetUserScore() > m_lBankerCondition)
        {
          //��������ׯ
          m_nBankerCount = 0;
          m_stlApplyBanker++;
          m_stnApplyCount++;
          m_pIAndroidUserItem->SetGameTimer(IDI_REQUEST_BANKER, (rand() % m_cbTimeLeave) + 1);
        }
        else
        {
          ////ִ��ȡ��
          //LONGLONG lDiffScore = (m_lRobotBankGetScoreBankerMax-m_lRobotBankGetScoreBankerMin)/100;
          //SCORE lScore = m_lRobotBankGetScoreBankerMin + (m_pIAndroidUserItem->GetUserID()%10)*(rand()%10)*lDiffScore +
          //               (rand()%10+1)*(rand()%(lDiffScore/10));

          //if(lScore > 0)
          //{
          //  m_pIAndroidUserItem->PerformTakeScore(lScore);
          //}

          IServerUserItem *pIUserItemBanker = m_pIAndroidUserItem->GetMeUserItem();
          if(pIUserItemBanker->GetUserScore() > m_lBankerCondition)
          {
            //��������ׯ
            m_nBankerCount = 0;
            m_stlApplyBanker++;
            m_pIAndroidUserItem->SetGameTimer(IDI_REQUEST_BANKER, (rand() % m_cbTimeLeave) + 1);
          }
        }
      }

      return false;
    }
    case IDI_REQUEST_BANKER:  //������ׯ
    {
      m_pIAndroidUserItem->KillGameTimer(nTimerID);

      m_pIAndroidUserItem->SendSocketData(SUB_C_APPLY_BANKER);

      return false;
    }
    case IDI_GIVEUP_BANKER:   //������ׯ
    {
      m_pIAndroidUserItem->KillGameTimer(nTimerID);

      m_pIAndroidUserItem->SendSocketData(SUB_C_CANCEL_BANKER);

      return false;
    }
    case IDI_BANK_OPERATE:    //���в���
    {
      m_pIAndroidUserItem->KillGameTimer(nTimerID);
			//���в���
			BankOperate(2);
      ////��������
      //IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
      //SCORE lRobotScore = pUserItem->GetUserScore();
      //{

      //  //�жϴ�ȡ
      //  if(lRobotScore > m_lRobotScoreRange[1])
      //  {
      //    SCORE lSaveScore=0L;

      //    lSaveScore = SCORE(lRobotScore*m_nRobotBankStorageMul/100);
      //    if(lSaveScore > lRobotScore)
      //    {
      //      lSaveScore = lRobotScore;
      //    }

      //    if(lSaveScore > 0 && m_wCurrentBanker != m_pIAndroidUserItem->GetChairID())
      //    {
      //      m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
      //    }

      //  }
      //  else if(lRobotScore < m_lRobotScoreRange[0])
      //  {
      //    LONGLONG lDiffScore = (m_lRobotBankGetScoreMax-m_lRobotBankGetScoreMin)/100;
      //    SCORE lScore = m_lRobotBankGetScoreMin + (m_pIAndroidUserItem->GetUserID()%10)*(rand()%10)*lDiffScore +
      //                   (rand()%10+1)*(rand()%(lDiffScore/10));

      //    if(lScore > 0)
      //    {
      //      m_pIAndroidUserItem->PerformTakeScore(lScore);
      //    }
      //  }
      //}
      return false;
    }
    default:
    {
      if(nTimerID >= IDI_PLACE_JETTON && nTimerID <= IDI_PLACE_JETTON+MAX_CHIP_TIME)
      {
        srand(GetTickCount());

        //��������
        int nRandNum = 0, nChipArea = 0, nCurChip = 0, nACTotal = 0, nCurJetLmt[2] = {};
        SCORE lMaxChipLmt = __min(m_lMaxChipBanker, m_lMaxChipUser);     //������עֵ
        WORD wMyID = m_pIAndroidUserItem->GetChairID();
        for(int i = 0; i <AREA_COUNT; i++)
        {
          nACTotal += m_RobotInfo.nAreaChance[i];
        }

        //ͳ�ƴ���
        m_nChipTimeCount++;

        //����˳�
        if(lMaxChipLmt < m_RobotInfo.nChip[m_nChipLimit[0]])
        {
          return false;
        }
        for(int i = 0; i < AREA_COUNT; i++)
        {
          if(m_lAreaChip[i] >= m_RobotInfo.nChip[m_nChipLimit[0]])
          {
            break;
          }
          if(i == AREA_COUNT-1)
          {
            return false;
          }
        }

        //��ע����
        ASSERT(nACTotal>0);
        static int nStFluc = 1;       //�������
        if(nACTotal <= 0)
        {
          return false;
        }
        do
        {
          nRandNum = (rand()+wMyID+nStFluc*3) % nACTotal;
          for(int i = 0; i < AREA_COUNT; i++)
          {
            nRandNum -= m_RobotInfo.nAreaChance[i];
            if(nRandNum < 0)
            {
              nChipArea = i;
              break;
            }
          }
        }
        while(m_lAreaChip[nChipArea] < m_RobotInfo.nChip[m_nChipLimit[0]]);
        nStFluc = nStFluc%3 + 1;

        //��ע��С
        if(m_nChipLimit[0] == m_nChipLimit[1])
        {
          nCurChip = m_nChipLimit[0];
        }
        else
        {
          //���ñ���
          lMaxChipLmt = __min(lMaxChipLmt, m_lAreaChip[nChipArea]);
          nCurJetLmt[0] = m_nChipLimit[0];
          nCurJetLmt[1] = m_nChipLimit[0];

          //���㵱ǰ������
          for(int i = m_nChipLimit[1]; i > m_nChipLimit[0]; i--)
          {
            if(lMaxChipLmt > m_RobotInfo.nChip[i])
            {
              nCurJetLmt[1] = i;
              break;
            }
          }

          //�����ע
          nRandNum = (rand()+wMyID) % (nCurJetLmt[1]-nCurJetLmt[0]+1);
          nCurChip = nCurJetLmt[0] + nRandNum;

          //���¿��� (��ׯ�ҽ�ҽ���ʱ�ᾡ����֤�������)
          if(m_nChipTimeCount < m_nChipTime)
          {
            SCORE lLeftJetton = SCORE((lMaxChipLmt-m_RobotInfo.nChip[nCurChip])/(m_nChipTime-m_nChipTimeCount));

            //�������� (��ȫ����С���Ƴ�����עҲ����)
            if(lLeftJetton < m_RobotInfo.nChip[m_nChipLimit[0]] && nCurChip > m_nChipLimit[0])
            {
              nCurChip--;
            }
          }
        }

        //��������
        CMD_C_PlaceJetton PlaceJetton = {};

        //�������
        PlaceJetton.cbJettonArea = nChipArea+1;   //������1��ʼ
        PlaceJetton.lJettonScore = m_RobotInfo.nChip[nCurChip];

        //������Ϣ
        m_pIAndroidUserItem->SendSocketData(SUB_C_PLACE_JETTON, &PlaceJetton, sizeof(PlaceJetton));
      }

      m_pIAndroidUserItem->KillGameTimer(nTimerID);
      return false;
    }
  }
  return false;
}

//��Ϸ��Ϣ
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
{
  switch(wSubCmdID)
  {
    case SUB_S_GAME_FREE:     //��Ϸ����
    {
      return OnSubGameFree(pBuffer, wDataSize);
    }
    case SUB_S_GAME_START:      //��Ϸ��ʼ
    {
      return OnSubGameStart(pBuffer, wDataSize);
    }
    case SUB_S_PLACE_JETTON:    //�û���ע
    {
      return OnSubPlaceJetton(pBuffer, wDataSize);
    }
    case SUB_S_APPLY_BANKER:    //������ׯ
    {
      return OnSubUserApplyBanker(pBuffer,wDataSize);
    }
    case SUB_S_CANCEL_BANKER:   //ȡ����ׯ
    {
      return OnSubUserCancelBanker(pBuffer,wDataSize);
    }
    case SUB_S_CHANGE_BANKER:   //�л�ׯ��
    {
      return OnSubChangeBanker(pBuffer,wDataSize);
    }
    case SUB_S_GAME_END:      //��Ϸ����
    {
      return OnSubGameEnd(pBuffer, wDataSize);
    }
    case SUB_S_SEND_RECORD:     //��Ϸ��¼ (����)
    {
      return true;
    }
    case SUB_S_PLACE_JETTON_FAIL: //��עʧ�� (����)
    {
      return true;
    }
  }

  //�������
  ASSERT(FALSE);

  return true;
}

//��Ϸ��Ϣ
bool  CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
  return true;
}

//������Ϣ
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
  switch(cbGameStatus)
  {
    case GAME_SCENE_FREE:     //����״̬
    {
      //Ч������
      ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
      if(wDataSize!=sizeof(CMD_S_StatusFree))
      {
        return false;
      }

      //��Ϣ����
      CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

      m_lUserLimitScore = pStatusFree->lUserMaxScore;
      m_lAreaLimitScore = pStatusFree->lAreaLimitScore;
      m_lBankerCondition = pStatusFree->lApplyBankerCondition;

      memcpy(m_szRoomName, pStatusFree->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation(&pStatusFree->CustomAndroid);

      ////��ׯ����
      //if(pStatusFree->wBankerUser == INVALID_CHAIR)
      //{
      //  if(m_bRobotBanker && m_nRobotWaitBanker == 0  && m_stlApplyBanker < m_nRobotApplyBanker)
      //  {
      //    //�Ϸ��ж�
      //    IServerUserItem *pIUserItemBanker = m_pIAndroidUserItem->GetMeUserItem();
      //    if(pIUserItemBanker->GetUserScore() > m_lBankerCondition)
      //    {
      //      //��������ׯ
      //      m_nBankerCount = 0;
      //      m_stlApplyBanker++;

      //      BYTE cbTime = (pStatusFree->cbTimeLeave>0?(rand()%pStatusFree->cbTimeLeave+1):2);
      //      if(cbTime == 0)
      //      {
      //        cbTime = 2;
      //      }

      //      m_pIAndroidUserItem->SetGameTimer(IDI_REQUEST_BANKER, cbTime);
      //    }
      //  }
      //}
					//���в���
			BankOperate(2);
      return true;
    }
    case GAME_SCENE_PLACE_JETTON:   //��Ϸ״̬
    case GAME_SCENE_GAME_END: //����״̬
    {
      //Ч������
      ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
      if(wDataSize!=sizeof(CMD_S_StatusPlay))
      {
        return false;
      }

      //��Ϣ����
      CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

      //ׯ����Ϣ
      m_wCurrentBanker = pStatusPlay->wBankerUser;

      m_lUserLimitScore = pStatusPlay->lUserMaxScore;
      m_lAreaLimitScore = pStatusPlay->lAreaLimitScore;
      m_lBankerCondition = pStatusPlay->lApplyBankerCondition;

      memcpy(m_szRoomName, pStatusPlay->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation(&pStatusPlay->CustomAndroid);
			//���в���
			BankOperate(2);
      return true;
    }
  }

  return true;
}

//�û�����
VOID  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//�û��뿪
VOID  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//�û�����
VOID  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//�û�״̬
VOID  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}


//��Ϸ����
bool CAndroidUserItemSink::OnSubGameFree(const VOID * pBuffer, WORD wDataSize)
{
  //��Ϣ����
  CMD_S_GameFree* pGameFree=(CMD_S_GameFree *)pBuffer;

  m_cbTimeLeave = pGameFree->cbTimeLeave;
  m_nListUserCount = pGameFree->nListUserCount;

  //���в���
  if(pGameFree->cbTimeLeave > 1 && m_wCurrentBanker != m_pIAndroidUserItem->GetChairID())
  {
    m_pIAndroidUserItem->SetGameTimer(IDI_BANK_OPERATE, (rand() % (pGameFree->cbTimeLeave-1)) + 1);
  }

  bool bMeGiveUp = false;
  if(m_wCurrentBanker == m_pIAndroidUserItem->GetChairID())
  {
    m_nBankerCount++;
    if(m_nBankerCount >= m_nRobotBankerCount)
    {
      //��������ׯ
      m_nBankerCount = 0;
      m_pIAndroidUserItem->SetGameTimer(IDI_GIVEUP_BANKER, rand()%2 + 1);

      bMeGiveUp = true;
    }
  }

  //�����ׯ
  if(m_wCurrentBanker != m_pIAndroidUserItem->GetChairID() || bMeGiveUp)
  {
    m_cbTimeLeave = pGameFree->cbTimeLeave - 3;
    m_pIAndroidUserItem->SetGameTimer(IDI_CHECK_BANKER, 3);
  }

  return true;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const VOID * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_GameStart));
  if(wDataSize!=sizeof(CMD_S_GameStart))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

  srand(GetTickCount());

  //�Լ���ׯ������ע������
  if(pGameStart->wBankerUser == m_pIAndroidUserItem->GetChairID() || pGameStart->nChipRobotCount <= 0)
  {
    return true;
  }

  //���ñ���
  m_lMaxChipBanker = pGameStart->lBankerScore/m_RobotInfo.nMaxTime;
  m_lMaxChipUser = pGameStart->lUserMaxScore/m_RobotInfo.nMaxTime;
  m_nRobotApplyBanker=rand()%m_nRobotApplyBanker+1;
  m_wCurrentBanker = pGameStart->wBankerUser;
  m_nChipTimeCount = 0;
  ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));
  for(int i = 0; i <AREA_COUNT; i++)
  {
    m_lAreaChip[i] = m_lAreaLimitScore;
  }

  //ϵͳ��ׯ
  if(pGameStart->wBankerUser == INVALID_CHAIR)
  {
    m_lMaxChipBanker = 2147483647/m_RobotInfo.nMaxTime;
  }
  else
  {
    m_lMaxChipUser = __min(m_lMaxChipUser, m_lMaxChipBanker);
  }

  if(pGameStart->nAndriodApplyCount > 0)
  {
    m_stlApplyBanker=pGameStart->nAndriodApplyCount;
  }
  else
  {
    m_stlApplyBanker=0;
  }

  //������ע����
  int nElapse = 0;
  WORD wMyID = m_pIAndroidUserItem->GetChairID();

  if(m_nRobotBetTimeLimit[0] == m_nRobotBetTimeLimit[1])
  {
    m_nChipTime = m_nRobotBetTimeLimit[0];
  }
  else
  {
    m_nChipTime = (rand()+wMyID)%(m_nRobotBetTimeLimit[1]-m_nRobotBetTimeLimit[0]+1) + m_nRobotBetTimeLimit[0];
  }
  ASSERT(m_nChipTime>=0);
  if(m_nChipTime <= 0)
  {
    return true;  //��ȷ,2����������
  }
  if(m_nChipTime > MAX_CHIP_TIME)
  {
    m_nChipTime = MAX_CHIP_TIME;  //�޶�MAX_CHIP����ע
  }

  //���㷶Χ
  if(!CalcJettonRange(__min(m_lMaxChipBanker, m_lMaxChipUser), m_lRobotJettonLimit, m_nChipTime, m_nChipLimit))
  {
    return true;
  }

  //����ʱ��
  int nTimeGrid = int(pGameStart->cbTimeLeave-5)*800/m_nChipTime;   //ʱ���,ǰ2�벻��ע,����-2,800��ʾ��������עʱ�䷶Χǧ�ֱ�
  for(int i = 0; i < m_nChipTime; i++)
  {
    int nRandRage = int(nTimeGrid * i / (1500*sqrt((double)m_nChipTime))) + 1;     //������Χ
    nElapse = 5 + (nTimeGrid*i)/1000 + ((rand()+wMyID)%(nRandRage*2) - (nRandRage-1));
    ASSERT(nElapse>=5&&nElapse<=pGameStart->cbTimeLeave);
    if(nElapse < 5 || nElapse > pGameStart->cbTimeLeave)
    {
      continue;
    }

    m_pIAndroidUserItem->SetGameTimer(IDI_PLACE_JETTON+i+1, nElapse);
  }

  return true;
}

//�û���ע
bool CAndroidUserItemSink::OnSubPlaceJetton(const VOID * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_PlaceJetton));
  if(wDataSize!=sizeof(CMD_S_PlaceJetton))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;

  //���ñ���
  m_lMaxChipBanker -= pPlaceJetton->lJettonScore;
  m_lAreaChip[pPlaceJetton->cbJettonArea-1] -= pPlaceJetton->lJettonScore;
  if(pPlaceJetton->wChairID == m_pIAndroidUserItem->GetChairID())
  {
    m_lMaxChipUser -= pPlaceJetton->lJettonScore;
  }

  return true;
}

//��עʧ��
bool CAndroidUserItemSink::OnSubPlaceJettonFail(const VOID * pBuffer, WORD wDataSize)
{
  return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const VOID * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
  if(wDataSize!=sizeof(CMD_S_GameEnd))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

  m_stnApplyCount = 0;

  return true;
}

//������ׯ
bool CAndroidUserItemSink::OnSubUserApplyBanker(const VOID * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
  if(wDataSize!=sizeof(CMD_S_ApplyBanker))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;

  //�Լ��ж�
  if(m_pIAndroidUserItem->GetChairID()==pApplyBanker->wApplyUser)
  {
    m_bMeApplyBanker=true;
  }

  return true;
}

//ȡ����ׯ
bool CAndroidUserItemSink::OnSubUserCancelBanker(const VOID * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_CancelBanker));
  if(wDataSize!=sizeof(CMD_S_CancelBanker))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;

  //�Լ��ж�
  if(m_pIAndroidUserItem->GetChairID()==pCancelBanker->wCancelUser)
  {
    m_bMeApplyBanker=false;
  }

  return true;
}

//�л�ׯ��
bool CAndroidUserItemSink::OnSubChangeBanker(const VOID * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
  if(wDataSize!=sizeof(CMD_S_ChangeBanker))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_ChangeBanker * pChangeBanker = (CMD_S_ChangeBanker *)pBuffer;

  if(m_wCurrentBanker == m_pIAndroidUserItem->GetChairID() && m_wCurrentBanker != pChangeBanker->wBankerUser)
  {
    //m_stlApplyBanker--;
    m_nWaitBanker = 0;
    m_bMeApplyBanker = false;
  }
  m_wCurrentBanker = pChangeBanker->wBankerUser;

  return true;
}

//��ȡ����
void CAndroidUserItemSink::ReadConfigInformation(tagCustomAndroid *pCustomAndroid)
{
  //��������
  m_nRobotBetTimeLimit[0] = pCustomAndroid->lRobotMinBetTime;
  m_nRobotBetTimeLimit[1] = pCustomAndroid->lRobotMaxBetTime;
  if(m_nRobotBetTimeLimit[0] < 0)
  {
    m_nRobotBetTimeLimit[0] = 0;
  }
  if(m_nRobotBetTimeLimit[1] < m_nRobotBetTimeLimit[0])
  {
    m_nRobotBetTimeLimit[1] = m_nRobotBetTimeLimit[0];
  }

  //��������
  m_lRobotJettonLimit[0] = pCustomAndroid->lRobotMinJetton;
  m_lRobotJettonLimit[1] = pCustomAndroid->lRobotMaxJetton;
  if(m_lRobotJettonLimit[1] > 1000)
  {
    m_lRobotJettonLimit[1] = 1000;
  }
  if(m_lRobotJettonLimit[0] < 1)
  {
    m_lRobotJettonLimit[0] = 1;
  }
  if(m_lRobotJettonLimit[1] < m_lRobotJettonLimit[0])
  {
    m_lRobotJettonLimit[1] = m_lRobotJettonLimit[0];
  }

  //�Ƿ���ׯ
  m_bRobotBanker = (pCustomAndroid->nEnableRobotBanker == TRUE)?true:false;

  //��ׯ����
  LONGLONG lRobotBankerCountMin = pCustomAndroid->lRobotBankerCountMin;
  LONGLONG lRobotBankerCountMax = pCustomAndroid->lRobotBankerCountMax;
  m_nRobotBankerCount = rand()%(lRobotBankerCountMax-lRobotBankerCountMin+1) + lRobotBankerCountMin;

  //�б�����
  m_nRobotListMinCount = pCustomAndroid->lRobotListMinCount;
  m_nRobotListMaxCount = pCustomAndroid->lRobotListMaxCount;

  //������
  m_nRobotApplyBanker = pCustomAndroid->lRobotApplyBanker;

  //��������
  m_nRobotWaitBanker = pCustomAndroid->lRobotWaitBanker;

  //��������
  m_lRobotScoreRange[0] = pCustomAndroid->lRobotScoreMin;
  m_lRobotScoreRange[1] = pCustomAndroid->lRobotScoreMax;
  if(m_lRobotScoreRange[1] < m_lRobotScoreRange[0])
  {
    m_lRobotScoreRange[1] = m_lRobotScoreRange[0];
  }

  //�������
  m_lRobotBankGetScoreMin = pCustomAndroid->lRobotBankGetMin;
  m_lRobotBankGetScoreMax = pCustomAndroid->lRobotBankGetMax;

  //������� (ׯ��)
  m_lRobotBankGetScoreBankerMin = pCustomAndroid->lRobotBankGetBankerMin;
  m_lRobotBankGetScoreBankerMax = pCustomAndroid->lRobotBankGetBankerMax;

  //����
  m_nRobotBankStorageMul = pCustomAndroid->lRobotBankStoMul;
  if(m_nRobotBankStorageMul<0 || m_nRobotBankStorageMul>100)
  {
    m_nRobotBankStorageMul = 20;
  }

  return;

}

//���㷶Χ  (����ֵ��ʾ�Ƿ����ͨ���½����޴ﵽ��ע)
bool CAndroidUserItemSink::CalcJettonRange(SCORE lMaxScore, SCORE lChipLmt[], int & nChipTime, int lJetLmt[])
{
  //�������
  bool bHaveSetMinChip = false;

  //����һע
  if(lMaxScore < m_RobotInfo.nChip[0])
  {
    return false;
  }

  //���÷�Χ
  for(int i = 0; i < CountArray(m_RobotInfo.nChip); i++)
  {
    if(!bHaveSetMinChip && m_RobotInfo.nChip[i] >= lChipLmt[0])
    {
      lJetLmt[0] = i;
      bHaveSetMinChip = true;
    }
    if(m_RobotInfo.nChip[i] <= lChipLmt[1])
    {
      lJetLmt[1] = i;
    }
  }
  if(lJetLmt[0] > lJetLmt[1])
  {
    lJetLmt[0] = lJetLmt[1];
  }

  //�Ƿ񽵵�����
  if(m_bReduceJettonLimit)
  {
    if(nChipTime * m_RobotInfo.nChip[lJetLmt[0]] > lMaxScore)
    {
      //�Ƿ񽵵���ע����
      if(nChipTime * m_RobotInfo.nChip[0] > lMaxScore)
      {
        nChipTime = int(lMaxScore/m_RobotInfo.nChip[0]);
        lJetLmt[0] = 0;
        lJetLmt[1] = 0;
      }
      else
      {
        //���͵���������
        while(nChipTime * m_RobotInfo.nChip[lJetLmt[0]] > lMaxScore)
        {
          lJetLmt[0]--;
          ASSERT(lJetLmt[0]>=0);
        }
      }
    }
  }

  return true;
}
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
//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
