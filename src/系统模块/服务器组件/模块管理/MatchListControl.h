#ifndef MATCH_LIST_CONTROL_HEAD_FILE
#define MATCH_LIST_CONTROL_HEAD_FILE

#pragma once

#include "ListControl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

//比赛列表
class MODULE_MANAGER_CLASS CMatchListControl : public CListControl
{
	//函数定义
public:
	//构造函数
	CMatchListControl();
	//析构函数
	virtual ~CMatchListControl();

	//重载函数
public:
	//配置列表
	virtual VOID InitListControl();
	//子项排序
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//获取颜色
	virtual VOID GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//功能函数
public:
	//插入列表
	bool InsertMatchOption(tagGameMatchOption * pGameMatchOption);
	//更新列表
	bool UpdateMatchOption(tagGameMatchOption * pGameMatchOption);

	//辅助函数
private:
	//插入索引
	WORD GetInsertIndex(tagGameMatchOption * pGameMatchOption);
	//描述字符
	LPCTSTR GetDescribeString(tagGameMatchOption * pGameMatchOption, WORD wColumnIndex);
};

//////////////////////////////////////////////////////////////////////////////////

//奖励列表
class MODULE_MANAGER_CLASS CRewardListControl : public CListControl
{
	//函数定义
public:
	//构造函数
	CRewardListControl();
	//析构函数
	virtual ~CRewardListControl();

	//重载函数
public:
	//配置列表
	virtual VOID InitListControl();
	//子项排序
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//获取颜色
	virtual VOID GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//功能函数
public:
	//插入列表
	bool InsertMatchReward(tagMatchRewardInfo * pMatchRewardInfo);
	//更新列表
	bool UpdateMatchReward(tagMatchRewardInfo * pMatchRewardInfo);
	//删除列表
	bool DeleteMatchReward(tagMatchRewardInfo * pMatchRewardInfo);

	//辅助函数
private:
	//插入索引
	WORD GetInsertIndex(tagMatchRewardInfo * pMatchRewardInfo);
	//描述字符
	LPCTSTR GetDescribeString(tagMatchRewardInfo * pMatchRewardInfo, WORD wColumnIndex);
};

#endif