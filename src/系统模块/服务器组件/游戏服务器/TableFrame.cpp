#include "StdAfx.h"
#include "TableFrame.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"
#include <float.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////

//���߶���
#define IDI_OFF_LINE				(TIME_TABLE_SINK_RANGE+1)			//���߱�ʶ
#define IDI_ANDROID_THEJOINT		(TIME_TABLE_SINK_RANGE+2)			//�����˴�������
#define MAX_OFF_LINE				3									//���ߴ���
#define TIME_OFF_LINE				60000L								//����ʱ��
#define TIME_OFF_LINE_PERSONAL		300000L								//����ʱ��
#define TIME_ANDROID_THEJOINT		30000L								//�����뿪ʱ��
//��ʼ��ʱ
#define IDI_START_OVERTIME		(TIME_TABLE_SINK_RANGE+3)			    //��ʼ��ʱ
#define IDI_START_OVERTIME_END	(TIME_TABLE_SINK_RANGE+22)			    //��ʼ��ʱ
#ifndef _DEBUG
#define TIME_OVERTIME				30000L								//��ʱʱ��
#else
#define TIME_OVERTIME               30000L                               //��ʱʱ��
#endif

//˽�˷���
#define IDI_PERSONAL_BEFORE_BEGAIN_TIME		TIME_TABLE_SINK_RANGE+23		//˽�˷��䳬ʱ
#define IDI_PERSONAL_AFTER_BEGIN_TIME			TIME_TABLE_SINK_RANGE+24		//���ʱ
#define IDI_PERSONAL_AFTER_CREATE_ROOM_TIME			TIME_TABLE_SINK_RANGE+25		//���䴴����೤ʱ������������ɢ����


//////////////////////////////////////////////////////////////////////////////////

//��Ϸ��¼
CGameScoreRecordArray				CTableFrame::m_GameScoreRecordBuffer;

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrame::CTableFrame()
{
	//��������
	m_wTableID=0;
	m_wChairCount=0;
	m_cbStartMode=START_MODE_ALL_READY;
	m_wUserCount=0;

	//��־����
	m_bGameStarted=false;
	m_bDrawStarted=false;
	m_bTableStarted=false;
	m_bTableInitFinish=false;
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));
	ZeroMemory(m_lFrozenedScore,sizeof(m_lFrozenedScore));
	//ZeroMemory(m_lWriteuser,sizeof(m_lWriteuser));
	//��Ϸ����
	m_lCellScore=0L;
	m_wDrawCount=0;
	m_cbGameStatus=GAME_STATUS_FREE;

	//ʱ�����
	m_dwDrawStartTime=0L;
	ZeroMemory(&m_SystemTimeStart,sizeof(m_SystemTimeStart));

	//��̬����
	m_dwTableOwnerID=0L;
	m_dwRecordTableID = 0;
	ZeroMemory(m_szEnterPassword,sizeof(m_szEnterPassword));

	//���߱���
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
	ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

	//������Ϣ
	m_pGameParameter=NULL;
	m_pGameMatchOption=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//����ӿ�
	m_pITimerEngine=NULL;
	m_pITableFrameSink=NULL;
	m_pIMainServiceFrame=NULL;
	m_pIAndroidUserManager=NULL;

	//���Žӿ�
	m_pITableUserAction=NULL;
	m_pITableUserRequest=NULL;

	//���ݽӿ�
	m_pIKernelDataBaseEngine=NULL;
	m_pIRecordDataBaseEngine=NULL;

	//�����ӿ�
	m_pITableFrameHook=NULL;
	m_pIMatchTableAction=NULL;

	//�û�����
	ZeroMemory(m_TableUserItemArray,sizeof(m_TableUserItemArray));

	//˽�˷���
	m_bPersonalLock = false;
	m_pTableOwnerUser = NULL;
	m_dwDrawCountLimit = 0;
	m_dwDrawTimeLimit = 0;
	m_lIniScore = 0;
	m_dwPersonalPlayCount = 0;
	m_dwTimeBegin = 0;
	m_PlayerWaitTime = 0;
	m_TableOwnerWaitTime = 0;
	m_GameStartOverTime = 0;
	m_bWaitConclude = false;
	m_dwReplyCount = 0;
	ZeroMemory(m_bRequestReply, sizeof(m_bRequestReply));
	ZeroMemory(m_szPersonalTableID,sizeof(m_szPersonalTableID));
	m_MapPersonalTableScoreInfo.InitHashTable(MAX_CHAIR);

	//�Ƿ�ǿ�ƽ�ɢ��Ϸ
	m_bIsDissumGame = false;
	memset(m_cbSpecialInfo, 0, sizeof(m_cbSpecialInfo));
	m_nSpecialInfoLen = 0;
	m_bIsAllRequstCancel = false;
	memset(m_bBeOut, 0, sizeof(m_bBeOut));
	m_bFirstRegister = false;
	return;
}

//��������
CTableFrame::~CTableFrame()
{
	//�ͷŶ���
	SafeRelease(m_pITableFrameSink);
	SafeRelease(m_pITableFrameHook);

	return;
}

//�ӿڲ�ѯ
VOID * CTableFrame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrame,Guid,dwQueryVer);
	QUERYINTERFACE(ICompilationSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrame,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��Ϸ
bool CTableFrame::StartGame()
{
	//��Ϸ״̬
	ASSERT(m_bDrawStarted==false);
	if (m_bDrawStarted==true) return false;

	//�������
	bool bGameStarted=m_bGameStarted;
	bool bTableStarted=m_bTableStarted;

	//����״̬
	m_bGameStarted=true;
	m_bDrawStarted=true;
	m_bTableStarted=true;
	//ZeroMemory(m_lWriteuser,sizeof(m_lWriteuser));
	//��ʼʱ��
	GetLocalTime(&m_SystemTimeStart);
	m_dwDrawStartTime=(DWORD)time(NULL);

	//��¼ʱ��
	if(m_dwPersonalPlayCount == 0 && m_bPersonalLock == true)
	{
		m_dwTimeBegin = (DWORD)time(NULL);

	}

	if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		m_dwPersonalPlayCount++;

	//��¼����
	m_dwRecordTableID = m_dwTableOwnerID;

	//��ʼ����
	if (bGameStarted==false)
	{
		//״̬����
		ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
		ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));
		memset(m_bBeOut, 0, sizeof(m_bBeOut));
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);

		//�����û�
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//�����û�
			if (pIServerUserItem!=NULL)
			{
				//������Ϸ��
				if (m_pGameServiceOption->lServiceScore>0L)
				{
					m_lFrozenedScore[i]=m_pGameServiceOption->lServiceScore;
					pIServerUserItem->FrozenedUserScore(m_pGameServiceOption->lServiceScore);
				}

				//����״̬
				BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
				if ((cbUserStatus!=US_OFFLINE)&&(cbUserStatus!=US_PLAYING))
				{
					pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,i);
					printf("��Ϸ��ʼ�������״̬_%d\n",pIServerUserItem->GetUserStatus());
				}

				////���ʱ��
				if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL && i == 0)
					SetGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME, m_PersonalTableParameter.dwTimeAfterBeginCount*60*1000,1,i);
				if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL &&  m_szPersonalTableID != NULL )		
				{
					m_pIMainServiceFrame->PersonalRoomWriteJoinInfo(pIServerUserItem->GetUserID(),  m_wTableID, m_szPersonalTableID);
				}
				

			}
		}

		//����״̬
		if (bTableStarted!=m_bTableStarted) SendTableStatus();
	}

	//������¼
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pUserItem = GetTableUserItem(i);
		if(pUserItem == NULL) continue;

		m_MapPersonalTableScoreInfo[pUserItem->GetUserID()] = pUserItem->GetUserScore();
	}

	//����֪ͨ
	bool bStart=true;
	if(m_pITableFrameHook!=NULL) bStart=m_pITableFrameHook->OnEventGameStart(m_wChairCount);

	//֪ͨ�¼�
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL&&bStart) m_pITableFrameSink->OnEventGameStart();

	return true;
}

//��ɢ��Ϸ
bool CTableFrame::DismissGame()
{
	//״̬�ж�
	ASSERT(m_bTableStarted==true);
	if (m_bTableStarted==false) return false;

	//������Ϸ
	if ((m_bGameStarted==true)&&(m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		//д�����
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//���ֱ���
			tagScoreInfo ScoreInfoArray;
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
			ScoreInfoArray.cbType = SCORE_TYPE_FLEE;								//��������
			ScoreInfoArray.lScore = 0;								//�û�����
			ScoreInfoArray.lGrade = 0;								//�û��ɼ�
			ScoreInfoArray.lRevenue = 0;							//��Ϸ˰��
			WriteUserScore(i,ScoreInfoArray);
		}
	}

	//����״̬
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//���ñ���
		m_bTableStarted=false;

		//����״̬
		SendTableStatus();
	}


	return false;
}

//������Ϸ
bool CTableFrame::ConcludeGame(BYTE cbGameStatus)
{
	//Ч��״̬
	ASSERT(m_bGameStarted==true);
	if (m_bGameStarted==false) return false;

	//�������
	bool bDrawStarted=m_bDrawStarted;

	//����״̬
	m_bDrawStarted=false;
	m_cbGameStatus=cbGameStatus;
	m_bGameStarted=(cbGameStatus>=GAME_STATUS_PLAY)?true:false;
	m_wDrawCount++;

	//�رտ�ʼ��ʱ��ʱ��
	KillGameTimer(IDI_PERSONAL_AFTER_BEGIN_TIME);

	//��Ϸ��¼
	RecordGameScore(bDrawStarted);
	
	//��������
	if (m_bGameStarted==false)
	{
		//��������
		bool bOffLineWait=false;

		//�����û�
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//�û�����
			if (pIServerUserItem!=NULL)
			{
				//������Ϸ��
				if (m_lFrozenedScore[i]!=0L)
				{
					pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[i]);
					m_lFrozenedScore[i]=0L;
				}

				//����״̬
				if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
				{
					//���ߴ���
					bOffLineWait=true;
					if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
					{						
						pIServerUserItem->SetClientReady(true);
						pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);
					}
					else
					{
						PerformStandUpAction(pIServerUserItem);
					}
				}
				else
				{
					if (pIServerUserItem->GetUserStatus() == US_NULL)
					{
						pIServerUserItem->SetUserStatus(US_NULL,m_wTableID,i);
						PerformStandUpAction(pIServerUserItem);
					}
					else
					{
						//����״̬
						pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);	
					}
				}
			}
		}

		//ɾ��ʱ��
		if (bOffLineWait==true) KillGameTimer(IDI_OFF_LINE);
	}

	//֪ͨ����
	if(m_pITableFrameHook!=NULL) m_pITableFrameHook->OnEventGameEnd(0, NULL, cbGameStatus);

	//��������
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL) m_pITableFrameSink->RepositionSink();

	//�߳����
	if (m_bGameStarted==false)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			if (m_TableUserItemArray[i]==NULL) continue;
			IServerUserItem * pIServerUserItem=m_TableUserItemArray[i];

			//if (m_pGameServiceOption->lCellScore < 1)
			//{
			//	m_pGameServiceOption->lCellScore  = 1;
			//}

			//��������
			if ((m_pGameServiceOption->lMinTableScore!=0L)&&
				(pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore)&& 
				((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) && (pIServerUserItem->IsAndroidUser() || m_pGameServiceAttrib->wChairCount>= MAX_CHAIR)				
				)
			{
				//������ʾ
				TCHAR szDescribe[128]=TEXT("");
				if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
				{
					_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"),m_pGameServiceOption->lMinTableScore);
				}
				else
				{
					_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������Ϸ�������� ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"),m_pGameServiceOption->lMinTableScore);
				}

				//������Ϣ
				if (pIServerUserItem->IsAndroidUser()==true)
					SendGameMessage(pIServerUserItem,szDescribe,SMT_CHAT|SMT_CLOSE_GAME|SMT_CLOSE_ROOM|SMT_EJECT);
				else
					SendGameMessage(pIServerUserItem,szDescribe,SMT_CHAT|SMT_CLOSE_GAME|SMT_EJECT);

				//�û�����
				PerformStandUpAction(pIServerUserItem);

				continue;
			}

			if (((m_pGameServiceOption->lMinTableScore!=0L) && pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
			{
				//������ʾ
				Personal_Room_Message personalRoomMessage;
				if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
				{
					_sntprintf(personalRoomMessage.szMessage,CountArray(personalRoomMessage.szMessage),TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"),m_pGameServiceOption->lMinTableScore* m_lCellScore/m_pGameServiceOption->lCellScore);
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));
					m_bBeOut[i]  = true;
					continue;
				}
			}

			//�ر��ж�
			if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
			{
				//������Ϣ
				LPCTSTR pszMessage=TEXT("����ϵͳά������ǰ��Ϸ���ӽ�ֹ�û�������Ϸ��");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

				//�û�����
				PerformStandUpAction(pIServerUserItem);

				continue;
			}

			//��������
			if (pIServerUserItem->GetUserStatus()!=US_OFFLINE && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				//��������
				if (pIServerUserItem->IsAndroidUser()==true)
				{
					//����Ϣ
					CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;
					tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

					//���һ���
					IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(),pBindParameter->dwSocketID);
					if(pIAndroidUserItem==NULL) continue;

					//�뿪�ж�
					//if(pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITLEAVE))
					{
						//��������
						//PerformStandUpAction(pIServerUserItem);

						continue;
					}

					//�����ж�
					//if(pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP))
					{
						//��������
						//PerformStandUpAction(pIServerUserItem);

						continue;
					}												
				}
			}

			if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
			{
				//������ʱ
				if(IsGameStarted()==false && pIServerUserItem->IsMobileUser() && m_pGameServiceAttrib->wChairCount<MAX_CHAIR)						SetGameTimer(IDI_START_OVERTIME+i,TIME_OVERTIME,1,i);
			}
		}		
	}

	//��������
	ConcludeTable();

	//����״̬
	SendTableStatus();

	//�߳��û�
	if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		DWORD dwTimeNow = (DWORD)time(NULL);
		if((m_dwDrawCountLimit!=0 && m_dwDrawCountLimit <= m_dwPersonalPlayCount) ||
			(m_dwDrawTimeLimit != 0 && m_dwDrawTimeLimit <= dwTimeNow - m_dwTimeBegin))
		{

			m_dwPersonalPlayCount = 0;

			CMD_GR_PersonalTableEnd PersonalTableEnd;
			ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));

			lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("Լս������"), CountArray(PersonalTableEnd.szDescribeString));
			TCHAR szInfo[260] = {0};
			for(int i = 0; i < m_wChairCount; ++i)
			{
				PersonalTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;
			}
			//���������Ϣ
			memcpy( PersonalTableEnd.cbSpecialInfo, m_cbSpecialInfo, m_nSpecialInfoLen);
			PersonalTableEnd.nSpecialInfoLen = m_nSpecialInfoLen;


			WORD wDataSize = sizeof(CMD_GR_PersonalTableEnd) - sizeof(PersonalTableEnd.lScore) + sizeof(LONGLONG) * m_wChairCount;

			for(int i = 0; i < m_wChairCount; ++i)
			{
				IServerUserItem* pUserItem = GetTableUserItem(i);
				if(pUserItem == NULL) continue;

				PersonalTableEnd.lScore[i] =m_PersonalUserScoreInfo[i].lScore;
				m_pIMainServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd) );
			}


			//��ֹ�ͻ�����Ϊ״̬Ϊfree��ȡ�����û�ָ��
			for(int i = 0; i < m_wChairCount; ++i)
			{
				IServerUserItem* pTableUserItem = GetTableUserItem(i);
				if(pTableUserItem == NULL) continue;

				PerformStandUpAction(pTableUserItem);
			}

			//LogPrintf(("��ɢ����"));
			//��ɢ����
			m_pIMainServiceFrame->DismissPersonalTable(m_pGameServiceOption->wServerID, m_wTableID);

			//��������
			m_dwTableOwnerID = 0;
			m_dwDrawCountLimit = 0;
			m_dwDrawTimeLimit = 0;
			m_dwTimeBegin = 0;

			m_dwReplyCount = 0;
			ZeroMemory(m_bRequestReply,sizeof(m_bRequestReply));
			//ZeroMemory(m_szPersonalTableID,sizeof(m_szPersonalTableID));
			m_MapPersonalTableScoreInfo.RemoveAll();
		}

	}

	return true;
}

