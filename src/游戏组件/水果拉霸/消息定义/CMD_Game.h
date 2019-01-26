#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID						406									//��Ϸ I D
#define GAME_NAME					TEXT("ˮ������")					//��Ϸ����

//�������
#define GAME_PLAYER					1									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//////////////////////////////////////////////////////////////////////////////////

// ��Ļ����	
#define Screen_Width				(1280)		// ��С���
#define Screen_Height				(798)		// ��С�߶�

//��Ŀ����
#define ITEM_COUNT					9										//ͼ������
#define ITEM_X_COUNT				5										//ͼ������
#define ITEM_Y_COUNT				3										//ͼ������

//�߼�����
#define CT_FUTOU					0					//�㽶				//��ͷ
#define CT_YINGQIANG				1					//����				//��ǹ
#define CT_DADAO					2					//â��				//��
#define CT_LU						3					//����				//³
#define CT_LIN						4					//����				//��
#define CT_SONG						5					//����				//��
#define CT_TITIANXINGDAO			6					//ӣ��				//�����е�
#define CT_ZHONGYITANG				7					//�ư�				//������
#define CT_SHUIHUZHUAN				8					//����				//ˮ䰴�
#define CT_SHUIHUZHUAN2				9					//7��				
#define CT_SHENBEN					10					//�ȱ�				

//����ģʽ
#define GM_NULL						0									//����
#define GM_SHUO_MING				1									//˵��
#define GM_ONE						2									//����
#define GM_TWO						3									//�ȱ�
#define GM_TWO_WAIT					4									//�ȴ��ȱ�
//#define GM_THREE					5									//С����
#define GM_SMALL					5									//С��Ϸ

//////////////////////////////////////////////////////////////////////////////////
//״̬����

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_ONE				GAME_STATUS_PLAY+1					//ˮ䰴�
#define GAME_SCENE_TWO				GAME_STATUS_PLAY+2					//����
#define GAME_SCENE_THREE			GAME_STATUS_PLAY+3					//ˮ����
#define GAME_SCENE_WIAT				GAME_STATUS_PLAY+4					//�ȴ�
#define GAME_SMALL_GAME				GAME_STATUS_PLAY+5					//С��Ϸ״̬
//����״̬
struct CMD_S_StatusFree
{
	//��Ϸ����
	LONG							lCellScore;							//��������

	//��עֵ
	BYTE							cbBetCount;							//��ע����
	LONGLONG						lBetScore[9];						//��ע��С
	BYTE							cbBetLine;							//��ע����
	LONGLONG 						ALLbonus;							 //����
};

//С��Ϸ��������
struct CMD_S_SmallStatus
{
	//��Ϸ����
	LONG							lCellScore;								//��������
	BYTE							cbGameMode;								//��Ϸģʽ
	LONGLONG						lBet;									//Ѻ����ע
	LONGLONG						lBetCount;								//ѹ������
	LONGLONG						lOneGameScore;							//Ѻ�ߵ÷�
	LONGLONG						lSmallGameScore;						//С��Ϸ�÷�	
	BYTE							cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//������Ϣ
	LONGLONG						lBetScore[9];							//��ע��С
	BYTE							cbCustoms;								//С��Ϸ�ؿ�
	WORD							wUseTime;								//С��Ϸʣʱ��
	LONGLONG                        cbBonus;									//����
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//��עֵ
	BYTE							cbBetCount;							//��ע����
	LONGLONG						lBetScore[9];						//��ע��С
	BYTE							cbBetCount1;						//��ע����

	LONGLONG                        ALLbonus;                 //����
};

//������¼
#define MAX_OPERATION_RECORD		20									 //������¼����
#define RECORD_LENGTH				200									 //ÿ����¼�ֳ�

//��������
typedef enum{CONTINUE_3, CONTINUE_4, CONTINUE_5, CONTINUE_ALL, CONTINUE_LOST, CONTINUE_CANCEL}CONTROL_TYPE;

//���ƽ��      ���Ƴɹ� ������ʧ�� ������ȡ���ɹ� ������ȡ����Ч
typedef enum{CONTROL_SUCCEED, CONTROL_FAIL, CONTROL_CANCEL_SUCCEED, CONTROL_CANCEL_INVALID}CONTROL_RESULT;

//�û���Ϊ
typedef enum{USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT}USERACTION;

//������Ϣ
typedef struct
{
	CONTROL_TYPE					controlType;					   //��������
	BYTE							cbControlData;					   //��������
	BYTE							cbControlCount;					   //���ƾ���
	BYTE							cbZhongJiang;					   //�н�����
	BYTE							cbZhongJiangJian;				   //˥������
}USERCONTROL;

//�����û���Ϣ
typedef struct
{
	WORD							wChairID;							//����ID
	WORD							wTableID;							//����ID
	DWORD							dwGameID;							//GAMEID
	bool							bAndroid;							//�����˱�ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE							cbUserStatus;						//�û�״̬
	BYTE							cbGameStatus;						//��Ϸ״̬
	LONGLONG						lGameScore;							//��Ϸ��Ӯ��
	USERCONTROL						UserContorl;						//������Ϣ
}ROOMUSERINFO;

//���ӿ��Ͷ����Ϣ
typedef struct
{
	WORD							wTableID;						
	LONGLONG						lTableStorageInput;
}TABLESTORAGEINPUT;

//��ѯ�û����
struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO					userinfo;							//�û���Ϣ
	bool							bFind;								//�ҵ���ʶ
};

//�û�����
struct CMD_S_UserControl
{
	DWORD							dwGameID;							//GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	CONTROL_RESULT					controlResult;						//���ƽ��
	USERCONTROL						UserControl;						//��������
	BYTE							cbControlCount;						//���ƾ���
};

//�û�����
struct CMD_S_UserControlComplete
{
	DWORD							dwGameID;							//GAMEID
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	USERCONTROL						UserControl;						//��������
	BYTE							cbRemainControlCount;				//ʣ����ƾ���
};

//���Ʒ���˿����Ϣ
struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG						lRoomStorageStart;					//������ʼ���
	LONGLONG						lRoomStorageCurrent;				//���䵱ǰ���
	LONGLONG						lRoomStorageInput;					//������Ͷ��
	LONGLONG						lCurrentStorageTable;				//��ǰ���ӿ��
	LONGLONG						lRoomStockRecoverScore;				//��������Ӯ
	LONGLONG						lCurrentDeductRoom;					//ȫ�ֿ��˥��
	LONGLONG						lMaxStorageRoom[2];					//ȫ�ֿ������
	WORD							wStorageMulRoom[2];					//ȫ�ֿ��Ӯ�ָ���
	WORD							wGameTwo;							//�ȱ�����
	WORD							wGameTwoDeduct;						//�ȱ�����
	WORD							wGameThree;							//�ȱ�����
	WORD							wGameThreeDeduct;					//С��������
};

//������¼
struct CMD_S_Operation_Record
{
	TCHAR							szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];		//��¼���²�����20����¼
};

//���·�����
struct CMD_S_UpdateRoomStorage
{
	LONGLONG						lRoomStorageStart;					//������ʼ���
	LONGLONG						lRoomStorageCurrent;				//���䵱ǰ���
	LONGLONG						lRoomStorageInput;					//������Ͷ��
	LONGLONG						lRoomStockRecoverScore;				//��������Ӯ
};

//��������б�
struct CMD_S_UpdateRoomUserList
{
	DWORD							dwUserID;							//�û�ID
	ROOMUSERINFO					roomUserInfo;
};


//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_S_GAME_START				100									//Ѻ�߽���
#define SUB_S_GAME_CONCLUDE				101									//С��������
#define SUB_S_SMALL_START				105									//С��Ϸ��ʼ
#define SUB_S_TWO_GAME_CONCLUDE			106									//С��Ϸ��������
#define SUB_S_THREE_KAI_JIAN			104									//�н���¼����


#define SUB_S_TWO_START					103									//Ѻ�߽���
#define SUB_S_THREE_END					107									//С��������
#define SUB_S_SET_BASESCORE				108									//���û���
#define SUB_S_AMDIN_COMMAND				109									//ϵͳ����
#define SUB_S_ADMIN_STORAGE_INFO		110									//ˢ�¿��Ʒ����
#define SUB_S_REQUEST_QUERY_RESULT		111									//��ѯ�û����
#define SUB_S_USER_CONTROL				112									//�û�����
#define SUB_S_USER_CONTROL_COMPLETE		113									//�û��������
#define SUB_S_OPERATION_RECORD			114									//������¼
#define SUB_S_UPDATE_ROOM_STORAGE		115									//���·�����
#define SUB_S_UPDATE_ROOMUSERLIST		116									//���·����û��б�

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	//��ע��Ϣ
	LONGLONG							lScore;										//��Ϸ����	
	BYTE								cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];		//������Ϣ
	BYTE								cbGameMode;									//��Ϸģʽ
	LONGLONG							cbBouns;									//С��������
};

struct CMD_S_WinningRecord
{
	BYTE								BonusCount;								//�н��ش���
	LONGLONG							BonusSorce[10];							//�н��ؼ�¼
	SYSTEMTIME							BonusSorceDate[10];						//�н���ʱ��
	TCHAR								NickName[10][LEN_NICKNAME];				//���
};

//�ȱ���ʼ
struct CMD_S_GameTwoStart
{
	//��ע��Ϣ
	BYTE								cbOpenSize[2];								//����
	LONGLONG							lScore;										//��Ϸ����	
};

//С������ʼ
struct CMD_S_GameThreeStart
{
	//��ע��Ϣ
	BYTE								cbItem[4];									//����
	BYTE								cbBounsGameCount;							//��Ϸ����	
	LONGLONG							lScore;										//��Ϸ����	
};

//С������ʼ
struct CMD_S_GameThreeKaiJiang
{
	//��ע��Ϣ
	BYTE								cbIndex;									//����
	int									nTime;										//����
	LONGLONG							lScore;										//��Ϸ����	
};

//��Ϸ����
struct CMD_S_OneGameConclude
{
	//���ֱ���
	LONGLONG							lCellScore;									//��Ԫ����
	LONGLONG							lGameScore;									//��Ϸ����
	BYTE								cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];		//������Ϣ
	BYTE								cbGameMode;									//��Ϸģʽ
};

//////////////////////////////////////////////////////////////////////////////////
//�����
#define SUB_C_SMALL_START			1									//��ʼС��Ϸ
#define SUB_C_SMALL_END				2									//����С��Ϸ
#define SUB_C_THREE_START			3									//�ȱ���ʼ
#define SUB_C_LOOK_RECORD			4									//�н���¼
#define SUB_C_THREE_END				5									//����С��Ϸ
#define SUB_C_ONE_START				6									//Ѻ�߿�ʼ
#define SUB_C_ONE_END				7									//����Ѻ��

#define SUB_C_UPDATE_TABLE_STORAGE	28									//�������ӿ��
#define	SUB_C_MODIFY_ROOM_CONFIG	29									//�޸ķ�������
#define SUB_C_REQUEST_QUERY_USER	30									//�����ѯ�û�
#define SUB_C_USER_CONTROL			31									//�û�����

//�����������
#define SUB_C_REQUEST_UDPATE_ROOMUSERLIST			32					//������·����û��б�
#define SUB_C_REQUEST_UDPATE_ROOMSTORAGE			33					//������·�����

//����ģʽ
#define SUB_C_SINGLEMODE_CONFIRM					35					//
//����ģʽ
#define SUB_C_BATCHMODE_CONFIRM						36					//

struct CMD_C_SmallStart
{
	BYTE				cbGameMode;									//��Ϸģʽ
	DWORD				SamllGameFruitData[4][5];					//С��Ϸ����
};

struct CMD_C_SmallStatus
{
	bool                Isover;						//�Ƿ����
	BYTE				cbCustoms;					//��ǰ�ؿ�
	LONGLONG			SmallSorce;					//ˮ������
};

//�û��з�
struct CMD_C_OneStart
{		
	LONGLONG							lBetCount;							//ѹ������
	LONGLONG							lBet;								//Ѻ�߷���
};

//�û��з�
struct CMD_C_TwoMode
{
	BYTE							cbOpenMode;							//�ȱ�ģʽ 0����ȱ�  1��ȫ�ȱ�   2��˫�ȱ�
};

//�û��з�
struct CMD_C_TwoStart
{
	BYTE							cbOpenArea;							//��ע���� 0����  1����   2����
};

//���±������
struct CMD_C_UpdateStorageTable
{
	LONGLONG						lStorageTable;							//���ӿ��
};

struct CMD_C_ModifyRoomConfig
{
	LONGLONG						lStorageDeductRoom;						//ȫ�ֿ��˥��
	LONGLONG						lMaxStorageRoom[2];						//ȫ�ֿ������
	WORD							wStorageMulRoom[2];						//ȫ��Ӯ�ָ���
	WORD							wGameTwo;								//�ȱ�����
	WORD							wGameTwoDeduct;							//�ȱ�����
	WORD							wGameThree;								//С��������
	WORD							wGameThreeDeduct;						//С��������
};

struct CMD_C_RequestQuery_User
{
	DWORD							dwGameID;								//��ѯ�û�GAMEID
};

//�û�����
struct CMD_C_UserControl
{
	DWORD							dwGameID;								//GAMEID
	USERCONTROL						userControlInfo;						//�û�������Ϣ
};

//�����޸�
struct CMD_C_SingleMode
{
	WORD							wTableID;
	LONGLONG						lTableStorage;
};

//�����޸�
struct CMD_C_BatchMode
{
	LONGLONG						lBatchModifyStorage;
	WORD							wBatchTableCount;						//�����޸ĵ���������
	bool							bBatchTableFlag[MAX_TABLE];				//TRUEΪ�޸ĵı�־	
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif