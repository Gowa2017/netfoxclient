// BankTipsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BankTipsDlg.h"


// CBankTipsDlg �Ի���

IMPLEMENT_DYNAMIC(CBankTipsDlg, CSkinDialog)
CBankTipsDlg::CBankTipsDlg(CWnd* pParent /*=NULL*/)
  : CSkinDialog(CBankTipsDlg::IDD, pParent)
{
}

CBankTipsDlg::~CBankTipsDlg()
{
  m_StartLoc.SetPoint(0,0);
}

void CBankTipsDlg::DoDataExchange(CDataExchange* pDX)
{
  CSkinDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDOK, m_btOK);
  DDX_Control(pDX, IDCANCEL, m_btCancel);
}


BEGIN_MESSAGE_MAP(CBankTipsDlg, CSkinDialog)
END_MESSAGE_MAP()


//��ʼ������
BOOL CBankTipsDlg::OnInitDialog()
{
  __super::OnInitDialog();

  m_bRestore=true;

  //���ñ���
  SetWindowText(TEXT("ѡ������"));
  //���ÿؼ�
  ((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(BST_CHECKED);

  CRect clientRect;
  GetClientRect(clientRect);

  MoveWindow(m_StartLoc.x,m_StartLoc.y,clientRect.Width(),clientRect.Height());

  return TRUE;
}

//ȷ����Ϣ
void CBankTipsDlg::OnOK()
{
  //��ȡ����
  m_bRestore=(((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck()==BST_CHECKED);

  __super::OnOK();
}

void CBankTipsDlg::SetStartLoc(CPoint startPoint)
{

  m_StartLoc=startPoint;
}

