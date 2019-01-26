#ifndef DEFINE_HEAD_FILE
#define DEFINE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//数值定义

//头像大小
#define FACE_CX						48									//头像宽度
#define FACE_CY						48									//头像高度

#define BIGFACE_CX					96									//头像宽度
#define BIGFACE_CY					96									//头像高度

//长度定义
#define LEN_LESS_ACCOUNTS			6									//最短帐号
#define LEN_LESS_NICKNAME			6									//最短昵称
#define LEN_LESS_PASSWORD			6									//最短密码
#define LEN_LESS_COMPELLATION		4									//最小长度

//人数定义
#define MAX_CHAIR					200									//最大椅子
#define MAX_TABLE					512									//最大桌子
#define MAX_COLUMN					32									//最大列表
#define MAX_BATCH					48									//最大批次
#define MAX_ANDROID					2048								//最大机器
#define MAX_PROPERTY				128									//最大道具
#define MAX_WHISPER_USER			16									//最大私聊
#define MAX_BUFF					10									//最大Buff

//列表定义
#define MAX_KIND					256									//最大类型
#define MAX_SERVER					1024								//最大房间

//参数定义
#define INVALID_CHAIR				0xFFFF								//无效椅子
#define INVALID_TABLE				0xFFFF								//无效桌子
#define INVALID_SERVER				0xFFFF								//无效房间
#define INVALID_KIND				0xFFFF								//无效游戏

//税收定义
#define REVENUE_BENCHMARK			0L								    //税收起点
#define REVENUE_DENOMINATOR			1000L								//税收分母
#define PERSONAL_ROOM_CHAIR			8									//私人房间座子上椅子的最大数目
//////////////////////////////////////////////////////////////////////////////////
//系统参数
#define	MAX_ACCOUNTS				5									//最多账号

//积分类型
#define SCORE						DOUBLE								//积分类型
#define SCORE_STRING				TEXT("%.2lf")						//积分类型

//游戏状态
#define GAME_STATUS_FREE			0									//空闲状态
#define GAME_STATUS_PLAY			100									//游戏状态
#define GAME_STATUS_WAIT			200									//等待状态

//系统参数
#define LEN_USER_CHAT				128									//聊天长度
#define TIME_USER_CHAT				1L									//聊天间隔
#define TRUMPET_MAX_CHAR            128									//喇叭长度

//////////////////////////////////////////////////////////////////////////////////
//索引质数

//列表质数
#define PRIME_TYPE					11L									//种类数目
#define PRIME_KIND					53L									//类型数目
#define PRIME_NODE					101L								//节点数目
#define PRIME_PAGE					53L									//自定数目
#define PRIME_SERVER				1009L								//房间数目

//人数质数
#define PRIME_SERVER_USER			503L								//房间人数
#define PRIME_ANDROID_USER			503L								//机器人数
#define PRIME_PLATFORM_USER			100003L								//平台人数

//属性质数
#define PRIME_ANDROID_PARAMETER		65									//配置数目

//////////////////////////////////////////////////////////////////////////////////
//数据长度

//资料数据
#define LEN_MD5						33									//加密密码
#define LEN_USERNOTE				32									//备注长度
#define LEN_ACCOUNTS				32									//帐号长度
#define LEN_NICKNAME				32									//昵称长度
#define LEN_PASSWORD				33									//密码长度
#define LEN_GROUP_NAME				32									//社团名字
#define LEN_UNDER_WRITE				32									//个性签名
#define LEN_REMARKS					32									//备注信息
#define LEN_DATETIME				20									//日期长度
#define  ROOM_ID_LEN 7
//数据长度
#define LEN_QQ						16									//Q Q 号码
#define LEN_IP						32									//IP长度
#define LEN_EMAIL					33									//电子邮件
#define LEN_USER_NOTE				256									//用户备注
#define LEN_SEAT_PHONE				33									//固定电话
#define LEN_MOBILE_PHONE			12									//移动电话
#define LEN_PASS_PORT_ID			19									//证件号码
#define LEN_COMPELLATION			16									//真实名字
#define LEN_DWELLING_PLACE			128									//联系地址
#define LEN_USER_UIN				33									//UIN长度
#define LEN_WEEK					7									//星期长度
#define LEN_TASK_NAME				64									//任务名称
#define LEN_TRANS_REMARK			32									//转账备注	
#define LEN_VERIFY_CODE				8									//验证长度
#define LEN_POSTALCODE				8									//邮政编码
#define LEN_BIRTHDAY				16									//用户生日
#define LEN_BLOODTYPE				6									//用户血型
#define LEN_CONSTELLATION			6									//用户星座
#define LEN_PHONE_MODE				21									//手机型号
#define LEN_REAL_NAME				32									//真实姓名
#define LEN_BANK_NO					32									//银行卡号
#define LEN_BANK_NAME				32									//银行名称
#define LEN_BANK_ADDRESS 		    128									//银行开户地址
#define LEN_AGENT_ID				20									//代理号长度
#define LEN_DESC					128									//描述长度
#define LEN_IMAGE_NAME				32									//文件名长度
#define LEN_ALIPAY					32									//支付宝账号
#define LEN_WECHAT					16									//微信账号长度
//机器标识
#define LEN_NETWORK_ID				13									//网卡长度
#define LEN_MACHINE_ID				33									//序列长度
#define LEN_WHERE_PARM				32									//操作参数

//列表数据
#define LEN_TYPE					32									//种类长度
#define LEN_KIND					32									//类型长度
#define LEN_NODE					32									//节点长度
#define LEN_PAGE					32									//定制长度
#define LEN_SERVER					32									//房间长度
#define LEN_PROCESS					32									//进程长度
#define LEN_ORDER_ID				64									//订单号

//////////////////////////////////////////////////////////////////////////
//好友定义
#define MAX_FRIEND_COUNT			1000								//最大好友
#define MIN_FRIEND_CONTENT			20									//最小容量	
#define MAX_FRIEND_TRANSFER			5									//最大传输	

#define FRIEND_OPERATE_REMOVE		1									//移除好友
#define FRIEND_OPERATE_MDGROUPID	2									//修改组别

//登录命令
#define LOGON_COMMAND_USERINFO      0x0001								//用户信息
#define LOGON_COMMAND_GROUPINFO		0x0002								//分组信息
#define LOGON_COMMAND_FACE			0x0004								//用户头像
#define LOGON_COMMAND_FRIENDS       0x0008								//用户好友
#define LOGON_COMMAND_REMARKS		0x0010								//备注信息
#define LOGON_TIME_SPACE			1000								//时间间隔

//分组定义
#define GROUP_MAX_COUNT				32									//最大分组
#define GROUP_LEN_NAME				32									//组名长度	
#define GROUP_LEN_INFO				1021								//信息长度

#define GROUP_OPERATE_ADD			1									//添加分组
#define GROUP_OPERATE_MODIFY		2									//编辑分组
#define GROUP_OPERATE_REMOVE		3									//移除分组


//常量定义
#define FRIEND_INVALID_SERVERID			WORD(0xffff)						//无效房间
#define FRIEND_INVALID_TABLEID			WORD(0xffff)						//无效桌子
#define FRIEND_INVALID_CHAIR			WORD(0xffff)						//无效椅子

//用户状态
#define FRIEND_US_OFFLINE					0x00								//下线状态
#define FRIEND_US_ONLINE					0x01								//在线状态
#define FRIEND_US_LEAVE						0x02								//离开状态
#define FRIEND_US_BUSY						0x04								//忙碌状态
#define FRIEND_US_UNDISTRUB					0x08								//勿打扰状态
#define FRIEND_US_CORBET					0x10								//隐身状态


//聊天数据
#define LEN_FONT_NAME				16									//字体名称
#define LEN_MESSAGE_CONTENT			512									//消息长度		
#define MAX_EXPRESSION_COUNT		100									//最大表情	

//////////////////////////////////////////////////////////////////////////////////

//用户关系
#define	CP_NORMAL					0									//未知关系
#define	CP_FRIEND					1									//好友关系
#define	CP_DETEST					2									//厌恶关系
#define CP_SHIELD					3									//屏蔽聊天

//////////////////////////////////////////////////////////////////////////////////

//性别定义
#define GENDER_FEMALE				0									//女性性别
#define GENDER_MANKIND				1									//男性性别

//////////////////////////////////////////////////////////////////////////////////

//游戏模式
#define GAME_GENRE_GOLD				0x0001								//金币类型
#define GAME_GENRE_SCORE			0x0002								//点值类型
#define GAME_GENRE_MATCH			0x0004								//比赛类型
#define GAME_GENRE_EDUCATE			0x0008								//训练类型
#define GAME_GENRE_PERSONAL			0x0010								//约战类型

//房间类型
#define SERVER_GENRE_NORMAL         0x0001                              //普通类型
#define SERVER_GENRE_PASSWD         0x0002                              //密码类型

//分数模式
#define SCORE_GENRE_NORMAL			0x0100								//普通模式
#define SCORE_GENRE_POSITIVE		0x0200								//非负模式

//////////////////////////////////////////////////////////////////////////////////

//任务类型
#define TASK_TYPE_WIN_INNINGS		0x01								//赢局任务
#define TASK_TYPE_SUM_INNINGS		0x02								//总局任务
#define TASK_TYPE_FIRST_WIN			0x04								//首胜任务
#define TASK_TYPE_KEEP_WIN			0x08								//连赢任务

//任务状态
#define TASK_STATUS_UNFINISH		0									//任务状态
#define TASK_STATUS_SUCCESS			1									//任务成功
#define TASK_STATUS_FAILED			2									//任务失败	
#define TASK_STATUS_REWARD			3									//领取奖励

//任务数量
#define TASK_MAX_COUNT				128									//任务数量

//////////////////////////////////////////////////////////////////////////////////
//抽奖功能
#define MAX_LOTTERY					16									//最多奖项

//游戏数据
#define LEN_GAME_DATA				1024								//游戏数据
#define LEN_GAME_ITEM				64									//数据子项
#define MAX_DATA_ITEM				16									//子项个数
	
//////////////////////////////////////////////////////////////////////////////////

//用户状态
#define US_NULL						0x00								//没有状态
#define US_FREE						0x01								//站立状态
#define US_SIT						0x02								//坐下状态
#define US_READY					0x03								//同意状态
#define US_LOOKON					0x04								//旁观状态
#define US_PLAYING					0x05								//游戏状态
#define US_OFFLINE					0x06								//断线状态

//////////////////////////////////////////////////////////////////////////////////
//比赛定义

//比赛用户状态
#define MUS_NULL					0x00								//没有状态
#define MUS_OUT						0x01								//淘汰状态
#define MUS_SIGNUPED     			0x02								//报名状态
#define MUS_PLAYING					0x04								//比赛进行

//比赛状态
#define MS_FREE						0x00								//空闲状态
#define MS_WAITPLAY					0x01								//等待开始
#define MS_MATCHING					0x02								//比赛进行
#define MS_WAITEND					0x04								//等待结束
#define MS_MATCHEND					0x08								//比赛结束

//缴费区域
#define DEDUCT_AREA_SERVER			0									//房间缴费
#define DEDUCT_AREA_WEBSITE			1									//网站缴费

//费用类型
#define FEE_TYPE_GOLD				0									//缴纳金币
#define FEE_TYPE_MEDAL				1									//缴纳奖牌	

//参赛条件
#define MATCH_JOINCD_MEMBER_ORDER	0x01								//会员等级	
#define MATCH_JOINCD_EXPERIENCE		0x02								//经验等级

//筛选方式
#define FILTER_TYPE_SINGLE_TURN		0									//单轮排名
#define FILTER_TYPE_TOTAL_RANKING	1									//总排名次

//报名模式
#define SIGNUP_MODE_SIGNUP_FEE		0x01								//报名模式
#define SIGNUP_MODE_MATCH_USER		0x02								//报名模式

//排名方式
#define RANKING_MODE_TOTAL_GRADES   0									//排名方式
#define RANKING_MODE_SPECIAL_GRADES 1									//排名方式 

//筛选成绩
#define FILTER_GRADES_MODE_BEST		0									//最优成绩
#define FILTER_GRADES_MODE_AVERAGE	1									//平均成绩
#define FILTER_GRADES_MODE_AVERAGEEX 2									//平均成绩

//开赛模式
#define MATCH_MODE_NORMAL			0x00								//常规开赛
#define MATCH_MODE_ROUND			0x01								//循环开赛

//比赛类型
#define MATCH_TYPE_LOCKTIME			0x00								//定时类型
#define MATCH_TYPE_IMMEDIATE		0x01								//即时类型

//////////////////////////////////////////////////////////////////////////////////

//房间规则
#define SRL_LOOKON					0x00000001							//旁观标志
#define SRL_OFFLINE					0x00000002							//断线标志
#define SRL_SAME_IP					0x00000004							//同网标志

//房间规则
#define SRL_ROOM_CHAT				0x00000100							//聊天标志
#define SRL_GAME_CHAT				0x00000200							//聊天标志
#define SRL_WISPER_CHAT				0x00000400							//私聊标志
#define SRL_HIDE_USER_INFO			0x00000800							//隐藏标志

//////////////////////////////////////////////////////////////////////////////////
//列表数据

//无效属性
#define UD_NULL						0									//无效子项
#define UD_IMAGE					100									//图形子项
#define UD_CUSTOM					200									//自定子项

//基本属性
#define UD_GAME_ID					1									//游戏标识
#define UD_USER_ID					2									//用户标识
#define	UD_NICKNAME					3									//用户昵称

//扩展属性
#define UD_GENDER					10									//用户性别
#define UD_GROUP_NAME				11									//社团名字
#define UD_UNDER_WRITE				12									//个性签名

//状态信息
#define UD_TABLE					20									//游戏桌号
#define UD_CHAIR					21									//椅子号码

//积分信息
#define UD_SCORE					30									//用户分数
#define UD_GRADE					31									//用户成绩
#define UD_USER_MEDAL				32									//用户经验
#define UD_EXPERIENCE				33									//用户经验
#define UD_LOVELINESS				34									//用户魅力
#define UD_WIN_COUNT				35									//胜局盘数
#define UD_LOST_COUNT				36									//输局盘数
#define UD_DRAW_COUNT				37									//和局盘数
#define UD_FLEE_COUNT				38									//逃局盘数
#define UD_PLAY_COUNT				39									//总局盘数

//积分比率
#define UD_WIN_RATE					40									//用户胜率
#define UD_LOST_RATE				41									//用户输率
#define UD_DRAW_RATE				42									//用户和率
#define UD_FLEE_RATE				43									//用户逃率
#define UD_GAME_LEVEL				44									//游戏等级

//扩展信息
#define UD_NOTE_INFO				50									//用户备注
#define UD_LOOKON_USER				51									//旁观用户

//图像列表
#define UD_IMAGE_FLAG				(UD_IMAGE+1)						//用户标志
#define UD_IMAGE_GENDER				(UD_IMAGE+2)						//用户性别
#define UD_IMAGE_STATUS				(UD_IMAGE+3)						//用户状态

//////////////////////////////////////////////////////////////////////////////////
//数据库定义

#define DB_ERROR 					-1  								//处理失败
#define DB_SUCCESS 					0  									//处理成功
#define DB_NEEDMB 					18 									//处理失败
#define DB_PASSPORT					19									//处理失败

//////////////////////////////////////////////////////////////////////////////////
//道具标示
#define PT_USE_MARK_DOUBLE_SCORE    0x0001								//双倍积分
#define PT_USE_MARK_FOURE_SCORE     0x0002								//四倍积分
#define PT_USE_MARK_GUARDKICK_CARD  0x0010								//防踢道具
#define PT_USE_MARK_POSSESS         0x0020								//附身道具 

#define MAX_PT_MARK                 4                                   //标识数目

//有效范围
#define VALID_TIME_DOUBLE_SCORE     3600                                //有效时间
#define VALID_TIME_FOUR_SCORE       3600                                //有效时间
#define VALID_TIME_GUARDKICK_CARD   3600                                //防踢时间
#define VALID_TIME_POSSESS          3600                                //附身时间
#define VALID_TIME_KICK_BY_MANAGER  3600                                //游戏时间 

#define PROPERTY_LEN_NAME			32									//道具名字
#define PROPERTY_LEN_INFO			128									//道具信息
//////////////////////////////////////////////////////////////////////////////////
//设备类型
#define DEVICE_TYPE_PC              0x00                                //PC
#define DEVICE_TYPE_ANDROID         0x10                                //Android
#define DEVICE_TYPE_ANDROID_COCOS2D	0x11								//Android COCOS2D
#define DEVICE_TYPE_ANDROID_U3D		0x12								//Android U3D
#define DEVICE_TYPE_ITOUCH          0x20                                //iTouch
#define DEVICE_TYPE_IPHONE          0x30                                //iPhone
#define DEVICE_TYPE_IPHONE_COCOS2D	0x31								//Android COCOS2D
#define DEVICE_TYPE_IPHONE_U3D		0x32								//Android U3D
#define DEVICE_TYPE_IPAD            0x40                                //iPad
#define DEVICE_TYPE_IPAD_COCOS2D	0x41								//Android COCOS2D
#define DEVICE_TYPE_IPAD_U3D		0x42								//Android U3D

/////////////////////////////////////////////////////////////////////////////////
//手机定义

//视图模式
#define	VIEW_MODE_ALL				0x0001								//全部可视
#define	VIEW_MODE_PART				0x0002								//部分可视

//信息模式
#define VIEW_INFO_LEVEL_1			0x0010								//部分信息
#define VIEW_INFO_LEVEL_2			0x0020								//部分信息
#define VIEW_INFO_LEVEL_3			0x0040								//部分信息
#define VIEW_INFO_LEVEL_4			0x0080								//部分信息

//其他配置
#define RECVICE_GAME_CHAT			0x0100								//接收聊天
#define RECVICE_ROOM_CHAT			0x0200								//接收聊天
#define RECVICE_ROOM_WHISPER		0x0400								//接收私聊

//行为标识
#define BEHAVIOR_LOGON_NORMAL       0x0000                              //普通登录
#define BEHAVIOR_LOGON_IMMEDIATELY  0x1000                              //立即登录

/////////////////////////////////////////////////////////////////////////////////
//处理结果
#define RESULT_ERROR 					-1  								//处理错误
#define RESULT_SUCCESS 					0  									//处理成功
#define RESULT_FAIL 					1  									//处理失败

/////////////////////////////////////////////////////////////////////////////////
//变化原因
#define SCORE_REASON_WRITE              0                                   //写分变化
#define SCORE_REASON_INSURE             1                                   //银行变化
#define SCORE_REASON_PROPERTY           2                                   //道具变化
#define SCORE_REASON_MATCH_FEE          3                                   //比赛报名
#define SCORE_REASON_MATCH_QUIT         4                                   //比赛退赛

/////////////////////////////////////////////////////////////////////////////////

//登录房间失败原因
#define LOGON_FAIL_SERVER_INVALIDATION  200   

/////////////////////////////////////////////////////////////////////////////////

//机器类型
#define ANDROID_SIMULATE				0x01								//相互模拟
#define ANDROID_PASSIVITY				0x02								//被动陪打
#define ANDROID_INITIATIVE				0x04								//主动陪打
#define ANDROID_THEJOINT				0x08								//串场模式
/////////////////////////////////////////////////////////////////////////////////
//比赛定义

//扣费类型
#define MATCH_FEE_TYPE_GOLD				0x00								//扣费类型
#define MATCH_FEE_TYPE_MEDAL			0x01								//扣费类型

//比赛类型
#define MATCH_TYPE_LOCKTIME				0x00								//定时类型
#define MATCH_TYPE_IMMEDIATE			0x01								//即时类型

////////////////////////////////////////////////////////////////////////////////

//货币类型
#define CONSUME_TYPE_GOLD				0x01								//游戏币
#define CONSUME_TYPE_USEER_MADEL		0x02								//元宝
#define CONSUME_TYPE_CASH				0x03								//游戏豆
#define CONSUME_TYPE_LOVELINESS			0x04								//魅力值

//热键定义
#define IDI_HOT_KEY_BOSS			0x0100								//老板热键
#define IDI_HOT_KEY_WHISPER			0x0200								//私聊热键

//失败错误码
#define FAILURE_TYPE_ADMIN_KICK_OUT		0x01								//被管理员请出房间
#define FAILURE_TYPE_SYSTEM_MAINTENANCE	0x02								//系统维护中
#define FAILURE_TYPE_VIP_LIMIT_MIN		0x03								//会员等级低于最低进入会员条件
#define FAILURE_TYPE_VIP_LIMIT_MAX		0x04								//会员等级高于最高进入会员条件
#define FAILURE_TYPE_ROOM_FULL			0x05								//房间爆满
#define FAILURE_TYPE_ROOM_PASSWORD_INCORRECT 0x06							//房间密码错误
#define FAILURE_TYPE_ROOM_COIN_NOTENOUGH 0x07							//房间金币不足错误

//私人房相关
const int RULE_LEN = 100;//房间规则长度
const int SPECIAL_INFO_LEN = 1000;//针对房间结束时结算时的一些特殊要求
const int MAX_CREATE_COUNT = 32;
//最大语音
#define MAXT_VOICE_LENGTH				15000								//语音长度

#endif