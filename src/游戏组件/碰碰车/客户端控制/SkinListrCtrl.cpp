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


//控件消息
BOOL CSkinHeaderCtrlEx::OnChildNotify(UINT uMessage, WPARAM wParam, LPARAM lParam, LRESULT * pLResult)
{
  //变量定义
  NMHEADER * pNMHearder=(NMHEADER*)lParam;

  //拖动消息
  if((pNMHearder->hdr.code==HDN_BEGINTRACKA)||(pNMHearder->hdr.code==HDN_BEGINTRACKW))
  {
    //禁止拖动
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

//重画函数
void CSkinHeaderCtrlEx::OnPaint()
{
  CPaintDC dc(this);


  ////建立缓冲图
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

  ////绘画背景

  //BITMAP map;
  //CDC dcCompatibale;
  //dcCompatibale.CreateCompatibleDC(&BufferDC);

  //CBitmap * pOldBitmap=dcCompatibale.SelectObject(CBitmap::FromHandle(m_bmpTitle));
  //m_bmpTitle.GetBitmap(&map);

  //BufferDC.StretchBlt(ClientRect.left,ClientRect.top,ClientRect.Width(),ClientRect.Height(),&dcCompatibale,
  //  0,0,map.bmWidth,map.bmHeight,SRCCOPY);

  ////绘画列
  //HDITEM HDItem;
  //CRect rcItemRect;
  //TCHAR szBuffer[30];

  //for (int i=0;i<GetItemCount();i++)
  //{
  //  //获取信息
  //  HDItem.pszText=szBuffer;
  //  HDItem.mask=HDI_TEXT|HDI_FORMAT;
  //  HDItem.cchTextMax=sizeof(szBuffer)/sizeof(szBuffer[0])-1;
  //  GetItemRect(i,&rcItemRect);
  //  GetItem(i,&HDItem);

  //  //绘画背景
  //  BufferDC.StretchBlt(rcItemRect.left,rcItemRect.top,rcItemRect.Width(),rcItemRect.Height(),&dcCompatibale,
  //    0,0,map.bmWidth,map.bmHeight,SRCCOPY);
  //  BufferDC.Draw3dRect(&rcItemRect,DFC_BUTTON,DFCS_BUTTONPUSH);
  //  rcItemRect.DeflateRect(6,2,6,2);

  //  //绘画标题头
  //  UINT uFormat=DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS;
  //  if (HDItem.fmt&HDF_CENTER) uFormat|=DT_CENTER;
  //  else if (HDItem.fmt&HDF_LEFT) uFormat|=DT_LEFT;
  //  else if (HDItem.fmt&HDF_RIGHT) uFormat|=DT_RIGHT;

  //  BufferDC.DrawText(HDItem.pszText,lstrlen(HDItem.pszText),&rcItemRect,uFormat);

  //}

  //dcCompatibale.SelectObject(pOldBitmap);
  //dcCompatibale.DeleteDC();
  ////绘画界面

  //dc.BitBlt(0,0,ClientRect.Width(),ClientRect.Height(),&BufferDC,0,0,SRCCOPY);
  ////BufferDC.DeleteDC();
  ////BufferBmp.DeleteObject();

  return;
}

//背景函数
BOOL CSkinHeaderCtrlEx::OnEraseBkgnd(CDC * pDC)
{
  return TRUE;
}
CSkinHeaderCtrlEx::CSkinHeaderCtrlEx()
{

  //font.CreateFont(5,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));

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
    TEXT("宋体"));


}
//////////////////////////////////////////////////////////////////////////


CSkinListCtrlEx::CSkinListCtrlEx()
{
  //设置变量
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

  // 画文本背景
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

  //设置字体颜色
  CString str;

  //得到焦点大小
  CRect rcFocus = lpDIS->rcItem;
  rcFocus.DeflateRect(1,1,1,1);

  if(lpDIS->itemAction & ODA_DRAWENTIRE)
  {
    //写文本
    CString szText;
    for(int i = 0; i < GetHeaderCtrl()->GetItemCount(); i++)
    {
      //循环得到文本
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

//控件绑定
void CSkinListCtrlEx::PreSubclassWindow()
{
  CListCtrl::PreSubclassWindow();

  //设置滚动
  //m_SkinScrollBar.InitScroolBar(this);
  // m_ListHeader.SubclassWindow(::GetDlgItem(m_hWnd,0));
  //InitListCtrl();
}

//建立消息
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

//背景函数
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

//初始化控件
BOOL CSkinListCtrlEx::InitListCtrl()
{
  m_ListHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());
  //SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_UNDERLINEHOT|LVS_EX_FLATSB|LBS_DISABLENOSCROLL);



  return TRUE;
}



//插入列表
void CSkinListCtrlEx::InserUser(sUserInfo & UserInfo,bool bSetColor,COLORREF TextColor, COLORREF TextBkColor)
{

  if(FindUser(UserInfo.strUserName))
  {
    UpdateUser(UserInfo);
    return;
  }

  //变量定义
  WORD wListIndex=0;
  WORD wColumnCount=1;
  TCHAR szBuffer[128]=TEXT("");
  //游戏玩家
  INT nItemIndex=InsertItem(GetItemCount(),TEXT(""),UserInfo.nImageIndex);

  //玩家姓名
  myprintf(szBuffer,CountArray(szBuffer),TEXT("%s"),UserInfo.strUserName);
  SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

  //玩家金币
  myprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),UserInfo.lUserScore);
  SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

  //总输赢成绩
  myprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),UserInfo.lWinScore);
  SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);
  if(bSetColor)
  {
    SetItemColor(nItemIndex,TextColor,TextBkColor);
  }

  return;
}

//查找玩家
bool CSkinListCtrlEx::FindUser(CString lpszUserName)
{
  ////构造变量
  //LVFINDINFO lvFindInfo;
  //ZeroMemory( &lvFindInfo, sizeof( lvFindInfo ) );
  //lvFindInfo.flags=LVFI_STRING;
  //lvFindInfo.psz=lpszUserName;

  ////查找子项
  //int nItem=FindItem(&lvFindInfo);

  ////返回结果
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


//删除用户
void CSkinListCtrlEx::DeleteUser(sUserInfo & UserInfo)
{
  //构造变量
  LVFINDINFO lvFindInfo;
  ZeroMemory(&lvFindInfo, sizeof(lvFindInfo));
  lvFindInfo.flags = LVFI_STRING;
  lvFindInfo.psz = (LPCTSTR)UserInfo.strUserName;

  //查找子项
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

  //删除子项
  if(nItem != -1)
  {
    DeleteItem(nItem);
  }
}

//更新列表
void CSkinListCtrlEx::UpdateUser(sUserInfo & UserInfo)
{
  //构造变量
  LVFINDINFO lvFindInfo;
  ZeroMemory(&lvFindInfo, sizeof(lvFindInfo));
  lvFindInfo.flags = LVFI_STRING;
  lvFindInfo.psz = (LPCTSTR)UserInfo.strUserName;

  //查找子项
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

  //删除子项
  if(nItem != -1)
  {
    TCHAR szBuffer[128]=TEXT("");

    //玩家金币
    myprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),UserInfo.lUserScore);
    SetItem(nItem,2,LVIF_TEXT,szBuffer,0,0,0,0);

    //总输赢
    myprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),UserInfo.lWinScore);
    SetItem(nItem,3,LVIF_TEXT,szBuffer,0,0,0,0);

    Invalidate(FALSE);
  }
}

//清空列表
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

    return (HBRUSH) br; //返回画刷句柄


  }


  return hbr;
}

void CSkinListCtrlEx::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
  // TODO: 在此添加消息处理程序代码和/或调用默认值
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

  //窗口位置
  CRect rcArce;
  SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);

  //读取位置
  CSize SizeRestrict(cx,cy);
  //正常位置
  CRect rcNormalSize;

  //位置调整
  SizeRestrict.cx=__min(rcArce.Width(),SizeRestrict.cx);
  SizeRestrict.cy=__min(rcArce.Height(),SizeRestrict.cy);

  //移动窗口
  rcNormalSize.top=(rcArce.Height()-SizeRestrict.cy)/2;
  rcNormalSize.left=(rcArce.Width()-SizeRestrict.cx)/2;
  rcNormalSize.right=rcNormalSize.left+SizeRestrict.cx;
  rcNormalSize.bottom=rcNormalSize.top+SizeRestrict.cy;

  //m_SkinVerticleScrollbar.MoveWindow(clientRect.Width()+10,rcNormalSize.Height()-96+clientRect.top,16,clientRect.Height()-2);
  //m_SkinHorizontalScrollbar.MoveWindow(clientRect.left+10,rcNormalSize.Height()-96+clientRect.bottom-16,clientRect.Width(),16);
}

void CSkinListCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  // TODO: 在此添加消息处理程序代码和/或调用默认值

  CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

  //m_SkinVerticleScrollbar.UpdateThumbPosition();
  //m_SkinHorizontalScrollbar.UpdateThumbPosition();
}

void CSkinListCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
  // TODO: 在此添加消息处理程序代码和/或调用默认值

  //m_SkinVerticleScrollbar.UpdateThumbPosition();
  //m_SkinHorizontalScrollbar.UpdateThumbPosition();
  CListCtrl::OnMouseMove(nFlags, point);
}





BOOL CSkinListCtrlEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  // TODO: 在此添加消息处理程序代码和/或调用默认值

  //if(zDelta <0)
  //{
  //  //后滚
  //  m_SkinVerticleScrollbar.PageDown();
  //}
  //else
  //{
  //  //前滚
  //  m_SkinVerticleScrollbar.PageUp();
  //}


  return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

//排列函数
INT CALLBACK CSkinListCtrlEx::SortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
  //变量定义
  tagSortInfo * pSortInfo=(tagSortInfo *)lParamList;
  CSkinListCtrlEx * pSkinListCtrl=pSortInfo->pSkinListCtrl;

  WORD columnClicked= pSortInfo->wColumnIndex;
  //数据排序
  return pSkinListCtrl->SortItemData(lParam1,lParam2,pSortInfo->wColumnIndex,pSortInfo->bAscendSort);

}

//点击消息
VOID CSkinListCtrlEx::OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult)
{
  //变量定义
  NM_LISTVIEW * pNMListView=(NM_LISTVIEW *)pNMHDR;

  //变量定义
  tagSortInfo SortInfo;
  ZeroMemory(&SortInfo,sizeof(SortInfo));

  //设置变量
  SortInfo.pSkinListCtrl=this;
  SortInfo.bAscendSort=m_bAscendSort;
  SortInfo.wColumnIndex=pNMListView->iSubItem;

  //设置变量
  m_bAscendSort=!m_bAscendSort;

  int len=this->GetItemCount();
  for(int i=0; i<len; i++)
  {
    this->SetItemData(i,(DWORD_PTR)i);
  }
  //排列列表
  SortItems(SortFunction,(LPARAM)&SortInfo);

  return;
}

//排列数据
INT CSkinListCtrlEx::SortItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex, bool bAscendSort)
{
  //获取数据
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


  //对比数据
  INT nResult=lstrcmp(szBuffer1,szBuffer2);
  return (bAscendSort==true)?nResult:-nResult;
}

void CSkinListCtrlEx::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
  // TODO: Add your control notification handler code here
  *pResult = CDRF_DODEFAULT;
  NMLVCUSTOMDRAW * lplvdr=(NMLVCUSTOMDRAW*)pNMHDR;
  NMCUSTOMDRAW &nmcd = lplvdr->nmcd;
  switch(lplvdr->nmcd.dwDrawStage)//判断状态
  {
    case CDDS_PREPAINT:
    {
      *pResult = CDRF_NOTIFYITEMDRAW;
      break;
    }
    case CDDS_ITEMPREPAINT://如果为画ITEM之前就要进行颜色的改变
    {
      TEXT_BK tb;

      if(MapItemColor.Lookup(nmcd.dwItemSpec, tb))
        // 根据在 SetItemColor(DWORD iItem, COLORREF color) 设置的
        // ITEM号和COLORREF 在摸板中查找，然后进行颜色赋值。
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


  MapItemColor.SetAt(iItem, tb);//设置某行的颜色。
  this->RedrawItems(iItem, iItem);//重新染色

  //this->SetCheck(iItem,1);
  this->SetFocus();    //设置焦点
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
