#pragma once
#include "Stdafx.h"
#include "Resource.h"

// CMessageTipDlg �Ի���

class CMessageTipDlg : public CSkinDialog
{
  DECLARE_DYNAMIC(CMessageTipDlg)

public:
  CMessageTipDlg(CWnd* pParent = NULL);   // ��׼���캯��
  virtual ~CMessageTipDlg();

  //�ؼ�����
public:
  CSkinButton           m_btOK;               //ȷ����ť
  CSkinButton           m_btCancel;             //ȡ����ť

// �Ի�������
  enum { IDD = IDD_MESSAGEDLG };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
  //��ʼ������
  virtual BOOL OnInitDialog();
  //ȷ����Ϣ
  virtual void OnOK();

  DECLARE_MESSAGE_MAP()
};
