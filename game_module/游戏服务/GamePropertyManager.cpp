#include "StdAfx.h"
#include "GamePropertyManager.h"


bool IsValid(const tagPropertyBuff PropertyBuff)
{
	return (PropertyBuff.tUseTime+PropertyBuff.UseResultsValidTime) > (DWORD)time(NULL);
}

//////////////////////////////////////////////////////////////////////////////////

CPropertyBuffMAP CGamePropertyManager::m_PropertyBuffMap;

//构造函数
CGamePropertyManager::CGamePropertyManager()
{
}

//析构函数
CGamePropertyManager::~CGamePropertyManager()
{
}

//设置道具
bool CGamePropertyManager::SetGamePropertyInfo(tagPropertyInfo PropertyInfo[], WORD wPropertyCount)
{
	//设置变量
	m_PropertyInfoArray.SetSize(wPropertyCount);

	//拷贝数组
	CopyMemory(m_PropertyInfoArray.GetData(),PropertyInfo,sizeof(tagPropertyInfo)*wPropertyCount);

	return true;
}

bool CGamePropertyManager::SetGamePropertyBuff(const DWORD dwUserID, const tagPropertyBuff PropertyBuff[], const WORD wBuffCount)
{
	if(dwUserID <= 0) return false;

	//保存玩家Buff
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

//查找道具
tagPropertyInfo * CGamePropertyManager::SearchPropertyItem(WORD wPropertyIndex)
{
	//查找道具
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
	//同种功能只返回 倍数最佳的道具Buff
	int index = -1;
	int dwMaxScoreMultiple = 0;	//最高倍数
	std::vector<tagPropertyBuff> vecBuff(CGamePropertyManager::m_PropertyBuffMap[dwUserID]);
	int nBuffCount = (int)vecBuff.size();
	for(int i = 0; i < nBuffCount; i++)
	{
		if( (PROP_KIND)vecBuff[i].dwKind == Kind && IsValid(vecBuff[i]) ) //时间有效判断
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
