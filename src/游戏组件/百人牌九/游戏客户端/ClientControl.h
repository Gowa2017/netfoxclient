#pragma once

//��Ϸ���ƻ���
class IClientControlDlg : public CDialog
{

public:
  CUserBetArray         m_UserBetArray;         //�û���ע

public:
  IClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent) {}
  virtual ~IClientControlDlg(void) {}

public:
  //���¿���
  virtual void __cdecl OnAllowControl(bool bEnable) = NULL;
  //������
  virtual bool __cdecl ReqResult(const void * pBuffer) = NULL;
  //���¿��
  virtual bool __cdecl UpdateStorage(const void * pBuffer) = NULL;
  //������ע
  virtual void __cdecl UpdateUserBet(bool bReSet) = NULL;
  //���¿ؼ�
  virtual void __cdecl UpdateControl() = NULL;
};
