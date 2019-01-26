#ifndef GAMEPROPERTYLISTMANAGER_HEAD_FILE
#define GAMEPROPERTYLISTMANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////////////

//列表管理
namespace NGamePropertyListManager
{

	//////////////////////////////////////////////////////////////////////////////////

	//游戏子项
	class CGamePropertyListItem
	{
		//变量定义
	public:
		bool							m_bDisuse;							//废弃标志

		//函数定义
	protected:
		//构造函数
		CGamePropertyListItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//种类结构
	class CGamePropertyTypeItem : public CGamePropertyListItem
	{
		//变量定义
	public:
		tagPropertyTypeItem				m_PropertyTypeItem;	

		//函数定义
	public:
		//构造函数
		CGamePropertyTypeItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//关系结构
	class CGamePropertyRelatItem : public CGamePropertyListItem
	{
		//变量定义
	public:
		tagPropertyRelatItem				m_PropertyRelatItem;	

		//函数定义
	public:
		//构造函数
		CGamePropertyRelatItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//道具结构
	class CGamePropertyItem : public CGamePropertyListItem
	{
		//变量定义
	public:
		tagPropertyItem					m_PropertyItem;	

		//函数定义
	public:
		//构造函数
		CGamePropertyItem();
	};

	//////////////////////////////////////////////////////////////////////////////////

	//道具结构
	class CGamePropertySubItem : public CGamePropertyListItem
	{
		//变量定义
	public:
		tagPropertySubItem					m_PropertySubItem;	

		//函数定义
	public:
		//构造函数
		CGamePropertySubItem();
	};


	//////////////////////////////////////////////////////////////////////////////////

	//数组说明
	typedef CWHArray<CGamePropertyTypeItem *> CGamePropertyTypeItemArray;
	typedef CWHArray<CGamePropertyRelatItem *>	CGamePropertyRelatItemArray;
	typedef CWHArray<CGamePropertyItem *>	CGamePropertyItemArray;
	typedef CWHArray<CGamePropertySubItem *>	CGamePropertySubItemArray;


	//索引说明
	typedef CMap<DWORD,DWORD,CGamePropertyTypeItem *,CGamePropertyTypeItem * &> CGamePropertyTypeItemMap;
	typedef CMap<DWORD,DWORD,CGamePropertyRelatItem *,CGamePropertyRelatItem * &> CGamePropertyRelatItemMap;
	typedef CMap<DWORD,DWORD,CGamePropertyItem *,CGamePropertyItem * &> CGamePropertyItemMap;
	typedef CMap<DWORD,DWORD,CGamePropertySubItem *,CGamePropertySubItem * &> CGamePropertySubItemMap;

	//////////////////////////////////////////////////////////////////////////////////

	//列表服务
	class CGamePropertyListManager
	{
		//索引变量
	protected:
		CGamePropertyTypeItemMap					m_GamePropertyTypeItemMap;				//种类索引
		CGamePropertyRelatItemMap					m_GamePropertyRelatItemMap;				//关系索引
		CGamePropertyItemMap						m_GamePropertyItemMap;					//道具索引
		CGamePropertySubItemMap						m_GamePropertySubItemMap;				//子道具索引

		//存储变量
	protected:
		CGamePropertyTypeItemArray					m_GamePropertyTypeItemArray;			//种类数组
		CGamePropertyRelatItemArray					m_GamePropertyRelatItemArray;			//关系数组
		CGamePropertyItemArray						m_GamePropertyItemArray;				//道具数组
		CGamePropertySubItemArray					m_GamePropertySubItemArray;				//子道具数组

		//函数定义
	public:
		//构造函数
		CGamePropertyListManager();
		//析构函数
		virtual ~CGamePropertyListManager();

		//管理接口
	public:
		//重置列表
		VOID ResetPropertyListManager();
		//清理种类
		VOID CleanPropertyTypeItem();
		//清理关系
		VOID CleanPropertyRelatItem();
		//清理道具
		VOID CleanPropertyItem();
		//清理子道具
		VOID CleanPropertySubItem();
		//废弃种类
		VOID DisusePropertyTypeItem();
		//废弃关系
		VOID DisusePropertyRelatItem();
		//废弃道具
		VOID DisusePropertyItem();
		//废弃子道具
		VOID DisusePropertySubItem();
		//插入接口
	public:
		//插入种类
		bool InsertGamePropertyTypeItem(tagPropertyTypeItem * ptagPropertyTypeItem);
		//插入关系
		bool InsertGamePropertyRelatItem(tagPropertyRelatItem * ptagPropertyRelatItem);
		//插入道具
		bool InsertGamePropertyItem(tagPropertyItem * ptagPropertyItem);
		//插入子道具
		bool InsertGamePropertySubItem(tagPropertySubItem * ptagPropertySubItem);

		//删除接口
	public:
		//删除种类
		bool DeleteGamePropertyTypeItem(DWORD dwTypeID);
		//删除道具
		bool DeleteGamePropertyItem(DWORD wPropertyID);

		//枚举接口
	public:
		//枚举种类
		CGamePropertyTypeItem * EmunGamePropertyTypeItem(POSITION & Position);
		//枚举道具
		CGamePropertyRelatItem * EmunGamePropertyRelatItem(POSITION & Position);
		//枚举道具
		CGamePropertyItem * EmunGamePropertyItem(POSITION & Position);
		//枚举子道具
		CGamePropertySubItem * EmunGamePropertySubItem(POSITION & Position);

		//查找接口
	public:
		//查找道具
		CGamePropertyItem * SearchGamePropertyItem(DWORD wPropertyID);

		//数目接口
	public:
		//种类数目
		DWORD GetGamePropertyTypeCount() { return (DWORD)m_GamePropertyTypeItemMap.GetCount(); }
		//关系数目
		DWORD GetGamePropertyRelatCount() { return (DWORD)m_GamePropertyRelatItemMap.GetCount(); }
		//道具数目
		DWORD GetGamePropertyCount() { return (DWORD)m_GamePropertyItemMap.GetCount(); }
		//子道具数目
		DWORD GetGamePropertySubCount() { return (DWORD)m_GamePropertySubItemMap.GetCount(); }

	};

	//////////////////////////////////////////////////////////////////////////////////

};

using namespace NGamePropertyListManager;

#endif