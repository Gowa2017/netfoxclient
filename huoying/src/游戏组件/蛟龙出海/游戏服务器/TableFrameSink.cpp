#include "StdAfx.h"
#include "TableFrameSink.h"
#include "tinyxml/tinyxml.h"
#include <stdlib.h>
#include <time.h>
#include <map>
#include<set>
#include <iterator>
#include <stdlib.h>
#include <cmath>

typedef CMap<DWORD, DWORD, SCORE, SCORE&> UserWinScoreMap;

// 应当注意：计时器ID的范围是1-20 ( >=1 && < 20 )
// 这里有超过20的 所以要把GameServiceHead.h的TIME_TABLE_SINK_RANGE改大点才行，或者改进计时器，比如共用
//特别是不同框架 突然发现定时器不能用了！！！
#define  TIME_GAME_LOOP 50
const DWORD kBulletIonTimer = 1;
const DWORD kLockTimer = 9;
const DWORD kClearTraceTimer = 10;
const DWORD kBuildSmallFishTraceTimer = 11;
const DWORD kBuildMediumFishTraceTimer = 12;
const DWORD kBuildFish18TraceTimer = 13;
const DWORD kBuildFish19TraceTimer = 14;
const DWORD kBuildFish20TraceTimer = 15;
const DWORD kBuildFishLKTraceTimer = 16;
const DWORD kBuildFishBombTraceTimer = 17;
const DWORD kBuildFishSuperBombTraceTimer = 18;
const DWORD kBuildFishLockBombTraceTimer = 19;
const DWORD kBuildFishSanTraceTimer = 20;
const DWORD kBuildFishSiTraceTimer = 21;
const DWORD kBuildFishKingTraceTimer = 22;
const DWORD kSwitchSceneTimer = 23;
const DWORD kSceneEndTimer = 24;
const DWORD kLKScoreTimer = 25;
const DWORD kFangShuiTimer = 26;				//放水检测
const DWORD dongdongbo = 27;             //波动
const DWORD jiqirenhuanzhuang = 28;
const DWORD min5jilu = 29;
const DWORD daodiantichu = 30;
const DWORD yifenzhongbaocun = 31;
const DWORD TBuildFish41TraceTimer = 32;
const DWORD kGameLOOP = 33;
const  char * fdd = NULL;

const DWORD kClearTraceElasped = 60/*60*/;
const DWORD checkfire = 1.5;
const DWORD BJjilu = 300;
const DWORD kBuildSmallFishTraceElasped = 2;
const DWORD kBuildMediumFishTraceElasped = 3;
const DWORD kBuildFish18TraceElasped = 35/*45*//*72*/; 
const DWORD kBuildFish19TraceElasped = 50/*152*//*152*/;
const DWORD kBuildFish20TraceElasped = 130/*205*//*296*/;
const DWORD kBuildFishLKTraceElasped = 30/*37*//*48*/;
const DWORD kBuildFishBombTraceElasped = 92;
const DWORD kBuildFishSuperBombTraceElasped = 122;
const DWORD kBuildFishLockBombTraceElasped = /*250*/290;
const DWORD kBuildFishSanTraceElasped = 120 + 25;
const DWORD kBuildFishSiTraceElasped = 135;
const DWORD kBuildFishKingTraceElasped = 40;
const DWORD TBuildFish41TraceElasped = 200;
static DWORD kSwitchSceneElasped =  100; //切换场景
const DWORD kSceneEndElasped = 20/*63*/;
const DWORD kLKScoreElasped = 1;
const DWORD kFangShuiElasped = 1;
const DWORD kRepeatTimer = (DWORD)0xFFFFFFFF;
const DWORD kFishAliveTime = 350000;
static DWORD    T_baoxiang[] = { 20, 50, 100, 300 ,500,1000,5000,50000};
static SCORE    g_stock_fangshui_yue = 0;
static SCORE	g_stock_fangshui_edu = 100000 ;						//放水额度
static SCORE    g_stock_meitian_edu = 100000;                       //每天额度
static double	g_stock_fangshui_probability = 2.0;					//放水概率
static DWORD	g_fangshui_cur_time = 0;							//持续时间
static DWORD	g_fangshui_time = 1;								//持续时间
static bool		g_bFangShuiStart=false;								//放水状态
static SCORE	g_fangshui_cur_score = 0;							//已放水金额
static DWORD	g_fangshui_begin_time = 0;							//已持续放水多少时间
static DWORD	g_fangshui_end_time = 0;							//已持续放水多少时间
static SCORE	g_lastfangshui_score = 0;							//上次已放水金额
static DWORD	g_lastfangshui_count_time = 0;						//上次已持续放水多少时间
static DWORD	g_nextfangshui_time = 0;							//下次放水开始时间
static int    time_a = 0;                       //随机3次
static int      g_hhhhhhh = 0;
static SCORE      g_fffffff = 0;
static  bool   g_sssssss = true, g_wwwww = true, T_bodong = true;
static double jiqirengailv = 0.1;
static SCORE  g_MAXbaojing = 1000000;
static int BulletCount = 20;
SCORE qujianfanwei[46] = {0};
float qjgl[] = { 0.8, 0.81, 0.82, 0.83, 0.84,0.85,0.86,0.87,0.88,0.9,0.91,0.915,0.92,0.93,0.94,0.95,0.96,0.965,0.97,0.975,0.98,0.99,1.12,1.13,1.14,1.15,1.16,1.17,1.18,1.19,1.21,1.22,1.23,1.24,1.25,1.26,1.27,1.11,1.12,1.13,1.14,1.15,1.16,1.17,1.21,1.22};
//用户金币兑换比例
static int g_exchange_ratio_userscore_ = 0;
//用户鱼币兑换比例
static int g_exchange_ratio_fishscore_ = 1;
//单次兑换鱼币数量
static int g_exchange_fishscore_count_ = 1000;
//总体返还分数百分比
static double g_total_return_rate_ = 0.99;
// 总体概率
static double g_total_probability_=1.0;
//大鱼小鱼初始库存
static SCORE g_stock_score_[2] = { 0, 0 };
//某椅子的难易度
//static WORD  yizihao[2] = { -1, -1 };
//波动玩家ID
//static DWORD youxiID[2] = { -1, -1 };
//波动时间
//static int S_bodongshijian = 60;
//static int s_bodongmax = 60;
//计算波动的时间10分钟
//static int suanshijian = 0;
static double chairgailvp[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
static bool b_f =true;
//波动难度
static double S_hard = 0.3;
//波动易度
static double S_easy = 1.5;
//放水大小鱼库存
static SCORE fangshuikucun[2] = { 0, 0 };
//记录库存
static SCORE jilukucun[2] = { 0, 0 };
//判断
static bool panduan = true;
//判断1
static bool panduan1 = true;
//判断2
static bool panduan2 = true;
//判断3
static bool panduan3 = true;
//文件大小
static int wenjiandx = 0;
//子弹最小最大值
static float g_bullet_multiple_[2] = { 1000, 9900 };
//大鱼库存档位
static SCORE g_stock_crucial_score_[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//小鱼库存档位
static SCORE g_stock_crucial_score_small[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0,0};
//总库存档位
static SCORE g_stock_crucial_score_all[21] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0};
//大鱼库存档位概率
static double g_stock_increase_probability_[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0,0};
//小鱼库存档位概率
static double g_stock_increase_probability_small[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0 };
//总库存档位概率
static double g_stock_increase_probability_all[21] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0};

//大鱼库存档位数量
static int g_stock_crucial_count_=0;
//小鱼库存档位数量
static int g_stock_crucial_count_small=0;
//大小鱼总库存档位数量
static int g_stock_crucial_count_all = 0;
//赢分档位分值
static int g_win_score_limit_[20] = { 0 };
//赢分档位概率
static double g_win_score_probability_[20] = { 0 };
//赢分档位数量
static double paogailv[10] = { 1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f };
const double paoqian[10] = { 100, 200, 500, 1000, 5000, 10000, 100000, 500000, 1000000, 5000000 };
static int g_win_score_limit_count_ = 0;
//局部炸弹 当库存小于此值炸弹被击中的几率为0
static int g_bomb_stock_ = 1000000;
//超级炸弹 当库存小于此值炸弹被击中的几率为0
static int g_super_bomb_stock_ = 4000000;

//彩金池大于（当前值）开启彩金 
static SCORE g_BeginCaijin = 1000000;
//（当前值）为中彩金最小值 
static SCORE g_MinCaijing = 1000000;
//（当前值）为中彩金最大值 
static SCORE g_MaxCaijing = 3000000;
//税收的千分之（当前值）作为彩金池分
static int   g_RateToCaijinChi = 1000;
static DWORD  qijirentime[2] = {100,120};
static DWORD  likaishijian[2] = { 300, 5000 };
//机器人的分数小于该值取款
static int g_RobotScoreMin = 10000000;
//机器人的分数大于该值存款
static int g_RobotScoreMax = 100000000;
//取款最小数额
static int g_RobotBankGet = 30000000;
//取款最大数额(此数值一定要大于RobotBankGet)
static int g_RobotBankGetBanker = 9000000;
//存款百分比，百分之N
static int g_RobotBankStoMul = 100;
//每张桌子机器人最小个数
static int g_TableMinAndroidCount = 4;
//每张桌子机器人最大个数
static int g_TableMaxAndroidCount = 4;

static int g_fish_speed_[FISH_KIND_COUNT] = { 0 };
//鱼的倍率
static int g_fish_multiple_[FISH_KIND_COUNT] = { 0 }; 
static int g_fish18_max_multiple_ = 0;  
static int g_fish19_max_multiple_ = 0; 
static int g_fishLK_max_multiple_ = 0;
static int g_fish_bounding_box_width_[FISH_KIND_COUNT] = { 0 }; //鱼的边框宽度
static int g_fish_bounding_box_height_[FISH_KIND_COUNT] = { 0 };//鱼的边框高度
static int g_fish_hit_radius_[FISH_KIND_COUNT] = { 0 }; //鱼的圆角
static double g_fish_capture_probability_[FISH_KIND_COUNT] = { 0 };
static double caijin[FISH_KIND_COUNT] = { 0 };

static int g_bomb_range_width_ = 0;
static int g_bomb_range_height_ = 0;

static int g_bullet_speed_[BULLET_KIND_COUNT] = { 0 };
//netRadius:渔网的半径
static int g_net_radius_[BULLET_KIND_COUNT] = { 0 };
static int zaixianwanjia; //在线玩家
static int winuser1; //变动赢家
static int loseuser1;//变动输家
static int savechipingcount;//保存持平数
static double winpercentage = 0.4; //赢得百分比
static double losepercentage = 0.5; //输得百分比
static int winuser2;
static int loseuser2;
static bool Bvar = 0, Bvar1 = 1, Bvar2 = 1, RHvar3 = 0;
static SCORE g_revenue_score = 0; //税收总分
static double heibaigailv[2] = { 0.5, 1.5 };
static std::vector<DWORD> g_balck_list_; 
static std::vector<DWORD> g_white_list_;
static std::vector<ipbaocun> iphao1;
static std::map<DWORD,double>iphao2;
static std::map<DWORD, double>biandonguser;
static std::vector<CString> baojingjilu;
// 每个用户在房间的赢的分数
UserWinScoreMap g_user_win_score_;
static double SuperPaoGailv = 1;
DWORD kBulletIonTime = 8;

//时间间隔反作弊
DWORD   g_timerInterval_ = 20000;
static bool one = true;
//抓了多少鱼后变身
DWORD yushu = 120;
//CRITICAL_SECTION critial_section_;

#define ERROR_COUNTS		15      //错误次数

struct Fish20Config 
{
  DWORD game_id;
  int catch_count;
  double catch_probability;
};

struct Fish21Config 
{
	DWORD game_id;
	int catch_count;
	double catch_probability;
};

struct FishJinShaConfig 
{
	DWORD game_id;
	int catch_count;
	double catch_probability;
};

static std::vector<Fish20Config> g_fish20_config_;
static std::vector<Fish21Config> g_fish21_config_;
static std::vector<FishJinShaConfig> g_fishjinsha_config_;

void AddUserFilter(DWORD game_id, unsigned char operate_code)
{
	std::vector<DWORD>::iterator iter;
	std::vector<Fish20Config>::iterator iter2;
	std::vector<Fish21Config>::iterator iter3;
	std::vector<FishJinShaConfig>::iterator iter4;
	if (operate_code == 0) 
	{
		for (iter = g_balck_list_.begin(); iter != g_balck_list_.end(); ++iter)
		{
			if ((*iter) == game_id) return;
		}
		g_balck_list_.push_back(game_id);
	}
	else if (operate_code == 1)
	{
		for (iter = g_white_list_.begin(); iter != g_white_list_.end(); ++iter)
		{
			if ((*iter) == game_id) return;
		}
		g_white_list_.push_back(game_id);
	}
	else if (operate_code == 2)
	{
		for (iter = g_balck_list_.begin(); iter != g_balck_list_.end(); ++iter)
		{
			if ((*iter) == game_id)
			{
				iter = g_balck_list_.erase(iter);
				break;
			}
		}
		for (iter = g_white_list_.begin(); iter != g_white_list_.end(); ++iter)
		{
			if ((*iter) == game_id)
			{
				iter = g_white_list_.erase(iter);
				break;
			}
		}
	}
	else if (operate_code == 3)
	{
		for (iter2 = g_fish20_config_.begin(); iter2 != g_fish20_config_.end(); ++iter2) {
			Fish20Config &tmp = *iter2;
			if (tmp.game_id == game_id)
			{
				g_fish20_config_.erase(iter2);
				break;
			}
		}

		for (iter3 = g_fish21_config_.begin(); iter3 != g_fish21_config_.end(); ++iter3) {
			Fish21Config &tmp = *iter3;
			if (tmp.game_id == game_id)
			{
				g_fish21_config_.erase(iter3);
				break;
			}
		}

		for (iter4 = g_fishjinsha_config_.begin(); iter4 != g_fishjinsha_config_.end(); ++iter4) {
			FishJinShaConfig &tmp = *iter4;
			if (tmp.game_id == game_id)
			{
				g_fishjinsha_config_.erase(iter4);
				break;
			}
		}
	}
}

// 返回值 0：黑名单  1 白名单 -1 正常
int CheckUserFilter(DWORD game_id) 
{
  std::vector<DWORD>::iterator iter;
  for (iter = g_balck_list_.begin(); iter != g_balck_list_.end(); ++iter) 
  {
    if ((*iter) == game_id) return 0;
  }

  for (iter = g_white_list_.begin(); iter != g_white_list_.end(); ++iter) 
  {
    if ((*iter) == game_id) return 1;
  }

  return -1;
}

void AddFish20Config(DWORD game_id, int catch_count, double catch_probability)
{
	std::vector<Fish20Config>::iterator iter;
	for (iter = g_fish20_config_.begin(); iter != g_fish20_config_.end(); ++iter)
	{
		Fish20Config& config = *iter;
		if (game_id == config.game_id)
		{
			if (catch_count == 0)
			{
				g_fish20_config_.erase(iter);
			}
			else
			{
				config.catch_count = catch_count;
				config.catch_probability = catch_probability;
			}
			return;
		}
	}

	Fish20Config config;
	config.game_id = game_id;
	config.catch_count = catch_count;
	config.catch_probability = catch_probability;
	g_fish20_config_.push_back(config);
}

void AddFish21Config(DWORD game_id, int catch_count, double catch_probability)
{
	std::vector<Fish21Config>::iterator iter;
	for (iter = g_fish21_config_.begin(); iter != g_fish21_config_.end(); ++iter)
	{
		Fish21Config& config = *iter;
		if (game_id == config.game_id)
		{
			if (catch_count == 0)
			{
				g_fish21_config_.erase(iter);
			}
			else
			{
				config.catch_count = catch_count;
				config.catch_probability = catch_probability;
			}
			return;
		}
	}

	Fish21Config config;
	config.game_id = game_id;
	config.catch_count = catch_count;
	config.catch_probability = catch_probability;
	g_fish21_config_.push_back(config);
}

void AddFishJinShaConfig(DWORD game_id, int catch_count, double catch_probability)
{
	std::vector<FishJinShaConfig>::iterator iter;
	for (iter = g_fishjinsha_config_.begin(); iter != g_fishjinsha_config_.end(); ++iter)
	{
		FishJinShaConfig& config = *iter;
		if (game_id == config.game_id)
		{
			if (catch_count == 0)
			{
				g_fishjinsha_config_.erase(iter);
			}
			else
			{
				config.catch_count = catch_count;
				config.catch_probability = catch_probability;
			}
			return;
		}
	}

	FishJinShaConfig config;
	config.game_id = game_id;
	config.catch_count = catch_count;
	config.catch_probability = catch_probability;
	g_fishjinsha_config_.push_back(config);
}

bool CheckFish20Config(DWORD game_id, int* catch_count, double* catch_probability)
{
	std::vector<Fish20Config>::iterator iter;
	for (iter = g_fish20_config_.begin(); iter != g_fish20_config_.end(); ++iter)
	{
		Fish20Config& config = *iter;
		if (game_id == config.game_id)
		{
			if (config.catch_count <= 0)
			{
				g_fish20_config_.erase(iter);
				return false;
			}
			*catch_count = config.catch_count;
			*catch_probability = config.catch_probability;
			return true;
		}
	}
	return false;
}

bool CheckFish21Config(DWORD game_id, int* catch_count, double* catch_probability)
{
	std::vector<Fish21Config>::iterator iter;
	for (iter = g_fish21_config_.begin(); iter != g_fish21_config_.end(); ++iter)
	{
		Fish21Config& config = *iter;
		if (game_id == config.game_id)
		{
			if (config.catch_count <= 0)
			{
				g_fish21_config_.erase(iter);
				return false;
			}
			*catch_count = config.catch_count;
			*catch_probability = config.catch_probability;
			return true;
		}
	}
	return false;
}

bool CheckFishJinShaConfig(DWORD game_id, int* catch_count, double* catch_probability)
{
	std::vector<FishJinShaConfig>::iterator iter;
	for (iter = g_fishjinsha_config_.begin(); iter != g_fishjinsha_config_.end(); ++iter)
	{
		FishJinShaConfig& config = *iter;
		if (game_id == config.game_id)
		{
			if (config.catch_count <= 0)
			{
				g_fishjinsha_config_.erase(iter);
				return false;
			}
			*catch_count = config.catch_count;
			*catch_probability = config.catch_probability;
			return true;
		}
	}
	return false;
}

// 修改赢分
void ModifyUserWinScore(IServerUserItem* user_item, SCORE win_score)
{
	if (user_item == NULL)
	{
		return;
	}

	if (user_item->IsAndroidUser())	return;
	SCORE lScore=0;
	if (g_user_win_score_.Lookup(user_item->GetUserID(), lScore))
	{
		g_user_win_score_[user_item->GetUserID()]+=win_score;
	}
	else
	{
		g_user_win_score_.SetAt(user_item->GetUserID(), win_score);
	}

}

