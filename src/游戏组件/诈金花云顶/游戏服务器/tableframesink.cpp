#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <conio.h>
#include <locale>

//////////////////////////////////////////////////////////////////////////

//公共加密原文
unsigned char chCommonEncryption[AESENCRYPTION_LENGTH] = 
{
	0x32, 0x43, 0xf6, 0xa8, 
	0x88, 0x5a, 0x30, 0x8d, 
	0x31, 0x31, 0x98, 0xa2, 
	0xe0, 0x37, 0x07, 0x34
};

//静态变量
const BYTE			CTableFrameSink::m_GameStartMode=START_MODE_ALL_READY;	//开始模式

//房间玩家信息
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo;	//玩家USERID映射玩家信息
//房间用户控制
CList<ROOMUSERCONTROL, ROOMUSERCONTROL&> g_ListRoomUserControl;		//房间用户控制链表
//操作控制记录
CList<CString, CString&> g_ListOperationRecord;						//操作控制记录

ROOMUSERINFO	g_CurrentQueryUserInfo;								//当前查询用户信息

//全局变量
SCORE						g_lRoomStorageStart = 0LL;								//房间起始库存
SCORE						g_lRoomStorageCurrent = 0LL;							//总输赢分
SCORE						g_lStorageDeductRoom = 0LL;								//回扣变量
SCORE						g_lStorageMax1Room = 0LL;								//库存封顶
SCORE						g_lStorageMul1Room = 0LL;								//系统输钱比例
SCORE						g_lStorageMax2Room = 0LL;								//库存封顶
SCORE						g_lStorageMul2Room = 0LL;								//系统输钱比例


//定时器 0~30
#define IDI_GAME_COMPAREEND					1									//结束定时器
#define IDI_GAME_OPENEND					2									//结束定时器
#define IDI_ADD_SCORE						3									//结束定时器

#define TIME_GAME_COMPAREEND				5000								//结束定时器
#define TIME_GAME_OPENEND					5000								//结束定时器
#define TIME_ADD_SCORE						40000								//结束定时器

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	AllocConsole();
	freopen("CON", "wt", stdout);

	int a = sizeof(CMD_C_AddScore);

	//游戏变量
	m_wPlayerCount = GAME_PLAYER;
	m_bOperaCount=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount=0;
	m_bGameEnd=false;
	ZeroMemory(m_wFlashUser,sizeof(m_wFlashUser));

	//用户状态
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

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	m_lMaxCellScore=0L;
	m_lCellScore=0L;
	m_lCurrentTimes=1L;		
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	//清空链表
	g_ListRoomUserControl.RemoveAll();
	g_ListOperationRecord.RemoveAll();
	ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

	//服务控制
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

	//随机种子
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

//析构函数
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

//释放对象
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

//接口查询--检测相关信息版本
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//读取配置
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

//复位桌子
void  CTableFrameSink::RepositionSink()
{
	//游戏变量
	m_bOperaCount=0;
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount=0;
	ZeroMemory(m_wFlashUser,sizeof(m_wFlashUser));

	//用户状态
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

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	m_lMaxCellScore=0L;		
	m_lCellScore=0L;						
	m_lCurrentTimes=1L;		
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));
	m_bAndroidControl = false;
	return;
}

//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//房卡金币房间,写分变化
	if (IsRoomCardTreasureType() && cbReason == SCORE_REASON_WRITE && !pIServerUserItem->IsAndroidUser())
	{
		SCORE lRoomCardTurnMaxScore = 0LL;
		
		//设置文件名
		TCHAR szPath[MAX_PATH] = TEXT("");
		TCHAR szFileName[MAX_PATH] = TEXT("");
		GetCurrentDirectory(sizeof(szPath),szPath);
		_sntprintf(szFileName,sizeof(szFileName),TEXT("%s\\ZaJinHuaRoomCard.ini"),szPath);

		//读取配置
		lRoomCardTurnMaxScore = GetPrivateProfileInt(m_pGameServiceOption->szServerName, TEXT("lTurnMaxScore"), 10000, szFileName);

		if (pIServerUserItem->GetUserScore() < lRoomCardTurnMaxScore)
		{
			m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_RC_TREASEURE_DEFICIENCY);
		}
	}

	return false;
}

//开始模式
BYTE  CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//游戏状态
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID<m_wPlayerCount);
	return (m_cbPlayStatus[wChairID]==TRUE)?true:false;
}

//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//更新房间用户信息
	UpdateRoomUserInfo(pIServerUserItem, USER_OFFLINE);

	return true;
}

//游戏开始
bool  CTableFrameSink::OnEventGameStart()
{

	//读取配置
	ReadConfigInformation();
	m_RoundsMap.clear();
	m_nCurrentRounds = 1;
	m_nTotalRounds = 0;

	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_RoundsMap[i] = 0;
	}


	//删除定时器
	m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);

	//设置状态
	m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);

	//私人房间
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL) !=0 )
	{
		//cbGameRule[1] 为  2 、3 、4 、5, 0分别对应 2人 、 3人 、 4人 、 5人 、 2-5人 这几种配置
		BYTE *pGameRule = m_pITableFrame->GetGameRule();
		if (pGameRule[1] != 0)
		{
			m_wPlayerCount = pGameRule[1];
			//设置人数
			m_pITableFrame->SetTableChairCount(m_wPlayerCount);
		}
		else
		{
			m_wPlayerCount = GAME_PLAYER;
			//设置人数
			m_pITableFrame->SetTableChairCount(GAME_PLAYER);
		}
	}

	//更新房间用户信息
	for (WORD i=0; i<m_wPlayerCount; i++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL)
		{
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
		}
	}

	m_bGameEnd=false;

	//最大下注
	SCORE lTimes=6L;
	BYTE cbAiCount = 0;
	BYTE cbPlayCount = 0;
	
	//判断约占积分场
	SCORE lRoomCardTurnMaxScore = 0;
	SCORE lRoomCardMaxCellScore = 0;
	if (IsRoomCardScoreType())
	{
		//设置文件名
		TCHAR szPath[MAX_PATH] = TEXT("");
		TCHAR szFileName[MAX_PATH] = TEXT("");
		GetCurrentDirectory(sizeof(szPath),szPath);
		_sntprintf(szFileName,sizeof(szFileName),TEXT("%s\\ZaJinHuaRoomCard.ini"),szPath);

		//读取配置
		lRoomCardMaxCellScore = GetPrivateProfileInt(m_pGameServiceOption->szServerName, TEXT("lMaxCellScore"), 1000, szFileName);
		lRoomCardTurnMaxScore = GetPrivateProfileInt(m_pGameServiceOption->szServerName, TEXT("lTurnMaxScore"), 10000, szFileName);
	}

	if (IsRoomCardTreasureType())
	{
		//设置文件名
		TCHAR szPath[MAX_PATH] = TEXT("");
		TCHAR szFileName[MAX_PATH] = TEXT("");
		GetCurrentDirectory(sizeof(szPath),szPath);
		_sntprintf(szFileName,sizeof(szFileName),TEXT("%s\\ZaJinHuaRoomCard.ini"),szPath);

		//读取配置
		lRoomCardTurnMaxScore = GetPrivateProfileInt(m_pGameServiceOption->szServerName, TEXT("lTurnMaxScore"), 10000, szFileName);
	}
	
	m_MapControlID.RemoveAll();
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//获取用户
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//加密赋值
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

		//设置变量
		m_cbPlayStatus[i]=TRUE;

		if (!pIServerUserItem->IsAndroidUser())
		{
			//添加需要控制的玩家
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

		//更新房间用户信息 
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

		//判断单注
		SCORE Temp=lTimes;
		if(m_lUserMaxScore[i]<10001)Temp=1L;
		else if(m_lUserMaxScore[i]<100001)Temp=2L;
		else if(m_lUserMaxScore[i]<1000001)Temp=3L;
		else if(m_lUserMaxScore[i]<10000001)Temp=4L;
		else if(m_lUserMaxScore[i]<100000001)Temp=5L;
		if(lTimes>Temp)lTimes=Temp;
	}

	//库存
	if(g_lRoomStorageCurrent>0 && NeedDeductStorage()) g_lRoomStorageCurrent = g_lRoomStorageCurrent-g_lRoomStorageCurrent*g_lStorageDeductRoom/1000;

	//发送库存消息
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if ( pIServerUserItem == NULL )
			continue;

		if( CUserRight::IsGameCheatUser(m_pITableFrame->GetTableUserItem(i)->GetUserRight()))
		{
			CString strInfo;
			strInfo.Format(TEXT("当前库存：%I64d"), g_lRoomStorageCurrent);

			m_pITableFrame->SendGameMessage(pIServerUserItem,strInfo,SMT_CHAT);
		}	
	}

	//写如库存日志

	CTime time = CTime::GetCurrentTime();
	CString strInfo;
	strInfo.Format(TEXT(" %d/%d/%d-%d:%d:%d-当前房间库存：%I64d\n"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(),
		g_lRoomStorageCurrent);

	CString strFileName;
	strFileName.Format(TEXT("诈金花[%s]库存日志.log"), m_pGameServiceOption->szServerName);
	//WriteInfo(strFileName, strInfo);

	//库存变量
	m_bUpdataStorage = (cbAiCount != cbPlayCount);
	//下注变量
	//m_lCellScore = 1;
	m_lCellScore=m_pGameServiceOption->lCellScore;
	//while((lTimes--)>0) m_lCellScore*=10;

	//基础分数
	for(BYTE i=0;i<m_wPlayerCount;i++)
	{
		if(m_lUserMaxScore[i] != 0)
			m_lCellScore=__min(m_lUserMaxScore[i],m_lCellScore);
	}

	m_lCurrentTimes=1;
	m_lMaxCellScore=m_lCellScore*50;

	if (!IsRoomCardScoreType() && !IsRoomCardTreasureType())
	{
		//最大下注
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

	//分发扑克
	//BYTE bTempArray[52];
	ZeroMemory(m_bTempArray,sizeof(m_bTempArray));

	//m_GameLogic.RandCardList(bTempArray,m_cbHandCardData[0],sizeof(m_cbHandCardData)/sizeof(m_cbHandCardData[0][0]));
	//m_GameLogic.RandCardList(m_bTempArray,m_cbHandCardData[0],52);
	m_GameLogic.RandCardList(m_bTempArray,sizeof(m_bTempArray));

	//排列扑克
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

		//派发扑克
		CopyMemory(m_cbHandCardData[i],&m_bTempArray[wIndex*MAX_COUNT],MAX_COUNT);
		m_GameLogic.SortCardList(m_cbHandCardData[i], MAX_COUNT);

		CopyMemory(AllCard.cbCardData[i],&m_bTempArray[wIndex*MAX_COUNT],MAX_COUNT);
		m_GameLogic.SortCardList(AllCard.cbCardData[i], MAX_COUNT);
		AllCard.bAICount[i] = pIServerUser->IsAndroidUser();
		wIndex++;
	}

	ZeroMemory(m_cbCardDataForShow,sizeof(m_cbCardDataForShow));

	//游戏开始
	AnalyseStartCard();

#ifdef _DEBUG
	m_cbHandCardData[0][0] = 0x33;
	m_cbHandCardData[0][1] = 0x23;
	m_cbHandCardData[0][2] = 0x13;
#endif
	//变量定义
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
			//	ProbeJudge(j,CHEAT_TYPE_WIN);  //指定玩家胜利
			//}
			//else
			//{
			//	DWORD dwWinChairID = ProbeJudge(j,CHEAT_TYPE_LOST);  //指定玩家失败
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
	//机器人消息
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//机器人数据
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem == NULL) continue;
		if (pServerUserItem->IsAndroidUser() == true)
		{
			//机器人数据
			CMD_S_AndroidCard AndroidCard ;
			ZeroMemory(&AndroidCard, sizeof(AndroidCard)) ;

			//设置变量
			AndroidCard.lStockScore = g_lRoomStorageCurrent;
			AndroidCard.bAndroidControl = m_bAndroidControl;
			CopyMemory(AndroidCard.cbRealPlayer, m_cbRealPlayer, sizeof(m_cbRealPlayer));
			CopyMemory(AndroidCard.cbAndroidStatus, m_cbAndroidStatus, sizeof(m_cbAndroidStatus));
			CopyMemory(AndroidCard.cbAllHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
			m_pITableFrame->SendTableData(i, SUB_S_ANDROID_CARD, &AndroidCard, sizeof(AndroidCard));
		}
	}


	//设置庄家
	if(m_wBankerUser==INVALID_CHAIR) m_wBankerUser=rand()%m_wPlayerCount;

	//庄家离开
	if(m_wBankerUser<m_wPlayerCount && m_cbPlayStatus[m_wBankerUser]==FALSE) m_wBankerUser=rand()%m_wPlayerCount;

	//确定庄家
	while(m_cbPlayStatus[m_wBankerUser]==FALSE)
	{
		m_wBankerUser = (m_wBankerUser+1)%m_wPlayerCount;
	}

	//用户设置
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			m_lTableScore[i]=m_lCellScore;
		}
	}

	//当前用户
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
		strFileName.Format(TEXT("诈金花机器人IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnEventGameStart  m_wCurrentUser==INVALID_CHAIR [%d-%d-%d %d:%d:%d]\n"), m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}

	//构造数据
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


	//房间名称
	memcpy(m_szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));
	m_nTotalRounds = GetPrivateProfileInt(m_szRoomName, TEXT("Rounds"), 20, m_szConfigFileName);
	GameStart.nTotalRounds = m_nTotalRounds;

	m_dElapse = GetTickCount();

	//发送数据
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i] == TRUE)
		{
			GameStart.lUserMaxScore=m_lUserMaxScore[i];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}

	//服务控制
	if (m_pServerControl)
	{
		//服务信息
		BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT];
		CopyMemory(cbHandCardData, m_cbHandCardData, sizeof(cbHandCardData) );

		//排列扑克
		for (WORD i= 0;i < GAME_PLAYER; i++)
			m_GameLogic.SortCardList(cbHandCardData[i], MAX_COUNT);

		//信息结算
		m_pServerControl->ServerControl(cbHandCardData, m_pITableFrame, m_wPlayerCount);
	}

	//设置定时器 非房卡房间
	if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) == 0)
	{
		m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE,1,0);
	}

	//超控发送数据
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
//试探性判断, 确保Chair的用户可以赢
bool CTableFrameSink::GetControlCard(BYTE cbControlCardByte[],BYTE cbCardCount)
{
	BYTE cbCardListData[52]=
	{
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//黑桃 A - K
	};

	int nControlChair[GAME_PLAYER] = {-1};
	int nAndroidCount[GAME_PLAYER] = {-1};
	//单控人数
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
				//全局下注
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
					//全局下注
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
				//全局下注
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
					//全局下注
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
				//全局下注
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
					//全局下注
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
				//全局下注
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
					//全局下注
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
				//全局下注
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
					//全局下注
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
//随机牌
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
			//全局下注
			CopyMemory(m_cbHandCardData[dwChairID],cbCard,sizeof(m_cbHandCardData[dwChairID]));
			m_GameLogic.RemoveGoodCardData(cbCard,3,cbRandCard,52);
			cbIndex++;
			break;
		}
	}while(true);

	return cbCardTybe;
}
bool CTableFrameSink::getSingBomb(BYTE cbCardData[52],int chairID,BYTE cbSingBomb[] )  //一个炸弹 
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
	//删除好牌数据
	m_GameLogic.RemoveGoodCardData(GoodCardData,3,cbCardData, 52 );

	return true;
}
bool CTableFrameSink::getSingSameStraight(BYTE cbCardData[52],int chairID,BYTE SingSameStraight[])  //一个同花顺
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
	//删除好牌数据
	m_GameLogic.RemoveGoodCardData(GoodCardData,3,cbCardData, 52 );

	return true;
}
//试探性判断, 确保Chair的用户可以赢
DWORD CTableFrameSink::ProbeJudge(WORD wChairIDCtrl,BYTE cbCheatType)
{	
	//获取用户
	IServerUserItem * pIServerCtrlUserItem=m_pITableFrame->GetTableUserItem(wChairIDCtrl);
	//待用户不存在, 不需要控制

	if(pIServerCtrlUserItem==NULL) return INVALID_CHAIR;

	//扑克变量
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));
	//排列扑克
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
	//iMaxIndex = EstimateWinner(0,4); //获取最大用户
	//iLostIndex = EstimateLose(0,4);	//获取最小用户

	//临时变量
	BYTE cbTemp[MAX_COUNT]={0};
	//交换牌
	WORD wWinerUser=iMaxIndex;     //赢

	if(cbCheatType==CHEAT_TYPE_LOST) wWinerUser=iLostIndex;   //输

	if (wWinerUser!=wChairIDCtrl) //如果当前用户的牌不是要换的牌的用户就开始换牌
	{
		//交牌扑克
		CopyMemory(cbTemp,m_cbHandCardData[wWinerUser],MAX_COUNT);   //把指定用户的牌放到变量
		CopyMemory(m_cbHandCardData[wWinerUser],m_cbHandCardData[wChairIDCtrl],MAX_COUNT); //把控制用户的牌与指定用户的牌交换
		CopyMemory(m_cbHandCardData[wChairIDCtrl],cbTemp,MAX_COUNT);  //最后交换
	}
	return EstimateWinner();
}
//推断胜者
WORD CTableFrameSink::EstimateWinner()
{
	//保存扑克
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(cbUserCardData));

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
//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_COMPARECARD:	//比牌结束
	case GER_NO_PLAYER:		//没有玩家
		{
			if(m_bGameEnd)return true;
			m_bGameEnd=true;

			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			for (WORD i=0;i<m_wPlayerCount;i++)
				for (WORD j=0;j<(GAME_PLAYER - 1);j++)
					GameEnd.wCompareUser[i][j]=INVALID_CHAIR;

			//唯一玩家
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

			//胜利者强退
			if (wUserCount==0)
			{
				wWinner=m_wBankerUser;
			}

			//延迟开始
			bool bDelayOverGame = false;

			//计算总注
			SCORE lWinnerScore=0L;
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				if(i==wWinner)continue;
				GameEnd.lGameScore[i]=-m_lTableScore[i];
				lWinnerScore+=m_lTableScore[i];
			}

			//处理税收
			GameEnd.lGameTax=m_pITableFrame->CalculateRevenue(wWinner,lWinnerScore);
			GameEnd.lGameScore[wWinner]=lWinnerScore-GameEnd.lGameTax;

			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			CopyMemory(m_cbCardDataForShow,m_cbHandCardData,sizeof(m_cbCardDataForShow));

			//扑克数据
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

			//胜利者强退
			if (wUserCount==1)
			{
				//修改积分
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

			//库存统计
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//获取用户
				IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserIte==NULL) continue;
				if(!m_cbPlayStatus[i]) continue;

				//库存累计
				if(!pIServerUserIte->IsAndroidUser())
					UpdateStorage(-GameEnd.lGameScore[i]);//g_lRoomStorageCurrent -= GameEnd.lGameScore[i];

			}
			//房间类型
			GameEnd.wServerType = m_pGameServiceOption->wServerType;
			//延迟开始
			if(bDelayOverGame)
			{
				GameEnd.bDelayOverGame = true;
			}

			if(wChairID==GAME_PLAYER)
			{
				//结束数据
				CopyMemory(&m_StGameEnd,&GameEnd,sizeof(m_StGameEnd));
				m_pITableFrame->SetGameTimer(IDI_GAME_COMPAREEND,TIME_GAME_COMPAREEND,1,0);
			}
			else
			{
				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				printf("发送游戏结束消息11111\n");
				//检查机器人存储款
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//获取用户
					IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
					if(!pIServerUserItem)
						continue;
					if(!pIServerUserItem->IsAndroidUser())
						continue;
					m_pITableFrame->SendTableData(i,SUB_S_ANDROID_BANKOPERATOR);
				}

				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}

			//更新房间用户信息
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//获取用户
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (!pIServerUserItem)
				{
					continue;
				}

				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}

			//发送库存
			CString strInfo;
			strInfo.Format(TEXT("当前库存：%I64d"), g_lRoomStorageCurrent);
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
	case GER_USER_LEAVE:		//用户强退
	case GER_NETWORK_ERROR:		//网络中断
		{
			if(m_bGameEnd || m_cbPlayStatus[wChairID]==FALSE) return true;

			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount);

			//CString str;
			//str.Format(TEXT("【%d桌】 %d退出放弃"), m_pITableFrame->GetTableID() + 1, m_wCurrentUser);
			//NcaTextOut(str, m_pGameServiceOption->szServerName);

			//强退处理
			return OnUserGiveUp(wChairID,true);
		}
	case GER_OPENCARD:		//开牌结束   
		{
			if(m_bGameEnd)return true;
			m_bGameEnd = true;

			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			for (WORD i=0;i<m_wPlayerCount;i++)
				for (WORD j=0;j<(GAME_PLAYER - 1);j++)
					GameEnd.wCompareUser[i][j]=INVALID_CHAIR;

			//胜利玩家
			WORD wWinner=m_wBankerUser;

			//计算分数
			SCORE lWinnerScore=0L;
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				if(i==wWinner)continue;
				lWinnerScore+=m_lTableScore[i];
				GameEnd.lGameScore[i]=-m_lTableScore[i];
			}

			//处理税收
			GameEnd.lGameTax=m_pITableFrame->CalculateRevenue(wWinner,lWinnerScore);
			GameEnd.lGameScore[wWinner]=lWinnerScore-GameEnd.lGameTax;

			//开牌结束
			GameEnd.wEndState=1;
			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			CopyMemory(m_cbCardDataForShow,m_cbHandCardData,sizeof(m_cbCardDataForShow));

			//扑克数据
			for (WORD i=0;i<m_wPlayerCount;i++) 
			{
				WORD wCount=0;
				while(m_wCompardUser[i].GetCount()>0)
				{
					GameEnd.wCompareUser[i][wCount++] = m_wCompardUser[i].GetAt(0);
					m_wCompardUser[i].RemoveAt(0);
				}
			}

			//修改积分
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

			//库存统计
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//获取用户
				IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserIte==NULL) continue;
				if(!m_cbPlayStatus[i]) continue;

				//库存累计
				if(!pIServerUserIte->IsAndroidUser())
					UpdateStorage(-GameEnd.lGameScore[i]);//g_lRoomStorageCurrent -= GameEnd.lGameScore[i];

			}

			//房间类型
			GameEnd.wServerType = m_pGameServiceOption->wServerType;

			if(wChairID==GAME_PLAYER)
			{
				//结束数据
				CopyMemory(&m_StGameEnd,&GameEnd,sizeof(m_StGameEnd));
				m_pITableFrame->SetGameTimer(IDI_GAME_OPENEND,TIME_GAME_OPENEND,1,0);
			}
			else
			{
				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				printf("发送游戏结束消息222222\n");
				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}
			
			//更新房间用户信息
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//获取用户
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

				if (!pIServerUserItem)
				{
					continue;
				}

				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}

			//发送库存
			CString strInfo;
			strInfo.Format(TEXT("当前库存：%I64d"), g_lRoomStorageCurrent);
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
	case GER_DISMISS:		//游戏解散
		{
			//变量定义
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			printf("发送游戏结束消息444444\n");
			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//更新房间用户信息
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				//获取用户
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

//发送场景
bool  CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore=0L;
			//StatusFree.lRoomStorageStart = g_lRoomStorageStart;
			//StatusFree.lRoomStorageCurrent = g_lRoomStorageCurrent;

			//获取自定义配置
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

			//权限判断
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

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_STATUS_PLAY:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//加注信息
			StatusPlay.lMaxCellScore=m_lMaxCellScore;
			StatusPlay.lCellScore=m_lCellScore;
			StatusPlay.lCurrentTimes=m_lCurrentTimes;
			StatusPlay.lUserMaxScore=m_lUserMaxScore[wChairID];

			//设置变量
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			CopyMemory(StatusPlay.bMingZhu,m_bMingZhu,sizeof(m_bMingZhu));
			CopyMemory(StatusPlay.lTableScore,m_lTableScore,sizeof(m_lTableScore));
			CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(m_cbPlayStatus));
			//StatusPlay.lRoomStorageStart = g_lRoomStorageStart;
			//StatusPlay.lRoomStorageCurrent = g_lRoomStorageCurrent;

			//获取自定义配置
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

			//当前状态
			StatusPlay.bCompareState=(m_lCompareCount<=0)?false:true;

			//设置扑克
			if(m_bMingZhu[wChairID])
				CopyMemory(&StatusPlay.cbHandCardData,&m_cbHandCardData[wChairID],MAX_COUNT);
			
			//更新房间用户信息
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);
			
			if (pIServerUserItem->GetUserStatus() != US_LOOKON)
			{
				CopyMemory(StatusPlay.chUserAESKey, m_chUserAESKey[wChairID], sizeof(m_chUserAESKey[wChairID]));
			}
			
			CopyMemory(StatusPlay.szServerName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szServerName));

			//第几轮 总轮数
            StatusPlay.nCurrentRounds = m_nCurrentRounds;
			StatusPlay.nTotalRounds = m_nTotalRounds;

			StatusPlay.dElapse = TIME_ADD_SCORE - (GetTickCount() - m_dElapse);
			if (TIME_ADD_SCORE - (GetTickCount() - m_dElapse) <= 0 || (GetTickCount() - m_dElapse <= 0))
			{
				StatusPlay.dElapse = 1;
			}
			//m_dElapse = GetTickCount();


			//权限判断
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

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	//效验错误
	ASSERT(FALSE);

	return false;
}

//定时器事件 
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	//结束定时
	if(wTimerID==IDI_GAME_COMPAREEND || IDI_GAME_OPENEND==wTimerID)
	{
		//删除时间
		if(wTimerID==IDI_GAME_COMPAREEND)m_pITableFrame->KillGameTimer(IDI_GAME_COMPAREEND);
		else m_pITableFrame->KillGameTimer(IDI_GAME_OPENEND);

		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
		printf("发送游戏结束消息666666\n");
		//检查机器人存储款
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//获取用户
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			if(!pIServerUserItem)
				continue;
			if(!pIServerUserItem->IsAndroidUser())
				continue;
			m_pITableFrame->SendTableData(i,SUB_S_ANDROID_BANKOPERATOR);
		}

		//结束游戏
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

		//更新房间用户信息
		for (WORD i=0; i<m_wPlayerCount; i++)
		{
			//获取用户
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
			printf("系统自动超时操作m_wCurrentUser_%d\n",m_wCurrentUser);
			if (wBindParam == 2)
			{
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentUser);
				if (pIServerUserItem && pIServerUserItem->GetUserStatus() == US_OFFLINE  || pIServerUserItem && pIServerUserItem->IsAndroidUser() == true)
				{

					//输出信息
					TCHAR szBuffer[128]=TEXT("");
					_sntprintf(szBuffer,CountArray(szBuffer),TEXT("机器人自动弃牌"));
					//输出信息
					CTraceService::TraceString(szBuffer,TraceLevel_Normal);

					m_lCompareCount=0;
					OnUserGiveUp(m_wCurrentUser,false);
				}
				else
				{
					//自动比牌
					OnUserAutoCompareCard(m_wCurrentUser);
				}
			} 
			else
			{
				OnUserGiveUp(m_wCurrentUser,false);
			}

			//CString str;
			//str.Format(TEXT("【%d桌】 %d超时放弃"), m_pITableFrame->GetTableID() + 1, m_wCurrentUser);
			//NcaTextOut(str, m_pGameServiceOption->szServerName);
		}		
	}

	return false;
}

//游戏消息处理
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_GIVE_UP:			//用户放弃
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserGiveUp(pUserData->wChairID);
		}
	case SUB_C_SHOW_CARD:			//亮牌操作
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus==US_READY) return true;


			//消息处理
			return OnUserShowCard(pUserData->wChairID);
		}
	case SUB_C_LOOK_CARD:		//用户看牌
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//消息处理
			return OnUserLookCard(pUserData->wChairID);
		}
	case SUB_C_COMPARE_CARD:	//用户比牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_CompareCard));
			if (wDataSize!=sizeof(CMD_C_CompareCard)) return false;

			//变量定义
			CMD_C_CompareCard * pCompareCard=(CMD_C_CompareCard *)pData;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//参数效验
			ASSERT(pUserData->wChairID==m_wCurrentUser);
			if(pUserData->wChairID!=m_wCurrentUser)
			{
				//for (int i =0;i<GAME_PLAYER;i++)
				//{
				//	IServerUserItem * pUserItem = m_pITableFrame->GetTableUserItem(i);
				//	if (pUserItem)
				//	{
				//		//输出信息
				//		TCHAR szBuffer[128]=TEXT("");
				//		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("SUB_C_COMPARE_CARD 玩家ID [ %d]"),pUserItem->GetGameID());
				//		//输出信息
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

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE && m_cbPlayStatus[pCompareCard->wCompareUser]==TRUE);
			if(m_cbPlayStatus[pUserData->wChairID]==FALSE || m_cbPlayStatus[pCompareCard->wCompareUser]==FALSE)return false;

			//消息处理
			return OnUserCompareCard(pUserData->wChairID,pCompareCard->wCompareUser);
		}
	case SUB_C_ADD_SCORE:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) 
			{
				return false;
			}

			//变量定义
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pData;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE && m_lCompareCount==0);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE || m_lCompareCount> 0) 
			{
				//输出信息
				TCHAR szBuffer[128]=TEXT("");
				_sntprintf(szBuffer,CountArray(szBuffer),TEXT("SUB_C_ADD_SCORE 游戏命令返回 [ %.2lf ]"),m_lCompareCount);
				//输出信息
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

			//当前状态
			if(pAddScore->wState>0)m_lCompareCount=pAddScore->lScore;
			printf("用户加注SUB_C_ADD_SCORE,pUserData->wChairID_%d\n",pUserData->wChairID);
			//消息处理
			bool bOK =  OnUserAddScore(pUserData->wChairID,pAddScore->lScore,false,((pAddScore->wState>0)?true:false), pAddScore->chciphertext);
			if (bOK == false)
			{
				if (pIServerUserItem->IsAndroidUser())
				{
					if (pAddScore->wState > 0)
					{
						//输出信息
						TCHAR szBuffer[128]=TEXT("");
						_sntprintf(szBuffer,CountArray(szBuffer),TEXT("OnUserGiveUp 游戏命令返回 [ %.2lf ]"),777);
						//输出信息
						CTraceService::TraceString(szBuffer,TraceLevel_Normal);
					}

					//消息处理
					return OnUserGiveUp(pUserData->wChairID);
				}
				else
				{
					return false;
				}
			}
			return true;
		}
	case SUB_C_WAIT_COMPARE:	//等待比牌
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
			if (m_cbPlayStatus[pUserData->wChairID]==FALSE) return false;

			//参数效验
			ASSERT(pUserData->wChairID==m_wCurrentUser);
			if(pUserData->wChairID!=m_wCurrentUser)return false;

			//消息处理
			CMD_S_WaitCompare WaitCompare;
			WaitCompare.wCompareUser = pUserData->wChairID;
			if(m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_WAIT_COMPARE,&WaitCompare,sizeof(WaitCompare)) &&
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_WAIT_COMPARE,&WaitCompare,sizeof(WaitCompare)) )

			return true;
		}
	case SUB_C_FINISH_FLASH:	//完成动画 
		{
			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_wFlashUser[pUserData->wChairID]==TRUE);
			if (m_wFlashUser[pUserData->wChairID]==FALSE) return true;

			//设置变量
			m_wFlashUser[pUserData->wChairID] = FALSE;
			printf("完成动画\n");
			//结束游戏
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_wFlashUser[i] != FALSE)break;
			}
			if(i==GAME_PLAYER)
			{
				//删除时间
				m_pITableFrame->KillGameTimer(IDI_GAME_COMPAREEND);
				m_pITableFrame->KillGameTimer(IDI_GAME_OPENEND);

				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&m_StGameEnd,sizeof(m_StGameEnd));
				printf("发送游戏结束消息9999999999\n");
				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}
			return true;
		}
// 	case SUB_C_ADD_SCORE_TIME:
// 		{
// 			//用户效验
// 			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
// 			if (pUserData->cbUserStatus!=US_PLAYING) return true;
// 
// 			m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE*30,1,0);
// 			return true;
// 		}
	case SUB_C_AMDIN_CHANGE_CARD:			//换牌
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

						//超控发送数据
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

			//权限判断
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			CMD_C_UpdateStorage *pUpdateStorage=(CMD_C_UpdateStorage *)pData;
			g_lRoomStorageCurrent = pUpdateStorage->lRoomStorageCurrent;
			g_lStorageDeductRoom = pUpdateStorage->lRoomStorageDeduct;

			//20条操作记录
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 控制账户[%s],修改当前库存为 %I64d,衰减值为 %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), 
				g_lRoomStorageCurrent, g_lStorageDeductRoom);

			g_ListOperationRecord.AddTail(strOperationRecord);

			//写入日志
			strOperationRecord += TEXT("\n");

			CString strFileName;
			strFileName.Format(TEXT("诈金花[%s]特殊账户日志.log"), m_pGameServiceOption->szServerName);
			WriteInfo(strFileName, strOperationRecord);

			//变量定义
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//数组下标
			while(posListRecord)
			{
				CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

				CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//发送数据
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
	case SUB_C_STORAGEMAXMUL:
		{
			ASSERT(wDataSize==sizeof(CMD_C_ModifyStorage));
			if(wDataSize!=sizeof(CMD_C_ModifyStorage)) return false;

			//权限判断
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			CMD_C_ModifyStorage *pModifyStorage = (CMD_C_ModifyStorage *)pData;
			g_lStorageMax1Room = pModifyStorage->lMaxRoomStorage[0];
			g_lStorageMax2Room = pModifyStorage->lMaxRoomStorage[1];
			g_lStorageMul1Room = (SCORE)(pModifyStorage->wRoomStorageMul[0]);
			g_lStorageMul2Room = (SCORE)(pModifyStorage->wRoomStorageMul[1]);

			//20条操作记录
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d,控制账户[%s], 修改库存上限值1为 %I64d,赢分概率1为 %I64d,上限值2为 %I64d,赢分概率2为 %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), g_lStorageMax1Room, g_lStorageMul1Room, g_lStorageMax2Room, g_lStorageMul2Room);

			g_ListOperationRecord.AddTail(strOperationRecord);

			//写入日志
			strOperationRecord += TEXT("\n");

			CString strFileName;
			strFileName.Format(TEXT("诈金花[%s]特殊账户日志.log"), m_pGameServiceOption->szServerName);
			WriteInfo(strFileName, strOperationRecord);

			//变量定义
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//数组下标
			while(posListRecord)
			{
				CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

				CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//发送数据
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

			//权限判断
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
			{
				return false;
			}

			CMD_C_RequestQuery_User *pQuery_User = (CMD_C_RequestQuery_User *)pData;

			//遍历映射
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
					//拷贝用户信息数据
					QueryResult.bFind = true;
					CopyMemory(&(QueryResult.userinfo), &userinfo, sizeof(userinfo));

					ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
					CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
				}
			}

			//发送数据
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

			//权限判断
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return false;
			}

			CMD_C_UserControl *pUserControl = (CMD_C_UserControl *)pData;

			//遍历映射
			POSITION ptMapHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			//20条操作记录
			if (g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				g_ListOperationRecord.RemoveHead();
			}

			//变量定义
			CMD_S_UserControl serverUserControl;
			ZeroMemory(&serverUserControl, sizeof(serverUserControl));

			TCHAR szNickName[LEN_NICKNAME];
			ZeroMemory(szNickName, sizeof(szNickName));

			//激活控制
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
						//激活控制标识
						bool bEnableControl = false;
						IsSatisfyControl(userinfo, bEnableControl);

						//满足控制
						if (bEnableControl)
						{
							ROOMUSERCONTROL roomusercontrol;
							ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));
							CopyMemory(&(roomusercontrol.roomUserInfo), &userinfo, sizeof(userinfo));
							CopyMemory(&(roomusercontrol.userControl), &(pUserControl->userControlInfo), sizeof(roomusercontrol.userControl));


							//遍历链表，除重
							TravelControlList(roomusercontrol);

							//压入链表（不压入同GAMEID和NICKNAME)
							g_ListRoomUserControl.AddHead(roomusercontrol);

							//复制数据
							serverUserControl.dwGameID = userinfo.dwGameID;
							CopyMemory(serverUserControl.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
							serverUserControl.controlResult = CONTROL_SUCCEED;
							serverUserControl.controlType = pUserControl->userControlInfo.control_type;
							serverUserControl.cbControlCount = pUserControl->userControlInfo.cbControlCount;

							//操作记录
							CString strOperationRecord;
							CString strControlType;
							GetControlTypeString(serverUserControl.controlType, strControlType);
							CTime time = CTime::GetCurrentTime();
							strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 控制账户[%s], 控制玩家%s,%s,控制局数%d "),
								time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName, strControlType, serverUserControl.cbControlCount);

							g_ListOperationRecord.AddTail(strOperationRecord);

							//写入日志
							strOperationRecord += TEXT("\n");

							CString strFileName;
							strFileName.Format(TEXT("诈金花[%s]特殊账户日志.log"), m_pGameServiceOption->szServerName);
							WriteInfo(strFileName, strOperationRecord);
						}
						else	//不满足
						{
							//复制数据
							serverUserControl.dwGameID = userinfo.dwGameID;
							CopyMemory(serverUserControl.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
							serverUserControl.controlResult = CONTROL_FAIL;
							serverUserControl.controlType = pUserControl->userControlInfo.control_type;
							serverUserControl.cbControlCount = 0;

							//操作记录
							CString strOperationRecord;
							CString strControlType;
							GetControlTypeString(serverUserControl.controlType, strControlType);
							CTime time = CTime::GetCurrentTime();
							strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 控制账户[%s], 控制玩家%s,%s,失败！"),
								time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName, strControlType);

							g_ListOperationRecord.AddTail(strOperationRecord);

							//写入日志
							strOperationRecord += TEXT("\n");

							CString strFileName;
							strFileName.Format(TEXT("诈金花[%s]特殊账户日志.log"), m_pGameServiceOption->szServerName);
							WriteInfo(strFileName, strOperationRecord);
						}

						//发送数据
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));

						CMD_S_Operation_Record OperationRecord;
						ZeroMemory(&OperationRecord, sizeof(OperationRecord));
						POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
						WORD wIndex = 0;//数组下标
						while(posListRecord)
						{
							CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

							CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
							wIndex++;
						}

						ASSERT(wIndex <= MAX_OPERATION_RECORD);

						//发送数据
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

						return true;
					}
				}

				ASSERT(FALSE);
				return false;
			}
			else	//取消控制
			{
				ASSERT(pUserControl->userControlInfo.control_type == CONTINUE_CANCEL);

				POSITION ptListHead = g_ListRoomUserControl.GetHeadPosition();
				POSITION ptTemp;
				ROOMUSERCONTROL roomusercontrol;
				ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

				//遍历链表
				while(ptListHead)
				{
					ptTemp = ptListHead;
					roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);
					if (pUserControl->dwGameID == roomusercontrol.roomUserInfo.dwGameID || _tcscmp(pUserControl->szNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
					{
						//复制数据
						serverUserControl.dwGameID = roomusercontrol.roomUserInfo.dwGameID;
						CopyMemory(serverUserControl.szNickName, roomusercontrol.roomUserInfo.szNickName, sizeof(roomusercontrol.roomUserInfo.szNickName));
						serverUserControl.controlResult = CONTROL_CANCEL_SUCCEED;
						serverUserControl.controlType = pUserControl->userControlInfo.control_type;
						serverUserControl.cbControlCount = 0;

						//移除元素
						g_ListRoomUserControl.RemoveAt(ptTemp);

						//发送数据
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));

						//操作记录
						CString strOperationRecord;
						CTime time = CTime::GetCurrentTime();
						strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 控制账户[%s], 取消对玩家%s的控制！"),
							time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName);

						g_ListOperationRecord.AddTail(strOperationRecord);

						//写入日志
						strOperationRecord += TEXT("\n");

						CString strFileName;
						strFileName.Format(TEXT("诈金花[%s]特殊账户日志.log"), m_pGameServiceOption->szServerName);
						WriteInfo(strFileName, strOperationRecord);

						CMD_S_Operation_Record OperationRecord;
						ZeroMemory(&OperationRecord, sizeof(OperationRecord));
						POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
						WORD wIndex = 0;//数组下标
						while(posListRecord)
						{
							CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

							CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
							wIndex++;
						}

						ASSERT(wIndex <= MAX_OPERATION_RECORD);

						//发送数据
						m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
						m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

						return true;
					}
				}

				//复制数据
				serverUserControl.dwGameID = pUserControl->dwGameID;
				CopyMemory(serverUserControl.szNickName, pUserControl->szNickName, sizeof(serverUserControl.szNickName));
				serverUserControl.controlResult = CONTROL_CANCEL_INVALID;
				serverUserControl.controlType = pUserControl->userControlInfo.control_type;
				serverUserControl.cbControlCount = 0;

				//发送数据
				m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
				m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));

				//操作记录
				CString strOperationRecord;
				CTime time = CTime::GetCurrentTime();
				strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 控制账户[%s], 取消对玩家%s的控制，操作无效！"),
					time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName);

				g_ListOperationRecord.AddTail(strOperationRecord);

				//写入日志
				strOperationRecord += TEXT("\n");

				CString strFileName;
				strFileName.Format(TEXT("诈金花[%s]特殊账户日志.log"), m_pGameServiceOption->szServerName);
				WriteInfo(strFileName, strOperationRecord);

				CMD_S_Operation_Record OperationRecord;
				ZeroMemory(&OperationRecord, sizeof(OperationRecord));
				POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
				WORD wIndex = 0;//数组下标
				while(posListRecord)
				{
					CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

					CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
					wIndex++;
				}

				ASSERT(wIndex <= MAX_OPERATION_RECORD);

				//发送数据
				m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
				m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			}

			return true;
		}
	case SUB_C_REQUEST_UDPATE_ROOMINFO:
		{
			//权限判断
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

			//遍历映射
			POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;
			ROOMUSERINFO userinfo;
			ZeroMemory(&userinfo, sizeof(userinfo));

			while(ptHead)
			{
				g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
				if (dwKeyGameID == userinfo.dwGameID && _tcscmp(szKeyNickName, userinfo.szNickName) == 0)
				{
					//拷贝用户信息数据
					CopyMemory(&(RoomInfo_Result.currentqueryuserinfo), &userinfo, sizeof(userinfo));

					ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
					CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
				}
			}

			//
			//变量定义
			POSITION ptListHead = g_ListRoomUserControl.GetHeadPosition();
			POSITION ptTemp;
			ROOMUSERCONTROL roomusercontrol;
			ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

			//遍历链表
			while(ptListHead)
			{
				ptTemp = ptListHead;
				roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

				//寻找玩家
				if ((dwKeyGameID == roomusercontrol.roomUserInfo.dwGameID) &&
					_tcscmp(szKeyNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
				{
					RoomInfo_Result.bExistControl = true;
					CopyMemory(&(RoomInfo_Result.currentusercontrol), &(roomusercontrol.userControl), sizeof(roomusercontrol.userControl));
					break;				
				}
			}

			//发送数据
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT, &RoomInfo_Result, sizeof(RoomInfo_Result));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT, &RoomInfo_Result, sizeof(RoomInfo_Result));

			return true;
		}
	case SUB_C_CLEAR_CURRENT_QUERYUSER:
		{
			//权限判断
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

//框架消息处理
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}



//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//更新房间用户信息
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	//更新同桌用户控制
	UpdateUserControl(pIServerUserItem);
	
	//初始化密钥
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

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//更新房间用户信息
	UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
	
	if (!bLookonUser)
	{
		//清除密钥
		ZeroMemory(m_chUserAESKey[wChairID], sizeof(m_chUserAESKey[wChairID]));
	}

	return true;
}

//用户同意
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//私人房设置游戏模式
	if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0)
	{
		//cbGameRule[1] 为  2 、3 、4 、5, 0分别对应 2人 、 3人 、 4人 、 5人 、 2-5人 这几种配置
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

//放弃事件
bool CTableFrameSink::OnUserGiveUp(WORD wChairID,bool bExit)
{
	//设置数据
	m_cbGiveUpUser[wChairID] = TRUE;
	m_cbPlayStatus[wChairID] = FALSE;

	//发送消息
	CMD_S_GiveUp GiveUp;
	GiveUp.wGiveUpUser=wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));

	//修改积分
	tagScoreInfo ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
	ScoreInfo.cbType =(bExit)?SCORE_TYPE_FLEE:SCORE_TYPE_LOSE;
	ScoreInfo.lRevenue=0;  
	ScoreInfo.lScore=-m_lTableScore[wChairID];

	m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);

	//获取用户
	IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);

	//库存统计
	if ((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser())) 
		UpdateStorage(m_lTableScore[wChairID]);//g_lRoomStorageCurrent += m_lTableScore[wChairID];

	m_dElapse = GetTickCount();

	//人数统计
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//判断结束
	if (wPlayerCount>=2)
	{
		//弃牌后设置游戏状态为SIT
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
			printf("放弃下注玩家_%d\n",m_wCurrentUser);
			m_lCompareCount = 0;
			OnUserAddScore(wChairID,0L,true,false, chTempEncryption);
		}
	}
	else OnEventGameConclude(INVALID_CHAIR,NULL,GER_NO_PLAYER);


	
	return true;
}

//看牌事件
bool CTableFrameSink::OnUserLookCard(WORD wChairID)
{
	//状态效验-liuyang
	//ASSERT(m_wCurrentUser==wChairID);
	//if (m_wCurrentUser!=wChairID) return false;

	//参数效验
	ASSERT(!m_bMingZhu[wChairID]);
	if (m_bMingZhu[wChairID]) return true;

	//设置参数
	m_bMingZhu[wChairID]=true;
	//构造数据
	CMD_S_LookCard LookCard;
	ZeroMemory(&LookCard,sizeof(LookCard));
	LookCard.wLookCardUser=wChairID;

	m_dElapse = GetTickCount();

	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_LOOK_CARD,&LookCard,sizeof(LookCard));
	printf("用户看牌OnUserLookCard_m_wCurrentUser_%d,GetChairID()_%d\n",m_wCurrentUser,wChairID);
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

//自动比牌事件
bool CTableFrameSink::OnUserAutoCompareCard(WORD wChairID)
{
	//发送消息
	CMD_S_AutoCompare AutoCompare;
	AutoCompare.wAutoCompareUser=wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_AUTO_COMPARE,&AutoCompare,sizeof(AutoCompare));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_AUTO_COMPARE,&AutoCompare,sizeof(AutoCompare));

	return true;
}

//比牌事件
bool CTableFrameSink::OnUserCompareCard(WORD wFirstChairID,WORD wNextChairID)
{
	//效验参数
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
	//原来>=2*m_lCellScore，如果第一轮都的积分都没能达到加注跟注的条件，那么只能开牌，而底注就是m_lCellScore，
	//那么比牌就一定会掉线，此时m_lTableScore[m_wCurrentUser]为底注+m_lCompareCount。
	//至于为什么要大于等于2*m_lCellScore，原因不明
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
	printf("用户比牌OnUserCompareCard_wFirstChairID_%d,wNextChairID_%d\n",wFirstChairID,wNextChairID);
	//删除定时器
	m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);

	//比较大小
	BYTE cbWin=m_GameLogic.CompareCard(m_cbHandCardData[wFirstChairID],m_cbHandCardData[wNextChairID],MAX_COUNT);

	//状态设置
	m_lCompareCount=0;

	//胜利用户
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

	//输牌后设置游戏状态为SIT
	//m_pITableFrame->GetTableUserItem(wLostUser)->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), wLostUser);

	//设置数据
	m_wCompardUser[wLostUser].Add(wWinUser);
	m_wCompardUser[wWinUser].Add(wLostUser);
	m_cbPlayStatus[wLostUser]=FALSE;

	//人数统计
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE) wPlayerCount++;
	}

	//继续游戏
	if(wPlayerCount>=2)
	{
		//用户切换
		WORD wNextPlayer=INVALID_CHAIR;
		for (WORD i=1;i<m_wPlayerCount;i++)
		{
			//设置变量
			wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

			//继续判断
			if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
		}
		//设置用户
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
			strFileName.Format(TEXT("诈金花机器人IDI_USER_ADD_SCORE.log"));
			strtip.Format(TEXT("OnUserCompareCard  m_wCurrentUser==INVALID_CHAIR [%d-%d-%d %d:%d:%d]\n"), m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
			//WriteInfo(strFileName, strtip);
		}
	}

	//构造数据
	CMD_S_CompareCard CompareCard;
	CompareCard.wCurrentUser = m_wCurrentUser;
	CompareCard.wLostUser = wLostUser;
	CompareCard.wCompareUser[0] = wFirstChairID;
	CompareCard.wCompareUser[1] = wNextChairID;

	m_dElapse = GetTickCount();

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_COMPARE_CARD,&CompareCard,sizeof(CompareCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_COMPARE_CARD,&CompareCard,sizeof(CompareCard));

	//修改积分
	tagScoreInfo ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
	ScoreInfo.cbType =SCORE_TYPE_LOSE;
	ScoreInfo.lRevenue=0;
	ScoreInfo.lScore=-m_lTableScore[wLostUser];

	m_pITableFrame->WriteUserScore(wLostUser, ScoreInfo);
	
	//库存统计
	IServerUserItem * pIServreUserItemLost=m_pITableFrame->GetTableUserItem(wLostUser);
	if ((pIServreUserItemLost!=NULL)&&(!pIServreUserItemLost->IsAndroidUser())) 
		UpdateStorage(m_lTableScore[wLostUser]);//g_lRoomStorageCurrent += m_lTableScore[wLostUser];

	//结束游戏
	//if (wPlayerCount<2)
	//{
	//	m_wBankerUser=wWinUser;
	//	m_wFlashUser[wNextChairID] = TRUE;
	//	m_wFlashUser[wFirstChairID] = TRUE;
	//	OnEventGameConclude(GAME_PLAYER,NULL,GER_COMPARECARD);
	//}

	////设置定时器 非房卡房间
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
		printf("游戏结束\n");
	}
	else
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wLostUser);
		if (pIServerUserItem->GetUserScore() < m_pGameServiceOption->lMinEnterScore)
		{
			//m_pITableFrame->PerformStandUpAction(pIServerUserItem);
		}
		//设置定时器 非房卡房间
		if (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) == 0)
		{
			m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
			m_pITableFrame->SetGameTimer(IDI_ADD_SCORE,TIME_ADD_SCORE,1,2);
		}
	}

	return true;
}

//开牌事件
bool CTableFrameSink::OnUserOpenCard(WORD wUserID)
{
	//效验参数
	ASSERT(m_lCompareCount>0);
	if(!(m_lCompareCount>0))return false;
	ASSERT(m_wCurrentUser==wUserID);
	if(m_wCurrentUser!=wUserID)return false;
	//LONGLONG lTemp=(m_bMingZhu[wUserID])?6:5;
	//ASSERT((m_lUserMaxScore[wUserID]-m_lTableScore[wUserID]+m_lCompareCount) < (m_lMaxCellScore*lTemp));
	//if((m_lUserMaxScore[wUserID]-m_lTableScore[wUserID]+m_lCompareCount) >= (m_lMaxCellScore*lTemp))return false;

	//清理数据
	m_wCurrentUser=INVALID_CHAIR;
	m_lCompareCount = 0;

	if (m_wCurrentUser==INVALID_CHAIR)
	{
		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("诈金花机器人IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnEventGameStart  m_wCurrentUser==INVALID_CHAIR [%d-%d-%d %d:%d:%d]\n"), m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}

	//保存扑克
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

	//比牌玩家
	WORD wWinner=wUserID;

	//查找最大玩家
	for (WORD i=1;i<m_wPlayerCount;i++)
	{
		WORD w=(wUserID+i)%m_wPlayerCount;

		//用户过滤
		if (m_cbPlayStatus[w]==FALSE) continue;

		//对比扑克
		if (m_GameLogic.CompareCard(cbUserCardData[w],cbUserCardData[wWinner],MAX_COUNT)>=TRUE) 
		{
			wWinner=w;
		}
	}
	ASSERT(m_cbPlayStatus[wWinner]==TRUE);
	if(m_cbPlayStatus[wWinner]==FALSE)return false;

	//胜利玩家
	m_wBankerUser = wWinner;

	//构造数据
	CMD_S_OpenCard OpenCard;
	OpenCard.wWinner=wWinner;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));

	//结束游戏
	for(WORD i=0;i<GAME_PLAYER;i++)if(m_cbPlayStatus[i]==TRUE)m_wFlashUser[i] = TRUE;
	OnEventGameConclude(GAME_PLAYER,NULL,GER_OPENCARD);

	return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, SCORE lScore, bool bGiveUp, bool bCompareCard, unsigned char	chciphertext[AESENCRYPTION_LENGTH])
{

	CString strtip;
	CTime time = CTime::GetCurrentTime();
	CString strFileName;
	//strFileName.Format(TEXT("诈金花[%s]OnUserAddScore.log"), m_pGameServiceOption->szServerName);
	strFileName.Format(TEXT("诈金花OnUserAddScore.log"));

	if (bGiveUp==false)				//设置数据
	{
		//解密操作
		AESEncrypt aes(m_chUserAESKey[wChairID]);
		aes.InvCipher(chciphertext);

		//校验数据
		//for (WORD i=0; i<AESENCRYPTION_LENGTH; i++)
		//{
		//	if (chCommonEncryption[i] != chciphertext[i])
		//	{
		//		//打印作弊名单
		//		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		//		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser() == false)
		//		{
		//			strtip.Format(TEXT("%d/%d/%d:%d-%d-%d___玩家[%s] USERID为%d 作弊\n"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(),
		//				pIServerUserItem->GetNickName(), pIServerUserItem->GetUserID());

		//			WriteInfo(strFileName, strtip);
		//		}

		//		return false;
		//	}
		//}

		//状态效验
		ASSERT(m_wCurrentUser==wChairID);
		if (m_wCurrentUser!=wChairID) 
		{
			strtip.Format(TEXT("m_wCurrentUser!=wChairID! m_wCurrentUser = %d  wChairID = %d bGiveUp = %d bCompareCard = %d [%d-%d-%d %d:%d:%d]\n"), m_wCurrentUser,wChairID,bGiveUp,bCompareCard,time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			WriteInfo(strFileName, strtip);
			return false;
		}

		//金币效验
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
				//用户注金
				if (m_lTableScore[wChairID] + lScore >= pIServreUserItem->GetUserInfo()->lScore)
				{
					lScore = pIServreUserItem->GetUserInfo()->lScore-m_lTableScore[wChairID];
					m_lTableScore[wChairID]+=lScore;
				}
				else if (m_nCurrentRounds >= m_nTotalRounds)
				{
					m_lTableScore[wChairID]+=lScore;
				}
				//设置用户
				if(!bCompareCard)
				{
					//用户切换
					WORD wNextPlayer=INVALID_CHAIR;
					for (WORD i=1;i<m_wPlayerCount;i++)
					{
						//设置变量
						wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;
						//继续判断
						if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
					}
					m_wCurrentUser=wNextPlayer;
				}

				//构造数据
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

				//判断状态
				AddScore.wCompareState=( bCompareCard )?TRUE:FALSE;

				//发送数据
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));


				//清理数据
				m_wCurrentUser=INVALID_CHAIR;

				//保存扑克
				BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
				CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

				//wChairID
				WORD wWinner=wChairID;

				//查找最大玩家
				for (WORD i=1;i<m_wPlayerCount;i++)
				{
					WORD w=(wChairID+i)%m_wPlayerCount;

					//用户过滤
					if (m_cbPlayStatus[w]==FALSE) continue;

					//对比扑克
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

				//胜利玩家
				m_wBankerUser = wWinner;

				//结束游戏
				for(WORD i=0;i<GAME_PLAYER;i++)if(m_cbPlayStatus[i]==TRUE)m_wFlashUser[i] = TRUE;
				m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
				OnEventGameConclude(GAME_PLAYER,NULL,GER_OPENCARD);
				//删除定时器
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
		printf("用户加注SUB_C_ADD_SCORE,pUserData->wChairID_%d----111111111111\n",wChairID);


		IServerUserItem *pIserberUser=m_pITableFrame->GetTableUserItem(wChairID);
		if(pIserberUser->GetUserScore() < m_lTableScore[wChairID]+lScore) 
		{
			strtip.Format(TEXT("pIserberUser->GetUserScore() < m_lTableScore[wChairID]+lScore  pIserberUser->GetUserScore() = %.2f  m_lTableScore[wChairID]+lScore = %.2f[%d-%d-%d:%d:%d:%d]\n"), pIserberUser->GetUserScore(),m_lTableScore[wChairID]+lScore,time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			WriteInfo(strFileName, strtip);
			return false;
		}
		printf("用户加注SUB_C_ADD_SCORE,pUserData->wChairID_%d----111111111111\n",wChairID);
		//当前倍数
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

		//用户注金
		m_lTableScore[wChairID]+=lScore;
	}

	//设置用户
	if(!bCompareCard)
	{
		//用户切换
		WORD wNextPlayer=INVALID_CHAIR;
		for (WORD i=1;i<m_wPlayerCount;i++)
		{
			//设置变量
			wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

			//继续判断
			if (m_cbPlayStatus[wNextPlayer]==TRUE) break;
		}
		m_wCurrentUser=wNextPlayer;
	}

	if (m_wCurrentUser==INVALID_CHAIR)
	{
		CString strtip;
		CTime time = CTime::GetCurrentTime();
		CString strFileName;
		strFileName.Format(TEXT("诈金花机器人IDI_USER_ADD_SCORE.log"));
		strtip.Format(TEXT("OnUserAddScore  m_wCurrentUser==INVALID_CHAIR [%d-%d-%d %d:%d:%d]\n"), m_wCurrentUser,time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		//WriteInfo(strFileName, strtip);
	}

	//乱序密钥
	unsigned char chTempRandAesKey[AESKEY_VARIABLECOUNT];
	ZeroMemory(chTempRandAesKey, sizeof(chTempRandAesKey));
	RandAESKey(chTempRandAesKey, AESKEY_VARIABLECOUNT);

	//更新密钥
	CopyMemory(&m_chUserAESKey[wChairID][AESKEY_TOTALCOUNT - AESKEY_VARIABLECOUNT], chTempRandAesKey, sizeof(chTempRandAesKey));
	CMD_S_UpdateAESKey UpdateAESKey;
	ZeroMemory(&UpdateAESKey, sizeof(UpdateAESKey));
	CopyMemory(UpdateAESKey.chUserUpdateAESKey, m_chUserAESKey[wChairID], sizeof(m_chUserAESKey[wChairID]));

	m_pITableFrame->SendTableData(wChairID, SUB_S_UPDATEAESKEY, &UpdateAESKey, sizeof(UpdateAESKey));

	//构造数据
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

	//判断状态
	AddScore.wCompareState=( bCompareCard )?TRUE:FALSE;
	printf("用户加注SUB_C_ADD_SCORE,pUserData->wChairID_%d----111111111111\n",wChairID);
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

	//超控发送数据
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

	//	//派发扑克
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
	//删除定时器
	m_pITableFrame->KillGameTimer(IDI_ADD_SCORE);
	//设置定时器
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

////扑克分析
//void CTableFrameSink::AnalyseStartCard()
//{
//	//机器人数
//	WORD wAiCount = 0;
//	WORD wPlayerCount = 0;
//	for (WORD i=0; i<m_wPlayerCount; i++)
//	{
//		//获取用户
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
//	//全部机器
//	//if(wPlayerCount == wAiCount || wAiCount==0) return;
//
//	if(wPlayerCount == wAiCount) return;
//
//	//扑克变量
//	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
//	CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
//
//	//排列扑克
//	for (WORD i=0;i<m_wPlayerCount;i++)
//	{
//		m_GameLogic.SortCardList(cbUserCardData[i], MAX_COUNT);
//	}
//
//	//变量定义
//	WORD wWinUser = INVALID_CHAIR;
//
//	//查找数据
//	for (WORD i = 0; i < m_wPlayerCount; i++)
//	{
//		//用户过滤
//		if (m_cbPlayStatus[i] == FALSE) continue;
//
//		//设置用户
//		if (wWinUser == INVALID_CHAIR)
//		{
//			wWinUser=i;
//			continue;
//		}
//
//		//对比扑克
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
//		//*****************************个人单控，多人受控，以最后坐下的人控制为准*************************//
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
//			//最后坐下受控玩家输
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
//				//交换数据
//				BYTE cbTempData[MAX_COUNT];
//				CopyMemory(cbTempData, m_cbHandCardData[wBeWin], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wBeWin], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
//			}
//			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wBeWin);
//			if (pIServerUserItem)
//			{
//				printf("单控玩家胜率:%d,ID:%d\n",nRate,pIServerUserItem->GetGameID());
//				printf("最后坐下受控玩家输,赢得人ID:%d\n",pIServerUserItem->GetGameID());
//			}
//
//		} 
//		else
//		{
//			//最后坐下受控玩家赢
//
//			if(wControlPlayer != wWinUser)
//			{
//				//交换数据
//				BYTE cbTempData[MAX_COUNT];
//				CopyMemory(cbTempData, m_cbHandCardData[wControlPlayer], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wControlPlayer], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
//				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
//			}
//			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wControlPlayer);
//			if (pIServerUserItem)
//			{
//				printf("单控玩家胜率:%d,ID:%d\n",nRate,pIServerUserItem->GetGameID());
//				printf("最后坐下受控玩家赢,ID:%d\n",pIServerUserItem->GetGameID());
//			}
//		}
//		//*****************************个人单控，多人受控，以最后坐下的人控制为准END**********************//
//	} 
//	else
//	{
//		//*****************************全局控制，控机器人输赢********************************************//
//		//设置文件名
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
//		//全局控制
//		if (!bIsWin)
//		{
//			//机器人赢
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
//				//交换数据
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
//			//玩家赢
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
//				//交换数据
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
//		//*****************************全局控制，控机器人输赢END********************************************//
//	}
//
//
//	
//
//
//	////库存判断
//	//if (g_lRoomStorageCurrent - m_lCellScore*10*wAiCount < 0)
//	//{
//	//	//随机赢家
//	//	WORD wBeWinAndroid = INVALID_CHAIR;
//
//	//	do 
//	//	{
//	//		wBeWinAndroid = rand()%GAME_PLAYER;
//	//	} while (m_pITableFrame->GetTableUserItem(wBeWinAndroid) == NULL || !m_pITableFrame->GetTableUserItem(wBeWinAndroid)->IsAndroidUser());
//
//	//	if(wBeWinAndroid != wWinUser)
//	//	{
//	//		//交换数据
//	//		BYTE cbTempData[MAX_COUNT];
//	//		CopyMemory(cbTempData, m_cbHandCardData[wBeWinAndroid], sizeof(BYTE)*MAX_COUNT);
//	//		CopyMemory(m_cbHandCardData[wBeWinAndroid], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
//	//		CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
//	//	}
//	//}
//	////控制输分
//	//else if ((g_lRoomStorageCurrent > g_lStorageMax1Room && rand()%100 <= g_lStorageMul1Room)
//	//      || (g_lRoomStorageCurrent > g_lStorageMax2Room && rand()%100 <= g_lStorageMul2Room))
//	//{
//	//	//随机赢家
//	//	WORD wWinPlayer = INVALID_CHAIR;
//
//	//	do 
//	//	{
//	//		wWinPlayer = rand()%GAME_PLAYER;
//	//	} while (m_pITableFrame->GetTableUserItem(wWinPlayer) == NULL || m_pITableFrame->GetTableUserItem(wWinPlayer)->IsAndroidUser());
//
//	//	if(wWinPlayer != wWinUser)
//	//	{
//	//		//交换数据
//	//		BYTE cbTempData[MAX_COUNT];
//	//		CopyMemory(cbTempData, m_cbHandCardData[wWinPlayer], sizeof(BYTE)*MAX_COUNT);
//	//		CopyMemory(m_cbHandCardData[wWinPlayer], m_cbHandCardData[wWinUser], sizeof(BYTE)*MAX_COUNT);
//	//		CopyMemory(m_cbHandCardData[wWinUser], cbTempData, sizeof(BYTE)*MAX_COUNT);
//	//	}
//	//}
//
//	return;
//}
//扑克分析
void CTableFrameSink::AnalyseStartCard()
{
	//机器人数
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i=0; i<m_wPlayerCount; i++)
	{
		//获取用户
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

	//全部机器
	if(wPlayerCount == wAiCount || wAiCount==0) return;


	//设置文件名
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
				ProbeJudge(j,CHEAT_TYPE_WIN);  //指定玩家胜利
		}
	}
	else
	{
		for (int j = 0;j < GAME_PLAYER;j++)
		{
			IServerUserItem *pServerUser = m_pITableFrame->GetTableUserItem(j);
			if(pServerUser == NULL)continue;
			if(!pServerUser->IsAndroidUser())
				ProbeJudge(j,CHEAT_TYPE_WIN);  //指定玩家胜利
		}
	}
	return;
}
//亮牌事件
bool CTableFrameSink::OnUserShowCard(WORD wChairID)
{
	//构造数据
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

//是否衰减
bool CTableFrameSink::NeedDeductStorage()
{
	//机器人数
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i=0; i<m_wPlayerCount; i++)
	{
		//获取用户
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

//读取配置
void CTableFrameSink::ReadConfigInformation()
{	
	//获取自定义配置
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

	//房间名称
	memcpy(m_szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));

	m_nTotalRounds = GetPrivateProfileInt(m_szRoomName, TEXT("Rounds"), 20, m_szConfigFileName);

	m_nZhaDan = GetPrivateProfileInt(m_szRoomName, TEXT("ZhaDan"), 5, m_szConfigFileName);
	m_nShunJin = GetPrivateProfileInt(m_szRoomName, TEXT("ShunJin"), 15, m_szConfigFileName);
	m_nJinHua = GetPrivateProfileInt(m_szRoomName, TEXT("JinHua"), 30, m_szConfigFileName);
	m_nShunZI = GetPrivateProfileInt(m_szRoomName, TEXT("ShunZi"), 50, m_szConfigFileName);
	m_nDuiZi = GetPrivateProfileInt(m_szRoomName, TEXT("DuiZi"), 70, m_szConfigFileName);
	m_nSanPai = GetPrivateProfileInt(m_szRoomName, TEXT("SanPai"), 100, m_szConfigFileName);
}

//更新房间用户信息
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction)
{
	//变量定义
	ROOMUSERINFO roomUserInfo;
	ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

	roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
	CopyMemory(&(roomUserInfo.szNickName), pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
	roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
	roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

	roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

	//用户坐下和重连
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

	//遍历映射，删除离开房间的玩家，
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
		strFileName.Format(TEXT("诈金花[%s]特殊账户日志.log"), m_pGameServiceOption->szServerName);
		WriteInfo(strFileName, strtip);
	}

	delete[] pdwRemoveKey;
}

//更新同桌用户控制
void CTableFrameSink::UpdateUserControl(IServerUserItem *pIServerUserItem)
{
	//变量定义
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERCONTROL roomusercontrol;

	//初始化
	ptListHead = g_ListRoomUserControl.GetHeadPosition();
	ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

	//遍历链表
	while(ptListHead)
	{
		ptTemp = ptListHead;
		roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

		//寻找已存在控制玩家
		if ((pIServerUserItem->GetGameID() == roomusercontrol.roomUserInfo.dwGameID) &&
			_tcscmp(pIServerUserItem->GetNickName(), roomusercontrol.roomUserInfo.szNickName) == 0)
		{
			//获取元素
			ROOMUSERCONTROL &tmproomusercontrol = g_ListRoomUserControl.GetAt(ptTemp);

			//重设参数
			tmproomusercontrol.roomUserInfo.wChairID = pIServerUserItem->GetChairID();
			tmproomusercontrol.roomUserInfo.wTableID = m_pITableFrame->GetTableID() + 1;

			return;
		}
	}
}

//除重用户控制
void CTableFrameSink::TravelControlList(ROOMUSERCONTROL keyroomusercontrol)
{
	//变量定义
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERCONTROL roomusercontrol;

	//初始化
	ptListHead = g_ListRoomUserControl.GetHeadPosition();
	ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

	//遍历链表
	while(ptListHead)
	{
		ptTemp = ptListHead;
		roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

		//寻找已存在控制玩家在用一张桌子切换椅子
		if ((keyroomusercontrol.roomUserInfo.dwGameID == roomusercontrol.roomUserInfo.dwGameID) &&
			_tcscmp(keyroomusercontrol.roomUserInfo.szNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
		{
			g_ListRoomUserControl.RemoveAt(ptTemp);
		}
	}
}

//是否满足控制条件
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

//分析房间用户控制
bool CTableFrameSink::AnalyseRoomUserControl(ROOMUSERCONTROL &Keyroomusercontrol, POSITION &ptList)
{
	//变量定义
	POSITION ptListHead;
	POSITION ptTemp;
	ROOMUSERCONTROL roomusercontrol;

	//遍历链表
	for (WORD i=0; i<m_wPlayerCount; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}

		//初始化
		ptListHead = g_ListRoomUserControl.GetHeadPosition();
		ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

		//遍历链表
		while(ptListHead)
		{
			ptTemp = ptListHead;
			roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

			//寻找玩家
			if ((pIServerUserItem->GetGameID() == roomusercontrol.roomUserInfo.dwGameID) &&
				_tcscmp(pIServerUserItem->GetNickName(), roomusercontrol.roomUserInfo.szNickName) == 0)
			{
				//清空控制局数为0的元素
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

				//拷贝数据
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
			controlTypestr = TEXT("控制类型为连赢");
			break;
		}
	case CONTINUE_LOST:
		{
			controlTypestr = TEXT("控制类型为连输");
			break;
		}
	case CONTINUE_CANCEL:
		{
			controlTypestr = TEXT("控制类型为取消控制");
			break;
		}
	}
}

//写日志文件
void CTableFrameSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
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

//修改库存
bool CTableFrameSink::UpdateStorage(SCORE lScore)
{
	if(!m_bUpdataStorage) return true;

	g_lRoomStorageCurrent += lScore;

	//库存为负
	if (g_lRoomStorageCurrent < 0)
	{
		CString str;
		CString strTemp;
		strTemp.Format(TEXT("%d桌 最大下注%I64d 库存为负%I64d 输赢%I64d"), m_pITableFrame->GetTableID() + 1, m_lMaxCellScore, g_lRoomStorageCurrent, lScore);
		str += strTemp;
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_lUserMaxScore[i] > 0)
			{
				strTemp.Format(TEXT("，%d号椅子最大下注%I64d，"), i, m_lUserMaxScore[i]);
				str += strTemp;
				//获取用户
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem!=NULL)
				{
					strTemp.Format(TEXT("UserID=%ld，机器人%d，分数%I64d，下注为%I64d，看牌%d，放弃%d，扑克："), pIServerUserItem->GetUserID(), pIServerUserItem->IsAndroidUser(), pIServerUserItem->GetUserScore(), m_lTableScore[i], m_bMingZhu[i], m_cbGiveUpUser[i]);
					str += strTemp;
				}
				else
				{
					strTemp.Format(TEXT("已经离开桌子，下注为%I64d 看牌%d，放弃%d，扑克："), m_lTableScore[i], m_bMingZhu[i], m_cbGiveUpUser[i]);
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
		strFileName.Format(TEXT("诈金花[%s]库存日志.log"), m_pGameServiceOption->szServerName);
		//WriteInfo(strFileName, str);
		return false;
	}
	return true;
}

//乱序密钥
void CTableFrameSink::RandAESKey(unsigned char chAESKeyBuffer[], BYTE cbBufferCount)
{
	for (WORD i=0; i<cbBufferCount; i++)
	{
		chAESKeyBuffer[i] = rand() % 256;
	}
}

//判断积分约占房间
bool CTableFrameSink::IsRoomCardScoreType()
{
	return (m_pITableFrame->GetDataBaseMode() == 1) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0);
}

//判断金币约占房间
bool CTableFrameSink::IsRoomCardTreasureType()
{
	return (m_pITableFrame->GetDataBaseMode() == 0) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0);
}

//////////////////////////////////////////////////////////////////////////
