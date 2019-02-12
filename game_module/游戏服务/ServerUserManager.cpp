#include "StdAfx.h"
#include "ServerUserManager.h"

//////////////////////////////////////////////////////////////////////////////////
const TCHAR g_szGameDataSpe[]=TEXT(":;");

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserItem::CServerUserItem()
{
	//附加变量
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//托管变量
	m_lTrusteeScore=0L;
	m_lRestrictScore=0L;
	m_lFrozenedScore=0L;
	m_lFrozenedScore=0L;

	//登录信息
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//客户类型
	m_bMobileUser=false;
	m_bAndroidUser=false;

	//组件接口
	m_pIServerUserItemSink=NULL;
	m_pIMatchUserItemSink=NULL;

	//系统属性
	m_wBindIndex=INVALID_WORD;
	m_dwClientAddr=INADDR_NONE;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//比赛信息
	m_dwSignUpTime=0;
	m_cbMatchStatus=0;
	m_pMatchData=NULL;

	//辅助变量
	m_bTrusteeUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));
	ZeroMemory(m_szUserGameData,sizeof(m_szUserGameData));

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wLastTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	//记录信息
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	//手机定义
	m_wMobileUserRule=0;
	m_wMobileUserRule |= VIEW_MODE_PART;
	m_wMobileUserRule |= VIEW_INFO_LEVEL_1;
	m_wDeskPos=0;

	//设置质数
	m_UserGameDataMap.InitHashTable(17);

	return;
}

//析构函数
CServerUserItem::~CServerUserItem()
{
	//清空数据
	INT wKey=0;
	tagGameDataItem * pGameDataItem=NULL;
	POSITION Position=m_UserGameDataMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserGameDataMap.GetNextAssoc(Position,wKey,pGameDataItem);
		SafeDelete(pGameDataItem);
	}
	m_UserGameDataMap.RemoveAll();
}

//接口查询
VOID * CServerUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserItem,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserItem,Guid,dwQueryVer);
	return NULL;
}

//用户胜率
WORD CServerUserItem::GetUserWinRate()
{
	//计算胜率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwWinCount*10000L/dwPlayCount);

	return 0;
}

//用户输率
WORD CServerUserItem::GetUserLostRate()
{
	//计算输率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwLostCount*10000L/dwPlayCount);

	return 0;
}

//用户和率
WORD CServerUserItem::GetUserDrawRate()
{
	//计算和率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwDrawCount*10000L/dwPlayCount);

	return 0;
}

//用户逃率
WORD CServerUserItem::GetUserFleeRate()
{
	//计算逃率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwFleeCount*10000L/dwPlayCount);

	return 0;
}

//游戏数据
LPCTSTR CServerUserItem::GetUserGameData()
{
	CString strMainItem=TEXT("");

	//便利数据
	INT wKey=0;
	tagGameDataItem * pGameDataItem=NULL;
	POSITION Position=m_UserGameDataMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserGameDataMap.GetNextAssoc(Position,wKey,pGameDataItem);
		if (pGameDataItem!=NULL)
		{
			strMainItem.AppendFormat(TEXT("%d:%s;"),pGameDataItem->nKey,pGameDataItem->szValue);
		}
	}

	lstrcpyn(m_szUserGameData,strMainItem,strMainItem.GetLength()+1);

	return m_szUserGameData;
}

//游戏数据
VOID CServerUserItem::GetUserGameData(INT nKey, LPTSTR pszValue, INT nMaxCount)
{
	ASSERT(nMaxCount<=LEN_GAME_ITEM);
	if (nMaxCount>LEN_GAME_ITEM) return;

	tagGameDataItem *pGameDataItem=NULL;
	m_UserGameDataMap.Lookup(nKey,pGameDataItem);
	lstrcpyn(pszValue,pGameDataItem->szValue,nMaxCount);
}