// 根据赢分调整概率
bool AdjustProbability(IServerUserItem* user_item, double* probability, int jizhong)
{
	if (user_item == NULL)
	{
		*probability = 0;
		return true;
	}
	if (user_item->IsAndroidUser())
	{
		return false;
	}

	//// 白钻红钻不调整概率
	//if (user_item->GetMemberOrder() == 4)
	//  return false;

	SCORE lScoreTemp=0;
	if (!g_user_win_score_.Lookup(user_item->GetUserID(), lScoreTemp))
	{
		*probability = 0;
		return true;
	}

	int win_score_limit_count = g_win_score_limit_count_;


	while ((--win_score_limit_count) >= 0)
	{
		if (g_win_score_limit_[win_score_limit_count] < 0)
			break;
		if (lScoreTemp >= g_win_score_limit_[win_score_limit_count])
		{
			/*	if (jizhong==0)*/
			{
				*probability = *probability * (1 - g_win_score_probability_[win_score_limit_count]);
			}
			//else if (jizhong==1&&it->second>=1000000)
			//{
			//	*probability=0;
			//}
			//else if (jizhong==2&&it->second>=2000000)
			//{
			//	*probability=0;
			//}
			//else if (jizhong==3&&it->second>=3000000)
			//{
			//	*probability=0;
			//}
			return true;
		}
	}


	//CString str2;
	//str2.Format(_T("临界值：%d"),g_win_score_limit_[g_win_score_limit_count]);
	//AfxMessageBox(str2);

	//CString str2;
	//str2.Format(_T("玩家输赢分数%ld"),it->second);
	//CTraceService::TraceString(str2, TraceLevel_Exception);

	while (win_score_limit_count >= 0)
	{

		/*   if (it->second <= g_win_score_limit_[win_score_limit_count]) {*/
		if (lScoreTemp <= g_win_score_limit_[win_score_limit_count]) {
			//CString str2;
			//str2.Format(_T("玩家输赢分数%ld"),it->second);
			//CTraceService::TraceString(str2, TraceLevel_Exception);
			//str2.Format(_T("当前分数区间：%d"),win_score_limit_[win_score_limit_count]);
			//CTraceService::TraceString(str2, TraceLevel_Exception);
			*probability = *probability * (1 - g_win_score_probability_[win_score_limit_count]);
			return true;
		}
		--win_score_limit_count;
	}


	//CString str;
	//str.Format(_T("%f"),*probability);
	//AfxMessageBox(str);
	return false;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{

	m_pITableFrame = NULL;
	m_pGameServiceAttrib = NULL;
	m_pGameServiceOption = NULL;
	
	
	next_scene_kind_ = SCENE_KIND_1;
	special_scene_ = false;
	current_fish_lk_multiple_ = 50;
	android_chairid_ = INVALID_CHAIR;

	for (WORD i = 0; i < GAME_PLAYER; ++i)
	{
		fish_score_[i] = 0;
		exchange_fish_score_[i] = 0;
		experience[i] = 0;
		bullet_id_[i] = 0;
		bullet_ion_[i] = false;
		bullet_count[i] = 0;
		timercount[i] = 60;
		catch_fish_count[i] = 0;
	}
	


}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
	g_user_win_score_.RemoveAll();

	g_exchange_ratio_userscore_ = 0;
	FishTraceInfoVecor::iterator iter;
	for (iter = storage_fish_trace_vector_.begin(); iter != storage_fish_trace_vector_.end(); ++iter)
	{
		delete (*iter);
	}
	storage_fish_trace_vector_.clear();
	for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter)
	{
		delete (*iter);
	}
	active_fish_trace_vector_.clear();

	ServerBulletInfoVector::iterator it;
	for (it = storage_bullet_info_vector_.begin(); it != storage_bullet_info_vector_.end(); ++it)
	{
		delete (*it);
	}
	storage_bullet_info_vector_.clear();
	for (WORD i = 0; i < GAME_PLAYER; ++i)
	{
		for (it = server_bullet_info_vector_[i].begin(); it != server_bullet_info_vector_[i].end(); ++it)
		{
			delete (*it);
		}
		server_bullet_info_vector_[i].clear();
	}
	
	/////////////////////////////////////////////////////////////////////////////////////
	//if (g_revenue_score > 0) {
	//  CString str;
	//  str.Format(L"吃水%I64d", g_revenue_score);
	//  AfxMessageBox(str);
	//  g_revenue_score = 0;
	//}

}

//接口查询
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
	return NULL;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{

}

//配置桌子
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
	//TCHAR szMachineID[LEN_MACHINE_ID];
	//ZeroMemory(szMachineID, sizeof(szMachineID));
	//CWHService::GetMachineIDEx(szMachineID);
	//CTraceService::TraceString(szMachineID, TraceLevel_Normal);
	////780C6EF16B0B66EF284C0416B9D6D5D0
	//if (StrCmp(szMachineID, TEXT("780C6EF16B0B66EF284C0416B9D6D5D0")) != 0)
	//{
	//	AfxMessageBox(TEXT("非授权机器，请联系...."));
	//	return false;
	//}

	//错误判断
	if (m_pITableFrame == NULL)
	{
		CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"), TraceLevel_Exception);
		return false;
	}

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	//游戏配置
	m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();


	if (!LoadConfig())
	{
		AfxMessageBox(TEXT("配置资源解析失败，请检查"));
		return false;
	}

	current_fish_lk_multiple_ = g_fish_multiple_[FISH_KIND_21];
	table_android_count = rand() % (g_TableMaxAndroidCount - g_TableMinAndroidCount + 1) + g_TableMinAndroidCount;

	return true;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//if (one)
	//{
	//	//InitializeCriticalSection(&critial_section_);
	//	one = false;
	//}
	m_dwLastTickTime =GetTickCount();
	m_pITableFrame->KillGameTimer(kGameLOOP);
	m_pITableFrame->SetGameTimer(kGameLOOP,TIME_GAME_LOOP,kRepeatTimer,0);
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	std::vector<ipbaocun>::iterator iter;
	if (cbReason == GER_DISMISS)
	{
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			IServerUserItem* user_item = m_pITableFrame->GetTableUserItem(i);
			if (user_item == NULL) continue;
			CalcScore(user_item);
		}
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
		KillAllGameTimer();
		m_pITableFrame->KillGameTimer(kSwitchSceneTimer);
		m_pITableFrame->KillGameTimer(kClearTraceTimer);
		m_pITableFrame->KillGameTimer(min5jilu);
		m_pITableFrame->KillGameTimer(daodiantichu);
		m_pITableFrame->KillGameTimer(yifenzhongbaocun);
		ClearFishTrace(true);
		next_scene_kind_ = SCENE_KIND_1;
		special_scene_ = false;
		android_chairid_ = INVALID_CHAIR;
	}
	else if (wChairID < GAME_PLAYER && pIServerUserItem != NULL)
	{
		if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
		{
			m_pITableFrame->KillGameTimer(dongdongbo);
			panduan2 = true; /*suanshijian = 0;*/
		}
		CalcScore(pIServerUserItem);
		for (iter = iphao1.begin(); iter != iphao1.end(); ++iter)
		{
			if (iter->id23 == pIServerUserItem->GetGameID())
			{
				int argv = biandonguser[pIServerUserItem->GetGameID()];
				switch (argv)
				{
				case 100:
					--winuser2;
					break;
				case 0:
					--loseuser2;
					break;
				default:
					break;
				}
				 iphao1.erase(iter);
				 biandonguser.erase(pIServerUserItem->GetGameID());
				break;
			}
		};

	}
	return true;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:
	case GAME_STATUS_PLAY:
	{
		SendGameConfig(pIServerUserItem);
		CMD_S_GameStatus gamestatus;
		gamestatus.game_version = GAME_VERSION;
		memcpy(gamestatus.fish_score, fish_score_, sizeof(gamestatus.fish_score));
		memcpy(gamestatus.exchange_fish_score, exchange_fish_score_, sizeof(gamestatus.exchange_fish_score));
		gamestatus.g_bullet_multiple_[0] = g_bullet_multiple_[0];
		gamestatus.g_bullet_multiple_[1] = g_bullet_multiple_[1];
		//防止手机的断线重连后ID 对不上
		bullet_ion_[pIServerUserItem->GetChairID()] = false;
		bullet_id_[pIServerUserItem->GetChairID()] = 0;
		bullet_count[pIServerUserItem->GetChairID()] = 0;
		gamestatus.isyuzhen = special_scene_;
		m_pITableFrame->SendGameScene(pIServerUserItem, &gamestatus, sizeof(gamestatus));

		m_pITableFrame->SendGameMessage(pIServerUserItem, TEXT("键盘↑↓键加减炮，→←键上下分，空格键或鼠标左键发射子弹，F4关闭声音!"), SMT_CHAT);
		return true;
	}
	}
	return false;
}

//时间事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	if (wTimerID == kBuildSmallFishTraceTimer)
	{
		return OnTimerBuildSmallFishTrace(wBindParam);
	}
	else if (wTimerID == kBuildMediumFishTraceTimer)
	{
		return OnTimerBuildMediumFishTrace(wBindParam);
	}
	else if (wTimerID == TBuildFish41TraceTimer)
	{
		return OnTimerBuild41FishTrace(wBindParam);
	}
	else if (wTimerID == kBuildFish18TraceTimer)
	{
		return OnTimerBuildFish18Trace(wBindParam);
	}
	else if (wTimerID == kBuildFish19TraceTimer)
	{
		return OnTimerBuildFish19Trace(wBindParam);
	}
	else if (wTimerID == kBuildFish20TraceTimer)
	{
		return OnTimerBuildFish20Trace(wBindParam);
	}
	else if (wTimerID == kBuildFishLKTraceTimer)
	{
		return OnTimerBuildFishLKTrace(wBindParam);
	}
	else if (wTimerID == kBuildFishBombTraceTimer)
	{
		return OnTimerBuildFishBombTrace(wBindParam);
	}
	else if (wTimerID == kBuildFishSuperBombTraceTimer)
	{
		return OnTimerBuildFishSuperBombTrace(wBindParam);
	}
	else if (wTimerID == kBuildFishLockBombTraceTimer)
	{
		return OnTimerBuildFishLockBombTrace(wBindParam);
	}
	else if (wTimerID == kBuildFishSanTraceTimer)
	{
		return OnTimerBuildFishSanTrace(wBindParam);
	}
	else if (wTimerID == kBuildFishSiTraceTimer)
	{
		return OnTimerBuildFishSiTrace(wBindParam);
	}
	else if (wTimerID == kBuildFishKingTraceTimer)
	{
		return OnTimerBuildFishKingTrace(wBindParam);
	}
	else if (wTimerID == kClearTraceTimer)
	{
		return OnTimerClearTrace(wBindParam);
	}
	else if (wTimerID == daodiantichu)
	{
		return OnTimertichu(wBindParam);
	}
	else if (wTimerID == yifenzhongbaocun)
	{
		return OnTimerBaocun(wBindParam);
	}
	else if (wTimerID >= kBulletIonTimer && wTimerID <= kBulletIonTimer + 7)
	{
		return OnTimerBulletIonTimeout(WPARAM(wTimerID - kBulletIonTimer));
	}
	else if (wTimerID == kLockTimer)
	{
		return OnTimerLockTimeout(wBindParam);
	}
	else if (wTimerID == min5jilu)
	{
		return OnTimerjilu(wBindParam);
	}

	else if (wTimerID == kSwitchSceneTimer)
	{
  		return OnTimerSwitchScene(wBindParam);
	}
	else if (wTimerID == kSceneEndTimer)
	{
		return OnTimerSceneEnd(wBindParam);
	}
	else if (wTimerID == kLKScoreTimer)
	{
		return OnTimerLKScore(wBindParam);
	}
	else if (wTimerID == kFangShuiTimer)
	{
		return OnTimerFangShui(wBindParam);
	}
	else if (wTimerID == dongdongbo)
	{
		return OnTimerbodong(wBindParam);
	}
	else if (wTimerID == kGameLOOP)
	{
		DWORD dwNowTime =GetTickCount();
		float dt = (dwNowTime -m_dwLastTickTime)/1000.0f;
		m_dwLastTickTime =dwNowTime;
		OnUpdatge(dt);
	}
	else
	{
		ASSERT(FALSE);
	}
	return false;
}

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)

