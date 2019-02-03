#ifndef CMD_HK_FIVE_CARD_HEAD_FILE
#define CMD_HK_FIVE_CARD_HEAD_FILE


#pragma pack(push)  
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID							6									//��Ϸ I D
#define GAME_PLAYER						7									//��Ϸ����
#define GAME_NAME						TEXT("թ��")						//��Ϸ����
#define MAX_COUNT						3									//�˿���Ŀ
#define VERSION_SERVER			    	PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				    PROCESS_VERSION(7,0,1)				//����汾

//��Ϸ����
#define GIVE_UP							2									//��������
#define LOOK_CARD						8									//���Ƹ���
#define COMPARE_CARD					4									//���Ƹ���
#define RAISE_SCORE						2									//���ּ�ע����
#define MAX_LOOK_INDEX					16									//��������
#define MAX_OPEN_INDEX					5									//��������

//����ԭ��
#define GER_NO_PLAYER					0x10								//û�����
#define GER_COMPARECARD					0x20								//���ƽ���
#define GER_OPENCARD					0x30								//���ƽ���

#define LEN_NICKNAME				32									//�ǳƳ���
#define    CHEAT_TYPE_LOST		     0		                      //��
#define    CHEAT_TYPE_WIN		     1		                      //Ӯ

#define    DOUBLE_DOUBLE_X			 0.001						  //������ͬdouble��ֵ�����ֵ��һ��Ϊ0
#define    DIFEN_TIMES			     2						      //�׷���Ե�ע�ı���
//��Ϸ״̬
#define GS_T_FREE					GAME_STATUS_FREE						//�ȴ���ʼ
#define GS_T_SCORE					GAME_STATUS_PLAY						//�з�״̬
#define GS_T_PLAYING				GAME_STATUS_PLAY+1						//��Ϸ����

//������Ϣ
#define IDM_ADMIN_UPDATE_STORAGE		WM_USER+1001
#define IDM_ADMIN_MODIFY_STORAGE		WM_USER+1011
#define IDM_REQUEST_QUERY_USER			WM_USER+1012
#define IDM_USER_CONTROL				WM_USER+1013
#define IDM_REQUEST_UPDATE_ROOMINFO		WM_USER+1014
#define IDM_CLEAR_CURRENT_QUERYUSER		WM_USER+1015

//������¼
#define MAX_OPERATION_RECORD			20									//������¼����
#define RECORD_LENGTH					128									//ÿ����¼�ֳ�

//��Կ�ܸ�����ǰ4��λȡ�û�USERID����λ����12λ�ɱ䣩
#define AESKEY_TOTALCOUNT					16

//�ɱ���Կ����
#define AESKEY_VARIABLECOUNT				12

//��������ԭ�ĳ���
#define AESENCRYPTION_LENGTH				16


//�˿�����
#define CT_SINGLE					1									//��������
#define CT_DOUBLE					2									//��������
#define	CT_SHUN_ZI					3									//˳������
#define CT_JIN_HUA					4									//������
#define	CT_SHUN_JIN					5									//˳������
#define	CT_BAO_ZI					6									//��������
#define CT_SPECIAL					7									//��������

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_GAME_START				100									//��Ϸ��ʼ
#define SUB_S_ADD_SCORE					101									//��ע���
#define SUB_S_GIVE_UP					102									//������ע
#define SUB_S_SEND_CARD					103									//������Ϣ
#define SUB_S_GAME_END					104									//��Ϸ����
#define SUB_S_COMPARE_CARD				105									//���Ƹ�ע
#define SUB_S_LOOK_CARD					106									//���Ƹ�ע
#define SUB_S_PLAYER_EXIT				107									//�û�ǿ��
#define SUB_S_OPEN_CARD					108									//������Ϣ
#define SUB_S_WAIT_COMPARE				109									//�ȴ�����
#define SUB_S_ANDROID_CARD				110									//������Ϣ
#define SUB_S_CHEAT_CARD				111									//������Ϣ

