#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

//1�ֽڶ���
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID						105									//��Ϸ I D
#define GAME_NAME					TEXT("�з���")						//��Ϸ����

#define GAME_PLAYER					MAX_CHAIR							//��Ϸ����

//�������
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//״̬����
#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_BET				GAME_STATUS_PLAY					//��ע״̬
#define	GAME_SCENE_END				GAME_STATUS_PLAY+1					//����״̬

//�������
#define AREA_ZHONG					0									//�м�����
#define AREA_PING					1									//ƽ������
#define AREA_FA						2									//��������
#define AREA_ZERO					3									//0
#define AREA_ONE					4									//1
#define AREA_TWO					5									//2
#define AREA_THREE					6									//3
#define AREA_FOUR					7									//4
#define AREA_FIVE					8									//5
#define AREA_SIX					9									//6
#define AREA_SEVEN					10									//7
#define AREA_EIGHT					11									//8
#define AREA_NINE					12									//9
#define AREA_LEOPARD				13									//����
#define AREA_TIANGANG				14									//���



#define AREA_MAX					15									//�������

//������multiple
#define MULTIPLE_ZHONG					2									//�м�����
#define MULTIPLE_PING					10									//ƽ������
#define MULTIPLE_FA						2									//��������
#define MULTIPLE_ZERO					5									//0
#define MULTIPLE_ONE					5									//1
#define MULTIPLE_TWO					5									//2
#define MULTIPLE_THREE					5									//3
#define MULTIPLE_FOUR					5									//4
#define MULTIPLE_FIVE					5									//5
#define MULTIPLE_SIX					5									//6
#define MULTIPLE_SEVEN					5									//7
#define MULTIPLE_EIGHT					5									//8
#define MULTIPLE_NINE					5									//9
#define MULTIPLE_LEOPARD				10									//����
#define MULTIPLE_TIANGANG				30									//���


//ռλ����
#define OCCUPYSEAT_VIPTYPE  0          //��Առλ
#define OCCUPYSEAT_CONSUMETYPE  1      //���Ľ��ռλ
#define OCCUPYSEAT_FREETYPE  2         //���ռλ

//���ʶ���
#define RATE_TWO_PAIR				12									//��������
#define SERVER_LEN					32									//���䳤��


#define MAX_OCCUPY_SEAT_COUNT       8                                 //���ռλ����

#define IDM_UPDATE_STORAGE			WM_USER+1001

#ifndef _UNICODE
#define myprintf	_snprintf
#define mystrcpy	strcpy
#define mystrlen	strlen
#define myscanf		_snscanf
#define	myLPSTR		LPCSTR
#else
#define myprintf	swprintf
#define mystrcpy	wcscpy
#define mystrlen	wcslen
#define myscanf		_snwscanf
#define	myLPSTR		LPWSTR
#endif
//���ýṹ
struct SUPERBANKERCONFIG
{
	int superbankerType;  //��ׯ����
	int enVipIndex;			//vip����
	SCORE lSuperBankerConsume;//��ׯ����
};
//ռλ���ýṹ
struct tagOccUpYesAtconfig 
{
	//ռλ����
	int occupyseatType;
	//vip����
	int enVipIndex;
	//ռλ����
	SCORE lOccupySeatConsume;
	//���ռλ�������
	SCORE lOccupySeatFree;
	//ǿ��վ������
	SCORE lForceStandUpCondition;
};
//��¼��Ϣ
struct tagServerGameRecord
{
	bool							bPlayerTianGang;					//��ܱ�ʶ
	bool							bBankerTianGang;					//��ܱ�ʶ
	bool							bPlayerTwoPair;						//���ӱ�ʶ
	bool							bBankerTwoPair;						//���ӱ�ʶ
	BYTE							cbPlayerCount;						//�мҵ���
	BYTE							cbBankerCount;						//ׯ�ҵ���
	BYTE							cbGameResult;						//��Ϸ���
};

struct tagCustomAndroid
{
	//��ׯ
	BOOL							nEnableRobotBanker;				//�Ƿ���ׯ
	LONGLONG						lRobotBankerCountMin;			//��ׯ����
	LONGLONG						lRobotBankerCountMax;			//��ׯ����
	LONGLONG						lRobotListMinCount;				//�б�����
	LONGLONG						lRobotListMaxCount;				//�б�����
	LONGLONG						lRobotApplyBanker;				//����������
	LONGLONG						lRobotWaitBanker;				//��������

	//��ע
	LONGLONG						lRobotMinBetTime;				//��ע�������
	LONGLONG						lRobotMaxBetTime;				//��ע�������
	SCORE						lRobotMinJetton;				//��ע������
	SCORE						lRobotMaxJetton;				//��ע������
	LONGLONG						lRobotBetMinCount;				//��ע��������
	LONGLONG						lRobotBetMaxCount;				//��ע��������
	SCORE						lRobotAreaLimit;				//��������

	//��ȡ��
	SCORE						lRobotScoreMin;					//�������
	SCORE						lRobotScoreMax;					//�������
	SCORE						lRobotBankGetMin;				//ȡ����Сֵ(��ׯ)
	SCORE						lRobotBankGetMax;				//ȡ�����ֵ(��ׯ)
	SCORE						lRobotBankGetBankerMin;			//ȡ����Сֵ(��ׯ)
	SCORE						lRobotBankGetBankerMax;			//ȡ�����ֵ(��ׯ)
	LONGLONG						lRobotBankStoMul;				//���ٷֱ�
	
	//������
	int								nAreaChance[15];		//������
	//���캯��
	tagCustomAndroid()
	{
		DefaultCustomRule();
	}

	void DefaultCustomRule()
	{
		nEnableRobotBanker = TRUE;
		lRobotBankerCountMin = 5;
		lRobotBankerCountMax = 10;
		lRobotListMinCount = 2;
		lRobotListMaxCount = 5;
		lRobotApplyBanker = 5;
		lRobotWaitBanker = 3;

		lRobotMinBetTime = 6;
		lRobotMaxBetTime = 8;
		lRobotMinJetton = 1;
		lRobotMaxJetton = 500;
		lRobotBetMinCount = 4;
		lRobotBetMaxCount = 8;
		lRobotAreaLimit = 10000000;
		
		lRobotScoreMin = 1000000;
		lRobotScoreMax = 100000000;
		lRobotBankGetMin = 100;
		lRobotBankGetMax = 30000000;
		lRobotBankGetBankerMin = 10000000;
		lRobotBankGetBankerMax = 50000000;
		lRobotBankStoMul = 50;

		int nTmpAreaChance[15] = {30, 3, 30, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1};
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));

	}
};

//��ע��Ϣ
struct tagUserBet
{
	TCHAR							szNickName[32];						//�û��ǳ�
	DWORD							dwUserGameID;						//�û�ID
	SCORE						lUserStartScore;					//�û����
	SCORE						lUserWinLost;						//�û����
	SCORE						lUserBet[AREA_MAX];				//�û���ע
};

//��ע��Ϣ����
typedef CWHArray<tagUserBet,tagUserBet&> CUserBetArray;

//������
#define RQ_REFRESH_STORAGE		1
#define RQ_SET_STORAGE			2
//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_FREE				99									//��Ϸ����
#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_PLACE_JETTON			101									//�û���ע
#define SUB_S_GAME_END				102									//��Ϸ����
#define SUB_S_APPLY_BANKER			103									//����ׯ��
#define SUB_S_CHANGE_BANKER			104									//�л�ׯ��
#define SUB_S_CHANGE_USER_SCORE		105									//���»���
#define SUB_S_SEND_RECORD			106									//��Ϸ��¼
#define SUB_S_PLACE_JETTON_FAIL		107									//��עʧ��
#define SUB_S_CANCEL_BANKER			108									//ȡ������
#define SUB_S_AMDIN_COMMAND			109									//����Ա����
#define SUB_S_UPDATE_STORAGE        110									//���¿��
#define SUB_S_SEND_USER_BET_INFO    111									//������ע
#define SUB_S_USER_SCORE_NOTIFY		112									//������ע
//������ׯ
#define SUB_S_SUPERROB_BANKER       113
//������ׯ����뿪
#define SUB_S_CURSUPERROB_LEAVE     114
//ռλ
#define SUB_S_OCCUPYSEAT            115
//ռλʧ��
#define SUB_S_OCCUPYSEAT_FAIL       116
//����ռλ
#define SUB_S_UPDATE_OCCUPYSEAT     117

#define SUB_S_PEIZHI_USER			119								    //�������
#define SUB_S_DelPeizhi				120									//ɾ������
#define SUB_S_UPALLLOSEWIN			121									//���������Ӯ
#define SUB_S_BET_INFO				122									//��ע��Ϣ

struct CMD_S_DelPeizhi
{
	DWORD						dwGameID;
};

struct CMD_S_peizhiVec
{
	//TCHAR						szNickName[32];
	DWORD						dwGameID;
	SCORE						lScore;
};
struct CMD_S_UpAlllosewin
{
	DWORD							dwGameID;						//���GameID
	SCORE							lTotalScore;					//����Ӯ��
	SCORE							lTdTotalScore;					//������Ӯ��
	SCORE							lScore;							//��ҽ��
};
//����ظ�
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//�ظ�����
		 #define ACK_SET_WIN_AREA  1
		 #define ACK_PRINT_SYN     2
		 #define ACK_RESET_CONTROL 3
	BYTE cbResult;
	#define CR_ACCEPT  2			//����
	#define CR_REFUSAL 3			//�ܾ�
	//BYTE cbExtendData[20];			//��������
	int  nControlTimes;				//���ƵĴ���
	BYTE bWinArea[15];				//����Ӯ������
	
};


//���¿��
struct CMD_S_UpdateStorage
{
	BYTE                            cbReqType;						//��������
	SCORE						lStorageStart;				//��ʼ���
	SCORE						lStorageDeduct;				//���˥��
	SCORE						lStorageCurrent;				//��ǰ���
	SCORE						lStorageMax1;					//�������1
	LONGLONG						lStorageMul1;					//ϵͳ��ָ���1
	SCORE						lStorageMax2;					//�������2
	LONGLONG						lStorageMul2;					//ϵͳ��ָ���2
};

//������ע
struct CMD_S_SendUserBetInfo
{
	SCORE						lUserStartScore[GAME_PLAYER];				//��ʼ����
	SCORE						lUserJettonScore[GAME_PLAYER][AREA_MAX];//������ע
};

//ʧ�ܽṹ
struct CMD_S_PlaceBetFail
{
	WORD							wPlaceUser;							//��ע���
	BYTE							lBetArea;							//��ע����
	SCORE						lPlaceScore;						//��ǰ��ע
};

//����ׯ��
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;							//�������
};

//ȡ������
struct CMD_S_CancelBanker
{
	WORD							wCancelUser;						//ȡ�����
};

//�л�ׯ��
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;						//��ׯ���
	SCORE						lBankerScore;						//ׯ�ҷ���
};

//��Ϸ״̬
struct CMD_S_StatusFree
{
	//ȫ����Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	//�����Ϣ
	SCORE						lPlayFreeSocre;						//������ɽ��

	//ׯ����Ϣ
	WORD							wBankerUser;						//��ǰׯ��
	SCORE						lBankerScore;						//ׯ�ҷ���
	SCORE						lBankerWinScore;					//ׯ��Ӯ��
	WORD							wBankerTime;						//ׯ�Ҿ���

	//�Ƿ�ϵͳ��ׯ
	bool							bEnableSysBanker;					//ϵͳ��ׯ

	//������Ϣ
	SCORE						lApplyBankerCondition;				//��������
	SCORE						lAreaLimitScore;					//��������

	BYTE							cbArea[15];
	BYTE							cbControlTimes;
	bool							bIsGameCheatUser;				//�Ƿ��й���Ȩ��

	//������Ϣ
	TCHAR							szGameRoomName[SERVER_LEN];			//��������
	bool							bGenreEducate;						//�Ƿ���ϰ��

