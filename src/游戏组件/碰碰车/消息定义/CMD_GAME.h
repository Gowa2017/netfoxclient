#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE
#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//�����궨��

#define KIND_ID           140                 //��Ϸ I D
#define GAME_PLAYER         MAX_CHAIR                 //��Ϸ����
#define GAME_NAME         TEXT("�������ֲ�")            //��Ϸ����

#define TEST_OUT(num)   CString str;str.Format(L"this is:%d",num);AfxMessageBox(str);
//�汾��Ϣ
#define VERSION_SERVER          PROCESS_VERSION(7,0,1)        //����汾
#define VERSION_CLIENT        PROCESS_VERSION(7,0,1)        //����汾

//״̬����
#define GS_PLACE_JETTON       GAME_STATUS_PLAY          //��ע״̬
#define GS_GAME_END         GAME_STATUS_PLAY+1            //����״̬
#define GS_MOVECARD_END       GAME_STATUS_PLAY+2            //����״̬

//��������
#define ID_TIAN_MEN         1                 //˳��
#define ID_DI_MEN         2                 //��߽�
#define ID_XUAN_MEN         3                 //��
#define ID_HUANG_MEN        4                 //����

//�������
#define BANKER_INDEX        0                 //ׯ������
#define SHUN_MEN_INDEX        1                 //˳������
#define DUI_MEN_INDEX       2                 //��������
#define DAO_MEN_INDEX       3                 //��������
#define HUAN_MEN_INDEX        4                 //��������

//������
#define RQ_REFRESH_STORAGE    13
#define RQ_SET_STORAGE      14
#define AREA_COUNT          8                 //������Ŀ

//���ʶ���
#define RATE_TWO_PAIR       12                  //��������

#define SUB_S_UPDATE_STORAGE        111                 //���¿��
//��ע��Ϣ
struct tagUserBet
{
  TCHAR             szNickName[32];           //�û��ǳ�
  DWORD             dwUserGameID;           //�û�ID
  LONGLONG            lUserScore;             //�û����
  LONGLONG            lUserBet[AREA_COUNT+1];       //�û���ע
  LONGLONG            lUserScoreTotal;              //�û����
};
//��ע��Ϣ����
typedef CWHArray<tagUserBet,tagUserBet&> CUserBetArray;
struct tagCustomAndroid
{
  //��ׯ
  BOOL              nEnableRobotBanker;       //�Ƿ���ׯ
  LONGLONG            lRobotBankerCount;        //��ׯ����
  LONGLONG            lRobotListMinCount;       //�б�����
  LONGLONG            lRobotListMaxCount;       //�б�����
  LONGLONG            lRobotApplyBanker;        //����������
  LONGLONG            lRobotWaitBanker;       //��������

  //��ע
  LONGLONG            lRobotMinBetTime;       //��ע�������
  LONGLONG            lRobotMaxBetTime;       //��ע�������
  LONGLONG            lRobotMinJetton;        //��ע������
  LONGLONG            lRobotMaxJetton;        //��ע������
  LONGLONG            lRobotBetMinCount;        //��ע��������
  LONGLONG            lRobotBetMaxCount;        //��ע��������
  LONGLONG            lRobotAreaLimit;        //��������

  //��ȡ��
  LONGLONG            lRobotGetCondition;       //������ȡ�������������˵ķ���С�ڸ�ֵʱִ��ȡ��
  LONGLONG            lRobotSaveCondition;      //�����˴������
  LONGLONG            lRobotGetMin;         //ȡ����Сֵ
  LONGLONG            lRobotGetMax;         //ȡ�����ֵ
  LONGLONG            lRobotBankStoMul;       //���ٷֱ�

  //���캯��
  tagCustomAndroid()
  {
    DefaultCustomRule();
  }

  void DefaultCustomRule()
  {
    nEnableRobotBanker = TRUE;
    lRobotBankerCount = 5;
    lRobotListMinCount = 2;
    lRobotListMaxCount = 5;
    lRobotApplyBanker = 5;
    lRobotWaitBanker = 3;

    lRobotMinBetTime = 6;
    lRobotMaxBetTime = 8;
    lRobotMinJetton = 100;
    lRobotMaxJetton = 5000000;
    lRobotBetMinCount = 4;
    lRobotBetMaxCount = 8;
    lRobotAreaLimit = 10000000;


    lRobotGetCondition = 10000;
    lRobotSaveCondition = 30000000;
    lRobotGetMin = 100000;
    lRobotGetMax = 500000;
    lRobotBankStoMul = 50;
  }
};
//��������Ϣ
struct tagRobotInfo
{
  int nChip[6];                           //���붨��
  int nAreaChance[AREA_COUNT];                    //������
  TCHAR szCfgFileName[MAX_PATH];                    //�����ļ�
  int nMaxTime;                           //�������

  tagRobotInfo()
  {
    int nTmpChip[6] = {100,1000,10000,100000,1000000,5000000};
    int nTmpAreaChance[AREA_COUNT] = {1, 2, 2, 5, 10, 10, 10, 10};
    TCHAR szTmpCfgFileName[MAX_PATH] = _T("BumperCarBattle.ini");

    nMaxTime = 10;
    memcpy(nChip, nTmpChip, sizeof(nChip));
    memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
    memcpy(szCfgFileName, szTmpCfgFileName, sizeof(szCfgFileName));
  }
};

//��¼��Ϣ
struct tagServerGameRecord
{
  BYTE              bWinMen[AREA_COUNT+1];            //˳��ʤ��
};

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
#define SUB_S_CHECK_IMAGE     109                 //ȡ������
#define SUB_S_ADMIN_COMMDN      110                 //ϵͳ����


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
  DOUBLE              lScore;               //��һ���

  //ׯ����Ϣ
  WORD              wCurrentBankerChairID;        //��ǰׯ��
  BYTE              cbBankerTime;           //ׯ�Ҿ���
  DOUBLE              lCurrentBankerScore;        //ׯ�ҷ���
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
  LONGLONG            lBankerScore;           //ׯ�ҽ��
};

//��Ϸ״̬
struct CMD_S_StatusFree
{
  //ȫ����Ϣ
  BYTE              cbTimeLeave;            //ʣ��ʱ��

  //�����Ϣ
  LONGLONG            lUserMaxScore;              //��ҽ��

  //ׯ����Ϣ
  WORD              wBankerUser;            //��ǰׯ��
  WORD              cbBankerTime;           //ׯ�Ҿ���
  LONGLONG            lBankerWinScore;          //ׯ�ҳɼ�
  LONGLONG            lBankerScore;           //ׯ�ҷ���
  bool              bEnableSysBanker;         //ϵͳ��ׯ

  //������Ϣ
  LONGLONG            lApplyBankerCondition;        //��������
  LONGLONG            lAreaLimitScore;          //��������

  TCHAR             szGameRoomName[32];     //��������
  TCHAR				szRoomTotalName[256];
  int				nMultiple;
  LONGLONG            lStorageStart;
  int               CheckImage;
  tagCustomAndroid        CustomAndroidConfig;        //����������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
  //ȫ����ע
  LONGLONG            lAllJettonScore[AREA_COUNT+1];    //ȫ����ע

  //�����ע
  LONGLONG            lUserJettonScore[AREA_COUNT+1];   //������ע

  //��һ���
  LONGLONG            lUserMaxScore;            //�����ע

  //������Ϣ
  LONGLONG            lApplyBankerCondition;        //��������
  LONGLONG            lAreaLimitScore;          //��������

  //�˿���Ϣ
  BYTE              cbTableCardArray[1][1];       //�����˿�

  //ׯ����Ϣ
  WORD              wBankerUser;            //��ǰׯ��
  WORD              cbBankerTime;           //ׯ�Ҿ���
  LONGLONG            lBankerWinScore;          //ׯ��Ӯ��
  LONGLONG            lBankerScore;           //ׯ�ҷ���
  bool              bEnableSysBanker;         //ϵͳ��ׯ

  //������Ϣ
  LONGLONG            lEndBankerScore;          //ׯ�ҳɼ�
  LONGLONG            lEndUserScore;            //��ҳɼ�
  LONGLONG            lEndUserReturnScore;        //���ػ���
  LONGLONG            lEndRevenue;            //��Ϸ˰��
  LONGLONG           lStorageStart;
  //ȫ����Ϣ
  BYTE              cbTimeLeave;            //ʣ��ʱ��
  BYTE              cbGameStatus;           //��Ϸ״̬
  int               CheckImage;
  TCHAR             szGameRoomName[32];     //��������
    TCHAR				szRoomTotalName[256];
  int               nMultiple;
  tagCustomAndroid        CustomAndroidConfig;        //����������
}; 

//��Ϸ����
struct CMD_S_GameFree
{
  BYTE              cbTimeLeave;            //ʣ��ʱ��
  INT64                             nListUserCount;           //�б�����
  LONGLONG            lStorageStart;
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
  WORD              wBankerUser;            //ׯ��λ��
  LONGLONG            lBankerScore;           //ׯ�ҽ��
  LONGLONG            lUserMaxScore;            //�ҵĽ��
  BYTE              cbTimeLeave;            //ʣ��ʱ��
  bool              bContiueCard;           //��������
  int               nChipRobotCount;          //�������� (��ע������)
  int               nAndriodCount;          //�������� (��ע������)
};

//�û���ע
struct CMD_S_PlaceJetton
{
  WORD              wChairID;             //�û�λ��
  BYTE              cbJettonArea;           //��������
  LONGLONG            lJettonScore;           //��ע��Ŀ
  BYTE              cbAndroid;              //������
  bool              bAndroid;             //
};

//��Ϸ����
struct CMD_S_GameEnd
{
  LONGLONG            lStorageStart;
  //�¾���Ϣ
  BYTE              cbTimeLeave;            //ʣ��ʱ��

  //�˿���Ϣ
  BYTE              cbTableCardArray[1][1];       //�����˿�
  BYTE              cbLeftCardCount;          //�˿���Ŀ

  BYTE              bcFirstCard;

  //ׯ����Ϣ
  LONGLONG            lBankerScore;           //ׯ�ҳɼ�
  LONGLONG            lBankerTotallScore;         //ׯ�ҳɼ�
  INT               nBankerTime;            //��ׯ����

  //��ҳɼ�
  LONGLONG            lUserScore;             //��ҳɼ�
  LONGLONG            lUserReturnScore;         //���ػ���
  LONGLONG                        lUserScoreTotal[GAME_PLAYER];//ÿ��ѡ�ַ������Ӯ��
  //ȫ����Ϣ
  LONGLONG            lRevenue;             //��Ϸ˰��
  LONGLONG            storageStart;                    //��Ϸ���
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_PLACE_JETTON      1                 //�û���ע
#define SUB_C_APPLY_BANKER      2                 //����ׯ��
#define SUB_C_CANCEL_BANKER     3                 //ȡ������
#define SUB_C_CONTINUE_CARD     4                 //��������
#define SUB_C_CHECK_IMAGE     5                 //��������
#define SUB_C_ADMIN_COMMDN      6                 //ϵͳ����
#define SUB_C_UPDATE_STORAGE        7                 //���¿��
#define SUB_C_DEUCT         8                 //���¿��

//�û���ע
struct CMD_C_PlaceJetton
{
  BYTE              cbJettonArea;           //��������
  LONGLONG            lJettonScore;           //��ע��Ŀ
};

struct CMD_C_CheckImage
{
  int Index;
};

//////////////////////////////////////////////////////////////////////////

#define IDM_ADMIN_COMMDN WM_USER+1000
#define IDM_ADMIN_STORAGE     WM_USER+1001
#define IDM_ADMIN_DEUCT       WM_USER+1002

//����������Ϣ
struct tagControlInfo
{
  BYTE cbControlArea;           //��������
};

struct CMD_C_FreshStorage
{
  BYTE                            cbReqType;            //��������
  LONGLONG            lStorageStart;        //��ʼ���
  LONGLONG            lStorageDeduct;       //���˥��
  LONGLONG            lStorageCurrent;        //��ǰ���
  LONGLONG            lStorageMax1;         //�������1
  LONGLONG            lStorageMul1;         //ϵͳ��ָ���1
  LONGLONG            lStorageMax2;         //�������2
  LONGLONG            lStorageMul2;         //ϵͳ��ָ���2
};

struct CMD_C_FreshDeuct
{
  LONGLONG            lStorageDeuct;            //���˥����ֵ

};

//���������Ʒ���
#define  S_CR_FAILURE       0   //ʧ��
#define  S_CR_UPDATE_SUCCES     1   //���³ɹ�
#define  S_CR_SET_SUCCESS     2   //���óɹ�
#define  S_CR_CANCEL_SUCCESS    3   //ȡ���ɹ�
struct CMD_S_ControlReturns
{
  BYTE cbReturnsType;       //�ظ�����
  BYTE cbControlArea;       //��������
  BYTE cbControlTimes;      //���ƴ���
};


//�ͻ��˿�������
#define  C_CA_UPDATE        1   //����
#define  C_CA_SET         2   //����
#define  C_CA_CANCELS       3   //ȡ��
struct CMD_C_ControlApplication
{
  BYTE cbControlAppType;      //��������
  BYTE cbControlArea;       //��������
  BYTE cbControlTimes;      //���ƴ���
};
struct CMD_C_AdminReq
{
  BYTE cbReqType;
#define RQ_SET_WIN_AREA 1
#define RQ_RESET_CONTROL  2
#define RQ_PRINT_SYN    3
  BYTE cbExtendData[20];      //��������
};
//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
