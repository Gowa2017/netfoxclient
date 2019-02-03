#if !defined(AFX_SKINHEADERCTRL_H__5A55D5B6_CD01_4049_943B_36EAC3B4B976__INCLUDED_)
#define AFX_SKINHEADERCTRL_H__5A55D5B6_CD01_4049_943B_36EAC3B4B976__INCLUDED_

#pragma once

#include "resource.h"

#include "SkinHorizontalScrollbar.h"
#include "SkinVerticleScrollbar.h"

#include "SkinScrollBarEx.h"
//////////////////////////////////////////////////////////////////////////
class CSkinListCtrlEx;

struct sUserInfo
{
  //�����Ϣ
  CString             strUserName;            //����ʺ�
  LONGLONG            lUserScore;             //��ҽ��
  LONGLONG            lWinScore;              //��Ӯ���
  WORD              wImageIndex;            //λͼ����
  WORD              wAndrod;              //������ʶ
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
public:
//  virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};

//////////////////////////////////////////////////////////////////////////

//��Ƥ���б�
class CSkinListCtrlEx : public CListCtrl
{

  //���ݱ���
protected:
  bool            m_bAscendSort;          //�����־
  //��������
public:
  CSkinScrollBarEx      m_SkinScrollBar;        //��������
  CSkinHeaderCtrlEx     m_ListHeader;         //�б�ͷ
  //CImageList          m_ImageUserStatus;        //״̬λͼ
  CBitImage         m_ImageUserStatus;          //״̬λͼ

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
  void InserUser(sUserInfo & UserInfo);
  //�������
  bool FindUser(CString lpszUserName);
  //ɾ���û�
  void DeleteUser(sUserInfo & UserInfo);
  //�����б�
  void UpdateUser(sUserInfo & UserInfo);
  //����б�
  void ClearAll();
  //����λ��
  void ReSetLoc(int cx, int cy);
  //�����и�
  void SetItemHeight(UINT nHeight);

  //�滭����
protected:
  //��ȡ��ɫ
  virtual VOID GetItemColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF & crColorText, COLORREF & crColorBack);
  //�滭����
  virtual VOID DrawCustomItem(CDC * pDC, LPDRAWITEMSTRUCT lpDrawItemStruct, CRect & rcSubItem, INT nColumnIndex);

  //���غ���
protected:
  //�ؼ���
  virtual void PreSubclassWindow();
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


  //��Ϣ����
protected:
  //{{AFX_MSG(CSkinHeaderCtrlEx)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg BOOL OnEraseBkgnd(CDC * pDC);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
};

//////////////////////////////////////////////////////////////////////////

#endif
