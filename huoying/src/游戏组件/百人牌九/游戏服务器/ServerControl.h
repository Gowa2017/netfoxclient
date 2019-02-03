#pragma once

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
  virtual bool __cdecl NeedControl() = NULL;

  //�������
  virtual bool __cdecl MeetControl(tagControlInfo ControlInfo) = NULL;

  //��ɿ���
  virtual bool __cdecl CompleteControl() = NULL;

  //���ؿ�������
  virtual bool __cdecl ReturnControlArea(tagControlInfo& ControlInfo) = NULL;

  //��ʼ����
  virtual void __cdecl GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], SCORE lAllJettonScore[]) = NULL;

  //��ʼ����
  virtual void __cdecl GetControlArea(BYTE Area[], BYTE & cbControlTimes){}
};
