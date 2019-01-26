#if !defined(AFX_SKINHEADERCTRL_H__5A55D5B6_CD01_4049_943B_36EAC3B4B976__INCLUDED_)
#define AFX_SKINHEADERCTRL_H__5A55D5B6_CD01_4049_943B_36EAC3B4B976__INCLUDED_

#pragma once

#include "resource.h"


//////////////////////////////////////////////////////////////////////////
class CSkinListCtrlEx;

struct sUserInfo
{
  //�����Ϣ
  CString             strUserName;            //����ʺ�
  LONGLONG            lUserScore;             //��ҽ��
  LONGLONG            lWinScore;              //��Ӯ���
  int               nImageIndex;            //λͼ����
};

//������Ϣ
struct tagSortInfo
{
  bool              bAscendSort;            //�����־
  WORD              wColumnIndex;           //�б�����
  CSkinListCtrlEx *       pSkinListCtrl;            //�б�ؼ�
};


//��Ƥ���б�ͷ
class CSkinHeaderCtrlEx : public CHeaderCtrl
{
  //��Ϣ����
public:
  CSkinHeaderCtrlEx();
  CBitmap m_bmpTitle;
  CFont font;

  //�ؼ���Ϣ
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
//��Ƥ���б�
class CSkinListCtrlEx : public CListCtrl
{

  //���ݱ���
protected:
  bool            m_bAscendSort;          //�����־
  //��������
public:

  CSkinHeaderCtrlEx     m_ListHeader;         //�б�ͷ

  //�������
protected:
  //CSkinVerticleScrollbar    m_SkinVerticleScrollbar;    //��ֱ����
  //CSkinHorizontalScrollbar  m_SkinHorizontalScrollbar;    //ˮƽ����
  //��������
public:
  //���캯��
  CSkinListCtrlEx();
  //��������
  virtual ~CSkinListCtrlEx();

  //�����б�
  void InserUser(sUserInfo & UserInfo,bool bSetColor=false,COLORREF TextColor=RGB(255,0,0), COLORREF TextBkColor=RGB(255,255,255));
  //�������
  bool FindUser(CString lpszUserName);
  //ɾ���û�
  void DeleteUser(sUserInfo & UserInfo);
  //�����б�
  void UpdateUser(sUserInfo & UserInfo);
  //����б�
  void ClearAll();
  //��ʼ��
  void Init();
  //����λ��
  void ReSetLoc(int cx, int cy);

  //���غ���
protected:
  //�ؼ���
  virtual void PreSubclassWindow();
  //��ʼ���ؼ�
  virtual BOOL InitListCtrl();
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

  //��̬����
protected:
  //���к���
  static INT CALLBACK SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

  //��������
  virtual INT SortItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex, bool bAscendSort);

  //��Ϣӳ��
protected:
  //����б�
  VOID OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult);
  CMap<DWORD, DWORD&, TEXT_BK, TEXT_BK&> MapItemColor;

  //��Ϣ����
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
  void SetItemColor(DWORD iItem, COLORREF TextColor, COLORREF TextBkColor);   //����ĳһ�е�ǰ��ɫ�ͱ���ɫ
  void SetAllItemColor(DWORD iItem, COLORREF TextColor, COLORREF TextBkColor);//����ȫ���е�ǰ��ɫ�ͱ���ɫ
  void ClearColor();
  void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

//////////////////////////////////////////////////////////////////////////

#endif