#define SUB_S_ADMIN_STORAGE_INFO		112									//ˢ�¿��Ʒ����
#define SUB_S_REQUEST_QUERY_RESULT		113									//��ѯ�û����
#define SUB_S_USER_CONTROL				114									//�û�����
#define SUB_S_USER_CONTROL_COMPLETE		115									//�û��������
#define SUB_S_OPERATION_RECORD		    116									//������¼
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT 117
#define SUB_S_ANDROID_BANKOPERATOR		119	
#define SUB_S_UPDATEAESKEY				120									//������Կ

#define SUB_S_RC_TREASEURE_DEFICIENCY	121									//������ҷ����Ҳ���

#define SUB_S_SHOW_CARD					122									//������Ϣ

#define SUB_S_AUTO_COMPARE				123									//�Զ�������Ϣ

#define SUB_S_ALL_CARD					124									//���ط�����Ϣ

#define SUB_S_ADMIN_COLTERCARD			125		


//�����˴��ȡ��
struct tagCustomAndroid
{
	double									lRobotScoreMin;	
	double									lRobotScoreMax;
	double	                                lRobotBankGet; 
	double									lRobotBankGetBanker; 
	double									lRobotBankStoMul; 
};

//��������
typedef enum{CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL}CONTROL_TYPE;

//���ƽ��      ���Ƴɹ� ������ʧ�� ������ȡ���ɹ� ������ȡ����Ч
typedef enum{CONTROL_SUCCEED, CONTROL_FAIL, CONTROL_CANCEL_SUCCEED, CONTROL_CANCEL_INVALID}CONTROL_RESULT;

//�û���Ϊ
typedef enum{USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT}USERACTION;

//������Ϣ
typedef struct
{
	CONTROL_TYPE						control_type;					  //��������
	BYTE								cbControlCount;					  //���ƾ���
	bool							    bCancelControl;					  //ȡ����ʶ
}USERCONTROL;

//�����û���Ϣ
typedef struct
{
	WORD								wChairID;							//����ID
	WORD								wTableID;							//����ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//�����˱�ʶ
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE								cbUserStatus;						//�û�״̬
	BYTE								cbGameStatus;						//��Ϸ״̬
}ROOMUSERINFO;

//�����û�����
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//�����û���Ϣ
	USERCONTROL							userControl;						//�û�����
}ROOMUSERCONTROL;

//��Ϸ״̬
struct CMD_S_StatusFree
{
	double							lCellScore;							//��������
	//double							lRoomStorageStart;					//������ʼ���
	//double							lRoomStorageCurrent;				//���䵱ǰ���
	tagCustomAndroid					CustomAndroid;						//����������
	unsigned char						chUserAESKey[AESKEY_TOTALCOUNT];	//��ʼ��Կ
	TCHAR								szServerName[32];					//��������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//��ע��Ϣ
	double							lMaxCellScore;						//��Ԫ����
	double							lCellScore;							//��Ԫ��ע
	double							lCurrentTimes;						//��ǰ����
	double							lUserMaxScore;						//�û���������

	//״̬��Ϣ
	WORD								wBankerUser;						//ׯ���û�
	WORD				 				wCurrentUser;						//��ǰ���
	BYTE								cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	bool								bMingZhu[GAME_PLAYER];				//����״̬
	double							lTableScore[GAME_PLAYER];			//��ע��Ŀ
	//double							lRoomStorageStart;					//������ʼ���
	//double							lRoomStorageCurrent;				//���䵱ǰ���
	tagCustomAndroid					CustomAndroid;						//����������

	//�˿���Ϣ
	BYTE								cbHandCardData[MAX_COUNT];			//�˿�����

	//״̬��Ϣ
	bool								bCompareState;						//����״̬		
	unsigned char						chUserAESKey[AESKEY_TOTALCOUNT];	//��ʼ��Կ
	TCHAR								szServerName[32];					//��������

	int									nCurrentRounds;						//��ǰ�ִ�
	int									nTotalRounds;						//������

	double								dElapse;							//����ʱ��
};

//�������˿�
struct CMD_S_AndroidCard
{
	BYTE								cbRealPlayer[GAME_PLAYER];				//�������
	BYTE								cbAndroidStatus[GAME_PLAYER];			//������Ŀ
	BYTE								cbAllHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
	double								lStockScore;							//��ǰ���
	bool								bAndroidControl;
};


//�������˿�
struct CMD_S_CheatCardInfo
{
	BYTE								cbAllHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
};



//��Ϸ��ʼ
struct CMD_S_GameStart
{
	//��ע��Ϣ
	double							lMaxScore;							//�����ע
	double							lCellScore;							//��Ԫ��ע
	double							lCurrentTimes;						//��ǰ����
	double							lUserMaxScore;						//��������

	//�û���Ϣ
	WORD								wBankerUser;						//ׯ���û�
	WORD				 				wCurrentUser;						//��ǰ���
	BYTE								cbHandCardData[GAME_PLAYER][MAX_COUNT];//�˿�����
	BYTE								cbPlayStatus[GAME_PLAYER];			//�û�״̬

	int									nCurrentRounds;						//��ǰ�ִ�
	int									nTotalRounds;						//������
};

//�û���ע
struct CMD_S_AddScore
{
	WORD								wCurrentUser;						//��ǰ�û�
	WORD								wAddScoreUser;						//��ע�û�
	WORD								wCompareState;						//����״̬
	double								lAddScoreCount;						//��ע��Ŀ
	double								lCurrentTimes;						//��ǰ����
	int									nCurrentRounds;						//��ǰ�ִ�
	int									nNetwaititem;						//ǿ�ƿ���
};

//�û�����
struct CMD_S_GiveUp
{
	WORD								wGiveUpUser;						//�����û�
};

//�Զ�����
struct CMD_S_AutoCompare
{
	WORD								wAutoCompareUser;					//�Զ����Ʒ����û�����ǰ�û�current
};

//�������ݰ�
struct CMD_S_CompareCard
{
	WORD								wCurrentUser;						//��ǰ�û�
	WORD								wCompareUser[2];					//�����û�
	WORD								wLostUser;							//�����û�
};

//�������ݰ�
struct CMD_S_LookCard
{
	WORD								wLookCardUser;						//�����û�
	BYTE								cbCardData[MAX_COUNT];				//�û��˿�
};

//�������ݰ�
struct CMD_S_ShowCard
{
	WORD								wShowCardUser;						//�����û�
	BYTE								cbCardData[MAX_COUNT];				//�û��˿�
};

//�����˿�
struct CMD_S_SendCard
{
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];	//�û��˿�
};

//�������ݰ�
struct CMD_S_OpenCard
{
	WORD								wWinner;							//ʤ���û�
};

//��Ϸ����
struct CMD_S_GameEnd
{
	double							lGameTax;							//��Ϸ˰��
	double							lGameScore[GAME_PLAYER];			//��Ϸ�÷�
	BYTE								cbCardData[GAME_PLAYER][3];			//�û��˿�
	WORD								wCompareUser[GAME_PLAYER][6];		//�����û�
	WORD								wEndState;							//����״̬
	bool								bDelayOverGame;						//�ӳٿ�ʼ
	WORD								wServerType;						//��������
};

//�û��˳�
struct CMD_S_PlayerExit
{
	WORD								wPlayerID;							//�˳��û�
};

//�ȴ�����
struct CMD_S_WaitCompare
{
	WORD								wCompareUser;						//�����û�
};

struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO						userinfo;							//�û���Ϣ
	bool								bFind;								//�ҵ���ʶ
};

//�û�����
struct CMD_S_UserControl
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//�û��ǳ�
	CONTROL_RESULT							controlResult;						//���ƽ��
	CONTROL_TYPE							controlType;						//��������
	BYTE									cbControlCount;						//���ƾ���
};

//�û�����
struct CMD_S_UserControlComplete
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//�û��ǳ�
	CONTROL_TYPE							controlType;						//��������
	BYTE									cbRemainControlCount;				//ʣ����ƾ���
};

//���Ʒ���˿����Ϣ
struct CMD_S_ADMIN_STORAGE_INFO
{
	double	lRoomStorageStart;						//������ʼ���
	double	lRoomStorageCurrent;
	double	lRoomStorageDeduct;
	double	lMaxRoomStorage[2];
	WORD		wRoomStorageMul[2];
};

//������¼
struct CMD_S_Operation_Record
{
	TCHAR		szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];					//��¼���²�����20����¼
};

//������½��
struct CMD_S_RequestUpdateRoomInfo_Result
{
	double							lRoomStorageCurrent;				//���䵱ǰ���
	ROOMUSERINFO						currentqueryuserinfo;				//��ǰ��ѯ�û���Ϣ
	bool								bExistControl;						//��ѯ�û����ڿ��Ʊ�ʶ
	USERCONTROL							currentusercontrol;
};

//������Կ
struct CMD_S_UpdateAESKey
{
	unsigned char					chUserUpdateAESKey[AESKEY_TOTALCOUNT];	//��Կ
};


//�������ݰ�
struct CMD_S_AllCard
{
	bool                bAICount[GAME_PLAYER];
	BYTE                cbCardData[GAME_PLAYER][MAX_COUNT]; //�û��˿�
	BYTE                cbSurplusCardCount;    //ʣ������
	BYTE                cbSurplusCardData[52]; //ʣ���˿�

};

struct CMD_S_Admin_ChangeCard
{
	WORD dwCharID;
	BYTE cbCardData[MAX_COUNT];	//�û��˿�

};
//////////////////////////////////////////////////////////////////////////

//�ͻ�������ṹ
#define SUB_C_ADD_SCORE					1									//�û���ע
#define SUB_C_GIVE_UP					2									//������Ϣ
#define SUB_C_COMPARE_CARD				3									//������Ϣ
#define SUB_C_LOOK_CARD					4									//������Ϣ
#define SUB_C_OPEN_CARD					5									//������Ϣ
#define SUB_C_WAIT_COMPARE				6									//�ȴ�����
#define SUB_C_FINISH_FLASH				7									//��ɶ���
#define SUB_C_ADD_SCORE_TIME			8									//��ɶ���
#define SUB_C_SHOW_CARD					9									//������Ϣ

#define SUB_C_STORAGE					10									//���¿��
#define	SUB_C_STORAGEMAXMUL				11									//��������
#define SUB_C_REQUEST_QUERY_USER		12									//�����ѯ�û�
#define SUB_C_USER_CONTROL				13									//�û�����

//�����������
#define SUB_C_REQUEST_UDPATE_ROOMINFO	14									//������·�����Ϣ
#define SUB_C_CLEAR_CURRENT_QUERYUSER	15

#define SUB_C_AMDIN_CHANGE_CARD			16									//����


//�û���ע
struct CMD_C_AddScore
{
	double							lScore;								//��ע��Ŀ
	WORD								wState;								//��ǰ״̬
	unsigned char						chciphertext[AESENCRYPTION_LENGTH];	//����
};

//�������ݰ�
struct CMD_C_CompareCard
{	
	WORD								wCompareUser;						//�����û�
};

struct CMD_C_UpdateStorage
{
	double						lRoomStorageCurrent;					//�����ֵ
	double						lRoomStorageDeduct;						//�����ֵ
};

struct CMD_C_ModifyStorage
{
	double						lMaxRoomStorage[2];							//�������
	WORD							wRoomStorageMul[2];							//Ӯ�ָ���
};

struct CMD_C_RequestQuery_User
{
	DWORD							dwGameID;								//��ѯ�û�GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			    //��ѯ�û��ǳ�
};

//�û�����
struct CMD_C_UserControl
{
	DWORD									dwGameID;							//GAMEID
	TCHAR									szNickName[LEN_NICKNAME];			//�û��ǳ�
	USERCONTROL								userControlInfo;					//
};
//����
struct CMD_C_Admin_ChangeCard
{
	BYTE							cbStart;
	BYTE							cbEnd;
	DWORD							dwChairID;								//��ѯ�û�GAMEID
};

//////////////////////////////////////////////////////////////////////////


#pragma pack(pop)

#endif
