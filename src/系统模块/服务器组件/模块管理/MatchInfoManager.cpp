#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "MatchInfoManager.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////
//构造函数
CMatchInfoBuffer::CMatchInfoBuffer()
{
}

//析构函数
CMatchInfoBuffer::~CMatchInfoBuffer()
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//删除数组
	m_GameMatchOptionBuffer.Append(m_GameMatchOptionArray);
	for (INT_PTR i=0;i<m_GameMatchOptionBuffer.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionBuffer[i];
		SafeDelete(pGameMatchOption);
	}

	//删除引用
	m_GameMatchOptionArray.RemoveAll();
	m_GameMatchOptionBuffer.RemoveAll();

	return;
}

//重置数据
bool CMatchInfoBuffer::ResetMatchOption()
{
	//添加对象
	m_GameMatchOptionBuffer.Append(m_GameMatchOptionArray);

	//删除元素
	m_GameMatchOptionArray.RemoveAll();

	return true;
}

//删除数据
bool CMatchInfoBuffer::DeleteMatchOption(DWORD dwMatchID)
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//查找对象
	for(INT_PTR i=0;i<m_GameMatchOptionArray.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionArray[i];
		if(pGameMatchOption->dwMatchID==dwMatchID)
		{
			//删除数据
			m_GameMatchOptionArray.RemoveAt(i);
			m_GameMatchOptionBuffer.Add(pGameMatchOption);

			//设置变量
			ZeroMemory(pGameMatchOption,sizeof(tagGameMatchOption));

			break;
		}
	}	

	return true;
}

//插入数据
tagGameMatchOption * CMatchInfoBuffer::InsertMatchOption(tagGameMatchOption * pGameMatchOption)
{
	//效验参数
	ASSERT(pGameMatchOption!=NULL);
	if (pGameMatchOption==NULL) return NULL;

	//查找现存
	tagGameMatchOption * pGameMatchInsert=SearchMatchOption(pGameMatchOption->dwMatchID);

	//创建判断
	if (pGameMatchInsert==NULL)
	{
		//创建对象
		pGameMatchInsert=CreateMatchOption();

		//结果判断
		if (pGameMatchInsert==NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		//添加数据
		m_GameMatchOptionArray.Add(pGameMatchInsert);
	}

	//拷贝数据	
	CopyMemory(pGameMatchInsert,pGameMatchOption,sizeof(tagGameMatchOption));

	return pGameMatchInsert;
}

//获取数目
DWORD CMatchInfoBuffer::GetMatchOptionCount()
{
	return (DWORD)(m_GameMatchOptionArray.GetCount());
}

//查找数据
tagGameMatchOption * CMatchInfoBuffer::SearchMatchOption(DWORD dwMatchID)
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//查找对象
	for(INT_PTR i=0;i<m_GameMatchOptionArray.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionArray[i];
		if(pGameMatchOption->dwMatchID==dwMatchID)
		{
			return pGameMatchOption;
		}
	}

	return NULL;
}

//创建对象
tagGameMatchOption * CMatchInfoBuffer::CreateMatchOption()
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//创建对象
	try
	{
		INT_PTR nArrayCount=m_GameMatchOptionBuffer.GetCount();
		if (nArrayCount>0)
		{
			pGameMatchOption=m_GameMatchOptionBuffer[nArrayCount-1];
			m_GameMatchOptionBuffer.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameMatchOption=new tagGameMatchOption;
			if (pGameMatchOption==NULL) return NULL;
		}
	}
	catch (...) 
	{ 
		return NULL; 
	}

	//设置变量
	ZeroMemory(pGameMatchOption,sizeof(tagGameMatchOption));

	return pGameMatchOption;
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CRewardInfoBuffer::CRewardInfoBuffer()
{
}

