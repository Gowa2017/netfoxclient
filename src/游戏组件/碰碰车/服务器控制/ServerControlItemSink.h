#pragma once
#include "../��Ϸ������/ServerControl.h"
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
  //���Ʋ���
protected:
  BYTE              m_cbControlArea;            //��������
  BYTE              m_cbControlTimes;           //���ƴ���


public:
  CServerControlItemSink(void);
  virtual ~CServerControlItemSink(void);


public:
  //����������
  virtual bool  ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);

  //��Ҫ����
  virtual bool  NeedControl();

  //�������
  virtual bool  MeetControl(tagControlInfo ControlInfo);

  //��ɿ���
  virtual bool  CompleteControl();

  //���ؿ�������
  virtual bool  ReturnControlArea(tagControlInfo& ControlInfo);

  void  UpdateControl(CMD_S_ControlReturns* pControlReturns,IServerUserItem * pIServerUserItem);

  //��Ϣ����
  void PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes);
  // ��¼����
  void WriteInfo(LPCTSTR pszString);

};