//游戏数据
VOID CServerUserItem::SetUserGameData(LPCTSTR pszValue, INT nMaxCount)
{
	ASSERT(nMaxCount<=LEN_GAME_DATA);
	if (nMaxCount>LEN_GAME_DATA) return;

	//有效性判断
	if (IsValidGameData(pszValue,g_szGameDataSpe)==false)
		return;

	//覆盖游戏数据
	ZeroMemory(m_szUserGameData,sizeof(m_szUserGameData));
	lstrcpyn(m_szUserGameData,pszValue,nMaxCount);

	//重新构造Map
	InitUserGameDataMap();

	return;
}

//游戏数据
VOID CServerUserItem::SetUserGameData(INT nKey, LPCTSTR pszValue, INT nMaxCount)
{
	ASSERT(nMaxCount<=LEN_GAME_ITEM);
	if (nMaxCount>LEN_GAME_ITEM) return;

	tagGameDataItem *pGameDataItem=NULL;
	m_UserGameDataMap.Lookup(nKey,pGameDataItem);

	if (pGameDataItem!=NULL)
	{
		lstrcpyn(pGameDataItem->szValue,pszValue,nMaxCount);
	}
	else
	{
		try
		{
			pGameDataItem=new tagGameDataItem;

			pGameDataItem->nKey=nKey;
			lstrcpyn(pGameDataItem->szValue,pszValue,nMaxCount);

			m_UserGameDataMap[nKey]=pGameDataItem;
		}
		catch (...)
		{
			ASSERT(FALSE);
			return;
		}
	}
}

//游戏数据
VOID CServerUserItem::WriteUserGameData()
{
	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemGameData(this,0);

	return;
}