{
	switch (wSubCmdID)
	{
	case SUB_jiqirengailv:
	{
							 double *f = static_cast<double*>(pData);
							jiqirengailv = *f;
							return true;
							 
	}

	case SUB_jiqiren:
	{
					
						qijiren *h = static_cast<qijiren*>(pData);
						qijirentime[0] = h->time1[0];
						qijirentime[1] = h->time1[1];
						qijiren f = *h;
						//play_time = 30;
//						return	m_pITableFrame->sendcomputer(&f,0);
					
						 
	}
	case SUB_jiqirenjinchu:
	{					
							qijiren *hd = static_cast<qijiren*>(pData);

							SYSTEMTIME SystemTime;
							GetLocalTime(&SystemTime);
							DWORD dwTodayTickCount = SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond;


							likaishijian[0] = hd->time1[0];
							likaishijian[1] = hd->time1[1];
							DWORD mofashaonv[3] = { likaishijian[0], likaishijian[1], dwTodayTickCount };
				//			return	m_pITableFrame->sendcomputer(&mofashaonv, 1);


	}
	case SUB_tiaozheng:
	{
						  winlose *hd = static_cast<winlose*>(pData);
						  winlose w = *hd;
						  return  OnSubtiaozheng(w);

	}
	case SUB_winlose:
	{				
						Bvar1 = *(static_cast<bool*>(pData));
						return true;
	}
	case SUB_kucun:
	{
					  Bvar2 = *(static_cast<bool*>(pData));
					  return true;
	}	
	case SUB_C_qujian:
	{
						 qujian *arg2 = static_cast<qujian*>(pData);
						 return qujianjisuan(*arg2);
	}
	case SUB_weizhigailv:
	{
							double *a = (double*)pData;
							return weizhitiaozheng(a);
	}
	case SUB_C_heibaigailv:
	{
		double *a = (double*)pData;
		return heibaigailvtiaozheng(a);
	}
	case SUB_C_EXCHANGE_FISHSCORE:
	{
		//assert(wDataSize == sizeof(CMD_C_ExchangeFishScore));
		if (wDataSize != sizeof(CMD_C_ExchangeFishScore)) 
			return false;
		CMD_C_ExchangeFishScore* exchange_fishscore = static_cast<CMD_C_ExchangeFishScore*>(pData);
		if (pIServerUserItem->GetUserStatus() == US_LOOKON) return true;
		return OnSubExchangeFishScore(pIServerUserItem, exchange_fishscore->increase);
	}
	case SUB_C_USER_FIRE:
	{
		//assert(wDataSize == sizeof(CMD_C_UserFire));
		if (wDataSize != sizeof(CMD_C_UserFire))
			return false;
		CMD_C_UserFire* user_fire = static_cast<CMD_C_UserFire*>(pData);
		if (pIServerUserItem->GetUserStatus() == US_LOOKON) return true;
		
		bool bBadManCheat_ = false;
		DWORD chairid = pIServerUserItem->GetChairID();
		
		timercount[chairid] = 60;
		
		if (pIServerUserItem->IsAndroidUser() == false)
		{
			////
			std::map<DWORD, CHEATRECORD>::iterator itr1 = fire_time_test_map_.find(chairid);
			DWORD now_time = GetTickCount();
			if (itr1 == fire_time_test_map_.end())
			{
				CHEATRECORD rc1;
				rc1.iCheatCount = 0;
				rc1.iDeltaTime = abs((long)(now_time - user_fire->fire_time));
				fire_time_test_map_[chairid] = rc1;
			}
			CHEATRECORD &cr = fire_time_test_map_[chairid];
			CString str;
			//现在的延迟时间
		/*	int dRangTime = now_time - user_fire->fire_time;*/
													//初始的延迟时间
			//int rang = abs(abs(dRangTime) - abs(cr.iDeltaTime));
			////str.Format(L"%s-%d:接收子弹时间：%d-%d = %d 第一个时间差%d  %d",pIServerUserItem->GetNickName(),pIServerUserItem->GetTableID(),now_time,user_fire->fire_time,dRangTime,cr.iDeltaTime,rang);
			////NcaTextOut(str);

			//if (abs(rang) > g_timerInterval_)
			//{
			//	cr.iCheatCount++;
			//	if (cr.iCheatCount > ERROR_COUNTS)
			//	{
			//		//m_pITableFrame->SendRoomMessage(pIServerUserItem,TEXT("亲！系统检测到您使用第三方非法软件，请文明游戏!"),SMT_EJECT);
			//		//m_pITableFrame->SendGameMessage(pIServerUserItem,TEXT("亲！系统检测到您使用第三方非法软件，请文明游戏!"),SMT_CLOSE_GAME);

			//		//itr1 = fire_time_test_map_.find(pIServerUserItem->GetChairID());
			//		//fire_time_test_map_.erase(itr1);
			//		//return true;

			//		//有人开挂 开始杀猪
			//		CString str;
			//		str.Format(L"%s开挂超%炮,开始杀猪", pIServerUserItem->GetNickName(), cr.iCheatCount);
			//		cr.iCheatCount = 25;
			//		CTraceService::TraceString(str, TraceLevel_Exception);
			//		bBadManCheat_ = true;
			//	}

			//}
			//else
			//{
			/*	cr.iCheatCount = 0;*/
			/*}*/
			////
		}

		return OnSubUserFire(pIServerUserItem, user_fire->bullet_id, user_fire->bullet_kind, user_fire->angle, user_fire->bullet_mulriple, user_fire->lock_fishid, user_fire->fire_speed,bBadManCheat_);
	}
	case SUB_C_CATCH_FISH:
	{
		//assert(wDataSize == sizeof(CMD_C_CatchFish));
		if (wDataSize != sizeof(CMD_C_CatchFish))
			return false;
		CMD_C_CatchFish* hit_fish = static_cast<CMD_C_CatchFish*>(pData);
		if (pIServerUserItem->GetUserStatus() == US_LOOKON) return true;
		IServerUserItem* user_item = m_pITableFrame->GetTableUserItem(hit_fish->chair_id);
		if (user_item == NULL) return true;
		/*if (hit_fish->bullet_mulriple < 0)
			AfxMessageBox(L"GG");*/
		
		return OnSubCatchFish(user_item, hit_fish->fish_id, (BulletKind)hit_fish->bullet_kind, hit_fish->bullet_id, hit_fish->bullet_mulriple);
	}
	case SUB_C_CATCH_SWEEP_FISH:
	{
		//assert(wDataSize == sizeof(CMD_C_CatchSweepFish));
		if (wDataSize != sizeof(CMD_C_CatchSweepFish))
			return false;
		CMD_C_CatchSweepFish* catch_sweep = static_cast<CMD_C_CatchSweepFish*>(pData);
		if (pIServerUserItem->GetUserStatus() == US_LOOKON) return true;
		IServerUserItem* user_item = m_pITableFrame->GetTableUserItem(catch_sweep->chair_id);
		if (user_item == NULL) return true;
		return OnSubCatchSweepFish(user_item, catch_sweep->fish_id, catch_sweep->catch_fish_id, catch_sweep->catch_fish_count);
	}
	case SUB_C_HIT_FISH_I:
	{
		//assert(wDataSize == sizeof(CMD_C_HitFishLK));
		if (wDataSize != sizeof(CMD_C_HitFishLK))
			return false;
		CMD_C_HitFishLK* hit_fish = static_cast<CMD_C_HitFishLK*>(pData);
		if (pIServerUserItem->GetUserStatus() == US_LOOKON) return true;
		return true /*OnSubHitFishLK(pIServerUserItem, hit_fish->fish_id);*/;
	}
	case SUB_C_ipfasong:
	{
						
						   return onsubipfasong(pIServerUserItem);
	}
	case SUB_C_ip1:
	{
					  tianjiashanchu *x = static_cast<tianjiashanchu *>(pData);
					  return onSubtianjia(pIServerUserItem, *x);
	}
	case SUB_C_ip2:
	{
					  DWORD *x = static_cast<DWORD*>(pData);
					  return onsubshanchu(pIServerUserItem, *x);
	}
	case SUB_C_USER_FILTER:
	{
		//assert(wDataSize == sizeof(CMD_C_UserFilter));
		if (wDataSize != sizeof(CMD_C_UserFilter)) 
			return false;
		CMD_C_UserFilter* user_filter = static_cast<CMD_C_UserFilter*>(pData);
		if (pIServerUserItem->GetUserStatus() == US_LOOKON) return true;
		return OnSubUserFilter(pIServerUserItem, user_filter->game_id, user_filter->operate_code);
	}
	case SUB_C_ANDROID_STAND_UP:
	{
		if (!pIServerUserItem->IsAndroidUser()) return true;
		if (pIServerUserItem->GetUserStatus() == US_LOOKON) return true;
		// OutputDebugString(TEXT("机器人到点起立"));
		m_pITableFrame->PerformStandUpAction(pIServerUserItem);
		return true;
	}
	case SUB_C_FISH20_CONFIG:
	{
		//assert(wDataSize == sizeof(CMD_C_Fish20Config));
		if (wDataSize != sizeof(CMD_C_Fish20Config))
			return false;
		CMD_C_Fish20Config* fish20_config = static_cast<CMD_C_Fish20Config*>(pData);
		if (pIServerUserItem->GetUserStatus() == US_LOOKON) return true;
		return OnSubFish20Config(pIServerUserItem, fish20_config->game_id, fish20_config->catch_count, fish20_config->catch_probability);
	}
	case SUB_gengxin:
	{
			CMD_gengxin a = {};
			a.daxiaoyu[0] = g_stock_score_[0];
			a.daxiaoyu[1] = g_stock_score_[1];
	return	m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_gengxin1,&a, sizeof(CMD_gengxin));


	}
	//
	case SUB_C_FISH21_CONFIG:
	{
		//assert(wDataSize == sizeof(CMD_C_Fish20Config));
		if (wDataSize != sizeof(CMD_C_Fish20Config)) 
			return false;
		CMD_C_Fish20Config* fish21_config = static_cast<CMD_C_Fish20Config*>(pData);
		if (pIServerUserItem->GetUserStatus() == US_LOOKON) return true;
		return OnSubFish21Config(pIServerUserItem, fish21_config->game_id, fish21_config->catch_count, fish21_config->catch_probability);
	}
		//金鲨概率设置
	case SUB_C_FISHJINSHA_CONFIG:
	{
		//assert(wDataSize == sizeof(CMD_C_Fish20Config));
		if (wDataSize != sizeof(CMD_C_Fish20Config))
			return false;
		CMD_C_Fish20Config* fishjinsha_config = static_cast<CMD_C_Fish20Config*>(pData);
		if (pIServerUserItem->GetUserStatus() == US_LOOKON) return true;
		return OnSubFishJinShaConfig(pIServerUserItem, fishjinsha_config->game_id, fishjinsha_config->catch_count, fishjinsha_config->catch_probability);
	}
		//报警记录删除
	case SUB_shanchujilu:
	{
		int *i = (int *)pData;

		return OnSubshanchujilu(pIServerUserItem, *i);
	}
		//报警的设置
	case SUB_shezhi:
	{
		 SCORE * i = static_cast<SCORE*>(pData);
		 g_MAXbaojing = *i;
		 return true;
	}
		//总库存
	case SUB_zongkucun:
	{
		return OnSubzongkucun(pIServerUserItem);
	}
		//报警记录
	case SUB_baojingjilu:
	{
		return	OnSubbaojingjilu(pIServerUserItem);
	}
		//总库存修改
	case SUB_zongkucunxiugai:
	{

		CMD_S_CONFIG * app = static_cast<CMD_S_CONFIG *>(pData);
		return	OnSubZKCxiugai(pIServerUserItem,*app );

	}
		//限制子弹数
	case SUB_C_BulletCount:
	{
		int *app = static_cast<int*>(pData);
		BulletCount = *app;
		return true;
	}
		//更改浪潮事件
	case SUB_C_LangChaoTime:
	{
		int *app = static_cast<int*>(pData);
		kSwitchSceneElasped = *app;
		m_pITableFrame->SetGameTimer(kSwitchSceneTimer, kSwitchSceneElasped * 1000L, kRepeatTimer, 0);
		return true;
	}
		//超级炮概率
	case SUB_C_SuPerGaiLv:
	{
		double *app = static_cast<double*>(pData);
		SuperPaoGailv = *app;
		return true;
	}
		//超级炮持续时间
	case SUB_C_SuperTimer:
	{
		DWORD *app = static_cast<DWORD*>(pData);
		kBulletIonTime = *app;
		return true;
	}
		//超级炮需要抓鱼的数量
	case SUB_C_FISH_Count:
	{
		DWORD *app = static_cast<DWORD*>(pData);
		yushu = *app;
		return true;
	}
		//黑白列表
	case SUB_C_HEIBAI_LIST:
	{
		//assert(wDataSize == sizeof(CMD_C_ChackList));
		if (wDataSize != sizeof(CMD_C_ChackList))
			return false;
		CMD_C_ChackList* chacklist = static_cast<CMD_C_ChackList*>(pData);
		return  OnSubScanFilter(pIServerUserItem);
	}
		//鱼列表
	case SUB_C_FISHTESHU_LIST:
	{
		//assert(wDataSize == sizeof(CMD_C_ChackList));
		if (wDataSize != sizeof(CMD_C_ChackList)) 
			return false;
		CMD_C_ChackList* chacklist = static_cast<CMD_C_ChackList*>(pData);
		return  OnSubScanFilter2(pIServerUserItem);
	}
		//踢人
	case sub_tichu:
	{
					  DWORD *f = static_cast<DWORD*>(pData);
					  return Onsubtichu(pIServerUserItem,*f);
	}
		//炮概率
	case SUB_pao:
	{
					if (wDataSize != sizeof(pao10086))
						return false;
					pao10086 *pao = static_cast<pao10086*>(pData);
					return Onsubtiaozhengpao(pIServerUserItem, *pao);
	}
		//ip显示
	case SUB_C_ip:
	{

					 iphaoshuzu  shuliang = {};

					 int j = 0;
 
					 for (int i= 0;i<iphao1.size();i++)
					 {
						 shuliang.iphao[j] = iphao1[i].ip23;
						 shuliang.gameid[j++] = iphao1[i].id23;
					 }
					
				 m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_iphao, &shuliang, sizeof(shuliang));
				
	}
	case SUB_C_GetSControl:
	{
		//读取管理员号
		if (!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
			return false;
		OnSubClientGetControl(pIServerUserItem);
		return true;
	}
	break;
	case SUB_C_ControlCfg:
	{
		//读取管理员号
		if (!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
			return false;
		CMD_S_CONTROLCFG* pControlConfig = static_cast<CMD_S_CONTROLCFG*>(pData);
		OnSubClientSetControlCfg(pControlConfig->total_return_rate_,						//返还百分比
			pControlConfig->revenue_score,								//游戏抽水 
			pControlConfig->zhengtigailv,								//整体概率
			pControlConfig->stock_score0,						//库存配置
			pControlConfig->stock_score1,						//库存配置
			pControlConfig->hard,					// 困难
			pControlConfig->easy);					//简单
		return true;
	}
	break;
	case SUB_C_ADMIN_CONFIG:
	{
		//读取管理员号
		if (!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
			return false;
		if (wDataSize != 0)
		{
			//assert(wDataSize == sizeof(CMD_S_ADMIN_CONFIG));
			if (wDataSize != sizeof(CMD_S_ADMIN_CONFIG)) 
				return false;
			CMD_S_ADMIN_CONFIG* pAdminConfig = static_cast<CMD_S_ADMIN_CONFIG*>(pData);

			g_total_return_rate_ = pAdminConfig->total_return_rate_;
			g_revenue_score = pAdminConfig->revenue_score;
			g_stock_score_[0] = pAdminConfig->stock_score[0];
			g_stock_score_[1] = pAdminConfig->stock_score[1];
			
			
			g_total_probability_ = pAdminConfig->zhengtigailv;

			S_easy = pAdminConfig->easy;
			S_hard = pAdminConfig->hard;
			/*S_bodongshijian = pAdminConfig->bodongshijian;*/
			/*S_bodongshijian *= 60;*/

			/*s_bodongmax = pAdminConfig->bodongmax;*/
			/*s_bodongmax *= 60;*/
			/*if (T_bodong&&S_bodongshijian > 0)
			{
				m_pITableFrame->SetGameTimer(dongdongbo, kFangShuiElasped * 1000, kRepeatTimer, 0);
				T_bodong = false;
			}
			else
			{
				m_pITableFrame->KillGameTimer(dongdongbo);
				T_bodong = true;
			}*/
			g_exchange_fishscore_count_ = pAdminConfig->exchange_fishscore_count_;
			g_exchange_ratio_userscore_ = pAdminConfig->exchange_ratio_userscore_;
			g_exchange_ratio_fishscore_ = pAdminConfig->exchange_ratio_fishscore_;
			g_bullet_multiple_[0] = pAdminConfig->bullet_multiple[0];
			g_bullet_multiple_[1] = pAdminConfig->bullet_multiple[1];

			for (int i = 0; i < 12; i++)
			{
				g_stock_crucial_score_[i] = pAdminConfig->stock_crucial_score_[i];
				g_stock_crucial_score_small[i] = pAdminConfig->stock_crucial_score_small[i];
				g_stock_increase_probability_[i] = pAdminConfig->stock_increase_probability_[i];
				g_stock_increase_probability_small[i] = pAdminConfig->stock_increase_probability_small[i];
			}
			for (int i = 0; i < 11; i++)
			{
				for (int j = 0; j < 12 - i - 1; ++j)
				if (g_stock_crucial_score_[j]>g_stock_crucial_score_[j + 1])
				{
					SCORE tmp = g_stock_crucial_score_[j];
					g_stock_crucial_score_[j] = g_stock_crucial_score_[j + 1];
					g_stock_crucial_score_[j + 1] = tmp;
					SCORE tmp1 = g_stock_increase_probability_[j];
					g_stock_increase_probability_[j] = g_stock_increase_probability_[j + 1];
					g_stock_increase_probability_[j + 1] = tmp1;
				}
			}
			for (int i = 0; i < 11; i++)
			{
				for (int j = 0; j < 12 - i - 1; ++j)
				if (g_stock_crucial_score_small[j]>g_stock_crucial_score_small[j + 1])
				{
					SCORE tmp = g_stock_crucial_score_small[j];
					g_stock_crucial_score_small[j] = g_stock_crucial_score_small[j + 1];
					g_stock_crucial_score_small[j + 1] = tmp;
					SCORE tmp1 = g_stock_increase_probability_small[j];
					g_stock_increase_probability_small[j] = g_stock_increase_probability_small[j + 1];
					g_stock_increase_probability_small[j + 1] = tmp1;
				}
			}

			for (int i = 0; i < FISH_KIND_COUNT; i++)
			{
				//鱼的倍数
				g_fish_multiple_[i] = pAdminConfig->nFishMinMultiple[i];
				if (i == FISH_KIND_18)			g_fish18_max_multiple_ = pAdminConfig->nFishMaxMultiple[i];
				else if (i == FISH_KIND_19)		g_fish19_max_multiple_ = pAdminConfig->nFishMaxMultiple[i];
				else if (i == FISH_KIND_21)		g_fishLK_max_multiple_ = pAdminConfig->nFishMaxMultiple[i];

				//鱼的速度
				g_fish_speed_[i] = pAdminConfig->nFishSpeed[i];
				//捕获概率(0-1)
				g_fish_capture_probability_[i] = pAdminConfig->fFishGailv[i];
			}
			//输赢档位限制
			for (int i = 0; i < 20; i++)
			{
				//鱼的倍数
				g_win_score_limit_[i] = pAdminConfig->win_score_limit_[i];
				g_win_score_probability_[i] = pAdminConfig->win_decrease_probability_[i];
			}
			for (int i = 0; i < 19; i++)
			{
				for (int j = 0; j < 20 - i - 1; ++j)
				if (g_win_score_limit_[j]>g_win_score_limit_[j+1])
				{
					SCORE tmp = g_win_score_limit_[j];
					g_win_score_limit_[j] = g_win_score_limit_[j+1];
					g_win_score_limit_[j + 1] = tmp;
					SCORE tmp1 = g_win_score_probability_[j];
					g_win_score_probability_[j] = g_win_score_probability_[j + 1];
					g_win_score_probability_[j + 1] = tmp1;
				}
			}
			wDataSize=0;
		}
		if (wDataSize == 0)
		{
			/*if (panduan2)
			{
				m_pITableFrame->SetGameTimer(dongdongbo, kFangShuiElasped * 1000, kRepeatTimer, 0);
				panduan2 = false;
			}*/
			panduan1 = true;
				//return m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_ADMIN_FANGSHUI, &AdminFangShui, sizeof(AdminFangShui)+wenjiandx);
			
			CMD_S_ADMIN_CONFIG	AdminConfig;

			AdminConfig.total_return_rate_ = g_total_return_rate_;
			AdminConfig.revenue_score = g_revenue_score;
			
			AdminConfig.stock_score[0] = g_stock_score_[0];
			AdminConfig.stock_score[1] = g_stock_score_[1];

			AdminConfig.zhengtigailv = g_total_probability_;
			AdminConfig.qijiren[0] = qijirentime[0];
			AdminConfig.qijiren[1] = qijirentime[1];
			AdminConfig.jiqirenleave[0] = likaishijian[0];
			AdminConfig.jiqirenleave[1] = likaishijian[1];
			AdminConfig.qijirengailv = jiqirengailv;
			AdminConfig.exchange_fishscore_count_ = g_exchange_fishscore_count_;
			AdminConfig.exchange_ratio_userscore_ = g_exchange_ratio_userscore_;
			AdminConfig.exchange_ratio_fishscore_ = g_exchange_ratio_fishscore_;
			AdminConfig.bullet_multiple[0] = g_bullet_multiple_[0];
			AdminConfig.bullet_multiple[1] = g_bullet_multiple_[1];
			int ibodong = 0;
			AdminConfig.bodongshijian = ibodong;
			int ibodong1 = 0;
			AdminConfig.bodongmax = ibodong1;
			AdminConfig.easy = S_easy;
			for (int i = 0; i < 10; i++)
			{
				AdminConfig.paogailv[i] = paogailv[i];
			}
			AdminConfig.hard = S_hard;
			for (int i = 0; i < 8; i++)
			{
				AdminConfig.zuoweigailv[i] = chairgailvp[i];
			}

			for (int i = 0; i < 12; i++)
			{
				AdminConfig.stock_crucial_score_[i] = g_stock_crucial_score_[i];
				AdminConfig.stock_crucial_score_small[i] = g_stock_crucial_score_small[i];
				AdminConfig.stock_increase_probability_[i] = g_stock_increase_probability_[i];
				AdminConfig.stock_increase_probability_small[i] = g_stock_increase_probability_small[i];
			}

			for (int i = 0; i < FISH_KIND_COUNT; i++)
			{
				//鱼的倍数
				AdminConfig.nFishMinMultiple[i] = g_fish_multiple_[i];
				if (i == FISH_KIND_18)			AdminConfig.nFishMaxMultiple[i] = g_fish18_max_multiple_;
				else if (i == FISH_KIND_19)		AdminConfig.nFishMaxMultiple[i] = g_fish19_max_multiple_;
				else if (i == FISH_KIND_21)		AdminConfig.nFishMaxMultiple[i] = g_fishLK_max_multiple_;
				else							AdminConfig.nFishMaxMultiple[i] = g_fish_multiple_[i];

				//鱼的速度
				AdminConfig.nFishSpeed[i] = g_fish_speed_[i];
				//捕获概率(0-1)
				AdminConfig.fFishGailv[i] = g_fish_capture_probability_[i];
			}
			//输赢档位限制
			for (int i = 0; i < 20; i++)
			{
				//鱼的倍数
				AdminConfig.win_score_limit_[i] = g_win_score_limit_[i];
				AdminConfig.win_decrease_probability_[i] = g_win_score_probability_[i];
			}
			return m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_ADMIN_CONFIG, &AdminConfig, sizeof(AdminConfig));
		}
		return false;
	}
	case SUB_C_ADMIN_FANGSHUI:
	{
		if (!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))	return false;
		if (wDataSize != 0)
		{
			//assert(wDataSize == sizeof(CMD_S_ADMIN_FANGSHUI));
			if (wDataSize != sizeof(CMD_S_ADMIN_FANGSHUI)) 
				return false;
			CMD_S_ADMIN_FANGSHUI* pAdminFangShui = static_cast<CMD_S_ADMIN_FANGSHUI*>(pData);

			//放水额度
			g_stock_fangshui_edu = pAdminFangShui->stock_fangshui_edu;

			//计算
			//每天额度
			g_stock_meitian_edu = pAdminFangShui->meitian_edu;
			
			SCORE aaa=g_stock_meitian_edu*(rand() % 30 + 1) / 100;
			fangshuikucun[1] = aaa*(rand() % 30 + 1) / 100;
			fangshuikucun[0] = aaa - fangshuikucun[1];
			jilukucun[1] = fangshuikucun[1];
			jilukucun[0] = aaa - fangshuikucun[1];
			//鱼的分
			//放水小鱼库存
			//fangshuikucun[0] = pAdminFangShui->fangshuixiaoyukucun;
			
			////放水大鱼库存
			//fangshuikucun[1] = pAdminFangShui->fangshuidayukucun;
		TCHAR pBUF[1024] = { 0 };
		
			panduan1 = true;
		
			//放水概率
			g_stock_fangshui_probability = pAdminFangShui->stock_fangshui_probability;
			//持续时间
			g_fangshui_time = pAdminFangShui->fangshui_time; 
			//放水状态
			g_bFangShuiStart = pAdminFangShui->bFangShuiStart;
			
		
			if (g_bFangShuiStart == false)
			{//放水余额
				g_lastfangshui_score = g_fangshui_cur_score;

				if (g_fangshui_end_time == 0)	g_fangshui_end_time = CTime::GetCurrentTime().GetTime();

				g_lastfangshui_count_time = g_fangshui_end_time - g_fangshui_begin_time;

				g_fangshui_begin_time = g_fangshui_end_time = g_nextfangshui_time = 0;
				g_fangshui_cur_time = 0;
				m_pITableFrame->KillGameTimer(kFangShuiTimer);
			}
			else
			{
				time_t tta = time(NULL);
				tm* tqq = localtime(&tta);
					g_stock_fangshui_yue = g_stock_meitian_edu;
					srand((unsigned)time(NULL));
				

				g_hhhhhhh = 24 - tqq->tm_hour;
				g_fangshui_begin_time = CTime::GetCurrentTime().GetTime();
				g_fangshui_end_time = 0;

				
			
			g_nextfangshui_time = CTime::GetCurrentTime().GetTime() + 3600;
				g_fangshui_cur_score = 0;
				g_fangshui_cur_time = 0;

				m_pITableFrame->SetGameTimer(kFangShuiTimer, kFangShuiElasped * 1000, kRepeatTimer, 0);
			}
			wDataSize = 0;
			/*if (g_bFangShuiStart)
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_zouni, &jilu, sizeof(jilu));*/
		}
		if (wDataSize == 0)
		{
			CMD_S_ADMIN_FANGSHUI	AdminFangShui;

			//放水额度
			AdminFangShui.stock_fangshui_edu = g_stock_fangshui_edu;
			//每天额度
			AdminFangShui.meitian_edu = g_stock_meitian_edu;
			//放水概率
			AdminFangShui.stock_fangshui_probability = g_stock_fangshui_probability;
			//持续时间
			AdminFangShui.fangshui_time = g_fangshui_time;
			//放水状态
			AdminFangShui.bFangShuiStart = g_bFangShuiStart;
			//已放水金额
			AdminFangShui.fangshui_score = g_fangshui_cur_score;
			//已持续放水多少时间
			DWORD dwCurrentTime = g_fangshui_end_time == 0 ? CTime::GetCurrentTime().GetTime() : g_fangshui_end_time;
			AdminFangShui.fangshui_count_time = g_bFangShuiStart == false ? g_fangshui_begin_time : dwCurrentTime - g_fangshui_begin_time;
			//上次已放水金额
			AdminFangShui.lastfangshui_score = g_lastfangshui_score;
			//已持续放水多少时间
			AdminFangShui.lastfangshui_count_time = g_lastfangshui_count_time;
			//下次放水开始时间
			AdminFangShui.nextfangshui_time = g_nextfangshui_time;
			//当前放水开始事件
			AdminFangShui.fangshui_Begin = g_fangshui_begin_time;
			//放水小鱼库存
			AdminFangShui.fangshuixiaoyukucun = fangshuikucun[0];
			//放水大鱼库存
			AdminFangShui.fangshuidayukucun = fangshuikucun[1];
		    
			AdminFangShui.fangshuixiaoyukucun1 = jilukucun[0];
			AdminFangShui.fangshuidayukucun1 = jilukucun[1];
			AdminFangShui.youxiid1[0] = 0;
			AdminFangShui.youxiid1[1] = 0;
			/*AdminFangShui.yizihao1[0] = yizihao[0];
			AdminFangShui.yizihao1[1] = yizihao[1];*/
			AdminFangShui.zaixianwanjia = zaixianwanjia;
			if (panduan1&&!g_bFangShuiStart)
			{
				jilu.clear();
				TCHAR pBUF[1024] = { 0 };

			FILE *wenjian = _wfopen(L"GameModule//jilu1.txt", L"a+,ccs=UTF-16LE");
			if (wenjian)
			{
				while (fgetws(pBUF, 1024, wenjian))
				{
					jilu.push_back(pBUF);
				}
				wenjiandx = ftell(wenjian);
				fclose(wenjian);
			}




			TCHAR  szString[128] = { 0 };
			int i = 0;
			for (; i<jilu.size(); i++)
			{
				wsprintf(szString, L"%s", jilu[i]);
				m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_chuansong, szString, 128);
			}
			if (jilu.size() > 120)
			{
				FILE *wenjian2 = _wfopen(L"GameModule//jilu1.txt", L"w,ccs=UNICODE");

				fclose(wenjian2);

			}


			panduan1 = false;

			jilu.clear();

			}

			return m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_ADMIN_FANGSHUI, &AdminFangShui, sizeof(AdminFangShui));
		}
	}
	case SUB_huoqu:
	{


					  winlose win = {};
					  win.lose233 = losepercentage;
					  win.win233 = winpercentage;
					  win.var233[0] = Bvar;
					  win.var233[1] = Bvar1;
					  win.var233[2] = Bvar2;
					  win.var233[3] = panduan3;
					  win.qujianmax = qujianfanwei[45];
					  win.qujianmin = qujianfanwei[0];
					return   m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_Smorenbaifenbi, &win, sizeof(winlose));
	}
	
	}
	CString cout;
	cout.Format(TEXT("%d"), wSubCmdID);
	CTraceService::TraceString(cout, TraceLevel_Normal);
	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD chair_id, IServerUserItem* server_user_item, bool lookon_user) 
{

	if (!lookon_user)
	{

		exchange_fish_score_[chair_id] = 0;
		fish_score_[chair_id] = 0;
		bullet_id_[chair_id] = 0;
		bullet_ion_[chair_id] = false;
		bullet_count[chair_id] = 0;
		
		if (android_chairid_ == INVALID_CHAIR && !server_user_item->IsAndroidUser())
		{
			android_chairid_ = chair_id;
		}
		if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
		{
			m_pITableFrame->StartGame();
			m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);
			StartAllGameTimer();
			m_pITableFrame->SetGameTimer(kSwitchSceneTimer, kSwitchSceneElasped * 1000L, kRepeatTimer, 0);
			m_pITableFrame->SetGameTimer(min5jilu, BJjilu * 1000, kRepeatTimer, 0);
			m_pITableFrame->SetGameTimer(kClearTraceTimer, kClearTraceElasped * 1000, kRepeatTimer, 0);
			m_pITableFrame->SetGameTimer(daodiantichu, 20000, kRepeatTimer, 0);
			m_pITableFrame->SetGameTimer(yifenzhongbaocun, 30*1000, kRepeatTimer, 0);
		}
		if (!server_user_item->IsAndroidUser())
		{
			zaixianwanjia++;
			winuser1 = zaixianwanjia*winpercentage;
			loseuser1 = zaixianwanjia*losepercentage;
			ipbaocun x = {};
			x.ip23 = server_user_item->GetClientAddr();
			x.id23 = server_user_item->GetGameID();
			x.USERID = server_user_item->GetUserID();
			iphao1.push_back(x);

			if (zaixianwanjia % 2)
			{
				if (winuser2 < winuser1)
				{
					biandonguser[server_user_item->GetGameID()] = 1.3;
					++winuser2;
				}
				else
					if (loseuser2 < loseuser1)
					{
					biandonguser[server_user_item->GetGameID()] = 0.8;
					++loseuser2;
					}
					else
						biandonguser[server_user_item->GetGameID()] = 1;
			}
			else
				if (loseuser2 < loseuser1)
				{
				biandonguser[server_user_item->GetGameID()] = 0.8;
				++loseuser2;
				}
				else
					if (winuser2 < winuser1)
					{
				biandonguser[server_user_item->GetGameID()] = 1.3;
				++winuser2;
					}
					else
						biandonguser[server_user_item->GetGameID()] = 1;



		}
			///int nTableAndroidCount=0;
			
		
		
	
	
     	////如果有玩家取最小数
		//for (WORD i = 0; i < GAME_PLAYER; ++i)
		//{
		//	IServerUserItem* user_item = m_pITableFrame->GetTableUserItem(i);
		//	if (user_item)
		//	{
		//		if (!user_item->IsAndroidUser())
		//		{
		//			table_android_count=TableMinAndroidCount;
		//			break;
		//		}
		//	}
		//}

		////控制每个桌子机器人数目
		//for (WORD i = 0; i < GAME_PLAYER; ++i)
		//{
		//	IServerUserItem* user_item = m_pITableFrame->GetTableUserItem(i);
		//	if (user_item)
		//	{
		//		if (user_item->IsAndroidUser())
		//		{
		//			nTableAndroidCount++;
		//			if (nTableAndroidCount>table_android_count)
		//			{
		//				/*m_pITableFrame->PerformStandUpAction(user_item);*/
		//			}
		//		}
		//	}
		//}
	}
	return true;
}
//起立
bool CTableFrameSink::OnActionUserStandUp(WORD chair_id, IServerUserItem* server_user_item, bool lookon_user) 
{
	if (lookon_user) return true;

	SCORE lScore = 0;
	if (g_user_win_score_.Lookup(server_user_item->GetUserID(), lScore))
	{
		g_user_win_score_.RemoveKey(server_user_item->GetUserID());
	}
	if (!server_user_item->IsAndroidUser())
	{
		zaixianwanjia--;
	}
		
	
	//用户退出 记录则消失
	exchange_fish_score_[chair_id] = 0;
	fish_score_[chair_id] = 0;
	bullet_id_[chair_id] = 0;
	bullet_count[chair_id] = 0;
	timercount[chair_id] = 60;
	bullet_ion_[chair_id] = false;
	FreeAllBulletInfo(chair_id);
	WORD user_count = 0;
	WORD player_count = 0;
	WORD android_chair_id[GAME_PLAYER];
	for (WORD i = 0; i < GAME_PLAYER; ++i) 
	{
		if (i == chair_id) continue;
		IServerUserItem* user_item = m_pITableFrame->GetTableUserItem(i);
		if (user_item) 
		{
			if (!user_item->IsAndroidUser())
			{
				android_chair_id[player_count++] = i;
				++user_count;
			}

		}
	}

	if (player_count == 0)
	{
		android_chairid_ = INVALID_CHAIR;
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			bullet_count[chair_id] = 0;
			IServerUserItem* user_item = m_pITableFrame->GetTableUserItem(i);
			if (user_item)
			{
				if (user_item->IsAndroidUser())
				{
					FreeAllBulletInfo(i);
				}
			}
		}
	}
	else
	{
		if (!server_user_item->IsAndroidUser() && chair_id == android_chairid_)
		{
			android_chairid_ = android_chair_id[0];
		}
	}

	if (user_count == 0) 
	{
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
		KillAllGameTimer();
		m_pITableFrame->KillGameTimer(kSwitchSceneTimer);
		m_pITableFrame->KillGameTimer(kClearTraceTimer);
		m_pITableFrame->KillGameTimer(min5jilu);
		m_pITableFrame->KillGameTimer(daodiantichu);
		m_pITableFrame->KillGameTimer(yifenzhongbaocun);
		ClearFishTrace(true);
		next_scene_kind_ = SCENE_KIND_1;
		special_scene_ = false;
	}
	else
	{
		m_pITableFrame->ConcludeGame(GAME_STATUS_PLAY);
	}

	//遍历所有的玩家
	std::map<DWORD, CHEATRECORD>::iterator itr1 = fire_time_test_map_.begin();
	for (; itr1 != fire_time_test_map_.end(); ++itr1)
	{
		if (itr1->first == server_user_item->GetChairID())
		{
			itr1->second.iCheatCount = 0;
			fire_time_test_map_.erase(itr1);
			break;
		}
	}
	return true;
}

FishTraceInfo* CTableFrameSink::ActiveFishTrace() {
  FishTraceInfo* fish_trace_info = NULL;
  if (storage_fish_trace_vector_.size() > 0) {
    fish_trace_info = storage_fish_trace_vector_.back();
    storage_fish_trace_vector_.pop_back();
    active_fish_trace_vector_.push_back(fish_trace_info);
  }

  if (fish_trace_info == NULL) {
    fish_trace_info = new FishTraceInfo;
    active_fish_trace_vector_.push_back(fish_trace_info);
  }

  return fish_trace_info;
}

bool CTableFrameSink::FreeFishTrace(FishTraceInfo* fish_trace_info) {
  FishTraceInfoVecor::iterator iter;
  for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter) {
    if (fish_trace_info == *iter) {
      active_fish_trace_vector_.erase(iter);
      storage_fish_trace_vector_.push_back(fish_trace_info);
      return true;
    }
  }

  //assert(!"FreeFishTrace Failed");
  return false;
}

void CTableFrameSink::FreeAllFishTrace() {
  std::copy(active_fish_trace_vector_.begin(), active_fish_trace_vector_.end(), std::back_inserter(storage_fish_trace_vector_));
  active_fish_trace_vector_.clear();
}

FishTraceInfo* CTableFrameSink::GetFishTraceInfo(int fish_id) {
  FishTraceInfoVecor::iterator iter;
  FishTraceInfo* fish_trace_info = NULL;
  for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter) {
    fish_trace_info = *iter;
    if (fish_trace_info->fish_id == fish_id) return fish_trace_info;
  }
  //assert(!"GetFishTraceInfo:not found fish");
  return NULL;
}

void CTableFrameSink::SaveSweepFish(FishKind fish_kind, int fish_id, BulletKind bullet_kind, float bullet_mulriple) {
  SweepFishInfo sweep_fish;
  sweep_fish.fish_kind = fish_kind;
  sweep_fish.fish_id = fish_id;
  sweep_fish.bullet_kind = bullet_kind;
  sweep_fish.bullet_mulriple = bullet_mulriple;
  sweep_fish_info_vector_.push_back(sweep_fish);
}

bool CTableFrameSink::FreeSweepFish(int fish_id) {
  std::vector<SweepFishInfo>::iterator iter;
  for (iter = sweep_fish_info_vector_.begin(); iter != sweep_fish_info_vector_.end(); ++iter) {
    if ((*iter).fish_id == fish_id) {
      sweep_fish_info_vector_.erase(iter);
      return true;
    }
  }

  //assert(!"FreeSweepFish Failed");
  return false;
}

SweepFishInfo* CTableFrameSink::GetSweepFish(int fish_id) {
  std::vector<SweepFishInfo>::iterator iter;
  for (iter = sweep_fish_info_vector_.begin(); iter != sweep_fish_info_vector_.end(); ++iter) {
    if ((*iter).fish_id == fish_id) {
      return &(*iter);
    }
  }
  return NULL;
}

ServerBulletInfo* CTableFrameSink::ActiveBulletInfo(WORD chairid) {
  ServerBulletInfo* bullet_info = NULL;
  if (storage_bullet_info_vector_.size() > 0) {
    bullet_info = storage_bullet_info_vector_.back();
    storage_bullet_info_vector_.pop_back();
    server_bullet_info_vector_[chairid].push_back(bullet_info);
  }

  if (bullet_info == NULL) {
    bullet_info = new ServerBulletInfo();
    server_bullet_info_vector_[chairid].push_back(bullet_info);
  }

  return bullet_info;
}

bool CTableFrameSink::FreeBulletInfo(WORD chairid, ServerBulletInfo* bullet_info) {
  ServerBulletInfoVector::iterator iter;
  for (iter = server_bullet_info_vector_[chairid].begin(); iter != server_bullet_info_vector_[chairid].end(); ++iter) {
    if (bullet_info == *iter) {
      server_bullet_info_vector_[chairid].erase(iter);
      storage_bullet_info_vector_.push_back(bullet_info);
	  --bullet_count[chairid];
      return true;
    }
  }

  //assert(!"FreeBulletInfo Failed");
  return false;
}

void CTableFrameSink::FreeAllBulletInfo(WORD chairid) {
  std::copy(server_bullet_info_vector_[chairid].begin(), server_bullet_info_vector_[chairid].end(), std::back_inserter(storage_bullet_info_vector_));
  server_bullet_info_vector_[chairid].clear();
}

ServerBulletInfo* CTableFrameSink::GetBulletInfo(WORD chairid, int bullet_id) {
  ServerBulletInfoVector::iterator iter;
  ServerBulletInfo* bullet_info = NULL;
  for (iter = server_bullet_info_vector_[chairid].begin(); iter != server_bullet_info_vector_[chairid].end(); ++iter) {
    bullet_info = *iter;
    if (bullet_info->bullet_id == bullet_id) return bullet_info;
  }
 // ASSERT(!"GetBulletInfo:not found");
  return NULL;
}

void CTableFrameSink::OnUpdatge(float dt)
{

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		  IServerUserItem* pUser = m_pITableFrame->GetTableUserItem(i);
		  if (NULL != pUser&& pUser->IsAndroidUser() )
		  {
			    CMD_S_UPDATE_ANDROID UPdateAndroid;
			    m_pITableFrame->SendTableData(i, SUB_S_UPDATE_ANDROID, &UPdateAndroid, sizeof(UPdateAndroid));
				 m_pITableFrame->SendLookonData(i, SUB_S_UPDATE_ANDROID, &UPdateAndroid, sizeof(UPdateAndroid));
			  
		  }
	}
}

void CTableFrameSink::StartAllGameTimer() {
	m_pITableFrame->SetGameTimer(kBuildSmallFishTraceTimer, kBuildSmallFishTraceElasped*1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(kBuildMediumFishTraceTimer, kBuildMediumFishTraceElasped * 1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(kBuildFish18TraceTimer, kBuildFish18TraceElasped * 1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(kBuildFish19TraceTimer, kBuildFish19TraceElasped * 1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(kBuildFish20TraceTimer,kBuildFish20TraceElasped * 1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(kBuildFishLKTraceTimer, kBuildFishLKTraceElasped * 1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(kBuildFishBombTraceTimer, kBuildFishBombTraceElasped * 1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(kBuildFishLockBombTraceTimer, kBuildFishLockBombTraceElasped * 1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(kBuildFishSuperBombTraceTimer, kBuildFishSuperBombTraceElasped * 1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(kBuildFishSanTraceTimer, kBuildFishSanTraceElasped * 1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(kBuildFishSiTraceTimer, kBuildFishSiTraceElasped * 1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(kBuildFishKingTraceTimer, kBuildFishKingTraceElasped * 1000, kRepeatTimer, 0);
  m_pITableFrame->SetGameTimer(TBuildFish41TraceTimer, TBuildFish41TraceElasped, kRepeatTimer, 0);
}

void CTableFrameSink::KillAllGameTimer() {
  m_pITableFrame->KillGameTimer(kBuildSmallFishTraceTimer);
  m_pITableFrame->KillGameTimer(kBuildMediumFishTraceTimer);
  m_pITableFrame->KillGameTimer(kBuildFish18TraceTimer);
  m_pITableFrame->KillGameTimer(kBuildFish19TraceTimer);
  m_pITableFrame->KillGameTimer(kBuildFish20TraceTimer);
  m_pITableFrame->KillGameTimer(kBuildFishLKTraceTimer);
  m_pITableFrame->KillGameTimer(kBuildFishBombTraceTimer);
  m_pITableFrame->KillGameTimer(kBuildFishLockBombTraceTimer);
  m_pITableFrame->KillGameTimer(kBuildFishSuperBombTraceTimer);
  m_pITableFrame->KillGameTimer(kBuildFishSanTraceTimer);
  m_pITableFrame->KillGameTimer(kBuildFishSiTraceTimer);
  m_pITableFrame->KillGameTimer(kBuildFishKingTraceTimer);
  m_pITableFrame->KillGameTimer(TBuildFish41TraceTimer);
}

bool CTableFrameSink::LoadConfig() 
{
	if (g_exchange_ratio_userscore_ != 0) return true;
	TCHAR fileName[MAX_PATH] = { 0 };
	_snwprintf(fileName, CountArray(fileName), TEXT("GameModule//jcby_Config_%d.xml"), m_pGameServiceOption->wServerID);
	//FILE *wenjian2 = _wfopen(L"GameModule//jilu2.txt", L"a+,ccs=UTF-16LE");
	//fclose(wenjian2);
	//panduan = true;
	//TCHAR pBUF[1024] = { 0 };
	//FILE *wenjian = _wfopen(L"GameModule//jilu1.txt", L"a+,ccs=UTF-16LE");
	//if (wenjian)
	//{
	//	while (fgetws(pBUF, 1024, wenjian))
	//	{
	//		jilu.push_back(pBUF);
	//	}
	//	fclose(wenjian);
	//}
	//if (jilu.size() >120)
	//{
	//	FILE *wenjian2 = _wfopen(L"GameModule//jilu1.txt", L"w,ccs=UTF-16LE");

	//	fclose(wenjian2);
	//}
	//
	TiXmlDocument xmlDoc;
	if (!xmlDoc.LoadFile(CT2A(fileName), TIXML_ENCODING_UTF8))
	{
		int row = xmlDoc.ErrorCol();
		const char *p = xmlDoc.ErrorDesc();
		return false;
	}

	TiXmlElement* xmlElement = xmlDoc.FirstChildElement("Config");
	if (xmlElement == NULL)	return false;

	TiXmlElement* xmlChild = NULL;
	int fishKindCount = 0, bulletKindCount = 0;
	for (xmlChild = xmlElement->FirstChildElement(); xmlChild != NULL; xmlChild = xmlChild->NextSiblingElement())
	{
		//(金币:渔币)
		if (!strcmp(xmlChild->Value(), "ScoreExchange"))
		{
			const char* attri = xmlChild->Attribute("exchangeRatio");
			char* temp = NULL;
			g_exchange_ratio_userscore_ = strtol(attri, &temp, 10);
			g_exchange_ratio_fishscore_ = strtol(temp + 1, &temp, 10);
			//每次兑换数量
			xmlChild->Attribute("exchangeCount", &g_exchange_fishscore_count_);
		}
		//金币控制
		else if(!strcmp(xmlChild->Value(), "ScoreControl"))
		{
			//总体返还分数百分比
			xmlChild->Attribute("totalReturnRate", &g_total_return_rate_);
			//总体概率
			xmlChild->Attribute("totalProbability", &g_total_probability_);
		}
		else if (!strcmp(xmlChild->Value(), "YuZhen"))
		{
			const char *atribute=xmlChild->Attribute("yuzhen");
			char *temp = NULL;
			kSwitchSceneElasped = strtol(atribute, &temp, 10);
			
		}
		//炮的倍数
		else if (!strcmp(xmlChild->Value(), "Cannon"))
		{
			const char* attri = xmlChild->Attribute("cannonMultiple");
			std::string attriMinMax = attri;
			int pos = attriMinMax.find(',',0);
			std::string attriMin = attriMinMax.substr(0,pos);
			std::string attriMax = attriMinMax.substr(pos+ 1,attriMinMax.size()- pos);
		 
			g_bullet_multiple_[0] = atof(attriMin.c_str());
			g_bullet_multiple_[1] = atof(attriMax.c_str());
		}
		//大鱼小鱼默认库存
		else if (!strcmp(xmlChild->Value(), "FishStock"))
		{
			const char* attri = xmlChild->Attribute("Stock");
			char* temp = NULL;
			//大鱼默认库存
			g_stock_score_[1] = strtol(attri, &temp, 10);

			
			//小鱼默认库存
			g_stock_score_[0] = strtol(temp + 1, &temp, 10);
			fangshuikucun[0] = 0;
				fangshuikucun[1] = 0;
		}
		//大鱼库存档位
		if (!strcmp(xmlChild->Value(), "StockBig") && g_stock_crucial_count_ == 0)
		{
			for (const TiXmlElement* xml_stock = xmlChild->FirstChildElement(); xml_stock; xml_stock = xml_stock->NextSiblingElement())
			{
				xml_stock->Attribute("stockScore", &g_stock_crucial_score_[g_stock_crucial_count_]);
				xml_stock->Attribute("increaseProbability", &g_stock_increase_probability_[g_stock_crucial_count_]);
				++g_stock_crucial_count_;
				if (g_stock_crucial_count_ >= 12) break;
			}
		}
		//小鱼库存档位
		else if (!strcmp(xmlChild->Value(), "StockSmall") && g_stock_crucial_count_small == 0)
		{
			for (const TiXmlElement* xml_stock = xmlChild->FirstChildElement(); xml_stock; xml_stock = xml_stock->NextSiblingElement())
			{
				xml_stock->Attribute("stockScoreSmall", &g_stock_crucial_score_small[g_stock_crucial_count_small]);
				xml_stock->Attribute("increaseProbabilitySmall", &g_stock_increase_probability_small[g_stock_crucial_count_small]);
				++g_stock_crucial_count_small;
				if (g_stock_crucial_count_small >= 12) break;
			}
		}
		//总库存档位
		else if (!strcmp(xmlChild->Value(), "StockAll") && g_stock_crucial_count_all == 0)
		{
			for (const TiXmlElement* xml_stock = xmlChild->FirstChildElement(); xml_stock; xml_stock = xml_stock->NextSiblingElement())
			{
				xml_stock->Attribute("stockScoreAll", &g_stock_crucial_score_all[g_stock_crucial_count_all]);
				xml_stock->Attribute("increaseProbabilityAll", &g_stock_increase_probability_all[g_stock_crucial_count_all]);
				++g_stock_crucial_count_all;
				if (g_stock_crucial_count_all >= 21) break;
			}
		}
		//赢分限制
		else if (!strcmp(xmlChild->Value(), "WinScoreLimit") && g_win_score_limit_count_ == 0)
		{
			for (const TiXmlElement* xml_stock = xmlChild->FirstChildElement(); xml_stock; xml_stock = xml_stock->NextSiblingElement())
			{
				xml_stock->Attribute("winScore", &g_win_score_limit_[g_win_score_limit_count_]);
				xml_stock->Attribute("decreaseProbability", &g_win_score_probability_[g_win_score_limit_count_]);
				++g_win_score_limit_count_;
				if (g_win_score_limit_count_ >= 20)
				{
					break;
				}
			}
		}
		//炸弹(局部炸弹, 超级炸弹) 当库存小于此值炸弹被击中的几率为0-->
		else if (!strcmp(xmlChild->Value(), "Bomb"))
		{
			const char* attri = xmlChild->Attribute("BombProbability");
			char* temp = NULL;
			g_bomb_stock_ = strtol(attri, &temp, 10);
			g_super_bomb_stock_ = strtol(temp + 1, &temp, 10);
		}
		//<!--彩金池大于（第一值）开启彩金 （第二值）为中彩金最小值 （第三值）为中彩金最大值 税收的千分之（第四值）作为彩金池分-->
		else if (!strcmp(xmlChild->Value(), "Setcaijin"))
		{
			const char* attri = xmlChild->Attribute("Caijin");
			char* temp = NULL;
			g_BeginCaijin = strtol(attri, &temp, 10);
			g_MinCaijing = strtol(temp + 1, &temp, 10);
			g_MaxCaijing = strtol(temp + 1, &temp, 10);
			g_RateToCaijinChi = strtol(temp + 1, &temp, 10);
		}
		//机器人银行操作
		else if (!strcmp(xmlChild->Value(), "AndroidBankOperate"))
		{
			//机器人的分数小于该值取款
			xmlChild->Attribute("RobotScoreMin", &g_RobotScoreMin);
			//机器人的分数大于该值存款
			xmlChild->Attribute("RobotScoreMax", &g_RobotScoreMax);
			//取款最小数额
			xmlChild->Attribute("RobotBankGet", &g_RobotBankGet);
			//取款最大数额(此数值一定要大于RobotBankGet)
			xmlChild->Attribute("RobotBankGetBanker", &g_RobotBankGetBanker);
			//存款百分比，百分之N
			xmlChild->Attribute("RobotBankStoMul", &g_RobotBankStoMul);
		}
		//机器人数量
		else if (!strcmp(xmlChild->Value(), "AndroidCount"))
		{
			const char* attri = xmlChild->Attribute("count");
			char* temp = NULL;
			g_TableMinAndroidCount = strtol(attri, &temp, 10);
			g_TableMaxAndroidCount = strtol(temp + 1, &temp, 10);
		}
		//鱼的属性
		else if (!strcmp(xmlChild->Value(), "Fish"))
		{
			int fish_kind;
			xmlChild->Attribute("kind", &fish_kind);
			if (fish_kind >= FISH_KIND_COUNT || fish_kind < 0) return false;

			//鱼的速度
			xmlChild->Attribute("speed", &g_fish_speed_[fish_kind]);

			//鱼的倍数
			if (fish_kind == FISH_KIND_18) 
			{
				const char* attri = xmlChild->Attribute("multiple");
				char* temp = NULL;
				g_fish_multiple_[fish_kind] = strtol(attri, &temp, 10);
				g_fish18_max_multiple_ = strtol(temp + 1, &temp, 10);
			}
			else if (fish_kind == FISH_KIND_19) 
			{
				const char* attri = xmlChild->Attribute("multiple");
				char* temp = NULL;
				g_fish_multiple_[fish_kind] = strtol(attri, &temp, 10);
				g_fish19_max_multiple_ = strtol(temp + 1, &temp, 10);
			}
			else if (fish_kind == FISH_KIND_21) 
			{
				const char* attri = xmlChild->Attribute("multiple");
				char* temp = NULL;
				g_fish_multiple_[fish_kind] = strtol(attri, &temp, 10);
				g_fishLK_max_multiple_ = strtol(temp + 1, &temp, 10);
			}
			else 
			{
				xmlChild->Attribute("multiple", &g_fish_multiple_[fish_kind]);
			}

			//鱼的边框
			const char* attri = xmlChild->Attribute("BoundingBox");
			char* temp = NULL;
			g_fish_bounding_box_width_[fish_kind] = strtol(attri, &temp, 10);
			g_fish_bounding_box_height_[fish_kind] = strtol(temp + 1, &temp, 10);
			if (fish_kind == FISH_KIND_23) 
			{
				g_bomb_range_width_ = strtol(temp + 1, &temp, 10);
				g_bomb_range_height_ = strtol(temp + 1, &temp, 10);
			}
			//击中半径
			xmlChild->Attribute("hitRadius", &g_fish_hit_radius_[fish_kind]);
			//捕获概率(0-1)
			xmlChild->Attribute("captureProbability", &g_fish_capture_probability_[fish_kind]);
			xmlChild->Attribute("caijin", &caijin[fish_kind]);
			++fishKindCount;
		}
		//子弹属性
		else if (!strcmp(xmlChild->Value(), "Bullet"))
		{
			int bullet_kind;
			xmlChild->Attribute("kind", &bullet_kind);
			if (bullet_kind >= BULLET_KIND_COUNT || bullet_kind < 0) return false;
			xmlChild->Attribute("speed", &g_bullet_speed_[bullet_kind]);
			xmlChild->Attribute("netRadius", &g_net_radius_[bullet_kind]);
			++bulletKindCount;
		}
	}



	if (fishKindCount != FISH_KIND_COUNT) return false;
	if (bulletKindCount != BULLET_KIND_COUNT) return false;

  return true;
}

bool CTableFrameSink::SendGameConfig(IServerUserItem* server_user_item) 
{
  CMD_S_GameConfig game_config;
  game_config.exchange_ratio_userscore = g_exchange_ratio_userscore_;
  game_config.exchange_ratio_fishscore = g_exchange_ratio_fishscore_;
  game_config.exchange_count = g_exchange_fishscore_count_;
  game_config.min_bullet_multiple = g_bullet_multiple_[0];
  game_config.max_bullet_multiple = g_bullet_multiple_[1];
  game_config.bomb_range_width = g_bomb_range_width_;
  game_config.bomb_range_height = g_bomb_range_height_;
  game_config.bomb_stock = g_bomb_stock_;
  game_config.super_bomb_stock = g_super_bomb_stock_;

  //AndroidAction
  game_config.RobotScoreMin = g_RobotScoreMin;
  game_config.RobotScoreMax = g_RobotScoreMax;
  game_config.RobotBankGet = g_RobotBankGet;
  game_config.RobotBankGetBanker = g_RobotBankGetBanker;
  game_config.RobotBankStoMul = g_RobotBankStoMul;

  for (int i = 0; i < FISH_KIND_COUNT; ++i)
  {
	  game_config.fish_multiple[i] = g_fish_multiple_[i];
	  game_config.fish_speed[i] = g_fish_speed_[i];
	  game_config.fish_bounding_box_width[i] = g_fish_bounding_box_width_[i];
	  game_config.fish_bounding_box_height[i] = g_fish_bounding_box_height_[i];
	  game_config.fish_hit_radius[i] = g_fish_hit_radius_[i];
  }

  for (int i = 0; i < BULLET_KIND_COUNT; ++i)
  {
	  game_config.bullet_speed[i] = g_bullet_speed_[i];
	  game_config.net_radius[i] = g_net_radius_[i];
  }

  game_config.bIsGameCheatUser = false;
  if (CUserRight::IsGameCheatUser(server_user_item->GetUserRight()))
  {
	  game_config.bIsGameCheatUser = true;
  }
 
  return m_pITableFrame->SendUserItemData(server_user_item, SUB_S_GAME_CONFIG, &game_config, sizeof(game_config));
}

int CTableFrameSink::GetNewFishID() {
  ++fish_id_;
  if (fish_id_ <= 0) fish_id_ = 1;
  return fish_id_;
}

int CTableFrameSink::GetBulletID(WORD chairid) {
  ASSERT(chairid < GAME_PLAYER);
  ++bullet_id_[chairid];
  if (bullet_id_[chairid] <= 0) bullet_id_[chairid] = 1;
  return bullet_id_[chairid];
}

bool CTableFrameSink::OnTimerBuildSmallFishTrace(WPARAM bind_param) {
  if (CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)) {
    OnEventGameConclude(GAME_PLAYER, NULL, GER_DISMISS);
    return true;
  }
  BuildFishTrace(4 + rand() % 8, FISH_KIND_1, FISH_KIND_10);//markbq0531 4->3 8->6
  return true;
}

bool CTableFrameSink::OnTimerBuildMediumFishTrace(WPARAM bind_param) {
  BuildFishTrace(1 + rand() % 2, FISH_KIND_11, FISH_KIND_17);
  return true;
}

bool CTableFrameSink::OnTimerBuild41FishTrace(WPARAM bind_param) {
	BuildFishTrace(1, FISH_KIND_41, FISH_KIND_41);
	return true;
}

bool CTableFrameSink::OnTimerBuildFish18Trace(WPARAM bind_param) {
  BuildFishTrace(1, FISH_KIND_18, FISH_KIND_18);
  return true;
}

bool CTableFrameSink::OnTimerBuildFish19Trace(WPARAM bind_param) {
  BuildFishTrace(1, FISH_KIND_19, FISH_KIND_19);
  return true;
}

bool CTableFrameSink::OnTimerBuildFish20Trace(WPARAM bind_param) {
  BuildFishTrace(1, FISH_KIND_20, FISH_KIND_20);
  return true;
}

bool CTableFrameSink::OnTimerBuildFishLKTrace(WPARAM bind_param) {
  //BuildFishTrace(1, FISH_KIND_21, FISH_KIND_21);

  CMD_S_FishTrace fish_trace;

  DWORD build_tick = GetTickCount();
  FishTraceInfo* fish_trace_info = ActiveFishTrace();
  fish_trace_info->fish_kind = FISH_KIND_21;
  fish_trace_info->build_tick = build_tick;
  fish_trace_info->fish_id = GetNewFishID();

  fish_trace.fish_id = fish_trace_info->fish_id;
  fish_trace.cmd_version = kCmdVersion;
  fish_trace.fish_kind = fish_trace_info->fish_kind;
  fish_trace.init_count = 3;
  fish_trace.trace_type = TRACE_BEZIER;
  BuildInitTrace(fish_trace.init_pos, fish_trace.init_count, fish_trace.fish_kind, fish_trace.trace_type);

  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_FISH_TRACE, &fish_trace, sizeof(fish_trace));
  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_FISH_TRACE, &fish_trace, sizeof(fish_trace));

  current_fish_lk_multiple_ = g_fish_multiple_[FISH_KIND_21];
  m_pITableFrame->SetGameTimer(kLKScoreTimer, kLKScoreElasped * 1000, g_fishLK_max_multiple_ - g_fish_multiple_[FISH_KIND_21], fish_trace_info->fish_id);

  return true;
}

bool CTableFrameSink::OnTimerBuildFishBombTrace(WPARAM bind_param) {
  BuildFishTrace(1, FISH_KIND_23, FISH_KIND_23);
  return true;
}

bool CTableFrameSink::OnTimerBuildFishLockBombTrace(WPARAM bind_param) {
  BuildFishTrace(1, FISH_KIND_22, FISH_KIND_22);
  return true;
}

bool CTableFrameSink::OnTimerBuildFishSuperBombTrace(WPARAM bind_param) {
  BuildFishTrace(1, FISH_KIND_24, FISH_KIND_24);
  return true;
}

bool CTableFrameSink::OnTimerBuildFishSanTrace(WPARAM bind_param) {
  BuildFishTrace(2, FISH_KIND_25, FISH_KIND_27);
  return true;
}

bool CTableFrameSink::OnTimerBuildFishSiTrace(WPARAM bind_param) {
  BuildFishTrace(2, FISH_KIND_28, FISH_KIND_30);
  return true;
}

bool CTableFrameSink::OnTimerBuildFishKingTrace(WPARAM bind_param) {
  BuildFishTrace(1, FISH_KIND_31, FISH_KIND_40);
  return true;
}

bool CTableFrameSink::OnTimerClearTrace(WPARAM bind_param) {
  ClearFishTrace();
  return true;
}
bool CTableFrameSink::OnTimerBaocun(WPARAM bind_parm)
{
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem* user_item = m_pITableFrame->GetTableUserItem(i);
		if (user_item == NULL) continue;
		if (user_item->IsAndroidUser())continue;
		WORD chair_id = user_item->GetChairID();

		tagScoreInfo score_info;
		memset(&score_info, 0, sizeof(score_info));
		score_info.cbType = SCORE_TYPE_WIN;
		score_info.lScore = (fish_score_[chair_id] - exchange_fish_score_[chair_id]) * g_exchange_ratio_userscore_ / g_exchange_ratio_fishscore_;

		exchange_fish_score_[chair_id] += score_info.lScore;
		//CString str2;
		//str2.Format(_T("%ld,%ld,%ld,%ld,%d"),fish_score_[chair_id],exchange_fish_score_[chair_id],fish_score_[chair_id] - exchange_fish_score_[chair_id],score_info.lScore,exchange_ratio_userscore_ / exchange_ratio_fishscore_);
		//AfxMessageBox(str2);
		m_pITableFrame->WriteUserScore(chair_id, score_info/*wexperience[chair_id]/100*60*exchange_ratio_userscore_ / exchange_ratio_fishscore_*/);

	}
	return true;
}
bool CTableFrameSink::OnTimertichu(WPARAM bind_parm)
{
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem* user_item = m_pITableFrame->GetTableUserItem(i);
		if (user_item == NULL) continue;
		
			if (timercount[i]-- <= 0)
			{
				m_pITableFrame->SendUserItemData(user_item, SUB_timerUp);
				m_pITableFrame->PerformStandUpAction(user_item);
			}
			if (user_item->IsAndroidUser())
				if (bullet_count[user_item->GetChairID()] <= BulletCount)
				{
				m_pITableFrame->SendUserItemData(user_item, SUB_S_canfire, 0, 0);

				}
	}
	return true;

}
bool CTableFrameSink::OnTimerBulletIonTimeout(WPARAM bind_param) {
  WORD chair_id = static_cast<WORD>(bind_param);
  bullet_ion_[chair_id] = false;
  CMD_S_BulletIonTimeout bullet_timeout;
  bullet_timeout.chair_id = chair_id;
  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_BULLET_ION_TIMEOUT, &bullet_timeout, sizeof(bullet_timeout));
  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_BULLET_ION_TIMEOUT, &bullet_timeout, sizeof(bullet_timeout));

  return true;
}

bool CTableFrameSink::OnTimerLockTimeout(WPARAM bind_param) {
  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_LOCK_TIMEOUT);
  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_LOCK_TIMEOUT);
  StartAllGameTimer();

  return true;
}
bool CTableFrameSink::OnTimerjilu(WPARAM bind_param)
{
	for (WORD i = 0; i < GAME_PLAYER; ++i)
	{
		IServerUserItem* user_item = m_pITableFrame->GetTableUserItem(i);
		if (user_item == NULL) continue;
		if (user_item->IsAndroidUser())continue;
		CTime sTime = CTime(CTime::GetCurrentTime().GetTime());
		FILE *wenjian2 = _wfopen(L"GameModule//jilu2.txt", L"a+,ccs = UTF-16LE");
		TCHAR   bufa[1024] = { 0 };
		
		CString strbuffer;
		in_addr a;
		a.s_addr = user_item->GetClientAddr();
		char * fx = inet_ntoa(a);
		strbuffer.Format(TEXT("%s"), CA2T(fx));		
		wsprintfW(bufa, L"ID号:%u,%s,IP:%s,时间:%d.%d  %d:%02ld分%02ld秒,赢分:%d", user_item->GetGameID(),
			user_item->GetUserInfo()->szNickName, strbuffer, sTime.GetMonth(), sTime.GetDay(),
			sTime.GetHour(), sTime.GetMinute(), sTime.GetSecond(),
			fish_score_[i] - exchange_fish_score_[i]);
		fwprintf(wenjian2, L"%s\n", bufa);
		fclose(wenjian2);
		if (fish_score_[i] - exchange_fish_score_[i] >= g_MAXbaojing)
		{
			FILE *wenjian3 = _wfopen(L"GameModule//baojing.txt", L"a+,ccs = UTF-16LE");
			fwprintf(wenjian3, L"%s\n", bufa);
			fclose(wenjian3);
		}
	}
	return true;
}
bool CTableFrameSink::OnTimerSwitchScene(WPARAM bind_param) {
	special_scene_ = true;
	KillAllGameTimer();
	ClearFishTrace(true); 
	if (next_scene_kind_ == SCENE_KIND_1) {
		m_pITableFrame->SetGameTimer(kSceneEndTimer, 46 * 1000, 1, 0);
		BuildSceneKind1();
	}
	else if (next_scene_kind_ == SCENE_KIND_2) {
		m_pITableFrame->SetGameTimer(kSceneEndTimer, 65 * 1000, 1, 0);
		BuildSceneKind2();
	}
	else if (next_scene_kind_ == SCENE_KIND_3) {
		m_pITableFrame->SetGameTimer(kSceneEndTimer, 45 * 1000, 1, 0);
		BuildSceneKind3();
	}
	
	else if (next_scene_kind_ == SCENE_KIND_4) {
		m_pITableFrame->SetGameTimer(kSceneEndTimer, 46 * 1000, 1, 0);
		BuildSceneKind4();
	}

	else if (next_scene_kind_ == SCENE_KIND_5) {
		m_pITableFrame->SetGameTimer(kSceneEndTimer, 32* 1000, 1, 0);
		BuildSceneKind5();
	}
	next_scene_kind_ = static_cast<SceneKind>((next_scene_kind_ + 1) % SCENE_KIND_COUNT);

  return true;
}

bool CTableFrameSink::OnTimerSceneEnd(WPARAM bind_param) {
  special_scene_ = false;
  m_pITableFrame->KillGameTimer(kSceneEndTimer);
  StartAllGameTimer();
  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SCENE_END);
  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SCENE_END);
  return true;
}

bool CTableFrameSink::OnTimerLKScore(WPARAM bind_param)
{
	if (current_fish_lk_multiple_ >= g_fishLK_max_multiple_)
	{
		m_pITableFrame->KillGameTimer(kLKScoreTimer);
		return true;
	}
	CMD_S_HitFishLK hit_fish;
	hit_fish.chair_id = 3;
	hit_fish.fish_id = (int)bind_param;
	hit_fish.fish_mulriple = ++current_fish_lk_multiple_;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_HIT_FISH_LK, &hit_fish, sizeof(hit_fish));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_HIT_FISH_LK, &hit_fish, sizeof(hit_fish));
	if (current_fish_lk_multiple_ == g_fishLK_max_multiple_)
	{
		m_pITableFrame->KillGameTimer(kLKScoreTimer);
	}
	return true;
}
bool CTableFrameSink::OnTimerbodong(WPARAM bind_parm)
{
	/*if (s_bodongmax > 0)
	{
		s_bodongmax--;

		if (suanshijian++==0||suanshijian == S_bodongshijian)
		{
			int h = 0;
				int w = 0;
				while (h==w)
				{
					h = rand() % 8;
					w = rand() % 8;
					
				}
				yizihao[0] = h;
				yizihao[1] = w;

				(suanshijian >= S_bodongshijian) ? suanshijian = 0 : 1;
		}
	}
	else
	{
		yizihao[0] = -1;
		yizihao[1] = -1;
		suanshijian = 0;
	}*/
	return true;

}
bool CTableFrameSink::OnTimerFangShui(WPARAM bind_param)
{

	if (g_fangshui_cur_time >= g_fangshui_time)
	{
		g_bFangShuiStart = false;
		m_pITableFrame->KillGameTimer(kFangShuiTimer);
		g_fangshui_end_time = g_nextfangshui_time = g_fangshui_cur_score = 0;
		return true;
	}
	if (g_bFangShuiStart && g_fangshui_cur_score >= g_stock_fangshui_edu )
	{
		g_fangshui_end_time = CTime::GetCurrentTime().GetTime();
	}
	DWORD dwCurrentTime = CTime::GetCurrentTime().GetTime();
	if (dwCurrentTime > g_nextfangshui_time)
	{
		CTime sTime = CTime(g_nextfangshui_time);
		FILE *wenjian2 = _wfopen(L"GameModule//jilu1.txt", L"a+,ccs=UTF-16LE");
		TCHAR   bufa[1024] = {0};
		
		wsprintfW(bufa, L"%d.%d  %d:%02ld分%02ld秒         %d分   ", sTime.GetMonth(), sTime.GetDay(),
			sTime.GetHour(),sTime.GetMinute(),sTime.GetSecond(),g_fangshui_cur_score);
		fwprintf(wenjian2, L"%s ",bufa);
		fwprintf(wenjian2, L"%d分    ", jilukucun[0] - fangshuikucun[0]);
		fwprintf(wenjian2, L"%d分\n", jilukucun[1] - fangshuikucun[1]);
		//jilu.clear();
		fclose(wenjian2);
		
		SCORE aaa = g_stock_fangshui_yue*(rand() % 30 + 1) / 100;
		fangshuikucun[1] = aaa*(rand() % 30 + 1) / 100;
		fangshuikucun[0] = aaa - fangshuikucun[1];
		jilukucun[1] = fangshuikucun[1];
		jilukucun[0] = aaa-fangshuikucun[1];
		g_lastfangshui_score = g_fangshui_cur_score;
		
		if (g_fangshui_end_time == 0)	g_fangshui_end_time = CTime::GetCurrentTime().GetTime();

		g_lastfangshui_count_time = g_fangshui_end_time - g_fangshui_begin_time;
		srand((unsigned)time(NULL));
		g_fangshui_begin_time = g_nextfangshui_time;
	
		g_fangshui_end_time = g_nextfangshui_time = g_fangshui_cur_score = 0;
		g_fangshui_cur_time++;

	

		g_nextfangshui_time = CTime::GetCurrentTime().GetTime() + 3600;
		
		g_hhhhhhh--;
	}
	
	
	return true;
}

bool CTableFrameSink::SendTableData(WORD sub_cmdid, void* data, WORD data_size, IServerUserItem* exclude_user_item)
{
	
	if (exclude_user_item == NULL)
	{
		m_pITableFrame->SendTableData(INVALID_CHAIR, sub_cmdid, data, data_size);
	}
	else
	{
		IServerUserItem* send_user_item = NULL;
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			send_user_item = m_pITableFrame->GetTableUserItem(i);
			if (send_user_item == NULL) continue;
			if (send_user_item == exclude_user_item) continue;
			m_pITableFrame->SendTableData(send_user_item->GetChairID(), sub_cmdid, data, data_size);
		}
	}
	m_pITableFrame->SendLookonData(INVALID_CHAIR, sub_cmdid, data, data_size);
	return true;
}
bool CTableFrameSink::qujianjisuan(qujian f)
{
	panduan3 = f.panduan;
	int arg=(f.qujianmax - f.qujianmin) / 46;
	qujianfanwei[0] = f.qujianmin;
	qujianfanwei[45] = f.qujianmax;
	for (int i = 1; i < 45; i++)
	{
		qujianfanwei[i] = qujianfanwei[i - 1] + arg;
	}
	return true;
}
bool CTableFrameSink::weizhitiaozheng(double a[8])
{
	for (int i = 0; i < 8; i++)
	{
		chairgailvp[i] = a[i];
	}
	return true;
}
bool CTableFrameSink::heibaigailvtiaozheng(double a[2])
{
	heibaigailv[0] = a[0];
	heibaigailv[1] = a[1];
	return true;
}
bool CTableFrameSink::OnSubExchangeFishScore(IServerUserItem* server_user_item, bool increase)
{
	WORD chair_id = server_user_item->GetChairID();

	CMD_S_ExchangeFishScore exchange_fish_score;
	exchange_fish_score.chair_id = chair_id;
	//每次需要的钱
	SCORE need_user_score = g_exchange_ratio_userscore_ * g_exchange_fishscore_count_ / g_exchange_ratio_fishscore_;
	//用户剩余钱
	SCORE user_leave_score = server_user_item->GetUserScore() - exchange_fish_score_[chair_id] * g_exchange_ratio_userscore_ / g_exchange_ratio_fishscore_;
	auto ffff = server_user_item->GetUserScore();
	if (increase)
	{
		if (need_user_score > user_leave_score)
		{
			if (server_user_item->IsAndroidUser())
			{
				OutputDebugString(TEXT("注释起立"));
				m_pITableFrame->PerformStandUpAction(server_user_item);
				return true;
			}
			else
			{
				fish_score_[chair_id] += (user_leave_score*g_exchange_ratio_fishscore_ / g_exchange_ratio_userscore_);
				exchange_fish_score_[chair_id] += (user_leave_score*g_exchange_ratio_fishscore_ / g_exchange_ratio_userscore_);
				exchange_fish_score.swap_fish_score = (user_leave_score*g_exchange_ratio_fishscore_ / g_exchange_ratio_userscore_);
			}
		}
		else
		{
			fish_score_[chair_id] += g_exchange_fishscore_count_;
			exchange_fish_score_[chair_id] += g_exchange_fishscore_count_;
			exchange_fish_score.swap_fish_score = g_exchange_fishscore_count_;
		}
	}
	else
	{
		if (fish_score_[chair_id] <= 0) return true;
		exchange_fish_score_[chair_id] -= fish_score_[chair_id];
		exchange_fish_score.swap_fish_score = -fish_score_[chair_id];
		fish_score_[chair_id] = 0;
	}

	exchange_fish_score.exchange_fish_score = exchange_fish_score_[chair_id];
	//交换鱼币
	return SendTableData(SUB_S_EXCHANGE_FISHSCORE, &exchange_fish_score, sizeof(exchange_fish_score), server_user_item->IsAndroidUser() ? NULL : server_user_item);
}

bool CTableFrameSink::OnSubUserFire(IServerUserItem * server_user_item, int bullet_id, BulletKind bullet_kind, float angle, float bullet_mul, int lock_fishid, float fire_speed, bool bBadManCheat) {
  WORD chair_id = server_user_item->GetChairID();
  int server_bullet_id = GetBulletID(chair_id);
 

  if ((bullet_id != server_bullet_id)&&(!server_user_item->IsAndroidUser())) {
	  return true;
  }
  // 没真实玩家机器人不打炮
  if (android_chairid_ == INVALID_CHAIR) return true;
  if (bullet_mul < g_bullet_multiple_[0] || bullet_mul > g_bullet_multiple_[1]) 
	  return false;
  //assert(fish_score_[chair_id] >= bullet_mul);
  if (fish_score_[chair_id] < bullet_mul) {
    if (server_user_item->IsAndroidUser()) {
      return true;
    } else {
		return true;
    }
  }

  fish_score_[chair_id] -= bullet_mul;
  if (!server_user_item->IsAndroidUser()) {
	  ModifyUserWinScore(server_user_item, -bullet_mul);

  }

  if (bBadManCheat==true)
  {
	  return true;
  }
 // /////////////////////
  if (lock_fishid > 0 && GetFishTraceInfo(lock_fishid) == NULL) {
	  lock_fishid = 0;
  }
/*
  if (++bullet_count[chair_id] > BulletCount)
  {
	  m_pITableFrame->SendTableData(chair_id, SUB_S_NoFire, &BulletCount, sizeof(BulletCount));
  }*/

  CMD_S_UserFire user_fire;
  user_fire.bullet_kind = bullet_kind;
  user_fire.bullet_id = bullet_id;
  user_fire.angle = angle;
  user_fire.chair_id = server_user_item->GetChairID();
  user_fire.android_chairid = server_user_item->IsAndroidUser() ? android_chairid_ : INVALID_CHAIR;
  user_fire.bullet_mulriple = bullet_mul;
  user_fire.fish_score = -bullet_mul;
  user_fire.lock_fishid = lock_fishid;
  user_fire.fire_speed = fire_speed;
  
  SendTableData(SUB_S_USER_FIRE, &user_fire, sizeof(user_fire), server_user_item->IsAndroidUser() ? NULL : server_user_item);
  
  ServerBulletInfo* bullet_info = ActiveBulletInfo(chair_id);
  bullet_info->bullet_id = user_fire.bullet_id;
  bullet_info->bullet_kind = user_fire.bullet_kind;
  bullet_info->bullet_mulriple = user_fire.bullet_mulriple;
  //
  return true;
}

bool CTableFrameSink::OnSubCatchFish(IServerUserItem* server_user_item, int fish_id, BulletKind bullet_kind, int bullet_id, float bullet_mul)
{
	
	if (bullet_mul < g_bullet_multiple_[0] || bullet_mul > g_bullet_multiple_[1]) 
		return false;
	WORD chair_id = server_user_item->GetChairID();
	//////////////////////////////////////////
	ServerBulletInfo* bullet_info = GetBulletInfo(chair_id, bullet_id);
	if (bullet_info == NULL)
		return true;
	FreeBulletInfo(chair_id, bullet_info);
	//ASSERT(bullet_info->bullet_mulriple == bullet_mul && bullet_info->bullet_kind == bullet_kind);

	if (!(bullet_info->bullet_mulriple == bullet_mul && bullet_info->bullet_kind == bullet_kind))
		return false;
	FishTraceInfo* fish_trace_info = GetFishTraceInfo(fish_id);
	if (fish_trace_info == NULL)
	{
		if (!server_user_item->IsAndroidUser())
		{
			CMD_S_FishMissed user_fire_miss;
			user_fire_miss.chair_id = chair_id;
			user_fire_miss.bullet_mul = bullet_mul;

			fish_score_[chair_id] += bullet_mul;

			ModifyUserWinScore(server_user_item, bullet_mul);

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_FISH_MISSED, &user_fire_miss, sizeof(user_fire_miss));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_FISH_MISSED, &user_fire_miss, sizeof(user_fire_miss));
		}
		return true;
	}
	if (fish_trace_info->fish_kind >= FISH_KIND_COUNT)
	{

		g_stock_score_[0] += bullet_mul;
		return true;
	}

	srand((DWORD)time(NULL) + rand() * 258 + rand() % 10000);
	if (!server_user_item->IsAndroidUser() && g_stock_score_[1] < 0 && g_stock_score_[0] < 0)
	{
		g_stock_score_[0] += bullet_mul;
		return true;

	}


	SCORE fish_score = g_fish_multiple_[fish_trace_info->fish_kind] * bullet_mul;

	int fish_multiple = g_fish_multiple_[fish_trace_info->fish_kind];
	int app = 0;
	if (fish_trace_info->fish_kind == FISH_KIND_18)
	{
		int fish18_mul = g_fish_multiple_[fish_trace_info->fish_kind] + rand() % (g_fish18_max_multiple_ - g_fish_multiple_[fish_trace_info->fish_kind] + 1);
		fish_multiple = fish18_mul;
		fish_score = fish18_mul * bullet_mul;
	}
	else if (fish_trace_info->fish_kind == FISH_KIND_19)
	{
		int fish19_mul = g_fish_multiple_[fish_trace_info->fish_kind] + rand() % (g_fish19_max_multiple_ - g_fish_multiple_[fish_trace_info->fish_kind] + 1);
		fish_score = fish19_mul * bullet_mul;
		fish_multiple = fish19_mul;
	}
	else if (fish_trace_info->fish_kind == FISH_KIND_21)
	{
		fish_score = current_fish_lk_multiple_ * bullet_mul;
		fish_multiple = current_fish_lk_multiple_;
	}
	else if (fish_trace_info->fish_kind == FISH_KIND_41)
	{
		 app = rand() % 4;
		fish_score = T_baoxiang[app] * bullet_mul;
		fish_multiple = T_baoxiang[app];
	}
	(bullet_ion_[chair_id]) ? fish_score *= 2 : 0;

	bool big_fish = fish_multiple >= 30;
	if (fish_trace_info->fish_kind == FISH_KIND_23)
	{
		big_fish = false;
	}
	else if (fish_trace_info->fish_kind == FISH_KIND_24)
	{
		big_fish = true;
	}
	if (!server_user_item->IsAndroidUser())
	{
		SCORE score = SCORE(g_total_return_rate_ *(SCORE)bullet_mul);
		SCORE PPPG = (SCORE)((SCORE)bullet_mul / 2);
		SCORE score1 = (SCORE)bullet_mul;
		experience[chair_id] += SCORE((SCORE)bullet_mul - score);


		if (g_bFangShuiStart && (fangshuikucun[0] > 0 || fangshuikucun[1] > 0) && (g_stock_fangshui_yue > 0 || (g_hhhhhhh <= 1)))
		{

			if (big_fish)
			{
				g_stock_score_[1] += score1;

				g_revenue_score += SCORE((SCORE)bullet_mul - ((SCORE)bullet_mul - score));
				g_stock_score_[1] -= SCORE((SCORE)bullet_mul - ((SCORE)bullet_mul - score));

				if (g_stock_score_[1] - fish_score <= 0)
				{
					return true;
				}
			}
			else
			{

				g_stock_score_[0] += score1;

				g_revenue_score += SCORE((SCORE)bullet_mul - ((SCORE)bullet_mul - score));
				g_stock_score_[0] -= SCORE((SCORE)bullet_mul - ((SCORE)bullet_mul - score));
				if (g_stock_score_[0] - fish_score <= 0)
				{
					return true;
				}
			}


		}
		else
		{

			if (big_fish)
			{
				g_stock_score_[1] += score1;
				g_revenue_score += SCORE((SCORE)bullet_mul - ((SCORE)bullet_mul - score));
				g_stock_score_[1] -= SCORE((SCORE)bullet_mul - ((SCORE)bullet_mul - score));
				if (g_stock_score_[1] - fish_score <= 0)
				{
					return true;
				}
			}
			else
			{

				g_stock_score_[0] += score1;
				g_revenue_score += SCORE((SCORE)bullet_mul - ((SCORE)bullet_mul - score));
				g_stock_score_[0] -= SCORE((SCORE)bullet_mul - ((SCORE)bullet_mul - score));
				if (g_stock_score_[0] - fish_score <= 0)
				{
					return true;
				}
			}


		}
	}
	int change_probability = -1;
	change_probability = (!server_user_item->IsAndroidUser()) ? CheckUserFilter(server_user_item->GetGameID()) : -1;



	//随机概率
	double probability = 0;
	probability = static_cast<double>((rand() % 1000 + 1)) / 1000;
	double icd = g_fish_capture_probability_[fish_trace_info->fish_kind];
	double fish_probability = icd;

	int fish20_catch_count = 0;
	int fish21_catch_count = 0;
	int fishjinsha_catch_count = 0;
	double fish20_catch_probability = 0.0;
	double fish21_catch_probability = 0.0;
	double fishjinsha_catch_probability = 0.0;
	bool fish20_config = false;
	bool fish21_config = false;
	bool fishjinsha_config = false;
	if (fish_trace_info->fish_kind == FISH_KIND_20 && !server_user_item->IsAndroidUser())
	{
		fish20_config = CheckFish20Config(server_user_item->GetGameID(), &fish20_catch_count, &fish20_catch_probability);
		(fish20_config) ?

			fish_probability = fish20_catch_probability : 0;

	}
	else if (fish_trace_info->fish_kind == FISH_KIND_21 && !server_user_item->IsAndroidUser())
	{
		fish21_config = CheckFish21Config(server_user_item->GetGameID(), &fish21_catch_count, &fish21_catch_probability);
		(fish21_config) ?

			fish_probability = fish21_catch_probability : 0;

	}
	else if (fish_trace_info->fish_kind == FISH_KIND_18 && !server_user_item->IsAndroidUser())
	{
		fishjinsha_config = CheckFishJinShaConfig(server_user_item->GetGameID(), &fishjinsha_catch_count, &fishjinsha_catch_probability);
		(fishjinsha_config) ?

			fish_probability = fishjinsha_catch_probability : 0;
	}
	(g_bFangShuiStart) ? fish_probability : (fish_probability *= g_total_probability_);

	for (int i = 0; i < 8; i++)
	{
		(chair_id == i) ? fish_probability *= chairgailvp[i] : 0;
	}
	(fish_trace_info->fish_kind == FISH_KIND_20 && server_user_item->IsAndroidUser()) ? fish_probability = 0.002 : 1;

	fish_probability *= (change_probability == 0) ?heibaigailv[0] : 1;
	fish_probability *= (change_probability == 1) ? heibaigailv[1]: 1;
	DWORD xy1 = server_user_item->GetClientAddr();
	if (Bvar)
		fish_probability *= (server_user_item->IsAndroidUser()) ? 1 :
		biandonguser[server_user_item->GetGameID()];

	fish_probability *= (iphao2.count(xy1)) ? iphao2[xy1] : 1;
	//int iyizi = server_user_item->GetChairID();
	//if (yizihao[0] != -1)
	//{
	//	youxiID[0] = (yizihao[0] == iyizi) ? server_user_item->GetGameID() : -1;
	//	youxiID[1] = (yizihao[1] == iyizi) ? server_user_item->GetGameID() : -1;

	////	(iyizi != yizihao[0] && (iyizi != yizihao[1])) ? ((iyizi % 2) ? fish_probability *= S_hard:fish_probability *= S_easy) : 0;
	//}




	double d_dangwei = 0.0;
	// 0    1  2  3    4    5  6   7   8  9
	//100 200 500 1000 5000 1w 10w 50w 100w 500w
	int i = 0;
	/*for (; i < 10; i++)
	{
		if (paoqian[i] == bullet_mul)
			break;
	}*/
	fish_probability *= paogailv[(int)floor(bullet_mul / g_bullet_multiple_[0]-1)];
	fish_probability *= (bullet_ion_[chair_id]) ?SuperPaoGailv : 1;
	SCORE abc = g_user_win_score_[server_user_item->GetUserID()];

	if (!Bvar1)
		for (int i = 0; i< 20; i++)
		{

		(abc>g_win_score_limit_[i]) ? (d_dangwei = g_win_score_probability_[i]) : 0;

		}

	fish_probability -= d_dangwei;
	SCORE all1 = g_stock_score_[0], all2 = g_stock_score_[1], all3 = all1 + all2;
	double d_gailv = 1.0;;
	if (panduan3)
	{
		double g_gailv = 1.0;
		for (int i = 0; i < 46; i++)
		{
			(all1>qujianfanwei[i]) ? (g_gailv = qjgl[i]) : 0;
		}
		fish_probability *= g_gailv;
	}
	if (!Bvar2)
		if (big_fish)
		{
		for (int i = 0; i < 12; i++)
		{
			(all2 > g_stock_crucial_score_[i]) ? (d_gailv = g_stock_increase_probability_[i]) : 0;
		}
		}
		else
		{
			for (int i = 0; i<12; i++)
			{
				(all1>g_stock_crucial_score_small[i]) ? (d_gailv = g_stock_increase_probability_small[i]) : 0;

			}
		}

	if (RHvar3)
	{
		for (int i = 0; i < 21; i++)
		{
			(all3 > g_stock_crucial_score_all[i]) ? (d_gailv = g_stock_increase_probability_all[i]) : 0;
		}
	}


	fish_probability *= d_gailv;

	(server_user_item->IsAndroidUser()) ? (fish_probability = jiqirengailv) : 1;
	if (g_bFangShuiStart && (fangshuikucun[0]>0 || fangshuikucun[1]>0) && g_stock_fangshui_yue>0)
	{
		fish_probability *= (fangshuikucun[1] > 0) ? ((big_fish) ? g_stock_fangshui_probability : 1) : 1;


		fish_probability *= (fangshuikucun[0] > 0) ? ((!big_fish) ? g_stock_fangshui_probability : 1) : 1;
		if (probability > fish_probability)
		{
			return true;
		}

	}
	else if (probability > fish_probability)
	{
		return true;
	}


	if (fish20_config)
	{
		AddFish20Config(server_user_item->GetGameID(), fish20_catch_count - 1, fish20_catch_probability);
	}
	else if (fish21_config)
	{
		AddFish21Config(server_user_item->GetGameID(), fish21_catch_count - 1, fish21_catch_probability);
	}
	else if (fishjinsha_config)
	{
		AddFishJinShaConfig(server_user_item->GetGameID(), fishjinsha_catch_count - 1, fishjinsha_catch_probability);
	}

	//抓到BOOS和炸弹时
	if (fish_trace_info->fish_kind == FISH_KIND_23 || fish_trace_info->fish_kind == FISH_KIND_24
		|| (fish_trace_info->fish_kind >= FISH_KIND_31 && fish_trace_info->fish_kind <= FISH_KIND_40))
	{
		SaveSweepFish(fish_trace_info->fish_kind, fish_id, bullet_kind, bullet_mul);
		CMD_S_CatchSweepFish catch_sweep_fish;
		catch_sweep_fish.chair_id = chair_id;
		catch_sweep_fish.fish_id = fish_id;
		catch_sweep_fish.bullet_mul = bullet_mul;
		if (bullet_kind >= BULLET_KIND_1_ION)  catch_sweep_fish.bullet_mul *= 2;
		m_pITableFrame->SendTableData(server_user_item->IsAndroidUser() ? android_chairid_ : chair_id, SUB_S_CATCH_SWEEP_FISH, &catch_sweep_fish, sizeof(catch_sweep_fish));
		m_pITableFrame->SendLookonData(server_user_item->IsAndroidUser() ? android_chairid_ : chair_id, SUB_S_CATCH_SWEEP_FISH, &catch_sweep_fish, sizeof(catch_sweep_fish));
	}
	else
	{
		CMD_S_CatchFish catch_fish;
		CMD_S_CatchFish catch_fish1;

		//SCORE caijinScore = 0;
		//SCORE caijinChiScore = g_revenue_score*g_RateToCaijinChi / 1000;

		//////////机器人中彩金不收彩金池影响
		//if (server_user_item->IsAndroidUser())
		//{
		//	if (rand() % 100>92)
		//	{
		//		caijinChiScore = 999999999999;
		//	}
		//	else
		//	{
		//		caijinChiScore = -1;
		//	}
		//}
		//if (0)
		//{
		//	if (probabilitycaijin<(caijin[fish_trace_info->fish_kind] * bullet_mul / 9900) && caijinChiScore >= g_BeginCaijin)//中彩金
		//	{
		//		if (caijinChiScore>g_MinCaijing&&caijinChiScore<g_MaxCaijing)//当抽水开启彩金值开始放彩金
		//		{
		//			caijinScore = g_MinCaijing + (RAND_MAX*rand() + rand()) % (caijinChiScore - g_MinCaijing);
		//		}
		//		else if (caijinChiScore>g_MinCaijing&&caijinChiScore >= g_MaxCaijing)
		//		{
		//			caijinScore = g_MinCaijing + (RAND_MAX*rand() + rand()) % (g_MaxCaijing - g_MinCaijing);
		//		}

		//		if (caijinScore > 0)
		//		{
		//			catch_fish.fish_caijin_score = caijinScore;
		//			//catch_caijinfish.chair_id=server_user_item->GetChairID();	
		//			//catch_caijinfish.caijin_score=caijinScore;	
		//		}
		//		else
		//		{
		//			catch_fish.fish_caijin_score = 0;
		//			//catch_caijinfish.chair_id=server_user_item->GetChairID();
		//			//catch_caijinfish.caijin_score=0;
		//		}
		//	}
		//	else
		//	{
		//		catch_fish.fish_caijin_score = 0;
		//		//catch_caijinfish.chair_id=server_user_item->GetChairID();	
		//		//catch_caijinfish.caijin_score=0;
		//	}
		//}
		fish_score_[chair_id] += (fish_score);


		if (!server_user_item->IsAndroidUser())
		{

			if (g_bFangShuiStart && (fangshuikucun[0]>0 || fangshuikucun[1]>0) && (g_stock_fangshui_yue>0 || (g_hhhhhhh <= 1)))
			{
				if (big_fish)
				{
					if (fangshuikucun[1] - fish_score <= 0)
					{

						g_fangshui_cur_score += fangshuikucun[1];
						g_stock_score_[1] += fangshuikucun[1];
						g_stock_score_[1] -= fish_score;
						fangshuikucun[1] = 0;


					}
					else
					{
						g_fangshui_cur_score += fish_score;

						//	g_stock_score_[1] += fish_score;
						//	g_stock_score_[1] -= fish_score;
						fangshuikucun[1] -= fish_score;
						g_stock_fangshui_yue -= fish_score;

					}
				}
				else
					if (fangshuikucun[0] - fish_score <= 0)
					{


					g_fangshui_cur_score += fangshuikucun[0];
					g_stock_score_[0] += fangshuikucun[0];
					g_stock_score_[0] -= fish_score;
					fangshuikucun[0] = 0;



					}
					else
					{
						g_fangshui_cur_score += fish_score;
						fangshuikucun[0] -= fish_score;

						g_stock_fangshui_yue -= fish_score;

					}
				if (g_hhhhhhh <= 1)
				{
					g_hhhhhhh = 24;
					g_stock_fangshui_edu = g_stock_fangshui_edu - g_stock_meitian_edu;
					g_bFangShuiStart = (g_stock_fangshui_edu <= 0) ? false : true;

					g_stock_fangshui_yue = (g_fangshui_time >= 0) ? g_stock_meitian_edu : 0;



				}

			}
			else
			{
				if (g_hhhhhhh <= 1)
				{
					g_hhhhhhh = 24;
					g_stock_fangshui_edu = g_stock_fangshui_edu - g_stock_meitian_edu;
					g_bFangShuiStart = (g_stock_fangshui_edu <= 0) ? false : true;
					g_stock_fangshui_yue = (g_fangshui_time >= 0) ? g_stock_meitian_edu : 0;

				}



				/*if (!g_bFangShuiStart)*/
				(big_fish) ? (g_stock_score_[1] -= fish_score) : (g_stock_score_[0] -= fish_score);
				/*g_revenue_score -= caijinScore;
				g_stock_score_[1] += caijinScore;*/



				/*	if (!g_bFangShuiStart)*/

				/*g_revenue_score -= caijinScore;
				g_stock_score_[0] += caijinScore;*/
			}

			ModifyUserWinScore(server_user_item, fish_score);

		}
		//////////////////////////////////////////////////////////////////////
		if (catch_fish_count[chair_id] >= yushu)
		{
			catch_fish.bullet_ion = true;
			catch_fish_count[chair_id] = 0;
		}
		else
		{
			if (!bullet_ion_[chair_id])
			{
				++catch_fish_count[chair_id];
			}
			catch_fish.bullet_ion = false;
		}
		
		catch_fish.chair_id = chair_id;
		catch_fish.fish_id = fish_id;
		catch_fish.fish_kind = fish_trace_info->fish_kind;
		catch_fish.fish_score = fish_score;
		catch_fish.fish_caijin_score = 0;
		catch_fish.app = app;
		catch_fish1 = catch_fish;
		catch_fish1.fish_score = fish_score_[catch_fish.chair_id];
		if (fish_trace_info->fish_kind == FISH_KIND_22)
		{
			m_pITableFrame->SetGameTimer(kLockTimer, kLockTime * 1000, 1, 0);
			KillAllGameTimer();
		}

		if (catch_fish.bullet_ion)
		{
			bullet_ion_[chair_id] = true;
			m_pITableFrame->SetGameTimer(kBulletIonTimer + chair_id, kBulletIonTime * 1000, 1, 0);
		}

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_zongfen, &catch_fish1,sizeof(catch_fish1) );
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CATCH_FISH, &catch_fish, sizeof(catch_fish));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CATCH_FISH, &catch_fish, sizeof(catch_fish));

		if (fish_trace_info->fish_kind == FISH_KIND_21) 
		{
			current_fish_lk_multiple_ = g_fish_multiple_[FISH_KIND_21];
		}
		/*if (fish_trace_info->fish_kind == FISH_KIND_20)
		{
		TCHAR tips_msg[1024] = { 0 };
		_sntprintf_s(tips_msg, CountArray(tips_msg), TEXT("%s %d桌的企鹅王被玩家%s打死了，获得%I64d鱼币奖励!"),
		m_pGameServiceOption->szServerName, m_pITableFrame->GetTableID() + 1, server_user_item->GetNickName(), fish_score);
		m_pITableFrame->SendGameMessage(tips_msg, SMT_CHAT | SMT_GLOBAL);
		}
		else if (fish_trace_info->fish_kind == FISH_KIND_21)
		{
		TCHAR tips_msg[1024] = { 0 };
		_sntprintf_s(tips_msg, CountArray(tips_msg), TEXT("%s %d桌的李逵被玩家%s打死了，获得%I64d鱼币奖励!"),
		m_pGameServiceOption->szServerName, m_pITableFrame->GetTableID() + 1, server_user_item->GetNickName(), fish_score);
		m_pITableFrame->SendGameMessage(tips_msg, SMT_CHAT | SMT_GLOBAL);
		}
		else if (fish_trace_info->fish_kind == FISH_KIND_19)
		{
		TCHAR tips_msg[1024] = { 0 };
		_sntprintf_s(tips_msg, CountArray(tips_msg), TEXT("%s %d桌的四海游龙被玩家%s打死了，获得%I64d鱼币奖励!"),
		m_pGameServiceOption->szServerName, m_pITableFrame->GetTableID() + 1, server_user_item->GetNickName(), fish_score);
		m_pITableFrame->SendGameMessage(tips_msg, SMT_CHAT | SMT_GLOBAL);
		}
		else if (fish_trace_info->fish_kind == FISH_KIND_18)
		{
		TCHAR tips_msg[1024] = { 0 };
		_sntprintf_s(tips_msg, CountArray(tips_msg), TEXT("%s %d桌的大黄鲨被玩家%s打死了，获得%I64d鱼币奖励!"),
		m_pGameServiceOption->szServerName, m_pITableFrame->GetTableID() + 1, server_user_item->GetNickName(), fish_score);
		m_pITableFrame->SendGameMessage(tips_msg, SMT_CHAT | SMT_GLOBAL);
		}*/

		/*	if (caijinScore>0)
		{
		current_fish_lk_multiple_ = g_fish_multiple_[FISH_KIND_21];
		TCHAR tips_msg[1024] = { 0 };
		_sntprintf_s(tips_msg, CountArray(tips_msg), TEXT("恭喜%s%d桌的%s打中彩金鱼，额外获得%I64d鱼币奖励!"),
		m_pGameServiceOption->szServerName, m_pITableFrame->GetTableID() + 1, server_user_item->GetNickName(), caijinScore);

		if (g_stock_score_[0] - caijinScore < 0)
		return true;
		else
		g_stock_score_[0] -= caijinScore;
		m_pITableFrame->SendGameMessage(tips_msg, SMT_CHAT | SMT_GLOBAL);
		}*/
	}

	FreeFishTrace(fish_trace_info);
	return true; 
}