//��������
bool CTableFrame::ConcludeTable()
{
	//��������
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//�����ж�
		WORD wTableUserCount=GetSitUserCount();
		if (wTableUserCount==0) m_bTableStarted=false;
		if (m_pGameServiceAttrib->wChairCount==MAX_CHAIR) m_bTableStarted=false;

		//ģʽ�ж�
		if (m_cbStartMode==START_MODE_FULL_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_PAIR_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_ALL_READY) m_bTableStarted=false;
	}

	return true;
}


//д�����
bool CTableFrame::WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal, DWORD dwPlayGameTime)
{
	//Ч�����
	ASSERT((wChairID<m_wChairCount)&&(ScoreInfo.cbType!=SCORE_TYPE_NULL));
	if ((wChairID>=m_wChairCount)&&(ScoreInfo.cbType==SCORE_TYPE_NULL)) 
		return false;

	//��ȡ�û�
	//ASSERT(GetTableUserItem(wChairID)!=NULL);
	IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
	TCHAR szMessage[128]=TEXT("");
	//if(m_lWriteuser[wChairID] == true)
	//	return true;
	//else m_lWriteuser[wChairID] = true;
	//д�����
	if (pIServerUserItem!=NULL)
	{
		if (ScoreInfo.lScore - m_pGameParameter->lHallNoticeThreshold >= DBL_EPSILON)
		{
			TCHAR szHallMessage[128] = TEXT("");
			_sntprintf(szHallMessage, sizeof(szHallMessage), TEXT("���˵�ͷ!ID:%d�ڡ�%s����һ��Ӯ��%0.2f��!"), pIServerUserItem->GetGameID(), m_pGameServiceOption->szServerName, ScoreInfo.lScore);
			m_pIMainServiceFrame->SendHallMessage(szHallMessage, 0);
		}

		//��������
		DWORD dwUserMemal=0L;
		SCORE lRevenueScore=__min(m_lFrozenedScore[wChairID],m_pGameServiceOption->lServiceScore);

		//�۷����
		if (m_pGameServiceOption->lServiceScore>0L 
			&& m_pGameServiceOption->wServerType == GAME_GENRE_GOLD
			&& m_pITableFrameSink->QueryBuckleServiceCharge(wChairID))
		{
			//�۷����
			ScoreInfo.lScore-=lRevenueScore;
			ScoreInfo.lRevenue+=lRevenueScore;

			//������Ϸ��
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//���Ƽ���
		if(dwGameMemal != INVALID_DWORD)
		{
			dwUserMemal = dwGameMemal;
		}
		else if (ScoreInfo.lRevenue>0L)
		{
			DWORD dwMedalRate=m_pGameParameter->dwMedalRate;
			dwUserMemal=(DWORD)(ScoreInfo.lRevenue*dwMedalRate/1000L);
		}

		//��Ϸʱ��
		DWORD dwCurrentTime=(DWORD)time(NULL);
		DWORD dwPlayTimeCount=((m_bDrawStarted==true)?(dwCurrentTime-m_dwDrawStartTime):0L);
		if(dwPlayGameTime!=INVALID_DWORD) dwPlayTimeCount=dwPlayGameTime;

		//��������
		tagUserProperty * pUserProperty=pIServerUserItem->GetUserProperty();

		//�����ж�
		if(m_pGameServiceOption->wServerType == GAME_GENRE_SCORE)
		{
			if (ScoreInfo.lScore>0L)
			{
				//����˫��
				tagPropertyBuff* pPropertyBuff = CGamePropertyManager::SearchValidPropertyBuff(pIServerUserItem->GetUserID(), PROP_KIND_DOOUBLE);
				if ( pPropertyBuff != NULL )
				{
					//���ַ��� 2�� 4��
					ScoreInfo.lScore *= pPropertyBuff->dwScoreMultiple;
					if( pPropertyBuff->dwScoreMultiple == 4 )
					{
						_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ �ı����ֿ� ]���÷ַ��ı���)"),pIServerUserItem->GetNickName());
					}
					else if( pPropertyBuff->dwScoreMultiple == 2 )
					{
						_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ ˫�����ֿ� ]���÷ַ�����"), pIServerUserItem->GetNickName());
					}
				}
			}
			else
			{
				//�����
				tagPropertyBuff* pPropertyBuff = CGamePropertyManager::SearchValidPropertyBuff(pIServerUserItem->GetUserID(), PROP_KIND_DEFENSE);
				if ( pPropertyBuff != NULL )
				{
					//���ֲ���
					ScoreInfo.lScore = 0;
					_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ ������� ]�����ֲ��䣡"),pIServerUserItem->GetNickName());
				}
			}
		}

		//д�����
		DWORD dwWinExperience=ScoreInfo.cbType==SCORE_TYPE_WIN?m_pGameParameter->dwWinExperience:0;
		pIServerUserItem->WriteUserScore(ScoreInfo.lScore,ScoreInfo.lGrade,ScoreInfo.lRevenue,dwUserMemal,ScoreInfo.cbType,dwPlayTimeCount,dwWinExperience);

		//��ȡ����
		tagVariationInfo VariationInfo;
		pIServerUserItem->DistillVariation(VariationInfo);

		m_PersonalUserScoreInfo[wChairID].dwUserID = pIServerUserItem->GetUserID();		//�û�ID
		//�û��ǳ�
		lstrcpyn(m_PersonalUserScoreInfo[wChairID].szUserNicname, pIServerUserItem->GetNickName(), sizeof(m_PersonalUserScoreInfo[wChairID].szUserNicname));
		m_PersonalUserScoreInfo[wChairID].lScore += ScoreInfo.lScore;							//�û�����
		m_PersonalUserScoreInfo[wChairID].lGrade += ScoreInfo.lGrade;							//�û��ɼ�
		m_PersonalUserScoreInfo[wChairID].lTaxCount += ScoreInfo.lRevenue;					//�û�˰��

		//��Ϸ��¼
		if (pIServerUserItem->IsAndroidUser()==false && CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//��������
			tagGameScoreRecord * pGameScoreRecord=NULL;

			//��ѯ���
			if (m_GameScoreRecordBuffer.GetCount()>0L)
			{
				//��ȡ����
				INT_PTR nCount=m_GameScoreRecordBuffer.GetCount();
				pGameScoreRecord=m_GameScoreRecordBuffer[nCount-1];

				//ɾ������
				m_GameScoreRecordBuffer.RemoveAt(nCount-1);
			}

			//��������
			if (pGameScoreRecord==NULL)
			{
				try
				{
					//��������
					pGameScoreRecord=new tagGameScoreRecord;
					if (pGameScoreRecord==NULL) throw TEXT("��Ϸ��¼���󴴽�ʧ��");
				}
				catch (...)
				{
					ASSERT(FALSE);
				}
			}

			//��¼����
			if (pGameScoreRecord!=NULL)
			{
				//�û���Ϣ
				pGameScoreRecord->wChairID=wChairID;
				pGameScoreRecord->dwUserID=pIServerUserItem->GetUserID();
				pGameScoreRecord->cbAndroid=(pIServerUserItem->IsAndroidUser()?TRUE:FALSE);

				//�û���Ϣ
				pGameScoreRecord->dwDBQuestID=pIServerUserItem->GetDBQuestID();
				pGameScoreRecord->dwInoutIndex=pIServerUserItem->GetInoutIndex();

				//�ɼ���Ϣ
				pGameScoreRecord->lScore=ScoreInfo.lScore;
				pGameScoreRecord->lGrade=ScoreInfo.lGrade;
				pGameScoreRecord->lRevenue=ScoreInfo.lRevenue;

				//������Ϣ
				pGameScoreRecord->dwUserMemal=dwUserMemal;
				pGameScoreRecord->dwPlayTimeCount=dwPlayTimeCount;

				//��������˰
				if(pIServerUserItem->IsAndroidUser())
				{
					pGameScoreRecord->lScore += pGameScoreRecord->lRevenue;
					pGameScoreRecord->lRevenue = 0L;
				}

				//��������
				m_GameScoreRecordActive.Add(pGameScoreRecord);
			}
		}

		//��Ϸ��¼
		if(dwGameMemal != INVALID_DWORD || dwPlayGameTime!=INVALID_DWORD)
		{
			DWORD dwStartGameTime=INVALID_DWORD;
			if(dwPlayGameTime!=INVALID_DWORD)
			{
				dwStartGameTime=(dwCurrentTime-dwPlayGameTime);
			}
			RecordGameScore(true, dwStartGameTime);
		}
	}
	else
	{
		////�뿪�û�
		//CTraceService::TraceString(TEXT("ϵͳ��ʱδ֧���뿪�û��Ĳ��ֲ�������"),TraceLevel_Exception);

		//return false;
	}

	//�㲥��Ϣ
	if (szMessage[0]!=0)
	{
		//��������
		IServerUserItem * pISendUserItem = NULL;
		WORD wEnumIndex=0;

		//��Ϸ���
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			pISendUserItem=GetTableUserItem(i);
			if(pISendUserItem==NULL) continue;

			//������Ϣ
			SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
		}

		//�Թ��û�
		do
		{
			pISendUserItem=EnumLookonUserItem(wEnumIndex++);
			if(pISendUserItem!=NULL) 
			{
				//������Ϣ
				SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
			}
		} while (pISendUserItem!=NULL);
	}

	return true;
}

//д�����
bool CTableFrame::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//Ч�����
	ASSERT(wScoreCount==m_wChairCount);
	if (wScoreCount!=m_wChairCount) return false;

	//д�����
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (ScoreInfoArray[i].cbType!=SCORE_TYPE_NULL)
		{
			WriteUserScore(i,ScoreInfoArray[i]);
		}
	}

	return true;
}

