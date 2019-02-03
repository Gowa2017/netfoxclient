#pragma once
#include "../游戏服务器/ServerControl.h"
#ifndef _UNICODE
#define myprintf  _snprintf
#define mystrcpy  strcpy
#define mystrlen  strlen
#define myscanf   _snscanf
#define myLPSTR   LPCSTR
#define myatoi      atoi
#define myatoi64    _atoi64
#else
#define myprintf  swprintf
#define mystrcpy  wcscpy
#define mystrlen  wcslen
#define myscanf   _snwscanf
#define myLPSTR   LPWSTR
#define myatoi      _wtoi
#define myatoi64  _wtoi64
#endif

class CServerControlItemSink : public IServerControl
{
  //控制操作
protected:
  BYTE              m_cbControlArea;            //控制区域
  BYTE              m_cbControlTimes;           //控制次数


public:
  CServerControlItemSink(void);
  virtual ~CServerControlItemSink(void);


public:
  //服务器控制
  virtual bool  ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);

  //需要控制
  virtual bool  NeedControl();

  //满足控制
  virtual bool  MeetControl(tagControlInfo ControlInfo);

  //完成控制
  virtual bool  CompleteControl();

  //返回控制区域
  virtual bool  ReturnControlArea(tagControlInfo& ControlInfo);

  void  UpdateControl(CMD_S_ControlReturns* pControlReturns,IServerUserItem * pIServerUserItem);

  //信息解析
  void PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes);
  // 记录函数
  void WriteInfo(LPCTSTR pszString);

};
