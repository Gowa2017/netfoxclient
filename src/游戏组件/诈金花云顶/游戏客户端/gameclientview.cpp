#include "StdAfx.h"
#include "Resource.h"
#include "GameClientDlg.h"
#include "GameClientView.h"
#include ".\gameclientview.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

//��ť��ʶ
#define IDC_START						100								//��ʼ��ť
#define IDC_MAX_SCORE					104								//���ť
#define IDC_MIN_SCORE					105								//���ٰ�ť
#define IDC_CONFIRM						106								//ȷ����ť
#define	IDC_CANCEL_ADD					109								//ȡ����ť
#define IDC_ADD_SCORE					110								//��ע��ť
#define IDC_LOOK_CARD					111								//������Ϣ
#define IDC_COMPARE_CARD				112								//������Ϣ
#define IDC_FOLLOW						113								//������Ϣ
#define IDC_GIVE_UP						114								//������Ϣ
#define	IDC_OPEN_CARD					115								//������Ϣ
#define IDC_CLOSE						116								//�رհ�ť
#define	IDC_MIN							117								//��С��ť	
#define IDC_JETTON_1					118								//��ע��ť
#define IDC_JETTON_2					119								//��ע��ť
#define IDC_JETTON_5					120								//��ע��ť
#define	IDC_SOUND						121								//������ť
#define IDC_SEND						122								//���Ͱ�ť
#define IDC_CHAT_INPUT					123								//�ؼ���ʶ
#define IDC_CHAT_DISPLAY				124								//�ؼ���ʶ
#define IDC_HISTORY_CHAT				125								//�ؼ���ʶ
#define IDC_OPTION						126								//�ؼ���ʶ
#define IDC_ADMIN						127									//��ť��ʶ

//��ʱ����ʶ
#define IDI_CARTOON						99								//������ʱ��
#define	TIME_CARTOON					100								//����ʱ��
#define IDI_MOVE_JETTON					101								//�ƶ����붨ʱ��
#define IDI_MOVE_NUMBER					102								//��������
#define IDI_CLOSE_EYE					103								//���۶�ʱ 
#define IDI_OPEN_EYE					120								//���۶�ʱ

//�����ٶ�  //(SPEED)10 (TIME_CARTOON)70 /CPU UP
#define	SPEED							7								//�����ٶȲ���
#define	ROCK							10								//�񶯸���
#define TIME_MOVE_JETTON				20								//�ƶ�����ʱ��
#define TIME_MOVE_NUMBER				50								//��������ʱ��

//λ�ñ���
#define	MY_SELF_CHAIRID					2								//�Լ�λ��

//���Ʊ���
#define VSCARD_TIME						3								//���ƴ���
#define VSCARD_COUNT_X					7								//������Ŀ
#define VSCARD_COUNT_Y					3								//������Ŀ
#define COMPARE_RADII					115								//���ƿ��

//���Ʊ���
#define SEND_PELS						30								//��������
#define LESS_SEND_COUNT					2								//���ʹ���

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)							  
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_SET_CAPTION, OnSetCaption)
	ON_BN_CLICKED(IDC_ADMIN,OpenAdminWnd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientView::CGameClientView()
{
	//��������
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_lDrawGold[i].RemoveAll();
		m_ptKeepJeton[i].RemoveAll();
		m_lStopUpdataScore[i] = 0;
		m_wFlashUser[i]=INVALID_CHAIR;	
	}
	m_bCartoon =false;
	m_SendCardPos.SetPoint(0,0);
	m_SendCardItemArray.RemoveAll();

	//���Ʊ���
	m_wConmareIndex=0;

	m_wConmareCount=0;
	m_wLoserUser=INVALID_CHAIR;
	ZeroMemory(m_wCompareChairID,sizeof(m_wCompareChairID));

	//���ݱ���
	ZeroMemory(m_tcBuffer,sizeof(m_tcBuffer));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_bCompareUser,sizeof(m_bCompareUser));
	m_wBankerUser=INVALID_CHAIR;
	m_lMaxCellScore=0;
	m_lCellScore=0;	
	m_wFalshCount=0;
	m_bStopDraw=false;
	m_bFalsh=false;
	m_bInvest=false;		
	m_bShow=false;
	m_bCompareCard=false;
	m_wWaitUserChoice=INVALID_CHAIR;
	m_bCheatUser = false;
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	m_bShowControlBack = false;
	m_bShowChatMessage=true;
	m_bChangSence = false;
	m_nWidth = 0;
	m_nHeight = 0;
	ZeroMemory(m_bCloseEye,sizeof(m_bCloseEye));
	ZeroMemory(m_cbWink,sizeof(m_cbWink));
	BYTE tmp[GAME_PLAYER+1] = {0,1,2,3,4,5};
	BYTE bPosition =0,bRandCount =0;
	do
	{
		srand(time(NULL));
		INT nrand = rand();
		bPosition=nrand%(GAME_PLAYER+1-bRandCount);
		m_cbUserIndex[bRandCount++]=tmp[bPosition];
		tmp[bPosition]=tmp[GAME_PLAYER+1-bRandCount];
	} while (bRandCount<GAME_PLAYER);
	m_nXFace=48;
	m_nYFace=48;
	m_nXTimer=65;
	m_nYTimer=69;
	m_strCaption = TEXT("");
	m_dwChatTime = 0;
	
	m_pClientControlDlg = NULL;
	m_hInst = NULL;

	return;
}

//��������
CGameClientView::~CGameClientView()
{
	if( m_pClientControlDlg )
	{
		delete m_pClientControlDlg;
		m_pClientControlDlg = NULL;
	}

	if( m_hInst )
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}

//������Ϣ
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//�����ؼ�
	CRect rcCreate(0,0,0,0);
	m_ScoreView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,this,10);

	//������ť
	m_btStart.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_START);
	m_btCompareCard.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_COMPARE_CARD);
	m_btGiveUp.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_GIVE_UP);
	m_btLookCard.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_LOOK_CARD);
	m_btFollow.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_FOLLOW);

	m_btClose.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_CLOSE);
	m_btSound.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_SOUND);
	m_btMin.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_MIN);
	m_btOption.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_OPTION);

	m_btJetton_1.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_JETTON_1);
	m_btJetton_2.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_JETTON_2);
	m_btJetton_5.Create(TEXT(""),WS_CHILD,rcCreate,this,IDC_JETTON_5);

	m_btSend.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SEND);
	m_btHistoryChat.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_HISTORY_CHAT);

	//����λͼ
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_START,hInstance,false,false);
	m_btCompareCard.SetButtonImage(IDB_COMPARE_CARD,hInstance,false,false);
	m_btGiveUp.SetButtonImage(IDB_GIVE_UP,hInstance,false,false);
	m_btLookCard.SetButtonImage(IDB_LO0K_CARD,hInstance,false,false);
	m_btFollow.SetButtonImage(IDB_FOLLOW,hInstance,false,false);

	m_btClose.SetButtonImage(IDB_CLOSE,hInstance,false,false);
	m_btMin.SetButtonImage(IDB_MIN,hInstance,false,false);
	m_btSound.SetButtonImage(IDB_OPEN_SOUND,hInstance,false,false);
	m_btOption.SetButtonImage(IDB_OPTION,hInstance,false,false);

	m_btJetton_1.SetButtonImage(IDB_10,hInstance,false,false);
	m_btJetton_2.SetButtonImage(IDB_20,hInstance,false,false);
	m_btJetton_5.SetButtonImage(IDB_50,hInstance,false,false);

	m_btSend.SetButtonImage(IDB_SEND,hInstance,false,false);
	m_btHistoryChat.SetButtonImage(IDB_BT_HIDE_CHAT, AfxGetInstanceHandle(), false, true);

	//����λͼ
	
	m_ImageCard.LoadImage(AfxGetInstanceHandle(),TEXT("SMALL_CARD"));
	m_ImageComPareCard.LoadImage(AfxGetInstanceHandle(),TEXT("CARD"));
	m_ImageBanker.LoadFromResource(hInstance,IDB_BANKER_FLAG);
	m_ImageNumber.LoadFromResource(hInstance,IDB_NUMBER);

	//������Դ
	m_PngImageViewBack_1.LoadImage(hInstance,TEXT("VIEW_BACK_1"));
	m_PngImageViewBack_2.LoadImage(hInstance,TEXT("VIEW_BACK_2"));
	m_PngImageTable.LoadImage(hInstance,TEXT("TABLE"));
	m_PngImagePlayerFrame.LoadImage(hInstance,TEXT("PLAYER_FRAME"));
	m_PngImageJetonBack.LoadImage(hInstance,TEXT("JETTON_BACK"));
	m_PngImageFrameBack.LoadImage(hInstance,TEXT("FRAME_BACK"));
	m_PngFengding.LoadImage(hInstance,TEXT("FENGDING"));
	m_PngImageTimeBack.LoadImage(hInstance,TEXT("CLOCK_BACK"));
	m_PngImageTimeNumber.LoadImage(hInstance,TEXT("CLOCK_NUMBER"));
	m_PngaImageXZNumber.LoadImage(hInstance,TEXT("XZ_NUMBER"));
	m_PngImageFDNumber.LoadImage(hInstance,TEXT("FD_NUMBER"));
	m_PngImageChatBack.LoadImage(hInstance,TEXT("CHAT_BACK"));
	m_PngImageMyself.LoadImage(hInstance,TEXT("MYSELF_NUM"));
	m_PngImageArrow.LoadImage(hInstance,TEXT("ARROW"));
	m_PngSelectPlayer.LoadImage(hInstance,TEXT("SELECT_PLAYER"));
	m_PngWaitSelect.LoadImage(hInstance,TEXT("WAIT_SELECT"));
	m_PngImageFrameBackBottom.LoadImage(hInstance,TEXT("FRAME_BACK_BOTTOM"));

	//�����ؼ�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_CardControl[i].Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcCreate,this,20+i);
		m_CardControl[i].SetCommandID(i,IDM_COMPARE_USER);

		//test
		if(i == 2)
			m_CardControl[i].SetSmallCard(false);
		else
			m_CardControl[i].SetSmallCard(true);		
	}

	//��������
	m_DFontEx.CreateFont(this, TEXT("����"), 12, 400 );

	//����ؼ�
	m_ChatInput.Create(ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER, rcCreate, this, IDC_CHAT_INPUT);
	//m_ChatDisplay.Create(ES_AUTOVSCROLL|ES_WANTRETURN|ES_MULTILINE|WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_VSCROLL, rcCreate, this, IDC_CHAT_DISPLAY);
	m_ChatMessage.Create(ES_AUTOVSCROLL|ES_WANTRETURN|ES_MULTILINE|WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_VSCROLL, rcCreate, this, IDC_CHAT_DISPLAY);

	//�ؼ�����
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));      
	lf.lfHeight = 15;
	wcscpy(lf.lfFaceName, TEXT("Arial"));
	m_ChatInput.SetLimitText(30);
// 	m_ChatDisplay.SetTextFont(lf);
// 	m_ChatDisplay.SetReadOnly(TRUE);
// 	m_ChatDisplay.SetTextColor(RGB(255,255,255));
// 	m_ChatDisplay.SetBackgroundColor(false,RGB(33,16,7));
	m_ChatMessage.SetBackgroundColor(false,RGB(33,16,7));
	m_ChatMessage.SetExpressionManager(CExpressionManager::GetInstance(),RGB(33,16,7));
	m_ChatMessage.SetReadOnly(TRUE);
	m_brush.CreateSolidBrush(RGB(255,0,255));
	/*m_ChatDisplay.SetHideWindow(false);*/

	IClientKernel * m_pIClientKernel;
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
	//��ѯ�ӿ�
	ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
	m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
	m_pIClientKernel->SetStringMessage(QUERY_OBJECT_INTERFACE(m_ChatMessage,IStringMessage));

	CFont m_font;
	m_font.CreateFontIndirect(&lf);

	//�󶨿ؼ�
	m_ChatMessage.SetFont(&m_font);

#ifdef _DEBUG
	//���Դ���
	//BYTE cbCard[3] = {0x12,0x21,0x32};
	//for(int i = 0; i < GAME_PLAYER; ++i)
	//{
	//	m_CardControl[i].SetCardData(cbCard,3);
	//	m_NumberControl[i].SetScore( 5442342 );
	//	m_lTableScore[i] = 423424;
	//	LoadUserImage(i,0);
	//}
	//m_JettonControl.AddScore(52100,m_ptJettons[1]);
	//BeginMoveJettons();
	//BeginMoveNumber();
#endif // _DEBUG
	
	//����
	m_hInst = NULL;
	m_pClientControlDlg = NULL;
	m_hInst = LoadLibrary(TEXT("ZaJinHuaClientControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(CWnd* pParentWnd); 
		CREATE ClientControl = (CREATE)GetProcAddress(m_hInst,"CreateClientControl"); 
		if ( ClientControl )
		{
			m_pClientControlDlg = static_cast<IClientControlDlg*>(ClientControl(this));
			m_pClientControlDlg->ShowWindow( SW_HIDE );
		}
	}
	
	//���ư�ť
	m_btOpenAdmin.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(4, 4, 11, 11), this, IDC_ADMIN);
	m_btOpenAdmin.ShowWindow(SW_HIDE);

	return 0;
}

//�����
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDC_START:				//��ʼ��ť
		{
			//������Ϣ
			SendEngineMessage(IDM_START,0,0);
			return TRUE;
		}
	case IDC_ADD_SCORE:			//��ע��ť
		{
			//������Ϣ
			SendEngineMessage(IDM_ADD_SCORE,0,0);
			return TRUE;
		}
	case IDC_CONFIRM:			//ȷ����ť
		{
			//������Ϣ
			SendEngineMessage(IDM_CONFIRM,0,0);
			return TRUE;
		}
	case IDC_CANCEL_ADD:		//ȡ����ť
		{
			//������Ϣ
			SendEngineMessage(IDM_CANCEL_ADD,0,0);
			return TRUE;
		}
	case IDC_FOLLOW:			//��ע��ť
		{
			//������Ϣ
			SendEngineMessage(IDM_CONFIRM,1,1);
			return TRUE;
		}
	case IDC_MIN_SCORE:			//���ٰ�ť
		{
			//������Ϣ
			SendEngineMessage(IDM_MIN_SCORE,0,0);
			return TRUE;
		}
	case IDC_MAX_SCORE:			//���ť
		{
			//������Ϣ
			SendEngineMessage(IDM_MAX_SCORE,0,0);
			return TRUE;
		}
	case IDC_COMPARE_CARD:		//���ư�ť
		{
			//������Ϣ
			SendEngineMessage(IDM_COMPARE_CARD,0,0);
			return TRUE;
		}
	case IDC_LOOK_CARD:			//���ư�ť
		{
			//������Ϣ
			SendEngineMessage(IDM_LOOK_CARD,0,0);
			return TRUE;
		}
	case IDC_OPEN_CARD:			//������Ϣ
		{
			//������Ϣ
			SendEngineMessage(IDM_OPEN_CARD,0,0);
			return TRUE;
		}
	case IDC_GIVE_UP:			//������ť
		{
			//������Ϣ
			SendEngineMessage(IDM_GIVE_UP,0,0);
			return TRUE;
		}
	case IDC_MIN:				//��С��ť
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND, 100, 0);
			return TRUE;
		}
	case IDC_CLOSE:				//�رհ�ť
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND, 101, 0);
			//SendEngineMessage(IDM_GAME_CLOSE,0,0);
			return TRUE;
		}
	case IDC_SEND:
		{
			OnSendMessage();
			return TRUE;
		}
	case IDC_JETTON_1:
	case IDC_JETTON_2:
	case IDC_JETTON_5:
		{
			//������Ϣ
			SendEngineMessage(IDM_ADD_SCORE,LOWORD(wParam)-IDC_JETTON_1,0);
			return TRUE;
		}
	case IDC_HISTORY_CHAT:
		{
			OnHideChatMessage();

			//���½���
			InvalidGameView(0,0,0,0);

			return TRUE;
		}
	case IDC_SOUND:
		{
			SendEngineMessage(IDM_GAME_SOUND,0,0);
			return TRUE;
		}
	case IDC_OPTION:
		{
			//CDlgGameOption DlgGameOption;
			//if (DlgGameOption.CreateGameOption(NULL,0)==true)
			//{
			//	return TRUE;
			//}

			return TRUE;
		}
	}

	return __super::OnCommand(wParam, lParam);
}

//���ý���
VOID CGameClientView::ResetGameView()
{
	//ɾ��ʱ��
	KillTimer(IDI_CARTOON);
	KillTimer(IDI_MOVE_JETTON);
	KillTimer(IDI_MOVE_NUMBER);

	//��������
	m_bCartoon =false;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_lDrawGold[i].RemoveAll();
		m_ptKeepJeton[i].RemoveAll();
		m_wFlashUser[i]=INVALID_CHAIR;
		m_lStopUpdataScore[i] = 0;
	}
	m_SendCardPos.SetPoint(0,0);
	m_SendCardItemArray.RemoveAll();

	//���Ʊ���
	m_wConmareIndex = 0;
	m_wConmareCount = 0;
	m_wLoserUser=INVALID_CHAIR;
	ZeroMemory(m_wCompareChairID,sizeof(m_wCompareChairID));

	//���ݱ���
	ZeroMemory(m_tcBuffer,sizeof(m_tcBuffer));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	m_wBankerUser=INVALID_CHAIR;			
	ZeroMemory(m_bCompareUser,sizeof(m_bCompareUser));
	m_lMaxCellScore=0;					
	m_lCellScore=0;		
	m_wFalshCount=0;
	m_bStopDraw=false;
	m_bFalsh=false;
	m_bInvest=false;
	m_bShow=false;
	m_bCompareCard=false;
	m_wWaitUserChoice=INVALID_CHAIR;
	m_bCheatUser = false;
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	m_bShowControlBack = false;

	//����ť
	m_btStart.ShowWindow(SW_HIDE);
	m_btCompareCard.EnableWindow(FALSE);
	m_btGiveUp.EnableWindow(FALSE);
	m_btLookCard.EnableWindow(FALSE);
	m_btFollow.EnableWindow(FALSE);


	//���ؿؼ�
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_NumberControl[i].ResetControl();
	}
	m_JettonControl.ResetControl();
	m_ScoreView.ShowWindow(SW_HIDE);
	for (WORD i=0;i<GAME_PLAYER;i++) m_CardControl[i].SetCardData(NULL,0);

	return;
}

