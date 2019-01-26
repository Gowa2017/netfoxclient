// DlgBank.cpp : implementation file
//

#include "stdafx.h"
#include "DlgBank.h"
#include "resource.h"

#define IDC_OK        500
#define IDC_CANCEL      501
#define IDC_FRESH     502
//��ʱ��I D
#define IDI_CHARMVALUE_UPDATE_VIEW    1               //���½���
#define TIME_CHARMVALUE_UPDATE_VIEW   200               //���½���
void MakeString(CString &strNum,LONGLONG lNumber)
{
  CString static strNumber=TEXT(""), strTmpNumber1,strTmpNumber2;
  strTmpNumber1.Empty();
  strTmpNumber2.Empty();
  strNumber.Empty();
  if(lNumber==0)
  {
    strNumber=TEXT("0");
  }
  int nNumberCount=0;
  LONGLONG lTmpNumber=lNumber;
  if(lNumber<0)
  {
    lNumber=-lNumber;
  }
  bool blfirst = true;
  bool bLongNum = false;
  if(lNumber>=1000)
  {
    bLongNum = true;
  }
  while(lNumber>0)
  {

    strTmpNumber1.Format(TEXT("%I64d"),lNumber%10);
    nNumberCount++;
    strTmpNumber2 = strTmpNumber1+strTmpNumber2;

    if(nNumberCount==3)
    {
      if(blfirst)
      {
        strTmpNumber2 += (TEXT("") +strNumber);
        blfirst = false;
      }
      else
      {
        strTmpNumber2 += (TEXT(",") +strNumber);
      }

      strNumber=strTmpNumber2;
      nNumberCount=0;
      strTmpNumber2=TEXT("");
    }
    lNumber/=10;
  }

  if(strTmpNumber2.IsEmpty()==FALSE)
  {
    if(bLongNum)
    {
      strTmpNumber2 += (TEXT(",") +strNumber);
    }
    else
    {
      strTmpNumber2 += strNumber;
    }

    strNumber=strTmpNumber2;
  }

  if(lTmpNumber<0)
  {
    strNumber=TEXT("-")+strNumber;
  }

  strNum = strNumber ;
}

void MakeStringToNum(CString str,LONGLONG &Num)
{
  CString str1;
  int Pos = 0;
  int len = 0;
  int Start = 0;
  while(1)
  {
    len = str.GetLength();
    Pos = str.Find(_TEXT(","),Pos);
    if(Pos==-1)
    {
      str1+=str;
      break;
    }
    else
    {
      char Tmp[126];
      memset(Tmp,0,126);
      memcpy(Tmp,&str.GetBuffer()[Start],Pos);
      if(Pos+1<=str.GetLength())
      {
        str = &str.GetBuffer()[Pos+1];
        str1+=Tmp;
        Start =  0;
        Pos = 0;
      }
      else
      {
        break;
      }
    }
  }

#ifdef _UNICODE
  _snwscanf(str1.GetBuffer(),str1.GetLength(),_TEXT("%I64d"),&Num);
#else
  _snscanf(str1.GetBuffer(),str1.GetLength(),_TEXT("%I64d"),&Num);
#endif
}

IMPLEMENT_DYNAMIC(CDlgBank, CSkinDialog)
CDlgBank::CDlgBank(CWnd* pParent /*=NULL*/)
  : CSkinDialog(IDD_BANK_STORAGE, pParent)
{
  m_lInCount=0;
  m_lGameGold=0;
  m_lStorageGold=0;
  m_pMeUserData=NULL;
  m_pIClientKernel=NULL;
  m_bBankStorage=true;
  m_blCanStore=false;
  m_blCanGetOnlyFree=false;
  m_ImageFrame.LoadImage(AfxGetInstanceHandle(),TEXT("BANK_FRAME"));
  m_ImageNumber.LoadImage(AfxGetInstanceHandle(),TEXT("BANKNUM"));
}

CDlgBank::~CDlgBank()
{
}

void CDlgBank::DoDataExchange(CDataExchange* pDX)
{
  __super::DoDataExchange(pDX);

  //DDX_Control(pDX, IDOK, m_btOK);
  //DDX_Control(pDX, IDCANCEL, m_btCancel);
  DDX_Control(pDX, IDC_USER_PASSWORD, m_Edit);
  DDX_Control(pDX, STATIC_PASS, m_Static);

  //DDX_Text(pDX, IDC_GAME_GOLD, m_lGameGold);
  //DDX_Text(pDX, IDC_STORAGE_GOLD, m_lStorageGold);
  CString strlGameGold;
  MakeString(strlGameGold,m_lGameGold);
  SetDlgItemText(IDC_GAME_GOLD,strlGameGold);

  MakeString(strlGameGold,m_lStorageGold);
  SetDlgItemText(IDC_STORAGE_GOLD,strlGameGold);
}


BEGIN_MESSAGE_MAP(CDlgBank, CSkinDialog)
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_TIMER()
  ON_WM_CREATE()
  ON_BN_CLICKED(IDC_OK,OnBnClickedOk)
  ON_BN_CLICKED(IDC_CANCEL,OnCancelEvent)
  ON_BN_CLICKED(IDC_CHECK_ALL, OnBnClickedCheck)
  ON_EN_CHANGE(IDC_IN_COUNT, OnEnChangeInCount)
  ON_MESSAGE(WM_CREADED,OnCreateEd)

END_MESSAGE_MAP()


void CDlgBank::OnEnRadioStore()
{
  SetBankerActionType(true);

}

void CDlgBank::OnEnRadioGet()
{

  SetBankerActionType(false);
}

//��ѡ
void CDlgBank::OnBnClickedCheck()
{
  if(IsButtonSelected(IDC_CHECK_ALL))
  {
    if(m_pMeUserData!=NULL)
    {
      //������Ϣ
      m_lGameGold=((m_pMeUserData->GetUserScore()<0)?0:m_pMeUserData->GetUserScore());
      m_lStorageGold=m_pMeUserData->GetUserInsure();
      m_lInCount=(m_bBankStorage)?m_lGameGold:m_lStorageGold;

      CString strlGameGold;
      MakeString(strlGameGold,m_lInCount);
      SetDlgItemText(IDC_IN_COUNT,strlGameGold);
    }
  }
  else
  {
    SetDlgItemText(IDC_IN_COUNT,_TEXT(""));
    GetDlgItem(IDC_IN_COUNT)->SetFocus();
    ((CEdit*)GetDlgItem(IDC_IN_COUNT))->SetSel(0,-1);
  }
}

//�滭����
VOID CDlgBank::DrawNumberString(CDC * pDC, SCORE lScore, INT nXPos, INT nYPos)
{
  //ת������
  TCHAR szControl[128]=TEXT("");
  CString cs;

  MakeString(cs,lScore);
  _sntprintf(szControl,sizeof(szControl),TEXT("%s"),cs);

  //��������
  INT nXDrawPos=nXPos;
  INT nScoreLength=lstrlen(szControl);

  //�滭�ж�
  if(nScoreLength>0L)
  {
    ////������Դ
    //CPngImage ImageNumber;
    //ImageNumber.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("BANKNUM"));

    //��ȡ��С
    CSize SizeNumber;
    SizeNumber.SetSize(m_ImageNumber.GetWidth()/12L,m_ImageNumber.GetHeight());

    //�滭����
    for(INT i=0; i<nScoreLength; i++)
    {
      //�滭����
      if(szControl[i]==TEXT(','))
      {
        m_ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*10L,0L);
      }

      //�滭���
      if(szControl[i]==TEXT('.'))
      {
        m_ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*11L,0L);
      }

      //�滭����
      if(szControl[i]>=TEXT('0')&&szControl[i]<=TEXT('9'))
      {
        m_ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*(szControl[i]-TEXT('0')),0L);
      }

      //����λ��
      nXDrawPos+=SizeNumber.cx;
    }
  }

  return;
}

