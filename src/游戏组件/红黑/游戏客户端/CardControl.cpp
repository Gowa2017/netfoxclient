#include "StdAfx.h"
#include "GameClient.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CCardControl::CCardControl()
{
	//״̬����
	m_bDisplayItem = false;
	m_bShowCardControl = true;

	//�˿�����
	m_wCardCount=0;
	ZeroMemory(m_CardItemArray,sizeof(m_CardItemArray));

	//λ�ñ���
	m_BenchmarkPos.SetPoint(0,0);

	return;
}

//��������
CCardControl::~CCardControl()
{
}

//��������
void CCardControl::Create(CWnd* pWnd)
{
	//������Դ
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageCard.LoadImage( hResInstance, TEXT("GAME_CARD"));

	//��ȡ��С
	m_CardSize.SetSize(m_ImageCard.GetWidth()/13,m_ImageCard.GetHeight()/5);
}

//�����˿�
bool CCardControl::SetCardData(WORD wCardCount)
{
	//Ч�����
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray)) return false;

	//���ñ���
	m_wCardCount=wCardCount;
	ZeroMemory(m_CardItemArray,sizeof(m_CardItemArray));

	return true;
}


//�����˿�
bool CCardControl::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
	//Ч�����
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray)) return false;

	//�˿���Ŀ
	m_wCardCount=wCardCount;

	//�����˿�
	for (WORD i=0;i<wCardCount;i++)
	{
		m_CardItemArray[i].bShoot=false;
		m_CardItemArray[i].cbCardData=cbCardData[i];
	}

	return true;
}


//��ȡ�˿�
WORD CCardControl::GetCardData(BYTE cbCardData[], WORD wBufferCount)
{
	//Ч�����
	ASSERT(wBufferCount>=m_wCardCount);
	if (wBufferCount<m_wCardCount) return 0;

	//�����˿�
	for (WORD i=0;i<m_wCardCount;i++) cbCardData[i]=m_CardItemArray[i].cbCardData;

	return m_wCardCount;
}

//��ȡ�˿�
WORD CCardControl::GetCardData(tagCardItem CardItemArray[], WORD wBufferCount)
{
	//Ч�����
	ASSERT(wBufferCount>=m_wCardCount);
	if (wBufferCount<m_wCardCount) return 0;

	//�����˿�
	CopyMemory(CardItemArray,m_CardItemArray,sizeof(tagCardItem)*m_wCardCount);

	return m_wCardCount;
}

//��׼λ��
VOID CCardControl::SetBenchmarkPos(INT nXPos, INT nYPos)
{
	//���ñ���
	m_BenchmarkPos.x = nXPos;
	m_BenchmarkPos.y = nYPos;
	return;
}


//�滭�˿�
VOID CCardControl::DrawCardControl(CDC * pDC)
{
	//��ʾ�ж�
	if (m_bShowCardControl==false) return;

	//�滭�˿�
	for (WORD i = 0 ; i < m_wCardCount; i++)
	{
		//��ȡ�˿�
		bool bShoot = m_CardItemArray[i].bShoot;
		BYTE cbCardData = m_CardItemArray[i].cbCardData;

		//��϶����
		if (cbCardData == SPACE_CARD_DATA) continue;

		//��Ļλ��
		INT nXDrawPos = 0; 
		INT nYDrawPos = 0;

		//�滭�˿�
		nXDrawPos = m_BenchmarkPos.x - m_CardSize.cx/2 - ((m_wCardCount - 1)*DEF_X_DISTANCE/2) + DEF_X_DISTANCE * i ;
		nYDrawPos = m_BenchmarkPos.y - m_CardSize.cy/2;
		DrawCard(pDC, cbCardData, nXDrawPos, nYDrawPos);

	}
	return;
}

//�滭�˿�
VOID CCardControl::DrawCard( CDC * pDC, BYTE cbCardData, int nPosX, int nPosY )
{
	CSize szCardSize(m_ImageCard.GetWidth()/13,m_ImageCard.GetHeight()/5);
	INT nXImagePos = 0;
	INT nYImagePos = 0;

	//ͼƬλ��
	if ((m_bDisplayItem==true)&&(cbCardData!=0))
	{
		if ((cbCardData==0x4E)||(cbCardData==0x4F))
		{
			nXImagePos = ((cbCardData&CARD_MASK_VALUE)%14)*szCardSize.cx;
			nYImagePos = ((cbCardData&CARD_MASK_COLOR)>>4)*szCardSize.cy;
		}
		else
		{
			nXImagePos = ((cbCardData&CARD_MASK_VALUE)-1)*szCardSize.cx;
			nYImagePos = ((cbCardData&CARD_MASK_COLOR)>>4)*szCardSize.cy;
		}
	}
	else
	{
		nXImagePos = szCardSize.cx*2;
		nYImagePos = szCardSize.cy*4;
	}

	m_ImageCard.DrawImage(pDC, nPosX, nPosY,szCardSize.cx,szCardSize.cy, nXImagePos, nYImagePos);
}

//��ȡλ��
CPoint CCardControl::GetBenchmarkPos()
{
	return m_BenchmarkPos;
}

//////////////////////////////////////////////////////////////////////////

