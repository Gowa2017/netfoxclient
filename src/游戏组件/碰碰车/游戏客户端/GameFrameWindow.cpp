#include "StdAfx.h"
#include "Resource.h"
#include "GameFrameWindow.h"
#include "GameClientEngine.h"
//////////////////////////////////////////////////////////////////////////////////

//���ư�ť
#define IDC_MIN           228               //��С��ť
#define IDC_CLOSE         229                 //�رհ�ť
#define IDC_OPTION          227                 //���ð�ť��ʶ
#define IDC_SOUND         230                 //�������
#define IDC_MAX           231                 //���ť
//�ؼ���ʶ
#define IDC_SKIN_SPLITTER     200                 //��ֿؼ�
#define IDC_GAME_CLIENT_VIEW    201                 //��ͼ��ʶ

#define IDC_GAME_WND    202                 //��ͼ��ʶ

//��Ļλ��
#define BORAD_SIZE          6                 //�߿��С
#define CAPTION_SIZE        32                  //�����С

//�ؼ���С
#define SPLITTER_CX         0                 //��ֿ��
#define CAPTION_SIZE        32                  //�����С

//��Ļ����
#define LESS_SCREEN_CY        768                 //��С�߶�
#define LESS_SCREEN_CX        1224                //��С���

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

END_MESSAGE_MAP()


//���캯��
CFrameEncircle::CFrameEncircle()
{
}

//��������
CFrameEncircle::~CFrameEncircle()
{
}

//�滭����
bool CFrameEncircle::PreDrawEncircleImage(tagEncircleBMP & EncircleImage)
{
  //����ͼ��
  CBitImage ImageLogo;
  ImageLogo.LoadFromResource(AfxGetInstanceHandle(),TEXT("GAME_LOGO"));

  //�滭ͼ��
  if(ImageLogo.IsNull()==false)
  {
    //��������
    CDC * pDC=CDC::FromHandle(EncircleImage.ImageTL.GetDC());

    //�滭ͼ��
    //ImageLogo.TransDrawImage(pDC,10,4,RGB(255,0,255));

    //�ͷ���Դ
    EncircleImage.ImageTL.ReleaseDC();
  }

  return true;
}

//���캯��
CGameFrameWindow::CGameFrameWindow()
{
  //����ӿ�
  m_pIClientKernel=NULL;
  m_pIGameFrameView=NULL;
  m_pIGameFrameService=NULL;
  m_nScrollXMax=0L;
  m_nScrollYMax=0L;
  m_bShowControl=false;
  //��ܻ���
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
  //ע�����
  CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
  pGlobalUnits->RegisterGlobalModule(MODULE_GAME_FRAME_WND,QUERY_ME_INTERFACE(IUnknownEx));

  int cx = GetSystemMetrics(SM_CXSCREEN);
  int cy = GetSystemMetrics(SM_CYSCREEN);
  //m_CurWindowSize.SetSize(cx,cy);
  m_CurWindowSize.SetSize(1024+245,768);
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
  switch(nCommandID)
  {
    case IDC_MIN:       //��С��ť
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
    case IDC_MAX:       //���ť
    {
      ////���ڿ���
      //if (m_bMaxShow==true)
      //{
      //  RestoreWindow();
      //}
      //else
      //{
      //  MaxSizeWindow();
      //}

      ////���½���
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
    case IDC_CLOSE:       //�رհ�ť
    {
      //��ȡ�û�
      ASSERT(m_pIClientKernel!=NULL);
      IClientUserItem * pIClientUserItem=m_pIClientKernel->GetMeUserItem();

      //ǿ����ʾ
      if((pIClientUserItem!=NULL)&&(pIClientUserItem->GetUserStatus()==US_PLAYING))
      {
        //��ʾ��Ϣ
        CInformation Information(this);
        INT nRes=0;
        if(m_pIClientKernel->GetServerAttribute()->wServerType!=GAME_GENRE_MATCH)
        {
          nRes=Information.ShowMessageBox(TEXT("��������Ϸ�У�ǿ���˳������۷֣�ȷʵҪǿ����"),MB_ICONQUESTION|MB_YESNO,0);
        }
        else
        {
          nRes=Information.ShowMessageBox(TEXT("�����ڱ����У�ǿ���˳����ᱻϵͳ�йܣ���Ӯ�Ը���ȷʵҪǿ����"),MB_ICONQUESTION|MB_YESNO,0);
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

      //Ͷ�ݹر�
      PostMessage(WM_CLOSE,0,0);

      return TRUE;
    }
    case IDC_OPTION:    //ѡ�ť
    {
      //��ʾ����
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

//�����ؼ�
VOID CGameFrameWindow::RectifyControl(INT nWidth, INT nHeight)
{
  //״̬�ж�
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

  //��������
  CRect rcSplitter;
  rcSplitter.top=EncircleInfoFrame.nTBorder;
  rcSplitter.bottom=m_nScrollYMax-EncircleInfoFrame.nBBorder;

  //��ֿؼ�
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
  //�ƶ�׼��
  HDWP hDwp=BeginDeferWindowPos(32);
  UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;

  //��ѯ��Ϸ
  CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
  IGameFrameView * pIGameFrameView=(IGameFrameView *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_VIEW,IID_IGameFrameView,VER_IGameFrameView);

  ////��Ϸ��ͼ
  //if (pIGameFrameView!=NULL)
  //{
  //  CRect rcArce;
  //  CSize SizeRestrict=m_CurWindowSize;
  //  SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);
  //  SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
  //  SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);
  //  DeferWindowPos(hDwp, pIGameFrameView->GetGameViewHwnd(), NULL, EncircleInfoFrame.nLBorder, EncircleInfoFrame.nTBorder, rcSplitter.left, SizeRestrict.cy-EncircleInfoFrame.nTBorder-EncircleInfoFrame.nBBorder, uFlags);
  //}
  ////������Ϣ
  //tagEncircleInfo EncircleInfoFrame;
  //m_FrameEncircle.GetEncircleInfo(EncircleInfoFrame);

  //DeferWindowPos(hDwp, m_GameFrameControl, NULL, 0, 0, 0, 0, uFlags);
  //�ƶ��ؼ�
  //������Ϣ
  DeferWindowPos(hDwp,m_btMin,AfxGetMainWnd()->GetSafeHwnd(), nWidth - 10-25*3-5*2,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btSound,AfxGetMainWnd()->GetSafeHwnd(), nWidth - 10-25*2-5,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btClose,AfxGetMainWnd()->GetSafeHwnd(), nWidth - 10-25,2,0,0,uFlags|SWP_NOSIZE);
  DeferWindowPos(hDwp,m_btOption,AfxGetMainWnd()->GetSafeHwnd(), nWidth - 10-25*4-5*3,2,0,0,uFlags|SWP_NOSIZE);

  m_GameFrameControl.ShowWindow(false);
  DeferWindowPos(hDwp,m_SkinSplitter,NULL,rcSplitter.left,rcSplitter.top,rcSplitter.Width(),rcSplitter.Height(),uFlags);
  DeferWindowPos(hDwp,m_GameFrameControl,NULL,rcSplitter.right,EncircleInfoFrame.nTBorder,m_nScrollXMax-rcSplitter.right-EncircleInfoFrame.nRBorder,
                 m_nScrollYMax-EncircleInfoFrame.nBBorder-EncircleInfoFrame.nTBorder,uFlags);

  //��Ϸ��ͼ
  if(pIGameFrameView!=NULL)
  {
    HWND hWndView=pIGameFrameView->GetGameViewHwnd();

    CRect rcWnd;
    GetWindowRect(&rcWnd);

    if(rcWnd.Width()<LESS_SCREEN_CX)
    {
      //����ʾ�û��б�
      DeferWindowPos(hDwp,m_GameFrameControl,NULL,0,0,0,0,uFlags);


      //������Ϸ����
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
  if(pGlobalUnits->m_bNotifyUserInOut==true)
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
  //��ȡλ��
  CRect rcClient;
  GetClientRect(&rcClient);

  //���λ��
  tagEncircleInfo FrameEncircleInfo;
  m_FrameEncircle.GetEncircleInfo(FrameEncircleInfo);

  //�滭���
  m_FrameEncircle.DrawEncircleFrame(pDC,rcClient);

  CRect rcCaption(10,7,1024,30);
  //m_FontValleysListTwo.DrawText( pDC, m_strCaption, rcCaption, RGB(255,255,255), DT_VCENTER|DT_LEFT);
  pDC->SetBkMode(TRANSPARENT);
  pDC->SetTextColor(RGB(255,255,255));
  pDC->DrawText(m_strCaption,rcCaption,DT_LEFT);
  return TRUE;
}

//λ����Ϣ
VOID CGameFrameWindow::OnSize(UINT nType, INT cx, INT cy)
{
  __super::OnSize(nType, cx, cy);
  m_nScrollXMax=cx;
  m_nScrollYMax=__max(LESS_SCREEN_CY,cy);
  //�����ؼ�
  RectifyControl(cx,cy);

  return;
}


//������Ϣ
INT CGameFrameWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(__super::OnCreate(lpCreateStruct)==-1)
  {
    return -1;
  }

  //���ô���
  ModifyStyle(WS_CAPTION,0,0);
  ModifyStyleEx(WS_BORDER|WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE,0,0);

  //��������
  ASSERT(CGlobalUnits::GetInstance()!=NULL);
  CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();

  //��ѯ�ӿ�
  m_pIClientKernel=(IClientKernel *)pGlobalUnits->QueryGlobalModule(MODULE_CLIENT_KERNEL,IID_IClientKernel,VER_IClientKernel);
  m_pIGameFrameView=(IGameFrameView *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_VIEW,IID_IGameFrameView,VER_IGameFrameView);
  m_pIGameFrameService=(IGameFrameService *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_SERVICE,IID_IGameFrameService,VER_IGameFrameService);

  //�û��ӿ�
  IUserEventSink * pIUserEventSink=QUERY_ME_INTERFACE(IUserEventSink);
  if(pIUserEventSink!=NULL)
  {
    m_pIClientKernel->SetUserEventSink(pIUserEventSink);
  }
  m_FontValleysListTwo.CreateFont(this, TEXT("����"), 12, 400);
  //���ƴ���
  AfxSetResourceHandle(GetModuleHandle(GAME_FRAME_DLL_NAME));
  m_GameFrameControl.Create(8011,this);
  AfxSetResourceHandle(GetModuleHandle(NULL));

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
  if(m_pIGameFrameView!=NULL)
  {
    m_pIGameFrameView->CreateGameViewWnd(this,IDC_GAME_CLIENT_VIEW);
  }
  //�����ؼ�
  CRect rcCreate(0,0,0,0);
  HINSTANCE hResInstance=AfxGetInstanceHandle();
  //��ܰ�ť
  m_btOption.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OPTION);
  m_btMin.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MIN);
  m_btClose.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_CLOSE);
  m_btSound.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_SOUND);

  m_btOption.SetButtonImage(IDB_BT_OPTION,hResInstance,false,false);
  m_btMin.SetButtonImage(IDB_BT_MIN,hResInstance,false,false);
  m_btClose.SetButtonImage(IDB_BT_CLOSE,hResInstance,false,false);

  m_btSound.SetButtonImage(IDB_BT_SOUND_OPEN,hResInstance,false,false);
  ////������ͼ
  //ASSERT(m_pIGameFrameWnd!=NULL);
  //if (m_pIGameFrameWnd!=NULL) m_pIGameFrameWnd->Create(this,IDC_GAME_WND);

  return 0L;
}

//�����Ϣ
VOID CGameFrameWindow::OnLButtonDown(UINT nFlags, CPoint Point)
{
  __super::OnLButtonDown(nFlags,Point);
  if(Point.y <=30)
  {
    PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));
  }

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
LRESULT CGameFrameWindow::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
  //Ĭ�ϵ���
  LRESULT lResult=DefWindowProc(WM_SETTEXT,wParam,lParam);
  m_strCaption = (LPCTSTR)lParam;

  //((CGameClientView*)m_pIGameFrameView)->SendMessage(WM_SET_CAPTION,wParam,lParam);

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
  SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
  SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

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