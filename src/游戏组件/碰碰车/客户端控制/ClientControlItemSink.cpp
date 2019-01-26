// ClientControlItemSink.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"
//��������
#define ID_BEN_CHI_40         0                 //˳��
#define ID_BAO_MA_30          1                 //��߽�
#define ID_OU_DI_20         2                 //��
#define ID_DA_ZHONG_10          3                 //����
#define ID_BEN_CHI_5          4                 //����
#define ID_BAO_MA_5         5                 //�ұ߽�
#define ID_OU_DI_5          6                 //����
#define ID_DA_ZHONG_5         7                 //�ұ߽�

// CClientControlItemSinkDlg �Ի���

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
  : IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
  , m_lcurrentStorage(0)
  , m_lStorageMax1(0)
  , m_lStorageMax2(0)
  , m_lStorageMul1(0)
  , m_lStorageMul2(0)
  , m_lStorageStart(0)
  , m_lStorageDeduct(0)
{
  m_UserBetArray.RemoveAll();

  m_lQueryGameID = -1;
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
  IClientControlDlg::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_SCORE, m_listUserBet);
  DDX_Text(pDX, IDC_STATIC_TEXT, m_lStorageStart);
  DDX_Text(pDX, IDC_STORAGE, m_lcurrentStorage);
  DDX_Text(pDX, IDC_DEDUCT, m_lStorageDeduct);
  DDX_Text(pDX, IDC_STORAGE_LIM1, m_lStorageMax1);
  DDX_Text(pDX, IDC_STORAGE_LIM2, m_lStorageMax2);
  DDX_Text(pDX, IDC_STORAGE_SCORE1, m_lStorageMul1);
  DDX_Text(pDX, IDC_STORAGE_SCORE2, m_lStorageMul2);
  DDX_Control(pDX, IDC_LIST_USER_BET_ALL, m_listUserBetAll);
}


BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
  ON_WM_CTLCOLOR()
  ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
  ON_BN_CLICKED(IDC_BUTTON_SYN, OnBnClickedButtonSyn)
  ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
  ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
  ON_BN_CLICKED(IDC_FRESHSTO,OnFreshStorage)

  ON_STN_CLICKED(IDC_STATIC_INFO, OnStnClickedStaticInfo)
  ON_BN_CLICKED(IDC_AREA_FRESHSTO, OnBnClickedAreaFreshsto)
  ON_BN_CLICKED(IDC_QUERY, OnBnClickedQuery)
  ON_BN_CLICKED(IDC_BT_RESET, OnBnClickedBtReset)
  ON_BN_CLICKED(IDC_BT_CURSET, OnBnClickedBtCurset)
  ON_BN_CLICKED(IDC_TOTAL, OnBnClickedTotal)
  ON_EN_SETFOCUS(IDC_INPUT_ID, OnEnSetfocusEditUserId)
END_MESSAGE_MAP()

// CClientControlItemSinkDlg ��Ϣ�������

