// ClientControlItemSink.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"

// CClientControlItemSinkDlg �Ի���

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)
BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
  ON_BN_CLICKED(IDC_BT_RESET,OnReSet)
  ON_BN_CLICKED(IDC_BT_EXCUTE,OnExcute)
  ON_BN_CLICKED(IDC_BT_CURSET,OnRefresh)
  ON_BN_CLICKED(IDC_RADIO_CT_AREA,OnRadioClick)
  ON_BN_CLICKED(IDC_RADIO_CT_BANKER,OnRadioClick)
  ON_BN_CLICKED(IDC_BTN_UPDATE_STORAGE, OnBnClickedBtnUpdateStorage)
  ON_BN_CLICKED(IDC_BTN_USER_BET_QUERY, OnBnClickedBtnUserBetQuery)
  ON_BN_CLICKED(IDC_BTN_USER_BET_ALL, OnBnClickedBtnUserBetAll)
  ON_EN_SETFOCUS(IDC_EDIT_USER_ID, OnEnSetfocusEditUserId)
  ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// CClientControlItemSinkDlg ��Ϣ�������
CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
  : IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
  m_lStorageStart = 0;
  m_lStorageDeduct = 0;
  m_lStorageCurrent = 0;
  m_lStorageMax1 = 0;
  m_lStorageMul1 = 0;
  m_lStorageMax2 = 0;
  m_lStorageMul2 = 0;

  m_UserBetArray.RemoveAll();

  m_lQueryGameID = -1;
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
  IClientControlDlg::DoDataExchange(pDX);

  DDX_Text(pDX,IDC_EDIT_STORAGE_START,m_lStorageStart);
  DDX_Text(pDX,IDC_EDIT_STORAGE_DEDUCT,m_lStorageDeduct);
  DDX_Text(pDX,IDC_EDIT_STORAGE_CURRENT,m_lStorageCurrent);
  DDX_Text(pDX,IDC_EDIT_STORAGE_MAX1,m_lStorageMax1);
  DDX_Text(pDX,IDC_EDIT_STORAGE_MUL1,m_lStorageMul1);
  DDX_Text(pDX,IDC_EDIT_STORAGE_MAX2,m_lStorageMax2);
  DDX_Text(pDX,IDC_EDIT_STORAGE_MUL2,m_lStorageMul2);

  /*DDV_MinMaxLongLong(pDX,m_lStorageStart, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_lStorageDeduct, 0, 1000);
  DDV_MinMaxLongLong(pDX,m_lStorageCurrent, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_lStorageMax1, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_lStorageMul1, 0, 100);
  DDV_MinMaxLongLong(pDX,m_lStorageMax2, 0, LLONG_MAX);
  DDV_MinMaxLongLong(pDX,m_lStorageMul2, 0, 100);*/

  DDX_Control(pDX, IDC_LIST_USER_BET, m_listUserBet);
  DDX_Control(pDX, IDC_LIST_USER_BET_ALL, m_listUserBetAll);

}

//���¿��
bool CClientControlItemSinkDlg::UpdateStorage(const void * pBuffer)
{

  const CMD_S_UpdateStorage*pResult=(CMD_S_UpdateStorage*)pBuffer;

  //���¿��
  m_lStorageStart = pResult->lStorageStart;
  m_lStorageDeduct = pResult->lStorageDeduct;
  m_lStorageCurrent = pResult->lStorageCurrent;
  m_lStorageMax1 = pResult->lStorageMax1;
  m_lStorageMul1 = pResult->lStorageMul1;
  m_lStorageMax2 = pResult->lStorageMax2;
  m_lStorageMul2 = pResult->lStorageMul2;

  //��������ֵ���ؼ���ʾ
  UpdateData(FALSE);

  ////���ñ�����Ϣ
  //if(RQ_SET_STORAGE == pResult->cbReqType)
  //{
  //  SetWindowText(TEXT("����޸ĳɹ���"));
  //}
  //else if(RQ_REFRESH_STORAGE == pResult->cbReqType)
  //{
  //  SetWindowText(TEXT("����Ѹ��£�"));
  //}

  return true;
}

//������ע
void __cdecl CClientControlItemSinkDlg::UpdateUserBet(bool bReSet)
{
  //����б�
  m_listUserBet.DeleteAllItems();

  ////test+
  //for(INT i=0; i<15; i++)
  //{
  //  //��������
  //  m_listUserBet.InsertItem(i, TEXT(""));

  //  CString strInfo;
  //  strInfo.Format(TEXT("%d"), i+1);
  //  m_listUserBet.SetItemText(i, 1, strInfo);
  //}
  ////test-

  if(bReSet)
  {
    m_lQueryGameID = -1;

    SetDlgItemText(IDC_EDIT_USER_ID, TEXT("���������ID"));
    GetDlgItem(IDC_EDIT_STORAGE_START)->SetFocus();

    for(BYTE cbSubItem=3; cbSubItem <=8; cbSubItem++)
    {
      m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
    }

    GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
    GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

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

    strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserStartScore);
    m_listUserBet.SetItemText(nItemIndex, 4, strInfo);

    strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserWinLost);
    m_listUserBet.SetItemText(nItemIndex, 5, strInfo);

    INT nSubItem = -1;
    for(BYTE j=1; j<=AREA_COUNT; j++)
    {
      nSubItem = j + 5;

      strInfo.Format(TEXT("%I64d"), m_UserBetArray[i].lUserBet[j]);
      m_listUserBet.SetItemText(nItemIndex, nSubItem, strInfo);

      lAllBetScore[j] += m_UserBetArray[i].lUserBet[j];
    }

    nItemIndex++;
  }

  INT nSubItemAll = -1;
  for(BYTE k=1; k<=AREA_COUNT; k++)
  {
    nSubItemAll = k + 2;

    CString strInfoAll;
    strInfoAll.Format(TEXT("%I64d"), lAllBetScore[k]);
    m_listUserBetAll.SetItemText(0, nSubItemAll, strInfoAll);
  }

  return;
}

//���¿ؼ�
void CClientControlItemSinkDlg::UpdateControl()
{

  RequestUpdateStorage();
  OnRefresh();

}

//������
bool  CClientControlItemSinkDlg::ReqResult(const void * pBuffer)
{
  const CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
  CString str;
  switch(pResult->cbAckType)
  {
    case ACK_SET_WIN_AREA:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        str=TEXT("��Ӯ���������Ѿ����ܣ�");
        switch(m_cbControlStyle)
        {
          case CS_BET_AREA:
          {
            str=TEXT("ʤ������:");
            BYTE cbIndex=0;
            bool bFlags=false;
            for(cbIndex=0; cbIndex<AREA_COUNT; cbIndex++)
            {
              if(m_bWinArea[cbIndex])
              {
                bFlags=true;
                switch(cbIndex)
                {
                  case 0:
                    str+=TEXT("�� ");
                    break;
                  case 1:
                    str+=TEXT("�� ");
                    break;
                  case 2:
                    str+=TEXT("�� ");
                    break;
                  case 3:
                    str+=TEXT("�� ");
                    break;
                  default:
                    break;
                }
              }
            }
            if(!bFlags)
            {
              str+=TEXT("ׯ��ͨɱ");
            }
            str.AppendFormat(TEXT(",ִ�д���:%d"),m_cbExcuteTimes);
            break;
          }
          case CS_BANKER_WIN:
          {
            str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:Ӯ,ִ�д���:%d"),m_cbExcuteTimes);
            break;
          }
          case CS_BANKER_LOSE:
          {
            str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:��,ִ�д���:%d"),m_cbExcuteTimes);
            break;
          }
          default:
            str=TEXT("�ǿ���״̬");
            break;
        }

      }
      else
      {
        str.Format(TEXT("����ʧ��!"));
        m_cbExcuteTimes=0;
        m_cbControlStyle=0;
        ZeroMemory(m_bWinArea,sizeof(m_bWinArea));
      }
      break;
    }
    case ACK_RESET_CONTROL:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        m_cbControlStyle=0;
        m_cbExcuteTimes=0;
        ZeroMemory(m_bWinArea,sizeof(m_bWinArea));
        ReSetAdminWnd();
        str=TEXT("ȡ������!");
      }
      break;
    }
    case ACK_PRINT_SYN:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        str=TEXT("��ǰ������ͬ��!");

        tagAdminReq*pAdminReq=(tagAdminReq*)pResult->cbExtendData;
        m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
        m_cbControlStyle=pAdminReq->m_cbControlStyle;
        memcpy(m_bWinArea,pAdminReq->m_bWinArea,sizeof(m_bWinArea));
        ReSetAdminWnd();
      }
      else
      {
        str.Format(TEXT("��ǰ����ͬ��ʧ��!"));
      }
      break;
    }

    default:
      break;
  }

  //SetWindowText(str);
  GetDlgItem(IDC_STATIC_CONTROL_INFO)->SetWindowText(str);

  return true;
}

void CClientControlItemSinkDlg::ReSetAdminWnd()
{
  if(m_cbControlStyle!=CS_BANKER_WIN&&m_cbControlStyle!=CS_BANKER_LOSE)
  {
    ((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(0);
    ((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(0);
  }
  else
  {
    ((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->SetCheck(1);
    ((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(0);
    ((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(m_cbControlStyle==CS_BANKER_WIN?1:0);
    ((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(m_cbControlStyle==CS_BANKER_LOSE?1:0);
  }

  if(m_cbControlStyle==CS_BET_AREA)
  {
    ((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->SetCheck(0);
    ((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
  }

  BYTE cbIndex=0;
  for(; cbIndex<CONTROL_AREA; cbIndex++)
  {
    ((CButton*)GetDlgItem(IDC_CHECK_TIAN+cbIndex))->SetCheck(m_bWinArea[cbIndex]?1:0);
  }
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(m_cbExcuteTimes>0?(m_cbExcuteTimes-1):-1);
  OnRadioClick();
}

void CClientControlItemSinkDlg::PrintCurStatus()
{
  CString str;
  switch(m_cbControlStyle)
  {
    case CS_BET_AREA:
    {
      str=TEXT("ʤ������:");
      BYTE cbIndex=0;
      bool bFlags=false;
      for(cbIndex=0; cbIndex<AREA_COUNT; cbIndex++)
      {
        if(m_bWinArea[cbIndex])
        {
          bFlags=true;
          switch(cbIndex)
          {
            case 0:
              str+=TEXT("�� ");
              break;
            case 1:
              str+=TEXT("�� ");
              break;
            case 2:
              str+=TEXT("�� ");
              break;
            case 3:
              str+=TEXT("�� ");
              break;
            default:
              break;
          }
        }
      }
      if(!bFlags)
      {
        str+=TEXT("ׯ��ͨɱ");
      }
      str.AppendFormat(TEXT(",ִ�д���:%d"),m_cbExcuteTimes);
      break;
    }
    case CS_BANKER_WIN:
    {
      str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:Ӯ,ִ�д���:%d"),m_cbExcuteTimes);
      break;
    }
    case CS_BANKER_LOSE:
    {
      str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:��,ִ�д���:%d"),m_cbExcuteTimes);
      break;
    }
    default:
      str=TEXT("�ǿ���״̬");
      break;
  }

  SetWindowText(str);
}

void CClientControlItemSinkDlg::OnRadioClick()
{
  if(((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck()==1)
  {
    GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(TRUE);
    GetDlgItem(IDC_RADIO_WIN)->EnableWindow(TRUE);
    GetDlgItem(IDC_CHECK_TIAN)->EnableWindow(FALSE);
    GetDlgItem(IDC_CHECK_DI)->EnableWindow(FALSE);
    GetDlgItem(IDC_CHECK_XUAN)->EnableWindow(FALSE);
    GetDlgItem(IDC_CHECK_HUANG)->EnableWindow(FALSE);
  }
  else
  {
    if(((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
    {
      GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(FALSE);
      GetDlgItem(IDC_RADIO_WIN)->EnableWindow(FALSE);
      GetDlgItem(IDC_CHECK_TIAN)->EnableWindow(TRUE);
      GetDlgItem(IDC_CHECK_DI)->EnableWindow(TRUE);
      GetDlgItem(IDC_CHECK_XUAN)->EnableWindow(TRUE);
      GetDlgItem(IDC_CHECK_HUANG)->EnableWindow(TRUE);
    }
  }
}

void CClientControlItemSinkDlg::OnReSet()
{
  CMD_C_AdminReq adminReq;
  adminReq.cbReqType=RQ_RESET_CONTROL;

  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
  }
}

void CClientControlItemSinkDlg::OnRefresh()
{
  CMD_C_AdminReq adminReq;
  adminReq.cbReqType=RQ_PRINT_SYN;

  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
  }
}

void CClientControlItemSinkDlg::OnExcute()
{
  BYTE cbIndex=0;
  bool bFlags=false;
  m_cbControlStyle=0;
  m_cbExcuteTimes=0;
  ZeroMemory(m_bWinArea,sizeof(m_bWinArea));

  //����ׯ��
  if(((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck())
  {
    if(((CButton*)GetDlgItem(IDC_RADIO_WIN))->GetCheck())
    {
      m_cbControlStyle=CS_BANKER_WIN;
      bFlags=true;
    }
    else
    {
      if(((CButton*)GetDlgItem(IDC_RADIO_LOSE))->GetCheck())
      {
        m_cbControlStyle=CS_BANKER_LOSE;
        bFlags=true;
      }
    }
  }
  else //��������
  {
    if(((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
    {
      m_cbControlStyle=CS_BET_AREA;
      for(cbIndex=0; cbIndex<CONTROL_AREA; cbIndex++)
      {
        m_bWinArea[cbIndex]=((CButton*)GetDlgItem(IDC_CHECK_TIAN+cbIndex))->GetCheck()?true:false;
      }
      bFlags=true;
    }
  }

  m_cbExcuteTimes=(BYTE)((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel()+1;
  //��ȡִ�д���
  if(m_cbExcuteTimes<=0)
  {
    bFlags=false;
  }


  if(bFlags) //������Ч
  {
    CMD_C_AdminReq adminReq;
    ZeroMemory(&adminReq,sizeof(CMD_C_AdminReq));
    adminReq.cbReqType=RQ_SET_WIN_AREA;
    tagAdminReq*pAdminReq=(tagAdminReq*)adminReq.cbExtendData;
    pAdminReq->m_cbExcuteTimes=m_cbExcuteTimes;
    pAdminReq->m_cbControlStyle=m_cbControlStyle;
    memcpy(pAdminReq->m_bWinArea,m_bWinArea,sizeof(m_bWinArea));
    //AfxGetMainWnd()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);

    CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
    if(pGameFrameEngine!=NULL)
    {
      pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
    }
  }
  else
  {
    AfxMessageBox(TEXT("��ѡ���ܿش����Լ��ܿط�ʽ!"));
    OnRefresh();
  }
}

//��ʼ��
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  //�������
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));

  ((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
  OnRadioClick();

  GetDlgItem(IDC_STATIC_CONTROL_INFO)->SetWindowText(TEXT(""));

  //�����ע
  m_listUserBet.SetExtendedStyle(m_listUserBet.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  m_listUserBet.GetHeaderCtrl()->EnableWindow(false);
  INT nColumnCount=0;
  m_listUserBet.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,0);
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("���"),LVCFMT_CENTER,40);
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("����ǳ�"),LVCFMT_CENTER,100);
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("���ID"),LVCFMT_CENTER,90);
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("������"),LVCFMT_CENTER,80);
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("����Ӯ"),LVCFMT_CENTER,70);
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("��"),LVCFMT_CENTER,70);
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("��"),LVCFMT_CENTER,70);
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("��"),LVCFMT_CENTER,70);
  m_listUserBet.InsertColumn(nColumnCount++,TEXT("��"),LVCFMT_CENTER,70);

  m_listUserBetAll.SetExtendedStyle(m_listUserBetAll.GetExtendedStyle()|LVS_EX_GRIDLINES);
  nColumnCount=0;
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,0);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,230);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,150);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,70);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,70);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,70);
  m_listUserBetAll.InsertColumn(nColumnCount++,TEXT(""),LVCFMT_CENTER,70);
  m_listUserBetAll.InsertItem(0, TEXT(""));
  m_listUserBetAll.SetItemText(0, 1, TEXT("��ʵ�����עͳ��"));
  for(BYTE cbSubItem=3; cbSubItem <=6; cbSubItem++)
  {
    m_listUserBetAll.SetItemText(0, cbSubItem, TEXT("0"));
  }

  SetDlgItemText(IDC_EDIT_USER_ID, TEXT("���������ID"));

  GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
  GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

  return TRUE;

}

void  CClientControlItemSinkDlg::OnAllowControl(bool bEnable)
{
  GetDlgItem(IDC_BT_EXCUTE)->EnableWindow(bEnable);
}

//���¿��
void CClientControlItemSinkDlg::OnBnClickedBtnUpdateStorage()
{
  //��ȡ������
  UpdateData(TRUE); //�ӿؼ���ȡ���ݵ�����

  if(m_lStorageDeduct < 0|| m_lStorageDeduct > 1000)
  {
    AfxMessageBox(TEXT("[���˥��ֵ]���뷶ΧΪ0-1000"));
    return;
  }
  if(m_lStorageCurrent < 0|| m_lStorageCurrent > LLONG_MAX)
  {
    CString str;
    str.Format(TEXT("[��ǰ���ֵ]���뷶ΧΪ0-%I64d"),LLONG_MAX);
    AfxMessageBox(str);
    return;
  }
  if(m_lStorageMax1 < 0|| m_lStorageMax1 > LLONG_MAX)
  {
    CString str;
    str.Format(TEXT("[�������1]���뷶ΧΪ0-%I64d"),LLONG_MAX);
    AfxMessageBox(str);
    return;
  }
  if(m_lStorageMul1 < 0|| m_lStorageMul1 > 100)
  {
    AfxMessageBox(TEXT("[���Ӯ�ָ���1]���뷶ΧΪ0-100"));
    return;
  }
  if(m_lStorageMax2 < 0|| m_lStorageMax2 > LLONG_MAX)
  {
    CString str;
    str.Format(TEXT("[�������2]���뷶ΧΪ0-%I64d"),LLONG_MAX);
    AfxMessageBox(str);
    return;
  }
  if(m_lStorageMul2 < 0|| m_lStorageMul2 > 100)
  {
    AfxMessageBox(TEXT("[���Ӯ�ָ���2]���뷶ΧΪ0-100"));
    return;
  }

  CMD_C_UpdateStorage adminReq;
  ZeroMemory(&adminReq,sizeof(adminReq));
  adminReq.cbReqType=RQ_SET_STORAGE;

  adminReq.lStorageDeduct = m_lStorageDeduct;
  adminReq.lStorageCurrent = m_lStorageCurrent;
  adminReq.lStorageMax1 = m_lStorageMax1;
  adminReq.lStorageMul1 = m_lStorageMul1;
  adminReq.lStorageMax2 = m_lStorageMax2;
  adminReq.lStorageMul2 = m_lStorageMul2;

  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_UPDATE_STORAGE,(WPARAM)&adminReq,0);
  }
}

void CClientControlItemSinkDlg::RequestUpdateStorage()
{
  CMD_C_UpdateStorage adminReq;
  ZeroMemory(&adminReq,sizeof(adminReq));
  adminReq.cbReqType=RQ_REFRESH_STORAGE;

  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_UPDATE_STORAGE,(WPARAM)&adminReq,0);
  }

}

void CClientControlItemSinkDlg::OnBnClickedBtnUserBetQuery()
{
  //��ȡ�û�ID
  CString strUserID = TEXT("");
  GetDlgItemText(IDC_EDIT_USER_ID,strUserID);

  //ȥ���ո�
  strUserID.TrimLeft();
  strUserID.TrimRight();

  m_lQueryGameID = StrToInt(strUserID);

  GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(true);

  //���¿ؼ�
  UpdateUserBet(false);
}

void CClientControlItemSinkDlg::OnBnClickedBtnUserBetAll()
{
  //��ʾȫ��
  m_lQueryGameID = -1;

  SetDlgItemText(IDC_EDIT_USER_ID, TEXT("���������ID"));

  GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(true);
  GetDlgItem(IDC_BTN_USER_BET_ALL)->EnableWindow(false);

  //���¿ؼ�
  UpdateUserBet(false);

  return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CClientControlItemSinkDlg::OnEnSetfocusEditUserId()
{
  SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
  return;
}

HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

  if(pWnd->GetDlgCtrlID() == IDC_STATIC_CONTROL_INFO)
  {
    pDC->SetTextColor(RGB(255,0,0)); //������ɫ
    //pDC->SetBkColor(RGB(0, 0, 255)); //���屳��ɫ
  }

  return hbr;
}
