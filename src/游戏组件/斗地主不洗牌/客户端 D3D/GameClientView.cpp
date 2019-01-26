#include "StdAfx.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "CardControl.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define BOMB_CARTOON_COUNT			14									//��ը��Ŀ
#define PLAN_CARTOON_COUNT			3									//�ɻ���Ŀ
#define CARD_WARNING_COUNT			5									//������Ŀ
#define BANKER_MOVIE_COUNT			4									//������Ŀ
#define REVERSAL_CARD_COUNT			4									//������Ŀ
#define LINE_CARTOON_COUNT			13									//˳����Ŀ

//ϴ�ƶ���
#define RIFFLE_CARD_COUNT_X			4									//ϴ����Ŀ
#define RIFFLE_CARD_COUNT_Y			6									//ϴ����Ŀ
#define RIFFLE_CARD_COUNT_ALL		RIFFLE_CARD_COUNT_X*RIFFLE_CARD_COUNT_Y

//���ƶ���
#define DISPATCH_CARD_TIMES			9									//���ƴ���
#define DISPATCH_CARD_SPACE			2									//���Ƽ��
#define DISPATCH_HEAP_DISTANCE		6									//�ƶѼ��

//////////////////////////////////////////////////////////////////////////////////

//��ʾ��ť
#define IDC_BT_START				100									//��ʼ��ť
#define IDC_BT_SCORE				101									//��ְ�ť

//�зְ�ť
#define IDC_CALL_SCORE_1			200									//�зְ�ť
#define IDC_CALL_SCORE_2			201									//�зְ�ť
#define IDC_CALL_SCORE_3			202									//�зְ�ť
#define IDC_CALL_SCORE_NONE			203									//�зְ�ť

//��Ϸ��ť
#define IDC_BT_OUT_CARD				300									//���ư�ť
#define IDC_BT_PASS_CARD			301									//PASS��ť
#define IDC_BT_OUT_PROMPT           302                             	//��ʾ��ť
#define IDC_BT_CLOSE				303									//�رհ�ť
#define IDC_BT_MIN					304									//��С����ť
#define IDC_BT_SOUND				305									//������ť

//���ܰ�ť
#define IDC_BT_TRUSTEE				400									//�йܿ���
#define IDC_BT_CANCEL_TRUSTEE		401									//�йܿ���
#define IDC_BT_SORT_CARD			402									//�˿�����
#define IDC_BT_LAST_TURN			403									//�����˿�
#define IDC_BT_EXPRESSION			404									//���鰴ť
#define IDC_BT_ENTER				405									//�س���ť
#define IDC_BT_VOICE				406									//������ť
#define IDC_BT_SWITCH_SENCE			407									//�����л�
#define IDC_HISTORY_CHAT			408									//�����¼

//��ʾ��ť
#define IDC_CARD_PROMPT_1			500									//��ʾ��ť
#define IDC_CARD_PROMPT_2			501									//��ʾ��ť
#define IDC_CARD_PROMPT_3			502									//��ʾ��ť
#define IDC_CARD_PROMPT_4			503									//��ʾ��ť
#define IDC_CARD_PROMPT_5			504									//��ʾ��ť
#define IDC_CARD_PROMPT_6			505									//��ʾ��ť

//���㰴ť
#define IDC_BT_PLAY_AGAIN			700									//
#define IDC_BT_RETURN				701									//

//��������
#define IDC_CHAT_INPUT				800									//��������
#define	IDC_CHAT_DISPLAY			801									//�����¼

//�ɼ�����
#define IDC_GAME_SCORE				600									//�ɼ�����

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewD3D)

	//ϵͳ��Ϣ
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()

	//���ư�ť
	ON_BN_CLICKED(IDC_BT_START, OnBnClickedStart)
	ON_BN_CLICKED(IDC_BT_SCORE, OnBnClickedScore)
	ON_BN_CLICKED(IDC_BT_SORT_CARD, OnBnClickedSortCard)
	ON_BN_CLICKED(IDC_BT_LAST_TURN, OnBnClickedLastTurn)
	ON_BN_CLICKED(IDC_BT_TRUSTEE, OnBnClickedTrustee)
	ON_BN_CLICKED(IDC_BT_CANCEL_TRUSTEE, OnBnClickedCancelTrustee)

	ON_BN_CLICKED(IDC_BT_EXPRESSION, OnBnClickedExpression)
	ON_BN_CLICKED(IDC_BT_ENTER, OnBnClickedEnter)
	ON_BN_CLICKED(IDC_BT_VOICE, OnBnClickedVoice)
	ON_BN_CLICKED(IDC_BT_SWITCH_SENCE, OnBnClickedSwitchSence)

	//��Ϸ��ť
	ON_BN_CLICKED(IDC_BT_OUT_CARD, OnBnClickedOutCard)
	ON_BN_CLICKED(IDC_BT_PASS_CARD, OnBnClickedPassCard)
	ON_BN_CLICKED(IDC_BT_OUT_PROMPT, OnBnClickedOutPrompt)

	//�зְ�ť
	ON_BN_CLICKED(IDC_CALL_SCORE_1, OnBnClickedCallScore1)
	ON_BN_CLICKED(IDC_CALL_SCORE_2, OnBnClickedCallScore2)
	ON_BN_CLICKED(IDC_CALL_SCORE_3, OnBnClickedCallScore3)
	ON_BN_CLICKED(IDC_CALL_SCORE_NONE, OnBnClickedCallScoreNone)

	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientView::CGameClientView()
{
	//��Ϸ����
	m_cbBankerScore=0;
	m_wBankerUser=INVALID_CHAIR;
	m_bCellScoreEnd = false;
	m_bGameEnd = false;
	m_bPlaySound = true;
	m_bChangeTimePos = false;
	m_wMessageLen = 0;
	m_bShowChatMessage = true;

	//��ը����
	m_bCartoonBomb=false;
	m_wCartoonBombIndex=0L;

	//�ɻ�����
	m_bCartoonPlan=false;
	m_wCartoonPlanIndex=0L;

	//�������
	m_bCartoonRocket=false;
	m_wCartoonRocketIndex=0l;

	//�������
	m_bCartoonLine=false;
	m_wCartoonLineIndex=0l;

	//ׯ�Ҷ���
	m_wBankerMovieIndex=0L;

	//���ƶ���
	m_bReversalCard=false;
	m_wReversalCardIndex=0L;

	//���ƶ���
	m_bRiffleCard=false;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=0L;

	//�ƶ�����
	m_bMoveMouse=false;
	m_bSelectCard=false;
	m_bSwitchCard=false;
	m_cbHoverCardItem=INVALID_ITEM;
	m_cbMouseDownItem=INVALID_ITEM;
	m_cbSwitchCardItem=INVALID_ITEM;

	//��Ϸ����
	m_lCellScore=0L;
	m_cbBombCount=0L;
	ZeroMemory(m_cbCallScore,sizeof(m_cbCallScore));
	ZeroMemory(m_bUserCheat,sizeof(m_bUserCheat));

	//��������
	m_wStartUser=INVALID_CHAIR;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//λ�ñ���
	ZeroMemory(&m_ptPass,sizeof(m_ptPass));
	ZeroMemory(&m_ptBanker,sizeof(m_ptBanker));
	ZeroMemory(&m_ptCountWarn,sizeof(m_ptCountWarn));
	ZeroMemory(&m_ptCallScore,sizeof(m_ptCallScore));
	ZeroMemory(&m_ptOrderFlag,sizeof(m_ptOrderFlag));

	//���ñ���
	m_bShowScore=false;
	m_bLastTurnState=false;
	m_bWaitCallScore=false;
	ZeroMemory(&m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(&m_pHistoryScore,sizeof(m_pHistoryScore));

	//��������
	ZeroMemory(&m_bCountWarn,sizeof(m_bCountWarn));
	ZeroMemory(&m_wCountWarnIndex,sizeof(m_wCountWarnIndex));

	m_dTimeScale = 1.0;

	//�������
	for(int i = 0 ; i< GAME_PLAYER ;++i)
		m_ChatChair[i] = INVALID_CHAIR;
	m_cbSenceIndex = 0;
	m_dwChatTime = 0;
	ZeroMemory(m_nChatStrlen,sizeof(m_nChatStrlen));



	return;
}

//��������
CGameClientView::~CGameClientView()
{
	//m_FontChat.DeleteFont();
	
}

//���ý���
VOID CGameClientView::ResetGameView()
{
	//��Ϸ����
	m_cbBankerScore=0;
	m_wBankerUser=INVALID_CHAIR;
	m_bPlaySound = true;

	//��ը����
	m_bCartoonBomb=false;
	m_wCartoonBombIndex=0L;

	//�ɻ�����
	m_bCartoonPlan=false;
	m_wCartoonPlanIndex=0L;

	//�������
	m_bCartoonRocket=false;
	m_wCartoonRocketIndex=0l;

	//ׯ�Ҷ���
	m_wBankerMovieIndex=0L;

	//���ƶ���
	m_bReversalCard=false;
	m_wReversalCardIndex=0L;

	//���ƶ���
	m_bRiffleCard=false;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=0L;

	//�ƶ�����
	m_bMoveMouse=false;
	m_bSelectCard=false;
	m_bSwitchCard=false;
	m_cbHoverCardItem=INVALID_ITEM;
	m_cbMouseDownItem=INVALID_ITEM;
	m_cbSwitchCardItem=INVALID_ITEM;

	//��Ϸ����
	m_lCellScore=0L;
	m_cbBombCount=0L;
	ZeroMemory(m_cbCallScore,sizeof(m_cbCallScore));
	ZeroMemory(m_bUserCheat,sizeof(m_bUserCheat));

	//��������
	m_wStartUser=INVALID_CHAIR;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//���ñ���
	m_bShowScore=false;
	m_bLastTurnState=false;
	m_bWaitCallScore=false;
	ZeroMemory(&m_bUserPass,sizeof(m_bUserPass));
	ZeroMemory(&m_pHistoryScore,sizeof(m_pHistoryScore));
	ZeroMemory(m_bUserTrustee,sizeof(m_bUserTrustee));

	//��������
	ZeroMemory(&m_bCountWarn,sizeof(m_bCountWarn));
	ZeroMemory(&m_wCountWarnIndex,sizeof(m_wCountWarnIndex));

	//���ý���
	m_btScore.ShowWindow(false);
	m_btStart.ShowWindow(false);
	m_btOutCard.ShowWindow(false);
	m_btPassCard.ShowWindow(false);
	m_btOutPrompt.ShowWindow(false);

	//��Ϸ�ؼ�
	m_btTrustee.ShowWindow(true);
	m_btCancelTrustee.ShowWindow(false);
	m_btTrustee.EnableWindow(false);
	m_btLastTurn.EnableWindow(false);
	m_btSortCard.EnableWindow(false);

	//�зְ�ť
	m_btCallScore1.ShowWindow(false);
	m_btCallScore2.ShowWindow(false);
	m_btCallScore3.ShowWindow(false);
	m_btCallScoreNone.ShowWindow(false);

	//��Ϸ�ؼ�
	m_ScoreControl.CloseControl();
	m_BackCardControl.SetCardData(NULL,0);
	m_HandCardControl[MYSELF_VIEW_ID].SetPositively(false);

	//�û��˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].SetCardData(NULL,0);
		m_UserCardControl[i].SetCardData(NULL,0);
		m_HandCardControl[i].SetDisplayItem(false);

		m_ChetCardContol[i].SetCardData(NULL,0);
		m_ChetCardContol[i].SetDisplayItem(false);
	}

	//������Դ
	if (m_TextureUserPass.IsNull()==false) m_TextureUserPass.Destory();
	if (m_TextureLastTurn.IsNull()==false) m_TextureLastTurn.Destory();
	if (m_TextureCallScore.IsNull()==false) m_TextureCallScore.Destory();
	if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();
	if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();
	if (m_TextureHistoryScore.IsNull()==false) m_TextureHistoryScore.Destory();
	if (m_TextureReversalCard.IsNull()==false) m_TextureReversalCard.Destory();
	if (m_TextureWaitCallScore.IsNull()==false) m_TextureWaitCallScore.Destory();
	if(m_TextureRocket.IsNull()==false) m_TextureRocket.Destory();
	if(m_TextureRocketLight.IsNull()==false) m_TextureRocketLight.Destory();

	//��ťͼƬ
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btScore.SetButtonImage(&m_D3DDevice,TEXT("BT_SHOW_SCORE"),TEXT("PNG"),hResInstance);
	m_btSortCard.SetButtonImage(&m_D3DDevice,TEXT("BT_SORT_CARD_ORDER"),TEXT("PNG"),hResInstance);

	return;
}

//�����ؼ�
VOID CGameClientView::RectifyControl(INT nWidth, INT nHeight)
{
	//ͷ��λ��
	m_ptAvatar[0].SetPoint(8,nHeight/2-91-15+102);
	m_ptAvatar[1].SetPoint(17+53,nHeight-113+48);
	m_ptAvatar[2].SetPoint(nWidth-121-60,nHeight/2-90-15+102);

	//�ʺ�λ��
	m_ptNickName[0].SetPoint(m_ptAvatar[0].x+87,m_ptAvatar[0].y+14);
	m_ptNickName[1].SetPoint(153,nHeight-29);
	m_ptNickName[2].SetPoint(m_ptAvatar[2].x+87,m_ptAvatar[2].y+14);

	//ʱ��λ��
	m_ptClock[0].SetPoint(nWidth*21/80-64,nHeight/2-100+135-65);
	m_ptClock[1].SetPoint(nWidth/2,nHeight-335+50);
	m_ptClock[2].SetPoint(nWidth*59/80+64,nHeight/2-100+135-65);

	//׼��λ��
	m_ptReady[0].SetPoint(nWidth*24/80,nHeight/2-90+110-15);
	m_ptReady[1].SetPoint(nWidth/2,nHeight-250);
	m_ptReady[2].SetPoint(nWidth*56/80,nHeight/2-90+110-15);

	//����λ��
	m_ptPass[0].SetPoint(nWidth*24/80,nHeight/2-90+110-15);
	m_ptPass[1].SetPoint(nWidth/2,nHeight-315+90);
	m_ptPass[2].SetPoint(nWidth*56/80,nHeight/2-90+110-15);

	//�з�λ��
	m_ptCallScore[0].SetPoint(nWidth*24/80,nHeight/2-90-15);
	m_ptCallScore[1].SetPoint(nWidth/2,nHeight-315);
	m_ptCallScore[2].SetPoint(nWidth*56/80,nHeight/2-90-15);

	//ׯ�ұ�־
	m_ptBanker[0].SetPoint(6,278+15-30);
	m_ptBanker[1].SetPoint(0,nHeight-176);
	m_ptBanker[2].SetPoint(nWidth-153-7-9,278+15-30);

	//����λ��
	m_ptCountWarn[0].SetPoint(25+46-30+182+5+33,m_ptAvatar[0].y+150+145-228-38-102+140);
	m_ptCountWarn[1].SetPoint(nWidth/2-322+155,nHeight-118);
	m_ptCountWarn[2].SetPoint(nWidth-60-73-109-35-33,m_ptAvatar[2].y+150+145-228-38-102+140);

	//�ȼ�λ��
	m_ptOrderFlag[0].SetPoint(61,nHeight/2-160-15);
	m_ptOrderFlag[1].SetPoint(nWidth/2-359,nHeight-58);
	m_ptOrderFlag[2].SetPoint(nWidth-27,nHeight/2-160-15);

	//����λ��
	m_UserCardControl[0].SetDispatchPos(160,nHeight/2-95+145-15);
	m_UserCardControl[1].SetDispatchPos(nWidth/2,nHeight-100);
	m_UserCardControl[2].SetDispatchPos(nWidth-115-90,nHeight/2-95+169-15);

	//�����˿�
	m_BackCardControl.SetBenchmarkPos(nWidth/2,51+8-30,enXCenter,enYTop);

	//�û��˿�
	m_HandCardControl[0].SetBenchmarkPos(160+77+35,nHeight/2-95+144+30-15,enXLeft,enYCenter);
	m_HandCardControl[1].SetBenchmarkPos(nWidth/2,nHeight-70,enXCenter,enYBottom);
	m_HandCardControl[2].SetBenchmarkPos(nWidth-160-38-50+30+20-55,nHeight/2-95+142+30-15,enXRight,enYCenter);
	//�����˿�
	m_ChetCardContol[0].SetBenchmarkPos(160+77+35-90,nHeight/2-95+144+30-15,enXLeft,enYBottom);
	m_ChetCardContol[1].SetBenchmarkPos(nWidth/2,nHeight-70,enXCenter,enYBottom);
	m_ChetCardContol[2].SetBenchmarkPos(nWidth-160-38-55+70,nHeight/2-95+142+30-15,enXRight,enYBottom);

	//�����˿�
	m_UserCardControl[0].SetBenchmarkPos(160+77+120-160+43,nHeight/2-95+144-160+150-15-15-10,enXLeft,enYCenter);
	m_UserCardControl[1].SetBenchmarkPos(nWidth/2-5,nHeight-320+28+45-20-10,enXCenter,enYCenter);
	m_UserCardControl[2].SetBenchmarkPos(nWidth-160-38-120+33+8+43,nHeight/2-95+142-158+150-15-15-10,enXRight,enYCenter);

	//����λ��
	m_ptChatBubble[0].SetPoint(80,220-30+35);
	m_ptChatBubble[1].SetPoint(134-46,nHeight-182-64+35);
	m_ptChatBubble[2].SetPoint(nWidth-245,220-30+35);

	//�й�λ��
	m_ptMachine[0].SetPoint(140,320-30);
	m_ptMachine[1].SetPoint(200,nHeight-115);
	m_ptMachine[2].SetPoint(nWidth-190,320-30);

	//���ⰴť
	if (m_bInitD3D==true)
	{
		//��ְ�ť
		CRect rcScore;
		m_btScore.GetClientRect(rcScore);
		m_btScore.SetWindowPos(nWidth-rcScore.Width()-5,8+26-30,0,0,SWP_NOSIZE);

		//��ʼ��ť
		CRect rcStart;
		m_btStart.GetClientRect(rcStart);
		m_btStart.SetWindowPos((nWidth-rcStart.Width())/2,nHeight-rcStart.Height()-195,0,0,SWP_NOSIZE);

		//��Ϸ��ť
		CRect rcGameButton;
		m_btOutCard.GetClientRect(rcGameButton);
		m_btPassCard.SetWindowPos(nWidth/2-rcGameButton.Width()*3/2-20,nHeight-rcGameButton.Height()-210,0,0,SWP_NOSIZE);
		m_btOutPrompt.SetWindowPos(nWidth/2-rcGameButton.Width()/2,nHeight-rcGameButton.Height()-210,0,0,SWP_NOSIZE);
		m_btOutCard.SetWindowPos(nWidth/2+rcGameButton.Width()/2+20,nHeight-rcGameButton.Height()-210,0,0,SWP_NOSIZE);

		CRect rcGameEndButton;
		m_btPlayAgain.GetClientRect(rcGameEndButton);
		m_btPlayAgain.SetWindowPos(nWidth/2-rcGameEndButton.Width()-15,nHeight-240+40+42,0,0,SWP_NOSIZE);
		m_btReturn.SetWindowPos(nWidth/2+15,nHeight-240+40+42,0,0,SWP_NOSIZE);
		
		//�зְ�ť
		CRect rcCallButton;
		m_btCallScore1.GetClientRect(rcCallButton);
		m_btCallScore1.SetWindowPos(nWidth/2-rcCallButton.Width()*2-30,nHeight-rcCallButton.Height()-210,0,0,SWP_NOSIZE);
		m_btCallScore2.SetWindowPos(nWidth/2-rcCallButton.Width()-10,nHeight-rcCallButton.Height()-210,0,0,SWP_NOSIZE);
		m_btCallScore3.SetWindowPos(nWidth/2+10,nHeight-rcCallButton.Height()-210,0,0,SWP_NOSIZE);
		m_btCallScoreNone.SetWindowPos(nWidth/2+rcCallButton.Width()+30,nHeight-rcCallButton.Height()-210,0,0,SWP_NOSIZE);

		//���ܰ�ť
		CRect rcTrustee;
		m_btTrustee.GetClientRect(rcTrustee);
		m_btSortCard.SetWindowPos(420+0*85,nHeight-37,0,0,SWP_NOSIZE);
		m_btLastTurn.SetWindowPos(420+1*85,nHeight-37,0,0,SWP_NOSIZE);
		m_btTrustee.SetWindowPos(420+2*85,nHeight-37,0,0,SWP_NOSIZE);
		m_btCancelTrustee.SetWindowPos(420+2*85,nHeight-37,0,0,SWP_NOSIZE);
		m_btHistoryChat.SetWindowPos(nWidth-300+13,nHeight-33,0,0,SWP_NOSIZE);

	//	m_btExpression.SetWindowPos(358+1*79,nHeight-37,0,0,SWP_NOSIZE);
	//	m_btVoice.SetWindowPos(358,nHeight-37,0,0,SWP_NOSIZE);
		m_btEnter.SetWindowPos(nWidth-178+45+7+81,nHeight-33,0,0,SWP_NOSIZE);
	//	m_btSwitchSence.SetWindowPos(nWidth-119+43,nHeight-37,0,0,SWP_NOSIZE);

		//�ɼ�����
		if (m_ScoreControl.IsWindowActive()==true)
		{
			CRect rcScoreControl;
			m_ScoreControl.GetWindowRect(rcScoreControl);
			m_ScoreControl.SetWindowPos((nWidth-rcScoreControl.Width())/2,(nHeight-rcScoreControl.Height())*2/5-50,0,0,SWP_NOSIZE);
		}

		HDWP hDwp=BeginDeferWindowPos(32);
		DeferWindowPos(hDwp, m_ChatInput, NULL, nWidth-263+12,nHeight-32,200+5,23, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
		DeferWindowPos(hDwp,m_ChatMessage,NULL,nWidth-261,nHeight-167-4,236-1+14,120+4+2, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
		//�����ƶ�
		EndDeferWindowPos(hDwp);

	}

	return;
}

//��������
VOID CGameClientView::CartoonMovie()
{
	//�˿˶���
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].CartoonMovie();
		m_UserCardControl[i].CartoonMovie();
	}

	//�ɻ�����
	if ((m_bCartoonPlan==true)&&(m_CartoonPlanLapseCount.GetLapseCount(10)>0L))
	{
		//���ñ���
		m_wCartoonPlanIndex++;

		//����λ��	
		m_ptCartoonPlan.x -= 30;

		//ֹͣ�ж�
		if (m_ptCartoonPlan.x < -200 )
		{
			//���ñ���
			m_bCartoonPlan=false;
			m_wCartoonPlanIndex=0L;

			m_ptCartoonPlan.SetPoint( 0, 0 );

			//ж����Դ
			ASSERT(m_TextureCartoonPlan.IsNull()==false);
			if (m_TextureCartoonPlan.IsNull()==false) m_TextureCartoonPlan.Destory();
		}
	}

	//�������
	if ((m_bCartoonRocket==true)&&(m_CartoonRocketLapseCount.GetLapseCount(30)>0L))
	{
		//���ñ���
		if(m_wCartoonRocketIndex<30)
			m_wCartoonRocketIndex++;

		//����λ��
		if(m_wCartoonRocketIndex >=7)
			m_ptCartoonRocket.y -= 50;

		//ֹͣ�ж�
		if (m_ptCartoonRocket.y < -550 )
		{
			//���ñ���
			m_bCartoonRocket=false;
			m_wCartoonRocketIndex=0L;

			m_ptCartoonRocket.SetPoint( 0, 0 );

			//ж����Դ
			ASSERT(m_TextureCartoonRocket.IsNull()==false&& m_TextureRocket.IsNull() == false && m_TextureRocketLight.IsNull() == false);
			if (m_TextureCartoonRocket.IsNull()==false) m_TextureCartoonRocket.Destory();
			if (m_TextureRocket.IsNull()==false) m_TextureRocket.Destory();
			if (m_TextureRocketLight.IsNull()==false) m_TextureRocketLight.Destory();

		}
	}

	//��ը����
	if ((m_bCartoonBomb==true)&&(m_CartoonBombLapseCount.GetLapseCount(100)>0L))
	{
		//���ñ���
		m_wCartoonBombIndex++;

		//ֹͣ�ж�
		if (m_wCartoonBombIndex>=BOMB_CARTOON_COUNT)
		{
			//���ñ���
			m_bCartoonBomb=false;
			m_wCartoonBombIndex=0L;

			//ж����Դ
			ASSERT(m_TextureCartoonBomb.IsNull()==false);
			if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();
		}
	}

	//˳�Ӷ���
	if((m_bCartoonLine == true)&&(m_CartoonLineLapseCount.GetLapseCount(80)>0l))
	{
		//���ñ���
		m_wCartoonLineIndex++;

		//ֹͣ�ж�
		if(m_wCartoonLineIndex >= LINE_CARTOON_COUNT)
		{
			//���ñ���
			m_bCartoonLine = false;
			m_wCartoonLineIndex = 0L;

			//ж����Դ
			ASSERT(m_TextureCartoonLine.IsNull() == false && m_TextureLine.IsNull() == false);
			if(m_TextureCartoonLine.IsNull() == false && m_TextureLine.IsNull() == false)
			{
				m_TextureCartoonLine.Destory();
				m_TextureLine.Destory();
			}
		}
	}

	//���ƶ���
	if ((m_bReversalCard==true)&&(m_ReversalCardLapseCount.GetLapseCount(180)>0L))
	{
		//���ñ���
		m_wReversalCardIndex++;

		//ֹͣ�ж�
		if (m_wReversalCardIndex>=REVERSAL_CARD_COUNT)
		{
			//���ñ���
			m_bReversalCard=false;
			m_wReversalCardIndex=0L;

			//�����˿�
			m_BackCardControl.SetShowCount(MAX_CARD_COUNT);

			//ж����Դ
			ASSERT(m_TextureReversalCard.IsNull()==false);
			if (m_TextureReversalCard.IsNull()==false) m_TextureReversalCard.Destory();

			//������Ϣ
			SendEngineMessage(IDM_REVERSAL_FINISH,0L,0L);
		}
	}

	//ׯ�Ҷ���
	if ((m_wBankerUser!=INVALID_TABLE)&&(m_BankerMovieLapseCount.GetLapseCount(500)>0L))
	{
		//���ñ���
		m_wBankerMovieIndex=(m_wBankerMovieIndex+1)%BANKER_MOVIE_COUNT;
	}

	//ϴ�ƶ���
	if ((m_bRiffleCard==true)&&(m_DispatchCardLapseCount.GetLapseCount(100)>0L))
	{
		//���ñ���
		m_wDispatchCardIndex++;

		//ֹͣ�ж�
		if (m_wDispatchCardIndex>=(RIFFLE_CARD_COUNT_ALL+6))
		{
			//���ñ���
			m_bRiffleCard=false;
			m_bDispatchCard=true;
			m_wDispatchCardIndex=0L;

			//��������
			PlayGameSound(AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"));
		}
	}

	//���ƶ���
	if ((m_bDispatchCard==true)&&(m_DispatchCardLapseCount.GetLapseCount(10)>0L))
	{
		//���ñ���
		m_wDispatchCardIndex++;

		//��������
		WORD wBaseIndex=DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES;

		//�����˿�
		if ((m_wDispatchCardIndex>=wBaseIndex)&&((m_wDispatchCardIndex-wBaseIndex)%DISPATCH_CARD_SPACE==0))
		{
			//��������
			WORD wCardIndex=m_wDispatchCardIndex-wBaseIndex;
			WORD wUserIndex=(m_wStartUser+wCardIndex/DISPATCH_CARD_SPACE)%GAME_PLAYER;

			//�����˿�
			if (wUserIndex==MYSELF_VIEW_ID || m_bUserCheat[wUserIndex])
			{
				//��ȡ�˿�
				BYTE cbCardData[MAX_COUNT];
				BYTE cbCardCount=m_HandCardControl[wUserIndex].GetCardCount() + 1;

				//�����˿�
				//cbCardData[cbCardCount]=m_cbHandCardData[wUserIndex][cbCardCount++];
				CopyMemory(cbCardData,m_cbHandCardData,cbCardCount*sizeof(BYTE));
				m_GameLogic.SortCardList(cbCardData,cbCardCount,ST_ORDER);

				//�����˿�
				m_HandCardControl[wUserIndex].SetCardData(cbCardData,cbCardCount);
				m_ChetCardContol[wUserIndex].SetCardData(cbCardData,cbCardCount);
			}
			else
			{
				//����˿�
				m_HandCardControl[wUserIndex].SetCardData(m_HandCardControl[wUserIndex].GetCardCount()+1);
			}
		}

		//ֹͣ�ж�
		if (m_wDispatchCardIndex>=DISPATCH_COUNT*DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES)
		{
			//���ñ���
			m_bDispatchCard=false;
			m_wDispatchCardIndex=0L;

			//ж����Դ
			ASSERT(m_TextureDispatchCard.IsNull()==false);
			if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();

			//������Ϣ
			SendEngineMessage(IDM_DISPATCH_FINISH,0L,0L);
		}
	}

	//��������
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if ((m_bCountWarn[i]==true)&&(m_CountWarnLapseCount[i].GetLapseCount(300)>0L))
		{
			//���ñ���
			m_wCountWarnIndex[i]=(m_wCountWarnIndex[i]+1)%2/*CARD_WARNING_COUNT*/;
		}
	}

	return;
}

//�����豸
VOID CGameClientView::InitGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight)
{
	//��������
	CRect rcCreate(0,0,0,0);
	HINSTANCE hResInstance=AfxGetInstanceHandle();
//
	//��Ϸ��ť
	m_btStart.ActiveWindow(rcCreate,0,IDC_BT_START,&m_VirtualEngine,NULL);
	m_btScore.ActiveWindow(rcCreate,0,IDC_BT_SCORE,&m_VirtualEngine,NULL);
	m_btOutCard.ActiveWindow(rcCreate,0,IDC_BT_OUT_CARD,&m_VirtualEngine,NULL);
	m_btPassCard.ActiveWindow(rcCreate,0,IDC_BT_PASS_CARD,&m_VirtualEngine,NULL);
	m_btOutPrompt.ActiveWindow(rcCreate,0,IDC_BT_OUT_PROMPT,&m_VirtualEngine,NULL);
	m_btPlayAgain.ActiveWindow(rcCreate,0,IDC_BT_PLAY_AGAIN,&m_VirtualEngine,NULL);
	m_btReturn.ActiveWindow(rcCreate,0,IDC_BT_RETURN,&m_VirtualEngine,NULL);
	m_btHistoryChat.ActiveWindow(rcCreate,WS_VISIBLE,IDC_HISTORY_CHAT,&m_VirtualEngine,NULL);

	//�зְ�ť
	m_btCallScore1.ActiveWindow(rcCreate,0,IDC_CALL_SCORE_1,&m_VirtualEngine,NULL);
	m_btCallScore2.ActiveWindow(rcCreate,0,IDC_CALL_SCORE_2,&m_VirtualEngine,NULL);
	m_btCallScore3.ActiveWindow(rcCreate,0,IDC_CALL_SCORE_3,&m_VirtualEngine,NULL);
	m_btCallScoreNone.ActiveWindow(rcCreate,0,IDC_CALL_SCORE_NONE,&m_VirtualEngine,NULL);

	//���ܰ�ť
	m_btCancelTrustee.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_BT_CANCEL_TRUSTEE,&m_VirtualEngine,NULL);
	m_btTrustee.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_TRUSTEE,&m_VirtualEngine,NULL);
	m_btLastTurn.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_BT_LAST_TURN,&m_VirtualEngine,NULL);
	m_btSortCard.ActiveWindow(rcCreate,WS_VISIBLE|WS_DISABLED,IDC_BT_SORT_CARD,&m_VirtualEngine,NULL);
	m_btEnter.ActiveWindow(rcCreate,WS_VISIBLE,IDC_BT_ENTER,&m_VirtualEngine,NULL);
	m_btCancelTrustee.ShowWindow(false);

	//��Ϸ��ť
	m_btStart.SetButtonImage(pD3DDevice,TEXT("BT_START"),TEXT("PNG"),hResInstance);
	m_btScore.SetButtonImage(pD3DDevice,TEXT("BT_SHOW_SCORE"),TEXT("PNG"),hResInstance);
	m_btOutCard.SetButtonImage(pD3DDevice,TEXT("BT_OUT_CARD"),TEXT("PNG"),hResInstance);
	m_btPassCard.SetButtonImage(pD3DDevice,TEXT("BT_PASS_CARD"),TEXT("PNG"),hResInstance);
	m_btOutPrompt.SetButtonImage(pD3DDevice,TEXT("BT_OUT_PROMPT"),TEXT("PNG"),hResInstance);
	m_btPlayAgain.SetButtonImage(pD3DDevice,TEXT("BT_PLAY_AGAIN"),TEXT("PNG"),hResInstance);
	m_btReturn.SetButtonImage(pD3DDevice,TEXT("BT_RETURN"),TEXT("PNG"),hResInstance);
	m_btHistoryChat.SetButtonImage(pD3DDevice,TEXT("BT_HIDE_CHAT"),TEXT("PNG"),hResInstance);


	//���ܰ�ť
	m_btLastTurn.SetButtonImage(pD3DDevice,TEXT("BT_LAST_TURN"),TEXT("PNG"),hResInstance);
	m_btTrustee.SetButtonImage(pD3DDevice,TEXT("BT_START_TRUSTEE"),TEXT("PNG"),hResInstance);
	m_btCancelTrustee.SetButtonImage(pD3DDevice,TEXT("BT_STOP_TRUSTEE"),TEXT("PNG"),hResInstance);
	m_btSortCard.SetButtonImage(pD3DDevice,TEXT("BT_SORT_CARD_COUNT"),TEXT("PNG"),hResInstance);
	m_btEnter.SetButtonImage(pD3DDevice,TEXT("BT_ENTER"),TEXT("PNG"),hResInstance);

	//�зְ�ť
	m_btCallScore1.SetButtonImage(pD3DDevice,TEXT("BT_CALL_SCORE_1"),TEXT("PNG"),hResInstance);
	m_btCallScore2.SetButtonImage(pD3DDevice,TEXT("BT_CALL_SCORE_2"),TEXT("PNG"),hResInstance);
	m_btCallScore3.SetButtonImage(pD3DDevice,TEXT("BT_CALL_SCORE_3"),TEXT("PNG"),hResInstance);
	m_btCallScoreNone.SetButtonImage(pD3DDevice,TEXT("BT_CALL_SCORE_NONE"),TEXT("PNG"),hResInstance);

	//��������
	m_ChatInput.Create(ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER, rcCreate, this, IDC_CHAT_INPUT);
	m_ChatMessage.Create(ES_AUTOVSCROLL|ES_WANTRETURN|ES_MULTILINE|WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_VSCROLL, rcCreate, this, IDC_CHAT_DISPLAY);
	m_brush.CreateSolidBrush(RGB(101,31,2));
	m_ChatInput.LimitText(24);
	m_ChatMessage.SetBackgroundColor(false,RGB(14,43,33));
	m_ChatMessage.SetExpressionManager(CExpressionManager::GetInstance(),RGB(14,43,33));
	m_ChatMessage.SetReadOnly(TRUE);
	
	IClientKernel * m_pIClientKernel;
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
	//��ѯ�ӿ�
	ASSERT(pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel)!=NULL);
	m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
	m_pIClientKernel->SetStringMessage(QUERY_OBJECT_INTERFACE(m_ChatMessage,IStringMessage));

	//�ؼ�����
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));      
	lf.lfHeight = 15;
	wcscpy(lf.lfFaceName, TEXT("Arial"));
	m_ChatInput.SetLimitText(30);
	CFont m_font;
	m_font.CreateFontIndirect(&lf);

	//�󶨿ؼ�
	m_ChatMessage.SetFont(&m_font);

	//�˿���Դ
	CCardControl::LoadCardResource(pD3DDevice);

	//������Դ
	m_TextureBack.LoadImage(pD3DDevice,hResInstance,TEXT("VIEW_BACK_2"),TEXT("PNG"));
	m_TextureBackButtom.LoadImage(pD3DDevice,hResInstance,TEXT("VIEW_BACK_BUTTOM"),TEXT("PNG"));
	m_TextureUserPlayer.LoadImage(pD3DDevice,hResInstance,TEXT("USER_FRAME_PLAYER"),TEXT("PNG"));
	m_TextureUserMySelf.LoadImage(pD3DDevice,hResInstance,TEXT("USER_FRAME_MYSELF"),TEXT("PNG"));
//	m_TextureMemberOrder.LoadImage(pD3DDevice,hResInstance,TEXT("MEMBER_FLAG"),TEXT("PNG"));
	m_TextureChatBubble.LoadImage(pD3DDevice,hResInstance,TEXT("CHAT_BUBBLE"),TEXT("PNG"));
	m_TextureGameScoreBack.LoadImage(pD3DDevice,hResInstance,TEXT("GAME_SCORE_BACK"),TEXT("PNG"));
	m_TextureChatBack.LoadImage(pD3DDevice,hResInstance,TEXT("CHAT_BACK"),TEXT("PNG"));

	//��������
	m_TextureCellNumber.LoadImage(pD3DDevice,hResInstance,TEXT("CELL_NUMBER"),TEXT("PNG"));
	m_TextureBankerFlag[0].LoadImage(pD3DDevice,hResInstance,TEXT("FLAG_BANKER_PLAYER_1"),TEXT("PNG"));
	m_TextureBankerFlag[1].LoadImage(pD3DDevice,hResInstance,TEXT("FLAG_BANKER_MYSELF"),TEXT("PNG"));
	m_TextureBankerFlag[2].LoadImage(pD3DDevice,hResInstance,TEXT("FLAG_BANKER_PLAYER_2"),TEXT("PNG"));
	m_TextureCardCountFrame1.LoadImage(pD3DDevice,hResInstance,TEXT("CARD_COUNT_FRAME_1"),TEXT("PNG"));
	m_TextureCardCountFrame2.LoadImage(pD3DDevice,hResInstance,TEXT("CARD_COUNT_FRAME_2"),TEXT("PNG"));

	m_TextureRemnantCardNumber.LoadImage(pD3DDevice,hResInstance,TEXT("REMNANT_CARD_NUMBER"),TEXT("PNG"));
	m_TextureMachine.LoadImage(pD3DDevice,hResInstance,TEXT("MACHINE"),TEXT("PNG"));

	////��������
	m_FontChat.CreateFont(pD3DDevice,12,400,TEXT("����"));

	//ʱ������
	m_TextureTimeBack.LoadImage(pD3DDevice,hResInstance,TEXT("TIME_BACK"),TEXT("PNG"));
	m_TextureTimeNumber.LoadImage(pD3DDevice,hResInstance,TEXT("TIME_NUMBER"),TEXT("PNG"));
	m_TextureCartoonTime.LoadImage(pD3DDevice,hResInstance,TEXT("CARTOON_TIME"),TEXT("PNG"));
	m_TextureTimeLight.LoadImage(pD3DDevice,hResInstance,TEXT("TIME_LIGHT"),TEXT("PNG"));


	//�����˿�
	m_BackCardControl.SetSmallMode(true);
	m_BackCardControl.SetCardType(CARD_TYPE_SMALL);
	m_BackCardControl.SetDisplayItem(true);
	m_BackCardControl.SetBackGround(CARD_LAND);
	m_BackCardControl.SetCardDistance(73L,0L,0L);

	//�����˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//����˿�
		if (i==0)
		{
			m_HandCardControl[i].SetSmallMode(true);
			m_HandCardControl[i].SetDirection(false);
			m_HandCardControl[i].SetCardDistance(-6,10,0);
			m_HandCardControl[i].SetCardType(CARD_TYPE_LEFT);
			m_HandCardControl[i].Setpartition(50,40);

			m_UserCardControl[i].SetSmallMode(true);
			m_UserCardControl[i].SetDirection(true);
			m_UserCardControl[i].SetCardDistance(15,0,0);
			m_UserCardControl[i].SetCardType(CARD_TYPE_OUT);
			m_UserCardControl[i].Setpartition(50,40);
			m_UserCardControl[i].SetDisplayItem(true);
		}
		if(i == 2)
		{
			m_HandCardControl[i].SetSmallMode(true);
			m_HandCardControl[i].SetDirection(false);
			m_HandCardControl[i].SetCardDistance(5,10,0);
			m_HandCardControl[i].SetCardType(CARD_TYPE_RIGHT);
			m_HandCardControl[i].Setpartition(30,50);

			m_UserCardControl[i].SetSmallMode(true);
			m_UserCardControl[i].SetDirection(true);
			m_UserCardControl[i].SetCardDistance(15,0,0);
			m_UserCardControl[i].SetCardType(CARD_TYPE_OUT);
			m_UserCardControl[i].Setpartition(30,50);
			m_UserCardControl[i].SetDisplayItem(true);
		}

		//�û��˿�
		if (i==MYSELF_VIEW_ID)
		{
			m_HandCardControl[i].SetDirection(true);
			m_HandCardControl[i].SetSmallMode(false);
			m_HandCardControl[i].SetCardDistance(18,DEF_Y_DISTANCE,DEF_SHOOT_DISTANCE);
			m_HandCardControl[i].SetCardType(CARD_TYPE_NOMAL);
			m_HandCardControl[i].SetDisplayItem(true);
			m_UserCardControl[i].SetCardDistance(15,15,DEF_SHOOT_DISTANCE);
			m_UserCardControl[i].SetCardType(CARD_TYPE_OUT);
			m_UserCardControl[i].SetDisplayItem(true);

			m_HandCardControl[i].SetShowCount(MAX_CARD_COUNT);
		}
		m_HandCardControl[i].SetDrawOrder(true);
		m_UserCardControl[i].SetDrawOrder(true);
		m_UserCardControl[i].SetShowCount(MAX_CARD_COUNT);

		m_HandCardControl[i].SetDisplayItem(true);
		m_UserCardControl[i].SetDisplayItem(true);
		
		m_ChetCardContol[i].SetDirection(true);
		m_ChetCardContol[i].SetSmallMode(true);
		m_ChetCardContol[i].SetCardType(CARD_TYPE_SMALL);
		m_ChetCardContol[i].SetDisplayItem(true);
		m_ChetCardContol[i].SetCardDistance(15,DEF_Y_DISTANCE,0);
	}


	//������Ⱦ
	StartRenderThread();

	return;
}

//��ʧ�豸
VOID CGameClientView::OnLostDevice(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight)
{
	m_FontChat.DeleteFont();

	return;
}

//�����豸
VOID CGameClientView::OnResetDevice(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight)
{
	////��������
	m_FontChat.CreateFont(pD3DDevice,12,400,TEXT("����"));

	return;
}

//�滭����
VOID CGameClientView::DrawGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight)
{
	
	//��ȡ��С
	CSize SizeBack;
	CSize SizeUserUserMySelf;

	SizeUserUserMySelf.SetSize(m_TextureUserMySelf.GetWidth(),m_TextureUserMySelf.GetHeight());

	//�滭����
	DrawViewImage(pD3DDevice,m_TextureBack,DRAW_MODE_ELONGGATE);
	m_TextureBack.DrawImage(pD3DDevice,0,0);
	m_TextureBackButtom.DrawImage(pD3DDevice,0,nHeight-m_TextureBackButtom.GetHeight());

	//ũ��������������·�
	if(m_wBankerUser!= INVALID_CHAIR)
	{
		for(int i =0 ; i <GAME_PLAYER ;++i)
		{
			if(m_wBankerUser!=i)
			{
				//��ȡ��С
				CSize SizeBankerFlag;
				SizeBankerFlag.SetSize(m_TextureBankerFlag[i].GetWidth()/2,m_TextureBankerFlag[i].GetHeight());

				m_TextureBankerFlag[i].DrawImage(pD3DDevice,m_ptBanker[i].x,m_ptBanker[i].y,SizeBankerFlag.cx,SizeBankerFlag.cy,
					SizeBankerFlag.cx,0);
			}

		}
	}

	//�з�
	TCHAR szOutNumT[128] = {0};
	if ( m_cbBankerScore != 0 && m_cbBankerScore != 255 )
	{
		_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%d"),m_cbBankerScore);
		DrawNumber(pD3DDevice,&m_TextureCellNumber, TEXT("0123456789"),szOutNumT,(nWidth)/2 -42 ,136+27-30, DT_CENTER);
	}

	//�滭��λ����
	_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%d"),m_lCellScore);
	int nOutNumber = lstrlen(szOutNumT);
	if ( nOutNumber > 3 )
	{
		DrawNumber(pD3DDevice,&m_TextureCellNumber, TEXT("0123456789"),szOutNumT,(nWidth)/2 -30 -20,140+3-30, DT_LEFT);
	}
	else
	{
		DrawNumber(pD3DDevice,&m_TextureCellNumber, TEXT("0123456789"),szOutNumT,(nWidth)/2 -42 ,140+3-30, DT_CENTER);
	}

	//��ҿ��
	if(m_bCellScoreEnd == false)
	{
		m_TextureUserPlayer.DrawImage(pD3DDevice,m_ptAvatar[0].x,m_ptAvatar[0].y);
		m_TextureUserPlayer.DrawImage(pD3DDevice,m_ptAvatar[2].x,m_ptAvatar[2].y);
		m_TextureUserMySelf.DrawImage(pD3DDevice,m_ptAvatar[1].x,m_ptAvatar[1].y);
	}
	else
	{
		m_TextureUserPlayerGame.DrawImage(pD3DDevice,m_ptAvatar[0].x-3,m_ptAvatar[0].y+190-103);
		m_TextureUserPlayerGame.DrawImage(pD3DDevice,m_ptAvatar[2].x+67,m_ptAvatar[2].y+190-103);
	}

	//�����˿�
	m_BackCardControl.DrawCardControl(pD3DDevice);

	
	//�滭�û�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��ȡ�û�
		IClientUserItem * pIClientUserItem=GetClientUserItem(i);
		if (pIClientUserItem==NULL) continue;

		//�û�����
		if(!m_bChangeTimePos)
			DrawUserTimerEx(pD3DDevice,m_ptClock[i].x,m_ptClock[i].y,GetUserClock(i));
		else
			DrawUserTimerEx(pD3DDevice,225+44,nHeight-173+19,GetUserClock(i));

		if(m_bCellScoreEnd == false)
			DrawUserAvatar(pD3DDevice,m_ptAvatar[i].x+6,m_ptAvatar[i].y+6,pIClientUserItem);


		//λ�ö���
		CRect rcNickName,rcUserScore,rcUserLevel;
		if(i == MYSELF_VIEW_ID)
		{
			rcNickName.SetRect(m_ptNickName[i].x,m_ptNickName[i].y,m_ptNickName[i].x+72,m_ptNickName[i].y+12);
			rcUserScore.SetRect(m_ptNickName[i].x+120,m_ptNickName[i].y,m_ptNickName[i].x+195,m_ptNickName[i].y+29);
		}
		else		//�зֽ���
		{
			if(m_bCellScoreEnd == false)
			{
				rcNickName.SetRect(m_ptNickName[i].x,m_ptNickName[i].y,m_ptNickName[i].x+75,m_ptNickName[i].y+12);
				rcUserScore.SetRect(m_ptNickName[i].x,m_ptNickName[i].y+20+3+2,m_ptNickName[i].x+75,m_ptNickName[i].y+32+3+2);
			}
			else
			{
				rcNickName.SetRect(m_ptNickName[i].x+10-((i==2)?0:70),m_ptNickName[i].y+86,m_ptNickName[i].x+85-((i==2)?0:70),m_ptNickName[i].y+97);
				rcUserScore.SetRect(m_ptNickName[i].x+10-((i==2)?0:70),m_ptNickName[i].y+105,m_ptNickName[i].x+85-((i==2)?0:70),m_ptNickName[i].y+117);
			}

		}

		//�滭�û�
		LPCTSTR pszNickName=pIClientUserItem->GetNickName();
		DrawTextString(pD3DDevice,pszNickName,rcNickName,DT_CENTER|DT_END_ELLIPSIS,D3DCOLOR_XRGB(153,255,0),D3DCOLOR_XRGB(0,50,0));

		//�û�����
		TCHAR szUserScore[64]=TEXT("");
		_sntprintf(szUserScore,CountArray(szUserScore),SCORE_STRING,pIClientUserItem->GetUserScore());
		CString strScore = AddComma(pIClientUserItem->GetUserScore());
		DrawTextString(pD3DDevice,strScore,rcUserScore,DT_CENTER|DT_END_ELLIPSIS,D3DCOLOR_XRGB(255,255,0),D3DCOLOR_XRGB(0,50,0));
		
		if(m_bCellScoreEnd == true)
		{
			//ׯ�ұ�־
			if (m_wBankerUser!=INVALID_CHAIR && (m_wBankerUser ==i || i == MYSELF_VIEW_ID))
			{
				//��ȡ��С
				CSize SizeBankerFlag;
				SizeBankerFlag.SetSize(m_TextureBankerFlag[i].GetWidth()/2,m_TextureBankerFlag[i].GetHeight());

				m_TextureBankerFlag[i].DrawImage(pD3DDevice,m_ptBanker[i].x,m_ptBanker[i].y,SizeBankerFlag.cx,SizeBankerFlag.cy,
					SizeBankerFlag.cx*((i==m_wBankerUser)?0:1),0);
			}

		}

		//�йܱ��
		if(m_bUserTrustee[i] == true)
			m_TextureMachine.DrawImage(pD3DDevice,m_ptMachine[i].x,m_ptMachine[i].y);
	}

	//�滭�˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(i!=MYSELF_VIEW_ID && m_bUserCheat[i])
		{
			m_ChetCardContol[i].DrawCardControl(pD3DDevice);
		}
		else
		{
			m_HandCardControl[i].DrawCardControl(pD3DDevice);
		}
		m_UserCardControl[i].DrawCardControl(pD3DDevice);
	}

	//ʣ���˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (i!=MYSELF_VIEW_ID)
		{
			//��������
			WORD wCardCount=m_HandCardControl[i].GetCardCount();
			INT nXDistance = i==0?-6:3;

			//�滭����
			if (wCardCount>0)
			{
				if(i != m_wBankerUser)
					m_TextureCardCountFrame1.DrawImage(pD3DDevice,(i==0?145:nWidth-210),nHeight/2+50+10);
				else
					m_TextureCardCountFrame2.DrawImage(pD3DDevice,(i==0?145:nWidth-210),nHeight/2+50+10);
				DrawNumberString(pD3DDevice,m_TextureRemnantCardNumber,wCardCount,(i==0?186+6:nWidth-169+6),nHeight/2+55+17+10);
			}
		}
	}
	
	//��������
	for(int i = 0 ; i < GAME_PLAYER ;++i)
	{
		if((m_strChat[i]!= TEXT("")) &&  (m_ChatChair[i]!= INVALID_CHAIR))
		{
			UINT nFormat = DT_CENTER|DT_TOP|DT_END_ELLIPSIS;
			INT nYoffset = 0;
			if(m_nChatStrlen[i]<24)
				nYoffset = 8;
			if(m_bCellScoreEnd == false)
			{
				m_TextureChatBubble.DrawImage(pD3DDevice,m_ptChatBubble[i].x,m_ptChatBubble[i].y+85,m_TextureChatBubble.GetWidth()/2,m_TextureChatBubble.GetHeight(),m_TextureChatBubble.GetWidth()/2*(i == 2?1:0),0);
				CRect rcChat(m_ptChatBubble[i].x+5,m_ptChatBubble[i].y+5+85+nYoffset,m_ptChatBubble[i].x+155,m_ptChatBubble[i].y+120+nYoffset);
				m_FontChat.DrawText(m_strChat[i], rcChat, D3DCOLOR_XRGB(255,241,0), nFormat);
				//DrawTextString(pD3DDevice,m_strChat[i],rcChat,D3DCOLOR_XRGB(255,241,0), nFormat);
				//DrawTextString(pD3DDevice,m_strChat[i],&rcChat,nFormat,D3DCOLOR_XRGB(255,241,0));
			}
			else
			{
				m_TextureChatBubble.DrawImage(pD3DDevice,m_ptChatBubble[i].x,m_ptChatBubble[i].y,m_TextureChatBubble.GetWidth()/2,m_TextureChatBubble.GetHeight(),m_TextureChatBubble.GetWidth()/2*(i == 2?1:0),0);
				CRect rcChat(m_ptChatBubble[i].x+5,m_ptChatBubble[i].y+5+nYoffset,m_ptChatBubble[i].x+155,m_ptChatBubble[i].y+40+nYoffset);
				m_FontChat.DrawText(m_strChat[i], rcChat, D3DCOLOR_XRGB(255,241,0), nFormat);
				//DrawTextString(pD3DDevice,m_strChat[i],rcChat,D3DCOLOR_XRGB(255,241,0), nFormat);
				//DrawTextString(pD3DDevice,m_strChat[i],&rcChat,nFormat,D3DCOLOR_XRGB(255,241,0));
			}
		}
	}
	//�����¼����
	if(m_bShowChatMessage)
		m_TextureChatBack.DrawImage(pD3DDevice,nWidth-268,nHeight-178);
	//��Ϸ����
	if(m_bGameEnd == true)
	{
		//��������
		DrawViewImage(pD3DDevice,m_TextureGameScoreBack,DRAW_MODE_SPREAD);
	}
	//�û���־
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��ȡ�û�
		IClientUserItem * pIClientUserItem=GetClientUserItem(i);
		if (pIClientUserItem==NULL) continue;

		//ͬ���־
		BYTE cbUserStatus=pIClientUserItem->GetUserStatus();
		if (cbUserStatus==US_READY) DrawUserReady(pD3DDevice,m_ptReady[i].x,m_ptReady[i].y);
	}

	//����״̬
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bUserPass[i]==true)
		{
			CSize SizeUserPass;
			SizeUserPass.SetSize(m_TextureUserPass.GetWidth(),m_TextureUserPass.GetHeight());
			m_TextureUserPass.DrawImage(pD3DDevice,m_ptPass[i].x-SizeUserPass.cx/2,m_ptPass[i].y-SizeUserPass.cy/2);
		}
	}

	//�û��з�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_cbCallScore[i]!=0)
		{
			//��ȡ��С
			CSize SizeCallScore;
			SizeCallScore.SetSize(m_TextureCallScore.GetWidth()/4,m_TextureCallScore.GetHeight());

			//�滭��־
			INT nImageIndex=(m_cbCallScore[i]==0xFF)?3:(m_cbCallScore[i]-1);
			m_TextureCallScore.DrawImage(pD3DDevice,m_ptCallScore[i].x-SizeCallScore.cx/2,m_ptCallScore[i].y-SizeCallScore.cy/2,
				SizeCallScore.cx,SizeCallScore.cy,nImageIndex*SizeCallScore.cx,0);
		}
	}

	//���ֱ�־
	if (m_bLastTurnState==true)
	{
		CSize SizeLastTurn;
		SizeLastTurn.SetSize(m_TextureLastTurn.GetWidth(),m_TextureLastTurn.GetHeight());
		m_TextureLastTurn.DrawImage(pD3DDevice,(nWidth-SizeLastTurn.cx)/2,(nHeight-SizeLastTurn.cy)/2-100);
	}

	//�ȴ��з�
	if (m_bWaitCallScore==true)
	{
		CSize SizeWaitCallScore;
		SizeWaitCallScore.SetSize(m_TextureWaitCallScore.GetWidth(),m_TextureWaitCallScore.GetHeight());
		m_TextureWaitCallScore.DrawImage(pD3DDevice,(nWidth-SizeWaitCallScore.cx)/2,(nHeight-SizeWaitCallScore.cy)/2-135);
	}

	//�ɻ�����
	if (m_bCartoonPlan==true)
	{
		//��ȡ��С
		CSize SizeCartoonPlan;
		SizeCartoonPlan.SetSize(m_TextureCartoonPlan.GetWidth()/PLAN_CARTOON_COUNT,m_TextureCartoonPlan.GetHeight());

		//�滭Ч��
		m_TextureCartoonPlan.DrawImage(pD3DDevice,m_ptCartoonPlan.x,m_ptCartoonPlan.y - SizeCartoonPlan.cy/2,SizeCartoonPlan.cx,SizeCartoonPlan.cy,
			SizeCartoonPlan.cx*(m_wCartoonPlanIndex%PLAN_CARTOON_COUNT),0);
	}

	//�������
	if(m_bCartoonRocket == true)
	{
		//��ȡ��С
		CSize SizeRocket,SizeLight,SizeCartoonRocket;
		SizeRocket.SetSize(m_TextureRocket.GetWidth(),m_TextureRocket.GetHeight());
		SizeLight.SetSize(m_TextureRocketLight.GetWidth(),m_TextureRocketLight.GetHeight());
		SizeCartoonRocket.SetSize(m_TextureCartoonRocket.GetWidth()/10,m_TextureCartoonRocket.GetHeight());

		//�滭�������
		m_TextureRocket.DrawImage(pD3DDevice,m_ptCartoonRocket.x-SizeRocket.cx/2,m_ptCartoonRocket.y,SizeRocket.cx,SizeRocket.cy,0,0);
		m_TextureRocketLight.DrawImage(pD3DDevice,m_ptCartoonRocket.x-SizeLight.cx/2,m_ptCartoonRocket.y+325,SizeLight.cx,SizeLight.cy,0,0);

		//�滭����
		if(m_wCartoonRocketIndex<30)
			m_TextureCartoonRocket.DrawImage(pD3DDevice,m_ptCartoonRocket.x - SizeCartoonRocket.cx/2,nHeight - 250,SizeCartoonRocket.cx,SizeCartoonRocket.cy,
			SizeCartoonRocket.cx*(m_wCartoonRocketIndex/3),0);
	}

	//��ը����
	if (m_bCartoonBomb==true && m_wCartoonBombIndex >=0)
	{
		//��ȡ��С
		CSize SizeCartoonBomb;
		SizeCartoonBomb.SetSize(m_TextureCartoonBomb.GetWidth()/7,m_TextureCartoonBomb.GetHeight()/2);

		//�滭Ч��
		INT Ximage = m_wCartoonBombIndex%7;
		INT Yimage = m_wCartoonBombIndex/7;
		m_TextureCartoonBomb.DrawImage(pD3DDevice,(nWidth-SizeCartoonBomb.cx)/2,(nHeight-SizeCartoonBomb.cy)/2,SizeCartoonBomb.cx,SizeCartoonBomb.cy,
			SizeCartoonBomb.cx*Ximage,SizeCartoonBomb.cy*Yimage);
	}

	//˳�Ӷ���
	if(m_bCartoonLine == true)
	{
		//��ȡ��С
		CSize SizeCartoonLine;
		SizeCartoonLine.SetSize(m_TextureCartoonLine.GetWidth()/13,m_TextureCartoonLine.GetHeight());

		//�滭Ч��

		m_TextureLine.DrawImage(pD3DDevice,(nWidth-m_TextureLine.GetWidth())/2,(nHeight-m_TextureLine.GetHeight())/2,m_TextureLine.GetWidth(),m_TextureLine.GetHeight(),
			0,0);
		INT Ximage = m_wCartoonLineIndex;
		m_TextureCartoonLine.DrawImage(pD3DDevice,(nWidth-SizeCartoonLine.cx)/2,(nHeight-SizeCartoonLine.cy)/2,SizeCartoonLine.cx,SizeCartoonLine.cy,
			SizeCartoonLine.cx*Ximage,0);
	}

	//���ƶ���
	if (m_bReversalCard==true)
	{
		//��ȡ��С
		CSize SizeReversalCard;
		SizeReversalCard.SetSize(m_TextureReversalCard.GetWidth()/REVERSAL_CARD_COUNT,m_TextureReversalCard.GetHeight());

		//�滭Ч��
		for (BYTE i=0;i<3;i++)
		{
			m_TextureReversalCard.DrawImage(pD3DDevice,nWidth/2+i*83-102-6-7,51-30,SizeReversalCard.cx,SizeReversalCard.cy,
				SizeReversalCard.cx*(m_wReversalCardIndex),0);
		}
	}

	//ϴ�ƶ���
	if (m_bRiffleCard==true)
	{
		if (m_wDispatchCardIndex>=RIFFLE_CARD_COUNT_ALL)
		{
			//��ȡ��С
			CSize SizeCardItem;
			CCardControl::m_CardResource.GetCardSize(CARD_TYPE_SMALL,SizeCardItem);

			//��ȡ����
			CD3DSprite * pD3DTexture=NULL;
			CCardControl::m_CardResource.GetCardTexture(true,&pD3DTexture);

			//����λ��
			INT nDistance=__min(DISPATCH_HEAP_DISTANCE-2,m_wDispatchCardIndex-RIFFLE_CARD_COUNT_ALL)+2;
			INT nXSourcePos=(nWidth-SizeCardItem.cx-(DISPATCH_COUNT-1)*nDistance)/2;

			//��ȡ����
			for (BYTE i=0;i<DISPATCH_COUNT;i++)
			{
				INT nXDrawPos=nXSourcePos+i*nDistance;
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nHeight/2-170,SizeCardItem.cx,SizeCardItem.cy,SizeCardItem.cx*2,SizeCardItem.cy*4);
			}
		}
		else
		{
			//��ȡ��С
			CSize SizeDispatchCard;
			SizeDispatchCard.SetSize(m_TextureDispatchCard.GetWidth()/RIFFLE_CARD_COUNT_X,m_TextureDispatchCard.GetHeight()/RIFFLE_CARD_COUNT_Y);

			//�滭Ч��
			m_TextureDispatchCard.DrawImage(pD3DDevice,(nWidth-SizeDispatchCard.cx)/2,nHeight/2-185,SizeDispatchCard.cx,SizeDispatchCard.cy,
				SizeDispatchCard.cx*(m_wDispatchCardIndex%RIFFLE_CARD_COUNT_X),SizeDispatchCard.cy*(m_wDispatchCardIndex/RIFFLE_CARD_COUNT_X));
		}
	}

	//���ƶ���
	if (m_bDispatchCard==true)
	{
		//��������
		CSize SizeControl[GAME_PLAYER];
		CPoint PointControl[GAME_PLAYER];

		//��ȡ��С
		CSize SizeCardItem;
		CCardControl::m_CardResource.GetCardSize(CARD_TYPE_SMALL,SizeCardItem);

		//��ȡ����
		CD3DSprite * pD3DTexture=NULL;
		CCardControl::m_CardResource.GetCardTexture(true,&pD3DTexture);

		//����λ��
		INT nYHeapPos=nHeight/2-170;
		INT nXHeapPos=(nWidth-SizeCardItem.cx-(DISPATCH_COUNT-1)*DISPATCH_HEAP_DISTANCE)/2;

		//��ȡλ��
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_HandCardControl[i].GetControlSize(SizeControl[i]);
			m_HandCardControl[i].GetOriginPoint(PointControl[i]);
		}

		//�滭�ƶ�
		for (BYTE i=0;i<DISPATCH_COUNT;i++)
		{
			if ((DISPATCH_COUNT-i)*DISPATCH_CARD_SPACE>m_wDispatchCardIndex)
			{
				INT nXDrawPos=nXHeapPos+i*DISPATCH_HEAP_DISTANCE;
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nHeight/2-170,SizeCardItem.cx,SizeCardItem.cy,SizeCardItem.cx*2,SizeCardItem.cy*4);
			}
		}

		//�滭����
		for (BYTE i=0;i<DISPATCH_COUNT;i++)
		{
			if ((m_wDispatchCardIndex>=(i+1)*DISPATCH_CARD_SPACE)&&(m_wDispatchCardIndex<(i+1)*DISPATCH_CARD_SPACE+DISPATCH_CARD_TIMES))
			{
				//��������
				WORD wSendUser=(m_wStartUser+i)%GAME_PLAYER;
				WORD wSendIndex=m_wDispatchCardIndex-(i+1)*DISPATCH_CARD_SPACE;

				//Ŀ��λ��
				INT nXTargetPos=PointControl[wSendUser].x+SizeControl[wSendUser].cx/2;
				INT nYTargetPos=PointControl[wSendUser].y+SizeControl[wSendUser].cy/2;

				//���λ��
				INT nYSourcePos=nYHeapPos+SizeCardItem.cy/2;
				INT nXSourcePos=nXHeapPos+(DISPATCH_COUNT-i-1)*DISPATCH_HEAP_DISTANCE+SizeCardItem.cx/2;

				//�滭�˿�
				INT nXDrawPos=nXSourcePos+wSendIndex*(nXTargetPos-nXSourcePos)/DISPATCH_CARD_TIMES-SizeCardItem.cx/2;
				INT nYDrawPos=nYSourcePos+wSendIndex*(nYTargetPos-nYSourcePos)/DISPATCH_CARD_TIMES-SizeCardItem.cy/2;

				INT nXImagePos=2;
				INT nYImagePos=4;
				if (m_bUserCheat[wSendUser])
				{
					BYTE cbCardData = m_cbHandCardData[wSendUser][i / GAME_PLAYER];
					if ((cbCardData==0x4E)||(cbCardData==0x4F))
					{
						nXImagePos=((cbCardData&CARD_MASK_VALUE)%14);
						nYImagePos=((cbCardData&CARD_MASK_COLOR)>>4);
					}
					else
					{
						nXImagePos=((cbCardData&CARD_MASK_VALUE)-1);
						nYImagePos=((cbCardData&CARD_MASK_COLOR)>>4);
					}
				}
				pD3DTexture->DrawImage(pD3DDevice,nXDrawPos,nYDrawPos,SizeCardItem.cx,SizeCardItem.cy,SizeCardItem.cx*nXImagePos,SizeCardItem.cy*nYImagePos);
			}
		}
	}

	//��������
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bCountWarn[i]==true)
		{

			CSize SizeCountWarn[5];
			for(INT j = 0 ; j < 5 ; ++j)
			{
				SizeCountWarn[j].SetSize(m_TextureWarn[j].GetWidth(),m_TextureWarn[j].GetHeight());
			}
			m_TextureWarn[0].DrawImage(pD3DDevice,m_ptCountWarn[i].x,m_ptCountWarn[i].y,SizeCountWarn[0].cx,SizeCountWarn[0].cy,
				0,0);
			m_TextureWarn[1].DrawImage(pD3DDevice,m_ptCountWarn[i].x-43,m_ptCountWarn[i].y,SizeCountWarn[1].cx,SizeCountWarn[1].cy,
				0,0);
			m_TextureWarn[2].DrawImage(pD3DDevice,m_ptCountWarn[i].x+43,m_ptCountWarn[i].y,SizeCountWarn[2].cx,SizeCountWarn[2].cy,
				0,0);

			if(m_wCountWarnIndex[i] == 0)
			{
				m_TextureWarn[3].DrawImage(pD3DDevice,m_ptCountWarn[i].x-19,m_ptCountWarn[i].y-17,SizeCountWarn[3].cx,SizeCountWarn[3].cy,
					0,0);
				m_TextureWarn[4].DrawImage(pD3DDevice,m_ptCountWarn[i].x+23,m_ptCountWarn[i].y+60-56+10,SizeCountWarn[4].cx,SizeCountWarn[4].cy,
					0,0);
			}
		}
		
	}


	//��ʷ����
	if (m_bShowScore==true)
	{
		//��ȡ��С
		CSize SizeHistoryScore;
		SizeHistoryScore.SetSize(m_TextureHistoryScore.GetWidth(),m_TextureHistoryScore.GetHeight());

		//���ֱ���
		m_TextureHistoryScore.DrawImage(pD3DDevice,nWidth-SizeHistoryScore.cx-40+35,5+26+28-30);

		//����λ��
		INT nYBenchmark=5+26+35-30;
		INT nXBenchmark=nWidth-SizeHistoryScore.cx-40+28;

		//�滭��Ϣ
		for (WORD i=0;i<GAME_PLAYER;i++)
		{			
			//��ȡ�û�
			IClientUserItem * pIClientUserItem=GetClientUserItem(i);
			if ((pIClientUserItem==NULL)||(m_pHistoryScore[i]==NULL)) continue;

			//λ�ü���
			CRect rcAccounts(nXBenchmark+13,nYBenchmark+31+i*23,nXBenchmark+83,nYBenchmark+43+i*23);
			CRect rcTurnScore(nXBenchmark+85,nYBenchmark+31+i*23,nXBenchmark+135,nYBenchmark+43+i*23);
			CRect rcCollectScore(nXBenchmark+139,nYBenchmark+31+i*23,nXBenchmark+185,nYBenchmark+43+i*23);

			//������Ϣ
			TCHAR szTurnScore[16]=TEXT("");
			TCHAR szCollectScore[16]=TEXT("");
			_sntprintf(szTurnScore,CountArray(szTurnScore),SCORE_STRING,m_pHistoryScore[i]->lTurnScore);
			_sntprintf(szCollectScore,CountArray(szCollectScore),SCORE_STRING,m_pHistoryScore[i]->lCollectScore);

			//�滭��Ϣ
			D3DCOLOR crColor1=D3DCOLOR_XRGB(255,153,0);
			D3DCOLOR crColor2=D3DCOLOR_XRGB(0,255,153);
			LPCTSTR pszNickName=pIClientUserItem->GetNickName();
			DrawTextString(pD3DDevice,pszNickName,&rcAccounts,DT_END_ELLIPSIS|DT_LEFT,D3DCOLOR_XRGB(255,255,255));
			DrawTextString(pD3DDevice,szTurnScore,&rcTurnScore,DT_END_ELLIPSIS|DT_CENTER,(m_pHistoryScore[i]->lTurnScore>=0)?crColor1:crColor2);
			DrawTextString(pD3DDevice,szCollectScore,&rcCollectScore,DT_END_ELLIPSIS|DT_CENTER,(m_pHistoryScore[i]->lCollectScore>=0)?crColor1:crColor2);
		}
	}

	return;
}

