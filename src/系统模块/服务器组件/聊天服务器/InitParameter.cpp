#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define TIME_CONNECT				30									//����ʱ��
#define TIME_COLLECT				30									//ͳ��ʱ��

//�ͻ�ʱ��
#define TIME_INTERMIT				0									//�ж�ʱ��
#define TIME_ONLINE_COUNT			600									//����ʱ��

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CInitParameter::CInitParameter()
{ 
	InitParameter();
}

//��������
CInitParameter::~CInitParameter()
{
}

//��ʼ��
VOID CInitParameter::InitParameter()
{
	//ʱ�䶨��
	m_wConnectTime=TIME_CONNECT;
	m_wCollectTime=TIME_COLLECT;

	//������Ϣ
	m_wServiceID=0;
	m_dwServiceAddr=ADDR_ANY;
	m_wServicePort=PORT_CHAT;
	m_wMaxPlayer=4096;
	m_dwServiceRule=0;
	m_cbMinOrder=0;

	//Э����Ϣ
	m_wCorrespondPort=PORT_CENTER;
	ZeroMemory(&m_CorrespondAddress,sizeof(m_CorrespondAddress));

	//������Ϣ
	ZeroMemory(&m_ServiceAddress,sizeof(m_ServiceAddress));
	ZeroMemory(&m_AccountsDBParameter,sizeof(m_AccountsDBParameter));
	ZeroMemory(&m_TreasureDBParameter,sizeof(m_TreasureDBParameter));
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//���ò���
	InitParameter();

	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf(szIniFile,CountArray(szIniFile),TEXT("%s\\ServerParameter.ini"),szWorkDir);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//��ȡ����
	m_wServiceID=IniData.ReadInt(TEXT("ChatServer"),TEXT("ServiceID"),0);
	m_dwServiceAddr=IniData.ReadInt(TEXT("ChatServer"),TEXT("ServiceAddr"),ADDR_ANY);
	m_wServicePort=IniData.ReadInt(TEXT("ChatServer"),TEXT("ServicePort"),m_wServicePort);
	m_dwServiceRule=IniData.ReadInt(TEXT("ChatServer"),TEXT("ServiceRule"),m_dwServiceRule);
	m_wMaxPlayer=IniData.ReadInt(TEXT("ChatServer"),TEXT("MaxPlayer"),m_wMaxPlayer);
	m_cbMinOrder=IniData.ReadInt(TEXT("ChatServer"),TEXT("MinOrder"),m_cbMinOrder);

	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("ServiceAddr"),NULL,m_ServiceAddress.szAddress,CountArray(m_ServiceAddress.szAddress));

	//Э����Ϣ
	m_wCorrespondPort=IniData.ReadInt(TEXT("Correspond"),TEXT("ServicePort"),m_wCorrespondPort);
	IniData.ReadEncryptString(TEXT("ServerInfo"),TEXT("CorrespondAddr"),NULL,m_CorrespondAddress.szAddress,CountArray(m_CorrespondAddress.szAddress));

	//������Ϣ
	m_AccountsDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("AccountsDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("AccountsDB"),TEXT("DBAddr"),NULL,m_AccountsDBParameter.szDataBaseAddr,CountArray(m_AccountsDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("AccountsDB"),TEXT("DBUser"),NULL,m_AccountsDBParameter.szDataBaseUser,CountArray(m_AccountsDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("AccountsDB"),TEXT("DBPass"),NULL,m_AccountsDBParameter.szDataBasePass,CountArray(m_AccountsDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("AccountsDB"),TEXT("DBName"),szAccountsDB,m_AccountsDBParameter.szDataBaseName,CountArray(m_AccountsDBParameter.szDataBaseName));

	//������Ϣ
	m_TreasureDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("TreasureDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBAddr"),NULL,m_TreasureDBParameter.szDataBaseAddr,CountArray(m_TreasureDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBUser"),NULL,m_TreasureDBParameter.szDataBaseUser,CountArray(m_TreasureDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBPass"),NULL,m_TreasureDBParameter.szDataBasePass,CountArray(m_TreasureDBParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("TreasureDB"),TEXT("DBName"),szTreasureDB,m_TreasureDBParameter.szDataBaseName,CountArray(m_TreasureDBParameter.szDataBaseName));

	//������Ϣ
	TCHAR szPlatformDBItem[32]=TEXT("PlatformDB");
	TCHAR szPlatformDBName[32]=TEXT("QPPlatformDB");
	IniData.ReadString(TEXT("LogonServer"),TEXT("PlatformDBItem"),TEXT("PlatformDB"),szPlatformDBItem,CountArray(szPlatformDBItem));
	IniData.ReadString(TEXT("LogonServer"),TEXT("PlatformDBName"),szPlatformDB,szPlatformDBName,CountArray(szPlatformDBName));
	m_PlatformDBParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("PlatformDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(szPlatformDBItem,TEXT("DBAddr"),NULL,m_PlatformDBParameter.szDataBaseAddr,CountArray(m_PlatformDBParameter.szDataBaseAddr));
	IniData.ReadEncryptString(szPlatformDBItem,TEXT("DBUser"),NULL,m_PlatformDBParameter.szDataBaseUser,CountArray(m_PlatformDBParameter.szDataBaseUser));
	IniData.ReadEncryptString(szPlatformDBItem,TEXT("DBPass"),NULL,m_PlatformDBParameter.szDataBasePass,CountArray(m_PlatformDBParameter.szDataBasePass));
	IniData.ReadEncryptString(szPlatformDBItem,TEXT("DBName"),szPlatformDBName,m_PlatformDBParameter.szDataBaseName,CountArray(m_PlatformDBParameter.szDataBaseName));

	return;
}

//////////////////////////////////////////////////////////////////////////////////