//�����ؼ�
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	//��������
	m_ptAvatar[4].x=nWidth/2+300;
	m_ptAvatar[4].y=nHeight/2-211-15;
	m_ptAvatar[3].x=nWidth/2+300;
	m_ptAvatar[3].y=nHeight/2-15;
	m_ptAvatar[2].x=nWidth/2-15;
	m_ptAvatar[2].y=nHeight/2+199-15;
	m_ptAvatar[1].x=nWidth/2-355;
	m_ptAvatar[1].y=nHeight/2-15;
	m_ptAvatar[0].x=nWidth/2-355;
	m_ptAvatar[0].y=nHeight/2-211-15;

	//�û��ǳ�
	m_ptNickName[4].x=nWidth/2+305;
	m_ptNickName[4].y=nHeight/2-211;
	m_ptNickName[3].x=nWidth/2+305;
	m_ptNickName[3].y=nHeight/2;
	m_ptNickName[2].x=nWidth/2-15;
	m_ptNickName[2].y=nHeight/2+199;
	m_ptNickName[1].x=nWidth/2-350;
	m_ptNickName[1].y=nHeight/2;
	m_ptNickName[0].x=nWidth/2-350;
	m_ptNickName[0].y=nHeight/2-211;

	//ʱ��λ��
	m_ptClock[4].x=nWidth/2+126-15;
	m_ptClock[4].y=nHeight/2-100-15;
	m_ptClock[3].x=nWidth/2+226+90;
	m_ptClock[3].y=nHeight/2+122-15;
	m_ptClock[2].x=nWidth/2-112;
	m_ptClock[2].y=nHeight/2+152-15-40;
	m_ptClock[1].x=nWidth/2-290-90;
	m_ptClock[1].y=nHeight/2+122-15;
	m_ptClock[0].x=nWidth/2-171;
	m_ptClock[0].y=nHeight/2-38-62-15;

	//׼��λ��
	m_ptReady[4].x=nWidth/2+229;
	m_ptReady[4].y=nHeight/2-15-30-15;
	m_ptReady[3].x=nWidth/2+330+20;
	m_ptReady[3].y=nHeight/2+163-15;
	m_ptReady[2].x=nWidth/2+10-15;
	m_ptReady[2].y=nHeight/2+178;
	m_ptReady[1].x=nWidth/2-331-20;
	m_ptReady[1].y=nHeight/2+163-15;
	m_ptReady[0].x=nWidth/2-212;
	m_ptReady[0].y=nHeight/2-15-30-15;

	//�û��˿�
	m_CardControl[4].SetBenchmarkPos(nWidth/2+229,nHeight/2-15+30-15,enXCenter,enYCenter);
	m_CardControl[3].SetBenchmarkPos(nWidth/2+330-70,nHeight/2+163-15,enXCenter,enYCenter);
	m_CardControl[2].SetBenchmarkPos(nWidth/2+10,nHeight/2+273-15-40,enXCenter,enYBottom);
	m_CardControl[1].SetBenchmarkPos(nWidth/2-331+70,nHeight/2+163-15,enXCenter,enYCenter);
	m_CardControl[0].SetBenchmarkPos(nWidth/2-212,nHeight/2-15+30-15,enXCenter,enYCenter);

	//���ֿؼ�λ��
	m_NumberControl[4].SetBencbmarkPos(nWidth/2+165+15,nHeight/2-170+134+30-15,enXRight);
	m_NumberControl[3].SetBencbmarkPos(nWidth/2+165+210-70,nHeight/2+40+27-15,enXRight);
	m_NumberControl[2].SetBencbmarkPos(nWidth/2-50-21,nHeight/2+130+84-15-40,enXRight);
	m_NumberControl[1].SetBencbmarkPos(nWidth/2-180-188+70,nHeight/2+40+27-15,enXLeft);
	m_NumberControl[0].SetBencbmarkPos(nWidth/2-180+21,nHeight/2-170+134+30-15,enXLeft);

	//ׯ�ұ�־
	m_PointBanker[4].x=nWidth/2+175+100;
	m_PointBanker[4].y=nHeight/2-66+45-15;
	m_PointBanker[3].x=nWidth/2+317;
	m_PointBanker[3].y=nHeight/2+84-15;
	m_PointBanker[2].x=nWidth/2-151;
	m_PointBanker[2].y=nHeight/2+167-15-40;
	m_PointBanker[1].x=nWidth/2-353;
	m_PointBanker[1].y=nHeight/2+84-15;
	m_PointBanker[0].x=nWidth/2-210-90;
	m_PointBanker[0].y=nHeight/2-66+45-15;

	//����λ��
	m_ptJettons[4].x=m_ptAvatar[4].x-202+50;
	m_ptJettons[4].y=m_ptAvatar[4].y+50+164+30;
	m_ptJettons[3].x=m_ptAvatar[3].x-22-70;
	m_ptJettons[3].y=m_ptAvatar[3].y+30+138;
	m_ptJettons[2].x=m_ptAvatar[2].x;
	m_ptJettons[2].y=m_ptAvatar[2].y-10;
	m_ptJettons[1].x=m_ptAvatar[1].x+72+70;
	m_ptJettons[1].y=m_ptAvatar[1].y+30+138;
	m_ptJettons[0].x=m_ptAvatar[0].x+202;
	m_ptJettons[0].y=m_ptAvatar[0].y+50+164+30;

	m_ptUserCard[4].x=m_CardControl[4].GetCardPos().x;
	m_ptUserCard[4].y=m_CardControl[4].GetCardPos().y;
	m_ptUserCard[3].x=m_CardControl[3].GetCardPos().x;
	m_ptUserCard[3].y=m_CardControl[3].GetCardPos().y;
	m_ptUserCard[2].x=m_CardControl[2].GetCardPos().x;
	m_ptUserCard[2].y=m_CardControl[2].GetCardPos().y;
	m_ptUserCard[1].x=m_CardControl[1].GetCardPos().x;
	m_ptUserCard[1].y=m_CardControl[1].GetCardPos().y;
	m_ptUserCard[0].x=m_CardControl[0].GetCardPos().x;
	m_ptUserCard[0].y=m_CardControl[0].GetCardPos().y;

	//��ҿ��λ��
	m_ptPlayerFrame[0].x = nWidth/2-372;
	m_ptPlayerFrame[0].y = nHeight/2-78-15;
	m_ptPlayerFrame[1].x = nWidth/2-497;
	m_ptPlayerFrame[1].y = nHeight/2+134-15;
	m_ptPlayerFrame[2].x = 0;
	m_ptPlayerFrame[2].y = 0;
	m_ptPlayerFrame[3].x = nWidth/2+390;
	m_ptPlayerFrame[3].y = nHeight/2+134-15;
	m_ptPlayerFrame[4].x = nWidth/2+309;
	m_ptPlayerFrame[4].y = nHeight/2-78-15;

	// ����λ��
	m_ptUser[0].SetPoint(150+2, 117-8-30);
	m_ptUser[1].SetPoint(-29, 304-30);
	m_ptUser[2].SetPoint(-3, nHeight-152-32);
	m_ptUser[3].SetPoint(nWidth/2 + 354, nHeight/2 - 80-15);
	m_ptUser[4].SetPoint(nWidth/2 + 175+4-2, nHeight/2 - 266-5-15);

	//��ť�ؼ�
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//��ʼ��ť
	m_btStart.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btStart,NULL,nWidth/2-36,nHeight-107,0,0,uFlags);

	DeferWindowPos(hDwp,m_btFollow,NULL,210+2*79,nHeight-102,0,0,uFlags);
	DeferWindowPos(hDwp,m_btLookCard,NULL,210,nHeight-102,0,0,uFlags);
	DeferWindowPos(hDwp,m_btCompareCard,NULL,210+1*79,nHeight-102,0,0,uFlags);
	DeferWindowPos(hDwp,m_btJetton_1,NULL,210+3*79,nHeight-102,0,0,uFlags);
	DeferWindowPos(hDwp,m_btJetton_2,NULL,210+4*79,nHeight-102,0,0,uFlags);
	DeferWindowPos(hDwp,m_btJetton_5,NULL,210+5*79,nHeight-102,0,0,uFlags);
	DeferWindowPos(hDwp,m_btGiveUp,NULL,210+6*79,nHeight-102,0,0,uFlags);

	DeferWindowPos(hDwp, m_btMin, NULL, nWidth-185, 4, 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btClose, NULL, nWidth-50, 4, 0, 0, uFlags);
	DeferWindowPos(hDwp,m_btSound,NULL,nWidth-96,4,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOption,NULL,nWidth-142,4,0,0,uFlags);

	DeferWindowPos(hDwp,m_btSend,NULL,nWidth-53,nHeight-27,0,0,uFlags);
	DeferWindowPos(hDwp, m_btHistoryChat, NULL, nWidth-266, nHeight-27, 0, 0, uFlags);

	DeferWindowPos(hDwp, m_ChatInput, NULL, nWidth-227, nHeight-25, 172, 21, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	//DeferWindowPos(hDwp, m_ChatDisplay, NULL, nWidth-262, nHeight-159, 254-17, 128, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
	DeferWindowPos(hDwp, m_ChatMessage, NULL, nWidth-262, nHeight-159, 254-17+14, 128, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);


	//�����ؼ�
	/*m_ChatDisplay.RectifyControl(nWidth, nHeight);*/

	//����λ��
	m_ptUserCompare.SetPoint(nWidth/2,nHeight*2/5);

	int iX=nWidth/2+80;
	int iY=nHeight/2+200;

	//�����ƶ�
	EndDeferWindowPos(hDwp);

	//������ͼ
	CRect rcControl;
	m_ScoreView.GetWindowRect(&rcControl);
	m_ScoreView.SetWindowPos(NULL,(nWidth-rcControl.Width())/2,nHeight/2-190,0,0,SWP_NOZORDER|SWP_NOSIZE);

	//����ؼ�
	m_JettonControl.SetBenchmarkPos(nWidth/2,nHeight/2+57-15);
}

