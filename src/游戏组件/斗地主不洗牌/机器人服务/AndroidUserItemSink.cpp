#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "AndroidServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//����ʱ��
#define TIME_LESS					4									//����ʱ��
#define TIME_DISPATCH				5									//����ʱ��

//��Ϸʱ��
#define TIME_OUT_CARD				3									//����ʱ��
#define TIME_START_GAME				8									//��ʼʱ��
#define TIME_CALL_SCORE				5									//�з�ʱ��

//��Ϸʱ��
#define IDI_OUT_CARD				(0)			//����ʱ��
#define IDI_START_GAME				(1)			//��ʼʱ��
#define IDI_CALL_SCORE				(2)			//�з�ʱ��
#define IDI_MULTIPLE				(3)			//�ӱ�ʱ��

//////////////////////////////////////////////////////////////////////////
//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;
	m_cbCurrentLandScore = 255 ;				//�ѽз���
	m_wOutCardUser = INVALID_CHAIR ;

	//�˿˱���
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//�����˿�
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//�ӿڱ���
	m_pIAndroidUserItem=NULL;;

	return;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
VOID * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ�ӿ�
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//���ýӿ�
bool CAndroidUserItemSink::RepositionSink()
{
	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;

	//�˿˱���
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//�����˿�
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//��ʼ��Ϸ
		{
			IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			if (pUserItem)
			{
				//��������
				SCORE lRobotScore = pUserItem->GetUserScore();
				//�жϴ�ȡ
				if ((lRobotScore >m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)||(lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore))
				{		
					//���ͻ������˳�
					m_pIAndroidUserItem->SendAndroidQuit(NULL, 0);
					return true;
				}
			}
					  		//�뿪�ж�
			if(m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITLEAVE))
			{
				//��������
				//PerformStandUpAction(pIServerUserItem);
			//������Ϣ
			m_pIAndroidUserItem->SendSocketData(SUB_S_ANDROIDUP);
				//continue;
				return true;
			}

			//�����ж�
			if(m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP))
			{
				//��������
				//PerformStandUpAction(pIServerUserItem);
			m_pIAndroidUserItem->SendSocketData(SUB_S_ANDROIDUP);
			//continue;
			return true;
		}	
			//��ʼ�ж�
			m_pIAndroidUserItem->SendUserReady(NULL,0);
			return true;
		}
	case IDI_CALL_SCORE:	//�û��з�
		{
			//�������
			CMD_C_CallScore CallScore;
			ZeroMemory(&CallScore,sizeof(CallScore));

			//���ñ���
			CallScore.cbCallScore=m_GameLogic.LandScore(m_pIAndroidUserItem->GetChairID(), m_cbCurrentLandScore);
			//��������
			m_pIAndroidUserItem->SendSocketData(SUB_C_CALL_SCORE,&CallScore,sizeof(CallScore));

			return true;
		}
	case IDI_MULTIPLE:		//�û��ӱ�
		{
			//�������
			CMD_C_Multiple Multiple;
			ZeroMemory(&Multiple,sizeof(Multiple));

			//���ñ���
			Multiple.bIsMultiple = false;

			//��������
			m_pIAndroidUserItem->SendSocketData(SUB_C_MULTIPLE,&Multiple,sizeof(Multiple));

			return true;
		}
	case IDI_OUT_CARD:		//�û�����
		{
			//�˿˷���
			tagOutCardResult OutCardResult;
			try
			{

				WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
				m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount[wMeChairID],m_cbTurnCardData,m_cbTurnCardCount, m_wOutCardUser, m_pIAndroidUserItem->GetChairID(), OutCardResult);
			}
			catch(...)
			{
				//��������ã�ʹ�ý��������if����
				ZeroMemory(OutCardResult.cbResultCard, sizeof(OutCardResult.cbResultCard)) ;
				OutCardResult.cbCardCount = 10 ;
			}
			//���ͺϷ��ж�

			if(OutCardResult.cbCardCount>0 && CT_ERROR==m_GameLogic.GetCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount))
			{
				ASSERT(false) ;
				ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;
			}
			int res = 0;
			//�ȳ��Ʋ���Ϊ��
			if(m_cbTurnCardCount==0)
			{
				ASSERT(OutCardResult.cbCardCount>0) ;
				if(OutCardResult.cbCardCount==0)
				{
					WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
					//��Сһ��
					OutCardResult.cbCardCount = 1 ;
					OutCardResult.cbResultCard[0]=m_cbHandCardData[m_cbHandCardCount[wMeChairID]-1] ;
				}
			}
			else
			{
				res = m_GameLogic.CompareCard(m_cbTurnCardData, OutCardResult.cbResultCard, m_cbTurnCardCount, OutCardResult.cbCardCount);
				if(res == 0)
				{
					//��������
					m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
					return true;
				}				
			}

			//�����һ�ֲ����ǵ��� 
			//�����С��ѹ��
			if (m_wOutCardUser != m_wBankerUser)
			{
				if (res == 2)
				{
					//��������
					m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
					return true;
				}
			}

			//����ж�
			if (OutCardResult.cbCardCount>0)
			{
				WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
				//ɾ���˿�
				m_cbHandCardCount[wMeChairID]-=OutCardResult.cbCardCount;
				m_GameLogic.RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,m_cbHandCardData,m_cbHandCardCount[wMeChairID]+OutCardResult.cbCardCount);
				//�������
				CMD_C_OutCard OutCard;
				ZeroMemory(&OutCard,sizeof(OutCard));

				//���ñ���
				OutCard.cbCardCount=OutCardResult.cbCardCount;
				CopyMemory(OutCard.cbCardData,OutCardResult.cbResultCard,OutCardResult.cbCardCount*sizeof(BYTE));

				//��������
				WORD wHeadSize=sizeof(OutCard)-sizeof(OutCard.cbCardData);
				m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD,&OutCard,wHeadSize+OutCard.cbCardCount*sizeof(BYTE));
			}
			else
			{
				//��������
				m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
			}
			 
		

			return true;
		}
	}

	return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//��Ϸ��ʼ
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_CALL_SCORE:	//�û��з�
		{
			return OnSubCallScore(pData,wDataSize);
		}
	case SUB_S_BANKER_INFO:	//ׯ����Ϣ
		{
			return OnSubBankerInfo(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:	//�û�����
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_PASS_CARD:	//�û�����
		{
			return OnSubPassCard(pData,wDataSize);
		}
	case SUB_S_GAME_CONCLUDE:	//��Ϸ����
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_OTHER_CARDS:	// ����������
		return true;
	case SUB_S_MULTIPLE: //�ӱ�
		return true;
	case SUB_S_START_OUTCARD: //��ʼ����
		{
			return OnSubStartOutCard(pData,wDataSize);
		}
	case SUB_S_ANDROID_BANKOPERATOR:
		{
			//BankOperate(2);
			return true;
		}
	}

	//�������
//	ASSERT(FALSE);

	return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��������
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();
			BankOperate(2);

			//�������
			if (pIServerUserItem->GetUserStatus()!=US_READY)
			{
				UINT nElapse=1;
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
			}

			return true;
		}
	case GAME_SCENE_CALL:	//�з�״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusCall)) return false;
			CMD_S_StatusCall * pStatusCall=(CMD_S_StatusCall *)pData;

			//�˿�����
			for (WORD i=0;i<GAME_PLAYER;i++) m_cbHandCardCount[i]=NORMAL_COUNT;
			CopyMemory(m_cbHandCardData,pStatusCall->cbHandCardData,sizeof(BYTE)*NORMAL_COUNT);
			BankOperate(2);

			//�з�����
			if (m_pIAndroidUserItem->GetChairID()==pStatusCall->wCurrentUser)
			{
				UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_CALL_SCORE,nElapse+TIME_DISPATCH);
			}

			return true;
		}
	case GAME_SCENE_PLAY:	//��Ϸ״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//���Ʊ���
			m_cbTurnCardCount=pStatusPlay->cbTurnCardCount;
			CopyMemory(m_cbTurnCardData,pStatusPlay->cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

			//�˿�����
			WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
			CopyMemory(m_cbHandCardCount,pStatusPlay->cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(m_cbHandCardData,pStatusPlay->cbHandCardData,sizeof(BYTE)*m_cbHandCardCount[wMeChairID]);
			BankOperate(2);

			//�������
			if (pStatusPlay->wCurrentUser==m_pIAndroidUserItem->GetChairID())
			{
				UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
			}

			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�û�����
VOID CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
VOID CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
VOID CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
VOID CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û���λ
VOID CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_AndroidCard));
	if (wDataSize!=sizeof(CMD_S_AndroidCard)) return false;

	//��������
	CMD_S_AndroidCard * pAndroidCard=(CMD_S_AndroidCard *)pData;

	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_CALL);
	m_cbCurrentLandScore = 255 ;

	//�˿˱���
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//�����˿�
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	for (WORD i=0;i<GAME_PLAYER;i++) m_cbHandCardCount[i]=NORMAL_COUNT;

	for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		if(wChairID==wMeChairID)
            CopyMemory(m_cbHandCardData,pAndroidCard->cbHandCard[wChairID],sizeof(BYTE)*NORMAL_COUNT);

		m_GameLogic.SetUserCard(wChairID, pAndroidCard->cbHandCard[wChairID], NORMAL_COUNT) ;
	}

	//�����˿�
	BYTE cbLandScoreCardData[MAX_COUNT] ;
	CopyMemory(cbLandScoreCardData, m_cbHandCardData,m_cbHandCardCount[wMeChairID]) ;
	CopyMemory(cbLandScoreCardData+NORMAL_COUNT, pAndroidCard->cbBankerCard, 3*sizeof(BYTE)) ;
	m_GameLogic.SetLandScoreCardData(cbLandScoreCardData, sizeof(cbLandScoreCardData)) ;

	//�����˿�
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID],ST_ORDER);

	//��Ҵ���
	if (m_pIAndroidUserItem->GetChairID()==pAndroidCard->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_CALL_SCORE,nElapse+TIME_DISPATCH);
	}
	return true;
}

