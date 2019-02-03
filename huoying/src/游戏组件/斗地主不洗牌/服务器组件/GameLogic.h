#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define ST_ORDER					1									//��С����
#define ST_COUNT					2									//��Ŀ����
#define ST_CUSTOM					3									//�Զ�����

//////////////////////////////////////////////////////////////////////////////////

//�����ṹ
struct tagAnalyseResult
{
	BYTE 							cbBlockCount[4];					//�˿���Ŀ
	BYTE							cbCardData[4][MAX_COUNT];			//�˿�����
};

//���ƽ��
struct tagOutCardResult
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbResultCard[MAX_COUNT];			//����˿�
};

//�ֲ���Ϣ
struct tagDistributing
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbDistributing[15][6];				//�ֲ���Ϣ
};

//�������
struct tagSearchCardResult
{
	BYTE							cbSearchCount;						//�����Ŀ
	BYTE							cbCardCount[MAX_COUNT];				//�˿���Ŀ
	BYTE							cbResultCard[MAX_COUNT][MAX_COUNT];	//����˿�
};

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//�˿�����

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//���ƺ���
public:
	//�����˿�
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//�����˿�
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//�����˿�
	VOID SortOutCardList(BYTE cbCardData[], BYTE cbCardCount);
	//ɾ���˿�
	bool RemoveCardList(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//ɾ���˿�
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//�߼�����
public:
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	//�Ա��˿�
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	//��������
	BYTE SearchOutCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, 
		tagSearchCardResult *pSearchCardResult );
	//ͬ������
	BYTE SearchSameCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount,
		tagSearchCardResult *pSearchCardResult );
	//��������
	BYTE SearchLineCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
		tagSearchCardResult *pSearchCardResult );
	//������������(����һ���Ĵ�һ��)
	BYTE SearchTakeCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount, 
		tagSearchCardResult *pSearchCardResult );
	//�����ɻ�
	BYTE SearchThreeTwoLine( const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult );

	//�ڲ�����
public:
	//�����˿�
	BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
	//�����˿�
	VOID AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//�����ֲ�
	VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing);
};

//////////////////////////////////////////////////////////////////////////////////

#endif