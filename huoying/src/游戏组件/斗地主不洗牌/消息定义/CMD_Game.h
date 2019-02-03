#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//������

//��Ϸ����
#define KIND_ID						202									//��Ϸ I D
#define GAME_NAME					TEXT("��ϴ�ƶ�����")						//��Ϸ����

//�������
#define GAME_PLAYER					3									//��Ϸ����
#define VERSION_SERVER				PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT				PROCESS_VERSION(7,0,1)				//����汾

//////////////////////////////////////////////////////////////////////////////////

//��Ŀ����
#define MAX_COUNT					20									//�����Ŀ
#define FULL_COUNT					54									//ȫ����Ŀ

//�߼���Ŀ
#define NORMAL_COUNT				17									//������Ŀ
#define DISPATCH_COUNT				51									//�ɷ���Ŀ
#define GOOD_CARD_COUTN				38									//������Ŀ

//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//�߼�����
#define CT_ERROR					0									//��������
#define CT_SINGLE					1									//��������
#define CT_DOUBLE					2									//��������
#define CT_THREE					3									//��������
#define CT_SINGLE_LINE				4									//��������
#define CT_DOUBLE_LINE				5									//��������
#define CT_THREE_LINE				6									//��������
#define CT_THREE_TAKE_ONE			7									//����һ��
#define CT_THREE_TAKE_TWO			8									//����һ��
#define CT_FOUR_TAKE_ONE			9									//�Ĵ�����
#define CT_FOUR_TAKE_TWO			10									//�Ĵ�����
#define CT_BOMB_CARD				11									//ը������
#define CT_MISSILE_CARD				12									//�������

//////////////////////////////////////////////////////////////////////////////////
//״̬����

#define	GAME_SCENE_FREE				GAME_STATUS_FREE					//�ȴ���ʼ
#define GAME_SCENE_CALL				GAME_STATUS_PLAY					//�з�״̬
#define GAME_SCENE_PLAY				GAME_STATUS_PLAY+1					//��Ϸ����
#define GAME_SCENE_MULTIPLE			GAME_STATUS_PLAY+2					//�ӱ�

//����״̬
struct CMD_S_StatusFree
{
	//��Ϸ����
	SCORE							lCellScore;							//��������

	//ʱ����Ϣ
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeCallScore;					//�з�ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��

	//��ʷ����
	SCORE							lTurnScore[GAME_PLAYER];			//������Ϣ
	SCORE							lCollectScore[GAME_PLAYER];			//������Ϣ
};

//�з�״̬
struct CMD_S_StatusCall
{
	//ʱ����Ϣ
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeCallScore;					//�з�ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��

	//��Ϸ��Ϣ
	SCORE							lCellScore;							//��Ԫ����
	WORD							wCurrentUser;						//��ǰ���
	BYTE							cbBankerScore;						//ׯ�ҽз�
	BYTE							cbScoreInfo[GAME_PLAYER];			//�з���Ϣ
	BYTE							cbHandCardData[NORMAL_COUNT];		//�����˿�
	bool							bTrustee[GAME_PLAYER];				//�йܱ�־
	//��ʷ����
	SCORE							lTurnScore[GAME_PLAYER];			//������Ϣ
	SCORE							lCollectScore[GAME_PLAYER];			//������Ϣ
};

//�ӱ�״̬
struct CMD_S_StatusMultiple
{
	//ʱ����Ϣ
	BYTE							cbMultiple;							//�ӱ�ʱ��

	//��Ϸ����
	SCORE							lCellScore;							//��Ԫ����
	WORD							wBankerUser;						//ׯ���û�
	BYTE							cbBankerScore;						//ׯ�ҽз�

	//�˿���Ϣ
	BYTE							cbBankerCard[3];					//��Ϸ����
	BYTE							cbHandCardData[MAX_COUNT];			//�����˿�
	BYTE							cbHandCardCount[GAME_PLAYER];		//�˿���Ŀ

	//��ʷ����
	SCORE							lTurnScore[GAME_PLAYER];			//������Ϣ
	SCORE							lCollectScore[GAME_PLAYER];			//������Ϣ
	bool							bTrustee[GAME_PLAYER];				//�Ƿ��й�
	SCORE							lEnterScore;						//�������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	//ʱ����Ϣ
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeCallScore;					//�з�ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��

	//��Ϸ����
	SCORE							lCellScore;							//��Ԫ����
	BYTE							cbBombCount;						//ը������
	WORD							wBankerUser;						//ׯ���û�
	WORD							wCurrentUser;						//��ǰ���
	BYTE							cbBankerScore;						//ׯ�ҽз�

	//������Ϣ
	WORD							wTurnWiner;							//ʤ�����
	BYTE							cbTurnCardCount;					//������Ŀ
	BYTE							cbTurnCardData[MAX_COUNT];			//��������

	//�˿���Ϣ
	BYTE							cbBankerCard[3];					//��Ϸ����
	BYTE							cbHandCardData[MAX_COUNT];			//�����˿�
	BYTE							cbHandCardCount[GAME_PLAYER];		//�˿���Ŀ

	//��ʷ����
	SCORE							lTurnScore[GAME_PLAYER];			//������Ϣ
	SCORE							lCollectScore[GAME_PLAYER];			//������Ϣ
	bool							bTrustee[GAME_PLAYER];				//�Ƿ��й�
};

//////////////////////////////////////////////////////////////////////////////////
//�����

#define	SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_CALL_SCORE			101									//�û��з�
#define SUB_S_BANKER_INFO			102									//ׯ����Ϣ
#define SUB_S_OUT_CARD				103									//�û�����
#define SUB_S_PASS_CARD				104									//�û�����
#define SUB_S_GAME_CONCLUDE			105									//��Ϸ����
#define SUB_S_TRUSTEE				106									//���û���
#define SUB_S_CHEAT_CARD			107									//�����˿�
#define SUB_S_SET_BASESCORE			108									//�й�
#define SUB_S_OTHER_CARDS			109									//����������
#define SUB_S_MULTIPLE				110									//�ӱ�
#define SUB_S_START_OUTCARD			111									//��ʼ����
#define SUB_S_ANDROID_BANKOPERATOR    112                 //���������в���

//�����˿�
struct CMD_S_GameStart
{
	WORD							wStartUser;							//��ʼ���
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbValidCardData;					//�����˿�
	BYTE							cbValidCardIndex;					//����λ��
	BYTE							cbTimeCallScore;					//�з�ʱ��
	BYTE							cbCardData[NORMAL_COUNT];			//�˿��б�
};

//�������˿�
struct CMD_S_AndroidCard
{
	BYTE							cbHandCard[GAME_PLAYER][NORMAL_COUNT];//�����˿�
	BYTE							cbBankerCard[3];
	WORD							wCurrentUser ;						//��ǰ���
};

//�����˿�
struct CMD_S_CheatCard
{
	WORD							wCardUser[GAME_PLAYER ];				//�������
	BYTE							cbUserCount;							//��������
	BYTE							cbCardData[GAME_PLAYER ][MAX_COUNT];	//�˿��б�
	BYTE							cbCardCount[GAME_PLAYER ];				//�˿�����

};
//�û��з�
struct CMD_S_CallScore
{
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wCallScoreUser;						//�з����
	BYTE							cbCurrentScore;						//��ǰ�з�
	BYTE							cbUserCallScore;					//�ϴνз�
	BYTE							cbTimeCallScore;					//�з�ʱ��
};

//ׯ����Ϣ
struct CMD_S_BankerInfo
{
	WORD				 			wBankerUser;						//ׯ�����
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbBankerScore;						//ׯ�ҽз�
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��
	BYTE							cbBankerCard[3];					//ׯ���˿�
	BYTE							cbMultiple;							//�ӱ�ʱ��
	SCORE							lEnterScore;						//�������
};

//�û�����
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//������Ŀ
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wOutCardUser;						//�������
	int								lMultiple;							//����
	bool							bSysOut;							//�й�ϵͳ����
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbCardData[MAX_COUNT];				//�˿��б�
};

//��������
struct CMD_S_PassCard
{
	BYTE							cbTurnOver;							//һ�ֽ���
	WORD				 			wCurrentUser;						//��ǰ���
	WORD				 			wPassCardUser;						//�������
	bool							bSysOut;							//�й�ϵͳ����
	BYTE							cbTimeOutCard;						//����ʱ��
};

//��Ϸ����
struct CMD_S_GameConclude
{
	//���ֱ���
	SCORE							lCellScore;							//��Ԫ����
	SCORE							lGameScore[GAME_PLAYER];			//��Ϸ����

	//�����־
	BYTE							bChunTian;							//�����־
	BYTE							bFanChunTian;						//�����־

	//ը����Ϣ
	BYTE							cbBombCount;						//ը������
	BYTE							cbEachBombCount[GAME_PLAYER];		//ը������

	//��Ϸ��Ϣ
	BYTE							cbBankerScore;						//�з���Ŀ
	BYTE							cbCardCount[GAME_PLAYER];			//�˿���Ŀ
	BYTE							cbHandCardData[FULL_COUNT];			//�˿��б�

	LONG							lAllMultiple;						//�ܱ���

	LONG							lChunTianMultiple;					//���챶��
	LONG							lFanChunTianMultiple;				//�����챶��
	LONG							lBombMultiple;						//ը������

	// �����Ϣ
	LONG							lRocketMultiple;					//�������
	BYTE							cbTimeStartGame;					//��ʼʱ��

	//������ʾ
	bool							bSpecialTip;						// ������ʾ
	BYTE							bMultiple[GAME_PLAYER];			//�Ƿ�ӱ�
};

//�û��й�
struct CMD_S_Trustee
{
	bool							bTrustee;							//�Ƿ��й�
	WORD							wChairID;							//�й��û�
};
//���������˵��˿�����
struct CMD_S_Other_CardData
{
	BYTE							cbCardDataList[GAME_PLAYER][MAX_COUNT];			//�˿��б�
};

//�û��ӱ�
struct CMD_S_Multiple
{
	bool							bIsMultiple;							//�Ƿ�ӱ�
	WORD							wChairID;							//�ӱ�/���ӱ��û�
	BYTE							cbBankerScore;						//ׯ�ҽз�
};

// ��ʼ����
struct CMD_S_START_OUTCARD
{
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��
	WORD				 			wBankerUser;						//ׯ�����
};
//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_C_CALL_SCORE			1									//�û��з�
#define SUB_C_OUT_CARD				2									//�û�����
#define SUB_C_PASS_CARD				3									//�û�����
#define SUB_C_TRUSTEE				4									//�û��й�
#define SUB_C_MULTIPLE				5									//�ӱ�
#define SUB_S_ANDROIDUP				11									//����������
//�û��з�
struct CMD_C_CallScore
{
	BYTE							cbCallScore;						//�з���Ŀ
};

//�û�����
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//������Ŀ
	BYTE							cbCardData[MAX_COUNT];				//�˿�����
};

//�û��й�
struct CMD_C_Trustee
{
	bool							bTrustee;							//�Ƿ��й�	
};

//�û��ӱ�
struct CMD_C_Multiple
{
	bool							bIsMultiple;						//�Ƿ�ӱ�
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif