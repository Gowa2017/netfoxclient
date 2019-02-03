#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

#define UR_GAME_CONTROL         0x20000000L       //��Ϸ�������
//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID           104                 //��Ϸ I D
#define GAME_NAME         TEXT("����ţţ")          //��Ϸ����

//�������
#define GAME_PLAYER         MAX_CHAIR                 //��Ϸ����
#define VERSION_SERVER        PROCESS_VERSION(7,0,1)        //����汾
#define VERSION_CLIENT        PROCESS_VERSION(7,0,1)        //����汾

//////////////////////////////////////////////////////////////////////////////////
//״̬����

#define GAME_SCENE_FREE       GAME_STATUS_FREE          //�ȴ���ʼ
#define GAME_SCENE_PLACE_JETTON   GAME_STATUS_PLAY          //��ע״̬
#define GAME_SCENE_GAME_END     GAME_STATUS_PLAY+1          //����״̬


//��������
#define ID_TIAN_MEN         1                 //��
#define ID_DI_MEN         2                 //��
#define ID_XUAN_MEN         3                 //��
#define ID_HUANG_MEN        4                 //��

//�������
#define BANKER_INDEX        0                 //ׯ������
#define SHUN_MEN_INDEX        1                 //˳������
#define DUI_MEN_INDEX       2                 //��������
#define DAO_MEN_INDEX       3                 //��������
#define HUAN_MEN_INDEX        4                 //��������
#define MAX_INDEX         3                 //�������

#define AREA_COUNT          4                 //������Ŀ
#define CONTROL_AREA        4

//���ʶ���
#define RATE_TWO_PAIR       12                  //��������
#define SERVER_LEN          32                  //���䳤��
#define MAX_OCCUPY_SEAT_COUNT       6                                 //���ռλ����
////////////////////////////////////////////////////////////////////////////////////
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
	LONGLONG lOccupySeatConsume;
	//���ռλ�������
	SCORE lOccupySeatFree;
	//ǿ��վ������
	SCORE lForceStandUpCondition;
};
struct tagCustomAndroid
{
  //��ׯ
  BOOL              nEnableRobotBanker;       //�Ƿ���ׯ
  LONGLONG            lRobotBankerCountMin;     //��ׯ����
  LONGLONG            lRobotBankerCountMax;     //��ׯ����
  LONGLONG            lRobotListMinCount;       //�б�����
  LONGLONG            lRobotListMaxCount;       //�б�����
  LONGLONG            lRobotApplyBanker;        //����������
  LONGLONG            lRobotWaitBanker;       //��������

  //��ע
  LONGLONG            lRobotMinBetTime;       //��ע�������
  LONGLONG            lRobotMaxBetTime;       //��ע�������
  SCORE				  lRobotMinJetton;        //��ע������
  SCORE				  lRobotMaxJetton;        //��ע������
  LONGLONG            lRobotBetMinCount;        //��ע��������
  LONGLONG            lRobotBetMaxCount;        //��ע��������
  SCORE				  lRobotAreaLimit;        //��������

  //��ȡ��
  SCORE		         lRobotScoreMin;         //�������
  SCORE				 lRobotScoreMax;         //�������
  SCORE              lRobotBankGetMin;       //ȡ����Сֵ(��ׯ)
  SCORE              lRobotBankGetMax;       //ȡ�����ֵ(��ׯ)
  SCORE              lRobotBankGetBankerMin;     //ȡ����Сֵ(��ׯ)
  SCORE              lRobotBankGetBankerMax;     //ȡ�����ֵ(��ׯ)
  LONGLONG           lRobotBankStoMul;       //���ٷֱ�

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
  TCHAR szCfgFileName[MAX_PATH];                    //�����ļ�
  int nMaxTime;                           //�������

  tagRobotInfo()
  {
    int nTmpChip[7] = {1, 5, 10, 50, 100, 500, 1000};
    int nTmpAreaChance[AREA_COUNT] = {1, 1, 1, 1};

    nMaxTime = 10;
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
  bool              bWinHuang;              //����ʤ��
};

//��ע��Ϣ
struct tagUserBet
{
  TCHAR             szNickName[32];           //�û��ǳ�
  DWORD             dwUserGameID;           //�û�ID
  SCORE            lUserStartScore;          //�û����
  SCORE            lUserWinLost;           //�û����
  SCORE            lUserBet[AREA_COUNT+1];       //�û���ע
};

//��ע��Ϣ����
typedef CWHArray<tagUserBet,tagUserBet&> CUserBetArray;

//������
#define RQ_REFRESH_STORAGE    1
#define RQ_SET_STORAGE      2
//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_FREE       99                  //��Ϸ����
#define SUB_S_GAME_START      100                 //��Ϸ��ʼ
#define SUB_S_PLACE_JETTON      101                 //�û���ע
#define SUB_S_GAME_END        102                 //��Ϸ����
#define SUB_S_APPLY_BANKER      103                 //����ׯ��
#define SUB_S_CHANGE_BANKER     104                 //�л�ׯ��
#define SUB_S_CHANGE_USER_SCORE   105                 //���»���
#define SUB_S_SEND_RECORD     106                 //��Ϸ��¼
#define SUB_S_PLACE_JETTON_FAIL   107                 //��עʧ��
#define SUB_S_CANCEL_BANKER     108                 //ȡ������


#define SUB_S_AMDIN_COMMAND     110                 //����Ա����
#define SUB_S_UPDATE_STORAGE        111                 //���¿��
#define SUB_S_SEND_USER_BET_INFO    112                 //������ע

#define SUB_S_ADVANCE_OPENCARD		113								//��ǰ����
#define SUB_S_SUPERROB_BANKER		114								//������ׯ
#define SUB_S_CURSUPERROB_LEAVE		115								//������ׯ����뿪

#define SUB_S_OCCUPYSEAT            116								//ռλ
#define SUB_S_OCCUPYSEAT_FAIL       117								//ռλʧ��
#define SUB_S_UPDATE_OCCUPYSEAT     118								//����ռλ
#define SUB_S_PEIZHI_USER			119								    //�������
#define SUB_S_DelPeizhi				120									//����������
#define SUB_S_UPALLLOSEWIN			121									//���������Ӯ
struct CMD_S_DelPeizhi
	{
	DWORD						dwGameID;
};

struct CMD_S_peizhiVec
{
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
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3

#define CR_ACCEPT  2      //����
#define CR_REFUSAL 3      //�ܾ�
//����ظ�
struct CMD_S_CommandResult
{
  BYTE cbAckType;         //�ظ�����
  BYTE cbResult;
  BYTE cbExtendData[20];      //��������
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
  WORD              wChairID;             //���Ӻ���
  SCORE              lScore;               //��һ���

  //ׯ����Ϣ
  WORD              wCurrentBankerChairID;        //��ǰׯ��
  BYTE              cbBankerTime;           //ׯ�Ҿ���
  SCORE              lCurrentBankerScore;        //ׯ�ҷ���
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
  BYTE							cbTimeLeave;				//ʣ��ʱ��

  //�����Ϣ
  SCORE						lUserMaxScore;				//��ҽ��

  //ׯ����Ϣ
  WORD							wBankerUser;				//��ǰׯ��
  WORD							cbBankerTime;				//ׯ�Ҿ���
  SCORE						lBankerWinScore;			//ׯ�ҳɼ�
  SCORE						lBankerScore;				//ׯ�ҷ���
  INT							nEndGameMul;				//��ǰ���ưٷֱ�
  bool							bEnableSysBanker;			//ϵͳ��ׯ

  BYTE							cbArea[4];
  BYTE							cbControlStyle;
  bool						    bControlLimits;				//����Ȩ��	
  //������Ϣ
  SCORE						lApplyBankerCondition;       //��������
  SCORE						lAreaLimitScore;			 //��������

  //������Ϣ
  TCHAR							szGameRoomName[SERVER_LEN];     //��������
  bool							bGenreEducate;				//��ϰģʽ

  SUPERBANKERCONFIG				superbankerConfig; //��ׯ����
  WORD							wCurSuperRobBankerUser;
  int							typeCurrentBanker;
  tagOccUpYesAtconfig   		occupyseatConfig;
  WORD							wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
  SCORE							lBottomPourImpose;    //��ע����
  tagCustomAndroid      		CustomAndroid;            //����������

};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
  //ȫ����ע
  SCORE            lAllJettonScore[AREA_COUNT+1];    //ȫ����ע

  //�����ע
  SCORE            lUserJettonScore[AREA_COUNT+1];   //������ע

  //��һ���
  SCORE            lUserMaxScore;            //�����ע

  //������Ϣ
  SCORE            lApplyBankerCondition;        //��������
  SCORE            lAreaLimitScore;          //��������
  BYTE							cbArea[4];
  BYTE							cbControlStyle;
  bool						    bControlLimits;				//����Ȩ��	
  //�˿���Ϣ
  BYTE              cbTableCardArray[5][5];       //�����˿�

  //ׯ����Ϣ
  WORD              wBankerUser;            //��ǰׯ��
  WORD              cbBankerTime;           //ׯ�Ҿ���
  SCORE            lBankerWinScore;          //ׯ��Ӯ��
  SCORE            lBankerScore;           //ׯ�ҷ���
  INT               nEndGameMul;            //��ǰ���ưٷֱ�
  bool              bEnableSysBanker;         //ϵͳ��ׯ

  //������Ϣ
  SCORE            lEndBankerScore;          //ׯ�ҳɼ�
  SCORE            lEndUserScore;            //��ҳɼ�
  SCORE            lEndUserReturnScore;        //���ػ���
  SCORE            lEndRevenue;            //��Ϸ˰��

  //ȫ����Ϣ
  BYTE              cbTimeLeave;            //ʣ��ʱ��
  BYTE              cbGameStatus;           //��Ϸ״̬

  //������Ϣ
  TCHAR             szGameRoomName[SERVER_LEN];     //��������
  bool              bGenreEducate;            //��ϰģʽ

  SUPERBANKERCONFIG				superbankerConfig; //��ׯ����
  WORD							wCurSuperRobBankerUser;
  int							typeCurrentBanker;
  tagOccUpYesAtconfig   		occupyseatConfig;
  WORD							wOccupySeatChairID[MAX_OCCUPY_SEAT_COUNT];
  //ռλ��ҳɼ�
  SCORE						lOccupySeatUserWinScore[MAX_OCCUPY_SEAT_COUNT];
  SCORE							lBottomPourImpose;    //��ע����
  tagCustomAndroid      		CustomAndroid;            //����������

};

//��Ϸ����
struct CMD_S_GameFree
{
  BYTE							cbTimeLeave;				//ʣ��ʱ��
  INT64                         nListUserCount;				//�б�����
  SCORE						lStorageStart;				//
  BYTE							cbControl;					//�����Ƿ���Ч
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
  WORD              wBankerUser;            //ׯ��λ��
  SCORE            lBankerScore;           //ׯ�ҽ��
  SCORE            lUserMaxScore;            //�ҵĽ��
  BYTE              cbTimeLeave;            //ʣ��ʱ��
  bool              bContiueCard;           //��������
  int               nChipRobotCount;          //�������� (��ע������)
  int               nAndriodApplyCount;         //�������б�����
  SCORE				lBottomPourImpose;    //��ע����
};

//�û���ע
struct CMD_S_PlaceJetton
{
  WORD              wChairID;             //�û�λ��
  BYTE              cbJettonArea;           //��������
  SCORE            lJettonScore;           //��ע��Ŀ
  bool              bIsAndroid;             //�Ƿ������
  bool              bAndroid;           //������ʶ
  SCORE				lAreaAllJetton;		//��������ע
   SCORE			lAllJetton;			//����ע
};

//��Ϸ����
struct CMD_S_GameEnd
{
  //�¾���Ϣ
  BYTE              cbTimeLeave;            //ʣ��ʱ��

  //�˿���Ϣ
  BYTE              cbTableCardArray[5][5];       //�����˿�
  BYTE              cbLeftCardCount;          //�˿���Ŀ

  BYTE              bcFirstCard;

  //ׯ����Ϣ
  SCORE            lBankerScore;           //ׯ�ҳɼ�
  SCORE            lBankerTotallScore;         //ׯ�ҳɼ�
  INT               nBankerTime;            //��ׯ����

  //��ҳɼ�
  SCORE            lUserScore;             //��ҳɼ�
  SCORE            lUserReturnScore;         //���ػ���

  //ȫ����Ϣ
  SCORE            lRevenue;             //��Ϸ˰��
  SCORE			  lOccupySeatUserWinScore[6];
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_PLACE_JETTON      1                 //�û���ע
#define SUB_C_APPLY_BANKER      2                 //����ׯ��
#define SUB_C_CANCEL_BANKER     3                 //ȡ������
#define SUB_C_CONTINUE_CARD     4                 //��������
#define SUB_C_AMDIN_COMMAND     5                 //����Ա����
#define SUB_C_UPDATE_STORAGE        6                 //���¿��

#define SUB_C_SUPERROB_BANKER		7						//������ׯ
#define SUB_C_OCCUPYSEAT			8						//ռλ
#define SUB_C_QUIT_OCCUPYSEAT		9						//�˳�ռλ

#define SUB_C_PEIZHI_USER			10								    //�������
#define SUB_C_DelPeizhi			    11									//ɢɾ������
#define SUB_C_ANDROIDXIAZHUANG	    12					//��������ׯ
//�ͻ�����Ϣ
#define IDM_ADMIN_COMMDN      WM_USER+1000
#define IDM_UPDATE_STORAGE      WM_USER+1001

//����������Ϣ
struct tagControlInfo
{
  BYTE cbControlArea[MAX_INDEX];      //��������
};
#define   CS_BANKER_LOSE    1
#define   CS_BANKER_WIN   2
#define   CS_BET_AREA     3

struct tagAdminReq
{
  BYTE              m_cbExcuteTimes;          //ִ�д���
  BYTE              m_cbControlStyle;         //���Ʒ�ʽ
  bool              m_bWinArea[4];            //Ӯ������
};
#define RQ_SET_WIN_AREA   1
#define RQ_RESET_CONTROL  2
#define RQ_PRINT_SYN    3

struct CMD_C_AdminReq
{
  BYTE cbReqType;
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
//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif