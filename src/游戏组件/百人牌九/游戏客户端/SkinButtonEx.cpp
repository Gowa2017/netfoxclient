#include "StdAfx.h"
#include "Resource.h"
#include "SkinButtonEx.h"

//���캯��
CSkinButtonEx::CSkinButtonEx(void)
{
}

//��������
CSkinButtonEx::~CSkinButtonEx(void)
{
}

//�󶨺���
VOID CSkinButtonEx::PreSubclassWindow()
{
  return ;
}

//�滭����
VOID CSkinButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  //��ȡλ��
  CRect rcClient;
  GetClientRect(&rcClient);

  //��������
  CDC CdcMdc;
  HDC hDC=lpDrawItemStruct->hDC;
  CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

  //��ť״̬
  bool bDisable=((lpDrawItemStruct->itemState&ODS_DISABLED)!=0);
  bool bButtonDown=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0);

  //������Դ
  CBitmap ImageBuffer;
  CBitImage ImageButton;
  if((m_hResInstance!=NULL)&&(m_pszResource!=NULL))
  {
    //������Դ
    ImageBuffer.LoadBitmap(m_pszResource);
    ImageButton.LoadFromResource(m_hResInstance, m_pszResource);
  }

  //���� DC
  if(!CdcMdc.CreateCompatibleDC(pDC))
  {
    ASSERT(FALSE);
  }

  //�滭����
  if(ImageButton.IsNull()==false)
  {
    INT nImagePos=0;
    INT nPartWidth=ImageButton.GetWidth()/5;
    INT nPartHeight=ImageButton.GetHeight();

    //����λ��
    if(bDisable==true)
    {
      nImagePos=nPartWidth*4;
    }
    if((bDisable==false)&&(bButtonDown==true))
    {
      nImagePos=nPartWidth;
    }
    if((bDisable==false)&&(bButtonDown==false)&&(m_bHovering==true))
    {
      nImagePos=nPartWidth*3;
    }

    //�滭����
    CdcMdc.SetBkMode(TRANSPARENT);
    CdcMdc.SelectObject(&ImageBuffer);

    //͸���滭
    pDC->TransparentBlt(0, 0, nPartWidth, nPartHeight, &CdcMdc, nImagePos, 0, nPartWidth, nPartHeight, RGB(255, 0, 255));
  }

  DrawButtonText(pDC,lpDrawItemStruct->itemState);

  return ;
}

//�滭����
VOID CSkinButtonEx::DrawButtonText(CDC * pDC, UINT uItemState)
{
  //��ȡ����
  CString strText;
  GetWindowText(strText);

  //�滭����
  if(strText.IsEmpty()==FALSE)
  {
    //��ȡλ��
    CRect rcClient;
    GetClientRect(&rcClient);

    //��ť״̬
    bool bDisable=((uItemState&ODS_DISABLED)!=0);
    bool bButtonDown=((uItemState&ODS_SELECTED)!=0);

    //���û���
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor((bDisable==true)?GetSysColor(COLOR_GRAYTEXT):m_crButtonText);

    //�滭����
    rcClient.top+=(bButtonDown==true)?3:1;
    pDC->DrawText(strText,strText.GetLength(),rcClient,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
  }

  return;
}
