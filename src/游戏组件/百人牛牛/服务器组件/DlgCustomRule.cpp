#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
  ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTab)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
}

//析构函数
CDlgCustomRule::~CDlgCustomRule()
{
}

//控件绑定
VOID CDlgCustomRule::DoDataExchange(CDataExchange* pDX)
{
  __super::DoDataExchange(pDX);
}

//初始化函数
BOOL CDlgCustomRule::OnInitDialog()
{
  __super::OnInitDialog();

  ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->InsertItem(0,TEXT("通用功能"));
  ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->InsertItem(1,TEXT("机器人"));

  m_DlgCustomGeneral.Create(IDD_CUSTOM_GENERAL,GetDlgItem(IDC_TAB_CUSTOM));
  m_DlgCustomAndroid.Create(IDD_CUSTOM_ANDROID,GetDlgItem(IDC_TAB_CUSTOM));

  CRect rcTabClient;
  GetDlgItem(IDC_TAB_CUSTOM)->GetClientRect(rcTabClient);
  rcTabClient.top+=20;
  rcTabClient.bottom-=4;
  rcTabClient.left+=4;
  rcTabClient.right-=4;
  m_DlgCustomGeneral.MoveWindow(rcTabClient);
  m_DlgCustomAndroid.MoveWindow(rcTabClient);

  m_DlgCustomGeneral.ShowWindow(TRUE);
  m_DlgCustomAndroid.ShowWindow(FALSE);
  ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->SetCurSel(0);

  return TRUE;
}

//确定函数
VOID CDlgCustomRule::OnOK()
{
  //投递消息
  GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

  return;
}

//取消消息
VOID CDlgCustomRule::OnCancel()
{
  //投递消息
  GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

  return;
}

//更新数据
BOOL CDlgCustomRule::UpdateConfigData(BOOL bSaveAndValidate)
{
  if(bSaveAndValidate)  //从控件读取数据到变量
  {
    m_DlgCustomGeneral.UpdateData(TRUE);
    m_DlgCustomAndroid.UpdateData(TRUE);

    CopyMemory(&m_CustomConfig.CustomGeneral, &m_DlgCustomGeneral.m_CustomGeneral, sizeof(tagCustomGeneral));
    CopyMemory(&m_CustomConfig.CustomAndroid, &m_DlgCustomAndroid.m_CustomAndroid, sizeof(tagCustomAndroid));

  }
  else          //拷贝变量值到控件显示
  {
    CopyMemory(&m_DlgCustomGeneral.m_CustomGeneral, &m_CustomConfig.CustomGeneral, sizeof(tagCustomGeneral));
    CopyMemory(&m_DlgCustomAndroid.m_CustomAndroid, &m_CustomConfig.CustomAndroid, sizeof(tagCustomAndroid));

    m_DlgCustomGeneral.UpdateData(FALSE);
    m_DlgCustomAndroid.UpdateData(FALSE);
  }

  return TRUE;
}

//设置配置
bool CDlgCustomRule::SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
  //设置变量
  m_wCustomSize=wCustomSize;
  m_pcbCustomRule=pcbCustomRule;

  //配置变量
  ASSERT(m_pcbCustomRule);
  if(!m_pcbCustomRule)
  {
    return false;
  }
  tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
  //if( pCustomConfig->lApplyBankerCondition != 0 )
  memcpy(&m_CustomConfig, pCustomConfig, sizeof(tagCustomConfig));

  //更新界面
  if(m_hWnd)
  {
    UpdateConfigData(FALSE);  //拷贝变量值到控件显示
  }

  return true;
}

//保存数据
bool CDlgCustomRule::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
  //更新界面
  if(m_hWnd)
  {
    //检测输入值非空
    bool bDataOK = true;
    INT nID = IDC_EDIT_GENERAL_1;
    while(nID <= IDC_EDIT_GENERAL_20)
    {
      TCHAR szData[256] = {0};
      m_DlgCustomGeneral.GetDlgItemText(nID, szData, sizeof(szData));

      if(0 == szData[0])
      {
        if(IDOK  == AfxMessageBox(TEXT("请输入一个合法数据！")))
        {
          return false;
        }
      }

      nID ++;
    }
    nID = IDC_EDIT_ANDROID_1;
    while(nID <= IDC_EDIT_ANDROID_20)
    {
      TCHAR szData[256] = {0};
      m_DlgCustomAndroid.GetDlgItemText(nID, szData, sizeof(szData));

      if(0 == szData[0])
      {
        if(IDOK  == AfxMessageBox(TEXT("请输入一个合法数据！")))
        {
          return false;
        }
      }

      nID ++;
    }

    UpdateConfigData(TRUE); //从控件读取数据到变量
  }

  if(m_DlgCustomGeneral.m_CustomGeneral.lFreeTime<5||m_DlgCustomGeneral.m_CustomGeneral.lFreeTime>99)
  {
    AfxMessageBox(L"[空闲时间]输入范围为5-99");
    return false;
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.lBetTime<10||
     m_DlgCustomGeneral.m_CustomGeneral.lBetTime>99)
  {
    AfxMessageBox(L"[下注时间]输入范围为10-99");
    return false;
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.lEndTime<20||m_DlgCustomGeneral.m_CustomGeneral.lEndTime>99)
  {
    AfxMessageBox(L"[结束时间]输入范围为20-99");
    return false;
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.StorageDeduct<0||m_DlgCustomGeneral.m_CustomGeneral.StorageDeduct>1000)
  {
    AfxMessageBox(L"[库存衰减值]输入范围为0-1000");
    return false;
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.StorageMul1<0||m_DlgCustomGeneral.m_CustomGeneral.StorageMul1>100)
  {
    AfxMessageBox(L"[玩家赢分概率1]输入范围为0-100");
    return false;
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.StorageMul2<0||m_DlgCustomGeneral.m_CustomGeneral.StorageMul2>100)
  {
    AfxMessageBox(L"[玩家赢分概率2]输入范围为0-100");
    return false;
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.lEndGameMul<1||m_DlgCustomGeneral.m_CustomGeneral.lEndGameMul>100)
  {
    AfxMessageBox(L"[提前开牌百分比]输入范围为1-100");
    return false;
  }

  if(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankStoMul<0||m_DlgCustomAndroid.m_CustomAndroid.lRobotBankStoMul>100)
  {
    AfxMessageBox(L"[存款百分比]输入范围为0-100");
    return false;
  }

  if(!CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lApplyBankerCondition))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerTime))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerTimeAdd))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerScoreMAX))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerTimeExtra))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.StorageStart))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.StorageMax1))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.StorageMax2))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lAreaLimitScore))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lUserLimitScore))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankerCountMin))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankerCountMax))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotListMinCount))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotListMaxCount))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotApplyBanker))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotWaitBanker))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMinBetTime))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMaxBetTime))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMinJetton))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMaxJetton))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBetMinCount))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBetMaxCount))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotAreaLimit))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotScoreMin))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotScoreMax))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankGetMin))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankGetMax))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankGetBankerMin))
  {
    return false;
  }
  if(!CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankGetBankerMax))
  {
    return false;
  }

  //设置变量
  m_wCustomSize=wCustomSize;
  m_pcbCustomRule=pcbCustomRule;

  //配置变量
  ASSERT(m_pcbCustomRule);
  if(!m_pcbCustomRule)
  {
    return true;
  }
  tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
  memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

  return true;
}

//保存数据
bool CDlgCustomRule::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
  //设置变量
  m_wCustomSize=wCustomSize;
  m_pcbCustomRule=pcbCustomRule;

  //配置变量
  ASSERT(m_pcbCustomRule);
  if(!m_pcbCustomRule)
  {
    return true;
  }
  tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;

  m_CustomConfig.DefaultCustomRule();
  memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

  //更新界面
  if(m_hWnd)
  {
    UpdateConfigData(FALSE);  // 拷贝变量值到控件显示
  }

  return true;
}

//隐藏机器人配置
bool CDlgCustomRule::HideAndroidModule(bool bHide)
{
  if(bHide)
  {
    ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->DeleteItem(1);
    m_DlgCustomAndroid.ShowWindow(false);
    ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->SetCurSel(0);
  }

  return true;
}