//对比帐号
bool CServerUserItem::ContrastNickName(LPCTSTR pszNickName)
{
	//效验参数
	ASSERT(pszNickName!=NULL);
	if (pszNickName==NULL) return false;

	//长度对比
	INT nContrastLen=lstrlen(pszNickName);
	INT nSourceLen=lstrlen(m_UserInfo.szNickName);

	//字符对比
	if (nContrastLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszNickName,nContrastLen,m_UserInfo.szNickName,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//对比密码
bool CServerUserItem::ContrastLogonPass(LPCTSTR pszPassword)
{
	//效验参数
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	//长度对比
	INT nTargetLen=lstrlen(pszPassword);
	INT nSourceLen=lstrlen(m_szLogonPass);

	//密码对比
	if (nTargetLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszPassword,nTargetLen,m_szLogonPass,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//设置状态
bool CServerUserItem::SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)
{
	//效验状态
	//ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//记录信息
	WORD wOldTableID=m_UserInfo.wTableID;
	WORD wOldChairID=m_UserInfo.wChairID;

	//设置变量
	m_UserInfo.wTableID=wTableID;
	m_UserInfo.wChairID=wChairID;
	m_UserInfo.cbUserStatus=cbUserStatus;
	if(cbUserStatus==US_PLAYING)m_UserInfo.wLastTableID=wTableID;

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemStatus(this,wOldTableID,wOldChairID);

	return true;
}

//报名状态
VOID CServerUserItem::SetUserMatchStatus(BYTE cbMatchStatus)
{
	BYTE cbOldMatchStatus=m_cbMatchStatus;

	m_cbMatchStatus=cbMatchStatus;

	//发送状态
	/*ASSERT(m_pIMatchUserItemSink!=NULL);
	if (m_pIMatchUserItemSink!=NULL) m_pIMatchUserItemSink->OnEventMatchUserStatus(this,cbOldMatchStatus,cbMatchStatus);*/
}

//写入积分
bool CServerUserItem::WriteUserScore(SCORE lScore, SCORE lGrade, SCORE lRevenue, SCORE lIngot, BYTE cbScoreType, DWORD dwPlayTimeCount,DWORD dwWinExperience)
{
	//效验状态
	ASSERT((m_UserInfo.dwUserID!=0L)&&(cbScoreType!=SCORE_TYPE_NULL));
	if ((m_UserInfo.dwUserID==0L)||(cbScoreType==SCORE_TYPE_NULL)) return false;

	//修改胜负
	switch (cbScoreType)
	{
	case SCORE_TYPE_WIN:	//胜局
		{
			m_UserInfo.dwWinCount++;
			m_RecordInfo.dwWinCount++;
			m_VariationInfo.dwWinCount++;
			break;
		}
	case SCORE_TYPE_LOSE:	//输局
		{
			m_UserInfo.dwLostCount++;
			m_RecordInfo.dwLostCount++;
			m_VariationInfo.dwLostCount++;
			break;
		}
	case SCORE_TYPE_DRAW:	//和局
		{
			m_UserInfo.dwDrawCount++;
			m_RecordInfo.dwDrawCount++;
			m_VariationInfo.dwDrawCount++;
			break;
		}
	case SCORE_TYPE_FLEE:	//逃局
		{
			m_UserInfo.dwFleeCount++;
			m_RecordInfo.dwFleeCount++;
			m_VariationInfo.dwFleeCount++;
			break;
		}
	}

	//设置标志
	m_bModifyScore=true;

	//设置积分
	m_UserInfo.lScore+=lScore;
	m_UserInfo.lGrade+=lGrade;
	m_UserInfo.lIngot+=lIngot;
	m_UserInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;	

	//历史积分
	m_UserInfo.lIntegralCount+=lScore;
	m_RecordInfo.lIntegralCount+=lScore;
	m_VariationInfo.lIntegralCount+=lScore;

	//记录信息
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lGrade+=lGrade;
	m_RecordInfo.lRevenue+=lRevenue;
	m_RecordInfo.lIngot+=lIngot;
	m_RecordInfo.dwPlayTimeCount+=dwPlayTimeCount;
	m_RecordInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;

	//变更信息
	m_VariationInfo.lIngot+=lIngot;
	m_VariationInfo.dwPlayTimeCount+=dwPlayTimeCount;
	m_VariationInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;
	
	//分数变更
	m_VariationInfo.lScore+=lScore;
	m_VariationInfo.lGrade+=lGrade;
	m_VariationInfo.lRevenue+=lRevenue;	

	//托管积分
	if (m_lRestrictScore>0L)
	{
		//变量定义
		SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

		//托管调整
		if (lTotalScore>m_lRestrictScore)
		{
			m_UserInfo.lScore=m_lRestrictScore;
			m_lTrusteeScore=lTotalScore-m_lRestrictScore;
		}
		else
		{
			m_lTrusteeScore=0L;
			m_UserInfo.lScore=lTotalScore;
		}
	}

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this,SCORE_REASON_WRITE);

	return true;
}

//领取奖励
bool CServerUserItem::SetUserTaskReward(SCORE lScore, SCORE lIngot)
{
	m_UserInfo.lScore=lScore;
	m_UserInfo.lIngot=lIngot;

	return true;
}

//查询记录
bool CServerUserItem::QueryRecordInfo(tagVariationInfo & RecordInfo)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//设置变量
	RecordInfo=m_RecordInfo;

	return true;
}

//提取变更
bool CServerUserItem::DistillVariation(tagVariationInfo & VariationInfo)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//修改判断
	if (m_bModifyScore==true)
	{
		//设置变量
		VariationInfo=m_VariationInfo;
	}
	else
	{
		//设置变量
		ZeroMemory(&VariationInfo,sizeof(VariationInfo));
	}

	//还原变量
	m_bModifyScore=false;
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	return true;
}

//冻结积分
bool CServerUserItem::FrozenedUserScore(SCORE lScore)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//效验积分
	ASSERT((lScore>=0L)&&((m_UserInfo.lScore+m_lTrusteeScore)>=lScore));
	if ((lScore<0L)||((m_UserInfo.lScore+m_lTrusteeScore)<lScore)) return false;

	//设置变量
	m_lFrozenedScore+=lScore;

	//设置用户
	if (m_lTrusteeScore<lScore)
	{
		//积分锁定
		lScore-=m_lTrusteeScore;
		m_UserInfo.lScore-=lScore;
		m_lTrusteeScore=0L;
	}
	else
	{
		//托管锁定
		m_lTrusteeScore-=lScore;
	}

	return true;
}

