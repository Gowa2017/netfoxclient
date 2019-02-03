#pragma once
#include "Stdafx.h"
#include "afxcmn.h"
#include "afxwin.h"


// CTrolListDialog 对话框

class CTrolListDialog : public CDialog
{
  DECLARE_DYNAMIC(CTrolListDialog)

public:
  CTrolListDialog(CWnd* pParent = NULL);   // 标准构造函数
  virtual ~CTrolListDialog();

// 对话框数据
  enum { IDD = IDD_CTROL_LIST };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  CListCtrl m_SkinListCtrl;
  virtual BOOL OnInitDialog();
  void ClearClist();
  void UpdateClistCtrl(LONGLONG m_lAllPlayBet[GAME_PLAYER][AREA_COUNT],BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair,IClientUserItem* pUserData);
private:
  LONGLONG                        m_areAtotalBet[AREA_COUNT];
  IClientUserItem* m_pUserData;
public:
  CEdit m_cedit;
};
