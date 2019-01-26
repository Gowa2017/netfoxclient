#if !defined(AFX_SKINHEADERCTRL_H__5A55D5B6_CD01_4049_943B_36EAC3B4B976__INCLUDED_)
#define AFX_SKINHEADERCTRL_H__5A55D5B6_CD01_4049_943B_36EAC3B4B976__INCLUDED_

#pragma once

#include "resource.h"


//////////////////////////////////////////////////////////////////////////
class CSkinListCtrlEx;

struct sUserInfo
{
  //玩家信息
  CString             strUserName;            //玩家帐号
  LONGLONG            lUserScore;             //玩家金币
  LONGLONG            lWinScore;              //输赢金币
  int               nImageIndex;            //位图索引
};

//排序信息
struct tagSortInfo
{
  bool              bAscendSort;            //升序标志
  WORD              wColumnIndex;           //列表索引
  CSkinListCtrlEx *       pSkinListCtrl;            //列表控件
};


//换皮肤列表头
class CSkinHeaderCtrlEx : public CHeaderCtrl
{
  //消息函数
public:
  CSkinHeaderCtrlEx();
  CBitmap m_bmpTitle;
  CFont font;

  //控件消息
  virtual BOOL OnChildNotify(UINT uMessage, WPARAM wParam, LPARAM lParam, LRESULT * pLResult);

protected:
  //{{AFX_MSG(CSkinHeaderCtrlEx)
  afx_msg void OnPaint();
  afx_msg BOOL OnEraseBkgnd(CDC * pDC);

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
typedef struct
{
  COLORREF colText;
  COLORREF colTextBk;
} TEXT_BK;
//换皮肤列表
class CSkinListCtrlEx : public CListCtrl
{

  //数据变量
protected:
  bool            m_bAscendSort;          //升序标志
  //变量定义
public:

  CSkinHeaderCtrlEx     m_ListHeader;         //列表头

  //组件变量
protected:
  //CSkinVerticleScrollbar    m_SkinVerticleScrollbar;    //垂直滚动
  //CSkinHorizontalScrollbar  m_SkinHorizontalScrollbar;    //水平滚动
  //函数定义
public:
  //构造函数
  CSkinListCtrlEx();
  //析构函数
  virtual ~CSkinListCtrlEx();

  //插入列表
  void InserUser(sUserInfo & UserInfo,bool bSetColor=false,COLORREF TextColor=RGB(255,0,0), COLORREF TextBkColor=RGB(255,255,255));
  //查找玩家
  bool FindUser(CString lpszUserName);
  //删除用户
  void DeleteUser(sUserInfo & UserInfo);
  //更新列表
  void UpdateUser(sUserInfo & UserInfo);
  //清空列表
  void ClearAll();
  //初始化
  void Init();
  //重设位置
  void ReSetLoc(int cx, int cy);

  //重载函数
protected:
  //控件绑定
  virtual void PreSubclassWindow();
  //初始化控件
  virtual BOOL InitListCtrl();
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

  //静态函数
protected:
  //排列函数
  static INT CALLBACK SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

  //排列数据
  virtual INT SortItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex, bool bAscendSort);

  //消息映射
protected:
  //点击列表
  VOID OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult);
  CMap<DWORD, DWORD&, TEXT_BK, TEXT_BK&> MapItemColor;

  //消息函数
protected:
  //{{AFX_MSG(CSkinHeaderCtrlEx)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg BOOL OnEraseBkgnd(CDC * pDC);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:

public:
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  void SetItemColor(DWORD iItem, COLORREF TextColor, COLORREF TextBkColor);   //设置某一行的前景色和背景色
  void SetAllItemColor(DWORD iItem, COLORREF TextColor, COLORREF TextBkColor);//设置全部行的前景色和背景色
  void ClearColor();
  void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

//////////////////////////////////////////////////////////////////////////

#endif
