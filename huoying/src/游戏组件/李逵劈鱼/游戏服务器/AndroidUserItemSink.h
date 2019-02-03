#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//机器人类
class CAndroidUserItemSink : public IAndroidUserItemSink
{
protected:
   IAndroidUserItem *				m_pIAndroidUserItem;				//用户接口
public:
	int exchange_ratio_userscore_;
	int exchange_ratio_fishscore_;
private:
  int exchange_count_;

  float min_bullet_multiple_;
  float max_bullet_multiple_;

  int RobotScoreMin;
  int RobotScoreMax;
  int RobotBankGet;
  int RobotBankGetBanker;
  int RobotBankStoMul;


  BulletKind current_bullet_kind_;
  float current_bullet_mulriple_;
  bool allow_fire_;
  bool free_fire;
  bool bAndroidInScene;
  bool scene_android_fire_mode;
  int  AndroidLockFish;

  SCORE exchange_fish_score_;
  SCORE fish_score_;

  float last_fire_angle_;
  int exchange_times_;
  float exchange_fire_multiple;

  int bullet_id_;
  int fire_mode;

	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { }
	//是否有效
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CAndroidUserItemSink))?true:false; }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//控制接口
public:
	//初始接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) {}
	//用户离开
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}
	//用户积分
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) {}
	//用户状态
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) {}
	//用户段位
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) {}

	// 消息处理
protected:
  bool OnSubGameConfig(void* data, WORD data_size);
  DWORD GetTodayTickCount(SYSTEMTIME & SystemTime);
 

  bool OnSubExchangeFishScore(void* data, WORD data_size);
  bool OnSubUserFire(void* data, WORD data_size);
  bool OnSubCatchFish(void* data, WORD data_size);
  bool OnSubBulletIonTimeout(void* data, WORD data_size);
  bool OnSubCatSweepFishResult(void* data, WORD data_size);
  bool OnSubSwitchScene(void* data, WORD data_size);
  bool OnNofire();
  bool Oncanfire();
  bool OnSubUpdate();
private:
  void ExchangeFishScore();
  void Fire();
 // void Fire2();
  float GetAngle(WORD chair_id);
  float GetAngleForFixFire(WORD chair_id);
  int GetBulletID();
public:
	void Fire2();
	void ExchangeFishScore2();
private:
	//银行操作
	void BankOperate();
	//机器人换桌进出
	void huanzhuojinchu();
};

#endif  // ANDROID_USER_ITEM_SINK_H_
