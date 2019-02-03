#include "StdAfx.h"
#include "Resource.h"
#include "GameClient.h"
#include "GameClientView.h"
#include "GameClientEngine.h"


//////////////////////////////////////////////////////////////////////////
//宏定义

//按钮标识
#define IDC_HINT_OX           130               //提示按钮
#define IDC_START           100               //开始按钮
#define IDC_MAX_SCORE         104               //最大按钮
#define IDC_MIN_SCORE         105               //最少按钮
#define IDC_ADD_SCORE         110               //加注按钮

#define IDC_READY           111               //摊牌按钮
#define IDC_RESORT            112               //重排按钮
#define IDC_OX              113               //牛牛按钮
#define IDC_ONE_SCORE         117               //加注按钮
#define IDC_TWO_SCORE         114               //加注按钮
#define IDC_THREE_SCORE         115               //加注按钮
#define IDC_FOUR_SCORE          116               //加注按钮
#define IDC_SHORTCUT          118               //快捷键按钮
#define IDC_IDLER           119               //闲家按钮
#define IDC_BANKER            120               //庄家按钮
#define IDC_SCORE           122               //查分按钮
#define IDC_YU_YIN            123               //语音按钮
#define IDC_ADMIN           124                 //按钮标识

//定时器标识
#define IDI_SEND_CARD         98                //发牌定时器
#define IDI_GOLD            97                //发牌定时器
#define IDI_USER_ACTION         101               //动作定时器
#define IDI_JETTON_ACTION       102               //动作定时器

#define IDI_BONUS           104               //彩金定时器

#define BONUS_FPS           26
#define BONUS_PERPOSX         320
#define BONUS_PERPOSY         240

//定时器时间
#define TIME_USER_ACTION        4000              //动作定时器

//移动速度
#define SEND_PELS           80                //发牌速度
#define TIME_SENDSPEED          50                //间隔速度


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
  ON_WM_CREATE()
  ON_BN_CLICKED(IDC_SCORE, OnBnClickedScore)
  ON_BN_CLICKED(IDC_ADMIN,OpenAdminWnd)
  ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
  //动画变量
  m_bJettonAction=false;
  m_cbDynamicJoin=FALSE;

  m_wBonusCartoonIndex = 0;
  m_bShowBonusCartoon = false;
  ZeroMemory(m_bBonus, sizeof(m_bBonus));
  m_lBonus = 0;

  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    m_bOxValue[i]=0xff;
    m_SendEndingPos[i].SetPoint(0,0);
  }
  m_wSendCount=0;
  m_wSendIndex=0;
  m_SendCardPos.SetPoint(0,0);
  m_SendCardItemArray.RemoveAll();
  ZeroMemory(m_cbShootData,sizeof(m_cbShootData));
  ZeroMemory(m_JettonStatus,sizeof(m_JettonStatus));
  ZeroMemory(m_lMoveGold,sizeof(m_lMoveGold));

  //数据变量
  ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
  ZeroMemory(m_bOpenCardUser,sizeof(m_bOpenCardUser));
  ZeroMemory(m_bShowSocre,sizeof(m_bShowSocre));
  m_wBankerUser=INVALID_CHAIR;
  m_bCallUser=0xff;
  m_bShow=false;
  m_bInvest=false;
  m_bOpenStatus=false;
  m_bUserAction=false;
  m_bLookOnUser=false;
  m_bShowScore=false;
  m_bMeOpenCard=false;
  ZeroMemory(m_pHistoryScore,sizeof(m_pHistoryScore));

  //加载位图
  HINSTANCE hInstance=AfxGetInstanceHandle();
  m_ImageScore.LoadImage(hInstance, _T("SCORE_BACK"));
  m_ImageJeton.LoadFromResource(hInstance, IDB__MONEY);
  m_ImageCard.LoadImage(hInstance, _T("GAME_CARD"));
  m_ImageViewFill.LoadImage(hInstance,TEXT("VIEW_FILL"));
  m_ImageViewCenter.LoadImage(hInstance,TEXT("VIEW_CENTER"));

  //加载资源
  m_PngReady.LoadImage(hInstance,TEXT("READY"));
  m_PngBonus.LoadImage(hInstance,TEXT("BONUS"));
  m_PngBonusNum.LoadImage(this, hInstance, TEXT("BONUSNUM"));
  m_PngUserBack.LoadImage(hInstance,TEXT("USER_HEAD_BACK"));
  m_ImageOxValueZero.LoadImage(hInstance,TEXT("COW_VALUE_ZERO"));
  m_ImageDoulbeOx.LoadImage(hInstance,TEXT("DOUBLE_COW"));
  m_ImageNumber.LoadImage(hInstance,TEXT("NUMBER"));
  m_ImageOxValue.LoadImage(hInstance,TEXT("COW_VALUE"));
  m_ImageWaitCall.LoadImage(hInstance,TEXT("CALLBANKER"));
  m_ImageTimeBack.LoadImage(hInstance,TEXT("TIME_BACK"));
  m_ImageTimeNumber.LoadImage(hInstance,TEXT("TIME_NUMBER"));
  m_ImageBanker.LoadImage(hInstance,TEXT("BANKER_FLAG"));
  m_PngAutoOpen.LoadImage(hInstance,TEXT("AUTO_OPEN"));
  m_PngWaitBet.LoadImage(hInstance,TEXT("WAIT_BET"));
  m_PngWaitOpen.LoadImage(hInstance,TEXT("WAIT_OPEN"));
  m_PngOpenTag.LoadImage(hInstance,TEXT("OPEN_TAG"));
  m_PngHistoryScore.LoadImage(hInstance,TEXT("HISTORY_SCORE"));
  m_PngFrameLR.LoadImage(hInstance,TEXT("FRAME_LR"));
  m_PngFrameTB.LoadImage(hInstance,TEXT("FRAME_TB"));
  m_PngVirtualLine.LoadImage(hInstance,TEXT("VIRTUAL_LINE"));
  m_PngOxAddBack.LoadImage(hInstance,TEXT("OX_ADD_BACK"));
  m_PngOxAddNum.LoadImage(hInstance,TEXT("OX_ADD_NUM"));
  m_PngOxAddType.LoadImage(hInstance,TEXT("OX_ADD_TYPE"));

  //获取大小
  m_sizeHistory.SetSize(m_PngHistoryScore.GetWidth(),m_PngHistoryScore.GetHeight());

  //字体
  m_FontEx.CreateFont(this,_T("宋体"),14,900);
  m_FontCardValue.CreateFont(this,_T("黑体"),26,700);

  m_pClientControlDlg = NULL;
  m_hInst = NULL;
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

  /*if( m_pSpeClientControlDlg )
  {
    delete m_pSpeClientControlDlg;
    m_pSpeClientControlDlg = NULL;
  }

  if( m_hSpeInst )
  {
    FreeLibrary(m_hSpeInst);
    m_hSpeInst = NULL;
  }*/
}

//消息解释
BOOL CGameClientView::PreTranslateMessage(MSG * pMsg)
{
  m_ToolTipCtrl.RelayEvent(pMsg);
  return __super::PreTranslateMessage(pMsg);
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(__super::OnCreate(lpCreateStruct)==-1)
  {
    return -1;
  }

  //创建控件
  CRect rcCreate(0,0,0,0);
  //m_GoldControl.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS,CRect(50,50,66,66),this,8);
  //m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,10);

  //创建按钮
  m_btYuYin.Create(NULL,WS_CHILD,rcCreate,this,IDC_YU_YIN);
  m_btScore.Create(NULL,WS_CHILD,rcCreate,this,IDC_SCORE);
  m_btHintOx.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_HINT_OX);
  m_btStart.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_START);
//  m_btReSort.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_RESORT);
  m_btOpenCard.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_READY);
  m_btOx.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_OX);
//  m_btShortcut.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_SHORTCUT);
  m_btOneScore.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_ONE_SCORE);
  m_btTwoScore.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_TWO_SCORE);
  m_btThreeScore.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_THREE_SCORE);
  m_btFourScore.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_FOUR_SCORE);
  m_btBanker.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_BANKER);
  m_btIdler.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_IDLER);

  //设置颜色
  m_btOneScore.SetButtonColor(RGB(42,67,68));
  m_btTwoScore.SetButtonColor(RGB(42,67,68));
  m_btThreeScore.SetButtonColor(RGB(42,67,68));
  m_btFourScore.SetButtonColor(RGB(42,67,68));

  //创建字体
  CFont btFont;
  btFont.CreateFont(0,14,0,0,900,0,0,0,134,3,2,1,1,TEXT("宋体"));
  m_btOneScore.SetFont(&btFont);
  m_btTwoScore.SetFont(&btFont);
  m_btThreeScore.SetFont(&btFont);
  m_btFourScore.SetFont(&btFont);

  //加载位图
  HINSTANCE hInstance=AfxGetInstanceHandle();
  m_btYuYin.SetButtonImage(IDB_BT_YU_YIN,hInstance,false,false);
  m_btScore.SetButtonImage(IDB_BT_SHOW_SCORE,hInstance,false,false);
  m_btHintOx.SetButtonImage(IDB_HINT_OX,hInstance,false,false);
  m_btStart.SetButtonImage(IDB_START,hInstance,false,false);
//  m_btReSort.SetButtonImage(IDB_RESOCT,hInstance,false,true);
  m_btOpenCard.SetButtonImage(IDB_OPENCARD,hInstance,false,false);
  m_btOx.SetButtonImage(IDB_COW,hInstance,false,false);
//  m_btShortcut.SetButtonImage(IDB_ShortCut,hInstance,false,true);
  m_btOneScore.SetButtonImage(IDB__MONEY,hInstance,false,false);
  m_btTwoScore.SetButtonImage(IDB__MONEY,hInstance,false,false);
  m_btThreeScore.SetButtonImage(IDB__MONEY,hInstance,false,false);
  m_btFourScore.SetButtonImage(IDB__MONEY,hInstance,false,false);
  m_btBanker.SetButtonImage(IDB_BANKER,hInstance,false,false);
  m_btIdler.SetButtonImage(IDB_IDLER,hInstance,false,false);

  //创建控件
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    m_CardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,20+i);
    m_CardControlOx[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,20+GAME_PLAYER+i);
  }

  //按钮提示
  m_ToolTipCtrl.Create(this);
  m_ToolTipCtrl.Activate(TRUE);
//  m_ToolTipCtrl.AddTool(&m_btShortcut,TEXT("牛提示快捷键"));

#ifdef VIDEO_GAME
  //创建视频
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //创建视频
    m_DlgVideoService[i].Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,200+i);
    m_DlgVideoService[i].InitVideoService(i==2,i!=2);

    //设置视频
    g_VideoServiceManager.SetVideoServiceControl(i,&m_DlgVideoService[i]);
  }
#endif

  //BYTE bCardData[3] = {0x01,0x02,0x03};
  //BYTE bCardDataOx[2] = {0x04,0x05};

  //配置控件
  for(int i=0; i<GAME_PLAYER; i++)
  {
    m_CardControlOx[i].SetDisplayFlag(true);
    m_CardControlOx[i].SetCardSpace(18);
    m_CardControl[i].SetCardSpace(18);

    if(i==MY_VIEW_CHAIRID)
    {
      m_CardControlOx[i].SetCardSpace(95);
      m_CardControl[i].SetCardSpace(95);
    }


    //m_CardControl[i].SetCardData(bCardData,3);
    //m_CardControlOx[i].SetCardData(bCardDataOx,2);
  }

  //控制
  m_hInst = NULL;
  m_pClientControlDlg = NULL;
  m_hInst = LoadLibrary(TEXT("OxTobiNewClientControl.dll"));
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

  //特殊控制
  //m_hSpeInst = NULL;
  //m_pSpeClientControlDlg = NULL;
  //m_hSpeInst = LoadLibrary(TEXT("OxSpeClientControl.dll"));
  /*if ( m_hSpeInst )
  {
    typedef void * (*CREATE)(CWnd* pParentWnd);
    CREATE ClientControl = (CREATE)GetProcAddress(m_hSpeInst,"CreateClientControl");
    if ( ClientControl )
    {
      m_pSpeClientControlDlg = static_cast<ISpeClientControlDlg*>(ClientControl(this));
      m_pSpeClientControlDlg->ShowWindow( SW_HIDE );
    }
  }*/


  //控制按钮
  m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(4,4,11,11),this,IDC_ADMIN);
  m_btOpenAdmin.ShowWindow(SW_HIDE);

  return 0;
}

//命令函数
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
  switch(LOWORD(wParam))
  {
    case IDC_HINT_OX:     //提示按钮
    {
      //发送消息
      SendEngineMessage(IDM_HINT_OX,0,0);
      return TRUE;
    }
    case IDC_START:       //开始按钮
    {
      //发送消息
      SendEngineMessage(IDM_START,0,0);
      ZeroMemory(m_cbShootData,sizeof(m_cbShootData));
      return TRUE;
    }
    case IDC_READY:       //摊牌按钮
    {
      //发送消息
      SendEngineMessage(IDM_READY,0,0);
      return TRUE;
    }
    case IDC_OX:        //牛牛按钮
    {
      //发送消息
      SendEngineMessage(IDM_OX,0,0);
      return TRUE;
    }
    case IDC_RESORT:      //重排按钮
    {
      //发送消息
      SendEngineMessage(IDM_RESORT,0,0);
      return TRUE;
    }
    case IDC_ONE_SCORE:     //加注按钮
    {
      //发送消息
      SendEngineMessage(IDM_ADD_SCORE,4,1);
      return TRUE;
    }
    case IDC_TWO_SCORE:     //加注按钮
    {
      //发送消息
      SendEngineMessage(IDM_ADD_SCORE,3,2);
      return TRUE;
    }
    case IDC_THREE_SCORE:   //加注按钮
    {
      //发送消息
      SendEngineMessage(IDM_ADD_SCORE,2,3);
      return TRUE;
    }
    case IDC_FOUR_SCORE:    //加注按钮
    {
      //发送消息
      SendEngineMessage(IDM_ADD_SCORE,1,4);
      return TRUE;
    }
    case IDC_SHORTCUT:      //快捷键按钮
    {
      //发送消息
      SendEngineMessage(IDM_SHORTCUT_KEY,0,0);
      return TRUE;
    }
    case IDC_BANKER:      //庄家按钮
    {
      //发送消息
      SendEngineMessage(IDM_BANKER,1,1);
      return TRUE;
    }
    case IDC_IDLER:       //闲家按钮
    {
      //发送消息
      SendEngineMessage(IDM_BANKER,0,0);
      return TRUE;
    }
    case IDC_YU_YIN:
    {
      //发送消息
      SendEngineMessage(IDM_YU_YIN,0,0);
      return TRUE;
    }
  }

  return __super::OnCommand(wParam, lParam);
}

//重置界面
VOID CGameClientView::ResetGameView()
{
  __super::ResetGameView();

  //动画变量
  m_bJettonAction=false;
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    m_bOxValue[i]=0xff;
  }
  m_wSendCount=0;
  m_wSendIndex=0;
  m_cbDynamicJoin=FALSE;
  m_SendCardPos.SetPoint(0,0);
  m_SendCardItemArray.RemoveAll();
  ZeroMemory(m_cbShootData,sizeof(m_cbShootData));
  ZeroMemory(m_JettonStatus,sizeof(m_JettonStatus));
  ZeroMemory(m_lMoveGold,sizeof(m_lMoveGold));

  m_wBonusCartoonIndex = 0;
  m_bShowBonusCartoon = false;
  ZeroMemory(m_bBonus, sizeof(m_bBonus));

  //数据变量
  ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
  ZeroMemory(m_bOpenCardUser,sizeof(m_bOpenCardUser));
  ZeroMemory(m_bShowSocre,sizeof(m_bShowSocre));
  m_wBankerUser=INVALID_CHAIR;
  m_bCallUser=0xff;
  m_bShow=false;
  m_bInvest=false;
  m_bOpenStatus=false;
  m_bLookOnUser=false;
  m_bUserAction=false;
  m_bShowScore=false;
  m_bMeOpenCard=false;
  ZeroMemory(m_pHistoryScore,sizeof(m_pHistoryScore));

  //删除定时器
  KillTimer(IDI_SEND_CARD);

  //隐藏按钮
  m_btScore.ShowWindow(SW_HIDE);
  m_btHintOx.ShowWindow(SW_HIDE);
  m_btStart.ShowWindow(SW_HIDE);
//  m_btReSort.ShowWindow(SW_HIDE);
  m_btOpenCard.ShowWindow(SW_HIDE);
  m_btOx.ShowWindow(SW_HIDE);
  m_btOneScore.ShowWindow(SW_HIDE);
  m_btTwoScore.ShowWindow(SW_HIDE);
  m_btThreeScore.ShowWindow(SW_HIDE);
  m_btFourScore.ShowWindow(SW_HIDE);
//  m_btShortcut.ShowWindow(SW_HIDE);
  m_btBanker.ShowWindow(SW_HIDE);
  m_btIdler.ShowWindow(SW_HIDE);
  m_btYuYin.EnableWindow(FALSE);

  //还原按钮
  HINSTANCE hInstance=AfxGetInstanceHandle();
  m_btScore.SetButtonImage(IDB_BT_SHOW_SCORE,hInstance,false,false);

  //隐藏控件
//  m_ScoreView.ShowWindow(SW_HIDE);
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    m_CardControl[i].SetDisplayFlag(false);
    //m_CardControlOx[i].SetDisplayFlag(false);
    m_CardControl[i].SetCardData(NULL,0);
    m_CardControlOx[i].SetCardData(NULL,0);
  }

  //m_pClientControlDlg = NULL;
  //m_hInst = NULL;

  return;
}

//调整控件
VOID CGameClientView::RectifyControl(int nWidth, int nHeight)
{
  //发牌位置
  m_SendStartPos.SetPoint(nWidth/2,nHeight/2-30);

  m_ptBonus[0].x = nWidth/2-FACE_CX/2-45 + 180;
  m_ptBonus[0].y = 9+6;
  m_ptAvatar[0].x=nWidth/2-FACE_CX/2-45;
  m_ptAvatar[0].y=9+6;
  m_ptNickName[0].x=nWidth/2+35-50;
  m_ptNickName[0].y=9+14;
  m_ptJeton[0].x=nWidth/2-49;
  m_ptJeton[0].y=210;
  m_ptCard[0].x=m_ptJeton[0].x;
  m_ptCard[0].y=m_ptJeton[0].y-100;
  m_ptReady[0].x=m_ptAvatar[0].x-80;
  m_ptReady[0].y=m_ptAvatar[0].y+83;
  m_ptClock[0].x=m_ptAvatar[0].x+190;
  m_ptClock[0].y=m_ptAvatar[0].y+23;
  m_PointBanker[0].x = m_ptAvatar[0].x - 55;
  m_PointBanker[0].y =m_ptAvatar[0].y + 3;
  m_ptValue[0].x=nWidth/2+85;
  m_ptValue[0].y=129;

  m_ptBonus[2].x = nWidth/2-361 + 610;
  m_ptBonus[2].y = nHeight-140;
  m_ptAvatar[2].x=nWidth/2-361;
  m_ptAvatar[2].y=nHeight-120;
  m_ptNickName[2].x=m_ptAvatar[2].x-5;
  m_ptNickName[2].y=m_ptAvatar[2].y +FACE_CY+16;
  m_ptJeton[2].x=nWidth/2-49;
  m_ptJeton[2].y=nHeight-300;
  m_ptCard[2].x=m_ptJeton[2].x;
  m_ptCard[2].y=m_ptJeton[2].y+90;
  m_ptReady[2].x=nWidth/2 - 33;
  m_ptReady[2].y=m_ptAvatar[2].y-160;
  m_ptClock[2].x=m_ptAvatar[2].x+(FACE_CX-65)/2+32;
  m_ptClock[2].y=m_ptAvatar[2].y-52;
  m_PointBanker[2].x = m_ptAvatar[2].x - 54+121;
  m_PointBanker[2].y =m_ptAvatar[2].y +FACE_CY-39+67;
  m_ptValue[2].x=nWidth/2+85-130;
  m_ptValue[2].y=nHeight-200;

  m_ptBonus[1].x = nWidth-FACE_CX-16-5 - 130;
  m_ptBonus[1].y = nHeight/2-FACE_CY/2-25 + 210;
  m_ptAvatar[1].x=nWidth-FACE_CX-16-5;
  m_ptAvatar[1].y=nHeight/2-FACE_CY/2-25;
  m_ptNickName[1].x=m_ptAvatar[1].x-5;
  m_ptNickName[1].y=m_ptAvatar[1].y+FACE_CY+6+9;
  m_ptJeton[1].x=nWidth-328+25-36;
  m_ptJeton[1].y=nHeight/2-62;
  m_ptCard[1].x=m_ptJeton[1].x+100;
  m_ptCard[1].y=m_ptJeton[1].y;
  m_ptReady[1].x=m_ptAvatar[1].x-160;
  m_ptReady[1].y=m_ptAvatar[1].y-70;
  m_ptClock[1].x=m_ptAvatar[1].x+22;
  m_ptClock[1].y=m_ptAvatar[1].y-50;
  m_PointBanker[1].x = m_ptAvatar[1].x+2;
  m_PointBanker[1].y = m_ptAvatar[1].y-40+157;
  m_ptValue[1].x=nWidth-180;
  m_ptValue[1].y=nHeight/2+40;

  m_ptBonus[3].x = 18;
  m_ptBonus[3].y = nHeight/2-FACE_CY/2-25 - 175;
  m_ptAvatar[3].x=18;
  m_ptAvatar[3].y=nHeight/2-FACE_CY/2-25;
  m_ptNickName[3].x=m_ptAvatar[3].x-5;
  m_ptNickName[3].y=m_ptAvatar[3].y+FACE_CY+6+9;
  m_ptJeton[3].x=263-30+10;
  m_ptJeton[3].y=nHeight/2-62;
  m_ptCard[3].x=m_ptJeton[3].x-100;
  m_ptCard[3].y=m_ptJeton[3].y;
  m_ptReady[3].x=m_ptAvatar[3].x+140;
  m_ptReady[3].y=m_ptAvatar[3].y-70;
  m_ptClock[3].x=m_ptAvatar[3].x+22;
  m_ptClock[3].y=m_ptAvatar[3].y-50;
  m_PointBanker[3].x =m_ptAvatar[3].x+2;
  m_PointBanker[3].y =m_ptAvatar[3].y-40+157;
  m_ptValue[3].x=99;
  m_ptValue[3].y=nHeight/2+40;

  //左右排列
  m_CardControl[0].SetBenchmarkPos(nWidth/2,55+25,enXCenter,enYTop);
  m_CardControlOx[0].SetBenchmarkPos(nWidth/2,55,enXCenter,enYTop);
  m_CardControl[2].SetBenchmarkPos(nWidth/2-229,nHeight-10,enXLeft,enYBottom);
  m_CardControlOx[2].SetBenchmarkPos(nWidth/2-229 + 95*3,nHeight-10,enXLeft,enYBottom);
  m_CardControl[1].SetBenchmarkPos(nWidth-87,nHeight/2-19,enXRight,enYCenter);
  m_CardControlOx[1].SetBenchmarkPos(nWidth-87-10,nHeight/2-19-25,enXRight,enYCenter);
  m_CardControl[3].SetBenchmarkPos(89,nHeight/2-19,enXLeft,enYCenter);
  m_CardControlOx[3].SetBenchmarkPos(89+10,nHeight/2-19-25,enXLeft,enYCenter);

  //结束位置
  m_SendEndingPos[0].x=m_CardControl[0].GetCardPos().x;
  m_SendEndingPos[0].y=m_CardControl[0].GetCardPos().y;
  m_SendEndingPos[1].x=m_CardControl[1].GetCardPos().x;
  m_SendEndingPos[1].y=m_CardControl[1].GetCardPos().y;
  m_SendEndingPos[2].x=m_CardControl[2].GetCardPos().x;
  m_SendEndingPos[2].y=m_CardControl[2].GetCardPos().y;
  m_SendEndingPos[3].x=m_CardControl[3].GetCardPos().x;
  m_SendEndingPos[3].y=m_CardControl[3].GetCardPos().y;

  //牛值位置
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    m_ptOpenTag[i]=m_ptValue[i];
  }

  //按钮控件
  CRect rcButton;
  HDWP hDwp=BeginDeferWindowPos(32);
  const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

  //开始按钮
  m_btStart.GetWindowRect(&rcButton);
  DeferWindowPos(hDwp,m_btStart,NULL,nWidth/2-46,nHeight-246,0,0,uFlags);

  //控制按钮
  m_btOpenCard.GetWindowRect(&rcButton);
  //DeferWindowPos(hDwp,m_btReSort,NULL,nWidth/2-32,nHeight-237,0,0,uFlags);
  DeferWindowPos(hDwp,m_btOpenCard,NULL,nWidth/2+255,nHeight-110,0,0,uFlags);
  DeferWindowPos(hDwp,m_btOx,NULL,nWidth/2-118,nHeight-237,0,0,uFlags);
  DeferWindowPos(hDwp,m_btHintOx,NULL,nWidth/2+255,nHeight-60,0,0,uFlags);

  //辅助按钮
//  DeferWindowPos(hDwp,m_btShortcut,NULL,nWidth/2+330,nHeight/2+320,0,0,uFlags);

  //筹码按钮
  int nBtSpace = 5;
  m_btOneScore.GetWindowRect(&rcButton);
  DeferWindowPos(hDwp,m_btOneScore,NULL,nWidth/2-200+rcButton.Width()*0+nBtSpace*0,nHeight-186,0,0,uFlags);
  DeferWindowPos(hDwp,m_btTwoScore,NULL,nWidth/2-200+rcButton.Width()*1+nBtSpace*1,nHeight-186,0,0,uFlags);
  DeferWindowPos(hDwp,m_btThreeScore,NULL,nWidth/2-200+rcButton.Width()*2+nBtSpace*2,nHeight-186,0,0,uFlags);
  DeferWindowPos(hDwp,m_btFourScore,NULL,nWidth/2-200+rcButton.Width()*3+nBtSpace*3,nHeight-186,0,0,uFlags);

  //选庄按钮
  DeferWindowPos(hDwp,m_btBanker,NULL,nWidth/2-99,nHeight-198+10,0,0,uFlags);
  DeferWindowPos(hDwp,m_btIdler,NULL,nWidth/2+10,nHeight-198+10,0,0,uFlags);

  m_btYuYin.GetWindowRect(&rcButton);
  DeferWindowPos(hDwp,m_btYuYin,NULL,nWidth/2,nHeight-rcButton.Height()-5,0,0,uFlags);

#ifdef VIDEO_GAME
  //视频窗口
  DeferWindowPos(hDwp,m_DlgVideoService[0],NULL,nWidth/2-162,nHeight/2-350,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_DlgVideoService[1],NULL,nWidth/2+287,nHeight/2-172,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_DlgVideoService[2],NULL,nWidth/2-290,nHeight/2+195,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_DlgVideoService[3],NULL,nWidth/2-375,nHeight/2-172,0,0,uFlags|SWP_NOSIZE);
#endif

  //查分按钮
  CRect rcScore;
  m_btScore.GetWindowRect(&rcScore);
  DeferWindowPos(hDwp,m_btScore,NULL,nWidth-40,25,0,0,uFlags);

  //结束移动
  EndDeferWindowPos(hDwp);

  //调整扩展
  RectifyControlEx();

  //积分视图
  //CRect rcControl;
  //m_ScoreView.GetWindowRect(&rcControl);
  //m_ScoreView.SetWindowPos(NULL,nWidth-66-m_sizeHistory.cx,5,0,0,SWP_NOZORDER|SWP_NOSIZE);

  return;
}

//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
  //绘画背景
  //DrawViewImage(pDC,m_ImageViewCenter,DRAW_MODE_SPREAD);
  //DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);
  for(int iW = 0 ; iW < nWidth; iW += m_ImageViewFill.GetWidth())
  {
    for(int iH = 0;  iH < nHeight; iH += m_ImageViewFill.GetHeight())
    {
      m_ImageViewFill.DrawImage(pDC, iW, iH);
    }
  }
  m_ImageViewCenter.DrawImage(pDC, nWidth/2 - m_ImageViewCenter.GetWidth()/2, nHeight/2 - m_ImageViewCenter.GetHeight()/2);

  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    if(i==0)
    {
      m_PngFrameTB.DrawImage(pDC,m_ptAvatar[i].x-9,m_ptAvatar[i].y-9);
    }
    else
    {
      m_PngFrameLR.DrawImage(pDC,m_ptAvatar[i].x-15,m_ptAvatar[i].y-14);
    }
  }

  //虚线框
  if(!m_bMeOpenCard)
  {
    BYTE bCardCount = m_CardControl[MY_VIEW_CHAIRID].GetCardCount() + m_CardControlOx[MY_VIEW_CHAIRID].GetCardCount();
    m_PngVirtualLine.DrawImage(pDC,(nWidth-m_PngVirtualLine.GetWidth())/2,nHeight-130,m_PngVirtualLine.GetWidth()/5*bCardCount,m_PngVirtualLine.GetHeight(),0,0);
  }

  //绘画用户
  pDC->SetTextAlign(TA_LEFT);
  pDC->SetTextColor(RGB(255,255,255));
  CRect rcUserInfo(0,0,0,0);
  UINT nTextFormat = DT_CENTER|DT_TOP|DT_END_ELLIPSIS|DT_NOCLIP;
  for(BYTE i=0; i<GAME_PLAYER; i++)
  {
    //变量定义
    WORD wUserTimer=GetUserClock(i);
    IClientUserItem * pIClientUserItem=GetClientUserItem(i);

    //绘画用户
    if(pIClientUserItem!=NULL)
    {
      TCHAR szBuffer[64]=TEXT("");

      //用户名字
      int nTextWidth = (i==0?80:64);
      _sntprintf(szBuffer,CountArray(szBuffer),TEXT("%s"),pIClientUserItem->GetNickName());
      rcUserInfo.SetRect(m_ptNickName[i].x,m_ptNickName[i].y,m_ptNickName[i].x+nTextWidth,m_ptNickName[i].y+18);
      CDFontEx::DrawText(this,pDC,  12, 400, szBuffer, rcUserInfo,RGB(255,255,255), nTextFormat);

      //用户金币
      SCORE lLeaveScore=pIClientUserItem->GetUserScore()-(pIClientUserItem->GetUserStatus()!=US_PLAYING?0:m_lTableScore[i]);
      _sntprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),lLeaveScore);
      rcUserInfo.SetRect(m_ptNickName[i].x+18,m_ptNickName[i].y+20,m_ptNickName[i].x+nTextWidth,m_ptNickName[i].y+37);
      CDFontEx::DrawText(this,pDC,  12, 400, szBuffer, rcUserInfo,RGB(255,255,255), DT_LEFT|DT_TOP|DT_END_ELLIPSIS|DT_NOCLIP);

      //其他信息
      if(wUserTimer!=0)
      {
        if(m_bCallUser==0xff && (pIClientUserItem->GetUserStatus()==US_PLAYING||m_cbDynamicJoin))
        {
          DrawUserTimerEx(pDC,nWidth/2,nHeight/2-50,wUserTimer,99L);
        }
        else
        {
          DrawUserTimerEx(pDC,m_ptClock[i].x,m_ptClock[i].y,wUserTimer,99L);
        }
      }

      if(pIClientUserItem->GetUserStatus()==US_READY)
      {
        DrawUserReadyEx(pDC,m_ptReady[i].x,m_ptReady[i].y);
      }

      //绘画属性
      pDC->FillSolidRect(m_ptAvatar[i].x-2,m_ptAvatar[i].y-2,52,52,RGB(255,255,255));
      DrawUserAvatar(pDC,m_ptAvatar[i].x,m_ptAvatar[i].y,pIClientUserItem);

    }
    //else
    //{
    //  m_CardControl[i].SetOX(false);
    //  m_CardControl[i].SetCardData(NULL,0);
    //  m_CardControlOx[i].SetCardData(NULL,0);
    //}
  }

  //等待下注
  if(m_bInvest)
  {
    m_PngWaitBet.DrawImage(pDC,nWidth/2-m_PngWaitBet.GetWidth()/2,nHeight/2-18);
  }

  //等待摊牌
  if(m_bOpenStatus)
  {
    m_PngWaitOpen.DrawImage(pDC,nWidth/2-m_PngWaitOpen.GetWidth()/2,nHeight/2-18);
  }

  //特殊牌型
  if(m_bUserAction)
  {
    m_PngAutoOpen.DrawImage(pDC,nWidth/2-m_PngAutoOpen.GetWidth()/2,nHeight/2+90);//特殊牌型,系统自动为你配牌
  }

  //等待叫庄
  if(m_bCallUser!=0xff)
  {
    m_ImageWaitCall.DrawImage(pDC,nWidth/2-m_ImageWaitCall.GetWidth()/2,nHeight/2-18);
  }

  //庄家信息
  if(m_wBankerUser!=INVALID_CHAIR)
  {
    IClientUserItem * pIClientUserItem=GetClientUserItem(m_wBankerUser);
    if(pIClientUserItem!=NULL)
    {
      m_ImageBanker.DrawImage(pDC,m_PointBanker[m_wBankerUser].x,m_PointBanker[m_wBankerUser].y);
    }
  }

  //绘画积分
  for(BYTE i=0; i<GAME_PLAYER; i++)
  {
    IClientUserItem * pIClientUserItem=GetClientUserItem(i);
    if(pIClientUserItem!=NULL &&(m_lTableScore[i]!=0L))
    {
      //调整位置
      int iPos=1;
      if(m_bShowSocre[i])
      {
        if(m_lTableScore[i]>0L)
        {
          iPos=0;
        }
        else if(m_lTableScore[i]<=0L)
        {
          iPos=2;
        }
      }

      INT nX = m_ptJeton[i].x;
      INT nY =m_ptJeton[i].y;

      //绘画背景
      m_ImageScore.DrawImage(pDC,nX,nY,m_ImageScore.GetWidth(),
                             m_ImageScore.GetHeight(),0,0);

      //绘画信息
      TCHAR tc[LEN_ACCOUNTS]=TEXT("");
      _sntprintf(tc,CountArray(tc),TEXT("%I64d"),m_lTableScore[i]);
      CRect rcDraw(nX+10,nY+10,nX+86,nY+35);
      m_FontEx.DrawText(pDC,tc,&rcDraw,RGB(42,67,68),DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    }
  }

  //显示摊牌标志
  for(BYTE i=0; i<GAME_PLAYER; i++)
  {
    IClientUserItem * pIClientUserItem=GetClientUserItem(i);

    if(pIClientUserItem!=NULL)
    {
      if(m_bOpenCardUser[i]==TRUE && i!=2)
      {
        m_PngOpenTag.DrawImage(pDC,m_ptOpenTag[i].x,m_ptOpenTag[i].y);
      }
    }

  }

  //绘画扑克
  if(m_SendCardItemArray.GetCount()>0)
  {
    //变量定义
    tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

    //获取大小
    int nItemWidth=m_ImageCard.GetWidth()/13;
    int nItemHeight=m_ImageCard.GetHeight()/5;

    //绘画扑克
    int nXPos=m_SendCardPos.x-nItemWidth/2;
    int nYPos=m_SendCardPos.y-nItemHeight/2+10;
    m_ImageCard.DrawImage(pDC,nXPos,nYPos,nItemWidth,nItemHeight,nItemWidth*2,nItemHeight*4);
  }


  //选牌点数
  if(m_btOpenCard.IsWindowVisible())
  {
    int nTotalValue = 0;
    CPoint ptAdd(nWidth/2-m_PngOxAddBack.GetWidth()/2,nHeight-209);
    m_PngOxAddBack.DrawImage(pDC,ptAdd.x,ptAdd.y);

    TCHAR szTotalValue[5];
    for(BYTE i=0; i<m_cbShootCount; i++)
    {
      if(i>2)
      {
        break;
      }

      BYTE cbCardValue = m_GameLogic.GetCardLogicValue(m_cbShootData[i]);
      nTotalValue += cbCardValue;

      if(cbCardValue>0)
      {
        DrawNumberString(pDC,cbCardValue,ptAdd.x+37+i*80,ptAdd.y+45,1);
      }
    }

    if(nTotalValue>0)
    {
      BYTE bTypeIndex = 0;
      ZeroMemory(szTotalValue,sizeof(szTotalValue));
      if(m_bOxValue[MY_VIEW_CHAIRID]!=0xff)
      {
        if(m_bOxValue[MY_VIEW_CHAIRID]!=0)
        {
          if(m_bOxValue[MY_VIEW_CHAIRID]==10 || m_bOxValue[MY_VIEW_CHAIRID]>=OX_THREE_SAME)
          {
            bTypeIndex = 10;
            _sntprintf(szTotalValue,sizeof(szTotalValue),_T("牛牛"));
          }
          else if(m_bOxValue[MY_VIEW_CHAIRID]<10)
          {
            bTypeIndex = m_bOxValue[MY_VIEW_CHAIRID];
            _sntprintf(szTotalValue,sizeof(szTotalValue),_T("牛%d"),m_bOxValue[MY_VIEW_CHAIRID]);
          }
        }
        else
        {
          bTypeIndex = 0;
          _sntprintf(szTotalValue,sizeof(szTotalValue),_T("无牛"));
        }
      }
      else if(m_cbShootCount >= 2)
      {
        bTypeIndex = 0;
        _sntprintf(szTotalValue,sizeof(szTotalValue),_T("无牛"));
      }

      int nPngWidth = m_PngOxAddType.GetWidth()/14;
      int nPngHeight = m_PngOxAddType.GetHeight();
      m_PngOxAddType.DrawImage(pDC,ptAdd.x+14+3*80,ptAdd.y+32,nPngWidth,nPngHeight,nPngWidth*bTypeIndex,0,nPngWidth,nPngHeight);
    }
  }

  //显示点数
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    for(BYTE i=0; i<GAME_PLAYER; i++)
    {
      IClientUserItem * pIClientUserItem=GetClientUserItem(i);

      if(pIClientUserItem!=NULL)
      {
        if(m_bOxValue[i]!=0xff)
        {
          if(m_bShow ||(i==MY_VIEW_CHAIRID && m_bMeOpenCard))
          {
            INT nXpt = m_ptValue[i].x;
            INT nypt = m_ptValue[i].y;
            if(m_bOxValue[i]!=0)
            {
              m_ImageOxValue.DrawImage(pDC,nXpt,nypt);
              if(m_bOxValue[i]==10 || m_bOxValue[i]>=OX_THREE_SAME)
              {
                m_ImageDoulbeOx.DrawImage(pDC,nXpt,nypt);
              }
              else if(m_bOxValue[i]<10)
              {
                m_ImageNumber.DrawImage(pDC,nXpt+40,nypt+7,
                                        m_ImageNumber.GetWidth()/9,m_ImageNumber.GetHeight(),
                                        m_ImageNumber.GetWidth()/9*((int)m_bOxValue[i]-1),0);
              }
            }
            else if(m_bOxValue[i]==0)
            {
              m_ImageOxValueZero.DrawImage(pDC,nXpt,nypt);
            }
          }
        }
      }
    }
  }

  //彩金动画
  if(m_bShowBonusCartoon)
  {
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      if(m_bBonus[i])
      {
        m_PngBonus.DrawImage(pDC, m_ptBonus[i].x, m_ptBonus[i].y, BONUS_PERPOSX, BONUS_PERPOSY,  BONUS_PERPOSX*m_wBonusCartoonIndex, 0);

        //彩金数字
        DrawNumber(pDC, &m_PngBonusNum, TEXT("0123456789"), m_lBonus, m_ptBonus[i].x + 188, m_ptBonus[i].y + 120);
      }
    }
  }

  //历史积分
  if(m_bShowScore==true)
  {
    //积分背景
    m_PngHistoryScore.DrawImage(pDC,nWidth-40-m_sizeHistory.cx,5);

    //计算位置
    INT nYBenchmark=22;
    INT nXBenchmark=nWidth-30-m_sizeHistory.cx;
    UINT nFormat=DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER;

    //绘画属性
    pDC->SetTextAlign(TA_LEFT|TA_TOP);

    //绘画信息
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      //获取用户
      IClientUserItem * pIClientUserItem=GetClientUserItem(i);
      if((pIClientUserItem==NULL)||(m_pHistoryScore[i]==NULL))
      {
        continue;
      }

      //位置计算
      INT nY1=27,nY2=nY1+18,nSpa=25;
      CRect rcAccounts(nXBenchmark+4,nYBenchmark+nY1+i*nSpa,nXBenchmark+68+7,nYBenchmark+nY2+i*nSpa);
      CRect rcTurnScore(nXBenchmark+70+15,nYBenchmark+nY1+i*nSpa,nXBenchmark+121+15,nYBenchmark+nY2+i*nSpa);
      CRect rcCollectScore(nXBenchmark+150,nYBenchmark+nY1+i*nSpa,nXBenchmark+201,nYBenchmark+nY2+i*nSpa);

      //构造信息
      TCHAR szTurnScore[16]=TEXT(""),szCollectScore[16]=TEXT("");
      _sntprintf(szTurnScore,CountArray(szTurnScore),TEXT("%I64d"),m_pHistoryScore[i]->lTurnScore);
      _sntprintf(szCollectScore,CountArray(szCollectScore),TEXT("%I64d"),m_pHistoryScore[i]->lCollectScore);

      //绘画信息
      COLORREF crf;
      CDFontEx::DrawText(this,pDC,  12, 400, pIClientUserItem->GetNickName(), rcAccounts,RGB(255,255,255), nFormat);

      crf = m_pHistoryScore[i]->lTurnScore>0?RGB(255,255,0):RGB(0,255,0);
      CDFontEx::DrawText(this,pDC,  12, 400, szTurnScore, rcTurnScore,crf, nFormat);

      crf = m_pHistoryScore[i]->lCollectScore>0?RGB(255,255,0):RGB(0,255,0);
      CDFontEx::DrawText(this,pDC,  12, 400, szCollectScore, rcCollectScore,crf, nFormat);
    }
  }

  return;
}


//绘画数字
void CGameClientView::DrawNumberString(CDC * pDC, SCORE lNumber, INT nXPos, INT nYPos, BYTE bPngType)
{
  //加载资源
  CSize SizeScoreNumber(m_ImageTimeNumber.GetWidth()/10,m_ImageTimeNumber.GetHeight());
  if(bPngType== 1)
  {
    SizeScoreNumber.SetSize(m_PngOxAddNum.GetWidth()/10, m_PngOxAddNum.GetHeight());
  }

  //计算数目
  int lNumberCount=0;
  SCORE lNumberTemp=lNumber;
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
    if(bPngType == 1)
    {
      m_PngOxAddNum.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
                              lCellNumber*SizeScoreNumber.cx,0);
    }
    else
    {
      m_ImageTimeNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
                                  lCellNumber*SizeScoreNumber.cx,0);
    }

    //设置变量
    lNumber/=10;
    nXDrawPos-=SizeScoreNumber.cx;
  };

  return;
}


//显示弹牌点数
void CGameClientView::SetShootCardData(BYTE cbCardData[MAX_COUNT],BYTE cbCardCount)
{

  ZeroMemory(m_cbShootData,sizeof(m_cbShootData));

  m_cbShootCount = cbCardCount;
  for(BYTE i=0; i<cbCardCount; i++)
  {
    m_cbShootData[i] = cbCardData[i];
  }


  //更新界面
  InvalidGameView(0,0,0,0);

  return;
}


//绘画时间
void CGameClientView::DrawUserTimerEx(CDC * pDC, int nXPos, int nYPos, WORD wTime, WORD wTimerArea)
{
//#ifdef _DEBUG
//  return;
//#endif
  //获取属性
  const INT nNumberHeight=m_ImageTimeNumber.GetHeight();
  const INT nNumberWidth=m_ImageTimeNumber.GetWidth()/10;

  //计算数目
  LONG lNumberCount=0;
  WORD wNumberTemp=wTime;
  do
  {
    lNumberCount++;
    wNumberTemp/=10;
  }
  while(wNumberTemp>0L);

  //位置定义
  INT nYDrawPos=nYPos-nNumberHeight/2+1;
  INT nXDrawPos=nXPos+(lNumberCount*nNumberWidth)/2-nNumberWidth;

  //绘画背景
  CSize SizeTimeBack(m_ImageTimeBack.GetWidth(),m_ImageTimeBack.GetHeight());
  m_ImageTimeBack.DrawImage(pDC,nXPos-SizeTimeBack.cx/2,nYPos-SizeTimeBack.cy/2);

  //绘画号码
  for(LONG i=0; i<lNumberCount; i++)
  {
    //绘画号码
    WORD wCellNumber=wTime%10;
    m_ImageTimeNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,wCellNumber*nNumberWidth,0);

    //设置变量
    wTime/=10;
    nXDrawPos-=nNumberWidth;
  }

  return;
}

//提示信息
void CGameClientView::SetUserAction(bool bWait)
{
  if(m_bUserAction!=bWait)
  {
    m_bUserAction=bWait;

    //更新界面
    InvalidGameView(0,0,0,0);
  }

  //设置时间
  if(bWait)
  {
    SetTimer(IDI_USER_ACTION,TIME_USER_ACTION,NULL);
  }
  else
  {
    KillTimer(IDI_USER_ACTION);
  }

  return;
}

//动画判断
bool CGameClientView::IsDispatchCard()
{
  return m_SendCardItemArray.GetCount()>0;
}

//完成发牌
void CGameClientView::FinishDispatchCard()
{
  //完成判断
  if(m_SendCardItemArray.GetCount()==0)
  {
    return;
  }

  //完成动画
  for(INT_PTR i=0; i<m_SendCardItemArray.GetCount(); i++)
  {
    //获取数据
    tagSendCardItem * pSendCardItem=&m_SendCardItemArray[i];

    //获取扑克
    BYTE cbCardData[MAX_COUNT];
    WORD wChairID=pSendCardItem->wChairID;
    BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

    //设置扑克
    if(cbCardCount<MAX_COUNT)
    {
      cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
      m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);
    }
  }

  SendEngineMessage(IDM_SEND_CARD_FINISH,0,0);

  //删除处理
  KillTimer(IDI_SEND_CARD);
  m_SendCardItemArray.RemoveAll();

  //更新界面
  InvalidGameView(0,0,0,0);

  return;
}

//发送扑克
void CGameClientView::DispatchUserCard(WORD wChairID, BYTE cbCardData)
{
  //效验参数
  ASSERT(wChairID<GAME_PLAYER);
  if(wChairID>=GAME_PLAYER)
  {
    return;
  }

  //设置变量
  tagSendCardItem SendCardItem;
  SendCardItem.cbCardData=cbCardData;
  SendCardItem.wChairID=wChairID;

  //插入数组
  m_SendCardItemArray.Add(SendCardItem);

  //启动动画
  if(m_SendCardItemArray.GetCount()==1)
  {
    //获取位置
    CRect rcClient;
    GetClientRect(&rcClient);

    //设置位置
    m_SendCardPos.x=m_SendStartPos.x;
    m_SendCardPos.y=m_SendStartPos.y;

    //位移计算
    int nXCount=abs(m_SendEndingPos[SendCardItem.wChairID].x-m_SendStartPos.x)/SEND_PELS;
    int nYCount=abs(m_SendEndingPos[SendCardItem.wChairID].y-m_SendStartPos.y)/SEND_PELS;

    //设置次数
    m_wSendIndex=0;
    //m_wSendCount=__max(1,__max(nXCount,nYCount));
    m_wSendCount=3;

    //设置定时器
    SetTimer(IDI_SEND_CARD,TIME_SENDSPEED,NULL);

    //播放声音
    CGameClientEngine * pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
    pGameClientEngine->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
  }

  return;
}

//设置下注
void CGameClientView::SetUserTableScore(WORD wChairID, SCORE lTableScore,bool bMove)
{
  //设置数据
  if(wChairID<GAME_PLAYER)
  {
    if(bMove)
    {
      DrawMoveAnte(wChairID,lTableScore);
    }
    else
    {
      m_lTableScore[wChairID]=lTableScore;
    }
  }
  else
  {
    ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
  }

  //更新界面
  InvalidGameView(0,0,0,0);
  return;
}

//庄家标志
void CGameClientView::SetBankerUser(WORD wBankerUser)
{
  m_wBankerUser=wBankerUser;

  //更新界面
  InvalidGameView(0,0,0,0);
  return;
}

//等待叫庄
void CGameClientView::SetWaitCall(BYTE bCallUser)
{
  m_bCallUser=bCallUser;

  //更新界面
  InvalidGameView(0,0,0,0);

  return ;
}

//等待标志
void CGameClientView::SetWaitInvest(bool bInvest)
{
  if(m_bInvest!=bInvest)
  {
    m_bInvest=bInvest;

    //更新界面
    InvalidGameView(0,0,0,0);
  }
  return;
}

//左上信息
//void CGameClientView::SetScoreInfo(SCORE lTurnMaxScore,SCORE lTurnLessScore)
//{
//  m_lTurnMaxScore=lTurnMaxScore;
//  //m_lTurnLessScore=lTurnLessScore;
//
//  //更新界面
//  InvalidGameView(0,0,0,0);
//  return;
//}

//显示牌型
void CGameClientView::DisplayTypeEx(bool bShow)
{
  if(m_bShow!=bShow)
  {
    m_bShow=bShow;

    //更新界面
    InvalidGameView(0,0,0,0);
  }

  return;
}

//显示点数
void CGameClientView::SetUserOxValue(WORD wChiarID,BYTE bValue)
{
  //牛牛数据
  m_bOxValue[wChiarID]=bValue;

  //更新界面
  InvalidGameView(0,0,0,0);

  return;
}

//显示摊牌
void CGameClientView::ShowOpenCard(WORD wChiarID,BOOL bShow)
{
  //摊牌用户
  m_bOpenCardUser[wChiarID]=bShow;

  //更新界面
  InvalidGameView(0,0,0,0);

  return;
}

//调整扩展
void CGameClientView::RectifyControlEx()
{
  CRect rcClient;
  GetClientRect(&rcClient);

  //上下排列
  if(m_bMeOpenCard)
  {
    m_CardControlOx[MY_VIEW_CHAIRID].SetCardSpace(23);
    m_CardControl[MY_VIEW_CHAIRID].SetCardSpace(23);
    m_CardControl[MY_VIEW_CHAIRID].SetBenchmarkPos(rcClient.Width()/2,rcClient.Height()-10,enXCenter,enYBottom);
    m_CardControlOx[MY_VIEW_CHAIRID].SetBenchmarkPos(rcClient.Width()/2,rcClient.Height()-10-25,enXCenter,enYBottom);
  }
  else
  {
    m_CardControlOx[MY_VIEW_CHAIRID].SetCardSpace(95);
    m_CardControl[MY_VIEW_CHAIRID].SetCardSpace(95);
    m_CardControl[2].SetBenchmarkPos(rcClient.Width()/2-229,rcClient.Height()-10,enXLeft,enYBottom);
    m_CardControlOx[2].SetBenchmarkPos(rcClient.Width()/2-229 + 95*3,rcClient.Height()-10,enXLeft,enYBottom);
  }

  //更新界面
  InvalidGameView(0,0,0,0);

  return;
}

//显示积分
void CGameClientView::ShowScore(WORD wChairID,bool bShow)
{
  //显示标志
  m_bShowSocre[wChairID]=bShow;

  //更新界面
  InvalidGameView(0,0,0,0);

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
  pDC->SetTextColor(crFrame);
  for(int i=0; i<CountArray(nXExcursion); i++)
  {
    //pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
    CDFontEx::DrawText(this,pDC,  12, 400, pszString, nXPos+nXExcursion[i],nYPos+nYExcursion[i],crFrame, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
  }

  //绘画字体
  pDC->SetTextColor(crText);
  //pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

  CDFontEx::DrawText(this,pDC,  12, 400, pszString, nXPos,nYPos,crText, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

  return;
}

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect)
{
  //变量定义
  int nStringLength=lstrlen(pszString);
  int nXExcursion[8]= {1,1,1,0,-1,-1,-1,0};
  int nYExcursion[8]= {-1,0,1,1,1,0,-1,-1};

  //绘画边框
  pDC->SetTextColor(crFrame);
  CRect rcDraw;
  for(int i=0; i<CountArray(nXExcursion); i++)
  {
    rcDraw.CopyRect(lpRect);
    rcDraw.OffsetRect(nXExcursion[i],nYExcursion[i]);
    //pDC->DrawText(pszString,nStringLength,&rcDraw,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

    CDFontEx::DrawText(this,pDC,  12, 400, pszString, rcDraw,crFrame, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
  }

  //绘画字体
  rcDraw.CopyRect(lpRect);
  pDC->SetTextColor(crText);
  //pDC->DrawText(pszString,nStringLength,&rcDraw,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

  CDFontEx::DrawText(this,pDC,  12, 400, pszString, rcDraw,crText, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

  return;
}

//积分按钮
VOID CGameClientView::OnBnClickedScore()
{
  //设置变量
  m_bShowScore=!m_bShowScore;

  //设置按钮
  HINSTANCE hInstance=AfxGetInstanceHandle();
  m_btScore.SetButtonImage((m_bShowScore==true)?IDB_BT_CLOSE_SCORE:IDB_BT_SHOW_SCORE,hInstance,false,false);

  //更新界面
  InvalidGameView(0,0,0,0);

  return;
}

//设置积分
bool CGameClientView::SetHistoryScore(WORD wChairID,tagHistoryScore * pHistoryScore)
{
  //效验参数
  ASSERT(wChairID<GAME_PLAYER);
  if(wChairID>=GAME_PLAYER)
  {
    return false;
  }

  //设置变量
  m_pHistoryScore[wChairID]=pHistoryScore;

  //更新界面
  InvalidGameView(0,0,0,0);

  return true;
}

//绘制动画, 0: 底注动画, 1: 加注动画, 2: 加注合并到底注, 3: 赢家收筹码
void CGameClientView::DrawMoveAnte(WORD wViewChairID, SCORE lTableScore)
{
  //动画步数
  int nAnimeStep = 60;

  m_JettonStatus[wViewChairID].wMoveIndex = 0;
  m_lMoveGold[wViewChairID]=lTableScore;

  m_JettonStatus[wViewChairID].ptFrom = m_ptCard[wViewChairID];
  m_JettonStatus[wViewChairID].ptDest = m_ptJeton[wViewChairID];
  m_JettonStatus[wViewChairID].ptCourse= m_ptCard[wViewChairID];
  m_JettonStatus[wViewChairID].lGold =lTableScore;

  //位移计算
  INT nXCount=abs(m_JettonStatus[wViewChairID].ptDest.x-m_JettonStatus[wViewChairID].ptFrom.x)/nAnimeStep+rand()%8;
  INT nYCount=abs(m_JettonStatus[wViewChairID].ptDest.y-m_JettonStatus[wViewChairID].ptFrom.y)/nAnimeStep+rand()%8;
  m_JettonStatus[wViewChairID].wMoveCount = __max(1,__max(nXCount,nYCount));;

  //设置时间
  if(!m_bJettonAction)
  {
    m_bJettonAction = true;
    SetTimer(IDI_JETTON_ACTION,50,NULL);  //--------以前是50
  }

  return;
}

//移动筹码
bool CGameClientView::MoveJetton()
{
  bool bAllClean = true;

  //设置变量
  for(BYTE i=0; i<GAME_PLAYER; i++)
  {
    //移动步数
    if(m_JettonStatus[i].wMoveIndex<m_JettonStatus[i].wMoveCount)
    {
      bAllClean = false;
      m_JettonStatus[i].wMoveIndex++;
      WORD wMoveIndex = m_JettonStatus[i].wMoveIndex;
      WORD wMoveCount = m_JettonStatus[i].wMoveCount;
      m_JettonStatus[i].ptCourse.x = m_JettonStatus[i].ptFrom.x + (m_JettonStatus[i].ptDest.x-m_JettonStatus[i].ptFrom.x)*wMoveIndex/wMoveCount;
      m_JettonStatus[i].ptCourse.y = m_JettonStatus[i].ptFrom.y + (m_JettonStatus[i].ptDest.y-m_JettonStatus[i].ptFrom.y)*wMoveIndex/wMoveCount;
    }
    else if(m_JettonStatus[i].wMoveCount>0)
    {
      //筹码处理
      m_lTableScore[i] = m_JettonStatus[i].lGold ;

      //清理信息
      m_lMoveGold[i]=0;
      ZeroMemory(&m_JettonStatus[i],sizeof(m_JettonStatus[i]));
    }
  }

  if(bAllClean)
  {
    m_bJettonAction = false;
  }

  //更新界面
  InvalidGameView(0,0,0,0);

  return !bAllClean;
}

//绘制用户图像
void CGameClientView::DrawUserReadyEx(CDC *pDC,int nXPos,int nYPos)
{
  m_PngReady.DrawImage(pDC,nXPos,nYPos);
}

//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
  //发送处理
  switch(nIDEvent)
  {
    case IDI_JETTON_ACTION: //筹码动作
    {
      bool bKillTime = true;

      //筹码移动
      if(m_bJettonAction && MoveJetton())
      {
        bKillTime = false;
      }

      //筹码移动
      //if(m_bCardAction && MoveCard()) bKillTime = false;

      //删除定时器
      if(bKillTime)
      {
        m_bJettonAction = false;
        //m_bCardAction = false;
        KillTimer(IDI_JETTON_ACTION);
      }

      return;
    }
    case IDI_SEND_CARD:   //移动扑克
    {
      //设置索引
      m_wSendIndex++;

      //设置位置
      tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];
      if(pSendCardItem->wChairID%2!=0)
      {
        m_SendCardPos.x=m_SendStartPos.x+(m_SendEndingPos[pSendCardItem->wChairID].x-m_SendStartPos.x)*m_wSendIndex/m_wSendCount;
      }
      else
      {
        m_SendCardPos.y=m_SendStartPos.y+(m_SendEndingPos[pSendCardItem->wChairID].y-m_SendStartPos.y)*m_wSendIndex/m_wSendCount;
      }

      //停止判断
      if(m_wSendIndex>=m_wSendCount)
      {
        //获取扑克
        BYTE cbCardData[MAX_COUNT];
        WORD wChairID=pSendCardItem->wChairID;
        BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

        //设置扑克
        cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
        m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);

        //删除动画
        m_SendCardItemArray.RemoveAt(0);

        //继续动画
        if(m_SendCardItemArray.GetCount()>0)
        {
          //变量定义
          tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

          //设置位置
          m_SendCardPos.x=m_SendStartPos.x;
          m_SendCardPos.y=m_SendStartPos.y;

          //位移计算
          int nXCount=abs(m_SendEndingPos[pSendCardItem->wChairID].x-m_SendStartPos.x)/SEND_PELS;
          int nYCount=abs(m_SendEndingPos[pSendCardItem->wChairID].y-m_SendStartPos.y)/SEND_PELS;

          //设置次数
          m_wSendIndex=0;
          m_wSendCount=3;

          //播放声音
          CGameClientEngine * pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
          pGameClientEngine->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
        }
        else
        {
          //完成处理
          KillTimer(IDI_SEND_CARD);
          SendEngineMessage(IDM_SEND_CARD_FINISH,0,0);
        }
      }

      //更新界面
      InvalidGameView(0,0,0,0);

      return ;
    }
    case IDI_USER_ACTION:   //用户动作
    {
      //清理数据
      m_bUserAction = false;
      KillTimer(IDI_USER_ACTION);

      //更新界面
      InvalidGameView(0,0,0,0);

      return;
    }
    case IDI_BONUS:
    {
      m_wBonusCartoonIndex++;

      if(m_wBonusCartoonIndex >= BONUS_FPS)
      {
        KillTimer(IDI_BONUS);
        m_wBonusCartoonIndex = 0;
        ZeroMemory(m_bBonus, sizeof(m_bBonus));
        m_bShowBonusCartoon = false;
      }

      //更新界面
      InvalidGameView(0,0,0,0);
      return;
    }
  }

  __super::OnTimer(nIDEvent);
}
//管理员控制
void CGameClientView::OpenAdminWnd()
{
  if(m_pClientControlDlg != NULL)
  {
    if(!m_pClientControlDlg->IsWindowVisible())
    {
      m_pClientControlDlg->ShowWindow(SW_SHOW);
    }
    else
    {
      m_pClientControlDlg->ShowWindow(SW_HIDE);
    }
  }
}

//更新视图
void CGameClientView::RefreshGameView()
{
  CRect rect;
  GetClientRect(&rect);
  InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

  return;
}
//更新视图
void CGameClientView::RefreshGameView(CRect &rect)
{
  InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

  return;
}

//开始彩金动画
void CGameClientView::StartBonusCartoon()
{
  //设置定时器
  SetTimer(IDI_BONUS, 40, NULL);
  m_bShowBonusCartoon = true;

}

//设置动画标志
void CGameClientView::SetBonusFlag(WORD wViewChairID, bool bBonus)
{
  m_bBonus[wViewChairID] = bBonus;
}

// 绘画数字
void CGameClientView::DrawNumber(CDC * pDC, CPngImageEx* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_RIGHT*/)
{
  TCHAR szOutNum[128] = {0};
  _sntprintf(szOutNum,CountArray(szOutNum),TEXT("%I64d"),lOutNum);
  DrawNumber(pDC, ImageNumber, szImageNum, szOutNum, nXPos, nYPos, uFormat);
}

// 绘画数字
void CGameClientView::DrawNumber(CDC * pDC, CPngImageEx* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat /*= DT_RIGHT*/)
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
//////////////////////////////////////////////////////////////////////////
