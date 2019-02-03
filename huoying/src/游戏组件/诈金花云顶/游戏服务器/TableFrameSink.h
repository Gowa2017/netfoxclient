#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerControl.h"
#include "AESEncrypt.h"
#include <map>
using namespace std;
typedef CMap<WORD,WORD,int,int> CMapControlID;
//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink :public ITableFrameSink, public ITableUserAction
{
	//��Ϸ����
protected:
	BYTE							m_bOperaCount;							//��������
	WORD							m_wCurrentUser;							//��ǰ�û�
	WORD							m_wBankerUser;							//ׯ���û�
	WORD							m_wFlashUser[GAME_PLAYER];				//�����û�

	//�û�״̬
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE							m_cbGiveUpUser[GAME_PLAYER];			//��Ϸ״̬
	SCORE						m_lCompareCount;						//����״̬
	bool							m_bGameEnd;								//����״̬
	CMD_S_GameEnd					m_StGameEnd;							//��������
	BYTE							m_cbRealPlayer[GAME_PLAYER];			//�������
	BYTE							m_cbAndroidStatus[GAME_PLAYER];			//����״̬

	//�˿˱���
protected:
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
	CWHArray<WORD>					m_wCompardUser[GAME_PLAYER];			//�˿�����
	BYTE							m_cbCardDataForShow[GAME_PLAYER][MAX_COUNT];//�����˿�

	//��ע��Ϣ
protected:
	SCORE						m_lTableScore[GAME_PLAYER];				//��ע��Ŀ
	SCORE						m_lUserMaxScore[GAME_PLAYER];			//�����ע
	SCORE						m_lMaxCellScore;						//��Ԫ����
	SCORE						m_lCellScore;							//��Ԫ��ע
	SCORE						m_lCurrentTimes;						//��ǰ����
	bool							m_bMingZhu[GAME_PLAYER];				//������ע

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���

	//�������
protected:
	HINSTANCE						m_hControlInst;
	IServerControl*					m_pServerControl;

	//AI����
protected:
	bool							m_bUpdataStorage;						//���¿��


	//���Ա���
protected:
	WORD							m_wPlayerCount;							//��Ϸ����
	static const BYTE				m_GameStartMode;						//��ʼģʽ

	//��Կ
	unsigned char					m_chUserAESKey[GAME_PLAYER][AESKEY_TOTALCOUNT];	//��ʼ��Կ

	//����ִ�
	map<DWORD,int>					m_RoundsMap;						//������ӣ���ǰ�ִ�
	TCHAR							m_szConfigFileName[MAX_PATH];		//�����ļ�
	int								m_nCurrentRounds;					//��ǰ�ִ�
	int								m_nTotalRounds;						//���ִ�
	TCHAR							m_szRoomName [32];

	DOUBLE							m_dElapse;							//����ʱ��
	CMapControlID					m_MapControlID;						//���������Ӯmap

	BYTE							m_bTempArray[52];
	bool							m_bAndroidControl;						//���ƻ�������Ӯ


	int								m_nZhaDan;					//ը��
	int								m_nShunJin ;				//˳��
	int								m_nJinHua  ;				///��
	int								m_nShunZI  ;				//˳��
	int								m_nDuiZi;					//����
	int								m_nSanPai;					//ɢ��
	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release();
	//�Ƿ���Ч
	virtual bool  IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//�ӿڲ�ѯ
	virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool  Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual void  RepositionSink();
	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){return 0;}
		//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(SCORE lBaseScore){};

	//��Ϣ�ӿ�
public:
	//��ʼģʽ
	virtual BYTE GetGameStartMode();
	//��Ϸ״̬
	virtual bool IsUserPlaying(WORD wChairID);

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);


	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);
	//�û���ע
	virtual bool OnActionUserPour(WORD wChairID, IServerUserItem * pIServerUserItem){return true;}
	//��Ϸ�¼�
protected:
	//�����¼�
	bool OnUserGiveUp(WORD wChairID,bool bExit=false);
	//�����¼�
	bool OnUserLookCard(WORD wChairID);
	//�����¼�
	bool OnUserCompareCard(WORD wFirstChairID,WORD wNextChairID);
	//�����¼�
	bool OnUserOpenCard(WORD wUserID);
	//��ע�¼�
	bool OnUserAddScore(WORD wChairID, SCORE lScore, bool bGiveUp, bool bCompareUser, unsigned char	chciphertext[AESENCRYPTION_LENGTH]);
	//�����¼�
	bool OnUserShowCard(WORD wChairID);
	//�Զ������¼�
	bool OnUserAutoCompareCard(WORD wChairID);

	//���ܺ���
protected:
	//�˿˷���
	void AnalyseStartCard();
	//�Ƿ�˥��
	bool NeedDeductStorage();
	//��ȡ����
	void ReadConfigInformation();
	//���·����û���Ϣ
	void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
	//����ͬ���û�����
	void UpdateUserControl(IServerUserItem *pIServerUserItem);
	//�����û�����
	void TravelControlList(ROOMUSERCONTROL keyroomusercontrol);
	//�Ƿ������������
	void IsSatisfyControl(ROOMUSERINFO &userInfo, bool &bEnableControl);
	//���������û�����
	bool AnalyseRoomUserControl(ROOMUSERCONTROL &Keyroomusercontrol, POSITION &ptList);
	//��ȡ��������
	void GetControlTypeString(CONTROL_TYPE &controlType, CString &controlTypestr);
	//д��־�ļ�
	void WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);
	//�޸Ŀ��
	bool UpdateStorage(SCORE lScore);
	//������Կ
	void RandAESKey(unsigned char chAESKeyBuffer[], BYTE cbBufferCount);
	//�жϻ���Լռ����
	bool IsRoomCardScoreType();
	//�жϽ��Լռ����
	bool IsRoomCardTreasureType();
	//��̽���ж�, ȷ��Chair���û�����Ӯ
	DWORD ProbeJudge(WORD wChairIDCtrl,BYTE cbCheatType);
	//�ƶ�ʤ��
	WORD EstimateWinner();
	//��ȡ��
	bool GetControlCard(BYTE cbControlCardByte[],BYTE cbCardCount);
	//�����
	BYTE GetRandCard(BYTE cbRandCard[],DWORD dwChairID,int &cbIndex,int nRandMin,int nRandMax,bool bFistGetCrad);
	bool getSingBomb(BYTE cbCardData[52],int chairID,BYTE cbSingBomb[]);  //����ը��
	bool getSingSameStraight(BYTE cbCardData[52],int chairID,BYTE SingSameStraight[]);  //����ͬ��˳
};

//////////////////////////////////////////////////////////////////////////

#endif
