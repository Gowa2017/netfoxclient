#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID           109                 //��Ϸ I D
#define GAME_NAME         _TEXT("�����ƾ�")           //��Ϸ����
#define GAME_PLAYER         MAX_CHAIR             //��Ϸ����

//�汾��Ϣ
#define VERSION_SERVER          PROCESS_VERSION(7,0,1)        //����汾
#define VERSION_CLIENT        PROCESS_VERSION(7,0,1)        //����汾

//״̬����
#define GAME_SCENE_FREE       GAME_STATUS_FREE          //�ȴ���ʼ
#define GS_PLACE_JETTON       GAME_STATUS_PLAY          //��ע״̬
#define GS_GAME_END         GAME_STATUS_PLAY+1          //����״̬

//��������
#define ID_SHUN_MEN         1                 //˳��
#define ID_JIAO_L			2                 //��߽�
#define ID_QIAO				3                 //��
#define ID_DUI_MEN          4                 //����
#define ID_DAO_MEN          5                 //����
#define ID_JIAO_R			6                 //�ұ߽�

//�������
#define BANKER_INDEX        0                 //ׯ������
#define SHUN_MEN_INDEX      1                 //˳������
#define DUI_MEN_INDEX       2                 //��������
#define DAO_MEN_INDEX       3                 //��������
#define MAX_INDEX			3                 //�������

#define AREA_COUNT          6                 //������Ŀ
#define CONTROL_AREA        3                 //�ܿ�����

//���ʶ���
#define RATE_TWO_PAIR       12                  //��������
#define SERVER_LEN          32                  //���䳤��

#define MAX_CARD			2
#define MAX_CARDGROUP       4

//�궨��
#ifndef _UNICODE
#define myprintf  _snprintf
#define mystrcpy  strcpy
#define mystrlen  strlen
#define myscanf   _snscanf
#define myLPSTR   LPCSTR
#else
#define myprintf  swprintf
#define mystrcpy  wcscpy
#define mystrlen  wcslen
#define myscanf   _snwscanf
#define myLPSTR   LPWSTR
#endif

//-----M----------------------------------------------------------
#define MAX_OCCUPY_SEAT_COUNT       4                                 //���ռλ����

//ռλ����
#define OCCUPYSEAT_VIPTYPE  0          //��Առλ
#define OCCUPYSEAT_CONSUMETYPE  1      //���Ľ��ռλ
#define OCCUPYSEAT_FREETYPE  2         //���ռλ
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

//������ׯ
struct CMD_S_SuperRobBanker
{
	bool bSucceed;
	WORD              wApplySuperRobUser;             //�������
	WORD              wCurSuperRobBankerUser;         //��ǰ���
};

//������ׯ����뿪
struct CMD_S_CurSuperRobLeave
{
	WORD              wCurSuperRobBankerUser;
};

//-----M----------------------------------------------------------


struct tagCustomAndroid
{
  //��ׯ
  BOOL              nEnableRobotBanker;			//�Ƿ���ׯ
  LONGLONG            lRobotBankerCountMin;     //��ׯ����
  LONGLONG            lRobotBankerCountMax;     //��ׯ����
  LONGLONG            lRobotListMinCount;       //�б�����
  LONGLONG            lRobotListMaxCount;       //�б�����
  LONGLONG            lRobotApplyBanker;        //����������
  LONGLONG            lRobotWaitBanker;			//��������

  //��ע
  LONGLONG            lRobotMinBetTime;			//��ע�������
  LONGLONG            lRobotMaxBetTime;			//��ע�������
  SCORE            lRobotMinJetton;				//��ע������
  SCORE            lRobotMaxJetton;				//��ע������
  LONGLONG            lRobotBetMinCount;        //��ע��������
  LONGLONG            lRobotBetMaxCount;        //��ע��������
  SCORE            lRobotAreaLimit;				//��������

  //��ȡ��
  SCORE            lRobotScoreMin;				//�������
  SCORE            lRobotScoreMax;				//�������
  SCORE            lRobotBankGetMin;			//ȡ����Сֵ(��ׯ)
  SCORE            lRobotBankGetMax;			//ȡ�����ֵ(��ׯ)
  SCORE            lRobotBankGetBankerMin;      //ȡ����Сֵ(��ׯ)
  SCORE            lRobotBankGetBankerMax;      //ȡ�����ֵ(��ׯ)
  LONGLONG            lRobotBankStoMul;         //���ٷֱ�

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
    lRobotMaxJetton = 1000;
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
  }
};

//��������Ϣ
struct tagRobotInfo
{
  int nChip[7];                           //���붨��
  int nAreaChance[AREA_COUNT];                    //������
  int nMaxTime;                           //�������

  tagRobotInfo()
  {
    int nTmpChip[7] = {1, 5, 10, 50, 100, 500, 1000};
    //int nTmpAreaChance[AREA_COUNT] = {2, 1, 1, 2, 2, 1};
	int nTmpAreaChance[AREA_COUNT] = {1, 1, 1, 1, 1, 1};

    nMaxTime = 1;
    memcpy(nChip, nTmpChip, sizeof(nChip));
    memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
  }
};

//��¼��Ϣ
struct tagServerGameRecord
{
  bool              bWinShunMen;            //˳��ʤ��
  bool              bWinDuiMen;             //����ʤ��
  bool              bWinDaoMen;             //����ʤ��
};

//��¼������Ϣ
struct tagServerCardRecord
{
	BYTE              m_card1;              //��ֵ1
	BYTE              m_card2;              //��ֵ2
	BYTE              m_card3;              //��ֵ3
	BYTE              m_card4;              //��ֵ4
	BYTE              m_card5;              //��ֵ5
	BYTE              m_card6;              //��ֵ6
	BYTE              m_card7;              //��ֵ7
	BYTE              m_card8;              //��ֵ8

};

//��ע��Ϣ
struct tagUserBet
{
  TCHAR             szNickName[32];				 //�û��ǳ�
  DWORD             dwUserGameID;				 //�û�ID
  SCORE            lUserStartScore;				 //�û����
  SCORE            lUserWinLost;				 //�û����
  SCORE            lUserBet[AREA_COUNT+1];       //�û���ע
};

//��ע��Ϣ����
typedef CWHArray<tagUserBet,tagUserBet&> CUserBetArray;

//������
#define RQ_REFRESH_STORAGE    1
#define RQ_SET_STORAGE      2
//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_FREE					99                  //��Ϸ����
#define SUB_S_GAME_START				100                 //��Ϸ��ʼ
#define SUB_S_PLACE_JETTON				101                 //�û���ע
#define SUB_S_GAME_END					102                 //��Ϸ����
#define SUB_S_APPLY_BANKER				103                 //����ׯ��
#define SUB_S_CHANGE_BANKER				104                 //�л�ׯ��
#define SUB_S_CHANGE_USER_SCORE			105                 //���»���
#define SUB_S_SEND_RECORD				106                 //��Ϸ��¼
#define SUB_S_PLACE_JETTON_FAIL			107                 //��עʧ��
#define SUB_S_CANCEL_BANKER				108                 //ȡ������
#define SUB_S_CHEAT						109                 //������Ϣ

#define SUB_S_AMDIN_COMMAND				110                 //����Ա����
#define SUB_S_UPDATE_STORAGE			111                 //���¿��
#define SUB_S_SEND_USER_BET_INFO		112                 //������ע

//--------------M-------------------------------------------------------------------
#define SUB_S_ADVANCE_OPENCARD			113								//��ǰ����
#define SUB_S_SUPERROB_BANKER			114								//������ׯ
#define SUB_S_CURSUPERROB_LEAVE			115								//������ׯ����뿪

#define SUB_S_OCCUPYSEAT				116								//ռλ
#define SUB_S_OCCUPYSEAT_FAIL			117								//ռλʧ��
#define SUB_S_UPDATE_OCCUPYSEAT			118								//����ռλ
#define SUB_S_PEIZHI_USER				119								//�������
#define SUB_S_DelPeizhi					120								//����������
#define SUB_S_UPALLLOSEWIN				121								//���������Ӯ
#define SUB_S_SEND_CARDRECORD			122								//���Ƽ�¼

struct CMD_S_DelPeizhi
{
	DWORD						dwGameID;
};

struct CMD_S_peizhiVec
{
	TCHAR						szNickName[32];
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
//--------------M-------------------------------------------------------------------

//����ظ�
struct CMD_S_CommandResult
{
  BYTE cbAckType;         //�ظ�����
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3
  BYTE cbResult;
#define CR_ACCEPT  2      //����
#define CR_REFUSAL 3      //�ܾ�
  BYTE cbExtendData[20];      //��������
};

//���¿��
struct CMD_S_UpdateStorage
{
  BYTE                            cbReqType;            //��������
  SCORE            lStorageStart;        //��ʼ���
  SCORE            lStorageDeduct;       //���˥��
  SCORE            lStorageCurrent;        //��ǰ���
  SCORE            lStorageMax1;         //�������1
  LONGLONG            lStorageMul1;         //ϵͳ��ָ���1
  SCORE            lStorageMax2;         //�������2
  LONGLONG            lStorageMul2;         //ϵͳ��ָ���2
};

//������ע
struct CMD_S_SendUserBetInfo
{
  SCORE            lUserStartScore[GAME_PLAYER];       //��ʼ����
  SCORE            lUserJettonScore[AREA_COUNT+1][GAME_PLAYER];//������ע
};

//ʧ�ܽṹ
struct CMD_S_PlaceJettonFail
{
  WORD              wPlaceUser;             //��ע���
  BYTE              lJettonArea;            //��ע����
  LONGLONG            lPlaceScore;            //��ǰ��ע
};

//���»���
struct CMD_S_ChangeUserScore
{
  WORD              wChairID;				//���Ӻ���
  SCORE              lScore;               //��һ���

  //ׯ����Ϣ
  WORD              wCurrentBankerChairID;          //��ǰׯ��
  BYTE              cbBankerTime;					//ׯ�Ҿ���
  SCORE              lCurrentBankerScore;          //ׯ�ҷ���
};

//����ׯ��
struct CMD_S_ApplyBanker
{
  WORD              wApplyUser;             //�������
};

//ȡ������
struct CMD_S_CancelBanker
{
  WORD              wCancelUser;          //ȡ�����
};

//�л�ׯ��
struct CMD_S_ChangeBanker
{
  WORD              wBankerUser;            //��ׯ���
  SCORE            lBankerScore;           //ׯ�ҽ��
};

//��Ϸ״̬
struct CMD_S_StatusFree
{
  //ȫ����Ϣ
  BYTE              cbTimeLeave;					//ʣ��ʱ��

  //�����Ϣ
  SCORE            lUserMaxScore;				//��ҽ��

  //ׯ����Ϣ
  WORD                wBankerUser;					//��ǰׯ��
  WORD                cbBankerTime;					//ׯ�Ҿ���
  SCORE            lBankerWinScore;				//ׯ�ҳɼ�
  SCORE            lBankerScore;					//ׯ�ҷ���
  INT                 nEndGameMul;					//��ǰ���ưٷֱ�
  bool                bEnableSysBanker;				//ϵͳ��ׯ
  WORD				  cbCardRecordCount;			//��������--MXM

  //������Ϣ
  SCORE            lApplyBankerCondition;        //��������
  SCORE            lAreaLimitScore;				//��������
  
  BYTE							cbArea[6];
  BYTE							cbControlTimes;
  bool							bIsGameCheatUser;				//�Ƿ��й���Ȩ��

  //������Ϣ
  TCHAR					szGameRoomName[SERVER_LEN];  //��������
  bool					bGenreEducate;				 //��ϰģʽ

  SUPERBANKERCONFIG		superbankerConfig;			 //��ׯ����-MXM
  WORD					wCurSuperRobBankerUser;		 //-MXM
  int					typeCurrentBanker;			 //ׯ������-MXM
  tagOccUpYesAtconfig   		occupyseatConfig;	//-ռλ����MXM
  WORD							wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];//ռλ����-MXM
  SCORE							lBottomPourImpose;    //��ע����//-MXM
  tagCustomAndroid      CustomAndroid;               //����������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
  //ȫ����ע
  SCORE            lAllJettonScore[AREA_COUNT+1];    //ȫ����ע

  //�����ע
  SCORE            lUserJettonScore[AREA_COUNT+1];   //������ע

  //��һ���
  SCORE            lUserMaxScore;				//�����ע

  //������Ϣ
  SCORE            lApplyBankerCondition;        //��������
  SCORE            lAreaLimitScore;				//��������
  
  BYTE							cbArea[6];
  BYTE							cbControlTimes;
  bool							bIsGameCheatUser;				//�Ƿ��й���Ȩ��

  //�˿���Ϣ
  BYTE              cbTableCardArray[4][2];       //�����˿�

  //ׯ����Ϣ
  WORD              wBankerUser;				//��ǰׯ��
  WORD              cbBankerTime;				//ׯ�Ҿ���
  SCORE            lBankerWinScore;          //ׯ��Ӯ��
  SCORE            lBankerScore;				//ׯ�ҷ���
  INT               nEndGameMul;				//��ǰ���ưٷֱ�
  bool              bEnableSysBanker;			//ϵͳ��ׯ
  WORD              cbCardRecordCount;			//��������--MXM

  //������Ϣ
  SCORE            lEndBankerScore;          //ׯ�ҳɼ�
  SCORE            lEndUserScore;            //��ҳɼ�
  SCORE            lEndUserReturnScore;      //���ػ���
  SCORE            lEndRevenue;				//��Ϸ˰��

  //ȫ����Ϣ
  BYTE              cbTimeLeave;				//ʣ��ʱ��
  BYTE              cbGameStatus;				//��Ϸ״̬

  //������Ϣ
  TCHAR             szGameRoomName[SERVER_LEN];     //��������
  bool              bGenreEducate;					//��ϰģʽ

  SUPERBANKERCONFIG		superbankerConfig;			 //��ׯ����-MXM
  WORD					wCurSuperRobBankerUser;		 //-MXM
  int					typeCurrentBanker;			 //ׯ������-MXM
  tagOccUpYesAtconfig   		occupyseatConfig;    //ռλ����-MXM
  WORD							wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];//ռλ����-MXM
  //ռλ��ҳɼ�
  SCORE						lOccupySeatUserWinScore[MAX_OCCUPY_SEAT_COUNT];//-MXM
  SCORE							lBottomPourImpose;    //��ע����//-MXM
  tagCustomAndroid        CustomAndroid;            //����������
};

//��Ϸ����
struct CMD_S_GameFree
{
  BYTE              cbTimeLeave;					//ʣ��ʱ��
  WORD              wCurrentBanker;					//��ǰׯ��
  INT               nBankerTime;					//��ׯ����
  INT64             nListUserCount;					//�б�����
  SCORE          lStorageStart;						//�����ֵ
  INT				nCardRecordCount;				//��������--MXM
  BYTE				cbControl;						//�����Ƿ���Ч--MXM
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
  WORD              wBankerUser;				//ׯ��λ��
  SCORE            lBankerScore;				//ׯ�ҽ��
  SCORE            lUserMaxScore;            //�ҵĽ��
  BYTE              cbTimeLeave;				//ʣ��ʱ��
  bool              bContiueCard;				//��������
  int               nChipRobotCount;            //�������� (��ע������)
  int               nAndriodApplyCount;         //�������б�����
  SCORE				lBottomPourImpose;			//��ע����-MXM
};

//�û���ע
struct CMD_S_PlaceJetton
{
  WORD              wChairID;				//�û�λ��
  BYTE              cbJettonArea;           //��������
  SCORE          lJettonScore;           //��ע��Ŀ
  bool              bIsAndroid;             //�Ƿ������
  bool              bAndroid;				//������ʶ
  SCORE				lAreaAllJetton;			//��������ע
  SCORE				lAllJetton;				//����ע
};

//��Ϸ����
struct CMD_S_GameEnd
{
  //�¾���Ϣ
  BYTE              cbTimeLeave;					//ʣ��ʱ��

  //�˿���Ϣ
  BYTE              cbTableCardArray[4][2];       //�����˿�
  BYTE              cbLeftCardCount;			  //�˿���Ŀ

  BYTE              bcFirstCard;
  BYTE              bcNextCard;

  //ׯ����Ϣ
  WORD              wCurrentBanker;				//��ǰׯ��
  SCORE          lBankerScore;				//ׯ�ҳɼ�
  SCORE          lBankerTotallScore;         //ׯ�ҳɼ�
  INT               nBankerTime;				//��ׯ����
  INT				nCardRecordCount;			//��������

  //��ҳɼ�
  SCORE            lUserScore;				//��ҳɼ�
  SCORE            lUserReturnScore;         //���ػ���

  //ȫ����Ϣ
  SCORE            lRevenue;					//��Ϸ˰��
  SCORE			  lOccupySeatUserWinScore[6];//ռλ��ҳɼ�--MXM
};

//��Ϸ����
struct CMD_S_Cheat
{
  BYTE              cbTableCardArray[4][2];       //�����˿�
};

//----------M-------------------------------------------------------
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
//ռλ
struct CMD_C_OccupySeat
{
	//ռλ���
	WORD wOccupySeatChairID;
	//ռλ����
	BYTE cbOccupySeatIndex;
};

struct CMD_C_peizhiVec
{
	DWORD						dwGameID;
};

struct CMD_C_DelPeizhi
{
	DWORD						dwGameID;
};
//----------M-------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_PLACE_JETTON      1                 //�û���ע
#define SUB_C_APPLY_BANKER      2                 //����ׯ��
#define SUB_C_CANCEL_BANKER     3                 //ȡ������
#define SUB_C_CONTINUE_CARD     4                 //��������
#define SUB_C_AMDIN_COMMAND     5                 //����Ա����
#define SUB_C_UPDATE_STORAGE        6                 //���¿��

//----------M-------------------------------------------------------
#define SUB_C_SUPERROB_BANKER		7						//������ׯ
#define SUB_C_OCCUPYSEAT			8						//ռλ
#define SUB_C_QUIT_OCCUPYSEAT		9						//�˳�ռλ

#define SUB_C_PEIZHI_USER			10								    //�������
#define SUB_C_DelPeizhi			    11									//ɢɾ������
#define SUB_C_ANDROIDXIAZHUANG	    12					//��������ׯ
//----------M-------------------------------------------------------

//�ͻ�����Ϣ
#define IDM_ADMIN_COMMDN      WM_USER+1000
#define IDM_UPDATE_STORAGE      WM_USER+1001


//����������Ϣ
struct tagControlInfo
{
  BYTE cbControlArea[MAX_INDEX];      //��������
};

struct tagAdminReq
{
  BYTE              m_cbExcuteTimes;          //ִ�д���
  BYTE              m_cbControlStyle;         //���Ʒ�ʽ
#define   CS_BANKER_LOSE    1
#define   CS_BANKER_WIN   2
#define   CS_BET_AREA     3
  bool              m_bWinArea[3];            //Ӯ������
};


struct CMD_C_AdminReq
{
  BYTE cbReqType;
#define RQ_SET_WIN_AREA  1
#define RQ_RESET_CONTROL 2
#define RQ_PRINT_SYN   3
  BYTE cbExtendData[20];      //��������
};

//�û���ע
struct CMD_C_PlaceJetton
{
  BYTE              cbJettonArea;           //��������
  SCORE            lJettonScore;           //��ע��Ŀ
};

//���¿��
struct CMD_C_UpdateStorage
{
  BYTE                            cbReqType;            //��������
  SCORE            lStorageDeduct;         //���˥��
  SCORE            lStorageCurrent;        //��ǰ���
  SCORE            lStorageMax1;         //�������1
  LONGLONG            lStorageMul1;         //ϵͳ��ָ���1
  SCORE            lStorageMax2;         //�������2
  LONGLONG            lStorageMul2;         //ϵͳ��ָ���2
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
