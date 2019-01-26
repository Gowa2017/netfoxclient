
#ifndef CMD_FISH_H_
#define CMD_FISH_H_
#include<vector>
#include<string>
using std::string;
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
// ������

#define KIND_ID               408  //301
#define GAME_NAME             TEXT("�������")
#define VERSION_SERVER        16777217        /*PROCESS_VERSION(1,0,0)*/
#define VERSION_CLIENT        16777217        /*PROCESS_VERSION(1,0,0)*/

// �򵥵İ汾���
// ����ͻ����и���Ӧ�ø������ֵ.���ֻ�޸�EXE�İ汾���������ݰ�û�޸ĵĻ������¿ͻ�����ûʲô����ģ���ΪEXE�İ汾�ǿ���ֱ�ӱ��޸ĵ�
// ֻҪ������ǰ��EXE�汾,������֮���ٻ���ȥ�������BUG�Ļ����ڻ���û�޸���
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
// ��Ϸ����

/*
// ��λ��
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
  FISH_KIND_18, //����
  FISH_KIND_19, //����
  FISH_KIND_20, // ���
  FISH_KIND_21, // ����
  FISH_KIND_22, // ����ը��
  FISH_KIND_23, // �ֲ�ը��
  FISH_KIND_24, // ����ը��
  FISH_KIND_25, // ����Ԫ1   ������һ��
  FISH_KIND_26, // ����Ԫ2   4����һ��
  FISH_KIND_27, // ����Ԫ3
  FISH_KIND_28, // ����ϲ1
  FISH_KIND_29, // ����ϲ2
  FISH_KIND_30, // ����ϲ3
  FISH_KIND_31, // ����1
  FISH_KIND_32, // ����2
  FISH_KIND_33, // ����3
  FISH_KIND_34, // ����4
  FISH_KIND_35, // ����5
  FISH_KIND_36, // ����6
  FISH_KIND_37, // ����7
  FISH_KIND_38, // ����8
  FISH_KIND_39, // ����9
  FISH_KIND_40, // ����10
  FISH_KIND_41,//����
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
// ���ƿͻ��˵�����������
const int kFishKindCount[FISH_KIND_COUNT] = { 10,10,8,8,7,6,6,6,6,6,4,4,4,3,3,3,2,1,1,1,1,1,1,1,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1  };
//���ǵ����ӵ����ٵ�
static const DWORD kFireInterval = 330;
//��¼
static std::vector<CString>jilu;
//////////////////////////////////////////////////////////////////////////
// ���������

#define SUB_S_GAME_CONFIG                   100  //��Ϸ����
#define SUB_S_FISH_TRACE                    101  //��Ĺ켣
#define SUB_S_EXCHANGE_FISHSCORE            102   //���·�
#define SUB_S_USER_FIRE                     103  //����
#define SUB_S_FISH_MISSED					104   //�㲻����
#define SUB_S_CATCH_FISH                    105  //ץ����
#define SUB_S_BULLET_ION_TIMEOUT            106  //�ӵ�����ʱ�䵽��
#define SUB_S_LOCK_TIMEOUT                  107  //����ʱ�䵽
#define SUB_S_CATCH_SWEEP_FISH              108 //ץ��BOSS��ը��ʱ
#define SUB_S_CATCH_SWEEP_FISH_RESULT       109   //ץ��BOSOS��ը���Ľ��
#define SUB_S_HIT_FISH_LK                   110    //�˺�������
#define SUB_S_SWITCH_SCENE                  111   //�ı䳡��
#define SUB_S_SCENE_END                     112       //��������
#define SUB_S_HEIBAI_LIST					113		//�ڰ�����
#define SUB_S_FISHTESHU_LIST				114		//����������
#define SUB_S_ADMIN_CONFIG					115     //����ϵͳ
#define SUB_S_ADMIN_FANGSHUI				116         //��ˮϵͳ
#define SUB_chuansong                        117   //��¼����
#define SUB_iphao                          118   //IP��
#define SUB_iphao1                    119  //���ɾ����IP����
#define sub_iptianjiashanchu           120   //ip���ɾ��
#define SUB_fanhuiiDhao              121   //����ID��
#define SUB_gengxin1               122   //����

#define SUB_Smorenbaifenbi       125 //��ӮĬ�ϰٷֱ�
#define SUB_baojing         126  //������¼
#define SUB_zongKC        127 //�ܿ��
#define SUB_S_NoFire  128 //��ǹ
#define SUB_timerUp 129 //ʱ�䵽��
#define SUB_S_canfire 130  //�����ӵ�����
#define SUB_S_zongfen 131 //��ǰ�����ܷ� ��Ҫ����Ϊ��Щ�˰ѷ�������д��
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
{//�û���Ҷһ�����
	int exchange_ratio_userscore;
	//�û���Ҷһ�����
	int exchange_ratio_fishscore;
	//���ζһ��������
	int exchange_count;
	//�ӵ���С���ֵ
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
	FPoint init_pos[5]; // ��������ֱ�ߵĵ�
	int init_count; // ���������� 3�� ֱ�߾��� 2��
	BYTE cmd_version; //�汾��
	FishKind fish_kind; // �������
	int fish_id;  //��� ID
	TraceType trace_type;
};

struct CMD_S_ExchangeFishScore
{
	WORD chair_id;
	//ÿ�����·ֵ�Ǯ
	SCORE swap_fish_score;
	//�ı��ܶ���˵Ļ���Ϊ����Ӯ�˵Ļ���Ϊ��
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
	//�����ӵ�
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

//����װ���� ������
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
	double					 total_return_rate_;						//�����ٷֱ�
	double 					 revenue_score;								//��Ϸ��ˮ 
	double                   zhengtigailv;								//�������
	double 				 stock_score0;						//�������
	double 				 stock_score1;
	double                 hard;					// ����
	double                 easy;					//��
};

//����������
struct CMD_S_ADMIN_CONFIG
{
	//
	double					 total_return_rate_;						//�����ٷֱ�
	SCORE					 revenue_score;								//��Ϸ��ˮ
	int						 exchange_ratio_userscore_;					//��Ҷһ�����
	int						 exchange_ratio_fishscore_;					//��Ҷһ�����
	int						 exchange_fishscore_count_;					//���ζһ�����
	double                       zhengtigailv;								//�������

	float						 bullet_multiple[2];
	//�������
	LONGLONG				 stock_score[2];
	SCORE						 stock_crucial_score_[12];
	SCORE						 stock_crucial_score_small[12];
	double					 stock_increase_probability_[12];
	double					 stock_increase_probability_small[12];
	//��������
	int						 nFishMinMultiple[FISH_KIND_COUNT];
	int						 nFishMaxMultiple[FISH_KIND_COUNT];
	int						 nFishSpeed[FISH_KIND_COUNT];
	double					 fFishGailv[FISH_KIND_COUNT];
	int						 win_score_limit_[20];					//��Ӯ��λ����
	double					 win_decrease_probability_[20];			//��Ӯ��λ����
	int                    bodongshijian;         //����ʱ��
	double                 hard;					// ����
	double                 easy;					//��
	int                   bodongmax;           //�������ֵ
	double                paogailv[10];    
	double				 zuoweigailv[8];
	DWORD  qijiren[2];
	double qijirengailv;
	DWORD  jiqirenleave[2];
};
//��
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


//��ˮϵͳ
struct CMD_S_ADMIN_FANGSHUI
{
	//��ˮ����
	bool					 bFangShuiStart;						//��ˮ״̬
	SCORE					 stock_fangshui_edu;					//��ˮ���
	double					 stock_fangshui_probability;			//��ˮ����
	DWORD					 fangshui_time;							//����ʱ��
	SCORE					 fangshui_score;						//�ѷ�ˮ���
	DWORD                    fangshui_Begin;                    //��ˮ��ʼʱ��
	DWORD					 fangshui_count_time;					//�ѳ�����ˮ����ʱ��
	SCORE					 lastfangshui_score;					//�ϴ��ѷ�ˮ���
	DWORD					 lastfangshui_count_time;				//�ϴ��ѳ�����ˮ����ʱ��
	DWORD					 nextfangshui_time;						//�´η�ˮ��ʼʱ��
	SCORE					 fangshuixiaoyukucun;                //��ˮС����
	SCORE					 fangshuidayukucun;						//��ˮ������
	SCORE                   meitian_edu;                         //ÿ����
	SCORE					 fangshuixiaoyukucun1 ;              //��¼��ˮ���
	SCORE					 fangshuidayukucun1;						//
	DWORD                    youxiid1[2];               //��������
	/*WORD                     yizihao1[2];*/         //���Ӻ�
	DWORD                     zaixianwanjia;    //�����������
};

//��ˮϵͳ
struct CMD_S_UPDATE_ANDROID
{
   int fishCount;
};
//////////////////////////////////////////////////////////////////////////
// �ͻ�������

#define SUB_C_EXCHANGE_FISHSCORE            101
#define SUB_C_USER_FIRE                     102
#define SUB_C_CATCH_FISH                    103
#define SUB_C_CATCH_SWEEP_FISH              104
#define SUB_C_HIT_FISH_I                    105 //�˺�LK
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
  int bullet_id;	//�ӵ� ID
  BulletKind bullet_kind; //�ӵ�����
  BYTE cmd_version; //�汾��
  float angle;	// �ӵ��Ƕ�
  float bullet_mulriple; //�ӵ����� 1000~1��
  int lock_fishid; //�������ID��
  DWORD fire_time; //����ʱ��
  float fire_speed; //�����ٶ�
};
//������Ⱥ
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
  unsigned char operate_code; // 0 ������ 1 ������ 2 ���
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