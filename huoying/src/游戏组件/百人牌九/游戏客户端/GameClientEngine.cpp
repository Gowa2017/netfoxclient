#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientEngine.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FREE          99                  //空闲时间
#define IDI_PLACE_JETTON      100                 //下注时间
#define IDI_DISPATCH_CARD     301                 //发牌时间

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
    fp=fopen("客户端红九下注.log","w");
  }
  else
  {
    fp=fopen("客户端红九下注.log","a");
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

//构造函数
CGameClientEngine::CGameClientEngine()
{
  //限制信息
  m_lMeMaxScore=0L;
  m_lAreaLimitScore=0L;
  m_lApplyBankerCondition=0L;
  m_nEndGameMul = 0;

  //个人下注
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));
  ZeroMemory(m_lInitUserScore,sizeof(m_lInitUserScore));

  //庄家信息
  m_lBankerScore=0L;
  m_wCurrentBanker=0L;
  m_cbLeftCardCount=0;

  //状态变量
  m_bMeApplyBanker=false;
  m_bPlaySound = true;

  //机器变量
  m_ListAndroid.RemoveAll();

  return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{
  ////全局对象
  //CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();
  //ASSERT(m_pGlobalUnits!=NULL);

  //设置图标
  HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
  m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
  SetIcon(hIcon,TRUE);
  SetIcon(hIcon,FALSE);

  //播放声音
  CGlobalUnits *pGlobalUnits=CGlobalUnits::GetInstance();
  if(pGlobalUnits->m_bAllowBackGroundSound && !pGlobalUnits->m_bMuteStatuts)
  {
    m_bPlaySound = true;
    PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));
  }
  pGlobalUnits->m_bAllowSound = true;

  return true;
}

//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
  //限制信息
  m_lMeMaxScore=0L;

  //个人下注
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //庄家信息
  m_lBankerScore=0L;
  m_wCurrentBanker=0L;
  m_cbLeftCardCount=0;

  //状态变量
  m_bMeApplyBanker=false;

  //机器变量
  m_ListAndroid.RemoveAll();

  return true;
}

//游戏设置
void CGameClientEngine::OnGameOptionSet()
{
  return;
}

//声音控制
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

//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
  return true;
}

//时间消息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
  if((wClockID==IDI_PLACE_JETTON)&&(nElapse==0))
  {
    //设置光标
    m_GameClientView.SetCurrentJetton(0L);

    //禁止按钮
    m_GameClientView.m_btJetton100.EnableWindow(FALSE);
    m_GameClientView.m_btJetton1000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton10000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton100000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton500000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton1000000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton5000000.EnableWindow(FALSE);


    //庄家按钮
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

//旁观消息
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
  return true;
}

//用户进入
VOID  CGameClientEngine::OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
  //更新用户列表
  UpdateUserList();

  //设置成绩
  if(bLookonUser==false)
  {

    //更新界面
    m_GameClientView.InvalidGameView(0,0,0,0);
  }

  return;
}

//用户离开
VOID  CGameClientEngine::OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)
{


  sUserInfo userInfo;
  userInfo.strUserName=pIClientUserItem->GetNickName();
  userInfo.lUserScore=pIClientUserItem->GetUserScore();
  userInfo.lWinScore=0;

  m_GameClientView.m_UserList.DeleteUser(userInfo);


  //设置成绩
  if(bLookonUser==false)
  {


    WORD wViewChairId = SwitchViewChairID(pIClientUserItem->GetChairID());

    //更新界面
    m_GameClientView.InvalidGameView(0,0,0,0);
  }

  return;
}

//用户状态
VOID  CGameClientEngine::OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
  //变量定义
  IClientUserItem * pIMySelfUserItem=GetMeUserItem();
  BYTE cbUserStatus=pIClientUserItem->GetUserStatus();
  return;
}

