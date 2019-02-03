#include "StdAfx.h"
#include "LockTimeMatch.h"
#include "..\��Ϸ������\DataBasePacket.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//����״̬
enum emMatchStatus
{
	MatchStatus_Free=0,
	MatchStatus_WaitPlay,
	MatchStatus_Playing,
	MatchStatus_WaitEnd,
	MatchStatus_End
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��������
#define INVALID_VALUE				0xFFFF								//��Чֵ

//ʱ�Ӷ���
#define IDI_SWITCH_STATUS			(IDI_MATCH_MODULE_START+1)					//�л�״̬
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+2)					//�����û�
#define IDI_CHECK_START_SIGNUP		(IDI_MATCH_MODULE_START+3)					//��ʼ����
#define IDI_CHECK_END_SIGNUP		(IDI_MATCH_MODULE_START+4)					//��ʼ��ֹ
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+5)					//��ʼʱ��
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+6)					//����ʱ��

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CLockTimeMatch::CLockTimeMatch()
{
	//״̬����
	m_MatchStatus=MatchStatus_Free;

	//��������
	m_pMatchOption=NULL;
	m_pLockTimeMatch=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//�ں˽ӿ�
	m_ppITableFrame=NULL;
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//����ӿ�
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;
}

CLockTimeMatch::~CLockTimeMatch(void)
{
	//�ͷ���Դ
	SafeDeleteArray(m_ppITableFrame);

	//�رն�ʱ��
	m_pITimerEngine->KillTimer(IDI_SWITCH_STATUS);
	m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
	m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);
	m_pITimerEngine->KillTimer(IDI_CHECK_START_SIGNUP);			

	//�Ƴ�Ԫ��
	m_DistributeManage.RemoveAll();
}

//�ӿڲ�ѯ
VOID* CLockTimeMatch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IGameMatchItem,Guid,dwQueryVer);
	QUERYINTERFACE(IMatchEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchItem,Guid,dwQueryVer);
	return NULL;
}

//����֪ͨ
void CLockTimeMatch::OnStartService()
{
	//���㳡��
	m_pMatchOption->lMatchNo=CalcMatchNo();

	//����״̬
	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
	{
		if (m_pMatchOption->cbMatchStatus==MS_FREE) m_MatchStatus=MatchStatus_Free;
		if (m_pMatchOption->cbMatchStatus==MS_MATCHEND) m_MatchStatus=MatchStatus_End;
		if (m_pMatchOption->cbMatchStatus==MS_MATCHING) m_MatchStatus=MatchStatus_Playing;	
	}

	//����״̬
	m_TimeLastMatch=CTime::GetCurrentTime();	

	//����״̬
	UpdateMatchStatus();

	//�л�ʱ��
	SwitchMatchTimer();		 

	//������ʼ
	if (IsMatchSignupStart()==false)
	{
		//��ʼ���	
		m_pITimerEngine->SetTimer(IDI_CHECK_START_SIGNUP,1000L,TIMES_INFINITY,NULL);
	}	

	//������ֹ
	if (IsMatchSignupEnd()==false && m_MatchStatus<MatchStatus_Playing)
	{
		m_pITimerEngine->SetTimer(IDI_CHECK_END_SIGNUP,1000L,TIMES_INFINITY,NULL);	
	}
}

//������
bool CLockTimeMatch::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	if(pTableFrame==NULL || wTableID>m_pGameServiceOption->wTableCount)
	{
		ASSERT(false);
		return false;
	}

	//��������
	CTableFrameHook * pTableFrameHook=new CTableFrameHook();
	pTableFrameHook->InitTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrame,IUnknownEx));
	pTableFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//���ýӿ�
	pTableFrame->SetTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrameHook,IUnknownEx));
	m_ppITableFrame[wTableID]=pTableFrame;

	return true;
}

//��ʼ���ӿ�
bool CLockTimeMatch::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
{
	//��������
	m_pMatchOption=MatchManagerParameter.pGameMatchOption;
	m_pGameServiceOption=MatchManagerParameter.pGameServiceOption;
	m_pGameServiceAttrib=MatchManagerParameter.pGameServiceAttrib;

	//�ں����
	m_pITimerEngine=MatchManagerParameter.pITimerEngine;
	m_pIDataBaseEngine=MatchManagerParameter.pICorrespondManager;
	m_pITCPNetworkEngineEvent=MatchManagerParameter.pTCPNetworkEngine;

	//�������		
	m_pIGameServiceFrame=MatchManagerParameter.pIMainServiceFrame;		
	m_pIServerUserManager=MatchManagerParameter.pIServerUserManager;
	m_pAndroidUserManager=MatchManagerParameter.pIAndroidUserManager;
	m_pIServerUserItemSink=MatchManagerParameter.pIServerUserItemSink;

	//��������
	m_DistributeManage.SetDistributeRule(m_pMatchOption->cbDistributeRule);

	//��������
	m_pLockTimeMatch=(tagLockTimeMatch *)m_pMatchOption->cbMatchRule;

	//�������
	if (m_pMatchOption->wDistributeTimeSpace==0)
	{
		m_pMatchOption->wDistributeTimeSpace=5;
	}

	//��������
	if (m_pMatchOption->wMaxPartakeGameUser<2) 
	{
		m_pMatchOption->wMaxPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//��������
	if (m_pMatchOption->wMaxPartakeGameUser>m_pGameServiceAttrib->wChairCount)
	{
		m_pMatchOption->wMaxPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//��������
	if (m_pMatchOption->wMinPartakeGameUser<2) 
	{
		m_pMatchOption->wMinPartakeGameUser=m_pGameServiceAttrib->wChairCount;
	}

	//��������
	if (m_pMatchOption->wMinPartakeGameUser>m_pMatchOption->wMaxPartakeGameUser)
	{
		m_pMatchOption->wMinPartakeGameUser=m_pMatchOption->wMaxPartakeGameUser;
	}

	//��������
	if (m_pMatchOption->wMinDistributeUser<m_pMatchOption->wMinPartakeGameUser)
	{
		m_pMatchOption->wMinDistributeUser=m_pMatchOption->wMinPartakeGameUser;
	}

	//��������
	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];
	}	

	return true;
}

//ʱ���¼�
bool CLockTimeMatch::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	switch(dwTimerID)
	{
	case IDI_DISTRIBUTE_USER:				//�����û�
		{
			//ִ�з���
			PerformDistribute();

			return true;
		}
	case IDI_SWITCH_STATUS:					//�л�״̬
		{
			//�ж�����
			if (m_MatchStatus==MatchStatus_End)
			{
				if (m_TimeLastMatch.GetDay()!=CTime::GetCurrentTime().GetDay())
				{
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
					{	
						//���㳡��
						m_pMatchOption->lMatchNo=CalcMatchNo();

						//���ñ���						
						m_MatchStatus=MatchStatus_Free;
						m_TimeLastMatch=CTime::GetCurrentTime();						

						//��ʼ���	
						m_pITimerEngine->SetTimer(IDI_CHECK_START_SIGNUP,1000L,TIMES_INFINITY,NULL);						

						//������ֹ
						m_pITimerEngine->SetTimer(IDI_CHECK_END_SIGNUP,1000L,TIMES_INFINITY,NULL);	

						//�л�ʱ��
						SwitchMatchTimer();		

						//�رն�ʱ��
						m_pITimerEngine->KillTimer(IDI_SWITCH_STATUS);

						//����״̬
						BYTE cbMatchStatus=MS_FREE;
						m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));	
					}					
					else
					{
						//��������
						CTime CurTime=CTime::GetCurrentTime();
						CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);

						//����ʱ��
						int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

						//������ʼ
						if (nCurrSeconds>=(MatchStartTime.GetHour()*3600+MatchStartTime.GetMinute()*60+MatchStartTime.GetSecond()))
						{
							//����״̬
							m_MatchStatus=MatchStatus_Playing;

							//�л�ʱ��
							SwitchMatchTimer();

							//�رն�ʱ��
							m_pITimerEngine->KillTimer(IDI_SWITCH_STATUS);							

							//����״̬
							BYTE cbMatchStatus=MS_MATCHING;
							m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));	
						}
					}
				}
			}
			
			return true;
		}
	case IDI_CHECK_START_SIGNUP:			//��ʼ����
		{
			//������ʼ
			if (IsMatchSignupStart()==true)
			{			
				//�رն�ʱ��
				m_pITimerEngine->KillTimer(IDI_CHECK_START_SIGNUP);

				//��������
				DBR_GR_SignupStart SignupStart;

				//����ṹ
				SignupStart.dwMatchID=m_pMatchOption->dwMatchID;
				SignupStart.lMatchNo=m_pMatchOption->lMatchNo;
				SignupStart.cbMatchType=m_pMatchOption->cbMatchType;
				SignupStart.wServerID=m_pGameServiceOption->wServerID;
				SignupStart.cbSignupMode=m_pMatchOption->cbSignupMode;

				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(SignupStart.dwMatchID,DBR_GR_MATCH_SIGNUP_START,0,&SignupStart,sizeof(SignupStart));
			}

			return true;
		}
	case IDI_CHECK_END_SIGNUP:				//������ֹ
		{
			if (IsMatchSignupEnd()==true)
			{			
				//�رն�ʱ��
				m_pITimerEngine->KillTimer(IDI_CHECK_END_SIGNUP);	

				//ȡ������
				if (m_MatchStatus==MatchStatus_WaitPlay)
				{
					//��������
					DBR_GR_MatchCancel MatchCancel;
					ZeroMemory(&MatchCancel,sizeof(MatchCancel));

					//��������
					MatchCancel.bMatchFinish=FALSE;
					MatchCancel.dwMatchID=m_pMatchOption->dwMatchID;
					MatchCancel.lMatchNo=m_pMatchOption->lMatchNo;
					MatchCancel.wServerID=m_pGameServiceOption->wServerID;
					MatchCancel.lSafeCardFee=m_pLockTimeMatch->lSafeCardFee;

					//���濪��
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
					{
						MatchCancel.bMatchFinish=TRUE;
					}

					//ѭ������
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
					{
						//��ȡʱ��
						CTime CurTime=CTime::GetCurrentTime();
						CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
						CTimeSpan TimeSpan=MatchEndTime-CurTime;

						//�ж�ʱ��
						if (TimeSpan.GetDays()==0) MatchCancel.bMatchFinish=TRUE;
					}					

					//Ͷ������
					m_pIDataBaseEngine->PostDataBaseRequest(MatchCancel.dwMatchID,DBR_GR_MATCH_CANCEL,0,&MatchCancel,sizeof(MatchCancel));
					
					return true;
				}
			}

			return true;
		}
	case IDI_CHECK_START_MATCH:				//��ʼ����
		{
			//��������
			BYTE cbMatchStatus=m_MatchStatus;

			//����״̬
			UpdateMatchStatus();

			//�ȴ�����
			if (m_MatchStatus==MatchStatus_WaitPlay)
			{
				//�л�ʱ��
				SwitchMatchTimer();	

				//�ж�״̬
				if (cbMatchStatus!=MatchStatus_WaitPlay)
				{
					//����״̬
					BYTE cbMatchStatus=MS_WAITPLAY;
					m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

					//��������
					CMD_GR_Match_Num MatchNum;
					MatchNum.dwWaitting=m_DistributeManage.GetCount();
					MatchNum.dwTotal=m_pLockTimeMatch->wStartUserCount;
					
					//��������
					m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
				}
			}

			//������ʼ
			if (m_MatchStatus==MatchStatus_Playing)
			{
				//�л�ʱ��
				SwitchMatchTimer();	

				//�رն�ʱ��
				m_pITimerEngine->KillTimer(IDI_CHECK_START_MATCH);

				//��������
				DBR_GR_MatchStart MatchStart;

				//����ṹ
				MatchStart.dwMatchID=m_pMatchOption->dwMatchID;
				MatchStart.lMatchNo=m_pMatchOption->lMatchNo;
				MatchStart.cbMatchType=m_pMatchOption->cbMatchType;
				MatchStart.wServerID=m_pGameServiceOption->wServerID;

				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(MatchStart.dwMatchID,DBR_GR_MATCH_START,0,&MatchStart,sizeof(MatchStart));

				//����״̬
				BYTE cbMatchStatus=MS_MATCHING;
				m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));	
			}

			return true;
		}
	case IDI_CHECK_END_MATCH:				//��������
		{
			//��������
			BYTE cbMatchStatus=m_MatchStatus;

			//����״̬
			UpdateMatchStatus();

			//��������
			if (m_MatchStatus==MatchStatus_WaitEnd)
			{
				if (cbMatchStatus!=MatchStatus_WaitEnd)
				{
					//����״̬
					BYTE cbMatchStatus=MS_WAITEND;
					m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));
					
					//��ȡʱ��
					CTime CurTime=CTime::GetCurrentTime();
					CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

					//��������
					bool bMatchOver=CurTime>=MatchEndTime;

					//��������					
					WORD wUserIndex=0;
					TCHAR szMessage[128];
					IServerUserItem *pIServerUserItem = NULL;

					do 
					{
						pIServerUserItem=m_pIServerUserManager->EnumUserItem(wUserIndex++);
						if (pIServerUserItem==NULL) break;

						//�������û�
						if (pIServerUserItem->GetUserStatus()==US_PLAYING)
						{							
							if (bMatchOver==false)
							{
								_sntprintf(szMessage,CountArray(szMessage),TEXT("����ı�������ʱ���ѵ����뾡���� %d �����ڽ���������Ϸ��"),m_pLockTimeMatch->wEndDelayTime/60);
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);
							}
							else
							{
								_sntprintf(szMessage,CountArray(szMessage),TEXT("������������ʱ���ѵ����뾡���� %d �����ڽ�����ǰ����������ϵͳ�����Զ���ɢ���غϲ�������ɼ���"),m_pLockTimeMatch->wEndDelayTime/60);
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);
							}
						}
						else
						{
							if (bMatchOver==false)
							{
								_sntprintf(szMessage,CountArray(szMessage),TEXT("����ı������˽������������������μӣ�"));
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);
							}
							else
							{
								//���ͽ�����Ϣ
								_sntprintf(szMessage,CountArray(szMessage),TEXT("������������ʱ���ѵ�����л���Ĳ��룻���� %d ���Ӻ�ǰ�����ҳ���ѯ���ı��������ɼ���"),m_pLockTimeMatch->wEndDelayTime/60);								
								m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);
							}
						}

					} while (true);
				}
			}

			//��������
			if (m_MatchStatus==MatchStatus_End)
			{
				//�رն�ʱ��
				m_pITimerEngine->KillTimer(IDI_CHECK_END_MATCH);
				m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_USER);

				//����״̬
				BYTE cbMatchStatus=MS_MATCHEND;
				m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

				//��ɢ��Ϸ
				for (INT i=0; i<m_pGameServiceOption->wTableCount; i++)
				{
					//��ȡ����
					ITableFrame * pITableFrame=m_ppITableFrame[i];					
					ASSERT(pITableFrame!=NULL);
					if (pITableFrame==NULL) continue;

					//��ɢ��Ϸ
					if (pITableFrame->IsGameStarted()) 
					{
						pITableFrame->DismissGame();
					}

					//ִ������
					PerformAllUserStandUp(pITableFrame);
				}

				//�Ƴ�����
				m_DistributeManage.RemoveAll();

				//��ȡʱ��
				CTime CurTime=CTime::GetCurrentTime();
				CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

				//��������
				if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND || CurTime>=MatchEndTime)
				{
					//��������
					DBR_GR_MatchOver MatchOver;
					ZeroMemory(&MatchOver,sizeof(MatchOver));

					//��������				
					MatchOver.cbMatchType=MATCH_TYPE_LOCKTIME;
					MatchOver.dwMatchID=m_pMatchOption->dwMatchID;
					MatchOver.lMatchNo=m_pMatchOption->lMatchNo;					
					MatchOver.wPlayCount=m_pLockTimeMatch->wMinPlayCount;
					MatchOver.wServerID=m_pGameServiceOption->wServerID;
					MatchOver.bMatchFinish=(CurTime>=MatchEndTime)?TRUE:FALSE;

					//���濪��
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
					{
						MatchOver.MatchEndTime=m_pLockTimeMatch->MatchEndTime;
						MatchOver.MatchStartTime=m_pLockTimeMatch->MatchStartTime;
					}	

					//ѭ������
					if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
					{
						//��������
						SYSTEMTIME SystemTimeCurr;
						CurTime.GetAsSystemTime(SystemTimeCurr);

						//����ʱ��
						SystemTimeCurr.wHour=m_pLockTimeMatch->MatchStartTime.wHour;
						SystemTimeCurr.wMinute=m_pLockTimeMatch->MatchStartTime.wMinute;
						SystemTimeCurr.wSecond=m_pLockTimeMatch->MatchStartTime.wSecond;

						//��ʼʱ��
						MatchOver.MatchStartTime=SystemTimeCurr;

                        //����ʱ��
						SystemTimeCurr.wHour=m_pLockTimeMatch->MatchEndTime.wHour;
						SystemTimeCurr.wMinute=m_pLockTimeMatch->MatchEndTime.wMinute;
						SystemTimeCurr.wSecond=m_pLockTimeMatch->MatchEndTime.wSecond;

						//����ʱ��
						MatchOver.MatchEndTime=SystemTimeCurr;						
					}									

					//Ͷ������
					m_pIDataBaseEngine->PostDataBaseRequest(MatchOver.dwMatchID,DBR_GR_MATCH_OVER,0,&MatchOver,sizeof(MatchOver));
					
					//�л�ʱ��
					if (CurTime<MatchEndTime) SwitchMatchTimer();					
						
					return true;
				}

				//���ͽ�����Ϣ
				TCHAR szMessage[128]=TEXT("����ı������˽������������������μӣ�");
				m_pIGameServiceFrame->SendGameMessage(szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);

				//�л�ʱ��
				SwitchMatchTimer();				
			}

			return true;
		}
	}
	return true;
}

//���ݿ��¼�
bool CLockTimeMatch::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)
{
	switch(wRequestID)
	{
	case DBO_GR_MATCH_QUERY_RESULT:			//��ѯ���
		{
			//����У��
			if (pIServerUserItem==NULL) return true;
			if (wDataSize!=sizeof(DBO_GR_MatchQueryResult)) return false;

			//��ȡ����
			DBO_GR_MatchQueryResult * pMatchQueryResult = (DBO_GR_MatchQueryResult*)pData;	

			//�߱��ʸ�
			if (pMatchQueryResult->bHoldQualify==true)
			{
				//������ʽ
				BYTE cbSignupMode=SIGNUP_MODE_MATCH_USER;

				//��������
				return OnEventSocketMatch(SUB_GR_MATCH_FEE,&cbSignupMode,sizeof(cbSignupMode),pIServerUserItem,dwContextID);
			}

			//�۳�����
			if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_SIGNUP_FEE)
			{
				//�۷���ʾ
				SendDeductFeeNotify(pIServerUserItem);
			}
			else
			{
				//��Ϣ֪ͨ
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("��Ǹ,�����߱����������ı����ʸ�,����ʧ�ܣ�"),SMT_EJECT);
			}

			return true;
		}
	case DBO_GR_MATCH_BUY_RESULT:			//������
		{
			//����У��
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchBuyResult)) return false;

			//��ȡ����
			DBO_GR_MatchBuyResult * pMatchBuyResult = (DBO_GR_MatchBuyResult*)pData;

			//����ʧ��
			if (pMatchBuyResult->bSuccessed==false)
			{
				//������Ϣ
				if (pMatchBuyResult->szDescribeString[0]!=0)
				{
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchBuyResult->szDescribeString,SMT_EJECT);
				}

				return true;
			}

			//��Ϣ֪ͨ
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("��ϲ�������տ�����ɹ�����ȴ�ϵͳΪ��������λ��"),SMT_CHAT);

			//�������
			InsertDistribute(pIServerUserItem);	

			//��Ҹ���
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchBuyResult->lCurrGold;

				//��������
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			return true;
		}		
	case DBO_GR_MATCH_REVIVE_INFO:			//������Ϣ
		{
			//����У��
			if (pIServerUserItem==NULL) return true;
			if (wDataSize!=sizeof(DBO_GR_MatchReviveInfo)) return false;

			//��ȡ����
			DBO_GR_MatchReviveInfo * pMatchReviveInfo = (DBO_GR_MatchReviveInfo*)pData;

			//��������
			bool bEliminate=false;

			//��̭�ж�
			if (!pMatchReviveInfo->bSuccessed || (!pMatchReviveInfo->bSafeCardRevive && pMatchReviveInfo->cbReviveTimesed >= m_pLockTimeMatch->cbReviveTimes))
			{
				bEliminate=true;
			}

			//��̭����
			if (bEliminate==true)
			{
				//��̭����
				SendEliminateNotify(pIServerUserItem);

				return true;
			}

			//��������
			SendReviveNotify(pIServerUserItem,pMatchReviveInfo->cbReviveTimesed,pMatchReviveInfo->bSafeCardRevive);

			//���տ�����
			if (pMatchReviveInfo->bSafeCardRevive==true)
			{
				//��Ҹ���
				OnEventSocketMatch(SUB_GR_MATCH_USER_REVIVE,NULL,0,pIServerUserItem,dwContextID);
			}

			return true;
		}
	case DBO_GR_MATCH_REVIVE_RESULT:		//������
		{
			//����У��
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchReviveResult)) return false;

			//��ȡ����
			DBO_GR_MatchReviveResult * pMatchReviveResult = (DBO_GR_MatchReviveResult*)pData;

			//����ʧ��
			if (pMatchReviveResult->bSuccessed==false)
			{
				//������Ϣ
				if (pMatchReviveResult->szDescribeString[0]!=0)
				{
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchReviveResult->szDescribeString,SMT_EJECT);
				}

				return true;
			}

			//���ͷ���
			SendMatchUserInitScore(pIServerUserItem);

			//����״̬
			pIServerUserItem->SetUserMatchStatus(MUS_SIGNUPED);

			//����״̬		
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

			//��Ϣ֪ͨ
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("��ϲ��������ɹ�����ȴ�ϵͳΪ��������λ��"),SMT_CHAT|SMT_EJECT);

			//�������
			InsertDistribute(pIServerUserItem);	

			//��Ҹ���
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchReviveResult->lCurrGold;

				//��������
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			return true;
		}
	case DBO_GR_MATCH_SIGNUP_RESULT:		//�������
		{
			//����Ч��
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//��ȡ����
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;			

			//����ʧ��
			if (pMatchSignupResult->bResultCode==false)
			{
				//��ʾ��Ϣ��Ϊ��
				if (pMatchSignupResult->szDescribeString[0]!=0)
				{
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);
				}

				return true;
			}

			//���ͷ���
			SendMatchUserInitScore(pIServerUserItem);

			//����״̬
			pIServerUserItem->SetUserMatchStatus(MUS_SIGNUPED);

			//����״̬		
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));		

			//ʹ�ñ��տ�
			if (m_pLockTimeMatch->cbReviveEnabled && m_pLockTimeMatch->cbSafeCardEnabled && pIServerUserItem->IsAndroidUser()==false)
			{
				SendSafeCardNotify(pIServerUserItem);
			}
			else
			{
				SendSignupSuccessNotify(pIServerUserItem);				
			}

			//��Ҹ���
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//��������
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_UNSIGNUP_RESULT:		//�������
		{
			//����Ч��
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//��ȡ����
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;

			//ϵͳȡ��
			if (pMatchSignupResult->dwReason==UNSIGNUP_REASON_SYSTEM) return true;

			//�˷�ʧ��
			if(pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			//��ȡ�ɼ�
			tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();

			//���óɼ�
			pUserScore->dwWinCount=0L;
			pUserScore->dwLostCount=0L;
			pUserScore->dwFleeCount=0L;
			pUserScore->dwDrawCount=0L;
			pUserScore->lIntegralCount=0L;
			pUserScore->lScore-=m_pLockTimeMatch->lMatchInitScore;

			//���ͷ���
			SendMatchUserScore(pIServerUserItem);

			//����״̬
			pIServerUserItem->SetUserMatchStatus(MUS_NULL);

			//����״̬		
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

			//��Ҹ���
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//��������
				return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_RANK_LIST:			//��������
		{
			//����У��
			ASSERT(wDataSize<=sizeof(DBO_GR_MatchRankList));
			if(wDataSize>sizeof(DBO_GR_MatchRankList)) return false;

			//��ȡ����
			DBO_GR_MatchRankList * pMatchRankList = (DBO_GR_MatchRankList*)pData;

			//��������
			bool bRewardUser=true;
			tagMatchRankInfo * pMatchRankInfo=NULL;
			IServerUserItem * pIRewardUserItem=NULL;

			//���Ž���
			for(WORD wIndex=0;wIndex<pMatchRankList->wUserCount;wIndex++)
			{
				//�������
				pMatchRankInfo=&pMatchRankList->MatchRankInfo[wIndex];
				pIRewardUserItem=m_pIServerUserManager->SearchUserItem(pMatchRankInfo->dwUserID);
				if (pIRewardUserItem!=NULL && pIRewardUserItem->IsAndroidUser()==true) continue;

				//д�뽱��
				WriteUserAward(pIRewardUserItem,pMatchRankInfo);

				//���ñ�ʶ
				if(pIRewardUserItem) pIRewardUserItem->SetMatchData(&bRewardUser);
			}

			//�����û�
			WORD wUserIndex=0;			
			while (true)
			{
				pIRewardUserItem=m_pIServerUserManager->EnumUserItem(wUserIndex++);
				if (pIRewardUserItem==NULL) break;
				if (pIRewardUserItem->IsAndroidUser()==true) continue;
				if (pIRewardUserItem->GetMatchData()==&bRewardUser)
				{
					//���ò���
					pIRewardUserItem->SetMatchData(NULL);

					//������Ϣ
					//TCHAR szMessage[256]=TEXT"\0";
					//_sntprintf(szMessage,CountArray(szMessage),TEXT"����鿴�������У�http://service.foxuc.com/GameMatch.aspx?PlazaStationID=%d&TypeID=2&MatchID=%d"),
					//pIRewardUserItem->GetPlazaStationID(),m_pMatchOption->dwMatchID);

					////������Ϣ
					//m_pIGameServiceFrame->SendGameMessage(pIRewardUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);	

					continue;
				}

				//���ͽ�����Ϣ
				TCHAR szMessage[128]=TEXT("���α������˽���������ǰ����վ�鿴ϵͳ�������Ƽ���������������������");
				m_pIGameServiceFrame->SendGameMessage(pIRewardUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME);	
			}			

			//�Ͽ�Э��
			if (m_pIGameServiceFrame!=NULL && pMatchRankList->bMatchFinish) 
			{
				m_pIGameServiceFrame->DisconnectCorrespond();
			}

			break;
		}
	case DBO_GR_MATCH_REWARD_RESULT:		//�������
		{
			//����Ч��
			ASSERT(wDataSize==sizeof(DBO_GR_MatchRewardResult));
			if(wDataSize!=sizeof(DBO_GR_MatchRewardResult)) return false;

			//��ȡ����
			DBO_GR_MatchRewardResult * pMatchRewardResult = (DBO_GR_MatchRewardResult*)pData;

			//�������
			IServerUserItem *  pIRewardUserItem=m_pIServerUserManager->SearchUserItem(pMatchRewardResult->dwUserID);
			if (pIRewardUserItem==NULL) return true;

			//�����ɹ�
			if(pMatchRewardResult->bResultCode==true)
			{
				//�����û���Ϣ
				tagUserInfo * pUserInfo=pIRewardUserItem->GetUserInfo();

				//���ͷ���
				SendMatchUserScore(pIRewardUserItem);

				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchRewardResult->lCurrGold;

				//��������
				m_pIGameServiceFrame->SendData(pIRewardUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_CANCEL_RESULT:		//ȡ�����
		{
			//����Ч��
			ASSERT(wDataSize==sizeof(DBO_GR_MatchCancelResult));
			if(wDataSize!=sizeof(DBO_GR_MatchCancelResult)) return false;

			//��ȡ����
			DBO_GR_MatchCancelResult * pMatchCancelResult = (DBO_GR_MatchCancelResult*)pData;

			//ȡ���ɹ�
			if (pMatchCancelResult->bSuccessed==true)
			{
				//���ͽ�����Ϣ
				TCHAR szMessage[128]=TEXT("�����������㣬ȡ�������������ѱ���������Ժ󽫻��յ�ϵͳ�˻��ı������ã���ע����ա�");
				m_pIGameServiceFrame->SendRoomMessage(szMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_GAME|SMT_CLOSE_ROOM);	
			}

			//��ȡʱ��
			CTime CurTime=CTime::GetCurrentTime();
			CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

			//�Ͽ�Э��
			if (m_pIGameServiceFrame!=NULL && pMatchCancelResult->bMatchFinish) 
			{
				m_pIGameServiceFrame->DisconnectCorrespond(); 
			}

			//�Ƴ�����
			m_DistributeManage.RemoveAll();

			break;
		}
	}
	return true;
}

//�����¼�
bool CLockTimeMatch::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_MATCH_FEE:	//��������
		{
			//����Ч��
			ASSERT(wDataSize==sizeof(BYTE));
			if(wDataSize!=sizeof(BYTE)) return false;

			//��ȡ����
			BYTE cbSignupMode=*(BYTE*)pData;
			if ((cbSignupMode&m_pMatchOption->cbSignupMode)==0)return false;

			//��������
			DBR_GR_MatchSignup MatchSignup;
			ZeroMemory(&MatchSignup,sizeof(MatchSignup));

			//����ṹ
			MatchSignup.cbSignupMode=cbSignupMode;			
			MatchSignup.lMatchNo=m_pMatchOption->lMatchNo;
			MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
			MatchSignup.dwUserID=pIServerUserItem->GetUserID();			
			MatchSignup.lInitScore=m_pLockTimeMatch->lMatchInitScore;			
			MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
			MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();			
			MatchSignup.wMaxSignupUser=m_pLockTimeMatch->wMaxSignupUser;
            
			//����������
			lstrcpyn(MatchSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchSignup.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID,DBR_GR_MATCH_SIGNUP,dwSocketID,&MatchSignup,sizeof(MatchSignup));

			return true;
		}
	case SUB_GR_LEAVE_MATCH:	//�˳�����
		{
			//����Ч��
			ASSERT(pIServerUserItem!=NULL);
			if(pIServerUserItem==NULL) return false;

			OnEventUserQuitMatch(pIServerUserItem,UNSIGNUP_REASON_PLAYER,0,dwSocketID);

			return true;
		}
	case SUB_GR_MATCH_BUY_SAFECARD:	//������
		{
			//����Ч��
			ASSERT(wDataSize==sizeof(CMD_GR_Match_BuySafeCard));
			if(wDataSize!=sizeof(CMD_GR_Match_BuySafeCard)) return false;

			//��ȡ����
			CMD_GR_Match_BuySafeCard * pMatchBuySafeCard =(CMD_GR_Match_BuySafeCard *)pData;

			//������
			if (pMatchBuySafeCard->bBuySafeCard)
			{
				//����ṹ
				DBR_GR_MatchBuySafeCard MatchBuySafeCard;

				//���ñ���
				MatchBuySafeCard.dwUserID=pIServerUserItem->GetUserID();
				MatchBuySafeCard.dwMatchID=m_pMatchOption->dwMatchID;
				MatchBuySafeCard.lMatchNo=m_pMatchOption->lMatchNo;
				MatchBuySafeCard.lSafeCardFee=m_pLockTimeMatch->lSafeCardFee;

				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(MatchBuySafeCard.dwUserID,DBR_GR_MATCH_BUY_SAFECARD,dwSocketID,&MatchBuySafeCard,sizeof(MatchBuySafeCard));
			}
			else
			{
				//��Ϣ֪ͨ
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("ϵͳ����Ŭ��Ϊ��������λ����ȴ���"),SMT_CHAT);

				//�������
				InsertDistribute(pIServerUserItem);
			}

			return true;
		}
	case SUB_GR_MATCH_USER_REVIVE:	//��Ҹ���
		{
			//����Ч��
			ASSERT(pIServerUserItem!=NULL);
			if(pIServerUserItem==NULL) return false;

			//�����ж�
			if (m_pLockTimeMatch->lMatchCullScore>0 && pIServerUserItem->GetUserScore()>m_pLockTimeMatch->lMatchCullScore)
			{
				//��Ϣ֪ͨ
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("���ķ���δ�ﵽ��̭�����ߣ�����Ҫ���"),SMT_CHAT);

				return true;
			}			

			//����ṹ
			DBR_GR_MatchUserRevive MatchUserRevive;

			//������Ϣ			
			MatchUserRevive.dwMatchID=m_pMatchOption->dwMatchID;
			MatchUserRevive.lMatchNo=m_pMatchOption->lMatchNo;
			MatchUserRevive.dwUserID=pIServerUserItem->GetUserID();

			//��������						
			MatchUserRevive.lReviveFee=m_pLockTimeMatch->lReviveFee;
			MatchUserRevive.lInitScore=m_pLockTimeMatch->lMatchInitScore;
			MatchUserRevive.lCullScore=m_pLockTimeMatch->lMatchCullScore;						
			MatchUserRevive.cbReviveTimes=m_pLockTimeMatch->cbReviveTimes;			

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(MatchUserRevive.dwUserID,DBR_GR_MATCH_USER_REVIVE,dwSocketID,&MatchUserRevive,sizeof(MatchUserRevive));

			return true;
		}
	}

	return true;
}

//�û�����
bool CLockTimeMatch::OnEventUserItemScore(IServerUserItem * pIServerUserItem,BYTE cbReason)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//��������
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	UserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//�������
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.dBeans=pUserInfo->dBeans;

	//�������
	UserScore.UserScore.lScore=pUserInfo->lScore;
	UserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();

	//��������
	m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	//��������
	CMD_GR_MobileUserScore MobileUserScore;
	ZeroMemory(&MobileUserScore,sizeof(MobileUserScore));

	//��������
	MobileUserScore.dwUserID=pUserInfo->dwUserID;
	MobileUserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	MobileUserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	MobileUserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	MobileUserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;
	MobileUserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	MobileUserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//�������
	MobileUserScore.UserScore.lScore=pUserInfo->lScore;
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();
	MobileUserScore.UserScore.dBeans=pUserInfo->dBeans;

	//��������
	m_pIGameServiceFrame->SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_GR_USER,SUB_GR_USER_SCORE,&MobileUserScore,sizeof(MobileUserScore));

	//��ʱд��
	if ((CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->IsVariation()==true))
	{
		//��������
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//�û���Ϣ
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//��ȡ����
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//������Ϣ
		WriteGameScore.dwMatchID=m_pMatchOption->dwMatchID;
		WriteGameScore.lMatchNo=m_pMatchOption->lMatchNo;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore),TRUE);
	}

	return true;
}

//�û�����
bool CLockTimeMatch::OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemGameData(pIServerUserItem,cbReason);
	}

	return true;
}

//�û�״̬
bool CLockTimeMatch::OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)
{
	//�������
	if(pIServerUserItem->GetUserStatus()==US_NULL) pIServerUserItem->SetMatchData(NULL);

	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wLastTableID,wLastChairID);
	}

	return true;
}

//�û�Ȩ��
bool CLockTimeMatch::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,cbRightKind);
	}

	return true;
}

//�û���¼
bool CLockTimeMatch::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	//����ṹ
	CMD_GR_Match_Rule MatchRule;

	//������Ϣ
	MatchRule.dwMatchID=m_pMatchOption->dwMatchID;
	MatchRule.cbMatchType=m_pMatchOption->cbMatchType;
	lstrcpyn(MatchRule.szMatchName,m_pMatchOption->szMatchName,CountArray(MatchRule.szMatchName));	

	//������Ϣ
	MatchRule.cbFeeType=m_pMatchOption->cbFeeType;
	MatchRule.lSignupFee=m_pMatchOption->lSignupFee;	
	MatchRule.dwExperience=m_pMatchOption->dwExperience;	
	MatchRule.cbDeductArea=m_pMatchOption->cbDeductArea;
	MatchRule.cbSignupMode=m_pMatchOption->cbSignupMode;
	MatchRule.cbMemberOrder=m_pMatchOption->cbMemberOrder;
	MatchRule.cbJoinCondition=m_pMatchOption->cbJoinCondition;	

	//������ʽ
	MatchRule.cbRankingMode=m_pMatchOption->cbRankingMode;
	MatchRule.wCountInnings=m_pMatchOption->wCountInnings;
	MatchRule.cbFilterGradesMode=m_pMatchOption->cbFilterGradesMode;

	//��������
	CopyMemory(MatchRule.cbMatchRule,m_pMatchOption->cbMatchRule,sizeof(MatchRule.cbMatchRule));

	//��������
	MatchRule.wRewardCount=__min(m_pMatchOption->wRewardCount,CountArray(MatchRule.MatchRewardInfo));
	CopyMemory(MatchRule.MatchRewardInfo,m_pMatchOption->MatchRewardInfo,MatchRule.wRewardCount* sizeof(MatchRule.MatchRewardInfo[0]));

	//�����С
	WORD wDataSize=sizeof(MatchRule)-sizeof(MatchRule.MatchRewardInfo);
	wDataSize+=MatchRule.wRewardCount*sizeof(MatchRule.MatchRewardInfo[0]);

	//��������
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_RULE,&MatchRule,wDataSize);

	return true;
}

//�û��ǳ�
bool CLockTimeMatch::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	RemoveDistribute(pIServerUserItem);

	return true;
}

//��¼���
bool CLockTimeMatch::OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)
{
	//����״̬
	if (m_MatchStatus>=MatchStatus_Free && pIServerUserItem->IsAndroidUser()==false)
	{
		//����״̬		
		BYTE cbMatchStatus=MS_FREE;		
		if (m_MatchStatus==MatchStatus_End) cbMatchStatus=MS_MATCHEND;
		if (m_MatchStatus==MatchStatus_WaitEnd) cbMatchStatus=MS_WAITEND;
		if (m_MatchStatus==MatchStatus_Playing) cbMatchStatus=MS_MATCHING;		
		if (m_MatchStatus==MatchStatus_WaitPlay) cbMatchStatus=MS_WAITPLAY;
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus));

		//��̭�ж�
		if (pIServerUserItem->GetUserMatchStatus()==MUS_SIGNUPED)
		{
			//��̭�ж�
			if (m_pLockTimeMatch->lMatchCullScore>0 && pIServerUserItem->GetUserScore()<m_pLockTimeMatch->lMatchCullScore)
			{
				pIServerUserItem->SetUserMatchStatus(MUS_OUT);
			}
		}		

		//����״̬		
		BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));		

		//��������ʱ
		if (m_MatchStatus==MatchStatus_WaitPlay)
		{
			//�������
			if (cbUserMatchStatus==MUS_SIGNUPED) InsertDistribute(pIServerUserItem);

			//��������
			CMD_GR_Match_Num MatchNum;
			MatchNum.dwWaitting=m_DistributeManage.GetCount();
			MatchNum.dwTotal=m_pLockTimeMatch->wStartUserCount;
			
			//��������
			m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
		}
	}
 
	return true;
}

//�����¼�
bool CLockTimeMatch::OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	//�ж�ʱ��
	TCHAR szDescribe[256];
	WORD wMessageType;
	if (VerifyMatchTime(szDescribe,CountArray(szDescribe),wMessageType)==false)
	{
		m_pIGameServiceFrame->SendRoomMessage(dwSocketID,szDescribe,wMessageType|SMT_GLOBAL,dwUserIP==0);
		return true;
	}

	//�ֻ��û�
	if (bIsMobile == true)
	{
		//������Ϣ
		CMD_GR_LogonMobile * pLogonMobile=(CMD_GR_LogonMobile *)pData;
		pLogonMobile->szPassword[CountArray(pLogonMobile->szPassword)-1]=0;
		pLogonMobile->szMachineID[CountArray(pLogonMobile->szMachineID)-1]=0;

		//��������
		DBR_GR_LogonMobile LogonMobile;
		ZeroMemory(&LogonMobile,sizeof(LogonMobile));

		//��������
		LogonMobile.dwUserID=pLogonMobile->dwUserID;
		LogonMobile.dwClientAddr=dwUserIP;		
		LogonMobile.dwMatchID=m_pMatchOption->dwMatchID;	
		LogonMobile.lMatchNo=m_pMatchOption->lMatchNo;
		LogonMobile.cbMatchType=m_pMatchOption->cbMatchType;
		LogonMobile.cbDeviceType=pLogonMobile->cbDeviceType;
		LogonMobile.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
		LogonMobile.wPageTableCount=pLogonMobile->wPageTableCount;		
		lstrcpyn(LogonMobile.szPassword,pLogonMobile->szPassword,CountArray(LogonMobile.szPassword));
		lstrcpyn(LogonMobile.szMachineID,pLogonMobile->szMachineID,CountArray(LogonMobile.szMachineID));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(LogonMobile.dwUserID,DBR_GR_LOGON_MOBILE,dwSocketID,&LogonMobile,sizeof(LogonMobile));
	}
	else
	{
		//������Ϣ
		CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
		pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
		pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

		//��������
		DBR_GR_LogonUserID LogonUserID;
		ZeroMemory(&LogonUserID,sizeof(LogonUserID));

		//��������
		LogonUserID.dwClientAddr=dwUserIP;
		LogonUserID.dwUserID=pLogonUserID->dwUserID;
		LogonUserID.dwMatchID=m_pMatchOption->dwMatchID;	
		LogonUserID.lMatchNo=m_pMatchOption->lMatchNo;	
		LogonUserID.cbMatchType=m_pMatchOption->cbMatchType;
		lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
		lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));
	}

	return true;
}

//�û�����
bool CLockTimeMatch::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	//����У��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;

	//�ж�״̬
	if(pIServerUserItem->GetUserStatus()!=US_FREE || pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		return false;
	}

	//����δ��ʼ
	if (IsMatchSignupStart()==false)
	{
		//������Ϣ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("������δ��ʼ�����Ժ�������"),SMT_EJECT);

		return true;
	}

	//�����ѽ�ֹ
	if (IsMatchSignupEnd()==true && pIServerUserItem->GetUserMatchStatus()==MUS_NULL)
	{
		//������Ϣ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("�����Ѿ���ֹ�������Ƽ�����������������������"),SMT_EJECT);

		return true;
	}

	//��Ա�ȼ�
	if (m_pMatchOption->cbJoinCondition&MATCH_JOINCD_MEMBER_ORDER)
	{
		if (m_pMatchOption->cbMemberOrder>pIServerUserItem->GetMemberOrder())
		{
			//������Ϣ
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("��Ǹ�����Ļ�Ա�ȼ��ﲻ����������������ʧ�ܣ�"),SMT_EJECT);

			return true;
		}
	}

	//����ȼ�
	if (m_pMatchOption->cbJoinCondition&MATCH_JOINCD_EXPERIENCE)
	{
		if (m_pMatchOption->dwExperience>pIServerUserItem->GetUserInfo()->dwExperience)
		{
			//������Ϣ
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("��Ǹ�����ľ���ֵ�ﲻ����������������ʧ�ܣ�"),SMT_EJECT);

			return true;
		}
	}

	//�ж�״̬
	if (pIServerUserItem->GetUserMatchStatus()!=MUS_SIGNUPED)
	{
		//�����û�
		if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
		{
			if (pIServerUserItem->IsAndroidUser()==false)
			{
				//����ṹ
				DBR_GR_QueryMatchQualify QueryMatchQualify;

				//���ñ���
				QueryMatchQualify.lMatchNo=m_pMatchOption->lMatchNo;
				QueryMatchQualify.dwMatchID=m_pMatchOption->dwMatchID;				
				QueryMatchQualify.dwUserID=pIServerUserItem->GetUserID();

				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(QueryMatchQualify.dwUserID,DBR_GR_MATCH_QUERY_QUALIFY,dwSocketID,&QueryMatchQualify,sizeof(QueryMatchQualify));

				return true;
			}
		}

		//�۳�������
		if (m_pMatchOption->cbSignupMode&SIGNUP_MODE_SIGNUP_FEE)
		{			
			//���˿۱�����
			if(m_pMatchOption->lSignupFee>0 && pIServerUserItem->IsAndroidUser()==false)
			{
				SendDeductFeeNotify(pIServerUserItem);

				return true;
			}			
		}		

		//�Զ�����
		if (m_pMatchOption->lSignupFee==0 || pIServerUserItem->IsAndroidUser()==true)
		{
			//������ʽ
			BYTE cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;

			return OnEventSocketMatch(SUB_GR_MATCH_FEE,&cbSignupMode,sizeof(cbSignupMode),pIServerUserItem,dwSocketID);
		}
	}

	//�������
	if (InsertDistribute(pIServerUserItem)==false) return false;
	
	//���ͳɼ�
	SendMatchUserScore(pIServerUserItem);

	//������Ϣ
	m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("ϵͳ����Ŭ��Ϊ��������λ�����Ժ󡣡���"),SMT_CHAT);

	return true;
}

//�û�����
bool CLockTimeMatch::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�жϱ���״̬
	if (pIServerUserItem->GetUserMatchStatus()==MUS_SIGNUPED)
	{
		return true;
	}

	//������Ϸ״̬
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING)
	{
		return true;
	}

	//�ѽ��й�����
	if (pIServerUserItem->GetUserPlayCount()>0)
	{
		//������Ϣ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("��Ǹ,���Ѳμӹ���������������������"),SMT_CHAT);

		return true;
	}
	
	//�˻�������
	if(m_pMatchOption->lSignupFee>0 && !pIServerUserItem->IsAndroidUser())
	{
		//��������
		DBR_GR_MatchUnSignup MatchUnSignup;
		MatchUnSignup.dwReason=cbReason;
		MatchUnSignup.dwUserID=pIServerUserItem->GetUserID();

		//����ṹ
		MatchUnSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
		MatchUnSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
		MatchUnSignup.dwMatchID=m_pMatchOption->dwMatchID;
		MatchUnSignup.lMatchNo=m_pMatchOption->lMatchNo;
		lstrcpyn(MatchUnSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchUnSignup.szMachineID));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,dwContextID,&MatchUnSignup,sizeof(MatchUnSignup));
	}

	//�Ƴ�����
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	//�Ƴ�ͬ����Ϣ
	m_DistributeManage.RemoveUserSameTableInfo(pIServerUserItem->GetUserID());

	return true;
}

//��Ϸ��ʼ
bool CLockTimeMatch::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	//���ͱ�����Ϣ
	SendTableUserMatchInfo(pITableFrame, INVALID_CHAIR);

	//�������ݰ�
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	IServerUserItem *pIServerUserItem=NULL;
	IServerUserItem *pITableUserItem=NULL;	

	for (INT i=0; i<pITableFrame->GetChairCount(); i++)
	{
		pIServerUserItem=pITableFrame->GetTableUserItem(i);
		//ASSERT(pIServerUserItem!=NULL);
		if (pIServerUserItem==NULL) continue;

		//������ʾ��Ϣ
		TCHAR szMessage[64];
		_sntprintf(szMessage,CountArray(szMessage),TEXT("���ֱ��������ĵ� %d �֡�"),pIServerUserItem->GetUserPlayCount()-pIServerUserItem->GetUserInfo()->dwDrawCount+1);
		lstrcpyn(SystemMessage.szString,szMessage,CountArray(SystemMessage.szString));
		SystemMessage.wLength=CountStringBuffer(SystemMessage.szString);
		SystemMessage.wType=SMT_CHAT;

		//������ʾ����
		WORD wSendSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString)+SystemMessage.wLength*sizeof(TCHAR);
		pITableFrame->SendTableData(pIServerUserItem->GetChairID(),SUB_GF_SYSTEM_MESSAGE, &SystemMessage,wSendSize,MDM_GF_FRAME);

		//���ͬ��
		ClearSameTableUser(pIServerUserItem->GetUserID());

		for (INT j=0; j<pITableFrame->GetChairCount(); j++)
		{
			//��ȡ�û�
			pITableUserItem=pITableFrame->GetTableUserItem(j);
			if(pITableUserItem==NULL) continue;
			if(pITableUserItem==pIServerUserItem) continue;

			//����ͬ���û�
			InsertSameTableUser(pIServerUserItem->GetUserID(),pITableUserItem->GetUserID());
		}
	}

	return true;
}

//��Ϸ����
bool CLockTimeMatch::OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return false;

	//��������
	for (INT i=0; i<pITableFrame->GetChairCount(); i++)
	{
		IServerUserItem *pIUserItem=pITableFrame->GetTableUserItem(i);
		//ASSERT(pIUserItem!=NULL);
		if (pIUserItem==NULL) continue;

		//��������
		if (cbReason!=REMOVE_BY_ENDMATCH)
		{
			//�ﵽ��̭��
			if (m_pLockTimeMatch->lMatchCullScore>0 && pIUserItem->GetUserScore()<m_pLockTimeMatch->lMatchCullScore)
			{
				//����״̬
				pIUserItem->SetUserMatchStatus(MUS_OUT);

				//����״̬		
				BYTE cbUserMatchStatus=pIUserItem->GetUserMatchStatus();
				m_pIGameServiceFrame->SendData(pIUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

				//��̭����
				if (m_pLockTimeMatch->cbReviveEnabled==TRUE)
				{
					//����ṹ
					DBR_GR_MatchQueryRevive MatchQueryRevive;

					//���ñ���
					MatchQueryRevive.dwUserID=pIUserItem->GetUserID();
					MatchQueryRevive.dwMatchID=m_pMatchOption->dwMatchID;
					MatchQueryRevive.lMatchNo=m_pMatchOption->lMatchNo;


					//Ͷ������
					m_pIDataBaseEngine->PostDataBaseRequest(MatchQueryRevive.dwUserID,DBR_GR_MATCH_QUERY_REVIVE,0,&MatchQueryRevive,sizeof(MatchQueryRevive));
				}
				else
				{
					//��̭����
					SendEliminateNotify(pIUserItem);
				}

				continue;
			}			

			//��ɾ���
			if (m_pMatchOption->cbRankingMode&RANKING_MODE_TOTAL_GRADES)
			{
				if (pIUserItem->GetUserPlayCount()-pIUserItem->GetUserInfo()->dwDrawCount==m_pLockTimeMatch->wMinPlayCount)
				{
					TCHAR szMessage[128]=TEXT("");
					_sntprintf(szMessage,CountArray(szMessage),TEXT("�������%d�ֱ��������Լ�����ǰ������ѡ���˳��ȴ����α����������������"),m_pLockTimeMatch->wMinPlayCount);
					m_pIGameServiceFrame->SendGameMessage(pIUserItem,szMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

					continue;
				}				
			}

			//�ض�����
			if (m_pMatchOption->cbRankingMode&RANKING_MODE_SPECIAL_GRADES)
			{
				if (pIUserItem->GetUserPlayCount()-pIUserItem->GetUserInfo()->dwDrawCount==m_pMatchOption->wCountInnings)
				{
					TCHAR szMessage[128]=TEXT("");
					_sntprintf(szMessage,CountArray(szMessage),TEXT("�������%d�ֱ�����ϵͳ�����¼����ǰ�ĳɼ������ñ����֣������Լ�����ǰ������ѡ���˳��ȴ����α���������鿴������"),m_pMatchOption->wCountInnings);
					m_pIGameServiceFrame->SendGameMessage(pIUserItem,szMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

					//���ͷ���
					SendMatchUserInitScore(pIUserItem);

					//����ṹ
					DBR_GR_MatchRecordGrades MatchRecordGrades;

					//���ñ���
					MatchRecordGrades.dwUserID=pIUserItem->GetUserID();
					MatchRecordGrades.dwMatchID=m_pMatchOption->dwMatchID;
					MatchRecordGrades.lMatchNo=m_pMatchOption->lMatchNo;
					MatchRecordGrades.wServerID=m_pGameServiceOption->wServerID;
					MatchRecordGrades.lInitScore=m_pLockTimeMatch->lMatchInitScore;

					//Ͷ������
					m_pIDataBaseEngine->PostDataBaseRequest(MatchRecordGrades.dwUserID,DBR_GR_MATCH_RECORD_GRADES,0,&MatchRecordGrades,sizeof(MatchRecordGrades));

					continue;
				}				
			}

			//�������
			if (InsertDistribute(pIUserItem)==true)
			{
				m_pIGameServiceFrame->SendGameMessage(pIUserItem,TEXT("���ֱ���������ϵͳ����Ŭ��Ϊ��������λ�����Ժ�..."),SMT_CHAT|SMT_TABLE_ROLL);
			}
		}
	}

	return true;
}

//��ҷ���
bool CLockTimeMatch::OnEventUserReturnMatch(ITableFrame *pITableFrame,IServerUserItem * pIServerUserItem)
{
	//���ͱ�����Ϣ
	SendTableUserMatchInfo(pITableFrame,pIServerUserItem->GetChairID());

	return true;
}

//�û�����
bool CLockTimeMatch::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	return true; 
}

//�û�����
bool CLockTimeMatch::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//�Թ��ж�
	if (bLookonUser==false)
	{
		//�Ƴ�����
		RemoveDistribute(pIServerUserItem);

		//�ж�����
		if (wTableID<m_pGameServiceOption->wTableCount)
		{
			//��ȡ����
			ITableFrame * pITableFrame=m_ppITableFrame[wTableID];
			if (pITableFrame==NULL ) return false;
			if (pITableFrame->GetGameStatus()>=GAME_STATUS_PLAY) return true;

			//�������
			for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
			{
				IServerUserItem * pITableUserItem=pITableFrame->GetTableUserItem(i);
				if (pITableUserItem==NULL) continue;
				if (pITableUserItem->GetUserStatus()!=US_READY) continue;			

				//�������
				if (InsertDistribute(pITableUserItem)==true)
				{
					//����״̬
					pITableUserItem->SetUserStatus(US_SIT,wTableID,i);
					m_pIGameServiceFrame->SendGameMessage(pITableUserItem,TEXT("��������뿪��ϵͳ����Ϊ�����·�����λ�����Ժ�..."),SMT_CHAT|SMT_TABLE_ROLL);
				}
			}
		}
	}

	return true;
}

 //�û�ͬ��
bool CLockTimeMatch::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{ 
	return true; 
}

//����Ȩ��
bool CLockTimeMatch::VerifyUserEnterMatch(IServerUserItem * pIServerUserItem)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������������
	if (pIServerUserItem->IsAndroidUser()==true) return true;

	//��������
	WORD wMessageType;
	TCHAR szMessage[128]=TEXT("");

	//У��ʱ��
	if (VerifyMatchTime(szMessage,CountArray(szMessage),wMessageType)==false)
	{
		//������Ϣ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,wMessageType);

		return false;
	}

	//��̭�ж�
	if (m_pLockTimeMatch->lMatchCullScore>0 && pIServerUserItem->GetUserScore()<m_pLockTimeMatch->lMatchCullScore)
	{
		_sntprintf(szMessage,CountArray(szMessage),TEXT("�������ı����ҵ��� %d���������ܼ�����Ϸ��"),m_pLockTimeMatch->lMatchCullScore);

		//������Ϣ
		if(pIServerUserItem->GetTableID()==INVALID_TABLE)
		{
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);
		}
		else
		{
			m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);
		}

		return false;
	}

	//������
	if (m_pLockTimeMatch->wMaxPlayCount>0 && (pIServerUserItem->GetUserPlayCount()-pIServerUserItem->GetUserInfo()->dwDrawCount)==m_pLockTimeMatch->wMaxPlayCount)
	{
		_sntprintf(szMessage,CountArray(szMessage),TEXT("������� %d �ֱ������������ܼ�����Ϸ����ȴ������������鿴������"),m_pLockTimeMatch->wMaxPlayCount);

		//������Ϣ
		if(pIServerUserItem->GetTableID()==INVALID_TABLE)
		{
			m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);
		}
		else
		{
			m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);
		}

		return false;
	}

	return true;
}

//��ֹ����
bool CLockTimeMatch::IsMatchSignupEnd()
{
	//��ȡʱ��
	CTime CurTime=CTime::GetCurrentTime();
	CTime SignupEndTime(m_pLockTimeMatch->SignupEndTime);	

	//���濪��
	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
	{
		//�Ƚ�ʱ��
		CTimeSpan TimeSpan=CurTime-SignupEndTime;
		
		return TimeSpan.GetTotalSeconds()>=0;	
	}
	else
	{
		//����ʱ��
		int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

		//�ж�ʱ��
		return nCurrSeconds>=(SignupEndTime.GetHour()*3600+SignupEndTime.GetMinute()*60+SignupEndTime.GetSecond());
	}		
}

//��ʼ����
bool CLockTimeMatch::IsMatchSignupStart()
{
	//��ȡʱ��
	CTime CurTime=CTime::GetCurrentTime();
	CTime SignupStartTime(m_pLockTimeMatch->SignupStartTime);	

	//���濪��
	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_NORMAL)
	{
		//�Ƚ�ʱ��
		CTimeSpan TimeSpan = CurTime-SignupStartTime;

		return TimeSpan.GetTotalSeconds()>=0;	
	}
	else
	{
		//����ʱ��
		int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

		//�ж�ʱ��
		return nCurrSeconds>=(SignupStartTime.GetHour()*3600+SignupStartTime.GetMinute()*60+SignupStartTime.GetSecond());
	}	
}

//��ʼ���
DWORD CLockTimeMatch::GetMatchStartInterval()
{
	//��ȡʱ��
	CTime CurTime=CTime::GetCurrentTime();
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);
	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);

	//��������
	DWORD dwTimeInterval=1;
	DWORD dwCurrStamp,dwStartStamp,dwEndStamp;

	//����δ��ʼ
	if(CurTime<MatchStartTime)
	{
		CTimeSpan TimeSpan = MatchStartTime-CurTime;
		dwTimeInterval = (DWORD)TimeSpan.GetTotalSeconds();
	}

	//�����ѽ���
	if(CurTime>=MatchEndTime) dwTimeInterval=0;

	//����ʱ��
	dwCurrStamp = CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();
	dwStartStamp = m_pLockTimeMatch->MatchStartTime.wHour*3600+m_pLockTimeMatch->MatchStartTime.wMinute*60+m_pLockTimeMatch->MatchStartTime.wSecond;
	dwEndStamp = m_pLockTimeMatch->MatchEndTime.wHour*3600+m_pLockTimeMatch->MatchEndTime.wMinute*60+m_pLockTimeMatch->MatchEndTime.wSecond;

	//����δ��ʼ
	if(dwCurrStamp<dwStartStamp) 
	{
		dwTimeInterval = dwStartStamp-dwCurrStamp;
	}

	//�����ѽ���
	if(dwCurrStamp>=dwEndStamp)
	{
		dwTimeInterval = 24*3600-(dwCurrStamp-dwStartStamp);		
	}

	return dwTimeInterval;
}

//��������
VOID CLockTimeMatch::PerformAllUserStandUp(ITableFrame *pITableFrame)
{
	//����У��
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	//��������
	for (int i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pUserServerItem=pITableFrame->GetTableUserItem(i);
		if (pUserServerItem&&pUserServerItem->GetTableID()!=INVALID_TABLE)
		{
			pITableFrame->PerformStandUpAction(pUserServerItem);
		}
	}	
}

//���㳡��
LONGLONG CLockTimeMatch::CalcMatchNo()
{
	//ѭ������
	if (m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND)
	{
		//��������
		SYSTEMTIME SystemStartTime=m_pLockTimeMatch->MatchStartTime;

		//���ñ���
		SystemStartTime.wHour=0;
		SystemStartTime.wMinute=0;
		SystemStartTime.wSecond=0;
		SystemStartTime.wMilliseconds=0;

		//��ȡʱ��
		CTime CurTime=CTime::GetCurrentTime();
		CTime MatchStartTime(SystemStartTime);

		//��������
		CTimeSpan TimeSpan=CurTime-MatchStartTime;

		return TimeSpan.GetDays()+1;
	}

	return 1;
}

//�����û�
bool CLockTimeMatch::PerformDistribute()
{
	//״̬�ж�
	if (m_MatchStatus!=MatchStatus_Playing) return true;

	//����У��
	if (m_DistributeManage.GetCount()<m_pMatchOption->wMinDistributeUser) return false;

	//�����û�
	while(true)
	{
		//��������
		CDistributeInfoArray DistributeInfoArray;

        //��ȡ�û�
		WORD wRandCount = __max(m_pMatchOption->wMaxPartakeGameUser-m_pMatchOption->wMinPartakeGameUser+1,1);
		WORD wChairCount = m_pMatchOption->wMinPartakeGameUser+rand()%wRandCount;
		WORD wDistributeCount = m_DistributeManage.PerformDistribute(DistributeInfoArray,wChairCount);
		if (wDistributeCount < wChairCount) break;

		//Ѱ��λ��
		ITableFrame * pICurrTableFrame=NULL;
		for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
		{
			//��ȡ����
			ASSERT(m_ppITableFrame[i]!=NULL);
			ITableFrame * pITableFrame=m_ppITableFrame[i];

			//״̬�ж�
			if (pITableFrame->GetNullChairCount()==pITableFrame->GetChairCount())
			{
				pICurrTableFrame=pITableFrame;
				break;
			}
		}

		//�����ж�
		if (pICurrTableFrame==NULL) break;

		//�������
		bool bSitSuccess=true;
		INT_PTR nSitFailedIndex=INVALID_CHAIR;
		for (INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
		{
			//��������
			WORD wChairID=INVALID_CHAIR;

			//��������
			for (WORD i=0;i<pICurrTableFrame->GetChairCount();i++)
			{
				if (pICurrTableFrame->GetTableUserItem(i)==NULL)
				{
					wChairID = i;
					break;
				}
			}

			//�����û�
			if (wChairID!=INVALID_CHAIR)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=DistributeInfoArray[nIndex].pIServerUserItem;

				//�û�����
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					WORD wTableID=pIServerUserItem->GetTableID();
					ITableFrame * pITableFrame = m_ppITableFrame[wTableID];
					pITableFrame->PerformStandUpAction(pIServerUserItem);
				}

				//�û�����
				if(pICurrTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
				{
					bSitSuccess=false;
					nSitFailedIndex=nIndex;
					break;
				}
			}
		}

		//���½��
		for (INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
		{
			if (bSitSuccess==true || nSitFailedIndex==nIndex)
			{
				m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[nIndex].pPertainNode);
			}
			else
			{
				//�û�����
				if (DistributeInfoArray[nIndex].pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					WORD wTableID=DistributeInfoArray[nIndex].pIServerUserItem->GetTableID();
					m_ppITableFrame[wTableID]->PerformStandUpAction(DistributeInfoArray[nIndex].pIServerUserItem);
				}
			}
		}			
	}

	return true;
}

//����״̬
VOID CLockTimeMatch::UpdateMatchStatus()
{
	//��������
	CTime CurTime=CTime::GetCurrentTime();
	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);	

	//����δ��ʼ
	BYTE MatchStatus=m_MatchStatus;

	//�ȴ���ʼ����
	if (MatchStatus==MatchStatus_Free && CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond() >= 
	   m_pLockTimeMatch->MatchStartTime.wHour*3600+m_pLockTimeMatch->MatchStartTime.wMinute*60+m_pLockTimeMatch->MatchStartTime.wSecond)
	{
		MatchStatus=MatchStatus_WaitPlay;
	}

	//����������
	if (MatchStatus==MatchStatus_WaitPlay && m_DistributeManage.GetCount()>=m_pLockTimeMatch->wStartUserCount)
	{
		MatchStatus=MatchStatus_Playing;
	}

	//�ȴ�����
	if (MatchStatus==MatchStatus_Playing && CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond() >= 
	   m_pLockTimeMatch->MatchEndTime.wHour*3600+m_pLockTimeMatch->MatchEndTime.wMinute*60+m_pLockTimeMatch->MatchEndTime.wSecond)
	{
		MatchStatus=MatchStatus_WaitEnd;
	}	

	//�����ж�
	if (MatchStatus==MatchStatus_WaitEnd)  
	{
		//��������
		bool bContinueWait=false;

		//��ɢ��Ϸ
		for (INT i=0; i<m_pGameServiceOption->wTableCount; i++)
		{
			//��ȡ����
			ITableFrame * pITableFrame=m_ppITableFrame[i];								
			if (pITableFrame==NULL) continue;

			//��ɢ��Ϸ
			if (pITableFrame->IsGameStarted())
			{
				bContinueWait=true;
				break;
			}
		}

		//����ȴ�
		if (bContinueWait==false)
		{
			MatchStatus=MatchStatus_End;
		}

		//ǿ�ƽ���
		if ((DWORD)(CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond()) >= 
			(DWORD)(m_pLockTimeMatch->MatchEndTime.wHour*3600+m_pLockTimeMatch->MatchEndTime.wMinute*60+m_pLockTimeMatch->MatchEndTime.wSecond+m_pLockTimeMatch->wEndDelayTime))
		{
			MatchStatus=MatchStatus_End;
		}
	}

	//����״̬
	m_MatchStatus=MatchStatus; 	
}

//�л�ʱ��
VOID CLockTimeMatch::SwitchMatchTimer()
{
	//����δ��ʼ
	if (m_MatchStatus==MatchStatus_Free)
	{
		//��ʼ���	
		m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH,2000L,TIMES_INFINITY,NULL);
	}

	//�ȴ���ʼ
	if (m_MatchStatus==MatchStatus_WaitPlay)
	{
		//�������
		m_pITimerEngine->SetTimer(IDI_CHECK_START_MATCH,2000L,TIMES_INFINITY,NULL);
	}

	//����������
	if (m_MatchStatus==MatchStatus_Playing)
	{
		//�������
		m_pITimerEngine->SetTimer(IDI_CHECK_END_MATCH,5000,TIMES_INFINITY,NULL);

		//������
		m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pMatchOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	}

	//�ȴ�����
	if (m_MatchStatus==MatchStatus_WaitEnd)
	{
		//�������
		m_pITimerEngine->SetTimer(IDI_CHECK_END_MATCH,5000,TIMES_INFINITY,NULL);
	}

	//�����ѽ���
	if (m_MatchStatus==MatchStatus_End)
	{
		//�л�״̬
		m_pITimerEngine->SetTimer(IDI_SWITCH_STATUS,2000,TIMES_INFINITY,NULL);
	}
}

//�����û�
bool CLockTimeMatch::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//����״̬
 	if(m_MatchStatus!=MatchStatus_WaitPlay && m_MatchStatus!=MatchStatus_Playing) return false;

	//״̬�ж�
	ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	//����У��
	if(VerifyUserEnterMatch(pIServerUserItem)==false) return false;

	//�����ж�
	if (m_DistributeManage.SearchNode(pIServerUserItem)!=NULL) return true;

	//��������
	tagDistributeInfo DistributeInfo;
	ZeroMemory(&DistributeInfo,sizeof(DistributeInfo));

	//���ñ���
	DistributeInfo.pPertainNode=NULL;	
	DistributeInfo.dwInsertStamp=(DWORD)time(NULL);
	DistributeInfo.pIServerUserItem=pIServerUserItem;   

	//����ڵ�
	if (m_DistributeManage.InsertDistributeNode(DistributeInfo)==false)
	{
		return pIServerUserItem->IsAndroidUser()?false:true; 
	}

	//�ȴ���ʼ
	if (m_MatchStatus==MatchStatus_WaitPlay)
	{
		//��������
		CMD_GR_Match_Num MatchNum;
		MatchNum.dwWaitting=m_DistributeManage.GetCount();
		MatchNum.dwTotal=m_pLockTimeMatch->wStartUserCount;

		//��������
		m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	}

	return true;
}


//������
void CLockTimeMatch::ClearSameTableUser(DWORD dwUserID)
{
	//��������
	tagSameTableInfo * pSameTableInfo = m_DistributeManage.GetUserSameTableInfo(dwUserID);
	if(pSameTableInfo!=NULL)
	{
		pSameTableInfo->wPlayerCount=0;
		ZeroMemory(pSameTableInfo->wPlayerIDSet,sizeof(pSameTableInfo->wPlayerIDSet));
	}
}

//�����û�
void CLockTimeMatch::InsertSameTableUser(DWORD dwUserID,DWORD dwTableUserID)
{
	//��������
	tagSameTableInfo * pSameTableInfo = m_DistributeManage.GetUserSameTableInfo(dwUserID);
	if(pSameTableInfo!=NULL)
	{
		//����û�
		pSameTableInfo->wPlayerIDSet[pSameTableInfo->wPlayerCount++] = dwTableUserID;
	}

	return;
}

//�¼�У��
bool CLockTimeMatch::VerifyMatchTime(LPTSTR pszMessage,WORD wMaxCount,WORD & wMessageType)
{
	//��������
	CTime CurTime=CTime::GetCurrentTime();
	CTime MatchEndTime(m_pLockTimeMatch->MatchEndTime);
	CTime MatchStartTime(m_pLockTimeMatch->MatchStartTime);
	CTime SignupStartTime(m_pLockTimeMatch->SignupStartTime);	
	CTime SignupEndTime(m_pLockTimeMatch->SignupEndTime);

	//��������
	bool bMatchRound=m_pLockTimeMatch->cbMatchMode==MATCH_MODE_ROUND;
	int nCurrSeconds=CurTime.GetHour()*3600+CurTime.GetMinute()*60+CurTime.GetSecond();

	//��������
	wMessageType=SMT_EJECT|SMT_CHAT;

	//��������
	if (CurTime>=MatchEndTime)
	{
		wMessageType |= SMT_CLOSE_ROOM|SMT_CLOSE_GAME;
		_sntprintf(pszMessage,wMaxCount,TEXT("�����Ѿ�����,�����Ƽ����������������������ɣ�"));
		return false;
	}

	//�����ѽ���
	if (nCurrSeconds >= MatchEndTime.GetHour()*3600+MatchEndTime.GetMinute()*60+MatchEndTime.GetSecond())
	{
		wMessageType |= SMT_CLOSE_ROOM|SMT_CLOSE_GAME;
		_sntprintf(pszMessage,wMaxCount,TEXT("����ı����Ѿ�����,����������%dʱ%d��%d��ǰ���μӱ�����"),MatchStartTime.GetHour(),MatchStartTime.GetMinute(),MatchStartTime.GetSecond());
		return false;
	}

	//����δ��ʼ
	if (CurTime<SignupStartTime)
	{
		wMessageType |= SMT_CLOSE_ROOM;
		_sntprintf(pszMessage,wMaxCount,TEXT("����������δ��ʼ,������������ʱ��Ϊ��%d-%d-%d %d:%d:%d��%d-%d-%d %d:%d:%d"),
			SignupStartTime.GetYear(),SignupStartTime.GetMonth(),SignupStartTime.GetDay(),SignupStartTime.GetHour(),SignupStartTime.GetMinute(),SignupStartTime.GetSecond(),
			SignupEndTime.GetYear(),SignupEndTime.GetMonth(),SignupEndTime.GetDay(),SignupEndTime.GetHour(),SignupEndTime.GetMinute(),SignupEndTime.GetSecond());
		return false;
	}


	//����δ��ʼ
	if (bMatchRound && nCurrSeconds<(SignupStartTime.GetHour()*3600+SignupStartTime.GetMinute()*60+SignupStartTime.GetSecond()))
	{
		wMessageType |= SMT_CLOSE_ROOM;
		_sntprintf(pszMessage,wMaxCount,TEXT("����������δ��ʼ,�����ڽ���%dʱ%d��%d��ǰ�������μӱ�����"),SignupStartTime.GetHour(),SignupStartTime.GetMinute(),SignupStartTime.GetSecond());
		return false;
	}	

	return true;
}

//�����û�����
bool CLockTimeMatch::SendMatchUserScore(IServerUserItem * pIServerUserItem)
{
	//��������
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore,sizeof(UserScore));

	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//��������
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;	
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;
	UserScore.UserScore.lIntegralCount=pUserInfo->lIntegralCount;

	//�������
	UserScore.UserScore.dBeans=pUserInfo->dBeans;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.lScore=pUserInfo->lScore;

	return m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
}