void CDlgCustomRule::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
  INT CurSel =((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->GetCurSel();

  switch(CurSel)
  {
    case 0:
      m_DlgCustomGeneral.ShowWindow(true);
      m_DlgCustomAndroid.ShowWindow(false);
      break;
    case 1:
      m_DlgCustomGeneral.ShowWindow(false);
      m_DlgCustomAndroid.ShowWindow(true);
      break;
  }
  *pResult = 0;

  return;
}

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomGeneral, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomGeneral::CDlgCustomGeneral() : CDialog(IDD_CUSTOM_GENERAL)
{
}

//析构函数
CDlgCustomGeneral::~CDlgCustomGeneral()
{
}

//控件绑定
VOID CDlgCustomGeneral::DoDataExchange(CDataExchange* pDX)
{
  __super::DoDataExchange(pDX);

  DDX_Text(pDX,IDC_EDIT_GENERAL_1,m_CustomGeneral.lApplyBankerCondition);
  DDX_Text(pDX,IDC_EDIT_GENERAL_2,m_CustomGeneral.lBankerTime);
  DDX_Text(pDX,IDC_EDIT_GENERAL_3,m_CustomGeneral.lBankerTimeAdd);
  DDX_Text(pDX,IDC_EDIT_GENERAL_4,m_CustomGeneral.lBankerScoreMAX);
  DDX_Text(pDX,IDC_EDIT_GENERAL_5,m_CustomGeneral.lBankerTimeExtra);
  DDX_Text(pDX,IDC_EDIT_GENERAL_6,m_CustomGeneral.lFreeTime);
  DDX_Text(pDX,IDC_EDIT_GENERAL_7,m_CustomGeneral.lBetTime);
  DDX_Text(pDX,IDC_EDIT_GENERAL_8,m_CustomGeneral.lEndTime);
  DDX_Text(pDX,IDC_EDIT_GENERAL_9,m_CustomGeneral.lAreaLimitScore);
  DDX_Text(pDX,IDC_EDIT_GENERAL_10,m_CustomGeneral.lUserLimitScore);

  DDX_Text(pDX,IDC_EDIT_GENERAL_11,m_CustomGeneral.szMessageItem1,64);
  DDX_Text(pDX,IDC_EDIT_GENERAL_12,m_CustomGeneral.szMessageItem2,64);
  DDX_Text(pDX,IDC_EDIT_GENERAL_13,m_CustomGeneral.szMessageItem3,64);

  DDX_Text(pDX,IDC_EDIT_GENERAL_14,m_CustomGeneral.StorageStart);
  DDX_Text(pDX,IDC_EDIT_GENERAL_15,m_CustomGeneral.StorageDeduct);
  DDX_Text(pDX,IDC_EDIT_GENERAL_16,m_CustomGeneral.StorageMax1);
  DDX_Text(pDX,IDC_EDIT_GENERAL_17,m_CustomGeneral.StorageMul1);
  DDX_Text(pDX,IDC_EDIT_GENERAL_18,m_CustomGeneral.StorageMax2);
  DDX_Text(pDX,IDC_EDIT_GENERAL_19,m_CustomGeneral.StorageMul2);

  DDX_Text(pDX,IDC_EDIT_GENERAL_20,m_CustomGeneral.lEndGameMul);

  DDX_Check(pDX,IDC_CHECK_GENERAL_1,m_CustomGeneral.nEnableSysBanker);

  /*DDV_MinMaxLongLong(pDX,m_CustomGeneral.lApplyBankerCondition, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomGeneral.lBankerTime, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomGeneral.lBankerTimeAdd, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomGeneral.lBankerScoreMAX, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomGeneral.lBankerTimeExtra, 0, LLONG_MAX);
  DDV_MinMaxByte(pDX,m_CustomGeneral.cbFreeTime, 5, 99);
  DDV_MinMaxByte(pDX,m_CustomGeneral.cbBetTime, 10, 99);
  DDV_MinMaxByte(pDX,m_CustomGeneral.cbEndTime, 20, 99);
  DDV_MinMaxLongLong(pDX,m_CustomGeneral.lAreaLimitScore, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomGeneral.lUserLimitScore, 0, LLONG_MAX);

  DDV_MaxChars(pDX,m_CustomGeneral.szMessageItem1,64);
  DDV_MaxChars(pDX,m_CustomGeneral.szMessageItem2,64);
  DDV_MaxChars(pDX,m_CustomGeneral.szMessageItem3,64);

  DDV_MinMaxLongLong(pDX,m_CustomGeneral.StorageStart, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomGeneral.StorageDeduct, 0, 1000);
  DDV_MinMaxLongLong(pDX,m_CustomGeneral.StorageMax1, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomGeneral.StorageMul1, 0, 100);
  DDV_MinMaxLongLong(pDX,m_CustomGeneral.StorageMax2, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomGeneral.StorageMul2, 0, 100);

  DDV_MinMaxLongLong(pDX,m_CustomGeneral.lEndGameMul, 1, 100);*/

}

//初始化函数
BOOL CDlgCustomGeneral::OnInitDialog()
{
  __super::OnInitDialog();

  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomAndroid, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomAndroid::CDlgCustomAndroid() : CDialog(IDD_CUSTOM_ANDROID)
{
}

//析构函数
CDlgCustomAndroid::~CDlgCustomAndroid()
{
}

//控件绑定
VOID CDlgCustomAndroid::DoDataExchange(CDataExchange* pDX)
{
  __super::DoDataExchange(pDX);

  DDX_Text(pDX,IDC_EDIT_ANDROID_1,m_CustomAndroid.lRobotBankerCountMin);
  DDX_Text(pDX,IDC_EDIT_ANDROID_2,m_CustomAndroid.lRobotBankerCountMax);
  DDX_Text(pDX,IDC_EDIT_ANDROID_3,m_CustomAndroid.lRobotListMinCount);
  DDX_Text(pDX,IDC_EDIT_ANDROID_4,m_CustomAndroid.lRobotListMaxCount);
  DDX_Text(pDX,IDC_EDIT_ANDROID_5,m_CustomAndroid.lRobotApplyBanker);
  DDX_Text(pDX,IDC_EDIT_ANDROID_6,m_CustomAndroid.lRobotWaitBanker);

  DDX_Text(pDX,IDC_EDIT_ANDROID_7,m_CustomAndroid.lRobotMinBetTime);
  DDX_Text(pDX,IDC_EDIT_ANDROID_8,m_CustomAndroid.lRobotMaxBetTime);
  DDX_Text(pDX,IDC_EDIT_ANDROID_9,m_CustomAndroid.lRobotMinJetton);
  DDX_Text(pDX,IDC_EDIT_ANDROID_10,m_CustomAndroid.lRobotMaxJetton);
  DDX_Text(pDX,IDC_EDIT_ANDROID_11,m_CustomAndroid.lRobotBetMinCount);
  DDX_Text(pDX,IDC_EDIT_ANDROID_12,m_CustomAndroid.lRobotBetMaxCount);
  DDX_Text(pDX,IDC_EDIT_ANDROID_13,m_CustomAndroid.lRobotAreaLimit);

  DDX_Text(pDX,IDC_EDIT_ANDROID_14,m_CustomAndroid.lRobotScoreMin);
  DDX_Text(pDX,IDC_EDIT_ANDROID_15,m_CustomAndroid.lRobotScoreMax);
  DDX_Text(pDX,IDC_EDIT_ANDROID_16,m_CustomAndroid.lRobotBankGetMin);
  DDX_Text(pDX,IDC_EDIT_ANDROID_17,m_CustomAndroid.lRobotBankGetMax);
  DDX_Text(pDX,IDC_EDIT_ANDROID_18,m_CustomAndroid.lRobotBankGetBankerMin);
  DDX_Text(pDX,IDC_EDIT_ANDROID_19,m_CustomAndroid.lRobotBankGetBankerMax);
  DDX_Text(pDX,IDC_EDIT_ANDROID_20,m_CustomAndroid.lRobotBankStoMul);
  DDX_Check(pDX,IDC_CHECK_ANDROID_1,m_CustomAndroid.nEnableRobotBanker);

  /*DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankerCountMin, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankerCountMax, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotListMinCount, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotListMaxCount, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotApplyBanker, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotWaitBanker, 0, LLONG_MAX);

  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotMinBetTime, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotMaxBetTime, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotMinJetton, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotMaxJetton, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBetMinCount, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBetMaxCount, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotAreaLimit, 0, LLONG_MAX);

  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotScoreMin, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotScoreMax, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankGetMin, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankGetMax, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankGetBankerMin, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankGetBankerMax, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankStoMul, 0, 100);*/

}

//初始化函数
BOOL CDlgCustomAndroid::OnInitDialog()
{
  __super::OnInitDialog();

  return TRUE;
}

bool CDlgCustomRule::CheckDataMinMax(LONGLONG valueMax)
{
  if(valueMax<0||valueMax>LLONG_MAX)
  {

    CString str;
    str.Format(L"输入范围为0-%I64d",LLONG_MAX);
    AfxMessageBox(str);

    return false;
  }
  return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////