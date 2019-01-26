#include "StdAfx.h"
#include "Math.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
#include ".\gameclientview.h"
#include "GameFrameWindow.h"
//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_FLASH_WINNER      100                 //������ʶ
#define IDI_SHOW_CHANGE_BANKER    101                 //�ֻ�ׯ��
#define IDI_DISPATCH_CARD     102                 //���Ʊ�ʶ
#define IDI_SHOWDISPATCH_CARD_TIP 103                 //������ʾ
#define IDI_OPENCARD        104                 //������ʾ
#define IDI_MOVECARD_END      105                 //�Ƅ��ƽY��
#define IDI_POSTCARD        106                 //������ʾ
#define IDC_MAX           300                 //���ť
#define IDC_RESTORE         301                 //�ָ���ť

//��ť��ʶ
#define IDC_JETTON_BUTTON_100   200                 //��ť��ʶ
#define IDC_JETTON_BUTTON_1000    201                 //��ť��ʶ
#define IDC_JETTON_BUTTON_10000   202                 //��ť��ʶ
#define IDC_JETTON_BUTTON_100000  203                 //��ť��ʶ
#define IDC_JETTON_BUTTON_1000000 204                 //��ť��ʶ
#define IDC_JETTON_BUTTON_5000000 205                 //��ť��ʶ
#define IDC_APPY_BANKER       206                 //��ť��ʶ
#define IDC_CANCEL_BANKER     207                 //��ť��ʶ
#define IDC_SCORE_MOVE_L      209                 //��ť��ʶ
#define IDC_SCORE_MOVE_R      210                 //��ť��ʶ
#define IDC_VIEW_CHART        211                 //��ť��ʶ
#define IDC_JETTON_BUTTON_500000  212                 //��ť��ʶ
#define IDC_AUTO_OPEN_CARD      213                 //��ť��ʶ
#define IDC_OPEN_CARD       214                 //��ť��ʶ
#define IDC_CONTINUE_CARD     216                 //��ť��ʶ
#define IDC_UP            223                 //��ť��ʶ
#define IDC_DOWN          224                 //��ť��ʶ
#define IDC_OPTION          225                 //���ð�ť��ʶ
#define IDC_MIN           226                 //���ð�ť��ʶ
#define IDC_CLOSE         227                 //���ð�ť��ʶ
#define IDC_BANK          228                 //��ť��ʶ
#define IDC_CHAT_DISPLAY      229                 //�ؼ���ʶ
#define IDC_USER_LIST       230                 //�ؼ���ʶ
#define IDC_SOUND         231                 //�������
#define IDC_SEND_CHAT       232                 //���Ͱ�ť
#define IDC_CHAT_INPUT        233                 //�ؼ���ʶ
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

//���캯��
CGameClientView::CGameClientView()
{
  //��ע��Ϣ
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //ȫ����ע
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));
  m_nEndGameMul = 0;

  //ׯ����Ϣ
  m_wBankerUser=INVALID_CHAIR;
  m_wBankerTime=0;
  m_lBankerScore=0L;
  m_lBankerWinScore=0L;
  m_lTmpBankerWinScore=0;
  m_TempScore=0;
  m_nShowValleyIndex = 0;

  //���ֳɼ�
  m_lMeCurGameScore=0L;
  m_lMeCurGameReturnScore=0L;
  m_lBankerCurGameScore=0L;
  m_lGameRevenue=0L;

  //״̬��Ϣ
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

  //���ױ���
  m_bShowCheatInfo = false;
  ZeroMemory(m_bCheatCard, sizeof(m_bCheatCard));
  ZeroMemory(m_nWinInfo, sizeof(m_nWinInfo));

  //λ����Ϣ
  m_nScoreHead = 0;
  m_nRecordFirst= 0;
  m_nRecordLast= 0;

  //��ʷ�ɼ�
  m_lMeStatisticScore=0;

  //�ؼ�����
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

//��������
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

//��Ϣ����
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

//������Ϣ
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(__super::OnCreate(lpCreateStruct)==-1)
  {
    return -1;
  }

  AfxSetResourceHandle(AfxGetInstanceHandle());

  //����λͼ
  HINSTANCE hInstance=AfxGetInstanceHandle();
  m_ImageViewFill.LoadFromResource(hInstance,IDB_VIEW_FILL);
  m_ImageViewBack.LoadImage(hInstance,TEXT("VIEW_BACK"));
  m_ImageWinFlags.LoadImage(hInstance,TEXT("WIN_FLAGS"));
  m_ImageJettonView.LoadImage(this,hInstance,TEXT("JETTOM_VIEW"));
  m_ImageScoreNumber.LoadImage(this,hInstance,TEXT("SCORE_NUMBER"));
  m_ImageMeScoreNumber.LoadImage(this,hInstance,TEXT("ME_SCORE_NUMBER"));
  m_ImageWaitValleys.LoadImage(hInstance,TEXT("WAIT_VALLEYS"));

  //�߿���Դ
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

  //�����ؼ�
  CRect rcCreate(0,0,0,0);
  m_GameRecord.Create(IDD_DLG_GAME_RECORD,this);
  //m_ApplyUser.Create( IDD_DLG_GAME_RECORD,this );
  m_GameRecord.ShowWindow(SW_HIDE);

  //��ע��ť
  m_btJetton100.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100);
  m_btJetton1000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000);
  m_btJetton10000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_10000);
  m_btJetton100000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100000);
  m_btJetton500000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_500000);
  m_btJetton1000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000000);
  m_btJetton5000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_5000000);

  //���밴ť
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

  //��ܰ�ť
  m_btOption.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OPTION);
  m_btMin.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MIN);
  m_btClose.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_CLOSE);
  m_btSound.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SOUND);

  m_btSendChat.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SEND_CHAT);

  //���ð�ť
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

  //�û���־
  m_UserList.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,35,0);
  m_UserList.InsertColumn(nColumnCount++,TEXT("�ǳ�"),LVCFMT_LEFT,75);
  m_UserList.InsertColumn(nColumnCount++,TEXT("�Ƹ�"),LVCFMT_LEFT,95);

  //�˿˿ؼ�
  for(int i=0; i<CountArray(m_CardControl); ++i)
  {
    m_CardControl[i].SetDisplayFlag(true);
  }

  //����
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

  m_DFontExT.CreateFont(this, TEXT("����"), 12, 400);

  m_brush.CreateSolidBrush(RGB(255,0,255));

  return 0;
}

//���ý���
VOID CGameClientView::ResetGameView()
{
  //��ע��Ϣ
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //ȫ����ע
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

  //ׯ����Ϣ
  m_wBankerUser=INVALID_CHAIR;
  m_wBankerTime=0;
  m_lBankerScore=0L;
  m_lBankerWinScore=0L;
  m_lTmpBankerWinScore=0;
  m_TempScore=0;
  m_nShowValleyIndex = 0;

  //���ֳɼ�
  m_lMeCurGameScore=0L;
  m_lMeCurGameReturnScore=0L;
  m_lBankerCurGameScore=0L;
  m_lGameRevenue=0L;

  //״̬��Ϣ
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

  //���ױ���
  m_bShowCheatInfo = false;
  ZeroMemory(m_bCheatCard, sizeof(m_bCheatCard));
  ZeroMemory(m_nWinInfo, sizeof(m_nWinInfo));

  //λ����Ϣ
  m_nScoreHead = 0;
  m_nRecordFirst= 0;
  m_nRecordLast= 0;

  //��ʷ�ɼ�
  m_lMeStatisticScore=0;

  //����б�
  m_ValleysList.RemoveAll();
  m_btUp.EnableWindow(false);
  m_btDown.EnableWindow(false);

  //�������
  CleanUserJetton();

  //���ð�ť
  m_btApplyBanker.ShowWindow(SW_SHOW);
  m_btApplyBanker.EnableWindow(FALSE);
  m_btCancelBanker.ShowWindow(SW_HIDE);
  m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);

  return;
}

//�����ؼ�
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
  //λ����Ϣ
  m_nWinFlagsExcursionX = LESS_SCREEN_CX/2-192;
  m_nWinFlagsExcursionY = nHeight - 114;

  //����λ��
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

  //��������
  int ExcursionY=10;
  m_PointJettonNumber[ID_SHUN_MEN-1].SetPoint((m_rcShunMen.right+m_rcShunMen.left)/2, (m_rcShunMen.bottom+m_rcShunMen.top)/2-ExcursionY);
  m_PointJettonNumber[ID_JIAO_L-1].SetPoint((m_rcJiaoL.right+m_rcJiaoL.left)/2, (m_rcJiaoL.bottom+m_rcJiaoL.top)/2-ExcursionY);
  m_PointJettonNumber[ID_QIAO-1].SetPoint((m_rcQiao.right+m_rcQiao.left)/2, (m_rcQiao.bottom+m_rcQiao.top)/2-ExcursionY);
  m_PointJettonNumber[ID_DUI_MEN-1].SetPoint((m_rcDuiMen.right+m_rcDuiMen.left)/2, (m_rcDuiMen.bottom+m_rcDuiMen.top)/2-ExcursionY);
  m_PointJettonNumber[ID_DAO_MEN-1].SetPoint((m_rcDaoMen.right+m_rcDaoMen.left)/2, (m_rcDaoMen.bottom+m_rcDaoMen.top)/2-ExcursionY);
  m_PointJettonNumber[ID_JIAO_R-1].SetPoint((m_rcJiaoR.right+m_rcJiaoR.left)/2, (m_rcJiaoR.bottom+m_rcJiaoR.top)/2-ExcursionY);

  //����λ��
  m_PointJetton[ID_SHUN_MEN-1].SetPoint(m_rcShunMen.left, m_rcShunMen.top);
  m_PointJetton[ID_JIAO_L-1].SetPoint(m_rcJiaoL.left, m_rcJiaoL.top);
  m_PointJetton[ID_QIAO-1].SetPoint(m_rcQiao.left, m_rcQiao.top);
  m_PointJetton[ID_DUI_MEN-1].SetPoint(m_rcDuiMen.left, m_rcDuiMen.top);
  m_PointJetton[ID_DAO_MEN-1].SetPoint(m_rcDaoMen.left, m_rcDaoMen.top);
  m_PointJetton[ID_JIAO_R-1].SetPoint(m_rcJiaoR.left, m_rcJiaoR.top);

  //�˿˿ؼ�
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

  //�û��б�λ��
  m_RectUserList.SetRect(12,nHeight-150-5+14,12+203,nHeight-148-5+14+126);
  //������ʾλ��
  int startX = nWidth-m_ImageChatBack.GetWidth()+2;
  int startY = nHeight-126;
  m_RectChartDisplay.SetRect(startX,startY,startX+181,startY+91);

  //�ƶ��ؼ�
  HDWP hDwp=BeginDeferWindowPos(32);
  const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

  //m_ApplyUser.m_viewHandle = m_hWnd;

  //�б�ؼ�
  //DeferWindowPos(hDwp,m_ApplyUser,NULL,nWidth/2 ,nHeight/2-292,210,52,uFlags);
  DeferWindowPos(hDwp,m_btUp,NULL,110 + 24,170,95/5,19,uFlags);
  DeferWindowPos(hDwp,m_btDown,NULL,110,170,95/5,19,uFlags);

  //���밴ť
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





  //��ׯ��ť
  DeferWindowPos(hDwp,m_btApplyBanker,NULL,10,285,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btCancelBanker,NULL,10,285,0,0,uFlags|SWP_NOSIZE);

  DeferWindowPos(hDwp,m_btScoreMoveL,NULL,IsZoomedMode()?244+CalFrameGap():244,nHeight - 110,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btScoreMoveR,NULL,IsZoomedMode()?nWidth - 240-CalFrameGap():nWidth - 240,nHeight - 110,0,0,uFlags|SWP_NOSIZE);

  //���ư�ť
  DeferWindowPos(hDwp,m_btAutoOpenCard,NULL,nWidth-97,nHeight-226,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btOpenCard,NULL,nWidth-97,nHeight-192,0,0,uFlags|SWP_NOSIZE);

  //������ť
  DeferWindowPos(hDwp,m_btContinueCard,NULL,nWidth-97,nHeight-260,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btBankerStorage,NULL,nWidth/2-290,nHeight/2+143,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btBankerDraw,NULL,nWidth/2-210,nHeight/2+143,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btBank,NULL,17,nHeight - 247,0,0,uFlags|SWP_NOSIZE);

  //��ܰ�ť
  //��ܰ�ť
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

  //�����ƶ�
  EndDeferWindowPos(hDwp);

  //���ƿͻ���
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

  // ��ť����
  int nHistoryCount = (m_nRecordLast - m_nRecordFirst + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
  if(!IsZoomedMode() && NORMAL_MAX_FALG_COUNT < nHistoryCount)
  {
    m_btScoreMoveR.EnableWindow(TRUE);
  }
  if(IsZoomedMode() && ZOOMED_MAX_FALG_COUNT < nHistoryCount)
  {
    m_btScoreMoveR.EnableWindow(TRUE);
  }

  //�Ƶ����¼�¼
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

//�滭����
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
  //�滭����
  //DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);
  //DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);

  CDC* pDCBuff = pDC;

  //�滭����
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

  // ����ģʽ
  if(!IsZoomedMode())
  {
    m_ImageHistoryFront.DrawImage(pDC,5 + nUserBackWidth,nHeight - nHistoryFrontHeight);
    for(WORD i = 0; i < 4; i++)
    {
      m_ImageHistoryMid.DrawImage(pDC,5+ nUserBackWidth +nHistoryFrontWidth+ i*nHistoryMidWidth,nHeight - nHistoryMidHeight);
    }
    m_ImageHistoryBack.DrawImage(pDC,5 + nUserBackWidth + nHistoryFrontWidth + 4*nHistoryMidWidth ,nHeight - nHistoryBackHeight);
  }
  else // ȫ��  20����ʶ
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
  //��ȡ״̬
  BYTE cbGameStatus=m_pGameClientDlg->GetGameStatus();

  //״̬��ʾ
  CFont static InfoFont;
  InfoFont.CreateFont(-16,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,_TEXT("����"));
  CFont * pOldFont=pDC->SelectObject(&InfoFont);
  pDC->SetTextColor(RGB(255,234,0));

  pDC->SelectObject(pOldFont);
  InfoFont.DeleteObject();

  //ʱ����ʾ
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

  //Dlg�����SetGameClock(GetMeChairID()��������õ�SwitchViewChairID��������������8ʱʵ����ViewIDת��Ϊ0��
  if(m_wMeChairID!=INVALID_CHAIR)
  {
    WORD wUserTimer=GetUserClock(m_wMeChairID);
    if(wUserTimer!=0)
    {
      DrawTime(pDC, wUserTimer, nWidth/2-m_ImageTimeBack.GetWidth(), 45);
    }
  }

  //ʤ���߿�
  FlashJettonAreaFrame(nWidth,nHeight,pDCBuff);

  //������Դ
  CSize static SizeJettonItem(m_ImageJettonView.GetWidth()/7,m_ImageJettonView.GetHeight());

  LONGLONG lScoreCount1=0L;
  //�滭����
  for(INT i=0; i<AREA_COUNT; i++)
  {
    //��������
    LONGLONG lScoreCount=0L;
    LONGLONG static lScoreJetton[JETTON_COUNT]= {100L,1000L,10000L,100000L,500000L,1000000L,5000000L};
    int static nJettonViewIndex=0;

    //�滭����
    for(INT_PTR j=0; j<m_JettonInfoArray[i].GetCount(); j++)
    {
      //��ȡ��Ϣ
      tagJettonInfo * pJettonInfo=&m_JettonInfoArray[i][j];

      /*if ( !pJettonInfo->bShow )
        continue;*/

      //�ۼ�����
      ASSERT(pJettonInfo->cbJettonIndex<JETTON_COUNT);
      lScoreCount+=lScoreJetton[pJettonInfo->cbJettonIndex];
      lScoreCount1+=lScoreJetton[pJettonInfo->cbJettonIndex];

      //ͼƬ����
      /*if (1==pJettonInfo->cbJettonIndex || 2==pJettonInfo->cbJettonIndex) nJettonViewIndex=pJettonInfo->cbJettonIndex+1;
      else if (0==pJettonInfo->cbJettonIndex) nJettonViewIndex=pJettonInfo->cbJettonIndex;
      else */nJettonViewIndex=pJettonInfo->cbJettonIndex;

      //�滭����
      m_ImageJettonView.DrawImage(pDCBuff,pJettonInfo->nXPos+m_PointJetton[i].x,
                                  pJettonInfo->nYPos+m_PointJetton[i].y,SizeJettonItem.cx,SizeJettonItem.cy,
                                  nJettonViewIndex*SizeJettonItem.cx,0);
    }

    //�滭����
    if(lScoreCount>0L)
    {
      DrawNumberString(pDCBuff,lScoreCount,m_PointJettonNumber[i].x,m_PointJettonNumber[i].y);
    }
  }
  m_TempScore=lScoreCount1;
  //�滭ׯ��
  DrawBankerInfo(pDCBuff,nWidth,nHeight);

  //�滭�û�
  DrawMeInfo(pDCBuff,nWidth,nHeight);

  //�滭��ׯ�б�
  DrawBankerList(pDC,nWidth,nHeight);

  //�л�ׯ��
  if(m_bShowChangeBanker)
  {
    int nXPos = nWidth / 2 - 88;
    int nYPos = nHeight / 2 - 160;

    //������ׯ
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

  //������Ϣ
  DrawCheatCard(nWidth, nHeight, pDCBuff);

  //������ʾ
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

  //�ҵ���ע
  DrawMeJettonNumber(pDCBuff);

  //����״̬
  //if (cbGameStatus==GS_GAME_END)
  {
    //�滭�˿�
    for(int i=0; i<CountArray(m_CardControl); ++i)
    {
      m_CardControl[i].DrawCardControl(pDCBuff);
    }
  }

  //ʤ����־
  DrawWinFlags(pDCBuff);

  if(m_blMoveFinish)
  {
    //��ʾ���
    ShowGameResult(pDCBuff, nWidth, nHeight);
  }

  //�滭���
  DrawGameFrame(pDC, nWidth, nHeight);

  //��������
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

//�滭���
void CGameClientView::DrawGameFrame(CDC *pDC, int nWidth, int nHeight)
{
  m_ImageUserBack.DrawImage(pDC,5,nHeight-m_ImageUserBack.GetHeight()-7);
  m_ImageChatBack.DrawImage(pDC,nWidth-m_ImageChatBack.GetWidth()-5,nHeight-m_ImageChatBack.GetHeight()-7);
  //�ϲ���
  m_ImageGameFrame[0].DrawImage(pDC,0,0);
  for(int i=m_ImageGameFrame[0].GetWidth(); i<nWidth-m_ImageGameFrame[2].GetWidth(); i+=m_ImageGameFrame[1].GetWidth())
  {
    m_ImageGameFrame[1].DrawImage(pDC,i,0);
  }
  m_ImageGameFrame[2].DrawImage(pDC,nWidth-m_ImageGameFrame[2].GetWidth(),0);

  //�м����߲���
  for(int iH=m_ImageGameFrame[0].GetHeight(); iH<nHeight-m_ImageGameFrame[3].GetHeight(); iH+=m_ImageGameFrame[3].GetHeight())
  {
    m_ImageGameFrame[3].DrawImage(pDC,0,iH);
    m_ImageGameFrame[4].DrawImage(pDC,nWidth-m_ImageGameFrame[4].GetWidth(),iH);
  }

  //�²���
  m_ImageGameFrame[5].DrawImage(pDC,0,nHeight-m_ImageGameFrame[5].GetHeight());
  for(int i=m_ImageGameFrame[5].GetWidth(); i<nWidth-m_ImageGameFrame[6].GetWidth(); i+=m_ImageGameFrame[6].GetWidth())
  {
    m_ImageGameFrame[6].DrawImage(pDC,i,nHeight-m_ImageGameFrame[6].GetHeight());
  }
  m_ImageGameFrame[7].DrawImage(pDC,nWidth-m_ImageGameFrame[7].GetWidth(),nHeight-m_ImageGameFrame[7].GetHeight());

  //�滭������
  CRect rectInfo(10, 8, 1024, 30);
  DrawTextString(pDC,m_strCaption,RGB(255,255,255),RGB(0,0,0),rectInfo, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_SINGLELINE|DT_LEFT);
}

// �滭��ׯ�б�
void CGameClientView::DrawBankerList(CDC *pDC, int nWidth, int nHeight)
{
  // �������
  CPoint ptBegin(0,0);
  // ������ׯ�б�
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
    // ����
    CRect rectName(rect);
    rectName.left = ptBegin.x;
    rectName.right = ptBegin.x + 75;
    DrawTextString(pDC, pIClientUserItem->GetNickName(), RGB(253,241,149) , RGB(38,26,2), rectName, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

    // ����
    CRect rectScore(rect);
    rectScore.left = ptBegin.x + 85;
    DrawNumberStringWithSpace(pDC,pIClientUserItem->GetUserScore(), rectScore,RGB(253,241,149) , RGB(38,26,2), DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

    // λ�õ���
    rect.OffsetRect(0, 18);

    // λ������
    nCount++;

    if(nCount == 5)
    {
      break;
    }
  }

}

// �滭ʱ��
void CGameClientView::DrawTime(CDC * pDC, WORD wUserTime,INT nXPos, INT nYPos)
{
  //��������
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

  //��������
  DrawNumber(pDC, &m_ImageTimeNumber, TEXT("0123456789"), szOutNum, nXPos + m_ImageTimeBack.GetWidth()/2, nYPos +20, DT_CENTER);
}


//������Ϣ
void CGameClientView::SetMeMaxScore(LONGLONG lMeMaxScore)
{
  if(m_lMeMaxScore!=lMeMaxScore)
  {
    //���ñ���
    m_lMeMaxScore=lMeMaxScore;
  }

  return;
}

//�����ע
void CGameClientView::SetAreaLimitScore(LONGLONG lAreaLimitScore)
{
  if(m_lAreaLimitScore!= lAreaLimitScore)
  {
    //���ñ���
    m_lAreaLimitScore=lAreaLimitScore;
  }
}

//���ó���
void CGameClientView::SetCurrentJetton(LONGLONG lCurrentJetton)
{
  //���ñ���
  ASSERT(lCurrentJetton>=0L);
  m_lCurrentJetton=lCurrentJetton;

  return;
}

//��ʷ��¼
void CGameClientView::SetGameHistory(bool bWinShunMen, bool bWinDaoMen, bool bWinDuiMen)
{
  //��������
  tagClientGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
  GameRecord.bWinShunMen=bWinShunMen;
  GameRecord.bWinDuiMen=bWinDuiMen;
  GameRecord.bWinDaoMen=bWinDaoMen;

  //��������
  //��������
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

  //�ƶ��±�
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

  //�Ƶ����¼�¼
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

//�������
void CGameClientView::CleanUserJetton()
{
  //��������
  for(BYTE i=0; i<CountArray(m_JettonInfoArray); i++)
  {
    m_JettonInfoArray[i].RemoveAll();
  }

  //��ע��Ϣ
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //ȫ����ע
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

  if(NULL != m_pClientControlDlg && NULL != m_pClientControlDlg->GetSafeHwnd())
  {
    m_pClientControlDlg->m_UserBetArray.RemoveAll();
    m_pClientControlDlg->UpdateUserBet(true);
  }

  //���½���
  RefreshGameView();

  return;
}

//������ע
void CGameClientView::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
  //Ч�����
  ASSERT(cbViewIndex<=ID_JIAO_R);
  if(cbViewIndex>ID_JIAO_R)
  {
    return;
  }

  m_lUserJettonScore[cbViewIndex]=lJettonCount;

  //���½���
  RefreshGameView();
}

//�����˿�
void CGameClientView::SetCardInfo(BYTE cbTableCardArray[4][2])
{
  if(cbTableCardArray!=NULL)
  {
    CopyMemory(m_cbTableCardArray,cbTableCardArray,sizeof(m_cbTableCardArray));

    //��ʼ����
    DispatchCard();
  }
  else
  {
    ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
  }
}

//���ó���
void CGameClientView::PlaceUserJetton(BYTE cbViewIndex, LONGLONG lScoreCount)
{
  //Ч�����
  ASSERT(cbViewIndex<=ID_JIAO_R);
  if(cbViewIndex>ID_JIAO_R)
  {
    return;
  }

  //��������
  bool bPlaceJetton=false;
  LONGLONG lScoreIndex[JETTON_COUNT]= {100L,1000L,10000L,100000L,500000L,1000000L,5000000L};

  //�߿���
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

  //�����ж�
  bool bAddJetton=lScoreCount>0?true:false;
  lScoreCount=(lScoreCount>0?lScoreCount:-lScoreCount);

  //���ӳ���
  for(BYTE i=0; i<CountArray(lScoreIndex); i++)
  {
    //������Ŀ
    BYTE cbScoreIndex=JETTON_COUNT-i-1;
    LONGLONG nCellCount=LONGLONG(lScoreCount/lScoreIndex[cbScoreIndex]);

    //�������
    if(nCellCount==0L)
    {
      continue;
    }

    //�������
    for(LONGLONG j=0; j<nCellCount; j++)
    {
      if(true==bAddJetton)
      {
        //�������
        tagJettonInfo JettonInfo;
        int nJettonSize = 68;
        JettonInfo.cbJettonIndex = cbScoreIndex;
        JettonInfo.nXPos = rand()%(nFrameWidth-nJettonSize);
        JettonInfo.nYPos = rand()%(nFrameHeight-nJettonSize);
        JettonInfo.bShow = TRUE;

        //��������
        bPlaceJetton=true;
        m_JettonInfoArray[cbViewIndex-1].Add(JettonInfo);
      }
      else
      {
        for(int nIndex=0; nIndex<m_JettonInfoArray[cbViewIndex-1].GetCount(); ++nIndex)
        {
          //�Ƴ��ж�
          tagJettonInfo &JettonInfo=m_JettonInfoArray[cbViewIndex-1][nIndex];
          if(JettonInfo.cbJettonIndex==cbScoreIndex)
          {
            m_JettonInfoArray[cbViewIndex-1].RemoveAt(nIndex);
            break;
          }
        }
      }
    }

    //������Ŀ
    lScoreCount-=nCellCount*lScoreIndex[cbScoreIndex];
  }

  //��������
  CRgn JettonRegion;        //��ʾ����
  CRgn SignedRegion;        //��������
  CRgn SignedAndJettonRegion;   //��������
  SignedAndJettonRegion.CreateRectRgn(0,0,0,0);

  //ͼƬ����
  CSize static SizeJettonItem(m_ImageJettonView.GetWidth()/7,m_ImageJettonView.GetHeight());

  int nIndex = cbViewIndex-1;
  //��������
  for(INT_PTR j = m_JettonInfoArray[nIndex].GetCount() - 1; j >= 0; j--)
  {
    //��ȡ��Ϣ
    tagJettonInfo * pJettonInfo = &m_JettonInfoArray[nIndex][j];
    pJettonInfo->bShow = false;
    CRect rcRect(pJettonInfo->nXPos+m_PointJetton[nIndex].x,
                 pJettonInfo->nYPos+m_PointJetton[nIndex].y,
                 pJettonInfo->nXPos+m_PointJetton[nIndex].x + m_ImageJettonView.GetWidth()/9,
                 pJettonInfo->nYPos+m_PointJetton[nIndex].y + m_ImageJettonView.GetHeight());

    //�״�����
    if(!JettonRegion.GetSafeHandle())
    {
      JettonRegion.CreateEllipticRgnIndirect(rcRect);
      pJettonInfo->bShow = true;
      continue;
    }

    //�����ж�
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

    //�ϲ�����
    SignedRegion.DeleteObject();
  }

  //ɾ������
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



  //���½���
  if(bPlaceJetton==true)
  {
    RefreshGameView();
  }

  return;
}

//���ֳɼ�
void CGameClientView::SetCurGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore, LONGLONG lGameRevenue)
{
  m_lMeCurGameScore=lMeCurGameScore;
  m_lMeCurGameReturnScore=lMeCurGameReturnScore;
  m_lBankerCurGameScore=lBankerCurGameScore;
  m_lGameRevenue=lGameRevenue;
}

//����ʤ��
void CGameClientView::SetWinnerSide(bool bWinShunMen, bool bWinDuiMen, bool bWinDaoMen, bool bSet)
{
  //����ʱ��
  if(true==bSet)
  {
    //���ö�ʱ��
    SetTimer(IDI_FLASH_WINNER,500,NULL);

    //ȫʤ�ж�
    if(false==bWinShunMen && false==bWinDuiMen && false==bWinDaoMen)
    {
      //�����Դ
      //m_ImageFrameShunMen.DestroyImage();
      //m_ImageFrameJiaoL.DestroyImage();
      //m_ImageFrameQiao.DestroyImage();
      //m_ImageFrameDuiMen.DestroyImage();
      //m_ImageFrameDaoMen.DestroyImage();
      //m_ImageFrameJiaoR.DestroyImage();

      ////������Դ
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
    //�����ʱ��
    KillTimer(IDI_FLASH_WINNER);

    //ȫʤ�ж�
    if(false==bWinShunMen && false==bWinDuiMen && false==bWinDaoMen)
    {
      //�����Դ
      //m_ImageFrameShunMen.DestroyImage();
      //m_ImageFrameJiaoL.DestroyImage();
      //m_ImageFrameQiao.DestroyImage();
      //m_ImageFrameDuiMen.DestroyImage();
      //m_ImageFrameDaoMen.DestroyImage();
      //m_ImageFrameJiaoR.DestroyImage();

      ////������Դ
      //HINSTANCE hInstance=AfxGetInstanceHandle();
      //m_ImageFrameShunMen.LoadImage(this,hInstance,TEXT("FRAME_SHUN_MEN_EX"));
      //m_ImageFrameJiaoL.LoadImage(this,hInstance,TEXT("FRAME_JIAO_L_EX"));
      //m_ImageFrameQiao.LoadImage(this,hInstance,TEXT("FRAME_QIAO_EX"));
      //m_ImageFrameDuiMen.LoadImage(this,hInstance,TEXT("FRAME_DUI_MEN_EX"));
      //m_ImageFrameDaoMen.LoadImage(this,hInstance,TEXT("FRAME_DAO_EMN_EX"));
      //m_ImageFrameJiaoR.LoadImage(this,hInstance,TEXT("FRAME_JIAO_R_EX"));
    }
  }

  //���ñ���
  m_bWinShunMen=bWinShunMen;
  m_bWinDuiMen=bWinDuiMen;
  m_bWinDaoMen=bWinDaoMen;
  m_bFlashResult=bSet;
  m_bShowGameResult=bSet;
  m_cbAreaFlash=0xFF;

  //���½���
  RefreshGameView();

  return;
}

//��ȡ����
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

//�滭����
void CGameClientView::DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore)
{
  //������Դ
  CSize SizeScoreNumber(m_ImageScoreNumber.GetWidth()/10,m_ImageScoreNumber.GetHeight());

  if(bMeScore)
  {
    SizeScoreNumber.SetSize(m_ImageMeScoreNumber.GetWidth()/11, m_ImageMeScoreNumber.GetHeight());
  }

  //������Ŀ
  int nNumberCount=0;
  LONGLONG lNumberTemp=lNumber;
  do
  {
    nNumberCount++;
    lNumberTemp/=10;
  }
  while(lNumberTemp>0);

  //λ�ö���
  INT nYDrawPos=nYPos-SizeScoreNumber.cy/2;
  INT nXDrawPos=nXPos+nNumberCount*SizeScoreNumber.cx/2-SizeScoreNumber.cx;

  //�滭����
  for(int i=0; i<nNumberCount; i++)
  {
    //�滭����
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

    //���ñ���
    lNumber/=10;
    nXDrawPos-=SizeScoreNumber.cx;
  };

  return;
}

//�滭����
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

  //�������
  TextOut(pDC,nXPos,nYPos,strNumber,lstrlen(strNumber));
}

//�滭����
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

  //�������
  if(nFormat==-1)
  {
    m_DFontExT.DrawText(pDC, strNumber, rcRect, RGB(255,234,0), DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
  }
  else
  {
    m_DFontExT.DrawText(pDC, strNumber, rcRect, RGB(255,234,0), nFormat);
  }
}

//�滭����
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

  //�滭�߿�
  for(INT i=0; i<CountArray(nXExcursion); i++)
  {
    //����λ��
    CRect rcFrame;
    rcFrame.top=rcRect.top+nYExcursion[i];
    rcFrame.left=rcRect.left+nXExcursion[i];
    rcFrame.right=rcRect.right+nXExcursion[i];
    rcFrame.bottom=rcRect.bottom+nYExcursion[i];

    //�滭�ַ�
    CDFontEx::DrawText(this,pDC,  12, 400, strNumber, rcFrame, crFrame, nFormat);
  }

  //�滭�ַ�
  CDFontEx::DrawText(this,pDC,  12, 400, strNumber, rcRect, crText, nFormat);
}

