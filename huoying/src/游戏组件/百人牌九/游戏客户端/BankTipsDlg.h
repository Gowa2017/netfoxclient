#pragma once

#include "Stdafx.h"
#include "Resource.h"

// CBankTipsDlg �Ի���

class CBankTipsDlg : public CSkinDialog
{
  DECLARE_DYNAMIC(CBankTipsDlg)

public:
  CBankTipsDlg(CWnd* pParent = NULL);   // ��׼���캯��
  virtual ~CBankTipsDlg();

  //�ؼ�����
public:
  CSkinButton           m_btOK;               //ȷ����ť
  CSkinButton           m_btCancel;             //ȡ����ť
  bool                            m_bRestore;             //�Ƿ���
  CPoint                          m_StartLoc;             //��ʼλ��
  void              SetStartLoc(CPoint startPoint);

// �Ի�������
  enum { IDD = IDD_BANKTIPS_DLG };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
  //��ʼ������
  virtual BOOL OnInitDialog();
  //ȷ����Ϣ
  virtual void OnOK();

  DECLARE_MESSAGE_MAP()
};
