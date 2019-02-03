#ifndef CMD_GAME_SERVER_HEAD_FILE
#define CMD_GAME_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//��¼����

#define MDM_GR_LOGON				1									//��¼��Ϣ

//��¼ģʽ
#define SUB_GR_LOGON_USERID			1									//I D ��¼
#define SUB_GR_LOGON_MOBILE			2									//�ֻ���¼
#define SUB_GR_LOGON_ACCOUNTS		3									//�ʻ���¼

//��¼���
#define SUB_GR_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_GR_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_GR_LOGON_FINISH			102									//��¼���

//������ʾ
#define SUB_GR_UPDATE_NOTIFY		200									//������ʾ

//////////////////////////////////////////////////////////////////////////////////

//I D ��¼
struct CMD_GR_LogonUserID
{
	//�汾��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	DWORD							dwFrameVersion;						//��ܰ汾
	DWORD							dwProcessVersion;					//���̰汾

	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR                           szServerPasswd[LEN_PASSWORD];       //��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	WORD							wKindID;							//��������
};

//�ֻ���¼
struct CMD_GR_LogonMobile
{
	//�汾��Ϣ
	WORD							wGameID;							//��Ϸ��ʶ
	DWORD							dwProcessVersion;					//���̰汾

	//��������
	BYTE                            cbDeviceType;                       //�豸����
	WORD                            wBehaviorFlags;                     //��Ϊ��ʶ
	WORD                            wPageTableCount;                    //��ҳ����

	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR                           szServerPasswd[LEN_PASSWORD];       //��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//�ʺŵ�¼
struct CMD_GR_LogonAccounts
{
	//�汾��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	DWORD							dwFrameVersion;						//��ܰ汾
	DWORD							dwProcessVersion;					//���̰汾

	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��¼�ɹ�
struct CMD_GR_LogonSuccess
{
	DWORD							dwUserRight;						//�û�Ȩ��
	DWORD							dwMasterRight;						//����Ȩ��
};

//��¼ʧ��
struct CMD_GR_LogonFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��¼���
struct CMD_GR_LogonFinish
{
	bool							bGuideTask;							//��������
};

//������ʾ
struct CMD_GR_UpdateNotify
{
	//������־
	BYTE							cbMustUpdatePlaza;					//ǿ������
	BYTE							cbMustUpdateClient;					//ǿ������
	BYTE							cbAdviceUpdateClient;				//��������

	//��ǰ�汾
	DWORD							dwCurrentPlazaVersion;				//��ǰ�汾
	DWORD							dwCurrentFrameVersion;				//��ǰ�汾
	DWORD							dwCurrentClientVersion;				//��ǰ�汾
};

//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_CONFIG				2									//������Ϣ

#define SUB_GR_CONFIG_COLUMN		100									//�б�����
#define SUB_GR_CONFIG_SERVER		101									//��������
#define SUB_GR_CONFIG_PROPERTY		102									//��������
#define SUB_GR_CONFIG_FINISH		103									//�������
#define SUB_GR_CONFIG_USER_RIGHT	104									//���Ȩ��

//////////////////////////////////////////////////////////////////////////////////

//�б�����
struct CMD_GR_ConfigColumn
{
	BYTE							cbColumnCount;						//�б���Ŀ
	tagColumnItem					ColumnItem[MAX_COLUMN];				//�б�����
};

//��������
struct CMD_GR_ConfigServer
{
	//��������
	WORD							wTableCount;						//������Ŀ
	WORD							wChairCount;						//������Ŀ

