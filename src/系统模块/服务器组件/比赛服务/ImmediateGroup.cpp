#include "StdAfx.h"
#include "ImmediateGroup.h"

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//��������
#define INVALID_VALUE				0xFFFF								//��Чֵ

//��̬����
WORD CImmediateGroup::m_wChairCount;									//������Ŀ

//////////////////////////////////////////////////////////////////////////
//���캯��
CImmediateGroup::CImmediateGroup(LONGLONG lMatchNo,tagGameMatchOption * pGameMatchOption,IImmediateGroupSink *pIImmediateGroupSink)
{
	//ָ�����
	m_pMatchOption=pGameMatchOption;
	m_pMatchSink=pIImmediateGroupSink;
	m_pImmediateMatch=(tagImmediateMatch *)pGameMatchOption->cbMatchRule;

	//״̬����
	m_lMatchNo=lMatchNo;
	m_cbMatchStatus=MatchStatus_Signup;
	m_lCurBase=m_pImmediateMatch->lInitalBase;

	//���ñ���
	ZeroMemory(&m_MatchStartTime,sizeof(m_MatchStartTime));

	//�Ƴ�Ԫ��
	m_OnMatchUserMap.RemoveAll();
	m_FreeUserArray.RemoveAll();
	m_MatchTableArray.RemoveAll();
    
	//���ñ���
	m_LoopTimer=0;
	m_wAndroidUserCount=0;	
}

//��������
CImmediateGroup::~CImmediateGroup(void)
{
	//ɱ����ʱ��
	AllKillMatchTimer();

	//����״̬
	m_wAndroidUserCount=0;
	m_cbMatchStatus=MatchStatus_Null;

	//�ͷ�����
	for (int i=0; i<m_MatchTableArray.GetCount(); i++)
	{
		SafeDelete(m_MatchTableArray[i]);
	}

	//�Ƴ�Ԫ��
	m_MatchTableArray.RemoveAll();
	m_OnMatchUserMap.RemoveAll();
	m_FreeUserArray.RemoveAll();
}

//�ӿڲ�ѯ
VOID * CImmediateGroup::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMatchEventSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IMatchEventSink,Guid,dwQueryVer);
	return NULL;
}

//�û�����
bool CImmediateGroup::OnUserSignUpMatch(IServerUserItem * pUserItem)
{
	//�û��ж�
	ASSERT(pUserItem!=NULL);
	if (pUserItem==NULL) return false;

	//�����û�
	IServerUserItem * pIServerUserExist=NULL;
	m_OnMatchUserMap.Lookup(pUserItem->GetUserID(),pIServerUserExist);

	//�û��ж�
	ASSERT(pIServerUserExist==NULL);
	if (pIServerUserExist!=NULL) return false;

	//����Ч��
	ASSERT(m_OnMatchUserMap.GetCount()<m_pImmediateMatch->wStartUserCount);
	if (m_OnMatchUserMap.GetCount()>=m_pImmediateMatch->wStartUserCount) return false;

	//�����ж�
	if (pUserItem->IsAndroidUser()==true)
	{
		if (m_wAndroidUserCount>=m_pImmediateMatch->wAndroidUserCount)
		{
			return false;
		}
		m_wAndroidUserCount++;
	}

	//���óɼ�
	tagUserInfo *pUserScore=pUserItem->GetUserInfo();
	pUserScore->dwWinCount=0L;
	pUserScore->dwLostCount=0L;
	pUserScore->dwFleeCount=0L;
	pUserScore->dwDrawCount=0L;
	pUserScore->lIntegralCount=0L;
	pUserScore->lScore=(LONGLONG)m_pImmediateMatch->lInitalScore;

	//���ͳɼ�
	m_pMatchSink->SendGroupUserScore(pUserItem,NULL);

	//�����û�
	m_OnMatchUserMap[pUserItem->GetUserID()]=pUserItem;

	//���ñ���״̬
	pUserItem->SetUserMatchStatus(MUS_SIGNUPED);

	//���ñ���ʱ��
	pUserItem->SetSignUpTime((DWORD)time(NULL));

	//���ͱ���״̬
	BYTE cbUserMatchStatus=pUserItem->GetUserMatchStatus();
	m_pMatchSink->SendData(pUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

	//��������
	if (m_OnMatchUserMap.GetCount()>=m_pImmediateMatch->wStartUserCount)
	{
		//�¼�֪ͨ
		ASSERT(m_pMatchSink!=NULL);
		if (m_pMatchSink!=NULL) m_pMatchSink->SendMatchInfo(NULL);
		EfficacyStartMatch();
	}

	return true;
}
//������ʼ
VOID CImmediateGroup::EfficacyStartMatch()
{
	//��������
	m_pMatchSink->InsertNullTable();

	//û�����ӵ����
	if( m_MatchTableArray.GetCount()<m_pImmediateMatch->wStartUserCount/m_wChairCount )
	{
		if(m_OnMatchUserMap.GetCount()<m_wChairCount)
		{
			//�������ݰ�
			CMD_CM_SystemMessage Message;
			Message.wType=SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME;
			lstrcpyn(Message.szString,TEXT("��Ǹ������ĳ�û�ǿ���˳��������ó�������ǰ����������Լ����ȴ�������"),CountArray(Message.szString));

			Message.wLength=CountStringBuffer(Message.szString);
			//��������
			WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
			m_pMatchSink->SendGroupData(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize, this);
			m_cbMatchStatus=MatchStatus_Signup;
			return ;
		}

		m_cbMatchStatus=MatchStatus_Wait;
		//3���������øú���ֱ��������
		SetMatchGameTimer(IDI_WAIT_TABLE,3000,1,NULL);

		//�������ݰ�
		CMD_CM_SystemMessage Message;
		Message.wType=SMT_CHAT;
		_sntprintf(Message.szString,CountArray(Message.szString),
			TEXT("��Ǹ�����ڹ����û������������������� %d �����ӿ���,��ȴ�������"),m_pImmediateMatch->wStartUserCount/m_wChairCount-m_MatchTableArray.GetCount());
		Message.wLength=CountStringBuffer(Message.szString);

		//��������
		WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
		m_pMatchSink->SendGroupData(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize, this);
		return ;
	}

	//Ԥ��������λ
	DWORD dwUserID=0L;
	IServerUserItem *pIServerUserItem=NULL;
	POSITION pos=m_OnMatchUserMap.GetStartPosition();

	WORD wChairID=0;
	while (pos!=NULL)
	{
		//��ȡ�û�
		m_OnMatchUserMap.GetNextAssoc(pos,dwUserID,pIServerUserItem);
		ASSERT(pIServerUserItem!=NULL);

		InsertFreeGroup(pIServerUserItem);

		//�����û�������
		pIServerUserItem->SetUserMatchStatus(MUS_PLAYING);
		pIServerUserItem->SetMatchData(this);

		//����״̬
		BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
		m_pMatchSink->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));
	}

	//�����Զ�����
	PerformDistributeTable();

	//����״̬	
	m_cbMatchStatus=MatchStatus_Playing;

	//��¼ʱ��
	CTime::GetCurrentTime().GetAsSystemTime(m_MatchStartTime);
	
	//��ʼ֪ͨ
	if (m_pMatchSink!=NULL) m_pMatchSink->OnEventMatchStart(this);
		
	//���ö�ʱ��
	SetMatchGameTimer(IDI_CHECK_MATCH,1000,TIMES_INFINITY,0);
}

