#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////

//��������
#define SEND_COUNT          300                 //���ʹ���

//��������
#define INDEX_PLAYER        0                 //�м�����
#define INDEX_BANKER        1                 //ׯ������

//��עʱ��
#define IDI_FREE          1                 //����ʱ��
#define TIME_FREE         5                 //����ʱ��

//��עʱ��
#define IDI_PLACE_JETTON      2                 //��עʱ��
#define TIME_PLACE_JETTON     10                  //��עʱ��

//����ʱ��
#define IDI_GAME_END        3                 //����ʱ��
#define TIME_GAME_END       20                  //����ʱ��

typedef list<SCORE>::reverse_iterator IT;
//////////////////////////////////////////////////////////////////////////
//��̬����
const WORD      CTableFrameSink::m_wPlayerCount=GAME_PLAYER;        //��Ϸ����
void Debug(char *text,...)
{
  static DWORD num=0;
  char buf[1024];
  FILE *fp=NULL;
  va_list ap;
  va_start(ap,text);
  vsprintf(buf,text,ap);
  va_end(ap);
  if(num == 0)
  {
    fp=fopen("���������.log","w");
  }
  else
  {
    fp=fopen("���������.log","a");
  }
  if(fp == NULL)
  {
    return ;
  }
  num++;
  SYSTEMTIME time;
  GetLocalTime(&time);
  fprintf(fp, "%d:%s - %d/%d/%d %d:%d:%d \n", num, buf, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
  fclose(fp);
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
  //����ע��
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

  //������ע
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));
  m_lStorageStart = 0;
  m_CheckImage = 0;

  //��ҳɼ�
  ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
  ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
  ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

  //�˿���Ϣ
  ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
  ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

  //״̬����
  m_dwJettonTime=0L;
  m_bControl=false;
  m_lControlStorage=0;
  CopyMemory(m_szControlName,TEXT("����"),sizeof(m_szControlName));

  //ׯ����Ϣ
  m_ApplyUserArray.RemoveAll();
  m_wCurrentBanker=INVALID_CHAIR;
  m_wBankerUser=INVALID_CHAIR;
  m_lBankerScore=0l;
  m_wBankerTime=0;
  m_lBankerWinScore=0L;
  m_lBankerCurGameScore=0L;
  m_bEnableSysBanker=true;
  m_cbLeftCardCount=0;
  m_bContiueCard=false;
  m_lBankerGold=0l;

  m_bRefreshCfg=true;
  //��¼����
  ZeroMemory(m_GameRecordArrary,sizeof(m_GameRecordArrary));
  m_nRecordFirst=0;
  m_nRecordLast=0;
  m_dwRecordCount=0;
  m_StorageList.clear();
  //���Ʊ���
  m_lStorageStart = 0l;
  m_StorageDeduct = 0l;
  m_StorageDispatch=0l;

  m_lMinPercent=0l;
  m_lMaxPercent=0l;
  m_DispatchGold=0l;

  //�����˿���
  m_lRobotAreaLimit = 0l;
  m_lRobotBetCount = 0l;
  m_nRobotListMaxCount =0;
  ZeroMemory(&m_CustomAndroidConfig,sizeof(m_CustomAndroidConfig));
  //ׯ������
  m_lBankerMAX = 0l;
  m_lBankerAdd = 0l;
  m_lBankerScoreMAX = 0l;
  m_lBankerScoreAdd = 0l;
  m_lPlayerBankerMAX = 0l;
  m_bExchangeBanker = true;
  m_lUserBankerPercent=0l;

  //ʱ�����
  m_cbFreeTime = TIME_FREE;
  m_cbBetTime = TIME_PLACE_JETTON;
  m_cbEndTime = TIME_GAME_END;

  m_lStorageLim1=0;
  m_lStorageLim2=0;
  m_lScoreProbability1=0;
  m_lScoreProbability2=0;

  //�����˿���
  m_nChipRobotCount = 0;
  ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

  //�������
  m_hInst = NULL;
  m_pServerContro = NULL;
  m_hInst = LoadLibrary(_TEXT("BumperCarBattleServerControl.dll"));
  if(m_hInst)
  {
    typedef void * (*CREATE)();
    CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl");
    if(ServerControl)
    {
      m_pServerContro = static_cast<IServerControl*>(ServerControl());
    }
  }

  return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
}

//�ӿڲ�ѯ
VOID * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
  QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
#ifdef __SPECIAL___
  QUERYINTERFACE(ITableUserActionEX,Guid,dwQueryVer);
#endif
  QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
  return NULL;
}

//��ʼ��
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
  //��ѯ�ӿ�
  ASSERT(pIUnknownEx!=NULL);
  m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
  if(m_pITableFrame==NULL)
  {
    return false;
  }

  //���ƽӿ�
  //m_pITableFrameControl=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameControl);
  //if (m_pITableFrameControl==NULL) return false;
  m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

  //��ȡ����
  m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
  ASSERT(m_pGameServiceOption!=NULL);

  //�����ļ���
  TCHAR szPath[MAX_PATH]=TEXT("");
  GetCurrentDirectory(sizeof(szPath),szPath);
  _sntprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\BumperCarBattle.ini"),szPath);
  memcpy(m_szGameRoomName, m_pGameServiceOption->szServerName, sizeof(m_szGameRoomName));

  ReadConfigInformation(true);

  return true;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
  //����ע��
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

  //������ע
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //��ҳɼ�
  ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
  ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
  ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

  //�����˿���
  m_nChipRobotCount = 0;
  ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));
  m_bControl=false;
  m_lControlStorage=0;

  return;
}

bool CTableFrameSink::OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const void*pDataBuffer)
{
  //��������й���ԱȨ�� �򷵻ش���
  if(!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
  {
    return false;
  }

  return true;
}

//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
  if(pIServerUserItem->GetUserStatus() == US_PLAYING)
  {
    return 0L;
  }
  else
  {
    return __max(pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore, 0L);
  }
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{

  //��������
  CMD_S_GameStart GameStart;
  ZeroMemory(&GameStart,sizeof(GameStart));

  //��ȡׯ��
  IServerUserItem *pIBankerServerUserItem=NULL;
  if(INVALID_CHAIR!=m_wCurrentBanker)
  {
    pIBankerServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
    m_lBankerScore=pIBankerServerUserItem->GetUserScore();
    m_lUserBankerPercent=m_lMinPercent+rand()%m_lMaxPercent+1;
  }

  if((INVALID_CHAIR!=m_wCurrentBanker)&&(m_wBankerUser!=m_wCurrentBanker))
  {
    pIBankerServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
    m_wBankerUser=m_wCurrentBanker;
    m_lBankerGold=pIBankerServerUserItem->GetUserScore();
  }

  //CString strStorage;
  //CTime Time(CTime::GetCurrentTime());
  //strStorage.Format(TEXT(" ����: %s | ʱ��: %d-%d-%d %d:%d:%d | ���: %I64d \n"), m_pGameServiceOption->szServerName, Time.GetYear(), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_lStorageStart );
  //WriteInfo(strStorage);


  //���ñ���
  GameStart.cbTimeLeave=m_cbBetTime;
  GameStart.wBankerUser=m_wCurrentBanker;
  GameStart.lBankerScore = 1000000000;
  if(pIBankerServerUserItem!=NULL)
  {
    GameStart.lBankerScore=pIBankerServerUserItem->GetUserScore();
  }

  GameStart.bContiueCard=m_bContiueCard;

  //��ע����������
  int nChipRobotCount = 0;
  for(int i = 0; i < GAME_PLAYER; i++)
  {
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
    {
      nChipRobotCount++;
    }
  }

  GameStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);

  //�����˿���
  m_nChipRobotCount = 0;

  //�Թ����
  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

  //��Ϸ���
  for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
  {
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(pIServerUserItem==NULL)
    {
      continue;
    }

    if(m_bEnableSysBanker==false && m_wCurrentBanker==INVALID_CHAIR)
    {
      GameStart.lBankerScore = 1;
    }

    //���û���
    GameStart.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore);

    m_pITableFrame->SendTableData(wChairID,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
  }

  return true;
}

//��Ϸ����
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
  switch(cbReason)
  {
    case GER_NORMAL:    //�������
    {
      //����
      if(m_pServerContro != NULL && m_pServerContro->NeedControl())
      {
        m_bControl=true;
        tagControlInfo ControlInfo;
        m_pServerContro->ReturnControlArea(ControlInfo);
        m_cbTableCardArray[0][0] = ControlInfo.cbControlArea;
        m_pServerContro->CompleteControl();

        JudgeSystemScore();
      }
      else
      {
        bool bSystemLost = TRUE;
        if(m_lStorageCurrent > m_lStorageMax2)
        {
          bSystemLost= (rand()%100) < m_lStorageMul2;
        }
        else if(m_lStorageCurrent > m_lStorageMax1)
        {
          bSystemLost= (rand()%100) < m_lStorageMul1;
        }
        else    // û�д��ڿ������ϵͳ��һ�뼸����
        {
          bSystemLost= (rand()%100) < 50;
        }


        while(true)
        {
          m_DispatchGold=rand()%m_StorageDispatch;
          //�ɷ��˿�
          DispatchTableCard();

          //��̽���ж�
          if(ProbeJudge(bSystemLost))
          {
            break;
          }
        }
      }

      //�������
      LONGLONG lBankerWinScore=CalculateScore();

      //��������
      m_wBankerTime++;

      //������Ϣ
      CMD_S_GameEnd GameEnd;
      ZeroMemory(&GameEnd,sizeof(GameEnd));

      //ׯ����Ϣ
      GameEnd.nBankerTime = m_wBankerTime;
      GameEnd.lBankerTotallScore=m_lBankerWinScore;
      GameEnd.lBankerScore=lBankerWinScore;
      GameEnd.bcFirstCard = m_bcFirstPostCard;

      //�˿���Ϣ
      CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
      GameEnd.cbLeftCardCount=m_cbLeftCardCount;

      //���ͻ���
      GameEnd.cbTimeLeave=m_cbEndTime;
      for(int wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex)
      {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
        if(pIServerUserItem == NULL)
        {
          continue;
        }
        GameEnd.lUserScoreTotal[wUserIndex]=m_lUserWinScore[wUserIndex];

      }
      for(WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex)
      {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
        if(pIServerUserItem == NULL)
        {
          continue;
        }

        //���óɼ�
        GameEnd.lUserScore=m_lUserWinScore[wUserIndex];

        //��������
        GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

        //����˰��
        if(m_lUserRevenue[wUserIndex]>0)
        {
          GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
        }
        else if(m_wCurrentBanker!=INVALID_CHAIR)
        {
          GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
        }
        else
        {
          GameEnd.lRevenue=0;
        }

        //������Ϣ
        m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
        m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
      }

      return true;
    }
    case GER_USER_LEAVE:    //�û��뿪
    {
      //�м��ж�
      if(m_wCurrentBanker!=wChairID)
      {
        //��������
        LONGLONG lScore=0;
        LONGLONG lRevenue=0;

        //ͳ�Ƴɼ�
        for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
        {
          lScore -= m_lUserJettonScore[nAreaIndex][wChairID];
        }

        //д�����
        if(m_pITableFrame->GetGameStatus() != GS_GAME_END)
        {
          for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
          {
            if(m_lUserJettonScore[nAreaIndex][wChairID] != 0)
            {
              CMD_S_PlaceJettonFail PlaceJettonFail;
              ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
              PlaceJettonFail.lJettonArea=nAreaIndex;
              PlaceJettonFail.lPlaceScore=m_lUserJettonScore[nAreaIndex][wChairID];
              PlaceJettonFail.wPlaceUser=wChairID;

              //��Ϸ���
              for(WORD i=0; i<GAME_PLAYER; ++i)
              {
                IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
                if(pIServerUserItem==NULL)
                {
                  continue;
                }

                m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
              }

              m_lAllJettonScore[nAreaIndex] -= m_lUserJettonScore[nAreaIndex][wChairID];
              m_lUserJettonScore[nAreaIndex][wChairID] = 0;
            }
          }
        }
        else
        {
          //д�����
          if(m_lUserWinScore[wChairID]!=0L)
          {
            tagScoreInfo ScoreInfo[GAME_PLAYER];
            ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
            ScoreInfo[wChairID].cbType =(m_lUserWinScore[wChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
            ScoreInfo[wChairID].lRevenue = m_lUserRevenue[wChairID];
            ScoreInfo[wChairID].lScore = m_lUserWinScore[wChairID];
            m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
            m_lUserWinScore[wChairID] = 0;
          }
          for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
          {
            if(m_lUserJettonScore[nAreaIndex][wChairID] != 0)
            {
              m_lUserJettonScore[nAreaIndex][wChairID] = 0;
            }
          }
        }
        return true;
      }

      //״̬�ж�
      if(m_pITableFrame->GetGameStatus()!=GS_GAME_END)
      {
        //��ʾ��Ϣ
        TCHAR szTipMsg[128];
        _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("����ׯ��[ %s ]ǿ�ˣ���Ϸ��ǰ������"),pIServerUserItem->GetNickName());

        //������Ϣ
        SendGameMessage(INVALID_CHAIR,szTipMsg);

        //����״̬
        m_pITableFrame->SetGameStatus(GS_GAME_END);

        //����ʱ��
        m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
        m_dwJettonTime=(DWORD)time(NULL);
        m_pITableFrame->SetGameTimer(IDI_GAME_END,m_cbEndTime*1000,1,0L);

        //����
        if(m_pServerContro != NULL && m_pServerContro->NeedControl())
        {
          tagControlInfo ControlInfo;
          m_pServerContro->ReturnControlArea(ControlInfo);
          m_cbTableCardArray[0][0] = ControlInfo.cbControlArea;
          m_pServerContro->CompleteControl();

          JudgeSystemScore();
        }
        else
        {

          bool bSystemLost = TRUE;
          if(m_lStorageCurrent > m_lStorageMax2)
          {
            bSystemLost= (rand()%100) < m_lStorageMul2;
          }
          else if(m_lStorageCurrent > m_lStorageMax1)
          {
            bSystemLost= (rand()%100) < m_lStorageMul1;
          }
          else    // û�д��ڿ������ϵͳ��һ�뼸����
          {
            bSystemLost= (rand()%100) < 50;
          }

          while(true)
          {
            //�ɷ��˿�
            DispatchTableCard();

            //��̽���ж�
            if(ProbeJudge(bSystemLost))
            {
              break;
            }
          }
        }

        //�������
        CalculateScore();

        //������Ϣ
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd,sizeof(GameEnd));

        //ׯ����Ϣ
        GameEnd.nBankerTime = m_wBankerTime;
        GameEnd.lBankerTotallScore=m_lBankerWinScore;
        if(m_lBankerWinScore>0)
        {
          GameEnd.lBankerScore=0;
        }

        //�˿���Ϣ
        CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
        GameEnd.cbLeftCardCount=m_cbLeftCardCount;

        //���ͻ���
        GameEnd.cbTimeLeave=m_cbEndTime;
        for(WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex)
        {
          IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
          if(pIServerUserItem == NULL)
          {
            continue;
          }

          //���óɼ�
          GameEnd.lUserScore=m_lUserWinScore[wUserIndex];

          //��������
          GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

          //����˰��
          if(m_lUserRevenue[wUserIndex]>0)
          {
            GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
          }
          else if(m_wCurrentBanker!=INVALID_CHAIR)
          {
            GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
          }
          else
          {
            GameEnd.lRevenue=0;
          }

          //������Ϣ
          m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
          m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
        }
      }

      //�۳�����
      if(m_lUserWinScore[m_wCurrentBanker] != 0l)
      {
        tagScoreInfo ScoreInfo[GAME_PLAYER];
        ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
        ScoreInfo[m_wCurrentBanker].cbType = SCORE_TYPE_FLEE;
        ScoreInfo[m_wCurrentBanker].lRevenue  = m_lUserRevenue[m_wCurrentBanker] ;
        ScoreInfo[m_wCurrentBanker].lScore = m_lUserWinScore[m_wCurrentBanker];
        m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
        m_lUserWinScore[m_wCurrentBanker] = 0;
      }

      //�л�ׯ��
      ChangeBanker(true);

      return true;
    }
  }

  return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
  switch(cbGameStatus)
  {
    case GAME_STATUS_FREE:      //����״̬
    {
      //���ͼ�¼
      SendGameRecord(pIServerUserItem);

      //��������
      CMD_S_StatusFree StatusFree;
      ZeroMemory(&StatusFree,sizeof(StatusFree));

      //������Ϣ
      StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
      StatusFree.lAreaLimitScore = m_lAreaLimitScore;
      StatusFree.CheckImage = m_CheckImage;

      //ׯ����Ϣ
      StatusFree.bEnableSysBanker=m_bEnableSysBanker;
      StatusFree.wBankerUser=m_wCurrentBanker;
      StatusFree.cbBankerTime=m_wBankerTime;
      StatusFree.lBankerWinScore=m_lBankerWinScore;
      StatusFree.lBankerScore = 1000000000;
      StatusFree.lStorageStart=m_lStorageCurrent;
	  StatusFree.nMultiple = 1;
      //����������
      CopyMemory(&(StatusFree.CustomAndroidConfig),&m_CustomAndroidConfig,sizeof(m_CustomAndroidConfig));
      if(m_wCurrentBanker!=INVALID_CHAIR)
      {
        IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
        StatusFree.lBankerScore=pIServerUserItem->GetUserScore();
      }

      //�����Ϣ
      if(pIServerUserItem->GetUserStatus()!=US_LOOKON)
      {
        StatusFree.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*4);
      }

      //ȫ����Ϣ
      DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
      StatusFree.cbTimeLeave=(BYTE)(m_cbFreeTime-__min(dwPassTime,m_cbFreeTime));

      //��������
      CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));

      //���ͳ���
      bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
#ifndef DEBUG
      //������ʾ
      TCHAR szTipMsg[128];
      _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d"),m_lApplyBankerCondition,
                 m_lAreaLimitScore,m_lUserLimitScore);

      m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
#endif
      //����������
      SendApplyUser(pIServerUserItem);
      //���¿����Ϣ
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
        CMD_C_FreshStorage updateStorage;
        ZeroMemory(&updateStorage, sizeof(updateStorage));

        updateStorage.cbReqType = RQ_REFRESH_STORAGE;
        updateStorage.lStorageStart = m_lStorageStart;
        updateStorage.lStorageDeduct = m_StorageDeduct;
        updateStorage.lStorageCurrent = m_lStorageCurrent;
        updateStorage.lStorageMax1 = m_lStorageMax1;
        updateStorage.lStorageMul1 = m_lStorageMul1;
        updateStorage.lStorageMax2 = m_lStorageMax2;
        updateStorage.lStorageMul2 = m_lStorageMul2;

        m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
      }
      return bSuccess;
    }
    case GS_PLACE_JETTON:   //��Ϸ״̬
    case GS_GAME_END:     //����״̬
    {
      //���ͼ�¼
      SendGameRecord(pIServerUserItem);

      //��������
      CMD_S_StatusPlay StatusPlay= {0};

      //ȫ����ע
      CopyMemory(StatusPlay.lAllJettonScore,m_lAllJettonScore,sizeof(StatusPlay.lAllJettonScore));

      //�����ע
      if(pIServerUserItem->GetUserStatus()!=US_LOOKON)
      {
        for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
        {
          StatusPlay.lUserJettonScore[nAreaIndex] = m_lUserJettonScore[nAreaIndex][wChairID];
        }

        //�����ע
        StatusPlay.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore);
      }

      //������Ϣ
      StatusPlay.lApplyBankerCondition=m_lApplyBankerCondition;
      StatusPlay.lAreaLimitScore=m_lAreaLimitScore;

      //ׯ����Ϣ
      StatusPlay.bEnableSysBanker=m_bEnableSysBanker;
      StatusPlay.wBankerUser=m_wCurrentBanker;
      StatusPlay.cbBankerTime=m_wBankerTime;
      StatusPlay.lBankerWinScore=m_lBankerWinScore;
      StatusPlay.CheckImage = m_CheckImage;
      StatusPlay.lBankerScore = 1000000000;
      StatusPlay.lStorageStart=m_lStorageCurrent;
	  	  StatusPlay.nMultiple = 1;
      //����������
      CopyMemory(&(StatusPlay.CustomAndroidConfig),&m_CustomAndroidConfig,sizeof(m_CustomAndroidConfig));
      if(m_wCurrentBanker!=INVALID_CHAIR)
      {
        StatusPlay.lBankerScore=m_lBankerScore;
      }

      //ȫ����Ϣ
      DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
      int nTotalTime = (cbGameStatus==GS_PLACE_JETTON?m_cbBetTime:m_cbEndTime);
      StatusPlay.cbTimeLeave=(BYTE)(nTotalTime-__min(dwPassTime,(DWORD)nTotalTime));
      StatusPlay.cbGameStatus=m_pITableFrame->GetGameStatus();

      //�����ж�
      if(cbGameStatus==GS_GAME_END)
      {
        StatusPlay.cbTimeLeave=(BYTE)(m_cbEndTime-__min(dwPassTime,m_cbEndTime));

        //���óɼ�
        StatusPlay.lEndUserScore=m_lUserWinScore[wChairID];

        //��������
        StatusPlay.lEndUserReturnScore=m_lUserReturnScore[wChairID];

        //����˰��
        if(m_lUserRevenue[wChairID]>0)
        {
          StatusPlay.lEndRevenue=m_lUserRevenue[wChairID];
        }
        else if(m_wCurrentBanker!=INVALID_CHAIR)
        {
          StatusPlay.lEndRevenue=m_lUserRevenue[m_wCurrentBanker];
        }
        else
        {
          StatusPlay.lEndRevenue=0;
        }

        //ׯ�ҳɼ�
        StatusPlay.lEndBankerScore=m_lBankerCurGameScore;

        //�˿���Ϣ
        CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
      }

      //��������
      CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));

      //���ͳ���
      bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
#ifndef DEBUG
      //������ʾ
      TCHAR szTipMsg[128];
      _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d"),m_lApplyBankerCondition,
                 m_lAreaLimitScore,m_lUserLimitScore);

      m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
#endif
      //����������
      SendApplyUser(pIServerUserItem);
      //���¿����Ϣ
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
        CMD_C_FreshStorage updateStorage;
        ZeroMemory(&updateStorage, sizeof(updateStorage));

        updateStorage.cbReqType = RQ_REFRESH_STORAGE;
        updateStorage.lStorageStart = m_lStorageStart;
        updateStorage.lStorageDeduct = m_StorageDeduct;
        updateStorage.lStorageCurrent = m_lStorageCurrent;
        updateStorage.lStorageMax1 = m_lStorageMax1;
        updateStorage.lStorageMul1 = m_lStorageMul1;
        updateStorage.lStorageMax2 = m_lStorageMax2;
        updateStorage.lStorageMul2 = m_lStorageMul2;

        m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
      }
      return bSuccess;
    }
  }

  return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
  switch(wTimerID)
  {
    case IDI_FREE:    //����ʱ��
    {
      //��ʼ��Ϸ
      //m_pITableFrameControl->StartGame();
      m_pITableFrame->StartGame();
      //����ʱ��
      m_dwJettonTime=(DWORD)time(NULL);
      m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_cbBetTime*1000,1,0L);

      //����״̬
      m_pITableFrame->SetGameStatus(GS_PLACE_JETTON);

      return true;
    }
    case IDI_PLACE_JETTON:    //��עʱ��
    {
      //״̬�ж�(��ֹǿ���ظ�����)
      if(m_pITableFrame->GetGameStatus()!=GS_GAME_END)
      {
        //����״̬
        m_pITableFrame->SetGameStatus(GS_GAME_END);

        //������Ϸ
        OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

        //����ʱ��
        m_dwJettonTime=(DWORD)time(NULL);
        m_pITableFrame->SetGameTimer(IDI_GAME_END,m_cbEndTime*1000,1,0L);
      }

      return true;
    }
    case IDI_GAME_END:      //������Ϸ
    {
      //д�����
      tagScoreInfo ScoreInfo[GAME_PLAYER];
      ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
      LONGLONG TempStartScore=0;
      for(WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID)
      {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
        if(pIServerUserItem == NULL||(QueryBuckleServiceCharge(wUserChairID)==false))
        {
          continue;
        }

        //д�����
        if(m_lUserWinScore[wUserChairID]!=0L)
        {
          ScoreInfo[wUserChairID].cbType=(m_lUserWinScore[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
          ScoreInfo[wUserChairID].lRevenue=m_lUserRevenue[wUserChairID];
          ScoreInfo[wUserChairID].lScore=m_lUserWinScore[wUserChairID];
        }
        //�����
        if(!pIServerUserItem->IsAndroidUser())
        {
          TempStartScore -= m_lUserWinScore[wUserChairID];
        }
      }
      if(m_pServerContro!=NULL&&m_bControl)
      {
        m_lControlStorage=TempStartScore;
        CString cs;
        cs.Format(TEXT("�������Ϊ��%I64d���˺�Ϊ��%s"),m_lControlStorage,m_szControlName);
        CTraceService::TraceString(cs,TraceLevel_Exception);
      }
      m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

      //������Ϸ
      m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

      //ReadConfigInformation(false);

      //�л�ׯ��
      ChangeBanker(false);

      //����ʱ��
      m_dwJettonTime=(DWORD)time(NULL);
      m_pITableFrame->SetGameTimer(IDI_FREE,m_cbFreeTime*1000,1,0L);

      //������Ϣ
      CMD_S_GameFree GameFree;
      ZeroMemory(&GameFree,sizeof(GameFree));

      GameFree.cbTimeLeave = m_cbFreeTime;
      GameFree.lStorageStart=m_lStorageCurrent;
      GameFree.nListUserCount=m_ApplyUserArray.GetCount()-1;
      m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
      m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));

      return true;
    }
  }

  return false;
}

//��Ϸ��Ϣ����
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{

  switch(wSubCmdID)
  {
    case SUB_C_PLACE_JETTON:    //�û���ע
    {
      //Ч������
      ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
      if(wDataSize!=sizeof(CMD_C_PlaceJetton))
      {
        return false;
      }

      //�û�Ч��
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      //��Ϣ����
      CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pData;
      return OnUserPlaceJetton(pUserData->wChairID,pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
    }
    case SUB_C_APPLY_BANKER:    //������ׯ
    {
      //�û�Ч��
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus==US_LOOKON)
      {
        return true;
      }

      return OnUserApplyBanker(pIServerUserItem);
    }
    case SUB_C_CANCEL_BANKER:   //ȡ����ׯ
    {
      //�û�Ч��
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus==US_LOOKON)
      {
        return true;
      }

      return OnUserCancelBanker(pIServerUserItem);
    }
    case SUB_C_CONTINUE_CARD:   //��������
    {
      //�û�Ч��
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus==US_LOOKON)
      {
        return true;
      }
      if(pUserData->wChairID!=m_wCurrentBanker)
      {
        return true;
      }
      if(m_cbLeftCardCount < 8)
      {
        return true;
      }

      //���ñ���
      m_bContiueCard=true;

      //������Ϣ
      SendGameMessage(pUserData->wChairID,TEXT("���óɹ�����һ�ֽ��������ƣ�"));

      return true;
    }
    case SUB_C_CHECK_IMAGE:   //�û�ѡ�񱳾�ͼ
    {
      ASSERT(wDataSize==sizeof(CMD_C_CheckImage));
      if(wDataSize!=sizeof(CMD_C_CheckImage))
      {
        return false;
      }

      CMD_C_CheckImage * pCheckImage=(CMD_C_CheckImage *)pData;
      this->m_CheckImage = pCheckImage->Index;
      m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CHECK_IMAGE, pCheckImage, sizeof(CMD_C_CheckImage));
      m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CHECK_IMAGE, pCheckImage, sizeof(CMD_C_CheckImage));
      return true;

    }
    case SUB_C_ADMIN_COMMDN:
    {
      ASSERT(wDataSize==sizeof(CMD_C_ControlApplication));
      if(wDataSize!=sizeof(CMD_C_ControlApplication))
      {
        return false;
      }

      //Ȩ���ж�
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
      {
        return false;
      }
      if(m_pServerContro == NULL)
      {
        return false;
      }
      CopyMemory(m_szControlName,pIServerUserItem->GetNickName(),sizeof(m_szControlName));

      return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame);
    }
    case SUB_C_UPDATE_STORAGE:
    {
      ASSERT(wDataSize==sizeof(CMD_C_FreshStorage));
      if(wDataSize!=sizeof(CMD_C_FreshStorage))
      {
        return false;
      }

      //Ȩ���ж�
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
      {
        return false;
      }

      CMD_C_FreshStorage *FreshStorage=(CMD_C_FreshStorage *)pData;
      m_lStorageCurrent=FreshStorage->lStorageCurrent;
      m_StorageDeduct = FreshStorage->lStorageDeduct;
      m_lStorageMax1 =FreshStorage->lStorageMax1;
      m_lStorageMax2 =FreshStorage->lStorageMax2;
      m_lStorageMul1 =FreshStorage->lStorageMul1;
      m_lStorageMul2 =FreshStorage->lStorageMul2;
      if(m_StorageList.size() >= 2)
      {
        m_StorageList.pop_front();
      }
      m_StorageList.push_back(m_lStorageCurrent);


      CString pszString;
      CTime time = CTime::GetCurrentTime();
      pszString.Format(TEXT("\nʱ��: %d-%d-%d %d:%d:%d, ����Ѹ��� "),time.GetYear(), time.GetMonth(), time.GetDay(),
                       time.GetHour(), time.GetMinute(), time.GetSecond());
      //������������
      char* old_locale = _strdup(setlocale(LC_CTYPE,NULL));
      setlocale(LC_CTYPE, "chs");
      CStdioFile myFile;
      CString strFileName;
      strFileName.Format(TEXT("���������Ƽ�¼.txt"));
      BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
      if(bOpen)
      {
        myFile.SeekToEnd();
        myFile.WriteString(pszString);
        myFile.Flush();
        myFile.Close();
      }

      //��ԭ�����趨
      setlocale(LC_CTYPE, old_locale);
      free(old_locale);


      return true;
    }
    case SUB_C_DEUCT:
    {
      ASSERT(wDataSize==sizeof(CMD_C_FreshDeuct));
      if(wDataSize!=sizeof(CMD_C_FreshDeuct))
      {
        return false;
      }

      //Ȩ���ж�
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
      {
        return false;
      }

      CMD_C_FreshDeuct *FreshDeuct=(CMD_C_FreshDeuct *)pData;
      m_StorageDeduct=FreshDeuct->lStorageDeuct;

      return true;
    }
  }

  return false;
}

//�����Ϣ����
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
  return false;
}

//�����¼�
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
  return false;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
  //����ʱ��
  if((bLookonUser==false)&&(m_dwJettonTime==0L))
  {
    m_dwJettonTime=(DWORD)time(NULL);
    m_pITableFrame->SetGameTimer(IDI_FREE,m_cbFreeTime*1000,1,NULL);
    m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
  }

  ////������ʾ
  //TCHAR szTipMsg[128];
  //_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d"),m_lApplyBankerCondition,
  //  m_lAreaLimitScore,m_lUserLimitScore);
  //m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);

  return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
  //��¼�ɼ�
  if(bLookonUser==false)
  {
    m_wBankerUser=INVALID_CHAIR;
    m_lBankerGold=0l;
    //�л�ׯ��
    if(pIServerUserItem->GetChairID()==m_wCurrentBanker)
    {
      ChangeBanker(true);
      m_bContiueCard=false;
    }

    //ȡ������
    for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
    {
      if(pIServerUserItem->GetChairID()!=m_ApplyUserArray[i])
      {
        continue;
      }

      //ɾ�����
      m_ApplyUserArray.RemoveAt(i);

      //�������
      CMD_S_CancelBanker CancelBanker;
      ZeroMemory(&CancelBanker,sizeof(CancelBanker));

      //���ñ���
      CancelBanker.wCancelUser=wChairID;

      //������Ϣ
      m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
      m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

      break;
    }

    return true;
  }

  return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore)
{
  //Ч�����
  ASSERT((cbJettonArea<=AREA_COUNT+1 && cbJettonArea>=1)&&(lJettonScore>0L));
  if((cbJettonArea>=AREA_COUNT+1)||(lJettonScore<=0L) || cbJettonArea<1)
  {
    return false;
  }

  if(m_pITableFrame->GetGameStatus()!=GS_PLACE_JETTON)
  {
    CMD_S_PlaceJettonFail PlaceJettonFail;
    ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
    PlaceJettonFail.lJettonArea=cbJettonArea;
    PlaceJettonFail.lPlaceScore=lJettonScore;
    PlaceJettonFail.wPlaceUser=wChairID;

    //������Ϣ
    m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
    return true;
  }

  //ׯ���ж�
  if(m_wCurrentBanker==wChairID)
  {
    return true;
  }
  if(m_bEnableSysBanker==false && m_wCurrentBanker==INVALID_CHAIR)
  {
    return true;
  }

  //��������
  IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
  LONGLONG lJettonCount=0L;
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];
  }

  //��һ���
  LONGLONG lUserScore = pIServerUserItem->GetUserScore();

  //�Ϸ�У��
  if(lUserScore < lJettonCount + lJettonScore)
  {
    return true;
  }
  if(m_lUserLimitScore < lJettonCount + lJettonScore)
  {
    return true;
  }

  //�ɹ���ʶ
  bool bPlaceJettonSuccess=true;

  //�Ϸ���֤
  if(GetUserMaxJetton(wChairID,cbJettonArea) >= lJettonScore)
  {
    //��������֤
    if(pIServerUserItem->IsAndroidUser())
    {
      //��������
      if(m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
      {
        return true;
      }

      //��Ŀ����
      bool bHaveChip = false;
      for(int i = 0; i <= AREA_COUNT; i++)
      {
        if(m_lUserJettonScore[i+1][wChairID] != 0)
        {
          bHaveChip = true;
        }
      }

      if(!bHaveChip)
      {
        if(m_nChipRobotCount+1 > m_nMaxChipRobot)
        {
          bPlaceJettonSuccess = false;
        }
        else
        {
          m_nChipRobotCount++;
        }
      }

      //ͳ�Ʒ���
      if(bPlaceJettonSuccess)
      {
        m_lRobotAreaScore[cbJettonArea] += lJettonScore;
      }
    }

    if(bPlaceJettonSuccess)
    {
      //������ע
      m_lAllJettonScore[cbJettonArea] += lJettonScore;
      m_lUserJettonScore[cbJettonArea][wChairID] += lJettonScore;
    }
  }
  else
  {
    bPlaceJettonSuccess=false;
  }

  if(bPlaceJettonSuccess)
  {
    //��������
    CMD_S_PlaceJetton PlaceJetton;
    ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

    //�������
    PlaceJetton.wChairID=wChairID;
    PlaceJetton.cbJettonArea=cbJettonArea;
    PlaceJetton.lJettonScore=lJettonScore;

    //��ȡ�û�
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(pIServerUserItem != NULL)
    {
      PlaceJetton.cbAndroid = pIServerUserItem->IsAndroidUser()? TRUE : FALSE;
    }
    PlaceJetton.bAndroid = pIServerUserItem->IsAndroidUser()? TRUE : FALSE;

    //������Ϣ
    m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
    m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
  }
  else
  {
    CMD_S_PlaceJettonFail PlaceJettonFail;
    ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
    PlaceJettonFail.lJettonArea=cbJettonArea;
    PlaceJettonFail.lPlaceScore=lJettonScore;
    PlaceJettonFail.wPlaceUser=wChairID;

    //������Ϣ
    m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
  }

  return true;
}
void CTableFrameSink::RandList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{

  //����׼��
  BYTE *cbCardData = new BYTE[cbBufferCount];
  CopyMemory(cbCardData,cbCardBuffer,cbBufferCount);

  //�����˿�
  BYTE cbRandCount=0,cbPosition=0;
  do
  {
    cbPosition=rand()%(cbBufferCount-cbRandCount);
    cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
    cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
  }
  while(cbRandCount<cbBufferCount);

  delete []cbCardData;
  cbCardData = NULL;

  return;
}


//�����˿�
bool CTableFrameSink::DispatchTableCard()
{
  INT cbControlArea[32] = { 1, 9, 1, 9,   3, 11, 3, 11,   5, 13, 5, 13,   7, 15, 7, 15,   2, 10, 2, 10,   4, 12, 4, 12,   6, 14, 6, 14,   8, 16, 8, 16 };
  INT cbnChance[32]   = { 1, 1, 1, 1,   1, 1, 1, 1,    1, 1, 1, 1,   2,  2,  2,  2,   12,12, 12, 12,  12, 12, 12, 12,  12, 12, 12, 12,  12, 12, 12, 12 };

  m_GameLogic.ChaosArray(cbControlArea, CountArray(cbControlArea), cbnChance, CountArray(cbnChance));

  //�������
  DWORD wTick = GetTickCount();

  //���ʺ�ֵ
  INT nChanceAndValue = 0;
  for(int n = 0; n < CountArray(cbnChance); ++n)
  {
    nChanceAndValue += cbnChance[n];
  }

  INT nMuIndex = 0;
  int nRandNum = 0;         //�������
  static int nStFluc = 1;
  nRandNum = (rand() + wTick + nStFluc*3) % nChanceAndValue;
  for(int j = 0; j < CountArray(cbnChance); j++)
  {
    nRandNum -= cbnChance[j];
    if(nRandNum < 0)
    {
      nMuIndex = j;
      break;
    }
  }
  nStFluc = nStFluc%3 + 1;

  m_cbTableCardArray[0][0] = cbControlArea[nMuIndex];
  m_cbCardCount[0] = 1;

  //���Ʊ�־
  m_bContiueCard = false;

  return true;
}

//����ׯ��
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
  //�Ϸ��ж�
  LONGLONG lUserScore=pIApplyServerUserItem->GetUserScore();
  if(lUserScore<m_lApplyBankerCondition)
  {
    m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("��Ľ�Ҳ���������ׯ�ң�����ʧ�ܣ�"),SMT_CHAT|SMT_EJECT);
    return true;
  }

  //�����ж�
  WORD wApplyUserChairID=pIApplyServerUserItem->GetChairID();
  for(INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
  {
    WORD wChairID=m_ApplyUserArray[nUserIdx];
    if(wChairID==wApplyUserChairID)
    {
      m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("���Ѿ�������ׯ�ң�����Ҫ�ٴ����룡"),SMT_CHAT|SMT_EJECT);
      return true;
    }
  }

  if(pIApplyServerUserItem->IsAndroidUser()&&(m_ApplyUserArray.GetCount())>m_nRobotListMaxCount)
  {
    return true;
  }

  //������Ϣ
  m_ApplyUserArray.Add(wApplyUserChairID);

  //�������
  CMD_S_ApplyBanker ApplyBanker;
  ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));

  //���ñ���
  ApplyBanker.wApplyUser=wApplyUserChairID;

  //������Ϣ
  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

  //�л��ж�
  if(m_pITableFrame->GetGameStatus()==GAME_STATUS_FREE && m_ApplyUserArray.GetCount()==1)
  {
    ChangeBanker(false);
  }


  return true;
}

//ȡ������
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
  //��ǰׯ��
  if(pICancelServerUserItem->GetChairID()==m_wCurrentBanker && m_pITableFrame->GetGameStatus()!=GAME_STATUS_FREE)
  {
    //������Ϣ
    m_pITableFrame->SendGameMessage(pICancelServerUserItem,TEXT("��Ϸ�Ѿ���ʼ��������ȡ����ׯ��"),SMT_CHAT|SMT_EJECT);
    return true;
  }

  //�����ж�
  for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
  {

    //��ȡ���
    WORD wChairID=m_ApplyUserArray[i];
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

    //��������
    if(pIServerUserItem==NULL)
    {
      continue;
    }
    if(pIServerUserItem->GetUserID()!=pICancelServerUserItem->GetUserID())
    {
      continue;
    }

    //ɾ�����
    m_ApplyUserArray.RemoveAt(i);

    if(m_wCurrentBanker!=wChairID)
    {
      //�������
      CMD_S_CancelBanker CancelBanker;
      ZeroMemory(&CancelBanker,sizeof(CancelBanker));

      //���ñ���
      CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

      //������Ϣ
      m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
      m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
    }
    else if(m_wCurrentBanker==wChairID)
    {
      //�л�ׯ��
      m_wCurrentBanker=INVALID_CHAIR;
      ChangeBanker(true);
    }

    return true;
  }

  return true;
}

//����ׯ��
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
  //�л���ʶ
  bool bChangeBanker=false;

  //ȡ����ǰ
  if(bCancelCurrentBanker)
  {
    for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
    {
      //��ȡ���
      WORD wChairID=m_ApplyUserArray[i];

      //��������
      if(wChairID!=m_wCurrentBanker)
      {
        continue;
      }

      //ɾ�����
      m_ApplyUserArray.RemoveAt(i);

      break;
    }

    //����ׯ��
    m_wCurrentBanker=INVALID_CHAIR;

    //�ֻ��ж�
    TakeTurns();

    //���ñ���
    bChangeBanker=true;
    m_bExchangeBanker = true;
  }
  //��ׯ�ж�
  else if(m_wCurrentBanker!=INVALID_CHAIR)
  {
    //��ȡׯ��
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

    if(pIServerUserItem!= NULL)
    {
      LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

      //�����ж�
      if(m_lPlayerBankerMAX<=m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
      {
        //ׯ�������ж� ͬһ��ׯ�������ֻ�ж�һ��
        if(m_lPlayerBankerMAX <= m_wBankerTime && m_bExchangeBanker && lBankerScore>=m_lApplyBankerCondition)
        {
          //��ׯ�������ã���ׯ�������趨�ľ���֮��(m_lBankerMAX)��
          //�������ֵ��������������ׯ���б�����������ҽ��ʱ��
          //�����ټ���ׯm_lBankerAdd�֣���ׯ���������á�

          //��ҳ���m_lBankerScoreMAX֮��
          //������������ҵĽ��ֵ�������Ľ��ֵ����Ҳ�����ټ�ׯm_lBankerScoreAdd�֡�
          bool bScoreMAX = true;
          m_bExchangeBanker = false;

          for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
          {
            //��ȡ���
            WORD wChairID = m_ApplyUserArray[i];
            IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
            LONGLONG lScore = pIServerUserItem->GetUserScore();

            if(wChairID != m_wCurrentBanker && lBankerScore <= lScore)
            {
              bScoreMAX = false;
              break;
            }
          }

          if(bScoreMAX || (lBankerScore > m_lBankerScoreMAX && m_lBankerScoreMAX != 0l))
          {
            if(bScoreMAX)
            {
              m_lPlayerBankerMAX += m_lBankerAdd;
            }
            if(lBankerScore > m_lBankerScoreMAX && m_lBankerScoreMAX != 0l)
            {
              m_lPlayerBankerMAX += m_lBankerScoreAdd;
            }
            return true;
          }
        }

        //�������
        for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
        {
          //��ȡ���
          WORD wChairID=m_ApplyUserArray[i];

          //��������
          if(wChairID!=m_wCurrentBanker)
          {
            continue;
          }

          //ɾ�����
          m_ApplyUserArray.RemoveAt(i);

          break;
        }

        //����ׯ��
        m_wCurrentBanker=INVALID_CHAIR;

        //�ֻ��ж�
        TakeTurns();

        bChangeBanker=true;
        m_bExchangeBanker = true;

        //��ʾ��Ϣ
        TCHAR szTipMsg[128];
        if(lBankerScore<m_lApplyBankerCondition)
        {
          _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��������(%I64d)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),m_lApplyBankerCondition);
        }
        else
        {
          _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��ׯ�����ﵽ(%d)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),m_lPlayerBankerMAX);
        }

        //������Ϣ
        SendGameMessage(INVALID_CHAIR,szTipMsg);
      }
    }
    else
    {
      for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
      {
        //��ȡ���
        WORD wChairID=m_ApplyUserArray[i];

        //��������
        if(wChairID!=m_wCurrentBanker)
        {
          continue;
        }

        //ɾ�����
        m_ApplyUserArray.RemoveAt(i);

        break;
      }
      //����ׯ��
      m_wCurrentBanker=INVALID_CHAIR;
    }

  }
  //ϵͳ��ׯ
  else if(m_wCurrentBanker==INVALID_CHAIR && m_ApplyUserArray.GetCount()!=0)
  {
    //�ֻ��ж�
    TakeTurns();

    bChangeBanker=true;
    m_bExchangeBanker = true;
  }

  //�л��ж�
  if(bChangeBanker)
  {
    //�����ׯ��
    m_lPlayerBankerMAX = m_lBankerMAX;

    //���ñ���
    m_wBankerTime = 0;
    m_lBankerWinScore=0;

    //������Ϣ
    CMD_S_ChangeBanker sChangeBanker;
    ZeroMemory(&sChangeBanker,sizeof(sChangeBanker));
    sChangeBanker.wBankerUser=m_wCurrentBanker;
    sChangeBanker.lBankerScore = 1000000000;
    if(m_wCurrentBanker!=INVALID_CHAIR)
    {
      IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
      sChangeBanker.lBankerScore=pIServerUserItem->GetUserScore();
    }
    m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&sChangeBanker,sizeof(sChangeBanker));
    m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&sChangeBanker,sizeof(sChangeBanker));

    if(m_wCurrentBanker!=INVALID_CHAIR)
    {
      //��ȡ��Ϣ
      LONGLONG lMessageCount=GetPrivateProfileInt(m_szGameRoomName,TEXT("MessageCount"),0,m_szConfigFileName);
      if(lMessageCount!=0)
      {
        //��ȡ����
        LONGLONG lIndex=rand()%lMessageCount;
        TCHAR szKeyName[32],szMessage1[256],szMessage2[256];
        _sntprintf(szKeyName,CountArray(szKeyName),TEXT("Item%I64d"),lIndex);
        GetPrivateProfileString(m_szGameRoomName,szKeyName,TEXT("��ϲ[ %s ]��ׯ"),szMessage1,CountArray(szMessage1),m_szConfigFileName);

        //��ȡ���
        IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

        //������Ϣ
        _sntprintf(szMessage2,CountArray(szMessage2),szMessage1,pIServerUserItem->GetNickName());
        SendGameMessage(INVALID_CHAIR,szMessage2);
      }
    }
  }

  return bChangeBanker;
}

//�ֻ��ж�
void CTableFrameSink::TakeTurns()
{
  //��������
  int nInvalidApply = 0;

  for(int i = 0; i < m_ApplyUserArray.GetCount(); i++)
  {
    if(m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
    {
      //��ȡ����
      IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray[i]);
      if(pIServerUserItem != NULL)
      {
        if(pIServerUserItem->GetUserScore() >= m_lApplyBankerCondition)
        {
          m_wCurrentBanker=m_ApplyUserArray[i];
          break;
        }
        else
        {
          nInvalidApply = i + 1;

          //������Ϣ
          CMD_S_CancelBanker CancelBanker = {};

          //���ñ���
          CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

          //������Ϣ
          m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
          m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

          //��ʾ��Ϣ
          TCHAR szTipMsg[128] = {};
          _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("������Ľ������%I64d��������ׯ����������%I64d��,���޷���ׯ��"),
                     pIServerUserItem->GetUserScore(), m_lApplyBankerCondition);
          SendGameMessage(m_ApplyUserArray[i],szTipMsg);
        }
      }
    }
  }

  //ɾ�����
  if(nInvalidApply != 0)
  {
    m_ApplyUserArray.RemoveAt(0, nInvalidApply);
  }
}

//����ׯ��
void CTableFrameSink::SendApplyUser(IServerUserItem *pRcvServerUserItem)
{
  for(INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
  {
    WORD wChairID=m_ApplyUserArray[nUserIdx];

    //��ȡ���
    IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(!pServerUserItem)
    {
      continue;
    }

    //ׯ���ж�
    if(pServerUserItem->GetChairID()==m_wCurrentBanker)
    {
      continue;
    }

    //�������
    CMD_S_ApplyBanker ApplyBanker;
    ApplyBanker.wApplyUser=wChairID;

    //������Ϣ
    m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
  }
}

//�û�����
bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem)
{
  //�л�ׯ��
  if(pIServerUserItem->GetChairID()==m_wCurrentBanker)
  {
    ChangeBanker(true);
  }

  //ȡ������
  for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
  {
    if(pIServerUserItem->GetChairID()!=m_ApplyUserArray[i])
    {
      continue;
    }

    //ɾ�����
    m_ApplyUserArray.RemoveAt(i);

    //�������
    CMD_S_CancelBanker CancelBanker;
    ZeroMemory(&CancelBanker,sizeof(CancelBanker));

    //���ñ���
    CancelBanker.wCancelUser=wChairID;

    //������Ϣ
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

    break;
  }

  return true;
}
void   CTableFrameSink::GetAllWinArea(BYTE bcWinArea[],BYTE bcAreaCount,BYTE InArea)
{
  if(InArea==0xFF)
  {
    return ;
  }
  ZeroMemory(bcWinArea,bcAreaCount);


  LONGLONG lMaxSocre = 0;

  for(int i = 0; i<32; i++)
  {
    BYTE bcOutCadDataWin[AREA_COUNT];
    BYTE bcData[1];
    bcData[0]=i+1;
    m_GameLogic.GetCardType(bcData,1,bcOutCadDataWin);
    for(int j= 0; j<=AREA_COUNT; j++)
    {

      if(bcOutCadDataWin[j]>1&&j==InArea-1)
      {
        LONGLONG Score = 0;
        for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
        {
          if(bcOutCadDataWin[nAreaIndex-1]>1)
          {
            Score += m_lAllJettonScore[nAreaIndex]*(bcOutCadDataWin[nAreaIndex-1]);
          }
        }
        if(Score>=lMaxSocre)
        {
          lMaxSocre = Score;
          CopyMemory(bcWinArea,bcOutCadDataWin,bcAreaCount);

        }
        break;
      }
    }
  }
}
//�����ע
LONGLONG CTableFrameSink::GetUserMaxJetton(WORD wChairID,BYTE Area)
{
  IServerUserItem *pIMeServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
  if(NULL==pIMeServerUserItem)
  {
    return 0L;
  }


  //����ע��
  LONGLONG lNowJetton = 0;
  ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];
  }

  //ׯ�ҽ��
  LONGLONG lBankerScore=1000000000;
  if(m_wCurrentBanker!=INVALID_CHAIR)
  {
    IServerUserItem *pIUserItemBanker=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
    if(NULL!=pIUserItemBanker)
    {
      lBankerScore=pIUserItemBanker->GetUserScore();
    }
  }

  BYTE bcWinArea[AREA_COUNT];
  LONGLONG LosScore = 0;
  LONGLONG WinScore = 0;

  GetAllWinArea(bcWinArea,AREA_COUNT,Area);

  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    if(bcWinArea[nAreaIndex-1]>1)
    {
      LosScore+=m_lAllJettonScore[nAreaIndex]*(bcWinArea[nAreaIndex-1]);
    }
    else
    {
      if(bcWinArea[nAreaIndex-1]==0)
      {
        WinScore+=m_lAllJettonScore[nAreaIndex];

      }
    }
  }
  lBankerScore = lBankerScore + WinScore - LosScore;

  if(lBankerScore < 0)
  {
    if(m_wCurrentBanker!=INVALID_CHAIR)
    {
      IServerUserItem *pIUserItemBanker=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
      if(NULL!=pIUserItemBanker)
      {
        lBankerScore=pIUserItemBanker->GetUserScore();
      }
    }
    else
    {
      lBankerScore = 1000000000;
    }
  }

  //��������
  LONGLONG lMeMaxScore = min((pIMeServerUserItem->GetUserScore()-lNowJetton), m_lUserLimitScore);

  //��������
  lMeMaxScore=min(lMeMaxScore,m_lAreaLimitScore);

  BYTE diMultiple[AREA_COUNT];

  for(int i = 0; i<32; i++)
  {
    BYTE bcData[1];
    bcData[0]= i+1;
    BYTE  bcOutCadDataWin[AREA_COUNT];
    m_GameLogic.GetCardType(bcData,1,bcOutCadDataWin);
    for(int j = 0; j<=AREA_COUNT; j++)
    {
      if(bcOutCadDataWin[j]>1)
      {
        diMultiple[j] = bcOutCadDataWin[j];

      }
    }
  }
  //ׯ������
  lMeMaxScore=min(lMeMaxScore,lBankerScore/(diMultiple[Area-1]));

  //��������
  ASSERT(lMeMaxScore >= 0);
  lMeMaxScore = max(lMeMaxScore, 0);

  return (LONGLONG)(lMeMaxScore);
}
//����÷�
LONGLONG CTableFrameSink::CalculateScore()
{
  //��������
  LONGLONG static cbRevenue=m_pGameServiceOption->wRevenueRatio;

  //�ƶ����
  bool static bWinTianMen, bWinDiMen, bWinXuanMen,bWinHuang;
  BYTE TianMultiple,diMultiple,TianXuanltiple,HuangMultiple;
  TianMultiple  = 1;
  diMultiple = 1 ;
  TianXuanltiple = 1;
  HuangMultiple = 1;

  BYTE  bcResulteOut[AREA_COUNT];
  memset(bcResulteOut,0,AREA_COUNT);
  m_GameLogic.GetCardType(&m_cbTableCardArray[0][0],1,bcResulteOut);

  //��Ϸ��¼
  tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];

  BYTE  cbMultiple[AREA_COUNT]= {1};

  for(WORD wAreaIndex = 1; wAreaIndex <= AREA_COUNT; ++wAreaIndex)
  {

    if(bcResulteOut[wAreaIndex-1]>0)
    {
      GameRecord.bWinMen[wAreaIndex-1] = 4;
    }
    else
    {
      GameRecord.bWinMen[wAreaIndex-1] = 0;
    }
  }

  //�ƶ��±�
  m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
  if(m_nRecordLast == m_nRecordFirst)
  {
    m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
  }

  //ׯ������
  LONGLONG lBankerWinScore = 0;

  //��ҳɼ�
  ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
  ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
  ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
  LONGLONG lUserLostScore[GAME_PLAYER];
  ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

  //�����ע
  LONGLONG *pUserScore[AREA_COUNT+1];
  pUserScore[0]=NULL;
  for(int i = 1; i<=AREA_COUNT; i++)
  {
    pUserScore[i]=m_lUserJettonScore[i];
  }

  //�������
  for(WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
  {
    //ׯ���ж�
    if(m_wCurrentBanker==wChairID)
    {
      continue;
    }

    //��ȡ�û�
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(pIServerUserItem==NULL)
    {
      continue;
    }

    for(WORD wAreaIndex = 1; wAreaIndex <= AREA_COUNT; ++wAreaIndex)
    {

      if(bcResulteOut[wAreaIndex-1]>0)
      {
        m_lUserWinScore[wChairID] += (pUserScore[wAreaIndex][wChairID] *(bcResulteOut[wAreaIndex-1])) ;
        m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID] ;
        lBankerWinScore -= (pUserScore[wAreaIndex][wChairID] * (bcResulteOut[wAreaIndex-1])) ;
      }
      else
      {
        if(bcResulteOut[wAreaIndex-1]==0)
        {
          lUserLostScore[wChairID] -= pUserScore[wAreaIndex][wChairID];
          lBankerWinScore += pUserScore[wAreaIndex][wChairID];
        }
        else
        {
          //���Ϊ1���ٷ�
          m_lUserWinScore[wChairID] += 0;
          m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID] ;
        }
      }
    }

    //�ܵķ���
    m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
  }

  //ׯ�ҳɼ�
  if(m_wCurrentBanker!=INVALID_CHAIR)
  {
    m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;
  }
  //����˰��
  float fRevenuePer=(float)cbRevenue/1000;

  for(WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
  {
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(pIServerUserItem==NULL)
    {
      continue;
    }
    if(wChairID != m_wCurrentBanker && m_lUserWinScore[wChairID] > 0)
    {
      m_lUserRevenue[wChairID]  = LONGLONG(m_lUserWinScore[wChairID]*fRevenuePer+0.5);
      m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
    }
    else if(m_wCurrentBanker!=INVALID_CHAIR && wChairID == m_wCurrentBanker && lBankerWinScore > 0)
    {
      m_lUserRevenue[m_wCurrentBanker]  = LONGLONG(m_lUserWinScore[m_wCurrentBanker]*fRevenuePer+0.5);
      m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
      lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
    }
  }

  //�ۼƻ���
  m_lBankerWinScore += lBankerWinScore;

  //��ǰ����
  m_lBankerCurGameScore=lBankerWinScore;

  return lBankerWinScore;
}
LONGLONG CTableFrameSink::JudgeSystemScore()
{

  BYTE  bcResulteOut[AREA_COUNT];
  memset(bcResulteOut,0,AREA_COUNT);
  m_GameLogic.GetCardType(&m_cbTableCardArray[0][0],1,bcResulteOut);

  //ϵͳ��Ӯ
  LONGLONG lSystemScore = 0l;

  //�����ע
  LONGLONG *pUserScore[AREA_COUNT+1];
  pUserScore[0] = NULL;
  for(int i = 1; i<=AREA_COUNT; i++)
  {
    pUserScore[i] = m_lUserJettonScore[i];
  }

  //ׯ���ǲ��ǻ�����
  bool bIsBankerAndroidUser = false;
  if(m_wCurrentBanker != INVALID_CHAIR)
  {
    IServerUserItem * pIBankerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
    if(pIBankerUserItem != NULL)
    {
      bIsBankerAndroidUser = pIBankerUserItem->IsAndroidUser();
    }
  }

  //�������
  for(WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
  {
    //ׯ���ж�
    if(m_wCurrentBanker == wChairID)
    {
      continue;
    }

    //��ȡ�û�
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(pIServerUserItem==NULL)
    {
      continue;
    }

    bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

    for(WORD wAreaIndex = 1; wAreaIndex <= AREA_COUNT; ++wAreaIndex)
    {

      if(bcResulteOut[wAreaIndex-1]>0)
      {
        if(bIsAndroidUser)
        {
          lSystemScore += (pUserScore[wAreaIndex][wChairID] *(bcResulteOut[wAreaIndex-1]));
        }

        if(m_wCurrentBanker == INVALID_CHAIR || bIsBankerAndroidUser)
        {
          lSystemScore -= (pUserScore[wAreaIndex][wChairID] *(bcResulteOut[wAreaIndex-1]));
        }
      }
      else
      {
        if(bcResulteOut[wAreaIndex-1]==0)
        {
          if(bIsAndroidUser)
          {
            lSystemScore -= pUserScore[wAreaIndex][wChairID];
          }

          if(m_wCurrentBanker == INVALID_CHAIR || bIsBankerAndroidUser)
          {
            lSystemScore += pUserScore[wAreaIndex][wChairID];
          }
        }
      }
    }
  }
  //������Ӯ�Ŀ�治����
  //m_lStorageCurrent += lSystemScore;

  if(m_StorageList.size() >= 2)
  {
    m_StorageList.pop_front();
  }
  m_StorageList.push_back(m_lStorageCurrent);
  IT it = m_StorageList.rbegin();
  IT itTmp = it;
  SCORE StorageTurn = *(++itTmp);
  ASSERT(StorageTurn == m_lStorageCurrent);
  WriteStorageInfo(StorageTurn,lSystemScore,TRUE);
  return lSystemScore;
}
//��̽���ж�
bool CTableFrameSink::ProbeJudge(bool& bSystemLost)
{
  BYTE  bcResulteOut[AREA_COUNT];
  memset(bcResulteOut,0,AREA_COUNT);
  //��ȡӮ������
  m_GameLogic.GetCardType(&m_cbTableCardArray[0][0],1,bcResulteOut);

  //ϵͳ��Ӯ
  LONGLONG lSystemScore = 0l;

  //�����ע
  LONGLONG *pUserScore[AREA_COUNT+1];
  pUserScore[0] = NULL;
  for(int i = 1; i<=AREA_COUNT; i++)
  {
    pUserScore[i] = m_lUserJettonScore[i];
  }

  //ׯ���ǲ��ǻ�����
  bool bIsBankerAndroidUser = false;
  if(m_wCurrentBanker != INVALID_CHAIR)
  {
    IServerUserItem * pIBankerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
    if(pIBankerUserItem != NULL)
    {
      bIsBankerAndroidUser = pIBankerUserItem->IsAndroidUser();
    }
  }

  //�������
  for(WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
  {
    //ׯ���ж�
    if(m_wCurrentBanker == wChairID)
    {
      continue;
    }

    //��ȡ�û�
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(pIServerUserItem==NULL)
    {
      continue;
    }

    bool bIsAndroidUser = pIServerUserItem->IsAndroidUser();

    for(WORD wAreaIndex = 1; wAreaIndex <= AREA_COUNT; ++wAreaIndex)
    {

      if(bcResulteOut[wAreaIndex-1]>0)
      {
        if(bIsAndroidUser)
        {
          lSystemScore += (pUserScore[wAreaIndex][wChairID] *(bcResulteOut[wAreaIndex-1]));
        }

        if(m_wCurrentBanker == INVALID_CHAIR || bIsBankerAndroidUser)
        {
          lSystemScore -= (pUserScore[wAreaIndex][wChairID] *(bcResulteOut[wAreaIndex-1]));
        }
      }
      else
      {
        if(bcResulteOut[wAreaIndex-1]==0)
        {
          if(bIsAndroidUser)
          {
            lSystemScore -= pUserScore[wAreaIndex][wChairID];
          }

          if(m_wCurrentBanker == INVALID_CHAIR || bIsBankerAndroidUser)
          {
            lSystemScore += pUserScore[wAreaIndex][wChairID];
          }
        }
      }
    }
  }

  // ��治���䣬�ؿ� ,��ʼ������Ϊ0
  if(m_lStorageCurrent + lSystemScore <  0)
  {
    return false;
  }

  if(lSystemScore>0)
  {
    m_lStorageCurrent += lSystemScore;
    if(NeedDeductStorage() && m_lStorageCurrent > 0)
    {
      m_lStorageCurrent = m_lStorageCurrent - (m_lStorageCurrent * m_StorageDeduct / 1000);
    }
    if(m_StorageList.size() >= 2)
    {
      m_StorageList.pop_front();
    }
    m_StorageList.push_back(m_lStorageCurrent);
    IT it = m_StorageList.rbegin();
    IT itTmp = it;
    SCORE StorageTurn = *(++itTmp);
    SCORE StorageAdd = (*it) - StorageTurn;
    WriteStorageInfo(StorageTurn,StorageAdd,FALSE);
    return true;
  }
  else if(lSystemScore<0)
  {
    if(bSystemLost==true)
    {
      m_lStorageCurrent += lSystemScore;
      if(NeedDeductStorage() && m_lStorageCurrent > 0)
      {
        m_lStorageCurrent = m_lStorageCurrent - (m_lStorageCurrent * m_StorageDeduct / 1000);
      }
      if(m_StorageList.size() >= 2)
      {
        m_StorageList.pop_front();
      }
      m_StorageList.push_back(m_lStorageCurrent);
      IT it = m_StorageList.rbegin();
      IT itTmp = it;
      SCORE StorageTurn = *(++itTmp);
      SCORE StorageAdd = (*it) - StorageTurn;
      WriteStorageInfo(StorageTurn,StorageAdd,FALSE);
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    m_lStorageCurrent += lSystemScore;
    if(NeedDeductStorage() && m_lStorageCurrent > 0)
    {
      m_lStorageCurrent = m_lStorageCurrent - (m_lStorageCurrent * m_StorageDeduct / 1000);
    }
    if(m_StorageList.size() >= 2)
    {
      m_StorageList.pop_front();
    }
    m_StorageList.push_back(m_lStorageCurrent);
    IT it = m_StorageList.rbegin();
    IT itTmp = it;
    SCORE StorageTurn = *(++itTmp);
    SCORE StorageAdd = (*it) - StorageTurn;
    WriteStorageInfo(StorageTurn,StorageAdd,FALSE);
    return true;
  }
}

//���ͼ�¼
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{
  WORD wBufferSize=0;
  BYTE cbBuffer[SOCKET_TCP_BUFFER];
  int nIndex = m_nRecordFirst;
  while(nIndex != m_nRecordLast)
  {
    if((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
    {
      m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
      wBufferSize=0;
    }
    CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
    wBufferSize+=sizeof(tagServerGameRecord);

    nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
  }
  if(wBufferSize>0)
  {
    m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
  }
}

//������Ϣ
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
  if(wChairID==INVALID_CHAIR)
  {
    //��Ϸ���
    for(WORD i=0; i<GAME_PLAYER; ++i)
    {
      IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
      if(pIServerUserItem==NULL)
      {
        continue;
      }
      m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT);
    }

    //�Թ����
    WORD wIndex=0;
    do
    {
      IServerUserItem *pILookonServerUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
      if(pILookonServerUserItem==NULL)
      {
        break;
      }

      m_pITableFrame->SendGameMessage(pILookonServerUserItem,pszTipMsg,SMT_CHAT);

    }
    while(true);
  }
  else
  {
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(pIServerUserItem!=NULL)
    {
      m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT|SMT_EJECT);
    }
  }
}

//��ȡ����
void CTableFrameSink::ReadConfigInformation(bool bReadFresh)
{

  //��ȡ�Զ�������
  tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
  ASSERT(pCustomConfig);

  //��ׯ
  m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
  m_lBankerMAX = pCustomConfig->CustomGeneral.lBankerTime;
  m_lBankerAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
  m_lBankerScoreMAX = pCustomConfig->CustomGeneral.lBankerScoreMAX;
  m_lBankerScoreAdd = pCustomConfig->CustomGeneral.lBankerTimeExtra;
  m_bEnableSysBanker = (pCustomConfig->CustomGeneral.nEnableSysBanker == TRUE)?true:false;

  //ʱ��
  m_cbFreeTime = pCustomConfig->CustomGeneral.cbFreeTime;
  m_cbBetTime = pCustomConfig->CustomGeneral.cbBetTime;
  m_cbEndTime = pCustomConfig->CustomGeneral.cbEndTime;
  if(m_cbFreeTime < TIME_FREE || m_cbFreeTime > 99)
  {
    m_cbFreeTime = TIME_FREE;
  }
  if(m_cbBetTime < TIME_PLACE_JETTON || m_cbBetTime > 99)
  {
    m_cbBetTime = TIME_PLACE_JETTON;
  }
  if(m_cbEndTime < TIME_GAME_END || m_cbEndTime > 99)
  {
    m_cbEndTime = TIME_GAME_END;
  }

  //��ע
  m_lAreaLimitScore = pCustomConfig->CustomGeneral.lAreaLimitScore;
  m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;


  //���
  m_lStorageStart = pCustomConfig->CustomGeneral.StorageStart;
  m_lStorageCurrent = m_lStorageStart;
  m_StorageList.push_back(m_lStorageCurrent);
  m_StorageDeduct = pCustomConfig->CustomGeneral.StorageDeduct;
  m_lStorageMax1 = pCustomConfig->CustomGeneral.StorageMax1;
  m_lStorageMul1 = pCustomConfig->CustomGeneral.StorageMul1;
  m_lStorageMax2 = pCustomConfig->CustomGeneral.StorageMax2;
  m_lStorageMul2 = pCustomConfig->CustomGeneral.StorageMul2;
  if(m_lStorageMul1 < 0 || m_lStorageMul1 > 100)
  {
    m_lStorageMul1 = 50;
  }
  if(m_lStorageMul2 < 0 || m_lStorageMul2 > 100)
  {
    m_lStorageMul2 = 80;
  }

  //������
  m_nRobotListMaxCount = pCustomConfig->CustomAndroid.lRobotListMaxCount;

  LONGLONG lRobotBetMinCount = pCustomConfig->CustomAndroid.lRobotBetMinCount;
  LONGLONG lRobotBetMaxCount = pCustomConfig->CustomAndroid.lRobotBetMaxCount;
  m_nMaxChipRobot = rand()%(lRobotBetMaxCount-lRobotBetMinCount+1) + lRobotBetMinCount;
  if(m_nMaxChipRobot < 0)
  {
    m_nMaxChipRobot = 8;
  }
  m_lRobotAreaLimit = pCustomConfig->CustomAndroid.lRobotAreaLimit;

  //�������������
  CopyMemory(&m_CustomAndroidConfig,&(pCustomConfig->CustomAndroid),sizeof(m_CustomAndroidConfig));
//..............................................................................


  m_StorageDispatch=20;//�ѿ��İٷ�֮0--�ٷ�֮N֮�������һ���ٷֱ��ó���������
  m_lMinPercent=0;//�����ׯ���������Ͻ�ҵİٷֱ� �ٷ�֮N
  m_lMaxPercent=20;//�����ׯ��������Ͻ�ҵİٷֱ� �ٷ�֮N
  if(m_lMaxPercent<=0)
  {
    m_lMaxPercent=10;
  }


  if(m_lBankerScoreMAX <= m_lApplyBankerCondition)
  {
    m_lBankerScoreMAX = 0l;
  }

  m_lPlayerBankerMAX = m_lBankerMAX;
}

//////////////////////////////////////////////////////////////////////////
//���в���
#ifdef __SPECIAL___
bool __cdecl CTableFrameSink::OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem)
{
  return true;
}
#endif

//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pUserItem==NULL)
    {
      continue;
    }
    if(wChairID==i)
    {
      //������ע
      for(int nAreaIndex=0; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
      {

        if(m_lUserJettonScore[nAreaIndex][wChairID] != 0)
        {
          return true;
        }
      }
      break;
    }
  }
  if(wChairID==m_wCurrentBanker)
  {
    return true;
  }
  return false;
}

//�Ƿ�˥��
bool CTableFrameSink::NeedDeductStorage()
{

  for(int i = 0; i < GAME_PLAYER; ++i)
  {
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem == NULL)
    {
      continue;
    }

    if(!pIServerUserItem->IsAndroidUser())
    {
      for(int nAreaIndex=0; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
      {
        if(m_lUserJettonScore[nAreaIndex][i]!=0)
        {
          return true;
        }
      }
    }
  }

  return false;

}

// ��¼�������
void CTableFrameSink::WriteStorageInfo(SCORE lStorageTurn, SCORE lStorageAdd ,bool bControl)
{
  ASSERT(m_StorageList.size() == 2);
  CString pszString;
  CTime time = CTime::GetCurrentTime();

  //�ǿ���
  if(!bControl)
  {
    pszString.Format(TEXT("\nʱ��: %d-%d-%d %d:%d:%d,ϵͳ���: %I64d�����仯: %I64d "),time.GetYear(), time.GetMonth(), time.GetDay(),
                     time.GetHour(), time.GetMinute(), time.GetSecond(),lStorageTurn,lStorageAdd);
  }
  else
  {
    pszString.Format(TEXT("\nʱ��: %d-%d-%d %d:%d:%d,ϵͳ���: %I64d�����ƺ���仯: %I64d "),time.GetYear(), time.GetMonth(), time.GetDay(),
                     time.GetHour(), time.GetMinute(), time.GetSecond(),lStorageTurn,lStorageAdd);
  }

  //������������
  char* old_locale = _strdup(setlocale(LC_CTYPE,NULL));
  setlocale(LC_CTYPE, "chs");

  CStdioFile myFile;
  CString strFileName;
  strFileName.Format(TEXT("���������Ƽ�¼.txt"));
  BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
  if(bOpen)
  {
    myFile.SeekToEnd();
    myFile.WriteString(pszString);
    myFile.Flush();
    myFile.Close();
  }

  //��ԭ�����趨
  setlocale(LC_CTYPE, old_locale);
  free(old_locale);
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
  //��ׯ�ҷ����ڿ���ʱ��䶯ʱ(��ׯ�ҽ����˴�ȡ��)У��ׯ�ҵ���ׯ����
  if(wChairID == m_wCurrentBanker && m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
  {
    ChangeBanker(false);
  }

  return true;
}
//////////////////////////////////////////////////////////////////////////