	//��������
	WORD							wServerType;						//��������
	DWORD							dwServerRule;						//�������
};


//���Ȩ��
struct CMD_GR_ConfigUserRight
{
	DWORD							dwUserRight;						//���Ȩ��
};
//////////////////////////////////////////////////////////////////////////////////
//�û�����

#define MDM_GR_USER					3									//�û���Ϣ

//�û�����
#define SUB_GR_USER_RULE			1									//�û�����
#define SUB_GR_USER_LOOKON			2									//�Թ�����
#define SUB_GR_USER_SITDOWN			3									//��������
#define SUB_GR_USER_STANDUP			4									//��������
#define SUB_GR_USER_INVITE			5									//�û�����
#define SUB_GR_USER_INVITE_REQ		6									//��������
#define SUB_GR_USER_REPULSE_SIT  	7									//�ܾ��������
#define SUB_GR_USER_KICK_USER       8                                   //�߳��û�
#define SUB_GR_USER_INFO_REQ        9                                   //�����û���Ϣ
#define SUB_GR_USER_CHAIR_REQ       10                                  //�������λ��
#define SUB_GR_USER_CHAIR_INFO_REQ  11                                  //���������û���Ϣ
#define SUB_GR_USER_WAIT_DISTRIBUTE 12									//�ȴ�����

//�û�״̬
#define SUB_GR_USER_ENTER			100									//�û�����
#define SUB_GR_USER_SCORE			101									//�û�����
#define SUB_GR_USER_STATUS			102									//�û�״̬
#define SUB_GR_REQUEST_FAILURE		103									//����ʧ��
#define SUB_GR_USER_GAME_DATA		104									//�û���Ϸ����

//��������
#define SUB_GR_USER_CHAT			201									//������Ϣ
#define SUB_GR_USER_EXPRESSION		202									//������Ϣ
#define SUB_GR_WISPER_CHAT			203									//˽����Ϣ
#define SUB_GR_WISPER_EXPRESSION	204									//˽�ı���
#define SUB_GR_COLLOQUY_CHAT		205									//�Ự��Ϣ
#define SUB_GR_COLLOQUY_EXPRESSION	206									//�Ự����

//�ȼ�����
#define SUB_GR_GROWLEVEL_QUERY			410								//��ѯ�ȼ�
#define SUB_GR_GROWLEVEL_PARAMETER		411								//�ȼ�����
#define SUB_GR_GROWLEVEL_UPGRADE		412								//�ȼ�����
//////////////////////////////////////////////////////////////////////////////////

//�Թ�����
struct CMD_GR_UserLookon
{
	WORD							wTableID;							//����λ��
	WORD							wChairID;							//����λ��
};

//��������
struct CMD_GR_UserSitDown
{
	WORD							wTableID;							//����λ��
	WORD							wChairID;							//����λ��
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//��������
struct CMD_GR_UserStandUp
{
	WORD							wTableID;							//����λ��
	WORD							wChairID;							//����λ��
	BYTE							cbForceLeave;						//ǿ���뿪
};

//�����û� 
struct CMD_GR_UserInvite
{
	WORD							wTableID;							//���Ӻ���
	DWORD							dwUserID;							//�û� I D
};

//�����û����� 
struct CMD_GR_UserInviteReq
{
	WORD							wTableID;							//���Ӻ���
	DWORD							dwUserID;							//�û� I D
};

//�û�����
struct CMD_GR_UserScore
{
	DWORD							dwUserID;							//�û���ʶ
	tagUserScore					UserScore;							//������Ϣ
};

//�û�����
struct CMD_GR_MobileUserScore
{
	DWORD							dwUserID;							//�û���ʶ
	tagMobileUserScore				UserScore;							//������Ϣ
};

//�û�״̬
struct CMD_GR_UserStatus
{
	DWORD							dwUserID;							//�û���ʶ
	tagUserStatus					UserStatus;							//�û�״̬
};

//�û���Ϸ����
struct CMD_GR_UserGameData
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szUserGameData[LEN_GAME_DATA];		//��Ϸ����					
};

//����ʧ��
struct CMD_GR_RequestFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[256];				//������Ϣ
};


//�û�����
struct CMD_GR_C_UserChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�����
struct CMD_GR_S_UserChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�����
struct CMD_GR_C_UserExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û�����
struct CMD_GR_S_UserExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û�˽��
struct CMD_GR_C_WisperChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�˽��
struct CMD_GR_S_WisperChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//˽�ı���
struct CMD_GR_C_WisperExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//˽�ı���
struct CMD_GR_S_WisperExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û��Ự
struct CMD_GR_ColloquyChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwConversationID;					//�Ự��ʶ
	DWORD							dwTargetUserID[16];					//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�����û�
struct CMD_GR_C_InviteUser
{
	WORD							wTableID;							//���Ӻ���
	DWORD							dwSendUserID;						//�����û�
};

//�����û�
struct CMD_GR_S_InviteUser
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û��ܾ�����������
struct CMD_GR_UserRepulseSit
{
	WORD							wTableID;							//���Ӻ���
	WORD							wChairID;							//����λ��
	DWORD							dwUserID;							//�û� I D
	DWORD							dwRepulseUserID;					//�û� I D
};

//////////////////////////////////////////////////////////////////////////////////

//�����־
#define UR_LIMIT_SAME_IP			0x01								//���Ƶ�ַ
#define UR_LIMIT_WIN_RATE			0x02								//����ʤ��
#define UR_LIMIT_FLEE_RATE			0x04								//��������
#define UR_LIMIT_GAME_SCORE			0x08								//���ƻ���

//�û�����
struct CMD_GR_UserRule
{
	BYTE							cbRuleMask;							//��������
	WORD							wMinWinRate;						//���ʤ��
	WORD							wMaxFleeRate;						//�������
	LONG							lMaxGameScore;						//��߷��� 
	LONG							lMinGameScore;						//��ͷ���
};

//�����û���Ϣ
struct CMD_GR_UserInfoReq
{
	DWORD                           dwUserIDReq;                        //�����û�
	WORD							wTablePos;							//����λ��
};

//�����û���Ϣ
struct CMD_GR_ChairUserInfoReq
{
	WORD							wTableID;							//���Ӻ���
	WORD							wChairID;							//����λ��
};

//////////////////////////////////////////////////////////////////////////////////
//�ȼ�����

//��ѯ�ȼ�
struct CMD_GR_GrowLevelQueryInfo
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�ȼ�����
struct CMD_GR_GrowLevelConfig
{
	WORD							wLevelCount;						//�ȼ���Ŀ
	tagGrowLevelConfig				GrowLevelItem[60];					//�ȼ�����
};

//�ȼ�����
struct CMD_GR_GrowLevelParameter
{
	WORD							wCurrLevelID;						//��ǰ�ȼ�
	DWORD							dwExperience;						//��ǰ����
	DWORD							dwUpgradeExperience;				//�¼�����
	SCORE							lUpgradeRewardGold;					//��������
	SCORE							lUpgradeRewardIngot;				//��������
};

//�ȼ�����
struct CMD_GR_GrowLevelUpgrade
{
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrIngot;							//��ǰԪ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//////////////////////////////////////////////////////////////////////////////////
//״̬����

#define MDM_GR_STATUS				4									//״̬��Ϣ

#define SUB_GR_TABLE_INFO			100									//������Ϣ
#define SUB_GR_TABLE_STATUS			101									//����״̬

//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
struct CMD_GR_TableInfo
{
	WORD							wTableCount;						//������Ŀ
	tagTableStatus					TableStatusArray[512];				//����״̬
};

//����״̬
struct CMD_GR_TableStatus
{
	WORD							wTableID;							//���Ӻ���
	tagTableStatus					TableStatus;						//����״̬
};

//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_INSURE				5									//�û���Ϣ

//��������
#define SUB_GR_ENABLE_INSURE_REQUEST 1									//��ͨ����
#define SUB_GR_QUERY_INSURE_INFO	2									//��ѯ����
#define SUB_GR_SAVE_SCORE_REQUEST	3									//������
#define SUB_GR_TAKE_SCORE_REQUEST	4									//ȡ�����
#define SUB_GR_TRANSFER_SCORE_REQUEST	5								//ȡ�����
#define SUB_GR_QUERY_USER_INFO_REQUEST	6								//��ѯ�û�

#define SUB_GR_USER_INSURE_INFO		100									//��������
#define SUB_GR_USER_INSURE_SUCCESS	101									//���гɹ�
#define SUB_GR_USER_INSURE_FAILURE	102									//����ʧ��
#define SUB_GR_USER_TRANSFER_USER_INFO	103								//�û�����
#define SUB_GR_USER_INSURE_ENABLE_RESULT 104							//��ͨ���

//////////////////////////////////////////////////////////////////////////////////

//��ͨ����
struct CMD_GR_C_EnableInsureRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwUserID;							//�û�I D
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ����
struct CMD_GR_C_QueryInsureInfoRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
};

//�������
struct CMD_GR_C_SaveScoreRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	SCORE							lSaveScore;							//�����Ŀ
};

//ȡ������
struct CMD_GR_C_TakeScoreRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	SCORE							lTakeScore;							//ȡ����Ŀ
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
};

//ת�˽��
struct CMD_GP_C_TransferScoreRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	SCORE							lTransferScore;						//ת�˽��
	TCHAR							szAccounts[LEN_ACCOUNTS];			//Ŀ���û�
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
	TCHAR							szTransRemark[LEN_TRANS_REMARK];	//ת�˱�ע
};

//��ѯ�û�
struct CMD_GR_C_QueryUserInfoRequest
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE                            cbByNickName;                       //�ǳ�����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//Ŀ���û�
};

//��������
struct CMD_GR_S_UserInsureInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE							cbEnjoinTransfer;					//ת�˿���
	WORD							wRevenueTake;						//˰�ձ���
	WORD							wRevenueTransfer;					//˰�ձ���
	WORD							wRevenueTransferMember;				//˰�ձ���	
	WORD							wServerID;							//�����ʶ
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//���н��
	SCORE							lTransferPrerequisite;				//ת������
};

//���гɹ�
struct CMD_GR_S_UserInsureSuccess
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	SCORE							lUserScore;							//���Ͻ��
	SCORE							lUserInsure;						//���н��
	TCHAR							szDescribeString[128];				//������Ϣ
};

//����ʧ��
struct CMD_GR_S_UserInsureFailure
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�û���Ϣ
struct CMD_GR_S_UserTransferUserInfo
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	DWORD							dwTargetGameID;						//Ŀ���û�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//Ŀ���û�
};