//����˰��
SCORE CTableFrame::CalculateRevenue(WORD wChairID, SCORE lScore)
{
	//Ч�����
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return 0L;

	//����˰��
	if ((m_pGameServiceOption->wRevenueRatio>0 || m_PersonalRoomOption.lPersonalRoomTax > 0)&&(lScore>=REVENUE_BENCHMARK))
	{
		//��ȡ�û�
		ASSERT(GetTableUserItem(wChairID)!=NULL);
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);

		//����˰��
		SCORE lRevenue=lScore*m_pGameServiceOption->wRevenueRatio/REVENUE_DENOMINATOR;

		if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		{
			lRevenue=lScore*(m_pGameServiceOption->wRevenueRatio + m_PersonalRoomOption.lPersonalRoomTax)/REVENUE_DENOMINATOR ;
		}

		return lRevenue;
	}

	return 0L;
}

//�����޶�
SCORE CTableFrame::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	//�û�Ч��
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	if (pIServerUserItem->GetTableID()!=m_wTableID) return 0L;

	//��ѯ���
	SCORE lTrusteeScore=pIServerUserItem->GetTrusteeScore();
	SCORE lMinEnterScore=m_pGameServiceOption->lMinTableScore;
	SCORE lUserConsumeQuota=m_pITableFrameSink->QueryConsumeQuota(pIServerUserItem);

	//Ч����
	ASSERT((lUserConsumeQuota>=0L)&&(lUserConsumeQuota<=pIServerUserItem->GetUserScore()-lMinEnterScore));
	if ((lUserConsumeQuota<0L)||(lUserConsumeQuota>pIServerUserItem->GetUserScore()-lMinEnterScore)) return 0L;

	return lUserConsumeQuota+lTrusteeScore;
}

//Ѱ���û�
IServerUserItem * CTableFrame::SearchUserItem(DWORD dwUserID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//�����û�
	for (WORD i=0;i<m_wChairCount;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	}

	//�Թ��û�
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	} while (pIServerUserItem!=NULL);

	return NULL;
}
//Ѱ���û�
IServerUserItem * CTableFrame::SearchUserItemGameID(DWORD dwGameID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//�����û�
	for (WORD i=0;i<m_wChairCount;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetGameID()==dwGameID)) return pIServerUserItem;
	}

	//�Թ��û�
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetGameID()==dwGameID)) return pIServerUserItem;
	} while (pIServerUserItem!=NULL);

	return NULL;
}
//��Ϸ�û�
IServerUserItem * CTableFrame::GetTableUserItem(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;

	//��ȡ�û�
	return m_TableUserItemArray[wChairID];
}

//�Թ��û�
IServerUserItem * CTableFrame::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_LookonUserItemArray.GetCount()) return NULL;
	return m_LookonUserItemArray[wEnumIndex];
}

//����ʱ��
bool CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//Ч�����
	ASSERT((dwTimerID>0)&&(dwTimerID<TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>=TIME_TABLE_MODULE_RANGE)) return false;

	//����ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//ɾ��ʱ��
bool CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	//Ч�����
	ASSERT((dwTimerID>0)&&(dwTimerID<=TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//ɾ��ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//��������
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	//���⴦��
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendData(BG_ALL_CLIENT,MDM_GF_GAME,wSubCmdID,NULL,0);
	}

	//״̬Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//��������
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//���⴦��
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendData(BG_ALL_CLIENT,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
	}

	//״̬Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//��������
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//Ч��״̬
			ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) continue;

			//��������
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

		return true;
	}
	else
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) return false;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

		return true;
	}

	return false;
}

//��������
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID)
{
	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

			//��������
			m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}
	else
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

		//��������
		m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);

		return true;
	}

	return false;
}

//��������
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

	} while (true);

	return true;
}

//��������
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		}

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrame::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	WORD wEnumIndex=0;

	//������Ϣ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		//������Ϣ
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//ö���û�
	do
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//������Ϣ
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrame::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//���⴦��
	if (pIServerUserItem==NULL)
	{
		return m_pIMainServiceFrame->SendRoomMessage(lpszMessage,wType);
	}

	//�û�Ч��
	/*ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;*/

	//������Ϣ
	m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//��Ϸ��Ϣ
bool CTableFrame::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������Ϣ
	return m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
}

//���ͳ���
bool CTableFrame::SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

	//���ͳ���
	ASSERT(m_pIMainServiceFrame!=NULL);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_SCENE,pData,wDataSize);

	return true;
}

//���ýӿ�
bool CTableFrame::SetTableFrameHook(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	if (pIUnknownEx==NULL) return false;

	//�����ж�
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;

	//��ѯ�ӿ�
	m_pITableFrameHook=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameHook);
	m_pIMatchTableAction=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableUserAction);

	return true;
}

//α������
bool CTableFrame::ImitateGameOption(IServerUserItem * pIServerUserItem)
{
	//����У��
	ASSERT(pIServerUserItem!=NULL);	
	if (pIServerUserItem==NULL) return false;

	//״̬�ж�
	if (m_bGameStarted==true) return true;
	if (pIServerUserItem->GetUserMatchStatus()!=MUS_PLAYING) return false;

	//�����ж�
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;	
	
	//����״̬
	pIServerUserItem->SetClientReady(true);

	//��ʼ�ж�
	if ((pIServerUserItem->GetUserStatus()==US_READY)&&(EfficacyStartGame(pIServerUserItem->GetChairID())==true))
	{
		StartGame(); 
	}

	return true;
}

bool CTableFrame::SendChatMessage(IServerUserItem * pIServerUserItem,IServerUserItem * pITargetServerUserItem,DWORD dwChatColor, LPCTSTR lpszChatString, LPTSTR lpszDescribeString)
{
	//����У��
	ASSERT(pIServerUserItem!=NULL);	
	if (pIServerUserItem==NULL) return false;

	//��ȡĿ��
	DWORD dwTargetUserID = 0;
	if (pITargetServerUserItem!=NULL)
	{
		dwTargetUserID = pITargetServerUserItem->GetUserID();
	}

	DWORD wDescribeLength = lstrlen(lpszDescribeString) + 1;
	if (wDescribeLength>LEN_USER_CHAT) wDescribeLength=LEN_USER_CHAT;
	WORD wChatLength = lstrlen(lpszChatString) + 1;
	if (wChatLength>LEN_USER_CHAT) wChatLength=LEN_USER_CHAT;

	//״̬�ж�
	if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		lstrcpyn(lpszDescribeString,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),wDescribeLength);
		return false;
	}

	//Ȩ���ж�
	if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
	{
		lstrcpyn(lpszDescribeString,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),wDescribeLength);
		return true;
	}

	//������Ϣ
	CMD_GF_S_UserChat UserChat;
	ZeroMemory(&UserChat,sizeof(UserChat));

	//�ַ�����
	m_pIMainServiceFrame->SensitiveWordFilter(lpszChatString,UserChat.szChatString,wChatLength);

	//��������
	UserChat.dwChatColor=dwChatColor;
	UserChat.wChatLength=wChatLength;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID=pIServerUserItem->GetUserID();

	//��������
	WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
	WORD wSendSize=wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]);

	//��Ϸ�û�
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
	}

	//�Թ��û�
	WORD wEnumIndex=0;
	IServerUserItem * pIWatchServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIWatchServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIWatchServerUserItem==NULL) break;

		//��������
		if (pIServerUserItem->IsClientReady()==true)
		{
			m_pIMainServiceFrame->SendData(pIWatchServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
		}
	} while (true);

	return true;
}

//�����¼�
bool CTableFrame::OnEventUserOffLine(IServerUserItem * pIServerUserItem)
{
	//����Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û�����
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	IServerUserItem * pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//�û�����
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//��Ϸ�û�
	if (cbUserStatus!=US_LOOKON)
	{
		//Ч���û�
		ASSERT(pIServerUserItem==GetTableUserItem(wChairID));
		if (pIServerUserItem!=GetTableUserItem(wChairID)) return false;

		//���ߴ���
		if ((cbUserStatus==US_PLAYING)&&(m_wOffLineCount[wChairID]<MAX_OFF_LINE) || 
			m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL &&
			(m_PersonalTableParameter.cbIsJoinGame && m_dwTableOwnerID == pUserInfo->dwUserID || (m_dwPersonalPlayCount != 0 && m_dwDrawCountLimit > m_dwPersonalPlayCount))
			)
		{
			//�û�����
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);

			//��������
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
			{
				if(pIServerUserItem->IsTrusteeUser()==false)
				{
					//�����й�
					pIServerUserItem->SetTrusteeUser(true);

					//����֪ͨ
					if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
				}

				return true;
			}
			else
			{
				//���ߴ���		
				if (cbUserStatus==US_PLAYING)
				{
					if(m_pITableUserAction->OnActionUserPour(wChairID,pIServerUserItem))
					{
						//�û�����
						if(m_pITableUserAction!=NULL && m_pGameServiceOption->wKindID == 306) 
						{
							m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
						}
						pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);
						pIServerUserItem->SetClientReady(true);
					}
					else
					{
						PerformStandUpAction(pIServerUserItem);
						pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);		
					}
		
				}else if(cbUserStatus==US_READY || cbUserStatus==US_SIT)  //�����û�׼����/ǰ��������
				{
					PerformStandUpAction(pIServerUserItem);	
					pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);			
				}
				return true;
			}
			//����֪ͨ
			if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);

			//���ߴ���
			DWORD dwElapse = TIME_OFF_LINE;
			if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				dwElapse = m_PersonalTableParameter.dwTimeOffLineCount;
			}
			
			//���ߴ���
			if (m_dwOffLineTime[wChairID]==0L)
			{
				//���ñ���
				m_wOffLineCount[wChairID]++;
				m_dwOffLineTime[wChairID]=(DWORD)time(NULL);

				//ʱ������
				WORD wOffLineCount=GetOffLineUserCount();

				if (wOffLineCount==1)
				{
					if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
					{
						SetGameTimer(IDI_OFF_LINE,dwElapse * 60 * 1000 ,1,wChairID);
					}
					else
					{
						SetGameTimer(IDI_OFF_LINE,dwElapse ,1,wChairID);
					}
				}
			}

			return true;
		}
		else
		{
			//���ߴ���		
			if (cbUserStatus==US_OFFLINE)
			{
				if(m_pITableUserAction->OnActionUserPour(wChairID,pIServerUserItem))
				{
					//�û�����
					pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);
					pIServerUserItem->SetClientReady(true);
				}
				else
				{
					PerformStandUpAction(pIServerUserItem);
					pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);		
				}
			}else if(cbUserStatus==US_READY || cbUserStatus==US_SIT)  //�����û�׼����/ǰ��������
			{
				PerformStandUpAction(pIServerUserItem);	
				pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);			
			}
			return true;

		}

	}

	//�û�����
	PerformStandUpAction(pIServerUserItem,true);
	
	//��������
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
	{
		if (pIServerUserItem->IsTrusteeUser()==true) return true;
	}

	//ɾ���û�
	ASSERT(pIServerUserItem->GetUserStatus()==US_FREE);
	pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);

	return true;
}

//�����¼�
bool CTableFrame::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//֪ͨ��Ϸ
	return m_pITableFrameSink->OnUserScroeNotify(wChairID,pIServerUserItem,cbReason);
}

