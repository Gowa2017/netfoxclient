#pragma once

//��Ϸ���ƻ���
class IClientControlDlg : public CSkinDialog
{
public:
  IClientControlDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent) {}
  virtual ~IClientControlDlg(void) {}

  //������ʼ���
  virtual void SetStartStorage(LONGLONG lStorageScore) = NULL;
  virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo) = NULL;

  virtual bool ReqResultAddUserRoster(CMD_S_AddUserRoster_Result * Roster_Result) = NULL;

  virtual bool ReqResultDeleteUserRoster(CMD_S_DeleteUserRoster_Result * Roster_Result) = NULL;

  //��������Ȩ��
  virtual void SetCheatRight() = NULL;

  //�����û�����
  virtual void UpdateUserRosterListCtrl(CMD_S_UpdateUserRoster *pUpdateUserRoster) = NULL;
  //�Ƴ���ֵ
  virtual void RemoveKeyUserRoster(CMD_S_RemoveKeyUserRoster *pRemoveKeyUserRoster) = NULL;
  //�ظ��û�����
  virtual void DuplicateUserRoster(CMD_S_DuplicateUserRoster *pDuplicateUserRoster) = NULL;
};

