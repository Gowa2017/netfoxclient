#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////

//����������
//#define CARD_DISPATCHER_CONTROL

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID						389									//��Ϸ I D
#define GAME_NAME					TEXT("�����齫")					//��Ϸ����

//�������
#define GAME_PLAYER					4									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//////////////////////////////////////////////////////////////////////////////////

//��������
#define MAX_WEAVE					4									//������
#define MAX_INDEX					34									//�������
#define MAX_COUNT					14									//�����Ŀ
#define MAX_REPERTORY				112							//�����
#define MAX_HUA_INDEX				0									//��������
#define MAX_HUA_COUNT				8									//���Ƹ���

//�˿˶���
#define HEAP_FULL_COUNT				28									//����ȫ��
#define MAX_RIGHT_COUNT				1									//���ȨλDWORD����	

//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//////////////////////////////////////////////////////////////////////////////////
//״̬����

#define GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY					//��Ϸ����

struct CMD_S_RECORD
{
	int		 nCount;
	BYTE cbHuCount[GAME_PLAYER];//���ƴ���
	BYTE cbMaCount[GAME_PLAYER];//�������
	BYTE cbAnGang[GAME_PLAYER];//���ܴ���
	BYTE cbMingGang[GAME_PLAYER];//���ܴ���
	SCORE lAllScore[GAME_PLAYER];	//�ܽ����
	SCORE lDetailScore[GAME_PLAYER][32];	//���ֽ����
};

//����״̬
struct CMD_S_StatusFree
{
	//��Ϸ����
	LONG						lCellScore;									//��������

	//ʱ����Ϣ
	BYTE							cbTimeOutCard;								//����ʱ��
	BYTE							cbTimeOperateCard;						//����ʱ��
	BYTE							cbTimeStartGame;							//��ʼʱ��

	//��ʷ����
	SCORE						lTurnScore[GAME_PLAYER];					//������Ϣ
	SCORE						lCollectScore[GAME_PLAYER];					//������Ϣ

	//����					
	BYTE							cbPlayerCount;				//�������
	BYTE							cbMaCount;					//����
};

//�������
struct tagWeaveItem
{
	BYTE							cbWeaveKind;								//�������
	BYTE							cbCenterCard;								//�����˿�
	BYTE							cbParam;									//���ͱ�־
	WORD						wProvideUser;								//��Ӧ�û�
	BYTE							cbCardData[4];								//�齫����
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//ʱ����Ϣ
	BYTE							cbTimeOutCard;								//����ʱ��
	BYTE							cbTimeOperateCard;							//�з�ʱ��
	BYTE							cbTimeStartGame;							//��ʼʱ��

	//��Ϸ����
	LONGLONG				lCellScore;									//��Ԫ����
	WORD						wBankerUser;								//ׯ���û�
	WORD						wCurrentUser;								//��ǰ�û�
	BYTE                           cbMagicIndex;								//��������

	//����					
	BYTE							cbPlayerCount;				//�������
	BYTE							cbMaCount;					//����

	//״̬����
	BYTE							cbActionCard;								//�����˿�
	BYTE							cbActionMask;								//��������
	BYTE							cbLeftCardCount;							//ʣ����Ŀ
	bool							bTrustee[GAME_PLAYER];						//�Ƿ��й�
	bool							bTing[GAME_PLAYER];							//�Ƿ�����

	//������Ϣ
	WORD						wOutCardUser;								//�����û�
	BYTE							cbOutCardData;								//�����˿�
	BYTE							cbDiscardCount[GAME_PLAYER];				//������Ŀ
	BYTE							cbDiscardCard[GAME_PLAYER][60];				//������¼

	//�˿�����
	BYTE							cbCardCount[GAME_PLAYER];					//�˿���Ŀ
	BYTE							cbCardData[MAX_COUNT];						//�˿��б�
	BYTE							cbSendCardData;								//�����˿�

	//����˿�
	BYTE							cbWeaveItemCount[GAME_PLAYER];				//�����Ŀ
	tagWeaveItem			WeaveItemArray[GAME_PLAYER][MAX_WEAVE];		//����˿�

	//������Ϣ
	WORD						wHeapHead;									//����ͷ��
	WORD						wHeapTail;									//����β��
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];				//������Ϣ

	BYTE							cbHuCardCount[MAX_COUNT];
	BYTE							cbHuCardData[MAX_COUNT][28];
	BYTE							cbOutCardCount;
	BYTE							cbOutCardDataEx[MAX_COUNT];
	//��ʷ����
	SCORE						lTurnScore[GAME_PLAYER];					//������Ϣ
	SCORE						lCollectScore[GAME_PLAYER];					//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_S_INIT_USERINFO			119									//�û�������Ϣ

#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_OUT_CARD				101									//�û�����
#define SUB_S_SEND_CARD				102									//�����˿�
#define SUB_S_OPERATE_NOTIFY		103									//������ʾ
#define SUB_S_OPERATE_RESULT		104									//��������
#define SUB_S_LISTEN_CARD			105									//�û�����
#define SUB_S_TRUSTEE				106									//�û��й�
#define SUB_S_REPLACE_CARD			107									//�û�����
#define SUB_S_GAME_CONCLUDE			108									//��Ϸ����
#define SUB_S_SET_BASESCORE			109									//���û���
#define SUB_S_HU_CARD						110									//���ƺ�������
#define SUB_S_RECORD							111									//��Ϸ��¼

struct CMD_S_GameUserInfo
{
	int		userId[GAME_PLAYER];
};
//�����˿�
struct CMD_S_GameStart
{
	WORD				 			wBankerUser;						//��ǰׯ��
	WORD				 			wReplaceUser;						//�����û�
	WORD							wSiceCount;							//���ӵ���
	WORD							wHeapHead;							//�ƶ�ͷ��
	WORD							wHeapTail;							//�ƶ�β��
	BYTE                            cbMagicIndex;                       //��������
	BYTE							cbHeapCardInfo[GAME_PLAYER][2];		//������Ϣ
	BYTE							cbUserAction;						//�û�����
	BYTE							cbCardData[MAX_COUNT];				//�齫�б�
	BYTE							cbOutCardCount;
	BYTE							cbOutCardData[MAX_COUNT][28];


	//��ʷ����
	SCORE						lTurnScore[GAME_PLAYER];					//������Ϣ
	SCORE						lCollectScore[GAME_PLAYER];					//������Ϣ
};

//�������˿�
struct CMD_S_AndroidCard
{
	BYTE							cbHandCard[GAME_PLAYER][MAX_COUNT];//�����˿�
	WORD							wCurrentUser ;						//��ǰ���
};

//�û�����
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//�����û�
	BYTE							cbOutCardData;						//�����˿�
	bool							bSysOut;								//�й�ϵͳ����
};

//�����˿�
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//�˿�����
	BYTE							cbActionMask;						//��������
	WORD							wCurrentUser;						//��ǰ�û�
	WORD							wSendCardUser;						//�����û�
	WORD							wReplaceUser;						//�����û�
	bool							bTail;								//ĩβ����
};

//������ʾ
struct CMD_S_OperateNotify
{
	BYTE							cbActionMask;						//��������
	BYTE							cbActionCard;						//�����˿�
};

//��������
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//�����û�
	BYTE							cbActionMask;						//��������
	WORD							wProvideUser;						//��Ӧ�û�
	BYTE							cbOperateCode;						//��������
	BYTE							cbOperateCard[3];					//�����˿�
};
struct CMD_S_Hu_Data
{
	//���ļ�������
	BYTE							cbOutCardCount;
	BYTE							cbOutCardData[MAX_COUNT];
	//�����ܺ��ļ�����
	BYTE							cbHuCardCount[MAX_COUNT];
	BYTE							cbHuCardData[MAX_COUNT][28];
	//����ʣ����
	BYTE							cbHuCardRemainingCount[MAX_COUNT][28];
};
//��������
struct CMD_S_ListenCard
{
	WORD							wListenUser;				//�����û�
	bool							bListen;							//�Ƿ�����
	BYTE							cbHuCardCount;				//��������
	BYTE							cbHuCardData[MAX_INDEX];//��������
};

//��Ϸ����
struct CMD_S_GameConclude
{
	//���ֱ���
	LONG							lCellScore;										//��Ԫ����
	SCORE							lGameScore[GAME_PLAYER];			//��Ϸ����
	SCORE							lRevenue[GAME_PLAYER];				//˰�ջ���
	SCORE							lGangScore[GAME_PLAYER];			//���ָ���Ӯ��
	//������Ϣ
	WORD							wProvideUser;						//��Ӧ�û�
	BYTE							cbProvideCard;						//��Ӧ�˿�
	BYTE							cbSendCardData;						//�����
	DWORD							dwChiHuKind[GAME_PLAYER];			//��������
	DWORD							dwChiHuRight[GAME_PLAYER][MAX_RIGHT_COUNT];		//��������	
	WORD							wLeftUser;							//�������
	WORD							wLianZhuang;					//��ׯ
	
	//��Ϸ��Ϣ
	BYTE							cbCardCount[GAME_PLAYER];			//�˿���Ŀ
	BYTE							cbHandCardData[GAME_PLAYER][MAX_COUNT];//�˿��б�

	BYTE							cbMaCount[GAME_PLAYER];							//����
	BYTE							cbMaData[7];							//������

	//��ʷ����
	SCORE						lTurnScore[GAME_PLAYER];					//������Ϣ
	SCORE						lCollectScore[GAME_PLAYER];					//������Ϣ

	BYTE							cbLeftCardCount;
	BYTE                            cbLeftCardData[MAX_REPERTORY];         //������Ϣ
};

//�û��й�
struct CMD_S_Trustee
{
	bool							bTrustee;							//�Ƿ��й�
	WORD							wChairID;							//�й��û�
};

//��������
struct CMD_S_ReplaceCard
{
	WORD							wReplaceUser;						//�����û�
	BYTE							cbReplaceCard;						//�����˿�
};

//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_C_OUT_CARD				1									//��������
#define SUB_C_OPERATE_CARD			2									//�����˿�
#define SUB_C_LISTEN_CARD			3									//�û�����
#define SUB_C_TRUSTEE				4									//�û��й�
#define SUB_C_REPLACE_CARD			5									//�û�����
#define SUB_C_SEND_CARD             6                                   //�����˿� 
//��������
struct CMD_C_OutCard
{
	BYTE							cbCardData;							//�˿�����
};

//��������
struct CMD_C_OperateCard
{
	BYTE							cbOperateCode;						//��������
	BYTE							cbOperateCard[3];					//�����˿�
};

//�û�����
struct CMD_C_ListenCard
{
	bool							bListenCard;						//�Ƿ�����	
};


//�û��й�
struct CMD_C_Trustee
{
	bool							bTrustee;							//�Ƿ��й�	
};

//��������
struct CMD_C_ReplaceCard
{
	BYTE							cbCardData;							//�˿�����
};


//�����˿�
struct CMD_C_SendCard
{
	BYTE							cbControlGameCount;				   //���ƴ���
	BYTE                            cbCardCount;                       //�˿���Ŀ
	WORD                            wBankerUser;                       //����ׯ��
	BYTE                            cbCardData[MAX_REPERTORY];         //�˿�����
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif