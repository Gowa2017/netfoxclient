#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <conio.h>
#include <locale>

//////////////////////////////////////////////////////////////////////////

//��������ԭ��
unsigned char chCommonEncryption[AESENCRYPTION_LENGTH] = 
{
	0x32, 0x43, 0xf6, 0xa8, 
	0x88, 0x5a, 0x30, 0x8d, 
	0x31, 0x31, 0x98, 0xa2, 
	0xe0, 0x37, 0x07, 0x34
};

//��̬����
const BYTE			CTableFrameSink::m_GameStartMode=START_MODE_ALL_READY;	//��ʼģʽ

//���������Ϣ
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo;	//���USERIDӳ�������Ϣ
//�����û�����
CList<ROOMUSERCONTROL, ROOMUSERCONTROL&> g_ListRoomUserControl;		//�����û���������
//�������Ƽ�¼
CList<CString, CString&> g_ListOperationRecord;						//�������Ƽ�¼

ROOMUSERINFO	g_CurrentQueryUserInfo;								//��ǰ��ѯ�û���Ϣ

//ȫ�ֱ���
SCORE						g_lRoomStorageStart = 0LL;								//������ʼ���
SCORE						g_lRoomStorageCurrent = 0LL;							//����Ӯ��
SCORE						g_lStorageDeductRoom = 0LL;								//�ؿ۱���
SCORE						g_lStorageMax1Room = 0LL;								//���ⶥ
SCORE						g_lStorageMul1Room = 0LL;								//ϵͳ��Ǯ����
SCORE						g_lStorageMax2Room = 0LL;								//���ⶥ
SCORE						g_lStorageMul2Room = 0LL;								//ϵͳ��Ǯ����


//��ʱ�� 0~30
#define IDI_GAME_COMPAREEND					1									//������ʱ��
#define IDI_GAME_OPENEND					2									//������ʱ��
#define IDI_ADD_SCORE						3									//������ʱ��

#define TIME_GAME_COMPAREEND				5000								//������ʱ��
#define TIME_GAME_OPENEND					5000								//������ʱ��
#define TIME_ADD_SCORE						40000								//������ʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
	AllocConsole();
	freopen("CON", "wt", stdout);

	int a = sizeof(CMD_C_AddScore);

	//��Ϸ����
	m_wPlayerCount = GAME_PLAYER;
	m_bOperaCount=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount=0;
	m_bGameEnd=false;
	ZeroMemory(m_wFlashUser,sizeof(m_wFlashUser));

	//�û�״̬
	ZeroMemory(&m_StGameEnd,sizeof(m_StGameEnd));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbGiveUpUser,sizeof(m_cbGiveUpUser));
	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_bMingZhu[i]=false;
		m_wCompardUser[i].RemoveAll();
	}
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));

	//�˿˱���
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//��ע��Ϣ
	m_lMaxCellScore=0L;
	m_lCellScore=0L;
	m_lCurrentTimes=1L;		
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));

	//�������
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	//�������
	g_ListRoomUserControl.RemoveAll();
	g_ListOperationRecord.RemoveAll();
	ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

	//�������
	m_bUpdataStorage=false;
	m_hControlInst = NULL;
	m_pServerControl = NULL;
	m_hControlInst = LoadLibrary(TEXT("ZaJinHuaServerControl.dll"));
	if ( m_hControlInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hControlInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerControl = static_cast<IServerControl*>(ServerControl());
		}
	}
	
	ZeroMemory(m_chUserAESKey, sizeof(m_chUserAESKey));

	//�������
	srand(time(NULL));

	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	swprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\ZJHConfig.ini"),szPath);

	m_nCurrentRounds = 0;

	m_dElapse = GetTickCount();

	m_MapControlID.InitHashTable(100L);
	m_bAndroidControl = false;
	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
	if( m_pServerControl )
	{
		delete m_pServerControl;
		m_pServerControl = NULL;
	}

	if( m_hControlInst )
	{
		FreeLibrary(m_hControlInst);
		m_hControlInst = NULL;
	}

	m_RoundsMap.clear();

	m_MapControlID.RemoveAll();
}

//�ͷŶ���
VOID CTableFrameSink::Release()
{
	if( m_pServerControl )
	{
		delete m_pServerControl;
		m_pServerControl = NULL;
	}

	if( m_hControlInst )
	{
		FreeLibrary(m_hControlInst);
		m_hControlInst = NULL;
	}

	delete this;
}

//�ӿڲ�ѯ--��������Ϣ�汾
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	//��ȡ����
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//��ȡ����
	ReadConfigInformation();

	bool bstatus = IsRoomCardScoreType();
	
	//m_cbHandCardData[0][0] = 0x17;
	//m_cbHandCardData[0][1] = 0x18;
	//m_cbHandCardData[0][2] = 0x19;

	//m_GameLogic.SortCardList(m_cbHandCardData[0], MAX_COUNT);

	//if (m_GameLogic.GetCardType(m_cbHandCardData[0], 3) >= CT_SHUN_ZI)
	//{
	//	int  nnn = 0;
	//}
	return true;
}

//��λ����
void  CTableFrameSink::RepositionSink()
{
	//��Ϸ����
	m_bOperaCount=0;
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount=0;
	ZeroMemory(m_wFlashUser,sizeof(m_wFlashUser));

	//�û�״̬
	ZeroMemory(&m_StGameEnd,sizeof(m_StGameEnd));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbGiveUpUser,sizeof(m_cbGiveUpUser));
	for(int i=0;i<m_wPlayerCount;i++)
	{
		m_bMingZhu[i]=false;	
		m_wCompardUser[i].RemoveAll();
	}
	ZeroMemory(m_cbRealPlayer, sizeof(m_cbRealPlayer));
	ZeroMemory(m_cbAndroidStatus, sizeof(m_cbAndroidStatus));

	//�˿˱���
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//��ע��Ϣ
	m_lMaxCellScore=0L;		
	m_lCellScore=0L;						
	m_lCurrentTimes=1L;		
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));
	m_bAndroidControl = false;
	return;
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//������ҷ���,д�ֱ仯
	if (IsRoomCardTreasureType() && cbReason == SCORE_REASON_WRITE && !pIServerUserItem->IsAndroidUser())
	{
		SCORE lRoomCardTurnMaxScore = 0LL;
		
		//�����ļ���
		TCHAR szPath[MAX_PATH] = TEXT("");
		TCHAR szFileName[MAX_PATH] = TEXT("");
		GetCurrentDirectory(sizeof(szPath),szPath);
		_sntprintf(szFileName,sizeof(szFileName),TEXT("%s\\ZaJinHuaRoomCard.ini"),szPath);

		//��ȡ����
		lRoomCardTurnMaxScore = GetPrivateProfileInt(m_pGameServiceOption->szServerName, TEXT("lTurnMaxScore"), 10000, szFileName);

		if (pIServerUserItem->GetUserScore() < lRoomCardTurnMaxScore)
		{
			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_RC_TREASEURE_DEFICIENCY);
		}
	}

	return false;
}

//��ʼģʽ
BYTE  CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//��Ϸ״̬
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID<m_wPlayerCount);
	return (m_cbPlayStatus[wChairID]==TRUE)?true:false;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_OFFLINE);

	return true;
}

//��Ϸ��ʼ
bool  CTableFrameSink::OnEventGameStart()
{

	//��ȡ����
	ReadConfigInformation();
	m_RoundsMap.clear();
	m_nCurrentRounds = 1;
	m_nTotalRounds = 0;

	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_RoundsMap[i] = 0;
	}


	//ɾ����ʱ��
	m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);

	//����״̬
	m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);

	//˽�˷���
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )
	{
		//cbGameRule[1] Ϊ  2 ��3 ��4 ��5, 0�ֱ��Ӧ 2�� �� 3�� �� 4�� �� 5�� �� 2-5�� �⼸������
		BYTE *pGameRule = m_pITableFrame->GetGameRule();
		if (pGameRule[1] != 0)
		{
			m_wPlayerCount = pGameRule[1];
			//��������
			m_pITableFrame->SetTableChairCount(m_wPlayerCount);
		}
		else
		{
			m_wPlayerCount = GAME_PLAYER;
			//��������
			m_pITableFrame->SetTableChairCount(GAME_PLAYER);
		}
	}

	//���·����û���Ϣ
	for (WORD i=0; i<m_wPlayerCount; i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL)
		{
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
		}
	}

	m_bGameEnd=false;

	//�����ע
	SCORE lTimes=6L;
	BYTE cbAiCount = 0;
	BYTE cbPlayCount = 0;
	
	//�ж�Լռ���ֳ�
	SCORE lRoomCardTurnMaxScore = 0;
	SCORE lRoomCardMaxCellScore = 0;
	if (IsRoomCardScoreType())
	{
		//�����ļ���
		TCHAR szPath[MAX_PATH] = TEXT("");
		TCHAR szFileName[MAX_PATH] = TEXT("");
		GetCurrentDirectory(sizeof(szPath),szPath);
		_sntprintf(szFileName,sizeof(szFileName),TEXT("%s\\ZaJinHuaRoomCard.ini"),szPath);

		//��ȡ����
		lRoomCardMaxCellScore = GetPrivateProfileInt(m_pGameServiceOption->szServerName, TEXT("lMaxCellScore"), 1000, szFileName);
		lRoomCardTurnMaxScore = GetPrivateProfileInt(m_pGameServiceOption->szServerName, TEXT("lTurnMaxScore"), 10000, szFileName);
	}

	if (IsRoomCardTreasureType())
	{
		//�����ļ���
		TCHAR szPath[MAX_PATH] = TEXT("");
		TCHAR szFileName[MAX_PATH] = TEXT("");
		GetCurrentDirectory(sizeof(szPath),szPath);
		_sntprintf(szFileName,sizeof(szFileName),TEXT("%s\\ZaJinHuaRoomCard.ini"),szPath);

		//��ȡ����
		lRoomCardTurnMaxScore = GetPrivateProfileInt(m_pGameServiceOption->szServerName, TEXT("lTurnMaxScore"), 10000, szFileName);
	}
	
	m_MapControlID.RemoveAll();
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//���ܸ�ֵ
		m_chUserAESKey[i][0] = (HIBYTE(HIWORD(pIServerUserItem->GetUserID())));
		m_chUserAESKey[i][1] = (LOBYTE(HIWORD(pIServerUserItem->GetUserID())));
		m_chUserAESKey[i][2] = (HIBYTE(LOWORD(pIServerUserItem->GetUserID())));
		m_chUserAESKey[i][3] = (LOBYTE(LOWORD(pIServerUserItem->GetUserID())));
		m_chUserAESKey[i][4] = rand() % 256;
		m_chUserAESKey[i][5] = rand() % 256;
		m_chUserAESKey[i][6] = rand() % 256;
		m_chUserAESKey[i][7] = rand() % 256;
		m_chUserAESKey[i][8] = rand() % 256;
		m_chUserAESKey[i][9] = rand() % 256;
		m_chUserAESKey[i][10] = rand() % 256;
		m_chUserAESKey[i][11] = rand() % 256;
		m_chUserAESKey[i][12] = rand() % 256;
		m_chUserAESKey[i][13] = rand() % 256;
		m_chUserAESKey[i][14] = rand() % 256;
		m_chUserAESKey[i][15] = rand() % 256;

		const SCORE lUserScore=pIServerUserItem->GetUserScore();

		//���ñ���
		m_cbPlayStatus[i]=TRUE;

		if (!pIServerUserItem->IsAndroidUser())
		{
			//�����Ҫ���Ƶ����
			if (pIServerUserItem->GetZhaJinHuaGameRate() < 101)
			{
				m_MapControlID[i] = pIServerUserItem->GetZhaJinHuaGameRate();
			}
		}

		if (IsRoomCardScoreType() && !IsRoomCardTreasureType())
		{
			m_lUserMaxScore[i] = min(lRoomCardTurnMaxScore, lUserScore);
		}
		else if (IsRoomCardTreasureType() && !IsRoomCardScoreType())
		{
			m_lUserMaxScore[i] = min(lRoomCardTurnMaxScore, lUserScore);
		}
		else
		{
			m_lUserMaxScore[i] = lUserScore;
		}

		//���·����û���Ϣ 
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

		cbPlayCount++;
		if (pIServerUserItem->IsAndroidUser() == true) 
		{
			cbAiCount++;
			m_cbAndroidStatus[i] = TRUE;
		}
		else
		{
			m_cbRealPlayer[i] = TRUE;
		}

		//�жϵ�ע
		SCORE Temp=lTimes;
		if(m_lUserMaxScore[i]<10001)Temp=1L;
		else if(m_lUserMaxScore[i]<100001)Temp=2L;
		else if(m_lUserMaxScore[i]<1000001)Temp=3L;
		else if(m_lUserMaxScore[i]<10000001)Temp=4L;
		else if(m_lUserMaxScore[i]<100000001)Temp=5L;
		if(lTimes>Temp)lTimes=Temp;
	}

	//���
	if(g_lRoomStorageCurrent>0 && NeedDeductStorage()) g_lRoomStorageCurrent = g_lRoomStorageCurrent-g_lRoomStorageCurrent*g_lStorageDeductRoom/1000;

	//���Ϳ����Ϣ
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if ( pIServerUserItem == NULL )
			continue;

		if( CUserRight::IsGameCheatUser(m_pITableFrame->GetTableUserItem(i)->GetUserRight()))
		{
			CString strInfo;
			strInfo.Format(TEXT("��ǰ��棺%I64d"), g_lRoomStorageCurrent);

			m_pITableFrame->SendGameMessage(pIServerUserItem,strInfo,SMT_CHAT);
		}	
	}

	//д������־

	CTime time = CTime::GetCurrentTime();
	CString strInfo;
	strInfo.Format(TEXT(" %d/%d/%d-%d:%d:%d-��ǰ�����棺%I64d\n"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(),
		g_lRoomStorageCurrent);

	CString strFileName;
	strFileName.Format(TEXT("թ��[%s]�����־.log"), m_pGameServiceOption->szServerName);
	//WriteInfo(strFileName, strInfo);

	//������
	m_bUpdataStorage = (cbAiCount != cbPlayCount);
	//��ע����
	//m_lCellScore = 1;
	m_lCellScore=m_pGameServiceOption->lCellScore;
	//while((lTimes--)>0) m_lCellScore*=10;

	//��������
	for(BYTE i=0;i<m_wPlayerCount;i++)
	{
		if(m_lUserMaxScore[i] != 0)
			m_lCellScore=__min(m_lUserMaxScore[i],m_lCellScore);
	}

	m_lCurrentTimes=1;
	m_lMaxCellScore=m_lCellScore*50;

	if (!IsRoomCardScoreType() && !IsRoomCardTreasureType())
	{
		//�����ע
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==TRUE)
			{
				m_lUserMaxScore[i] = __min(m_lUserMaxScore[i],m_lMaxCellScore*101);
				if(m_pGameServiceOption->lRestrictScore != 0)
					m_lUserMaxScore[i]=__min(m_lUserMaxScore[i],m_pGameServiceOption->lRestrictScore);
			}
		}
	}
	else if (IsRoomCardScoreType() && !IsRoomCardTreasureType())
	{
		m_lCellScore = 1;
		m_lMaxCellScore = lRoomCardMaxCellScore;
	}

	//�ַ��˿�
	//BYTE bTempArray[52];
	ZeroMemory(m_bTempArray,sizeof(m_bTempArray));

	//m_GameLogic.RandCardList(bTempArray,m_cbHandCardData[0],sizeof(m_cbHandCardData)/sizeof(m_cbHandCardData[0][0]));
	//m_GameLogic.RandCardList(m_bTempArray,m_cbHandCardData[0],52);
	m_GameLogic.RandCardList(m_bTempArray,sizeof(m_bTempArray));

	//�����˿�
	//for (WORD i= 0;i < GAME_PLAYER; i++)
	//{
	//	m_GameLogic.SortCardList(m_cbHandCardData[i], MAX_COUNT);
	//}

	//CopyMemory(m_bTempArray,m_cbHandCardData[0],sizeof(m_cbHandCardData)/sizeof(m_cbHandCardData[0][0]));

	WORD wIndex = 0;
	CMD_S_AllCard AllCard;
	ZeroMemory(&AllCard,sizeof(CMD_S_AllCard));
	for(WORD i=0; i<m_wPlayerCount; i++)
	{
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
		if(pIServerUser==NULL)continue;
		if(m_cbPlayStatus[i]==FALSE)continue;

		//�ɷ��˿�
		CopyMemory(m_cbHandCardData[i],&m_bTempArray[wIndex*MAX_COUNT],MAX_COUNT);
		m_GameLogic.SortCardList(m_cbHandCardData[i], MAX_COUNT);

		CopyMemory(AllCard.cbCardData[i],&m_bTempArray[wIndex*MAX_COUNT],MAX_COUNT);
		m_GameLogic.SortCardList(AllCard.cbCardData[i], MAX_COUNT);
		AllCard.bAICount[i] = pIServerUser->IsAndroidUser();
		wIndex++;
	}

	ZeroMemory(m_cbCardDataForShow,sizeof(m_cbCardDataForShow));

	//��Ϸ��ʼ
	AnalyseStartCard();

#ifdef _DEBUG
	m_cbHandCardData[0][0] = 0x33;
	m_cbHandCardData[0][1] = 0x23;
	m_cbHandCardData[0][2] = 0x13;
#endif
	//��������
	ROOMUSERCONTROL roomusercontrol;
	ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));
	POSITION posKeyList;

	bool bControl = false;
	for (int j = 0;j < GAME_PLAYER;j++)
	{
		IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
		if (pServerUser == NULL) continue;
		int nRate= pServerUser->GetZhaJinHuaGameRate();
		if(pServerUser->GetZhaJinHuaGameRate() <101)
		{
			bControl = true;
			//if(rand()%100 < pServerUser->GetGameRate())
			//{
			//	ProbeJudge(j,CHEAT_TYPE_WIN);  //ָ�����ʤ��
			//}
			//else
			//{
			//	DWORD dwWinChairID = ProbeJudge(j,CHEAT_TYPE_LOST);  //ָ�����ʧ��
			//	IServerUserItem *pServerAndriodUser = m_pITableFrame->GetTableUserItem(dwWinChairID);
			//	if(pServerAndriodUser != NULL && pServerAndriodUser->IsAndroidUser())
			//	{
			//		m_bAndroidControl = true;
			//	}
			//}
		}
	}
	BYTE cbControlRemainCrad[52]  ={ 0};
	if (bControl == true)
	{
		GetControlCard(cbControlRemainCrad,0);
	}

	//m_cbHandCardData[0][0] = 0x0b;
	//m_cbHandCardData[0][1] = 0x1b;
	//m_cbHandCardData[0][2] = 0x2b;
	//m_cbHandCardData[1][0] = 0x3c;
	//m_cbHandCardData[1][1] = 0x35;
	//m_cbHandCardData[1][2] = 0x33;

	//m_cbHandCardData[2][0] = 0x21;
	//m_cbHandCardData[2][1] = 0x0c;
	//m_cbHandCardData[2][2] = 0x0d;

	//m_cbHandCardData[3][0] = 0x27;
	//m_cbHandCardData[3][1] = 0x15;
	//m_cbHandCardData[3][2] = 0x05;
	//m_cbHandCardData[4][0] = 0x31;
	//m_cbHandCardData[4][1] = 0x32;
	//m_cbHandCardData[4][2] = 0x33;
	//m_cbHandCardData[5][0] = 0x34;
	//m_cbHandCardData[5][1] = 0x35;
	//m_cbHandCardData[5][2] = 0x36;
	//m_cbHandCardData[6][0] = 0x37;
	//m_cbHandCardData[6][1] = 0x38;
	//m_cbHandCardData[6][2] = 0x39;


	for(int i =0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
		if(pIServerUser==NULL)continue;
		if(m_cbPlayStatus[i]==FALSE)continue;
		m_GameLogic.SortCardList(m_cbHandCardData[i],MAX_COUNT);
		m_GameLogic.GetCardType(m_cbHandCardData[i],MAX_COUNT);
	}
	//��������Ϣ
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//����������
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem == NULL) continue;
		if (pServerUserItem->IsAndroidUser() == true)
		{
			//����������
			CMD_S_AndroidCard AndroidCard ;
			ZeroMemory(&AndroidCard, sizeof(AndroidCard)) ;

			//���ñ���
			AndroidCard.lStockScore = g_lRoomStorageCurrent;
			AndroidCard.bAndroidControl = m_bAndroidControl;
			CopyMemory(AndroidCard.cbRealPlayer, m_cbRealPlayer, sizeof(m_cbRealPlayer));
			CopyMemory(AndroidCard.cbAndroidStatus, m_cbAndroidStatus, sizeof(m_cbAndroidStatus));
			CopyMemory(AndroidCard.cbAllHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
			m_pITableFrame->SendTableData(i, SUB_S_ANDROID_CARD, &AndroidCard, sizeof(AndroidCard));
		}
	}


	//����ׯ��
	if(m_wBankerUser==INVALID_CHAIR) m_wBankerUser=rand()%m_wPlayerCount;

	//ׯ���뿪
	if(m_wBankerUser<m_wPlayerCount && m_cbPlayStatus[m_wBankerUser]==FALSE) m_wBankerUser=rand()%m_wPlayerCount;

	//ȷ��ׯ��
	while(m_cbPlayStatus[m_wBankerUser]==FALSE)
	{
		m_wBankerUser = (m_wBankerUser+1)%m_wPlayerCount;
	}

	//�û�����
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			m_lTableScore[i]=m_lCellScore;
		}
	}

	//��ǰ�û�
	m_wCurrentUser=(m_wBankerUser+1)%m_wPlayerCount;
	while(m_cbPlayStatus[m_wCurrentUser]==FALSE)
	{
		m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	}

	if (m_wCurrentUser==INVALID_CHAIR)
	{
		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("թ�𻨻�����IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnEventGameStart  m_wCurrentUser==INVALID_CHAIR [%d-%d-%d %d:%d:%d]\n"), m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}

	//��������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	GameStart.lMaxScore=m_lMaxCellScore;
	GameStart.lCellScore=m_lCellScore;
	GameStart.lCurrentTimes=m_lCurrentTimes;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.wBankerUser=m_wBankerUser;
	CopyMemory(GameStart.cbPlayStatus, m_cbPlayStatus, sizeof(BYTE)*GAME_PLAYER);

	GameStart.nCurrentRounds = 1;

	CopyMemory(GameStart.cbHandCardData, m_cbHandCardData, sizeof(BYTE)*GAME_PLAYER*MAX_COUNT);

	//liuyang
	m_RoundsMap[m_wBankerUser] = 1;


	//��������
	memcpy(m_szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));
	m_nTotalRounds = GetPrivateProfileInt(m_szRoomName, TEXT("Rounds"), 20, m_szConfigFileName);
	GameStart.nTotalRounds = m_nTotalRounds;

	m_dElapse = GetTickCount();

	//��������
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i] == TRUE)
		{
			GameStart.lUserMaxScore=m_lUserMaxScore[i];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	//�������
	if (m_pServerControl)
	{
		//������Ϣ
		BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT];
		CopyMemory(cbHandCardData, m_cbHandCardData, sizeof(cbHandCardData) );

		//�����˿�
		for (WORD i= 0;i < GAME_PLAYER; i++)
			m_GameLogic.SortCardList(cbHandCardData[i], MAX_COUNT);

		//��Ϣ����
		m_pServerControl->ServerControl(cbHandCardData, m_pITableFrame, m_wPlayerCount);
	}

	//���ö�ʱ�� �Ƿ�������
	if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) == 0)
	{
		m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE,1,0);
	}

	//���ط�������
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]!=true)continue;
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
		if(pIServerUser==NULL)continue;
		if (bControl == false)
		{
			CopyMemory(AllCard.cbSurplusCardData,&m_bTempArray[wIndex*MAX_COUNT],52-wIndex*MAX_COUNT);
		}
		else
		{	
			CopyMemory(AllCard.cbSurplusCardData,cbControlRemainCrad,52-wIndex*MAX_COUNT);
		}

		AllCard.cbSurplusCardCount = 52-wIndex*MAX_COUNT;
		if(CUserRight::IsGameCheatUser(pIServerUser->GetUserRight())==false)
		{
			continue;
		}
 		AllCard.bAICount[i] = true;
		m_pITableFrame->SendTableData(i,SUB_S_ALL_CARD,&AllCard,sizeof(AllCard));
	}
	
	return true;
}
//��̽���ж�, ȷ��Chair���û�����Ӯ
bool CTableFrameSink::GetControlCard(BYTE cbControlCardByte[],BYTE cbCardCount)
{
	BYTE cbCardListData[52]=
	{
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//���� A - K
	};

	int nControlChair[GAME_PLAYER] = {-1};
	int nAndroidCount[GAME_PLAYER] = {-1};
	//��������
	BYTE cbControlCount = 0;
	BYTE cbAndroidCount = 0;
	bool bControlWin = false;
	bool bControlLose = false;
	for (int j = 0;j < GAME_PLAYER;j++)
	{
		IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
		if (pServerUser == NULL) continue;
		if(pServerUser->GetZhaJinHuaGameRate() <101)
		{
			nControlChair[cbControlCount] = j;
			cbControlCount++;
		}
		if (pServerUser->IsAndroidUser())
		{
			nAndroidCount[cbAndroidCount] = j;
			cbAndroidCount++;
		}
		if(pServerUser->GetZhaJinHuaGameRate() == 0)
		{
			bControlLose = true;
		}
		if(pServerUser->GetZhaJinHuaGameRate() == 100)
		{
			bControlWin = true;
		}
	}
	if (cbControlCount >= 2 && cbAndroidCount == 0)
	{
		int cbIndex = 0;
		for (int j = 0;j < GAME_PLAYER;j++)
		{
			IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
			if (pServerUser == NULL) continue;
			GetRandCard(cbCardListData,j,cbIndex,0,m_nSanPai,FALSE);
		}
	}
	else if (cbControlCount >= 2 && cbAndroidCount > 0)
	{
		int cbIndex = 0;
		BYTE cbWinAndroidChairID = nAndroidCount[rand()%cbAndroidCount];
		BYTE cbNextUserChairID = nControlChair[rand()%cbControlCount];
		GetRandCard(cbCardListData,cbWinAndroidChairID,cbIndex,0,m_nShunZI,true);

		do 
		{
			BYTE cbCard[3] = {0};
			m_GameLogic.RandCardList2(cbCard,3,cbCardListData,52 - cbIndex*3);
			m_GameLogic.SortCardList(cbCard, MAX_COUNT);
			if (m_GameLogic.CompareCard(cbCard,m_cbHandCardData[cbWinAndroidChairID],3) == 0 && m_GameLogic.GetCardType(cbCard, 3) >= CT_SHUN_ZI&& m_GameLogic.GetCardType(cbCard, 3) != CT_SPECIAL )
			{
				//ȫ����ע
				CopyMemory(m_cbHandCardData[cbNextUserChairID],cbCard,sizeof(m_cbHandCardData[cbNextUserChairID]));
				m_GameLogic.RemoveGoodCardData(cbCard,3,cbCardListData,52);
				cbIndex++;
				break;
			}
		}while(true);
		for (int j = 0;j < GAME_PLAYER;j++)
		{
			if (j == cbWinAndroidChairID || j == cbNextUserChairID)continue;
			IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
			if (pServerUser == NULL) continue;
			do 
			{
				BYTE cbCard[3] = {0};
				m_GameLogic.RandCardList2(cbCard,3,cbCardListData,52 - cbIndex*3);
				m_GameLogic.SortCardList(cbCard, MAX_COUNT);
				if (m_GameLogic.CompareCard(cbCard,m_cbHandCardData[cbNextUserChairID],3) == 0 && m_GameLogic.GetCardType(cbCard, 3) != CT_SPECIAL )
				{
					//ȫ����ע
					CopyMemory(m_cbHandCardData[j],cbCard,sizeof(m_cbHandCardData[j]));
					m_GameLogic.RemoveGoodCardData(cbCard,3,cbCardListData,52);
					cbIndex++;
					break;
				}
			}while(true);
		}
	}
	else if (cbControlCount ==1 && cbAndroidCount > 0 && bControlWin == false)
	{
		int cbIndex = 0;
		BYTE cbWinAndroidChairID = nAndroidCount[rand()%cbAndroidCount];
		BYTE cbNextUserChairID = nControlChair[rand()%cbControlCount];
		GetRandCard(cbCardListData,cbWinAndroidChairID,cbIndex,0,m_nShunZI,true);

		do 
		{
			BYTE cbCard[3] = {0};
			m_GameLogic.RandCardList2(cbCard,3,cbCardListData,52 - cbIndex*3);
			m_GameLogic.SortCardList(cbCard, MAX_COUNT);
			if (m_GameLogic.CompareCard(cbCard,m_cbHandCardData[cbWinAndroidChairID],3) == 0 && m_GameLogic.GetCardType(cbCard, 3) >= CT_SHUN_ZI&& m_GameLogic.GetCardType(cbCard, 3) != CT_SPECIAL )
			{
				//ȫ����ע
				CopyMemory(m_cbHandCardData[cbNextUserChairID],cbCard,sizeof(m_cbHandCardData[cbNextUserChairID]));
				m_GameLogic.RemoveGoodCardData(cbCard,3,cbCardListData,52);
				cbIndex++;
				break;
			}
		}while(true);
		for (int j = 0;j < GAME_PLAYER;j++)
		{
			if (j == cbWinAndroidChairID || j == cbNextUserChairID)continue;
			IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
			if (pServerUser == NULL) continue;
			do 
			{
				BYTE cbCard[3] = {0};
				m_GameLogic.RandCardList2(cbCard,3,cbCardListData,52 - cbIndex*3);
				m_GameLogic.SortCardList(cbCard, MAX_COUNT);
				if (m_GameLogic.CompareCard(cbCard,m_cbHandCardData[cbNextUserChairID],3) == 0&& m_GameLogic.GetCardType(cbCard, 3) != CT_SPECIAL )
				{
					//ȫ����ע
					CopyMemory(m_cbHandCardData[j],cbCard,sizeof(m_cbHandCardData[j]));
					m_GameLogic.RemoveGoodCardData(cbCard,3,cbCardListData,52);
					cbIndex++;
					break;
				}
			}while(true);
		}
	}
	else if (cbControlCount ==1 && cbAndroidCount > 0 && bControlWin == true)
	{
		int cbIndex = 0;
		BYTE cbWinAndroidChairID = nAndroidCount[rand()%cbAndroidCount];
		BYTE cbNextUserChairID = nControlChair[rand()%cbControlCount];
		GetRandCard(cbCardListData,cbNextUserChairID,cbIndex,0,m_nShunZI,true);


		do 
		{
			BYTE cbCard[3] = {0};
			m_GameLogic.RandCardList2(cbCard,3,cbCardListData,52 - cbIndex*3);
			m_GameLogic.SortCardList(cbCard, MAX_COUNT);
			if (m_GameLogic.CompareCard(cbCard,m_cbHandCardData[cbNextUserChairID],3) == 0 && m_GameLogic.GetCardType(cbCard, 3) >= CT_SHUN_ZI&& m_GameLogic.GetCardType(cbCard, 3) != CT_SPECIAL )
			{
				//ȫ����ע
				CopyMemory(m_cbHandCardData[cbWinAndroidChairID],cbCard,sizeof(m_cbHandCardData[cbWinAndroidChairID]));
				m_GameLogic.RemoveGoodCardData(cbCard,3,cbCardListData,52);
				cbIndex++;
				break;
			}
		}while(true);
		for (int j = 0;j < GAME_PLAYER;j++)
		{
			if (j == cbWinAndroidChairID || j == cbNextUserChairID)continue;
			IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
			if (pServerUser == NULL) continue;
			do 
			{
				BYTE cbCard[3] = {0};
				m_GameLogic.RandCardList2(cbCard,3,cbCardListData,52 - cbIndex*3);
				m_GameLogic.SortCardList(cbCard, MAX_COUNT);
				if (m_GameLogic.CompareCard(cbCard,m_cbHandCardData[cbNextUserChairID],3) == 0)
				{
					//ȫ����ע
					CopyMemory(m_cbHandCardData[j],cbCard,sizeof(m_cbHandCardData[j]));
					m_GameLogic.RemoveGoodCardData(cbCard,3,cbCardListData,52);
					cbIndex++;
					break;
				}
			}while(true);
		}
	}
	else if (cbControlCount ==1 && cbAndroidCount == 0 && bControlWin == true)
	{
		int cbIndex = 0;
		BYTE cbFistUserChairID = nControlChair[rand()%cbControlCount];
		GetRandCard(cbCardListData,cbFistUserChairID,cbIndex,0,m_nShunZI,true);


		BYTE cbUserConut = 0;
		int  cbUserChairID[GAME_PLAYER] = {-1};
		for (int j = 0;j < GAME_PLAYER;j++)
		{
			IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
			if (pServerUser == NULL) continue;
			if(pServerUser->GetZhaJinHuaGameRate() >= 101 && pServerUser->IsAndroidUser() == false)
			{
				cbUserChairID[cbUserConut] = j;
				cbUserConut++;
			}
		}
		BYTE cbNextserChairID = cbUserChairID[rand()%cbUserConut];
		do 
		{
			BYTE cbCard[3] = {0};
			m_GameLogic.RandCardList2(cbCard,3,cbCardListData,52 - cbIndex*3);
			m_GameLogic.SortCardList(cbCard, MAX_COUNT);
			if (m_GameLogic.CompareCard(cbCard,m_cbHandCardData[cbFistUserChairID],3) == 0 && m_GameLogic.GetCardType(cbCard, 3) >= CT_SHUN_ZI)
			{
				//ȫ����ע
				CopyMemory(m_cbHandCardData[cbNextserChairID],cbCard,sizeof(m_cbHandCardData[cbNextserChairID]));
				m_GameLogic.RemoveGoodCardData(cbCard,3,cbCardListData,52);
				cbIndex++;
				break;
			}
		}while(true);
		for (int j = 0;j < GAME_PLAYER;j++)
		{
			if (j == cbFistUserChairID || j == cbNextserChairID)continue;
			IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
			if (pServerUser == NULL) continue;
			do 
			{
				BYTE cbCard[3] = {0};
				m_GameLogic.RandCardList2(cbCard,3,cbCardListData,52 - cbIndex*3);
				m_GameLogic.SortCardList(cbCard, MAX_COUNT);
				if (m_GameLogic.CompareCard(cbCard,m_cbHandCardData[cbNextserChairID],3) == 0)
				{
					//ȫ����ע
					CopyMemory(m_cbHandCardData[j],cbCard,sizeof(m_cbHandCardData[j]));
					m_GameLogic.RemoveGoodCardData(cbCard,3,cbCardListData,52);
					cbIndex++;
					break;
				}
			}while(true);
		}
	}
	else if (cbControlCount ==1 && cbAndroidCount == 0 && bControlWin == false)
	{
		int cbIndex = 0;
		BYTE cbUserConut = 0;
		int  cbUserChairID[GAME_PLAYER] = {-1};
		for (int j = 0;j < GAME_PLAYER;j++)
		{
			IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
			if (pServerUser == NULL) continue;
			if(pServerUser->GetZhaJinHuaGameRate() >= 101 && pServerUser->IsAndroidUser() == false)
			{
				cbUserChairID[cbUserConut] = j;
				cbUserConut++;
			}
		}

		BYTE cbFistUserChairID = cbUserChairID[rand()%cbUserConut];
		GetRandCard(cbCardListData,cbFistUserChairID,cbIndex,0,m_nShunZI,true);

		BYTE cbNextserChairID = nControlChair[rand()%cbControlCount];

		do 
		{
			BYTE cbCard[3] = {0};
			m_GameLogic.RandCardList2(cbCard,3,cbCardListData,52 - cbIndex*3);
			m_GameLogic.SortCardList(cbCard, MAX_COUNT);
			if (m_GameLogic.CompareCard(cbCard,m_cbHandCardData[cbFistUserChairID],3) == 0 && m_GameLogic.GetCardType(cbCard, 3) >= CT_SHUN_ZI)
			{
				//ȫ����ע
				CopyMemory(m_cbHandCardData[cbNextserChairID],cbCard,sizeof(m_cbHandCardData[cbNextserChairID]));
				m_GameLogic.RemoveGoodCardData(cbCard,3,cbCardListData,52);
				cbIndex++;
				break;
			}
		}while(true);
		for (int j = 0;j < GAME_PLAYER;j++)
		{
			if (j == cbFistUserChairID || j == cbNextserChairID)continue;
			IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
			if (pServerUser == NULL) continue;
			do 
			{
				BYTE cbCard[3] = {0};
				m_GameLogic.RandCardList2(cbCard,3,cbCardListData,52 - cbIndex*3);
				m_GameLogic.SortCardList(cbCard, MAX_COUNT);
				if (m_GameLogic.CompareCard(cbCard,m_cbHandCardData[cbNextserChairID],3) == 0)
				{
					//ȫ����ע
					CopyMemory(m_cbHandCardData[j],cbCard,sizeof(m_cbHandCardData[j]));
					m_GameLogic.RemoveGoodCardData(cbCard,3,cbCardListData,52);
					cbIndex++;
					break;
				}
			}while(true);
		}
	}
	CopyMemory(cbControlCardByte,cbCardListData,52);
	return true;
}
//�����
BYTE CTableFrameSink::GetRandCard(BYTE cbRandCard[],DWORD dwChairID,int &cbIndex,int nRandMin,int nRandMax,bool bFistGetCrad)
{
	int nRand = rand()%(nRandMax-nRandMin) + nRandMin;
	BYTE cbCardTybe = 0;
	if(nRand < m_nZhaDan)
	{
		cbCardTybe = CT_BAO_ZI;
	}
	else if(m_nZhaDan <= nRand &&  nRand < m_nShunJin)
	{
		cbCardTybe = CT_SHUN_JIN;
	}
	else if(m_nShunJin <= nRand && nRand < m_nJinHua)
	{
		cbCardTybe = CT_JIN_HUA;
	}
	else if(m_nJinHua <= nRand && nRand< m_nShunZI)
	{
		cbCardTybe = CT_SHUN_ZI;
	}
	else if(m_nShunZI <= nRand && nRand< m_nDuiZi)
	{
		cbCardTybe = CT_DOUBLE;
	}
	else if(m_nDuiZi <= nRand && nRand < m_nSanPai)
	{
		cbCardTybe = CT_SINGLE;
	}
	do 
	{
		BYTE cbCard[3] = {0};
		m_GameLogic.RandCardList2(cbCard,3,cbRandCard,52 - cbIndex*3);
		m_GameLogic.SortCardList(cbCard, MAX_COUNT);
		if (bFistGetCrad)
		{
			if (m_GameLogic.GetCardType(cbCard, 3) == CT_SHUN_ZI || m_GameLogic.GetCardType(cbCard, 3) == CT_SHUN_JIN)
			{
				bool bOne=false,bTwo=false,bThree=false;
				for(BYTE i=0;i<MAX_COUNT;i++)
				{
					if(m_GameLogic.GetCardValue(cbCard[i])==1)		bOne=true;
					else if(m_GameLogic.GetCardValue(cbCard[i])==2)	bTwo=true;
					else if(m_GameLogic.GetCardValue(cbCard[i])==3)	bThree=true;
				}
				if(bOne && bTwo && bThree)continue;;
			}
			else if (m_GameLogic.GetCardType(cbCard, 3) == CT_JIN_HUA )
			{
				bool bOne=false,bTwo=false,bThree=false;
				for(BYTE i=0;i<MAX_COUNT;i++)
				{
					if(m_GameLogic.GetCardValue(cbCard[i])==2)		bOne=true;
					else if(m_GameLogic.GetCardValue(cbCard[i])==3)	bTwo=true;
					else if(m_GameLogic.GetCardValue(cbCard[i])==5)	bThree=true;
				}
				if(bOne && bTwo && bThree)continue;;
			}
			else if (m_GameLogic.GetCardType(cbCard, 3) == CT_BAO_ZI )
			{
				bool bOne=false,bTwo=false,bThree=false;
				for(BYTE i=0;i<MAX_COUNT;i++)
				{
					if(m_GameLogic.GetCardValue(cbCard[i])==2)		bOne=true;
					else if(m_GameLogic.GetCardValue(cbCard[i])==2)	bTwo=true;
					else if(m_GameLogic.GetCardValue(cbCard[i])==2)	bThree=true;
				}
				if(bOne && bTwo && bThree)continue;;
			}
		}
		bool bErrorCard = false; 
		for (int i =0;i<3;i++)
		{
			if (cbCard[i] <= 0 || cbCard[i] >= 62)
			{
				bErrorCard = true;
				break;
			}
		}
		if (bErrorCard == true)continue;
		if (m_GameLogic.GetCardType(cbCard, 3) == cbCardTybe)
		{
			//ȫ����ע
			CopyMemory(m_cbHandCardData[dwChairID],cbCard,sizeof(m_cbHandCardData[dwChairID]));
			m_GameLogic.RemoveGoodCardData(cbCard,3,cbRandCard,52);
			cbIndex++;
			break;
		}
	}while(true);

	return cbCardTybe;
}
bool CTableFrameSink::getSingBomb(BYTE cbCardData[52],int chairID,BYTE cbSingBomb[] )  //һ��ը�� 
{
	bool bOK_2 = true;
	BYTE GoodCardData[3] = {0};
	do 
	{
		int d = rand()%13 + 1;

		for (int i = 0;i<3;i++)
		{
			GoodCardData[i] = d +(i)*16;
		}
		bool bOK[3] = {false};
		for (int i = 0;i<3;i++)
		{
			for (int j = 0;j<52;j++)
			{
				if (GoodCardData[i] == cbCardData[j])
				{
					bOK[i] = !bOK[i];
				}
			}
		}
		for (int i = 0;i<3;i++)
		{
			if (bOK[i] == false)
			{
				bOK_2 = false;
				break;
			}
		}

	} while (bOK_2 == false);


	CopyMemory(cbSingBomb , GoodCardData,3);
	//ɾ����������
	m_GameLogic.RemoveGoodCardData(GoodCardData,3,cbCardData, 52 );

	return true;
}
bool CTableFrameSink::getSingSameStraight(BYTE cbCardData[52],int chairID,BYTE SingSameStraight[])  //һ��ͬ��˳
{

	BYTE GoodCardData[3];
	bool bOK_2 = true;
	do 
	{
		int d = rand()%4;
		int b = rand()%10 + 1;
		int xxx = 0;
		for (int i = 0;i<3;i++)
		{
			xxx = b + i;
			if (xxx == 14)
			{
				xxx = 1;
			}
			GoodCardData[i] = xxx +(d)*16;
		}

		bool bOK[3] = {false};
		for (int i = 0;i<3;i++)
		{
			for (int j = 0;j<52;j++)
			{
				if (GoodCardData[i] == cbCardData[j])
				{
					bOK[i] = !bOK[i];
				}
			}
		}
		for (int i = 0;i<3;i++)
		{
			if (bOK[i] == false)
			{
				bOK_2 = false;
				break;
			}
		}
	}while(bOK_2 == false);
	

	CopyMemory(SingSameStraight , GoodCardData,3);
	//ɾ����������
	m_GameLogic.RemoveGoodCardData(GoodCardData,3,cbCardData, 52 );

	return true;
}
//��̽���ж�, ȷ��Chair���û�����Ӯ
DWORD CTableFrameSink::ProbeJudge(WORD wChairIDCtrl,BYTE cbCheatType)
{	
	//��ȡ�û�
	IServerUserItem * pIServerCtrlUserItem=m_pITableFrame->GetTableUserItem(wChairIDCtrl);
	//���û�������, ����Ҫ����

	if(pIServerCtrlUserItem==NULL) return INVALID_CHAIR;

	//�˿˱���
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));
	//�����˿�
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i],MAX_COUNT);
	}

	BOOL bUserOxData[GAME_PLAYER];
	ZeroMemory(bUserOxData,sizeof(bUserOxData));
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==FALSE)continue;
		bUserOxData[i] = (m_GameLogic.GetCardType(cbUserCardData[i],MAX_COUNT)>0)?TRUE:FALSE;
	}

	IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(m_wBankerUser);
	//WORD wChairID=0;
	int iMaxIndex=0;
	int iLostIndex=0;

	for (int i = 0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==FALSE)continue;
		iMaxIndex=i;
		iLostIndex=i;
		break;
	}
	for (int i = 0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==FALSE ||   iMaxIndex ==i )continue;
		if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[iMaxIndex], MAX_COUNT) >= TRUE)
		{
			iMaxIndex = i;
		}
	}
	for (int i = 0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==FALSE ||   iLostIndex ==i )continue;
		if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[iLostIndex], MAX_COUNT) < TRUE)
		{
			iLostIndex = i;
		}
	}
	//iMaxIndex = EstimateWinner(0,4); //��ȡ����û�
	//iLostIndex = EstimateLose(0,4);	//��ȡ��С�û�

	//��ʱ����
	BYTE cbTemp[MAX_COUNT]={0};
	//������
	WORD wWinerUser=iMaxIndex;     //Ӯ

	if(cbCheatType==CHEAT_TYPE_LOST) wWinerUser=iLostIndex;   //��

	if (wWinerUser!=wChairIDCtrl) //�����ǰ�û����Ʋ���Ҫ�����Ƶ��û��Ϳ�ʼ����
	{
		//�����˿�
		CopyMemory(cbTemp,m_cbHandCardData[wWinerUser],MAX_COUNT);   //��ָ���û����Ʒŵ�����
		CopyMemory(m_cbHandCardData[wWinerUser],m_cbHandCardData[wChairIDCtrl],MAX_COUNT); //�ѿ����û�������ָ���û����ƽ���
		CopyMemory(m_cbHandCardData[wChairIDCtrl],cbTemp,MAX_COUNT);  //��󽻻�
	}
	return EstimateWinner();
}
//�ƶ�ʤ��
WORD CTableFrameSink::EstimateWinner()
{
	//�����˿�
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(cbUserCardData));

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
//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_COMPARECARD:	//���ƽ���
	case GER_NO_PLAYER:		//û�����
		{
			if(m_bGameEnd)return true;
			m_bGameEnd=true;

			//�������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			for (WORD i=0;i<m_wPlayerCount;i++)
				for (WORD j=0;j<(GAME_PLAYER - 1);j++)
					GameEnd.wCompareUser[i][j]=INVALID_CHAIR;

			//Ψһ���
			WORD wWinner,wUserCount=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{	
				if(m_cbPlayStatus[i]==TRUE)
				{
					wUserCount++;
					wWinner=i;
					if(GER_COMPARECARD==cbReason)ASSERT(m_wBankerUser==i);
					m_wBankerUser=i;
				}
			}

			//ʤ����ǿ��
			if (wUserCount==0)
			{
				wWinner=m_wBankerUser;
			}

			//�ӳٿ�ʼ
			bool bDelayOverGame = false;

			//������ע
			SCORE lWinnerScore=0L;
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				if(i==wWinner)continue;
				GameEnd.lGameScore[i]=-m_lTableScore[i];
				lWinnerScore+=m_lTableScore[i];
			}

			//����˰��
			GameEnd.lGameTax=m_pITableFrame->CalculateRevenue(wWinner,lWinnerScore);
			GameEnd.lGameScore[wWinner]=lWinnerScore-GameEnd.lGameTax;

			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			CopyMemory(m_cbCardDataForShow,m_cbHandCardData,sizeof(m_cbCardDataForShow));

			//�˿�����
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				WORD wCount=0;
				while(m_wCompardUser[i].GetCount()>0)
				{
					GameEnd.wCompareUser[i][wCount++] = m_wCompardUser[i].GetAt(0);
					m_wCompardUser[i].RemoveAt(0);
				}
			}

			GameEnd.wEndState=0;

			//ʤ����ǿ��
			if (wUserCount==1)
			{
				//�޸Ļ���
				tagScoreInfo ScoreInfo;
				
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

					if(m_cbPlayStatus[i]==FALSE)continue;
					ScoreInfo.cbType =(GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
					ScoreInfo.lRevenue=GameEnd.lGameTax;                                  
					ScoreInfo.lScore=GameEnd.lGameScore[i];

					if (ScoreInfo.cbType == SCORE_TYPE_LOSE && bDelayOverGame == false)
					{
						IServerUserItem * pUserItem = m_pITableFrame->GetTableUserItem(i);
						if (pUserItem!=NULL && (pUserItem->GetUserScore()+GameEnd.lGameScore[i])<m_pGameServiceOption->lMinTableScore)
						{
							bDelayOverGame = true;
						}
					}

					m_pITableFrame->WriteUserScore(i, ScoreInfo);
				}

				
			}

			//���ͳ��
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserIte==NULL) continue;
				if(!m_cbPlayStatus[i]) continue;

				//����ۼ�
				if(!pIServerUserIte->IsAndroidUser())
					UpdateStorage(-GameEnd.lGameScore[i]);//g_lRoomStorageCurrent -= GameEnd.lGameScore[i];

			}
			//��������
			GameEnd.wServerType = m_pGameServiceOption->wServerType;
			//�ӳٿ�ʼ
			if(bDelayOverGame)
			{
				GameEnd.bDelayOverGame = true;
			}

			if(wChairID==GAME_PLAYER)
			{
				//��������
				CopyMemory(&m_StGameEnd,&GameEnd,sizeof(m_StGameEnd));
				m_pITableFrame->SetGameTimer(IDI_GAME_COMPAREEND,TIME_GAME_COMPAREEND,1,0);
			}
			else
			{
				//������Ϣ
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				printf("������Ϸ������Ϣ11111\n");
				//�������˴洢��
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//��ȡ�û�
					IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
					if(!pIServerUserItem)
						continue;
					if(!pIServerUserItem->IsAndroidUser())
						continue;
					m_pITableFrame->SendTableData(i,SUB_S_ANDROID_BANKOPERATOR);
				}

				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}

			//���·����û���Ϣ
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//��ȡ�û�
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (!pIServerUserItem)
				{
					continue;
				}

				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}

			//���Ϳ��
			CString strInfo;
			strInfo.Format(TEXT("��ǰ��棺%I64d"), g_lRoomStorageCurrent);
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (!pIServerUserItem)
				{
					continue;
				}
				if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
				{
					m_pITableFrame->SendGameMessage(pIServerUserItem, strInfo, SMT_CHAT);

					CMD_S_ADMIN_STORAGE_INFO StorageInfo;
					ZeroMemory(&StorageInfo, sizeof(StorageInfo));
					StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
					StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
					StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
					StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
					StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
					StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
					StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
					m_pITableFrame->SendTableData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
					m_pITableFrame->SendLookonData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				}
			}

			m_RoundsMap.clear();

			return true;
		}
	case GER_USER_LEAVE:		//�û�ǿ��
	case GER_NETWORK_ERROR:		//�����ж�
		{
			if(m_bGameEnd || m_cbPlayStatus[wChairID]==FALSE) return true;

			//Ч�����
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//CString str;
			//str.Format(TEXT("��%d���� %d�˳�����"), m_pITableFrame->GetTableID() + 1, m_wCurrentUser);
			//NcaTextOut(str, m_pGameServiceOption->szServerName);

			//ǿ�˴���
			return OnUserGiveUp(wChairID,true);
		}
	case GER_OPENCARD:		//���ƽ���   
		{
			if(m_bGameEnd)return true;
			m_bGameEnd = true;

			//�������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			for (WORD i=0;i<m_wPlayerCount;i++)
				for (WORD j=0;j<(GAME_PLAYER - 1);j++)
					GameEnd.wCompareUser[i][j]=INVALID_CHAIR;

			//ʤ�����
			WORD wWinner=m_wBankerUser;

			//�������
			SCORE lWinnerScore=0L;
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				if(i==wWinner)continue;
				lWinnerScore+=m_lTableScore[i];
				GameEnd.lGameScore[i]=-m_lTableScore[i];
			}

			//����˰��
			GameEnd.lGameTax=m_pITableFrame->CalculateRevenue(wWinner,lWinnerScore);
			GameEnd.lGameScore[wWinner]=lWinnerScore-GameEnd.lGameTax;

			//���ƽ���
			GameEnd.wEndState=1;
			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			CopyMemory(m_cbCardDataForShow,m_cbHandCardData,sizeof(m_cbCardDataForShow));

			//�˿�����
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				WORD wCount=0;
				while(m_wCompardUser[i].GetCount()>0)
				{
					GameEnd.wCompareUser[i][wCount++] = m_wCompardUser[i].GetAt(0);
					m_wCompardUser[i].RemoveAt(0);
				}
			}

			//�޸Ļ���
			tagScoreInfo ScoreInfo;
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
			for (WORD i=0; i<m_wPlayerCount; i++) 
			{			
				ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
				if(i==wWinner || m_cbPlayStatus[i]==FALSE)continue;
				ScoreInfo.lScore=GameEnd.lGameScore[i];
				ScoreInfo.cbType=SCORE_TYPE_LOSE;

				m_pITableFrame->WriteUserScore(i, ScoreInfo);
			}

			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
			ScoreInfo.lScore=GameEnd.lGameScore[wWinner];
			ScoreInfo.cbType=SCORE_TYPE_WIN;
			ScoreInfo.lRevenue=GameEnd.lGameTax;
	
			m_pITableFrame->WriteUserScore(wWinner, ScoreInfo);

			//���ͳ��
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserIte==NULL) continue;
				if(!m_cbPlayStatus[i]) continue;

				//����ۼ�
				if(!pIServerUserIte->IsAndroidUser())
					UpdateStorage(-GameEnd.lGameScore[i]);//g_lRoomStorageCurrent -= GameEnd.lGameScore[i];

			}

			//��������
			GameEnd.wServerType = m_pGameServiceOption->wServerType;

			if(wChairID==GAME_PLAYER)
			{
				//��������
				CopyMemory(&m_StGameEnd,&GameEnd,sizeof(m_StGameEnd));
				m_pITableFrame->SetGameTimer(IDI_GAME_OPENEND,TIME_GAME_OPENEND,1,0);
			}
			else
			{
				//������Ϣ
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				printf("������Ϸ������Ϣ222222\n");
				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}
			
			//���·����û���Ϣ
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//��ȡ�û�
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (!pIServerUserItem)
				{
					continue;
				}

				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}

			//���Ϳ��
			CString strInfo;
			strInfo.Format(TEXT("��ǰ��棺%I64d"), g_lRoomStorageCurrent);
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (!pIServerUserItem)
				{
					continue;
				}
				if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
				{
					m_pITableFrame->SendGameMessage(pIServerUserItem, strInfo, SMT_CHAT);

					CMD_S_ADMIN_STORAGE_INFO StorageInfo;
					ZeroMemory(&StorageInfo, sizeof(StorageInfo));
					StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
					StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
					StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
					StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
					StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
					StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
					StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
					m_pITableFrame->SendTableData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
					m_pITableFrame->SendLookonData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				}
			}

			return true;
		}
	case GER_DISMISS:		//��Ϸ��ɢ
		{
			//��������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			printf("������Ϸ������Ϣ444444\n");
			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//���·����û���Ϣ
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//��ȡ�û�
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (!pIServerUserItem)
				{
					continue;
				}

				UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
			}

			return true;
		}
	}

	return false;
}

