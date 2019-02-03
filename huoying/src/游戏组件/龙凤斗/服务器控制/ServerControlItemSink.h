#pragma once
#include "../游戏服务器/ServerControl.h"
#include "../游戏服务器/GameLogic.h"
//索引定义
#define INDEX_PLAYER				0									//龙
#define INDEX_BANKER				1									//凤

class CServerControlItemSink : public IServerControl
{
	//控制变量
protected:
	BYTE							m_cbWinSideControl;					//控制输赢
	BYTE							m_cbExcuteTimes;					//执行次数
	int								m_nSendCardCount;					//发送次数
	BYTE							m_bWinArea[AREA_MAX];				//控制输赢区域
	int								m_nControlCount;					//控制区域数量


	//扑克信息
protected:
	BYTE							m_cbCardCount[2];						//扑克数目
	BYTE							m_cbTableCardArray[2][1];					//桌面扑克
	CGameLogic						m_GameLogic;							//游戏逻辑

public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

public:
	//服务器控制
	virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize,
		IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	//需要控制
	virtual bool __cdecl NeedControl();
	//开始控制
	virtual void __cdecl GetControlArea(BYTE Area[], BYTE & cbControlTimes);

	//返回控制区域
	virtual bool __cdecl ControlResult(BYTE	cbTableCardArray[], BYTE cbCardCount[]);

	//控制信息
	VOID ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	// 记录函数
	VOID WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString );
	//发送扑克
	bool  DispatchCard();
	//推断赢家
	void DeduceWinner(BYTE* pWinArea);
};
