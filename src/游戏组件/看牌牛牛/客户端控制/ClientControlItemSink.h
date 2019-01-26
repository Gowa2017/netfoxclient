#pragma once
#include "../��Ϸ�ͻ���/ClientControl.h"

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
  DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
  SCORE            m_lMaxStorage;          //�������
  WORD              m_wStorageMul;          //Ӯ�ָ���
  SCORE            m_lStorageCurrent;        //�����ֵ
  SCORE            m_lStorageDeduct;       //���˥��
  CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO>  m_userRosterMap;      //����ӳ��
  //�ؼ�����
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

// �Ի�������
  enum { IDD = IDD_CLIENT_CONTROL };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  DECLARE_MESSAGE_MAP()

public:
  //������ʼ���
  virtual void SetStartStorage(SCORE lStorageScore);
  //���ÿ����Ϣ
  virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo);
  virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual void OnCancel();

  virtual bool ReqResultAddUserRoster(CMD_S_AddUserRoster_Result * Roster_Result);

  virtual bool ReqResultDeleteUserRoster(CMD_S_DeleteUserRoster_Result * Roster_Result);

  //��������Ȩ��
  virtual void SetCheatRight();

  //�����û�����
  virtual void UpdateUserRosterListCtrl(CMD_S_UpdateUserRoster *pUpdateUserRoster);
  //�Ƴ���ֵ
  virtual void RemoveKeyUserRoster(CMD_S_RemoveKeyUserRoster *pRemoveKeyUserRoster);
  //�ظ��û�����
  virtual void DuplicateUserRoster(CMD_S_DuplicateUserRoster *pDuplicateUserRoster);

public:
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnAddUserRoster();
  afx_msg void OnDeleteUserRoster();
  afx_msg void OnModifyStorage();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnClickListUserRoster(NMHDR* pNMHDR, LRESULT* pResult);
};
