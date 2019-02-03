#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//机器人类
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	BYTE							m_cbGameMode;						//游戏模式

	//控件变量
protected:
	IAndroidUserItem *				m_pIAndroidUserItem;				//用户接口

	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID  Release() {delete this; }
	//接口查询
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//控制接口
public:
	//初始接口
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool  RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool  OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool  OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏消息
	virtual bool  OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//场景消息
	virtual bool  OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void  OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual void  OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual void  OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual void  OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	
	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubTwoStart(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubThreeStart(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubThreeKaiJian(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubThreeEnd(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubGameConclude(VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif
