#include "Stdafx.h"
#include "Math.h"
#include "Resource.h"
#include "GoldControl.h"
#include ".\goldcontrol.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

#define CELL_WIDTH							23							//��Ԫ���
#define LESS_WIDTH							70							//��С���
#define SPACE_WIDTH							12							//��Ϯ���
#define BUTTON_WIDTH						0							//��������
#define CONTROL_HEIGHT						50							//�ؼ��߶�

#define IDC_MAX_SCORE					116								//���ť
#define IDC_MIN_SCORE					117								//���ٰ�ť
#define IDC_CONFIRM						118								//ȷ����ť
#define	IDC_CANCEL_ADD					120								//ȡ����ť

#define	IDM_CONFIRM					WM_USER+109							//ȷ����Ϣ	
#define	IDM_CANCEL_ADD				WM_USER+119							//ȡ����Ϣ	
#define IDM_MIN_SCORE				WM_USER+102							//���ټ�ע
#define IDM_MAX_SCORE				WM_USER+103							//����ע

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGoldControl, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGoldControl::CGoldControl()
{
	m_bMingTag=false;
	m_nCellCount=0;
	m_lAllGold=0;

	//�������
	m_nWidth=0;
	m_lGoldCount[0]=10;
	m_lGoldCount[1]=20;
	m_lGoldCount[2]=50;
	m_AppendWidth=0;

	//���ñ���
	m_lMaxGold=0L;
	m_lMinGold=0L;
	memset(m_lGoldCell,0,sizeof(m_lGoldCell));

	HINSTANCE hInstance=AfxGetInstanceHandle();
//	m_ImageMoney.LoadFromResource(hInstance,IDB_GOLD);
///	m_ImageBack.LoadFromResource(hInstance,IDB_ADD_INFO);
//	m_ImageNumber.LoadFromResource(hInstance,IDB_NUMBER);
}

//��������
CGoldControl::~CGoldControl()
{
}

//���ó���
void CGoldControl::SetGoldCount(LONGLONG lCellSocre)
{
	if(lCellSocre>0L)
	{
		for (int i=0;i<3;i++)
		{
			m_lGoldCount[i]*=(lCellSocre/10);
		}
	}
	else 
	{
		m_lGoldCount[0]=10;
		m_lGoldCount[1]=20;
		m_lGoldCount[2]=50;
	}
	return;
}

//���ý��
void CGoldControl::SetGold(LONGLONG lGold)
{
	//��������
	if (lGold>m_lMaxGold)m_lMaxGold= lGold;
	memset(m_lGoldCell,0,sizeof(m_lGoldCell));
	m_lAllGold=lGold;

	//���ñ���
	int nIndex=0;
	while (lGold>0L)
	{
		m_lGoldCell[nIndex++]=lGold%10L;
		lGold/=10L;
	}

	//�ػ�����
	Invalidate(FALSE);

	return;
}

//���ٵ�Ԫ
void CGoldControl::SetMinGold(LONGLONG lMinGold)
{
	ASSERT(lMinGold%10==0);

	//���ñ���
	m_lAllGold=lMinGold;
	m_lMinGold=lMinGold;

	//�ػ�����
	Invalidate(FALSE);
	return;
}

//�����û������ע��
void CGoldControl::SetMaxGold(LONGLONG lMaxGold)
{
	//Ч��ı�
	if (m_lMaxGold==lMaxGold) return;

	//���ñ���
	m_lMaxGold=lMaxGold;
	if (m_lMaxGold>999999999L) m_lMaxGold=999999999L;
	memset(m_lGoldCell,0,sizeof(m_lGoldCell));

	//���㵥Ԫ
	m_nCellCount=0;
	while (lMaxGold>0L)
	{
		lMaxGold/=10L;
		m_nCellCount++;
	}
	m_nCellCount=__min(CountArray(m_lGoldCell),__max(m_nCellCount,1));

	//�ػ�����
	//Invalidate(FALSE);

	return;
}

//����λ��
void CGoldControl::SetBasicPoint(int nXPos, int nYPos)
{
	//���ñ���
	m_BasicPoint.x=nXPos;
	m_BasicPoint.y=nYPos;

	//��������
	RectifyControl();
	return;
}

