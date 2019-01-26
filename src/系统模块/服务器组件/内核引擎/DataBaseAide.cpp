#include "StdAfx.h"
#include "DataBaseAide.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseAide::CDataBaseAide(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pIDataBase=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBase);

	return;
}

//��������
CDataBaseAide::~CDataBaseAide()
{
}

//���ö���
bool CDataBaseAide::SetDataBase(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBase)!=NULL);
		m_pIDataBase=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBase);

		//�ɹ��ж�
		if (m_pIDataBase==NULL) return false;
	}
	else m_pIDataBase=NULL;

	return true;
}

//��ȡ����
VOID * CDataBaseAide::GetDataBase(REFGUID Guid, DWORD dwQueryVer)
{
	if (m_pIDataBase==NULL) return NULL;
	return m_pIDataBase->QueryInterface(Guid,dwQueryVer);
}

//��ȡ����
INT CDataBaseAide::GetValue_INT(LPCTSTR pszItem)
{
	//Ч�����
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//��ȡ����
UINT CDataBaseAide::GetValue_UINT(LPCTSTR pszItem)
{
	//Ч�����
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//��ȡ����
LONG CDataBaseAide::GetValue_LONG(LPCTSTR pszItem)
{
	//Ч�����
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//��ȡ����
BYTE CDataBaseAide::GetValue_BYTE(LPCTSTR pszItem)
{
	//Ч�����
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//��ȡ����
WORD CDataBaseAide::GetValue_WORD(LPCTSTR pszItem)
{
	//Ч�����
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//��ȡ����
DWORD CDataBaseAide::GetValue_DWORD(LPCTSTR pszItem)
{
	//Ч�����
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//��ȡ����
FLOAT CDataBaseAide::GetValue_FLOAT(LPCTSTR pszItem)
{
	//Ч�����
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//��ȡ����
DOUBLE CDataBaseAide::GetValue_DOUBLE(LPCTSTR pszItem)
{
	//Ч�����
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//��ȡ����
LONGLONG CDataBaseAide::GetValue_LONGLONG(LPCTSTR pszItem)
{
	//Ч�����
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return DBVarValue;
}

//��ȡ����
VOID CDataBaseAide::GetValue_VarValue(LPCTSTR pszItem, CDBVarValue & DBVarValue)
{
	//Ч�����
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	return;
}

//��ȡ����
VOID CDataBaseAide::GetValue_SystemTime(LPCTSTR pszItem, SYSTEMTIME & SystemTime)
{
	//Ч�����
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	ZeroMemory(&SystemTime,sizeof(SystemTime));
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	//ת������
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

//��ȡ�ַ�
VOID CDataBaseAide::GetValue_String(LPCTSTR pszItem, LPSTR pszString, UINT uMaxCount)
{
	//Ч�����
	ASSERT(pszString!=NULL);
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	//ת������
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

//��ȡ�ַ�
VOID CDataBaseAide::GetValue_String(LPCTSTR pszItem, LPWSTR pszString, UINT uMaxCount)
{
	//Ч�����
	ASSERT(pszString!=NULL);
	ASSERT(m_pIDataBase!=NULL);

	//��ȡ����
	CDBVarValue DBVarValue;
	m_pIDataBase->GetRecordsetValue(pszItem,DBVarValue);

	//ת������
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

//���ò���
VOID CDataBaseAide::ResetParameter()
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->ClearParameters();
	m_pIDataBase->AddParameter(TEXT("RETURN_VALUE"),adInteger,adParamReturnValue,sizeof(LONG),CDBVarValue((LONG)0));
	return;
}

//��ȡ����
VOID CDataBaseAide::GetParameter(LPCTSTR pszItem, CDBVarValue & DBVarValue)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->GetParameter(pszItem,DBVarValue);
	return;
}

//��ȡ����
VOID CDataBaseAide::GetParameter(LPCTSTR pszItem, LPSTR pszString, UINT uSize)
{
	//��������
	CDBVarValue DBVarValue;

	//��ȡ����
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->GetParameter(pszItem,DBVarValue);

	//���ý��
	lstrcpynA(pszString,(LPCSTR)(CW2A(DBVarValue.bstrVal)),uSize);

	return;
}

//��ȡ����
VOID CDataBaseAide::GetParameter(LPCTSTR pszItem, LPWSTR pszString, UINT uSize)
{
	//��������
	CDBVarValue DBVarValue;

	//��ȡ����
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->GetParameter(pszItem,DBVarValue);

	//���ý��
	lstrcpynW(pszString,(LPCWSTR)(DBVarValue.bstrVal),uSize);

	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, INT nValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adInteger,ParameterDirection,sizeof(LONG),CDBVarValue((LONG)nValue));
	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, UINT uValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adInteger,ParameterDirection,sizeof(LONG),CDBVarValue((LONG)uValue));
	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, LONG lValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adInteger,ParameterDirection,sizeof(LONG),CDBVarValue((LONG)lValue));
	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, LONGLONG lValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adBigInt,ParameterDirection,sizeof(LONGLONG),CDBVarValue((LONGLONG)lValue));
	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, BYTE cbValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adTinyInt,ParameterDirection,sizeof(BYTE),CDBVarValue((BYTE)cbValue));
	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, WORD wValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adInteger,ParameterDirection,sizeof(LONG),CDBVarValue((LONG)wValue));
	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, DWORD dwValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adInteger,ParameterDirection,sizeof(LONG),CDBVarValue((LONG)dwValue));
	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, FLOAT fValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adDouble,ParameterDirection,sizeof(DOUBLE),CDBVarValue((DOUBLE)fValue));
	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, DOUBLE dValue, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adDouble,ParameterDirection,sizeof(DOUBLE),CDBVarValue((DOUBLE)dValue));
	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, LPCSTR pszString, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adVarChar,ParameterDirection,CountStringBufferA(pszString),CDBVarValue(pszString));
	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, LPCWSTR pszString, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adVarChar,ParameterDirection,CountStringBufferW(pszString),_variant_t(pszString));
	return;
}

//�������
VOID CDataBaseAide::AddParameter(LPCTSTR pszItem, SYSTEMTIME & SystemTime, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);

	double dNow; 
    SystemTimeToVariantTime(&SystemTime, &dNow); 
    _variant_t vtEnrollmentDate(dNow, VT_DATE); 
	m_pIDataBase->AddParameter(pszItem,adDate,ParameterDirection,-1,vtEnrollmentDate);
	return;
}

//�������
VOID CDataBaseAide::AddParameterOutput(LPCTSTR pszItem, LPSTR pszString, UINT uSize, ParameterDirectionEnum ParameterDirection)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->AddParameter(pszItem,adVarChar,ParameterDirection,uSize,CDBVarValue(pszString));
	return;
}

//�������
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

//�洢����
LONG CDataBaseAide::ExecuteProcess(LPCTSTR pszSPName, bool bRecordset)
{
	ASSERT(m_pIDataBase!=NULL);
	m_pIDataBase->ExecuteProcess(pszSPName,bRecordset);
	return m_pIDataBase->GetReturnValue();
}

//////////////////////////////////////////////////////////////////////////