//���ͳ���
bool  CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//���ñ���
			StatusFree.lCellScore=0L;
			//StatusFree.lRoomStorageStart = g_lRoomStorageStart;
			//StatusFree.lRoomStorageCurrent = g_lRoomStorageCurrent;

			//��ȡ�Զ�������
			tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
			ASSERT(pCustomRule);
			tagCustomAndroid CustomAndroid;
			ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
			CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
			CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
			CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
			CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
			CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
			CopyMemory(&StatusFree.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
			
			if (pIServerUserItem->GetUserStatus() != US_LOOKON)
			{
				CopyMemory(StatusFree.chUserAESKey, m_chUserAESKey[wChairID], sizeof(m_chUserAESKey[wChairID]));
			}

			CopyMemory(StatusFree.szServerName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szServerName));

			//Ȩ���ж�
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));
				StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
				StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
				StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
				StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
				StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
				StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
				StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;

				m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_STATUS_PLAY:	//��Ϸ״̬
		{
			//��������
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//��ע��Ϣ
			StatusPlay.lMaxCellScore=m_lMaxCellScore;
			StatusPlay.lCellScore=m_lCellScore;
			StatusPlay.lCurrentTimes=m_lCurrentTimes;
			StatusPlay.lUserMaxScore=m_lUserMaxScore[wChairID];

			//���ñ���
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			CopyMemory(StatusPlay.bMingZhu,m_bMingZhu,sizeof(m_bMingZhu));
			CopyMemory(StatusPlay.lTableScore,m_lTableScore,sizeof(m_lTableScore));
			CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));
			//StatusPlay.lRoomStorageStart = g_lRoomStorageStart;
			//StatusPlay.lRoomStorageCurrent = g_lRoomStorageCurrent;

			//��ȡ�Զ�������
			tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
			ASSERT(pCustomRule);
			tagCustomAndroid CustomAndroid;
			ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
			CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
			CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
			CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
			CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
			CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
			CopyMemory(&StatusPlay.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));

			//��ǰ״̬
			StatusPlay.bCompareState=(m_lCompareCount<=0)?false:true;

			//�����˿�
			if(m_bMingZhu[wChairID])
				CopyMemory(&StatusPlay.cbHandCardData,&m_cbHandCardData[wChairID],MAX_COUNT);
			
			//���·����û���Ϣ
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);
			
			if (pIServerUserItem->GetUserStatus() != US_LOOKON)
			{
				CopyMemory(StatusPlay.chUserAESKey, m_chUserAESKey[wChairID], sizeof(m_chUserAESKey[wChairID]));
			}
			
			CopyMemory(StatusPlay.szServerName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szServerName));

			//�ڼ��� ������
            StatusPlay.nCurrentRounds = m_nCurrentRounds;
			StatusPlay.nTotalRounds = m_nTotalRounds;

			StatusPlay.dElapse = TIME_ADD_SCORE - (GetTickCount() - m_dElapse);
			if (TIME_ADD_SCORE - (GetTickCount() - m_dElapse) <= 0 || (GetTickCount() - m_dElapse <= 0))
			{
				StatusPlay.dElapse = 1;
			}
			//m_dElapse = GetTickCount();


			//Ȩ���ж�
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));
				StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
				StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
				StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
				StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
				StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
				StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
				StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
				m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	//Ч�����
	ASSERT(FALSE);

	return false;
}

