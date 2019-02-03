#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//���ض���
#define MAX_PELS					25									//��С����
#define LESS_PELS					10									//��С����
#define DEFAULT_PELS				18									//Ĭ������

//////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CGameOption : public CSkinDialog
{
	//��������
public:
	bool							m_bEnableSound;						//��������
	bool							m_bAllowLookOn;						//����ۿ�
	DWORD							m_dwCardHSpace;						//�˿�����
	bool                            m_bEnableBGSound;					//��������
	CBrush							m_brush;

	//�ؼ�����
public:
	CSkinButton						m_btOK;								//ȷ����ť
	CSkinButton						m_btCancel;							//ȡ����ť

	//��������
public:
	//���캯��
	CGameOption();
	//��������
	virtual ~CGameOption();

	//���غ���
protected:
	//�ؼ���
	virtual void DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ����Ϣ
	virtual void OnOK();

	//��Ϣ����
public:
	//�ؼ���ɫ
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