// �滭����
void CGameClientView::DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, LONGLONG lOutNum,INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/)
{
  TCHAR szOutNum[128] = {0};
  _sntprintf(szOutNum,CountArray(szOutNum),TEXT("%I64d"),lOutNum);
  DrawNumber(pDC, ImageNumber, szImageNum, szOutNum, nXPos, nYPos, uFormat);
}

// �滭����
void CGameClientView::DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/)
{
  TCHAR szOutNumT[128] = {0};
  _sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%s"),szOutNum);
  DrawNumber(pDC, ImageNumber, szImageNum, szOutNumT, nXPos, nYPos, uFormat);
}


// �滭����
void CGameClientView::DrawNumber(CDC * pDC , CPngImage* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat /*= DT_LEFT*/)
{
  // ������Դ
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

//��ʱ����Ϣ
void CGameClientView::OnTimer(UINT nIDEvent)
{
  //����ʤ��
  if(nIDEvent==IDI_FLASH_WINNER)
  {
    //���ñ���
    m_bFlashResult=!m_bFlashResult;

    //���½���
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
      //���½���
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
        //���½���
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
            //������ʾ�ƶ�ʱ��
            SetTimer(IDI_DISPATCH_CARD,30,NULL);
          }

        }

      }
    }
  }
  //�ֻ�ׯ��
  else if(nIDEvent == IDI_SHOW_CHANGE_BANKER)
  {
    ShowChangeBanker(false);

    return;
  }
  else if(nIDEvent==IDI_DISPATCH_CARD)
  {
    //��ɷ���
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
    //���½���
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

//�����Ϣ
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
    //��ע����
    BYTE cbJettonArea=GetJettonArea(Point);

    if(cbJettonArea >= 1 && cbJettonArea <= AREA_COUNT)
    {
      //�����ע
      LONGLONG lMaxJettonScore=GetUserMaxJetton(cbJettonArea);

      //�Ϸ��ж�
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

//���������ؼ���������Ϣ
//LRESULT CGameClientView::OnViLBtUp(WPARAM wParam, LPARAM lParam)
//{
//  CPoint *pPoint = (CPoint*)lParam;
//  ScreenToClient(pPoint);
//  OnLButtonUp(1,*pPoint);
//  return 1;
//
//}

//����ƶ���Ϣ
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

//�����Ϣ
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint Point)
{
  //���ñ���
  m_lCurrentJetton=0L;

  if(m_pGameClientDlg->GetGameStatus()!=GS_GAME_END && m_cbAreaFlash!=0xFF)
  {
    m_cbAreaFlash=0xFF;
    RefreshGameView();
  }

  __super::OnLButtonDown(nFlags,Point);
}

//�����Ϣ
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
  if(m_lCurrentJetton!=0L)
  {
    //��ȡ����
    CPoint MousePoint;
    GetCursorPos(&MousePoint);
    ScreenToClient(&MousePoint);
    BYTE cbJettonArea=GetJettonArea(MousePoint);

    //���ñ���
    if(m_cbAreaFlash!= cbJettonArea)
    {
      m_cbAreaFlash = cbJettonArea;
      RefreshGameView();
    }

    //�����ж�
    if(cbJettonArea==0xFF)
    {
      SetCursor(LoadCursor(NULL,IDC_ARROW));
      return TRUE;
    }

    //�����ע
    LONGLONG lMaxJettonScore=GetUserMaxJetton(cbJettonArea);

    LONGLONG lLeaveScore=lMaxJettonScore;

    //���ù��
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

    //�Ϸ��ж�
    if((m_lAllJettonScore[cbJettonArea]+m_lCurrentJetton)>m_lAreaLimitScore || lMaxJettonScore < m_lCurrentJetton || 0 == m_lCurrentJetton)
    {
      SetCursor(LoadCursor(NULL,IDC_NO));
      return TRUE;
    }

    //���ù��
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

//���ñ���
LRESULT CGameClientView::OnSetCaption(WPARAM wParam, LPARAM lParam)
{
  m_strCaption = (LPCTSTR)lParam;
  CRect rcCaption(5,0,1024,30);
  CDC* pDC = GetDC();

  return 0;
}

//�ֻ�ׯ��
void CGameClientView::ShowChangeBanker(bool bChangeBanker)
{
  //�ֻ�ׯ��
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

  //���½���
  RefreshGameView();
}

//��ׯ��ť
void CGameClientView::OnApplyBanker()
{
  SendEngineMessage(IDM_APPLY_BANKER,1,0);

}

//��ׯ��ť
void CGameClientView::OnCancelBanker()
{
  SendEngineMessage(IDM_APPLY_BANKER,0,0);
}

//��������
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat)
{
  //��������
  //��������
  INT nStringLength=lstrlen(pszString);
  INT nXExcursion[8]= {1,1,1,0,-1,-1,-1,0};
  INT nYExcursion[8]= {-1,0,1,1,1,0,-1,-1};

  //�滭�߿�
  for(INT i=0; i<CountArray(nXExcursion); i++)
  {
    //����λ��
    CRect rcFrame;
    rcFrame.top=rcRect.top+nYExcursion[i];
    rcFrame.left=rcRect.left+nXExcursion[i];
    rcFrame.right=rcRect.right+nXExcursion[i];
    rcFrame.bottom=rcRect.bottom+nYExcursion[i];

    //�滭�ַ�
    CDFontEx::DrawText(this,pDC,  12, 400, pszString, rcFrame, crFrame, nDrawFormat);
  }

  //�滭�ַ�
  CDFontEx::DrawText(this,pDC,  12, 400, pszString, rcRect, crText, nDrawFormat);

  return;
}
//ׯ����Ϣ
void CGameClientView::SetBankerInfo(DWORD dwBankerUserID, LONGLONG lBankerScore)
{
  //ׯ�����Ӻ�
  WORD wBankerUser=INVALID_CHAIR;

  //�������Ӻ�
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

  //�л��ж�
  if(m_wBankerUser!=wBankerUser)
  {
    m_wBankerUser=wBankerUser;
    m_wBankerTime=0L;
    m_lBankerWinScore=0L;
    m_lTmpBankerWinScore=0L;
  }
  m_lBankerScore=lBankerScore;
}

//�滭��ʶ
void CGameClientView::DrawWinFlags(CDC * pDC)
{
  //m_nRecordLast = 15;
  //�ǿ��ж�
  if(m_nRecordLast==m_nRecordFirst)
  {
    return;
  }

  //��Դ����
  int nIndex = m_nScoreHead;
  COLORREF static clrOld ;

  clrOld = pDC->SetTextColor(RGB(255,234,0));
  int nDrawCount = 0;
  int nDrawTotalHorCount = IsZoomedMode() ? ZOOMED_MAX_FALG_COUNT : NORMAL_MAX_FALG_COUNT;
  int xOffset = 0;
  // ȫ����ʶ����ƽ��
  if(IsZoomedMode())
  {
    xOffset = CalFrameGap();
  }

  while(nIndex != m_nRecordLast && (m_nRecordLast!=m_nRecordFirst) && nDrawCount < nDrawTotalHorCount)
  {
    //ʤ����ʶ
    tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
    bool static bWinMen[3];
    bWinMen[0]=ClientGameRecord.bWinShunMen;
    bWinMen[1]=ClientGameRecord.bWinDaoMen;
    bWinMen[2]=ClientGameRecord.bWinDuiMen;

    //�������
    enOperateResult static OperateResult[3];
    OperateResult[0]=ClientGameRecord.enOperateShunMen;
    OperateResult[1]=ClientGameRecord.enOperateDaoMen;
    OperateResult[2]=ClientGameRecord.enOperateDuiMen;

    for(int i=0; i<3; ++i)
    {
      //λ�ñ���
      int static nYPos=0,nXPos=0;
      nYPos=m_nWinFlagsExcursionY+i*31;
      nXPos=m_nWinFlagsExcursionX + ((nIndex - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY) * 30;

      //ʤ����ʶ
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
      //�滭��ʶ
      m_ImageWinFlags.DrawImage(pDC, nXPos + xOffset, nYPos, m_ImageWinFlags.GetWidth()/4 , m_ImageWinFlags.GetHeight(),m_ImageWinFlags.GetWidth()/4 * nFlagsIndex, 0);
    }

    //�ƶ��±�
    nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
    nDrawCount++;
  }
  pDC->SetTextColor(clrOld);
}
//�ֹ�����
afx_msg void CGameClientView::OnOpenCard()
{
  SendEngineMessage(IDM_OPEN_CARD,0,0);
}

//�Զ�����
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

  //���½���
  UpdateButtonContron();
  InvalidGameView(0,0,0,0);
}

void CGameClientView::OnDown()
{
  if(m_nShowValleyIndex < m_ValleysList.GetCount() - 1)
  {
    m_nShowValleyIndex++;
  }

  //���½���
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

  //��ѯ�ӿ�
  IClientKernel * m_pIClientKernel;
  CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
  ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
  m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
  m_pIClientKernel->SendUserChatMessage(0, str, RGB(0,0,0));

  GetDlgItem(IDC_CHAT_INPUT)->SetWindowText(TEXT(""));
  GetDlgItem(IDC_CHAT_INPUT)->SetFocus();

  return;
}

//����Ч��
bool CGameClientView::EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex)
{
  IClientKernel * m_pIClientKernel;
  CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
  ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
  m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);

  IClientUserItem * m_pIMySelfUserItem = GetClientUserItem(m_wMeChairID);

  //״̬�ж�
  if(m_pIClientKernel==NULL)
  {
    return false;
  }
  if(m_pIMySelfUserItem==NULL)
  {
    return false;
  }

  IStringMessage * pIStringMessage = m_pIClientKernel->GetIStringMessage();

  //���ȱȽ�
  if(pszChatString != NULL)
  {
    CString strChat=pszChatString;
    if(strChat.GetLength() >= (LEN_USER_CHAT/2))
    {
      pIStringMessage->InsertCustomString(TEXT("��Ǹ����������������ݹ����������̺��ٷ��ͣ�\r\n"),COLOR_WARN);
      return false;
    }
  }

  //��������
  BYTE cbMemberOrder=m_pIMySelfUserItem->GetMemberOrder();
  BYTE cbMasterOrder=m_pIMySelfUserItem->GetMasterOrder();

  //���Զ���
  tagUserAttribute * pUserAttribute=m_pIClientKernel->GetUserAttribute();
  tagServerAttribute * pServerAttribute=m_pIClientKernel->GetServerAttribute();

  //��������
  if(CServerRule::IsForfendGameChat(pServerAttribute->dwServerRule)&&(cbMasterOrder==0))
  {
    //ԭʼ��Ϣ
    if(wExpressionIndex==INVALID_WORD)
    {
      CString strDescribe;
      strDescribe.Format(TEXT("\n��%s������ʧ��"),pszChatString);
      pIStringMessage->InsertCustomString(strDescribe,COLOR_EVENT);
    }

    //������Ϣ
    pIStringMessage->InsertSystemString(TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û��������죡"));

    return false;
  }

  //Ȩ���ж�
  if(CUserRight::CanGameChat(pUserAttribute->dwUserRight)==false)
  {
    //ԭʼ��Ϣ
    if(wExpressionIndex==INVALID_WORD)
    {
      CString strDescribe;
      strDescribe.Format(TEXT("\n��%s������ʧ��"),pszChatString);
      pIStringMessage->InsertCustomString(strDescribe,COLOR_EVENT);
    }

    //������Ϣ
    pIStringMessage->InsertSystemString(TEXT("��Ǹ����û����Ϸ���Ե�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"));

    return false;
  }

  //�ٶ��ж�
  DWORD dwCurrentTime=(DWORD)time(NULL);
  if((cbMasterOrder==0)&&((dwCurrentTime-m_dwChatTime)<=TIME_USER_CHAT))
  {
    //ԭʼ��Ϣ
    if(wExpressionIndex==INVALID_WORD)
    {
      CString strDescribe;
      strDescribe.Format(TEXT("\n��%s������ʧ��"),pszChatString);
      pIStringMessage->InsertCustomString(strDescribe,COLOR_EVENT);
    }

    //������Ϣ
    pIStringMessage->InsertSystemString(TEXT("����˵���ٶ�̫���ˣ����������ȱ�����Ϣ�°ɣ�"));

    return false;
  }

  //���ñ���
  m_dwChatTime=dwCurrentTime;
  //m_strChatString=pszChatString;

  return true;
}

