#include "StdAfx.h"
#include "ImmediateMatch.h"
#include "..\��Ϸ������\DataBasePacket.h"
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INVALID_VALUE				0xFFFF								//��Чֵ

//////////////////////////////////////////////////////////////////////////

//ʱ�Ӷ���
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+1)			//�����û�
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+2)			//��ʼʱ��
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+3)			//����ʱ��

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CImmediateMatch::CImmediateMatch()
{
	//��������
	m_dwStartTime=0;

	//��������
	m_pMatchOption=NULL;	
	m_pImmediateMatch=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//����ָ��
	m_ppITableFrame=NULL;
	m_pCurMatchGroup=NULL;

	//�ں˽ӿ�
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//����ӿ�
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;
	m_pIServerUserItemSink=NULL;
}

CImmediateMatch::~CImmediateMatch(void)
{
	SafeDeleteArray(m_ppITableFrame);
	
	m_LoopTimer.RemoveAll();
	SafeDelete(m_pCurMatchGroup);
	m_MatchGroup.Append(m_OverMatchGroup);
	for (int i=0; i<m_MatchGroup.GetCount(); i++)
	{
		SafeDelete(m_MatchGroup[i]);
	}

	m_MatchGroup.RemoveAll();
	m_OverMatchGroup.RemoveAll();
}

//�ӿڲ�ѯ
VOID* CImmediateMatch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IGameMatchItem,Guid,dwQueryVer);	
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchItem,Guid,dwQueryVer);
	return NULL;
}

//����֪ͨ
void CImmediateMatch::OnStartService()
{
	//��������
	if (m_pCurMatchGroup==NULL)
	{
		//����ʱ��
		m_dwStartTime=(DWORD)time(NULL);
		m_pCurMatchGroup=new CImmediateGroup(CalcMatchNo(),m_pMatchOption,this);
	}

	//���ͬʱ9����
	for (int i=0;i<79;i++) m_LoopTimer.Add(i);

	//���ö�ʱ��
	m_pITimerEngine->SetTimer(IDI_CHECK_OVER_MATCH,60000L,TIMES_INFINITY,0);
	m_pITimerEngine->SetTimer(IDI_DELETE_OVER_MATCH, 6000,TIMES_INFINITY,0);

	//�������
	InsertNullTable();
}

//������
bool CImmediateMatch::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
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
bool CImmediateMatch::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
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
	m_pImmediateMatch=(tagImmediateMatch *)m_pMatchOption->cbMatchRule;

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

	//������������
	CImmediateGroup::m_wChairCount=m_pMatchOption->wMinPartakeGameUser;
	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];
	}

	return true;
}

//ʱ���¼�
bool CImmediateMatch::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	if(dwTimerID>=IDI_GROUP_TIMER_START)
	{
		for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
		{
			//�鿴���ĳ������Ķ�ʱ��
			if (m_MatchGroup[i]->m_LoopTimer==((dwTimerID-IDI_GROUP_TIMER_START)/10))
			{
				m_MatchGroup[i]->OnTimeMessage(dwTimerID-(m_MatchGroup[i]->m_LoopTimer*10),dwBindParameter);
			}
		}
		return true;
	}

	switch(dwTimerID)
	{
	case IDI_CHECK_OVER_MATCH:
		{
			//��ʱ���һ�� ��Щ����������������������
			for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
			{
				//�����û�С���������� ��Ҫ����
				m_MatchGroup[i]->CheckMatchUser();
			}
			return true;
		}
	case IDI_DELETE_OVER_MATCH:
		{
			for (int i=0;m_OverMatchGroup.GetCount();i++)
			{
				CImmediateGroup * pOverMatch=m_OverMatchGroup[i];
				m_OverMatchGroup.RemoveAt(i--);
				SafeDelete(pOverMatch);
			}
			return true;
		}
	}

	return true;
}


//���ݿ��¼�
bool CImmediateMatch::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,DWORD dwContextID)
{
	switch (wRequestID)
	{
	case DBO_GR_MATCH_SIGNUP_RESULT:		//�������
		{
			//����Ч��
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//��ȡ����
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;			

			//����ʧ��
			if(pMatchSignupResult->bResultCode==false)
			{
				//��ʾ��Ϣ��Ϊ��
				if (pMatchSignupResult->szDescribeString[0]!=0)
				{
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);
				}

				return true;
			}

			//�жϳ���
			if (m_pCurMatchGroup->m_lMatchNo!=pMatchSignupResult->lMatchNo)
			{
				SendRoomMessage(pIServerUserItem, TEXT("�����Ѿ���ʼ,����ʧ��,�����±�����һ��������"),SMT_CHAT|SMT_EJECT);

				//�˻�����
				if (m_pMatchOption->lSignupFee>0)
				{
					//��������
					DBR_GR_MatchUnSignup MatchUnSignup;
					MatchUnSignup.dwReason=UNSIGNUP_REASON_SYSTEM;
					MatchUnSignup.dwUserID=pIServerUserItem->GetUserID();

					//����ṹ
					MatchUnSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
					MatchUnSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
					MatchUnSignup.dwMatchID=m_pMatchOption->dwMatchID;
					MatchUnSignup.lMatchNo=pMatchSignupResult->lMatchNo;
					lstrcpyn(MatchUnSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchUnSignup.szMachineID));

					//Ͷ������
					m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,dwContextID,&MatchUnSignup,sizeof(MatchUnSignup));
				}

				return true;
			}

			//�������
			if (m_pCurMatchGroup->OnUserSignUpMatch(pIServerUserItem)==false)
			{
				SendRoomMessage(pIServerUserItem, TEXT("����ʧ��"),SMT_CHAT);
				return false;
			}

			//�����ɹ�
			m_OnMatchUserItem.Add(pIServerUserItem);
			SendMatchInfo(pIServerUserItem);
			if (m_pIGameServiceFrame!=NULL)
			{
				SendRoomMessage(pIServerUserItem, TEXT("��ϲ�������ɹ�����ȴ�ϵͳΪ��������λ��"),SMT_CHAT);
			}

			//��Ҹ���
			if (pIServerUserItem->IsAndroidUser()==false)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//��������
				return SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_UNSIGNUP_RESULT:		//�������
		{
			//����Ч��
			if (pIServerUserItem==NULL) return true;
			if (wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//��ȡ����
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;			

			//�˷�ʧ��
			if (pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			//����״̬
			pIServerUserItem->SetUserMatchStatus(MUS_NULL);

			//���ͱ���״̬
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			SendData (pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_USTATUS,&cbUserMatchStatus,sizeof(cbUserMatchStatus));

			//�۳�������
			if (pMatchSignupResult->cbSignupMode==SIGNUP_MODE_SIGNUP_FEE)
			{
				//����۷�
				if(m_pMatchOption->cbDeductArea==DEDUCT_AREA_SERVER)
				{
					//�˷�֪ͨ
					TCHAR szDescribe[128]=TEXT("");
					if (m_pMatchOption->lSignupFee>0)
					{
						LPCTSTR pszFeeType[]={TEXT("��Ϸ��"),TEXT("Ԫ��")};
						_sntprintf(szDescribe,CountArray(szDescribe),TEXT("�����ɹ����˻�������%.2f%s��"),m_pMatchOption->lSignupFee,pszFeeType[m_pMatchOption->cbFeeType-FEE_TYPE_GOLD]);
					}
					else
					{
						_sntprintf(szDescribe,CountArray(szDescribe),TEXT("�����ɹ���"));
					}
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szDescribe,SMT_EJECT);
				}
				
				//��վ�۷�
				if (m_pMatchOption->cbDeductArea==DEDUCT_AREA_WEBSITE)
				{
					TCHAR szDescribe[128]=TEXT("�����ɹ���");
					m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szDescribe,SMT_EJECT);
				}
			}

			//��������
			if (pMatchSignupResult->cbSignupMode==SIGNUP_MODE_MATCH_USER)
			{
				TCHAR szDescribe[128]=TEXT("�����ɹ���");
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szDescribe,SMT_EJECT);
			}

			//��Ҹ���
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;

				//��������
				return SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
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
			IServerUserItem * pIRewardUserItem=NULL;
			tagMatchRankInfo * pMatchRankInfo=NULL;

			//���Ž���
			for(WORD wIndex=0;wIndex<pMatchRankList->wUserCount;wIndex++)
			{
				//�������
				pMatchRankInfo=&pMatchRankList->MatchRankInfo[wIndex];
				pIRewardUserItem=m_pIServerUserManager->SearchUserItem(pMatchRankInfo->dwUserID);
				if(pIRewardUserItem==NULL || pIRewardUserItem->IsAndroidUser()==true) continue;

				//д�뽱��
				WriteUserAward(pIRewardUserItem,pMatchRankInfo);
			}

			//��������
			CImmediateGroup * pImmediateGroup=NULL;

			//���ҷ���
			for (int i=0;i<m_MatchGroup.GetCount();i++)
			{
				if(m_MatchGroup[i]->m_lMatchNo==pMatchRankList->lMatchNo)
				{
					pImmediateGroup=m_MatchGroup[i];

					if(pImmediateGroup!=NULL)
					{
						//ȫ������
						pImmediateGroup->PerformAllUserStandUp();

						//����״̬
						BYTE cbMatchStatus=MS_FREE;
						SendGroupData (MDM_GR_MATCH,SUB_GR_MATCH_STATUS,&cbMatchStatus,sizeof(cbMatchStatus),pImmediateGroup);

						//��ԭ���
						m_LoopTimer.Add(pImmediateGroup->m_LoopTimer);

						//�����������
						POSITION pos=pImmediateGroup->m_OnMatchUserMap.GetStartPosition();
						IServerUserItem *pITempUserItem=NULL;
						DWORD dwUserID=0;
						WORD wQuitUserCount=0;
						WORD wMatchUserCount=(WORD)pImmediateGroup->m_OnMatchUserMap.GetCount();
						while(pos!=NULL)
						{
							pImmediateGroup->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pITempUserItem);
							if (pITempUserItem!=NULL)
							{
								pITempUserItem->SetUserMatchStatus(MUS_NULL);
								if (true==OnEventUserQuitMatch(pITempUserItem,UNSIGNUP_REASON_PLAYER))
									wQuitUserCount++;

								//����������������δ���أ����߳�����
								bool bAndroidUser=pITempUserItem->IsAndroidUser();
								bool bTrusteeUser=pITempUserItem->IsTrusteeUser();
								if (bAndroidUser==false && bTrusteeUser==true)
								{
									//�뿪����
									WORD wTableID=pITempUserItem->GetTableID();
									if (wTableID!=INVALID_TABLE)
									{
										//״̬�ж�
										ASSERT (pITempUserItem->GetUserStatus()!=US_PLAYING);
										SendGameMessage(pITempUserItem,TEXT(""),SMT_CLOSE_GAME);
										
										//�������Ӳ�����
										for(int m=0;m<pImmediateGroup->m_MatchTableArray.GetCount();m++)
										{
											if(pITempUserItem->GetTableID()==pImmediateGroup->m_MatchTableArray[m]->wTableID)
											{
												pImmediateGroup->m_MatchTableArray[m]->pTableFrame->PerformStandUpAction(pITempUserItem);
												break;
											}
										}
									}

									//�뿪����
									if (pITempUserItem->GetBindIndex()==INVALID_WORD)
									{
										pITempUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
									}
								}
							}
						}
						
						//У���������
						ASSERT(wQuitUserCount==wMatchUserCount);
						if (wQuitUserCount!=wMatchUserCount)
						{
							CTraceService::TraceString(TEXT("�������������������쳣"),TraceLevel_Exception);
						}

						//����ȫ������
						for (int j=0;pImmediateGroup->m_MatchTableArray.GetCount();)
						{
							tagTableFrameInfo* pTempTable=pImmediateGroup->m_MatchTableArray[j];
							pImmediateGroup->m_MatchTableArray.RemoveAt(j);
							SafeDelete(pTempTable);
						}
					}
					//�Ƴ�����
					m_MatchGroup.RemoveAt(i);
					m_OverMatchGroup.Add(pImmediateGroup);

					break;
				}
			}


			break;
		}
	case DBO_GR_MATCH_REWARD_RESULT:		//�������
		{
			//����Ч��
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchRewardResult)) return false;

			//��ȡ����
			DBO_GR_MatchRewardResult * pMatchRewardResult = (DBO_GR_MatchRewardResult*)pData;

			//�����ɹ�
			if(pMatchRewardResult->bResultCode==true)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchRewardResult->lCurrGold;

				//��������
				SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	}
	return true;
}

