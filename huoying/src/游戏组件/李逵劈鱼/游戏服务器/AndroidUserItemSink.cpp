#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
#include<windows.h>

//////////////////////////////////////////////////////////////////////////

// 定时器标识
#define IDI_SWITCH_SCENE				  1									// 切换场景不能打炮
#define IDI_EXCHANGE_FISHSCORE    2                 // 交换子弹
#define IDI_FIRE                  3
#define IDI_STAND_UP              4
#define IDI_FIRE_MODE	          5
#define IDI_SWITCH_SCENE_BEING	  6					// 切换场景机器人打炮模式开始
#define IDI_STAND_UP1            38
#define IDI_leave_time           39
#define IDI_SWITCH_SCENE_END	  7					// 切换场景机器人打炮模式还原
#define IDI_SWITCH_SCENE_ANDROID_STOP	  8			// 切换场景机器人停止发射
#define IDI_SWITCH_SCENE_ANDROID_START	  9			// 切换场景机器人开始发射
#define IDI_SWITCH_SCENE_ANDROID_PAUSE	  11		// 切换场景机器人随机暂停

#define IDI_ANDROID_SWITCH_LOCK			  12		// 机器人换鱼儿打
#define IDI_ANDROID_STOP_SWITCH_LOCK	  13		// 机器人停止换鱼儿打
#define IDI_CURRENT_FIRE		  35
#define IDI_NEXT_FIRE             10

#define IDI_CHECK_BANKER_OPERATE		 14			//机器人定时银行操作

// 时间标识
#define TIME_SWITCH_SCENE				  7									// 切换场景不能打炮 大概7秒钟 客户端有修改 这个应该也要修改
#define TIME_EXCHANGE_FISHSCORE   2
#define TIME_FIRE                 2											// 这里不能改成1 会造成死循环, 如果想要加快子弹发射速度只能改平台.
#define TIME_SWITCH_SCENE_BEING    5									// 场景机器人瞄准时间
#define TIME_SWITCH_SCENE_PAUSE    1									// 场景机器人暂停时间
#define TIME_ANDROID_SWITCH_LOCK   1									//机器人换鱼打
#define TIME_ANDROID_STOP_SWITCH_LOCK   1									//机器人换鱼打



#define TIME_SWITCH_SCENE_END       15									// 场景机器人瞄准时间
#define TIME_SWITCH_SCENE_ANDROID_STOP     60									// 场景机器人停止发射
#define TIME_SWITCH_SCENE_ANDROID_START     17									// 场景机器人停止发射




//////////////////////////////////////////////////////////////////////////

//DWORD   dwThreadId1=0; 
//HANDLE  hThread1=NULL;
//DWORD   dwThreadId2=0; 
//HANDLE  hThread2=NULL;
DWORD CALLBACK DoThread2(PVOID pvoid) ;
DWORD CALLBACK DoThread(PVOID pvoid) ;
DWORD CALLBACK DoThreadExchangFishScore(PVOID pvoid);
DWORD CALLBACK DoThreadExchangFishScore2(PVOID pvoid);
DWORD CALLBACK DoThreadExchangFishScore3(PVOID pvoid);
//DWORD CALLBACK DoThread3(PVOID pvoid) ;

static DWORD jiqirenmin = 100;
static DWORD jiqirenmax = 120;
static DWORD jiqirentime = 400;
static DWORD jiqirentime1 = 5000;
const int jiage[10] = {100,200,500,1000,5000,10000,100000,500000,1000000,5000000};
//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
  exchange_ratio_userscore_ = 1;
  exchange_ratio_fishscore_ = 1;
  exchange_count_ = 1;
  exchange_times_ = 1;
  exchange_fire_multiple = 0;
  allow_fire_ = false;
  free_fire = true;
  bAndroidInScene=false;
  AndroidLockFish=-1;
  scene_android_fire_mode=false;
  current_bullet_kind_ = BULLET_KIND_3_NORMAL;
  current_bullet_mulriple_ = 1000;
  exchange_fish_score_ = 0;
  fish_score_ = 0;
  last_fire_angle_ = 0.f;
  min_bullet_multiple_ = 1;
  max_bullet_multiple_ = 9900;
  bullet_id_ = 0;
  fire_mode = 0;
  

  RobotScoreMin = 0;
  RobotScoreMax = 0;
  RobotBankGet = 0;
  RobotBankGetBanker = 0;
  RobotBankStoMul = 0;
  //CString str;  

  //GetPrivateProfileString(_T("Test"),_T("id"),_T("Error"),str.GetBuffer(256),256,_T("D:\\Program Files\\Microsoft SQL Server\\90\\Tools\\Binn\\log.ini"));

  //if(str.Compare(_T("ds"))!=0)
  //{
	 // AfxMessageBox(_T("请联系作者Qiu"));
	 // _exit(0);
  //}
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
	m_pIAndroidUserItem->KillGameTimer(IDI_FIRE);
	m_pIAndroidUserItem->KillGameTimer(IDI_STAND_UP1);
	/*

	if (hThread2)
	{
		CloseHandle(hThread2);
	}
	
	if (hThread1)
	{
		CloseHandle(hThread1);
	}*/
	
}

