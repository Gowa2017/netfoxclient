#ifndef CMD_CHAT_SERVER_HEAD_FILE
#define CMD_CHAT_SERVER_HEAD_FILE

#pragma pack(1)

#define CHAT_MSG_OK						0								//��Ϣ�ɹ�
#define CHAT_MSG_ERR					1								//��Ϣʧ��

//////////////////////////////////////////////////////////////////////////////////
//��¼����
#define MDM_GC_LOGON					1								//��¼��Ϣ

#define SUB_GC_PC_LOGON_USERID   		100								//PC ��¼
#define SUB_GC_MB_LOGON_USERID			101								//�ֻ���¼
#define SUB_GC_MB_LOGON_ACCOUNTS		102								//�˺ŵ�¼

//��¼���
#define SUB_GC_LOGON_SUCCESS			200								//��¼�ɹ�
#define SUB_GC_LOGON_FAILURE			210								//��¼ʧ��

//�û���Ϣ
#define SUB_S_USER_GROUP				302								//�û�����
#define SUB_S_USER_FRIEND				303								//�û�����
#define SUB_S_USER_REMARKS				304								//�û���ע	
#define SUB_S_USER_SINDIVIDUAL			305								//��������			
#define SUB_S_LOGON_AFRESH				306								//�û�����
#define SUB_S_LOGON_FINISH				307								//��¼���


//////////////////////////////////////////////////////////////////////////////////

//�û���¼
struct CMD_GC_MB_LogonByUserID
{
	//��¼��Ϣ
	DWORD							dwUserID;							//��ұ�ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����	
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//�ֻ��ͺ�
	TCHAR							szMachineID[LEN_MACHINE_ID];			//������
};

//�û���¼
struct CMD_GC_MB_LogonByAccounts
{
	//��¼��Ϣ
	TCHAR							szAccounts[LEN_ACCOUNTS];			//����˺�
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����	
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//�ֻ��ͺ�
	TCHAR							szMachineID[LEN_MACHINE_ID];			//������
};

//��¼����
struct CMD_GC_LogonEcho
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�𴦵�¼
struct CMD_GC_UserLogonAfresh
{
	TCHAR							wNotifyMessage[128];				//������Ϣ
};

//�û���Ϣ
struct CMD_GC_UserFriendInfo
{
	WORD							wFriendCount;						//������Ŀ
	tagClientFriendInfo				FriendInfo[MAX_FRIEND_TRANSFER];	//������Ϣ
};

//Я����Ϣ
#define DTP_GP_UI_QQ						1								 //QQ
#define DTP_GP_UI_EMAIL						2								 //EMail
#define DTP_GP_UI_SEATPHOHE					3								 //��������
#define DTP_GP_UI_MOBILEPHONE				4								 //�ֻ�����
#define DTP_GP_UI_DWELLINGPLACE				5								 //��ϵ��ַ
#define DTP_GP_UI_POSTALCODE				6								 //��������	
#define DTP_GP_UI_NICKNAME					7								 //�û��ǳ�
#define DTP_GP_UI_UNDERWRITE				8								 //����ǩ��
#define DTP_GP_UI_COMPELLATION				9								 //��ʵ����
#define DTP_GP_UI_REMARKS					10								 //�û���ע

//��������
struct CMD_GC_UserIndividual
{
	//������Ϣ
	DWORD								dwUserID;							//�û���ʶ
};

//�û�����
#define MDM_GC_USER						3									//�û���Ϣ
//////////////////////////////////////////////////////////////////////////////////
#define	SUB_GC_USER_STATUS_NOTIFY		1									//�û�״̬
#define SUB_GC_GAME_STATUS_NOTIFY		2									//��Ϸ��̬

#define SUB_GC_USER_CHAT				100									//�û�����	
#define SUB_GC_APPLYFOR_FRIEND   		101									//�������
#define SUB_GC_RESPOND_FRIEND			102									//���ѻ�Ӧ
#define SUB_GC_SEARCH_USER				103									//�����û�
#define SUB_GC_INVITE_GAME				104									//������Ϸ
#define SUB_GC_TRUMPET					105									//�û�����
#define SUB_GC_DELETE_FRIEND			106									//ɾ������
#define SUB_GC_MODIFY_FRIEND			107									//�޸ĺ���
#define SUB_GC_MODIFY_GROUP				108									//�޸ķ���
#define SUB_GC_UPDATE_COORDINATE		109									//��������
#define SUB_GC_GET_NEARUSER				110									//��������
#define SUB_GC_QUERY_NEARUSER			111									//��ѯ����
#define SUB_GC_USER_SHARE				112									//������Ϣ
#define SUB_GC_INVITE_PERSONAL			113									//������Ϸ
#define SUB_GC_FORCE_OFFLINE			114									//�û�ǿ������

#define SUB_GC_USER_CHAT_NOTIFY			200									//����֪ͨ
#define SUB_GC_APPLYFOR_NOTIFY			201									//����֪ͨ
#define SUB_GC_RESPOND_NOTIFY			202									//��Ӧ֪ͨ
#define SUB_GC_SEARCH_USER_RESULT		203									//���ҽ��
#define SUB_GC_INVITE_GAME_NOTIFY		204									//����֪ͨ
#define SUB_GC_TRUMPET_NOTIFY			205									//����֪ͨ
#define SUB_GC_DELETE_FRIEND_NOTIFY		206									//ɾ��֪ͨ
#define SUB_GC_MODIFY_FRIEND_NOTIFY		207									//�޸Ľ��
#define SUB_GC_MODIFY_GROUP_NOTIFY		208									//�޸Ľ��
#define SUB_GC_UPDATE_COORDINATE_NOTIFY	209									//��������
#define SUB_GC_GET_NEARUSER_RESULT		210									//�������
#define SUB_GC_QUERY_NEARUSER_RESULT	211									//��ѯ���
#define SUB_GC_USER_SHARE_NOTIFY		212									//����֪ͨ
#define SUB_GC_INVITE_PERSONAL_NOTIFY	213									//����֪ͨ
#define SUB_GC_HALL_NOTIFY				214									//����֪ͨ

#define SUB_GC_USER_CHAT_ECHO			300									//���췴��
#define SUB_GC_APPLYFOR_FRIEND_ECHO		301									//���뷴��
#define SUB_GC_RESPOND_FRIEND_ECHO		302									//��Ӧ����
#define SUB_GC_SEARCH_USER_ECHO			303									//���ҷ���
#define SUB_GC_INVITE_GAME_ECHO			304									//���뷴��
#define SUB_GC_TRUMPET_ECHO				305									//���ȷ���
#define SUB_GC_DELETE_FRIEND_ECHO		306									//ɾ������
#define SUB_GC_MODIFY_FRIEND_ECHO		307									//�޸ķ���
#define SUB_GC_MODIFY_GROUP_ECHO		308									//�޸ķ���
#define SUB_GC_UPDATE_COORDINATE_ECHO	309									//��������
#define SUB_GC_GET_NEARUSER_ECHO		310									//��������
#define SUB_GC_QUERY_NEARUSER_ECHO		311									//��ѯ����
#define SUB_GC_USER_SHARE_ECHO			312									//������
#define SUB_GC_INVITE_PERSONAL_ECHO		313									//���뷴��

#define SUB_GC_SYSTEM_MESSAGE			401									//ϵͳ��Ϣ

//�������
#define SUB_GC_OPERATE_SUCCESS			800									//�����ɹ�
#define SUB_GP_OPERATE_FAILURE			801									//����ʧ��
//////////////////////////////////////////////////////////////////////////

//�û�����/����
struct CMD_GC_UserOnlineStatusNotify
{
	DWORD								dwUserID;							//�û� I D
	BYTE								cbMainStatus;						//�û�״̬
};

//��Ϸ״̬
struct CMD_GC_UserGameStatusNotify
{
	DWORD								dwUserID;							//�û���ʶ
	BYTE								cbGameStatus;						//��Ϸ״̬
	WORD								wKindID;							//��Ϸ��ʶ
	WORD								wServerID;							//�����ʶ
	WORD								wTableID;							//���ӱ�ʶ
	TCHAR								szServerName[LEN_SERVER];			//��������
};


//�û�����
struct CMD_GC_UserChat
{
	DWORD								dwSenderID;							//�û���ʶ
	DWORD								dwTargetUserID;						//Ŀ���û�
	DWORD								dwFontColor;						//������ɫ
	BYTE								cbFontSize;							//�����С
	BYTE								cbFontAttri;						//��������
	TCHAR								szFontName[LEN_FONT_NAME];			//��������
	TCHAR								szMessageContent[LEN_MESSAGE_CONTENT]; //��Ϣ����	
};

//�������
struct CMD_GC_UserChatEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};


//�������
struct CMD_GC_ApplyForFriend
{
	DWORD								dwUserID;							//�û���ʶ
	DWORD								dwFriendID;							//���ѱ�ʶ
	BYTE								cbGroupID;							//����ʶ
};

//�������
struct CMD_GC_ApplyForFriendEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//����֪ͨ
struct CMD_GC_ApplyForNotify
{
	DWORD								dwRequestID;						//�û���ʶ
	BYTE								cbGroupID;							//����ʶ
	TCHAR								szNickName[LEN_ACCOUNTS];			//�û��ǳ�
};

//��Ӧ����
struct CMD_GC_RespondFriend
{
	DWORD								dwUserID;							//�û���ʶ
	DWORD								dwRequestID;						//�û���ʶ
	BYTE								cbRequestGroupID;					//����ʶ
	BYTE								cbGroupID;							//����ʶ
	bool								bAccepted;							//�Ƿ����
};

//��Ӧ����
struct CMD_GC_RespondFriendEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//��Ӧ֪ͨ
struct CMD_GC_RespondNotify
{
	TCHAR								szNotifyContent[128];				//��ʾ����
};

//���Һ���
struct CMD_GC_SearchByGameID
{
	DWORD								dwSearchByGameID;					//�û���ʶ
};

//���һ���
struct CMD_GC_SearchByGameIDEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//���ҽ��
struct CMD_GC_SearchByGameIDResult
{
	BYTE								cbUserCount;						//�û���Ŀ
	tagClientFriendInfo					FriendInfo;							//������Ϣ
};


//������Ϸ
struct CMD_GC_InviteGame
{
	DWORD								dwUserID;							//�����û�	
	DWORD								dwInvitedUserID;					//�����û�
	WORD								wKindID;							//��Ϸ��ʶ
	WORD								wServerID;							//�����ʶ
	WORD								wTableID;							//���ӱ�ʶ
	TCHAR								szInviteMsg[128];					//������Ϣ
};

//���뷴��
struct CMD_GC_InviteGameEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};


//����֪ͨ
struct CMD_GC_InviteGameNotify
{
	DWORD								dwInviteUserID;						//�����û�
	WORD								wKindID;							//��Ϸ��ʶ
	WORD								wServerID;							//�����ʶ
	WORD								wTableID;							//���ӱ�ʶ
	TCHAR								szInviteMsg[128];					//������Ϣ
};

////�û�����
//struct CMD_GC_Trumpet
//{
//	DWORD								dwSenderID;							//�û���ʶ
//	TCHAR								szNickName[LEN_ACCOUNTS];			//�û��ǳ�
//	DWORD								dwFontColor;						//������ɫ
//	BYTE								cbFontSize;							//�����С
//	BYTE								cbFontAttri;						//��������
//	TCHAR								szFontName[LEN_FONT_NAME];			//��������
//	TCHAR								szMessageContent[LEN_MESSAGE_CONTENT]; //��Ϣ����	
//};

//�û�����
struct CMD_GC_Trumpet
{
	WORD                           wPropertyID;                         //�������� 
	DWORD                          dwSendUserID;                        //�û� I D
	DWORD                          TrumpetColor;                        //������ɫ
	TCHAR                          szSendNickName[32];				    //����ǳ� 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //��������
};

//���ȷ���
struct CMD_GC_TrumpetEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//ɾ������
struct CMD_GC_DeleteFriend
{
	DWORD								dwUserID;							//�û���ʶ
	DWORD								dwFriendUserID;						//�û���ʶ	
	BYTE								cbGroupID;							//����ʶ
};

//ɾ������
struct CMD_GC_DeleteFriendEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//ɾ�����
struct CMD_GC_DeleteFriendNotify
{
	DWORD								dwFriendUserID;						//�û���ʶ	
};

//�޸ĺ���
struct CMD_GC_ModifyFriend
{
	DWORD								dwUserID;							//�û���ʶ
	DWORD								dwFriendUserID;						//�û���ʶ	
	BYTE								cbGroupID;							//����ʶ
};
//�޸ķ���
struct CMD_GC_ModifyFriendEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//�޸ĺ���
struct CMD_GC_ModifyFriendResult
{
	bool								bSuccessed;							//�ɹ���ʶ
	DWORD								dwFriendUserID;						//�û���ʶ	
	BYTE								cbGroupID;							//����ʶ
};

//�޸ķ���
struct CMD_GC_ModifyGroup
{
	DWORD								dwUserID;							//�û���ʶ
	BYTE								cbGroupID;							//����ʶ
	TCHAR								szGroupName[GROUP_LEN_NAME];		//�������
};

//�޸ķ���
struct CMD_GC_ModifyGroupEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//�޸Ľ��
struct CMD_GC_ModifyGroupResult
{
	BYTE								cbGroupID;							//����ʶ
	TCHAR								szGroupName[GROUP_LEN_NAME];		//�������
};

//��������
struct CMD_GC_Update_Coordinate
{
	DWORD								dwUserID;							//�û���ʶ
	DOUBLE								dLongitude;							//���꾭��
	DOUBLE								dLatitude;							//����γ��
};

//���귴��
struct CMD_GC_Update_CoordinateEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//����֪ͨ
struct CMD_GC_Update_CoordinateNotify
{
	DOUBLE								dLongitude;							//���꾭��
	DOUBLE								dLatitude;							//����γ��
	BYTE								cbCoordinate;						//������ʶ
	DWORD								dwClientAddr;						//�û���ַ
};


//��ȡ����
struct CMD_GC_Get_Nearuser
{
	DWORD								dwUserID;							//�û���ʶ
	DOUBLE								dLongitude;							//���꾭��
	DOUBLE								dLatitude;							//����γ��
};

//��������
struct CMD_GC_Get_NearuserEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//�������
struct CMD_GC_Get_NearuserResult
{
	BYTE								cbUserCount;						//�û���Ŀ
	tagNearUserInfo						NearUserInfo[MAX_FRIEND_TRANSFER];	//������Ϣ
};

//��ѯ����
struct CMD_GC_Query_Nearuser
{
	DWORD								dwUserID;							//�û���ʶ
	DWORD								dwNearuserUserID;					//�����û�
};

//��ѯ����
struct CMD_GC_Query_NearuserEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//��ѯ���
struct CMD_GC_Query_NearuserResult
{
	BYTE								cbUserCount;						//�û���Ŀ
	tagNearUserInfo						NearUserInfo;						//������Ϣ
};


//ϵͳ��Ϣ
struct CMD_GC_S_SystemMessage
{
	WORD								wType;								//��Ϣ����
	WORD								wLength;							//��Ϣ����
	TCHAR								szString[1024];						//��Ϣ����
};

//������Ϣ
struct CMD_GC_UserShare
{
	DWORD								dwUserID;								//�û���ʶ
	DWORD								dwSharedUserID;							//�������û�	
	TCHAR								szShareImageAddr[128];					//ͼƬ��ַ
	TCHAR								szMessageContent[128];				    //��Ϣ����
};

//������
struct CMD_GC_UserShareEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//����֪ͨ
struct CMD_GC_UserShareNotify
{
	DWORD								dwShareUserID;							//�û���ʶ
	TCHAR								szShareImageAddr[128];					//ͼƬ��ַ
	TCHAR								szMessageContent[128];				    //��Ϣ����
};

//������Ϸ
struct CMD_GC_InvitePersonalGame
{
	DWORD								dwUserID;							//�����û�	
	DWORD								dwInvitedUserID;					//�����û�
	WORD								wKindID;							//��Ϸ��ʶ
	DWORD								dwServerNumber;						//�������
	WORD								wTableID;							//���ӱ�ʶ
	TCHAR								szInviteMsg[128];					//������Ϣ
};

//���뷴��
struct CMD_GC_InvitePersonalGameEcho
{
	LONG								lErrorCode;							//�������
	TCHAR								szDescribeString[128];				//������Ϣ
};


//����֪ͨ
struct CMD_GC_InvitePersonalGameNotify
{
	DWORD								dwInviteUserID;						//�����û�
	WORD								wKindID;							//��Ϸ��ʶ
	DWORD								dwServerNumber;						//�������
	WORD								wTableID;							//���ӱ�ʶ
	TCHAR								szInviteMsg[128];					//������Ϣ
};


//����ʧ��
struct CMD_GC_OperateFailure
{
	LONG								lErrorCode;							//�������
	LONG								lResultCode;						//��������
	TCHAR								szDescribeString[128];				//������Ϣ
};

//�����ɹ�
struct CMD_GC_OperateSuccess
{
	LONG								lResultCode;						//��������
	TCHAR								szDescribeString[128];				//�ɹ���Ϣ
};


//////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GC_CONFIG				2									//��¼��Ϣ

#define SUB_GC_CONFIG_SERVER		1									//���ò���

struct CMD_GC_ConfigServer
{
	BYTE							cbMinOrder;							//��ͼ���
	WORD							wMaxPlayer;							//�������
	WORD							wServerPort;						//����˿�
	DWORD							dwServerAddr;						//�����ַ
	DWORD							dwServiceRule;						//�������
};


//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif