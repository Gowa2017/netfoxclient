local chat_cmd = {}

chat_cmd.CHAT_MSG_OK					= 0							    --消息成功
chat_cmd.CHAT_MSG_ERR					= 1								--消息失败

--用户状态
chat_cmd.FRIEND_US_OFFLINE				= 0								--下线状态
chat_cmd.FRIEND_US_ONLINE				= 1								--在线状态
 
--登录命令= 
chat_cmd.MDM_GC_LOGON					= 1								--登录信息

chat_cmd.SUB_GC_PC_LOGON_USERID   		= 100							--pc登录
chat_cmd.SUB_GC_MB_LOGON_USERID			= 101							--手机登录
--登录结果=
chat_cmd.SUB_GC_LOGON_SUCCESS			= 200							--登录成功
chat_cmd.SUB_GC_LOGON_FAILURE			= 210							--登录失败
--用户信息=
chat_cmd.SUB_S_USER_GROUP				= 302							--用户分组
chat_cmd.SUB_S_USER_FRIEND				= 303							--用户好友
chat_cmd.SUB_S_USER_REMARKS				= 304							--用户备注	
chat_cmd.SUB_S_USER_SINDIVIDUAL			= 305							--简易资料			
chat_cmd.SUB_S_LOGON_AFRESH				= 306							--用户重入
chat_cmd.SUB_S_LOGON_FINISH				= 307							--登录完成


---------------
------用户命令
---------------
chat_cmd.MDM_GC_USER					= 3								--用户信息

chat_cmd.SUB_GC_USER_STATUS_NOTIFY		= 1								--用户状态(上下线)
chat_cmd.SUB_GC_GAME_STATUS_NOTIFY 		= 2								--游戏动态()

chat_cmd.SUB_GC_USER_CHAT				= 100							--用户聊天	
chat_cmd.SUB_GC_APPLYFOR_FRIEND   		= 101							--申请好友
chat_cmd.SUB_GC_RESPOND_FRIEND			= 102							--好友回应
chat_cmd.SUB_GC_SEARCH_USER				= 103							--查找用户
chat_cmd.SUB_GC_INVITE_GAME				= 104							--邀请游戏
chat_cmd.SUB_GC_TRUMPET					= 105							--用户喇叭
chat_cmd.SUB_GC_DELETE_FRIEND			= 106							--删除好友
chat_cmd.SUB_GC_UPDATE_COORDINATE 		= 109 							--更新坐标
chat_cmd.SUB_GC_GET_NEARUSER 			= 110 							--附近的人
chat_cmd.SUB_GC_QUERY_NEARUSER 			= 111 							--查询附近
chat_cmd.SUB_GC_USER_SHARE 				= 112	 						--分享消息
chat_cmd.SUB_GC_INVITE_PERSONAL 		= 113 							--私人房邀请

chat_cmd.SUB_GC_USER_CHAT_NOTIFY		= 200							--聊天通知
chat_cmd.SUB_GC_APPLYFOR_NOTIFY			= 201							--申请通知
chat_cmd.SUB_GC_RESPOND_NOTIFY			= 202							--回应通知
chat_cmd.SUB_GC_SEARCH_USER_RESULT		= 203							--查找结果
chat_cmd.SUB_GC_INVITE_GAME_NOTIFY		= 204							--邀请通知
chat_cmd.SUB_GC_TRUMPET_NOTIFY			= 205							--喇叭通知
chat_cmd.SUB_GC_DELETE_FRIEND_NOTIFY	= 206							--删除通知
chat_cmd.SUB_GC_UPDATE_COORDINATE_NOTIFY = 209 							--更新坐标
chat_cmd.SUB_GC_GET_NEARUSER_RESULT 	= 210 							--附近结果
chat_cmd.SUB_GC_QUERY_NEARUSER_RESULT 	= 211 							--查询结果
chat_cmd.SUB_GC_USER_SHARE_NOTIFY 		= 212							--分享通知
chat_cmd.SUB_GC_INVITE_PERSONAL_NOTIFY  = 213 							--私人房邀请通知

chat_cmd.SUB_GC_USER_CHAT_ECHO			= 300							--聊天反馈
chat_cmd.SUB_GC_APPLYFOR_FRIEND_ECHO	= 301							--申请反馈
chat_cmd.SUB_GC_RESPOND_FRIEND_ECHO		= 302							--回应反馈
chat_cmd.SUB_GC_SEARCH_USER_ECHO		= 303							--查找反馈
chat_cmd.SUB_GC_INVITE_GAME_ECHO		= 304							--邀请反馈
chat_cmd.SUB_GC_TRUMPET_ECHO			= 305							--喇叭反馈
chat_cmd.SUB_GC_UPDATE_COORDINATE_ECHO 	= 309 							--更新坐标反馈
chat_cmd.SUB_GC_GET_NEARUSER_ECHO 		= 310 							--附近反馈
chat_cmd.SUB_GC_QUERY_NEARUSER_ECHO 	= 311 							--查询反馈
chat_cmd.SUB_GC_USER_SHARE_ECHO 		= 312 							--分享反馈
chat_cmd.SUB_GC_INVITE_PERSONAL_ECHO 	= 313 							--私人房邀请反馈

chat_cmd.SUB_GC_SYSTEM_MESSAGE			= 401							--系统消息
chat_cmd.SUB_GC_OPERATE_SUCCESS			= 800							--操作成功
chat_cmd.SUB_GP_OPERATE_FAILURE			= 801							--操作失败

---------------
------数据长度
---------------
chat_cmd.LEN_COMPELLATION			= 16								--真实名字
chat_cmd.LEN_CONSTELLATION			= 6									--用户星座
chat_cmd.LEN_UNDER_WRITE			= 32								--个性签名
chat_cmd.LEN_BLOODTYPE				= 6									--用户血型
chat_cmd.LEN_BIRTHDAY				= 16								--用户生日
chat_cmd.LEN_SEAT_PHONE				= 33								--固定电话
chat_cmd.LEN_MOBILE_PHONE			= 12								--移动电话
chat_cmd.LEN_POSTALCODE				= 8									--邮政编码
chat_cmd.LEN_DWELLING_PLACE			= 128								--联系地址
chat_cmd.LEN_PHONE_MODE				= 21								--手机型号
chat_cmd.LEN_FONT_NAME				= 16								--字体数据长度
chat_cmd.LEN_MESSAGE_CONTENT		= 128								--数据长度

---好友信息
chat_cmd.tagClientFriendInfo = 
{
	--用户属性
	{t = "dword",	k = "dwUserID"},									--用户ID
	{t = "dword",	k = "dwGameID"},									--游戏id
	{t = "string",	k = "szNickName",s= yl.LEN_ACCOUNTS},				--用户昵称
	{t = "dword",	k = "wFaceID"},										--头像 I D
	{t = "dword",	k = "wCustomID"},									--头像 I D
	{t = "byte",	k = "cbGender"},									--性别	
	{t = "word",	k = "cbMemberOrder"},								--会员等级
	{t = "word",	k = "wGrowLevel"},									--用户等级
	{t = "string",	k = "szSign",s= chat_cmd.LEN_UNDER_WRITE},			--个性签名
	{t = "string",	k = "szCompellation",s= chat_cmd.LEN_COMPELLATION},	--真实姓名	
	{t = "string",	k = "szPhoneMode",s= chat_cmd.LEN_PHONE_MODE},		--手机型号	
	{t = "byte",	k = "cbGroupID"},									--组别标识	

	--在线信息
	{t = "byte",	k = "cbMainStatus"},								--用户状态
	{t = "byte",	k = "cbGameStatus"},								--游戏状态
	{t = "word",	k = "wKindID"},										--游戏标识
	{t = "word",	k = "wServerID"},									--房间标识
	{t = "word",	k = "wTableID"},									--桌子标识
	{t = "word",	k = "wChairID"},									--椅子位置
	{t = "string",	k = "szServerName", s = yl.LEN_SERVER},				--房间名字
}

-- 附近的人
chat_cmd.tagNearUserInfo =
{
	--用户属性
	{t = "dword",	k = "dwUserID"},									--用户ID
	{t = "dword",	k = "dwGameID"},									--游戏id
	{t = "string",	k = "szNickName",s= yl.LEN_ACCOUNTS},				--用户昵称
	{t = "dword",	k = "wFaceID"},										--头像 I D
	{t = "dword",	k = "wCustomID"},									--头像 I D
	{t = "byte",	k = "cbGender"},									--性别	
	{t = "word",	k = "cbMemberOrder"},								--会员等级
	{t = "word",	k = "wGrowLevel"},									--用户等级
	{t = "string",	k = "szSign",s= chat_cmd.LEN_UNDER_WRITE},			--个性签名

	-- 经度纬度
	{t = "double", k = "dLongitude"},                                   -- 坐标经度
	{t = "double", k = "dLatitude"},                                    -- 坐标纬度
	{t = "dword", k = "dwDistance"},                                    -- 目标距离

	-- 用户地址
	{t = "dword", k = "dwClientAddr"},                                  -- ip地址
}

chat_cmd.CMD_GC_UserFriendInfo = 
{	
	{k = "wFriendCount", t = "word"},                            				--好友数目
	{k = "FriendInfo", t = "table", d = chat_cmd.tagClientFriendInfo,l={5}},  	--好友信息
}

--用户上线/离线
chat_cmd.CMD_GC_UserOnlineStatusNotify = 
{
	{t = "dword", 	k = "dwUserID"},											--用户id
	{t = "byte",	k = "cbMainStatus"},										--用户状态
}

--游戏状态
chat_cmd.CMD_GC_UserGameStatusNotify = 
{
	{t = "dword", 	k = "dwUserID"},											--用户id
	{t = "byte", 	k = "cbGameStatus"},										--游戏状态
	{t = "word",	k = "wKindID"},												--游戏标识
	{t = "word", 	k = "wServerID"},											--房间标识
	{t = "word",	k = "wTableID"},											--桌子标识
	{t = "string",	k = "szServerName", s = yl.LEN_SERVER},						--房间名字
}

--用户聊天
chat_cmd.CMD_GC_UserChat = 
{
	{t = "dword",k = "dwSenderID"},							    		  		--用户标识
	{t = "dword",k = "dwTargetUserID"},									  		--目标用户
	{t = "dword",k = "dwFontColor"},									  		--字体颜色
	{t = "byte", k = "cbFontSize"},										  		--字体大小
	{t = "byte", k = "cbFontAttri"},									  		--字体属性
	{t = "string",k = "szFontName", s = chat_cmd.LEN_FONT_NAME},		     	--字体名称
	{t = "string",k = "szMessageContent", s= chat_cmd.LEN_MESSAGE_CONTENT}   	--消息内容	
};

--申请好友
chat_cmd.CMD_GC_ApplyForNotify = 
{
	{t = "dword", k = "dwRequestID"},					--用户标识
	{t = "byte",  k = "cbGroupID"},						--组别标识
	{t = "string",k = "szNickName",s= yl.LEN_ACCOUNTS},	--用户昵称
};

--回应好友
chat_cmd.CMD_GC_RespondFriend = 
{
	{t = "dword", k = "dwUserID"},						--用户标识
	{t = "dword", k = "dwRequestID"},					--用户标识
	{t = "byte",  k = "cbRequestGroupID"},				--组别标识
	{t = "byte",  k = "cbGroupID"},						--组别标识
	{t = "bool",  k = "bAccepted"},						--是否接受
};

--回应通知
chat_cmd.CMD_GC_RespondNotify = 
{
	{t = "string", k = "szNotifyContent",s = 128},		--提示内容
};

--查找好友
chat_cmd.CMD_GC_SearchByGameID = 
{
	{t = "dword", k = "dwSearchByGameID"},								--用户标识
}

--查找结果
chat_cmd.CMD_GC_SearchByGameIDResult = 
{
	{t = "byte",	k = "cbUserCount"},									--用户数目
	{t = "table",	k = "FriendInfo", d = chat_cmd.tagClientFriendInfo} --好友信息
}

--邀请游戏
chat_cmd.CMD_GC_InviteGame = 
{
	{t = "dword", 	k = "dwUserID"},									--邀请用户
	{t = "dword", 	k = "dwInvitedUserID"},								--被邀用户
	{t = "word", 	k = "wKindID"}, 									--游戏标识
	{t = "word", 	k = "wServerID"},									--房间标识
	{t = "word", 	k = "wTableID"},									--桌子标识
	{t = "tchar", 	k = "szInviteMsg", s = 128}							--邀请信息
}

-- 邀请反馈
chat_cmd.CMD_GC_InviteGameEcho = 
{
	{t = "int",		k = "lErrorCode"}, 									-- 错误代码
	{t = "string",  k = "szDescribeString"} 							-- 描述信息
}

--邀请通知
chat_cmd.CMD_GC_InviteGameNotify = 
{
	{t = "dword", 	k = "dwSenderID"},--dwInviteUserID					--邀请用户
	{t = "word", 	k = "wKindID"}, 									--游戏标识
	{t = "word",  	k = "wServerID"},									--房间标识
	{t = "word", 	k = "wTableID"},									--桌子标识
	{t = "string", 	k = "szInviteMsg", s = 128}							--邀请信息
}

--私人房邀请游戏
chat_cmd.CMD_GC_InvitePersonalGame = 
{
	{t = "dword", 	k = "dwUserID"},									--邀请用户
	{t = "dword", 	k = "dwInvitedUserID"},								--被邀用户
	{t = "word", 	k = "wKindID"}, 									--游戏标识
	{t = "dword", 	k = "dwServerNumber"},								--房间号码
	{t = "word", 	k = "wTableID"},									--桌子标识
	{t = "tchar", 	k = "szInviteMsg", s = 128}							--邀请信息
}

-- 邀请反馈
chat_cmd.CMD_GC_InvitePersonalGameEcho = 
{
	{t = "int",		k = "lErrorCode"}, 									-- 错误代码
	{t = "string",  k = "szDescribeString"} 							-- 描述信息
}

--邀请通知
chat_cmd.CMD_GC_InvitePersonalGameNotify = 
{
	{t = "dword", 	k = "dwSenderID"},--dwInviteUserID					--邀请用户
	{t = "word", 	k = "wKindID"}, 									--游戏标识
	{t = "dword",  	k = "dwServerNumber"},								--房间号码
	{t = "word", 	k = "wTableID"},									--桌子标识
	{t = "string", 	k = "szInviteMsg", s = 128}							--邀请信息
}

--用户喇叭(发包结构)
chat_cmd.CMD_GC_Trumpet_C = 
--[[{
	{t = "dword",	k = "dwSenderID"},									--用户标识
	{t = "tchar",	k = "szNickName", s = yl.LEN_NICKNAME},				--用户昵称
	{t = "dword", 	k = "dwFontColor"},									--字体颜色
	{t = "byte", 	k = "cbFontSize"},									--字体大小
	{t = "byte",	k = "cbFontAttri"},									--字体属性
	{t = "tchar",	k = "szFontName",s = chat_cmd.LEN_FONT_NAME},		--字体名称
	{t = "tchar",	k = "szMessageContent", s= chat_cmd.LEN_MESSAGE_CONTENT},--消息内容
}]]
{
	{t = "word",	k = "wPropertyID"},									--道具索引
	{t = "dword",	k = "dwSenderID"},									--用户标识
	{t = "dword", 	k = "dwFontColor"},									--字体颜色
	{t = "tchar",	k = "szNickName", s = yl.LEN_NICKNAME},				--用户昵称
	{t = "tchar",	k = "szMessageContent", s= chat_cmd.LEN_MESSAGE_CONTENT},--消息内容
}

--用户喇叭(回包结构)
chat_cmd.CMD_GC_Trumpet_S = 
--[[{
	{t = "dword",	k = "dwSenderID"},									--用户标识
	{t = "string",	k = "szNickName", s = yl.LEN_NICKNAME},				--用户昵称
	{t = "dword", 	k = "dwFontColor"},									--字体颜色
	{t = "byte", 	k = "cbFontSize"},									--字体大小
	{t = "byte",	k = "cbFontAttri"},									--字体属性
	{t = "string",	k = "szFontName",s = chat_cmd.LEN_FONT_NAME},		--字体名称
	{t = "string",	k = "szMessageContent", s= chat_cmd.LEN_MESSAGE_CONTENT},--消息内容
}]]
{
	{t = "word",	k = "wPropertyID"},									--道具索引
	{t = "dword",	k = "dwSenderID"},									--用户标识
	{t = "dword", 	k = "dwFontColor"},									--字体颜色
	{t = "string",	k = "szNickName", s = yl.LEN_NICKNAME},				--用户昵称
	{t = "string",	k = "szMessageContent", s= chat_cmd.LEN_MESSAGE_CONTENT},--消息内容
}

--喇叭通知/喇叭反馈
chat_cmd.CMD_GC_TrumpetEcho = 
{
	{t = "int", 	k = "lErrorCode"},									--错误代码
	{t = "string",	k = "szDescribeString", s = 128}					--描述消息
}

--删除好友
chat_cmd.CMD_GC_DeleteFriend = 
{
	{t = "dword", 	k = "dwUserID"},									--用户标识(自己)
	{t = "dword",	k = "dwFriendUserID"},								--用户标识(好友)
	{t = "byte",	k = "cbGroupID"},									--组别标识
}

--删除反馈
chat_cmd.CMD_GC_DeleteFriendEcho = 
{
	{t = "int", 	k = "lErrorCode"},									--错误代码
	{t = "string",	k = "szDescribeString", s = 128}					--描述消息
}

--删除结果
chat_cmd.CMD_GC_DeleteFriendNotify = 
{
	{t = "dword", k = "dwFriendUserID"},								--用户标识
}

