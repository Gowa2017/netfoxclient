#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//数据库类
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//用户数据库
protected:
	CDataBaseAide					m_AccountsDBAide;					//用户数据库
	CDataBaseHelper					m_AccountsDBModule;					//用户数据库

	//游戏币数据库
protected:
	CDataBaseAide					m_TreasureDBAide;					//游戏币数据库
	CDataBaseHelper					m_TreasureDBModule;					//游戏币数据库

	//平台数据库
protected:
	CDataBaseAide					m_PlatformDBAide;					//平台数据库
	CDataBaseHelper					m_PlatformDBModule;					//平台数据库

	//组件变量
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件
	//辅助变量
protected:
	DBO_GR_LogonFailure				m_LogonFailure;						//登录失败
	DBO_GR_LogonSuccess				m_LogonSuccess;						//登录成功

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//登录函数
protected:
	//I D 登录
	bool OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//账号登录
	bool OnRequestLogonUserAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改好友
	bool OnRequestModifyFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//添加好友
	bool OnRequestAddFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//删除好友
	bool OnRequestDeleteFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改分组
	bool OnRequestModifyGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改备注
	bool OnRequestModifyRemarks(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查找用户
	bool OnRequestSearchUser(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//存储消息
	bool OnRequestSaveOfflineMessage(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//喇叭消息
	bool OnRequestTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//辅助函数
private:
	//操作结果
	VOID OnUserOperateResult(DWORD dwContextID,bool bOperateSucess,DWORD dwErrorCode,WORD wOperateCode,LPCTSTR pszDescribeString);
};

//////////////////////////////////////////////////////////////////////////////////

#endif