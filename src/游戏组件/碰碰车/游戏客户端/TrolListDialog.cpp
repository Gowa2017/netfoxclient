// TrolListDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GameClient.h"
#include "TrolListDialog.h"
#include ".\trollistdialog.h"


// CTrolListDialog �Ի���

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


// CTrolListDialog ��Ϣ�������

void CTrolListDialog::OnBnClickedOk()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������


  //���ҵ�2���
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
  ////��������
  //nItem = m_SkinListCtrl.FindItem( &lvFindInfo );
  if(nItem == -1)
  {

    AfxMessageBox(L"����ҵ��ǳƲ�����");
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
      m_SkinListCtrl.SetItemText(numLine-2, index, strScore);   //���û���
    }

  }

}

BOOL CTrolListDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  �ڴ���Ӷ���ĳ�ʼ��
  m_SkinListCtrl.ModifyStyle(0,LVS_REPORT);
  //ͨ��GetWindowLong����ȡCListCtrl���е���ʽ
  DWORD dwStyle = GetWindowLong(m_SkinListCtrl.m_hWnd, GWL_STYLE);

  //��ԭ����ʽ�Ļ����ϣ����LVS_REPORT��չ��ʽ
  SetWindowLong(m_SkinListCtrl.m_hWnd, GWL_STYLE, dwStyle|LVS_REPORT);

  //��ȡ���е���չ��ʽ
  DWORD dwStyles = m_SkinListCtrl.GetExStyle();
  //ȡ����ѡ����ʽ
  dwStyles &= ~LVS_EX_CHECKBOXES;

  //�������ѡ��ͱ������չ��ʽ
  m_SkinListCtrl.SetExtendedStyle(dwStyles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  m_SkinListCtrl.InsertColumn(0,TEXT("���"));
  m_SkinListCtrl.InsertColumn(1,TEXT("����ǳ�"));
  m_SkinListCtrl.InsertColumn(2,TEXT("���ID"));
  m_SkinListCtrl.InsertColumn(3,TEXT("������"));
  m_SkinListCtrl.InsertColumn(4,TEXT("��1"));
  m_SkinListCtrl.InsertColumn(5,TEXT("��2"));
  m_SkinListCtrl.InsertColumn(6,TEXT("��3"));
  m_SkinListCtrl.InsertColumn(7,TEXT("��4"));
  m_SkinListCtrl.InsertColumn(8,TEXT("��5"));
  m_SkinListCtrl.InsertColumn(9,TEXT("��6"));
  m_SkinListCtrl.InsertColumn(10,TEXT("��7"));
  m_SkinListCtrl.InsertColumn(11,TEXT("��8"));

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
  // �쳣: OCX ����ҳӦ���� FALSE
}

void CTrolListDialog::UpdateClistCtrl(LONGLONG  m_lAllPlayBet[GAME_PLAYER][AREA_COUNT],BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair,IClientUserItem* pUserData)
{
  //��ȡ���
  m_lAllPlayBet[wChair][cbViewIndex-1] += lScoreCount;
  m_pUserData=pUserData;
  //�����Ϣ
  //IClientUserItem* pUserData = GetClientUserItem(wChair);
  int nListIndex = cbViewIndex-1;

  //�������
  LVFINDINFO lvFindInfo;
  ZeroMemory(&lvFindInfo, sizeof(lvFindInfo));
  lvFindInfo.flags =  LVFI_STRING | LVFI_PARAM;
  lvFindInfo.psz = (LPCTSTR)pUserData->GetNickName();

  //���ҵ�2���
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
  ////��������
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
    nItem = m_SkinListCtrl.InsertItem(line, strNum);//������
  }

  CString strScore;
  int index=0;
  for(int i=0; i<8; i++)
  {
    strScore.Format(TEXT("%I64d"), m_lAllPlayBet[wChair][i]);
    index=i+4;
    m_SkinListCtrl.SetItemText(nItem, index, strScore);   //���û���
  }


  strScore.Format(TEXT("%s"),pUserData->GetNickName());
  m_SkinListCtrl.SetItemText(nItem, 1, strScore);   //�����ǳ�

  strScore.Format(TEXT("%d"),pUserData->GetGameID());
  m_SkinListCtrl.SetItemText(nItem, 2, strScore);   //����ID

  strScore.Format(TEXT("%d"),pUserData->GetUserScore());
  m_SkinListCtrl.SetItemText(nItem, 3, strScore);   //���ý��

  //���ÿ��ٲ�ѯ
  numLine=m_SkinListCtrl.GetItemCount();
  if(numLine>=3)
  {
    m_SkinListCtrl.DeleteItem(numLine-1);
    m_SkinListCtrl.DeleteItem(numLine-2);
  }
  CString total;
  total="���ٲ�ѯ";
  ZeroMemory(&lvFindInfo, sizeof(lvFindInfo));
  lvFindInfo.flags = LVFI_STRING;
  lvFindInfo.psz =total;
  nItem=m_SkinListCtrl.FindItem(&lvFindInfo);
  if(nItem == -1)
  {

    CString strNum;
    strNum.Format(L"%d",m_SkinListCtrl.GetItemCount()+1);
    int line=m_SkinListCtrl.GetItemCount();
    nItem = m_SkinListCtrl.InsertItem(line, total);//������
    //nItem = m_SkinListCtrl.InsertItem(m_SkinListCtrl.GetItemCount(), L"��ע�ϼ�");//������
    //m_SkinListCtrl.setItem

  }
  //���������ܽ��
  total="��ע�ϼ�";
  ZeroMemory(&lvFindInfo, sizeof(lvFindInfo));
  lvFindInfo.flags = LVFI_STRING;
  lvFindInfo.psz =total;
  nItem=m_SkinListCtrl.FindItem(&lvFindInfo);
  if(nItem == -1)
  {

    CString strNum;
    strNum.Format(L"%d",m_SkinListCtrl.GetItemCount()+1);
    nItem = m_SkinListCtrl.InsertItem(m_SkinListCtrl.GetItemCount(), total);//������
    //nItem = m_SkinListCtrl.InsertItem(m_SkinListCtrl.GetItemCount(), L"��ע�ϼ�");//������
  }
  m_areAtotalBet[cbViewIndex-1]+=lScoreCount;
  for(int i=0; i<8; i++)
  {
    strScore.Format(TEXT("%I64d"), m_areAtotalBet[i]);
    index=i+4;
    m_SkinListCtrl.SetItemText(nItem, index, strScore);   //��������
  }


}

void CTrolListDialog::ClearClist()
{
  m_SkinListCtrl.DeleteAllItems();
  ZeroMemory(m_areAtotalBet,sizeof(m_areAtotalBet));
}
