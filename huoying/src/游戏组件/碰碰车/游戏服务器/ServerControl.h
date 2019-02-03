#pragma once

//游戏控制基类
class IServerControl
{
public:
  IServerControl(void) {};
  virtual ~IServerControl(void) {};

public:
  //服务器控制
  virtual bool  ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame) = NULL;

  //需要控制
  virtual bool  NeedControl() = NULL;

  //满足控制
  virtual bool  MeetControl(tagControlInfo ControlInfo) = NULL;

  //完成控制
  virtual bool  CompleteControl() = NULL;

  //返回控制区域
  virtual bool  ReturnControlArea(tagControlInfo& ControlInfo) = NULL;
};