//�ƶ���ť
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

  //���½���
  RefreshGameView();
}

//�ƶ���ť
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

  //���½���
  RefreshGameView();
}

//��ʾ���
void CGameClientView::ShowGameResult(CDC *pDC, int nWidth, int nHeight)
{
  //��ʾ�ж�
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

//͸���滭
bool CGameClientView::DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha)
{
  ASSERT(pDC != 0 && lpRect != 0 && clr != CLR_NONE);
  if(pDC == 0 || lpRect == 0 || clr == CLR_NONE)
  {
    return false;
  }
  //ȫ͸��
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

//�����ע
LONGLONG CGameClientView::GetUserMaxJetton(BYTE cbArea/* = 0xFF*/)
{
  //��������
  LONGLONG lMeMaxScore = 0L;

  //����ע��
  LONGLONG lNowJetton = 0;
  ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lNowJetton += m_lUserJettonScore[nAreaIndex];
  }

  //ׯ�ҽ��
  LONGLONG lBankerScore=2147483647;
  if(m_wBankerUser!=INVALID_CHAIR)
  {
    lBankerScore=m_lBankerScore*m_nEndGameMul/100;
  }

  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lBankerScore-=m_lAllJettonScore[nAreaIndex];
  }

  //��������
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

  //ׯ������
  lMeMaxScore=min(lMeMaxScore,lBankerScore);

  //��������
  ASSERT(lMeMaxScore >= 0);
  lMeMaxScore = max(lMeMaxScore, 0);

  return lMeMaxScore;
}

//�ɼ�����
void CGameClientView::SetGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore)
{
  m_lMeCurGameScore=lMeCurGameScore;
  m_lMeCurGameReturnScore=lMeCurGameReturnScore;
  m_lBankerCurGameScore=lBankerCurGameScore;
}

//�滭����
void CGameClientView::DrawMeJettonNumber(CDC *pDC)
{
  //�滭����
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    if(m_lUserJettonScore[nAreaIndex] > 0)
    {
      DrawNumberString(pDC,m_lUserJettonScore[nAreaIndex],m_PointJettonNumber[nAreaIndex-1].x,m_PointJettonNumber[nAreaIndex-1].y+25, true);
    }
  }
}

//��ʼ����
void CGameClientView::DispatchCard()
{
  //���ý���
  for(int i=0; i<CountArray(m_CardControl); ++i)
  {
    m_CardControl[i].m_CardItemArray.SetSize(0);
  }
  //���ñ�ʶ
  m_bNeedSetGameRecord=true;
}

//��������
void CGameClientView::FinishDispatchCard(bool bNotScene)
{
  //����ж�
  if(m_bNeedSetGameRecord==false)
  {
    return;
  }

  //���ñ�ʶ
  m_bNeedSetGameRecord=false;

  //ɾ����ʱ��
  KillTimer(IDI_DISPATCH_CARD);

  ////�����˿�
  //for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].SetCardData(m_cbTableCardArray[i],2);

  //�ƶ�Ӯ��
  bool bWinShunMen, bWinDuiMen, bWinDaoMen;
  DeduceWinner(bWinShunMen, bWinDuiMen, bWinDaoMen);

  //�����¼
  if(bNotScene)
  {
    SetGameHistory(bWinShunMen, bWinDaoMen, bWinDuiMen);
  }

  //�ۼƻ���
  m_lMeStatisticScore+=m_lMeCurGameScore;
  m_lBankerWinScore=m_lTmpBankerWinScore;

  //����Ӯ��
  SetWinnerSide(bWinShunMen, bWinDuiMen, bWinDaoMen, true);
  InsertGameEndInfo();

  //��������
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

//��ֹ����
void CGameClientView::StopAnimal()
{
  //��ʱ��
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

//ʤ���߿�
void CGameClientView::FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC)
{
  //�Ϸ��ж�
  if(m_cbAreaFlash==0xFF && false==m_bFlashResult)
  {
    return;
  }

  //��ע�ж�
  if(false==m_bFlashResult)
  {
    switch(m_cbAreaFlash)
    {
      case ID_SHUN_MEN:
      {
        //�滭ͼƬ
        m_ImageFrameShunMen.DrawImage(pDC,m_rcShunMen.left-67, m_rcShunMen.top-110);
        break;
      }
      case ID_JIAO_L:
      {
        //�滭ͼƬ
        m_ImageFrameJiaoL.DrawImage(pDC,m_rcJiaoL.left-58, m_rcJiaoL.top);
        break;
      }
      case ID_QIAO:
      {
        //�滭ͼƬ
        m_ImageFrameQiao.DrawImage(pDC,m_rcQiao.left-7, m_rcQiao.top-93);
        break;
      }
      case ID_DUI_MEN:
      {
        //�滭ͼƬ
        m_ImageFrameDuiMen.DrawImage(pDC,m_rcDuiMen.left-13, m_rcDuiMen.top);
        break;
      }
      case ID_DAO_MEN:
      {
        //�滭ͼƬ
        m_ImageFrameDaoMen.DrawImage(pDC,m_rcDaoMen.left-15, m_rcDaoMen.top-115);
        break;
      }
      case ID_JIAO_R:
      {
        //�滭ͼƬ
        m_ImageFrameJiaoR.DrawImage(pDC,m_rcJiaoR.left-6, m_rcJiaoR.top);
        break;
      }
    }
  }
  else
  {
    //ׯȫʤ�ж�
    if(false==m_bWinDaoMen && false==m_bWinShunMen && false==m_bWinDuiMen)
    {
      //�滭ͼƬ
      /*m_ImageFrameShunMen.DrawImage(pDC,m_rcShunMen.left, m_rcShunMen.top);
      m_ImageFrameJiaoL.DrawImage(pDC,m_rcJiaoL.left, m_rcJiaoL.top);
      m_ImageFrameQiao.DrawImage(pDC,m_rcQiao.left, m_rcQiao.top);
      m_ImageFrameDuiMen.DrawImage(pDC,m_rcDuiMen.left, m_rcDuiMen.top);
      m_ImageFrameDaoMen.DrawImage(pDC,m_rcDaoMen.left, m_rcDaoMen.top);
      m_ImageFrameJiaoR.DrawImage(pDC,m_rcJiaoR.left, m_rcJiaoR.top);*/
    }
    else
    {
      //���ж�
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

      //���ж�
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


//�ƶ�Ӯ��
void CGameClientView::DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen)
{
  //��С�Ƚ�
  bWinShunMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[SHUN_MEN_INDEX],2)==1?true:false;
  bWinDuiMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DUI_MEN_INDEX],2)==1?true:false;
  bWinDaoMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DAO_MEN_INDEX],2)==1?true:false;
}

