#include "StdAfx.h"
#include "Resource.h"
#include "NumberControl.h"

//////////////////////////////////////////////////////////////////////////

#define		MAX_SCROLL_COUNT			1
#define		MAX_EXCUSION				2

//////////////////////////////////////////////////////////////////////////

//���캯��
CNumberControl::CNumberControl()
{
	//λ�ñ���
	m_BenchmarkPos.SetPoint(0,0);
	m_XCollocateMode = enXLeft;
	m_rcDraw.SetRectEmpty();

	//���ֱ���
	m_lScore = 0L;

	//��������
	m_nYExcusion = 0;
	m_nScrollCount = 0;
	m_nScrollIndex = 0;
	m_bMoving = FALSE;

	//λͼ����
	HINSTANCE hInst = AfxGetInstanceHandle();
	m_PngScoreNum.LoadImage(hInst,TEXT("SCORE_NUM"));
	m_PngScoreBack.LoadImage(hInst,TEXT("SCORE_BACK"));
}

//��������
CNumberControl::~CNumberControl()
{
}

//���û�׼
VOID CNumberControl::SetBencbmarkPos( INT nXPos, INT nYPos, enXCollocateMode XCollocateMode )
{
	//����滭����ǿ�,����
	if( !m_rcDraw.IsRectEmpty() )
	{
		CRect rc = m_rcDraw;
		rc.OffsetRect( nXPos,nYPos );
		m_rcDraw |= rc;
	}
	m_BenchmarkPos.SetPoint(nXPos,nYPos);
	m_XCollocateMode = XCollocateMode;
}

//��������
VOID CNumberControl::SetScore( LONGLONG lScore )
{
	m_lScore = lScore;
}

//��ʼ����
BOOL CNumberControl::BeginScrollNumber()
{
	//�����ڶ���,����FALSE
	if( m_bMoving ) return FALSE;

	//������Ϊ0,�򷵻�
	if( m_lScore == 0 ) return FALSE;

	//���ñ���
	m_arScoreNum.RemoveAll();
	m_rcDraw.SetRectEmpty();

	//���춯������
	LONGLONG lScore = m_lScore;
	if( lScore < 0 ) lScore = -lScore;
	BYTE byCellNum = 0;
	do
	{
		if( lScore != 0 )
			byCellNum = BYTE(lScore%10);
		lScore /= 10L;
		m_arScoreNum.Add(byCellNum);
	}while( lScore != 0 );
	m_nYExcusion = 0;
	m_nScrollIndex = 0;
	m_nScrollCount = (rand()%MAX_SCROLL_COUNT)*10;
	m_nScrollCount += m_arScoreNum[0];

	//����Ϊ0
	m_arScoreNum[0] = 0;
	m_bMoving = TRUE;

	return TRUE;
}

//��������
BOOL CNumberControl::PlayScrollNumber()
{
	if( m_lScore == 0 ) return FALSE;

	//����Ƿ���ɶ���
	if( m_nScrollCount == 0 )
	{
		//�Ƿ�ȫ����ɶ���
		if( m_nScrollIndex + 1 == m_arScoreNum.GetCount() ) 
		{
			m_bMoving = FALSE;
			return FALSE;
		}

		//������������
		m_nScrollIndex++;

		//����ƫ��
		m_nYExcusion = 0;

		//���ɹ�������
		m_nScrollCount = (rand()%MAX_SCROLL_COUNT)*10;
		m_nScrollCount += m_arScoreNum[m_nScrollIndex];

		//��0
		m_arScoreNum[m_nScrollIndex] = 0;
	};

	//����ƫ��
	if( ++m_nYExcusion == MAX_EXCUSION )
	{
		m_nYExcusion = 0;

		//�Լ���������
		m_nScrollCount--;

		//���ӹ�������
		BYTE &byCellNum = m_arScoreNum[m_nScrollIndex];
		byCellNum = (byCellNum+1)%10;
	}
	return TRUE;
}

//ֹͣ��������
BOOL CNumberControl::FinishScrollNumber()
{
	if( !m_bMoving && m_lScore == 0 ) return FALSE;

	//�������δ���
	if( m_arScoreNum.GetCount() >= m_nScrollIndex+1 && m_nScrollCount > 0 )
	{
		//���õ�ǰ��������
		BYTE &byCellNum = m_arScoreNum[m_nScrollIndex];
		byCellNum = (byCellNum+m_nScrollCount)%10;
	}
	//�������δ��ʼ
	else if( m_arScoreNum.GetCount() == 0 )
	{
		LONGLONG lScore = m_lScore;
		BYTE byCellNum = 0;
		do
		{
			if( lScore != 0 )
				byCellNum = BYTE(lScore%10);
			lScore /= 10L;
			m_arScoreNum.Add(byCellNum);
		}while( lScore != 0 );
	}

	//���ù�������
	m_nScrollIndex = m_arScoreNum.GetCount()-1;

	//�ù�������0
	m_nScrollCount = 0;

	//��ƫ��0
	m_nYExcusion = 0;

	m_bMoving = FALSE;

	//���ø��»滭����
	m_rcDraw.SetRectEmpty();

	return TRUE;
}

//���ÿؼ�
VOID CNumberControl::ResetControl()
{
	//���ñ���
	m_lScore = 0;
	m_nScrollCount = 0;
	m_nScrollIndex = 0;
	m_nYExcusion = 0;
	m_arScoreNum.RemoveAll();
	m_rcDraw.SetRectEmpty();
	m_bMoving = FALSE;
}

//�滭�ؼ�
VOID CNumberControl::DrawNumberControl( CDC *pDC )
{
	//��ȡ����
	if( m_arScoreNum.GetCount() == 0 ) return;
	
	//����滭��
	CPoint ptOrg;
	GetOriginPoint(ptOrg);
	INT nXPos = ptOrg.x;
	INT nYPos = ptOrg.y;

	//�滭����
	INT nScoreBackWidth = m_PngScoreBack.GetWidth()/4;
	INT nScoreBackHeight = m_PngScoreBack.GetHeight();
	m_PngScoreBack.DrawImage(pDC,nXPos,nYPos,nScoreBackWidth,nScoreBackHeight,
		(m_lScore<0L)?nScoreBackWidth:0,0);
	nXPos += nScoreBackWidth;

	INT nNumWidth = m_PngScoreNum.GetWidth()/10;
	INT nNumHeight = m_PngScoreNum.GetHeight();

	//�滭����
	for( INT_PTR i = m_arScoreNum.GetCount()-1; i >= 0; i-- )
	{
		//������ڻ�С�ڵ�ǰ��������,��0
		if( i > m_nScrollIndex || i < m_nScrollIndex )
		{
			m_PngScoreBack.DrawImage(pDC,nXPos,nYPos,nScoreBackWidth,nScoreBackHeight,
				nScoreBackWidth*(m_lScore<0L?3:2),0);

			//��Ԫ����
			BYTE byCellNum = 0;
			if( i < m_nScrollIndex )
				byCellNum = m_arScoreNum[i];
			m_PngScoreNum.DrawImage(pDC,nXPos,nYPos+4,nNumWidth,nNumHeight,byCellNum*nNumWidth,0);
		}
		//������ڵ�ǰ��������
		else
		{
			m_PngScoreBack.DrawImage(pDC,nXPos,nYPos,nScoreBackWidth,nScoreBackHeight,
				nScoreBackWidth*(m_lScore<0L?3:2),0);

			//ƫ��
			INT nYExcusion = m_nYExcusion*nNumHeight/MAX_EXCUSION;
			BYTE byCellNum = m_arScoreNum[i];
			if( nNumHeight - nYExcusion > 0 )
				m_PngScoreNum.DrawImage(pDC,nXPos,nYPos+4,nNumWidth,nNumHeight-nYExcusion,byCellNum*nNumWidth,nYExcusion);
			byCellNum = (byCellNum+1)%10;
			if( nYExcusion > 0 )
				m_PngScoreNum.DrawImage(pDC,nXPos,nYPos+4+nNumHeight-nYExcusion,nNumWidth,nYExcusion,byCellNum*nNumWidth,0);
		}
		nXPos += nNumWidth;
	}
}

//��ȡ�滭��ʼλ��
VOID CNumberControl::GetOriginPoint( CPoint &OriginPoint )
{
	//��ȡλ��
	CSize ControlSize;
	ControlSize.cy = m_PngScoreBack.GetHeight();
	ControlSize.cx = m_PngScoreBack.GetWidth()/4*2+(LONG)((m_arScoreNum.GetCount()-1)*m_PngScoreNum.GetWidth()/10);

	//����λ��
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ OriginPoint.x=m_BenchmarkPos.x; break; }
	case enXCenter: { OriginPoint.x=m_BenchmarkPos.x-ControlSize.cx/2; break; }
	case enXRight:	{ OriginPoint.x=m_BenchmarkPos.x-ControlSize.cx; break; }
	}
	OriginPoint.y = m_BenchmarkPos.y;
	return;
}

//��ȡ���»滭����
VOID CNumberControl::GetDrawRect( CRect &rc )
{
	//�����������Ϊ0,��ȡ�����ؼ�����
	if( m_nScrollIndex == 0 )
	{
		CPoint ptOrg;
		GetOriginPoint(ptOrg);
		CSize ControlSize;
		ControlSize.cy = m_PngScoreBack.GetHeight();
		ControlSize.cx = m_PngScoreBack.GetWidth()/4*2+(LONG)((m_arScoreNum.GetCount()-1)*m_PngScoreNum.GetWidth()/10);
		rc.SetRect(ptOrg.x,ptOrg.y,ptOrg.x+ControlSize.cx,ptOrg.y+ControlSize.cy);
	}
	//��ȡ��������
	else if( m_arScoreNum.GetCount() >= m_nScrollIndex+1 && m_nScrollCount > 0 )
	{
		CPoint ptOrg;
		GetOriginPoint(ptOrg);
		INT nScoreBackWidht = m_PngScoreBack.GetWidth()/4;
		INT nScoreBackHeith = m_PngScoreBack.GetHeight();
		INT nScoreWidth = m_PngScoreNum.GetWidth()/10;
		INT nScoreHeight = m_PngScoreNum.GetHeight();
		ptOrg.Offset(nScoreBackWidht+nScoreWidth*(INT)(m_arScoreNum.GetCount()-m_nScrollIndex-1),0);
		rc.SetRect(ptOrg.x,ptOrg.y,ptOrg.x+nScoreWidth,ptOrg.y+nScoreBackHeith);
		m_rcDraw = rc;
	}
	else
	{
		rc = m_rcDraw;
	}
	return;
}

//////////////////////////////////////////////////////////////////////////