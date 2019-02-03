#include "Stdafx.h"
#include "SkinListCtrl.h"
#include ".\skinlistctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
#define COLOR_TEXT          RGB(255,214,101)

BEGIN_MESSAGE_MAP(CSkinHeaderCtrlEx, CHeaderCtrl)
  //{{AFX_MSG_MAP(CSkinHeaderCtrlEx)
  ON_WM_PAINT()
  ON_WM_ERASEBKGND()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CSkinListCtrlEx, CListCtrl)
  //{{AFX_MSG_MAP(CSkinListCtrlEx)
  ON_WM_CREATE()
  ON_WM_ERASEBKGND()
  //}}AFX_MSG_MAP
  ON_WM_CTLCOLOR()
  ON_WM_NCCALCSIZE()
  ON_WM_VSCROLL()
  ON_WM_MOUSEMOVE()
  ON_WM_MOUSEWHEEL()
  ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
  ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
END_MESSAGE_MAP()


//�ؼ���Ϣ
BOOL CSkinHeaderCtrlEx::OnChildNotify(UINT uMessage, WPARAM wParam, LPARAM lParam, LRESULT * pLResult)
{
  //��������
  NMHEADER * pNMHearder=(NMHEADER*)lParam;

  //�϶���Ϣ
  if((pNMHearder->hdr.code==HDN_BEGINTRACKA)||(pNMHearder->hdr.code==HDN_BEGINTRACKW))
  {
    //��ֹ�϶�
    /*if (pNMHearder->iItem<(INT)m_uLockCount)
    {
      *pLResult=TRUE;
      return TRUE;
    }*/
    *pLResult=TRUE;
    return TRUE;
  }

  if(pNMHearder->hdr.code == HDN_DIVIDERDBLCLICKA || pNMHearder->hdr.code == HDN_DIVIDERDBLCLICKW)
  {

    return *pLResult=TRUE; // eat message to disallow sizing
  }
  return __super::OnChildNotify(uMessage,wParam,lParam,pLResult);
}

//�ػ�����
void CSkinHeaderCtrlEx::OnPaint()
{
  CPaintDC dc(this);


  ////��������ͼ
  //CRect ClientRect;
  //GetClientRect(&ClientRect);
  //CBitmap BufferBmp;
  //BufferBmp.CreateCompatibleBitmap(&dc,ClientRect.Width(),ClientRect.Height());
  //CDC BufferDC;
  //BufferDC.CreateCompatibleDC(&dc);
  //BufferDC.SelectObject(&BufferBmp);
  //BufferDC.SetBkMode(TRANSPARENT);
  //BufferDC.SelectObject(&font);
  //BufferDC.SetTextColor(COLOR_TEXT);

  ////�滭����

  //BITMAP map;
  //CDC dcCompatibale;
  //dcCompatibale.CreateCompatibleDC(&BufferDC);

  //CBitmap * pOldBitmap=dcCompatibale.SelectObject(CBitmap::FromHandle(m_bmpTitle));
  //m_bmpTitle.GetBitmap(&map);

  //BufferDC.StretchBlt(ClientRect.left,ClientRect.top,ClientRect.Width(),ClientRect.Height(),&dcCompatibale,
  //  0,0,map.bmWidth,map.bmHeight,SRCCOPY);

  ////�滭��
  //HDITEM HDItem;
  //CRect rcItemRect;
  //TCHAR szBuffer[30];

  //for (int i=0;i<GetItemCount();i++)
  //{
  //  //��ȡ��Ϣ
  //  HDItem.pszText=szBuffer;
  //  HDItem.mask=HDI_TEXT|HDI_FORMAT;
  //  HDItem.cchTextMax=sizeof(szBuffer)/sizeof(szBuffer[0])-1;
  //  GetItemRect(i,&rcItemRect);
  //  GetItem(i,&HDItem);

  //  //�滭����
  //  BufferDC.StretchBlt(rcItemRect.left,rcItemRect.top,rcItemRect.Width(),rcItemRect.Height(),&dcCompatibale,
  //    0,0,map.bmWidth,map.bmHeight,SRCCOPY);
  //  BufferDC.Draw3dRect(&rcItemRect,DFC_BUTTON,DFCS_BUTTONPUSH);
  //  rcItemRect.DeflateRect(6,2,6,2);

  //  //�滭����ͷ
  //  UINT uFormat=DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS;
  //  if (HDItem.fmt&HDF_CENTER) uFormat|=DT_CENTER;
  //  else if (HDItem.fmt&HDF_LEFT) uFormat|=DT_LEFT;
  //  else if (HDItem.fmt&HDF_RIGHT) uFormat|=DT_RIGHT;

  //  BufferDC.DrawText(HDItem.pszText,lstrlen(HDItem.pszText),&rcItemRect,uFormat);

  //}

  //dcCompatibale.SelectObject(pOldBitmap);
  //dcCompatibale.DeleteDC();
  ////�滭����

  //dc.BitBlt(0,0,ClientRect.Width(),ClientRect.Height(),&BufferDC,0,0,SRCCOPY);
  ////BufferDC.DeleteDC();
  ////BufferBmp.DeleteObject();

  return;
}

//��������
BOOL CSkinHeaderCtrlEx::OnEraseBkgnd(CDC * pDC)
{
  return TRUE;
}
CSkinHeaderCtrlEx::CSkinHeaderCtrlEx()
{

  //font.CreateFont(5,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("����"));

  font.CreateFont(
    13,                        // nHeight
    0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_NORMAL,                 // nWeight
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
    TEXT("����"));


}
//////////////////////////////////////////////////////////////////////////


CSkinListCtrlEx::CSkinListCtrlEx()
{
  //���ñ���
  m_bAscendSort=false;
}

CSkinListCtrlEx::~CSkinListCtrlEx()
{

}
// CSkinListCtrlEx message handlers

void CSkinListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

  LPDRAWITEMSTRUCT   lpDIS   =   lpDrawItemStruct;
  CDC*   pDC   =   CDC::FromHandle(lpDIS->hDC);

  LVITEM lvi = {0};
  lvi.mask = LVIF_STATE;//|LVIF_IMAGE;
  lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED ;
  lvi.iItem = lpDIS->itemID;
  BOOL bGet = GetItem(&lvi);

  BOOL bHighlight =((lvi.state & LVIS_DROPHILITED)||((lvi.state & LVIS_SELECTED) &&
                    ((GetFocus() == this)|| (GetStyle() & LVS_SHOWSELALWAYS))));

  // ���ı�����
  CRect rcBack = lpDIS->rcItem;
  pDC->SetBkMode(TRANSPARENT);
  if(bHighlight)
  {
    pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
    pDC->FillRect(rcBack, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
  }
  else
  {
    /*if (lpDIS->itemID%2 ==0)
    {
      pDC->SetTextColor(0x0C0F80);
      pDC->FillRect(rcBack, &CBrush(0xEEF1E3));
    }
    else
    {
      pDC->SetTextColor(RGB(0,128,255));
      pDC->FillRect(rcBack, &CBrush(0xE3e3e3));
    } */

    pDC->SetTextColor(0x0C0F80);
    pDC->FillRect(rcBack, &CBrush(0xEEF1E3));
  }

  //����������ɫ
  CString str;

  //�õ������С
  CRect rcFocus = lpDIS->rcItem;
  rcFocus.DeflateRect(1,1,1,1);

  if(lpDIS->itemAction & ODA_DRAWENTIRE)
  {
    //д�ı�
    CString szText;
    for(int i = 0; i < GetHeaderCtrl()->GetItemCount(); i++)
    {
      //ѭ���õ��ı�
      CRect rcItem;
      if(!GetSubItemRect(lpDIS->itemID, i, LVIR_LABEL, rcItem))
      {
        continue;
      }
      szText = GetItemText(lpDIS->itemID, i);
      rcItem.left += 5;
      rcItem.right -= 1;
      pDC->DrawText(szText, lstrlen(szText), &rcItem, DT_LEFT|DT_NOPREFIX|DT_SINGLELINE);
    }
  }

}

//�ؼ���
void CSkinListCtrlEx::PreSubclassWindow()
{
  CListCtrl::PreSubclassWindow();

  //���ù���
  //m_SkinScrollBar.InitScroolBar(this);
  // m_ListHeader.SubclassWindow(::GetDlgItem(m_hWnd,0));
  //InitListCtrl();
}

//������Ϣ
int CSkinListCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(CListCtrl::OnCreate(lpCreateStruct)==-1)
  {
    return -1;
  }



  InitListCtrl();
  Init();
  return 0;
}

//��������
BOOL CSkinListCtrlEx::OnEraseBkgnd(CDC * pDC)
{



  if(m_ListHeader.GetSafeHwnd())
  {
    CRect ClientRect,ListHeaderRect;
    GetClientRect(&ClientRect);
    m_ListHeader.GetWindowRect(&ListHeaderRect);
    ClientRect.top=ListHeaderRect.Height();
    pDC->FillSolidRect(&ClientRect,GetBkColor());
    //return FALSE;
  }

  //m_SkinVerticleScrollbar.UpdateThumbPosition();
  //m_SkinHorizontalScrollbar.UpdateThumbPosition();

  //return TRUE;

  return CListCtrl::OnEraseBkgnd(pDC);
}

//��ʼ���ؼ�
BOOL CSkinListCtrlEx::InitListCtrl()
{
  m_ListHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());
  //SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_UNDERLINEHOT|LVS_EX_FLATSB|LBS_DISABLENOSCROLL);



  return TRUE;
}



//�����б�
void CSkinListCtrlEx::InserUser(sUserInfo & UserInfo,bool bSetColor,COLORREF TextColor, COLORREF TextBkColor)
{

  if(FindUser(UserInfo.strUserName))
  {
    UpdateUser(UserInfo);
    return;
  }

  //��������
  WORD wListIndex=0;
  WORD wColumnCount=1;
  TCHAR szBuffer[128]=TEXT("");
  //��Ϸ���
  INT nItemIndex=InsertItem(GetItemCount(),TEXT(""),UserInfo.nImageIndex);

  //�������
  myprintf(szBuffer,CountArray(szBuffer),TEXT("%s"),UserInfo.strUserName);
  SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

  //��ҽ��
  myprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),UserInfo.lUserScore);
  SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

  //����Ӯ�ɼ�
  myprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),UserInfo.lWinScore);
  SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);
  if(bSetColor)
  {
    SetItemColor(nItemIndex,TextColor,TextBkColor);
  }

  return;
}

//�������
bool CSkinListCtrlEx::FindUser(CString lpszUserName)
{
  ////�������
  //LVFINDINFO lvFindInfo;
  //ZeroMemory( &lvFindInfo, sizeof( lvFindInfo ) );
  //lvFindInfo.flags=LVFI_STRING;
  //lvFindInfo.psz=lpszUserName;

  ////��������
  //int nItem=FindItem(&lvFindInfo);

  ////���ؽ��
  //return -1==nItem ? false : true;


  for(int i=0; i<GetItemCount(); i++)
  {
    CString strName=GetItemText(i,1);

    if(strName==lpszUserName)
    {
      return true;
    }
  }

  return false;


}


//ɾ���û�
void CSkinListCtrlEx::DeleteUser(sUserInfo & UserInfo)
{
  //�������
  LVFINDINFO lvFindInfo;
  ZeroMemory(&lvFindInfo, sizeof(lvFindInfo));
  lvFindInfo.flags = LVFI_STRING;
  lvFindInfo.psz = (LPCTSTR)UserInfo.strUserName;

  //��������
  int nItem = -1;//FindItem( &lvFindInfo );


  for(int i=0; i<GetItemCount(); i++)
  {
    CString strName=GetItemText(i,1);

    if(strName==UserInfo.strUserName)
    {
      nItem=i;
      break;
    }
  }

  //ɾ������
  if(nItem != -1)
  {
    DeleteItem(nItem);
  }
}

//�����б�
void CSkinListCtrlEx::UpdateUser(sUserInfo & UserInfo)
{
  //�������
  LVFINDINFO lvFindInfo;
  ZeroMemory(&lvFindInfo, sizeof(lvFindInfo));
  lvFindInfo.flags = LVFI_STRING;
  lvFindInfo.psz = (LPCTSTR)UserInfo.strUserName;

  //��������
  int nItem = -1;//FindItem( &lvFindInfo );


  for(int i=0; i<GetItemCount(); i++)
  {
    CString strName=GetItemText(i,1);

    if(strName==UserInfo.strUserName)
    {
      nItem=i;
      break;
    }
  }

  //ɾ������
  if(nItem != -1)
  {
    TCHAR szBuffer[128]=TEXT("");

    //��ҽ��
    myprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),UserInfo.lUserScore);
    SetItem(nItem,2,LVIF_TEXT,szBuffer,0,0,0,0);

    //����Ӯ
    myprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),UserInfo.lWinScore);
    SetItem(nItem,3,LVIF_TEXT,szBuffer,0,0,0,0);

    Invalidate(FALSE);
  }
}

//����б�
void CSkinListCtrlEx::ClearAll()
{
  DeleteAllItems();
}
//////////////////////////////////////////////////////////////////////////

HBRUSH CSkinListCtrlEx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = CListCtrl::OnCtlColor(pDC, pWnd, nCtlColor);

  if(nCtlColor==CTLCOLOR_SCROLLBAR)
  {
    CRect rect;
    pWnd->GetClientRect(&rect);
    COLORREF bkColor=RGB(255,0,0);
    CBrush br;
    br.CreateSolidBrush(bkColor);
    pDC->FillRect(rect, &br);
    pDC->SetBkColor(bkColor);

    return (HBRUSH) br; //���ػ�ˢ���


  }


  return hbr;
}

void CSkinListCtrlEx::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
  // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
  //ModifyStyle(WS_HSCROLL | WS_VSCROLL,0,0);
  CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CSkinListCtrlEx::Init()
{
  //InitializeFlatSB(this->m_hWnd);
  //FlatSB_EnableScrollBar(this->m_hWnd, SB_BOTH, ESB_DISABLE_BOTH);

  /*CRect windowRect;
  GetWindowRect(&windowRect);

  int nTitleBarHeight = 0;

  CWnd* pParent = GetParent();

  if(pParent->GetStyle() & WS_CAPTION)
    nTitleBarHeight = GetSystemMetrics(SM_CYSIZE);


  int nDialogFrameHeight = 0;
  int nDialogFrameWidth = 0;
  if((pParent->GetStyle() & WS_BORDER))
  {
    nDialogFrameHeight = GetSystemMetrics(SM_CYDLGFRAME);
    nDialogFrameWidth = GetSystemMetrics(SM_CYDLGFRAME);
  }

  if(pParent->GetStyle() & WS_THICKFRAME)
  {
    nDialogFrameHeight+=1;
    nDialogFrameWidth+=1;
  }

  CRect rect1(windowRect.right-nDialogFrameWidth,windowRect.top-nTitleBarHeight-nDialogFrameHeight-1,windowRect.right+12-nDialogFrameWidth,windowRect.bottom+11-nTitleBarHeight-nDialogFrameHeight);
  CRect rect2(windowRect.left-nDialogFrameWidth,windowRect.bottom-nTitleBarHeight-nDialogFrameHeight-1,windowRect.right+1-nDialogFrameWidth,windowRect.bottom+11-nTitleBarHeight-nDialogFrameHeight);*/

  //m_SkinVerticleScrollbar.Create(NULL, WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP|WS_CLIPCHILDREN,rect1, pParent);
  //m_SkinHorizontalScrollbar.Create(NULL, WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE|WS_GROUP|WS_CLIPCHILDREN,rect2, pParent);

  //m_SkinVerticleScrollbar.pList = this;
  //m_SkinHorizontalScrollbar.pList = this;
}

void CSkinListCtrlEx::ReSetLoc(int cx, int cy)
{
  CRect windowRect,clientRect;
  GetWindowRect(&windowRect);

  GetClientRect(&clientRect);

  //����λ��
  CRect rcArce;
  SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);

  //��ȡλ��
  CSize SizeRestrict(cx,cy);
  //����λ��
  CRect rcNormalSize;

  //λ�õ���
  SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
  SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

  //�ƶ�����
  rcNormalSize.top=(rcArce.Height()-SizeRestrict.cy)/2;
  rcNormalSize.left=(rcArce.Width()-SizeRestrict.cx)/2;
  rcNormalSize.right=rcNormalSize.left+SizeRestrict.cx;
  rcNormalSize.bottom=rcNormalSize.top+SizeRestrict.cy;

  //m_SkinVerticleScrollbar.MoveWindow(clientRect.Width()+10,rcNormalSize.Height()-96+clientRect.top,16,clientRect.Height()-2);
  //m_SkinHorizontalScrollbar.MoveWindow(clientRect.left+10,rcNormalSize.Height()-96+clientRect.bottom-16,clientRect.Width(),16);
}

void CSkinListCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

  CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

  //m_SkinVerticleScrollbar.UpdateThumbPosition();
  //m_SkinHorizontalScrollbar.UpdateThumbPosition();
}

void CSkinListCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
  // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

  //m_SkinVerticleScrollbar.UpdateThumbPosition();
  //m_SkinHorizontalScrollbar.UpdateThumbPosition();
  CListCtrl::OnMouseMove(nFlags, point);
}





BOOL CSkinListCtrlEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

  //if(zDelta <0)
  //{
  //  //���
  //  m_SkinVerticleScrollbar.PageDown();
  //}
  //else
  //{
  //  //ǰ��
  //  m_SkinVerticleScrollbar.PageUp();
  //}


  return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

//���к���
INT CALLBACK CSkinListCtrlEx::SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
  //��������
  tagSortInfo * pSortInfo=(tagSortInfo *)lParamList;
  CSkinListCtrlEx * pSkinListCtrl=pSortInfo->pSkinListCtrl;

  WORD columnClicked= pSortInfo->wColumnIndex;
  //��������
  return pSkinListCtrl->SortItemData(lParam1,lParam2,pSortInfo->wColumnIndex,pSortInfo->bAscendSort);

}

//�����Ϣ
VOID CSkinListCtrlEx::OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult)
{
  //��������
  NM_LISTVIEW * pNMListView=(NM_LISTVIEW *)pNMHDR;

  //��������
  tagSortInfo SortInfo;
  ZeroMemory(&SortInfo,sizeof(SortInfo));

  //���ñ���
  SortInfo.pSkinListCtrl=this;
  SortInfo.bAscendSort=m_bAscendSort;
  SortInfo.wColumnIndex=pNMListView->iSubItem;

  //���ñ���
  m_bAscendSort=!m_bAscendSort;

  int len=this->GetItemCount();
  for(int i=0; i<len; i++)
  {
    this->SetItemData(i,(DWORD_PTR)i);
  }
  //�����б�
  SortItems(SortFunction,(LPARAM)&SortInfo);

  return;
}

//��������
INT CSkinListCtrlEx::SortItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex, bool bAscendSort)
{
  //��ȡ����
  TCHAR szBuffer1[256]=TEXT("");
  TCHAR szBuffer2[256]=TEXT("");
  GetItemText((INT)lParam1,wColumnIndex,szBuffer1,CountArray(szBuffer1));
  GetItemText((INT)lParam2,wColumnIndex,szBuffer2,CountArray(szBuffer2));

  if(wColumnIndex==2||wColumnIndex==3)
  {
    int nData1=_wtoi(szBuffer1);
    int nData2=_wtoi(szBuffer2);

    INT nResult=0;
    if(nData1>nData2)
    {
      nResult=1;
    }
    if(nData1<nData2)
    {
      nResult=-1;
    }
    return (bAscendSort==true)?nResult:-nResult;
  }


  //�Ա�����
  INT nResult=lstrcmp(szBuffer1,szBuffer2);
  return (bAscendSort==true)?nResult:-nResult;
}

void CSkinListCtrlEx::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
  // TODO: Add your control notification handler code here
  *pResult = CDRF_DODEFAULT;
  NMLVCUSTOMDRAW * lplvdr=(NMLVCUSTOMDRAW*)pNMHDR;
  NMCUSTOMDRAW &nmcd = lplvdr->nmcd;
  switch(lplvdr->nmcd.dwDrawStage)//�ж�״̬
  {
    case CDDS_PREPAINT:
    {
      *pResult = CDRF_NOTIFYITEMDRAW;
      break;
    }
    case CDDS_ITEMPREPAINT://���Ϊ��ITEM֮ǰ��Ҫ������ɫ�ĸı�
    {
      TEXT_BK tb;

      if(MapItemColor.Lookup(nmcd.dwItemSpec, tb))
        // ������ SetItemColor(DWORD iItem, COLORREF color) ���õ�
        // ITEM�ź�COLORREF �������в��ң�Ȼ�������ɫ��ֵ��
      {
        lplvdr->clrText = tb.colText;
        lplvdr->clrTextBk = tb.colTextBk;
        *pResult = CDRF_DODEFAULT;
      }
    }
    break;
  }
}

void CSkinListCtrlEx::ClearColor()
{
  MapItemColor.RemoveAll();
}

void CSkinListCtrlEx::SetItemColor(DWORD iItem, COLORREF TextColor, COLORREF TextBkColor)
{
  TEXT_BK tb;
  tb.colText = TextColor;
  tb.colTextBk = TextBkColor;


  MapItemColor.SetAt(iItem, tb);//����ĳ�е���ɫ��
  this->RedrawItems(iItem, iItem);//����Ⱦɫ

  //this->SetCheck(iItem,1);
  this->SetFocus();    //���ý���
  UpdateWindow();
}


void CSkinListCtrlEx::SetAllItemColor(DWORD iItem, COLORREF TextColor, COLORREF TextBkColor)
{
  //INT_PTR ncount = this->GetItemCount();
  TEXT_BK tb;
  tb.colText = TextColor;
  tb.colTextBk = TextBkColor;

  if(iItem > 0)
  {
    for(DWORD numItem = 0; numItem < iItem ; numItem ++)
    {
      //iItem = i;
      MapItemColor.SetAt(numItem, tb);
      this->RedrawItems(numItem, numItem);
    }
  }

  return;

}