//��ʼ��
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
  IClientControlDlg::OnInitDialog();

  //������Ϣ
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));
  SetDlgItemText(IDC_STATIC_TIMES,TEXT("���ƾ�����"));
  SetDlgItemText(IDC_STATIC_AREA, TEXT("�������(�����ڿ�����)��"));
  SetDlgItemText(IDC_STATIC_NOTIC,TEXT("����˵����"));


  SetDlgItemText(IDC_RADIO_BEN_40,TEXT("��ʱ�ݡ�40"));
  SetDlgItemText(IDC_RADIO_BAO_30,TEXT("�����30"));
  SetDlgItemText(IDC_RADIO_AO_20,TEXT("���ۡ�20"));
  SetDlgItemText(IDC_RADIO_DA_10,TEXT("���ڡ�10"));
  SetDlgItemText(IDC_RADIO_BEN_5,TEXT("��ʱ�ݡ�5"));
  SetDlgItemText(IDC_RADIO_BAO_5,TEXT("�����5"));
  SetDlgItemText(IDC_RADIO_AO_5,TEXT("���ۡ�5"));
  SetDlgItemText(IDC_RADIO_DA_5,TEXT("���ڡ�5"));
  SetDlgItemText(IDC_BUTTON_RESET,TEXT("ȡ������"));
  SetDlgItemText(IDC_BUTTON_SYN,TEXT("������Ϣ"));
  SetDlgItemText(IDC_BUTTON_OK,TEXT("ִ��"));
  SetDlgItemText(IDC_BUTTON_CANCEL,TEXT("ȡ��"));;
  m_lStorageStart=0;
  m_lStorageDeduct=0;


  // TODO:  �ڴ���Ӷ���ĳ�ʼ��
  m_listUserBet.ModifyStyle(0,LVS_REPORT);
  //ͨ��GetWindowLong����ȡCListCtrl���е���ʽ
  DWORD dwStyle = GetWindowLong(m_listUserBet.m_hWnd, GWL_STYLE);

  //��ԭ����ʽ�Ļ����ϣ����LVS_REPORT��չ��ʽ
  SetWindowLong(m_listUserBet.m_hWnd, GWL_STYLE, dwStyle|LVS_REPORT);

  //��ȡ���е���չ��ʽ
  DWORD dwStyles = m_listUserBet.GetExStyle();
  //ȡ����ѡ����ʽ
  dwStyles &= ~LVS_EX_CHECKBOXES;

  //�������ѡ��ͱ������չ��ʽ
  m_listUserBet.SetExtendedStyle(dwStyles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  m_listUserBet.GetHeaderCtrl()->EnableWindow(false);
  INT nColumnCount=0;
  m_listUserBet.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,0);
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("���"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("����ǳ�"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("���ID"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("������"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("��Ӯ��"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("��ʱ��X40"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("����X30"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("����X20"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("����X10"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("��ʱ��X5"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("����X5"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("����X5"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("����X5"));

  CRect rcList;
  m_listUserBet.GetWindowRect(rcList);

  int widthCol=rcList.right-rcList.left;
  int tempWdith=60;
  m_listUserBet.SetColumnWidth(0, 0);
  m_listUserBet.SetColumnWidth(1, 40);
  m_listUserBet.SetColumnWidth(2, tempWdith);
  m_listUserBet.SetColumnWidth(3, tempWdith);
  m_listUserBet.SetColumnWidth(4, tempWdith);
  m_listUserBet.SetColumnWidth(5, tempWdith);
  m_listUserBet.SetColumnWidth(6, tempWdith);
  m_listUserBet.SetColumnWidth(7, tempWdith);
  m_listUserBet.SetColumnWidth(8, tempWdith);
  m_listUserBet.SetColumnWidth(9, tempWdith);
  m_listUserBet.SetColumnWidth(10, tempWdith);
  m_listUserBet.SetColumnWidth(11, tempWdith);
  m_listUserBet.SetColumnWidth(12, tempWdith);
  m_listUserBet.SetColumnWidth(13, tempWdith);
  m_listUserBetAll.SetExtendedStyle(m_listUserBetAll.GetExtendedStyle()|LVS_EX_GRIDLINES);
  nColumnCount=0;
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,281);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,tempWdith);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,tempWdith);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,tempWdith);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,tempWdith);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,tempWdith);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,tempWdith);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,tempWdith);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,tempWdith);
  m_listUserBetAll.InsertItem(0, TEXT(""));
  m_listUserBetAll.SetItemText(0, 0, TEXT("��ʵ�����עͳ��"));
  for(BYTE cbSubItem=1; cbSubItem <=8; cbSubItem++)
  {
    m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
  }
  SetDlgItemText(IDC_INPUT_ID, TEXT("���������ID"));
  return TRUE;
}

//������ɫ
HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

  if(pWnd->GetDlgCtrlID()==IDC_STATIC_INFO)
  {
    pDC->SetTextColor(RGB(255,10,10));
  }
  if(pWnd->GetDlgCtrlID()==IDC_STORAGE_STATIC)
  {
    pDC->SetTextColor(RGB(255,10,10));
  }
  return hbr;
}

//ȡ������
void CClientControlItemSinkDlg::OnBnClickedButtonReset()
{
  CMD_C_ControlApplication ControlApplication;
  ZeroMemory(&ControlApplication, sizeof(ControlApplication));
  ControlApplication.cbControlAppType = C_CA_CANCELS;
  //AfxGetMainWnd()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
  ReSetAdminWnd();
  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
  }

}

//���ֿ���
void CClientControlItemSinkDlg::OnBnClickedButtonSyn()
{
  CMD_C_ControlApplication ControlApplication;
  ZeroMemory(&ControlApplication, sizeof(ControlApplication));
  ControlApplication.cbControlAppType = C_CA_UPDATE;
  //AfxGetMainWnd()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);

  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
  }
}

//���¿��
void CClientControlItemSinkDlg::OnFreshStorage()
{
  //SetDlgItemInt(IDC_STORAGE,m_lStorageStart);
  UpdateData(TRUE);
  CString str;
  CMD_C_FreshStorage FreshStorage;
  //m_lStorageStart=(SCORE)GetDlgItemInt(IDC_STORAGE);
  //FreshStorage.lStorageStart=m_lStorageStart;
  FreshStorage.lStorageDeduct=m_lStorageDeduct;
  FreshStorage.lStorageCurrent=m_lcurrentStorage;

  FreshStorage.lStorageMax1=m_lStorageMax1;
  FreshStorage.lStorageMax2=m_lStorageMax2;
  FreshStorage.lStorageMul1=m_lStorageMul1;
  FreshStorage.lStorageMul2=m_lStorageMul2;
  if(m_lStorageDeduct<0||m_lStorageDeduct>1000)
  {
    AfxMessageBox(L"[���˥��ֵ]���뷶ΧΪ0-1000");
    return ;
  }
  if(m_lStorageMul1<0||m_lStorageMul1>100)
  {
    AfxMessageBox(L"[���Ӯ�ָ���1]���뷶ΧΪ0-100");
    return ;
  }
  if(m_lStorageMul2<0||m_lStorageMul2>100)
  {
    AfxMessageBox(L"[���Ӯ�ָ���2]���뷶ΧΪ0-100");
    return ;
  }
  if((m_lcurrentStorage<0)||(m_lStorageDeduct<0)||
     (m_lStorageMax1<0)||(m_lStorageMax2<0)||(m_lStorageMul1<0)||(m_lStorageMul2<0))
  {
    str=TEXT("��ֵ����Ϊ����������Ч��");
    return;
  }
  else
  {

    CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
    if(pGameFrameEngine!=NULL)
    {
      pGameFrameEngine->SendMessage(IDM_ADMIN_STORAGE,(WPARAM)&FreshStorage,0);
    }
    str=TEXT("����Ѹ��£�");
  }

  //SetWindowText(str);
  SetDlgItemText(IDC_STORAGE_STATIC,str);
}

//���¿��
bool CClientControlItemSinkDlg::UpdateStorage(const void * pBuffer)
{

  const CMD_C_FreshStorage *pResult=(CMD_C_FreshStorage*)pBuffer;

  //���¿��
  m_lStorageStart = pResult->lStorageStart;
  m_lStorageDeduct = pResult->lStorageDeduct;
  m_lcurrentStorage = pResult->lStorageCurrent;
  m_lStorageMax1 = pResult->lStorageMax1;
  m_lStorageMul1 = pResult->lStorageMul1;
  m_lStorageMax2 = pResult->lStorageMax2;
  m_lStorageMul2 = pResult->lStorageMul2;

  //��������ֵ���ؼ���ʾ
  UpdateData(FALSE);

  ////���ñ�����Ϣ
  //if(RQ_SET_STORAGE == pResult->cbReqType)
  //{
  //    SetDlgItemText(IDC_STORAGE_STATIC,L"����޸ĳɹ���");
  //}
  //else if(RQ_REFRESH_STORAGE == pResult->cbReqType)
  //{
  //  SetDlgItemText(IDC_STORAGE_STATIC,L"����Ѹ��£�");
  //}

  return true;
}
//���¿��
void CClientControlItemSinkDlg::OnFreshDeuct()
{
  CString str;
  CMD_C_FreshDeuct FreshDeuct;
  m_lStorageDeduct=(SCORE)GetDlgItemInt(IDC_DEDUCT);
  FreshDeuct.lStorageDeuct=m_lStorageDeduct;
  if(m_lStorageDeduct<0)
  {
    str=TEXT("���˥��ֵΪ����������Ч��");
  }
  else
  {
    CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
    if(pGameFrameEngine!=NULL)
    {
      pGameFrameEngine->SendMessage(IDM_ADMIN_DEUCT,(WPARAM)&FreshDeuct,0);
    }
    str=TEXT("���˥��ֵ�Ѹ��£�");
  }
  SetWindowText(str);
}
//��������
void CClientControlItemSinkDlg::OnBnClickedButtonOk()
{

}

//ȡ���ر�
void CClientControlItemSinkDlg::OnBnClickedButtonCancel()
{
  ShowWindow(SW_HIDE);
}

//���¿���
void  CClientControlItemSinkDlg::UpdateControl(CMD_S_ControlReturns* pControlReturns)
{
  switch(pControlReturns->cbReturnsType)
  {
    case S_CR_FAILURE:
    {
      SetDlgItemText(IDC_STATIC_INFO,TEXT("����ʧ�ܣ�"));
      break;
    }
    case S_CR_UPDATE_SUCCES:
    {
      TCHAR zsText[256] = TEXT("");
      TCHAR zsTextTemp[256] = TEXT("");
      PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
      myprintf(zsText,CountArray(zsText),TEXT("�������ݳɹ���\r\n %s"),zsTextTemp);
      SetDlgItemText(IDC_STATIC_INFO,zsText);
      break;
    }
    case S_CR_SET_SUCCESS:
    {
      TCHAR zsText[256] = TEXT("");
      TCHAR zsTextTemp[256] = TEXT("");
      PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
      myprintf(zsText,CountArray(zsText),TEXT("�������ݳɹ���\r\n %s"),zsTextTemp);
      SetDlgItemText(IDC_STATIC_INFO,zsText);
      break;
    }
    case S_CR_CANCEL_SUCCESS:
    {
      m_lStorageStart=0;
      m_lStorageDeduct=0;
      SetDlgItemText(IDC_STATIC_INFO,TEXT("ȡ�����óɹ���"));
      break;
    }
  }
}

//��Ϣ
void CClientControlItemSinkDlg::PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes)
{
  if(cbArea == 0xff)
  {
    myprintf(pText,cbCount,TEXT("��ʱ�޿��ơ�"));
    return;
  }
  TCHAR szDesc[32] = TEXT("");
  myprintf(pText,cbCount,TEXT("ʤ������"));
  if(cbArea == 0)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("��ʱ�ݡ�40��"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 1)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("�����30��"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 2)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("���ۡ�20��"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 3)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("���ڡ�10��"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 4)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("��ʱ�ݡ�5��"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 5)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("�����5��"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 6)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("���ۡ�5��"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 7)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("���ڡ�5��"));
    lstrcat(pText,szDesc);
  }
  myprintf(szDesc,CountArray(szDesc),TEXT("ִ�д�����%d��"), cbTimes);
  lstrcat(pText,szDesc);
}

void CClientControlItemSinkDlg::ReSetAdminWnd()
{
  ((CButton*)GetDlgItem(IDC_RADIO_BEN_40))->SetCheck(0);
  ((CButton*)GetDlgItem(IDC_RADIO_BAO_30))->SetCheck(0);
  ((CButton*)GetDlgItem(IDC_RADIO_AO_20))->SetCheck(0);
  ((CButton*)GetDlgItem(IDC_RADIO_DA_10))->SetCheck(0);
  ((CButton*)GetDlgItem(IDC_RADIO_BEN_5))->SetCheck(0);
  ((CButton*)GetDlgItem(IDC_RADIO_BAO_5))->SetCheck(0);
  ((CButton*)GetDlgItem(IDC_RADIO_AO_5))->SetCheck(0);
  ((CButton*)GetDlgItem(IDC_RADIO_DA_5))->SetCheck(0);
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(-1);
}


