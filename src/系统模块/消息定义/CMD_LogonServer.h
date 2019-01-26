#ifndef CMD_LONGON_HEAD_FILE
#define CMD_LONGON_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//��¼����

#define MDM_GP_LOGON				1									//�㳡��¼

//��¼ģʽ
#define SUB_GP_LOGON_GAMEID			1									//I D ��¼
#define SUB_GP_LOGON_ACCOUNTS		2									//�ʺŵ�¼
#define SUB_GP_REGISTER_ACCOUNTS	3									//ע���ʺ�
#define SUB_GP_LOGON_MANAGETOOL		4									//������
#define SUB_GP_VERIFY_INDIVIDUAL	5									//��֤����
#define SUB_GP_LOGON_VISITOR		6									//�ο͵�¼

//��¼���
#define SUB_GP_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_GP_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_GP_LOGON_FINISH			102									//��¼���
#define SUB_GP_VALIDATE_MBCARD      103                                 //��¼ʧ��
#define SUB_GP_VALIDATE_PASSPORT	104									//��¼ʧ��	
#define SUB_GP_VERIFY_RESULT		105									//��֤���
#define SUB_GP_MATCH_SIGNUPINFO		106									//������Ϣ
#define SUB_GP_GROWLEVEL_CONFIG		107									//�ȼ�����
#define SUB_GP_VERIFY_CODE_RESULT	108									//��֤���
#define SUB_GP_REAL_AUTH_CONFIG		110									//��֤����

//������ʾ
#define SUB_GP_UPDATE_NOTIFY		200									//������ʾ

//////////////////////////////////////////////////////////////////////////////////
//
#define MB_VALIDATE_FLAGS           0x01                                //Ч���ܱ�
#define LOW_VER_VALIDATE_FLAGS      0x02                                //Ч��Ͱ汾

//У������
#define VERIFY_ACCOUNTS				0x01								//У���˺�
#define VERIFY_NICKNAME				0x02								//У���ǳ�

//I D ��¼
struct CMD_GP_LogonGameID
{
	//ϵͳ��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//��¼��Ϣ
	DWORD							dwGameID;							//��Ϸ I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	BYTE							cbValidateFlags;			        //У���ʶ
};

//�ʺŵ�¼
struct CMD_GP_LogonAccounts
{
	//ϵͳ��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//��¼��Ϣ
	BYTE							cbValidateFlags;			        //У���ʶ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//���֤��
};

//ע���ʺ�
struct CMD_GP_RegisterAccounts
{
	//ϵͳ��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����

	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//֤������
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	BYTE							cbValidateFlags;			        //У���ʶ
	TCHAR							szAgentID[LEN_ACCOUNTS];			//�����ʶ
	DWORD							dwSpreaderGameID;					//�Ƽ���ʶ
};

//��֤����
struct CMD_GP_VerifyIndividual
{
	//ϵͳ��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾

	//��֤��Ϣ
	WORD							wVerifyMask;						//��֤����
};

//�ο͵�¼
struct CMD_GP_LogonVisitor
{
	DWORD							dwPlazaVersion;						//�㳡�汾
	BYTE							cbValidateFlags;			        //У���ʶ

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��¼�ɹ�
struct CMD_GP_LogonSuccess
{
	//��������
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwGroupID;							//���ű�ʶ
	DWORD							dwCustomID;							//�Զ���ʶ	
	DWORD							dwExperience;						//������ֵ
	SCORE							lLoveLiness;						//�û�����

	//�û��ɼ�
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//�û�����
	SCORE							lUserIngot;							//�û�Ԫ��
	DOUBLE							dUserBeans;							//�û���Ϸ��

	//�û���Ϣ
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMoorMachine;						//��������
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szDynamicPass[LEN_PASSWORD];		//��̬����
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������

