#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//�������ݰ�

//��¼����
#define	DBR_GP_LOGON_GAMEID			1									//I D ��¼
#define	DBR_GP_LOGON_ACCOUNTS		2									//�ʺŵ�¼
#define DBR_GP_REGISTER_ACCOUNTS	3									//ע���ʺ�
#define DBR_GP_VERIFY_INDIVIDUAL	4									//��֤����
#define DBR_GP_LOGON_VISITOR		5									//�ο͵�½

//�ʺŷ���
#define DBR_GP_MODIFY_MACHINE		10									//�޸Ļ���
#define DBR_GP_MODIFY_LOGON_PASS	11									//�޸�����
#define DBR_GP_MODIFY_INSURE_PASS	12									//�޸�����
#define DBR_GP_MODIFY_UNDER_WRITE	13									//�޸�ǩ��
#define DBR_GP_MODIFY_INDIVIDUAL	14									//�޸�����
#define DBR_GP_MODIFY_REAL_AUTH		15									//ʵ����֤

//ͷ������
#define DBR_GP_MODIFY_SYSTEM_FACE	20									//�޸�ͷ��
#define DBR_GP_MODIFY_CUSTOM_FACE	21									//�޸�ͷ��

//��������
#define DBR_GP_USER_ENABLE_INSURE	30									//��ͨ����
#define DBR_GP_USER_SAVE_SCORE		31									//������Ϸ��
#define DBR_GP_USER_TAKE_SCORE		32									//��ȡ��Ϸ��
#define DBR_GP_USER_TRANSFER_SCORE	33									//ת����Ϸ��
#define DBR_GP_QUERY_BANK_INFO	34										//��ѯ�û���������Ϣ
#define DBR_GP_BIND_BANK_INFO	35										//�����п���Ϣ
#define DBR_GP_EXCHANGE_BANK_SCORE	36										//�һ����н��

//��ѯ����
#define DBR_GP_QUERY_INDIVIDUAL		40									//��ѯ����
#define DBR_GP_QUERY_INSURE_INFO	41									//��ѯ����
#define DBR_GP_QUERY_USER_INFO	    42									//��ѯ�û�
#define DBR_GP_QUERY_TRANSFER_REBATE	43								//��ѯ����

//ϵͳ����
#define DBR_GP_ONLINE_COUNT_INFO	60									//������Ϣ
#define DBR_GP_LOAD_GAME_LIST		61									//�����б�
#define DBR_GP_LOAD_CHECKIN_REWARD	62									//ǩ������
#define DBR_GP_LOAD_TASK_LIST		63									//��������
#define DBR_GP_LOAD_BASEENSURE		64									//���صͱ�
#define DBR_GP_LOAD_MEMBER_PARAMETER 66									//��Ա����
#define DBR_GP_LOAD_PLATFORM_PARAMETER 67								//ƽ̨����
#define DBR_GP_LOAD_GROWLEVEL_CONFIG 68									//�ȼ�����
#define DBR_GP_LOAD_GAME_PROPERTY_LIST	69								//���ص���


//��������
#define DBR_GP_GET_PARAMETER		70									//��ȡ����
#define DBR_GP_ADD_PARAMETER		71									//��Ӳ���
#define DBR_GP_MODIFY_PARAMETER		72									//�޸Ĳ���
#define DBR_GP_DELETE_PARAMETER		73									//ɾ������

//ǩ������
#define DBR_GP_CHECKIN_DONE			80									//ִ��ǩ��
#define DBR_GP_CHECKIN_QUERY_INFO	81									//��ѯ��Ϣ

//��������
#define DBR_GP_TASK_TAKE			90									//��ȡ����
#define DBR_GP_TASK_REWARD			91									//������
#define DBR_GP_TASK_QUERY_INFO		92									//��������	
#define DBR_GP_TASK_GIVEUP			93									//��������

//�ͱ�����
#define DBR_GP_BASEENSURE_TAKE		100									//��ȡ�ͱ�

//�ȼ�����
#define DBR_GP_GROWLEVEL_QUERY_IFNO	 110								//��ѯ�ȼ�

//�һ�����
#define DBR_GP_PURCHASE_MEMBER		120									//�����Ա
#define DBR_GP_EXCHANGE_SCORE_INGOT 121									//�һ���Ϸ��
#define DBR_GP_EXCHANGE_SCORE_BEANS 122									//�һ���Ϸ��

//�ƹ�����
#define DBR_GP_QUERY_SPREAD_INFO	130									//�ƹ���Ϣ

//�齱����
#define DBR_GP_LOTTERY_CONFIG_REQ	140									//��������
#define DBR_GP_LOTTERY_START		141									//��ʼ�齱

//��Ϸ����
#define DBR_GP_QUERY_USER_GAME_DATA	150									//��Ϸ����

//˽�˷���
#define DBR_MB_CREATE_PERSONAL_TABLE	160								//��������

//��֤����
#define DBR_GP_LOAD_REAL_AUTH		161									//��֤��Ϣ
#define DBR_MB_QUERY_PERSONAL_ROOM_INFO	162						//����˽�˷�����Ϣ
#define DBR_GR_QUERY_USER_ROOM_SCORE	163							//������󷿼�ɼ�
#define DBR_GR_CLOSE_ROOM_WRITE_DISSUME_TIME	 164				//�ر���Ϸ������д˽�˷��Ľ���ʱ��
#define DBR_MB_GET_PERSONAL_PARAMETER	165							//��������
#define DBR_MB_QUERY_PERSONAL_ROOM_USER_INFO	 166			//˽�˷����������Ϣ
#define DBR_MB_ROOM_CARD_EXCHANGE_TO_SCORE	 167				//�����һ���Ϸ��
#define DBR_GP_SERVER_DUMMY_ONLINE	168							//��ѯ������������

//�ʺŰ�
#define DBR_GP_ACCOUNT_BIND			171									//�ʺŰ�
#define DBR_GP_ACCOUNT_BIND_EXISTS	172									//�ʺŰ�

#define DBR_GP_IP_LOCATION			173									//IP������

//��Ա����
#define DBR_GP_MEMBER_QUERY_INFO	340									//��Ա��ѯ
#define DBR_GP_MEMBER_DAY_PRESENT	341									//��Ա�ͽ�
#define DBR_GP_MEMBER_DAY_GIFT		342									//��Ա���

//��������
#define DBR_GP_PROPERTY_BUY			440									//�������
#define DBR_GP_PROPERTY_USE			441									//ʹ�õ���
#define DBR_GP_QUERY_BACKPACK		442									//��ѯ����
#define DBR_GP_USER_PROPERTY_BUFF	443									//����Buff
#define DBR_GP_PROPERTY_PRESENT		444									//��������
#define DBR_GP_QUERY_SEND_PRESENT	445									//��ѯ����
#define DBR_GP_GET_SEND_PRESENT		446									//��ȡ����
#define DBR_GP_QUERY_SINGLE			447									//��ѯ����
//////////////////////////////////////////////////////////////////////////////////

//ID ��¼
struct DBR_GP_LogonGameID
{
	//��¼��Ϣ
	DWORD							dwGameID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	BYTE							cbNeeValidateMBCard;				//�ܱ�У��

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//�ʺŵ�¼
struct DBR_GP_LogonAccounts
{
	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//���֤��
	BYTE							cbNeeValidateMBCard;				//�ܱ�У��

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���

	//��չ��Ϣ
	DWORD							dwCheckUserRight;					//���Ȩ��
};

//�ο͵�½
struct DBR_GP_LogonVisitor
{
	//��¼��Ϣ
	BYTE							cbPlatformID;						//ƽ̨���

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//�ʺŰ�
struct DBR_GP_AccountBind
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	TCHAR							szBindNewAccounts[LEN_ACCOUNTS];	//���ʺ�
	TCHAR							szBindNewPassword[LEN_PASSWORD];	//������
	TCHAR							szBindNewSpreader[LEN_ACCOUNTS];	//���Ƽ�
};

//ip������
struct DBR_GP_IpLocation
{
	DWORD							dwUserID;
	TCHAR							szLocation[LEN_IP];	//ip������
};

//�ʺŰ�
struct DBR_GP_AccountBind_Exists
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	TCHAR							szBindExistsAccounts[LEN_ACCOUNTS];	//���ʺ�
	TCHAR							szBindExistsPassword[LEN_PASSWORD];	//������
};

//�ʺ�ע��
struct DBR_GP_RegisterAccounts
{
	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//֤������
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szAgentID[LEN_ACCOUNTS];			//�����ʶ
	DWORD							dwSpreaderGameID;					//�Ƽ���ʶ

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//��֤����
struct DBR_GP_VerifyIndividual
{
	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���

	WORD							wVerifyMask;						//У����֤
	TCHAR							szVerifyContent[LEN_ACCOUNTS];		//��֤����
};

//�޸Ļ���
struct DBR_GP_ModifyMachine
{
	BYTE							cbBind;								//�󶨱�־
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸�����
struct DBR_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�����
struct DBR_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�ǩ��
struct DBR_GP_ModifyUnderWrite
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
};

//ʵ����֤
struct DBR_GP_RealAuth
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//֤������
};

//�޸�ͷ��
struct DBR_GP_ModifySystemFace
{
	//�û���Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸�ͷ��
struct DBR_GP_ModifyCustomFace
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//ͼƬ��Ϣ

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸�����
struct DBR_GP_ModifyIndividual
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//�ʺ�����
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��

	//�û���Ϣ
	TCHAR							szUserNote[LEN_USER_NOTE];			//�û�˵��
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//֤������
	TCHAR							szSpreader[LEN_ACCOUNTS];			//�Ƽ��ʺ�

	//�绰����
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//�̶��绰
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ƶ��绰

	//��ϵ����
	TCHAR							szQQ[LEN_QQ];						//Q Q ����
	TCHAR							szEMail[LEN_EMAIL];					//�����ʼ�
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//��ϵ��ַ
};

