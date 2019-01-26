#include "StdAfx.h"
#include "GamePropertyManager.h"


bool IsValid(const tagPropertyBuff PropertyBuff)
{
	return (PropertyBuff.tUseTime+PropertyBuff.UseResultsValidTime) > (DWORD)time(NULL);
}

//////////////////////////////////////////////////////////////////////////////////

CPropertyBuffMAP CGamePropertyManager::m_PropertyBuffMap;

//���캯��
CGamePropertyManager::CGamePropertyManager()
{
}

//��������
CGamePropertyManager::~CGamePropertyManager()
{
}

//���õ���
bool CGamePropertyManager::SetGamePropertyInfo(tagPropertyInfo PropertyInfo[], WORD wPropertyCount)
{
	//���ñ���
	m_PropertyInfoArray.SetSize(wPropertyCount);

	//��������
	CopyMemory(m_PropertyInfoArray.GetData(),PropertyInfo,sizeof(tagPropertyInfo)*wPropertyCount);

	return true;
}

bool CGamePropertyManager::SetGamePropertyBuff(const DWORD dwUserID, const tagPropertyBuff PropertyBuff[], const WORD wBuffCount)
{
	if(dwUserID <= 0) return false;

	//�������Buff
	for(int i = 0; i < wBuffCount; i++)
	{
		CGamePropertyManager::m_PropertyBuffMap[dwUserID].push_back( PropertyBuff[i] );
	}
	return true;
}

bool CGamePropertyManager::ClearUserBuff(const DWORD dwUserID)
{
	if( CGamePropertyManager::m_PropertyBuffMap[dwUserID].empty() ) return false;

	CGamePropertyManager::m_PropertyBuffMap[dwUserID].clear();
	return true;
}

//���ҵ���
tagPropertyInfo * CGamePropertyManager::SearchPropertyItem(WORD wPropertyIndex)
{
	//���ҵ���
	for (INT_PTR i=0;i<m_PropertyInfoArray.GetCount();i++)
	{
		if (m_PropertyInfoArray[i].wIndex==wPropertyIndex)
		{
			return &m_PropertyInfoArray[i];
		}
	}

	return NULL;
}

tagPropertyBuff* CGamePropertyManager::SearchValidPropertyBuff(const DWORD dwUserID, const PROP_KIND Kind)
{
	//ͬ�ֹ���ֻ���� ������ѵĵ���Buff
	int index = -1;
	int dwMaxScoreMultiple = 0;	//��߱���
	std::vector<tagPropertyBuff> vecBuff(CGamePropertyManager::m_PropertyBuffMap[dwUserID]);
	int nBuffCount = (int)vecBuff.size();
	for(int i = 0; i < nBuffCount; i++)
	{
		if( (PROP_KIND)vecBuff[i].dwKind == Kind && IsValid(vecBuff[i]) ) //ʱ����Ч�ж�
		{
			int dwScoreMultiple = vecBuff[i].dwScoreMultiple;
			if(dwScoreMultiple >= dwMaxScoreMultiple )
			{
				dwMaxScoreMultiple = dwScoreMultiple;
				index = i;
			}
		}
	}
	if(index != -1) return &CGamePropertyManager::m_PropertyBuffMap[dwUserID][index];
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