//��ȡ��ť
void CDlgBank::OnBnClickedOk()
{
  //����ת��
  CString strInCount;
  GetDlgItem(IDC_IN_COUNT)->GetWindowText(strInCount);
  MakeStringToNum(strInCount, m_lInCount);

  //������֤
  if(false==m_bBankStorage && (m_lInCount <= 0 || m_lInCount > m_lStorageGold))
  {
    CString strMessage;
    if(m_lStorageGold>0)
    {
      strMessage.Format(TEXT("���������Ϸ��ֵ������1��%I64d֮��"), m_lStorageGold);
    }
    else
    {
      strMessage.Format(TEXT("��Ĵ洢��Ϸ����ĿΪ0,���ܽ�����ȡ����!"));
    }
    MessageBox(strMessage,TEXT("��ܰ��ʾ"));

    GetDlgItem(IDC_IN_COUNT)->SetFocus();
    ((CEdit*)GetDlgItem(IDC_IN_COUNT))->SetSel(0,-1);
    GetDlgItem(IDC_USER_PASSWORD)->SetWindowText(_TEXT(""));
    return;
  }

  //������֤
  if(true==m_bBankStorage && (m_lInCount <= 0 || m_lInCount > m_lGameGold))
  {
    CString strMessage;
    if(m_lGameGold>0)
    {
      strMessage.Format(TEXT("���������Ϸ��ֵ������1��%I64d֮��"),m_lGameGold);
    }
    else
    {
      strMessage.Format(TEXT("��ĵ�ǰ��Ϸ����ĿΪ0,���ܽ��д洢����!"));
    }
    MessageBox(strMessage,TEXT("��ܰ��ʾ"));

    GetDlgItem(IDC_IN_COUNT)->SetFocus();
    ((CEdit*)GetDlgItem(IDC_IN_COUNT))->SetSel(0,-1);
    GetDlgItem(IDC_USER_PASSWORD)->SetWindowText(_TEXT(""));
    return;
  }

  //������֤
  TCHAR szPassword[LEN_PASSWORD]=TEXT("");
  GetDlgItemText(IDC_USER_PASSWORD,szPassword,CountArray(szPassword));
  if(szPassword[0]==0&&m_blUsingPassWord)
  {
    MessageBox(TEXT("���벻��Ϊ�գ��������������룡"),TEXT("��ܰ��ʾ"));
    GetDlgItem(IDC_USER_PASSWORD)->SetWindowText(_TEXT(""));
    GetDlgItem(IDC_USER_PASSWORD)->SetFocus();
    return;
  }

  if(m_blUsingPassWord)
  {
    TCHAR szTempPassword[LEN_PASSWORD]=TEXT("");
    CopyMemory(szTempPassword,szPassword,sizeof(szTempPassword));
    CWHEncrypt::MD5Encrypt(szTempPassword,szPassword);
  }

  if(false==m_bBankStorage)
  {
    //��������
    CMD_GR_C_TakeScoreRequest TakeScoreRequest;
    ZeroMemory(&TakeScoreRequest,sizeof(TakeScoreRequest));
    TakeScoreRequest.cbActivityGame=TRUE;
    TakeScoreRequest.lTakeScore=m_lInCount;
    CopyMemory(TakeScoreRequest.szInsurePass, szPassword, sizeof(TakeScoreRequest.szInsurePass));

    //��������
    m_pIClientKernel->SendSocketData(MDM_GR_INSURE,SUB_GR_TAKE_SCORE_REQUEST,&TakeScoreRequest,sizeof(TakeScoreRequest));
  }
  else
  {
    //״̬�ж�
    if(US_PLAYING==m_pMeUserData->GetUserStatus())
    {
      MessageBox(TEXT("�������Ϸ���ٴ�"),TEXT("��ܰ��ʾ"));
      return;
    }

    //��������
    CMD_GR_C_SaveScoreRequest SaveScoreRequest;
    SaveScoreRequest.cbActivityGame=TRUE;
    SaveScoreRequest.lSaveScore=m_lInCount;

    //��������
    m_pIClientKernel->SendSocketData(MDM_GR_INSURE,SUB_GR_SAVE_SCORE_REQUEST,&SaveScoreRequest,sizeof(SaveScoreRequest));
  }

  //�������
  GetDlgItem(IDC_USER_PASSWORD)->SetWindowText(_TEXT(""));
  return;
}