//析构函数
CRewardInfoBuffer::~CRewardInfoBuffer()
{
	//变量定义
	tagMatchRewardInfo * pMatchRewardInfo=NULL;

	//删除数组
	m_MatchRewardInfoBuffer.Append(m_MatchRewardInfoArray);
	for (INT_PTR i=0;i<m_MatchRewardInfoBuffer.GetCount();i++)
	{
		pMatchRewardInfo=m_MatchRewardInfoBuffer[i];
		SafeDelete(pMatchRewardInfo);
	}

	//删除引用
	m_MatchRewardInfoArray.RemoveAll();
	m_MatchRewardInfoBuffer.RemoveAll();

	return;
}

//重置数据
bool CRewardInfoBuffer::ResetMatchRewardInfo()
{
	//添加对象
	m_MatchRewardInfoBuffer.Append(m_MatchRewardInfoArray);

	//删除元素
	m_MatchRewardInfoArray.RemoveAll();

	return true;
}

//删除数据
bool CRewardInfoBuffer::DeleteMatchRewardInfo(WORD wRankID)
{
	//变量定义
	tagMatchRewardInfo * pMatchRewardInfo=NULL;

	//查找对象
	for (INT_PTR i=0;i<m_MatchRewardInfoArray.GetCount();i++)
	{
		pMatchRewardInfo=m_MatchRewardInfoArray[i];
		if (pMatchRewardInfo->wRankID==wRankID)
		{
			//删除数据
			m_MatchRewardInfoArray.RemoveAt(i);
			m_MatchRewardInfoBuffer.Add(pMatchRewardInfo);

			//设置变量
			ZeroMemory(pMatchRewardInfo,sizeof(tagMatchRewardInfo));

			break;
		}
	}	

	return true;
}

//插入数据
tagMatchRewardInfo * CRewardInfoBuffer::InsertMatchRewardInfo(tagMatchRewardInfo & MatchRewardInfo)
{
	//查找现存
	tagMatchRewardInfo * pMatchRewardInsert=SearchMatchReward(MatchRewardInfo.wRankID);

	//创建判断
	if (pMatchRewardInsert==NULL)
	{
		//创建对象
		pMatchRewardInsert=CreateMatchRewardInfo();

		//结果判断
		if (pMatchRewardInsert==NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		//添加数据
		m_MatchRewardInfoArray.Add(pMatchRewardInsert);		
	}

	//拷贝数据
	CopyMemory(pMatchRewardInsert,&MatchRewardInfo,sizeof(tagMatchRewardInfo));

	return pMatchRewardInsert;
}

//获取数目
DWORD CRewardInfoBuffer::GetMatchRewardCount()
{
	return (DWORD)(m_MatchRewardInfoArray.GetCount());
}
	
//查找数据
tagMatchRewardInfo * CRewardInfoBuffer::SearchMatchReward(WORD wRankID)
{
	//变量定义
	tagMatchRewardInfo * pMatchRewardInfo=NULL;

	//查找对象
	for (INT_PTR i=0;i<m_MatchRewardInfoArray.GetCount();i++)
	{
		pMatchRewardInfo=m_MatchRewardInfoArray[i];
		if (pMatchRewardInfo->wRankID==wRankID)
		{
			return pMatchRewardInfo;
		}
	}

	return NULL;
}

//创建对象
tagMatchRewardInfo * CRewardInfoBuffer::CreateMatchRewardInfo()
{
	//变量定义
	tagMatchRewardInfo * pMatchRewardInfo=NULL;

	//创建对象
	try
	{
		INT_PTR nArrayCount=m_MatchRewardInfoBuffer.GetCount();
		if (nArrayCount>0)
		{
			pMatchRewardInfo=m_MatchRewardInfoBuffer[nArrayCount-1];
			m_MatchRewardInfoBuffer.RemoveAt(nArrayCount-1);
		}
		else
		{
			pMatchRewardInfo=new tagMatchRewardInfo;
			if (pMatchRewardInfo==NULL) return NULL;
		}
	}
	catch (...) 
	{ 
		return NULL; 
	}

	//设置变量
	ZeroMemory(pMatchRewardInfo,sizeof(tagMatchRewardInfo));

	return pMatchRewardInfo;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CMatchInfoManager::CMatchInfoManager()
{
}

//析构函数
CMatchInfoManager::~CMatchInfoManager()
{
}

//加载奖励
WORD CMatchInfoManager::LoadGameMatchReward(DWORD dwMatchID,CRewardInfoBuffer & RewardInfoBuffer)
{
	//变量定义
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//插入参数
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),dwMatchID);

		//读取列表
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_LoadMatchReward"),true)==DB_SUCCESS)
		{
			//变量定义
			tagMatchRewardInfo MatchRewardInfo={0};

			while (GameMatchDBModule->IsRecordsetEnd()==false)
			{
				//读取数据
				MatchRewardInfo.wRankID=GameMatchDBAide.GetValue_WORD(TEXT("MatchRank"));
				MatchRewardInfo.lRewardGold=GameMatchDBAide.GetValue_DOUBLE(TEXT("RewardGold"));
				MatchRewardInfo.lRewardIngot=GameMatchDBAide.GetValue_DOUBLE(TEXT("RewardIngot"));
				MatchRewardInfo.dwRewardExperience=GameMatchDBAide.GetValue_DWORD(TEXT("RewardExperience"));

				//添加奖励
				RewardInfoBuffer.InsertMatchRewardInfo(MatchRewardInfo);

				//移动游标
				GameMatchDBModule->MoveToNext();
			}
		}		

		return (WORD)RewardInfoBuffer.GetMatchRewardCount();
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return 0;
	}

	return 0;
}

