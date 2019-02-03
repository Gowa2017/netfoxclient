#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include <vector>
#include <map>

struct FishTraceInfo 
{
  FishKind fish_kind;
  int fish_id;
  DWORD build_tick;
};
typedef std::vector<FishTraceInfo*> FishTraceInfoVecor;

struct SweepFishInfo 
{
  FishKind fish_kind;
  int fish_id;
  BulletKind bullet_kind;
  float bullet_mulriple;
};

struct ServerBulletInfo 
{
  BulletKind bullet_kind;
  int bullet_id;
  float bullet_mulriple;
};

struct CHEATRECORD
{
	int iDeltaTime;
	unsigned int iCheatCount;
};

//////////////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//组件接口
private:

	std::map<DWORD, CHEATRECORD> fire_time_test_map_;//markbq0828
	FishTraceInfoVecor active_fish_trace_vector_;
	FishTraceInfoVecor storage_fish_trace_vector_;

	std::vector<SweepFishInfo> sweep_fish_info_vector_;

	typedef std::vector<ServerBulletInfo*> ServerBulletInfoVector;
	ServerBulletInfoVector server_bullet_info_vector_[GAME_PLAYER];
	ServerBulletInfoVector storage_bullet_info_vector_;

	int fish_id_;
	SCORE exchange_fish_score_[GAME_PLAYER];
	SCORE fish_score_[GAME_PLAYER];
	SCORE experience[GAME_PLAYER];
	bool bullet_ion_[GAME_PLAYER];
	int catch_fish_count[GAME_PLAYER];
	int bullet_id_[GAME_PLAYER];
	int bullet_count[GAME_PLAYER];
	SceneKind next_scene_kind_;
	bool special_scene_;
	int current_fish_lk_multiple_;
	int timercount[GAME_PLAYER];
	WORD android_chairid_;
	DWORD m_dwLastTickTime;
	int table_android_count;
	//组件接口
protected:
	ITableFrame	*					m_pITableFrame;						//框架接口
	tagGameServiceOption *			m_pGameServiceOption;				//游戏配置
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//游戏属性
	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//管理接口
public:
	//复位桌子
	virtual VOID RepositionSink();
	//配置桌子
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){ return 0; }
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) { return 0; }
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){ return true; }

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//时间事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){	return false;	}
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){ return false; }

	//网络接口
public:
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){};

  //用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//用户下注
	virtual bool OnActionUserPour(WORD wChairID, IServerUserItem * pIServerUserItem){ return false; }
private:
	FishTraceInfo* ActiveFishTrace();

	bool FreeFishTrace(FishTraceInfo* fish_trace_info);

	void FreeAllFishTrace();

	FishTraceInfo* GetFishTraceInfo(int fish_id);

	void SaveSweepFish(FishKind fish_kind, int fish_id, BulletKind bullet_kind, float bullet_mulriple);

	bool FreeSweepFish(int fish_id);

	SweepFishInfo* GetSweepFish(int fish_id);

	ServerBulletInfo* ActiveBulletInfo(WORD chairid);

	bool FreeBulletInfo(WORD chairid, ServerBulletInfo* bullet_info);

	void FreeAllBulletInfo(WORD chairid);

	ServerBulletInfo* GetBulletInfo(WORD chairid, int bullet_id);
   public:
	   //设置基数
	   virtual void SetGameBaseScore(SCORE lBaseScore){};

	   void OnUpdatge(float dt);
 private:
	 void StartAllGameTimer();
	 void KillAllGameTimer();
	 bool LoadConfig();
	 bool SendGameConfig(IServerUserItem* server_user_item);
	 int GetNewFishID();
	 int GetBulletID(WORD chairid);

	 bool OnTimerBuildSmallFishTrace(WPARAM bind_param);
	 bool OnTimerBuildMediumFishTrace(WPARAM bind_param);
	 bool OnTimerBuild41FishTrace(WPARAM bind_param);
	 bool OnTimerBuildFish17Trace(WPARAM bind_param);
	 bool OnTimerBuildFish18Trace(WPARAM bind_param);
	 bool OnTimerBuildFish19Trace(WPARAM bind_param);
	 bool OnTimerBuildFish20Trace(WPARAM bind_param);
	 bool OnTimerBuildFishLKTrace(WPARAM bind_param);
	 bool OnTimerBuildFishBombTrace(WPARAM bind_param);
	 bool OnTimerBuildFishSuperBombTrace(WPARAM bind_param);
	 bool OnTimerBuildFishLockBombTrace(WPARAM bind_param);
	 bool OnTimerBuildFishSanTrace(WPARAM bind_param);
	 bool OnTimerBuildFishSiTrace(WPARAM bind_param);
	 bool OnTimerBuildFishKingTrace(WPARAM bind_param);
	 bool OnTimerClearTrace(WPARAM bind_param);
	 bool OnTimertichu(WPARAM bind_parm);
	 bool OnTimerBaocun(WPARAM bind_parm);
	 bool OnTimerBulletIonTimeout(WPARAM bind_param);
	 bool OnTimerLockTimeout(WPARAM bind_param);
	 bool OnTimerSwitchScene(WPARAM bind_param);
	 bool OnTimerSceneEnd(WPARAM bind_param);
	 bool OnTimerLKScore(WPARAM bind_param);
	 bool OnTimerFangShui(WPARAM bind_param);
	 bool OnTimerjilu(WPARAM bind_param);
	 bool OnTimerbodong(WPARAM bind_param);
	
	 bool SendTableData(WORD sub_cmdid, void* data, WORD data_size, IServerUserItem* exclude_user_item);
	 
	 bool qujianjisuan(qujian);
	 bool weizhitiaozheng(double [8]);
	 bool heibaigailvtiaozheng(double[2]);
	 bool OnSubExchangeFishScore(IServerUserItem* server_user_item, bool increase);
	 bool OnSubUserFire(IServerUserItem* server_user_item, int bullet_id, BulletKind bullet_kind, float angle, float bullet_mul, int lock_fishid, float fire_speed,bool bBadManCheat = false);
	 bool OnSubCatchFish(IServerUserItem* server_user_item, int fish_id, BulletKind bullet_kind, int bullet_id, float bullet_mul);
	 bool OnSubCatchSweepFish(IServerUserItem* server_user_item, int fish_id, int* catch_fish_id, int catch_fish_count);
	 bool OnSubHitFishLK(IServerUserItem* server_user_item, int fish_id);
	 bool onsubipfasong(IServerUserItem* server_user_item);
	 bool onSubtianjia(IServerUserItem* server_user_item, tianjiashanchu ip);
	 bool onsubshanchu(IServerUserItem* server_user_item, DWORD ip);
	 bool OnSubUserFilter(IServerUserItem* server_user_item, DWORD game_id, unsigned char operate_code);
	 bool OnSubFish20Config(IServerUserItem* server_user_item, DWORD game_id, int catch_count, double catch_probability);
	 bool OnSubFish21Config(IServerUserItem* server_user_item, DWORD game_id, int catch_count, double catch_probability);
	 bool OnSubFishJinShaConfig(IServerUserItem* server_user_item, DWORD game_id, int catch_count, double catch_probability);
	 bool OnSubScanFilter(IServerUserItem* server_user_item);
	 bool OnSubbaojingjilu(IServerUserItem* server_user_item);
	 bool OnSubZKCxiugai(IServerUserItem* server_user_item,CMD_S_CONFIG);
	 bool OnSubzongkucun(IServerUserItem* server_user_item);

	 bool OnSubshanchujilu(IServerUserItem* server_user_item, int ZZ);
	 bool OnSubScanFilter2(IServerUserItem* server_user_item);
	 bool Onsubtiaozhengpao(IServerUserItem* , pao10086);
	 bool Onsubtichu(IServerUserItem*,DWORD);
	 bool OnSubtiaozheng(winlose);
	 void BuildInitTrace(FPoint init_pos[5], int init_count, FishKind fish_kind, TraceType trace_type);
	 void BuildFishTrace(int fish_count, FishKind fish_kind_start, FishKind fish_kind_end);
	 void BuildSceneKind1();
	 void BuildSceneKind2();
	 void BuildSceneKind3();
	 void BuildSceneKind4();
	 void BuildSceneKind5();
	 void ClearFishTrace(bool force = false);

	 void CalcScore(IServerUserItem* server_user_item);
	
	 void  OnSubClientGetControl(IServerUserItem* server_user_item);
	 void	OnSubClientSetControlCfg(double					 total_return_rate_,					//返还百分比
		 double					 revenue_score,								//游戏抽水 
		 double                   zhengtigailv,								//整体概率
		 double 				 stock_score0,						//库存配置
		 double 				 stock_score1,						//库存配置
		 double                 hard,					// 困难
		 double                 easy);					//简单)
};

#endif // TABLE_FRAME_SINK_H_
