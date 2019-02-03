#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
  //设置变量
  ZeroMemory(&m_CustomRule,sizeof(m_CustomRule));

  m_CustomRule.lRobotScoreMin = 100000;
  m_CustomRule.lRobotScoreMax = 1000000;
  m_CustomRule.lRobotBankGet = 1000000;
  m_CustomRule.lRobotBankGetBanker = 10000000;
  m_CustomRule.lRobotBankStoMul = 10;

  //库存控制
  m_CustomRule.lStorageStart = 1000000;
  m_CustomRule.lStorageDeduct = 0;
  m_CustomRule.lStorageMax = 5000000;
  m_CustomRule.lStorageMul = 80;
  m_CustomRule.lBonus = 500000;

  return;
}

//析构函数
CDlgCustomRule::~CDlgCustomRule()
{
}

//配置函数
BOOL CDlgCustomRule::OnInitDialog()
{
  __super::OnInitDialog();

  //设置控件
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

  //更新参数
  FillDataToControl();

  return FALSE;
}

//确定函数
VOID CDlgCustomRule::OnOK()
{
  //投递消息
  GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDOK, 0), 0);

  return;
}

//取消消息
VOID CDlgCustomRule::OnCancel()
{
  //投递消息
  GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDCANCEL, 0), 0);

  return;
}

//更新控件
bool CDlgCustomRule::FillDataToControl()
{
  //机器人存储款
  SetDlgItemInt(IDC_EDIT_ROBOTSCOREMIN, m_CustomRule.lRobotScoreMin);
  SetDlgItemInt(IDC_EDIT_ROBOTSCOREMAX, m_CustomRule.lRobotScoreMax);
  SetDlgItemInt(IDC_EDIT_ROBOTBANKGET, m_CustomRule.lRobotBankGet);
  SetDlgItemInt(IDC_EDIT_ROBOTBANKGETBANKER, m_CustomRule.lRobotBankGetBanker);
  SetDlgItemInt(IDC_EDIT_ROBOTBANKSTOMUL, m_CustomRule.lRobotBankStoMul);

  //库存控制
  SetDlgItemInt(IDC_EDIT_STORAGE_START, m_CustomRule.lStorageStart);
  SetDlgItemInt(IDC_EDIT_STORAGE_DEDUCT, m_CustomRule.lStorageDeduct);
  SetDlgItemInt(IDC_EDIT_STORAGE_MAX, m_CustomRule.lStorageMax);
  SetDlgItemInt(IDC_EDIT_STORAGE_MUL, m_CustomRule.lStorageMul);
  SetDlgItemInt(IDC_EDIT_BONUS, m_CustomRule.lBonus);

  return true;
}

//更新数据
bool CDlgCustomRule::FillControlToData()
{
  //机器人存储款
  m_CustomRule.lRobotScoreMin = (SCORE)GetDlgItemInt(IDC_EDIT_ROBOTSCOREMIN);
  m_CustomRule.lRobotScoreMax = (SCORE)GetDlgItemInt(IDC_EDIT_ROBOTSCOREMAX);
  m_CustomRule.lRobotBankGet = (SCORE)GetDlgItemInt(IDC_EDIT_ROBOTBANKGET);
  m_CustomRule.lRobotBankGetBanker = (SCORE)GetDlgItemInt(IDC_EDIT_ROBOTBANKGETBANKER);
  m_CustomRule.lRobotBankStoMul = (SCORE)GetDlgItemInt(IDC_EDIT_ROBOTBANKSTOMUL);

  //库存控制
  m_CustomRule.lStorageStart = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_START);
  m_CustomRule.lStorageDeduct = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_DEDUCT);
  m_CustomRule.lStorageMax = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_MAX);
  m_CustomRule.lStorageMul = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_MUL);
  m_CustomRule.lBonus = (SCORE)GetDlgItemInt(IDC_EDIT_BONUS);

  if(!(m_CustomRule.lRobotScoreMax > m_CustomRule.lRobotScoreMin))
  {
    AfxMessageBox(TEXT("存款最大值应大于取款最小值"), MB_ICONSTOP);
    return false;
  }

  if(!(m_CustomRule.lRobotBankGetBanker > m_CustomRule.lRobotBankGet))
  {
    AfxMessageBox(TEXT("取款最大数额应大于取款最小数额"), MB_ICONSTOP);
    return false;
  }

  return true;
}

//读取配置
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
  //读取参数
  if(FillControlToData() == true)
  {
    CustomRule = m_CustomRule;
    return true;
  }

  return false;
}

//设置配置
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
  //设置变量
  m_CustomRule = CustomRule;

  //更新参数
  if(m_hWnd != NULL)
  {
    FillDataToControl();
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////////
