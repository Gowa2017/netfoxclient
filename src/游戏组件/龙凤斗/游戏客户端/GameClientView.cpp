#include "StdAfx.h"
#include "Math.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
#include "GameFrameWindow.h"
//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_FLASH_WINNER			100									//������ʶ
#define IDI_SHOW_CHANGE_BANKER		101									//�ֻ�ׯ��
#define IDI_FLEX_MOVE				102									//������Ϣ
#define IDI_DISPATCH_CARD			103									//���Ʊ�ʶ
#define IDI_DISPATCH_INTERVAL		104									//���Ƽ��
#define IDI_END_INTERVAL			105									//���Ƽ��
#define IDI_FLASH_BET				106									//������ʶ
#define IDI_WIN_TYPE_DELAY			107									//ʤ�������ӳ�
#define IDI_WIN_TYPE				108									//ʤ������
#define IDI_BET_DELAY				200									//��ע�ӳ�


//��ť��ʶ
#define IDC_CM_BUTTON_100			200									//��ť��ʶ
#define IDC_CM_BUTTON_1000			201									//��ť��ʶ
#define IDC_CM_BUTTON_10000			202									//��ť��ʶ
#define IDC_CM_BUTTON_100000		203									//��ť��ʶ
#define IDC_CM_BUTTON_1000000		204									//��ť��ʶ
#define IDC_CM_BUTTON_5000000		205									//��ť��ʶ
#define IDC_CM_BUTTON_10000000		206									//��ť��ʶ
#define IDC_APPY_BANKER				207									//��ť��ʶ
#define IDC_CANCEL_BANKER			208									//��ť��ʶ
#define IDC_SCORE_MOVE_L			209									//��ť��ʶ
#define IDC_SCORE_MOVE_R			210									//��ť��ʶ
#define IDC_VIEW_CHART				211									//��ť��ʶ
#define IDC_BANK_STORAGE			212									//��ť��ʶ
#define IDC_BANK_DRAW				213									//��ť��ʶ
#define IDC_BANK					214									//���а�ť

#define IDC_UP						223									//��ť��ʶ
#define IDC_DOWN					224									//��ť��ʶ

#define IDC_OPTION					225									//���ð�ť��ʶ
#define IDC_MIN						226									//���ð�ť��ʶ
#define IDC_CLOSE					227									//���ð�ť��ʶ
#define IDC_SOUND					228									//�������
#define IDC_MAX                     229
#define IDC_RESTORE					230	

#define IDC_CHAT_DISPLAY			240									//�ؼ���ʶ
#define IDC_USER_LIST				241									//�ؼ���ʶ

#define IDC_SEND					242									//���Ͱ�ť
#define IDC_CHAT_INPUT				243									//��������

//��ը��Ŀ
#define BOMB_EFFECT_COUNT			8									//��ը��Ŀ
#define DEAL_MOVE_COUNT_H			5									//���������
#define DEAL_MOVE_COUNT_S			5									//���������
#define FLEX_MOVE_COUNT				10									//���������




//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_BT_ADMIN,OpenAdminWnd)
	ON_BN_CLICKED(IDC_CM_BUTTON_100, OnBetButton100)
	ON_BN_CLICKED(IDC_CM_BUTTON_1000, OnBetButton1000)
	ON_BN_CLICKED(IDC_CM_BUTTON_10000, OnBetButton10000)
	ON_BN_CLICKED(IDC_CM_BUTTON_100000, OnBetButton100000)
	ON_BN_CLICKED(IDC_CM_BUTTON_1000000, OnBetButton1000000)
	ON_BN_CLICKED(IDC_CM_BUTTON_5000000, OnBetButton5000000)
	ON_BN_CLICKED(IDC_CM_BUTTON_10000000, OnBetButton10000000)

	ON_BN_CLICKED(IDC_VIEW_CHART, OnViewChart)
	ON_BN_CLICKED(IDC_APPY_BANKER, OnApplyBanker)
	ON_BN_CLICKED(IDC_CANCEL_BANKER, OnCancelBanker)
	ON_BN_CLICKED(IDC_SCORE_MOVE_L, OnScoreMoveL)
	ON_BN_CLICKED(IDC_SCORE_MOVE_R, OnScoreMoveR)

	ON_BN_CLICKED(IDC_UP, OnValleysUp)
	ON_BN_CLICKED(IDC_DOWN, OnValleysDown)

	ON_BN_CLICKED(IDC_BANK_STORAGE, OnBankStorage)
	//ON_BN_CLICKED(IDC_BANK_DRAW, OnBankDraw)

	ON_BN_CLICKED(IDC_OPTION, OnButtonGameOption)
	ON_BN_CLICKED(IDC_MIN, OnButtonMin)
	ON_BN_CLICKED(IDC_CLOSE, OnButtonClose)
	ON_BN_CLICKED(IDC_BANK, OnBankDraw)

	ON_MESSAGE(WM_SET_CAPTION, OnSetCaption)

	ON_WM_CTLCOLOR()

	ON_WM_LBUTTONDBLCLK()
	//ON_WM_SIZE()

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientView::CGameClientView()
{
	//������Ϣ
	m_lMeMaxScore = 0L;
	m_lAreaLimitScore = 0L;

	//��ע��Ϣ
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	//���������ע
	ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));
	ZeroMemory(m_bFillTrace,sizeof(m_bFillTrace));

	ZeroMemory(m_RightGameRecordArray,sizeof(m_RightGameRecordArray));

	m_RightGameRecordCount = 0;
		
	m_RecordResult.SetSize(MAX_RECORD_COL_COUNT);
	for(int i = 0;i < m_RecordResult.GetCount();i++)
	{
		for(int j = 0;j < 6;j++)
		{
			m_RecordResult[i].resultIndex[j] = INVALID_DWORD;
		}
	}

	//ׯ����Ϣ
	m_wBankerUser=INVALID_CHAIR;		
	m_wBankerTime=0;
	m_lBankerScore=0L;	
	m_lBankerWinScore=0L;
	m_lBankerWinTempScore=0L;
	m_cbJetArea = 0xFF;
	m_bFlashResult=false;
	m_lBankerCurGameScore = 0;

	//���ֳɼ�
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	m_lPlayAllScore = 0;
	m_lRevenue = 0;

	//��������
	m_nDealMove = 0;
	m_nDealIndex = 0;
	m_ptDispatchCard.SetPoint(0,0);


	//��������
	m_nFlexMove = 0;
	m_bFlexShow = false;
	m_ptFlexBeing.SetPoint(0,0);
	m_ptFlexMove.SetPoint(0,0);
	m_pImageFlex = NULL;

	//״̬��Ϣ
	m_nWinCount = 0;
	m_nLoseCount = 0;
	m_cbGameStatus = GAME_SCENE_FREE;
	m_lCurrentBet = 0L;
	m_wMeChairID = INVALID_CHAIR;
	m_bShowChangeBanker = false;
	m_bNeedSetGameRecord = false;
	m_bEnableBet = true;

	//������
	m_ArrayCurrentFlash.RemoveAll();
	m_ArrayFlashArea.RemoveAll();
	m_nFlashAreaAlpha = 0;
	m_bFlashAreaAlpha = false;

	//���ֳɼ�
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	m_lPlayAllScore = 0;	

	//��������
	m_nWinShowArea = INT_MAX;
	ZeroMemory(m_nWinShowIndex, sizeof(m_nWinShowIndex));

	//λ����Ϣ
	m_nRecordHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	
	m_sizeWin.SetSize(0,0);

	//��ʷ�ɼ�
	m_lMeStatisticScore=0;
	m_nTotalCol = 0;
	m_nHeadCol = 0;

	// ��ׯ�б�
	m_nShowValleyIndex = 0;

	// ����
	m_pClientControlDlg = NULL;
	m_hControlInst = NULL;

	m_dwChatTime = 0;

	return;
}

//��������
CGameClientView::~CGameClientView(void)
{
	if( m_pClientControlDlg )
	{
		delete m_pClientControlDlg;
		m_pClientControlDlg = NULL;
	}

	if( m_hControlInst )
	{
		FreeLibrary(m_hControlInst);
		m_hControlInst = NULL;
	}
}

//������Ϣ
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1) return -1;

	//����λͼ
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageViewFill.LoadFromResource( hInstance,IDB_VIEW_FILL);



	m_ImageViewBack.LoadImage( hInstance,TEXT("VIEW_BACK"));	
	m_ImageHistoryFront.LoadImage(hInstance,TEXT("HISTORY_RESULT_FRONT"));
	m_ImageHistoryMid.LoadImage(hInstance,TEXT("HISTORY_RESULT_MID"));
	m_ImageHistoryBack.LoadImage(hInstance,TEXT("HISTORY_RESULT_BACK"));

	m_ImageChipInfo.LoadImage( hInstance,TEXT("CHIP_INFO"));				
	m_ImagePlayerResultInfo.LoadImage( hInstance,TEXT("PLAYER_RESULTINFO"));           
	m_ImageBankerResultInfo.LoadImage( hInstance,TEXT("BANKER_RESULTINFO"));

	m_ImageViewBackJB.LoadImage( hInstance,TEXT("VIEW_BACK_JB"));	  
	m_ImageWinFlags.LoadImage( hInstance,TEXT("WIN_FLAGS"));
	m_ImageBetView.LoadImage( hInstance,TEXT("BET_VIEW")); 
	m_ImageScoreNumber.LoadImage( hInstance,TEXT("SCORE_NUMBER"));	
	m_ImageScoreBack.LoadImage( hInstance,TEXT("SCORE_BACK"));	
	m_ImageMeScoreNumber.LoadImage( hInstance,TEXT("ME_SCORE_NUMBER"));	
	m_ImageMeScoreBack.LoadImage( hInstance,TEXT("ME_SCORE_BACK"));
	m_ImgaeGameTimeCount.LoadImage(hInstance,TEXT("GAME_NUM"));

	m_ImageFrame[AREA_XIAN].LoadImage(  hInstance,TEXT("FRAME_XIAN_JIA") );
	m_ImageFrame[AREA_ZHUANG].LoadImage(  hInstance,TEXT("FRAME_ZHUANG_JIA") );
	m_ImageFrame[AREA_XIAN_TIAN].LoadImage(  hInstance,TEXT("FRAME_XIAN_TIAN_WANG") );
	m_ImageFrame[AREA_ZHUANG_TIAN].LoadImage(  hInstance,TEXT("FRAME_ZHUANG_TIAN_WANG") );
	m_ImageFrame[AREA_PING].LoadImage(  hInstance,TEXT("FRAME_PING_JIA") );
	m_ImageFrame[AREA_TONG_DUI].LoadImage(  hInstance,TEXT("FRAME_TONG_DIAN_PING") );
	m_ImageFrame[AREA_XIAN_DUI].LoadImage(  hInstance,TEXT("FRAME_PLAYER_TWO_PAIR") );
	m_ImageFrame[AREA_TIANGANG].LoadImage(  hInstance,TEXT("FRAME_BANKER_TWO_PAIR") );

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

	m_ImageGameEnd.LoadImage(  hInstance, TEXT("GAME_END") );
	m_ImageDealBack.LoadImage( hInstance,TEXT("GAME_END_FRAME"));
	m_ImageGamePoint.LoadImage(  hInstance,TEXT("GAME_POINT") );  

	m_ImageGameEndMyScore.LoadImage(  hInstance, TEXT("GAME_END_MY_SCORE") );
	m_ImageGameEndAllScore.LoadImage(  hInstance, TEXT("GAME_END_ALL_SCORE") );
	m_ImageGameEndPoint.LoadImage(  hInstance, TEXT("GAME_END_CRAD_POINT") );

	m_ImageWinType.LoadImage(  hInstance, TEXT("WIN_TYPE") );					
	m_ImageWinXian.LoadImage(  hInstance, TEXT("WIN_XIAN") );					
	m_ImageWinZhuang.LoadImage(  hInstance, TEXT("WIN_ZHUANG") );				

	m_ImageBetTip.LoadImage( hInstance,TEXT("BET_TIP"));

	m_ImageMeBanker.LoadImage(  hInstance, TEXT("ME_BANKER") );
	m_ImageChangeBanker.LoadImage(  hInstance, TEXT("CHANGE_BANKER") );
	m_ImageNoBanker.LoadImage( hInstance, TEXT("NO_BANKER") );	

	m_ImageBrandBoxRight.LoadImage( hInstance,TEXT("BRAND_BOX_RIGHT"));

	m_ImageTimeNumber.LoadImage( hInstance,TEXT("TIME_NUMBER"));
	m_ImageTimeFlag.LoadImage( hInstance,TEXT("TIME_FLAG"));
	m_ImageTimeBack.LoadImage( hInstance,TEXT("TIME_BACK"));
	m_ImageTimeType.LoadImage( hInstance,TEXT("TIME_TYPE"));

	m_ImagePlayLeft.LoadImage( hInstance,TEXT("PLAY_LEFT"));
	m_ImagePlayMiddle.LoadImage( hInstance,TEXT("PLAY_MIDDLE"));
	m_ImagePlayRight.LoadImage( hInstance,TEXT("PLAY_RIGHT"));

	m_ImageScoreInfo.LoadImage( hInstance,TEXT("SCORE_INFO"));
	m_ImageBankerInfo.LoadImage( hInstance,TEXT("BANKER_INFO"));
	m_ImageWaitValleys.LoadImage( hInstance,TEXT("WAIT_VALLEYS"));
	m_ImageWaitFirst.LoadImage( hInstance,TEXT("WAIT_FIRST"));

	//����ͼƬ
	m_PngPlayerFlag.LoadImage(hInstance,TEXT("BOTTOM_XIAN"));
	m_PngPlayerFlagTowPair.LoadImage(hInstance,TEXT("BOTTOM_XIAN_TOW_PAIR"));
	m_PngPlayerEXFlag.LoadImage(hInstance,TEXT("BOTTOM_XIAN_EX"));
	m_PngPlayerEXFlagTowPair.LoadImage(hInstance,TEXT("BOTTOM_XIAN_EX_TOW_PAIR"));
	m_PngBankerFlag.LoadImage(hInstance,TEXT("BOTTOM_ZHUANG"));
	m_PngBankerFlagTowPair.LoadImage(hInstance,TEXT("BOTTOM_ZHUANG_TOW_PAIR"));
	m_PngBankerEXFlag.LoadImage(hInstance,TEXT("BOTTOM_ZHUANG_EX"));
	m_PngBankerEXFlagTowPair.LoadImage(hInstance,TEXT("BOTTOM_ZHUANG_EX_TOW_PAIR"));
	m_PngTieFlag.LoadImage(hInstance,TEXT("BOTTOM_PING"));
	m_PngTieEXFlag.LoadImage(hInstance,TEXT("BOTTOM_PING_EX"));
	m_PngTwopairFlag.LoadImage(hInstance,TEXT("TWO_PAIR_FLAG"));

	//�����ؼ�
	CRect rcCreate(0,0,0,0);

	//��ע��ť
	m_btBet100.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_100);
	m_btBet1000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_1000);
	m_btBet10000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_10000);
	m_btBet100000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_100000);
	m_btBet1000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_1000000);
	m_btBet5000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_5000000);
	m_btBet10000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CM_BUTTON_10000000);
	m_btViewChart.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_VIEW_CHART);
	m_btBank.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_BANK);
	m_btBet100.ShowWindow(SW_HIDE);
	m_btSend.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SEND);
	m_ChatInput.Create(ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER, rcCreate, this, IDC_CHAT_INPUT);
	m_ChatInput.SetLimitText(LEN_USER_CHAT/2-1);

	//��ܰ�ť
	m_btOption.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OPTION);
	m_btMin.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MIN);

	m_btMax.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MAX);
	m_btRestore.Create(NULL,WS_CHILD,rcCreate,this,IDC_RESTORE);

	m_btClose.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_CLOSE);
	m_btSound.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SOUND);
		
	//���밴ť
	m_btApplyBanker.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_APPY_BANKER);
	m_btCancelBanker.Create(NULL,WS_CHILD|WS_DISABLED,rcCreate,this,IDC_CANCEL_BANKER);

	m_btScoreMoveL.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_SCORE_MOVE_L);
	m_btScoreMoveR.Create(NULL,WS_CHILD|WS_DISABLED|WS_VISIBLE,rcCreate,this,IDC_SCORE_MOVE_R);

	m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(4,31,11,38),this,IDC_BT_ADMIN);
	m_btOpenAdmin.ShowWindow(SW_HIDE);
	
	m_btValleysUp.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_UP);
	m_btValleysDown.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_DOWN);

	//���ð�ť
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btBet100.SetButtonImage(IDB_BT_CM_100,hResInstance,false,false);
	m_btBet1000.SetButtonImage(IDB_BT_CM_1000,hResInstance,false,false);
	m_btBet10000.SetButtonImage(IDB_BT_CM_10000,hResInstance,false,false);
	m_btBet100000.SetButtonImage(IDB_BT_CM_100000,hResInstance,false,false);
	m_btBet1000000.SetButtonImage(IDB_BT_CM_1000000,hResInstance,false,false);
	m_btBet5000000.SetButtonImage(IDB_BT_CM_5000000,hResInstance,false,false);
	m_btBet10000000.SetButtonImage(IDB_BT_CM_10000000,hResInstance,false,false);
	m_btSend.SetButtonImage(IDB_BT_SEND,hResInstance,false,false);

	m_btOption.SetButtonImage(IDB_BT_OPTION,hResInstance,false,false);
	m_btMin.SetButtonImage(IDB_BT_MIN,hResInstance,false,false);
	m_btMax.SetButtonImage(IDB_BT_MAX,hResInstance,false,false);
	m_btRestore.SetButtonImage(IDB_BT_RESTORE,hResInstance,false,false);
	m_btClose.SetButtonImage(IDB_BT_CLOSE,hResInstance,false,false);
	m_btBank.SetButtonImage(IDB_BT_BANK,hResInstance,false,false);
	CGlobalUnits* pGlabalUnits = CGlobalUnits::GetInstance();
	m_btSound.SetButtonImage(!pGlabalUnits->m_bMuteStatuts?IDB_BT_SOUND_OPEN:IDB_BT_SOUND_CLOSE,hResInstance,false,false);

	m_btViewChart.SetButtonImage(IDB_BT_CHART,hResInstance,false,false);	

	m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false,true);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,hResInstance,false,true);

	m_btScoreMoveL.SetButtonImage(IDB_BT_SCORE_MOVE_L,hResInstance,false,false);
	m_btScoreMoveR.SetButtonImage(IDB_BT_SCORE_MOVE_R,hResInstance,false,false);

	m_btValleysUp.SetButtonImage(IDB_BT_BT_S,hResInstance,false,false);
	m_btValleysDown.SetButtonImage(IDB_BT_BT_X,hResInstance,false,false);

	m_DispatchCard.Create(this);
	m_CardControl[INDEX_PLAYER].Create(this);
	m_CardControl[INDEX_BANKER].Create(this);

	// ����
	m_FontValleysListOne.CreateFont( this, TEXT("����"), 16, 600 );
	m_FontValleysListTwo.CreateFont( this, TEXT("����"), 12, 400 );
	m_FontCaption.CreateFont( this, TEXT("����"), 12, 400 );


	//�˿˿ؼ�
	m_CardControl[INDEX_PLAYER].SetDisplayFlag(true);
	m_CardControl[INDEX_BANKER].SetDisplayFlag(true);

	m_DispatchCard.SetDisplayFlag(false);

	m_ChatDisplay.Create(ES_AUTOVSCROLL|ES_WANTRETURN|ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP |WS_VSCROLL, rcCreate, this, IDC_CHAT_DISPLAY);

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));      
	lf.lfHeight = 15;                      
	mystrcpy(lf.lfFaceName, TEXT("Arial"));
	m_brush.CreateSolidBrush(RGB(106,58,26));

	m_ChatDisplay.SetTextFont(lf);
	//m_ChatDisplay.SetBackColor(RGB(88,68,43));
	m_ChatDisplay.SetTextColor(RGB(255,255,255));

	m_ChatDisplay.SetBackgroundColor(false,RGB(31,10,5));
	m_ChatDisplay.SetReadOnly(TRUE);

	m_UserList.Create(LVS_REPORT | LVS_OWNERDRAWFIXED | WS_CHILD | WS_VISIBLE ,rcCreate, this, IDC_USER_LIST);
	m_UserList.SetBkColor(RGB(31,10,5));
	m_UserList.SetTextColor(RGB(255,255,255));
	m_UserList.SetTextBkColor(RGB(31,10,5));
	int nColumnCount=0;

	//�û���־
	m_UserList.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,35,0);
	m_UserList.InsertColumn(nColumnCount++,TEXT("�ǳ�"),LVCFMT_LEFT,75);
	m_UserList.InsertColumn(nColumnCount++,TEXT("�Ƹ�"),LVCFMT_LEFT,95);

	//·���ؼ�
	if (m_DlgViewChart.m_hWnd == NULL) m_DlgViewChart.Create(IDD_VIEW_CHART,this);

	//����
	m_hControlInst = NULL;
	m_pClientControlDlg = NULL;
	m_hControlInst = LoadLibrary(TEXT("BaccaratNewClientControl.dll"));
	if ( m_hControlInst )
	{
		typedef void * (*CREATE)(CWnd* pParentWnd); 
		CREATE ClientControl = (CREATE)GetProcAddress(m_hControlInst,"CreateClientControl"); 
		if ( ClientControl )
		{
			m_pClientControlDlg = static_cast<IClientControlDlg*>(ClientControl(this));
			m_pClientControlDlg->ShowWindow( SW_HIDE );
		}
	}

	m_btBankerStorage.Create(NULL,WS_CHILD,rcCreate,this,IDC_BANK_STORAGE);
	m_btBankerDraw.Create(NULL,WS_CHILD,rcCreate,this,IDC_BANK_DRAW);

	m_btBankerStorage.SetButtonImage(IDB_BT_STORAGE,hResInstance,false,false);
	m_btBankerDraw.SetButtonImage(IDB_BT_DRAW,hResInstance,false,false);

//#ifdef _DEBUG
//	for(int i = 0; i < 100; ++i)
//	{
//		BYTE cbPlayerCount = rand()%10;
//		BYTE cbBankerCount = rand()%10;
//		SetGameHistory(enOperateResult_NULL,cbPlayerCount,cbBankerCount,0,FALSE,FALSE);
//		SetRecordCol();
//	}
//#endif // _DEBUG

	return 0;
}

//���ý���
VOID CGameClientView::ResetGameView()
{
	//��ע��Ϣ
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	//���������ע
	ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));
	ZeroMemory(m_bFillTrace,sizeof(m_bFillTrace));
	
	m_RecordResult.SetSize(MAX_RECORD_COL_COUNT);
	for(int i = 0;i < m_RecordResult.GetCount();i++)
	{
		for(int j = 0;j < 6;j++)
		{
			m_RecordResult[i].resultIndex[j] = INVALID_DWORD;
		}
	}

	//ׯ����Ϣ
	m_wBankerUser=INVALID_CHAIR;		
	m_wBankerTime=0;
	m_lBankerScore=0L;	
	m_lBankerWinScore=0L;
	m_lBankerWinTempScore=0L;
	m_cbJetArea = 0xFF;
	m_bFlashResult=false;
	m_lBankerCurGameScore = 0;

	//���ֳɼ�
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	m_lPlayAllScore = 0;
	m_lRevenue = 0;

	//��������
	m_nDealMove = 0;
	m_nDealIndex = 0;

	//��������
	m_nFlexMove = 0;
	m_bFlexShow = false;
	m_ptFlexBeing.SetPoint(0,0);
	m_ptFlexMove.SetPoint(0,0);
	m_pImageFlex = NULL;

	//״̬��Ϣ
	m_nWinCount = 0;
	m_nLoseCount = 0;
	m_cbGameStatus = GAME_SCENE_FREE;
	m_lCurrentBet=0L;
	m_wMeChairID=INVALID_CHAIR;
	m_bShowChangeBanker=false;
	m_bNeedSetGameRecord=false;
	m_lAreaLimitScore=0L;	

	//������
	m_ArrayCurrentFlash.RemoveAll();
	m_ArrayFlashArea.RemoveAll();
	m_nFlashAreaAlpha = 0;
	m_bFlashAreaAlpha = false;

	//��������
	m_nWinShowArea = INT_MAX;
	ZeroMemory(m_nWinShowIndex, sizeof(m_nWinShowIndex));

	//λ����Ϣ
	m_nRecordHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	

	//��ʷ�ɼ�
	m_lMeStatisticScore=0;

	//����б�
	m_ValleysList.RemoveAll();
	m_btValleysUp.EnableWindow(false);
	m_btValleysDown.EnableWindow(false);

	// ��ׯ�б�
	m_nShowValleyIndex = 0;

	//�������
	CleanUserBet();

	//���ð�ť
	m_btApplyBanker.ShowWindow(SW_SHOW);
	m_btApplyBanker.EnableWindow(FALSE);
	m_btCancelBanker.ShowWindow(SW_HIDE);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);

}

//�����ؼ�
VOID CGameClientView::RectifyControl(int nWidth, int nHeight)
{
	//����λ��
	CPoint ptBenchmark( nWidth/2 , nHeight/2  );
	CRect rectArea;
	if(!IsZoomedMode())
	{
		ptBenchmark.SetPoint(nWidth/2 , nHeight/2);
	}
	else // ȫ������
	{
		int cx = GetSystemMetrics(SM_CXSCREEN);   
		int cy = GetSystemMetrics(SM_CYSCREEN);
		ptBenchmark.SetPoint(cx/2,LESS_SCREEN_CY/2);
	}
	
	for ( int i = 0; i < AREA_MAX; ++i )
	{
		m_ArrayBetArea[i].RemoveAll();
	}

	//�ж���
	rectArea.SetRect( ptBenchmark.x - 332, ptBenchmark.y - 193, ptBenchmark.x - 332 + 226, ptBenchmark.y - 193 + 60);
	m_rcXianDui.SetRect( ptBenchmark.x - 340, ptBenchmark.y - 195, ptBenchmark.x - 340 + 226, ptBenchmark.y - 195 + 60);
	m_ArrayBetArea[AREA_XIAN_DUI].Add(rectArea);

	//ׯ����
	rectArea.SetRect( ptBenchmark.x + 116, ptBenchmark.y - 193, ptBenchmark.x + 116 + 226, ptBenchmark.y - 193 + 60);
	m_rcZhuangDui.SetRect( ptBenchmark.x + 143, ptBenchmark.y - 195, ptBenchmark.x + 143 + 226, ptBenchmark.y - 195 + 60);
	m_ArrayBetArea[AREA_TIANGANG].Add(rectArea);

	//�м�
	rectArea.SetRect( ptBenchmark.x - 354, ptBenchmark.y - 111, ptBenchmark.x - 354 + 227, ptBenchmark.y - 111 + 111);
	m_rcXian.SetRect( ptBenchmark.x - 332, ptBenchmark.y - 141, ptBenchmark.x - 332 + 227, ptBenchmark.y - 141 + 111);	
	m_ArrayBetArea[AREA_XIAN].Add(rectArea);

	//ƽ��
	rectArea.SetRect( ptBenchmark.x - 111, ptBenchmark.y - 107, ptBenchmark.x - 111 + 225, ptBenchmark.y - 107 + 138);
	m_rcPing.SetRect( ptBenchmark.x - 89, ptBenchmark.y - 112, ptBenchmark.x - 89 + 225, ptBenchmark.y - 112 + 138);


	//INT PingPosX = nWidth/2 - m_ImageFrame[AREA_PING].GetWidth()/2 + 1;
	//m_rcPing.SetRect(PingPosX,261,m_ImageFrame[AREA_PING].GetWidth(),m_ImageFrame[AREA_PING].GetHeight());	
	m_ArrayBetArea[AREA_PING].Add(rectArea);

	//ׯ��
	rectArea.SetRect( ptBenchmark.x + 132, ptBenchmark.y - 110, ptBenchmark.x + 132 + 227, ptBenchmark.y - 110 + 111);
	m_rcZhuang.SetRect( ptBenchmark.x + 152, ptBenchmark.y - 156, ptBenchmark.x + 152 + 227, ptBenchmark.y - 156 + 111);
	m_ArrayBetArea[AREA_ZHUANG].Add(rectArea);

	//������
	rectArea.SetRect( ptBenchmark.x - 312, ptBenchmark.y + 31, ptBenchmark.x - 312 + 178, ptBenchmark.y + 31 + 52);
	m_rcXianTian.SetRect( ptBenchmark.x - 299, ptBenchmark.y + 5, ptBenchmark.x - 299 + 178, ptBenchmark.y + 5 + 52);	
	m_ArrayBetArea[AREA_XIAN_TIAN].Add(rectArea);

	//ͬ��ƽ	
	rectArea.SetRect( ptBenchmark.x - 124, ptBenchmark.y + 35, ptBenchmark.x - 124 + 254, ptBenchmark.y + 35 + 60);
	m_rcTongDui.SetRect( ptBenchmark.x - 97, ptBenchmark.y + 29, ptBenchmark.x - 97 + 254, ptBenchmark.y + 29 + 60);
	m_ArrayBetArea[AREA_TONG_DUI].Add(rectArea);

	//ׯ����
	rectArea.SetRect( ptBenchmark.x + 138, ptBenchmark.y + 31, ptBenchmark.x + 138 + 178, ptBenchmark.y + 31 + 52);
	m_rcZhuangTian.SetRect( ptBenchmark.x + 166, ptBenchmark.y + 5, ptBenchmark.x + 166 + 178, ptBenchmark.y + 5 + 52);
	m_ArrayBetArea[AREA_ZHUANG_TIAN].Add(rectArea);

	//��������
	m_ptBetNumber[AREA_XIAN].SetPoint( ptBenchmark.x - 240, ptBenchmark.y - 60 );
	m_ptBetNumber[AREA_XIAN_TIAN].SetPoint( ptBenchmark.x - 240, ptBenchmark.y + 45 );
	m_ptBetNumber[AREA_XIAN_DUI].SetPoint( ptBenchmark.x - 240, ptBenchmark.y - 168 );

	m_ptBetNumber[AREA_ZHUANG].SetPoint( ptBenchmark.x + 232, ptBenchmark.y - 60 );
	m_ptBetNumber[AREA_ZHUANG_TIAN].SetPoint( ptBenchmark.x + 232, ptBenchmark.y + 45 );
	m_ptBetNumber[AREA_TIANGANG].SetPoint( ptBenchmark.x + 232, ptBenchmark.y - 168 );

	m_ptBetNumber[AREA_PING].SetPoint( ptBenchmark.x , ptBenchmark.y - 60 );
	m_ptBetNumber[AREA_TONG_DUI].SetPoint( ptBenchmark.x , ptBenchmark.y + 60 );

	//�ܳ�������
	m_ptAllBetNumber[AREA_XIAN].SetPoint( ptBenchmark.x - 230, ptBenchmark.y );
	m_ptAllBetNumber[AREA_XIAN_TIAN].SetPoint( ptBenchmark.x - 230, ptBenchmark.y + 73 );
	m_ptAllBetNumber[AREA_XIAN_DUI].SetPoint( ptBenchmark.x - 230, ptBenchmark.y - 138 );

	m_ptAllBetNumber[AREA_ZHUANG].SetPoint( ptBenchmark.x + 232, ptBenchmark.y );
	m_ptAllBetNumber[AREA_ZHUANG_TIAN].SetPoint( ptBenchmark.x + 232, ptBenchmark.y + 73 );
	m_ptAllBetNumber[AREA_TIANGANG].SetPoint( ptBenchmark.x + 232, ptBenchmark.y - 138 );

	m_ptAllBetNumber[AREA_PING].SetPoint( ptBenchmark.x , ptBenchmark.y +18 );
	m_ptAllBetNumber[AREA_TONG_DUI].SetPoint( ptBenchmark.x , ptBenchmark.y + 90 );


	//������λ��
	m_ptBetFrame[AREA_XIAN].SetPoint( ptBenchmark.x - 332-34, ptBenchmark.y - 141+2 );
	m_ptBetFrame[AREA_XIAN_TIAN].SetPoint( ptBenchmark.x - 299-34, ptBenchmark.y + 5+2 );
	m_ptBetFrame[AREA_XIAN_DUI].SetPoint( ptBenchmark.x - 340-34-11, ptBenchmark.y - 195+2 );

	m_ptBetFrame[AREA_ZHUANG].SetPoint( ptBenchmark.x + 152-34, ptBenchmark.y - 156+2+15 );
	m_ptBetFrame[AREA_ZHUANG_TIAN].SetPoint( ptBenchmark.x + 166-34+1, ptBenchmark.y + 5+2 );
	m_ptBetFrame[AREA_TIANGANG].SetPoint( ptBenchmark.x + 143-34, ptBenchmark.y - 195+2 );

	m_ptBetFrame[AREA_PING].SetPoint( ptBenchmark.x - 89-34, ptBenchmark.y - 112+2 );
	m_ptBetFrame[AREA_TONG_DUI].SetPoint( ptBenchmark.x - 97-34, ptBenchmark.y + 29+2 );
	
	//�˿˿ؼ�
	m_CardControl[0].SetBenchmarkPos(nWidth/2 - 100, 340);
	m_CardControl[1].SetBenchmarkPos(nWidth/2 + 100, 340);

	//�û��б�λ��
	m_RectUserList.SetRect(12,nHeight-148-5+14,12+202,nHeight-148-5+14+125);
	//������ʾλ��
	int startX = nWidth-m_ImageChatBack.GetWidth()+2;
	int startY = nHeight-m_ImageChatBack.GetHeight()+22;
	m_RectChartDisplay.SetRect(startX,startY,startX+181,startY+112);

	//���ƿ�ʼ��
	m_ptDispatchCard.SetPoint( nWidth/2 - 12, 120 );

	//����λ��
	m_ptFlexBeing.SetPoint( nWidth/2, 0);
	m_ptFlexMove.x = nWidth/2;

	//�ƶ��ؼ�
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	//�б�ؼ�
	DeferWindowPos(hDwp, m_btValleysUp,   NULL,110+18, 175, 0, 0, uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp, m_btValleysDown, NULL,110, 175, 0, 0, uFlags|SWP_NOSIZE);

	//���밴ť
	CRect rcBet;
	m_btBet100.GetWindowRect(&rcBet);
	int nSpace = 7;
	int nYPos = nHeight - 165;
	int nXPos = nWidth/2 - (rcBet.Width()*7 + nSpace*6)/2 + 85;
	DeferWindowPos(hDwp,m_btBet100,NULL,nXPos,nYPos,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btBet1000,NULL,nWidth/2 - 320 +7,470 ,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBet10000,NULL,nWidth/2 - 206,511,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBet100000,NULL,nWidth/2 - 96,524,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBet1000000,NULL,nWidth/2 + 96 - 89,524,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBet5000000,NULL,nWidth/2 + 206 - 89-1,512,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBet10000000,NULL,nWidth/2 + 320 - 89 - 5,470,0,0,uFlags|SWP_NOSIZE);

	//��ׯ��ť
	DeferWindowPos(hDwp,m_btApplyBanker, NULL, 7, 280, 0, 0, uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btCancelBanker,NULL, 7, 280, 0, 0, uFlags|SWP_NOSIZE);

	//��ʷ��¼
	DeferWindowPos(hDwp,m_btScoreMoveL, NULL,IsZoomedMode()? 233+CalFrameGap():233, nHeight - 141,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btScoreMoveR, NULL,IsZoomedMode()? nWidth - (233+CalFrameGap()):nWidth - 233,  nHeight - 141,0,0,uFlags|SWP_NOSIZE);

	//�鿴·��
	DeferWindowPos(hDwp,m_btViewChart,NULL, nWidth - 17-84,nHeight - 241,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBank,NULL, 17,nHeight - 247,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBankerStorage,NULL,nWidth/2-123,nHeight-2*rcBet.Height()+23,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btBankerDraw,NULL,nWidth/2-123,nHeight-rcBet.Height()-5,0,0,uFlags|SWP_NOSIZE);

	//��ܰ�ť
	DeferWindowPos(hDwp,m_btMin,NULL, nWidth - 10-25*4-5*3,2,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btMax,NULL, nWidth - 10-25*3-5*2,2,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btRestore,NULL, nWidth - 10-25*3-5*2,2,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btSound,NULL, nWidth - 10-25*2-5,2,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btClose,NULL, nWidth - 10-25,2,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btOption,NULL, nWidth - 10-25*5-5*4,2,0,0,uFlags|SWP_NOSIZE);

	//�б�ؼ�
	DeferWindowPos(hDwp,m_UserList,NULL,m_RectUserList.left,m_RectUserList.top,m_RectUserList.Width(),m_RectUserList.Height(),uFlags);
	DeferWindowPos(hDwp,m_ChatDisplay,NULL,m_RectChartDisplay.left,m_RectChartDisplay.top,m_RectChartDisplay.Width()/*-16*/,m_RectChartDisplay.Height()-20,uFlags);
	DeferWindowPos(hDwp,m_btSend,NULL,nWidth-47,nHeight-32,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_ChatInput,NULL,nWidth-192,nHeight-32,145,20,uFlags);

	//�����ƶ�
	EndDeferWindowPos(hDwp);

	// �任����λ��
	CPoint ptOffset( (nWidth - m_sizeWin.cx)/2,(nHeight - m_sizeWin.cy)/2 ); 
	for ( int i = 0; i < AREA_MAX; ++i )
	{
		for ( int j = 0; j < m_BetInfoArray[i].GetCount(); ++j )
		{
			m_BetInfoArray[i][j].nXPos += ptOffset.x;
			m_BetInfoArray[i][j].nYPos += ptOffset.y;
		}
	}

	// ���ڴ�С
	m_sizeWin.SetSize(nWidth, nHeight);

	if(m_DlgViewChart.GetSafeHwnd())
		m_DlgViewChart.ShowWindow(SW_HIDE);

	//���ƿͻ���
	if ( m_pClientControlDlg )
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
	
	if(!IsZoomedMode() && CalLastRecordHorCount() > NORMAL_RECORD_COL_COUNT)
	{
		m_btScoreMoveL.EnableWindow(TRUE);
	}
	if(IsZoomedMode() && CalLastRecordHorCount() > ZOOMED_RECORD_COL_COUNT)
	{
		m_btScoreMoveL.EnableWindow(TRUE);
	}
	if(!IsZoomedMode() && CalLastRecordHorCount() > NORMAL_RECORD_COL_COUNT)
	{
		m_btScoreMoveL.EnableWindow(TRUE);
		m_btScoreMoveR.EnableWindow(FALSE);
		m_nRecordHead = (CalLastRecordHorCount() - NORMAL_RECORD_COL_COUNT+MAX_RECORD_COL_COUNT) % MAX_RECORD_COL_COUNT;
	}
	if( IsZoomedMode() && CalLastRecordHorCount() > ZOOMED_RECORD_COL_COUNT)
	{
		m_btScoreMoveL.EnableWindow(TRUE);
		m_btScoreMoveR.EnableWindow(FALSE);
		m_nRecordHead = (CalLastRecordHorCount() - ZOOMED_RECORD_COL_COUNT+MAX_RECORD_COL_COUNT) % MAX_RECORD_COL_COUNT;
	}
	if( IsZoomedMode() && CalLastRecordHorCount() <= ZOOMED_RECORD_COL_COUNT)
	{
		m_nRecordHead = 0;
		m_btScoreMoveR.EnableWindow(FALSE);
		m_btScoreMoveL.EnableWindow(FALSE);
	}


	return;
}