//��ʼ������
BOOL CDlgBank::OnInitDialog()
{
  CSkinDialog::OnInitDialog();

  UpdateData(FALSE);

  SetWindowText(TEXT("����"));

  SetBankerActionType(true);

  HideStorage();

  //��ʱ����
  SetTimer(IDI_CHARMVALUE_UPDATE_VIEW,TIME_CHARMVALUE_UPDATE_VIEW,NULL);

  return TRUE;
}

//�滭����
VOID CDlgBank::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
  m_ImageFrame.DrawImage(pDC,30,55);
  DrawNumberString(pDC,m_lGameGold,200,78);
  DrawNumberString(pDC,m_lStorageGold,200,104);
  CRect rcScore(120,80,200,95);
  pDC->DrawText(TEXT("��ǰ��Ϸ�ң�"),rcScore,DT_END_ELLIPSIS | DT_CENTER | DT_WORDBREAK);
  CRect rcBank(120,105,200,120);
  pDC->DrawText(TEXT("������Ϸ�ң�"),rcBank,DT_END_ELLIPSIS | DT_CENTER | DT_WORDBREAK);
  CRect rect(0,315,nWidth,nHeight);
  pDC->DrawText(TEXT("��ܰ��ʾ��������Ϸ���������ѣ�ȡ�����۳�1%��������"),rect,DT_END_ELLIPSIS | DT_CENTER | DT_WORDBREAK);


}

//���½���
void CDlgBank::UpdateView()
{
  CString strinCount;
  GetDlgItemText(IDC_IN_COUNT,strinCount);
  LONGLONG OutNum = 0;
  MakeStringToNum(strinCount,OutNum);

  if(m_OrInCount!= OutNum)
  {
    m_OrInCount = OutNum;
    if(OutNum!=0)
    {
      MakeString(strinCount,OutNum);
      SetDlgItemText(IDC_IN_COUNT,strinCount);
      ((CEdit*)GetDlgItem(IDC_IN_COUNT))->SetSel(strinCount.GetLength(),strinCount.GetLength(),TRUE);
    }

    //������֤
    if(false==m_bBankStorage && (OutNum < 0 || OutNum > m_lStorageGold))
    {
      return;
    }

    //������֤
    if(true==m_bBankStorage && (OutNum < 0 || OutNum > m_lGameGold))
    {
      return;
    }
  }

  UpdateData(TRUE);

  if(m_pMeUserData==NULL)
  {
    return;
  }

  //������Ϣ
  m_lGameGold=((m_pMeUserData->GetUserScore()<0)?0:m_pMeUserData->GetUserScore());
  m_lStorageGold=m_pMeUserData->GetUserInsure();

  //���½���
  RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

  CString strlGameGold;

  MakeString(strlGameGold,m_lGameGold);
  SetDlgItemText(IDC_GAME_GOLD,strlGameGold);

  MakeString(strlGameGold,m_lStorageGold);
  SetDlgItemText(IDC_STORAGE_GOLD,strlGameGold);

  UpdateData(FALSE);
}

//�����Ϣ
void CDlgBank::OnLButtonDown(UINT nFlags, CPoint point)
{
  CSkinDialog::OnLButtonDown(nFlags, point);
}

//�����Ϣ
void CDlgBank::OnMouseMove(UINT nFlags, CPoint point)
{
  CSkinDialog::OnMouseMove(nFlags, point);
}

//������Ϣ
BOOL CDlgBank::PreTranslateMessage(MSG* pMsg)
{
  //������ĸ
  if(GetFocus()==GetDlgItem(IDC_IN_COUNT)&& pMsg->message==WM_CHAR)
  {
    BYTE bMesValue = (BYTE)(pMsg->wParam);
    BYTE bTemp = bMesValue-'0';
    if((bTemp<0 || bTemp>9) && bMesValue!=VK_BACK)
    {
      return TRUE;
    }
  }
  return __super::PreTranslateMessage(pMsg);
}

//ʱ����Ϣ
void CDlgBank::OnTimer(UINT nIDEvent)
{
  //ʱ����Ϣ
  switch(nIDEvent)
  {
    case IDI_CHARMVALUE_UPDATE_VIEW:    //���½���
    {
      //���½���
      UpdateView();
      return;
    }
  }
  __super::OnTimer(nIDEvent);
}

//������Ϣ
void CDlgBank::OnEnChangeInCount()
{
  try
  {
    CString strInCount;
    CString strStorageGold;
    CString strGameGold;
    GetDlgItem(IDC_IN_COUNT)->GetWindowText(strInCount);
    MakeStringToNum(strInCount, m_lInCount);
    MakeString(strStorageGold, m_lStorageGold);
    MakeString(strGameGold, m_lGameGold);

    //������֤
    if(false==m_bBankStorage && (m_lInCount < 0 || m_lInCount > m_lStorageGold || strInCount.GetLength() > strStorageGold.GetLength()))
    {
      m_lInCount = m_lStorageGold;
      MakeString(strInCount,m_lInCount);
      GetDlgItem(IDC_IN_COUNT)->SetWindowText(strInCount);
    }

    //������֤
    if(true==m_bBankStorage && (m_lInCount < 0 || m_lInCount > m_lGameGold || strInCount.GetLength() > strGameGold.GetLength()))
    {
      m_lInCount = m_lGameGold;
      MakeString(strInCount,m_lInCount);
      GetDlgItem(IDC_IN_COUNT)->SetWindowText(strInCount);
    }
  }
  catch(...)
  {
    ASSERT(FALSE);
  }
  return ;
}

//�Ƿ�ѡ��
bool CDlgBank::IsButtonSelected(UINT uButtonID)
{
  return ((CButton *)GetDlgItem(uButtonID))->GetCheck()==BST_CHECKED;
}

int CDlgBank::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(__super::OnCreate(lpCreateStruct)==-1)
  {
    return -1;
  }
  CRect rect(0, 0, 0, 0);
  m_btOK.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rect,this,IDC_OK);
  m_btCancel.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rect,this,IDC_CANCEL);
  m_btFresh.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rect,this,IDC_FRESH);
  m_btOK.SetWindowPos(NULL,110,240,94,32,SWP_NOSIZE);
  m_btCancel.SetWindowPos(NULL,285,240,94,32,SWP_NOSIZE);
  m_btFresh.SetWindowPos(NULL,390,66,64,64,SWP_NOSIZE);
  return 0;
}

afx_msg LRESULT  CDlgBank::OnCreateEd(WPARAM wparam,LPARAM lparam)
{
  return 1;
}

//ѡ�а�ť
void CDlgBank::SetButtonSelected(UINT uButtonID, bool bSelected)
{
  CButton * pButton=(CButton *)GetDlgItem(uButtonID);
  if(bSelected)
  {
    pButton->SetCheck(BST_CHECKED);
  }
  else
  {
    pButton->SetCheck(BST_UNCHECKED);
  }
  return;
}

//������Ϣ
void CDlgBank::SetSendInfo(IClientKernel *pClientKernel,IClientUserItem*pMeUserData)
{
  ASSERT(pClientKernel!=NULL);

  //������Ϣ
  m_pIClientKernel = pClientKernel;
  m_pMeUserData=const_cast<IClientUserItem *>(pMeUserData);
  return;
}

