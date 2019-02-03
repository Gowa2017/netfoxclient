#pragma once
#include "Stdafx.h"
#include "../��Ϸ�ͻ���/ClientControl.h"


// CClientControlItemSinkDlg �Ի���

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

//������
protected:
	LONGLONG						m_lStorageStart;				//��ʼ���
	LONGLONG						m_lStorageDeduct;				//���˥��
	LONGLONG						m_lStorageCurrent;				//��ǰ���
	LONGLONG						m_lStorageMax1;					//�������1
	LONGLONG						m_lStorageMul1;					//ϵͳ��ָ���1
	LONGLONG						m_lStorageMax2;					//�������2
	LONGLONG						m_lStorageMul2;					//ϵͳ��ָ���2

	//�������
protected:
	BYTE m_cbWinArea;				//Ӯ������
	BYTE m_cbExcuteTimes;			//ִ�д���
	
	//�����ע
protected:
	CListCtrl						m_listUserBet;					//�б�ؼ�
	CListCtrl						m_listUserBetAll;				//�б�ؼ�
	LONGLONG						m_lQueryGameID;					//��ѯID

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClientControlItemSinkDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ADMIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	//������
	virtual bool __cdecl ReqResult(const void * pBuffer);
	//���¿��
	virtual bool __cdecl UpdateStorage(const void * pBuffer);
	//������ע
	virtual void __cdecl UpdateUserBet(bool bReSet);
	//���¿ؼ�
	virtual void __cdecl UpdateControl();

	virtual BOOL OnInitDialog();
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();

public:
	afx_msg void OnBnClickedBtnUpdateStorage();
	void RequestUpdateStorage();
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnBnClickedBtnUserBetAll();
	afx_msg void OnEnSetfocusEditUserId();
	afx_msg void OnBnClickedRadioRb();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
