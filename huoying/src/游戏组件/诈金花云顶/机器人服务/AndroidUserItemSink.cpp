#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "AESEncrypt.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_LESS						3									//最少时间
#define TIME_DELAY_TIME					3									//延时时间

//游戏时间
#define TIME_START_GAME					4									//开始时间
#define TIME_USER_ADD_SCORE				4									//下注时间
#define TIME_USER_COMPARE_CARD			4									//比牌时间
#define	TIME_USER_FINISH_FLASH			4									//比牌时间

//公共加密原文
unsigned char chCommonEncryption[AESENCRYPTION_LENGTH] = 
{
	0x32, 0x43, 0xf6, 0xa8, 
	0x88, 0x5a, 0x30, 0x8d, 
	0x31, 0x31, 0x98, 0xa2, 
	0xe0, 0x37, 0x07, 0x34
};

//游戏时间
#define IDI_START_GAME					(100)								//开始定时器
#define IDI_USER_ADD_SCORE				(101)								//下注定时器
#define IDI_USER_COMPARE_CARD			(102)								//比牌定时器
#define IDI_DELAY_TIME					(103)								//延时定时器
#define IDI_USER_FINISH_FLASH			(104)								//动画定时器
#define IDI_ANDROID_RANDLOOKCARD		(105)								//随机看牌
#define IDI_ANDROID_RANDUPCARD			(106)								//随机弃牌
#define IDI_ANDROID_UP					(107)								//随机起来
//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//游戏变量
	m_lStockScore = 0;
	m_wBankerUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	m_cbCardType = 0;

	//加注信息
	m_bMingZhu = false;
	m_lCellScore = 0;
	m_lMaxCellScore = 0;
	m_lUserMaxScore = 0;
	m_lCurrentTimes = 1;
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));

	//库存操作
	m_nRobotBankStorageMul=0;
	m_lRobotBankGetScore=0;
	m_lRobotBankGetScoreBanker=0;
	ZeroMemory(m_lRobotScoreRange, sizeof(m_lRobotScoreRange));

	//游戏状态
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));

	//用户扑克
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbAllHandCardData, sizeof(m_cbAllHandCardData));

	//接口变量
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

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	//游戏变量
	m_lStockScore = 0;
	m_wBankerUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	m_cbCardType = 0;

	//加注信息
	m_bMingZhu = false;
	m_lCellScore = 0;
	m_lMaxCellScore = 0;
	m_lUserMaxScore = 0;
	m_lCurrentTimes = 1;
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));

	//游戏状态
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));

	//用户扑克
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
	//定义变量
	bool bGiveUp = false;
	BYTE cbProbability = 0;
	SCORE lCurrentScore = 0;
	SCORE lAndroidMaxScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	//玩家人数
	BYTE cbPlayerCount = 0;
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i] == TRUE) cbPlayerCount++;
	}


	//首次操作
	if (m_lTableScore[wMeChairID] - m_lCellScore < DOUBLE_DOUBLE_X)
	{
		//随机概率
		cbProbability = rand()%(10);

		//看牌概率
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
			//散牌放弃
			if (m_cbCardType == 0 && wMeChairID != EstimateWinner())
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

				return true;
			}
			else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner())
			{
				//放弃概率
				if (cbProbability < GIVE_UP)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

					return true;
				}
			}
		}

		//首轮加注
		ASSERT(m_lCurrentTimes <= 3 && m_lCurrentTimes >= 0);
		SCORE iAddTimes = GetAddScoreTimes();
		lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;
		//明注加倍
		if (m_bMingZhu == true) lCurrentScore *= 2;

		//发送消息
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
		//是否看牌
		if (m_bMingZhu == false)
		{
			//随机概率
			cbProbability = rand()%(10);

			//看牌概率
			if (cbProbability < 2)
			{
				m_bMingZhu = true;
				m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);

				return true;
			}

			//用户加注
			if (m_lTableScore[wMeChairID] < 2 * m_lMaxCellScore && (4 * cbPlayerCount * m_lMaxCellScore + m_lCellScore + m_lTableScore[wMeChairID]) < lAndroidMaxScore)
			{
				//加注积分
				ASSERT(m_lCurrentTimes <= 3 && m_lCurrentTimes >= 0);
				SCORE iAddTimes = GetAddScoreTimes();
				lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

				//发送消息
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
			//比牌概率
			else
			{
				//加注积分
				lCurrentScore = m_lCurrentTimes*m_lCellScore * 2;

				//发送消息
				CMD_C_AddScore AddScore;
				AddScore.wState = TRUE;
				AddScore.lScore = lCurrentScore;
				//AddScore.cbState = COMPARE_STATUS;
				m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
				m_wAndroidPour++;
				//构造消息
				CMD_C_CompareCard CompareCard;
				ZeroMemory(&CompareCard, sizeof(CompareCard));

				//判断人数
				if (cbPlayerCount > 2)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
					//设置定时器
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
				}
				else
				{
					//查找玩家
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{
						if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
						{
							CompareCard.wCompareUser = i;
							break;
						}
					}
					printf("机器人比牌%d\n",__LINE__);
					m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
				}

				return true;
			}
		}
		else //看牌
		{
			//随机概率
			cbProbability = rand()%(10);

			//散牌放弃
			if (m_cbCardType == 0 && wMeChairID != EstimateWinner())
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

				return true;
			}
			else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner())
			{
				//放弃概率
				if (cbProbability < GIVE_UP)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

					return true;
				}
			}

			//加注概率
			if ((m_wAndroidPour < m_wAndroidPourNumber) && (4 * cbPlayerCount * m_lMaxCellScore + m_lCellScore + m_lTableScore[wMeChairID]) < lAndroidMaxScore)
			{
				//下注积分
				ASSERT(m_lCurrentTimes <= 3 && m_lCurrentTimes >= 0);

				//加注
				SCORE iAddTimes = GetAddScoreTimes();
				lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

				//明注加倍
				if (m_bMingZhu == true) lCurrentScore *= 2;

				//发送消息
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
			//用户比牌
			else
			{
				//加注积分
				lCurrentScore = 4 * m_lCurrentTimes*m_lCellScore;

				//发送消息
				CMD_C_AddScore AddScore;
				AddScore.wState = TRUE;
				AddScore.lScore = lCurrentScore;
				//AddScore.cbState = COMPARE_STATUS;
				m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
				m_wAndroidPour++;
				//构造消息
				CMD_C_CompareCard CompareCard;
				ZeroMemory(&CompareCard, sizeof(CompareCard));

				//判断人数
				if (cbPlayerCount > 2)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
					//设置定时器
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
				}
				else
				{
					//查找玩家
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{
						if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
						{
							CompareCard.wCompareUser = i;
							break;
						}
					}
					printf("机器人比牌%d\n",__LINE__);
					m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
				}

				return true;
			}
		}
	}
}
//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//开始定时器
		{
			IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			if (pUserItem)
			{
				//变量定义
				SCORE lRobotScore = pUserItem->GetUserScore();
				//判断存取
				if ((lRobotScore >m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)||(lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore))
				{		
					//发送机器人退出
					m_pIAndroidUserItem->SendAndroidQuit(NULL, 0);
					return true;
				}
			}
			//发送准备
			m_pIAndroidUserItem->SendUserReady(NULL, 0);

			return true;
		}
	case IDI_ANDROID_RANDUPCARD:
		{
			m_cbCardType =  m_GameLogic.GetCardType(m_cbAllHandCardData[m_pIAndroidUserItem->GetChairID()], 3);
			//删除定时器
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
					//随机概率
					cbProbability = rand()%(16);
					WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

					//散牌放弃
					if (m_cbCardType == 0 && wMeChairID != EstimateWinner() && m_bMingZhu == true &&  m_dwCompareState == 0)
					{
						m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
						return true;
					}
					else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner() && m_bMingZhu == true)
					{
						//放弃概率
						if (cbProbability < GIVE_UP&& m_dwCompareState == 0)
						{
							m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

							return true;
						}
						else
						{
							//设置定时器
							UINT nElapse= 5 + (rand() % 8);
							m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDUPCARD, nElapse);
						}
					}
					else
					{
						//设置定时器
						UINT nElapse= 5 + (rand() % 8);
						m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDUPCARD, nElapse);
					}

				}
			}
			else if(m_cbPlayStatus[m_pIAndroidUserItem->GetChairID()] == TRUE)
			{
				//设置定时器
				UINT nElapse= 5 + (rand() % 8);
				m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDUPCARD, nElapse);
			}

			return true;
		}
	case IDI_ANDROID_RANDLOOKCARD:
		{
			//删除定时器
			m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDLOOKCARD);

			if(m_pIAndroidUserItem->GetChairID() != m_wCurrentUser && m_cbPlayStatus[m_pIAndroidUserItem->GetChairID()] == TRUE && m_wCurrentUser != INVALID_CHAIR)
			{
				BYTE cbProbability = 0;
				//随机概率
				cbProbability = rand()%(16);
				WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
				//看牌概率
				if (m_bMingZhu == false)
				{
					if (cbProbability < LOOK_CARD&& m_dwCompareState == 0)
					{

						m_bMingZhu = true;
						m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);

						//设置定时器
						UINT nElapse=2 + (rand() % 5);
						m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDUPCARD, nElapse);

						return true;
					}
					else
					{
						//设置定时器
						UINT nElapse=5 + (rand() % 15);
						m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDLOOKCARD, nElapse);
					}
				}
			}
			else if(m_cbPlayStatus[m_pIAndroidUserItem->GetChairID()] == TRUE)
			{
				//设置定时器
				UINT nElapse=5 + (rand() % 15);
				m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDLOOKCARD, nElapse);
			}
			return true;
		}
	case IDI_USER_ADD_SCORE:	//加注定时器
		{
			//删除定时器
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);

			//数据效验
			ASSERT(m_pIAndroidUserItem->GetChairID() == m_wCurrentUser);
			if (m_pIAndroidUserItem->GetChairID() != m_wCurrentUser) 
			{
				CString strtip;
				CTime time = CTime::GetCurrentTime();
				CString strFileName;
				strFileName.Format(TEXT("诈金花OnUserAddScore.log"));
				strtip.Format(TEXT("m_pIAndroidUserItem->GetChairID() != m_wCurrentUser gameID= %d  m_pIAndroidUserItem->GetChairID() = %d  m_wCurrentUser = %d[%d-%d-%d %d:%d:%d]\n"),m_pIAndroidUserItem->GetMeUserItem()->GetGameID(), m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
				WriteInfo(strFileName, strtip);
				return false;
			}
			printf("机器人加注IDI_USER_ADD_SCORE_m_wCurrentUser_%d,GetChairID()_%d------1111111\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
			//定义变量
			bool bGiveUp = false;
			BYTE cbProbability = 0;
			SCORE lCurrentScore = 0;
			SCORE lAndroidMaxScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
			WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
			m_cbCardType =  m_GameLogic.GetCardType(m_cbAllHandCardData[m_pIAndroidUserItem->GetChairID()], 3);
			//玩家人数
			BYTE cbPlayerCount = 0;
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if (m_cbPlayStatus[i] == TRUE) cbPlayerCount++;
			}


			if (IsOpenBrand() == 2 && m_nCurrentRounds >= 2)
			{
				//加注积分
				if (m_bMingZhu == false)
				{
					lCurrentScore = 4 * m_lCurrentTimes*m_lCellScore;
				}
				else
				{
					lCurrentScore = 2 * m_lCurrentTimes*m_lCellScore;
				}


				//发送消息
				CMD_C_AddScore AddScore;
				AddScore.wState = TRUE;
				AddScore.lScore = lCurrentScore;
				//AddScore.cbState = COMPARE_STATUS;
				m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

				//构造消息
				CMD_C_CompareCard CompareCard;
				ZeroMemory(&CompareCard, sizeof(CompareCard));

				//判断人数
				if (cbPlayerCount > 2)
				{
					m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
					//设置定时器
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
					printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------222222\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
				}
				else
				{
					//查找玩家
					for (WORD i = 0; i < GAME_PLAYER; i++)
					{ 
						if (m_cbPlayStatus[i] == TRUE && i != wMeChairID)
						{
							CompareCard.wCompareUser = i;
							break;
						}
					}

					m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));
					printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------3333333\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
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
					printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------444444444\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
					m_bGiveUp = false;

					return true;
				}

				//首次操作
				if (m_lTableScore[wMeChairID] <= DIFEN_TIMES*m_lCellScore )
				{
					//随机概率
					cbProbability = rand()%(10);

					//看牌概率
					if (m_bMingZhu == false)
					{
						if (cbProbability < 7)
						{
							m_bMingZhu = true;      //弃牌前看牌
							m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);
							printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------4444444444\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							return true;
						}
					}
					else
					{

						if (m_cbCardType == 0 && wMeChairID != EstimateWinner())    //放弃
						{	
							m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
							return true;
						}
						else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner())    //散牌放弃
						{
							//大概率放弃
							if (cbProbability < 8)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
								printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------5555555555\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								return true;
							}
						}
						else if (m_cbCardType == CT_DOUBLE && wMeChairID != EstimateWinner())
						{
							//放弃
							if (cbProbability < 6)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

								return true;
							}
						}
					}

					//首轮加注
					SCORE iAddTimes = GetAddScoreTimes();
					lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;
					//明注加倍
					if (m_bMingZhu == true) lCurrentScore *= 2;

					//发送消息
					CMD_C_AddScore AddScore;
					AddScore.wState = FALSE;
					AddScore.lScore = lCurrentScore;
					m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
					printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------999999999\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
					return true;
				}
				else
				{
					//是否看牌
					if (m_bMingZhu == false)
					{
						//随机概率
						cbProbability = rand()%(10);

						//看牌概率
						if (cbProbability < 3)
						{
							m_bMingZhu = true;
							m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);
							printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------aaaaaaaaaa\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							return true;
						}
						else if (wMeChairID != EstimateWinner() )
						{
							if (cbProbability < 7)
							{
								if (m_cbCardType <= CT_DOUBLE)    //散牌放弃
								{
									m_bGiveUp = true;      //弃牌前看牌
								}
								m_bMingZhu = true;
								m_pIAndroidUserItem->SendSocketData(SUB_C_LOOK_CARD, NULL, 0);
								printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------bbbbbbbbbb\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								return true;
							}

						}

						//用户加注
						if (m_lTableScore[wMeChairID] + m_lCurrentTimes*m_lCellScore <= m_lUserMaxScore && m_cbCardType <= CT_DOUBLE)
						{
							//加注积分
							SCORE iAddTimes = GetAddScoreTimes();
							lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

							//发送消息
							CMD_C_AddScore AddScore;
							AddScore.wState = FALSE;
							AddScore.lScore = lCurrentScore;
							m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
							printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------ccccccccc\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							return true;
						}
						//比牌概率
						else if(m_cbCardType <= CT_DOUBLE)
						{
							//加注积分
							lCurrentScore = m_lCurrentTimes*m_lCellScore * 2;

							//发送消息
							CMD_C_AddScore AddScore;
							AddScore.wState = TRUE;
							AddScore.lScore = lCurrentScore;
							m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
							printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------ddddddddd\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							//构造消息
							CMD_C_CompareCard CompareCard;
							ZeroMemory(&CompareCard, sizeof(CompareCard));

							//判断人数
							if (cbPlayerCount > 2)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
								//设置定时器
								UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
								m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
								return true;
								printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------eeeeeeee\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							}
							else
							{
								//查找玩家
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
								printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------fffffffff\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							}
						}
						else if (m_cbCardType > CT_DOUBLE)
						{
							BYTE cbbrand = IsOpenBrand();
							if (cbbrand == 0 || cbbrand == 2)
							{
								//加注积分
								if (m_bMingZhu == false)
								{
									lCurrentScore = 4 * m_lCurrentTimes*m_lCellScore;
								}
								else
								{
									lCurrentScore = 2 * m_lCurrentTimes*m_lCellScore;
								}


								//发送消息
								CMD_C_AddScore AddScore;
								AddScore.wState = TRUE;
								AddScore.lScore = lCurrentScore;
								//AddScore.cbState = COMPARE_STATUS;
								m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

								//构造消息
								CMD_C_CompareCard CompareCard;
								ZeroMemory(&CompareCard, sizeof(CompareCard));

								//判断人数
								if (cbPlayerCount > 2)
								{
									m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
									//设置定时器
									UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
									m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
									return true;
									printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------qqqqqqqqqqq\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								}
								else
								{
									//查找玩家
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
									printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------ttttttttttt\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								}
								return true;
							}
							else
							{
								//加注
								SCORE iAddTimes = GetAddScoreTimes();
								lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

								//明注加倍
								if (m_bMingZhu == true) lCurrentScore *= 2;

								//发送消息
								CMD_C_AddScore AddScore;
								AddScore.wState = FALSE;
								AddScore.lScore = lCurrentScore;

								m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
								printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------hhhhhhhhhhhhhhh\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());

								return true;
							}
						}
					}
					else //看牌
					{
						//随机概率
						srand((unsigned int)GetTickCount());
						cbProbability = rand()%(10);

						//散牌放弃
						if (m_cbCardType == 0 && wMeChairID != EstimateWinner())
						{
							m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

							return true;
						}
						else if (m_cbCardType == CT_SINGLE && wMeChairID != EstimateWinner())
						{
							//放弃概率
							if (cbProbability < 8)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
								printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------wwwwwwww\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								return true;
							}
						}
						else if ( m_cbCardType == CT_DOUBLE && wMeChairID != EstimateWinner())
						{
							//放弃概率
							if (cbProbability < 5)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);

								return true;
							}
						}


						//加注概率
						if ( ((m_cbCardType < CT_SHUN_ZI) && wMeChairID == EstimateWinner() && cbProbability<8)   ||
							((m_cbCardType < CT_SHUN_ZI) && wMeChairID != EstimateWinner() && cbProbability<2)   ||
							(m_lTableScore[wMeChairID]+ 4 * m_lCurrentTimes*m_lCellScore > m_lUserMaxScore && (m_cbCardType < CT_SHUN_ZI)))

						{
							//加注
							SCORE iAddTimes = GetAddScoreTimes();
							lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

							//明注加倍
							if (m_bMingZhu == true) lCurrentScore *= 2;

							//发送消息
							CMD_C_AddScore AddScore;
							AddScore.wState = FALSE;
							AddScore.lScore = lCurrentScore;
							m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

							return true;
						}
						//比牌概率
						else if(m_cbCardType <= CT_DOUBLE)
						{
							//加注积分
							lCurrentScore = m_lCurrentTimes*m_lCellScore * 2;

							//发送消息
							CMD_C_AddScore AddScore;
							AddScore.wState = TRUE;
							AddScore.lScore = lCurrentScore;
							m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
							printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------ddddddddd\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							//构造消息
							CMD_C_CompareCard CompareCard;
							ZeroMemory(&CompareCard, sizeof(CompareCard));

							//判断人数
							if (cbPlayerCount > 2)
							{
								m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
								//设置定时器
								UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
								m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
								return true;
								printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------eeeeeeee\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							}
							else
							{
								//查找玩家
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
								printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------fffffffff\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
							}
						}
						else if (m_cbCardType > CT_DOUBLE)
						{
							BYTE cbbrand = IsOpenBrand();
							if (cbbrand == 0 || cbbrand == 2)
							{
								//加注积分
								if (m_bMingZhu == false)
								{
									lCurrentScore = 4 * m_lCurrentTimes*m_lCellScore;
								}
								else
								{
									lCurrentScore = 2 * m_lCurrentTimes*m_lCellScore;
								}


								//发送消息
								CMD_C_AddScore AddScore;
								AddScore.wState = TRUE;
								AddScore.lScore = lCurrentScore;
								//AddScore.cbState = COMPARE_STATUS;
								m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

								//构造消息
								CMD_C_CompareCard CompareCard;
								ZeroMemory(&CompareCard, sizeof(CompareCard));

								//判断人数
								if (cbPlayerCount > 2)
								{
									m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, &CompareCard, sizeof(CompareCard));
									//设置定时器
									UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
									m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
									return true;
									printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------qqqqqqqqqqq\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								}
								else
								{
									//查找玩家
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
									printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------ttttttttttt\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
								}
							}
							else
							{
								//加注
								SCORE iAddTimes = GetAddScoreTimes();
								lCurrentScore = m_lCurrentTimes*m_lCellScore + m_lCellScore * iAddTimes;

								//明注加倍
								if (m_bMingZhu == true) lCurrentScore *= 2;

								//发送消息
								CMD_C_AddScore AddScore;
								AddScore.wState = FALSE;
								AddScore.lScore = lCurrentScore;

								m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
								printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------hhhhhhhhhhhhhhh\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());

								return true;
							}
						}
					}
				}
			}
			m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP, NULL, 0);
			//输出信息
			TCHAR szBuffer[128]=TEXT("");
			_sntprintf(szBuffer,CountArray(szBuffer),TEXT("机器人弃牌"));
			//输出信息
			CTraceService::TraceString(szBuffer,TraceLevel_Normal);
			return true;
		}
	case IDI_USER_COMPARE_CARD:		//比牌定时器
		{
			//定义变量
			WORD wCompareUser = INVALID_CHAIR;
			WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

			//构造消息
			CMD_C_CompareCard CompareCard;

			//查找玩家
			while (true)
			{
				wCompareUser = rand()%GAME_PLAYER;
				if (wCompareUser != wMeChairID && m_cbPlayStatus[wCompareUser] == TRUE)
				{
					CompareCard.wCompareUser = wCompareUser;
					break;
				}
			}
			printf("机器人加注IDI_USER_COMPARE_CARD_m_wCurrentUser_%d,GetChairID()_%d------;;;;;;;;;;;;\n",m_wCurrentUser,m_pIAndroidUserItem->GetMeUserItem()->GetGameID());
			//发送消息
			m_pIAndroidUserItem->SendSocketData(SUB_C_COMPARE_CARD, &CompareCard, sizeof(CompareCard));

			return true;	
		}
	case IDI_DELAY_TIME:		//延时定时器
		{
			//删除定时器
			m_pIAndroidUserItem->KillGameTimer(IDI_DELAY_TIME);

			//设置定时器
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

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//游戏开始
		{
			//消息处理
			return OnSubGameStart(pData, wDataSize);
		}
	case SUB_S_ADD_SCORE:	//用户下注
		{
			//消息处理
			return OnSubAddScore(pData, wDataSize);
		}
	case SUB_S_GIVE_UP:		//用户放弃
		{
			//消息处理
			return OnSubGiveUp(pData, wDataSize);
		}
	case SUB_S_SEND_CARD:	//发牌消息
		{
			//消息处理
			return true;
		}
	case SUB_S_GAME_END:	//游戏结束
		{
			//消息处理
			return OnSubGameEnd(pData, wDataSize);
		}
	case SUB_S_COMPARE_CARD://用户比牌
		{
			//消息处理
			return OnSubCompareCard(pData, wDataSize);
		}
	case SUB_S_LOOK_CARD:	//用户看牌
		{
			//消息处理
			return OnSubLookCard(pData, wDataSize);
		}
	case SUB_S_PLAYER_EXIT:	//用户强退
		{
			//消息处理
			return OnSubPlayerExit(pData, wDataSize);
		}
	case SUB_S_OPEN_CARD:	//用户摊牌
		{
			//消息处理
			return OnSubOpenCard(pData, wDataSize);
		}
	case SUB_S_WAIT_COMPARE://等待比牌
		{
			//消息处理
			if (wDataSize != sizeof(CMD_S_WaitCompare)) return false;
			CMD_S_WaitCompare * pWaitCompare = (CMD_S_WaitCompare *)pData;

			//用户效验
			ASSERT(pWaitCompare->wCompareUser == m_wCurrentUser);

			////重新定时
			//if (m_pIAndroidUserItem->GetChairID() == m_wCurrentUser)
			//{
			//	//设置定时器
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
			//效验参数
			ASSERT(wDataSize == sizeof(CMD_S_Admin_ChangeCard));
			if (wDataSize != sizeof(CMD_S_Admin_ChangeCard)) return false;

			//变量定义
			CMD_S_Admin_ChangeCard * pChangeCard = (CMD_S_Admin_ChangeCard *)pData;

			//设置变量
			CopyMemory(m_cbAllHandCardData[pChangeCard->dwCharID], pChangeCard->cbCardData, sizeof(pChangeCard->cbCardData));
			if (pChangeCard->dwCharID == m_pIAndroidUserItem->GetMeUserItem()->GetChairID())
			{
				//设置变量
				CopyMemory(m_cbHandCardData, pChangeCard->cbCardData, sizeof(pChangeCard->cbCardData));
			}

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//消息处理
			CMD_S_StatusFree * pStatusFree = (CMD_S_StatusFree *)pData;



			if (!bLookonOther)
			{
				//拷贝密钥
				CopyMemory(m_chUserAESKey, pStatusFree->chUserAESKey, sizeof(m_chUserAESKey));
			}

			CopyMemory(m_szServerName, pStatusFree->szServerName, sizeof(m_szServerName));
			//读取配置
			ReadConfigInformation(&(pStatusFree->CustomAndroid));
			//银行操作
			BankOperate(2);

			//设置定时器
			UINT nElapse=TIME_LESS + (rand() % TIME_START_GAME);
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);

			return true;
		}
	case GAME_STATUS_PLAY:		//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay = (CMD_S_StatusPlay *)pData;



			if (!bLookonOther)
			{
				//拷贝密钥
				CopyMemory(m_chUserAESKey, pStatusPlay->chUserAESKey, sizeof(m_chUserAESKey));
			}
			
			CopyMemory(m_szServerName, pStatusPlay->szServerName, sizeof(m_szServerName));
			//读取配置
			ReadConfigInformation(&(pStatusPlay->CustomAndroid));
			//银行操作
			BankOperate(2);

			//用户信息
			m_wBankerUser = pStatusPlay->wBankerUser;
			m_wCurrentUser = pStatusPlay->wCurrentUser;
			printf("当前玩家GAME_STATUS_PLAY_%d====%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());
			//加注信息
			m_lCellScore = pStatusPlay->lCellScore;
			m_lMaxCellScore = pStatusPlay->lMaxCellScore;
			m_lCurrentTimes = pStatusPlay->lCurrentTimes;
			m_lUserMaxScore = pStatusPlay->lUserMaxScore;
			CopyMemory(m_lTableScore, pStatusPlay->lTableScore, sizeof(m_lTableScore));
			m_bMingZhu = pStatusPlay->bMingZhu[m_pIAndroidUserItem->GetChairID()];

			//游戏状态
			CopyMemory(m_cbPlayStatus, pStatusPlay->cbPlayStatus, sizeof(m_cbPlayStatus));

			//用户扑克
			CopyMemory(m_cbHandCardData, pStatusPlay->cbHandCardData, sizeof(m_cbHandCardData));

			//判断用户
			if (m_pIAndroidUserItem->GetChairID() == m_wCurrentUser)
			{
				//判断状态
				if (pStatusPlay->bCompareState == true)
				{
					//等待比牌
					m_pIAndroidUserItem->SendSocketData(SUB_C_WAIT_COMPARE, NULL, 0);

					//设置定时器
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_COMPARE_CARD);
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_COMPARE_CARD, nElapse);
				}
				else
				{
					//设置定时器
					UINT nElapse = TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
					m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
					printf("GAME_STATUS_PLAY%d===,%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());

					CString strtip;
					CTime time = CTime::GetCurrentTime();
					CString strFileName;
					strFileName.Format(TEXT("诈金花机器人IDI_USER_ADD_SCORE.log"));
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

//用户进入
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户段位
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//机器人消息
bool CAndroidUserItemSink::OnSubAndroidCard(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_S_AndroidCard));
	if (wDataSize != sizeof(CMD_S_AndroidCard)) return false;

	//变量定义
	CMD_S_AndroidCard * pAndroidCard = (CMD_S_AndroidCard *)pBuffer;

	//设置变量
	m_lStockScore = pAndroidCard->lStockScore;
	m_bAndroidControl = pAndroidCard->bAndroidControl;
	CopyMemory(m_cbRealPlayer, pAndroidCard->cbRealPlayer, sizeof(m_cbRealPlayer));
	CopyMemory(m_cbAllHandCardData, pAndroidCard->cbAllHandCardData, sizeof(m_cbAllHandCardData));
	CopyMemory(m_cbAndroidStatus, pAndroidCard->cbAndroidStatus, sizeof(m_cbAndroidStatus));
	//if(m_bAndroidControl)
	{
		BYTE cbHandCardData[3] = {0};
		//用户扑克
		CopyMemory(cbHandCardData, m_cbAllHandCardData[m_pIAndroidUserItem->GetChairID()], sizeof(cbHandCardData));

		//获取类型
		m_GameLogic.SortCardList(cbHandCardData, MAX_COUNT);
		m_cbCardType = m_GameLogic.GetCardType(cbHandCardData, MAX_COUNT);

		//如为散牌
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
			m_wAndroidPourNumber = 100;    //这样设置说明不会主动开牌。
	}
	return true;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	ReadConfigInformation2();
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//用户信息
	m_wBankerUser = pGameStart->wBankerUser;
	m_wCurrentUser = pGameStart->wCurrentUser;
	printf("当前玩家OnSubGameStart%d===%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());
	m_nCurrentRounds = pGameStart->nCurrentRounds;
	m_nTotalRounds = pGameStart->nTotalRounds;
	m_bGiveUp = false;
	//加注信息
	m_bMingZhu = false;
	m_lCellScore = pGameStart->lCellScore;
	m_lMaxCellScore = pGameStart->lMaxScore;
	m_lUserMaxScore = pGameStart->lUserMaxScore;
	m_lCurrentTimes = pGameStart->lCurrentTimes;
	CopyMemory(m_cbPlayStatus, pGameStart->cbPlayStatus, sizeof(BYTE)*GAME_PLAYER);
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
	//用户状态
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//获取用户
		if (m_cbPlayStatus[i] != FALSE)
		{
			m_lTableScore[i] = m_lCellScore;
		}
	}

	//玩家人数
	BYTE cbPlayerCount = 0;
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		if (m_cbPlayStatus[i] == TRUE) cbPlayerCount++;
	}
	//玩家处理
	ASSERT(m_pIAndroidUserItem != NULL);
	if (m_pIAndroidUserItem->GetChairID()==pGameStart->wCurrentUser)
	{
		//设置定时器
		UINT nElapse=TIME_LESS*cbPlayerCount + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
		printf("OnSubGameStart%d===,%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());

		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("诈金花机器人IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnSubGameStart   m_pIAndroidUserItem->GetChairID() = %d  m_wCurrentUser = %d[%d-%d-%d %d:%d:%d]\n"), m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}
	else
	{
		//设置定时器
		UINT nElapse=5+cbPlayerCount + (rand() % 12);
		m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_RANDLOOKCARD, nElapse);
	}
	return true;
}

//用户放弃
bool CAndroidUserItemSink::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_GiveUp));
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;

	//处理消息
	CMD_S_GiveUp * pGiveUp = (CMD_S_GiveUp *)pBuffer;

	//设置变量
	m_cbRealPlayer[pGiveUp->wGiveUpUser] = FALSE;
	m_cbPlayStatus[pGiveUp->wGiveUpUser] = FALSE;
	m_cbAndroidStatus[pGiveUp->wGiveUpUser] = FALSE;
	if(pGiveUp->wGiveUpUser == m_pIAndroidUserItem->GetChairID())
	{
		//设置定时器
		UINT nElapse=3+(rand() % 17);
		m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_UP, nElapse);
		m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDLOOKCARD);
		m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDUPCARD);
		m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	}
	return true;
}

//用户下注
bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_AddScore));
	if (wDataSize != sizeof(CMD_S_AddScore)) return false;

	//处理消息
	CMD_S_AddScore * pAddScore = (CMD_S_AddScore *)pBuffer;

	//设置变量
	m_wCurrentUser = pAddScore->wCurrentUser;
	printf("当前玩家OnSubAddScore%d===%d,下注玩家_%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID(),pAddScore->wAddScoreUser);
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
	//用户动作
	if (wMeChairID==m_wCurrentUser && pAddScore->wCompareState==FALSE)
	{
		//设置定时器
		UINT nElapse=TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
		printf("OnSubAddScore%d===,%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());

		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("诈金花机器人IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnSubAddScore   m_pIAndroidUserItem->GetChairID() = %d  m_wCurrentUser = %d[%d-%d-%d %d:%d:%d]\n"), m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}

	return true;
}

//用户看牌
bool CAndroidUserItemSink::OnSubLookCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_LookCard));
	if (wDataSize != sizeof(CMD_S_LookCard)) return false;

	//处理消息
	CMD_S_LookCard * pLookCard = (CMD_S_LookCard *)pBuffer;

	//看牌用户
	if (m_wCurrentUser == m_pIAndroidUserItem->GetChairID() && m_pIAndroidUserItem->GetChairID() == pLookCard->wLookCardUser)
	{

		//用户扑克
		CopyMemory(m_cbHandCardData, pLookCard->cbCardData, sizeof(m_cbHandCardData));

		//获取类型
		m_GameLogic.SortCardList(m_cbHandCardData, MAX_COUNT);
		m_cbCardType = m_GameLogic.GetCardType(m_cbHandCardData, MAX_COUNT);

		//如为散牌
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


		//设置定时器
		UINT nElapse=TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse);
		printf("OnSubLookCard%d===,%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());
		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("诈金花机器人IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnSubLookCard   m_pIAndroidUserItem->GetChairID() = %d  m_wCurrentUser = %d[%d-%d-%d %d:%d:%d]\n"), m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}
	else if (pLookCard->wLookCardUser == m_pIAndroidUserItem->GetChairID())
	{
		//用户扑克
		CopyMemory(m_cbHandCardData, pLookCard->cbCardData, sizeof(m_cbHandCardData));

		//获取类型
		m_GameLogic.SortCardList(m_cbHandCardData, MAX_COUNT);
		m_cbCardType = m_GameLogic.GetCardType(m_cbHandCardData, MAX_COUNT);

		//如为散牌
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

//用户比牌
bool CAndroidUserItemSink::OnSubCompareCard(const void * pBuffer, WORD wDataSize)
{
	//数据效验
	ASSERT(wDataSize == sizeof(CMD_S_CompareCard));
	if (wDataSize != sizeof(CMD_S_CompareCard)) return false;

	//处理消息
	CMD_S_CompareCard * pCompareCard = (CMD_S_CompareCard *)pBuffer;

	//设置变量
	m_wCurrentUser=pCompareCard->wCurrentUser;
	printf("当前玩家OnSubCompareCard%d===%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());
	m_cbPlayStatus[pCompareCard->wLostUser] = FALSE;

	//玩家人数
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
	//判断结束
	if (cbUserCount > 1)
	{
		//当前用户
		if (m_pIAndroidUserItem->GetChairID() == m_wCurrentUser)
		{
			//设置定时器
			UINT nElapse=TIME_LESS + (rand() % TIME_USER_ADD_SCORE);
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE, nElapse+TIME_DELAY_TIME);
			printf("OnSubCompareCard%d===,%d\n",m_wCurrentUser,m_pIAndroidUserItem->GetChairID());

			CString strtip;
			CTime time = CTime::GetCurrentTime();
			CString strFileName;
			strFileName.Format(TEXT("诈金花机器人IDI_USER_ADD_SCORE.log"));
			strtip.Format(TEXT("OnSubCompareCard   m_pIAndroidUserItem->GetChairID() = %d  m_wCurrentUser = %d[%d-%d-%d %d:%d:%d]\n"), m_pIAndroidUserItem->GetChairID(),m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
			//WriteInfo(strFileName, strtip);
		}
	}
	else if(m_pIAndroidUserItem->GetChairID() == pCompareCard->wCompareUser[0]
			|| m_pIAndroidUserItem->GetChairID() == pCompareCard->wCompareUser[1])
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_FINISH_FLASH,TIME_USER_FINISH_FLASH);
		printf("机器人IDI_USER_FINISH_FLASH\n");
	}

	return true;
}

//用户开牌
bool CAndroidUserItemSink::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//数据效验
	ASSERT(wDataSize == sizeof(CMD_S_OpenCard));
	if (wDataSize != sizeof(CMD_S_OpenCard)) return false;

	return true;
}

//用户强退
bool CAndroidUserItemSink::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_PlayerExit));
	if (wDataSize != sizeof(CMD_S_PlayerExit)) return false;

	//处理消息
	CMD_S_PlayerExit * pPlayerExit=(CMD_S_PlayerExit *)pBuffer;

	//设置变量
	m_cbPlayStatus[pPlayerExit->wPlayerID] = FALSE;

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd = (CMD_S_GameEnd *)pBuffer;

	//删除定时器
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_COMPARE_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDLOOKCARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_RANDUPCARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_ANDROID_UP);
	//清理变量
	m_lMaxCellScore = 0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	m_bGiveUp = false;
	//开始设置
	WORD wServerType=pGameEnd->wServerType;
	if (wServerType==GAME_GENRE_MATCH)
	{
		return true;
	}
	printf("CAndroidUserItemSink::OnSubGameEnd机器人结束\n");
	//开始时间
	if(pGameEnd->bDelayOverGame==true)
	{
		UINT nElapse=TIME_LESS + (rand() % TIME_DELAY_TIME);
		m_pIAndroidUserItem->SetGameTimer(IDI_DELAY_TIME, nElapse);
		printf("机器人设置开始定时器\n");
	}
	else
	{
		UINT nElapse=TIME_LESS + (rand() % TIME_START_GAME);
		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);
		printf("机器人开启开始定时器\n");
	}

	//BankOperate(2);

	return true;
}

