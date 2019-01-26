#include "StdAfx.h"
#include "Resource.h"
#include "GameFrameWindow.h"
#include "GameClientview.h"
#include ".\gameframewindow.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//控制按钮
#define IDC_MIN						100									//最小按钮
#define IDC_CLOSE					101									//关闭按钮

#define IDC_BT_MIN					102
#define IDC_BT_CLOSE				103
#define IDC_SOUND					104
#define IDC_OPTION					105

//控件标识
#define IDC_SKIN_SPLITTER			200									//拆分控件
#define IDC_GAME_CLIENT_VIEW		201									//视图标识

//屏幕位置
#define BORAD_SIZE					6									//边框大小
#define CAPTION_SIZE				32									//标题大小

//控件大小
#define SPLITTER_CX					0									//拆分宽度
#define CAPTION_SIZE				32									//标题大小

//屏幕限制
#define LESS_SCREEN_CY				740									//最小高度
#define LESS_SCREEN_CX				1024								//最小宽度

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameFrameWindow, CFrameWnd)

	//系统消息
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETTINGCHANGE()
	ON_WM_NCHITTEST()

	//自定消息
	ON_MESSAGE(WM_SETTEXT,OnSetTextMesage)

	ON_WM_PAINT()
END_MESSAGE_MAP()

//构造函数
CGameFrameWindow::CGameFrameWindow()
{
	//组件接口
	m_pIClientKernel=NULL;
	m_pIGameFrameView=NULL;
	m_pIGameFrameService=NULL;

	//注册组件
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
	pGlobalUnits->RegisterGlobalModule(MODULE_GAME_FRAME_WND,QUERY_ME_INTERFACE(IUnknownEx));
	
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	//m_CurWindowSize.SetSize(cx,cy);
	m_CurWindowSize.SetSize(1024,768);
}

//析构函数
CGameFrameWindow::~CGameFrameWindow()
{
}

//接口查询
VOID * CGameFrameWindow::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IUserEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameFrameWnd,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameFrameWnd,Guid,dwQueryVer);
	return NULL;
}

//消息解释
BOOL CGameFrameWindow::PreTranslateMessage(MSG * pMsg)
{
	return __super::PreTranslateMessage(pMsg);
}

//命令函数
BOOL CGameFrameWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	UINT nCommandID=LOWORD(wParam);

	//功能按钮
	switch (nCommandID)
	{
	case IDC_MIN:				//最小按钮
		{
			if (IsIconic())	
				ShowWindow(SW_RESTORE);
			else
				ShowWindow(SW_MINIMIZE);
			
			return TRUE;
		}
	case IDC_CLOSE:				//关闭按钮
		{
			PostMessage(WM_CLOSE,0,0);
			return TRUE;
		}
	case IDC_OPTION:
		{
			//显示配置
			CDlgGameOption DlgGameOption;
			if (DlgGameOption.CreateGameOption(NULL,0)==true)
			{
				//变量定义
				ASSERT(CGlobalUnits::GetInstance()!=NULL);
				CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

				//查询接口
				IGameFrameEngine* pIGameFrameEngine=(IGameFrameEngine *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_ENGINE,IID_IGameFrameEngine,VER_IGameFrameEngine);
				CGameClientEngine* pGameClientEngine = (CGameClientEngine*)pIGameFrameEngine;
				pGameClientEngine->AllowBackGroundSound(pGlobalUnits->m_bAllowBackGroundSound);
				break;
			}
			return TRUE;
		}
	case IDC_SOUND:
		{
			//查询游戏
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

//调整控件
VOID CGameFrameWindow::RectifyControl(INT nWidth, INT nHeight)
{
	//状态判断
	if ((nWidth==0)||(nHeight==0)) return;

	//移动准备
	HDWP hDwp=BeginDeferWindowPos(32);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;

	//查询游戏
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
	IGameFrameView * pIGameFrameView=(IGameFrameView *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_VIEW,IID_IGameFrameView,VER_IGameFrameView);

	//游戏视图
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

	//移动结束
	EndDeferWindowPos(hDwp);

	return;
}

//用户进入
VOID CGameFrameWindow::OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	//变量定义
	ASSERT(CGlobalUnits::GetInstance()!=NULL);
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

	//提示信息
	if (pGlobalUnits->m_bNotifyUserInOut==true)
	{
		//CDialogChat::m_ChatMessage.InsertUserEnter(pIClientUserItem->GetNickName());
	}

	return;
}

//用户离开
VOID CGameFrameWindow::OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	return;
}

//用户积分
VOID CGameFrameWindow::OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser)
{

	return;
}

//用户状态
VOID CGameFrameWindow::OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	return;
}

//用户头像
VOID CGameFrameWindow::OnEventCustomFace(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	return;

}

//用户属性
VOID CGameFrameWindow::OnEventUserAttrib(IClientUserItem * pIClientUserItem, bool bLookonUser)
{

}

//绘画背景
BOOL CGameFrameWindow::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}

//位置消息
VOID CGameFrameWindow::OnSize(UINT nType, INT cx, INT cy) 
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}


//建立消息
INT CGameFrameWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置窗口
	ModifyStyle(WS_CAPTION,0,0);
	ModifyStyle(WS_BORDER,WS_SYSMENU|WS_MINIMIZEBOX);
	ModifyStyleEx(WS_BORDER|WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE,0,0);

	//变量定义
	ASSERT(CGlobalUnits::GetInstance()!=NULL);
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

	//查询接口
	m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
	m_pIGameFrameView=(IGameFrameView *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_VIEW,IID_IGameFrameView,VER_IGameFrameView);
	m_pIGameFrameService=(IGameFrameService *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_SERVICE,IID_IGameFrameService,VER_IGameFrameService);
	IGameFrameEngine* pIGameFrameEngine = (IGameFrameEngine *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_ENGINE,IID_IGameFrameEngine,VER_IGameFrameEngine);

	//用户接口
	IUserEventSink * pIUserEventSink=QUERY_ME_INTERFACE(IUserEventSink);
	if (pIUserEventSink!=NULL) m_pIClientKernel->SetUserEventSink(pIUserEventSink);

	//控制窗口
	AfxSetResourceHandle(GetModuleHandle(GAME_FRAME_DLL_NAME));
	m_GameFrameControl.Create(8011,this);
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//聊天接口
	m_pIClientKernel->SetStringMessage(QUERY_OBJECT_INTERFACE(m_StringMessage,IUnknownEx));
	
	//窗口位置
	CRect rcArce;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);

	//读取位置
	CSize SizeRestrict=m_CurWindowSize;
	//正常位置
	CRect rcNormalSize;			

	//位置调整
	SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
	SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

	//移动窗口
	rcNormalSize.top=(rcArce.Height()-SizeRestrict.cy)/2;
	rcNormalSize.left=(rcArce.Width()-SizeRestrict.cx)/2;
	rcNormalSize.right=rcNormalSize.left+SizeRestrict.cx;
	rcNormalSize.bottom=rcNormalSize.top+SizeRestrict.cy;
	SetWindowPos(NULL,rcNormalSize.left,rcNormalSize.top,rcNormalSize.Width(),rcNormalSize.Height(),SWP_NOZORDER);

	//创建视图
	ASSERT(m_pIGameFrameView!=NULL);
	if (m_pIGameFrameView!=NULL) m_pIGameFrameView->CreateGameViewWnd(this,IDC_GAME_CLIENT_VIEW);

	//创建按钮
	//创建按钮
	CRect rcCreate(0,0,0,0);
	m_btSound.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SOUND);
	m_btMin.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MIN);
	m_btClose.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_CLOSE);
	m_btOption.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OPTION);

	m_btClose.SetButtonImage(IDB_CLOSE,AfxGetInstanceHandle(),false,false);
	m_btMin.SetButtonImage(IDB_MIN,AfxGetInstanceHandle(),false,false);
	m_btSound.SetButtonImage((pGlobalUnits->m_bMuteStatuts)?IDB_CLOSE_SOUND:IDB_OPEN_SOUND,AfxGetInstanceHandle(),false,false);
	m_btOption.SetButtonImage(IDB_OPTION,AfxGetInstanceHandle(),false,false);
	//加载图片
	//m_PngFrameTop.LoadImage(AfxGetInstanceHandle(),TEXT("FRAME_TOP"));
	m_ImageTop.LoadFromResource(AfxGetInstanceHandle(),IDB_FRAME_TOP);

	//创建字体
	m_DFontEx.CreateFont(this, TEXT("宋体"), 12, 400 );

	return 0L;
}

//鼠标消息
VOID CGameFrameWindow::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	if(Point.y <=30)
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y)); 

	return;
}

//鼠标消息
VOID CGameFrameWindow::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	return __super::OnLButtonDblClk(nFlags,Point);
}

//设置改变
VOID CGameFrameWindow::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	__super::OnSettingChange(uFlags,lpszSection);
	return;
}

//标题消息
LRESULT	CGameFrameWindow::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
	//默认调用
	LRESULT lResult=DefWindowProc(WM_SETTEXT,wParam,lParam);

	((CGameClientView*)m_pIGameFrameView)->SendMessage(WM_SET_CAPTION,wParam,lParam);
	//更新标题
	Invalidate(TRUE);

	return lResult;
}

//游戏规则
bool CGameFrameWindow::ShowGameRule()
{
	return true;
}

//最大窗口
bool CGameFrameWindow::MaxSizeWindow()
{	
	return true;
}
//还原窗口
bool CGameFrameWindow::RestoreWindow()
{
	return true;
}

//声音控制
bool CGameFrameWindow::AllowGameSound(bool bAllowSound)
{
	return true;
}

//旁观控制
bool CGameFrameWindow::AllowGameLookon(DWORD dwUserID, bool bAllowLookon)
{
	return true;
}

//控制接口
bool CGameFrameWindow::OnGameOptionChange()
{
	return true;
}

//重设大小
void CGameFrameWindow::ReSetDlgSize(int cx,int cy)
{
	m_CurWindowSize.SetSize(cx,cy);
	//窗口位置
	CRect rcArce;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);

	//读取位置
	CSize SizeRestrict=m_CurWindowSize;
	//正常位置
	CRect rcNormalSize;			

	//位置调整
//	SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
//	SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

	//移动窗口
	rcNormalSize.top=(rcArce.Height()-SizeRestrict.cy)/2;
	rcNormalSize.left=(rcArce.Width()-SizeRestrict.cx)/2;
	rcNormalSize.right=rcNormalSize.left+SizeRestrict.cx;
	rcNormalSize.bottom=rcNormalSize.top+SizeRestrict.cy;

	m_CurWindowSize.SetSize(rcNormalSize.Width(),rcNormalSize.Height());
	SetWindowPos(NULL,rcNormalSize.left,rcNormalSize.top,rcNormalSize.Width(),rcNormalSize.Height(),SWP_NOZORDER);

	return;
}

//点击测试
UINT CGameFrameWindow::OnNcHitTest(CPoint point)
{
	CFrameWnd::OnNcHitTest(point);   
	return HTCAPTION;

}

//////////////////////////////////////////////////////////////////////////////////
void CGameFrameWindow::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 __super::OnPaint()
	//m_PngFrameTop.DrawImage(&dc,0,0);
	m_ImageTop.TransDrawImage(&dc,0,0,RGB(255,0,255));

	CString str;
	CRect rcTittle(10,3,1024,25);
	GetWindowText(str);
	
	//变量定义
	INT nXMove[8]={1,1,1,0,-1,-1,-1,0};
	INT nYMove[8]={-1,0,1,1,1,0,-1,-1};

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(100,100,100));
	dc.SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//绘画边框
	for (INT i=0;i<CountArray(nXMove);i++)
	{
		//计算位置
		CRect rcTitleFrame;
		rcTitleFrame.top=rcTittle.top+nYMove[i];
		rcTitleFrame.left=rcTittle.left+nXMove[i];
		rcTitleFrame.right=rcTittle.right+nXMove[i];
		rcTitleFrame.bottom=rcTittle.bottom+nYMove[i];

		//绘画字符
		dc.DrawText(str,&rcTitleFrame,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}
	
	//绘画文字
	dc.SetTextColor(RGB(255,255,255));
	dc.DrawText(str,&rcTittle,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	//m_DFontEx.DrawText(&dc,str,&rcTittle,RGB(255,255,255),DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS);
}
