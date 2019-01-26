#pragma once

#define MAX_CARD          5

//游戏控制基类
class IServerControl
{
public:
  IServerControl(void) {};
  virtual ~IServerControl(void) {};

public:
  //服务器控制
  virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption) = NULL;

  //需要控制
  virtual bool  NeedControl() = NULL;

  //满足控制
  virtual bool  MeetControl(tagControlInfo ControlInfo) = NULL;

  //完成控制
  virtual bool  CompleteControl() = NULL;

  //返回控制区域
  virtual bool  ReturnControlArea(tagControlInfo& ControlInfo) = NULL;

  //开始控制
  virtual void  GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], SCORE lAllJettonScore[]) = NULL;
  //开始控制
  virtual void  GetControlArea(BYTE Area[], BYTE &cbControlStyle) = NULL;
  //开始控制
  virtual BYTE  GetControlTybe() = NULL;
  //开始控制
  virtual void  SetControlArea(BYTE Area[]) = NULL;
};
