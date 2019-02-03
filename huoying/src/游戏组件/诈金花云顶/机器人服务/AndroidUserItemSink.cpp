#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "AESEncrypt.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////

//����ʱ��
#define TIME_LESS						3									//����ʱ��
#define TIME_DELAY_TIME					3									//��ʱʱ��

//��Ϸʱ��
#define TIME_START_GAME					4									//��ʼʱ��
#define TIME_USER_ADD_SCORE				4									//��עʱ��
#define TIME_USER_COMPARE_CARD			4									//����ʱ��
#define	TIME_USER_FINISH_FLASH			4									//����ʱ��

//��������ԭ��
unsigned char chCommonEncryption[AESENCRYPTION_LENGTH] = 
{
	0x32, 0x43, 0xf6, 0xa8, 
	0x88, 0x5a, 0x30, 0x8d, 
	0x31, 0x31, 0x98, 0xa2, 
	0xe0, 0x37, 0x07, 0x34
};

//��Ϸʱ��
#define IDI_START_GAME					(100)								//��ʼ��ʱ��
#define IDI_USER_ADD_SCORE				(101)								//��ע��ʱ��
#define IDI_USER_COMPARE_CARD			(102)								//���ƶ�ʱ��
#define IDI_DELAY_TIME					(103)								//��ʱ��ʱ��
#define IDI_USER_FINISH_FLASH			(104)								//������ʱ��
#define IDI_ANDROID_RANDLOOKCARD		(105)								//�������
#define IDI_ANDROID_RANDUPCARD			(106)								//�������
#define IDI_ANDROID_UP					(107)								//�������
//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//��Ϸ����
	m_lStockScore = 0;
	m_wBankerUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	m_cbCardType = 0;

	//��ע��Ϣ
	m_bMingZhu = false;
	m_lCellScore = 0;
	m_lMaxCellScore = 0;
	m_lUserMaxScore = 0;
	m_lCurrentTimes = 1;
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));

	//������
	m_nRobotBankStorageMul=0;
	m_lRobotBankGetScore=0;
	m_lRobotBankGetScoreBanker=0;
	ZeroMemory(m_lRobotScoreRange, sizeof(m_lRobotScoreRange));

	//��Ϸ״̬
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));

	//�û��˿�
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbAllHandCardData, sizeof(m_cbAllHandCardData));

	//�ӿڱ���
	m_pIAndroidUserItem=NULL;
	srand((unsigned)time(NULL));   
	
	ZeroMemory(m_chUserAESKey, sizeof(m_chUserAESKey));
	m_bAndroidControl = false;
	m_bGiveUp = false;
	m_wAndroidPour = 0;
	m_wAndroidPourNumber = 0;
	m_dwCompareState = 0;

	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	swprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\ZJHConfig.ini"),szPath);

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
	m_lStockScore = 0;
	m_wBankerUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	m_cbCardType = 0;

	//��ע��Ϣ
	m_bMingZhu = false;
	m_lCellScore = 0;
	m_lMaxCellScore = 0;
	m_lUserMaxScore = 0;
	m_lCurrentTimes = 1;
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));

	//��Ϸ״̬
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));

	//�û��˿�
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbAllHandCardData, sizeof(m_cbAllHandCardData));
	m_bAndroidControl = false;
	m_wAndroidPour = 0;
	m_wAndroidPourNumber = 0;
	m_dwCompareState = 0;

	return true;
}
BYTE CAndroidUserItemSink::IsOpenBrand()
{
	SCORE lAllABetScore = 0;
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		lAllABetScore += m_lTableScore[i];
	}
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	BYTE cbCardTybe =  m_GameLogic.GetCardType(m_cbAllHandCardData[wMeChairID], 3);
	if (cbCardTybe == CT_BAO_ZI)
	{
		if (m_nZhaDanBetMin <= lAllABetScore && lAllABetScore <= m_nZhaDanBetMax && rand()%100 < m_nZhaDanProbability)
		{
			return 0;
		}
		else if (lAllABetScore >= m_nZhaDanBetMax)
		{
			return 2;
		}
		else return 1;
	}
	else if (cbCardTybe == CT_SHUN_JIN)
	{
		if (m_nShunJinBetMin <= lAllABetScore && lAllABetScore <= m_nShunJinBetMax && rand()%100 < m_nShunJinProbability)
		{
			return 0;
		}
		else if (lAllABetScore >= m_nShunJinBetMax)
		{
			return 2;
		}
		else return 1;
	}
	else if (cbCardTybe == CT_JIN_HUA)
	{
		if (m_nJInHuaBetMin <= lAllABetScore && lAllABetScore <= m_nJInHuaBetMax && rand()%100 < m_nJInHuaProbability)
		{
			return 0;
		}
		else if (lAllABetScore >= m_nJInHuaBetMax)
		{
			return 2;
		}
		else return 1;
	}
	else if (cbCardTybe == CT_SHUN_ZI)
	{
		if (m_nShunZiBetMin <= lAllABetScore && lAllABetScore <= m_nShunZiBetMax && rand()%100 < m_nShunZiProbability)
		{
			return 0;
		}
		else if (lAllABetScore >= m_nShunZiBetMax)
		{
			return 2;
		}
		else return 1;
	}
	else if (cbCardTybe == CT_DOUBLE)
	{
		if ( rand()%100 < m_nDuiziProbability)
		{
			return 0;
		}
		else return 1;
	}
	else if (cbCardTybe == CT_SINGLE)
	{
		if (rand()%100 < m_nSanPaiProbability)
		{
			return 0;
		}
		else return 1;
	}
	return 0;
}
bool CAndroidUserItemSink::ControlBottompour()
{
	//�������
	bool bGiveUp = false;
	BYTE cbProbability = 0;
	SCORE lCurrentScore = 0;
	SCORE lAndroidMaxScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	//�������
	BYTE cbPlayerCount = 0;
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i] == TRUE) cbPlayerCount++;
	}


	//�״β���
	if (m_lTableScore[wMeChairID] - m_lCellScore < DOUBLE_DOUBLE_X)
	{
		//�������
		cbProbability = rand()%(10);

		//���Ƹ���
		if (m_bMingZhu == false)
		{
			if (cbProbability < LOOK_CARD)
			{
				m_bMingZhu = true;
				m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);

				return true;
			}
		}
		else
		{
			//ɢ�Ʒ���
			if (m_cbCardType == 0 && wMeChairID != EstimateWinner())
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

				return true;
			}
			else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner())
			{
				//��������
				if (cbProbability < GIVE_UP)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

					return true;
				}
			}
		}

		//���ּ�ע
		ASSERT(m_lCurrentTimes <= 3 && m_lCurrentTimes >= 0);
		SCORE iAddTimes = GetAddScoreTimes();
		lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;
		//��ע�ӱ�
		if (m_bMingZhu == true) lCurrentScore *= 2;

		//������Ϣ
		CMD_C_AddScore AddScore;
		AddScore.wState = FALSE;
		AddScore.lScore = lCurrentScore;
		//if ( 0 == iAddTimes)
		//{
		//	AddScore.cbState = FOLLOW_STATUS;
		//} 
		//else
		//{
		//	AddScore.cbState = ADD_STATUS;
		//}

		m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
		m_wAndroidPour++;
		return true;
	}
	else
	{
		//�Ƿ���
		if (m_bMingZhu == false)
		{
			//�������
			cbProbability = rand()%(10);

			//���Ƹ���
			if (cbProbability < 2)
			{
				m_bMingZhu = true;
				m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);

				return true;
			}

			//�û���ע
			if (m_lTableScore[wMeChairID] < 2 * m_lMaxCellScore && (4 * cbPlayerCount * m_lMaxCellScore + m_lCellScore + m_lTableScore[wMeChairID]) < lAndroidMaxScore)
			{
				//��ע����
				ASSERT(m_lCurrentTimes <= 3 && m_lCurrentTimes >= 0);
				SCORE iAddTimes = GetAddScoreTimes();
				lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

				//������Ϣ
				CMD_C_AddScore AddScore;
				AddScore.wState = FALSE;
				AddScore.lScore = lCurrentScore;
				//if ( 0 == iAddTimes)
				//{
				//	AddScore.cbState = FOLLOW_STATUS;
				//} 
				//else
				//{
				//	AddScore.cbState = ADD_STATUS;
				//}

				m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
				m_wAndroidPour++;
				return true;
			}
			//���Ƹ���
			else
			{
				//��ע����
				lCurrentScore = m_lCurrentTimes*m_lCellScore * 2;

				//������Ϣ
				CMD_C_AddScore AddScore;
				AddScore.wState = TRUE;
				AddScore.lScore = lCurrentScore;
				//AddScore.cbState = COMPARE_STATUS;
				m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
				m_wAndroidPour++;
				//������Ϣ
				CMD_C_CompareCard CompareCard;
				ZeroMemory(&CompareCard, sizeof(CompareCard));

				//�ж�����
				if (cbPlayerCount > 2)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
					//���ö�ʱ��
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
				}
				else
				{
					//�������
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{
						if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
						{
							CompareCard.wCompareUser = i;
							break;
						}
					}
					printf("�����˱���%d\n",__LINE__);
					m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
				}

				return true;
			}
		}
		else //����
		{
			//�������
			cbProbability = rand()%(10);

			//ɢ�Ʒ���
			if (m_cbCardType == 0 && wMeChairID != EstimateWinner())
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

				return true;
			}
			else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner())
			{
				//��������
				if (cbProbability < GIVE_UP)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

					return true;
				}
			}

			//��ע����
			if ((m_wAndroidPour < m_wAndroidPourNumber) && (4 * cbPlayerCount * m_lMaxCellScore + m_lCellScore + m_lTableScore[wMeChairID]) < lAndroidMaxScore)
			{
				//��ע����
				ASSERT(m_lCurrentTimes <= 3 && m_lCurrentTimes >= 0);

				//��ע
				SCORE iAddTimes = GetAddScoreTimes();
				lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

				//��ע�ӱ�
				if (m_bMingZhu == true) lCurrentScore *= 2;

				//������Ϣ
				CMD_C_AddScore AddScore;
				AddScore.wState = FALSE;
				AddScore.lScore = lCurrentScore;
				//if ( 0 == iAddTimes)
				//{
				//	AddScore.cbState = FOLLOW_STATUS;
				//} 
				//else
				//{
				//	AddScore.cbState = ADD_STATUS;
				//}

				m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
				m_wAndroidPour++;
				return true;
			}
			//�û�����
			else
			{
				//��ע����
				lCurrentScore = 4 * m_lCurrentTimes*m_lCellScore;

				//������Ϣ
				CMD_C_AddScore AddScore;
				AddScore.wState = TRUE;
				AddScore.lScore = lCurrentScore;
				//AddScore.cbState = COMPARE_STATUS;
				m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
				m_wAndroidPour++;
				//������Ϣ
				CMD_C_CompareCard CompareCard;
				ZeroMemory(&CompareCard, sizeof(CompareCard));

				//�ж�����
				if (cbPlayerCount > 2)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
					//���ö�ʱ��
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
				}
				else
				{
					//�������
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{
						if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
						{
							CompareCard.wCompareUser = i;
							break;
						}
					}
					printf("�����˱���%d\n",__LINE__);
					m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
				}

				return true;
			}
		}
	}
}
//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//��ʼ��ʱ��
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
			//����׼��
			m_pIAndroidUserItem->SendUserReady(NULL, 0);

			return true;
		}
	case IDI_ANDROID_RANDUPCARD:
		{
			m_cbCardType =  m_GameLogic.GetCardType(m_cbAllHandCardData[m_pIAndroidUserItem->GetChairID()], 3);
			//ɾ����ʱ��
			m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDUPCARD);
			BYTE cbUserCount = 0;
			for (int i =0;i<GAME_PLAYER;i++)
			{
				if (m_cbPlayStatus[i] == TRUE)
				{
					cbUserCount++;
				}
			}
			if(cbUserCount <= 2)return true;
			if(m_pIAndroidUserItem->GetChairID() != m_wCurrentUser && m_cbPlayStatus[m_pIAndroidUserItem->GetChairID()] == TRUE && m_wCurrentUser != INVALID_CHAIR)
			{
				if (m_bMingZhu == true)
				{
					BYTE cbProbability = 0;
					//�������
					cbProbability = rand()%(16);
					WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

					//ɢ�Ʒ���
					if (m_cbCardType == 0 && wMeChairID != EstimateWinner() && m_bMingZhu == true &&  m_dwCompareState == 0)
					{
						m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
						return true;
					}
					else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner() && m_bMingZhu == true)
					{
						//��������
						if (cbProbability < GIVE_UP&& m_dwCompareState == 0)
						{
							m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

							return true;
						}
						else
						{
							//���ö�ʱ��
							UINT nElapse= 5 + (rand() % 8);
							m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDUPCARD, nElapse);
						}
					}
					else
					{
						//���ö�ʱ��
						UINT nElapse= 5 + (rand() % 8);
						m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDUPCARD, nElapse);
					}

				}
			}
			else if(m_cbPlayStatus[m_pIAndroidUserItem->GetChairID()] == TRUE)
			{
				//���ö�ʱ��
				UINT nElapse= 5 + (rand() % 8);
				m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDUPCARD, nElapse);
			}

			return true;
		}
	case IDI_ANDROID_RANDLOOKCARD:
		{
			//ɾ����ʱ��
			m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDLOOKCARD);

			if(m_pIAndroidUserItem->GetChairID() != m_wCurrentUser && m_cbPlayStatus[m_pIAndroidUserItem->GetChairID()] == TRUE && m_wCurrentUser != INVALID_CHAIR)
			{
				BYTE cbProbability = 0;
				//�������
				cbProbability = rand()%(16);
				WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
				//���Ƹ���
				if (m_bMingZhu == false)
				{
					if (cbProbability < LOOK_CARD&& m_dwCompareState == 0)
					{

						m_bMingZhu = true;
						m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);

						//���ö�ʱ��
						UINT nElapse=2 + (rand() % 5);
						m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDUPCARD, nElapse);

						return true;
					}
					else
					{
						//���ö�ʱ��
						UINT nElapse=5 + (rand() % 15);
						m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDLOOKCARD, nElapse);
					}
				}
			}
			else if(m_cbPlayStatus[m_pIAndroidUserItem->GetChairID()] == TRUE)
			{
				//���ö�ʱ��
				UINT nElapse=5 + (rand() % 15);
				m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDLOOKCARD, nElapse);
			}
			return true;
		}
	case IDI_USER_ADD_SCORE:	//��ע��ʱ��
		{
			//ɾ����ʱ��
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);

			//����Ч��
			ASSERT(m_pIAndroidUserItem->GetChairID() == m_wCurrentUser);
			if (m_pIAndroidUserItem->GetChairID() != m_wCurrentUser) 
			{
				CString strtip;
				CTime time = CTime::GetCurrentTime();
				CString strFileName;
				strFileName.Format(TEXT("թ��OnUserAddScore.log"));
				strtip.Format(TEXT("m_pIAndroidUserItem->GetChairID() != m_wCurrentUser gameID= %d  m_pIAndroidUserItem->GetChairID() = %d  m_wCurrentUser = %d[%d-%d-%d %d:%d:%d]\n"),m_pIAndroidUserItem->GetMeUserItem()->GetGameID(), m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
				WriteInfo(strFileName, strtip);
				return false;
			}
			printf("�����˼�עIDI_USER_ADD_SCORE_m_wCurrentUser_%d,GetChairID()_%d------1111111\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
			//�������
			bool bGiveUp = false;
			BYTE cbProbability = 0;
			SCORE lCurrentScore = 0;
			SCORE lAndroidMaxScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
			WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
			m_cbCardType =  m_GameLogic.GetCardType(m_cbAllHandCardData[m_pIAndroidUserItem->GetChairID()], 3);
			//�������
			BYTE cbPlayerCount = 0;
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if (m_cbPlayStatus[i] == TRUE) cbPlayerCount++;
			}


			if (IsOpenBrand() == 2 && m_nCurrentRounds >= 2)
			{
				//��ע����
				if (m_bMingZhu == false)
				{
					lCurrentScore = 4 * m_lCurrentTimes*m_lCellScore;
				}
				else
				{
					lCurrentScore = 2 * m_lCurrentTimes*m_lCellScore;
				}


				//������Ϣ
				CMD_C_AddScore AddScore;
				AddScore.wState = TRUE;
				AddScore.lScore = lCurrentScore;
				//AddScore.cbState = COMPARE_STATUS;
				m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

				//������Ϣ
				CMD_C_CompareCard CompareCard;
				ZeroMemory(&CompareCard, sizeof(CompareCard));

				//�ж�����
				if (cbPlayerCount > 2)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
					//���ö�ʱ��
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
					printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------222222\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
				}
				else
				{
					//�������
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{ 
						if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
						{
							CompareCard.wCompareUser = i;
							break;
						}
					}

					m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
					printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------3333333\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
				}

				return true;
			}

			if(m_bAndroidControl)
			{
				ControlBottompour();
				return true;
			}
			else
			{
				if (m_bGiveUp)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
					printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------444444444\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
					m_bGiveUp = false;

					return true;
				}

				//�״β���
				if (m_lTableScore[wMeChairID] <= DIFEN_TIMES*m_lCellScore )
				{
					//�������
					cbProbability = rand()%(10);

					//���Ƹ���
					if (m_bMingZhu == false)
					{
						if (cbProbability < 7)
						{
							m_bMingZhu = true;      //����ǰ����
							m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);
							printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------4444444444\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							return true;
						}
					}
					else
					{

						if (m_cbCardType == 0 && wMeChairID != EstimateWinner())    //����
						{	
							m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
							return true;
						}
						else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner())    //ɢ�Ʒ���
						{
							//����ʷ���
							if (cbProbability < 8)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
								printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------5555555555\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								return true;
							}
						}
						else if (m_cbCardType == CT_DOUBLE && wMeChairID != EstimateWinner())
						{
							//����
							if (cbProbability < 6)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

								return true;
							}
						}
					}

					//���ּ�ע
					SCORE iAddTimes = GetAddScoreTimes();
					lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;
					//��ע�ӱ�
					if (m_bMingZhu == true) lCurrentScore *= 2;

					//������Ϣ
					CMD_C_AddScore AddScore;
					AddScore.wState = FALSE;
					AddScore.lScore = lCurrentScore;
					m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
					printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------999999999\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
					return true;
				}
				else
				{
					//�Ƿ���
					if (m_bMingZhu == false)
					{
						//�������
						cbProbability = rand()%(10);

						//���Ƹ���
						if (cbProbability < 3)
						{
							m_bMingZhu = true;
							m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);
							printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------aaaaaaaaaa\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							return true;
						}
						else if (wMeChairID != EstimateWinner() )
						{
							if (cbProbability < 7)
							{
								if (m_cbCardType <= CT_DOUBLE)    //ɢ�Ʒ���
								{
									m_bGiveUp = true;      //����ǰ����
								}
								m_bMingZhu = true;
								m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);
								printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------bbbbbbbbbb\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								return true;
							}

						}

						//�û���ע
						if (m_lTableScore[wMeChairID] + m_lCurrentTimes*m_lCellScore <= m_lUserMaxScore && m_cbCardType <= CT_DOUBLE)
						{
							//��ע����
							SCORE iAddTimes = GetAddScoreTimes();
							lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

							//������Ϣ
							CMD_C_AddScore AddScore;
							AddScore.wState = FALSE;
							AddScore.lScore = lCurrentScore;
							m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
							printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------ccccccccc\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							return true;
						}
						//���Ƹ���
						else if(m_cbCardType <= CT_DOUBLE)
						{
							//��ע����
							lCurrentScore = m_lCurrentTimes*m_lCellScore * 2;

							//������Ϣ
							CMD_C_AddScore AddScore;
							AddScore.wState = TRUE;
							AddScore.lScore = lCurrentScore;
							m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
							printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------ddddddddd\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							//������Ϣ
							CMD_C_CompareCard CompareCard;
							ZeroMemory(&CompareCard, sizeof(CompareCard));

							//�ж�����
							if (cbPlayerCount > 2)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
								//���ö�ʱ��
								UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
								m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
								return true;
								printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------eeeeeeee\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							}
							else
							{
								//�������
								for (WORD i = 0; i < GAME_PLAYER; i++)
								{
									if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
									{
										CompareCard.wCompareUser = i;
										break;
									}
								}
								m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
								return true;
								printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------fffffffff\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							}
						}
						else if (m_cbCardType > CT_DOUBLE)
						{
							BYTE cbbrand = IsOpenBrand();
							if (cbbrand == 0 || cbbrand == 2)
							{
								//��ע����
								if (m_bMingZhu == false)
								{
									lCurrentScore = 4 * m_lCurrentTimes*m_lCellScore;
								}
								else
								{
									lCurrentScore = 2 * m_lCurrentTimes*m_lCellScore;
								}


								//������Ϣ
								CMD_C_AddScore AddScore;
								AddScore.wState = TRUE;
								AddScore.lScore = lCurrentScore;
								//AddScore.cbState = COMPARE_STATUS;
								m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

								//������Ϣ
								CMD_C_CompareCard CompareCard;
								ZeroMemory(&CompareCard, sizeof(CompareCard));

								//�ж�����
								if (cbPlayerCount > 2)
								{
									m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
									//���ö�ʱ��
									UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
									m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
									return true;
									printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------qqqqqqqqqqq\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								}
								else
								{
									//�������
									for (WORD i = 0; i < GAME_PLAYER; i++)
									{ 
										if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
										{
											CompareCard.wCompareUser = i;
											break;
										}
									}

									m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
									return true;
									printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------ttttttttttt\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								}
								return true;
							}
							else
							{
								//��ע
								SCORE iAddTimes = GetAddScoreTimes();
								lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

								//��ע�ӱ�
								if (m_bMingZhu == true) lCurrentScore *= 2;

								//������Ϣ
								CMD_C_AddScore AddScore;
								AddScore.wState = FALSE;
								AddScore.lScore = lCurrentScore;

								m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
								printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------hhhhhhhhhhhhhhh\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());

								return true;
							}
						}
					}
					else //����
					{
						//�������
						srand((unsigned int)GetTickCount());
						cbProbability = rand()%(10);

						//ɢ�Ʒ���
						if (m_cbCardType == 0 && wMeChairID != EstimateWinner())
						{
							m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

							return true;
						}
						else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner())
						{
							//��������
							if (cbProbability < 8)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
								printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------wwwwwwww\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								return true;
							}
						}
						else if ( m_cbCardType == CT_DOUBLE && wMeChairID != EstimateWinner())
						{
							//��������
							if (cbProbability < 5)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

								return true;
							}
						}


						//��ע����
						if ( ((m_cbCardType < CT_SHUN_ZI) && wMeChairID == EstimateWinner() && cbProbability<8)   ||
							((m_cbCardType < CT_SHUN_ZI) && wMeChairID != EstimateWinner() && cbProbability<2)   ||
							(m_lTableScore[wMeChairID]+ 4 * m_lCurrentTimes*m_lCellScore > m_lUserMaxScore && (m_cbCardType < CT_SHUN_ZI)))

						{
							//��ע
							SCORE iAddTimes = GetAddScoreTimes();
							lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

							//��ע�ӱ�
							if (m_bMingZhu == true) lCurrentScore *= 2;

							//������Ϣ
							CMD_C_AddScore AddScore;
							AddScore.wState = FALSE;
							AddScore.lScore = lCurrentScore;
							m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

							return true;
						}
						//���Ƹ���
						else if(m_cbCardType <= CT_DOUBLE)
						{
							//��ע����
							lCurrentScore = m_lCurrentTimes*m_lCellScore * 2;

							//������Ϣ
							CMD_C_AddScore AddScore;
							AddScore.wState = TRUE;
							AddScore.lScore = lCurrentScore;
							m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
							printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------ddddddddd\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							//������Ϣ
							CMD_C_CompareCard CompareCard;
							ZeroMemory(&CompareCard, sizeof(CompareCard));

							//�ж�����
							if (cbPlayerCount > 2)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
								//���ö�ʱ��
								UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
								m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
								return true;
								printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------eeeeeeee\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							}
							else
							{
								//�������
								for (WORD i = 0; i < GAME_PLAYER; i++)
								{
									if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
									{
										CompareCard.wCompareUser = i;
										break;
									}
								}
								m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
								return true;
								printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------fffffffff\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							}
						}
						else if (m_cbCardType > CT_DOUBLE)
						{
							BYTE cbbrand = IsOpenBrand();
							if (cbbrand == 0 || cbbrand == 2)
							{
								//��ע����
								if (m_bMingZhu == false)
								{
									lCurrentScore = 4 * m_lCurrentTimes*m_lCellScore;
								}
								else
								{
									lCurrentScore = 2 * m_lCurrentTimes*m_lCellScore;
								}


								//������Ϣ
								CMD_C_AddScore AddScore;
								AddScore.wState = TRUE;
								AddScore.lScore = lCurrentScore;
								//AddScore.cbState = COMPARE_STATUS;
								m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

								//������Ϣ
								CMD_C_CompareCard CompareCard;
								ZeroMemory(&CompareCard, sizeof(CompareCard));

								//�ж�����
								if (cbPlayerCount > 2)
								{
									m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
									//���ö�ʱ��
									UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
									m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
									return true;
									printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------qqqqqqqqqqq\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								}
								else
								{
									//�������
									for (WORD i = 0; i < GAME_PLAYER; i++)
									{ 
										if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
										{
											CompareCard.wCompareUser = i;
											break;
										}
									}

									m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
									return true;
									printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------ttttttttttt\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								}
							}
							else
							{
								//��ע
								SCORE iAddTimes = GetAddScoreTimes();
								lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

								//��ע�ӱ�
								if (m_bMingZhu == true) lCurrentScore *= 2;

								//������Ϣ
								CMD_C_AddScore AddScore;
								AddScore.wState = FALSE;
								AddScore.lScore = lCurrentScore;

								m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
								printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------hhhhhhhhhhhhhhh\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());

								return true;
							}
						}
					}
				}
			}
			m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
			//�����Ϣ
			TCHAR szBuffer[128]=TEXT("");
			_sntprintf(szBuffer,CountArray(szBuffer),TEXT("����������"));
			//�����Ϣ
			CTraceService::TraceString(szBuffer,TraceLevel_Normal);
			return true;
		}
	case IDI_USER_COMPARE_CARD:		//���ƶ�ʱ��
		{
			//�������
			WORD wCompareUser = INVALID_CHAIR;
			WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

			//������Ϣ
			CMD_C_CompareCard CompareCard;

			//�������
			while (true)
			{
				wCompareUser = rand()%GAME_PLAYER;
				if (wCompareUser != wMeChairID && m_cbPlayStatus[wCompareUser] == TRUE)
				{
					CompareCard.wCompareUser = wCompareUser;
					break;
				}
			}
			printf("�����˼�עIDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------;;;;;;;;;;;;\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
			//������Ϣ
			m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));

			return true;	
		}
	case IDI_DELAY_TIME:		//��ʱ��ʱ��
		{
			//ɾ����ʱ��
			m_pIAndroidUserItem->KillGameTimer(IDI_DELAY_TIME);

			//���ö�ʱ��
			UINT nElapse=TIME_LESS + (rand() % TIME_START_GAME);
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);

			return true;
		}
	case IDI_USER_FINISH_FLASH:	
		{
			m_pIAndroidUserItem->SendSocketData(SUB_C_FINISH_FLASH, NULL, 0);
			return true;
		}
	}

	return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//��Ϸ��ʼ
		{
			//��Ϣ����
			return OnSubGameStart(pData, wDataSize);
		}
	case SUB_S_ADD_SCORE:	//�û���ע
		{
			//��Ϣ����
			return OnSubAddScore(pData, wDataSize);
		}
	case SUB_S_GIVE_UP:		//�û�����
		{
			//��Ϣ����
			return OnSubGiveUp(pData, wDataSize);
		}
	case SUB_S_SEND_CARD:	//������Ϣ
		{
			//��Ϣ����
			return true;
		}
	case SUB_S_GAME_END:	//��Ϸ����
		{
			//��Ϣ����
			return OnSubGameEnd(pData, wDataSize);
		}
	case SUB_S_COMPARE_CARD://�û�����
		{
			//��Ϣ����
			return OnSubCompareCard(pData, wDataSize);
		}
	case SUB_S_LOOK_CARD:	//�û�����
		{
			//��Ϣ����
			return OnSubLookCard(pData, wDataSize);
		}
	case SUB_S_PLAYER_EXIT:	//�û�ǿ��
		{
			//��Ϣ����
			return OnSubPlayerExit(pData, wDataSize);
		}
	case SUB_S_OPEN_CARD:	//�û�̯��
		{
			//��Ϣ����
			return OnSubOpenCard(pData, wDataSize);
		}
	case SUB_S_WAIT_COMPARE://�ȴ�����
		{
			//��Ϣ����
			if (wDataSize != sizeof(CMD_S_WaitCompare)) return false;
			CMD_S_WaitCompare * pWaitCompare = (CMD_S_WaitCompare *)pData;

			//�û�Ч��
			ASSERT(pWaitCompare->wCompareUser == m_wCurrentUser);

			////���¶�ʱ
			//if (m_pIAndroidUserItem->GetChairID() == m_wCurrentUser)
			//{
			//	//���ö�ʱ��
			//	UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
			//	m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
			//}
			return true;
		}
	case SUB_S_ANDROID_CARD:
		{
			return OnSubAndroidCard(pData, wDataSize);
		}
	case SUB_S_ANDROID_BANKOPERATOR:
		{
			//BankOperate(2);
			return true;
		}
	case SUB_S_UPDATEAESKEY:
		{
			return OnSubUpdateAESKey(pData,wDataSize);
		}
	case SUB_S_RC_TREASEURE_DEFICIENCY:
		{
			return true;
		}
	case SUB_S_ADMIN_COLTERCARD:
		{
			//Ч�����
			ASSERT(wDataSize == sizeof(CMD_S_Admin_ChangeCard));
			if (wDataSize != sizeof(CMD_S_Admin_ChangeCard)) return false;

			//��������
			CMD_S_Admin_ChangeCard * pChangeCard = (CMD_S_Admin_ChangeCard *)pData;

			//���ñ���
			CopyMemory(m_cbAllHandCardData[pChangeCard->dwCharID], pChangeCard->cbCardData, sizeof(pChangeCard->cbCardData));
			if (pChangeCard->dwCharID == m_pIAndroidUserItem->GetMeUserItem()->GetChairID())
			{
				//���ñ���
				CopyMemory(m_cbHandCardData, pChangeCard->cbCardData, sizeof(pChangeCard->cbCardData));
			}

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
	case GAME_STATUS_FREE:		//����״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��Ϣ����
			CMD_S_StatusFree * pStatusFree = (CMD_S_StatusFree *)pData;



			if (!bLookonOther)
			{
				//������Կ
				CopyMemory(m_chUserAESKey, pStatusFree->chUserAESKey, sizeof(m_chUserAESKey));
			}

			CopyMemory(m_szServerName, pStatusFree->szServerName, sizeof(m_szServerName));
			//��ȡ����
			ReadConfigInformation(&(pStatusFree->CustomAndroid));
			//���в���
			BankOperate(2);

			//���ö�ʱ��
			UINT nElapse=TIME_LESS + (rand() % TIME_START_GAME);
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);

			return true;
		}
	case GAME_STATUS_PLAY:		//��Ϸ״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay = (CMD_S_StatusPlay *)pData;



			if (!bLookonOther)
			{
				//������Կ
				CopyMemory(m_chUserAESKey, pStatusPlay->chUserAESKey, sizeof(m_chUserAESKey));
			}
			
			CopyMemory(m_szServerName, pStatusPlay->szServerName, sizeof(m_szServerName));
			//��ȡ����
			ReadConfigInformation(&(pStatusPlay->CustomAndroid));
			//���в���
			BankOperate(2);

			//�û���Ϣ
			m_wBankerUser = pStatusPlay->wBankerUser;
			m_wCurrentUser = pStatusPlay->wCurrentUser;
			printf("��ǰ���GAME_STATUS_PLAY_%d====%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());
			//��ע��Ϣ
			m_lCellScore = pStatusPlay->lCellScore;
			m_lMaxCellScore = pStatusPlay->lMaxCellScore;
			m_lCurrentTimes = pStatusPlay->lCurrentTimes;
			m_lUserMaxScore = pStatusPlay->lUserMaxScore;
			CopyMemory(m_lTableScore, pStatusPlay->lTableScore, sizeof(m_lTableScore));
			m_bMingZhu = pStatusPlay->bMingZhu[m_pIAndroidUserItem->GetChairID()];

			//��Ϸ״̬
			CopyMemory(m_cbPlayStatus, pStatusPlay->cbPlayStatus, sizeof(m_cbPlayStatus));

			//�û��˿�
			CopyMemory(m_cbHandCardData, pStatusPlay->cbHandCardData, sizeof(m_cbHandCardData));

			//�ж��û�
			if (m_pIAndroidUserItem->GetChairID() == m_wCurrentUser)
			{
				//�ж�״̬
				if (pStatusPlay->bCompareState == true)
				{
					//�ȴ�����
					m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, NULL, 0);

					//���ö�ʱ��
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
				}
				else
				{
					//���ö�ʱ��
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
					printf("GAME_STATUS_PLAY%d===,%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());

					CString strtip;
					CTime time = CTime::GetCurrentTime();
					CString strFileName;
					strFileName.Format(TEXT("թ�𻨻�����IDI_USER_ADD_SCORE.log"));
					strtip.Format(TEXT("OnEventSceneMessage:GAME_STATUS_PLAY   m_pIAndroidUserItem->GetChairID() = %d  m_wCurrentUser = %d[%d-%d-%d %d:%d:%d]\n"), m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
					//WriteInfo(strFileName, strtip);
				}
			}

			return true;
		}
	}

	ASSERT(FALSE);

	return false;
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

