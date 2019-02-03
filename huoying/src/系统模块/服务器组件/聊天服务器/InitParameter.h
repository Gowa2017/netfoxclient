#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//������Ϣ
public:
	WORD							m_wConnectTime;						//����ʱ��
	WORD							m_wCollectTime;						//ͳ��ʱ��

	//������Ϣ
public:
	WORD							m_wServiceID;						//�����ʶ
	WORD							m_wServicePort;						//����˿�
	DWORD							m_dwServiceAddr;					//�����ַ
	DWORD							m_dwServiceRule;					//�������
	WORD							m_wMaxPlayer;						//�������
	BYTE							m_cbMinOrder;						//��ͼ���

	//Э����Ϣ
public:
	WORD							m_wCorrespondPort;					//Э���˿�
	tagAddressInfo					m_CorrespondAddress;				//Э����ַ

	//�����ַ
public:
	tagAddressInfo					m_ServiceAddress;					//�����ַ
	tagDataBaseParameter			m_AccountsDBParameter;				//���ӵ�ַ
	tagDataBaseParameter			m_TreasureDBParameter;				//���ӵ�ַ
	tagDataBaseParameter			m_PlatformDBParameter;				//���ӵ�ַ

	//��������
public:
	//���캯��
	CInitParameter();
	//��������
	virtual ~CInitParameter();

	//���ܺ���
public:
	//��ʼ��
	VOID InitParameter();
	//��������
	VOID LoadInitParameter();
};

//////////////////////////////////////////////////////////////////////////////////

#endif
