#include "Stdafx.h"
#include "GameClient.h"
#include "GameOption.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_FREE          99                  //����ʱ��
#define IDI_PLACE_JETTON      100                 //��עʱ��
#define IDI_DISPATCH_CARD     301                 //����ʱ��
#define IDI_OPEN_CARD       302                   //����ʱ��

#define IDI_ANDROID_BET       1000


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
  ON_MESSAGE(IDM_PLACE_JETTON,OnPlaceJetton)
  ON_MESSAGE(IDM_APPLY_BANKER, OnApplyBanker)
  ON_MESSAGE(IDM_CONTINUE_CARD, OnContinueCard)
  ON_MESSAGE(IDM_AUTO_OPEN_CARD, OnAutoOpenCard)
  ON_MESSAGE(IDM_OPEN_CARD, OnOpenCard)
  ON_MESSAGE(IDM_SHOW_RESULT,OnShowResult)
  ON_MESSAGE(IDM_SOUND,OnPlaySound)
  ON_MESSAGE(IDM_ADMIN_COMMDN, OnAdminControl)
  ON_MESSAGE(IDM_ADMIN_STORAGE,OnStorage)
  ON_MESSAGE(IDM_ADMIN_DEUCT,OnDeuct)
  ON_MESSAGE(IDM_GAME_SOUND,OnMessageGameSound)
  ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//���캯��
CGameClientEngine::CGameClientEngine()
{
  //������Ϣ
  m_lMeMaxScore=0L;
  m_lAreaLimitScore=0L;
  m_lApplyBankerCondition=0L;
  m_GameEndTime = 0;

  m_blUsing = false;

  //������ע
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //ׯ����Ϣ
  m_lBankerScore=0L;
  m_wCurrentBanker=0;
  m_cbLeftCardCount=0;

  //״̬����
  m_bMeApplyBanker=false;
  m_bAddScore=false;
  return;
}

//��������
CGameClientEngine::~CGameClientEngine()
{
}

//��ʼ����
bool CGameClientEngine::OnInitGameEngine()
{
  //ȫ�ֶ���
  m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();
  ASSERT(m_pGlobalUnits!=NULL);

  //���ñ���
  SetWindowText(TEXT("��������Ϸ  --  Ver��6.7.0.1"));
  //����ͼ��
  HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
  m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
  SetIcon(hIcon,TRUE);
  SetIcon(hIcon,FALSE);

  m_GameClientView.SetClientEngine(this);

  //��������
  if(m_pGlobalUnits->m_bAllowBackGroundSound && !m_pGlobalUnits->m_bMuteStatuts)
  {
    PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));

  }
  return true;
}

//���ÿ��
bool CGameClientEngine::OnResetGameEngine()
{
  //������Ϣ
  m_lMeMaxScore=0L;

  //������ע
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //ׯ����Ϣ
  m_lBankerScore=0L;
  m_wCurrentBanker=0;
  m_cbLeftCardCount=0;

  //״̬����
  m_bMeApplyBanker=false;
  m_bAddScore=false;
  return true;
}

//��Ϸ����
void CGameClientEngine::OnGameOptionSet()
{

  //��������
  CGameOption GameOption;
  //GameOption.m_bEnableSound=m_pGlobalUnits->m_bAllowSound;
  //��������
  //if (GameOption.DoModal()==IDOK)
  //{
  //  //���ÿؼ�
  ////  m_pGlobalUnits->m_bAllowSound = GameOption.m_bEnableSound;
  //}

  return;
}

//ʱ��ɾ��
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{

  return true;
}

//��������
bool CGameClientEngine::AllowBackGroundSound(bool bAllowSound)
{
  if(bAllowSound)
  {
    PlayBackGroundSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));
  }
  else
  {
    StopSound();
  }

  return true;
}

//ʱ����Ϣ
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{

  if((wClockID==IDI_PLACE_JETTON)&&(nElapse==0))
  {
    //���ù��
    m_GameClientView.SetCurrentJetton(0L);

    //��ֹ��ť
    m_GameClientView.m_btJetton100.EnableWindow(FALSE);
    m_GameClientView.m_btJetton1000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton10000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton100000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton500000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton1000000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton5000000.EnableWindow(FALSE);


    //ׯ�Ұ�ť
    m_GameClientView.m_btApplyBanker.EnableWindow(FALSE);
    m_GameClientView.m_btCancelBanker.EnableWindow(FALSE);
  }
  if(IDI_DISPATCH_CARD == wClockID && nElapse==0)
  {
    if(nElapse == 0)
    {
      KillTimer(IDI_DISPATCH_CARD);
    }
    if(m_GameEndTime <= 0 || m_GameEndTime > 100)
    {
      m_GameEndTime = 25;
    }

    return false;
  }
  if(m_GameEndTime==15&&GetGameStatus()==GS_GAME_END&&m_bAddScore)
  {
    m_bAddScore=false;
    --m_GameEndTime;
    CString strInfo=TEXT("");
    strInfo=TEXT("���ֽ���,�ɼ�ͳ��:");
    m_pIStringMessage->InsertSystemString(strInfo);
    strInfo.Format(TEXT("ׯ��:%I64d"),m_TempData.c);
    m_pIStringMessage->InsertNormalString(strInfo);
    strInfo.Format(TEXT("����:%I64d"),m_TempData.a);
    m_pIStringMessage->InsertNormalString(strInfo);
  }
  else
  {
    if(m_GameEndTime>0)
    {
      --m_GameEndTime;
    }
  }
  if(m_pGlobalUnits->m_bAllowSound)
  {
    if(wClockID==IDI_PLACE_JETTON&&nElapse<=5)
    {
      PlayGameSound(AfxGetInstanceHandle(),TEXT("TIME_WARIMG"));
    }
  }



  return true;
}

//�Թ�״̬
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
  return true;
}

//������Ϣ
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
  switch(wSubCmdID)
  {
    case SUB_S_GAME_FREE:   //��Ϸ����
    {
      return OnSubGameFree(pData,wDataSize);
    }
    case SUB_S_GAME_START:    //��Ϸ��ʼ
    {
      return OnSubGameStart(pData,wDataSize);
    }
    case SUB_S_PLACE_JETTON:  //�û���ע
    {
      return OnSubPlaceJetton(pData,wDataSize);
    }
    case SUB_S_APPLY_BANKER:  //������ׯ
    {
      return OnSubUserApplyBanker(pData, wDataSize);
    }
    case SUB_S_CANCEL_BANKER: //ȡ����ׯ
    {
      return OnSubUserCancelBanker(pData, wDataSize);
    }
    case SUB_S_CHANGE_BANKER: //�л�ׯ��
    {
      return OnSubChangeBanker(pData, wDataSize);
    }
    case SUB_S_GAME_END:    //��Ϸ����
    {
      return OnSubGameEnd(pData,wDataSize);
    }
    case SUB_S_SEND_RECORD:   //��Ϸ��¼
    {
      return OnSubGameRecord(pData,wDataSize);
    }
    case SUB_S_PLACE_JETTON_FAIL: //��עʧ��
    {
      return OnSubPlaceJettonFail(pData,wDataSize);
    }
    case SUB_S_CHECK_IMAGE:
    {
      return OnSubCheckImageIndex(pData,wDataSize);
    }
    case SUB_S_ADMIN_COMMDN:    //����
    {
      return OnSubAdminControl(pData,wDataSize);
    }
    case SUB_S_UPDATE_STORAGE:  //���¿��
    {
      return OnSubUpdateStorage(pData,wDataSize);
    }
  }

  //�������
  ASSERT(FALSE);

  return true;
}

//��Ϸ����
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
  //����״̬
  SetGameStatus(GAME_STATUS_FREE);
  switch(cbGameStatus)
  {
    case GAME_STATUS_FREE:      //����״̬
    {
      //Ч������
      ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
      if(wDataSize!=sizeof(CMD_S_StatusFree))
      {
        return false;
      }

      //��Ϣ����
      CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
      m_bEnableSysBanker=pStatusFree->bEnableSysBanker;


      m_lStorageCurrent=pStatusFree->lStorageStart;
      m_GameClientView.m_ShowImageIndex = pStatusFree->CheckImage;

      //�����Ϣ
      m_lMeMaxScore=pStatusFree->lUserMaxScore;
      m_GameClientView.SetMeMaxScore(m_lMeMaxScore);
      IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());
      m_GameClientView.SetMeChairID(pMeUserData->GetChairID());

      //ׯ����Ϣ
      SetBankerInfo(pStatusFree->wBankerUser,pStatusFree->lBankerScore);
      m_GameClientView.SetBankerScore(pStatusFree->cbBankerTime,pStatusFree->lBankerWinScore);

      m_GameClientView.EnableSysBanker(m_bEnableSysBanker);

      //������Ϣ
      m_lApplyBankerCondition=pStatusFree->lApplyBankerCondition;
      m_lAreaLimitScore=pStatusFree->lAreaLimitScore;
      m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);

      //�����ж�
      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);


      }


      //����ʱ��
      SetGameClock(GetMeChairID(),IDI_FREE,pStatusFree->cbTimeLeave);

      if(IsLookonMode()==false && GetMeChairID() == m_wCurrentBanker)
      {
        m_bMeApplyBanker =true;
      }

      //���¿���
      UpdateButtonContron();
      m_GameClientView.RefreshGameView();


      return true;
    }
    case GAME_STATUS_PLAY:    //��Ϸ״̬
    case GS_GAME_END:   //����״̬
    {
      //Ч������
      ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
      if(wDataSize!=sizeof(CMD_S_StatusPlay))
      {
        return false;
      }

      //��Ϣ����
      CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

      m_GameClientView.m_ShowImageIndex = pStatusPlay->CheckImage;
      m_lStorageCurrent=pStatusPlay->lStorageStart;
      //��ע��Ϣ
      for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
      {
        m_GameClientView.PlaceUserJetton(nAreaIndex,pStatusPlay->lAllJettonScore[nAreaIndex]);
        SetMePlaceJetton(nAreaIndex,pStatusPlay->lUserJettonScore[nAreaIndex]);
      }

      //��һ���
      m_lMeMaxScore=pStatusPlay->lUserMaxScore;
      m_GameClientView.SetMeMaxScore(m_lMeMaxScore);
      IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());
      m_GameClientView.SetMeChairID(pMeUserData->GetChairID());

      //������Ϣ
      m_lApplyBankerCondition=pStatusPlay->lApplyBankerCondition;
      m_lAreaLimitScore=pStatusPlay->lAreaLimitScore;
      m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);

      if(pStatusPlay->cbGameStatus==GS_GAME_END)
      {
        //�˿���Ϣ
        m_GameClientView.SetCardInfo(pStatusPlay->cbTableCardArray);
        m_GameClientView.FinishDispatchCard(false);

        //���óɼ�
        m_GameClientView.SetCurGameScore(pStatusPlay->lEndUserScore,pStatusPlay->lEndUserReturnScore,pStatusPlay->lEndBankerScore,pStatusPlay->lEndRevenue);
      }
      else
      {
        m_GameClientView.SetCardInfo(NULL);
        m_blUsing = true;
      }

      //ׯ����Ϣ
      SetBankerInfo(pStatusPlay->wBankerUser,pStatusPlay->lBankerScore);
      m_GameClientView.SetBankerScore(pStatusPlay->cbBankerTime,pStatusPlay->lBankerWinScore);
      m_bEnableSysBanker=pStatusPlay->bEnableSysBanker;
      m_GameClientView.EnableSysBanker(m_bEnableSysBanker);

      //����
      /*if((GetTableUserItem(GetMeChairID())->dwUserRight&UR_GAME_CONTROL)!=0 && m_GameClientView.m_pClientControlDlg)
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);*/

      //IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());

      //�����ж�
      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
      }

      m_GameClientView.StartRandShowSide();

      if(IsLookonMode()==false && GetMeChairID() == m_wCurrentBanker)
      {
        m_bMeApplyBanker =true;
      }

      //���°�ť
      UpdateButtonContron();

      //����ʱ��
      if(pStatusPlay->cbGameStatus != GS_GAME_END)
      {
        SetGameClock(GetMeChairID(), IDI_PLACE_JETTON, pStatusPlay->cbTimeLeave);
      }

      //����״̬
      SetGameStatus(pStatusPlay->cbGameStatus);

      m_GameClientView.RefreshGameView();
      return true;
    }
  }

  return false;
}

//��Ϸ��ʼ
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_GameStart));
  if(wDataSize!=sizeof(CMD_S_GameStart))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

  m_GameClientView.SwithToNormalView();

  m_GameClientView.KillCardTime();

  m_blUsing = true;

  //ׯ����Ϣ
  SetBankerInfo(pGameStart->wBankerUser,pGameStart->lBankerScore);

  //�����Ϣ
  m_lMeMaxScore=pGameStart->lUserMaxScore;
  m_GameClientView.SetMeMaxScore(m_lMeMaxScore);

  //����ʱ��
  SetGameClock(GetMeChairID(),IDI_PLACE_JETTON,pGameStart->cbTimeLeave);

  m_GameClientView.StartRandShowSide();

  //����״̬
  SetGameStatus(GS_PLACE_JETTON);

  m_GameClientView.m_bShowBao = true;


  //���¿���
  UpdateButtonContron();

  //������ʾ
  m_GameClientView.SetDispatchCardTip(pGameStart->bContiueCard ? enDispatchCardTip_Continue : enDispatchCardTip_Dispatch);



  //��������

  if(m_pGlobalUnits->m_bAllowSound)
  {
    PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
  }

  return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_GameFree));
  if(wDataSize!=sizeof(CMD_S_GameFree))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;

  m_blUsing = true;

  //����ʱ��
  SetGameClock(GetMeChairID(),IDI_FREE,pGameFree->cbTimeLeave);

  //m_GameClientView.StartMove();

  //����״̬
  SetGameStatus(GAME_STATUS_FREE);

  //����ǹ���Ա��ÿ�ַ��Ϳ����
  CString cs;
  cs.Format(TEXT("��ǰ�����Ϊ��%I64d"),pGameFree->lStorageStart);
  if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
  {
    m_GameClientView.m_pClientControlDlg->SetStorageCurrentValue(pGameFree->lStorageStart);
    if(m_pIStringMessage!=NULL)
    {
      m_pIStringMessage->InsertSystemString(cs);
    }
  }

  //����ʱ��
  KillTimer(IDI_ANDROID_BET);

  ////��������
  bool blWin[AREA_COUNT+1];
  for(int i = 0; i<=AREA_COUNT; i++)
  {

    blWin[i]=false;
  }

  m_GameClientView.FinishDispatchCard();

  m_GameClientView.SetWinnerSide(blWin, false);



  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    SetMePlaceJetton(nAreaIndex,0);
  }

  //m_GameClientView.CleanUserJetton();
  //���ý���
  m_GameClientView.ResetGameView();

  m_GameClientView.SwitchToCheck();

  //���¿ؼ�
  UpdateButtonContron();

  return true;
}

//�û���ע
bool CGameClientEngine::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_PlaceJetton));
  if(wDataSize!=sizeof(CMD_S_PlaceJetton))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;


  if(pPlaceJetton->cbAndroid == TRUE)
  {
    //����
    static WORD wStFluc = 1;  //�������
    tagAndroidBet androidBet = {};
    androidBet.cbJettonArea = pPlaceJetton->cbJettonArea;
    androidBet.lJettonScore = pPlaceJetton->lJettonScore;
    androidBet.wChairID = pPlaceJetton->wChairID;
    androidBet.nLeftTime = ((rand()+androidBet.wChairID+wStFluc*3)%10+1)*100;
    wStFluc = wStFluc%3 + 1;

    m_ListAndroid.AddTail(androidBet);
    if(m_ListAndroid.GetCount()==1)
    {
      SetTimer(IDI_ANDROID_BET, 100, NULL);
    }
    return true;
  }


  if(!pPlaceJetton->bAndroid && CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_hInst)
  {
    m_GameClientView.ControlBetDetection(pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore,SwitchViewChairID(pPlaceJetton->wChairID));
  }

  if(GetMeChairID()!=pPlaceJetton->wChairID || IsLookonMode())
  {

    {
      m_GameClientView.PlaceUserJetton(pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);

      //��������
      if(m_pGlobalUnits->m_bAllowSound)
      {
        if(pPlaceJetton->wChairID!=GetMeChairID() || IsLookonMode())
        {
          if(pPlaceJetton->lJettonScore==5000000)
          {
            PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD_EX"));
          }
          else
          {
            PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
          }
        }
      }
    }
  }

  UpdateButtonContron();

  return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
  if(wDataSize!=sizeof(CMD_S_GameEnd))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

  m_GameClientView.m_blMoveFinish = false;


  m_GameEndTime = pGameEnd->cbTimeLeave;

  m_blUsing = true;

  m_GameClientView.StartHandle_Leave();

  //����ʱ��
  SetGameClock(GetMeChairID(),IDI_DISPATCH_CARD, pGameEnd->cbTimeLeave);

  //�˿���Ϣ
  m_GameClientView.SetCardInfo(pGameEnd->cbTableCardArray);
  m_GameClientView.ClearAreaFlash();

  //�O�Ó���Ƅ�
  m_GameClientView.StartRunCar(20);

  //����״̬
  SetGameStatus(GS_GAME_END);

  m_cbLeftCardCount=pGameEnd->cbLeftCardCount;

  for(int i=0; i<GAME_PLAYER; i++)
  {
    WORD id=SwitchViewChairID(i);
    m_GameClientView.SetUserTotal(pGameEnd->lUserScoreTotal[id],id);
  }

  //ׯ����Ϣ
  m_GameClientView.SetBankerScore(pGameEnd->nBankerTime, pGameEnd->lBankerTotallScore);
  //�ɼ���Ϣ
  m_GameClientView.SetCurGameScore(pGameEnd->lUserScore,pGameEnd->lUserReturnScore,pGameEnd->lBankerScore,pGameEnd->lRevenue);

  m_GameClientView.PerformAllBetAnimation();

  m_TempData.a = pGameEnd->lUserScore;
  m_TempData.b = pGameEnd->lUserReturnScore;
  m_TempData.c = pGameEnd->lBankerScore;
  m_TempData.d = pGameEnd->lRevenue;


  //���¿ؼ�
  UpdateButtonContron();


  return true;
}

//���¿���
void CGameClientEngine::UpdateButtonContron()
{
  /*if(__TEST__)
  {
    return ;
  }*/
  //�����ж�
  bool bEnablePlaceJetton=true;

  if(GetGameStatus()!=GS_PLACE_JETTON)
  {
    bEnablePlaceJetton=false;

  }
  if(m_wCurrentBanker==GetMeChairID())
  {
    bEnablePlaceJetton=false;
  }
  if(IsLookonMode())
  {
    bEnablePlaceJetton=false;
  }
  if(m_bEnableSysBanker==false&&m_wCurrentBanker==INVALID_CHAIR)
  {
    bEnablePlaceJetton=false;
  }

  if(GetGameStatus()==GS_GAME_END)
  {
    m_GameClientView.m_btOpenCard.EnableWindow(false);
    m_GameClientView.m_btAutoOpenCard.EnableWindow(false);

  }
  else
  {
    m_GameClientView.m_btOpenCard.EnableWindow(true);
    m_GameClientView.m_btAutoOpenCard.EnableWindow(true);

  }

  m_GameClientView.SetEnablePlaceJetton(bEnablePlaceJetton);

  //��ע��ť
  if(bEnablePlaceJetton==true)
  {

    //�������
    LONGLONG lCurrentJetton=m_GameClientView.GetCurrentJetton();
    LONGLONG lLeaveScore=m_lMeMaxScore;
    for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
    {
      lLeaveScore -= m_lUserJettonScore[nAreaIndex];
    }

    //�����ע
    LONGLONG lUserMaxJetton = 0;

    for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
    {
      if(lUserMaxJetton==0&&nAreaIndex == 1)
      {
        lUserMaxJetton = m_GameClientView.GetUserMaxJetton(nAreaIndex);

      }
      else
      {
        if(m_GameClientView.GetUserMaxJetton(nAreaIndex)>lUserMaxJetton)
        {
          lUserMaxJetton = m_GameClientView.GetUserMaxJetton(nAreaIndex);
        }
      }
    }
    lLeaveScore = min((lLeaveScore),lUserMaxJetton); //�û����·� �����ֱȽ� �������屶

    //���ù��
    if(lCurrentJetton>lLeaveScore)
    {
      if(lLeaveScore>=5000000L)
      {
        m_GameClientView.SetCurrentJetton(5000000L);
      }
      else if(lLeaveScore>=1000000L)
      {
        m_GameClientView.SetCurrentJetton(1000000L);
      }
      else if(lLeaveScore>=100000L)
      {
        m_GameClientView.SetCurrentJetton(100000L);
      }
      else if(lLeaveScore>=10000L)
      {
        m_GameClientView.SetCurrentJetton(10000L);
      }
      else if(lLeaveScore>=1000L)
      {
        m_GameClientView.SetCurrentJetton(1000L);
      }
      else if(lLeaveScore>=100L)
      {
        m_GameClientView.SetCurrentJetton(100L);
      }
      else
      {
        m_GameClientView.SetCurrentJetton(0L);
      }
    }

    //���ư�ť
    int iTimer = 1;

    if(m_blUsing==false)
    {
      lLeaveScore = 0;
      lUserMaxJetton = 0;

    }
    m_GameClientView.m_btJetton100.EnableWindow((lLeaveScore>=100*iTimer && lUserMaxJetton>=100*iTimer)?TRUE:FALSE);
    m_GameClientView.m_btJetton1000.EnableWindow((lLeaveScore>=1000*iTimer && lUserMaxJetton>=1000*iTimer)?TRUE:FALSE);
    m_GameClientView.m_btJetton10000.EnableWindow((lLeaveScore>=10000*iTimer && lUserMaxJetton>=10000*iTimer)?TRUE:FALSE);
    m_GameClientView.m_btJetton100000.EnableWindow((lLeaveScore>=100000*iTimer && lUserMaxJetton>=100000*iTimer)?TRUE:FALSE);
    m_GameClientView.m_btJetton1000000.EnableWindow((lLeaveScore>=1000000*iTimer && lUserMaxJetton>=1000000*iTimer)?TRUE:FALSE);
    m_GameClientView.m_btJetton5000000.EnableWindow((lLeaveScore>=5000000*iTimer && lUserMaxJetton>=5000000*iTimer)?TRUE:FALSE);
  }
  else
  {
    //���ù��
    m_GameClientView.SetCurrentJetton(0L);

    //��ֹ��ť
    m_GameClientView.m_btJetton100.EnableWindow(FALSE);
    m_GameClientView.m_btJetton1000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton50000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton10000.EnableWindow(FALSE);

    m_GameClientView.m_btJetton100000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton500000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton1000000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton5000000.EnableWindow(FALSE);
  }




  //ׯ�Ұ�ť
  if(!IsLookonMode())
  {
    //��ȡ��Ϣ
    IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());

    //���밴ť
    bool bEnableApply=true;
    if(m_wCurrentBanker==GetMeChairID())
    {
      bEnableApply=false;
    }
    if(m_bMeApplyBanker)
    {
      bEnableApply=false;
    }
    if(pMeUserData->GetUserScore()<m_lApplyBankerCondition)
    {
      bEnableApply=false;
    }
    m_GameClientView.m_btApplyBanker.EnableWindow(bEnableApply?TRUE:FALSE);

    //ȡ����ť
    bool bEnableCancel=true;
    if(m_wCurrentBanker==GetMeChairID() && GetGameStatus()!=GAME_STATUS_FREE)
    {
      bEnableCancel=false;
    }
    if(m_bMeApplyBanker==false)
    {
      bEnableCancel=false;
    }
    m_GameClientView.m_btCancelBanker.EnableWindow(bEnableCancel?TRUE:FALSE);
    m_GameClientView.m_btCancelBanker.SetButtonImage(m_wCurrentBanker==GetMeChairID()?IDB_BT_CANCEL_BANKER:IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);

    //��ʾ�ж�
    if(m_bMeApplyBanker)
    {
      m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);
      m_GameClientView.m_btCancelBanker.ShowWindow(SW_SHOW);

    }
    else
    {
      m_GameClientView.m_btCancelBanker.ShowWindow(SW_HIDE);
      m_GameClientView.m_btApplyBanker.ShowWindow(SW_SHOW);
    }

  }
  else
  {
    m_GameClientView.m_btCancelBanker.EnableWindow(FALSE);
    m_GameClientView.m_btApplyBanker.EnableWindow(FALSE);
    m_GameClientView.m_btApplyBanker.ShowWindow(SW_SHOW);
    m_GameClientView.m_btCancelBanker.ShowWindow(SW_HIDE);

  }
  //��ȡ��Ϣ
  IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());

#ifdef __SPECIAL___
  //���а�ť
  m_GameClientView.m_btBankerDraw.EnableWindow(TRUE);
  m_GameClientView.m_btBankerStorage.EnableWindow(FALSE);
  //m_GameClientView.m_btBankerStorage.ShowWindow(SW_SHOW);
  m_GameClientView.m_btBankerStorage.EnableWindow((GetGameStatus()==GAME_STATUS_FREE || IsLookonMode())?TRUE:FALSE);
  if(m_wCurrentBanker==GetMeChairID()&&IsLookonMode()==false)
  {
    m_GameClientView.m_btBankerStorage.EnableWindow(FALSE);
  }
  if(m_wCurrentBanker==GetMeChairID())
  {
    m_GameClientView.m_blCanStore = false;
  }
  else
  {
    m_GameClientView.m_blCanStore = true;
  }
  m_GameClientView.SetInsureOption(false, m_GameClientView.m_blCanStore);
#endif

  return;
}
bool CGameClientEngine::OnSubCheckImageIndex(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_C_CheckImage));
  if(wDataSize!=sizeof(CMD_C_CheckImage))
  {
    return false;
  }

  CMD_C_CheckImage * pCheckImage=(CMD_C_CheckImage *)pBuffer;
  m_GameClientView.m_ShowImageIndex = pCheckImage->Index;

  return true;


}
//��ע��Ϣ
LRESULT CGameClientEngine::OnPlaceJetton(WPARAM wParam, LPARAM lParam)
{
  //��������
  BYTE cbJettonArea=(BYTE)wParam;
  LONGLONG lJettonScore=(LONGLONG)(*((LONGLONG*)lParam));

  //�Ϸ��ж�
  ASSERT(cbJettonArea>=1 && cbJettonArea<=AREA_COUNT);
  if(!(cbJettonArea>=1 && cbJettonArea<=AREA_COUNT))
  {
    return 0;
  }

  //ׯ���ж�
  if(GetMeChairID() == m_wCurrentBanker)
  {
    return true;
  }

  //״̬�ж�
  if(GetGameStatus()!=GS_PLACE_JETTON)
  {
    UpdateButtonContron();
    return true;
  }

  //���ñ���
  m_lUserJettonScore[cbJettonArea] += lJettonScore;
  m_GameClientView.SetMePlaceJetton(cbJettonArea, m_lUserJettonScore[cbJettonArea]);



  //��������
  CMD_C_PlaceJetton PlaceJetton;
  ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

  //�������
  PlaceJetton.cbJettonArea=cbJettonArea;
  PlaceJetton.lJettonScore=lJettonScore;

  m_bAddScore=true;
  //������Ϣ
  SendSocketData(SUB_C_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));

  //���°�ť
  UpdateButtonContron();

  //Ԥ����ʾ
  m_GameClientView.PlaceUserJetton(cbJettonArea,lJettonScore);

  //��������
  if(m_pGlobalUnits->m_bAllowSound)
  {
    if(lJettonScore==5000000)
    {
      PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD_EX"));
    }
    else
    {
      PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
    }
  }

  return 0;

}
LRESULT  CGameClientEngine::OnPlaySound(WPARAM wParam, LPARAM lParam)
{
  if(m_pGlobalUnits->m_bAllowSound)
  {
    if(lParam == 0)
    {
      PlayGameSound(AfxGetInstanceHandle(),TEXT("PLEASEJETTONWAV"));
    }
    else if(lParam==1)
    {
      //PlayGameSound(AfxGetInstanceHandle(),TEXT("LEAVEHANDLEWAV"));
      //StopSound();
      PlayGameSound(AfxGetInstanceHandle(),TEXT("CAR_RUN"));
    }
    else if(lParam==3)
    {
      PlayGameSound(AfxGetInstanceHandle(),TEXT("IDC_SNDWAV"));
    }
    else if(lParam==4)
    {
      PlayGameSound(AfxGetInstanceHandle(),TEXT("END_LOST"));
    }
    else if(lParam==5)
    {
      PlayGameSound(AfxGetInstanceHandle(),TEXT("END_WIN"));
    }
    else if(lParam==6)
    {
      PlayGameSound(AfxGetInstanceHandle(),TEXT("END_DRAW"));
    }
    else if(lParam==7)
    {
      PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
    }
  }


  return 1;

}

LRESULT  CGameClientEngine::OnShowResult(WPARAM wParam, LPARAM lParam)
{

  TCHAR szBuffer[128] = TEXT("");
  _sntprintf(szBuffer,CountArray(szBuffer),TEXT(" ���ַ��� %I64d ���ط��� %I64d ׯ�ҷ��� %I64d "),m_TempData.a,m_TempData.b,m_TempData.c);
  //InsertGeneralString(szBuffer,RGB(255,0,255),true);

  return 1;

}
//�ֹ�����
LRESULT  CGameClientEngine::OnOpenCard(WPARAM wParam, LPARAM lParam)
{
  if(GetGameStatus()==GS_GAME_END)
  {
    //InsertGeneralString(TEXT("��ϵͳ��ʾ�ݿ���ʱ�䲻���л�����ģʽ��"),RGB(255,0,255),true);
    return 1;

  }
  m_GameClientView.m_blAutoOpenCard = false;
  //InsertGeneralString(TEXT("��ϵͳ��ʾ����ѡ�����ֹ�����ģʽ�����ƺ�������ʹ������϶��˿ˣ�"),RGB(255,0,255),true);
  return 1;
}
//�Զ�����
LRESULT  CGameClientEngine::OnAutoOpenCard(WPARAM wParam, LPARAM lParam)
{
  if(GetGameStatus()==GS_GAME_END)
  {
    //InsertGeneralString(TEXT("��ϵͳ��ʾ�ݿ���ʱ�䲻���л�����ģʽ��"),RGB(255,0,255),true);
    return 1;

  }
  m_GameClientView.m_blAutoOpenCard = true;
  //InsertGeneralString(TEXT("��ϵͳ��ʾ����ѡ�����Զ�����ģʽ�����ƺ�ϵͳ���Զ��������ҵ��ƣ�"),RGB(255,0,255),true);

  return 1;
}
//��������
LRESULT CGameClientEngine::OnContinueCard(WPARAM wParam, LPARAM lParam)
{
  //�Ϸ��ж�
  if(GetMeChairID()!=m_wCurrentBanker)
  {
    return 0;
  }
  if(GetGameStatus()!=GAME_STATUS_FREE)
  {
    return 0;
  }
  if(m_cbLeftCardCount < 8)
  {
    return 0;
  }
  if(IsLookonMode())
  {
    return 0;
  }

  //������Ϣ
  //SendSocketData(SUB_C_CONTINUE_CARD);
  CMD_C_CheckImage CheckImage;
  CheckImage.Index =   m_GameClientView.m_CheckImagIndex;
  SendSocketData(SUB_C_CHECK_IMAGE,&CheckImage,sizeof(CheckImage));
  //���ð�ť

  return 0;
}

//������Ϣ
LRESULT CGameClientEngine::OnApplyBanker(WPARAM wParam, LPARAM lParam)
{
  //�Ϸ��ж�
  IClientUserItem *pMeUserData = GetTableUserItem(GetMeChairID());
  if(pMeUserData->GetUserScore() < m_lApplyBankerCondition)
  {
    return true;
  }

  //�Թ��ж�
  if(IsLookonMode())
  {
    return true;
  }

  //ת������
  bool bApplyBanker = wParam ? true:false;

  //��ǰ�ж�
  if(m_wCurrentBanker == GetMeChairID() && bApplyBanker)
  {
    return true;
  }

  if(bApplyBanker)
  {
    //������Ϣ
    SendSocketData(SUB_C_APPLY_BANKER, NULL, 0);

    m_bMeApplyBanker=true;
  }
  else
  {
    //������Ϣ
    SendSocketData(SUB_C_CANCEL_BANKER, NULL, 0);

    m_bMeApplyBanker=false;
  }

  //���ð�ť
  UpdateButtonContron();
  m_GameClientView.m_btCancelBanker.EnableWindow(FALSE);
  m_GameClientView.m_btApplyBanker.EnableWindow(FALSE);

  return true;
}

//������ׯ
bool CGameClientEngine::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
  if(wDataSize!=sizeof(CMD_S_ApplyBanker))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;

  //��ȡ���
  IClientUserItem *pClientUserItem=GetTableUserItem(pApplyBanker->wApplyUser);

  //�������
  if(m_wCurrentBanker != pApplyBanker->wApplyUser)
  {
    m_GameClientView.m_ValleysList.Add(SwitchViewChairID(pApplyBanker->wApplyUser));
  }

  //�Լ��ж�
  if(IsLookonMode()==false && GetMeChairID()==pApplyBanker->wApplyUser)
  {
    m_bMeApplyBanker=true;
  }

  //���¿ؼ�
  UpdateButtonContron();
  //m_GameClientView.m_btCancelBanker.EnableWindow(TRUE);

  return true;
}

//ȡ����ׯ
bool CGameClientEngine::OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_CancelBanker));
  if(wDataSize!=sizeof(CMD_S_CancelBanker))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;

  for(int i = 0; i < m_GameClientView.m_ValleysList.GetCount(); ++i)
  {
    if(SwitchViewChairID(pCancelBanker->wCancelUser) == m_GameClientView.m_ValleysList[i])
    {
      m_GameClientView.m_ValleysList.RemoveAt(i);
      break;
    }
  }

  if(IsLookonMode()==false && pCancelBanker->wCancelUser == GetMeChairID())
  {
    m_bMeApplyBanker=false;
  }

  //���¿ؼ�
  UpdateButtonContron();

  //m_GameClientView.m_btApplyBanker.EnableWindow(TRUE);

  return true;
}

//�л�ׯ��
bool CGameClientEngine::OnSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
  if(wDataSize!=sizeof(CMD_S_ChangeBanker))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_ChangeBanker * pChangeBanker=(CMD_S_ChangeBanker *)pBuffer;

  //��ʾͼƬ
  m_GameClientView.ShowChangeBanker(m_wCurrentBanker!=pChangeBanker->wBankerUser);

  //�Լ��ж�
  if(m_wCurrentBanker==GetMeChairID() && IsLookonMode() == false && pChangeBanker->wBankerUser!=GetMeChairID())
  {
    m_bMeApplyBanker=false;
  }
  else if(IsLookonMode() == false && pChangeBanker->wBankerUser==GetMeChairID())
  {
    m_bMeApplyBanker=true;
  }

  //ׯ����Ϣ
  SetBankerInfo(pChangeBanker->wBankerUser,pChangeBanker->lBankerScore);
  m_GameClientView.SetBankerScore(0,0);

  //ɾ�����
  if(m_wCurrentBanker != INVALID_CHAIR)
  {
    for(int i = 0; i < m_GameClientView.m_ValleysList.GetCount(); ++i)
    {
      if(SwitchViewChairID(m_wCurrentBanker) == m_GameClientView.m_ValleysList[i])
      {
        m_GameClientView.m_ValleysList.RemoveAt(i);
        break;
      }
    }
  }

  //���½���
  UpdateButtonContron();
  m_GameClientView.RefreshGameView();

  return true;
}

//��Ϸ��¼
bool CGameClientEngine::OnSubGameRecord(const void * pBuffer, WORD wDataSize)
{
  //Ч�����
  ASSERT(wDataSize%sizeof(tagServerGameRecord)==0);
  if(wDataSize%sizeof(tagServerGameRecord)!=0)
  {
    return false;
  }

  //���ü�¼
  WORD wRecordCount=wDataSize/sizeof(tagServerGameRecord);
  for(WORD wIndex=0; wIndex<wRecordCount; wIndex++)
  {
    tagServerGameRecord * pServerGameRecord=(((tagServerGameRecord *)pBuffer)+wIndex);

    m_GameClientView.SetGameHistory(pServerGameRecord->bWinMen);
  }

  return true;
}

//��עʧ��
bool CGameClientEngine::OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_PlaceJettonFail));
  if(wDataSize!=sizeof(CMD_S_PlaceJettonFail))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_PlaceJettonFail * pPlaceJettonFail=(CMD_S_PlaceJettonFail *)pBuffer;

  //Ч�����
  BYTE cbViewIndex=pPlaceJettonFail->lJettonArea;
  ASSERT(cbViewIndex>=1 && cbViewIndex<=AREA_COUNT);
  if(!(cbViewIndex>=1 && cbViewIndex<=AREA_COUNT))
  {
    return false;
  }

  //��ע����
  m_GameClientView.PlaceUserJetton(pPlaceJettonFail->lJettonArea,-pPlaceJettonFail->lPlaceScore);

  //�Լ��ж�
  if(GetMeChairID()==pPlaceJettonFail->wPlaceUser && false==IsLookonMode())
  {
    LONGLONG lJettonCount=pPlaceJettonFail->lPlaceScore;
    //�Ϸ�У��
    ASSERT(m_lUserJettonScore[cbViewIndex]>=lJettonCount);
    if(lJettonCount>m_lUserJettonScore[cbViewIndex])
    {
      return false;
    }

    //������ע
    m_lUserJettonScore[cbViewIndex]-=lJettonCount;
    m_GameClientView.SetMePlaceJetton(cbViewIndex,m_lUserJettonScore[cbViewIndex]);
  }

  return true;
}