//��ѯ����
struct DBR_GP_QueryIndividual
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//��ͨ����
struct DBR_GP_UserEnableInsure
{
	DWORD							dwUserID;							//�û� I D	
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������Ϸ��
struct DBR_GP_UserSaveScore
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lSaveScore;							//������Ϸ��
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ�û���������Ϣ
struct DBR_GP_QueryBankInfo
{
	DWORD							dwUserID;							//�û� I D
};

//������
struct DBR_GP_BindBankInfo
{											
	DWORD dwUserID;							//�û�id
	TCHAR szRealName[LEN_REAL_NAME];		//��ʵ����
	TCHAR szBankNo[LEN_BANK_NO];			//���п���
	TCHAR szBankName[LEN_BANK_NAME];		//��������
	TCHAR szBankAddress[LEN_BANK_ADDRESS];	//��������
	TCHAR szAlipayID[LEN_BANK_ADDRESS];		//֧����
};

//������
struct DBR_GP_ExchangeBankScore
{											
	DWORD dwUserID;							//�û�id
	SCORE dwScore;		//��ʵ����
	TCHAR szBankPass[LEN_PASSWORD];			//���п���
	TCHAR szOrderID[LEN_ORDER_ID];		//��������
	DWORD dwClientAddr;						//���ӵ�ַ
	DWORD dwType;							//0 bank 1 alipay
};

//ȡ����Ϸ��
struct DBR_GP_UserTakeScore
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lTakeScore;							//��ȡ��Ϸ��
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ת����Ϸ��
struct DBR_GP_UserTransferScore
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	SCORE							lTransferScore;						//ת����Ϸ��
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szPassword[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szTransRemark[LEN_TRANS_REMARK];	//ת�ʱ�ע
};

//��ѯ����
struct DBR_GP_QueryInsureInfo
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//��ѯ�û�
struct DBR_GP_QueryInsureUserInfo
{
	BYTE                            cbByNickName;                       //�ǳ�����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//Ŀ���û�
};

//�û�����
struct DBO_GP_UserTransferUserInfo
{
	DWORD							dwGameID;							//�û� I D
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��ʺ�
};

//��ѯ����
struct DBR_GP_QueryTransferRebate
{
	DWORD							dwUserID;							//�û�I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//������Ϣ
struct DBR_GP_OnLineCountInfo
{
	WORD							wKindCount;							//������Ŀ
	DWORD							dwOnLineCountSum;					//��������
	DWORD							dwAndroidCountSum;					//��������
	tagOnLineInfoKindEx				OnLineCountKind[MAX_KIND];			//��������
};


//��������
struct DBR_GP_MatchSignup
{
	//������Ϣ
	WORD							wServerID;							//�����ʶ
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��������
struct DBR_GP_MatchUnSignup
{
	//������Ϣ
	WORD							wServerID;							//�����ʶ
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};


//��ȡ����
struct DBR_GP_GetParameter
{
	WORD							wServerID;							//�����ʶ
};

//��Ӳ���
struct DBR_GP_AddParameter
{
	WORD							wServerID;							//�����ʶ
	tagAndroidParameter				AndroidParameter;					//��������
};

//�޸Ĳ���
struct DBR_GP_ModifyParameter
{
	WORD							wServerID;							//�����ʶ
	tagAndroidParameter				AndroidParameter;					//��������
};

//ɾ������
struct DBR_GP_DeleteParameter
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwBatchID;							//���α�ʶ
};

//��ѯǩ��
struct DBR_GP_CheckInQueryInfo
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//ִ��ǩ��
struct DBR_GP_CheckInDone
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ����
struct DBR_GP_TaskQueryInfo
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//��������
struct DBR_GP_TaskGiveUP
{
	//�û���Ϣ
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ����
struct DBR_GP_TaskTake
{
	//�û���Ϣ
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������
struct DBR_GP_TaskReward
{
	//�û���Ϣ
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ�ͱ�
struct DBR_GP_TakeBaseEnsure
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ�ȼ�
struct DBR_GP_GrowLevelQueryInfo
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��Ա��ѯ
struct DBR_GP_MemberQueryInfo
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��Ա����
struct DBR_GP_MemberDayPresent
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��Ա���
struct DBR_GP_MemberDayGift
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//���߹���
struct DBR_GP_PropertyBuy
{
	DWORD							dwUserID;							//�������
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwItemCount;						//������Ŀ
	BYTE							cbConsumeType;						//��������
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//����ʹ��
struct DBR_GP_PropertyUse
{
	//ʹ����Ϣ
	DWORD							dwUserID;							//ʹ����
	DWORD							dwRecvUserID;						//���߶�˭ʹ��
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//��ѯ����
struct DBR_GP_PropertyQuerySingle
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwPropertyID;						//���߱�ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//�����Ա
struct DBR_GP_PurchaseMember
{
	DWORD							dwUserID;							//�û� I D
	BYTE							cbMemberOrder;						//��Ա��ʶ
	WORD							wPurchaseTime;						//����ʱ��

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�һ���Ϸ��
struct DBR_GP_ExchangeScoreByIngot
{
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lExchangeIngot;						//�һ�����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�һ���Ϸ��
struct DBR_GP_ExchangeScoreByBeans
{
	DWORD							dwUserID;							//�û���ʶ
	double							dExchangeBeans;						//�һ�����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�ƹ���Ϣ
struct DBR_GP_QuerySpreadInfo
{
	DWORD							dwUserID;							//�û���ʶ
};

//////////////////////////////////////////////////////////////////////////////////
//�齱����

//��ʼ�齱
struct DBR_GP_LotteryConfigReq
{
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//��ʼ�齱
struct DBR_GP_LotteryStart
{
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//////////////////////////////////////////////////////////////////////////////////
//��Ϸ����
struct DBR_GP_QueryUserGameData
{
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szDynamicPass[LEN_MD5];				//�û�����	
};

//��ѯ����
struct DBR_GP_QueryBackpack
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwKindID;							//��������
	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//����Buff
struct DBR_GP_UserPropertyBuff
{
	DWORD							dwUserID;							//�û���ʶ
};

//��������
struct DBR_GP_PropertyPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwRecvGameID;						//���߸�˭(GameID)
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
	WORD							wType;								//0 ��������ǳ�  1 �������GameID
	DWORD							dwClientAddr;						//�ͻ��˵�ַ
	TCHAR							szRecvNickName[16];					//���߶�˭ʹ�ã��ǳƣ�
};

//��ѯ����
struct DBR_GP_QuerySendPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwClientAddr;						//�ͻ��˵�ַ
};

//��ȡ����
struct DBR_GP_GetSendPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwClientAddr;						//�ͻ��˵�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};


//˽�˷�
//////////////////////////////////////////////////////////////////////////////////
//��������
struct DBR_MB_CreatePersonalTable 
{
	DWORD							dwUserID;							//�û���ʶ

	//������Ϣ
	DWORD							dwServerID;							//����I D
	DWORD							dwTableID;							//����I D

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//˽�˷�������
struct DBR_MB_GetPersonalParameter
{
	DWORD							dwServerID;							//��ϷI D
};



//////////////////////////////////////////////////////////////////////////////////
//�����Ϣ

//��¼���
#define DBO_GP_LOGON_SUCCESS		100									//��¼�ɹ�
#define DBO_GP_LOGON_FAILURE		101									//��¼ʧ��
#define DBO_GP_VALIDATE_MBCARD		102									//��¼ʧ��
#define DBO_GP_VALIDATE_PASSPORT	103									//��¼ʧ��
#define DBO_GP_VERIFY_RESULT		104									//��֤���									

//�������
#define DBO_GP_USER_FACE_INFO		110									//�û�ͷ��
#define DBO_GP_USER_INDIVIDUAL		111									//�û�����

//��������
#define DBO_GP_USER_INSURE_INFO		120									//��������
#define DBO_GP_USER_INSURE_SUCCESS	121									//���гɹ�
#define DBO_GP_USER_INSURE_FAILURE	122									//����ʧ��
#define DBO_GP_USER_INSURE_USER_INFO 123								//�û�����
#define DBO_GP_USER_INSURE_ENABLE_RESULT 124							//��ͨ���
#define DBO_GP_QUERY_TRANSFER_REBATE_RESULT	125							//��ѯ���
#define DBO_GP_QUERY_BANKINFO_SUCCESS 126								//��ѯ�����п���Ϣ

//�б���
#define DBO_GP_GAME_TYPE_ITEM		130									//������Ϣ
#define DBO_GP_GAME_KIND_ITEM		131									//������Ϣ
#define DBO_GP_GAME_NODE_ITEM		132									//�ڵ���Ϣ
#define DBO_GP_GAME_PAGE_ITEM		133									//������Ϣ
#define DBO_GP_GAME_LIST_RESULT		134									//���ؽ��

//ϵͳ���
#define DBO_GP_PLATFORM_PARAMETER	140									//ƽ̨����
#define DBO_GP_SERVER_DUMMY_ONLINE	141									//������������
#define DBO_GP_SERVER_DUMMY_ONLINE_BEGIN	142							//������������
#define DBO_GP_SERVER_DUMMY_ONLINE_END	143								//������������
//��������
#define DBO_GP_ANDROID_PARAMETER	150									//��������	

//ǩ������
#define DBO_GP_CHECKIN_REWARD		160									//ǩ������
#define DBO_GP_CHECKIN_INFO			161									//ǩ����Ϣ
#define DBO_GP_CHECKIN_RESULT		162									//ǩ�����

//��������
#define DBO_GP_TASK_LIST			170									//�����б�
#define DBO_GP_TASK_LIST_END		171									//�б����
#define DBO_GP_TASK_INFO			172									//������Ϣ
#define DBO_GP_TASK_RESULT			173									//������

//�ͱ�����
#define DBO_GP_BASEENSURE_PARAMETER	180									//�ͱ�����
#define DBO_GP_BASEENSURE_RESULT	181									//��ȡ���

//�ƹ�����
#define DBO_GP_SPREAD_INFO			190									//�ƹ����

//��֤����
#define DBO_GP_REAL_AUTH_PARAMETER			191									//��֤����

//�ȼ�����
#define DBO_GP_GROWLEVEL_CONFIG		200									//�ȼ�����
#define DBO_GP_GROWLEVEL_PARAMETER	201									//�ȼ�����
#define DBO_GP_GROWLEVEL_UPGRADE	202									//�ȼ�����

//�һ�����
#define DBO_GP_PURCHASE_RESULT		211									//������
#define DBO_GP_EXCHANGE_RESULT		212									//�һ����
#define DBO_GP_ROOM_CARD_EXCHANGE_RESULT		213			//�����һ����



//�齱���
#define DBO_GP_LOTTERY_CONFIG		230									//�齱����
#define DBO_GP_LOTTERY_USER_INFO	231									//�齱��Ϣ
#define DBO_GP_LOTTERY_RESULT		232									//�齱���

//�齱���
#define DBO_GP_QUERY_USER_GAME_DATA	250									//��Ϸ����

//���Ͻ��
#define DBO_GP_INDIVIDUAL_RESULT	260									//���Ͻ��	
#define DBO_GP_QUERY_USERDATA_RESULT 261								//��ѯUSERData���

//��������
#define DBO_GP_GAME_PROPERTY_TYPE_ITEM		290									//���ͽڵ�
#define DBO_GP_GAME_PROPERTY_RELAT_ITEM		291									//���߹�ϵ
#define DBO_GP_GAME_PROPERTY_ITEM			292									//���߽ڵ�
#define DBO_GP_GAME_PROPERTY_SUB_ITEM		293									//���߽ڵ�
#define DBO_GP_GAME_PROPERTY_LIST_RESULT	294									//���߽��

#define DBO_GP_GAME_PROPERTY_BUY	300									//���߹���
#define DBO_GP_QUERY_BACKPACK		301									//��ѯ����
#define DBO_GP_GAME_PROPERTY_USE	302									//����ʹ��
#define DBO_GP_USER_PROPERTY_BUFF	303									//����Buff
#define DBO_GP_PROPERTY_PRESENT		304									//��������
#define DBO_GP_QUERY_SEND_PRESENT	305									//��ѯ����
#define DBO_GP_GET_SEND_PRESENT		306									//��ȡ����
#define DBO_GP_PROPERTY_QUERY_SINGLE	307								//���߹���
#define DBO_GP_GAME_PROPERTY_FAILURE 310									//����ʧ��

//��Ա���
#define DBO_GP_MEMBER_PARAMETER						340					//��Ա����
#define DBO_GP_MEMBER_QUERY_INFO_RESULT				341					//��ѯ���
#define DBO_GP_MEMBER_DAY_PRESENT_RESULT			342					//�ͽ���
#define DBO_GP_MEMBER_DAY_GIFT_RESULT				343					//������

//�����б�
#define DBO_GP_AGENT_GAME_KIND_ITEM		351								//������Ϣ

//������
#define DBO_GP_OPERATE_SUCCESS		800									//�����ɹ�
#define DBO_GP_OPERATE_FAILURE		801									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////////////

//��¼�ɹ�
struct DBO_GP_LogonSuccess
{
	//��������
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	DWORD							dwGroupID;							//���ű�ʶ
	DWORD							dwCustomID;							//�Զ�����
	DWORD							dwExperience;						//������ֵ
	SCORE							lLoveLiness;						//�û�����
	TCHAR							szPassword[LEN_MD5];				//��¼����	
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szDynamicPass[LEN_PASSWORD];		//��̬����
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������

