#ifndef GAME_SERVER_SERVER_DLG_HEAD_FILE
#define GAME_SERVER_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////////////

//���Ի���
class CChatServerDlg : public CDialog, public IServiceUnitsSink
{
	//�������
protected:
	CServiceUnits					m_ServiceUnits;						//����Ԫ
	CTraceServiceControl			m_TraceServiceControl;				//׷�ٴ���

	//���ò���
protected:
	bool							m_bAutoControl;						//�Զ�����
	bool							m_bOptionSuccess;					//���ñ�־

	//��������
public:
	//���캯��
	CChatServerDlg();
	//��������
	virtual ~CChatServerDlg();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ����Ϣ
	virtual VOID OnOK();
	//ȡ������
	virtual VOID OnCancel();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//����ӿ�
public:
	//����״̬
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);

	//��������
protected:
	//���±���
	VOID UpdateServerTitle(enServiceStatus ServiceStatus);

	//�������
protected:
	//��������
	bool StartServerService(WORD wServerID);

	//��ť��Ϣ
protected:
	//��������
	VOID OnBnClickedStartService();
	//ֹͣ����
	VOID OnBnClickedStopService();
	//��Ϣӳ��
public:
	//�ر�ѯ��
	BOOL OnQueryEndSession();
	//�����
	LRESULT OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif