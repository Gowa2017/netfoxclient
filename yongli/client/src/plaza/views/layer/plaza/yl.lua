yl = yl or {}

yl.WIDTH								= 1334--1136--1334
yl.HEIGHT								= 750--640--750

yl.DEVICE_TYPE							= 0x10
yl.KIND_ID								= 122
yl.STATION_ID							= 1

--测试
yl.LOGONSERVER							="134.175.154.178"--"103.60.167.154"--"game.beiyoub.top"--"game.miankee.top"--"47.107.119.160"--"47.99.108.20"----"game.yoohg.xyz"
yl.SERVER_LIST = 
{
    "134.175.154.178"
    --"103.60.167.154"
--"game.miankee.top"
    --"game.beiyoub.top"
   --"game.yoohg.xyz"
    --"47.99.108.20"
  --"47.107.119.160"
}
yl.PAY_SERVER ="http://134.175.154.178:8888"
yl.GAME_SHISHICAI ="http://134.175.154.178:8889"
-- 当前地址索引
yl.CURRENT_INDEX = 1
yl.TOTAL_COUNT = #yl.SERVER_LIST
yl.LOGONPORT							= 8600 --@login_port
yl.FRIENDPORT							= 8630 --@friend_port
--编译码
yl.VALIDATE 							= "B3D44854-9C2F-4C78-807F-8C08E940166D" --@compilation

-- http请求支持(loginScene)
yl.HTTP_SUPPORT							= true
-- 是否显示信息弹窗的ip和位置信息
yl.SHOW_IP_ADDRESS                      = true
-- 是否单游戏模式(游戏列表数目为1生效)
yl.SINGLE_GAME_MODOLE                   = true

yl.DOWN_PRO_INFO						= 1
yl.DOWN_COMPELETED						= 2
yl.DOWN_ERROR_PATH						= 3 									--路径出错
yl.DOWN_ERROR_CREATEFILE				= 4 									--文件创建出错
yl.DOWN_ERROR_CREATEURL					= 5 									--创建连接失败
yl.DOWN_ERROR_NET		 				= 6 									--下载失败
yl.DOWN_FILE_ERROR 						= 7

yl.SCENE_GAMELIST 						= 1
yl.SCENE_ROOMLIST 						= 2
yl.SCENE_USERINFO 						= 3
yl.SCENE_BANK							= 4
yl.SCENE_OPTION							= 5
yl.SCENE_RANKINGLIST					= 6
yl.SCENE_BANKRECORD						= 8
yl.SCENE_CASHRECORD						= 9
yl.SCENE_ORDERRECORD					= 10
yl.SCENE_EVERYDAY						= 11
yl.SCENE_CHECKIN						= 12
yl.SCENE_TASK							= 13
yl.SCENE_AGENT							= 14
yl.SCENE_ROOM  							= 20
yl.SCENE_GAME 							= 21
yl.SCENE_SHOP							= 22
yl.SCENE_SHOPDETAIL						= 23
yl.SCENE_BAG							= 24
yl.SCENE_BAGDETAIL						= 25
yl.SCENE_BAGTRANS						= 26
yl.SCENE_BINDING						= 27
yl.SCENE_FRIEND							= 28
yl.SCENE_MODIFY							= 29		--密码修改
yl.SCENE_TABLE							= 30		--转盘界面
yl.SCENE_FEEDBACK					 	= 31		--反馈界面
yl.SCENE_FEEDBACKLIST				 	= 32		--反馈列表
yl.SCENE_FAQ							= 33		--常见问题
yl.SCENE_BINDINGREG						= 34		--绑定注册
yl.SCENE_ACTIVITY						= 35		--活动
yl.SCENE_EARN							= 36		--赚金
yl.SCENE_PRIBRAND						= 37		--约牌
yl.SCENE_PRIGAMELIST					= 38		--约牌游戏列表
yl.SCENE_SHARE							= 39

yl.SCENE_EX_END 						= 50

yl.MAIN_SOCKET_INFO						= 0

yl.SUB_SOCKET_CONNECT					= 1
yl.SUB_SOCKET_ERROR						= 2
yl.SUB_SOCKET_CLOSE						= 3

yl.US_NULL								= 0x00		--没有状态
yl.US_FREE								= 0x01		--站立状态
yl.US_SIT								= 0x02		--坐下状态
yl.US_READY								= 0x03		--同意状态
yl.US_LOOKON							= 0x04		--旁观状态
yl.US_PLAYING					 		= 0x05		--游戏状态
yl.US_OFFLINE							= 0x06		--断线状态

yl.INVALID_TABLE						= 65535
yl.INVALID_CHAIR						= 65535
yl.INVALID_ITEM							= 65535
yl.INVALID_USERID						= 0			--无效用户
yl.INVALID_BYTE                         = 255
yl.INVALID_WORD                         = 65535

--头像信息
yl.FACE_CX 								= 48
yl.FACE_CY								= 48

yl.GENDER_FEMALE						= 0				--女性性别
yl.GENDER_MANKIND						= 1				--男性性别

yl.GAME_GENRE_GOLD						= 0x0001		--金币类型
yl.GAME_GENRE_SCORE						= 0x0002		--点值类型
yl.GAME_GENRE_MATCH						= 0x0004		--比赛类型
yl.GAME_GENRE_EDUCATE					= 0x0008		--训练类型
yl.GAME_GENRE_PERSONAL 					= 0x0010 		-- 约战类型

yl.SERVER_GENRE_PASSWD					= 0x0002 		-- 密码类型

yl.SR_ALLOW_AVERT_CHEAT_MODE			= 0x00000040	--隐藏信息


yl.KIND_SUPPORT_FANKA					= 0x0001		--房卡类型
yl.KIND_SUPPORT_GOLD					= 0x0002		--金币类型
yl.KIND_SUPPORT_BAIREN					= 0x0004		--百人类型
yl.KIND_SUPPORT_JINGJI					= 0x0008		--竞技类型

yl.LEN_GAME_SERVER_ITEM					= 187			--房间长度
yl.LEN_GAME_KIND_ITEM					= 162			--类型长度
--yl.LEN_GAME_SERVER_ITEM					= 177			--房间长度
yl.LEN_TASK_PARAMETER					= 813			--任务长度
yl.LEN_TASK_STATUS						= 5             --任务长度

