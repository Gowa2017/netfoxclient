#include "stdafx.h"
#include "MyEdit.h"
#include ".\myedit.h"
// CMyEdit
IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit()
{
  //��ʼ��Ϊϵͳ����ʹ�����ɫ
  m_crText = GetSysColor(COLOR_WINDOWTEXT);
  m_crBackGnd = GetSysColor(COLOR_WINDOW);
  m_font.CreatePointFont(60,TEXT("����"));
  m_brBackGnd.CreateSolidBrush(GetSysColor(COLOR_WINDOW));

}

CMyEdit::~CMyEdit()
{
}

BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
//WM_CTLCOLOR����Ϣ����
  ON_WM_CTLCOLOR_REFLECT()
  ON_WM_CHAR()
  ON_WM_NCCALCSIZE()
  ON_WM_CREATE()
  ON_WM_VSCROLL()
END_MESSAGE_MAP()

void CMyEdit::SetBackColor(COLORREF rgb)
{
  //�������ֱ�����ɫ
  m_crBackGnd = rgb;

  //�ͷžɵĻ�ˢ
  if(m_brBackGnd.GetSafeHandle())
  {
    m_brBackGnd.DeleteObject();
  }
  //ʹ�����ֱ�����ɫ�����µĻ�ˢ,ʹ�����ֿ򱳾������ֱ���һ��
  m_brBackGnd.CreateSolidBrush(rgb);
  //redraw
  Invalidate(TRUE);
}
void CMyEdit::SetTextColor(COLORREF rgb)
{
  //����������ɫ
  m_crText = rgb;
  //redraw
  Invalidate(TRUE);
}
void CMyEdit::SetTextFont(const LOGFONT &lf)
{
//�����µ�����,������ΪCEDIT������
  if(m_font.GetSafeHandle())
  {
    m_font.DeleteObject();
  }

  m_font.CreateFontIndirect(&lf);
  //CEdit::SetFont(&m_font);
  CRichEditCtrl::SetFont(&m_font);
  //redraw
  Invalidate(TRUE);
}
BOOL CMyEdit::GetTextFont(LOGFONT &lf)
{
  if(m_font.GetLogFont(&lf)!=0)
  {
    return TRUE;
  }
  return FALSE;
}

HBRUSH CMyEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
  //ˢ��ǰ�����ı���ɫ
  pDC->SetTextColor(m_crText);

  //ˢ��ǰ�����ı�����
  pDC->SetBkColor(m_crBackGnd);

  //���ػ�ˢ,�������������ؼ�����
  return m_brBackGnd;
}



void CMyEdit::PreSubclassWindow()
{
  // TODO: �ڴ����ר�ô����/����û���

  //CEdit::PreSubclassWindow();
  CRichEditCtrl::PreSubclassWindow();

  //���ù���
  //m_SkinScrollBar.InitScroolBar(this);
}

void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
  return;
  CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);
}


void CMyEdit::Init()
{
  InitializeFlatSB(this->m_hWnd);
  FlatSB_EnableScrollBar(this->m_hWnd, SB_BOTH, ESB_DISABLE_BOTH);

  CRect windowRect;
  GetWindowRect(&windowRect);

  int nTitleBarHeight = 0;

  CWnd* pParent = GetParent();

  if(pParent->GetStyle() & WS_CAPTION)
  {
    nTitleBarHeight = GetSystemMetrics(SM_CYSIZE);
  }


  int nDialogFrameHeight = 0;
  int nDialogFrameWidth = 0;
  if((pParent->GetStyle() & WS_BORDER))
  {
    nDialogFrameHeight = GetSystemMetrics(SM_CYDLGFRAME);
    nDialogFrameWidth = GetSystemMetrics(SM_CYDLGFRAME);
  }

  if(pParent->GetStyle() & WS_THICKFRAME)
  {
    nDialogFrameHeight+=1;
    nDialogFrameWidth+=1;
  }

  CRect rect1(windowRect.right-nDialogFrameWidth,windowRect.top-nTitleBarHeight-nDialogFrameHeight-1,windowRect.right+12-nDialogFrameWidth,windowRect.bottom+11-nTitleBarHeight-nDialogFrameHeight);
  CRect rect2(windowRect.left-nDialogFrameWidth,windowRect.bottom-nTitleBarHeight-nDialogFrameHeight-1,windowRect.right+1-nDialogFrameWidth,windowRect.bottom+11-nTitleBarHeight-nDialogFrameHeight);

  //CS1//m_SkinVerticleScrollbar.Create(NULL, WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP,rect1, pParent);

  //CS1//m_SkinVerticleScrollbar.pEdit = (CEdit  *)this;

}

void CMyEdit::ReSetLoc(int cx, int cy)
{
  CRect windowRect,clientRect;
  GetWindowRect(&windowRect);

  GetClientRect(&clientRect);

  //����λ��
  CRect rcArce;
  SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);

  //��ȡλ��
  CSize SizeRestrict(cx,cy);
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

  //CS1//m_SkinVerticleScrollbar.MoveWindow(rcNormalSize.Width()-196+clientRect.Width()+4,rcNormalSize.Height()-126+clientRect.top,16,clientRect.Height());

}

void CMyEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
  // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
  ModifyStyle(WS_HSCROLL | WS_VSCROLL,0,0);
  CRichEditCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

int CMyEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }
  Init();
  // TODO:  �ڴ������ר�õĴ�������

  //���ù���
  m_SkinScrollBar.InitScroolBar(this);

  return 0;
}

void CMyEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

  CRichEditCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
  //CS1//m_SkinVerticleScrollbar.UpdateThumbPosition();
}