	//������Ϣ
	BYTE							cbInsureEnabled;					//����ʹ�ܱ�ʶ
	BYTE                            cbShowServerStatus;                 //��ʾ������״̬
	BYTE							cbIsAgent;							//�����ʶ
};

//��¼ʧ��
struct CMD_GP_LogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��½���
struct CMD_GP_LogonFinish
{
	WORD							wIntermitTime;						//�ж�ʱ��
	WORD							wOnLineCountTime;					//����ʱ��
};

//��¼ʧ��
struct CMD_GP_ValidateMBCard
{
	UINT								uMBCardID;						//��������
};

//��֤���
struct CMD_GP_VerifyIndividualResult
{
	bool							bVerifyPassage;						//��֤ͨ��
	WORD							wVerifyMask;						//��֤����
	TCHAR							szErrorMsg[128];					//������Ϣ
};

//������ʾ
struct CMD_GP_UpdateNotify
{
	BYTE							cbMustUpdate;						//ǿ������
	BYTE							cbAdviceUpdate;						//��������
	DWORD							dwCurrentVersion;					//��ǰ�汾
};

//��֤���
struct CMD_GP_VerifyCodeResult
{
	BYTE								cbResultCode;					//�����ʶ��1�ɹ���0ʧ�ܣ�2�ظ����룩
	TCHAR								szDescString[64];				//����ַ���
};

//////////////////////////////////////////////////////////////////////////////////
//Я����Ϣ CMD_GP_LogonSuccess

#define DTP_GP_GROUP_INFO			1									//������Ϣ
#define DTP_GP_MEMBER_INFO			2									//��Ա��Ϣ
#define	DTP_GP_UNDER_WRITE			3									//����ǩ��
#define DTP_GP_STATION_URL			4									//��ҳ��Ϣ

//������Ϣ
struct DTP_GP_GroupInfo
{
	DWORD							dwGroupID;							//��������
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������
};

//��Ա��Ϣ
struct DTP_GP_MemberInfo
{
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	SYSTEMTIME						MemberOverDate;						//����ʱ��
};

//////////////////////////////////////////////////////////////////////////////////
//�б�����

#define MDM_GP_SERVER_LIST			2									//�б���Ϣ

//��ȡ����
#define SUB_GP_GET_LIST				1									//��ȡ�б�
#define SUB_GP_GET_SERVER			2									//��ȡ����
#define SUB_GP_GET_MATCH			3									//��ȡ����
#define SUB_GP_GET_ONLINE			4									//��ȡ����
#define SUB_GP_GET_COLLECTION		5									//��ȡ�ղ�
#define SUB_GP_GET_PROPERTY			6									//��ȡ����

//�б���Ϣ
#define SUB_GP_LIST_TYPE			100									//�����б�
#define SUB_GP_LIST_KIND			101									//�����б�
#define SUB_GP_LIST_NODE			102									//�ڵ��б�
#define SUB_GP_LIST_PAGE			103									//�����б�
#define SUB_GP_LIST_SERVER			104									//�����б�
#define SUB_GP_LIST_MATCH			105									//�����б�
#define SUB_GP_VIDEO_OPTION			106									//��Ƶ����
#define SUB_GP_AGENT_KIND			107									//�����б�

//������Ϣ
#define SUB_GP_LIST_PROPERTY_TYPE   110									//��������
#define SUB_GP_LIST_PROPERTY_RELAT  111									//���߹�ϵ
#define SUB_GP_LIST_PROPERTY	    112									//�����б�
#define SUB_GP_LIST_PROPERTY_SUB    113									//�ӵ����б�

//�����Ϣ
#define SUB_GP_LIST_FINISH			200									//�������
#define SUB_GP_SERVER_FINISH		201									//�������
#define SUB_GP_MATCH_FINISH			202									//�������
#define SUB_GP_PROPERTY_FINISH		203									//�������

//������Ϣ
#define SUB_GR_KINE_ONLINE			300									//��������
#define SUB_GR_SERVER_ONLINE		301									//��������

//////////////////////////////////////////////////////////////////////////////////

//��ȡ����
struct CMD_GP_GetOnline
{
	WORD							wServerCount;						//������Ŀ
	WORD							wOnLineServerID[MAX_SERVER];		//�����ʶ
};

//��������
struct CMD_GP_KindOnline
{
	WORD							wKindCount;							//������Ŀ
	tagOnLineInfoKind				OnLineInfoKind[MAX_KIND];			//��������
};

//��������
struct CMD_GP_ServerOnline
{
	WORD							wServerCount;						//������Ŀ
	tagOnLineInfoServer				OnLineInfoServer[MAX_SERVER];		//��������
};

//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GP_USER_SERVICE				3								//�û�����

//�˺ŷ���
#define SUB_GP_MODIFY_MACHINE			100								//�޸Ļ���
#define SUB_GP_MODIFY_LOGON_PASS		101								//�޸�����
#define SUB_GP_MODIFY_INSURE_PASS		102								//�޸�����
#define SUB_GP_MODIFY_UNDER_WRITE		103								//�޸�ǩ��

//�޸�ͷ��
#define SUB_GP_USER_FACE_INFO			120								//ͷ����Ϣ
#define SUB_GP_SYSTEM_FACE_INFO			122								//ϵͳͷ��
#define SUB_GP_CUSTOM_FACE_INFO			123								//�Զ�ͷ��

//��������
#define SUB_GP_USER_INDIVIDUAL			140								//��������
#define	SUB_GP_QUERY_INDIVIDUAL			141								//��ѯ��Ϣ
#define SUB_GP_MODIFY_INDIVIDUAL		152								//�޸�����
#define SUB_GP_INDIVIDUAL_RESULT		153								//��������
#define SUB_GP_REAL_AUTH_QUERY			154								//��֤����
#define SUB_GP_REAL_AUTH_RESULT			155								//��֤���

//���з���
#define SUB_GP_USER_ENABLE_INSURE		160								//��ͨ����
#define SUB_GP_USER_SAVE_SCORE			161								//������
#define SUB_GP_USER_TAKE_SCORE			162								//ȡ�����
#define SUB_GP_USER_TRANSFER_SCORE		163								//ת�˲���
#define SUB_GP_USER_INSURE_INFO			164								//��������
#define SUB_GP_QUERY_INSURE_INFO		165								//��ѯ����
#define SUB_GP_USER_INSURE_SUCCESS		166								//���гɹ�
#define SUB_GP_USER_INSURE_FAILURE		167								//����ʧ��
#define SUB_GP_QUERY_USER_INFO_REQUEST	168								//��ѯ�û�
#define SUB_GP_QUERY_USER_INFO_RESULT	169								//�û���Ϣ
#define SUB_GP_USER_INSURE_ENABLE_RESULT 170							//��ͨ���	
#define SUB_GP_QUERY_TRANSFER_REBATE	171								//��ѯ����
#define SUB_GP_QUERY_TRANSFER_REBATE_RESULT	172							//��ѯ���
#define SUB_GP_QUERY_USERDATA_RESULT	173								//��ѯ���
#define SUB_GP_QUERY_BANK_INFO			174								//��ѯ�û���������Ϣ
#define SUB_GP_QUERY_BANK_INFO_RESULT	175								//��ѯ�û������н��
#define SUB_GP_BIND_BANK_INFO			176								//�����п���Ϣ
#define SUB_GP_EXCHANGE_BANK_SCORE		177								//�һ����н��
#define SUB_GP_QUERY_GAMELOCKINFO_RESULT		178						//��ѯ������

//ǩ������
#define SUB_GP_CHECKIN_QUERY			220								//��ѯǩ��
#define SUB_GP_CHECKIN_INFO				221								//ǩ����Ϣ
#define SUB_GP_CHECKIN_DONE				222								//ִ��ǩ��
#define SUB_GP_CHECKIN_RESULT			223								//ǩ�����

//�������
#define SUB_GP_TASK_LOAD				240								//�������
#define SUB_GP_TASK_TAKE				241								//������ȡ
#define SUB_GP_TASK_REWARD				242								//������
#define SUB_GP_TASK_GIVEUP				243								//�������
#define SUB_GP_TASK_INFO				250								//������Ϣ
#define SUB_GP_TASK_LIST				251								//������Ϣ
#define SUB_GP_TASK_RESULT				252								//������
#define SUB_GP_TASK_GIVEUP_RESULT		253								//�������

//�ͱ�����
#define SUB_GP_BASEENSURE_LOAD			260								//���صͱ�
#define SUB_GP_BASEENSURE_TAKE			261								//��ȡ�ͱ�
#define SUB_GP_BASEENSURE_PARAMETER		262								//�ͱ�����
#define SUB_GP_BASEENSURE_RESULT		263								//�ͱ����

//�ƹ����
#define SUB_GP_SPREAD_QUERY				280								//�ƹ㽱��
#define SUB_GP_SPREAD_INFO				281								//��������

//�ȼ�����
#define SUB_GP_GROWLEVEL_QUERY			300								//��ѯ�ȼ�
#define SUB_GP_GROWLEVEL_PARAMETER		301								//�ȼ�����
#define SUB_GP_GROWLEVEL_UPGRADE		302								//�ȼ�����

//�һ�����
#define SUB_GP_EXCHANGE_QUERY			320								//�һ�����
#define SUB_GP_EXCHANGE_PARAMETER		321								//�һ�����
#define SUB_GP_PURCHASE_MEMBER			322								//�����Ա
#define SUB_GP_PURCHASE_RESULT			323								//������
#define SUB_GP_EXCHANGE_SCORE_BYINGOT	324								//�һ���Ϸ��
#define SUB_GP_EXCHANGE_SCORE_BYBEANS	325								//�һ���Ϸ��
#define SUB_GP_EXCHANGE_RESULT			326								//�һ����

//��Ա����
#define SUB_GP_MEMBER_PARAMETER			340								//��Ա����
#define SUB_GP_MEMBER_QUERY_INFO		341								//��Ա��ѯ
#define SUB_GP_MEMBER_DAY_PRESENT		342								//��Ա�ͽ�
#define SUB_GP_MEMBER_DAY_GIFT			343								//��Ա���
#define SUB_GP_MEMBER_PARAMETER_RESULT	350								//�������
#define SUB_GP_MEMBER_QUERY_INFO_RESULT	351								//��ѯ���
#define SUB_GP_MEMBER_DAY_PRESENT_RESULT	352							//�ͽ���
#define SUB_GP_MEMBER_DAY_GIFT_RESULT	353								//������

//�齱����
#define SUB_GP_LOTTERY_CONFIG_REQ		360								//��������
#define SUB_GP_LOTTERY_CONFIG			361								//�齱����
#define SUB_GP_LOTTERY_USER_INFO		362								//�齱��Ϣ
#define SUB_GP_LOTTERY_START			363								//��ʼ�齱
#define SUB_GP_LOTTERY_RESULT			364								//�齱���

//��Ϸ����
#define SUB_GP_QUERY_USER_GAME_DATA		370								//��ѯ����	

//�ʺŰ�
#define SUB_GP_ACCOUNT_BINDING			380								//�ʺŰ�
#define SUB_GP_ACCOUNT_BINDING_EXISTS	381								//�ʺŰ�

#define SUB_GP_IP_LOCATION				382								//ip������

//�������
#define SUB_GP_OPERATE_SUCCESS			500								//�����ɹ�
#define SUB_GP_OPERATE_FAILURE			501								//����ʧ��

//////////////////////////////////////////////////////////////////////////////////

//�޸�����
struct CMD_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�����
struct CMD_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�ǩ��
struct CMD_GP_ModifyUnderWrite
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
};