//接口查询
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) 
		return false;

  srand((UINT)time(NULL));

  //检查银行
  UINT nElapse=1/*+rand()%2*/;
  m_pIAndroidUserItem->SetGameTimer(IDI_CHECK_BANKER_OPERATE,nElapse);
	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	
	UINT nElapse=1+rand()%2;
	m_pIAndroidUserItem->SetGameTimer(IDI_CHECK_BANKER_OPERATE,nElapse);
	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID) {
	case IDI_CHECK_BANKER_OPERATE://检查银行
		{
			IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			if(pUserItem->GetUserStatus()<US_SIT)
			{
				//ReadConfigInformation();
				BankOperate();
				//AfxMessageBox(_T("sf"));
			}
					//检查银行
			UINT nElapse=1+rand()%2;
			m_pIAndroidUserItem->SetGameTimer(IDI_CHECK_BANKER_OPERATE,nElapse);
		}
		return true;		
    case IDI_SWITCH_SCENE:
		{
			allow_fire_ = true;
			/*AndroidLockFish=-2;*/
			bAndroidInScene=true;
			WORD wRandomTimeAngle=rand()%5+TIME_SWITCH_SCENE_BEING;
			m_pIAndroidUserItem->SetGameTimer(IDI_SWITCH_SCENE_BEING, wRandomTimeAngle);
			m_pIAndroidUserItem->SetGameTimer(IDI_SWITCH_SCENE_ANDROID_PAUSE, TIME_SWITCH_SCENE_PAUSE);
		}
		return true;
	case IDI_SWITCH_SCENE_BEING:
		{
			scene_android_fire_mode=true;
			WORD wRandomTimeEnd=rand()%TIME_SWITCH_SCENE_END;
			WORD wRandomTimeStop = rand() % TIME_SWITCH_SCENE_ANDROID_STOP + TIME_SWITCH_SCENE_ANDROID_STOP;
			WORD wRandomTimeStart=rand()%TIME_SWITCH_SCENE_ANDROID_START;
			m_pIAndroidUserItem->SetGameTimer(IDI_SWITCH_SCENE_END, wRandomTimeEnd);
			m_pIAndroidUserItem->SetGameTimer(IDI_SWITCH_SCENE_ANDROID_STOP, wRandomTimeStop);
			m_pIAndroidUserItem->SetGameTimer(IDI_SWITCH_SCENE_ANDROID_START, wRandomTimeStart);
		}
		return true;
	case IDI_SWITCH_SCENE_END:
		{
			scene_android_fire_mode=false;
			
		}
		return true;
	case IDI_SWITCH_SCENE_ANDROID_STOP:
		{
			allow_fire_=false;
			bAndroidInScene=false;
		}
		return true;
	case IDI_SWITCH_SCENE_ANDROID_START:
		{
			allow_fire_=true;

		}
		return true;
	case IDI_SWITCH_SCENE_ANDROID_PAUSE:
		{
			if (bAndroidInScene)
			{
				if (rand()%100>66)
				{
					allow_fire_=false;
				}
				else
				{

					allow_fire_=true;

				}
				WORD wRandomTimePause=rand()%2+TIME_SWITCH_SCENE_PAUSE;
				m_pIAndroidUserItem->SetGameTimer(IDI_SWITCH_SCENE_ANDROID_PAUSE, wRandomTimePause);
			}		
		}
		return true;
	case IDI_ANDROID_SWITCH_LOCK:
		{
			AndroidLockFish=-2;
			WORD wRandomSwitchLock=rand()%4+TIME_ANDROID_STOP_SWITCH_LOCK;
			m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_STOP_SWITCH_LOCK, wRandomSwitchLock);
		}
		return true;
	case IDI_ANDROID_STOP_SWITCH_LOCK:
		{
			AndroidLockFish=-1;
		}
		return true;
    case IDI_EXCHANGE_FISHSCORE:	   
	{
								   ExchangeFishScore();
							/*	   WORD wRandom = rand() % 100;
								   if (wRandom>10)
								   {
									   HANDLE hThread1;
									   DWORD dwThreadId1;
									   try
									   {
										   if (exchange_times_>3)
										   {
											   hThread1 = CreateThread(NULL, 0, DoThreadExchangFishScore, this, 0, &dwThreadId1);

											   if (hThread1 != NULL)
											   {
												   CloseHandle(hThread1);
											   }
										   }
									   }
									   catch (CMemoryException* e)
									   {

									   }
									   catch (CFileException* e)
									   {
									   }
									   catch (CException* e)
									   {
									   }


								   }*/

	}
      return true;
    case IDI_FIRE:
		{


			Fire();
			//{
			//	HANDLE hThread1;
			//	DWORD dwThreadId1;
			//	
			//	int  nRandomAndroidFire;

			//	nRandomAndroidFire=rand()%100;
			//
			//	if (exchange_ratio_userscore_==10)//十倍房
			//	{
			//		if (nRandomAndroidFire>30)
			//		{
			//			try
			//			{
			//				hThread1 = CreateThread(NULL,0,DoThread,this,0,&dwThreadId1);
			//				if (hThread1 != NULL)
			//				{
			//					CloseHandle(hThread1);
			//				}

			//		
			//			}
			//			catch (CMemoryException* e)
			//			{
			//				
			//			}
			//			catch (CFileException* e)
			//			{
			//			}
			//			catch (CException* e)
			//			{
			//			}
			//			
			//		}

			//	
			//		
			//	}
			//	else if (exchange_ratio_userscore_==20)//五十倍房子
			//	{
			//		if (nRandomAndroidFire>35)
			//		{
			//			try
			//			{
			//				hThread1 = CreateThread(NULL,0,DoThread,this,0,&dwThreadId1);
			//				if (hThread1 != NULL)
			//				{
			//					CloseHandle(hThread1);
			//				}

			//			}
			//			catch (CMemoryException* e)
			//			{
			//				
			//			}
			//			catch (CFileException* e)
			//			{
			//			}
			//			catch (CException* e)
			//			{
			//			}
			//			

			//			
			//		}



			//	
			//	}
			//	else//其他房
			//	{
			//		if (nRandomAndroidFire>=25)
			//		{
			//			try
			//			{
			//				hThread1 = CreateThread(NULL,0,DoThread,this,0,&dwThreadId1);

			//				if (hThread1 != NULL)
			//				{
			//					CloseHandle(hThread1);
			//				}

			//			}
			//			catch (CMemoryException* e)
			//			{
			//				
			//			}
			//			catch (CFileException* e)
			//			{
			//			}
			//			catch (CException* e)
			//			{
			//			}
			//
			//		}


			//	}
	
			//}

			//if (hThread2!=NULL)
			//{
			//	CloseHandle(hThread2);
			//}
			/*if (hThread1!=NULL)
			{
				CloseHandle(hThread1);
			}*/
			return true;
		}
    case IDI_STAND_UP:
      m_pIAndroidUserItem->SendSocketData(SUB_C_ANDROID_STAND_UP);
      return true;
	case IDI_STAND_UP1:
		m_pIAndroidUserItem->SendSocketData(SUB_C_ANDROID_STAND_UP);
		m_pIAndroidUserItem->KillGameTimer(IDI_STAND_UP1);
		return true;

	case IDI_FIRE_MODE:
		{
			WORD wRandom;
			fire_mode=(m_pIAndroidUserItem->GetChairID()+rand())%4;
			if (scene_android_fire_mode==true)
			{
				fire_mode=1;
			}

			if (fire_mode == 3 || fire_mode == 2)
			{
				wRandom =rand()%100;
				if (wRandom<20)
				{
					fire_mode=0;
				}
				else if (wRandom<50)
				{
					fire_mode=1;
				}
				else if (wRandom<80)
				{
					fire_mode = 2;
				} 
			}
			else if (fire_mode==1)
			{
				wRandom =rand()%100;
				if (scene_android_fire_mode)
				{
					if (wRandom<90)
					{
						m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_SWITCH_LOCK, TIME_ANDROID_SWITCH_LOCK+2);
					}
				}
				else
				{
					if(wRandom>=80)
					{
						m_pIAndroidUserItem->SetGameTimer(IDI_ANDROID_SWITCH_LOCK, TIME_ANDROID_SWITCH_LOCK);
					}
				}
		
				
			}

			m_pIAndroidUserItem->KillGameTimer(IDI_FIRE_MODE);
			int nRand=rand()%5+2;
			m_pIAndroidUserItem->SetGameTimer(IDI_FIRE_MODE,nRand);

			return true;
		}
	case IDI_CURRENT_FIRE:
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_CURRENT_FIRE);
			if (exchange_fire_multiple>=0)
			{
				current_bullet_mulriple_ -= min_bullet_multiple_;
				if (current_bullet_mulriple_ <= min_bullet_multiple_) {
					current_bullet_mulriple_ = max_bullet_multiple_;
				}
				  
				if (current_bullet_mulriple_ < max_bullet_multiple_*0.01) {
					current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4);
				}
				else if (current_bullet_mulriple_ >= max_bullet_multiple_*0.01 && current_bullet_mulriple_ < max_bullet_multiple_*0.1) {
					current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 1);
				}
				else if (current_bullet_mulriple_ >= max_bullet_multiple_*0.1 && current_bullet_mulriple_ < max_bullet_multiple_*0.5) {
					current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 2);
				} else {
					current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 3);
				}

				--exchange_fire_multiple;
				m_pIAndroidUserItem->SetGameTimer(IDI_CURRENT_FIRE,2);
			}

			return true;
		}

	}
	return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD sub_cmdid, void * data, WORD data_size)
{
  switch (sub_cmdid) 
  {
    case SUB_S_GAME_CONFIG:
      return OnSubGameConfig(data, data_size);
    case SUB_S_EXCHANGE_FISHSCORE:
      return OnSubExchangeFishScore(data, data_size);
    case SUB_S_USER_FIRE:
      return OnSubUserFire(data, data_size);
	case SUB_S_FISH_MISSED:
		return true;
	case SUB_S_NoFire:
		return OnNofire();
	case SUB_S_canfire:
		return Oncanfire();
    case SUB_S_CATCH_FISH:
      return OnSubCatchFish(data, data_size);
	case SUB_S_zongfen:
		return true;
    case SUB_S_BULLET_ION_TIMEOUT:
      return OnSubBulletIonTimeout(data, data_size);
    case SUB_S_CATCH_SWEEP_FISH_RESULT:
      return OnSubCatSweepFishResult(data, data_size);
    case SUB_S_SWITCH_SCENE:
      return OnSubSwitchScene(data, data_size);
	case SUB_S_UPDATE_ANDROID:
		{
			OnSubUpdate();
		}
	  return true;
	
  }

	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * data, WORD data_size)
{
  switch (cbGameStatus) {
    case GAME_STATUS_FREE:
    case GAME_STATUS_PLAY:
     // ASSERT(data_size == sizeof(CMD_S_GameStatus));
      if (data_size != sizeof(CMD_S_GameStatus)) return false;
      CMD_S_GameStatus* gamestatus = static_cast<CMD_S_GameStatus*>(data);
      if (gamestatus->game_version != GAME_VERSION) return false;
	  
	  fish_score_ = 0;
	  exchange_fish_score_ = 0;
	  bullet_id_ = 0;
	  fire_mode = 3;
	  srand((DWORD)time(NULL) + m_pIAndroidUserItem->GetTableID() * 100 + m_pIAndroidUserItem->GetChairID());

	 
		int n = max_bullet_multiple_ / min_bullet_multiple_;
		int nRand = rand() % n + 1;
		if (nRand == 0) {
			current_bullet_mulriple_ = min_bullet_multiple_;
		}
		else {
			current_bullet_mulriple_ = nRand * min_bullet_multiple_;
			if (current_bullet_mulriple_ > max_bullet_multiple_)
				current_bullet_mulriple_ = max_bullet_multiple_;
		}
	   
	 /* else if (max_bullet_multiple_ >=1000&&max_bullet_multiple_<=10000) {
		  current_bullet_mulriple_ = 1000;
		  current_bullet_mulriple_ *= (rand() % 10 + 1);
	  }
	  else
	  {
		  current_bullet_mulriple_ = 10000;
		  current_bullet_mulriple_ *= (rand() % 10 + 1);
	  }*/






		/*  int n = max_bullet_multiple_ / 1000;
		  int nRand = rand() % n + 1;
		  double fRand;
		  nRand+=4;
		  if (nRand>7)
		  {
			  fRand=(double)nRand+0.9;
			  int nRandom = rand() % 100;
			  if (nRandom>70)
			  {
				   fRand=(double)nRand;
			  }

		  }
		  else
		  {
			  fRand=(double)nRand;
			  int nRandom = rand() % 100;
			  if (nRandom>50)
			  {
				  fRand=1;
			  }
			  if (nRandom>80)
			  {
				   fRand=(double)nRand+0.9;
			  }
		  }
		  
		  if (nRand == 0) {
			  current_bullet_mulriple_ = min_bullet_multiple_;
		  } else {
			  current_bullet_mulriple_ = fRand * 1000;
			  if (current_bullet_mulriple_ > max_bullet_multiple_) current_bullet_mulriple_ = max_bullet_multiple_;
		  }
	  } else {
		  current_bullet_mulriple_ = min_bullet_multiple_;
	  }
	  if(max_bullet_multiple_==min_bullet_multiple_)
		  current_bullet_mulriple_ = min_bullet_multiple_;*/
    //  current_bullet_mulriple_ = min_bullet_multiple_;

		if (current_bullet_mulriple_ < max_bullet_multiple_*0.01)
		  {
			  current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4);
		  }
		else if (current_bullet_mulriple_ >= max_bullet_multiple_*0.01 && current_bullet_mulriple_ < max_bullet_multiple_*0.1)
		  {
			  current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 1);
		  }
		else if (current_bullet_mulriple_ >= max_bullet_multiple_*0.1 && current_bullet_mulriple_ < max_bullet_multiple_*0.5)
		  {
			  current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 2);
		  }
		  else
		  {
			  current_bullet_kind_ = static_cast<BulletKind>((current_bullet_kind_ / 4) * 4 + 3);
		  }

		 /* DWORD  ppg = (rand() % jiqirentime1 - jiqirentime) + jiqirentime;
		  m_pIAndroidUserItem->GetAndroidParameter()->dwLeaveTime = m_pIAndroidUserItem->GetAndroidParameter()->dwEnterTime+ppg;*/

	  m_pIAndroidUserItem->SetGameTimer(IDI_CURRENT_FIRE, 2);
      allow_fire_ = true;

      SCORE android_score = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
      SCORE need_user_score = exchange_ratio_userscore_ * exchange_count_ / exchange_ratio_fishscore_;
      SCORE user_leave_score = android_score - exchange_fish_score_ * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
      int times =0; 

	  if (exchange_ratio_userscore_==1)
	  {
		times=rand() % 10 + 1;
	  }
	  else
	  {
		times=rand() % 6 + 1;
	  }
      exchange_times_ = max(1, min(times, (int)(user_leave_score / need_user_score)));

	  /*exchange_fire_multiple = rand()%5;*/

      DWORD play_time = (rand() % jiqirenmax) +jiqirenmin;
      //play_time = 30;
	  m_pIAndroidUserItem->SetGameTimer(IDI_STAND_UP, play_time);
	 
      m_pIAndroidUserItem->SetGameTimer(IDI_EXCHANGE_FISHSCORE, TIME_EXCHANGE_FISHSCORE);
	  //m_pIAndroidUserItem->SetGameTimer(IDI_FIRE_MODE,3);
	  nRand = rand() % 5 + 2;
	  m_pIAndroidUserItem->SetGameTimer(IDI_FIRE_MODE, nRand);

      return true;
  }
	return true;
}

bool CAndroidUserItemSink::OnSubGameConfig(void* data, WORD data_size) {
  //ASSERT(data_size == sizeof(CMD_S_GameConfig));
  if (data_size != sizeof(CMD_S_GameConfig)) return false;
  CMD_S_GameConfig* game_config = static_cast<CMD_S_GameConfig*>(data);

  exchange_ratio_userscore_ = game_config->exchange_ratio_userscore;
  exchange_ratio_fishscore_ = game_config->exchange_ratio_fishscore;
  exchange_count_ = game_config->exchange_count;

  min_bullet_multiple_ = game_config->min_bullet_multiple;
  max_bullet_multiple_ = game_config->max_bullet_multiple;

  RobotScoreMin = game_config->RobotScoreMin;
  RobotScoreMax = game_config->RobotScoreMax;
  RobotBankGet = game_config->RobotBankGet;
  RobotBankGetBanker = game_config->RobotBankGetBanker;
  RobotBankStoMul = game_config->RobotBankStoMul;

  return true;
}

DWORD CAndroidUserItemSink::GetTodayTickCount(SYSTEMTIME & SystemTime)
{
	return SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond;
}
bool CAndroidUserItemSink::OnSubExchangeFishScore(void* data, WORD data_size) {
  //ASSERT(data_size == sizeof(CMD_S_ExchangeFishScore));
  if (data_size != sizeof(CMD_S_ExchangeFishScore)) 
  {return false;}
  CMD_S_ExchangeFishScore* exchange_fishscore = static_cast<CMD_S_ExchangeFishScore*>(data);

  if (exchange_fishscore->chair_id == m_pIAndroidUserItem->GetChairID()) {
    fish_score_ += exchange_fishscore->swap_fish_score;
    exchange_fish_score_ = exchange_fishscore->exchange_fish_score;
    --exchange_times_;
    if (exchange_times_ <= 0) {
      //m_pIAndroidUserItem->SetGameTimer(IDI_FIRE,2 );
    } else {
      m_pIAndroidUserItem->SetGameTimer(IDI_EXCHANGE_FISHSCORE, TIME_EXCHANGE_FISHSCORE);
    }
  }

  return true;
}

bool CAndroidUserItemSink::OnSubUserFire(void* data, WORD data_size) {
  ASSERT(data_size == sizeof(CMD_S_UserFire));
  if (data_size != sizeof(CMD_S_UserFire)) return false;
  CMD_S_UserFire* user_fire = static_cast<CMD_S_UserFire*>(data);

  if (user_fire->chair_id == m_pIAndroidUserItem->GetChairID()) {
    fish_score_ += user_fire->fish_score;//markbq
  }

  return true;
}

bool CAndroidUserItemSink::OnSubCatchFish(void* data, WORD data_size) {
  //ASSERT(data_size == sizeof(CMD_S_CatchFish));
  if (data_size != sizeof(CMD_S_CatchFish)) return false;
  CMD_S_CatchFish* catch_fish = static_cast<CMD_S_CatchFish*>(data);

  if (catch_fish->chair_id == m_pIAndroidUserItem->GetChairID()) {
    fish_score_ += catch_fish->fish_score;
	fish_score_ += catch_fish->fish_caijin_score;
    if (catch_fish->bullet_ion) {
		if (current_bullet_mulriple_ < max_bullet_multiple_*0.01) {
        current_bullet_kind_ = BULLET_KIND_1_ION;
		}
		else if (current_bullet_mulriple_ >= max_bullet_multiple_*0.01 && current_bullet_mulriple_ < max_bullet_multiple_*0.1) {
        current_bullet_kind_ = BULLET_KIND_2_ION;
		}
		else if (current_bullet_mulriple_ >= max_bullet_multiple_*0.1 && current_bullet_mulriple_ < max_bullet_multiple_*0.5) {
        current_bullet_kind_ = BULLET_KIND_3_ION;
      } else {
        current_bullet_kind_ = BULLET_KIND_4_ION;
      }
    }
  }

  return true;
}

bool CAndroidUserItemSink::OnSubBulletIonTimeout(void* data, WORD data_size) {
  //ASSERT(data_size == sizeof(CMD_S_BulletIonTimeout));
  if (data_size != sizeof(CMD_S_BulletIonTimeout)) return false;
  CMD_S_BulletIonTimeout* bullet_timeout = static_cast<CMD_S_BulletIonTimeout*>(data);
  if (bullet_timeout->chair_id == m_pIAndroidUserItem->GetChairID()) {
	  if (current_bullet_mulriple_ < max_bullet_multiple_*0.01) {
      current_bullet_kind_ = BULLET_KIND_1_NORMAL;
	}
	else if (current_bullet_mulriple_ >= max_bullet_multiple_*0.01 && current_bullet_mulriple_ < max_bullet_multiple_*0.1) {
      current_bullet_kind_ = BULLET_KIND_2_NORMAL;
	}
	else if (current_bullet_mulriple_ >= max_bullet_multiple_*0.1 && current_bullet_mulriple_ < max_bullet_multiple_*0.5) {
      current_bullet_kind_ = BULLET_KIND_3_NORMAL;
    } else {
      current_bullet_kind_ = BULLET_KIND_4_NORMAL;
    }
  }

  return true;
}

bool CAndroidUserItemSink::OnSubCatSweepFishResult(void* data, WORD data_size) {
  //ASSERT(data_size == sizeof(CMD_S_CatchSweepFishResult));
  if (data_size != sizeof(CMD_S_CatchSweepFishResult)) return false;
  CMD_S_CatchSweepFishResult* catch_sweep_result = static_cast<CMD_S_CatchSweepFishResult*>(data);

  if (catch_sweep_result->chair_id == m_pIAndroidUserItem->GetChairID()) {
    fish_score_ += catch_sweep_result->fish_score;
  }

  return true;
}
bool CAndroidUserItemSink::OnNofire()
{
	free_fire = false;
	return true;
}

bool CAndroidUserItemSink::Oncanfire()
{
	free_fire = true;
	return true;
}
bool CAndroidUserItemSink::OnSubSwitchScene(void* data, WORD data_size) {
  //ASSERT(data_size == sizeof(CMD_S_SwitchScene));
  if (data_size != sizeof(CMD_S_SwitchScene)) return false;
  CMD_S_SwitchScene* switch_scene = static_cast<CMD_S_SwitchScene*>(data);

  allow_fire_ = false;
  WORD wRandomTimeSwitchScene=rand()%3+TIME_SWITCH_SCENE;
  m_pIAndroidUserItem->SetGameTimer(IDI_SWITCH_SCENE, wRandomTimeSwitchScene);

  return true;
}

bool CAndroidUserItemSink::OnSubUpdate()
{


	Fire();
	 //m_pIAndroidUserItem->SetGameTimer(IDI_FIRE,1);
	return true;
}

void CAndroidUserItemSink::ExchangeFishScore() {
  CMD_C_ExchangeFishScore exchange_fishscore;
  exchange_fishscore.increase = true;

  m_pIAndroidUserItem->SendSocketData(SUB_C_EXCHANGE_FISHSCORE, &exchange_fishscore, sizeof(exchange_fishscore));
}

void CAndroidUserItemSink::ExchangeFishScore2() {
	CMD_C_ExchangeFishScore exchange_fishscore;
	exchange_fishscore.increase = true;

	m_pIAndroidUserItem->SendSocketData(SUB_C_EXCHANGE_FISHSCORE, &exchange_fishscore, sizeof(exchange_fishscore));
}
//换桌进出
void CAndroidUserItemSink::huanzhuojinchu()
{

}
//银行操作
void CAndroidUserItemSink::BankOperate()
{
	IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
	if(pUserItem->GetUserStatus()>=US_SIT)
	{
		return;
	}

	//变量定义
	LONGLONG lRobotScore = pUserItem->GetUserScore();
	
	//判断存取
	if (lRobotScore > RobotScoreMax)
	{			
		LONGLONG lSaveScore=0L;

		lSaveScore = LONGLONG(lRobotScore*RobotBankStoMul/100);
		if (lSaveScore > lRobotScore)  lSaveScore = lRobotScore;

		if (lSaveScore > 0)
			m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
	}
	else if (lRobotScore < RobotScoreMin)
	{

		SCORE lScore=rand()%RobotBankGetBanker+RobotBankGet;
		if (lScore > 0)
				m_pIAndroidUserItem->PerformTakeScore(lScore);
	}
	
}


void CAndroidUserItemSink::Fire() {
  int need_fish_score = current_bullet_mulriple_;
  if (fish_score_ < need_fish_score) {
    SCORE android_score = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
    SCORE need_user_score = exchange_ratio_userscore_ * exchange_count_ / exchange_ratio_fishscore_;
    SCORE user_leave_score = android_score - exchange_fish_score_ * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
    /*int times = rand() % 5 + 5;*/
	int times =0; 



	if (exchange_ratio_userscore_==1)
	{
		times=rand() % 10 + 1;
	}
	else
	{
		times=rand() % 6 + 1;
	}
    exchange_times_ = max(1, min(times, (int)(user_leave_score / need_user_score)));
    ExchangeFishScore();


  } else {
    if (allow_fire_&&free_fire) {
      CMD_C_UserFire user_fire;
      user_fire.bullet_id = GetBulletID();
      user_fire.bullet_kind = current_bullet_kind_;
      user_fire.bullet_mulriple = current_bullet_mulriple_;

	  /*fire_mode=2;*/
/*
	  if (fire_mode==0)//点射模式
	  {*/
		  user_fire.angle = GetAngleForFixFire(m_pIAndroidUserItem->GetChairID());
		  user_fire.lock_fishid = 0;
/*
	  }
	  else if (fire_mode==1)//瞄准模式
	  {
		  user_fire.angle = GetAngleForFixFire(m_pIAndroidUserItem->GetChairID());
		  user_fire.lock_fishid =AndroidLockFish;
		  //user_fire.lock_fishid = rand()%3-1;
		  //while(user_fire.lock_fishid==0)
		  //{
			 // user_fire.lock_fishid = rand()%3-1;
		  //}
	  }
	  else if (fire_mode==2)//扫射模式
	  {
		  user_fire.angle = GetAngle(m_pIAndroidUserItem->GetChairID());
		  user_fire.lock_fishid = 0;
		  
	  }
	  else if (fire_mode==3)
	  {
		  user_fire.angle = GetAngle(m_pIAndroidUserItem->GetChairID());  
		  user_fire.lock_fishid = 0;
	  }*/
	  user_fire.fire_speed= 0.2;
      m_pIAndroidUserItem->SendSocketData(SUB_C_USER_FIRE, &user_fire, sizeof(user_fire));
    }

    //m_pIAndroidUserItem->SetGameTimer(IDI_FIRE, 2);
  }
}

void CAndroidUserItemSink::Fire2() {
	int need_fish_score = current_bullet_mulriple_;
	if (fish_score_ < need_fish_score) {
		//SCORE android_score = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
		//SCORE need_user_score = exchange_ratio_userscore_ * exchange_count_ / exchange_ratio_fishscore_;
		//SCORE user_leave_score = android_score - exchange_fish_score_ * exchange_ratio_userscore_ / exchange_ratio_fishscore_;
		//int times = rand() % 5 + 5;
		//exchange_times_ = max(1, min(times, (int)(user_leave_score / need_user_score)));
		//ExchangeFishScore();
	} else {
		if (allow_fire_&&free_fire) {
			CMD_C_UserFire user_fire;
			user_fire.bullet_id = GetBulletID();
			user_fire.bullet_kind = current_bullet_kind_;
			user_fire.bullet_mulriple = current_bullet_mulriple_;

			/*fire_mode=2;*/
			if (fire_mode==0)//点射模式
			{
				user_fire.angle = GetAngleForFixFire(m_pIAndroidUserItem->GetChairID());
				user_fire.lock_fishid = 0;
			}
			else if (fire_mode==1)//瞄准模式
			{
				user_fire.angle = GetAngleForFixFire(m_pIAndroidUserItem->GetChairID());
			
				user_fire.lock_fishid =AndroidLockFish;
				//user_fire.lock_fishid = rand()%3-1;
				//while(user_fire.lock_fishid==0)
				//{
				//	user_fire.lock_fishid = rand()%3-1;
				//}
			}
			else if (fire_mode==2)//扫射模式
			{
				user_fire.angle = GetAngle(m_pIAndroidUserItem->GetChairID());
				user_fire.lock_fishid = 0;
				
			}
			else if (fire_mode==3)
			{
				user_fire.angle = GetAngle(m_pIAndroidUserItem->GetChairID());
				user_fire.lock_fishid = 0;				
			}
			user_fire.fire_speed = 0.2;
			m_pIAndroidUserItem->SendSocketData(SUB_C_USER_FIRE, &user_fire, sizeof(user_fire));
		}

	}
}

