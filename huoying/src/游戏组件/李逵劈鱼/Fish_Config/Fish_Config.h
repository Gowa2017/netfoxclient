
// Fish_Config.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CFish_ConfigApp: 
// 有关此类的实现，请参阅 Fish_Config.cpp
//

class CFish_ConfigApp : public CWinApp
{
	HANDLE						m_hClientPipe;
	HANDLE						m_hServerPipe;
public:
	CFish_ConfigApp();

	bool PipeLoading();

	bool PipeMsgEvent(WORD wSubCmdID, VOID * pData = 0, WORD wDataSize = 0);

	bool PipeSendData(WORD wSubCmdID, VOID * pData = 0, WORD wDataSize = 0);

	bool pipeSendDataTren(VOID * pData = 0, WORD wDataSize = 0);

// 重写
public:
	virtual BOOL InitInstance();

// 实现

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
	// 实现  
protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);//声明消息  
	afx_msg void OnSetFocus(CWnd* pCwnd);
	afx_msg void OnKillFocus(CWnd* pCwnd);

	DECLARE_MESSAGE_MAP()
};
extern CFish_ConfigApp theApp;