#ifndef CMD_CORRESPOND_HEAD_FILE
#define CMD_CORRESPOND_HEAD_FILE

#pragma pack(1)

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"

//////////////////////////////////////////////////////////////////////////////////
//ע������

#define MDM_CS_REGISTER				1									//����ע��

//����ע��
#define SUB_CS_C_REGISTER_PLAZA		100									//ע��㳡
#define SUB_CS_C_REGISTER_SERVER	101									//ע�᷿��
#define SUB_CS_C_REGISTER_MATCH		102									//ע�����
#define SUB_CS_C_REGISTER_CHAT		103									//ע������	
#define SUB_CS_C_REGISTER_PERSONAL		104								//ע��Լս��������	
//ע����
#define SUB_CS_S_REGISTER_FAILURE	200									//ע��ʧ��

//////////////////////////////////////////////////////////////////////////////////

//ע��㳡
struct CMD_CS_C_RegisterPlaza
{
	TCHAR							szServerAddr[32];					//�����ַ
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//ע����Ϸ
struct CMD_CS_C_RegisterServer
{
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wServerID;							//��������
	WORD                            wServerKind;                        //�������
	WORD                            wServerType;                        //��������		
	WORD							wServerLevel;						//����ȼ�
	WORD							wServerPort;						//����˿�
	SCORE							lCellScore;							//��Ԫ����
	SCORE							lEnterScore;						//�������
	BYTE							cbEnterMember;						//�����Ա
	DWORD							dwOnLineCount;						//��������
	DWORD							dwFullCount;						//��Ա����
	WORD							wTableCount;						//������Ŀ
	DWORD							dwServerRule;						//�������
	TCHAR							szServerAddr[32];					//�����ַ
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//ע��ʧ��
struct CMD_CS_S_RegisterFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//ע�����
struct CMD_CS_C_RegisterMatch
{
	//������Ϣ
	WORD							wServerID;							//�����ʶ
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ	
	LONGLONG						lMatchNo;							//��������
	BYTE							cbMatchStatus;						//����״̬
	BYTE							cbMatchType;						//��������
	WCHAR							szMatchName[32];					//��������

	//������Ϣ
	BYTE							cbFeeType;							//��������
	BYTE							cbDeductArea;						//�ɷ�����
	LONGLONG						lSignupFee;							//��������	
	BYTE							cbSignupMode;						//������ʽ
	BYTE							cbJoinCondition;					//��������
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	DWORD							dwExperience;						//��Ҿ���
	DWORD							dwFromMatchID;						//������ʶ		
	BYTE							cbFilterType;						//ɸѡ��ʽ
	WORD							wMaxRankID;							//�������
	SYSTEMTIME						MatchEndDate;						//��������
	SYSTEMTIME						MatchStartDate;						//��ʼ����	

	//������ʽ
	BYTE							cbRankingMode;						//������ʽ	
	WORD							wCountInnings;						//ͳ�ƾ���
	BYTE							cbFilterGradesMode;					//ɸѡ��ʽ

	//��������
	BYTE							cbDistributeRule;					//�������
	WORD							wMinDistributeUser;					//��������
	WORD							wDistributeTimeSpace;				//������	
	WORD							wMinPartakeGameUser;				//��������
	WORD							wMaxPartakeGameUser;				//�������

	//��������
	BYTE							cbMatchRule[512];					//��������

	//������Ϣ
	WORD							wRewardCount;						//��������
	tagMatchRewardInfo				MatchRewardInfo[3];					//��������
};

//Լս��������������
struct CMD_CS_S_RegisterPersonal
{
	DWORD dwKindID;
	DWORD dwMaxCreate;
};
//////////////////////////////////////////////////////////////////////////////////
//������Ϣ

#define MDM_CS_SERVICE_INFO			2									//������Ϣ

//������Ϣ
#define SUB_CS_C_SERVER_ONLINE		1									//��������
#define SUB_CS_C_SERVER_MODIFY		2									//�����޸�
#define SUB_CS_S_SERVER_DUMMY_INFO	3									//ͳ��������������
#define SUB_CS_S_SERVER_DUMMY_INFO_START	4							//ͳ�������������ݿ�ʼ
#define SUB_CS_S_SERVER_DUMMY_INFO_END	5								//ͳ�������������ݽ���

//�㳡����
#define SUB_CS_S_PLAZA_INFO			100									//�㳡��Ϣ
#define SUB_CS_S_PLAZA_INSERT		101									//�㳡�б�
#define SUB_CS_S_PLAZA_REMOVE		103									//�㳡ɾ��
#define SUB_CS_S_PLAZA_FINISH		104									//�㳡���
#define SUB_CS_S_PLAZA_CHANGE_DUMMY	106									//�㳡�����������

//��������
#define SUB_CS_S_SERVER_INFO		110									//������Ϣ
#define SUB_CS_S_SERVER_ONLINE		111									//��������
#define SUB_CS_S_SERVER_INSERT		112									//�����б�
#define SUB_CS_S_SERVER_MODIFY		113									//�����޸�
#define SUB_CS_S_SERVER_REMOVE		114									//����ɾ��
#define SUB_CS_S_SERVER_FINISH		115									//�������
#define SUB_CS_S_CLEARUER			116									//�������
#define SUB_CS_S_MODIFY_DUMMY		117									//�޸���������
//��������
#define SUB_CS_S_MATCH_INSERT		120									//�����б�

//��������
#define SUB_CS_S_CHAT_INSERT		130									//�������
#define SUB_SS_S_ONLINE				140									//�û�����
#define SUB_SS_S_OFFLINE			141									//�û�����
#define SUB_SS_S_STATUS				142									//�û�״̬
#define SUB_SS_S_GAMESTATUS			143									//��Ϸ״̬
#define SUB_SS_S_SYNCHRFRIEND		144									//ͬ������	
#define SUB_SS_S_SYNCHARFINISH		145									//ͬ�����
#define SUB_SS_S_MODIFYFRIEND		146									//�޸ĺ���
#define SUB_SS_S_REMOVEGROUP		147									//�Ƴ�����

//////////////////////////////////////////////////////////////////////////////////
//��ɢ����
struct CMD_CS_S_ClearUser
{
	DWORD							dwServerID;							//����ID
	DWORD							dwUserID;							//UserID
};

//ǿ�ƴ�������
struct CMD_CS_S_ForceOffline
{
	DWORD							dwUserID;							//UserID
};

//�޸���������
struct CMD_CS_S_ModifyDummyCount
{
	DWORD							dwServerID;							//����ID
	DWORD							dwAddcounts;						//�޸�����
};


//��������
struct CMD_CS_C_ServerModify
{
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wServerPort;						//����˿�
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//��������
	DWORD							dwFullCount;						//��Ա����
	TCHAR							szServerAddr[32];					//�����ַ
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//��������
struct CMD_CS_C_ServerOnLine
{
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//��������
};

//��������
struct CMD_CS_C_DummyOnLine
{
	WORD							wServerID;							//������ID
	DWORD							dwDummyCount;						//������������
};

//////////////////////////////////////////////////////////////////////////////////

//�㳡ɾ��
struct CMD_CS_S_PlazaRemove
{
	DWORD							dwPlazaID;							//�㳡��ʶ
};

//��������
struct CMD_CS_S_ServerOnLine
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//��������
};

//�����޸�
struct CMD_CS_S_ServerModify
{
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wServerID;							//�����ʶ
	WORD							wServerPort;						//����˿�
	DWORD							dwOnLineCount;						//��������
	DWORD							dwAndroidCount;						//��������
	DWORD							dwFullCount;						//��Ա����
	TCHAR							szServerAddr[32];					//�����ַ
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//����ɾ��
struct CMD_CS_S_ServerRemove
{
	WORD							wServerID;							//�����ʶ
};

//////////////////////////////////////////////////////////////////////////////////
//�û�����

#define MDM_CS_USER_COLLECT			3									//�û�����

//�û�״̬
#define SUB_CS_C_USER_ENTER			1									//�û�����
#define SUB_CS_C_USER_LEAVE			2									//�û��뿪
#define SUB_CS_C_USER_FINISH		3									//�û����
#define SUB_CS_C_USER_STATUS		4									//�û�״̬

#define SUB_CS_C_CHAT_USER_ENTER	5									//�����û�����
#define SUB_CS_C_CHAT_USER_LEAVE	6									//�����û��뿪

//�û�״̬
#define SUB_CS_S_COLLECT_REQUEST	100									//��������

#define SUB_CS_S_USER_GAMESTATE		101									//�û�״̬

//�ĵ��û�ǿ������
#define SUB_CS_S_CHAT_FORCE_OFFLINE	102									//�����û�ǿ������
//////////////////////////////////////////////////////////////////////////////////

//�û�����
struct CMD_CS_C_UserEnter
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�

	//������Ϣ
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�

	BYTE							cbAndroidUser;						//���������

	//��ϸ��Ϣ
	tagUserInfo						userInfo;							//�û���Ϣ
};

//�û��뿪
struct CMD_CS_C_UserLeave
{
	DWORD							dwUserID;							//�û���ʶ
};

//�û�����
struct CMD_CS_C_ChatUserEnter
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
};

//�û��뿪
struct CMD_CS_C_ChatUserLeave
{
	DWORD							dwUserID;							//�û���ʶ
};


//�û�״̬
struct CMD_CS_C_UserStatus
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbUserStatus;						//�û�״̬
	WORD							wKindID;							//��Ϸ��ʶ
	WORD							wServerID;							//�����ʶ
	WORD							wTableID;							//��������
	WORD							wChairID;							//����λ��
};

//�û�״̬
struct CMD_CS_S_UserGameStatus
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbGameStatus;						//�û�״̬
	WORD							wKindID;							//��Ϸ��ʶ
	WORD							wServerID;							//�����ʶ
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wChairID;							//����λ��
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//////////////////////////////////////////////////////////////////////////////////
//Զ�̷���

#define MDM_CS_REMOTE_SERVICE		4									//Զ�̷���

//���ҷ���
#define SUB_CS_C_SEARCH_DATABASE	1									//���ݲ���
#define SUB_CS_C_SEARCH_CORRESPOND	2									//Э������
#define SUB_CS_C_SEARCH_ALLCORRESPOND	3								//Э������

//���ҽ��
#define SUB_CS_S_SEARCH_DATABASE	100									//���ݲ���
#define SUB_CS_S_SEARCH_CORRESPOND	101									//Э������
#define SUB_CS_S_SEARCH_ALLCORRESPOND	102								//Э������

//////////////////////////////////////////////////////////////////////////////////

//Э������
struct CMD_CS_C_SearchCorrespond
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	//��������
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
};

//Э������
struct CMD_CS_S_SearchCorrespond
{
	//������Ϣ
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	//������Ϣ
	WORD							wUserCount;							//�û���Ŀ
	tagUserRemoteInfo				UserRemoteInfo[16];					//�û���Ϣ
};


//Э������
struct CMD_CS_C_AllSearchCorrespond
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	DWORD							dwCount;
	DWORD							dwGameID[1];						//��Ϸ��ʶ
};

//Э������
struct CMD_CS_S_SearchAllCorrespond
{
	//������Ϣ
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	//������Ϣ
	DWORD							dwCount;
	tagUserRemoteInfo				UserRemoteInfo[1];
};


//////////////////////////////////////////////////////////////////////////////////
//�������

#define MDM_CS_MANAGER_SERVICE		5									//�������

#define SUB_CS_C_SYSTEM_MESSAGE 	1									//ϵͳ��Ϣ
#define SUB_CS_C_PROPERTY_TRUMPET 	2									//������Ϣ
#define SUB_CS_C_HALL_MESSAGE 	3										//������Ϣ

#define SUB_CS_S_SYSTEM_MESSAGE 	100									//ϵͳ��Ϣ
#define SUB_CS_S_HALL_MESSAGE		101									//������Ϣ

#define SUB_CS_S_PROPERTY_TRUMPET  	200									//������Ϣ
#define SUB_CS_S_PLATFORM_PARAMETER 300									//ƽ̨����	


//��������
struct CMD_CS_S_SendTrumpet
{
	WORD                           wPropertyIndex;                      //�������� 
	DWORD                          dwSendUserID;                         //�û� I D
	DWORD                          TrumpetColor;                        //������ɫ
	TCHAR                          szSendNickName[32];				    //����ǳ� 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //��������
};
//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_CS_ANDROID_SERVICE	    6									//��������

#define SUB_CS_C_ADDPARAMETER		100									//��Ӳ���
#define SUB_CS_C_MODIFYPARAMETER    101									//�޸Ĳ���
#define SUB_CS_C_DELETEPARAMETER	102									//ɾ������	

#define SUB_CS_S_ADDPARAMETER		200									//��Ӳ���
#define SUB_CS_S_MODIFYPARAMETER    201									//�޸Ĳ���
#define SUB_CS_S_DELETEPARAMETER	202									//ɾ������	

//////////////////////////////////////////////////////////////////////////////////
//��Ӳ���
struct CMD_CS_C_AddParameter
{
	WORD							wServerID;							//�����ʶ
	tagAndroidParameter				AndroidParameter;					//��������
};

//�޸Ĳ���
struct CMD_CS_C_ModifyParameter
{
	WORD							wServerID;							//�����ʶ
	tagAndroidParameter				AndroidParameter;					//��������
};

//ɾ������
struct CMD_CS_C_DeleteParameter
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwBatchID;							//���α�ʶ
};


//��Ӳ���
struct CMD_CS_S_AddParameter
{	
	tagAndroidParameter				AndroidParameter;					//��������
};

//�޸Ĳ���
struct CMD_CS_S_ModifyParameter
{
	tagAndroidParameter				AndroidParameter;					//��������
};

//ɾ������
struct CMD_CS_S_DeleteParameter
{
	DWORD							dwBatchID;							//���α�ʶ
};

//++++++++++++++++++++++++++++++Լս����
//˽�˷�������
#define SUB_CS_C_SEARCH_TABLE		131									//��������
#define SUB_CS_S_SEARCH_TABLE_RESULT	132								//���ҽ��
#define SUB_CS_C_QUERY_GAME_SERVER	133									//���ҷ���
#define SUB_CS_S_QUERY_GAME_SERVER_RESULT	134							//���ҽ��
#define SUB_CS_C_CREATE_TABLE		135									//��������
#define SUB_CS_S_CREATE_TABLE_RESULT	136								//�������
#define SUB_CS_C_DISMISS_TABLE		137									//��ɢ����
#define SUB_CS_C_DISMISS_TABLE_RESULT		138									//��ɢ����
#define SUB_CS_S_INSERT_CREATE_RECORD	139									//���봴����¼
#define SUB_CS_S_QUERY_PERSONAL_ROOM_LIST	140				//����˽�˷����б�
#define SUB_CS_C_QUERY_PERSONAL_ROOM_LIST_RESULT	141  //����˽�˷����� 
#define SUB_CS_C_DISSUME_SEARCH_TABLE	142						//�����ɢ˽�˷��� 
#define SUB_CS_S_DISSUME_SEARCH_TABLE_RESULT	143			//���ҽ��
//��ȡ˽�˷����б�
struct CMD_MB_QeuryPersonalRoomList
{
	DWORD							dwUserID;							//�û�ID
	DWORD							dwKindID;								//��������
};

struct CMD_MB_SC_QeuryPersonalRoomList
{
	DWORD							dwUserID;							//�û�ID
	DWORD							dwSocketID;						//����ID
	DWORD							dwKindID;								//��Ϸ����
};


//������󷿼�ɼ�
struct CMD_GR_QUERY_USER_ROOM_SCORE
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwKindID;								//��������
};

//++++++++++++++++++++++++++++++Լս����
//��������
struct CMD_CS_C_SearchTable
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwKindID;							//��Ϸ���ͱ�ʶ
	TCHAR							szPersonalTableID[7];				//�����ʶ
};

//���ҽ��
struct CMD_CS_S_SearchTableResult
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	DWORD							dwServerID;							//����I D
	DWORD							dwTableID;							//����I D
};

//��ѯ����
struct CMD_CS_C_QueryGameServer
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwKindID;							//��Ϸ��ʶ
	BYTE									cbIsJoinGame;						//�Ƿ������Ϸ
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//��ѯ���
struct CMD_CS_S_QueryGameServerResult
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ

	//������Ϣ
	DWORD							dwServerID;							//��ѯ���
	bool									bCanCreateRoom;				//�Ƿ���Դ�������

	//��������
	TCHAR								szErrDescrybe[MAX_PATH];						//��������
};

//��������
struct CMD_CS_C_CreateTable
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR								szClientAddr[16];							//���������IP��ַ
	//������Ϣ
	tagPersonalTableInfo			PersonalTable;						//������Ϣ
};

//�������
struct CMD_CS_S_CreateTableResult
{
	//���Ӳ���
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR								szClientAddr[16];							//���������IP��ַ

	//������Ϣ
	tagPersonalTableInfo			PersonalTable;						//������Ϣ
	////������Ϣ
	//DWORD							dwTableID;							//����I D						
	//TCHAR							szServerID[7];						//������
};

//��ɢ����
struct CMD_CS_C_DismissTable
{
	DWORD							dwSocketID;						//����ID
	DWORD							dwServerID;							//����I D
	DWORD							dwTableID;							//����I D
	CMD_CS_C_DismissTable()
	{
		dwSocketID = -1;
	}
};

struct CMD_CS_C_DismissTableResult
{
	DWORD							dwSocketID;						//����ID
	tagPersonalTableInfo		PersonalTableInfo;
	tagPersonalUserScoreInfo PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];
};

//˽�˷����б�
struct CMD_CS_C_HostCreatRoomInfo
{
	DWORD							wSocketID;							//�����ʶ
	tagHostCreatRoomInfo  HostCreatRoomInfo;
};


//���Ӵ�����¼
#define ROOM_ID 7
struct DBR_GR_InsertCreateRecord
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwServerID;							//�����ʶ
	LONGLONG						lCellScore;							//���õ׷�
	DWORD							dwDrawCountLimit;					//��������
	DWORD							dwDrawTimeLimit;					//ʱ������
	TCHAR							szRoomID[ROOM_ID];
	TCHAR							szPassword[LEN_PASSWORD];	//��������
	DWORD							dwRoomTax;							//����һ��˽�˷����˰�ʣ�ǧ�ֱ�
	WORD								wJoinGamePeopleCount;			//������Ϸ������
	TCHAR								szClientAddr[16];							//���������IP��ַ
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif