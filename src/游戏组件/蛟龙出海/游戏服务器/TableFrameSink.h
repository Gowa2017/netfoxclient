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

//��Ϸ����
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//����ӿ�
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
	//����ӿ�
protected:
	ITableFrame	*					m_pITableFrame;						//��ܽӿ�
	tagGameServiceOption *			m_pGameServiceOption;				//��Ϸ����
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��Ϸ����
	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��λ����
	virtual VOID RepositionSink();
	//��������
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){ return 0; }
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) { return 0; }
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){ return true; }

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){	return false;	}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){ return false; }

	//����ӿ�
public:
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};

  //�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem){ return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//�û���ע
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
	   //���û���
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
	 void	OnSubClientSetControlCfg(double					 total_return_rate_,					//�����ٷֱ�
		 double					 revenue_score,								//��Ϸ��ˮ 
		 double                   zhengtigailv,								//�������
		 double 				 stock_score0,						//�������
		 double 				 stock_score1,						//�������
		 double                 hard,					// ����
		 double                 easy);					//��)
};

#endif // TABLE_FRAME_SINK_H_
