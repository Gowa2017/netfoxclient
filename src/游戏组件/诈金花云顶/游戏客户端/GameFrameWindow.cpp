#include "StdAfx.h"
#include "Resource.h"
#include "GameFrameWindow.h"
#include "GameClientview.h"
#include ".\gameframewindow.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//���ư�ť
#define IDC_MIN						100									//��С��ť
#define IDC_CLOSE					101									//�رհ�ť

#define IDC_BT_MIN					102
#define IDC_BT_CLOSE				103
#define IDC_SOUND					104
#define IDC_OPTION					105

//�ؼ���ʶ
#define IDC_SKIN_SPLITTER			200									//��ֿؼ�
#define IDC_GAME_CLIENT_VIEW		201									//��ͼ��ʶ

//��Ļλ��
#define BORAD_SIZE					6									//�߿��С
#define CAPTION_SIZE				32									//�����С

//�ؼ���С
#define SPLITTER_CX					0									//��ֿ��
#define CAPTION_SIZE				32									//�����С

//��Ļ����
#define LESS_SCREEN_CY				740									//��С�߶�
#define LESS_SCREEN_CX				1024								//��С���

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameFrameWindow, CFrameWnd)

	//ϵͳ��Ϣ
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETTINGCHANGE()
	ON_WM_NCHITTEST()

	//�Զ���Ϣ
	ON_MESSAGE(WM_SETTEXT,OnSetTextMesage)

	ON_WM_PAINT()
END_MESSAGE_MAP()

//���캯��
CGameFrameWindow::CGameFrameWindow()
{
	//����ӿ�
	m_pIClientKernel=NULL;
	m_pIGameFrameView=NULL;
	m_pIGameFrameService=NULL;

	//ע�����
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
	pGlobalUnits->RegisterGlobalModule(MODULE_GAME_FRAME_WND,QUERY_ME_INTERFACE(IUnknownEx));
	
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	//m_CurWindowSize.SetSize(cx,cy);
	m_CurWindowSize.SetSize(1024,768);
}

//��������
CGameFrameWindow::~CGameFrameWindow()
{
}

//�ӿڲ�ѯ
VOID * CGameFrameWindow::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IUserEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameFrameWnd,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameFrameWnd,Guid,dwQueryVer);
	return NULL;
}

//��Ϣ����
BOOL CGameFrameWindow::PreTranslateMessage(MSG * pMsg)
{
	return __super::PreTranslateMessage(pMsg);
}

//�����
BOOL CGameFrameWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//��������
	UINT nCommandID=LOWORD(wParam);

	//���ܰ�ť
	switch (nCommandID)
	{
	case IDC_MIN:				//��С��ť
		{
			if (IsIconic())	
				ShowWindow(SW_RESTORE);
			else
				ShowWindow(SW_MINIMIZE);
			
			return TRUE;
		}
	case IDC_CLOSE:				//�رհ�ť
		{
			PostMessage(WM_CLOSE,0,0);
			return TRUE;
		}
	case IDC_OPTION:
		{
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
				break;
			}
			return TRUE;
		}
	case IDC_SOUND:
		{
			//��ѯ��Ϸ
			CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
			IGameFrameEngine* pIGameFrameEngine = (IGameFrameEngine *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_ENGINE,IID_IGameFrameEngine,VER_IGameFrameEngine);
			if(pIGameFrameEngine == NULL) return TRUE;

			pGlobalUnits->m_bMuteStatuts = !pGlobalUnits->m_bMuteStatuts;
			if(!pGlobalUnits->m_bMuteStatuts)
				((CGameClientEngine*)pIGameFrameEngine)->PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));
			else
				((CGameClientEngine*)pIGameFrameEngine)->StopSound();
			
			m_btSound.SetButtonImage((pGlobalUnits->m_bMuteStatuts)?IDB_CLOSE_SOUND:IDB_OPEN_SOUND,AfxGetInstanceHandle(),false,false);
			return TRUE;
		}
	}

	return __super::OnCommand(wParam,lParam);
}

//�����ؼ�
VOID CGameFrameWindow::RectifyControl(INT nWidth, INT nHeight)
{
	//״̬�ж�
	if ((nWidth==0)||(nHeight==0)) return;

	//�ƶ�׼��
	HDWP hDwp=BeginDeferWindowPos(32);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;

	//��ѯ��Ϸ
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
	IGameFrameView * pIGameFrameView=(IGameFrameView *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_VIEW,IID_IGameFrameView,VER_IGameFrameView);

	//��Ϸ��ͼ
	if (pIGameFrameView!=NULL)
	{
		CRect rcArce;
		CSize SizeRestrict=m_CurWindowSize;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);
		SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
		SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);
		DeferWindowPos(hDwp, pIGameFrameView->GetGameViewHwnd(), NULL, 0, 30, SizeRestrict.cx, SizeRestrict.cy-30, uFlags);
	}

	DeferWindowPos(hDwp, m_GameFrameControl, NULL, 0, 0, 0, 0, uFlags);

	uFlags = uFlags|SWP_NOCOPYBITS|SWP_NOSIZE;

	DeferWindowPos(hDwp, m_btMin, NULL, nWidth-185, 4, 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btClose, NULL, nWidth-50, 4, 0, 0, uFlags);
	DeferWindowPos(hDwp,m_btSound,NULL,nWidth-96,4,0,0,uFlags);
	DeferWindowPos(hDwp,m_btOption,NULL,nWidth-142,4,0,0,uFlags);

	//�ƶ�����
	EndDeferWindowPos(hDwp);

	return;
}

//�û�����
VOID CGameFrameWindow::OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	//��������
	ASSERT(CGlobalUnits::GetInstance()!=NULL);
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

	//��ʾ��Ϣ
	if (pGlobalUnits->m_bNotifyUserInOut==true)
	{
		//CDialogChat::m_ChatMessage.InsertUserEnter(pIClientUserItem->GetNickName());
	}

	return;
}

//�û��뿪
VOID CGameFrameWindow::OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	return;
}

//�û�����
VOID CGameFrameWindow::OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser)
{

	return;
}

//�û�״̬
VOID CGameFrameWindow::OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	return;
}

//�û�ͷ��
VOID CGameFrameWindow::OnEventCustomFace(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	return;

}

//�û�����
VOID CGameFrameWindow::OnEventUserAttrib(IClientUserItem * pIClientUserItem, bool bLookonUser)
{

}

//�滭����
BOOL CGameFrameWindow::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}

//λ����Ϣ
VOID CGameFrameWindow::OnSize(UINT nType, INT cx, INT cy) 
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}


//������Ϣ
INT CGameFrameWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//���ô���
	ModifyStyle(WS_CAPTION,0,0);
	ModifyStyle(WS_BORDER,WS_SYSMENU|WS_MINIMIZEBOX);
	ModifyStyleEx(WS_BORDER|WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE,0,0);

	//��������
	ASSERT(CGlobalUnits::GetInstance()!=NULL);
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

	//��ѯ�ӿ�
	m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
	m_pIGameFrameView=(IGameFrameView *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_VIEW,IID_IGameFrameView,VER_IGameFrameView);
	m_pIGameFrameService=(IGameFrameService *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_SERVICE,IID_IGameFrameService,VER_IGameFrameService);
	IGameFrameEngine* pIGameFrameEngine = (IGameFrameEngine *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_ENGINE,IID_IGameFrameEngine,VER_IGameFrameEngine);

	//�û��ӿ�
	IUserEventSink * pIUserEventSink=QUERY_ME_INTERFACE(IUserEventSink);
	if (pIUserEventSink!=NULL) m_pIClientKernel->SetUserEventSink(pIUserEventSink);

	//���ƴ���
	AfxSetResourceHandle(GetModuleHandle(GAME_FRAME_DLL_NAME));
	m_GameFrameControl.Create(8011,this);
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//����ӿ�
	m_pIClientKernel->SetStringMessage(QUERY_OBJECT_INTERFACE(m_StringMessage,IUnknownEx));
	
	//����λ��
	CRect rcArce;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);

	//��ȡλ��
	CSize SizeRestrict=m_CurWindowSize;
	//����λ��
	CRect rcNormalSize;			

	//λ�õ���
	SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
	SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

	//�ƶ�����
	rcNormalSize.top=(rcArce.Height()-SizeRestrict.cy)/2;
	rcNormalSize.left=(rcArce.Width()-SizeRestrict.cx)/2;
	rcNormalSize.right=rcNormalSize.left+SizeRestrict.cx;
	rcNormalSize.bottom=rcNormalSize.top+SizeRestrict.cy;
	SetWindowPos(NULL,rcNormalSize.left,rcNormalSize.top,rcNormalSize.Width(),rcNormalSize.Height(),SWP_NOZORDER);

	//������ͼ
	ASSERT(m_pIGameFrameView!=NULL);
	if (m_pIGameFrameView!=NULL) m_pIGameFrameView->CreateGameViewWnd(this,IDC_GAME_CLIENT_VIEW);

	//������ť
	//������ť
	CRect rcCreate(0,0,0,0);
	m_btSound.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SOUND);
	m_btMin.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MIN);
	m_btClose.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_CLOSE);
	m_btOption.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OPTION);

	m_btClose.SetButtonImage(IDB_CLOSE,AfxGetInstanceHandle(),false,false);
	m_btMin.SetButtonImage(IDB_MIN,AfxGetInstanceHandle(),false,false);
	m_btSound.SetButtonImage((pGlobalUnits->m_bMuteStatuts)?IDB_CLOSE_SOUND:IDB_OPEN_SOUND,AfxGetInstanceHandle(),false,false);
	m_btOption.SetButtonImage(IDB_OPTION,AfxGetInstanceHandle(),false,false);
	//����ͼƬ
	//m_PngFrameTop.LoadImage(AfxGetInstanceHandle(),TEXT("FRAME_TOP"));
	m_ImageTop.LoadFromResource(AfxGetInstanceHandle(),IDB_FRAME_TOP);

	//��������
	m_DFontEx.CreateFont(this, TEXT("����"), 12, 400 );

	return 0L;
}

//�����Ϣ
VOID CGameFrameWindow::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	if(Point.y <=30)
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y)); 

	return;
}

//�����Ϣ
VOID CGameFrameWindow::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	return __super::OnLButtonDblClk(nFlags,Point);
}

//���øı�
VOID CGameFrameWindow::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	__super::OnSettingChange(uFlags,lpszSection);
	return;
}

//������Ϣ
LRESULT	CGameFrameWindow::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
	//Ĭ�ϵ���
	LRESULT lResult=DefWindowProc(WM_SETTEXT,wParam,lParam);

	((CGameClientView*)m_pIGameFrameView)->SendMessage(WM_SET_CAPTION,wParam,lParam);
	//���±���
	Invalidate(TRUE);

	return lResult;
}

//��Ϸ����
bool CGameFrameWindow::ShowGameRule()
{
	return true;
}

//��󴰿�
bool CGameFrameWindow::MaxSizeWindow()
{	
	return true;
}
//��ԭ����
bool CGameFrameWindow::RestoreWindow()
{
	return true;
}

//��������
bool CGameFrameWindow::AllowGameSound(bool bAllowSound)
{
	return true;
}

//�Թۿ���
bool CGameFrameWindow::AllowGameLookon(DWORD dwUserID, bool bAllowLookon)
{
	return true;
}

//���ƽӿ�
bool CGameFrameWindow::OnGameOptionChange()
{
	return true;
}

//�����С
void CGameFrameWindow::ReSetDlgSize(int cx,int cy)
{
	m_CurWindowSize.SetSize(cx,cy);
	//����λ��
	CRect rcArce;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);

	//��ȡλ��
	CSize SizeRestrict=m_CurWindowSize;
	//����λ��
	CRect rcNormalSize;			

	//λ�õ���
//	SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
//	SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

	//�ƶ�����
	rcNormalSize.top=(rcArce.Height()-SizeRestrict.cy)/2;
	rcNormalSize.left=(rcArce.Width()-SizeRestrict.cx)/2;
	rcNormalSize.right=rcNormalSize.left+SizeRestrict.cx;
	rcNormalSize.bottom=rcNormalSize.top+SizeRestrict.cy;

	m_CurWindowSize.SetSize(rcNormalSize.Width(),rcNormalSize.Height());
	SetWindowPos(NULL,rcNormalSize.left,rcNormalSize.top,rcNormalSize.Width(),rcNormalSize.Height(),SWP_NOZORDER);

	return;
}

//�������
UINT CGameFrameWindow::OnNcHitTest(CPoint point)
{
	CFrameWnd::OnNcHitTest(point);   
	return HTCAPTION;

}

//////////////////////////////////////////////////////////////////////////////////
void CGameFrameWindow::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� __super::OnPaint()
	//m_PngFrameTop.DrawImage(&dc,0,0);
	m_ImageTop.TransDrawImage(&dc,0,0,RGB(255,0,255));

	CString str;
	CRect rcTittle(10,3,1024,25);
	GetWindowText(str);
	
	//��������
	INT nXMove[8]={1,1,1,0,-1,-1,-1,0};
	INT nYMove[8]={-1,0,1,1,1,0,-1,-1};

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(100,100,100));
	dc.SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//�滭�߿�
	for (INT i=0;i<CountArray(nXMove);i++)
	{
		//����λ��
		CRect rcTitleFrame;
		rcTitleFrame.top=rcTittle.top+nYMove[i];
		rcTitleFrame.left=rcTittle.left+nXMove[i];
		rcTitleFrame.right=rcTittle.right+nXMove[i];
		rcTitleFrame.bottom=rcTittle.bottom+nYMove[i];

		//�滭�ַ�
		dc.DrawText(str,&rcTitleFrame,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}
	
	//�滭����
	dc.SetTextColor(RGB(255,255,255));
	dc.DrawText(str,&rcTittle,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	//m_DFontEx.DrawText(&dc,str,&rcTittle,RGB(255,255,255),DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS);
}