//�û��з�
bool CAndroidUserItemSink::OnSubCallScore(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_CallScore));
	if (wDataSize!=sizeof(CMD_S_CallScore)) return false;

	//��������
	CMD_S_CallScore * pCallScore=(CMD_S_CallScore *)pData;
	m_cbCurrentLandScore = pCallScore->cbCurrentScore ;

	//�û�����
	if (m_pIAndroidUserItem->GetChairID()==pCallScore->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_CALL_SCORE,nElapse);
	}
	return true;
}

//ׯ����Ϣ
bool CAndroidUserItemSink::OnSubBankerInfo(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_BankerInfo));
	if (wDataSize!=sizeof(CMD_S_BankerInfo)) return false;

	//��������
	CMD_S_BankerInfo * pBankerInfo=(CMD_S_BankerInfo *)pData;

	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_PLAY);

	//���ñ���
	m_wBankerUser=pBankerInfo->wBankerUser;
	m_cbHandCardCount[m_wBankerUser]+=CountArray(pBankerInfo->cbBankerCard);

	//�����˿�
	if (pBankerInfo->wBankerUser==m_pIAndroidUserItem->GetChairID())
	{
		//�����˿�
		CopyMemory(&m_cbHandCardData[NORMAL_COUNT],pBankerInfo->cbBankerCard,sizeof(pBankerInfo->cbBankerCard));

		//�����˿�
		WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
		m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID],ST_ORDER);
	}
	//���õ���
	m_GameLogic.SetBackCard(pBankerInfo->wBankerUser, pBankerInfo->cbBankerCard, 3) ;

	////�������
	//if (pBankerInfo->wCurrentUser==m_pIAndroidUserItem->GetChairID())
	//{
	//	UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS+TIME_LESS;
	//	m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
	//}
	//�������
	UINT nElapse=rand()%(pBankerInfo->cbMultiple - 2) + 3;
	m_pIAndroidUserItem->SetGameTimer(IDI_MULTIPLE,nElapse);
	m_GameLogic.SetBanker(pBankerInfo->wBankerUser) ;
	return true;
}

// ��ʼ����
bool CAndroidUserItemSink::OnSubStartOutCard(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_START_OUTCARD));
	if (wDataSize!=sizeof(CMD_S_START_OUTCARD)) return false;
	//��������
	CMD_S_START_OUTCARD * pStartOutCard =(CMD_S_START_OUTCARD *)pData;
	if (pStartOutCard == NULL) return false;
	//�������
	if (pStartOutCard->wBankerUser==m_pIAndroidUserItem->GetChairID())
	{
		UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
	}
	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubOutCard(VOID * pData, WORD wDataSize)
{
	//��������
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);

	//Ч������
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(BYTE)))) return false;

	//���Ʊ���
	if (pOutCard->wCurrentUser==pOutCard->wOutCardUser)
	{
		m_cbTurnCardCount=0;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	}
	else
	{
		m_cbTurnCardCount=pOutCard->cbCardCount;
		CopyMemory(m_cbTurnCardData,pOutCard->cbCardData,pOutCard->cbCardCount*sizeof(BYTE));
		m_wOutCardUser = pOutCard->wOutCardUser ;
	}

	//�˿���Ŀ
	if (pOutCard->wOutCardUser!=m_pIAndroidUserItem->GetChairID())
	{
		m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
	}
	//�������
	if (m_pIAndroidUserItem->GetChairID()==pOutCard->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
	}
	//���ñ���
	m_GameLogic.RemoveUserCardData(pOutCard->wOutCardUser, pOutCard->cbCardData, pOutCard->cbCardCount) ;

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubPassCard(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_PassCard));
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;

	//��������
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pData;

	//һ���ж�
	if (pPassCard->cbTurnOver==TRUE)
	{
		m_cbTurnCardCount=0;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	}

	//�������
	if (m_pIAndroidUserItem->GetChairID()==pPassCard->wCurrentUser)
	{
		UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
	}

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameConclude));
	if (wDataSize!=sizeof(CMD_S_GameConclude)) return false;
	//��������
	CMD_S_GameConclude * pGameEnd=(CMD_S_GameConclude *)pData;
	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GAME_SCENE_FREE);

	//���ñ���
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//ɾ��ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OUT_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_CALL_SCORE);
	m_pIAndroidUserItem->KillGameTimer(IDI_MULTIPLE);
	//��ʼ����
	CGameServiceManager *pGameServiceManager=CGameServiceManager::GetInstance();
	ASSERT(pGameServiceManager!=NULL);
	if (pGameServiceManager==NULL) return true;
	WORD wServerType=pGameServiceManager->m_GameServiceOption.wServerType;
	if (wServerType!=GAME_GENRE_MATCH)
	{
		UINT nElapse=rand()%TIME_START_GAME+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
		//BankOperate(3);
	}
	return true;
}

//���в���
void CAndroidUserItemSink::BankOperate(BYTE cbType)
{
	IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
	if(pUserItem->GetUserStatus()>=US_SIT)
	{
		if(cbType==1)
		{
			CString strInfo;
			strInfo.Format(TEXT("������״̬���ԣ���ִ�д�ȡ��"));
			//NcaTextOut(strInfo);
			return;
		}
	}

	//��������
	SCORE lRobotScore = pUserItem->GetUserScore();

	{
		CString strInfo;
		strInfo.Format(TEXT("[%s] ����(%I64d)"),pUserItem->GetNickName(),lRobotScore);

		if (lRobotScore > m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)
		{
			CString strInfo1;
			strInfo1.Format(TEXT("����������(%I64d)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore);
			strInfo+=strInfo1;

			//NcaTextOut(strInfo);
		}
		else if (lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)
		{
			CString strInfo1;
			strInfo1.Format(TEXT("����ȡ������(%I64d)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore);
			strInfo+=strInfo1;

			//NcaTextOut(strInfo);
		}

		//�жϴ�ȡ
		if (lRobotScore >m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)
		{			
			SCORE lSaveScore=0L;
			lSaveScore = lRobotScore - m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore;

			if (lSaveScore > 0) 
			{
				lSaveScore += (rand()%(int)(m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore - m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore));
				m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
			}
			SCORE lRobotNewScore = pUserItem->GetUserScore();

			CString strInfo;
			strInfo.Format(TEXT("[%s] ִ�д����ǰ���(%I64d)��������(%I64d)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			//NcaTextOut(strInfo);
		}
		else if (lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)
		{
			CString strInfo;
			SCORE lScore=m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore-lRobotScore;
			if (lScore > 0)
			{
				lScore += (rand()%(int)(m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore - m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore));
				m_pIAndroidUserItem->PerformTakeScore(lScore);
			}

			SCORE lRobotNewScore = pUserItem->GetUserScore();
			strInfo.Format(TEXT("[%s] ִ��ȡ�ȡ��ǰ���(%I64d)��ȡ�����(%I64d)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			//NcaTextOut(strInfo);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
