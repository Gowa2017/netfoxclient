#pragma once

//��Ϸ���ƻ���
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	//����������
	virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption) = NULL;

	//��Ҫ����
	virtual bool __cdecl NeedControl() = NULL;

	//���ؿ�������
	virtual bool __cdecl ControlResult(BYTE	cbTableCardArray[], BYTE cbCardCount[]) = NULL;

	//��ʼ����
	virtual void __cdecl GetControlArea(BYTE Area[], BYTE cbControlTimes){}
};