//�û�����
bool CImmediateGroup::OnUserQuitMatch(IServerUserItem *pUserItem)
{
	//�û��ж�
	ASSERT(pUserItem!=NULL);
	if (pUserItem==NULL) return false;

	//ɾ���û�
	bool bSuccess=(m_OnMatchUserMap.RemoveKey(pUserItem->GetUserID()) == TRUE);
	if(bSuccess==false)return false;

	if (pUserItem->IsAndroidUser())
	{
		ASSERT(m_wAndroidUserCount>0);
		if (m_wAndroidUserCount>0) m_wAndroidUserCount--;
	}

	//����״̬
	pUserItem->SetUserMatchStatus(MUS_NULL);
	BYTE cbUserMatchStatus=pUserItem->GetUserMatchStatus();
	m_pMatchSink->SendData(pUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

	return bSuccess;
}

//��ʱ��
bool CImmediateGroup::OnTimeMessage(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//״̬У��
	if(m_cbMatchStatus==MatchStatus_Null) return true;

	switch(dwTimerID)
	{
	case IDI_CHECK_MATCH:
		{
			CaptureMatchTimer();

			return true;
		}
	case IDI_WAIT_TABLE:
		{
			if(m_cbMatchStatus==MatchStatus_Wait)
			{
				EfficacyStartMatch();
			}

			return true;
		}
	case IDI_SWITCH_TABLE:
		{	
			//�رն�ʱ��
			KillMatchGameTimer(IDI_SWITCH_TABLE);

			//ȫ������
			PerformAllUserStandUp();

			//��������
			PerformDistributeTable();

			return true;
		}
	case IDI_MATCH_FINISH:
		{
			//������
			MatchOver();

			return true;
		}
	}

	return true;
}

//�û�����
WORD CImmediateGroup::SortMapUser(tagMatchScore score[])
{
	INT_PTR nCount=m_OnMatchUserMap.GetCount();
	POSITION pos=m_OnMatchUserMap.GetStartPosition();
	nCount=0;
	while(pos!=NULL)
	{
		m_OnMatchUserMap.GetNextAssoc(pos,score[nCount].dwUserID, score[nCount].pUserItem);
		score[nCount].dwUserID=score[nCount].pUserItem->GetUserID();
		score[nCount].lScore=score[nCount].pUserItem->GetUserScore();
		nCount++;
	}
	for(INT_PTR i = 1;i < nCount;i++)
	{
		INT_PTR left = 0,right = i - 1;
		tagMatchScore  Temp = score[i];
		while(left <= right)
		{
			INT_PTR mid = (left + right)/2;
			if(score[i].lScore > score[mid].lScore) right = mid - 1;
			else left = mid + 1;
		}
		for(INT_PTR j = i - 1;j >= left;j--)
		{
			score[j+1] = score[j];
		}
		score[left] = Temp;
	}

	return (WORD)nCount;
}

//��Ϸ��ʼ
bool CImmediateGroup::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	//���õ׷�
	pITableFrame->SetCellScore((SCORE)m_lCurBase);
	KillMatchTimer(IDI_CHECK_TABLE_START,(WPARAM)pITableFrame);

	//��ȡ�ӿ�
	tagTableFrameInfo *pTableFrameInfo=GetTableInterface(pITableFrame);

	//����״̬
	pTableFrameInfo->bMatchTaskFinish=false;
	pTableFrameInfo->bSwtichTaskFinish=false;
	pTableFrameInfo->bWaitForNextInnings=false;

	//����ɾ�����1
	pTableFrameInfo->wPlayCount++;

	//���ͱ�����Ϣ
	SendTableMatchInfo(pITableFrame, INVALID_CHAIR);

	//�������ݰ�
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	TCHAR szMsg[256];
	_sntprintf(szMsg, CountArray(szMsg),TEXT("������������%d�ֿ�ʼ����%d�֣���"),pTableFrameInfo->wPlayCount,m_pImmediateMatch->wPlayCount);
	lstrcpyn(SystemMessage.szString,szMsg,CountArray(SystemMessage.szString));
	SystemMessage.wLength=CountStringBuffer(SystemMessage.szString);
	SystemMessage.wType=SMT_CHAT;

	//��������
	WORD wSendSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString)+SystemMessage.wLength*sizeof(TCHAR);
	pITableFrame->SendTableData(INVALID_CHAIR,SUB_GF_SYSTEM_MESSAGE, (VOID*)&SystemMessage,wSendSize,MDM_GF_FRAME);

	return true;
}

