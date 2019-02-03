#include "Stdafx.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameOption, CSkinDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameOption::CGameOption() : CSkinDialog(IDD_OPTION)
{
  m_bEnableSound=true;
  m_bEnableBGSound=true;
  return;
}

//��������
CGameOption::~CGameOption()
{
}

//�ؼ���
void CGameOption::DoDataExchange(CDataExchange * pDX)
{
  __super::DoDataExchange(pDX);
  DDX_Control(pDX, IDOK, m_btOK);
  DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//��ʼ������
BOOL CGameOption::OnInitDialog()
{
  __super::OnInitDialog();

  //���ñ���
  SetWindowText(TEXT("��Ϸ����"));
  //���ÿؼ�
  if(m_bEnableSound==true)
  {
    ((CButton *)GetDlgItem(IDC_ENABLE_SOUND))->SetCheck(BST_CHECKED);
  }

  if(m_bEnableBGSound==true)
  {
    ((CButton *)GetDlgItem(IDC_ENABLE_BG_SOUND))->SetCheck(BST_CHECKED);
  }

  return TRUE;
}

//ȷ����Ϣ
void CGameOption::OnOK()
{
  //��ȡ����
  m_bEnableSound=(((CButton *)GetDlgItem(IDC_ENABLE_SOUND))->GetCheck()==BST_CHECKED);
  m_bEnableBGSound=(((CButton *)GetDlgItem(IDC_ENABLE_BG_SOUND))->GetCheck()==BST_CHECKED);

  __super::OnOK();
}

//////////////////////////////////////////////////////////////////////////
