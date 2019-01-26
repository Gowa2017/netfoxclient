#ifndef TCP_SOCKET_SERVICE_HEAD_FILE
#define TCP_SOCKET_SERVICE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////

//网络线程
class CTCPSocketServiceThread : public CWHThread
{
	//内核变量
protected:
	HWND							m_hWnd;								//窗口句柄
	SOCKET							m_hSocket;							//连接句柄
	BYTE				            m_TCPSocketStatus;					//网络状态

	//组件变量
protected:
	CWHDataQueue					m_DataQueue;						//请求队列
	CCriticalSection				m_CriticalSection;					//队列锁定

	//接收变量
protected:
	WORD							m_wRecvSize;						//接收长度
	BYTE							m_cbRecvBuf[SOCKET_TCP_BUFFER*10];		//接收缓冲

	//缓冲变量
protected:
	bool							m_bNeedBuffer;						//缓冲状态
	DWORD							m_dwBufferData;						//缓冲数据
	DWORD							m_dwBufferSize;						//缓冲大小
	LPBYTE							m_pcbDataBuffer;					//缓冲数据

	//加密数据
protected:
	BYTE							m_cbSendRound;						//字节映射
	BYTE							m_cbRecvRound;						//字节映射
	DWORD							m_dwSendXorKey;						//发送密钥
	DWORD							m_dwRecvXorKey;						//接收密钥

	//计数变量
protected:
	DWORD							m_dwSendTickCount;					//发送时间
	DWORD							m_dwRecvTickCount;					//接收时间
	DWORD							m_dwSendPacketCount;				//发送计数
	DWORD							m_dwRecvPacketCount;				//接受计数

	//函数定义
public:
	//构造函数
	CTCPSocketServiceThread();
	//析构函数
	virtual ~CTCPSocketServiceThread();

	//重载函数
public:
	//运行函数
	virtual bool OnEventThreadRun();
	//开始事件
	virtual bool OnEventThreadStrat();
	//停止事件
	virtual bool OnEventThreadConclude();
	//停止线程
	virtual bool ConcludeThread(DWORD dwWaitSeconds);

	//请求函数
public:
	//投递请求
	bool PostThreadRequest(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize);

	//控制函数
private:
	//关闭连接
	VOID PerformCloseSocket(BYTE cbShutReason);
	//连接服务器
	DWORD PerformConnect(DWORD dwServerIP, WORD wPort);
	//发送函数
	DWORD PerformSendData(WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	DWORD PerformSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//辅助函数
private:
	//发送数据
	DWORD SendBuffer(VOID * pBuffer, WORD wSendSize);
	//缓冲数据
	VOID AmortizeBuffer(VOID * pData, WORD wDataSize);
	//解密数据
	WORD CrevasseBuffer(BYTE cbDataBuffer[], WORD wDataSize);
	//加密数据
	WORD EncryptBuffer(BYTE cbDataBuffer[], WORD wDataSize, WORD wBufferSize);

	//处理函数
private:
	//网络消息
	LRESULT OnSocketNotify(WPARAM wParam, LPARAM lParam);
	//请求消息
	LRESULT OnServiceRequest(WPARAM wParam, LPARAM lParam);

	//辅助函数
private:
	//网络读取
	LRESULT OnSocketNotifyRead(WPARAM wParam, LPARAM lParam);
	//网络发送
	LRESULT OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam);
	//网络关闭
	LRESULT OnSocketNotifyClose(WPARAM wParam, LPARAM lParam);
	//网络连接
	LRESULT OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam);

	//内联函数
private:
	//随机映射
	inline WORD SeedRandMap(WORD wSeed);
	//发送映射
	inline BYTE MapSendByte(BYTE cbData);
	//接收映射
	inline BYTE MapRecvByte(BYTE cbData);
};

//////////////////////////////////////////////////////////////////////////

//网络服务
class CTCPSocketService : public ITCPSocketService
{
	//友元说明
	friend class CTCPSocketServiceThread;

	//内核变量
protected:
	bool							m_bService;							//服务标志
	WORD							m_wServiceID;						//服务标识
	ITCPSocketEvent	*				m_pITCPSocketEvent;					//事件接口

	//组件变量
protected:
	CTCPSocketServiceThread			m_TCPSocketServiceThread;			//网络线程

	//函数定义
public:
	//构造函数
	CTCPSocketService();
	//析构函数
	virtual ~CTCPSocketService();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//服务接口
public:
	//启动服务
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();

	//配置接口
public:
	//配置函数
	virtual bool SetServiceID(WORD wServiceID);
	//设置接口
	virtual bool SetTCPSocketEvent(IUnknownEx * pIUnknownEx);

	//功能接口
public:
	//关闭连接
	virtual bool CloseSocket();
	//连接地址
	virtual bool Connect(DWORD dwServerIP, WORD wPort);
	//连接地址
	virtual bool Connect(LPCTSTR szServerIP, WORD wPort);
	//发送函数
	virtual bool SendData(WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	virtual bool SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//辅助函数
protected:
	//连接消息
	bool OnSocketLink(INT nErrorCode);
	//关闭消息
	bool OnSocketShut(BYTE cbShutReason);
	//读取消息
	bool OnSocketRead(TCP_Command Command, VOID * pData, WORD wDataSize);

	//内部函数
private:
	//地址解释
	DWORD TranslateAddress(LPCTSTR szServerIP);
};

//////////////////////////////////////////////////////////////////////////

#endif