//更新密钥
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

//银行操作
void CAndroidUserItemSink::BankOperate(BYTE cbType)
{
	IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
	if(pUserItem->GetUserStatus()>=US_SIT)
	{
		if(cbType==1)
		{
			CString strInfo;
			strInfo.Format(TEXT("大厅：状态不对，不执行存取款"));
			//NcaTextOut(strInfo, m_szServerName);
			return;
		}
	}

	//变量定义
	SCORE lRobotScore = pUserItem->GetUserScore();

	{
		CString strInfo;
		strInfo.Format(TEXT("[%s] 分数(%.2lf)"),pUserItem->GetNickName(),lRobotScore);

		if (lRobotScore > m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore)
		{
			CString strInfo1;
			strInfo1.Format(TEXT("满足存款条件(%.2lf)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMaxScore);
			strInfo+=strInfo1;

			//NcaTextOut(strInfo, m_szServerName);
		}
		else if (lRobotScore < m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore)
		{
			CString strInfo1;
			strInfo1.Format(TEXT("满足取款条件(%.2lf)"),m_pIAndroidUserItem->GetAndroidParameter()->lTakeMinScore);
			strInfo+=strInfo1;

			//NcaTextOut(strInfo, m_szServerName);
		}

		//判断存取
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
			strInfo.Format(TEXT("[%s] 执行存款：存款前金币(%.2lf)，存款后金币(%.2lf)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

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
			strInfo.Format(TEXT("[%s] 执行取款：取款前金币(%.2lf)，取款后金币(%.2lf)"),pUserItem->GetNickName(),lRobotScore,lRobotNewScore);

			//NcaTextOut(strInfo, m_szServerName);
		}
	}
}

//读取配置
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
//读取配置
void CAndroidUserItemSink::ReadConfigInformation2()
{
	m_nShunZiBetMin = GetPrivateProfileInt(m_szServerName, TEXT("ShunZiBetMin"), 0, m_szConfigFileName);
	if(m_nShunZiBetMin == 0)AfxMessageBox(_T("没有找到配置文件"));
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
//推断胜者
WORD CAndroidUserItemSink::EstimateWinner()
{
	//保存扑克
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData, m_cbAllHandCardData, sizeof(cbUserCardData));

	//排列扑克
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i], MAX_COUNT);
	}

	//变量定义
	WORD wWinUser = INVALID_CHAIR;

	//查找数据
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//用户过滤
		if (m_cbPlayStatus[i] == FALSE) continue;

		//设置用户
		if (wWinUser == INVALID_CHAIR)
		{
			wWinUser=i;
			continue;
		}

		//对比扑克
		if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinUser], MAX_COUNT) >= TRUE)
		{
			wWinUser = i;
		}
	}

	return wWinUser;
}

//加注倍数
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

//写日志文件
void CAndroidUserItemSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
	//设置语言区域
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

	//还原区域设定
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}

//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
