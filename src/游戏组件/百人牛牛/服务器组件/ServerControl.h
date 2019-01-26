#pragma once

#define MAX_CARD          5

//��Ϸ���ƻ���
class IServerControl
{
public:
  IServerControl(void) {};
  virtual ~IServerControl(void) {};

public:
  //����������
  virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption) = NULL;

  //��Ҫ����
  virtual bool  NeedControl() = NULL;

  //�������
  virtual bool  MeetControl(tagControlInfo ControlInfo) = NULL;

  //��ɿ���
  virtual bool  CompleteControl() = NULL;

  //���ؿ�������
  virtual bool  ReturnControlArea(tagControlInfo& ControlInfo) = NULL;

  //��ʼ����
  virtual void  GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], SCORE lAllJettonScore[]) = NULL;
  //��ʼ����
  virtual void  GetControlArea(BYTE Area[], BYTE &cbControlStyle) = NULL;
  //��ʼ����
  virtual BYTE  GetControlTybe() = NULL;
  //��ʼ����
  virtual void  SetControlArea(BYTE Area[]) = NULL;
};