//解冻积分
bool CServerUserItem::UnFrozenedUserScore(SCORE lScore)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//效验积分
	ASSERT((lScore>=0L)&&(m_lFrozenedScore>=lScore));
	if ((lScore<0L)||(m_lFrozenedScore<lScore)) return false;

	//设置变量
	m_lFrozenedScore-=lScore;
	m_UserInfo.lScore+=lScore;

	//托管积分
	if (m_lRestrictScore>0L)
	{
		//变量定义
		SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

		//托管调整
		if (lTotalScore>m_lRestrictScore)
		{
			m_UserInfo.lScore=m_lRestrictScore;
			m_lTrusteeScore=lTotalScore-m_lRestrictScore;
		}
		else
		{
			m_lTrusteeScore=0L;
			m_UserInfo.lScore=lTotalScore;
		}
	}

	return true;
}

//修改信息
bool CServerUserItem::ModifyUserProperty(SCORE lScore,LONG lLoveLiness)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//设置积分
	m_UserInfo.lScore+=lScore;
	m_UserInfo.lLoveLiness+=lLoveLiness;

	//记录信息
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lLoveLiness+=lLoveLiness;

	//变更信息
	if (lLoveLiness!=0L)
	{
		m_VariationInfo.lLoveLiness+=lLoveLiness;
	}

	//托管积分
	if ((m_lRestrictScore>0L)&&(lScore!=0L))
	{
		//变量定义
		SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

		//托管调整
		if (lTotalScore>m_lRestrictScore)
		{
			m_UserInfo.lScore=m_lRestrictScore;
			m_lTrusteeScore=lTotalScore-m_lRestrictScore;
		}
		else
		{
			m_lTrusteeScore=0L;
			m_UserInfo.lScore=lTotalScore;
		}
	}

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, SCORE_REASON_PROPERTY);

	return true;
}

//解除绑定
bool CServerUserItem::DetachBindStatus()
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//用户属性
	m_bClientReady=false;

	//连接属性
	m_wBindIndex=INVALID_WORD;

	return true;
}

//银行操作
bool CServerUserItem::ModifyUserInsure(SCORE lScore, SCORE lInsure, SCORE lRevenue)
{
	//平衡效验
	/*ASSERT((m_bAndroidUser==true)||(((lScore+lInsure+lRevenue)==0L)&&(lRevenue>=0L)));
	if ((m_bAndroidUser==false)&&(((lScore+lInsure+lRevenue)!=0L)||(lRevenue<0L))) return false;*/

	//效验状态
	/*ASSERT((m_UserInfo.dwUserID!=0L)&&((m_UserInfo.lScore+m_lTrusteeScore+lScore)>=0L));
	if ((m_UserInfo.dwUserID==0L)||((m_UserInfo.lScore+m_lTrusteeScore+lScore)<0L)) return false;*/

	//设置积分
	m_UserInfo.lScore+=lScore;
	m_UserInfo.lInsure+=lInsure;

	//记录信息
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lInsure+=lInsure;
	m_RecordInfo.lRevenue+=lRevenue;

	//托管积分
	if (m_lRestrictScore>0L)
	{
		//变量定义
		SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

		//托管调整
		if (lTotalScore>m_lRestrictScore)
		{
			m_UserInfo.lScore=m_lRestrictScore;
			m_lTrusteeScore=lTotalScore-m_lRestrictScore;
		}
		else
		{
			m_lTrusteeScore=0L;
			m_UserInfo.lScore=lTotalScore;
		}
	}

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, SCORE_REASON_INSURE);

	return true;
}

//设置参数
bool CServerUserItem::SetUserParameter(DWORD dwClientAddr, WORD wBindIndex, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser, bool bClientReady)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//用户属性
	m_bAndroidUser=bAndroidUser;
	m_bClientReady=bClientReady;

	//连接属性
	m_wBindIndex=wBindIndex;
	m_dwClientAddr=dwClientAddr;
	lstrcpyn(m_szMachineID,szMachineID,CountArray(m_szMachineID));

	return true;
}

//修改权限
VOID CServerUserItem::ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind)
{
	//添加权限
	m_dwUserRight |= dwAddRight;

	//删除权限
	m_dwUserRight &= ~dwRemoveRight;

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemRight(this,dwAddRight,dwRemoveRight,cbRightKind);

	return;
}

//重置数据
VOID CServerUserItem::ResetUserItem()
{
	//附加变量
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//托管变量
	m_lTrusteeScore=0L;
	m_lRestrictScore=0L;
	m_lFrozenedScore=0L;
	m_lFrozenedScore=0L;

	//登录信息
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//系统属性
	m_wBindIndex=INVALID_WORD;
	m_dwClientAddr=INADDR_ANY;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//客户类型
	m_bMobileUser=false;
	m_bAndroidUser=false;

	//比赛信息
	m_dwSignUpTime=0;
	m_cbMatchStatus=0;
	m_pMatchData=NULL;

	//辅助变量
	m_bTrusteeUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));
	ZeroMemory(m_szUserGameData,sizeof(m_szUserGameData));

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wLastTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	//记录信息
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	//手机定义
	m_wMobileUserRule=0;
	m_wMobileUserRule |= VIEW_MODE_PART;
	m_wMobileUserRule |= VIEW_INFO_LEVEL_1;
	m_wDeskPos=0;

	//接口变量
	m_pIServerUserItemSink=NULL;
	m_pIMatchUserItemSink=NULL;

	//清空数据
	INT wKey=0;
	tagGameDataItem * pGameDataItem=NULL;
	POSITION Position=m_UserGameDataMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserGameDataMap.GetNextAssoc(Position,wKey,pGameDataItem);
		SafeDelete(pGameDataItem);
	}
	m_UserGameDataMap.RemoveAll();

	return;
}

//构建游戏数据
VOID CServerUserItem::InitUserGameDataMap()
{
	//清空数据
	INT wKey=0;
	tagGameDataItem * pGameDataItem=NULL;
	POSITION Position=m_UserGameDataMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserGameDataMap.GetNextAssoc(Position,wKey,pGameDataItem);
		SafeDelete(pGameDataItem);
	}
	m_UserGameDataMap.RemoveAll();

	//游戏数据格式如 Key1:Value1;Key2:Value2;Key3:Value3;Key4:Value4;
	CStringArray SubItemArray;
	SplitString(m_szUserGameData,g_szGameDataSpe,SubItemArray);

	INT nSubItemCount=(INT)SubItemArray.GetCount();
	ASSERT(nSubItemCount%2==0);
	if(nSubItemCount%2!=0) return;

	TCHAR szSubItem[32]=TEXT("");
	for (INT i=nSubItemCount-1; i>=0; i-=2)
	{
		_sntprintf(szSubItem,CountArray(szSubItem),TEXT("%s"),SubItemArray.GetAt(i-1));
	
		bool bPureDigit=IsPureDigitString(szSubItem);
		if (bPureDigit)
		{
			INT nKey=_ttoi(szSubItem);
			CString pszValue=SubItemArray.GetAt(i);

			tagGameDataItem *pNewDataItem=new tagGameDataItem;
			pNewDataItem->nKey=nKey;
			lstrcpyn(pNewDataItem->szValue,pszValue,CountArray(pNewDataItem->szValue));

			//加入集合
			m_UserGameDataMap[nKey]=pNewDataItem;
		}
	}

	return;
}

//字符串拆分
VOID CServerUserItem::SplitString(CString strSource, CString strSpliter, CStringArray& ResultArray)
{
	////参数校验
	//ASSERT(strSource.GetLength()>0 && strSpliter.GetLength()>0);
	//if (strSource.GetLength()<=0 || strSpliter.GetLength()<=0) return;

	//清空数组
	ResultArray.RemoveAll();

	int nPostion=0;
	while (1)
	{
		CString strToken=strSource.Tokenize(strSpliter,nPostion);
		if (strToken.IsEmpty())
			break;
		ResultArray.Add(strToken);
	}
}

//是否为纯数字字符串
bool CServerUserItem::IsPureDigitString(CString strSource)
{
	for (INT i=0; i< strSource.GetLength(); i++)
	{
		TCHAR chSplit=strSource[i];
		if (chSplit<TEXT('0') || chSplit>TEXT('9'))
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return true;
}

//有效性判断
bool CServerUserItem::IsValidGameData(CString strSource, CString strSpliter)
{
	////参数校验
	//ASSERT(strSource.GetLength()>0 && strSpliter.GetLength()>0);
	//if (strSource.GetLength()<=0 || strSpliter.GetLength()<=0) return false;

	CStringArray SubItemArray;
	SplitString(strSource,strSpliter,SubItemArray);

	INT nSubItemCount=(INT)SubItemArray.GetCount();
	ASSERT(nSubItemCount%2==0);
	if(nSubItemCount%2!=0) return false;

	TCHAR szSubItem[LEN_GAME_ITEM]=TEXT("");
	for (INT i=0; i<nSubItemCount; i+=2)
	{
		_sntprintf(szSubItem,CountArray(szSubItem),TEXT("%s"),SubItemArray.GetAt(i));

		bool bPureDigit=IsPureDigitString(szSubItem);
		bool bChString=SubItemArray.GetAt(i+1).GetLength()>0;

		if (bPureDigit==false || bChString==false)
			return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserManager::CServerUserManager()
{
	//组件接口
	m_pIServerUserItemSink=NULL;
	m_pIMatchUserItemSink=NULL;

	//设置质数
	m_UserIDMap.InitHashTable(PRIME_SERVER_USER);

	return;
}

//析构函数
CServerUserManager::~CServerUserManager()
{
	//释放用户
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++) m_UserItemStore[i]->Release();
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++) m_UserItemArray[i]->Release();

	//删除数据
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//接口查询
VOID * CServerUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserManager,Guid,dwQueryVer);
	return NULL;
}

//设置接口
bool CServerUserManager::SetServerUserItemSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink)!=NULL);
		m_pIServerUserItemSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink);

		//成功判断
		if (m_pIServerUserItemSink==NULL) return false;
	}
	else m_pIServerUserItemSink=NULL;

	return true;
}

//设置接口
bool CServerUserManager::SetMatchUserItemSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IMatchUserItemSink)!=NULL);
		m_pIMatchUserItemSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IMatchUserItemSink);

		//成功判断
		if (m_pIMatchUserItemSink==NULL) return false;
	}
	else m_pIMatchUserItemSink=NULL;

	return true;
}

//枚举用户
IServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//查找用户
IServerUserItem * CServerUserManager::SearchUserItem(DWORD dwUserID)
{
	return m_UserIDMap[dwUserID];
}

//机器人数
DWORD CServerUserManager::GetAndroidCount()
{
	//变量定义
	DWORD dwAndroidCount=0;
	CServerUserItem * pServerUserItem=NULL;

	//搜索用户
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if(pServerUserItem->IsAndroidUser() == true) dwAndroidCount++;
	}

	return dwAndroidCount;
}

//查找用户
IServerUserItem * CServerUserManager::SearchUserItem(LPCTSTR pszNickName)
{
	//变量定义
	CServerUserItem * pServerUserItem=NULL;

	//搜索用户
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if (pServerUserItem->ContrastNickName(pszNickName)==true) return pServerUserItem;
	}

	return NULL;
}

//删除用户
bool CServerUserManager::DeleteUserItem()
{
	//存储对象
	m_UserItemStore.Append(m_UserItemArray);

	//删除对象
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();

	return true;
}

