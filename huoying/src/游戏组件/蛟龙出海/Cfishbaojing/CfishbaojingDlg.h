
// CfishbaojingDlg.h : ͷ�ļ�
//
#include "afxcmn.h"
#include "afxwin.h"
#include "Cfishbaojing.h"

#pragma once


// CCfishbaojingDlg �Ի���
class CCfishbaojingDlg : public CDialogEx
{
	CListCtrl                    m_baojing;
	CListCtrl                 m_zongku;
	bool    RHVar;
	int							m_ListRow;
	int							m_ListCol;
// ����
public:
	CCfishbaojingDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CFISHBAOJING_DIALOG };
	bool PipeSendData(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	bool PipeMsgEvent(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	


// ʵ��
protected:
	HICON m_hIcon;
	//�ñ����Ϳؼ�������
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	///��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	void OnDBlclkList(NMHDR *pNMHDR, LRESULT *pResult);
	//void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);

	CListCtrl					m_list;				// �ۺ���

	CMyEdit						m_EditTest;    //�����
	CButton    RHVAR;
	CMyEdit  m_bulletcount; //�ӵ�����
	// ���ɵ���Ϣӳ�亯��
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
