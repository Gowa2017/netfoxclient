#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_BANK_OPERATE			3									//���ж�ʱ
#define IDI_PLACE_JETTON1			103									//��ע��ʱ
#define IDI_PLACE_JETTON2			104									//��ע��ʱ
#define IDI_PLACE_JETTON3			105									//��ע��ʱ
#define IDI_PLACE_JETTON4			106									//��ע��ʱ
#define IDI_PLACE_JETTON5			107									//��ע��ʱ
#define IDI_CHECK_BANKER			108									//�����ׯ
#define IDI_REQUEST_BANKER			101									//���붨ʱ
#define IDI_GIVEUP_BANKER			102									//��ׯ��ʱ
#define IDI_PLACE_JETTON			110									//��ע���� (Ԥ��110-160)

//////////////////////////////////////////////////////////////////////////

int CAndroidUserItemSink::m_stlApplyBanker = 0L;
int CAndroidUserItemSink::m_stnApplyCount = 0L;

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//��Ϸ����
	m_lMaxChipBanker = 0;
	m_lMaxChipUser = 0;
	m_lAndroidBet = 0;
	m_lBankerScore = 0;
	m_wCurrentBanker = 0;
	m_nChipTime = 0;
	m_nChipTimeCount = 0;
	m_cbTimeLeave = 0;
	ZeroMemory(m_lAreaChip, sizeof(m_lAreaChip));
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));
	ZeroMemory(m_lAllBet, sizeof(m_lAllBet));

	//��ׯ����
	m_bMeApplyBanker = false;
	m_nWaitBanker=0;
	m_nBankerCount=0;
	m_nListUserCount= 0;
	
	// ��ȡ������ʼ��
	m_lRobotBetLimit[1] = 500;
	m_lRobotBetLimit[0] = 1;
	if (m_lRobotBetLimit[1] > 500)					m_lRobotBetLimit[1] = 500;
	if (m_lRobotBetLimit[0] < 1)						m_lRobotBetLimit[0] = 1;
	if (m_lRobotBetLimit[1] < m_lRobotBetLimit[0])	m_lRobotBetLimit[1] = m_lRobotBetLimit[0];

	//��������
	m_nRobotBetTimeLimit[0] = 4;
	m_nRobotBetTimeLimit[1] = 8;

	if (m_nRobotBetTimeLimit[0] < 0)							m_nRobotBetTimeLimit[0] = 0;
	if (m_nRobotBetTimeLimit[1] < m_nRobotBetTimeLimit[0])		m_nRobotBetTimeLimit[1] = m_nRobotBetTimeLimit[0];

	//�Ƿ���ׯ
	m_bRobotBanker = 0;

	//��ׯ����
	m_nRobotBankerCount = 3;
	m_nMinBankerTimes=2;
	m_nMaxBankerTimes=5;

	//��������
	m_nRobotWaitBanker = 3;

	//��ׯ����
	m_nRobotListMaxCount = 5;
	m_nRobotListMinCount = 2;

	//��������
	m_bReduceBetLimit = 0;

	//�������
	//m_RobotInfo.nAreaChance[0] = 3;
	//m_RobotInfo.nAreaChance[1] = 1;
	//m_RobotInfo.nAreaChance[2] = 3;
	//m_RobotInfo.nAreaChance[3] = 1;
	//m_RobotInfo.nAreaChance[4] = 1;
	//m_RobotInfo.nAreaChance[5] = 0;
	//m_RobotInfo.nAreaChance[6] = 0;
	//m_RobotInfo.nAreaChance[7] = 0;

	srand((unsigned)time(NULL));  

	return;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
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
	m_lMaxChipBanker = 0;
	m_lMaxChipUser = 0;
	m_lAndroidBet = 0;
	m_lBankerScore = 0;
	m_wCurrentBanker = 0;
	m_nChipTime = 0;
	m_nChipTimeCount = 0;
	m_cbTimeLeave = 0;
	ZeroMemory(m_lAreaChip, sizeof(m_lAreaChip));
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));
	ZeroMemory(m_lAllBet,sizeof(m_lAllBet));

	//��ׯ����
	m_bMeApplyBanker = false;
	m_nWaitBanker=0;
	m_nBankerCount=0;

	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_CHECK_BANKER:		//�����ׯ
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);

			if (m_wCurrentBanker == m_pIAndroidUserItem->GetChairID())
				return false;

			int nMinCount = m_nRobotListMaxCount;
			if ( m_nRobotListMaxCount > m_nRobotListMinCount)
				nMinCount = (rand()%(m_nRobotListMaxCount - m_nRobotListMinCount)) + m_nRobotListMinCount;

			//��ׯ
			m_nWaitBanker++;

			//��������ׯ
			if ( m_bRobotBanker 
				&& !m_bMeApplyBanker 
				&& m_nWaitBanker >= m_nRobotWaitBanker 
				&& m_nListUserCount < m_nRobotListMaxCount 
				&& m_stlApplyBanker < m_nRobotListMaxCount
				&& m_stlApplyBanker < m_nRobotApplyBanker
				&& m_stlApplyBanker < nMinCount 
				&& m_stnApplyCount < 2 )
			{
				m_nWaitBanker = 0;

				//�Ϸ��ж�
				IServerUserItem *pIUserItemBanker = m_pIAndroidUserItem->GetMeUserItem();
				if (pIUserItemBanker->GetUserScore() > m_lBankerCondition ) 
				{
					//��������ׯ
					m_nBankerCount = 0;
					m_stlApplyBanker++;
					m_stnApplyCount++;
					m_pIAndroidUserItem->SetGameTimer(IDI_REQUEST_BANKER, (rand() % m_cbTimeLeave) + 1);
				}
				else
				{
					////ִ��ȡ��
					//LONGLONG lDiffScore = (m_lRobotBankGetScoreBankerMax-m_lRobotBankGetScoreBankerMin)/100;
					//SCORE lScore = m_lRobotBankGetScoreBankerMin + (m_pIAndroidUserItem->GetUserID()%10)*(rand()%10)*lDiffScore + 
					//	(rand()%10+1)*(rand()%(lDiffScore/10));

					//if (lScore > 0)
					//{
					//	m_pIAndroidUserItem->PerformTakeScore(lScore);
					//}
					
					IServerUserItem *pIUserItemBanker = m_pIAndroidUserItem->GetMeUserItem();
					if (pIUserItemBanker->GetUserScore() > m_lBankerCondition) 
					{
						//��������ׯ
						m_nBankerCount = 0;
						m_stlApplyBanker++;
						m_pIAndroidUserItem->SetGameTimer(IDI_REQUEST_BANKER, (rand() % m_cbTimeLeave) + 1);
					}
				}
			}


			return false;
		}
	case IDI_REQUEST_BANKER:	//������ׯ
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);

			m_pIAndroidUserItem->SendSocketData(SUB_C_APPLY_BANKER);

			return false;
		}
	case IDI_GIVEUP_BANKER:		//������ׯ
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);

			m_pIAndroidUserItem->SendSocketData(SUB_C_CANCEL_BANKER);

			return false;
		}
	case IDI_BANK_OPERATE:		//���в���
		{
			m_pIAndroidUserItem->KillGameTimer(nTimerID);
						//���в���
			BankOperate(2);
			////��������
			//IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			//SCORE lRobotScore = pUserItem->GetUserScore();			
			//{

			//	//�жϴ�ȡ
			//	if (lRobotScore > m_lRobotScoreRange[1])
			//	{
			//		SCORE lSaveScore=0L;

			//		lSaveScore = SCORE(lRobotScore*m_nRobotBankStorageMul/100);
			//		if (lSaveScore > lRobotScore)  lSaveScore = lRobotScore;

			//		if (lSaveScore > 0 && m_wCurrentBanker != m_pIAndroidUserItem->GetChairID())
			//			m_pIAndroidUserItem->PerformSaveScore(lSaveScore);

			//	}
			//	else if (lRobotScore < m_lRobotScoreRange[0])
			//	{
			//		LONGLONG lDiffScore = (m_lRobotBankGetScoreMax-m_lRobotBankGetScoreMin)/100;
			//		SCORE lScore = m_lRobotBankGetScoreMin + (m_pIAndroidUserItem->GetUserID()%10)*(rand()%10)*lDiffScore + 
			//			(rand()%10+1)*(rand()%(lDiffScore/10));
			//		if (lScore > 0)
			//			m_pIAndroidUserItem->PerformTakeScore(lScore);								
			//	}
			//}
			return false;
		}
	default:
		{
			if (nTimerID >= IDI_PLACE_JETTON && nTimerID <= IDI_PLACE_JETTON+MAX_CHIP_TIME)
			{
				//�Գ���ע
				int	nAreaChance[AREA_COUNT] = {0};	//������
				SCORE nMaxBet[AREA_COUNT] = {0};	//���������ע
				CopyMemory( nAreaChance, m_RobotInfo.nAreaChance, sizeof(nAreaChance));

				//�ж������ע
				for( int i = 0 ; i < AREA_COUNT; ++i )
				{
					nMaxBet[i] = GetMaxPlayerScore(i);
					if( nMaxBet[i] < 100 )
					{
						nAreaChance[i] = 0;
					}
				}
				
				//�������
				int nACTotal = 0;
				int nRandNum = 0;
				int nChipArea = 0;
				for (int i = 0; i < AREA_COUNT; i++)
					nACTotal += nAreaChance[i];
				static int nStFluc = 1;				//�������
				if (nACTotal > 0)	
				{
					//nRandNum = (rand()+nStFluc*3) % nACTotal;
					nRandNum = rand() % nACTotal;
					for (int i = 0; i < AREA_COUNT; i++)
					{
						nRandNum -= nAreaChance[i];
						if (nRandNum <= 0)
						{
							nChipArea = i;
							break;
						}
					}
					//if(nChipArea > 5)
					//	int  nn = 10;
					//nStFluc = nStFluc + 2 + rand()%3;

					//�����ע
					SCORE lBetScore = 0;

					if( nMaxBet[nChipArea] < m_lRobotBetLimit[0] )
					{
						for( int i = CountArray(m_RobotInfo.nChip) - 1; i >= 0 ; i-- )
						{
							if( nMaxBet[nChipArea] >= m_RobotInfo.nChip[i] )
							{
								lBetScore = m_RobotInfo.nChip[i];
								break;
							}
						}
					}
					else if( nMaxBet[nChipArea] == m_lRobotBetLimit[0] )
					{
						lBetScore = nMaxBet[nChipArea];
					}
					else
					{
						int nCurChip[2] = {0 , CountArray(m_RobotInfo.nChip) - 1};
						for( int i = 0 ; i < CountArray(m_RobotInfo.nChip); ++i )
						{
							if( m_lRobotBetLimit[0] == m_RobotInfo.nChip[i] )
								nCurChip[0] = i;
							if( m_lRobotBetLimit[1] == m_RobotInfo.nChip[i] )
								nCurChip[1] = i;
						}
						if( nCurChip[1] == nCurChip[0] )
							lBetScore = m_RobotInfo.nChip[nCurChip[0]];
						else
							lBetScore = m_RobotInfo.nChip[ (rand()%(nCurChip[1] - nCurChip[0]+1)) + nCurChip[0]];
					}

					if( lBetScore > 0 )
					{
						//��������
						CMD_C_PlaceBet PlaceBet = {};

						//�������
						PlaceBet.cbBetArea = nChipArea;	
						PlaceBet.lBetScore = lBetScore;

						//������Ϣ
						m_pIAndroidUserItem->SendSocketData(SUB_C_PLACE_JETTON, &PlaceBet, sizeof(PlaceBet));

						
					}
				}
			}

			m_pIAndroidUserItem->KillGameTimer(nTimerID);
			return false;
		}
	}
	return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_FREE:			//��Ϸ���� 
		{
			return OnSubGameFree(pBuffer, wDataSize);
		}
	case SUB_S_GAME_START:			//��Ϸ��ʼ
		{
			return OnSubGameStart(pBuffer, wDataSize);
		}
	case SUB_S_PLACE_JETTON:		//�û���ע
		{
			return OnSubPlaceBet(pBuffer, wDataSize);
		}
	case SUB_S_APPLY_BANKER:		//������ׯ 
		{
			return OnSubUserApplyBanker(pBuffer,wDataSize);
		}
	case SUB_S_CANCEL_BANKER:		//ȡ����ׯ 
		{
			return OnSubUserCancelBanker(pBuffer,wDataSize);
		}
	case SUB_S_CHANGE_BANKER:		//�л�ׯ�� 
		{
			return OnSubChangeBanker(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:			//��Ϸ���� 
		{
			return OnSubGameEnd(pBuffer, wDataSize);
		}
	case SUB_S_SEND_RECORD:			//��Ϸ��¼ (����)
		{
			return true;
		}
	case SUB_S_PLACE_JETTON_FAIL:	//��עʧ�� (����)
		{
			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:			//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��Ϣ����
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			m_lUserLimitScore = pStatusFree->lPlayFreeSocre;
			m_lAreaLimitScore = pStatusFree->lAreaLimitScore;
			m_lBankerCondition = pStatusFree->lApplyBankerCondition;

			memcpy(m_szRoomName, pStatusFree->szGameRoomName, sizeof(m_szRoomName));

			ReadConfigInformation(&pStatusFree->CustomAndroid);
						//���в���
			BankOperate(2);
			return true;
		}
	case GAME_SCENE_BET:		//��Ϸ״̬
	case GAME_SCENE_END:		//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//��Ϣ����
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//ׯ����Ϣ
			m_wCurrentBanker = pStatusPlay->wBankerUser;

			m_lUserLimitScore = pStatusPlay->lPlayBetScore;
			m_lAreaLimitScore = pStatusPlay->lAreaLimitScore;
			m_lBankerCondition = pStatusPlay->lApplyBankerCondition;

			memcpy(m_szRoomName, pStatusPlay->szGameRoomName, sizeof(m_szRoomName));

			ReadConfigInformation(&pStatusPlay->CustomAndroid);
						//���в���
			BankOperate(2);
			return true;
		}
	}

	return true;
}

//�û�����
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}



//��Ϸ����
bool CAndroidUserItemSink::OnSubGameFree(const void * pBuffer, WORD wDataSize)
{
	//��Ϣ����
	CMD_S_GameFree* pGameFree=(CMD_S_GameFree *)pBuffer;

	m_cbTimeLeave = pGameFree->cbTimeLeave;
	 m_nListUserCount = pGameFree->nListUserCount;

	bool bMeGiveUp = false;
	if (m_wCurrentBanker == m_pIAndroidUserItem->GetChairID())
	{
		m_nBankerCount++;
		if ( m_nBankerCount >= m_nRobotBankerCount )
		{
			//��������ׯ
			m_nBankerCount = 0;
			m_pIAndroidUserItem->SetGameTimer(IDI_GIVEUP_BANKER, rand()%2 + 1);

			bMeGiveUp = true;
		}
	}

	//�����ׯ
	if (m_wCurrentBanker != m_pIAndroidUserItem->GetChairID() || bMeGiveUp)
	{
		m_cbTimeLeave = pGameFree->cbTimeLeave - 3;
		m_pIAndroidUserItem->SetGameTimer(IDI_CHECK_BANKER, 3);
	}


	//���в���
	if (pGameFree->cbTimeLeave > 2&& m_wCurrentBanker != m_pIAndroidUserItem->GetChairID())
		m_pIAndroidUserItem->SetGameTimer(IDI_BANK_OPERATE, (rand() % (pGameFree->cbTimeLeave-1)) + 1);

	return true;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{	
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//��Ϣ����
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//�Լ���ׯ������ע������
	if (pGameStart->wBankerUser == m_pIAndroidUserItem->GetChairID() || pGameStart->nChipRobotCount <= 0)
		return true;

	//���ñ���
	m_lAndroidBet = 0;
	m_nListUserCount = pGameStart->nListUserCount;
	m_lMaxChipBanker = pGameStart->lBankerScore/m_RobotInfo.nMaxTime;

	CString strO = m_pIAndroidUserItem->GetMeUserItem()->GetNickName();
	m_lMaxChipUser = pGameStart->lPlayBetScore/m_RobotInfo.nMaxTime;
	m_wCurrentBanker = pGameStart->wBankerUser;
	m_lBankerScore = pGameStart->lBankerScore;
	m_nChipTimeCount = 0;
	ZeroMemory(m_nChipLimit, sizeof(m_nChipLimit));
	ZeroMemory(m_lAllBet,sizeof(m_lAllBet));

	if(pGameStart->nAndriodCount > 0)
		m_stlApplyBanker=pGameStart->nAndriodCount;
	else
		m_stlApplyBanker=0;

	for (int i = 0; i < AREA_COUNT; i++)
		m_lAreaChip[i] = m_lAreaLimitScore;

	ASSERT(m_lMaxChipUser >= 0);

	//������ע����
	int nElapse = 0;												
	WORD wMyID = m_pIAndroidUserItem->GetChairID();

	if (m_nRobotBetTimeLimit[0] == m_nRobotBetTimeLimit[1])
		m_nChipTime = m_nRobotBetTimeLimit[0];
	else
		m_nChipTime = (rand()+wMyID)%(m_nRobotBetTimeLimit[1]-m_nRobotBetTimeLimit[0]+1) + m_nRobotBetTimeLimit[0];
	ASSERT(m_nChipTime>=0);		
	if (m_nChipTime <= 0)	return false;								//��ȷ,2����������
	if (m_nChipTime > MAX_CHIP_TIME)	m_nChipTime = MAX_CHIP_TIME;	//�޶�MAX_CHIP����ע

	//���㷶Χ
	//if (!CalcBetRange(__min(m_lMaxChipBanker, m_lMaxChipUser), m_lRobotBetLimit, m_nChipTime, m_nChipLimit))
	//	return true;

	//����ʱ��
	int nTimeGrid = int(pGameStart->cbTimeLeave-2)*800/m_nChipTime;		//ʱ���,ǰ2�벻��ע,����-2,800��ʾ��������עʱ�䷶Χǧ�ֱ�
	for (int i = 0; i < m_nChipTime; i++)
	{
		int nRandRage = int( nTimeGrid * i / (1500*sqrt((double)m_nChipTime)) ) + 1;		//������Χ
		nElapse = 2 + (nTimeGrid*i)/1000 + ( (rand()+wMyID)%(nRandRage*2) - (nRandRage-1) );
		ASSERT(nElapse>=2&&nElapse<=pGameStart->cbTimeLeave);
		if (nElapse < 2 || nElapse > pGameStart->cbTimeLeave)	continue;
		
		m_pIAndroidUserItem->SetGameTimer(IDI_PLACE_JETTON+i+1, nElapse);
	}

	return true;
}

//�û���ע
bool CAndroidUserItemSink::OnSubPlaceBet(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_PlaceBet));
	if (wDataSize!=sizeof(CMD_S_PlaceBet)) return false;

	//��Ϣ����
	CMD_S_PlaceBet * pPlaceBet=(CMD_S_PlaceBet *)pBuffer;

	//��ӳ���
	if( pPlaceBet->wChairID == m_pIAndroidUserItem->GetChairID())
		m_lAndroidBet += pPlaceBet->lBetScore;

	m_lAllBet[pPlaceBet->cbBetArea] += pPlaceBet->lBetScore;

	//���ñ���
	m_lMaxChipBanker -= pPlaceBet->lBetScore;

	//�Գ����
	m_lAreaChip[pPlaceBet->cbBetArea] -= pPlaceBet->lBetScore;

	//if (pPlaceBet->wChairID == m_pIAndroidUserItem->GetChairID())
	//	m_lMaxChipUser -= pPlaceBet->lBetScore;

	ASSERT(m_lMaxChipUser >= 0);

	return true;
}

