
// CfishbaojingDlg.h : 头文件
//
#include "afxcmn.h"
#include "afxwin.h"
#include "Cfishbaojing.h"

#pragma once


// CCfishbaojingDlg 对话框
class CCfishbaojingDlg : public CDialogEx
{
	CListCtrl                    m_baojing;
	CListCtrl                 m_zongku;
	bool    RHVar;
	int							m_ListRow;
	int							m_ListCol;
// 构造
public:
	CCfishbaojingDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CFISHBAOJING_DIALOG };
	bool PipeSendData(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	bool PipeMsgEvent(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	


// 实现
protected:
	HICON m_hIcon;
	//让变量和控件绑定起来
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	///消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	void OnDBlclkList(NMHDR *pNMHDR, LRESULT *pResult);
	//void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);

	CListCtrl					m_list;				// 综合鱼

	CMyEdit						m_EditTest;    //输入框
	CButton    RHVAR;
	CMyEdit  m_bulletcount; //子弹上限
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedSc2();
	afx_msg void OnBnClickedOk3();
	afx_msg void OnBnClickedlangchao();
	afx_msg void OnBnClickedOk5();
	afx_msg void OnBnClickedOk6();
	afx_msg void OnBnClickedOk7();
	afx_msg void OnBnClickedheibaiqueding();
};