//�����ؼ�
void CGoldControl::RectifyControl()
{
	MoveWindow(m_BasicPoint.x,m_BasicPoint.y,m_ImageBack.GetWidth(),m_ImageBack.GetHeight());
	return;
}

//�ػ�����
void CGoldControl::OnPaint()
{
	CPaintDC dc(this);

	//��ȡλ��
	CRect ClientRect;
	GetClientRect(&ClientRect);

	//��������ͼ
	CDC BackFaceDC;
	CBitmap BufferBmp;
	BufferBmp.CreateCompatibleBitmap(&dc,ClientRect.Width(),ClientRect.Height());
	BackFaceDC.CreateCompatibleDC(&dc);
	BackFaceDC.SelectObject(&BufferBmp);

	//�滭����
	m_ImageBack.BitBlt(BackFaceDC,0,0);

	//�滭����
	for (int i=0;i<3;i++)
	{
		int iX=0;
		if(m_lGoldCount[0] == 100)iX=3;
		else if(m_lGoldCount[0] == 1000)iX=6;
		else if(m_lGoldCount[0] == 10000)iX=9;
		else if(m_lGoldCount[0] == 100000)iX=12;
		else if(m_lGoldCount[0] == 1000000)iX=15;
		m_ImageMoney.TransDrawImage(&BackFaceDC,(i)*(m_ImageMoney.GetWidth()/18+5)+17,m_ImageMoney.GetHeight()/2+12,
			m_ImageMoney.GetWidth()/18,m_ImageMoney.GetHeight(),m_ImageMoney.GetWidth()/18*(i+iX),0,RGB(255,0,255));
	}

	//�滭���
	int nXExcursion=ClientRect.Width()/2+20;

	//�滭����
	LONGLONG lGold=m_lAllGold*((m_bMingTag)?2:1);
	int iCount=0;
	while (lGold>0)
	{
		LONGLONG lTemp=lGold%10;
		m_ImageNumber.TransDrawImage(&BackFaceDC,nXExcursion-m_ImageNumber.GetWidth()/10*(iCount++),5,
			m_ImageNumber.GetWidth()/10,m_ImageNumber.GetHeight(),
			m_ImageNumber.GetWidth()/10*(int)lTemp,0,RGB(255,0,255));
		lGold/=10;
	}

	//�滭����
	dc.BitBlt(0,0,ClientRect.Width(),ClientRect.Height(),&BackFaceDC,0,0,SRCCOPY);

	return;
}

//���������Ϣ
void CGoldControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	//λ�ù���
	int nX = m_ImageMoney.GetWidth()/3+27;
	int nY = m_ImageMoney.GetHeight()/2+9;
	if ((point.x <=15) || (point.x>=nX)) return;
	if ((point.y>=77) || (point.y<=nY)) return;

	//��ť����
	int iCellPos=(point.x-20)/48+1;
	if(iCellPos==4)iCellPos=3;

	//��������
	ASSERT((iCellPos>=0)&&(iCellPos<=3));
	LONGLONG lAddGold=m_lGoldCount[iCellPos-1];
	if ((GetGold()+lAddGold)>m_lMaxGold) return;

	//��������
	m_lAllGold=GetGold()+lAddGold;

	LONGLONG lGold=m_lAllGold;
	int nIndex=0;
	while (lGold>0L)
	{
		m_lGoldCell[nIndex++]=lGold%10L;
		lGold/=10L;
	}

	//�ػ�����
	Invalidate(FALSE);

	return;
}

//���ù��
BOOL CGoldControl::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	//��ȡ���
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	//λ�ù���
	int nX = m_ImageMoney.GetWidth()/3+27;
	int nY = m_ImageMoney.GetHeight()/2+9;
	if ((point.x <=15) || (point.x>=nX) || (point.y>=77) || (point.y<=nY))
		return __super::OnSetCursor(pWnd, nHitTest, message);

	//���ù��
	SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
	return TRUE;

}

//////////////////////////////////////////////////////////////////////////

int CGoldControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CGoldControl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CWnd::OnCommand(wParam, lParam);
}