//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
  switch(wSubCmdID)
  {
    case SUB_S_GAME_FREE:   //游戏空闲
    {
      return OnSubGameFree(pData,wDataSize);
    }
    case SUB_S_GAME_START:    //游戏开始
    {
      return OnSubGameStart(pData,wDataSize);
    }
    case SUB_S_PLACE_JETTON:  //用户加注
    {
      return OnSubPlaceJetton(pData,wDataSize);
    }
    case SUB_S_APPLY_BANKER:  //申请做庄
    {
      return OnSubUserApplyBanker(pData, wDataSize);
    }
    case SUB_S_CANCEL_BANKER: //取消做庄
    {
      return OnSubUserCancelBanker(pData, wDataSize);
    }
    case SUB_S_CHANGE_BANKER: //切换庄家
    {
      return OnSubChangeBanker(pData, wDataSize);
    }
    case SUB_S_GAME_END:    //游戏结束
    {
      return OnSubGameEnd(pData,wDataSize);
    }
    case SUB_S_SEND_RECORD:   //游戏记录
    {
      return OnSubGameRecord(pData,wDataSize);
    }
    case SUB_S_PLACE_JETTON_FAIL: //下注失败
    {
      return OnSubPlaceJettonFail(pData,wDataSize);
    }
    case SUB_S_CHEAT:     //玩家作弊
    {
      return OnSubCheat(pData,wDataSize);
    }
    case SUB_S_AMDIN_COMMAND:
    {
      return OnSubReqResult(pData,wDataSize);
    }
    case SUB_S_UPDATE_STORAGE:  //更新库存
    {
      return OnSubUpdateStorage(pData,wDataSize);
    }
    case SUB_S_SEND_USER_BET_INFO:  //更新库存
    {
      return OnSubSendUserBetInfo(pData,wDataSize);
    }
  }

  //错误断言
  ASSERT(FALSE);

  return true;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
  SetGameStatus(GAME_SCENE_FREE);
  switch(cbGameStatus)
  {
    case GAME_SCENE_FREE:     //空闲状态
    {
      //效验数据
      ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
      if(wDataSize!=sizeof(CMD_S_StatusFree))
      {
        return false;
      }

      //消息处理
      CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

      //设置界面
      CString str;
      str.Format(TEXT("%s - [ %s ]"), GAME_NAME, pStatusFree->szGameRoomName);
      m_GameClientView.SetRoomName(str);
      if(pStatusFree->bGenreEducate)
      {
        m_GameClientView.m_btBank.EnableWindow(FALSE);
      }

      //设置时间
      SetGameClock(GetMeChairID(),IDI_FREE,pStatusFree->cbTimeLeave);

      //玩家信息
      m_lMeMaxScore=pStatusFree->lUserMaxScore;
      m_GameClientView.SetMeMaxScore(m_lMeMaxScore);
      IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());
      m_GameClientView.SetMeChairID(SwitchViewChairID(GetMeChairID()));

      //庄家信息
      SetBankerInfo(pStatusFree->wBankerUser,pStatusFree->lBankerScore);
      m_GameClientView.SetBankerScore(pStatusFree->cbBankerTime,pStatusFree->lBankerWinScore);
      m_bEnableSysBanker=pStatusFree->bEnableSysBanker;
      m_GameClientView.EnableSysBanker(m_bEnableSysBanker);
      m_nEndGameMul = pStatusFree->nEndGameMul;
      m_GameClientView.m_nEndGameMul = m_nEndGameMul;

      //控制信息
      m_lApplyBankerCondition=pStatusFree->lApplyBankerCondition;
      m_lAreaLimitScore=pStatusFree->lAreaLimitScore;
      m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);

      //播放声音
      //PlayBackGroundSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));

      /*if((GetTableUserItem(GetMeChairID())->dwUserRight&UR_GAME_CONTROL)!=0)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
      }*/

      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
      }

      //设置状态
      SetGameStatus(GAME_SCENE_FREE);
      m_GameClientView.AllowControl(GAME_SCENE_FREE);

      //更新控制
      UpdateButtonContron();
      m_GameClientView.RefreshGameView();

      return true;
    }
    case GS_PLACE_JETTON:   //游戏状态
    case GS_GAME_END:   //结束状态
    {
      //效验数据
      ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
      if(wDataSize!=sizeof(CMD_S_StatusPlay))
      {
        return false;
      }

      //消息处理
      CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

      //设置界面
      CString str;
      str.Format(TEXT("%s - [ %s ]"), GAME_NAME, pStatusPlay->szGameRoomName);
      m_GameClientView.SetRoomName(str);
      if(pStatusPlay->bGenreEducate)
      {
        m_GameClientView.m_btBank.EnableWindow(FALSE);
      }

      //下注信息
      for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
      {
        m_GameClientView.PlaceUserJetton(nAreaIndex,pStatusPlay->lAllJettonScore[nAreaIndex]);
        SetMePlaceJetton(nAreaIndex,pStatusPlay->lUserJettonScore[nAreaIndex]);
      }

      //玩家积分
      m_lMeMaxScore=pStatusPlay->lUserMaxScore;
      m_GameClientView.SetMeMaxScore(m_lMeMaxScore);
      IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());
      m_GameClientView.SetMeChairID(SwitchViewChairID(GetMeChairID()));

      //控制信息
      m_lApplyBankerCondition=pStatusPlay->lApplyBankerCondition;
      m_lAreaLimitScore=pStatusPlay->lAreaLimitScore;
      m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);

      if(pStatusPlay->cbGameStatus==GS_GAME_END)
      {
        //扑克信息
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

        //设置成绩
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

      //播放声音
      //PlayBackGroundSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));

      //庄家信息
      SetBankerInfo(pStatusPlay->wBankerUser,pStatusPlay->lBankerScore);
      m_GameClientView.SetBankerScore(pStatusPlay->cbBankerTime,pStatusPlay->lBankerWinScore);
      m_bEnableSysBanker=pStatusPlay->bEnableSysBanker;
      m_GameClientView.EnableSysBanker(m_bEnableSysBanker);

      m_GameClientView.AllowControl(pStatusPlay->cbGameStatus);

      m_nEndGameMul = pStatusPlay->nEndGameMul;
      m_GameClientView.m_nEndGameMul = m_nEndGameMul;

      //更新按钮
      UpdateButtonContron();

      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
      }

      //设置时间
      SetGameClock(GetMeChairID(),pStatusPlay->cbGameStatus==GS_GAME_END?IDI_DISPATCH_CARD:IDI_PLACE_JETTON,pStatusPlay->cbTimeLeave);

      //设置状态
      SetGameStatus(pStatusPlay->cbGameStatus);

      m_GameClientView.RefreshGameView();

      return true;
    }
  }

  return false;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_GameStart));
  if(wDataSize!=sizeof(CMD_S_GameStart))
  {
    return false;
  }
  m_GameClientView.m_TempScore=0;
  //更新用户列表
  UpdateUserList();

  //消息处理
  CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

  //庄家信息
  SetBankerInfo(pGameStart->wBankerUser,pGameStart->lBankerScore);

  //玩家信息
  m_lMeMaxScore=pGameStart->lUserMaxScore;
  m_GameClientView.SetMeMaxScore(m_lMeMaxScore);

  m_GameClientView.m_bPlaceJettonEnd=false;

  //设置时间
  SetGameClock(GetMeChairID(),IDI_PLACE_JETTON,pGameStart->cbTimeLeave);
  SetTimer(IDI_ANDROID_BET, 100, NULL);

  //设置状态
  SetGameStatus(GS_PLACE_JETTON);
  m_GameClientView.AllowControl(GS_PLACE_JETTON);

  //机器变量
  m_ListAndroid.RemoveAll();

  //更新控制
  UpdateButtonContron();

  //设置提示
  m_GameClientView.SetDispatchCardTip(pGameStart->bContiueCard ? enDispatchCardTip_Continue : enDispatchCardTip_Dispatch);

  //播放声音
  PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

  return true;
}

//游戏空闲
bool CGameClientEngine::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_GameFree));
  if(wDataSize!=sizeof(CMD_S_GameFree))
  {
    return false;
  }

  //消息处理
  CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;

  //设置时间
  SetGameClock(GetMeChairID(),IDI_FREE,pGameFree->cbTimeLeave);

  //清理时间
  KillTimer(IDI_ANDROID_BET);
  KillTimer(IDI_DISPATCH_CARD);

  //设置状态
  SetGameStatus(GAME_SCENE_FREE);
  m_GameClientView.AllowControl(GAME_SCENE_FREE);

  ////如果是管理员则每局发送库存量
  //CString cs;
  //cs.Format(TEXT("当前库存量为：%I64d"),pGameFree->lStorageStart);
  //if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
  //{
  //  if(m_pIStringMessage!=NULL)
  //    m_pIStringMessage->InsertSystemString(cs);
  //}

  ////清空下注列表
  //if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight))
  //{
  //  if( NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd())
  //  {
  //    m_GameClientView.m_pClientControlDlg->m_UserBetArray.RemoveAll();
  //    m_GameClientView.m_pClientControlDlg->UpdateUserBet(true);
  //  }
  //}

  //更新用户列表
  UpdateUserList();
  //清理桌面
  m_GameClientView.StopAnimal();
  m_GameClientView.SetWinnerSide(false, false, false, false);
  m_GameClientView.CleanUserJetton();
  for(int nAreaIndex=ID_SHUN_MEN; nAreaIndex<=ID_JIAO_R; ++nAreaIndex)
  {
    SetMePlaceJetton(nAreaIndex,0);
  }

  //设置扑克
  for(int i=0; i<CountArray(m_GameClientView.m_CardControl); ++i)
  {
    m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
  }

  //更新控件
  UpdateButtonContron();

  //完成发牌
  m_GameClientView.FinishDispatchCard(true);

  //更新成绩
  //for (WORD wUserIndex = 0; wUserIndex < MAX_CHAIR; ++wUserIndex)
  //{
  //  IClientUserItem *pClientUserItem = GetTableUserItem(wUserIndex);

  //  if ( pClientUserItem == NULL ) continue;
  //  tagApplyUser ApplyUser ;

  //  //更新信息
  //  ApplyUser.lUserScore = pClientUserItem->GetUserScore();
  //  ApplyUser.strUserName = pClientUserItem->GetNickName();
  //  m_GameClientView.m_ApplyUser.UpdateUser(ApplyUser);
  //}

  return true;
}

//用户加注
bool CGameClientEngine::OnSubPlaceJetton(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_PlaceJetton));
  if(wDataSize!=sizeof(CMD_S_PlaceJetton))
  {
    return false;
  }

  srand(GetTickCount());

  //消息处理
  CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;

  //if( CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&pPlaceJetton->bAndroid&& m_GameClientView.m_hInst)
  //{
  //  return true;
  //}

  if(pPlaceJetton->wChairID!=GetMeChairID() || IsLookonMode())
  {
    //是否机器人
    if(pPlaceJetton->bIsAndroid)
    {
      //保存
      static WORD wStFluc = 1;  //随机辅助
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

  //更新按钮
  UpdateButtonContron();

  return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
  if(wDataSize!=sizeof(CMD_S_GameEnd))
  {
    return false;
  }

  //消息处理
  CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

  m_GameClientView.m_blMoveFinish = false;

  //设置时间
  SetGameClock(GetMeChairID(),IDI_DISPATCH_CARD, pGameEnd->cbTimeLeave);

  /*Debug("顺门：%I64d  左下角：%I64d  桥：%I64d  对门：%I64d  倒门：%I64d  右下角：%I64d",
    m_GameClientView.m_lAllJettonScore[ID_SHUN_MEN],m_GameClientView.m_lAllJettonScore[ID_JIAO_L],m_GameClientView.m_lAllJettonScore[ID_QIAO],
    m_GameClientView.m_lAllJettonScore[ID_DUI_MEN],m_GameClientView.m_lAllJettonScore[ID_DAO_MEN],m_GameClientView.m_lAllJettonScore[ID_JIAO_R]);*/
  LONGLONG lscore1=m_GameClientView.m_lAllJettonScore[ID_SHUN_MEN]+m_GameClientView.m_lAllJettonScore[ID_JIAO_L]+m_GameClientView.m_lAllJettonScore[ID_QIAO]+
                   m_GameClientView.m_lAllJettonScore[ID_DUI_MEN]+m_GameClientView.m_lAllJettonScore[ID_DAO_MEN]+m_GameClientView.m_lAllJettonScore[ID_JIAO_R];

  Debug("总注1：%I64d,总注2：%I64d",lscore1,m_GameClientView.m_TempScore);
  //扑克信息
  m_GameClientView.SetCardInfo(pGameEnd->cbTableCardArray);
  
  BYTE bcTmp[2];
  bcTmp[0] = pGameEnd->bcFirstCard;
  bcTmp[1] = pGameEnd->bcNextCard;
  m_GameClientView.SetFirstShowCard(bcTmp,2);

  //m_GameClientView.SetFirstShowCard(pGameEnd->bcFirstCard);

  m_GameClientView.ClearAreaFlash();

  //设置扑克
  for(int i=0; i<CountArray(m_GameClientView.m_CardControl); ++i)
  {
    m_GameClientView.m_CardControl[i].SetCardData(m_GameClientView.m_cbTableCardArray[i],2);
  }
  m_GameClientView.SetCheatInfo(NULL, NULL);

  //設置撲克移動
  m_GameClientView.SetMoveCardTimer();

  //设置状态
  SetGameStatus(GS_GAME_END);
  m_GameClientView.AllowControl(GS_GAME_END);

  m_GameClientView.m_bPlaceJettonEnd=true;

  m_cbLeftCardCount=pGameEnd->cbLeftCardCount;

  //庄家信息
  m_GameClientView.SetBankerScore(pGameEnd->nBankerTime, pGameEnd->lBankerTotallScore);

  //成绩信息
  m_GameClientView.SetCurGameScore(pGameEnd->lUserScore,pGameEnd->lUserReturnScore,pGameEnd->lBankerScore,pGameEnd->lRevenue);

  for(int i = 0; i<4; i++)
  {
    m_GameClientView.m_CardControl[i].m_blGameEnd = false;
  }

  //更新控件
  UpdateButtonContron();

  return true;
}

//更新控制
void CGameClientEngine::UpdateButtonContron()
{
  //置能判断
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

  //下注按钮
  if(bEnablePlaceJetton==true)
  {
    //计算积分
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

    //最大下注
    LONGLONG lUserMaxJetton=m_GameClientView.GetUserMaxJetton();

    //设置光标
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

    //控制按钮
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
    //设置光标
    m_GameClientView.SetCurrentJetton(0L);

    //禁止按钮
    m_GameClientView.m_btJetton100.EnableWindow(FALSE);
    m_GameClientView.m_btJetton1000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton10000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton100000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton500000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton1000000.EnableWindow(FALSE);
    m_GameClientView.m_btJetton5000000.EnableWindow(FALSE);
  }

  //庄家按钮
  if(!IsLookonMode())
  {
    //获取信息
    IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());

    //申请按钮
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

    //取消按钮
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

    //显示判断
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

    //发牌按钮
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

  //银行按钮
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

//更新用户列表
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
    //图标索引
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

//聊天
LRESULT CGameClientEngine::OnChatMessage(WPARAM wParam, LPARAM lParam)
{
  m_GameClientView.InsertAllChatMessage((LPCTSTR)wParam,int(lParam));
  return true;
}

//加注消息
LRESULT CGameClientEngine::OnPlaceJetton(WPARAM wParam, LPARAM lParam)
{
  //变量定义
  BYTE cbJettonArea=(BYTE)wParam;
  LONGLONG lJettonScore=m_GameClientView.GetCurrentJetton();

  //合法判断
  ASSERT(cbJettonArea>=ID_SHUN_MEN && cbJettonArea<=ID_JIAO_R);
  if(!(cbJettonArea>=ID_SHUN_MEN && cbJettonArea<=ID_JIAO_R))
  {
    return 0;
  }

  //庄家判断
  if(GetMeChairID() == m_wCurrentBanker)
  {
    return true;
  }

  //状态判断
  if(GetGameStatus()!=GS_PLACE_JETTON)
  {
    UpdateButtonContron();
    return true;
  }

  //设置变量
  m_lUserJettonScore[cbJettonArea] += lJettonScore;
  m_GameClientView.SetMePlaceJetton(cbJettonArea, m_lUserJettonScore[cbJettonArea]);

  //变量定义
  CMD_C_PlaceJetton PlaceJetton;
  ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

  //构造变量
  PlaceJetton.cbJettonArea=cbJettonArea;
  PlaceJetton.lJettonScore=lJettonScore;

  //发送消息
  SendSocketData(SUB_C_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));

  //更新按钮
  UpdateButtonContron();

  //预先显示
  m_GameClientView.PlaceUserJetton(cbJettonArea,lJettonScore);

  //播放声音
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
//手工搓牌
LRESULT  CGameClientEngine::OnOpenCard(WPARAM wParam, LPARAM lParam)
{
  if(GetGameStatus()==GS_GAME_END)
  {
    if(m_pIStringMessage!=NULL)
    {
      m_pIStringMessage->InsertCustomString(_TEXT("［系统提示］开牌时间不能切换腾牌模式！"),RGB(255,0,255));
    }
    return 1;

  }
  m_GameClientView.m_blAutoOpenCard = false;
  if(m_pIStringMessage!=NULL)
  {
    m_pIStringMessage->InsertCustomString(_TEXT("［系统提示］您选择了手工腾牌模式，发牌后您可以使用鼠标拖动扑克！"),RGB(255,0,255));
  }
  m_GameClientView.m_btOpenCard.EnableWindow(FALSE);
  m_GameClientView.m_btAutoOpenCard.EnableWindow(TRUE);

  return 1;
}
//自动搓牌
LRESULT  CGameClientEngine::OnAutoOpenCard(WPARAM wParam, LPARAM lParam)
{
  if(GetGameStatus()==GS_GAME_END)
  {
    if(m_pIStringMessage!=NULL)
    {
      m_pIStringMessage->InsertCustomString(_TEXT("［系统提示］开牌时间不能切换腾牌模式！"),RGB(255,0,255));
    }
    return 1;

  }
  m_GameClientView.m_blAutoOpenCard = true;
  if(m_pIStringMessage!=NULL)
  {
    m_pIStringMessage->InsertCustomString(_TEXT("［系统提示］您选择了自动腾牌模式，开牌后系统将自动开出各家的牌！"),RGB(255,0,255));
  }
  m_GameClientView.m_btOpenCard.EnableWindow(TRUE);
  m_GameClientView.m_btAutoOpenCard.EnableWindow(FALSE);
  return 1;
}
//继续发牌
LRESULT CGameClientEngine::OnContinueCard(WPARAM wParam, LPARAM lParam)
{
  //合法判断
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

  //发送消息
  SendSocketData(SUB_C_CONTINUE_CARD);

  //设置按钮
  m_GameClientView.m_btContinueCard.EnableWindow(FALSE);

  return 0;
}

//申请消息
LRESULT CGameClientEngine::OnApplyBanker(WPARAM wParam, LPARAM lParam)
{
  //合法判断
  IClientUserItem *pMeUserData = GetTableUserItem(GetMeChairID());
  if(pMeUserData->GetUserScore() < m_lApplyBankerCondition)
  {
    return true;
  }

  //旁观判断
  if(IsLookonMode())
  {
    return true;
  }

  //转换变量
  bool bApplyBanker = wParam ? true:false;

  //当前判断
  if(m_wCurrentBanker == GetMeChairID() && bApplyBanker)
  {
    return true;
  }

  if(bApplyBanker)
  {
    //发送消息
    SendSocketData(SUB_C_APPLY_BANKER, NULL, 0);

    m_bMeApplyBanker=true;
  }
  else
  {
    //发送消息
    SendSocketData(SUB_C_CANCEL_BANKER, NULL, 0);

    m_bMeApplyBanker=false;
  }

  //设置按钮
  UpdateButtonContron();

  return true;
}

//设置
LRESULT CGameClientEngine::OnGameSetting(WPARAM wParam, LPARAM lParam)
{
  CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();

  //构造数据
  CGameOption GameOption;
  GameOption.m_bEnableSound=m_pGlobalUnits->IsAllowGameSound();
  GameOption.m_bEnableBGSound=m_pGlobalUnits->IsAllowBackGroundSound();
  //配置数据
  if(GameOption.DoModal()==IDOK)
  {
    m_pGlobalUnits->m_bAllowSound=GameOption.m_bEnableSound;
    m_pGlobalUnits->m_bAllowBackGroundSound=GameOption.m_bEnableBGSound;

    AllowBackGroundSound(GameOption.m_bEnableBGSound);
  }

  return 0;
}


//申请做庄
bool CGameClientEngine::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
  if(wDataSize!=sizeof(CMD_S_ApplyBanker))
  {
    return false;
  }

  //消息处理
  CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;

  //获取玩家
  IClientUserItem *pClientUserItem=GetTableUserItem(pApplyBanker->wApplyUser);

  //插入玩家
  if(m_wCurrentBanker!=pApplyBanker->wApplyUser)
  {
    m_GameClientView.m_ValleysList.Add(SwitchViewChairID(pApplyBanker->wApplyUser));
  }

  //自己判断
  if(IsLookonMode()==false && GetMeChairID()==pApplyBanker->wApplyUser)
  {
    m_bMeApplyBanker=true;
  }

  //更新控件
  UpdateButtonContron();

  return true;
}

//取消做庄
bool CGameClientEngine::OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_CancelBanker));
  if(wDataSize!=sizeof(CMD_S_CancelBanker))
  {
    return false;
  }

  //消息处理
  CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;

  //删除玩家
  for(int i = 0; i < m_GameClientView.m_ValleysList.GetCount(); ++i)
  {
    if(SwitchViewChairID(pCancelBanker->wCancelUser) == m_GameClientView.m_ValleysList[i])
    {
      m_GameClientView.m_ValleysList.RemoveAt(i);
      break;
    }
  }

  //自己判断
  if(IsLookonMode()==false && pCancelBanker->wCancelUser == GetMeChairID())
  {
    m_bMeApplyBanker=false;
  }

  //更新控件
  UpdateButtonContron();

  return true;
}

//切换庄家
bool CGameClientEngine::OnSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
  if(wDataSize!=sizeof(CMD_S_ChangeBanker))
  {
    return false;
  }

  //消息处理
  CMD_S_ChangeBanker * pChangeBanker=(CMD_S_ChangeBanker *)pBuffer;

  //显示图片
  m_GameClientView.ShowChangeBanker(m_wCurrentBanker!=pChangeBanker->wBankerUser);

  //自己判断
  if(m_wCurrentBanker==GetMeChairID() && IsLookonMode() == false && pChangeBanker->wBankerUser!=GetMeChairID())
  {
    m_bMeApplyBanker=false;
  }
  else if(IsLookonMode() == false && pChangeBanker->wBankerUser==GetMeChairID())
  {
    m_bMeApplyBanker=true;
  }

  //庄家信息
  SetBankerInfo(pChangeBanker->wBankerUser,pChangeBanker->lBankerScore);
  m_GameClientView.SetBankerScore(0,0);

  //删除玩家
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

  //更新界面
  UpdateButtonContron();
  m_GameClientView.RefreshGameView();

  return true;
}

//游戏记录
bool CGameClientEngine::OnSubGameRecord(const void * pBuffer, WORD wDataSize)
{
  //效验参数
  ASSERT(wDataSize%sizeof(tagServerGameRecord)==0);
  if(wDataSize%sizeof(tagServerGameRecord)!=0)
  {
    return false;
  }

  //结果变量
  tagGameRecord GameRecord;
  ZeroMemory(&GameRecord,sizeof(GameRecord));

  //设置记录
  WORD wRecordCount=wDataSize/sizeof(tagServerGameRecord);
  for(WORD wIndex=0; wIndex<wRecordCount; wIndex++)
  {
    tagServerGameRecord * pServerGameRecord=(((tagServerGameRecord *)pBuffer)+wIndex);

    m_GameClientView.SetGameHistory(pServerGameRecord->bWinShunMen, pServerGameRecord->bWinDaoMen, pServerGameRecord->bWinDuiMen);
  }

  return true;
}

//下注失败
bool CGameClientEngine::OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_PlaceJettonFail));
  if(wDataSize!=sizeof(CMD_S_PlaceJettonFail))
  {
    return false;
  }

  //消息处理
  CMD_S_PlaceJettonFail * pPlaceJettonFail=(CMD_S_PlaceJettonFail *)pBuffer;

  //效验参数
  BYTE cbViewIndex=pPlaceJettonFail->lJettonArea;
  ASSERT(cbViewIndex<=ID_JIAO_R && cbViewIndex>=ID_SHUN_MEN);
  if(!(cbViewIndex<=ID_JIAO_R && cbViewIndex>=ID_SHUN_MEN))
  {
    return false;
  }

  //加注界面
  m_GameClientView.PlaceUserJetton(pPlaceJettonFail->lJettonArea,-pPlaceJettonFail->lPlaceScore);

  //自己判断
  if(GetMeChairID()==pPlaceJettonFail->wPlaceUser && false==IsLookonMode())
  {
    LONGLONG lJettonCount=pPlaceJettonFail->lPlaceScore;
    //合法校验
    ASSERT(m_lUserJettonScore[cbViewIndex]>=lJettonCount);
    if(lJettonCount>m_lUserJettonScore[cbViewIndex])
    {
      return false;
    }

    //设置下注
    m_lUserJettonScore[cbViewIndex]-=lJettonCount;
    m_GameClientView.SetMePlaceJetton(cbViewIndex,m_lUserJettonScore[cbViewIndex]);
  }

  //更新按钮
  UpdateButtonContron();

  return true;
}

//玩家作弊
bool CGameClientEngine::OnSubCheat(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_Cheat));
  if(wDataSize!=sizeof(CMD_S_Cheat))
  {
    return false;
  }

  //消息处理
  CMD_S_Cheat * pCheat=(CMD_S_Cheat *)pBuffer;

  //权限复查
//  if ((GetTableUserItem(GetMeChairID())->dwUserRight&UR_GAME_CONTROL) == 0) return true;

  //胜负信息
  int nWinInfo[3] = {};
  for(int i = 0; i < 3; i++)
  {
    nWinInfo[i] = m_GameLogic.CompareCard(pCheat->cbTableCardArray[i+1], 2, pCheat->cbTableCardArray[0], 2);
  }

  m_GameClientView.SetCheatInfo(&pCheat->cbTableCardArray[0][0], nWinInfo);

  return true;
}

//设置庄家
void CGameClientEngine::SetBankerInfo(WORD wBanker,LONGLONG lScore)
{
  m_wCurrentBanker=wBanker;
  m_lBankerScore=lScore;
  IClientUserItem *pClientUserItem=m_wCurrentBanker==INVALID_CHAIR ? NULL : GetTableUserItem(m_wCurrentBanker);
  DWORD dwBankerUserID = (NULL==pClientUserItem) ? INVALID_CHAIR : pClientUserItem->GetChairID();
  m_GameClientView.SetBankerInfo(dwBankerUserID,m_lBankerScore);
}

//个人下注
void CGameClientEngine::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
  //合法判断
  ASSERT(cbViewIndex>=ID_SHUN_MEN && cbViewIndex<=ID_JIAO_R);
  if(!(cbViewIndex>=ID_SHUN_MEN && cbViewIndex<=ID_JIAO_R))
  {
    return;
  }

  //设置变量
  m_lUserJettonScore[cbViewIndex]=lJettonCount;

  //设置界面
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

//更新库存
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

//更新下注信息
bool CGameClientEngine::OnSubSendUserBetInfo(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_SendUserBetInfo));
  if(wDataSize!=sizeof(CMD_S_SendUserBetInfo))
  {
    return false;
  }

  //消息处理
  CMD_S_SendUserBetInfo * pSendUserBetInfo=(CMD_S_SendUserBetInfo *)pBuffer;
  LONGLONG lUserJettonScore[AREA_COUNT+1][GAME_PLAYER];
  CopyMemory(lUserJettonScore,pSendUserBetInfo->lUserJettonScore, sizeof(lUserJettonScore));

  //校验权限
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

    //构造数据
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

    //插入数据
    pUserBetArray->Add(UserBet);
  }

  //更新控件
  m_GameClientView.m_pClientControlDlg->UpdateUserBet(false);

  return true;
}


LRESULT CGameClientEngine::OnAdminCommand(WPARAM wParam,LPARAM lParam)
{
  SendSocketData(SUB_C_AMDIN_COMMAND,(CMD_C_AdminReq*)wParam,sizeof(CMD_C_AdminReq));
  return true;
}

//更新库存
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

      //遍历下注链表
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
          //模拟消息
          CMD_S_PlaceJetton placeJetton = {};
          placeJetton.bIsAndroid = false;
          placeJetton.bAndroid=true;
          placeJetton.cbJettonArea = androidBet.cbJettonArea;
          placeJetton.lJettonScore = androidBet.lJettonScore;
          placeJetton.wChairID = androidBet.wChairID;

          OnSubPlaceJetton((void*)&placeJetton, sizeof(placeJetton));

          //删除元素
          m_ListAndroid.RemoveAt(posTmp);
        }
      }
    }
  }

  CGameFrameEngine::OnTimer(nIDEvent);
}

//游戏声音
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