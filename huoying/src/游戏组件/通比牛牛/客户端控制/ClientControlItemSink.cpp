// ClientControlItemSink.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"

//定时器
#define TIME_USERROSTER             2000  //2000毫秒刷新一次
#define IDI_REQUEST_UPDATE_USERROSTER     111   //用户列表定时器


// CClientControlItemSinkDlg 对话框

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
  ON_BN_CLICKED(IDC_BUTTON_ADD_USERROSTER, OnAddUserRoster)
  ON_BN_CLICKED(IDC_BUTTON_DELETE_USERROSTER, OnDeleteUserRoster)
  ON_BN_CLICKED(IDC_BUTTON_MODIFY_STORAGE, OnModifyStorage)
  ON_NOTIFY(NM_CLICK, IDC_LIST_USERROSTER, &CClientControlItemSinkDlg::OnClickListUserRoster)
  ON_WM_CTLCOLOR()
  ON_WM_TIMER()
END_MESSAGE_MAP()

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
  : IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
  m_lMaxStorage = 0;
  m_wStorageMul = 0;
  m_lStorageCurrent = 0;
  m_lStorageDeduct = 0;
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
  IClientControlDlg::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_BUTTON_ADD_USERROSTER, m_btaddUserRoster);
  DDX_Control(pDX, IDC_BUTTON_DELETE_USERROSTER, m_btdeleteUserRoster);
  DDX_Control(pDX, IDC_BUTTON_MODIFY_STORAGE, m_btModifyStorage);
  DDX_Control(pDX, IDC_EDIT_CURRENT_STORAGE, m_editCurrentStorage);
  DDX_Control(pDX, IDC_EDIT_STORAGE_DEDUCT, m_editStorageDeduct);
  DDX_Control(pDX, IDC_EDIT_STORAGE_MAX, m_editStorageMax);
  DDX_Control(pDX, IDC_EDIT_STORAGE_MUL, m_editStorageMul);
  DDX_Control(pDX, IDC_EDIT_USER_GAMEID, m_editUserGameID);
  DDX_Control(pDX, IDC_EDIT_USER_SCORELIMIT, m_editUserScoreLimit);
  DDX_Control(pDX, IDC_LIST_USERROSTER, m_listUserRoster);
}

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
  CSkinDialog::OnInitDialog();
  SetWindowText(TEXT("控制管理器"));

  SetDlgItemText(IDC_BUTTON_ADD_USERROSTER, TEXT("增加名单"));
  SetDlgItemText(IDC_BUTTON_DELETE_USERROSTER, TEXT("删除名单"));
  SetDlgItemText(IDC_BUTTON_MODIFY_STORAGE, TEXT("修改库存"));

  //玩家下注
  m_listUserRoster.SetExtendedStyle(m_listUserRoster.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  m_listUserRoster.GetHeaderCtrl()->EnableWindow(false);
  INT nColumnCount = 0;
  m_listUserRoster.InsertColumn(nColumnCount++, TEXT("玩家GameID"), LVCFMT_CENTER,100);
  m_listUserRoster.InsertColumn(nColumnCount++, TEXT("昵称"), LVCFMT_CENTER,150);
  m_listUserRoster.InsertColumn(nColumnCount++, TEXT("限制分数"), LVCFMT_CENTER,140);
  m_listUserRoster.InsertColumn(nColumnCount++, TEXT("赢或输"), LVCFMT_CENTER,50);
  m_listUserRoster.InsertColumn(nColumnCount++, TEXT("输赢分数"), LVCFMT_CENTER,140);

  //限制范围
  m_editCurrentStorage.LimitText(11);
  m_editStorageDeduct.LimitText(3);
  m_editStorageMax.LimitText(11);
  m_editStorageMul.LimitText(2);
  m_editUserGameID.LimitText(9);
  m_editUserScoreLimit.LimitText(11);

  return TRUE;
}

BOOL CClientControlItemSinkDlg::PreTranslateMessage(MSG* pMsg)
{
  if(pMsg->message == WM_KEYDOWN)
  {
    if(pMsg->wParam == VK_ESCAPE)
    {
      return true;
    }
  }
  return false;
}

//设置起始库存
void CClientControlItemSinkDlg::SetStartStorage(SCORE lStorageScore)
{
  CString strBuf;
  strBuf.Format(TEXT("%d"), lStorageScore);
  GetDlgItem(IDC_STATIC_START_STORAGE)->SetWindowText(strBuf);
}

//设置库存信息
void CClientControlItemSinkDlg::SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo)
{
  CString strBuf;
  //当前库存
  strBuf.Format(TEXT("%I64d"), pStorageInfo->lCurrentStorage);
  GetDlgItem(IDC_EDIT_CURRENT_STORAGE)->SetWindowText(strBuf);
  //当前衰减
  strBuf.Format(TEXT("%I64d"), pStorageInfo->lCurrentDeduct);
  GetDlgItem(IDC_EDIT_STORAGE_DEDUCT)->SetWindowText(strBuf);
  //库存上限
  strBuf.Format(TEXT("%I64d"), pStorageInfo->lMaxStorage);
  GetDlgItem(IDC_EDIT_STORAGE_MAX)->SetWindowText(strBuf);
  //赢分概率
  strBuf.Format(TEXT("%d"), pStorageInfo->wStorageMul);
  GetDlgItem(IDC_EDIT_STORAGE_MUL)->SetWindowText(strBuf);


  //保存数值
  m_lStorageCurrent = pStorageInfo->lCurrentStorage;
  m_lStorageDeduct = pStorageInfo->lCurrentDeduct;
  m_lMaxStorage = pStorageInfo->lMaxStorage;
  m_wStorageMul = pStorageInfo->wStorageMul;
}

void CClientControlItemSinkDlg::OnCancel()
{
  ShowWindow(SW_HIDE);
}

bool CClientControlItemSinkDlg::ReqResultAddUserRoster(CMD_S_AddUserRoster_Result * Roster_Result)
{
  CString strResult;
  if(Roster_Result->bSucceed == false)
  {
    strResult.Format(TEXT("用户GAMEID为[%d]增加名单失败"), Roster_Result->dwGameID);
    GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(strResult);
    return true;
  }

  ASSERT(Roster_Result->bSucceed);

  CString strroster = (Roster_Result->userroster == WHITE_ROSTER ? TEXT("白名单") : TEXT("黑名单"));
  strResult.Format(TEXT("用户GAMEID为[%d]增加[%s]成功"), Roster_Result->dwGameID, strroster);
  GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(strResult);


  ROOMUSERINFO userinfo;
  ZeroMemory(&userinfo, sizeof(userinfo));
  userinfo.dwGameID = Roster_Result->dwGameID;
  userinfo.lGameScore = Roster_Result->lGameScore;
  userinfo.lScoreLimit = Roster_Result->lScoreLimit;
  CopyMemory(userinfo.szNickName, Roster_Result->szNickName, sizeof(userinfo.szNickName));
  userinfo.userroster = Roster_Result->userroster;

  m_userRosterMap.SetAt(Roster_Result->dwUserID, userinfo);

  UpdateUserRosterListCtrl(NULL);

  return true;
}

bool CClientControlItemSinkDlg::ReqResultDeleteUserRoster(CMD_S_DeleteUserRoster_Result * Roster_Result)
{
  CString strResult;
  if(Roster_Result->bFind == false)
  {
    strResult.Format(TEXT("用户GAMEID为[%d]不存在"), Roster_Result->dwGameID);
    GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(strResult);
    return true;
  }

  ASSERT(Roster_Result->bFind);

  if(Roster_Result->bExistRoster == false)
  {
    strResult.Format(TEXT("用户GAMEID为[%d]没有设置名单"), Roster_Result->dwGameID);
    GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(strResult);
    return true;
  }

  ASSERT(Roster_Result->bExistRoster);

  CString strroster = (Roster_Result->beforeuserroster == WHITE_ROSTER ? TEXT("白名单") : TEXT("黑名单"));
  strResult.Format(TEXT("用户GAMEID为[%d]删除[%s]成功"), Roster_Result->dwGameID, strroster);
  GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(strResult);

  //
  m_userRosterMap.RemoveKey(Roster_Result->dwUserID);

  UpdateUserRosterListCtrl(NULL);

  return true;
}