//��ʱ���¼� 
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	//������ʱ
	if(wTimerID==IDI_GAME_COMPAREEND || IDI_GAME_OPENEND==wTimerID)
	{
		//ɾ��ʱ��
		if(wTimerID==IDI_GAME_COMPAREEND)m_pITableFrame->KillGameTimer(IDI_GAME_COMPAREEND);
		else m_pITableFrame->KillGameTimer(IDI_GAME_OPENEND);

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
		printf("������Ϸ������Ϣ666666\n");
		//�������˴洢��
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//��ȡ�û�
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			if(!pIServerUserItem)
				continue;
			if(!pIServerUserItem->IsAndroidUser())
				continue;
			m_pITableFrame->SendTableData(i,SUB_S_ANDROID_BANKOPERATOR);
		}

		//������Ϸ
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

		//���·����û���Ϣ
		for (WORD i=0; i<m_wPlayerCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

			if (!pIServerUserItem)
			{
				continue;
			}

			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
		}
	}
	if(wTimerID==IDI_ADD_SCORE)
	{
		m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
		bool bl = (m_lCompareCount<=0)?false:true;
		if (m_wCurrentUser!=INVALID_CHAIR)
		{
			printf("ϵͳ�Զ���ʱ����m_wCurrentUser_%d\n",m_wCurrentUser);
			if (wBindParam == 2)
			{
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
				if (pIServerUserItem && pIServerUserItem->GetUserStatus() == US_OFFLINE  || pIServerUserItem && pIServerUserItem->IsAndroidUser() == true)
				{

					//�����Ϣ
					TCHAR szBuffer[128]=TEXT("");
					_sntprintf(szBuffer,CountArray(szBuffer),TEXT("�������Զ�����"));
					//�����Ϣ
					CTraceService::TraceString(szBuffer,TraceLevel_Normal);

					m_lCompareCount=0;
					OnUserGiveUp(m_wCurrentUser,false);
				}
				else
				{
					//�Զ�����
					OnUserAutoCompareCard(m_wCurrentUser);
				}
			} 
			else
			{
				OnUserGiveUp(m_wCurrentUser,false);
			}

			//CString str;
			//str.Format(TEXT("��%d���� %d��ʱ����"), m_pITableFrame->GetTableID() + 1, m_wCurrentUser);
			//NcaTextOut(str, m_pGameServiceOption->szServerName);
		}		
	}

	return false;
}

//��Ϸ��Ϣ����
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_GIVE_UP:			//�û�����
		{
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//��Ϣ����
			return OnUserGiveUp(pUserData->wChairID);
		}
	case SUB_C_SHOW_CARD:			//���Ʋ���
		{
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_READY) return true;


			//��Ϣ����
			return OnUserShowCard(pUserData->wChairID);
		}
	case SUB_C_LOOK_CARD:		//�û�����
		{
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//��Ϣ����
			return OnUserLookCard(pUserData->wChairID);
		}
	case SUB_C_COMPARE_CARD:	//�û�����
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_CompareCard));
			if (wDataSize!=sizeof(CMD_C_CompareCard)) return false;

			//��������
			CMD_C_CompareCard * pCompareCard=(CMD_C_CompareCard *)pData;

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//����Ч��
			ASSERT(pUserData->wChairID==m_wCurrentUser);
			if(pUserData->wChairID!=m_wCurrentUser)
			{
				//for (int i =0;i<GAME_PLAYER;i++)
				//{
				//	IServerUserItem * pUserItem = m_pITableFrame->GetTableUserItem(i);
				//	if (pUserItem)
				//	{
				//		//�����Ϣ
				//		TCHAR szBuffer[128]=TEXT("");
				//		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("SUB_C_COMPARE_CARD ���ID [ %d]"),pUserItem->GetGameID());
				//		//�����Ϣ
				//		CTraceService::TraceString(szBuffer,TraceLevel_Normal);
				//		int n = 0;
				//	}
				//}
				//if (pIServerUserItem->IsAndroidUser())
				//{
				//	return OnUserGiveUp(m_wCurrentUser);
				//}

				m_wCurrentUser = pUserData->wChairID;
				//printf("error2\n");

				//return false;
			}

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE && m_cbPlayStatus[pCompareCard->wCompareUser]==TRUE);
			if(m_cbPlayStatus[pUserData->wChairID]==FALSE || m_cbPlayStatus[pCompareCard->wCompareUser]==FALSE)return false;

			//��Ϣ����
			return OnUserCompareCard(pUserData->wChairID,pCompareCard->wCompareUser);
		}
	case SUB_C_ADD_SCORE:		//�û���ע
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) 
			{
				return false;
			}

			//��������
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pData;

			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE && m_lCompareCount==0);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE || m_lCompareCount> 0) 
			{
				//�����Ϣ
				TCHAR szBuffer[128]=TEXT("");
				_sntprintf(szBuffer,CountArray(szBuffer),TEXT("SUB_C_ADD_SCORE ��Ϸ����� [ %.2lf ]"),m_lCompareCount);
				//�����Ϣ
				CTraceService::TraceString(szBuffer,TraceLevel_Normal);
				if (pIServerUserItem->IsAndroidUser())
				{
					m_lCompareCount = 0;
				}
				else
				{
					return false;
				}
			}

			//��ǰ״̬
			if(pAddScore->wState>0)m_lCompareCount=pAddScore->lScore;
			printf("�û���עSUB_C_ADD_SCORE,pUserData->wChairID_%d\n",pUserData->wChairID);
			//��Ϣ����
			bool bOK =  OnUserAddScore(pUserData->wChairID,pAddScore->lScore,false,((pAddScore->wState>0)?true:false), pAddScore->chciphertext);
			if (bOK == false)
			{
				if (pIServerUserItem->IsAndroidUser())
				{
					if (pAddScore->wState > 0)
					{
						//�����Ϣ
						TCHAR szBuffer[128]=TEXT("");
						_sntprintf(szBuffer,CountArray(szBuffer),TEXT("OnUserGiveUp ��Ϸ����� [ %.2lf ]"),777);
						//�����Ϣ
						CTraceService::TraceString(szBuffer,TraceLevel_Normal);
					}

					//��Ϣ����
					return OnUserGiveUp(pUserData->wChairID);
				}
				else
				{
					return false;
				}
			}
			return true;
		}
	case SUB_C_WAIT_COMPARE:	//�ȴ�����
		{
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//����Ч��
			ASSERT(pUserData->wChairID==m_wCurrentUser);
			if(pUserData->wChairID!=m_wCurrentUser)return false;

			//��Ϣ����
			CMD_S_WaitCompare WaitCompare;
			WaitCompare.wCompareUser = pUserData->wChairID;
			if(m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_WAIT_COMPARE,&WaitCompare,sizeof(WaitCompare)) &&
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_WAIT_COMPARE,&WaitCompare,sizeof(WaitCompare)) )

			return true;
		}
	case SUB_C_FINISH_FLASH:	//��ɶ��� 
		{
			//�û�Ч��
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_wFlashUser[pUserData->wChairID]==TRUE);
			if (m_wFlashUser[pUserData->wChairID]==FALSE) return true;

			//���ñ���
			m_wFlashUser[pUserData->wChairID] = FALSE;
			printf("��ɶ���\n");
			//������Ϸ
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_wFlashUser[i] != FALSE)break;
			}
			if(i==GAME_PLAYER)
			{
				//ɾ��ʱ��
				m_pITableFrame->KillGameTimer(IDI_GAME_COMPAREEND);
				m_pITableFrame->KillGameTimer(IDI_GAME_OPENEND);

				//������Ϣ
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
				printf("������Ϸ������Ϣ9999999999\n");
				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}
			return true;
		}
// 	case SUB_C_ADD_SCORE_TIME:
// 		{
// 			//�û�Ч��
// 			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
// 			if (pUserData->cbUserStatus!=US_PLAYING) return true;
// 
// 			m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE*30,1,0);
// 			return true;
// 		}
	case SUB_C_AMDIN_CHANGE_CARD:			//����
		{
			CMD_C_Admin_ChangeCard* pChangeCard = (CMD_C_Admin_ChangeCard*)pData;

			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
			{
				WORD wChairID = pChangeCard->dwChairID;

				IServerUserItem * pIUserItem=m_pITableFrame->GetTableUserItem(wChairID);
				if (CUserRight::IsGameCheatUser(pIUserItem->GetUserRight()) == false)
				{
					if (pIUserItem->IsAndroidUser() == false)
					{
						return true;
					}
				}

				for (WORD i=0;i<3;i++)
				{
					if (m_cbHandCardData[wChairID][i] == pChangeCard->cbStart)
					{
						// 						printf("pChangeCard->cbStart=%d,pChangeCard->cbEnd=%d\n",m_GameLogic.GetCardValue(pChangeCard->cbStart),
						// 							m_GameLogic.GetCardValue(pChangeCard->cbEnd));
						m_cbHandCardData[wChairID][i] = pChangeCard->cbEnd;

						CMD_S_Admin_ChangeCard Admin_ChangeCard;
						ZeroMemory(&Admin_ChangeCard,sizeof(Admin_ChangeCard));
						Admin_ChangeCard.dwCharID = wChairID;

						for (WORD i= 0;i < GAME_PLAYER; i++)
						{
							m_GameLogic.SortCardList(m_cbHandCardData[wChairID], MAX_COUNT);
						}
						CopyMemory(Admin_ChangeCard.cbCardData,m_cbHandCardData[wChairID],sizeof(Admin_ChangeCard.cbCardData));

						//���ط�������
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
							if(pIServerUser==NULL)continue;
							if(CUserRight::IsGameCheatUser(pIServerUser->GetUserRight())==false)
								continue;
							m_pITableFrame->SendTableData(i,SUB_S_ADMIN_COLTERCARD,&Admin_ChangeCard,sizeof(Admin_ChangeCard));
						}
						break;
					}
				}
			}
			return true;
		}
	case SUB_C_STORAGE:
		{
			ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
			if(wDataSize!=sizeof(CMD_C_UpdateStorage)) return false;

			//Ȩ���ж�
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			CMD_C_UpdateStorage *pUpdateStorage=(CMD_C_UpdateStorage *)pData;
			g_lRoomStorageCurrent = pUpdateStorage->lRoomStorageCurrent;
			g_lStorageDeductRoom = pUpdateStorage->lRoomStorageDeduct;

			//20��������¼
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s],�޸ĵ�ǰ���Ϊ %I64d,˥��ֵΪ %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), 
				g_lRoomStorageCurrent, g_lStorageDeductRoom);

			g_ListOperationRecord.AddTail(strOperationRecord);

			//д����־
			strOperationRecord += TEXT("\n");

			CString strFileName;
			strFileName.Format(TEXT("թ��[%s]�����˻���־.log"), m_pGameServiceOption->szServerName);
			WriteInfo(strFileName, strOperationRecord);

			//��������
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//�����±�
			while(posListRecord)
			{
				CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

				CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
	case SUB_C_STORAGEMAXMUL:
		{
			ASSERT(wDataSize==sizeof(CMD_C_ModifyStorage));
			if(wDataSize!=sizeof(CMD_C_ModifyStorage)) return false;

			//Ȩ���ж�
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			CMD_C_ModifyStorage *pModifyStorage = (CMD_C_ModifyStorage *)pData;
			g_lStorageMax1Room = pModifyStorage->lMaxRoomStorage[0];
			g_lStorageMax2Room = pModifyStorage->lMaxRoomStorage[1];
			g_lStorageMul1Room = (SCORE)(pModifyStorage->wRoomStorageMul[0]);
			g_lStorageMul2Room = (SCORE)(pModifyStorage->wRoomStorageMul[1]);

			//20��������¼
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d,�����˻�[%s], �޸Ŀ������ֵ1Ϊ %I64d,Ӯ�ָ���1Ϊ %I64d,����ֵ2Ϊ %I64d,Ӯ�ָ���2Ϊ %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), g_lStorageMax1Room, g_lStorageMul1Room, g_lStorageMax2Room, g_lStorageMul2Room);

			g_ListOperationRecord.AddTail(strOperationRecord);

			//д����־
			strOperationRecord += TEXT("\n");

			CString strFileName;
			strFileName.Format(TEXT("թ��[%s]�����˻���־.log"), m_pGameServiceOption->szServerName);
			WriteInfo(strFileName, strOperationRecord);

			//��������
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//�����±�
			while(posListRecord)
			{
				CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

				CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
	case SUB_C_REQUEST_QUERY_USER:
		{
			ASSERT(wDataSize == sizeof(CMD_C_RequestQuery_User));
			if (wDataSize != sizeof(CMD_C_RequestQuery_User)) 
			{
				return false;
			}

			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
			{
				return false;
			}

			CMD_C_RequestQuery_User *pQuery_User = (CMD_C_RequestQuery_User *)pData;

			//����ӳ��
			POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			CMD_S_RequestQueryResult QueryResult;
			ZeroMemory(&QueryResult, sizeof(QueryResult));

			while(ptHead)
			{
				g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
				if (pQuery_User->dwGameID == userinfo.dwGameID || _tcscmp(pQuery_User->szNickName, userinfo.szNickName) == 0)
				{
					//�����û���Ϣ����
					QueryResult.bFind = true;
					CopyMemory(&(QueryResult.userinfo), &userinfo, sizeof(userinfo));

					ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
					CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
				}
			}

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

			return true;
		}
	case SUB_C_USER_CONTROL:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UserControl));
			if (wDataSize != sizeof(CMD_C_UserControl)) 
			{
				return false;
			}

			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_C_UserControl *pUserControl = (CMD_C_UserControl *)pData;

			//����ӳ��
			POSITION ptMapHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			//20��������¼
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			//��������
			CMD_S_UserControl serverUserControl;
			ZeroMemory(&serverUserControl, sizeof(serverUserControl));

			TCHAR szNickName[LEN_NICKNAME];
			ZeroMemory(szNickName, sizeof(szNickName));

			//�������
			if (pUserControl->userControlInfo.bCancelControl == false)
			{
				ASSERT(pUserControl->userControlInfo.control_type == CONTINUE_WIN || pUserControl->userControlInfo.control_type == CONTINUE_LOST);

				while(ptMapHead)
				{
					g_MapRoomUserInfo.GetNextAssoc(ptMapHead, dwUserID, userinfo);

					if (_tcscmp(pUserControl->szNickName, szNickName) == 0 && _tcscmp(userinfo.szNickName, szNickName) == 0)
					{
						continue;
					}

					if (pUserControl->dwGameID == userinfo.dwGameID || _tcscmp(pUserControl->szNickName, userinfo.szNickName) == 0)
					{
						//������Ʊ�ʶ
						bool bEnableControl = false;
						IsSatisfyControl(userinfo, bEnableControl);

						//�������
						if (bEnableControl)
						{
							ROOMUSERCONTROL roomusercontrol;
							ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));
							CopyMemory(&(roomusercontrol.roomUserInfo), &userinfo, sizeof(userinfo));
							CopyMemory(&(roomusercontrol.userControl), &(pUserControl->userControlInfo), sizeof(roomusercontrol.userControl));


							//������������
							TravelControlList(roomusercontrol);

							//ѹ��������ѹ��ͬGAMEID��NICKNAME)
							g_ListRoomUserControl.AddHead(roomusercontrol);

							//��������
							serverUserControl.dwGameID = userinfo.dwGameID;
							CopyMemory(serverUserControl.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
							serverUserControl.controlResult = CONTROL_SUCCEED;
							serverUserControl.controlType = pUserControl->userControlInfo.control_type;
							serverUserControl.cbControlCount = pUserControl->userControlInfo.cbControlCount;

							//������¼
							CString strOperationRecord;
							CString strControlType;
							GetControlTypeString(serverUserControl.controlType, strControlType);
							CTime time = CTime::GetCurrentTime();
							strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], �������%s,%s,���ƾ���%d "),
								time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName, strControlType, serverUserControl.cbControlCount);

							g_ListOperationRecord.AddTail(strOperationRecord);

							//д����־
							strOperationRecord += TEXT("\n");

							CString strFileName;
							strFileName.Format(TEXT("թ��[%s]�����˻���־.log"), m_pGameServiceOption->szServerName);
							WriteInfo(strFileName, strOperationRecord);
						}
						else	//������
						{
							//��������
							serverUserControl.dwGameID = userinfo.dwGameID;
							CopyMemory(serverUserControl.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
							serverUserControl.controlResult = CONTROL_FAIL;
							serverUserControl.controlType = pUserControl->userControlInfo.control_type;
							serverUserControl.cbControlCount = 0;

							//������¼
							CString strOperationRecord;
							CString strControlType;
							GetControlTypeString(serverUserControl.controlType, strControlType);
							CTime time = CTime::GetCurrentTime();
							strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], �������%s,%s,ʧ�ܣ�"),
								time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName, strControlType);

							g_ListOperationRecord.AddTail(strOperationRecord);

							//д����־
							strOperationRecord += TEXT("\n");

							CString strFileName;
							strFileName.Format(TEXT("թ��[%s]�����˻���־.log"), m_pGameServiceOption->szServerName);
							WriteInfo(strFileName, strOperationRecord);
						}

						//��������
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));

						CMD_S_Operation_Record OperationRecord;
						ZeroMemory(&OperationRecord, sizeof(OperationRecord));
						POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
						WORD wIndex = 0;//�����±�
						while(posListRecord)
						{
							CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

							CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
							wIndex++;
						}

						ASSERT(wIndex <= MAX_OPERATION_RECORD);

						//��������
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

						return true;
					}
				}

				ASSERT(FALSE);
				return false;
			}
			else	//ȡ������
			{
				ASSERT(pUserControl->userControlInfo.control_type == CONTINUE_CANCEL);

				POSITION ptListHead = g_ListRoomUserControl.GetHeadPosition();
				POSITION ptTemp;
				ROOMUSERCONTROL roomusercontrol;
				ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

				//��������
				while(ptListHead)
				{
					ptTemp = ptListHead;
					roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);
					if (pUserControl->dwGameID == roomusercontrol.roomUserInfo.dwGameID || _tcscmp(pUserControl->szNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
					{
						//��������
						serverUserControl.dwGameID = roomusercontrol.roomUserInfo.dwGameID;
						CopyMemory(serverUserControl.szNickName, roomusercontrol.roomUserInfo.szNickName, sizeof(roomusercontrol.roomUserInfo.szNickName));
						serverUserControl.controlResult = CONTROL_CANCEL_SUCCEED;
						serverUserControl.controlType = pUserControl->userControlInfo.control_type;
						serverUserControl.cbControlCount = 0;

						//�Ƴ�Ԫ��
						g_ListRoomUserControl.RemoveAt(ptTemp);

						//��������
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));

						//������¼
						CString strOperationRecord;
						CTime time = CTime::GetCurrentTime();
						strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], ȡ�������%s�Ŀ��ƣ�"),
							time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName);

						g_ListOperationRecord.AddTail(strOperationRecord);

						//д����־
						strOperationRecord += TEXT("\n");

						CString strFileName;
						strFileName.Format(TEXT("թ��[%s]�����˻���־.log"), m_pGameServiceOption->szServerName);
						WriteInfo(strFileName, strOperationRecord);

						CMD_S_Operation_Record OperationRecord;
						ZeroMemory(&OperationRecord, sizeof(OperationRecord));
						POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
						WORD wIndex = 0;//�����±�
						while(posListRecord)
						{
							CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

							CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
							wIndex++;
						}

						ASSERT(wIndex <= MAX_OPERATION_RECORD);

						//��������
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

						return true;
					}
				}

				//��������
				serverUserControl.dwGameID = pUserControl->dwGameID;
				CopyMemory(serverUserControl.szNickName, pUserControl->szNickName, sizeof(serverUserControl.szNickName));
				serverUserControl.controlResult = CONTROL_CANCEL_INVALID;
				serverUserControl.controlType = pUserControl->userControlInfo.control_type;
				serverUserControl.cbControlCount = 0;

				//��������
				m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
				m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));

				//������¼
				CString strOperationRecord;
				CTime time = CTime::GetCurrentTime();
				strOperationRecord.Format(TEXT("����ʱ��: %d/%d/%d-%d:%d:%d, �����˻�[%s], ȡ�������%s�Ŀ��ƣ�������Ч��"),
					time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName);

				g_ListOperationRecord.AddTail(strOperationRecord);

				//д����־
				strOperationRecord += TEXT("\n");

				CString strFileName;
				strFileName.Format(TEXT("թ��[%s]�����˻���־.log"), m_pGameServiceOption->szServerName);
				WriteInfo(strFileName, strOperationRecord);

				CMD_S_Operation_Record OperationRecord;
				ZeroMemory(&OperationRecord, sizeof(OperationRecord));
				POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
				WORD wIndex = 0;//�����±�
				while(posListRecord)
				{
					CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

					CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
					wIndex++;
				}

				ASSERT(wIndex <= MAX_OPERATION_RECORD);

				//��������
				m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
				m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			}

			return true;
		}
	case SUB_C_REQUEST_UDPATE_ROOMINFO:
		{
			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_S_RequestUpdateRoomInfo_Result RoomInfo_Result;
			ZeroMemory(&RoomInfo_Result, sizeof(RoomInfo_Result));

			RoomInfo_Result.lRoomStorageCurrent = g_lRoomStorageCurrent;


			DWORD dwKeyGameID = g_CurrentQueryUserInfo.dwGameID;
			TCHAR szKeyNickName[LEN_NICKNAME];	
			ZeroMemory(szKeyNickName, sizeof(szKeyNickName));
			CopyMemory(szKeyNickName, g_CurrentQueryUserInfo.szNickName, sizeof(szKeyNickName));

			//����ӳ��
			POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			while(ptHead)
			{
				g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
				if (dwKeyGameID == userinfo.dwGameID && _tcscmp(szKeyNickName, userinfo.szNickName) == 0)
				{
					//�����û���Ϣ����
					CopyMemory(&(RoomInfo_Result.currentqueryuserinfo), &userinfo, sizeof(userinfo));

					ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
					CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
				}
			}

			//
			//��������
			POSITION ptListHead = g_ListRoomUserControl.GetHeadPosition();
			POSITION ptTemp;
			ROOMUSERCONTROL roomusercontrol;
			ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

			//��������
			while(ptListHead)
			{
				ptTemp = ptListHead;
				roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

				//Ѱ�����
				if ((dwKeyGameID == roomusercontrol.roomUserInfo.dwGameID) &&
					_tcscmp(szKeyNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
				{
					RoomInfo_Result.bExistControl = true;
					CopyMemory(&(RoomInfo_Result.currentusercontrol), &(roomusercontrol.userControl), sizeof(roomusercontrol.userControl));
					break;				
				}
			}

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT, &RoomInfo_Result, sizeof(RoomInfo_Result));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT, &RoomInfo_Result, sizeof(RoomInfo_Result));

			return true;
		}
	case SUB_C_CLEAR_CURRENT_QUERYUSER:
		{
			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

			return true;
		}
	}

	return false;
}

//�����Ϣ����
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}



//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	//����ͬ���û�����
	UpdateUserControl(pIServerUserItem);
	
	//��ʼ����Կ
	//if (!bLookonUser)
	//{
		//m_chUserAESKey[wChairID][0] = (HIBYTE(HIWORD(pIServerUserItem->GetUserID())));
		//m_chUserAESKey[wChairID][1] = (LOBYTE(HIWORD(pIServerUserItem->GetUserID())));
		//m_chUserAESKey[wChairID][2] = (HIBYTE(LOWORD(pIServerUserItem->GetUserID())));
		//m_chUserAESKey[wChairID][3] = (LOBYTE(LOWORD(pIServerUserItem->GetUserID())));
		//m_chUserAESKey[wChairID][4] = rand() % 256;
		//m_chUserAESKey[wChairID][5] = rand() % 256;
		//m_chUserAESKey[wChairID][6] = rand() % 256;
		//m_chUserAESKey[wChairID][7] = rand() % 256;
		//m_chUserAESKey[wChairID][8] = rand() % 256;
		//m_chUserAESKey[wChairID][9] = rand() % 256;
		//m_chUserAESKey[wChairID][10] = rand() % 256;
		//m_chUserAESKey[wChairID][11] = rand() % 256;
		//m_chUserAESKey[wChairID][12] = rand() % 256;
		//m_chUserAESKey[wChairID][13] = rand() % 256;
		//m_chUserAESKey[wChairID][14] = rand() % 256;
		//m_chUserAESKey[wChairID][15] = rand() % 256;
	//}
	
	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
	
	if (!bLookonUser)
	{
		//�����Կ
		ZeroMemory(m_chUserAESKey[wChairID], sizeof(m_chUserAESKey[wChairID]));
	}

	return true;
}

//�û�ͬ��
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//˽�˷�������Ϸģʽ
	if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0)
	{
		//cbGameRule[1] Ϊ  2 ��3 ��4 ��5, 0�ֱ��Ӧ 2�� �� 3�� �� 4�� �� 5�� �� 2-5�� �⼸������
		BYTE *pGameRule = m_pITableFrame->GetGameRule();
		switch(pGameRule[1])
		{
		case 2:
		case 3:
		case 4:
		case 5:
			{
				if (m_pITableFrame->GetStartMode() != START_MODE_FULL_READY)
				{
					m_pITableFrame->SetStartMode(START_MODE_FULL_READY);
				}

				break;
			}
		case 0:
			{
				if (m_pITableFrame->GetStartMode() != START_MODE_ALL_READY)
				{
					m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
				}
				break;
			}
		default:
			ASSERT(false);

		}
	}

	return true;
}