//��Ԫ����
VOID CGameClientView::SetCellScore(LONG lCellScore)
{
	if (m_lCellScore!=lCellScore)
	{
		//���ñ���
		m_lCellScore=lCellScore;

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//ը������
VOID CGameClientView::SetBombCount(BYTE cbBombCount)
{
	if (m_cbBombCount!=cbBombCount)
	{
		//���ñ���
		m_cbBombCount=cbBombCount;

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//����ׯ��
VOID CGameClientView::SetBankerUser(WORD wBankerUser)
{
	if (m_wBankerUser!=wBankerUser)
	{
		//���ñ���
		m_wBankerUser=wBankerUser;

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//���ñ���
VOID CGameClientView::SetBankerScore(BYTE cbBankerScore)
{
	if (m_cbBankerScore!=cbBankerScore)
	{
		//���ñ���
		m_cbBankerScore=cbBankerScore;

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//��ʾ����
VOID CGameClientView::ShowBankerCard(BYTE cbCardData[3])
{
	//���ñ���
	m_bReversalCard=true;
	m_wReversalCardIndex=0;

	//����ʱ��
	m_ReversalCardLapseCount.Initialization();

	//���ÿؼ�
	m_BackCardControl.SetShowCount(0);
	m_BackCardControl.SetCardData(cbCardData,3);

	//������Դ
	ASSERT(m_TextureReversalCard.IsNull()==true);
	if (m_TextureReversalCard.IsNull()==true) m_TextureReversalCard.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("REVERSAL_CARD"),TEXT("PNG"));

	//���½���
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//���ý���
VOID CGameClientView::ShowGameScoreInfo(tagScoreInfo & ScoreInfo,bool bGameEnd)
{
	m_bGameEnd = bGameEnd;
	if(bGameEnd == false)
		return;
	//������Ϣ
	m_ScoreControl.SetScoreInfo(ScoreInfo);
	//���ñ���
	m_ScoreInfo=ScoreInfo;

	//�����ؼ�
	if (m_ScoreControl.IsWindowActive()==false)
	{
		//��ȡλ��
		CRect rcClient;
		GetClientRect(&rcClient);

		//��������
		CRect rcCreate(0,0,0,0);
		m_ScoreControl.ActiveWindow(rcCreate,WS_VISIBLE,IDC_GAME_SCORE,&m_VirtualEngine,NULL);

		//�ƶ�λ��
		CRect rcScoreControl;
		m_ScoreControl.GetWindowRect(rcScoreControl);
		m_ScoreControl.SetWindowPos((rcClient.Width()-rcScoreControl.Width())/2,(rcClient.Height()-rcScoreControl.Height())*2/5-20,0,0,SWP_NOSIZE);
	}
	else
	{
		m_ScoreControl.ShowWindow(true);
	}
	return;
}



// �滭����
void CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum,INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),SCORE_STRING,lOutNum);
	DrawNumber(pD3DDevice, ImageNumber, szImageNum, szOutNum, nXPos, nYPos, uFormat);
}

// �滭����
void CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNumT[128] = {0};
	_sntprintf(szOutNumT,CountArray(szOutNumT),TEXT("%s"),szOutNum);
	DrawNumber(pD3DDevice, ImageNumber, szImageNum, szOutNumT, nXPos, nYPos, uFormat);
}


// �滭����
void CGameClientView::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat /*= DT_LEFT*/)
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
				ImageNumber->DrawImage(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}

//�滭ʱ��
VOID CGameClientView::DrawUserTimerEx(CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, WORD wUserClock)
{
	//�滭ʱ��
	if ((wUserClock>0)&&(wUserClock<100))
	{
		//��ȡ��С
		CSize SizeClockItem;
		CSize SizeClockBack;
		CSize SizeTimeCartoon;
		SizeClockBack.SetSize(m_TextureTimeBack.GetWidth(),m_TextureTimeBack.GetHeight());
		SizeClockItem.SetSize(m_TextureTimeNumber.GetWidth()/10,m_TextureTimeNumber.GetHeight());
		SizeTimeCartoon.SetSize(m_TextureCartoonTime.GetWidth()/15,m_TextureCartoonTime.GetHeight());

		//�滭����
		INT nXDrawPos=nXPos-SizeClockBack.cx/2;
		INT nYDrawPos=nYPos-SizeClockBack.cy/2;
		INT wTimeIndex = (INT)((double)wUserClock/m_dTimeScale)-1;
		m_TextureTimeBack.DrawImage(pD3DDevice,nXDrawPos,nYDrawPos);

		//�滭ʱ�䶯��
		m_TextureCartoonTime.DrawImage(pD3DDevice,nXDrawPos+7,nYDrawPos+7,SizeTimeCartoon.cx,SizeTimeCartoon.cy,wTimeIndex*SizeTimeCartoon.cx,0);
		m_TextureTimeLight.DrawImage(pD3DDevice,nXDrawPos+12,nYDrawPos+9,m_TextureTimeLight.GetWidth(),m_TextureTimeLight.GetHeight(),0,0);

		//�滭ʱ��
		WORD nClockItem1=wUserClock/10;
		WORD nClockItem2=wUserClock%10;
		m_TextureTimeNumber.DrawImage(pD3DDevice,nXDrawPos+13,nYDrawPos+18,SizeClockItem.cx,SizeClockItem.cy,nClockItem1*SizeClockItem.cx,0);
		m_TextureTimeNumber.DrawImage(pD3DDevice,nXDrawPos+32,nYDrawPos+18,SizeClockItem.cx,SizeClockItem.cy,nClockItem2*SizeClockItem.cx,0);
		
	}
	return;
}

//�ɻ�����
VOID CGameClientView::ShowCartoonPlan(bool bCartoon)
{
	if (bCartoon==true)
	{
		//���ñ���
		m_bCartoonPlan=true;
		m_wCartoonPlanIndex=0L;

		//����ʱ��
		m_CartoonPlanLapseCount.Initialization();

		//������Դ
		if (m_TextureCartoonPlan.IsNull()==true)
		{
			m_TextureCartoonPlan.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CARTOON_PLAN"),TEXT("PNG"));
		}

		//���ڴ�С
		CRect rcClient;
		GetClientRect(&rcClient);

		//����λ��
		m_ptCartoonPlan.SetPoint( rcClient.Width() , rcClient.Height()/2 );

		//���½���
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}
	else
	{
		//ֹͣ����
		if (m_bCartoonPlan==true)
		{
			//���ñ���
			m_bCartoonPlan=false;
			m_wCartoonPlanIndex=0L;

			//����λ��
			m_ptCartoonPlan.SetPoint( 0, 0 );

			//ж����Դ
			ASSERT(m_TextureCartoonPlan.IsNull()==false);
			if (m_TextureCartoonPlan.IsNull()==false) m_TextureCartoonPlan.Destory();

			//���½���
			CRect rcClient;
			GetClientRect(&rcClient);
			InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
		}
	}

	return;
}

//��ը����
VOID CGameClientView::ShowCartoonBomb(bool bCartoon)
{
	if (bCartoon==true)
	{
		//���ñ���
		m_bCartoonBomb=true;

		//ͼƬ�Դ��ӳ�һ֡����ֹ������֡
		m_wCartoonBombIndex=-1L;

		

		//������Դ
		if (m_TextureCartoonBomb.IsNull()==true)
		{
			m_TextureCartoonBomb.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CARTOON_BOMB"),TEXT("PNG"));
		}
		
		//����ʱ��
		m_CartoonBombLapseCount.Initialization();

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}
	else
	{
		//ֹͣ����
		if (m_bCartoonBomb==true)
		{
			//���ñ���
			m_bCartoonBomb=false;
			m_wCartoonBombIndex=0L;

			//ж����Դ
			ASSERT(m_TextureCartoonBomb.IsNull()==false);
			if (m_TextureCartoonBomb.IsNull()==false) m_TextureCartoonBomb.Destory();

			//���½���
			CRect rcClient;
			GetClientRect(&rcClient);
			InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
		}
	}

	return;
}

//�������
VOID CGameClientView::ShowCartoonRocket(bool bCartoon)
{
	if (bCartoon==true)
	{
		//���ñ���
		m_bCartoonRocket=true;
		m_wCartoonRocketIndex=0L;

		//����ʱ��
		m_CartoonRocketLapseCount.Initialization();

		//������Դ
		if (m_TextureCartoonRocket.IsNull()==true && m_TextureRocket.IsNull() == true && m_TextureRocketLight.IsNull() == true)
		{
			m_TextureRocket.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("ROCKET"),TEXT("PNG"));
			m_TextureRocketLight.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("LIGHT"),TEXT("PNG"));
			m_TextureCartoonRocket.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CARTOON_ROCKET"),TEXT("PNG"));
		}

		//���ڴ�С
		CRect rcClient;
		GetClientRect(&rcClient);

		//����λ��
		m_ptCartoonRocket.SetPoint( rcClient.Width() /2 , rcClient.Height() - 500);

		//���½���
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}
	else
	{
		//ֹͣ����
		if (m_bCartoonRocket==true)
		{
			//���ñ���
			m_bCartoonRocket=false;
			m_wCartoonRocketIndex=0L;

			//����λ��
			m_ptCartoonRocket.SetPoint( 0, 0 );

			//ж����Դ
			ASSERT(m_TextureCartoonRocket.IsNull()==false&& m_TextureRocket.IsNull() == false && m_TextureRocketLight.IsNull() == false);
			if (m_TextureCartoonRocket.IsNull()==false) m_TextureCartoonRocket.Destory();
			if (m_TextureRocket.IsNull()==false) m_TextureRocket.Destory();
			if (m_TextureRocketLight.IsNull()==false) m_TextureRocketLight.Destory();

			//���½���
			CRect rcClient;
			GetClientRect(&rcClient);
			InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
		}
	}

	return;
}

//��������
VOID CGameClientView::SetLastTurnState(bool bLastTurnState)
{
	if (m_bLastTurnState!=bLastTurnState)
	{
		//���ñ���
		m_bLastTurnState=bLastTurnState;

		//��Դ����
		if (m_bLastTurnState==true)
		{
			ASSERT(m_TextureLastTurn.IsNull()==true);
			if (m_TextureLastTurn.IsNull()==true) m_TextureLastTurn.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("LAST_TURN"),TEXT("PNG"));
		}
		else
		{
			ASSERT(m_TextureLastTurn.IsNull()==false);
			if (m_TextureLastTurn.IsNull()==false) m_TextureLastTurn.Destory();
		}

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//�ȴ��з�
VOID CGameClientView::SetWaitCallScore(bool bWaitCallScore)
{
	if (m_bWaitCallScore!=bWaitCallScore)
	{
		//���ñ���
		m_bWaitCallScore=bWaitCallScore;

		//��Դ����
		if (m_bWaitCallScore==true)
		{
			ASSERT(m_TextureWaitCallScore.IsNull()==true);
			if (m_TextureWaitCallScore.IsNull()==true) m_TextureWaitCallScore.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("WAIT_CALL_SCORE"),TEXT("PNG"));
		}
		else
		{
			ASSERT(m_TextureWaitCallScore.IsNull()==false);
			if (m_TextureWaitCallScore.IsNull()==false) m_TextureWaitCallScore.Destory();
		}

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//���÷���
VOID CGameClientView::SetUserPassState(WORD wChairID, bool bUserPass)
{
	//��������
	bool bLoadImage=false;

	//���ñ���
	if (wChairID==INVALID_CHAIR)
	{
		//���ñ���
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_bUserPass[i]=bUserPass;
		}
	}
	else
	{
		//���ñ���
		ASSERT(wChairID<GAME_PLAYER);
		m_bUserPass[wChairID]=bUserPass;
	}

	//״̬���
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bUserPass[i]==true)
		{
			bLoadImage=true;
			break;
		}
	}

	//��Դ����
	if (bLoadImage==true)
	{
		if (m_TextureUserPass.IsNull()==true)
		{
			m_TextureUserPass.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("USER_PASS"),TEXT("PNG"));
		}
	}
	else
	{
		if (m_TextureUserPass.IsNull()==false)
		{
			m_TextureUserPass.Destory();
		}
	}

	//���½���
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//���þ���
VOID CGameClientView::SetUserCountWarn(WORD wChairID, bool bCountWarn)
{
	//��������
	bool bLoadImage=false;

	//���ñ���
	if (wChairID==INVALID_CHAIR)
	{
		//���ñ���
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_bCountWarn[i]=bCountWarn;
		}
	}
	else
	{
		//���ñ���
		ASSERT(wChairID<GAME_PLAYER);
		m_bCountWarn[wChairID]=bCountWarn;
	}

	//״̬���
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bCountWarn[i]==true)
		{
			bLoadImage=true;
			break;
		}
	}

	HINSTANCE hResInstance = AfxGetInstanceHandle();
	//��Դ����
	if (bLoadImage==true)
	{
		if (m_TextureWarn[0].IsNull()==true)
			m_TextureWarn[0].LoadImage(&m_D3DDevice,hResInstance,TEXT("WARN_BACK"),TEXT("PNG"));
		if (m_TextureWarn[1].IsNull()==true)
			m_TextureWarn[1].LoadImage(&m_D3DDevice,hResInstance,TEXT("WARN_1"),TEXT("PNG"));
		if (m_TextureWarn[2].IsNull()==true)
			m_TextureWarn[2].LoadImage(&m_D3DDevice,hResInstance,TEXT("WARN_2"),TEXT("PNG"));
		if (m_TextureWarn[3].IsNull()==true)
			m_TextureWarn[3].LoadImage(&m_D3DDevice,hResInstance,TEXT("WARN_LIGHT"),TEXT("PNG"));
		if (m_TextureWarn[4].IsNull()==true)
			m_TextureWarn[4].LoadImage(&m_D3DDevice,hResInstance,TEXT("WARN_3"),TEXT("PNG"));
	}
	else
	{

		if (m_TextureWarn[0].IsNull()==false)
			m_TextureWarn[0].Destory();
		if (m_TextureWarn[1].IsNull()==false)
			m_TextureWarn[1].Destory();
		if (m_TextureWarn[2].IsNull()==false)
			m_TextureWarn[2].Destory();
		if (m_TextureWarn[3].IsNull()==false)
			m_TextureWarn[3].Destory();
		if (m_TextureWarn[4].IsNull()==false)
			m_TextureWarn[4].Destory();
	}

	//���½���
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//���ýз�
VOID CGameClientView::SetUserCallScore(WORD wChairID, BYTE cbCallScore)
{
	//��������
	bool bLoadImage=false;

	//���ñ���
	if (wChairID==INVALID_CHAIR)
	{
		//���ñ���
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			m_cbCallScore[i]=cbCallScore;
		}
	}
	else
	{
		//���ñ���
		ASSERT(wChairID<GAME_PLAYER);
		m_cbCallScore[wChairID]=cbCallScore;
	}

	//״̬���
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_cbCallScore[i]!=0)
		{
			bLoadImage=true;
			break;
		}
	}

	//��Դ����
	if (bLoadImage==true)
	{
		if (m_TextureCallScore.IsNull()==true)
		{
			m_TextureCallScore.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CALL_SCORE"),TEXT("PNG"));
		}
	}
	else
	{
		if (m_TextureCallScore.IsNull()==false)
		{
			m_TextureCallScore.Destory();
		}
	}

	//���½���
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//���û���
VOID CGameClientView::SetHistoryScore(WORD wChairID, tagHistoryScore * pHistoryScore)
{
	//Ч�����
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//���ñ���
	m_pHistoryScore[wChairID]=pHistoryScore;

	//���½���
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	return;
}

//���ƶ���
VOID CGameClientView::StopDispatchCard()
{
	//ֹͣ����
	if ((m_bRiffleCard==true)||(m_bDispatchCard==true))
	{
		//���ñ���
		m_bRiffleCard=false;
		m_bDispatchCard=false;
		m_wDispatchCardIndex=0L;

		//�����˿�
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (i==MYSELF_VIEW_ID)
			{
				m_HandCardControl[i].SetCardData(m_cbHandCardData[MYSELF_VIEW_ID],CountArray(m_cbHandCardData));
			}
			else
			{
				m_HandCardControl[i].SetCardData(NORMAL_COUNT);
			}
		}

		//ж����Դ
		ASSERT(m_TextureDispatchCard.IsNull()==false);
		if (m_TextureDispatchCard.IsNull()==false) m_TextureDispatchCard.Destory();

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//���ƶ���
VOID CGameClientView::ShowDispatchCard(BYTE cbCardData[NORMAL_COUNT], WORD wStartUser)
{
	//���ñ���
	m_bRiffleCard=true;
	m_bDispatchCard=false;
	m_wDispatchCardIndex=0L;

	//���ñ���
	m_wStartUser=wStartUser;
	CopyMemory(m_cbHandCardData[MYSELF_VIEW_ID],cbCardData,sizeof(BYTE)*NORMAL_COUNT);

	//�����˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_HandCardControl[i].SetCardData(0);
	}

	//����ʱ��
	m_DispatchCardLapseCount.Initialization();

	//������Դ
	ASSERT(m_TextureDispatchCard.IsNull()==true);
	if (m_TextureDispatchCard.IsNull()==true) m_TextureDispatchCard.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"),TEXT("PNG"));

	//���½���
	CRect rcClient;
	GetClientRect(&rcClient);
	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());

	//��������
	PlayGameSound(AfxGetInstanceHandle(),TEXT("REVERSAL_CARD"));

	return;
}

//��ʼ��ť
VOID CGameClientView::OnBnClickedStart()
{
	//������Ϣ
	SendEngineMessage(IDM_START,0,0);

	return;
}

//���ְ�ť
VOID CGameClientView::OnBnClickedScore()
{
	//���ñ���
	m_bShowScore=!m_bShowScore;

	//������Դ
	if (m_bShowScore==true)
	{
		ASSERT(m_TextureHistoryScore.IsNull()==true);
		m_TextureHistoryScore.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("HISTORY_SCORE"),TEXT("PNG"));
	}
	else
	{
		ASSERT(m_TextureHistoryScore.IsNull()==false);
		if (m_TextureHistoryScore.IsNull()==false) m_TextureHistoryScore.Destory();
	}

	//���ð�ť
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btScore.SetButtonImage(&m_D3DDevice,(m_bShowScore==true)?TEXT("BT_CLOSE_SCORE"):TEXT("BT_SHOW_SCORE"),TEXT("PNG"),hResInstance);

	return;
}

//���ư�ť
VOID CGameClientView::OnBnClickedOutCard()
{
	//������Ϣ
	SendEngineMessage(IDM_OUT_CARD,1,0);

	return;
}

//����ť
VOID CGameClientView::OnBnClickedSortCard()
{
	//������Ϣ
	SendEngineMessage(IDM_SORT_HAND_CARD,0,0);

	return;
}

//���ְ�ť
VOID CGameClientView::OnBnClickedLastTurn()
{
	//������Ϣ
	SendEngineMessage(IDM_LAST_TURN_CARD,0,0);

	return;
}

//���鰴ť
VOID  CGameClientView::OnBnClickedExpression()
{
	//test
	//cardcount -- ;
	//if(cardcount == 0)
	//	cardcount = 13;
	//for(int i = 0 ; i < GAME_PLAYER ; ++i)
	//{
	//	BYTE cbCardData[MAX_COUNT] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x11,0x12,0x13,0x14,0x15,0x16,0x17};
	//	m_UserCardControl[i].SetCardData(cbCardData,cardcount);
	//	m_BackCardControl.SetCardData(cbCardData,3); 
	//}/*
	//if(m_bCartoonLine == false)
	//	ShowCartoonLine(true);*/

	return;
}

//�س���ť
VOID  CGameClientView::OnBnClickedEnter()
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

//	m_strChat = str;
	return;
}

//������ť
VOID  CGameClientView::OnBnClickedVoice()
{
	return;
}

//�л�������ť
VOID  CGameClientView::OnBnClickedSwitchSence()
{
	m_cbSenceIndex = (m_cbSenceIndex+1)%2;
	ASSERT(m_TextureBack.IsNull() == false);
	if(m_TextureBack.IsNull() == false)
	{
		HINSTANCE hResInstance = AfxGetInstanceHandle();
		m_TextureBack.Destory();
		m_TextureBack.LoadImage(&m_D3DDevice,hResInstance,(m_cbSenceIndex == 0)?TEXT("VIEW_BACK_1"):TEXT("VIEW_BACK_2"),TEXT("PNG"));
	}
	return;
}

//�йܰ�ť
VOID CGameClientView::OnBnClickedTrustee()
{
	//������Ϣ
	SendEngineMessage(IDM_TRUSTEE_CONTROL,0,0);

	return;
}

//�йܰ�ť
VOID CGameClientView::OnBnClickedCancelTrustee()
{
	//������Ϣ
	SendEngineMessage(IDM_TRUSTEE_CONTROL,0,0);

	return;
}

//PASS��ť
VOID CGameClientView::OnBnClickedPassCard()
{
	//������Ϣ
	SendEngineMessage(IDM_PASS_CARD,0,0);

	return;
}

//��ʾ��ť
VOID CGameClientView::OnBnClickedOutPrompt()
{
	//������Ϣ
	SendEngineMessage(IDM_OUT_PROMPT,0,0);

	return;
}

//һ�ְ�ť
VOID CGameClientView::OnBnClickedCallScore1()
{
	//������Ϣ
	SendEngineMessage(IDM_CALL_SCORE,1,1);

	return;
}

//���ְ�ť
VOID  CGameClientView::OnBnClickedCallScore2()
{
	//������Ϣ
	SendEngineMessage(IDM_CALL_SCORE,2,2);

	return;
}

//���ְ�ť
VOID  CGameClientView::OnBnClickedCallScore3()
{
	//������Ϣ
	SendEngineMessage(IDM_CALL_SCORE,3,3);

	return;
}

//������ť
VOID  CGameClientView::OnBnClickedCallScoreNone()
{
	//������Ϣ
	SendEngineMessage(IDM_CALL_SCORE,0xFF,0xFF);

	return;
}

//�����Ϣ
VOID CGameClientView::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//���ͳ���
	if ( m_HandCardControl[MYSELF_VIEW_ID].GetPositively() )
	{
		SendEngineMessage(IDM_OUT_CARD,1,1);
	}

	return;
}

//�����Ϣ
VOID CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);

	//��ȡ���������˿�
	BYTE cbOldShootCard[MAX_CARD_COUNT];
	BYTE cbOldShootCrad = m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbOldShootCard, MAX_CARD_COUNT);

	//Ĭ�ϴ���
	if (m_bMoveMouse==false)
	{
		//��ȡ�˿�
		tagCardItem * pCardItem=NULL;
		WORD wMouseDownItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(Point);
		if (wMouseDownItem==m_cbMouseDownItem) pCardItem=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromPoint(Point);

		//�����˿�
		if (pCardItem!=NULL)
		{
			//�����˿�
			pCardItem->bShoot=!pCardItem->bShoot;

			//������Ϣ
			SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

			//���½���
			CRect rcClient;
			GetClientRect(&rcClient);
			InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
		}
	}

	//ѡ����
	if ((m_bSelectCard==true)&&(m_bMoveMouse==true))
	{
		//�����˿�
		if (m_cbHoverCardItem>m_cbMouseDownItem)
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetShootArea(m_cbMouseDownItem,m_cbHoverCardItem);
		}
		else
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetShootArea(m_cbHoverCardItem,m_cbMouseDownItem);
		}

		//������Ϣ
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	//��������
	if ((m_bSwitchCard==true)&&(m_bMoveMouse==true))
	{
		//�����˿�
		m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

		//������Ϣ
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);
		SendEngineMessage(IDM_SORT_HAND_CARD,ST_CUSTOM,ST_CUSTOM);

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	//��ȡ�˿�
	BYTE cbShootCard[MAX_CARD_COUNT];
	ZeroMemory(cbShootCard, sizeof(cbShootCard));
	BYTE cbShootCrad = m_HandCardControl[MYSELF_VIEW_ID].GetShootCard(cbShootCard, MAX_CARD_COUNT);

	//����˳��
	if ( cbOldShootCrad == 1 && cbShootCrad == 2 && !m_bSwitchCard && !m_bMoveMouse &&m_HandCardControl[MYSELF_VIEW_ID].SetShootOrderCard(cbShootCard[0], cbShootCard[1]) )
	{
		//������Ϣ
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	//�ͷ����
	ReleaseCapture();


	//���ñ���
	m_bMoveMouse=false;
	m_bSwitchCard=false;
	m_bSelectCard=false;

	//��������
	m_cbMouseDownItem=INVALID_ITEM;
	m_cbHoverCardItem=INVALID_ITEM;

	return;
}

//�����Ϣ
VOID CGameClientView::OnMouseMove(UINT nFlags, CPoint MousePoint)
{
	__super::OnMouseMove(nFlags, MousePoint);

	//��������
	BYTE cbHoverCardItem=INVALID_ITEM;

	//�ƶ��ж�
	if (m_cbHoverCardItem!=INVALID_ITEM)
	{
		//�˿˴�С
		CSize ControlSize;
		m_HandCardControl[MYSELF_VIEW_ID].GetControlSize(ControlSize);

		//�˿�λ��
		CPoint OriginPoint;
		m_HandCardControl[MYSELF_VIEW_ID].GetOriginPoint(OriginPoint);

		//���е���
		if (MousePoint.x<OriginPoint.x) MousePoint.x=OriginPoint.x;
		if (MousePoint.x>(OriginPoint.x+ControlSize.cx)) MousePoint.x=(OriginPoint.x+ControlSize.cx);

		//��ȡ����
		MousePoint.y=OriginPoint.y+DEF_SHOOT_DISTANCE;
		cbHoverCardItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(MousePoint);

		//�ƶ�����
		if (cbHoverCardItem!=m_cbHoverCardItem) m_bMoveMouse=true;
	}

	////�����˿�
	//if ((m_bSwitchCard==true)&&(cbHoverCardItem!=m_cbHoverCardItem))
	//{
	//	//Ŀ���ж�
	//	BYTE cbTargetCardItem=INVALID_ITEM;
	//	cbTargetCardItem=(m_cbSwitchCardItem>cbHoverCardItem)?0:cbHoverCardItem-m_cbSwitchCardItem;

	//	//�����˿�
	//	m_HandCardControl[MYSELF_VIEW_ID].MoveCardItem(cbTargetCardItem);

	//	//���½���
	//	CRect rcClient;
	//	GetClientRect(&rcClient);
	//	InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	//}

	//ѡ���˿�
	if ((m_bSelectCard==true)&&(cbHoverCardItem!=m_cbHoverCardItem))
	{
		//�����˿�
		if (cbHoverCardItem>m_cbMouseDownItem)
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetSelectIndex(m_cbMouseDownItem,cbHoverCardItem);
		}
		else
		{
			m_HandCardControl[MYSELF_VIEW_ID].SetSelectIndex(cbHoverCardItem,m_cbMouseDownItem);
		}

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	//���ñ���
	m_cbHoverCardItem=cbHoverCardItem;

	return;
}

//�����Ϣ
VOID CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);


	//״̬�ж�
	if (m_HandCardControl[MYSELF_VIEW_ID].GetPositively()==true)
	{
		//��ȡ�˿�
		m_cbMouseDownItem=m_HandCardControl[MYSELF_VIEW_ID].SwitchCardPoint(Point);

		//�����˿�
		if (m_cbMouseDownItem!=INVALID_ITEM)
		{
			//��ȡ�˿�
			tagCardItem * pCardItem=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(m_cbMouseDownItem);

			//���ñ���
			m_bMoveMouse=false;
			m_cbHoverCardItem=m_cbMouseDownItem;

			//��������
			if (pCardItem->bShoot==true)
			{
				//���ñ���
				m_bSwitchCard=true;
				m_cbSwitchCardItem=0;

				//����״̬
				for (BYTE i=0;i<m_cbMouseDownItem;i++)
				{
					tagCardItem * pCardItemTemp=m_HandCardControl[MYSELF_VIEW_ID].GetCardFromIndex(i);
					if ((pCardItemTemp!=NULL)&&(pCardItemTemp->bShoot==true)) m_cbSwitchCardItem++;
				}
			}
			else
			{
				m_bSelectCard=true;
			}

			//�������
			SetCapture();
		}
	}

	LPARAM lParam = MAKEWPARAM(Point.x,Point.y);
	AfxGetMainWnd()->PostMessage(WM_LBUTTONDOWN,0,lParam);

	return;
}

//�����Ϣ
VOID CGameClientView::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags, Point);

	//��ȡ�˿�
	tagCardItem * pCardItem = m_HandCardControl[MYSELF_VIEW_ID].GetCardFromPoint(Point);

	//�����˿�
	if ( pCardItem == NULL )
	{
		//��ֹ��ť
		m_btOutCard.EnableWindow(FALSE);

		//�����˿�
		m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}
	else if ( m_HandCardControl[MYSELF_VIEW_ID].GetPositively()  )
	{
		//�����˿�
		m_HandCardControl[MYSELF_VIEW_ID].SetShootCard(NULL,0);

		//����ͬ��
		m_HandCardControl[MYSELF_VIEW_ID].SetShootSameCard(pCardItem->cbCardData);

		//������Ϣ
		SendEngineMessage(IDM_LEFT_HIT_CARD,0,0);

		//���½���
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}

	return;
}

//�����Ϣ
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//��ȡ���
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//�˿˲���
	if (m_HandCardControl[MYSELF_VIEW_ID].OnEventSetCursor(MousePoint)==true)
	{
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//��Ϣ����
BOOL CGameClientView::OnCommand( WPARAM wParam, LPARAM lParam )
{
	switch( LOWORD(wParam) )
	{
	case IDC_CARD_PROMPT_1:
	case IDC_CARD_PROMPT_2:
	case IDC_CARD_PROMPT_3:	
	case IDC_CARD_PROMPT_4:
	case IDC_CARD_PROMPT_5:	
	case IDC_CARD_PROMPT_6:
		{
			//������Ϣ
			SendEngineMessage(IDM_SEARCH_CARD,LOWORD(wParam)-IDC_CARD_PROMPT_1,0);

			return TRUE;
		}
	case IDC_BT_CLOSE:
		{
			SendEngineMessage(IDM_GAME_CLOSE,0,0);
			return TRUE;
		}
	case IDC_BT_MIN:
		{
			AfxGetMainWnd()->PostMessage(WM_SYSCOMMAND,SC_MINIMIZE,0);
			return TRUE;
		}
	case IDC_BT_SOUND:
		{
			SendEngineMessage(IDM_GAME_SOUND,0,0);
			return TRUE;
		}
	case IDC_BT_PLAY_AGAIN:
		{
			SendEngineMessage(IDM_START,0,0);
			return TRUE;
		}
	case IDC_BT_RETURN:
		{
			AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);
			return TRUE;
		}
	case IDC_HISTORY_CHAT:
		{
			OnHideChatMessage();
			return TRUE;
		}
	}

	return __super::OnCommand(wParam,lParam);
}

//�û�����
VOID CGameClientView::UserValidCard(WORD wChairID, BYTE cbCardData[MAX_COUNT], BYTE cbCardCount/* = 17*/)
{
	ASSERT(wChairID >=0 && wChairID < GAME_PLAYER);

	if(wChairID != MYSELF_VIEW_ID)		
	{
		m_HandCardControl[wChairID].SetDisplayItem(false);
		static bool bChange = false;
		if(!bChange)
		{
			m_UserCardControl[0].SetBenchmarkPos(m_UserCardControl[0].m_BenchmarkPos.x,
				m_UserCardControl[0].m_BenchmarkPos.y+78,
				m_UserCardControl[0].m_XCollocateMode,
				m_UserCardControl[0].m_YCollocateMode);

			m_UserCardControl[2].SetBenchmarkPos(m_UserCardControl[2].m_BenchmarkPos.x,
				m_UserCardControl[2].m_BenchmarkPos.y+78,
				m_UserCardControl[2].m_XCollocateMode,
				m_UserCardControl[2].m_YCollocateMode);
			bChange = true;
		}
	}
	m_ChetCardContol[wChairID].SetDisplayItem(true);

	CopyMemory(m_cbHandCardData[wChairID], cbCardData, sizeof(BYTE)*cbCardCount);
	m_bUserCheat[wChairID] = true;
	if (m_HandCardControl[wChairID].GetCardCount() > 0)
	{
		m_HandCardControl[wChairID].SetCardData(cbCardData, m_HandCardControl[wChairID].GetCardCount());
		m_ChetCardContol[wChairID].SetCardData(cbCardData, m_ChetCardContol[wChairID].GetCardCount());

	}
	else
	{
		m_HandCardControl[wChairID].SetCardData(0);
		m_ChetCardContol[wChairID].SetCardData(0);

	}
}

//���ó���
VOID CGameClientView::SetUserAvatar(bool bCellScoreEnd)
{
	if(m_bCellScoreEnd!=bCellScoreEnd)
		m_bCellScoreEnd = bCellScoreEnd;
	if(m_bCellScoreEnd == false)
		m_bGameEnd = false;

	HINSTANCE hResInstance = AfxGetInstanceHandle();
	if(m_bCellScoreEnd == true)
	{
		
		if(m_TextureUserPlayer.IsNull() == false) m_TextureUserPlayer.Destory();
		if(m_TextureUserPlayerGame.IsNull() == true)
			m_TextureUserPlayerGame.LoadImage(&m_D3DDevice,hResInstance,TEXT("USER_FRAME_PLAYER_GAME"),TEXT("PNG"));
	}
	else
	{
		if(m_TextureUserPlayerGame.IsNull() == false) m_TextureUserPlayerGame.Destory();
		if(m_TextureUserPlayer.IsNull() == true)
			m_TextureUserPlayer.LoadImage(&m_D3DDevice,hResInstance,TEXT("USER_FRAME_PLAYER"),TEXT("PNG"));
	}
	return;
}

//���ñ���
VOID CGameClientView::SetTimeScale(WORD wTime)
{
	m_dTimeScale = (double)wTime/15.0;
	return;
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
				OnBnClickedEnter();
			}
			else
			{
				GetDlgItem(IDC_CHAT_INPUT)->SetFocus();
			}
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

//��ʾ����
void CGameClientView::ShowChatMessage(LPCTSTR strMessage,LPCTSTR UserName)
{
	CString strUserName = UserName;
	CString cstr = strMessage;
	TCHAR szMessage[100];
	_tcscpy(szMessage,cstr);
	int len = wcslen(szMessage);
	m_wMessageLen = 0;
	for(int i = 0 ; i < len ; ++i)
	{
		if(szMessage[i]>127)
			m_wMessageLen+=2;
		else
			m_wMessageLen+=1;
	}
	
	for(int i = 0 ; i< GAME_PLAYER ; ++i)
	{
		IClientUserItem * pIClientUserItem = GetClientUserItem(i);
		if(pIClientUserItem == NULL) continue;
		CString nametmp =  pIClientUserItem->GetNickName();
		if(strUserName == nametmp)
		{
			if(strMessage == TEXT(""))
			{
				m_ChatChair[i] = INVALID_CHAIR;
			}
			else
				m_ChatChair[i] = i;
			m_strChat[i] = strMessage;
			m_nChatStrlen[i] = m_wMessageLen;
			break;
		}
	}
	
	return;
}

//˳�Ӷ���
VOID CGameClientView::ShowCartoonLine(bool bCartoon,BYTE cbCardType)
{
	if (bCartoon==true)
	{
		//���ñ���
		m_bCartoonLine=true;
		m_wCartoonLineIndex=0L;

		//����ʱ��
		m_CartoonLineLapseCount.Initialization();

		//������Դ
		if (m_TextureCartoonLine.IsNull()==true && m_TextureLine.IsNull() == true && cbCardType<3)
		{
			CString str;
			str.Format(TEXT("GAME_SHUNZI_%d"),cbCardType);
			m_TextureLine.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),str,TEXT("PNG"));
			m_TextureCartoonLine.LoadImage(&m_D3DDevice,AfxGetInstanceHandle(),TEXT("CARTOON_LINE"),TEXT("PNG"));
		}

		//���ڴ�С
		CRect rcClient;
		GetClientRect(&rcClient);

		//���½���
		InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
	}
	else
	{
		//ֹͣ����
		if (m_bCartoonRocket==true)
		{
			//���ñ���
			m_bCartoonRocket=false;
			m_wCartoonRocketIndex=0L;

			//����λ��
			m_ptCartoonRocket.SetPoint( 0, 0 );

			//ж����Դ
			ASSERT(m_TextureCartoonLine.IsNull()==false&& m_TextureLine.IsNull() == false );
			if (m_TextureCartoonLine.IsNull()==false) m_TextureCartoonLine.Destory();
			if (m_TextureLine.IsNull()==false) m_TextureLine.Destory();

			//���½���
			CRect rcClient;
			GetClientRect(&rcClient);
			InvalidGameView(0,0,rcClient.Width(),rcClient.Height());
		}
	}



	return;
}

//���ý�����λ��
void CGameClientView::SetScoreControlPos(INT nXOffset,INT nYOffset)
{
	CRect rcScoreControl;
	bool bYmove = true;
	m_ScoreControl.GetWindowRect(rcScoreControl);
	if(rcScoreControl.top+nYOffset+rcScoreControl.Height()>768-20)
		bYmove = false;
	m_ScoreControl.SetWindowPos(rcScoreControl.left+nXOffset,bYmove == true?rcScoreControl.top+nYOffset:rcScoreControl.top,0,0,SWP_NOSIZE);
	return;
}

//����Ч��
bool CGameClientView::EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex)
{
	//״̬�ж�
	CGlobalUnits* pGlobalUnits = CGlobalUnits::GetInstance();
	IClientKernel* m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
	IClientUserItem* m_pIMySelfUserItem = GetClientUserItem(MYSELF_VIEW_ID);
	if (m_pIClientKernel==NULL) return false;
	if (m_pIMySelfUserItem==NULL) return false;

	//���ȱȽ�
	if(pszChatString != NULL)
	{
		CString strChat=pszChatString;
		if(strChat.GetLength() >= (LEN_USER_CHAT/2))
		{
			m_ChatMessage.InsertString(TEXT("��Ǹ����������������ݹ����������̺��ٷ��ͣ�\r\n"),COLOR_WARN);
			//InsertMessage(TEXT("\r\n"));
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

	return true;
}


//�鿴����
void CGameClientView::OnHideChatMessage()
{
	if (m_bShowChatMessage == true)
	{
		m_bShowChatMessage = false;
		m_ChatMessage.ShowWindow(SW_HIDE);
		m_btHistoryChat.SetButtonImage(&m_D3DDevice,TEXT("BT_SHOW_CHAT"),TEXT("PNG"),AfxGetInstanceHandle());
	}
	else
	{
		m_bShowChatMessage = true;
		m_ChatMessage.ShowWindow(SW_SHOW);
		m_btHistoryChat.SetButtonImage(&m_D3DDevice,TEXT("BT_HIDE_CHAT"),TEXT("PNG"),AfxGetInstanceHandle());

	}

	return;
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
void CGameClientView::OnDestroy()
{
	m_FontChat.DeleteFont();
	
	__super::OnDestroy();
}
//////////////////////////////////////////////////////////////////////////////////