yl.LEN_MD5								= 33			--加密密码
yl.LEN_ACCOUNTS							= 32			--帐号长度
yl.LEN_NICKNAME							= 32			--昵称长度
yl.LEN_PASSWORD							= 33			--密码长度
yl.LEN_USER_UIN							= 33
yl.LEN_QQ                               = 16            --Q Q 号码
yl.LEN_EMAIL                            = 33            --电子邮件
yl.LEN_COMPELLATION 					= 16			--真实名字
yl.LEN_SEAT_PHONE                       = 33            --固定电话
yl.LEN_MOBILE_PHONE                     = 12            --移动电话
yl.LEN_PASS_PORT_ID                     = 19            --证件号码
yl.LEN_COMPELLATION                     = 16            --真实名字
yl.LEN_DWELLING_PLACE                   = 128           --联系地址
yl.LEN_UNDER_WRITE                      = 32            --个性签名
yl.LEN_PHONE_MODE                       = 21            --手机型号
yl.LEN_SERVER                           = 32            --房间长度
yl.LEN_TRANS_REMARK						= 32			--转账备注
yl.LEN_TASK_NAME						= 64			--任务名称
yl.LEN_IP								= 32			--ip地址
yl.LEN_VERIFY_CODE 						= 8				--验证码长度
yl.LEN_WHERE_PARM                      	= 32            --操作参数
yl.LEN_REAL_NAME 						= 32			--真实姓名
yl.LEN_BANK_NO 							= 32			--银行卡号
yl.LEN_BANK_NAME 						= 32			--银行名称
yl.LEN_BANK_ADDRESS 					= 128			--银行开户地址
yl.LEN_ALIPAY_ID 						= 128			--支付宝
yl.LEN_ORDER_ID							= 64			--订单号长度
yl.LEN_AGENT_ID							= 20			--代理号长度
yl.LEN_DESC								= 128			--描述长度
yl.LEN_IMAGE_NAME						= 32			--文件名长度
yl.LEN_ALIPAY							= 32			--支付宝账号
yl.LEN_WECHAT							= 16			--微信账号长度

yl.LEN_MOBILE_PHONE						= 12			--移动电话
yl.LEN_COMPELLATION						= 16			--真实名字
yl.LEN_MACHINE_ID						= 33			--序列长度
yl.LEN_USER_CHAT						= 128			--聊天长度
yl.SOCKET_TCP_BUFFER					= 16384			--网络缓冲

yl.MDM_GP_LOGON							= 1				--广场登录
yl.MDM_MB_LOGON							= 100			--广场登录

yl.SUB_MB_LOGON_ACCOUNTS				= 2				--帐号登录
yl.SUB_MB_REGISTER_ACCOUNTS 			= 3				--注册帐号
yl.SUB_MB_LOGON_VISITOR					= 5             --游客登录
yl.SUB_MB_PHONE_REGISTER_VERIFYCODE     = 6				--手机号注册
yl.SUB_MB_PHONE_GET_REGISTER_VERIFYCODE = 7				--获取手机号验证码
yl.SUB_MB_GET_MODIFY_PASS_VERIFYCODE    = 8				--获取设置密码验证码
yl.SUB_MB_MODIFY_PASS_VERIFYCODE        = 9				--设置密码
yl.SUB_MB_QUERY_USERDATA 				= 10				--查询用户信息
yl.SUB_MB_TOUSU_AGENT 					= 11				--投诉代理
yl.SUB_MB_SHENGQING_AGENT_CHECK			= 12			--申请代理检查条件
yl.SUB_MB_SHENGQING_AGENT				= 13			--申请代理
yl.SUB_MB_QUERY_GAMELOCKINFO			= 14			--查询游戏锁表信息
yl.SUB_MB_GET_MODIFY_BANKINFO_VERIFYCODE= 15			--获取修改银行信息验证码
yl.SUB_MB_MODIFY_BANKINFO_VERIFYCODE	= 16							--修改银行信息

yl.SUB_MB_LOGON_SUCCESS					= 100			--登录成功
yl.SUB_MB_LOGON_FAILURE					= 101			--登录失败
yl.SUB_MB_UPDATE_NOTIFY					= 200			--升级提示

--列表主命令掩码
yl.MDM_MB_SERVER_LIST					= 101			--列表信息
--查询消息
yl.SUB_QUEREY_SERVER					= 1				--查询房间列表
--结果消息
yl.SUB_MB_LIST_SERVER_START				= 1				--开始发送列表通知
yl.SUB_MB_LIST_SERVER_END				= 2				--结束发送列表通知
yl.SUB_MB_LIST_KIND						= 100			--种类列表
yl.SUB_MB_LIST_SERVER					= 101			--房间列表
yl.SUB_MB_LIST_FINISH					= 200			--列表完成
yl.SUB_MB_AGENT_KIND					= 400			--代理列表


yl.MDM_GP_USER_SERVICE					= 3 			--用户服务

yl.SUB_GP_MODIFY_LOGON_PASS				= 101			--修改密码
yl.SUB_GP_MODIFY_INSURE_PASS			= 102			--修改密码
yl.SUB_GP_MODIFY_UNDER_WRITE			= 103			--修改签名
yl.SUB_GP_VERIFY_CODE_RESULT			= 108			--验证码结果

--修改头像
yl.SUB_GP_USER_FACE_INFO				= 120			--头像信息
yl.SUB_GP_SYSTEM_FACE_INFO				= 122			--系统头像
yl.SUB_GP_CUSTOM_FACE_INFO				= 123			--自定义头像

yl.SUB_GP_USER_INDIVIDUAL				= 140			--个人资料
yl.SUB_GP_QUERY_INDIVIDUAL				= 141			--查询信息
yl.SUB_GP_MODIFY_INDIVIDUAL				= 152			--修改资料

yl.SUB_GP_USER_ENABLE_INSURE		    = 160			--开通银行
yl.SUB_GP_USER_SAVE_SCORE				= 161			--存款操作
yl.SUB_GP_USER_TAKE_SCORE				= 162			--取款操作
yl.SUB_GP_USER_INSURE_INFO				= 164			--银行资料
yl.SUB_GR_USER_TRANSFER_SCORE  			= 163 			--转帐操作
yl.SUB_GP_QUERY_INSURE_INFO				= 165			--查询银行
yl.SUB_GP_USER_INSURE_SUCCESS			= 166			--银行成功
yl.SUB_GP_USER_INSURE_FAILURE			= 167			--银行失败
yl.SUB_GP_USER_INSURE_ENABLE_RESULT 	= 170			--开通结果

yl.SUB_GP_OPERATE_SUCCESS				= 500			--操作成功
yl.SUB_GP_OPERATE_FAILURE				= 501			--操作失败

yl.DTP_GP_UI_NICKNAME					= 2				--用户昵称
yl.DTP_GP_UI_UNDER_WRITE				= 3				--个性签名
yl.DTP_GP_MODIFY_UNDER_WRITE			= 4

