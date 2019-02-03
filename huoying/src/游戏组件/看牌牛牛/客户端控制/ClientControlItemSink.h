#pragma once
#include "../游戏客户端/ClientControl.h"

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
  DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
  SCORE            m_lMaxStorage;          //库存上限
  WORD              m_wStorageMul;          //赢分概率
  SCORE            m_lStorageCurrent;        //库存数值
  SCORE            m_lStorageDeduct;       //库存衰减
  CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO>  m_userRosterMap;      //名单映射
  //控件变量
public:
  CSkinButton           m_btaddUserRoster;
  CSkinButton           m_btdeleteUserRoster;
  CSkinButton           m_btModifyStorage;
  CSkinEdit           m_editCurrentStorage;
  CSkinEdit           m_editStorageDeduct;
  CSkinEdit           m_editStorageMax;
  CSkinEdit           m_editStorageMul;
  CSkinEdit           m_editUserGameID;
  CSkinEdit           m_editUserScoreLimit;
  CSkinListCtrl         m_listUserRoster;


public:
  CClientControlItemSinkDlg(CWnd* pParent = NULL);
  virtual ~CClientControlItemSinkDlg();

// 对话框数据
  enum { IDD = IDD_CLIENT_CONTROL };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  DECLARE_MESSAGE_MAP()

public:
  //设置起始库存
  virtual void SetStartStorage(SCORE lStorageScore);
  //设置库存信息
  virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo);
  virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual void OnCancel();

  virtual bool ReqResultAddUserRoster(CMD_S_AddUserRoster_Result * Roster_Result);

  virtual bool ReqResultDeleteUserRoster(CMD_S_DeleteUserRoster_Result * Roster_Result);

  //设置特殊权限
  virtual void SetCheatRight();

  //更新用户名单
  virtual void UpdateUserRosterListCtrl(CMD_S_UpdateUserRoster *pUpdateUserRoster);
  //移除键值
  virtual void RemoveKeyUserRoster(CMD_S_RemoveKeyUserRoster *pRemoveKeyUserRoster);
  //重复用户名单
  virtual void DuplicateUserRoster(CMD_S_DuplicateUserRoster *pDuplicateUserRoster);

public:
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnAddUserRoster();
  afx_msg void OnDeleteUserRoster();
  afx_msg void OnModifyStorage();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnClickListUserRoster(NMHDR* pNMHDR, LRESULT* pResult);
};
