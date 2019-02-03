#pragma once

//��Ϸ���ƻ���
class IServerControl
{
public:
  IServerControl(void) {};
  virtual ~IServerControl(void) {};

public:
  //����������
  virtual bool  ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame) = NULL;

  //��Ҫ����
  virtual bool  NeedControl() = NULL;

  //�������
  virtual bool  MeetControl(tagControlInfo ControlInfo) = NULL;

  //��ɿ���
  virtual bool  CompleteControl() = NULL;

  //���ؿ�������
  virtual bool  ReturnControlArea(tagControlInfo& ControlInfo) = NULL;
};
