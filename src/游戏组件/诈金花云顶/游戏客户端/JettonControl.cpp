#include "StdAfx.h"
#include "GameClient.h"
#include "JettonControl.h"

//////////////////////////////////////////////////////////////////////////

#define X_RADIUS					100									//�滭����
#define	Y_RADIUS					70									//�滭����

#define MAX_DRAW					30									//�滭��������

#define MOVE_STEP_LEN				40									//����

//////////////////////////////////////////////////////////////////////////


LONGLONG			CJettonControl::m_lJetonIndex[MAX_JETTON_INDEX] = {
	1L,5L,10L,50L,100L,500L,1000L,5000L,10000L,50000L,100000L,500000L,1000000L,5000000L};

//���캯��
CJettonControl::CJettonControl()
{
	//���ñ���
	m_lScore=0L;
	m_BenchmarkPos.SetPoint(0,0);

	//��������
	m_lScorePending = 0L;
	m_rcDraw.SetRectEmpty();
	m_rcMove.SetRectEmpty();

	//����λͼ
	HINSTANCE hInst = AfxGetInstanceHandle();
	m_PngJetton.LoadImage(hInst,TEXT("JETTON_VIEW"));

	m_nJettonHeight = m_PngJetton.GetHeight();
	m_nJettonWidth = m_PngJetton.GetWidth()/MAX_JETTON_INDEX;

	return;
}

//��������
CJettonControl::~CJettonControl()
{
}

//���ÿؼ�
VOID CJettonControl::ResetControl()
{
	//���ñ���
	m_lScore=0L;

	//��������
	m_arJetIndex.RemoveAll();
	m_arJetExcusions.RemoveAll();

	//��������
	m_arJetPending.RemoveAll();

	m_arIndexPending.RemoveAll();
	m_arExcusionsPend.RemoveAll();
	m_arStepCount.RemoveAll();
	m_arXStep.RemoveAll();
	m_arYStep.RemoveAll();

	//������
	m_lScorePending = 0L;
	m_rcDraw.SetRectEmpty();
	m_rcMove.SetRectEmpty();
}

//�����ƶ�����
VOID CJettonControl::ComputeMoveRect()
{
	//Ч���Ƿ��д�������
	if( m_arExcusionsPend.GetCount() == 0 ) return;

	//��ȡƫ��
	CPoint &ptExcusion = m_arExcusionsPend[0];
	m_rcMove.SetRect(m_BenchmarkPos.x+ptExcusion.x-1,m_BenchmarkPos.y+ptExcusion.y-1,
		m_BenchmarkPos.x+ptExcusion.x+1,m_BenchmarkPos.y+ptExcusion.y+1);
	for( INT_PTR i = 1; i < m_arExcusionsPend.GetCount(); i++ )
	{
		CPoint &pt = m_arExcusionsPend[i];
		CPoint ptCur(m_BenchmarkPos.x+pt.x,m_BenchmarkPos.y+pt.y);
		if( !m_rcMove.PtInRect(ptCur) )
		{
			m_rcMove |= CRect(ptCur.x-1,ptCur.y-1,ptCur.x+1,ptCur.y+1);
		}
	}
}

