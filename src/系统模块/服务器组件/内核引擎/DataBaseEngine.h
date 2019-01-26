#pragma once

#include "KernelEngineHead.h"
#include "AsynchronismEngine.h"

//////////////////////////////////////////////////////////////////////////

//ADO ������
class CDataBaseException : public IDataBaseException
{
	//��������
protected:
	HRESULT							m_hResult;							//�쳣����
	enSQLException					m_SQLException;						//�쳣����

	//��������
protected:
	CString							m_strDescribe;						//�쳣��Ϣ

	//��������
public:
	//���캯��
	CDataBaseException();
	//��������
	virtual ~CDataBaseException();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ܽӿ�
public:
	//�������
	virtual HRESULT GetExceptionResult() { return m_hResult; }
	//��������
	virtual LPCTSTR GetExceptionDescribe() { return m_strDescribe; }
	//��������
	virtual enSQLException GetExceptionType() { return m_SQLException; }

	//���ܺ���
public:
	//���ô���
	VOID SetExceptionInfo(enSQLException SQLException, CComError * pComError);
};

//////////////////////////////////////////////////////////////////////////

//���ݿ����
class CDataBase : public IDataBase
{
	//������Ϣ
protected:
	CString							m_strConnect;						//�����ַ�
	CDataBaseException				m_DataBaseException;				//�������

	//״̬����
protected:
	DWORD							m_dwConnectCount;					//���Դ���
	DWORD							m_dwConnectErrorTime;				//����ʱ��
	const DWORD						m_dwResumeConnectTime;				//�ָ�ʱ��
	const DWORD						m_dwResumeConnectCount;				//�ָ�����

	//�ں˱���
protected:
	_CommandPtr						m_DBCommand;						//�������
	_RecordsetPtr					m_DBRecordset;						//��¼����
	_ConnectionPtr					m_DBConnection;						//���ݶ���

	//��������
public:
	//���캯��
	CDataBase();
	//��������
	virtual ~CDataBase();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ӽӿ�
public:
	//������
	virtual VOID OpenConnection();
	//�ر�����
	virtual VOID CloseConnection();
	//������Ϣ
	virtual bool SetConnectionInfo(DWORD dwDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword);
	//������Ϣ
	virtual bool SetConnectionInfo(LPCTSTR szDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword);

	//�����ӿ�
public:
	//�������
	virtual VOID ClearParameters();
	//��ȡ����
	virtual VOID GetParameter(LPCTSTR pszParamName, CDBVarValue & DBVarValue);
	//�������
	virtual VOID AddParameter(LPCTSTR pszName, DataTypeEnum Type, ParameterDirectionEnum Direction, LONG lSize, CDBVarValue & DBVarValue);

	//���ƽӿ�
public:
	//�л���¼
	virtual VOID NextRecordset();
	//�رռ�¼
	virtual VOID CloseRecordset();
	//�󶨶���
	virtual VOID BindToRecordset(CADORecordBinding * pBind);

	//��¼�ӿ�
public:
	//�����ƶ�
	virtual VOID MoveToNext();
	//�Ƶ���ͷ
	virtual VOID MoveToFirst();
	//�Ƿ����
	virtual bool IsRecordsetEnd();
	//��ȡ��Ŀ
	virtual LONG GetRecordCount();
	//������ֵ
	virtual LONG GetReturnValue();
	//��ȡ����
	virtual VOID GetRecordsetValue(LPCTSTR pszItem, CDBVarValue & DBVarValue);

	//���ƽӿ�
public:
	//�洢����
	virtual VOID ExecuteProcess(LPCTSTR pszSPName, bool bRecordset);
	//ִ�����
	virtual VOID ExecuteSentence(LPCTSTR pszCommand, bool bRecordset);

	//�ڲ�����
protected:
	//���Ӵ���
	bool IsConnectError();
	//�Ƿ��
	bool IsRecordsetOpened();
	//��������
	bool TryConnectAgain(CComError * pComError);
	//ת���ַ�
	VOID ConvertToSQLSyntax(LPCTSTR pszString, CString & strResult);
	//������
	VOID OnSQLException(enSQLException SQLException, CComError * pComError);
};

//////////////////////////////////////////////////////////////////////////

//���ݿ�������
class CDataBaseEngine : public IDataBaseEngine, public IAsynchronismEngineSink
{
	//�������
protected:
	CCriticalSection				m_CriticalLocker;					//��������
	CAsynchronismEngine				m_AsynchronismEngine;				//�첽����

	//�ӿڱ���
protected:
	IDataBaseEngineSink *			m_pIDataBaseEngineSink;				//���ӽӿ�

	//��������
protected:
	BYTE							m_cbBuffer[MAX_ASYNCHRONISM_DATA];	//��ʱ����

	//��������
public:
	//���캯��
	CDataBaseEngine();
	//��������
	virtual ~CDataBaseEngine();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();

	//���渺��
	virtual bool GetBurthenInfo(tagBurthenInfo & BurthenInfo);

	//���ýӿ�
public:
	//����ģ��
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx);
	//����ģ��
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx[], WORD wSinkCount);

	//�����¼�
public:
	//�����¼�
	virtual bool PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool DeferDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAsynchronismEngineStart();
	//ֹͣ�¼�
	virtual bool OnAsynchronismEngineConclude();
	//�첽����
	virtual bool OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////