//�滭����
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	int nTime = GetTickCount();
	m_ImageViewFill.DrawImageTile(pDC,0,0,nWidth,nWidth);
	m_ImageViewBack.DrawImage( pDC, nWidth/2 - m_ImageViewBack.GetWidth()/2, 35);
	m_ImageChipInfo.DrawImage( pDC, nWidth/2 - m_ImageChipInfo.GetWidth()/2, 35);

	m_ImageBankerResultInfo.DrawImage( pDC, 0,35);
	m_ImagePlayerResultInfo.DrawImage( pDC, nWidth - m_ImagePlayerResultInfo.GetWidth(),35);

	INT nHistoryMidWidth = m_ImageHistoryMid.GetWidth(); //19
	INT	nHistoryMidHeight = m_ImageHistoryMid.GetHeight(); // 130

	INT nHistoryFrontWidth = m_ImageHistoryFront.GetWidth(); // 82
	INT nHistoryFrontHeight = m_ImageHistoryFront.GetHeight(); // 150
	INT nHistoryBackWidth = m_ImageHistoryBack.GetWidth();  // 84
	INT nHistoryBackHeight = m_ImageHistoryBack.GetHeight(); // 150
	
	INT nUserBackWidth = m_ImageUserBack.GetWidth();  // 216
	INT nChatBackWidth = m_ImageChatBack.GetWidth();  // 194
	// �������¼�϶ 5pixel

	
	// ����ģʽ�� // ������С�� 27��·��
	if(!IsZoomedMode())
	{
		m_ImageHistoryFront.DrawImage(pDC, nUserBackWidth+5,nHeight - nHistoryFrontHeight -5);
		for(WORD i = 0;i < 23;i++)
		{
			m_ImageHistoryMid.DrawImage(pDC,nUserBackWidth+5+nHistoryFrontWidth+i*nHistoryMidWidth,nHeight - nHistoryMidHeight - 6);
		}
		m_ImageHistoryBack.DrawImage(pDC,(nUserBackWidth+5)+nHistoryFrontWidth+23*nHistoryMidWidth,nHeight - nHistoryBackHeight -5);
	}
	else // ȫ���� 40 ��·��
	{
		// ����λ�� nGap �����ر������߶Գ�
		m_ImageHistoryFront.DrawImage(pDC, nUserBackWidth+5 + CalFrameGap(),nHeight - nHistoryFrontHeight -5);
		//INT nLuDanCount = (GetSystemMetrics(SM_CXSCREEN)-(nUserBackWidth+5) -(nChatBackWidth+5)-nHistoryFrontWidth-nHistoryBackWidth) / nHistoryMidWidth;
		INT nLuDanCount = ZOOMED_RECORD_COL_COUNT - 4;
		for(WORD i = 0;i < nLuDanCount;i++)
		{
			m_ImageHistoryMid.DrawImage(pDC,(nUserBackWidth+5)+CalFrameGap()+nHistoryFrontWidth+i*nHistoryMidWidth,nHeight - nHistoryMidHeight - 6);
		}

		m_ImageHistoryBack.DrawImage(pDC,(nUserBackWidth+5)+ CalFrameGap()+nHistoryFrontWidth+nLuDanCount*nHistoryMidWidth,nHeight - nHistoryBackHeight -5);
	}



	//ʤ���߿�
	DrawFlashFrame(pDC,nWidth,nHeight);
	JettonAreaFrame(pDC);	

	//������Դ
	CSize SizeBetItem(m_ImageBetView.GetWidth()/BET_COUNT,m_ImageBetView.GetHeight());

	//�滭����
	for (INT i=0;i<8;i++)
	{
		//��������
		LONGLONG lScoreCount=0L;
		LONGLONG lScoreBet[BET_COUNT]={1000L,10000L,100000L,1000000L,5000000L,10000000L};

		//�滭����
		for (INT_PTR j = 0;j< m_BetInfoArray[i].GetCount(); j++)
		{
			//��ȡ��Ϣ
			tagBetInfo * pBetInfo = &m_BetInfoArray[i][j];

			//�ۼ�����
			ASSERT(pBetInfo->cbBetIndex<BET_COUNT);
			lScoreCount += lScoreBet[pBetInfo->cbBetIndex];

			//�滭����
			m_ImageBetView.DrawImage(pDC,pBetInfo->nXPos - SizeBetItem.cx / 2,pBetInfo->nYPos - SizeBetItem.cy /2 ,
				SizeBetItem.cx, SizeBetItem.cy, pBetInfo->cbBetIndex*SizeBetItem.cx, 0);
		}

		//�滭����
		if ( lScoreCount > 0L )	
		{
			TCHAR szScore[128] = {0};
			_sntprintf(szScore,CountArray(szScore),TEXT("%s"), AddComma(lScoreCount));
			DrawNumber(pDC, &m_ImageScoreNumber,TEXT(",0123456789"), szScore, m_ptAllBetNumber[i].x + m_ImageScoreBack.GetWidth()/2, m_ptAllBetNumber[i].y - m_ImageScoreNumber.GetHeight() / 2, DT_CENTER);
			m_ImageScoreBack.DrawImage(pDC, m_ptAllBetNumber[i].x - (INT)(((DOUBLE)(lstrlen(szScore)) / 2.0) * (m_ImageScoreNumber.GetWidth() / 11)) - m_ImageScoreBack.GetWidth()/2 - 3, m_ptAllBetNumber[i].y  - m_ImageScoreBack.GetHeight() / 2 );
		
		}
	}

	//�ҵ���ע
	for ( int i = 0; i < AREA_MAX; ++i )
	{
		if ( m_lPlayBet[i] == 0 )
			continue;

		m_ImageMeScoreBack.DrawImage(pDC, m_ptBetNumber[i].x  - m_ImageMeScoreBack.GetWidth()/2, m_ptBetNumber[i].y - m_ImageMeScoreBack.GetHeight()/2);
		DrawNumber(pDC,&m_ImageMeScoreNumber, TEXT(",0123456789"), AddComma(m_lPlayBet[i]),m_ptBetNumber[i].x,m_ptBetNumber[i].y - m_ImageMeScoreNumber.GetHeight()/2, DT_CENTER);
	}

	//�л�ׯ��
	if ( m_bShowChangeBanker )
	{
		//������ׯ
		if ( m_wMeChairID == m_wBankerUser )
		{
			m_ImageMeBanker.DrawImage(pDC, nWidth / 2 - m_ImageMeBanker.GetWidth() / 2, nHeight / 2 - m_ImageMeBanker.GetHeight() / 2);
		}
		else if ( m_wBankerUser != INVALID_CHAIR )
		{
			m_ImageChangeBanker.DrawImage(pDC, nWidth / 2 - m_ImageChangeBanker.GetWidth() / 2, nHeight / 2 - m_ImageChangeBanker.GetHeight() / 2);
		}
		else
		{
			m_ImageNoBanker.DrawImage(pDC, nWidth / 2 - m_ImageNoBanker.GetWidth() / 2, nHeight / 2 - m_ImageNoBanker.GetHeight() / 2);
		}
	}

	//��ʾ������Ϣ
	if ( m_pImageFlex )
	{
		m_pImageFlex->DrawImage(pDC, m_ptFlexMove.x - m_pImageFlex->GetWidth()/2, m_ptFlexMove.y);

		if ( m_cbGameStatus == GAME_SCENE_END && m_enFlexMode == enFlexDealCrad )
		{
			//��ʾ���
			DrawWinType(pDC, nWidth, nHeight, m_ptFlexMove.x - m_pImageFlex->GetWidth()/2, m_ptFlexMove.y);
		}

		if ( m_cbGameStatus == GAME_SCENE_END && m_enFlexMode == enFlexGameEnd )
		{
			//��ʾ���
			DrawGameOver(pDC, nWidth, nHeight, m_ptFlexMove.x - m_pImageFlex->GetWidth()/2, m_ptFlexMove.y);
		}
	}

	// �滭������Ϣ
	DrawTopInfo(pDC, nWidth, nHeight);

	// �滭�ײ���Ϣ
	DrawBottomInfo(pDC, nWidth, nHeight);

	//�滭���
	DrawGameFrame(pDC, nWidth, nHeight);

	CString str;
	str.Format(TEXT("%d"), GetTickCount() - nTime);

	//����
	CRect rcCaption(5,7,1024,30);
	m_FontValleysListTwo.DrawText( pDC, m_strCaption, rcCaption, RGB(255,255,255), DT_VCENTER|DT_LEFT);
	//AfxMessageBox(str);
	return;
}

//�滭���
void CGameClientView::DrawGameFrame(CDC *pDC, int nWidth, int nHeight)
{
	//�ϲ���
	m_ImageGameFrame[0].DrawImage(pDC,0,0);
	for (int i=m_ImageGameFrame[0].GetWidth();i<nWidth-m_ImageGameFrame[2].GetWidth();i+=m_ImageGameFrame[1].GetWidth())
	{
		m_ImageGameFrame[1].DrawImage(pDC,i,0);
	}
	m_ImageGameFrame[2].DrawImage(pDC,nWidth-m_ImageGameFrame[2].GetWidth(),0);

	//�м����߲���
	for (int iH=m_ImageGameFrame[0].GetHeight();iH<nHeight-m_ImageGameFrame[3].GetHeight();iH+=m_ImageGameFrame[3].GetHeight())
	{
		m_ImageGameFrame[3].DrawImage(pDC,0,iH);
		m_ImageGameFrame[4].DrawImage(pDC,nWidth-m_ImageGameFrame[4].GetWidth(),iH);
	}
	
	//�²���
	m_ImageGameFrame[5].DrawImage(pDC,0,nHeight-m_ImageGameFrame[5].GetHeight());
	for (int i=m_ImageGameFrame[5].GetWidth();i<nWidth-m_ImageGameFrame[6].GetWidth();i+=m_ImageGameFrame[6].GetWidth())
	{
		m_ImageGameFrame[6].DrawImage(pDC,i,nHeight-m_ImageGameFrame[6].GetHeight());
	}	
	m_ImageGameFrame[7].DrawImage(pDC,nWidth-m_ImageGameFrame[7].GetWidth(),nHeight-m_ImageGameFrame[7].GetHeight());
}

