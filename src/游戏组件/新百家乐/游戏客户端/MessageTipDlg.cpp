// MessageTipDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MessageTipDlg.h"


// CMessageTipDlg �Ի���


IMPLEMENT_DYNAMIC(CMessageTipDlg, CSkinDialog)
CMessageTipDlg::CMessageTipDlg(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CMessageTipDlg::IDD, pParent)
{
}

CMessageTipDlg::~CMessageTipDlg()
{
}

void CMessageTipDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}


BEGIN_MESSAGE_MAP(CMessageTipDlg, CSkinDialog)

END_MESSAGE_MAP()



//��ʼ������
BOOL CMessageTipDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("��ʾ"));
	
	m_brush.CreateSolidBrush(RGB(136,77,32));

	return TRUE;
}

//ȷ����Ϣ
void CMessageTipDlg::OnOK()
{
		
	__super::OnOK();
}

// CMessageTipDlg ��Ϣ�������
