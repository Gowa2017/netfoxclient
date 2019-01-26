#include "StdAfx.h"
#include "Math.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
#include "gameclient.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FLASH_WINNER      100                 //闪动标识
#define IDI_SHOW_CHANGE_BANKER    101                 //轮换庄家
#define IDI_DISPATCH_CARD     102                 //发牌标识
#define IDI_SHOWDISPATCH_CARD_TIP 103                 //发牌提示
#define IDI_OPENCARD        104                 //发牌提示
#define IDI_MOVECARD_END      105                 //移動牌結束
#define IDI_POSTCARD        106                 //发牌提示
#define IDI_OUT_CARD_TIME     107
#define IDI_JETTON_ANIMATION    108
#define IDI_HANDLELEAVE_ANIMATION 109
#define IDI_OPENBOX_ANIMATION   110
#define IDI_LEAVHANDLESOUND     111
#define IDI_PLEASEJETTONSOUND   112
#define IDI_SPEEKSOUND        113
#define IDI_FLASH_CARD        115                 //闪动标识
#define IDI_FLASH_RAND_SIDE     116                 //闪动标识
#define IDI_SHOW_CARD_RESULT    118
#define IDI_SHOW_END        119                 //显示结束界面

//机器人下注,以下所有定时器索引保留(1000以后),专机器人提供
#define IDI_ANDROID_BET       1000

//按钮标识
#define IDC_JETTON_BUTTON_100   200                 //按钮标识
#define IDC_JETTON_BUTTON_1000    201                 //按钮标识
#define IDC_JETTON_BUTTON_10000   202                 //按钮标识
#define IDC_JETTON_BUTTON_100000  203                 //按钮标识
#define IDC_JETTON_BUTTON_1000000 204                 //按钮标识
#define IDC_JETTON_BUTTON_5000000 205                 //按钮标识
#define IDC_APPY_BANKER       206                 //按钮标识
#define IDC_CANCEL_BANKER     207                 //按钮标识
#define IDC_SCORE_MOVE_L      209                 //按钮标识
#define IDC_SCORE_MOVE_R      210                 //按钮标识
#define IDC_VIEW_CHART        211                 //按钮标识
#define IDC_JETTON_BUTTON_50000     219                 //按钮标识
#define IDC_JETTON_BUTTON_500000  212                 //按钮标识
#define IDC_AUTO_OPEN_CARD      213                 //按钮标识
#define IDC_OPEN_CARD       214                 //按钮标识
#define IDC_BANK          215                 //按钮标识
#define IDC_CONTINUE_CARD     216                 //按钮标识
#define IDC_BANK_STORAGE      217                 //按钮标识
#define IDC_BANK_DRAW       218                 //按钮标识
#define IDC_UP            223                 //按钮标识
#define IDC_DOWN          224                 //按钮标识
#define IDC_RADIO         225                 //按钮标识
#define IDC_ADMIN         226                 //按钮标识



//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
  ON_WM_TIMER()
  ON_WM_CREATE()
  ON_WM_SETCURSOR()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_BN_CLICKED(IDC_ADMIN,OpenAdminWnd)
  ON_BN_CLICKED(IDC_APPY_BANKER, OnApplyBanker)
  ON_BN_CLICKED(IDC_CANCEL_BANKER, OnCancelBanker)
  ON_BN_CLICKED(IDC_SCORE_MOVE_L, OnScoreMoveL)
  ON_BN_CLICKED(IDC_SCORE_MOVE_R, OnScoreMoveR)
  ON_BN_CLICKED(IDC_OPEN_CARD, OnOpenCard)
  ON_BN_CLICKED(IDC_AUTO_OPEN_CARD,OnAutoOpenCard)
  ON_MESSAGE(WM_VIEWLBTUP,OnViLBtUp)
#ifdef  __SPECIAL___
  ON_BN_CLICKED(IDC_BANK_STORAGE, OnBankStorage)
  ON_BN_CLICKED(IDC_BANK_DRAW, OnBankDraw)
#endif

  ON_BN_CLICKED(IDC_UP, OnUp)
  ON_BN_CLICKED(IDC_DOWN, OnDown)

  /*ON_BN_CLICKED(IDC_OPTION, OnButtonGameOption)
  ON_BN_CLICKED(IDC_MIN, OnButtonMin)
  ON_BN_CLICKED(IDC_CLOSE, OnButtonClose)*/
  ON_MESSAGE(WM_SET_CAPTION, OnSetCaption)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView()
{
  //下注信息
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //全体下注
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

  //所有玩家下注
  ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));
  ZeroMemory(m_lUserScoreTotal,sizeof(m_lUserScoreTotal));
  ZeroMemory(m_fRecordRate,sizeof(m_fRecordRate));
  ZeroMemory(m_areAtotalBet,sizeof(m_areAtotalBet));
  //庄家信息
  m_wBankerUser=INVALID_CHAIR;
  m_wBankerTime=0;
  m_lBankerScore=0L;
  m_lBankerWinScore=0L;
  m_lTmpBankerWinScore=0;
  m_Out_Bao_y = 0;
  m_CarIndex = 0;
  m_lRobotMaxJetton = 5000000l;
  m_nShowValleyIndex = 0;


  //当局成绩
  m_lMeCurGameScore=0L;
  m_lMeCurGameReturnScore=0L;
  m_lBankerCurGameScore=0L;
  m_lGameRevenue=0L;

  m_JettonQueIndex = 0;

  m_lLastJetton = 0;

  //状态信息
  m_lCurrentJetton=0L;
  m_cbAreaFlash=0xFF;
  m_wMeChairID=INVALID_CHAIR;
  m_bShowChangeBanker=FALSE;
  m_bNeedSetGameRecord=FALSE;
  m_bWinTianMen=FALSE;
  m_bWinHuangMen=FALSE;
  m_bWinXuanMen=FALSE;
  m_bFlashResult=FALSE;
  m_blMoveFinish = FALSE;
  m_blAutoOpenCard = TRUE;
  m_bShowBao = FALSE;
  m_enDispatchCardTip=enDispatchCardTip_NULL;

  m_lMeCurGameScore=0L;
  m_lMeCurGameReturnScore=0L;
  m_lBankerCurGameScore=0L;
  m_blCanStore=false;

  m_lAreaLimitScore=0L;

  //位置信息
  m_nScoreHead = 0;
  m_nRecordFirst= 0;
  m_nRecordLast= 0;
  m_CheckImagIndex = 0;
  m_ShowImageIndex  = 0;

  //历史成绩
  m_lMeStatisticScore=0;

  //控件变量
  m_pGameClientDlg=NULL;



  m_bEnablePlaceJetton = false;

  m_pClientControlDlg = NULL;
  m_hInst = NULL;

  return;
}

//析构函数
CGameClientView::~CGameClientView(void)
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
//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(__super::OnCreate(lpCreateStruct)==-1)
  {
    return -1;
  }

  AfxSetResourceHandle(AfxGetInstanceHandle());

  m_FontValleysListTwo.CreateFont(this, TEXT("宋体"), 12, 400);
  //加载位图
  HINSTANCE hInstance=AfxGetInstanceHandle();
  m_ImageViewFill.LoadFromResource(this,hInstance,IDB_VIEW_FILL);

  m_ImageWinFlags.LoadImage(this,hInstance,_TEXT("IDB_IDB_FLAG_PNG"));

  m_ImageJettonView.LoadFromResource(this,hInstance,IDB_JETTOM_VIEW);
  m_ImageScoreNumber.LoadFromResource(this,hInstance,IDB_SCORE_NUMBER);
  m_ImageMeScoreNumber.LoadFromResource(this,hInstance,IDB_ME_SCORE_NUMBER);
  m_ViewBackPng.LoadImage(this,hInstance,_TEXT("IDB_VIEW_BACK_PNG"));
  m_idb_selPng.LoadImage(this,hInstance,_TEXT("IDB_IDB_SEL_PNG"));
  m_ImageRunCard.LoadImage(this,hInstance,_TEXT("IDB_RUNNING_CAR"));
  for(int i = 0; i<5; i++)
  {
    m_ImageCardType[i].LoadFromResource(this,hInstance,IDB_CARDTYPE);
  }

  m_ImageGameEnd.LoadFromResource(this, hInstance,IDB_GAME_END);
  m_ImageMeBanker.LoadFromResource(this, hInstance,IDB_ME_BANKER);
  m_ImageChangeBanker.LoadFromResource(this, hInstance,IDB_CHANGE_BANKER);
  m_ImageNoBanker.LoadFromResource(this, hInstance,IDB_NO_BANKER);
  //m_ImageFrameTianMen.LoadFromResource(this,hInstance,IDB_FRAME_TIAN_EMN);
  m_ImageTimeFlagPng.LoadImage(this,hInstance,_TEXT("IDB_TIME_PNG"));
  m_TimerCount_png.LoadImage(this,hInstance,_TEXT("IDB_TIMERCOUNT_PNG"));
  m_ImageMeScoreNumberPng.LoadImage(this,hInstance,_TEXT("IDB_MENUM_PNG"));
  m_ImageScoreNumberPng.LoadImage(this,hInstance,_TEXT("IDB_SCORE_PNG"));
  m_pngGameEnd.LoadImage(this,hInstance,_TEXT("IDB_GAME_END_PNG"));
  m_ImaNumberBack.LoadImage(this,hInstance,_TEXT("NUMBER_BACK"));

  m_PngFrameTianMen.LoadImage(this,hInstance,_TEXT("TIAN_MEM_0"));
  m_PngFrameTianMen1.LoadImage(this,hInstance,_TEXT("TIAN_MEM_1"));
  m_PngFrameTianMen2.LoadImage(this,hInstance,_TEXT("TIAN_MEM_2"));
  m_PngFrameTianMen3.LoadImage(this,hInstance,_TEXT("TIAN_MEM_3"));
  m_PngFrameTianMen4.LoadImage(this,hInstance,_TEXT("TIAN_MEM_4"));
  m_PngFrameTianMen5.LoadImage(this,hInstance,_TEXT("TIAN_MEM_5"));
  m_PngFrameTianMen6.LoadImage(this,hInstance,_TEXT("TIAN_MEM_6"));
  m_PngFrameTianMen7.LoadImage(this,hInstance,_TEXT("TIAN_MEM_7"));

  //for (int i = 0;i<AREA_COUNT;i++) m_sT_ShowInfo.blShow[i]=false;

  TCHAR szBuffer[128]=TEXT("");
  //加载推宝资源
  for(int i = 0; i<7; i++)
  {
    _sntprintf(szBuffer,CountArray(szBuffer),TEXT("IDB_PUSHBOX%d"),i+1);
    m_PngPushBox[i].LoadImage(this,hInstance,szBuffer);
  }

  for(int i = 0; i<4; i++)
  {
    _sntprintf(szBuffer,CountArray(szBuffer),TEXT("IDB_JETTON%d"),i+1);
    m_PngShowJetton[i].LoadImage(this,hInstance,szBuffer);
  }

  for(int i = 0; i<4; i++)
  {
    _sntprintf(szBuffer,CountArray(szBuffer),TEXT("IDB_HANDLELEAVE%d"),i+1);
    m_PngShowLeaveHandle[i].LoadImage(this,hInstance,szBuffer);
  }


  for(int i = 0; i<4; i++)
  {
    _sntprintf(szBuffer,CountArray(szBuffer),TEXT("IDB_RESULT%d"),i+1);
    m_PngResult[i].LoadImage(this,hInstance,szBuffer);
  }

  //创建控件
  CRect rcCreate(0,0,0,0);
  //m_ApplyUser.Create( IDD_DLG_GAME_RECORD , this );

  //下注按钮
  m_btJetton100.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100);
  m_btJetton1000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000);
  m_btJetton10000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_10000);
  m_btJetton50000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_50000);
  m_btJetton100000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100000);

  m_btJetton500000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_500000);
  m_btJetton1000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000000);
  m_btJetton5000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_5000000);

  //申请按钮
  m_btApplyBanker.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_APPY_BANKER);
  m_btCancelBanker.Create(NULL,WS_CHILD|WS_DISABLED,rcCreate,this,IDC_CANCEL_BANKER);

  m_btScoreMoveL.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_L);
  m_btScoreMoveR.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_R);

  m_btAutoOpenCard.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_AUTO_OPEN_CARD);
  m_btOpenCard.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_OPEN_CARD);

  m_btBank.Create(NULL,WS_CHILD,rcCreate,this,IDC_BANK);

  m_btBankerStorage.Create(NULL,WS_CHILD|WS_DISABLED,rcCreate,this,IDC_BANK_STORAGE);
  m_btBankerDraw.Create(NULL,WS_CHILD,rcCreate,this,IDC_BANK_DRAW);
  //m_btBankerStorage.EnableWindow(TRUE);
  //m_btBankerStorage.ShowWindow(SW_SHOW);
  m_btBankerDraw.EnableWindow(TRUE);
  m_btBankerDraw.ShowWindow(SW_SHOW);
  m_btUp.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_UP);
  m_btDown.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_DOWN);



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

  m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false,false);
  m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,hResInstance,false,false);
  m_btBank.SetButtonImage(IDB_BT_BANK,hResInstance,false,false);



  m_btUp.SetButtonImage(IDB_BT_BT_S,hResInstance,false,false);
  m_btDown.SetButtonImage(IDB_BT_BT_X,hResInstance,false,false);

  m_btScoreMoveL.SetButtonImage(IDB_BT_SCORE_MOVE_L,hResInstance,false,false);
  m_btScoreMoveR.SetButtonImage(IDB_BT_SCORE_MOVE_R,hResInstance,false,false);

  m_btAutoOpenCard.SetButtonImage(IDB_BT_AUTO_OPEN_CARD,hResInstance,false,false);
  m_btOpenCard.SetButtonImage(IDB_BT_OPEN_CARD,hResInstance,false,false);

  m_btBank.SetButtonImage(IDB_BT_BANK,hResInstance,false,false);

  m_btBankerStorage.SetButtonImage(IDB_BT_STORAGE,hResInstance,false,false);
  m_btBankerDraw.SetButtonImage(IDB_BT_DRAW,hResInstance,false,false);

  m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(4,4,11,11),this,IDC_ADMIN);
  m_btOpenAdmin.ShowWindow(SW_HIDE);

  SwitchToCheck();
  SetJettonHide(0);

  //控制
  m_hInst = NULL;
  m_pClientControlDlg = NULL;
  m_hInst = LoadLibrary(_TEXT("BumperCarBattleClientControl.dll"));
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


  if(m_pClientControlDlg)
  {
    //m_ctrolListDialog.Create(IDD_CTROL_LIST,this);
    //
    //m_SkinListCtrl.Create(WS_CHILD,  CRect(200,200,600,400), this, 2023);

    //m_SkinListCtrl.ModifyStyle(0,LVS_REPORT);
    ////通过GetWindowLong来获取CListCtrl已有的样式
    //DWORD dwStyle = GetWindowLong(m_SkinListCtrl.m_hWnd, GWL_STYLE);

    ////在原有样式的基本上，添加LVS_REPORT扩展样式
    //SetWindowLong(m_SkinListCtrl.m_hWnd, GWL_STYLE, dwStyle|LVS_REPORT);

    ////获取已有的扩展样式
    //DWORD dwStyles = m_SkinListCtrl.GetExStyle();
    ////取消复选框样式
    //dwStyles &= ~LVS_EX_CHECKBOXES;

    ////添加整行选择和表格线扩展样式
    //m_SkinListCtrl.SetExtendedStyle(dwStyles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
    //m_SkinListCtrl.InsertColumn(0,TEXT("序号"));
    //m_SkinListCtrl.InsertColumn(1,TEXT("玩家昵称"));
    //m_SkinListCtrl.InsertColumn(2,TEXT("玩家ID"));
    //m_SkinListCtrl.InsertColumn(3,TEXT("带入金币"));
    //m_SkinListCtrl.InsertColumn(4,TEXT("区1"));
    //m_SkinListCtrl.InsertColumn(5,TEXT("区2"));
    //      m_SkinListCtrl.InsertColumn(6,TEXT("区3"));
    //m_SkinListCtrl.InsertColumn(7,TEXT("区4"));
    //      m_SkinListCtrl.InsertColumn(8,TEXT("区5"));
    //m_SkinListCtrl.InsertColumn(9,TEXT("区6"));
    //m_SkinListCtrl.InsertColumn(10,TEXT("区7"));
    //m_SkinListCtrl.InsertColumn(11,TEXT("区8"));
    //
    //m_SkinListCtrl.ShowWindow(SW_HIDE);


  }

  return 0;
}

//重置界面
VOID CGameClientView::ResetGameView()
{
  ////下注信息
  //ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  ////全体下注
  //ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

  ////所有玩家下注
  //ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));

  //ZeroMemory(m_fRecordRate,sizeof(m_fRecordRate));

  ZeroMemory(m_areAtotalBet,sizeof(m_areAtotalBet));

  ////庄家信息
  //m_wBankerUser=INVALID_CHAIR;
  //m_wBankerTime=0;
  //m_lBankerScore=0L;
  //m_lBankerWinScore=0L;
  //m_lTmpBankerWinScore=0;
  //m_nShowValleyIndex = 0;

  ////当局成绩
  //m_lMeCurGameScore=0L;
  //m_lMeCurGameReturnScore=0L;
  //m_lBankerCurGameScore=0L;
  //m_lGameRevenue=0L;

  ////状态信息
  //m_lCurrentJetton=0L;
  //m_cbAreaFlash=0xFF;
  //m_wMeChairID=INVALID_CHAIR;
  //m_bShowChangeBanker=false;
  //m_bNeedSetGameRecord=false;
  //m_bWinTianMen=false;
  //m_bWinHuangMen=false;
  //m_bWinXuanMen=false;
  //m_bFlashResult=false;
  //m_bShowGameResult=false;
  //m_enDispatchCardTip=enDispatchCardTip_NULL;

  //m_lMeCurGameScore=0L;
  //m_lMeCurGameReturnScore=0L;
  //m_lBankerCurGameScore=0L;

  //m_lAreaLimitScore=0L;

  ////位置信息
  //m_nScoreHead = 0;
  //m_nRecordFirst= 0;
  //m_nRecordLast= 0;

  ////历史成绩
  //m_lMeStatisticScore=0;

  //清空列表
  //m_ValleysList.RemoveAll();

  //清除桌面
  CleanUserJetton();
  if(m_pClientControlDlg != NULL)
  {
    m_pClientControlDlg->ClearClist();
  }
  ////设置按钮
  //m_btApplyBanker.ShowWindow(SW_SHOW);
  //m_btApplyBanker.EnableWindow(FALSE);
  //m_btCancelBanker.ShowWindow(SW_HIDE);
  //m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);

  return;
}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
  if(nWidth<=0)
  {
    return;
  }

  int nClientWidth = 1000;
  int nUserListWidth = 300;

  ////获取窗口
  //CGameClientApp *pAppWnd = (CGameClientApp*)AfxGetApp();
  //CGameFrameWnd *pGameFrameWnd = pAppWnd->GetFrameWnd();

  ////隐藏控制列表
  //if (pAppWnd!=NULL)
  //  pGameFrameWnd->m_GameFrameControl.ShowWindow(false);

  //CRect rcWindow;
  //AfxGetMainWnd()->GetWindowRect(&rcWindow);

  ////强制大小
  //if (rcWindow.Width()<nClientWidth+nUserListWidth)
  //{
  //  nWidth =rcWindow.Width()-17;
  //  SetWindowPos(NULL,9,29,nWidth,nHeight,NULL);
  //}
  //else
  //{
  //  //显示控制列表
  //  if (pAppWnd!=NULL)
  //    pGameFrameWnd->m_GameFrameControl.ShowWindow(true);
  //}

  //位置信息
  CSize Size;
  Size.cy = Size.cy/2;

  //CImageHandle ImageHandle(&m_ImageViewBack[0]);
  int iWidth =756;// m_ImageViewBack[0].GetWidth();
  int iHeight =705;// m_ImageViewBack[0].GetHeight();

  int LifeWidth = nWidth/2-iWidth/2;
  int TopHeight = nHeight/2-iHeight/2;

  for(int i = 0,  j = 0; i<32; i++)
  {
    if(j ==0)
    {
      if(i==0)
      {
        m_CarRect[i].top =nHeight/2-276-8-57;
        m_CarRect[i].left =nWidth/2-362-17+i*77-5-47;
      }
      else
      {
        m_CarRect[i].left =nWidth/2-362-17+i*77-5-47-14;
        m_CarRect[i].top =nHeight/2-276-8-57-30;
      }

    }
    else
    {
      if(j==1)
      {
        if(i<10)
        {
          m_CarRect[i].left =nWidth/2-362-17+i*77-5-47-14;
          m_CarRect[i].top =nHeight/2-276-8-57-30;
        }
        else
        {
          if(i==10)
          {
            m_CarRect[i].left =  m_CarRect[i-1].left+73;
            m_CarRect[i].top =  m_CarRect[i-1].top+29;
          }
          if(i==11)
          {
            m_CarRect[i].left =  m_CarRect[i-1].left+50;
            m_CarRect[i].top =  m_CarRect[i-1].top+55;
          }
          if(i==12)
          {
            m_CarRect[i].left =  m_CarRect[i-1].left+32;
            m_CarRect[i].top =  m_CarRect[i-1].top+74;
          }
          if(i==13)
          {
            m_CarRect[i].left =  m_CarRect[i-1].left+8;
            m_CarRect[i].top =  m_CarRect[i-1].top+72;
          }
          if(i==14)
          {
            m_CarRect[i].left =  m_CarRect[i-1].left-8;
            m_CarRect[i].top =  m_CarRect[i-1].top+72;
          }
          if(i==15)
          {
            m_CarRect[i].left =  m_CarRect[i-1].left-32;
            m_CarRect[i].top =  m_CarRect[i-1].top+74;
          }
          if(i==16)
          {
            m_CarRect[i].left =  m_CarRect[i-1].left-50;
            m_CarRect[i].top =  m_CarRect[i-1].top+55;
          }
        }


      }
      else
      {
        if(j==2)
        {
          m_CarRect[i].top =nHeight/2-276-8+8*57-84;
          m_CarRect[i].left =nWidth/2-362-17+8*81-5-(i-(2*8))*77+63;

        }
        else
        {
          if(j==3)
          {
            m_CarRect[i].top =nHeight/2-276-8+8*57-(i-24)*57-28;
            m_CarRect[i].left =nWidth/2-362-17+15;

            if(i==26)
            {
              m_CarRect[i].left =  m_CarRect[i-1].left-72;
              m_CarRect[i].top =  m_CarRect[i-1].top-25;
            }
            if(i==27)
            {
              m_CarRect[i].left =  m_CarRect[i-1].left-50;
              m_CarRect[i].top =  m_CarRect[i-1].top-55;
            }
            if(i==28)
            {
              m_CarRect[i].left =  m_CarRect[i-1].left-32;
              m_CarRect[i].top =  m_CarRect[i-1].top-74;
            }
            if(i==29)
            {
              m_CarRect[i].left =  m_CarRect[i-1].left-8;
              m_CarRect[i].top =  m_CarRect[i-1].top-72;
            }
            if(i==30)
            {
              m_CarRect[i].left =  m_CarRect[i-1].left+8;
              m_CarRect[i].top =  m_CarRect[i-1].top-72;
            }
            if(i==31)
            {
              m_CarRect[i].left =  m_CarRect[i-1].left+32;
              m_CarRect[i].top =  m_CarRect[i-1].top-74;
            }
          }
        }
      }
    }

    j = i/8;
  }

  //公路跑车
  for(int i = 0,  j = 0; i<32; i++)
  {
    if(j ==0)
    {
      if(i==0)
      {
        m_RunCardRect[i].top =m_CarRect[i].top +60;
        m_RunCardRect[i].left =m_CarRect[i].left+20 ;
      }
      else
      {
        m_RunCardRect[i].left =m_CarRect[i].left+5 ;
        m_RunCardRect[i].top =m_CarRect[i].top +60 ;

        if(i==1)
        {
          m_RunCardRect[i].top =m_CarRect[i].top +70 ;
        }
      }

    }
    else
    {
      if(j==1)
      {
        if(i<10)
        {
          m_RunCardRect[i].top =m_CarRect[i].top +70;
          m_RunCardRect[i].left =m_CarRect[i].left+5;
        }
        else
        {
          if(i==10)
          {
            m_RunCardRect[i].left =  m_CarRect[i].left-20 ;
            m_RunCardRect[i].top =  m_CarRect[i].top +70 ;
          }
          if(i==11)
          {
            m_RunCardRect[i].left =  m_CarRect[i].left-50;
            m_RunCardRect[i].top =  m_CarRect[i].top+40;
          }
          if(i==12)
          {
            m_RunCardRect[i].left =  m_CarRect[i].left-60;
            m_RunCardRect[i].top =  m_CarRect[i].top+10;
          }
          if(i==13)
          {
            m_RunCardRect[i].left =  m_CarRect[i].left-50;
            m_RunCardRect[i].top =  m_CarRect[i].top;
          }
          if(i==14)
          {
            m_RunCardRect[i].left =  m_CarRect[i].left-50;
            m_RunCardRect[i].top =  m_CarRect[i].top-10;
          }
          if(i==15)
          {
            m_RunCardRect[i].left =  m_CarRect[i].left-40;
            m_RunCardRect[i].top =  m_CarRect[i].top-40;
          }
          if(i==16)
          {
            m_RunCardRect[i].left =  m_CarRect[i].left-20;
            m_RunCardRect[i].top =  m_CarRect[i].top-70;
          }
        }


      }
      else
      {
        if(j==2)
        {
          m_RunCardRect[i].top = m_CarRect[i].top-55;
          m_RunCardRect[i].left =m_CarRect[i].left;

          if(i==17)
          {
            m_RunCardRect[i].top = m_CarRect[i].top-70;
          }

        }
        else
        {
          if(j==3)
          {
            m_RunCardRect[i].top =m_CarRect[i].top-70;
            m_RunCardRect[i].left =m_CarRect[i].left;

            if(i==26)
            {
              m_RunCardRect[i].left =  m_CarRect[i].left+30;
              m_RunCardRect[i].top =  m_CarRect[i].top-70;
            }
            if(i==27)
            {
              m_RunCardRect[i].left =  m_CarRect[i].left+40;
              m_RunCardRect[i].top =  m_CarRect[i].top-70;
            }
            if(i==28)
            {
              m_RunCardRect[i].left =  m_CarRect[i].left+60;
              m_RunCardRect[i].top =  m_CarRect[i].top-40;
            }
            if(i==29)
            {
              m_RunCardRect[i].left =  m_CarRect[i].left+70;
              m_RunCardRect[i].top =  m_CarRect[i].top-10;
            }
            if(i==30)
            {
              m_RunCardRect[i].left =  m_CarRect[i].left+70;
              m_RunCardRect[i].top =  m_CarRect[i].top;
            }
            if(i==31)
            {
              m_RunCardRect[i].left =  m_CarRect[i].left+40;
              m_RunCardRect[i].top =  m_CarRect[i].top+40;
            }
          }
        }
      }
    }

    j = i/8;
  }

  //下注区域
  for(int i = 0; i<8; i++)
  {
    if(i<4)
    {
      m_RectArea[i].top =nHeight/2-224;
      m_RectArea[i].left =nWidth/2-213;

      m_EllipseX = nWidth/2-213;
      m_EllipseY = nHeight/2-224;

      m_RectArea[i].bottom =m_RectArea[i].top+141;


      if(i==1)
      {
        m_RectArea[i].left =nWidth/2-213+156;
      }
      if(i==2)
      {
        m_RectArea[i].left =nWidth/2+86;
      }
      if(i==3)
      {
        m_RectArea[i].left =nWidth/2+86+141;
      }

      m_RectArea[i].right  = m_RectArea[i].left+137;


    }
    else
    {
      m_RectArea[i].top =nHeight/2-276-8+133+162-92;
      m_RectArea[i].left =nWidth/2-362-17+(i-4)*155-5+106-96-8+128+41;
      m_RectArea[i].bottom =m_RectArea[i].top+141;
      m_RectArea[i].right  = m_RectArea[i].left+128;

      if(i==5)
      {
        m_RectArea[i].left =nWidth/2-213+156;
      }
      if(i==6)
      {
        m_RectArea[i].left =nWidth/2+86;
      }
      if(i==7)
      {
        m_RectArea[i].left =nWidth/2+86+141;
      }

      m_RectArea[i].right  = m_RectArea[i].left+137;
    }
  }

  int ExcursionY=10;
  for(int i = 0; i<AREA_COUNT; i++)
  {
    m_PointJettonNumber[i].SetPoint((m_RectArea[i].right+m_RectArea[i].left)/2, (m_RectArea[i].bottom+m_RectArea[i].top)/2-ExcursionY);
    m_PointJetton[i].SetPoint(m_RectArea[i].left, m_RectArea[i].top);

    if(i==0|| i==4)
    {
      m_PointJetton[i].Offset(30,0);
    }

    if(i==3|| i==7)
    {
      m_PointJetton[i].Offset(-20,0);
    }
  }

  m_CardTypePoint[0] =CPoint(nWidth/2+125,nHeight/2-355+135+62);

  //移动控件
  HDWP hDwp=BeginDeferWindowPos(33);
  const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

  //m_ApplyUser.m_viewHandle = m_hWnd;

  //列表控件
  //DeferWindowPos(hDwp,m_ApplyUser,NULL,nWidth/2 + 148-10+50,nHeight/2-291-7-35,178,28,uFlags);
  DeferWindowPos(hDwp,m_btUp,NULL,LifeWidth+245+20+310+85-657,TopHeight+10+10+40+209,18,20,uFlags);
  DeferWindowPos(hDwp,m_btDown,NULL,LifeWidth+245+20+310+85+40-657+95,TopHeight+10+10+40+209,18,20,uFlags);



  m_btBankerStorage.EnableWindow(TRUE);

  m_MeInfoRect.top = TopHeight+10+12;
  m_MeInfoRect.left = LifeWidth+245+20+389;


  m_BaoPosion.top = TopHeight;
  m_BaoPosion.left = nWidth/2;


  //筹码按钮
  CRect rcJetton;
  m_btJetton100.GetWindowRect(&rcJetton);
  int nYPos =TopHeight+40+434+5+16-2+100+20-72;
  int nXPos =  LifeWidth+165 ;
  int nSpace = 15;

  m_TopHeight = TopHeight;
  m_LifeWidth = LifeWidth;

  m_nWinFlagsExcursionX = LifeWidth+152+48+10-15;
  m_nWinFlagsExcursionY = TopHeight+545+7+93-70;

  //nSpace = 15;

  DeferWindowPos(hDwp,m_btJetton100,NULL,nXPos,nYPos,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btJetton1000,NULL,nXPos + nSpace + rcJetton.Width(),nYPos,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btJetton10000,NULL,nXPos + nSpace * 2 + rcJetton.Width() * 2,nYPos,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btJetton100000,NULL,nXPos + nSpace * 3 + rcJetton.Width() * 3,nYPos,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btJetton1000000,NULL,nXPos + nSpace * 4 + rcJetton.Width() * 4,nYPos,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btJetton5000000,NULL,nXPos + nSpace * 5 + rcJetton.Width() *5,nYPos,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btApplyBanker,NULL,m_LifeWidth+609-568-3-31,m_TopHeight+169+35-3+22,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btCancelBanker,NULL,m_LifeWidth+609-568-3-31,m_TopHeight+169+35-3+22,0,0,uFlags|SWP_NOSIZE);


  DeferWindowPos(hDwp,m_btScoreMoveL,NULL,LifeWidth+123+70-88,TopHeight+562+21+86-71,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btScoreMoveR,NULL,LifeWidth+606-63+100,TopHeight+562+21+86-71,0,0,uFlags|SWP_NOSIZE);

  //开牌按钮
  DeferWindowPos(hDwp,m_btAutoOpenCard,NULL,LifeWidth+624+20,TopHeight+198,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btOpenCard,NULL,LifeWidth+624+20,TopHeight+198+30,0,0,uFlags|SWP_NOSIZE);
  //其他按钮
  DeferWindowPos(hDwp,m_btBank,NULL,nWidth/2+290,nHeight/2-340+74,0,0,uFlags|SWP_NOSIZE);
  //其他按钮
  DeferWindowPos(hDwp,m_btBankerStorage,NULL,LifeWidth+10-1+571+109,TopHeight+592+50-17+33-20,104,400,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btBankerDraw,NULL,LifeWidth+10-1+571+109,TopHeight+592-12+33-29,0,0,uFlags|SWP_NOSIZE);
  //结束移动
  EndDeferWindowPos(hDwp);



  if(m_pClientControlDlg)
  {
    /* int topBottom=200;
    CRect rcList;
    rcList.left=250;
    rcList.top=200+topBottom;
    rcList.right=1000;
    rcList.bottom=300+topBottom;
         int widthCol=rcList.right-rcList.left;

    m_SkinListCtrl.MoveWindow(rcList);
    m_SkinListCtrl.SetColumnWidth( 0, widthCol/12 );
    m_SkinListCtrl.SetColumnWidth( 1, widthCol/12 );
    m_SkinListCtrl.SetColumnWidth( 2, widthCol/12 );
    m_SkinListCtrl.SetColumnWidth( 3, widthCol/12 );
    m_SkinListCtrl.SetColumnWidth( 4, widthCol/12 );
    m_SkinListCtrl.SetColumnWidth( 5, widthCol/12 );
    m_SkinListCtrl.SetColumnWidth( 6, widthCol/12 );
    m_SkinListCtrl.SetColumnWidth( 7, widthCol/12 );
    m_SkinListCtrl.SetColumnWidth( 8, widthCol/12 );
    m_SkinListCtrl.SetColumnWidth( 9, widthCol/12 );
    m_SkinListCtrl.SetColumnWidth( 10, widthCol/12 );
    m_SkinListCtrl.SetColumnWidth( 11, widthCol/12 );*/


  }



  return;
}
void CGameClientView::SwitchToCheck()
{
  SwithToNormalView();
  return;

  m_btBank.ShowWindow(SW_SHOW);

  m_btAutoOpenCard.ShowWindow(SW_HIDE);
  m_btOpenCard.ShowWindow(SW_HIDE);
  //m_ApplyUser.ShowWindow(SW_HIDE);
  m_btApplyBanker.ShowWindow(SW_HIDE);
  m_btCancelBanker.ShowWindow(SW_HIDE);
  m_btBank.ShowWindow(SW_HIDE);
  m_btScoreMoveL.ShowWindow(SW_HIDE);
  m_btScoreMoveR.ShowWindow(SW_HIDE);
  m_btJetton100.ShowWindow(SW_HIDE);
  m_btJetton1000.ShowWindow(SW_HIDE);
  m_btJetton10000.ShowWindow(SW_HIDE);
  m_btJetton50000.ShowWindow(SW_HIDE);
  m_btJetton100000.ShowWindow(SW_HIDE);
  m_btJetton500000.ShowWindow(SW_HIDE);
  m_btJetton1000000.ShowWindow(SW_HIDE);
  m_btJetton5000000.ShowWindow(SW_HIDE);
  m_DrawBack = false;
  SwithToNormalView();


}
void CGameClientView::SwithToNormalView()
{

  m_btBank.ShowWindow(SW_HIDE);

  m_btAutoOpenCard.ShowWindow(SW_HIDE);
  m_btOpenCard.ShowWindow(SW_HIDE);
  //m_ApplyUser.ShowWindow(SW_SHOW);
  m_btApplyBanker.ShowWindow(SW_SHOW);
  m_btCancelBanker.ShowWindow(SW_SHOW);
  m_btBank.ShowWindow(SW_HIDE);
  m_btScoreMoveL.ShowWindow(SW_SHOW);
  m_btScoreMoveR.ShowWindow(SW_SHOW);

  m_btJetton100.ShowWindow(SW_SHOW);
  m_btJetton1000.ShowWindow(SW_SHOW);
  m_btJetton10000.ShowWindow(SW_SHOW);
  m_btJetton50000.ShowWindow(SW_HIDE);
  m_btJetton100000.ShowWindow(SW_SHOW);

  m_btJetton500000.ShowWindow(SW_HIDE);
  m_btJetton1000000.ShowWindow(SW_SHOW);
  m_btJetton1000000.ShowWindow(SW_SHOW);
  m_btJetton5000000.ShowWindow(SW_SHOW);


  m_DrawBack = true;

}
//绘画界面
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
  //获取玩家
  IClientUserItem *pClientUserItem = m_wBankerUser==INVALID_CHAIR ? NULL : GetClientUserItem(m_wBankerUser);
  //绘画背景
  //DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);

  CDC* pDCBuff = pDC;

  //////绘画背景
  //for ( int iW = 0 ; iW < nWidth; iW += m_ImageViewFill.GetWidth() )
  //{
  //  for ( int iH = 0;  iH < nHeight; iH += m_ImageViewFill.GetHeight() )
  //  {
  //    m_ImageViewFill.BitBlt(pDC->GetSafeHdc(), iW, iH);
  //  }
  //}
  //m_ImageViewBack.BitBlt( pDC->GetSafeHdc(), nWidth/2 - m_ImageViewBack.GetWidth()/2, nHeight/2 - m_ImageViewBack.GetHeight()/2 );
  DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);

  m_ViewBackPng.DrawImage(pDCBuff,(nWidth-m_ViewBackPng.GetWidth())/2,nHeight/2-m_ViewBackPng.GetHeight()/2,
                          m_ViewBackPng.GetWidth() ,m_ViewBackPng.GetHeight(),0,0,m_ViewBackPng.GetWidth() ,m_ViewBackPng.GetHeight());

  if(m_blRungingCar)
  {
    //for (BYTE m_CarIndex=0;m_CarIndex<32;m_CarIndex++)  //test
    {
      m_idb_selPng.DrawImage(pDCBuff ,m_CarRect[m_CarIndex].left,m_CarRect[m_CarIndex].top,
                             m_idb_selPng.GetWidth()/8 ,m_idb_selPng.GetHeight(),(m_CarIndex%8)*(m_idb_selPng.GetWidth()/8),0,m_idb_selPng.GetWidth()/8 ,m_idb_selPng.GetHeight());


      m_ImageRunCard.DrawImage(pDCBuff ,m_RunCardRect[m_CarIndex].left,m_RunCardRect[m_CarIndex].top,
                               m_ImageRunCard.GetWidth()/32 ,m_ImageRunCard.GetHeight(),(m_CarIndex)*(m_ImageRunCard.GetWidth()/32),0,m_ImageRunCard.GetWidth()/32 ,m_ImageRunCard.GetHeight());
    }
  }
  if(m_bFlashrandShow)
  {
    m_idb_selPng.DrawImage(pDCBuff ,m_CarRect[m_CarIndex].left,m_CarRect[m_CarIndex].top,
                           m_idb_selPng.GetWidth()/8 ,m_idb_selPng.GetHeight(),(m_CarIndex%8)*(m_idb_selPng.GetWidth()/8),0,m_idb_selPng.GetWidth()/8 ,m_idb_selPng.GetHeight());

  }

  //获取状态
  BYTE cbGameStatus=0;
  if(m_pGameClientDlg != NULL)
  {
    cbGameStatus=m_pGameClientDlg->GetGameStatus();
  }

  //状态提示
  CFont static InfoFont;
  InfoFont.CreateFont(-16,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
  CFont * pOldFont=pDC->SelectObject(&InfoFont);
  pDC->SetTextColor(RGB(255,255,0));

  pDC->SelectObject(pOldFont);
  InfoFont.DeleteObject();

  //时间提示
  if(m_DrawBack)
  {

    int nTimeFlagWidth = m_ImageTimeFlagPng.GetWidth()/3;
    int nFlagIndex=0;
    if(cbGameStatus==GAME_STATUS_FREE)
    {
      nFlagIndex=0;
    }
    else if(cbGameStatus==GS_PLACE_JETTON)
    {
      nFlagIndex=1;
    }
    else if(cbGameStatus==GS_GAME_END)
    {
      nFlagIndex=2;
    }

    //m_ImageTimeFlagPng.DrawImage(pDCBuff,nWidth/2-348+215+30, m_TopHeight+566+15+7+20+5-441+7-16, nTimeFlagWidth, m_ImageTimeFlagPng.GetHeight(),
    //  nFlagIndex*nTimeFlagWidth,0);

    WORD wUserTimer =((m_wMeChairID!=INVALID_CHAIR)?GetUserClock(m_wMeChairID):0);

    if(wUserTimer!=0)
    {
      DrawNumberString(pDCBuff,wUserTimer,m_LifeWidth+424-303,m_TopHeight+244,true);
    }

  }
  //CRect rcDispatchCardTips(m_LifeWidth+612, m_TopHeight+297,m_LifeWidth+612+200, m_TopHeight+297+115);

  //胜利边框
  FlashJettonAreaFrame(nWidth,nHeight,pDCBuff);

  //绘画筹码
  IsJettonDraw(pDC);

  for(INT i=0; i<AREA_COUNT&&m_DrawBack; i++)
  {
    //变量定义
    LONGLONG lScoreCount=0L;
    LONGLONG static lScoreJetton[JETTON_COUNT]= {100L,1000L,10000L,100000L,1000000L,5000000L};
    int static nJettonViewIndex=0;

    //绘画筹码
    for(INT_PTR j=0; j<m_JettonInfoArray[i].GetCount(); j++)
    {
      //获取信息
      tagJettonInfo * pJettonInfo=&m_JettonInfoArray[i][j];

      //累计数字
      ASSERT(pJettonInfo->cbJettonIndex<JETTON_COUNT);
      lScoreCount+=lScoreJetton[pJettonInfo->cbJettonIndex];

    }
    //绘画数字
    if(lScoreCount>0L)
    {
      m_ImaNumberBack.DrawImage(pDCBuff,m_PointJettonNumber[i].x-60,m_PointJettonNumber[i].y-13);
      DrawNumberString(pDCBuff,lScoreCount,m_PointJettonNumber[i].x-5,m_PointJettonNumber[i].y);

    }

  }

  //绘画庄家
  if(m_DrawBack)
  {
    DrawBankerInfo(pDCBuff,nWidth,nHeight);

    //绘画用户
    DrawMeInfo(pDCBuff,nWidth,nHeight);
  }


  //切换庄家
  if(m_bShowChangeBanker)
  {
    int nXPos = nWidth / 2 - 130;
    int nYPos = nHeight / 2 - 160+20;

    //由我做庄
    if(m_wMeChairID == m_wBankerUser)
    {
      m_ImageMeBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
    }
    else if(m_wBankerUser != INVALID_CHAIR)
    {
      m_ImageChangeBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
    }
    else
    {
      m_ImageNoBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
    }
  }

  //发牌提示
  m_enDispatchCardTip = enDispatchCardTip_NULL;//暂时屏蔽
  if(enDispatchCardTip_NULL!=m_enDispatchCardTip)
  {
    if(m_ImageDispatchCardTip.IsNull()==false)
    {
      m_ImageDispatchCardTip.Destroy();
    }
    if(enDispatchCardTip_Dispatch==m_enDispatchCardTip)
    {
      m_ImageDispatchCardTip.LoadFromResource(this,AfxGetInstanceHandle(),IDB_DISPATCH_CARD);
    }
    else
    {
      m_ImageDispatchCardTip.LoadFromResource(this,AfxGetInstanceHandle(),IDB_CONTINUE_CARD);
    }
    m_ImageDispatchCardTip.BitBlt(pDC->GetSafeHdc(), (nWidth-m_ImageDispatchCardTip.GetWidth())/2, nHeight/2);
  }

  //我的下注
  DrawMeJettonNumber(pDCBuff);
  //胜利标志
  DrawWinFlags(pDCBuff);

  if(cbGameStatus != GAME_STATUS_FREE)
  {
    m_Out_Bao_y = 6;
  }
  if(m_blMoveFinish&&cbGameStatus==GS_GAME_END)
  {
    ShowGameResult(pDCBuff, nWidth, nHeight);
  }
  if(m_DrawBack)
  {
    DrawMoveInfo(pDCBuff,m_rcTianMen);
  }

  //绘画界面
  if(m_bShowBao&&0)
  {
    if(m_bEnableSysBanker==0&&pClientUserItem==NULL)
      ;
    else
    {
      if(0<=m_Out_Bao_y&&m_Out_Bao_y<7)
      {
        if(m_Out_Bao_y>=6)
        {
          m_PngPushBox[m_Out_Bao_y].DrawImage(pDCBuff,m_BaoPosion.left-m_PngPushBox[m_Out_Bao_y].GetWidth()/2 ,m_BaoPosion.top-120+6*45-50);

        }
        else
        {
          m_PngPushBox[m_Out_Bao_y].DrawImage(pDCBuff,m_BaoPosion.left-m_PngPushBox[m_Out_Bao_y].GetWidth()/2 ,m_BaoPosion.top-120+m_Out_Bao_y*20-30);
        }
      }

    }
  }
  if(m_bShowJettonAn&&cbGameStatus!=GAME_STATUS_FREE&&0)
  {
    if(0<=m_bShowJettonIndex&&m_bShowJettonIndex<4)
    {
      m_PngShowJetton[m_bShowJettonIndex].DrawImage(pDCBuff,m_BaoPosion.left-m_PngShowJetton[m_bShowJettonIndex].GetWidth()/2 ,m_BaoPosion.top-120+6*20-30);
    }
  }
  if(m_bShowLeaveHandleAn&&cbGameStatus==GS_GAME_END&&0)
  {
    if(0<=m_bShowLeaveHandleIndex&&m_bShowLeaveHandleIndex<4)
    {
      m_PngShowLeaveHandle[m_bShowLeaveHandleIndex].DrawImage(pDCBuff,m_BaoPosion.left-m_PngShowLeaveHandle[m_bShowLeaveHandleIndex].GetWidth()/2 ,m_BaoPosion.top-120+6*20-30);
    }

  }

  DrawBankerList(pDC,nWidth,nHeight);
  CRect rcCaption(5,7,1024,30);
  m_FontValleysListTwo.DrawText(pDC, m_strCaption, rcCaption, RGB(255,255,255), DT_VCENTER|DT_LEFT);
  return;
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
  if(lCurrentJetton==0)
  {
    SetJettonHide(0);
  }
  return;
}
//设置标题
LRESULT CGameClientView::OnSetCaption(WPARAM wParam, LPARAM lParam)
{
  m_strCaption = (LPCTSTR)lParam;
  CRect rcCaption(5,0,1024,30);
  CDC* pDC = GetDC();

  return 0;
}

//历史记录
void CGameClientView::SetGameHistory(BYTE *bcResulte)
{
  //设置数据
  BYTE bcResulteTmp[AREA_COUNT];
  memcpy(bcResulteTmp,bcResulte,AREA_COUNT);
  tagClientGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];


  for(int i = 1; i<=AREA_COUNT; i++)
  {

    if(bcResulteTmp[i-1]>0)
    {
      GameRecord.enOperateMen[i]=enOperateResult_Win;
      m_fRecordRate[i-1]++;

    }
    else
    {
      GameRecord.enOperateMen[i]=enOperateResult_Lost;

    }
    /*if (0==m_lUserJettonScore[i]) GameRecord.enOperateMen[i]=enOperateResult_NULL;
    else if (m_lUserJettonScore[i] > 0 && (bcResulte[i-1]==4)) GameRecord.enOperateMen[i]=enOperateResult_Win;
    else if (m_lUserJettonScore[i] > 0 && (0==bcResulte[i-1])) GameRecord.enOperateMen[i]=enOperateResult_Lost;*/

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
  if(MAX_FALG_COUNT < nHistoryCount)
  {
    m_btScoreMoveR.EnableWindow(TRUE);
  }



  //移到最新记录
  if(MAX_FALG_COUNT < nHistoryCount)
  {
    m_nScoreHead = (m_nRecordLast - MAX_FALG_COUNT + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
    m_btScoreMoveL.EnableWindow(TRUE);
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

  //所有玩家下注
  ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));

  /*  if ( m_SkinListCtrl.GetSafeHwnd() )
      m_SkinListCtrl.DeleteAllItems();
  */


  //下注信息
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //全体下注
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

  //所有玩家下注
  ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));


  ZeroMemory(m_JettonQue,sizeof(m_JettonQue));


  ZeroMemory(m_JettonQueArea,sizeof(m_JettonQueArea));




  m_JettonQueIndex = 0;





  //更新界面
  RefreshGameView();

  return;
}

//个人下注
void CGameClientView::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
  //效验参数
  ASSERT(cbViewIndex<=AREA_COUNT);
  if(cbViewIndex>AREA_COUNT)
  {
    return;
  }

  m_lUserJettonScore[cbViewIndex]=lJettonCount;

  bool blHave = false;
  for(int i = 0; i<18; i++)
  {
    if(m_JettonQueArea[i]==cbViewIndex&&m_JettonQueIndex>i)
    {
      m_JettonQue[i] = m_lUserJettonScore[cbViewIndex];

      blHave= true;
      break;

    }
  }
  if(blHave==false)
  {
    if(m_JettonQueIndex>COUNT_DRAWINFO-1)
    {
      for(int i = 0; i<COUNT_DRAWINFO-1; i++)
      {
        m_JettonQueArea[i]= m_JettonQueArea[i+1];
        m_JettonQue[i] = m_JettonQue[i+1];

      }
      m_JettonQue[COUNT_DRAWINFO-1]=m_lUserJettonScore[cbViewIndex];
      m_JettonQueArea[COUNT_DRAWINFO-1] = cbViewIndex;
    }
    else
    {
      m_JettonQueArea[m_JettonQueIndex]= cbViewIndex;
      m_JettonQue[m_JettonQueIndex++] = m_lUserJettonScore[cbViewIndex];

    }

    //m_JettonQueIndex = m_JettonQueIndex%16;

  }





  //更新界面
  RefreshGameView();
}

//设置扑克
void CGameClientView::SetCardInfo(BYTE cbTableCardArray[1][1])
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
  ASSERT(cbViewIndex<=AREA_COUNT);
  if(cbViewIndex>AREA_COUNT)
  {
    return;
  }

  //变量定义
  bool bPlaceJetton = false;
  LONGLONG lScoreIndex[JETTON_COUNT] = {100L,1000L,10000L,100000L,1000000L,5000000L};

  //边框宽度
  int nFrameWidth=0, nFrameHeight=0;
  int nBorderWidth=6;

  m_lAllJettonScore[cbViewIndex] += lScoreCount;
  nFrameWidth = m_RectArea [cbViewIndex-1].right-m_RectArea [cbViewIndex-1].left;
  nFrameHeight = m_RectArea[cbViewIndex-1].bottom-m_RectArea [cbViewIndex-1].top;


  //增加判断
  bool bAddJetton = lScoreCount>0?true:false;

  if(lScoreCount < 0)
  {
    lScoreCount = -lScoreCount;
  }

  //增加筹码
  for(BYTE i=0; i<CountArray(lScoreIndex); i++)
  {
    //计算数目
    BYTE cbScoreIndex = JETTON_COUNT-i-1;
    LONGLONG lCellCount = lScoreCount / lScoreIndex[cbScoreIndex];

    //插入过虑
    if(lCellCount==0L)
    {
      continue;
    }

    //加入筹码
    for(LONGLONG j=0; j<lCellCount; j++)
    {
      if(true == bAddJetton)
      {
        //构造变量
        tagJettonInfo JettonInfo;
        int nJettonSize=55;
        JettonInfo.cbJettonIndex=cbScoreIndex;
        int iWSize = nFrameWidth-nJettonSize-5;
        int iHSize =  nFrameHeight-nJettonSize-40;

        if(cbViewIndex-1 == 0)
        {
          JettonInfo.nXPos=rand()%(nFrameWidth - nJettonSize - 30);
          JettonInfo.nYPos=rand()%(nFrameHeight - nJettonSize - 20);
        }
        else if(cbViewIndex-1 == 4)
        {
          JettonInfo.nXPos=rand()%(nFrameWidth - nJettonSize - 30);
          JettonInfo.nYPos=rand()%(nFrameHeight - nJettonSize - 30);
        }
        else if(cbViewIndex-1 == 3)
        {
          JettonInfo.nXPos=rand()%(nFrameWidth - nJettonSize -30);
          JettonInfo.nYPos=rand()%(nFrameHeight - nJettonSize -20);
        }
        else if(cbViewIndex-1 == 7)
        {
          JettonInfo.nXPos=rand()%(nFrameWidth - nJettonSize - 30);
          JettonInfo.nYPos=rand()%(nFrameHeight - nJettonSize -30);
        }
        else
        {
          JettonInfo.nXPos=rand()%(nFrameWidth - nJettonSize);
          JettonInfo.nYPos=rand()%(nFrameHeight - nJettonSize);
        }
        //插入数组
        bPlaceJetton = true;
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
    lScoreCount -= lCellCount*lScoreIndex[cbScoreIndex];
  }

  //更新界面
  if(bPlaceJetton==true)
  {
    RefreshGameView();
  }

  return;
}

//机器人下注
void CGameClientView::AndroidBet(BYTE cbViewIndex, LONGLONG lScoreCount)
{
  //效验参数
  ASSERT(cbViewIndex<=AREA_COUNT);
  if(cbViewIndex>AREA_COUNT)
  {
    return;
  }

  if(lScoreCount <= 0L)
  {
    return;
  }

  tagAndroidBet Androi;
  Androi.cbJettonArea = cbViewIndex;
  Androi.lJettonScore = lScoreCount;
  m_ArrayAndroid.Add(Androi);
  SetTimer(IDI_ANDROID_BET,100,NULL);
  int nHaveCount = 0;
  for(int i = 0 ; i < m_ArrayAndroid.GetCount(); ++i)
  {
    if(m_ArrayAndroid[i].lJettonScore > 0)
    {
      nHaveCount++;
    }
  }
  UINT nElapse = 0;
  if(nHaveCount <= 1)
  {
    nElapse = 260;
  }
  else if(nHaveCount <= 2)
  {
    nElapse = 160;
  }
  else
  {
    nElapse = 100;
  }

  SetTimer(IDI_ANDROID_BET+m_ArrayAndroid.GetCount(),nElapse,NULL);
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
void CGameClientView::SetWinnerSide(bool blWin[], bool bSet)
{
  //设置时间
  for(int i= 0; i<AREA_COUNT; i++)
  {
    m_bWinFlag[i]=blWin[i];
  }
  if(true==bSet)
  {
    //设置定时器
    SetTimer(IDI_FLASH_WINNER,500,NULL);

    //全胜判断
    bool blWinAll = true;

    for(int i= 0; i<AREA_COUNT; i++)
    {
      if(m_bWinFlag[i]==true)
      {
        blWinAll = false;
      }
    }
    if(blWinAll)
    {
      //重设资源
      HINSTANCE hInstance=AfxGetInstanceHandle();
    }
  }
  else
  {
    //清楚定时器
    KillTimer(IDI_FLASH_WINNER);

    //全胜判断
    bool blWinAll = true;

    for(int i= 0; i<AREA_COUNT; i++)
    {

      if(m_bWinFlag[i]==true)
      {
        blWinAll = false;
      }
    }
    if(blWinAll)
    {
    }
  }

  //设置变量
  m_bFlashResult=bSet;
  m_cbAreaFlash=0xFF;

  if(!bSet)
  {
    m_bShowGameResult=bSet;
  }
  else
  {
    SetTimer(IDI_SHOW_END, 2000, NULL);
  }

  //更新界面
  //RefreshGameView();

  return;
}

//获取区域
BYTE CGameClientView::GetJettonArea(CPoint MousePoint)
{
  for(int i = 0; i<AREA_COUNT; i++)
  {
    if(m_RectArea[i].PtInRect(MousePoint))
    {
      return i+1;
    }
  }
  return 0xFF;
}
//绘画数字
void CGameClientView::DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool blTimer,bool bMeScore)
{
  //加载资源
  CSize SizeScoreNumber(m_ImageScoreNumberPng.GetWidth()/11,m_ImageScoreNumberPng.GetHeight());

  if(bMeScore)
  {
    SizeScoreNumber.SetSize(m_ImageMeScoreNumberPng.GetWidth()/11, m_ImageMeScoreNumberPng.GetHeight());
  }

  if(blTimer)
  {
    SizeScoreNumber.SetSize(m_TimerCount_png.GetWidth()/10, m_TimerCount_png.GetHeight());
  }

  //计算数目
  LONGLONG lNumberCount=0;
  LONGLONG lNumberTemp=lNumber;
  do
  {
    lNumberCount++;
    lNumberTemp/=10;
  }
  while(lNumberTemp>0);

  LONGLONG tmpNum= lNumberCount+lNumberCount/4;

  //位置定义
  INT nYDrawPos=nYPos-SizeScoreNumber.cy/2;
  INT nXDrawPos=(INT)(nXPos+tmpNum*SizeScoreNumber.cx/2-SizeScoreNumber.cx);

  if(!blTimer)
  {
    nXDrawPos = (INT)(nXPos+(SizeScoreNumber.cx-2)*lNumberCount/2+(SizeScoreNumber.cx-5)*(lNumberCount>3?0:(lNumberCount-1)/3)/2);
  }

  //绘画桌号
  for(LONGLONG i=0; i<lNumberCount; i++)
  {
    //绘画号码
    if(i!=0&&i%3==0)
    {
      if(blTimer)
      {
      }
      else
      {
        nXDrawPos += 5;
        if(bMeScore)
        {
          m_ImageMeScoreNumberPng.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
                                            10*SizeScoreNumber.cx,0);
        }
        else
        {
          m_ImageScoreNumberPng.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
                                          10*SizeScoreNumber.cx,0);
        }
        nXDrawPos-=(SizeScoreNumber.cx-2);
      }
    }
    LONG lCellNumber=(LONG)(lNumber%10);
    if(blTimer)
    {
      m_TimerCount_png.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
                                 lCellNumber*SizeScoreNumber.cx,0);

    }
    else
    {
      if(bMeScore)
      {
        m_ImageMeScoreNumberPng.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
                                          lCellNumber*SizeScoreNumber.cx,0);
      }
      else
      {
        m_ImageScoreNumberPng.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
                                        lCellNumber*SizeScoreNumber.cx,0);
      }

    }
    //设置变量
    lNumber/=10;

    if(blTimer)
    {
      nXDrawPos-=SizeScoreNumber.cx;
    }
    else
    {
      nXDrawPos-=(SizeScoreNumber.cx-2);
    }
  };

  return;
}

//绘画数字
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos)
{
  LONGLONG lTmpNumber = lNumber;
  CString strNumber;
  CString strTmpNumber1;
  CString strTmpNumber2;
  bool blfirst = true;
  bool bLongNum = false;
  int nNumberCount = 0;

  strTmpNumber1.Empty();
  strTmpNumber2.Empty();
  strNumber.Empty();

  if(lNumber == 0)
  {
    strNumber=TEXT("0");
  }

  if(lNumber < 0)
  {
    lNumber =- lNumber;
  }

  if(lNumber >= 100)
  {
    bLongNum = true;
  }

  while(lNumber > 0)
  {
    strTmpNumber1.Format(TEXT("%I64d"),lNumber%10);
    nNumberCount++;
    strTmpNumber2 = strTmpNumber1+strTmpNumber2;

    if(nNumberCount == 3)
    {
      if(blfirst)
      {
        strTmpNumber2 += (TEXT("") +strNumber);
        blfirst = false;
      }
      else
      {
        strTmpNumber2 += (TEXT(",") +strNumber);
      }

      strNumber = strTmpNumber2;
      nNumberCount = 0;
      strTmpNumber2 = TEXT("");
    }
    lNumber /= 10;
  }

  if(strTmpNumber2.IsEmpty() == FALSE)
  {
    if(bLongNum)
    {
      strTmpNumber2 += (TEXT(",") +strNumber);
    }
    else
    {
      strTmpNumber2 += strNumber;
    }

    strNumber = strTmpNumber2;
  }

  if(lTmpNumber < 0)
  {
    strNumber = TEXT("-") + strNumber;
  }
  //输出数字
  TextOut(pDC,nXPos,nYPos,strNumber,lstrlen(strNumber));
}
//绘画数字
CString CGameClientView::NumberStringWithSpace(LONGLONG lNumber)
{

  CString static strNumber=TEXT(""), strTmpNumber1,strTmpNumber2;
  strTmpNumber1.Empty();
  strTmpNumber2.Empty();
  strNumber.Empty();
  if(lNumber==0)
  {
    strNumber=TEXT("0");
  }
  int nNumberCount=0;
  LONGLONG lTmpNumber=lNumber;
  if(lNumber<0)
  {
    lNumber=-lNumber;
  }
  bool blfirst = true;
  while(lNumber>0)
  {
    strTmpNumber1.Format(TEXT("%ld"),lNumber%10);
    nNumberCount++;
    strTmpNumber2 = strTmpNumber1+strTmpNumber2;

    if(nNumberCount==3)
    {
      if(blfirst)
      {
        strTmpNumber2 +=strNumber;
        blfirst = false;

      }
      else
      {
        strTmpNumber2 += (TEXT(",") +strNumber);
      }

      strNumber=strTmpNumber2;
      nNumberCount=0;
      strTmpNumber2=TEXT("");
    }
    lNumber/=10;
  }

  if(strTmpNumber2.IsEmpty()==FALSE)
  {
    if(blfirst==false)
    {
      strTmpNumber2 += (TEXT(",") +strNumber);
    }
    else
    {
      strTmpNumber2 += strNumber;
    }

    strNumber=strTmpNumber2;
  }

  if(lTmpNumber<0)
  {
    strNumber=TEXT("-")+strNumber;
  }

  return  strNumber;


}
//筹码检测
void CGameClientView::IsJettonDraw(CDC *pDC)
{
  int nTime = GetTickCount();

  //变量定义
  CRgn JettonRegion;        //显示区域
  CRgn SignedRegion;        //筹码区域
  CRgn SignedAndJettonRegion;   //并集区域
  SignedAndJettonRegion.CreateRectRgn(0,0,0,0);

  //图片区域
  CSize static SizeJettonItem(m_ImageJettonView.GetWidth()/6,m_ImageJettonView.GetHeight());

  //设置区域
  for(INT i = 0; i < AREA_COUNT; i++)
  {
    for(INT_PTR j = m_JettonInfoArray[i].GetCount() - 1; j >= 0; j--)
    {
      //获取信息
      tagJettonInfo * pJettonInfo = &m_JettonInfoArray[i][j];
      pJettonInfo->bShow = false;
      CRect rcRect(pJettonInfo->nXPos+m_PointJetton[i].x,
                   pJettonInfo->nYPos+m_PointJetton[i].y +16,
                   pJettonInfo->nXPos+m_PointJetton[i].x + m_ImageJettonView.GetWidth()/6,
                   pJettonInfo->nYPos+m_PointJetton[i].y +16 + m_ImageJettonView.GetHeight());

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

    //删除显示区域
    if(JettonRegion.GetSafeHandle())
    {
      JettonRegion.DeleteObject();
    }
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

  //绘画筹码
  int iJettonCount=0;
  for(INT i=0; i<AREA_COUNT; i++)
  {
    for(INT_PTR j=0; j<m_JettonInfoArray[i].GetCount(); j++)
    {
      //获取信息
      tagJettonInfo * pJettonInfo=&m_JettonInfoArray[i][j];
      if(!pJettonInfo->bShow)
      {
        continue;
      }

      iJettonCount++;

      int OriginPosX = pJettonInfo->nXPos+m_PointJetton[i].x;
      int OriginPosY = pJettonInfo->nYPos+m_PointJetton[i].y;

      //绘画界面
      if(0 == i)
      {
        OriginPosY += 20;
      }
      else if(3 == i)
      {
        OriginPosY += 20;
        OriginPosX += 20;
      }
      else if(7 == i)
      {
        OriginPosX += 20;
      }
      m_ImageJettonView.TransDrawImage(pDC, OriginPosX,
                                       OriginPosY,SizeJettonItem.cx,SizeJettonItem.cy,pJettonInfo->cbJettonIndex*SizeJettonItem.cx,0,RGB(255,0,255));


    }
  }

//#ifdef _DEBUG
  ////绘画数字
  //DrawNumberString(pDC,iJettonCount,50,500);
  //DrawNumberString(pDC,GetTickCount() - nTime,50,550);
//#endif


  ////设置区域
  //JettonRegion.DeleteObject();
  //SignedRegionOne.DeleteObject();
}
//绘画数字
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, INT nFormat)
{
  LONGLONG lTmpNumber = lNumber;
  CString strNumber;
  CString strTmpNumber1;
  CString strTmpNumber2;
  bool blfirst = true;
  bool bLongNum = false;
  int nNumberCount = 0;

  strTmpNumber1.Empty();
  strTmpNumber2.Empty();
  strNumber.Empty();

  if(lNumber == 0)
  {
    strNumber=TEXT("0");
  }

  if(lNumber < 0)
  {
    lNumber =- lNumber;
  }

  if(lNumber >= 100)
  {
    bLongNum = true;
  }

  while(lNumber > 0)
  {
    strTmpNumber1.Format(TEXT("%I64d"),lNumber%10);
    nNumberCount++;
    strTmpNumber2 = strTmpNumber1+strTmpNumber2;

    if(nNumberCount == 3)
    {
      if(blfirst)
      {
        strTmpNumber2 += (TEXT("") +strNumber);
        blfirst = false;
      }
      else
      {
        strTmpNumber2 += (TEXT(",") +strNumber);
      }

      strNumber = strTmpNumber2;
      nNumberCount = 0;
      strTmpNumber2 = TEXT("");
    }
    lNumber /= 10;
  }

  if(strTmpNumber2.IsEmpty() == FALSE)
  {
    if(bLongNum)
    {
      strTmpNumber2 += (TEXT(",") +strNumber);
    }
    else
    {
      strTmpNumber2 += strNumber;
    }

    strNumber = strTmpNumber2;
  }

  if(lTmpNumber < 0)
  {
    strNumber = TEXT("-") + strNumber;
  }
  //输出数字
  //if (nFormat==-1) pDC->DrawText(strNumber,rcRect,DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
  //else pDC->DrawText(strNumber,rcRect,nFormat);

  //输出数字
  if(nFormat==-1)
  {
    CDFontEx::DrawText(this,pDC,  12, 400, strNumber, rcRect, RGB(255,255,255), DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
  }
  else
  {
    CDFontEx::DrawText(this,pDC,  12, 400, strNumber, rcRect, RGB(255,255,255), nFormat);
  }
}
void CGameClientView::SetMoveCardTimer()
{
}
void CGameClientView::KillCardTime()
{
  KillTimer(IDI_FLASH_WINNER);
  KillTimer(IDI_DISPATCH_CARD);
  KillTimer(IDI_SHOWDISPATCH_CARD_TIP);

}
void CGameClientView::StartRunCar(int iTimer)
{
  m_bFlashrandShow = false;
  KillTimer(IDI_FLASH_RAND_SIDE);
  KillTimer(IDI_FLASH_CARD);
  KillTimer(IDI_SHOW_CARD_RESULT);
  iTimerStep = 400;
  SetTimer(IDI_FLASH_CARD,iTimer,NULL);
  ifirstTimer = 0;
  iOpenSide = m_cbTableCardArray[0][0];
  iTotoalRun = iOpenSide+32*3-2;
  iRunIndex = 0;
  m_CarIndex = 1;
  m_blRungingCar = true;
  SetTimer(IDI_SHOW_CARD_RESULT,20*1000,NULL);


}
void CGameClientView::RuningCar(int iTimer)
{
  if(iRunIndex<10)
  {
    iTimerStep-=43;

  }
  if(iRunIndex >= iTotoalRun-15)
  {
    iTimerStep+=47 /*8*(iRunIndex-iTotoalRun+15)*/;
  }
  if(iRunIndex==iTotoalRun)
  {
    KillTimer(IDI_FLASH_CARD);
    KillTimer(IDI_SHOW_CARD_RESULT);
    m_blRungingCar = false;
    m_blMoveFinish = true;
    //设置定时器
    this->DispatchCard();
    FinishDispatchCard();
    //AfxMessageBox(TEXT("RuningCar"));

    SendEngineMessage(IDM_SOUND,3,3);
    return ;

  }
  if(iTimerStep<0)
  {
    return ;
  }
  KillTimer(IDI_FLASH_CARD);
  SetTimer(IDI_FLASH_CARD,iTimer,NULL);

}
void CGameClientView::FlashWinOpenSide()
{

}
void CGameClientView::StartRandShowSide()
{
  KillTimer(IDI_FLASH_RAND_SIDE);
  iTimerStep = 100;
  m_bFlashrandShow = true;
  SetTimer(IDI_FLASH_RAND_SIDE,iTimerStep,NULL);


}
//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
  if(IDI_FLASH_CARD == nIDEvent)
  {
    m_CarIndex = (m_CarIndex+1)%32;
    iRunIndex++;
    RuningCar(iTimerStep);


    //更新界面
    RefreshGameView();
  }
  if(IDI_SHOW_CARD_RESULT==nIDEvent)
  {
    for(int i = iRunIndex; i<iTotoalRun; i++)
    {
      m_CarIndex = (m_CarIndex+1)%32;
      iRunIndex++;
      if(iRunIndex==iTotoalRun)
      {
        KillTimer(IDI_FLASH_CARD);
        KillTimer(IDI_SHOW_CARD_RESULT);
        m_blRungingCar = false;
        m_blMoveFinish = true;
        //设置定时器
        this->DispatchCard();
        FinishDispatchCard();
        //AfxMessageBox(TEXT("IDI_SHOW_CARD_RESULT"));
        return ;
      }
    }
  }

  if(IDI_FLASH_RAND_SIDE ==nIDEvent)
  {
    m_bFlashrandShow=!m_bFlashrandShow;
    m_CarIndex = rand()%32;
    //更新界面
    RefreshGameView();
  }
  //闪动胜方
  if(nIDEvent==IDI_FLASH_WINNER)
  {
    //设置变量
    m_bFlashResult=!m_bFlashResult;

    //更新界面
    RefreshGameView();
    return;
  }
  //轮换庄家
  else if(nIDEvent == IDI_SHOW_CHANGE_BANKER)
  {
    ShowChangeBanker(false);
    return;
  }
  else if(IDI_SHOWDISPATCH_CARD_TIP==nIDEvent)
  {
    SetDispatchCardTip(enDispatchCardTip_NULL);
  }
  else if(nIDEvent == IDI_ANDROID_BET)
  {
    //更新界面
    RefreshGameView();
    return;
  }
  else if(nIDEvent >= (UINT)(IDI_ANDROID_BET + 1) && nIDEvent < (UINT)(IDI_ANDROID_BET + m_ArrayAndroid.GetCount() + 1))
  {
    INT_PTR Index = nIDEvent - IDI_ANDROID_BET - 1;
    if(Index < 0 || Index >= m_ArrayAndroid.GetCount())
    {
      ASSERT(FALSE);
      KillTimer(nIDEvent);
      return;
    }

    if(m_ArrayAndroid[Index].lJettonScore > 0)
    {
      LONGLONG lScoreIndex[] = {5000000L,1000000L,100000L,10000L,1000L,100L};
      BYTE cbViewIndex = m_ArrayAndroid[Index].cbJettonArea;

      //增加筹码
      for(BYTE i=0; i<CountArray(lScoreIndex); i++)
      {
        if(lScoreIndex[i] > m_lRobotMaxJetton)
        {
          continue;
        }

        if(m_ArrayAndroid[Index].lJettonScore >= lScoreIndex[i])
        {
          m_ArrayAndroid[Index].lJettonScore -= lScoreIndex[i];
          m_lAllJettonScore[cbViewIndex] += lScoreIndex[i];

          tagJettonInfo JettonInfo;
          int iWSize = m_RectArea[cbViewIndex-1].right - m_RectArea[cbViewIndex-1].left - 60;
          int iHSize = m_RectArea[cbViewIndex-1].bottom - m_RectArea[cbViewIndex-1].top - 95;
          JettonInfo.nXPos=rand()%(iWSize);
          JettonInfo.nYPos=rand()%(iHSize);
          JettonInfo.cbJettonIndex = JETTON_COUNT - i - 1;

          //插入数组
          m_JettonInfoArray[cbViewIndex-1].Add(JettonInfo);
          //播放声音
          PostEngineMessage(IDM_SOUND,7,7);
          break;
        }
      }
    }
    return;
  }
  else if(IDI_SHOW_END == nIDEvent)
  {
    KillTimer(nIDEvent);

    BYTE cbGameStatus=0;
    if(m_pGameClientDlg != NULL)
    {
      cbGameStatus = m_pGameClientDlg->GetGameStatus();

      if(cbGameStatus == GS_GAME_END)
      {
        m_bShowGameResult=true;
      }
    }
  }

  __super::OnTimer(nIDEvent);
}

//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
  LPARAM lParam = MAKEWPARAM(Point.x,Point.y);
  AfxGetMainWnd()->PostMessage(WM_LBUTTONDOWN,0,lParam);

  if(m_lCurrentJetton!=0L)
  {
    int iTimer = 1;
    //下注区域
    BYTE cbJettonArea=GetJettonArea(Point);

    //最大下注
    LONGLONG lMaxJettonScore=GetUserMaxJetton(cbJettonArea);

    if((m_lAllJettonScore[cbJettonArea]+m_lCurrentJetton)>m_lAreaLimitScore)
    {
      return ;
    }

    //合法判断
    if(lMaxJettonScore < m_lCurrentJetton)
    {
      SetJettonHide(0);
      return ;
    }

    //发送消息
    if(cbJettonArea!=0xFF)
    {
      SendEngineMessage(IDM_PLACE_JETTON,cbJettonArea,(LPARAM)(&m_lCurrentJetton));
    }
  }
  RefreshGameView();

  __super::OnLButtonDown(nFlags,Point);


}
void CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
  __super::OnLButtonUp(nFlags,Point);
}
//接受其他控件传来的消息
LRESULT CGameClientView::OnViLBtUp(WPARAM wParam, LPARAM lParam)
{
  CPoint *pPoint = (CPoint*)lParam;

  ScreenToClient(pPoint);
  OnLButtonUp(1,*pPoint);
  return 1;
}
void CGameClientView::SetEnablePlaceJetton(bool bEnablePlaceJetton)
{
  m_bEnablePlaceJetton = bEnablePlaceJetton;
}
void CGameClientView::UpdataJettonButton()
{
  if(m_CurArea==0xFF||m_bEnablePlaceJetton == false)
  {
    return ;
  }
  //计算积分
  LONGLONG lCurrentJetton=GetCurrentJetton();
  LONGLONG lLeaveScore=m_lMeMaxScore;
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lLeaveScore -= m_lUserJettonScore[nAreaIndex];
  }
  //最大下注
  LONGLONG lUserMaxJetton = 0;

  lUserMaxJetton = GetUserMaxJetton(m_CurArea);

  lLeaveScore = min((lLeaveScore),lUserMaxJetton); //用户可下分 和最大分比较 由于是五倍


  lCurrentJetton = m_lLastJetton ;

  //设置光标
  if(lCurrentJetton>lLeaveScore)
  {
    if(lLeaveScore>=5000000L)
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
  else
  {
    SetCurrentJetton(m_lLastJetton);
  }
}
//鼠标移动消息
void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
  return __super::OnMouseMove(nFlags,point);
}
//鼠标消息
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint Point)
{
  //设置变量
  m_lCurrentJetton=0L;

  if(m_pGameClientDlg != NULL)
  {
    if(m_pGameClientDlg->GetGameStatus()!=GS_GAME_END && m_cbAreaFlash!=0xFF)
    {
      m_cbAreaFlash=0xFF;
      SetJettonHide(0);
      RefreshGameView();
    }
  }
  m_lLastJetton = 0L;

  __super::OnLButtonDown(nFlags,Point);
}
void CGameClientView::SetJettonHide(int ID)
{
  CSkinButton *btJetton[]= {NULL,&m_btJetton100,&m_btJetton1000,&m_btJetton10000,&m_btJetton100000,&m_btJetton1000000,&m_btJetton5000000};

  if(1<=ID&&ID<=6)
  {
    for(int i = 1; i<=6; i++)
    {
      if(i!=ID)
      {
        btJetton[i]->ShowWindow(SW_SHOW);
      }
    }
    btJetton[ID]->ShowWindow(SW_HIDE);

    OutputDebugString(_TEXT("设置ID\r\n"));

  }
  else
  {
    for(int i = 1; i<=6; i++)
    {
      btJetton[i]->ShowWindow(SW_SHOW);
    }
  }
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
    if(m_cbAreaFlash != cbJettonArea)
    {
      m_cbAreaFlash = cbJettonArea;
      RefreshGameView();
    }

    bool bButtonArea = false;
    CSkinButton* pRgnButton[] = { &m_btJetton100, &m_btJetton1000, &m_btJetton10000, &m_btJetton100000, &m_btJetton1000000, &m_btJetton5000000};
    for(int i = 0 ; i < CountArray(pRgnButton) && cbJettonArea==0xFF; ++i)
    {
      CRgn rgn;
      CRect rect;
      rgn.CreateRectRgn(0,0,0,0);
      pRgnButton[i]->GetWindowRgn(rgn);
      pRgnButton[i]->GetWindowRect(rect);
      ScreenToClient(&rect);
      rgn.OffsetRgn(CPoint(rect.left,rect.top));
      if(rgn.PtInRegion(MousePoint))
      {
        rgn.DeleteObject();
        bButtonArea = true;
        break;
      }
      rgn.DeleteObject();
    }

    //区域判断
    if(cbJettonArea==0xFF && !bButtonArea)
    {
      SetCursor(LoadCursor(NULL,IDC_ARROW));
      SetJettonHide(0);
      return TRUE;
    }

    //最大下注
    LONGLONG lMaxJettonScore=GetUserMaxJetton(cbJettonArea);

    //合法判断
    int iTimer = 1;

    if((m_lAllJettonScore[cbJettonArea]+m_lCurrentJetton)>m_lAreaLimitScore  && !bButtonArea)
    {
      SetJettonHide(0);
      SetCursor(LoadCursor(NULL,IDC_NO));
      return TRUE;

    }
    if(lMaxJettonScore< m_lCurrentJetton*iTimer && !bButtonArea)
    {
      SetJettonHide(0);
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
      case 50000:
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
      RefreshGameView();
    }
  }
  else
  {
    ClearAreaFlash();
  }

  return __super::OnSetCursor(pWnd, nHitTest, uMessage);
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
  RefreshGameView();
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
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
  //变量定义
  int nStringLength=lstrlen(pszString);
  int nXExcursion[8]= {1,1,1,0,-1,-1,-1,0};
  int nYExcursion[8]= {-1,0,1,1,1,0,-1,-1};

  //绘画边框
  //pDC->SetTextColor(crFrame);
  for(int i=0; i<CountArray(nXExcursion); i++)
  {
    //pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
    CDFontEx::DrawText(this,pDC,  nStringLength, 12, pszString, nXPos+nXExcursion[i], nYPos+nYExcursion[i], crFrame, TA_CENTER|TA_TOP);
  }

  //绘画字体
  //pDC->SetTextColor(crText);
  //pDC->TextOut(nXPos,nYPos,pszString,nStringLength);
  CDFontEx::DrawText(this,pDC,  nStringLength, 12, pszString, nXPos, nYPos, crText, TA_CENTER|TA_TOP);

  return;
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
}
//庄家信息
void CGameClientView::SetBankerInfo(DWORD dwBankerUserID, LONGLONG lBankerScore)
{
  //庄家椅子号
  WORD wBankerUser=INVALID_CHAIR;

  //查找椅子号
  if(INVALID_CHAIR!=dwBankerUserID)
  {
    for(WORD wChairID=0; wChairID<MAX_CHAIR; ++wChairID)
    {
      IClientUserItem *pClientUserItem=GetClientUserItem(wChairID);
      if(pClientUserItem == NULL)
      {
        continue;
      }
      if(dwBankerUserID==pClientUserItem->GetChairID())
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
void CGameClientView::DrawMoveInfo(CDC* pDC,CRect rcRect)
{
  return ;
}
//绘画标识
void CGameClientView::DrawWinFlags(CDC * pDC)
{
  //非空判断
  //m_nRecordLast = 14;
  //if (m_nRecordLast==m_nRecordFirst) return;

  //资源变量

  int nIndex = m_nScoreHead;
  COLORREF static clrOld ;

  clrOld = pDC->SetTextColor(RGB(52,116,23));
  int nDrawCount=0;
  while(nIndex != m_nRecordLast && (m_nRecordLast!=m_nRecordFirst) && nDrawCount < MAX_FALG_COUNT)
  {
    //胜利标识
    tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];



    for(int i=1; i<=1; ++i)
    {
      //位置变量
      int static nYPos=0,nXPos=0;
      nYPos=m_nWinFlagsExcursionY+i*27-i*1;
      nXPos=m_nWinFlagsExcursionX + ((nIndex - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY) * (49);

      //胜利标识
      int static nFlagsIndex=0;

      for(int i = 1; i<=AREA_COUNT; i++)
      {

        if(ClientGameRecord.enOperateMen[i]==enOperateResult_Win)
        {
          nFlagsIndex = (i-1)*2;
          if(i==5)
          {
            nFlagsIndex = 1;
          }
          if(i==6)
          {
            nFlagsIndex = 3;
          }
          if(i==7)
          {
            nFlagsIndex = 5;
          }
          if(i==8)
          {
            nFlagsIndex = 7;
          }

        }
      }
      {
        if(nFlagsIndex%2 ==0)
        {
          nYPos+=32;
        }

        //绘画标识
        m_ImageWinFlags.DrawImage(pDC, nXPos, nYPos, m_ImageWinFlags.GetWidth()/8,
                                  m_ImageWinFlags.GetHeight(),m_ImageWinFlags.GetWidth()/8 * nFlagsIndex, 0);

      }

    }

    //移动下标
    nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
    nDrawCount++;
  }

  //开奖比率
  for(BYTE i=0; i<AREA_COUNT; i++)
  {
    int static nYPos=0,nXPos=0;
    nYPos=m_nWinFlagsExcursionY+90+18;
    nXPos=m_nWinFlagsExcursionX -6 + i*125;

    if(i>3)
    {
      nYPos=m_nWinFlagsExcursionY+90;
      nXPos=m_nWinFlagsExcursionX -6 + (i-4)*125;
    }

    float fRate=0.00;
    if(m_nRecordLast!=0)
    {
      fRate = m_fRecordRate[i]/((m_nRecordLast - m_nRecordFirst + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY)*100;
    }

    //float fRate = 20.00;
    CString str;
    str.Format(_TEXT("%0.2f%%"),fRate);

    CDFontEx::DrawText(this,pDC,12,400,str,nXPos,nYPos,RGB(255,255,255),NULL);
  }


  pDC->SetTextColor(clrOld);
}
//手工搓牌
afx_msg void CGameClientView::OnOpenCard()
{
  SendEngineMessage(IDM_OPEN_CARD,0,0);

}
//自动搓牌
afx_msg void CGameClientView::OnAutoOpenCard()
{
  SendEngineMessage(IDM_AUTO_OPEN_CARD,0,0);

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
  RefreshGameView();
}

//移动按钮
void CGameClientView::OnScoreMoveR()
{
  int nHistoryCount = (m_nRecordLast - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
  if(nHistoryCount == MAX_FALG_COUNT)
  {
    return;
  }

  m_nScoreHead = (m_nScoreHead + 1) % MAX_SCORE_HISTORY;
  if(nHistoryCount-1 == MAX_FALG_COUNT)
  {
    m_btScoreMoveR.EnableWindow(FALSE);
  }

  m_btScoreMoveL.EnableWindow(TRUE);

  //更新界面
  RefreshGameView();
}

//显示结果
void CGameClientView::ShowGameResult(CDC *pDC, int nWidth, int nHeight)
{

  if(false==m_bShowGameResult)
  {
    return;
  }

  int nXPos = nWidth / 2 - 200+20+30;
  int nYPos = nHeight / 2 - 208;
  m_pngGameEnd.DrawImage(pDC, nXPos+2-20-30, nYPos+60,
                         m_pngGameEnd.GetWidth(), m_pngGameEnd.GetHeight(),0, 0);
  pDC->SetTextColor(RGB(255,255,255));


  CRect rcMeWinScore, rcMeReturnScore;
  rcMeWinScore.left = nXPos+2 + 40+30;
  rcMeWinScore.top = nYPos+70 + 32+10+10;
  rcMeWinScore.right = rcMeWinScore.left + 111;
  rcMeWinScore.bottom = rcMeWinScore.top + 34;

  rcMeReturnScore.left = nXPos+2 + 150+50;
  rcMeReturnScore.top = nYPos+70 + 32+10+10;
  rcMeReturnScore.right = rcMeReturnScore.left + 111;
  rcMeReturnScore.bottom = rcMeReturnScore.top + 34;

  CString strMeGameScore, strMeReturnScore;
  DrawNumberStringWithSpace(pDC,m_lMeCurGameScore,rcMeWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
  DrawNumberStringWithSpace(pDC,m_lMeCurGameReturnScore,rcMeReturnScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

  CRect rcBankerWinScore;
  rcBankerWinScore.left = nXPos+2 + 40+30;
  rcBankerWinScore.top =  nYPos+70 + 32+10+35+10;
  rcBankerWinScore.right = rcBankerWinScore.left + 111;
  rcBankerWinScore.bottom = rcBankerWinScore.top + 34;

  CString strBankerCurGameScore;
  strBankerCurGameScore.Format(TEXT("%I64d"), m_lBankerCurGameScore);
  DrawNumberStringWithSpace(pDC,m_lBankerCurGameScore,rcBankerWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);


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

  for(LONG l=lpRect->top; l<lpRect->bottom; l++)
  {
    for(LONG k=lpRect->left; k<lpRect->right; k++)
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
void   CGameClientView::GetAllWinArea(BYTE bcWinArea[],BYTE bcAreaCount,BYTE InArea)
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
    for(int j= 0; j<AREA_COUNT; j++)
    {

      if(bcOutCadDataWin[j]>0&&j==InArea-1)
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
//最大下注
LONGLONG CGameClientView::GetUserMaxJetton(BYTE cbJettonArea)
{
  if(cbJettonArea==0xFF)
  {
    return 0;
  }

  //已下注额
  LONGLONG lNowJetton = 0;
  ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lNowJetton += m_lUserJettonScore[nAreaIndex];
  }
  //庄家金币
  LONGLONG lBankerScore = 0x7fffffffffffffff;
  if(m_wBankerUser!=INVALID_CHAIR)
  {
    lBankerScore = m_lBankerScore;
  }

  BYTE bcWinArea[AREA_COUNT];
  LONGLONG LosScore = 0;
  LONGLONG WinScore = 0;

  GetAllWinArea(bcWinArea,AREA_COUNT,cbJettonArea);

  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    if(bcWinArea[nAreaIndex-1]>1)
    {
      LosScore+=m_lAllJettonScore[nAreaIndex]*(bcWinArea[nAreaIndex-1]);
    }
    else
    {
      //if(bcWinArea[nAreaIndex-1]==0)
      {
        WinScore+=m_lAllJettonScore[nAreaIndex];
      }
    }
  }

  LONGLONG lTemp = lBankerScore;
  lBankerScore = lBankerScore + WinScore - LosScore;

  if(lBankerScore < 0)
  {
    if(m_wBankerUser!=INVALID_CHAIR)
    {
      lBankerScore = m_lBankerScore;
    }
    else
    {
      lBankerScore = 0x7fffffffffffffff;
    }
  }

  //区域限制
  LONGLONG lMeMaxScore;

  if((m_lMeMaxScore - lNowJetton)>m_lAreaLimitScore)
  {
    lMeMaxScore= m_lAreaLimitScore;
  }
  else
  {
    lMeMaxScore = m_lMeMaxScore-lNowJetton;
    lMeMaxScore = lMeMaxScore;
  }

  //庄家限制
  lMeMaxScore=min(lMeMaxScore,(lBankerScore)/(bcWinArea[cbJettonArea-1]));

  //非零限制
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

void CGameClientView::DrawMeJettonNumber(CDC *pDC)
{
  //绘画数字
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    if(m_lUserJettonScore[nAreaIndex] > 0)
    {
      if(nAreaIndex==5)
      {
        DrawNumberString(pDC,m_lUserJettonScore[nAreaIndex],m_PointJettonNumber[nAreaIndex-1].x+25,m_PointJettonNumber[nAreaIndex-1].y-15+50, false,true);
      }
      else
      {
        DrawNumberString(pDC,m_lUserJettonScore[nAreaIndex],m_PointJettonNumber[nAreaIndex-1].x-15,m_PointJettonNumber[nAreaIndex-1].y-15+50, false,true);
      }
    }
  }
}

//开始发牌
void CGameClientView::DispatchCard()
{

  //设置标识
  m_bNeedSetGameRecord=true;
}
//结算消息


//结束发牌
void CGameClientView::FinishDispatchCard(bool bRecord /*= true*/)
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

  BYTE  bcResulteOut[AREA_COUNT];
  memset(bcResulteOut,0,AREA_COUNT);

  m_GameLogic.GetCardType(&m_cbTableCardArray[0][0],1,bcResulteOut);

  //保存记录
  if(bRecord)
  {
    SetGameHistory(bcResulteOut);
  }

  //累计积分
  m_lMeStatisticScore+=m_lMeCurGameScore;
  m_lBankerWinScore=m_lTmpBankerWinScore;


  bool blWin[AREA_COUNT];
  for(int i = 0; i<AREA_COUNT; i++)
  {

    if(bcResulteOut[i]>0)
    {
      blWin[i]=true;
    }
    else
    {
      blWin[i]=false;
    }
  }

  //设置赢家
  SetWinnerSide(blWin, true);

  //播放声音
  if(m_lMeCurGameScore>0)
  {
    SendEngineMessage(IDM_SOUND,5,5);

  }
  else if(m_lMeCurGameScore==0)
  {
    //SendEngineMessage(IDM_SOUND,4,4);

  }
  else
  {
    SendEngineMessage(IDM_SOUND,4,4);

  }

}

//胜利边框
void CGameClientView::FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC)
{
  //合法判断

  if(m_bFlashResult&&!m_blRungingCar)
  {
    m_idb_selPng.DrawImage(pDC ,m_CarRect[m_CarIndex].left,m_CarRect[m_CarIndex].top,\
                           m_idb_selPng.GetWidth()/8 ,m_idb_selPng.GetHeight(),(m_CarIndex%8)*(m_idb_selPng.GetWidth()/8),0,m_idb_selPng.GetWidth()/8 ,m_idb_selPng.GetHeight());
  }

  //if (m_cbAreaFlash==0xFF && false==m_bFlashResult) return;

  //下注判断
  if(false==m_bFlashResult && m_pGameClientDlg != NULL)
  {
    if(m_pGameClientDlg->GetGameStatus()==GS_PLACE_JETTON)
    {
      //pDC->Draw3dRect(m_RectArea[m_cbAreaFlash-1].left,m_RectArea[m_cbAreaFlash-1].top,m_RectArea[m_cbAreaFlash-1].Width()  ,m_RectArea[m_cbAreaFlash-1].Height(),RGB(255,255,0),RGB(255,255,0));
      //m_ImageFrameTianMen.TransDrawImage(pDC,m_RectArea[m_cbAreaFlash-1].left,m_RectArea[m_cbAreaFlash-1].top,RGB(255,0,255));
      //m_PngFrameTianMen.DrawImage(pDC,m_RectArea[m_cbAreaFlash-1].left,m_RectArea[m_cbAreaFlash-1].top);

      for(BYTE i=0; i<8; i++)
      {
        if(m_cbAreaFlash-1 != i)
        {
          continue;
        }

        if(i==0)
        {
          m_PngFrameTianMen.DrawImage(pDC,m_RectArea[0].left,m_RectArea[0].top);
        }
        if(i==1)
        {
          m_PngFrameTianMen1.DrawImage(pDC,m_RectArea[1].left,m_RectArea[1].top);
        }
        if(i==2)
        {
          m_PngFrameTianMen2.DrawImage(pDC,m_RectArea[2].left,m_RectArea[2].top);
        }
        if(i==3)
        {
          m_PngFrameTianMen3.DrawImage(pDC,m_RectArea[3].left,m_RectArea[3].top);
        }
        if(i==4)
        {
          m_PngFrameTianMen4.DrawImage(pDC,m_RectArea[4].left,m_RectArea[4].top);
        }
        if(i==5)
        {
          m_PngFrameTianMen5.DrawImage(pDC,m_RectArea[5].left,m_RectArea[5].top);
        }
        if(i==6)
        {
          m_PngFrameTianMen6.DrawImage(pDC,m_RectArea[6].left,m_RectArea[6].top);
        }
        if(i==7)
        {
          m_PngFrameTianMen7.DrawImage(pDC,m_RectArea[7].left,m_RectArea[7].top);
        }
      }

    }
  }
  else
  {
    {
      for(int i = 0; i<AREA_COUNT; i++)
      {
        if(m_bWinFlag[i])
        {
          //pDC->Draw3dRect(m_RectArea[i].left,m_RectArea[i].top,m_RectArea[i].Width()  ,m_RectArea[i].Height(),RGB(255,255,0),RGB(255,255,0));
          //m_ImageFrameTianMen.TransDrawImage(pDC,m_RectArea[i].left,m_RectArea[i].top,RGB(255,0,255));

          if(i==0)
          {
            m_PngFrameTianMen.DrawImage(pDC,m_RectArea[0].left,m_RectArea[0].top);
          }
          if(i==1)
          {
            m_PngFrameTianMen1.DrawImage(pDC,m_RectArea[1].left,m_RectArea[1].top);
          }
          if(i==2)
          {
            m_PngFrameTianMen2.DrawImage(pDC,m_RectArea[2].left,m_RectArea[2].top);
          }
          if(i==3)
          {
            m_PngFrameTianMen3.DrawImage(pDC,m_RectArea[3].left,m_RectArea[3].top);
          }
          if(i==4)
          {
            m_PngFrameTianMen4.DrawImage(pDC,m_RectArea[4].left,m_RectArea[4].top);
          }
          if(i==5)
          {
            m_PngFrameTianMen5.DrawImage(pDC,m_RectArea[5].left,m_RectArea[5].top);
          }
          if(i==6)
          {
            m_PngFrameTianMen6.DrawImage(pDC,m_RectArea[6].left,m_RectArea[6].top);
          }
          if(i==7)
          {
            m_PngFrameTianMen7.DrawImage(pDC,m_RectArea[7].left,m_RectArea[7].top);
          }
        }
      }
    }
  }
}

//推断赢家
void CGameClientView::DeduceWinner(bool bWinMen[])
{
  BYTE bcData = m_cbTableCardArray[0][0];
  if(1==bcData||bcData==2||bcData==1+8||bcData==2+8||bcData==1+2*8||bcData==2+2*8||bcData==1+3*8||bcData==2+3*8)
  {
    if(bcData%2==1)
    {
      bWinMen[0]= true;
    }
    else
    {
      bWinMen[1]= true;
    }

  }
  else if(3==bcData||bcData==4||bcData==3+8||bcData==4+8||bcData==3+2*8||bcData==4+2*8||bcData==3+3*8||bcData==4+3*8)
  {
    if(bcData%2==1)
    {
      bWinMen[2]= true;
    }
    else
    {
      bWinMen[3]= true;
    }



  }
  else if(5==bcData||bcData==6||bcData==5+8||bcData==6+8||bcData==5+2*8||bcData==6+2*8||bcData==5+3*8||bcData==6+3*8)
  {
    if(bcData%2==1)
    {
      bWinMen[4]= true;
    }
    else
    {
      bWinMen[5]= true;
    }

  }
  else if(7==bcData||bcData==8||bcData==7+8||bcData==8+8||bcData==7+2*8||bcData==8+2*8||bcData==7+3*8||bcData==8+3*8)
  {
    if(bcData%2==1)
    {
      bWinMen[6]= true;
    }
    else
    {
      bWinMen[7]= true;
    }

  }
}

//控件命令
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
  //获取ID
  WORD wControlID=LOWORD(wParam);

  //控件判断
  switch(wControlID)
  {

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
      m_lCurrentJetton=50000L;
      break;
    }
    case IDC_JETTON_BUTTON_100000:
    {
      //设置变量
      m_lCurrentJetton=100000L;
      break;
    }
    case IDC_JETTON_BUTTON_500000:
    {
      //设置变量
      m_lCurrentJetton=500000L;
      break;
    }
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
    case IDC_AUTO_OPEN_CARD:
    {
      break;
    }
    case IDC_OPEN_CARD:
    {
      break;
    }
    case IDC_BANK:
    {
      break;
    }
    case IDC_CONTINUE_CARD:
    {
      SwithToNormalView();
      SendEngineMessage(IDM_CONTINUE_CARD,0,0);
      break;
    }
    case IDC_RADIO:
    {
      m_CheckImagIndex = 0;

    }

    break;
    case IDC_RADIO+1:
    {
      m_CheckImagIndex = 1;
    }
    break;
    case IDC_RADIO+2:
    {
      m_CheckImagIndex = 2;

    }
    break;
    case IDC_RADIO+3:
    {
      m_CheckImagIndex = 3;

    }
    break;

  }

  m_lLastJetton = m_lCurrentJetton;

  return CGameFrameViewGDI::OnCommand(wParam, lParam);
}

//我的位置
void CGameClientView::SetMeChairID(DWORD dwMeUserID)
{
  //查找椅子号
  for(WORD wChairID=0; wChairID<MAX_CHAIR; ++wChairID)
  {
    IClientUserItem *pClientUserItem=GetClientUserItem(wChairID);
    if(NULL!=pClientUserItem && dwMeUserID==pClientUserItem->GetChairID())
    {
      m_wMeChairID=wChairID;
      break;
    }
  }
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
  RefreshGameView();
}

//绘画庄家
void CGameClientView::DrawBankerInfo(CDC *pDC,int nWidth,int nHeight)
{
  //庄家信息
  pDC->SetTextColor(RGB(255,255,0));

  //获取玩家
  IClientUserItem *pClientUserItem = m_wBankerUser==INVALID_CHAIR ? NULL : GetClientUserItem(m_wBankerUser);

  //位置信息
  m_LifeWidth;
  m_TopHeight;

  CRect static StrRect;
  StrRect.left = m_LifeWidth+79-66+125;
  StrRect.top = m_TopHeight+84-66+116;
  StrRect.right = StrRect.left + 171;
  StrRect.bottom = StrRect.top + 18;

  //庄家名字
  TCHAR Text[256];
  if(m_bEnableSysBanker)
  {
    _sntprintf(Text,256,_TEXT("%s"),pClientUserItem==NULL?(m_bEnableSysBanker?TEXT("系统坐庄"):TEXT("系统坐庄")):pClientUserItem->GetNickName());
    //pDC->DrawText(Text, StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE );

    CDFontEx::DrawText(this,pDC,  12, 400, Text, StrRect, RGB(255,255,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);
  }
  else
  {
    _sntprintf(Text,256,_TEXT("%s"),pClientUserItem==NULL?(m_bEnableSysBanker==false?TEXT("无人坐庄"):TEXT("无人坐庄")):pClientUserItem->GetNickName());
    //pDC->DrawText(Text, StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE );

    CDFontEx::DrawText(this,pDC,  12, 400, Text, StrRect, RGB(255,255,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);
  }

  StrRect.left -= 28;
  StrRect.right -= 28;
  StrRect.top = StrRect.bottom;
  StrRect.bottom = StrRect.top + 20;
  _sntprintf(Text,256,_TEXT("%s"),NumberStringWithSpace(pClientUserItem==NULL?0:pClientUserItem->GetUserScore()));
  //pDC->DrawText(Text, StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);

  CDFontEx::DrawText(this,pDC,  12, 400, Text, StrRect, RGB(255,255,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);

  StrRect.left -= 23;
  StrRect.right -= 23;
  StrRect.top = StrRect.bottom;
  StrRect.bottom = StrRect.top + 20;
  _sntprintf(Text,256,_TEXT("%s"),NumberStringWithSpace(m_lBankerWinScore));
  //pDC->DrawText(Text, StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE );
  CDFontEx::DrawText(this,pDC,  12, 400, Text, StrRect, RGB(255,255,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);

  StrRect.left -= 13;
  StrRect.right -= 13;
  StrRect.top = StrRect.bottom;
  StrRect.bottom = StrRect.top + 17;
  _sntprintf(Text,256,_TEXT("%d"),m_wBankerTime);
  //pDC->DrawText(Text, StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE );

  CDFontEx::DrawText(this,pDC,  12, 400, Text, StrRect, RGB(255,255,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);
}

// 绘画上庄列表
void CGameClientView::DrawBankerList(CDC *pDC, int nWidth, int nHeight)
{
  // 定义变量
  CPoint ptBegin(0,0);
  // 申请上庄列表
  ptBegin.SetPoint(nWidth/2 - 360, nHeight/2+42-96);

  int   nCount = 0;
  CRect rect(0, 0, 0, 0);
  rect.SetRect(ptBegin.x , ptBegin.y , ptBegin.x + 179, ptBegin.y + 37);

  CRect rectName(rect);
  rectName.left = ptBegin.x;
  rectName.right = ptBegin.x + 85;

  CRect rectScore(rect);
  rectScore.left = ptBegin.x + 85;
  rectScore.right = rectScore.left +70;

  for(int i = m_nShowValleyIndex; i < m_ValleysList.GetCount(); ++i)
  {
    IClientUserItem* pIClientUserItem = GetClientUserItem(m_ValleysList[i]);
    if(pIClientUserItem == NULL)
    {
      continue;
    }

    // 名字
    DrawTextString(pDC, pIClientUserItem->GetNickName(), RGB(255,234,0) , RGB(38,26,2), rectName, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

    // 积分
    DrawNumberStringWithSpace(pDC,pIClientUserItem->GetUserScore(), rectScore, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

    //我的上庄队位
    if(m_ValleysList[i]  == m_wMeChairID)
    {
      CString sChairID;
      sChairID.Format(_TEXT("%d"),i+1);
      CDFontEx::DrawText(this,pDC,  12, 400, sChairID,  nWidth/2-177,nHeight/2+40, RGB(255,255,255), TA_CENTER|TA_TOP);
    }


    // 位置调移
    rectName.OffsetRect(10, 15);
    rectScore.OffsetRect(10, 15);

    // 位数增加
    nCount++;

    if(nCount == 5)
    {
      break;
    }
  }

}
void CGameClientView::SetFirstShowCard(BYTE bcCard)
{


}
//绘画玩家
void CGameClientView::DrawMeInfo(CDC *pDC,int nWidth,int nHeight)
{
  //合法判断
  if(INVALID_CHAIR==m_wMeChairID)
  {
    return;
  }

  //庄家信息
  pDC->SetTextColor(RGB(255,255,0));

  //获取玩家
  IClientUserItem *pClientUserItem =  GetClientUserItem(m_wMeChairID);

  //位置信息
  m_LifeWidth;
  m_TopHeight;

  CRect static StrRect;
  StrRect.left = m_LifeWidth+79-66+17-75;
  StrRect.top = m_TopHeight+84-66+610-41;
  StrRect.right = StrRect.left + 144;
  StrRect.bottom = StrRect.top + 24;

  //庄家名字
  TCHAR Text[256];
  _sntprintf(Text,256,_TEXT("%s"),pClientUserItem->GetNickName());
  CDFontEx::DrawText(this,pDC,  12, 400, Text, StrRect, RGB(255,255,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);

  StrRect.top = StrRect.bottom;
  StrRect.bottom = StrRect.top + 27;
  _sntprintf(Text,256,_TEXT("%s"),NumberStringWithSpace(pClientUserItem->GetUserScore()));
  CDFontEx::DrawText(this,pDC,  12, 400, Text, StrRect, RGB(255,255,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);

  StrRect.top = StrRect.bottom;
  StrRect.bottom = StrRect.top + 27;
  _sntprintf(Text,256,_TEXT("%s"),NumberStringWithSpace(m_lMeStatisticScore));
  CDFontEx::DrawText(this,pDC,  12, 400, Text, StrRect, RGB(255,255,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);

  StrRect.top = StrRect.bottom;
  StrRect.bottom = StrRect.top + 27;
  LONGLONG lMeJetton=0L;
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lMeJetton += m_lUserJettonScore[nAreaIndex];
  }
  _sntprintf(Text,256,_TEXT("%s"),NumberStringWithSpace(pClientUserItem==NULL?0:lMeJetton));
  CDFontEx::DrawText(this,pDC,  12, 400, Text, StrRect, RGB(255,255,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP|DT_SINGLELINE);

}
//////////////////////////////////////////////////////////////////////////
//银行存款
void CGameClientView::OnBankStorage()
{
#ifdef __SPECIAL___
  //获取接口
  CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
  IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

  if(NULL!=pIClientKernel)
  {
    CRect btRect;
    m_btBankerStorage.GetWindowRect(&btRect);
    ShowInsureSave(pIClientKernel,CPoint(btRect.left-100,btRect.top));
  }
#endif
}

//银行取款
void CGameClientView::OnBankDraw()
{
#ifdef __SPECIAL___
  //获取接口
  CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
  IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

  if(NULL!=pIClientKernel)
  {
    CRect btRect;
    m_btBankerDraw.GetWindowRect(&btRect);
    ShowInsureGet(pIClientKernel,CPoint(btRect.left-550,btRect.top-100));
  }
#endif
}

void CGameClientView::OnUp()
{
  if(m_nShowValleyIndex > 0)
  {
    m_nShowValleyIndex--;
  }

  //更新界面
  InvalidGameView(0,0,0,0);

}
void CGameClientView::OnDown()
{
  if(m_nShowValleyIndex < m_ValleysList.GetCount() - 1)
  {
    m_nShowValleyIndex++;
  }

  //更新界面
  InvalidGameView(0,0,0,0);
};
void CGameClientView::StartMove()
{
  m_Out_Bao_y = 0;
  m_bShowJettonIndex = 0;
  m_bShowLeaveHandleIndex = 0;
  m_bShowBao = TRUE;
  m_bShowJettonAn = false;
  m_bShowLeaveHandleAn = false;
  m_bOPenBoxAn = false;
  m_bOPenBoxIndex = 0;
  m_blShowLastResult = false;
  m_bShowResult = false;

}
void CGameClientView::StartJetton_AniMationN()
{
  m_bShowJettonAn = true;
  m_bShowJettonIndex = 0;


}

void CGameClientView::StartOPenBox()
{
  m_bOPenBoxAn = true;
  m_bOPenBoxIndex = 0;
  m_bShowBao = false;
  m_Out_Bao_y = 6;
  SetJettonHide(0);

}

void CGameClientView::StartHandle_Leave()
{
  m_bShowLeaveHandleAn = true;
  m_bShowLeaveHandleIndex = 0;

  SendEngineMessage(IDM_SOUND,0,1);


}
void CGameClientView::StarShowResult()
{
  m_bShowResult = true;

}
//绘画时间
void CGameClientView::MyDrawUserTimer(CDC * pDC, int nXPos, int nYPos, WORD wTime, WORD wTimerArea)
{
  //加载资源
  CPngImageEx ImageTimeBack;
  CPngImageEx ImageTimeNumber;
  ImageTimeBack.LoadImage(this,GetModuleHandle(GAME_FRAME_DLL_NAME),TEXT("TIME_BACK"));
  ImageTimeNumber.LoadImage(this,GetModuleHandle(GAME_FRAME_DLL_NAME),TEXT("TIME_NUMBER"));

  //获取属性
  const INT nNumberHeight=ImageTimeNumber.GetHeight();
  const INT nNumberWidth=ImageTimeNumber.GetWidth()/10;

  //计算数目
  LONGLONG lNumberCount=0;
  WORD wNumberTemp=wTime;
  do
  {
    lNumberCount++;
    wNumberTemp/=10;
  }
  while(wNumberTemp>0L);

  //位置定义
  INT nYDrawPos=nYPos-nNumberHeight/2+1;
  INT nXDrawPos=static_cast<INT>(nXPos+(lNumberCount*nNumberWidth)/2-nNumberWidth);

  //绘画背景
  CSize SizeTimeBack(ImageTimeBack.GetWidth(),ImageTimeBack.GetHeight());
  ImageTimeBack.DrawImage(pDC,nXPos-SizeTimeBack.cx/2,nYPos-SizeTimeBack.cy/2);

  //绘画号码
  for(LONGLONG i=0; i<lNumberCount; i++)
  {
    //绘画号码
    WORD wCellNumber=wTime%10;
    ImageTimeNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,wCellNumber*nNumberWidth,0);

    //设置变量
    wTime/=10;
    nXDrawPos-=nNumberWidth;
  };

  return;
}

//管理员控制
void CGameClientView::OpenAdminWnd()
{
  //有权限
  if(m_pClientControlDlg != NULL)
  {
    if(!m_pClientControlDlg->IsWindowVisible())
    {
      m_pClientControlDlg->CenterWindow();
      CRect rect;
      m_pClientControlDlg->GetWindowRect(&rect);
      rect.left-=110;
      rect.right-=110;
      m_pClientControlDlg->MoveWindow(rect);
      m_pClientControlDlg->ClearText();
      m_pClientControlDlg->ShowWindow(SW_SHOW);

    }
    else
    {
      m_pClientControlDlg->ShowWindow(SW_HIDE);
    }
  }

}

//执行剩余所有的缓冲动画
void CGameClientView::PerformAllBetAnimation()
{
  KillTimer(IDI_ANDROID_BET);
  for(int i = 0 ; i < m_ArrayAndroid.GetCount(); ++i)
  {
    KillTimer(IDI_ANDROID_BET + i + 1);
    PlaceUserJetton(m_ArrayAndroid[i].cbJettonArea, m_ArrayAndroid[i].lJettonScore);
  }
  m_ArrayAndroid.RemoveAll();
}

//更新视图
void CGameClientView::RefreshGameView()
{
  CRect rect;
  GetClientRect(&rect);
  InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

  return;
}

//控制下注检测
void CGameClientView::ControlBetDetection(BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair)
{
  if(m_pClientControlDlg == NULL || wChair >= GAME_PLAYER)
  {
    return;
  }

  ////获取金币
  m_lAllPlayBet[wChair][cbViewIndex-1] += lScoreCount;

  //玩家信息
  //IClientUserItem* pUserData = GetClientUserItem(wChair);

  // m_pClientControlDlg->UpdateClistCtrl(m_lAllPlayBet,cbViewIndex,lScoreCount,wChair,pUserData);
  CUserBetArray *pUserBetArray = &m_pClientControlDlg->m_UserBetArray;
  pUserBetArray->RemoveAll();

  for(WORD i=0; i<GAME_PLAYER; i++)
  {

    IClientUserItem* pClientUserItem = GetClientUserItem(i);
    if(NULL == pClientUserItem)
    {
      continue;
    }
    if(pClientUserItem->IsAndroidUser())
    {
      continue;
    }

    LONGLONG lUserAllBet = 0;
    for(BYTE j=0; j<AREA_COUNT; j++)
    {
      lUserAllBet += m_lAllPlayBet[i][j];
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
    UserBet.lUserScore = pClientUserItem->GetUserScore();
    UserBet.lUserScoreTotal=m_lUserScoreTotal[CGameFrameEngine::GetInstance()->SwitchViewChairID(i)];
    for(BYTE k=0; k<AREA_COUNT; k++)
    {
      UserBet.lUserBet[k] = m_lAllPlayBet[i][k];
    }

    //插入数据
    pUserBetArray->Add(UserBet);
  }

  //更新控件
  m_pClientControlDlg->UpdateUserBet(false);



}

void CGameClientView::SetUserTotal(LONGLONG lUserScoreTotal,WORD wChair)
{

  m_lUserScoreTotal[wChair]+=lUserScoreTotal;
}

////游戏设置
//void CGameClientView::OnButtonGameOption()
//{
//  //SendEngineMessage(IDM_GAME_OPTIONS,1,0);
//  //显示配置
//  CDlgGameOption DlgGameOption;
//  if (DlgGameOption.CreateGameOption(NULL,0)==true)
//  {
//    return;
//  }
//}

//关闭游戏
//void CGameClientView::OnButtonClose()
//{
//
//  CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
//  if (pGameClientEngine->GetGameStatus()!=GAME_STATUS_FREE)
//  {
//
//    //构造数据
//    CMessageTipDlg GameTips;
//  }
//
//  //  //配置数据
//  //  if (GameTips.DoModal()==IDOK)
//  //  {
//  //    AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_CLOSE);
//  //  }
//  //  else
//  //    return;
//  //}
//  //else
//  //  AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_CLOSE);
//}