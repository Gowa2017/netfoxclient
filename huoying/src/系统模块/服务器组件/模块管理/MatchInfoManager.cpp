#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "MatchInfoManager.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////
//���캯��
CMatchInfoBuffer::CMatchInfoBuffer()
{
}

//��������
CMatchInfoBuffer::~CMatchInfoBuffer()
{
	//��������
	tagGameMatchOption * pGameMatchOption=NULL;

	//ɾ������
	m_GameMatchOptionBuffer.Append(m_GameMatchOptionArray);
	for (INT_PTR i=0;i<m_GameMatchOptionBuffer.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionBuffer[i];
		SafeDelete(pGameMatchOption);
	}

	//ɾ������
	m_GameMatchOptionArray.RemoveAll();
	m_GameMatchOptionBuffer.RemoveAll();

	return;
}

//��������
bool CMatchInfoBuffer::ResetMatchOption()
{
	//��Ӷ���
	m_GameMatchOptionBuffer.Append(m_GameMatchOptionArray);

	//ɾ��Ԫ��
	m_GameMatchOptionArray.RemoveAll();

	return true;
}

//ɾ������
bool CMatchInfoBuffer::DeleteMatchOption(DWORD dwMatchID)
{
	//��������
	tagGameMatchOption * pGameMatchOption=NULL;

	//���Ҷ���
	for(INT_PTR i=0;i<m_GameMatchOptionArray.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionArray[i];
		if(pGameMatchOption->dwMatchID==dwMatchID)
		{
			//ɾ������
			m_GameMatchOptionArray.RemoveAt(i);
			m_GameMatchOptionBuffer.Add(pGameMatchOption);

			//���ñ���
			ZeroMemory(pGameMatchOption,sizeof(tagGameMatchOption));

			break;
		}
	}	

	return true;
}