//�ؼ�����
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
  //��ȡID
  WORD wControlID=LOWORD(wParam);

  //�ؼ��ж�
  switch(wControlID)
  {
    case IDC_JETTON_BUTTON_100:
    {
      //���ñ���
      m_lCurrentJetton=100L;
      break;
    }
    case IDC_JETTON_BUTTON_1000:
    {
      //���ñ���
      m_lCurrentJetton=1000L;
      break;
    }
    case IDC_JETTON_BUTTON_10000:
    {
      //���ñ���
      m_lCurrentJetton=10000L;
      break;
    }
    case IDC_JETTON_BUTTON_100000:
    {
      //���ñ���
      m_lCurrentJetton=100000L;
      break;
    }
    case IDC_JETTON_BUTTON_500000:
    {
      //���ñ���
      m_lCurrentJetton=500000L;
      break;
    }
    case IDC_JETTON_BUTTON_1000000:
    {
      //���ñ���
      m_lCurrentJetton=1000000L;
      break;
    }
    case IDC_JETTON_BUTTON_5000000:
    {
      //���ñ���
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
      //��ʾ����
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
        //��������
        CMessageTipDlg GameTips;

        //��������
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

//�ҵ�λ��
void CGameClientView::SetMeChairID(WORD dwMeUserID)
{
  //�������Ӻ�
  m_wMeChairID=dwMeUserID;
}

//������ʾ
void CGameClientView::SetDispatchCardTip(enDispatchCardTip DispatchCardTip)
{
  //���ñ���
  m_enDispatchCardTip=DispatchCardTip;

  //���ö�ʱ��
  if(enDispatchCardTip_NULL!=DispatchCardTip)
  {
    SetTimer(IDI_SHOWDISPATCH_CARD_TIP,2*1000,NULL);
  }
  else
  {
    KillTimer(IDI_SHOWDISPATCH_CARD_TIP);
  }

  //���½���
  RefreshGameView();
}

//�滭ׯ��
void CGameClientView::DrawBankerInfo(CDC *pDC,int nWidth,int nHeight)
{
  //ׯ����Ϣ
  pDC->SetTextColor(RGB(255,234,0));

  //��ȡ���
  IClientUserItem  *pClientUserItem = m_wBankerUser==INVALID_CHAIR ? NULL : GetClientUserItem(m_wBankerUser);

  //λ����Ϣ
  CRect static StrRect;
  StrRect.left = 63;
  StrRect.top = 60;
  StrRect.right = StrRect.left + 122;
  StrRect.bottom = StrRect.top + 15;

  //ׯ������
  m_DFontExT.DrawText(pDC, pClientUserItem==NULL?(m_bEnableSysBanker?_TEXT("ϵͳ��ׯ"):_TEXT("������ׯ")):pClientUserItem->GetNickName(), StrRect, RGB(255,234,0), DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE);

  //ׯ���ܷ�
  //StrRect.left = nWidth/2-293;
  StrRect.top = 81;
  //StrRect.right = StrRect.left + 122;
  StrRect.bottom = StrRect.top + 15;
  DrawNumberStringWithSpace(pDC,pClientUserItem==NULL?0:pClientUserItem->GetUserScore(), StrRect);

  //ׯ�ҳɼ�
  //StrRect.left = nWidth/2-293;
  StrRect.top = 102;
  //StrRect.right = StrRect.left + 122;
  StrRect.bottom = StrRect.top + 15;
  DrawNumberStringWithSpace(pDC,m_lBankerWinScore,StrRect);

  //ׯ�Ҿ���
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

//������������
void CGameClientView::SetCheatInfo(BYTE bCardData[], int nWinInfo[])
{
  //��������
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

//������ͼ
void CGameClientView::RefreshGameView()
{
  CRect rect;
  GetClientRect(&rect);
  InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

  return;
}

//���°�ť
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

//�滭���
void CGameClientView::DrawMeInfo(CDC *pDC,int nWidth,int nHeight)
{
  //�Ϸ��ж�
  if(INVALID_CHAIR==m_wMeChairID)
  {
    return;
  }

  IClientUserItem *pMeUserData = GetClientUserItem(m_wMeChairID);
  if(pMeUserData != NULL)
  {
    //������ɫ
    pDC->SetTextColor(RGB(255,234,0));

    //�ܹ���ע
    LONGLONG lMeJetton=0L;
    for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
    {
      lMeJetton += m_lUserJettonScore[nAreaIndex];
    }

    //����λ��
    CRect static rcDraw;
    rcDraw.left=nWidth-137;
    rcDraw.top=57;
    rcDraw.right=rcDraw.left+140;
    rcDraw.bottom=rcDraw.top+15;

    //�滭�ʺ�
    m_DFontExT.DrawText(pDC, pMeUserData->GetNickName(), rcDraw, RGB(255,234,0), DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

    //�滭����
    rcDraw.top=78;
    rcDraw.bottom=rcDraw.top+15;
    DrawNumberStringWithSpace(pDC,pMeUserData->GetUserScore()-lMeJetton,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

    //�滭�ɼ�
    rcDraw.top=99;
    rcDraw.bottom=rcDraw.top+15;
    DrawNumberStringWithSpace(pDC,m_lMeStatisticScore,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

    //�滭��ע
    rcDraw.top=120;
    rcDraw.bottom=rcDraw.top+15;
    DrawNumberStringWithSpace(pDC,lMeJetton,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
  }
}

//�����˿�
void CGameClientView::DrawCheatCard(int nWidth, int nHeight, CDC *pDC)
{
  if(!m_bShowCheatInfo)
  {
    return;
  }


  //��������
  int nXPos[4] = {nWidth/2-48, nWidth/2-360, nWidth/2-48, nWidth/2+265};
  int nYPos[4] = {nHeight/2-340, nHeight/2-152, nHeight/2+45, nHeight/2-152};
  int nXImagePos = 0, nYImagePos = 0;
  CSize cardSize(CCardControl::m_ImageCard.GetWidth()/13, CCardControl::m_ImageCard.GetHeight()/5);

  //�滭�˿�
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

  //�滭�߿�
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

//������Ϣ
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

//ϵͳ��Ϣ
void CGameClientView::InsertSystemMessage(LPCTSTR pszString)
{
  CString strMessage;
  strMessage.Format(TEXT("[ϵͳ��Ϣ]:"));
  InsertMessage(strMessage,RGB(255,0,0));
  InsertMessage(pszString);
  InsertMessage(TEXT("\r\n"));
}

//������Ϣ
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

//������Ϣ
void CGameClientView::InsertNormalMessage(LPCTSTR pszString)
{
  InsertMessage(pszString);
  InsertMessage(TEXT("\r\n"));
}

//��Ϣ����
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

//������Ϣ
void CGameClientView::InsertGameEndInfo()
{
  CString strInfo;
  strInfo.Format(TEXT("\r\n"));
  InsertAllChatMessage(strInfo,0);

  strInfo.Format(TEXT("���ֽ���,�ɼ�ͳ��:"));
  InsertAllChatMessage(strInfo,0);

  strInfo.Format(TEXT("ׯ��:%I64d"),m_lBankerCurGameScore);

  InsertAllChatMessage(strInfo,0);

  strInfo.Format(TEXT("����:%I64d"),m_lMeCurGameScore);

  InsertAllChatMessage(strInfo,0);
}
//////////////////////////////////////////////////////////////////////////
//���а�ť
void CGameClientView::OnButtonGoBanker()
{
  //��ȡ�ӿ�
  CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
  IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

  if(NULL==pIClientKernel)
  {
    return;
  }


  //��������
  CBankTipsDlg GameTips;

  POINT locPoint;
  locPoint.x=10;
  locPoint.y=340;
  ClientToScreen(&locPoint);

  GameTips.SetStartLoc(CPoint(locPoint.x,locPoint.y));
  //��������
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
//���д��
void CGameClientView::OnBankStorage()
{
  //��ȡ�ӿ�
  CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
  IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

  if(NULL!=pIClientKernel)
  {
    CRect btRect;
    m_btBankerStorage.GetWindowRect(&btRect);
    ShowInsureSave(pIClientKernel,CPoint(btRect.right,btRect.top));
  }
}

//����ȡ��
void CGameClientView::OnBankDraw()
{
  //��ȡ�ӿ�
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
  //����д�Ȩ��
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

//�������
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


