#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////////////

#define IDI_CHECK_TABLE				1					//�������
#define IDI_START_GAME				2					//��ʼ��ʱ��
#define IDI_OUT_CARD				3					//���ƶ�ʱ��
#define IDI_OPERATE_CARD			4					//������ʱ��
#define IDI_OVER_TIME				5					//��ʱ����

//���캯��
CTableFrameSink::CTableFrameSink()
{
	m_wPlayerCount = GAME_PLAYER;

	//�������
	m_pITableFrame = NULL;
	m_pGameCustomRule = NULL;
	m_pGameServiceOption = NULL;
	m_pGameServiceAttrib = NULL;	

	diFenUnit = 1;

	//��Ϸ����
	m_wSiceCount = 0;
	m_wBankerUser = INVALID_CHAIR;
	m_wLianZhuang = 1;
	m_wCurrentUser = INVALID_CHAIR;
	m_cbMagicIndex = MAX_INDEX;
	ZeroMemory(m_bTing, sizeof(m_bTing));
	ZeroMemory(m_bTrustee, sizeof(m_bTrustee));

	//������Ϣ
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	ZeroMemory(m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));

	//���б���
	m_cbProvideCard = 0;
	m_wResumeUser = INVALID_CHAIR;
	m_wProvideUser = INVALID_CHAIR;

	//�û�״̬
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

	//����˿�
	ZeroMemory(m_WeaveItemArray, sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));

	//������Ϣ
	m_cbChiHuCard = 0;
	ZeroMemory(m_dwChiHuKind, sizeof(m_dwChiHuKind));

	//״̬����
	m_bGangOutCard = false;
	m_enSendStatus = Not_Send;
	m_cbGangStatus = WIK_GANERAL;
	m_wProvideGangUser = INVALID_CHAIR;
	ZeroMemory(m_bEnjoinChiHu, sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng, sizeof(m_bEnjoinChiPeng));
	ZeroMemory(m_bEnjoinGang, sizeof(m_bEnjoinGang));
	ZeroMemory(m_bGangCard, sizeof(m_bGangCard));
	ZeroMemory(m_cbChiPengCount, sizeof(m_cbChiPengCount));	
	ZeroMemory(m_cbGangCount, sizeof(m_cbGangCount));

	//������Ϣ
	m_cbOutCardData = 0;
	m_cbOutCardCount = 0;
	m_wOutCardUser = INVALID_CHAIR;

	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));

	//������Ϣ
	m_cbMinusHeadCount = 0;
	m_cbMinusLastCount = 0;
	m_cbLeftCardCount = MAX_REPERTORY;
	m_cbEndLeftCount = 0;
	m_cbSendCardCount = 0;

	//�˿���Ϣ
	ZeroMemory(m_cbCardIndex, sizeof(m_cbCardIndex));
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));

	ZeroMemory(m_lUserGangScore,sizeof(m_lUserGangScore));
	ZeroMemory(m_cbUserCatchCardCount,sizeof(m_cbUserCatchCardCount));
	ZeroMemory(m_bPlayStatus,sizeof(m_bPlayStatus));

	ZeroMemory(m_cbHuCardCount,sizeof(m_cbHuCardCount));
	ZeroMemory(m_cbHuCardData,sizeof(m_cbHuCardData));
	ZeroMemory(m_cbUserMaCount,sizeof(m_cbUserMaCount));
	ZeroMemory(&m_stRecord,sizeof(m_stRecord));
	m_wLastCatchCardUser=INVALID_CHAIR;

	//�������
	m_cbMaCount=0;
	m_cbPlayerCount=0;

	m_cbJiePao = FALSE;

#ifdef  CARD_DISPATCHER_CONTROL
	m_cbControlGameCount = 0;
#endif

	m_pC = NULL;
	m_hC = LoadLibrary(TEXT("CMj.dll"));
	if (m_hC)
	{
		typedef void * (*CREATE)();
		CREATE pFunc = (CREATE)GetProcAddress(m_hC, "CreateCGame");
		if (pFunc)
		{
			m_pC = static_cast<ICGame*>(pFunc());
		}
		else
		{
			//CTraceService::TraceString(_TEXT("cerr"));
		}
	}


	return;
}

//��������
CTableFrameSink::~CTableFrameSink()
{
	if (m_pC)
	{
		delete m_pC;
		m_pC = NULL;
	}

	if (m_hC)
	{
		FreeLibrary(m_hC);
		m_hC = NULL;
	}
}

//�ӿڲ�ѯ
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	//��Ϸ����
	m_wSiceCount = 0;
	m_wCurrentUser = INVALID_CHAIR;
	m_cbMagicIndex = MAX_INDEX;
	m_cbMinusHeadCount = 0;
	m_cbMinusLastCount = 0;
	m_cbLeftCardCount = MAX_REPERTORY;
	m_cbEndLeftCount = 0;
	m_cbSendCardCount = 0;

	ZeroMemory(m_bTing, sizeof(m_bTing));
	ZeroMemory(m_bTrustee, sizeof(m_bTrustee));

	//������Ϣ
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	ZeroMemory(m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));

	//���б���
	m_cbProvideCard = 0;
	m_wResumeUser = INVALID_CHAIR;
	m_wProvideUser = INVALID_CHAIR;

	//״̬����
	m_bGangOutCard = false;
	m_enSendStatus = Not_Send;
	m_cbGangStatus = WIK_GANERAL;
	m_wProvideGangUser = INVALID_CHAIR;
	ZeroMemory(m_bEnjoinChiHu, sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng, sizeof(m_bEnjoinChiPeng));	
	ZeroMemory(m_bEnjoinGang, sizeof(m_bEnjoinGang));
	ZeroMemory(m_cbChiPengCount, sizeof(m_cbChiPengCount));
	ZeroMemory(m_bGangCard, sizeof(m_bGangCard));	
	ZeroMemory(m_cbGangCount, sizeof(m_cbGangCount));

	for(int i = 0; i < GAME_PLAYER; i++)
	{
		m_vecEnjoinChiHu[i].clear();
		m_vecEnjoinChiPeng[i].clear();	
	}

	//�û�״̬
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

	//����˿�
	ZeroMemory(m_WeaveItemArray, sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));

	//������Ϣ
	m_cbChiHuCard=0;
	ZeroMemory(m_dwChiHuKind, sizeof(m_dwChiHuKind));
	
	//������Ϣ
	m_cbOutCardData = 0;
	m_cbOutCardCount = 0;
	m_wOutCardUser = INVALID_CHAIR;

	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));

	//�˿���Ϣ
	ZeroMemory(m_cbCardIndex, sizeof(m_cbCardIndex));
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));
	ZeroMemory(m_cbUserCatchCardCount,sizeof(m_cbUserCatchCardCount));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory(m_lUserGangScore,sizeof(m_lUserGangScore));
	ZeroMemory(m_bPlayStatus,sizeof(m_bPlayStatus));


	m_wLastCatchCardUser=INVALID_CHAIR;

	ZeroMemory(m_cbHuCardCount,sizeof(m_cbHuCardCount));
	ZeroMemory(m_cbHuCardData,sizeof(m_cbHuCardData));
	ZeroMemory(m_cbUserMaCount,sizeof(m_cbUserMaCount));
	return;
}

//��������
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	//�����ж�
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"),TraceLevel_Exception);
		return false;
	}

#ifdef CARD_DISPATCHER_CONTROL
	if(!m_pITableFrame->GetTableID())
		CTraceService::TraceString(TraceLevel_Exception,L"�����齫���������ܿ���,�����ļ�ΪSparrowData.dat");
#endif

	//��Ϸ����
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//�Զ�����
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	m_cbMaCount = m_pGameCustomRule->cbMaCount;
	m_cbPlayerCount = m_pGameCustomRule->cbPlayerCount;

	ZeroMemory(&m_stRecord,sizeof(m_stRecord));
	//��ʼģʽ
	if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//����ģʽ
		m_pITableFrame->SetStartMode(START_MODE_FULL_READY);
	else
		m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	return true;
}

//��������
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	return 0L;
}

//���ٻ���
SCORE CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//�����Ϸ
	if (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{		
		return 64 * m_pITableFrame->GetCellScore();
	}
	return 0;
}

void CTableFrameSink::OnPrivateTableEnded(int flag)
{
	m_TableEnded = true;
}
void CTableFrameSink::OnPrivateTableCreated()
{
	m_PlayCount = 0;
	m_TableEnded = false;

	m_HistoryScore.ResetData();
	m_wBankerUser = INVALID_CHAIR;
	m_wLianZhuang = 1;

	BYTE* rule = m_pITableFrame->GetGameRule();
	ASSERT(rule[0] == 1);
	m_cbPlayerCount = rule[1];
	m_cbMaCount = rule[2];
	m_cbJiePao = rule[3];
	m_cbQiangGang = rule[4];
	m_cbQiXiaoDui = rule[5];

	diFenUnit = rule[98]*10;

	m_GameLogic.m_cbQiXiaoDui = m_cbQiXiaoDui;
}

void CTableFrameSink::PrintCards()
{
	return;
	strInfo.Format(TEXT("���ƿ��:"));
	//GameLog(strInfo);

	for (int n = 0; n < MAX_REPERTORY; n++)
	{
		strInfo.Format(TEXT("card[%d]:0x%02x"),n, m_cbRepertoryCard[n]);
		//GameLog(strInfo);
	}



	strInfo.Format(TEXT("���ƿ��ͳ��:"));
	//GameLog(strInfo);

	BYTE cards[3 * 9 + 1];

	int pos = 0;
	for (int i = 0; i <= 2; i++)
	{
		for (int j = 1; j <= 9; j++)
		{
			cards[pos++] = (i << 4) + j;
		}
	}
	cards[27] = 0x35;

	for (int i = 0; i < CountArray(cards); i++)
	{
		BYTE value = cards[i];
		int count = 0;
		for (int n = 0; n < MAX_REPERTORY; n++)
		{
			if (m_cbRepertoryCard[n] == value)
			{
				count++;
			}
		}

		strInfo.Format(TEXT("card:0x%02x, %d��"), value, count);
		//GameLog(strInfo);
	}


}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	m_PlayCount++;

	m_TableEnded = false;

	//mGameRecordStream.resetValues();
	//GetLocalTime(&mGameRecordStream.timeStart); 
	//����״̬
	m_pITableFrame->SetGameStatus(GAME_SCENE_PLAY);

	//����˿�
	BYTE Sice1 = rand()%6 + 1;
	BYTE Sice2 = rand()%6 + 1;
	BYTE minSice = min(Sice1,Sice2);
	m_wSiceCount = MAKEWORD(Sice1,Sice2);
	m_cbLeftCardCount=MAX_REPERTORY;
	m_cbSendCardCount = 0;
	m_bUserActionDone=false;
	m_enSendStatus = Not_Send;
	m_cbGangStatus = WIK_GANERAL;
	m_wProvideGangUser = INVALID_CHAIR;


	bool ok = false;
	/*if (m_pC)
	{
		ok = m_pC->CCards(m_pITableFrame, this);
	}*/

	if (!ok)
	{
		//���ݷ��ƿ��
		BYTE repoLen = 0;
		BYTE repoCards[CountArray(m_GameLogic.m_cbCardDataArray)];
		int MAX_REPO = repoLen = CountArray(repoCards);
		CopyMemory(repoCards, m_GameLogic.m_cbCardDataArray, repoLen);
		m_GameLogic.GetCards(&m_cbRepertoryCard[0], repoLen, repoCards, repoLen);
	}

	//m_GameLogic.RandCardList(m_cbRepertoryCard,CountArray(m_cbRepertoryCard));

#ifdef  CARD_DISPATCHER_CONTROL

	TCHAR szPath[MAX_PATH] = TEXT("");
	GetCurrentDirectory(sizeof(szPath), szPath);
	CString strSaveFileName;
	strSaveFileName.Format(TEXT("%s\\SparrowData.dat"), szPath);
	//�ǿ���״̬
	if(m_cbControlGameCount==0)
	{
		CFile file;
		if (file.Open(strSaveFileName, CFile::modeRead))
		{
			file.Read(&m_wBankerUser,sizeof(m_wBankerUser));
			file.Read(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));		
			file.Close();
		}		
	}
	else
	{
		m_wBankerUser = m_wControBankerUser;
		CopyMemory(m_cbRepertoryCard, m_cbControlRepertoryCard, sizeof(m_cbRepertoryCard));
		m_cbControlGameCount--;
	}

#endif

	//const BYTE cbCardDataArray[MAX_REPERTORY]=
	//{
	//	0x35, 0x15, 0x35, 0x01, 0x24, 0x02, 0x04, 0x08, 0x09,					
	//	0x01, 0x14, 0x03, 0x04, 0x06, 0x07, 0x08, 0x09, 0x28,					
	//	0x01, 0x02, 0x03, 0x04, 0x05, 0x09, 0x26, 0x27, 0x29,					
	//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,					
	//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,					
	//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,					
	//	0x11, 0x12, 0x13, 0x17, 0x18, 0x19, 0x16, 0x06, 0x07,					
	//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,					
	//	0x21, 0x22, 0x23, 0x02, 0x25, 0x06, 0x07, 0x08, 0x05,					
	//	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,					
	//	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,					
	//	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,					
	//	0x03, 0x35, 0x05, 0x35													
	//};

	//for (BYTE i = 0; i < MAX_REPERTORY; i++)
	//{
	//	m_cbRepertoryCard[i] = cbCardDataArray[i];
	//}

	//���п��Ե�����
 	m_cbMagicIndex = m_GameLogic.SwitchToCardIndex(0x35);
 	m_GameLogic.SetMagicIndex(m_cbMagicIndex);

	//�ַ��˿�
	for (WORD i = 0; i < m_cbPlayerCount; i++)
	{
		if(m_pITableFrame->GetTableUserItem(i) == NULL)
			continue;

		m_bPlayStatus[i]=true;
		m_cbLeftCardCount -= (MAX_COUNT - 1);
		m_cbMinusHeadCount += (MAX_COUNT - 1);
		m_cbHandCardCount[i] = (MAX_COUNT - 1);
		m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount], MAX_COUNT - 1, m_cbCardIndex[i]);
	}

	if(m_wBankerUser == INVALID_CHAIR && (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)!=0 )//����ģʽ���Ȱ�ׯ�Ҹ�����
	{
		DWORD OwnerId =  m_pITableFrame->GetTableOwner();
		IServerUserItem *pOwnerItem = m_pITableFrame->SearchUserItem(OwnerId);
		if (pOwnerItem != NULL && pOwnerItem->GetChairID() != INVALID_CHAIR)
			m_wBankerUser = pOwnerItem->GetChairID();
	}
	//ȷ��ׯ��
	if(m_wBankerUser == INVALID_CHAIR || m_bPlayStatus[m_wBankerUser]==false)
	{
		m_wBankerUser = rand() % m_cbPlayerCount;
		while(m_pITableFrame->GetTableUserItem(m_wBankerUser)==NULL)
		{
			m_wBankerUser = (m_wBankerUser+1)%m_cbPlayerCount;
		}
	}

	//�����˿�
	m_cbMinusHeadCount++;
	m_cbSendCardData = m_cbRepertoryCard[--m_cbLeftCardCount];
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
	m_cbUserCatchCardCount[m_wBankerUser]++;
	//���ñ���
	m_cbProvideCard = m_cbSendCardData;
	m_wProvideUser = m_wBankerUser;
	m_wCurrentUser = m_wBankerUser;

	//������Ϣ
	WORD wSiceCount = LOBYTE(m_wSiceCount) + HIBYTE(m_wSiceCount);
	WORD wTakeChairID = (m_wBankerUser + wSiceCount - 1) % GAME_PLAYER;
	BYTE cbTakeCount = MAX_REPERTORY - m_cbLeftCardCount;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//������Ŀ
		BYTE cbValidCount = HEAP_FULL_COUNT - m_cbHeapCardInfo[wTakeChairID][1] - ((i == 0) ? (minSice) * 2 : 0);
		BYTE cbRemoveCount = __min(cbValidCount, cbTakeCount);

		//��ȡ�˿�
		cbTakeCount -= cbRemoveCount;
		m_cbHeapCardInfo[wTakeChairID][(i == 0) ? 1 : 0] += cbRemoveCount;

		//����ж�
		if (cbTakeCount == 0)
		{
			m_wHeapHead = wTakeChairID;
			m_wHeapTail = (m_wBankerUser + wSiceCount - 1) % GAME_PLAYER;
			break;
		}
		//�л�����
		wTakeChairID = (wTakeChairID + GAME_PLAYER - 1) % GAME_PLAYER;
	}

	//��������
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));

	//�����ж�
	tagGangCardResult GangCardResult;
	m_cbUserAction[m_wBankerUser]|=m_GameLogic.AnalyseGangCardEx(m_cbCardIndex[m_wBankerUser],NULL,0,0,GangCardResult);

	//�����ж�
	CChiHuRight chr;
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]--;
	m_cbUserAction[m_wBankerUser]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[m_wBankerUser],NULL,0,m_cbSendCardData,chr,true);
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
	m_cbHandCardCount[m_wBankerUser]++;


	//�û�������λ��Ϣ
	CMD_S_GameUserInfo initUserInfo;
	ZeroMemory(&initUserInfo, sizeof(initUserInfo));
	for (int i = 0; i < m_cbPlayerCount; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem == NULL)
		{
			continue;
		}

		initUserInfo.userId[i] = pServerUserItem->GetUserID();
	}
	addGameRecord(INVALID_CHAIR, SUB_S_INIT_USERINFO, &initUserInfo, sizeof(initUserInfo));


	//�����ж�
	BYTE cbCount = 0;
	BYTE cbOutCard[MAX_COUNT][28] = {0};
	CMD_S_Hu_Data HuData;
	ZeroMemory(&HuData,sizeof(HuData));
	if(m_bTing[m_wBankerUser] == false)
	{
		cbCount =m_GameLogic.AnalyseTingCard(m_cbCardIndex[m_wBankerUser],0,0,HuData.cbOutCardCount,HuData.cbOutCardData,HuData.cbHuCardCount,HuData.cbHuCardData);
		if(cbCount >0)
		{
			m_cbUserAction[m_wBankerUser] |= WIK_LISTEN;
			for(int i=0;i<MAX_COUNT;i++)
			{
				if(HuData.cbHuCardCount[i]>0)
				{
					for(int j=0;j<HuData.cbHuCardCount[i];j++)
					{
						HuData.cbHuCardRemainingCount[i][j] = GetRemainingCount(m_wBankerUser,HuData.cbHuCardData[i][j]);
					}
				}
				else
					break;
			}
			//addGameRecord(m_wBankerUser, SUB_S_HU_CARD, &HuData, sizeof(HuData));
			m_pITableFrame->SendTableData(m_wBankerUser,SUB_S_HU_CARD,&HuData,sizeof(HuData));
		}
	}
	
	//�������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	GameStart.wBankerUser = m_wBankerUser;
	GameStart.wSiceCount = m_wSiceCount;
	GameStart.wHeapHead = m_wHeapHead;
	GameStart.wHeapTail = m_wHeapTail;
	GameStart.cbMagicIndex = m_cbMagicIndex;
	CopyMemory(GameStart.cbHeapCardInfo, m_cbHeapCardInfo, sizeof(GameStart.cbHeapCardInfo));

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		GameStart.lCollectScore[i] = m_HistoryScore.GetHistoryScore(i)->lCollectScore;
		GameStart.lTurnScore[i] = m_HistoryScore.GetHistoryScore(i)->lTurnScore;
	}

	//��������
	for (WORD i=0;i<m_cbPlayerCount;i++)
	{
		if(m_pITableFrame->GetTableUserItem(i) == NULL)
			continue;
		GameStart.cbUserAction = m_cbUserAction[i];
		ZeroMemory(GameStart.cbCardData, sizeof(GameStart.cbCardData));
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameStart.cbCardData);

		GameStart.cbOutCardCount=0;
		ZeroMemory(GameStart.cbOutCardData, sizeof(GameStart.cbOutCardData));
		if(i == m_wBankerUser && cbCount>0)
		{
			GameStart.cbOutCardCount=cbCount;
			CopyMemory(GameStart.cbOutCardData,cbOutCard,sizeof(cbOutCard));
		}

		addGameRecord(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
		m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
		m_pITableFrame->SendLookonData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));		
	}

	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) != 0)
	{
		m_pITableFrame->SetGameTimer(IDI_CHECK_TABLE, 30000, -1, NULL);
		m_cbWaitTime = 0;
	}

	m_pITableFrame->KillGameTimer(IDI_OVER_TIME);
	m_pITableFrame->SetGameTimer(IDI_OVER_TIME, 30000, 1, 0);

	//û�ж�̬���룬���Ǵ�������ָ��������
	//mGameRecordStream.userCount = m_cbPlayerCount;

	return true;
}

void CTableFrameSink::SetLeftCards(CMD_S_GameConclude &conclude)
{

	strInfo.Format(TEXT("��ַ��͵���:%d��"), m_cbLeftCardCount);
	//GameLog(strInfo);


	for (int i = 0; i<m_cbLeftCardCount; i++)
	{
		conclude.cbLeftCardData[i] = m_cbRepertoryCard[m_cbMinusLastCount + i];

		//strInfo.Format(TEXT("card[%d]:0x%02x"), (m_cbMinusLastCount + i), m_cbRepertoryCard[m_cbMinusLastCount + i]);
		////GameLog(strInfo);
	}
	conclude.cbLeftCardCount = m_cbLeftCardCount;

}
//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	if((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0)
		m_pITableFrame->KillGameTimer(IDI_CHECK_TABLE);
	m_pITableFrame->KillGameTimer(IDI_OVER_TIME);

	switch (cbReason)
	{
	case GER_NORMAL:		//�������
	{

			//��������
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude, sizeof(GameConclude));
			GameConclude.cbSendCardData = m_cbSendCardData;
			GameConclude.wLeftUser = INVALID_CHAIR;			

			//������Ϣ
			for (WORD i = 0; i < m_cbPlayerCount; i++)
			{
				GameConclude.dwChiHuKind[i] = m_dwChiHuKind[i];
				//Ȩλ����
				if(m_dwChiHuKind[i] == WIK_CHI_HU)
				{
					FiltrateRight(i, m_ChiHuRight[i]);
					m_ChiHuRight[i].GetRightData(GameConclude.dwChiHuRight[i], MAX_RIGHT_COUNT);
				}
				GameConclude.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameConclude.cbHandCardData[i]);
			}

			int nCount = m_cbMaCount;
			//if(nCount>1)
			//	nCount++;


			//������ڿ�棬���Ҳ���õ���
			bool reSetMa = false;
			WORD cChair = INVALID_CHAIR;
			for (int i = 0; i < GAME_PLAYER; i++)
			{
				if (WIK_CHI_HU == m_dwChiHuKind[(m_wBankerUser + i) % GAME_PLAYER])
				{
					IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem((m_wBankerUser + i) % GAME_PLAYER);
					if (pUserItem != NULL)
					{
						cChair = (m_wBankerUser + i) % GAME_PLAYER;
						reSetMa = true;
					}
				}
			}
			
			//���������Ӯ��
			SCORE lUserGameScore[GAME_PLAYER] = { 0 };
			CalHuPaiScore(lUserGameScore);

			//����������
			CopyMemory(GameConclude.cbMaCount, m_cbUserMaCount, sizeof(m_cbUserMaCount));

			for(int i=0;i<nCount;i++)
			{
				GameConclude.cbMaData[i] = m_cbRepertoryCard[m_cbMinusLastCount+i];
			}

			//���ֱ���
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

			GameConclude.wProvideUser = m_wProvideUser;
			GameConclude.cbProvideCard = m_cbProvideCard;

			//ͳ�ƻ���
			for (WORD i = 0; i < m_cbPlayerCount; i++)
			{
				if(!m_bPlayStatus[i])
					continue;

				GameConclude.lGameScore[i] = lUserGameScore[i];
				//���Ʒ�������ټ��ϸܵ���Ӯ�־�����ұ���������Ӯ��
				GameConclude.lGameScore[i] += m_lUserGangScore[i];
				GameConclude.lGangScore[i] = m_lUserGangScore[i];

				//��˰
				if (GameConclude.lGameScore[i]>0 && (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0)
				{
					GameConclude.lRevenue[i] = m_pITableFrame->CalculateRevenue(i,GameConclude.lGameScore[i]);
					GameConclude.lGameScore[i] -= GameConclude.lRevenue[i];
				}

				ScoreInfoArray[i].lRevenue = GameConclude.lRevenue[i];
				ScoreInfoArray[i].lScore = GameConclude.lGameScore[i];
				ScoreInfoArray[i].cbType = ScoreInfoArray[i].lScore > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

				//��ʷ����
				m_HistoryScore.OnEventUserScore(i, GameConclude.lGameScore[i]);
				if(m_stRecord.nCount<32)
				{
					m_stRecord.lDetailScore[i][m_stRecord.nCount]=GameConclude.lGameScore[i];
					m_stRecord.lAllScore[i] += GameConclude.lGameScore[i];
				}


				IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);

			}
			m_stRecord.nCount++;

			if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//����ģʽ
			{
				addGameRecord(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_RECORD,&m_stRecord,sizeof(m_stRecord));
			}


			for (int i = 0; i < GAME_PLAYER; i++)
			{
				GameConclude.lCollectScore[i] = m_HistoryScore.GetHistoryScore(i)->lCollectScore;
				GameConclude.lTurnScore[i] = m_HistoryScore.GetHistoryScore(i)->lTurnScore;
			}

			SetLeftCards(GameConclude);

			addGameRecord(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			//��������
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));

			//д�����
			m_pITableFrame->WriteTableScore(ScoreInfoArray, m_cbPlayerCount);

			//������Ϸ
			//TODO:д�뱾�ּ�¼
			//GetLocalTime(&mGameRecordStream.timeEnd);
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);
			//mGameRecordStream.resetValues();

			if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//����ģʽ
			{
				//��ɢ���䡢����Լ�ֽ�������Ҫ������ƴ���
				if (m_pITableFrame->IsPersonalRoomDisumme() || m_TableEnded )
				{
					ZeroMemory(&m_stRecord,sizeof(m_stRecord));
				}
			}
			
			return true;
		}
	case GER_NETWORK_ERROR:	//�����ж�
		{
			return true;
		}
	case GER_USER_LEAVE:	//�û�ǿ��
		{
			
			//�Զ��й�
			OnUserTrustee(wChairID,true);

			return true;
		}
	case GER_DISMISS:		//��Ϸ��ɢ
	{

			//��������
			CMD_S_GameConclude GameConclude;
			ZeroMemory(&GameConclude, sizeof(GameConclude));

			m_wBankerUser = INVALID_CHAIR;

			GameConclude.cbSendCardData = m_cbSendCardData;

			//�û��˿�
			BYTE cbCardIndex=0;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				GameConclude.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameConclude.cbHandCardData[i]);
			}

			for (int i = 0; i < GAME_PLAYER; i++)
			{
				GameConclude.lCollectScore[i] = m_HistoryScore.GetHistoryScore(i)->lCollectScore;
				GameConclude.lTurnScore[i] = m_HistoryScore.GetHistoryScore(i)->lTurnScore;
			}

			SetLeftCards(GameConclude);
			//������Ϣ
			addGameRecord(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_CONCLUDE, &GameConclude, sizeof(GameConclude));


			//������Ϸ
			//TODO:д�뱾�ּ�¼??
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//����ģʽ
			{
				if(m_pITableFrame->IsPersonalRoomDisumme() )//��ǰ���Ѿֽ�ɢ�����¼
				{
					ZeroMemory(&m_stRecord,sizeof(m_stRecord));
				}
			}
			
			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));

			//���ñ���
			StatusFree.lCellScore = m_pITableFrame->GetCellScore();
			StatusFree.cbMaCount = m_cbMaCount;
			StatusFree.cbPlayerCount = m_cbPlayerCount;
			//�Զ�����
			StatusFree.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusFree.cbTimeOperateCard = m_pGameCustomRule->cbTimeOperateCard;
			StatusFree.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;

			//��ʷ����
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//��������
				tagHistoryScore * pHistoryScore = m_HistoryScore.GetHistoryScore(i);

				//���ñ���
				StatusFree.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//����ģʽ
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_RECORD,&m_stRecord,sizeof(m_stRecord));

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
		}
	case GAME_SCENE_PLAY:	//��Ϸ״̬
		{
			//��������
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));

			//�Զ�����
			StatusPlay.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusPlay.cbTimeOperateCard = m_pGameCustomRule->cbTimeOperateCard;
			StatusPlay.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
			
			OnUserTrustee(wChairID,false);//����ȡ���й�

			//����
			StatusPlay.cbMaCount = m_cbMaCount;
			StatusPlay.cbPlayerCount = m_cbPlayerCount;
			//��Ϸ����
			StatusPlay.wBankerUser = m_wBankerUser;
			StatusPlay.wCurrentUser = m_wCurrentUser;
			StatusPlay.lCellScore = m_pGameServiceOption->lCellScore;
			StatusPlay.cbMagicIndex = m_cbMagicIndex;
			CopyMemory(StatusPlay.bTrustee, m_bTrustee, sizeof(m_bTrustee));

			//״̬����
			StatusPlay.cbActionCard = m_cbProvideCard;
			StatusPlay.cbLeftCardCount = m_cbLeftCardCount;
			StatusPlay.cbActionMask = !m_bResponse[wChairID] ? m_cbUserAction[wChairID] : WIK_NULL;
			
			CopyMemory(StatusPlay.bTrustee, m_bTrustee, sizeof(StatusPlay.bTrustee));
			CopyMemory(StatusPlay.bTing, m_bTing, sizeof(StatusPlay.bTing));
			//��ǰ�ܺ�����
			StatusPlay.cbOutCardCount = m_GameLogic.AnalyseTingCard(m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],m_cbWeaveItemCount[wChairID],StatusPlay.cbOutCardCount,StatusPlay.cbOutCardDataEx,StatusPlay.cbHuCardCount,StatusPlay.cbHuCardData); 

			//��ʷ��¼
			StatusPlay.wOutCardUser = m_wOutCardUser;
			StatusPlay.cbOutCardData = m_cbOutCardData;
			CopyMemory(StatusPlay.cbDiscardCard, m_cbDiscardCard, sizeof(StatusPlay.cbDiscardCard));
			CopyMemory(StatusPlay.cbDiscardCount, m_cbDiscardCount, sizeof(StatusPlay.cbDiscardCount));

			//����˿�
			CopyMemory(StatusPlay.WeaveItemArray, m_WeaveItemArray, sizeof(StatusPlay.WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveItemCount, m_cbWeaveItemCount, sizeof(StatusPlay.cbWeaveItemCount));

			//������Ϣ
			StatusPlay.wHeapHead = m_wHeapHead;
			StatusPlay.wHeapTail = m_wHeapTail;
			CopyMemory(StatusPlay.cbHeapCardInfo, m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));

			//�˿�����
			for(int i = 0; i < GAME_PLAYER; i++)
			{
				StatusPlay.cbCardCount[i] = m_GameLogic.GetCardCount(m_cbCardIndex[i]);
			}
			m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID], StatusPlay.cbCardData);
			StatusPlay.cbSendCardData = (m_wCurrentUser == wChairID) ? m_cbSendCardData : 0x00;

			//��ʷ����
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				//��������
				tagHistoryScore * pHistoryScore = m_HistoryScore.GetHistoryScore(i);

				//���ñ���
				StatusPlay.lTurnScore[i] = pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i] = pHistoryScore->lCollectScore;
			}

			if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//����ģʽ
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_RECORD,&m_stRecord,sizeof(m_stRecord));
			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		if(wTimerID==IDI_CHECK_TABLE)
		{
			m_cbWaitTime++;
			if(m_cbWaitTime>=3)
			{
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS);
			}
			else if(m_cbWaitTime>=2)
			{
				for(int i=0;i<GAME_PLAYER;i++)
				{
					IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(i);
					if(pServerUserItem!=NULL && pServerUserItem->GetUserStatus()==US_OFFLINE)return true;				
				}
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS);
			}
			return true;
		}
	}
	//�йܳ��ƴ���
	if(wTimerID == IDI_OUT_CARD)
	{
		m_pITableFrame->KillGameTimer(wTimerID);

		BYTE card = m_cbSendCardData;
		if (!m_GameLogic.IsValidCard(card) || m_GameLogic.IsMagicCard(card))
		{
			for(int i=0;i<MAX_INDEX;i++)
			{
				if(m_cbCardIndex[m_wCurrentUser][i]>0)
				{
					card = m_GameLogic.SwitchToCardData(i);

					if (!m_GameLogic.IsMagicCard(card))
						break;
				}
			}
		}

		OnUserOutCard(m_wCurrentUser,card,true);
		return true;
	}
	//��ʱ����
	else if (wTimerID == IDI_OVER_TIME)
	{
		m_pITableFrame->KillGameTimer(wTimerID);

		if (m_wCurrentUser == INVALID_CHAIR)
		{
			//������ʾ
			for (WORD i = 0; i<m_cbPlayerCount; i++)
			{
				if (m_cbUserAction[i] != WIK_NULL)
				{
					BYTE cbOperateCard[3] = { 0 };
					if (m_cbUserAction[i] != WIK_LISTEN)
						OnUserOperateCard(i, WIK_NULL, cbOperateCard);
					else
						OnUserListenCard(i, false);
				}
			}
		}
		else
		{
			BYTE card = m_cbSendCardData;
			if (!m_GameLogic.IsValidCard(card) || m_GameLogic.IsMagicCard(card))
			{
				for (int i = 0; i<MAX_INDEX; i++)
				{
					if (m_cbCardIndex[m_wCurrentUser][i]>0)
					{
						card = m_GameLogic.SwitchToCardData(i);

						if (!m_GameLogic.IsMagicCard(card))
							break;
					}
				}
			}

			OnUserOutCard(m_wCurrentUser, card, true);
		}
	}
	//�йܲ�������
	else if(wTimerID == IDI_OPERATE_CARD)
	{
		m_pITableFrame->KillGameTimer(wTimerID);

		//������ʾ
		for (WORD i = 0; i<m_cbPlayerCount; i++)
		{
			if (m_cbUserAction[i] != WIK_NULL && m_bTrustee[i])
			{
				BYTE cbOperateCard[3] = { 0 };
				if (m_cbUserAction[i] != WIK_LISTEN)
					OnUserOperateCard(i, WIK_NULL, cbOperateCard);
				else
					OnUserListenCard(i, false);
			}
		}
	}

	return false;
}

//�����¼�
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return false;
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	m_cbWaitTime = 0;
	switch (wSubCmdID)
	{
	case SUB_C_OUT_CARD:
		{
			//Ч����Ϣ
			ASSERT(wDataSize == sizeof(CMD_C_OutCard));
			if (wDataSize != sizeof(CMD_C_OutCard)) return false;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//��Ϣ����
			CMD_C_OutCard * pOutCard = (CMD_C_OutCard *)pData;
			return OnUserOutCard(pIServerUserItem->GetChairID(), pOutCard->cbCardData);
		}
	case SUB_C_OPERATE_CARD:	//������Ϣ
		{
			//Ч����Ϣ
			ASSERT(wDataSize == sizeof(CMD_C_OperateCard));
			if (wDataSize != sizeof(CMD_C_OperateCard)) return false;

			//�û�Ч��
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//��Ϣ����
			CMD_C_OperateCard * pOperateCard = (CMD_C_OperateCard *)pData;
			return OnUserOperateCard(pIServerUserItem->GetChairID(), pOperateCard->cbOperateCode, pOperateCard->cbOperateCard);
		}
	case SUB_C_LISTEN_CARD:
		{
			//Ч����Ϣ
			ASSERT(wDataSize == sizeof(CMD_C_ListenCard));
			if (wDataSize != sizeof(CMD_C_ListenCard)) return false;

			//�û�Ч��
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//��Ϣ����
			CMD_C_ListenCard * pOperateCard = (CMD_C_ListenCard *)pData;
			return OnUserListenCard(pIServerUserItem->GetChairID(), pOperateCard->bListenCard);
		}
	case SUB_C_TRUSTEE:
		{
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pData;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;

			return OnUserTrustee(pIServerUserItem->GetChairID(), pTrustee->bTrustee);
		}
	case SUB_C_REPLACE_CARD:	//�û�����
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_ReplaceCard));
			if (wDataSize != sizeof(CMD_C_ReplaceCard)) return false;
			//�û�Ч��
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//��������
			CMD_C_ReplaceCard * pReplaceCard = (CMD_C_ReplaceCard *)pData;

			//��Ϣ����
			return OnUserReplaceCard(pIServerUserItem->GetChairID(), pReplaceCard->cbCardData);
		}

#ifdef  CARD_DISPATCHER_CONTROL
	case SUB_C_SEND_CARD:
		{
			ASSERT(wDataSize==sizeof(CMD_C_SendCard));
			if(wDataSize!=sizeof(CMD_C_SendCard)) return false;

			CMD_C_SendCard * pSendCard = (CMD_C_SendCard *)pData;

			return OnUserSendCard(pSendCard->cbCardCount, pSendCard->wBankerUser, pSendCard->cbCardData, pSendCard->cbControlGameCount);
		}
#endif

	default:
		break;
	}

	return false;
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//������뷿�䣬���ٴ�ִ������
	return true;

	//��ʷ����
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		if (wChairID == INVALID_CHAIR)
		{
			return false;
		}
		m_HistoryScore.OnEventUserEnter(wChairID);
		m_wBankerUser = INVALID_CHAIR;
		m_wLianZhuang = 1;
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//�ǽ�ֲ���ִ��

	//��ʷ����
	if (bLookonUser==false)
	{
		ASSERT(wChairID != INVALID_CHAIR);
		if (wChairID == INVALID_CHAIR)
		{
			return false;
		}
		m_HistoryScore.OnEventUserLeave(wChairID);
		m_wBankerUser = INVALID_CHAIR;
		m_wLianZhuang = 1;
	}

	if( (m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )//����ģʽ
	{
		if(m_pITableFrame->IsPersonalRoomDisumme() )//��ǰ���Ѿֽ�ɢ�����¼
		{
			ZeroMemory(&m_stRecord,sizeof(m_stRecord));
		}
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData,bool bSysOut/*=false*/)
{
	//Ч��״̬
	ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY);
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY) return true;

	//�������
	ASSERT(wChairID == m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData));

	//Ч�����
	if (wChairID != m_wCurrentUser) return true;
	if (!m_GameLogic.IsValidCard(cbCardData)) return true;

	//���ܳ�����
	if (m_GameLogic.IsMagicCard(cbCardData))
	{
		BYTE cbHandCardCount = 0;
		for (BYTE i = 0; i < MAX_INDEX; i++)
		{
			cbHandCardCount += m_cbCardIndex[wChairID][i];
		}
		//��������ֻ�к�����
		if (!(m_cbMagicIndex != MAX_INDEX && m_cbCardIndex[wChairID][m_cbMagicIndex] == cbHandCardCount))
			return true;
	}

	//ɾ���˿�
 	if (!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbCardData))
	{
 		ASSERT(FALSE);
 		return false;
 	}

	//�����ֹ
	m_bEnjoinChiHu[wChairID] = false;
	m_bEnjoinChiPeng[wChairID] = false;
	m_vecEnjoinChiPeng[wChairID].clear();
	m_vecEnjoinChiHu[wChairID].clear();

	//���ñ���
	m_enSendStatus = OutCard_Send;
	m_cbSendCardData = 0;
	m_cbUserAction[wChairID] = WIK_NULL;
	m_cbPerformAction[wChairID] = WIK_NULL;

	//���Ƽ�¼
	m_wOutCardUser = wChairID;
	m_cbOutCardData = cbCardData;

	//��������
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser = wChairID;
	OutCard.cbOutCardData = cbCardData;
	OutCard.bSysOut=bSysOut;
	//������Ϣ
	addGameRecord(INVALID_CHAIR, SUB_S_OUT_CARD, &OutCard, sizeof(OutCard));
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OUT_CARD, &OutCard, sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OUT_CARD, &OutCard, sizeof(OutCard));

	m_wProvideUser = wChairID;
	m_cbProvideCard = cbCardData;

	//�û��л�
	m_wCurrentUser = (wChairID + 1) % m_cbPlayerCount;
	for(int i=0;i<m_cbPlayerCount;i++)
	{
		if(m_bPlayStatus[m_wCurrentUser])
			break;
		m_wCurrentUser = (m_wCurrentUser+1)%m_cbPlayerCount;
	}

	//��Ӧ�ж�
 	bool bAroseAction = EstimateUserRespond(wChairID, cbCardData, EstimatKind_OutCard);
 
	if(m_cbGangStatus != WIK_GANERAL)
	{
		m_bGangOutCard = true;
		m_cbGangStatus = WIK_GANERAL;
		m_wProvideGangUser = INVALID_CHAIR;
	}	
	else
	{
		m_bGangOutCard = false;
	}

 	//�ɷ��˿�
 	if (!bAroseAction) 
	{
		DispatchCardData(m_wCurrentUser);
	}
	else
	{
		m_bUserActionDone = true;
		m_pITableFrame->SetGameTimer(IDI_OPERATE_CARD, 1000, 1, 0);
	}

	m_pITableFrame->KillGameTimer(IDI_OVER_TIME);
	m_pITableFrame->SetGameTimer(IDI_OVER_TIME, 30000, 1, 0);

	return true;
}

//�û�����
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard[3])
{
	//Ч��״̬
	ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLAY);
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_PLAY) return true;

	//Ч���û� ע�⣺�������п��ܷ����˶���
	//ASSERT((wChairID == m_wCurrentUser) || (m_wCurrentUser == INVALID_CHAIR));
	if ((wChairID != m_wCurrentUser) && (m_wCurrentUser != INVALID_CHAIR))  return true;

 	//��������
 	if (m_wCurrentUser == INVALID_CHAIR)
 	{
 		//Ч��״̬
 		ASSERT(!m_bResponse[wChairID]);
 		ASSERT(m_cbUserAction[wChairID] != WIK_NULL);
 		ASSERT((cbOperateCode == WIK_NULL) || ((m_cbUserAction[wChairID]&cbOperateCode) != 0));
 
 		//Ч��״̬
 		if (m_bResponse[wChairID]) return true;
 		if(m_cbUserAction[wChairID] == WIK_NULL) return true;
 		if ((cbOperateCode!=WIK_NULL) && ((m_cbUserAction[wChairID]&cbOperateCode) == 0)) return true;
 
 		//��������
 		WORD wTargetUser = wChairID;
 		BYTE cbTargetAction = cbOperateCode;
 
 		//���ñ���
		m_bEnjoinGang[wTargetUser] = false;
 		m_bResponse[wTargetUser] = true;
 		m_cbPerformAction[wTargetUser] = cbOperateCode;
 		if(cbOperateCard[0] == 0)
		{
			m_cbOperateCard[wTargetUser][0] = m_cbProvideCard;
		}
 		else 
		{
			CopyMemory(m_cbOperateCard[wTargetUser], cbOperateCard, sizeof(m_cbOperateCard[wTargetUser]));
		}
 
		//��������
		if (cbTargetAction == WIK_NULL)
		{
			////��ֹ���ֳԺ�
			if((m_cbUserAction[wTargetUser] & WIK_CHI_HU) != 0)
			{
				m_bEnjoinChiHu[wTargetUser]=true;
				//m_vecEnjoinChiHu[wTargetUser].push_back(m_cbProvideCard);
			}
			////��ֹ������������
			//if((m_cbUserAction[wTargetUser] & WIK_PENG) != 0)
			//{
			//	m_vecEnjoinChiPeng[wTargetUser].push_back(m_cbProvideCard);
			//}
		}

 		//ִ���ж�
 		for (WORD i = 0; i < m_cbPlayerCount; i++)
 		{
 			//��ȡ����
 			BYTE cbUserAction = (!m_bResponse[i]) ? m_cbUserAction[i] : m_cbPerformAction[i];
 
 			//���ȼ���
 			BYTE cbUserActionRank = m_GameLogic.GetUserActionRank(cbUserAction);
 			BYTE cbTargetActionRank = m_GameLogic.GetUserActionRank(cbTargetAction);
 
 			//�����ж�
 			if (cbUserActionRank > cbTargetActionRank)
 			{
 				wTargetUser = i;
 				cbTargetAction = cbUserAction;
 			}
 		}
 		if (!m_bResponse[wTargetUser]) 
 			return true;
 
 		//�Ժ��ȴ�
 		if (cbTargetAction == WIK_CHI_HU)
 		{
			for (WORD i = 0; i < m_cbPlayerCount; i++)
 			{
 				if (!m_bResponse[i] && (m_cbUserAction[i]&WIK_CHI_HU))
 					return true;
 			}
 		}
 
 		//��������
 		if (cbTargetAction == WIK_NULL)
 		{
			//�û�״̬
 			ZeroMemory(m_bResponse, sizeof(m_bResponse));
 			ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
 			ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
 			ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));
 
			DispatchCardData(m_wResumeUser,m_cbGangStatus != WIK_GANERAL);
 			return true;
 		}
 
 		//��������
 		BYTE cbTargetCard = m_cbOperateCard[wTargetUser][0];
 
 		//���Ʊ���
 		m_enSendStatus = Gang_Send;
		m_cbSendCardData = 0;
 		m_wOutCardUser = INVALID_CHAIR;
		m_cbOutCardData = 0;
 
 		//���Ʋ���
 		if (cbTargetAction == WIK_CHI_HU)
 		{
 			//������Ϣ
 			m_cbChiHuCard = cbTargetCard;
 
			WORD wChiHuUser = m_wBankerUser;
			for(int i = 0; i < m_cbPlayerCount; i++)
 			{
 				wChiHuUser = (m_wBankerUser + i) % m_cbPlayerCount;
				//�����ж�
 				if ((m_cbPerformAction[wChiHuUser]&WIK_CHI_HU) == 0)
 					continue;
 
 				//�����ж�
 				BYTE cbWeaveItemCount = m_cbWeaveItemCount[wChiHuUser];
 				tagWeaveItem * pWeaveItem = m_WeaveItemArray[wChiHuUser];
 				m_dwChiHuKind[wChiHuUser] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChiHuUser], pWeaveItem, cbWeaveItemCount, m_cbChiHuCard, m_ChiHuRight[wChiHuUser]);

 				//�����˿�
 				if (m_dwChiHuKind[wChiHuUser] != WIK_NULL) 
 				{
 					wTargetUser = wChiHuUser;
					//break;
 				}				
 			}
 
 			//������Ϸ
 			ASSERT(m_dwChiHuKind[wTargetUser] != WIK_NULL);
 			OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
 
 			return true;
 		}
 
 		//����˿�
 		ASSERT(m_cbWeaveItemCount[wTargetUser] < MAX_WEAVE);
 		WORD wIndex = m_cbWeaveItemCount[wTargetUser]++;
 		m_WeaveItemArray[wTargetUser][wIndex].cbParam = WIK_GANERAL;
 		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard = cbTargetCard;
 		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind = cbTargetAction;
 		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wTargetUser : m_wProvideUser;
 		m_WeaveItemArray[wTargetUser][wIndex].cbCardData[0] = cbTargetCard;
 		if(cbTargetAction&(WIK_LEFT | WIK_CENTER | WIK_RIGHT))
 		{
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[1] = m_cbOperateCard[wTargetUser][1];
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[2] = m_cbOperateCard[wTargetUser][2];
 		}
 		else
 		{
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[1] = cbTargetCard;
 			m_WeaveItemArray[wTargetUser][wIndex].cbCardData[2] = cbTargetCard;
 			if(cbTargetAction & WIK_GANG)
			{
				m_WeaveItemArray[wTargetUser][wIndex].cbParam = WIK_FANG_GANG;
				m_WeaveItemArray[wTargetUser][wIndex].cbCardData[3] = cbTargetCard;
			}
 		}
 
 		//ɾ���˿�
 		switch (cbTargetAction)
 		{
 		case WIK_LEFT:		//���Ʋ���
 			{
 				//ɾ���˿�
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], &m_cbOperateCard[wTargetUser][1], 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;
 
 				break;
 			}
 		case WIK_RIGHT:		//���Ʋ���
 			{
 				//ɾ���˿�
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], &m_cbOperateCard[wTargetUser][1], 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;
 
 				break;
 			}
 		case WIK_CENTER:	//���Ʋ���
 			{
 				//ɾ���˿�
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], &m_cbOperateCard[wTargetUser][1], 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;
 
 				break;
 			}
 		case WIK_PENG:		//���Ʋ���
 			{
 				//ɾ���˿�
 				BYTE cbRemoveCard[] = {cbTargetCard,cbTargetCard};
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], cbRemoveCard, 2))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_cbChiPengCount[wTargetUser]++;

 				break;
 			}
 		case WIK_GANG:		//���Ʋ���
 			{
 				//ɾ���˿�,��������ֻ���ڷŸ�
 				BYTE cbRemoveCard[] = {cbTargetCard, cbTargetCard, cbTargetCard};
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], cbRemoveCard, CountArray(cbRemoveCard)))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
 
 				break;
 			}
 		default:
 			ASSERT(FALSE);
 			return false;
 		}
 
 		//������
 		CMD_S_OperateResult OperateResult;
 		ZeroMemory(&OperateResult, sizeof(OperateResult));
 		OperateResult.wOperateUser = wTargetUser;
 		OperateResult.cbOperateCode = cbTargetAction;
 		OperateResult.wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wTargetUser : m_wProvideUser;
 		OperateResult.cbOperateCard[0] = cbTargetCard;
 		if(cbTargetAction & (WIK_LEFT | WIK_CENTER | WIK_RIGHT))
		{
			CopyMemory(&OperateResult.cbOperateCard[1], &m_cbOperateCard[wTargetUser][1], 2 * sizeof(BYTE));
		}
 		else if(cbTargetAction&WIK_PENG)
 		{
 			OperateResult.cbOperateCard[1] = cbTargetCard;
 			OperateResult.cbOperateCard[2] = cbTargetCard;
 		}
 
 		//�û�״̬
 		ZeroMemory(m_bResponse, sizeof(m_bResponse));
 		ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
 		ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
 		ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));
 
 		//����Ǹ���
 		if(cbTargetAction != WIK_GANG)
 		{
			m_wProvideUser = INVALID_CHAIR;
			m_cbProvideCard = 0;

 			tagGangCardResult gcr;
 			m_cbUserAction[wTargetUser] |= m_GameLogic.AnalyseGangCardEx(m_cbCardIndex[wTargetUser], m_WeaveItemArray[wTargetUser], m_cbWeaveItemCount[wTargetUser],0, gcr);

			if(m_bTing[wTargetUser] == false)
			{
				BYTE cbCount = 0;
				CMD_S_Hu_Data HuData;
				ZeroMemory(&HuData,sizeof(HuData));
				cbCount =m_GameLogic.AnalyseTingCard(m_cbCardIndex[wTargetUser],m_WeaveItemArray[wTargetUser],m_cbWeaveItemCount[wTargetUser],HuData.cbOutCardCount,HuData.cbOutCardData,HuData.cbHuCardCount,HuData.cbHuCardData);
				if(cbCount >0)
				{
					m_cbUserAction[wTargetUser] |= WIK_LISTEN; 

					for(int i=0;i<MAX_COUNT;i++)
					{
						if(HuData.cbHuCardCount[i]>0)
						{
							for(int j=0;j<HuData.cbHuCardCount[i];j++)
							{
								HuData.cbHuCardRemainingCount[i][j] = GetRemainingCount(wTargetUser,HuData.cbHuCardData[i][j]);
							}
						}
						else
							break;
					}
					//addGameRecord(wTargetUser, SUB_S_HU_CARD, &HuData, sizeof(HuData));
					m_pITableFrame->SendTableData(wTargetUser,SUB_S_HU_CARD,&HuData,sizeof(HuData));
				}

			}
 			OperateResult.cbActionMask |= m_cbUserAction[wTargetUser];
 		}
 
 		//������Ϣ
		addGameRecord(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 
		m_pITableFrame->KillGameTimer(IDI_OVER_TIME);
		m_pITableFrame->SetGameTimer(IDI_OVER_TIME, 30000, 1, 0);

 		//�����û�
 		m_wCurrentUser = wTargetUser;
 
 		//���ƴ���
 		if (cbTargetAction == WIK_GANG)
 		{
 			m_cbGangStatus = WIK_FANG_GANG;
			m_wProvideGangUser = (m_wProvideUser == INVALID_CHAIR) ? wTargetUser : m_wProvideUser;
			m_bGangCard[wTargetUser] = true;
			m_cbGangCount[wTargetUser]++;

			/*�Ÿܲ����ܺ�
			//Ч�鶯��
			bool bAroseAction = false;
			bAroseAction = EstimateUserRespond(wTargetUser, cbOperateCard[0], EstimatKind_GangCard);

			//�����˿�
			if (!bAroseAction)
			{
				DispatchCardData(wTargetUser, true);
			}
			*/
			DispatchCardData(wTargetUser, true);
 		}
 
 		return true;
 	}
 
 	//��������
 	if (m_wCurrentUser == wChairID)
 	{
 		//Ч�����
 		//ASSERT((cbOperateCode != WIK_NULL) && ((m_cbUserAction[wChairID]&cbOperateCode) != 0));
 		//if ((cbOperateCode == WIK_NULL) || ((m_cbUserAction[wChairID]&cbOperateCode) == 0)) return false;
 
 		//�˿�Ч��
 		ASSERT((cbOperateCode == WIK_NULL) || (cbOperateCode == WIK_CHI_HU) || (m_GameLogic.IsValidCard(cbOperateCard[0])));
 		if ((cbOperateCode != WIK_NULL) && (cbOperateCode != WIK_CHI_HU) && (!m_GameLogic.IsValidCard(cbOperateCard[0]))) return false;
 
 		//���ñ���
 		m_cbUserAction[m_wCurrentUser] = WIK_NULL;
 		m_cbPerformAction[m_wCurrentUser] = WIK_NULL;
 
 		//ִ�ж���
 		switch (cbOperateCode)
 		{
 		case WIK_GANG:			//���Ʋ���
 			{
				m_enSendStatus = Gang_Send;

 				//��������
 				BYTE cbWeaveIndex = 0xFF;
 				BYTE cbCardIndex = m_GameLogic.SwitchToCardIndex(cbOperateCard[0]);
				WORD wProvideUser = wChairID;
				BYTE cbGangKind = WIK_MING_GANG;
 				//���ƴ���
 				if (m_cbCardIndex[wChairID][cbCardIndex] == 1)
 				{
 					//Ѱ�����
 					for (BYTE i = 0; i < m_cbWeaveItemCount[wChairID]; i++)
 					{
 						BYTE cbWeaveKind = m_WeaveItemArray[wChairID][i].cbWeaveKind;
 						BYTE cbCenterCard = m_WeaveItemArray[wChairID][i].cbCenterCard;
 						if ((cbCenterCard == cbOperateCard[0]) && (cbWeaveKind == WIK_PENG))
 						{
 							cbWeaveIndex = i;
 							break;
 						}
 					}
 
 					//Ч�鶯��
 					ASSERT(cbWeaveIndex != 0xFF);
 					if (cbWeaveIndex == 0xFF) return false;
 
					cbGangKind = WIK_MING_GANG;

 					//����˿�
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbParam = WIK_MING_GANG;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind = cbOperateCode;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard = cbOperateCard[0];
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCardData[3] = cbOperateCard[0];
 
 					//���Ƶ÷�
 					wProvideUser = m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser;
 				}
 				else
 				{
 					//�˿�Ч��
 					ASSERT(m_cbCardIndex[wChairID][cbCardIndex] == 4);
 					if (m_cbCardIndex[wChairID][cbCardIndex] != 4) 
 						return false;
 
 					//���ñ���
 					cbGangKind = WIK_AN_GANG;
 					cbWeaveIndex = m_cbWeaveItemCount[wChairID]++;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbParam = WIK_AN_GANG;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser = wChairID;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind = cbOperateCode;
 					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard = cbOperateCard[0];
 					for(BYTE j = 0; j < 4; j++) 
					{
						m_WeaveItemArray[wChairID][cbWeaveIndex].cbCardData[j] = cbOperateCard[0];
					}
 				}
 
 				//ɾ���˿�
 				m_cbCardIndex[wChairID][cbCardIndex] = 0;
				m_cbGangStatus = cbGangKind;
				m_wProvideGangUser = wProvideUser;
				m_bGangCard[wChairID] = true;
				m_cbGangCount[wChairID]++;
 
 				//������
 				CMD_S_OperateResult OperateResult;
 				ZeroMemory(&OperateResult, sizeof(OperateResult));
 				OperateResult.wOperateUser = wChairID;
 				OperateResult.wProvideUser = wProvideUser;
 				OperateResult.cbOperateCode = cbOperateCode;
 				OperateResult.cbOperateCard[0] = cbOperateCard[0];
 
 				//������Ϣ
				addGameRecord(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
 				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));

 				//Ч�鶯��
 				bool bAroseAction = false;
 				if (cbGangKind == WIK_MING_GANG && m_cbQiangGang) //������ܺ�
				{
					bAroseAction=EstimateUserRespond(wChairID, cbOperateCard[0], EstimatKind_GangCard);
				}
 
 				//�����˿�
 				if (!bAroseAction)
 				{
 					DispatchCardData(wChairID, true);
 				}
 
 				return true;
 			}
 		case WIK_CHI_HU:		//����
 			{
 				//��ͨ����
 				BYTE cbWeaveItemCount = m_cbWeaveItemCount[wChairID];
 				tagWeaveItem * pWeaveItem = m_WeaveItemArray[wChairID];
 				if(!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], &m_cbSendCardData, 1))
 				{
 					ASSERT(FALSE);
 					return false;
 				}
				m_dwChiHuKind[wChairID] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID], pWeaveItem, cbWeaveItemCount, m_cbSendCardData, m_ChiHuRight[wChairID], m_cbOutCardData == 0/*δ���Ƶ�һ�ѣ�������4�����к���*/);
 
 				//������Ϣ
				m_cbChiHuCard = m_cbSendCardData;
				m_cbProvideCard = m_cbSendCardData;
				
 				//������Ϸ
 				OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
 
 				return true;
 			}
 		}
 
 		return true;
 	}

	return false;
}


//�û�����
bool CTableFrameSink::OnUserListenCard(WORD wChairID, bool bListenCard)
{
	if(bListenCard)
	{
		ASSERT(!m_bTing[wChairID]);
		if(WIK_LISTEN == m_GameLogic.AnalyseTingCard(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID]))
		{
			m_bTing[wChairID] = true;
			CMD_S_ListenCard ListenCard;
			ZeroMemory(&ListenCard,sizeof(ListenCard));

			ListenCard.wListenUser = wChairID;
			ListenCard.bListen = true;
			//������Ϣ
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LISTEN_CARD,&ListenCard,sizeof(ListenCard));

			for(int i=0;i<m_cbPlayerCount;i++)
			{
				if( i != wChairID)
					m_pITableFrame->SendTableData(i,SUB_S_LISTEN_CARD,&ListenCard,sizeof(ListenCard));
			}

			//�����������
			m_cbHuCardCount[wChairID] = m_GameLogic.GetHuCard(m_cbCardIndex[wChairID],m_WeaveItemArray[wChairID],m_cbWeaveItemCount[wChairID],m_cbHuCardData[wChairID]);
			ASSERT(m_cbHuCardCount[wChairID]>0);
			ListenCard.cbHuCardCount = m_cbHuCardCount[wChairID];
			CopyMemory(ListenCard.cbHuCardData,m_cbHuCardData[wChairID],sizeof(ListenCard.cbHuCardData));

			m_pITableFrame->SendTableData(wChairID,SUB_S_LISTEN_CARD,&ListenCard,sizeof(ListenCard));
			

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{

		ASSERT(m_bTing[wChairID]);
		m_bTing[wChairID] = false;

		CMD_S_ListenCard ListenCard;
		ListenCard.wListenUser = wChairID;
		ListenCard.bListen = false;

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_LISTEN_CARD, &ListenCard, sizeof(ListenCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_LISTEN_CARD, &ListenCard, sizeof(ListenCard));
		return true;
	}
}

//�û��й�
bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	//Ч��״̬
	ASSERT((wChairID < m_cbPlayerCount));
	if ((wChairID>=m_cbPlayerCount)) return false;

	m_bTrustee[wChairID]=bTrustee;

	CMD_S_Trustee Trustee;
	Trustee.bTrustee=bTrustee;
	Trustee.wChairID = wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));

	if(bTrustee)
	{
		if(wChairID == m_wCurrentUser && m_bUserActionDone==false)
		{
			
			BYTE cardindex=INVALID_BYTE;
			if(m_cbSendCardData != 0)
				cardindex = m_GameLogic.SwitchToCardIndex(m_cbSendCardData);
			else
			{
				for(int i=0;i<MAX_INDEX;i++)
				{
					if(m_cbCardIndex[wChairID][i]>0)
					{
						cardindex = i;
						break;
					}
				}
			}

			ASSERT(cardindex != INVALID_BYTE);
			OnUserOutCard(wChairID,m_GameLogic.SwitchToCardData(cardindex));
		}
		else if(m_wCurrentUser == INVALID_CHAIR && m_bUserActionDone==false)
		{
			BYTE operatecard[3]={0};
			OnUserOperateCard(wChairID,WIK_NULL,operatecard);
		}
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnUserReplaceCard(WORD wChairID, BYTE cbCardData)
{
	//�������
	ASSERT(m_GameLogic.IsValidCard(cbCardData));
	//Ч�����
	if(!m_GameLogic.IsValidCard(cbCardData))  return false;

	//ɾ���˿�
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbCardData)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//״̬����
	m_enSendStatus = BuHua_Send;
	m_cbGangStatus = WIK_GANERAL;
	m_wProvideGangUser = INVALID_CHAIR;

	//��������
	CMD_S_ReplaceCard ReplaceCard;
	ReplaceCard.wReplaceUser = wChairID;
	ReplaceCard.cbReplaceCard = cbCardData;

	//������Ϣ
	addGameRecord(INVALID_CHAIR, SUB_S_REPLACE_CARD, &ReplaceCard, sizeof(ReplaceCard));
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_REPLACE_CARD, &ReplaceCard, sizeof(ReplaceCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_REPLACE_CARD, &ReplaceCard, sizeof(ReplaceCard));

	//�ɷ��˿�
	DispatchCardData(wChairID, true);

	return true;
}

//�����˿�
bool CTableFrameSink::OnUserSendCard(BYTE cbCardCount, WORD wBankerUser, BYTE cbCardData[], BYTE cbControlGameCount)
{
#ifdef  CARD_DISPATCHER_CONTROL
	ASSERT(cbCardCount == MAX_REPERTORY && wBankerUser < GAME_PLAYER);
	if(cbCardCount != MAX_REPERTORY || wBankerUser >= GAME_PLAYER) return false;

	//���ÿ��ƾ���
	m_cbControlGameCount = cbControlGameCount;

	m_wControBankerUser = wBankerUser;

	//���������˿�
	CopyMemory(m_cbControlRepertoryCard, cbCardData, sizeof(m_cbControlRepertoryCard));
#endif
	return true;
}

//���Ͳ���
bool CTableFrameSink::SendOperateNotify()
{
	//������ʾ
	for (WORD i=0;i<m_cbPlayerCount;i++)
	{
		if (m_cbUserAction[i]!=WIK_NULL)
		{
			//��������
			CMD_S_OperateNotify OperateNotify;
			OperateNotify.cbActionCard=m_cbProvideCard;
			OperateNotify.cbActionMask=m_cbUserAction[i];

			//��������
			addGameRecord(i,SUB_S_OPERATE_NOTIFY, &OperateNotify, sizeof(OperateNotify));
			m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
			m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
			
// 			if(m_bTrustee[i])
// 			{
// 				m_pITableFrame->SetGameTimer(IDI_OPERATE_CARD+i,1000,1,0);
// 			}
		}
	}

	return true;
}

//ȡ���˿�
BYTE CTableFrameSink::GetSendCard(bool bTail)
{
	//�����˿�
	m_cbSendCardCount++;
	m_cbLeftCardCount--;

	BYTE cbSendCardData = 0;
	
	BYTE cbIndexCard;
	if(bTail)
	{	
		cbSendCardData = m_cbRepertoryCard[m_cbMinusLastCount];
		m_cbMinusLastCount++;
	}
	else
	{
		m_cbMinusHeadCount++;
		cbIndexCard = MAX_REPERTORY - m_cbMinusHeadCount;
		cbSendCardData=m_cbRepertoryCard[cbIndexCard];
	}	

	//������Ϣ
	ASSERT(m_wHeapHead != INVALID_CHAIR && m_wHeapTail != INVALID_CHAIR);
	if(!bTail)
	{
		//�л�����
		BYTE cbHeapCount = m_cbHeapCardInfo[m_wHeapHead][0] + m_cbHeapCardInfo[m_wHeapHead][1];
		if (cbHeapCount == HEAP_FULL_COUNT) 
		{
			m_wHeapHead = (m_wHeapHead + GAME_PLAYER - 1) % CountArray(m_cbHeapCardInfo);
		}
		m_cbHeapCardInfo[m_wHeapHead][0]++;
	}
	else
	{
		//�л�����
		BYTE cbHeapCount = m_cbHeapCardInfo[m_wHeapTail][0] + m_cbHeapCardInfo[m_wHeapTail][1];
		if (cbHeapCount == HEAP_FULL_COUNT) 
		{
			m_wHeapTail = (m_wHeapTail + 1) % CountArray(m_cbHeapCardInfo);
		}
		m_cbHeapCardInfo[m_wHeapTail][1]++;
	}

	return cbSendCardData;
}

//�ɷ��˿�
bool CTableFrameSink::DispatchCardData(WORD wSendCardUser, bool bTail)
{
	//״̬Ч��
	ASSERT(wSendCardUser != INVALID_CHAIR);
	if (wSendCardUser == INVALID_CHAIR) return false;

	ASSERT(m_enSendStatus != Not_Send);
	if(m_enSendStatus == Not_Send) return false;

	WORD wCurrentUser = wSendCardUser;

	//�����˿�
	if ((m_wOutCardUser != INVALID_CHAIR) && (m_cbOutCardData != 0))
	{
		m_cbOutCardCount++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]] = m_cbOutCardData;
		m_cbDiscardCount[m_wOutCardUser]++;
	}

	//��ׯ����
	if (m_cbLeftCardCount <= (m_cbMaCount>1?m_cbMaCount:m_cbMaCount) )//2-6��Ҫ����һ�ţ�һ��ȫ�в���
	{
		m_cbChiHuCard = 0;
		m_wProvideUser = INVALID_CHAIR;
		OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);

		return true;
	}

	//�����˿�
	m_cbProvideCard = GetSendCard(bTail);	
	m_cbSendCardData = m_cbProvideCard;
	m_wLastCatchCardUser = wSendCardUser;
	//�����ֹ���Ƶ���
	m_bEnjoinChiHu[wCurrentUser] = false;
	m_vecEnjoinChiHu[wCurrentUser].clear();
	m_bEnjoinChiPeng[wCurrentUser] = false;		
	m_vecEnjoinChiPeng[wCurrentUser].clear();
	m_bEnjoinGang[wCurrentUser] = false;
	
	//���ñ���	
	m_wOutCardUser = INVALID_CHAIR;
	m_cbOutCardData = 0;	
	m_wCurrentUser = wCurrentUser;
	m_wProvideUser = wCurrentUser;		
	m_bGangOutCard = false;
	
	if(bTail)//��β��ȡ�ƣ�˵����Ҹ�����,�������
	{
		CalGangScore();
	}

	//����
	m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;
	m_cbUserCatchCardCount[wCurrentUser]++;

	if(!m_bTrustee[wCurrentUser])
	{
		//�����ж�
		if(!m_bEnjoinChiHu[wCurrentUser])
		{		
			CChiHuRight chr;
			m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]--;
			m_cbUserAction[wCurrentUser] |= m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wCurrentUser], m_WeaveItemArray[wCurrentUser],
				m_cbWeaveItemCount[wCurrentUser], m_cbSendCardData, chr);
			m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
		}

		//�����ж�
		if ((!m_bEnjoinGang[wCurrentUser]) && (m_cbLeftCardCount > m_cbEndLeftCount) && !m_bTing[wCurrentUser])
		{
			tagGangCardResult GangCardResult;
			m_cbUserAction[wCurrentUser] |= m_GameLogic.AnalyseGangCardEx(m_cbCardIndex[wCurrentUser],
				m_WeaveItemArray[wCurrentUser], m_cbWeaveItemCount[wCurrentUser],m_cbProvideCard ,GangCardResult);
		}
	}

		//��������
	CMD_S_SendCard SendCard;
	ZeroMemory(&SendCard,sizeof(SendCard));

	//�����ж�
	CMD_S_Hu_Data HuData;
	ZeroMemory(&HuData,sizeof(HuData));

	BYTE cbCount = 0;
	if(m_bTing[wCurrentUser] == false)
	{
		cbCount =m_GameLogic.AnalyseTingCard(m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],HuData.cbOutCardCount,HuData.cbOutCardData,HuData.cbHuCardCount,HuData.cbHuCardData);
		if(cbCount >0)
		{
			m_cbUserAction[wCurrentUser] |= WIK_LISTEN; 
			
			for(int i=0;i<MAX_COUNT;i++)
			{
				if(HuData.cbHuCardCount[i]>0)
				{
					for(int j=0;j<HuData.cbHuCardCount[i];j++)
					{
						HuData.cbHuCardRemainingCount[i][j] = GetRemainingCount(wCurrentUser,HuData.cbHuCardData[i][j]);
					}
				}
				else
					break;
			}
			//addGameRecord(wCurrentUser, SUB_S_HU_CARD, &HuData, sizeof(HuData));
			m_pITableFrame->SendTableData(wCurrentUser,SUB_S_HU_CARD,&HuData,sizeof(HuData));
		}
	}

	SendCard.wSendCardUser = wSendCardUser;
	SendCard.wCurrentUser = wCurrentUser;
	SendCard.bTail = bTail;
	SendCard.cbActionMask = m_cbUserAction[wCurrentUser];
	SendCard.cbCardData = m_cbProvideCard;
	
	//��������
	addGameRecord(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

	m_bUserActionDone=false;
	if(m_bTrustee[wCurrentUser])
	{
		m_bUserActionDone=true;
		m_pITableFrame->SetGameTimer(IDI_OUT_CARD,1000,1,0);
	}

	m_pITableFrame->KillGameTimer(IDI_OVER_TIME);
	m_pITableFrame->SetGameTimer(IDI_OVER_TIME, 30000, 1, 0);

	return true;
}

//��Ӧ�ж�
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//��������
	bool bAroseAction = false;

	//�û�״̬
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

	//�����ж�
	for (WORD i = 0; i < m_cbPlayerCount; i++)
	{
		//�û�����
		if (wCenterUser == i || !m_bPlayStatus[i] || m_bTrustee[i]) continue;

		//��������
		if (EstimatKind == EstimatKind_OutCard )
		{
			//�����ж�
			if(!m_bEnjoinChiPeng[i])
			{
				bool bPeng = true;
				for(UINT j = 0; j < m_vecEnjoinChiPeng[i].size(); j++)
				{
					if(m_vecEnjoinChiPeng[i][j] == cbCenterCard)
					{
						bPeng = false;
					}
				}
				if(bPeng)
				{
					//�����ж�
					m_cbUserAction[i] |= m_GameLogic.EstimatePengCard(m_cbCardIndex[i], cbCenterCard);
				}

				//�����ж�
				if(m_cbLeftCardCount > m_cbEndLeftCount && !m_bEnjoinGang[i]) 
				{
					m_cbUserAction[i] |= m_GameLogic.EstimateGangCard(m_cbCardIndex[i], cbCenterCard);
				}
			}
		}

		//����ں�(������ڻ������ܺ�)
   		if ((EstimatKind == EstimatKind_GangCard) || (m_cbJiePao && !m_bEnjoinChiHu[i]))
   		{
			if(m_cbMagicIndex == MAX_INDEX || (m_cbMagicIndex != MAX_INDEX && cbCenterCard != m_GameLogic.SwitchToCardData(m_cbMagicIndex)))
			{
				bool bChiHu = true;
				for(UINT j = 0; j < m_vecEnjoinChiHu[i].size(); j++)
				{
					if(m_vecEnjoinChiHu[i][j] == cbCenterCard)
					{
						bChiHu = false;
						break;
					}
				}
				if(bChiHu)
				{
					//�Ժ��ж�
					CChiHuRight chr;
					BYTE cbWeaveCount = m_cbWeaveItemCount[i];
					BYTE cbAction = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i], m_WeaveItemArray[i], cbWeaveCount, cbCenterCard, chr);
					m_cbUserAction[i] |= cbAction;
				}
			}
   		}

		//����ж�
		if (m_cbUserAction[i] != WIK_NULL) 
			bAroseAction = true;
	}

	//�������
	if (bAroseAction) 
	{
		//���ñ���
		m_wProvideUser = wCenterUser;
		m_cbProvideCard = cbCenterCard;
		m_wResumeUser = m_wCurrentUser;
		m_wCurrentUser = INVALID_CHAIR;

		//������ʾ
		SendOperateNotify();

		return true;
	}

	return false;
}

//���
void CTableFrameSink::CalHuPaiScore(LONGLONG lEndScore[GAME_PLAYER])
{
	//��ʼ��
	ZeroMemory(lEndScore,sizeof(LONGLONG)*GAME_PLAYER);
	ZeroMemory(m_cbUserMaCount,sizeof(m_cbUserMaCount));

	bool bNoNegative = (m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0 || (m_pGameServiceOption->wServerType & SCORE_GENRE_POSITIVE) != 0;

	SCORE lCellScore = m_pITableFrame->GetCellScore();

	SCORE lUserScore[GAME_PLAYER] = {0};//������Ϸ�
	SCORE lTempScore = 0;
	SCORE lTempAllScore = 0;
	for(int i = 0; i < GAME_PLAYER; i++)
	{
		if(!m_bPlayStatus[i])
			continue;
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		ASSERT(pIServerUserItem);
		if(pIServerUserItem)
		{
			lUserScore[i] = pIServerUserItem->GetUserScore();
		}		
	}

	WORD wWinUser[GAME_PLAYER] = {0};
	BYTE cbWinCount = 0;

	for(int i = 0; i < GAME_PLAYER; i++)
	{
		if(WIK_CHI_HU == m_dwChiHuKind[(m_wBankerUser + i) % GAME_PLAYER])
		{
			wWinUser[cbWinCount++] = (m_wBankerUser + i) % GAME_PLAYER;

			//ͳ�ƺ��ƴ���
			m_stRecord.cbHuCount[(m_wBankerUser + i) % GAME_PLAYER]++;
		}
	}

	if(cbWinCount > 0)//���˺���
	{
		bool bZiMo= (m_wProvideUser == wWinUser[0]);
		if(bZiMo)//����
		{
			BYTE cbTimes = GetTimes(wWinUser[0]);
			for(int i=0;i<GAME_PLAYER;i++)
			{
				if(!m_bPlayStatus[i])
					continue;
				if(i != wWinUser[0])
				{
					lEndScore[i] -= cbTimes * lCellScore * diFenUnit;
					lEndScore[wWinUser[0]] += cbTimes * lCellScore * diFenUnit;
				}
			}
		}
		else if(m_cbGangStatus==WIK_MING_GANG)//����
		{
			for(int i=0;i<cbWinCount;i++)
			{
				BYTE cbTimes=GetTimes(wWinUser[i]);

				for(int j=0;j<GAME_PLAYER;j++)
				{
					if(!m_bPlayStatus[j])
						continue;
					if(j != wWinUser[i])
					{
						lEndScore[wWinUser[i]] += cbTimes * lCellScore * diFenUnit;
					}
				}
				lEndScore[m_wProvideUser] -= lEndScore[wWinUser[i]];
			}
		}
		else//����
		{
			for(int i=0;i<cbWinCount;i++)
			{
				BYTE cbTimes=GetTimes(wWinUser[i]);
				lEndScore[wWinUser[i]] += cbTimes * lCellScore * diFenUnit;
				lEndScore[m_wProvideUser] -= cbTimes * lCellScore * diFenUnit;
			}
		}

		//˭��˭��ׯ
		m_wBankerUser = wWinUser[0];
		if(cbWinCount > 1)//�����Һ��ƣ������ߵ�ׯ
			m_wBankerUser = m_wProvideUser;
	}
	else//��ׯ
	{
		m_wBankerUser = m_wLastCatchCardUser;//���һ�����Ƶ��˵�ׯ
	}

}

void CTableFrameSink::CalGangScore()
{
	SCORE lcell = m_pITableFrame->GetCellScore();
	if(m_cbGangStatus == WIK_FANG_GANG)//�Ÿ�һ�ҿ۷�
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(!m_bPlayStatus[i])
				continue;
			if(i != m_wCurrentUser)
			{
				m_lUserGangScore[m_wProvideGangUser] -= lcell * diFenUnit;
				m_lUserGangScore[m_wCurrentUser] += lcell * diFenUnit;
			}
		}
		//��¼���ܴ���
		m_stRecord.cbMingGang[m_wCurrentUser]++;
	}
	else if(m_cbGangStatus == WIK_MING_GANG)//����ÿ�ҳ�1��
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(!m_bPlayStatus[i])
				continue;
			if(i != m_wCurrentUser)
			{
				m_lUserGangScore[i] -= lcell * diFenUnit;
				m_lUserGangScore[m_wCurrentUser] += lcell * diFenUnit;
			}
		}
		//��¼���ܴ���
		m_stRecord.cbMingGang[m_wCurrentUser]++;
	}
	else if(m_cbGangStatus == WIK_AN_GANG)//����ÿ�ҳ�2��
	{
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(!m_bPlayStatus[i])
				continue;
			if(i != m_wCurrentUser)
			{
				m_lUserGangScore[i] -= 2 * lcell * diFenUnit;
				m_lUserGangScore[m_wCurrentUser] += 2 * lcell * diFenUnit;
			}
		}
		//��¼���ܴ���
		m_stRecord.cbAnGang[m_wCurrentUser]++;
	}
}
//Ȩλ����
void CTableFrameSink::FiltrateRight(WORD wWinner, CChiHuRight &chr)
{
	//����
	if(wWinner == m_wProvideUser)
	{
		chr |= CHR_ZI_MO;

	}
	else if(m_cbGangStatus == WIK_MING_GANG)
	{
		chr |= CHR_QIANG_GANG_HU;
	}
	else
	{
		//chr |= CHR_JIE_PAO;
	}

}

//���û���
void CTableFrameSink::SetGameBaseScore(LONG lBaseScore)
{
	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
}

BYTE CTableFrameSink::GetTimes(WORD wChairId)
{
	BYTE cbScore = 0;
	ASSERT(m_cbMaCount >= 0 && m_cbMaCount <= 6);
	m_cbUserMaCount[wChairId]=m_cbMaCount;

	if(m_cbMaCount == 1)//һ��ȫ��
	{
		BYTE carddata = m_cbRepertoryCard[m_cbMinusLastCount];
		if(m_GameLogic.GetCardColor(carddata) < 0x30)
		{
			cbScore = m_GameLogic.GetCardValue(carddata);
		}
		else//����10��
		{
			cbScore = 10;
		}
		//ͳ���������,һ��ȫ������Ӧ��Ϊ����
		m_stRecord.cbMaCount[wChairId]+=cbScore;
	}
	else//2-6��
	{
		//ûѡ����Ļ�,��������û���У���һ����
		//if(m_cbUserMaCount[wChairId] && m_cbCardIndex[wChairId][m_cbMagicIndex] == 0 && m_GameLogic.SwitchToCardIndex(m_cbProvideCard) != m_cbMagicIndex)
		//	m_cbUserMaCount[wChairId]++;

		for(int i=0;i<m_cbUserMaCount[wChairId];i++)
		{
			BYTE carddata = m_cbRepertoryCard[m_cbMinusLastCount+i];

			if(m_GameLogic.GetCardValue(carddata)%4 == 1)//1,5,9,���� ������
			{
				cbScore += 2;
				//ͳ���������
				m_stRecord.cbMaCount[wChairId]++;
			}
		}
	}

	return cbScore+2;//��������+2
}

BYTE CTableFrameSink::GetRemainingCount(WORD wChairID,BYTE cbCardData)
{
	BYTE cbIndex = m_GameLogic.SwitchToCardIndex(cbCardData);
	BYTE nCount=0;
	for(int i=m_cbMinusLastCount;i<MAX_REPERTORY-m_cbMinusHeadCount;i++)
	{
		if(m_cbRepertoryCard[i] == cbCardData)
			nCount++;
	}
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if( i == wChairID)
			continue;

		nCount += m_cbCardIndex[i][cbIndex];
	}
	return nCount;
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	
	//�Զ��й�
	OnUserTrustee(wChairID,true);


	return true;
}



void CTableFrameSink::addGameRecord(WORD wChairId, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
}


//////////////////////////////////////////////////////////////////////////////////