//ʵ����֤
struct CMD_GP_RealAuth
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//֤������
};
//////////////////////////////////////////////////////////////////////////////////

//�û�ͷ��
struct CMD_GP_UserFaceInfo
{
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwCustomID;							//�Զ���ʶ
};

//�޸�ͷ��
struct CMD_GP_SystemFaceInfo
{
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸�ͷ��
struct CMD_GP_CustomFaceInfo
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//ͼƬ��Ϣ
};

//////////////////////////////////////////////////////////////////////////////////

//�󶨻���
struct CMD_GP_ModifyMachine
{
	BYTE							cbBind;								//�󶨱�־
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_GP_UserIndividual
{
	DWORD							dwUserID;							//�û� I D
};

//��ѯ��Ϣ
struct CMD_GP_QueryIndividual
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//�޸�����
struct CMD_GP_ModifyIndividual
{
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//��ѯ����
struct CMD_GP_QuerySendPresent
{
	DWORD							dwUserID;							//�û� I D
};

//////////////////////////////////////////////////////////////////////////////////
//Я����Ϣ CMD_GP_UserIndividual
#define DTP_GP_UI_ACCOUNTS			1									//�û��˺�	
#define DTP_GP_UI_NICKNAME			2									//�û��ǳ�
#define DTP_GP_UI_USER_NOTE			3									//�û�˵��
#define DTP_GP_UI_UNDER_WRITE		4									//����ǩ��
#define DTP_GP_UI_QQ				5									//Q Q ����
#define DTP_GP_UI_EMAIL				6									//�����ʼ�
#define DTP_GP_UI_SEAT_PHONE		7									//�̶��绰
#define DTP_GP_UI_MOBILE_PHONE		8									//�ƶ��绰
#define DTP_GP_UI_COMPELLATION		9									//��ʵ����
#define DTP_GP_UI_DWELLING_PLACE	10									//��ϵ��ַ
#define DTP_GP_UI_PASSPORTID    	11									//��ݱ�ʶ
#define DTP_GP_UI_SPREADER			12									//�ƹ��ʶ
//////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_GP_UserInsureInfo
{
	BYTE							cbEnjoinTransfer;					//ת�˿���
	WORD							wRevenueTake;						//˰�ձ���
	WORD							wRevenueTransfer;					//˰�ձ���
	WORD							wRevenueTransferMember;				//˰�ձ���
	WORD							wServerID;							//�����ʶ
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//���н��
	SCORE							lTransferPrerequisite;				//ת������
};

//��ͨ����
struct CMD_GP_UserEnableInsure
{
	DWORD							dwUserID;							//�û�I D
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������
struct CMD_GP_UserSaveScore
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lSaveScore;							//������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ���
struct CMD_GP_UserTakeScore
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lTakeScore;							//��ȡ���
	TCHAR							szPassword[LEN_MD5];				//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ת�˽��
struct CMD_GP_UserTransferScore
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lTransferScore;						//ת�˽��
	TCHAR							szPassword[LEN_MD5];				//��������
	TCHAR							szAccounts[LEN_NICKNAME];			//Ŀ���û�
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szTransRemark[LEN_TRANS_REMARK];	//ת�˱�ע
};

//���гɹ�
struct CMD_GP_UserInsureSuccess
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//���н��
	TCHAR							szDescribeString[128];				//������Ϣ
};

//����ʧ��
struct CMD_GP_UserInsureFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��ȡ���
struct CMD_GP_UserTakeResult
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//���н��
};

//��ѯ����
struct CMD_GP_QueryInsureInfo
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��������
};

//��ѯ�û������п���Ϣ
struct CMD_GP_QueryBankInfo
{
	DWORD							dwUserID;							//�û� I D
};

//��ѯ�û�
struct CMD_GP_QueryUserInfoRequest
{
	BYTE                            cbByNickName;                       //�ǳ�����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//Ŀ���û�
};

//�û���Ϣ
struct CMD_GP_UserTransferUserInfo
{
	DWORD							dwTargetGameID;						//Ŀ���û�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//Ŀ���û�
};

