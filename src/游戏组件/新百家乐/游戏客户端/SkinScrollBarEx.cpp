#include "StdAfx.h"
#include "Resource.h"
#include "SkinScrollBarEx.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CSkinScrollBarEx::CSkinScrollBarEx()
{
	m_ImageScroll.LoadFromResource(AfxGetInstanceHandle(),IDB_SKIN_SCROLL);
}

//��������
CSkinScrollBarEx::~CSkinScrollBarEx()
{
}

//���ù���
VOID CSkinScrollBarEx::InitScroolBar(CWnd * pWnd)
{
	//���ù���
	SkinSB_Init(pWnd->GetSafeHwnd(),m_ImageScroll);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
