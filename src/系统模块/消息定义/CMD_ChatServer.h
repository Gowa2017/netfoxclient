#ifndef CMD_CHAT_SERVER_HEAD_FILE
#define CMD_CHAT_SERVER_HEAD_FILE

#pragma pack(1)

#define CHAT_MSG_OK						0								//消息成功
#define CHAT_MSG_ERR					1								//消息失败

//////////////////////////////////////////////////////////////////////////////////
//登录命令
#define MDM_GC_LOGON					1								//登录信息

#define SUB_GC_PC_LOGON_USERID   		100								//PC 登录
#define SUB_GC_MB_LOGON_USERID			101								//手机登录
#define SUB_GC_MB_LOGON_ACCOUNTS		102								//账号登录

//登录结果
#define SUB_GC_LOGON_SUCCESS			200								//登录成功
#define SUB_GC_LOGON_FAILURE			210								//登录失败

//用户信息
#define SUB_S_USER_GROUP				302								//用户分组
#define SUB_S_USER_FRIEND				303								//用户好友
#define SUB_S_USER_REMARKS				304								//用户备注	
#define SUB_S_USER_SINDIVIDUAL			305								//附属资料			
#define SUB_S_LOGON_AFRESH				306								//用户重入
#define SUB_S_LOGON_FINISH				307								//登录完成


//////////////////////////////////////////////////////////////////////////////////

//用户登录
struct CMD_GC_MB_LogonByUserID
{
	//登录信息
	DWORD							dwUserID;							//玩家标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码	
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//手机型号
	TCHAR							szMachineID[LEN_MACHINE_ID];			//机器码
};

//用户登录
struct CMD_GC_MB_LogonByAccounts
{
	//登录信息
	TCHAR							szAccounts[LEN_ACCOUNTS];			//玩家账号
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码	
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//手机型号
	TCHAR							szMachineID[LEN_MACHINE_ID];			//机器码
};

//登录回馈
struct CMD_GC_LogonEcho
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//别处登录
struct CMD_GC_UserLogonAfresh
{
	TCHAR							wNotifyMessage[128];				//描述消息
};

//用户信息
struct CMD_GC_UserFriendInfo
{
	WORD							wFriendCount;						//好友数目
	tagClientFriendInfo				FriendInfo[MAX_FRIEND_TRANSFER];	//好友信息
};

//携带信息
#define DTP_GP_UI_QQ						1								 //QQ
#define DTP_GP_UI_EMAIL						2								 //EMail
#define DTP_GP_UI_SEATPHOHE					3								 //座机号码
#define DTP_GP_UI_MOBILEPHONE				4								 //手机号码
#define DTP_GP_UI_DWELLINGPLACE				5								 //联系地址
#define DTP_GP_UI_POSTALCODE				6								 //邮政编码	
#define DTP_GP_UI_NICKNAME					7								 //用户昵称
#define DTP_GP_UI_UNDERWRITE				8								 //个性签名
#define DTP_GP_UI_COMPELLATION				9								 //真实名字
#define DTP_GP_UI_REMARKS					10								 //用户备注

//个人资料
struct CMD_GC_UserIndividual
{
	//基本信息
	DWORD								dwUserID;							//用户标识
};

//用户命令
#define MDM_GC_USER						3									//用户信息
//////////////////////////////////////////////////////////////////////////////////
#define	SUB_GC_USER_STATUS_NOTIFY		1									//用户状态
#define SUB_GC_GAME_STATUS_NOTIFY		2									//游戏动态

#define SUB_GC_USER_CHAT				100									//用户聊天	
#define SUB_GC_APPLYFOR_FRIEND   		101									//申请好友
#define SUB_GC_RESPOND_FRIEND			102									//好友回应
#define SUB_GC_SEARCH_USER				103									//查找用户
#define SUB_GC_INVITE_GAME				104									//邀请游戏
#define SUB_GC_TRUMPET					105									//用户喇叭
#define SUB_GC_DELETE_FRIEND			106									//删除好友
#define SUB_GC_MODIFY_FRIEND			107									//修改好友
#define SUB_GC_MODIFY_GROUP				108									//修改分组
#define SUB_GC_UPDATE_COORDINATE		109									//更新坐标
#define SUB_GC_GET_NEARUSER				110									//附近的人
#define SUB_GC_QUERY_NEARUSER			111									//查询附近
#define SUB_GC_USER_SHARE				112									//分享消息
#define SUB_GC_INVITE_PERSONAL			113									//邀请游戏
#define SUB_GC_FORCE_OFFLINE			114									//用户强制下线

#define SUB_GC_USER_CHAT_NOTIFY			200									//聊天通知
#define SUB_GC_APPLYFOR_NOTIFY			201									//申请通知
#define SUB_GC_RESPOND_NOTIFY			202									//回应通知
#define SUB_GC_SEARCH_USER_RESULT		203									//查找结果
#define SUB_GC_INVITE_GAME_NOTIFY		204									//邀请通知
#define SUB_GC_TRUMPET_NOTIFY			205									//喇叭通知
#define SUB_GC_DELETE_FRIEND_NOTIFY		206									//删除通知
#define SUB_GC_MODIFY_FRIEND_NOTIFY		207									//修改结果
#define SUB_GC_MODIFY_GROUP_NOTIFY		208									//修改结果
#define SUB_GC_UPDATE_COORDINATE_NOTIFY	209									//更新坐标
#define SUB_GC_GET_NEARUSER_RESULT		210									//附近结果
#define SUB_GC_QUERY_NEARUSER_RESULT	211									//查询结果
#define SUB_GC_USER_SHARE_NOTIFY		212									//分享通知
#define SUB_GC_INVITE_PERSONAL_NOTIFY	213									//邀请通知
#define SUB_GC_HALL_NOTIFY				214									//大厅通知

#define SUB_GC_USER_CHAT_ECHO			300									//聊天反馈
#define SUB_GC_APPLYFOR_FRIEND_ECHO		301									//申请反馈
#define SUB_GC_RESPOND_FRIEND_ECHO		302									//回应反馈
#define SUB_GC_SEARCH_USER_ECHO			303									//查找反馈
#define SUB_GC_INVITE_GAME_ECHO			304									//邀请反馈
#define SUB_GC_TRUMPET_ECHO				305									//喇叭反馈
#define SUB_GC_DELETE_FRIEND_ECHO		306									//删除反馈
#define SUB_GC_MODIFY_FRIEND_ECHO		307									//修改反馈
#define SUB_GC_MODIFY_GROUP_ECHO		308									//修改反馈
#define SUB_GC_UPDATE_COORDINATE_ECHO	309									//更新坐标
#define SUB_GC_GET_NEARUSER_ECHO		310									//附近反馈
#define SUB_GC_QUERY_NEARUSER_ECHO		311									//查询反馈
#define SUB_GC_USER_SHARE_ECHO			312									//分享反馈
#define SUB_GC_INVITE_PERSONAL_ECHO		313									//邀请反馈

#define SUB_GC_SYSTEM_MESSAGE			401									//系统消息

//操作结果
#define SUB_GC_OPERATE_SUCCESS			800									//操作成功
#define SUB_GP_OPERATE_FAILURE			801									//操作失败
//////////////////////////////////////////////////////////////////////////

//用户上线/离线
struct CMD_GC_UserOnlineStatusNotify
{
	DWORD								dwUserID;							//用户 I D
	BYTE								cbMainStatus;						//用户状态
};

//游戏状态
struct CMD_GC_UserGameStatusNotify
{
	DWORD								dwUserID;							//用户标识
	BYTE								cbGameStatus;						//游戏状态
	WORD								wKindID;							//游戏标识
	WORD								wServerID;							//房间标识
	WORD								wTableID;							//桌子标识
	TCHAR								szServerName[LEN_SERVER];			//房间名称
};


//用户聊天
struct CMD_GC_UserChat
{
	DWORD								dwSenderID;							//用户标识
	DWORD								dwTargetUserID;						//目标用户
	DWORD								dwFontColor;						//字体颜色
	BYTE								cbFontSize;							//字体大小
	BYTE								cbFontAttri;						//字体属性
	TCHAR								szFontName[LEN_FONT_NAME];			//字体名称
	TCHAR								szMessageContent[LEN_MESSAGE_CONTENT]; //消息内容	
};

//聊天回馈
struct CMD_GC_UserChatEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};


//申请好友
struct CMD_GC_ApplyForFriend
{
	DWORD								dwUserID;							//用户标识
	DWORD								dwFriendID;							//好友标识
	BYTE								cbGroupID;							//组别标识
};

//申请回馈
struct CMD_GC_ApplyForFriendEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};

//申请通知
struct CMD_GC_ApplyForNotify
{
	DWORD								dwRequestID;						//用户标识
	BYTE								cbGroupID;							//组别标识
	TCHAR								szNickName[LEN_ACCOUNTS];			//用户昵称
};

//回应好友
struct CMD_GC_RespondFriend
{
	DWORD								dwUserID;							//用户标识
	DWORD								dwRequestID;						//用户标识
	BYTE								cbRequestGroupID;					//组别标识
	BYTE								cbGroupID;							//组别标识
	bool								bAccepted;							//是否接受
};

//回应回馈
struct CMD_GC_RespondFriendEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};

//回应通知
struct CMD_GC_RespondNotify
{
	TCHAR								szNotifyContent[128];				//提示内容
};

//查找好友
struct CMD_GC_SearchByGameID
{
	DWORD								dwSearchByGameID;					//用户标识
};

//查找回馈
struct CMD_GC_SearchByGameIDEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};

//查找结果
struct CMD_GC_SearchByGameIDResult
{
	BYTE								cbUserCount;						//用户数目
	tagClientFriendInfo					FriendInfo;							//好友信息
};


//邀请游戏
struct CMD_GC_InviteGame
{
	DWORD								dwUserID;							//邀请用户	
	DWORD								dwInvitedUserID;					//被邀用户
	WORD								wKindID;							//游戏标识
	WORD								wServerID;							//房间标识
	WORD								wTableID;							//桌子标识
	TCHAR								szInviteMsg[128];					//邀请信息
};

//邀请反馈
struct CMD_GC_InviteGameEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};


//邀请通知
struct CMD_GC_InviteGameNotify
{
	DWORD								dwInviteUserID;						//邀请用户
	WORD								wKindID;							//游戏标识
	WORD								wServerID;							//房间标识
	WORD								wTableID;							//桌子标识
	TCHAR								szInviteMsg[128];					//邀请信息
};

////用户喇叭
//struct CMD_GC_Trumpet
//{
//	DWORD								dwSenderID;							//用户标识
//	TCHAR								szNickName[LEN_ACCOUNTS];			//用户昵称
//	DWORD								dwFontColor;						//字体颜色
//	BYTE								cbFontSize;							//字体大小
//	BYTE								cbFontAttri;						//字体属性
//	TCHAR								szFontName[LEN_FONT_NAME];			//字体名称
//	TCHAR								szMessageContent[LEN_MESSAGE_CONTENT]; //消息内容	
//};

//用户喇叭
struct CMD_GC_Trumpet
{
	WORD                           wPropertyID;                         //道具索引 
	DWORD                          dwSendUserID;                        //用户 I D
	DWORD                          TrumpetColor;                        //喇叭颜色
	TCHAR                          szSendNickName[32];				    //玩家昵称 
	TCHAR                          szTrumpetContent[TRUMPET_MAX_CHAR];  //喇叭内容
};

//喇叭反馈
struct CMD_GC_TrumpetEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};

//删除好友
struct CMD_GC_DeleteFriend
{
	DWORD								dwUserID;							//用户标识
	DWORD								dwFriendUserID;						//用户标识	
	BYTE								cbGroupID;							//组别标识
};

//删除反馈
struct CMD_GC_DeleteFriendEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};

//删除结果
struct CMD_GC_DeleteFriendNotify
{
	DWORD								dwFriendUserID;						//用户标识	
};

//修改好友
struct CMD_GC_ModifyFriend
{
	DWORD								dwUserID;							//用户标识
	DWORD								dwFriendUserID;						//用户标识	
	BYTE								cbGroupID;							//组别标识
};
//修改反馈
struct CMD_GC_ModifyFriendEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};

//修改好友
struct CMD_GC_ModifyFriendResult
{
	bool								bSuccessed;							//成功标识
	DWORD								dwFriendUserID;						//用户标识	
	BYTE								cbGroupID;							//组别标识
};

//修改分组
struct CMD_GC_ModifyGroup
{
	DWORD								dwUserID;							//用户标识
	BYTE								cbGroupID;							//组别标识
	TCHAR								szGroupName[GROUP_LEN_NAME];		//组别名称
};

//修改反馈
struct CMD_GC_ModifyGroupEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};

//修改结果
struct CMD_GC_ModifyGroupResult
{
	BYTE								cbGroupID;							//组别标识
	TCHAR								szGroupName[GROUP_LEN_NAME];		//组别名称
};

//更新坐标
struct CMD_GC_Update_Coordinate
{
	DWORD								dwUserID;							//用户标识
	DOUBLE								dLongitude;							//坐标经度
	DOUBLE								dLatitude;							//坐标纬度
};

//坐标反馈
struct CMD_GC_Update_CoordinateEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};

//坐标通知
struct CMD_GC_Update_CoordinateNotify
{
	DOUBLE								dLongitude;							//坐标经度
	DOUBLE								dLatitude;							//坐标纬度
	BYTE								cbCoordinate;						//开启标识
	DWORD								dwClientAddr;						//用户地址
};


//获取附近
struct CMD_GC_Get_Nearuser
{
	DWORD								dwUserID;							//用户标识
	DOUBLE								dLongitude;							//坐标经度
	DOUBLE								dLatitude;							//坐标纬度
};

//附近反馈
struct CMD_GC_Get_NearuserEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};

//附近结果
struct CMD_GC_Get_NearuserResult
{
	BYTE								cbUserCount;						//用户数目
	tagNearUserInfo						NearUserInfo[MAX_FRIEND_TRANSFER];	//附近信息
};

//查询附近
struct CMD_GC_Query_Nearuser
{
	DWORD								dwUserID;							//用户标识
	DWORD								dwNearuserUserID;					//附近用户
};

//查询反馈
struct CMD_GC_Query_NearuserEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};

//查询结果
struct CMD_GC_Query_NearuserResult
{
	BYTE								cbUserCount;						//用户数目
	tagNearUserInfo						NearUserInfo;						//附近信息
};


//系统消息
struct CMD_GC_S_SystemMessage
{
	WORD								wType;								//消息类型
	WORD								wLength;							//消息长度
	TCHAR								szString[1024];						//消息内容
};

//分享消息
struct CMD_GC_UserShare
{
	DWORD								dwUserID;								//用户标识
	DWORD								dwSharedUserID;							//被分享用户	
	TCHAR								szShareImageAddr[128];					//图片地址
	TCHAR								szMessageContent[128];				    //消息内容
};

//分享反馈
struct CMD_GC_UserShareEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};

//分享通知
struct CMD_GC_UserShareNotify
{
	DWORD								dwShareUserID;							//用户标识
	TCHAR								szShareImageAddr[128];					//图片地址
	TCHAR								szMessageContent[128];				    //消息内容
};

//邀请游戏
struct CMD_GC_InvitePersonalGame
{
	DWORD								dwUserID;							//邀请用户	
	DWORD								dwInvitedUserID;					//被邀用户
	WORD								wKindID;							//游戏标识
	DWORD								dwServerNumber;						//房间号码
	WORD								wTableID;							//桌子标识
	TCHAR								szInviteMsg[128];					//邀请信息
};

//邀请反馈
struct CMD_GC_InvitePersonalGameEcho
{
	LONG								lErrorCode;							//错误代码
	TCHAR								szDescribeString[128];				//描述消息
};


//邀请通知
struct CMD_GC_InvitePersonalGameNotify
{
	DWORD								dwInviteUserID;						//邀请用户
	WORD								wKindID;							//游戏标识
	DWORD								dwServerNumber;						//房间号码
	WORD								wTableID;							//桌子标识
	TCHAR								szInviteMsg[128];					//邀请信息
};


//操作失败
struct CMD_GC_OperateFailure
{
	LONG								lErrorCode;							//错误代码
	LONG								lResultCode;						//操作代码
	TCHAR								szDescribeString[128];				//错误消息
};

//操作成功
struct CMD_GC_OperateSuccess
{
	LONG								lResultCode;						//操作代码
	TCHAR								szDescribeString[128];				//成功消息
};


//////////////////////////////////////////////////////////////////////////
//配置命令

#define MDM_GC_CONFIG				2									//登录信息

#define SUB_GC_CONFIG_SERVER		1									//配置参数

struct CMD_GC_ConfigServer
{
	BYTE							cbMinOrder;							//最低级别
	WORD							wMaxPlayer;							//最大人数
	WORD							wServerPort;						//服务端口
	DWORD							dwServerAddr;						//服务地址
	DWORD							dwServiceRule;						//服务规则
};


//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif