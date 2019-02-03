#include "StdAfx.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "GameClientView.h"
#include "Math.h"
#include "GameClientEngine.h"
//////////////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FLASH_WINNER      100                 //闪动标识
#define IDI_SHOW_CHANGE_BANKER    101                 //轮换庄家
#define IDI_DISPATCH_CARD     102                 //发牌标识
#define IDI_SHOWDISPATCH_CARD_TIP 103                 //发牌提示
#define IDI_OPENCARD        104                 //发牌提示
#define IDI_MOVECARD_END      105                 //移動牌結束
#define IDI_POSTCARD        106                 //发牌提示

//按钮标识
#define IDC_OPTION          225                 //设置按钮标识
#define IDC_MIN           226                 //设置按钮标识
#define IDC_CLOSE         227                 //设置按钮标识
#define IDC_MAX           111                 //最大按钮
#define IDC_RESTORE         112                 //恢复按钮
#define IDC_CHAT_DISPLAY      228                 //控件标识
#define IDC_USER_LIST       229                 //控件标识
#define IDC_JETTON_BUTTON_100   260                 //按钮标识
#define IDC_JETTON_BUTTON_1000    261                 //按钮标识
#define IDC_JETTON_BUTTON_10000   262                 //按钮标识
#define IDC_JETTON_BUTTON_100000  263                 //按钮标识
#define IDC_JETTON_BUTTON_1000000 264                 //按钮标识
#define IDC_JETTON_BUTTON_5000000 265                 //按钮标识
#define IDC_APPY_BANKER       266                 //按钮标识
#define IDC_CANCEL_BANKER     267                 //按钮标识
#define IDC_SCORE_MOVE_L      268                 //按钮标识
#define IDC_SCORE_MOVE_R      269                 //按钮标识
#define IDC_VIEW_CHART        270                 //按钮标识
#define IDC_JETTON_BUTTON_50000     271                 //按钮标识
#define IDC_JETTON_BUTTON_500000  272                 //按钮标识
#define IDC_AUTO_OPEN_CARD      273                 //按钮标识
#define IDC_OPEN_CARD       274                 //按钮标识
#define IDC_BANK          275                 //按钮标识
#define IDC_CONTINUE_CARD     276                 //按钮标识
#define IDC_UP            277                 //按钮标识
#define IDC_DOWN          278                 //按钮标识
#define IDC_SOUND         279
#define IDC_SEND_CHAT       280                 //发送按钮
#define IDC_CHAT_INPUT        281                 //控件标识
//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
  ON_WM_TIMER()
  ON_WM_CREATE()
  ON_WM_SETCURSOR()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_WM_CTLCOLOR()
  ON_WM_LBUTTONDBLCLK()
  ON_MESSAGE(WM_SET_CAPTION, OnSetCaption)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
  //下注信息
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //全体下注
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));
  m_nEndGameMul = 0;

  //庄家信息
  m_wBankerUser=INVALID_CHAIR;
  m_wBankerTime=0;
  m_lBankerScore=0L;
  m_lBankerWinScore=0L;
  m_lTmpBankerWinScore=0;

  //当局成绩
  m_lMeCurGameScore=0L;
  m_lMeCurGameReturnScore=0L;
  m_lBankerCurGameScore=0L;
  m_lGameRevenue=0L;
  m_nShowValleyIndex = 0;

  //状态信息
  m_strRoomName = GAME_NAME;
  m_lCurrentJetton=0L;
  m_cbAreaFlash=0xFF;
  m_wMeChairID=INVALID_CHAIR;
  m_bShowChangeBanker=false;
  m_bNeedSetGameRecord=false;
  m_bWinTianMen=false;
  m_bWinHuangMen=false;
  m_bWinXuanMen=false;
  m_bFlashResult=false;
  m_blMoveFinish = false;
  m_blAutoOpenCard = true;
  m_enDispatchCardTip=enDispatchCardTip_NULL;

  m_lMeCurGameScore=0L;
  m_lMeCurGameReturnScore=0L;
  m_lBankerCurGameScore=0L;

  m_lAreaLimitScore=0L;

  //位置信息
  m_nScoreHead = 0;
  m_nRecordFirst= 0;
  m_nRecordLast= 0;

  //历史成绩
  m_lMeStatisticScore=0;

  //控件变量
  m_pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);

  m_pClientControlDlg = NULL;
  m_hInst = NULL;

  m_dwChatTime = 0L;

  return;
}

//析构函数
CGameClientView::~CGameClientView()
{
  if(m_pClientControlDlg)
  {
    delete m_pClientControlDlg;
    m_pClientControlDlg = NULL;
  }

  if(m_hInst)
  {
    FreeLibrary(m_hInst);
    m_hInst = NULL;
  }
}

//消息解释
BOOL CGameClientView::PreTranslateMessage(MSG * pMsg)
{

  if(pMsg->message == WM_KEYUP)
  {
    if(pMsg-> wParam == VK_RETURN)
    {
      CString str;
      GetDlgItem(IDC_CHAT_INPUT)->GetWindowText(str);
      if(str.GetLength() > 0)
      {
        OnSendMessage();
      }
      else
      {
        GetDlgItem(IDC_CHAT_INPUT)->SetFocus();
      }
    }
  }

  return __super::PreTranslateMessage(pMsg);
}

//重置界面
VOID CGameClientView::ResetGameView()
{
  //下注信息
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //全体下注
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

  //庄家信息
  m_wBankerUser=INVALID_CHAIR;
  m_wBankerTime=0;
  m_lBankerScore=0L;
  m_lBankerWinScore=0L;
  m_lTmpBankerWinScore=0;
  m_nShowValleyIndex = 0;

  //当局成绩
  m_lMeCurGameScore=0L;
  m_lMeCurGameReturnScore=0L;
  m_lBankerCurGameScore=0L;
  m_lGameRevenue=0L;

  //状态信息
  m_lCurrentJetton=0L;
  m_cbAreaFlash=0xFF;
  m_wMeChairID=INVALID_CHAIR;
  m_bShowChangeBanker=false;
  m_bNeedSetGameRecord=false;
  m_bWinTianMen=false;
  m_bWinHuangMen=false;
  m_bWinXuanMen=false;
  m_bFlashResult=false;
  m_bShowGameResult=false;
  m_enDispatchCardTip=enDispatchCardTip_NULL;

  m_lMeCurGameScore=0L;
  m_lMeCurGameReturnScore=0L;
  m_lBankerCurGameScore=0L;

  m_lAreaLimitScore=0L;

  //位置信息
  m_nScoreHead = 0;
  m_nRecordFirst= 0;
  m_nRecordLast= 0;

  //历史成绩
  m_lMeStatisticScore=0;

  //清空列表
  m_ValleysList.RemoveAll();
  m_btUp.EnableWindow(false);
  m_btDown.EnableWindow(false);

  //清除桌面
  CleanUserJetton();

  //设置按钮
  m_btApplyBanker.ShowWindow(SW_SHOW);
  m_btApplyBanker.EnableWindow(FALSE);
  m_btCancelBanker.ShowWindow(SW_HIDE);
  m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);

}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
//位置信息
  m_nWinFlagsExcursionX = 301;
  m_nWinFlagsExcursionY = nHeight-103;

  CSize Size;
  m_CardControl[0].GetControlSize(Size,3);
  Size.cy = Size.cy/2;

  int iWidth = m_ImageViewBack.GetWidth();
  int LifeWidth = nWidth/2-iWidth/2;
  int TopHeight = LESS_SCREEN_CY/2-m_ImageViewBack.GetHeight()/2;

  //区域位置
  int nCenterX=nWidth/2, nCenterY=nHeight/2, nBorderWidth=4;
  m_rcTianMen.left=LifeWidth+33+22+76;
  m_rcTianMen.top=TopHeight+252;
  m_rcTianMen.right=m_rcTianMen.left+186;
  m_rcTianMen.bottom=m_rcTianMen.top+158;

  m_rcDimen.left=LifeWidth+33+25+172+90;
  m_rcDimen.top=TopHeight+252;
  m_rcDimen.right=m_rcDimen.left+186;
  m_rcDimen.bottom=m_rcDimen.top+158;

  m_rcXuanMen.left=LifeWidth+137+25+177+170;
  m_rcXuanMen.top=TopHeight+252;
  m_rcXuanMen.right=m_rcXuanMen.left+186;
  m_rcXuanMen.bottom=m_rcXuanMen.top+158;


  m_rcHuangMen.left=LifeWidth+150+25+175+175+173;
  m_rcHuangMen.top=TopHeight+252;
  m_rcHuangMen.right=m_rcHuangMen.left+186;
  m_rcHuangMen.bottom=m_rcHuangMen.top+158;

  //筹码数字
  int ExcursionY=30;
  m_PointJettonNumber[ID_TIAN_MEN-1].SetPoint((m_rcTianMen.right+m_rcTianMen.left)/2, (m_rcTianMen.bottom+m_rcTianMen.top)/2-ExcursionY);
  m_PointJettonNumber[ID_DI_MEN -1].SetPoint((m_rcDimen.right+m_rcDimen.left)/2, (m_rcDimen.bottom+m_rcDimen.top)/2-ExcursionY);
  m_PointJettonNumber[ID_HUANG_MEN-1].SetPoint((m_rcHuangMen.right+m_rcHuangMen.left)/2, (m_rcHuangMen.bottom+m_rcHuangMen.top)/2-ExcursionY);
  m_PointJettonNumber[ID_XUAN_MEN-1].SetPoint((m_rcXuanMen.right+m_rcXuanMen.left)/2, (m_rcXuanMen.bottom+m_rcXuanMen.top)/2-ExcursionY);

  //筹码位置
  m_PointJetton[ID_TIAN_MEN-1].SetPoint(m_rcTianMen.left, m_rcTianMen.top);
  m_PointJetton[ID_DI_MEN-1].SetPoint(m_rcDimen.left, m_rcDimen.top);
  m_PointJetton[ID_HUANG_MEN -1].SetPoint(m_rcHuangMen.left, m_rcHuangMen.top);
  m_PointJetton[ID_XUAN_MEN-1].SetPoint(m_rcXuanMen.left, m_rcXuanMen.top);


  //扑克控件
  m_CardControl[0].SetBenchmarkPos(CPoint(nWidth/2,100),enXCenter,enYTop);
  m_CardTypePoint[0] =CPoint(nWidth/2+105,150);

  m_CardControl[1].SetBenchmarkPos(CPoint(LifeWidth+Size.cy+Size.cy/6+83,nHeight/2-10),enXCenter,enYTop);
  m_CardTypePoint[1] =CPoint(LifeWidth+Size.cy+Size.cy/6+105-55,nHeight/2+112);

  m_CardControl[2].SetBenchmarkPos(CPoint(nWidth/2-Size.cy+Size.cy/4-24,nHeight/2-10),enXCenter,enYTop);
  m_CardTypePoint[2] =CPoint(nWidth/2-Size.cy+Size.cy/4-12-45,nHeight/2+112);

  m_CardControl[3].SetBenchmarkPos(CPoint(nWidth/2+Size.cy-Size.cy/4,nHeight/2-10),enXCenter,enYTop);
  m_CardTypePoint[3] =CPoint(nWidth/2+Size.cy-Size.cy/4+20-50,nHeight/2+112);

  m_CardControl[4].SetBenchmarkPos(CPoint(nWidth-LifeWidth-Size.cy-Size.cy/6-107,nHeight/2-10),enXCenter,enYTop);
  m_CardTypePoint[4] =CPoint(nWidth-LifeWidth-Size.cy-Size.cy/6-97-45,nHeight/2+112);

  m_CardControl[5].SetBenchmarkPos(CPoint(nWidth/2,nHeight/2-100),enXCenter,enYTop);

  /*BYTE card[5] = {0x01,0x01,0x01,0x01,0x01};
  for (int i = 0;i<5;i++)
  {
    m_CardControl[i].SetCardData(card,5,false);
    m_CardControl[i].OnCopyCard();
    m_CardControl[i].m_blShowLineResult = false;
  }*/

  //用户列表位置
  m_RectUserList.SetRect(12,nHeight-150-5+14,12+203,nHeight-148-5+14+126);
  //聊天显示位置
  int startX = nWidth-m_ImageChatBack.GetWidth()+7;
  int startY = nHeight-m_ImageChatBack.GetHeight()+25;
  m_RectChartDisplay.SetRect(startX,startY,startX+178,startY+96);

  //移动控件
  HDWP hDwp=BeginDeferWindowPos(32);
  const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

  //m_ApplyUser.m_viewHandle = m_hWnd;

  //列表控件
  //DeferWindowPos(hDwp,m_ApplyUser,NULL,LifeWidth+245+20,TopHeight+10+9,311,92,uFlags);
  DeferWindowPos(hDwp,m_btUp,NULL,110+18,190,90/5,18,uFlags);
  DeferWindowPos(hDwp,m_btDown,NULL,110,190,90/5,18,uFlags);

  m_MeInfoRect.top = TopHeight+10+12;
  m_MeInfoRect.left = LifeWidth+245+20+389;

  //筹码按钮
  CRect rcJetton;
  m_btJetton100.GetWindowRect(&rcJetton);
  int nYPos = nHeight-190;
  int nXPos = 260;
  int nSpace = 4;


  DeferWindowPos(hDwp,m_btJetton100,NULL,nWidth/2 - rcJetton.Width()/2 - 6*3 - rcJetton.Width()*3 +10,nYPos,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btJetton1000,NULL,nWidth/2 - rcJetton.Width()/2 - 6*2 - rcJetton.Width()*2 +10,nYPos,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btJetton10000,NULL,nWidth/2 - rcJetton.Width()/2 - 6 - rcJetton.Width() +10 ,nYPos,0,0,uFlags|SWP_NOSIZE);

  DeferWindowPos(hDwp,m_btJetton100000,NULL,nWidth/2 - rcJetton.Width()/2 +10,nYPos,0,0,uFlags|SWP_NOSIZE);

  DeferWindowPos(hDwp,m_btJetton1000000,NULL,nWidth/2 + rcJetton.Width()/2 +6 +10,nYPos,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btJetton5000000,NULL,nWidth/2 + rcJetton.Width()/2 +6*2 + rcJetton.Width() +10,nYPos,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btJetton50000,NULL,nWidth/2 + rcJetton.Width()/2 +6*3 + rcJetton.Width()*2 +10,nYPos,0,0,uFlags|SWP_NOSIZE);


  //DeferWindowPos(hDwp,m_btJetton1000000,NULL,nXPos + nSpace * 4 + rcJetton.Width() * 4,nYPos,0,0,uFlags|SWP_NOSIZE);
  //DeferWindowPos(hDwp,m_btJetton5000000,NULL,nXPos + nSpace * 5 + rcJetton.Width() *5,nYPos,0,0,uFlags|SWP_NOSIZE);
  //DeferWindowPos(hDwp,m_btJetton50000,NULL,nXPos + nSpace * 6 + rcJetton.Width() * 6,nYPos,0,0,uFlags|SWP_NOSIZE);

  //上庄按钮
  DeferWindowPos(hDwp,m_btApplyBanker,NULL,13,305,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btCancelBanker,NULL,13,305,0,0,uFlags|SWP_NOSIZE);

  DeferWindowPos(hDwp,m_btScoreMoveL,NULL,IsZoomedMode()? CalFrameGap()+256:256,nHeight - 83,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btScoreMoveR,NULL,IsZoomedMode()? nWidth-CalFrameGap() - 266:nWidth- 266,nHeight - 83,0,0,uFlags|SWP_NOSIZE);

  //开牌按钮
  DeferWindowPos(hDwp,m_btContinueCard,NULL,LifeWidth+624+290,TopHeight+205+250,0,0,uFlags|SWP_NOSIZE);

  DeferWindowPos(hDwp,m_btAutoOpenCard,NULL,LifeWidth+624+290,TopHeight+205+285,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btOpenCard,NULL,LifeWidth+624+290,TopHeight+205+320,0,0,uFlags|SWP_NOSIZE);

  //其他按钮
  DeferWindowPos(hDwp,m_btBank,NULL,17,nHeight - 247,0,0,uFlags|SWP_NOSIZE);

  //框架按钮
  DeferWindowPos(hDwp,m_btMin,NULL, nWidth - 10-25*4-5*3,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btMax,NULL, nWidth - 10-25*3-5*2,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btRestore,NULL, nWidth - 10-25*3-5*2,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btSound,NULL, nWidth - 10-25*2-5,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btClose,NULL, nWidth - 10-25,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btOption,NULL, nWidth - 10-25*5-5*4,2,0,0,uFlags|SWP_NOSIZE);

  //列表控件
  DeferWindowPos(hDwp,m_UserList,NULL,m_RectUserList.left,m_RectUserList.top,m_RectUserList.Width(),m_RectUserList.Height(),uFlags);
  DeferWindowPos(hDwp,m_ChatDisplay,NULL,m_RectChartDisplay.left,m_RectChartDisplay.top,m_RectChartDisplay.Width()/*-16*/,m_RectChartDisplay.Height(),uFlags);

  DeferWindowPos(hDwp, m_ChatInput, NULL, nWidth-193, nHeight-29, 140, 20, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
  DeferWindowPos(hDwp,m_btSendChat,NULL,nWidth-52,nHeight-30,0,0,uFlags|SWP_NOSIZE);

  //结束移动
  EndDeferWindowPos(hDwp);

  //控制客户端
  if(m_pClientControlDlg)
  {
    CRect rcClient;
    GetWindowRect(rcClient);
    CRect rcControl;
    m_pClientControlDlg->GetWindowRect(rcControl);
    m_pClientControlDlg->MoveWindow(rcClient.left+(rcClient.Width()-rcControl.Width())/2,
                                    rcClient.top+(rcClient.Height()-rcControl.Height())/2, rcControl.Width(), rcControl.Height());
  }

  m_UserList.ReSetLoc(nWidth,nHeight);
  m_ChatDisplay.ReSetLoc(nWidth,nHeight);


  // 按钮设置
  int nHistoryCount = (m_nRecordLast - m_nRecordFirst + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
  if(!IsZoomedMode() && NORMAL_MAX_FALG_COUNT < nHistoryCount)
  {
    m_btScoreMoveR.EnableWindow(TRUE);
  }
  if(IsZoomedMode() && ZOOMED_MAX_FALG_COUNT < nHistoryCount)
  {
    m_btScoreMoveR.EnableWindow(TRUE);
  }

  //移到最新记录
  if(!IsZoomedMode() && NORMAL_MAX_FALG_COUNT < nHistoryCount)
  {
    m_nScoreHead = (m_nRecordLast - NORMAL_MAX_FALG_COUNT + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
    m_btScoreMoveL.EnableWindow(TRUE);
    m_btScoreMoveR.EnableWindow(FALSE);
  }

  if(IsZoomedMode() && ZOOMED_MAX_FALG_COUNT < nHistoryCount)
  {
    m_nScoreHead = (m_nRecordLast - ZOOMED_MAX_FALG_COUNT + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
    m_btScoreMoveL.EnableWindow(TRUE);
    m_btScoreMoveR.EnableWindow(FALSE);
  }
  if(IsZoomedMode() && ZOOMED_MAX_FALG_COUNT >= nHistoryCount)
  {
    m_nScoreHead = 0;
    m_btScoreMoveL.EnableWindow(FALSE);
    m_btScoreMoveR.EnableWindow(FALSE);
  }


  return;
}
//命令函数
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
  switch(LOWORD(wParam))
  {
    case IDC_UP:
      OnUp();
      break;
    case IDC_DOWN:
      OnDown();
      break;
//  case IDC_OPEN_CARD:
//      OnOpenCard();
//      break;
    case IDC_BANK_DRAW:
      //OnBankDraw();
      break;
    case IDC_BT_ADMIN:
      OpenAdminWnd();
      break;
    case IDC_APPY_BANKER:
      OnApplyBanker();
      break;
    case IDC_SCORE_MOVE_L:
      OnScoreMoveL();
      break;
    case IDC_SCORE_MOVE_R:
      OnScoreMoveR();
      break;
    case IDC_CANCEL_BANKER:
      OnCancelBanker();
      break;
    case IDC_AUTO_OPEN_CARD:
      OnAutoOpenCard();
      break;
    case IDC_JETTON_BUTTON_100:
    {
      //设置变量
      m_lCurrentJetton=100L;
      break;
    }
    case IDC_JETTON_BUTTON_1000:
    {
      //设置变量
      m_lCurrentJetton=1000L;
      break;
    }
    case IDC_JETTON_BUTTON_10000:
    {
      //设置变量
      m_lCurrentJetton=10000L;
      break;
    }
    case IDC_JETTON_BUTTON_50000:
    {
      //设置变量
      m_lCurrentJetton=10000000L;
      break;
    }
    case IDC_JETTON_BUTTON_100000:
    {
      //设置变量
      m_lCurrentJetton=100000L;
      break;
    }
    //case IDC_JETTON_BUTTON_500000:
    //  {
    //    //设置变量
    //    m_lCurrentJetton=500000L;
    //    break;
    //  }
    case IDC_JETTON_BUTTON_1000000:
    {
      //设置变量
      m_lCurrentJetton=1000000L;
      break;
    }
    case IDC_JETTON_BUTTON_5000000:
    {
      //设置变量
      m_lCurrentJetton=5000000L;
      break;
    }
    case IDC_BANK:
    {
      OnBankDraw();
      //OnButtonGoBanker();
      break;
    }
    case IDC_CONTINUE_CARD:
    {
      SendEngineMessage(IDM_CONTINUE_CARD,0,0);
      break;
    }
    case IDC_OPTION:
    {
      //显示配置
      CDlgGameOption DlgGameOption;
      if(DlgGameOption.CreateGameOption(NULL,0)==true)
      {
        break;
      }

      //SendEngineMessage(IDM_GAME_OPTIONS,1,0);
      break;
    }
    case IDC_MIN:
    {
      AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_MIN,0);
      break;
    }
    case IDC_CLOSE:
    {
      OnButtonClose();
      break;
    }
    case IDC_MAX:
    {
      AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_MAX,0);
      m_btRestore.ShowWindow(SW_SHOW);
      m_btMax.ShowWindow(SW_HIDE);
      break;
    }
    case IDC_RESTORE:
    {
      AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_RESTORE,0);
      m_btRestore.ShowWindow(SW_HIDE);
      m_btMax.ShowWindow(SW_SHOW);
      break;
    }
    case IDC_SOUND:
    {
      SendEngineMessage(IDM_GAME_SOUND,0,0);
      break;
    }
    case IDC_SEND_CHAT:
    {
      OnSendMessage();
      return TRUE;
    }
  }
  return __super::OnCommand(wParam, lParam);
}

//关闭游戏
void CGameClientView::OnButtonClose()
{
  CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
  if(pGameClientEngine->GetGameStatus()!=GAME_STATUS_FREE)
  {
    //构造数据
    CMessageTipDlg GameTips;

    //配置数据
    if(GameTips.DoModal()==IDOK)
    {
      AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_CLOSE);
    }
    else
    {
      return;
    }
  }
  else
  {
    AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_CLOSE);
  }
}

void CGameClientView::OnSendMessage()
{
  CString str, mes;
  GetDlgItem(IDC_CHAT_INPUT)->GetWindowText(str);
  if(str.GetLength() < 1)
  {
    return;
  }

  if(false == EfficacyUserChat(str, 0))
  {
    return;
  }

  //查询接口
  IClientKernel * m_pIClientKernel;
  CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
  ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
  m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
  m_pIClientKernel->SendUserChatMessage(0, str, RGB(0,0,0));

  GetDlgItem(IDC_CHAT_INPUT)->SetWindowText(TEXT(""));
  GetDlgItem(IDC_CHAT_INPUT)->SetFocus();

  return;
}

//聊天效验
bool CGameClientView::EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex)
{
  IClientKernel * m_pIClientKernel;
  CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
  ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
  m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);

  IClientUserItem * m_pIMySelfUserItem = GetClientUserItem(m_wMeChairID);

  //状态判断
  if(m_pIClientKernel==NULL)
  {
    return false;
  }
  if(m_pIMySelfUserItem==NULL)
  {
    return false;
  }

  IStringMessage * pIStringMessage = m_pIClientKernel->GetIStringMessage();

  //长度比较
  if(pszChatString != NULL)
  {
    CString strChat=pszChatString;
    if(strChat.GetLength() >= (LEN_USER_CHAT/2))
    {
      pIStringMessage->InsertCustomString(TEXT("抱歉，您输入的聊天内容过长，请缩短后再发送！\r\n"),COLOR_WARN);
      return false;
    }
  }

  //变量定义
  BYTE cbMemberOrder=m_pIMySelfUserItem->GetMemberOrder();
  BYTE cbMasterOrder=m_pIMySelfUserItem->GetMasterOrder();

  //属性定义
  tagUserAttribute * pUserAttribute=m_pIClientKernel->GetUserAttribute();
  tagServerAttribute * pServerAttribute=m_pIClientKernel->GetServerAttribute();

  //房间配置
  if(CServerRule::IsForfendGameChat(pServerAttribute->dwServerRule)&&(cbMasterOrder==0))
  {
    //原始消息
    if(wExpressionIndex==INVALID_WORD)
    {
      CString strDescribe;
      strDescribe.Format(TEXT("\n“%s”发送失败"),pszChatString);
      pIStringMessage->InsertCustomString(strDescribe,COLOR_EVENT);
    }

    //插入消息
    pIStringMessage->InsertSystemString(TEXT("抱歉，当前此游戏房间禁止用户房间聊天！"));

    return false;
  }

  //权限判断
  if(CUserRight::CanGameChat(pUserAttribute->dwUserRight)==false)
  {
    //原始消息
    if(wExpressionIndex==INVALID_WORD)
    {
      CString strDescribe;
      strDescribe.Format(TEXT("\n“%s”发送失败"),pszChatString);
      pIStringMessage->InsertCustomString(strDescribe,COLOR_EVENT);
    }

    //插入消息
    pIStringMessage->InsertSystemString(TEXT("抱歉，您没有游戏发言的权限，若需要帮助，请联系游戏客服咨询！"));

    return false;
  }

  //速度判断
  DWORD dwCurrentTime=(DWORD)time(NULL);
  if((cbMasterOrder==0)&&((dwCurrentTime-m_dwChatTime)<=TIME_USER_CHAT))
  {
    //原始消息
    if(wExpressionIndex==INVALID_WORD)
    {
      CString strDescribe;
      strDescribe.Format(TEXT("\n“%s”发送失败"),pszChatString);
      pIStringMessage->InsertCustomString(strDescribe,COLOR_EVENT);
    }

    //插入消息
    pIStringMessage->InsertSystemString(TEXT("您的说话速度太快了，请坐下来喝杯茶休息下吧！"));

    return false;
  }

  //设置变量
  m_dwChatTime=dwCurrentTime;
  //m_strChatString=pszChatString;

  return true;
}

//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
  //绘画背景
  //DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);
  //DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);


  CDC* pDCBuff = pDC;

  //绘画背景
  /*for ( int iW = 0 ; iW < nWidth; iW += m_ImageViewFill.GetWidth() )
  {
    for ( int iH = 0;  iH < nHeight; iH += m_ImageViewFill.GetHeight() )
    {
      m_ImageViewFill.BitBlt(pDC->GetSafeHdc(), iW, iH);
    }
  }*/
  // 1024 740
  // 1280 770
  m_ImageViewFill.DrawImageTile(pDC,0,0,nWidth,nHeight);

  m_ImageViewBack.DrawImage(pDC, nWidth/2 - m_ImageViewBack.GetWidth()/2, 35);

  m_ImageBankerInfo.DrawImage(pDC,15,38);
  m_ImagePlayerInfo.DrawImage(pDC,nWidth - m_ImagePlayerInfo.GetWidth() - 5,38);
  m_ImageChipInfo.DrawImage(pDC,nWidth - m_ImagePlayerInfo.GetWidth()+5,40 + m_ImagePlayerInfo.GetHeight());

  INT nHistoryMidWidth = m_ImageHistoryMid.GetWidth(); //32
  INT nHistoryMidHeight = m_ImageHistoryMid.GetHeight(); // 164

  INT nHistoryFrontWidth = m_ImageHistoryFront.GetWidth(); // 293
  INT nHistoryFrontHeight = m_ImageHistoryFront.GetHeight(); // 164
  INT nHistoryBackWidth = m_ImageHistoryBack.GetWidth();  // 334
  INT nHistoryBackHeight = m_ImageHistoryBack.GetHeight(); // 164

  INT nUserBackWidth = m_ImageUserBack.GetWidth();  // 218
  INT nChatBackWidth = m_ImageChatBack.GetWidth();  // 194
  // 左右上下间隙 2pixel


  m_ImageWaitValleys.DrawImage(pDC, 15, 190);

  // 正常模式
  if(!IsZoomedMode())
  {
    m_ImageHistoryFront.DrawImage(pDC,2 + nUserBackWidth,nHeight - nHistoryFrontHeight+10);
    m_ImageHistoryBack.DrawImage(pDC,nWidth - 2 - nChatBackWidth -nHistoryBackWidth ,nHeight - nHistoryFrontHeight+10);
  }
  else // 全屏  20个标识
  {
    m_ImageHistoryFront.DrawImage(pDC,2 + nUserBackWidth + CalFrameGap(),nHeight - nHistoryFrontHeight+10);
    for(WORD i = 0; i < 7; i++)
    {
      m_ImageHistoryMid.DrawImage(pDC,2 + nUserBackWidth + CalFrameGap() + nHistoryFrontWidth + i*nHistoryMidWidth,nHeight - nHistoryMidHeight+10);
    }
    m_ImageHistoryBack.DrawImage(pDC,2 + nUserBackWidth + CalFrameGap() + nHistoryFrontWidth + 7*nHistoryMidWidth,nHeight - nHistoryBackHeight+10);

  }



  //获取状态
  BYTE cbGameStatus=m_pGameClientDlg->GetGameStatus();

  //状态提示
  CFont static InfoFont;
  InfoFont.CreateFont(-16,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
  CFont * pOldFont=pDC->SelectObject(&InfoFont);
  pDC->SetTextColor(RGB(255,234,0));

  pDC->SelectObject(pOldFont);
  InfoFont.DeleteObject();

  //时间提示
  int nTimeFlagWidth = m_ImageTimeFlag.GetWidth()/3;
  int nFlagIndex=0;
  if(cbGameStatus==GAME_SCENE_FREE)
  {
    nFlagIndex=1;
  }
  else if(cbGameStatus==GAME_SCENE_PLACE_JETTON)
  {
    nFlagIndex=2;
  }
  else if(cbGameStatus==GAME_SCENE_GAME_END)
  {
    nFlagIndex=0;
  }
  m_ImageTimeFlag.DrawImage(pDC, 255, 145, nTimeFlagWidth, m_ImageTimeFlag.GetHeight(), nFlagIndex * nTimeFlagWidth, 0);

  //Dlg里面的SetGameTimer(GetMeChairID()）里面调用的SwitchViewChairID在桌子人数大于8时实际是ViewID转化为0了

  if(m_wMeChairID!=INVALID_CHAIR)
  {
    WORD wUserTimer=GetUserClock(m_wMeChairID);
    if(wUserTimer!=0)
    {
      DrawTime(pDC, wUserTimer, 270, 65);
    }
  }

  //胜利边框
  FlashJettonAreaFrame(nWidth,nHeight,pDCBuff);

  //筹码资源
  CSize static SizeJettonItem(m_ImageJettonView.GetWidth()/7,m_ImageJettonView.GetHeight());

  //绘画筹码
  for(INT i=0; i<AREA_COUNT; i++)
  {
    //变量定义
    LONGLONG lScoreCount=0L;
    LONGLONG static lScoreJetton[JETTON_COUNT]= {100L,1000L,10000L,100000L,1000000L,5000000L,10000000L};
    int static nJettonViewIndex=0;

    //绘画筹码
    for(INT_PTR j=0; j<m_JettonInfoArray[i].GetCount(); j++)
    {
      //获取信息
      tagJettonInfo * pJettonInfo=&m_JettonInfoArray[i][j];

      //累计数字
      ASSERT(pJettonInfo->cbJettonIndex<JETTON_COUNT);
      lScoreCount+=lScoreJetton[pJettonInfo->cbJettonIndex];

      //图片索引
      /*if (1==pJettonInfo->cbJettonIndex || 2==pJettonInfo->cbJettonIndex) nJettonViewIndex=pJettonInfo->cbJettonIndex+1;
      else if (0==pJettonInfo->cbJettonIndex) nJettonViewIndex=pJettonInfo->cbJettonIndex;*/
      nJettonViewIndex=pJettonInfo->cbJettonIndex;

      //绘画界面
      m_ImageJettonView.DrawImage(pDC,pJettonInfo->nXPos+m_PointJetton[i].x,pJettonInfo->nYPos+m_PointJetton[i].y,SizeJettonItem.cx,SizeJettonItem.cy,nJettonViewIndex*SizeJettonItem.cx,0);
      /*m_ImageJettonView.TransDrawImage(pDCBuff,pJettonInfo->nXPos+m_PointJetton[i].x,
        pJettonInfo->nYPos+m_PointJetton[i].y,SizeJettonItem.cx,SizeJettonItem.cy,
        nJettonViewIndex*SizeJettonItem.cx,0,RGB(255,0,255));*/
    }

    //绘画数字
    if(lScoreCount>0L)
    {
      DrawNumberString(pDCBuff,lScoreCount,m_PointJettonNumber[i].x,m_PointJettonNumber[i].y);
    }
  }

  //绘画庄家
  DrawBankerInfo(pDCBuff,nWidth,nHeight);

  //绘画用户
  DrawMeInfo(pDCBuff,nWidth,nHeight);

  //绘画上庄列表
  DrawBankerList(pDC,nWidth,nHeight);

  //切换庄家
  if(m_bShowChangeBanker)
  {
    int nXPos = nWidth / 2 - 88;
    int nYPos = nHeight / 2 - 160;

    //由我做庄
    if(m_wMeChairID == m_wBankerUser)
    {
      m_ImageMeBanker.DrawImage(pDC, nXPos, nYPos);
    }
    else if(m_wBankerUser != INVALID_CHAIR)
    {
      m_ImageChangeBanker.DrawImage(pDC, nXPos, nYPos);
    }
    else
    {
      m_ImageNoBanker.DrawImage(pDC, nXPos, nYPos);
    }
  }

  //发牌提示
  if(enDispatchCardTip_NULL!=m_enDispatchCardTip)
  {
    if(m_ImageDispatchCardTip.IsNull()==false)
    {
      m_ImageDispatchCardTip.DestroyImage();
    }
    if(enDispatchCardTip_Dispatch==m_enDispatchCardTip)
    {
      m_ImageDispatchCardTip.LoadImage(AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"));
    }
    else
    {
      m_ImageDispatchCardTip.LoadImage(AfxGetInstanceHandle(),TEXT("CONTINUE_CARD"));
    }
    m_ImageDispatchCardTip.DrawImage(pDC, (nWidth-m_ImageDispatchCardTip.GetWidth())/2, nHeight/2);
  }

  //我的下注
  DrawMeJettonNumber(pDCBuff);

  /*test*/
  if(0)
  {
    //绘画扑克
    for(int i=0; i<CountArray(m_CardControl); ++i)
    {
      m_CardControl[i].DrawCardControl(pDCBuff);
    }
  }

  //胜利标志
  DrawWinFlags(pDCBuff);

//  if(0)
//  {
//    m_bShowGameResult= 1;
//    DrawType(pDC,0);
//  }
//  if(m_blMoveFinish)
  {
    //显示结果
    DrawType(pDCBuff,0);

  }

  //结束状态
  if(cbGameStatus==GAME_SCENE_GAME_END)
  {
    //绘画扑克
    for(int i=0; i<CountArray(m_CardControl); ++i)
    {
      m_CardControl[i].DrawCardControl(pDCBuff);
    }

  }
  if(m_blMoveFinish)
  {
    ShowGameResult(pDCBuff, nWidth, nHeight);
  }

  //绘画框架
  DrawGameFrame(pDC, nWidth, nHeight);

  //聊天输入
  CDC dcBuffer;
  CBitmap bmpBuffer;
  m_brush.DeleteObject();
  dcBuffer.CreateCompatibleDC(pDC);
  bmpBuffer.CreateCompatibleBitmap(pDC, 140, 20);
  CBitmap *pBmpBuffer = dcBuffer.SelectObject(&bmpBuffer);
  dcBuffer.BitBlt(0, 0, 140, 20, pDC, nWidth-193, nHeight-29, SRCCOPY);
  dcBuffer.SelectObject(pBmpBuffer);
  m_brush.CreatePatternBrush(&bmpBuffer);
  m_ChatDisplay.Invalidate(TRUE);

  return;
}

//更新按钮
VOID CGameClientView::UpdateButtonContron()
{
  bool bEnableDown = false;
  bool bEnableUp = false;
  INT nUserCount = m_ValleysList.GetCount();

  if(nUserCount <= 5)
  {
    m_nShowValleyIndex = 0;
    bEnableDown = false;
    bEnableUp = false;
  }
  else
  {
    bEnableDown = ((m_nShowValleyIndex + 5) < nUserCount)?true:false;
    bEnableUp = (m_nShowValleyIndex > 0)?true:false;
  }

  m_btDown.EnableWindow(bEnableDown);
  m_btUp.EnableWindow(bEnableUp);
  return;
}

//绘画框架
void CGameClientView::DrawGameFrame(CDC *pDC, int nWidth, int nHeight)
{
  m_ImageUserBack.DrawImage(pDC,2,nHeight - m_ImageUserBack.GetHeight());
  m_ImageChatBack.DrawImage(pDC,nWidth-m_ImageChatBack.GetWidth()-2,nHeight-m_ImageChatBack.GetHeight()-3);
  //上部分
  m_ImageGameFrame[0].DrawImage(pDC,0,0);
  for(int i=m_ImageGameFrame[0].GetWidth(); i<nWidth-m_ImageGameFrame[2].GetWidth(); i+=m_ImageGameFrame[1].GetWidth())
  {
    m_ImageGameFrame[1].DrawImage(pDC,i,0);
  }
  m_ImageGameFrame[2].DrawImage(pDC,nWidth-m_ImageGameFrame[2].GetWidth(),0);

  //中间两边部分
  for(int iH=m_ImageGameFrame[0].GetHeight(); iH<nHeight-m_ImageGameFrame[3].GetHeight(); iH+=m_ImageGameFrame[3].GetHeight())
  {
    m_ImageGameFrame[3].DrawImage(pDC,0,iH);
    m_ImageGameFrame[4].DrawImage(pDC,nWidth-m_ImageGameFrame[4].GetWidth(),iH);
  }

  //下部分
  m_ImageGameFrame[5].DrawImage(pDC,0,nHeight-m_ImageGameFrame[5].GetHeight());
  for(int i=m_ImageGameFrame[5].GetWidth(); i<nWidth-m_ImageGameFrame[6].GetWidth(); i+=m_ImageGameFrame[6].GetWidth())
  {
    m_ImageGameFrame[6].DrawImage(pDC,i,nHeight-m_ImageGameFrame[6].GetHeight());
  }
  m_ImageGameFrame[7].DrawImage(pDC,nWidth-m_ImageGameFrame[7].GetWidth(),nHeight-m_ImageGameFrame[7].GetHeight());

  //绘画房间名
  CRect rectInfo(10, 8, 250, 30);
  DrawTextString(pDC,m_strCaption,RGB(255,255,255),RGB(0,0,0),rectInfo, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_SINGLELINE|DT_LEFT);
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(__super::OnCreate(lpCreateStruct)==-1)
  {
    return -1;
  }
  AfxSetResourceHandle(AfxGetInstanceHandle());

  //加载位图
  HINSTANCE hInstance=AfxGetInstanceHandle();
  m_ImageViewBack.LoadImage(hInstance,TEXT("VIEW_BACK"));
  m_ImageWinFlags.LoadImage(hInstance,TEXT("WIN_FLAGS"));
  m_ImageJettonView.LoadImage(hInstance,TEXT("JETTOM_VIEW"));
  m_ImageScoreNumber.LoadImage(hInstance,TEXT("SCORE_NUMBER"));
  m_ImageMeScoreNumber.LoadImage(hInstance,TEXT("ME_SCORE_NUMBER"));
  m_ImageWaitValleys.LoadImage(hInstance,TEXT("WAIT_VALLEYS"));

  m_ImageBankerInfo.LoadImage(hInstance,TEXT("BANKER_INFO"));
  m_ImagePlayerInfo.LoadImage(hInstance,TEXT("PLAYER_INFO"));
  m_ImageChipInfo.LoadImage(hInstance,TEXT("CHIP_INFO"));

  m_ImageHistoryFront.LoadImage(hInstance,TEXT("HISTORY_FRONT"));
  m_ImageHistoryBack.LoadImage(hInstance,TEXT("HISTORY_BACK"));
  m_ImageHistoryMid.LoadImage(hInstance,TEXT("HISTORY_MID"));
  //边框资源
  m_ImageFrameTianMen.LoadImage(hInstance,TEXT("FRAME_TIAN_MEN_EX"));
  m_ImageFrameDiMen.LoadImage(hInstance,TEXT("FRAME_DI_MEN_EX"));
  //m_ImageFrameQiao.LoadFromResource(hInstance,IDB_FRAME_QIAO_EX);
  m_ImageFrameHuangMen.LoadImage(hInstance,TEXT("FRAME_HUANG_MEN_EX"));
  m_ImageFrameXuanMen.LoadImage(hInstance,TEXT("FRAME_XUAN_MEN_EX"));
  //m_ImageFrameJiaoR.LoadFromResource(hInstance,IDB_FRAME_JIAO_R_EX);
  for(int i = 0; i<5; i++)
  {
    m_ImageCardType[i].LoadImage(hInstance,TEXT("CARDTYPE"));
  }
  m_ImageGameEnd.LoadImage(hInstance,TEXT("GAME_END"));

  m_ImageGameFrame[0].LoadImage(hInstance,TEXT("FRAME_TL"));
  m_ImageGameFrame[1].LoadImage(hInstance,TEXT("FRAME_TM"));
  m_ImageGameFrame[2].LoadImage(hInstance,TEXT("FRAME_TR"));
  m_ImageGameFrame[3].LoadImage(hInstance,TEXT("FRAME_LM"));
  m_ImageGameFrame[4].LoadImage(hInstance,TEXT("FRAME_RM"));
  m_ImageGameFrame[5].LoadImage(hInstance,TEXT("FRAME_BL"));
  m_ImageGameFrame[6].LoadImage(hInstance,TEXT("FRAME_BM"));
  m_ImageGameFrame[7].LoadImage(hInstance,TEXT("FRAME_BR"));

  m_ImageMeBanker.LoadImage(hInstance,TEXT("ME_BANKER"));
  m_ImageChangeBanker.LoadImage(hInstance,TEXT("CHANGE_BANKER"));
  m_ImageNoBanker.LoadImage(hInstance,TEXT("NO_BANKER"));

  m_ImageTimeFlag.LoadImage(hInstance,TEXT("TIME_FLAG"));
  m_ImageTimeBack.LoadImage(hInstance,TEXT("TIME_BACK"));
  m_ImageTimeNumber.LoadImage(hInstance,TEXT("TIME_NUMBER"));

  m_ImageUserBack.LoadImage(hInstance,TEXT("USER_BACK"));
  m_ImageChatBack.LoadImage(hInstance,TEXT("CHAT_BACK"));

  //创建控件
  CRect rcCreate(0,0,0,0);
  m_GameRecord.Create(IDD_DLG_GAME_RECORD,this);
  //m_ApplyUser.Create( IDD_DLG_GAME_RECORD , this );
  m_GameRecord.ShowWindow(SW_HIDE);

  m_ImageViewFill.LoadFromResource(hInstance,IDB_VIEW_FILL);


  //下注按钮
  m_btJetton100.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100);
  m_btJetton1000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000);
  m_btJetton10000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_10000);
  m_btJetton50000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_50000);
  m_btJetton100000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100000);

  m_btJetton500000.Create(NULL,WS_CHILD|/*WS_VISIBLE|*/WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_500000);
  m_btJetton1000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000000);
  m_btJetton5000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_5000000);

  //框架按钮
  m_btOption.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OPTION);
  m_btMin.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MIN);
  m_btRestore.Create(NULL,WS_CHILD,rcCreate,this,IDC_RESTORE);
  m_btClose.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_CLOSE);
  m_btMax.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MAX);
  m_btSound.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SOUND);

  //申请按钮
  m_btApplyBanker.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_APPY_BANKER);
  m_btCancelBanker.Create(NULL,WS_CHILD|WS_DISABLED,rcCreate,this,IDC_CANCEL_BANKER);

  m_btScoreMoveL.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_L);
  m_btScoreMoveR.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_R);

  m_btAutoOpenCard.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_AUTO_OPEN_CARD);
  m_btOpenCard.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OPEN_CARD);
  m_btOpenCard.ShowWindow(SW_HIDE);

  m_btBank.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_BANK);
  m_btContinueCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_CONTINUE_CARD);

  m_btUp.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_UP);
  m_btDown.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_DOWN);

  //  m_btBankerStorage.EnableWindow(TRUE);
  m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(10,40,17,47),this,IDC_BT_ADMIN);
  m_btOpenAdmin.ShowWindow(SW_HIDE);

  m_btSendChat.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SEND_CHAT);

  //设置按钮
  HINSTANCE hResInstance=AfxGetInstanceHandle();
  m_btJetton100.SetButtonImage(IDB_BT_JETTON_100,hResInstance,false,false);
  m_btJetton1000.SetButtonImage(IDB_BT_JETTON_1000,hResInstance,false,false);
  m_btJetton50000.SetButtonImage(IDB_BT_JETTON_50000,hResInstance,false,false);
  m_btJetton10000.SetButtonImage(IDB_BT_JETTON_10000,hResInstance,false,false);
  m_btJetton100000.SetButtonImage(IDB_BT_JETTON_100000,hResInstance,false,false);
  m_btJetton500000.SetButtonImage(IDB_BT_JETTON_500000,hResInstance,false,false);
  m_btJetton1000000.SetButtonImage(IDB_BT_JETTON_1000000,hResInstance,false,false);
  m_btJetton5000000.SetButtonImage(IDB_BT_JETTON_5000000,hResInstance,false,false);

  m_btOption.SetButtonImage(IDB_BT_OPTION,hResInstance,false,false);
  m_btMin.SetButtonImage(IDB_BT_MIN,hResInstance,false,false);
  m_btClose.SetButtonImage(IDB_BT_CLOSE,hResInstance,false,false);
  m_btMax.SetButtonImage(IDB_BT_MAX,hResInstance,false,false);
  m_btRestore.SetButtonImage(IDB_BT_RESTORE,hResInstance,false,false);
  m_btSound.SetButtonImage(IDB_BT_SOUND_OPEN,hResInstance,false,false);

  m_btSendChat.SetButtonImage(IDB_BT_SEND_CHAT,hInstance,false,false);

  m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false,false);
  m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,hResInstance,false,false);

  m_btUp.SetButtonImage(IDB_BT_BT_S,hResInstance,false,false);
  m_btDown.SetButtonImage(IDB_BT_BT_X,hResInstance,false,false);

  m_btScoreMoveL.SetButtonImage(IDB_BT_SCORE_MOVE_L,hResInstance,false,false);
  m_btScoreMoveR.SetButtonImage(IDB_BT_SCORE_MOVE_R,hResInstance,false,false);

  m_btAutoOpenCard.SetButtonImage(IDB_BT_AUTO_OPEN_CARD,hResInstance,false,false);
  m_btOpenCard.SetButtonImage(IDB_BT_OPEN_CARD,hResInstance,false,false);

  m_btBank.SetButtonImage(IDB_BT_BANK,hResInstance,false,false);
  m_btContinueCard.SetButtonImage(IDB_BT_CONTINUE_CARD,hResInstance,false,false);

  m_ChatDisplay.Create(ES_AUTOVSCROLL|ES_WANTRETURN|ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP |WS_VSCROLL, rcCreate, this, IDC_CHAT_DISPLAY);
  m_ChatInput.Create(ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER, rcCreate, this, IDC_CHAT_INPUT);

  LOGFONT lf;
  memset(&lf, 0, sizeof(LOGFONT));
  lf.lfHeight = 15;
  mystrcpy(lf.lfFaceName, TEXT("Arial"));

  m_ChatInput.SetLimitText(30);

  m_ChatDisplay.SetTextFont(lf);
  //m_ChatDisplay.SetBackColor(RGB(88,68,43));
  m_ChatDisplay.SetTextColor(RGB(255,255,255));

  m_ChatDisplay.SetBackgroundColor(false,RGB(31,27,20));
  m_ChatDisplay.SetReadOnly(TRUE);

  m_UserList.Create(LVS_REPORT | LVS_OWNERDRAWFIXED | WS_CHILD | WS_VISIBLE ,rcCreate, this, IDC_USER_LIST);

  m_UserList.SetBkColor(RGB(31,27,20));
  m_UserList.SetTextColor(RGB(255,255,255));
  m_UserList.SetTextBkColor(RGB(31,27,20));
  int nColumnCount=0;

  //用户标志
  m_UserList.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,35,0);
  m_UserList.InsertColumn(nColumnCount++,TEXT("昵称"),LVCFMT_LEFT,75);
  m_UserList.InsertColumn(nColumnCount++,TEXT("财富"),LVCFMT_LEFT,95);

  //扑克控件
  for(int i=0; i<CountArray(m_CardControl); ++i)
  {
    m_CardControl[i].SetDisplayFlag(true);
  }

  //控制
  m_hInst = NULL;
  m_pClientControlDlg = NULL;
  m_hInst = LoadLibrary(TEXT("OxBattleClientControl.dll"));
  if(m_hInst)
  {
    typedef void * (*CREATE)(CWnd* pParentWnd);
    CREATE ClientControl = (CREATE)GetProcAddress(m_hInst,"CreateClientControl");
    if(ClientControl)
    {
      m_pClientControlDlg = static_cast<IClientControlDlg*>(ClientControl(this));

      m_pClientControlDlg->ShowWindow(SW_HIDE);
    }
  }

  m_DFontExT.CreateFont(this, TEXT("宋体"), 12, 400);
  m_DBigFont.CreateFont(this, TEXT("宋体"), 16, 400);

  m_brush.CreateSolidBrush(RGB(255,0,255));

  return 0;
}
void CGameClientView::SetMoveCardTimer()
{
  KillTimer(IDI_POSTCARD);
  m_OpenCardIndex = 1;
  m_bcShowCount = 0;
  m_PostStartIndex =  m_GameLogic.GetCardValue(m_bcfirstShowCard);
  if(m_PostStartIndex>1)
  {
    m_PostStartIndex  = (m_PostStartIndex-1)%5;
  }
  m_PostCardIndex=m_PostStartIndex;
  for(int i = 0; i<5; i++)
  {
    m_CardControl[i].m_blAdd = true;
    m_CardControl[i].m_blAuto = m_blAutoOpenCard;
    m_CardControl[i].m_blGameEnd = true;
    m_CardControl[i].m_blShowResult = false;
    m_CardControl[i].m_blShowLineResult = false;
  }
  SetTimer(IDI_POSTCARD,300,NULL);
  SetTimer(IDI_DISPATCH_CARD,25000,NULL);

}

//设置信息
void CGameClientView::SetMeMaxScore(LONGLONG lMeMaxScore)
{
  if(m_lMeMaxScore!=lMeMaxScore)
  {
    //设置变量
    m_lMeMaxScore=lMeMaxScore;
  }

  return;
}

//最大下注
void CGameClientView::SetAreaLimitScore(LONGLONG lAreaLimitScore)
{
  if(m_lAreaLimitScore!= lAreaLimitScore)
  {
    //设置变量
    m_lAreaLimitScore=lAreaLimitScore;
  }
}

//设置筹码
void CGameClientView::SetCurrentJetton(LONGLONG lCurrentJetton)
{
  //设置变量
  ASSERT(lCurrentJetton>=0L);
  m_lCurrentJetton=lCurrentJetton;

  return;
}

//历史记录
void CGameClientView::SetGameHistory(bool bWinTian, bool bWinDi, bool bWinXuan,bool bWinHuang)
{
  //设置数据
  tagClientGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
  GameRecord.bWinTianMen=bWinTian;
  GameRecord.bWinDiMen =bWinDi;
  GameRecord.bWinXuanMen=bWinXuan;
  GameRecord.bWinHuangMen=bWinHuang;

  //操作类型
  if(0==m_lUserJettonScore[ID_TIAN_MEN])
  {
    GameRecord.enOperateTianMen=enOperateResult_NULL;
  }
  else if(m_lUserJettonScore[ID_TIAN_MEN] > 0 && true==bWinTian)
  {
    GameRecord.enOperateTianMen=enOperateResult_Win;
  }
  else if(m_lUserJettonScore[ID_TIAN_MEN] > 0 && false==bWinTian)
  {
    GameRecord.enOperateTianMen=enOperateResult_Lost;
  }

  if(0==m_lUserJettonScore[ID_DI_MEN])
  {
    GameRecord.enOperateDiMen=enOperateResult_NULL;
  }
  else if(m_lUserJettonScore[ID_DI_MEN] > 0 && true==bWinDi)
  {
    GameRecord.enOperateDiMen=enOperateResult_Win;
  }
  else if(m_lUserJettonScore[ID_DI_MEN] >0 && false==bWinDi)
  {
    GameRecord.enOperateDiMen=enOperateResult_Lost;
  }

  if(0==m_lUserJettonScore[ID_XUAN_MEN])
  {
    GameRecord.enOperateXuanMen=enOperateResult_NULL;
  }
  else if(m_lUserJettonScore[ID_XUAN_MEN] > 0 && true==bWinXuan)
  {
    GameRecord.enOperateXuanMen=enOperateResult_Win;
  }
  else if(m_lUserJettonScore[ID_XUAN_MEN] >0 && false==bWinXuan)
  {
    GameRecord.enOperateXuanMen=enOperateResult_Lost;
  }


  if(0==m_lUserJettonScore[ID_HUANG_MEN])
  {
    GameRecord.enOperateHuangMen=enOperateResult_NULL;
  }
  else if(m_lUserJettonScore[ID_HUANG_MEN] > 0 && true==bWinHuang)
  {
    GameRecord.enOperateHuangMen=enOperateResult_Win;
  }
  else if(m_lUserJettonScore[ID_HUANG_MEN] >0 && false==bWinHuang)
  {
    GameRecord.enOperateHuangMen=enOperateResult_Lost;
  }

  //移动下标
  m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
  if(m_nRecordLast == m_nRecordFirst)
  {
    m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
    if(m_nScoreHead < m_nRecordFirst)
    {
      m_nScoreHead = m_nRecordFirst;
    }
  }

  int nHistoryCount = (m_nRecordLast - m_nRecordFirst + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
  if(!IsZoomedMode() && NORMAL_MAX_FALG_COUNT < nHistoryCount)
  {
    m_btScoreMoveR.EnableWindow(TRUE);
  }
  if(IsZoomedMode() && ZOOMED_MAX_FALG_COUNT < nHistoryCount)
  {
    m_btScoreMoveR.EnableWindow(TRUE);
  }

  //移到最新记录
  if(!IsZoomedMode() && NORMAL_MAX_FALG_COUNT < nHistoryCount)
  {
    m_nScoreHead = (m_nRecordLast - NORMAL_MAX_FALG_COUNT + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
    m_btScoreMoveL.EnableWindow(TRUE);
    m_btScoreMoveR.EnableWindow(FALSE);
  }

  if(IsZoomedMode() && ZOOMED_MAX_FALG_COUNT < nHistoryCount)
  {
    m_nScoreHead = (m_nRecordLast - ZOOMED_MAX_FALG_COUNT + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
    m_btScoreMoveL.EnableWindow(TRUE);
    m_btScoreMoveR.EnableWindow(FALSE);
  }
  if(IsZoomedMode() && ZOOMED_MAX_FALG_COUNT >= nHistoryCount)
  {
    m_nScoreHead = 0;
    m_btScoreMoveL.EnableWindow(FALSE);
    m_btScoreMoveR.EnableWindow(FALSE);
  }
  return;
}

//清理筹码
void CGameClientView::CleanUserJetton()
{
  //清理数组
  for(BYTE i=0; i<CountArray(m_JettonInfoArray); i++)
  {
    m_JettonInfoArray[i].RemoveAll();
  }

  //下注信息
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //全体下注
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

  if(NULL != m_pClientControlDlg && NULL != m_pClientControlDlg->GetSafeHwnd())
  {
    m_pClientControlDlg->m_UserBetArray.RemoveAll();
    m_pClientControlDlg->UpdateUserBet(true);
  }

  //更新界面
  InvalidGameView(0,0,0,0);

  return;
}

//个人下注
void CGameClientView::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
  //效验参数
  ASSERT(cbViewIndex<=ID_HUANG_MEN);
  if(cbViewIndex>ID_HUANG_MEN)
  {
    return;
  }

  m_lUserJettonScore[cbViewIndex]=lJettonCount;

  //更新界面
  InvalidGameView(0,0,0,0);
}

//设置扑克
void CGameClientView::SetCardInfo(BYTE cbTableCardArray[5][5])
{
  if(cbTableCardArray!=NULL)
  {
    CopyMemory(m_cbTableCardArray,cbTableCardArray,sizeof(m_cbTableCardArray));

    //开始发牌
    DispatchCard();
  }
  else
  {
    ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
  }
}

//设置筹码
void CGameClientView::PlaceUserJetton(BYTE cbViewIndex, LONGLONG lScoreCount)
{
  //效验参数
  ASSERT(cbViewIndex<=ID_HUANG_MEN);
  if(cbViewIndex>ID_HUANG_MEN)
  {
    return;
  }

  //变量定义
  bool bPlaceJetton=false;
  LONGLONG lScoreIndex[JETTON_COUNT]= {100L,1000L,10000L,100000L,1000000L,5000000L,10000000L};

  //边框宽度
  int nFrameWidth=0, nFrameHeight=0;
  int nBorderWidth=6;

  switch(cbViewIndex)
  {
    case ID_TIAN_MEN:
    {
      m_lAllJettonScore[ID_TIAN_MEN] += lScoreCount;
      nFrameWidth = m_rcTianMen.right-m_rcTianMen.left;
      nFrameHeight = m_rcTianMen.bottom-m_rcTianMen.top;
      break;
    }
    case ID_DI_MEN:
    {
      m_lAllJettonScore[ID_DI_MEN] += lScoreCount;
      nFrameWidth = m_rcDimen.right-m_rcDimen.left;
      nFrameHeight = m_rcDimen.bottom-m_rcDimen.top;
      break;
    }
    case ID_HUANG_MEN :
    {
      m_lAllJettonScore[ID_HUANG_MEN] += lScoreCount;
      nFrameWidth = m_rcHuangMen.right-m_rcHuangMen.left;
      nFrameHeight = m_rcHuangMen.bottom-m_rcHuangMen.top;
      break;
    }
    case ID_XUAN_MEN:
    {
      m_lAllJettonScore[ID_XUAN_MEN] += lScoreCount;
      nFrameWidth = m_rcXuanMen.right-m_rcXuanMen.left;
      nFrameHeight = m_rcXuanMen.bottom-m_rcXuanMen.top;
      break;
    }
    default:
    {
      ASSERT(FALSE);
      return;
    }
  }

  nFrameWidth += nBorderWidth;
  nFrameHeight += nBorderWidth;

  //增加判断
  bool bAddJetton=lScoreCount>0?true:false;
  lScoreCount= (lScoreCount>0?lScoreCount:-lScoreCount);

  //增加筹码
  for(BYTE i=0; i<CountArray(lScoreIndex); i++)
  {
    //计算数目
    BYTE cbScoreIndex=JETTON_COUNT-i-1;
    LONGLONG lCellCount=lScoreCount/lScoreIndex[cbScoreIndex];

    //插入过虑
    if(lCellCount==0L)
    {
      continue;
    }

    //加入筹码
    for(LONGLONG j=0; j<lCellCount; j++)
    {
      if(true==bAddJetton)
      {
        //构造变量
        tagJettonInfo JettonInfo;
        int nJettonSize=46;
        JettonInfo.cbJettonIndex=cbScoreIndex;
        JettonInfo.nXPos=rand()%(nFrameWidth-nJettonSize-20)+6;
        JettonInfo.nYPos=rand()%(nFrameHeight-nJettonSize-23-14)+6;

        //插入数组
        bPlaceJetton=true;
        m_JettonInfoArray[cbViewIndex-1].Add(JettonInfo);
      }
      else
      {
        for(int nIndex=0; nIndex<m_JettonInfoArray[cbViewIndex-1].GetCount(); ++nIndex)
        {
          //移除判断
          tagJettonInfo &JettonInfo=m_JettonInfoArray[cbViewIndex-1][nIndex];
          if(JettonInfo.cbJettonIndex==cbScoreIndex)
          {
            m_JettonInfoArray[cbViewIndex-1].RemoveAt(nIndex);
            break;
          }
        }
      }
    }

    //减少数目
    lScoreCount-=lCellCount*lScoreIndex[cbScoreIndex];
  }

  //变量定义
  CRgn JettonRegion;        //显示区域
  CRgn SignedRegion;        //筹码区域
  CRgn SignedAndJettonRegion;   //并集区域
  SignedAndJettonRegion.CreateRectRgn(0,0,0,0);

  //图片区域
  CSize static SizeJettonItem(m_ImageJettonView.GetWidth()/7,m_ImageJettonView.GetHeight());

  int nIndex = cbViewIndex-1;
  //设置区域
  for(INT_PTR j = m_JettonInfoArray[nIndex].GetCount() - 1; j >= 0; j--)
  {
    //获取信息
    tagJettonInfo * pJettonInfo = &m_JettonInfoArray[nIndex][j];
    pJettonInfo->bShow = false;
    CRect rcRect(pJettonInfo->nXPos+m_PointJetton[nIndex].x,
                 pJettonInfo->nYPos+m_PointJetton[nIndex].y,
                 pJettonInfo->nXPos+m_PointJetton[nIndex].x + m_ImageJettonView.GetWidth()/7,
                 pJettonInfo->nYPos+m_PointJetton[nIndex].y + m_ImageJettonView.GetHeight());

    //首次区域
    if(!JettonRegion.GetSafeHandle())
    {
      JettonRegion.CreateEllipticRgnIndirect(rcRect);
      pJettonInfo->bShow = true;
      continue;
    }

    //区域判断
    SignedRegion.CreateEllipticRgnIndirect(rcRect);
    SignedAndJettonRegion.CombineRgn(&JettonRegion,&SignedRegion, RGN_AND);
    if(SignedAndJettonRegion.EqualRgn(&SignedRegion))
    {
      pJettonInfo->bShow = false;
    }
    else
    {
      pJettonInfo->bShow = true;
      JettonRegion.CombineRgn(&JettonRegion,&SignedRegion,RGN_OR);
    }

    //合并区域
    SignedRegion.DeleteObject();
  }

  //删除区域
  if(JettonRegion.GetSafeHandle())
  {
    JettonRegion.DeleteObject();
  }
  if(SignedRegion.GetSafeHandle())
  {
    SignedRegion.DeleteObject();
  }
  if(SignedAndJettonRegion.GetSafeHandle())
  {
    SignedAndJettonRegion.DeleteObject();
  }

  //更新界面
  if(bPlaceJetton==true)
  {
    InvalidGameView(0,0,0,0);
  }

  return;
}

//当局成绩
void CGameClientView::SetCurGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore, LONGLONG lGameRevenue)
{
  m_lMeCurGameScore=lMeCurGameScore;
  m_lMeCurGameReturnScore=lMeCurGameReturnScore;
  m_lBankerCurGameScore=lBankerCurGameScore;
  m_lGameRevenue=lGameRevenue;
}
//设置胜方
void CGameClientView::SetWinnerSide(bool bWinTian, bool bWinDi, bool bWinXuan,bool bWinHuang, bool bSet)
{
  //设置时间
  if(true==bSet)
  {
    //设置定时器
    SetTimer(IDI_FLASH_WINNER,500,NULL);

    //全胜判断
    //if (false==bWinTian && false==bWinDi && false==bWinXuan &&false ==bWinHuang )
    //{
    //  //清除资源
    //  m_ImageFrameTianMen.DestroyImage();
    //  m_ImageFrameDiMen.DestroyImage();
    //  m_ImageFrameHuangMen.DestroyImage();
    //  m_ImageFrameXuanMen.DestroyImage();
    //  //重设资源
    //  HINSTANCE hInstance=AfxGetInstanceHandle();
    //  m_ImageFrameTianMen.LoadImage(hInstance,TEXT("FRAME_TIAN_EMN"));
    //  m_ImageFrameDiMen.LoadImage(hInstance,TEXT("FRAME_DI_EMN"));
    //  m_ImageFrameHuangMen.LoadImage(hInstance,TEXT("FRAME_HUANG_EMN"));
    //  m_ImageFrameXuanMen.LoadImage(hInstance,TEXT("FRAME_XUAN_EMN"));

    //}
  }
  else
  {
    //清楚定时器
    KillTimer(IDI_FLASH_WINNER);

    //全胜判断
    //if (false==m_bWinXuanMen && false==m_bWinTianMen && false==m_bWinHuangMen)
    //{
    //  //清除资源
    //  m_ImageFrameTianMen.DestroyImage();
    //  m_ImageFrameDiMen.DestroyImage();
    //  m_ImageFrameHuangMen.DestroyImage();
    //  m_ImageFrameXuanMen.DestroyImage();

    //  //重设资源
    //  HINSTANCE hInstance=AfxGetInstanceHandle();
    //  m_ImageFrameTianMen.LoadImage(hInstance,TEXT("FRAME_TIAN_MEN_EX"));
    //  m_ImageFrameDiMen.LoadImage(hInstance,TEXT("FRAME_DI_MEN_EX"));
    //  m_ImageFrameHuangMen.LoadImage(hInstance,TEXT("FRAME_HUANG_MEN_EX"));
    //  m_ImageFrameXuanMen.LoadImage(hInstance,TEXT("FRAME_XUAN_MEN_EX"));
    //}
  }

  //设置变量
  m_bWinTianMen=bWinTian;
  m_bWinHuangMen=bWinHuang;
  m_bWinXuanMen=bWinXuan;
  m_bWinDiMen=bWinDi;
  m_bFlashResult=bSet;
  m_bShowGameResult=bSet;
  m_cbAreaFlash=0xFF;

  //更新界面
  InvalidGameView(0,0,0,0);

  return;
}

//获取区域
BYTE CGameClientView::GetJettonArea(CPoint MousePoint)
{
  if(m_rcTianMen.PtInRect(MousePoint))
  {
    return ID_TIAN_MEN;

  }
  else
  {
    if(m_rcDimen.PtInRect(MousePoint))
    {
      return ID_DI_MEN;

    }

  }
  if(m_rcHuangMen.PtInRect(MousePoint))
  {
    return ID_HUANG_MEN;
  }
  if(m_rcXuanMen.PtInRect(MousePoint))
  {
    return ID_XUAN_MEN ;
  }
  return 0xFF;
}
void CGameClientView::DrawType(CDC* pDC,WORD wChairID)
{
  //if (false==m_bShowGameResult) return;
  if(m_pGameClientDlg->GetGameStatus() != GAME_SCENE_GAME_END)
  {
    return;
  }
  for(int i = 0; i<5; i++)
  {
    if(m_CardControl[i].m_blShowResult==false)
    {
      continue;
    }
    int iIndex = 0;
    //iIndex = CT_SPECIAL_NIUNIUXW-2;
    //m_lUserCardType[IDC_BANK]
    if(m_lUserCardType[i]!= CT_POINT)
    {
      iIndex = m_lUserCardType[i]-2;
      if(iIndex>=13)
      {
        if(iIndex == 13)
        {
          iIndex = 12;
          //break;
        }
        if(iIndex == 14)
        {
          iIndex = 11;
          //break;
        }
        if(iIndex == 15)
        {
          iIndex = 13;
          //  break;
        }
      }
      else
      {
        if(iIndex>10)
        {
          iIndex = 10;
        }
        if(iIndex<0)
        {
          iIndex = 0;
        }
      }
      //iIndex = 11;
    }

    //HandleImageCardType[i](&m_ImageCardType[i]);
    m_ImageCardType[i].DrawImage(pDC,m_CardTypePoint[i].x,m_CardTypePoint[i].y,m_ImageCardType[i].GetWidth()/14,
                                 m_ImageCardType[i].GetHeight(),m_ImageCardType[i].GetWidth()/14*iIndex,0);
  }
}

//绘画数字
void CGameClientView::DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore)
{
  //加载资源
  CSize SizeScoreNumber(m_ImageScoreNumber.GetWidth()/10,m_ImageScoreNumber.GetHeight());

  if(bMeScore)
  {
    SizeScoreNumber.SetSize(m_ImageMeScoreNumber.GetWidth()/11, m_ImageMeScoreNumber.GetHeight());
  }

  //计算数目
  int lNumberCount=0;
  LONGLONG lNumberTemp=lNumber;
  do
  {
    lNumberCount++;
    lNumberTemp/=10;
  }
  while(lNumberTemp>0);

  //位置定义
  int nYDrawPos=nYPos-SizeScoreNumber.cy/2;
  int nXDrawPos=nXPos+lNumberCount*SizeScoreNumber.cx/2-SizeScoreNumber.cx;

  //绘画桌号
  for(int i=0; i<lNumberCount; i++)
  {
    //绘画号码
    int lCellNumber=(int)(lNumber%10);
    if(bMeScore)
    {
      m_ImageMeScoreNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
                                     (lCellNumber+1)*SizeScoreNumber.cx,0);
    }
    else
    {
      m_ImageScoreNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
                                   lCellNumber*SizeScoreNumber.cx,0);
    }

    //设置变量
    lNumber/=10;
    nXDrawPos-=SizeScoreNumber.cx;
  };

  return;
}

//绘画数字
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos)
{
  CString strNumber, strTempNum;
  strTempNum.Format(_T("%I64d"), (lNumber>0?lNumber:-lNumber));

  int nLength = strTempNum.GetLength();
  if(nLength <= 3)
  {
    strNumber.Format(_T("%I64d"), lNumber);
  }
  else
  {
    for(int i = 0; i < int((nLength-1)/3); i++)
    {
      strTempNum.Insert(nLength - 3*(i+1), _T(","));
    }
    strNumber.Format(_T("%s%s"), (lNumber<0?_T("-"):_T("")), strTempNum);
  }

  //输出数字
  //pDC->TextOut(nXPos,nYPos,strNumber);
  m_DFontExT.DrawText(pDC,strNumber,nXPos,nYPos,RGB(255,234,0),DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
}

//绘画数字
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, INT nFormat,bool bBig)
{
  CString strNumber, strTempNum;
  strTempNum.Format(_T("%I64d"), (lNumber>0?lNumber:-lNumber));

  int nLength = strTempNum.GetLength();
  if(nLength <= 3)
  {
    strNumber.Format(_T("%I64d"), lNumber);
  }
  else
  {
    for(int i = 0; i < int((nLength-1)/3); i++)
    {
      strTempNum.Insert(nLength - 3*(i+1), _T(","));
    }
    strNumber.Format(_T("%s%s"), (lNumber<0?_T("-"):_T("")), strTempNum);
  }

  //输出数字
  /*if (nFormat==-1) pDC->DrawText(strNumber,rcRect,DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
  else pDC->DrawText(strNumber,rcRect,nFormat);*/

  if(bBig)
  {
    if(nFormat==-1)
    {
      m_DBigFont.DrawText(pDC, strNumber, rcRect, RGB(253,241,149), DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
    }
    else
    {
      m_DBigFont.DrawText(pDC, strNumber, rcRect, RGB(253,241,149), nFormat);
    }
  }
  else
  {
    if(nFormat==-1)
    {
      m_DFontExT.DrawText(pDC, strNumber, rcRect, RGB(253,241,149), DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
    }
    else
    {
      m_DFontExT.DrawText(pDC, strNumber, rcRect, RGB(253,241,149), nFormat);
    }
  }

}

// 绘画时钟
void CGameClientView::DrawTime(CDC * pDC, WORD wUserTime,INT nXPos, INT nYPos)
{
  //背景数字
  m_ImageTimeBack.DrawImage(pDC, nXPos, nYPos);

  TCHAR szOutNum[128] = {0};
  if(wUserTime > 99)
  {
    _sntprintf(szOutNum,CountArray(szOutNum),TEXT("99"));
  }
  else if(wUserTime < 10)
  {
    _sntprintf(szOutNum,CountArray(szOutNum),TEXT("0%d"), wUserTime);
  }
  else
  {
    _sntprintf(szOutNum,CountArray(szOutNum),TEXT("%d"), wUserTime);
  }

  //背景数字
  DrawNumber(pDC, &m_ImageTimeNumber, TEXT("0123456789"), szOutNum, nXPos + m_ImageTimeBack.GetWidth()/2, nYPos +20, DT_CENTER);
}

// 绘画数字
void CGameClientView::DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, LONGLONG lOutNum,INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/)
{
  TCHAR szOutNum[128] = {0};
  _sntprintf(szOutNum,CountArray(szOutNum),TEXT("%I64d"),lOutNum);
  DrawNumber(pDC, ImageNumber, szImageNum, szOutNum, nXPos, nYPos, uFormat);
}

// 绘画数字
void CGameClientView::DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/)
{
  TCHAR szOutNumT[128] = {0};
  _sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%s"),szOutNum);
  DrawNumber(pDC, ImageNumber, szImageNum, szOutNumT, nXPos, nYPos, uFormat);
}


// 绘画数字
void CGameClientView::DrawNumber(CDC * pDC , CPngImage* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat /*= DT_LEFT*/)
{
  // 加载资源
  INT nNumberHeight=ImageNumber->GetHeight();
  INT nNumberWidth=ImageNumber->GetWidth()/lstrlen(szImageNum);

  if(uFormat == DT_CENTER)
  {
    nXPos -= (INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth);
  }
  else if(uFormat == DT_RIGHT)
  {
    nXPos -= lstrlen(szOutNum) * nNumberWidth;
  }

  for(INT i = 0; i < lstrlen(szOutNum); ++i)
  {
    for(INT j = 0; j < lstrlen(szImageNum); ++j)
    {
      if(szOutNum[i] == szImageNum[j] && szOutNum[i] != '\0')
      {
        ImageNumber->DrawImage(pDC, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight);
        nXPos += nNumberWidth;
        break;
      }
    }
  }
}

void CGameClientView::KillCardTime()
{
  KillTimer(IDI_FLASH_WINNER);
  KillTimer(IDI_POSTCARD);
  KillTimer(IDI_OPENCARD);
  KillTimer(IDI_DISPATCH_CARD);
  KillTimer(IDI_SHOWDISPATCH_CARD_TIP);
  for(int i = 0; i<5; i++)
  {
    m_CardControl[i].m_blShowResult = false;
    m_CardControl[i].m_blShowLineResult = false;
  }
}
//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
  //闪动胜方
  if(nIDEvent==IDI_FLASH_WINNER)
  {
    //设置变量
    m_bFlashResult=!m_bFlashResult;

    //更新界面
    InvalidGameView(0,0,0,0);

    return;
  }
  if(IDI_POSTCARD ==nIDEvent)
  {

    if(m_bcShowCount<5)
    {
      if(m_CardControl[5].m_CardTempItemArray.GetCount()>0)
      {
        m_CardControl[5].m_blhideOneCard = false;
        m_CardControl[5].m_blGameEnd = true;
        m_CardControl[5].SetDisplayFlag(true);
        m_CardControl[5].ShowCardControl(true);
        m_CardControl[5].m_CardItemArray.SetSize(1);
        m_CardControl[5].m_CardItemArray[0]=m_CardControl[5].m_CardTempItemArray[0];
        m_CardControl[5].m_CardItemArray[0].cbCardData = m_bcfirstShowCard;
        m_CardControl[5].m_CardItemArray[0].bShoot = false;
      }

      if(m_bcShowCount == 2)
      {
        m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
      }
    }
    else
    {
      m_CardControl[5].m_CardItemArray.SetSize(0);

      bool blKillTimer = true;
      for(int i = 0; i<5; i++)
      {
        if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
        {
          blKillTimer = false;
          break;
        }
      }
      m_PostCardIndex = m_PostCardIndex%5;
      m_CardControl[m_PostCardIndex].OnCopyCard();
      m_PostCardIndex++;
      if(blKillTimer)
      {
        KillTimer(IDI_POSTCARD);
        SetTimer(IDI_OPENCARD,25,NULL);
      }
      else
      {
        m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
      }

      InvalidGameView(0,0,0,0);
    }
    m_bcShowCount++;


  }
  if(IDI_OPENCARD==nIDEvent)
  {
    if(m_blAutoOpenCard == false)
    {
      while(1)
      {
        bool blCopy = false;
		int i = 0;
        for(i = 0; i<5; i++)
        {
          if(m_CardControl[i].m_CardItemArray.GetCount()!=5)
          {
            blCopy = true;
            break;
          }
          if(m_CardControl[i].m_CardTempItemArray.GetCount()==0)
          {
            blCopy = false;
            break;
          }
        }
        if(false == blCopy)
        {
          break;
        }
        else
        {
          m_CardControl[i].OnCopyCard();
        }
      }

      for(int i = 0; i<5; i++)
      {
        m_CardControl[i].m_blGameEnd = false;
        m_CardControl[i].m_blhideOneCard = false;
      }
      //更新界面
      InvalidGameView(0,0,0,0);
      KillTimer(IDI_OPENCARD);

      SetTimer(IDI_DISPATCH_CARD,10000,NULL);

      return ;

    }
    {
      if(m_CardControl[m_OpenCardIndex%5].m_MovePoint.x<40&&m_CardControl[m_OpenCardIndex%5].m_blAdd)
      {
        m_CardControl[m_OpenCardIndex%5].m_blGameEnd = false;
        m_CardControl[m_OpenCardIndex%5].m_MovePoint.x+=2;
        m_CardControl[m_OpenCardIndex%5].m_blhideOneCard = false;
        //更新界面
        InvalidGameView(0,0,0,0);


      }
      else
      {

        m_CardControl[m_OpenCardIndex%5].m_blAdd = false;
        m_CardControl[m_OpenCardIndex%5].m_blGameEnd = false;
        if(m_CardControl[m_OpenCardIndex%5].m_MovePoint.x>10)
        {
          m_CardControl[m_OpenCardIndex%5].m_MovePoint.x-=2;
        }
        m_CardControl[m_OpenCardIndex%5].m_blhideOneCard = false;
        InvalidGameView(0,0,0,0);
        if(m_CardControl[m_OpenCardIndex%5].m_MovePoint.x<=10)
        {
          m_CardControl[m_OpenCardIndex%5].m_tMoveCard.m_blMoveCard = true;
          m_CardControl[m_OpenCardIndex%5].m_tMoveCard.m_blMoveFinish = true;
          m_CardControl[m_OpenCardIndex%5].m_MovePoint.x = 0;
          m_CardControl[m_OpenCardIndex%5].m_MovePoint.y = 0;
          m_CardControl[m_OpenCardIndex%5].m_blAdd = true;
          m_CardControl[m_OpenCardIndex%5].m_blGameEnd = true;

          if(m_lUserCardType[m_OpenCardIndex%5]==CT_POINT&&m_lUserCardType[m_OpenCardIndex%5]==CT_SPECIAL_BOMEBOME)
          {
            m_CardControl[m_OpenCardIndex%5].m_blShowLineResult = true;
          }
          else
          {
            m_CardControl[m_OpenCardIndex%5].m_blShowLineResult = false;

          }
          //BYTE bcTmp[5];
          //BYTE bcCardDataTmp[5];
          //m_CardControl[m_OpenCardIndex%5].GetCardData(bcCardDataTmp,5);
          /*  int iType = m_GameLogic.GetCardType((const BYTE*)bcCardDataTmp,5,bcTmp);

            BYTE bcResult[5];
            if(iType==CT_POINT)
            {
              m_CardControl[m_OpenCardIndex%5].m_blShowLineResult = true;
              CopyMemory(bcResult,bcTmp,5);
            }else
            {
              CopyMemory(bcResult,bcTmp+3,2);
              CopyMemory(bcResult+2,bcTmp,3);

            }*/

          m_CardControl[m_OpenCardIndex%5].SetCardData(m_cbTableSortCardArray[m_OpenCardIndex%5],5,false);
          m_CardControl[m_OpenCardIndex%5].m_blShowResult = true;


          //播放声音
          CString str;
          int nCardType = 0;
          if(m_lUserCardType[m_OpenCardIndex%5]!=CT_POINT)
          {
            if(CT_SPECIAL_NIUNIU<=m_lUserCardType[m_OpenCardIndex%5])
            {
              nCardType=10;
            }
            else
            {
              nCardType =m_lUserCardType[m_OpenCardIndex%5]-CT_SPECIAL_NIU1+1;
            }
          }
          str.Format(_T("NN_%d"),nCardType);
          m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),str);


          m_OpenCardIndex++;

          InvalidGameView(0,0,0,0);

          if(m_OpenCardIndex==6)
          {
            /*for (int i = 0;i<5;i++)
            {
              m_CardControl[i].m_blGameEnd = true;
              BYTE bcTmp[5];
              BYTE bcCardDataTmp[5];
              m_CardControl[i].GetCardData(bcCardDataTmp,5);
              m_GameLogic.GetCardType((const BYTE*)bcCardDataTmp,5,bcTmp);
              m_CardControl[i].SetCardData(bcTmp,5,false);
            }*/
            KillTimer(IDI_OPENCARD);
            InvalidGameView(0,0,0,0);

            //设置显示牌定时器
            SetTimer(IDI_DISPATCH_CARD,30,NULL);
          }

        }

      }
    }
  }
  //轮换庄家
  else if(nIDEvent == IDI_SHOW_CHANGE_BANKER)
  {
    ShowChangeBanker(false);

    return;
  }
  else if(nIDEvent==IDI_DISPATCH_CARD)
  {

    //完成发牌
    FinishDispatchCard(true);

    /*  while (1)
      {
        bool blCopy = false;
        for (int i = 0;i<5;i++)
        {
          if(m_CardControl[i].m_CardItemArray.GetCount()!=5)
          {
            blCopy = true;
            break;
          }
          if(m_CardControl[i].m_CardTempItemArray.GetCount()==0)
          {
            blCopy = false;
            break;
          }
        }
        if(false == blCopy)
        {
          break;
        }else
        {
          m_CardControl[i].OnCopyCard();
        }
      }*/

    {
      for(int i = 0; i<5; i++)
      {
        m_CardControl[i].m_blGameEnd = true;
        m_CardControl[i].m_blhideOneCard = false;

        if(m_lUserCardType[i]==CT_POINT||m_lUserCardType[i]==CT_SPECIAL_BOMEBOME)
        {
          m_CardControl[i].m_blShowLineResult = true;
        }
        else
        {
          m_CardControl[i].m_blShowLineResult = false;

        }
        //  if (m_blAutoOpenCard==false)
        {
          //  BYTE bcTmp[5];
          //BYTE bcCardDataTmp[5];
          /*  m_CardControl[i].GetCardData(bcCardDataTmp,5);
            int iType = m_GameLogic.GetCardType((const BYTE*)bcCardDataTmp,5,bcTmp);
            BYTE bcResult[5];

            if(iType==CT_POINT)
            {
              m_CardControl[i].m_blShowLineResult = true;
              CopyMemory(bcResult,bcTmp,5);
            }else
            {
              CopyMemory(bcResult,bcTmp+3,2);
              CopyMemory(bcResult+2,bcTmp,3);
            }*/
          m_CardControl[i].SetCardData(m_cbTableSortCardArray[i],5,false);
          //m_CardControl[i].SetCardData(bcResult,5,false);
          m_CardControl[i].m_blShowResult = true;
        }
      }
      KillTimer(IDI_OPENCARD);
      InvalidGameView(0,0,0,0);

    }
    m_blMoveFinish = true;
    //更新界面
    InvalidGameView(0,0,0,0);

    return;
  }
  else if(IDI_SHOWDISPATCH_CARD_TIP==nIDEvent)
  {
    SetDispatchCardTip(enDispatchCardTip_NULL);

  }
  else if(nIDEvent==IDI_MOVECARD_END)
  {
    KillTimer(IDI_MOVECARD_END);
  }

  __super::OnTimer(nIDEvent);
}

//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{

  for(int i = 0; i<5; i++)
  {
    if(!m_CardControl[i].m_blAuto)
    {
      m_CardControl[i].OnLButtonDown(nFlags, Point);
    }
  }

  if(m_lCurrentJetton!=0L)
  {
    int iTimer = 1;
    //下注区域
    BYTE cbJettonArea=GetJettonArea(Point);

    //最大下注
    LONGLONG lMaxJettonScore=GetUserMaxJetton();

    if((m_lAllJettonScore[cbJettonArea]+m_lCurrentJetton)>m_lAreaLimitScore)
    {
      return ;
    }
    //合法判断
    //if (lMaxJettonScore < m_lCurrentJetton*iTimer) return ;

    //发送消息
    if(cbJettonArea!=0xFF)
    {
      SendEngineMessage(IDM_PLACE_JETTON,cbJettonArea,0);
    }
  }
  InvalidGameView(0,0,0,0);

  __super::OnLButtonDown(nFlags,Point);

  if(Point.y >0 && Point.y < m_ImageGameFrame[1].GetHeight())
  {
    LPARAM lParam = MAKEWPARAM(Point.x,Point.y);
    AfxGetMainWnd()->PostMessage(WM_LBUTTONDOWN,0,lParam);
  }

  return;
}
void CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{

  for(int i = 0; i<5; i++)
  {
    if(!m_CardControl[i].m_blAuto)
    {
      m_CardControl[i].OnLButtonUp(nFlags, Point);
      InvalidGameView(0,0,0,0);
    }

  }
  __super::OnLButtonUp(nFlags,Point);
}

//鼠标移动消息
void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
  for(int i = 0; i<5; i++)
  {
    if(!m_CardControl[i].m_blAuto)
    {
      m_CardControl[i].OnMouseMove(nFlags, point);
      InvalidGameView(0,0,0,0);

    }

  }
  __super::OnMouseMove(nFlags,point);
}
//鼠标消息
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint Point)
{
  //设置变量
  m_lCurrentJetton=0L;

  if(m_pGameClientDlg->GetGameStatus()!=GAME_SCENE_GAME_END && m_cbAreaFlash!=0xFF)
  {
    m_cbAreaFlash=0xFF;
    InvalidGameView(0,0,0,0);
  }

  __super::OnLButtonDown(nFlags,Point);
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
  if(m_lCurrentJetton!=0L)
  {
    //获取区域
    CPoint MousePoint;
    GetCursorPos(&MousePoint);
    ScreenToClient(&MousePoint);
    BYTE cbJettonArea=GetJettonArea(MousePoint);

    //设置变量
    if(m_cbAreaFlash!= cbJettonArea)
    {
      m_cbAreaFlash = cbJettonArea;
      InvalidGameView(0,0,0,0);
    }

    //区域判断
    if(cbJettonArea==0xFF)
    {
      SetCursor(LoadCursor(NULL,IDC_ARROW));
      return TRUE;
    }

    //最大下注
    LONGLONG lMaxJettonScore=GetUserMaxJetton();

    LONGLONG lLeaveScore=lMaxJettonScore;

    //设置光标
    if(m_lCurrentJetton>lLeaveScore)
    {
      if(lLeaveScore>=10000000L)
      {
        SetCurrentJetton(10000000L);
      }
      else if(lLeaveScore>=5000000L)
      {
        SetCurrentJetton(5000000L);
      }
      else if(lLeaveScore>=1000000L)
      {
        SetCurrentJetton(1000000L);
      }
      else if(lLeaveScore>=100000L)
      {
        SetCurrentJetton(100000L);
      }
      else if(lLeaveScore>=10000L)
      {
        SetCurrentJetton(10000L);
      }
      else if(lLeaveScore>=1000L)
      {
        SetCurrentJetton(1000L);
      }
      else if(lLeaveScore>=100L)
      {
        SetCurrentJetton(100L);
      }
      else
      {
        SetCurrentJetton(0L);
      }
    }

    //合法判断
    int iTimer = 1;
    if((m_lAllJettonScore[cbJettonArea]+m_lCurrentJetton)>m_lAreaLimitScore || lMaxJettonScore < m_lCurrentJetton || 0 == m_lCurrentJetton)
    {
      SetCursor(LoadCursor(NULL,IDC_NO));
      return TRUE;
    }

    //设置光标
    switch(m_lCurrentJetton)
    {
      case 100:
      {
        SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100)));
        return TRUE;
      }
      case 1000:
      {
        SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000)));
        return TRUE;
      }
      case 10000:
      {
        SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_10000)));
        return TRUE;
      }
      case 10000000:
      {
        SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_50000)));
        return TRUE;
      }
      case 100000:
      {
        SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100000)));
        return TRUE;
      }
      case 500000:
      {
        SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_500000)));
        return TRUE;
      }
      case 1000000:
      {
        SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000000)));
        return TRUE;
      }
      case 5000000:
      {
        SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_5000000)));
        return TRUE;
      }
    }
  }
  else
  {
    ClearAreaFlash();
  }

  return __super::OnSetCursor(pWnd, nHitTest, uMessage);
}

//设置标题
LRESULT CGameClientView::OnSetCaption(WPARAM wParam, LPARAM lParam)
{
  m_strCaption = (LPCTSTR)lParam;
  CRect rcCaption(5,0,1024,30);
  CDC* pDC = GetDC();

  return 0;
}

HBRUSH CGameClientView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

  if(pWnd->GetSafeHwnd() == GetDlgItem(IDC_CHAT_INPUT)->GetSafeHwnd())
  {
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(RGB(193,167,108));
    return m_brush;
  }
  return hbr;
}

//轮换庄家
void CGameClientView::ShowChangeBanker(bool bChangeBanker)
{
  //轮换庄家
  if(bChangeBanker)
  {
    SetTimer(IDI_SHOW_CHANGE_BANKER, 3000, NULL);
    m_bShowChangeBanker = true;
  }
  else
  {
    KillTimer(IDI_SHOW_CHANGE_BANKER);
    m_bShowChangeBanker = false ;
  }

  //更新界面
  InvalidGameView(0,0,0,0);
}

//上庄按钮
void CGameClientView::OnApplyBanker()
{
  SendEngineMessage(IDM_APPLY_BANKER,1,0);

}

//下庄按钮
void CGameClientView::OnCancelBanker()
{
  SendEngineMessage(IDM_APPLY_BANKER,0,0);
}

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat)
{
  //变量定义
  INT nStringLength=lstrlen(pszString);
  INT nXExcursion[8]= {1,1,1,0,-1,-1,-1,0};
  INT nYExcursion[8]= {-1,0,1,1,1,0,-1,-1};

  //绘画边框
  for(INT i=0; i<CountArray(nXExcursion); i++)
  {
    //计算位置
    CRect rcFrame;
    rcFrame.top=rcRect.top+nYExcursion[i];
    rcFrame.left=rcRect.left+nXExcursion[i];
    rcFrame.right=rcRect.right+nXExcursion[i];
    rcFrame.bottom=rcRect.bottom+nYExcursion[i];

    //绘画字符
    CDFontEx::DrawText(this,pDC,  12, 400, pszString, rcFrame, crFrame, nDrawFormat);
  }

  //绘画字符
  CDFontEx::DrawText(this,pDC,  12, 400, pszString, rcRect, crText, nDrawFormat);

  return;
}

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
  //变量定义
  int nStringLength=lstrlen(pszString);
  int nXExcursion[8]= {1,1,1,0,-1,-1,-1,0};
  int nYExcursion[8]= {-1,0,1,1,1,0,-1,-1};

  //绘画边框
  //pDC->SetTextColor(crFrame);
  //for (int i=0;i<CountArray(nXExcursion);i++)
  //{
  //  pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
  //}

  ////绘画字体
  //pDC->SetTextColor(crText);
  //pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

  return;
}
//庄家信息
void CGameClientView::SetBankerInfo(DWORD dwBankerUserID, LONGLONG lBankerScore)
{
  //庄家椅子号
  WORD wBankerUser=INVALID_CHAIR;

  //查找椅子号
  if(0!=dwBankerUserID)
  {
    for(WORD wChairID=0; wChairID<MAX_CHAIR; ++wChairID)
    {
      IClientUserItem *pUserItem = GetClientUserItem(wChairID);
      if(pUserItem == NULL)
      {
        continue;
      }
      tagUserInfo  *pUserData=pUserItem->GetUserInfo();
      if(NULL!=pUserData && dwBankerUserID==pUserData->dwUserID)
      {
        wBankerUser=wChairID;
        break;
      }
    }
  }

  //切换判断
  if(m_wBankerUser!=wBankerUser)
  {
    m_wBankerUser=wBankerUser;
    m_wBankerTime=0L;
    m_lBankerWinScore=0L;
    m_lTmpBankerWinScore=0L;
  }
  m_lBankerScore=lBankerScore;
}

//绘画标识
void CGameClientView::DrawWinFlags(CDC * pDC)
{
  //非空判断
  //m_nRecordLast = 14;
  if(m_nRecordLast==m_nRecordFirst)
  {
    return;
  }

  //资源变量

  int nIndex = m_nScoreHead;
  COLORREF static clrOld ;

  clrOld = pDC->SetTextColor(RGB(255,234,0));
  int nDrawCount=0;
  int xOffset = 0;
  // 全屏标识向右平移
  if(IsZoomedMode())
  {
    xOffset = CalFrameGap();
  }
  int nDrawColCount = IsZoomedMode() ? ZOOMED_MAX_FALG_COUNT : NORMAL_MAX_FALG_COUNT;
  while(nIndex != m_nRecordLast && (m_nRecordLast!=m_nRecordFirst) && nDrawCount < nDrawColCount)
  {
    //胜利标识
    tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
    bool  static bWinMen[4];
    bWinMen[0]=ClientGameRecord.bWinTianMen;
    bWinMen[1]=ClientGameRecord.bWinDiMen ;
    bWinMen[2]=ClientGameRecord.bWinXuanMen;
    bWinMen[3]=ClientGameRecord.bWinHuangMen;

    //操作结果
    enOperateResult static OperateResult[4];
    OperateResult[0]=ClientGameRecord.enOperateTianMen;
    OperateResult[1]=ClientGameRecord.enOperateDiMen;
    OperateResult[2]=ClientGameRecord.enOperateXuanMen;
    OperateResult[3]=ClientGameRecord.enOperateHuangMen;

    for(int i=0; i<4; ++i)
    {
      //位置变量
      int static nYPos=0,nXPos=0;
      nYPos=m_nWinFlagsExcursionY+i*24;
      nXPos=m_nWinFlagsExcursionX + ((nIndex - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY) * 32;

      //胜利标识
      int static nFlagsIndex=0;
      if(false==bWinMen[i])
      {
        nFlagsIndex=2;
      }
      else
      {
        nFlagsIndex=0;
      }
      if(OperateResult[i] != enOperateResult_NULL)
      {
        nFlagsIndex+=1;

        if(nFlagsIndex>=4)
        {
          nFlagsIndex=3;
        }
      }
      //绘画标识
      m_ImageWinFlags.DrawImage(pDC, nXPos+30 + xOffset, nYPos - 7, m_ImageWinFlags.GetWidth()/4 ,
                                m_ImageWinFlags.GetHeight(),m_ImageWinFlags.GetWidth()/4 * nFlagsIndex, 0);
    }

    //移动下标
    nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
    nDrawCount++;
  }
  pDC->SetTextColor(clrOld);
}
//手工搓牌
afx_msg void CGameClientView::OnOpenCard()
{
  //SendEngineMessage(IDM_OPEN_CARD,0,0);

}
//自动搓牌
afx_msg void CGameClientView::OnAutoOpenCard()
{
  SendEngineMessage(IDM_AUTO_OPEN_CARD,0,0);
}

//银行按钮
void CGameClientView::OnButtonGoBanker()
{
  //获取接口
  CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
  IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

  if(NULL==pIClientKernel)
  {
    return;
  }


  //构造数据
  CBankTipsDlg GameTips;

  POINT locPoint;
  locPoint.x=10;
  locPoint.y=340;
  ClientToScreen(&locPoint);

  GameTips.SetStartLoc(CPoint(locPoint.x,locPoint.y));
  //配置数据
  if(GameTips.DoModal()==IDOK)
  {
    if(GameTips.m_bRestore)
    {
      POINT locPoint;
      locPoint.x=266;
      locPoint.y=410;
      ClientToScreen(&locPoint);
      ShowInsureSave(pIClientKernel,CPoint(locPoint.x,locPoint.y));

    }
    else
    {
      POINT locPoint;
      locPoint.x=266;
      locPoint.y=410;
      ClientToScreen(&locPoint);
      ShowInsureGet(pIClientKernel,CPoint(locPoint.x,locPoint.y));
    }

  }
  else
  {
    return;
  }
}

//移动按钮
void CGameClientView::OnScoreMoveL()
{
  if(m_nRecordFirst == m_nScoreHead)
  {
    return;
  }

  m_nScoreHead = (m_nScoreHead - 1 + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
  if(m_nScoreHead == m_nRecordFirst)
  {
    m_btScoreMoveL.EnableWindow(FALSE);
  }

  m_btScoreMoveR.EnableWindow(TRUE);

  //更新界面
  InvalidGameView(0,0,0,0);
}

//移动按钮
void CGameClientView::OnScoreMoveR()
{
  if(!IsZoomedMode())
  {
    int nHistoryCount = (m_nRecordLast - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
    if(nHistoryCount == NORMAL_MAX_FALG_COUNT)
    {
      return;
    }
    m_nScoreHead = (m_nScoreHead + 1) % MAX_SCORE_HISTORY;
    if(nHistoryCount-1 == NORMAL_MAX_FALG_COUNT)
    {
      m_btScoreMoveR.EnableWindow(FALSE);
    }
  }
  else
  {
    int nHistoryCount = (m_nRecordLast - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
    if(nHistoryCount == ZOOMED_MAX_FALG_COUNT)
    {
      return;
    }
    m_nScoreHead = (m_nScoreHead + 1) % MAX_SCORE_HISTORY;
    if(nHistoryCount-1 == ZOOMED_MAX_FALG_COUNT)
    {
      m_btScoreMoveR.EnableWindow(FALSE);
    }
  }


  m_btScoreMoveL.EnableWindow(TRUE);

  //更新界面
  InvalidGameView(0,0,0,0);
}

//聊天消息
void CGameClientView::InsertAllChatMessage(LPCTSTR pszString,int nMessageType)
{

  if(nMessageType==1)
  {
    InsertSystemMessage(pszString);
  }
  else if(nMessageType==2)
  {
    InsertChatMessage(pszString);
  }
  else
  {
    InsertNormalMessage(pszString);
  }

  m_ChatDisplay.PostMessage(WM_VSCROLL,   SB_BOTTOM,0);

}

//系统消息
void CGameClientView::InsertSystemMessage(LPCTSTR pszString)
{
  CString strMessage;
  strMessage.Format(TEXT("[系统消息]:"));
  InsertMessage(strMessage,RGB(255,0,0));
  InsertMessage(pszString);
  InsertMessage(TEXT("\r\n"));
}

//聊天消息
void CGameClientView::InsertChatMessage(LPCTSTR pszString)
{

  CString  strInit=pszString;
  CString  FileLeft,FileRight;
  int index;
  index   = strInit.Find(':');
  if(index<0)
  {
    return;
  }

  FileLeft  = strInit.Left(index+1);
  FileRight   = strInit.Right(strInit.GetLength()-index-1);

  InsertMessage(FileLeft,RGB(0,255,0));
  InsertMessage(FileRight);
  InsertMessage(TEXT("\r\n"));

}

//常规消息
void CGameClientView::InsertNormalMessage(LPCTSTR pszString)
{
  InsertMessage(pszString);
  InsertMessage(TEXT("\r\n"));
}

//消息函数
void CGameClientView::InsertMessage(LPCTSTR pszString,COLORREF color)
{

  CString str, mes;
  CEdit *pEditCtrl=(CEdit *)GetDlgItem(IDC_CHAT_DISPLAY);
  pEditCtrl->GetWindowText(mes);

  CHARFORMAT   cf;
  cf.cbSize   =   sizeof(cf);
  m_ChatDisplay.GetSelectionCharFormat(cf);

  cf.dwMask=CFM_SIZE|CFM_COLOR;
  cf.dwEffects=CFE_PROTECTED;//cf.dwEffects^CFE_AUTOCOLOR;

  //****
  cf.crTextColor  = color;

  m_ChatDisplay.SetSelectionCharFormat(cf);

  int nlength=mes.GetLength();
  pEditCtrl->SetSel(nlength,nlength);

  pEditCtrl->ReplaceSel(pszString);

  //pEditCtrl->LineScroll(pEditCtrl->GetLineCount());

}

//结算消息
void CGameClientView::InsertGameEndInfo()
{
  CString strInfo;
  strInfo.Format(TEXT("\r\n"));
  InsertAllChatMessage(strInfo,0);

  strInfo.Format(TEXT("本局结束,成绩统计:"));
  InsertAllChatMessage(strInfo,0);

  strInfo.Format(TEXT("庄家:%I64d"),m_lBankerCurGameScore);

  InsertAllChatMessage(strInfo,0);

  strInfo.Format(TEXT("本家:%I64d"),m_lMeCurGameScore);

  InsertAllChatMessage(strInfo,0);
}

//显示结果
void CGameClientView::ShowGameResult(CDC *pDC, int nWidth, int nHeight)
{
  //显示判断
  //if (m_pGameClientDlg->GetGameStatus()!=GS_GAME_END) return;
  if(false==m_bShowGameResult)
  {
    return;
  }

  int nXPos = nWidth / 2 - 174;
  int nYPos = nHeight / 2 - 230;

  //CRect rcAlpha(nXPos+2, nYPos+70, nXPos+2 + m_ImageGameEnd.GetWidth(), nYPos+70+m_ImageGameEnd.GetHeight());
  //DrawAlphaRect(pDC, &rcAlpha, RGB(74,70,73), 0.8f);
  m_ImageGameEnd.DrawImage(pDC, nXPos+2, nYPos+70);

  pDC->SetTextColor(RGB(255,234,0));
  CRect rcMeWinScore, rcMeReturnScore;
  rcMeWinScore.left = nXPos+2 + 70;
  rcMeWinScore.top = nYPos+100 + 30;
  rcMeWinScore.right = rcMeWinScore.left + 111;
  rcMeWinScore.bottom = rcMeWinScore.top + 34;

  rcMeReturnScore.left = nXPos+2 + 200;
  rcMeReturnScore.top = nYPos+100 + 30;
  rcMeReturnScore.right = rcMeReturnScore.left + 111;
  rcMeReturnScore.bottom = rcMeReturnScore.top + 34;

  CString strMeGameScore, strMeReturnScore;
  DrawNumberStringWithSpace(pDC,m_lMeCurGameScore,rcMeWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE,true);
  DrawNumberStringWithSpace(pDC,m_lMeCurGameReturnScore,rcMeReturnScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE,true);

  CRect rcBankerWinScore;
  rcBankerWinScore.left = nXPos+2 + 70;
  rcBankerWinScore.top = nYPos+100 + 71;
  rcBankerWinScore.right = rcBankerWinScore.left + 111;
  rcBankerWinScore.bottom = rcBankerWinScore.top + 34;

  CString strBankerCurGameScore;
  strBankerCurGameScore.Format(TEXT("%I64d"), m_lBankerCurGameScore);
  DrawNumberStringWithSpace(pDC,m_lBankerCurGameScore,rcBankerWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE,true);
}

//透明绘画
bool CGameClientView::DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha)
{
  ASSERT(pDC != 0 && lpRect != 0 && clr != CLR_NONE);
  if(pDC == 0 || lpRect == 0 || clr == CLR_NONE)
  {
    return false;
  }
  //全透明
  if(abs(fAlpha) <= 0.000001)
  {
    return true;
  }

  for(int l=lpRect->top; l<lpRect->bottom; l++)
  {
    for(int k=lpRect->left; k<lpRect->right; k++)
    {
      COLORREF clrBk = pDC->GetPixel(k, l);
      COLORREF clrBlend = RGB(GetRValue(clrBk)*(1-fAlpha)+GetRValue(clr)*fAlpha,
                              GetGValue(clrBk)*(1-fAlpha)+GetGValue(clr)*fAlpha,
                              GetBValue(clrBk)*(1-fAlpha)+GetBValue(clr)*fAlpha);
      pDC->SetPixel(k, l, clrBlend);
    }
  }

  return true;
}

//最大下注
LONGLONG CGameClientView::GetUserMaxJetton()
{
  int iTimer = 10;
  //已下注额
  LONGLONG lNowJetton = 0;
  ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lNowJetton += m_lUserJettonScore[nAreaIndex]*iTimer;
  }

  //庄家金币
  LONGLONG lBankerScore=2147483647;
  if(m_wBankerUser!=INVALID_CHAIR)
  {
    lBankerScore=m_lBankerScore*m_nEndGameMul/100;
  }
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lBankerScore-=m_lAllJettonScore[nAreaIndex]*iTimer;
  }

  //区域限制
  LONGLONG lMeMaxScore;
  if((m_lMeMaxScore-lNowJetton)/iTimer>m_lAreaLimitScore)
  {
    lMeMaxScore= m_lAreaLimitScore*iTimer;

  }
  else
  {
    lMeMaxScore = m_lMeMaxScore-lNowJetton;
    lMeMaxScore = lMeMaxScore;
  }

  //庄家限制
  lMeMaxScore=min(lMeMaxScore,lBankerScore);

  lMeMaxScore /= iTimer;

  //非零限制
  //ASSERT(lMeMaxScore >= 0);
  lMeMaxScore = max(lMeMaxScore, 0);

  return lMeMaxScore;
}

//成绩设置
void CGameClientView::SetGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore)
{
  m_lMeCurGameScore=lMeCurGameScore;
  m_lMeCurGameReturnScore=lMeCurGameReturnScore;
  m_lBankerCurGameScore=lBankerCurGameScore;
}

//绘画数字
void CGameClientView::DrawMeJettonNumber(CDC *pDC)
{
  //绘画数字
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    if(m_lUserJettonScore[nAreaIndex] > 0)
    {
      DrawNumberString(pDC,m_lUserJettonScore[nAreaIndex],m_PointJettonNumber[nAreaIndex-1].x,m_PointJettonNumber[nAreaIndex-1].y+25, true);
    }
  }
}

//开始发牌
void CGameClientView::DispatchCard()
{
  //设置界面
  for(int i=0; i<CountArray(m_CardControl); ++i)
  {
    m_CardControl[i].m_CardItemArray.SetSize(0);
  }
  //设置标识
  m_bNeedSetGameRecord=true;
}

//结束发牌
void CGameClientView::FinishDispatchCard(bool bNotScene)
{
  //完成判断
  if(m_bNeedSetGameRecord==false)
  {
    return;
  }

  //设置标识
  m_bNeedSetGameRecord=false;

  //删除定时器
  KillTimer(IDI_DISPATCH_CARD);

  ////设置扑克
  //for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].SetCardData(m_cbTableCardArray[i],2);

  //推断赢家
  bool bWinTianMen, bWinDiMen, bWinXuanMen,bWinHuang;

  BYTE TianMultiple,diMultiple,TianXuanltiple,HuangMultiple;

  DeduceWinner(bWinTianMen, bWinDiMen, bWinXuanMen,bWinHuang,TianMultiple,diMultiple,TianXuanltiple,HuangMultiple);

  //保存记录
  if(bNotScene)
  {
    SetGameHistory(bWinTianMen, bWinDiMen, bWinXuanMen,bWinHuang);
  }

  //累计积分
  m_lMeStatisticScore+=m_lMeCurGameScore;
  m_lBankerWinScore=m_lTmpBankerWinScore;

  //设置赢家
  SetWinnerSide(bWinTianMen, bWinDiMen, bWinXuanMen, bWinHuang, true);
  InsertGameEndInfo();

  //播放声音
  if(m_lMeCurGameScore>0)
  {
    m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_WIN"));
  }
  else if(m_lMeCurGameScore<0)
  {
    m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_LOST"));
  }
  else
  {
    m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_DRAW"));
  }
}

//终止动画
void CGameClientView::StopAnimal()
{
  //定时器
  KillTimer(IDI_POSTCARD);
  KillTimer(IDI_DISPATCH_CARD);
  KillTimer(IDI_FLASH_WINNER);
  KillTimer(IDI_OPENCARD);

  FinishDispatchCard(true);

  for(int i = 0; i<4; i++)
  {
    m_CardControl[i].m_blGameEnd = true;
    m_CardControl[i].m_blhideOneCard = false;
  }
  m_blMoveFinish = true;
  m_bFlashResult = false;
}

//胜利边框
void CGameClientView::FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC)
{
  //合法判断
  if(m_cbAreaFlash==0xFF && false==m_bFlashResult)
  {
    return;
  }

  //下注判断
  if(false==m_bFlashResult)
  {
    if(m_pGameClientDlg->GetGameStatus()==GAME_SCENE_PLACE_JETTON)
    {
      switch(m_cbAreaFlash)
      {
        case ID_TIAN_MEN:
        {
          //绘画图片

          m_ImageFrameTianMen.DrawImage(pDC,m_rcTianMen.left, m_rcTianMen.top);
          break;
        }
        case ID_DI_MEN:
        {
          //绘画图片

          m_ImageFrameDiMen.DrawImage(pDC,m_rcDimen.left, m_rcDimen.top);
          break;
        }
        case ID_XUAN_MEN:
        {
          //绘画图片

          m_ImageFrameXuanMen.DrawImage(pDC,m_rcXuanMen.left, m_rcXuanMen.top);
          break;
        }
        case ID_HUANG_MEN:
        {
          //绘画图片

          m_ImageFrameHuangMen.DrawImage(pDC,m_rcHuangMen.left, m_rcHuangMen.top);
          break;
        }
      }
    }

  }
  else
  {
    //庄全胜判断
    if(false==m_bWinXuanMen && false==m_bWinTianMen && false==m_bWinHuangMen&&m_bWinDiMen == false)
    {
      //加载资源


      //绘画图片
      /*m_ImageFrameTianMen.DrawImage(pDC,m_rcTianMen.left, m_rcTianMen.top);
      m_ImageFrameDiMen.DrawImage(pDC,m_rcDimen.left, m_rcDimen.top);
      m_ImageFrameHuangMen.DrawImage(pDC,m_rcHuangMen.left, m_rcHuangMen.top);
      m_ImageFrameXuanMen.DrawImage(pDC,m_rcXuanMen.left, m_rcXuanMen.top);*/
    }
    else
    {
      //门判断
      if(true==m_bWinXuanMen)
      {

        m_ImageFrameXuanMen.DrawImage(pDC,m_rcXuanMen.left, m_rcXuanMen.top);
      }
      if(true==m_bWinTianMen)
      {

        m_ImageFrameTianMen.DrawImage(pDC,m_rcTianMen.left, m_rcTianMen.top);
      }
      if(true==m_bWinHuangMen)
      {

        m_ImageFrameHuangMen.DrawImage(pDC,m_rcHuangMen.left, m_rcHuangMen.top);
      }
      if(true==m_bWinDiMen)
      {

        m_ImageFrameDiMen.DrawImage(pDC,m_rcDimen.left, m_rcDimen.top);
      }
    }
  }
}


//推断赢家
void CGameClientView::DeduceWinner(bool &bWinTian, bool &bWinDi, bool &bWinXuan,bool &bWinHuan,BYTE &TianMultiple,BYTE &diMultiple,BYTE &TianXuanltiple,BYTE &HuangMultiple)
{
  //大小比较
  bWinTian=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[SHUN_MEN_INDEX],5,TianMultiple)==1?true:false;
  bWinDi=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[DUI_MEN_INDEX],5,diMultiple)==1?true:false;
  bWinXuan=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[DAO_MEN_INDEX],5,TianXuanltiple)==1?true:false;
  bWinHuan=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[HUAN_MEN_INDEX],5,HuangMultiple)==1?true:false;
}

////控件命令
//BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
//{
//  //获取ID
//  WORD wControlID=LOWORD(wParam);
//
//  //控件判断
//  switch (wControlID)
//  {
//  case IDC_JETTON_BUTTON_100:
//    {
//      //设置变量
//      m_lCurrentJetton=100L;
//      break;
//    }
//  case IDC_JETTON_BUTTON_1000:
//    {
//      //设置变量
//      m_lCurrentJetton=1000L;
//      break;
//    }
//  case IDC_JETTON_BUTTON_10000:
//    {
//      //设置变量
//      m_lCurrentJetton=10000L;
//      break;
//    }
//  case IDC_JETTON_BUTTON_50000:
//    {
//      //设置变量
//      m_lCurrentJetton=50000L;
//      break;
//    }
//  case IDC_JETTON_BUTTON_100000:
//    {
//      //设置变量
//      m_lCurrentJetton=100000L;
//      break;
//    }
//  case IDC_JETTON_BUTTON_500000:
//    {
//      //设置变量
//      m_lCurrentJetton=500000L;
//      break;
//    }
//  case IDC_JETTON_BUTTON_1000000:
//    {
//      //设置变量
//      m_lCurrentJetton=1000000L;
//      break;
//    }
//  case IDC_JETTON_BUTTON_5000000:
//    {
//      //设置变量
//      m_lCurrentJetton=5000000L;
//      break;
//    }
//  case IDC_AUTO_OPEN_CARD:
//    {
//      break;
//    }
//  case IDC_OPEN_CARD:
//    {
//      break;
//    }
//  case IDC_BANK:
//    {
//      break;
//    }
//  case IDC_CONTINUE_CARD:
//    {
//      SendEngineMessage(IDM_CONTINUE_CARD,0,0);
//      break;
//    }
//  }
//
//  return CGameFrameView::OnCommand(wParam, lParam);
//}

//我的位置
void CGameClientView::SetMeChairID(WORD dwMeUserID)
{
  m_wMeChairID=dwMeUserID;
}

//设置提示
void CGameClientView::SetDispatchCardTip(enDispatchCardTip DispatchCardTip)
{
  //设置变量
  m_enDispatchCardTip=DispatchCardTip;

  //设置定时器
  if(enDispatchCardTip_NULL!=DispatchCardTip)
  {
    SetTimer(IDI_SHOWDISPATCH_CARD_TIP,2*1000,NULL);
  }
  else
  {
    KillTimer(IDI_SHOWDISPATCH_CARD_TIP);
  }

  //更新界面
  InvalidGameView(0,0,0,0);
}

// 绘画上庄列表
void CGameClientView::DrawBankerList(CDC *pDC, int nWidth, int nHeight)
{
  // 定义变量
  CPoint ptBegin(0,0);
  // 申请上庄列表
  ptBegin.SetPoint(15, 210);

  int   nCount = 0;
  CRect rect(0, 0, 0, 0);
  rect.SetRect(ptBegin.x , ptBegin.y, ptBegin.x + 165, ptBegin.y + 20);

  for(int i = m_nShowValleyIndex; i < m_ValleysList.GetCount(); ++i)
  {
    IClientUserItem* pIClientUserItem = GetClientUserItem(m_ValleysList[i]);
    if(pIClientUserItem == NULL)
    {
      continue;
    }
    // 名字
    CRect rectName(rect);
    rectName.left = ptBegin.x;
    rectName.right = ptBegin.x + 75;
    DrawTextString(pDC, pIClientUserItem->GetNickName(), RGB(253,241,149) , RGB(38,26,2), rectName, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

    // 积分
    CRect rectScore(rect);
    rectScore.left = ptBegin.x + 85;
    DrawNumberStringWithSpace(pDC,pIClientUserItem->GetUserScore(), rectScore, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

    // 位置调移
    rect.OffsetRect(0, 18);

    // 位数增加
    nCount++;

    if(nCount == 5)
    {
      break;
    }
  }

}

//绘画庄家
void CGameClientView::DrawBankerInfo(CDC *pDC,int nWidth,int nHeight)
{
  //庄家信息
  IClientUserItem *pUserItem =NULL;
  if(m_wBankerUser != INVALID_CHAIR)
  {
    pUserItem = GetClientUserItem(m_wBankerUser);
  }
  //获取玩家
  tagUserInfo const *pUserData = pUserItem==NULL ? NULL : pUserItem->GetUserInfo();

  //位置信息
  CRect static StrRect;
  StrRect.left = 74;
  StrRect.top = 71;
  StrRect.right = StrRect.left + 94;
  StrRect.bottom = StrRect.top + 15;

  //庄家名字
  m_DFontExT.DrawText(pDC,pUserData==NULL?(m_bEnableSysBanker?TEXT("系统坐庄"):TEXT("无人坐庄")):pUserData->szNickName, StrRect, RGB(0,0,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE);

  pDC->SetTextColor(RGB(255,234,0));

  //庄家成绩
  StrRect.left = 74;
  StrRect.top = 96;
  StrRect.right = StrRect.left + 94;
  StrRect.bottom = StrRect.top + 15;
  DrawNumberStringWithSpace(pDC,m_lBankerWinScore,StrRect);

  //庄家局数
  StrRect.left = 74;
  StrRect.top = 121;
  StrRect.right = StrRect.left + 104;
  StrRect.bottom = StrRect.top + 15;
  DrawNumberStringWithSpace(pDC,m_wBankerTime,StrRect);

  //庄家总分
  StrRect.left = 74;
  StrRect.top = 146;
  StrRect.right = StrRect.left + 130;
  StrRect.bottom = StrRect.top + 15;
  DrawNumberStringWithSpace(pDC,pUserData==NULL?0:pUserData->lScore, StrRect);
}

void CGameClientView::SetFirstShowCard(BYTE bcCard)
{
  m_bcfirstShowCard =  bcCard;
  m_CardControl[5].SetCardData(NULL,0);
  m_CardControl[5].SetCardData(&bcCard,1);
  m_CardControl[5].m_blGameEnd = true;
  m_CardControl[5].SetDisplayFlag(true);
  m_CardControl[5].ShowCardControl(true);
}

//绘画玩家
void CGameClientView::DrawMeInfo(CDC *pDC,int nWidth,int nHeight)
{
  //合法判断
  if(INVALID_CHAIR==m_wMeChairID)
  {
    return;
  }

  const tagUserInfo *pMeUserData = GetClientUserItem(m_wMeChairID)->GetUserInfo();

  if(pMeUserData != NULL)
  {
    //总共下注
    LONGLONG lMeJetton=0L;
    for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
    {
      lMeJetton += m_lUserJettonScore[nAreaIndex];
    }

    //设置位置
    CRect static rcDraw;
    rcDraw.left=nWidth-155;
    rcDraw.top=70;
    rcDraw.right=rcDraw.left+90;
    rcDraw.bottom=rcDraw.top+15;

    //绘画帐号
    //pDC->DrawText(pMeUserData->szNickName,lstrlen(pMeUserData->szNickName),rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
    m_DFontExT.DrawText(pDC,pMeUserData->szNickName, rcDraw, RGB(0,0,0), DT_VCENTER|DT_SINGLELINE|DT_RIGHT|DT_END_ELLIPSIS);

    //字体颜色
    pDC->SetTextColor(RGB(255,234,0));

    //绘画成绩
    rcDraw.top=95;
    rcDraw.bottom=rcDraw.top+15;
    DrawNumberStringWithSpace(pDC,m_lMeStatisticScore,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_RIGHT|DT_END_ELLIPSIS);

    //绘画积分
    rcDraw.left=nWidth-193;
    rcDraw.top=120;
    rcDraw.right=rcDraw.left+128;
    rcDraw.bottom=rcDraw.top+15;
    DrawNumberStringWithSpace(pDC,pMeUserData->lScore-lMeJetton,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_RIGHT|DT_END_ELLIPSIS);
  }
}

//银行取款
void CGameClientView::OnBankDraw()
{
  //获取接口
  CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
  IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

  if(NULL!=pIClientKernel)
  {
    CRect btRect;
    m_btBank.GetWindowRect(&btRect);
    ShowInsureGet(pIClientKernel,CPoint(btRect.right,btRect.top));
  }
}

void CGameClientView::OnUp()
{
  if(m_nShowValleyIndex > 0)
  {
    m_nShowValleyIndex--;
  }

  //更新界面
  UpdateButtonContron();
  InvalidGameView(0,0,0,0);
}

void CGameClientView::OnDown()
{
  if(m_nShowValleyIndex < m_ValleysList.GetCount() - 1)
  {
    m_nShowValleyIndex++;
  }

  //更新界面
  UpdateButtonContron();
  InvalidGameView(0,0,0,0);
};

void CGameClientView::OpenAdminWnd()
{
  //如果有此权限
  if(m_pClientControlDlg != NULL)
  {
    if(!m_pClientControlDlg->IsWindowVisible())
    {
      m_pClientControlDlg->ShowWindow(SW_SHOW);
      m_pClientControlDlg->UpdateControl();
    }
    else
    {
      m_pClientControlDlg->ShowWindow(SW_HIDE);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  int cx = GetSystemMetrics(SM_CXSCREEN);
  CRect rc(0,0,cx,30);
  if(rc.PtInRect(point) && m_btMax.IsWindowVisible())
  {
    AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_MAX,0);
    m_btMax.ShowWindow(SW_HIDE);
    m_btRestore.ShowWindow(SW_SHOW);
    return;
  }
  else if(rc.PtInRect(point) && m_btRestore.IsWindowVisible())
  {
    AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_RESTORE,0);
    m_btMax.ShowWindow(SW_SHOW);
    m_btRestore.ShowWindow(SW_HIDE);
  }
}

bool CGameClientView::IsZoomedMode()
{
  return !m_btMax.IsWindowVisible();
}

INT CGameClientView::CalFrameGap()
{
  ASSERT(IsZoomedMode() == true);
  if(IsZoomedMode() == false)
  {
    return -1;
  }
  INT nGap = GetSystemMetrics(SM_CXSCREEN) - (ZOOMED_MAX_FALG_COUNT - NORMAL_MAX_FALG_COUNT)*m_ImageHistoryMid.GetWidth() -
             m_ImageUserBack.GetWidth()-m_ImageChatBack.GetWidth() - 4 - m_ImageHistoryFront.GetWidth() - m_ImageHistoryBack.GetWidth();
  return nGap/2;
}
