#include "Stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
  ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTab)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
}

//��������
CDlgCustomRule::~CDlgCustomRule()
{
}

//�ؼ���
VOID CDlgCustomRule::DoDataExchange(CDataExchange* pDX)
{
  __super::DoDataExchange(pDX);
}

//��ʼ������
BOOL CDlgCustomRule::OnInitDialog()
{
  __super::OnInitDialog();

  ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->InsertItem(0,TEXT("ͨ�ù���"));
  ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->InsertItem(1,TEXT("������"));

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

//ȷ������
VOID CDlgCustomRule::OnOK()
{
  //Ͷ����Ϣ
  GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

  return;
}

//ȡ����Ϣ
VOID CDlgCustomRule::OnCancel()
{
  //Ͷ����Ϣ
  GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

  return;
}

//��������
BOOL CDlgCustomRule::UpdateConfigData(BOOL bSaveAndValidate)
{
  if(bSaveAndValidate)  //�ӿؼ���ȡ���ݵ�����
  {
    m_DlgCustomGeneral.UpdateData(TRUE);
    m_DlgCustomAndroid.UpdateData(TRUE);

    CopyMemory(&m_CustomConfig.CustomGeneral, &m_DlgCustomGeneral.m_CustomGeneral, sizeof(tagCustomGeneral));
    CopyMemory(&m_CustomConfig.CustomAndroid, &m_DlgCustomAndroid.m_CustomAndroid, sizeof(tagCustomAndroid));

  }
  else          //��������ֵ���ؼ���ʾ
  {
    CopyMemory(&m_DlgCustomGeneral.m_CustomGeneral, &m_CustomConfig.CustomGeneral, sizeof(tagCustomGeneral));
    CopyMemory(&m_DlgCustomAndroid.m_CustomAndroid, &m_CustomConfig.CustomAndroid, sizeof(tagCustomAndroid));

    m_DlgCustomGeneral.UpdateData(FALSE);
    m_DlgCustomAndroid.UpdateData(FALSE);
  }

  return TRUE;
}

//��������
bool CDlgCustomRule::SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
  //���ñ���
  m_wCustomSize=wCustomSize;
  m_pcbCustomRule=pcbCustomRule;

  //���ñ���
  ASSERT(m_pcbCustomRule);
  if(!m_pcbCustomRule)
  {
    return false;
  }
  tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
  //if( pCustomConfig->lApplyBankerCondition != 0 )
  memcpy(&m_CustomConfig, pCustomConfig, sizeof(tagCustomConfig));

  //���½���
  if(m_hWnd)
  {
    UpdateConfigData(FALSE);  //��������ֵ���ؼ���ʾ
  }

  return true;
}

//��������
bool CDlgCustomRule::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
  //���½���
  if(m_hWnd)
  {
    UpdateConfigData(TRUE);  //�ӿؼ���ȡ���ݵ�����
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.cbFreeTime<5||m_DlgCustomGeneral.m_CustomGeneral.cbFreeTime>99)
  {
    AfxMessageBox(L"[����ʱ��]���뷶ΧΪ5-99");
    return false;
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.cbBetTime<10||
     m_DlgCustomGeneral.m_CustomGeneral.cbBetTime>99)
  {
    AfxMessageBox(L"[��עʱ��]���뷶ΧΪ10-99");
    return false;
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.cbEndTime<20||m_DlgCustomGeneral.m_CustomGeneral.cbEndTime>99)
  {
    AfxMessageBox(L"[����ʱ��]���뷶ΧΪ20-99");
    return false;
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.StorageDeduct<0||m_DlgCustomGeneral.m_CustomGeneral.StorageDeduct>1000)
  {
    AfxMessageBox(L"[���˥��ֵ]���뷶ΧΪ0-1000");
    return false;
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.StorageMul1<0||m_DlgCustomGeneral.m_CustomGeneral.StorageMul1>100)
  {
    AfxMessageBox(L"[���Ӯ�ָ���1]���뷶ΧΪ0-100");
    return false;
  }
  if(m_DlgCustomGeneral.m_CustomGeneral.StorageMul2<0||m_DlgCustomGeneral.m_CustomGeneral.StorageMul2>100)
  {
    AfxMessageBox(L"[���Ӯ�ָ���2]���뷶ΧΪ0-100");
    return false;
  }

  if(m_DlgCustomAndroid.m_CustomAndroid.lRobotListMaxCount <= m_DlgCustomAndroid.m_CustomAndroid.lRobotListMinCount)
  {
    AfxMessageBox(L"�б��������Ӧ�ñ��б����������");
    return false;
  }

  if(m_DlgCustomAndroid.m_CustomAndroid.lRobotMaxBetTime <= m_DlgCustomAndroid.m_CustomAndroid.lRobotMinBetTime)
  {
    AfxMessageBox(L"����������Ӧ�ñȳ��������͸�");
    return false;
  }

  if(m_DlgCustomAndroid.m_CustomAndroid.lRobotMaxJetton <= m_DlgCustomAndroid.m_CustomAndroid.lRobotMinJetton)
  {
    AfxMessageBox(L"���������Ӧ�ñȳ�������С��");
    return false;
  }

  if(m_DlgCustomAndroid.m_CustomAndroid.lRobotBetMaxCount <= m_DlgCustomAndroid.m_CustomAndroid.lRobotBetMinCount)
  {
    AfxMessageBox(L"��ע�������Ӧ�ñ���ע�������ٴ�");
    return false;
  }

  if(m_DlgCustomAndroid.m_CustomAndroid.lRobotSaveCondition <= m_DlgCustomAndroid.m_CustomAndroid.lRobotGetCondition)
  {
    AfxMessageBox(L"�������Ӧ�ñ�ȡ��������");
    return false;
  }

  if(m_DlgCustomAndroid.m_CustomAndroid.lRobotGetMax <= m_DlgCustomAndroid.m_CustomAndroid.lRobotGetMin)
  {
    AfxMessageBox(L"ȡ�����ֵӦ�ñ�ȡ����Сֵ��");
    return false;
  }

  if(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankStoMul<0||m_DlgCustomAndroid.m_CustomAndroid.lRobotBankStoMul>100)
  {
    AfxMessageBox(L"[���ٷֱ�]���뷶ΧΪ0-100");
    return false;
  }
  CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lApplyBankerCondition);
  CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerTime);
  CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerTimeAdd);
  CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerScoreMAX);
  CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lBankerTimeExtra);

  CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.StorageStart);

  CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.StorageMax1);

  CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.StorageMax2);

  CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lAreaLimitScore);
  CheckDataMinMax(m_DlgCustomGeneral.m_CustomGeneral.lUserLimitScore);

  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBankerCount);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotListMinCount);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotListMaxCount);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotApplyBanker);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotWaitBanker);

  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMinBetTime);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMaxBetTime);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMinJetton);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotMaxJetton);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBetMinCount);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotBetMaxCount);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotAreaLimit);


  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotGetCondition);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotSaveCondition);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotGetMin);
  CheckDataMinMax(m_DlgCustomAndroid.m_CustomAndroid.lRobotGetMax);


  //���ñ���
  m_wCustomSize=wCustomSize;
  m_pcbCustomRule=pcbCustomRule;

  //���ñ���
  ASSERT(m_pcbCustomRule);
  if(!m_pcbCustomRule)
  {
    return true;
  }
  tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;
  memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

  return true;
}

//��������
bool CDlgCustomRule::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize)
{
  //���ñ���
  m_wCustomSize=wCustomSize;
  m_pcbCustomRule=pcbCustomRule;

  //���ñ���
  ASSERT(m_pcbCustomRule);
  if(!m_pcbCustomRule)
  {
    return true;
  }
  tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pcbCustomRule;

  m_CustomConfig.DefaultCustomRule();
  memcpy(pCustomConfig, &m_CustomConfig, sizeof(tagCustomConfig));

  //���½���
  if(m_hWnd)
  {
    UpdateConfigData(FALSE);  // ��������ֵ���ؼ���ʾ
  }

  return true;
}
//���������������ʱ ɾ������������ѡ��
void CDlgCustomRule::DeleteAndroid()
{
  ((CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM))->DeleteItem(1);
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

//���캯��
CDlgCustomGeneral::CDlgCustomGeneral() : CDialog(IDD_CUSTOM_GENERAL)
{
}

//��������
CDlgCustomGeneral::~CDlgCustomGeneral()
{
}

//�ؼ���
VOID CDlgCustomGeneral::DoDataExchange(CDataExchange* pDX)
{
  __super::DoDataExchange(pDX);

  DDX_Text(pDX,IDC_EDIT_GENERAL_1,m_CustomGeneral.lApplyBankerCondition);
  DDX_Text(pDX,IDC_EDIT_GENERAL_2,m_CustomGeneral.lBankerTime);
  DDX_Text(pDX,IDC_EDIT_GENERAL_3,m_CustomGeneral.lBankerTimeAdd);
  DDX_Text(pDX,IDC_EDIT_GENERAL_4,m_CustomGeneral.lBankerScoreMAX);
  DDX_Text(pDX,IDC_EDIT_GENERAL_5,m_CustomGeneral.lBankerTimeExtra);
  DDX_Text(pDX,IDC_EDIT_GENERAL_6,m_CustomGeneral.cbFreeTime);
  DDX_Text(pDX,IDC_EDIT_GENERAL_7,m_CustomGeneral.cbBetTime);
  DDX_Text(pDX,IDC_EDIT_GENERAL_8,m_CustomGeneral.cbEndTime);
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


  DDX_Check(pDX,IDC_CHECK_GENERAL_1,m_CustomGeneral.nEnableSysBanker);

  //DDV_MinMaxLongLong(pDX,m_CustomGeneral.lApplyBankerCondition, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomGeneral.lBankerTime, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomGeneral.lBankerTimeAdd, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomGeneral.lBankerScoreMAX, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomGeneral.lBankerTimeExtra, 0, LLONG_MAX);
  //DDV_MinMaxByte(pDX,m_CustomGeneral.cbFreeTime, 5, 99);
  //DDV_MinMaxByte(pDX,m_CustomGeneral.cbBetTime, 10, 99);
  //DDV_MinMaxByte(pDX,m_CustomGeneral.cbEndTime, 20, 99);
  //DDV_MinMaxLongLong(pDX,m_CustomGeneral.lAreaLimitScore, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomGeneral.lUserLimitScore, 0, LLONG_MAX);

  DDV_MaxChars(pDX,m_CustomGeneral.szMessageItem1,64);
  DDV_MaxChars(pDX,m_CustomGeneral.szMessageItem2,64);
  DDV_MaxChars(pDX,m_CustomGeneral.szMessageItem3,64);

  //DDV_MinMaxLongLong(pDX,m_CustomGeneral.StorageStart, 0, LLONG_MAX);
  //
  //DDV_MinMaxLongLong(pDX,m_CustomGeneral.StorageMax1, 0, LLONG_MAX);

  //DDV_MinMaxLongLong(pDX,m_CustomGeneral.StorageMax2, 0, LLONG_MAX);


}

//��ʼ������
BOOL CDlgCustomGeneral::OnInitDialog()
{
  __super::OnInitDialog();

  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCustomAndroid, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgCustomAndroid::CDlgCustomAndroid() : CDialog(IDD_CUSTOM_ANDROID)
{
}

//��������
CDlgCustomAndroid::~CDlgCustomAndroid()
{
}

//�ؼ���
VOID CDlgCustomAndroid::DoDataExchange(CDataExchange* pDX)
{
  __super::DoDataExchange(pDX);

  DDX_Text(pDX,IDC_EDIT_ANDROID_1,m_CustomAndroid.lRobotBankerCount);
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

  DDX_Text(pDX,IDC_EDIT_ANDROID_16,m_CustomAndroid.lRobotGetCondition);
  DDX_Text(pDX,IDC_EDIT_ANDROID_17,m_CustomAndroid.lRobotSaveCondition);
  DDX_Text(pDX,IDC_EDIT_ANDROID_18,m_CustomAndroid.lRobotGetMin);
  DDX_Text(pDX,IDC_EDIT_ANDROID_19,m_CustomAndroid.lRobotGetMax);
  DDX_Text(pDX,IDC_EDIT_ANDROID_20,m_CustomAndroid.lRobotBankStoMul);
  DDX_Check(pDX,IDC_CHECK_ANDROID_1,m_CustomAndroid.nEnableRobotBanker);

  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankerCountMin, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotListMinCount, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotListMaxCount, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotApplyBanker, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotWaitBanker, 0, LLONG_MAX);

  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotMinBetTime, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotMaxBetTime, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotMinJetton, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotMaxJetton, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBetMinCount, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBetMaxCount, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotAreaLimit, 0, LLONG_MAX);


  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankGetMin, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankGetMax, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankGetBankerMin, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankGetBankerMax, 0, LLONG_MAX);
  //DDV_MinMaxLongLong(pDX,m_CustomAndroid.lRobotBankStoMul, 0, 100);

}

//��ʼ������
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
    str.Format(L"���뷶ΧΪ0-%I64d",LLONG_MAX);
    AfxMessageBox(str);
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////