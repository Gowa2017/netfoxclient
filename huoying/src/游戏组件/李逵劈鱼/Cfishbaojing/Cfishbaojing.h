
// Cfishbaojing.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCfishbaojingApp: 
// �йش����ʵ�֣������ Cfishbaojing.cpp
//

class CCfishbaojingApp : public CWinApp
{
	HANDLE						m_hClientPipe;
	HANDLE						m_hServerPipe;
public:
	CCfishbaojingApp();
	bool PipeLoading();

	bool PipeMsgEvent(WORD wSubCmdID, VOID * pData = 0, WORD wDataSize = 0);

	bool PipeSendData(WORD wSubCmdID, VOID * pData = 0, WORD wDataSize = 0);
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};
class CMyEdit : public CEdit
{
	bool m_bFocus;
public:
	CMyEdit(void);
	~CMyEdit(void){};
public:
	bool isFocus(){ return m_bFocus; }
	// ʵ��  
protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);//������Ϣ  
	afx_msg void OnSetFocus(CWnd* pCwnd);
	afx_msg void OnKillFocus(CWnd* pCwnd);

	DECLARE_MESSAGE_MAP()
};
extern CCfishbaojingApp theApp;