//删除用户
bool CServerUserManager::DeleteUserItem(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()==US_NULL));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetUserStatus()!=US_NULL)) return false;

	//变量定义
	CServerUserItem * pTempUserItem=NULL;
	DWORD dwUserID=pIServerUserItem->GetUserID();

	//寻找对象
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//获取用户
		pTempUserItem=m_UserItemArray[i];
		if (pIServerUserItem!=pTempUserItem) continue;

		//重置对象
		pTempUserItem->ResetUserItem();

		//删除对象
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//插入用户
bool CServerUserManager::InsertUserItem(IServerUserItem * * pIServerUserResult, tagUserInfo & UserInfo,tagUserInfoPlus & UserInfoPlus)
{
	//变量定义
	CServerUserItem * pServerUserItem=NULL;

	//获取指针
	if (m_UserItemStore.GetCount()>0)
	{
		INT_PTR nItemPostion=m_UserItemStore.GetCount()-1;
		pServerUserItem=m_UserItemStore[nItemPostion];
		m_UserItemStore.RemoveAt(nItemPostion);

		//重置对象
		pServerUserItem->ResetUserItem();		
	}
	else
	{
		try
		{
			pServerUserItem=new CServerUserItem;
		}
		catch (...)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置接口
	pServerUserItem->m_pIServerUserItemSink=m_pIServerUserItemSink;
	pServerUserItem->m_pIMatchUserItemSink=m_pIMatchUserItemSink;

	//原始变量
	pServerUserItem->m_ScoreFormer.lScore=UserInfo.lScore;
	pServerUserItem->m_ScoreFormer.dwWinCount=UserInfo.dwWinCount;
	pServerUserItem->m_ScoreFormer.dwLostCount=UserInfo.dwLostCount;
	pServerUserItem->m_ScoreFormer.dwDrawCount=UserInfo.dwDrawCount;
	pServerUserItem->m_ScoreFormer.dwFleeCount=UserInfo.dwFleeCount;
	pServerUserItem->m_ScoreFormer.dwExperience=UserInfo.dwExperience;
	pServerUserItem->m_ScoreFormer.lIntegralCount = UserInfo.lIntegralCount;

	//属性变量
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));
	ZeroMemory(&pServerUserItem->m_RecordInfo,sizeof(pServerUserItem->m_RecordInfo));
	ZeroMemory(&pServerUserItem->m_VariationInfo,sizeof(pServerUserItem->m_VariationInfo));

	//登录信息
	pServerUserItem->m_dwLogonTime=UserInfoPlus.dwLogonTime;
	pServerUserItem->m_dwInoutIndex=UserInfoPlus.dwInoutIndex;

	//用户权限
	pServerUserItem->m_dwUserRight=UserInfoPlus.dwUserRight;
	pServerUserItem->m_dwMasterRight=UserInfoPlus.dwMasterRight;
	pServerUserItem->m_lRestrictScore=UserInfoPlus.lRestrictScore;

	//连接信息
	pServerUserItem->m_wBindIndex=UserInfoPlus.wBindIndex;
	pServerUserItem->m_dwClientAddr=UserInfoPlus.dwClientAddr;
	lstrcpyn(pServerUserItem->m_szMachineID,UserInfoPlus.szMachineID,CountArray(pServerUserItem->m_szMachineID));

	//辅助变量
	pServerUserItem->m_bClientReady=false;
	pServerUserItem->m_bModifyScore=false;
	pServerUserItem->m_bTrusteeUser=false;
	pServerUserItem->m_bMobileUser=UserInfoPlus.bMobileUser;
	pServerUserItem->m_bAndroidUser=UserInfoPlus.bAndroidUser;
	lstrcpyn(pServerUserItem->m_szLogonPass,UserInfoPlus.szPassword,CountArray(pServerUserItem->m_szLogonPass));
	lstrcpyn(pServerUserItem->m_szUserGameData,UserInfoPlus.szUserGameData,CountArray(pServerUserItem->m_szUserGameData));
	if (pServerUserItem->IsValidGameData(pServerUserItem->m_szUserGameData,g_szGameDataSpe)==true)
	{
		pServerUserItem->InitUserGameDataMap();
	}

	//托管调整
	if ((UserInfoPlus.lRestrictScore>0L)&&(UserInfo.lScore>UserInfoPlus.lRestrictScore))
	{
		pServerUserItem->m_UserInfo.lScore=UserInfoPlus.lRestrictScore;
		pServerUserItem->m_lTrusteeScore=UserInfo.lScore-UserInfoPlus.lRestrictScore;
	}

	//插入用户
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	//设置变量
	*pIServerUserResult=pServerUserItem;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