//ʱ���¼�
bool CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//�ص��¼�
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		ASSERT(m_pITableFrameSink!=NULL);
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//�¼�����
	switch (dwTimerID)
	{
	case IDI_OFF_LINE:	//�����¼�
		{
			//Ч��״̬
			ASSERT(m_bGameStarted==true || m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL);
			if (m_bGameStarted==false && m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL) return false;

			//��������
			DWORD dwOffLineTime=0L;
			WORD wOffLineChair=INVALID_CHAIR;

			//Ѱ���û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
				{
					wOffLineChair=i;
					dwOffLineTime=m_dwOffLineTime[i];
				}
			}

			//λ���ж�
			ASSERT(wOffLineChair!=INVALID_CHAIR);
			if (wOffLineChair==INVALID_CHAIR) return false;

			//�û��ж�
			ASSERT(dwBindParameter<m_wChairCount);

			//˽�˷�����߳�ʱ�����������洦��
			if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
			{
				break;
				//return true;
			}

			DWORD dwElapse = TIME_OFF_LINE;
			if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)	dwElapse = TIME_OFF_LINE_PERSONAL;

			if (wOffLineChair!=(WORD)dwBindParameter)
			{
				//ʱ�����
				DWORD dwCurrentTime=(DWORD)time(NULL);
				DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

				//����ʱ��
				dwLapseTime=__min(dwLapseTime,dwElapse-2000L);
				SetGameTimer(IDI_OFF_LINE,dwElapse-dwLapseTime,1,wOffLineChair);

				return true;
			}

			//��ȡ�û�
			ASSERT(GetTableUserItem(wOffLineChair)!=NULL);
			IServerUserItem * pIServerUserItem=GetTableUserItem(wOffLineChair);

			//������Ϸ
			if (pIServerUserItem!=NULL)
			{
				//���ñ���
				m_dwOffLineTime[wOffLineChair]=0L;

				//�û�����
				PerformStandUpAction(pIServerUserItem,true);
			}

			//����ʱ��
			if (m_bGameStarted==true)
			{
				//��������
				DWORD dwOffLineTime=0L;
				WORD wOffLineChair=INVALID_CHAIR;

				//Ѱ���û�
				for (WORD i=0;i<m_wChairCount;i++)
				{
					if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
					{
						wOffLineChair=i;
						dwOffLineTime=m_dwOffLineTime[i];
					}
				}

				//����ʱ��
				if (wOffLineChair!=INVALID_CHAIR)
				{
					//ʱ�����
					DWORD dwCurrentTime=(DWORD)time(NULL);
					DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

					//����ʱ��
					dwLapseTime=__min(dwLapseTime,dwElapse-2000L);
					SetGameTimer(IDI_OFF_LINE,dwElapse-dwLapseTime,1,wOffLineChair);
				}
			}

			return true;
		}
	}

	//δ��ʼ��ʱ
	if(dwTimerID >= IDI_START_OVERTIME && dwTimerID <=IDI_START_OVERTIME_END)
	{
		//��������
		WORD wChair=(WORD)dwBindParameter;

		//
		if(wChair != (WORD)(dwTimerID-IDI_START_OVERTIME)) return false;

		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChair);

		//��ʱ����
		if(pIServerUserItem && pIServerUserItem->GetUserStatus()==US_SIT)
		{
			//�û�����
			PerformStandUpAction(pIServerUserItem);
		}

		return false;
	}

	//Լսδ��ʼ��Ϸ��ʱ
	if(dwTimerID == IDI_PERSONAL_BEFORE_BEGAIN_TIME  || dwTimerID ==  IDI_PERSONAL_AFTER_CREATE_ROOM_TIME)
	{
		if (dwTimerID == IDI_PERSONAL_BEFORE_BEGAIN_TIME)
		{
			OutputDebugStringA("ptdtcs **** IDI_PERSONAL_BEFORE_BEGAIN_TIME");
		}

		if (dwTimerID == IDI_PERSONAL_AFTER_CREATE_ROOM_TIME)
		{
			OutputDebugStringA("ptdtcs **** IDI_PERSONAL_BEFORE_BEGAIN_TIME");
		}
		//����״̬
		if(m_bGameStarted == true && m_bPersonalLock == true) return true;

		m_bIsEnfoceDissum = true;
		m_bIsDissumGame = true;
		//�˻�����
		m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit,CANCELTABLE_REASON_NOT_START ,  m_wTableID,  m_szPersonalTableID);//CANCELTABLE_REASON_ENFOCE
		return true;
	}

	//��Ϸ��ʼ֮��ʱ ��Ϸ���߳�ʱ
	if(dwTimerID == IDI_PERSONAL_AFTER_BEGIN_TIME  || dwTimerID == IDI_OFF_LINE  &&  m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL )
	{
		HostDissumeGame();

		return true;
	}
	if (IDI_ANDROID_THEJOINT == dwTimerID)
	{
		KillGameTimer(IDI_ANDROID_THEJOINT);
		SetGameTimer(IDI_ANDROID_THEJOINT, TIME_ANDROID_THEJOINT,1,NULL);
		DWORD dwTheJoint = rand()%4;
		if (dwTheJoint == 0)return true;
		DWORD dwIndex = 0;
		//��Ϸ�û�
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsAndroidUser()==false)) continue;
			//����Ϣ
			CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

			//���һ���
			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(),pBindParameter->dwSocketID);
			if(pIAndroidUserItem==NULL) continue;
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();
			if (pAndroidParameter == NULL)continue;
			if((pAndroidParameter->dwServiceMode&ANDROID_THEJOINT)!=0)
			{
				PerformStandUpAction(pIServerUserItem);
				dwIndex++;
			}
			if (dwIndex >= dwTheJoint)
			{
				break;
			}
		}
	}
	//�������
	ASSERT(FALSE);

	return false;
}

//��Ϸ�¼�
bool CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//��Ϣ����
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//����¼�
bool CTableFrame::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//��Ϸ����
	if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//Ĭ�ϴ���
	switch (wSubCmdID)
	{
	case SUB_GF_GAME_OPTION:	//��Ϸ����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GF_GameOption));
			if (wDataSize!=sizeof(CMD_GF_GameOption)) return false;

			//if (m_pGameServiceOption->lCellScore < 1)
			//{
			//	m_pGameServiceOption->lCellScore  = 1;
			//}

			if (((m_pGameServiceOption->lMinTableScore!=0L) && pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
			{
				//������ʾ
				Personal_Room_Message personalRoomMessage;
				if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
				{

					_sntprintf(personalRoomMessage.szMessage,CountArray(personalRoomMessage.szMessage),TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"),m_pGameServiceOption->lMinTableScore* m_lCellScore/m_pGameServiceOption->lCellScore);
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));
					return false;
				}

			}

			//��������
			CMD_GF_GameOption * pGameOption=(CMD_GF_GameOption *)pData;

			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//��������
			if ((cbUserStatus!=US_LOOKON)&&((m_dwOffLineTime[wChairID]!=0L)))
			{
				//���ñ���
				m_dwOffLineTime[wChairID]=0L;

				//ɾ��ʱ��
				WORD wOffLineCount=GetOffLineUserCount();
				if (wOffLineCount==0)
				{
					//OutputDebugStringA("ptdt *** OnEventSocketFrame ɱ�� ���߶�ʱ�� ��ʼ��ʱ��ʱ��");
					//LogPrintf(("OnEventSocketFrame ɱ�� ���߶�ʱ�� ��ʼ��ʱ��ʱ��"));
					KillGameTimer(IDI_OFF_LINE);
					KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
					OutputDebugStringA("ptdtcs ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 1");
				}
			}

			//����״̬
			pIServerUserItem->SetClientReady(true);
			if (cbUserStatus!=US_LOOKON) m_bAllowLookon[wChairID]=pGameOption->cbAllowLookon?true:false;

			//����״̬
			CMD_GF_GameStatus GameStatus;
			GameStatus.cbGameStatus=m_cbGameStatus;
			GameStatus.cbAllowLookon=m_bAllowLookon[wChairID]?TRUE:FALSE;
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_STATUS,&GameStatus,sizeof(GameStatus));

			//��ʾ��Ϣ
			CMD_GR_PersonalTableTip PersonalTableTip;
			ZeroMemory(&PersonalTableTip, sizeof(CMD_GR_PersonalTableTip));

			PersonalTableTip.dwTableOwnerUserID = m_dwTableOwnerID;
			PersonalTableTip.dwDrawCountLimit = m_dwDrawCountLimit;
			PersonalTableTip.dwDrawTimeLimit = m_dwDrawTimeLimit;
			PersonalTableTip.dwPlayCount = m_dwPersonalPlayCount;
			PersonalTableTip.dwPlayTime = (DWORD)time(NULL) - m_dwTimeBegin;
			PersonalTableTip.lCellScore = m_lCellScore;
			PersonalTableTip.lIniScore = m_lIniScore;
			lstrcpyn(PersonalTableTip.szServerID, m_szPersonalTableID, CountArray(PersonalTableTip.szServerID));
			PersonalTableTip.cbIsJoinGame = m_PersonalTableParameter.cbIsJoinGame;

			//���ʹ�õ��ǽ�����ݿ�
			if (lstrcmp(m_pGameServiceOption->szDataBaseName,  TEXT("RYTreasureDB")) == 0)
			{
				PersonalTableTip.cbIsGoldOrGameScore = 0;
			}
			else
			{
				PersonalTableTip.cbIsGoldOrGameScore = 1;
			}


			m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_TIP, &PersonalTableTip, sizeof(CMD_GR_PersonalTableTip));

			//������Ϣ
			TCHAR szMessage[128]=TEXT("");
			_sntprintf(szMessage,CountArray(szMessage),TEXT("��ӭ�����롰%s����Ϸ��ף����Ϸ��죡"),m_pGameServiceAttrib->szGameName);
			m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT);


			//���ͳ���
			bool bSendSecret=((cbUserStatus!=US_LOOKON)||(m_bAllowLookon[wChairID]==true));
			m_pITableFrameSink->OnEventSendGameScene(wChairID,pIServerUserItem,m_cbGameStatus,bSendSecret);
			

				//׼������

			for (WORD i=0;i<m_wChairCount;i++)
			{
				//��ȡ�û�
				IServerUserItem * pITableUserItem=GetTableUserItem(i);
				if (pITableUserItem != NULL) continue;
							//����״̬
				CMD_GF_GameUserData GameUserData;
				GameUserData.cbUserCharID=i;
				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_DATA,&GameUserData,sizeof(GameUserData));
			}

			//ȡ���й�
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
			{
				if(pIServerUserItem->IsTrusteeUser()==true)
				{
					//�����й�
					pIServerUserItem->SetTrusteeUser(false);
				}

				//�ع�֪ͨ
				if (cbUserStatus==US_PLAYING || cbUserStatus==US_OFFLINE)
				{
					if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserConnect(wChairID,pIServerUserItem);
					if(m_pITableFrameHook) m_pITableFrameHook->OnEventUserReturnMatch(pIServerUserItem);
				}
			}

			//��ʼ�ж�
			if ((cbUserStatus==US_READY)&&(EfficacyStartGame(wChairID)==true))
			{
				StartGame(); 
			}

			return true;
		}
	case SUB_GF_USER_READY:		//�û�׼��
		{
			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//Ч��״̬
			ASSERT(GetTableUserItem(wChairID)==pIServerUserItem);
			if (GetTableUserItem(wChairID)!=pIServerUserItem) return false;

			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD  && pIServerUserItem->GetUserScore() < m_pGameServiceOption->lMinTableScore)
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"),m_pGameServiceOption->lMinTableScore);
				//������Ϣ
				if (pIServerUserItem->IsAndroidUser()==true)
					SendGameMessage(pIServerUserItem,szDescribe,SMT_CHAT|SMT_CLOSE_GAME|SMT_CLOSE_ROOM|SMT_EJECT);
				else
					SendGameMessage(pIServerUserItem,szDescribe,SMT_CHAT|SMT_CLOSE_GAME|SMT_EJECT);

				//�û�����
				//�뿪����
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					PerformStandUpAction(pIServerUserItem);
				}
				return true;
			}

			//Ч��״̬
			//ASSERT(cbUserStatus==US_SIT);
			if (cbUserStatus!=US_SIT) return true;

			//�����׷����ж�
			if((CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
				&& (m_wDrawCount >= m_pGameServiceOption->wDistributeDrawCount || CheckDistribute()))
			{
				//������Ϣ
				LPCTSTR pszMessage=TEXT("ϵͳ���·������ӣ����Ժ�");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_CHAT);

				//������Ϣ
				m_pIMainServiceFrame->InsertDistribute(pIServerUserItem);

				////�û�����
				//PerformStandUpAction(pIServerUserItem);

				return true;
			}

			//�¼�֪ͨ
			if(m_pIMatchTableAction!=NULL && !m_pIMatchTableAction->OnActionUserOnReady(wChairID,pIServerUserItem, pData,wDataSize))
			{
				return true;
			}

			//�¼�֪ͨ
			if (m_pITableUserAction!=NULL)
			{
				m_pITableUserAction->OnActionUserOnReady(wChairID,pIServerUserItem,pData,wDataSize);
			}

			//ɾ����ʱ
			if(m_pGameServiceAttrib->wChairCount < MAX_CHAIR) KillGameTimer(IDI_START_OVERTIME+wChairID);

			//��ʼ�ж�
			if (EfficacyStartGame(wChairID)==false)
			{
				//����״̬
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
			else
			{
				StartGame(); 
			}

			return true;
		}
	case SUB_GF_ANDEOID_QUIT:
		{
			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//Ч��״̬
			ASSERT(GetTableUserItem(wChairID)==pIServerUserItem);
			if (GetTableUserItem(wChairID)!=pIServerUserItem) return false;

			PerformStandUpAction(pIServerUserItem);
			return true;
		}
	case SUB_GF_USER_CHAT:		//�û�����
		{
			//��������
			CMD_GF_C_UserChat * pUserChat=(CMD_GF_C_UserChat *)pData;

			//Ч�����
			ASSERT(wDataSize<=sizeof(CMD_GF_C_UserChat));
			ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)));
			ASSERT(wDataSize==(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));

			//Ч�����
			if (wDataSize>sizeof(CMD_GF_C_UserChat)) return false;
			if (wDataSize<(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
			if (wDataSize!=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;

			//Ŀ���û�
			if ((pUserChat->dwTargetUserID!=0)&&(SearchUserItem(pUserChat->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//״̬�ж�
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),SMT_CHAT);
				return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//������Ϣ
			CMD_GF_S_UserChat UserChat;
			ZeroMemory(&UserChat,sizeof(UserChat));

			//�ַ�����
			m_pIMainServiceFrame->SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));

			//��������
			UserChat.dwChatColor=pUserChat->dwChatColor;
			UserChat.wChatLength=pUserChat->wChatLength;
			UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
			UserChat.dwSendUserID=pIServerUserItem->GetUserID();
			UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

			//��������
			WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
			WORD wSendSize=wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]);

			//��Ϸ�û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
			}

			//�Թ��û�
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//ö���û�
			do
			{
				//��ȡ�û�
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//��������
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
				}
			} while (true);

			return true;
		}
	case SUB_GF_USER_EXPRESSION:	//�û�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GF_C_UserExpression));
			if (wDataSize!=sizeof(CMD_GF_C_UserExpression)) return false;

			//��������
			CMD_GF_C_UserExpression * pUserExpression=(CMD_GF_C_UserExpression *)pData;

			//Ŀ���û�
			if ((pUserExpression->dwTargetUserID!=0)&&(SearchUserItem(pUserExpression->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//״̬�ж�
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),SMT_CHAT);
				return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//������Ϣ
			CMD_GR_S_UserExpression UserExpression;
			ZeroMemory(&UserExpression,sizeof(UserExpression));

			//��������
			UserExpression.wItemIndex=pUserExpression->wItemIndex;
			UserExpression.dwSendUserID=pIServerUserItem->GetUserID();
			UserExpression.dwTargetUserID=pUserExpression->dwTargetUserID;

			//��Ϸ�û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				//��������
				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));
			}

			//�Թ��û�
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//ö���û�
			do
			{
				//��ȡ�û�
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//��������
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));
				}
			} while (true);

			return true;
		}
	case SUB_GF_USER_VOICE:	//�û�����
		{
			//��������
			CMD_GF_C_UserVoice * pUserVoice=(CMD_GF_C_UserVoice *)pData;

			//Ч�����
			ASSERT(wDataSize<=sizeof(CMD_GF_C_UserVoice));
			ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)));
			ASSERT(wDataSize==(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)+pUserVoice->dwVoiceLength*sizeof(pUserVoice->byVoiceData[0])));

			//Ч�����
			if (wDataSize>sizeof(CMD_GF_C_UserVoice)) return false;
			if (wDataSize<(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData))) return false;
			if (wDataSize!=(sizeof(CMD_GF_C_UserVoice)-sizeof(pUserVoice->byVoiceData)+pUserVoice->dwVoiceLength*sizeof(pUserVoice->byVoiceData[0]))) return false;

			//Ŀ���û�
			if ((pUserVoice->dwTargetUserID!=0)&&(SearchUserItem(pUserVoice->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//״̬�ж�
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),SMT_CHAT);
				return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//������Ϣ
			CMD_GF_S_UserVoice UserVoice;
			ZeroMemory(&UserVoice,sizeof(UserVoice));

			//��������
			UserVoice.dwSendUserID=pIServerUserItem->GetUserID();
			UserVoice.dwTargetUserID=pUserVoice->dwTargetUserID;
			UserVoice.dwVoiceLength = pUserVoice->dwVoiceLength>MAXT_VOICE_LENGTH?MAXT_VOICE_LENGTH:pUserVoice->dwVoiceLength;
			CopyMemory(UserVoice.byVoiceData,pUserVoice->byVoiceData,UserVoice.dwVoiceLength);

			//��������
			WORD wHeadSize=sizeof(UserVoice)-sizeof(UserVoice.byVoiceData);
			WORD wSendSize=(WORD)(wHeadSize+UserVoice.dwVoiceLength*sizeof(UserVoice.byVoiceData[0]));

			//��Ϸ�û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				//��������
				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_VOICE,&UserVoice,wSendSize);
			}

			//�Թ��û�
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//ö���û�
			do
			{
				//��ȡ�û�
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//��������
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_VOICE,&UserVoice,wSendSize);
				}
			} while (true);

			return true;
		}
	case SUB_GF_LOOKON_CONFIG:		//�Թ�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//��������
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//Ŀ���û�
			if ((pLookonConfig->dwUserID!=0)&&(SearchUserItem(pLookonConfig->dwUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()!=US_LOOKON);
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//�Թ۴���
			if (pLookonConfig->dwUserID!=0L)
			{
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

					//������Ϣ
					ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//�����ж�
				bool bAllowLookon=(pLookonConfig->cbAllowLookon==TRUE)?true:false;
				if (bAllowLookon==m_bAllowLookon[pIServerUserItem->GetChairID()]) return true;

				//���ñ���
				m_bAllowLookon[pIServerUserItem->GetChairID()]=bAllowLookon;

				//������Ϣ
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//������Ϣ
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	}

	return false;
}

//��Ϸ����
WORD CTableFrame::GetDrawCount()
{
	return m_wDrawCount;
}

//��ȡ��λ
WORD CTableFrame::GetNullChairID()
{
	//��������
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_TableUserItemArray[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//�����λ
WORD CTableFrame::GetRandNullChairID()
{
	//��������
	WORD wIndex = rand()%m_wChairCount;
	for (WORD i=wIndex;i<m_wChairCount+wIndex;i++)
	{
		if (m_TableUserItemArray[i%m_wChairCount]==NULL)
		{
			return i%m_wChairCount;
		}
	}

	return INVALID_CHAIR;
}
//���ӻ���������
WORD CTableFrame::GetTableAndroidConut()
{
	WORD wAnddroidCount = 0;
	//�û�����
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;
		if (pIServerUserItem->IsAndroidUser())
			wAnddroidCount++;
	}
	if (wAnddroidCount == m_pGameServiceAttrib->wChairCount)
	{
		//������ʾ
		TCHAR szString[512]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("������������"));

		//��ʾ��Ϣ
		CTraceService::TraceString(szString,TraceLevel_Normal);
	}
	return wAnddroidCount;
}
//�û���Ŀ
WORD CTableFrame::GetSitUserCount()
{
	//��������
	WORD wUserCount=0;

	//��Ŀͳ��
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (GetTableUserItem(i)!=NULL)
		{
			wUserCount++;
		}
	}

	return wUserCount;
}

