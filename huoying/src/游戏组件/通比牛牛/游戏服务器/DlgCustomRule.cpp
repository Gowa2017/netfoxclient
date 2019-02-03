#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
  //���ñ���
  ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));

  m_CustomRule.lRobotScoreMin = 100000;
  m_CustomRule.lRobotScoreMax = 1000000;
  m_CustomRule.lRobotBankGet = 1000000;
  m_CustomRule.lRobotBankGetBanker = 10000000;
  m_CustomRule.lRobotBankStoMul = 10;

  //������
  m_CustomRule.lStorageStart = 1000000;
  m_CustomRule.lStorageDeduct = 0;
  m_CustomRule.lStorageMax = 5000000;
  m_CustomRule.lStorageMul = 80;
  m_CustomRule.lBonus = 500000;

  return;
}

//��������
CDlgCustomRule::~CDlgCustomRule()
{
}

//���ú���
BOOL CDlgCustomRule::OnInitDialog()
{
  __super::OnInitDialog();

  //���ÿؼ�
  ((CEdit*)GetDlgItem(IDC_EDIT_ROBOTSCOREMIN))->LimitText(10);
  ((CEdit*)GetDlgItem(IDC_EDIT_ROBOTSCOREMAX))->LimitText(10);
  ((CEdit*)GetDlgItem(IDC_EDIT_ROBOTBANKGET))->LimitText(10);
  ((CEdit*)GetDlgItem(IDC_EDIT_ROBOTBANKGETBANKER))->LimitText(10);
  ((CEdit*)GetDlgItem(IDC_EDIT_ROBOTBANKSTOMUL))->LimitText(2);

  ((CEdit*)GetDlgItem(IDC_EDIT_STORAGE_START))->LimitText(10);
  ((CEdit*)GetDlgItem(IDC_EDIT_STORAGE_DEDUCT))->LimitText(3);
  ((CEdit*)GetDlgItem(IDC_EDIT_STORAGE_MAX))->LimitText(10);
  ((CEdit*)GetDlgItem(IDC_EDIT_STORAGE_MUL))->LimitText(2);
  ((CEdit*)GetDlgItem(IDC_EDIT_BONUS))->LimitText(10);

  //���²���
  FillDataToControl();

  return FALSE;
}

//ȷ������
VOID CDlgCustomRule::OnOK()
{
  //Ͷ����Ϣ
  GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDOK, 0), 0);

  return;
}

//ȡ����Ϣ
VOID CDlgCustomRule::OnCancel()
{
  //Ͷ����Ϣ
  GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDCANCEL, 0), 0);

  return;
}

//���¿ؼ�
bool CDlgCustomRule::FillDataToControl()
{
  //�����˴洢��
  SetDlgItemInt(IDC_EDIT_ROBOTSCOREMIN, m_CustomRule.lRobotScoreMin);
  SetDlgItemInt(IDC_EDIT_ROBOTSCOREMAX, m_CustomRule.lRobotScoreMax);
  SetDlgItemInt(IDC_EDIT_ROBOTBANKGET, m_CustomRule.lRobotBankGet);
  SetDlgItemInt(IDC_EDIT_ROBOTBANKGETBANKER, m_CustomRule.lRobotBankGetBanker);
  SetDlgItemInt(IDC_EDIT_ROBOTBANKSTOMUL, m_CustomRule.lRobotBankStoMul);

  //������
  SetDlgItemInt(IDC_EDIT_STORAGE_START, m_CustomRule.lStorageStart);
  SetDlgItemInt(IDC_EDIT_STORAGE_DEDUCT, m_CustomRule.lStorageDeduct);
  SetDlgItemInt(IDC_EDIT_STORAGE_MAX, m_CustomRule.lStorageMax);
  SetDlgItemInt(IDC_EDIT_STORAGE_MUL, m_CustomRule.lStorageMul);
  SetDlgItemInt(IDC_EDIT_BONUS, m_CustomRule.lBonus);

  return true;
}

//��������
bool CDlgCustomRule::FillControlToData()
{
  //�����˴洢��
  m_CustomRule.lRobotScoreMin = (SCORE)GetDlgItemInt(IDC_EDIT_ROBOTSCOREMIN);
  m_CustomRule.lRobotScoreMax = (SCORE)GetDlgItemInt(IDC_EDIT_ROBOTSCOREMAX);
  m_CustomRule.lRobotBankGet = (SCORE)GetDlgItemInt(IDC_EDIT_ROBOTBANKGET);
  m_CustomRule.lRobotBankGetBanker = (SCORE)GetDlgItemInt(IDC_EDIT_ROBOTBANKGETBANKER);
  m_CustomRule.lRobotBankStoMul = (SCORE)GetDlgItemInt(IDC_EDIT_ROBOTBANKSTOMUL);

  //������
  m_CustomRule.lStorageStart = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_START);
  m_CustomRule.lStorageDeduct = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_DEDUCT);
  m_CustomRule.lStorageMax = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_MAX);
  m_CustomRule.lStorageMul = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_MUL);
  m_CustomRule.lBonus = (SCORE)GetDlgItemInt(IDC_EDIT_BONUS);

  if(!(m_CustomRule.lRobotScoreMax > m_CustomRule.lRobotScoreMin))
  {
    AfxMessageBox(TEXT("������ֵӦ����ȡ����Сֵ"), MB_ICONSTOP);
    return false;
  }

  if(!(m_CustomRule.lRobotBankGetBanker > m_CustomRule.lRobotBankGet))
  {
    AfxMessageBox(TEXT("ȡ���������Ӧ����ȡ����С����"), MB_ICONSTOP);
    return false;
  }

  return true;
}

//��ȡ����
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
  //��ȡ����
  if(FillControlToData() == true)
  {
    CustomRule = m_CustomRule;
    return true;
  }

  return false;
}

//��������
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
  //���ñ���
  m_CustomRule = CustomRule;

  //���²���
  if(m_hWnd != NULL)
  {
    FillDataToControl();
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////////
