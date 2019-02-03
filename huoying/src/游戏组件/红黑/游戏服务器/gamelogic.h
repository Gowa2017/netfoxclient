#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//��������
#define	ST_VALUE					1									//��ֵ����
#define	ST_LOGIC					2									//�߼�����

//�˿���Ŀ
#define CARD_COUNT					52									//�˿���Ŀ

#define MAX_COUNT					3									//�����Ŀ

// ����
#define CARD_BAOZI					3									//����
#define CARD_TONGHUASHUN			4									//ͬ��˳
#define CARD_JINHUA					5									//��
#define CARD_SHUNZI					6									//˳��
#define CARD_DUIZI					7									//����
#define CARD_SINGLE					8									//����
#define CARD_SPECIAL				9									//��������(235)
//////////////////////////////////////////////////////////////////////////

//��Ϸ�߼�
void RandCount(BYTE cbCardBuffer[], BYTE cbBufferCount);
class CGameLogic
{
	//��������
private:
	static const BYTE				m_cbCardListData[CARD_COUNT];		//�˿˶���

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ͺ���
public:
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//���ƺ���
public:
	//�����˿�
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//�����˿�
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);

	//�߼�����
public:
	//��ȡ����
	DWORD GetCardType(const BYTE cbCardData[], BYTE cbCardCount);

	BYTE GetNewCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//��С�Ƚ�
	BYTE CompareCard(const BYTE cbFirstCardData[], BYTE cbFirstCardCount,const BYTE cbNextCardData[], BYTE cbNextCardCount, BYTE pWinArea[]);

	int CompareCardCycle(const BYTE cbFirstCardData[], const BYTE cbNextCardData[], BYTE maxIndex);

	//�߼���С
	BYTE GetCardLogicValue(BYTE cbCardData);

	void GetCardPoint(BYTE bcCardData ,CPoint &Point,int CardX,int CardY);
	//��ȡ����
	DWORD CGameLogic::GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount);
};

//////////////////////////////////////////////////////////////////////////

#endif