//�û���λ
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//��������Ϣ
bool CAndroidUserItemSink::OnSubAndroidCard(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_S_AndroidCard));
	if (wDataSize != sizeof(CMD_S_AndroidCard)) return false;

	//��������
	CMD_S_AndroidCard * pAndroidCard = (CMD_S_AndroidCard *)pBuffer;

	//���ñ���
	m_lStockScore = pAndroidCard->lStockScore;
	m_bAndroidControl = pAndroidCard->bAndroidControl;
	CopyMemory(m_cbRealPlayer, pAndroidCard->cbRealPlayer, sizeof(m_cbRealPlayer));
	CopyMemory(m_cbAllHandCardData, pAndroidCard->cbAllHandCardData, sizeof(m_cbAllHandCardData));
	CopyMemory(m_cbAndroidStatus, pAndroidCard->cbAndroidStatus, sizeof(m_cbAndroidStatus));
	//if(m_bAndroidControl)
	{
		BYTE cbHandCardData[3] = {0};
		//�û��˿�
		CopyMemory(cbHandCardData, m_cbAllHandCardData[m_pIAndroidUserItem->GetChairID()], sizeof(cbHandCardData));

		//��ȡ����
		m_GameLogic.SortCardList(cbHandCardData, MAX_COUNT);
		m_cbCardType = m_GameLogic.GetCardType(cbHandCardData, MAX_COUNT);

		//��Ϊɢ��
		//if (m_cbCardType < CT_DOUBLE)
		//{
		//	for(BYTE i = 0; i < MAX_COUNT; i++)
		//	{
		//		m_cbCardType = 0;
		//		if (m_GameLogic.GetCardLogicValue(cbHandCardData[i]) > 11)
		//		{
		//			m_cbCardType = 1;
		//			break;
		//		}
		//	}
		//}
		if(m_cbCardType < CT_DOUBLE)
			m_wAndroidPourNumber = rand()%2 + 1;
		else if(m_cbCardType == CT_DOUBLE)
			m_wAndroidPourNumber = rand()%3 + 2;
		else
			m_wAndroidPourNumber = 100;    //��������˵�������������ơ�
	}
	return true;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	ReadConfigInformation2();
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//��������
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//�û���Ϣ
	m_wBankerUser = pGameStart->wBankerUser;
	m_wCurrentUser = pGameStart->wCurrentUser;
	printf("��ǰ���OnSubGameStart%d===%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());
	m_nCurrentRounds = pGameStart->nCurrentRounds;
	m_nTotalRounds = pGameStart->nTotalRounds;
	m_bGiveUp = false;
	//��ע��Ϣ
	m_bMingZhu = false;
	m_lCellScore = pGameStart->lCellScore;
	m_lMaxCellScore = pGameStart->lMaxScore;
	m_lUserMaxScore = pGameStart->lUserMaxScore;
	m_lCurrentTimes = pGameStart->lCurrentTimes;
	CopyMemory(m_cbPlayStatus, pGameStart->cbPlayStatus, sizeof(BYTE)*GAME_PLAYER);
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
	//�û�״̬
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//��ȡ�û�
		if (m_cbPlayStatus[i] != FALSE)
		{
			m_lTableScore[i] = m_lCellScore;
		}
	}

	//�������
	BYTE cbPlayerCount = 0;
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i] == TRUE) cbPlayerCount++;
	}
	//��Ҵ���
	ASSERT(m_pIAndroidUserItem != NULL);
	if (m_pIAndroidUserItem->GetChairID()==pGameStart->wCurrentUser)
	{
		//���ö�ʱ��
		UINT nElapse=TIME_LESS*cbPlayerCount + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
		printf("OnSubGameStart%d===,%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());

		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("թ�𻨻�����IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnSubGameStart   m_pIAndroidUserItem->GetChairID() = %d  m_wCurrentUser = %d[%d-%d-%d %d:%d:%d]\n"), m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}
	else
	{
		//���ö�ʱ��
		UINT nElapse=5+cbPlayerCount + (rand() % 12);
		m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDLOOKCARD, nElapse);
	}
	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_GiveUp));
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;

	//������Ϣ
	CMD_S_GiveUp * pGiveUp = (CMD_S_GiveUp *)pBuffer;

	//���ñ���
	m_cbRealPlayer[pGiveUp->wGiveUpUser] = FALSE;
	m_cbPlayStatus[pGiveUp->wGiveUpUser] = FALSE;
	m_cbAndroidStatus[pGiveUp->wGiveUpUser] = FALSE;
	if(pGiveUp->wGiveUpUser == m_pIAndroidUserItem->GetChairID())
	{
		//���ö�ʱ��
		UINT nElapse=3+(rand() % 17);
		m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_UP, nElapse);
		m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDLOOKCARD);
		m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDUPCARD);
		m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	}
	return true;
}

//�û���ע
bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_AddScore));
	if (wDataSize != sizeof(CMD_S_AddScore)) return false;

	//������Ϣ
	CMD_S_AddScore * pAddScore = (CMD_S_AddScore *)pBuffer;

	//���ñ���
	m_wCurrentUser = pAddScore->wCurrentUser;
	printf("��ǰ���OnSubAddScore%d===%d,��ע���_%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID(),pAddScore->wAddScoreUser);
	m_lCurrentTimes = pAddScore->lCurrentTimes;
	m_lTableScore[pAddScore->wAddScoreUser] += pAddScore->lAddScoreCount;
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	m_nCurrentRounds = pAddScore->nCurrentRounds;
	m_dwCompareState = pAddScore->wCompareState;
	if (pAddScore->nNetwaititem == 1 ||  pAddScore->nNetwaititem == 2)
	{
		m_pIAndroidUserItem->KillGameTimer(IDI_USER_COMPARE_CARD);
		return true;
	}
	//�û�����
	if (wMeChairID==m_wCurrentUser && pAddScore->wCompareState==FALSE)
	{
		//���ö�ʱ��
		UINT nElapse=TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
		printf("OnSubAddScore%d===,%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());

		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("թ�𻨻�����IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnSubAddScore   m_pIAndroidUserItem->GetChairID() = %d  m_wCurrentUser = %d[%d-%d-%d %d:%d:%d]\n"), m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubLookCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_LookCard));
	if (wDataSize != sizeof(CMD_S_LookCard)) return false;

	//������Ϣ
	CMD_S_LookCard * pLookCard = (CMD_S_LookCard *)pBuffer;

	//�����û�
	if (m_wCurrentUser == m_pIAndroidUserItem->GetChairID() && m_pIAndroidUserItem->GetChairID() == pLookCard->wLookCardUser)
	{

		//�û��˿�
		CopyMemory(m_cbHandCardData, pLookCard->cbCardData, sizeof(m_cbHandCardData));

		//��ȡ����
		m_GameLogic.SortCardList(m_cbHandCardData, MAX_COUNT);
		m_cbCardType = m_GameLogic.GetCardType(m_cbHandCardData, MAX_COUNT);

		//��Ϊɢ��
		if (m_cbCardType < CT_DOUBLE)
		{
			for(BYTE i = 0; i < MAX_COUNT; i++)
			{
				m_cbCardType = 0;
				if (m_GameLogic.GetCardLogicValue(m_cbHandCardData[i]) > 11)
				{
					m_cbCardType = 1;
					break;
				}
			}
		}


		//���ö�ʱ��
		UINT nElapse=TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
		printf("OnSubLookCard%d===,%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());
		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("թ�𻨻�����IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnSubLookCard   m_pIAndroidUserItem->GetChairID() = %d  m_wCurrentUser = %d[%d-%d-%d %d:%d:%d]\n"), m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}
	else if (pLookCard->wLookCardUser == m_pIAndroidUserItem->GetChairID())
	{
		//�û��˿�
		CopyMemory(m_cbHandCardData, pLookCard->cbCardData, sizeof(m_cbHandCardData));

		//��ȡ����
		m_GameLogic.SortCardList(m_cbHandCardData, MAX_COUNT);
		m_cbCardType = m_GameLogic.GetCardType(m_cbHandCardData, MAX_COUNT);

		//��Ϊɢ��
		if (m_cbCardType < CT_DOUBLE)
		{
			for(BYTE i = 0; i < MAX_COUNT; i++)
			{
				m_cbCardType = 0;
				if (m_GameLogic.GetCardLogicValue(m_cbHandCardData[i]) > 11)
				{
					m_cbCardType = 1;
					break;
				}
			}
		}
	}

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubCompareCard(const void * pBuffer, WORD wDataSize)
{
	//����Ч��
	ASSERT(wDataSize == sizeof(CMD_S_CompareCard));
	if (wDataSize != sizeof(CMD_S_CompareCard)) return false;

	//������Ϣ
	CMD_S_CompareCard * pCompareCard = (CMD_S_CompareCard *)pBuffer;

	//���ñ���
	m_wCurrentUser=pCompareCard->wCurrentUser;
	printf("��ǰ���OnSubCompareCard%d===%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());
	m_cbPlayStatus[pCompareCard->wLostUser] = FALSE;

	//�������
	BYTE cbUserCount = 0;
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i] == TRUE) cbUserCount++;
	}
	if(pCompareCard->wLostUser == m_pIAndroidUserItem->GetChairID())
	{
		m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDLOOKCARD);
		m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDUPCARD);
	}
	//�жϽ���
	if (cbUserCount > 1)
	{
		//��ǰ�û�
		if (m_pIAndroidUserItem->GetChairID() == m_wCurrentUser)
		{
			//���ö�ʱ��
			UINT nElapse=TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse+TIME_DELAY_TIME);
			printf("OnSubCompareCard%d===,%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());

			CString strtip;
			CTime time = CTime::GetCurrentTime();
			CString strFileName;
			strFileName.Format(TEXT("թ�𻨻�����IDI_USER_ADD_SCORE.log"));
			strtip.Format(TEXT("OnSubCompareCard   m_pIAndroidUserItem->GetChairID() = %d  m_wCurrentUser = %d[%d-%d-%d %d:%d:%d]\n"), m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
			//WriteInfo(strFileName, strtip);
		}
	}
	else if(m_pIAndroidUserItem->GetChairID() == pCompareCard->wCompareUser[0]
			|| m_pIAndroidUserItem->GetChairID() == pCompareCard->wCompareUser[1])
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_FINISH_FLASH,TIME_USER_FINISH_FLASH);
		printf("������IDI_USER_FINISH_FLASH\n");
	}

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//����Ч��
	ASSERT(wDataSize == sizeof(CMD_S_OpenCard));
	if (wDataSize != sizeof(CMD_S_OpenCard)) return false;

	return true;
}

//�û�ǿ��
bool CAndroidUserItemSink::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_PlayerExit));
	if (wDataSize != sizeof(CMD_S_PlayerExit)) return false;

	//������Ϣ
	CMD_S_PlayerExit * pPlayerExit=(CMD_S_PlayerExit *)pBuffer;

	//���ñ���
	m_cbPlayStatus[pPlayerExit->wPlayerID] = FALSE;

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd = (CMD_S_GameEnd *)pBuffer;

	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_COMPARE_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDLOOKCARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDUPCARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_UP);
	//�������
	m_lMaxCellScore = 0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	m_bGiveUp = false;
	//��ʼ����
	WORD wServerType=pGameEnd->wServerType;
	if (wServerType==GAME_GENRE_MATCH)
	{
		return true;
	}
	printf("CAndroidUserItemSink::OnSubGameEnd�����˽���\n");
	//��ʼʱ��
	if(pGameEnd->bDelayOverGame==true)
	{
		UINT nElapse=TIME_LESS + (rand() % TIME_DELAY_TIME);
		m_pIAndroidUserItem->SetGameTimer(IDI_DELAY_TIME, nElapse);
		printf("���������ÿ�ʼ��ʱ��\n");
	}
	else
	{
		UINT nElapse=TIME_LESS + (rand() % TIME_START_GAME);
		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);
		printf("�����˿�����ʼ��ʱ��\n");
	}

	//BankOperate(2);

	return true;
}