bool CTableFrameSink::OnSubCatchSweepFish(IServerUserItem* server_user_item, int fish_id, int* catch_fish_id, int catch_fish_count) 
{
	SweepFishInfo* sweep_fish_info = GetSweepFish(fish_id);
	if (sweep_fish_info == NULL) return true;
	//assert(sweep_fish_info->fish_kind == FISH_KIND_23 || sweep_fish_info->fish_kind == FISH_KIND_24 
	//|| (sweep_fish_info->fish_kind >= FISH_KIND_31 && sweep_fish_info->fish_kind <= FISH_KIND_40));
	if (!(sweep_fish_info->fish_kind == FISH_KIND_23 || sweep_fish_info->fish_kind == FISH_KIND_24
		|| (sweep_fish_info->fish_kind >= FISH_KIND_31 && sweep_fish_info->fish_kind <= FISH_KIND_40))) return false;

	WORD chair_id = server_user_item->GetChairID();

	SCORE fish_score = g_fish_multiple_[sweep_fish_info->fish_kind] * sweep_fish_info->bullet_mulriple;
	if (sweep_fish_info->fish_kind == FISH_KIND_18)
	{
		int fish18_mul = g_fish_multiple_[sweep_fish_info->fish_kind] + rand() % (g_fish18_max_multiple_ - g_fish_multiple_[sweep_fish_info->fish_kind] + 1);
		fish_score = fish18_mul * sweep_fish_info->bullet_mulriple;
	}
	else if (sweep_fish_info->fish_kind == FISH_KIND_19)
	{
		int fish19_mul = g_fish_multiple_[sweep_fish_info->fish_kind] + rand() % (g_fish19_max_multiple_ - g_fish_multiple_[sweep_fish_info->fish_kind] + 1);
		fish_score = fish19_mul * sweep_fish_info->bullet_mulriple;
	}
	FishTraceInfoVecor::iterator iter;
	FishTraceInfo* fish_trace_info = NULL;
	for (int i = 0; i < catch_fish_count; ++i)
	{
		for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end(); ++iter)
		{
			fish_trace_info = *iter;
			if (fish_trace_info->fish_id == catch_fish_id[i])
			{
				fish_score += g_fish_multiple_[fish_trace_info->fish_kind] * sweep_fish_info->bullet_mulriple;

				active_fish_trace_vector_.erase(iter);
				storage_fish_trace_vector_.push_back(fish_trace_info);
				break;
			}
		}
	}
	if (sweep_fish_info->bullet_kind >= BULLET_KIND_1_ION) fish_score *= 2;
	fish_score_[chair_id] += fish_score;

	if (!server_user_item->IsAndroidUser())
	{
		ModifyUserWinScore(server_user_item, fish_score);
	}
	if (g_stock_score_[1] - fish_score < 0)
		return true;
	CMD_S_CatchSweepFishResult catch_sweep_result;
	memset(&catch_sweep_result, 0, sizeof(catch_sweep_result));
	catch_sweep_result.fish_id = fish_id;
	catch_sweep_result.chair_id = chair_id;
	catch_sweep_result.fish_score = fish_score;
	catch_sweep_result.catch_fish_count = catch_fish_count;
	memcpy(catch_sweep_result.catch_fish_id, catch_fish_id, catch_fish_count * sizeof(int));
	
	g_stock_score_[1] -= fish_score;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CATCH_SWEEP_FISH_RESULT, &catch_sweep_result, sizeof(catch_sweep_result));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CATCH_SWEEP_FISH_RESULT, &catch_sweep_result, sizeof(catch_sweep_result));

	FreeSweepFish(fish_id);

	return true;
}
bool CTableFrameSink::onsubipfasong(IServerUserItem* server_user_item)
{
	CMD_S_a wangz = {};
	int j = 0;
 
	for(std::map<DWORD,double>::iterator itr = iphao2.begin();itr!= iphao2.end();itr++) 
	{
		wangz.abc[j] = itr->first;
		wangz.gailv[j++] = itr->second;
	}

	m_pITableFrame->SendUserItemData(server_user_item, sub_iptianjiashanchu, &wangz, sizeof(wangz));
	return true;
}
bool CTableFrameSink::onSubtianjia(IServerUserItem* server_user_item, tianjiashanchu game_IP)
{
	iphao2[game_IP.abc] = game_IP.gailv;

	CMD_S_a wangz = {};
	int j = 0;
	for(std::map<DWORD,double>::iterator itr = iphao2.begin();itr!= iphao2.end();itr++) 
	{
		wangz.abc[j] = itr->first;
		wangz.gailv[j++] = itr->second; 
	}
	
	m_pITableFrame->SendUserItemData(server_user_item, sub_iptianjiashanchu, &wangz, sizeof(wangz));
	return true;
}


bool CTableFrameSink::onsubshanchu(IServerUserItem* server_user_item, DWORD game_IP)
{
	iphao2.erase(game_IP);
	CMD_S_a wangz = {};
	int j = 0;
	for(std::map<DWORD,double>::iterator itr = iphao2.begin();itr!= iphao2.end();itr++) 
	{
		wangz.abc[j] = itr->first;
		wangz.gailv[j++] = itr->second;
	}
	m_pITableFrame->SendUserItemData(server_user_item, sub_iptianjiashanchu, &wangz, sizeof(wangz));
	return true;
}

bool CTableFrameSink::OnSubHitFishLK(IServerUserItem* server_user_item, int fish_id)
{
	FishTraceInfo* fish_trace_info = GetFishTraceInfo(fish_id);
	if (fish_trace_info == NULL) return true;
	if (fish_trace_info->fish_kind != FISH_KIND_21) return true;
	if (current_fish_lk_multiple_ >= g_fishLK_max_multiple_) return true;

	++current_fish_lk_multiple_;
	CMD_S_HitFishLK hit_fish;
	hit_fish.chair_id = server_user_item->GetChairID();
	hit_fish.fish_id = fish_id;
	hit_fish.fish_mulriple = current_fish_lk_multiple_;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_HIT_FISH_LK, &hit_fish, sizeof(hit_fish));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_HIT_FISH_LK, &hit_fish, sizeof(hit_fish));
	return true;
}

bool CTableFrameSink::OnSubFish20Config(IServerUserItem* server_user_item, DWORD game_id, int catch_count, double catch_probability)
{
	if (!CUserRight::IsGameCheatUser(server_user_item->GetUserRight())) return false;

	AddFish20Config(game_id, catch_count, catch_probability);

	return true;
}

bool CTableFrameSink::OnSubFish21Config(IServerUserItem* server_user_item, DWORD game_id, int catch_count, double catch_probability)
{
	if (!CUserRight::IsGameCheatUser(server_user_item->GetUserRight())) return false;

	AddFish21Config(game_id, catch_count, catch_probability);

	return true;
}

bool CTableFrameSink::OnSubFishJinShaConfig(IServerUserItem* server_user_item, DWORD game_id, int catch_count, double catch_probability)
{
	if (!CUserRight::IsGameCheatUser(server_user_item->GetUserRight())) return false;

	AddFishJinShaConfig(game_id, catch_count, catch_probability);

	return true;
}
bool CTableFrameSink::OnSubshanchujilu(IServerUserItem* server_user_item, int zz)
{
	static std::vector<CString>::iterator xx = baojingjilu.begin();
	FILE *wenjian3 = _wfopen(L"GameModule//baojing.txt", L"w,ccs = UTF-16LE");
	for (int i = 0; i < baojingjilu.size(); i++,xx++)
	{
		
		if (i == zz)
		{
			baojingjilu.erase(xx);
			
			break;
		}
	}
	for (int i = 0; i < baojingjilu.size(); i++)
	{
		fwprintf(wenjian3, L"%s", baojingjilu[i].GetString());
	}
	fclose(wenjian3);
	return true;
}
bool CTableFrameSink::OnSubzongkucun(IServerUserItem* server_user_item)
{

	CMD_S_CONFIG AdminConfig;


		for (int i = 0; i < 21; i++)
		{
			AdminConfig.stock_crucial_score_All[i] = g_stock_crucial_score_all[i];
			AdminConfig.stock_increase_probability_All[i] = g_stock_increase_probability_all[i];
		}
		AdminConfig.RHRHvar3 = RHvar3;
		AdminConfig.BulletCount = BulletCount;
		AdminConfig.LangchaoTime = kSwitchSceneElasped;
		AdminConfig.Bullet_Count = yushu;
		AdminConfig.BulletTimer = kBulletIonTime;
		AdminConfig.Bullet_gailv = SuperPaoGailv;
		AdminConfig.heibaigailv[0] = heibaigailv[0];
		AdminConfig.heibaigailv[1] = heibaigailv[1];
		return m_pITableFrame->SendTableData(server_user_item->GetChairID(), SUB_zongKC, &AdminConfig, sizeof(AdminConfig));
}
bool  CTableFrameSink::OnSubZKCxiugai(IServerUserItem* server_user_item, CMD_S_CONFIG AdminConfig)
{
	CMD_S_CONFIG afx;
	RHvar3 = AdminConfig.RHRHvar3;
	if (RHvar3)
	{
		for (int i = 0; i < 21; i++)
		{
			g_stock_crucial_score_all[i] = AdminConfig.stock_crucial_score_All[i];
			g_stock_increase_probability_all[i] = AdminConfig.stock_increase_probability_All[i];
		}
		for (int i = 0; i < 20; i++)
		{
			for (int j = 0; j < 20 - i; ++j)
				if (g_stock_crucial_score_all[j]>g_stock_crucial_score_all[j + 1])
				{

				SCORE tmp = g_stock_crucial_score_all[j];
				g_stock_crucial_score_all[j] = g_stock_crucial_score_all[j + 1];
				g_stock_crucial_score_all[j + 1] = tmp;
				SCORE tmp1 = g_stock_increase_probability_all[j];
				g_stock_increase_probability_all[j] = g_stock_increase_probability_all[j + 1];
				g_stock_increase_probability_all[j + 1] = tmp1;

				}
		}

		for (int i = 0; i < 21; i++)
		{
			afx.stock_crucial_score_All[i] = g_stock_crucial_score_all[i];
			afx.stock_increase_probability_All[i] = g_stock_increase_probability_all[i];
		}

		m_pITableFrame->SendTableData(server_user_item->GetChairID(), SUB_zongKC, &afx, sizeof(afx));
	}
	return true;
}


