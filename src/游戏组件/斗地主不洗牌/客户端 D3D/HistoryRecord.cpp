#include "StdAfx.h"
#include "HistoryRecord.h"

#define new DEBUG_NEW

//////////////////////////////////////////////////////////////////////////////////
//�궨��

//��������
#define INDEX_CURRENT				0									//��ǰ����
#define INDEX_HISTORY				1									//��ʷ����

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CHistoryCard::CHistoryCard()
{
	//���ñ���
	m_bHistoryCard=false;
	ZeroMemory(m_cbCardData,sizeof(m_cbCardData));
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbEventFlag,sizeof(m_cbEventFlag));

	return;
}

//��������
CHistoryCard::~CHistoryCard()
{
}

//��������
VOID CHistoryCard::ResetData()
{
	//���ñ���
	m_bHistoryCard=false;
	ZeroMemory(m_cbCardData,sizeof(m_cbCardData));
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbEventFlag,sizeof(m_cbEventFlag));

	return;
}

//��ȡ����
BYTE CHistoryCard::GetCurrentEvent(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return AF_NULL;

	return m_cbEventFlag[wChairID][INDEX_CURRENT];
}

//��ȡ����
BYTE CHistoryCard::GetHistoryEvent(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return AF_NULL;

	return m_cbEventFlag[wChairID][INDEX_HISTORY];
}

//��ȡ�˿�
BYTE CHistoryCard::GetCurrentCard(WORD wChairID, BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//Ч�����
	ASSERT((wChairID<GAME_PLAYER)&&(cbBufferCount>=MAX_COUNT));
	if ((wChairID>=GAME_PLAYER)||(cbBufferCount<MAX_COUNT)) return 0;

	//�����˿�
	BYTE cbCardCount=m_cbCardCount[wChairID][INDEX_CURRENT];
	CopyMemory(cbCardBuffer,m_cbCardData[wChairID][INDEX_CURRENT],cbCardCount);

	return cbCardCount;
}

//��ȡ�˿�
BYTE CHistoryCard::GetHistoryCard(WORD wChairID, BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//Ч�����
	ASSERT((wChairID<GAME_PLAYER)&&(cbBufferCount>=MAX_COUNT));
	if ((wChairID>=GAME_PLAYER)||(cbBufferCount<MAX_COUNT)) return 0;

	//�����˿�
	BYTE cbCardCount=m_cbCardCount[wChairID][INDEX_HISTORY];
	CopyMemory(cbCardBuffer,m_cbCardData[wChairID][INDEX_HISTORY],cbCardCount);

	return cbCardCount;
}

//�����¼�
VOID CHistoryCard::OnEventSaveData(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//��������
	if (m_cbEventFlag[wChairID][INDEX_CURRENT]!=AF_NULL)
	{
		//������Ϣ
		if (m_bHistoryCard==false)
		{
			CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
			if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(WM_ENABLE_HISTORY,0,0);
		}

		//���ñ�־
		m_bHistoryCard=true;

		//��������
		m_cbEventFlag[wChairID][INDEX_HISTORY]=m_cbEventFlag[wChairID][INDEX_CURRENT];
		m_cbCardCount[wChairID][INDEX_HISTORY]=m_cbCardCount[wChairID][INDEX_CURRENT];
		CopyMemory(m_cbCardData[wChairID][INDEX_HISTORY],m_cbCardData[wChairID][INDEX_CURRENT],sizeof(BYTE)*MAX_COUNT);

		//��������
		m_cbCardCount[wChairID][INDEX_CURRENT]=0;
		m_cbEventFlag[wChairID][INDEX_CURRENT]=AF_NULL;
		ZeroMemory(m_cbCardData[wChairID][INDEX_CURRENT],sizeof(BYTE)*MAX_COUNT);
	}

	return;
}

//�û�����
VOID CHistoryCard::OnEventUserPass(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//Ч���¼
	//ASSERT(m_cbCardCount[wChairID][INDEX_CURRENT]==0);
	//ASSERT(m_cbEventFlag[wChairID][INDEX_CURRENT]==AF_NULL);

	if ( m_cbCardCount[wChairID][INDEX_CURRENT] != 0 )
	{
		m_cbCardCount[wChairID][INDEX_HISTORY] = m_cbCardCount[wChairID][INDEX_CURRENT];
		CopyMemory(m_cbCardData[wChairID][INDEX_HISTORY],m_cbCardData[wChairID][INDEX_CURRENT],sizeof(BYTE)*m_cbCardCount[wChairID][INDEX_CURRENT]);
	}

	if ( m_cbEventFlag[wChairID][INDEX_CURRENT] != AF_NULL )
	{
		m_cbEventFlag[wChairID][INDEX_HISTORY] = m_cbEventFlag[wChairID][INDEX_CURRENT];
	}

	//������¼
	m_cbEventFlag[wChairID][INDEX_CURRENT] = AF_PASS;

	return;
}

//�û�����
VOID CHistoryCard::OnEventUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//Ч�����
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;
	
	//Ч���¼
	//ASSERT(m_cbCardCount[wChairID][INDEX_CURRENT]==0);
	//ASSERT(m_cbEventFlag[wChairID][INDEX_CURRENT]==AF_NULL);


	if ( m_cbCardCount[wChairID][INDEX_CURRENT] != 0 )
	{
		m_cbCardCount[wChairID][INDEX_HISTORY] = m_cbCardCount[wChairID][INDEX_CURRENT];
		CopyMemory(m_cbCardData[wChairID][INDEX_HISTORY],m_cbCardData[wChairID][INDEX_CURRENT],sizeof(BYTE)*m_cbCardCount[wChairID][INDEX_CURRENT]);
	}

	if ( m_cbEventFlag[wChairID][INDEX_CURRENT] != AF_NULL )
	{
		m_cbEventFlag[wChairID][INDEX_HISTORY] = m_cbEventFlag[wChairID][INDEX_CURRENT];
	}

	//������¼
	m_cbEventFlag[wChairID][INDEX_CURRENT]=AF_OUT_CARD;
	m_cbCardCount[wChairID][INDEX_CURRENT]=cbCardCount;
	CopyMemory(m_cbCardData[wChairID][INDEX_CURRENT],cbCardData,sizeof(BYTE)*cbCardCount);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CHistoryScore::CHistoryScore()
{
	//���ñ���
	ZeroMemory(m_HistoryScore,sizeof(m_HistoryScore));

	return;
}

//��������
CHistoryScore::~CHistoryScore()
{
}

//��������
VOID CHistoryScore::ResetData()
{
	//���ñ���
	ZeroMemory(m_HistoryScore,sizeof(m_HistoryScore));

	return;
}

//��ȡ����
tagHistoryScore * CHistoryScore::GetHistoryScore(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return NULL;

	//��ȡ����
	return &m_HistoryScore[wChairID];
}

//�û�����
VOID CHistoryScore::OnEventUserEnter(WORD wChairID)
{
	//Ч������
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;
	
	//���ñ���
	m_HistoryScore[wChairID].lTurnScore=0;
	m_HistoryScore[wChairID].lCollectScore=0;

	return;
}

//�û��뿪
VOID CHistoryScore::OnEventUserLeave(WORD wChairID)
{
	//Ч������
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;
	
	//���ñ���
	m_HistoryScore[wChairID].lTurnScore=0;
	m_HistoryScore[wChairID].lCollectScore=0;

	return;
}

//�û�����
VOID CHistoryScore::OnEventUserScore(WORD wChairID, SCORE lGameScore)
{
	//Ч�����
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//���ñ���
	m_HistoryScore[wChairID].lTurnScore=lGameScore;
	m_HistoryScore[wChairID].lCollectScore+=lGameScore;

	return;
}

//////////////////////////////////////////////////////////////////////////////////