//�Թ���Ŀ
WORD CTableFrame::GetLookonUserCount()
{
	//��ȡ��Ŀ
	INT_PTR nLookonCount=m_LookonUserItemArray.GetCount();

	return (WORD)(nLookonCount);
}

//������Ŀ
WORD CTableFrame::GetOffLineUserCount()
{
	//��������
	WORD wOffLineCount=0;

	//��������
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_dwOffLineTime[i]!=0L)
		{
			wOffLineCount++;
		}
	}

	return wOffLineCount;
}

//����״��
WORD CTableFrame::GetTableUserInfo(tagTableUserInfo & TableUserInfo)
{
	//���ñ���
	ZeroMemory(&TableUserInfo,sizeof(TableUserInfo));

	//�û�����
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//�û�����
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			TableUserInfo.wTableUserCount++;
		}
		else
		{
			TableUserInfo.wTableAndroidCount++;
		}

		//׼���ж�
		if (pIServerUserItem->GetUserStatus()==US_READY)
		{
			TableUserInfo.wTableReadyCount++;
		}
	}

	//������Ŀ
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:		//����׼��
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_PAIR_READY:		//��Կ�ʼ
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_TIME_CONTROL:	//ʱ�����
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	default:						//Ĭ��ģʽ
		{
			TableUserInfo.wMinUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserInfo.wTableAndroidCount+TableUserInfo.wTableUserCount;
}

//��������
bool CTableFrame::InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//���ñ���
	m_wTableID=wTableID;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//���ò���
	m_pGameParameter=TableFrameParameter.pGameParameter;
	m_pGameMatchOption=TableFrameParameter.pGameMatchOption;
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;

	//����ӿ�
	m_pITimerEngine=TableFrameParameter.pITimerEngine;
	m_pIMainServiceFrame=TableFrameParameter.pIMainServiceFrame;
	m_pIAndroidUserManager=TableFrameParameter.pIAndroidUserManager;
	m_pIKernelDataBaseEngine=TableFrameParameter.pIKernelDataBaseEngine;
	m_pIRecordDataBaseEngine=TableFrameParameter.pIRecordDataBaseEngine;

	//��������
	IGameServiceManager * pIGameServiceManager=TableFrameParameter.pIGameServiceManager;
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);

	//�����ж�
	if (m_pITableFrameSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}
	
	//���ñ���
	m_lCellScore=m_pGameServiceOption->lCellScore;

	//��������
	IUnknownEx * pITableFrame=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->Initialization(pITableFrame)==false) return false;

	//���ñ�ʶ
	m_bTableInitFinish=true;

	//��չ�ӿ�
	m_pITableUserAction=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserAction);
	m_pITableUserRequest=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserRequest);

	//��ȡ����
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf(szIniFile,CountArray(szIniFile),TEXT("%s\\PersonalTable.ini"),szWorkDir);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	m_PlayerWaitTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("PlayerWaitTime"),120);
	m_TableOwnerWaitTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("TableOwnerWaitTime"),1800);
	m_GameStartOverTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("GameStartOverTime"),900);

	return true;
}

//��������
bool CTableFrame::PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative/*=false*/)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	ASSERT(pIServerUserItem->GetChairID()<=m_wChairCount);

	//�û�����
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	DWORD dwTimeNow = (DWORD)time(NULL);

	//�ж��Ƿ��ǽ�ҳ�
	bool bTreasureDB = false;
	if (lstrcmp(m_pGameServiceOption->szDataBaseName,  TEXT("RYTreasureDB")) == 0)
	{
		bTreasureDB = true;
	}

	//��Ϸ�û�
	if (
		(
		(m_bGameStarted==true)&&
		((cbUserStatus==US_PLAYING)||(cbUserStatus==US_OFFLINE)) &&
		m_pGameServiceOption->wServerType!=GAME_GENRE_PERSONAL
		) ||
		(
		((m_pGameServiceOption->lMinTableScore!=0L) && (pIServerUserItem->GetUserScore()>=m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore && bTreasureDB || !bTreasureDB)) &&
		(m_PersonalTableParameter.cbIsJoinGame || m_dwPersonalPlayCount != 0 ) && !m_bIsDissumGame &&//�������������Ϸ,�Ҳ���ǿ�ƽ�ɢ��Ϸ
		(m_pGameServiceOption->wServerType==GAME_GENRE_PERSONAL && (m_dwPersonalPlayCount != 0  || (m_dwTableOwnerID == pIServerUserItem->GetUserID() && ((m_dwDrawCountLimit!=0 && m_dwDrawCountLimit <= m_dwPersonalPlayCount) ||
		(m_dwDrawTimeLimit != 0 && m_dwDrawTimeLimit <= dwTimeNow - m_dwTimeBegin)) )))
		)
		)
	{	
		//�������ͣ�˽�˷�������
		if((m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)||
			m_pGameServiceOption->wServerType==GAME_GENRE_PERSONAL && (m_PersonalTableParameter.cbIsJoinGame ||   
			(m_dwPersonalPlayCount != 0  && (
			( 
			(m_dwDrawCountLimit!=0 && m_dwDrawCountLimit  >= m_dwPersonalPlayCount) ||
			(m_dwDrawTimeLimit != 0 && m_dwDrawTimeLimit >= dwTimeNow - m_dwTimeBegin)
			) 
			)
			)))
		{

			//�����й�
			pIServerUserItem->SetTrusteeUser(true);

			//����֪ͨ
			if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);

			return true;
		}
		if(m_pITableUserAction->OnActionUserPour(wChairID,pIServerUserItem))
		{
			//�û�����
			if(m_pITableUserAction!=NULL && m_pGameServiceOption->wKindID == 306) //
			{
				m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
			}
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);
			//�����й�
			pIServerUserItem->SetTrusteeUser(true);

			//-------------------------------//
			pIServerUserItem->SetClientReady(true);
			return true;
		}
		else
		{
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID); 
			//-------------------------------//
			pIServerUserItem->SetClientReady(true);
		}

		//������Ϸ
		BYTE cbConcludeReason=(cbUserStatus==US_OFFLINE)?GER_NETWORK_ERROR:GER_USER_LEAVE;
		m_pITableFrameSink->OnEventGameConclude(wChairID,pIServerUserItem,cbConcludeReason);

		//�뿪�ж�
		if (m_TableUserItemArray[wChairID]!=pIServerUserItem) return true;
	}

	//�������йܴ���
	if ((m_pGameServiceOption->wServerType==GAME_GENRE_MATCH) && (m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE))
	{
		//��ȡ״̬
		BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
		BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();

		//�ж�״̬
		if (bInitiative==true && cbUserMatchStatus==MUS_PLAYING)
		{
			if (pIServerUserItem->IsTrusteeUser()==false)
			{
				pIServerUserItem->SetTrusteeUser(true);

				//����֪ͨ
				if(m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(wChairID,pIServerUserItem);
			}	
			return true;
		}
	}

	//���ñ���
	if (pIServerUserItem==pITableUserItem)
	{
		//ɾ����ʱ
		if(m_pGameServiceAttrib->wChairCount < MAX_CHAIR) KillGameTimer(IDI_START_OVERTIME+wChairID);

		//���ñ���
		m_TableUserItemArray[wChairID]=NULL;

		//������Ϸ��
		if (m_lFrozenedScore[wChairID]!=0L)
		{
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//�¼�֪ͨ
		if (m_pITableUserAction!=NULL)
		{
			m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,false);
		}

		//�¼�֪ͨ
		if(m_pIMatchTableAction!=NULL && bInitiative==true)
		{
			m_pIMatchTableAction->OnActionUserStandUp(wChairID,pIServerUserItem,false);
		}

		//��������
		if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE )
		{
			//��ȡ״̬
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();

			if (cbUserStatus==US_OFFLINE && cbUserMatchStatus==MUS_PLAYING)
			{
				CString strMessage;
				strMessage.Format(TEXT("��ʱ���г������: %d����ʱ״̬ΪUS_OFFLINE�����"),pIServerUserItem->GetUserID());

				//��ʾ��Ϣ
				CTraceService::TraceString(strMessage,TraceLevel_Exception);
			}
		}

		//�û�״̬
		pIServerUserItem->SetClientReady(false);
		if (cbUserStatus == US_NULL)
		{
			if (pIServerUserItem->GetUserInfo()->dwUserID == 6346   || pIServerUserItem->GetUserInfo()->dwUserID == 6354)
			{
				printf(("��Ϸ��������й��˳�________________________________________%d\n"),pIServerUserItem->GetUserInfo()->dwUserID);
			}
			pIServerUserItem->SetUserStatus((cbUserStatus==US_NULL)?US_NULL:US_FREE,INVALID_TABLE,INVALID_CHAIR);

		}
		else
		{
			pIServerUserItem->SetUserStatus((cbUserStatus==US_OFFLINE)?US_NULL:US_FREE,INVALID_TABLE,INVALID_CHAIR);
		}


		//�˻�����
		if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
		{	
			pIServerUserItem->SetClientReady(false);
			if((((m_pGameServiceOption->lMinTableScore!=0L) && pIServerUserItem->GetUserScore()>=m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore) ) &&
				m_dwPersonalPlayCount != 0 && m_dwTableOwnerID == pIServerUserItem->GetUserID() && m_bPersonalLock == true )
			{
				//�˻�����
				KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
				OutputDebugStringA("ptdtcs ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 2");
				m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_PLAYER, m_wTableID, m_szPersonalTableID);
			}

			if (!m_bBeOut[wChairID] )
			{
				if(cbUserStatus == US_OFFLINE)
				{
					pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
				}
				else
				{
					//LogPrintf(("PerformStandUpAction �� ���� ��������Ϊ��"));
					pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);
				}
			}
		}

		//��������
		bool bTableLocked=IsTableLocked();
		bool bTableStarted=IsTableStarted();
		WORD wTableUserCount=GetSitUserCount();

		//���ñ���
		m_wUserCount=wTableUserCount;

		//������Ϣ
		if (wTableUserCount==0 && m_pGameServiceOption->wServerType!=GAME_GENRE_PERSONAL)
		{
			m_dwTableOwnerID=0L;
			m_szEnterPassword[0]=0;
		}

		if (wTableUserCount==0 && m_pGameServiceOption->wServerType==GAME_GENRE_PERSONAL &&  m_PersonalTableParameter.cbIsJoinGame)
		{
			m_dwTableOwnerID=0L;
			m_szEnterPassword[0]=0;
		}

		//�����Թ�
		if (wTableUserCount==0)
		{
			for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
			{
				SendGameMessage(m_LookonUserItemArray[i],TEXT("����Ϸ������������Ѿ��뿪�ˣ�"),SMT_CLOSE_GAME|SMT_EJECT);
			}
		}

		//��������
		ConcludeTable();

		//��ʼ�ж�
		if (m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH && EfficacyStartGame(INVALID_CHAIR)==true)
		{
			StartGame();
		}

		//����״̬
		if ((bTableLocked!=IsTableLocked())||(bTableStarted!=IsTableStarted()))
		{
			SendTableStatus();
		}

		//�����׷����ж�
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
			&&m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
		{
			//�����׷����ж�
			if(IsTableStarted()==false && CheckDistribute())
			{
				for(int i=0; i<m_wChairCount; i++)
				{
					if(m_TableUserItemArray[i] != NULL && m_TableUserItemArray[i]->GetUserStatus()==US_READY)
					{
						//������Ϣ
						LPCTSTR pszMessage=TEXT("������뿪��ϵͳ���·������ӣ����Ժ�");
						SendGameMessage(m_TableUserItemArray[i],pszMessage,SMT_CHAT);

						//������Ϣ
						m_pIMainServiceFrame->InsertDistribute(m_TableUserItemArray[i]);

						////�û�����
						//PerformStandUpAction(m_TableUserItemArray[i]);
					}
				}
			}
		}

		return true;
	}
	else
	{
		//��������
		for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		{
			if (pIServerUserItem==m_LookonUserItemArray[i])
			{
				//ɾ������
				m_LookonUserItemArray.RemoveAt(i);

				//�¼�֪ͨ
				if (m_pITableUserAction!=NULL)
				{
					m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);
				}

				//�¼�֪ͨ
				if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);

				//�û�״̬
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

				return true;
			}
		}

		//�������
		ASSERT(FALSE);
	}

	return true;
}

//�Թ۶���
bool CTableFrame::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//��������
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//��Ϸ״̬
	if ((m_bGameStarted==false)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ϸ��û�п�ʼ�������Թ۴���Ϸ����"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//ģ�⴦��
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser()==false)
	{
		//�������
		CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

		//���һ���
		for (WORD i=0; i<m_pGameServiceAttrib->wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIUserItem=m_TableUserItemArray[i];
			if(pIUserItem==NULL) continue;
			if(pIUserItem->IsAndroidUser()==false)break;

			//��ȡ����
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(),pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

			//ģ���ж�
			if((pAndroidParameter->dwServiceMode&ANDROID_SIMULATE)!=0
				&& (pAndroidParameter->dwServiceMode&ANDROID_PASSIVITY)==0
				&& (pAndroidParameter->dwServiceMode&ANDROID_INITIATIVE)==0)
			{
				SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ���ӽ�ֹ�û��Թۣ�"),REQUEST_FAILURE_NORMAL);
				return false;
			}

			break;
		}
	}


	//�Թ��ж�
	if (CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule)==true
		&& (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule)==false))
	{
		if ((pITableUserItem!=NULL)&&(pITableUserItem->IsAndroidUser()==true))
		{
			SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ�û��Թۣ�"),REQUEST_FAILURE_NORMAL);
			return false;
		}
	}

	//״̬�ж�
	if ((CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ�û��Թۣ�"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//�����ж�
	if ((pITableUserItem==NULL)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		SendRequestFailure(pIServerUserItem,TEXT("���������λ��û����Ϸ��ң��޷��Թ۴���Ϸ��"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//����Ч��
	if ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L)&&(lstrcmp(pUserRule->szPassword,m_szEnterPassword)!=0))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ϸ���������벻��ȷ�������Թ���Ϸ��"),REQUEST_FAILURE_PASSWORD);
		return false;
	}

	//��չЧ��
	if (m_pITableUserRequest!=NULL)
	{
		//��������
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//����Ч��
		if (m_pITableUserRequest->OnUserRequestLookon(wChairID,pIServerUserItem,RequestResult)==false)
		{
			//������Ϣ
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//�����û�
	m_LookonUserItemArray.Add(pIServerUserItem);

	//�û�״̬
	pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetUserStatus(US_LOOKON,m_wTableID,wChairID);

	//�¼�֪ͨ
	if (m_pITableUserAction!=NULL)m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,true);

	//�¼�֪ͨ
	if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserSitDown(wChairID,pIServerUserItem,true);

	return true;
}

//���¶���
bool CTableFrame::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword)
{
	
	//// ���˽�˷��Ѿ�����
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL  &&( m_dwTableOwnerID == 0 && m_dwPersonalPlayCount > 0   || m_bIsEnfoceDissum))
	{
		//������Ϣ
		SendRequestFailure(pIServerUserItem,TEXT("˽�˷����Ѿ���ɢ��"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//��������
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//���ֱ���
	SCORE lUserScore=pIServerUserItem->GetUserScore();
	SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	//if (m_pGameServiceOption->lCellScore < 1)
	//{
	//	m_pGameServiceOption->lCellScore  = 1;
	//}
	if (((m_pGameServiceOption->lMinTableScore!=0L) && pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore * m_lCellScore/m_pGameServiceOption->lCellScore) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
	{
		//������ʾ
		Personal_Room_Message personalRoomMessage;
		if (lstrcmp(m_pGameServiceOption->szDataBaseName, szTreasureDB) == 0)
		{
			_sntprintf(personalRoomMessage.szMessage,CountArray(personalRoomMessage.szMessage),TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"),m_pGameServiceOption->lMinTableScore* m_lCellScore/m_pGameServiceOption->lCellScore);
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_PERSONAL_TABLE,SUB_GF_PERSONAL_MESSAGE,&personalRoomMessage,sizeof(personalRoomMessage));
			return false;

		}

	}

	//�����˽�˷������Ѿ��������������ڻ�������õ����� ����������
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && GetSitUserCount() >= m_wChairCount)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ��������������"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//״̬�ж�
	if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ���ӽ�ֹ�û����룡"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//ģ�⴦��
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser()==false)
	{
		//�������
		CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

		//���һ���
		for (WORD i=0; i<m_pGameServiceAttrib->wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIUserItem=m_TableUserItemArray[i];
			if(pIUserItem==NULL) continue;
			if(pIUserItem->IsAndroidUser()==false)break;

			//��ȡ����
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(),pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

			//ģ���ж�
			if((pAndroidParameter->dwServiceMode&ANDROID_SIMULATE)!=0
				&& (pAndroidParameter->dwServiceMode&ANDROID_PASSIVITY)==0
				&& (pAndroidParameter->dwServiceMode&ANDROID_INITIATIVE)==0)
			{
				SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ���ӽ�ֹ�û����룡"),REQUEST_FAILURE_NORMAL);
				return false;
			}

			break;
		}
	}

	//��̬����
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	//��Ϸ״̬
	if ((m_bGameStarted==true)&&(bDynamicJoin==false))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ϸ�Ѿ���ʼ�ˣ����ڲ��ܽ�����Ϸ����"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//�����ж�
	if (pITableUserItem!=NULL)
	{
		//������
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return false;

		//������Ϣ
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("�����Ѿ��� [ %s ] �����ȵ��ˣ��´ζ���Ҫ����ˣ�"),pITableUserItem->GetNickName());

		//������Ϣ
		SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

		return false;
	}

	//����Ч��
	if (m_wChairCount < MAX_CHAIR)
	{
		//������
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule) == false)
		{
			if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			&&((lpszPassword==NULL)||(lstrcmp(lpszPassword,m_szEnterPassword)!=0)))
			{
				SendRequestFailure(pIServerUserItem,TEXT("��Ϸ���������벻��ȷ�����ܼ�����Ϸ��"),REQUEST_FAILURE_PASSWORD);
				return false;
			}
		}
	}

	//����Ч��
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyEnterTableScoreRule(wChairID,pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyIPAddress(pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyScoreRule(pIServerUserItem)==false) return false;

	//��չЧ��
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && m_pITableUserRequest!=NULL)
	{
		//��������
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//����Ч��
		if (m_pITableUserRequest->OnUserRequestSitDown(wChairID,pIServerUserItem,RequestResult)==false)
		{
			//������Ϣ
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//���ñ���
	m_TableUserItemArray[wChairID]=pIServerUserItem;
	m_wDrawCount=0;

	//У������
	if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
	{
		if(m_bPersonalLock == false)
		{
			//������Ϣ
			SendRequestFailure(pIServerUserItem,TEXT("���仹δ�������޷����룡"),REQUEST_FAILURE_NORMAL);
			return false;
		}

		pIServerUserItem->ModifyUserInsure(m_lIniScore,0,0);

	}

	//�û�״̬
	if ((IsGameStarted()==false)||(m_cbStartMode!=START_MODE_TIME_CONTROL))
	{
		if (CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)==false && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
		{
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,wChairID);
		}
		else
		{
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH && m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
			{
				pIServerUserItem->SetClientReady(pIServerUserItem->IsTrusteeUser());
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
			else
			{
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
		}
	}
	else
	{
		//���ñ���
		m_wOffLineCount[wChairID]=0L;
		m_dwOffLineTime[wChairID]=0L;

		//������Ϸ��
		if (m_pGameServiceOption->lServiceScore>0L)
		{
			m_lFrozenedScore[wChairID]=m_pGameServiceOption->lServiceScore;
			pIServerUserItem->FrozenedUserScore(m_pGameServiceOption->lServiceScore);
		}

		//����״̬
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,wChairID);
	}

	//���ñ���
	m_wUserCount=GetSitUserCount();

	//������Ϣ
	if (GetSitUserCount()==1)
	{
		//״̬����
		bool bTableLocked=IsTableLocked();

		//���ñ���
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
		{
			m_dwTableOwnerID=pIServerUserItem->GetUserID();
		}
		lstrcpyn(m_szEnterPassword,pUserRule->szPassword,CountArray(m_szEnterPassword));

		//˽������
		if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
		{
			m_bWaitConclude = false;
			m_dwPersonalPlayCount = 0;
			m_MapPersonalTableScoreInfo.RemoveAll();

			//��ʼ��ʱ
			SetGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME, m_PersonalTableParameter.dwTimeNotBeginGame * 60 * 1000,1,NULL);
		}

		//����״̬
		if (bTableLocked!=IsTableLocked()) SendTableStatus();
	}

	if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
	{
		//������ʱ
		if(IsGameStarted()==false && pIServerUserItem->IsMobileUser() && m_pGameServiceAttrib->wChairCount<MAX_CHAIR && m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL)
		{
			SetGameTimer(IDI_START_OVERTIME+wChairID,TIME_OVERTIME,1,wChairID);
		}
	}


	//�رմ�����ʼ����������
	KillGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME);

	m_bBeOut[wChairID]  = false;

	//�¼�֪ͨ
	if (m_pITableUserAction!=NULL)
	{
		if (m_wChairCount >= MAX_CHAIR)
		{
			if (m_bFirstRegister == false)
			{
				SetGameTimer(IDI_ANDROID_THEJOINT, TIME_ANDROID_THEJOINT,1,NULL);
			}
			m_bFirstRegister = true;
			m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);
		}
		else
		{
			m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);
		}

	}

	//�¼�֪ͨ
	if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);

	return true;
}

//��Ԫ����
VOID CTableFrame::SetCellScore(SCORE lCellScore)
{
	m_lCellScore=lCellScore;
	if (m_bTableInitFinish==true)
	{
		SendTableStatus();
		m_pITableFrameSink->SetGameBaseScore(lCellScore);
	}
}

//����״̬
bool CTableFrame::SendTableStatus()
{
	//��������
	CMD_GR_TableStatus TableStatus;
	ZeroMemory(&TableStatus,sizeof(TableStatus));

	//��������
	TableStatus.wTableID=m_wTableID;
	TableStatus.TableStatus.lCellScore=m_lCellScore;
	TableStatus.TableStatus.cbTableLock=IsTableLocked()?TRUE:FALSE;
	TableStatus.TableStatus.cbPlayStatus=IsTableStarted()?TRUE:FALSE;

	//��������
	m_pIMainServiceFrame->SendData(BG_COMPUTER,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	//�ֻ�����
	m_pIMainServiceFrame->SendData(BG_MOBILE,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	return true;
}

//����ʧ��
bool CTableFrame::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//��������
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);

	return true;
}

//��ʼЧ��
bool CTableFrame::EfficacyStartGame(WORD wReadyChairID)
{
	//״̬�ж�
	if (m_bGameStarted==true) return false;

	//ģʽ����
	if (m_cbStartMode==START_MODE_TIME_CONTROL) return false;
	if (m_cbStartMode==START_MODE_MASTER_CONTROL) return false;

	//׼������
	WORD wReadyUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//�û�ͳ��
		if (pITableUserItem!=NULL)
		{
			//״̬�ж�
			if (pITableUserItem->IsClientReady()==false) return false;
			if ((wReadyChairID!=i)&&(pITableUserItem->GetUserStatus()!=US_READY)) return false;

			//�û�����
			wReadyUserCount++;
		}
	}

	//��ʼ����
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:			//����׼��
		{
			//��Ŀ�ж�
			if (wReadyUserCount>=2L) return true;

			return false;
		}
	case START_MODE_FULL_READY:			//���˿�ʼ
		{
			//�����ж�
			if (wReadyUserCount==m_wChairCount) return true;

			return false;
		}
	case START_MODE_PAIR_READY:			//��Կ�ʼ
		{
			//��Ŀ�ж�
			if (wReadyUserCount==m_wChairCount) return true;
			if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

			//λ���ж�
			for (WORD i=0;i<m_wChairCount/2;i++)
			{
				//��ȡ�û�
				IServerUserItem * pICurrentUserItem=GetTableUserItem(i);
				IServerUserItem * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

				//λ�ù���
				if ((pICurrentUserItem==NULL)&&(pITowardsUserItem!=NULL)) return false;
				if ((pICurrentUserItem!=NULL)&&(pITowardsUserItem==NULL)) return false;
			}

			return true;
		}
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return false;
}

//��ַЧ��
bool CTableFrame::EfficacyIPAddress(IServerUserItem * pIServerUserItem)
{
	//����Ա��������
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//�����ж�
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return true;

	//���˲�����
	if(m_wChairCount >= MAX_CHAIR) return true;

	//��ַЧ��
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),*pTableUserRule=NULL;
	bool bCheckSameIP=pUserRule->bLimitSameIP;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
		{
			pTableUserRule=pITableUserItem->GetUserRule();
			if (pTableUserRule->bLimitSameIP==true) 
			{
				bCheckSameIP=true;
				break;
			}
		}
	}

	//��ַЧ��
	if (bCheckSameIP==true)
	{
		DWORD dwUserIP=pIServerUserItem->GetClientAddr();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if ((pITableUserItem!=NULL)&&(pITableUserItem != pIServerUserItem)&&(!pITableUserItem->IsAndroidUser())&&(pITableUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==dwUserIP))
			{
				if (!pUserRule->bLimitSameIP)
				{
					//������Ϣ
					LPCTSTR pszDescribe=TEXT("����Ϸ����������˲�����ͬ IP ��ַ�������Ϸ���� IP ��ַ�����ҵ� IP ��ַ��ͬ�����ܼ�����Ϸ��");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
				else
				{
					//������Ϣ
					LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ���������� IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
			}
		}
		for (WORD i=0;i<m_wChairCount-1;i++)
		{
			//��ȡ�û�
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
			{
				for (WORD j=i+1;j<m_wChairCount;j++)
				{
					//��ȡ�û�
					IServerUserItem * pITableNextUserItem=GetTableUserItem(j);
					if ((pITableNextUserItem!=NULL) && (!pITableNextUserItem->IsAndroidUser()) && (pITableNextUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==pITableNextUserItem->GetClientAddr()))
					{
						LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ������ IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
						SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
						return false;
					}
				}
			}
		}
	}
	return true;
}

//����Ч��
bool CTableFrame::EfficacyScoreRule(IServerUserItem * pIServerUserItem)
{
	//����Ա��������
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//�����ж�
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return true;

	//���˲�����
	if(m_wChairCount >= MAX_CHAIR) return true;

	//��������
	WORD wWinRate=pIServerUserItem->GetUserWinRate();
	WORD wFleeRate=pIServerUserItem->GetUserFleeRate();

	//���ַ�Χ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);

		//����Ч��
		if (pITableUserItem!=NULL)
		{
			//��ȡ����
			tagUserRule * pTableUserRule=pITableUserItem->GetUserRule();

			//����Ч��
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate))
			{
				//������Ϣ
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("����������̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

				//������Ϣ
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//ʤ��Ч��
			if ((pTableUserRule->bLimitWinRate)&&(wWinRate<pTableUserRule->wMinWinRate))
			{
				//������Ϣ
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("����ʤ��̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

				//������Ϣ
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//����Ч��
			if (pTableUserRule->bLimitGameScore==true)
			{
				//��߻���
				if (pIServerUserItem->GetUserScore()>pTableUserRule->lMaxGameScore)
				{
					//������Ϣ
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,CountArray(szDescribe),TEXT("���Ļ���̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

					//������Ϣ
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}

				//��ͻ���
				if (pIServerUserItem->GetUserScore()<pTableUserRule->lMinGameScore)
				{
					//������Ϣ
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,CountArray(szDescribe),TEXT("���Ļ���̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

					//������Ϣ
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}
			}
		}
	}

	return true;
}

//����Ч��
bool CTableFrame::EfficacyEnterTableScoreRule(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//���ֱ���
	SCORE lUserScore=pIServerUserItem->GetUserScore();
	SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);
	if (pIServerUserItem->IsAndroidUser()) return true;   //������ֱ��������
	if (((lMinTableScore!=0L)&&(lUserScore<lMinTableScore))||((lLessEnterScore!=0L)&&(lUserScore<lLessEnterScore)))
	{
		//������Ϣ
		TCHAR szDescribe[128]=TEXT("");
		if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" ����Ϸ�ң�������Ϸ�Ҳ��������ܼ��룡"),__max(lMinTableScore,lLessEnterScore));
		else if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" �ı����ң����ı����Ҳ��������ܼ��룡"),__max(lMinTableScore,lLessEnterScore));
		else
		{
			if (m_pGameServiceOption->wServerType != GAME_GENRE_PERSONAL)
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" ����Ϸ���֣����Ļ��ֲ��������ܼ��룡"),__max(lMinTableScore,lLessEnterScore));

			}
			else
			{
				return true;
			}
		}

		//������Ϣ
		SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NOSCORE);

		return false;
	}

	return true;
}


//������
bool CTableFrame::CheckDistribute()
{
	//������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule))
	{
		//����״��
		tagTableUserInfo TableUserInfo;
		WORD wUserSitCount=GetTableUserInfo(TableUserInfo);

		//�û�����
		if(wUserSitCount < TableUserInfo.wMinUserCount)
		{
			return true;
		}
	}

	return false;
}

//��Ϸ��¼
void CTableFrame::RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime)
{
	if (bDrawStarted==true)
	{
		//д���¼
		if (CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//��������
			DBR_GR_GameScoreRecord GameScoreRecord;
			ZeroMemory(&GameScoreRecord,sizeof(GameScoreRecord));

			//���ñ���
			GameScoreRecord.wTableID=m_wTableID;
			GameScoreRecord.dwPlayTimeCount=(bDrawStarted==true)?(DWORD)time(NULL)-m_dwDrawStartTime:0;

			//��Ϸʱ��
			if(dwStartGameTime!=INVALID_DWORD)
			{
				CTime startTime(dwStartGameTime);
				startTime.GetAsSystemTime(GameScoreRecord.SystemTimeStart);
			}
			else
			{
				GameScoreRecord.SystemTimeStart=m_SystemTimeStart;
			}
			GetLocalTime(&GameScoreRecord.SystemTimeConclude);

			//�û�����
			for (INT_PTR i=0;i<m_GameScoreRecordActive.GetCount();i++)
			{
				//��ȡ����
				ASSERT(m_GameScoreRecordActive[i]!=NULL);
				tagGameScoreRecord * pGameScoreRecord=m_GameScoreRecordActive[i];

				//�û���Ŀ
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.wUserCount++;
				}
				else
				{
					GameScoreRecord.wAndroidCount++;
				}

				//����ͳ��
				GameScoreRecord.dwUserMemal+=pGameScoreRecord->dwUserMemal;

				//ͳ����Ϣ
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.lWasteCount-=(pGameScoreRecord->lScore+pGameScoreRecord->lRevenue);
					GameScoreRecord.lRevenueCount+=pGameScoreRecord->lRevenue;
				}

				//�ɼ���Ϣ
				if (GameScoreRecord.wRecordCount<CountArray(GameScoreRecord.GameScoreRecord))
				{
					WORD wIndex=GameScoreRecord.wRecordCount++;
					CopyMemory(&GameScoreRecord.GameScoreRecord[wIndex],pGameScoreRecord,sizeof(tagGameScoreRecord));
				}
			}

			//Ͷ������
			if(GameScoreRecord.wUserCount > 0)
			{
				WORD wHeadSize=sizeof(GameScoreRecord)-sizeof(GameScoreRecord.GameScoreRecord);
				WORD wDataSize=sizeof(GameScoreRecord.GameScoreRecord[0])*GameScoreRecord.wRecordCount;
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_GAME_SCORE_RECORD,0,&GameScoreRecord,wHeadSize+wDataSize);
			}
		}

		//�����¼
		if (m_GameScoreRecordActive.GetCount()>0L)
		{
			m_GameScoreRecordBuffer.Append(m_GameScoreRecordActive);
			m_GameScoreRecordActive.RemoveAll();
		}
	}

}

//�����ɢ
bool CTableFrame::CancelTableRequest(DWORD dwUserID, WORD dwChairID)
{
	//У���û�
	IServerUserItem* pUserItem = GetTableUserItem(dwChairID);
	if(pUserItem == NULL) return false;
	if(pUserItem->GetUserID() != dwUserID) return false;

	if(m_wUserCount == 1 && pUserItem->GetUserID() == m_dwTableOwnerID)//m_PersonalTableParameter.cbIsJoinGame)
	{
		//�˻�����
		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		OutputDebugStringA("ptdtcs ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 3");
		m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_ENFOCE,  m_wTableID, m_szPersonalTableID);
		m_dwPersonalPlayCount = 0;

		return true;
	}
	m_dwReplyCount = 1;
	ZeroMemory(m_bRequestReply,sizeof(m_bRequestReply));
	m_bRequestReply[dwChairID] = true;

	return true;
}

