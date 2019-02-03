#include "StdAfx.h"
#include "MatchListControl.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CMatchListControl::CMatchListControl()
{
}

//析构函数
CMatchListControl::~CMatchListControl()
{
}

//配置列表
VOID CMatchListControl::InitListControl()
{
	//变量定义
	INT nColIndex=0;

	//配置列表
	InsertColumn(nColIndex++,TEXT("比赛类型"),LVCFMT_CENTER,80);
	InsertColumn(nColIndex++,TEXT("比赛名称"),LVCFMT_CENTER,200);

	return;
}

//子项排序
INT CMatchListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//变量定义
	tagGameMatchOption * pGameMatchOption1=(tagGameMatchOption *)lParam1;
	tagGameMatchOption * pGameMatchOption2=(tagGameMatchOption *)lParam2;

	//子项排序
	switch (wColumnIndex)
	{
	case 0:		//比赛类型
		{
			return pGameMatchOption1->cbMatchType>pGameMatchOption2->cbMatchType?SORT_POSITION_AFTER:SORT_POSITION_FRONT;			
		}
	case 1:		//比赛名称
		{
			return lstrcmp(pGameMatchOption1->szMatchName,pGameMatchOption2->szMatchName);
		}	
	}

	return 0;
}

//获取颜色
VOID CMatchListControl::GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//变量定义
	ASSERT(lItemParam!=NULL);

	//设置颜色
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

	return;
}

//插入列表
bool CMatchListControl::InsertMatchOption(tagGameMatchOption * pGameMatchOption)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameMatchOption;

	//存在判断
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pGameMatchOption);
			LPCTSTR pszDescribe=GetDescribeString(pGameMatchOption,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameMatchOption);
		}
		else
		{
			//字符子项
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameMatchOption,i),0,0,0,0);
		}
	}

	return true;
}

//更新列表
bool CMatchListControl::UpdateMatchOption(tagGameMatchOption * pGameMatchOption)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameMatchOption;

	//存在判断
	INT nUpdateItem=FindItem(&FindInfo);
	if (nUpdateItem==LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		//字符子项
		SetItem(nUpdateItem,i,LVIF_TEXT,GetDescribeString(pGameMatchOption,i),0,0,0,0);
	}

	//更新子项
	RedrawItems(nUpdateItem,nUpdateItem);

	return true;
}

//插入索引
WORD CMatchListControl::GetInsertIndex(tagGameMatchOption * pGameMatchOption)
{
	//变量定义
	INT nItemCount=GetItemCount();
	tagGameMatchOption * pGameMatchTemp=NULL;

	//获取位置
	for (INT i=0;i<nItemCount;i++)
	{
		//获取数据
		pGameMatchTemp=(tagGameMatchOption *)GetItemData(i);

		//比赛名称
		if (lstrcmp(pGameMatchOption->szMatchName,pGameMatchTemp->szMatchName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//描述字符
LPCTSTR CMatchListControl::GetDescribeString(tagGameMatchOption * pGameMatchOption, WORD wColumnIndex)
{
	//变量定义
	static TCHAR szDescribe[128]=TEXT("");

	//构造字符
	switch (wColumnIndex)
	{
	case 0:		//比赛类型
		{
			//变量定义
			LPCTSTR pszMatchType[] = {TEXT("定时赛"),TEXT("即时赛")};

			//判断索引
			if(pGameMatchOption->cbMatchType<CountArray(pszMatchType))
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%s"),pszMatchType[pGameMatchOption->cbMatchType]);
			}

			return szDescribe;
		}
	case 1:		//比赛名称
		{
			return pGameMatchOption->szMatchName;
		}	
	//case 2:		//报名方式
	//	{
	//		if(pGameMatchOption->cbSignupMode==SIGNUP_MODE_SIGNUP_FEE)
	//		{
	//			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("扣除金币"));
	//		}
	//		else if(pGameMatchOption->cbSignupMode==SIGNUP_MODE_MATCH_USER)
	//		{
	//			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("跳转网站"));
	//		}

	//		return szDescribe;
	//	}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////


//构造函数
CRewardListControl::CRewardListControl()
{
}

//析构函数
CRewardListControl::~CRewardListControl()
{
}

//配置列表
VOID CRewardListControl::InitListControl()
{
	//变量定义
	INT nColIndex=0;

	//配置列表
	InsertColumn(nColIndex++,TEXT("名    次"),LVCFMT_LEFT,120);
	InsertColumn(nColIndex++,TEXT("奖励金币"),LVCFMT_LEFT,100);
	InsertColumn(nColIndex++,TEXT("奖励元宝"),LVCFMT_LEFT,100);
	InsertColumn(nColIndex++,TEXT("奖励经验"),LVCFMT_LEFT,100);

	return;
}

//子项排序
INT CRewardListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//变量定义
	tagMatchRewardInfo * pMatchRewardInfo1=(tagMatchRewardInfo *)lParam1;
	tagMatchRewardInfo * pMatchRewardInfo2=(tagMatchRewardInfo *)lParam2;

	//子项排序
	switch (wColumnIndex)
	{
	case 0:		//奖励名称
		{
			return pMatchRewardInfo1->wRankID>pMatchRewardInfo2->wRankID;
		}
	case 1:		//奖励金币
		{
			return pMatchRewardInfo1->lRewardGold>pMatchRewardInfo2->lRewardGold;			
		}
	case 2:		//奖励元宝
		{
			return pMatchRewardInfo1->lRewardIngot>pMatchRewardInfo2->lRewardIngot;			
		}
	case 3:		//奖励经验
		{
			return pMatchRewardInfo1->dwRewardExperience>pMatchRewardInfo2->dwRewardExperience;	;
		}
	}

	return 0;
}

//获取颜色
VOID CRewardListControl::GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//变量定义
	ASSERT(lItemParam!=NULL);

	//设置颜色
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

	return;
}

//插入列表
bool CRewardListControl::InsertMatchReward(tagMatchRewardInfo * pMatchRewardInfo)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardInfo;

	//存在判断
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//插入首项
			INT nIndex=GetInsertIndex(pMatchRewardInfo);
			LPCTSTR pszDescribe=GetDescribeString(pMatchRewardInfo,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pMatchRewardInfo);
		}
		else
		{
			//字符子项
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pMatchRewardInfo,i),0,0,0,0);
		}
	}

	return true;
}

//更新列表
bool CRewardListControl::UpdateMatchReward(tagMatchRewardInfo * pMatchRewardInfo)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardInfo;

	//存在判断
	INT nUpdateItem=FindItem(&FindInfo);
	if (nUpdateItem==LB_ERR) return true;

	//插入列表
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		//字符子项
		SetItem(nUpdateItem,i,LVIF_TEXT,GetDescribeString(pMatchRewardInfo,i),0,0,0,0);
	}

	//更新子项
	RedrawItems(nUpdateItem,nUpdateItem);

	return true;
}

//删除列表
bool CRewardListControl::DeleteMatchReward(tagMatchRewardInfo * pMatchRewardInfo)
{
	//变量定义
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//设置变量
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardInfo;

	//存在判断
	INT nDeleteItem=FindItem(&FindInfo);
	if (nDeleteItem==LB_ERR) return true;

	//删除子项
	DeleteItem(nDeleteItem);

	return true;
}

//插入索引
WORD CRewardListControl::GetInsertIndex(tagMatchRewardInfo * pMatchRewardInfo)
{
	//变量定义
	INT nItemCount=GetItemCount();
	tagMatchRewardInfo * pMatchRewardTemp=NULL;

	//获取位置
	for (INT i=0;i<nItemCount;i++)
	{
		//获取数据
		pMatchRewardTemp=(tagMatchRewardInfo *)GetItemData(i);

		//比赛名称
		if (pMatchRewardInfo->wRankID < pMatchRewardTemp->wRankID)
		{
			return i;
		}
	}

	return nItemCount;
}


//描述字符
LPCTSTR CRewardListControl::GetDescribeString(tagMatchRewardInfo * pMatchRewardInfo, WORD wColumnIndex)
{
	//变量定义
	static TCHAR szDescribe[128]=TEXT("");

	//构造字符
	switch (wColumnIndex)
	{
	case 0:		//比赛名次
		{
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("第%02d名"),pMatchRewardInfo->wRankID);

			return szDescribe;
		}
	case 1:		//金币奖励
		{
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%.2f"),pMatchRewardInfo->lRewardGold);

			return szDescribe;
		}
	case 2:		//元宝奖励
		{
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%.2f"),pMatchRewardInfo->lRewardIngot);

			return szDescribe;
		}
	case 3:		//经验奖励
		{
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pMatchRewardInfo->dwRewardExperience);

			return szDescribe;
		}	
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
