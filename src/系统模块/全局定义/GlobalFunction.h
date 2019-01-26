#ifndef GLOBALFUNCTION_HEAD_FILE
#define GLOBALFUNCTION_HEAD_FILE

#pragma once

inline void ShowLog(LPCTSTR strInfo,...)
{
	static TCHAR szInfo[250] = {0};
	va_list ap;
	va_start(ap,strInfo);
	_vsntprintf(szInfo,sizeof(szInfo),strInfo,ap);
	va_end(ap);
	OutputDebugString(szInfo);
}

//VOID DrawMultLineText(CDC *pDC, CRect rect, int nRowDis, UINT nFromat, CString strText)
//{
//
//	if( strText.GetLength() <= 0 )
//		return;
//
//	WCHAR* pText = strText.GetBuffer(strText.GetLength());
//	int nCount = strText.GetLength();
//	CRect rtChar;
//	CSize size = pDC->GetTextExtent(pText + 0, 1);
//	int nRowHeight = size.cy + nRowDis;
//	rtChar.top = rect.top;
//	rtChar.left = rect.left;
//	rtChar.bottom  = rtChar.top + nRowDis + size.cy;
//	rtChar.right  = rtChar.left + size.cx; 
//	CString strChar;
//	for (int nCharIndex = 0; nCharIndex < nCount; nCharIndex++)
//	{
//		if( rtChar.right > rect.right )
//		{
//			rtChar.top = rtChar.bottom;
//			rtChar.bottom += nRowHeight;
//			size = pDC->GetTextExtent(pText + nCharIndex, 1);
//			rtChar.left = rect.left;
//			rtChar.right = rtChar.left + size.cx;
//			if( rtChar.bottom > rect.bottom )
//				break;
//		}
//		strChar = pText[nCharIndex];
//		pDC->DrawText(strChar, rtChar, nFromat);
//		size = pDC->GetTextExtent(pText + nCharIndex + 1, 1);
//		rtChar.left = rtChar.right;
//		rtChar.right += size.cx;
//	}
//}


//////////////////////////////////////////////////////////////////////////////////

#endif