//��ͨ���
struct CMD_GR_S_UserInsureEnableResult
{
	BYTE                            cbActivityGame;                     //��Ϸ����
	BYTE							cbInsureEnabled;					//ʹ�ܱ�ʶ
	TCHAR							szDescribeString[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//��������
#define MDM_GR_TASK					6									//��������

#define SUB_GR_TASK_LOAD_INFO		1									//��������
#define SUB_GR_TASK_TAKE			2									//��ȡ����
#define SUB_GR_TASK_REWARD			3									//������
#define SUB_GR_TASK_GIVEUP			4									//�������


#define SUB_GR_TASK_INFO			11									//������Ϣ
#define SUB_GR_TASK_FINISH			12									//�������
#define SUB_GR_TASK_LIST			13									//�����б�
#define SUB_GR_TASK_RESULT			14									//������
#define SUB_GR_TASK_GIVEUP_RESULT	15									//�������

//////////////////////////////////////////////////////////////////////////////////
//��������
struct CMD_GR_C_LoadTaskInfo
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//��������
struct CMD_GR_C_TakeGiveUp
{
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ����
struct CMD_GR_C_TakeTask
{
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ����
struct CMD_GR_C_TaskReward
{
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������Ϣ
struct CMD_GR_S_TaskInfo
{
	WORD							wTaskCount;							//��������
	tagTaskStatus					TaskStatus[TASK_MAX_COUNT];			//����״̬
};

//�������
struct CMD_GR_S_TaskFinish
{
	WORD							wFinishTaskID;						//�����ʶ
	TCHAR							szTaskName[LEN_TASK_NAME];			//��������
};

//������
struct CMD_GR_S_TaskResult
{
	//�����Ϣ
	bool							bSuccessed;							//�ɹ���ʶ
	WORD							wCommandID;							//�����ʶ

	//�Ƹ���Ϣ
	SCORE							lCurrScore;							//��ǰ���
	SCORE							lCurrIngot;							//��ǰԪ��
	
	//��ʾ��Ϣ
	TCHAR							szNotifyContent[128];				//��ʾ����
};


//////////////////////////////////////////////////////////////////////////////////
//�һ�����

#define MDM_GR_EXCHANGE					7								//�һ�����

#define SUB_GR_EXCHANGE_LOAD_INFO		400								//������Ϣ
#define SUB_GR_EXCHANGE_PARAM_INFO		401								//�һ�����
#define SUB_GR_PURCHASE_MEMBER			402								//�����Ա
#define SUB_GR_PURCHASE_RESULT			403								//������
#define SUB_GR_EXCHANGE_SCORE_BYINGOT	404								//�һ���Ϸ��
#define SUB_GR_EXCHANGE_SCORE_BYBEANS	405								//�һ���Ϸ��
#define SUB_GR_EXCHANGE_RESULT			406								//�һ����

//////////////////////////////////////////////////////////////////////////////////

//��ѯ����
struct CMD_GR_ExchangeParameter
{
	DWORD							dwExchangeRate;						//Ԫ����Ϸ�Ҷһ�����
	DWORD							dwPresentExchangeRate;				//������Ϸ�Ҷһ���
	DWORD							dwRateGold;							//��Ϸ����Ϸ�Ҷһ���
	WORD							wMemberCount;						//��Ա��Ŀ
	tagMemberParameter				MemberParameter[10];				//��Ա����
};

//�����Ա
struct CMD_GR_PurchaseMember
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbMemberOrder;						//��Ա��ʶ
	WORD							wPurchaseTime;						//����ʱ��
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//������
struct CMD_GR_PurchaseResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	BYTE							cbMemberOrder;						//��Աϵ��
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	DOUBLE							dCurrBeans;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�һ���Ϸ��
struct CMD_GR_ExchangeScoreByIngot
{
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lExchangeIngot;						//�һ�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//�һ���Ϸ��
struct CMD_GR_ExchangeScoreByBeans
{
	DWORD							dwUserID;							//�û���ʶ
	double							dExchangeBeans;						//�һ�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//�һ����
struct CMD_GR_ExchangeResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrIngot;							//��ǰԪ��
	double							dCurrBeans;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//////////////////////////////////////////////////////////////////////////////////
//��������
#define MDM_GR_PROPERTY				8									//��������

//������Ϣ
#define SUB_GR_QUERY_PROPERTY					1						//���߲�ѯ
#define SUB_GR_GAME_PROPERTY_BUY				2						//�������
#define SUB_GR_PROPERTY_BACKPACK				3						//��������
#define SUB_GR_PROPERTY_USE						4						//��Ʒʹ��
#define SUB_GR_QUERY_SEND_PRESENT				5						//��ѯ����
#define SUB_GR_PROPERTY_PRESENT					6						//���͵���
#define SUB_GR_GET_SEND_PRESENT					7						//��ȡ����

#define SUB_GR_QUERY_PROPERTY_RESULT			101						//���߲�ѯ
#define SUB_GR_QUERY_PROPERTY_RESULT_FINISH		111						//���߲�ѯ
#define SUB_GR_GAME_PROPERTY_BUY_RESULT			102						//�������
#define SUB_GR_PROPERTY_BACKPACK_RESULT			103						//��������
#define SUB_GR_PROPERTY_USE_RESULT				104						//��Ʒʹ��
#define SUB_GR_QUERY_SEND_PRESENT_RESULT		105						//��ѯ����
#define SUB_GR_PROPERTY_PRESENT_RESULT			106						//���͵���
#define SUB_GR_GET_SEND_PRESENT_RESULT			107						//��ȡ����

#define SUB_GR_PROPERTY_SUCCESS					201						//���߳ɹ�
#define SUB_GR_PROPERTY_FAILURE					202						//����ʧ��
#define SUB_GR_PROPERTY_MESSAGE					203                     //������Ϣ
#define SUB_GR_PROPERTY_EFFECT					204                     //����ЧӦ
#define SUB_GR_PROPERTY_TRUMPET					205                     //������Ϣ
#define SUB_GR_USER_PROP_BUFF					206						//����Buff
#define SUB_GR_USER_TRUMPET						207						//��������

#define SUB_GR_GAME_PROPERTY_FAILURE			404						//����ʧ��


//����ʧ��
struct CMD_GR_GamePropertyFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�������
struct CMD_GR_PropertyBuy
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwItemCount;						//������Ŀ
	BYTE							cbConsumeType;						//��������
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};


//������
struct CMD_GR_PropertyBuyResult
{
	//������Ϣ
	DWORD							dwUserID;							//�û� I D
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwItemCount;						//������Ŀ
	LONGLONG						lInsureScore;						//���д��
	LONGLONG						lUserMedal;							//�û�Ԫ��
	LONGLONG						lLoveLiness;						//����ֵ
	double							dCash;								//��Ϸ��
	BYTE							cbCurrMemberOrder;					//��Ա�ȼ�
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�������
struct CMD_GR_C_GamePropertyUse
{
	//BYTE                            cbRequestArea;					//����Χ
	BYTE							cbConsumeType;						//��������
	WORD							wItemCount;							//������Ŀ
	WORD							wPropertyIndex;						//��������	
	DWORD							dwUserID;							//ʹ�ö���
};



//���߳ɹ�
struct CMD_GR_S_PropertySuccess
{
	BYTE                            cbRequestArea;						//ʹ�û���
	WORD							wKind;								//��������
	WORD							wItemCount;							//������Ŀ
	WORD							wPropertyIndex;						//��������
	DWORD							dwSourceUserID;						//Ŀ�����
	DWORD							dwTargetUserID;						//ʹ�ö���
};

//����ʧ��
struct CMD_GR_PropertyFailure
{
	WORD                            wRequestArea;                       //��������
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[256];				//������Ϣ
};

//������Ϣ
struct CMD_GR_S_PropertyMessage
{
	//������Ϣ
	WORD                            wPropertyIndex;                     //��������
	WORD                            wPropertyCount;                     //������Ŀ
	DWORD                           dwSourceUserID;                     //Ŀ�����
	DWORD                           dwTargerUserID;                     //ʹ�ö���
};

//�������
struct DBO_GR_QueryBackpack
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwStatus;							//״̬��ʶ
	DWORD							dwCount;							//����
	tagBackpackProperty				PropertyInfo[1];					//������Ϣ
};


//����ЧӦ
struct CMD_GR_S_PropertyEffect
{
	DWORD                           wUserID;					        //�� ��I D
	BYTE							wPropKind;							//��������
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	DWORD							dwFleeCount;						//���ܾ���
	SCORE							lScore;								//��������
};

//��������
struct CMD_GR_C_SendTrumpet
{
	BYTE                           cbRequestArea;                       //����Χ 
	WORD                           wPropertyIndex;                      //�������� 
	DWORD                          TrumpetColor;                        //������ɫ
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //��������
};

//��������
struct CMD_GR_S_SendTrumpet
{
	WORD                           wPropertyIndex;                      //�������� 
	DWORD                          dwSendUserID;                        //�û� I D
	DWORD                          TrumpetColor;                        //������ɫ
	TCHAR                          szSendNickName[32];				    //����ǳ� 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //��������
};

//��������
struct CMD_GR_C_BackpackProperty
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwKindID;							//��������
};

//��������
struct CMD_GR_S_BackpackProperty
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwStatus;							//״̬
	DWORD							dwCount;							//����
	tagBackpackProperty				PropertyInfo[1];					//������Ϣ
};

//ʹ�õ���
struct CMD_GR_C_PropertyUse
{
	DWORD							dwUserID;							//ʹ����
	DWORD							dwRecvUserID;						//��˭ʹ��
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
};

//ʹ�õ���
struct CMD_GR_S_PropertyUse
{
	DWORD							dwUserID;							//ʹ����
	WORD							wUseArea;							//������Χ
	WORD							wServiceArea;						//����Χ
	DWORD							dwRecvUserID;						//��˭ʹ��
	DWORD							dwPropID;							//����ID
	DWORD							wPropCount;							//ʹ����Ŀ
	DWORD							dwRemainderPropCount;				//ʣ������
	SCORE							Score;								//��Ϸ���

	SCORE							lSendLoveLiness;					//��������
	SCORE							lRecvLoveLiness;					//��������
	SCORE							lUseResultsGold;					//��ý��					
	DWORD							dwPropKind;							//��������
	time_t							tUseTime;							//ʹ�õ�ʱ��
	DWORD							UseResultsValidTime;				//��Чʱ��(��)
	DWORD							dwHandleCode;						//������
	TCHAR							szName[16];							//��������
	BYTE							cbMemberOrder;						//��Ա��ʶ
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//����Buff
struct CMD_GR_S_PropertyBuff
{
	DWORD							dwUserID;
	BYTE							cbBuffCount;						//Buff��Ŀ
	tagPropertyBuff					PropertyBuff[MAX_BUFF];	
};

//�������
struct CMD_GR_S_UserTrumpet
{
	DWORD							dwTrumpetCount;						//С������
	DWORD							dwTyphonCount;						//��������	
};

//��ѯ����
struct CMD_GR_C_QuerySendPresent
{
	DWORD							dwUserID;							//�û� I D
};

//��ѯ����
struct CMD_GR_S_QuerySendPresent
{
	WORD							wPresentCount;						//���ʹ���
	SendPresent						Present[MAX_PROPERTY];		
};

//��ȡ����
struct CMD_GR_C_GetSendPresent
{
	DWORD							dwUserID;							//������
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//��ȡ����
struct CMD_GR_S_GetSendPresent
{
	WORD							wPresentCount;						//���ʹ���
	SendPresent						Present[MAX_PROPERTY];	
};

//���͵���
struct CMD_GR_C_PropertyPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwRecvGameID;						//���߸�˭
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
	WORD							wType;								//Ŀ������
	TCHAR							szRecvNickName[16];					//��˭ʹ��
};

//���͵���
struct CMD_GR_S_PropertyPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwRecvGameID;						//���߸�˭
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
	WORD							wType;								//Ŀ������
	TCHAR							szRecvNickName[16];					//��˭ʹ��
	int								nHandleCode;						//������
	TCHAR							szNotifyContent[64];				//��ʾ����
};




//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_MANAGE				9									//��������

#define SUB_GR_SEND_WARNING			1									//���;���
#define SUB_GR_SEND_MESSAGE			2									//������Ϣ
#define SUB_GR_LOOK_USER_IP			3									//�鿴��ַ
#define SUB_GR_KILL_USER			4									//�߳��û�
#define SUB_GR_LIMIT_ACCOUNS		5									//�����ʻ�
#define SUB_GR_SET_USER_RIGHT		6									//Ȩ������

//��������
#define SUB_GR_QUERY_OPTION			7									//��ѯ����
#define SUB_GR_OPTION_SERVER		8									//��������
#define SUB_GR_OPTION_CURRENT		9									//��ǰ����
#define SUB_GR_LIMIT_USER_CHAT		10									//��������
#define SUB_GR_KICK_ALL_USER		11									//�߳��û�
#define SUB_GR_DISMISSGAME		    12									//��ɢ��Ϸ

//////////////////////////////////////////////////////////////////////////////////

//���;���
struct CMD_GR_SendWarning
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szWarningMessage[LEN_USER_CHAT];	//������Ϣ
};

//ϵͳ��Ϣ
struct CMD_GR_SendMessage
{
	BYTE							cbGame;								//�Ƿ���Ϣ�㲥����Ϸ
	BYTE							cbRoom;								//�Ƿ���Ϣ�㲥�����䣨���ӽ��棩
	BYTE							cbAllRoom;							//�Ƿ���Ϣ�㲥��������Ϸ
	BYTE                            cbLoop;                             //ѭ����־
	DWORD                           dwTimeRate;                         //ѭ�����
	__time64_t                      tConcludeTime;                      //����ʱ��
	WORD							wChatLength;						//��Ϣ����
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//ϵͳ��Ϣ
};

//�鿴��ַ
struct CMD_GR_LookUserIP
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�߳��û�
struct CMD_GR_KickUser
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�����ʻ�
struct CMD_GR_LimitAccounts
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//Ȩ������
struct CMD_GR_SetUserRight
{
	//Ŀ���û�
	DWORD							dwTargetUserID;						//Ŀ���û�

	//�󶨱���
	BYTE							cbGameRight;						//�ʺ�Ȩ��
	BYTE							cbAccountsRight;					//�ʺ�Ȩ��

	//Ȩ�ޱ仯
	BYTE							cbLimitRoomChat;					//��������
	BYTE							cbLimitGameChat;					//��Ϸ����
	BYTE							cbLimitPlayGame;					//��ϷȨ��
	BYTE							cbLimitSendWisper;					//������Ϣ
	BYTE							cbLimitLookonGame;					//�Թ�Ȩ��
};

//��������
struct CMD_GR_OptionCurrent
{
	DWORD							dwRuleMask;							//��������
	tagServerOptionInfo				ServerOptionInfo;					//��������
};

//��������
struct CMD_GR_ServerOption
{
	tagServerOptionInfo				ServerOptionInfo;					//��������
};

//�߳������û�
struct CMD_GR_KickAllUser
{
	TCHAR							szKickMessage[LEN_USER_CHAT];		//�߳���ʾ
};

//��ɢ��Ϸ
struct CMD_GR_DismissGame
{
	WORD							wDismissTableNum;		            //��ɢ����
};
//////////////////////////////////////////////////////////////////////////////////

//���ñ�־
#define OSF_ROOM_CHAT				1									//��������
#define OSF_GAME_CHAT				2									//��Ϸ����
#define OSF_ROOM_WISPER				3									//����˽��
#define OSF_ENTER_TABLE				4									//������Ϸ
#define OSF_ENTER_SERVER			5									//���뷿��
#define OSF_SEND_BUGLE				12									//��������

//��������
struct CMD_GR_OptionServer
{
	BYTE							cbOptionFlags;						//���ñ�־
	BYTE							cbOptionValue;						//���ñ�־
};

//��������
struct CMD_GR_LimitUserChat
{
	DWORD							dwTargetUserID;						//Ŀ���û�
	BYTE							cbLimitFlags;						//���Ʊ�־
	BYTE							cbLimitValue;						//�������
};

//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_MATCH				10									//��������

#define SUB_GR_MATCH_INFO			1									//������Ϣ	
#define SUB_GR_MATCH_RULE			2									//��������
#define SUB_GR_MATCH_FEE			3									//��������
#define SUB_GR_MATCH_NUM			4									//�ȴ�����
#define SUB_GR_LEAVE_MATCH			5									//�˳�����
#define SUB_GR_MATCH_SAFECARD		6									//��������
#define SUB_GR_MATCH_BUY_SAFECARD	7									//������
#define SUB_GR_MATCH_WAIT_TIP		8									//�ȴ���ʾ
#define SUB_GR_MATCH_RESULT			9									//�������
#define SUB_GR_MATCH_STATUS			10									//����״̬
#define SUB_GR_MATCH_USTATUS		11									//����״̬	
#define SUB_GR_MATCH_DESC			12									//��������
#define SUB_GR_MATCH_GOLDUPDATE		13									//��Ҹ���
#define SUB_GR_MATCH_ELIMINATE		14									//������̭
#define SUB_GR_MATCH_REVIVE			15									//��������
#define SUB_GR_MATCH_USER_REVIVE	16									//�û�����


////////////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_GR_Match_Rule
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ	
	BYTE							cbMatchType;						//��������
	TCHAR							szMatchName[32];					//��������

	//������Ϣ
	BYTE							cbFeeType;							//��������
	BYTE							cbDeductArea;						//�ɷ�����
	LONGLONG						lSignupFee;							//��������	
	BYTE							cbSignupMode;						//������ʽ
	BYTE							cbJoinCondition;					//��������
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	DWORD							dwExperience;						//��Ҿ���

	//������ʽ
	BYTE							cbRankingMode;						//������ʽ	
	WORD							wCountInnings;						//ͳ�ƾ���
	BYTE							cbFilterGradesMode;					//ɸѡ��ʽ
	
	//������Ϣ
	WORD							wRewardCount;						//��������
	tagMatchRewardInfo				MatchRewardInfo[3];					//��������
	//��������
	BYTE							cbMatchRule[512];					//��������
};

//��������
struct CMD_GR_Match_Revive
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNO;							//��������

	//������Ϣ
	LONGLONG						lReviveFee;							//�������
	BYTE							cbReviveTimes;						//�������
	BYTE							cbReviveTimesed;					//�������
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//��������
struct CMD_GR_Match_SafeCard
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNO;							//��������

	//���շ���
	LONGLONG						lSafeCardFee;						//���շ���
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//������
struct CMD_GR_Match_BuySafeCard
{
	bool							bBuySafeCard;						//�����ʶ
};

//��������
struct CMD_GR_Match_Fee
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNO;							//��������

	//������Ϣ
	BYTE							cbSignupMode;						//������ʽ
	BYTE							cbFeeType;							//��������
	BYTE							cbDeductArea;						//�۷�����
	LONGLONG						lSignupFee;							//��������
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//��������
struct CMD_GR_Match_Num
{
	DWORD							dwWaitting;							//�ȴ�����
	DWORD							dwTotal;							//��������
};

//������Ϣ
struct CMD_GR_Match_Info
{
	TCHAR							szTitle[4][64];						//��Ϣ����
    WORD							wGameCount;							//��Ϸ����
};

//��ʾ��Ϣ
struct CMD_GR_Match_Wait_Tip
{
	LONGLONG						lScore;								//��ǰ����
	WORD							wRank;								//��ǰ����
	WORD							wCurTableRank;						//��������
	WORD							wUserCount;							//��ǰ����
	WORD							wCurGameCount;						//��ǰ����
	WORD							wGameCount;							//�ܹ�����
	WORD							wPlayingTable;						//��Ϸ����
	TCHAR							szMatchName[LEN_SERVER];			//��������
};

//�������
struct CMD_GR_MatchResult
{	
	//������Ϣ
	SCORE							lGold;								//��ҽ���
	SCORE							lIngot;								//Ԫ������
	DWORD							dwExperience;						//���齱��
	TCHAR							szNickName[LEN_ACCOUNTS];			//����ǳ�

	//������Ϣ
	WORD							wMatchRank;							//��������
	TCHAR							szMatchName[64];					//��������	
	SYSTEMTIME						MatchEndTime;						//����ʱ��	
};

#define MAX_MATCH_DESC				4									//�������

//��������
struct CMD_GR_MatchDesc
{
	TCHAR							szTitle[MAX_MATCH_DESC][16];		//��Ϣ����
	TCHAR							szDescribe[MAX_MATCH_DESC][64];		//��������
	COLORREF						crTitleColor;						//������ɫ
	COLORREF						crDescribeColor;					//������ɫ
};

//��Ҹ���
struct CMD_GR_MatchGoldUpdate
{
	LONGLONG						lCurrGold;							//��ǰ���
};

//�û�����
struct CMD_GR_Match_UserRevive
{
	bool							bUseSafeCard;						//ʹ�ñ��տ�
};

//////////////////////////////////////////////////////////////////////////

//�ͱ�����
#define MDM_GR_BASEENSURE				11

#define SUB_GR_BASEENSURE_LOAD			260								//���صͱ�
#define SUB_GR_BASEENSURE_TAKE			261								//��ȡ�ͱ�
#define SUB_GR_BASEENSURE_PARAMETER		262								//�ͱ�����
#define SUB_GR_BASEENSURE_RESULT		263								//�ͱ����

//��ȡ�ͱ�
struct CMD_GR_BaseEnsureTake
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�ͱ�����
struct CMD_GR_BaseEnsureParamter
{
	SCORE							lScoreCondition;					//��Ϸ������
	SCORE							lScoreAmount;						//��Ϸ������
	BYTE							cbTakeTimes;						//��ȡ����	
};

//�ͱ����
struct CMD_GR_BaseEnsureResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lGameScore;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};
//////////////////////////////////////////////////////////////////////////
//ǩ������
#define MDM_GR_CHECKIN					12
#define SUB_GR_CHECKIN_QUERY			220								//��ѯǩ��
#define SUB_GR_CHECKIN_INFO				221								//ǩ����Ϣ
#define SUB_GR_CHECKIN_DONE				222								//ִ��ǩ��
#define SUB_GR_CHECKIN_RESULT			223								//ǩ�����

//��ѯǩ��
struct CMD_GR_CheckInQueryInfo
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//ǩ����Ϣ
struct CMD_GR_CheckInInfo
{	
	WORD							wSeriesDate;						//��������
	bool							bTodayChecked;						//ǩ����ʶ
	SCORE							lRewardGold[LEN_WEEK];				//�������	
};

//ִ��ǩ��
struct CMD_GR_CheckInDone
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ǩ�����
struct CMD_GR_CheckInResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lScore;								//��ǰ���
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//////////////////////////////////////////////////////////////////////////
//��Ա����
#define MDM_GR_MEMBER					13
#define SUB_GR_MEMBER_PARAMETER			340								//��Ա����
#define SUB_GR_MEMBER_QUERY_INFO		341								//��Ա��ѯ
#define SUB_GR_MEMBER_DAY_PRESENT		342								//��Ա�ͽ�
#define SUB_GR_MEMBER_DAY_GIFT			343								//��Ա���
#define SUB_GR_MEMBER_PARAMETER_RESULT	350								//�������
#define SUB_GR_MEMBER_QUERY_INFO_RESULT	351								//��ѯ���
#define SUB_GR_MEMBER_DAY_PRESENT_RESULT	352							//�ͽ���
#define SUB_GR_MEMBER_DAY_GIFT_RESULT	353								//������

//��Ա��ѯ
struct CMD_GR_MemberQueryInfo
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ���
struct CMD_GR_MemberQueryInfoResult
{
	bool							bPresent;							//�ͽ���
	bool							bGift;								//������
	DWORD							GiftSubCount;						//��������
	tagGiftPropertyInfo				GiftSub[50];						//��������
};

//��Ա�ͽ�
struct CMD_GR_MemberDayPresent
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�ͽ���
struct CMD_GR_MemberDayPresentResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lGameScore;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//��Ա���
struct CMD_GR_MemberDayGift
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������
struct CMD_GR_MemberDayGiftResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	TCHAR							szNotifyContent[128];				//��ʾ����
};


