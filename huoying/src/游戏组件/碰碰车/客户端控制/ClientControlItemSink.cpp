// ClientControlItemSink.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"
//区域索引
#define ID_BEN_CHI_40         0                 //顺门
#define ID_BAO_MA_30          1                 //左边角
#define ID_OU_DI_20         2                 //桥
#define ID_DA_ZHONG_10          3                 //对门
#define ID_BEN_CHI_5          4                 //倒门
#define ID_BAO_MA_5         5                 //右边角
#define ID_OU_DI_5          6                 //倒门
#define ID_DA_ZHONG_5         7                 //右边角

// CClientControlItemSinkDlg 对话框

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

// CClientControlItemSinkDlg 消息处理程序

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
  IClientControlDlg::OnInitDialog();

  //设置信息
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));
  SetDlgItemText(IDC_STATIC_TIMES,TEXT("控制局数："));
  SetDlgItemText(IDC_STATIC_AREA, TEXT("区域控制(优先于库存控制)："));
  SetDlgItemText(IDC_STATIC_NOTIC,TEXT("控制说明："));


  SetDlgItemText(IDC_RADIO_BEN_40,TEXT("保时捷×40"));
  SetDlgItemText(IDC_RADIO_BAO_30,TEXT("宝马×30"));
  SetDlgItemText(IDC_RADIO_AO_20,TEXT("奔驰×20"));
  SetDlgItemText(IDC_RADIO_DA_10,TEXT("大众×10"));
  SetDlgItemText(IDC_RADIO_BEN_5,TEXT("保时捷×5"));
  SetDlgItemText(IDC_RADIO_BAO_5,TEXT("宝马×5"));
  SetDlgItemText(IDC_RADIO_AO_5,TEXT("奔驰×5"));
  SetDlgItemText(IDC_RADIO_DA_5,TEXT("大众×5"));
  SetDlgItemText(IDC_BUTTON_RESET,TEXT("取消控制"));
  SetDlgItemText(IDC_BUTTON_SYN,TEXT("本局信息"));
  SetDlgItemText(IDC_BUTTON_OK,TEXT("执行"));
  SetDlgItemText(IDC_BUTTON_CANCEL,TEXT("取消"));;
  m_lStorageStart=0;
  m_lStorageDeduct=0;


  // TODO:  在此添加额外的初始化
  m_listUserBet.ModifyStyle(0,LVS_REPORT);
  //通过GetWindowLong来获取CListCtrl已有的样式
  DWORD dwStyle = GetWindowLong(m_listUserBet.m_hWnd, GWL_STYLE);

  //在原有样式的基本上，添加LVS_REPORT扩展样式
  SetWindowLong(m_listUserBet.m_hWnd, GWL_STYLE, dwStyle|LVS_REPORT);

  //获取已有的扩展样式
  DWORD dwStyles = m_listUserBet.GetExStyle();
  //取消复选框样式
  dwStyles &= ~LVS_EX_CHECKBOXES;

  //添加整行选择和表格线扩展样式
  m_listUserBet.SetExtendedStyle(dwStyles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  m_listUserBet.GetHeaderCtrl()->EnableWindow(false);
  INT nColumnCount=0;
  m_listUserBet.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,0);
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("序号"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("玩家昵称"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("玩家ID"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("带入金币"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("总赢输"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("保时捷X40"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("宝马X30"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("奔驰X20"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("大众X10"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("保时捷X5"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("宝马X5"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("奔驰X5"));
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("大众X5"));

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
  m_listUserBetAll.SetItemText(0, 0, TEXT("真实玩家下注统计"));
  for(BYTE cbSubItem=1; cbSubItem <=8; cbSubItem++)
  {
    m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
  }
  SetDlgItemText(IDC_INPUT_ID, TEXT("请输入玩家ID"));
  return TRUE;
}

//设置颜色
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

//取消控制
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

//本局控制
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

//更新库存
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
    AfxMessageBox(L"[库存衰减值]输入范围为0-1000");
    return ;
  }
  if(m_lStorageMul1<0||m_lStorageMul1>100)
  {
    AfxMessageBox(L"[玩家赢分概率1]输入范围为0-100");
    return ;
  }
  if(m_lStorageMul2<0||m_lStorageMul2>100)
  {
    AfxMessageBox(L"[玩家赢分概率2]输入范围为0-100");
    return ;
  }
  if((m_lcurrentStorage<0)||(m_lStorageDeduct<0)||
     (m_lStorageMax1<0)||(m_lStorageMax2<0)||(m_lStorageMul1<0)||(m_lStorageMul2<0))
  {
    str=TEXT("数值不能为负数，不生效！");
    return;
  }
  else
  {

    CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
    if(pGameFrameEngine!=NULL)
    {
      pGameFrameEngine->SendMessage(IDM_ADMIN_STORAGE,(WPARAM)&FreshStorage,0);
    }
    str=TEXT("库存已更新！");
  }

  //SetWindowText(str);
  SetDlgItemText(IDC_STORAGE_STATIC,str);
}

//更新库存
bool CClientControlItemSinkDlg::UpdateStorage(const void * pBuffer)
{

  const CMD_C_FreshStorage *pResult=(CMD_C_FreshStorage*)pBuffer;

  //更新库存
  m_lStorageStart = pResult->lStorageStart;
  m_lStorageDeduct = pResult->lStorageDeduct;
  m_lcurrentStorage = pResult->lStorageCurrent;
  m_lStorageMax1 = pResult->lStorageMax1;
  m_lStorageMul1 = pResult->lStorageMul1;
  m_lStorageMax2 = pResult->lStorageMax2;
  m_lStorageMul2 = pResult->lStorageMul2;

  //拷贝变量值到控件显示
  UpdateData(FALSE);

  ////设置标题消息
  //if(RQ_SET_STORAGE == pResult->cbReqType)
  //{
  //    SetDlgItemText(IDC_STORAGE_STATIC,L"库存修改成功！");
  //}
  //else if(RQ_REFRESH_STORAGE == pResult->cbReqType)
  //{
  //  SetDlgItemText(IDC_STORAGE_STATIC,L"库存已更新！");
  //}

  return true;
}
//更新库存
void CClientControlItemSinkDlg::OnFreshDeuct()
{
  CString str;
  CMD_C_FreshDeuct FreshDeuct;
  m_lStorageDeduct=(SCORE)GetDlgItemInt(IDC_DEDUCT);
  FreshDeuct.lStorageDeuct=m_lStorageDeduct;
  if(m_lStorageDeduct<0)
  {
    str=TEXT("库存衰减值为负数，不生效！");
  }
  else
  {
    CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
    if(pGameFrameEngine!=NULL)
    {
      pGameFrameEngine->SendMessage(IDM_ADMIN_DEUCT,(WPARAM)&FreshDeuct,0);
    }
    str=TEXT("库存衰减值已更新！");
  }
  SetWindowText(str);
}
//开启控制
void CClientControlItemSinkDlg::OnBnClickedButtonOk()
{

}

//取消关闭
void CClientControlItemSinkDlg::OnBnClickedButtonCancel()
{
  ShowWindow(SW_HIDE);
}

//更新控制
void  CClientControlItemSinkDlg::UpdateControl(CMD_S_ControlReturns* pControlReturns)
{
  switch(pControlReturns->cbReturnsType)
  {
    case S_CR_FAILURE:
    {
      SetDlgItemText(IDC_STATIC_INFO,TEXT("操作失败！"));
      break;
    }
    case S_CR_UPDATE_SUCCES:
    {
      TCHAR zsText[256] = TEXT("");
      TCHAR zsTextTemp[256] = TEXT("");
      PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
      myprintf(zsText,CountArray(zsText),TEXT("更新数据成功！\r\n %s"),zsTextTemp);
      SetDlgItemText(IDC_STATIC_INFO,zsText);
      break;
    }
    case S_CR_SET_SUCCESS:
    {
      TCHAR zsText[256] = TEXT("");
      TCHAR zsTextTemp[256] = TEXT("");
      PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
      myprintf(zsText,CountArray(zsText),TEXT("设置数据成功！\r\n %s"),zsTextTemp);
      SetDlgItemText(IDC_STATIC_INFO,zsText);
      break;
    }
    case S_CR_CANCEL_SUCCESS:
    {
      m_lStorageStart=0;
      m_lStorageDeduct=0;
      SetDlgItemText(IDC_STATIC_INFO,TEXT("取消设置成功！"));
      break;
    }
  }
}

//信息
void CClientControlItemSinkDlg::PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes)
{
  if(cbArea == 0xff)
  {
    myprintf(pText,cbCount,TEXT("暂时无控制。"));
    return;
  }
  TCHAR szDesc[32] = TEXT("");
  myprintf(pText,cbCount,TEXT("胜利区域："));
  if(cbArea == 0)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("保时捷×40，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 1)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("宝马×30，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 2)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("奔驰×20，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 3)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("大众×10，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 4)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("保时捷×5，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 5)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("宝马×5，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 6)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("奔驰×5，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 7)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("大众×5，"));
    lstrcat(pText,szDesc);
  }
  myprintf(szDesc,CountArray(szDesc),TEXT("执行次数：%d。"), cbTimes);
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
  // TODO: 在此添加控件通知处理程序代码
}

void CClientControlItemSinkDlg::OnBnClickedAreaFreshsto()
{
  // TODO: 在此添加控件通知处理程序代码
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
    SetDlgItemText(IDC_STATIC_INFO,TEXT("请选择受控次数以及受控区域！"));
  }

}

void CClientControlItemSinkDlg::SetStorageCurrentValue(LONGLONG lvalue)
{
  m_lcurrentStorage=lvalue;
  UpdateData(FALSE);
}

void CClientControlItemSinkDlg::OnBnClickedQuery()
{
  // TODO: 在此添加控件通知处理程序代码
  //读取用户ID
  CString strUserID = TEXT("");
  GetDlgItemText(IDC_INPUT_ID,strUserID);

  //去掉空格
  strUserID.TrimLeft();
  strUserID.TrimRight();

  m_lQueryGameID = StrToInt(strUserID);

  //更新控件
  UpdateUserBet(false);

}

void CClientControlItemSinkDlg::OnBnClickedBtReset()
{
  // TODO: 在此添加控件通知处理程序代码
  OnBnClickedButtonReset();
}

void CClientControlItemSinkDlg::OnBnClickedBtCurset()
{
  // TODO: 在此添加控件通知处理程序代码
  CMD_C_AdminReq adminReq;
  adminReq.cbReqType=C_CA_UPDATE;
  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
  }
}

//更新下注
void __cdecl CClientControlItemSinkDlg::UpdateUserBet(bool bReSet)
{
  //清空列表
  m_listUserBet.DeleteAllItems();

  if(bReSet)
  {
    m_lQueryGameID = -1;
    SetDlgItemText(IDC_INPUT_ID, TEXT("请输入玩家ID"));
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
    //校验用户ID
    if(-1 != m_lQueryGameID && m_UserBetArray[i].dwUserGameID != m_lQueryGameID)
    {
      continue;
    }

    //插入数据
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
  // TODO: 在此添加控件通知处理程序代码
  //显示全部
  m_lQueryGameID = -1;

  SetDlgItemText(IDC_INPUT_ID, TEXT("请输入玩家ID"));

  //更新控件
  UpdateUserBet(false);
}
void CClientControlItemSinkDlg::OnEnSetfocusEditUserId()
{
  SetDlgItemText(IDC_INPUT_ID, TEXT(""));
  return;
}