//��������
tagGameMatchOption * CMatchInfoBuffer::InsertMatchOption(tagGameMatchOption * pGameMatchOption)
{
	//Ч�����
	ASSERT(pGameMatchOption!=NULL);
	if (pGameMatchOption==NULL) return NULL;

	//�����ִ�
	tagGameMatchOption * pGameMatchInsert=SearchMatchOption(pGameMatchOption->dwMatchID);

	//�����ж�
	if (pGameMatchInsert==NULL)
	{
		//��������
		pGameMatchInsert=CreateMatchOption();

		//����ж�
		if (pGameMatchInsert==NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		//�������
		m_GameMatchOptionArray.Add(pGameMatchInsert);
	}

	//��������	
	CopyMemory(pGameMatchInsert,pGameMatchOption,sizeof(tagGameMatchOption));

	return pGameMatchInsert;
}

//��ȡ��Ŀ
DWORD CMatchInfoBuffer::GetMatchOptionCount()
{
	return (DWORD)(m_GameMatchOptionArray.GetCount());
}

//��������
tagGameMatchOption * CMatchInfoBuffer::SearchMatchOption(DWORD dwMatchID)
{
	//��������
	tagGameMatchOption * pGameMatchOption=NULL;

	//���Ҷ���
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

//��������
tagGameMatchOption * CMatchInfoBuffer::CreateMatchOption()
{
	//��������
	tagGameMatchOption * pGameMatchOption=NULL;

	//��������
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

	//���ñ���
	ZeroMemory(pGameMatchOption,sizeof(tagGameMatchOption));

	return pGameMatchOption;
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CRewardInfoBuffer::CRewardInfoBuffer()
{
}

//��������
CRewardInfoBuffer::~CRewardInfoBuffer()
{
	//��������
	tagMatchRewardInfo * pMatchRewardInfo=NULL;

	//ɾ������
	m_MatchRewardInfoBuffer.Append(m_MatchRewardInfoArray);
	for (INT_PTR i=0;i<m_MatchRewardInfoBuffer.GetCount();i++)
	{
		pMatchRewardInfo=m_MatchRewardInfoBuffer[i];
		SafeDelete(pMatchRewardInfo);
	}

	//ɾ������
	m_MatchRewardInfoArray.RemoveAll();
	m_MatchRewardInfoBuffer.RemoveAll();

	return;
}

//��������
bool CRewardInfoBuffer::ResetMatchRewardInfo()
{
	//��Ӷ���
	m_MatchRewardInfoBuffer.Append(m_MatchRewardInfoArray);

	//ɾ��Ԫ��
	m_MatchRewardInfoArray.RemoveAll();

	return true;
}

//ɾ������
bool CRewardInfoBuffer::DeleteMatchRewardInfo(WORD wRankID)
{
	//��������
	tagMatchRewardInfo * pMatchRewardInfo=NULL;

	//���Ҷ���
	for (INT_PTR i=0;i<m_MatchRewardInfoArray.GetCount();i++)
	{
		pMatchRewardInfo=m_MatchRewardInfoArray[i];
		if (pMatchRewardInfo->wRankID==wRankID)
		{
			//ɾ������
			m_MatchRewardInfoArray.RemoveAt(i);
			m_MatchRewardInfoBuffer.Add(pMatchRewardInfo);

			//���ñ���
			ZeroMemory(pMatchRewardInfo,sizeof(tagMatchRewardInfo));

			break;
		}
	}	

	return true;
}

//��������
tagMatchRewardInfo * CRewardInfoBuffer::InsertMatchRewardInfo(tagMatchRewardInfo & MatchRewardInfo)
{
	//�����ִ�
	tagMatchRewardInfo * pMatchRewardInsert=SearchMatchReward(MatchRewardInfo.wRankID);

	//�����ж�
	if (pMatchRewardInsert==NULL)
	{
		//��������
		pMatchRewardInsert=CreateMatchRewardInfo();

		//����ж�
		if (pMatchRewardInsert==NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		//�������
		m_MatchRewardInfoArray.Add(pMatchRewardInsert);		
	}

	//��������
	CopyMemory(pMatchRewardInsert,&MatchRewardInfo,sizeof(tagMatchRewardInfo));

	return pMatchRewardInsert;
}

//��ȡ��Ŀ
DWORD CRewardInfoBuffer::GetMatchRewardCount()
{
	return (DWORD)(m_MatchRewardInfoArray.GetCount());
}
	
//��������
tagMatchRewardInfo * CRewardInfoBuffer::SearchMatchReward(WORD wRankID)
{
	//��������
	tagMatchRewardInfo * pMatchRewardInfo=NULL;

	//���Ҷ���
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

//��������
tagMatchRewardInfo * CRewardInfoBuffer::CreateMatchRewardInfo()
{
	//��������
	tagMatchRewardInfo * pMatchRewardInfo=NULL;

	//��������
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

	//���ñ���
	ZeroMemory(pMatchRewardInfo,sizeof(tagMatchRewardInfo));

	return pMatchRewardInfo;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CMatchInfoManager::CMatchInfoManager()
{
}

//��������
CMatchInfoManager::~CMatchInfoManager()
{
}

//���ؽ���
WORD CMatchInfoManager::LoadGameMatchReward(DWORD dwMatchID,CRewardInfoBuffer & RewardInfoBuffer)
{
	//��������
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//�������
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),dwMatchID);

		//��ȡ�б�
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_LoadMatchReward"),true)==DB_SUCCESS)
		{
			//��������
			tagMatchRewardInfo MatchRewardInfo={0};

			while (GameMatchDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				MatchRewardInfo.wRankID=GameMatchDBAide.GetValue_WORD(TEXT("MatchRank"));
				MatchRewardInfo.lRewardGold=GameMatchDBAide.GetValue_DOUBLE(TEXT("RewardGold"));
				MatchRewardInfo.lRewardIngot=GameMatchDBAide.GetValue_DOUBLE(TEXT("RewardIngot"));
				MatchRewardInfo.dwRewardExperience=GameMatchDBAide.GetValue_DWORD(TEXT("RewardExperience"));

				//��ӽ���
				RewardInfoBuffer.InsertMatchRewardInfo(MatchRewardInfo);

				//�ƶ��α�
				GameMatchDBModule->MoveToNext();
			}
		}		

		return (WORD)RewardInfoBuffer.GetMatchRewardCount();
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return 0;
	}

	return 0;
}

//���ط���
bool CMatchInfoManager::LoadGameMatchOption(WORD wKindID,DWORD dwMatchID,tagGameMatchOption & GameMatchResult)
{
	//��������
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//��ȡ�б�
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@wKindID"),wKindID);
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),dwMatchID);
		GameMatchDBAide.AddParameter(TEXT("@strServiceMachine"),TEXT(""));

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//��ȡ�б�
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameMatchItem"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		if (GameMatchDBModule->IsRecordsetEnd()==false)
		{
			ReadGameMatchOption(GameMatchDBAide,GameMatchResult);

			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//���ر���
bool CMatchInfoManager::LoadGameMatchOption(WORD wKindID,TCHAR szMachineID[LEN_MACHINE_ID],CMatchInfoBuffer & MatchInfoBuffer)
{
	//��������
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//�������
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@wKindID"),wKindID);
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),0);
		GameMatchDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//��ȡ�б�
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameMatchItem"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//����б�
		MatchInfoBuffer.ResetMatchOption();

		//��ȡ�б�
		while (GameMatchDBModule->IsRecordsetEnd()==false)
		{
			//��ȡ����
			tagGameMatchOption GameMatchResult;
			ReadGameMatchOption(GameMatchDBAide,GameMatchResult);

			//�����б�
			MatchInfoBuffer.InsertMatchOption(&GameMatchResult);

			//�ƶ���¼
			GameMatchDBModule->MoveToNext();
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//ɾ������
bool CMatchInfoManager::DeleteGameMatchOption(DWORD dwMatchID)
{
	//��������
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//�������
		GameMatchDBAide.ResetParameter();		
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),dwMatchID);

		//��ȡ�б�
		LONG lResultCode=GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_DeleteGameMatchItem"),false);
		
		return lResultCode==DB_SUCCESS;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//���뷿��
bool CMatchInfoManager::InsertGameMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagGameMatchOption & GameMatchResult)
{
	//��������
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());		

		//��������
		TCHAR szMatchRule[CountArray(pCreateMatchInfo->cbMatchRule)*2+1]=TEXT("");
		for (INT i=0;i<CountArray(pCreateMatchInfo->cbMatchRule);i++) _stprintf(&szMatchRule[i*2],TEXT("%02X"),pCreateMatchInfo->cbMatchRule[i]);

		//�������
		GameMatchDBAide.ResetParameter();		
 		GameMatchDBAide.AddParameter(TEXT("@wKindID"),pCreateMatchInfo->wKindID);							
		GameMatchDBAide.AddParameter(TEXT("@strMatchName"),pCreateMatchInfo->szMatchName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchDate"),pCreateMatchInfo->szMatchDate);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchType"),pCreateMatchInfo->cbMatchType);		

		//��������
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

		//��������
		GameMatchDBAide.AddParameter(TEXT("@cbRankingMode"),pCreateMatchInfo->cbRankingMode);
		GameMatchDBAide.AddParameter(TEXT("@wCountInnings"),pCreateMatchInfo->wCountInnings);
		GameMatchDBAide.AddParameter(TEXT("@cbFilterGradesMode"),pCreateMatchInfo->cbFilterGradesMode);

		//��������
		GameMatchDBAide.AddParameter(TEXT("@cbDistributeRule"),pCreateMatchInfo->cbDistributeRule);
		GameMatchDBAide.AddParameter(TEXT("@wMinDistributeUser"),pCreateMatchInfo->wMinDistributeUser);
		GameMatchDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pCreateMatchInfo->wDistributeTimeSpace);
		GameMatchDBAide.AddParameter(TEXT("@wMinPartakeGameUser"),pCreateMatchInfo->wMinPartakeGameUser);
		GameMatchDBAide.AddParameter(TEXT("@wMaxPartakeGameUser"),pCreateMatchInfo->wMaxPartakeGameUser);

		//��������
		GameMatchDBAide.AddParameter(TEXT("@strRewardGold"),pCreateMatchInfo->szRewardGold);
		GameMatchDBAide.AddParameter(TEXT("@strRewardIngot"),pCreateMatchInfo->szRewardIngot);
		GameMatchDBAide.AddParameter(TEXT("@strRewardExperience"),pCreateMatchInfo->szRewardExperience);

		//��չ����
		GameMatchDBAide.AddParameter(TEXT("@strMatchRule"),szMatchRule);
		GameMatchDBAide.AddParameter(TEXT("@strServiceMachine"),pCreateMatchInfo->szServiceMachine);		

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_InsertGameMatchItem"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadGameMatchOption(GameMatchDBAide,GameMatchResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//�޸ķ���
bool CMatchInfoManager::ModifyGameMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagGameMatchOption & GameMatchResult)
{
	//��������
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//��������
		TCHAR szMatchRule[CountArray(pCreateMatchInfo->cbMatchRule)*2+1]=TEXT("");
		for (INT i=0;i<CountArray(pCreateMatchInfo->cbMatchRule);i++) _stprintf(&szMatchRule[i*2],TEXT("%02X"),pCreateMatchInfo->cbMatchRule[i]);

		//��������
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@wKindID"),pCreateMatchInfo->wKindID);	
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),pCreateMatchInfo->dwMatchID);		
		GameMatchDBAide.AddParameter(TEXT("@strMatchName"),pCreateMatchInfo->szMatchName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchDate"),pCreateMatchInfo->szMatchDate);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchType"),pCreateMatchInfo->cbMatchType);

		//��������
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

		//��������
		GameMatchDBAide.AddParameter(TEXT("@cbRankingMode"),pCreateMatchInfo->cbRankingMode);
		GameMatchDBAide.AddParameter(TEXT("@wCountInnings"),pCreateMatchInfo->wCountInnings);
		GameMatchDBAide.AddParameter(TEXT("@cbFilterGradesMode"),pCreateMatchInfo->cbFilterGradesMode);

		//��������
		GameMatchDBAide.AddParameter(TEXT("@cbDistributeRule"),pCreateMatchInfo->cbDistributeRule);
		GameMatchDBAide.AddParameter(TEXT("@wMinDistributeUser"),pCreateMatchInfo->wMinDistributeUser);
		GameMatchDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pCreateMatchInfo->wDistributeTimeSpace);
		GameMatchDBAide.AddParameter(TEXT("@wMinPartakeGameUser"),pCreateMatchInfo->wMinPartakeGameUser);
		GameMatchDBAide.AddParameter(TEXT("@wMaxPartakeGameUser"),pCreateMatchInfo->wMaxPartakeGameUser);

		//��������
		GameMatchDBAide.AddParameter(TEXT("@strRewardGold"),pCreateMatchInfo->szRewardGold);
		GameMatchDBAide.AddParameter(TEXT("@strRewardIngot"),pCreateMatchInfo->szRewardIngot);	
		GameMatchDBAide.AddParameter(TEXT("@strRewardExperience"),pCreateMatchInfo->szRewardExperience);

		//��������
		GameMatchDBAide.AddParameter(TEXT("@strMatchRule"),szMatchRule);		

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_ModifyGameMatchItem"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadGameMatchOption(GameMatchDBAide,GameMatchResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}