yl.DTP_GP_MEMBER_INFO					= 2 			--会员信息

yl.SUB_GP_GROWLEVEL_QUERY				= 300			--查询等级
yl.SUB_GP_GROWLEVEL_PARAMETER			= 301			--等级参数
yl.SUB_GP_GROWLEVEL_UPGRADE				= 302			--等级升级

yl.SUB_GP_TASK_LOAD						= 240			--任务加载
yl.SUB_GP_TASK_TAKE						= 241			--任务领取
yl.SUB_GP_TASK_REWARD					= 242			--任务奖励
yl.SUB_GP_TASK_GIVEUP					= 243			--任务放弃
yl.SUB_GP_TASK_INFO						= 250			--任务信息
yl.SUB_GP_TASK_LIST						= 251			--任务信息
yl.SUB_GP_TASK_RESULT					= 252			--任务结果
yl.SUB_GP_TASK_GIVEUP_RESULT			= 253			--放弃结果

--低保服务
yl.SUB_GP_BASEENSURE_LOAD				= 260			--加载低保
yl.SUB_GP_BASEENSURE_TAKE				= 261			--领取低保
yl.SUB_GP_BASEENSURE_PARAMETER			= 262			--低保参数
yl.SUB_GP_BASEENSURE_RESULT				= 263			--低保结果

--帐号绑定
yl.SUB_GP_ACCOUNT_BINDING				= 380			--注册绑定
yl.SUB_GP_ACCOUNT_BINDING_EXISTS		= 381			--帐号绑定
yl.SUB_GP_IP_LOCATION					= 382			--IP归属地

yl.MDM_GR_LOGON							= 1				--登录信息
yl.SUB_GR_LOGON_MOBILE					= 2				--手机登录

yl.SUB_GR_LOGON_SUCCESS					= 100			--登录成功
yl.SUB_GR_LOGON_FAILURE					= 101			--登录失败
yl.SUB_GR_LOGON_FINISH					= 102			--登录完成

yl.SUB_GR_UPDATE_NOTIFY					= 200			--升级提示


yl.MDM_GR_CONFIG						= 2				--配置信息
yl.SUB_GR_CONFIG_COLUMN					= 100			--列表配置
yl.SUB_GR_CONFIG_SERVER					= 101			--房间配置
yl.SUB_GR_CONFIG_FINISH					= 102			--配置完成

yl.MDM_GR_USER							= 3				--用户信息

yl.SUB_GR_USER_SITDOWN					= 3				--坐下请求
yl.SUB_GR_USER_STANDUP					= 4				--起立请求

yl.SUB_GR_USER_CHAIR_REQ 	   			= 10 			--请求更换位置
yl.SUB_GR_USER_CHAIR_INFO_REQ 	 		= 11 			--请求椅子用户信息

yl.SUB_GR_USER_ENTER					= 100			--用户进入
yl.SUB_GR_USER_SCORE					= 101			--用户分数
yl.SUB_GR_USER_STATUS					= 102			--用户状态
yl.SUB_GR_REQUEST_FAILURE				= 103			--请求失败
yl.SUB_GR_USER_GAME_DATA				= 104			--用户游戏数据

yl.SUB_GR_USER_CHAT						= 201			--聊天消息
yl.SUB_GR_USER_EXPRESSION				= 202			--表情消息
yl.SUB_GR_WISPER_CHAT					= 203			--私聊消息
yl.SUB_GR_WISPER_EXPRESSION				= 204			--私聊表情
yl.SUB_GR_COLLOQUY_CHAT					= 205			--会话消息
yl.SUB_GR_COLLOQUY_EXPRESSION			= 206			--会话表情

yl.MDM_GF_FRAME							= 100    		--框架命令

yl.SUB_GF_GAME_OPTION					= 1				--游戏配置
yl.SUB_GF_USER_READY					= 2				--用户准备
yl.SUB_GF_LOOKON_CONFIG					= 3				--旁观配置

yl.SUB_GF_GAME_STATUS					= 100			--游戏状态
yl.SUB_GF_GAME_SCENE					= 101			--游戏场景
yl.SUB_GF_LOOKON_STATUS					= 102			--旁观状态
yl.SUB_GF_USER_DATA						= 103			--玩家数据

yl.SUB_GF_SYSTEM_MESSAGE				= 200			--系统消息
yl.SUB_GF_ACTION_MESSAGE				= 201			--动作消息

yl.MDM_GR_STATUS						= 4				--状态信息

yl.SUB_GR_TABLE_INFO					= 100			--桌子信息
yl.SUB_GR_TABLE_STATUS					= 101			--桌子状态

yl.MDM_GF_GAME							= 200			--游戏命令

yl.DTP_GR_TABLE_PASSWORD				= 1				--桌子密码
yl.DTP_GR_NICK_NAME						= 10			--用户昵称
yl.DTP_GR_UNDER_WRITE					= 12 			--个性签名

yl.REQUEST_FAILURE_NORMAL				= 0				--常规原因
yl.REQUEST_FAILURE_NOGOLD				= 1				--金币不足
yl.REQUEST_FAILURE_NOSCORE				= 2				--积分不足
yl.REQUEST_FAILURE_PASSWORD				= 3				--密码错误

yl.MDM_CM_SYSTEM						= 1000			--系统命令

ylSUB_CM_SYSTEM_MESSAGE					= 1				--系统消息
ylSUB_CM_ACTION_MESSAGE					= 2				--动作消息

yl.SMT_CHAT								= 0x0001		--聊天消息
yl.SMT_EJECT							= 0x0002		--弹出消息
yl.SMT_GLOBAL							= 0x0004		--全局消息
yl.SMT_PROMPT							= 0x0008		--提示消息
yl.SMT_TABLE_ROLL						= 0x0010		--滚动消息

yl.SMT_CLOSE_ROOM						= 0x0100		--关闭房间
yl.SMT_CLOSE_GAME						= 0x0200		--关闭游戏
yl.SMT_CLOSE_LINK						= 0x0400		--中断连接
yl.SMT_CLOSE_INSURE						= 0x0800		--关闭银行

--任务类型
yl.TASK_TYPE_WIN_INNINGS				= 0x01			--赢局任务
yl.TASK_TYPE_SUM_INNINGS				= 0x02			--总局任务
yl.TASK_TYPE_FIRST_WIN					= 0x04			--首胜任务
yl.TASK_TYPE_JOIN_MATCH					= 0x08			--比赛任务

--任务状态
yl.TASK_STATUS_UNFINISH					= 0				--任务状态
yl.TASK_STATUS_SUCCESS					= 1				--任务成功
yl.TASK_STATUS_FAILED					= 2				--任务失败	
yl.TASK_STATUS_REWARD					= 3				--领取奖励
yl.TASK_STATUS_WAIT						= 4 			--等待领取

--任务数量
yl.TASK_MAX_COUNT						= 128			--任务数量

--签到服务
yl.SUB_GP_CHECKIN_QUERY					= 220			--查询签到
yl.SUB_GP_CHECKIN_INFO					= 221			--签到信息
yl.SUB_GP_CHECKIN_DONE					= 222			--执行签到
yl.SUB_GP_CHECKIN_RESULT				= 223			--签到结果

--会员服务
yl.SUB_GP_MEMBER_PARAMETER				= 340			--会员参数
yl.SUB_GP_MEMBER_QUERY_INFO				= 341			--会员查询
yl.SUB_GP_MEMBER_DAY_PRESENT			= 342			--会员送金
yl.SUB_GP_MEMBER_DAY_GIFT				= 343			--会员礼包
yl.SUB_GP_MEMBER_PARAMETER_RESULT		= 350			--参数结果
yl.SUB_GP_MEMBER_QUERY_INFO_RESULT		= 351			--查询结果
yl.SUB_GP_MEMBER_DAY_PRESENT_RESULT		= 352			--送金结果
yl.SUB_GP_MEMBER_DAY_GIFT_RESULT		= 353			--礼包结果

--道具命令
yl.MDM_GP_PROPERTY						= 6	

--道具信息
yl.SUB_GP_QUERY_PROPERTY				= 1				--道具查询
yl.SUB_GP_PROPERTY_BUY					= 2				--购买道具
yl.SUB_GP_PROPERTY_USE					= 3				--道具使用
yl.SUB_GP_QUERY_BACKPACKET				= 4				--背包查询
yl.SUB_GP_QUERY_SEND_PRESENT			= 6				--查询赠送
yl.SUB_GP_PROPERTY_PRESENT				= 7				--赠送道具
yl.SUB_GP_GET_SEND_PRESENT				= 8				--获取赠送

yl.SUB_GP_QUERY_PROPERTY_RESULT			= 101			--道具查询
yl.SUB_GP_PROPERTY_BUY_RESULT			= 102			--购买道具
yl.SUB_GP_PROPERTY_USE_RESULT			= 103			--道具使用
yl.SUB_GP_QUERY_BACKPACKET_RESULT		= 104			--背包查询
yl.SUB_GP_QUERY_SEND_PRESENT_RESULT		= 106			--查询赠送
yl.SUB_GP_PROPERTY_PRESENT_RESULT		= 107			--赠送道具
yl.SUB_GP_GET_SEND_PRESENT_RESULT		= 108			--获取赠送
yl.SUB_GP_PROPERTY_FAILURE				= 404			--道具失败

yl.LEN_WEEK								= 7

--货币类型
yl.CONSUME_TYPE_GOLD					= 0x01			--游戏币
yl.CONSUME_TYPE_USEER_MADEL				= 0x02			--元宝
yl.CONSUME_TYPE_CASH					= 0x03			--游戏豆
yl.CONSUME_TYPE_LOVELINESS				= 0x04			--魅力值

--发行范围
yl.PT_ISSUE_AREA_PLATFORM				= 0x01			--大厅道具（大厅可以使用）
yl.PT_ISSUE_AREA_SERVER					= 0x02			--房间道具（在房间可以使用）
yl.PT_ISSUE_AREA_GAME					= 0x04			--游戏道具（在玩游戏时可以使用）
--喇叭物品
yl.LARGE_TRUMPET                        = 306           --大喇叭id
yl.SMALL_TRUMPET                        = 307           --小喇叭id

--赠送目标类型
yl.PRESEND_NICKNAME						= 0
yl.PRESEND_GAMEID						= 1

--notifycation
yl.RY_USERINFO_NOTIFY					= "ry_userinfo_notify"		--玩家信息更新通知
yl.RY_MSG_USERWEALTH					= 102						--更新用户财富
yl.RY_CHAT_LOGIN_SUCCESS_NOTIFY			= "ry_chat_login_success_notify" --登录聊天服务器成功

yl.RY_FRIEND_NOTIFY                     = "ry_friend_notify"        --好友更新
yl.RY_MSG_FRIENDDEL                     = 101                       --好友删除

yl.TRUMPET_COUNT_UPDATE_NOTIFY			= "ry_trumpet_count_update" --喇叭数量更新

yl.RY_NEARUSER_NOTIFY                   = "ry_nearuser_notify"      --附近玩家信息获取
yl.RY_IMAGE_DOWNLOAD_NOTIFY             = "ry_image_download_notify"--图片下载结束
yl.RY_UPDATE_FACE_NOTIFY             = "ry_update_face_notify"--头像刷新通知

yl.RY_UPDATE_SERVERINFO_NOTIFY		= "RY_UPDATE_SERVERINFO_NOTIFY"	--更新房间信息


yl.RY_SELECT_PRIVATEROOM_NOTIFY				= "RY_SELECT_PRIVATEROOM_NOTIFY"		--选择私人房
yl.RY_CREATE_PRIVATEROOM_NOTIFY				= "RY_CREATE_PRIVATEROOM_NOTIFY"		--创建私人房
yl.RY_SELECT_PRIVATELIST_NOTIFY				= "RY_SELECT_PRIVATELIST_NOTIFY"		--选中的私人房游戏

yl.RY_GET_GAME_VERSION_NOTIFY       = "RY_GET_GAME_VERSION_NOTIFY"	--获取游戏版本号
yl.RY_SHOW_VIEW_GAME_KIND			= "RY_SHOW_VIEW_GAME_KIND"		--当前显示的界面
yl.RY_START_LOGON					= "RY_START_LOGON"				--开始执行登录

--后台切换类型
yl.APP_NONE =			0			--初始状态
yl.APP_FOCUS =			1			--其他顶层activity在覆盖cocos2d activity
yl.APP_UNFOCUS =		2			--cocos2d activity跳转到其他层activity(暂未实现)
yl.APP_FOREGROUND =		3			--进入前台
yl.APP_BACKGROUND =		4			--进入后台


--登录类型
yl.LOGON_MODE_ACCOUNTS			= 0			--账号登录
yl.LOGON_MODE_REGISTER_ACCOUNTS	= 1			--账号注册登录
yl.LOGON_MODE_REGISTER_PHONE	= 2			--手机注册登录
yl.LOGON_MODE_VISITORS			= 3			--游客登录
yl.LOGON_MODE_WECHAT			= 4			--微信登录

--支付渠道常量
yl.PAYMENT_LE_wypays	= 1		--网银非直联
yl.PAYMENT_ALIPAY		= 2		--支付宝支付渠道
yl.PAYMENT_WECHAT		= 3		--微信支付渠道
yl.PAYMENT_ALIPAY_SM	= 4		--支付宝扫码扫码支付渠道
yl.PAYMENT_WECHAT_SM	= 5		--微信扫码扫码支付渠道
yl.PAYMENT_QQ			= 6		--QQ支付渠道
yl.PAYMENT_QQ_SM		= 8		--QQ扫码扫码支付渠道
yl.PAYMENT_QUICK		= 7		--快捷支付渠道
yl.PAYMENT_JINDONG		= 9		--京东支付渠道
yl.PAYMENT_BAIDU		= 10	--百度支付渠道
yl.PAYMENT_VIP			= 11	--代理充值
yl.PAYMENT_APPLE		= 12	--苹果内购
yl.PAYMENT_CZF_ZF		= 13	--畅支付
yl.PAYMENT_LE_ICBC		= 14	--工商银行
yl.PAYMENT_LE_ABC		= 15	--农业银行
yl.PAYMENT_LE_CCB		= 16	--建设银行
yl.PAYMENT_LE_BOC		= 17	--中国银行
yl.PAYMENT_LE_CMB		= 18	--招商银行
yl.PAYMENT_LE_BCCB		= 19	--北京银行
yl.PAYMENT_LE_BOCO		= 20	--交通银行
yl.PAYMENT_LE_CIB		= 21	--兴业银行
yl.PAYMENT_LE_NJCB		= 22	--南京银行
yl.PAYMENT_LE_CMBC		= 23	--民生银行
yl.PAYMENT_LE_CEB		= 24	--光大银行
yl.PAYMENT_LE_PINGANBANK= 25	--平安银行
yl.PAYMENT_LE_CBHB		= 26	--渤海银行
yl.PAYMENT_LE_HKBEA		= 27	--东亚银行
yl.PAYMENT_LE_NBCB		= 28	--宁波银行
yl.PAYMENT_LE_CTTIC		= 29	--中信银行
yl.PAYMENT_LE_GDB		= 30	--广发银行
yl.PAYMENT_LE_SHB		= 31	--上海银行
yl.PAYMENT_LE_SPDB		= 32	--上海浦东发展银行
yl.PAYMENT_LE_PSBS		= 33	--中国邮政
yl.PAYMENT_LE_HXB		= 34	--华夏银行
yl.PAYMENT_LE_BJRCB		= 35	--北京农村商业银行
yl.PAYMENT_LE_SRCB		= 36	--上海农商银行
yl.PAYMENT_LE_SDB		= 37	--深圳发展银行
yl.PAYMENT_LE_CZB		= 38	--浙江稠州商业银行
--yl.PAYMENT_LE_wypay		= 39	--网银非直联
yl.PAYMENT_LE_ALIPAY	= 40	--支付宝扫码
yl.PAYMENT_LE_ALIPAYF2F	= 41	--支付宝当面付（备用）
yl.PAYMENT_LE_ALIPAYWAP	= 42	--支付宝Wap
yl.PAYMENT_LE_WEIXIN	= 43	--微信
yl.PAYMENT_LE_QQ	    = 44	--QQ钱包支付
yl.PAYMENT_LE_cjwx   	= 45	--微信超级扫码
yl.PAYMENT_LE_kjpay  	= 46	--网关快捷支付

yl.PAYMENT_YY_WXGZH  	= 47	--微信公众号
yl.PAYMENT_YY_WXSMZF  	= 48	--微信扫码支付
yl.PAYMENT_YY_ZFBSMZF  	= 49	--支付宝扫码支付
yl.PAYMENT_YY_ZFBSJZF  	= 50	--支付宝手机
yl.PAYMENT_YY_QQSJZF  	= 51	--QQ手机支付
yl.PAYMENT_YY_WYZF  	= 52	--网银支付
yl.PAYMENT_YY_QQSMZF  	= 53	--QQ扫码支付
yl.PAYMENT_YY_BDQB  	= 54	--百度钱包
yl.PAYMENT_YY_JDZF  	= 55	--京东支付


yl.PAYMENT_YF_ALIPAYWAPT0   = 56  --支付宝WAPT0
yl.PAYMENT_YF_WEIXINT1      = 57  --微信扫码T1
yl.PAYMENT_YF_ALIPAYSMT0    = 58  --支付宝扫码T0
yl.PAYMENT_YF_ALIPAYWGT0    = 59  --支付宝网关扫码
yl.PAYMENT_YF_QUICKPAYT0    = 60  --快捷支付T0
yl.PAYMENT_YF_UNIONPAYT0    = 61  --银联扫码T0
yl.PAYMENT_YF_QQ            = 62  --QQ钱包T1
yl.PAYMENT_YF_WANGYINT0     = 63  --网银T0
yl.PAYMENT_YF_WANGYINT1     = 64  --网银T1

yl.PAYMENT_YF_ALIPAYWAPT1   = 65  --支付宝WAPT1
yl.PAYMENT_YF_ALIPAYSMT1    = 66  --支付宝扫码T1
yl.PAYMENT_YF_QUICKPAYT1    = 67  --快捷支付T1
yl.PAYMENT_YF_UNIONPAYT1    = 68  --银联扫码T1
yl.PAYMENT_YF_WEIXINT0      = 69  --微信扫码T0
yl.PAYMENT_YF_QQT0          = 70  --QQ钱包T1



--[[yl.PAYMENT_YF_ALIPAY    = 56    --支付宝
yl.PAYMENT_YF_WEIXIN    = 57    --逸付微信
yl.PAYMENT_YF_B2CPAY    = 58	--网银
yl.PAYMENT_YF_QQ        = 59	--QQ
yl.PAYMENT_YF_UNIONPAY  = 60	--银联扫码
yl.PAYMENT_YF_QUICKPAY  = 61	--快捷支付
yl.PAYMENT_YF_JD	    = 62	--京东扫码
yl.PAYMENT_YF_MEITUAN   = 63	--美团扫码
]]
yl.PAYMENT_APPSTORE_APPLE=100	--苹果审核内购

yl.PayString = 
{
	[yl.PAYMENT_LE_wypays] = {nameStr = "wypay"},
	[yl.PAYMENT_LE_ICBC] = {nameStr = "ICBC"},
	[yl.PAYMENT_LE_ABC] = {nameStr = "ABC"},
	[yl.PAYMENT_LE_CCB] = {nameStr = "CCB"},
	[yl.PAYMENT_LE_BOC] = {nameStr = "BOC"},
	[yl.PAYMENT_LE_CMB] = {nameStr = "CMB"},
	[yl.PAYMENT_LE_BCCB] = {nameStr = "BCCB"},
	[yl.PAYMENT_LE_BOCO] = {nameStr = "BOCO"},
	[yl.PAYMENT_LE_CIB] = {nameStr = "CIB"},
	[yl.PAYMENT_LE_NJCB] = {nameStr = "NJCB"},
	[yl.PAYMENT_LE_CMBC] = {nameStr = "CMBC"},
	[yl.PAYMENT_LE_CEB] = {nameStr = "CEB"},
	[yl.PAYMENT_LE_PINGANBANK] = {nameStr = "PINGANBANK"},
	[yl.PAYMENT_LE_CBHB] = {nameStr = "CBHB"},
	[yl.PAYMENT_LE_HKBEA] = {nameStr = "HKBEA"},
	[yl.PAYMENT_LE_NBCB] = {nameStr = "NBCB"},
	[yl.PAYMENT_LE_CTTIC] = {nameStr = "CTTIC"},
	[yl.PAYMENT_LE_GDB] = {nameStr = "GDB"},
	[yl.PAYMENT_LE_SHB] = {nameStr = "SHB"},
	[yl.PAYMENT_LE_SPDB] = {nameStr = "SPDB"},
	[yl.PAYMENT_LE_PSBS] = {nameStr = "PSBS"},
	[yl.PAYMENT_LE_HXB] = {nameStr = "HXB"},
	[yl.PAYMENT_LE_BJRCB] = {nameStr = "BJRCB"},
	[yl.PAYMENT_LE_SRCB] = {nameStr = "SRCB"},
	[yl.PAYMENT_LE_SDB] = {nameStr = "SDB"},
	[yl.PAYMENT_LE_CZB] = {nameStr = "CZB"},
	[yl.PAYMENT_LE_ALIPAY] = {nameStr = "ALIPAY"},
	[yl.PAYMENT_LE_ALIPAYF2F] = {nameStr = "ALIPAYF2F"},
	[yl.PAYMENT_LE_ALIPAYWAP] = {nameStr = "ALIPAYWAP"},
	[yl.PAYMENT_LE_WEIXIN] = {nameStr = "WEIXIN"},
	[yl.PAYMENT_LE_QQ] = {nameStr = "QQ"},
	[yl.PAYMENT_LE_cjwx] = {nameStr = "cjwx"},
	[yl.PAYMENT_LE_kjpay] = {nameStr = "kjpay"},
	[yl.PAYMENT_YY_WXGZH]  	= {nameStr = "901"},	--微信公众号
    [yl.PAYMENT_YY_WXSMZF]  	= {nameStr = "902"},	--微信扫码支付
    [yl.PAYMENT_YY_ZFBSMZF]  	= {nameStr = "903"},	--支付宝扫码支付
    [yl.PAYMENT_YY_ZFBSJZF]  	= {nameStr = "904"},	--支付宝手机
    [yl.PAYMENT_YY_QQSJZF]  	= {nameStr = "905"},	--QQ手机支付
    [yl.PAYMENT_YY_WYZF]	= {nameStr = "907"},	--网银支付
    [yl.PAYMENT_YY_QQSMZF]  	= {nameStr = "908"},	--QQ扫码支付
    [yl.PAYMENT_YY_BDQB] 	= {nameStr = "909"},	--百度钱包
    [yl.PAYMENT_YY_JDZF]  	= {nameStr = "910"},	--京东支付

	[yl.PAYMENT_YF_WEIXINT1]  = {nameStr = "10000101"},     --微信扫码T1
    [yl.PAYMENT_YF_WEIXINT0]  = {nameStr = "10000103"},     --微信扫码T0
    [yl.PAYMENT_YF_ALIPAYWAPT1]  = {nameStr = "20000201"},  --支付宝WAPT1
    [yl.PAYMENT_YF_ALIPAYWAPT0]  = {nameStr = "20000203"},  --支付宝WAPT0
    [yl.PAYMENT_YF_ALIPAYSMT1]  = {nameStr = "20000301"},   --支付宝扫码T1
    [yl.PAYMENT_YF_ALIPAYSMT0]  = {nameStr = "20000303"},   --支付宝扫码T0
    [yl.PAYMENT_YF_ALIPAYWGT0]  = {nameStr = "20000403"},   --支付宝网关扫码
    [yl.PAYMENT_YF_WANGYINT0]  = {nameStr = "50000101"},   --网银t1
    [yl.PAYMENT_YF_WANGYINT1]  = {nameStr = "50000103"},   --网银t0
    [yl.PAYMENT_YF_QUICKPAYT1]  = {nameStr = "40000101"},   --快捷支付T1
    [yl.PAYMENT_YF_QUICKPAYT0]  = {nameStr = "40000103"},   --快捷支付T0
    [yl.PAYMENT_YF_UNIONPAYT1]  = {nameStr = "60000101"},   --银联扫码T1
    [yl.PAYMENT_YF_UNIONPAYT0]  = {nameStr = "60000103"},   --银联扫码T0
    [yl.PAYMENT_YF_QQ]  = {nameStr = "70000101"},   --QQ钱包T1
    [yl.PAYMENT_YF_QQT0]  = {nameStr = "70000103"},   --QQ钱包

	--[[[yl.PAYMENT_YF_ALIPAY]  = {nameStr = "ALIPAY"},    	--支付宝
	[yl.PAYMENT_YF_WEIXIN]    = {nameStr = "WEIXIN"},   	--逸付微信
	[yl.PAYMENT_YF_B2CPAY]    = {nameStr = "B2CPAY"},		--网银
	[yl.PAYMENT_YF_QQ]        = {nameStr = "QQ"},		--QQ
	[yl.PAYMENT_YF_UNIONPAY]  = {nameStr = "UNIONPAY"},		--银联扫码
	[yl.PAYMENT_YF_QUICKPAY]  = {nameStr = "QUICKPAY"},		--快捷支付
	[yl.PAYMENT_YF_JD]	    = {nameStr = "JD"},		--京东扫码
	[yl.PAYMENT_YF_MEITUAN]   = {nameStr = "MEITUAN"},		--美团扫码]]

}

--失败错误码
yl.FAILURE_TYPE_ADMIN_KICK_OUT		= 0x01								--被管理员请出房间
yl.FAILURE_TYPE_SYSTEM_MAINTENANCE	= 0x02								--系统维护中
yl.FAILURE_TYPE_VIP_LIMIT_MIN		= 0x03								--会员等级低于最低进入会员条件
yl.FAILURE_TYPE_VIP_LIMIT_MAX		= 0x04								--会员等级高于最高进入会员条件
yl.FAILURE_TYPE_ROOM_FULL			= 0x05								--房间爆满
yl.FAILURE_TYPE_ROOM_PASSWORD_INCORRECT	= 0x06							--房间密码错误
yl.FAILURE_TYPE_ROOM_COIN_NOTENOUGH	= 0x07								--房间金币不足错误


--分享配置
yl.SocialShare =
{
	title 								= "明月娱乐邀您来赚金", --@share_title_social
	content 							= "利博娱乐，真人竞技,分享游戏竟然还能日如万金，你玩过吗？",
	url 								= BaseConfig.WEB_HTTP_URL,
	AppKey							 	= " ", --@share_appkey_social
}

-- 分享错误代码
yl.ShareErrorCode = 
{
    NOT_CONFIG                          = 1
}

--微信配置定义
yl.WeChat = 
{
	AppID 								= "wxc795e248bd85d55c", --@wechat_appid_wx
	AppSecret 							= "977a0d307cc4bccc10807b75b2090f72", --@wechat_secret_wx
	-- 商户id
	PartnerID 							= " ", --@wechat_partnerid_wx
	-- 支付密钥					        
	PayKey 								= " ", --@wechat_paykey_wx
	URL 								= BaseConfig.WEB_HTTP_URL,
}

--支付宝配置
yl.AliPay = 
{
	-- 合作者身份id
	PartnerID							= " ", --@alipay_partnerid_zfb
	-- 收款支付宝账号						
	SellerID							= " ", --@alipay_sellerid_zfb
	-- rsa密钥
	RsaKey								= " ", --@alipay_rsa_zfb
	NotifyURL							= BaseConfig.WEB_HTTP_URL .. "/Pay/ZFB/notify_url.aspx",
	-- ios支付宝Schemes
	AliSchemes							= "WHAliPay", --@alipay_schemes_zfb
}

--竣付通配置
yl.JFT =
{
	--商户支付密钥
	PayKey 								= " ", --@jft_paykey_jtpay
	--商户id											
	PartnerID 							= " ", --@jft_partnerid_jtpay
	--token												
	TokenURL							= "http://api.jtpay.com/jft/sdk/token/", --@jft_tokenurl_jtpay
	--后台通知url
	NotifyURL							= BaseConfig.WEB_HTTP_URL .. "/Pay/JFTAPP/Notify.aspx",
	--appid				
	JftAppID							= " ", --@jft_appid_jtpay								
	JftAesKey							= " ", --@jft_aeskey_jtpay
	JftAesVec 							= " ", --@jft_aesvec_jtpay
}
--畅支付配置
yl.CZF =
{
	Url 		= "http://api.ntlmh.com/PayBank.aspx",                                                									
	PartnerID 	= "10000",                              --商户                                                									
    PayKey      = "4272fafab8869dbd292d959b7542530c",   --密钥
	Callbackurl	= "http://www.mydomain.com/backAction",      --下行异步通知地址
    Hrefbackurl = "http://www.mydomain.com/notifyAction",               --下行同步通知地址    
    Attach      = "jrapi",                                --备注 
}
--新支付配置
yl.HTZF =
{
	MerChantId 	= "1039",--商户号                                                									
	SignKey 	= "9d213dd74eacbef65c33a1741e563813",                      --密钥                                                              									
    getPayUrl   = "http://open.024hd.com/gateway/soa",                     --接口地址
	NotifyUrl	= "http://localhost:8090/paydemo/pay_notify_url.php",      --异步通知地址
    ReturnUrl   = "http://localhost:8090/paydemo/pay_return_url.php",      --同步跳转地址  
    Subject     = "支付测试",                                               --订单名称
    Body        = "支付测试",
}
--高德配置
yl.AMAP = 
{
	-- 开发KEY
	AmapKeyIOS							= "6c23cefb689a8a391672e8f75465e6e1", --@ios_devkey_amap
	AmapKeyAndroid						= "5e83f2df90cd101d8fc0f9866ddb9f45", --@android_devkey_amap
}

yl.PLATFORM_WX							= 5				--微信平台

--第三方平台定义(同java/ios端定义值一致)
yl.ThirdParty = 
{
	WECHAT 								= 0,	--微信
	WECHAT_CIRCLE						= 1,	--朋友圈
	ALIPAY								= 2,	--支付宝
	JFT								 	= 3,	--俊付通
	AMAP 								= 4,	--高德地图
	IAP 							 	= 5,	--ios iap
    SMS                                 = 6,    --分享到短信
    CZF                                 = 7,    --畅支付
    HTZF                                = 8,    --新支付
}
--平台id列表(服务器登陆用)
yl.PLATFORM_LIST = {}
yl.PLATFORM_LIST[yl.ThirdParty.WECHAT]	= 5

yl.MAX_INT                              = 2 ^ 15
--是否动态加入
yl.m_bDynamicJoin                       = false
--设备类型
yl.DEVICE_TYPE_LIST = {}
yl.DEVICE_TYPE_LIST[cc.PLATFORM_OS_WINDOWS] 	= 0x00
yl.DEVICE_TYPE_LIST[cc.PLATFORM_OS_ANDROID] 	= 0x11
yl.DEVICE_TYPE_LIST[cc.PLATFORM_OS_IPHONE] 	= 0x31
yl.DEVICE_TYPE_LIST[cc.PLATFORM_OS_IPAD] 	= 0x41

--右移
function yl.rShiftOp(left,num) 
    return math.floor(left / (2 ^ num))  
end  

--左移
function yl.lShiftOp(left, num) 
 return left * (2 ^ num)  
end


function yl.tab_cutText(str)
    local list = {}
    local len = string.len(str)
    local i = 1 
    while i <= len do
        local c = string.byte(str, i)
        local shift = 1
        if c > 0 and c <= 127 then
            shift = 1
        elseif (c >= 192 and c <= 223) then
            shift = 2
        elseif (c >= 224 and c <= 239) then
            shift = 3
        elseif (c >= 240 and c <= 247) then
            shift = 4
        end
        local char = string.sub(str, i, i+shift-1)
        i = i + shift
        table.insert(list, char)
    end
    return list, len
end

function yl.stringCount(szText)
	
	local i = 1
	local nCount = 0
	
	if szText ~= nil then
		--文本长度
		while true do
			local cur = string.sub(szText,i)
			local c = string.byte(cur)
			if c == nil then
				break
			end
			if c > 0 and c <= 127 then
				i = i + 1
			elseif (c >= 192 and c <= 223) then
				i = i + 2
			elseif (c >= 224 and c <= 239) then
			 	i = i + 3
			elseif (c >= 240 and c <= 247) then
			 	i = i + 4
			else
				i = i + 1
			end
			nCount = nCount + 1
			-- if byte > 128 then
			-- 	i = i + 3
			-- else
			-- 	i = i + 1
			-- end
			-- nCount = nCount + 1
		end
	end
	
	return nCount
end

function string.getConfig(fontfile,fontsize)
	local config = {}
	local tmpEN = cc.LabelTTF:create("A", fontfile, fontsize)
	local tmpCN = cc.LabelTTF:create("网", fontfile, fontsize)
	local tmpen = cc.LabelTTF:create("a", fontfile, fontsize)
	local tmpNu = cc.LabelTTF:create("2", fontfile, fontsize)
	config.upperEnSize = tmpEN:getContentSize().width
	config.cnSize = tmpCN:getContentSize().width
	config.lowerEnSize = tmpen:getContentSize().width
	config.numSize = tmpNu:getContentSize().width
	return config
end

function string.EllipsisByConfig(szText, maxWidth,config)
	if not config then
		return szText
	end
--当前计算宽度
	local width = 0
	--截断结果
	local szResult = "..."
	--完成判断
	local bOK = false
	 
	local i = 1

	local endwidth = 3*config.numSize
	 
	while true do
		local cur = string.sub(szText,i,i)
		local byte = string.byte(cur)
		if byte == nil then
			break
		end
		if byte > 128 then
			if width <= maxWidth - endwidth then
				width = width + config.cnSize
				i = i + 3
			else
				bOK = true
				break
			end
		elseif	byte ~= 32 then
			if width <= maxWidth - endwidth then
				if string.byte('A') <= byte and byte <= string.byte('Z') then
					width = width + config.upperEnSize
				elseif string.byte('a') <= byte and byte <= string.byte('z') then
					width = width + config.lowerEnSize
				else
					width = width + config.numSize
				end
				i = i + 1
			else
				bOK = true
				break
			end
		else
			i = i + 1
		end
	end
	 
 	if i ~= 1 then
		szResult = string.sub(szText, 1, i-1)
		if(bOK) then
			szResult = szResult.."..."
		end
	end
	return szResult
end

--依据宽度截断字符
function string.stringEllipsis(szText, sizeE,sizeCN,maxWidth)
	--当前计算宽度
	local width = 0
	--截断结果
	local szResult = "..."
	--完成判断
	local bOK = false
	 
	local i = 1
	 
	while true do
		local cur = string.sub(szText,i,i)
		local byte = string.byte(cur)
		if byte == nil then
			break
		end
		if byte > 128 then
			if width <= maxWidth - 3*sizeE then
				width = width + sizeCN
				i = i + 3
			else
				bOK = true
				break
			end
		elseif	byte ~= 32 then
			if width <= maxWidth - 3*sizeE then
				width = width +sizeE
				i = i + 1
			else
				bOK = true
				break
			end
		else
			i = i + 1
		end
	end
	 
 	if i ~= 1 then
		szResult = string.sub(szText, 1, i-1)
		if(bOK) then
			szResult = szResult.."..."
		end
	end
	return szResult
end

-- 获取余数
function math.mod(a, b)
    return a - math.floor(a/b)*b
end

function string.formatNumberThousands(num,dot,flag)

	local formatted 
	if not dot then
    	formatted = string.format("%0.2f",tonumber(num))
    else
    	formatted = tonumber(num)
    end
    local sp
    if not flag then
    	sp = ","
    else
    	sp = flag
    end
    local k
    while true do
        formatted, k = string.gsub(formatted, "^(-?%d+)(%d%d%d)", '%1'..sp..'%2')
        if k == 0 then break end
    end
    return formatted
end

function string.formatNumberTThousands(num)

	assert(type(num) == "number")
	if (type(num) ~= "number") then
		return "0"
	end
	
	if (num >= 10000 and num < 100000000) then
		return string.format("%0.2f", tonumber(num) / 10000)
	elseif (num >= 100000000) then
		return string.format("%0.2f", tonumber(num) / 100000000)
	else
		return string.formatNumberThousands(num, false, "/")
	end
end

function string.formatNumberFhousands(num)

	assert(type(num) == "number")
	if (type(num) ~= "number") then
		return "0"
	end
	
	return string.format("%0.2f", tonumber(num))
end

--eg:1 转 1.0
function string.formatNumberTwoFloat(num)

	assert(type(num) == "number")
	if (type(num) ~= "number") then
		return "0"
	end
	if (num >= 1 and num < 10000) then
		return string.format("%0.2f", tonumber(num))
	elseif (num >= 10000 and num < 100000000) then
		return string.format("%0.2f", tonumber(num) / 10000)
	elseif (num >= 100000000) then
		return string.format("%0.2f", tonumber(num) / 100000000)
	else
		return string.formatNumberThousands(num, true, "/")
	end
end

function yl.getLevelDescribe(lScore)
    local lLevelScore = { 2000, 4000, 8000, 20000, 40000, 80000, 150000, 300000, 500000, 1000000, 2000000, 5000000, 10000000, 50000000, 100000000, 500000000, 1000000000 };
	local lLevelDesc={ "务农","佃户","雇工","作坊主","农场主","地主","大地主","财主","富翁","大富翁","小财神","大财神","赌棍","赌侠","赌王","赌圣","赌神","职业赌神"}
	for i=#lLevelScore ,1,-1 do
		if lScore >= lLevelScore[i] then
			return lLevelDesc[i]
		end
	end
	return lLevelDesc[1]
end

local poker_data = 
{
	0x00,
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, -- 方块
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, -- 梅花
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, -- 红桃
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, -- 黑桃
    0x4E, 0x4F
}
-- 逻辑数值
yl.POKER_VALUE = {}
-- 逻辑花色
yl.POKER_COLOR = {}
-- 纹理花色
yl.CARD_COLOR = {}
function yl.GET_POKER_VALUE()
	for k,v in pairs(poker_data) do
		yl.POKER_VALUE[v] = math.mod(v, 16)
		yl.POKER_COLOR[v] = bit.band(v , 0xF0)
		yl.CARD_COLOR[v] = math.floor(v / 16)
	end
end
yl.GET_POKER_VALUE()