//��ͨ���
struct CMD_GP_UserInsureEnableResult
{
	BYTE							cbInsureEnabled;					//ʹ�ܱ�ʶ
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��ѯ����
struct CMD_GP_QueryTransferRebate
{
	DWORD							dwUserID;							//�û�I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//��ѯ���
struct CMD_GP_QueryTransferRebateResult
{
	DWORD							dwUserID;							//�û�I D
	BYTE							cbRebateEnabled;					//ʹ�ܱ�ʶ	
	SCORE							lIngot;								//����Ԫ��
	SCORE							lLoveLiness;						//��������ֵ
};

//�û����ݽ��
struct CMD_GP_QueryUserDataResult
{
	DWORD							dwUserID;							//�û�ID
};

//////////////////////////////////////////////////////////////////////////////////
//��ѯǩ��
struct CMD_GP_CheckInQueryInfo
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//ǩ����Ϣ
struct CMD_GP_CheckInInfo
{	
	WORD							wSeriesDate;						//��������
	bool							bTodayChecked;						//ǩ����ʶ
	SCORE							lRewardGold[LEN_WEEK];				//�������	
};

//ִ��ǩ��
struct CMD_GP_CheckInDone
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ǩ�����
struct CMD_GP_CheckInResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lScore;								//��ǰ���
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//////////////////////////////////////////////////////////////////////////////////
//�������

//��������
struct CMD_GP_TaskLoadInfo
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//��������
struct CMD_GP_TaskGiveUp
{
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ����
struct CMD_GP_TaskTake
{
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ����
struct CMD_GP_TaskReward
{
	WORD							wTaskID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������Ϣ
struct CMD_GP_TaskInfo
{
	WORD							wTaskCount;							//��������
	tagTaskStatus					TaskStatus[TASK_MAX_COUNT];			//����״̬
};

//������
struct CMD_GP_TaskResult
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
//�ͱ�����

//��ȡ�ͱ�
struct CMD_GP_BaseEnsureTake
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�ͱ�����
struct CMD_GP_BaseEnsureParamter
{
	SCORE							lScoreCondition;					//��Ϸ������
	SCORE							lScoreAmount;						//��Ϸ������
	BYTE							cbTakeTimes;						//��ȡ����	
};

//�ͱ����
struct CMD_GP_BaseEnsureResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lGameScore;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//////////////////////////////////////////////////////////////////////////////////
//�ƹ����

//�ƹ��ѯ
struct CMD_GP_UserSpreadQuery
{
	DWORD							dwUserID;							//�û���ʶ
};

//�ƹ����
struct CMD_GP_UserSpreadInfo
{
	DWORD							dwSpreadCount;						//�ƹ�����
	SCORE							lSpreadReward;						//�ƹ㽱��
};

//��֤����
struct CMD_GP_RealAuthParameter
{
	DWORD							dwAuthentDisable;					//��֤����
	SCORE							dwAuthRealAward;					//�������
};


//////////////////////////////////////////////////////////////////////////////////
//�ȼ�����

//��ѯ�ȼ�
struct CMD_GP_GrowLevelQueryInfo
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�ȼ�����
struct CMD_GP_GrowLevelConfig
{
	WORD							wLevelCount;						//�ȼ���Ŀ
	tagGrowLevelConfig				GrowLevelItem[60];					//�ȼ�����
};

//�ȼ�����
struct CMD_GP_GrowLevelParameter
{
	WORD							wCurrLevelID;						//��ǰ�ȼ�
	DWORD							dwExperience;						//��ǰ����
	DWORD							dwUpgradeExperience;				//�¼�����
	SCORE							lUpgradeRewardGold;					//��������
	SCORE							lUpgradeRewardIngot;				//��������
};

//�ȼ�����
struct CMD_GP_GrowLevelUpgrade
{
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrIngot;							//��ǰԪ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};
//////////////////////////////////////////////////////////////////////////////////
//��Ա����

//��Ա����
struct CMD_GP_MemberParameterResult
{
	WORD							wMemberCount;						//��Ա��Ŀ
	tagMemberParameterNew			MemberParameter[10];				//��Ա����
};

//��Ա��ѯ
struct CMD_GP_MemberQueryInfo
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ���
struct CMD_GP_MemberQueryInfoResult
{
	bool							bPresent;							//�ͽ���
	bool							bGift;								//������
	DWORD							GiftSubCount;						//��������
	tagGiftPropertyInfo				GiftSub[50];						//��������
};

//��Ա�ͽ�
struct CMD_GP_MemberDayPresent
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�ͽ���
struct CMD_GP_MemberDayPresentResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lGameScore;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//��Ա���
struct CMD_GP_MemberDayGift
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������

struct CMD_GP_MemberDayGiftResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//////////////////////////////////////////////////////////////////////////////////
//�һ�����

//��ѯ����
struct CMD_GP_ExchangeParameter
{
	DWORD							dwExchangeRate;						//Ԫ����Ϸ�Ҷһ�����
	DWORD							dwPresentExchangeRate;				//������Ϸ�Ҷһ���
	DWORD							dwRateGold;							//��Ϸ����Ϸ�Ҷһ���
	WORD							wMemberCount;						//��Ա��Ŀ
	tagMemberParameter				MemberParameter[10];				//��Ա����
};

//�����Ա
struct CMD_GP_PurchaseMember
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbMemberOrder;						//��Ա��ʶ
	WORD							wPurchaseTime;						//����ʱ��
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//������
struct CMD_GP_PurchaseResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	BYTE							cbMemberOrder;						//��Աϵ��
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	DOUBLE							dCurrBeans;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�һ���Ϸ��
struct CMD_GP_ExchangeScoreByIngot
{
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lExchangeIngot;						//Ԫ������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//�һ���Ϸ��
struct CMD_GP_ExchangeScoreByBeans
{
	DWORD							dwUserID;							//�û���ʶ
	double							dExchangeBeans;						//��Ϸ������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//�һ����
struct CMD_GP_ExchangeResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lCurrIngot;							//��ǰԪ��
	double							dCurrBeans;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�һ����
struct CMD_GP_ExchangeRoomCardResult
{
	bool								bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	SCORE							lRoomCard;							//��ǰ����
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//////////////////////////////////////////////////////////////////////////////////
//�齱����

//��������
struct CMD_GP_LotteryConfigReq
{
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_MD5];				//��¼����
};

//�齱����
struct CMD_GP_LotteryConfig
{
	WORD							wLotteryCount;						//�������
	tagLotteryItem					LotteryItem[MAX_LOTTERY];			//��������
};

//�齱��Ϣ
struct CMD_GP_LotteryUserInfo
{
	BYTE							cbFreeCount;						//��Ѵ���
	BYTE							cbAlreadyCount;						//�������
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lChargeFee;							//�齱����
};

//��ʼ�齱
struct CMD_GP_LotteryStart
{
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_MD5];				//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�齱���
struct CMD_GP_LotteryResult
{
	bool							bWined;								//�н���ʶ
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lUserScore;							//�û�����
	DOUBLE							dUserBeans;							//�û���Ϸ��
	tagLotteryItem					LotteryItem;						//�н�����
};

//////////////////////////////////////////////////////////////////////////////////
//��Ϸ����
struct CMD_GP_QueryUserGameData
{
	WORD							wKindID;							//��Ϸ��ʶ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szDynamicPass[LEN_MD5];				//�û�����	
};

//������Ϣ
#define DTP_GP_UI_USER_GAME_DATA	1									//��Ϸ����	

//////////////////////////////////////////////////////////////////////////////////
//���Ͻ��
struct CMD_GP_IndividuaResult
{
	bool							bSuccessed;							//�ɹ���ʶ
	SCORE							lCurrScore;							//��ǰ��Ϸ��
	TCHAR							szNotifyContent[128];				//��ʾ����
};


//////////////////////////////////////////////////////////////////////////////////
//��������
struct CMD_GP_MatchSignup
{
	//������Ϣ
	WORD							wServerID;							//�����ʶ
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ȡ������
struct CMD_GP_MatchUnSignup
{
	//������Ϣ
	WORD							wServerID;							//�����ʶ
	DWORD							dwMatchID;							//������ʶ
	DWORD							dwMatchNO;							//��������

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�������
struct CMD_GP_MatchSignupResult
{
	bool							bSignup;							//������ʶ
	bool							bSuccessed;							//�ɹ���ʶ
	WORD							wServerID;							//�����ʶ
	SCORE							lCurrScore;							//��ǰ���
	TCHAR							szDescribeString[128];				//������Ϣ
};
//////////////////////////////////////////////////////////////////////////////////
//�˻���
struct CMD_GP_AccountBind
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	TCHAR							szBindNewAccounts[LEN_ACCOUNTS];	//���ʺ�
	TCHAR							szBindNewPassword[LEN_PASSWORD];	//������
	TCHAR							szBindNewSpreader[LEN_ACCOUNTS];	//���Ƽ�
};

//�˻���
struct CMD_GP_AccountBind_Exists
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	TCHAR							szBindExistsAccounts[LEN_ACCOUNTS];	//���ʺ�
	TCHAR							szBindExistsPassword[LEN_PASSWORD];	//������
};

//ip������
struct CMD_GP_IpLocation
{
	DWORD							dwUserID;
	TCHAR							szLocation[LEN_IP];	//ip������
};

//////////////////////////////////////////////////////////////////////////////////

//����ʧ��
struct CMD_GP_OperateFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�����ɹ�
struct CMD_GP_OperateSuccess
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//Զ�̷���

#define MDM_GP_REMOTE_SERVICE		4									//Զ�̷���

//���ҷ���
#define SUB_GP_C_SEARCH_DATABASE	100									//���ݲ���
#define SUB_GP_C_SEARCH_CORRESPOND	101									//Э������
#define SUB_GP_C_SEARCH_ALLCORRESPOND	102								//Э������

//���ҷ���
#define SUB_GP_S_SEARCH_DATABASE	200									//���ݲ���
#define SUB_GP_S_SEARCH_CORRESPOND	201									//Э������
#define SUB_GP_S_SEARCH_ALLCORRESPOND	202								//Э������

//////////////////////////////////////////////////////////////////////////////////

//Э������
struct CMD_GP_C_SearchCorrespond
{
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
};

struct CMD_GP_C_SearchAllCorrespond
{
	DWORD							dwCount;
	DWORD							dwGameID[1];							//��Ϸ��ʶ
};

//Э������
struct CMD_GP_S_SearchCorrespond
{
	WORD							wUserCount;							//�û���Ŀ
	tagUserRemoteInfo				UserRemoteInfo[16];					//�û���Ϣ
};

struct CMD_GP_S_SearchAllCorrespond
{
	DWORD							dwCount;
	tagUserRemoteInfo				UserRemoteInfo[1];					//�û���Ϣ				
};


//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GP_ANDROID_SERVICE		5									//��������

//��������
#define SUB_GP_GET_PARAMETER		100									//��ȡ����
#define SUB_GP_ADD_PARAMETER		101									//��Ӳ���
#define SUB_GP_MODIFY_PARAMETER		102									//�޸Ĳ���
#define SUB_GP_DELETE_PARAMETER		103									//ɾ������

//������Ϣ
#define SUB_GP_ANDROID_PARAMETER	200									//��������		

//////////////////////////////////////////////////////////////////////////////////
//��ȡ����
struct CMD_GP_GetParameter
{
	WORD							wServerID;							//�����ʶ
};

//��Ӳ���
struct CMD_GP_AddParameter
{
	WORD							wServerID;							//�����ʶ
	tagAndroidParameter				AndroidParameter;					//��������
};

//�޸Ĳ���
struct CMD_GP_ModifyParameter
{
	WORD							wServerID;							//�����ʶ
	tagAndroidParameter				AndroidParameter;					//��������
};

//ɾ������
struct CMD_GP_DeleteParameter
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwBatchID;							//���α�ʶ
};



//////////////////////////////////////////////////////////////////////////////////
//��������
#define	MDM_GP_PROPERTY						6	

//������Ϣ
#define SUB_GP_QUERY_PROPERTY				1							//���߲�ѯ
#define SUB_GP_PROPERTY_BUY					2							//�������
#define SUB_GP_PROPERTY_USE					3							//����ʹ��
#define SUB_GP_QUERY_BACKPACKET				4							//������ѯ
#define SUB_GP_PROPERTY_BUFF				5							//����Buff
#define SUB_GP_QUERY_SEND_PRESENT			6							//��ѯ����
#define SUB_GP_PROPERTY_PRESENT				7							//���͵���
#define SUB_GP_GET_SEND_PRESENT				8							//��ȡ����
#define SUB_GP_QUERY_SINGLE					9							//������ѯ

#define SUB_GP_QUERY_PROPERTY_RESULT		101							//���߲�ѯ
#define SUB_GP_PROPERTY_BUY_RESULT			102							//�������
#define SUB_GP_PROPERTY_USE_RESULT			103							//����ʹ��
#define SUB_GP_QUERY_BACKPACKET_RESULT		104							//������ѯ
#define SUB_GP_PROPERTY_BUFF_RESULT			105							//����Buff
#define SUB_GP_QUERY_SEND_PRESENT_RESULT	106							//��ѯ����
#define SUB_GP_PROPERTY_PRESENT_RESULT		107							//���͵���
#define SUB_GP_GET_SEND_PRESENT_RESULT		108							//��ȡ����
#define SUB_GP_QUERY_SINGLE_RESULT			109							//��ȡ����


#define SUB_GP_QUERY_PROPERTY_RESULT_FINISH	310							//���߲�ѯ

#define SUB_GP_PROPERTY_FAILURE				404							//����ʧ��


//����ʧ��
struct CMD_GP_PropertyFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�������
struct CMD_GP_PropertyBuy
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwItemCount;						//������Ŀ
	BYTE							cbConsumeType;						//��������
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������
struct CMD_GP_PropertyBuyResult
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

//��ѯ����
struct CMD_GP_PropertyQuerySingle
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwPropertyID;						//���߱�ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��¼����
};

//��ѯ���
struct CMD_GP_PropertyQuerySingleResult
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwPropertyID;						//���߱�ʶ
	DWORD							dwItemCount;						//������Ŀ
};

//ʹ�õ���
struct CMD_GP_C_PropertyUse
{
	DWORD							dwUserID;							//ʹ����
	DWORD							dwRecvUserID;						//��˭ʹ��
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
};

//ʹ�õ���
struct CMD_GP_S_PropertyUse
{
	DWORD							dwUserID;							//ʹ����
	DWORD							dwRecvUserID;						//��˭ʹ��
	DWORD							dwPropID;							//����ID
	DWORD							dwScoreMultiple;					//Ч������
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

//���͵���
struct CMD_GP_S_PropertyPresent
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

//��ѯ����
struct CMD_GP_S_QuerySendPresent
{
	WORD							wPresentCount;						//���ʹ���
	SendPresent						Present[MAX_PROPERTY];		
};

//��ȡ����
struct CMD_GP_S_GetSendPresent
{
	WORD							wPresentCount;						//���ʹ���
	SendPresent						Present[MAX_PROPERTY];	
};

//������������
struct CMD_GP_C_BackpackProperty
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwKind;								//��������
};


//���߽��
struct CMD_GP_S_BackpackProperty
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwStatus;							//״̬
	DWORD							dwCount;							//����
	tagBackpackProperty				PropertyInfo[1];					//������Ϣ
};


//����Buff
struct CMD_GP_C_UserPropertyBuff
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
};

struct CMD_GP_S_UserPropertyBuff
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbBuffCount;						//Buff��Ŀ
	tagPropertyBuff					PropertyBuff[MAX_BUFF];			
};


//���͵���
struct CMD_GP_C_PropertyPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwRecvGameID;						//���߸�˭
	DWORD							dwPropID;							//����ID
	WORD							wPropCount;							//ʹ����Ŀ
	WORD							wType;								//Ŀ������
	TCHAR							szRecvNickName[16];					//��˭ʹ��
};

//��ȡ����
struct CMD_GP_C_GetSendPresent
{
	DWORD							dwUserID;							//������
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};



//////////////////////////////////////////////////////////////////////////////////
//��������
struct CMD_GP_AndroidParameter
{
	WORD							wSubCommdID;						//��������
	WORD							wParameterCount;					//������Ŀ
	tagAndroidParameter				AndroidParameter[MAX_BATCH];		//��������
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//��¼����
#define MDM_MB_LOGON				100									//�㳡��¼

//��¼ģʽ
#define SUB_MB_LOGON_GAMEID			1									//I D ��¼
#define SUB_MB_LOGON_ACCOUNTS		2									//�ʺŵ�¼
#define SUB_MB_REGISTER_ACCOUNTS	3									//ע���ʺ�
#define SUB_MB_LOGON_OTHERPLATFORM	4									//������¼
#define SUB_MB_LOGON_VISITOR		5									//�ο͵�¼
#define SUB_MB_PHONE_REGISTER_VERIFYCODE 6								//�ֻ���ע��
#define SUB_MB_PHONE_GET_REGISTER_VERIFYCODE 7							//��ȡ�ֻ�����֤��
#define SUB_MB_GET_MODIFY_PASS_VERIFYCODE	8							//��ȡ����������֤��
#define SUB_MB_MODIFY_PASS_VERIFYCODE		9							//�޸�����
#define SUB_MB_QUERY_USERDATA 				 10							//��ѯ�û���Ϣ
#define SUB_MB_TOUSU_AGENT 				 11								//Ͷ�ߴ���
#define SUB_MB_SHENGQING_AGENT_CHECK	12								//�������������
#define SUB_MB_SHENGQING_AGENT			13								//�������
#define SUB_MB_QUERY_GAMELOCKINFO 				 14							//��ѯ��Ϸ������Ϣ
#define SUB_MB_GET_MODIFY_BANKINFO_VERIFYCODE	15							//��ȡ�޸�������Ϣ��֤��
#define SUB_MB_MODIFY_BANKINFO_VERIFYCODE		16							//�޸�������Ϣ

//��¼���
#define SUB_MB_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_MB_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_MB_MATCH_SIGNUPINFO		102									//������Ϣ
#define SUB_MB_PERSONAL_TABLE_CONFIG	103								//˽�˷�������

//������ʾ
#define SUB_MB_UPDATE_NOTIFY		200									//������ʾ

//////////////////////////////////////////////////////////////////////////////////

//I D ��¼
struct CMD_MB_LogonGameID
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	DWORD							dwPlazaVersion;						//�㳡�汾
	BYTE                            cbDeviceType;                       //�豸����

	//��¼��Ϣ
	DWORD							dwGameID;							//��Ϸ I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
};

//�ʺŵ�¼
struct CMD_MB_LogonAccounts
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	DWORD							dwPlazaVersion;						//�㳡�汾
	BYTE                            cbDeviceType;                       //�豸����

	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
};

//�ʺŵ�¼
struct CMD_MB_LogonOtherPlatform
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	DWORD							dwPlazaVersion;						//�㳡�汾
	BYTE                            cbDeviceType;                       //�豸����

	//��¼��Ϣ
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbPlatformID;						//ƽ̨���
	TCHAR							szUserUin[LEN_USER_UIN];			//�û�Uin
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szAgentID[LEN_ACCOUNTS];			//�����ʶ

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
};

//ע���ʺ�
struct CMD_MB_RegisterAccounts
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	DWORD							dwPlazaVersion;						//�㳡�汾
	BYTE                            cbDeviceType;                       //�豸����

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����

	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	DWORD							dwSpreaderGameID;					//�Ƽ���ʶ	
	TCHAR							szAgentID[LEN_ACCOUNTS];			//�����ʶ	

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
	TCHAR							szRegisterMobile[LEN_MOBILE_PHONE];	//ע���ֻ�����-���˺���
};

//ע���ֻ��ʺ�
struct CMD_MB_PhoneRegisterAccounts
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	DWORD							dwPlazaVersion;						//�㳡�汾
	BYTE                            cbDeviceType;                       //�豸����

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����

	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//��¼�ʺ�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	DWORD							dwSpreaderGameID;					//�Ƽ���ʶ	
	TCHAR							szAgentID[LEN_ACCOUNTS];			//�����ʶ	
	TCHAR							szVerifyCode[LEN_VERIFY_CODE];		//ע�������֤��

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};


//��ȡ�ֻ�ע����֤��
struct CMD_MB_GetRegisterVerifyCode
{
	//ϵͳ��Ϣ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ���
	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��ȡ�û���Ϣ
struct CMD_MB_QueryUserData
{
	//ϵͳ��Ϣ
	BYTE cbType;								//��������
	TCHAR szWhere[LEN_WHERE_PARM];				//��������
};

//Ͷ�ߴ���
struct CMD_MB_TouSuDaili
{
	//�û�ID
	DWORD dwUserID;								//�û�ID
	TCHAR szUserWx[LEN_AGENT_ID];				//�û�΢��
	TCHAR szAgentWx[LEN_AGENT_ID];				//����΢��
	TCHAR szReason[LEN_DESC];					//����΢��
	TCHAR szImageName[LEN_IMAGE_NAME];			//����΢��
};

//�������ǰ���
struct CMD_MB_ShenQingDailiCheck
{
	DWORD dwUserID;								//�û�ID
};

//�������
struct CMD_MB_ShenQingDaili
{
	DWORD							dwUserID;						//�û�ID
	TCHAR							szAgentAcc[LEN_ACCOUNTS];		//�����˺�
	TCHAR							szAgentName[LEN_AGENT_ID];		//��������
	TCHAR							szAlipay[LEN_ALIPAY];			//֧����
	TCHAR							szWeChat[LEN_WECHAT];			//΢�ź�
	TCHAR							szQQ[LEN_QQ];					//QQ��
	TCHAR							szNote[LEN_DESC];				//��ע
};

//��ѯ��Ϸ������Ϣ
struct CMD_MB_QueryGameLockInfo
{
	DWORD							dwUserID;						//�û�ID
};

//��ȡ�޸�������֤��
struct CMD_MB_GetModifyPassVerifyCode
{
	//ϵͳ��Ϣ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ���
	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��ȡ�޸�������Ϣ��֤��
struct CMD_MB_GetModifyBankInfoVerifyCode
{
	//ϵͳ��Ϣ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ���
	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��֤���޸�����
struct CMD_MB_ModifyPassVerifyCode
{
	//ϵͳ��Ϣ
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��˺�
	TCHAR							szPassword[LEN_PASSWORD];			//������
	BYTE							cbMode;								//���� ��¼
	TCHAR							szVerifyCode[LEN_VERIFY_CODE];		//ע�������֤��
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ���
	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��֤���޸�������Ϣ
struct CMD_MB_ModifyBankInfoVerifyCode
{
	//ϵͳ��Ϣ
	DWORD							dwUserID;						//�û�ID
	TCHAR							szBankNo[LEN_BANK_NO];			//���п���
	TCHAR							szBankName[LEN_BANK_NAME];		//��������

	TCHAR							szVerifyCode[LEN_VERIFY_CODE];		//ע�������֤��
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ���
	TCHAR							szBankAddress[LEN_BANK_ADDRESS];	//���е�ַ
	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//�ο͵�¼
struct CMD_MB_LogonVisitor
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szAgentID[LEN_ACCOUNTS];			//�����ʶ
	BYTE                            cbDeviceType;                       //�豸����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
};

//��¼�ɹ�
struct CMD_MB_LogonSuccess
{
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwCustomID;							//�Զ�ͷ��
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwExperience;						//������ֵ
	SCORE							lLoveLiness;						//�û�����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��ʺ�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szDynamicPass[LEN_PASSWORD];		//��̬����
	TCHAR							szIpAddress[LEN_IP];				//IP��ַ(��ʵIP)
	TCHAR							szChangeLogonIP[LEN_IP];			//�޸���ʾ������IP(α��IP)
	TCHAR							szRegisterMobile[LEN_MOBILE_PHONE];	//�ο������ж��Ƿ�����ֻ�
	//�Ƹ���Ϣ
	SCORE							lUserScore;							//�û���Ϸ��
	SCORE							lUserIngot;							//�û�Ԫ��
	SCORE							lUserInsure;						//�û�����	
	DOUBLE							dUserBeans;							//�û���Ϸ��

	//��չ��Ϣ
	BYTE							cbInsureEnabled;					//ʹ�ܱ�ʶ
	BYTE							cbIsAgent;							//�����ʶ
	BYTE							cbMoorMachine;						//��������

	//Լս�����
	SCORE							lRoomCard;							//�û�����
	DWORD						dwLockServerID;						//��������
	DWORD						dwKindID;							//��Ϸ����
	SYSTEMTIME						RegisterData;					//ע��ʱ��
};

//��¼ʧ��
struct CMD_MB_LogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//������ʾ
struct CMD_MB_UpdateNotify
{
	BYTE							cbMustUpdate;						//ǿ������
	BYTE							cbAdviceUpdate;						//��������
	DWORD							dwCurrentVersion;					//��ǰ�汾
};

//////////////////////////////////////////////////////////////////////////////////
//�б�����������
#define MDM_MB_SERVER_LIST			101									//�б���Ϣ

//��ѯ��Ϣ
#define SUB_QUEREY_SERVER			1									//��ѯ�����б�

struct CMD_MB_QueryServer
{
	WORD							wKindID;							
};

//�����Ϣ
#define SUB_MB_LIST_SERVER_START	1									//��ʼ�����б�֪ͨ
#define SUB_MB_LIST_SERVER_END		2									//���������б�֪ͨ

#define SUB_MB_LIST_KIND			100									//�����б�
#define SUB_MB_LIST_SERVER			101									//�����б�
#define SUB_MB_LIST_MATCH			102									//�����б�	
#define SUB_MB_LIST_FINISH			200									//�б����

#define SUB_MB_GET_ONLINE			300									//��ȡ����
#define SUB_MB_KINE_ONLINE			301									//��������
#define SUB_MB_SERVER_ONLINE		302									//��������
#define SUB_MB_AGENT_KIND			400									//�����б�
//////////////////////////////////////////////////////////////////////////////////

//��ȡ����
struct CMD_MB_GetOnline
{
	WORD							wServerCount;						//������Ŀ
	WORD							wOnLineServerID[MAX_SERVER];		//�����ʶ
};

//��������
struct CMD_MB_KindOnline
{
	WORD							wKindCount;							//������Ŀ
	tagOnLineInfoKind				OnLineInfoKind[MAX_KIND];			//��������
};

//��������
struct CMD_MB_ServerOnline
{
	WORD							wServerCount;						//������Ŀ
	tagOnLineInfoServer				OnLineInfoServer[MAX_SERVER];		//��������
};



//////////////////////////////////////////////////////////////////////////////////

//˽�˷���
#define MDM_MB_PERSONAL_SERVICE		200									//˽�˷���

#define SUB_MB_QUERY_GAME_SERVER	204									//��ѯ����
#define SUB_MB_QUERY_GAME_SERVER_RESULT	205								//��ѯ���
#define SUB_MB_SEARCH_SERVER_TABLE	206									//������������
#define SUB_MB_SEARCH_RESULT		207									//�������
#define SUB_MB_GET_PERSONAL_PARAMETER	208								//˽�˷�������
#define SUB_MB_PERSONAL_PARAMETER	209									//˽�˷�������
#define SUB_MB_QUERY_PERSONAL_ROOM_LIST	210									//����˽�˷����б�
#define SUB_MB_QUERY_PERSONAL_ROOM_LIST_RESULT 	211									//����˽�˷����б�
#define SUB_MB_PERSONAL_FEE_PARAMETER	212									//˽�˷�������
#define SUB_MB_DISSUME_SEARCH_SERVER_TABLE	213									//Ϊ��ɢ��������ID
#define SUB_MB_DISSUME_SEARCH_RESULT	214									//��ɢ������������ID���
#define SUB_MB_QUERY_USER_ROOM_INFO	215									//�������������Ϣ
#define SUB_GR_USER_QUERY_ROOM_SCORE	216							//˽�˷��䵥��������󷿼�ɼ�
#define SUB_GR_USER_QUERY_ROOM_SCORE_RESULT	217							//˽�˷��䵥��������󷿼�ɼ����
#define SUB_GR_USER_QUERY_ROOM_SCORE_RESULT_FINSIH	218				//˽�˷��䵥��������󷿼�ɼ����
#define SUB_MB_QUERY_PERSONAL_ROOM_USER_INFO			219		//˽�˷�������ҵķ�������Ϸ��
#define SUB_MB_QUERY_PERSONAL_ROOM_USER_INFO_RESULT			220		//˽�˷�������ҵķ�������Ϸ�����
#define SUB_MB_ROOM_CARD_EXCHANGE_TO_SCORE			221		//�����һ���Ϸ��
#define SUB_GP_EXCHANGE_ROOM_CARD_RESULT			222		//�����һ���Ϸ�ҽ��

//˽�˷�������
struct CMD_MB_PersonalTableConfig
{
	DWORD							dwCount;							//��������
	tagPersonalTableParameter		PersonalTableParameter[50];			//������Ϣ
};

//���ҷ���
struct CMD_MB_QueryGameServer
{
	DWORD							dwUserID;							//�û�I D
	DWORD							dwKindID;							//��ϷI D
	BYTE									cbIsJoinGame;						//�Ƿ������Ϸ
};

//��ѯ���
struct CMD_MB_QueryGameServerResult
{
	DWORD							dwServerID;							//����I D
	bool									bCanCreateRoom;				//�Ƿ���Դ�������

	//��������
	TCHAR								szErrDescrybe[MAX_PATH];						//��������
};

//�������˷���
struct CMD_MB_SearchServerTable
{
	TCHAR							szServerID[7];						//������
	DWORD						dwKindID;			//��������
};

//�������˷���
struct CMD_MB_DissumeSearchServerTable
{
	TCHAR							szServerID[7];						//������
};

//�������
struct CMD_MB_SearchResult
{
	DWORD							dwServerID;							//���� I D
	DWORD							dwTableID;							//���� I D
};

//��ɢʱ�������
struct CMD_MB_DissumeSearchResult
{
	DWORD							dwServerID;							//���� I D
	DWORD							dwTableID;							//���� I D
};

//˽�˷������
struct CMD_MB_GetPersonalParameter
{
	DWORD							dwServerID;							//��Ϸ��ʶ
};

//˽�˷����б���Ϣ
struct CMD_MB_PersonalRoomInfoList
{
	DWORD							dwUserID;																		//��������
	tagPersonalRoomInfo		PersonalRoomInfo[MAX_CREATE_SHOW_ROOM];			//������Ϣ
};

//˽�˷����û���Ϣ
struct CMD_MB_PersonalRoomUserInfo
{
	SCORE			lRoomCard;		//��������
	DOUBLE		dBeans;			//��Ϸ��
};

//˽�˷����û���Ϣ
struct CMD_MB_QueryPersonalRoomUserInfo
{
	DWORD dwUserID;
};

//��ѯ������Ϣ���
struct CMD_MB_QueryGameLockInfoResult
{
	DWORD						dwKindID;
	DWORD						dwServerID;
};


//�����һ���Ϸ��
struct CMD_GP_ExchangeScoreByRoomCard
{
	DWORD						dwUserID;										//�û���ʶ
	SCORE							lRoomCard;										//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//��ѯ�û������н��
struct CMD_GP_QueryBankInfoResult
{
	TCHAR							szRealName[LEN_REAL_NAME];			//��ʵ����
	TCHAR							szBankNo[LEN_BANK_NO];				//���п���
	SCORE							lDrawScore;							//�����ֽ��
	TCHAR							szAlipayID[LEN_BANK_ADDRESS];		//֧����
};

//������
struct CMD_GP_BindBankInfo
{											
	DWORD dwUserID;							//�û�id
	TCHAR szRealName[LEN_REAL_NAME];		//��ʵ����
	TCHAR szBankNo[LEN_BANK_NO];			//���п���
	TCHAR szBankName[LEN_BANK_NAME];		//��������
	TCHAR szBankAddress[LEN_BANK_ADDRESS];	//��������
	TCHAR szAlipayID[LEN_BANK_ADDRESS];	//֧����
};

//�һ����
struct CMD_GP_ExchangeBankScore
{											
	DWORD dwUserID;							//�û�id
	SCORE dwScore;		//��ʵ����
	TCHAR szBankPass[LEN_PASSWORD];			//���п���
	TCHAR szOrderID[LEN_ORDER_ID];		//��������
	DWORD dwType;							//0 bank 1 alipay
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif