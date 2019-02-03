#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//��������
class CAndroidUserItemSink : public IAndroidUserItemSink
{
protected:
   IAndroidUserItem *				m_pIAndroidUserItem;				//�û��ӿ�
public:
	int exchange_ratio_userscore_;
	int exchange_ratio_fishscore_;
private:
  int exchange_count_;

  float min_bullet_multiple_;
  float max_bullet_multiple_;

  int RobotScoreMin;
  int RobotScoreMax;
  int RobotBankGet;
  int RobotBankGetBanker;
  int RobotBankStoMul;


  BulletKind current_bullet_kind_;
  float current_bullet_mulriple_;
  bool allow_fire_;
  bool free_fire;
  bool bAndroidInScene;
  bool scene_android_fire_mode;
  int  AndroidLockFish;

  SCORE exchange_fish_score_;
  SCORE fish_score_;

  float last_fire_angle_;
  int exchange_times_;
  float exchange_fire_multiple;

  int bullet_id_;
  int fire_mode;

	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { }
	//�Ƿ���Ч
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CAndroidUserItemSink))?true:false; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��ʼ�ӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) {}
	//�û��뿪
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}
	//�û�����
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) {}
	//�û�״̬
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) {}
	//�û���λ
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser) {}

	// ��Ϣ����
protected:
  bool OnSubGameConfig(void* data, WORD data_size);
  DWORD GetTodayTickCount(SYSTEMTIME & SystemTime);
 

  bool OnSubExchangeFishScore(void* data, WORD data_size);
  bool OnSubUserFire(void* data, WORD data_size);
  bool OnSubCatchFish(void* data, WORD data_size);
  bool OnSubBulletIonTimeout(void* data, WORD data_size);
  bool OnSubCatSweepFishResult(void* data, WORD data_size);
  bool OnSubSwitchScene(void* data, WORD data_size);
  bool OnNofire();
  bool Oncanfire();
  bool OnSubUpdate();
private:
  void ExchangeFishScore();
  void Fire();
 // void Fire2();
  float GetAngle(WORD chair_id);
  float GetAngleForFixFire(WORD chair_id);
  int GetBulletID();
public:
	void Fire2();
	void ExchangeFishScore2();
private:
	//���в���
	void BankOperate();
	//�����˻�������
	void huanzhuojinchu();
};

#endif  // ANDROID_USER_ITEM_SINK_H_