//�滭����
VOID CGameClientView::DrawGameView(CDC * pDC, INT nWidth, INT nHeight)
{
	//����Ч��
	if(m_bFalsh)
	{
		int xValue=rand()%ROCK;
		int yValue=rand()%ROCK;
		if(xValue%2==0)xValue*=(-1);
		if(yValue%2==0)yValue*=(-1);
	}
	else 
	{

		CSize SizeViewBack/*,SizeFrame,SizeTable*/;
		SizeViewBack.SetSize(m_PngImageViewBack_1.GetWidth(),m_PngImageViewBack_1.GetHeight());
		//�滭����
		m_PngImageViewBack_1.DrawImage(pDC,0,0,SizeViewBack.cx,SizeViewBack.cy,0,0);

		DrawUser(pDC,nWidth,nHeight);
	}

	////����
	//CRect rc(5,10,1024,30);
	//m_DFontEx.DrawText(pDC,m_strCaption,&rc,RGB(255,255,255),DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS);

	//��ע���ⶥ����
	CSize SizeFengding;
	SizeFengding.SetSize(m_PngFengding.GetWidth(),m_PngFengding.GetHeight());		
	m_PngFengding.DrawImage(pDC,10+165,0,SizeFengding.cx,SizeFengding.cy,0,0);

	if(!m_bStopDraw)
	{
		//�滭����
		m_JettonControl.DrawJettonControl(pDC);

		//�滭�û�
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//��������
			WORD wUserTimer=GetUserClock(i);
			IClientUserItem * pClientUserItem=GetClientUserItem(i);
#ifdef _DEBUG
			//���Դ���
			//tagUserData Obj;
			//Obj.cbGender = 1;
			//_sntprintf(Obj.szName,sizeof(Obj.szName),TEXT("�û�������"));
			//Obj.cbUserStatus=US_READY;
			////Obj.wFaceID = 2;
			//pClientUserItem = &Obj;

			////ׯ�ұ�־
			//CImageHandle ImageHandleBanker(&m_ImageBanker);
			//m_ImageBanker.TransDrawImage(pDC,m_PointBanker[i].x,m_PointBanker[i].y,m_ImageBanker.GetWidth(),m_ImageBanker.GetHeight(),0,0,RGB(255,0,255));
#endif
			//�滭�û�
			if (pClientUserItem!=NULL)
			{
				//�û�����
				pDC->SetTextColor((wUserTimer>0)?RGB(250,250,250):RGB(220,220,220));
				CSize SizePlayerFrame;
				SizePlayerFrame.SetSize(m_PngImagePlayerFrame.GetWidth(),m_PngImagePlayerFrame.GetHeight());
			
				if(i!=MY_SELF_CHAIRID)
				{
					m_PngImagePlayerFrame.DrawImage(pDC,m_ptPlayerFrame[i].x,m_ptPlayerFrame[i].y,SizePlayerFrame.cx,SizePlayerFrame.cy,0,0);
					pDC->SetTextColor((i>0)?RGB(250,250,250):RGB(220,220,220));					
					DrawTextString(pDC,pClientUserItem->GetNickName(),RGB(255,253,0),RGB(0,0,0),m_ptPlayerFrame[i].x+5,m_ptPlayerFrame[i].y+9,DT_CENTER|DT_END_ELLIPSIS);

				}
				else
				{
					DrawTextString(pDC,pClientUserItem->GetNickName(),RGB(255,253,0),RGB(0,0,0),250,nHeight-22,DT_CENTER|DT_END_ELLIPSIS);
				}

				//�û����
				TCHAR szBuffer[64]=TEXT("");
				LONGLONG lTempScore=((m_ScoreView.IsWindowVisible()==TRUE || m_NumberControl[i].GetScore()!=0)?(0):(m_lTableScore[i]));
				if(m_btStart.IsWindowVisible()==TRUE || m_cbPlayStatus[i]==FALSE)lTempScore=0;
				LONGLONG lLeaveScore=pClientUserItem->GetUserScore()-lTempScore;
				if(m_lStopUpdataScore[i]>0)lLeaveScore=m_lStopUpdataScore[i]-m_lTableScore[i];
				_sntprintf(szBuffer,sizeof(szBuffer),TEXT("%I64d"),lLeaveScore);
			
				if(i==MY_SELF_CHAIRID)
				{
					CSize SizeMyselfNum;
					SizeMyselfNum.SetSize(m_PngImageMyself.GetWidth()/10,m_PngImageMyself.GetHeight());
					for(INT i = 0 ; i < 10 ; ++i)
					{
						LONGLONG tmp = lLeaveScore%10;
						
						m_PngImageMyself.DrawImage(pDC,503+SizeMyselfNum.cx*(9-i),nHeight-26,SizeMyselfNum.cx,SizeMyselfNum.cy,SizeMyselfNum.cx*tmp,0,SizeMyselfNum.cx,SizeMyselfNum.cy);
						if((lLeaveScore/=10)==0) break;
					}
				}
				else
				{
					DrawTextString(pDC,szBuffer,RGB(162,255,0),RGB(0,0,0),m_ptPlayerFrame[i].x+5,m_ptPlayerFrame[i].y+32,DT_CENTER|DT_END_ELLIPSIS);
				}

				//������Ϣ
				if (wUserTimer!=0) 
				{
					if(m_CardControl[MY_SELF_CHAIRID].GetCardCount()==0)
						DrawUserTimerEx(pDC,m_ptClock[i].x,m_ptClock[i].y,wUserTimer);
					else DrawUserTimerEx(pDC,m_ptClock[i].x,m_ptClock[i].y,wUserTimer);
				}

				//׼����־
				if (pClientUserItem->GetUserStatus()==US_READY) 
				{
					DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
				}
			}
		}

		//ׯ����Ϣ
		if (m_wBankerUser<5)
		{
			//ׯ�ұ�־
			m_ImageBanker.TransDrawImage(pDC,m_PointBanker[m_wBankerUser].x,m_PointBanker[m_wBankerUser].y,m_ImageBanker.GetWidth(),m_ImageBanker.GetHeight(),0,0,RGB(255,0,255));
		}

		//������ע
		LONGLONG lTableScore=0L;
		for (WORD i=0;i<GAME_PLAYER;i++) lTableScore+=m_lTableScore[i];

		//��ע��Ϣ
		if (lTableScore>=0L)
		{	
			int iCount=0;
			LONGLONG lCell[11];
			ZeroMemory(lCell,sizeof(lCell));
			
			//������עֵ
			CSize SizeXZNumber;
			SizeXZNumber.SetSize(m_PngaImageXZNumber.GetWidth()/10,m_PngaImageXZNumber.GetHeight());
			for(INT i = 0 ; i < 11 ; ++i)
			{
				LONGLONG wXZTmp = lTableScore%10;
				
				m_PngaImageXZNumber.DrawImage(pDC,nWidth/2-61+(10-i)*SizeXZNumber.cx,nHeight-500,SizeXZNumber.cx,SizeXZNumber.cy,wXZTmp*SizeXZNumber.cx,0,SizeXZNumber.cx,SizeXZNumber.cy);
				if((lTableScore/=10)==0) break;
			}

			//����עֵ
			int iTemp=0;
			int iX=m_ImageNumber.GetWidth();
			for (int i=iCount-1;i>=0;i--)
			{
				m_ImageNumber.TransDrawImage(pDC,nWidth/2+39+(iX/10)*(iTemp++),nHeight/2-296,
					iX/10,m_ImageNumber.GetHeight(),iX/10*(int)lCell[i],0,RGB(255,0,255));
			}
		}

		//�÷����ֹ�������
		for( WORD i = 0; i < GAME_PLAYER; i++ )
		{
			m_NumberControl[i].DrawNumberControl(pDC);
		}

		//�滭����
		for (BYTE i=0;i<GAME_PLAYER;i++)
		{
			if (m_lTableScore[i]==0L) continue;

			//�滭��Ŀ
			TCHAR szBuffer[64];
			_sntprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),m_lTableScore[i]);

			//����λ��
			CRect rcDrawRect;
			CPoint BenchMarkPos = m_NumberControl[i].GetBenchMarkPos();
			UINT XCollocateMode = m_NumberControl[i].GetXCollocateMode();
			rcDrawRect.top=BenchMarkPos.y+35;
			rcDrawRect.left=BenchMarkPos.x - (XCollocateMode==enXLeft?0:60);
			rcDrawRect.right=rcDrawRect.left+60;
			rcDrawRect.bottom=rcDrawRect.top+12;
			
			//��������
			CFont ViewFont;
			ViewFont.CreateFont(-14,0,0,0,700,0,0,0,134,3,2,1,2,TEXT("����"));

			//��������
			CFont * pOldFont=pDC->SelectObject(&ViewFont);
		
			//�滭��Ŀ
			UINT uFormate = (XCollocateMode==enXLeft?DT_LEFT:DT_RIGHT)|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE;
			DrawTextString(pDC,szBuffer,RGB(250,200,40),RGB(0,0,0),&rcDrawRect,uFormate);
			
			//��������
			pDC->SelectObject(pOldFont);
			ViewFont.DeleteObject();
		}

		//�滭��Ϣ
		if(m_lCellScore>0)
		{
			CSize SizeFDNum;
			SizeFDNum.SetSize(m_PngImageFDNumber.GetWidth()/10,m_PngImageFDNumber.GetHeight());
			DWORD wDanzhu=1,wFengding=1;
			LONGLONG lDanzhu = m_lCellScore;
			LONGLONG lFending = m_lMaxCellScore;
			while(lFending != 0 || lDanzhu != 0)
			{
				if(lDanzhu/10!=0)
					wDanzhu++;
				if(lFending/10!=0)
					wFengding++;
				lDanzhu/=10;
				lFending/=10;
			}
			INT nXDanzhuPos,nXFengdingPos;
			nXDanzhuPos = 118+165-wDanzhu*SizeFDNum.cx/2;
			nXFengdingPos = 118+165-wFengding*SizeFDNum.cx/2;

			lDanzhu = m_lCellScore;
			lFending = m_lMaxCellScore;
			//��ע
			for(INT i = 0 ; i < wDanzhu ; ++i)
			{
				DWORD wTmp = lDanzhu%10;
				lDanzhu/=10;
				m_PngImageFDNumber.DrawImage(pDC,nXDanzhuPos+(wDanzhu-i)*SizeFDNum.cx,78-30,SizeFDNum.cx,SizeFDNum.cy,wTmp*SizeFDNum.cx,0,SizeFDNum.cx,SizeFDNum.cy);
			}

			//�ⶥ
			for(INT i = 0 ; i < wFengding ; ++i)
			{
				DWORD wTmp = lFending%10;
				lFending/=10;
				m_PngImageFDNumber.DrawImage(pDC,nXFengdingPos+(wFengding-i)*SizeFDNum.cx,52-30,SizeFDNum.cx,SizeFDNum.cy,wTmp*SizeFDNum.cx,0,SizeFDNum.cx,SizeFDNum.cy);
			}
		
		}

		//ѡ�ȱ�־
		if(m_bCompareCard)
		{
			for (int i=0;i<GAME_PLAYER;i++)
			{
				if(m_bCompareUser[i]==TRUE)
				{
					CPoint cPost=m_CardControl[i].GetCardPos();
					m_PngImageArrow.DrawImage(pDC,cPost.x+40,cPost.y-m_PngImageArrow.GetHeight(),m_PngImageArrow.GetWidth(),m_PngImageArrow.GetHeight(),0,0,m_PngImageArrow.GetWidth(),m_PngImageArrow.GetHeight());
				}
			}
		}
	}

	//���ƻ滭
	if (m_SendCardItemArray.GetCount()>0)
	{
		//��������
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];

		//��ȡ��С
		int nItemWidth=m_ImageCard.GetWidth()/13;
		int nItemHeight=m_ImageCard.GetHeight()/5;

		//�滭�˿�
		int nXPos=m_SendCardPos.x-nItemWidth/2;
		int nYPos=m_SendCardPos.y-nItemHeight/2+10;
		m_ImageCard.DrawImage(pDC,nXPos,nYPos,nItemWidth,nItemHeight,nItemWidth*2,nItemHeight*4);
	}

	//�����˿�
	if (m_wLoserUser!=INVALID_CHAIR)
	{
		//����״̬
		if (m_wConmareIndex<m_wConmareCount)
		{
			//�滭�˿�
			for (BYTE i=0;i<2;i++)
			{
				//��������
				CPoint PointCard;
				CPoint PointStart=m_ptUserCard[m_wCompareChairID[i]];

				//����λ��
				INT nQuotiety=(i==0)?-1:1;
				PointCard.y=PointStart.y+(m_ptUserCompare.y-PointStart.y)*m_wConmareIndex/m_wConmareCount;
				PointCard.x=PointStart.x+(m_ptUserCompare.x-PointStart.x+COMPARE_RADII*nQuotiety)*m_wConmareIndex/m_wConmareCount;

				//�滭�˿�
				DrawCompareCard(pDC,PointCard.x,PointCard.y,false);
			}
		}

		//����״̬
		if ((m_wConmareIndex>=m_wConmareCount)&&((m_wConmareIndex<m_wConmareCount+VSCARD_COUNT_X*VSCARD_COUNT_Y)))
		{
			//�滭�˿�
			for (BYTE i=0;i<2;i++)
			{
				//��������
				CPoint PointCard;
				CPoint PointStart=m_ptUserCard[m_wCompareChairID[i]];

				//����λ��
				INT nQuotiety=(i==0)?-1:1;
				PointCard.y=m_ptUserCompare.y;
				PointCard.x=m_ptUserCompare.x+(COMPARE_RADII+30)*nQuotiety;

				//�滭�˿�
				DrawCompareCard(pDC,PointCard.x,PointCard.y,false);
			}

			//���Ʊ�־
			CSize SizeVSCard;
			CPngImage ImageVSCard;
			ImageVSCard.LoadImage(AfxGetInstanceHandle(),TEXT("VS_CARD"));
			SizeVSCard.SetSize(ImageVSCard.GetWidth()/VSCARD_COUNT_X,ImageVSCard.GetHeight()/VSCARD_COUNT_Y);

			//�滭��־
			INT nXImageIndex=(m_wConmareIndex-m_wConmareCount)%(VSCARD_COUNT_X);
			INT nYImageIndex=(m_wConmareIndex-m_wConmareCount)/(VSCARD_COUNT_X);
			ImageVSCard.DrawImage(pDC,m_ptUserCompare.x-SizeVSCard.cx/2,m_ptUserCompare.y-SizeVSCard.cy/2-10,
				SizeVSCard.cx,SizeVSCard.cy,nXImageIndex*SizeVSCard.cx,nYImageIndex*SizeVSCard.cy);
			TRACE(TEXT("nXImageIndex = %d, nYImageIndex = %d\n"), (int)nXImageIndex, nYImageIndex);

			//����Ч��
			CGameClientEngine * pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
			pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("COMPARE_CARD"));

		}

		//����״̬
		if (m_wConmareIndex>=m_wConmareCount+VSCARD_COUNT_X*VSCARD_COUNT_Y)
		{
			//��������
			WORD wConmareIndex=m_wConmareIndex-m_wConmareCount-VSCARD_COUNT_X*VSCARD_COUNT_Y;

			//�滭�˿�
			for (BYTE i=0;i<2;i++)
			{
				//��������
				CPoint PointCard;
				CPoint PointStart=m_ptUserCard[m_wCompareChairID[i]];

				//����λ��
				INT nQuotiety=(i==0)?-1:1;
				PointCard.y=m_ptUserCompare.y+(PointStart.y-m_ptUserCompare.y)*wConmareIndex/m_wConmareCount;
				PointCard.x=m_ptUserCompare.x+COMPARE_RADII*nQuotiety+(PointStart.x-m_ptUserCompare.x-COMPARE_RADII*nQuotiety)*wConmareIndex/m_wConmareCount;

				//�滭�˿�
				DrawCompareCard(pDC,PointCard.x,PointCard.y,(m_wLoserUser==m_wCompareChairID[i])?true:false);
			}

			//����Ч��
			if(m_wConmareIndex==m_wConmareCount+VSCARD_COUNT_X*VSCARD_COUNT_Y)
			{
				CGameClientEngine * pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
				pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("CENTER_SEND_CARD"));
			}
		}
	}

	//�ȴ�ѡ��
	if(m_wWaitUserChoice!=INVALID_CHAIR)
	{
		//TCHAR szBuffer[64]=TEXT("");

		////��������
		//CFont ViewFont;
		//ViewFont.CreateFont(20,0,0,0,800,0,0,0,134,3,2,1,2,TEXT("����"));

		////��������
		//CFont * pOldFont=pDC->SelectObject(&ViewFont);

		////�����Ϣ
		//if(m_wWaitUserChoice==TRUE)_sntprintf(szBuffer,sizeof(szBuffer),TEXT("�ȴ����ѡ�����"));
		//else _sntprintf(szBuffer,sizeof(szBuffer),TEXT("��ѡ����ҽ��б���"));
		//CRect rcdraw;
		//rcdraw.left = nWidth/2+10;
		//rcdraw.top = nHeight/2+45;
		//rcdraw.right = nWidth/2+10+150;
		//rcdraw.bottom = nHeight/2+45+30;
		//DrawTextString(pDC,szBuffer,RGB(247,243,55),RGB(0,0,0),&rcdraw,DT_CENTER|DT_NOCLIP);

		////��������
		//pDC->SelectObject(pOldFont);
		//ViewFont.DeleteObject();

		//�����Ϣ
		if(m_wWaitUserChoice==TRUE)
		{
			m_PngWaitSelect.DrawImage(pDC,(nWidth-m_PngWaitSelect.GetWidth())/2,(nHeight-m_PngWaitSelect.GetHeight())/2);
		}
		else 
			m_PngSelectPlayer.DrawImage(pDC,(nWidth-m_PngSelectPlayer.GetWidth())/2,(nHeight-m_PngSelectPlayer.GetHeight())/2);
	}
	if(m_bShowControlBack == true)
	{
		CSize SizeJetonBack;
		SizeJetonBack.SetSize(m_PngImageJetonBack.GetWidth(),m_PngImageJetonBack.GetHeight());
		m_PngImageJetonBack.DrawImage(pDC,(nWidth-SizeJetonBack.cx)/2,nHeight-106,SizeJetonBack.cx,SizeJetonBack.cy,0,0);
	}

	//��������
	CDC dcBuffer;
	CBitmap bmpBuffer;
	m_brush.DeleteObject();
	dcBuffer.CreateCompatibleDC(pDC);
	bmpBuffer.CreateCompatibleBitmap(pDC, 172, 21);
	CBitmap *pBmpBuffer = dcBuffer.SelectObject(&bmpBuffer);	
	dcBuffer.BitBlt(0, 0, 172, 21, pDC, nWidth-227, nHeight-25, SRCCOPY);
	dcBuffer.SelectObject(pBmpBuffer);
	m_brush.CreatePatternBrush(&bmpBuffer);
