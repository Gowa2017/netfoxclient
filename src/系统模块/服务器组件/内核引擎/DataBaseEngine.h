#pragma once

#include "KernelEngineHead.h"
#include "AsynchronismEngine.h"

//////////////////////////////////////////////////////////////////////////

//ADO 错误类
class CDataBaseException : public IDataBaseException
{
	//错误类型
protected:
	HRESULT							m_hResult;							//异常代码
	enSQLException					m_SQLException;						//异常类型

	//辅助变量
protected:
	CString							m_strDescribe;						//异常信息

	//函数定义
public:
	//构造函数
	CDataBaseException();
	//析构函数
	virtual ~CDataBaseException();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//功能接口
public:
	//错误代号
	virtual HRESULT GetExceptionResult() { return m_hResult; }
	//错误描述
	virtual LPCTSTR GetExceptionDescribe() { return m_strDescribe; }
	//错误类型
	virtual enSQLException GetExceptionType() { return m_SQLException; }

	//功能函数
public:
	//设置错误
	VOID SetExceptionInfo(enSQLException SQLException, CComError * pComError);
};

//////////////////////////////////////////////////////////////////////////

//数据库对象
class CDataBase : public IDataBase
{
	//连接信息
protected:
	CString							m_strConnect;						//连接字符
	CDataBaseException				m_DataBaseException;				//错误对象

	//状态变量
protected:
	DWORD							m_dwConnectCount;					//重试次数
	DWORD							m_dwConnectErrorTime;				//错误时间
	const DWORD						m_dwResumeConnectTime;				//恢复时间
	const DWORD						m_dwResumeConnectCount;				//恢复次数

	//内核变量
protected:
	_CommandPtr						m_DBCommand;						//命令对象
	_RecordsetPtr					m_DBRecordset;						//记录对象
	_ConnectionPtr					m_DBConnection;						//数据对象

	//函数定义
public:
	//构造函数
	CDataBase();
	//析构函数
	virtual ~CDataBase();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//连接接口
public:
	//打开连接
	virtual VOID OpenConnection();
	//关闭连接
	virtual VOID CloseConnection();
	//连接信息
	virtual bool SetConnectionInfo(DWORD dwDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword);
	//连接信息
	virtual bool SetConnectionInfo(LPCTSTR szDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword);

	//参数接口
public:
	//清除参数
	virtual VOID ClearParameters();
	//获取参数
	virtual VOID GetParameter(LPCTSTR pszParamName, CDBVarValue & DBVarValue);
	//插入参数
	virtual VOID AddParameter(LPCTSTR pszName, DataTypeEnum Type, ParameterDirectionEnum Direction, LONG lSize, CDBVarValue & DBVarValue);

	//控制接口
public:
	//切换记录
	virtual VOID NextRecordset();
	//关闭记录
	virtual VOID CloseRecordset();
	//绑定对象
	virtual VOID BindToRecordset(CADORecordBinding * pBind);

	//记录接口
public:
	//往下移动
	virtual VOID MoveToNext();
	//移到开头
	virtual VOID MoveToFirst();
	//是否结束
	virtual bool IsRecordsetEnd();
	//获取数目
	virtual LONG GetRecordCount();
	//返回数值
	virtual LONG GetReturnValue();
	//获取数据
	virtual VOID GetRecordsetValue(LPCTSTR pszItem, CDBVarValue & DBVarValue);

	//控制接口
public:
	//存储过程
	virtual VOID ExecuteProcess(LPCTSTR pszSPName, bool bRecordset);
	//执行语句
	virtual VOID ExecuteSentence(LPCTSTR pszCommand, bool bRecordset);

	//内部函数
protected:
	//连接错误
	bool IsConnectError();
	//是否打开
	bool IsRecordsetOpened();
	//重新连接
	bool TryConnectAgain(CComError * pComError);
	//转换字符
	VOID ConvertToSQLSyntax(LPCTSTR pszString, CString & strResult);
	//错误处理
	VOID OnSQLException(enSQLException SQLException, CComError * pComError);
};

//////////////////////////////////////////////////////////////////////////

//数据库引擎类
class CDataBaseEngine : public IDataBaseEngine, public IAsynchronismEngineSink
{
	//组件变量
protected:
	CCriticalSection				m_CriticalLocker;					//缓冲锁定
	CAsynchronismEngine				m_AsynchronismEngine;				//异步引擎

	//接口变量
protected:
	IDataBaseEngineSink *			m_pIDataBaseEngineSink;				//钩子接口

	//辅助变量
protected:
	BYTE							m_cbBuffer[MAX_ASYNCHRONISM_DATA];	//临时对象

	//函数定义
public:
	//构造函数
	CDataBaseEngine();
	//析构函数
	virtual ~CDataBaseEngine();

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

	//引擎负荷
	virtual bool GetBurthenInfo(tagBurthenInfo & BurthenInfo);

	//配置接口
public:
	//配置模块
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx);
	//配置模块
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx[], WORD wSinkCount);

	//控制事件
public:
	//控制事件
	virtual bool PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//延期请求
	virtual bool DeferDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//异步接口
public:
	//启动事件
	virtual bool OnAsynchronismEngineStart();
	//停止事件
	virtual bool OnAsynchronismEngineConclude();
	//异步数据
	virtual bool OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////
