#ifndef GAMEPROPERTYLISTMANAGER_HEAD_FILE
#define GAMEPROPERTYLISTMANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////////////

//�б����
namespace NGamePropertyListManager
{

	//////////////////////////////////////////////////////////////////////////////////

	//��Ϸ����
	class CGamePropertyListItem
	{
		//��������
	public:
		bool							m_bDisuse;							//������־

		//��������
	protected:
		//���캯��
		CGamePropertyListItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//����ṹ
	class CGamePropertyTypeItem : public CGamePropertyListItem
	{
		//��������
	public:
		tagPropertyTypeItem				m_PropertyTypeItem;	

		//��������
	public:
		//���캯��
		CGamePropertyTypeItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//��ϵ�ṹ
	class CGamePropertyRelatItem : public CGamePropertyListItem
	{
		//��������
	public:
		tagPropertyRelatItem				m_PropertyRelatItem;	

		//��������
	public:
		//���캯��
		CGamePropertyRelatItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//���߽ṹ
	class CGamePropertyItem : public CGamePropertyListItem
	{
		//��������
	public:
		tagPropertyItem					m_PropertyItem;	

		//��������
	public:
		//���캯��
		CGamePropertyItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//���߽ṹ
	class CGamePropertySubItem : public CGamePropertyListItem
	{
		//��������
	public:
		tagPropertySubItem					m_PropertySubItem;	

		//��������
	public:
		//���캯��
		CGamePropertySubItem();
	};


	//////////////////////////////////////////////////////////////////////////////////

	//����˵��
	typedef CWHArray<CGamePropertyTypeItem *> CGamePropertyTypeItemArray;
	typedef CWHArray<CGamePropertyRelatItem *>	CGamePropertyRelatItemArray;
	typedef CWHArray<CGamePropertyItem *>	CGamePropertyItemArray;
	typedef CWHArray<CGamePropertySubItem *>	CGamePropertySubItemArray;


	//����˵��
	typedef CMap<DWORD,DWORD,CGamePropertyTypeItem *,CGamePropertyTypeItem * &> CGamePropertyTypeItemMap;
	typedef CMap<DWORD,DWORD,CGamePropertyRelatItem *,CGamePropertyRelatItem * &> CGamePropertyRelatItemMap;
	typedef CMap<DWORD,DWORD,CGamePropertyItem *,CGamePropertyItem * &> CGamePropertyItemMap;
	typedef CMap<DWORD,DWORD,CGamePropertySubItem *,CGamePropertySubItem * &> CGamePropertySubItemMap;

	//////////////////////////////////////////////////////////////////////////////////

	//�б����
	class CGamePropertyListManager
	{
		//��������
	protected:
		CGamePropertyTypeItemMap					m_GamePropertyTypeItemMap;				//��������
		CGamePropertyRelatItemMap					m_GamePropertyRelatItemMap;				//��ϵ����
		CGamePropertyItemMap						m_GamePropertyItemMap;					//��������
		CGamePropertySubItemMap						m_GamePropertySubItemMap;				//�ӵ�������

		//�洢����
	protected:
		CGamePropertyTypeItemArray					m_GamePropertyTypeItemArray;			//��������
		CGamePropertyRelatItemArray					m_GamePropertyRelatItemArray;			//��ϵ����
		CGamePropertyItemArray						m_GamePropertyItemArray;				//��������
		CGamePropertySubItemArray					m_GamePropertySubItemArray;				//�ӵ�������

		//��������
	public:
		//���캯��
		CGamePropertyListManager();
		//��������
		virtual ~CGamePropertyListManager();

		//����ӿ�
	public:
		//�����б�
		VOID ResetPropertyListManager();
		//��������
		VOID CleanPropertyTypeItem();
		//�����ϵ
		VOID CleanPropertyRelatItem();
		//�������
		VOID CleanPropertyItem();
		//�����ӵ���
		VOID CleanPropertySubItem();
		//��������
		VOID DisusePropertyTypeItem();
		//������ϵ
		VOID DisusePropertyRelatItem();
		//��������
		VOID DisusePropertyItem();
		//�����ӵ���
		VOID DisusePropertySubItem();
		//����ӿ�
	public:
		//��������
		bool InsertGamePropertyTypeItem(tagPropertyTypeItem * ptagPropertyTypeItem);
		//�����ϵ
		bool InsertGamePropertyRelatItem(tagPropertyRelatItem * ptagPropertyRelatItem);
		//�������
		bool InsertGamePropertyItem(tagPropertyItem * ptagPropertyItem);
		//�����ӵ���
		bool InsertGamePropertySubItem(tagPropertySubItem * ptagPropertySubItem);

		//ɾ���ӿ�
	public:
		//ɾ������
		bool DeleteGamePropertyTypeItem(DWORD dwTypeID);
		//ɾ������
		bool DeleteGamePropertyItem(DWORD wPropertyID);

		//ö�ٽӿ�
	public:
		//ö������
		CGamePropertyTypeItem * EmunGamePropertyTypeItem(POSITION & Position);
		//ö�ٵ���
		CGamePropertyRelatItem * EmunGamePropertyRelatItem(POSITION & Position);
		//ö�ٵ���
		CGamePropertyItem * EmunGamePropertyItem(POSITION & Position);
		//ö���ӵ���
		CGamePropertySubItem * EmunGamePropertySubItem(POSITION & Position);

		//���ҽӿ�
	public:
		//���ҵ���
		CGamePropertyItem * SearchGamePropertyItem(DWORD wPropertyID);

		//��Ŀ�ӿ�
	public:
		//������Ŀ
		DWORD GetGamePropertyTypeCount() { return (DWORD)m_GamePropertyTypeItemMap.GetCount(); }
		//��ϵ��Ŀ
		DWORD GetGamePropertyRelatCount() { return (DWORD)m_GamePropertyRelatItemMap.GetCount(); }
		//������Ŀ
		DWORD GetGamePropertyCount() { return (DWORD)m_GamePropertyItemMap.GetCount(); }
		//�ӵ�����Ŀ
		DWORD GetGamePropertySubCount() { return (DWORD)m_GamePropertySubItemMap.GetCount(); }

	};

	//////////////////////////////////////////////////////////////////////////////////

};

using namespace NGamePropertyListManager;

#endif