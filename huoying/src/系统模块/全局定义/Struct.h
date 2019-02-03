#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE


#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//游戏列表

//游戏类型
struct tagGameType
{
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wTypeID;							//类型索引
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//游戏种类
struct tagGameKind
{
	WORD							wTypeID;							//类型索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wGameID;							//模块索引
	WORD							wRecommend;							//推荐游戏
	WORD							wGameFlag;							//游戏标志
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//机器人数
	DWORD							dwDummyCount;						//虚拟人数
	DWORD							dwFullCount;						//满员人数
	DWORD							dwSuportType;						//支持类型
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	TCHAR							szProcessName[LEN_PROCESS];			//进程名字
};

//游戏节点
struct tagGameNode
{
	WORD							wKindID;							//名称索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wNodeID;							//节点索引
	TCHAR							szNodeName[LEN_NODE];				//节点名称
};

//定制类型
struct tagGamePage
{
	WORD							wPageID;							//页面索引
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wOperateType;						//控制类型
	TCHAR							szDisplayName[LEN_PAGE];			//显示名称
};

//游戏房间
struct tagGameServer
{
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wServerID;							//房间索引
	WORD                            wServerKind;                        //房间类型
	WORD							wServerType;						//房间类型
	WORD							wServerLevel;						//房间等级
	WORD							wServerPort;						//房间端口
	SCORE							lCellScore;							//单元积分
	BYTE							cbEnterMember;						//进入会员
	SCORE							lEnterScore;						//进入积分
	DWORD							dwServerRule;						//房间规则
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//机器人数
	DWORD							dwFullCount;						//满员人数
	TCHAR							szServerAddr[32];					//房间名称
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	//私人房添加
	DWORD							dwSurportType;						//支持类型
	WORD							wTableCount;						//桌子数目
	DWORD							dwDummyCount;						//虚拟人数
};

//代理列表
struct tagAgentGameKind
{
	WORD							wKindID;							//类型索引
	WORD							wSortID;							//排序索引
};

//定时赛
struct tagLockTimeMatch
{
	//时间配置
	DWORD							wEndDelayTime;						//延迟时间
	SYSTEMTIME						MatchStartTime;						//开始日期
	SYSTEMTIME						MatchEndTime;						//结束日期

	//报名参数
	BYTE							cbMatchMode;						//开赛模式
	WORD							wMaxSignupUser;						//报名人数
	WORD							wStartUserCount;					//开赛人数	
	SYSTEMTIME						SignupStartTime;					//报名开始
	SYSTEMTIME						SignupEndTime;						//报名截止	

	//比赛分数
	LONGLONG						lMatchInitScore;					//初始积分
	LONGLONG						lMatchCullScore;					//淘汰积分

	//比赛局数
	WORD							wMinPlayCount;						//最少局数
	WORD							wMaxPlayCount;						//最大局数

	//复活配置
	BYTE							cbReviveEnabled;					//启用标识
	BYTE							cbSafeCardEnabled;					//启用标识
	BYTE							cbReviveTimes;						//复活次数
	LONGLONG						lReviveFee;							//复活费用
	LONGLONG						lSafeCardFee;						//保险费用
};

//即时赛
struct tagImmediateMatch
{
	//人数信息
	WORD							wStartUserCount;					//开赛人数	
	WORD							wAndroidUserCount;					//机器数量
	WORD							wAndroidDelaySignupTime;			//延迟报名

	//分数信息
	LONGLONG						lInitalBase;						//初始基数
	LONGLONG						lInitalScore;						//初始积分

	//局数信息
	WORD							wPlayCount;							//游戏局数
	BYTE							cbSwitchTableCount;					//换桌局数(0为不换桌)
	WORD							wPrecedeTimer;						//优先坐下
};	

//奖励信息
struct tagMatchRewardInfo
{
	WORD							wRankID;							//奖励名次
	SCORE							lRewardGold;						//奖励金币
	SCORE							lRewardIngot;						//奖励元宝	
	DWORD							dwRewardExperience;					//奖励经验
};

//比赛信息
struct tagGameMatch
{
	//基本信息
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识	
	BYTE							cbMatchType;						//比赛类型
	TCHAR							szMatchName[32];					//比赛名称

	//报名信息
	BYTE							cbFeeType;							//费用类型
	BYTE							cbDeductArea;						//缴费区域
	LONGLONG						lSignupFee;							//报名费用	
	BYTE							cbSignupMode;						//报名方式
	BYTE							cbJoinCondition;					//参赛条件
	BYTE							cbMemberOrder;						//会员等级
	LONG							lExperience;						//玩家经验

	//排名方式
	BYTE							cbRankingMode;						//排名方式	
	WORD							wCountInnings;						//统计局数
	BYTE							cbFilterGradesMode;					//筛选方式
	
	//奖励信息
	WORD							wRewardCount;						//奖励人数
	tagMatchRewardInfo				MatchRewardInfo[3];					//比赛奖励

	//比赛规则
	BYTE							cbMatchRule[512];					//比赛规则
};

//聊天信息
struct tagChatServer
{
	WORD							wChatID;							//广场标识
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
};

//视频配置
struct tagAVServerOption
{
	WORD							wAVServerPort;						//视频端口
	DWORD							dwAVServerAddr;						//视频地址
};

//在线信息
struct tagOnLineInfoKind
{
	WORD							wKindID;							//类型标识
	DWORD							dwOnLineCount;						//在线人数
};

//在线统计
struct tagOnLineInfoKindEx
{
	WORD							wKindID;							//类型标识
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwAndroidCount;						//机器人数
};

//在线信息
struct tagOnLineInfoServer
{
	WORD							wServerID;							//房间标识
	DWORD							dwOnLineCount;						//在线人数
};

//奖项子项
struct tagLotteryItem
{
	BYTE							cbItemIndex;						//奖项索引
	BYTE							cbItemType;							//奖励类型
	SCORE							lItemQuota;							//奖励额度
};
//////////////////////////////////////////////////////////////////////////////////
//机器参数
struct tagAndroidParameter
{
	//属性变量
	DWORD							dwBatchID;							//批次标识
	DWORD							dwServiceMode;						//服务模式
	DWORD							dwAndroidCount;						//机器数目
	DWORD							dwEnterTime;						//进入时间
	DWORD							dwLeaveTime;						//离开时间
	SCORE							lTakeMinScore;						//携带分数
	SCORE							lTakeMaxScore;						//携带分数
	DWORD							dwEnterMinInterval;					//进入间隔
	DWORD							dwEnterMaxInterval;					//进入间隔
	DWORD							dwLeaveMinInterval;					//离开间隔
	DWORD							dwLeaveMaxInterval;					//离开间隔	
	DWORD							dwSwitchMinInnings;					//换桌局数
	DWORD							dwSwitchMaxInnings;					//换桌局数
	DWORD							AndroidCountMember0;				//普通会员	
	DWORD							AndroidCountMember1;				//一级会员
	DWORD							AndroidCountMember2;				//二级会员
	DWORD							AndroidCountMember3;				//三级会员
	DWORD							AndroidCountMember4;				//四级会员
	DWORD							AndroidCountMember5;				//五级会员	
};

//////////////////////////////////////////////////////////////////////////////////
//用户信息

//桌子状态
struct tagTableStatus
{
	BYTE							cbTableLock;						//锁定标志
	BYTE							cbPlayStatus;						//游戏标志
	LONG							lCellScore;							//单元积分
};

//用户状态
struct tagUserStatus
{
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子位置
	BYTE							cbUserStatus;						//用户状态
};

//用户属性
struct tagUserAttrib
{
	BYTE							cbCompanion;						//用户关系
};

//用户积分
struct tagUserScore
{
	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lInsure;							//用户银行
	SCORE							lIngot;								//用户元宝
	DOUBLE							dBeans;								//用户游戏豆

	//输赢信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	SCORE							lIntegralCount;						//积分总数(当前房间)

	//全局信息
	DWORD							dwExperience;						//用户经验
	LONG							lLoveLiness;						//用户魅力
};

//用户积分
struct tagMobileUserScore
{
	//积分信息
	SCORE							lScore;								//用户分数
	DOUBLE							dBeans;								//用户游戏豆

	//输赢信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	SCORE							lIntegralCount;						//积分总数(当前房间)

	//全局信息
	DWORD							dwExperience;						//用户经验
};

//道具节点
struct tagPropertyTypeItem
{
	DWORD							dwTypeID;							//类型标识
	DWORD							dwSortID;							//排序标识
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//道具节点
struct tagPropertyRelatItem
{
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwTypeID;							//类型标识
};

//道具信息
struct tagPropertyItem
{
	//道具信息
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwPropertyKind;						//功能类型

	BYTE							cbUseArea;							//使用范围
	BYTE							cbServiceArea;						//对象范围
	BYTE							cbRecommend;						//推荐标识

	//销售价格
	SCORE							lPropertyGold;						//道具金币
	DOUBLE							dPropertyCash;						//道具价格
	SCORE							lPropertyUserMedal;					//道具金币
	SCORE							lPropertyLoveLiness;				//道具金币

	//获得财富
	SCORE							lSendLoveLiness;					//获得魅力（赠送方）
	SCORE							lRecvLoveLiness;					//获得魅力（接收方）
	SCORE							lUseResultsGold;					//获得金币

	//持续效果
	DWORD							dwUseResultsValidTime;				//有效时间
	DWORD							dwUseResultsValidTimeScoreMultiple;	//有效倍率

	//礼物拆包
	DWORD							dwUseResultsGiftPackage;			//礼物拆包

	DWORD							dwSortID;							 //排序标识
	TCHAR							szName[PROPERTY_LEN_NAME];			 //道具名称
	TCHAR							szRegulationsInfo[PROPERTY_LEN_INFO];//使用信息
};

//子道具信息
struct tagPropertySubItem
{
	DWORD							dwPropertyID;						//道具标识
	DWORD							dwOwnerPropertyID;					//道具标识
	DWORD                           dwPropertyCount;                    //道具数目
	DWORD							dwSortID;							//排序标识
};

//道具礼包
struct tagGiftPropertyInfo
{
	WORD                            wPropertyCount;                     //道具数目
	WORD							wPropertyID;						//道具标识
};

//道具使用
struct tagUsePropertyInfo
{
	WORD                            wPropertyCount;                     //道具数目
	WORD                            dwValidNum;						    //有效数字
	DWORD                           dwEffectTime;                       //生效时间
};


//用户道具
struct tagUserProperty
{
	WORD                            wPropertyUseMark;                   //道具标示
	tagUsePropertyInfo              PropertyInfo[MAX_PT_MARK];			//使用信息   
};

//道具包裹
struct tagPropertyPackage
{
	WORD                            wTrumpetCount;                     //小喇叭数
	WORD                            wTyphonCount;                      //大喇叭数
};

//时间信息
struct tagTimeInfo
{
	DWORD						dwEnterTableTimer;						//进出桌子时间
	DWORD						dwLeaveTableTimer;						//离开桌子时间
	DWORD						dwStartGameTimer;						//开始游戏时间
	DWORD						dwEndGameTimer;							//离开游戏时间
};

//////////////////////////////////////////////////////////////////////////
//好友信息

struct tagNearUserInfo
{
	//用户属性
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	DWORD							dwFaceID;							//头像 I D
	DWORD							dwCustomID;							//头像 I D
	BYTE							cbGender;							//用户性别
	WORD							wMemberOrder;						//会员等级
	WORD							wGrowLevel;							//用户等级
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//经度纬度
	DOUBLE							dLongitude;							//坐标经度
	DOUBLE							dLatitude;							//坐标纬度
	DWORD							dwDistance;							//目标距离	

	//用户地址
	DWORD							dwClientAddr;						//用户地址
};

//用户信息
struct tagFriendUserInfo
{
	//用户属性
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户账号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	DWORD							dwFaceID;							//头像 I D
	DWORD							dwCustomID;							//头像 I D
	BYTE							cbGender;							//用户性别
	WORD							wMemberOrder;						//会员等级
	WORD							wGrowLevel;							//用户等级
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实姓名

	//附属资料
	TCHAR							szQQ[LEN_QQ];						//用户QQ
	TCHAR							szEMail[LEN_EMAIL];					//用户Eamil
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//座机号码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
	TCHAR							szPostalCode[LEN_POSTALCODE];		//邮政编码

	//用户状态
	BYTE							cbMainStatus;						//用户状态
	BYTE							cbGameStatus;						//游戏状态
	WORD							wKindID;							//游戏标识
	WORD							wServerID;							//房间标识
	WORD							wTableID;							//桌子标识
	WORD							wChairID;							//椅子位置
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//手机型号
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器码
	DWORD							dwClientAddr;						//用户地址

	//经度纬度
	BYTE							cbCoordinate;						//开启标识
	DOUBLE							dLongitude;							//坐标经度
	DOUBLE							dLatitude;							//坐标纬度
};

//用户分组
struct tagClientGroupInfo
{
	BYTE							cbGroupIndex;						//分组索引
	TCHAR							szGroupName[GROUP_LEN_NAME];		//分组名称		
};

//好友信息
struct tagServerFriendInfo
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbMainStatus;						//用户状态
	BYTE							cbGameStatus;						//用户状态
	BYTE							cbGroupID;							//组别标识	
};

//好友信息
struct tagClientFriendInfo
{
	//用户属性
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	DWORD							dwFaceID;							//头像 I D
	DWORD							dwCustomID;							//头像 I D
	BYTE							cbGender;							//用户性别
	WORD							wMemberOrder;						//用户等级
	WORD							wGrowLevel;							//用户等级
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实姓名
	TCHAR							szPhoneMode[LEN_PHONE_MODE];		//手机型号
	BYTE							cbGroupID;							//组别标识	

	//在线信息
	BYTE							cbMainStatus;						//用户状态
	BYTE							cbGameStatus;						//游戏状态
	WORD							wKindID;							//游戏标识
	WORD							wServerID;							//房间标识
	WORD							wTableID;							//桌子标识
	WORD							wChairID;							//椅子位置
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//用户资料
struct tagUserIndividual
{
	DWORD							dwUserID;							//用户标识
	//附属资料
	TCHAR							szQQ[LEN_QQ];						//用户QQ
	TCHAR							szEMail[LEN_EMAIL];					//用户Eamil
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//座机号码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码	
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
	TCHAR							szPostalCode[LEN_POSTALCODE];		//邮政编码
};


//备注信息
struct tagUserRemarksInfo
{
	DWORD							dwFriendUserID;						//好友标识
	TCHAR							szRemarksInfo[LEN_REMARKS];			//备注信息
};

//财富信息
struct tagUserWealthInfo
{
	DWORD							dwUserID;							//用户标识
	LONGLONG						lUserGold;							//用户金币	
};




//////////////////////////////////////////////////////////////////////////

//用户信息
struct tagUserInfo
{
	//基本属性
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwGroupID;							//社团 I D
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	TCHAR							szQQ[LEN_QQ];						//QQ号码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识

	//用户资料
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wLastTableID;					    //游戏桌子
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lInsure;							//用户银行
	SCORE							lIngot;								//用户元宝
	DOUBLE							dBeans;								//游戏豆
	//私人房添加
	SCORE							lRoomCard;							//房卡数量

	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数	
	DWORD							dwExperience;						//用户经验
	LONG							lLoveLiness;						//用户魅力
	SCORE							lIntegralCount;						//积分总数(当前房间)

	//代理信息
	DWORD							dwAgentID;							//代理 I D

	TCHAR							szChangeLogonIP[LEN_IP];			//修改IP归属地

	//时间信息
	tagTimeInfo						TimerInfo;							//时间信息	
};

//用户信息
struct tagUserInfoHead
{
	//用户属性
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGroupID;							//社团 I D

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识

