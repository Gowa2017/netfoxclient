#ifndef MODULE_DB_PARAMETER_HEAD_FILE
#define MODULE_DB_PARAMETER_HEAD_FILE

#pragma once

#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

//ģ�����
class MODULE_MANAGER_CLASS CModuleDBParameter
{
	//�������
protected:
	tagDataBaseParameter			m_PlatformDBParameter;				//������Ϣ
	tagDataBaseParameter			m_GameMatchDBParameter;				//������Ϣ

	//��̬����
protected:
	static CModuleDBParameter *		m_pModuleDBParameter;				//����ָ��

	//��������
public:
	//���캯��
	CModuleDBParameter();
	//��������
	virtual ~CModuleDBParameter();

	//������Ϣ
public:
	//������Ϣ
	tagDataBaseParameter * GetPlatformDBParameter() { return &m_PlatformDBParameter; }
	//������Ϣ
	VOID SetPlatformDBParameter(tagDataBaseParameter & PlatformDBParameter) { m_PlatformDBParameter=PlatformDBParameter; }

	//������Ϣ
public:
	//������Ϣ
	tagDataBaseParameter * GetGameMatchDBParameter() { return &m_GameMatchDBParameter; }
	//������Ϣ
	VOID SetGameMatchDBParameter(tagDataBaseParameter & GameMatchDBParameter) { m_GameMatchDBParameter=GameMatchDBParameter; }

	//��̬����
public:
	//��ȡ����
	static CModuleDBParameter * GetModuleDBParameter() { return m_pModuleDBParameter; }
};

//////////////////////////////////////////////////////////////////////////////////

#endif