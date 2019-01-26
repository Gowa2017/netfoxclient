#pragma once
#include "../游戏客户端/ClientControl.h"
#include "SkinListCtrl.h"
#include "afxcmn.h"

// CClientControlItemSinkDlg 对话框
#ifndef _UNICODE
#define myprintf  _snprintf
#define mystrcpy  strcpy
#define mystrlen  strlen
#define myscanf   _snscanf
#define myLPSTR   LPCSTR
#define myatoi      atoi
#define myatoi64    _atoi64
#else
#define myprintf  swprintf
#define mystrcpy  wcscpy
#define mystrlen  wcslen
#define myscanf   _snwscanf
#define myLPSTR   LPWSTR
#define myatoi      _wtoi
#define myatoi64  _wtoi64
#endif
class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
  DECLARE_DYNAMIC(CClientControlItemSinkDlg)

public:
  CClientControlItemSinkDlg(CWnd* pParent = NULL);   // 标准构造函数
  virtual ~CClientControlItemSinkDlg();
  LONGLONG            m_lStorageStart;            //库存数值
  LONGLONG            m_lStorageDeduct;           //库存衰减

// 对话框数据
  enum { IDD = IDD_CLIENT_CONTROL_EX };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

  DECLARE_MESSAGE_MAP()

public:
  //更新控制
  virtual void  UpdateControl(CMD_S_ControlReturns* pControlReturns);

protected:
  //信息解析
  void PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes);
  //重置界面
  void ReSetAdminWnd();

public:
  //初始化
  virtual BOOL OnInitDialog();
  //设置颜色
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  //取消控制
  afx_msg void OnBnClickedButtonReset();
  //本局控制
  afx_msg void OnBnClickedButtonSyn();
  //开启控制
  afx_msg void OnBnClickedButtonOk();
  //更新下注
  virtual void __cdecl UpdateUserBet(bool bReSet);
  //取消关闭
  afx_msg void OnBnClickedButtonCancel();
  afx_msg void  OnFreshStorage();
  afx_msg void  OnFreshDeuct();
  //更新库存
  virtual bool __cdecl UpdateStorage(const void * pBuffer);
  //CSkinListCtrlEx m_SkinListCtrl;
  CSkinListCtrlEx           m_listUserBet;          //列表控件
  CListCtrl           m_listUserBetAll;       //列表控件
  LONGLONG            m_lQueryGameID;         //查询ID
  void SetStorageCurrentValue(LONGLONG lvalue);

  void ClearClist();
  virtual     void ClearText();
  void UpdateClistCtrl(LONGLONG m_lAllPlayBet[GAME_PLAYER][AREA_COUNT],BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair,IClientUserItem* pUserData);
private:
  LONGLONG                        m_areAtotalBet[AREA_COUNT];
  IClientUserItem* m_pUserData;
public:


  LONGLONG m_lcurrentStorage;
  LONGLONG m_lStorageMax1;
  LONGLONG m_lStorageMax2;
  LONGLONG m_lStorageMul1;
  LONGLONG m_lStorageMul2;
  afx_msg void OnStnClickedStaticInfo();
  afx_msg void OnBnClickedAreaFreshsto();
  afx_msg void OnBnClickedQuery();
  afx_msg void OnBnClickedBtReset();
  afx_msg void OnBnClickedBtCurset();

  afx_msg void OnEnSetfocusEditUserId();
  afx_msg void OnBnClickedTotal();

};
