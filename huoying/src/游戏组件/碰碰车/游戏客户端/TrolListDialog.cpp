// TrolListDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "GameClient.h"
#include "TrolListDialog.h"
#include ".\trollistdialog.h"


// CTrolListDialog 对话框

IMPLEMENT_DYNAMIC(CTrolListDialog, CDialog)
CTrolListDialog::CTrolListDialog(CWnd* pParent /*=NULL*/)
  : CDialog(CTrolListDialog::IDD, pParent)
{
  ZeroMemory(m_areAtotalBet,sizeof(m_areAtotalBet));
  m_pUserData=NULL;
}

CTrolListDialog::~CTrolListDialog()
{
}

void CTrolListDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST1, m_SkinListCtrl);
  DDX_Control(pDX, IDC_EDIT1, m_cedit);
}


BEGIN_MESSAGE_MAP(CTrolListDialog, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CTrolListDialog 消息处理程序

void CTrolListDialog::OnBnClickedOk()
{
  // TODO: 在此添加控件通知处理程序代码


  //查找第2项的
  int numLine = 0;
  CString strlist;
  m_cedit.GetWindowText(strlist);
  numLine = m_SkinListCtrl.GetItemCount();
  int nItem = -1;
  for(int i = 0; i< numLine; i++)
  {
    if(strlist.Compare(m_SkinListCtrl.GetItemText(i,1)) == 0)
    {
      nItem=i;
      break;
    }

  }
  ////查找子项
  //nItem = m_SkinListCtrl.FindItem( &lvFindInfo );
  if(nItem == -1)
  {

    AfxMessageBox(L"你查找的昵称不存在");
  }
  else
  {
    CString strScore;
    int index=0;
    m_SkinListCtrl.SetItemText(numLine-2, 1,  m_SkinListCtrl.GetItemText(nItem,1));
    for(int i=0; i<8; i++)
    {
      index=i+4;
      strScore.Format(TEXT("%s"), m_SkinListCtrl.GetItemText(nItem,index));
      m_SkinListCtrl.SetItemText(numLine-2, index, strScore);   //设置积分
    }

  }

}

BOOL CTrolListDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  在此添加额外的初始化
  m_SkinListCtrl.ModifyStyle(0,LVS_REPORT);
  //通过GetWindowLong来获取CListCtrl已有的样式
  DWORD dwStyle = GetWindowLong(m_SkinListCtrl.m_hWnd, GWL_STYLE);

  //在原有样式的基本上，添加LVS_REPORT扩展样式
  SetWindowLong(m_SkinListCtrl.m_hWnd, GWL_STYLE, dwStyle|LVS_REPORT);

  //获取已有的扩展样式
  DWORD dwStyles = m_SkinListCtrl.GetExStyle();
  //取消复选框样式
  dwStyles &= ~LVS_EX_CHECKBOXES;

  //添加整行选择和表格线扩展样式
  m_SkinListCtrl.SetExtendedStyle(dwStyles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  m_SkinListCtrl.InsertColumn(0,TEXT("序号"));
  m_SkinListCtrl.InsertColumn(1,TEXT("玩家昵称"));
  m_SkinListCtrl.InsertColumn(2,TEXT("玩家ID"));
  m_SkinListCtrl.InsertColumn(3,TEXT("带入金币"));
  m_SkinListCtrl.InsertColumn(4,TEXT("区1"));
  m_SkinListCtrl.InsertColumn(5,TEXT("区2"));
  m_SkinListCtrl.InsertColumn(6,TEXT("区3"));
  m_SkinListCtrl.InsertColumn(7,TEXT("区4"));
  m_SkinListCtrl.InsertColumn(8,TEXT("区5"));
  m_SkinListCtrl.InsertColumn(9,TEXT("区6"));
  m_SkinListCtrl.InsertColumn(10,TEXT("区7"));
  m_SkinListCtrl.InsertColumn(11,TEXT("区8"));

  CRect rcList;
  m_SkinListCtrl.GetWindowRect(rcList);

  int widthCol=rcList.right-rcList.left;

  m_SkinListCtrl.SetColumnWidth(0, widthCol/12);
  m_SkinListCtrl.SetColumnWidth(1, widthCol/8);
  m_SkinListCtrl.SetColumnWidth(2, widthCol/12);
  m_SkinListCtrl.SetColumnWidth(3, widthCol/12);
  m_SkinListCtrl.SetColumnWidth(4, widthCol/12);
  m_SkinListCtrl.SetColumnWidth(5, widthCol/12);
  m_SkinListCtrl.SetColumnWidth(6, widthCol/12);
  m_SkinListCtrl.SetColumnWidth(7, widthCol/12);
  m_SkinListCtrl.SetColumnWidth(8, widthCol/12);
  m_SkinListCtrl.SetColumnWidth(9, widthCol/12);
  m_SkinListCtrl.SetColumnWidth(10, widthCol/12);
  m_SkinListCtrl.SetColumnWidth(11, widthCol/12);

  return TRUE;  // return TRUE unless you set the focus to a control
  // 异常: OCX 属性页应返回 FALSE
}

void CTrolListDialog::UpdateClistCtrl(LONGLONG  m_lAllPlayBet[GAME_PLAYER][AREA_COUNT],BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair,IClientUserItem* pUserData)
{
  //获取金币
  m_lAllPlayBet[wChair][cbViewIndex-1] += lScoreCount;
  m_pUserData=pUserData;
  //玩家信息
  //IClientUserItem* pUserData = GetClientUserItem(wChair);
  int nListIndex = cbViewIndex-1;

  //构造变量
  LVFINDINFO lvFindInfo;
  ZeroMemory(&lvFindInfo, sizeof(lvFindInfo));
  lvFindInfo.flags =  LVFI_STRING | LVFI_PARAM;
  lvFindInfo.psz = (LPCTSTR)pUserData->GetNickName();

  //查找第2项的
  int numLine = 0;
  CString strlist=lvFindInfo.psz ;
  numLine = m_SkinListCtrl.GetItemCount();
  int nItem = -1;
  for(int i = 0; i< numLine; i++)
  {
    if(strlist.Compare(m_SkinListCtrl.GetItemText(i,1)) == 0)
    {
      nItem=i;
      break;
    }

  }
  ////查找子项
  //nItem = m_SkinListCtrl.FindItem( &lvFindInfo );
  if(nItem == -1)
  {

    int line=m_SkinListCtrl.GetItemCount();
    if(line>=3)
    {
      line-=2;
    }
    CString strNum;
    strNum.Format(L"%d",line);
    nItem = m_SkinListCtrl.InsertItem(line, strNum);//插入行
  }

  CString strScore;
  int index=0;
  for(int i=0; i<8; i++)
  {
    strScore.Format(TEXT("%I64d"), m_lAllPlayBet[wChair][i]);
    index=i+4;
    m_SkinListCtrl.SetItemText(nItem, index, strScore);   //设置积分
  }


  strScore.Format(TEXT("%s"),pUserData->GetNickName());
  m_SkinListCtrl.SetItemText(nItem, 1, strScore);   //设置昵称

  strScore.Format(TEXT("%d"),pUserData->GetGameID());
  m_SkinListCtrl.SetItemText(nItem, 2, strScore);   //设置ID

  strScore.Format(TEXT("%d"),pUserData->GetUserScore());
  m_SkinListCtrl.SetItemText(nItem, 3, strScore);   //设置金币

  //设置快速查询
  numLine=m_SkinListCtrl.GetItemCount();
  if(numLine>=3)
  {
    m_SkinListCtrl.DeleteItem(numLine-1);
    m_SkinListCtrl.DeleteItem(numLine-2);
  }
  CString total;
  total="快速查询";
  ZeroMemory(&lvFindInfo, sizeof(lvFindInfo));
  lvFindInfo.flags = LVFI_STRING;
  lvFindInfo.psz =total;
  nItem=m_SkinListCtrl.FindItem(&lvFindInfo);
  if(nItem == -1)
  {

    CString strNum;
    strNum.Format(L"%d",m_SkinListCtrl.GetItemCount()+1);
    int line=m_SkinListCtrl.GetItemCount();
    nItem = m_SkinListCtrl.InsertItem(line, total);//插入行
    //nItem = m_SkinListCtrl.InsertItem(m_SkinListCtrl.GetItemCount(), L"下注合计");//插入行
    //m_SkinListCtrl.setItem

  }
  //设置区域总金额
  total="下注合计";
  ZeroMemory(&lvFindInfo, sizeof(lvFindInfo));
  lvFindInfo.flags = LVFI_STRING;
  lvFindInfo.psz =total;
  nItem=m_SkinListCtrl.FindItem(&lvFindInfo);
  if(nItem == -1)
  {

    CString strNum;
    strNum.Format(L"%d",m_SkinListCtrl.GetItemCount()+1);
    nItem = m_SkinListCtrl.InsertItem(m_SkinListCtrl.GetItemCount(), total);//插入行
    //nItem = m_SkinListCtrl.InsertItem(m_SkinListCtrl.GetItemCount(), L"下注合计");//插入行
  }
  m_areAtotalBet[cbViewIndex-1]+=lScoreCount;
  for(int i=0; i<8; i++)
  {
    strScore.Format(TEXT("%I64d"), m_areAtotalBet[i]);
    index=i+4;
    m_SkinListCtrl.SetItemText(nItem, index, strScore);   //设置数据
  }


}

void CTrolListDialog::ClearClist()
{
  m_SkinListCtrl.DeleteAllItems();
  ZeroMemory(m_areAtotalBet,sizeof(m_areAtotalBet));
}
