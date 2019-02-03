#include "StdAfx.h"
#include "GamePropertyListManager.h "

//////////////////////////////////////////////////////////////////////////////////


CGamePropertyListItem::CGamePropertyListItem()
{
	m_bDisuse=false;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGamePropertyTypeItem::CGamePropertyTypeItem()
{
	//设置变量
	ZeroMemory(&m_PropertyTypeItem,sizeof(m_PropertyTypeItem));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGamePropertyRelatItem::CGamePropertyRelatItem()
{
	//设置变量
	ZeroMemory(&m_PropertyRelatItem,sizeof(m_PropertyRelatItem));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGamePropertyItem::CGamePropertyItem()
{
	//设置变量
	ZeroMemory(&m_PropertyItem,sizeof(m_PropertyItem));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGamePropertySubItem::CGamePropertySubItem()
{
	//设置变量
	ZeroMemory(&m_PropertySubItem,sizeof(m_PropertySubItem));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGamePropertyListManager::CGamePropertyListManager()
{
	//设置质数
	m_GamePropertyTypeItemMap.InitHashTable(PRIME_TYPE);
	m_GamePropertyRelatItemMap.InitHashTable(PRIME_KIND);
	m_GamePropertyItemMap.InitHashTable(PRIME_SERVER);
	m_GamePropertySubItemMap.InitHashTable(PRIME_KIND);

	return;
}

//析构函数
CGamePropertyListManager::~CGamePropertyListManager()
{
	//变量定义
	DWORD wKey=0;

	//删除种类
	CGamePropertyTypeItem * pGamePropertyTypeItem=NULL;
	POSITION Position=m_GamePropertyTypeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GamePropertyTypeItemMap.GetNextAssoc(Position,wKey,pGamePropertyTypeItem);
		SafeDelete(pGamePropertyTypeItem);
	}
	for (INT_PTR i=0;i<m_GamePropertyTypeItemArray.GetCount();i++)
	{
		pGamePropertyTypeItem=m_GamePropertyTypeItemArray[i];
		SafeDelete(pGamePropertyTypeItem);
	}
	m_GamePropertyTypeItemMap.RemoveAll();
	m_GamePropertyTypeItemArray.RemoveAll();

	//删除关系
	CGamePropertyRelatItem * pGamePropertyRelatItem=NULL;
	Position=m_GamePropertyRelatItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GamePropertyRelatItemMap.GetNextAssoc(Position,wKey,pGamePropertyRelatItem);
		SafeDelete(pGamePropertyRelatItem);
	}
	for (INT_PTR i=0;i<m_GamePropertyRelatItemArray.GetCount();i++)
	{
		pGamePropertyRelatItem=m_GamePropertyRelatItemArray[i];
		SafeDelete(pGamePropertyRelatItem);
	}
	m_GamePropertyRelatItemMap.RemoveAll();
	m_GamePropertyRelatItemArray.RemoveAll();

	//删除道具
	CGamePropertyItem * pGamePropertyItem=NULL;
	Position=m_GamePropertyItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GamePropertyItemMap.GetNextAssoc(Position,wKey,pGamePropertyItem);
		SafeDelete(pGamePropertyItem);
	}
	for (INT_PTR i=0;i<m_GamePropertyItemArray.GetCount();i++)
	{
		pGamePropertyItem=m_GamePropertyItemArray[i];
		SafeDelete(pGamePropertyItem);
	}
	m_GamePropertyItemMap.RemoveAll();
	m_GamePropertyItemArray.RemoveAll();

	//删除子道具
	CGamePropertySubItem * pGamePropertySubItem=NULL;
	Position=m_GamePropertySubItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GamePropertySubItemMap.GetNextAssoc(Position,wKey,pGamePropertySubItem);
		SafeDelete(pGamePropertySubItem);
	}
	for (INT_PTR i=0;i<m_GamePropertySubItemArray.GetCount();i++)
	{
		pGamePropertySubItem=m_GamePropertySubItemArray[i];
		SafeDelete(pGamePropertySubItem);
	}
	m_GamePropertySubItemMap.RemoveAll();
	m_GamePropertySubItemArray.RemoveAll();

	return;
}

//重置列表
VOID CGamePropertyListManager::ResetPropertyListManager()
{
	//废弃种类
	DisusePropertyTypeItem();

	//废弃关系
	DisusePropertyRelatItem();

	//废弃道具
	DisusePropertySubItem();

	//废弃道具
	DisusePropertyItem();

	//清理种类
	CleanPropertyTypeItem();

	//清理关系
	CleanPropertyRelatItem();

	//清理道具
	CleanPropertyItem();

	//清理道具
	CleanPropertySubItem();

	return;
}

//清理种类
VOID CGamePropertyListManager::CleanPropertyTypeItem()
{
	POSITION PositionPropertyType=m_GamePropertyTypeItemMap.GetStartPosition();
	while (PositionPropertyType!=NULL)
	{
		DWORD dwTypeID=0;
		CGamePropertyTypeItem * pGamePropertyTypeItem=NULL;
		m_GamePropertyTypeItemMap.GetNextAssoc(PositionPropertyType,dwTypeID,pGamePropertyTypeItem);
		if (pGamePropertyTypeItem->m_bDisuse==true)
		{
			m_GamePropertyTypeItemMap.RemoveKey(dwTypeID);
			m_GamePropertyTypeItemArray.Add(pGamePropertyTypeItem);
		}
	}
	return;
}

//清理关系
VOID CGamePropertyListManager::CleanPropertyRelatItem()
{
	POSITION PositionPropertyRelat=m_GamePropertyRelatItemMap.GetStartPosition();
	while (PositionPropertyRelat!=NULL)
	{

		DWORD dwTypeID=0;
		CGamePropertyRelatItem * pGamePropertyRelatItem=NULL;
		m_GamePropertyRelatItemMap.GetNextAssoc(PositionPropertyRelat,dwTypeID,pGamePropertyRelatItem);

		if (pGamePropertyRelatItem->m_bDisuse==true)
		{
			m_GamePropertyRelatItemMap.RemoveKey(dwTypeID);
			m_GamePropertyRelatItemArray.Add(pGamePropertyRelatItem);
		}
	}

	return;
}

//清理道具
VOID CGamePropertyListManager::CleanPropertyItem()
{
	POSITION PositionProperty=m_GamePropertyItemMap.GetStartPosition();
	while (PositionProperty!=NULL)
	{
		DWORD dwPropertyID=0;
		CGamePropertyItem * pGamePropertyItem=NULL;
		m_GamePropertyItemMap.GetNextAssoc(PositionProperty,dwPropertyID,pGamePropertyItem);
		if (pGamePropertyItem->m_bDisuse==true)
		{
			m_GamePropertyItemMap.RemoveKey(dwPropertyID);
			m_GamePropertyItemArray.Add(pGamePropertyItem);
		}
	}

	return;
}

//清理子道具
VOID CGamePropertyListManager::CleanPropertySubItem()
{
	POSITION PositionPropertySub=m_GamePropertySubItemMap.GetStartPosition();
	while (PositionPropertySub!=NULL)
	{
		DWORD dwPropertyID=0;
		CGamePropertySubItem * pGamePropertySubItem=NULL;
		m_GamePropertySubItemMap.GetNextAssoc(PositionPropertySub,dwPropertyID,pGamePropertySubItem);
		if (pGamePropertySubItem->m_bDisuse==true)
		{
			m_GamePropertySubItemMap.RemoveKey(dwPropertyID);
			m_GamePropertySubItemArray.Add(pGamePropertySubItem);
		}
	}

	return;
}

//废弃种类
VOID CGamePropertyListManager::DisusePropertyTypeItem()
{
	POSITION PositionPropertyType=m_GamePropertyTypeItemMap.GetStartPosition();
	while (PositionPropertyType!=NULL)
	{
		DWORD dwTypeID=0;
		CGamePropertyTypeItem * pGamePropertyTypeItem=NULL;
		m_GamePropertyTypeItemMap.GetNextAssoc(PositionPropertyType,dwTypeID,pGamePropertyTypeItem);
		pGamePropertyTypeItem->m_bDisuse=true;
	}
	return;
}

//废弃关系
VOID CGamePropertyListManager::DisusePropertyRelatItem()
{
	POSITION PositionPropertyRelat=m_GamePropertyRelatItemMap.GetStartPosition();
	while (PositionPropertyRelat!=NULL)
	{

		DWORD dwTypeID=0;
		CGamePropertyRelatItem * pGamePropertyRelatItem=NULL;
		m_GamePropertyRelatItemMap.GetNextAssoc(PositionPropertyRelat,dwTypeID,pGamePropertyRelatItem);
		pGamePropertyRelatItem->m_bDisuse=true;
	}
	return;
}

//废弃道具
VOID CGamePropertyListManager::DisusePropertyItem()
{
	POSITION PositionProperty=m_GamePropertyItemMap.GetStartPosition();
	while (PositionProperty!=NULL)
	{

		DWORD dwPropertyID=0;
		CGamePropertyItem * pGamePropertyItem=NULL;
		m_GamePropertyItemMap.GetNextAssoc(PositionProperty,dwPropertyID,pGamePropertyItem);
		pGamePropertyItem->m_bDisuse=true;
	}
	return;
}

//废弃子道具
VOID CGamePropertyListManager::DisusePropertySubItem()
{
	POSITION PositionPropertySub=m_GamePropertySubItemMap.GetStartPosition();
	while (PositionPropertySub!=NULL)
	{

		DWORD dwPropertyID=0;
		CGamePropertySubItem * pGamePropertySubItem=NULL;
		m_GamePropertySubItemMap.GetNextAssoc(PositionPropertySub,dwPropertyID,pGamePropertySubItem);
		pGamePropertySubItem->m_bDisuse=true;
	}
	return;
}

//插入种类
bool CGamePropertyListManager::InsertGamePropertyTypeItem(tagPropertyTypeItem * ptagPropertyTypeItem)
{
	//效验参数
	ASSERT(ptagPropertyTypeItem!=NULL);
	if (ptagPropertyTypeItem==NULL) return false;

	//查找现存
	CGamePropertyTypeItem * pGamePropertyTypeItem=NULL;
	if (m_GamePropertyTypeItemMap.Lookup(ptagPropertyTypeItem->dwTypeID,pGamePropertyTypeItem)==FALSE)
	{
		//创建对象
		try
		{
			INT_PTR nStroeCount=m_GamePropertyTypeItemArray.GetCount();
			if (nStroeCount>0)
			{
				pGamePropertyTypeItem=m_GamePropertyTypeItemArray[nStroeCount-1];
				m_GamePropertyTypeItemArray.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGamePropertyTypeItem=new CGamePropertyTypeItem;
				if (pGamePropertyTypeItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//设置变量
		ZeroMemory(pGamePropertyTypeItem,sizeof(CGamePropertyTypeItem));
	}

	//设置数据
	CopyMemory(&pGamePropertyTypeItem->m_PropertyTypeItem,ptagPropertyTypeItem,sizeof(tagPropertyTypeItem));

	//设置索引
	m_GamePropertyTypeItemMap[ptagPropertyTypeItem->dwTypeID]=pGamePropertyTypeItem;

	return true;
}

//插入关系
bool CGamePropertyListManager::InsertGamePropertyRelatItem(tagPropertyRelatItem * ptagPropertyRelatItem)
{
	//效验参数
	ASSERT(ptagPropertyRelatItem!=NULL);
	if (ptagPropertyRelatItem==NULL) return false;

	//查找现存
	CGamePropertyRelatItem * pPropertyRelatItem=NULL;

	bool bFinder = false;
	DWORD  dwkey= 0;
	DWORD  dwCurrKey =0;
	POSITION Position=NULL;
	while (true)
	{
		if (Position==NULL) Position=m_GamePropertyRelatItemMap.GetStartPosition();
		if (Position!=NULL) m_GamePropertyRelatItemMap.GetNextAssoc(Position,dwkey,pPropertyRelatItem);

		if (pPropertyRelatItem==NULL) break;

		if (pPropertyRelatItem->m_PropertyRelatItem.dwPropertyID == ptagPropertyRelatItem->dwPropertyID
			&& pPropertyRelatItem->m_PropertyRelatItem.dwTypeID == ptagPropertyRelatItem->dwTypeID)
		{
			bFinder = true;
			break;
		}
		dwCurrKey++;

		if (Position==NULL) break;
	}

	//获取子项
	if (bFinder == false)
	{
		//创建对象
		try
		{
			pPropertyRelatItem=new CGamePropertyRelatItem;
			if (pPropertyRelatItem==NULL) return false;
		}
		catch (...) { return false; }

		//设置变量
		ZeroMemory(pPropertyRelatItem,sizeof(CGamePropertyRelatItem));
		dwCurrKey++;
	}
	else
	{
		dwCurrKey = dwkey;
	}

	//设置数据
	CopyMemory(&pPropertyRelatItem->m_PropertyRelatItem,ptagPropertyRelatItem,sizeof(tagPropertyRelatItem));

	//设置索引
	m_GamePropertyRelatItemMap[dwCurrKey]=pPropertyRelatItem;

	return true;
}


//插入道具
bool CGamePropertyListManager::InsertGamePropertyItem(tagPropertyItem * ptagPropertyItem)
{
	//效验参数
	ASSERT(ptagPropertyItem!=NULL);
	if (ptagPropertyItem==NULL) return false;

	//查找现存
	CGamePropertyItem * pGamePropertyItem=NULL;

	//获取子项
	if (m_GamePropertyItemMap.Lookup(ptagPropertyItem->dwPropertyID,pGamePropertyItem)==FALSE)
	{
		//创建对象
		try
		{
			INT_PTR nStroeCount=m_GamePropertyItemArray.GetCount();
			if (nStroeCount>0)
			{
				pGamePropertyItem=m_GamePropertyItemArray[nStroeCount-1];
				m_GamePropertyItemArray.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGamePropertyItem=new CGamePropertyItem;
				if (pGamePropertyItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//设置变量
		ZeroMemory(pGamePropertyItem,sizeof(CGamePropertyItem));
	}


	//设置数据
	CopyMemory(&pGamePropertyItem->m_PropertyItem,ptagPropertyItem,sizeof(tagPropertyItem));

	//设置索引
	m_GamePropertyItemMap[ptagPropertyItem->dwPropertyID]=pGamePropertyItem;

	return true;
}

//插入子道具
bool CGamePropertyListManager::InsertGamePropertySubItem(tagPropertySubItem * ptagPropertySubItem)
{
	//效验参数
	ASSERT(ptagPropertySubItem!=NULL);
	if (ptagPropertySubItem==NULL) return false;

	//查找现存
	CGamePropertySubItem * pGamePropertySubItem=NULL;

	bool bFinder = false;
	DWORD  dwkey= 0;
	DWORD  dwCurrKey =0;
	POSITION Position=NULL;
	while (true)
	{
		if (Position==NULL) Position=m_GamePropertySubItemMap.GetStartPosition();
		if (Position!=NULL) m_GamePropertySubItemMap.GetNextAssoc(Position,dwkey,pGamePropertySubItem);

		if (pGamePropertySubItem==NULL) break;

		if (pGamePropertySubItem->m_PropertySubItem.dwPropertyID == ptagPropertySubItem->dwPropertyID
			&& pGamePropertySubItem->m_PropertySubItem.dwOwnerPropertyID == ptagPropertySubItem->dwOwnerPropertyID)
		{
			bFinder = true;
			break;
		}
		dwCurrKey++;

		if (Position==NULL) break;
	}

	//获取子项
	if (bFinder == false)
	{
		//创建对象
		try
		{
			pGamePropertySubItem=new CGamePropertySubItem;
			if (pGamePropertySubItem==NULL) return false;
		}
		catch (...) { return false; }

		//设置变量
		ZeroMemory(pGamePropertySubItem,sizeof(CGamePropertySubItem));
		dwCurrKey++;
	}
	else
	{
		dwCurrKey = dwkey;
	}

	//设置数据
	CopyMemory(&pGamePropertySubItem->m_PropertySubItem,ptagPropertySubItem,sizeof(tagPropertySubItem));

	//设置索引
	m_GamePropertySubItemMap[dwCurrKey]=pGamePropertySubItem;

	return true;
}

//删除种类
bool CGamePropertyListManager::DeleteGamePropertyTypeItem(DWORD dwTypeID)
{
	//查找种类
	CGamePropertyTypeItem * pGamePropertyTypeItem=NULL;
	if (m_GamePropertyTypeItemMap.Lookup(dwTypeID,pGamePropertyTypeItem)==FALSE) return false;

	//删除数据
	m_GamePropertyTypeItemMap.RemoveKey(dwTypeID);

	return true;
}

//删除道具
bool CGamePropertyListManager::DeleteGamePropertyItem(DWORD dwPropertyID)
{
	//查找道具
	CGamePropertyItem * pGamePropertyItem=NULL;
	if (m_GamePropertyItemMap.Lookup(dwPropertyID,pGamePropertyItem)==FALSE) return false;

	//删除数据
	m_GamePropertyItemMap.RemoveKey(dwPropertyID);
	m_GamePropertyItemArray.Add(pGamePropertyItem);

	return true;
}

//枚举种类
CGamePropertyTypeItem * CGamePropertyListManager::EmunGamePropertyTypeItem(POSITION & Position)
{
	//变量定义
	DWORD wKey=0;
	CGamePropertyTypeItem * pGamePropertyTypeItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_GamePropertyTypeItemMap.GetStartPosition();
	if (Position!=NULL) m_GamePropertyTypeItemMap.GetNextAssoc(Position,wKey,pGamePropertyTypeItem);

	return pGamePropertyTypeItem;
}

//枚举道具
CGamePropertyRelatItem * CGamePropertyListManager::EmunGamePropertyRelatItem(POSITION & Position)
{
	//变量定义
	DWORD wKey=0;
	CGamePropertyRelatItem * pGamePropertyRelatItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_GamePropertyRelatItemMap.GetStartPosition();
	if (Position!=NULL) m_GamePropertyRelatItemMap.GetNextAssoc(Position,wKey,pGamePropertyRelatItem);

	return pGamePropertyRelatItem;
}

//枚举道具
CGamePropertyItem * CGamePropertyListManager::EmunGamePropertyItem(POSITION & Position)
{
	//变量定义
	DWORD wKey=0;
	CGamePropertyItem * pGamePropertyItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_GamePropertyItemMap.GetStartPosition();
	if (Position!=NULL) m_GamePropertyItemMap.GetNextAssoc(Position,wKey,pGamePropertyItem);

	return pGamePropertyItem;
}

//枚举子道具
CGamePropertySubItem * CGamePropertyListManager::EmunGamePropertySubItem(POSITION & Position)
{
	//变量定义
	DWORD wKey=0;
	CGamePropertySubItem * pGamePropertySubItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_GamePropertySubItemMap.GetStartPosition();
	if (Position!=NULL) m_GamePropertySubItemMap.GetNextAssoc(Position,wKey,pGamePropertySubItem);

	return pGamePropertySubItem;
}

//查找道具
CGamePropertyItem * CGamePropertyListManager::SearchGamePropertyItem(DWORD dwPropertyID)
{
	CGamePropertyItem * pGamePropertyItem=NULL;
	m_GamePropertyItemMap.Lookup(dwPropertyID,pGamePropertyItem);
	return pGamePropertyItem;
}


//////////////////////////////////////////////////////////////////////////////////