//��Ϸ����
bool CImmediateGroup::OnEventGameEnd(ITableFrame * pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//��ȡ������Ϣ
	tagTableFrameInfo * pTableFrameInfo=GetTableInterface(pITableFrame);
	if (pTableFrameInfo==NULL) return false;

	//���ñ�ʶ
	pTableFrameInfo->bWaitForNextInnings=true;

	//��������
	TCHAR szMessage[128]=TEXT("");

	//��ɾ���
	if (pTableFrameInfo->wPlayCount>=m_pImmediateMatch->wPlayCount)
	{
		//�ȴ�����
		SendWaitMessage(pITableFrame);	

		//���ñ���
		pTableFrameInfo->wPlayCount=0;
		pTableFrameInfo->bMatchTaskFinish=true;	

		//�������
		if (CheckMatchTaskFinish()==true)
		{
			SetMatchGameTimer(IDI_MATCH_FINISH,5000,1,0);		
		}

		//������Ϣ		
		_sntprintf(szMessage,CountArray(szMessage),TEXT("��ϲ��,��������%d����Ϸ,����������ɺ�ϵͳ����������,�������ĵȴ���"),m_pImmediateMatch->wPlayCount);
	}
	else
	{
		//�����ж�
		if (m_pImmediateMatch->cbSwitchTableCount>0&&(pTableFrameInfo->wPlayCount%m_pImmediateMatch->cbSwitchTableCount)==0)
		{
			//����״̬
			pTableFrameInfo->bSwtichTaskFinish=true;

			//��������б�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem!=NULL) InsertFreeGroup(pIServerUserItem);
			}

			//��黻��
			if (CheckSwitchTaskFinish()==true)
			{
				SetMatchGameTimer(IDI_SWITCH_TABLE,5000,1,0);
			}

			//�ȴ�����
			SendWaitMessage(pITableFrame);

			//������Ϣ
			lstrcpyn(szMessage,TEXT("�����ѽ����������ĵȴ���������ɣ��Ժ�Ϊ������..."),CountArray(szMessage));
		}
		else
		{
			//���ö�ʱ��
			PostMatchTimer(IDI_CONTINUE_GAME,5,(WPARAM)pITableFrame);

			//������Ϣ
			lstrcpyn(szMessage,TEXT("�����ѽ��������Ժ�ϵͳ������..."),CountArray(szMessage));
		}			
	}

	//������Ϣ
	pITableFrame->SendGameMessage(szMessage,SMT_CHAT|SMT_TABLE_ROLL);

	//����֪ͨ
	if (m_OnMatchUserMap.GetCount()>0)
	{
		//����֪ͨ
		tagMatchScore *pScore = new tagMatchScore[m_OnMatchUserMap.GetCount()];
		ZeroMemory(pScore,sizeof(tagMatchScore)*m_OnMatchUserMap.GetCount());
		WORD wCount=SortMapUser(pScore);
		for (WORD i=0;i<wCount;i++)
		{
			m_pMatchSink->SendGroupUserScore(pScore[i].pUserItem,this);
		}

		SafeDeleteArray(pScore);
	}

	return true;
}

//��ҷ���
bool CImmediateGroup::OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem)
{
	//���ͱ�����Ϣ
	SendTableMatchInfo(pITableFrame,pIServerUserItem->GetChairID());

	return true;
}

//�û�����
bool CImmediateGroup::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	return true; 
}

//�û�����
bool CImmediateGroup::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

 //�û�ͬ��
bool CImmediateGroup::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)
{ 
	return false; 
}

//��ȡ����
WORD CImmediateGroup::GetUserRank(IServerUserItem * pUserServerItem, ITableFrame *pITableFrame)
{
	//����Ч��
	ASSERT(pUserServerItem!=NULL);
	if(pUserServerItem==NULL) return INVALID_WORD;

	//�������
	LONGLONG lUserScore=pUserServerItem->GetUserScore();
	WORD wRank=1;
	DWORD dwUserID=0;
	IServerUserItem *pLoopUserItem=NULL;

	if(pITableFrame==NULL)
	{
		//��������
		POSITION pos=m_OnMatchUserMap.GetStartPosition();
		while(pos!=NULL)
		{
			m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pLoopUserItem);
			if(pLoopUserItem!=pUserServerItem && pLoopUserItem->GetUserScore()>lUserScore)
				wRank++;
		}
	}
	else
	{
		ASSERT(pUserServerItem->GetTableID()==pITableFrame->GetTableID());
		if(pUserServerItem->GetTableID()!=pITableFrame->GetTableID()) return INVALID_WORD;

		for(WORD i=0; i<m_wChairCount; i++)
		{
			pLoopUserItem=pITableFrame->GetTableUserItem(i);
			if(pLoopUserItem==NULL) continue;

			if(pLoopUserItem!=pUserServerItem && pLoopUserItem->GetUserScore()>lUserScore)
				wRank++;
		}
	}

	return wRank;
}

//��ȡ�ӿ�
tagTableFrameInfo* CImmediateGroup::GetTableInterface(ITableFrame* pITableFrame)
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if (m_MatchTableArray[i]->pTableFrame==pITableFrame) return m_MatchTableArray[i];
	}

	return NULL;
}