void CClientControlItemSinkDlg::SetCheatRight()
{
  //设置定时器
  SetTimer(IDI_REQUEST_UPDATE_USERROSTER, TIME_USERROSTER, NULL);
}

//更新用户名单
void CClientControlItemSinkDlg::UpdateUserRosterListCtrl(CMD_S_UpdateUserRoster *pUpdateUserRoster)
{
  if(pUpdateUserRoster != NULL)
  {
    m_userRosterMap.SetAt(pUpdateUserRoster->dwUserID, pUpdateUserRoster->roomUserInfo);
  }

  //清空用户名单
  m_listUserRoster.DeleteAllItems();

  //遍历映射
  POSITION ptHead = m_userRosterMap.GetStartPosition();
  DWORD dwUserID = 0;
  ROOMUSERINFO KeyUserInfo;
  ZeroMemory(&KeyUserInfo, sizeof(KeyUserInfo));
  INT nItemIndex = 0;

  while(ptHead)
  {
    m_userRosterMap.GetNextAssoc(ptHead, dwUserID, KeyUserInfo);

    ASSERT(KeyUserInfo.userroster != INVALID_ROSTER);

    //插入数据
    m_listUserRoster.InsertItem(nItemIndex, TEXT(""));

    CString strInfo;
    strInfo.Format(TEXT("%d"), KeyUserInfo.dwGameID);
    m_listUserRoster.SetItemText(nItemIndex, 0, strInfo);

    m_listUserRoster.SetItemText(nItemIndex, 1, KeyUserInfo.szNickName);

    strInfo.Format(TEXT("%I64d"), KeyUserInfo.lScoreLimit);
    m_listUserRoster.SetItemText(nItemIndex, 2, strInfo);

    m_listUserRoster.SetItemText(nItemIndex, 3, (KeyUserInfo.userroster == WHITE_ROSTER ? TEXT("赢") : TEXT("输")));

    strInfo.Format(TEXT("%I64d"), KeyUserInfo.lGameScore);
    m_listUserRoster.SetItemText(nItemIndex, 4, strInfo);

    nItemIndex++;
  }
}

//移除键值
void CClientControlItemSinkDlg::RemoveKeyUserRoster(CMD_S_RemoveKeyUserRoster *pRemoveKeyUserRoster)
{
  m_userRosterMap.RemoveKey(pRemoveKeyUserRoster->dwUserID);

  UpdateUserRosterListCtrl(NULL);

  CString strRoster = (pRemoveKeyUserRoster->userroster == WHITE_ROSTER ? TEXT("白名单") : TEXT("黑名单"));
  CString strInfo;
  strInfo.Format(TEXT("用户GAMEID为[%d]达到限制分数\n[%I64d],将移除[%s]"), pRemoveKeyUserRoster->dwGameID, pRemoveKeyUserRoster->lScoreLimit, strRoster);

  GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(strInfo);
}

//重复用户名单
void CClientControlItemSinkDlg::DuplicateUserRoster(CMD_S_DuplicateUserRoster *pDuplicateUserRoster)
{
  CString strRoster = (pDuplicateUserRoster->userroster == WHITE_ROSTER ? TEXT("白名单") : TEXT("黑名单"));
  CString strInfo;
  strInfo.Format(TEXT("用户GAMEID为[%d]已经是[%s]\n,需先删除原有名单才可重新设置新名单"), pDuplicateUserRoster->dwGameID, strRoster);
  GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(strInfo);

  CString strScoreLimit;
  strScoreLimit.Format(SCORE_STRING, pDuplicateUserRoster->lScoreLimit);
  SetDlgItemText(IDC_EDIT_USER_SCORELIMIT, strScoreLimit);
}

HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

  if(pWnd->GetDlgCtrlID() == IDC_STATIC_STORAGEINFO || pWnd->GetDlgCtrlID() == IDC_STATIC_START_STORAGE
     || pWnd->GetDlgCtrlID() == IDC_STATIC_USERROSTER_INFO)
  {
    pDC->SetTextColor(RGB(255, 10, 10));
  }
  return hbr;
}

void CClientControlItemSinkDlg::OnAddUserRoster()
{
  //获取数据
  CString strUserGameID;
  GetDlgItemText(IDC_EDIT_USER_GAMEID, strUserGameID);
  CString strScoreLimit;
  GetDlgItemText(IDC_EDIT_USER_SCORELIMIT, strScoreLimit);

  //校验是否为空值
  if(strUserGameID == TEXT("") || strScoreLimit == TEXT(""))
  {
    GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(TEXT("您输入空值，请重新设置！"));
    return;
  }

  DWORD dwUserGameID = (DWORD)GetDlgItemInt(IDC_EDIT_USER_GAMEID);
  SCORE lScoreLimit = (SCORE)GetDlgItemInt(IDC_EDIT_USER_SCORELIMIT);
  bool bWin = (((CButton*)GetDlgItem(IDC_CHECK_WINORLOST))->GetCheck() == BST_CHECKED ? true : false);

  if(dwUserGameID <= 0)
  {
    GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(TEXT("用户GameID不能小于或等于0，请重设"));
    return;
  }

  if(lScoreLimit <= 0)
  {
    GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(TEXT("用户限制分数不能小于或等于0，请重设"));
    return;
  }

  CMD_C_Request_Add_Userroster Userroster;
  ZeroMemory(&Userroster, sizeof(Userroster));
  Userroster.dwUserGameID = dwUserGameID;
  Userroster.lScoreLimit = lScoreLimit;
  Userroster.bWin = bWin;

  CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
  if(pGameFrameEngine != NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_REQUEST_ADD_USERROSTER, (WPARAM)&Userroster, 0);
  }
}

void CClientControlItemSinkDlg::OnDeleteUserRoster()
{
  //获取数据
  CString strUserGameID;
  GetDlgItemText(IDC_EDIT_USER_GAMEID, strUserGameID);

  //校验是否为空值
  if(strUserGameID == TEXT(""))
  {
    GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(TEXT("您输入空值，请重新设置！"));
    return;
  }

  DWORD dwUserGameID = (DWORD)GetDlgItemInt(IDC_EDIT_USER_GAMEID);

  if(dwUserGameID <= 0)
  {
    GetDlgItem(IDC_STATIC_USERROSTER_INFO)->SetWindowText(TEXT("用户GameID不能小于或等于0，请重设"));
    return;
  }

  CMD_C_Request_Delete_Userroster Userroster;
  ZeroMemory(&Userroster, sizeof(Userroster));
  Userroster.dwUserGameID = dwUserGameID;

  CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
  if(pGameFrameEngine != NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_REQUEST_DELETE_USERROSTER, (WPARAM)&Userroster, 0);
  }
}

void CClientControlItemSinkDlg::OnModifyStorage()
{
  CMD_C_ModifyStorage modifyStorage;
  ZeroMemory(&modifyStorage,sizeof(modifyStorage));

  CString strStorageCurrent;
  GetDlgItemText(IDC_EDIT_CURRENT_STORAGE, strStorageCurrent);
  CString strStorageDudect;
  GetDlgItemText(IDC_EDIT_STORAGE_DEDUCT, strStorageDudect);
  CString strMaxStorage;
  GetDlgItemText(IDC_EDIT_STORAGE_MAX, strMaxStorage);
  CString strMul;
  GetDlgItemText(IDC_EDIT_STORAGE_MUL, strMul);

  //校验是否为空值
  if(strMaxStorage == TEXT("") || strMul == TEXT("")
     || strStorageCurrent == TEXT("") || strStorageDudect == TEXT(""))
  {
    GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("您输入空值，请重新设置！"));

    //还原原来值
    CString strBuf;
    //当前库存
    strBuf.Format(TEXT("%I64d"), m_lStorageCurrent);
    GetDlgItem(IDC_EDIT_CURRENT_STORAGE)->SetWindowText(strBuf);
    //当前衰减
    strBuf.Format(TEXT("%I64d"), m_lStorageDeduct);
    GetDlgItem(IDC_EDIT_STORAGE_DEDUCT)->SetWindowText(strBuf);
    //库存封顶值
    strBuf.Format(TEXT("%I64d"), m_lMaxStorage);
    GetDlgItem(IDC_EDIT_STORAGE_MAX)->SetWindowText(strBuf);
    //赢分概率
    strBuf.Format(TEXT("%d"), m_wStorageMul);
    GetDlgItem(IDC_EDIT_STORAGE_MUL)->SetWindowText(strBuf);

    return;
  }

  strStorageCurrent.TrimLeft();
  strStorageCurrent.TrimRight();
  LPTSTR lpsz = new TCHAR[strStorageCurrent.GetLength() + 1];
  _tcscpy(lpsz, strStorageCurrent);
  modifyStorage.lStorageCurrent = _ttoi64(lpsz);
  modifyStorage.lStorageDeduct = (SCORE)GetDlgItemInt(IDC_EDIT_STORAGE_DEDUCT);
  delete lpsz;

  //校验合法性
  if(!(modifyStorage.lStorageDeduct >= 0 && modifyStorage.lStorageDeduct < 1000))
  {
    GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("库存衰减范围0-1000"));
    return;
  }

  strMaxStorage.TrimLeft();
  strMaxStorage.TrimRight();
  LPTSTR lpsz1 = new TCHAR[strMaxStorage.GetLength() + 1];
  _tcscpy(lpsz1, strMaxStorage);
  modifyStorage.lMaxStorage = _ttoi64(lpsz1);
  modifyStorage.wStorageMul = GetDlgItemInt(IDC_EDIT_STORAGE_MUL);
  delete lpsz1;

  //校验合法性
  if(modifyStorage.wStorageMul < 0 || modifyStorage.wStorageMul > 100)
  {
    GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("赢分概率范围0-100"));
    return;
  }

  CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
  if(pGameFrameEngine != NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_MODIFY_STORAGE, (WPARAM)&modifyStorage, 0);
  }

  m_lStorageCurrent = modifyStorage.lStorageCurrent;
  m_lStorageDeduct = modifyStorage.lStorageDeduct;
  m_lMaxStorage = modifyStorage.lMaxStorage;
  m_wStorageMul = modifyStorage.wStorageMul;

  GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("库存信息已修改！"));
}

void CClientControlItemSinkDlg::OnTimer(UINT_PTR nIDEvent)
{
  switch(nIDEvent)
  {
    case IDI_REQUEST_UPDATE_USERROSTER:
    {
      CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
      if(pGameFrameEngine != NULL)
      {
        pGameFrameEngine->SendMessage(IDM_REQUEST_UPDATE_USERROSTER, 0, 0);
      }

      return;
    }
  }
  __super::OnTimer(nIDEvent);
}

void CClientControlItemSinkDlg::OnClickListUserRoster(NMHDR* pNMHDR, LRESULT* pResult)
{
  //获取选中行号
  int nLineIndex = m_listUserRoster.GetSelectionMark();
  CString strUserGameID = m_listUserRoster.GetItemText(nLineIndex, 0);
  SetDlgItemText(IDC_EDIT_USER_GAMEID, strUserGameID);

  SetDlgItemText(IDC_STATIC_USERROSTER_INFO, TEXT(""));
}

