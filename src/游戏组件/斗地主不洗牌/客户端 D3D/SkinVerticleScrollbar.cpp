// SkinVerticleScrollbar.cpp : implementation file
//

#include "stdafx.h"
#include "SkinVerticleScrollbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinVerticleScrollbar

CSkinVerticleScrollbar::CSkinVerticleScrollbar()
{
	m_bMouseDown = false;
	m_bMouseDownArrowUp = false;
	m_bMouseDownArrowDown = false;

	m_nThumbTop = NHEIGHT-1;
	m_dbThumbInterval = 0.000000;
	m_pList = NULL;
	m_pEdit = NULL;

}

CSkinVerticleScrollbar::~CSkinVerticleScrollbar()
{
}


BEGIN_MESSAGE_MAP(CSkinVerticleScrollbar, CStatic)

	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinVerticleScrollbar message handlers

BOOL CSkinVerticleScrollbar::OnEraseBkgnd(CDC* pDC) 
{
	return CStatic::OnEraseBkgnd(pDC);
}

void CSkinVerticleScrollbar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	CRect clientRect;
	GetClientRect(&clientRect);

	int nHeight = clientRect.Height() ;
	CRect rectUpArrow(0,0,NWIDTH,NHEIGHT);
	CRect rectDownArrow(0,nHeight-NHEIGHT,NWIDTH,nHeight);
	CRect rectThumb(0,m_nThumbTop,NWIDTH,m_nThumbTop+NHEIGHT);

	if (rectThumb.PtInRect(point))
	{
		m_bMouseDown = true;
	}

	if (rectDownArrow.PtInRect(point))
	{
		m_bMouseDownArrowDown = true;
	}

	if (rectUpArrow.PtInRect(point))
	{
		m_bMouseDownArrowUp = true;
	}
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CSkinVerticleScrollbar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	bool bInChannel = true;

	CRect clientRect;
	GetClientRect(&clientRect);
	int nHeight = clientRect.Height() ;
	
	CRect rectUpArrow(0,0,NWIDTH,NHEIGHT);
	CRect rectDownArrow(0,nHeight-NHEIGHT,NWIDTH,nHeight);
	CRect rectThumb(0,m_nThumbTop,NWIDTH,m_nThumbTop+NHEIGHT);

	if(rectUpArrow.PtInRect(point) && m_bMouseDownArrowUp)
	{
		ScrollUp();
		bInChannel = false;
	}

	if(rectDownArrow.PtInRect(point) && m_bMouseDownArrowDown)
	{
		ScrollDown();
		bInChannel = false;
	}

	if(rectThumb.PtInRect(point))
	{
		bInChannel = false;
	}

	if(bInChannel == true && m_bMouseDown != true)
	{
		if(point.y > m_nThumbTop)
		{
			PageDown();
		}
		else
		{
			PageUp();
		}
	}

	m_bMouseDown = false;
	m_bMouseDownArrowUp = false;
	m_bMouseDownArrowDown = false;
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CSkinVerticleScrollbar::OnMouseMove(UINT nFlags, CPoint point) 
{
	//数据效验
	if (m_pList == NULL && m_pEdit == NULL) return;

	if (m_bMouseDown == true)
	{
		if (m_pList != NULL)
		{
			//m_pList->Scroll(size);
		}
		else
		{
			//定义变量
			CRect ClientRect;
			GetClientRect(&ClientRect);
			int nLineCount = m_pEdit->GetLineCount();
			
			//满屏绘画
			if (nLineCount > 8)
			{
				m_nThumbTop = point.y-NHEIGHT/2;
				LimitThumbPosition();
				double nHeight = ClientRect.Height() - NHEIGHT * 3 - 1;
				m_pEdit->LineScroll((int)(m_pEdit->GetLineCount() * (m_nThumbTop - NHEIGHT - 1) / nHeight) - m_pEdit->GetFirstVisibleLine());
				TRACE(TEXT("m_nThumbTop = %d, point.y = %d, nHeight = %d\n"), (int)m_nThumbTop, point.y, (int)nHeight);
				Draw();
			}
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CSkinVerticleScrollbar::OnPaint() 
{
	CPaintDC dc(this); 
	
	Draw();

	return;
}

void CSkinVerticleScrollbar::OnTimer(UINT nIDEvent) 
{
	return;
}

void CSkinVerticleScrollbar::PageDown()
{
	
	if (m_pList!=NULL)
	{
		m_pList->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEDOWN,0),NULL);
	}
	else
	{
		m_pEdit->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEDOWN,0),NULL);
	}
	
	UpdateThumbPosition();

	return;
}

void CSkinVerticleScrollbar::PageUp()
{
	if (m_pList!=NULL)
	{
		m_pList->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEUP,0),NULL);
	}
	else
	{
		m_pEdit->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEUP,0),NULL);
	}
	
	UpdateThumbPosition();

	return;
}

void CSkinVerticleScrollbar::ScrollUp()
{	
	if (m_pList!=NULL)
	{
		m_pList->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0),NULL);
	}
	else
	{
		m_pEdit->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0),NULL);
	}
	
	UpdateThumbPosition();

	return;
}

void CSkinVerticleScrollbar::ScrollDown()
{
	if (m_pList!=NULL)
	{
		m_pList->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0),NULL);
	}
	else
	{
		m_pEdit->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0),NULL);
	}
	
	UpdateThumbPosition();

	return;
}

void CSkinVerticleScrollbar::UpdateThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	double nPos ;
	double nMax ;

	if (m_pList!=NULL)
	{
		 nPos = m_pList->GetScrollPos(SB_VERT);
		 nMax = m_pList->GetScrollLimit(SB_VERT);
	}
	else
	{
		 nPos = m_pEdit->GetScrollPos(SB_VERT);
		 nMax = m_pEdit->GetScrollLimit(SB_VERT);
	}

	double nHeight = (clientRect.Height()-NHEIGHT*3);
	double nVar = nMax;
	m_dbThumbInterval = nHeight/nVar;
	double nNewdbValue = (m_dbThumbInterval * nPos);
	int nNewValue = (int)nNewdbValue;
	m_nThumbTop = NHEIGHT-1+nNewValue;

	LimitThumbPosition();
	Draw();

	return;
}

void CSkinVerticleScrollbar::Draw()
{
	CClientDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);
	CMemDC memDC(&dc, &clientRect);
	memDC.FillSolidRect(&clientRect,  RGB(54,47,36));
	CDC bitmapDC;
	bitmapDC.CreateCompatibleDC(&dc);

	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_SCROLL_BAR);
	CBitmap* pOldBitmap = bitmapDC.SelectObject(&bitmap);
	
	int indexX=0;
	int indexY=0;
	int nHeight = clientRect.Height();

	indexX=5*NWIDTH+5*3;
	for(int i=0; i<nHeight; i++)
	{
		memDC.BitBlt(clientRect.left,(clientRect.top)+(i),NWIDTH,1,&bitmapDC,indexX,indexY,SRCCOPY);
	}

	indexX=0;
	indexY=0;
	memDC.BitBlt(clientRect.left,clientRect.top,NWIDTH,NHEIGHT,&bitmapDC,indexX,indexY,SRCCOPY);

	indexX=NWIDTH+3;
	memDC.BitBlt(clientRect.left,nHeight-NHEIGHT,NWIDTH,NHEIGHT,&bitmapDC,indexX,indexY,SRCCOPY);
	int nThumbLength=GetScrollBarLength();

	indexX=2*NWIDTH+2*3;
	int start=clientRect.top+m_nThumbTop+1;
	if (start+nThumbLength>clientRect.Height()-NHEIGHT)
	{
		start=clientRect.Height()-NHEIGHT-nThumbLength;
	}
	memDC.StretchBlt(clientRect.left,start,NWIDTH,nThumbLength,&bitmapDC,indexX,indexY,NWIDTH,NHEIGHT,SRCCOPY);

	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;

	return;
}

void CSkinVerticleScrollbar::LimitThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	if(m_nThumbTop+GetScrollBarLength() > (clientRect.Height()-NHEIGHT))
	{
		m_nThumbTop = clientRect.Height()-NHEIGHT-GetScrollBarLength()-1;
	}

	if(m_nThumbTop < (clientRect.top+NHEIGHT))
	{
		m_nThumbTop = clientRect.top+NHEIGHT-1;
	}

	return;
}

int CSkinVerticleScrollbar::GetScrollBarLength()
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS | SIF_RANGE;

	CRect clientRect;
	GetClientRect(&clientRect);

	int nThumbLength=NWIDTH;
	if (m_pList!=NULL)
	{
		//m_pList->GetScrollInfo(SB_VERT,&si);
		//nThumbLength = si.nPage*(clientRect.Height() - NHEIGHT*2)/(si.nPage+si.nMax-si.nMin);
	}

	if (m_pEdit!=NULL)
	{
		//m_pEdit->GetScrollInfo(SB_VERT,&si);
		//nThumbLength = si.nPage*(clientRect.Height() - NHEIGHT*2)/(si.nPage+si.nMax-si.nMin);
	}

	return nThumbLength;
}