//������Կ
bool CAndroidUserItemSink::OnSubUpdateAESKey(const void * pBuffer, WORD wDataSize)
{
	ASSERT (wDataSize == sizeof(CMD_S_UpdateAESKey));
	if (wDataSize != sizeof(CMD_S_UpdateAESKey))
	{
		return false;
	}

	//IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
	//if(pUserItem->GetUserStatus() !=US_PLAYING)
	//{
 //       return true;
	//}

	CMD_S_UpdateAESKey *pUpdateAESKey = (CMD_S_UpdateAESKey*)pBuffer;

	CopyMemory(m_chUserAESKey, pUpdateAESKey->chUserUpdateAESKey, sizeof(m_chUserAESKey));

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
			//NcaTextOut(strInfo, m_szServerName);
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

			//NcaTextOut(strInfo, m_szServerName);
		}
		else if (lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)
		{
			CString strInfo1;
			strInfo1.Format(TEXT("����ȡ������(%.2lf)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore);
			strInfo+=strInfo1;

			//NcaTextOut(strInfo, m_szServerName);
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

			//NcaTextOut(strInfo, m_szServerName);
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

			//NcaTextOut(strInfo, m_szServerName);
		}
	}
}

//��ȡ����
void CAndroidUserItemSink::ReadConfigInformation(tagCustomAndroid *pCustomAndroid)
{
	m_lRobotScoreRange[0] = pCustomAndroid->lRobotScoreMin;
	m_lRobotScoreRange[1] = pCustomAndroid->lRobotScoreMax;

	if (m_lRobotScoreRange[1] < m_lRobotScoreRange[0])	
		m_lRobotScoreRange[1] = m_lRobotScoreRange[0];

	m_lRobotBankGetScore = pCustomAndroid->lRobotBankGet;
	m_lRobotBankGetScoreBanker = pCustomAndroid->lRobotBankGetBanker;
	m_nRobotBankStorageMul = pCustomAndroid->lRobotBankStoMul;

	if (m_nRobotBankStorageMul<0||m_nRobotBankStorageMul>100) 
		m_nRobotBankStorageMul =20;




}
//��ȡ����
void CAndroidUserItemSink::ReadConfigInformation2()
{
	m_nShunZiBetMin = GetPrivateProfileInt(m_szServerName, TEXT("ShunZiBetMin"), 0, m_szConfigFileName);
	if(m_nShunZiBetMin == 0)AfxMessageBox(_T("û���ҵ������ļ�"));
	m_nShunZiBetMax = GetPrivateProfileInt(m_szServerName, TEXT("ShunZiBetMax"), 50, m_szConfigFileName);
	m_nJInHuaBetMin = GetPrivateProfileInt(m_szServerName, TEXT("JInHuaBetMin"), 50, m_szConfigFileName);
	m_nJInHuaBetMax = GetPrivateProfileInt(m_szServerName, TEXT("JInHuaBetMax"), 200, m_szConfigFileName);
	m_nShunJinBetMin = GetPrivateProfileInt(m_szServerName, TEXT("ShunJinBetMin"), 200, m_szConfigFileName);
	m_nShunJinBetMax = GetPrivateProfileInt(m_szServerName, TEXT("ShunJinBetMax"), 600, m_szConfigFileName);
	m_nZhaDanBetMin = GetPrivateProfileInt(m_szServerName, TEXT("ZhaDanBetMin"), 600, m_szConfigFileName);
	m_nZhaDanBetMax = GetPrivateProfileInt(m_szServerName, TEXT("ZhaDanBetMax"), 1200, m_szConfigFileName);

	m_nSanPaiProbability = GetPrivateProfileInt(m_szServerName, TEXT("SanPaiProbability"), 30, m_szConfigFileName);
	m_nDuiziProbability = GetPrivateProfileInt(m_szServerName, TEXT("DuiZiProbability"), 30, m_szConfigFileName);
	m_nShunZiProbability = GetPrivateProfileInt(m_szServerName, TEXT("ShunZiProbability"), 30, m_szConfigFileName);
	m_nJInHuaProbability = GetPrivateProfileInt(m_szServerName, TEXT("JInHuaProbability"), 30, m_szConfigFileName);
	m_nShunJinProbability = GetPrivateProfileInt(m_szServerName, TEXT("ShunJinProbability"), 30, m_szConfigFileName);
	m_nZhaDanProbability = GetPrivateProfileInt(m_szServerName, TEXT("ZhaDanProbability"), 30, m_szConfigFileName);
}
//�ƶ�ʤ��
WORD CAndroidUserItemSink::EstimateWinner()
{
	//�����˿�
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData, m_cbAllHandCardData, sizeof(cbUserCardData));

	//�����˿�
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i], MAX_COUNT);
	}

	//��������
	WORD wWinUser = INVALID_CHAIR;

	//��������
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//�û�����
		if (m_cbPlayStatus[i] == FALSE) continue;

		//�����û�
		if (wWinUser == INVALID_CHAIR)
		{
			wWinUser=i;
			continue;
		}

		//�Ա��˿�
		if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinUser], MAX_COUNT) >= TRUE)
		{
			wWinUser = i;
		}
	}

	return wWinUser;
}

//��ע����
LONGLONG CAndroidUserItemSink::GetAddScoreTimes()
{
	LONGLONG lTimesSelect[4] = {1,2,5,10};
	LONGLONG lTimes = 50 - m_lCurrentTimes;
	if(lTimes >= lTimesSelect[3])
		lTimes = lTimesSelect[rand()%4];
	else if(lTimes >= lTimesSelect[2])
		lTimes = lTimesSelect[rand()%3];
	else if(lTimes >= lTimesSelect[1])
		lTimes = lTimesSelect[rand()%2];
	else if(lTimes == 1)
		lTimes = lTimesSelect[0];
	else
		lTimes = 0;
	//LONGLONG lTimesSelect[4] = {1,1,2,5};
	//LONGLONG lTimes = 10 - m_lCurrentTimes;
	//if(lTimes >= lTimesSelect[3])
	//	lTimes = lTimesSelect[rand()%4];
	//else if(lTimes >= lTimesSelect[2])
	//	lTimes = lTimesSelect[rand()%2];
	//else if(lTimes == 1)
	//	lTimes = lTimesSelect[1];
	//else
	//	lTimes = lTimesSelect[0];

	return lTimes;
}

//д��־�ļ�
void CAndroidUserItemSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
	//������������
	char* old_locale = _strdup(setlocale(LC_CTYPE,NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("%s"), pszFileName);
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
	if (bOpen)
	{	
		myFile.SeekToEnd();
		myFile.WriteString(pszString);
		myFile.Flush();
		myFile.Close();
	}

	//��ԭ�����趨
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}

//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