//��עʧ��
bool CAndroidUserItemSink::OnSubPlaceBetFail(const void * pBuffer, WORD wDataSize)
{
	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//��Ϣ����
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	m_stnApplyCount = 0;

	return true;
}

//������ׯ
bool CAndroidUserItemSink::OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) return false;

	//��Ϣ����
	CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;

	//�Լ��ж�
	if (m_pIAndroidUserItem->GetChairID()==pApplyBanker->wApplyUser) 
		m_bMeApplyBanker = true;

	return true;
}

//ȡ����ׯ
bool CAndroidUserItemSink::OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_CancelBanker));
	if (wDataSize!=sizeof(CMD_S_CancelBanker)) return false;

	//��Ϣ����
	CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;

	//�Լ��ж�
	if ( m_pIAndroidUserItem->GetMeUserItem()->GetChairID() == pCancelBanker->wCancelUser ) 
		m_bMeApplyBanker = false;

	return true;
}

//�л�ׯ��
bool CAndroidUserItemSink::OnSubChangeBanker(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
	if (wDataSize!=sizeof(CMD_S_ChangeBanker)) return false;

	//��Ϣ����
	CMD_S_ChangeBanker * pChangeBanker = (CMD_S_ChangeBanker *)pBuffer;

	if ( m_wCurrentBanker == m_pIAndroidUserItem->GetChairID() && m_wCurrentBanker != pChangeBanker->wBankerUser )
	{
		//m_stlApplyBanker--;
		m_nWaitBanker = 0;
		m_bMeApplyBanker = false;
	}
	m_wCurrentBanker = pChangeBanker->wBankerUser;

	return true;
}

//��ȡ����
void CAndroidUserItemSink::ReadConfigInformation(tagCustomAndroid *pCustomAndroid)
{
	//��������
	m_nRobotBetTimeLimit[0] = pCustomAndroid->lRobotMinBetTime;
	m_nRobotBetTimeLimit[1] = pCustomAndroid->lRobotMaxBetTime;
	if (m_nRobotBetTimeLimit[0] < 0) m_nRobotBetTimeLimit[0] = 0;
	if (m_nRobotBetTimeLimit[1] < m_nRobotBetTimeLimit[0]) m_nRobotBetTimeLimit[1] = m_nRobotBetTimeLimit[0];

	//��������
	m_lRobotBetLimit[0] = pCustomAndroid->lRobotMinJetton;
	m_lRobotBetLimit[1] = pCustomAndroid->lRobotMaxJetton;
	if (m_lRobotBetLimit[1] > 500)					m_lRobotBetLimit[1] = 500;
	if (m_lRobotBetLimit[0] < 1)						m_lRobotBetLimit[0] = 1;
	if (m_lRobotBetLimit[1] < m_lRobotBetLimit[0])	m_lRobotBetLimit[1] = m_lRobotBetLimit[0];

	//�Ƿ���ׯ
	m_bRobotBanker = (pCustomAndroid->nEnableRobotBanker == TRUE)?true:false;

	//��ׯ����
	LONGLONG lRobotBankerCountMin = pCustomAndroid->lRobotBankerCountMin;
	LONGLONG lRobotBankerCountMax = pCustomAndroid->lRobotBankerCountMax;
	m_nRobotBankerCount = rand()%(lRobotBankerCountMax-lRobotBankerCountMin+1) + lRobotBankerCountMin;
	
	//�б�����
	m_nRobotListMinCount = pCustomAndroid->lRobotListMinCount;
	m_nRobotListMaxCount = pCustomAndroid->lRobotListMaxCount;
	
	//������
	m_nRobotApplyBanker = pCustomAndroid->lRobotApplyBanker;

	//��������
	m_nRobotWaitBanker = pCustomAndroid->lRobotWaitBanker;

	//��������
	m_lRobotScoreRange[0] = pCustomAndroid->lRobotScoreMin;
	m_lRobotScoreRange[1] = pCustomAndroid->lRobotScoreMax;
	if (m_lRobotScoreRange[1] < m_lRobotScoreRange[0])	m_lRobotScoreRange[1] = m_lRobotScoreRange[0];

	//�������
	m_lRobotBankGetScoreMin = pCustomAndroid->lRobotBankGetMin;
	m_lRobotBankGetScoreMax = pCustomAndroid->lRobotBankGetMax;

	//������� (ׯ��)
	m_lRobotBankGetScoreBankerMin = pCustomAndroid->lRobotBankGetBankerMin;
	m_lRobotBankGetScoreBankerMax = pCustomAndroid->lRobotBankGetBankerMax;

	//����
	m_nRobotBankStorageMul = pCustomAndroid->lRobotBankStoMul;
	if (m_nRobotBankStorageMul<0 || m_nRobotBankStorageMul>100) m_nRobotBankStorageMul = 20;
	
	//������
	//CopyMemory(m_RobotInfo.nAreaChance,pCustomAndroid->nAreaChance,sizeof(m_RobotInfo.nAreaChance));
	return;

}

//���㷶Χ	(����ֵ��ʾ�Ƿ����ͨ���½����޴ﵽ��ע)
bool CAndroidUserItemSink::CalcBetRange(SCORE lMaxScore, SCORE lChipLmt[], int & nChipTime, int lJetLmt[])
{
	//�������
	bool bHaveSetMinChip = false;

	//����һע
	if (lMaxScore < m_RobotInfo.nChip[0])	return false;

	//���÷�Χ
	for (int i = 0; i < CountArray(m_RobotInfo.nChip); i++)
	{
		if (!bHaveSetMinChip && m_RobotInfo.nChip[i] >= lChipLmt[0])
		{ 
			lJetLmt[0] = i;
			bHaveSetMinChip = true;
		}
		if (m_RobotInfo.nChip[i] <= lChipLmt[1])
			lJetLmt[1] = i;
	}
	if (lJetLmt[0] > lJetLmt[1])	lJetLmt[0] = lJetLmt[1];

	//�Ƿ񽵵�����
	if (m_bReduceBetLimit)
	{
		if (nChipTime * m_RobotInfo.nChip[lJetLmt[0]] > lMaxScore)
		{
			//�Ƿ񽵵���ע����
			if (nChipTime * m_RobotInfo.nChip[0] > lMaxScore)
			{
				nChipTime = int(lMaxScore/m_RobotInfo.nChip[0]);
				lJetLmt[0] = 0;
				lJetLmt[1] = 0;
			}
			else
			{
				//���͵���������
				while (nChipTime * m_RobotInfo.nChip[lJetLmt[0]] > lMaxScore)
				{
					lJetLmt[0]--;
					ASSERT(lJetLmt[0]>=0);
				}
			}
		}
	}

	return true;
}


//�����ע
SCORE CAndroidUserItemSink::GetMaxPlayerScore( BYTE cbBetArea )
{	
	if( cbBetArea >= AREA_MAX )
		return 0L;

	//����ע��
	SCORE lNowBet = m_lAndroidBet;

	//ׯ�ҽ��
	SCORE lBankerScore = -1;

	//������
	BYTE cbMultiple[AREA_MAX] = {MULTIPLE_XIAN, MULTIPLE_PING, MULTIPLE_ZHUANG, 
		MULTIPLE_XIAN_TIAN, MULTIPLE_ZHUANG_TIAN, MULTIPLE_TONG_DIAN, 
		MULTIPLE_XIAN_PING, MULTIPLE_ZHUANG_PING};

	//������Ӯ
	BYTE cbArae[4][4] = {	{ AREA_XIAN_DUI,	255,			AREA_MAX,			AREA_MAX }, 
	{ AREA_ZHUANG_DUI,	255,			AREA_MAX,			AREA_MAX }, 
	{ AREA_XIAN,		AREA_PING,		AREA_ZHUANG,		AREA_MAX },  
	{ AREA_XIAN_TIAN,	AREA_TONG_DUI,	AREA_ZHUANG_TIAN,	255 }};
	//�����趨
	for ( int nTopL = 0; nTopL < 4; ++nTopL )
	{
		if( cbArae[0][nTopL] == AREA_MAX )
			continue;

		for ( int nTopR = 0; nTopR < 4; ++nTopR )
		{
			if( cbArae[1][nTopR] == AREA_MAX )
				continue;

			for ( int nCentral = 0; nCentral < 4; ++nCentral )
			{
				if( cbArae[2][nCentral] == AREA_MAX )
					continue;

				for ( int nBottom = 0; nBottom < 4; ++nBottom )
				{
					if( cbArae[3][nBottom] == AREA_MAX )
						continue;

					BYTE cbWinArea[AREA_MAX] = {FALSE};

					//ָ����ʤ����
					if ( cbArae[0][nTopL] != 255 && cbArae[0][nTopL] != AREA_MAX )
						cbWinArea[cbArae[0][nTopL]] = TRUE;

					if ( cbArae[1][nTopR] != 255 && cbArae[1][nTopR] != AREA_MAX )
						cbWinArea[cbArae[1][nTopR]] = TRUE;

					if ( cbArae[2][nCentral] != 255 && cbArae[2][nCentral] != AREA_MAX )
						cbWinArea[cbArae[2][nCentral]] = TRUE;

					if ( cbArae[3][nBottom] != 255 && cbArae[3][nBottom] != AREA_MAX )
						cbWinArea[cbArae[3][nBottom]] = TRUE;

					//ѡ������Ϊ���ʤ����ͬ�ȼ�������������Ϊ����䡣�Եó������עֵ
					for ( int i = 0; i < 4; i++ )
					{
						for ( int j = 0; j < 4; j++ )
						{
							if ( cbArae[i][j] == cbBetArea )
							{
								for ( int n = 0; n < 4; ++n )
								{
									if ( cbArae[i][n] != 255 && cbArae[i][n] != AREA_MAX )
									{
										cbWinArea[cbArae[i][n]] = FALSE;
									}
								}
								cbWinArea[cbArae[i][j]] = TRUE;
							}
						}
					}

					SCORE lScore = m_lBankerScore;
					for (int nAreaIndex = 0; nAreaIndex < AREA_MAX; ++nAreaIndex ) 
					{
						if ( cbWinArea[nAreaIndex] == TRUE )
						{
							lScore -= m_lAllBet[nAreaIndex]*(cbMultiple[nAreaIndex] - 1);
						}
						else if ( cbWinArea[AREA_PING] == TRUE && ( nAreaIndex == AREA_XIAN || nAreaIndex == AREA_ZHUANG ) )
						{

						}
						else
						{
							lScore += m_lAllBet[nAreaIndex];
						}
					}
					if ( lBankerScore == -1 )
						lBankerScore = lScore;
					else
						lBankerScore = min(lBankerScore, lScore);
				}
			}
		}
	}

	//�����ע
	SCORE lMaxBet = 0L;

	ASSERT(m_lMaxChipUser >= 0);

	//�����ע
	lMaxBet = min( m_lMaxChipUser - lNowBet, m_lAreaLimitScore - m_lAndroidBet);

	lMaxBet = min( m_lMaxChipUser - lNowBet, m_lAreaLimitScore - m_lAllBet[cbBetArea]);

	lMaxBet = min( lMaxBet, lBankerScore / (cbMultiple[cbBetArea] - 1));

	//��������
	lMaxBet = max(lMaxBet, 0);

	return lMaxBet;
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
			NcaTextOut(strInfo);
			return;
		}
	}

	//��������
	SCORE lRobotScore = pUserItem->GetUserScore();

	{
		CString strInfo;
		strInfo.Format(TEXT("[%s] ����(%.2lf)"),pUserItem->GetNickName(),lRobotScore);

		if (lRobotScore > m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)
		{
			CString strInfo1;
			strInfo1.Format(TEXT("����������(%.2lf)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore);
			strInfo+=strInfo1;

			NcaTextOut(strInfo);
		}
		else if (lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)
		{
			CString strInfo1;
			strInfo1.Format(TEXT("����ȡ������(%.2lf)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore);
			strInfo+=strInfo1;

			NcaTextOut(strInfo);
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
			strInfo.Format(TEXT("[%s] ִ�д����ǰ���(%.2lf)��������(%.2lf)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			NcaTextOut(strInfo);
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
			strInfo.Format(TEXT("[%s] ִ��ȡ�ȡ��ǰ���(%.2lf)��ȡ�����(%.2lf)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			NcaTextOut(strInfo);
		}
	}

	//IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
	//if(pUserItem->GetUserStatus()>=US_SIT)
	//{
	//	if(cbType==1)
	//	{
	//		CString strInfo;
	//		strInfo.Format(TEXT("������״̬���ԣ���ִ�д�ȡ��"));
	//		NcaTextOut(strInfo);
	//		return;
	//	}
	//}

	////��������
	//SCORE lRobotScore = pUserItem->GetUserScore();

	//{
	//	CString strInfo;
	//	strInfo.Format(TEXT("[%s] ����(%.2lf)"),pUserItem->GetNickName(),lRobotScore);

	//	if (lRobotScore > m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)
	//	{
	//		CString strInfo1;
	//		strInfo1.Format(TEXT("����������(%.2lf)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore);
	//		strInfo+=strInfo1;

	//		NcaTextOut(strInfo);
	//	}
	//	else if (lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)
	//	{
	//		CString strInfo1;
	//		strInfo1.Format(TEXT("����ȡ������(%.2lf)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore);
	//		strInfo+=strInfo1;

	//		NcaTextOut(strInfo);
	//	}

	//		//�жϴ�ȡ
	//	if (lRobotScore >m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore || lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)
	//	{			
	//		SCORE lSaveScore=0L;

	//		//if (lSaveScore > 0) 
	//		
	//			SCORE lTakeScore = (m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore - m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)/10;
	//			lSaveScore=(SCORE)(m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore+(rand()%10)*lTakeScore+(rand()%1000)*lTakeScore/1000);

	//			m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
	//		
	//		SCORE lRobotNewScore = pUserItem->GetUserScore();

	//		CString strInfo;
	//		strInfo.Format(TEXT("[%s] ִ�д����ǰ���(%.2lf)��������(%.2lf)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

	//		NcaTextOut(strInfo);
	//	}
	///*	else if (lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lMinTakeScore)
	//	{
	//		CString strInfo;
	//		SCORE lScore=m_pIAndroidUserItem->GetAndroidParameter()->lMinTakeScore-lRobotScore;
	//		if (lScore > 0)
	//		{
	//			lScore += (rand()%(int)(m_pIAndroidUserItem->GetAndroidParameter()->lMaxTakeScore - m_pIAndroidUserItem->GetAndroidParameter()->lMinTakeScore));
	//			m_pIAndroidUserItem->PerformTakeScore(lScore);
	//		}

	//		SCORE lRobotNewScore = pUserItem->GetUserScore();
	//		strInfo.Format(TEXT("[%s] ִ��ȡ�ȡ��ǰ���(%.2lf)��ȡ�����(%.2lf)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

	//		NcaTextOut(strInfo);
	//	}*/
	//}
}
//////////////////////////////////////////////////////////////////////////