//���춯��
BOOL CJettonControl::ConstructCartoon()
{	
	//�������δ��ɶ���,�򷵻�FALSE
	if( m_arIndexPending.GetCount() > 0 ) return FALSE;

	//���޴�������,�򷵻�
	if( m_arJetPending.GetCount() == 0 ) return FALSE;

	//�����������
	m_arExcusionsPend.RemoveAll();
	m_arStepCount.RemoveAll();
	m_arXStep.RemoveAll();
	m_arYStep.RemoveAll();
	m_lScorePending = 0L;
	m_rcDraw.SetRectEmpty();
	m_rcMove.SetRectEmpty();

	//��ȡ��������
	LONGLONG lFirstScore = m_arJetPending[0].lScore;
	for( ; ; )
	{
		tagJetPending &jp = m_arJetPending[0];

		//����0�ֳ���,�ͷ������һ���෴�ĳ���
		if( jp.lScore == 0L || (lFirstScore>0L&&jp.lScore<0L) || (lFirstScore<0L&&jp.lScore>0L)
			) 
		{
			if( jp.lScore == 0L )
				m_arJetPending.RemoveAt(0);
			break;
		}

		//�����������
		LONGLONG lJetScore = jp.lScore;
		while( lJetScore != 0 )
		{
			//�������
			WORD wIndexPending;
			CPoint ptFrom,ptTo,ptExcusion;
			INT nStepCount,nXStep,nYStep;

			//�����������
			if( lJetScore > 0L )
			{
				//��ȡ����
				for( WORD i = MAX_JETTON_INDEX-1; i >= 0; i-- )
				{
					if( lJetScore >= m_lJetonIndex[i] )
					{
						wIndexPending = i;
						lJetScore -= m_lJetonIndex[i];
						break;
					}
				}
				//������ʼ��
				ptFrom = jp.ptJetton;

				//����Ŀ���
				ptTo.x = m_BenchmarkPos.x + (rand()%X_RADIUS)*((rand()%100)>49?1:-1);
				ptTo.y = m_BenchmarkPos.y + (rand()%Y_RADIUS)*((rand()%100)>49?1:-1);

				//���ɲ���
				if( abs(ptTo.x-ptFrom.x) > abs(ptTo.y-ptFrom.y) )
					nStepCount = abs(ptTo.x-ptFrom.x)/MOVE_STEP_LEN;
				else nStepCount = abs(ptTo.y-ptFrom.y)/MOVE_STEP_LEN;

				//����X����
				nXStep = (ptTo.x-ptFrom.x)/nStepCount;

				//����Y����
				nYStep = (ptTo.y-ptFrom.y)/nStepCount;

				//����ƫ��
				ptExcusion.x = ptFrom.x-m_BenchmarkPos.x;
				ptExcusion.y = ptFrom.y-m_BenchmarkPos.y;
			}
			//����
			else
			{
				//�ӿ�����ȡ����
				INT_PTR nStoreIndex = 0;
				if( DistillJetIndex(lJetScore,nStoreIndex) )
				{
					wIndexPending = m_arJetIndex[nStoreIndex];
					lJetScore += m_lJetonIndex[wIndexPending];

					//ɾ�����г�������
					m_arJetIndex.RemoveAt(nStoreIndex);
				}
				else return FALSE;

				//����ƫ��
				ptExcusion = m_arJetExcusions[nStoreIndex];

				//ɾ������ƫ��
				m_arJetExcusions.RemoveAt(nStoreIndex);

				//������ʼ��
				ptFrom.x = m_BenchmarkPos.x+ptExcusion.x;
				ptFrom.y = m_BenchmarkPos.y+ptExcusion.y;

				//����Ŀ���
				ptTo = jp.ptJetton;

				//���ɲ���
				if( abs(ptTo.x-ptFrom.x) > abs(ptTo.y-ptFrom.y) )
					nStepCount = abs(ptTo.x-ptFrom.x)/MOVE_STEP_LEN;
				else nStepCount = abs(ptTo.y-ptFrom.y)/MOVE_STEP_LEN;

				//����X����
				nXStep = (ptTo.x-ptFrom.x)/nStepCount;

				//����Y����
				nYStep = (ptTo.y-ptFrom.y)/nStepCount;
			}

			//���춯��
			m_arIndexPending.Add(wIndexPending);
			m_arExcusionsPend.Add(ptExcusion);
			m_arStepCount.Add(nStepCount);
			m_arXStep.Add(nXStep);
			m_arYStep.Add(nYStep);
		}

		//�ۼӴ��������
		m_lScorePending += jp.lScore;
		
		//ɾ����������
		m_arJetPending.RemoveAt(0);
		if( m_arJetPending.GetCount() == 0 ) break;
	}

	return m_arIndexPending.GetCount()>0?TRUE:FALSE;
}

//��ȡ��������
BOOL CJettonControl::DistillJetIndex( LONGLONG lScore,INT_PTR &nJetIndex )
{
	//Ч������
	if( lScore < 0L ) lScore = -lScore;

	//�����
	if( m_arJetIndex.GetCount() == 0 ) return FALSE;

	for( INT_PTR n = 0; n < m_arJetIndex.GetCount(); n++ )
	{
		WORD wIndex = m_arJetIndex[n];
		if( lScore >= m_lJetonIndex[wIndex] )
		{
			nJetIndex = n;
			return TRUE;
		}
	}

	INT_PTR nLastIndex = m_arJetIndex.GetCount()-1;

	//��ȡ���һ���������
	WORD wJettonIndex = m_arJetIndex[nLastIndex];
	LONGLONG lLastScore = m_lJetonIndex[wJettonIndex];
	CPoint ptLastExcusion = m_arJetExcusions[nLastIndex];

	//�ӿ���ɾ��
	m_arJetIndex.RemoveAt(nLastIndex);
	m_arJetExcusions.RemoveAt(nLastIndex);

	//�ֽ����,���������
	do
	{
		int i = wJettonIndex-1;
		if( i < 0 ) return FALSE;
		for( ; i >= 0; i-- )
		{
			while( lLastScore >= m_lJetonIndex[i] )
			{
				//�������
				m_arJetIndex.Add(i);
				m_arJetExcusions.Add(ptLastExcusion);

				//��ȥ�����
				lLastScore -= m_lJetonIndex[i];

				//��������ƫ��
				ptLastExcusion.x = (rand()%X_RADIUS)*((rand()%100)>49?1:-1);
				ptLastExcusion.y = (rand()%Y_RADIUS)*((rand()%100)>49?1:-1);
			}
			if( lLastScore == 0 ) break;
		}

		//����Ƿ����ȡ
		wJettonIndex = m_arJetIndex[nLastIndex];
		if( lScore >= m_lJetonIndex[wJettonIndex] ) break;

		//������ȡ,���ٷֽ�
		nLastIndex = m_arJetIndex.GetCount()-1;
		wJettonIndex = m_arJetIndex[nLastIndex];
		lLastScore = m_lJetonIndex[wJettonIndex];
		ptLastExcusion = m_arJetExcusions[nLastIndex];
		m_arJetIndex.RemoveAt(nLastIndex);
		m_arJetExcusions.RemoveAt(nLastIndex);
	}while(true);

	nJetIndex = nLastIndex;
	return TRUE;
}

//ѹ������,��Сֵ������ϳɴ�ֵ����
VOID CJettonControl::CompactJetIndex()
{
	//�����
	if( m_arJetIndex.GetCount() == 0 ) return;

	//ȷ�����ѹ������ֵ
	LONGLONG lMostScore = m_lJetonIndex[MAX_JETTON_INDEX-1];
	LONGLONG lScore = 0L;

	//�������
	CWHArray<WORD> arJetIndex;
	CWHArray<CPoint> arJetExcusions;
	arJetIndex.Copy( m_arJetIndex );
	arJetExcusions.Copy( m_arJetExcusions );

	INT_PTR nCompactCount,nPreCompactCount;
	do
	{
		//��ȡ���
		nPreCompactCount = arJetIndex.GetCount();
		nCompactCount = 0;
		for( INT_PTR i = 0; i < arJetIndex.GetCount(); i++ )
		{
			//��ȡ����ֵ
			WORD wJetIndex = arJetIndex[i];
			lScore += m_lJetonIndex[wJetIndex];
		}
		//ɾ������
		arJetIndex.RemoveAll();
		//ɾ��ƫ��
		arJetExcusions.RemoveAll();

		if( lScore == 0L ) break;

		INT_PTR n = arJetIndex.GetCount();

		//���
		for( int i = MAX_JETTON_INDEX-1; i >= 0; i-- )
		{
			while( lScore >= m_lJetonIndex[i] )
			{
				//����������
				arJetIndex.Add(i);

				//��ȥ�����
				lScore -= m_lJetonIndex[i];

				//��������ƫ��
				CPoint ptExcusion;
				ptExcusion.x = (rand()%X_RADIUS)*((rand()%100)>49?1:-1);
				ptExcusion.y = (rand()%Y_RADIUS)*((rand()%100)>49?1:-1);

				//���ƫ��
				arJetExcusions.Add( ptExcusion );

				//
				nCompactCount++;
			}
			if( lScore == 0 ) break;
		}
	}while( nCompactCount < nPreCompactCount );
	
	//�������
	m_arJetIndex.Copy( arJetIndex );
	m_arJetExcusions.Copy( arJetExcusions );
}

//�ӳ���
VOID CJettonControl::AddScore( LONGLONG lScore, CPoint ptFrom )
{
	if( lScore > 0L )
	{
		tagJetPending jp;
		jp.lScore = lScore;
		jp.ptJetton = ptFrom;
		m_arJetPending.Add(jp);
	}
}

//�Ƴ�����
VOID CJettonControl::RemoveScore( LONGLONG lScore,CPoint ptTo )
{
	//�����������0
	if( lScore > 0L )
	{
		tagJetPending jp;

		//ȡ��
		jp.lScore = -lScore;
		jp.ptJetton = ptTo;
		m_arJetPending.Add(jp);
	}
}

//�Ƴ����г���
VOID CJettonControl::RemoveAllScore( CPoint ptTo )
{
	RemoveScore(m_lScore,ptTo);
}

//��ʼ���붯��
BOOL CJettonControl::BeginMoveJettons()
{
	//����������벻Ϊ0,���һ������ֲ�Ϊ0
	INT_PTR nJetPendCount = m_arJetPending.GetCount();
	if( nJetPendCount > 0 && m_arJetPending[nJetPendCount-1].lScore != 0 )
	{
		//����ճ���
		tagJetPending jp;
		jp.lScore = 0L;
		jp.ptJetton.SetPoint(0,0);
		m_arJetPending.Add(jp);
	}	

	//���춯���ṹ
	if( ConstructCartoon() )
	{
		ComputeMoveRect();
		m_rcDraw = m_rcMove;
		return TRUE;
	}

	return FALSE;
}

//���붯��
BOOL CJettonControl::PlayMoveJettons()
{
	BOOL bMoving = FALSE;
	if( m_arIndexPending.GetCount() > 0 )
	{
		//������������
		for( INT_PTR i = 0; i < m_arIndexPending.GetCount(); i++ )
		{
			//���δ����ƶ�
			INT &nStepCount = m_arStepCount[i];
			if( nStepCount > 0 )
			{
				//����ƫ��
				CPoint &pt = m_arExcusionsPend[i];
				INT nXStep = m_arXStep[i];
				INT nYStep = m_arYStep[i];
				pt.x += nXStep;
				pt.y += nYStep;
				//���ٲ���
				nStepCount--;
				//�����ƶ�״̬
				bMoving = TRUE;
			}
		}

		//������ƶ�
		if( bMoving )
			//�����ƶ�����
			ComputeMoveRect();

		//�������������ƶ�
		if( !bMoving )
		{
			//��������붯��
			if( m_lScorePending > 0L )
			{
				//��ӳ����������
				m_arJetIndex.Append(m_arIndexPending);

				//���ƫ�����
				m_arJetExcusions.Append(m_arExcusionsPend);
			}

			//���¿��������
			m_lScore += m_lScorePending;

			//ɾ��������ر���
			m_arExcusionsPend.RemoveAll();
			m_arIndexPending.RemoveAll();
			m_arStepCount.RemoveAll();
			m_arXStep.RemoveAll();
			m_arYStep.RemoveAll();

			//�ж��Ƿ������󶯻�
			if( !BeginMoveJettons() )
			{
				//�������������MAX_DRAW,��ѹ������
				if( m_arJetIndex.GetCount() >= MAX_DRAW )
				{
					CompactJetIndex();
				}
				return FALSE;
			}
			else return TRUE;
		}
	}
	return bMoving;
}

//ֹͣ���붯��
BOOL CJettonControl::FinishMoveJettons()
{	
	BOOL bMoveJetton = FALSE;

	//������ڶ���,���д�������
	while( m_arIndexPending.GetCount() > 0 || ConstructCartoon() )
	{
		//������������
		for( INT_PTR i = 0; i < m_arIndexPending.GetCount(); i++ )
		{
			//�����δ����ƶ�
			INT nStepCount = m_arStepCount[i];
			if( nStepCount > 0 )
			{
				//����ƫ��
				CPoint &pt = m_arExcusionsPend[i];
				pt.x += m_arXStep[i]*nStepCount;
				pt.y += m_arYStep[i]*nStepCount;
			}
		}

		//������������
		if( m_lScorePending > 0L )
		{
			//������
			m_arJetIndex.Append(m_arIndexPending);
			m_arJetExcusions.Append(m_arExcusionsPend);
		}

		//���¿��������
		m_lScore += m_lScorePending;

		//ɾ����������
		m_arExcusionsPend.RemoveAll();
		m_arIndexPending.RemoveAll();
		m_arStepCount.RemoveAll();
		m_arXStep.RemoveAll();
		m_arYStep.RemoveAll();

		//
		bMoveJetton = TRUE;
	}

	//�������������MAX_DRAW,��ѹ������
	if( m_arJetIndex.GetCount() >= MAX_DRAW )
	{
		CompactJetIndex();
	}

	return bMoveJetton;
}

//��׼λ��
VOID CJettonControl::SetBenchmarkPos(INT nXPos, INT nYPos)
{

	//�����滭����
	if( !m_rcDraw.IsRectEmpty() )
		m_rcDraw.InflateRect(abs(m_BenchmarkPos.x-nXPos),abs(m_BenchmarkPos.y-nYPos));
	if( !m_rcMove.IsRectEmpty() )
		m_rcMove.OffsetRect(nXPos-m_BenchmarkPos.x,nYPos-m_BenchmarkPos.y);

	//���ñ���
	m_BenchmarkPos.SetPoint(nXPos,nYPos);

	return;
}

//�滭�ؼ�
VOID CJettonControl::DrawJettonControl(CDC * pDC)
{
	//�滭�ж� 
	if (m_lScore<=0L&&m_arIndexPending.GetCount()==0) return;

	INT nXPos,nYPos;
	INT_PTR i;

	//�滭����ӳ���,ֻ�����MAX_DRAW
	if( m_arJetIndex.GetCount() > MAX_DRAW )
		i = m_arJetIndex.GetCount()-MAX_DRAW;
	else i = 0;

	for( ; i < m_arJetIndex.GetCount(); i++ )
	{
		WORD wJetIndex = m_arJetIndex[i];
		nXPos = m_BenchmarkPos.x + m_arJetExcusions[i].x - m_nJettonWidth/2;
		nYPos = m_BenchmarkPos.y + m_arJetExcusions[i].y - m_nJettonHeight/2;

		//�滭����
		m_PngJetton.DrawImage(pDC,nXPos,nYPos,m_nJettonWidth,m_nJettonHeight,m_nJettonWidth*wJetIndex,0);
	}

	//�滭�ƶ��еĳ���
	if( m_arIndexPending.GetCount() > 0 )
	{
		//�滭����,ֻ�����MAX_DRAW
		if( m_arIndexPending.GetCount() > MAX_DRAW )
			i = m_arIndexPending.GetCount() - MAX_DRAW;
		else i = 0;

		for( ; i < m_arIndexPending.GetCount(); i++ )
		{
			WORD wJetIndex = m_arIndexPending[i];
			nXPos = m_BenchmarkPos.x + m_arExcusionsPend[i].x - m_nJettonWidth/2;
			nYPos = m_BenchmarkPos.y + m_arExcusionsPend[i].y - m_nJettonHeight/2;

			//�滭����
			m_PngJetton.DrawImage(pDC,nXPos,nYPos,m_nJettonWidth,m_nJettonHeight,m_nJettonWidth*wJetIndex,0);
		}

		//ȥ���滭����
		m_rcDraw = m_rcMove;
	}

	return;
}

//�Ƿ��ڶ���
BOOL CJettonControl::IsPlayMoving()
{
	return m_arIndexPending.GetCount()>0;
}

//��ȡ���»滭����
VOID CJettonControl::GetDrawRect( CRect &rc )
{
	rc = m_rcMove | m_rcDraw;
	rc.InflateRect(m_nJettonWidth/2,m_nJettonHeight/2);
	return;
}

//////////////////////////////////////////////////////////////////////////