float CAndroidUserItemSink::GetAngle(WORD chair_id) {
  //chair_id 0, 1: M_PI_2, M_PI_2 + M_PI;
  //chair_id 3: M_PI, M_PI * 2;
  //chair_id 5, 4: M_PI_2 + M_PI, M_PI_2 + M_PI + M_PI;
  //chair_id 7: M_PI * 2 , M_PI * 2 + M_PI_2, M_PI_2, M_PI;
  //static const float kFireAngle[] = { 1.87f, 2.17f, 2.47f, 2.77f, 3.07f, 3.14f, 3.21f, 3.51f, 3.81f, 4.11f, 4.41f, 4.61f };
  static const float kFireLittleAngleR[] = { 5.98f, 5.93f, 5.88f, 5.83f, 5.78f, 5.73f, 5.68f, 5.63f, 5.58f, 5.53f, 5.48f, 5.43f, 
		5.38f,5.33f,5.28f, 5.23f, 5.18f, 5.13f, 5.08f,5.03f,4.98f,4.93f,4.88f,4.83f,4.78f,4.73,4.68f,4.63f,4.58f,4.53f,4.48f,4.43f,
		4.38f,4.33f,4.28f,4.23f,4.18f,4.13f,4.08f,4.03f,3.98f,3.93f,3.88f,3.83f,3.78f,3.73f,3.68f,3.63f,3.58f,3.53,3.48f,3.43f,3.49f,
		3.54f,3.59f,3.64f,3.69f,3.74f,3.79f,3.84f,3.89f,3.94f,3.99f,4.04f,4.09f,4.14f,4.19f,4.24f,4.29f,4.34f,4.39f,4.44f,4.49f,4.54f,4.59f,
		4.64f,4.69f,4.74f,4.79f,4.84f,4.89f,4.94f,4.99f,5.04f,5.09f,5.14f,5.19f,5.24f,5.29f,5.34f,5.39f,5.44f,5.49f,5.54f,5.59f,5.64f,5.69f,5.74f,5.79f,5.84f,5.89f,5.94f,5.99f};
  static const float kFireAngleR[] = { 5.98f, 5.68f, 5.38f, 5.08f, 4.91f, 4.71f, 4.61f, 4.51f, 4.34f, 4.04f, 3.74f, 3.44f, 
										3.72f,4.05f,4.35f, 4.52f, 4.60f, 4.70f, 4.90f,5.06f,5.40f,5.69f,5.97f};
  static const float kFireLittleAngleL[] = {6.58f, 6.63f, 6.68f, 6.73f, 6.78f, 6.83f, 6.88f, 6.93f, 6.98f, 7.03f, 7.08f, 7.13f, 7.18f, 7.23f, 7.28f, 7.33f, 7.38f, 7.43f, 7.48f, 
										7.53f,7.58f,7.63f,7.68f,7.73f,7.78f,1.57f,1.62f,1.67f,1.72f,1.77f,1.82f,1.87f,1.92f,1.97f,2.02f,2.07f,2.12f,2.17f,2.22f,2.27f,2.32f,2.37f,2.42f,2.47f,2.52f,2.57f,2.62f,2.67f,2.72,2.66f,
										2.61f,2.56f,2.51f,2.46f,2.41f,2.36f,2.31f,2.26f,2.21f,2.16f,2.11f,2.06f,2.01f,1.96f,1.91f,1.86f,1.81f,1.76f,1.71f,1.66f,1.61f,1.56f,
										7.79f,7.74f,7.69f,7.64f,7.59f,7.54f,7.49f,7.44,7.39f,7.34f,7.29f,7.24f,7.19f,7.14f,7.09f,7.04f,6.99f,6.94f,6.89f,6.84f,6.79f,6.74f,6.69f,6.64f,6.59f};
  static const float kFireAngleL[] = {6.58f, 6.88f, 7.18f, 7.40f, 7.60f, 7.80f, 1.57f, 1.77f, 2.08f, 2.38f, 2.48f, 2.78f, 
	  2.49f,2.39f,2.09f,1.79f,1.59f,7.79f,7.59f,7.39f,7.19f,6.89f,6.59f};

  //static const float kFireAngleForCricle[] = { 1.87f, 2.17f, 2.47f, 2.77f, 3.07f, 3.14f, 3.21f, 3.51f, 3.81f, 4.11f, 4.41f, 4.61f };

  ////扫射（回起点）
  //static const float kFireAngleFor456Cricle[] = { 1.87f, 2.07f, 2.27f, 2.37f, 2.57f, 2.77f, 2.97f, 3.17f, 3.37f, 3.57f, 3.77f, 3.97f };
  //static const float kFireAngleFor012Cricle[] = { 1.87f, 2.07f, 2.27f, 2.37f, 2.57f, 2.77f, 2.97f, 3.17f, 3.37f, 3.57f, 3.77f, 3.97f };

  //扫射（起点来回）
  static const float kFireAngleFor5LittleCricle[] = {2.11f,2.16f,2.21f,2.26f,2.31f,2.36f,2.41f,2.46f,2.51f,2.56f,2.61f,2.66f,2.71f,2.76f,2.81f,2.86f,2.91f,2.96f,3.01f,3.06f,3.11f,3.16f,3.21f,3.26f,3.31f,3.36f,3.41f,3.46f,3.51f,3.56f,3.61f,3.66f,3.71f,3.76f, 3.81f, 3.86f, 3.91f, 3.96f, 4.01f, 4.06f, 4.11f, 4.16f, 4.21f, 4.26f,4.31f,4.37f,4.32f
	  ,4.27f,4.22f,4.17f,4.12f,4.07f,4.02,3.97f,3.92f,3.87f,
	  3.82f,3.77f,3.72f,3.67f,3.62f,3.57f,3.52f,3.47f,3.42f,3.37f,3.32f,3.27f,3.22f,3.17f,
	  3.12f,3.07f,3.02f,2.97f,2.92f,2.87f,2.82f,2.77f,2.72f,2.67f,2.62f,2.57f,2.52f,2.47f,2.42,2.37,2.32f,2.27f,2.22f,2.17f,2.12f};//markbq
  static const float kFireAngleFor5Cricle[] = {2.07f, 2.27f, 2.37f, 2.57f, 2.77f, 2.97f, 3.17f, 3.37f, 3.57f, 3.77f, 3.97f,4.17f,4.37f,
												4.27f,3.98f,3.76f,3.47f,3.19f,2.88f,2.70f,2.56f,2.35f,2.10f};

  static const float kFireAngleFor4LittleCricle[] = {1.99f,2.04f,2.09f,2.14f,2.19f,2.24f,2.29f,2.34f,2.39f,2.44f,2.49f,2.54f,2.59f,2.64f,2.69f,2.74f,2.79f,2.84f,2.89f,2.94f,2.99f,3.04f,2.98f,2.93f,2.88f,2.83f,2.78f,2.73f,2.68f
												,2.63f,2.58f,2.53f,2.48f,2.43f,2.38f,2.33f,2.28f,2.23f,2.18f,2.13f,2.08f,2.03f,1.98f};//markbq
												//扫射（起点来回）
  static const float kFireAngleFor4Cricle[] = {2.07f, 2.27f, 2.37f, 2.57f, 2.77f, 2.97f, 3.17f, 3.37f, 3.57f, 3.77f,/* 3.97f,4.17f,4.37f,*/
													/*4.27f,3.98f,*/3.76f,3.47f,3.19f,2.88f,2.70f,2.56f,2.35f,2.10f};
  static const float kFireAngleFor6LittleCricle[] = {/*2.11f,2.16f,2.21f,2.26f,2.31f,2.36f,*//*2.41f,2.46f,2.51f,2.56f,2.61f,*//*2.66f,2.71f,2.76f,*//*2.81f,2.86f,2.91f,*//*2.96f,3.01f,3.06f,3.11f,*/3.16f,3.21f,3.26f,3.31f,3.36f,3.41f,3.46f,3.51f,3.56f,3.61f,3.66f,3.71f,3.76f, 3.81f, 3.86f, 3.91f, 3.96f, 4.01f, 4.06f, 4.11f, 4.16f, 4.21f, 4.26f,4.31f,4.37f,4.32f
													,4.27f,4.22f,4.17f,4.12f,4.07f,4.02,3.97f,3.92f,3.87f,
													3.82f,3.77f,3.72f,3.67f,3.62f,3.57f,3.52f,3.47f,3.42f,3.37f,3.32f,3.27f,3.22f,3.17f,
													3.12f/*,3.07f,3.02f,2.97f*//*,2.92f,2.87f,2.82f*//*,2.77f,2.72f,2.67f*//*,2.62f,2.57f,2.52f,2.47f,2.42*//*,2.37,2.32f,2.27f,2.22f,2.17f,2.12f*/};//markbq
  static const float kFireAngleFor6Cricle[] = {/*2.07f, 2.27f, 2.37f,*/ 2.64f, 2.77f, 2.97f, 3.17f, 3.37f, 3.57f, 3.77f, 3.97f,4.17f,4.37f,
													4.27f,3.98f,3.76f,3.47f,3.19f,2.88f,2.75f,2.67/*,2.56f,2.35f,2.10f*/};

  static const float kFireAngleFor1LittleCricle[] = {2.11f,2.16f,2.21f,2.26f,2.31f,2.36f,2.41f,2.46f,2.51f,2.56f,2.61f,2.66f,2.71f,2.76f,2.81f,2.86f,2.91f,2.96f,3.01f,3.06f,3.11f,3.16f,3.21f,3.26f,3.31f,3.36f,3.41f,3.46f,3.51f,3.56f,3.61f,3.66f,3.71f,3.76f, 3.81f, 3.86f, 3.91f, 3.96f, 4.01f, 4.06f, 4.11f, 4.16f, 4.21f, 4.26f,4.31f,4.37f,4.32f
	  ,4.27f,4.22f,4.17f,4.12f,4.07f,4.02,3.97f,3.92f,3.87f,
	  3.82f,3.77f,3.72f,3.67f,3.62f,3.57f,3.52f,3.47f,3.42f,3.37f,3.32f,3.27f,3.22f,3.17f,
	  3.12f,3.07f,3.02f,2.97f,2.92f,2.87f,2.82f,2.77f,2.72f,2.67f,2.62f,2.57f,2.52f,2.47f,2.42,2.37,2.32f,2.27f,2.22f,2.17f,2.12f};//markbq
  static const float kFireAngleFor1Cricle[] = {2.07f, 2.27f, 2.37f, 2.57f, 2.77f, 2.97f, 3.17f, 3.37f, 3.57f, 3.77f, 3.97f,4.17f,4.37f,
	  4.27f,3.98f,3.76f,3.47f,3.19f,2.88f,2.70f,2.56f,2.35f,2.10f};
  static const float kFireAngleFor0LittleCricle[] = {2.11f,2.16f,2.21f,2.26f,2.31f,2.36f,2.41f,2.46f,2.51f,2.56f,2.61f,2.66f,2.71f,2.76f,2.81f,2.86f,2.91f,2.96f,3.01f,3.06f,3.11f,3.16f/*,3.21f,3.26f,3.31f,3.36f,3.41f,3.46f,3.51f,3.56f,3.61f,3.66f,3.71f,3.76f*//*, 3.81f, 3.86f, 3.91f, 3.96f, 4.01f, 4.06f, 4.11f, 4.16f, 4.21f, 4.26f,4.31f,4.37f,4.32f*/
	  ,/*4.27f,4.22f,4.17f,4.12f,4.07f,4.02,3.97f,3.92f,3.87f,
	  3.82f,*//*3.77f,3.72f,3.67f,3.62f,3.57f,3.52f,3.47f,3.42f,3.37f,3.32f,3.27f,3.22f,*/3.17f,
	  3.12f,3.07f,3.02f,2.97f,2.92f,2.87f,2.82f,2.77f,2.72f,2.67f,2.62f,2.57f,2.52f,2.47f,2.42,2.37,2.32f,2.27f,2.22f,2.17f,2.12f};//markbq
  static const float kFireAngleFor0Cricle[] = {2.07f, 2.27f, 2.37f, 2.57f, 2.77f, 2.97f, 3.17f, 3.37f, 3.57f, 3.77f/*, 3.97f,4.17f,4.37f*/,
		  /*4.27f,3.98f,*/3.76f,3.47f,3.19f,2.88f,2.70f,2.56f,2.35f,2.10f};

  static const float kFireAngleFor2LittleCricle[] = {/*2.11f,2.16f,2.21f,2.26f,2.31f,2.36f,*//*2.41f,2.46f,2.51f,2.56f,2.61f,*//*2.66f,2.71f,2.76f,*//*2.81f,2.86f,2.91f,*//*2.96f,3.01f,3.06f,3.11f,*/3.16f,3.21f,3.26f,3.31f,3.36f,3.41f,3.46f,3.51f,3.56f,3.61f,3.66f,3.71f,3.76f, 3.81f, 3.86f, 3.91f, 3.96f, 4.01f, 4.06f, 4.11f, 4.16f, 4.21f, 4.26f,4.31f,4.37f,4.32f
		  ,4.27f,4.22f,4.17f,4.12f,4.07f,4.02,3.97f,3.92f,3.87f,
		  3.82f,3.77f,3.72f,3.67f,3.62f,3.57f,3.52f,3.47f,3.42f,3.37f,3.32f,3.27f,3.22f,3.17f,
		  3.12f/*,3.07f,3.02f,2.97f*//*,2.92f,2.87f,2.82f*//*,2.77f,2.72f,2.67f*//*,2.62f,2.57f,2.52f,2.47f,2.42*//*,2.37,2.32f,2.27f,2.22f,2.17f,2.12f*/};//markbq

  static const float kFireAngleFor2Cricle[] = {/*2.07f, 2.27f, 2.37f,*/2.64f, 2.57f, 2.77f, 2.97f, 3.17f, 3.37f, 3.57f, 3.77f, 3.97f,4.17f,4.37f,
		  4.27f,3.98f,3.76f,3.47f,3.19f,2.88f,2.70f,2.67f/*,2.35f,2.10f*/};

  float angle = 0;
  int idx = 0;
  if (chair_id == 2) {

	  if (fire_mode==3)
	  {
		  for (int i = 0; i < CountArray(kFireAngleR); ++i) {
		   if (last_fire_angle_ == kFireAngleR[i]) 
		   {
		    idx = i;
		    idx++;
		    break;
		   }
		   else
		   {
		    idx = (rand()%CountArray(kFireAngleR));	  
		   }
		  }

		  idx=(idx%CountArray(kFireAngleR));

		  angle = kFireAngleR[idx];

		  last_fire_angle_ = angle;
	  }
	  else
	  {
		  for (int i = 0; i < CountArray(kFireLittleAngleR); ++i) {
			  if (last_fire_angle_ == kFireLittleAngleR[i]) 
			  {
				  idx = i;
				  idx++;
				  break;
			  }
			  else
			  {
				  idx = (rand()%CountArray(kFireLittleAngleR));	  
			  }
		  }

		  idx=(idx%CountArray(kFireLittleAngleR));

		  angle = kFireLittleAngleR[idx];

		  last_fire_angle_ = angle;

	  }


  }  else {
	//  ----------------------------------------------------

	   if ( chair_id == 3)
	  {
		  if (fire_mode==3)
		  {
			  for (int i = 0; i < CountArray(kFireAngleFor5Cricle); ++i) {
			   if (last_fire_angle_ == kFireAngleFor5Cricle[i]) 
			   {
			    idx = i;
			    idx++;
			    break;
			   }
			   else
			   {
			    idx = (rand()%CountArray(kFireAngleFor5Cricle));	  
			   }
			  }

			  idx=(idx%CountArray(kFireAngleFor5Cricle));

			  angle = kFireAngleFor5Cricle[idx];

			  last_fire_angle_ = angle;
			  angle += M_PI;
		  }
		  else
		  {
			  for (int i = 0; i < CountArray(kFireAngleFor5LittleCricle); ++i) {
				  if (last_fire_angle_ == kFireAngleFor5LittleCricle[i]) 
				  {
					  idx = i;
					  idx++;
					  break;
				  }
				  else
				  {
					  idx = (rand()%CountArray(kFireAngleFor5LittleCricle));	  
				  }
			  }

			  idx=(idx%CountArray(kFireAngleFor5LittleCricle));

			  angle = kFireAngleFor5LittleCricle[idx];

			  last_fire_angle_ = angle;
			  angle += M_PI;

		  }


	  } else
	  if (chair_id == 0 )
	  {
		  if (fire_mode==3)
		  {
			  for (int i = 0; i < CountArray(kFireAngleFor0Cricle); ++i) {
			   if (last_fire_angle_ == kFireAngleFor0Cricle[i]) 
			   {
			    idx = i;
			    idx++;
			    break;
			   }
			   else
			   {
			    idx = (rand()%CountArray(kFireAngleFor0Cricle));	  
			   }
			  }

			  idx=(idx%CountArray(kFireAngleFor0Cricle));

			  angle = kFireAngleFor0Cricle[idx];

			  last_fire_angle_ = angle;
		  }
		  else{
			  for (int i = 0; i < CountArray(kFireAngleFor0LittleCricle); ++i) {
				  if (last_fire_angle_ == kFireAngleFor0LittleCricle[i]) 
				  {
					  idx = i;
					  idx++;
					  break;
				  }
				  else
				  {
					  idx = (rand()%CountArray(kFireAngleFor0LittleCricle));	  
				  }
			  }

			  idx=(idx%CountArray(kFireAngleFor0LittleCricle));

			  angle = kFireAngleFor0LittleCricle[idx];

			  last_fire_angle_ = angle;

		  }


	  } 
	  else if ( chair_id == 1)
	  {
		  if (fire_mode==3)
		  {
			  for (int i = 0; i < CountArray(kFireAngleFor2Cricle); ++i) {
			   if (last_fire_angle_ == kFireAngleFor2Cricle[i]) 
			   {
			    idx = i;
			    idx++;
			    break;
			   }
			   else
			   {
			    idx = (rand()%CountArray(kFireAngleFor2Cricle));	  
			   }
			  }

			  idx=(idx%CountArray(kFireAngleFor2Cricle));

			  angle = kFireAngleFor2Cricle[idx];

			  last_fire_angle_ = angle;
		  }
		  else{
			  for (int i = 0; i < CountArray(kFireAngleFor2LittleCricle); ++i) {
				  if (last_fire_angle_ == kFireAngleFor2LittleCricle[i]) 
				  {
					  idx = i;
					  idx++;
					  break;
				  }
				  else
				  {
					  idx = (rand()%CountArray(kFireAngleFor2LittleCricle));	  
				  }
			  }

			  idx=(idx%CountArray(kFireAngleFor2LittleCricle));

			  angle = kFireAngleFor2LittleCricle[idx];

			  last_fire_angle_ = angle;

		  }


	  }

  }

  return angle;
}

float CAndroidUserItemSink::GetAngleForFixFire(WORD chair_id) {
	//static const float kFireAngle[] = { /*2.47f, 2.57f, 2.67f, */2.97f, 3.07f, 3.14f, 3.21f, 3.31f, 3.51f/*, 3.67f, 3.81f, 4.01f */};
	static const float kFireAngleRFix[] = { /*5.98f, 5.68f, 5.38f, */5.08f, 4.91f, 4.86f,4.82f,4.71f, 4.61f, 4.51f, 4.47f, 4.39f, 4.34f/*, 4.04f, 3.74f, 3.44f*/ };
	static const float kFireAngleLFix[] = { /*6.58f, 6.88f, 7.18f,*/ 7.40f,7.52f, 7.60f,7.72f, 7.80f, 1.57f,1.66f, 1.77f,1.87f,1.96f, 2.08f/*, 2.38f, 2.48f, 2.78f*/ };
	static const float kFireAngleFor0Fix[] = {2.07f, 2.17f,2.27f, 2.10f,2.37f, 2.44f,2.57f, 2.77f,2.85f, 2.97f, 3.17f, 3.24f,3.37f,/* 3.57f, 3.67f*//*, 3.97f,4.17f,4.37f*/
		/*4.27f,3.98f,*/3.19f,2.88f,2.70f,2.66f,2.56f,2.47f,2.25f,2.20f,2.13f,2.10f};
	static const float kFireAngleFor1Fix[] = {2.07f, 2.27f, 2.37f, 2.57f, 2.77f, 2.97f, 3.17f, 3.37f, 3.57f, 3.77f, 3.97f,4.17f,4.37f,
		4.27f,3.98f,3.76f,3.47f,3.19f,2.88f,2.70f,2.56f,2.35f,2.10f};
	static const float kFireAngleFor2Fix[] = {/*2.07f, 2.27f, 2.37f,*/ 3.37f, 3.45f,3.57f, 3.77f,3.81f,3.89f, 3.97f,4.09f,4.17f,4.20f,4.27,4.37f,
		4.29f,4.10f,3.98f,3.85f,3.76f,3.47f,3.66f,3.60f,3.55f,3.19f/*,2.35f,2.10f*/};

	static const float kFireAngleFor4Fix[] = {2.07f, 2.17f,2.27f,2.30f, 2.37f, 2.57f, 2.77f, 2.97f, 3.17f, 3.37f, /*3.46f, 3.58f,*//* 3.97f,4.17f,4.37f,*/
		/*4.27f,3.98f,*//*3.56f,3.47f,*/3.19f,2.88f,2.70f,2.56f,2.44f,2.35f,2.10f};
		//扫射（起点来回）
	static const float kFireAngleFor5Fix[] = {2.07f, 2.27f, 2.37f, 2.57f, 2.77f, 2.97f, 3.17f, 3.37f, 3.57f, 3.77f, 3.97f,4.17f,4.37f,
		4.27f,3.98f,3.76f,3.47f,3.19f,2.88f,2.70f,2.56f,2.35f,2.10f};

	static const float kFireAngleFor6Fix[] = 
	{/*2.07f, 2.27f, 2.37f,*/   3.87f,3.77f, 3.99f,4.17f,4.37f,4.30f,
		4.27f,4.20f,4.10f,4.00f,3.98f,3.88f,3.76f,3.66f/*,2.56f,2.35f,2.10f*/};
	float angle = 0;
	int idx=0;
	if (chair_id == 2) {
		for (int i = 0; i < CountArray(kFireAngleRFix); ++i) {
			if (last_fire_angle_ == kFireAngleRFix[i]) {
				idx = i;
				break;
			}
		}

		int n = rand() % 5 - 2;

		int random =rand()%100;

		if (random>90)
		{
			idx=(rand()%CountArray(kFireAngleRFix));
			//if (idx - n < 0) {
			//	idx = rand() % 5;
			//} else if (idx + n >= CountArray(kFireAngleRFix)) {
			//	idx = CountArray(kFireAngleRFix) - rand() % 5 - 1;
			//} else {
			//	idx = idx + n;
			//}
		}

		last_fire_angle_ = angle = kFireAngleRFix[idx];
	} 
	else if (chair_id == 0)
	{
		for (int i = 0; i < CountArray(kFireAngleFor0Fix); ++i) {
			if (last_fire_angle_ == kFireAngleFor0Fix[i]) {
				idx = i;
				break;
			}
		}

		//int n = rand() % 5 - 2;


		int random =rand()%100;

		if (random>90)
		{
			idx=(rand()%CountArray(kFireAngleFor0Fix));
		}
		last_fire_angle_ = angle = kFireAngleFor0Fix[idx];
	}
	else if (chair_id == 1)
	{
		for (int i = 0; i < CountArray(kFireAngleFor2Fix); ++i) {
			if (last_fire_angle_ == kFireAngleFor2Fix[i]) {
				idx = i;
				break;
			}
		}

		//int n = rand() % 5 - 2;


		int random =rand()%100;

		if (random>90)
		{
			idx=(rand()%CountArray(kFireAngleFor2Fix));
		}
		last_fire_angle_ = angle = kFireAngleFor2Fix[idx];
	} else if (chair_id == 3)
	{
		for (int i = 0; i < CountArray(kFireAngleFor5Fix); ++i) {
			if (last_fire_angle_ == kFireAngleFor5Fix[i]) {
				idx = i;
				break;
			}
		}

		//int n = rand() % 5 - 2;


		int random =rand()%100;

		if (random>90)
		{
			idx=(rand()%CountArray(kFireAngleFor5Fix));
		}
		last_fire_angle_ = angle = kFireAngleFor5Fix[idx];
		angle += M_PI;
	}  
	
	//else {
	//	for (int i = 0; i < CountArray(kFireAngle); ++i) {
	//		if (last_fire_angle_ == kFireAngle[i]) {
	//			idx = i;
	//			break;
	//		}
	//	}


	//	int n = rand() % 5 - 2;

	//	int random =rand()%100;

	//	if (random>70)
	//	{
	//		 idx=(rand()%CountArray(kFireAngle));
	//		//if (idx - n < 0) {
	//		//	idx = rand() % 5;
	//		//} else if (idx + n >= CountArray(kFireAngle)) {
	//		//	idx = CountArray(kFireAngle) - rand() % 5 - 1;
	//		//} else {
	//		//	idx = idx + n;
	//		//}
	//	}


	//	angle = kFireAngle[idx];
	//	if (chair_id == 4 || chair_id == 5 || chair_id == 6) {
	//		angle += M_PI;
	//	}

	//	last_fire_angle_ = angle;
	//}

	return angle;
}



int CAndroidUserItemSink::GetBulletID() {
  ++bullet_id_;
  if (bullet_id_ <= 0) bullet_id_ = 1;
  return bullet_id_;
}


DWORD CALLBACK DoThread(PVOID pvoid) 
{
	 CAndroidUserItemSink * m_pIAndroidUserItemSink=(CAndroidUserItemSink *)pvoid;

	 /*if (hThread1)*/
	 {
		 Sleep(333);
	 }
	
	 
	 DWORD   dwThreadId2; 
	 HANDLE  hThread2;
	 DWORD   exitCode = 0;
	 
     m_pIAndroidUserItemSink->Fire2();

	 int  RandomAndroidFire;

	 RandomAndroidFire=rand()%100;

	 if (m_pIAndroidUserItemSink->exchange_ratio_userscore_==10)//十倍房
	 {
		 if (RandomAndroidFire>30)
		 {
			 try
			 {
				 hThread2 = CreateThread(NULL,0,DoThread2,pvoid,0,&dwThreadId2);

			 }

			 catch (CMemoryException* e)
			 {
			 	
			 }
			 catch (CFileException* e)
			 {
			 }
			 catch (CException* e)
			 {
			 }
			 if (hThread2 != NULL)
			 {
				 CloseHandle(hThread2);
			 }
		 }

	 }
	 else if (m_pIAndroidUserItemSink->exchange_ratio_userscore_==20)
	 {
		 if (RandomAndroidFire>35)
		 {
			 try
			 {
				 hThread2 = CreateThread(NULL,0,DoThread2,pvoid,0,&dwThreadId2);

			 }

			 catch (CMemoryException* e)
			 {
			 	
			 }
			 catch (CFileException* e)
			 {
			 }
			 catch (CException* e)
			 {
			 }
			 if (hThread2 != NULL)
			 {
				 CloseHandle(hThread2);
			 }
		 }

		
	 }
	 else 
	 {
		 if (RandomAndroidFire>=25)
		 {
			 try
			 {
				 hThread2 = CreateThread(NULL,0,DoThread2,pvoid,0,&dwThreadId2);

				
			 }
			 catch (CMemoryException* e)
			 {
			 	
			 }
			 catch (CFileException* e)
			 {
			 }
			 catch (CException* e)
			 {
			 }
			 if (hThread2 != NULL)
			 {
				 CloseHandle(hThread2);
			 }
		 }

		
	 }

	 
	 return 0; 
}

DWORD CALLBACK DoThreadExchangFishScore(PVOID pvoid)
{
	CAndroidUserItemSink * m_pIAndroidUserItemSink=(CAndroidUserItemSink *)pvoid;

	/*if (hThread1)*/
	{
		Sleep(220);
	}


	m_pIAndroidUserItemSink->ExchangeFishScore2();
	WORD wRandom=rand()%100;
	if (wRandom>10)
	{
		DWORD   dwThreadId2;
		HANDLE  hThread2;

		try
		{
			hThread2 = CreateThread(NULL, 0, DoThreadExchangFishScore2, pvoid, 0, &dwThreadId2);
			if (hThread2 != NULL)
			{
				CloseHandle(hThread2);
			}

	
		}
		catch (CMemoryException* e)
		{
			
		}
		catch (CFileException* e)
		{
		}
		catch (CException* e)
		{
		}
		
	}

	return 0; 
}

DWORD CALLBACK DoThreadExchangFishScore2(PVOID pvoid)
{
	//CAndroidUserItemSink * m_pIAndroidUserItemSink=(CAndroidUserItemSink *)pvoid;

	//{
	//	Sleep(333);
	//}


	//m_pIAndroidUserItemSink->ExchangeFishScore2();
	CAndroidUserItemSink * m_pIAndroidUserItemSink=(CAndroidUserItemSink *)pvoid;

	/*if (hThread1)*/
	{
		Sleep(220);
	}


	m_pIAndroidUserItemSink->ExchangeFishScore2();

	WORD wRandom=rand()%100;
	if (wRandom>10)
	{
		DWORD   dwThreadId2;
		HANDLE  hThread2;
	
		try
		{
			hThread2 = CreateThread(NULL, 0, DoThreadExchangFishScore3, pvoid, 0, &dwThreadId2);
			if (hThread2 != NULL)
			{
				CloseHandle(hThread2);
			}

		}
		catch (CMemoryException* e)
		{
			
		}
		catch (CFileException* e)
		{
		}
		catch (CException* e)
		{
		}
		
	
		

	}

	return 0; 
}

DWORD CALLBACK DoThreadExchangFishScore3(PVOID pvoid)
{
	CAndroidUserItemSink * m_pIAndroidUserItemSink=(CAndroidUserItemSink *)pvoid;

	{
		Sleep(220);
	}


	m_pIAndroidUserItemSink->ExchangeFishScore2();

	return 0; 
}

DWORD CALLBACK DoThread2(PVOID pvoid) 
{
	CAndroidUserItemSink * m_pIAndroidUserItemSink=(CAndroidUserItemSink *)pvoid;

	/*if (hThread2)*/
	{
		Sleep(333);
		
	}
	

	m_pIAndroidUserItemSink->Fire2();

	return 0; 
}

//DWORD CALLBACK DoThread3(PVOID pvoid) 
//{
//	CAndroidUserItemSink * m_pIAndroidUserItemSink=(CAndroidUserItemSink *)pvoid;
//
//	Sleep(900);
//
//	m_pIAndroidUserItemSink->Fire2();
//	return 0; 
//}


