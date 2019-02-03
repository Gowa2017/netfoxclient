#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientEngine.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_FREE          99                  //����ʱ��
#define IDI_PLACE_JETTON      100                 //��עʱ��
#define IDI_DISPATCH_CARD     301                 //����ʱ��

#define IDI_ANDROID_BET       1000

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
  ON_MESSAGE(IDM_OPEN_CARD, OnOpenCard)
  ON_MESSAGE(IDM_PLACE_JETTON,OnPlaceJetton)
  ON_MESSAGE(IDM_APPLY_BANKER, OnApplyBanker)
  ON_MESSAGE(IDM_CONTINUE_CARD, OnContinueCard)
  ON_MESSAGE(IDM_AUTO_OPEN_CARD, OnAutoOpenCard)
  ON_MESSAGE(IDM_ADMIN_COMMDN,OnAdminCommand)
  ON_MESSAGE(IDM_UPDATE_STORAGE,OnUpdateStorage)
  ON_MESSAGE(IDM_GAME_OPTIONS,OnGameSetting)
  ON_MESSAGE(IDM_CHAT_MESSAGE,OnChatMessage)
  ON_MESSAGE(IDM_GAME_SOUND,OnMessageGameSound)
  ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
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
    fp=fopen("�ͻ��˺����ע.log","w");
  }
  else
  {
    fp=fopen("�ͻ��˺����ע.log","a");
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

//���캯��
CGameClientEngine::CGameClientEngine()
{
  //������Ϣ
  m_lMeMaxScore=0L;
  m_lAreaLimitScore=0L;
  m_lApplyBankerCondition=0L;
  m_nEndGameMul = 0;

  //������ע
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));
  ZeroMemory(m_lInitUserScore,sizeof(m_lInitUserScore));

  //ׯ����Ϣ
  m_lBankerScore=0L;
  m_wCurrentBanker=0L;
  m_cbLeftCardCount=0;

  //״̬����
  m_bMeApplyBanker=false;
  m_bPlaySound = true;

  //��������
  m_ListAndroid.RemoveAll();

  return;
}

//��������
CGameClientEngine::~CGameClientEngine()
{
}

//��ʼ����
bool CGameClientEngine::OnInitGameEngine()
{
  ////ȫ�ֶ���
  //CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();
  //ASSERT(m_pGlobalUnits!=NULL);

  //����ͼ��
  HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
  m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
  SetIcon(hIcon,TRUE);
  SetIcon(hIcon,FALSE);

  //��������
  CGlobalUnits *pGlobalUnits=CGlobalUnits::GetInstance();
  if(pGlobalUnits->m_bAllowBackGroundSound && !pGlobalUnits->m_bMuteStatuts)
  {
    m_bPlaySound = true;
    PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));
  }
  pGlobalUnits->m_bAllowSound = true;

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
  m_wCurrentBanker=0L;
  m_cbLeftCardCount=0;

  //״̬����
  m_bMeApplyBanker=false;

  //��������
  m_ListAndroid.RemoveAll();

  return true;
}

//��Ϸ����
void CGameClientEngine::OnGameOptionSet()
{
  return;
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

//ʱ��ɾ��
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
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

//  if (m_pGlobalUnits->m_bAllowSound)
  {
    if(wClockID==IDI_PLACE_JETTON&&nElapse<=5)
    {
      PlayGameSound(AfxGetInstanceHandle(),_TEXT("TIME_WARIMG"));
    }
  }

  return true;
}

//�Թ���Ϣ
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
  return true;
}

//�û�����
VOID  CGameClientEngine::OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
  //�����û��б�
  UpdateUserList();

  //���óɼ�
  if(bLookonUser==false)
  {

    //���½���
    m_GameClientView.InvalidGameView(0,0,0,0);
  }

  return;
}

//�û��뿪
VOID  CGameClientEngine::OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)
{


  sUserInfo userInfo;
  userInfo.strUserName=pIClientUserItem->GetNickName();
  userInfo.lUserScore=pIClientUserItem->GetUserScore();
  userInfo.lWinScore=0;

  m_GameClientView.m_UserList.DeleteUser(userInfo);


  //���óɼ�
  if(bLookonUser==false)
  {


    WORD wViewChairId = SwitchViewChairID(pIClientUserItem->GetChairID());

    //���½���
    m_GameClientView.InvalidGameView(0,0,0,0);
  }

  return;
}

//�û�״̬
VOID  CGameClientEngine::OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
  //��������
  IClientUserItem * pIMySelfUserItem=GetMeUserItem();
  BYTE cbUserStatus=pIClientUserItem->GetUserStatus();
  return;
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
    case SUB_S_CHEAT:     //�������
    {
      return OnSubCheat(pData,wDataSize);
    }
    case SUB_S_AMDIN_COMMAND:
    {
      return OnSubReqResult(pData,wDataSize);
    }
    case SUB_S_UPDATE_STORAGE:  //���¿��
    {
      return OnSubUpdateStorage(pData,wDataSize);
    }
    case SUB_S_SEND_USER_BET_INFO:  //���¿��
    {
      return OnSubSendUserBetInfo(pData,wDataSize);
    }
  }

  //�������
  ASSERT(FALSE);

  return true;
}

//��Ϸ����
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
  SetGameStatus(GAME_SCENE_FREE);
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

      //���ý���
      CString str;
      str.Format(TEXT("%s - [ %s ]"), GAME_NAME, pStatusFree->szGameRoomName);
      m_GameClientView.SetRoomName(str);
      if(pStatusFree->bGenreEducate)
      {
        m_GameClientView.m_btBank.EnableWindow(FALSE);
      }

      //����ʱ��
      SetGameClock(GetMeChairID(),IDI_FREE,pStatusFree->cbTimeLeave);

      //�����Ϣ
      m_lMeMaxScore=pStatusFree->lUserMaxScore;
      m_GameClientView.SetMeMaxScore(m_lMeMaxScore);
      IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());
      m_GameClientView.SetMeChairID(SwitchViewChairID(GetMeChairID()));

      //ׯ����Ϣ
      SetBankerInfo(pStatusFree->wBankerUser,pStatusFree->lBankerScore);
      m_GameClientView.SetBankerScore(pStatusFree->cbBankerTime,pStatusFree->lBankerWinScore);
      m_bEnableSysBanker=pStatusFree->bEnableSysBanker;
      m_GameClientView.EnableSysBanker(m_bEnableSysBanker);
      m_nEndGameMul = pStatusFree->nEndGameMul;
      m_GameClientView.m_nEndGameMul = m_nEndGameMul;

      //������Ϣ
      m_lApplyBankerCondition=pStatusFree->lApplyBankerCondition;
      m_lAreaLimitScore=pStatusFree->lAreaLimitScore;
      m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);

      //��������
      //PlayBackGroundSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));

      /*if((GetTableUserItem(GetMeChairID())->dwUserRight&UR_GAME_CONTROL)!=0)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
      }*/

      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
      }

      //����״̬
      SetGameStatus(GAME_SCENE_FREE);
      m_GameClientView.AllowControl(GAME_SCENE_FREE);

      //���¿���
      UpdateButtonContron();
      m_GameClientView.RefreshGameView();

      return true;
    }
    case GS_PLACE_JETTON:   //��Ϸ״̬
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

      //���ý���
      CString str;
      str.Format(TEXT("%s - [ %s ]"), GAME_NAME, pStatusPlay->szGameRoomName);
      m_GameClientView.SetRoomName(str);
      if(pStatusPlay->bGenreEducate)
      {
        m_GameClientView.m_btBank.EnableWindow(FALSE);
      }

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
      m_GameClientView.SetMeChairID(SwitchViewChairID(GetMeChairID()));

      //������Ϣ
      m_lApplyBankerCondition=pStatusPlay->lApplyBankerCondition;
      m_lAreaLimitScore=pStatusPlay->lAreaLimitScore;
      m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);

      if(pStatusPlay->cbGameStatus==GS_GAME_END)
      {
        //�˿���Ϣ
        m_GameClientView.SetCardInfo(pStatusPlay->cbTableCardArray);
        m_GameClientView.FinishDispatchCard(false);

        m_GameClientView.m_bShowGameResult = true;
        m_GameClientView.m_blMoveFinish = true;

        for(int i = 0; i<4; i++)
        {
          m_GameClientView.m_CardControl[i].SetCardData(pStatusPlay->cbTableCardArray[i],2);
          m_GameClientView.m_CardControl[i].OnCopyCard();
          m_GameClientView.m_CardControl[i].OnCopyCard();

          m_GameClientView.m_CardControl[i].m_blGameEnd = true;
          m_GameClientView.m_CardControl[i].m_blhideOneCard = false;
          m_GameClientView.m_CardControl[i].SetDisplayFlag(true);
          m_GameClientView.m_CardControl[i].ShowCardControl(true);
        }

        //���óɼ�
        m_GameClientView.SetCurGameScore(pStatusPlay->lEndUserScore,pStatusPlay->lEndUserReturnScore,pStatusPlay->lEndBankerScore,pStatusPlay->lEndRevenue);
      }
      else
      {
        m_GameClientView.SetCardInfo(NULL);

        for(int i = 0; i<5; i++)
        {
          m_GameClientView.m_CardControl[i].m_CardItemArray.SetSize(0);
        }
      }

      //��������
      //PlayBackGroundSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));

      //ׯ����Ϣ
      SetBankerInfo(pStatusPlay->wBankerUser,pStatusPlay->lBankerScore);
      m_GameClientView.SetBankerScore(pStatusPlay->cbBankerTime,pStatusPlay->lBankerWinScore);
      m_bEnableSysBanker=pStatusPlay->bEnableSysBanker;
      m_GameClientView.EnableSysBanker(m_bEnableSysBanker);

      m_GameClientView.AllowControl(pStatusPlay->cbGameStatus);

      m_nEndGameMul = pStatusPlay->nEndGameMul;
      m_GameClientView.m_nEndGameMul = m_nEndGameMul;

      //���°�ť
      UpdateButtonContron();

      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
      }

      //����ʱ��
      SetGameClock(GetMeChairID(),pStatusPlay->cbGameStatus==GS_GAME_END?IDI_DISPATCH_CARD:IDI_PLACE_JETTON,pStatusPlay->cbTimeLeave);

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
  m_GameClientView.m_TempScore=0;
  //�����û��б�
  UpdateUserList();

  //��Ϣ����
  CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

  //ׯ����Ϣ
  SetBankerInfo(pGameStart->wBankerUser,pGameStart->lBankerScore);

  //�����Ϣ
  m_lMeMaxScore=pGameStart->lUserMaxScore;
  m_GameClientView.SetMeMaxScore(m_lMeMaxScore);

  m_GameClientView.m_bPlaceJettonEnd=false;

  //����ʱ��
  SetGameClock(GetMeChairID(),IDI_PLACE_JETTON,pGameStart->cbTimeLeave);
  SetTimer(IDI_ANDROID_BET, 100, NULL);

  //����״̬
  SetGameStatus(GS_PLACE_JETTON);
  m_GameClientView.AllowControl(GS_PLACE_JETTON);

  //��������
  m_ListAndroid.RemoveAll();

  //���¿���
  UpdateButtonContron();

  //������ʾ
  m_GameClientView.SetDispatchCardTip(pGameStart->bContiueCard ? enDispatchCardTip_Continue : enDispatchCardTip_Dispatch);

  //��������
  PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

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

  //����ʱ��
  SetGameClock(GetMeChairID(),IDI_FREE,pGameFree->cbTimeLeave);

  //����ʱ��
  KillTimer(IDI_ANDROID_BET);
  KillTimer(IDI_DISPATCH_CARD);

  //����״̬
  SetGameStatus(GAME_SCENE_FREE);
  m_GameClientView.AllowControl(GAME_SCENE_FREE);

  ////����ǹ���Ա��ÿ�ַ��Ϳ����
  //CString cs;
  //cs.Format(TEXT("��ǰ�����Ϊ��%I64d"),pGameFree->lStorageStart);
  //if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
  //{
  //  if(m_pIStringMessage!=NULL)
  //    m_pIStringMessage->InsertSystemString(cs);
  //}

  ////�����ע�б�
  //if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight))
  //{
  //  if( NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd())
  //  {
  //    m_GameClientView.m_pClientControlDlg->m_UserBetArray.RemoveAll();
  //    m_GameClientView.m_pClientControlDlg->UpdateUserBet(true);
  //  }
  //}

  //�����û��б�
  UpdateUserList();
  //��������
  m_GameClientView.StopAnimal();
  m_GameClientView.SetWinnerSide(false, false, false, false);
  m_GameClientView.CleanUserJetton();
  for(int nAreaIndex=ID_SHUN_MEN; nAreaIndex<=ID_JIAO_R; ++nAreaIndex)
  {
    SetMePlaceJetton(nAreaIndex,0);
  }

  //�����˿�
  for(int i=0; i<CountArray(m_GameClientView.m_CardControl); ++i)
  {
    m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
  }

  //���¿ؼ�
  UpdateButtonContron();

  //��ɷ���
  m_GameClientView.FinishDispatchCard(true);

  //���³ɼ�
  //for (WORD wUserIndex = 0; wUserIndex < MAX_CHAIR; ++wUserIndex)
  //{
  //  IClientUserItem *pClientUserItem = GetTableUserItem(wUserIndex);

  //  if ( pClientUserItem == NULL ) continue;
  //  tagApplyUser ApplyUser ;

  //  //������Ϣ
  //  ApplyUser.lUserScore = pClientUserItem->GetUserScore();
  //  ApplyUser.strUserName = pClientUserItem->GetNickName();
  //  m_GameClientView.m_ApplyUser.UpdateUser(ApplyUser);
  //}

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

  srand(GetTickCount());

  //��Ϣ����
  CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;

  //if( CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&pPlaceJetton->bAndroid&& m_GameClientView.m_hInst)
  //{
  //  return true;
  //}

  if(pPlaceJetton->wChairID!=GetMeChairID() || IsLookonMode())
  {
    //�Ƿ������
    if(pPlaceJetton->bIsAndroid)
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
    }
    else
    {
      m_GameClientView.PlaceUserJetton(pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);

      if(pPlaceJetton->lJettonScore==5000000)
      {
        PlayGameSound(AfxGetInstanceHandle(),_TEXT("ADD_GOLD_EX"));
      }
      else
      {
        PlayGameSound(AfxGetInstanceHandle(),_TEXT("ADD_GOLD"));
      }
      /*switch(rand()%3)
      {
      case 0:
        PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER1"));
        break;
      case 1:
        PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER2"));
        break;
      case 2:
        PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER3"));
        break;
      }*/
    }
  }

  /*#ifdef _DEBUG
    for (int i=0;i<200;i++)
    {
      m_GameClientView.PlaceUserJetton(pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
    }
  #endif*/

  //���°�ť
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

  //����ʱ��
  SetGameClock(GetMeChairID(),IDI_DISPATCH_CARD, pGameEnd->cbTimeLeave);

  /*Debug("˳�ţ�%I64d  ���½ǣ�%I64d  �ţ�%I64d  ���ţ�%I64d  ���ţ�%I64d  ���½ǣ�%I64d",
    m_GameClientView.m_lAllJettonScore[ID_SHUN_MEN],m_GameClientView.m_lAllJettonScore[ID_JIAO_L],m_GameClientView.m_lAllJettonScore[ID_QIAO],
    m_GameClientView.m_lAllJettonScore[ID_DUI_MEN],m_GameClientView.m_lAllJettonScore[ID_DAO_MEN],m_GameClientView.m_lAllJettonScore[ID_JIAO_R]);*/
  LONGLONG lscore1=m_GameClientView.m_lAllJettonScore[ID_SHUN_MEN]+m_GameClientView.m_lAllJettonScore[ID_JIAO_L]+m_GameClientView.m_lAllJettonScore[ID_QIAO]+
                   m_GameClientView.m_lAllJettonScore[ID_DUI_MEN]+m_GameClientView.m_lAllJettonScore[ID_DAO_MEN]+m_GameClientView.m_lAllJettonScore[ID_JIAO_R];

  Debug("��ע1��%I64d,��ע2��%I64d",lscore1,m_GameClientView.m_TempScore);
  //�˿���Ϣ
  m_GameClientView.SetCardInfo(pGameEnd->cbTableCardArray);
  
  BYTE bcTmp[2];
  bcTmp[0] = pGameEnd->bcFirstCard;
  bcTmp[1] = pGameEnd->bcNextCard;
  m_GameClientView.SetFirstShowCard(bcTmp,2);

  //m_GameClientView.SetFirstShowCard(pGameEnd->bcFirstCard);

  m_GameClientView.ClearAreaFlash();

  //�����˿�
  for(int i=0; i<CountArray(m_GameClientView.m_CardControl); ++i)
  {
    m_GameClientView.m_CardControl[i].SetCardData(m_GameClientView.m_cbTableCardArray[i],2);
  }
  m_GameClientView.SetCheatInfo(NULL, NULL);

  //�O�Ó���Ƅ�
  m_GameClientView.SetMoveCardTimer();

  //����״̬
  SetGameStatus(GS_GAME_END);
  m_GameClientView.AllowControl(GS_GAME_END);

  m_GameClientView.m_bPlaceJettonEnd=true;

  m_cbLeftCardCount=pGameEnd->cbLeftCardCount;

  //ׯ����Ϣ
  m_GameClientView.SetBankerScore(pGameEnd->nBankerTime, pGameEnd->lBankerTotallScore);

  //�ɼ���Ϣ
  m_GameClientView.SetCurGameScore(pGameEnd->lUserScore,pGameEnd->lUserReturnScore,pGameEnd->lBankerScore,pGameEnd->lRevenue);

  for(int i = 0; i<4; i++)
  {
    m_GameClientView.m_CardControl[i].m_blGameEnd = false;
  }

  //���¿ؼ�
  UpdateButtonContron();

  return true;
}

//���¿���
void CGameClientEngine::UpdateButtonContron()
{
  //�����ж�
  bool bEnablePlaceJetton=true;
  if(m_bEnableSysBanker==false&&m_wCurrentBanker==INVALID_CHAIR)
  {
    bEnablePlaceJetton=false;
  }
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
  //if (GetTableUserItem(GetMeChairID())->GetUserStatus()!=GAME_STATUS_PLAY) bEnablePlaceJetton=false;

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

  //��ע��ť
  if(bEnablePlaceJetton==true)
  {
    //�������
    LONGLONG lCurrentJetton=m_GameClientView.GetCurrentJetton();
    LONGLONG lBankerScore=0;
    if(m_wCurrentBanker!=INVALID_CHAIR)
    {
      IClientUserItem *pBankerData=GetTableUserItem(m_wCurrentBanker);
      lBankerScore=pBankerData->GetUserScore()*m_nEndGameMul/100;
    }
    else
    {
      lBankerScore=2147483647;
    }

    LONGLONG lLeaveScore=__min(m_lMeMaxScore,lBankerScore);

    for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
    {
      lLeaveScore -= m_lUserJettonScore[nAreaIndex];
    }

    //�����ע
    LONGLONG lUserMaxJetton=m_GameClientView.GetUserMaxJetton();

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
      else if(lLeaveScore>=500000L)
      {
        m_GameClientView.SetCurrentJetton(500000L);
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
    m_GameClientView.m_btJetton100.EnableWindow((lLeaveScore>=100 && lUserMaxJetton>=100)?TRUE:FALSE);
    m_GameClientView.m_btJetton1000.EnableWindow((lLeaveScore>=1000 && lUserMaxJetton>=1000)?TRUE:FALSE);
    m_GameClientView.m_btJetton10000.EnableWindow((lLeaveScore>=10000 && lUserMaxJetton>=10000)?TRUE:FALSE);
    m_GameClientView.m_btJetton100000.EnableWindow((lLeaveScore>=100000 && lUserMaxJetton>=100000)?TRUE:FALSE);
    m_GameClientView.m_btJetton500000.EnableWindow((lLeaveScore>=500000 && lUserMaxJetton>=500000)?TRUE:FALSE);
    m_GameClientView.m_btJetton1000000.EnableWindow((lLeaveScore>=1000000 && lUserMaxJetton>=1000000)?TRUE:FALSE);
    m_GameClientView.m_btJetton5000000.EnableWindow((lLeaveScore>=5000000 && lUserMaxJetton>=5000000)?TRUE:FALSE);
  }
  else
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
    if(pMeUserData->GetUserScore()<=m_lApplyBankerCondition)
    {
      bEnableApply=false;
    }
    m_GameClientView.m_btApplyBanker.EnableWindow(bEnableApply?TRUE:FALSE);

    //ȡ����ť
    bool bEnableCancel=true;
    if(m_wCurrentBanker==GetMeChairID() && GetGameStatus()!=GAME_SCENE_FREE)
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
      m_GameClientView.m_btCancelBanker.ShowWindow(SW_SHOW);
      m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);
    }
    else
    {
      m_GameClientView.m_btCancelBanker.ShowWindow(SW_HIDE);
      m_GameClientView.m_btApplyBanker.ShowWindow(SW_SHOW);
    }

    //���ư�ť
    if(GetMeChairID()==m_wCurrentBanker)
    {
      BOOL bEnableButton=TRUE;
      if(GetGameStatus()!=GAME_SCENE_FREE)
      {
        bEnableButton=FALSE;
      }
      if(m_cbLeftCardCount < 8)
      {
        bEnableButton=FALSE;
      }

      m_GameClientView.m_btContinueCard.ShowWindow(SW_SHOW);
      m_GameClientView.m_btContinueCard.EnableWindow(bEnableButton);
    }
    else
    {
      m_GameClientView.m_btContinueCard.ShowWindow(SW_HIDE);
    }
  }
  else
  {
    m_GameClientView.m_btCancelBanker.EnableWindow(FALSE);
    m_GameClientView.m_btApplyBanker.EnableWindow(FALSE);
    m_GameClientView.m_btApplyBanker.ShowWindow(SW_SHOW);
    m_GameClientView.m_btCancelBanker.ShowWindow(SW_HIDE);
    m_GameClientView.m_btContinueCard.ShowWindow(SW_HIDE);
  }

  //���а�ť
  /*m_GameClientView.m_btBankerDraw.EnableWindow(TRUE);
  m_GameClientView.m_btBankerStorage.EnableWindow(FALSE);
  m_GameClientView.m_btBankerStorage.ShowWindow(SW_SHOW);
  m_GameClientView.m_btBankerStorage.EnableWindow((GetGameStatus()==GAME_STATUS_FREE || IsLookonMode())?TRUE:FALSE);
  if(m_wCurrentBanker==GetMeChairID()&&IsLookonMode()==false)
  {
    m_GameClientView.m_btBankerStorage.EnableWindow(FALSE);
  }
  if (m_wCurrentBanker==GetMeChairID()) m_GameClientView.m_blCanStore = false;
  else  m_GameClientView.m_blCanStore = true;
  m_GameClientView.SetInsureOption(false, m_GameClientView.m_blCanStore);*/

  ReSetGameCtr();

  m_GameClientView.UpdateButtonContron();

  return;
}