-- 更新坐标
chat_cmd.CMD_GC_Update_Coordinate = 
{
	{t = "dword", k = "dwUserID"},										--用户标识
	{t = "double", k = "dLongitude"}, 									--坐标经度
	{t = "double", k = "dLatitude"}, 									--坐标纬度
}

-- 坐标反馈
chat_cmd.CMD_GC_Update_CoordinateEcho = 
{
	{t = "int", 	k = "lErrorCode"},									--错误代码
	{t = "string",	k = "szDescribeString", s = 128}					--描述消息
}

-- 坐标通知
chat_cmd.CMD_GC_Update_CoordinateNotify = 
{
	{t = "double", k = "dLongitude"}, 									--坐标经度
	{t = "double", k = "dLatitude"}, 									--坐标纬度
	{t = "dword", k = "dwClientAddr"}, 									--用户地址
}

-- 获取附近
chat_cmd.CMD_GC_Get_Nearuser = 
{
	{t = "dword", k = "dwUserID"},										--用户标识
	{t = "double", k = "dLongitude"}, 									--坐标经度
	{t = "double", k = "dLatitude"}, 									--坐标纬度
}

-- 附近反馈
chat_cmd.CMD_GC_Get_NearuserEcho = 
{
	{t = "int", 	k = "lErrorCode"},									--错误代码
	{t = "string",	k = "szDescribeString", s = chat_cmd.LEN_MESSAGE_CONTENT}					--描述消息
}

-- 附近结果
chat_cmd.CMD_GC_Get_NearuserResult = 
{
	{t = "byte", k = "cbUserCount"},									--用户数目
	-- tagNearUserInfo
}

-- 查询附近
chat_cmd.CMD_GC_Query_Nearuser = 
{
	{t = "dword", k = "dwUserID"},										--用户标识
	{t = "dword", k = "dwNearuserUserID"},								--附近用户
}

-- 查询反馈
chat_cmd.CMD_GC_Query_NearuserEcho = 
{
	{t = "int", 	k = "lErrorCode"},									--错误代码
	{t = "string",	k = "szDescribeString", s = chat_cmd.LEN_MESSAGE_CONTENT}					--描述消息
}

-- 查询结果
chat_cmd.CMD_GC_Query_NearuserResult = 
{
	{t = "byte", k = "cbUserCount"},									--用户数目
	-- tagNearUserInfo
	{k = "NearUserInfo", t = "table", d = chat_cmd.tagNearUserInfo}, 	--用户信息
}

-- 分享消息
chat_cmd.CMD_GC_UserShare = 
{
	{t = "dword", k = "dwUserID"}, 										-- 用户标识
	{t = "dword", k = "dwSharedUserID"},								-- 被分享用户
	{t = "tchar", k = "szShareImageAddr", s = chat_cmd.LEN_MESSAGE_CONTENT}, -- 图片地址
	{t = "tchar", k = "szMessageContent", s = chat_cmd.LEN_MESSAGE_CONTENT} -- 消息内容
}

-- 分享反馈
chat_cmd.CMD_GC_InviteGameEcho = 
{
	{t = "int",		k = "lErrorCode"}, 									-- 错误代码
	{t = "string",  k = "szDescribeString"} 							-- 描述信息
}

--分享通知
chat_cmd.CMD_GC_UserShareNotify = 
{
	{t = "dword", 	k = "dwSenderID"},--dwShareUserID						--用户标识
	{t = "string", k = "szShareImageAddr", s = chat_cmd.LEN_MESSAGE_CONTENT}, -- 图片地址
	{t = "string", k = "szMessageContent", s = chat_cmd.LEN_MESSAGE_CONTENT} -- 消息内容
}

--操作失败
chat_cmd.CMD_GC_OperateFailure = 
{
	{t = "int",		k = "lErrorCode"},									--错误代码
	{t = "int",		k = "lResultCode"},									--操作代码
	{t = "string",	k = "szDescribeString", s = chat_cmd.LEN_MESSAGE_CONTENT}					--错误信息
}

--操作成功
chat_cmd.CMD_GC_OperateSuccess = 
{
	{t = "int",		k = "lResultCode"},									--操作代码
	{t = "string",	k = "szDescribeString", s = chat_cmd.LEN_MESSAGE_CONTENT}					--错误信息
}

--通用反馈消息
chat_cmd.CMD_GC_ECHO = 
{
	{t = "int", 	k = "lErrorCode"},									--错误代码
	{t = "string",	k = "szDescribeString"}					--描述消息
}

return chat_cmd