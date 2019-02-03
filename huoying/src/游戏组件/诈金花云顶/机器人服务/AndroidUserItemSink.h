#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\��Ϸ������\GameLogic.h"

//////////////////////////////////////////////////////////////////////////



//��Ϸ�Ի���
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//��Ϸ����
protected:
	SCORE						m_lStockScore;							//��ǰ���
	WORD							m_wBankerUser;							//ׯ���û�
	WORD							m_wCurrentUser;							//��ǰ�û�
	BYTE							m_cbCardType;							//�û�����
	int								m_nCurrentRounds;						//��ǰ�ִ�
	int								m_nTotalRounds;							//������

	//��ע��Ϣ
protected:
	bool							m_bMingZhu;								//���ƶ���
	SCORE						m_lCellScore;							//��Ԫ��ע
	SCORE						m_lMaxCellScore;						//�����ע
	SCORE						m_lUserMaxScore;						//������
	SCORE						m_lCurrentTimes;						//��ǰ����
	SCORE						m_lTableScore[GAME_PLAYER];				//��ע��Ŀ
	unsigned char					m_chUserAESKey[AESKEY_TOTALCOUNT];		//��ʼ��Կ
	TCHAR							m_szServerName[LEN_SERVER];				//��������

	//�û�״̬
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE							m_cbRealPlayer[GAME_PLAYER];			//�������
	BYTE							m_cbAndroidStatus[GAME_PLAYER];			//�������

	//�û��˿�
protected:
	BYTE							m_cbHandCardData[MAX_COUNT];			//�û�����
	BYTE							m_cbAllHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�

	//�ؼ�����
public:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;					//�û��ӿ�

	//�����˴�ȡ��
	SCORE						m_lRobotScoreRange[2];					//���Χ
	SCORE						m_lRobotBankGetScore;					//�������
	SCORE						m_lRobotBankGetScoreBanker;				//������� (ׯ��)
	int								m_nRobotBankStorageMul;					//����
	bool							m_bAndroidControl;						//���ƻ�������Ӯ
	bool                            m_bGiveUp;                              //���������ƿ���
	WORD							m_wAndroidPour;							//����������ע����
	WORD							m_wAndroidPourNumber;					//��������ע����
	WORD							m_dwCompareState;						//����״̬

	TCHAR							m_szConfigFileName[MAX_PATH];		//�����ļ�

	int								m_nShunZiBetMin;		//˳����С��ע����
	int								m_nShunZiBetMax;		//˳�������ע����
	int								m_nJInHuaBetMin;		//����С��ע����
	int								m_nJInHuaBetMax;		//�������ע����
	int								m_nShunJinBetMin ;		//˳����С��ע����
	int								m_nShunJinBetMax;		//˳�������ע����
	int								m_nZhaDanBetMin;		//ը����С��ע����
	int								m_nZhaDanBetMax;		//ը�������ע����

	int								m_nSanPaiProbability;	//˳�ӿ��Ƹ���
	int								m_nDuiziProbability;	//�𻨿��Ƹ���
	int								m_nShunZiProbability;	//˳�ӿ��Ƹ���
	int								m_nJInHuaProbability;	//�𻨿��Ƹ���
	int								m_nShunJinProbability;	//˳���Ƹ���
	int								m_nZhaDanProbability;	//ը�����Ƹ���

	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
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
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û���λ
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//��Ϣ����
protected:
	//��������Ϣ
	bool OnSubAndroidCard(const void * pBuffer, WORD wDataSize);
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//�û�����
	bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
	//�û���ע
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//�û�����
	bool OnSubLookCard(const void * pBuffer, WORD wDataSize);
	//�û�����
	bool OnSubCompareCard(const void * pBuffer, WORD wDataSize);
	//�û�����
	bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);
	//�û�ǿ��
	bool OnSubPlayerExit(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//������Կ
	bool OnSubUpdateAESKey(const void * pBuffer, WORD wDataSize);

	//������
private:
	//��ȡ����
	void ReadConfigInformation(tagCustomAndroid *pCustomAndroid);
	//��ȡ����
	void ReadConfigInformation2();
	//���в���
	void BankOperate(BYTE cbType);

	//д��־�ļ�
	void WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString);

	//�߼�����
protected:
	//�ƶ�ʤ��
	WORD EstimateWinner();
	//��ע����
	LONGLONG GetAddScoreTimes();
	bool ControlBottompour();
	//�Ƿ���
	BYTE IsOpenBrand();
};

//////////////////////////////////////////////////////////////////////////

#endif