void CDlgBank::SetPostPoint(CPoint Point)
{
  CRect Rect;
  GetWindowRect(&Rect);
  SetWindowPos(NULL,Point.x,Point.y-Rect.bottom+Rect.top,Rect.right-Rect.left,Rect.bottom-Rect.top,/*SWP_NOMOVE|*/SWP_NOZORDER);
}

//������Ϣ
void CDlgBank::SendSocketData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
  ASSERT(m_pIClientKernel!=NULL);

  //������Ϣ
  m_pIClientKernel->SendSocketData(wMainCmdID, wSubCmdID, pBuffer, wDataSize);

  return ;
}

void CDlgBank::HideStorage()
{
  //m_RadioStore.ShowWindow(SW_HIDE);
  //m_RadioGet.ShowWindow(SW_HIDE);
}

void CDlgBank::ShowItem()
{
  HINSTANCE hInstance = AfxGetInstanceHandle();
  m_btFresh.SetButtonImage(IDB_BT_FRESH,hInstance,false,false);
  m_btFresh.EnableWindow(TRUE);
  if(m_bBankStorage)
  {
    GetDlgItem(IDC_IN_COUNT)->EnableWindow(m_blCanStore);
    GetDlgItem(IDC_USER_PASSWORD)->EnableWindow(FALSE);
    GetDlgItem(IDC_USER_PASSWORD)->ShowWindow(SW_HIDE);
    GetDlgItem(STATIC_PASS)->ShowWindow(SW_HIDE);
    m_btOK.SetButtonImage(IDB_SAVEGOLD, hInstance, false,false);
    m_btCancel.SetButtonImage(IDB_CLOSE,hInstance,false,false);
    m_btOK.EnableWindow(m_blCanStore);
    m_btCancel.EnableWindow(TRUE);
  }
  else
  {
    BOOL bShow = TRUE;
    if(m_blCanGetOnlyFree && m_pMeUserData->GetUserStatus() == US_PLAYING)
    {
      bShow = FALSE;
    }
    GetDlgItem(IDC_IN_COUNT)->EnableWindow(TRUE);
    GetDlgItem(IDC_USER_PASSWORD)->EnableWindow(TRUE);
    GetDlgItem(IDC_USER_PASSWORD)->ShowWindow(SW_SHOW);
    GetDlgItem(STATIC_PASS)->ShowWindow(SW_SHOW);
    m_btOK.SetButtonImage(IDB_GETGOLD, hInstance, false,false);
    m_btCancel.SetButtonImage(IDB_CLOSE,hInstance,false,false);
    m_btOK.EnableWindow(bShow);
    m_btCancel.EnableWindow(TRUE);
  }

}

//��������
void CDlgBank::SetBankerActionType(bool bStorage)
{
  //���ñ���
  m_bBankStorage = bStorage;
  m_lInCount = 0;
  GetDlgItem(IDC_IN_COUNT)->SetWindowText(TEXT(""));
  ((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(0);

  //���ñ���
  if(m_bBankStorage)
  {

    //GetDlgItem(IDOK)->SetWindowText(TEXT("���"));
    SetDlgItemText(IDC_USER_PASSWORD,_TEXT(""));
    m_blUsingPassWord = false;
  }
  else
  {
    //GetDlgItem(IDOK)->SetWindowText(TEXT("ȡ��"));
    SetDlgItemText(IDC_USER_PASSWORD,_TEXT(""));
    m_blUsingPassWord = true;
  }

  if(m_blUsingPassWord)
  {
    GetDlgItem(IDC_USER_PASSWORD)->EnableWindow(m_blCanStore);
  }

  ShowItem();

}

void CDlgBank::OnCancelEvent()
{
  DestroyWindow();
}

void CDlgBank::OnCancel()
{
  // TODO: Add your specialized code here and/or call the base class
  DestroyWindow();

  //CSkinDialog::OnCancel();
}
