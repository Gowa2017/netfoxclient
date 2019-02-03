#pragma once
#include "Stdafx.h"
#include "../�ͻ������/ClientControl.h"


// CClientControlItemSinkDlg �Ի���

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
  DECLARE_DYNAMIC(CClientControlItemSinkDlg)

//������
protected:
  LONGLONG            m_lStorageStart;        //��ʼ���
  LONGLONG            m_lStorageDeduct;       //���˥��
  LONGLONG            m_lStorageCurrent;        //��ǰ���
  LONGLONG            m_lStorageMax1;         //�������1
  LONGLONG            m_lStorageMul1;         //ϵͳ��ָ���1
  LONGLONG            m_lStorageMax2;         //�������2
  LONGLONG            m_lStorageMul2;         //ϵͳ��ָ���2

  //�������
protected:
  bool m_bWinArea[CONTROL_AREA];
  BYTE m_cbControlStyle;
  BYTE m_cbExcuteTimes;

  //�����ע
protected:
  CListCtrl           m_listUserBet;          //�б�ؼ�
  CListCtrl           m_listUserBetAll;       //�б�ؼ�
  LONGLONG            m_lQueryGameID;         //��ѯID

public:
  CClientControlItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
  virtual ~CClientControlItemSinkDlg();

// �Ի�������
  enum { IDD = IDD_DIALOG_ADMIN };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

  DECLARE_MESSAGE_MAP()

public:
  //���¿���
  virtual void __cdecl OnAllowControl(bool bEnable);
  //������
  virtual bool __cdecl ReqResult(const void * pBuffer);
  //���¿��
  virtual bool __cdecl UpdateStorage(const void * pBuffer);
  //������ע
  virtual void __cdecl UpdateUserBet(bool bReSet);
  //���¿ؼ�
  virtual void __cdecl UpdateControl();

public:
  //���ý���
  void ReSetAdminWnd();

public:
  //��ʾ״̬
  void PrintCurStatus();

public:
  afx_msg void  OnReSet();
  afx_msg void  OnRefresh();
  afx_msg void  OnExcute();
  afx_msg void  OnRadioClick();
protected:
  void RequestUpdateStorage();
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedBtnUpdateStorage();
  afx_msg void OnBnClickedBtnUserBetQuery();
  afx_msg void OnBnClickedBtnUserBetAll();
  afx_msg void OnEnSetfocusEditUserId();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
