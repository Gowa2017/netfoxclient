#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE
#pragma pack(1)
//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID           140                 //游戏 I D
#define GAME_PLAYER         MAX_CHAIR                 //游戏人数
#define GAME_NAME         TEXT("豪车俱乐部")            //游戏名字

#define TEST_OUT(num)   CString str;str.Format(L"this is:%d",num);AfxMessageBox(str);
//版本信息
#define VERSION_SERVER          PROCESS_VERSION(7,0,1)        //程序版本
#define VERSION_CLIENT        PROCESS_VERSION(7,0,1)        //程序版本

//状态定义
#define GS_PLACE_JETTON       GAME_STATUS_PLAY          //下注状态
#define GS_GAME_END         GAME_STATUS_PLAY+1            //结束状态
#define GS_MOVECARD_END       GAME_STATUS_PLAY+2            //结束状态

//区域索引
#define ID_TIAN_MEN         1                 //顺门
#define ID_DI_MEN         2                 //左边角
#define ID_XUAN_MEN         3                 //桥
#define ID_HUANG_MEN        4                 //对门

//玩家索引
#define BANKER_INDEX        0                 //庄家索引
#define SHUN_MEN_INDEX        1                 //顺门索引
#define DUI_MEN_INDEX       2                 //对门索引
#define DAO_MEN_INDEX       3                 //倒门索引
#define HUAN_MEN_INDEX        4                 //倒门索引

//库存控制
#define RQ_REFRESH_STORAGE    13
#define RQ_SET_STORAGE      14
#define AREA_COUNT          8                 //区域数目

//赔率定义
#define RATE_TWO_PAIR       12                  //对子赔率

#define SUB_S_UPDATE_STORAGE        111                 //更新库存
//下注信息
struct tagUserBet
{
  TCHAR             szNickName[32];           //用户昵称
  DWORD             dwUserGameID;           //用户ID
  LONGLONG            lUserScore;             //用户金币
  LONGLONG            lUserBet[AREA_COUNT+1];       //用户下注
  LONGLONG            lUserScoreTotal;              //用户金币
};
//下注信息数组
typedef CWHArray<tagUserBet,tagUserBet&> CUserBetArray;
struct tagCustomAndroid
{
  //坐庄
  BOOL              nEnableRobotBanker;       //是否做庄
  LONGLONG            lRobotBankerCount;        //坐庄次数
  LONGLONG            lRobotListMinCount;       //列表人数
  LONGLONG            lRobotListMaxCount;       //列表人数
  LONGLONG            lRobotApplyBanker;        //最多申请个数
  LONGLONG            lRobotWaitBanker;       //空盘重申

  //下注
  LONGLONG            lRobotMinBetTime;       //下注筹码个数
  LONGLONG            lRobotMaxBetTime;       //下注筹码个数
  LONGLONG            lRobotMinJetton;        //下注筹码金额
  LONGLONG            lRobotMaxJetton;        //下注筹码金额
  LONGLONG            lRobotBetMinCount;        //下注机器人数
  LONGLONG            lRobotBetMaxCount;        //下注机器人数
  LONGLONG            lRobotAreaLimit;        //区域限制

  //存取款
  LONGLONG            lRobotGetCondition;       //机器人取款条件，机器人的分数小于该值时执行取款
  LONGLONG            lRobotSaveCondition;      //机器人存款条件
  LONGLONG            lRobotGetMin;         //取款最小值
  LONGLONG            lRobotGetMax;         //取款最大值
  LONGLONG            lRobotBankStoMul;       //存款百分比

  //构造函数
  tagCustomAndroid()
  {
    DefaultCustomRule();
  }

  void DefaultCustomRule()
  {
    nEnableRobotBanker = TRUE;
    lRobotBankerCount = 5;
    lRobotListMinCount = 2;
    lRobotListMaxCount = 5;
    lRobotApplyBanker = 5;
    lRobotWaitBanker = 3;

    lRobotMinBetTime = 6;
    lRobotMaxBetTime = 8;
    lRobotMinJetton = 100;
    lRobotMaxJetton = 5000000;
    lRobotBetMinCount = 4;
    lRobotBetMaxCount = 8;
    lRobotAreaLimit = 10000000;


    lRobotGetCondition = 10000;
    lRobotSaveCondition = 30000000;
    lRobotGetMin = 100000;
    lRobotGetMax = 500000;
    lRobotBankStoMul = 50;
  }
};
//机器人信息
struct tagRobotInfo
{
  int nChip[6];                           //筹码定义
  int nAreaChance[AREA_COUNT];                    //区域几率
  TCHAR szCfgFileName[MAX_PATH];                    //配置文件
  int nMaxTime;                           //最大赔率

  tagRobotInfo()
  {
    int nTmpChip[6] = {100,1000,10000,100000,1000000,5000000};
    int nTmpAreaChance[AREA_COUNT] = {1, 2, 2, 5, 10, 10, 10, 10};
    TCHAR szTmpCfgFileName[MAX_PATH] = _T("BumperCarBattle.ini");

    nMaxTime = 10;
    memcpy(nChip, nTmpChip, sizeof(nChip));
    memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
    memcpy(szCfgFileName, szTmpCfgFileName, sizeof(szCfgFileName));
  }
};

//记录信息
struct tagServerGameRecord
{
  BYTE              bWinMen[AREA_COUNT+1];            //顺门胜利
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_FREE       99                  //游戏空闲
#define SUB_S_GAME_START      100                 //游戏开始
#define SUB_S_PLACE_JETTON      101                 //用户下注
#define SUB_S_GAME_END        102                 //游戏结束
#define SUB_S_APPLY_BANKER      103                 //申请庄家
#define SUB_S_CHANGE_BANKER     104                 //切换庄家
#define SUB_S_CHANGE_USER_SCORE   105                 //更新积分
#define SUB_S_SEND_RECORD     106                 //游戏记录
#define SUB_S_PLACE_JETTON_FAIL   107                 //下注失败
#define SUB_S_CANCEL_BANKER     108                 //取消申请
#define SUB_S_CHECK_IMAGE     109                 //取消申请
#define SUB_S_ADMIN_COMMDN      110                 //系统控制


//失败结构
struct CMD_S_PlaceJettonFail
{
  WORD              wPlaceUser;             //下注玩家
  BYTE              lJettonArea;            //下注区域
  LONGLONG            lPlaceScore;            //当前下注
};

//更新积分
struct CMD_S_ChangeUserScore
{
  WORD              wChairID;             //椅子号码
  DOUBLE              lScore;               //玩家积分

  //庄家信息
  WORD              wCurrentBankerChairID;        //当前庄家
  BYTE              cbBankerTime;           //庄家局数
  DOUBLE              lCurrentBankerScore;        //庄家分数
};

//申请庄家
struct CMD_S_ApplyBanker
{
  WORD              wApplyUser;             //申请玩家
};

//取消申请
struct CMD_S_CancelBanker
{
  WORD              wCancelUser;          //取消玩家
};

//切换庄家
struct CMD_S_ChangeBanker
{
  WORD              wBankerUser;            //当庄玩家
  LONGLONG            lBankerScore;           //庄家金币
};

//游戏状态
struct CMD_S_StatusFree
{
  //全局信息
  BYTE              cbTimeLeave;            //剩余时间

  //玩家信息
  LONGLONG            lUserMaxScore;              //玩家金币

  //庄家信息
  WORD              wBankerUser;            //当前庄家
  WORD              cbBankerTime;           //庄家局数
  LONGLONG            lBankerWinScore;          //庄家成绩
  LONGLONG            lBankerScore;           //庄家分数
  bool              bEnableSysBanker;         //系统做庄

  //控制信息
  LONGLONG            lApplyBankerCondition;        //申请条件
  LONGLONG            lAreaLimitScore;          //区域限制

  TCHAR             szGameRoomName[32];     //房间名称
  TCHAR				szRoomTotalName[256];
  int				nMultiple;
  LONGLONG            lStorageStart;
  int               CheckImage;
  tagCustomAndroid        CustomAndroidConfig;        //机器人配置
};

//游戏状态
struct CMD_S_StatusPlay
{
  //全局下注
  LONGLONG            lAllJettonScore[AREA_COUNT+1];    //全体总注

  //玩家下注
  LONGLONG            lUserJettonScore[AREA_COUNT+1];   //个人总注

  //玩家积分
  LONGLONG            lUserMaxScore;            //最大下注

  //控制信息
  LONGLONG            lApplyBankerCondition;        //申请条件
  LONGLONG            lAreaLimitScore;          //区域限制

  //扑克信息
  BYTE              cbTableCardArray[1][1];       //桌面扑克

  //庄家信息
  WORD              wBankerUser;            //当前庄家
  WORD              cbBankerTime;           //庄家局数
  LONGLONG            lBankerWinScore;          //庄家赢分
  LONGLONG            lBankerScore;           //庄家分数
  bool              bEnableSysBanker;         //系统做庄

  //结束信息
  LONGLONG            lEndBankerScore;          //庄家成绩
  LONGLONG            lEndUserScore;            //玩家成绩
  LONGLONG            lEndUserReturnScore;        //返回积分
  LONGLONG            lEndRevenue;            //游戏税收
  LONGLONG           lStorageStart;
  //全局信息
  BYTE              cbTimeLeave;            //剩余时间
  BYTE              cbGameStatus;           //游戏状态
  int               CheckImage;
  TCHAR             szGameRoomName[32];     //房间名称
    TCHAR				szRoomTotalName[256];
  int               nMultiple;
  tagCustomAndroid        CustomAndroidConfig;        //机器人配置
}; 

//游戏空闲
struct CMD_S_GameFree
{
  BYTE              cbTimeLeave;            //剩余时间
  INT64                             nListUserCount;           //列表人数
  LONGLONG            lStorageStart;
};

//游戏开始
struct CMD_S_GameStart
{
  WORD              wBankerUser;            //庄家位置
  LONGLONG            lBankerScore;           //庄家金币
  LONGLONG            lUserMaxScore;            //我的金币
  BYTE              cbTimeLeave;            //剩余时间
  bool              bContiueCard;           //继续发牌
  int               nChipRobotCount;          //人数上限 (下注机器人)
  int               nAndriodCount;          //人数上限 (下注机器人)
};

//用户下注
struct CMD_S_PlaceJetton
{
  WORD              wChairID;             //用户位置
  BYTE              cbJettonArea;           //筹码区域
  LONGLONG            lJettonScore;           //加注数目
  BYTE              cbAndroid;              //机器人
  bool              bAndroid;             //
};

//游戏结束
struct CMD_S_GameEnd
{
  LONGLONG            lStorageStart;
  //下局信息
  BYTE              cbTimeLeave;            //剩余时间

  //扑克信息
  BYTE              cbTableCardArray[1][1];       //桌面扑克
  BYTE              cbLeftCardCount;          //扑克数目

  BYTE              bcFirstCard;

  //庄家信息
  LONGLONG            lBankerScore;           //庄家成绩
  LONGLONG            lBankerTotallScore;         //庄家成绩
  INT               nBankerTime;            //做庄次数

  //玩家成绩
  LONGLONG            lUserScore;             //玩家成绩
  LONGLONG            lUserReturnScore;         //返回积分
  LONGLONG                        lUserScoreTotal[GAME_PLAYER];//每个选手房间的总赢输
  //全局信息
  LONGLONG            lRevenue;             //游戏税收
  LONGLONG            storageStart;                    //游戏库存
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_JETTON      1                 //用户下注
#define SUB_C_APPLY_BANKER      2                 //申请庄家
#define SUB_C_CANCEL_BANKER     3                 //取消申请
#define SUB_C_CONTINUE_CARD     4                 //继续发牌
#define SUB_C_CHECK_IMAGE     5                 //继续发牌
#define SUB_C_ADMIN_COMMDN      6                 //系统控制
#define SUB_C_UPDATE_STORAGE        7                 //更新库存
#define SUB_C_DEUCT         8                 //更新库存

//用户下注
struct CMD_C_PlaceJetton
{
  BYTE              cbJettonArea;           //筹码区域
  LONGLONG            lJettonScore;           //加注数目
};

struct CMD_C_CheckImage
{
  int Index;
};

//////////////////////////////////////////////////////////////////////////

#define IDM_ADMIN_COMMDN WM_USER+1000
#define IDM_ADMIN_STORAGE     WM_USER+1001
#define IDM_ADMIN_DEUCT       WM_USER+1002

//控制区域信息
struct tagControlInfo
{
  BYTE cbControlArea;           //控制区域
};

struct CMD_C_FreshStorage
{
  BYTE                            cbReqType;            //请求类型
  LONGLONG            lStorageStart;        //起始库存
  LONGLONG            lStorageDeduct;       //库存衰减
  LONGLONG            lStorageCurrent;        //当前库存
  LONGLONG            lStorageMax1;         //库存上限1
  LONGLONG            lStorageMul1;         //系统输分概率1
  LONGLONG            lStorageMax2;         //库存上限2
  LONGLONG            lStorageMul2;         //系统输分概率2
};

struct CMD_C_FreshDeuct
{
  LONGLONG            lStorageDeuct;            //库存衰减数值

};

//服务器控制返回
#define  S_CR_FAILURE       0   //失败
#define  S_CR_UPDATE_SUCCES     1   //更新成功
#define  S_CR_SET_SUCCESS     2   //设置成功
#define  S_CR_CANCEL_SUCCESS    3   //取消成功
struct CMD_S_ControlReturns
{
  BYTE cbReturnsType;       //回复类型
  BYTE cbControlArea;       //控制区域
  BYTE cbControlTimes;      //控制次数
};


//客户端控制申请
#define  C_CA_UPDATE        1   //更新
#define  C_CA_SET         2   //设置
#define  C_CA_CANCELS       3   //取消
struct CMD_C_ControlApplication
{
  BYTE cbControlAppType;      //申请类型
  BYTE cbControlArea;       //控制区域
  BYTE cbControlTimes;      //控制次数
};
struct CMD_C_AdminReq
{
  BYTE cbReqType;
#define RQ_SET_WIN_AREA 1
#define RQ_RESET_CONTROL  2
#define RQ_PRINT_SYN    3
  BYTE cbExtendData[20];      //附加数据
};
//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
