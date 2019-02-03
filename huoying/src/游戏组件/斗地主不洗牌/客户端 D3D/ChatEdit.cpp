#include "StdAfx.h"
#include "ChatEdit.h"
#include ".\chatedit.h"

///////////////////////////////////////////////////////////////////////////////////

//IMPLEMENT_DYNAMIC(CChatEdit, CRichEditCtrl)

BEGIN_MESSAGE_MAP(CChatEdit, CRichEditCtrl)
	
	ON_WM_CHAR()
	ON_WM_NCCALCSIZE()
	ON_WM_CREATE()
	ON_WM_VSCROLL()

	ON_CONTROL_REFLECT(EN_VSCROLL, OnEnVscroll)
//	ON_WM_PAINT()
//	ON_WM_NCPAINT()
//ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//���캯��
CChatEdit::CChatEdit()
{
	m_font.CreatePointFont(60,TEXT("����")); 
	m_crTextColor = GetSysColor(COLOR_WINDOWTEXT);
}

//��������
CChatEdit::~CChatEdit()
{
}

//��ʼ������
VOID CChatEdit::Initialization()
{
	InitializeFlatSB(this->m_hWnd);
	FlatSB_EnableScrollBar(this->m_hWnd, SB_BOTH, ESB_DISABLE_BOTH);
//	SetWindowLong(this->m_hWnd, GWL_EXSTYLE, GetWindowLong(this->m_hWnd,GWL_EXSTYLE)|WS_EX_TRANSPARENT); 

	CRect windowRect;
	GetWindowRect(&windowRect);

	int nTitleBarHeight = 0;
	CWnd* pParent = GetParent();
	if(pParent->GetStyle() & WS_CAPTION)
		nTitleBarHeight = GetSystemMetrics(SM_CYSIZE);

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
	m_SkinVerticleScrollbar.Create(NULL, WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP, rect1, pParent);
	m_SkinVerticleScrollbar.m_pEdit = (CEdit *)this;
	
	SetEventMask(ENM_SCROLL);
	
	m_PngBack.LoadImage(AfxGetInstanceHandle(),TEXT("DISPLAY_BACK"));
	return;
}

//�����ؼ�
VOID CChatEdit::RectifyControl(INT nWidth, INT nHeigth)
{
	CRect windowRect,clientRect;
	GetWindowRect(&windowRect);
	GetClientRect(&clientRect);

	////����λ��
	//CRect rcArce;
	//SystemParametersInfo(SPI_GETWORKAREA, 0, &rcArce, SPIF_SENDCHANGE);

	////��ȡλ��
	//CSize SizeRestrict(nWidth, nHeigth);

	////����λ��
	//CRect rcNormalSize;

	////λ�õ���
	//SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
	//SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

	////�ƶ�����
	//rcNormalSize.top=(rcArce.Height()-SizeRestrict.cy)/2;
	//rcNormalSize.left=(rcArce.Width()-SizeRestrict.cx)/2;
	//rcNormalSize.right=rcNormalSize.left+SizeRestrict.cx;
	//rcNormalSize.bottom=rcNormalSize.top+SizeRestrict.cy;

	m_SkinVerticleScrollbar.MoveWindow(nWidth - 5 - 20-1, nHeigth - 168-5, 16, 128);

	return;
}

//����������ɫ
VOID CChatEdit::SetTextColor(COLORREF rcTextColor)
{
	//����������ɫ
	m_crTextColor = rcTextColor;

	//ˢ�¿ؼ�
	Invalidate(TRUE);

	return;
}

//��������
VOID CChatEdit::SetTextFont(const LOGFONT &lfTextFont)
{
	//��������
	if (m_font.GetSafeHandle())
	{
		m_font.DeleteObject();
	}
	m_font.CreateFontIndirect(&lfTextFont);

	//�󶨿ؼ�
	CRichEditCtrl::SetFont(&m_font);

	//ˢ�¿ؼ�
	Invalidate(TRUE);

	return;
}

//��������
VOID CChatEdit::SetHideWindow(bool bHide)
{
	if (bHide == true)
	{
		m_SkinVerticleScrollbar.ShowWindow(SW_HIDE);
	}
	else
	{
		m_SkinVerticleScrollbar.ShowWindow(SW_SHOW);
	}

	return;
}

//��ȡ��ǰ����
BOOL CChatEdit::GetTextFont(LOGFONT &lfTextFont)
{
	if (m_font.GetLogFont(&lfTextFont) != 0) return TRUE;   

	return FALSE;
}

void CChatEdit::PreSubclassWindow()
{
	CRichEditCtrl::PreSubclassWindow();

	return;
}

//�����ؼ�
int CChatEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetEventMask(ENM_SCROLL);

	//��ʼ���ؼ�
	Initialization();

	return 0;
}

void CChatEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);

	return;
}

void CChatEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	ModifyStyle(WS_HSCROLL | WS_VSCROLL, 0, 0); 
	CRichEditCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);

	return;
}

void CChatEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CRichEditCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	m_SkinVerticleScrollbar.UpdateThumbPosition();

	return;
}

void CChatEdit::OnEnVscroll()
{
	m_SkinVerticleScrollbar.UpdateThumbPosition();
}

VOID CChatEdit::DrawScrollBar()
{
	m_SkinVerticleScrollbar.SendMessage(WM_PAINT,0,0);
	return;
}
//void CChatEdit::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//	// TODO: �ڴ˴������Ϣ����������
//	// ��Ϊ��ͼ��Ϣ���� CRichEditCtrl::OnPaint()
////	m_PngBack.DrawImage(&dc,0,0);
//	__super::OnPaint();
//}

//void CChatEdit::OnNcPaint()
//{
//	// TODO: �ڴ˴������Ϣ����������
//	// ��Ϊ��ͼ��Ϣ���� CRichEditCtrl::OnNcPaint()
//	CDC* pDC = GetDC();
////	m_PngBack.DrawImage(pDC,0,0);
//
//}

//BOOL CChatEdit::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
////	m_PngBack.DrawImage(pDC,0,0);
//	return CRichEditCtrl::OnEraseBkgnd(pDC);
//}