//������Ϣ
bool CImmediateMatch::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
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
			MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
			MatchSignup.lMatchNo=m_pCurMatchGroup->m_lMatchNo;	
			MatchSignup.dwUserID=pIServerUserItem->GetUserID();
			MatchSignup.lInitScore=m_pImmediateMatch->lInitalScore;			
			MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
			MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();					
			MatchSignup.wMaxSignupUser=m_pImmediateMatch->wStartUserCount;
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

			//�˳�����
			OnEventUserQuitMatch(pIServerUserItem,UNSIGNUP_REASON_PLAYER,0,dwSocketID);

			return true;
		}
	}
	return true;
}

//�û���¼
bool CImmediateMatch::OnEventUserLogon(IServerUserItem * pIServerUserItem)
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
bool CImmediateMatch::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	//�˳�����
	OnEventUserQuitMatch(pIServerUserItem, UNSIGNUP_REASON_SYSTEM);

	return true;
}

//��¼���
bool CImmediateMatch::OnEventUserLogonFinish(IServerUserItem * pIServerUserItem)
{	
	//���ͱ�����Ϣ
	return SendMatchInfo(pIServerUserItem);
}

//�����¼�
bool CImmediateMatch::OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	//�ֻ��û�
	if(bIsMobile == true)
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
		lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
		lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));
	}

	return true;
}

//�û�����
bool CImmediateMatch::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	//����У��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL)
	{
		CTraceService::TraceString(TEXT("�û�ָ��Ϊ��!"),TraceLevel_Warning);
		return true;
	}

	//�����ӳٱ���
	if (pIServerUserItem->IsAndroidUser()==true)
	{
		//��ȡʱ��
		DWORD dwCurrTime=(DWORD)time(NULL);
		if (dwCurrTime-m_dwStartTime<m_pImmediateMatch->wAndroidDelaySignupTime)
		{
			return true;
		}
	}

	//���Ӳ���
	WORD wTableCount=GetNullTable();
	if (wTableCount<m_pImmediateMatch->wStartUserCount/CImmediateGroup::m_wChairCount)
	{
		//������Ϣ
		TCHAR szString[128];
		_sntprintf(szString,CountArray(szString),TEXT("��Ǹ����ǰ���Ӳ�����ȴ������� %d �����ӿ��Ա���"),m_pImmediateMatch->wStartUserCount/CImmediateGroup::m_wChairCount-wTableCount);
		SendRoomMessage(pIServerUserItem,szString,SMT_CHAT);
		return true;
	}

	//��������
	if (m_pCurMatchGroup==NULL || m_pCurMatchGroup->m_cbMatchStatus==MatchStatus_Wait)
	{
		SendRoomMessage(pIServerUserItem, TEXT("��Ǹ���÷�������������������Ժ��ٱ������߽�����һ���������"),SMT_CHAT);
		return true;
	}

	//�ظ�����
	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]->GetUserID()==pIServerUserItem->GetUserID())
		{
			SendRoomMessage(pIServerUserItem, TEXT("���Ѿ��ɹ������������ظ�������"),SMT_CHAT|SMT_EJECT);
			return true;
		}
	}

	//��Ų���
	if (m_LoopTimer.GetCount()==0)
	{
		SendRoomMessage(pIServerUserItem, TEXT("��Ǹ��������Ų������Եȡ�"),SMT_CHAT);
		return true;
	}

	//״̬����
	if (pIServerUserItem->GetUserStatus()!=US_FREE || pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		SendRoomMessage(pIServerUserItem, TEXT("�Բ�������ǰ��״̬������μӱ�����"),SMT_CHAT);
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

	//���ɱ�����
	if (m_pMatchOption->cbSignupMode==SIGNUP_MODE_SIGNUP_FEE)
	{
		//���˿۱�����
		if (m_pMatchOption->lSignupFee>0 && pIServerUserItem->IsAndroidUser()==false)
		{
			//����ṹ
			CMD_GR_Match_Fee MatchFee;

			//���ñ���
			MatchFee.dwMatchID=m_pMatchOption->dwMatchID;
			MatchFee.lMatchNO=m_pCurMatchGroup->m_lMatchNo;
			MatchFee.lSignupFee=m_pMatchOption->lSignupFee;
			MatchFee.cbFeeType=m_pMatchOption->cbFeeType;
			MatchFee.cbDeductArea=m_pMatchOption->cbDeductArea;
			MatchFee.cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;

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
	}

	//�����˱���
	if (m_pMatchOption->lSignupFee==0 || pIServerUserItem->IsAndroidUser()==true)
	{
		//�������ж�
		if(pIServerUserItem->IsAndroidUser()==true && (m_pCurMatchGroup->m_wAndroidUserCount >= 
		   m_pImmediateMatch->wAndroidUserCount))
		{
			return true;
		}

		//��������
		BYTE cbSignupMode=SIGNUP_MODE_SIGNUP_FEE;
		return OnEventSocketMatch(SUB_GR_MATCH_FEE,&cbSignupMode,sizeof(cbSignupMode),pIServerUserItem ,dwSocketID);
	}

	//�����ɹ�
	m_OnMatchUserItem.Add(pIServerUserItem);
	SendMatchInfo(NULL);
	if (m_pIGameServiceFrame!=NULL)
	{
		LONGLONG lScore=LONGLONG(m_pMatchOption->lSignupFee)*-1;
		/*if(m_pImmediateMatch->lInitalScore==0)
		{
			pIServerUserItem->GetUserInfo()->lScore+=lScore;
			m_pIServerUserItemSink->OnEventUserItemScore(pIServerUserItem,SCORE_REASON_MATCH_FEE);
		}*/
		SendRoomMessage(pIServerUserItem, TEXT("��ϲ�������ɹ�����ȴ�ϵͳΪ��������λ��"),SMT_CHAT);
	}

	return true;
}

//�û�����
bool CImmediateMatch::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������Ϸ״̬
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING)
	{
		return true;
	}

	//���ڱ����׶�
	BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
	if (cbUserMatchStatus==MUS_PLAYING)
	{
		return true;
	}

	//�����������
	if(RemoveMatchUserItem(pIServerUserItem))
	{
		//�����ڱ���������ɾ�������
		INT_PTR nGroupCount=m_MatchGroup.GetCount();
		for (INT_PTR i=0;i<nGroupCount;i++)
		{
			CImmediateGroup *pMatch=m_MatchGroup[i];
			if(pMatch->OnUserQuitMatch(pIServerUserItem))
			{
				if (pMatch->m_cbMatchStatus!=MatchStatus_Null)
				{
					SendRoomMessage(pIServerUserItem, TEXT("�����ɹ�����ӭ�������μ�����������"), SMT_CHAT|SMT_EJECT);
				}
				return true;
			}
		}

		//�����ڱ���������ɾ�������
		if(m_pCurMatchGroup->OnUserQuitMatch(pIServerUserItem))
		{
			//�˻�������
			if(m_pMatchOption->lSignupFee>=0 /*&& !pIServerUserItem->IsAndroidUser()*/)
			{
				//��������
				DBR_GR_MatchUnSignup MatchUnSignup;
				MatchUnSignup.dwReason=cbReason;
				MatchUnSignup.dwUserID=pIServerUserItem->GetUserID();

				//����ṹ
				MatchUnSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
				MatchUnSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
				MatchUnSignup.dwMatchID=m_pMatchOption->dwMatchID;
				MatchUnSignup.lMatchNo=m_pCurMatchGroup->m_lMatchNo;
				lstrcpyn(MatchUnSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchUnSignup.szMachineID));

				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(MatchUnSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,dwContextID,&MatchUnSignup,sizeof(MatchUnSignup));
			}
			else
			{
				if (m_pCurMatchGroup->m_cbMatchStatus!=MatchStatus_Null)
				{
					SendRoomMessage(pIServerUserItem, TEXT("�����ɹ�����ӭ�������μ�����������"), SMT_CHAT|SMT_EJECT);
				}
			}

			//���ͱ���
			SendMatchInfo(NULL);

			return true;
		}

		//���ͱ���
		SendMatchInfo(NULL);
	}

	return false;
}

//�û�����
bool CImmediateMatch::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CImmediateGroup * pImmediateGroup = static_cast<CImmediateGroup *>(pIServerUserItem->GetMatchData());
	if(pImmediateGroup==NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemScore(pIServerUserItem, cbReason);
	}

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
	UserScore.UserScore.lIntegralCount =pUserInfo->lIntegralCount;

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
		WriteGameScore.lMatchNo=pImmediateGroup->m_lMatchNo;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore),TRUE);
	}

	return true;
}

//�û�����
bool CImmediateMatch::OnEventUserItemGameData(IServerUserItem *pIServerUserItem, BYTE cbReason)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemGameData(pIServerUserItem,cbReason);
	}

	return true;
}

//�û�״̬
bool CImmediateMatch::OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)
{
	//�������
	if(pIServerUserItem->GetUserStatus()==US_NULL) pIServerUserItem->SetMatchData(NULL);

	//�ص��ӿ�
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wLastTableID,wLastChairID);
	}

	return true;
}

//�û�Ȩ��
bool CImmediateMatch::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,cbRightKind);
	}

	return true;
}

//������������ʼ����
bool CImmediateMatch::OnEventMatchStart(CImmediateGroup *pMatch)
{
	ASSERT(pMatch==m_pCurMatchGroup&&m_LoopTimer.GetCount()>0);

	if (pMatch!=NULL)
	{
		//��������
		DBR_GR_MatchStart MatchStart;

		//����ṹ
		MatchStart.dwMatchID=m_pMatchOption->dwMatchID;
		MatchStart.lMatchNo=pMatch->m_lMatchNo;
		MatchStart.cbMatchType=m_pMatchOption->cbMatchType;
		MatchStart.wServerID=m_pGameServiceOption->wServerID;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(MatchStart.dwMatchID,DBR_GR_MATCH_START,0,&MatchStart,sizeof(MatchStart));
	
		//��Ų��� ���ܱ�����
		pMatch->m_LoopTimer=m_LoopTimer[0];
		m_LoopTimer.RemoveAt(0);
		m_MatchGroup.Add(pMatch);

		//׼���µ�һ�����
		m_dwStartTime=(DWORD)time(NULL);
		m_pCurMatchGroup=new CImmediateGroup(CalcMatchNo(),m_pMatchOption,this);
		
		SendMatchInfo(NULL);
		BYTE cbMatchStatus=MS_MATCHING;
		SendGroupData( MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus),pMatch);
	}

	return true;
}


//��������
bool CImmediateMatch::OnEventMatchOver(CImmediateGroup *pMatch)
{
	//��������
	if(pMatch!=NULL)
	{
		DBR_GR_MatchOver MatchOver;
		ZeroMemory(&MatchOver,sizeof(MatchOver));

		//��������				
		MatchOver.bMatchFinish=TRUE;
		MatchOver.dwMatchID=m_pMatchOption->dwMatchID;
		MatchOver.lMatchNo=pMatch->m_lMatchNo;		
		MatchOver.cbMatchType=MATCH_TYPE_IMMEDIATE;
		MatchOver.wPlayCount=m_pImmediateMatch->wPlayCount;
		MatchOver.wServerID=m_pGameServiceOption->wServerID;
		
		//��ȡʱ��
		MatchOver.MatchStartTime=pMatch->m_MatchStartTime;
		CTime::GetCurrentTime().GetAsSystemTime(MatchOver.MatchEndTime);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(MatchOver.dwMatchID,DBR_GR_MATCH_OVER,0,&MatchOver,sizeof(MatchOver));
	}

	return true;
}

//�رն�ʱ��
bool CImmediateMatch::KillGameTimer(DWORD dwTimerID,CImmediateGroup *pMatch)
{
	m_pITimerEngine->KillTimer(dwTimerID);

	return false;
}

//���ö�ʱ��
bool CImmediateMatch::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CImmediateGroup *pMatch)
{
	return m_pITimerEngine->SetTimer(dwTimerID,dwElapse,dwRepeat,dwBindParameter);
}


//��������
bool CImmediateMatch::SendMatchInfo(IServerUserItem * pIServerUserItem)
{

	CMD_GR_Match_Num MatchNum;
	MatchNum.dwWaitting=(DWORD)m_pCurMatchGroup->m_OnMatchUserMap.GetCount();
	MatchNum.dwTotal=m_pImmediateMatch->wStartUserCount;
	m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	if(pIServerUserItem==NULL)
	{
		return true;
	}

	//���ͱ�����Ϣ
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_DESC, &m_MatchDesc, sizeof(m_MatchDesc));

	//���ͱ���״̬
	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]==pIServerUserItem)
		{
			BYTE cbUserMatchStatus=pIServerUserItem->GetUserMatchStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_USTATUS, &cbUserMatchStatus, sizeof(cbUserMatchStatus));

			return true;
		}
	}

	tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();
	pUserScore->dwWinCount=pUserScore->dwLostCount=pUserScore->dwFleeCount=pUserScore->dwDrawCount=pUserScore->cbGender=0;
	if((LONGLONG)m_pImmediateMatch->lInitalScore!=0)
	{
		pUserScore->lScore=(LONG)m_pImmediateMatch->lInitalScore;
	}

	return SendGroupUserScore(pIServerUserItem, NULL);
}


//������Ϣ
bool CImmediateMatch::SendGroupUserMessage(LPCTSTR pStrMessage,CImmediateGroup *pMatch)
{
	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		SendGameMessage(pUserItem,pStrMessage, SMT_CHAT|SMT_TABLE_ROLL);
	}
	return true;
}


//�����û�����
bool CImmediateMatch::SendGroupUserScore(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch)
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
	UserScore.UserScore.lIntegralCount =pUserInfo->lIntegralCount;

	//�������
	UserScore.UserScore.lScore=pUserInfo->lScore;
	UserScore.UserScore.lIngot=pUserInfo->lIngot;
	UserScore.UserScore.dBeans=pUserInfo->dBeans;

	if(pMatch!=NULL)
	{
		POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
		IServerUserItem *pUserItem=NULL;
		DWORD dwUserID=0;
		while(pos!=NULL)
		{
			pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
			m_pIGameServiceFrame->SendData(pUserItem,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));	
		}
		return true;
	}

	return m_pIGameServiceFrame->SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
}


//����״̬
bool  CImmediateMatch::SendGroupUserStatus(IServerUserItem * pIServerUserItem,CImmediateGroup *pMatch)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsClientReady()==true);
	if (pIServerUserItem->IsClientReady()==false) return false;

	//��������
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));

	tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();

	//��������
	UserStatus.dwUserID=pUserData->dwUserID;
	UserStatus.UserStatus.wTableID=pUserData->wTableID;
	UserStatus.UserStatus.wChairID=pUserData->wChairID;
	UserStatus.UserStatus.cbUserStatus=pUserData->cbUserStatus;

	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		if(pUserItem->IsClientReady())
			m_pIGameServiceFrame->SendData(pUserItem,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));	
	}
	return true;
}


//������Ϸ��Ϣ
bool CImmediateMatch::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	ASSERT(pIServerUserItem!=-NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if ((pIServerUserItem->GetBindIndex()!=INVALID_WORD)&&(pIServerUserItem->IsClientReady()==true)&&(pIServerUserItem->IsAndroidUser()==false))
	{
		//�������ݰ�
		CMD_CM_SystemMessage Message;
		Message.wType=wMessageType;
		lstrcpyn(Message.szString,lpszMessage,CountArray(Message.szString));
		Message.wLength=CountStringBuffer(Message.szString);

		//��������
		WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
		return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&Message,wSendSize);
	}

	return true;
}


//������Ϣ
bool CImmediateMatch::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	ASSERT(pIServerUserItem!=-NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD && false==pIServerUserItem->IsAndroidUser())
	{
		//�������ݰ�
		CMD_CM_SystemMessage Message;
		Message.wType=wMessageType;

		lstrcpyn(Message.szString,lpszMessage,CountArray(Message.szString));
		Message.wLength=CountStringBuffer(Message.szString);

		//��������
		WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
		return m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize);
	}

	return true;
}

//�������ݵ�һ���û�
bool CImmediateMatch::SendGroupData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize,CImmediateGroup *pMatch)
{
	POSITION pos=pMatch->m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pUserItem=NULL;
	DWORD dwUserID=0;
	while(pos!=NULL)
	{
		pMatch->m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pUserItem);
		m_pIGameServiceFrame->SendData(pUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);	
	}
	return true;
}

//��������
bool CImmediateMatch::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if(pIServerUserItem!=NULL)
		return m_pIGameServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	else
		return m_pIGameServiceFrame->SendData(BG_COMPUTER,wMainCmdID,wSubCmdID, pData, wDataSize);
    
	return true;
}

//Ϊ���ֲ��������
void CImmediateMatch::InsertNullTable()
{
	for (int i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//�������˳�
		WORD nCurTableCount=(WORD)m_pCurMatchGroup->m_MatchTableArray.GetCount();
		if(nCurTableCount>=m_pImmediateMatch->wStartUserCount/CImmediateGroup::m_wChairCount)break;

		BOOL bIsInsert=true;
		for (int j=0;j<m_MatchGroup.GetCount();j++)
		{
			WORD wMatchGroupTableCount=(WORD)m_MatchGroup[j]->m_MatchTableArray.GetCount();
			for (int k=0;k<wMatchGroupTableCount;k++)
			{
				WORD wTableID=m_MatchGroup[j]->m_MatchTableArray[k]->wTableID;
				if(i==wTableID){bIsInsert=false;break;}//�������Ѿ������˸���
			}
		}
		//��������
		if( bIsInsert&&m_ppITableFrame[i]->GetNullChairCount()==m_pGameServiceAttrib->wChairCount)
		{
			tagTableFrameInfo * pTableFrameInfo=new tagTableFrameInfo;
			ZeroMemory(pTableFrameInfo,sizeof(tagTableFrameInfo));			
			pTableFrameInfo->wTableID=i;
			pTableFrameInfo->pTableFrame=m_ppITableFrame[i];
			pTableFrameInfo->lBaseScore=m_pImmediateMatch->lInitalBase;
			ITableFrameHook * pFrameHook=QUERY_OBJECT_PTR_INTERFACE(m_ppITableFrame[i]->GetTableFrameHook(),ITableFrameHook);
			pFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(m_pCurMatchGroup,IUnknownEx));
			m_pCurMatchGroup->AddMatchTable(pTableFrameInfo);
		}
	}
}

//��ȡһ�����еĻ�����
IAndroidUserItem * CImmediateMatch::GetFreeAndroidUserItem()
{
	return NULL;
}

//д�뽱��
bool CImmediateMatch::WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo)
{
	//����Ч��
	ASSERT(pIServerUserItem!=NULL && pMatchRankInfo!=NULL);
	if(pIServerUserItem==NULL || pMatchRankInfo==NULL) return false;

	//д���ж�
	bool bWriteReward=(pMatchRankInfo->lRewardGold+pMatchRankInfo->lRewardIngot+pMatchRankInfo->dwRewardExperience)>0;

	//д�뽱��
	if (bWriteReward==true)
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

			//������Ϣ
			//TCHAR szMessage[256]=TEXT("\0");
			//_sntprintf(szMessage,CountArray(szMessage),TEXT("����鿴�������У�http://service.foxuc.com/GameMatch.aspx?PlazaStationID=%d&TypeID=2&MatchID=%d"),
			//pIServerUserItem->GetPlazaStationID(),m_pMatchOption->dwMatchID);

			////������Ϣ
			//m_pIGameServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_GLOBAL);	
		}
	}
	else
	{
		//������Ϣ
		TCHAR szMessage[256];
		_sntprintf(szMessage,CountArray(szMessage),TEXT("�����ѽ�������ϲ����õ�%d������ӭ���μ��������εı�����"),pMatchRankInfo->wRankID);

		//������Ϣ
		SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);

		return true;
	}

	//д���¼
	if(m_pIDataBaseEngine!=NULL)
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

	return true;
}

//���㳡��
LONGLONG CImmediateMatch::CalcMatchNo()
{
	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime); 

	//ʱ�����
	WORD wDay=SystemTime.wDay;	
	WORD wMonth=SystemTime.wMonth;
	WORD wYear=SystemTime.wYear-1980;
	DWORD dwSecond=SystemTime.wHour*3600+SystemTime.wMinute*60+SystemTime.wSecond;

	//��¼����
	static DWORD wMatchCount=0;
	static DWORD wLastDay=SystemTime.wDay;	

	//���¼�¼
	if (SystemTime.wDay>wLastDay)
	{
		wMatchCount=1;
		wLastDay=SystemTime.wDay;
	}
	else
	{
		wMatchCount++;
	}

	return (LONGLONG(wYear)<<44|LONGLONG(wMonth)<<40|LONGLONG(wDay)<<34|LONGLONG(dwSecond)<<16|LONGLONG(wMatchCount));
}

//��ȡ������
WORD CImmediateMatch::GetNullTable()
{
	WORD wTableCount=0;
	for (int i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		BOOL bIsInsert=true;
		for (int j=0;j<m_MatchGroup.GetCount();j++)
		{
			WORD wMatchGroupTableCount=(WORD)m_MatchGroup[j]->m_MatchTableArray.GetCount();
			for (int k=0;k<wMatchGroupTableCount;k++)
			{
				WORD wTableID=m_MatchGroup[j]->m_MatchTableArray[k]->wTableID;
				if(i==wTableID){bIsInsert=false;break;}//�������Ѿ������˸���
			}
		}
		//��������
		if( bIsInsert&&m_ppITableFrame[i]->GetNullChairCount()==m_pGameServiceAttrib->wChairCount)
		{
			wTableCount++;
		}
	}
	return wTableCount;
}

//ɾ���û�
bool CImmediateMatch::DeleteUserItem(DWORD dwUserIndex)
{
	try
	{
		if (m_pITCPNetworkEngineEvent == NULL)
		{
			throw 0;
		}
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwUserIndex,0,0L);
	}
	catch (...)
	{
		//�������
		ASSERT(FALSE);
		return false;
	}
	return true;
}

//�Ƴ������û�
bool CImmediateMatch::RemoveMatchUserItem(IServerUserItem *pIServerUserItem)
{
	for (INT_PTR i=0;i<m_OnMatchUserItem.GetCount();i++)
	{
		//��ȡ�û�
		IServerUserItem * pOnLineUserItem=m_OnMatchUserItem[i];
		
		//�û��ж�
		if (pOnLineUserItem==pIServerUserItem)
		{
			ASSERT(pOnLineUserItem->GetUserID()==pIServerUserItem->GetUserID());

			m_OnMatchUserItem.RemoveAt(i);
			return true;
		}
	}

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