//��ɢ��
bool CTableFrame::CancelTableRequestReply(DWORD dwUserID, BYTE cbAgree)
{
	//�����û�
	IServerUserItem* pUserItem = SearchUserItem(dwUserID);
	if(pUserItem == NULL) return false;

	//������
	WORD wChairID = pUserItem->GetChairID();
	if(cbAgree == 0)
	{
		m_bRequestReply[wChairID] = false;			//��ͬ��
	}
	else
	{
		m_bRequestReply[wChairID] = true;			//ͬ��
	}
	m_dwReplyCount++;

	//����ж�
	bool bCancel = false;
	WORD wOnlineCount = 0;
	WORD wAgreeCount = 0;
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pIServerUserItem = GetTableUserItem(i);
		if(pIServerUserItem == NULL) continue;
		if(pIServerUserItem->GetUserStatus() != US_OFFLINE) wOnlineCount++;
		//if(m_dwOffLineTime[i] == 0 && GetTableUserItem(i) != NULL) wOnlineCount++;
	}

	if(m_dwReplyCount == wOnlineCount)
	{
		for(int i = 0; i <m_wChairCount; ++i)
		{
			if(m_bRequestReply[i])
			{
				wAgreeCount++;
			}
		}
	}

	if(m_dwReplyCount < wOnlineCount) return true;
	if(wAgreeCount >= wOnlineCount || wAgreeCount >= (m_wChairCount/2 + 1)) bCancel = true;

	//������
	CMD_GR_RequestResult RequestResult;
	ZeroMemory(&RequestResult, sizeof(CMD_GR_RequestResult));
	RequestResult.dwTableID = m_wTableID;
	RequestResult.cbResult = bCancel?1:0;	//1 �� ��ɢ���� 0 �� ������Ϸ
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pTableUserItem = GetTableUserItem(i);
		if(pTableUserItem == NULL) continue;
		m_pIMainServiceFrame->SendData(pTableUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_REQUEST_RESULT, &RequestResult, sizeof(CMD_GR_RequestResult));

	}

	//��ɢ����
	if(bCancel == true)
	{
		m_bIsAllRequstCancel = true;
		m_bIsDissumGame = true;
		if(m_bGameStarted == true)
		{
			//��ɢ��Ϸ
			DismissGame();
		}

		KillGameTimer(IDI_PERSONAL_BEFORE_BEGAIN_TIME);
		OutputDebugStringA("ptdtcs ****  kill   IDI_PERSONAL_BEFORE_BEGAIN_TIME 4");

		TCHAR szInfo[260] = {0};	
		//�û��ɼ�
		CMD_GR_PersonalTableEnd PersonalTableEnd;
		ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));

		lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("��������ɢ����Ϸ��ǰ������"), CountArray(PersonalTableEnd.szDescribeString));
		for(int i = 0; i < m_wChairCount; ++i)
		{
			PersonalTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;
		}

		//���������Ϣ
		memcpy( PersonalTableEnd.cbSpecialInfo, m_cbSpecialInfo, m_nSpecialInfoLen);
		PersonalTableEnd.nSpecialInfoLen = m_nSpecialInfoLen;


		if(m_dwPersonalPlayCount == 0)		//��Ϸδ��ʼ
		{
			m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit, CANCELTABLE_REASON_ENFOCE, m_wTableID, m_szPersonalTableID);
		}
		else	//��Ϸ��ʼ
		{
			m_dwPersonalPlayCount = 0;
			WORD wDataSize = sizeof(CMD_GR_PersonalTableEnd) - sizeof(PersonalTableEnd.lScore) + sizeof(LONGLONG) * m_wChairCount;
			for(int i = 0; i < m_wChairCount; ++i)
			{
				IServerUserItem* pTableUserItem = GetTableUserItem(i);
				if(pTableUserItem == NULL) continue;

				m_pIMainServiceFrame->SendData(pTableUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));

				//	PerformStandUpAction(pTableUserItem);
			}

			//��ֹ�ͻ�����Ϊ״̬Ϊfree��ȡ�����û�ָ��
			for(int i = 0; i < m_wChairCount; ++i)
			{
				IServerUserItem* pTableUserItem = GetTableUserItem(i);
				if(pTableUserItem == NULL) continue;

				PerformStandUpAction(pTableUserItem);
			}
			m_pIMainServiceFrame->DismissPersonalTable(m_pGameServiceOption->wServerID, m_wTableID);
		}

		//��������
		m_pTableOwnerUser = NULL;
		//m_bPersonalLock = false;
		m_dwTimeBegin = 0;
		m_dwReplyCount = 0;
		ZeroMemory(m_bRequestReply,sizeof(m_bRequestReply));
		m_MapPersonalTableScoreInfo.RemoveAll();

	}

	return true;
}

//˽�˱��
void CTableFrame::SetPersonalTableID(LPCTSTR szTableID)
{
	if(szTableID == NULL)
	{
		ZeroMemory(m_szPersonalTableID, sizeof(m_szPersonalTableID));
	}
	else
	{
		lstrcpyn(m_szPersonalTableID, szTableID, CountArray(m_szPersonalTableID));
		m_bIsAllRequstCancel = false;
		memset(m_bBeOut, 0, sizeof(m_bBeOut));
		m_bIsDissumGame = false;
	}

	return;
}

//˽�˱��
TCHAR * CTableFrame::GetPersonalTableID()
{
	return m_szPersonalTableID;
}

//�������������ӵĸ���
VOID CTableFrame::SetTableChairCount(WORD wChairCount)
{
	m_wChairCount = wChairCount;
	m_pGameServiceAttrib->wChairCount = wChairCount;
	m_PersonalTableParameter.wJoinGamePeopleCount = wChairCount;
}

//��������
VOID CTableFrame::SetPersonalTable(DWORD dwDrawCountLimit, DWORD dwDrawTimeLimit, LONGLONG lIniScore)
{
	m_dwDrawCountLimit = dwDrawCountLimit;
	m_dwDrawTimeLimit = dwDrawTimeLimit;
	m_lIniScore = lIniScore;

	return;
}

//�������Ӳ���
VOID CTableFrame::SetPersonalTableParameter(tagPersonalTableParameter  PersonalTableParameter,  tagPersonalRoomOption PersonalRoomOption )
{
	m_PersonalTableParameter = PersonalTableParameter;
	m_dwDrawCountLimit = PersonalTableParameter.dwPlayTurnCount;
	m_dwDrawTimeLimit = PersonalTableParameter.dwPlayTimeLimit;
	m_PersonalRoomOption = PersonalRoomOption;

	m_dwPersonalPlayCount = 0; 
	m_bIsEnfoceDissum = false;
}

//��ȡ����
bool CTableFrame::GetPersonalScore(DWORD dwUserID, LONGLONG& lScore)
{
	LONGLONG lScoretmp = 0;
	lScore = 0;
	if(m_MapPersonalTableScoreInfo.Lookup(dwUserID, lScoretmp) == TRUE)
	{
		lScore = lScoretmp;
		return true;
	}
	return false;
}

//������Ϣ
tagPersonalTableParameter CTableFrame::GetPersonalTableParameter()
{
	tagPersonalTableParameter PersonalTableParameter;
	ZeroMemory(&PersonalTableParameter, sizeof(tagPersonalTableParameter));

	PersonalTableParameter.dwPlayTimeLimit = m_dwDrawTimeLimit;
	PersonalTableParameter.dwPlayTurnCount = m_dwDrawCountLimit;
	PersonalTableParameter.lIniScore = m_lIniScore;

	return PersonalTableParameter;
}

//���Ӵ�����೤ʱ��δ��ʼ��Ϸ ��ɢ����
VOID CTableFrame::SetTimerNotBeginAfterCreate()
{
	if (m_PersonalTableParameter.cbIsJoinGame)
	{
		return;
	}

	SetGameTimer(IDI_PERSONAL_AFTER_CREATE_ROOM_TIME, m_PersonalTableParameter.dwTimeAfterCreateRoom * 60 * 1000,1,NULL);
}

//����ǿ�ƽ�ɢ����
void CTableFrame::HostDissumeGame(BOOL bIsEnforce )
{
	m_bIsDissumGame = true;
	if(m_bGameStarted == true)
	{
		//��ɢ��Ϸ
		DismissGame();
	}

	//������Ϣ
	CMD_GR_PersonalTableEnd PersonalTableEnd;
	ZeroMemory(&PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));
	lstrcpyn(PersonalTableEnd.szDescribeString, TEXT("Լս������"), CountArray(PersonalTableEnd.szDescribeString));
	for(int i = 0; i < m_wChairCount; ++i)
	{
		//IServerUserItem* pUserItem = m_TableUserItemArray[i];
		//if(pUserItem == NULL) continue;
		//PersonalTableEnd.lScore[i] = pUserItem->GetUserScore();
		PersonalTableEnd.lScore[i] = m_PersonalUserScoreInfo[i].lScore;

	}

	//���������Ϣ
	memcpy( PersonalTableEnd.cbSpecialInfo, m_cbSpecialInfo, m_nSpecialInfoLen);
	PersonalTableEnd.nSpecialInfoLen = m_nSpecialInfoLen;
	TCHAR szInfo[260] = {0};


	WORD wDataSize = sizeof(CMD_GR_PersonalTableEnd) - sizeof(PersonalTableEnd.lScore) + sizeof(LONGLONG) * m_wChairCount;
	for(int i = 0; i < m_wChairCount; ++i)
	{
		IServerUserItem* pUserItem = m_TableUserItemArray[i];
		if(pUserItem == NULL ) 
		{
			continue;
		}

		//Լս���
		m_pIMainServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_PERSONAL_TABLE_END, &PersonalTableEnd, sizeof(CMD_GR_PersonalTableEnd));

		//��������
		CMD_GR_CancelTable CancelTable;
		ZeroMemory(&CancelTable, sizeof(CMD_GR_CancelTable));
		CancelTable.dwReason = CANCELTABLE_REASON_SYSTEM;
		if (bIsEnforce)
		{
			CancelTable.dwReason = CANCELTABLE_REASON_ENFOCE;
			lstrcpyn(CancelTable.szDescribeString, TEXT("����ǿ�ƽ�ɢ��Ϸ��"), CountArray(CancelTable.szDescribeString));
		}
		else
		{
			lstrcpyn(CancelTable.szDescribeString, TEXT("��Ϸ�Զ���ɢ��"), CountArray(CancelTable.szDescribeString));
		}


		//��ɢ��Ϣ
		m_pIMainServiceFrame->SendData(pUserItem, MDM_GR_PERSONAL_TABLE, SUB_GR_CANCEL_TABLE, &CancelTable, sizeof(CMD_GR_CancelTable));

		//�û�״̬
		//pUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
		//PerformStandUpAction(pUserItem);
	}

	if(!bIsEnforce)
	{
		if (m_dwPersonalPlayCount == 0 )
		{
			m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit,CANCELTABLE_REASON_ENFOCE ,  m_wTableID,  m_szPersonalTableID);//CANCELTABLE_REASON_ENFOCE
		}
		else
		{
			m_pIMainServiceFrame->CancelCreateTable(m_dwTableOwnerID, m_dwDrawCountLimit, m_dwDrawTimeLimit,CANCELTABLE_REASON_OVER_TIME ,  m_wTableID,  m_szPersonalTableID);
		}
	}

	m_pTableOwnerUser = NULL;
	m_dwTableOwnerID = 0;
	m_dwDrawCountLimit = 0;
	m_dwDrawTimeLimit = 0;
	m_dwTimeBegin = 0;
	m_dwReplyCount = 0;
	ZeroMemory(m_bRequestReply,sizeof(m_bRequestReply));
	m_MapPersonalTableScoreInfo.RemoveAll();
	m_dwPersonalPlayCount = 0;
	m_bIsEnfoceDissum = true;
}

//������Ϸ����
VOID CTableFrame::SetGameRule(byte * cbRule,  int nLen)
{
	memcpy(m_cbGanmeRule,  cbRule, sizeof(m_cbGanmeRule));
}

//��ȡ��Ϸ����
byte * CTableFrame::GetGameRule()
{
	return m_cbGanmeRule;
}

//��ȡ����ʱ��������Ϣ
void CTableFrame::GetSpeicalInfo(byte * cbSpecialInfo, int nSpecialLen)
{
	memcpy(m_cbSpecialInfo, cbSpecialInfo, nSpecialLen);
	m_nSpecialInfoLen = nSpecialLen;
}

DWORD CTableFrame::GetRecordTableOwner()
{
	TCHAR szInfo[260] = {0};
	wsprintf(szInfo, TEXT("ptdt *** GetRecordTableOwner = %d "),  m_dwRecordTableID);
	OutputDebugString(szInfo);
		return  m_dwRecordTableID; 
}

//˽�˷����Ƿ��ɢ
bool CTableFrame::IsPersonalRoomDisumme()
{
	return m_bIsDissumGame;
}

//�����ǽ�����ݿ⻹�ǻ������ݿ�,  0 Ϊ��ҿ� 1 Ϊ ���ֿ�
void CTableFrame::SetDataBaseMode(byte cbDataBaseMode)
{
		m_cbDataBaseMode = cbDataBaseMode;
}

//��ȡ���ݿ�ģʽ,  0 Ϊ��ҿ� 1 Ϊ ���ֿ�
byte CTableFrame::GetDataBaseMode()
{
	return m_cbDataBaseMode;
}
//////////////////////////////////////////////////////////////////////////////////