// �滭������Ϣ
void CGameClientView::DrawTopInfo(CDC *pDC, int nWidth, int nHeight)
{
	// �������
	CPoint ptBegin(0,0);

	//--------------------------------------------------------------------
	// ʱ����Ϣ
	ptBegin.SetPoint( 210, 86 );
	if ( m_wMeChairID != INVALID_CHAIR )
	{
		WORD wUserTimer = GetUserClock(m_wMeChairID);
		if ( wUserTimer != 0 ) 
		{
			DrawTime(pDC, wUserTimer, ptBegin.x, ptBegin.y );

			int nTimeFlagWidth = m_ImageTimeType.GetWidth()/4;
			int nFlagIndex=0;
			if (m_cbGameStatus == GAME_SCENE_FREE) 
				nFlagIndex = 0;
			else if (m_cbGameStatus == GAME_SCENE_BET)
				nFlagIndex = 1;
			else if (m_cbGameStatus == GAME_SCENE_END) 
				nFlagIndex = 2;

			ptBegin.SetPoint( 285, 120 );
			m_ImageTimeType.DrawImage(pDC, ptBegin.x, ptBegin.y - m_ImageTimeType.GetHeight()/2, nTimeFlagWidth, m_ImageTimeType.GetHeight(),
				nFlagIndex * nTimeFlagWidth, 0);

		}
	}
	//----------------------------------------------------------------
	// ���·���Ϣ
	ptBegin.SetPoint( nWidth/2, 46 );
	//m_ImageScoreInfo.DrawImage( pDC, ptBegin.x, ptBegin.y );

	//�����ע
	if (m_wBankerUser != INVALID_CHAIR || m_bEnableSysBanker)
	{ 
		pDC->SetTextAlign(TA_LEFT);
		
		DrawTextString( pDC, AddComma(GetMaxPlayerScore(AREA_ZHUANG)), RGB(78,220,224), RGB(39,27,1), ptBegin.x - 205, ptBegin.y);

		DrawTextString( pDC, AddComma(GetMaxPlayerScore(AREA_XIAN)), RGB(112,223,118), RGB(39,27,1), ptBegin.x - 18, ptBegin.y);

		DrawTextString( pDC, AddComma(GetMaxPlayerScore(AREA_PING)), RGB(253,111,109), RGB(39,27,1), ptBegin.x + 170, ptBegin.y);
	}

	// ���ϵ���ʷ��¼
	/*int nDrawCount = 0;
	int nIndex = m_nScoreHead;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ) && nDrawCount < 12 )
	{
		tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
		pDC->SetTextAlign(TA_CENTER);

		DrawTextString( pDC, AddComma(ClientGameRecord.cbBankerCount), RGB(205,173,80), RGB(39,27,1), ptBegin.x + 39 + nDrawCount * 15, ptBegin.y + 83);

		DrawTextString( pDC, AddComma(ClientGameRecord.cbPlayerCount), RGB(205,173,80), RGB(39,27,1), ptBegin.x + 39 + nDrawCount * 15, ptBegin.y + 103);

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
		nDrawCount++;
	}*/

	//----------------------------------------------------------------
	// ׯ����Ϣ
	ptBegin.SetPoint( 55,57 );
	//m_ImageBankerInfo.DrawImage( pDC, ptBegin.x, ptBegin.y );

	// ��ʾ��Ϣ
	LONGLONG lBankerScore = 0;
	CString strBankName = TEXT("������ׯ");
	if ( m_wBankerUser != INVALID_CHAIR && GetClientUserItem(m_wBankerUser) )
	{
		IClientUserItem* pClientUserItem = GetClientUserItem(m_wBankerUser);
		lBankerScore = pClientUserItem->GetUserScore();
		strBankName = pClientUserItem->GetNickName();

		// ͷ����ʾ
		//DrawUserAvatar(pDC, ptBegin.x + 18, ptBegin.y + 68, pClientUserItem);
	}
	else if( m_bEnableSysBanker )
	{
		strBankName = TEXT("ϵͳ��ׯ");
		lBankerScore = m_lBankerScore;

		//m_ImageUserFace.DrawImage(pDC, ptBegin.x + 18, ptBegin.y + 68, FACE_CX, FACE_CY, 0, 0, FACE_CX, FACE_CY);
	}

	// ׯ������
	CRect rcBanker;
	rcBanker.SetRect( ptBegin.x , ptBegin.y , ptBegin.x + 120, ptBegin.y + 12);
	DrawTextString( pDC, strBankName, RGB(255,232,164), RGB(0,31,18), rcBanker, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	
	// ׯ���ܷ�
	rcBanker.OffsetRect(0, 21);
	DrawTextString( pDC, AddComma(lBankerScore), RGB(255,223,44), RGB(0,31,18), rcBanker, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	
	// ׯ�ҳɼ�
	rcBanker.OffsetRect(0, 21);
	DrawTextString( pDC, AddComma(m_lBankerWinScore), RGB(254,110,110), RGB(0,31,18), rcBanker, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	
	// ׯ�Ҿ���
	rcBanker.OffsetRect(0, 21);
	DrawTextString( pDC, AddComma(m_wBankerTime), RGB(106,192,217), RGB(0,31,18), rcBanker, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
   
	//----------------------------------------------------------------
	// ʱ����ʾ
	/*int nTimeFlagWidth = m_ImageTimeFlag.GetWidth()/3;
	int nFlagIndex=0;
	if (m_cbGameStatus == GAME_SCENE_FREE) 
		nFlagIndex = 0;
	else if (m_cbGameStatus == GAME_SCENE_BET)
		nFlagIndex = 1;
	else if (m_cbGameStatus == GAME_SCENE_END) 
		nFlagIndex = 2;

	m_ImageTimeFlag.DrawImage(pDC, nWidth/2 - m_ImageTimeFlag.GetWidth()/6, 103, nTimeFlagWidth, m_ImageTimeFlag.GetHeight(),
		nFlagIndex * nTimeFlagWidth, 0);*/

	//--------------------------------------------------------------------
	// ������Ϣ
	ptBegin.SetPoint(  nWidth-142,57 );
	if (m_wMeChairID != INVALID_CHAIR)
	{
		//��ȡ���
		IClientUserItem* pMeUserItem = GetClientUserItem(m_wMeChairID);
		if ( pMeUserItem != NULL )
		{
			// ����
			CRect rect( ptBegin.x , ptBegin.y , ptBegin.x + 120, ptBegin.y + 12);
			DrawTextString(pDC, &m_FontValleysListTwo, pMeUserItem->GetNickName(), RGB(205,174,83), RGB(34,26,3), rect, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			// ����
			rect.OffsetRect(0, 21);
			LONGLONG lMeBet = 0;
			for( int i = 0; i < AREA_MAX; ++i )
				lMeBet += m_lPlayBet[i];

			DrawTextString(pDC, &m_FontValleysListTwo, AddComma( pMeUserItem->GetUserScore() - lMeBet ), RGB(255,225,45), RGB(34,26,3), rect, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			// �ɼ�
			rect.OffsetRect(0, 21);
			DrawTextString(pDC, &m_FontValleysListTwo, AddComma(m_lMeStatisticScore), RGB(83,204,101), RGB(34,26,3), rect, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			// ������
			CString strHit;
			FLOAT fHit = 0.f;
			rect.OffsetRect(0, 21);
			if ( m_nWinCount + m_nLoseCount > 0 )
				fHit = (FLOAT)m_nWinCount / ((FLOAT)m_nWinCount + (FLOAT)m_nLoseCount);

			if ( fHit < 0.001f )
				strHit = TEXT("��ս��");
			else
				strHit.Format(TEXT("%.2f%%"), fHit*100);

			DrawTextString(pDC, &m_FontValleysListTwo, strHit, RGB(63,156,200), RGB(34,26,3), rect, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		}
	}


	//----------------------------------------------------------------
	// ������ׯ�б�
	ptBegin.SetPoint( 15, 175 );
	m_ImageWaitValleys.DrawImage( pDC, ptBegin.x, ptBegin.y );

	int   nCount = 0;
	CRect rect(0, 0, 0, 0);
	//CFont* pOldFont = pDC->GetCurrentFont();
	rect.SetRect(ptBegin.x , ptBegin.y + 20, ptBegin.x + 165, ptBegin.y + 32);	

	for( int i = m_nShowValleyIndex; i < m_ValleysList.GetCount(); ++i )
	{
		IClientUserItem* pIClientUserItem = GetClientUserItem( m_ValleysList[i] );
		if ( pIClientUserItem == NULL  )
			continue;		

		//// ��������
		//pDC->SelectObject(&m_FontValleysListTwo);

		// ͷ��
		//DrawUserAvatar(pDC, rect.left, rect.top + rect.Height()/2 - 18/2, 18, 18, pIClientUserItem );

		// ����
		CRect rectName(rect);
		rectName.left = ptBegin.x;
		rectName.right = ptBegin.x + 75;
		DrawTextString( pDC, pIClientUserItem->GetNickName(), RGB(255,249,102) , RGB(0,0,0), rectName, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

		// ����
		CRect rectScore(rect);
		rectScore.left = ptBegin.x + 85;
		DrawTextString( pDC,AddComma(pIClientUserItem->GetUserScore() ), RGB(255,249,102) , RGB(0,0,0), rectScore, DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER|DT_SINGLELINE);

		// λ�õ���
		rect.OffsetRect(0, 18);

		// λ������
		nCount++;

		if ( nCount == 4 )
		{
			break;
		}
	}
	//pDC->SelectObject(pOldFont);
}

// �滭�ײ���Ϣ
void CGameClientView::DrawBottomInfo(CDC *pDC, int nWidth, int nHeight)
{
	// �滭λ��
	int nDrawWidth = m_ImagePlayLeft.GetWidth() + m_ImagePlayMiddle.GetWidth()*3 + m_ImagePlayRight.GetWidth();
	CRect rectDraw( nWidth/2 , nHeight - 117, nWidth/2 + nDrawWidth/2, nHeight);

	// �ײ�����
	CPoint ptBegin( rectDraw.left, rectDraw.top );
	m_ImageUserBack.DrawImage(pDC,5,nHeight-m_ImageUserBack.GetHeight()-5);
	m_ImageChatBack.DrawImage(pDC,nWidth-m_ImageChatBack.GetWidth()-5,nHeight-m_ImageChatBack.GetHeight()-5);
	
	//�滭·��
	int nPosX = (IsZoomedMode() ? 304 + CalFrameGap() : 304);
	int nPosY = nHeight - 122;
	int nGridWidth = 19;
	int nGridHeight = 18;
	int nVerCount = 0, nHorCount = 0;
	BYTE cbPreWinSide = 0, cbCurWinSide = 0;
	int nRightIndex = m_nRecordFirst;
	ZeroMemory(m_bFillTrace,sizeof(m_bFillTrace));
	int horRecordCount = IsZoomedMode()? ZOOMED_RECORD_COL_COUNT : NORMAL_RECORD_COL_COUNT;
		
	m_RecordResult.RemoveAll();
	m_RecordResult.SetSize(MAX_RECORD_COL_COUNT);
	/*for(int i = 0;i < m_RecordResult.GetCount();i++)
	{
		for(int j = 0;j < 6;j++)
		{
			m_RecordResult[i].resultIndex[j] = INVALID_DWORD;
		}
	}*/
	
	int nDrawCount = (m_nRecordLast-m_nRecordFirst +MAX_RECORD_SUM)%MAX_RECORD_SUM;
	nRightIndex = (m_nRecordLast-__min(MAX_RECORD_SUM,nDrawCount) +MAX_RECORD_SUM)%MAX_RECORD_SUM;

	while (nRightIndex != m_nRecordLast && ( m_nRecordLast != m_nRecordFirst ) )
	{
		tagClientGameRecord &ClientGameRecord = m_RightGameRecordArray[nRightIndex];
		if (ClientGameRecord.cbPlayerCount > ClientGameRecord.cbBankerCount) cbCurWinSide = AREA_XIAN + 1;
		else if (ClientGameRecord.cbPlayerCount < ClientGameRecord.cbBankerCount) cbCurWinSide = AREA_ZHUANG + 1;
		else cbCurWinSide = AREA_PING + 1;

		//������Ŀ
		if ( cbPreWinSide != 0 && ( cbPreWinSide == cbCurWinSide || cbCurWinSide == AREA_PING + 1 ))
		{
			nVerCount++;
			if (m_bFillTrace[nVerCount][nHorCount]==true || nVerCount==6)
			{
				nVerCount--;
				nHorCount++;				
			}
		}
		else
		{
			nVerCount=0;

			//��һ��
			if ( cbPreWinSide != 0 )
			{
				for (int i=0; i < MAX_RECORD_SUM; ++i)
				{
					if (m_bFillTrace[0][i]==false)
					{
						nHorCount=i;
						break;
					}
				}				
			}
			cbPreWinSide=cbCurWinSide;
		}
		if(nVerCount > 5)
		{
			ASSERT(FALSE);
		}
		//���ñ�ʶ
		m_bFillTrace[nVerCount][nHorCount]=true;	
		
		//�����ж�  ��� ˮƽ80 ��·����
		//if (nHorCount == MAX_RECORD_COL_COUNT)
		//{
		//	m_RightGameRecordCount=0;
		//	ZeroMemory(m_bFillTrace,sizeof(m_bFillTrace));
		//	for(int i = 0;i < m_RecordResult.GetCount();i++)
		//	{
		//		for(int j = 0;j < 6;j++)
		//		{
		//			m_RecordResult[i].resultIndex[j] = INVALID_DWORD;
		//		}
		//	}
		//	// ���ΰ�ť
		//	m_btScoreMoveL.EnableWindow(FALSE);
		//	m_btScoreMoveR.EnableWindow(FALSE);
		//	// ���ñ���
		//	m_nRecordFirst = 0;
		//	m_nRecordHead = 0;
		//	m_nRecordLast = 0;
		//	InvalidateRect(NULL);
		//	break;
		//}

		//����

		INT nMaxCol = IsZoomedMode()?ZOOMED_RECORD_COL_COUNT:NORMAL_RECORD_COL_COUNT;
		if(nHorCount - m_nHeadCol < 0) 
		{
			nRightIndex = (nRightIndex+1) % MAX_SCORE_HISTORY;
			continue;
		}
		if(nHorCount - m_nHeadCol >= nMaxCol)
		{
			nRightIndex = (nRightIndex+1) % MAX_SCORE_HISTORY;
			continue;
		}
		
		if ( cbCurWinSide == AREA_ZHUANG + 1 )
		{
			m_PngBankerEXFlag.DrawImage(pDC, nPosX + (nHorCount - m_nHeadCol) * nGridWidth, nPosY + nVerCount * nGridHeight + 1);
			if (ClientGameRecord.bBankerTwoPair)
				m_PngBankerEXFlagTowPair.DrawImage(pDC, nPosX + (nHorCount - m_nHeadCol) * nGridWidth, nPosY + nVerCount * nGridHeight + 1);
			else
				m_PngBankerEXFlag.DrawImage(pDC, nPosX + (nHorCount - m_nHeadCol) * nGridWidth, nPosY + nVerCount * nGridHeight + 1);
		}
		else if ( cbCurWinSide == AREA_XIAN + 1 )
		{
			m_PngPlayerEXFlag.DrawImage(pDC, nPosX + (nHorCount - m_nHeadCol) * nGridWidth, nPosY + nVerCount * nGridHeight);
			if (ClientGameRecord.bPlayerTwoPair)
				m_PngPlayerEXFlagTowPair.DrawImage(pDC, nPosX + (nHorCount - m_nHeadCol) * nGridWidth, nPosY + nVerCount * nGridHeight);
			else
				m_PngPlayerEXFlag.DrawImage(pDC, nPosX + (nHorCount - m_nHeadCol) * nGridWidth, nPosY + nVerCount * nGridHeight);
		}
		else
		{
			m_PngTieEXFlag.DrawImage(pDC, nPosX + (nHorCount - m_nHeadCol) * nGridWidth, nPosY + nVerCount * nGridHeight);
		}

		nRightIndex = (nRightIndex+1) % MAX_SCORE_HISTORY;
	}	
	
	

	int nIndex = m_nRecordHead;
	//if( !IsZoomedMode())
	//{
	//	int beyondHorCount = CalLastRecordHorCount() - NORMAL_RECORD_COL_COUNT;
	//	nIndex = ( beyondHorCount > 0 ? beyondHorCount + m_nRecordHead : m_nRecordHead );
	//}
	//else
	//{
	//	int beyondHorCount = CalLastRecordHorCount() - ZOOMED_RECORD_COL_COUNT;
	//	nIndex = ( beyondHorCount > 0 ? beyondHorCount + m_nRecordHead : m_nRecordHead );
	//}

	//int lastRecordHorCount = CalLastRecordHorCount();
	//int drawCount = ( IsZoomedMode() ? ZOOMED_RECORD_COL_COUNT : NORMAL_RECORD_COL_COUNT);
	//for(int i = 0;i < drawCount;i++)
	//{
	//	for(int j = 0;j < 6;j++)
	//	{
	//		if( m_RecordResult[nIndex].resultIndex[j] == INVALID_DWORD)
	//			continue;
	//		
	//		switch (m_RecordResult[nIndex].resultIndex[j])
	//		{
	//		case AREA_ZHUANG:
	//			{
	//				m_PngBankerEXFlag.DrawImage(pDC, nPosX + i * nGridWidth, nPosY + j * nGridHeight);
	//			}
	//			break;
	//		case AREA_TIANGANG:
	//			{
	//				m_PngBankerEXFlagTowPair.DrawImage(pDC, nPosX + i * nGridWidth, nPosY + j * nGridHeight);
	//			}
	//			break;
	//		case AREA_XIAN:
	//			{	
	//				m_PngPlayerEXFlag.DrawImage(pDC, nPosX + i * nGridWidth, nPosY + j * nGridHeight);
	//			}
	//			break;
	//		case AREA_XIAN_DUI:
	//			{
	//				m_PngPlayerEXFlagTowPair.DrawImage(pDC, nPosX + i * nGridWidth, nPosY + j * nGridHeight);
	//			}
	//			break;
	//		case AREA_PING:
	//			{
	//				m_PngTieEXFlag.DrawImage(pDC, nPosX + i * nGridWidth, nPosY + j * nGridHeight);
	//			}
	//			break;
	//		}
	//	}
	//	nIndex = (nIndex + 1) % MAX_RECORD_COL_COUNT;
	//}
	//--------------------------------------------------------------------
	// ���¼��·��

	/*nIndex = m_nRecordHead;
	int nDrawCount = 0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ) && nDrawCount < 12 )
	{

		int nYPos = 0;
		if ( m_GameRecordArrary[nIndex].cbBankerCount > m_GameRecordArrary[nIndex].cbPlayerCount ) nYPos = rectDraw.bottom - 80;
		else if (m_GameRecordArrary[nIndex].cbBankerCount < m_GameRecordArrary[nIndex].cbPlayerCount ) nYPos = rectDraw.bottom - 53;
		else nYPos = rectDraw.bottom - 26;
		int nXPos = rectDraw.right - 330 + ((nIndex - m_nRecordHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY) * 23;

		int nFlagsIndex = 2;
		if ( m_GameRecordArrary[nIndex].enOperateFlags == enOperateResult_NULL ) nFlagsIndex = 2;
		else if ( m_GameRecordArrary[nIndex].enOperateFlags == enOperateResult_Win) nFlagsIndex = 0;
		else if ( m_GameRecordArrary[nIndex].enOperateFlags == enOperateResult_Lost) nFlagsIndex = 1;

		m_ImageWinFlags.DrawImage( pDC, nXPos - m_ImageWinFlags.GetWidth()/6, nYPos - m_ImageWinFlags.GetHeight()/2, m_ImageWinFlags.GetWidth()/3 , 
			m_ImageWinFlags.GetHeight(),m_ImageWinFlags.GetWidth()/3 * nFlagsIndex, 0 );

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
		nDrawCount++;
	}*/

	//ͳ�Ƹ���
	nIndex = m_nRecordFirst;
	int nPlayerCount = 0, nBankerCount = 0, nTieCount = 0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast != m_nRecordFirst ) )
	{
		if ( m_GameRecordArrary[nIndex].cbBankerCount < m_GameRecordArrary[nIndex].cbPlayerCount ) nPlayerCount++;
		else if ( m_GameRecordArrary[nIndex].cbBankerCount == m_GameRecordArrary[nIndex].cbPlayerCount ) nTieCount++;
		else nBankerCount++;

		nIndex = (nIndex+1) % MAX_RECORD_SUM;
	}

	DrawNumber(pDC,&m_ImgaeGameTimeCount,TEXT("0123456789"),nBankerCount,IsZoomedMode() ? 280+CalFrameGap() : 280,nHeight-111,DT_CENTER);
	DrawNumber(pDC,&m_ImgaeGameTimeCount,TEXT("0123456789"),nPlayerCount,IsZoomedMode() ? 280+CalFrameGap() : 280,nHeight-75,DT_CENTER);
	DrawNumber(pDC,&m_ImgaeGameTimeCount,TEXT("0123456789"),nTieCount,IsZoomedMode() ? 280+CalFrameGap() : 280,nHeight-39,DT_CENTER);
	/*CRect rect(rectDraw.right - 61, rectDraw.bottom - 92, rectDraw.right - 38, rectDraw.bottom - 67);
	DrawTextString( pDC, AddComma(nBankerCount), RGB(205,173,80), RGB(39,27,1), rect, DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	
	rect.OffsetRect(0, 26);
	rect.bottom += 1;
	DrawTextString( pDC, AddComma(nPlayerCount), RGB(205,173,80), RGB(39,27,1), rect, DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	
	rect.OffsetRect(0, 28);
	DrawTextString( pDC, AddComma(nTieCount), RGB(205,173,80), RGB(39,27,1), rect, DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER|DT_SINGLELINE);*/

}

//��ʾ��Ӯ
void CGameClientView::DrawWinType(CDC *pDC, int nWidth, int nHeight, int nBeginX, int nBeginY )
{
	// ��ʾ��Ӯ
	int nTypeWidth = m_ImageWinType.GetWidth()/5;
	int nTypeHeight = m_ImageWinType.GetHeight();


	//ƽ
	if( m_nWinShowIndex[AREA_PING] > 0 )
	{
		BYTE cbAlpha = (BYTE)((DOUBLE)m_nWinShowIndex[AREA_PING] / 6.0 * 255.0);
		m_ImageWinType.AlphaDrawImage(pDC, nBeginX + 390/2 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, 0, 0, nTypeWidth, nTypeHeight, cbAlpha);
		m_ImageWinType.AlphaDrawImage(pDC, nBeginX + 390/4*3 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 3, 0, nTypeWidth, nTypeHeight, 255 - cbAlpha );
		m_ImageWinType.AlphaDrawImage(pDC, nBeginX + 390/4 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 1, 0, nTypeWidth, nTypeHeight, 255 - cbAlpha );
	}
	else
	{

		// ׯ
		if( m_nWinShowIndex[AREA_ZHUANG] == 0 )
		{
			m_ImageWinType.DrawImage(pDC, nBeginX + 390/4*3 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 3, 0, nTypeWidth, nTypeHeight );
		}
		else if( m_nWinShowIndex[AREA_ZHUANG] < 6 )
		{
			int nZhuangWidth = m_ImageWinZhuang.GetWidth()/6;
			int nZhuangHeight = m_ImageWinZhuang.GetHeight();
			m_ImageWinZhuang.DrawImage(pDC, nBeginX + 390/4*3 - nZhuangWidth/2, nBeginY + 70, nZhuangWidth, nZhuangHeight, nZhuangWidth * m_nWinShowIndex[AREA_ZHUANG], 0, nZhuangWidth, nZhuangHeight );
		}
		else
		{
			m_ImageWinType.DrawImage(pDC, nBeginX + 390/4*3 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 4, 0, nTypeWidth, nTypeHeight );;
		}

		// ��
		if( m_nWinShowIndex[AREA_XIAN] == 0 )
		{
			m_ImageWinType.DrawImage(pDC, nBeginX + 390/4 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 1, 0, nTypeWidth, nTypeHeight );
		}
		else if( m_nWinShowIndex[AREA_XIAN] < 6 )
		{
			int nXianWidth = m_ImageWinXian.GetWidth()/6;
			int nXianHeight = m_ImageWinXian.GetHeight();
			m_ImageWinXian.DrawImage(pDC, nBeginX + 390/4 - nXianWidth/2, nBeginY + 70, nXianWidth, nXianHeight, nXianWidth * m_nWinShowIndex[AREA_XIAN], 0, nXianWidth, nXianHeight );
		}
		else
		{
			m_ImageWinType.DrawImage(pDC, nBeginX + 390/4 - nTypeWidth/2, nBeginY + 70, nTypeWidth, nTypeHeight, nTypeWidth * 2, 0, nTypeWidth, nTypeHeight );
		}
	}

	//�������
	BYTE cbCardData[2][5] = {0};
	WORD cbCardCount[2] = {m_CardControl[INDEX_PLAYER].GetCardCount(), m_CardControl[INDEX_BANKER].GetCardCount() };
	int  nPointWidth = m_ImageGamePoint.GetWidth() / 10;

	// �мҵ���
	if ( cbCardCount[INDEX_PLAYER] > 0 )
	{
		// �������
		m_CardControl[INDEX_PLAYER].GetCardData(cbCardData[INDEX_PLAYER], 5);
		BYTE cbPlayerPoint = m_GameLogic.GetCardListPip(cbCardData[INDEX_PLAYER], (BYTE)cbCardCount[INDEX_PLAYER]);

		//�滭����
		m_ImageGamePoint.DrawImage(pDC, nWidth/2 - 100 - nPointWidth/2, 170, nPointWidth, m_ImageGamePoint.GetHeight(), cbPlayerPoint * nPointWidth, 0);
	}

	// ׯ�ҵ���
	if ( cbCardCount[INDEX_BANKER] > 0 )
	{
		// �������
		m_CardControl[INDEX_BANKER].GetCardData(cbCardData[INDEX_BANKER], 5);
		BYTE cbBankerPoint = m_GameLogic.GetCardListPip(cbCardData[INDEX_BANKER], (BYTE)cbCardCount[INDEX_BANKER]);

		//�滭����
		m_ImageGamePoint.DrawImage(pDC, nWidth/2 + 100 - nPointWidth/2 , 170, nPointWidth, m_ImageGamePoint.GetHeight(), cbBankerPoint * nPointWidth, 0);
	}

	//�滭�˿�
	m_CardControl[INDEX_PLAYER].DrawCardControl(pDC);
	m_CardControl[INDEX_BANKER].DrawCardControl(pDC);

	//�����˿�
	m_DispatchCard.DrawCardControl(pDC);	


	// ������Ϣ
	if( m_CardControl[INDEX_PLAYER].GetCardCount() || m_CardControl[INDEX_BANKER].GetCardCount() )
	{
		// ��������
		UINT   pOldAlign = pDC->SetTextAlign(TA_CENTER);

		// ������ʾ
		int nBegin = 0;
		int nStrCount = 0;
		TCHAR szString[512] = {0};
		_sntprintf(szString, sizeof(szString), TEXT("%s"), m_strDispatchCardTips);
		for(int i = 0; i < lstrlen(szString); ++i  )
		{
			if ( szString[i] == '\n' || i == lstrlen(szString) - 1 )
			{	
				TCHAR szTemp[128];
				memcpy(szTemp, &szString[nBegin], (i - nBegin + 1) * sizeof(TCHAR));

				if ( i == lstrlen(szString) - 1 )
					szTemp[(i - nBegin) + 1] = '\0';
				else
					szTemp[(i - nBegin)] = '\0';

				nBegin = i;
				DrawTextString( pDC, &m_FontValleysListTwo, szTemp, RGB(204,174,84) , RGB(38,26,2),  nWidth/2, 443 + nStrCount * 18 );
				nStrCount++;
			}
		}

		// ��ԭ����
		pDC->SetTextAlign(pOldAlign);
	}
}

//��ʾ���
void CGameClientView::DrawGameOver(CDC *pDC, int nWidth, int nHeight, int nBeginX, int nBeginY )
{
	// ����	
	int  nPointWidth = m_ImageGameEndPoint.GetWidth() / 10;
	BYTE cbPlayerPoint = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER], m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerPoint = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER], m_cbCardCount[INDEX_BANKER]);

	//�滭����
	m_ImageGameEndPoint.DrawImage(pDC, nBeginX + 80 , nBeginY + 63, nPointWidth, m_ImageGameEndPoint.GetHeight(), cbPlayerPoint * nPointWidth, 0);
	m_ImageGameEndPoint.DrawImage(pDC, nBeginX + 321 , nBeginY + 63, nPointWidth, m_ImageGameEndPoint.GetHeight(), cbBankerPoint * nPointWidth, 0);

	// ����
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_XIAN_DUI], true), nBeginX + 87 , nBeginY + 125, DT_CENTER);
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_TIANGANG], true), nBeginX + 334 , nBeginY + 125, DT_CENTER);

	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_XIAN], true), nBeginX + 87 , nBeginY + 174, DT_CENTER);
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_PING], true), nBeginX + 208 , nBeginY + 174, DT_CENTER);
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_ZHUANG], true), nBeginX + 334 , nBeginY + 174, DT_CENTER);

	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_XIAN_TIAN], true), nBeginX + 87 , nBeginY + 223, DT_CENTER);
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_TONG_DUI], true), nBeginX + 208 , nBeginY + 223, DT_CENTER);
	DrawNumber(pDC, &m_ImageGameEndMyScore, TEXT("+-,0123456789"), AddComma(m_lPlayScore[AREA_ZHUANG_TIAN], true), nBeginX + 334 , nBeginY + 223, DT_CENTER);

	LONGLONG lPlayAllScore = 0l;
	for ( int i = 0; i < AREA_MAX; ++i )
	{
		lPlayAllScore += m_lPlayScore[i];
	}
	DrawNumber(pDC, &m_ImageGameEndAllScore, TEXT("+-,0123456789"), AddComma(m_lPlayAllScore, true), nBeginX + 97 , nBeginY + 259, DT_LEFT);
	//DrawNumber(pDC, &m_ImageGameEndPoint, TEXT("0123456789"), AddComma(m_lRevenue, true), nBeginX + 97 , nBeginY + 293, DT_LEFT);
}

//���ó���
void CGameClientView::SetCurrentBet(LONGLONG lCurrentBet)
{
	//���ñ���
	ASSERT(lCurrentBet>=0L);
	m_lCurrentBet = lCurrentBet;

	if (m_lCurrentBet == 0L && m_cbGameStatus != GAME_SCENE_END )
	{
		for ( int i = 0 ; i < m_ArrayCurrentFlash.GetCount(); ++i )
		{
			m_ArrayCurrentFlash[i].bFlashAreaAlpha = false;
		}
		InvalidGameView(0,0,0,0);
	}
	return;
}

//��ʷ��¼
void CGameClientView::SetGameHistory(enOperateResult OperateResult, BYTE cbPlayerCount, BYTE cbBankerCount, BYTE cbKingWinner, bool bPlayerTwoPair, bool bBankerTwoPair)
{
	//��������
	tagClientGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.enOperateFlags = OperateResult;
	GameRecord.cbPlayerCount = cbPlayerCount;
	GameRecord.cbBankerCount = cbBankerCount;
	GameRecord.bPlayerTwoPair=bPlayerTwoPair;
	GameRecord.bBankerTwoPair=bBankerTwoPair;
	GameRecord.cbKingWinner=cbKingWinner;
	SetBottomRecord(GameRecord);

	//�ƶ��±�      
	//m_nRecordLast = (m_nRecordLast+1) % MAX_RECORD_SUM;
	//�ƶ��±�
	m_nRecordLast = (m_nRecordLast+1) % MAX_RECORD_SUM;
	if ( m_nRecordLast == m_nRecordFirst )
	{
		m_nRecordFirst = (m_nRecordFirst+1) % MAX_RECORD_SUM;
		if ( m_nScoreHead < m_nRecordFirst ) m_nScoreHead = m_nRecordFirst;
	}

	INT nMaxCol = IsZoomedMode()?ZOOMED_RECORD_COL_COUNT:NORMAL_RECORD_COL_COUNT;
	if(m_nTotalCol > nMaxCol) m_btScoreMoveR.EnableWindow(TRUE);
	
	/*if(!IsZoomedMode() && CalLastRecordHorCount() > NORMAL_RECORD_COL_COUNT)
	{
		m_btScoreMoveL.EnableWindow(TRUE);
	}
	if(IsZoomedMode() && CalLastRecordHorCount() > ZOOMED_RECORD_COL_COUNT)
	{
		m_btScoreMoveL.EnableWindow(TRUE);
	}
	if(!IsZoomedMode() && CalLastRecordHorCount() > NORMAL_RECORD_COL_COUNT)
	{
		m_btScoreMoveL.EnableWindow(TRUE);
		m_btScoreMoveR.EnableWindow(FALSE);
		m_nRecordHead = (CalLastRecordHorCount() - NORMAL_RECORD_COL_COUNT+MAX_RECORD_COL_COUNT) % MAX_RECORD_COL_COUNT;
	}
	if( IsZoomedMode() && CalLastRecordHorCount() > ZOOMED_RECORD_COL_COUNT)
	{
		m_btScoreMoveL.EnableWindow(TRUE);
		m_btScoreMoveR.EnableWindow(FALSE);
		m_nRecordHead = (CalLastRecordHorCount() - ZOOMED_RECORD_COL_COUNT+MAX_RECORD_COL_COUNT) % MAX_RECORD_COL_COUNT;
	}
	if( IsZoomedMode() && CalLastRecordHorCount() <= ZOOMED_RECORD_COL_COUNT)
	{
		m_nRecordHead = 0;
		m_btScoreMoveR.EnableWindow(FALSE);
		m_btScoreMoveL.EnableWindow(FALSE);
	}*/
	

	// ����
	//if ( m_nRecordLast == m_nRecordFirst )
	//{
	//	m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
	//	if ( m_nRecordHead < m_nRecordFirst ) m_nRecordHead = m_nRecordFirst;
	//}
		
	//if(!IsZoomedMode() && CalLastRecordHorCount() ==  NORMOL_RECORD_HORCOUNT )
	//	m_btScoreMoveR.EnableWindow(TRUE);
	//if(IsZoomedMode() && CalLastRecordHorCount() == CalZoomedRecordCount() )
	//	m_btScoreMoveR.EnableWindow(TRUE);


	//�Ƶ����¼�¼
	//if ( (CalLastRecordHorCount() ==  NORMOL_RECORD_HORCOUNT && !IsZoomedMode())
	//   || (CalLastRecordHorCount() ==  CalZoomedRecordCount() && IsZoomedMode()))
	//{
	//	m_btScoreMoveL.EnableWindow(TRUE);
	//	m_btScoreMoveR.EnableWindow(FALSE);
	//}
	
	//int RecordSum = ( IsZoomedMode() ? ZOOMED_RECORD_SUM : NORMAL_RECORD_SUM );
	//if(m_RecordResult.GetCount() >= RecordSum)
	//{
	//	m_RecordResult.RemoveAt(0);
	//}

	//����·��
	if (m_DlgViewChart.m_hWnd!=NULL)
	{
		m_DlgViewChart.SetGameRecord(GameRecord);
	}

	return;
}

//��ʷ��¼
void CGameClientView::SetBottomRecord(const tagClientGameRecord &ClientGameRecord)
{
	//������

	m_RightGameRecordArray[m_RightGameRecordCount++]=ClientGameRecord;

	//����ж�
	if (m_RightGameRecordCount >= MAX_RECORD_SUM) m_RightGameRecordCount=0;

	//���½���
	InvalidGameView(0,0,0,0);
}

//��¼����
void CGameClientView::SetRecordCol()
{
	//��������		//����Щbug �����100�ѵ�ʱ����������
	BYTE nIndex,nVerCount,nHorCount,cbCurWinSide,cbPreWinSide;
	m_nTotalCol = 1;
	nIndex = m_nRecordFirst;
	nVerCount = 0, nHorCount = 0;
	cbCurWinSide = 0;
	cbPreWinSide = 0;
	ZeroMemory(m_bFillTrace,sizeof(m_bFillTrace));
	while (nIndex != m_nRecordLast && ( m_nRecordLast != m_nRecordFirst ) )
	{
		//break;
		tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];

		if (ClientGameRecord.cbPlayerCount > ClientGameRecord.cbBankerCount) cbCurWinSide = AREA_XIAN + 1;
		else if (ClientGameRecord.cbPlayerCount < ClientGameRecord.cbBankerCount) cbCurWinSide = AREA_ZHUANG + 1;
		else cbCurWinSide = AREA_PING + 1;

		//������Ŀ
		if ( cbPreWinSide != 0 && ( cbPreWinSide == cbCurWinSide || cbCurWinSide == AREA_PING + 1 ))
		{
			nVerCount++;
			if (m_bFillTrace[nVerCount][nHorCount]==true || nVerCount==6)
			{
				nVerCount--;
				nHorCount++;
			}
		}
		else
		{
			nVerCount=0;

			//��һ��
			if ( cbPreWinSide != 0 )
			{
				for (int i=0; i<MAX_RECORD_SUM; ++i)
				{
					if (m_bFillTrace[0][i]==false)
					{
						nHorCount=i;
						break;
					}
				}				
			}

			cbPreWinSide=cbCurWinSide;
		}

		//���ñ�ʶ
		m_bFillTrace[nVerCount][nHorCount]=true;

		nIndex = (nIndex+1) % MAX_RECORD_SUM;
	}
	m_nTotalCol = nHorCount;
	m_DlgViewChart.SetRecordCol(m_nTotalCol);

	INT nMaxCol = IsZoomedMode()?ZOOMED_RECORD_COL_COUNT:NORMAL_RECORD_COL_COUNT;
	if(m_nTotalCol > nMaxCol) 
	{
		m_nHeadCol = m_nTotalCol - (nMaxCol-1);
		m_btScoreMoveL.EnableWindow(TRUE);
		m_btScoreMoveR.EnableWindow(FALSE);
	}

	return;
}


//�������
void CGameClientView::CleanUserBet()
{
	//��������
	for (BYTE i = 0; i < CountArray(m_BetInfoArray); i++)
		m_BetInfoArray[i].RemoveAll();

	//��ע��Ϣ
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));
	ZeroMemory(m_lPlayBet, sizeof(m_lPlayBet));
	ZeroMemory(m_lPlayScore, sizeof(m_lPlayScore));
	//���������ע
	ZeroMemory(m_lAllPlayBet,sizeof(m_lAllPlayBet));
	m_lBankerCurGameScore = 0;

	//ɾ����ע��Ϣ
	m_strDispatchCardTips = TEXT("");

	//ɾ����ʱ��
	KillTimer(IDI_FLASH_WINNER);
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_DISPATCH_INTERVAL);
	KillTimer(IDI_FLASH_BET);
	KillTimer(IDI_WIN_TYPE);
	KillTimer(IDI_WIN_TYPE_DELAY);
	

	//�˿���Ϣ
	ZeroMemory(m_cbCardCount, sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
	m_DispatchCard.SetCardData(NULL, 0);
	m_CardControl[0].SetCardData(NULL, 0);
	m_CardControl[1].SetCardData(NULL, 0);
	m_cbJetArea = 0xFF;
	m_bFlashResult = false;

	if( NULL != m_pClientControlDlg && NULL != m_pClientControlDlg->GetSafeHwnd())
	{
		m_pClientControlDlg->m_UserBetArray.RemoveAll();
		m_pClientControlDlg->UpdateUserBet(true);
	}
	
	//���½���
	InvalidGameView(0,0,0,0);

	return;
}

//����״̬
VOID CGameClientView::SetGameStatus( BYTE cbGameStatus )
{
	m_cbGameStatus = cbGameStatus;

	if ( m_cbGameStatus == GAME_SCENE_BET )
	{
		FlexAnimation(enFlexBetTip, true);

		//SetTimer(IDI_FLASH_BET, 30, NULL);
		//������ע
		if(m_wBankerUser!=m_wMeChairID) m_bEnableBet = true;
	}
	else if ( m_cbGameStatus == GAME_SCENE_END )
	{
		FlexAnimation(enFlexBetTip, false);

		KillTimer(IDI_FLASH_BET);
	}
	else if ( m_cbGameStatus == GAME_SCENE_FREE )
	{
		//���ý���
		m_CardControl[INDEX_PLAYER].SetCardData(NULL, 0);
		m_CardControl[INDEX_BANKER].SetCardData(NULL, 0);
		FlexAnimation(m_enFlexMode, false);

		KillTimer(IDI_FLASH_BET);
	}
}

//�ҵ�λ��
void CGameClientView::SetMeChairID(WORD wMeChairID)
{
	m_wMeChairID = wMeChairID;
}

//���������ע
void CGameClientView::SetPlayBetScore(LONGLONG lPlayBetScore)
{
	m_lMeMaxScore = lPlayBetScore;
}

//��������
void CGameClientView::SetAreaLimitScore(LONGLONG lAreaLimitScore)
{
	m_lAreaLimitScore = lAreaLimitScore;
}

//ׯ����Ϣ
void CGameClientView::SetBankerInfo(WORD wBankerUser, LONGLONG lBankerScore, LONGLONG lBankerWinScore, WORD wBankerTime)
{
	m_wBankerUser = wBankerUser;
	m_lBankerScore = lBankerScore;
	m_lBankerWinScore = lBankerWinScore;
	m_wBankerTime = wBankerTime;
}

//ׯ����Ϣ
void CGameClientView::SetBankerInfo( WORD wBankerUser, LONGLONG lBankerScore )
{
	m_wBankerUser = wBankerUser;
	m_lBankerScore = lBankerScore;
}

//ׯ����Ϣ
void CGameClientView::SetBankerOverInfo( LONGLONG lBankerWinScore, WORD wBankerTime )
{
	m_lBankerWinTempScore = lBankerWinScore;
	m_wBankerTime = wBankerTime;
}

//����ϵͳ�Ƿ���ׯ
void CGameClientView::SetEnableSysBanker(bool bEnableSysBanker)
{
	m_bEnableSysBanker = bEnableSysBanker;
}

//������ע
void CGameClientView::SetPlayBet(BYTE cbViewIndex, LONGLONG lBetCount)
{
	m_lPlayBet[cbViewIndex] = lBetCount;
}

//ȫ����ע
void CGameClientView::SetAllBet(BYTE cbViewIndex, LONGLONG lBetCount)
{
	m_lAllBet[cbViewIndex] = lBetCount;
}

//��ӳ���
void CGameClientView::AddChip(BYTE cbViewIndex, LONGLONG lScoreCount)
{
	//Ч�����
	ASSERT(cbViewIndex < AREA_MAX);
	if (cbViewIndex >= AREA_MAX) return;

	//��������
	LONGLONG lScoreIndex[BET_COUNT]={1000L,10000L,100000L,1000000L,5000000L,10000000L};

	//�����ж�
	bool bAddBet = (lScoreCount > 0L)?true:false;
	if(lScoreCount < 0L) lScoreCount = (lScoreCount*-1L);

	//���ӳ���
	for (BYTE i = 0;i < CountArray(lScoreIndex); i++)
	{
		//������Ŀ
		BYTE cbScoreIndex=BET_COUNT-i-1;
		LONGLONG lCellCount=lScoreCount/lScoreIndex[cbScoreIndex];

		//�������
		if (lCellCount == 0L) continue;

		//�������
		for (LONGLONG j = 0;j < lCellCount;j++)
		{
			if (true == bAddBet)
			{
				// �������
				tagBetInfo BetInfo;
				INT_PTR nAreaIndex = rand()%m_ArrayBetArea[cbViewIndex].GetCount();
				CRect rect(m_ArrayBetArea[cbViewIndex][nAreaIndex]);
				BetInfo.cbBetIndex = cbScoreIndex;

				//������Դ
				CSize SizeBetItem(m_ImageBetView.GetWidth()/BET_COUNT,m_ImageBetView.GetHeight());
				SizeBetItem += CSize(10,10);

				BetInfo.nXPos = (rand()%(rect.Width() - SizeBetItem.cx)) + rect.left + SizeBetItem.cx/2; 
				BetInfo.nYPos = (rand()%(rect.Height() - SizeBetItem.cy)) + rect.top + SizeBetItem.cy/2;

				//��������
				m_BetInfoArray[cbViewIndex].Add(BetInfo);
			}
			else
			{
				for (int nIndex=0; nIndex<m_BetInfoArray[cbViewIndex].GetCount(); ++nIndex)
				{
					//�Ƴ��ж�
					tagBetInfo &BetInfo=m_BetInfoArray[cbViewIndex][nIndex];
					if (BetInfo.cbBetIndex == cbScoreIndex)
					{
						m_BetInfoArray[cbViewIndex].RemoveAt(nIndex);
						break;
					}
				}
			}
		}

		//������Ŀ
		lScoreCount -= lCellCount*lScoreIndex[cbScoreIndex];
	}

	//���½���
	InvalidGameView(0,0,0,0);

	return;
}


//�����˿�
void CGameClientView::SetCardInfo(BYTE cbCardCount[2], BYTE cbTableCardArray[2][3])
{
	if (cbCardCount != NULL)
	{
		CopyMemory(m_cbCardCount,cbCardCount,sizeof(m_cbCardCount));
		CopyMemory(m_cbTableCardArray,cbTableCardArray,sizeof(m_cbTableCardArray));
	}
	else
	{
		ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
		ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
	}
}


//���ֳɼ�
void CGameClientView::SetCurGameScore(LONGLONG lPlayScore[AREA_MAX], LONGLONG lPlayAllScore, LONGLONG lRevenue)
{
	memcpy(m_lPlayScore, lPlayScore, sizeof(m_lPlayScore));
	m_lPlayAllScore = lPlayAllScore;
	m_lRevenue = lRevenue;

	/*for ( int i = 0 ; i < AREA_MAX; ++i )
	{
		if ( lPlayScore[i] > 0 )
		{
			m_nWinCount++;
		}
		else if ( lPlayScore[i] < 0)
		{
			m_nLoseCount++;
		}
	}*/
}

//��ȡ����
BYTE CGameClientView::GetBetArea(CPoint MousePoint)
{
	for ( int i = 0 ; i < AREA_MAX; ++i )
	{
		for ( int n = 0; n < m_ArrayBetArea[i].GetCount(); ++n )
		{
			if ( m_ArrayBetArea[i][n].PtInRect(MousePoint))
			{
				return i;
			}
		}
	}

	return 0xFF;
}

//��Ϸ��С��
void CGameClientView::OnButtonMin()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_MIN,0);
}

//��Ϸ����
void CGameClientView::OnButtonGameOption()
{
	//SendEngineMessage(IDM_GAME_OPTIONS,1,0);
	//��ʾ����
	CDlgGameOption DlgGameOption;
	if (DlgGameOption.CreateGameOption(NULL,0)==true)
	{
		//��������
		ASSERT(CGlobalUnits::GetInstance()!=NULL);
		CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

		//��ѯ�ӿ�
		IGameFrameEngine* pIGameFrameEngine=(IGameFrameEngine *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_ENGINE,IID_IGameFrameEngine,VER_IGameFrameEngine);
		CGameClientEngine* pGameClientEngine = (CGameClientEngine*)pIGameFrameEngine;
		pGameClientEngine->AllowBackGroundSound(pGlobalUnits->m_bAllowBackGroundSound);
		return;
	}
}

//�ر���Ϸ
void CGameClientView::OnButtonClose()
{
	CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
	if (pGameClientEngine->GetGameStatus()!=GAME_STATUS_FREE)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		INT nRes=0;
		nRes=Information.ShowMessageBox(TEXT("��������Ϸ�У�ȷ��Ҫǿ����"),MB_ICONQUESTION|MB_YESNO,0);
		if (nRes==IDYES)
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_CLOSE);
		}

		return;
	}
	else
		AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_CLOSE);
}

//���а�ť
void CGameClientView::OnButtonGoBanker()
{
	//��ȡ�ӿ�
	CGameClientEngine *pGameClientEngine=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
	IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

	if(NULL==pIClientKernel) return;


	//��������
	CBankTipsDlg GameTips;

	POINT locPoint;
	locPoint.x=10;
	locPoint.y=340;
	ClientToScreen(&locPoint);

	GameTips.SetStartLoc(CPoint(locPoint.x,locPoint.y));
	//��������
	if (GameTips.DoModal()==IDOK)
	{
		if (GameTips.m_bRestore)
		{
			POINT locPoint;
			locPoint.x=266;
			locPoint.y=410-100;
			ClientToScreen(&locPoint);
			ShowInsureSave(pIClientKernel,CPoint(locPoint.x,locPoint.y));

		}
		else
		{
			POINT locPoint;
			locPoint.x=266;
			locPoint.y=410-100;
			ClientToScreen(&locPoint);
			ShowInsureGet(pIClientKernel,CPoint(locPoint.x,locPoint.y));
		}
			
	} 
	else 
		return;	
}

//���밴ť
void CGameClientView::OnBetButton100()
{
	//���ñ���
	m_lCurrentBet=100L;

	return;
}

//���밴ť
void CGameClientView::OnBetButton1000()
{
	//���ñ���
	m_lCurrentBet=1000L;

	return;
}

//���밴ť
void CGameClientView::OnBetButton10000()
{
	//���ñ���
	m_lCurrentBet=10000L;

	return;
}

//���밴ť
void CGameClientView::OnBetButton100000()
{
	//���ñ���
	m_lCurrentBet=100000L;

	return;
}

//���밴ť
void CGameClientView::OnBetButton1000000()
{
	//���ñ���
	m_lCurrentBet=1000000L;

	return;
}
//���밴ť
void CGameClientView::OnBetButton5000000()
{
	//���ñ���
	m_lCurrentBet=5000000L;

	return;
}

//���밴ť
void CGameClientView::OnBetButton10000000()
{
	//���ñ���
	m_lCurrentBet=10000000L;

	return;
}
//�鿴·��
void CGameClientView::OnViewChart()
{
	if (m_DlgViewChart.m_hWnd == NULL) m_DlgViewChart.Create(IDD_VIEW_CHART,this);

	//��ʾ�ж�
	if (!m_DlgViewChart.IsWindowVisible())
	{
		m_DlgViewChart.CenterWindow(this);
		m_DlgViewChart.ShowWindow(SW_SHOW);
	}
	else
	{
		m_DlgViewChart.ShowWindow(SW_HIDE);
	}
}

//��ʱ����Ϣ
void CGameClientView::OnTimer(UINT nIDEvent)
{

	//��ע�ӳ�
	if(nIDEvent == IDI_BET_DELAY)
	{
		m_bEnableBet = !m_bEnableBet;
		KillTimer(IDI_BET_DELAY);
	}
	
	if ( nIDEvent == IDI_FLASH_WINNER )				//����ʤ��
	{
		//���ñ���		
		m_bFlashResult=!m_bFlashResult;
		/*if ( m_bFlashAreaAlpha )
		{
			m_nFlashAreaAlpha += 10;
			if ( m_nFlashAreaAlpha > 240)
			{
				m_bFlashAreaAlpha = false;
			}
		}
		else
		{
			m_nFlashAreaAlpha -= 10;
			if ( m_nFlashAreaAlpha < 10)
			{
				m_bFlashAreaAlpha = true;
			}
		}*/
		

		//���½���
		InvalidGameView(0,0,0,0);

		return;
	}
	//else if ( nIDEvent == IDI_FLASH_BET )	//ѡȡ����
	//{
	//	int nIndex = 0;
	//	while( nIndex < m_ArrayCurrentFlash.GetCount() )
	//	{
	//		if( m_ArrayCurrentFlash[nIndex].bFlashAreaAlpha )
	//		{
	//			m_ArrayCurrentFlash[nIndex].nFlashAreaAlpha += 30;
	//			if ( m_ArrayCurrentFlash[nIndex].nFlashAreaAlpha > 255 )
	//			{
	//				m_ArrayCurrentFlash[nIndex].nFlashAreaAlpha = 255;
	//			}
	//			nIndex++;
	//		}
	//		else
	//		{
	//			m_ArrayCurrentFlash[nIndex].nFlashAreaAlpha -= 30;
	//			if ( m_ArrayCurrentFlash[nIndex].nFlashAreaAlpha < 10 )
	//			{
	//				m_ArrayCurrentFlash.RemoveAt(nIndex);
	//				continue;
	//			}
	//			nIndex++;
	//		}
	//	}

	//	//���½���
	//	//InvalidGameView(0,0,0,0);

	//	return;
	//}
	else if ( nIDEvent == IDI_SHOW_CHANGE_BANKER )	//�ֻ�ׯ��
	{
		ShowChangeBanker( false );
		return;
	}
	else if ( nIDEvent == IDI_FLEX_MOVE )		//�����ƶ�
	{
		if ( m_pImageFlex == NULL )
			return;
		
		//λ�øı�
		m_ptFlexMove.y = (m_pImageFlex->GetHeight() + 137) * m_nFlexMove / FLEX_MOVE_COUNT + m_ptFlexBeing.y - m_pImageFlex->GetHeight();

		if ( m_bFlexShow )
		{
			m_nFlexMove++;
			if( m_nFlexMove > FLEX_MOVE_COUNT )
			{
				m_nFlexMove = FLEX_MOVE_COUNT;
				KillTimer(IDI_FLEX_MOVE);

				if ( m_cbGameStatus == GAME_SCENE_END && m_enFlexMode == enFlexDealCrad )
				{
					//��ʼ����
					DispatchCard();
				}
			}
		}
		else
		{
			m_nFlexMove--;
			if ( m_nFlexMove < 0 )
			{
				m_nFlexMove = 0;
				KillTimer(IDI_FLEX_MOVE);

				if ( m_cbGameStatus == GAME_SCENE_FREE && m_enFlexMode == enFlexDealCrad )
				{
					FlexAnimation(enFlexNULL, false);
				}
				else if ( m_cbGameStatus == GAME_SCENE_END && m_enFlexMode == enFlexBetTip )
				{
					//��ʾ����
					m_ptFlexMove.y = m_ptFlexBeing.y - m_ImageDealBack.GetHeight();
					FlexAnimation(enFlexDealCrad, true);
				}
				else if ( m_cbGameStatus == GAME_SCENE_END && m_enFlexMode == enFlexDealCrad )
				{
					//��ʾ����
					m_ptFlexMove.y = m_ptFlexBeing.y - m_ImageGameEnd.GetHeight();
					FlexAnimation(enFlexGameEnd, true);
					InsertGameEndInfo();
					for ( int i = 0 ; i < AREA_MAX; ++i )
					{
						if ( m_lPlayScore[i] > 0 )
						{
							m_nWinCount++;
						}
						else if ( m_lPlayScore[i] < 0)
						{
							m_nLoseCount++;
						}
					}
				}
			}
		}
		
		//���½���
		InvalidGameView(0,0,0,0);
		return;
	}
	else if ( nIDEvent == IDI_DISPATCH_CARD )		// ���ƶ���
	{
		CPoint ptMoveCrad;
		CPoint ptObjectiveCrad = m_CardControl[m_nDealIndex].GetBenchmarkPos();

		//��������
		if( m_nDealMove == 0 )
		{
			SendEngineMessage(IDM_PLAY_SOUND,(WPARAM)TEXT("DISPATCH_CARD"),0);
		}

		if( m_nDealMove <= DEAL_MOVE_COUNT_S )
		{
			ptMoveCrad.y = ( ptObjectiveCrad.y - m_ptDispatchCard.y ) * m_nDealMove / DEAL_MOVE_COUNT_S + m_ptDispatchCard.y;
			ptMoveCrad.x = m_ptDispatchCard.x;
		}
		else if( m_nDealMove - DEAL_MOVE_COUNT_S <= DEAL_MOVE_COUNT_H )
		{
			ptMoveCrad.y = ptObjectiveCrad.y;
			ptMoveCrad.x = ( ptObjectiveCrad.x - m_ptDispatchCard.x ) * (m_nDealMove - DEAL_MOVE_COUNT_S) / DEAL_MOVE_COUNT_H + m_ptDispatchCard.x;
		}
	
		m_DispatchCard.SetBenchmarkPos(ptMoveCrad.x,ptMoveCrad.y);
   		m_DispatchCard.ShowCardControl(true);

		m_nDealMove++;
		if ( m_nDealMove > DEAL_MOVE_COUNT_H + DEAL_MOVE_COUNT_S )
		{
			m_nDealMove = 0;
			if ( m_CardControl[m_nDealIndex].GetCardCount() < m_cbCardCount[m_nDealIndex] )
			{
				m_CardControl[m_nDealIndex].SetCardData( m_cbTableCardArray[m_nDealIndex], m_CardControl[m_nDealIndex].GetCardCount() + 1);
			}
			int nNextCrad = (m_nDealIndex + 1)%2;
			if ( m_CardControl[nNextCrad].GetCardCount() < m_cbCardCount[nNextCrad] )
			{
				m_nDealIndex = nNextCrad;
			}

			if ( m_CardControl[INDEX_PLAYER].GetCardCount() == m_cbCardCount[INDEX_PLAYER]
				&& m_CardControl[INDEX_BANKER].GetCardCount() == m_cbCardCount[INDEX_BANKER])
			{
				m_DispatchCard.SetCardData(NULL, 0);

				//������ʾ
				SetDispatchCardTips();

				//��ɷ���
				FinishDispatchCard();
			}
			else 
			{
				//���η�����
				m_DispatchCard.ShowCardControl(false);

				//������ʾ
				SetDispatchCardTips();

				//ɾ����ʱ
				KillTimer(IDI_DISPATCH_CARD);

				//���������ʱ
				if( m_CardControl[m_nDealIndex].GetCardCount() >= 2 )
					SetTimer(IDI_DISPATCH_INTERVAL, 1000, NULL);
				else
					SetTimer(IDI_DISPATCH_INTERVAL, 300, NULL);
			}
		}

		//���½���
		InvalidGameView(0,0,0,0);
		return;
	}
	else if ( nIDEvent == IDI_DISPATCH_INTERVAL )
	{
		//ɾ����ʱ
		KillTimer(IDI_DISPATCH_INTERVAL);

		//��������
		SetTimer(IDI_DISPATCH_CARD, 30, NULL);

		//���½���
		InvalidGameView(0,0,0,0);
		return;
	}
	else if ( nIDEvent == IDI_END_INTERVAL )
	{
		//ɾ����ʱ
		KillTimer(IDI_END_INTERVAL);

		BYTE cbCrad[1] = {0};
		m_CardControl[INDEX_PLAYER].SetCardData(cbCrad, 0);
		m_CardControl[INDEX_BANKER].SetCardData(cbCrad, 0);

		//��������
		FlexAnimation(enFlexDealCrad, false);

		//���½���
		InvalidGameView(0,0,0,0);
		return;
	}
	else if ( nIDEvent == IDI_WIN_TYPE_DELAY)
	{
		//ɾ����ʱ
		KillTimer(IDI_WIN_TYPE_DELAY);

		//��������
		SetTimer(IDI_WIN_TYPE, 40, NULL);

		//���½���
		InvalidGameView(0,0,0,0);

		return;
	}
	// ��Ӯ������
	else if ( nIDEvent == IDI_WIN_TYPE )
	{
		if ( m_nWinShowArea >= CountArray(m_nWinShowIndex) || m_nWinShowArea < 0 )
		{
			KillTimer(IDI_WIN_TYPE);
			return;
		}

		m_nWinShowIndex[m_nWinShowArea]++;

		if ( m_nWinShowIndex[m_nWinShowArea] == 6)
		{
			m_nWinShowArea = INT_MAX;
			KillTimer(IDI_WIN_TYPE);

			//�������Ӯ ������Ӯ����
			for ( int i = 0 ; i < AREA_MAX; ++i )
			{
				if ( m_lPlayScore[i] != 0 )
				{
					SetTimer(IDI_END_INTERVAL, 3000, NULL);
					break;
				}
			}
			return;
		}

		//���½���
		InvalidGameView(0,0,0,0);
		return;
	}

	__super::OnTimer(nIDEvent);
}

//�����Ϣ
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	//BYTE cbCrad[3] = {0x03,0x03,0x03};
	//m_CardControl[INDEX_PLAYER].SetCardData(cbCrad, 3);

	//m_CardControl[1].SetCardData(cbCrad, 1);

	////���½���
	//InvalidGameView(0,0,0,0);
	if ( m_lCurrentBet != 0L && m_bEnableBet == true)
	{
		LONGLONG lMaxPlayerScore = 0;
		BYTE cbBetArea = GetBetArea(Point);
		lMaxPlayerScore = GetMaxPlayerScore(cbBetArea);

		if ( lMaxPlayerScore < m_lCurrentBet )
			return ;

		//������Ϣ
		if ( cbBetArea != 0xFF ) 
		{
			/*m_bEnableBet = false;
			SetTimer(IDI_BET_DELAY,100,NULL);*/
			SendEngineMessage(IDM_PALY_BET, cbBetArea, 0);
		}
	}
	
	RefreshGameView();

	__super::OnLButtonDown(nFlags,Point);

	LPARAM lParam = MAKEWPARAM(Point.x,Point.y);
	AfxGetMainWnd()->PostMessage(WM_LBUTTONDOWN,0,lParam);
}

//�����Ϣ
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//���ñ���
	m_lCurrentBet = 0L;
	for ( int i = 0 ; i < m_ArrayCurrentFlash.GetCount(); ++i )
	{
		m_ArrayCurrentFlash[i].bFlashAreaAlpha = false;
	}

	if (m_cbJetArea!=0xFF && m_cbGameStatus!=GAME_SCENE_END)
	{
		m_cbJetArea=0xFF;
	}	

	InvalidGameView(0,0,0,0);

	__super::OnLButtonDown(nFlags,Point);
}

//�����Ϣ
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	if ( m_lCurrentBet != 0L && m_cbGameStatus == GAME_SCENE_BET )
	{
		//��ȡ����
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);
		BYTE cbBetArea = GetBetArea(MousePoint);

		//���ñ���
		if ( m_cbJetArea!= cbBetArea )
		{
			m_cbJetArea = cbBetArea;
			InvalidGameView(0,0,0,0);
		}

		//���ñ���
		bool bBid = false;
		for ( int i = 0 ; i < m_ArrayCurrentFlash.GetCount(); ++i )
		{
			if ( m_ArrayCurrentFlash[i].cbFlashArea == cbBetArea )
			{
				bBid = true;
				m_ArrayCurrentFlash[i].bFlashAreaAlpha = true;
			}
			else
			{
				m_ArrayCurrentFlash[i].bFlashAreaAlpha = false;
			}
		}

		if ( !bBid && cbBetArea < AREA_MAX )
		{
			tagFlashInfo FlashInfo;
			FlashInfo.cbFlashArea = cbBetArea;
			FlashInfo.bFlashAreaAlpha = true;
			FlashInfo.nFlashAreaAlpha = 10;
			m_ArrayCurrentFlash.Add(FlashInfo);
		}

		//�����ж�
		if (cbBetArea == 0xFF)
		{
			SetCursor( LoadCursor(NULL, IDC_ARROW) );
			return TRUE;
		}

		//��С�ж�
		LONGLONG lMaxPlayerScore = GetMaxPlayerScore(cbBetArea);
		
		//LONGLONG lLeaveScore=lMaxPlayerScore;
		//
		////���ù��
		//if (m_lCurrentBet>lLeaveScore)
		//{
		//	if (lLeaveScore>=10000000L) SetCurrentBet(10000000L);
		//	else if (lLeaveScore>=5000000L) SetCurrentBet(5000000L);
		//	else if (lLeaveScore>=1000000L) SetCurrentBet(1000000L);
		//	else if (lLeaveScore>=100000L) SetCurrentBet(100000L);
		//	else if (lLeaveScore>=10000L) SetCurrentBet(10000L);
		//	else if (lLeaveScore>=1000L) SetCurrentBet(1000L);
		//	else SetCurrentBet(0L);
		//}

		if ( lMaxPlayerScore < m_lCurrentBet )
		{
			SetCursor( LoadCursor(NULL, IDC_NO) );
			return TRUE;
		}

		////�Ϸ��ж�
		//if ((m_lAllBet[cbBetArea]+m_lCurrentBet)>m_lAreaLimitScore || lMaxPlayerScore < m_lCurrentBet || 0 == m_lCurrentBet)
		//{
		//	SetCursor(LoadCursor(NULL,IDC_NO));
		//		return TRUE;	
		//}

		//���ù��
		switch (m_lCurrentBet)
		{
		case 100:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100)) );
				return TRUE;
			}
		case 1000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000)) );
				return TRUE;
			}
		case 10000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_10000)) );
				return TRUE;
			}
		case 100000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100000)) );
				return TRUE;
			}
		case 1000000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000000)) );
				return TRUE;
			}
		case 5000000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_5000000)) );
				return TRUE;
			}	
		case 10000000:
			{
				SetCursor( LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_10000000)) );
				return TRUE;
			}
		}
	}

	return __super::OnSetCursor(pWnd, nHitTest, uMessage);
}

//�滭��ע����
void CGameClientView::JettonAreaFrame(CDC *pDC)
{
	//�Ϸ��ж�
	if (m_cbJetArea==0xFF) return;
	if (m_cbGameStatus == GAME_SCENE_BET)
	{
		switch (m_cbJetArea)
		{
		case AREA_XIAN:	
			{
				//�滭ͼƬ
				m_ImageFrame[m_cbJetArea].DrawImage(pDC,m_rcXian.left-34, m_rcXian.top+2);
				break;
			}
		case AREA_PING:
			{
				//�滭ͼƬ
				m_ImageFrame[m_cbJetArea].DrawImage(pDC,m_rcPing.left-34, m_rcPing.top+2);
				break;
			}
		case AREA_ZHUANG:
			{	
				//�滭ͼƬ
				m_ImageFrame[m_cbJetArea].DrawImage(pDC,m_rcZhuang.left-34, m_rcZhuang.top+2+15);
				break;
			}
		case AREA_XIAN_TIAN:	
			{
				//�滭ͼƬ
				m_ImageFrame[m_cbJetArea].DrawImage(pDC,m_rcXianTian.left-34, m_rcXianTian.top+2);
				break;
			}
		case AREA_ZHUANG_TIAN:
			{
				//�滭ͼƬ
				m_ImageFrame[m_cbJetArea].DrawImage(pDC,m_rcZhuangTian.left-34+1, m_rcZhuangTian.top+2);
				break;
			}
		case AREA_TONG_DUI:
			{	
				//�滭ͼƬ
				m_ImageFrame[m_cbJetArea].DrawImage(pDC,m_rcTongDui.left-34, m_rcTongDui.top+2);
				break;
			}
		case AREA_XIAN_DUI:	
			{
				//�滭ͼƬ
				m_ImageFrame[m_cbJetArea].DrawImage(pDC,m_rcXianDui.left-34-11, m_rcXianDui.top+2);
				break;
			}
		case AREA_TIANGANG:
			{
				//�滭ͼƬ
				m_ImageFrame[m_cbJetArea].DrawImage(pDC,m_rcZhuangDui.left-34, m_rcZhuangDui.top+2);
				break;
			}		
		}
	}
}

//�ֻ�ׯ��
void CGameClientView::ShowChangeBanker( bool bChangeBanker )
{
	//�ֻ�ׯ��
	if ( bChangeBanker )
	{
		SetTimer( IDI_SHOW_CHANGE_BANKER, 3000, NULL );
		m_bShowChangeBanker = true;
	}
	else
	{
		KillTimer( IDI_SHOW_CHANGE_BANKER );
		m_bShowChangeBanker = false ;
	}

	//���½���
	InvalidGameView(0,0,0,0);
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

//�ƶ���ť
void CGameClientView::OnScoreMoveL()
{
	INT nMaxCol = IsZoomedMode()?ZOOMED_RECORD_COL_COUNT:NORMAL_RECORD_COL_COUNT;
	if(m_nHeadCol >=0) m_nHeadCol--;
	if(m_nHeadCol == 0)	m_btScoreMoveL.EnableWindow(FALSE);
	m_btScoreMoveR.EnableWindow(TRUE);
	//if(!IsZoomedMode())
	//{
	//	m_nRecordHead = ( m_nRecordHead + 1 ) % ( MAX_RECORD_COL_COUNT -  NORMAL_RECORD_COL_COUNT);
	//}
	//else
	//{
	//	m_nRecordHead = ( m_nRecordHead + 1 ) % ( MAX_RECORD_COL_COUNT -  ZOOMED_RECORD_COL_COUNT);
	//}

	//m_btScoreMoveR.EnableWindow(TRUE);
	//if(!IsZoomedMode())
	//{
	//	for(int i = 0;i < 6;i++)
	//	{
	//		if(m_RecordResult[NORMAL_RECORD_COL_COUNT + m_nRecordHead].resultIndex[i] != INVALID_DWORD)
	//			break;
	//	}
	//	if(i == 6)
	//	{
	//		m_btScoreMoveL.EnableWindow(FALSE);
	//	}
	//}
	//else
	//{
	//	for(int i = 0;i < 6;i++)
	//	{
	//		if(m_RecordResult[ZOOMED_RECORD_COL_COUNT + m_nRecordHead].resultIndex[i] != INVALID_DWORD)
	//			break;
	//	}
	//	if(i == 6)
	//	{
	//		m_btScoreMoveL.EnableWindow(FALSE);
	//	}
	//}
	////���½���
	//InvalidGameView(0,0,0,0);

	//if( m_nRecordHead == 0)
	//	return;
	//int normalCount = MAX_RECORD_COL_COUNT -  NORMAL_RECORD_COL_COUNT;
	//int zoomedCount = MAX_RECORD_COL_COUNT -  ZOOMED_RECORD_COL_COUNT;
	//if(!IsZoomedMode())
	//{
	//	m_nRecordHead = (m_nRecordHead - 1 + normalCount) % normalCount;
	//}
	//else
	//{
	//	m_nRecordHead = (m_nRecordHead - 1 + zoomedCount) % zoomedCount;
	//}
	//if ( m_nRecordHead == m_nRecordFirst ) 
	//	m_btScoreMoveL.EnableWindow(FALSE);
	//m_btScoreMoveR.EnableWindow(TRUE);

	//���½���
	InvalidGameView(0,0,0,0);
}

//�ƶ���ť
void CGameClientView::OnScoreMoveR()
{
	INT nMaxCol = IsZoomedMode()?ZOOMED_RECORD_COL_COUNT:NORMAL_RECORD_COL_COUNT;
	if(m_nTotalCol - m_nHeadCol >=nMaxCol) m_nHeadCol++;
	if(m_nTotalCol - m_nHeadCol < nMaxCol)	m_btScoreMoveR.EnableWindow(FALSE);

	m_btScoreMoveL.EnableWindow(TRUE);
	//if( m_nRecordHead == 0)
	//	return;
	//int normalCount = MAX_RECORD_COL_COUNT -  NORMAL_RECORD_COL_COUNT;
	//int zoomedCount = MAX_RECORD_COL_COUNT -  ZOOMED_RECORD_COL_COUNT;
	//if(!IsZoomedMode())
	//{
	//	m_nRecordHead = (m_nRecordHead - 1 + normalCount) % normalCount;
	//}
	//else
	//{
	//	m_nRecordHead = (m_nRecordHead - 1 + zoomedCount) % zoomedCount;
	//}
	//if ( m_nRecordHead == m_nRecordFirst ) 
	//	m_btScoreMoveR.EnableWindow(FALSE);
	//m_btScoreMoveL.EnableWindow(TRUE);
	////���½���
	//InvalidGameView(0,0,0,0);

	/*if(!IsZoomedMode())
	{
		m_nRecordHead = ( m_nRecordHead + 1 ) % ( MAX_RECORD_COL_COUNT -  NORMAL_RECORD_COL_COUNT);
	}
	else
	{
		m_nRecordHead = ( m_nRecordHead + 1 ) % ( MAX_RECORD_COL_COUNT -  ZOOMED_RECORD_COL_COUNT);
	}

	m_btScoreMoveL.EnableWindow(TRUE);
	if(!IsZoomedMode())
	{
		for(int i = 0;i < 6;i++)
		{
			if(m_RecordResult[NORMAL_RECORD_COL_COUNT + m_nRecordHead].resultIndex[i] != INVALID_DWORD)
				break;
		}
		if(i == 6)
		{
			m_btScoreMoveR.EnableWindow(FALSE);
		}
	}
	else
	{
		for(int i = 0;i < 6;i++)
		{
			if(m_RecordResult[ZOOMED_RECORD_COL_COUNT + m_nRecordHead].resultIndex[i] != INVALID_DWORD)
				break;
		}
		if(i == 6)
		{
			m_btScoreMoveR.EnableWindow(FALSE);
		}
	}*/
	//���½���
	InvalidGameView(0,0,0,0);
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

// �����б���
void CGameClientView::OnValleysUp()
{
	if ( m_nShowValleyIndex > 0 )
		m_nShowValleyIndex--;

	//���½���
	UpdateButtonContron();
	InvalidGameView(0,0,0,0);
}

// �����б���
void CGameClientView::OnValleysDown()
{
	if( m_nShowValleyIndex < m_ValleysList.GetCount() - 1 )
		m_nShowValleyIndex++;

	//���½���
	UpdateButtonContron();
	InvalidGameView(0,0,0,0);
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
	strMessage.Format(TEXT("%s"), TEXT("[ϵͳ��Ϣ]:"));

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
	index		= strInit.Find(':');
	if (index<0)
	{
		return;
	}
	
	FileLeft	= strInit.Left(index+1);
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
	ZeroMemory(&cf, sizeof(cf));

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

//����Ч��
bool CGameClientView::EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex)
{
	//״̬�ж�
	CGlobalUnits* pGlobalUnits = CGlobalUnits::GetInstance();
	IClientKernel* m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
	IClientUserItem* m_pIMySelfUserItem = GetClientUserItem(m_wMeChairID);
	if (m_pIClientKernel==NULL) return false;
	if (m_pIMySelfUserItem==NULL) return false;

	//���ȱȽ�
	if(pszChatString != NULL)
	{
		CString strChat=pszChatString;
		if(strChat.GetLength() >= (LEN_USER_CHAT/2))
		{
			InsertMessage(TEXT("��Ǹ����������������ݹ����������̺��ٷ��ͣ�"),COLOR_WARN);
			InsertMessage(TEXT("\r\n"));
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
	if (CServerRule::IsForfendGameChat(pServerAttribute->dwServerRule)&&(cbMasterOrder==0))
	{
		//ԭʼ��Ϣ
		if (wExpressionIndex==INVALID_WORD)
		{
			CString strDescribe;
			strDescribe.Format(TEXT("��%s������ʧ��"),pszChatString);
			InsertMessage(strDescribe,COLOR_EVENT);
			InsertMessage(TEXT("\r\n"));
		}

		//������Ϣ
		InsertSystemMessage(TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û��������죡"));

		return false;
	}

	//Ȩ���ж�
	if (CUserRight::CanGameChat(pUserAttribute->dwUserRight)==false)
	{
		//ԭʼ��Ϣ
		if (wExpressionIndex==INVALID_WORD)
		{
			CString strDescribe;
			strDescribe.Format(TEXT("��%s������ʧ��"),pszChatString);
			InsertMessage(strDescribe,COLOR_EVENT);
			InsertMessage(TEXT("\r\n"));
		}

		//������Ϣ
		InsertSystemMessage(TEXT("��Ǹ����û����Ϸ���Ե�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"));

		return false;
	}

	//�ٶ��ж�
	DWORD dwCurrentTime=(DWORD)time(NULL);
	if ((cbMasterOrder==0)&&((dwCurrentTime-m_dwChatTime)<=TIME_USER_CHAT))
	{
		//ԭʼ��Ϣ
		if (wExpressionIndex==INVALID_WORD)
		{
			CString strDescribe;
			strDescribe.Format(TEXT("��%s������ʧ��"),pszChatString);
			InsertMessage(strDescribe,COLOR_EVENT);
			InsertMessage(TEXT("\r\n"));
		}

		//������Ϣ
		InsertSystemMessage(TEXT("����˵���ٶ�̫���ˣ����������ȱ�����Ϣ�°ɣ�"));

		return false;
	}

	//���ñ���
	m_dwChatTime=dwCurrentTime;
	//	m_strChatString=pszChatString;

	return true;
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

	strInfo.Format(TEXT("����:%I64d"),m_lPlayAllScore);

	InsertAllChatMessage(strInfo,0);
}

//����Ա����
void CGameClientView::OpenAdminWnd()
{
	if( m_pClientControlDlg != NULL )
	{
		if( !m_pClientControlDlg->IsWindowVisible() ) 
		{
			m_pClientControlDlg->ShowWindow(SW_SHOW);
			m_pClientControlDlg->UpdateControl();
		}
		else 
			m_pClientControlDlg->ShowWindow(SW_HIDE);
	}
}


//�����ע
LONGLONG CGameClientView::GetMaxPlayerScore( BYTE cbBetArea )
{	
	if ( m_wMeChairID == INVALID_CHAIR)
		return 0L;

	IClientUserItem* pMeUserItem = GetClientUserItem(m_wMeChairID);
	if ( pMeUserItem == NULL || cbBetArea >= AREA_MAX )
		return 0L;

	if( cbBetArea >= AREA_MAX )
		return 0L;

	//����ע��
	LONGLONG lNowBet = 0l;
	for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex ) 
		lNowBet += m_lPlayBet[nAreaIndex];

	//ׯ�ҽ��
	LONGLONG lBankerScore = -1;

	//������
	BYTE cbMultiple[AREA_MAX] = {MULTIPLE_XIAN, MULTIPLE_PING, MULTIPLE_ZHUANG, 
		MULTIPLE_XIAN_TIAN, MULTIPLE_ZHUANG_TIAN, MULTIPLE_TONG_DIAN, 
		MULTIPLE_XIAN_PING, MULTIPLE_ZHUANG_PING};

	//������Ӯ
	BYTE cbArae[4][4] = {	{ AREA_XIAN_DUI,	255,			AREA_MAX,			AREA_MAX }, 
							{ AREA_TIANGANG,	255,			AREA_MAX,			AREA_MAX }, 
							{ AREA_XIAN,		AREA_PING,		AREA_ZHUANG,		AREA_MAX },  
							{ AREA_XIAN_TIAN,	AREA_TONG_DUI,	AREA_ZHUANG_TIAN,	255 }};
	//�����趨
	for ( int nTopL = 0; nTopL < 4; ++nTopL )
	{
		if( cbArae[0][nTopL] == AREA_MAX )
			continue;

		for ( int nTopR = 0; nTopR < 4; ++nTopR )
		{
			if( cbArae[1][nTopR] == AREA_MAX )
				continue;

			for ( int nCentral = 0; nCentral < 4; ++nCentral )
			{
				if( cbArae[2][nCentral] == AREA_MAX )
					continue;

				for ( int nBottom = 0; nBottom < 4; ++nBottom )
				{
					if( cbArae[3][nBottom] == AREA_MAX )
						continue;

					BYTE cbWinArea[AREA_MAX] = {FALSE};

					//ָ����ʤ����
					if ( cbArae[0][nTopL] != 255 && cbArae[0][nTopL] != AREA_MAX )
						cbWinArea[cbArae[0][nTopL]] = TRUE;

					if ( cbArae[1][nTopR] != 255 && cbArae[1][nTopR] != AREA_MAX )
						cbWinArea[cbArae[1][nTopR]] = TRUE;

					if ( cbArae[2][nCentral] != 255 && cbArae[2][nCentral] != AREA_MAX )
						cbWinArea[cbArae[2][nCentral]] = TRUE;

					if ( cbArae[3][nBottom] != 255 && cbArae[3][nBottom] != AREA_MAX )
						cbWinArea[cbArae[3][nBottom]] = TRUE;

					//ѡ������Ϊ���ʤ����ͬ�ȼ�������������Ϊ����䡣�Եó������עֵ
					for ( int i = 0; i < 4; i++ )
					{
						for ( int j = 0; j < 4; j++ )
						{
							if ( cbArae[i][j] == cbBetArea )
							{
								for ( int n = 0; n < 4; ++n )
								{
									if ( cbArae[i][n] != 255 && cbArae[i][n] != AREA_MAX )
									{
										cbWinArea[cbArae[i][n]] = FALSE;
									}
								}
								cbWinArea[cbArae[i][j]] = TRUE;
							}
						}
					}

					LONGLONG lScore = m_lBankerScore;
					for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex ) 
					{
						if ( cbWinArea[nAreaIndex] == TRUE )
						{
							lScore -= m_lAllBet[nAreaIndex]*(cbMultiple[nAreaIndex] - 1);
						}
						else if ( cbWinArea[AREA_PING] == TRUE && ( nAreaIndex == AREA_XIAN || nAreaIndex == AREA_ZHUANG ) )
						{

						}
						else
						{
							lScore += m_lAllBet[nAreaIndex];
						}
					}
					if ( lBankerScore == -1 )
						lBankerScore = lScore;
					else
						lBankerScore = min(lBankerScore, lScore);
				}
			}
		}
	}

	//�����ע
	LONGLONG lMaxBet = 0L;

	//�����ע
	lMaxBet = min( m_lMeMaxScore - lNowBet, m_lAreaLimitScore - m_lPlayBet[cbBetArea]);

	lMaxBet = min( m_lMeMaxScore - lNowBet, m_lAreaLimitScore - m_lAllBet[cbBetArea]);

	lMaxBet = min( lMaxBet, lBankerScore / (cbMultiple[cbBetArea] - 1));

	//��������
	ASSERT(lMaxBet >= 0);
	lMaxBet = max(lMaxBet, 0);

	return lMaxBet;
}

//��ʼ����
void CGameClientView::DispatchCard()
{
	//�����˿�
	BYTE cbCardData = 0;
	m_nDealMove = 0;
	m_nDealIndex = 0;
	m_DispatchCard.SetCardData(&cbCardData,1);
	m_DispatchCard.SetBenchmarkPos(m_ptDispatchCard.x,m_ptDispatchCard.y);

	//���ö�ʱ��
	SetTimer(IDI_DISPATCH_CARD, 30, NULL);

	//��Ӯ����
	m_nWinShowArea = INT_MAX;
	ZeroMemory(m_nWinShowIndex,sizeof(m_nWinShowIndex));

	//���ñ�ʶ
	m_bNeedSetGameRecord = true;
}

//��������
void CGameClientView::FinishDispatchCard(bool bScene)
{
	//����ж�
	if (m_bNeedSetGameRecord == false) return;

	//���ñ�ʶ
	m_bNeedSetGameRecord = false;

	//ɾ����ʱ��
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_FLEX_MOVE);

	//������Ϣ
	FlexAnimation(enFlexDealCrad, true, false);

	//�����˿�
	m_CardControl[INDEX_PLAYER].SetCardData(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	m_CardControl[INDEX_BANKER].SetCardData(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);
	m_DispatchCard.SetCardData(NULL,0);

	//����
	FlashAnimation(true);

	//���ü�¼
	//�˿˵���
	BYTE cbPlayerPoint = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerPoint = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//��������
	enOperateResult OperateResult = enOperateResult_NULL;
	if (0 < m_lPlayAllScore) OperateResult = enOperateResult_Win;
	else if (m_lPlayAllScore < 0) OperateResult = enOperateResult_Lost;
	else OperateResult = enOperateResult_NULL;

	//�ƶ�Ӯ��
	BYTE cbWinner,cbKingWinner;
	bool bPlayerTwoPair,bBankerTwoPair;
	DeduceWinner(cbWinner, cbKingWinner, bPlayerTwoPair, bBankerTwoPair);

	//�����¼
	if(!bScene)
	{
		SetGameHistory(OperateResult, cbPlayerPoint, cbBankerPoint, cbKingWinner, bPlayerTwoPair,bBankerTwoPair);
		SetRecordCol();
	}

	//�ۼƻ���
	m_lMeStatisticScore += m_lPlayAllScore;
	m_lBankerWinScore = m_lBankerWinTempScore;

	//��������
	if ( m_lPlayAllScore > 0 ) 
		SendEngineMessage(IDM_PLAY_SOUND,(WPARAM)TEXT("END_WIN"),0);
	else if ( m_lPlayAllScore < 0 ) 
		SendEngineMessage(IDM_PLAY_SOUND,(WPARAM)TEXT("END_LOST"),0);
	else 
		SendEngineMessage(IDM_PLAY_SOUND,(WPARAM)TEXT("END_DRAW"),0);

	//������Ӯ����
	if( cbPlayerPoint < cbBankerPoint )
		m_nWinShowArea = AREA_ZHUANG;
	else if( cbPlayerPoint > cbBankerPoint )
		m_nWinShowArea = AREA_XIAN;
	else
		m_nWinShowArea = AREA_PING;

	SetTimer(IDI_WIN_TYPE_DELAY, 1100, NULL);
}

//ʤ���߿�
void CGameClientView::DrawFlashFrame(CDC *pDC, int nWidth, int nHeight)
{
	if (m_bFlashResult && m_cbGameStatus == GAME_SCENE_END)
	{
		for ( int i = 0; i < m_ArrayFlashArea.GetCount(); ++i )
		{
			m_ImageFrame[m_ArrayFlashArea[i]].DrawImage(pDC,m_ptBetFrame[m_ArrayFlashArea[i]].x, m_ptBetFrame[m_ArrayFlashArea[i]].y);
		}
	}
	/*if ( m_cbGameStatus == GAME_SCENE_BET )
	{
		for ( int i = 0; i < m_ArrayCurrentFlash.GetCount(); ++i )
		{
			m_ImageFrame[m_ArrayCurrentFlash[i].cbFlashArea].AlphaDrawImage(pDC, m_ptBetFrame[m_ArrayCurrentFlash[i].cbFlashArea].x, m_ptBetFrame[m_ArrayCurrentFlash[i].cbFlashArea].y, (BYTE)m_ArrayCurrentFlash[i].nFlashAreaAlpha);
		}
	}
	else if ( m_cbGameStatus == GAME_SCENE_END && m_nFlashAreaAlpha > 10 )
	{
		for ( int i = 0; i < m_ArrayFlashArea.GetCount(); ++i )
		{
			m_ImageFrame[m_ArrayFlashArea[i]].AlphaDrawImage(pDC, m_ptBetFrame[m_ArrayFlashArea[i]].x, m_ptBetFrame[m_ArrayFlashArea[i]].y, (BYTE)m_nFlashAreaAlpha);
		}
	}*/
}

//�ƶ�Ӯ��
void CGameClientView::DeduceWinner( BYTE* pWinArea )
{
	//�����Ƶ�
	BYTE cbPlayerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//ʤ������--------------------------
	//ƽ
	if( cbPlayerCount == cbBankerCount )
	{
		pWinArea[AREA_PING] = TRUE;

		// ͬƽ��
		if ( m_cbCardCount[INDEX_PLAYER] == m_cbCardCount[INDEX_BANKER] )
		{
			for (WORD wCardIndex = 0; wCardIndex < m_cbCardCount[INDEX_PLAYER]; ++wCardIndex )
			{
				BYTE cbBankerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][wCardIndex]);
				BYTE cbPlayerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][wCardIndex]);
				if ( cbBankerValue != cbPlayerValue ) break;
			}

			if ( wCardIndex == m_cbCardCount[INDEX_PLAYER] )
			{
				pWinArea[AREA_TONG_DUI] = TRUE;
			}
		}
	}
	// ׯ
	else if ( cbPlayerCount < cbBankerCount)  
	{
		pWinArea[AREA_ZHUANG] = TRUE;

		//�����ж�
		if ( cbBankerCount == 8 || cbBankerCount == 9 )
		{
			pWinArea[AREA_ZHUANG_TIAN] = TRUE;
		}
	}
	// ��
	else 
	{
		pWinArea[AREA_XIAN] = TRUE;

		//�����ж�
		if ( cbPlayerCount == 8 || cbPlayerCount == 9 )
		{
			pWinArea[AREA_XIAN_TIAN] = TRUE;
		}
	}


	//�����ж�
	if (m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][0]) == m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][1]))
	{
		pWinArea[AREA_XIAN_DUI] = TRUE;
	}
	if (m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][0]) == m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][1]))
	{
		pWinArea[AREA_TIANGANG] = TRUE;
	}
}

//�ƶ�Ӯ��
void CGameClientView::DeduceWinner(BYTE &cbWinner, BYTE &cbKingWinner, bool &bPlayerTwoPair, bool &bBankerTwoPair)
{
	cbWinner = 0;
	cbKingWinner = 0;

	//�����Ƶ�
	BYTE cbPlayerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	BYTE cbBankerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//ʤ�����
	if (cbPlayerCount == cbBankerCount)
	{
		cbWinner=AREA_PING;

		//ͬ��ƽ�ж�
		bool bAllPointSame = false;
		if ( m_cbCardCount[INDEX_PLAYER] == m_cbCardCount[INDEX_BANKER] )
		{
			for (WORD wCardIndex = 0; wCardIndex < m_cbCardCount[INDEX_PLAYER]; ++wCardIndex )
			{
				BYTE cbBankerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][wCardIndex]);
				BYTE cbPlayerValue = m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][wCardIndex]);
				if ( cbBankerValue != cbPlayerValue ) break;
			}
			if ( wCardIndex == m_cbCardCount[INDEX_PLAYER] ) bAllPointSame = true;
		}
		if ( bAllPointSame ) cbKingWinner = AREA_TONG_DUI;
	}
	else if (cbPlayerCount<cbBankerCount) 
	{
		cbWinner=AREA_ZHUANG;

		//�����ж�
		if ( cbBankerCount == 8 || cbBankerCount == 9 ) cbKingWinner = AREA_ZHUANG_TIAN;
	}
	else 
	{
		cbWinner=AREA_XIAN;

		//�����ж�
		if ( cbPlayerCount == 8 || cbPlayerCount == 9 ) cbKingWinner = AREA_XIAN_TIAN;
	}

	//�����ж�
	bPlayerTwoPair=false;
	bBankerTwoPair=false;
	if (m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][0]) == m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_PLAYER][1]))
		bPlayerTwoPair=true;
	if (m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][0]) == m_GameLogic.GetCardValue(m_cbTableCardArray[INDEX_BANKER][1]))
		bBankerTwoPair=true;
}

//������ʾ
void CGameClientView::SetDispatchCardTips()
{
	if (m_CardControl[INDEX_PLAYER].GetCardCount() + m_CardControl[INDEX_BANKER].GetCardCount() < 4) 
		return;

	//if ( m_CardControl[INDEX_PLAYER].GetCardCount() == m_cbCardCount[INDEX_PLAYER] && m_CardControl[INDEX_BANKER].GetCardCount() == m_cbCardCount[INDEX_BANKER])
	//{
	//	//�������
	//	BYTE cbBankerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);
	//	BYTE cbPlayerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);

	//	CString strTips;
	//	if ( cbPlayerCount > cbBankerCount)
	//	{
	//		strTips.Format(TEXT("��%d�㣬ׯ%d�㣬��Ӯ��\n"), cbPlayerCount, cbBankerCount);
	//		m_strDispatchCardTips += strTips;
	//	}
	//	else if ( cbPlayerCount < cbBankerCount)
	//	{
	//		strTips.Format(TEXT("��%d�㣬ׯ%d�㣬ׯӮ��\n"), cbPlayerCount, cbBankerCount);
	//		m_strDispatchCardTips += strTips;
	//	}
	//	if ( cbPlayerCount == cbBankerCount)
	//	{
	//		strTips.Format(TEXT("��%d�㣬ׯ%d�㣬ƽ��\n"), cbPlayerCount, cbBankerCount);
	//		m_strDispatchCardTips += strTips;
	//	}
	//	return;
	//}


	//�������
	BYTE cbBankerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],2);
	BYTE cbPlayerTwoCardCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],2);

	//�мҲ���	
	if( cbPlayerTwoCardCount <= 5 && cbBankerCount < 8 
		&& m_CardControl[INDEX_PLAYER].GetCardCount() + m_CardControl[INDEX_BANKER].GetCardCount() == 4)
	{		
		CString strTips;
		strTips.Format(TEXT("��%d�㣬ׯ%d�㣬�м�������\n"), cbPlayerTwoCardCount, cbBankerCount);
		m_strDispatchCardTips = strTips;
		return;
	}

	if ( m_CardControl[INDEX_BANKER].GetCardCount() == 3 ) return;

	BYTE cbPlayerThirdCardValue = 0 ;	//�������Ƶ���

	//�������
	cbPlayerThirdCardValue = m_GameLogic.GetCardPip(m_cbTableCardArray[INDEX_PLAYER][2]);

	//ׯ�Ҳ���
	CString strTips;
	if( cbPlayerTwoCardCount < 8 && cbBankerCount < 8 )
	{
		switch(cbBankerCount)
		{
		case 0:
		case 1:
		case 2:
			{
				strTips.Format(TEXT("��ǰ������%d�㣬ׯ%d�㣬ׯ��������\n"), cbPlayerTwoCardCount, cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			break;
		case 3:
			if(m_cbCardCount[INDEX_PLAYER] == 3 && cbPlayerThirdCardValue != 8)
			{
				strTips.Format(TEXT("�е�������%d�㣬ׯ%d�㣬ׯ��������\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			else if (m_cbCardCount[INDEX_PLAYER] == 2) 
			{
				strTips.Format(TEXT("�в����ƣ�ׯ%d�㣬ׯ��������\n"), cbBankerCount);
				m_strDispatchCardTips += strTips;
			}			
			break;
		case 4:
			if(m_cbCardCount[INDEX_PLAYER] == 3 && cbPlayerThirdCardValue != 1 && cbPlayerThirdCardValue != 8 && cbPlayerThirdCardValue != 9 && cbPlayerThirdCardValue != 0)
			{
				strTips.Format(TEXT("�е�������%d�㣬ׯ%d�㣬ׯ��������\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			else if ( m_cbCardCount[INDEX_PLAYER] == 2) 
			{
				strTips.Format(TEXT("�в����ƣ�ׯ%d�㣬ׯ��������\n"), cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			break;
		case 5:
			if( m_cbCardCount[INDEX_PLAYER] == 3 && cbPlayerThirdCardValue != 1 && cbPlayerThirdCardValue != 2 && cbPlayerThirdCardValue != 3  && cbPlayerThirdCardValue != 8 && cbPlayerThirdCardValue != 9 &&  cbPlayerThirdCardValue != 0)
			{
				strTips.Format(TEXT("�е�������%d�㣬ׯ%d�㣬ׯ��������\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			else if ( m_cbCardCount[INDEX_PLAYER] == 2 ) 
			{
				strTips.Format(TEXT("�в����ƣ�ׯ%d�㣬ׯ��������\n"), cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			break;
		case 6:
			if( m_cbCardCount[INDEX_PLAYER] == 3 && ( cbPlayerThirdCardValue == 6 || cbPlayerThirdCardValue == 7 ))
			{
				strTips.Format(TEXT("�е�������%d�㣬ׯ%d�㣬ׯ��������\n"), cbPlayerThirdCardValue, cbBankerCount);
				m_strDispatchCardTips += strTips;
			}
			break;
			//���벹��
		case 7:
		case 8:
		case 9:
			break;
		default:
			break;
		}
	}

	return ;
}

// ��Ӷ���
CString CGameClientView::AddComma( LONGLONG lScore , bool bPlus /*= false*/)
{
	CString strScore;
	CString strReturn;
	LONGLONG lNumber = lScore;
	if ( lScore < 0 )
		lNumber = -lNumber;

	strScore.Format(TEXT("%I64d"), lNumber);

	int nStrCount = 0;
	for( int i = strScore.GetLength() - 1; i >= 0; )
	{
		if( (nStrCount%3) == 0 && nStrCount != 0 )
		{
			strReturn.Insert(0, ',');
			nStrCount = 0;
		}
		else
		{
			strReturn.Insert(0, strScore.GetAt(i));
			nStrCount++;
			i--;
		}
	}

	if ( lScore < 0 )
		strReturn.Insert(0, '-');

	if ( bPlus && lScore > 0 )
		strReturn.Insert(0, '+');

	return strReturn;
}

// ɾ������
LONGLONG CGameClientView::DeleteComma( CString strScore )
{
	LONGLONG lScore = 0l;
	strScore.Remove(',');
	if ( !strScore.IsEmpty() )
		lScore = _ttoi64(strScore);

	return lScore;
}

//��������
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//��������
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};


	//��������
	UINT uAlign = pDC->GetTextAlign();
	UINT nDrawFormat = 0;
	if ( uAlign&TA_CENTER )
		nDrawFormat |= DT_CENTER;
	else if( uAlign&TA_RIGHT )
		nDrawFormat |= DT_RIGHT;
	else
		nDrawFormat |= DT_LEFT;

	if( uAlign&TA_BOTTOM )
		nDrawFormat |= DT_BOTTOM;
	else 
		nDrawFormat |= DT_TOP;

	//�滭�߿�
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		//�滭�ַ�
		CDFontEx::DrawText(this,pDC,  12, 400, pszString, nXPos+nXExcursion[i],nYPos+nYExcursion[i], crFrame, nDrawFormat);
	}
	//�滭�ַ�
	CDFontEx::DrawText(this,pDC,  12, 400, pszString, nXPos,nYPos, crText, nDrawFormat);

}

void CGameClientView::DrawTextString( CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat )
{
	//��������
	INT nStringLength=lstrlen(pszString);
	INT nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	INT nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//�滭�߿�
	for (INT i=0;i<CountArray(nXExcursion);i++)
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
}

//�滭�ַ�
void CGameClientView::DrawTextString( CDC * pDC, CDFontEx* pFont, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos )
{
	//��������
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};


	//��������
	UINT uAlign = pDC->GetTextAlign();
	UINT nDrawFormat = 0;
	if ( uAlign&TA_CENTER )
		nDrawFormat |= DT_CENTER;
	else if( uAlign&TA_RIGHT )
		nDrawFormat |= DT_RIGHT;
	else
		nDrawFormat |= DT_LEFT;

	if( uAlign&TA_BOTTOM )
		nDrawFormat |= DT_BOTTOM;
	else 
		nDrawFormat |= DT_TOP;

	//�滭�߿�
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		//�滭�ַ�
		pFont->DrawText(pDC, pszString, nXPos+nXExcursion[i],nYPos+nYExcursion[i], crFrame, nDrawFormat);
	}

	//�滭�ַ�
	pFont->DrawText( pDC, pszString, nXPos,nYPos, crText, nDrawFormat);
}

//�滭�ַ�
void CGameClientView::DrawTextString( CDC * pDC, CDFontEx* pFont, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat )
{
	//��������
	INT nStringLength=lstrlen(pszString);
	INT nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	INT nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//�滭�߿�
	for (INT i=0;i<CountArray(nXExcursion);i++)
	{
		//����λ��
		CRect rcFrame;
		rcFrame.top=rcRect.top+nYExcursion[i];
		rcFrame.left=rcRect.left+nXExcursion[i];
		rcFrame.right=rcRect.right+nXExcursion[i];
		rcFrame.bottom=rcRect.bottom+nYExcursion[i];

		//�滭�ַ�
		pFont->DrawText( pDC, pszString, rcFrame, crFrame, nDrawFormat);
	}

	//�滭�ַ�
	pFont->DrawText( pDC, pszString, rcRect, crText, nDrawFormat);
}

// �滭����
void CGameClientView::DrawNumber( CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, LONGLONG lOutNum,INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),TEXT("%I64d"),lOutNum);
	DrawNumber(pDC, ImageNumber, szImageNum, szOutNum, nXPos, nYPos, uFormat);
}

// �滭����
void CGameClientView::DrawNumber( CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
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

	if ( uFormat == DT_CENTER )
	{
		nXPos -= (INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth);
	}
	else if ( uFormat == DT_RIGHT )
	{
		nXPos -= lstrlen(szOutNum) * nNumberWidth;
	}

	for ( INT i = 0; i < lstrlen(szOutNum); ++i )
	{
		for ( INT j = 0; j < lstrlen(szImageNum); ++j )
		{
			if ( szOutNum[i] == szImageNum[j] && szOutNum[i] != '\0' )
			{
				ImageNumber->DrawImage(pDC, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}


// �滭ʱ��
void CGameClientView::DrawTime(CDC * pDC, WORD wUserTime,INT nXPos, INT nYPos)
{
	//��������
	m_ImageTimeBack.DrawImage( pDC, nXPos, nYPos  );

	TCHAR szOutNum[128] = {0};
	if ( wUserTime > 99 )
		_sntprintf(szOutNum,CountArray(szOutNum),TEXT("99"));
	else if ( wUserTime < 10 )
		_sntprintf(szOutNum,CountArray(szOutNum),TEXT("0%d"), wUserTime);
	else
		_sntprintf(szOutNum,CountArray(szOutNum),TEXT("%d"), wUserTime);

	//��������
	DrawNumber( pDC, &m_ImageTimeNumber, TEXT("0123456789"), szOutNum, nXPos + m_ImageTimeBack.GetWidth()/2, nYPos +20, DT_CENTER);
}

//��������
void CGameClientView::FlexAnimation( enFlexMode nFlexMode, bool bShow , bool bMove /*= true*/)
{
	//��ֵ����
	m_enFlexMode = nFlexMode;

	// ָ��
	if ( nFlexMode == enFlexNULL )
	{
		m_pImageFlex = NULL;
		KillTimer(IDI_FLEX_MOVE);
		//InvalidGameView(0,0,0,0);
		return;
	}
	else if ( nFlexMode == enFlexBetTip )
	{
		m_pImageFlex = &m_ImageBetTip;
	}
	else if ( nFlexMode == enFlexDealCrad )
	{
		m_pImageFlex = &m_ImageDealBack;
	}
	else if ( nFlexMode == enFlexGameEnd )
	{
		m_pImageFlex = &m_ImageGameEnd;
	}

	//  �޶��� ������ʾ
	if ( !bMove )
	{
		if ( !bShow )
		{
			m_pImageFlex = NULL;
			m_bFlexShow = false;
			m_nFlexMove = 0;
		}
		else
		{
			m_bFlexShow = true;
			m_nFlexMove = FLEX_MOVE_COUNT;
			m_ptFlexMove = m_ptFlexBeing;
			m_ptFlexMove.y += 137;
		}
		return;
	}

	// ״̬һ������ʾ
	if( m_bFlexShow == bShow)
		return;

	// ��������
	m_bFlexShow = bShow;
	SetTimer( IDI_FLEX_MOVE, 30, NULL );
	return;
}

//��˸����
void CGameClientView::FlashAnimation( bool bBegin )
{
	if ( !bBegin )
	{
		m_ArrayFlashArea.RemoveAll();
		m_nFlashAreaAlpha = 0;
		m_bFlashAreaAlpha = false;
		KillTimer(IDI_FLASH_WINNER);
		return;
	}

	//�ƶ����
	BYTE cbWinArea[AREA_MAX] = {FALSE};
	DeduceWinner(cbWinArea);

	for ( int i = 0 ; i < AREA_MAX; ++i )
	{
		if ( cbWinArea[i] == TRUE )
		{
			m_ArrayFlashArea.Add(i);
		}
	}
	m_nFlashAreaAlpha = 0;
	m_bFlashAreaAlpha = TRUE;
	SetTimer( IDI_FLASH_WINNER, 700, NULL );
}

//���ñ���
LRESULT CGameClientView::OnSetCaption(WPARAM wParam, LPARAM lParam)
{
	m_strCaption = (LPCTSTR)lParam;
	CRect rcCaption(5,0,1024,30);
	CDC* pDC = GetDC();
	
	return 0;
}

//�ؼ�����
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//��ȡID
	WORD wControlID=LOWORD(wParam);
	//�ؼ��ж�
	switch (wControlID)
	{
	case IDC_SOUND:
		{
			SendEngineMessage(IDM_GAME_SOUND,0,0);
			break;
		}
	case IDC_SEND:
		{
			CString str, mes;
			GetDlgItem(IDC_CHAT_INPUT)->GetWindowText(str);
			if (str.GetLength() < 1) return TRUE;

			//У������
			if(EfficacyUserChat(str,INVALID_WORD) == false)
				return TRUE;

			//��ѯ�ӿ�
			IClientKernel * m_pIClientKernel;
			CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
			ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
			m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
			m_pIClientKernel->SendUserChatMessage(0, str, RGB(0,0,0));

			GetDlgItem(IDC_CHAT_INPUT)->SetWindowText(TEXT(""));
			GetDlgItem(IDC_CHAT_INPUT)->SetFocus();
			return TRUE;
			break;
		}
	case IDC_MAX:
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_MAX,0);
			m_btRestore.ShowWindow(SW_SHOW);
			m_btMax.ShowWindow(SW_HIDE);

			INT nMaxCol = ZOOMED_RECORD_COL_COUNT;
			if(m_nTotalCol > nMaxCol) 
			{
				m_nHeadCol = m_nTotalCol - (nMaxCol-1);
				m_btScoreMoveL.EnableWindow(TRUE);
			}

			break;
		}
	case IDC_RESTORE:
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_RESTORE,0);
			m_btRestore.ShowWindow(SW_HIDE);
			m_btMax.ShowWindow(SW_SHOW);
			
			INT nMaxCol = NORMAL_RECORD_COL_COUNT;
			if(m_nTotalCol > nMaxCol) 
			{
				m_nHeadCol = m_nTotalCol - (nMaxCol-1);
				m_btScoreMoveL.EnableWindow(TRUE);
			}

			break;

		}
	}
	return CGameFrameViewGDI::OnCommand(wParam, lParam);
}

//�ؼ���ɫ
HBRUSH CGameClientView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	if(pWnd->GetSafeHwnd() == GetDlgItem(IDC_CHAT_INPUT)->GetSafeHwnd())
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(213,166,107));
		return m_brush; 
	}
	return hbr;
}

//��Ϣ����
BOOL CGameClientView::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg-> wParam == VK_RETURN)
		{
			CString str;
			GetDlgItem(IDC_CHAT_INPUT)->GetWindowText(str);
			if (str.GetLength() > 0)
			{
				SendMessage(WM_COMMAND,IDC_SEND,0);
			}
			else
			{
				GetDlgItem(IDC_CHAT_INPUT)->SetFocus();
			}
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

//���°�ť
VOID CGameClientView::UpdateButtonContron()
{
	bool bEnableDown = false;
	bool bEnableUp = false;
	INT nUserCount = m_ValleysList.GetCount();

	if(nUserCount <= 4)
	{
		m_nShowValleyIndex = 0;
		bEnableDown = false;
		bEnableUp = false;
	}
	else
	{
		bEnableDown = ((m_nShowValleyIndex + 4) < nUserCount)?true:false;
		bEnableUp = (m_nShowValleyIndex > 0)?true:false;
	}

	m_btValleysDown.EnableWindow(bEnableDown);
	m_btValleysUp.EnableWindow(bEnableUp);
	return;
}

//������ͼ
void CGameClientView::RefreshGameView()
{
	CRect rect;
	GetClientRect(&rect);
	InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());

	return;
}


void CGameClientView::OnSize( UINT nType, int cx, int cy )
{
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;
	DeferWindowPos(hDwp,m_btBet1000,NULL,197-3,470-4,0,0,uFlags|SWP_NOSIZE);
	m_btBet1000.MoveWindow(100,100,300,300,TRUE);
}

void CGameClientView::OnLButtonDblClk(UINT nFlags,CPoint point)
{
	int cx = GetSystemMetrics(SM_CXSCREEN);
	CRect rc(0,0,cx,30);
	//if(rc.PtInRect(point))
	//{
	//	m_btMax.IsWindowVisible() ? (AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_MAX,0)),m_btMax.ShowWindow(SW_HIDE),m_btRestore.ShowWindow(SW_SHOW)
	//		:(AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_RESTORE,0)),m_btRestore.ShowWindow(SW_HIDE),m_btMax.ShowWindow(SW_SHOW);

	//	m_btRestore.IsWindowVisible() ? (AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_RESTORE,0),m_btMax.ShowWindow(SW_SHOW),m_btRestore.ShowWindow(SW_HIDE))
	//		:AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_MAX,0),m_btMax.ShowWindow(SW_HIDE),m_btRestore.ShowWindow(SW_SHOW);
	//}

	
	if(rc.PtInRect(point) && m_btMax.IsWindowVisible())
	{
		AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_MAX,0);
		m_btMax.ShowWindow(SW_HIDE);
		m_btRestore.ShowWindow(SW_SHOW);

		INT nMaxCol = ZOOMED_RECORD_COL_COUNT;
		if(m_nTotalCol > nMaxCol) 
		{
			m_nHeadCol = m_nTotalCol - (nMaxCol-1);
			m_btScoreMoveL.EnableWindow(TRUE);
		}

		return;
	}
	else if(rc.PtInRect(point) && m_btRestore.IsWindowVisible())
	{
		AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_RESTORE,0);
		m_btMax.ShowWindow(SW_SHOW);
		m_btRestore.ShowWindow(SW_HIDE);

		INT nMaxCol = NORMAL_RECORD_COL_COUNT;
		if(m_nTotalCol > nMaxCol) 
		{
			m_nHeadCol = m_nTotalCol - (nMaxCol-1);
			m_btScoreMoveL.EnableWindow(TRUE);
		}

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
	//INT nGap = GetSystemMetrics(SM_CXSCREEN) - (CalZoomedRecordCount()-4)*m_ImageHistoryMid.GetWidth() -
	//	m_ImageUserBack.GetWidth()-m_ImageChatBack.GetWidth() - 10-m_ImageHistoryFront.GetWidth()-m_ImageHistoryBack.GetWidth();

	INT nGap = GetSystemMetrics(SM_CXSCREEN) - (ZOOMED_RECORD_COL_COUNT - 4)*m_ImageHistoryMid.GetWidth() -
	m_ImageUserBack.GetWidth()-m_ImageChatBack.GetWidth() - 10 - m_ImageHistoryFront.GetWidth() - m_ImageHistoryBack.GetWidth();
	return nGap/2;
}

INT CGameClientView::CalLastRecordHorCount()
{	
	bool tmpFillTrace[6][MAX_RECORD_SUM];
	ZeroMemory(tmpFillTrace,sizeof(tmpFillTrace));
	int nVerCount = 0, nHorCount = 0;
	BYTE cbPreWinSide = 0, cbCurWinSide = 0;
	for(int rightIndex = 0;rightIndex < m_RightGameRecordCount;rightIndex++)
	{
		tagClientGameRecord &ClientGameRecord = m_RightGameRecordArray[rightIndex];
		if (ClientGameRecord.cbPlayerCount > ClientGameRecord.cbBankerCount) cbCurWinSide = AREA_XIAN + 1;
		else if (ClientGameRecord.cbPlayerCount < ClientGameRecord.cbBankerCount) cbCurWinSide = AREA_ZHUANG + 1;
		else cbCurWinSide = AREA_PING + 1;
		//������Ŀ
		if ( cbPreWinSide != 0 && ( cbPreWinSide == cbCurWinSide || cbCurWinSide == AREA_PING + 1 ))
		{
			nVerCount++;
			if (tmpFillTrace[nVerCount][nHorCount]==true || nVerCount==6)
			{
				nVerCount--;
				nHorCount++;

			}
		}
		else
		{
			nVerCount=0;
			if ( cbPreWinSide != 0 )
			{
				for (int i=0; i < MAX_RECORD_SUM; ++i)
				{
					if (tmpFillTrace[0][i]==false)
					{
						nHorCount=i;
						break;
					}
				}				
			}
			cbPreWinSide=cbCurWinSide;
		}
		tmpFillTrace[nVerCount][nHorCount]=true;		
	}
	return nHorCount+1;
}

//////////////////////////////////////////////////////////////////////////