//加载房间
bool CMatchInfoManager::LoadGameMatchOption(WORD wKindID,DWORD dwMatchID,tagGameMatchOption & GameMatchResult)
{
	//变量定义
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//读取列表
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@wKindID"),wKindID);
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),dwMatchID);
		GameMatchDBAide.AddParameter(TEXT("@strServiceMachine"),TEXT(""));

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//读取列表
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameMatchItem"),true)!=DB_SUCCESS)
		{
			//获取信息
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取数据
		if (GameMatchDBModule->IsRecordsetEnd()==false)
		{
			ReadGameMatchOption(GameMatchDBAide,GameMatchResult);

			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//加载比赛
bool CMatchInfoManager::LoadGameMatchOption(WORD wKindID,TCHAR szMachineID[LEN_MACHINE_ID],CMatchInfoBuffer & MatchInfoBuffer)
{
	//变量定义
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//插入参数
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@wKindID"),wKindID);
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),0);
		GameMatchDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//读取列表
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameMatchItem"),true)!=DB_SUCCESS)
		{
			//获取信息
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//清空列表
		MatchInfoBuffer.ResetMatchOption();

		//读取列表
		while (GameMatchDBModule->IsRecordsetEnd()==false)
		{
			//读取数据
			tagGameMatchOption GameMatchResult;
			ReadGameMatchOption(GameMatchDBAide,GameMatchResult);

			//插入列表
			MatchInfoBuffer.InsertMatchOption(&GameMatchResult);

			//移动记录
			GameMatchDBModule->MoveToNext();
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//删除房间
bool CMatchInfoManager::DeleteGameMatchOption(DWORD dwMatchID)
{
	//变量定义
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//插入参数
		GameMatchDBAide.ResetParameter();		
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),dwMatchID);

		//读取列表
		LONG lResultCode=GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_DeleteGameMatchItem"),false);
		
		return lResultCode==DB_SUCCESS;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//插入房间
bool CMatchInfoManager::InsertGameMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagGameMatchOption & GameMatchResult)
{
	//变量定义
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());		

		//比赛规则
		TCHAR szMatchRule[CountArray(pCreateMatchInfo->cbMatchRule)*2+1]=TEXT("");
		for (INT i=0;i<CountArray(pCreateMatchInfo->cbMatchRule);i++) _stprintf(&szMatchRule[i*2],TEXT("%02X"),pCreateMatchInfo->cbMatchRule[i]);

		//插入参数
		GameMatchDBAide.ResetParameter();		
 		GameMatchDBAide.AddParameter(TEXT("@wKindID"),pCreateMatchInfo->wKindID);							
		GameMatchDBAide.AddParameter(TEXT("@strMatchName"),pCreateMatchInfo->szMatchName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchDate"),pCreateMatchInfo->szMatchDate);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchType"),pCreateMatchInfo->cbMatchType);		

		//报名参数
		GameMatchDBAide.AddParameter(TEXT("@cbFeeType"),pCreateMatchInfo->cbFeeType);
		GameMatchDBAide.AddParameter(TEXT("@cbDeductArea"),pCreateMatchInfo->cbDeductArea);		
		GameMatchDBAide.AddParameter(TEXT("@lSignupFee"),pCreateMatchInfo->lSignupFee);
		GameMatchDBAide.AddParameter(TEXT("@cbSignupMode"),pCreateMatchInfo->cbSignupMode);		
		GameMatchDBAide.AddParameter(TEXT("@cbJoinCondition"),pCreateMatchInfo->cbJoinCondition);
		GameMatchDBAide.AddParameter(TEXT("@cbMemberOrder"),pCreateMatchInfo->cbMemberOrder);
		GameMatchDBAide.AddParameter(TEXT("@dwExperience"),pCreateMatchInfo->dwExperience);
		GameMatchDBAide.AddParameter(TEXT("@dwFromMatchID"),pCreateMatchInfo->dwFromMatchID);
		GameMatchDBAide.AddParameter(TEXT("@cbFilterType"),pCreateMatchInfo->cbFilterType);
		GameMatchDBAide.AddParameter(TEXT("@wMaxRankID"),pCreateMatchInfo->wMaxRankID);
		GameMatchDBAide.AddParameter(TEXT("@MatchEndDate"),pCreateMatchInfo->MatchEndDate);
		GameMatchDBAide.AddParameter(TEXT("@MatchStartDate"),pCreateMatchInfo->MatchStartDate);

		//排名参数
		GameMatchDBAide.AddParameter(TEXT("@cbRankingMode"),pCreateMatchInfo->cbRankingMode);
		GameMatchDBAide.AddParameter(TEXT("@wCountInnings"),pCreateMatchInfo->wCountInnings);
		GameMatchDBAide.AddParameter(TEXT("@cbFilterGradesMode"),pCreateMatchInfo->cbFilterGradesMode);

		//配桌参数
		GameMatchDBAide.AddParameter(TEXT("@cbDistributeRule"),pCreateMatchInfo->cbDistributeRule);
		GameMatchDBAide.AddParameter(TEXT("@wMinDistributeUser"),pCreateMatchInfo->wMinDistributeUser);
		GameMatchDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pCreateMatchInfo->wDistributeTimeSpace);
		GameMatchDBAide.AddParameter(TEXT("@wMinPartakeGameUser"),pCreateMatchInfo->wMinPartakeGameUser);
		GameMatchDBAide.AddParameter(TEXT("@wMaxPartakeGameUser"),pCreateMatchInfo->wMaxPartakeGameUser);

		//比赛奖励
		GameMatchDBAide.AddParameter(TEXT("@strRewardGold"),pCreateMatchInfo->szRewardGold);
		GameMatchDBAide.AddParameter(TEXT("@strRewardIngot"),pCreateMatchInfo->szRewardIngot);
		GameMatchDBAide.AddParameter(TEXT("@strRewardExperience"),pCreateMatchInfo->szRewardExperience);

		//扩展配置
		GameMatchDBAide.AddParameter(TEXT("@strMatchRule"),szMatchRule);
		GameMatchDBAide.AddParameter(TEXT("@strServiceMachine"),pCreateMatchInfo->szServiceMachine);		

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_InsertGameMatchItem"),true)!=DB_SUCCESS)
		{
			//获取信息
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取配置
		ReadGameMatchOption(GameMatchDBAide,GameMatchResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//修改房间
bool CMatchInfoManager::ModifyGameMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagGameMatchOption & GameMatchResult)
{
	//变量定义
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//比赛规则
		TCHAR szMatchRule[CountArray(pCreateMatchInfo->cbMatchRule)*2+1]=TEXT("");
		for (INT i=0;i<CountArray(pCreateMatchInfo->cbMatchRule);i++) _stprintf(&szMatchRule[i*2],TEXT("%02X"),pCreateMatchInfo->cbMatchRule[i]);

		//基础参数
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@wKindID"),pCreateMatchInfo->wKindID);	
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),pCreateMatchInfo->dwMatchID);		
		GameMatchDBAide.AddParameter(TEXT("@strMatchName"),pCreateMatchInfo->szMatchName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchDate"),pCreateMatchInfo->szMatchDate);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchType"),pCreateMatchInfo->cbMatchType);

		//报名参数
		GameMatchDBAide.AddParameter(TEXT("@cbFeeType"),pCreateMatchInfo->cbFeeType);
		GameMatchDBAide.AddParameter(TEXT("@cbDeductArea"),pCreateMatchInfo->cbDeductArea);		
		GameMatchDBAide.AddParameter(TEXT("@lSignupFee"),pCreateMatchInfo->lSignupFee);
		GameMatchDBAide.AddParameter(TEXT("@cbSignupMode"),pCreateMatchInfo->cbSignupMode);		
		GameMatchDBAide.AddParameter(TEXT("@cbJoinCondition"),pCreateMatchInfo->cbJoinCondition);
		GameMatchDBAide.AddParameter(TEXT("@cbMemberOrder"),pCreateMatchInfo->cbMemberOrder);
		GameMatchDBAide.AddParameter(TEXT("@dwExperience"),pCreateMatchInfo->dwExperience);
		GameMatchDBAide.AddParameter(TEXT("@dwFromMatchID"),pCreateMatchInfo->dwFromMatchID);
		GameMatchDBAide.AddParameter(TEXT("@cbFilterType"),pCreateMatchInfo->cbFilterType);
		GameMatchDBAide.AddParameter(TEXT("@wMaxRankID"),pCreateMatchInfo->wMaxRankID);
		GameMatchDBAide.AddParameter(TEXT("@MatchEndDate"),pCreateMatchInfo->MatchEndDate);
		GameMatchDBAide.AddParameter(TEXT("@MatchStartDate"),pCreateMatchInfo->MatchStartDate);

		//排名参数
		GameMatchDBAide.AddParameter(TEXT("@cbRankingMode"),pCreateMatchInfo->cbRankingMode);
		GameMatchDBAide.AddParameter(TEXT("@wCountInnings"),pCreateMatchInfo->wCountInnings);
		GameMatchDBAide.AddParameter(TEXT("@cbFilterGradesMode"),pCreateMatchInfo->cbFilterGradesMode);

		//配桌参数
		GameMatchDBAide.AddParameter(TEXT("@cbDistributeRule"),pCreateMatchInfo->cbDistributeRule);
		GameMatchDBAide.AddParameter(TEXT("@wMinDistributeUser"),pCreateMatchInfo->wMinDistributeUser);
		GameMatchDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pCreateMatchInfo->wDistributeTimeSpace);
		GameMatchDBAide.AddParameter(TEXT("@wMinPartakeGameUser"),pCreateMatchInfo->wMinPartakeGameUser);
		GameMatchDBAide.AddParameter(TEXT("@wMaxPartakeGameUser"),pCreateMatchInfo->wMaxPartakeGameUser);

		//比赛奖励
		GameMatchDBAide.AddParameter(TEXT("@strRewardGold"),pCreateMatchInfo->szRewardGold);
		GameMatchDBAide.AddParameter(TEXT("@strRewardIngot"),pCreateMatchInfo->szRewardIngot);	
		GameMatchDBAide.AddParameter(TEXT("@strRewardExperience"),pCreateMatchInfo->szRewardExperience);

		//比赛规则
		GameMatchDBAide.AddParameter(TEXT("@strMatchRule"),szMatchRule);		

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_ModifyGameMatchItem"),true)!=DB_SUCCESS)
		{
			//获取信息
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取配置
		ReadGameMatchOption(GameMatchDBAide,GameMatchResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}



//连接数据
bool CMatchInfoManager::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
{
	//获取参数
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("CreateGameServer 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//设置连接
	PlatformDBModule->SetConnectionInfo(pDataBaseParameter->szDataBaseAddr,pDataBaseParameter->wDataBasePort,
		pDataBaseParameter->szDataBaseName,pDataBaseParameter->szDataBaseUser,pDataBaseParameter->szDataBasePass);

	//发起连接
	PlatformDBModule->OpenConnection();

	return true;
}

//连接数据
bool CMatchInfoManager::ConnectGameMatchDB(CDataBaseHelper & GameMatchDBModule)
{
	//获取参数
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetGameMatchDBParameter();

	//创建对象
	if ((GameMatchDBModule.GetInterface()==NULL)&&(GameMatchDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("ConnectGameMatchDB 创建 GameMatchDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//设置连接
	GameMatchDBModule->SetConnectionInfo(pDataBaseParameter->szDataBaseAddr,pDataBaseParameter->wDataBasePort,
		pDataBaseParameter->szDataBaseName,pDataBaseParameter->szDataBaseUser,pDataBaseParameter->szDataBasePass);

	//发起连接
	GameMatchDBModule->OpenConnection();

	return true;
}

//读取比赛
bool CMatchInfoManager::ReadGameMatchOption(CDataBaseAide & GameMatchDBAide, tagGameMatchOption & GameMatchResult)
{
	//设置变量
	ZeroMemory(&GameMatchResult,sizeof(GameMatchResult));

	//索引变量	
	GameMatchResult.dwMatchID=GameMatchDBAide.GetValue_DWORD(TEXT("MatchID"));	
	GameMatchResult.cbMatchStatus=GameMatchDBAide.GetValue_BYTE(TEXT("MatchStatus"));

	//比赛信息
	GameMatchResult.cbMatchType = GameMatchDBAide.GetValue_BYTE(TEXT("MatchType"));		
	GameMatchDBAide.GetValue_String(TEXT("MatchName"),GameMatchResult.szMatchName,CountArray(GameMatchResult.szMatchName));

	//报名信息
	GameMatchResult.cbFeeType= GameMatchDBAide.GetValue_BYTE(TEXT("FeeType"));
	GameMatchResult.cbDeductArea= GameMatchDBAide.GetValue_BYTE(TEXT("DeductArea"));
	GameMatchResult.lSignupFee= GameMatchDBAide.GetValue_DOUBLE(TEXT("SignupFee"));
	GameMatchResult.cbSignupMode= GameMatchDBAide.GetValue_BYTE(TEXT("SignupMode"));	
	GameMatchResult.cbJoinCondition = GameMatchDBAide.GetValue_BYTE(TEXT("JoinCondition"));
	GameMatchResult.cbMemberOrder = GameMatchDBAide.GetValue_BYTE(TEXT("MemberOrder"));
	GameMatchResult.dwExperience = GameMatchDBAide.GetValue_DWORD(TEXT("Experience"));
	GameMatchResult.dwFromMatchID = GameMatchDBAide.GetValue_DWORD(TEXT("FromMatchID"));	
	GameMatchResult.cbFilterType = GameMatchDBAide.GetValue_BYTE(TEXT("FilterType"));
	GameMatchResult.wMaxRankID = GameMatchDBAide.GetValue_WORD(TEXT("MaxRankID"));
	GameMatchDBAide.GetValue_SystemTime(TEXT("MatchEndDate"),GameMatchResult.MatchEndDate);
	GameMatchDBAide.GetValue_SystemTime(TEXT("MatchStartDate"),GameMatchResult.MatchStartDate);

	//排名参数
	GameMatchResult.cbRankingMode = GameMatchDBAide.GetValue_BYTE(TEXT("RankingMode"));
	GameMatchResult.wCountInnings = GameMatchDBAide.GetValue_WORD(TEXT("CountInnings"));
	GameMatchResult.cbFilterGradesMode = GameMatchDBAide.GetValue_BYTE(TEXT("FilterGradesMode"));

	//配桌参数
	GameMatchResult.cbDistributeRule=GameMatchDBAide.GetValue_BYTE(TEXT("DistributeRule"));
	GameMatchResult.wMinDistributeUser=GameMatchDBAide.GetValue_WORD(TEXT("MinDistributeUser"));
	GameMatchResult.wDistributeTimeSpace=GameMatchDBAide.GetValue_WORD(TEXT("DistributeTimeSpace"));
	GameMatchResult.wMinPartakeGameUser=GameMatchDBAide.GetValue_WORD(TEXT("MinPartakeGameUser"));
	GameMatchResult.wMaxPartakeGameUser=GameMatchDBAide.GetValue_WORD(TEXT("MaxPartakeGameUser"));

	//比赛规则
	TCHAR szMatchRule[CountArray(GameMatchResult.cbMatchRule)*2+1];
	GameMatchDBAide.GetValue_String(TEXT("MatchRule"),szMatchRule,CountArray(szMatchRule));

	//扩展配置
	if (szMatchRule[0]!=0)
	{
		//获取长度
		ASSERT(lstrlen(szMatchRule)%2==0);
		INT nMatchRuleSize=lstrlen(szMatchRule)/2;

		//转换字符
		for (INT i=0;i<nMatchRuleSize;i++)
		{
			//获取字符
			TCHAR cbChar1=szMatchRule[i*2];
			TCHAR cbChar2=szMatchRule[i*2+1];

			//效验字符
			ASSERT((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))||(cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F')));
			ASSERT((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))||(cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F')));

			//生成结果
			if ((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))) GameMatchResult.cbMatchRule[i]+=(cbChar2-TEXT('0'));
			if ((cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F'))) GameMatchResult.cbMatchRule[i]+=(cbChar2-TEXT('A')+0x0A);
			if ((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))) GameMatchResult.cbMatchRule[i]+=(cbChar1-TEXT('0'))*0x10;
			if ((cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F'))) GameMatchResult.cbMatchRule[i]+=(cbChar1-TEXT('A')+0x0A)*0x10;
		}
	}

	//加载奖励
	CRewardInfoBuffer RewardInfoBuffer;
	LoadGameMatchReward(GameMatchResult.dwMatchID,RewardInfoBuffer);	

	//奖励人数
	GameMatchResult.wRewardCount=(WORD)__min(RewardInfoBuffer.GetMatchRewardCount(),CountArray(GameMatchResult.MatchRewardInfo));

	//变量定义
	tagMatchRewardInfo * pMatchRewardInfo=NULL;
	for (WORD i=0;i<GameMatchResult.wRewardCount;i++)
	{
		//查询奖励
		pMatchRewardInfo=RewardInfoBuffer.SearchMatchReward(i+1);
		if (pMatchRewardInfo==NULL) break;

		//拷贝奖励
		CopyMemory(&GameMatchResult.MatchRewardInfo[i],pMatchRewardInfo,sizeof(GameMatchResult.MatchRewardInfo[0]));
	}	

	return true;
}

//////////////////////////////////////////////////////////////////////////