//��������
VOID CImmediateGroup::MatchOver()
{
	//ɾ����ʱ��
	AllKillMatchTimer();
	KillMatchGameTimer(IDI_CHECK_MATCH);
	KillMatchGameTimer(IDI_WAIT_TABLE);
	KillMatchGameTimer(IDI_SWITCH_TABLE);
	KillMatchGameTimer(IDI_MATCH_FINISH);

	//���ñ���״̬
	if (m_cbMatchStatus==MatchStatus_Playing)
	{
		m_cbMatchStatus=MatchStatus_Null;
		m_pMatchSink->OnEventMatchOver(this);
	}
}

//������ʾ
VOID CImmediateGroup::SendWaitMessage(ITableFrame *pTableFrame, WORD wChairID/*=INVALID_CHAIR*/)
{
	//״̬У��
	if (m_cbMatchStatus==MatchStatus_Null || m_cbMatchStatus==MatchStatus_Signup)
	{
		return;
	}

	//��ȡ��Ϣ
	tagTableFrameInfo * pITableFameInfo=GetTableInterface(pTableFrame);
	if (pITableFameInfo==NULL) return;

	//��������
	WORD wPlayingTable=0;
	for(WORD i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(m_MatchTableArray[i]->pTableFrame->GetGameStatus()>=GAME_STATUS_PLAY)
		{
			wPlayingTable++;
		}
	}

	//��������
	CMD_GR_Match_Wait_Tip WaitTip;
	ZeroMemory(&WaitTip,sizeof(WaitTip));
	WaitTip.wPlayingTable=wPlayingTable;
	WaitTip.wUserCount=(WORD)m_OnMatchUserMap.GetCount();
	WaitTip.wCurGameCount=pITableFameInfo->wPlayCount;
	WaitTip.wGameCount=m_pImmediateMatch->wPlayCount;

	lstrcpyn(WaitTip.szMatchName, m_pMatchOption->szMatchName,LEN_SERVER);

	//���ͱ�����Ϣ�����������
	for(WORD i=0;i<m_wChairCount;i++)
	{
		if(wChairID!=INVALID_CHAIR && wChairID!=i) continue;
		IServerUserItem *pTableUserItem=pTableFrame->GetTableUserItem(i);
		if(pTableUserItem==NULL) continue;
		WaitTip.wCurTableRank=GetUserRank(pTableUserItem,pTableFrame);
		if(WaitTip.wCurTableRank==INVALID_WORD) continue;


		WaitTip.wRank=GetUserRank(pTableUserItem);
		WaitTip.lScore=pTableUserItem->GetUserScore();

		m_pMatchSink->SendData(pTableUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,&WaitTip,sizeof(WaitTip));
	}

	//���ͱ�����Ϣ�������������
	for(WORD i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(pTableFrame->GetTableID()==m_MatchTableArray[i]->pTableFrame->GetTableID())continue;
		if(m_MatchTableArray[i]->pTableFrame->GetGameStatus()>=GAME_STATUS_PLAY) continue; 
		for(WORD j=0;j<m_wChairCount;j++)
		{
			IServerUserItem *pTableUserItem=m_MatchTableArray[i]->pTableFrame->GetTableUserItem(j);
			if(pTableUserItem==NULL) continue;
			WaitTip.wCurTableRank=GetUserRank(pTableUserItem,m_MatchTableArray[i]->pTableFrame);
			if(WaitTip.wCurTableRank==INVALID_WORD) continue;

			WaitTip.wRank=GetUserRank(pTableUserItem);
			WaitTip.lScore=pTableUserItem->GetUserScore();

			m_pMatchSink->SendData(pTableUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,&WaitTip,sizeof(WaitTip));
		}
	}
}

//��������
VOID CImmediateGroup::PerformDistributeTable()
{
	//�������
	if (CheckMatchUser()) return;

	//��������
	CTableFrameMananerArray	FreeTableArray;	

	//��ȡ����
	for (INT_PTR i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if (m_MatchTableArray[i]->pTableFrame->IsGameStarted()==false&&m_MatchTableArray[i]->bMatchTaskFinish==false)
		{
			//��ȡ����
			INT_PTR cbTimer=m_MatchTimerArray.GetCount();
			bool cbIsInsert=true;
			for (int j=0;j<cbTimer;j++)
			{
				//�ж�һ����û�п����������ڵȴ�
				if ((m_MatchTimerArray[j]->dwTimerID==IDI_CHECK_TABLE_START||m_MatchTimerArray[j]->dwTimerID==IDI_CONTINUE_GAME)&&(ITableFrame*)(m_MatchTimerArray[j]->wParam)==m_MatchTableArray[i]->pTableFrame)
				{
					cbIsInsert=false;
					break;
				}
			}

			//�����ж�
			if (cbIsInsert==true)
			{
				FreeTableArray.Add(m_MatchTableArray[i]);
			}
		}
	}

	for (INT_PTR i=0;i<FreeTableArray.GetCount();i++)
	{
		DWORD nCurUserCount=(DWORD)m_FreeUserArray.GetCount();		
		if(nCurUserCount/m_wChairCount>0)
		{
			//ȫ������
			PerformAllUserStandUp(FreeTableArray[i]->pTableFrame);

			//��������
			for (WORD wChairID=0; wChairID<m_wChairCount; wChairID++)
			{
				//�û��������
				WORD wRandID=rand()%(WORD)m_FreeUserArray.GetCount();

				//��ȡ�û�
				IServerUserItem *pIServerUserItem=m_FreeUserArray[wRandID];				
				if (pIServerUserItem==NULL) continue;
				
				//������������ �ø�������ȫ������
				if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					for(int m=0;m<m_MatchTableArray.GetCount();m++)
					{
						if(pIServerUserItem->GetTableID()==m_MatchTableArray[m]->wTableID)
						{
							m_MatchTableArray[m]->pTableFrame->PerformStandUpAction(pIServerUserItem);
							break;
						}
					}
				}

				if (FreeTableArray[i]->pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem))
				{
					m_FreeUserArray.RemoveAt(wRandID);
				}
				else
				{
					CTraceService::TraceString(TEXT("����ʧ�ܣ�"),TraceLevel_Exception);
				}
			}

			//10�����һ����� ���ָ�����û�п�ʼ��Ϸ ˵���Ѿ�������
			PostMatchTimer(IDI_CHECK_TABLE_START,10,(WPARAM)FreeTableArray[i]->pTableFrame);
		}
	}

	return;
}

//����һ����Ϣ
VOID CImmediateGroup::SendGroupMessage(LPCTSTR pStrMessage)
{
	//�������ݰ�
	CMD_CM_SystemMessage Message;
	Message.wType=SMT_EJECT|SMT_CHAT;
	lstrcpyn(Message.szString,pStrMessage,CountArray(Message.szString));
	Message.wLength=CountStringBuffer(Message.szString);

    //��������
	WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
	m_pMatchSink->SendGroupData(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize, this);
}

//������
bool CImmediateGroup::CheckMatchTaskFinish()
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if (m_MatchTableArray[i]->bMatchTaskFinish==false) return false;
	}

	return true;
}

//��黻��
bool CImmediateGroup::CheckSwitchTaskFinish()
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(m_MatchTableArray[i]->bSwtichTaskFinish==false) return false;
	}

	return true;
}


//��������
VOID CImmediateGroup::PerformAllUserStandUp()
{	
	//��������
	for (WORD i=0; i<m_MatchTableArray.GetCount();i++)
	{
		//������Ϣ
		tagTableFrameInfo * pTableFrameInfo=m_MatchTableArray[i];
		if (pTableFrameInfo==NULL) continue;

		//����ָ��
		ITableFrame* pTableFrame=pTableFrameInfo->pTableFrame;		
		if (pTableFrame==NULL) continue;

		//ǿ�ƽ�ɢ��Ϸ
		if (pTableFrame->IsGameStarted()==true)
		{
			pTableFrame->DismissGame();			
		}

		//��������
		PerformAllUserStandUp(pTableFrame);
	}
	return;
}

