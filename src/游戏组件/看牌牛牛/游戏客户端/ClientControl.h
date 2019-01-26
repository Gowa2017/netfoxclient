#pragma once

//游戏控制基类
class IClientControlDlg : public CSkinDialog
{
public:
  IClientControlDlg(UINT UID, CWnd* pParent) : CSkinDialog(UID, pParent) {}
  virtual ~IClientControlDlg(void) {}

  //设置起始库存
  virtual void SetStartStorage(LONGLONG lStorageScore) = NULL;
  virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo) = NULL;

  virtual bool ReqResultAddUserRoster(CMD_S_AddUserRoster_Result * Roster_Result) = NULL;

  virtual bool ReqResultDeleteUserRoster(CMD_S_DeleteUserRoster_Result * Roster_Result) = NULL;

  //设置特殊权限
  virtual void SetCheatRight() = NULL;

  //更新用户名单
  virtual void UpdateUserRosterListCtrl(CMD_S_UpdateUserRoster *pUpdateUserRoster) = NULL;
  //移除键值
  virtual void RemoveKeyUserRoster(CMD_S_RemoveKeyUserRoster *pRemoveKeyUserRoster) = NULL;
  //重复用户名单
  virtual void DuplicateUserRoster(CMD_S_DuplicateUserRoster *pDuplicateUserRoster) = NULL;
};

