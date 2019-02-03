#pragma once

//游戏控制基类
class IClientControlDlg : public CDialog
{
public:
  IClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent) {}
  virtual ~IClientControlDlg(void) {}
  CUserBetArray         m_UserBetArray;         //用户下注
public:
  //更新控制
  virtual void  SetStorageCurrentValue(LONGLONG lvalue)=NULL;
  virtual void  UpdateControl(CMD_S_ControlReturns* pControlReturns) = NULL;
  virtual void UpdateClistCtrl(LONGLONG m_lAllPlayBet[GAME_PLAYER][AREA_COUNT],BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair,IClientUserItem* pUserData)=NULL;
  virtual   void ClearClist()=NULL;
  virtual     void ClearText()=NULL;
  //更新下注
  virtual void __cdecl UpdateUserBet(bool bReSet) = NULL;
  //更新库存
  virtual bool __cdecl UpdateStorage(const void * pBuffer) = NULL;
};
