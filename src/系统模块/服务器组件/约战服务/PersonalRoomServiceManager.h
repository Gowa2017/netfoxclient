#ifndef GAME_PERSONAL_ROOM_SERVICE_MANAGER_HEAD_FILE
#define GAME_PERSONAL_ROOM_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "PersonalRoomServiceHead.h"

////////////////////////////////////////////////////////////////////////
																	
//私人房服务管理类									
class PERSONAL_ROOM_SERVICE_CLASS CPersonalRoomServiceManager : public IPersonalRoomServiceManager
{
	//状态变量
protected:
	bool								m_bIsService;					//服务标识	
	
	//接口变量
protected:	
	IPersonalRoomItem *					m_pIPersonalRoomItem;				//私人房子项

	//服务接口
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//功能接口

	//函数定义
public:
	//构造函数
	CPersonalRoomServiceManager(void);
	//析构函数
	virtual ~CPersonalRoomServiceManager(void);

	//基础接口
public:
	//释放对象
	virtual VOID  Release() { delete this; }
	//接口查询
	virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);
	
	//控制接口
public:
	//停止服务
	virtual bool StopService();
	//启动服务
	virtual bool StartService();
	
	//管理接口

public:

	////初始化接口
	//virtual bool InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter);	

	

	//接口信息
public:
	//用户接口
	virtual IUnknownEx * GetServerUserItemSink();

	//测试
public:
	virtual void TestPersonal();

	//查询房间
	virtual bool OnTCPNetworkSubMBQueryGameServer(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService);

	//搜索房间桌号
	virtual bool OnTCPNetworkSubMBSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService);

	//强制解散搜索房间桌号
	virtual bool OnTCPNetworkSubMBDissumeSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService);

	//私人房间配置
	virtual bool OnTCPNetworkSubMBPersonalParameter(VOID * pData, WORD wDataSize, DWORD dwSocketID, IDataBaseEngine *	 pIDataBaseEngine);

	//查询私人房间列表
	virtual bool OnTCPNetworkSubMBQueryPersonalRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID,  ITCPSocketService * pITCPSocketService);

	///玩家请求房间成绩
	virtual bool OnTCPNetworkSubQueryUserRoomScore(VOID * pData, WORD wDataSize, DWORD dwSocketID,  IDataBaseEngine *	 pIDataBaseEngine);

};

//////////////////////////////////////////////////////////////////////////
#endif