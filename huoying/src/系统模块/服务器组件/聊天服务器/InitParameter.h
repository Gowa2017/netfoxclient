#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//配置信息
public:
	WORD							m_wConnectTime;						//重连时间
	WORD							m_wCollectTime;						//统计时间

	//配置信息
public:
	WORD							m_wServiceID;						//服务标识
	WORD							m_wServicePort;						//服务端口
	DWORD							m_dwServiceAddr;					//服务地址
	DWORD							m_dwServiceRule;					//服务规则
	WORD							m_wMaxPlayer;						//最大人数
	BYTE							m_cbMinOrder;						//最低级别

	//协调信息
public:
	WORD							m_wCorrespondPort;					//协调端口
	tagAddressInfo					m_CorrespondAddress;				//协调地址

	//服务地址
public:
	tagAddressInfo					m_ServiceAddress;					//服务地址
	tagDataBaseParameter			m_AccountsDBParameter;				//连接地址
	tagDataBaseParameter			m_TreasureDBParameter;				//连接地址
	tagDataBaseParameter			m_PlatformDBParameter;				//连接地址

	//函数定义
public:
	//构造函数
	CInitParameter();
	//析构函数
	virtual ~CInitParameter();

	//功能函数
public:
	//初始化
	VOID InitParameter();
	//加载配置
	VOID LoadInitParameter();
};

//////////////////////////////////////////////////////////////////////////////////

#endif
