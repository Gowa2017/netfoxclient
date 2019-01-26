#pragma once

//��Ϸ���ƻ���
class IClientControlDlg : public CDialog
{
public:
  IClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent) {}
  virtual ~IClientControlDlg(void) {}
  CUserBetArray         m_UserBetArray;         //�û���ע
public:
  //���¿���
  virtual void  SetStorageCurrentValue(LONGLONG lvalue)=NULL;
  virtual void  UpdateControl(CMD_S_ControlReturns* pControlReturns) = NULL;
  virtual void UpdateClistCtrl(LONGLONG m_lAllPlayBet[GAME_PLAYER][AREA_COUNT],BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair,IClientUserItem* pUserData)=NULL;
  virtual   void ClearClist()=NULL;
  virtual     void ClearText()=NULL;
  //������ע
  virtual void __cdecl UpdateUserBet(bool bReSet) = NULL;
  //���¿��
  virtual bool __cdecl UpdateStorage(const void * pBuffer) = NULL;
};
