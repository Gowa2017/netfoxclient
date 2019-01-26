#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//请求数据包

//////////////////////////////////////////////////////////////////////////////////

//登录命令
#define LOGON_COMMAND_USERINFO      0x0001								//用户信息
#define LOGON_COMMAND_GROUPINFO		0x0002								//分组信息
#define LOGON_COMMAND_FACE			0x0004								//用户头像
#define LOGON_COMMAND_FRIENDS       0x0008								//用户好友
#define LOGON_COMMAND_REMARKS		0x0010								//备注信息

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//请求数据包

//用户事件
#define	DBR_GR_LOGON_USERID			100									//I D 登录
#define	DBR_GR_LOGON_MOBILE			101									//手机登录
#define	DBR_GR_LOGON_ACCOUNTS		102									//账号登录

//用户命令
#define DBR_GR_MODIFY_LOGONPASS		200									//修改密码
#define DBR_GR_MODIFY_UNDER_WRITE	201									//修改签名
#define DBR_GR_MODIFY_INDIVIDUAL	202									//修改资料
#define DBR_GR_QUERY_INDIVIDUAL		203									//查询资料
#define DBR_GR_MODIFY_FRIEND		204									//修改好友
#define DBR_GR_MODIFY_GROUP			205									//修改分组
#define DBR_GR_MODIFY_REMARKS		206									//修改备注	
#define DBR_GR_SEARCH_USER			207									//搜索好友
#define DBR_GR_ADD_FRIEND			208									//添加好友
#define DBR_GR_SYSTEM_FACEINFO		209									//系统头像
#define DBR_GR_CUSTOM_FACEINFO		210									//自定头像
#define DBR_GR_CUSTOM_FACEDELETE	211									//删除头像
#define DBR_GR_SAVE_OFFLINEMESSAGE  212									//离线消息
#define DBR_GR_QUERY_FRIENDWEALTH	213									//好友财富	
#define DBR_GR_TRUMPET				214									//喇叭消息
#define DBR_GR_DELETE_FRIEND		215									//删除好友


//////////////////////////////////////////////////////////////////////////////////
//输出信息

//逻辑事件
#define DBO_GR_LOGON_SUCCESS		100									//登录成功
#define DBO_GR_LOGON_FAILURE		101									//登录失败
#define DBO_GR_ACCOUNT_LOGON_FAILURE		102							//登录失败

//用户事件
#define DBO_GR_USER_GROUPINFO		200									//分组信息
#define DBO_GR_USER_FRIENDINFO		201									//好友信息
#define DBO_GR_USER_REMARKS			202									//好友备注
#define DBO_GR_USER_SINDIVIDUAL		203									//简易资料
#define DBO_GR_LOAD_OFFLINEMESSAGE  204									//读取消息	

//配置事件
#define DBO_GR_SERVER_PARAMETER		300									//配置信息

//用户命令
#define DBO_GR_MODIFY_LOGONPASS		400									//修改密码
#define DBO_GR_MODIFY_UNDERWRITE	401									//修改签名
#define DBO_GR_MODIFY_INDIVIDUAL	402									//修改资料
#define DBO_GR_QUERY_INDIVIDUAL		403									//查询资料
#define DBO_GR_MODIFY_GROUP			404									//修改分组
#define DBO_GR_MODIFY_FRIEND		405									//修改好友
#define DBO_GR_MODIFY_REMARKS		406									//修改备注
#define DBO_GR_ADD_FRIEND			407									//添加好友
#define DBO_GR_SEARCH_USER_RESULT	408									//搜索结果
#define DBO_GR_OPERATE_RESULT		409									//操作结果
#define DBO_GR_FRIEND_WEALTH		410									//好友财富	
#define DBO_GR_TRUMPET_RESULT		411									//道具成功
#define DBO_GR_DELETE_FRIEND		412									//删除好友
//////////////////////////////////////////////////////////////////////////////////

//ID 登录
struct DBR_GR_LogonUserID
{		
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//手机型号
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器码
	BYTE							wLogonComand;						//登录命令
	DWORD							dwClientAddr;						//连接地址
};

//账号 登录
struct DBR_GR_LogonUserAccounts
{		
	//登录信息
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户 账号
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//手机型号
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器码
	BYTE							wLogonComand;						//登录命令
	DWORD							dwClientAddr;						//连接地址
};

//ID 登录
struct DBR_GR_LogonMobile
{		
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	BYTE							wLogonComand;						//登录命令

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	WORD							wClientPort;						//连接端口
};

//离线消息
struct DBR_GR_SaveOfflineMessage
{
	DWORD							dwUserID;							//用户标识
	WORD							wMessageType;						//消息类型
	WORD							wDataSize;							//数据大小
	TCHAR							szOfflineData[2400];				//消息内容
};

//////////////////////////////////////////////////////////////////////////////////

//登录成功
struct DBO_GR_LogonSuccess
{
	//用户属性
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户账号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	DWORD							dwFaceID;							//头像 I D
	DWORD							dwCustomID;							//头像 I D
	BYTE							cbGender;							//用户性别
	WORD							wMemberOrder;						//用户等级
	WORD							wGrowLevel;							//用户等级
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实姓名
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//手机型号
	DWORD							dwClientAddr;						//用户地址

	//附属资料
	TCHAR							szQQ[LEN_QQ];						//用户QQ
	TCHAR							szEMail[LEN_EMAIL];					//用户Eamil
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//座机号码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码	
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
	TCHAR							szPostalCode[LEN_POSTALCODE];		//邮政编码

	//状态
	TCHAR							szMachineID[LEN_MACHINE_ID];			//机器码
};

//登录失败
struct DBO_GR_LogonFailure
{
	LONG							lResultCode;						//错误代码
	DWORD							dwUserID;							//用户标识
	TCHAR							szDescribeString[128];				//错误消息
};

//登录失败
struct DBO_GR_AccountLogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//用户分组
struct DBO_GR_UserGroupInfo
{
	WORD							wGroupCount;						//分组数目
	tagClientGroupInfo				GroupInfo[GROUP_MAX_COUNT];			//分组信息
};

//好友信息
struct DBO_GR_UserFriendInfo
{
	bool							bLogonFlag;							//登录标识
	DWORD							dwUserID;							//用户标识
	WORD							wFriendCount;						//好友数目
	tagClientFriendInfo				FriendInfo[MAX_FRIEND_COUNT];		//好友信息
};

//备注信息
struct DBO_GR_UserRemarksInfo
{
	DWORD							dwUserID;							//用户标识
	WORD							wFriendCount;						//用户数目
	tagUserRemarksInfo				RemarksInfo[MAX_FRIEND_COUNT];		//备注信息
};

//好友资料
struct DBO_GR_UserIndividual
{
	DWORD							dwUserID;							//用户标识
	WORD							wUserCount;							//用户数目
	tagUserIndividual				UserIndividual[MAX_FRIEND_COUNT];	//简易资料
};

//配置信息
struct DBO_GR_ServerParameter
{
	//版本信息
	DWORD							dwClientVersion;					//客户版本
};




//修改好友
struct DBO_GR_ModifyGroup
{
	DWORD							dwUserID;							//用户标识
	bool							bSuccessed;							//成功标识
	BYTE							cbOperateKind;						//操作类型
	BYTE							cbGroupID;							//组别标识
	TCHAR							szGroupName[GROUP_LEN_NAME];		//分组名称	
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//提示消息
};

//修改好友
struct DBO_GR_ModifyFriend
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwFriendUserID;						//用户标识	
	bool							bSuccessed;							//成功标识
	BYTE							cbOperateKind;						//操作类型
	BYTE							cbGroupID;							//组别标识
};

//修改备注
struct DBO_GR_ModifyRemarks
{
	bool							bSuccessed;							//成功标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwFriendID;							//好友标识
	TCHAR							szUserRemarks[LEN_REMARKS];			//好友备注
};

//添加好友
struct DBO_GR_AddFriend
{
	bool							bLoadUserInfo;						//加载信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwRequestUserID;					//用户标识	
	BYTE							cbGroupID;							//组别标识
	BYTE							cbRequestGroupID;					//组别标识
};

//搜索用户
struct DBO_GR_SearchUserResult
{
	BYTE							cbUserCount;						//用户数目
	tagClientFriendInfo				FriendInfo;							//好友信息
};



//操作结果
struct DBO_GR_OperateResult
{
	bool							bModifySucesss;						//成功标识
	DWORD							dwErrorCode;						//错误代码
	WORD							wOperateCode;						//操作代码
	TCHAR							szDescribeString[128];				//提示消息
};

//离线消息
struct DBO_GR_UserOfflineMessage
{
	WORD							wMessageType;						//消息类型
	WORD							wDataSize;							//数据大小
	TCHAR							szOfflineData[2400];				//消息内容
};

//////////////////////////////////////////////////////////////////////////////////



//修改好友
struct DBR_GR_ModifyFriend
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwFriendUserID;						//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	BYTE							cbOperateKind;						//操作类型
	BYTE							cbGroupID;							//组别标识
};

//添加好友
struct DBR_GR_AddFriend
{
	bool							bLoadUserInfo;						//加载信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwRequestUserID;					//用户标识	
	BYTE							cbGroupID;							//组别标识
	BYTE							cbRequestGroupID;					//组别标识
};

//删除好友
struct DBR_GR_DeleteFriend
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwFriendUserID;						//用户标识	
	BYTE							cbGroupID;							//组别标识
};

//删除好友
struct DBO_GR_DeleteFriend
{
	bool							bSuccessed;							//成功标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwFriendUserID;						//用户标识	
	BYTE							cbGroupID;							//组别标识
};

//搜索用户
struct DBR_GR_SearchUser
{
	DWORD							dwSearchByGameID;					//用户标识
};

//修改分组
struct DBR_GR_ModifyGroup
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	BYTE							cbOperateKind;						//操作类型
	BYTE							cbGroupID;							//组别标识
	TCHAR							szGroupName[GROUP_LEN_NAME];		//组别名称
};

//修改备注
struct DBR_GR_ModifyRemarks
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwFriendID;							//好友标识
	TCHAR							szUserRemarks[LEN_REMARKS];			//好友备注
};

////喇叭
//struct DBR_GR_Trumpet
//{
//	DWORD							dwClientAddr;							//连接地址
//	DWORD							dwSourceUserID;							//购买对象
//	DWORD							dwFontColor;							//字体颜色
//	BYTE							cbFontSize;								//字体大小
//	BYTE							cbFontAttri;							//字体属性
//	TCHAR							szNickName[LEN_ACCOUNTS];				//用户昵称
//	TCHAR							szFontName[LEN_FONT_NAME];				//字体名称
//	TCHAR							szMessageContent[LEN_MESSAGE_CONTENT];  //消息内容
//};

//struct DBO_GR_TrumpetResult
//{
//	LONG							lResult;
//	DWORD							dwSourceUserID;							//购买对象
//	DWORD							dwFontColor;							//字体颜色
//	BYTE							cbFontSize;								//字体大小
//	BYTE							cbFontAttri;							//字体属性
//	TCHAR							szNickName[LEN_ACCOUNTS];				//用户昵称
//	TCHAR							szFontName[LEN_FONT_NAME];				//字体名称
//	TCHAR							szMessageContent[LEN_MESSAGE_CONTENT];  //消息内容
//	TCHAR							szNotifyContent[128];					//描述消息
//};

//喇叭
struct DBR_GR_Trumpet
{
	DWORD						   dwClientAddr;						//连接地址
	WORD                           wPropertyID;                         //道具索引 
	DWORD                          dwSendUserID;                        //用户 I D
	DWORD                          TrumpetColor;                        //喇叭颜色
	TCHAR                          szSendNickName[32];				    //玩家昵称 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //喇叭内容
};

struct DBO_GR_TrumpetResult
{
	LONG						   lResult;
	WORD                           wPropertyID;                         //道具索引 
	DWORD                          dwSendUserID;                        //用户 I D
	DWORD                          TrumpetColor;                        //喇叭颜色
	TCHAR                          szSendNickName[32];				    //玩家昵称 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //喇叭内容
	TCHAR						   szNotifyContent[128];					//描述消息
};


//////////////////////////////////////////////////////////////////////////////////

#endif