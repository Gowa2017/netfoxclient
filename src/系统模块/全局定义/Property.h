#ifndef PROPERTY_HEAD_FILE
#define PROPERTY_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//发行范围 issue area
#define PT_ISSUE_AREA_PLATFORM		0x01								//大厅道具（大厅可以使用）
#define PT_ISSUE_AREA_SERVER		0x02								//房间道具（在房间可以使用）
#define PT_ISSUE_AREA_GAME			0x04								//游戏道具（在玩游戏时可以使用）

//使用范围 service area
#define PT_SERVICE_AREA_MESELF		0x0001								//自己范围
#define PT_SERVICE_AREA_PLAYER		0x0002								//玩家范围
#define PT_SERVICE_AREA_LOOKON		0x0004								//旁观范围

//道具功能类型
enum PROP_KIND
{
	PROP_KIND_ALL=					0,		//所有
	PROP_KIND_GIFT=					1,		//礼物
	PROP_KIND_GEM=					2,		//宝石
	PROP_KIND_DOOUBLE=				3,		//积分双卡
	PROP_KIND_DEFENSE=				4,		//防身符
	PROP_KIND_DEFENSE_TICK=			5,		//防踢卡
	PROP_KIND_VIP=					6,		//vip卡
	PROP_KIND_TRUMPET=				7,		//大喇叭
	PROP_KIND_TYPHON=				8,		//小喇叭
	PROP_KIND_NEGATIVE_SCORE_CLEAR=	9,		//负分清零
	PROP_KIND_ESCAPE_CLEAR=			10,		//逃跑清零
	PROP_KIND_MENOY=				12,		//金币
	PROP_KIND_LOVELINESS=			13		//魅力
};



//道具类型
#define PT_TYPE_ERROR               0                                   //道具类型 
#define PT_TYPE_PROPERTY            1	                                //道具类型  
#define PT_TYPE_PRESENT             2                                   //道具类型

//道具定义
#define PROPERTY_ID_CAR				1									//汽车礼物
#define PROPERTY_ID_CLAP			2									//鼓掌礼物
#define PROPERTY_ID_KISS			3									//香吻礼物
#define PROPERTY_ID_BEER			4									//啤酒礼物
#define PROPERTY_ID_CAKE			5									//蛋糕礼物
#define PROPERTY_ID_RING			6									//钻戒礼物
#define PROPERTY_ID_SMOKE			7									//香烟礼物
#define PROPERTY_ID_VILLA			8									//别墅礼物
#define PROPERTY_ID_FLOWER			9									//鲜花礼物
#define PROPERTY_ID_BEAT			10									//暴打礼物
#define PROPERTY_ID_BOMB			11									//炸弹礼物
#define PROPERTY_ID_BRICK			12									//砖头礼物
#define PROPERTY_ID_EGG				13									//臭蛋礼物


#define PROPERTY_ID_CRYSTAL			101									//水晶
#define PROPERTY_ID_AGATE			102									//玛瑙
#define PROPERTY_ID_BOULDER			103									//玉石
#define PROPERTY_ID_GEM				104									//宝石
#define PROPERTY_ID_DIAMOND			105									//钻石

#define PROPERTY_ID_DOUBLE_1HOUR	201									//1小时双倍积分卡
#define PROPERTY_ID_DOUBLE_3HOUR	202									//3小时双倍积分卡
#define PROPERTY_ID_DOUBLE_1DAY		203									//1天双倍积分卡
#define PROPERTY_ID_DOUBLE_1WEEK	204									//1周双倍积分卡
#define PROPERTY_ID_AMULET_1HOUR	205									//1小时护身卡
#define PROPERTY_ID_AMULET_3HOUR	206									//3小时护身卡
#define PROPERTY_ID_AMULET_1DAY		207									//1天护身卡
#define PROPERTY_ID_AMULET_1WEEK	208									//1周护身卡
#define PROPERTY_ID_GUARDKICK_1HOUR 209									//1小时防踢道具
#define PROPERTY_ID_GUARDKICK_3HOUR 210									//3小时防踢道具
#define PROPERTY_ID_GUARDKICK_1DAY  211									//1天防踢道具
#define PROPERTY_ID_GUARDKICK_1WEEK 212									//1周防踢道具

#define PROPERTY_ID_TYPHON			306									//喇叭道具
#define PROPERTY_ID_TRUMPET			307									//喇叭道具
#define PROPERTY_ID_SCORE_CLEAR     308									//负分清零
#define PROPERTY_ID_ESCAPE_CLEAR    309									//逃跑清零

#define PROPERTY_ID_TWO_CARD	    14				                    //积分道具 
#define PROPERTY_ID_FOUR_CARD		15			                        //积分道具  
#define PROPERTY_ID_GUARDKICK_CARD  20									//防踢道具
#define PROPERTY_ID_POSSESS			21									//附身道具
#define PROPERTY_ID_BLUERING_CARD	22									//蓝钻道具
#define PROPERTY_ID_YELLOWRING_CARD	23									//黄钻道具
#define PROPERTY_ID_WHITERING_CARD	24  								//白钻道具 
#define PROPERTY_ID_REDRING_CARD	25									//红钻道具
#define PROPERTY_ID_VIPROOM_CARD	26									//VIP房道具

//////////////////////////////////////////////////////////////////////////////////


//道具信息
struct tagPropertyInfo
{
	//道具信息
	WORD							wIndex;								//道具标识
	WORD							wKind;								//功能类型
	WORD							wDiscount;							//会员折扣
	WORD							wUseArea;							//发布范围
	WORD							wServiceArea;						//服务范围
	WORD							wRecommend;							//推荐标识
	WORD							wScoreMultiple;						//功能倍数
	BYTE							cbSuportMobile;						//支持手机

	//销售价格
	SCORE							lPropertyGold;						//道具金币
	DOUBLE							dPropertyCash;						//道具价格
	SCORE							lPropertyUserMedal;					//道具金币
	SCORE							lPropertyLoveLiness;				//道具金币

	//赠送魅力
	SCORE							lSendLoveLiness;					//赠送魅力
	SCORE							lRecvLoveLiness;					//接受魅力
	SCORE							lUseResultsGold;					//获得金币
	
	TCHAR							szName[PROPERTY_LEN_NAME];			//道具名称
	TCHAR							szRegulationsInfo[PROPERTY_LEN_INFO];//使用信息
};


//道具属性
struct tagPropertyAttrib
{
	WORD							wIndex;								//道具标识
	WORD                            wPropertyType;                      //道具类型
	WORD							wServiceArea;						//使用范围
	TCHAR                           szMeasuringunit[8];					//计量单位 
	TCHAR							szPropertyName[32];					//道具名字
	TCHAR							szRegulationsInfo[256];				//使用信息
};


//单个背包道具
struct tagBackpackProperty
{
	int								nCount;								//道具个数
	tagPropertyInfo					Property;							//道具描述
};

//道具Buff状态
struct tagPropertyBuff
{
	DWORD	dwKind;														//功能类型
	DWORD	dwScoreMultiple;											//倍数（2倍积分卡，4倍积分卡）
	time_t	tUseTime;													//使用时刻的时间
	DWORD	UseResultsValidTime;										//持续时间
	TCHAR	szName[16];													//名字
};

//发送的道具
struct SendPresent
{
	DWORD							dwUserID;							//赠送者
	DWORD							dwRecvUserID;						//道具给谁
	DWORD							dwPropID;
	WORD							wPropCount;
	time_t							tSendTime;							//赠送的时间
	TCHAR							szPropName[16];						//道具名称
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif