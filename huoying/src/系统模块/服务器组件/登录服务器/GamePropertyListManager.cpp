#include "StdAfx.h"
#include "GamePropertyListManager.h "

//////////////////////////////////////////////////////////////////////////////////


CGamePropertyListItem::CGamePropertyListItem()
{
	m_bDisuse=false;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGamePropertyTypeItem::CGamePropertyTypeItem()
{
	//���ñ���
	ZeroMemory(&m_PropertyTypeItem,sizeof(m_PropertyTypeItem));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGamePropertyRelatItem::CGamePropertyRelatItem()
{
	//���ñ���
	ZeroMemory(&m_PropertyRelatItem,sizeof(m_PropertyRelatItem));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGamePropertyItem::CGamePropertyItem()
{
	//���ñ���
	ZeroMemory(&m_PropertyItem,sizeof(m_PropertyItem));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGamePropertySubItem::CGamePropertySubItem()
{
	//���ñ���
	ZeroMemory(&m_PropertySubItem,sizeof(m_PropertySubItem));

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGamePropertyListManager::CGamePropertyListManager()
{
	//��������
	m_GamePropertyTypeItemMap.InitHashTable(PRIME_TYPE);
	m_GamePropertyRelatItemMap.InitHashTable(PRIME_KIND);
	m_GamePropertyItemMap.InitHashTable(PRIME_SERVER);
	m_GamePropertySubItemMap.InitHashTable(PRIME_KIND);

	return;
}

//��������
CGamePropertyListManager::~CGamePropertyListManager()
{
	//��������
	DWORD wKey=0;

	//ɾ������
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

	//ɾ����ϵ
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

	//ɾ������
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

	//ɾ���ӵ���
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

//�����б�
VOID CGamePropertyListManager::ResetPropertyListManager()
{
	//��������
	DisusePropertyTypeItem();

	//������ϵ
	DisusePropertyRelatItem();

	//��������
	DisusePropertySubItem();

	//��������
	DisusePropertyItem();

	//��������
	CleanPropertyTypeItem();

	//�����ϵ
	CleanPropertyRelatItem();

	//�������
	CleanPropertyItem();

	//�������
	CleanPropertySubItem();

	return;
}

//��������
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

//�����ϵ
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

//�������
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

//�����ӵ���
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

//��������
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

//������ϵ
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

//��������
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

//�����ӵ���
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

//��������
bool CGamePropertyListManager::InsertGamePropertyTypeItem(tagPropertyTypeItem * ptagPropertyTypeItem)
{
	//Ч�����
	ASSERT(ptagPropertyTypeItem!=NULL);
	if (ptagPropertyTypeItem==NULL) return false;

	//�����ִ�
	CGamePropertyTypeItem * pGamePropertyTypeItem=NULL;
	if (m_GamePropertyTypeItemMap.Lookup(ptagPropertyTypeItem->dwTypeID,pGamePropertyTypeItem)==FALSE)
	{
		//��������
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

		//���ñ���
		ZeroMemory(pGamePropertyTypeItem,sizeof(CGamePropertyTypeItem));
	}

	//��������
	CopyMemory(&pGamePropertyTypeItem->m_PropertyTypeItem,ptagPropertyTypeItem,sizeof(tagPropertyTypeItem));

	//��������
	m_GamePropertyTypeItemMap[ptagPropertyTypeItem->dwTypeID]=pGamePropertyTypeItem;

	return true;
}

//�����ϵ
bool CGamePropertyListManager::InsertGamePropertyRelatItem(tagPropertyRelatItem * ptagPropertyRelatItem)
{
	//Ч�����
	ASSERT(ptagPropertyRelatItem!=NULL);
	if (ptagPropertyRelatItem==NULL) return false;

	//�����ִ�
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

	//��ȡ����
	if (bFinder == false)
	{
		//��������
		try
		{
			pPropertyRelatItem=new CGamePropertyRelatItem;
			if (pPropertyRelatItem==NULL) return false;
		}
		catch (...) { return false; }

		//���ñ���
		ZeroMemory(pPropertyRelatItem,sizeof(CGamePropertyRelatItem));
		dwCurrKey++;
	}
	else
	{
		dwCurrKey = dwkey;
	}

	//��������
	CopyMemory(&pPropertyRelatItem->m_PropertyRelatItem,ptagPropertyRelatItem,sizeof(tagPropertyRelatItem));

	//��������
	m_GamePropertyRelatItemMap[dwCurrKey]=pPropertyRelatItem;

	return true;
}


//�������
bool CGamePropertyListManager::InsertGamePropertyItem(tagPropertyItem * ptagPropertyItem)
{
	//Ч�����
	ASSERT(ptagPropertyItem!=NULL);
	if (ptagPropertyItem==NULL) return false;

	//�����ִ�
	CGamePropertyItem * pGamePropertyItem=NULL;

	//��ȡ����
	if (m_GamePropertyItemMap.Lookup(ptagPropertyItem->dwPropertyID,pGamePropertyItem)==FALSE)
	{
		//��������
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

		//���ñ���
		ZeroMemory(pGamePropertyItem,sizeof(CGamePropertyItem));
	}


	//��������
	CopyMemory(&pGamePropertyItem->m_PropertyItem,ptagPropertyItem,sizeof(tagPropertyItem));

	//��������
	m_GamePropertyItemMap[ptagPropertyItem->dwPropertyID]=pGamePropertyItem;

	return true;
}

//�����ӵ���
bool CGamePropertyListManager::InsertGamePropertySubItem(tagPropertySubItem * ptagPropertySubItem)
{
	//Ч�����
	ASSERT(ptagPropertySubItem!=NULL);
	if (ptagPropertySubItem==NULL) return false;

	//�����ִ�
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

	//��ȡ����
	if (bFinder == false)
	{
		//��������
		try
		{
			pGamePropertySubItem=new CGamePropertySubItem;
			if (pGamePropertySubItem==NULL) return false;
		}
		catch (...) { return false; }

		//���ñ���
		ZeroMemory(pGamePropertySubItem,sizeof(CGamePropertySubItem));
		dwCurrKey++;
	}
	else
	{
		dwCurrKey = dwkey;
	}

	//��������
	CopyMemory(&pGamePropertySubItem->m_PropertySubItem,ptagPropertySubItem,sizeof(tagPropertySubItem));

	//��������
	m_GamePropertySubItemMap[dwCurrKey]=pGamePropertySubItem;

	return true;
}

//ɾ������
bool CGamePropertyListManager::DeleteGamePropertyTypeItem(DWORD dwTypeID)
{
	//��������
	CGamePropertyTypeItem * pGamePropertyTypeItem=NULL;
	if (m_GamePropertyTypeItemMap.Lookup(dwTypeID,pGamePropertyTypeItem)==FALSE) return false;

	//ɾ������
	m_GamePropertyTypeItemMap.RemoveKey(dwTypeID);

	return true;
}

//ɾ������
bool CGamePropertyListManager::DeleteGamePropertyItem(DWORD dwPropertyID)
{
	//���ҵ���
	CGamePropertyItem * pGamePropertyItem=NULL;
	if (m_GamePropertyItemMap.Lookup(dwPropertyID,pGamePropertyItem)==FALSE) return false;

	//ɾ������
	m_GamePropertyItemMap.RemoveKey(dwPropertyID);
	m_GamePropertyItemArray.Add(pGamePropertyItem);

	return true;
}

//ö������
CGamePropertyTypeItem * CGamePropertyListManager::EmunGamePropertyTypeItem(POSITION & Position)
{
	//��������
	DWORD wKey=0;
	CGamePropertyTypeItem * pGamePropertyTypeItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_GamePropertyTypeItemMap.GetStartPosition();
	if (Position!=NULL) m_GamePropertyTypeItemMap.GetNextAssoc(Position,wKey,pGamePropertyTypeItem);

	return pGamePropertyTypeItem;
}

//ö�ٵ���
CGamePropertyRelatItem * CGamePropertyListManager::EmunGamePropertyRelatItem(POSITION & Position)
{
	//��������
	DWORD wKey=0;
	CGamePropertyRelatItem * pGamePropertyRelatItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_GamePropertyRelatItemMap.GetStartPosition();
	if (Position!=NULL) m_GamePropertyRelatItemMap.GetNextAssoc(Position,wKey,pGamePropertyRelatItem);

	return pGamePropertyRelatItem;
}

//ö�ٵ���
CGamePropertyItem * CGamePropertyListManager::EmunGamePropertyItem(POSITION & Position)
{
	//��������
	DWORD wKey=0;
	CGamePropertyItem * pGamePropertyItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_GamePropertyItemMap.GetStartPosition();
	if (Position!=NULL) m_GamePropertyItemMap.GetNextAssoc(Position,wKey,pGamePropertyItem);

	return pGamePropertyItem;
}

//ö���ӵ���
CGamePropertySubItem * CGamePropertyListManager::EmunGamePropertySubItem(POSITION & Position)
{
	//��������
	DWORD wKey=0;
	CGamePropertySubItem * pGamePropertySubItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_GamePropertySubItemMap.GetStartPosition();
	if (Position!=NULL) m_GamePropertySubItemMap.GetNextAssoc(Position,wKey,pGamePropertySubItem);

	return pGamePropertySubItem;
}

//���ҵ���
CGamePropertyItem * CGamePropertyListManager::SearchGamePropertyItem(DWORD dwPropertyID)
{
	CGamePropertyItem * pGamePropertyItem=NULL;
	m_GamePropertyItemMap.Lookup(dwPropertyID,pGamePropertyItem);
	return pGamePropertyItem;
}


//////////////////////////////////////////////////////////////////////////////////