	//用户属性
	bool							bIsAndroid;							//机器标识
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lInsure;							//用户银行
	SCORE							lIngot;								//用户元宝
	DOUBLE							dBeans;								//用户游戏豆
	bool							bAndroid;							//是否为机器人
	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	DWORD							dwExperience;						//用户经验
	LONG							lLoveLiness;						//用户魅力
	SCORE							lIntegralCount;						//积分总数(当前房间)

	//代理信息
	DWORD							dwAgentID;							//代理 I D
};

//头像信息
struct tagCustomFaceInfo
{
	DWORD							dwDataSize;							//数据大小
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//图片信息
};
//头像信息96*96
struct tagCustomFaceInfo96
{
	DWORD							dwDataSize;							//数据大小
	DWORD							dwCustomFace[BIGFACE_CX*BIGFACE_CY];//图片信息
};

//用户信息
struct tagUserRemoteInfo
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	WORD							wFaceID;							//头像索引

	//等级信息
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	//位置信息
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识
	TCHAR							szGameServer[LEN_SERVER];			//房间位置

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wLastTableID;					    //游戏桌子
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态
};

//////////////////////////////////////////////////////////////////////////////////
//任务参数
struct tagTaskParameter
{
	//基本信息
	WORD							wTaskID;							//任务标识
	WORD							wTaskType;							//任务类型
	WORD							wTaskObject;						//任务目标
	BYTE							cbPlayerType;						//玩家类型
	WORD							wKindID;							//类型标识
	DWORD							dwTimeLimit;						//时间限制

	//奖励信息
	SCORE							lStandardAwardGold;					//奖励金币
	SCORE							lStandardAwardMedal;				//奖励奖牌
	SCORE							lMemberAwardGold;					//奖励金币
	SCORE							lMemberAwardMedal;					//奖励奖牌

	//描述信息
	TCHAR							szTaskName[LEN_TASK_NAME];			//任务名称
	TCHAR							szTaskDescribe[320];				//任务描述
};

//任务状态
struct tagTaskStatus
{
	WORD							wTaskID;							//任务标识
	WORD							wTaskProgress;						//任务进度
	BYTE							cbTaskStatus;						//任务状态
};

//低保参数
struct tagBaseEnsureParameter
{
	SCORE							lScoreCondition;					//游戏币条件
	SCORE							lScoreAmount;						//游戏币数量
	BYTE							cbTakeTimes;						//领取次数	
};

//推广信息
struct tagUserSpreadInfo
{
	DWORD							dwSpreadCount;						//推广人数
	SCORE							lSpreadReward;						//推广奖励
};

//等级配置
struct tagGrowLevelConfig
{
	WORD							wLevelID;							//等级 I D
	DWORD							dwExperience;						//相应经验
};

//等级参数
struct tagGrowLevelParameter
{
	WORD							wCurrLevelID;						//当前等级
	DWORD							dwExperience;						//当前经验
	DWORD							dwUpgradeExperience;				//下级经验
	SCORE							lUpgradeRewardGold;					//升级奖励
	SCORE							lUpgradeRewardIngot;				//升级奖励
};

//会员参数
struct tagMemberParameter
{
	BYTE							cbMemberOrder;						//会员标识
	TCHAR							szMemberName[16];					//会员名称
	SCORE							lMemberPrice;						//会员价格
	SCORE							lPresentScore;						//赠送游戏币
};

//会员参数
struct tagMemberParameterNew
{
	BYTE							cbMemberOrder;						//会员标识
	TCHAR							szMemberName[16];					//会员名称
	DWORD                           dwMemberRight;                      //会员权限
	DWORD                           dwMemberTask;                       //会员任务
	DWORD                           dwMemberShop;                       //会员商城
	DWORD                           dwMemberInsure;                     //会员银行
	DWORD                           dwMemberDayPresent;                 //会员送金
	DWORD                           dwMemberDayGiftID;                  //会员礼包
};

//实名参数
struct tagAuthRealParameter
{
	DWORD							dwAuthentDisable;					//验证开启
	SCORE							dwAuthRealAward;					//奖励金币
};
//////////////////////////////////////////////////////////////////////////////////

//广场子项
struct tagGamePlaza
{
	WORD							wPlazaID;							//广场标识
	TCHAR							szServerAddr[32];					//服务地址
	TCHAR							szServerName[32];					//服务器名
};

//级别子项
struct tagLevelItem
{
	LONG							lLevelScore;						//级别积分
	TCHAR							szLevelName[16];					//级别描述
};

//会员子项
struct tagMemberItem
{
	BYTE							cbMemberOrder;						//等级标识
	TCHAR							szMemberName[16];					//等级名字
};

//管理子项
struct tagMasterItem
{
	BYTE							cbMasterOrder;						//等级标识
	TCHAR							szMasterName[16];					//等级名字
};

//列表子项
struct tagColumnItem
{
	BYTE							cbColumnWidth;						//列表宽度
	BYTE							cbDataDescribe;						//字段类型
	TCHAR							szColumnName[16];					//列表名字
};

//地址信息
struct tagAddressInfo
{
	TCHAR							szAddress[32];						//服务地址
};

//数据信息
struct tagDataBaseParameter
{
	WORD							wDataBasePort;						//数据库端口
	TCHAR							szDataBaseAddr[32];					//数据库地址
	TCHAR							szDataBaseUser[32];					//数据库用户
	TCHAR							szDataBasePass[32];					//数据库密码
	TCHAR							szDataBaseName[32];					//数据库名字
};

//房间配置
struct tagServerOptionInfo
{
	//挂接属性
	WORD							wKindID;							//挂接类型
	WORD							wNodeID;							//挂接节点
	WORD							wSortID;							//排列标识