	SUPERBANKERCONFIG				superbankerConfig; //��ׯ����
	WORD							wCurSuperRobBankerUser;
	int								typeCurrentBanker;
	tagOccUpYesAtconfig   			occupyseatConfig;
	WORD							wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
	SCORE							lBottomPourImpose;    //��ע����
	tagCustomAndroid				CustomAndroid;						//����������


};
//��ע��Ϣ
struct CMD_S_StatusBetInfo
{
	SCORE							lPlayerBet[AREA_MAX];					//����ÿ����������ע
	SCORE							lAndroidBet[AREA_MAX];					//������ÿ����������ע
	SCORE							lPlayerAreaBet[AREA_MAX];				//����ÿ����ÿ��������ע
	DWORD							dwGameID;								//���id
	SCORE							lPlayerTotleBet;						//ÿ���������ע

};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//ȫ����Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	BYTE							cbGameStatus;						//��Ϸ״̬

	//��ע��
	SCORE							lAllBet[AREA_MAX];					//����ע
	SCORE							lPlayBet[AREA_MAX];					//�����ע

	//��һ���
	SCORE						lPlayBetScore;						//��������ע	
	SCORE						lPlayFreeSocre;						//������ɽ��

	//�����Ӯ
	SCORE						lPlayScore[AREA_MAX];				//�����Ӯ
	SCORE						lPlayAllScore;						//��ҳɼ�
	SCORE						lRevenue;							//˰��
	SCORE						lAllPlayerScore[GAME_PLAYER];		//������ҳɼ�

	//ׯ����Ϣ
	WORD							wBankerUser;						//��ǰׯ��
	SCORE						lBankerScore;						//ׯ�ҷ���
	SCORE						lBankerWinScore;					//ׯ��Ӯ��
	WORD							wBankerTime;						//ׯ�Ҿ���

	//�Ƿ�ϵͳ��ׯ
	bool							bEnableSysBanker;					//ϵͳ��ׯ

	//������Ϣ
	SCORE						lApplyBankerCondition;				//��������
	SCORE						lAreaLimitScore;					//��������

	BYTE							cbArea[15];
	BYTE							cbControlTimes;
	bool							bIsGameCheatUser;				//�Ƿ��й���Ȩ��

	//�˿���Ϣ
 	BYTE							cbCardCount[2];						//�˿���Ŀ
	BYTE							cbTableCardArray[2][2];				//�����˿�

	//������Ϣ
	TCHAR							szGameRoomName[SERVER_LEN];			//��������
	bool							bGenreEducate;						//�Ƿ���ϰ��


	SUPERBANKERCONFIG				superbankerConfig; //��ׯ����
	WORD							wCurSuperRobBankerUser;
	int								typeCurrentBanker;
	tagOccUpYesAtconfig   			occupyseatConfig;
	WORD							wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
	//ռλ��ҳɼ�
	SCORE						lOccupySeatUserWinScore[MAX_OCCUPY_SEAT_COUNT];

	BYTE							cbWinArea[AREA_MAX];					//��Ӯ����
	SCORE							lBottomPourImpose;    //��ע����	
	tagCustomAndroid				CustomAndroid;						//����������	

};

//��Ϸ����
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��
	INT64							nListUserCount;						//�б�����
	SCORE						lStorageStart;						//
	BYTE							cbControl;							//�����Ƿ���Ч
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	WORD							wBankerUser;						//ׯ��λ��
	SCORE						lBankerScore;						//ׯ�ҽ��

	SCORE						lPlayBetScore;						//��������ע	
	SCORE						lPlayFreeSocre;						//������ɽ��
	SCORE							lBottomPourImpose;    //��ע����
	int								nChipRobotCount;					//�������� (��ע������)
	__int64                         nListUserCount;						//�б�����
	int								nAndriodCount;						//�������б�����
};

//�û���ע
struct CMD_S_PlaceBet
{
	WORD							wChairID;							//�û�λ��
	BYTE							cbBetArea;							//��������
	SCORE						lBetScore;							//��ע��Ŀ
	bool							cbAndroidUser;						//������ʶ
	bool							cbAndroidUserT;						//������ʶ
	SCORE						lPlayerAreaBet;						//������������ע
};

//��Ϸ����
struct CMD_S_GameEnd
{
	//�¾���Ϣ
	BYTE							cbTimeLeave;						//ʣ��ʱ��

	//�˿���Ϣ
	BYTE							cbCardCount[2];						//�˿���Ŀ
	BYTE							cbTableCardArray[2][2];				//�����˿�
 
	//ׯ����Ϣ
	SCORE						lBankerScore;						//ׯ�ҳɼ�
	SCORE						lBankerTotallScore;					//ׯ�ҳɼ�
	INT								nBankerTime;						//��ׯ����

	//��ҳɼ�
	SCORE						lPlayScore[AREA_MAX];				//��ҳɼ�
	SCORE						lPlayAllScore;						//��ҳɼ�
	SCORE						lAllPlayerScore[GAME_PLAYER];		//������ҳɼ�

	//ȫ����Ϣ
	SCORE						lRevenue;							//��Ϸ˰��

	//ռλ��ҳɼ�
	SCORE lOccupySeatUserWinScore[MAX_OCCUPY_SEAT_COUNT];
	BYTE							cbWinArea[AREA_MAX];					//��Ӯ����
};

struct CMD_S_UserScoreNotify
{
	WORD							wChairID;							//���ID
	//��һ���
	SCORE						lPlayBetScore;						//��������ע
};
//ռλ
struct CMD_S_OccupySeat
{
	//����ռλ���id
	WORD			wOccupySeatChairID;
	//ռλ����
	BYTE			cbOccupySeatIndex;
	//ռλ����id
	WORD			tabWOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
};
//����ռλ
struct CMD_S_UpdateOccupySeat
{
	//ռλ����id
	WORD tabWOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
	//�����˳�ռλ���
	WORD wQuitOccupySeatChairID;
};
//ռλʧ��
struct CMD_S_OccupySeat_Fail
{
	//������ռλ���ID
	WORD wAlreadyOccupySeatChairID;
	//��ռλ����
	BYTE cbAlreadyOccupySeatIndex;
	//ռλ����id
	WORD tabWOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_PLACE_JETTON			1									//�û���ע
#define SUB_C_APPLY_BANKER			2									//����ׯ��
#define SUB_C_CANCEL_BANKER			3									//ȡ������
#define SUB_C_AMDIN_COMMAND			4									//����Ա����
#define SUB_C_UPDATE_STORAGE        5									//���¿��
#define SUB_C_SUPERROB_BANKER       6									//������ׯ
#define SUB_C_OCCUPYSEAT            7									//ռλ
#define SUB_C_QUIT_OCCUPYSEAT       8									//�˳�ռλ   

#define SUB_C_PEIZHI_USER			10								    //�������
#define SUB_C_DelPeizhi			    11									//ɾ������

#define RQ_SET_WIN_AREA	1
#define RQ_RESET_CONTROL	2
#define RQ_PRINT_SYN		3
struct CMD_C_AdminReq
{
	BYTE cbReqType;
	int  nControlTimes;				//���ƵĴ���
	BYTE bWinArea[15];				//����Ӯ������

};
//ռλ
struct CMD_C_OccupySeat
{
	//ռλ���
	WORD wOccupySeatChairID;
	//ռλ����
	BYTE cbOccupySeatIndex;
};
//�û���ע
struct CMD_C_PlaceBet
{
	
	BYTE							cbBetArea;						//��������
	SCORE						lBetScore;						//��ע��Ŀ
};
struct CMD_C_peizhiVec
{
	DWORD						dwGameID;
};

struct CMD_C_DelPeizhi
{
	DWORD						dwGameID;
};

//���¿��
struct CMD_C_UpdateStorage
{
	BYTE                            cbReqType;						//��������
	SCORE						lStorageDeduct;					//���˥��
	SCORE						lStorageCurrent;				//��ǰ���
	SCORE						lStorageMax1;					//�������1
	LONGLONG						lStorageMul1;					//ϵͳ��ָ���1
	SCORE						lStorageMax2;					//�������2
	LONGLONG						lStorageMul2;					//ϵͳ��ָ���2
};

//��ԭ������
#pragma pack()
//////////////////////////////////////////////////////////////////////////

#endif