//�����¼�
bool CTableFrameSink::OnUserGiveUp(WORD wChairID,bool bExit)
{
	//��������
	m_cbGiveUpUser[wChairID] = TRUE;
	m_cbPlayStatus[wChairID] = FALSE;

	//������Ϣ
	CMD_S_GiveUp GiveUp;
	GiveUp.wGiveUpUser=wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));

	//�޸Ļ���
	tagScoreInfo ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
	ScoreInfo.cbType =(bExit)?SCORE_TYPE_FLEE:SCORE_TYPE_LOSE;
	ScoreInfo.lRevenue=0;  
	ScoreInfo.lScore=-m_lTableScore[wChairID];

	m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);

	//��ȡ�û�
	IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);

	//���ͳ��
	if ((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser())) 
		UpdateStorage(m_lTableScore[wChairID]);//g_lRoomStorageCurrent += m_lTableScore[wChairID];

	m_dElapse = GetTickCount();

	//����ͳ��
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//�жϽ���
	if (wPlayerCount>=2)
	{
		//���ƺ�������Ϸ״̬ΪSIT
		if (pIServerUserIte)
		{
			if (pIServerUserIte->GetUserStatus() == US_NULL)
			{
				pIServerUserIte->SetUserStatus(US_NULL, pIServerUserIte->GetTableID(), pIServerUserIte->GetChairID());
			}
			else if (pIServerUserIte->GetUserStatus() == US_OFFLINE)
			{
				pIServerUserIte->SetUserStatus(US_OFFLINE, pIServerUserIte->GetTableID(), pIServerUserIte->GetChairID());
			}
			else
			{
				pIServerUserIte->SetUserStatus(US_SIT, pIServerUserIte->GetTableID(), pIServerUserIte->GetChairID());
			}

			if (pIServerUserIte->GetUserScore() < m_pGameServiceOption->lMinEnterScore)
			{
				//m_pITableFrame->PerformStandUpAction(pIServerUserIte);
			}
		}

		AESEncrypt aes(m_chUserAESKey[wChairID]);
		unsigned char chTempEncryption[AESENCRYPTION_LENGTH];
		ZeroMemory(chTempEncryption, sizeof(chTempEncryption));
		CopyMemory(chTempEncryption, chCommonEncryption, sizeof(chTempEncryption));

		aes.Cipher(chTempEncryption);
	
		if (m_wCurrentUser==wChairID) 
		{
			printf("������ע���_%d\n",m_wCurrentUser);
			m_lCompareCount = 0;
			OnUserAddScore(wChairID,0L,true,false, chTempEncryption);
		}
	}
	else OnEventGameConclude(INVALID_CHAIR,NULL,GER_NO_PLAYER);


	
	return true;
}

//�����¼�
bool CTableFrameSink::OnUserLookCard(WORD wChairID)
{
	//״̬Ч��-liuyang
	//ASSERT(m_wCurrentUser==wChairID);
	//if (m_wCurrentUser!=wChairID) return false;

	//����Ч��
	ASSERT(!m_bMingZhu[wChairID]);
	if (m_bMingZhu[wChairID]) return true;

	//���ò���
	m_bMingZhu[wChairID]=true;
	//��������
	CMD_S_LookCard LookCard;
	ZeroMemory(&LookCard,sizeof(LookCard));
	LookCard.wLookCardUser=wChairID;

	m_dElapse = GetTickCount();

	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LOOK_CARD,&LookCard,sizeof(LookCard));
	printf("�û�����OnUserLookCard_m_wCurrentUser_%d,GetChairID()_%d\n",m_wCurrentUser,wChairID);
	for(WORD i = 0;i<m_wPlayerCount;i++)
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem == NULL ) continue;
		if(i == wChairID)
		{
			CopyMemory(LookCard.cbCardData,m_cbHandCardData[wChairID],sizeof(m_cbHandCardData[0]));
		}
		else
		{
			ZeroMemory(LookCard.cbCardData,sizeof(LookCard.cbCardData));
		}
		m_pITableFrame->SendTableData(i,SUB_S_LOOK_CARD,&LookCard,sizeof(LookCard));
	}

	return true;
}

//�Զ������¼�
bool CTableFrameSink::OnUserAutoCompareCard(WORD wChairID)
{
	//������Ϣ
	CMD_S_AutoCompare AutoCompare;
	AutoCompare.wAutoCompareUser=wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_AUTO_COMPARE,&AutoCompare,sizeof(AutoCompare));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_AUTO_COMPARE,&AutoCompare,sizeof(AutoCompare));

	return true;
}

//�����¼�
bool CTableFrameSink::OnUserCompareCard(WORD wFirstChairID,WORD wNextChairID)
{
	//Ч�����
	ASSERT(m_lCompareCount>0);
	if(!(m_lCompareCount>0))
	{
		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("OnUserCompareCard.log"));
		strtip.Format(TEXT("OnUserCompareCard  m_lCompareCount>0 m_lCompareCount=%lld [%d-%d-%d %d:%d:%d]\n"), m_lCompareCount,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
		IServerUserItem * pIServreUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
		if (pIServreUserItem->IsAndroidUser())
		{
			OnUserGiveUp(m_wCurrentUser,false);
			m_lCompareCount=0;
			return true;
		}
		return false;
	}
	//LONGLONG lTemp=(m_bMingZhu[m_wCurrentUser])?6:5;
	//ASSERT((m_lUserMaxScore[m_wCurrentUser]-m_lTableScore[m_wCurrentUser]+m_lCompareCount) >= (m_lMaxCellScore*lTemp));
	//if((m_lUserMaxScore[m_wCurrentUser]-m_lTableScore[m_wCurrentUser]+m_lCompareCount) < (m_lMaxCellScore*lTemp))return false;
	//ԭ��>=2*m_lCellScore�������һ�ֶ��Ļ��ֶ�û�ܴﵽ��ע��ע����������ôֻ�ܿ��ƣ�����ע����m_lCellScore��
	//��ô���ƾ�һ������ߣ���ʱm_lTableScore[m_wCurrentUser]Ϊ��ע+m_lCompareCount��
	//����ΪʲôҪ���ڵ���2*m_lCellScore��ԭ����
	ASSERT(m_wCurrentUser==m_wBankerUser || m_lTableScore[m_wCurrentUser]-m_lCompareCount>=m_lCellScore);	
	if(!(m_wCurrentUser==m_wBankerUser || m_lTableScore[m_wCurrentUser]-m_lCompareCount>=m_lCellScore))
	{
		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("OnUserCompareCard.log"));
		strtip.Format(TEXT("OnUserCompareCard  m_wCurrentUser==m_wBankerUser || m_lTableScore[m_wCurrentUser]=%lf  m_lCompareCount=%lf m_lCellScore=%lf [%d-%d-%d %d:%d:%d]\n"), m_lTableScore[m_wCurrentUser],m_lCompareCount,m_lCellScore,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		WriteInfo(strFileName, strtip);

		return false;
	}
	printf("�û�����OnUserCompareCard_wFirstChairID_%d,wNextChairID_%d\n",wFirstChairID,wNextChairID);
	//ɾ����ʱ��
	m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);

	//�Ƚϴ�С
	BYTE cbWin=m_GameLogic.CompareCard(m_cbHandCardData[wFirstChairID],m_cbHandCardData[wNextChairID],MAX_COUNT);

	//״̬����
	m_lCompareCount=0;

	//ʤ���û�
	WORD wLostUser,wWinUser;
	if(cbWin==TRUE) 
	{
		wWinUser=wFirstChairID;
		wLostUser=wNextChairID;
	}
	else
	{
		wWinUser=wNextChairID;
		wLostUser=wFirstChairID;
	}

	if (m_pITableFrame->GetTableUserItem(wLostUser)->GetUserStatus() == US_NULL)
	{
		m_pITableFrame->GetTableUserItem(wLostUser)->SetUserStatus(US_NULL, m_pITableFrame->GetTableID(), wLostUser);
	}
	else if (m_pITableFrame->GetTableUserItem(wLostUser)->GetUserStatus() == US_OFFLINE)
	{
		m_pITableFrame->GetTableUserItem(wLostUser)->SetUserStatus(US_OFFLINE, m_pITableFrame->GetTableID(), wLostUser);
	}
	else
	{
		m_pITableFrame->GetTableUserItem(wLostUser)->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), wLostUser);
	}

	//���ƺ�������Ϸ״̬ΪSIT
	//m_pITableFrame->GetTableUserItem(wLostUser)->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), wLostUser);

	//��������
	m_wCompardUser[wLostUser].Add(wWinUser);
	m_wCompardUser[wWinUser].Add(wLostUser);
	m_cbPlayStatus[wLostUser]=FALSE;

	//����ͳ��
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//������Ϸ
	if(wPlayerCount>=2)
	{
		//�û��л�
		WORD wNextPlayer=INVALID_CHAIR;
		for (WORD i=1;i<m_wPlayerCount;i++)
		{
			//���ñ���
			wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

			//�����ж�
			if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
		}
		//�����û�
		m_wCurrentUser=wNextPlayer;
	}
	else 
	{
		m_wCurrentUser=INVALID_CHAIR;
		if (m_wCurrentUser==INVALID_CHAIR)
		{
			CString strtip;
			CTime time = CTime::GetCurrentTime();
			CString strFileName;
			strFileName.Format(TEXT("թ�𻨻�����IDI_USER_ADD_SCORE.log"));
			strtip.Format(TEXT("OnUserCompareCard  m_wCurrentUser==INVALID_CHAIR [%d-%d-%d %d:%d:%d]\n"), m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
			//WriteInfo(strFileName, strtip);
		}
	}

	//��������
	CMD_S_CompareCard CompareCard;
	CompareCard.wCurrentUser = m_wCurrentUser;
	CompareCard.wLostUser = wLostUser;
	CompareCard.wCompareUser[0] = wFirstChairID;
	CompareCard.wCompareUser[1] = wNextChairID;

	m_dElapse = GetTickCount();

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_COMPARE_CARD,&CompareCard,sizeof(CompareCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_COMPARE_CARD,&CompareCard,sizeof(CompareCard));

	//�޸Ļ���
	tagScoreInfo ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
	ScoreInfo.cbType =SCORE_TYPE_LOSE;
	ScoreInfo.lRevenue=0;
	ScoreInfo.lScore=-m_lTableScore[wLostUser];

	m_pITableFrame->WriteUserScore(wLostUser, ScoreInfo);
	
	//���ͳ��
	IServerUserItem * pIServreUserItemLost=m_pITableFrame->GetTableUserItem(wLostUser);
	if ((pIServreUserItemLost!=NULL)&&(!pIServreUserItemLost->IsAndroidUser())) 
		UpdateStorage(m_lTableScore[wLostUser]);//g_lRoomStorageCurrent += m_lTableScore[wLostUser];

	//������Ϸ
	//if (wPlayerCount<2)
	//{
	//	m_wBankerUser=wWinUser;
	//	m_wFlashUser[wNextChairID] = TRUE;
	//	m_wFlashUser[wFirstChairID] = TRUE;
	//	OnEventGameConclude(GAME_PLAYER,NULL,GER_COMPARECARD);
	//}

	////���ö�ʱ�� �Ƿ�������
	//if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) == 0)
	//{
	//	m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE,1,0);
	//}

	if (wPlayerCount<2)
	{
		m_wBankerUser=wWinUser;
		m_wFlashUser[wNextChairID] = TRUE;
		m_wFlashUser[wFirstChairID] = TRUE;
		OnEventGameConclude(GAME_PLAYER,NULL,GER_COMPARECARD);
		printf("��Ϸ����\n");
	}
	else
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wLostUser);
		if (pIServerUserItem->GetUserScore() < m_pGameServiceOption->lMinEnterScore)
		{
			//m_pITableFrame->PerformStandUpAction(pIServerUserItem);
		}
		//���ö�ʱ�� �Ƿ�������
		if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) == 0)
		{
			m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
			m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE,1,2);
		}
	}

	return true;
}