	//�û��ɼ�
	SCORE							lUserScore;							//�û���Ϸ��
	SCORE							lUserIngot;							//�û�Ԫ��
	SCORE							lUserInsure;						//�û�����	
	DOUBLE							dUserBeans;							//�û���Ϸ��

	//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMoorMachine;						//��������
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��

	//��Ա����
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	SYSTEMTIME						MemberOverDate;						//����ʱ��

	//��չ��Ϣ
	BYTE							cbInsureEnabled;					//ʹ�ܱ�ʶ
	DWORD							dwCheckUserRight;					//���Ȩ��
	BYTE							cbIsAgent;							//�����ʶ	

	//������Ϣ
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��¼ʧ��
struct DBO_GP_LogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��¼ʧ��
struct DBO_GP_ValidateMBCard
{
	UINT							uMBCardID;						//��������
};

//��֤���
struct DBO_GP_VerifyIndividualResult
{
	bool							bVerifyPassage;						//��֤ͨ��
	WORD							wVerifyMask;						//��֤����
	TCHAR							szErrorMsg[128];					//������Ϣ
};

//ͷ����Ϣ
struct DBO_GP_UserFaceInfo
{
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwCustomID;							//�Զ�����
};

//��������
struct DBO_GP_UserIndividual
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szUserNote[LEN_USER_NOTE];			//�û�˵��
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//֤������

	//�绰����
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//�̶��绰
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ƶ��绰

	//��ϵ����
	TCHAR							szQQ[LEN_QQ];						//Q Q ����
	TCHAR							szEMail[LEN_EMAIL];					//�����ʼ�
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//��ϵ��ַ

	//�ƹ���Ϣ
	TCHAR							szSpreader[LEN_ACCOUNTS];			//�ƹ���Ϣ
};

//��������
struct DBO_GP_UserInsureInfo
{
	BYTE							cbEnjoinTransfer;					//ת�ʿ���
	WORD							wRevenueTake;						//˰�ձ���
	WORD							wRevenueTransfer;					//˰�ձ���
	WORD							wRevenueTransferMember;				//˰�ձ���
	WORD							wServerID;							//�����ʶ
	SCORE							lUserScore;							//�û���Ϸ��
	SCORE							lUserInsure;						//������Ϸ��
	SCORE							lTransferPrerequisite;				//ת������
};

//���гɹ�
struct DBO_GP_UserInsureSuccess
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lSourceScore;						//ԭ����Ϸ��
	SCORE							lSourceInsure;						//ԭ����Ϸ��
	SCORE							lInsureRevenue;						//����˰��
	SCORE							lVariationScore;					//��Ϸ�ұ仯
	SCORE							lVariationInsure;					//���б仯
	TCHAR							szDescribeString[128];				//������Ϣ
};

//����ʧ��
struct  DBO_GP_UserInsureFailure
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��ͨ���
struct DBO_GP_UserInsureEnableResult
{
	BYTE							cbInsureEnabled;					//ʹ�ܱ�ʶ
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��ѯ�û������н��
struct DBO_GP_QueryBankInfoResult
{
	TCHAR							szRealName[LEN_REAL_NAME];			//��ʵ����
	TCHAR							szBankNo[LEN_BANK_NO];				//���п���
	SCORE							lDrawScore;							//�����ֽ��
	TCHAR							szAlipayID[LEN_BANK_ADDRESS];		//֧����
};

//��������
struct DBO_GP_PlatformParameter
{
	DWORD							dwExchangeRate;						//�һ�����
	DWORD							dwPresentExchangeRate;				//������Ϸ�Ҷһ���
	DWORD							dwRateGold;							//��Ϸ����Ϸ�Ҷһ���
};

//��������
struct DBO_GP_AndroidParameter
{
	WORD							wSubCommdID;						//��������
	WORD							wServerID;							//�����ʶ
	WORD							wParameterCount;					//������Ŀ
	tagAndroidParameter				AndroidParameter[MAX_BATCH];		//��������
};

//���Ͻ��
struct DBO_GP_IndividualResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lScore;								//��ǰ����
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��ѯ���
struct DBO_GP_QueryTransferRebateResult
{
	DWORD							dwUserID;							//�û�I D
	BYTE							cbRebateEnabled;					//ʹ�ܱ�ʶ	
	SCORE							lIngot;								//����Ԫ��
	SCORE							lLoveLiness;						//��������ֵ
};

//ǩ������
struct DBO_GP_CheckInReward
{
	SCORE							lRewardGold[LEN_WEEK];				//�������
};

//ǩ����Ϣ
struct DBO_GP_CheckInInfo
{
	WORD							wSeriesDate;						//��������
	bool							bTodayChecked;						//ǩ����ʶ
};

//ǩ�����
struct DBO_GP_CheckInResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lScore;								//��ǰ����
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�����б�
struct DBO_GP_TaskListInfo
{
	WORD							wTaskCount;							//������Ŀ
};

//������Ϣ
struct DBO_GP_TaskInfo
{
	WORD							wTaskCount;							//��������
	tagTaskStatus					TaskStatus[TASK_MAX_COUNT];			//����״̬
};

//ǩ�����
struct DBO_GP_TaskResult
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

//�ͱ�����
struct DBO_GP_BaseEnsureParameter
{
	SCORE							lScoreCondition;					//��Ϸ������
	SCORE							lScoreAmount;						//��Ϸ������
	BYTE							cbTakeTimes;						//��ȡ����	
};

//�ͱ����
struct DBO_GP_BaseEnsureResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lGameScore;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�ƹ���Ϣ
struct DBO_GP_UserSpreadInfo
{
	DWORD							dwSpreadCount;						//�ƹ�����
	SCORE							lSpreadReward;						//�ƹ㽱��
};

//ʵ����֤
struct DBO_GP_RealAuthParameter
{
	DWORD							dwAuthentDisable;					//��֤����
	SCORE							dwAuthRealAward;					//�������
};

//�ȼ�����
struct DBO_GP_GrowLevelConfig
{
	WORD							wLevelCount;						//�ȼ���Ŀ
	tagGrowLevelConfig				GrowLevelConfig[60];				//�ȼ�����
};

//�ȼ�����
struct DBO_GP_GrowLevelParameter
{
	WORD							wCurrLevelID;						//��ǰ�ȼ�	
	DWORD							dwExperience;						//��ǰ����
	DWORD							dwUpgradeExperience;				//�¼�����
	SCORE							lUpgradeRewardGold;					//��������
	SCORE							lUpgradeRewardIngot;				//��������
};

//�ȼ�����
struct DBO_GP_GrowLevelUpgrade
{
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrIngot;							//��ǰԪ��
	TCHAR							szNotifyContent[128];				//������ʾ
};

//��Ա����
struct DBO_GP_MemberParameter
{
	WORD							wMemberCount;						//��Ա��Ŀ
	tagMemberParameterNew			MemberParameter[64];				//��Ա����
};

//��Ա��ѯ
struct DBO_GP_MemberQueryInfoResult
{
	bool							bPresent;							//��Ա�ͽ�
	bool							bGift;								//��Ա���
	DWORD							GiftSubCount;						//��������
	tagGiftPropertyInfo				GiftSub[50];						//��������
};

//��Ա����
struct DBO_GP_MemberDayPresentResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lGameScore;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//��Ա���
struct DBO_GP_MemberDayGiftResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	TCHAR							szNotifyContent[128];				//��ʾ����
};
//////////////////////////////////////////////////////////////////////////////////
//���߽ڵ�
struct DBO_GP_GamePropertyTypeItem
{
	DWORD							dwTypeID;							//���ͱ�ʶ
	DWORD							dwSortID;							//�����ʶ
	TCHAR							szTypeName[LEN_TYPE];				//��������
};

//���߽ڵ�
struct DBO_GP_GamePropertyRelatItem
{
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwTypeID;							//���ͱ�ʶ
};

//���߽ڵ�
struct DBO_GP_GamePropertyItem
{
	//������Ϣ
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwPropertyKind;						//��������

	BYTE							cbUseArea;							//ʹ�÷�Χ
	BYTE							cbServiceArea;						//����Χ
	BYTE							cbRecommend;						//�Ƽ���ʶ

	//���ۼ۸�
	SCORE							lPropertyGold;						//���߽��
	DOUBLE							dPropertyCash;						//���߼۸�
	SCORE							lPropertyUserMedal;					//���߽��
	SCORE							lPropertyLoveLiness;				//���߽��

	//��òƸ�
	SCORE							lSendLoveLiness;					//������������ͷ���
	SCORE							lRecvLoveLiness;					//������������շ���
	SCORE							lUseResultsGold;					//��ý��

	//����Ч��
	DWORD							dwUseResultsValidTime;				//��Чʱ��
	DWORD							dwUseResultsValidTimeScoreMultiple;	//��Ч����

	//������
	DWORD							dwUseResultsGiftPackage;			//������

	DWORD							dwSortID	;						//�����ʶ
	TCHAR							szName[PROPERTY_LEN_NAME];			 //��������
	TCHAR							szRegulationsInfo[PROPERTY_LEN_INFO];//ʹ����Ϣ
};

//�ӵ��߽ڵ�
struct DBO_GP_GamePropertySubItem
{
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwOwnerPropertyID;					//���߱�ʶ
	DWORD                           dwPropertyCount;                    //������Ŀ
	DWORD							dwSortID;							//�����ʶ
};

//���ؽ��
struct DBO_GP_GamePropertyListResult
{
	BYTE							cbSuccess;							//�ɹ���־
};

//������
struct DBO_GP_PropertyBuyResult
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

//����ʧ��
struct DBO_GP_PropertyFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��ѯ���
struct DBO_GP_PropertyQuerySingle
{
	//������Ϣ
	DWORD							dwUserID;							//�û� I D
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwItemCount;						//������Ŀ
};

//������
struct DBO_GP_PurchaseResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	BYTE							cbMemberOrder;						//��Աϵ��
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	DOUBLE							dCurrBeans;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�������
struct DBO_GP_QueryBackpack
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwStatus;							//״̬ 0������ 1���ͽ���
	DWORD							dwCount;							//����
	tagBackpackProperty				PropertyInfo[1];					//������Ϣ
};
 
struct DBO_GP_PropertyUse
{
	DWORD							dwUserID;							//ʹ����
	DWORD							dwRecvUserID;						//���߶�˭ʹ��
	DWORD							dwPropID;							//����ID
	DWORD							wPropCount;							//ʹ����Ŀ
	DWORD							dwScoreMultiple;					//Ч������
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
	TCHAR							szNotifyContent[128];				//��ʾ��Ϣ
};

//��ҵ���Buff
struct DBO_GR_UserPropertyBuff
{
	DWORD							dwUserID;
	BYTE							cbBuffCount;						//Buff��Ŀ
	tagPropertyBuff					PropertyBuff[MAX_BUFF];			
};

//�������
struct DBO_GP_PropertyPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwRecvGameID;						//���߸�˭(GameID)
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
	WORD							wType;								//0 ��������ǳ�  1 �������GameID
	TCHAR							szRecvNickName[16];					//���߶�˭ʹ�ã��ǳƣ�
	int								nHandleCode;						//������
	TCHAR							szNotifyContent[64];				//��ʾ����
};


//��ѯ����
struct DBO_GP_QuerySendPresent
{
	WORD							wPresentCount;						//���ʹ���
	SendPresent						Present[MAX_PROPERTY];											
};

struct DBO_GP_GetSendPresent
{
	WORD							wPresentCount;						//���ʹ���
	SendPresent						Present[MAX_PROPERTY];											
};

//�һ����
struct DBO_GP_ExchangeResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrIngot;							//��ǰԪ��
	DOUBLE							dCurrBeans;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�û����ݽ��
struct DBO_GP_QueryUserDataResult
{
	DWORD							dwUserID;							//�û�ID
};

//�齱����
struct DBO_GP_LotteryConfig
{
	WORD							wLotteryCount;						//�������
	tagLotteryItem					LotteryItem[MAX_LOTTERY];			//��������
};

//�齱��Ϣ
struct DBO_GP_LotteryUserInfo
{
	BYTE							cbFreeCount;						//��Ѵ���
	BYTE							cbAlreadyCount;						//�������
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lChargeFee;							//�齱����
};

//�齱���
struct DBO_GP_LotteryResult
{
	bool							bWined;								//�н���ʶ
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lUserScore;							//�û�����
	DOUBLE							dUserBeans;							//�û���Ϸ��
	tagLotteryItem					LotteryItem;						//�н�����
};

//��Ϸ����
struct DBO_GP_QueryUserGameData
{
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szUserGameData[1024];				//��Ϸ����
};

//����ʧ��
struct DBO_GP_OperateFailure
{
	bool							bCloseSocket;						//�ر�����
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�����ɹ�
struct DBO_GP_OperateSuccess
{
	bool							bCloseSocket;						//�ر�����
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
struct DBO_GP_GameType
{
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wTypeID;							//��������
	TCHAR							szTypeName[LEN_TYPE];				//��������
};

//��Ϸ����
struct DBO_GP_GameKind
{
	WORD							wTypeID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wGameID;							//ģ������
	WORD							wRecommend;							//�Ƽ���Ϸ
	WORD							wGameFlag;							//��Ϸ��־
	DWORD							dwSuportType;						//֧������
	TCHAR							szKindName[LEN_KIND];				//��Ϸ����
	TCHAR							szProcessName[LEN_PROCESS];			//��������
};

//��Ϸ�ڵ�
struct DBO_GP_GameNode
{
	WORD							wKindID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wNodeID;							//�ڵ�����
	TCHAR							szNodeName[LEN_NODE];				//�ڵ�����
};

//��������
struct DBO_GP_GamePage
{
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wPageID;							//��������
	WORD							wOperateType;						//��������
	TCHAR							szDisplayName[LEN_PAGE];			//��ʾ����
};

//���ؽ��
struct DBO_GP_GameListResult
{
	BYTE							cbSuccess;							//�ɹ���־
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//�ֻ����ݰ�

//��¼����
#define	DBR_MB_LOGON_GAMEID			700									//I D ��¼
#define	DBR_MB_LOGON_ACCOUNTS		701									//�ʺŵ�¼
#define DBR_MB_REGISTER_ACCOUNTS	702									//ע���ʺ�
#define	DBR_MB_LOGON_OTHERPLATFORM	703									//������¼
#define	DBR_MB_LOGON_VISITOR		704									//�ο͵�¼
#define DBR_MB_PHONE_REGISTER_ACCOUNTS	705								//�ֻ���ע��
#define DBR_MB_MODIFY_PASS	706								//�޸��ֻ���֤��
#define DBR_MB_QUERY_USERDATA	707								//��ѯ�û�����
#define DBR_MB_TOUSU_AGENT	708								//Ͷ�ߴ���
#define DBR_MB_SHENQING_AGENT_CHECK	709							//�������ǰ���
#define DBR_MB_SHENQING_AGENT	710								//�������
#define DBR_MB_QUERY_GAMELOCKINFO	711								//��Ϸ�����ѯ
#define DBR_MB_MODIFY_BANKINFO	712								//�޸��ֻ���֤��

#define	DBO_MB_AGENT_GAME_KIND_ITEM		1100							//�����ʺ�
//��¼���
#define DBO_MB_LOGON_SUCCESS		900									//��¼�ɹ�
#define DBO_MB_LOGON_FAILURE		901									//��¼ʧ��


#define DBO_MB_PERSONAL_PARAMETER	1000						//˽�˷�������
#define DBO_MB_PERSONAL_ROOM_LIST	1001						//˽�˷����б���Ϣ
#define DBO_MB_PERSONAL_FEE_LIST	    1002						//˽�˷����б���Ϣ���ķ�����Ϣ
#define DBO_GR_QUERY_USER_ROOM_SCORE	    1003			//˽�˷����б���Ϣ���ķ�����Ϣ
#define DBO_GR_QUERY_PERSONAL_ROOM_USER_INFO_RESULT	    1004			//˽�˷����б���Ϣ���ķ�����Ϣ

#define DBO_MB_QUERY_GAMELOCKINFO_RESULT	    1005						//��ѯ������Ϣ���

//////////////////////////////////////////////////////////////////////////////////

//ID ��¼
struct DBR_MB_LogonGameID
{
	//��¼��Ϣ
	DWORD							dwGameID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//�ʺŵ�¼
struct DBR_MB_LogonAccounts
{
	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//�ʺŵ�¼
struct DBR_MB_LogonOtherPlatform
{
	//��¼��Ϣ
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbPlatformID;						//ƽ̨���
	TCHAR							szUserUin[LEN_USER_UIN];			//�û�Uin
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szAgentID[LEN_ACCOUNTS];			//�����ʶ
	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//�ο͵�¼
struct DBR_MB_LogonVisitor
{
	//��¼��Ϣ
	BYTE							cbPlatformID;						//ƽ̨���

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
	TCHAR							szAgentID[LEN_ACCOUNTS];			//�����ʶ

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//Ͷ�ߴ���
struct DBR_MB_TouSuDaili
{
	//�û�ID
	DWORD dwUserID;								//�û�ID
	TCHAR szUserWx[LEN_AGENT_ID];				//�û�΢��
	TCHAR szAgentWx[LEN_AGENT_ID];				//����΢��
	TCHAR szReason[LEN_DESC];					//����΢��
	TCHAR szImageName[LEN_IMAGE_NAME];			//����΢��
};

//�������ǰ���
struct DBR_MB_ShenQingDailiCheck
{
	DWORD dwUserID;								//�û�ID
};

//�������
struct DBR_MB_ShenQingDaili
{
	DWORD							dwUserID;						//�û�ID
	TCHAR							szAgentAcc[LEN_ACCOUNTS];		//�����˺�
	TCHAR							szAgentName[LEN_AGENT_ID];		//��������
	TCHAR							szAlipay[LEN_ALIPAY];			//֧����
	TCHAR							szWeChat[LEN_WECHAT];			//΢�ź�
	TCHAR							szQQ[LEN_QQ];					//QQ��
	TCHAR							szNote[LEN_DESC];				//��ע
};

//�������
struct DBR_MB_QueryGameLockInfo
{
	DWORD							dwUserID;						//�û�ID
};


//�ʺ�ע��
struct DBR_MB_RegisterAccounts
{
	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	BYTE                            cbDeviceType;                       //�豸����
	DWORD							dwSpreaderGameID;					//�Ƽ���ʶ
	TCHAR							szAgentID[LEN_ACCOUNTS];							//�����ʶ

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����
	TCHAR							szInsurePass[LEN_MD5];				//��������

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//�ֻ���ע��
struct DBR_MB_PhoneRegisterAccounts
{
	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	BYTE                            cbDeviceType;                       //�豸����
	DWORD							dwSpreaderGameID;					//�Ƽ���ʶ
	TCHAR							szAgentID[LEN_ACCOUNTS];							//�����ʶ

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����
	TCHAR							szInsurePass[LEN_MD5];				//��������

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//�޸�����
struct DBR_MB_ModifyPass
{
	//ע����Ϣ
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��˺�
	TCHAR							szPassword[LEN_PASSWORD];			//������
	BYTE							cbMode;								//���� ��¼
	TCHAR							szVerifyCode[LEN_VERIFY_CODE];		//ע�������֤��
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ���
	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//��֤���޸�������Ϣ
struct DBR_MB_ModifyBankInfo
{
	//ϵͳ��Ϣ
	DWORD							dwUserID;						//�û�ID
	TCHAR							szBankNo[LEN_BANK_NO];			//���п���
	TCHAR							szBankName[LEN_BANK_NAME];		//��������

	TCHAR							szBankAddress[LEN_BANK_ADDRESS];	//��������
	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//��ѯ�û���Ϣ
struct DBR_MB_QueryUserData
{
	//ϵͳ��Ϣ
	BYTE cbType;								//��������
	TCHAR szWhere[LEN_WHERE_PARM];				//��������
};

//��¼�ɹ�
struct DBO_MB_LogonSuccess
{
	//�û�����
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwCustomID;							//�Զ�ͷ��
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwExperience;						//������ֵ
	SCORE							lLoveLiness;						//�û�����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��ʺ�
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szDynamicPass[LEN_PASSWORD];		//��̬����
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
	TCHAR							szChangeLogonIP[LEN_IP];			//�޸Ŀͻ�����ʾ������IP
	TCHAR							szRegisterMobile[LEN_MOBILE_PHONE];	//�ο������ж��Ƿ�����ֻ�
	//�û��ɼ�
	SCORE							lUserScore;							//�û���Ϸ��
	SCORE							lUserIngot;							//�û�Ԫ��
	SCORE							lUserInsure;						//�û�����	
	DOUBLE							dUserBeans;							//�û���Ϸ��

	//��Ա����
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	SYSTEMTIME						MemberOverDate;						//����ʱ��

	//��չ��Ϣ
	BYTE							cbInsureEnabled;					//ʹ�ܱ�ʶ
	BYTE							cbIsAgent;							//�����ʶ						
	BYTE							cbMoorMachine;						//��������
	SCORE							lRoomCard;						//��ҷ���
	DWORD						dwLockServerID;						//��������
	DWORD						dwKindID;							//��Ϸ����

	SYSTEMTIME						RegisterData;					//ע��ʱ��
	//������Ϣ
	TCHAR							szDescribeString[128];				//������Ϣ
};


//��¼ʧ��
struct DBO_MB_LogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//˽�˷�������
struct DBO_MB_PersonalTableConfig
{
	DWORD							dwCount;							//��������
	tagPersonalTableParameter		PersonalTableParameter[50];			//������Ϣ
};

//˽�˷����б���Ϣ
struct DBO_MB_PersonalRoomInfoList
{
	DWORD							dwUserID;													//��������
	tagPersonalRoomInfo		PersonalRoomInfo[MAX_CREATE_SHOW_ROOM];			//������Ϣ
	DBO_MB_PersonalRoomInfoList()
	{
		memset(PersonalRoomInfo, 0, sizeof(tagPersonalRoomInfo));
	}
};

//��������
struct DBR_GR_QUERY_USER_ROOM_INFO
{
	DWORD							dwUserID;							//�û���ʶ
};

//��������
struct DBR_GR_CLOSE_ROOM_SERVER_ID
{
	DWORD							dwServerID;							//�û���ʶ
};

//������ҵ��û���Ϣ
struct DBR_GR_QUERY_PERSONAL_ROOM_USER_INFO
{
	DWORD							dwUserID;							//�û���ʶ
};

//˽�˷����б���Ϣ
struct DBO_MB_PersonalRoomUserInfo
{
	SCORE							lRoomCard;								//��������
	DOUBLE						dBeans;									//��Ϸ��
};

struct DBO_MB_QueryGameLockInfoResult
{
	DWORD							dwKindID;							//������ϷID
	DWORD							dwServerID;							//��Ϸ����ID
};

//�һ���Ϸ��
struct DBR_GP_ExchangeScoreByRoomCard
{
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lExchangeRoomCard;						//�һ�����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�����һ����
struct DBO_GP_RoomCardExchangeResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrRoomCard;							//��ǰԪ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//////////////////////////////////////////////////////////////////////////////////

#endif