//����ׯ��
void CGameClientEngine::SetBankerInfo(WORD wBanker,LONGLONG lScore)
{
  m_wCurrentBanker=wBanker;
  m_lBankerScore=lScore;
  IClientUserItem *pClientUserItem=m_wCurrentBanker==INVALID_CHAIR ? NULL : GetTableUserItem(m_wCurrentBanker);
  DWORD dwBankerUserID = (NULL==pClientUserItem) ? INVALID_CHAIR : pClientUserItem->GetChairID();
  m_GameClientView.SetBankerInfo(dwBankerUserID,m_lBankerScore);

}

//������ע
void CGameClientEngine::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
  //�Ϸ��ж�
  ASSERT(cbViewIndex>=1 && cbViewIndex<=AREA_COUNT);
  if(!(cbViewIndex>=1 && cbViewIndex<=AREA_COUNT))
  {
    return;
  }

  //���ñ���
  m_lUserJettonScore[cbViewIndex]=lJettonCount;

  //���ý���
  m_GameClientView.SetMePlaceJetton(cbViewIndex,lJettonCount);
}
//////////////////////////////////////////////////////////////////////////
//����
LRESULT CGameClientEngine::OnAdminControl(WPARAM wParam, LPARAM lParam)
{
  SendSocketData(SUB_C_ADMIN_COMMDN,(CMD_C_ControlApplication*)wParam,sizeof(CMD_C_ControlApplication));
  return true;
}


LRESULT CGameClientEngine::OnStorage(WPARAM wParam,LPARAM lParam)
{
  SendSocketData(SUB_C_UPDATE_STORAGE,(CMD_C_FreshStorage*)wParam,sizeof(CMD_C_FreshStorage));
  return true;
}

LRESULT CGameClientEngine::OnDeuct(WPARAM wParam,LPARAM lParam)
{
  SendSocketData(SUB_C_DEUCT,(CMD_C_FreshDeuct*)wParam,sizeof(CMD_C_FreshDeuct));
  return true;
}

//����
bool CGameClientEngine::OnSubAdminControl(const void * pBuffer, WORD wDataSize)
{
  ASSERT(wDataSize==sizeof(CMD_S_ControlReturns));
  if(wDataSize!=sizeof(CMD_S_ControlReturns))
  {
    return false;
  }

  if(NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd())
  {
    CMD_S_ControlReturns* pResult = (CMD_S_ControlReturns*)pBuffer;
    m_GameClientView.m_pClientControlDlg->UpdateControl(pResult);
  }
  return true;
}

void CGameClientEngine::OnTimer(UINT nIDEvent)
{
  // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
  if(nIDEvent == IDI_ANDROID_BET)
  {
    if(m_ListAndroid.GetCount() != 0)
    {
      POSITION pos = m_ListAndroid.GetTailPosition();
      POSITION posTmp = pos;

      //������ע����
      while(true)
      {
        if(pos == 0)
        {
          break;
        }

        posTmp = pos;
        tagAndroidBet & androidBet = m_ListAndroid.GetPrev(pos);

        androidBet.nLeftTime -= 100;
        if(androidBet.nLeftTime <= 0)
        {
          //ģ����Ϣ
          CMD_S_PlaceJetton placeJetton = {};
          placeJetton.cbAndroid = FALSE;
          placeJetton.bAndroid=true;
          placeJetton.cbJettonArea = androidBet.cbJettonArea;
          placeJetton.lJettonScore = androidBet.lJettonScore;
          placeJetton.wChairID = androidBet.wChairID;

          OnSubPlaceJetton((void*)&placeJetton, sizeof(placeJetton));

          //ɾ��Ԫ��
          m_ListAndroid.RemoveAt(posTmp);
        }
      }
    }
  }
  CGameFrameEngine::OnTimer(nIDEvent);
}

//��Ϸ����
LRESULT CGameClientEngine::OnMessageGameSound(WPARAM wParam, LPARAM lParam)
{
  CGlobalUnits* pGlabalUnits = CGlobalUnits::GetInstance();
  HINSTANCE hInstance=AfxGetInstanceHandle();
  if(pGlabalUnits->m_bMuteStatuts == false)
  {
    StopSound();
    //m_GameClientView.m_btSound.SetButtonImage(IDB_BT_SOUND_CLOSE,hInstance,false,false);
    pGlabalUnits->m_bMuteStatuts = true;
  }
  else
  {
    pGlabalUnits->m_bMuteStatuts = false;

    if(pGlabalUnits->m_bAllowBackGroundSound==true)
    {
      PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));
    }
  }
  return 0;
}

//���¿��
bool CGameClientEngine::OnSubUpdateStorage(const void * pBuffer, WORD wDataSize)
{
  ASSERT(wDataSize==sizeof(CMD_C_FreshStorage));
  if(wDataSize!=sizeof(CMD_C_FreshStorage))
  {
    return false;
  }

  if(NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd())
  {
    m_GameClientView.m_pClientControlDlg->UpdateStorage(pBuffer);
  }

  return true;
}
