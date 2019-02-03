#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//�������ݰ�

//////////////////////////////////////////////////////////////////////////////////

//��¼����
#define LOGON_COMMAND_USERINFO      0x0001								//�û���Ϣ
#define LOGON_COMMAND_GROUPINFO		0x0002								//������Ϣ
#define LOGON_COMMAND_FACE			0x0004								//�û�ͷ��
#define LOGON_COMMAND_FRIENDS       0x0008								//�û�����
#define LOGON_COMMAND_REMARKS		0x0010								//��ע��Ϣ

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//�������ݰ�

//�û��¼�
#define	DBR_GR_LOGON_USERID			100									//I D ��¼
#define	DBR_GR_LOGON_MOBILE			101									//�ֻ���¼
#define	DBR_GR_LOGON_ACCOUNTS		102									//�˺ŵ�¼

//�û�����
#define DBR_GR_MODIFY_LOGONPASS		200									//�޸�����
#define DBR_GR_MODIFY_UNDER_WRITE	201									//�޸�ǩ��
#define DBR_GR_MODIFY_INDIVIDUAL	202									//�޸�����
#define DBR_GR_QUERY_INDIVIDUAL		203									//��ѯ����
#define DBR_GR_MODIFY_FRIEND		204									//�޸ĺ���
#define DBR_GR_MODIFY_GROUP			205									//�޸ķ���
#define DBR_GR_MODIFY_REMARKS		206									//�޸ı�ע	
#define DBR_GR_SEARCH_USER			207									//��������
#define DBR_GR_ADD_FRIEND			208									//��Ӻ���
#define DBR_GR_SYSTEM_FACEINFO		209									//ϵͳͷ��
#define DBR_GR_CUSTOM_FACEINFO		210									//�Զ�ͷ��
#define DBR_GR_CUSTOM_FACEDELETE	211									//ɾ��ͷ��
#define DBR_GR_SAVE_OFFLINEMESSAGE  212									//������Ϣ
#define DBR_GR_QUERY_FRIENDWEALTH	213									//���ѲƸ�	
#define DBR_GR_TRUMPET				214									//������Ϣ
#define DBR_GR_DELETE_FRIEND		215									//ɾ������


//////////////////////////////////////////////////////////////////////////////////
//�����Ϣ

//�߼��¼�
#define DBO_GR_LOGON_SUCCESS		100									//��¼�ɹ�
#define DBO_GR_LOGON_FAILURE		101									//��¼ʧ��
#define DBO_GR_ACCOUNT_LOGON_FAILURE		102							//��¼ʧ��

//�û��¼�
#define DBO_GR_USER_GROUPINFO		200									//������Ϣ
#define DBO_GR_USER_FRIENDINFO		201									//������Ϣ
#define DBO_GR_USER_REMARKS			202									//���ѱ�ע
#define DBO_GR_USER_SINDIVIDUAL		203									//��������
#define DBO_GR_LOAD_OFFLINEMESSAGE  204									//��ȡ��Ϣ	

//�����¼�
#define DBO_GR_SERVER_PARAMETER		300									//������Ϣ

//�û�����
#define DBO_GR_MODIFY_LOGONPASS		400									//�޸�����
#define DBO_GR_MODIFY_UNDERWRITE	401									//�޸�ǩ��
#define DBO_GR_MODIFY_INDIVIDUAL	402									//�޸�����
#define DBO_GR_QUERY_INDIVIDUAL		403									//��ѯ����
#define DBO_GR_MODIFY_GROUP			404									//�޸ķ���
#define DBO_GR_MODIFY_FRIEND		405									//�޸ĺ���
#define DBO_GR_MODIFY_REMARKS		406									//�޸ı�ע
#define DBO_GR_ADD_FRIEND			407									//��Ӻ���
#define DBO_GR_SEARCH_USER_RESULT	408									//�������
#define DBO_GR_OPERATE_RESULT		409									//�������
#define DBO_GR_FRIEND_WEALTH		410									//���ѲƸ�	
#define DBO_GR_TRUMPET_RESULT		411									//���߳ɹ�
#define DBO_GR_DELETE_FRIEND		412									//ɾ������
//////////////////////////////////////////////////////////////////////////////////

//ID ��¼
struct DBR_GR_LogonUserID
{		
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//�ֻ��ͺ�
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������
	BYTE							wLogonComand;						//��¼����
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//�˺� ��¼
struct DBR_GR_LogonUserAccounts
{		
	//��¼��Ϣ
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û� �˺�
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//�ֻ��ͺ�
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������
	BYTE							wLogonComand;						//��¼����
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//ID ��¼
struct DBR_GR_LogonMobile
{		
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	BYTE							wLogonComand;						//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	WORD							wClientPort;						//���Ӷ˿�
};

//������Ϣ
struct DBR_GR_SaveOfflineMessage
{
	DWORD							dwUserID;							//�û���ʶ
	WORD							wMessageType;						//��Ϣ����
	WORD							wDataSize;							//���ݴ�С
	TCHAR							szOfflineData[2400];				//��Ϣ����
};

//////////////////////////////////////////////////////////////////////////////////

//��¼�ɹ�
struct DBO_GR_LogonSuccess
{
	//�û�����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��˺�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	DWORD							dwFaceID;							//ͷ�� I D
	DWORD							dwCustomID;							//ͷ�� I D
	BYTE							cbGender;							//�û��Ա�
	WORD							wMemberOrder;						//�û��ȼ�
	WORD							wGrowLevel;							//�û��ȼ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//�ֻ��ͺ�
	DWORD							dwClientAddr;						//�û���ַ

	//��������
	TCHAR							szQQ[LEN_QQ];						//�û�QQ
	TCHAR							szEMail[LEN_EMAIL];					//�û�Eamil
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����	
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//��ϵ��ַ
	TCHAR							szPostalCode[LEN_POSTALCODE];		//��������

	//״̬
	TCHAR							szMachineID[LEN_MACHINE_ID];			//������
};

//��¼ʧ��
struct DBO_GR_LogonFailure
{
	LONG							lResultCode;						//�������
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��¼ʧ��
struct DBO_GR_AccountLogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�û�����
struct DBO_GR_UserGroupInfo
{
	WORD							wGroupCount;						//������Ŀ
	tagClientGroupInfo				GroupInfo[GROUP_MAX_COUNT];			//������Ϣ
};

//������Ϣ
struct DBO_GR_UserFriendInfo
{
	bool							bLogonFlag;							//��¼��ʶ
	DWORD							dwUserID;							//�û���ʶ
	WORD							wFriendCount;						//������Ŀ
	tagClientFriendInfo				FriendInfo[MAX_FRIEND_COUNT];		//������Ϣ
};

//��ע��Ϣ
struct DBO_GR_UserRemarksInfo
{
	DWORD							dwUserID;							//�û���ʶ
	WORD							wFriendCount;						//�û���Ŀ
	tagUserRemarksInfo				RemarksInfo[MAX_FRIEND_COUNT];		//��ע��Ϣ
};

//��������
struct DBO_GR_UserIndividual
{
	DWORD							dwUserID;							//�û���ʶ
	WORD							wUserCount;							//�û���Ŀ
	tagUserIndividual				UserIndividual[MAX_FRIEND_COUNT];	//��������
};

//������Ϣ
struct DBO_GR_ServerParameter
{
	//�汾��Ϣ
	DWORD							dwClientVersion;					//�ͻ��汾
};




//�޸ĺ���
struct DBO_GR_ModifyGroup
{
	DWORD							dwUserID;							//�û���ʶ
	bool							bSuccessed;							//�ɹ���ʶ
	BYTE							cbOperateKind;						//��������
	BYTE							cbGroupID;							//����ʶ
	TCHAR							szGroupName[GROUP_LEN_NAME];		//��������	
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//��ʾ��Ϣ
};

//�޸ĺ���
struct DBO_GR_ModifyFriend
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwFriendUserID;						//�û���ʶ	
	bool							bSuccessed;							//�ɹ���ʶ
	BYTE							cbOperateKind;						//��������
	BYTE							cbGroupID;							//����ʶ
};

//�޸ı�ע
struct DBO_GR_ModifyRemarks
{
	bool							bSuccessed;							//�ɹ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwFriendID;							//���ѱ�ʶ
	TCHAR							szUserRemarks[LEN_REMARKS];			//���ѱ�ע
};

//��Ӻ���
struct DBO_GR_AddFriend
{
	bool							bLoadUserInfo;						//������Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwRequestUserID;					//�û���ʶ	
	BYTE							cbGroupID;							//����ʶ
	BYTE							cbRequestGroupID;					//����ʶ
};

//�����û�
struct DBO_GR_SearchUserResult
{
	BYTE							cbUserCount;						//�û���Ŀ
	tagClientFriendInfo				FriendInfo;							//������Ϣ
};



//�������
struct DBO_GR_OperateResult
{
	bool							bModifySucesss;						//�ɹ���ʶ
	DWORD							dwErrorCode;						//�������
	WORD							wOperateCode;						//��������
	TCHAR							szDescribeString[128];				//��ʾ��Ϣ
};

//������Ϣ
struct DBO_GR_UserOfflineMessage
{
	WORD							wMessageType;						//��Ϣ����
	WORD							wDataSize;							//���ݴ�С
	TCHAR							szOfflineData[2400];				//��Ϣ����
};

//////////////////////////////////////////////////////////////////////////////////



//�޸ĺ���
struct DBR_GR_ModifyFriend
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwFriendUserID;						//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	BYTE							cbOperateKind;						//��������
	BYTE							cbGroupID;							//����ʶ
};

//��Ӻ���
struct DBR_GR_AddFriend
{
	bool							bLoadUserInfo;						//������Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwRequestUserID;					//�û���ʶ	
	BYTE							cbGroupID;							//����ʶ
	BYTE							cbRequestGroupID;					//����ʶ
};

//ɾ������
struct DBR_GR_DeleteFriend
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwFriendUserID;						//�û���ʶ	
	BYTE							cbGroupID;							//����ʶ
};

//ɾ������
struct DBO_GR_DeleteFriend
{
	bool							bSuccessed;							//�ɹ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwFriendUserID;						//�û���ʶ	
	BYTE							cbGroupID;							//����ʶ
};

//�����û�
struct DBR_GR_SearchUser
{
	DWORD							dwSearchByGameID;					//�û���ʶ
};

//�޸ķ���
struct DBR_GR_ModifyGroup
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	BYTE							cbOperateKind;						//��������
	BYTE							cbGroupID;							//����ʶ
	TCHAR							szGroupName[GROUP_LEN_NAME];		//�������
};

//�޸ı�ע
struct DBR_GR_ModifyRemarks
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwFriendID;							//���ѱ�ʶ
	TCHAR							szUserRemarks[LEN_REMARKS];			//���ѱ�ע
};

////����
//struct DBR_GR_Trumpet
//{
//	DWORD							dwClientAddr;							//���ӵ�ַ
//	DWORD							dwSourceUserID;							//�������
//	DWORD							dwFontColor;							//������ɫ
//	BYTE							cbFontSize;								//�����С
//	BYTE							cbFontAttri;							//��������
//	TCHAR							szNickName[LEN_ACCOUNTS];				//�û��ǳ�
//	TCHAR							szFontName[LEN_FONT_NAME];				//��������
//	TCHAR							szMessageContent[LEN_MESSAGE_CONTENT];  //��Ϣ����
//};

//struct DBO_GR_TrumpetResult
//{
//	LONG							lResult;
//	DWORD							dwSourceUserID;							//�������
//	DWORD							dwFontColor;							//������ɫ
//	BYTE							cbFontSize;								//�����С
//	BYTE							cbFontAttri;							//��������
//	TCHAR							szNickName[LEN_ACCOUNTS];				//�û��ǳ�
//	TCHAR							szFontName[LEN_FONT_NAME];				//��������
//	TCHAR							szMessageContent[LEN_MESSAGE_CONTENT];  //��Ϣ����
//	TCHAR							szNotifyContent[128];					//������Ϣ
//};

//����
struct DBR_GR_Trumpet
{
	DWORD						   dwClientAddr;						//���ӵ�ַ
	WORD                           wPropertyID;                         //�������� 
	DWORD                          dwSendUserID;                        //�û� I D
	DWORD                          TrumpetColor;                        //������ɫ
	TCHAR                          szSendNickName[32];				    //����ǳ� 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //��������
};

struct DBO_GR_TrumpetResult
{
	LONG						   lResult;
	WORD                           wPropertyID;                         //�������� 
	DWORD                          dwSendUserID;                        //�û� I D
	DWORD                          TrumpetColor;                        //������ɫ
	TCHAR                          szSendNickName[32];				    //����ǳ� 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //��������
	TCHAR						   szNotifyContent[128];					//������Ϣ
};


//////////////////////////////////////////////////////////////////////////////////

#endif