//////////////////////////////////////////////////////////////////////////////////
//�������

#define MDM_GF_FRAME				100									//�������

//////////////////////////////////////////////////////////////////////////////////
//�������

//�û�����
#define SUB_GF_GAME_OPTION			1									//��Ϸ����
#define SUB_GF_USER_READY			2									//�û�׼��
#define SUB_GF_LOOKON_CONFIG		3									//�Թ�����
#define SUB_GF_ANDEOID_QUIT			4									//�������˳�
//��������
#define SUB_GF_USER_CHAT			10									//�û�����
#define SUB_GF_USER_EXPRESSION		11									//�û�����
#define SUB_GF_USER_VOICE			12									//�û�����

//��Ϸ��Ϣ
#define SUB_GF_GAME_STATUS			100									//��Ϸ״̬
#define SUB_GF_GAME_SCENE			101									//��Ϸ����
#define SUB_GF_LOOKON_STATUS		102									//�Թ�״̬
#define SUB_GF_USER_DATA			103									//�������
//ϵͳ��Ϣ
#define SUB_GF_SYSTEM_MESSAGE		200									//ϵͳ��Ϣ
#define SUB_GF_ACTION_MESSAGE		201									//������Ϣ
#define SUB_GF_PERSONAL_MESSAGE		202									//˽�˷���Ϣ


//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
struct CMD_GF_GameOption
{
	BYTE							cbAllowLookon;						//�Թ۱�־
	DWORD							dwFrameVersion;						//��ܰ汾
	DWORD							dwClientVersion;					//��Ϸ�汾
};

//�Թ�����
struct CMD_GF_LookonConfig
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbAllowLookon;						//�����Թ�
};

//�Թ�״̬
struct CMD_GF_LookonStatus
{
	BYTE							cbAllowLookon;						//�����Թ�
};

//��Ϸ����
struct CMD_GF_GameStatus
{
	BYTE							cbGameStatus;						//��Ϸ״̬
	BYTE							cbAllowLookon;						//�Թ۱�־
};
//��Ϸ����
struct CMD_GF_GameUserData
{
	DWORD							cbUserCharID;						//��Ϸ״̬
};
//�û�����
struct CMD_GF_C_UserChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�����
struct CMD_GF_S_UserChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�����
struct CMD_GF_C_UserExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û�����
struct CMD_GF_S_UserExpression
{
	WORD							wItemIndex;							//��������
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û�����
struct CMD_GF_C_UserVoice
{
	DWORD							dwTargetUserID;						//Ŀ���û�
	DWORD							dwVoiceLength;						//��������
	BYTE							byVoiceData[MAXT_VOICE_LENGTH];		//��������
};

//�û�����
struct CMD_GF_S_UserVoice
{
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	DWORD							dwVoiceLength;						//��������
	BYTE							byVoiceData[MAXT_VOICE_LENGTH];		//��������
};

//////////////////////////////////////////////////////////////////////////////////
//��Ϸ����

#define MDM_GF_GAME					200									//��Ϸ����

//////////////////////////////////////////////////////////////////////////////////
//Я����Ϣ

//������Ϣ
#define DTP_GR_TABLE_PASSWORD		1									//��������

//�û�����
#define DTP_GR_NICK_NAME			10									//�û��ǳ�
#define DTP_GR_GROUP_NAME			11									//��������
#define DTP_GR_UNDER_WRITE			12									//����ǩ��
#define DTP_GR_QQ					13									//QQ����
#define DTP_GR_MOBILE_PHONE			14									//�ֻ�����

//������Ϣ
#define DTP_GR_USER_NOTE			20									//�û���ע
#define DTP_GR_CUSTOM_FACE			21									//�Զ�ͷ��

//////////////////////////////////////////////////////////////////////////////////

//�������
#define REQUEST_FAILURE_NORMAL		0									//����ԭ��
#define REQUEST_FAILURE_NOGOLD		1									//��Ҳ���
#define REQUEST_FAILURE_NOSCORE		2									//���ֲ���
#define REQUEST_FAILURE_PASSWORD	3									//�������

//////////////////////////////////////////////////////////////////////////////////
//+++++++++++++++++++++++++++Լս����
//˽�˷���
#define MDM_GR_PERSONAL_TABLE		210									//˽�˷���

#define SUB_GR_CREATE_TABLE			1									//��������
#define SUB_GR_CREATE_SUCCESS		2									//�����ɹ�
#define SUB_GR_CREATE_FAILURE		3									//����ʧ��
#define SUB_GR_CANCEL_TABLE			4									//��ɢ����
#define SUB_GR_CANCEL_REQUEST		5									//�����ɢ
#define SUB_GR_REQUEST_REPLY		6									//�����
#define SUB_GR_REQUEST_RESULT		7									//������
#define SUB_GR_WAIT_OVER_TIME		8									//��ʱ�ȴ�
#define SUB_GR_PERSONAL_TABLE_TIP	9									//��ʾ��Ϣ
#define SUB_GR_PERSONAL_TABLE_END	10							//������Ϣ
#define SUB_GR_HOSTL_DISSUME_TABLE	11							//����ǿ�ƽ�ɢ����
//#define SUB_GR_HOSTL_DISSUME_TABLE	12							//����ǿ�ƽ�ɢ����
#define SUB_GR_HOST_DISSUME_TABLE_RESULT	13							//��ɢ����
//#define SUB_GR_USER_QUERY_ROOM_SCORE	14							//˽�˷��䵥��������󷿼�ɼ�
//#define SUB_GR_USER_QUERY_ROOM_SCORE_RESULT	15							//˽�˷��䵥��������󷿼�ɼ����
#define SUB_GR_CURRECE_ROOMCARD_AND_BEAN	16							//��ɢ����
//��������
struct CMD_GR_CreateTable
{
	LONGLONG						lCellScore;							    //�׷�����
	DWORD							dwDrawCountLimit;					//��������
	DWORD							dwDrawTimeLimit;					//ʱ������
	WORD								wJoinGamePeopleCount;			//������Ϸ������
	DWORD							dwRoomTax;								//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
	TCHAR								szPassword[LEN_PASSWORD];	//��������
	BYTE									cbGameRule[RULE_LEN];				//��Ϸ���� �� 0 λ��ʶ �Ƿ����ù��� 0 ����δ���� 1 ��������
	//����λ��Ϸ�Զ������ ����Э������Ϸ����˺Ϳͻ����̶�

};

//�����ɹ�
struct CMD_GR_CreateSuccess
{
	TCHAR								szServerID[7];						//������
	DWORD							dwDrawCountLimit;				//��������
	DWORD							dwDrawTimeLimit;				//ʱ������
	DOUBLE							dBeans;								//��Ϸ��
	LONGLONG						lRoomCard;							//��������
};

//����ʧ��
struct CMD_GR_CreateFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//ȡ������
struct CMD_GR_CancelTable
{
	DWORD							dwReason;							//ȡ��ԭ��
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��ɢ����
struct CMD_GR_DissumeTable
{
	BYTE							cbIsDissumSuccess;					//�Ƿ��ɢ�ɹ�
	TCHAR						szRoomID[ROOM_ID_LEN];				//���� I D			
	SYSTEMTIME				sysDissumeTime;						//��ɢʱ��	
	tagPersonalUserScoreInfo PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];
};

//�����ɢ
struct CMD_GR_CancelRequest
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwTableID;							//���� I D
	DWORD							dwChairID;							//���� I D
};

//����ǿ�ƽ�ɢ��Ϸ
struct CMD_GR_HostDissumeGame
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwTableID;							//���� I D
};




//�����
struct CMD_GR_RequestReply
{
	DWORD							dwUserID;							//�û�I D
	DWORD							dwTableID;							//���� I D
	BYTE							cbAgree;							//�û���
};

//������
struct CMD_GR_RequestResult
{
	DWORD							dwTableID;							//���� I D
	BYTE							cbResult;							//������
};

//��ʱ�ȴ�
struct CMD_GR_WaitOverTime
{
	DWORD							dwUserID;							//�û� I D
};

//��ʾ��Ϣ
struct CMD_GR_PersonalTableTip
{
	DWORD							dwTableOwnerUserID;					//���� I D
	DWORD							dwDrawCountLimit;					//��������
	DWORD							dwDrawTimeLimit;					//ʱ������
	DWORD							dwPlayCount;						//�������
	DWORD							dwPlayTime;							//����ʱ��
	LONGLONG						lCellScore;							//��Ϸ�׷�
	LONGLONG						lIniScore;								//��ʼ����
	TCHAR							szServerID[7];						//������
	byte									cbIsJoinGame;					//�Ƿ������Ϸ
	byte									cbIsGoldOrGameScore;	//��ҳ����ǻ��ֳ� 0 ��ʶ ��ҳ� 1 ��ʶ ���ֳ� 
};

//������Ϣ
struct CMD_GR_PersonalTableEnd
{
	TCHAR							szDescribeString[128];
	LONGLONG					lScore[MAX_CHAIR];
	int								nSpecialInfoLen;			//������Ϣ����
	byte								cbSpecialInfo[SPECIAL_INFO_LEN];//������Ϣ����
};


//˽�˷�����Ϣ
struct Personal_Room_Message
{
	TCHAR szMessage[260];
	BYTE  cbMessageType;
};

//ǿ�ƽ�ɢ���Ӻ����Ϸ���ͷ���
struct CMD_GR_CurrenceRoomCardAndBeans
{
	DOUBLE					dbBeans;							//�����ʶ
	SCORE					lRoomCard;								//���ID
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif