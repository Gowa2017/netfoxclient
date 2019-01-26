#pragma once


//��Ϸ���ƻ���
class IClientControlDlg : public CSkinDialog 
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}

	//���÷�����
	virtual void SetRoomStorage(LONGLONG lRoomStartStorage, LONGLONG lRoomCurrentStorage) = NULL;
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo) = NULL;
	//��ѯ�û����
	virtual void RequestQueryResult(CMD_S_RequestQueryResult *pQueryResult) = NULL;
	//�����û����ƽ��
	virtual void RoomUserControlResult(CMD_S_UserControl *pUserControl) = NULL;
	//�û��������
	virtual void UserControlComplete(CMD_S_UserControlComplete *pUserControlComplete) = NULL;
	//������¼
	virtual void UpdateOperationRecord(CMD_S_Operation_Record *pOperation_Record) = NULL;
	//���ø��¶�ʱ��
	virtual void SetUpdateIDI() = NULL; 
	//���·�����Ϣ
	virtual void UpdateRoomInfoResult(CMD_S_RequestUpdateRoomInfo_Result *RoomInfo_Result) = NULL; 
};