//�����û��б�
void CGameClientEngine::UpdateUserList()
{

  for(int i=0; i<GAME_PLAYER; i++)
  {
    IClientUserItem *pClientUserItem=GetTableUserItem(i);
    if(pClientUserItem==NULL)
    {
      m_lInitUserScore[i]=0;
      continue;
    }

    tagUserInfo * pUserInfo=pClientUserItem->GetUserInfo();
    //ͼ������
    UINT uImageIndex=INDEX_NORMAL;

    if((pClientUserItem->GetMasterOrder()>0))
    {
      uImageIndex=INDEX_MASTER+(pUserInfo->cbMasterOrder-1);
    }

    if(pClientUserItem->GetMemberOrder()>0)
    {
      uImageIndex=INDEX_MEMBER+(pUserInfo->cbMemberOrder-1);
    }

    sUserInfo userInfo;

    userInfo.strUserName=pClientUserItem->GetNickName();
    userInfo.lUserScore=pClientUserItem->GetUserScore();
    userInfo.lWinScore=0;
    userInfo.wImageIndex=uImageIndex;

    if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && pClientUserItem->IsAndroidUser())
    {
      userInfo.wAndrod = 1;
    }
    else
    {
      userInfo.wAndrod = 0;
    }

    if(m_GameClientView.m_UserList.FindUser(userInfo.strUserName))
    {
      userInfo.lWinScore=pClientUserItem->GetUserScore()-m_lInitUserScore[i];

    }
    else
    {
      m_lInitUserScore[i]=pClientUserItem->GetUserScore();
    }

    m_GameClientView.m_UserList.InserUser(userInfo);


  }

}

//����
LRESULT CGameClientEngine::OnChatMessage(WPARAM wParam, LPARAM lParam)
{
  m_GameClientView.InsertAllChatMessage((LPCTSTR)wParam,int(lParam));
  return true;
}

//��ע��Ϣ
LRESULT CGameClientEngine::OnPlaceJetton(WPARAM wParam, LPARAM lParam)
{
  //��������
  BYTE cbJettonArea=(BYTE)wParam;
  LONGLONG lJettonScore=m_GameClientView.GetCurrentJetton();

  //�Ϸ��ж�
  ASSERT(cbJettonArea>=ID_SHUN_MEN && cbJettonArea<=ID_JIAO_R);
  if(!(cbJettonArea>=ID_SHUN_MEN && cbJettonArea<=ID_JIAO_R))
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

  //������Ϣ
  SendSocketData(SUB_C_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));

  //���°�ť
  UpdateButtonContron();

  //Ԥ����ʾ
  m_GameClientView.PlaceUserJetton(cbJettonArea,lJettonScore);

  //��������
  if(lJettonScore==5000000)
  {
    PlayGameSound(AfxGetInstanceHandle(),_TEXT("ADD_GOLD_EX"));
  }
  else
  {
    PlayGameSound(AfxGetInstanceHandle(),_TEXT("ADD_GOLD"));
  }
  /*switch(rand()%3)
  {
  case 0:
    PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER1"));
    break;
  case 1:
    PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER2"));
    break;
  case 2:
    PlayGameSound(AfxGetInstanceHandle(),TEXT("CHEER3"));
    break;
  }*/


  return 0;
}
//�ֹ�����
LRESULT  CGameClientEngine::OnOpenCard(WPARAM wParam, LPARAM lParam)
{
  if(GetGameStatus()==GS_GAME_END)
  {
    if(m_pIStringMessage!=NULL)
    {
      m_pIStringMessage->InsertCustomString(_TEXT("��ϵͳ��ʾ�ݿ���ʱ�䲻���л�����ģʽ��"),RGB(255,0,255));
    }
    return 1;

  }
  m_GameClientView.m_blAutoOpenCard = false;
  if(m_pIStringMessage!=NULL)
  {
    m_pIStringMessage->InsertCustomString(_TEXT("��ϵͳ��ʾ����ѡ�����ֹ�����ģʽ�����ƺ�������ʹ������϶��˿ˣ�"),RGB(255,0,255));
  }
  m_GameClientView.m_btOpenCard.EnableWindow(FALSE);
  m_GameClientView.m_btAutoOpenCard.EnableWindow(TRUE);

  return 1;
}
//�Զ�����
LRESULT  CGameClientEngine::OnAutoOpenCard(WPARAM wParam, LPARAM lParam)
{
  if(GetGameStatus()==GS_GAME_END)
  {
    if(m_pIStringMessage!=NULL)
    {
      m_pIStringMessage->InsertCustomString(_TEXT("��ϵͳ��ʾ�ݿ���ʱ�䲻���л�����ģʽ��"),RGB(255,0,255));
    }
    return 1;

  }
  m_GameClientView.m_blAutoOpenCard = true;
  if(m_pIStringMessage!=NULL)
  {
    m_pIStringMessage->InsertCustomString(_TEXT("��ϵͳ��ʾ����ѡ�����Զ�����ģʽ�����ƺ�ϵͳ���Զ��������ҵ��ƣ�"),RGB(255,0,255));
  }
  m_GameClientView.m_btOpenCard.EnableWindow(TRUE);
  m_GameClientView.m_btAutoOpenCard.EnableWindow(FALSE);
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
  if(GetGameStatus()!=GAME_SCENE_FREE)
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
  SendSocketData(SUB_C_CONTINUE_CARD);

  //���ð�ť
  m_GameClientView.m_btContinueCard.EnableWindow(FALSE);

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

  return true;
}

//����
LRESULT CGameClientEngine::OnGameSetting(WPARAM wParam, LPARAM lParam)
{
  CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();

  //��������
  CGameOption GameOption;
  GameOption.m_bEnableSound=m_pGlobalUnits->IsAllowGameSound();
  GameOption.m_bEnableBGSound=m_pGlobalUnits->IsAllowBackGroundSound();
  //��������
  if(GameOption.DoModal()==IDOK)
  {
    m_pGlobalUnits->m_bAllowSound=GameOption.m_bEnableSound;
    m_pGlobalUnits->m_bAllowBackGroundSound=GameOption.m_bEnableBGSound;

    AllowBackGroundSound(GameOption.m_bEnableBGSound);
  }

  return 0;
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
  if(m_wCurrentBanker!=pApplyBanker->wApplyUser)
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

  //ɾ�����
  for(int i = 0; i < m_GameClientView.m_ValleysList.GetCount(); ++i)
  {
    if(SwitchViewChairID(pCancelBanker->wCancelUser) == m_GameClientView.m_ValleysList[i])
    {
      m_GameClientView.m_ValleysList.RemoveAt(i);
      break;
    }
  }

  //�Լ��ж�
  if(IsLookonMode()==false && pCancelBanker->wCancelUser == GetMeChairID())
  {
    m_bMeApplyBanker=false;
  }

  //���¿ؼ�
  UpdateButtonContron();

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

  //�������
  tagGameRecord GameRecord;
  ZeroMemory(&GameRecord,sizeof(GameRecord));

  //���ü�¼
  WORD wRecordCount=wDataSize/sizeof(tagServerGameRecord);
  for(WORD wIndex=0; wIndex<wRecordCount; wIndex++)
  {
    tagServerGameRecord * pServerGameRecord=(((tagServerGameRecord *)pBuffer)+wIndex);

    m_GameClientView.SetGameHistory(pServerGameRecord->bWinShunMen, pServerGameRecord->bWinDaoMen, pServerGameRecord->bWinDuiMen);
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
  ASSERT(cbViewIndex<=ID_JIAO_R && cbViewIndex>=ID_SHUN_MEN);
  if(!(cbViewIndex<=ID_JIAO_R && cbViewIndex>=ID_SHUN_MEN))
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

  //���°�ť
  UpdateButtonContron();

  return true;
}

//�������
bool CGameClientEngine::OnSubCheat(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_Cheat));
  if(wDataSize!=sizeof(CMD_S_Cheat))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_Cheat * pCheat=(CMD_S_Cheat *)pBuffer;

  //Ȩ�޸���
//  if ((GetTableUserItem(GetMeChairID())->dwUserRight&UR_GAME_CONTROL) == 0) return true;

  //ʤ����Ϣ
  int nWinInfo[3] = {};
  for(int i = 0; i < 3; i++)
  {
    nWinInfo[i] = m_GameLogic.CompareCard(pCheat->cbTableCardArray[i+1], 2, pCheat->cbTableCardArray[0], 2);
  }

  m_GameClientView.SetCheatInfo(&pCheat->cbTableCardArray[0][0], nWinInfo);

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
  ASSERT(cbViewIndex>=ID_SHUN_MEN && cbViewIndex<=ID_JIAO_R);
  if(!(cbViewIndex>=ID_SHUN_MEN && cbViewIndex<=ID_JIAO_R))
  {
    return;
  }

  //���ñ���
  m_lUserJettonScore[cbViewIndex]=lJettonCount;

  //���ý���
  m_GameClientView.SetMePlaceJetton(cbViewIndex,lJettonCount);
}
//////////////////////////////////////////////////////////////////////////

void CGameClientEngine::ReSetGameCtr()
{
}



bool CGameClientEngine::OnSubReqResult(const void * pBuffer, WORD wDataSize)
{
  ASSERT(wDataSize==sizeof(CMD_S_CommandResult));
  if(wDataSize!=sizeof(CMD_S_CommandResult))
  {
    return false;
  }

  if(NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd())
  {
    m_GameClientView.m_pClientControlDlg->ReqResult(pBuffer);
  }

  return true;
}

//���¿��
bool CGameClientEngine::OnSubUpdateStorage(const void * pBuffer, WORD wDataSize)
{
  ASSERT(wDataSize==sizeof(CMD_S_UpdateStorage));
  if(wDataSize!=sizeof(CMD_S_UpdateStorage))
  {
    return false;
  }

  if(NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd())
  {
    m_GameClientView.m_pClientControlDlg->UpdateStorage(pBuffer);
  }

  return true;
}

//������ע��Ϣ
bool CGameClientEngine::OnSubSendUserBetInfo(const void * pBuffer, WORD wDataSize)
{
  //Ч������
  ASSERT(wDataSize==sizeof(CMD_S_SendUserBetInfo));
  if(wDataSize!=sizeof(CMD_S_SendUserBetInfo))
  {
    return false;
  }

  //��Ϣ����
  CMD_S_SendUserBetInfo * pSendUserBetInfo=(CMD_S_SendUserBetInfo *)pBuffer;
  LONGLONG lUserJettonScore[AREA_COUNT+1][GAME_PLAYER];
  CopyMemory(lUserJettonScore,pSendUserBetInfo->lUserJettonScore, sizeof(lUserJettonScore));

  //У��Ȩ��
  if(NULL == m_GameClientView.m_pClientControlDlg || NULL == m_GameClientView.m_pClientControlDlg->GetSafeHwnd())
  {
    return true;
  }

  CUserBetArray *pUserBetArray = &m_GameClientView.m_pClientControlDlg->m_UserBetArray;
  pUserBetArray->RemoveAll();

  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    IClientUserItem *pClientUserItem=GetTableUserItem(i);

    if(NULL == pClientUserItem)
    {
      continue;
    }
    if(pClientUserItem->IsAndroidUser())
    {
      continue;
    }

    LONGLONG lUserAllBet = 0;
    for(BYTE j=1; j<=AREA_COUNT; j++)
    {
      lUserAllBet += lUserJettonScore[j][i];
    }
    if(0 == lUserAllBet)
    {
      continue;
    }

    //��������
    tagUserBet UserBet;
    ZeroMemory(&UserBet, sizeof(UserBet));

    CopyMemory(UserBet.szNickName, pClientUserItem->GetNickName(), sizeof(UserBet.szNickName));
    UserBet.dwUserGameID = pClientUserItem->GetGameID();
    UserBet.lUserStartScore = pSendUserBetInfo->lUserStartScore[i];
    UserBet.lUserWinLost = pClientUserItem->GetUserScore() - UserBet.lUserStartScore;

    for(BYTE k=1; k<=AREA_COUNT; k++)
    {
      UserBet.lUserBet[k] = lUserJettonScore[k][i];
    }

    //��������
    pUserBetArray->Add(UserBet);
  }

  //���¿ؼ�
  m_GameClientView.m_pClientControlDlg->UpdateUserBet(false);

  return true;
}


LRESULT CGameClientEngine::OnAdminCommand(WPARAM wParam,LPARAM lParam)
{
  SendSocketData(SUB_C_AMDIN_COMMAND,(CMD_C_AdminReq*)wParam,sizeof(CMD_C_AdminReq));
  return true;
}

//���¿��
LRESULT CGameClientEngine::OnUpdateStorage(WPARAM wParam,LPARAM lParam)
{
  SendSocketData(SUB_C_UPDATE_STORAGE,(CMD_C_UpdateStorage*)wParam,sizeof(CMD_C_UpdateStorage));
  return true;
}

void CGameClientEngine::OnTimer(UINT nIDEvent)
{
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
          placeJetton.bIsAndroid = false;
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
  if(m_bPlaySound == true)
  {
    StopSound();
    m_bPlaySound = false;
    m_GameClientView.m_btSound.SetButtonImage(IDB_BT_SOUND_CLOSE,hInstance,false,false);
    pGlabalUnits->m_bAllowSound = false;
  }
  else
  {
    PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));
    m_GameClientView.m_btSound.SetButtonImage(IDB_BT_SOUND_OPEN,hInstance,false,false);
    pGlabalUnits->m_bAllowSound = true;
    m_bPlaySound = true;
  }
  bool sound = pGlabalUnits->IsAllowGameSound();
  sound = pGlabalUnits->m_bMuteStatuts;
  return 0;
}