//��������
VOID CImmediateGroup::PerformAllUserStandUp(ITableFrame *pITableFrame)
{
	//����У��
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	//��������
	for (int i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pUserServerItem=pITableFrame->GetTableUserItem(i);
		if(pUserServerItem&&pUserServerItem->GetTableID()!=INVALID_TABLE)
		{
			pITableFrame->PerformStandUpAction(pUserServerItem);
		}
	}	
}
//����������Ϸ
VOID CImmediateGroup::ContinueGame(ITableFrame * pITableFrame)
{
	//����У��
	ASSERT (pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	//��ȡ�ӿ�
	tagTableFrameInfo * pTableFrameInfo=GetTableInterface(pITableFrame);
	if (pTableFrameInfo==NULL) return;

	//�����û�
	for (int i=0;i<m_wChairCount;i++)
	{
		//�����û�
		IServerUserItem * pIServerUserItem=pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//��������
		if (pITableFrame->PerformStandUpAction(pIServerUserItem)==false)
		{
			ASSERT (FALSE);
			CTraceService::TraceString(TEXT("������Ϸ_����ʧ��"),TraceLevel_Exception);

			continue;
		}

		//��������
		if (pITableFrame->PerformSitDownAction(i,pIServerUserItem)==false)
		{			
			ASSERT (FALSE);
			CTraceService::TraceString(TEXT("������Ϸ_����ʧ��"),TraceLevel_Exception);

			continue;
		}		
	}

	//���ö�ʱ��
	PostMatchTimer(IDI_CHECK_TABLE_START,30,(WPARAM)pITableFrame);
}

VOID CImmediateGroup::SetMatchGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	if (m_pMatchSink!=NULL)
	{
		m_pMatchSink->SetGameTimer(dwTimerID+10*m_LoopTimer,dwElapse,dwRepeat,dwBindParameter,this);
	}
}

//ɱ������ר�ö�ʱ��
VOID CImmediateGroup::KillMatchGameTimer(DWORD dwTimerID)
{
	if (m_pMatchSink!=NULL)
	{
		m_pMatchSink->KillGameTimer(dwTimerID+10*m_LoopTimer,this);
	}
}

//����������
bool CImmediateGroup::CheckMatchUser()
{
	//�����û����� ����������Ҫ��ֹ����
	DWORD nCurUserCount=(DWORD)m_OnMatchUserMap.GetCount();
	if (nCurUserCount<m_wChairCount)
	{
		ASSERT(FALSE);
		CTraceService::TraceString(TEXT("�������..."),TraceLevel_Exception);
		return false;

		//�������ݰ�
		CString szString;
		szString.Format(TEXT("��Ǹ�����ڱ����û�ǿ���˳��������������� %d �ˣ���ǰ���㡣��ӭ�����������μӱ�����"),m_wChairCount);
		SendGroupMessage(szString);

		MatchOver();

		return true;
	}
	return false;
}

//�������
VOID CImmediateGroup::CheckTableStart(ITableFrame *pITableFrame)
{
	//У�����
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	if(pITableFrame&&pITableFrame->IsGameStarted()==false)
	{
		for (int i=0;i<m_wChairCount;i++)
		{
			IServerUserItem * pIServerUserItem=pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem && pIServerUserItem->GetTableID()!=INVALID_TABLE)
			{
				if (pIServerUserItem->IsClientReady()==false || pIServerUserItem->IsTrusteeUser()==true)
				{
					pITableFrame->ImitateGameOption(pIServerUserItem);
				}
			}
		}
	}
}

//�������
VOID CImmediateGroup::InsertFreeGroup(IServerUserItem * pUserServerItem)
{
	//����У��
	ASSERT (pUserServerItem!=NULL);
	if (pUserServerItem==NULL) return;
	
	for(int i=0;i<m_FreeUserArray.GetCount();i++)
	{
		if( m_FreeUserArray[i] != NULL && m_FreeUserArray[i]->GetUserID()==pUserServerItem->GetUserID()) return;
	}

	m_FreeUserArray.Add(pUserServerItem);
}

