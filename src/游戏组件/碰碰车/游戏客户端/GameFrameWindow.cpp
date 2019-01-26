#include "StdAfx.h"
#include "Resource.h"
#include "GameFrameWindow.h"
#include "GameClientEngine.h"
//////////////////////////////////////////////////////////////////////////////////

//控制按钮
#define IDC_MIN           228               //最小按钮
#define IDC_CLOSE         229                 //关闭按钮
#define IDC_OPTION          227                 //设置按钮标识
#define IDC_SOUND         230                 //声音那妞
#define IDC_MAX           231                 //最大按钮
//控件标识
#define IDC_SKIN_SPLITTER     200                 //拆分控件
#define IDC_GAME_CLIENT_VIEW    201                 //视图标识

#define IDC_GAME_WND    202                 //视图标识

//屏幕位置
#define BORAD_SIZE          6                 //边框大小
#define CAPTION_SIZE        32                  //标题大小

//控件大小
#define SPLITTER_CX         0                 //拆分宽度
#define CAPTION_SIZE        32                  //标题大小

//屏幕限制
#define LESS_SCREEN_CY        768                 //最小高度
#define LESS_SCREEN_CX        1224                //最小宽度

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

END_MESSAGE_MAP()


//构造函数
CFrameEncircle::CFrameEncircle()
{
}

//析构函数
CFrameEncircle::~CFrameEncircle()
{
}

//绘画处理
bool CFrameEncircle::PreDrawEncircleImage(tagEncircleBMP & EncircleImage)
{
  //加载图标
  CBitImage ImageLogo;
  ImageLogo.LoadFromResource(AfxGetInstanceHandle(),TEXT("GAME_LOGO"));

  //绘画图标
  if(ImageLogo.IsNull()==false)
  {
    //变量定义
    CDC * pDC=CDC::FromHandle(EncircleImage.ImageTL.GetDC());

    //绘画图标
    //ImageLogo.TransDrawImage(pDC,10,4,RGB(255,0,255));

    //释放资源
    EncircleImage.ImageTL.ReleaseDC();
  }

  return true;
}

//构造函数
CGameFrameWindow::CGameFrameWindow()
{
  //组件接口
  m_pIClientKernel=NULL;
  m_pIGameFrameView=NULL;
  m_pIGameFrameService=NULL;
  m_nScrollXMax=0L;
  m_nScrollYMax=0L;
  m_bShowControl=false;
  //框架环绕
  tagEncircleResource EncircleFrame;
  EncircleFrame.pszImageTL=MAKEINTRESOURCE(IDB_FRAME_TL);
  EncircleFrame.pszImageTM=MAKEINTRESOURCE(IDB_FRAME_TM);
  EncircleFrame.pszImageTR=MAKEINTRESOURCE(IDB_FRAME_TR);
  EncircleFrame.pszImageML=MAKEINTRESOURCE(IDB_FRAME_ML);
  EncircleFrame.pszImageMR=MAKEINTRESOURCE(IDB_FRAME_MR);
  EncircleFrame.pszImageBL=MAKEINTRESOURCE(IDB_FRAME_BL);
  EncircleFrame.pszImageBM=MAKEINTRESOURCE(IDB_FRAME_BM);
  EncircleFrame.pszImageBR=MAKEINTRESOURCE(IDB_FRAME_BR);

  m_FrameEncircle.InitEncircleResource(EncircleFrame,GetModuleHandle(GAME_FRAME_DLL_NAME));
  //注册组件
  CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
  pGlobalUnits->RegisterGlobalModule(MODULE_GAME_FRAME_WND,QUERY_ME_INTERFACE(IUnknownEx));

  int cx = GetSystemMetrics(SM_CXSCREEN);
  int cy = GetSystemMetrics(SM_CYSCREEN);
  //m_CurWindowSize.SetSize(cx,cy);
  m_CurWindowSize.SetSize(1024+245,768);
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
  switch(nCommandID)
  {
    case IDC_MIN:       //最小按钮
    {
      if(IsIconic())
      {
        ShowWindow(SW_RESTORE);
      }
      else
      {
        ShowWindow(SW_MINIMIZE);
      }

      return TRUE;
    }
    case IDC_MAX:       //最大按钮
    {
      ////窗口控制
      //if (m_bMaxShow==true)
      //{
      //  RestoreWindow();
      //}
      //else
      //{
      //  MaxSizeWindow();
      //}

      ////更新界面
      //RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_ERASENOW|RDW_UPDATENOW);

      return TRUE;
    }
    case IDC_SOUND:
    {
      //SendEngineMessage(IDM_GAME_SOUND,0,0);
      ASSERT(CGameFrameEngine::GetInstance()!=NULL);
      CGameFrameEngine *pGameFrameEngine=CGameFrameEngine::GetInstance();
      HINSTANCE hInstance=AfxGetInstanceHandle();
      static bool bSound=true;
      if(bSound)
      {
        bSound=false;
        m_btSound.SetButtonImage(IDB_BT_SOUND_CLOSE,hInstance,false,false);
      }
      else
      {
        bSound=true;
        m_btSound.SetButtonImage(IDB_BT_SOUND_OPEN,hInstance,false,false);
      }
      pGameFrameEngine->PostMessage(IDM_GAME_SOUND,0,0);
      return TRUE;
    }
    case IDC_CLOSE:       //关闭按钮
    {
      //获取用户
      ASSERT(m_pIClientKernel!=NULL);
      IClientUserItem * pIClientUserItem=m_pIClientKernel->GetMeUserItem();

      //强退提示
      if((pIClientUserItem!=NULL)&&(pIClientUserItem->GetUserStatus()==US_PLAYING))
      {
        //提示消息
        CInformation Information(this);
        INT nRes=0;
        if(m_pIClientKernel->GetServerAttribute()->wServerType!=GAME_GENRE_MATCH)
        {
          nRes=Information.ShowMessageBox(TEXT("您正在游戏中，强行退出将被扣分，确实要强退吗？"),MB_ICONQUESTION|MB_YESNO,0);
        }
        else
        {
          nRes=Information.ShowMessageBox(TEXT("您正在比赛中，强行退出将会被系统托管，输赢自负，确实要强退吗？"),MB_ICONQUESTION|MB_YESNO,0);
        }
        if(nRes!=IDYES)
        {
          return TRUE;
        }
      }
      if(m_pIClientKernel->GetServerAttribute()->wServerType==GAME_GENRE_MATCH)
      {
        m_pIClientKernel->SendProcessData(IPC_CMD_GF_MATCH_INFO,IPC_SUB_GF_EXIT_MATCH);
      }

      //投递关闭
      PostMessage(WM_CLOSE,0,0);

      return TRUE;
    }
    case IDC_OPTION:    //选项按钮
    {
      //显示配置
      CDlgGameOption DlgGameOption;
      if(DlgGameOption.CreateGameOption(NULL,0)==true)
      {
        return true;
      }
      return true;
    }

  }

  return __super::OnCommand(wParam,lParam);
}

//调整控件
VOID CGameFrameWindow::RectifyControl(INT nWidth, INT nHeight)
{
  //状态判断
  if((nWidth==0)||(nHeight==0))
  {
    return;
  }

  //if (nWidth<1008+336)
  //{
  //  SetWindowPos(NULL,9,29,1008,704,NULL);
  //  nWidth =1008;
  //  nHeight=704;
  //}
  tagEncircleInfo EncircleInfoFrame;
  m_FrameEncircle.GetEncircleInfo(EncircleInfoFrame);

  //变量定义
  CRect rcSplitter;
  rcSplitter.top=EncircleInfoFrame.nTBorder;
  rcSplitter.bottom=m_nScrollYMax-EncircleInfoFrame.nBBorder;

  //拆分控件
  if(m_bShowControl==true)
  {
    rcSplitter.left=m_nScrollXMax*75L/100L;
    rcSplitter.right=rcSplitter.left+SPLITTER_CX;
  }
  else
  {
    rcSplitter.left=m_nScrollXMax-EncircleInfoFrame.nRBorder-SPLITTER_CX;
    rcSplitter.right=m_nScrollXMax-EncircleInfoFrame.nRBorder;
  }
  rcSplitter.left=nWidth-245;
  rcSplitter.right=rcSplitter.left+SPLITTER_CX;
  //移动准备
  HDWP hDwp=BeginDeferWindowPos(32);
  UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;

  //查询游戏
  CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
  IGameFrameView * pIGameFrameView=(IGameFrameView *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_VIEW,IID_IGameFrameView,VER_IGameFrameView);

  ////游戏视图
  //if (pIGameFrameView!=NULL)
  //{
  //  CRect rcArce;
  //  CSize SizeRestrict=m_CurWindowSize;
  //  SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);
  //  SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
  //  SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);
  //  DeferWindowPos(hDwp, pIGameFrameView->GetGameViewHwnd(), NULL, EncircleInfoFrame.nLBorder, EncircleInfoFrame.nTBorder, rcSplitter.left, SizeRestrict.cy-EncircleInfoFrame.nTBorder-EncircleInfoFrame.nBBorder, uFlags);
  //}
  ////环绕信息
  //tagEncircleInfo EncircleInfoFrame;
  //m_FrameEncircle.GetEncircleInfo(EncircleInfoFrame);

  //DeferWindowPos(hDwp, m_GameFrameControl, NULL, 0, 0, 0, 0, uFlags);
  //移动控件
  //环绕信息
  DeferWindowPos(hDwp,m_btMin,AfxGetMainWnd()->GetSafeHwnd(), nWidth - 10-25*3-5*2,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btSound,AfxGetMainWnd()->GetSafeHwnd(), nWidth - 10-25*2-5,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btClose,AfxGetMainWnd()->GetSafeHwnd(), nWidth - 10-25,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btOption,AfxGetMainWnd()->GetSafeHwnd(), nWidth - 10-25*4-5*3,2,0,0,uFlags|SWP_NOSIZE);

  m_GameFrameControl.ShowWindow(false);
  DeferWindowPos(hDwp,m_SkinSplitter,NULL,rcSplitter.left,rcSplitter.top,rcSplitter.Width(),rcSplitter.Height(),uFlags);
  DeferWindowPos(hDwp,m_GameFrameControl,NULL,rcSplitter.right,EncircleInfoFrame.nTBorder,m_nScrollXMax-rcSplitter.right-EncircleInfoFrame.nRBorder,
                 m_nScrollYMax-EncircleInfoFrame.nBBorder-EncircleInfoFrame.nTBorder,uFlags);

  //游戏视图
  if(pIGameFrameView!=NULL)
  {
    HWND hWndView=pIGameFrameView->GetGameViewHwnd();

    CRect rcWnd;
    GetWindowRect(&rcWnd);

    if(rcWnd.Width()<LESS_SCREEN_CX)
    {
      //不显示用户列表
      DeferWindowPos(hDwp,m_GameFrameControl,NULL,0,0,0,0,uFlags);


      //增加游戏区域
      DeferWindowPos(hDwp,hWndView,NULL,EncircleInfoFrame.nLBorder,EncircleInfoFrame.nTBorder,rcWnd.Width()-EncircleInfoFrame.nRBorder-5,
                     m_nScrollYMax-EncircleInfoFrame.nTBorder-EncircleInfoFrame.nBBorder,uFlags);
    }
    else
    {
      m_GameFrameControl.ShowWindow(true);

      DeferWindowPos(hDwp,hWndView,NULL,EncircleInfoFrame.nLBorder,EncircleInfoFrame.nTBorder,
                     rcSplitter.left-EncircleInfoFrame.nLBorder,
                     m_nScrollYMax-EncircleInfoFrame.nTBorder-EncircleInfoFrame.nBBorder,uFlags);
    }
  }


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
  if(pGlobalUnits->m_bNotifyUserInOut==true)
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
  //获取位置
  CRect rcClient;
  GetClientRect(&rcClient);

  //框架位置
  tagEncircleInfo FrameEncircleInfo;
  m_FrameEncircle.GetEncircleInfo(FrameEncircleInfo);

  //绘画框架
  m_FrameEncircle.DrawEncircleFrame(pDC,rcClient);

  CRect rcCaption(10,7,1024,30);
  //m_FontValleysListTwo.DrawText( pDC, m_strCaption, rcCaption, RGB(255,255,255), DT_VCENTER|DT_LEFT);
  pDC->SetBkMode(TRANSPARENT);
  pDC->SetTextColor(RGB(255,255,255));
  pDC->DrawText(m_strCaption,rcCaption,DT_LEFT);
  return TRUE;
}

//位置消息
VOID CGameFrameWindow::OnSize(UINT nType, INT cx, INT cy)
{
  __super::OnSize(nType, cx, cy);
  m_nScrollXMax=cx;
  m_nScrollYMax=__max(LESS_SCREEN_CY,cy);
  //调整控件
  RectifyControl(cx,cy);

  return;
}


//建立消息
INT CGameFrameWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(__super::OnCreate(lpCreateStruct)==-1)
  {
    return -1;
  }

  //设置窗口
  ModifyStyle(WS_CAPTION,0,0);
  ModifyStyleEx(WS_BORDER|WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE,0,0);

  //变量定义
  ASSERT(CGlobalUnits::GetInstance()!=NULL);
  CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

  //查询接口
  m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
  m_pIGameFrameView=(IGameFrameView *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_VIEW,IID_IGameFrameView,VER_IGameFrameView);
  m_pIGameFrameService=(IGameFrameService *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_SERVICE,IID_IGameFrameService,VER_IGameFrameService);

  //用户接口
  IUserEventSink * pIUserEventSink=QUERY_ME_INTERFACE(IUserEventSink);
  if(pIUserEventSink!=NULL)
  {
    m_pIClientKernel->SetUserEventSink(pIUserEventSink);
  }
  m_FontValleysListTwo.CreateFont(this, TEXT("宋体"), 12, 400);
  //控制窗口
  AfxSetResourceHandle(GetModuleHandle(GAME_FRAME_DLL_NAME));
  m_GameFrameControl.Create(8011,this);
  AfxSetResourceHandle(GetModuleHandle(NULL));

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
  if(m_pIGameFrameView!=NULL)
  {
    m_pIGameFrameView->CreateGameViewWnd(this,IDC_GAME_CLIENT_VIEW);
  }
  //创建控件
  CRect rcCreate(0,0,0,0);
  HINSTANCE hResInstance=AfxGetInstanceHandle();
  //框架按钮
  m_btOption.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OPTION);
  m_btMin.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MIN);
  m_btClose.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_CLOSE);
  m_btSound.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SOUND);

  m_btOption.SetButtonImage(IDB_BT_OPTION,hResInstance,false,false);
  m_btMin.SetButtonImage(IDB_BT_MIN,hResInstance,false,false);
  m_btClose.SetButtonImage(IDB_BT_CLOSE,hResInstance,false,false);

  m_btSound.SetButtonImage(IDB_BT_SOUND_OPEN,hResInstance,false,false);
  ////创建视图
  //ASSERT(m_pIGameFrameWnd!=NULL);
  //if (m_pIGameFrameWnd!=NULL) m_pIGameFrameWnd->Create(this,IDC_GAME_WND);

  return 0L;
}

//鼠标消息
VOID CGameFrameWindow::OnLButtonDown(UINT nFlags, CPoint Point)
{
  __super::OnLButtonDown(nFlags,Point);
  if(Point.y <=30)
  {
    PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));
  }

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
LRESULT CGameFrameWindow::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
  //默认调用
  LRESULT lResult=DefWindowProc(WM_SETTEXT,wParam,lParam);
  m_strCaption = (LPCTSTR)lParam;

  //((CGameClientView*)m_pIGameFrameView)->SendMessage(WM_SET_CAPTION,wParam,lParam);

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
  SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
  SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

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