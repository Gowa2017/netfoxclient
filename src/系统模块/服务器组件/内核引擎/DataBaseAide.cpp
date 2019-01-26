#include "StdAfx.h"
#include "DataBaseAide.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseAide::CDataBaseAide(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIDataBase=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBase);

	return;
}

//析构函数
CDataBaseAide::~CDataBaseAide()
{
}

//设置对象
bool CDataBaseAide::SetDataBase(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBase)!=NULL);
		m_pIDataBase=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBase);

		//成功判断
		if (m_pIDataBase==NULL) return false;
	}
	else m_pIDataBase=NULL;

	return true;
}

//获取对象
VOID * CDataBaseAide::GetDataBase(REFGUID Guid, DWORD dwQueryVer)
{
	if (m_pIDataBase==NULL) return NULL;
	return m_pIDataBase->QueryInterface(Guid,dwQueryVer);
}

//获取数据
INT CDataBaseAide::GetValue_INT(LPCTSTR pszItem)
{
	//效验参数
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//获取数据
UINT CDataBaseAide::GetValue_UINT(LPCTSTR pszItem)
{
	//效验参数
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//获取数据
LONG CDataBaseAide::GetValue_LONG(LPCTSTR pszItem)
{
	//效验参数
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//获取数据
BYTE CDataBaseAide::GetValue_BYTE(LPCTSTR pszItem)
{
	//效验参数
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//获取数据
WORD CDataBaseAide::GetValue_WORD(LPCTSTR pszItem)
{
	//效验参数
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//获取数据
DWORD CDataBaseAide::GetValue_DWORD(LPCTSTR pszItem)
{
	//效验参数
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//获取数据
FLOAT CDataBaseAide::GetValue_FLOAT(LPCTSTR pszItem)
{
	//效验参数
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//获取数据
DOUBLE CDataBaseAide::GetValue_DOUBLE(LPCTSTR pszItem)
{
	//效验参数
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//获取数据
LONGLONG CDataBaseAide::GetValue_LONGLONG(LPCTSTR pszItem)
{
	//效验参数
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//获取数据
VOID CDataBaseAide::GetValue_VarValue(LPCTSTR pszItem, CDBVarValue & DBVarValue)
{
	//效验参数
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return;
}

//获取数据
VOID CDataBaseAide::GetValue_SystemTime(LPCTSTR pszItem, SYSTEMTIME & SystemTime)
{
	//效验参数
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	ZeroMemory(&SystemTime,sizeof(SystemTime));
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	//转换变量
	COleDateTime OleDateTime=DBVarValue;
	SystemTime.wYear=OleDateTime.GetYear();
	SystemTime.wMonth=OleDateTime.GetMonth();
	SystemTime.wDayOfWeek=OleDateTime.GetDayOfWeek();
	SystemTime.wDay=OleDateTime.GetDay();
	SystemTime.wHour=OleDateTime.GetHour();
	SystemTime.wMinute=OleDateTime.GetMinute();
	SystemTime.wSecond=OleDateTime.GetSecond();

	return;
}

//获取字符
VOID CDataBaseAide::GetValue_String(LPCTSTR pszItem, LPSTR pszString, UINT uMaxCount)
{
	//效验参数
	ASSERT(pszString!=NULL);
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	//转换变量
	switch (DBVarValue.vt)
	{
	case VT_BSTR:
		{
			CW2A DBString(DBVarValue.bstrVal);
			lstrcpynA(pszString,DBString,uMaxCount);
			return;
		}
	default:
		{
			pszString[0]=0;
			return;
		}
	}
	
	return;
}

//获取字符
VOID CDataBaseAide::GetValue_String(LPCTSTR pszItem, LPWSTR pszString, UINT uMaxCount)
{
	//效验参数
	ASSERT(pszString!=NULL);
	ASSERT(m_pIDataBase!=NULL);

	//读取变量
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	//转换变量
	switch (DBVarValue.vt)
	{
	case VT_BSTR:
		{
			lstrcpynW(pszString,DBVarValue.bstrVal,uMaxCount);
			return;
		}
	default:
		{
			pszString[0]=0;
			return;
		}
	}
	
	return;
}

//重置参数
VOID CDataBaseAide::ResetParameter()
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->ClearParameters();
	m_pIDataBase->AddParameter(TEXT("RETURN_VALUE"),adInteger,adParamReturnValue,sizeof(LONG),CDBVarValue((LONG)0));
	return;
}

//获取参数
VOID CDataBaseAide::GetParameter(LPCTSTR pszItem, CDBVarValue & DBVarValue)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->GetParameter(pszItem,DBVarValue);
	return;
}

//获取参数
VOID CDataBaseAide::GetParameter(LPCTSTR pszItem, LPSTR pszString, UINT uSize)
{
	//变量定义
	CDBVarValue DBVarValue;

	//获取参数
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->GetParameter(pszItem,DBVarValue);

	//设置结果
	lstrcpynA(pszString,(LPCSTR)(CW2A(DBVarValue.bstrVal)),uSize);

	return;
}

//获取参数
VOID CDataBaseAide::GetParameter(LPCTSTR pszItem, LPWSTR pszString, UINT uSize)
{
	//变量定义
	CDBVarValue DBVarValue;

	//获取参数
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->GetParameter(pszItem,DBVarValue);

	//设置结果
	lstrcpynW(pszString,(LPCWSTR)(DBVarValue.bstrVal),uSize);

	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, INT nValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adInteger,ParameterDirection,sizeof(LONG),CDBVarValue((LONG)nValue));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, UINT uValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adInteger,ParameterDirection,sizeof(LONG),CDBVarValue((LONG)uValue));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, LONG lValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adInteger,ParameterDirection,sizeof(LONG),CDBVarValue((LONG)lValue));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, LONGLONG lValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adBigInt,ParameterDirection,sizeof(LONGLONG),CDBVarValue((LONGLONG)lValue));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, BYTE cbValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adTinyInt,ParameterDirection,sizeof(BYTE),CDBVarValue((BYTE)cbValue));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, WORD wValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adInteger,ParameterDirection,sizeof(LONG),CDBVarValue((LONG)wValue));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, DWORD dwValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adInteger,ParameterDirection,sizeof(LONG),CDBVarValue((LONG)dwValue));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, FLOAT fValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adDouble,ParameterDirection,sizeof(DOUBLE),CDBVarValue((DOUBLE)fValue));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, DOUBLE dValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adDouble,ParameterDirection,sizeof(DOUBLE),CDBVarValue((DOUBLE)dValue));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, LPCSTR pszString, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adVarChar,ParameterDirection,CountStringBufferA(pszString),CDBVarValue(pszString));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, LPCWSTR pszString, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adVarChar,ParameterDirection,CountStringBufferW(pszString),_variant_t(pszString));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, SYSTEMTIME & SystemTime, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);

	double dNow; 
    SystemTimeToVariantTime(&SystemTime, &dNow); 
    _variant_t vtEnrollmentDate(dNow, VT_DATE); 
	m_pIDataBase->AddParameter(pszItem,adDate,ParameterDirection,-1,vtEnrollmentDate);
	return;
}

//插入参数
VOID CDataBaseAide::AddParameterOutput(LPCTSTR pszItem, LPSTR pszString, UINT uSize, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adVarChar,ParameterDirection,uSize,CDBVarValue(pszString));
	return;
}

//插入参数
VOID CDataBaseAide::AddParameterOutput(LPCTSTR pszItem, LPWSTR pszString, UINT uSize, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adVarChar,ParameterDirection,uSize,CDBVarValue(pszString));
	return;
}  
LONG CDataBaseAide::GetReturnValue()
{
	ASSERT(m_pIDataBase!=NULL);
	return m_pIDataBase->GetReturnValue();
}

//存储过程
LONG CDataBaseAide::ExecuteProcess(LPCTSTR pszSPName, bool bRecordset)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->ExecuteProcess(pszSPName,bRecordset);
	return m_pIDataBase->GetReturnValue();
}

//////////////////////////////////////////////////////////////////////////