//	m_ChatDisplay.Invalidate(TRUE);

	//���챳��
	if (m_bShowChatMessage == true)
	{
		m_PngImageChatBack.DrawImage(pDC,nWidth-269,nHeight-167);
	}

#ifdef _DEBUG

#endif // _DEBUG

	return;
}

//��ʼ���ֹ���
void CGameClientView::BeginMoveNumber()
{
	BOOL bSuccess = FALSE;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_NumberControl[i].BeginScrollNumber() )
			bSuccess = TRUE;
	}
	if( bSuccess )
	{
		SetTimer( IDI_MOVE_NUMBER,TIME_MOVE_NUMBER,NULL );
		RefreshGameView( );
	}

	return;
}

//ֹͣ���ֹ���
void CGameClientView::StopMoveNumber()
{
	BOOL bMoving = FALSE;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( m_NumberControl[i].FinishScrollNumber() )
			bMoving = TRUE;
	}
	if( bMoving )
	{
		KillTimer( IDI_MOVE_NUMBER );
		RefreshGameView(  );
	}

	return;
}

//������ע
void CGameClientView::SetUserTableScore(WORD wChairID, LONGLONG lTableScore,LONGLONG lCurrentScore)
{
	//��������
	if (wChairID!=INVALID_CHAIR) 
	{
		m_lTableScore[wChairID]=lTableScore;
		m_JettonControl.AddScore(lCurrentScore,m_ptJettons[wChairID]);
	}
	else 
	{
		ZeroMemory(m_tcBuffer,sizeof(m_tcBuffer));
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_lDrawGold[i].RemoveAll();
			m_ptKeepJeton[i].RemoveAll();
		}
		ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	}

	//���½���
	RefreshGameView();

	return;
}

//��ʼ���붯��
void CGameClientView::BeginMoveJettons()
{
	if( m_JettonControl.BeginMoveJettons() )
	{
		SetTimer( IDI_MOVE_JETTON,TIME_MOVE_JETTON,NULL );

		//���½���
		RefreshGameView();
	}
	return;
}

//ֹͣ���붯��
void CGameClientView::StopMoveJettons()
{
	if( m_JettonControl.FinishMoveJettons() )
	{
		KillTimer( IDI_MOVE_JETTON );

		//���½���
		RefreshGameView(  );
	}
	return;
}

//ʤ�����
void CGameClientView::SetGameEndInfo( WORD wWinner )
{
	if( wWinner == INVALID_CHAIR ) return ;

	m_JettonControl.RemoveAllScore( m_ptJettons[wWinner] );
	if( m_JettonControl.BeginMoveJettons() )
		SetTimer( IDI_MOVE_JETTON,TIME_MOVE_JETTON,NULL );

	//���½���
	RefreshGameView(  );
	return;
}

//�ȴ�ѡ��
void CGameClientView::SetWaitUserChoice(WORD wChoice)
{
	m_wWaitUserChoice = wChoice;

	//���½���
	RefreshGameView();

	return;
}

//ׯ�ұ�־
void CGameClientView::SetBankerUser(WORD wBankerUser)
{
	m_wBankerUser=wBankerUser;

	//���½���
	RefreshGameView();
	return;
}

//������Ϣ
void CGameClientView::SetScoreInfo(LONGLONG lTurnMaxScore,LONGLONG lTurnLessScore)
{
	m_lMaxCellScore=lTurnMaxScore;
	m_lCellScore=lTurnLessScore;

	//���½���
	RefreshGameView();
	return;
}

//���Ʊ�־
void CGameClientView::SetCompareCard(bool bCompareCard,BOOL bCompareUser[])
{
	m_bCompareCard=bCompareCard;
	if(bCompareUser!=NULL)
	{
		CopyMemory(m_bCompareUser,bCompareUser,sizeof(m_bCompareUser));
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(m_bCompareUser[i]==TRUE)m_CardControl[i].SetCompareCard(true);
			else m_CardControl[i].SetCompareCard(false);
		}
	}
	//���½���
	RefreshGameView();

	if(!bCompareCard)
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			m_CardControl[i].SetCompareCard(false) ;
		}
	}

	return;
}

//ֹͣ����
void CGameClientView::StopUpdataScore(bool bStop)
{
	//��������
	ZeroMemory(m_lStopUpdataScore,sizeof(m_lStopUpdataScore));

	//������Ŀ
	if(bStop)
	{
		//��ȡ�û�
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//��������
			IClientUserItem * pClientUserItem=GetClientUserItem(i);
			if(pClientUserItem!=NULL)
			{
				m_lStopUpdataScore[i]=pClientUserItem->GetUserScore();
			}
		}
	}

	//���½���
	RefreshGameView();

	return;
}

//�滭�˿�
void CGameClientView::DrawCompareCard(CDC * pDC, INT nXPos, INT nYPos, bool bChapped)
{
	//������Դ
	CSize SizeGameCard(m_ImageComPareCard.GetWidth()/13,m_ImageComPareCard.GetHeight()/5);

	//����λ��
	nYPos-=SizeGameCard.cy/2;
	nXPos-=(SizeGameCard.cx+DEFAULT_PELS*(MAX_COUNT-1))/2;

	//�滭�˿�
	for (BYTE i=0;i<MAX_COUNT;i++)
	{
		INT nXDrawPos=nXPos+DEFAULT_PELS*i;
		m_ImageComPareCard.DrawImage(pDC,nXDrawPos,nYPos,SizeGameCard.cx,SizeGameCard.cy,
			SizeGameCard.cx*((bChapped==true)?3:2),SizeGameCard.cy*4);
	}

	return;
}

//��������
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos,UINT nFormat)
{
	//��������
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//�滭�߿�
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		CRect rcFrame(nXPos+nXExcursion[i],nYPos+nYExcursion[i],nXPos+nXExcursion[i]+100,nYPos+nYExcursion[i]+16);
		m_DFontEx.DrawText(pDC,pszString,&rcFrame,crFrame,nFormat);
	}

	//�滭����
	CRect rcText(nXPos,nYPos,nXPos+100,nYPos+16);
	m_DFontEx.DrawText(pDC,pszString,&rcText,crText,nFormat);

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

//��������
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect,UINT nFormat)
{
	//��������
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//�滭�߿�
	pDC->SetTextColor(crFrame);
	CRect rcDraw;
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		rcDraw.CopyRect(lpRect);
		rcDraw.OffsetRect(nXExcursion[i],nYExcursion[i]);
		DrawText(pDC,pszString,nStringLength,&rcDraw,nFormat);
	}

	//�滭����
	rcDraw.CopyRect(lpRect);
	pDC->SetTextColor(crText);
	DrawText(pDC,pszString,nStringLength,&rcDraw,nFormat);

	return;
}

//���ƶ���
void CGameClientView::PerformCompareCard(WORD wCompareUser[2], WORD wLoserUser)
{
	//Ч��״̬
	ASSERT(m_wLoserUser==INVALID_CHAIR);
	if (m_wLoserUser!=INVALID_CHAIR) return;

	//���ñ���
	m_wConmareIndex=0;
	m_wLoserUser=wLoserUser;
	m_wConmareCount=LESS_SEND_COUNT;

	//�Ա��û�
	m_wCompareChairID[0]=wCompareUser[0];
	m_wCompareChairID[1]=wCompareUser[1];

	//��������
	for (BYTE i=0;i<2;i++)
	{
		//λ�Ƽ���
		INT nXCount=abs(m_ptUserCard[wCompareUser[i]].x-m_ptUserCompare.x)/SEND_PELS;
		INT nYCount=abs(m_ptUserCard[wCompareUser[i]].y-m_ptUserCompare.y)/SEND_PELS;

		//��������
		m_wConmareCount=__max(m_wConmareCount,__max(nXCount,nYCount));
	}

	//�����˿�
	m_CardControl[wCompareUser[0]].SetCardData(NULL,0);
	m_CardControl[wCompareUser[1]].SetCardData(NULL,0);

	//����ʱ��
	if (!m_bCartoon)
	{
		m_bCartoon=true;
		SetTimer(IDI_CARTOON,TIME_CARTOON,NULL);
	}

	//����Ч��
	CGameClientEngine * pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
	pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("CENTER_SEND_CARD"));
	return;
}

//���ƶ���
void CGameClientView::DispatchUserCard(WORD wChairID, BYTE cbCardData)
{
	//Ч�����
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//���ñ���
	tagSendCardItem SendCardItem;
	SendCardItem.wChairID=wChairID;
	SendCardItem.cbCardData=cbCardData;
	m_SendCardItemArray.Add(SendCardItem);

	//��������
	if (m_SendCardItemArray.GetCount()==1) 
	{
		//��ȡλ��
		CRect rcClient;
		GetClientRect(&rcClient);

		//����λ�� /2
		m_SendCardPos.x=rcClient.Width()/2;
		m_SendCardPos.y=rcClient.Height()/2;
		m_KeepPos.x =m_SendCardPos.x;
		m_KeepPos.y=m_SendCardPos.y;

		if(!m_bCartoon)
		{
			m_bCartoon = true;
			//���ö�ʱ��
			SetTimer(IDI_CARTOON,TIME_CARTOON/2,NULL);
		}

		//��������
		CGameClientEngine * pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
		pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
	}

	return;
}

//���ƶ���
bool CGameClientView::bFalshCard(WORD wFalshUser[])
{
	for(int i=0;i<GAME_PLAYER;i++)
	{
		if(wFalshUser[i]<GAME_PLAYER)m_wFlashUser[i] = wFalshUser[i];
	}

	//���ö�ʱ��
	m_bStopDraw=true;

	if(!m_bCartoon)
	{
		m_bCartoon = true;
		SetTimer(IDI_CARTOON,TIME_CARTOON,NULL);
	}

	return true;
}

//���ƴ���
bool CGameClientView::SendCard()
{
	//�����ж�
	if (m_SendCardItemArray.GetCount()==0) return false;

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	int nXExcursion[GAME_PLAYER]={-1,-1,-1,1,1};
	int nYExcursion[GAME_PLAYER]={-1,1,1,1,-1};

	//����λ��
	tagSendCardItem * pSendCardItem=&m_SendCardItemArray[0];
	WORD bID=pSendCardItem->wChairID;
	BYTE bTimes;
	if(bID==0) bTimes=4;
	else if(bID==1) bTimes=2;
	else if(bID==2) bTimes=2;
	else if(bID==3) bTimes=2;
	else if(bID==4) bTimes=3;

	m_SendCardPos.x+=nXExcursion[bID]*(abs(m_ptUserCard[bID].x-m_KeepPos.x)/SPEED*bTimes);
	m_SendCardPos.y+=nYExcursion[bID]*(abs(m_ptUserCard[bID].y-m_KeepPos.y)/SPEED*bTimes);

	//���½���
	RefreshGameView();

	//ֹͣ�ж�
	int bTempX=0,bTempY=0;
	bTempX=m_ptUserCard[bID].x;
	bTempY=m_ptUserCard[bID].y;
	if((pSendCardItem->wChairID<=2 && m_SendCardPos.x<=bTempX)||(pSendCardItem->wChairID>=3 && m_SendCardPos.x>=bTempX))
	{
		//��ȡ�˿�
		BYTE cbCardData[MAX_COUNT];
		WORD wChairID=pSendCardItem->wChairID;
		BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

		//�����˿�&& pSendCardItem->cbCardData!=0
		if (cbCardCount<MAX_COUNT)
		{
			cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
			m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);
		}

		//ɾ������
		m_SendCardItemArray.RemoveAt(0);

		//��������
		if (m_SendCardItemArray.GetCount()>0)
		{
			//����λ�� /2
			GetClientRect(&rcClient);
			m_SendCardPos.x=rcClient.Width()/2;
			m_SendCardPos.y=rcClient.Height()/2;

			//��������
			CGameClientEngine * pGameClientDlg=CONTAINING_RECORD(this,CGameClientEngine,m_GameClientView);
			pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));

			return true;
		}
		else
		{
			//��ɴ���
			SendEngineMessage(IDM_SEND_CARD_FINISH,0,0);

			return false;
		}
	}

	return true;
}

//���ƴ���
bool CGameClientView::FlashCard()
{	
	//�����ж�
	for(int i=0;i<GAME_PLAYER;i++) if(m_wFlashUser[i]<GAME_PLAYER)break;
	if(i==GAME_PLAYER)return false;

	//���ñ���
	m_wFalshCount++;
	WORD wTimes=m_wFalshCount*TIME_CARTOON;

	//���½���
	RefreshGameView();
	m_bFalsh=!m_bFalsh;

	//�˿˱���
	WORD wValue=(rand()%2==0)?((m_wFalshCount+1)%6):((m_wFalshCount+2)%6);
	for(int i=0;i<GAME_PLAYER;i++)
	{
		if(m_wFlashUser[i]<GAME_PLAYER)m_CardControl[m_wFlashUser[i]].SetCardColor(wValue);
	}

	//ֹͣ����
	if(2200 <= wTimes)
	{
		//���ò���
		m_bFalsh=false;
		m_bStopDraw=false;
		m_wFalshCount=0;
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(m_wFlashUser[i]<GAME_PLAYER)
			{
				m_CardControl[m_wFlashUser[i]].SetCardColor(INVALID_CHAIR);
				m_wFlashUser[i]=INVALID_CHAIR;
			}
		}

		//��ɴ���
		SendEngineMessage(IDM_FALSH_CARD_FINISH,0,0);

		//���½���
		RefreshGameView();

		return false; 
	}

	return true; 
}

//���ƴ���
bool CGameClientView::CompareCard()
{
	//�����ж�
	if (m_wLoserUser==INVALID_CHAIR) return false;

	//��������
	m_wConmareIndex++;

	//���½���
	RefreshGameView();

	//ֹͣ�ж�
	if (m_wConmareIndex>=((m_wConmareCount*2)+VSCARD_COUNT_X*VSCARD_COUNT_Y))
	{
		//�����˿�
		BYTE bTemp[]={0,0,0};

		//�������û���ʾ�Ʊ�
		if (!m_bCheatUser) m_CardControl[m_wLoserUser].SetCardColor(2);

		m_CardControl[m_wCompareChairID[0]].SetCardData(bTemp,MAX_COUNT);
		m_CardControl[m_wCompareChairID[1]].SetCardData(bTemp,MAX_COUNT);

		//���ñ���
		m_wConmareIndex=0;
		m_wConmareCount=0;
		m_wLoserUser=INVALID_CHAIR;
		ZeroMemory(m_wCompareChairID,sizeof(m_wCompareChairID));

		//������Ϣ
		SendEngineMessage(IDM_FALSH_CARD_FINISH,0,0);

		return false;
	}

	return true;
}

//ֹͣ����
void CGameClientView::StopFlashCard()
{
	//�����ж�
	for(int i=0;i<GAME_PLAYER;i++) if(m_wFlashUser[i]<GAME_PLAYER)break;
	if(i==GAME_PLAYER)return ;

	//���ò���
	m_bFalsh=false;
	m_bStopDraw=false;
	m_wFalshCount=0;
	for(int i=0;i<GAME_PLAYER;i++)
	{
		if(m_wFlashUser[i]<GAME_PLAYER)
		{
			m_CardControl[m_wFlashUser[i]].SetCardColor(INVALID_CHAIR);
			m_wFlashUser[i]=INVALID_CHAIR;
		}
	}

	//��ɴ���
	SendEngineMessage(IDM_FALSH_CARD_FINISH,0,0);

	return ;
}

//ֹͣ����
void CGameClientView::StopCompareCard()
{
	if(m_wLoserUser>GAME_PLAYER)return ;

	//�����˿�
	BYTE bTemp[]={0,0,0};

	//�������û���ʾ�Ʊ�
	if (!m_bCheatUser) m_CardControl[m_wLoserUser].SetCardColor(2);

	m_CardControl[m_wCompareChairID[0]].SetCardData(bTemp,MAX_COUNT);
	m_CardControl[m_wCompareChairID[1]].SetCardData(bTemp,MAX_COUNT);

	//���ñ���
	m_wConmareIndex=0;
	m_wConmareCount=0;
	m_wLoserUser=INVALID_CHAIR;
	ZeroMemory(m_wCompareChairID,sizeof(m_wCompareChairID));

	//������Ϣ
	SendEngineMessage(IDM_FALSH_CARD_FINISH,0,0);

	return ;
}

//��ɷ���
void CGameClientView::FinishDispatchCard()
{
	//����ж�
	if (m_SendCardItemArray.GetCount()==0) return;

	//��ɶ���
	for (INT_PTR i=0;i<m_SendCardItemArray.GetCount();i++)
	{
		//��ȡ����
		tagSendCardItem * pSendCardItem=&m_SendCardItemArray[i];

		//��ȡ�˿�
		BYTE cbCardData[MAX_COUNT];
		WORD wChairID=pSendCardItem->wChairID;
		BYTE cbCardCount=(BYTE)m_CardControl[wChairID].GetCardData(cbCardData,CountArray(cbCardData));

		//�����˿�
		if (cbCardCount<MAX_COUNT)
		{
			cbCardData[cbCardCount++]=pSendCardItem->cbCardData;
			m_CardControl[wChairID].SetCardData(cbCardData,cbCardCount);
		}
	}

	//ɾ������
	m_SendCardItemArray.RemoveAll();

	//��ɴ���
	SendEngineMessage(IDM_SEND_CARD_FINISH,0,0);

	//���½���
	RefreshGameView();

	return;
}

//��ʱ����Ϣ
void CGameClientView::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==IDI_CARTOON)		//������ʱ
	{
		//��������
		bool bKillTimer=true;

		//���ƶ���
		if(m_bStopDraw && (FlashCard()==true))
		{
			bKillTimer=false;
		}

		//���ƶ���
		if ((m_wLoserUser!=INVALID_CHAIR)&&(CompareCard()==true))
		{
			bKillTimer=false;
		}

		//���ƶ���
		if ((m_SendCardItemArray.GetCount()>0)&&(SendCard()==true))
		{
			bKillTimer=false;
		}

		//ɾ��ʱ��
		if (bKillTimer==true)
		{
			m_bCartoon=false;
			KillTimer(nIDEvent);
		}

		return;
	}

	//���붯��
	if( IDI_MOVE_JETTON==nIDEvent)
	{
		if( !m_JettonControl.PlayMoveJettons() )
		{
			KillTimer(IDI_MOVE_JETTON);
			RefreshGameView(  );

			return;
		}

		//��ȡ��������
		CRect rcDraw;
		m_JettonControl.GetDrawRect(rcDraw);
		RefreshGameView();

		return;
	}

	//���ֹ���
	if( IDI_MOVE_NUMBER==nIDEvent )
	{
		BOOL bMoving = FALSE;
		for( INT i = 0; i < GAME_PLAYER; i++ )
		{
			if( m_NumberControl[i].PlayScrollNumber() )
			{
				bMoving = TRUE;
				//��ȡ��������
				CRect rc;
				m_NumberControl[i].GetDrawRect(rc);
				RefreshGameView();
			}
		}
		if( !bMoving )
			KillTimer(IDI_MOVE_NUMBER);
		return ;
	}

	//���۶�ʱ
	if(nIDEvent >= IDI_CLOSE_EYE && nIDEvent <=IDI_CLOSE_EYE+4)
	{
		KillTimer(nIDEvent);
		SetTimer(nIDEvent-IDI_CLOSE_EYE+IDI_OPEN_EYE,200,NULL);
		m_bCloseEye[nIDEvent-IDI_CLOSE_EYE] = true;
		m_cbWink[nIDEvent-IDI_CLOSE_EYE]--;

		RefreshGameView(  );
	}

	//���۶�ʱ
	if(nIDEvent >= IDI_OPEN_EYE && nIDEvent <=IDI_OPEN_EYE+4)
	{
		KillTimer(nIDEvent);
		if(m_cbWink[nIDEvent-IDI_OPEN_EYE]!=0)
			SetTimer(nIDEvent+IDI_CLOSE_EYE-IDI_OPEN_EYE,(3+rand()%4)*1000,NULL);
		else
		{
			SetTimer(nIDEvent+IDI_CLOSE_EYE-IDI_OPEN_EYE,300,NULL);
			m_cbWink[nIDEvent-IDI_OPEN_EYE] = 2+rand()%4;
		}
		m_bCloseEye[nIDEvent-IDI_OPEN_EYE] = false;

		RefreshGameView(  );
	}

	__super::OnTimer(nIDEvent);
}

//�����Ϣ
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//���½���
	InvalidGameView(0, 0, 0, 0);

//	LPARAM lParam = MAKEWPARAM(Point.x,Point.y);
//	AfxGetMainWnd()->PostMessage(WM_LBUTTONDOWN,0,lParam);

	return;
}

//�滭ʱ��
VOID CGameClientView::DrawUserTimerEx(CDC * pDC, INT nXPos, INT nYPos, WORD wUserClock)
{
	//�滭ʱ��
	if ((wUserClock>0)&&(wUserClock<100))
	{
		//��ȡ��С
		CSize SizeClockItem;
		CSize SizeClockBack;

		SizeClockBack.SetSize(m_PngImageTimeBack.GetWidth(),m_PngImageTimeBack.GetHeight());
		SizeClockItem.SetSize(m_PngImageTimeNumber.GetWidth()/10,m_PngImageTimeNumber.GetHeight());


		//�滭����
		INT nXDrawPos=nXPos-SizeClockBack.cx/2;
		INT nYDrawPos=nYPos-SizeClockBack.cy/2;
		m_PngImageTimeBack.DrawImage(pDC,nXPos,nYPos,SizeClockBack.cx,SizeClockBack.cy,0,0,SizeClockBack.cx,SizeClockBack.cy);

		//�滭ʱ��
		WORD nClockItem1=wUserClock/10;
		WORD nClockItem2=wUserClock%10;
		m_PngImageTimeNumber.DrawImage(pDC,nXPos+14+4,nYPos+20,SizeClockItem.cx,SizeClockItem.cy,SizeClockItem.cx*nClockItem1,0,SizeClockItem.cx,SizeClockItem.cy);
		m_PngImageTimeNumber.DrawImage(pDC,nXPos+31+2,nYPos+20,SizeClockItem.cx,SizeClockItem.cy,SizeClockItem.cx*nClockItem2,0,SizeClockItem.cx,SizeClockItem.cy);
	}
	return;
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

void CGameClientView::OnSendMessage() 
{
	CString str, mes;
	GetDlgItem(IDC_CHAT_INPUT)->GetWindowText(str);
	if (str.GetLength() < 1) return;

	//У������
	if(EfficacyUserChat(str,INVALID_WORD) == false)
		return ;

	//��ѯ�ӿ�
	IClientKernel * m_pIClientKernel;
	CGlobalUnits * pGlobalUnits = CGlobalUnits::GetInstance();
	ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
	m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
	m_pIClientKernel->SendUserChatMessage(0, str, RGB(193,167,108));

	GetDlgItem(IDC_CHAT_INPUT)->SetWindowText(TEXT(""));
	GetDlgItem(IDC_CHAT_INPUT)->SetFocus();

	return;
}

//����Ч��
bool CGameClientView::EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex)
{
	//״̬�ж�
	CGlobalUnits* pGlobalUnits = CGlobalUnits::GetInstance();
	IClientKernel* m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
	IClientUserItem* m_pIMySelfUserItem = GetClientUserItem(MY_SELF_CHAIRID);
	if (m_pIClientKernel==NULL) return false;
	if (m_pIMySelfUserItem==NULL) return false;

	//���ȱȽ�
	if(pszChatString != NULL)
	{
		CString strChat=pszChatString;
		if(strChat.GetLength() >= (LEN_USER_CHAT/2))
		{
			m_ChatMessage.InsertString(TEXT("��Ǹ����������������ݹ����������̺��ٷ��ͣ�"),COLOR_WARN);
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
			strDescribe.Format(TEXT("\n��%s������ʧ��"),pszChatString);
			m_ChatMessage.InsertString(strDescribe,COLOR_EVENT);
		}

		//������Ϣ
		m_ChatMessage.InsertSystemString(TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û��������죡"));

		return false;
	}

	//Ȩ���ж�
	if (CUserRight::CanGameChat(pUserAttribute->dwUserRight)==false)
	{
		//ԭʼ��Ϣ
		if (wExpressionIndex==INVALID_WORD)
		{
			CString strDescribe;
			strDescribe.Format(TEXT("\n��%s������ʧ��"),pszChatString);
			m_ChatMessage.InsertString(strDescribe,COLOR_EVENT);
		}

		//������Ϣ
		m_ChatMessage.InsertSystemString(TEXT("��Ǹ����û����Ϸ���Ե�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"));

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
			strDescribe.Format(TEXT("\n��%s������ʧ��"),pszChatString);
			m_ChatMessage.InsertString(strDescribe,COLOR_EVENT);
		}

		//������Ϣ
		m_ChatMessage.InsertSystemString(TEXT("����˵���ٶ�̫���ˣ����������ȱ�����Ϣ�°ɣ�"));

		return false;
	}

	//���ñ���
	m_dwChatTime=dwCurrentTime;
	//	m_strChatString=pszChatString;

	return true;
}


// �滭����
void CGameClientView::DrawUser(CDC * pDC,INT nWidth, INT nHeight)
{
	//����ߴ�
	CSize UserSize,EyeSize,SizeFrame,SizeTable;
	SizeTable.SetSize(m_PngImageTable.GetWidth(),m_PngImageTable.GetHeight());
	SizeFrame.SetSize(m_PngImageFrameBack.GetWidth(),m_PngImageFrameBack.GetHeight());

	//�������
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(i == MY_SELF_CHAIRID) continue;
		IClientUserItem * pClientUserItem=GetClientUserItem(i);
		if(pClientUserItem == NULL) continue;
		if(m_ImageUser[i].IsNull() || m_ImageEye[i].IsNull()) continue;
		UserSize.SetSize(m_ImageUser[i].GetWidth()/6,m_ImageUser[i].GetHeight());
		EyeSize.SetSize(m_ImageEye[i].GetWidth()/6,m_ImageEye[i].GetHeight());
		//��������
		m_ImageUser[i].DrawImage( pDC, m_ptUser[i].x, m_ptUser[i].y, UserSize.cx, UserSize.cy, UserSize.cx*m_cbUserIndex[i], 0);
		//���ƶ���
		if(m_bCloseEye[i])
			m_ImageEye[i].DrawImage( pDC, m_ptUser[i].x, m_ptUser[i].y, EyeSize.cx, EyeSize.cy, EyeSize.cx*m_cbUserIndex[i], 0);

	}
	//��������
	m_PngImageTable.DrawImage(pDC,(nWidth-SizeTable.cx)/2,nHeight-SizeTable.cy,SizeTable.cx,SizeTable.cy,0,0);
	m_PngImageFrameBack.DrawImage(pDC,0,0,SizeFrame.cx,SizeFrame.cy,0,0);
	m_PngImageFrameBackBottom.DrawImage(pDC,0,nHeight-30);



	if(GetClientUserItem(MY_SELF_CHAIRID)!=NULL)
	{
		UserSize.SetSize(m_ImageUser[MY_SELF_CHAIRID].GetWidth()/6,m_ImageUser[MY_SELF_CHAIRID].GetHeight());
		EyeSize.SetSize(m_ImageEye[MY_SELF_CHAIRID].GetWidth()/6,m_ImageEye[MY_SELF_CHAIRID].GetHeight());
		//�Լ�����
		m_ImageUser[MY_SELF_CHAIRID].DrawImage( pDC, m_ptUser[MY_SELF_CHAIRID].x, m_ptUser[MY_SELF_CHAIRID].y, UserSize.cx, UserSize.cy, UserSize.cx*m_cbUserIndex[MY_SELF_CHAIRID], 0);
		//���ƶ���
		if(m_bCloseEye[MY_SELF_CHAIRID])
			m_ImageEye[MY_SELF_CHAIRID].DrawImage( pDC, m_ptUser[MY_SELF_CHAIRID].x, m_ptUser[MY_SELF_CHAIRID].y, EyeSize.cx, EyeSize.cy, EyeSize.cx*m_cbUserIndex[MY_SELF_CHAIRID], 0);
	}

}

//�鿴����
void CGameClientView::OnHideChatMessage()
{
	if (m_bShowChatMessage == true)
	{
		m_bShowChatMessage = false;
		//m_ChatDisplay.ShowWindow(SW_HIDE);
		m_ChatMessage.ShowWindow(SW_HIDE);
		//m_ChatDisplay.SetHideWindow(true);
		m_btHistoryChat.SetButtonImage(IDB_BT_SHOW_CHAT, AfxGetInstanceHandle(), false, true);

	}
	else
	{
		m_bShowChatMessage = true;
		//m_ChatDisplay.ShowWindow(SW_SHOW);
		m_ChatMessage.ShowWindow(SW_SHOW);
		//m_ChatDisplay.SetHideWindow(false);
		m_btHistoryChat.SetButtonImage(IDB_BT_HIDE_CHAT, AfxGetInstanceHandle(), false, true);

	}

	return;
}

//�����û�
void CGameClientView::LoadUserImage(WORD wChairID,INT nGender)
{
	//�������
	CString StrUserSource,StrEyeSource;
	StrUserSource.Format(TEXT("USER_%s_%d"),nGender==GENDER_FEMALE?TEXT("GIRL"):TEXT("BOY"),wChairID);
	StrEyeSource.Format(TEXT("EYE_%s_%d"),nGender==GENDER_FEMALE?TEXT("GIRL"):TEXT("BOY"),wChairID);

	//ж����Դ
	if(m_ImageUser[wChairID].IsNull()!=true)  m_ImageUser[wChairID].DestroyImage();
	if(m_ImageEye[wChairID].IsNull()!=true)   m_ImageEye[wChairID].DestroyImage();

	//������Դ
	m_ImageUser[wChairID].LoadImage(this, AfxGetInstanceHandle(),StrUserSource);
	m_ImageEye[wChairID].LoadImage(this, AfxGetInstanceHandle(),StrEyeSource);

	//��ƶ�ʱ
	SetTimer(IDI_CLOSE_EYE+wChairID,(3+rand()%4)*1000,NULL);
	return;
}
//ж���û�
void CGameClientView::DestroyUserImage(WORD wChairID)
{
	//ж��ͼƬ
	if(m_ImageUser[wChairID].IsNull()!=true)  m_ImageUser[wChairID].DestroyImage();
	if(m_ImageEye[wChairID].IsNull()!=true)   m_ImageEye[wChairID].DestroyImage();

	return;
}

//���ñ���
LRESULT CGameClientView::OnSetCaption(WPARAM wParam, LPARAM lParam)
{
	m_strCaption = (LPCTSTR)lParam;
	return 0;
}
//����Ա����
void CGameClientView::OpenAdminWnd()
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
//////////////////////////////////////////////////////////////////////////