//���ͷ���
bool CLockTimeMatch::SendMatchUserInitScore(IServerUserItem * pIServerUserItem)
{
	//��ȡ�ɼ�
	tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();

	//���óɼ�
	pUserScore->dwWinCount=0L;
	pUserScore->dwLostCount=0L;
	pUserScore->dwFleeCount=0L;
	pUserScore->dwDrawCount=0L;
	pUserScore->lIntegralCount=0L;
	pUserScore->lScore=(LONGLONG)m_pLockTimeMatch->lMatchInitScore;

	//���ͷ���
	return SendMatchUserScore(pIServerUserItem);
}

//��������
bool CLockTimeMatch::SendReviveNotify(IServerUserItem * pIServerUserItem, BYTE cbReviveTimesed, bool bUseSafeCard)
{
	if (bUseSafeCard==false)
	{
		//����ṹ 
		CMD_GR_Match_Revive MatchRevive;

		//������Ϣ
		MatchRevive.dwMatchID=m_pMatchOption->dwMatchID;
		MatchRevive.lMatchNO=m_pMatchOption->lMatchNo;

		//������Ϣ
		MatchRevive.lReviveFee=m_pLockTimeMatch->lReviveFee;
		MatchRevive.cbReviveTimes=m_pLockTimeMatch->cbReviveTimes;
		MatchRevive.cbReviveTimesed=cbReviveTimesed;

		//������Ϣ
		_sntprintf(MatchRevive.szNotifyContent,CountArray(MatchRevive.szNotifyContent),TEXT("���ı����ҵ��� %d�������ܼ�����Ϸ�� ��ֻ��֧�� %.2f ��ұ�ɸ��������������ȷ��Ҫ������"),m_pLockTimeMatch->lMatchCullScore,m_pLockTimeMatch->lReviveFee);

        //��������
		WORD wDataSize=sizeof(MatchRevive)-sizeof(MatchRevive.szNotifyContent);
		wDataSize+=CountStringBuffer(MatchRevive.szNotifyContent);
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_REVIVE,&MatchRevive,wDataSize);
	}
	else
	{
		//��������
		TCHAR szMessage[128];
		_sntprintf(szMessage,CountArray(szMessage),TEXT("���ı����ҵ��� %d�������ܼ�����Ϸ�������������˱��տ���ϵͳ�����Ϊ������һ�Σ�"),m_pLockTimeMatch->lMatchCullScore);

		//��Ϣ֪ͨ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_EJECT);
	}

	return true;
}

//��������
bool CLockTimeMatch::SendSafeCardNotify(IServerUserItem * pIServerUserItem)
{
	//����ṹ
	CMD_GR_Match_SafeCard MatchSafeCard;

	//���ñ���
	MatchSafeCard.dwMatchID=m_pMatchOption->dwMatchID;
	MatchSafeCard.lMatchNO=m_pMatchOption->lMatchNo;
	MatchSafeCard.lSafeCardFee=m_pLockTimeMatch->lSafeCardFee;

	//������Ϣ
	_sntprintf(MatchSafeCard.szNotifyContent,CountArray(MatchSafeCard.szNotifyContent),TEXT("��ϲ�������ɹ��������������۱��տ�, ����̭����Ѹ���һ�Σ��ۼ� %.2f ��ң����Ƿ�Ҫ����"),MatchSafeCard.lSafeCardFee);

	//��������
	WORD wDataSize=sizeof(MatchSafeCard)-sizeof(MatchSafeCard.szNotifyContent);
	wDataSize+=CountStringBuffer(MatchSafeCard.szNotifyContent);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_SAFECARD,&MatchSafeCard,wDataSize);

	return true;
}

//��������
bool CLockTimeMatch::SendDeductFeeNotify(IServerUserItem * pIServerUserItem)
{
	//����ṹ
	CMD_GR_Match_Fee MatchFee;

	//���ñ���
	MatchFee.cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;
	MatchFee.dwMatchID=m_pMatchOption->dwMatchID;
	MatchFee.lMatchNO=m_pMatchOption->lMatchNo;
	MatchFee.lSignupFee=m_pMatchOption->lSignupFee;
	MatchFee.cbFeeType=m_pMatchOption->cbFeeType;
	MatchFee.cbDeductArea=m_pMatchOption->cbDeductArea;	

	ZeroMemory(MatchFee.szNotifyContent,sizeof(MatchFee.szNotifyContent));

	//������ʾ			
	if (m_pMatchOption->cbDeductArea==DEDUCT_AREA_SERVER && MatchFee.lSignupFee>0)
	{
		LPCTSTR pszFeeType[]={TEXT("��Ϸ��"),TEXT("Ԫ��")};
		_sntprintf(MatchFee.szNotifyContent,CountArray(MatchFee.szNotifyContent),TEXT("�������۳������� %.2f %s��ȷ��Ҫ������"),m_pMatchOption->lSignupFee,pszFeeType[m_pMatchOption->cbFeeType-FEE_TYPE_GOLD]);
	}

	//������Ϣ
	WORD wDataSize=sizeof(MatchFee)-sizeof(MatchFee.szNotifyContent);
	wDataSize+=CountStringBuffer(MatchFee.szNotifyContent);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_FEE,&MatchFee,wDataSize);

	return true;
}

//��̭����
bool CLockTimeMatch::SendEliminateNotify(IServerUserItem * pIServerUserItem)
{
	//��������
	TCHAR szMessage[128]=TEXT("");
	_sntprintf(szMessage,CountArray(szMessage),TEXT("�������ı����ҵ��� %d���������ܼ�����Ϸ��"),m_pLockTimeMatch->lMatchCullScore);

	//������Ϣ
	m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_EJECT|SMT_CHAT);

	//����ṹ
	DBR_GR_MatchEliminate MatchEliminate;
	MatchEliminate.cbMatchType=MATCH_TYPE_LOCKTIME;
	MatchEliminate.dwMatchID=m_pMatchOption->dwMatchID;
	MatchEliminate.lMatchNo=m_pMatchOption->lMatchNo;
	MatchEliminate.dwUserID=pIServerUserItem->GetUserID();

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(MatchEliminate.dwUserID,DBR_GR_MATCH_ELIMINATE,0,&MatchEliminate,sizeof(MatchEliminate));

	//��������
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_ELIMINATE,NULL,0);

	return true;
}

//�����ɹ�
bool CLockTimeMatch::SendSignupSuccessNotify(IServerUserItem * pIServerUserItem)
{
	//�����ɹ�
	if (m_MatchStatus!=MatchStatus_Free)
	{
		//��Ϣ֪ͨ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("��ϲ���������ɹ�����ȴ�ϵͳΪ��������λ��"),SMT_CHAT);

		//�������
		InsertDistribute(pIServerUserItem);					
	}
	else
	{
		//��Ϣ֪ͨ
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("��ϲ���������ɹ������ʱǰ���μӱ�����"),SMT_CHAT);
	}

	return true;
}

//�Ƴ����
bool CLockTimeMatch::RemoveDistribute(IServerUserItem * pIServerUserItem)
{
	//��֤����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�Ƴ��û�
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	return true;
}


//���ͱ�����Ϣ
void CLockTimeMatch::SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID)
{
	//��֤����
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	//��������
	CMD_GR_Match_Info MatchInfo;
	ZeroMemory(&MatchInfo,sizeof(MatchInfo));

	//���ñ���
	_sntprintf(MatchInfo.szTitle[0], CountArray(MatchInfo.szTitle[0]),TEXT("���ַ�ʽ���ۼƻ���"));
	_sntprintf(MatchInfo.szTitle[1], CountArray(MatchInfo.szTitle[1]),TEXT("��Ϸ������%d"), m_pLockTimeMatch->lMatchInitScore);
	_sntprintf(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]),TEXT("�������ƣ���ʱ����"));

	for(WORD i=0; i<pITableFrame->GetChairCount(); i++)
	{
		//��ȡ�û�
		if(wChairID!=INVALID_CHAIR && wChairID!=i) continue;
		IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem==NULL) continue;

		//���þ���
		MatchInfo.wGameCount=(WORD)(pIServerUserItem->GetUserPlayCount()-pIServerUserItem->GetUserInfo()->dwDrawCount)+1;

		//��������
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			pITableFrame->SendTableData(i,SUB_GR_MATCH_INFO,&MatchInfo,sizeof(MatchInfo),MDM_GF_FRAME);
		}
	}

	return;
}

//д�뽱��
bool CLockTimeMatch::WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo)
{
	//����Ч��
	ASSERT(pMatchRankInfo!=NULL);
	if(pMatchRankInfo==NULL) return false;

	//д���¼
	if (m_pIDataBaseEngine!=NULL)
	{
		//��������
		DBR_GR_MatchReward MatchReward;
		ZeroMemory(&MatchReward,sizeof(MatchReward));
		
		//����ṹ
		MatchReward.dwUserID=pMatchRankInfo->dwUserID;			
		MatchReward.lRewardGold=pMatchRankInfo->lRewardGold;
		MatchReward.lRewardIngot=pMatchRankInfo->lRewardIngot;
		MatchReward.dwRewardExperience=pMatchRankInfo->dwRewardExperience;
		MatchReward.dwClientAddr=pIServerUserItem?pIServerUserItem->GetClientAddr():0;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(MatchReward.dwUserID,DBR_GR_MATCH_REWARD,0,&MatchReward,sizeof(MatchReward));
	}

	//�������
	if (pIServerUserItem!=NULL)
	{
		//��������
		CMD_GR_MatchResult MatchResult;
		ZeroMemory(&MatchResult,sizeof(MatchResult));

		//��������
		GetSystemTime(&MatchResult.MatchEndTime);
		MatchResult.wMatchRank=pMatchRankInfo->wRankID;
		MatchResult.lGold=pMatchRankInfo->lRewardGold;		
		MatchResult.lIngot=pMatchRankInfo->lRewardIngot;	
		MatchResult.dwExperience=pMatchRankInfo->dwRewardExperience;
		lstrcpyn(MatchResult.szNickName,pIServerUserItem->GetNickName(),CountArray(MatchResult.szNickName));
		lstrcpyn(MatchResult.szMatchName,m_pMatchOption->szMatchName,CountArray(MatchResult.szMatchName));

		//��������
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));
		}
	}		

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
