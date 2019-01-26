#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//排序类型
#define	ST_VALUE					1									//数值排序
#define	ST_LOGIC					2									//逻辑排序

//扑克数目
#define CARD_COUNT					52									//扑克数目

#define MAX_COUNT					3									//最大数目

// 牌型
#define CARD_BAOZI					3									//豹子
#define CARD_TONGHUASHUN			4									//同花顺
#define CARD_JINHUA					5									//金花
#define CARD_SHUNZI					6									//顺子
#define CARD_DUIZI					7									//对子
#define CARD_SINGLE					8									//单张
#define CARD_SPECIAL				9									//特殊牌型(235)
//////////////////////////////////////////////////////////////////////////

//游戏逻辑
void RandCount(BYTE cbCardBuffer[], BYTE cbBufferCount);
class CGameLogic
{
	//变量定义
private:
	static const BYTE				m_cbCardListData[CARD_COUNT];		//扑克定义

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//控制函数
public:
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);

	//逻辑函数
public:
	//获取牌型
	DWORD GetCardType(const BYTE cbCardData[], BYTE cbCardCount);

	BYTE GetNewCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//大小比较
	BYTE CompareCard(const BYTE cbFirstCardData[], BYTE cbFirstCardCount,const BYTE cbNextCardData[], BYTE cbNextCardCount, BYTE pWinArea[]);

	int CompareCardCycle(const BYTE cbFirstCardData[], const BYTE cbNextCardData[], BYTE maxIndex);

	//逻辑大小
	BYTE GetCardLogicValue(BYTE cbCardData);

	void GetCardPoint(BYTE bcCardData ,CPoint &Point,int CardX,int CardY);
	//获取点数
	DWORD CGameLogic::GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount);
};

//////////////////////////////////////////////////////////////////////////

#endif