//��������
bool CMatchInfoManager::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
{
	//��ȡ����
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("CreateGameServer ���� PlatformDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//��������
	PlatformDBModule->SetConnectionInfo(pDataBaseParameter->szDataBaseAddr,pDataBaseParameter->wDataBasePort,
		pDataBaseParameter->szDataBaseName,pDataBaseParameter->szDataBaseUser,pDataBaseParameter->szDataBasePass);

	//��������
	PlatformDBModule->OpenConnection();

	return true;
}

//��������
bool CMatchInfoManager::ConnectGameMatchDB(CDataBaseHelper & GameMatchDBModule)
{
	//��ȡ����
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetGameMatchDBParameter();

	//��������
	if ((GameMatchDBModule.GetInterface()==NULL)&&(GameMatchDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("ConnectGameMatchDB ���� GameMatchDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//��������
	GameMatchDBModule->SetConnectionInfo(pDataBaseParameter->szDataBaseAddr,pDataBaseParameter->wDataBasePort,
		pDataBaseParameter->szDataBaseName,pDataBaseParameter->szDataBaseUser,pDataBaseParameter->szDataBasePass);

	//��������
	GameMatchDBModule->OpenConnection();

	return true;
}

//��ȡ����
bool CMatchInfoManager::ReadGameMatchOption(CDataBaseAide & GameMatchDBAide, tagGameMatchOption & GameMatchResult)
{
	//���ñ���
	ZeroMemory(&GameMatchResult,sizeof(GameMatchResult));

	//��������	
	GameMatchResult.dwMatchID=GameMatchDBAide.GetValue_DWORD(TEXT("MatchID"));	
	GameMatchResult.cbMatchStatus=GameMatchDBAide.GetValue_BYTE(TEXT("MatchStatus"));

	//������Ϣ
	GameMatchResult.cbMatchType = GameMatchDBAide.GetValue_BYTE(TEXT("MatchType"));		
	GameMatchDBAide.GetValue_String(TEXT("MatchName"),GameMatchResult.szMatchName,CountArray(GameMatchResult.szMatchName));

	//������Ϣ
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

	//��������
	GameMatchResult.cbRankingMode = GameMatchDBAide.GetValue_BYTE(TEXT("RankingMode"));
	GameMatchResult.wCountInnings = GameMatchDBAide.GetValue_WORD(TEXT("CountInnings"));
	GameMatchResult.cbFilterGradesMode = GameMatchDBAide.GetValue_BYTE(TEXT("FilterGradesMode"));

	//��������
	GameMatchResult.cbDistributeRule=GameMatchDBAide.GetValue_BYTE(TEXT("DistributeRule"));
	GameMatchResult.wMinDistributeUser=GameMatchDBAide.GetValue_WORD(TEXT("MinDistributeUser"));
	GameMatchResult.wDistributeTimeSpace=GameMatchDBAide.GetValue_WORD(TEXT("DistributeTimeSpace"));
	GameMatchResult.wMinPartakeGameUser=GameMatchDBAide.GetValue_WORD(TEXT("MinPartakeGameUser"));
	GameMatchResult.wMaxPartakeGameUser=GameMatchDBAide.GetValue_WORD(TEXT("MaxPartakeGameUser"));

	//��������
	TCHAR szMatchRule[CountArray(GameMatchResult.cbMatchRule)*2+1];
	GameMatchDBAide.GetValue_String(TEXT("MatchRule"),szMatchRule,CountArray(szMatchRule));

	//��չ����
	if (szMatchRule[0]!=0)
	{
		//��ȡ����
		ASSERT(lstrlen(szMatchRule)%2==0);
		INT nMatchRuleSize=lstrlen(szMatchRule)/2;

		//ת���ַ�
		for (INT i=0;i<nMatchRuleSize;i++)
		{
			//��ȡ�ַ�
			TCHAR cbChar1=szMatchRule[i*2];
			TCHAR cbChar2=szMatchRule[i*2+1];

			//Ч���ַ�
			ASSERT((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))||(cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F')));
			ASSERT((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))||(cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F')));

			//���ɽ��
			if ((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))) GameMatchResult.cbMatchRule[i]+=(cbChar2-TEXT('0'));
			if ((cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F'))) GameMatchResult.cbMatchRule[i]+=(cbChar2-TEXT('A')+0x0A);
			if ((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))) GameMatchResult.cbMatchRule[i]+=(cbChar1-TEXT('0'))*0x10;
			if ((cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F'))) GameMatchResult.cbMatchRule[i]+=(cbChar1-TEXT('A')+0x0A)*0x10;
		}
	}

	//���ؽ���
	CRewardInfoBuffer RewardInfoBuffer;
	LoadGameMatchReward(GameMatchResult.dwMatchID,RewardInfoBuffer);	

	//��������
	GameMatchResult.wRewardCount=(WORD)__min(RewardInfoBuffer.GetMatchRewardCount(),CountArray(GameMatchResult.MatchRewardInfo));

	//��������
	tagMatchRewardInfo * pMatchRewardInfo=NULL;
	for (WORD i=0;i<GameMatchResult.wRewardCount;i++)
	{
		//��ѯ����
		pMatchRewardInfo=RewardInfoBuffer.SearchMatchReward(i+1);
		if (pMatchRewardInfo==NULL) break;

		//��������
		CopyMemory(&GameMatchResult.MatchRewardInfo[i],pMatchRewardInfo,sizeof(GameMatchResult.MatchRewardInfo[0]));
	}	

	return true;
}

//////////////////////////////////////////////////////////////////////////
