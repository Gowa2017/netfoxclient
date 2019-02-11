#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
//////////////////////////////////////////////////////////////////////////

//用于财神的转换，如果有牌可以代替财神本身牌使用，则设为该牌索引，否则设为MAX_INDEX. 注:如果替换牌是序数牌,将出错.
#define	INDEX_REPLACE_CARD					MAX_INDEX

//////////////////////////////////////////////////////////////////////////
//逻辑掩码

#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////
//动作定义

//动作类型
#define WIK_GANERAL					0x00								//普通操作
#define WIK_MING_GANG				0x01								//明杠（碰后再杠）
#define WIK_FANG_GANG				0x02								//放杠
#define WIK_AN_GANG					0x03								//暗杠

//动作标志
#define WIK_NULL					0x00								//没有类型
#define WIK_LEFT					0x01								//左吃类型
#define WIK_CENTER					0x02								//中吃类型
#define WIK_RIGHT					0x04								//右吃类型
#define WIK_PENG					0x08								//碰牌类型
#define WIK_GANG					0x10								//杠牌类型
#define WIK_LISTEN					0x20								//听牌类型
#define WIK_CHI_HU					0x40								//吃胡类型
#define WIK_FANG_PAO			0x80								//放炮

//////////////////////////////////////////////////////////////////////////
//胡牌定义

#define CHR_PING_HU					0x00000001							//平胡
#define CHR_PENG_PENG				0x00000002							//碰碰胡
#define CHR_DAN_DIAN_QI_DUI	0x00000004							//单点七对
#define CHR_MA_QI_DUI				0x00000008							//麻七对
#define CHR_MA_QI_WANG			0x00000010							//麻七王
#define CHR_MA_QI_WZW				0x00000020							//麻七王中王
#define CHR_SHI_SAN_LAN			0x00000040							//十三烂
#define CHR_QX_SHI_SAN_LAN		0x00000080							//七星十三烂
#define CHR_TIAN_HU					0x00000100							//天胡
#define CHR_DI_HU						0x00000200							//地胡
#define CHR_QI_SHOU_LISTEN		0x00000400							//起首听



#define CHR_GANG_SHANG_HUA          0x00800000                          //杠上花
#define CHR_GANG_SHANG_PAO          0x01000000                          //杠上炮
#define CHR_QIANG_GANG_HU           0x02000000                          //抢杠胡
#define CHR_CHI_HU					0x04000000							//放炮
#define CHR_ZI_MO					0x08000000							//自摸

//////////////////////////////////////////////////////////////////////////////////

//类型子项
struct tagKindItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbValidIndex[3];					//实际扑克索引
	BYTE							cbMagicCount;						//财神牌数
};

//杠牌结果
struct tagGangCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[MAX_WEAVE];				//扑克数据
};


//分析子项
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//牌眼扑克
	bool                            bMagicEye;                          //牌眼是否是王霸
	BYTE							cbWeaveKind[MAX_WEAVE];				//组合类型
	BYTE							cbCenterCard[MAX_WEAVE];			//中心扑克
	BYTE                            cbCardData[MAX_WEAVE][4];           //实际扑克
};


//////////////////////////////////////////////////////////////////////////


#define MASK_CHI_HU_RIGHT			0x0fffffff

/*
//	权位类。
//  注意，在操作仅位时最好只操作单个权位.例如
//  CChiHuRight chr;
//  chr |= (chr_zi_mo|chr_peng_peng)，这样结果是无定义的。
//  只能单个操作:
//  chr |= chr_zi_mo;
//  chr |= chr_peng_peng;
*/
class CChiHuRight
{	
	//静态变量
private:
	static bool						m_bInit;
	static DWORD					m_dwRightMask[MAX_RIGHT_COUNT];

	//权位变量
private:
	DWORD							m_dwRight[MAX_RIGHT_COUNT];

public:
	//构造函数
	CChiHuRight();

	//运算符重载
public:
	//赋值符
	CChiHuRight & operator = (DWORD dwRight);

	//与等于
	CChiHuRight & operator &= (DWORD dwRight);
	//或等于
	CChiHuRight & operator |= (DWORD dwRight);

	//与
	CChiHuRight operator & (DWORD dwRight);
	CChiHuRight operator & (DWORD dwRight) const;

	//或
	CChiHuRight operator | (DWORD dwRight);
	CChiHuRight operator | (DWORD dwRight) const;

	//相等
	bool operator == (DWORD dwRight) const;
	bool operator == (const CChiHuRight chr) const;

	//不相等
	bool operator != (DWORD dwRight) const;
	bool operator != (const CChiHuRight chr) const;

	//功能函数
public:
	//是否权位为空
	bool IsEmpty();

	//设置权位为空
	void SetEmpty();

	//获取权位数值
	BYTE GetRightData(DWORD dwRight[], BYTE cbMaxCount);

	//设置权位数值
	bool SetRightData(const DWORD dwRight[], BYTE cbRightCount);

private:
	//检查权位是否正确
	bool IsValidRight(DWORD dwRight);
};

//////////////////////////////////////////////////////////////////////////////////

//数组说明
typedef CWHArray<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;

//游戏逻辑类
class CGameLogic
{
	//变量定义
public:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//扑克数据
	BYTE							m_cbMagicIndex;						//钻牌索引
	BYTE							m_cbQiXiaoDui;						//是否可胡七小对
	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//控制函数
public:
	//混乱扑克
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	int GetCards(BYTE outCards[], BYTE outLen, BYTE repoCardsPrime[], BYTE repoLen, BYTE cCards[] = NULL, BYTE cLen = 0);
	//混乱扑克
	VOID RandCardList(BYTE cbCardData[], BYTE cbCardBuffer[], BYTE cbBufferCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//排序,根据牌值排序
	bool SortCardList(BYTE cbCardData[MAX_COUNT], BYTE cbCardCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//设置财神
	void SetMagicIndex(BYTE cbMagicIndex) { m_cbMagicIndex = cbMagicIndex; }
	//财神判断
	bool IsMagicCard(BYTE cbCardData);
	//花牌判断
	bool IsHuaCard(BYTE cbCardData);
	//花牌判断
	BYTE IsHuaCard(BYTE cbCardIndex[MAX_INDEX]);

	//逻辑函数
public:

	//等级函数
public:
	//动作等级
	BYTE GetUserActionRank(BYTE cbUserAction);
	//胡牌等级
	WORD GetChiHuActionRank(const CChiHuRight & ChiHuRight);
	//胡牌倍数
	WORD GetChiHuTime(const CChiHuRight & ChiHuRight);
	//自动出牌
	BYTE AutomatismOutCard(const BYTE cbCardIndex[MAX_INDEX], const BYTE cbEnjoinOutCard[MAX_COUNT], BYTE cbEnjoinOutCardCount);

	//动作判断
public:
	//吃牌判断
	BYTE EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//碰牌判断
	BYTE EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//杠牌判断
	BYTE EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

	//动作判断
public:
	//杠牌分析
	BYTE AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult);
	//杠牌分析
	BYTE AnalyseGangCardEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbProvideCard, tagGangCardResult & GangCardResult);
	//吃胡分析
	BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight,bool b4HZHu=false);
	//听牌分析
	BYTE AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//听牌分析，获取打那些牌能听，以及能胡哪些牌
	BYTE AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbOutCard[][28]);
	BYTE AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount,BYTE cbOutCardData[],BYTE cbHuCardCount[],BYTE cbHuCardData[][28]);
	//获取胡牌数据，听牌后调用
	BYTE GetHuCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbHuCardData[]);

	//转换函数
public:
	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//扑克转换
	BYTE SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//扑克转换
	BYTE SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);
	

	//辅助函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//扑克数目
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);	
	//组合扑克
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);

	//内部函数
private:
	bool AddKindItem(tagKindItem &TempKindItem, tagKindItem KindItem[], BYTE &cbKindItemCount, bool &bMagicThree);
	//分析扑克
	bool AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveICount, CAnalyseItemArray & AnalyseItemArray);

	//胡法分析
protected:

	//碰碰和
	bool IsPengPeng(const tagAnalyseItem *pAnalyseItem);
	//麻七系列
	bool IsMaQi(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount,CChiHuRight &ChiHuRight);
	//十三烂系列
	bool IsShiSanLan(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount,CChiHuRight &ChiHuRight);

	bool IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount);

	//平胡
	bool IsPingHu(const tagAnalyseItem *pAnalyseItem);
	//鸡胡
	bool IsJiHu(const tagAnalyseItem *pAnalyseItem);
	//清一色
	bool IsQingYiSe(const tagAnalyseItem * pAnalyseItem, bool &bQuanFan);	
	//混一色
	bool IsHunYiSe(const tagAnalyseItem * pAnalyseItem);	
};

//////////////////////////////////////////////////////////////////////////////////


#endif