bool CTableFrameSink::OnSubbaojingjilu(IServerUserItem* server_user_item)
{
	TCHAR bufa[150] = { 0 };
	FILE *wenjian3 = _wfopen(L"GameModule//baojing.txt", L"a+,ccs = UTF-16LE");
	baojingjilu.clear();
	while (fgetws(bufa, 150, wenjian3))
	{	
		baojingjilu.push_back(bufa);
		if (baojingjilu.size()>150)
		{
			FILE *wenjian2 = _wfopen(L"GameModule//baojing.txt", L"w,ccs=UTF-16LE");
			baojingjilu.clear();
			fclose(wenjian2);
		}
		m_pITableFrame->SendTableData(server_user_item->GetChairID(), SUB_baojing, bufa, 150);
	}

	fclose(wenjian3);

	return true;
}

bool CTableFrameSink::OnSubScanFilter(IServerUserItem* server_user_item)
{
	std::vector<DWORD>::iterator iter;

	CMD_S_WhiteBlackList list;
	memset(&list,0,sizeof(CMD_S_WhiteBlackList));

	int i;
	int j;
	for (iter = g_white_list_.begin(), i=0; iter != g_white_list_.end()&&i<30; ++iter)
	{
		if (*iter!=0)
		{
			list.WhiteList[i]=*iter;
			++i;
		}

	}

	for (iter = g_balck_list_.begin(), j=0; iter != g_balck_list_.end()&&j<30; ++iter)
	{
		if (*iter!=0)
		{
			list.BlackList[j]=*iter;
			++j;
		}
	}

	m_pITableFrame->SendUserItemData(server_user_item, SUB_S_HEIBAI_LIST, &list, sizeof(list));

	return true;
}


bool CTableFrameSink::OnSubScanFilter2(IServerUserItem* server_user_item)
{
	std::vector<Fish20Config>::iterator iter;
	std::vector<Fish21Config>::iterator iter2;
	std::vector<FishJinShaConfig>::iterator iter3;

	CMD_S_SpecialList list;
	memset(&list,0,sizeof(CMD_S_SpecialList));

	int i;
	for (iter = g_fish20_config_.begin(), i=0; iter != g_fish20_config_.end()&&i<30; ++iter)
	{
		//if (*iter!=0)
		//{
			Fish20Config &tmp=*iter;
			if (tmp.game_id!=0)
			{
				list.fish20gameid[i]=tmp.game_id;
				list.fish20gameidkind[i]=20;
				list.fish20count[i]=tmp.catch_count;
				list.fish20probability[i]=tmp.catch_probability;
				++i;
			}	
		//}
	}

	int j;
	for (iter2 = g_fish21_config_.begin(), j=0; iter2 != g_fish21_config_.end()&&j<30; ++iter2)
	{
		//if (*iter!=0)
		//{
			Fish21Config &tmp=*iter2;
			if (tmp.game_id!=0)
			{
				list.fish21gameid[j]=tmp.game_id;
				list.fish21gameidkind[j]=21;
				list.fish21count[j]=tmp.catch_count;
				list.fish21probability[j]=tmp.catch_probability;
				++j;

			}
		/*}*/
	}

	int k;
	for (iter3 = g_fishjinsha_config_.begin(), k=0; iter3 != g_fishjinsha_config_.end()&&k<30; ++iter3)
	{
		//if (*iter!=0)
		//{
			FishJinShaConfig &tmp=*iter3;
			if (tmp.game_id!=0)
			{
				list.fishjinsha[k]=tmp.game_id;
				list.fishjinshakind[k]=18;
				list.fishjinshacount[k]=tmp.catch_count;
				list.fishjinshaprobability[k]=tmp.catch_probability;
				++k;
			}

		//}
	}

	m_pITableFrame->SendUserItemData(server_user_item, SUB_S_FISHTESHU_LIST, &list, sizeof(list));

	return true;
}

bool CTableFrameSink::Onsubtichu(IServerUserItem* server_user_item,DWORD abc)
{
	userIDD kickuser;
	for (int i = 0; i < iphao1.size(); i++)
	{
		if (iphao1[i].id23 == abc)
		{
			kickuser.UserID = iphao1[i].USERID;
			 m_pITableFrame->SendTableData(server_user_item->GetChairID(), SUB_fanhuiiDhao, &kickuser, sizeof(kickuser));
			 break;
		}
	}
	return true;

}
bool CTableFrameSink::OnSubtiaozheng(winlose var)
{
	winpercentage = var.win233;
	losepercentage = var.lose233;
	Bvar = var.var233[0];
	srand(time(0));
	//
const int b = biandonguser.size();
int h = zaixianwanjia*winpercentage;
int q = zaixianwanjia*losepercentage;
int f = h + q;

	bool var1[500] = { 0 };
	bool go = 0;
	int i1 = 0;
	for ( i1 = 0; h>0&&q>0; i1++)
	{
		go=rand() % 2;
		(go == 1) ? --h : --q;
		var1[i1] = go;
	}

	int aa1 = (h > 0 )?1:0;

	for (; i1 < f; i1++)
		var1[i1] = (aa1) ? 1 : 0;

	int i = 0;
	for ( i = 0; i < f; i++)
	{
	
			biandonguser[iphao1[i].id23] = (var1[i]) ? 1.3 : 0.8;
		
	}
	for (; i < b; i++)
	{
		biandonguser[iphao1[i].id23] = 1;
	}
	return true;

}
bool CTableFrameSink::Onsubtiaozhengpao(IServerUserItem* server_user_item, pao10086 pao)
{
	for (int i = 0; i < 10; i++)
		paogailv[i] = pao.paogailv[i];

	return true;
}

bool CTableFrameSink::OnSubUserFilter(IServerUserItem* pIServerUserItem, DWORD dwGameID, BYTE cbOperateCode)
{
	if (!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())) 
		return false;
	AddUserFilter(dwGameID, cbOperateCode);

  return true;
}

void CTableFrameSink::BuildInitTrace(FPoint init_pos[5], int init_count, FishKind fish_kind, TraceType trace_type) {
  //assert(init_count >= 2 && init_count <= 3);
  srand(GetTickCount() + rand() % kResolutionWidth);
  WORD chair_id = rand() % GAME_PLAYER;
  int center_x = kResolutionWidth / 2;
  int center_y = kResolutionHeight / 2;
  int factor = rand() % 2 == 0 ? 1 : -1;
  switch (chair_id) {
    case 0:
    case 1:
    case 2:
      init_pos[0].x = static_cast<float>(center_x + factor * (rand() % center_x));
	  init_pos[0].y = 0.f - static_cast<float>(g_fish_bounding_box_height_[fish_kind]) * 2;
      init_pos[1].x = static_cast<float>(center_x + factor * (rand() % center_x));
      init_pos[1].y = static_cast<float>(center_y + (rand() % center_y));
      init_pos[2].x = static_cast<float>(center_x - factor * (rand() % center_x));
	  init_pos[2].y = static_cast<float>(kResolutionHeight + g_fish_bounding_box_height_[fish_kind] * 2);
      break;
    case 3:
		init_pos[0].x = static_cast<float>(kResolutionWidth + g_fish_bounding_box_width_[fish_kind] * 2);
      init_pos[0].y = static_cast<float>(center_y + factor* (rand() % center_y));
      init_pos[1].x = static_cast<float>(center_x - (rand() % center_x));
      init_pos[1].y = static_cast<float>(center_y + factor* (rand() % center_y));
	  init_pos[2].x = -static_cast<float>(g_fish_bounding_box_width_[fish_kind] * 2);
      init_pos[2].y = static_cast<float>(center_y - factor* (rand() % center_y));
      break;
    case 5:
    case 6:
    case 4:
      init_pos[0].x = static_cast<float>(center_x + factor * (rand() % center_x));
	  init_pos[0].y = kResolutionHeight + static_cast<float>(g_fish_bounding_box_height_[fish_kind] * 2);
      init_pos[1].x = static_cast<float>(center_x + factor * (rand() % center_x));
      init_pos[1].y = static_cast<float>(center_y - (rand() % center_y));
      init_pos[2].x = static_cast<float>(center_x - factor * (rand() % center_x));
	  init_pos[2].y = static_cast<float>(-g_fish_bounding_box_height_[fish_kind] * 2);
      break;
    case 7:
		init_pos[0].x = static_cast<float>(-g_fish_bounding_box_width_[fish_kind] * 2);
		init_pos[0].y = static_cast<float>(center_y + factor* (rand() % center_y));
		init_pos[1].x = static_cast<float>(center_x + (rand() % center_x));
		init_pos[1].y = static_cast<float>(center_y + factor* (rand() % center_y));
		init_pos[2].x = static_cast<float>(kResolutionWidth + g_fish_bounding_box_width_[fish_kind] * 2);
		init_pos[2].y = static_cast<float>(center_y - factor* (rand() % center_y));
		break;
  }

  if (trace_type == TRACE_LINEAR) {
    init_pos[1].x = init_pos[2].x;
    init_pos[1].y = init_pos[2].y;
  }
}

void CTableFrameSink::BuildFishTrace(int fish_count, FishKind fish_kind_start, FishKind fish_kind_end) {
  BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
  WORD send_size = 0;
  CMD_S_FishTrace* fish_trace = reinterpret_cast<CMD_S_FishTrace*>(tcp_buffer);

  DWORD build_tick = GetTickCount();
  srand(build_tick + fish_count * 123321);
  for (int i = 0; i < fish_count; ++i) {
    if (send_size + sizeof(CMD_S_FishTrace) > sizeof(tcp_buffer)) {
      m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_FISH_TRACE, tcp_buffer, send_size);
      m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_FISH_TRACE, tcp_buffer, send_size);
      send_size = 0;
    }

    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = static_cast<FishKind>(fish_kind_start + (rand() + i) % (fish_kind_end - fish_kind_start + 1));
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    fish_trace->fish_id = fish_trace_info->fish_id;
	fish_trace->cmd_version = rand() % 35 + 1;
    fish_trace->fish_kind = fish_trace_info->fish_kind;
    if (fish_trace_info->fish_kind == FISH_KIND_1 || fish_trace_info->fish_kind == FISH_KIND_2) {
		fish_trace->init_count = 2;
      fish_trace->trace_type = TRACE_LINEAR;
    } else {
		fish_trace->init_count = 3;
      fish_trace->trace_type = TRACE_BEZIER;
    } 
    BuildInitTrace(fish_trace->init_pos, fish_trace->init_count, fish_trace->fish_kind, fish_trace->trace_type);

    send_size += sizeof(CMD_S_FishTrace);
    ++fish_trace;
  }

  if (send_size > 0) {
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_FISH_TRACE, tcp_buffer, send_size);
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_FISH_TRACE, tcp_buffer, send_size);
  }
}

void CTableFrameSink::BuildSceneKind1() {
  BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
  CMD_S_SwitchScene* switch_scene = reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);
  switch_scene->scene_kind = next_scene_kind_;
  DWORD build_tick = GetTickCount();
  switch_scene->fish_count = 0;
  for (int i = 0; i < 100; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_1;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 100;
  for (int i = 0; i < 17; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_3;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 17;
  for (int i = 0; i < 17; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_5;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 17;
  for (int i = 0; i < 30; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_2;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 30;
  for (int i = 0; i < 30; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_4;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 30;
  for (int i = 0; i < 15; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_6;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 15;

  FishTraceInfo* fish_trace_info = ActiveFishTrace();
  fish_trace_info->fish_kind = FISH_KIND_18;
  fish_trace_info->build_tick = build_tick;
  fish_trace_info->fish_id = GetNewFishID();

  switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
  switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
  switch_scene->fish_count += 1;

  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
}

void CTableFrameSink::BuildSceneKind2() {
  BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
  CMD_S_SwitchScene* switch_scene = reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);
  switch_scene->scene_kind = next_scene_kind_;
  DWORD build_tick = GetTickCount();
  switch_scene->fish_count = 0;
  for (int i = 0; i < 200; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_1;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 200;
  for (int i = 0; i < 14; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = static_cast<FishKind>(FISH_KIND_12 + i % 7);
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 14;

  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
}

void CTableFrameSink::BuildSceneKind3() {
  BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
  CMD_S_SwitchScene* switch_scene = reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);
  switch_scene->scene_kind = next_scene_kind_;
  DWORD build_tick = GetTickCount();
  switch_scene->fish_count = 0;
  for (int i = 0; i < 50; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_1;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 50;

  for (int i = 0; i < 40; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_3;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 40;

  for (int i = 0; i < 30; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_4;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 30;

  FishTraceInfo* fish_trace_info = ActiveFishTrace();
  fish_trace_info->fish_kind = FISH_KIND_16;
  fish_trace_info->build_tick = build_tick;
  fish_trace_info->fish_id = GetNewFishID();

  switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
  switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
  switch_scene->fish_count += 1;

  for (int i = 0; i < 50; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_1;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 50;

  for (int i = 0; i < 40; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_2;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 40;

  for (int i = 0; i < 30; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_5;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 30;

  fish_trace_info = ActiveFishTrace();
  fish_trace_info->fish_kind = FISH_KIND_17;
  fish_trace_info->build_tick = build_tick;
  fish_trace_info->fish_id = GetNewFishID();

  switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
  switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
  switch_scene->fish_count += 1;

  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
}

void CTableFrameSink::BuildSceneKind4() {
  BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
  CMD_S_SwitchScene* switch_scene = reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);
  switch_scene->scene_kind = next_scene_kind_;
  DWORD build_tick = GetTickCount();
  switch_scene->fish_count = 0;
  for (int i = 0; i < 8; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_11;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 8;
  for (int i = 0; i < 8; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_12;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 8;
  for (int i = 0; i < 8; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_13;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 8;
  for (int i = 0; i < 8; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_14;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 8;
  for (int i = 0; i < 8; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_15;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 8;
  for (int i = 0; i < 8; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_16;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 8;
  for (int i = 0; i < 8; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_17;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 8;
  for (int i = 0; i < 8; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_18;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 8;

  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
}

void CTableFrameSink::BuildSceneKind5() {
  BYTE tcp_buffer[SOCKET_TCP_PACKET] = { 0 };
  CMD_S_SwitchScene* switch_scene = reinterpret_cast<CMD_S_SwitchScene*>(tcp_buffer);
  switch_scene->scene_kind = next_scene_kind_;
  DWORD build_tick = GetTickCount();
  switch_scene->fish_count = 0;


  for (int i = 0; i < 40; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_1;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 40;
  for (int i = 0; i < 40; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_2;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 40;
  for (int i = 0; i < 40; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_5;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 40;
  for (int i = 0; i < 40; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_3;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 40;
  for (int i = 0; i < 24; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_4;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 24;
  for (int i = 0; i < 24; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_6;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 24;
  for (int i = 0; i < 13; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_7;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 13;
  for (int i = 0; i < 13; ++i) {
    FishTraceInfo* fish_trace_info = ActiveFishTrace();
    fish_trace_info->fish_kind = FISH_KIND_6;
    fish_trace_info->build_tick = build_tick;
    fish_trace_info->fish_id = GetNewFishID();

    switch_scene->fish_id[switch_scene->fish_count + i] = fish_trace_info->fish_id;
    switch_scene->fish_kind[switch_scene->fish_count + i] = fish_trace_info->fish_kind;
  }
  switch_scene->fish_count += 13;

  FishTraceInfo* fish_trace_info = ActiveFishTrace();
  fish_trace_info->fish_kind = FISH_KIND_18;
  fish_trace_info->build_tick = build_tick;
  fish_trace_info->fish_id = GetNewFishID();
  switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
  switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
  switch_scene->fish_count += 1;

  fish_trace_info = ActiveFishTrace();
  fish_trace_info->fish_kind = FISH_KIND_17;
  fish_trace_info->build_tick = build_tick;
  fish_trace_info->fish_id = GetNewFishID();
  switch_scene->fish_id[switch_scene->fish_count] = fish_trace_info->fish_id;
  switch_scene->fish_kind[switch_scene->fish_count] = fish_trace_info->fish_kind;
  switch_scene->fish_count += 1;

  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SWITCH_SCENE, tcp_buffer, sizeof(CMD_S_SwitchScene));
}

void CTableFrameSink::ClearFishTrace(bool force)
{
	if (force)
	{
		std::copy(active_fish_trace_vector_.begin(), active_fish_trace_vector_.end(), std::back_inserter(storage_fish_trace_vector_));
		active_fish_trace_vector_.clear();
	}
	else
	{
		FishTraceInfoVecor::iterator iter;
		FishTraceInfo* fish_trace_info = NULL;
		DWORD now_tick = GetTickCount();
		for (iter = active_fish_trace_vector_.begin(); iter != active_fish_trace_vector_.end();)
		{
			fish_trace_info = *iter;
			if (now_tick >= (fish_trace_info->build_tick + kFishAliveTime))
			{
				iter = active_fish_trace_vector_.erase(iter);
				storage_fish_trace_vector_.push_back(fish_trace_info);
			}
			else
			{
				++iter;
			}
		}
	}
}

void CTableFrameSink::CalcScore(IServerUserItem* server_user_item) {
  if (server_user_item == NULL) return;
  WORD chair_id = server_user_item->GetChairID();

  tagScoreInfo score_info;
  memset(&score_info, 0, sizeof(score_info));
  score_info.cbType = SCORE_TYPE_WIN;
  score_info.lScore = (fish_score_[chair_id] - exchange_fish_score_[chair_id]) * g_exchange_ratio_userscore_ / g_exchange_ratio_fishscore_;

  //CString str2;
  //str2.Format(_T("%ld,%ld,%ld,%ld,%d"),fish_score_[chair_id],exchange_fish_score_[chair_id],fish_score_[chair_id] - exchange_fish_score_[chair_id],score_info.lScore,exchange_ratio_userscore_ / exchange_ratio_fishscore_);
  //AfxMessageBox(str2);
  m_pITableFrame->WriteUserScore(chair_id, score_info/*wexperience[chair_id]/100*60*exchange_ratio_userscore_ / exchange_ratio_fishscore_*/);
  //CString KK;
  //KK.Format(TEXT("%f"),exchange_ratio_userscore_*1.0 / (exchange_ratio_fishscore_*1.0));
  //OutputDebugString(KK);

  experience[chair_id]=0;
  fish_score_[chair_id] = 0;
  exchange_fish_score_[chair_id] = 0;
}


void  CTableFrameSink::OnSubClientGetControl(IServerUserItem* server_user_item)
{
	CMD_S_CONTROLCFG stControlCfg;
	stControlCfg.total_return_rate_ = g_total_return_rate_;						//返还百分比
	stControlCfg.revenue_score = g_revenue_score;								//游戏抽水 
	stControlCfg.zhengtigailv = g_total_probability_;								//整体概率
	stControlCfg.stock_score0 = g_stock_score_[0];						//库存配置
	stControlCfg.stock_score1 = g_stock_score_[1];
	stControlCfg.hard = S_hard;					// 困难
	stControlCfg.easy = S_easy;					//简单
	 
	m_pITableFrame->SendUserItemData(server_user_item, SUB_S_ControlCfg, &stControlCfg, sizeof(stControlCfg));
}
void	CTableFrameSink::OnSubClientSetControlCfg(double					 total_return_rate_,					//返还百分比
	double					 revenue_score,								//游戏抽水 
	double                   zhengtigailv,								//整体概率
	double 				 stock_score0,						//库存配置
	double 				 stock_score1,						//库存配置
	double                 hard,					// 困难
	double                 easy)				//简单)
{

	g_total_return_rate_ =  total_return_rate_;
	g_revenue_score = revenue_score;
	g_stock_score_[0] =  stock_score0;
	g_stock_score_[1] = stock_score1;


	g_total_probability_ = zhengtigailv;

	S_easy = easy;
	S_hard = hard;

}