void CClientControlItemSinkDlg::UpdateClistCtrl(LONGLONG  m_lAllPlayBet[GAME_PLAYER][AREA_COUNT],BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair,IClientUserItem* pUserData)
{



}

void CClientControlItemSinkDlg::ClearText()
{

  SetDlgItemText(IDC_STORAGE_STATIC,TEXT(""));
  SetDlgItemText(IDC_STATIC_INFO,TEXT(""));
}
void CClientControlItemSinkDlg::ClearClist()
{
  m_listUserBet.DeleteAllItems();
  m_UserBetArray.RemoveAll();
  for(BYTE cbSubItem=1; cbSubItem <=8; cbSubItem++)
  {
    m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
  }
  SetDlgItemText(IDC_STATIC_INFO,TEXT(""));
  ZeroMemory(m_areAtotalBet,sizeof(m_areAtotalBet));
}

void CClientControlItemSinkDlg::OnStnClickedStaticInfo()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CClientControlItemSinkDlg::OnBnClickedAreaFreshsto()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
  CMD_C_ControlApplication ControlApplication;
  ZeroMemory(&ControlApplication, sizeof(ControlApplication));

  BYTE cbSelectedIndex = 0xff;
  if(((CButton*)GetDlgItem(IDC_RADIO_BEN_40))->GetCheck())
  {
    cbSelectedIndex = 0;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_BAO_30))->GetCheck())
  {
    cbSelectedIndex = 1;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_AO_20))->GetCheck())
  {
    cbSelectedIndex = 2;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_DA_10))->GetCheck())
  {
    cbSelectedIndex = 3;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_BEN_5))->GetCheck())
  {
    cbSelectedIndex = 4;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_BAO_5))->GetCheck())
  {
    cbSelectedIndex = 5;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_AO_5))->GetCheck())
  {
    cbSelectedIndex = 6;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_DA_5))->GetCheck())
  {
    cbSelectedIndex = 7;
  }

  int nSelectTimes = ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel();

  if(cbSelectedIndex != 0xff && nSelectTimes >= 0 && nSelectTimes != 0xffffffff)
  {
    ControlApplication.cbControlAppType = C_CA_SET;
    ControlApplication.cbControlArea = cbSelectedIndex;
    ControlApplication.cbControlTimes = static_cast<BYTE>(nSelectTimes + 1);
    //AfxGetMainWnd()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
    CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
    if(pGameFrameEngine!=NULL)
    {
      pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
    }
  }
  else
  {
    SetDlgItemText(IDC_STATIC_INFO,TEXT("��ѡ���ܿش����Լ��ܿ�����"));
  }

}

void CClientControlItemSinkDlg::SetStorageCurrentValue(LONGLONG lvalue)
{
  m_lcurrentStorage=lvalue;
  UpdateData(FALSE);
}

void CClientControlItemSinkDlg::OnBnClickedQuery()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
  //��ȡ�û�ID
  CString strUserID = TEXT("");
  GetDlgItemText(IDC_INPUT_ID,strUserID);

  //ȥ���ո�
  strUserID.TrimLeft();
  strUserID.TrimRight();

  m_lQueryGameID = StrToInt(strUserID);

  //���¿ؼ�
  UpdateUserBet(false);

}

void CClientControlItemSinkDlg::OnBnClickedBtReset()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
  OnBnClickedButtonReset();
}

void CClientControlItemSinkDlg::OnBnClickedBtCurset()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
  CMD_C_AdminReq adminReq;
  adminReq.cbReqType=C_CA_UPDATE;
  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
  }
}

//������ע
void __cdecl CClientControlItemSinkDlg::UpdateUserBet(bool bReSet)
{
  //����б�
  m_listUserBet.DeleteAllItems();

  if(bReSet)
  {
    m_lQueryGameID = -1;
    SetDlgItemText(IDC_INPUT_ID, TEXT("���������ID"));
    for(BYTE cbSubItem=1; cbSubItem <=8; cbSubItem++)
    {
      m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
    }
    return;
  }

  if(0 == m_UserBetArray.GetCount())
  {
    return;
  }

  LONGLONG lAllBetScore[AREA_COUNT+1];
  ZeroMemory(lAllBetScore, sizeof(lAllBetScore));

  INT nItemIndex = 0;
  for(INT i=0; i<m_UserBetArray.GetCount(); i++)
  {
    //У���û�ID
    if(-1 != m_lQueryGameID && m_UserBetArray[i].dwUserGameID != m_lQueryGameID)
    {
      continue;
    }

    //��������
    m_listUserBet.InsertItem(nItemIndex, TEXT(""));

    CString strInfo;
    strInfo.Format(TEXT("%d"), nItemIndex+1);
    m_listUserBet.SetItemText(nItemIndex, 1, strInfo);

    m_listUserBet.SetItemText(nItemIndex, 2, m_UserBetArray[i].szNickName);

    strInfo.Format(TEXT("%u"), m_UserBetArray[i].dwUserGameID);
    m_listUserBet.SetItemText(nItemIndex, 3, strInfo);

    strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserScore);
    m_listUserBet.SetItemText(nItemIndex, 4, strInfo);

    strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserScoreTotal);
    m_listUserBet.SetItemText(nItemIndex, 5, strInfo);

    INT nSubItem = -1;
    for(BYTE j=0; j<AREA_COUNT; j++)
    {
      switch(j)
      {
        case ID_BEN_CHI_40:
        {
          nSubItem = 5;
          break;
        }
        case ID_BAO_MA_30:
        {
          nSubItem = 6;
          break;
        }
        case ID_OU_DI_20:
        {
          nSubItem = 7;
          break;
        }
        case ID_DA_ZHONG_10:
        {
          nSubItem = 8;
          break;
        }
        case ID_BEN_CHI_5:
        {
          nSubItem = 9;
          break;
        }
        case ID_BAO_MA_5:
        {
          nSubItem = 10;
          break;
        }
        case ID_OU_DI_5:
        {
          nSubItem = 11;
          break;
        }
        case ID_DA_ZHONG_5:
        {
          nSubItem = 12;
          break;
        }
      }
      nSubItem++;
      strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserBet[j]);
      m_listUserBet.SetItemText(nItemIndex, nSubItem, strInfo);

      lAllBetScore[j] += m_UserBetArray[i].lUserBet[j];
    }

    nItemIndex++;
  }

  INT nSubItemAll = -1;
  for(BYTE k=0; k<AREA_COUNT; k++)
  {
    switch(k)
    {
      case ID_BEN_CHI_40:
      {
        nSubItemAll = 1;
        break;
      }
      case ID_BAO_MA_30:
      {
        nSubItemAll = 2;
        break;
      }
      case ID_OU_DI_20:
      {
        nSubItemAll = 3;
        break;
      }
      case ID_DA_ZHONG_10:
      {
        nSubItemAll = 4;
        break;
      }
      case ID_BEN_CHI_5:
      {
        nSubItemAll = 5;
        break;
      }
      case ID_BAO_MA_5:
      {
        nSubItemAll = 6;
        break;
      }
      case ID_OU_DI_5:
      {
        nSubItemAll = 7;
        break;
      }
      case ID_DA_ZHONG_5:
      {
        nSubItemAll = 8;
        break;
      }
    }

    CString strInfoAll;
    strInfoAll.Format(TEXT("%I64d"), lAllBetScore[k]);
    m_listUserBetAll.SetItemText(0, nSubItemAll, strInfoAll);
  }

  return;
}

void CClientControlItemSinkDlg::OnBnClickedTotal()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
  //��ʾȫ��
  m_lQueryGameID = -1;

  SetDlgItemText(IDC_INPUT_ID, TEXT("���������ID"));

  //���¿ؼ�
  UpdateUserBet(false);
}
void CClientControlItemSinkDlg::OnEnSetfocusEditUserId()
{
  SetDlgItemText(IDC_INPUT_ID, TEXT(""));
  return;
}

