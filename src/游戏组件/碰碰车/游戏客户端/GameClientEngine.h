#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

//#include "DirectSound.h"
#include "afxtempl.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientEngine : public CGameFrameEngine
{
  //友元声明
  friend class CGameClientView;

  //限制信息
protected:
  LONGLONG            m_lMeMaxScore;            //最大下注
  LONGLONG            m_lAreaLimitScore;          //区域限制
  LONGLONG            m_lApplyBankerCondition;      //申请条件
  int                             m_GameEndTime;
  bool              m_blUsing;
  LONGLONG           m_lStorageCurrent;              //房间启动每桌子的库存数值，读取失败按 0 设置
  //个人下注
protected:
  LONGLONG            m_lUserJettonScore[AREA_COUNT+1]; //个人总注

  //庄家信息
protected:
  LONGLONG            m_lBankerScore;           //庄家积分
  WORD              m_wCurrentBanker;         //当前庄家
  BYTE              m_cbLeftCardCount;          //扑克数目
  bool              m_bEnableSysBanker;         //系统做庄
  bool                            m_bPlaySound;
  bool                            m_bAddScore;///是否进行了下注
  //状态变量
protected:
  bool              m_bMeApplyBanker;         //申请标识

  //控件变量
protected:
  CGameLogic            m_GameLogic;            //游戏逻辑
  CGameClientView         m_GameClientView;         //游戏视图

  //控件变量
protected:
  CList<tagAndroidBet,tagAndroidBet>    m_ListAndroid;        //机器人下注链表

  //声音资源
protected:
//  CDirectSound          m_DTSDBackground;         //背景音乐
//  CDirectSound          m_DTSDCheer[4];           //背景音乐

  //全局配置
protected:
  CGlobalUnits *                  m_pGlobalUnits;                     //全局单元


  struct SortTemp
  {
    LONGLONG a;
    LONGLONG b;
    LONGLONG c;
    LONGLONG d;
    SortTemp()
    {
      memset(this,0,sizeof(*this));
    }
  };


  SortTemp m_TempData;

  //函数定义
public:
  //构造函数
  CGameClientEngine();
  //析构函数
  virtual ~CGameClientEngine();

  //常规继承
private:
  //初始函数
  virtual bool OnInitGameEngine();
  //重置框架
  virtual bool OnResetGameEngine();
  //游戏设置
  virtual void OnGameOptionSet();

  //时钟事件
public:
  //时钟删除
  virtual bool OnEventGameClockKill(WORD wChairID);
  //时钟信息
  virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);
  //声音控制
  virtual bool AllowBackGroundSound(bool bAllowSound);

  //游戏事件
public:
  //旁观消息
  virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
  //游戏消息
  virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
  //场景消息
  virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

  //消息处理
protected:
  //游戏开始
  bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
  //游戏空闲
  bool OnSubGameFree(const void * pBuffer, WORD wDataSize);
  //用户加注
  bool OnSubPlaceJetton(const void * pBuffer, WORD wDataSize);
  //游戏结束
  bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
  //申请做庄
  bool OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize);
  //取消做庄
  bool OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize);
  //切换庄家
  bool OnSubChangeBanker(const void * pBuffer, WORD wDataSize);
  //游戏记录
  bool OnSubGameRecord(const void * pBuffer, WORD wDataSize);
  //下注失败
  bool OnSubPlaceJettonFail(const void * pBuffer, WORD wDataSize);
  //扑克牌
  bool OnSubSendCard(const void * pBuffer, WORD wDataSize);

  bool OnSubCheckImageIndex(const void * pBuffer, WORD wDataSize);
  //控制
  bool OnSubAdminControl(const void * pBuffer, WORD wDataSize);


  //赋值函数
protected:
  //设置庄家
  void SetBankerInfo(WORD wBanker,LONGLONG lScore);
  //个人下注
  void SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount);

  //功能函数
protected:
  //更新控制
  void UpdateButtonContron();

  //消息映射
protected:
  //加注消息
  LRESULT OnPlaceJetton(WPARAM wParam, LPARAM lParam);
  //申请消息
  LRESULT OnApplyBanker(WPARAM wParam, LPARAM lParam);
  //继续发牌
  LRESULT OnContinueCard(WPARAM wParam, LPARAM lParam);
  //手工搓牌
  LRESULT  OnOpenCard(WPARAM wParam, LPARAM lParam);
  //自动搓牌
  LRESULT  OnAutoOpenCard(WPARAM wParam, LPARAM lParam);
  //自动搓牌
  LRESULT  OnShowResult(WPARAM wParam, LPARAM lParam);

  LRESULT  OnPlaySound(WPARAM wParam, LPARAM lParam);
  //控制
  LRESULT  OnAdminControl(WPARAM wParam, LPARAM lParam);
  //更新库存
  LRESULT OnStorage(WPARAM wParam,LPARAM lParam);
  //更新库存
  LRESULT OnDeuct(WPARAM wParam,LPARAM lParam);
  //游戏声音
  LRESULT OnMessageGameSound(WPARAM wParam, LPARAM lParam);

  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnTimer(UINT nIDEvent);
  //更新库存
  bool OnSubUpdateStorage(const void * pBuffer, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////
