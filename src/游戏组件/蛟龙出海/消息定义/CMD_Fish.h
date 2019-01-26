
#ifndef CMD_FISH_H_
#define CMD_FISH_H_
#include<vector>
#include<string>
using std::string;
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
// 服务定义

#define KIND_ID               408  //301
#define GAME_NAME             TEXT("金蝉捕鱼")
#define VERSION_SERVER        16777217        /*PROCESS_VERSION(1,0,0)*/
#define VERSION_CLIENT        16777217        /*PROCESS_VERSION(1,0,0)*/

// 简单的版本检测
// 如果客户端有更新应该更改这个值.如果只修改EXE的版本而网络数据包没修改的话，更新客户端是没什么意义的，因为EXE的版本是可以直接被修改的
// 只要保存以前的EXE版本,更新完之后再换回去，如果有BUG的话等于还是没修复。
#define GAME_VERSION          30
const BYTE kCmdVersion = 0x01;

//#ifndef SCORE
//#define SCORE LONGLONG
//#endif

const int kResolutionWidth = 1152;
const int kResolutionHeight = 720;

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#define M_PI_2  1.57079632679489661923f
#define M_PI_4  0.785398163397448309616f
#define M_1_PI  0.318309886183790671538f
#define M_2_PI  0.636619772367581343076f
#endif
//#ifdef GAMEPLAYER8
//#define GAME_PLAYER           8
//const float kChairDefaultAngle[GAME_PLAYER] = { M_PI, M_PI, M_PI, -M_PI_2, 0, 0, 0, M_PI_2 };
//#else
#define GAME_PLAYER           4
const float kChairDefaultAngle[GAME_PLAYER] = { M_PI, M_PI, 0, 0 };
//#endif

enum TraceType {
  TRACE_LINEAR = 0,
  TRACE_BEZIER
};

struct FPoint {
  float x;
  float y;
};

struct FPointAngle {
  float x;
  float y;
  float angle;
};

//////////////////////////////////////////////////////////////////////////
// 游戏定义

/*
// 座位号
-------------
    0   1   2 
7               3                
    6   5   4
-------------
*/

enum SceneKind {
  SCENE_KIND_1 = 0,
  SCENE_KIND_2,
  SCENE_KIND_3,
  SCENE_KIND_4,
  SCENE_KIND_5,

  SCENE_KIND_COUNT
};

enum FishKind {
  FISH_KIND_1 = 0,
  FISH_KIND_2,
  FISH_KIND_3,
  FISH_KIND_4,
  FISH_KIND_5,
  FISH_KIND_6,
  FISH_KIND_7,
  FISH_KIND_8,
  FISH_KIND_9,
  FISH_KIND_10,
  FISH_KIND_11,
  FISH_KIND_12,
  FISH_KIND_13,
  FISH_KIND_14,
  FISH_KIND_15,
  FISH_KIND_16,
  FISH_KIND_17,
  FISH_KIND_18, //金鲨
  FISH_KIND_19, //金龙
  FISH_KIND_20, // 企鹅
  FISH_KIND_21, // 李逵
  FISH_KIND_22, // 定屏炸弹
  FISH_KIND_23, // 局部炸弹
  FISH_KIND_24, // 超级炸弹
  FISH_KIND_25, // 大三元1   三条鱼一起
  FISH_KIND_26, // 大三元2   4条鱼一起
  FISH_KIND_27, // 大三元3
  FISH_KIND_28, // 大四喜1
  FISH_KIND_29, // 大四喜2
  FISH_KIND_30, // 大四喜3
  FISH_KIND_31, // 鱼王1
  FISH_KIND_32, // 鱼王2
  FISH_KIND_33, // 鱼王3
  FISH_KIND_34, // 鱼王4
  FISH_KIND_35, // 鱼王5
  FISH_KIND_36, // 鱼王6
  FISH_KIND_37, // 鱼王7
  FISH_KIND_38, // 鱼王8
  FISH_KIND_39, // 鱼王9
  FISH_KIND_40, // 鱼王10
  FISH_KIND_41,//宝箱
  FISH_KIND_COUNT
};

enum BulletKind {
  BULLET_KIND_1_NORMAL = 0,
  BULLET_KIND_2_NORMAL,
  BULLET_KIND_3_NORMAL,
  BULLET_KIND_4_NORMAL,
  BULLET_KIND_1_ION,
  BULLET_KIND_2_ION,
  BULLET_KIND_3_ION,
  BULLET_KIND_4_ION,

  BULLET_KIND_COUNT
};

const DWORD kLockTime = 10;

const int kMaxCatchFishCount = 2;
// 限制客户端的鱼种类数量
const int kFishKindCount[FISH_KIND_COUNT] = { 10,10,8,8,7,6,6,6,6,6,4,4,4,3,3,3,2,1,1,1,1,1,1,1,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1  };
//，是调整子弹射速的
static const DWORD kFireInterval = 330;
//记录
static std::vector<CString>jilu;
//////////////////////////////////////////////////////////////////////////
// 服务端命令

#define SUB_S_GAME_CONFIG                   100  //游戏控制
#define SUB_S_FISH_TRACE                    101  //鱼的轨迹
#define SUB_S_EXCHANGE_FISHSCORE            102   //上下分
#define SUB_S_USER_FIRE                     103  //开火
#define SUB_S_FISH_MISSED					104   //鱼不存在
#define SUB_S_CATCH_FISH                    105  //抓到鱼
#define SUB_S_BULLET_ION_TIMEOUT            106  //子弹变身时间到了
#define SUB_S_LOCK_TIMEOUT                  107  //锁定时间到
#define SUB_S_CATCH_SWEEP_FISH              108 //抓到BOSS和炸弹时
#define SUB_S_CATCH_SWEEP_FISH_RESULT       109   //抓到BOSOS和炸弹的结果
#define SUB_S_HIT_FISH_LK                   110    //伤害了李逵
#define SUB_S_SWITCH_SCENE                  111   //改变场景
#define SUB_S_SCENE_END                     112       //场景结束
#define SUB_S_HEIBAI_LIST					113		//黑白名单
#define SUB_S_FISHTESHU_LIST				114		//特殊鱼名单
#define SUB_S_ADMIN_CONFIG					115     //控制系统
#define SUB_S_ADMIN_FANGSHUI				116         //放水系统
#define SUB_chuansong                        117   //记录传输
#define SUB_iphao                          118   //IP号
#define SUB_iphao1                    119  //添加删除的IP号码
#define sub_iptianjiashanchu           120   //ip添加删除
#define SUB_fanhuiiDhao              121   //返回ID号
#define SUB_gengxin1               122   //更新

#define SUB_Smorenbaifenbi       125 //输赢默认百分比
#define SUB_baojing         126  //报警记录
#define SUB_zongKC        127 //总库存
#define SUB_S_NoFire  128 //别开枪
#define SUB_timerUp 129 //时间到了
#define SUB_S_canfire 130  //机器子弹限制
#define SUB_S_zongfen 131 //当前框里总分 主要是因为有些人把分数都能写错。
#define SUB_S_ControlCfg 132
#define SUB_S_UPDATE_ANDROID 138

struct CMD_S_GameStatus
{
	DWORD game_version;
	SCORE fish_score[GAME_PLAYER];
	SCORE exchange_fish_score[GAME_PLAYER];
	float g_bullet_multiple_[2];
	bool isyuzhen;

};

struct CMD_S_GameConfig
{//用户金币兑换比例
	int exchange_ratio_userscore;
	//用户鱼币兑换比例
	int exchange_ratio_fishscore;
	//单次兑换鱼币数量
	int exchange_count;
	//子弹最小最大值
	float min_bullet_multiple;
	float max_bullet_multiple;

	int bomb_range_width;
	int bomb_range_height;
	int bomb_stock;
	int super_bomb_stock;

	int fish_multiple[FISH_KIND_COUNT];
	int fish_speed[FISH_KIND_COUNT];
	int fish_bounding_box_width[FISH_KIND_COUNT];
	int fish_bounding_box_height[FISH_KIND_COUNT];
	int fish_hit_radius[FISH_KIND_COUNT];

	int bullet_speed[BULLET_KIND_COUNT];
	int net_radius[BULLET_KIND_COUNT];


	int RobotScoreMin;
	int RobotScoreMax;
	int RobotBankGet;
	int RobotBankGetBanker;
	int RobotBankStoMul;

	bool bIsGameCheatUser;
};

struct CMD_S_FishTrace
{
	FPoint init_pos[5]; // 比萨尔和直线的点
	int init_count; // 比塞尔就是 3个 直线就是 2个
	BYTE cmd_version; //版本号
	FishKind fish_kind; // 鱼的种类
	int fish_id;  //鱼的 ID
	TraceType trace_type;
};

struct CMD_S_ExchangeFishScore
{
	WORD chair_id;
	//每次上下分的钱
	SCORE swap_fish_score;
	//改变总额，输了的话则为正，赢了的话则为负
	SCORE exchange_fish_score;
};

struct CMD_S_UserFire
{
	BulletKind bullet_kind;
	int bullet_id;
	WORD chair_id;
	WORD android_chairid;
	float angle;
	float bullet_mulriple;
	int lock_fishid;
	SCORE fish_score;
	float fire_speed;
};

struct CMD_S_FishMissed
{
	WORD chair_id;
	float bullet_mul;
};
struct CMD_S_CatchFish
{
	WORD chair_id;
	int fish_id;
	FishKind fish_kind;
	//变身子弹
	bool bullet_ion;
	SCORE fish_score;
	SCORE fish_caijin_score;
	int app;
};

struct CMD_S_BulletIonTimeout
{
	WORD chair_id;
};

struct CMD_S_CatchSweepFish
{
	WORD chair_id;
	int fish_id;
	float bullet_mul;
};

struct CMD_S_CatchSweepFishResult
{
	WORD chair_id;
	int fish_id;
	SCORE fish_score;
	int catch_fish_count;
	int catch_fish_id[300];
};
struct tianjiashanchu
{

	DWORD abc;
	double gailv;
};
struct CMD_S_a
{
	DWORD abc[30];
	double gailv[30];
};
struct CMD_S_HitFishLK
{
	WORD chair_id;
	int fish_id;
	int fish_mulriple;
};

struct CMD_S_SwitchScene
{
	SceneKind scene_kind;
	int fish_count;
	FishKind fish_kind[300];
	int fish_id[300];
};

struct CMD_S_WhiteBlackList
{
	int WhiteList[30];
	int BlackList[30];
};
struct ipbaocun
{
	DWORD ip23;
	DWORD id23;
	DWORD USERID;
};

struct userIDD
{
	DWORD  UserID;
};
struct  iphaoshuzu
{
	DWORD iphao[30];
	 DWORD gameid[30];
};

struct CMD_S_SpecialList
{
	int fish20gameid[30];
	int fish20gameidkind[30];
	int fish20count[30];
	double fish20probability[30];

	int fish21gameid[30];
	int fish21gameidkind[30];
	int fish21count[30];
	double fish21probability[30];

	int fishjinsha[30];
	int fishjinshakind[30];
	int fishjinshacount[30];
	double fishjinshaprobability[30];
};

//下面装不下 放这了
struct CMD_S_CONFIG
{
	SCORE						 stock_crucial_score_All[21];
	double					 stock_increase_probability_All[21];
	bool RHRHvar3;
	int BulletCount;
	int LangchaoTime;
	DWORD BulletTimer;
	DWORD Bullet_Count;
	double heibaigailv[2];
	double Bullet_gailv;
};

struct CMD_S_CONTROLCFG
{
	double					 total_return_rate_;						//返还百分比
	double 					 revenue_score;								//游戏抽水 
	double                   zhengtigailv;								//整体概率
	double 				 stock_score0;						//库存配置
	double 				 stock_score1;
	double                 hard;					// 困难
	double                 easy;					//简单
};

//管理申请结果
struct CMD_S_ADMIN_CONFIG
{
	//
	double					 total_return_rate_;						//返还百分比
	SCORE					 revenue_score;								//游戏抽水
	int						 exchange_ratio_userscore_;					//金币兑换比例
	int						 exchange_ratio_fishscore_;					//鱼币兑换比例
	int						 exchange_fishscore_count_;					//单次兑换数量
	double                       zhengtigailv;								//整体概率

	float						 bullet_multiple[2];
	//库存配置
	LONGLONG				 stock_score[2];
	SCORE						 stock_crucial_score_[12];
	SCORE						 stock_crucial_score_small[12];
	double					 stock_increase_probability_[12];
	double					 stock_increase_probability_small[12];
	//鱼类配置
	int						 nFishMinMultiple[FISH_KIND_COUNT];
	int						 nFishMaxMultiple[FISH_KIND_COUNT];
	int						 nFishSpeed[FISH_KIND_COUNT];
	double					 fFishGailv[FISH_KIND_COUNT];
	int						 win_score_limit_[20];					//输赢档位限制
	double					 win_decrease_probability_[20];			//输赢档位概率
	int                    bodongshijian;         //波动时间
	double                 hard;					// 困难
	double                 easy;					//简单
	int                   bodongmax;           //波动最大值
	double                paogailv[10];    
	double				 zuoweigailv[8];
	DWORD  qijiren[2];
	double qijirengailv;
	DWORD  jiqirenleave[2];
};
//炮
struct pao10086
{
	double                paogailv[10];

};
struct qujian
{
	SCORE qujianmax;
	SCORE qujianmin;
	bool panduan;
};
struct winlose
{
	bool var233[4];
	double win233;
	double lose233;
	SCORE qujianmax;
	SCORE qujianmin;
};
struct qijiren
{
	DWORD time1[2];

};


//放水系统
struct CMD_S_ADMIN_FANGSHUI
{
	//放水配置
	bool					 bFangShuiStart;						//放水状态
	SCORE					 stock_fangshui_edu;					//放水额度
	double					 stock_fangshui_probability;			//放水概率
	DWORD					 fangshui_time;							//持续时间
	SCORE					 fangshui_score;						//已放水金额
	DWORD                    fangshui_Begin;                    //放水开始时间
	DWORD					 fangshui_count_time;					//已持续放水多少时间
	SCORE					 lastfangshui_score;					//上次已放水金额
	DWORD					 lastfangshui_count_time;				//上次已持续放水多少时间
	DWORD					 nextfangshui_time;						//下次放水开始时间
	SCORE					 fangshuixiaoyukucun;                //放水小鱼库存
	SCORE					 fangshuidayukucun;						//放水大鱼库存
	SCORE                   meitian_edu;                         //每天额度
	SCORE					 fangshuixiaoyukucun1 ;              //记录放水库存
	SCORE					 fangshuidayukucun1;						//
	DWORD                    youxiid1[2];               //波动椅子
	/*WORD                     yizihao1[2];*/         //椅子号
	DWORD                     zaixianwanjia;    //在线玩家数量
};

//放水系统
struct CMD_S_UPDATE_ANDROID
{
   int fishCount;
};
//////////////////////////////////////////////////////////////////////////
// 客户端命令

#define SUB_C_EXCHANGE_FISHSCORE            101
#define SUB_C_USER_FIRE                     102
#define SUB_C_CATCH_FISH                    103
#define SUB_C_CATCH_SWEEP_FISH              104
#define SUB_C_HIT_FISH_I                    105 //伤害LK
#define SUB_C_USER_FILTER                   106
#define SUB_C_ANDROID_STAND_UP              107

#define SUB_C_HEIBAI_LIST					108
#define SUB_C_FISHTESHU_LIST				109

#define SUB_C_FISH20_CONFIG                 110
#define SUB_C_FISH21_CONFIG                 111
#define SUB_C_FISHJINSHA_CONFIG             112

#define SUB_C_ADMIN_CONFIG					114
#define SUB_C_ADMIN_FANGSHUI				115
#define SUB_C_jilu                          116
#define SUB_C_ip							117

#define SUB_C_ip1                         118
#define SUB_C_ip2                        119
#define SUB_C_ipfasong                  135
#define SUB_pao                   120
#define sub_tichu                    121

#define SUB_gengxin                122
#define SUB_jiqiren              123
#define SUB_jiqirengailv          133
#define SUB_jiqirenjinchu        134
#define SUB_tiaozheng    139
#define SUB_huoqu 140
#define SUB_winlose 141
#define SUB_kucun 142
#define SUB_C_qujian 143
#define SUB_weizhigailv 144
#define SUB_baojingjilu 145
#define SUB_shanchujilu 146
#define SUB_shezhi 147
#define SUB_zongkucun 148
#define SUB_zongkucunxiugai 149
#define SUB_C_BulletCount 150
#define SUB_C_LangChaoTime 151
#define SUB_C_SuPerGaiLv 152
#define SUB_C_SuperTimer 153
#define SUB_C_FISH_Count 154
#define SUB_C_heibaigailv 155 
#define SUB_C_GetSControl 156
#define SUB_C_ControlCfg 157

struct CMD_C_ExchangeFishScore {
  bool increase;
};

struct CMD_C_UserFire {
  int bullet_id;	//子弹 ID
  BulletKind bullet_kind; //子弹种类
  BYTE cmd_version; //版本号
  float angle;	// 子弹角度
  float bullet_mulriple; //子弹倍率 1000~1万
  int lock_fishid; //锁定鱼的ID号
  DWORD fire_time; //开火时间
  float fire_speed; //开火速度
};
//击中鱼群
struct CMD_C_CatchFish {
  WORD chair_id;
  int fish_id;
  int bullet_kind;
  int bullet_id;
  float bullet_mulriple;
};

struct CMD_C_CatchSweepFish {
  WORD chair_id;
  int fish_id;
  int catch_fish_count;
  int catch_fish_id[300];
};

struct CMD_C_HitFishLK {
  int fish_id;
};



struct CMD_C_ChackList {
	unsigned char operate_code;
};


struct CMD_C_UserFilter {
  DWORD game_id;
  unsigned char operate_code; // 0 黑名单 1 白名单 2 清除
};
struct CMD_gengxin{

	SCORE daxiaoyu[2];
};
struct CMD_C_Fish20Config {
  DWORD game_id;
  int catch_count;
  double catch_probability;
};

#pragma pack()

#endif // CMD_FISH_H_