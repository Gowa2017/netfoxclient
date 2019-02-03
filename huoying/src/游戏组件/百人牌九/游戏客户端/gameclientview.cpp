#include "StdAfx.h"
#include "Math.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
#include ".\gameclientview.h"
#include "GameFrameWindow.h"
//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FLASH_WINNER      100                 //闪动标识
#define IDI_SHOW_CHANGE_BANKER    101                 //轮换庄家
#define IDI_DISPATCH_CARD     102                 //发牌标识
#define IDI_SHOWDISPATCH_CARD_TIP 103                 //发牌提示
#define IDI_OPENCARD        104                 //发牌提示
#define IDI_MOVECARD_END      105                 //移動牌結束
#define IDI_POSTCARD        106                 //发牌提示
#define IDC_MAX           300                 //最大按钮
#define IDC_RESTORE         301                 //恢复按钮

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
#define IDC_JETTON_BUTTON_500000  212                 //按钮标识
#define IDC_AUTO_OPEN_CARD      213                 //按钮标识
#define IDC_OPEN_CARD       214                 //按钮标识
#define IDC_CONTINUE_CARD     216                 //按钮标识
#define IDC_UP            223                 //按钮标识
#define IDC_DOWN          224                 //按钮标识
#define IDC_OPTION          225                 //设置按钮标识
#define IDC_MIN           226                 //设置按钮标识
#define IDC_CLOSE         227                 //设置按钮标识
#define IDC_BANK          228                 //按钮标识
#define IDC_CHAT_DISPLAY      229                 //控件标识
#define IDC_USER_LIST       230                 //控件标识
#define IDC_SOUND         231                 //声音那妞
#define IDC_SEND_CHAT       232                 //发送按钮
#define IDC_CHAT_INPUT        233                 //控件标识
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
  ON_WM_TIMER()
  ON_WM_CREATE()
  ON_WM_SETCURSOR()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_CTLCOLOR()
  ON_BN_CLICKED(IDC_UP,OnUp)
  ON_BN_CLICKED(IDC_DOWN,OnDown)
  ON_BN_CLICKED(IDC_BT_ADMIN,OpenAdminWnd)
  //ON_BN_CLICKED(IDC_BANK_DRAW, OnBankDraw)
  ON_BN_CLICKED(IDC_BANK_STORAGE, OnBankStorage)
  ON_BN_CLICKED(IDC_APPY_BANKER, OnApplyBanker)
  ON_BN_CLICKED(IDC_CANCEL_BANKER, OnCancelBanker)
  ON_BN_CLICKED(IDC_SCORE_MOVE_L, OnScoreMoveL)
  ON_BN_CLICKED(IDC_SCORE_MOVE_R, OnScoreMoveR)
  ON_BN_CLICKED(IDC_OPEN_CARD, OnOpenCard)
  ON_BN_CLICKED(IDC_AUTO_OPEN_CARD,OnAutoOpenCard)
  //ON_MESSAGE(WM_VIEWLBTUP,OnViLBtUp)
  ON_WM_LBUTTONDBLCLK()
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
  m_nEndGameMul = 0;

  //庄家信息
  m_wBankerUser=INVALID_CHAIR;
  m_wBankerTime=0;
  m_lBankerScore=0L;
  m_lBankerWinScore=0L;
  m_lTmpBankerWinScore=0;
  m_TempScore=0;
  m_nShowValleyIndex = 0;

  //当局成绩
  m_lMeCurGameScore=0L;
  m_lMeCurGameReturnScore=0L;
  m_lBankerCurGameScore=0L;
  m_lGameRevenue=0L;

  //状态信息
  m_strRoomName = GAME_NAME;
  m_lCurrentJetton=0L;
  m_cbAreaFlash=0xFF;
  m_wMeChairID=INVALID_CHAIR;
  m_bShowChangeBanker=false;
  m_bNeedSetGameRecord=false;
  m_bWinShunMen=false;
  m_bWinDuiMen=false;
  m_bWinDaoMen=false;
  m_bFlashResult=false;
  m_blMoveFinish = false;
  m_blAutoOpenCard = true;
  m_enDispatchCardTip=enDispatchCardTip_NULL;

  m_lMeCurGameScore=0L;
  m_lMeCurGameReturnScore=0L;
  m_lBankerCurGameScore=0L;

  m_lAreaLimitScore=0L;

  //作弊变量
  m_bShowCheatInfo = false;
  ZeroMemory(m_bCheatCard, sizeof(m_bCheatCard));
  ZeroMemory(m_nWinInfo, sizeof(m_nWinInfo));

  //位置信息
  m_nScoreHead = 0;
  m_nRecordFirst= 0;
  m_nRecordLast= 0;

  //历史成绩
  m_lMeStatisticScore=0;

  //控件变量
  m_pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);

  m_cbTableCardArray[BANKER_INDEX][0]=0x1A;
  m_cbTableCardArray[BANKER_INDEX][1]=0x37;
  m_cbTableCardArray[SHUN_MEN_INDEX][0]=0x2A;
  m_cbTableCardArray[SHUN_MEN_INDEX][1]=0x07;
  bool bWinShunMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[SHUN_MEN_INDEX],2)==1?true:false;

  m_pClientControlDlg = NULL;
  m_hInst = NULL;

  m_bPlaceJettonEnd=false;

  m_dwChatTime = 0L;

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

void CGameClientView::SetMoveCardTimer()
{
  KillTimer(IDI_POSTCARD);
  m_OpenCardIndex = 1;
  m_bcShowCount = 0;
  m_PostStartIndex =  m_GameLogic.GetCardValue(m_bcfirstShowCard);
  if(m_PostStartIndex>1)
  {
    m_PostStartIndex  = (m_PostStartIndex-1)%4;
  }
  m_PostCardIndex=m_PostStartIndex;
  for(int i = 0; i<4; i++)
  {
    m_CardControl[i].m_blAdd = true;
    m_CardControl[i].m_blAuto = m_blAutoOpenCard;
    m_CardControl[i].m_blGameEnd = true;
  }
  SetTimer(IDI_POSTCARD,500,NULL);
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
  m_ImageViewFill.LoadFromResource(hInstance,IDB_VIEW_FILL);
  m_ImageViewBack.LoadImage(hInstance,TEXT("VIEW_BACK"));
  m_ImageWinFlags.LoadImage(hInstance,TEXT("WIN_FLAGS"));
  m_ImageJettonView.LoadImage(this,hInstance,TEXT("JETTOM_VIEW"));
  m_ImageScoreNumber.LoadImage(this,hInstance,TEXT("SCORE_NUMBER"));
  m_ImageMeScoreNumber.LoadImage(this,hInstance,TEXT("ME_SCORE_NUMBER"));
  m_ImageWaitValleys.LoadImage(hInstance,TEXT("WAIT_VALLEYS"));

  //边框资源
  m_ImageFrameShunMen.LoadImage(this,hInstance,TEXT("FRAME_SHUN_MEN_EX"));
  m_ImageFrameJiaoL.LoadImage(this,hInstance,TEXT("FRAME_JIAO_L_EX"));
  m_ImageFrameQiao.LoadImage(this,hInstance,TEXT("FRAME_QIAO_EX"));
  m_ImageFrameDuiMen.LoadImage(this,hInstance,TEXT("FRAME_DUI_MEN_EX"));
  m_ImageFrameDaoMen.LoadImage(this,hInstance,TEXT("FRAME_DAO_EMN_EX"));
  m_ImageFrameJiaoR.LoadImage(this,hInstance,TEXT("FRAME_JIAO_R_EX"));

  m_ImageGameEnd.LoadImage(this, hInstance,TEXT("GAME_END"));

  m_ImageMeBanker.LoadImage(hInstance,TEXT("ME_BANKER"));
  m_ImageChangeBanker.LoadImage(hInstance,TEXT("CHANGE_BANKER"));
  m_ImageNoBanker.LoadImage(hInstance,TEXT("NO_BANKER"));

  m_ImageBankerInfo.LoadImage(hInstance,TEXT("BANKER_INFO"));
  m_ImagePlayerInfo.LoadImage(hInstance,TEXT("PLAYER_INFO"));

  m_ImageHistoryMid.LoadImage(hInstance,TEXT("HISTORY_MID"));
  m_ImageHistoryFront.LoadImage(hInstance,TEXT("HISTORY_FRONT"));
  m_ImageHistoryBack.LoadImage(hInstance,TEXT("HISTORY_BACK"));

  m_ImageTimeFlag.LoadImage(hInstance,TEXT("TIME_FLAG"));
  m_ImageTimeBack.LoadImage(hInstance,TEXT("TIME_BACK"));
  m_ImageTimeNumber.LoadImage(hInstance,TEXT("TIME_NUMBER"));

  m_ImageGameFrame[0].LoadImage(hInstance,TEXT("FRAME_TL"));
  m_ImageGameFrame[1].LoadImage(hInstance,TEXT("FRAME_TM"));
  m_ImageGameFrame[2].LoadImage(hInstance,TEXT("FRAME_TR"));
  m_ImageGameFrame[3].LoadImage(hInstance,TEXT("FRAME_LM"));
  m_ImageGameFrame[4].LoadImage(hInstance,TEXT("FRAME_RM"));
  m_ImageGameFrame[5].LoadImage(hInstance,TEXT("FRAME_BL"));
  m_ImageGameFrame[6].LoadImage(hInstance,TEXT("FRAME_BM"));
  m_ImageGameFrame[7].LoadImage(hInstance,TEXT("FRAME_BR"));

  m_ImageUserBack.LoadImage(hInstance,TEXT("USER_BACK"));
  m_ImageChatBack.LoadImage(hInstance,TEXT("CHAT_BACK"));

  //创建控件
  CRect rcCreate(0,0,0,0);
  m_GameRecord.Create(IDD_DLG_GAME_RECORD,this);
  //m_ApplyUser.Create( IDD_DLG_GAME_RECORD,this );
  m_GameRecord.ShowWindow(SW_HIDE);

  //下注按钮
  m_btJetton100.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100);
  m_btJetton1000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000);
  m_btJetton10000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_10000);
  m_btJetton100000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100000);
  m_btJetton500000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_500000);
  m_btJetton1000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000000);
  m_btJetton5000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_5000000);

  //申请按钮
  m_btApplyBanker.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_APPY_BANKER);
  m_btCancelBanker.Create(NULL,WS_CHILD|WS_DISABLED,rcCreate,this,IDC_CANCEL_BANKER);

  m_btScoreMoveL.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_L);
  m_btScoreMoveR.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_R);

  m_btAutoOpenCard.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_AUTO_OPEN_CARD);
  m_btOpenCard.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OPEN_CARD);

  m_btContinueCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_CONTINUE_CARD);
  m_btBank.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_BANK);

  m_btUp.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_UP);
  m_btDown.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_DOWN);
  m_btRestore.Create(NULL,WS_CHILD,rcCreate,this,IDC_RESTORE);
  m_btMax.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MAX);

  //框架按钮
  m_btOption.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OPTION);
  m_btMin.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MIN);
  m_btClose.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_CLOSE);
  m_btSound.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SOUND);

  m_btSendChat.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SEND_CHAT);

  //设置按钮
  HINSTANCE hResInstance=AfxGetInstanceHandle();
  m_btJetton100.SetButtonImage(IDB_BT_JETTON_100,hResInstance,false,true);
  m_btJetton1000.SetButtonImage(IDB_BT_JETTON_1000,hResInstance,false,true);
  m_btJetton10000.SetButtonImage(IDB_BT_JETTON_10000,hResInstance,false,true);
  m_btJetton100000.SetButtonImage(IDB_BT_JETTON_100000,hResInstance,false,true);
  m_btJetton500000.SetButtonImage(IDB_BT_JETTON_500000,hResInstance,false,true);
  m_btJetton1000000.SetButtonImage(IDB_BT_JETTON_1000000,hResInstance,false,true);
  m_btJetton5000000.SetButtonImage(IDB_BT_JETTON_5000000,hResInstance,false,true);

  m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false,false);
  m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,hResInstance,false,false);

  m_btScoreMoveL.SetButtonImage(IDB_BT_SCORE_MOVE_L,hResInstance,false,false);
  m_btScoreMoveR.SetButtonImage(IDB_BT_SCORE_MOVE_R,hResInstance,false,false);

  m_btAutoOpenCard.SetButtonImage(IDB_BT_AUTO_OPEN_CARD,hResInstance,false,false);
  m_btOpenCard.SetButtonImage(IDB_BT_OPEN_CARD,hResInstance,false,false);

  m_btContinueCard.SetButtonImage(IDB_BT_CONTINUE_CARD,hResInstance,false,false);
  m_btBank.SetButtonImage(IDB_BT_BANK,hResInstance,false,false);

  m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(4,34,11,41),this,IDC_BT_ADMIN);
  m_btOpenAdmin.ShowWindow(SW_HIDE);

  m_btUp.SetButtonImage(IDB_BT_BT_S,hResInstance,false,false);
  m_btDown.SetButtonImage(IDB_BT_BT_X,hResInstance,false,false);

  m_btOption.SetButtonImage(IDB_BT_OPTION,hResInstance,false,false);
  m_btMin.SetButtonImage(IDB_BT_MIN,hResInstance,false,false);
  m_btClose.SetButtonImage(IDB_BT_CLOSE,hResInstance,false,false);
  m_btSound.SetButtonImage(IDB_BT_SOUND_OPEN,hResInstance,false,false);

  m_btMax.SetButtonImage(IDB_BT_MAX,hResInstance,false,false);
  m_btRestore.SetButtonImage(IDB_BT_RESTORE,hResInstance,false,false);
  m_btSendChat.SetButtonImage(IDB_BT_SEND_CHAT,hInstance,false,false);


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

  m_ChatDisplay.SetBackgroundColor(false,RGB(38,29,25));
  m_ChatDisplay.SetReadOnly(TRUE);

  m_UserList.Create(LVS_REPORT | LVS_OWNERDRAWFIXED | WS_CHILD | WS_VISIBLE ,rcCreate, this, IDC_USER_LIST);

  m_UserList.SetBkColor(RGB(38,29,25));
  m_UserList.SetTextColor(RGB(255,255,255));
  m_UserList.SetTextBkColor(RGB(38,29,25));
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
  m_hInst = LoadLibrary(_TEXT("RedNineBattleClientControl.dll"));
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

  m_btBankerStorage.Create(NULL,WS_CHILD,rcCreate,this,IDC_BANK_STORAGE);
  m_btBankerDraw.Create(NULL,WS_CHILD,rcCreate,this,IDC_BANK_DRAW);

  m_btBankerStorage.SetButtonImage(IDB_BT_STORAGE,hResInstance,false,false);
  m_btBankerDraw.SetButtonImage(IDB_BT_DRAW,hResInstance,false,false);

  m_DFontExT.CreateFont(this, TEXT("宋体"), 12, 400);

  m_brush.CreateSolidBrush(RGB(255,0,255));

  return 0;
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
  m_TempScore=0;
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
  m_bWinShunMen=false;
  m_bWinDuiMen=false;
  m_bWinDaoMen=false;
  m_bFlashResult=false;
  m_bShowGameResult=false;
  m_enDispatchCardTip=enDispatchCardTip_NULL;

  m_lMeCurGameScore=0L;
  m_lMeCurGameReturnScore=0L;
  m_lBankerCurGameScore=0L;

  m_lAreaLimitScore=0L;

  //作弊变量
  m_bShowCheatInfo = false;
  ZeroMemory(m_bCheatCard, sizeof(m_bCheatCard));
  ZeroMemory(m_nWinInfo, sizeof(m_nWinInfo));

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

  return;
}

//调整控件
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
  //位置信息
  m_nWinFlagsExcursionX = LESS_SCREEN_CX/2-192;
  m_nWinFlagsExcursionY = nHeight - 114;

  //区域位置
  int nCenterX=nWidth/2, nCenterY=LESS_SCREEN_CY/2, nBorderWidth=4;
  m_rcShunMen.left=nCenterX - 258-58;
  m_rcShunMen.top=nCenterY-227+15;
  m_rcShunMen.right=m_rcShunMen.left+177;
  m_rcShunMen.bottom=m_rcShunMen.top+181;

  m_rcJiaoL.left=m_rcShunMen.left-5;
  m_rcJiaoL.top=m_rcShunMen.bottom+3;
  m_rcJiaoL.right=m_rcJiaoL.left+177;
  m_rcJiaoL.bottom=m_rcJiaoL.top+110;

  m_rcQiao.left=nCenterX-126;
  m_rcQiao.top=nCenterY-140-94;
  m_rcQiao.right=m_rcQiao.left+240;
  m_rcQiao.bottom=m_rcQiao.top+140;

  m_rcDuiMen.left=m_rcQiao.left-7;
  m_rcDuiMen.top=m_rcQiao.bottom+3;
  m_rcDuiMen.right=m_rcQiao.left+248;
  m_rcDuiMen.bottom=m_rcJiaoL.top+140;

  m_rcDaoMen.left=nCenterX+128;
  m_rcDaoMen.top=nCenterY-212;
  m_rcDaoMen.right=m_rcDaoMen.left+177;
  m_rcDaoMen.bottom=m_rcDaoMen.top+181;

  m_rcJiaoR.left=m_rcDaoMen.left+5;
  m_rcJiaoR.top=m_rcDaoMen.bottom+3;
  m_rcJiaoR.right=m_rcJiaoR.left+177;
  m_rcJiaoR.bottom=m_rcJiaoR.top+110;

  //筹码数字
  int ExcursionY=10;
  m_PointJettonNumber[ID_SHUN_MEN-1].SetPoint((m_rcShunMen.right+m_rcShunMen.left)/2, (m_rcShunMen.bottom+m_rcShunMen.top)/2-ExcursionY);
  m_PointJettonNumber[ID_JIAO_L-1].SetPoint((m_rcJiaoL.right+m_rcJiaoL.left)/2, (m_rcJiaoL.bottom+m_rcJiaoL.top)/2-ExcursionY);
  m_PointJettonNumber[ID_QIAO-1].SetPoint((m_rcQiao.right+m_rcQiao.left)/2, (m_rcQiao.bottom+m_rcQiao.top)/2-ExcursionY);
  m_PointJettonNumber[ID_DUI_MEN-1].SetPoint((m_rcDuiMen.right+m_rcDuiMen.left)/2, (m_rcDuiMen.bottom+m_rcDuiMen.top)/2-ExcursionY);
  m_PointJettonNumber[ID_DAO_MEN-1].SetPoint((m_rcDaoMen.right+m_rcDaoMen.left)/2, (m_rcDaoMen.bottom+m_rcDaoMen.top)/2-ExcursionY);
  m_PointJettonNumber[ID_JIAO_R-1].SetPoint((m_rcJiaoR.right+m_rcJiaoR.left)/2, (m_rcJiaoR.bottom+m_rcJiaoR.top)/2-ExcursionY);

  //筹码位置
  m_PointJetton[ID_SHUN_MEN-1].SetPoint(m_rcShunMen.left, m_rcShunMen.top);
  m_PointJetton[ID_JIAO_L-1].SetPoint(m_rcJiaoL.left, m_rcJiaoL.top);
  m_PointJetton[ID_QIAO-1].SetPoint(m_rcQiao.left, m_rcQiao.top);
  m_PointJetton[ID_DUI_MEN-1].SetPoint(m_rcDuiMen.left, m_rcDuiMen.top);
  m_PointJetton[ID_DAO_MEN-1].SetPoint(m_rcDaoMen.left, m_rcDaoMen.top);
  m_PointJetton[ID_JIAO_R-1].SetPoint(m_rcJiaoR.left, m_rcJiaoR.top);

  //扑克控件
  m_CardControl[0].SetBenchmarkPos(CPoint(nWidth/2-8,LESS_SCREEN_CY/2-245),enXCenter,enYTop);
  m_CardControl[1].SetBenchmarkPos(CPoint(nWidth/2-310,LESS_SCREEN_CY/2-180),enXLeft,enYTop);
  m_CardControl[2].SetBenchmarkPos(CPoint(nWidth/2-8,LESS_SCREEN_CY/2-15),enXCenter,enYTop);
  m_CardControl[3].SetBenchmarkPos(CPoint(nWidth/2+164,LESS_SCREEN_CY/2-180),enXLeft,enYTop);
  m_CardControl[4].SetBenchmarkPos(CPoint(nWidth/2-44,LESS_SCREEN_CY/2-152),enXCenter,enYTop);

  /*BYTE card[2]={0x01,0x02};
  for (int i=0;i<4;i++)
  {
    m_CardControl[i].SetCardData(card,2);
    m_CardControl[i].m_blhideOneCard = false;
    m_CardControl[i].m_blGameEnd = true;
    m_CardControl[i].SetDisplayFlag(true);
    m_CardControl[i].OnCopyCard();
  }*/

  //用户列表位置
  m_RectUserList.SetRect(12,nHeight-150-5+14,12+203,nHeight-148-5+14+126);
  //聊天显示位置
  int startX = nWidth-m_ImageChatBack.GetWidth()+2;
  int startY = nHeight-126;
  m_RectChartDisplay.SetRect(startX,startY,startX+181,startY+91);

  //移动控件
  HDWP hDwp=BeginDeferWindowPos(32);
  const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

  //m_ApplyUser.m_viewHandle = m_hWnd;

  //列表控件
  //DeferWindowPos(hDwp,m_ApplyUser,NULL,nWidth/2 ,nHeight/2-292,210,52,uFlags);
  DeferWindowPos(hDwp,m_btUp,NULL,110 + 24,170,95/5,19,uFlags);
  DeferWindowPos(hDwp,m_btDown,NULL,110,170,95/5,19,uFlags);

  //筹码按钮
  CRect rcJetton;
  m_btJetton100.GetWindowRect(&rcJetton);
  int nYPos = 511;
  int nXPos = 189;
  int nSpace = 6;

  if(!IsZoomedMode())
  {
    DeferWindowPos(hDwp,m_btJetton100,NULL,nWidth/2 - rcJetton.Width()/2 - 6*3 - rcJetton.Width()*3 +6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton1000,NULL,nWidth/2 - rcJetton.Width()/2 - 6*2 - rcJetton.Width()*2+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton10000,NULL,nWidth/2 - rcJetton.Width()/2 - 6 - rcJetton.Width()+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton100000,NULL,nWidth/2 - rcJetton.Width()/2+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton500000,NULL,nWidth/2 + rcJetton.Width()/2 +6+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton1000000,NULL,nWidth/2 + rcJetton.Width()/2 +6*2 + rcJetton.Width()+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton5000000,NULL,nWidth/2 + rcJetton.Width()/2 +6*3 + rcJetton.Width()*2+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
  }
  else
  {
    DeferWindowPos(hDwp,m_btJetton100,NULL,nWidth/2 - rcJetton.Width()/2 - 26*3 - rcJetton.Width()*3+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton1000,NULL,nWidth/2 - rcJetton.Width()/2 - 26*2 - rcJetton.Width()*2+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton10000,NULL,nWidth/2 - rcJetton.Width()/2 - 26 - rcJetton.Width()+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton100000,NULL,nWidth/2 - rcJetton.Width()/2+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton500000,NULL,nWidth/2 + rcJetton.Width()/2 +26+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton1000000,NULL,nWidth/2 + rcJetton.Width()/2 +26*2 + rcJetton.Width()+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
    DeferWindowPos(hDwp,m_btJetton5000000,NULL,nWidth/2 + rcJetton.Width()/2 +26*3 + rcJetton.Width()*2+6,nHeight - 230,0,0,uFlags|SWP_NOSIZE);
  }





  //上庄按钮
  DeferWindowPos(hDwp,m_btApplyBanker,NULL,10,285,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btCancelBanker,NULL,10,285,0,0,uFlags|SWP_NOSIZE);

  DeferWindowPos(hDwp,m_btScoreMoveL,NULL,IsZoomedMode()?244+CalFrameGap():244,nHeight - 110,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btScoreMoveR,NULL,IsZoomedMode()?nWidth - 240-CalFrameGap():nWidth - 240,nHeight - 110,0,0,uFlags|SWP_NOSIZE);

  //开牌按钮
  DeferWindowPos(hDwp,m_btAutoOpenCard,NULL,nWidth-97,nHeight-226,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btOpenCard,NULL,nWidth-97,nHeight-192,0,0,uFlags|SWP_NOSIZE);

  //其他按钮
  DeferWindowPos(hDwp,m_btContinueCard,NULL,nWidth-97,nHeight-260,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btBankerStorage,NULL,nWidth/2-290,nHeight/2+143,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btBankerDraw,NULL,nWidth/2-210,nHeight/2+143,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btBank,NULL,17,nHeight - 247,0,0,uFlags|SWP_NOSIZE);

  //框架按钮
  //框架按钮
  DeferWindowPos(hDwp,m_btMin,NULL, nWidth - 10-25*4-5*3,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btMax,NULL, nWidth - 10-25*3-5*2,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btRestore,NULL, nWidth - 10-25*3-5*2,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btSound,NULL, nWidth - 10-25*2-5,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btClose,NULL, nWidth - 10-25,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btOption,NULL, nWidth - 10-25*5-5*4,2,0,0,uFlags|SWP_NOSIZE);

  DeferWindowPos(hDwp,m_UserList,NULL,m_RectUserList.left,m_RectUserList.top,m_RectUserList.Width(),m_RectUserList.Height(),uFlags);
  DeferWindowPos(hDwp,m_ChatDisplay,NULL,m_RectChartDisplay.left,m_RectChartDisplay.top,m_RectChartDisplay.Width()/*-16*/,m_RectChartDisplay.Height(),uFlags);

  DeferWindowPos(hDwp, m_ChatInput, NULL, nWidth-195, nHeight-31, 140, 20, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
  DeferWindowPos(hDwp,m_btSendChat,NULL,nWidth-53,nHeight-32,0,0,uFlags|SWP_NOSIZE);

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
  m_ImageViewFill.DrawImageTile(pDC,0,0,nWidth,nHeight);
  m_ImageViewBack.DrawImage(pDC, nWidth/2 - m_ImageViewBack.GetWidth()/2, 35);

  m_ImageWaitValleys.DrawImage(pDC, 10, 170);

  m_ImageBankerInfo.DrawImage(pDC,7,38);
  m_ImagePlayerInfo.DrawImage(pDC,nWidth - m_ImagePlayerInfo.GetWidth() ,38);



  INT nHistoryMidWidth = m_ImageHistoryMid.GetWidth(); // 30
  INT nHistoryMidHeight = m_ImageHistoryMid.GetHeight(); //157

  INT nHistoryFrontWidth = m_ImageHistoryFront.GetWidth(); //248
  INT nHistoryFrontHeight = m_ImageHistoryFront.GetHeight(); //157
  INT nHistoryBackWidth = m_ImageHistoryBack.GetWidth(); //238
  INT nHistoryBackHeight = m_ImageHistoryBack.GetHeight(); //157

  INT nUserBackWidth = m_ImageUserBack.GetWidth();
  INT nChatBackWidth = m_ImageChatBack.GetWidth();

  // 正常模式
  if(!IsZoomedMode())
  {
    m_ImageHistoryFront.DrawImage(pDC,5 + nUserBackWidth,nHeight - nHistoryFrontHeight);
    for(WORD i = 0; i < 4; i++)
    {
      m_ImageHistoryMid.DrawImage(pDC,5+ nUserBackWidth +nHistoryFrontWidth+ i*nHistoryMidWidth,nHeight - nHistoryMidHeight);
    }
    m_ImageHistoryBack.DrawImage(pDC,5 + nUserBackWidth + nHistoryFrontWidth + 4*nHistoryMidWidth ,nHeight - nHistoryBackHeight);
  }
  else // 全屏  20个标识
  {
    m_ImageHistoryFront.DrawImage(pDC,5 + nUserBackWidth + CalFrameGap(),nHeight - nHistoryFrontHeight);
    for(WORD i = 0; i < 9; i++)
    {
      m_ImageHistoryMid.DrawImage(pDC,5 + nUserBackWidth + CalFrameGap() + nHistoryFrontWidth + i*nHistoryMidWidth,nHeight - nHistoryMidHeight);
    }
    m_ImageHistoryBack.DrawImage(pDC,5 + nUserBackWidth + CalFrameGap() + nHistoryFrontWidth + 9*nHistoryMidWidth,nHeight - nHistoryBackHeight);

  }


//  DrawAlphaRect(pDC, CRect(nWidth/2 + 351, nHeight/2-318, nWidth/2 + 351 + 20, nHeight/2-250+20), RGB(0,0,0), 0.5);
  //pDC->Draw3dRect(m_rcJiaoR,RGB(255,255,255),RGB(255,255,255));
  //m_ImageFrameJiaoR.DrawImage(pDC,m_rcJiaoR.left-6, m_rcJiaoR.top);
  //获取状态
  BYTE cbGameStatus=m_pGameClientDlg->GetGameStatus();

  //状态提示
  CFont static InfoFont;
  InfoFont.CreateFont(-16,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,_TEXT("宋体"));
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
  else if(cbGameStatus==GS_PLACE_JETTON)
  {
    nFlagIndex=2;
  }
  else if(cbGameStatus==GS_GAME_END)
  {
    nFlagIndex=0;
  }
  m_ImageTimeFlag.DrawImage(pDC, nWidth/2, 85, nTimeFlagWidth, m_ImageTimeFlag.GetHeight(),nFlagIndex * nTimeFlagWidth, 0);

  //Dlg里面的SetGameClock(GetMeChairID()）里面调用的SwitchViewChairID在桌子人数大于8时实际是ViewID转化为0了
  if(m_wMeChairID!=INVALID_CHAIR)
  {
    WORD wUserTimer=GetUserClock(m_wMeChairID);
    if(wUserTimer!=0)
    {
      DrawTime(pDC, wUserTimer, nWidth/2-m_ImageTimeBack.GetWidth(), 45);
    }
  }

  //胜利边框
  FlashJettonAreaFrame(nWidth,nHeight,pDCBuff);

  //筹码资源
  CSize static SizeJettonItem(m_ImageJettonView.GetWidth()/7,m_ImageJettonView.GetHeight());

  LONGLONG lScoreCount1=0L;
  //绘画筹码
  for(INT i=0; i<AREA_COUNT; i++)
  {
    //变量定义
    LONGLONG lScoreCount=0L;
    LONGLONG static lScoreJetton[JETTON_COUNT]= {100L,1000L,10000L,100000L,500000L,1000000L,5000000L};
    int static nJettonViewIndex=0;

    //绘画筹码
    for(INT_PTR j=0; j<m_JettonInfoArray[i].GetCount(); j++)
    {
      //获取信息
      tagJettonInfo * pJettonInfo=&m_JettonInfoArray[i][j];

      /*if ( !pJettonInfo->bShow )
        continue;*/

      //累计数字
      ASSERT(pJettonInfo->cbJettonIndex<JETTON_COUNT);
      lScoreCount+=lScoreJetton[pJettonInfo->cbJettonIndex];
      lScoreCount1+=lScoreJetton[pJettonInfo->cbJettonIndex];

      //图片索引
      /*if (1==pJettonInfo->cbJettonIndex || 2==pJettonInfo->cbJettonIndex) nJettonViewIndex=pJettonInfo->cbJettonIndex+1;
      else if (0==pJettonInfo->cbJettonIndex) nJettonViewIndex=pJettonInfo->cbJettonIndex;
      else */nJettonViewIndex=pJettonInfo->cbJettonIndex;

      //绘画界面
      m_ImageJettonView.DrawImage(pDCBuff,pJettonInfo->nXPos+m_PointJetton[i].x,
                                  pJettonInfo->nYPos+m_PointJetton[i].y,SizeJettonItem.cx,SizeJettonItem.cy,
                                  nJettonViewIndex*SizeJettonItem.cx,0);
    }

    //绘画数字
    if(lScoreCount>0L)
    {
      DrawNumberString(pDCBuff,lScoreCount,m_PointJettonNumber[i].x,m_PointJettonNumber[i].y);
    }
  }
  m_TempScore=lScoreCount1;
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

  //作弊信息
  DrawCheatCard(nWidth, nHeight, pDCBuff);

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

  //结束状态
  //if (cbGameStatus==GS_GAME_END)
  {
    //绘画扑克
    for(int i=0; i<CountArray(m_CardControl); ++i)
    {
      m_CardControl[i].DrawCardControl(pDCBuff);
    }
  }

  //胜利标志
  DrawWinFlags(pDCBuff);

  if(m_blMoveFinish)
  {
    //显示结果
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
  dcBuffer.BitBlt(0, 0, 140, 20, pDC, nWidth-195, nHeight-31, SRCCOPY);
  dcBuffer.SelectObject(pBmpBuffer);
  m_brush.CreatePatternBrush(&bmpBuffer);
  m_ChatDisplay.Invalidate(TRUE);

  return;
}

//绘画框架
void CGameClientView::DrawGameFrame(CDC *pDC, int nWidth, int nHeight)
{
  m_ImageUserBack.DrawImage(pDC,5,nHeight-m_ImageUserBack.GetHeight()-7);
  m_ImageChatBack.DrawImage(pDC,nWidth-m_ImageChatBack.GetWidth()-5,nHeight-m_ImageChatBack.GetHeight()-7);
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
  CRect rectInfo(10, 8, 1024, 30);
  DrawTextString(pDC,m_strCaption,RGB(255,255,255),RGB(0,0,0),rectInfo, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_SINGLELINE|DT_LEFT);
}

// 绘画上庄列表
void CGameClientView::DrawBankerList(CDC *pDC, int nWidth, int nHeight)
{
  // 定义变量
  CPoint ptBegin(0,0);
  // 申请上庄列表
  ptBegin.SetPoint(10, 190);

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
    DrawNumberStringWithSpace(pDC,pIClientUserItem->GetUserScore(), rectScore,RGB(253,241,149) , RGB(38,26,2), DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

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
void CGameClientView::SetGameHistory(bool bWinShunMen, bool bWinDaoMen, bool bWinDuiMen)
{
  //设置数据
  tagClientGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
  GameRecord.bWinShunMen=bWinShunMen;
  GameRecord.bWinDuiMen=bWinDuiMen;
  GameRecord.bWinDaoMen=bWinDaoMen;

  //操作类型
  //操作类型
  if(0==m_lUserJettonScore[ID_SHUN_MEN])
  {
    GameRecord.enOperateShunMen=enOperateResult_NULL;
  }
  else if(m_lUserJettonScore[ID_SHUN_MEN] > 0 && true==bWinShunMen)
  {
    GameRecord.enOperateShunMen=enOperateResult_Win;
  }
  else if(m_lUserJettonScore[ID_SHUN_MEN] > 0 && false==bWinShunMen)
  {
    GameRecord.enOperateShunMen=enOperateResult_Lost;
  }

  if(0==m_lUserJettonScore[ID_DAO_MEN])
  {
    GameRecord.enOperateDaoMen=enOperateResult_NULL;
  }
  else if(m_lUserJettonScore[ID_DAO_MEN] > 0 && true==bWinDaoMen)
  {
    GameRecord.enOperateDaoMen=enOperateResult_Win;
  }
  else if(m_lUserJettonScore[ID_DAO_MEN] >0 && false==bWinDaoMen)
  {
    GameRecord.enOperateDaoMen=enOperateResult_Lost;
  }

  if(0==m_lUserJettonScore[ID_DUI_MEN])
  {
    GameRecord.enOperateDuiMen=enOperateResult_NULL;
  }
  else if(m_lUserJettonScore[ID_DUI_MEN] > 0 && true==bWinDuiMen)
  {
    GameRecord.enOperateDuiMen=enOperateResult_Win;
  }
  else if(m_lUserJettonScore[ID_DUI_MEN]>0 && false==bWinDuiMen)
  {
    GameRecord.enOperateDuiMen=enOperateResult_Lost;
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
  RefreshGameView();

  return;
}

//个人下注
void CGameClientView::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
  //效验参数
  ASSERT(cbViewIndex<=ID_JIAO_R);
  if(cbViewIndex>ID_JIAO_R)
  {
    return;
  }

  m_lUserJettonScore[cbViewIndex]=lJettonCount;

  //更新界面
  RefreshGameView();
}

//设置扑克
void CGameClientView::SetCardInfo(BYTE cbTableCardArray[4][2])
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
  ASSERT(cbViewIndex<=ID_JIAO_R);
  if(cbViewIndex>ID_JIAO_R)
  {
    return;
  }

  //变量定义
  bool bPlaceJetton=false;
  LONGLONG lScoreIndex[JETTON_COUNT]= {100L,1000L,10000L,100000L,500000L,1000000L,5000000L};

  //边框宽度
  int nFrameWidth=0, nFrameHeight=0;
  int nBorderWidth=6;

  switch(cbViewIndex)
  {
    case ID_SHUN_MEN:
    {
      m_lAllJettonScore[ID_SHUN_MEN] += lScoreCount;
      nFrameWidth = m_rcShunMen.right-m_rcShunMen.left;
      nFrameHeight = m_rcShunMen.bottom-m_rcShunMen.top;
      break;
    }
    case ID_JIAO_L:
    {
      m_lAllJettonScore[ID_JIAO_L] += lScoreCount;
      nFrameWidth = m_rcJiaoL.right-m_rcJiaoL.left;
      nFrameHeight = m_rcJiaoL.bottom-m_rcJiaoL.top;
      break;
    }
    case ID_QIAO:
    {
      m_lAllJettonScore[ID_QIAO] += lScoreCount;
      nFrameWidth = m_rcQiao.right-m_rcQiao.left;
      nFrameHeight = m_rcQiao.bottom-m_rcQiao.top;
      break;
    }
    case ID_DUI_MEN:
    {
      m_lAllJettonScore[ID_DUI_MEN] += lScoreCount;
      nFrameWidth = m_rcDuiMen.right-m_rcDuiMen.left;
      nFrameHeight = m_rcDuiMen.bottom-m_rcDuiMen.top;
      break;
    }
    case ID_DAO_MEN:
    {
      m_lAllJettonScore[ID_DAO_MEN] += lScoreCount;
      nFrameWidth = m_rcDaoMen.right-m_rcDaoMen.left;
      nFrameHeight = m_rcDaoMen.bottom-m_rcDaoMen.top;
      break;
    }
    case ID_JIAO_R:
    {
      m_lAllJettonScore[ID_JIAO_R] += lScoreCount;
      nFrameWidth = m_rcJiaoR.right-m_rcJiaoR.left;
      nFrameHeight = m_rcJiaoR.bottom-m_rcJiaoR.top;
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
  lScoreCount=(lScoreCount>0?lScoreCount:-lScoreCount);

  //增加筹码
  for(BYTE i=0; i<CountArray(lScoreIndex); i++)
  {
    //计算数目
    BYTE cbScoreIndex=JETTON_COUNT-i-1;
    LONGLONG nCellCount=LONGLONG(lScoreCount/lScoreIndex[cbScoreIndex]);

    //插入过虑
    if(nCellCount==0L)
    {
      continue;
    }

    //加入筹码
    for(LONGLONG j=0; j<nCellCount; j++)
    {
      if(true==bAddJetton)
      {
        //构造变量
        tagJettonInfo JettonInfo;
        int nJettonSize = 68;
        JettonInfo.cbJettonIndex = cbScoreIndex;
        JettonInfo.nXPos = rand()%(nFrameWidth-nJettonSize);
        JettonInfo.nYPos = rand()%(nFrameHeight-nJettonSize);
        JettonInfo.bShow = TRUE;

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
    lScoreCount-=nCellCount*lScoreIndex[cbScoreIndex];
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
                 pJettonInfo->nXPos+m_PointJetton[nIndex].x + m_ImageJettonView.GetWidth()/9,
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
    RefreshGameView();
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
void CGameClientView::SetWinnerSide(bool bWinShunMen, bool bWinDuiMen, bool bWinDaoMen, bool bSet)
{
  //设置时间
  if(true==bSet)
  {
    //设置定时器
    SetTimer(IDI_FLASH_WINNER,500,NULL);

    //全胜判断
    if(false==bWinShunMen && false==bWinDuiMen && false==bWinDaoMen)
    {
      //清除资源
      //m_ImageFrameShunMen.DestroyImage();
      //m_ImageFrameJiaoL.DestroyImage();
      //m_ImageFrameQiao.DestroyImage();
      //m_ImageFrameDuiMen.DestroyImage();
      //m_ImageFrameDaoMen.DestroyImage();
      //m_ImageFrameJiaoR.DestroyImage();

      ////重设资源
      //HINSTANCE hInstance=AfxGetInstanceHandle();
      //m_ImageFrameShunMen.LoadImage(this,hInstance,TEXT("FRAME_SHUN_MEN"));
      //m_ImageFrameJiaoL.LoadImage(this,hInstance,TEXT("FRAME_JIAO_L"));
      //m_ImageFrameQiao.LoadImage(this,hInstance,TEXT("FRAME_QIAO"));
      //m_ImageFrameDuiMen.LoadImage(this,hInstance,TEXT("FRAME_DUI_MEN"));
      //m_ImageFrameDaoMen.LoadImage(this,hInstance,TEXT("FRAME_DAO_EMN"));
      //m_ImageFrameJiaoR.LoadImage(this,hInstance,TEXT("FRAME_JIAO_R"));
    }
  }
  else
  {
    //清楚定时器
    KillTimer(IDI_FLASH_WINNER);

    //全胜判断
    if(false==bWinShunMen && false==bWinDuiMen && false==bWinDaoMen)
    {
      //清除资源
      //m_ImageFrameShunMen.DestroyImage();
      //m_ImageFrameJiaoL.DestroyImage();
      //m_ImageFrameQiao.DestroyImage();
      //m_ImageFrameDuiMen.DestroyImage();
      //m_ImageFrameDaoMen.DestroyImage();
      //m_ImageFrameJiaoR.DestroyImage();

      ////重设资源
      //HINSTANCE hInstance=AfxGetInstanceHandle();
      //m_ImageFrameShunMen.LoadImage(this,hInstance,TEXT("FRAME_SHUN_MEN_EX"));
      //m_ImageFrameJiaoL.LoadImage(this,hInstance,TEXT("FRAME_JIAO_L_EX"));
      //m_ImageFrameQiao.LoadImage(this,hInstance,TEXT("FRAME_QIAO_EX"));
      //m_ImageFrameDuiMen.LoadImage(this,hInstance,TEXT("FRAME_DUI_MEN_EX"));
      //m_ImageFrameDaoMen.LoadImage(this,hInstance,TEXT("FRAME_DAO_EMN_EX"));
      //m_ImageFrameJiaoR.LoadImage(this,hInstance,TEXT("FRAME_JIAO_R_EX"));
    }
  }

  //设置变量
  m_bWinShunMen=bWinShunMen;
  m_bWinDuiMen=bWinDuiMen;
  m_bWinDaoMen=bWinDaoMen;
  m_bFlashResult=bSet;
  m_bShowGameResult=bSet;
  m_cbAreaFlash=0xFF;

  //更新界面
  RefreshGameView();

  return;
}

//获取区域
BYTE CGameClientView::GetJettonArea(CPoint MousePoint)
{
  if(m_rcShunMen.PtInRect(MousePoint))
  {
    return ID_SHUN_MEN;
  }
  if(m_rcJiaoL.PtInRect(MousePoint))
  {
    return ID_JIAO_L;
  }
  if(m_rcQiao.PtInRect(MousePoint))
  {
    return ID_QIAO;
  }
  if(m_rcDuiMen.PtInRect(MousePoint))
  {
    return ID_DUI_MEN;
  }
  if(m_rcDaoMen.PtInRect(MousePoint))
  {
    return ID_DAO_MEN;
  }
  if(m_rcJiaoR.PtInRect(MousePoint))
  {
    return ID_JIAO_R;
  }

  return 0xFF;
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
  int nNumberCount=0;
  LONGLONG lNumberTemp=lNumber;
  do
  {
    nNumberCount++;
    lNumberTemp/=10;
  }
  while(lNumberTemp>0);

  //位置定义
  INT nYDrawPos=nYPos-SizeScoreNumber.cy/2;
  INT nXDrawPos=nXPos+nNumberCount*SizeScoreNumber.cx/2-SizeScoreNumber.cx;

  //绘画桌号
  for(int i=0; i<nNumberCount; i++)
  {
    //绘画号码
    int nCellNumber=(int)(lNumber%10);
    if(bMeScore)
    {
      m_ImageMeScoreNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
                                     (nCellNumber+1)*SizeScoreNumber.cx,0);
    }
    else
    {
      m_ImageScoreNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
                                   nCellNumber*SizeScoreNumber.cx,0);
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
  TextOut(pDC,nXPos,nYPos,strNumber,lstrlen(strNumber));
}

//绘画数字
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect,INT nFormat)
{
  CString strNumber, strTempNum;
  strTempNum.Format(_T("%I64d"), (lNumber>0?lNumber:-lNumber));

  int nLength = strTempNum.GetLength();
  if(nLength <= 3)
  {
    strNumber.Format(_TEXT("%I64d"), lNumber);
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
  if(nFormat==-1)
  {
    m_DFontExT.DrawText(pDC, strNumber, rcRect, RGB(255,234,0), DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
  }
  else
  {
    m_DFontExT.DrawText(pDC, strNumber, rcRect, RGB(255,234,0), nFormat);
  }
}

//绘画数字
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, COLORREF crText, COLORREF crFrame, INT nFormat)
{
  CString strNumber, strTempNum;
  strTempNum.Format(_T("%I64d"), (lNumber>0?lNumber:-lNumber));

  int nLength = strTempNum.GetLength();
  if(nLength <= 3)
  {
    strNumber.Format(_TEXT("%I64d"), lNumber);
  }
  else
  {
    for(int i = 0; i < int((nLength-1)/3); i++)
    {
      strTempNum.Insert(nLength - 3*(i+1), _T(","));
    }
    strNumber.Format(_T("%s%s"), (lNumber<0?_T("-"):_T("")), strTempNum);
  }

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
    CDFontEx::DrawText(this,pDC,  12, 400, strNumber, rcFrame, crFrame, nFormat);
  }

  //绘画字符
  CDFontEx::DrawText(this,pDC,  12, 400, strNumber, rcRect, crText, nFormat);
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

//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
  //闪动胜方
  if(nIDEvent==IDI_FLASH_WINNER)
  {
    //设置变量
    m_bFlashResult=!m_bFlashResult;

    //更新界面
    RefreshGameView();

    return;
  }
  if(IDI_POSTCARD ==nIDEvent)
  {

    if(m_bcShowCount<2)
    {
      if(m_CardControl[4].m_CardTempItemArray.GetCount()>0)
      {
        m_CardControl[4].m_blhideOneCard = false;
        m_CardControl[4].m_blGameEnd = true;
        m_CardControl[4].SetDisplayFlag(true);
        m_CardControl[4].ShowCardControl(true);
        m_CardControl[4].m_CardItemArray.SetSize(1);
        m_CardControl[4].m_CardItemArray[0]=m_CardControl[4].m_CardTempItemArray[0];
        m_CardControl[4].m_CardItemArray[0].cbCardData = m_bcfirstShowCard;
        m_CardControl[4].m_CardItemArray[0].bShoot = false;
      }

      m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),_TEXT("OUT_CARD"));
    }
    else
    {
      m_CardControl[4].m_CardItemArray.SetSize(0);

      bool blKillTimer = true;
      for(int i = 0; i<4; i++)
      {
        if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
        {
          blKillTimer = false;
          break;
        }
      }
      m_PostCardIndex = m_PostCardIndex%4;
      m_CardControl[m_PostCardIndex].OnCopyCard();
      m_PostCardIndex++;
      if(blKillTimer)
      {

        KillTimer(IDI_POSTCARD);
        SetTimer(IDI_OPENCARD,50,NULL);
      }
      else
      {
        m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),_TEXT("OUT_CARD"));
      }

      RefreshGameView();
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
        for(int i = 0; i<4; i++)
        {
          if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
          {
            blCopy = true;
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

      for(int i = 0; i<4; i++)
      {
        m_CardControl[i].m_blGameEnd = false;
        m_CardControl[i].m_blhideOneCard = false;
      }
      //更新界面
      RefreshGameView();
      KillTimer(IDI_OPENCARD);

      SetTimer(IDI_DISPATCH_CARD,10000,NULL);

      return ;

    }
    else
    {
      if(m_CardControl[m_OpenCardIndex%4].m_MovePoint.x<40&&m_CardControl[m_OpenCardIndex%4].m_blAdd)
      {
        m_CardControl[m_OpenCardIndex%4].m_blGameEnd = false;
        m_CardControl[m_OpenCardIndex%4].m_MovePoint.x+=2;
        m_CardControl[m_OpenCardIndex%4].m_blhideOneCard = false;
        //更新界面
        RefreshGameView();


      }
      else
      {

        m_CardControl[m_OpenCardIndex%4].m_blAdd = false;
        m_CardControl[m_OpenCardIndex%4].m_blGameEnd = false;
        if(m_CardControl[m_OpenCardIndex%4].m_MovePoint.x>10)
        {
          m_CardControl[m_OpenCardIndex%4].m_MovePoint.x-=2;
        }
        m_CardControl[m_OpenCardIndex%4].m_blhideOneCard = false;
        RefreshGameView();
        if(m_CardControl[m_OpenCardIndex%4].m_MovePoint.x<=10)
        {
          m_CardControl[m_OpenCardIndex%4].m_tMoveCard.m_blMoveCard = true;
          m_CardControl[m_OpenCardIndex%4].m_tMoveCard.m_blMoveFinish = true;
          m_CardControl[m_OpenCardIndex%4].m_MovePoint.x = 0;
          m_CardControl[m_OpenCardIndex%4].m_MovePoint.y = 0;
          m_CardControl[m_OpenCardIndex%4].m_blAdd = true;
          m_CardControl[m_OpenCardIndex%4].m_blGameEnd = true;
          m_OpenCardIndex++;
          if(m_OpenCardIndex==5)
          {
            for(int i = 0; i<4; i++)
            {
              m_CardControl[i].m_blGameEnd = true;
            }
            KillTimer(IDI_OPENCARD);
            RefreshGameView();
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
    {
      for(int i = 0; i<4; i++)
      {
        m_CardControl[i].m_blGameEnd = true;
        m_CardControl[i].m_blhideOneCard = false;
      }
      KillTimer(IDI_OPENCARD);
      RefreshGameView();

    }
    while(1)
    {
      bool blCopy = false;
      for(int i = 0; i<4; i++)
      {
        if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
        {
          blCopy = true;
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
    m_blMoveFinish = true;
    //更新界面
    RefreshGameView();

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
  //m_lCurrentJetton = 100;
  for(int i = 0; i<4; i++)
  {
    if(!m_CardControl[i].m_blAuto)
    {
      m_CardControl[i].OnLButtonDown(nFlags, Point);
    }
  }

  if(m_lCurrentJetton!=0L)
  {
    //下注区域
    BYTE cbJettonArea=GetJettonArea(Point);

    if(cbJettonArea >= 1 && cbJettonArea <= AREA_COUNT)
    {
      //最大下注
      LONGLONG lMaxJettonScore=GetUserMaxJetton(cbJettonArea);

      //合法判断
      if(lMaxJettonScore < m_lCurrentJetton)
      {
        return ;
      }

      SendEngineMessage(IDM_PLACE_JETTON,cbJettonArea,0);
    }
  }

  RefreshGameView();

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
  for(int i = 0; i<4; i++)
  {
    if(!m_CardControl[i].m_blAuto)
    {
      m_CardControl[i].OnLButtonUp(nFlags, Point);
      RefreshGameView();
    }

  }
  __super::OnLButtonUp(nFlags,Point);
}

//接受其他控件传来的消息
//LRESULT CGameClientView::OnViLBtUp(WPARAM wParam, LPARAM lParam)
//{
//  CPoint *pPoint = (CPoint*)lParam;
//  ScreenToClient(pPoint);
//  OnLButtonUp(1,*pPoint);
//  return 1;
//
//}

//鼠标移动消息
void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
  for(int i = 0; i<4; i++)
  {
    if(!m_CardControl[i].m_blAuto)
    {
      m_CardControl[i].OnMouseMove(nFlags, point);
      RefreshGameView();

    }

  }
  __super::OnMouseMove(nFlags,point);
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

//鼠标消息
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint Point)
{
  //设置变量
  m_lCurrentJetton=0L;

  if(m_pGameClientDlg->GetGameStatus()!=GS_GAME_END && m_cbAreaFlash!=0xFF)
  {
    m_cbAreaFlash=0xFF;
    RefreshGameView();
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
      RefreshGameView();
    }

    //区域判断
    if(cbJettonArea==0xFF)
    {
      SetCursor(LoadCursor(NULL,IDC_ARROW));
      return TRUE;
    }

    //最大下注
    LONGLONG lMaxJettonScore=GetUserMaxJetton(cbJettonArea);

    LONGLONG lLeaveScore=lMaxJettonScore;

    //设置光标
    if(m_lCurrentJetton>lLeaveScore)
    {
      if(lLeaveScore>=5000000L)
      {
        SetCurrentJetton(5000000L);
      }
      else if(lLeaveScore>=1000000L)
      {
        SetCurrentJetton(1000000L);
      }
      else if(lLeaveScore>=500000L)
      {
        SetCurrentJetton(500000L);
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
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat)
{
  //变量定义
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
//庄家信息
void CGameClientView::SetBankerInfo(DWORD dwBankerUserID, LONGLONG lBankerScore)
{
  //庄家椅子号
  WORD wBankerUser=INVALID_CHAIR;

  //查找椅子号
  //if (0!=dwBankerUserID)
  {
    for(WORD wChairID=0; wChairID<MAX_CHAIR; ++wChairID)
    {
      IClientUserItem *pClientUserItem=GetClientUserItem(wChairID);
      if(NULL!=pClientUserItem && dwBankerUserID==pClientUserItem->GetChairID())
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
  //m_nRecordLast = 15;
  //非空判断
  if(m_nRecordLast==m_nRecordFirst)
  {
    return;
  }

  //资源变量
  int nIndex = m_nScoreHead;
  COLORREF static clrOld ;

  clrOld = pDC->SetTextColor(RGB(255,234,0));
  int nDrawCount = 0;
  int nDrawTotalHorCount = IsZoomedMode() ? ZOOMED_MAX_FALG_COUNT : NORMAL_MAX_FALG_COUNT;
  int xOffset = 0;
  // 全屏标识向右平移
  if(IsZoomedMode())
  {
    xOffset = CalFrameGap();
  }

  while(nIndex != m_nRecordLast && (m_nRecordLast!=m_nRecordFirst) && nDrawCount < nDrawTotalHorCount)
  {
    //胜利标识
    tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
    bool static bWinMen[3];
    bWinMen[0]=ClientGameRecord.bWinShunMen;
    bWinMen[1]=ClientGameRecord.bWinDaoMen;
    bWinMen[2]=ClientGameRecord.bWinDuiMen;

    //操作结果
    enOperateResult static OperateResult[3];
    OperateResult[0]=ClientGameRecord.enOperateShunMen;
    OperateResult[1]=ClientGameRecord.enOperateDaoMen;
    OperateResult[2]=ClientGameRecord.enOperateDuiMen;

    for(int i=0; i<3; ++i)
    {
      //位置变量
      int static nYPos=0,nXPos=0;
      nYPos=m_nWinFlagsExcursionY+i*31;
      nXPos=m_nWinFlagsExcursionX + ((nIndex - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY) * 30;

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
      m_ImageWinFlags.DrawImage(pDC, nXPos + xOffset, nYPos, m_ImageWinFlags.GetWidth()/4 , m_ImageWinFlags.GetHeight(),m_ImageWinFlags.GetWidth()/4 * nFlagsIndex, 0);
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
  SendEngineMessage(IDM_OPEN_CARD,0,0);
}

//自动搓牌
afx_msg void CGameClientView::OnAutoOpenCard()
{
  SendEngineMessage(IDM_AUTO_OPEN_CARD,0,0);
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
  RefreshGameView();
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

  int nXPos = nWidth / 2 - 175;
  int nYPos = LESS_SCREEN_CY / 2 - 230;

  //CRect rcAlpha(nXPos+2, nYPos+70, nXPos+2 + m_ImageGameEnd.GetWidth(), nYPos+70+m_ImageGameEnd.GetHeight());
  //DrawAlphaRect(pDC, &rcAlpha, RGB(74,70,73), 0.8f);
  m_ImageGameEnd.DrawImage(pDC, nXPos+2, nYPos+70);

  pDC->SetTextColor(RGB(255,234,0));
  CRect rcMeWinScore, rcMeReturnScore;
  rcMeWinScore.left = nXPos+80;
  rcMeWinScore.top = nYPos+100 + 22;
  rcMeWinScore.right = rcMeWinScore.left + 111;
  rcMeWinScore.bottom = rcMeWinScore.top + 34;

  rcMeReturnScore.left = nXPos+210;
  rcMeReturnScore.top = nYPos+100 + 22;
  rcMeReturnScore.right = rcMeReturnScore.left + 111;
  rcMeReturnScore.bottom = rcMeReturnScore.top + 34;

  CString strMeGameScore, strMeReturnScore;
  DrawNumberStringWithSpace(pDC,m_lMeCurGameScore,rcMeWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
  DrawNumberStringWithSpace(pDC,m_lMeCurGameReturnScore,rcMeReturnScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

  CRect rcBankerWinScore;
  rcBankerWinScore.left = nXPos+80;
  rcBankerWinScore.top = nYPos+100 + 57;
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
LONGLONG CGameClientView::GetUserMaxJetton(BYTE cbArea/* = 0xFF*/)
{
  //变量定义
  LONGLONG lMeMaxScore = 0L;

  //已下注额
  LONGLONG lNowJetton = 0;
  ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lNowJetton += m_lUserJettonScore[nAreaIndex];
  }

  //庄家金币
  LONGLONG lBankerScore=2147483647;
  if(m_wBankerUser!=INVALID_CHAIR)
  {
    lBankerScore=m_lBankerScore*m_nEndGameMul/100;
  }

  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lBankerScore-=m_lAllJettonScore[nAreaIndex];
  }

  //区域限制
  LONGLONG lAreaLimitScore = 0;
  if(cbArea == 0xFF)
  {
    for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
    {
      LONGLONG lAreaLeaveScore = m_lAreaLimitScore-m_lAllJettonScore[nAreaIndex];
      if(lAreaLeaveScore > lAreaLimitScore)
      {
        lAreaLimitScore = lAreaLeaveScore;
      }
    }
  }
  else
  {
    lAreaLimitScore = m_lAreaLimitScore - m_lAllJettonScore[cbArea];
  }

  lMeMaxScore=min(m_lMeMaxScore-lNowJetton,lAreaLimitScore);

  //庄家限制
  lMeMaxScore=min(lMeMaxScore,lBankerScore);

  //非零限制
  ASSERT(lMeMaxScore >= 0);
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
  bool bWinShunMen, bWinDuiMen, bWinDaoMen;
  DeduceWinner(bWinShunMen, bWinDuiMen, bWinDaoMen);

  //保存记录
  if(bNotScene)
  {
    SetGameHistory(bWinShunMen, bWinDaoMen, bWinDuiMen);
  }

  //累计积分
  m_lMeStatisticScore+=m_lMeCurGameScore;
  m_lBankerWinScore=m_lTmpBankerWinScore;

  //设置赢家
  SetWinnerSide(bWinShunMen, bWinDuiMen, bWinDaoMen, true);
  InsertGameEndInfo();

  //播放声音
  if(m_lMeCurGameScore>0)
  {
    m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),_TEXT("END_WIN"));
  }
  else if(m_lMeCurGameScore<0)
  {
    m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),_TEXT("END_LOST"));
  }
  else
  {
    m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),_TEXT("END_DRAW"));
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
    switch(m_cbAreaFlash)
    {
      case ID_SHUN_MEN:
      {
        //绘画图片
        m_ImageFrameShunMen.DrawImage(pDC,m_rcShunMen.left-67, m_rcShunMen.top-110);
        break;
      }
      case ID_JIAO_L:
      {
        //绘画图片
        m_ImageFrameJiaoL.DrawImage(pDC,m_rcJiaoL.left-58, m_rcJiaoL.top);
        break;
      }
      case ID_QIAO:
      {
        //绘画图片
        m_ImageFrameQiao.DrawImage(pDC,m_rcQiao.left-7, m_rcQiao.top-93);
        break;
      }
      case ID_DUI_MEN:
      {
        //绘画图片
        m_ImageFrameDuiMen.DrawImage(pDC,m_rcDuiMen.left-13, m_rcDuiMen.top);
        break;
      }
      case ID_DAO_MEN:
      {
        //绘画图片
        m_ImageFrameDaoMen.DrawImage(pDC,m_rcDaoMen.left-15, m_rcDaoMen.top-115);
        break;
      }
      case ID_JIAO_R:
      {
        //绘画图片
        m_ImageFrameJiaoR.DrawImage(pDC,m_rcJiaoR.left-6, m_rcJiaoR.top);
        break;
      }
    }
  }
  else
  {
    //庄全胜判断
    if(false==m_bWinDaoMen && false==m_bWinShunMen && false==m_bWinDuiMen)
    {
      //绘画图片
      /*m_ImageFrameShunMen.DrawImage(pDC,m_rcShunMen.left, m_rcShunMen.top);
      m_ImageFrameJiaoL.DrawImage(pDC,m_rcJiaoL.left, m_rcJiaoL.top);
      m_ImageFrameQiao.DrawImage(pDC,m_rcQiao.left, m_rcQiao.top);
      m_ImageFrameDuiMen.DrawImage(pDC,m_rcDuiMen.left, m_rcDuiMen.top);
      m_ImageFrameDaoMen.DrawImage(pDC,m_rcDaoMen.left, m_rcDaoMen.top);
      m_ImageFrameJiaoR.DrawImage(pDC,m_rcJiaoR.left, m_rcJiaoR.top);*/
    }
    else
    {
      //门判断
      if(true==m_bWinDaoMen)
      {
        m_ImageFrameDaoMen.DrawImage(pDC,m_rcDaoMen.left-15, m_rcDaoMen.top-115);
      }
      if(true==m_bWinShunMen)
      {
        m_ImageFrameShunMen.DrawImage(pDC,m_rcShunMen.left-67, m_rcShunMen.top-110);
      }
      if(true==m_bWinDuiMen)
      {
        m_ImageFrameDuiMen.DrawImage(pDC,m_rcDuiMen.left-13, m_rcDuiMen.top);
      }

      //角判断
      if(true==m_bWinShunMen && true==m_bWinDuiMen)
      {
        m_ImageFrameJiaoL.DrawImage(pDC,m_rcJiaoL.left-58, m_rcJiaoL.top);
      }
      if(true==m_bWinShunMen && true==m_bWinDaoMen)
      {
        m_ImageFrameQiao.DrawImage(pDC,m_rcQiao.left-7, m_rcQiao.top-93);
      }
      if(true==m_bWinDuiMen && true==m_bWinDaoMen)
      {
        m_ImageFrameJiaoR.DrawImage(pDC,m_rcJiaoR.left-6, m_rcJiaoR.top);
      }
    }
  }
}


//推断赢家
void CGameClientView::DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen)
{
  //大小比较
  bWinShunMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[SHUN_MEN_INDEX],2)==1?true:false;
  bWinDuiMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DUI_MEN_INDEX],2)==1?true:false;
  bWinDaoMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DAO_MEN_INDEX],2)==1?true:false;
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
      CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
      if(pGameClientEngine->GetGameStatus()!=GAME_SCENE_FREE)
      {
        //构造数据
        CMessageTipDlg GameTips;

        //配置数据
        if(GameTips.DoModal()==IDOK)
        {
          AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_CLOSE);
        }
      }
      else
      {
        AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_CLOSE);
      }
      break;
    }
    case IDC_BANK:
    {
      OnBankDraw();
      //OnButtonGoBanker();
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
  }

  return CGameFrameViewGDI::OnCommand(wParam, lParam);
}

//我的位置
void CGameClientView::SetMeChairID(WORD dwMeUserID)
{
  //查找椅子号
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
  RefreshGameView();
}

//绘画庄家
void CGameClientView::DrawBankerInfo(CDC *pDC,int nWidth,int nHeight)
{
  //庄家信息
  pDC->SetTextColor(RGB(255,234,0));

  //获取玩家
  IClientUserItem  *pClientUserItem = m_wBankerUser==INVALID_CHAIR ? NULL : GetClientUserItem(m_wBankerUser);

  //位置信息
  CRect static StrRect;
  StrRect.left = 63;
  StrRect.top = 60;
  StrRect.right = StrRect.left + 122;
  StrRect.bottom = StrRect.top + 15;

  //庄家名字
  m_DFontExT.DrawText(pDC, pClientUserItem==NULL?(m_bEnableSysBanker?_TEXT("系统坐庄"):_TEXT("无人坐庄")):pClientUserItem->GetNickName(), StrRect, RGB(255,234,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE);

  //庄家总分
  //StrRect.left = nWidth/2-293;
  StrRect.top = 81;
  //StrRect.right = StrRect.left + 122;
  StrRect.bottom = StrRect.top + 15;
  DrawNumberStringWithSpace(pDC,pClientUserItem==NULL?0:pClientUserItem->GetUserScore(), StrRect);

  //庄家成绩
  //StrRect.left = nWidth/2-293;
  StrRect.top = 102;
  //StrRect.right = StrRect.left + 122;
  StrRect.bottom = StrRect.top + 15;
  DrawNumberStringWithSpace(pDC,m_lBankerWinScore,StrRect);

  //庄家局数
  //StrRect.left = nWidth/2-293;
  StrRect.top = 123;
  //StrRect.right = StrRect.left + 122;
  StrRect.bottom = StrRect.top + 15;
  DrawNumberStringWithSpace(pDC,m_wBankerTime,StrRect);
}

void CGameClientView::SetFirstShowCard(BYTE bcCard)
{
  m_bcfirstShowCard =  bcCard;
  m_CardControl[4].SetCardData(NULL,0);
  m_CardControl[4].SetCardData(&bcCard,1);
  m_CardControl[4].m_blGameEnd = true;
  m_CardControl[4].SetDisplayFlag(true);
  m_CardControl[4].ShowCardControl(true);
}

//设置作弊数据
void CGameClientView::SetCheatInfo(BYTE bCardData[], int nWinInfo[])
{
  //清理数据
  if(bCardData == NULL || nWinInfo == NULL)
  {
    m_bShowCheatInfo = false;
    ZeroMemory(m_bCheatCard, sizeof(m_bCheatCard));
    ZeroMemory(m_nWinInfo, sizeof(m_nWinInfo));
  }
  else
  {
    m_bShowCheatInfo = true;
    memcpy(m_bCheatCard, bCardData, sizeof(m_bCheatCard));
    memcpy(m_nWinInfo, nWinInfo, sizeof(m_nWinInfo));
  }

  RefreshGameView();
}

//更新视图
void CGameClientView::RefreshGameView()
{
  CRect rect;
  GetClientRect(&rect);
  InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

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

//绘画玩家
void CGameClientView::DrawMeInfo(CDC *pDC,int nWidth,int nHeight)
{
  //合法判断
  if(INVALID_CHAIR==m_wMeChairID)
  {
    return;
  }

  IClientUserItem *pMeUserData = GetClientUserItem(m_wMeChairID);
  if(pMeUserData != NULL)
  {
    //字体颜色
    pDC->SetTextColor(RGB(255,234,0));

    //总共下注
    LONGLONG lMeJetton=0L;
    for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
    {
      lMeJetton += m_lUserJettonScore[nAreaIndex];
    }

    //设置位置
    CRect static rcDraw;
    rcDraw.left=nWidth-137;
    rcDraw.top=57;
    rcDraw.right=rcDraw.left+140;
    rcDraw.bottom=rcDraw.top+15;

    //绘画帐号
    m_DFontExT.DrawText(pDC, pMeUserData->GetNickName(), rcDraw, RGB(255,234,0), DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

    //绘画积分
    rcDraw.top=78;
    rcDraw.bottom=rcDraw.top+15;
    DrawNumberStringWithSpace(pDC,pMeUserData->GetUserScore()-lMeJetton,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

    //绘画成绩
    rcDraw.top=99;
    rcDraw.bottom=rcDraw.top+15;
    DrawNumberStringWithSpace(pDC,m_lMeStatisticScore,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

    //绘画下注
    rcDraw.top=120;
    rcDraw.bottom=rcDraw.top+15;
    DrawNumberStringWithSpace(pDC,lMeJetton,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
  }
}

//作弊扑克
void CGameClientView::DrawCheatCard(int nWidth, int nHeight, CDC *pDC)
{
  if(!m_bShowCheatInfo)
  {
    return;
  }


  //变量定义
  int nXPos[4] = {nWidth/2-48, nWidth/2-360, nWidth/2-48, nWidth/2+265};
  int nYPos[4] = {nHeight/2-340, nHeight/2-152, nHeight/2+45, nHeight/2-152};
  int nXImagePos = 0, nYImagePos = 0;
  CSize cardSize(CCardControl::m_ImageCard.GetWidth()/13, CCardControl::m_ImageCard.GetHeight()/5);

  //绘画扑克
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 2; j++)
    {
      BYTE cbCardData = m_bCheatCard[i][j];
      if(cbCardData == 0)
      {
        continue;
      }

      nXImagePos = ((cbCardData&CARD_MASK_VALUE)-1)*cardSize.cx;
      nYImagePos = ((cbCardData&CARD_MASK_COLOR)>>4)*cardSize.cy;

      CCardControl::m_ImageCard.DrawImage(pDC,nXPos[i]+j*DEF_X_DISTANCE,nYPos[i],
                                          cardSize.cx,cardSize.cy,nXImagePos,nYImagePos);
    }
  }

  //绘画边框
  for(int i = 0; i < 3; i++)
  {
    if(m_nWinInfo[i] < 0)
    {
      pDC->Draw3dRect(nXPos[i+1]-1, nYPos[i+1]-1, cardSize.cx+DEF_X_DISTANCE+2, cardSize.cy+2, RGB(255,255,0), RGB(255,255,0));
      pDC->Draw3dRect(nXPos[i+1]-2, nYPos[i+1]-2, cardSize.cx+DEF_X_DISTANCE+4, cardSize.cy+4, RGB(255,255,0), RGB(255,255,0));
    }
    else if(m_nWinInfo[i] > 0)
    {
      pDC->Draw3dRect(nXPos[i+1]-1, nYPos[i+1]-1, cardSize.cx+DEF_X_DISTANCE+2, cardSize.cy+2, RGB(255,0,0), RGB(255,0,0));
      pDC->Draw3dRect(nXPos[i+1]-2, nYPos[i+1]-2, cardSize.cx+DEF_X_DISTANCE+4, cardSize.cy+4, RGB(255,0,0), RGB(255,0,0));
    }
    else
    {
      pDC->Draw3dRect(nXPos[i+1]-1, nYPos[i+1]-1, cardSize.cx+DEF_X_DISTANCE+2, cardSize.cy+2, RGB(0,255,255), RGB(0,255,255));
      pDC->Draw3dRect(nXPos[i+1]-2, nYPos[i+1]-2, cardSize.cx+DEF_X_DISTANCE+4, cardSize.cy+4, RGB(0,255,255), RGB(0,255,255));
    }
  }
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
//////////////////////////////////////////////////////////////////////////
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
//银行存款
void CGameClientView::OnBankStorage()
{
  //获取接口
  CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
  IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

  if(NULL!=pIClientKernel)
  {
    CRect btRect;
    m_btBankerStorage.GetWindowRect(&btRect);
    ShowInsureSave(pIClientKernel,CPoint(btRect.right,btRect.top));
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
    CRect rcClient;
    GetClientRect(&rcClient);
    CPoint pt;
    pt.SetPoint((rcClient.Width() - 490)/2,(rcClient.Height() - 334)/2 + 334);
    ClientToScreen(&pt);
    ShowInsureGet(pIClientKernel,pt);
  }
}

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

//允许控制
void CGameClientView::AllowControl(BYTE cbStatus)
{
  if(m_pClientControlDlg != NULL && m_pClientControlDlg->m_hWnd!=NULL && m_hInst)
  {
    bool bEnable=false;
    switch(cbStatus)
    {
      case GAME_SCENE_FREE:
        bEnable=true;
        break;
      case GS_GAME_END:
        bEnable=false;
        break;
      case GS_PLACE_JETTON:
        bEnable=true;
        break;
      default:
        bEnable=false;
        break;
    }
    m_pClientControlDlg->OnAllowControl(bEnable);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
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
  INT nGap = GetSystemMetrics(SM_CXSCREEN) - (ZOOMED_MAX_FALG_COUNT - 11)*m_ImageHistoryMid.GetWidth() -
             m_ImageUserBack.GetWidth()-m_ImageChatBack.GetWidth() - 10 - m_ImageHistoryFront.GetWidth() - m_ImageHistoryBack.GetWidth();
  return nGap/2;
}