	//税收配置
	WORD							wRevenueRatio;						//税收比例
	SCORE							lServiceScore;						//服务费用

	//房间配置
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinTableScore;						//最低积分
	SCORE							lMinEnterScore;						//最低积分
	SCORE							lMaxEnterScore;						//最高积分

	//会员限制
	BYTE							cbMinEnterMember;					//最低会员
	BYTE							cbMaxEnterMember;					//最高会员

	//房间属性
	DWORD							dwServerRule;						//房间规则
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//用户信息
struct tagMobileUserInfoHead
{
	//用户属性
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwUserID;							//用户 I D

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识

	//用户属性
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lIngot;								//用户元宝
	DOUBLE							dBeans;								//用户游戏豆
	bool							bAndroid;							//是否为机器人
	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	DWORD							dwExperience;						//用户经验
	SCORE							lIntegralCount;						//积分总数(当前房间)

	//代理信息
	DWORD							dwAgentID;							//代理 I D
	DWORD							dwClientAddr;						//用户地址
	TCHAR							szChangeLogonIP[LEN_IP];			//修改IP归属地
};
//////////////////////////////////////////////////////////////////////////////////

//房间虚拟人数
struct tagServerDummyOnLine
{
	WORD							wServerID;							//房间标示
	DWORD							dwDummyMaxOnLine;					//虚拟在线最大区间
	DWORD							dwDummyMinOnLine;					//虚拟在线最小区间
	time_t							lTimeStart;							//变化起始时间
	time_t							lTimeEnd;							//变化结束时间
	long long						lAccumulation;						//累加变化量
	long long						lAccumulationCount;					//累计变化次数

	DWORD							dwCurDummyOnline;					//当前记录变化人数
};

//更新信息
struct tagUpdateInfo
{
	WORD							wStructSize;						//结构大小
	WORD							wUpdateCount;						//更新数目
	char							szDownLoadAddress[MAX_PATH];		//下载地址
};

//更新子项
struct tagUpdateItem
{
	WORD							wStructSize;						//结构大小
	char							szFileCheckSum[33];					//文件校验和
	char							szFileName[128];					//文件名称
};

//私人房间配置
struct tagPersonalTableParameter
{

	LONGLONG						lIniScore;										//初始游积分

	WORD wJoinGamePeopleCount;  //参加游戏的最大人数
	SCORE lCellScore;						//游戏底分

	DWORD  dwPlayTurnCount; 	 //私人放进行游戏的最大局数
	DWORD  dwPlayTimeLimit;		 //私人房进行游戏的最大时间 单位秒
	BYTE		  cbIsJoinGame;			 //是否参与游戏

	DWORD  dwTimeAfterBeginCount;//一局开始多长时间后解散桌子 单位秒
	DWORD  dwTimeOffLineCount;	 //掉线多长时间后解散桌子  单位秒
	DWORD  dwTimeNotBeginGame;  //多长时间未开始游戏解散桌子	 单位秒
	DWORD  dwTimeAfterCreateRoom; //私人房创建多长时间后无人坐桌解散桌子
};


//私人房间配置
struct tagPersonalTableFeeList
{
	DWORD							dwKindID;							//房间ID
	DWORD							dwDrawTimeLimit;					//时间限制
	DWORD							dwDrawCountLimit;					//局数限制
	LONGLONG						lIniScore;							//初始分数
	LONGLONG						lFeeScore;							//创建费用
};


//私人房间信息
struct tagPersonalTableInfo
{
	DWORD							dwServerID;							//房间I D
	DWORD							dwKindID;							//房间类型
	DWORD							dwTableID;							//桌子I D
	DWORD							dwUserID;							//桌主I D
	DWORD							dwDrawCountLimit;					//局数限制
	DWORD							dwDrawTimeLimit;					//时间限制
	SCORE								lCellScore;								// 房间底分
	TCHAR								szRoomID[ROOM_ID_LEN];		//房间ID
	DWORD							dwRoomTax;										//单独一个私人房间的税率，千分比
	WORD								wJoinGamePeopleCount;						//参与游戏的人数
	TCHAR								szPassword[LEN_PASSWORD];			//桌子密码
};


//私人房配置选项
struct tagPersonalRoomOption
{
	byte  cbCardOrBean;   //0 表示 消耗游戏豆  1 表示 消耗房卡	
	SCORE  lFeeCardOrBeanCount;   //消耗游戏豆 或 房卡 的数量	
	byte  cbIsJoinGame;		// 0 表示不参与游戏  1 标识参与游戏
	byte  cbMinPeople;	    //房间最小人数	
	byte  cbMaxPeople;	    //房间最大人数	
	SCORE lMaxCellScore;    //私人房的最大底分
	SCORE  lPersonalRoomTax;			//玩家初始分

	WORD	 wCanCreateCount;	 //玩家能够创建的私人房的最大数目
	DWORD  dwPlayTurnCount; 	 //私人放进行游戏的最大局数
	DWORD  dwPlayTimeLimit;	 //私人房进行游戏的最大时间 单位秒

	DWORD  dwTimeAfterBeginCount;	//一局开始多长时间后解散桌子 单位秒
	DWORD  dwTimeOffLineCount;		//掉线多长时间后解散桌子  单位秒
	DWORD  dwTimeNotBeginGame;		//多长时间未开始游戏解散桌子	 单位秒
	DWORD  dwTimeAfterCreateRoom; ////私人房创建多长时间后无人坐桌解散桌子
};


//私人房配置选项
const int FEE_OPTION_COUNT = 5;
struct CreateRoomPayFee
{
	DWORD  dwPlayTurnLimit;	//房间局数限制
	DWORD  dwPlayTimeLimit;	//房间时间限制
	DWORD  dwPayFee;				//创建房间消息费用
	DWORD  dwIniScore;			//房间初始积分
	CreateRoomPayFee()
	{
		dwPlayTurnLimit = 0;
		dwPlayTimeLimit = 0;
		dwPayFee = 0;
		dwIniScore = 0;
	}
};

struct tagPersonalRoomOptionRightAndFee
{
	byte cbCreateRight; //创建私人房间的权限
	CreateRoomPayFee stCreateRoomPayFee[FEE_OPTION_COUNT];
};




const int MAX_CREATE_PERSONAL_ROOM = 20;
const int MAX_CREATE_SHOW_ROOM = 20;
//房主创建房间信息
struct tagHostCreatRoomInfo
{
	DWORD dwUserID;
	DWORD	 dwKindID;
	TCHAR szRoomID[MAX_CREATE_PERSONAL_ROOM][ROOM_ID_LEN];

	tagHostCreatRoomInfo()
	{
		memset(szRoomID, 0, sizeof(TCHAR) * MAX_CREATE_PERSONAL_ROOM *ROOM_ID_LEN );
	}

};


//私人房间玩家信息
struct tagPersonalUserScoreInfo
{
	DWORD						dwUserID;							//玩家ID
	TCHAR							szUserNicname[LEN_NICKNAME];		//用户昵称

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lTaxCount;						//税收总数

	tagPersonalUserScoreInfo()
	{
		memset(this, 0, sizeof(tagPersonalUserScoreInfo));
	}

};

////私人房间玩家信息
//struct tagPersonalUserScoreInfo
//{
//	DWORD						dwUserID;							//玩家ID
//	TCHAR							szUserNicname[LEN_NICKNAME];		//用户昵称
//
//	//积分信息
//	SCORE							lScore;								//用户分数
//	SCORE							lGrade;								//用户成绩
//	SCORE							lTaxCount;						//税收总数
//	//输赢信息
//	DWORD							dwWinCount;						//胜利盘数
//	DWORD							dwLostCount;						//失败盘数
//	DWORD							dwDrawCount;						//和局盘数
//	DWORD							dwFleeCount;						//逃跑盘数
//
//	//全局信息
//	DWORD						dwExperience;				//用户经验
//	LONG							lLoveLiness;						//用户魅力
//	tagPersonalUserScoreInfo()
//	{
//		memset(this, 0, sizeof(tagPersonalUserScoreInfo));
//	}
//
//};

//用于请求单个私人房间信息
struct tagPersonalRoomInfo
{
	TCHAR szRoomID[ROOM_ID_LEN];
	byte  cbCardOrBean;					//0 表示 消耗游戏豆  1 表示 消耗房卡	
	SCORE  lFeeCardOrBeanCount;   //消耗游戏豆 或 房卡 的数量	
	DWORD  dwPlayTurnCount; 	 //私人放进行游戏的最大局数
	DWORD  dwPlayTimeLimit;		//私人房进行游戏的最大时间 单位秒
	byte 	  cbIsDisssumRoom;		//是否解散房间
	SYSTEMTIME sysCreateTime;   //私人房间创建时间
	SYSTEMTIME sysDissumeTime; //私人房间结束时
	LONGLONG   lTaxCount;			//税收总数
	tagPersonalUserScoreInfo PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];//私人房间所有玩家信息 
	tagPersonalRoomInfo()
	{
		cbCardOrBean = 0;
		lFeeCardOrBeanCount = 0;
		dwPlayTurnCount = 0;
		dwPlayTimeLimit = 0;
		cbIsDisssumRoom = 0;
		lTaxCount = 0;
		memset(szRoomID, 0, sizeof(TCHAR)  *ROOM_ID_LEN );
	}
};




////用于请求单个私人房间信息
//用于请求单个私人房间信息
struct tagQueryPersonalRoomUserScore
{
	TCHAR szRoomID[ROOM_ID_LEN];
	TCHAR	szUserNicname[LEN_NICKNAME];		//用户昵称
	DWORD  dwPlayTurnCount; 	 //私人放进行游戏的最大局数
	DWORD  dwPlayTimeLimit;		//私人房进行游戏的最大时间 单位秒
	SYSTEMTIME sysCreateTime;   //私人房间创建时间
	SYSTEMTIME sysDissumeTime; //私人房间结束时

	tagPersonalUserScoreInfo PersonalUserScoreInfo[PERSONAL_ROOM_CHAIR];//私人房间所有玩家信息 
	tagQueryPersonalRoomUserScore()
	{
		dwPlayTurnCount = 0;
		dwPlayTimeLimit = 0;
		memset(szRoomID, 0, sizeof(TCHAR)  *ROOM_ID_LEN );
	}
};
//////////////////////////////////////////////////////////////////////////////////

enum enUserLogonMode
{
	//登陆方式
	ULMByAccount,
	ULMByGameID,
	ULMByVisitor,
	ULMBySina,
	ULMByTencent,
	ULMByRenRen,
};

#pragma pack()

#endif