//�����¼�
bool CTableFrameSink::OnUserOpenCard(WORD wUserID)
{
	//Ч�����
	ASSERT(m_lCompareCount>0);
	if(!(m_lCompareCount>0))return false;
	ASSERT(m_wCurrentUser==wUserID);
	if(m_wCurrentUser!=wUserID)return false;
	//LONGLONG lTemp=(m_bMingZhu[wUserID])?6:5;
	//ASSERT((m_lUserMaxScore[wUserID]-m_lTableScore[wUserID]+m_lCompareCount) < (m_lMaxCellScore*lTemp));
	//if((m_lUserMaxScore[wUserID]-m_lTableScore[wUserID]+m_lCompareCount) >= (m_lMaxCellScore*lTemp))return false;

	//��������
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount = 0;

	if (m_wCurrentUser==INVALID_CHAIR)
	{
		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("թ�𻨻�����IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnEventGameStart  m_wCurrentUser==INVALID_CHAIR [%d-%d-%d %d:%d:%d]\n"), m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}

	//�����˿�
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

	//�������
	WORD wWinner=wUserID;

	//����������
	for (WORD i=1;i<m_wPlayerCount;i++)
	{
		WORD w=(wUserID+i)%m_wPlayerCount;

		//�û�����
		if (m_cbPlayStatus[w]==FALSE) continue;

		//�Ա��˿�
		if (m_GameLogic.CompareCard(cbUserCardData[w],cbUserCardData[wWinner],MAX_COUNT)>=TRUE) 
		{
			wWinner=w;
		}
	}
	ASSERT(m_cbPlayStatus[wWinner]==TRUE);
	if(m_cbPlayStatus[wWinner]==FALSE)return false;

	//ʤ�����
	m_wBankerUser = wWinner;

	//��������
	CMD_S_OpenCard OpenCard;
	OpenCard.wWinner=wWinner;

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));

	//������Ϸ
	for(WORD i=0;i<GAME_PLAYER;i++)if(m_cbPlayStatus[i]==TRUE)m_wFlashUser[i] = TRUE;
	OnEventGameConclude(GAME_PLAYER,NULL,GER_OPENCARD);

	return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserAddScore(WORD wChairID, SCORE lScore, bool bGiveUp, bool bCompareCard, unsigned char	chciphertext[AESENCRYPTION_LENGTH])
{

	CString strtip;
	CTime time = CTime::GetCurrentTime();
	CString strFileName;
	//strFileName.Format(TEXT("թ��[%s]OnUserAddScore.log"), m_pGameServiceOption->szServerName);
	strFileName.Format(TEXT("թ��OnUserAddScore.log"));

	if (bGiveUp==false)				//��������
	{
		//���ܲ���
		AESEncrypt aes(m_chUserAESKey[wChairID]);
		aes.InvCipher(chciphertext);

		//У������
		//for (WORD i=0; i<AESENCRYPTION_LENGTH; i++)
		//{
		//	if (chCommonEncryption[i] != chciphertext[i])
		//	{
		//		//��ӡ��������
		//		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		//		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser() == false)
		//		{
		//			strtip.Format(TEXT("%d/%d/%d:%d-%d-%d___���[%s] USERIDΪ%d ����\n"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(),
		//				pIServerUserItem->GetNickName(), pIServerUserItem->GetUserID());

		//			WriteInfo(strFileName, strtip);
		//		}

		//		return false;
		//	}
		//}

		//״̬Ч��
		ASSERT(m_wCurrentUser==wChairID);
		if (m_wCurrentUser!=wChairID) 
		{
			strtip.Format(TEXT("m_wCurrentUser!=wChairID! m_wCurrentUser = %d  wChairID = %d bGiveUp = %d bCompareCard = %d [%d-%d-%d %d:%d:%d]\n"), m_wCurrentUser,wChairID,bGiveUp,bCompareCard,time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			WriteInfo(strFileName, strtip);
			return false;
		}

		//���Ч��
		//ASSERT(lScore>=0 && abs(fmod(lScore,m_lCellScore))==0);
		//ASSERT((lScore+m_lTableScore[wChairID])<=m_lUserMaxScore[wChairID]);
		//if (lScore<0 || abs(fmod(lScore,m_lCellScore))==0)
		//{
		//	strtip.Format(TEXT("lScore<0 || lScore%m_lCellScore!=0  lScore = %lld  lScore%m_lCellScore = %d[%d-%d-%d:%d:%d:%d]\n"), lScore,abs(fmod(lScore,m_lCellScore))==0,time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
		//	WriteInfo(strFileName, strtip);
		//	return false;
		//}
		IServerUserItem * pIServreUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServreUserItem)
		{
			if (m_lTableScore[wChairID] + lScore >= pIServreUserItem->GetUserInfo()->lScore|| m_nCurrentRounds >= m_nTotalRounds)
			{
				//�û�ע��
				if (m_lTableScore[wChairID] + lScore >= pIServreUserItem->GetUserInfo()->lScore)
				{
					lScore = pIServreUserItem->GetUserInfo()->lScore-m_lTableScore[wChairID];
					m_lTableScore[wChairID]+=lScore;
				}
				else if (m_nCurrentRounds >= m_nTotalRounds)
				{
					m_lTableScore[wChairID]+=lScore;
				}
				//�����û�
				if(!bCompareCard)
				{
					//�û��л�
					WORD wNextPlayer=INVALID_CHAIR;
					for (WORD i=1;i<m_wPlayerCount;i++)
					{
						//���ñ���
						wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;
						//�����ж�
						if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
					}
					m_wCurrentUser=wNextPlayer;
				}

				//��������
				CMD_S_AddScore AddScore;
				AddScore.lCurrentTimes=m_lCurrentTimes;
				AddScore.wCurrentUser=m_wCurrentUser;
				AddScore.wAddScoreUser=wChairID;
				AddScore.lAddScoreCount=lScore;
				AddScore.nNetwaititem = 1;
				if (m_nCurrentRounds >= m_nTotalRounds)
				{
					AddScore.nNetwaititem = 2;
				}
				m_dElapse = GetTickCount();

				map<DWORD, int>::iterator itr = m_RoundsMap.find(wChairID);  
				if(itr == m_RoundsMap.end()) 
				{
					m_RoundsMap[wChairID] = 1;
				}
				else
				{
					int nRounds = itr->second;
					m_RoundsMap[wChairID] = nRounds + 1;
					m_nCurrentRounds = m_RoundsMap[wChairID];
				}

				AddScore.nCurrentRounds = m_RoundsMap[wChairID];

				//�ж�״̬
				AddScore.wCompareState=( bCompareCard )?TRUE:FALSE;

				//��������
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));


				//��������
				m_wCurrentUser=INVALID_CHAIR;

				//�����˿�
				BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
				CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

				//wChairID
				WORD wWinner=wChairID;

				//����������
				for (WORD i=1;i<m_wPlayerCount;i++)
				{
					WORD w=(wChairID+i)%m_wPlayerCount;

					//�û�����
					if (m_cbPlayStatus[w]==FALSE) continue;

					//�Ա��˿�
					if (m_GameLogic.CompareCard(cbUserCardData[w],cbUserCardData[wWinner],MAX_COUNT)>=TRUE) 
					{
						wWinner=w;
					}
				}
				ASSERT(m_cbPlayStatus[wWinner]==TRUE);
				if(m_cbPlayStatus[wWinner]==FALSE)
				{
					return false;
				}

				//ʤ�����
				m_wBankerUser = wWinner;

				//������Ϸ
				for(WORD i=0;i<GAME_PLAYER;i++)if(m_cbPlayStatus[i]==TRUE)m_wFlashUser[i] = TRUE;
				m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
				OnEventGameConclude(GAME_PLAYER,NULL,GER_OPENCARD);
				//ɾ����ʱ��
				m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);

				return true;
			}
		}
		if ((lScore+m_lTableScore[wChairID])>m_lUserMaxScore[wChairID]) 
		{
			strtip.Format(TEXT("lScore+m_lTableScore[wChairID])>m_lUserMaxScore[wChairID]  lScore = %.2f  m_lTableScore[wChairID] = %.2f  wChairID = %d  m_lUserMaxScore[wChairID] = %.2f [%d/%d/%d:%d-%d-%d]\n"), lScore,m_lTableScore[wChairID],wChairID,m_lUserMaxScore[wChairID],time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			WriteInfo(strFileName, strtip);
			return false;
		}
		printf("�û���עSUB_C_ADD_SCORE,pUserData->wChairID_%d----111111111111\n",wChairID);


		IServerUserItem *pIserberUser=m_pITableFrame->GetTableUserItem(wChairID);
		if(pIserberUser->GetUserScore() < m_lTableScore[wChairID]+lScore) 
		{
			strtip.Format(TEXT("pIserberUser->GetUserScore() < m_lTableScore[wChairID]+lScore  pIserberUser->GetUserScore() = %.2f  m_lTableScore[wChairID]+lScore = %.2f[%d-%d-%d:%d:%d:%d]\n"), pIserberUser->GetUserScore(),m_lTableScore[wChairID]+lScore,time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			WriteInfo(strFileName, strtip);
			return false;
		}
		printf("�û���עSUB_C_ADD_SCORE,pUserData->wChairID_%d----111111111111\n",wChairID);
		//��ǰ����
		LONGLONG lTimes=(m_bMingZhu[wChairID] || bCompareCard)?2:1;
		if(m_bMingZhu[wChairID] && bCompareCard)lTimes=4;
		LONGLONG lTemp=lScore/m_lCellScore/lTimes;
		//ASSERT(m_lCurrentTimes<=lTemp && m_lCurrentTimes<=m_lMaxCellScore/m_lCellScore);
		//
		//if(!(m_lCurrentTimes<=lTemp && m_lCurrentTimes<=m_lMaxCellScore/m_lCellScore))
		//{
		//	strtip.Format(TEXT("m_lCurrentTimes<=lTemp && m_lCurrentTimes<=m_lMaxCellScore/m_lCellScore  m_lCurrentTimes = %d  lTemp = %d  m_lMaxCellScore = %d m_lCellScore = %d[%d/%d/%d:%d-%d-%d]\n"), m_lCurrentTimes,lTemp,m_lMaxCellScore,m_lCellScore,time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
		//	WriteInfo(strFileName, strtip);
		//	return false;
		//}

		m_lCurrentTimes = lTemp;

		//�û�ע��
		m_lTableScore[wChairID]+=lScore;
	}

	//�����û�
	if(!bCompareCard)
	{
		//�û��л�
		WORD wNextPlayer=INVALID_CHAIR;
		for (WORD i=1;i<m_wPlayerCount;i++)
		{
			//���ñ���
			wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

			//�����ж�
			if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
		}
		m_wCurrentUser=wNextPlayer;
	}

	if (m_wCurrentUser==INVALID_CHAIR)
	{
		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("թ�𻨻�����IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnUserAddScore  m_wCurrentUser==INVALID_CHAIR [%d-%d-%d %d:%d:%d]\n"), m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}

	//������Կ
	unsigned char chTempRandAesKey[AESKEY_VARIABLECOUNT];
	ZeroMemory(chTempRandAesKey, sizeof(chTempRandAesKey));
	RandAESKey(chTempRandAesKey, AESKEY_VARIABLECOUNT);

	//������Կ
	CopyMemory(&m_chUserAESKey[wChairID][AESKEY_TOTALCOUNT - AESKEY_VARIABLECOUNT], chTempRandAesKey, sizeof(chTempRandAesKey));
	CMD_S_UpdateAESKey UpdateAESKey;
	ZeroMemory(&UpdateAESKey, sizeof(UpdateAESKey));
	CopyMemory(UpdateAESKey.chUserUpdateAESKey, m_chUserAESKey[wChairID], sizeof(m_chUserAESKey[wChairID]));

	m_pITableFrame->SendTableData(wChairID, SUB_S_UPDATEAESKEY, &UpdateAESKey, sizeof(UpdateAESKey));

	//��������
	CMD_S_AddScore AddScore;
	AddScore.lCurrentTimes=m_lCurrentTimes;
	AddScore.wCurrentUser=m_wCurrentUser;
	AddScore.wAddScoreUser=wChairID;
	AddScore.lAddScoreCount=lScore;
	AddScore.nNetwaititem = 0;
	m_dElapse = GetTickCount();

	map<DWORD, int>::iterator itr = m_RoundsMap.find(wChairID);  
	if(itr == m_RoundsMap.end()) 
	{
		m_RoundsMap[wChairID] = 1;
	}
	else
	{
		int nRounds = itr->second;
		m_RoundsMap[wChairID] = nRounds + 1;
		m_nCurrentRounds = m_RoundsMap[wChairID];
	}

	AddScore.nCurrentRounds = m_RoundsMap[wChairID];

	//�ж�״̬
	AddScore.wCompareState=( bCompareCard )?TRUE:FALSE;
	printf("�û���עSUB_C_ADD_SCORE,pUserData->wChairID_%d----111111111111\n",wChairID);
	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

	//���ط�������
	//WORD wIndex = 0;
	//CMD_S_AllCard AllCard;
	//ZeroMemory(&AllCard,sizeof(CMD_S_AllCard));
	//for(WORD i=0; i<m_wPlayerCount; i++)
	//{
	//	IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
	//	if(pIServerUser==NULL)continue;
	//	if(m_cbPlayStatus[i]==FALSE)continue;
	//	//if(pIServerUser->IsAndroidUser())
	//	//{
	//	//	AllCard.bAICount[i] =true;
	//	//}

	//	//�ɷ��˿�
	//	CopyMemory(AllCard.cbCardData[i],&m_bTempArray[wIndex*MAX_COUNT],MAX_COUNT);
	//	wIndex++;
	//}

	//for (WORD i=0;i<m_wPlayerCount;i++)
	//{
	//	if(m_cbPlayStatus[i]!=true)continue;
	//	IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
	//	if(pIServerUser==NULL)continue;
	//	CopyMemory(AllCard.cbSurplusCardData,&m_bTempArray[wIndex*MAX_COUNT],52-wIndex*MAX_COUNT);
	//	AllCard.cbSurplusCardCount = 52-wIndex*MAX_COUNT;
	//	//if(CUserRight::IsGameCheatUser(pIServerUser->GetUserRight())==false)continue;
	//	m_pITableFrame->SendTableData(i,SUB_S_ALL_CARD,&AllCard,sizeof(AllCard));
	//}
	//ɾ����ʱ��
	m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
	//���ö�ʱ��
	if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) == 0)
	{
		//m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE,1,0);
		if (bCompareCard)
		{
			m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE,1,2);
		} 
		else
		{
			m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE,1,0);
		}
	}
	
	return true;
}

////�˿˷���
//void CTableFrameSink::AnalyseStartCard()
//{
//	//��������
//	WORD wAiCount = 0;
//	WORD wPlayerCount = 0;
//	for (WORD i=0; i<m_wPlayerCount; i++)
//	{
//		//��ȡ�û�
//		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
//		if (pIServerUserItem!=NULL)
//		{
//			if(m_cbPlayStatus[i]==FALSE)continue;
//			if(pIServerUserItem->IsAndroidUser()) 
//			{
//				wAiCount++ ;
//			}
//			wPlayerCount++;
//		}
//	}
//
//	//ȫ������
//	//if(wPlayerCount == wAiCount || wAiCount==0) return;
//
//	if(wPlayerCount == wAiCount) return;
//
//	//�˿˱���
//	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
//	CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
//
//	//�����˿�
//	for (WORD i=0;i<m_wPlayerCount;i++)
//	{
//		m_GameLogic.SortCardList(cbUserCardData[i], MAX_COUNT);
//	}
//
//	//��������
//	WORD wWinUser = INVALID_CHAIR;
//
//	//��������
//	for (WORD i = 0; i < m_wPlayerCount; i++)
//	{
//		//�û�����
//		if (m_cbPlayStatus[i] == FALSE) continue;
//
//		//�����û�
//		if (wWinUser == INVALID_CHAIR)
//		{
//			wWinUser=i;
//			continue;
//		}
//
//		//�Ա��˿�
//		if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinUser], MAX_COUNT) >= TRUE)
//		{
//			wWinUser = i;
//		}
//	}
//
//	int nRand = rand()%100;
//	int nControl = m_MapControlID.GetCount();
//
//	if (nControl > 0)
//	{
//		//*****************************���˵��أ������ܿأ���������µ��˿���Ϊ׼*************************//
//		WORD wControlPlayer = INVALID_CHAIR;
//		int nRate = 0;
//
//		CMapControlID::CPair* pValue=  m_MapControlID.PGetFirstAssoc();  
//		//while(pValue != NULL)  
//		//{  
//		//	wControlPlayer = pValue->key;
//		//	nRate = pValue->value;
//		//	pValue = m_MapControlID.PGetNextAssoc(pValue);
//		//}
//
//		wControlPlayer = pValue->key;
//		nRate = pValue->value;
//		//pValue = m_MapControlID.PGetNextAssoc(pValue);
//
//		if (nRand > nRate)
//		{
//			//��������ܿ������
//			WORD wBeWin = INVALID_CHAIR;
//
//			//do 
//			//{
//			//	wBeWin = rand()%GAME_PLAYER;
//			//} while (m_pITableFrame->GetTableUserItem(wBeWin) == NULL || wBeWin == wControlPlayer);
//			for (int i =0;i<GAME_PLAYER;i++)
//			{
//				if(m_pITableFrame->GetTableUserItem(i) == NULL)continue;
//				if (i == wControlPlayer)continue;
//				wBeWin = i;
//				break;
//			}
//
//			if(wBeWin != wWinUser)
//			{
//				//��������
//				BYTE cbTempData[MAX_COUNT];
//				CopyMemory(cbTempData, m_cbHandCardData[wBeWin], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wBeWin], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
//			}
//			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wBeWin);
//			if (pIServerUserItem)
//			{
//				printf("�������ʤ��:%d,ID:%d\n",nRate,pIServerUserItem->GetGameID());
//				printf("��������ܿ������,Ӯ����ID:%d\n",pIServerUserItem->GetGameID());
//			}
//
//		} 
//		else
//		{
//			//��������ܿ����Ӯ
//
//			if(wControlPlayer != wWinUser)
//			{
//				//��������
//				BYTE cbTempData[MAX_COUNT];
//				CopyMemory(cbTempData, m_cbHandCardData[wControlPlayer], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wControlPlayer], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
//			}
//			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wControlPlayer);
//			if (pIServerUserItem)
//			{
//				printf("�������ʤ��:%d,ID:%d\n",nRate,pIServerUserItem->GetGameID());
//				printf("��������ܿ����Ӯ,ID:%d\n",pIServerUserItem->GetGameID());
//			}
//		}
//		//*****************************���˵��أ������ܿأ���������µ��˿���Ϊ׼END**********************//
//	} 
//	else
//	{
//		//*****************************ȫ�ֿ��ƣ��ػ�������Ӯ********************************************//
//		//�����ļ���
//		TCHAR szPath[MAX_PATH]=TEXT("");
//		GetModuleFileName( NULL,szPath,CountArray(szPath));
//
//		CString szTemp;
//		szTemp = szPath;
//		int nPos = szTemp.ReverseFind(TEXT('\\'));
//		_sntprintf(szPath, CountArray(szPath), TEXT("%s"), szTemp.Left(nPos));
//
//		TCHAR szFileNameG[MAX_PATH]=TEXT("");
//		_sntprintf(szFileNameG,sizeof(szFileNameG),TEXT("%s\\GameControl\\global.ini"),szPath);
//
//		TCHAR szGameRoomName[32]=TEXT("");
//		_sntprintf(szGameRoomName, sizeof(szGameRoomName), _T("%d"),m_pGameServiceOption->wServerID);
//
//		int iJiLv = 0;
//		int nIsOpen = false;
//		iJiLv = GetPrivateProfileInt( szGameRoomName, TEXT("WinRate"),0, szFileNameG);
//		nIsOpen = GetPrivateProfileInt( szGameRoomName, TEXT("IsOpen"),0, szFileNameG);
//		if(nIsOpen == false)return;
//		bool bIsWin = false;
//		printf("WinRate = %d\n",iJiLv);
//		printf("nRand = %d\n",nRand);
//		if (nRand <= iJiLv) 
//		{
//			//android win
//			bIsWin = false;
//		}
//		else 
//		{
//			//player win
//			bIsWin = true;
//		}
//
//		//ȫ�ֿ���
//		if (!bIsWin)
//		{
//			//������Ӯ
//			WORD wBeWinAndroid = INVALID_CHAIR;
//
//			//do 
//			//{
//			//	wBeWinAndroid = rand()%GAME_PLAYER;
//			//} while (m_pITableFrame->GetTableUserItem(wBeWinAndroid) == NULL || !m_pITableFrame->GetTableUserItem(wBeWinAndroid)->IsAndroidUser());
//
//
//			for (int i =0;i<GAME_PLAYER;i++)
//			{
//				if(m_pITableFrame->GetTableUserItem(i) == NULL)continue;
//				if (!m_pITableFrame->GetTableUserItem(i)->IsAndroidUser())continue;
//				wBeWinAndroid = i;
//				break;
//			}
//
//			if(wBeWinAndroid != wWinUser)
//			{
//				//��������
//				BYTE cbTempData[MAX_COUNT];
//				CopyMemory(cbTempData, m_cbHandCardData[wBeWinAndroid], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wBeWinAndroid], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
//			}
//			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wBeWinAndroid);
//			if (pIServerUserItem)
//			{
//				printf("Android Win,ID:%d\n",pIServerUserItem->GetGameID());
//			}
//		}
//		else 
//		{
//			//���Ӯ
//			WORD wWinPlayer = INVALID_CHAIR;
//
//			//do 
//			//{
//			//	wWinPlayer = rand()%GAME_PLAYER;
//			//} while (m_pITableFrame->GetTableUserItem(wWinPlayer) == NULL || m_pITableFrame->GetTableUserItem(wWinPlayer)->IsAndroidUser());
//
//
//			for (int i =0;i<GAME_PLAYER;i++)
//			{
//				if(m_pITableFrame->GetTableUserItem(i) == NULL)continue;
//				if (m_pITableFrame->GetTableUserItem(i)->IsAndroidUser())continue;
//				wWinPlayer = i;
//				break;
//			}
//
//			if(wWinPlayer != wWinUser)
//			{
//				//��������
//				BYTE cbTempData[MAX_COUNT];
//				CopyMemory(cbTempData, m_cbHandCardData[wWinPlayer], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wWinPlayer], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
//			}
//			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wWinPlayer);
//			if (pIServerUserItem)
//			{
//				printf("Player Win,ID:%d\n",pIServerUserItem->GetGameID());
//			}
//		}
//
//		//*****************************ȫ�ֿ��ƣ��ػ�������ӮEND********************************************//
//	}
//
//
//	
//
//
//	////����ж�
//	//if (g_lRoomStorageCurrent - m_lCellScore*10*wAiCount < 0)
//	//{
//	//	//���Ӯ��
//	//	WORD wBeWinAndroid = INVALID_CHAIR;
//
//	//	do 
//	//	{
//	//		wBeWinAndroid = rand()%GAME_PLAYER;
//	//	} while (m_pITableFrame->GetTableUserItem(wBeWinAndroid) == NULL || !m_pITableFrame->GetTableUserItem(wBeWinAndroid)->IsAndroidUser());
//
//	//	if(wBeWinAndroid != wWinUser)
//	//	{
//	//		//��������
//	//		BYTE cbTempData[MAX_COUNT];
//	//		CopyMemory(cbTempData, m_cbHandCardData[wBeWinAndroid], sizeof(BYTE)*MAX_COUNT);
//	//		CopyMemory(m_cbHandCardData[wBeWinAndroid], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
//	//		CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
//	//	}
//	//}
//	////�������
//	//else if ((g_lRoomStorageCurrent > g_lStorageMax1Room && rand()%100 <= g_lStorageMul1Room)
//	//      || (g_lRoomStorageCurrent > g_lStorageMax2Room && rand()%100 <= g_lStorageMul2Room))
//	//{
//	//	//���Ӯ��
//	//	WORD wWinPlayer = INVALID_CHAIR;
//
//	//	do 
//	//	{
//	//		wWinPlayer = rand()%GAME_PLAYER;
//	//	} while (m_pITableFrame->GetTableUserItem(wWinPlayer) == NULL || m_pITableFrame->GetTableUserItem(wWinPlayer)->IsAndroidUser());
//
//	//	if(wWinPlayer != wWinUser)
//	//	{
//	//		//��������
//	//		BYTE cbTempData[MAX_COUNT];
//	//		CopyMemory(cbTempData, m_cbHandCardData[wWinPlayer], sizeof(BYTE)*MAX_COUNT);
//	//		CopyMemory(m_cbHandCardData[wWinPlayer], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
//	//		CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
//	//	}
//	//}
//
//	return;
//}
//�˿˷���
void CTableFrameSink::AnalyseStartCard()
{
	//��������
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i=0; i<m_wPlayerCount; i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem!=NULL)
		{
			if(m_cbPlayStatus[i]==FALSE)continue;
			if(pIServerUserItem->IsAndroidUser()) 
			{
				wAiCount++ ;
			}
			wPlayerCount++;
		}
	}

	//ȫ������
	if(wPlayerCount == wAiCount || wAiCount==0) return;


	//�����ļ���
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetModuleFileName( NULL,szPath,CountArray(szPath));

	CString szTemp;
	szTemp = szPath;
	int nPos = szTemp.ReverseFind(TEXT('\\'));
	_sntprintf(szPath, CountArray(szPath), TEXT("%s"), szTemp.Left(nPos));

	TCHAR szFileNameG[MAX_PATH]=TEXT("");
	_sntprintf(szFileNameG,sizeof(szFileNameG),TEXT("%s\\GameControl\\global.ini"),szPath);

	TCHAR szGameRoomName[32]=TEXT("");
	_sntprintf(szGameRoomName, sizeof(szGameRoomName), _T("%d"),m_pGameServiceOption->wServerID);
	int iJiLv = 0;
	int nIsOpen = false;

	iJiLv = GetPrivateProfileInt( szGameRoomName, TEXT("WinRate"),0, szFileNameG);
	nIsOpen = GetPrivateProfileInt( szGameRoomName, TEXT("IsOpen"),0, szFileNameG);
	if(nIsOpen == false)return;

	bool bIsWin = false;
	if (iJiLv >= rand()%100) 
	{
		bIsWin = true;
		//m_bAndroidControl = true;
	}
	else
	{
		bIsWin = false;
		//m_bAndroidControl = false;
	}


	if(bIsWin)
	{
		for (int j = 0;j < GAME_PLAYER;j++)
		{
			IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
			if(pServerUser == NULL)continue;
			if(pServerUser->IsAndroidUser())
				ProbeJudge(j,CHEAT_TYPE_WIN);  //ָ�����ʤ��
		}
	}
	else
	{
		for (int j = 0;j < GAME_PLAYER;j++)
		{
			IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
			if(pServerUser == NULL)continue;
			if(!pServerUser->IsAndroidUser())
				ProbeJudge(j,CHEAT_TYPE_WIN);  //ָ�����ʤ��
		}
	}
	return;
}
//�����¼�
bool CTableFrameSink::OnUserShowCard(WORD wChairID)
{
	//��������
	CMD_S_ShowCard ShowCard;
	ZeroMemory(&ShowCard,sizeof(ShowCard));
	ShowCard.wShowCardUser=wChairID;

	for(WORD i = 0;i<m_wPlayerCount;i++)
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem == NULL ) continue;
		if(i == wChairID)
		{
			CopyMemory(ShowCard.cbCardData,m_cbCardDataForShow[wChairID],sizeof(m_cbCardDataForShow[0]));
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SHOW_CARD,&ShowCard,sizeof(ShowCard));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SHOW_CARD,&ShowCard,sizeof(ShowCard));
			break;
		}
	}

	return true;
}

//�Ƿ�˥��
bool CTableFrameSink::NeedDeductStorage()
{
	//��������
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i=0; i<m_wPlayerCount; i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem!=NULL)
		{
			if(m_cbPlayStatus[i]==FALSE)continue;
			if(pIServerUserItem->IsAndroidUser()) 
			{
				wAiCount++ ;
			}
			wPlayerCount++;
		}
	}

	return wPlayerCount != wAiCount && wAiCount > 0;

}

//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{	
	//��ȡ�Զ�������
	tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomRule);

	g_lRoomStorageStart = pCustomRule->lRoomStorageStart;
	g_lRoomStorageCurrent = pCustomRule->lRoomStorageStart;
	g_lStorageDeductRoom = pCustomRule->lRoomStorageDeduct;
	g_lStorageMax1Room = pCustomRule->lRoomStorageMax1;
	g_lStorageMul1Room = pCustomRule->lRoomStorageMul1;
	g_lStorageMax2Room = pCustomRule->lRoomStorageMax2;
	g_lStorageMul2Room = pCustomRule->lRoomStorageMul2;

	if( g_lStorageDeductRoom < 0 || g_lStorageDeductRoom > 1000 )
		g_lStorageDeductRoom = 0;
	if ( g_lStorageDeductRoom > 1000 )
		g_lStorageDeductRoom = 1000;
	if (g_lStorageMul1Room < 0 || g_lStorageMul1Room > 100) 
		g_lStorageMul1Room = 50;
	if (g_lStorageMul2Room < 0 || g_lStorageMul2Room > 100) 
		g_lStorageMul2Room = 80;

	//��������
	memcpy(m_szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));

	m_nTotalRounds = GetPrivateProfileInt(m_szRoomName, TEXT("Rounds"), 20, m_szConfigFileName);

	m_nZhaDan = GetPrivateProfileInt(m_szRoomName, TEXT("ZhaDan"), 5, m_szConfigFileName);
	m_nShunJin = GetPrivateProfileInt(m_szRoomName, TEXT("ShunJin"), 15, m_szConfigFileName);
	m_nJinHua = GetPrivateProfileInt(m_szRoomName, TEXT("JinHua"), 30, m_szConfigFileName);
	m_nShunZI = GetPrivateProfileInt(m_szRoomName, TEXT("ShunZi"), 50, m_szConfigFileName);
	m_nDuiZi = GetPrivateProfileInt(m_szRoomName, TEXT("DuiZi"), 70, m_szConfigFileName);
	m_nSanPai = GetPrivateProfileInt(m_szRoomName, TEXT("SanPai"), 100, m_szConfigFileName);
}

//���·����û���Ϣ
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction)
{
	//��������
	ROOMUSERINFO roomUserInfo;
	ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

	roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
	CopyMemory(&(roomUserInfo.szNickName), pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
	roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
	roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

	roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

	//�û����º�����
	if (userAction == USER_SITDOWN || userAction == USER_RECONNECT)
	{
		roomUserInfo.wChairID = pIServerUserItem->GetChairID();
		roomUserInfo.wTableID = pIServerUserItem->GetTableID() + 1;
	}
	else if (userAction == USER_STANDUP || userAction == USER_OFFLINE)
	{
		roomUserInfo.wChairID = INVALID_CHAIR;
		roomUserInfo.wTableID = INVALID_TABLE;
	}

	g_MapRoomUserInfo.SetAt(pIServerUserItem->GetUserID(), roomUserInfo);

	//����ӳ�䣬ɾ���뿪�������ң�
	POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
	DWORD dwUserID = 0;
	ROOMUSERINFO userinfo;
	ZeroMemory(&userinfo, sizeof(userinfo));
	TCHAR szNickName[LEN_NICKNAME];
	ZeroMemory(szNickName, sizeof(szNickName));
	DWORD *pdwRemoveKey	= new DWORD[g_MapRoomUserInfo.GetSize()];
	ZeroMemory(pdwRemoveKey, sizeof(DWORD) * g_MapRoomUserInfo.GetSize());
	WORD wRemoveKeyIndex = 0;

	while(ptHead)
	{
		g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

		if (userinfo.dwGameID == 0 && (_tcscmp(szNickName, userinfo.szNickName) == 0) && userinfo.cbUserStatus == 0 )
		{
			pdwRemoveKey[wRemoveKeyIndex++] = dwUserID;
		}

	}

	for (WORD i=0; i<wRemoveKeyIndex; i++)
	{
		g_MapRoomUserInfo.RemoveKey(pdwRemoveKey[i]);

		CString strtip;
		strtip.Format(TEXT("RemoveKey,wRemoveKeyIndex = %d, g_MapRoomUserInfosize = %d \n"), wRemoveKeyIndex, g_MapRoomUserInfo.GetSize());
		
		CString strFileName;
		strFileName.Format(TEXT("թ��[%s]�����˻���־.log"), m_pGameServiceOption->szServerName);
		WriteInfo(strFileName, strtip);
	}

	delete[] pdwRemoveKey;
}

//����ͬ���û�����
void CTableFrameSink::UpdateUserControl(IServerUserItem *pIServerUserItem)
{
	//��������
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERCONTROL roomusercontrol;

	//��ʼ��
	ptListHead = g_ListRoomUserControl.GetHeadPosition();
	ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

	//��������
	while(ptListHead)
	{
		ptTemp = ptListHead;
		roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

		//Ѱ���Ѵ��ڿ������
		if ((pIServerUserItem->GetGameID() == roomusercontrol.roomUserInfo.dwGameID) &&
			_tcscmp(pIServerUserItem->GetNickName(), roomusercontrol.roomUserInfo.szNickName) == 0)
		{
			//��ȡԪ��
			ROOMUSERCONTROL &tmproomusercontrol = g_ListRoomUserControl.GetAt(ptTemp);

			//�������
			tmproomusercontrol.roomUserInfo.wChairID = pIServerUserItem->GetChairID();
			tmproomusercontrol.roomUserInfo.wTableID = m_pITableFrame->GetTableID() + 1;

			return;
		}
	}
}

//�����û�����
void CTableFrameSink::TravelControlList(ROOMUSERCONTROL keyroomusercontrol)
{
	//��������
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERCONTROL roomusercontrol;

	//��ʼ��
	ptListHead = g_ListRoomUserControl.GetHeadPosition();
	ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

	//��������
	while(ptListHead)
	{
		ptTemp = ptListHead;
		roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

		//Ѱ���Ѵ��ڿ����������һ�������л�����
		if ((keyroomusercontrol.roomUserInfo.dwGameID == roomusercontrol.roomUserInfo.dwGameID) &&
			_tcscmp(keyroomusercontrol.roomUserInfo.szNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
		{
			g_ListRoomUserControl.RemoveAt(ptTemp);
		}
	}
}

//�Ƿ������������
void CTableFrameSink::IsSatisfyControl(ROOMUSERINFO &userInfo, bool &bEnableControl)
{
	if (userInfo.wChairID == INVALID_CHAIR || userInfo.wTableID == INVALID_TABLE)
	{
		bEnableControl = FALSE;
		return;
	}

	if (userInfo.cbUserStatus == US_SIT || userInfo.cbUserStatus == US_READY || userInfo.cbUserStatus == US_PLAYING)
	{
		bEnableControl = TRUE;
		return;
	}
	else
	{
		bEnableControl = FALSE;
		return;
	}
}

//���������û�����
bool CTableFrameSink::AnalyseRoomUserControl(ROOMUSERCONTROL &Keyroomusercontrol, POSITION &ptList)
{
	//��������
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERCONTROL roomusercontrol;

	//��������
	for (WORD i=0; i<m_wPlayerCount; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}

		//��ʼ��
		ptListHead = g_ListRoomUserControl.GetHeadPosition();
		ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

		//��������
		while(ptListHead)
		{
			ptTemp = ptListHead;
			roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

			//Ѱ�����
			if ((pIServerUserItem->GetGameID() == roomusercontrol.roomUserInfo.dwGameID) &&
				_tcscmp(pIServerUserItem->GetNickName(), roomusercontrol.roomUserInfo.szNickName) == 0)
			{
				//��տ��ƾ���Ϊ0��Ԫ��
				if (roomusercontrol.userControl.cbControlCount == 0)
				{
					g_ListRoomUserControl.RemoveAt(ptTemp);
					break;
				}

				if (roomusercontrol.userControl.control_type == CONTINUE_CANCEL)
				{
					g_ListRoomUserControl.RemoveAt(ptTemp);
					break;
				}

				//��������
				CopyMemory(&Keyroomusercontrol, &roomusercontrol, sizeof(roomusercontrol));
				ptList = ptTemp;

				return true;
			}

		}

	}

	return false;
}

void CTableFrameSink::GetControlTypeString(CONTROL_TYPE &controlType, CString &controlTypestr)
{
	switch(controlType)
	{
	case CONTINUE_WIN:
		{
			controlTypestr = TEXT("��������Ϊ��Ӯ");
			break;
		}
	case CONTINUE_LOST:
		{
			controlTypestr = TEXT("��������Ϊ����");
			break;
		}
	case CONTINUE_CANCEL:
		{
			controlTypestr = TEXT("��������Ϊȡ������");
			break;
		}
	}
}

//д��־�ļ�
void CTableFrameSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
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

//�޸Ŀ��
bool CTableFrameSink::UpdateStorage(SCORE lScore)
{
	if(!m_bUpdataStorage) return true;

	g_lRoomStorageCurrent += lScore;

	//���Ϊ��
	if (g_lRoomStorageCurrent < 0)
	{
		CString str;
		CString strTemp;
		strTemp.Format(TEXT("%d�� �����ע%I64d ���Ϊ��%I64d ��Ӯ%I64d"), m_pITableFrame->GetTableID() + 1, m_lMaxCellScore, g_lRoomStorageCurrent, lScore);
		str += strTemp;
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_lUserMaxScore[i] > 0)
			{
				strTemp.Format(TEXT("��%d�����������ע%I64d��"), i, m_lUserMaxScore[i]);
				str += strTemp;
				//��ȡ�û�
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem!=NULL)
				{
					strTemp.Format(TEXT("UserID=%ld��������%d������%I64d����עΪ%I64d������%d������%d���˿ˣ�"), pIServerUserItem->GetUserID(), pIServerUserItem->IsAndroidUser(), pIServerUserItem->GetUserScore(), m_lTableScore[i], m_bMingZhu[i], m_cbGiveUpUser[i]);
					str += strTemp;
				}
				else
				{
					strTemp.Format(TEXT("�Ѿ��뿪���ӣ���עΪ%I64d ����%d������%d���˿ˣ�"), m_lTableScore[i], m_bMingZhu[i], m_cbGiveUpUser[i]);
					str += strTemp;
				}

				for (int j = 0; j < MAX_COUNT; j++)
				{
					strTemp.Format(TEXT("%02x "), m_cbHandCardData[i][j]);
					str += strTemp;
				}
			}
		}

		CString strFileName;
		strFileName.Format(TEXT("թ��[%s]�����־.log"), m_pGameServiceOption->szServerName);
		//WriteInfo(strFileName, str);
		return false;
	}
	return true;
}

//������Կ
void CTableFrameSink::RandAESKey(unsigned char chAESKeyBuffer[], BYTE cbBufferCount)
{
	for (WORD i=0; i<cbBufferCount; i++)
	{
		chAESKeyBuffer[i] = rand() % 256;
	}
}

//�жϻ���Լռ����
bool CTableFrameSink::IsRoomCardScoreType()
{
	return (m_pITableFrame->GetDataBaseMode() == 1) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0);
}

//�жϽ��Լռ����
bool CTableFrameSink::IsRoomCardTreasureType()
{
	return (m_pITableFrame->GetDataBaseMode() == 0) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0);
}

//////////////////////////////////////////////////////////////////////////
