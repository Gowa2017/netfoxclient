#include "Stdafx.h"
#include "SkinListCtrl.h"
#include ".\skinlistctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
#define COLOR_TEXT					RGB(255,214,101)

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
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()


//�ؼ���Ϣ
BOOL CSkinHeaderCtrlEx::OnChildNotify(UINT uMessage, WPARAM wParam, LPARAM lParam, LRESULT * pLResult)
{
	//��������
	NMHEADER * pNMHearder=(NMHEADER*)lParam;

	//�϶���Ϣ
	if ((pNMHearder->hdr.code==HDN_BEGINTRACKA)||(pNMHearder->hdr.code==HDN_BEGINTRACKW))
	{
		//��ֹ�϶�
		/*if (pNMHearder->iItem<(INT)m_uLockCount)
		{
			*pLResult=TRUE;
			return TRUE;
		}*/
	}

	return __super::OnChildNotify(uMessage,wParam,lParam,pLResult);
}

//�ػ�����
void CSkinHeaderCtrlEx::OnPaint() 
{
	CPaintDC dc(this);


	//��������ͼ
	CRect ClientRect;
	GetClientRect(&ClientRect);
	CBitmap BufferBmp;
	BufferBmp.CreateCompatibleBitmap(&dc,ClientRect.Width(),ClientRect.Height());
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(&dc);
	BufferDC.SelectObject(&BufferBmp);
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(&font);
	BufferDC.SetTextColor(COLOR_TEXT);

	//�滭����

	BITMAP map;
	CDC dcCompatibale; 
	dcCompatibale.CreateCompatibleDC(&BufferDC); 

	CBitmap * pOldBitmap=dcCompatibale.SelectObject(CBitmap::FromHandle(m_bmpTitle)); 
	m_bmpTitle.GetBitmap(&map);   

	BufferDC.StretchBlt(ClientRect.left,ClientRect.top,ClientRect.Width(),ClientRect.Height(),&dcCompatibale,
		0,0,map.bmWidth,map.bmHeight,SRCCOPY);

	//�滭��
	HDITEM HDItem;
	CRect rcItemRect;
	TCHAR szBuffer[30];

	for (int i=0;i<GetItemCount();i++)
	{
		//��ȡ��Ϣ
		HDItem.pszText=szBuffer;
		HDItem.mask=HDI_TEXT|HDI_FORMAT;
		HDItem.cchTextMax=sizeof(szBuffer)/sizeof(szBuffer[0])-1;
		GetItemRect(i,&rcItemRect);
		GetItem(i,&HDItem);

		//�滭����
		BufferDC.StretchBlt(rcItemRect.left,rcItemRect.top,rcItemRect.Width(),rcItemRect.Height(),&dcCompatibale,
			0,0,map.bmWidth,map.bmHeight,SRCCOPY);
		BufferDC.Draw3dRect(&rcItemRect,DFC_BUTTON,DFCS_BUTTONPUSH);
		rcItemRect.DeflateRect(6,2,6,2);

		//�滭����ͷ
		UINT uFormat=DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS;
		if (HDItem.fmt&HDF_CENTER) uFormat|=DT_CENTER;
		else if (HDItem.fmt&HDF_LEFT) uFormat|=DT_LEFT;
		else if (HDItem.fmt&HDF_RIGHT) uFormat|=DT_RIGHT;

		BufferDC.DrawText(HDItem.pszText,lstrlen(HDItem.pszText),&rcItemRect,uFormat);

	}

	dcCompatibale.SelectObject(pOldBitmap);
	dcCompatibale.DeleteDC();
	//�滭����

	dc.BitBlt(0,0,ClientRect.Width(),ClientRect.Height(),&BufferDC,0,0,SRCCOPY);
	//BufferDC.DeleteDC();
	//BufferBmp.DeleteObject();

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

	m_bmpTitle.LoadBitmap(IDB_LIST_TITLE);//IDB_LIST_TITLE

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
	//��������
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	//��ȡ����
	INT nItemID=lpDrawItemStruct->itemID;
	INT nColumnCount=m_ListHeader.GetItemCount();

	//������ɫ
	COLORREF rcTextColor=RGB(255,255,255);
	COLORREF rcBackColor=RGB(38,29,25);
	GetItemColor(lpDrawItemStruct,rcTextColor,rcBackColor);

	//�滭����
	CRect rcClipBox;
	pDC->GetClipBox(&rcClipBox);

	//���û���
	pDC->SetBkColor(rcBackColor);
	pDC->SetTextColor(rcTextColor);

	//�滭����
	for (INT i=0;i<nColumnCount;i++)
	{
		//��ȡλ��
		CRect rcSubItem;
		GetSubItemRect(nItemID,i,LVIR_BOUNDS,rcSubItem);

		//�滭�ж�
		if (rcSubItem.left>rcClipBox.right) break;
		if (rcSubItem.right<rcClipBox.left) continue;

		//�滭����
		pDC->FillSolidRect(&rcSubItem,rcBackColor);

		//�滭����
		DrawCustomItem(pDC,lpDrawItemStruct,rcSubItem,i);
	}

	//�滭����
	if (lpDrawItemStruct->itemState&ODS_FOCUS)
	{
		pDC->DrawFocusRect(&rcItem);
	}

	return;
}

//�滭����
VOID CSkinListCtrlEx::DrawCustomItem(CDC * pDC, LPDRAWITEMSTRUCT lpDrawItemStruct, CRect&rcSubItem, INT nColumnIndex)
{
	//��������
	INT nItemID=lpDrawItemStruct->itemID;
	WORD wImageIndex = HIWORD(lpDrawItemStruct->itemData);

	if(0 == nColumnIndex)
	{
		INT nStatusWidth = USER_STATUS_CX;
		INT nStatusHeight=m_ImageUserStatus.GetHeight();
		m_ImageUserStatus.TransDrawImage(pDC, rcSubItem.left,rcSubItem.top, nStatusWidth,nStatusHeight,
			wImageIndex*nStatusWidth,0,nStatusWidth,nStatusHeight, RGB(255,0,255));
	}
	else
	{
		//��ȡ����
		TCHAR szString[256]=TEXT("");
		GetItemText(lpDrawItemStruct->itemID,nColumnIndex,szString,CountArray(szString));

		//�滭����
		rcSubItem.left+=5;
		pDC->DrawText(szString,lstrlen(szString),&rcSubItem,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	return;
}

//�ؼ���
void CSkinListCtrlEx::PreSubclassWindow() 
{
	__super::PreSubclassWindow();
	
	SetExtendedStyle(GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	
	//����״̬ͼƬ
	if (m_ImageUserStatus.IsNull())
	{
		m_ImageUserStatus.LoadFromResource(AfxGetInstanceHandle(),IDB_USER_STATUS_IMAGE);
	}
}

//������Ϣ
int CSkinListCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct)==-1) return -1;
	
	m_ListHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());
	
	//���ù���
	m_SkinScrollBar.InitScroolBar(this);
	
	//�����и�
	SetItemHeight(18);
	
	return 0;
}

//��������
BOOL CSkinListCtrlEx::OnEraseBkgnd(CDC * pDC) 
{
	if (m_ListHeader.GetSafeHwnd())
	{
		CRect ClientRect,ListHeaderRect;
		GetClientRect(&ClientRect);
		m_ListHeader.GetWindowRect(&ListHeaderRect);
		ClientRect.top=ListHeaderRect.Height();
		pDC->FillSolidRect(&ClientRect,GetBkColor());
	}

	return TRUE;
	
	return CListCtrl::OnEraseBkgnd(pDC);
}

//�����б�
void CSkinListCtrlEx::InserUser(sUserInfo & UserInfo)
{
	
	if (FindUser(UserInfo.strUserName))
	{
		UpdateUser(UserInfo);
		return;
	}
	
	//��������
	WORD wListIndex=0;
	WORD wColumnCount=1;
	TCHAR szBuffer[128]=TEXT("");
	//��Ϸ���
	INT nItemIndex=InsertItem(GetItemCount(),TEXT(""));
	
	//�������
	myprintf(szBuffer,CountArray(szBuffer),TEXT("%s"),UserInfo.strUserName);
	SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

	//��ҽ��
	myprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),UserInfo.lUserScore);
	SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);
	
	//����Ӯ�ɼ�
	myprintf(szBuffer,CountArray(szBuffer),TEXT("%I64d"),UserInfo.lWinScore);
	SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);	
	
	//ͼƬ �����˱�ʶ
	DWORD dwItemData = MAKELONG(UserInfo.wAndrod, UserInfo.wImageIndex);
	SetItemData(nItemIndex, dwItemData);
	
	return;
}

//�������
bool CSkinListCtrlEx::FindUser(CString lpszUserName)
{
	for(int i=0;i<GetItemCount();i++)
	{
		CString strName=GetItemText(i,1);

		if(strName==lpszUserName) return true;
	}

	return false;
}


//ɾ���û�
void CSkinListCtrlEx::DeleteUser(sUserInfo & UserInfo)
{
	//�������
	LVFINDINFO lvFindInfo;
	ZeroMemory( &lvFindInfo, sizeof( lvFindInfo ) );
	lvFindInfo.flags = LVFI_STRING;
	lvFindInfo.psz = (LPCTSTR)UserInfo.strUserName;

	//��������
	int nItem = -1;

	for(int i=0;i<GetItemCount();i++)
	{
		CString strName=GetItemText(i,1);

		if(strName==UserInfo.strUserName) 
		{
			nItem=i;
			break;
		}
	}

	//ɾ������
	if ( nItem != -1 ) DeleteItem( nItem );
}

//�����б�
void CSkinListCtrlEx::UpdateUser( sUserInfo & UserInfo )
{
	//�������
	LVFINDINFO lvFindInfo;
	ZeroMemory( &lvFindInfo, sizeof( lvFindInfo ) );
	lvFindInfo.flags = LVFI_STRING;
	lvFindInfo.psz = (LPCTSTR)UserInfo.strUserName;

	//��������
	int nItem = -1;


	for(int i=0;i<GetItemCount();i++)
	{
		CString strName=GetItemText(i,1);

		if(strName==UserInfo.strUserName) 
		{
			nItem=i;
			break;
		}
	}

	//ɾ������
	if ( nItem != -1 ) 
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

void CSkinListCtrlEx::ReSetLoc( int cx, int cy)
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

}

//��ȡ��ɫ
VOID CSkinListCtrlEx::GetItemColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF&crColorText, COLORREF&crColorBack)
{
	//���Ʊ���
	WORD wAndroid = LOWORD(lpDrawItemStruct->itemData);

	//��ɫ����
	if (lpDrawItemStruct->itemState&ODS_SELECTED)
	{
		//ѡ����ɫ
		crColorText=RGB(255,234,2);
		crColorBack=RGB(85,72,53);
	}
	else if(1 == wAndroid)
	{
		//������ɫ
		crColorText=RGB(255,255,255);
		crColorBack=RGB(255,0,0);
	}
	else
	{
		//Ĭ����ɫ
		crColorText=RGB(255,255,255);
		crColorBack=RGB(38,29,25);
	}

	return;
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
	for(int i=0;i<len;i++)
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

void CSkinListCtrlEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
   lpMeasureItemStruct->itemHeight = 20;
}

//�����и�
void CSkinListCtrlEx::SetItemHeight(UINT nHeight)
{
	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