//���ͱ�����Ϣ
VOID CImmediateGroup::SendTableMatchInfo(ITableFrame *pITableFrame, WORD wChairID)
{
	//��֤����
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	tagTableFrameInfo *pTableFrameInfo=GetTableInterface(pITableFrame);
	if (pTableFrameInfo==NULL) return;

	//��������
	WORD wChairCount=pITableFrame->GetChairCount();
	TCHAR szMsg[256]=TEXT("");

	if(m_cbMatchStatus==MatchStatus_Playing)
	{
		//��������
		CMD_GR_Match_Info MatchInfo;
		ZeroMemory(&MatchInfo,sizeof(MatchInfo));
		_sntprintf(MatchInfo.szTitle[0], CountArray(MatchInfo.szTitle[0]),TEXT("���ַ�ʽ���ۼƻ���"));
		_sntprintf(MatchInfo.szTitle[1], CountArray(MatchInfo.szTitle[1]),TEXT("��Ϸ������%d"), m_lCurBase);
		_sntprintf(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]),TEXT("�������ƣ���������"));
		_sntprintf(MatchInfo.szTitle[3], CountArray(MatchInfo.szTitle[3]),TEXT("��Ҫ��ɣ�%d�ֱ���"), m_pImmediateMatch->wPlayCount);
		MatchInfo.wGameCount=pTableFrameInfo->wPlayCount;
		
		//wChairIDΪINVALID_CHAIR��������������ң�����ֻ����ָ�����
		for(WORD i=0; i<wChairCount; i++)
		{
			//��ȡ�û�
			if(wChairID!=INVALID_CHAIR && wChairID!=i) continue;
			IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(i);
			if(pIServerUserItem==NULL) continue;

			//��������
			pITableFrame->SendTableData(i, SUB_GR_MATCH_INFO, &MatchInfo, sizeof(MatchInfo), MDM_GF_FRAME);
		}

		//������ʾ
		if ((wChairID!=INVALID_CHAIR) && (pTableFrameInfo->bWaitForNextInnings==true))
		{
			SendWaitMessage(pITableFrame,wChairID);
		}
	}

	return;
}

//�������ר�ö�ʱ��
VOID CImmediateGroup::CaptureMatchTimer()
{
	for(int i=0;i<m_MatchTimerArray.GetCount();i++)
	{
		if(--m_MatchTimerArray[i]->iElapse<=0)
		{
			if (m_MatchTimerArray[i]->dwTimerID==IDI_CONTINUE_GAME)
			{
				//������Ϸ
				ContinueGame((ITableFrame*)m_MatchTimerArray[i]->wParam);
			}
			else if(m_MatchTimerArray[i]->dwTimerID==IDI_LEAVE_TABLE)
			{
				//�뿪����
				PerformAllUserStandUp((ITableFrame*)m_MatchTimerArray[i]->wParam);

			}else if(m_MatchTimerArray[i]->dwTimerID==IDI_CHECK_TABLE_START)
			{
				//�������
				CheckTableStart((ITableFrame*)m_MatchTimerArray[i]->wParam);
			}
			//ɾ����ʱ��
			if (KillMatchTimer(i)) i--;
			else ASSERT(false);
		}
	}
}
//Ͷ�ݱ���ר�ö�ʱ��
VOID CImmediateGroup::PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam, LPARAM lParam)
{
	tagMatchTimer* pMatchTimer=new tagMatchTimer;
	pMatchTimer->dwTimerID=dwTimerID;
	pMatchTimer->iElapse=iElapse;
	//pMatchTimer->dwRepeat=dwRepeat;
	pMatchTimer->wParam=wParam;
	pMatchTimer->lParam=lParam;
	m_MatchTimerArray.Add(pMatchTimer);
}

//ɱ������ר�ö�ʱ��
bool CImmediateGroup::KillMatchTimer(DWORD dwTimerID, WPARAM wParam)
{
	for(int i=0;i<m_MatchTimerArray.GetCount();i++)
	{
		if(m_MatchTimerArray[i]->dwTimerID==dwTimerID&&m_MatchTimerArray[i]->wParam==wParam)
		{
			tagMatchTimer* pMatchTimer=m_MatchTimerArray[i];
			m_MatchTimerArray.RemoveAt(i);
			SafeDelete(pMatchTimer);
			return true;
		}
	}
	return false;
}
//ɱ������ר�ö�ʱ��
bool CImmediateGroup::KillMatchTimer(INT_PTR dwIndexID)
{
	ASSERT(dwIndexID >= 0 && dwIndexID < m_MatchTimerArray.GetCount());
	if( dwIndexID >= 0 && dwIndexID < m_MatchTimerArray.GetCount() )
	{
		tagMatchTimer* pMatchTimer=m_MatchTimerArray[dwIndexID];
		m_MatchTimerArray.RemoveAt(dwIndexID);
		SafeDelete(pMatchTimer);
		return true;
	}
	return false;
}
//ȫ��ɱ������ר�ö�ʱ��
VOID CImmediateGroup::AllKillMatchTimer()
{
	for(int i=0;i<m_MatchTimerArray.GetCount();/*i++*/)
	{
		tagMatchTimer* pMatchTimer=m_MatchTimerArray[i];
		m_MatchTimerArray.RemoveAt(i);
		SafeDelete(pMatchTimer);
	}
}