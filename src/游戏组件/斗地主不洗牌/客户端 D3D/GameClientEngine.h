#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "HistoryRecord.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CGameClientEngine : public CGameFrameEngine
{
	//��������
protected:
	int								m_bTrusteeCount;					//�йܼ���
	bool							m_bTrustee;							//�йܱ�־
	bool							m_bLastTurn;						//���ֱ�־
	BYTE							m_cbSortType;						//��������
	bool							m_bBackGroundSound;					//��������
	bool							m_bCheatRight;						//�����û�
	bool							m_bPlaySound;						//��������
	BYTE							m_cbGameCount;						//

	//��Ϸ����
protected:
	BYTE							m_cbBombCount;						//ը������
	WORD							m_wBankerUser;						//ׯ���û�
	BYTE							m_cbBankerScore;					//ׯ�ҽз�
	WORD							m_wCurrentUser;						//��ǰ�û�
	WORD							m_wMostCardUser;					//������
	bool							m_bUserCheat[GAME_PLAYER];			//���ױ�־
	TCHAR							m_strAccounts[GAME_PLAYER][LEN_ACCOUNTS];

	//ʱ�䶨��
protected:
	BYTE							m_cbTimeOutCard;					//����ʱ��
	BYTE							m_cbTimeCallScore;					//�з�ʱ��
	BYTE							m_cbTimeStartGame;					//��ʼʱ��
	BYTE							m_cbTimeHeadOutCard;				//�׳�ʱ��

	//��������
protected:
	BYTE							m_cbSearchResultIndex;				//�����������
	tagSearchCardResult				m_SearchCardResult;					//�������

	int								m_nCurSearchType;					//��ǰ����״̬
	BYTE							m_cbEachSearchIndex;				//������������
	tagSearchCardResult				m_EachSearchResult;					//�����������

	//���Ʊ���
protected:
	BYTE							m_cbTurnCardCount;					//������Ŀ
	BYTE							m_cbTurnCardData[MAX_COUNT];		//�����б�

	//�˿˱���
protected:
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
	BYTE							m_cbHandCardCount[GAME_PLAYER];		//�˿���Ŀ

	//�������
protected:
	CHistoryCard					m_HistoryCard;						//��ʷ�˿�
	CHistoryScore					m_HistoryScore;						//��ʷ����

	//�������
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
public:
	CGameClientView					m_GameClientView;					//��Ϸ��ͼ

	//��������
public:
	//���캯��
	CGameClientEngine();
	//��������
	virtual ~CGameClientEngine();

	//ϵͳ�¼�
public:
	//��������
	virtual bool OnInitGameEngine();
	//���ú���
	virtual bool OnResetGameEngine();

	//ʱ���¼�
public:
	//ʱ��ɾ��
	virtual bool OnEventGameClockKill(WORD wChairID);
	//ʱ����Ϣ
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);
	//��������
	virtual bool AllowBackGroundSound(bool bAllowSound);

	//��Ϸ�¼�
public:
	//�Թ���Ϣ
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);

	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	//�û��з�
	bool OnSubCallScore(VOID * pData, WORD wDataSize);
	//ׯ����Ϣ
	bool OnSubBankerInfo(VOID * pData, WORD wDataSize);
	//�û�����
	bool OnSubOutCard(VOID * pData, WORD wDataSize);
	//�û�����
	bool OnSubPassCard(VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameConclude(VOID * pData, WORD wDataSize);
	//��������
	bool OnSubCheatCard(VOID * pData, WORD wDataSize);
	

	//���ܺ���
public:
	//�����ж�
	bool VerdictOutCard();
	//�Զ���ʼ
	bool PerformAutoStart();
	//��ǰ�˿�
	bool SwitchToCurrentCard();
	//�Զ�����
	bool AutomatismOutCard();
	//���°�ť
	VOID UpdateButtonControl();
	//���¿�ʼ
	void PlayAgain();
	//���ش���
	void ExitGame();
	//�ƶ������
	void MoveSoreControl(INT nXOffset,INT nYOffset);

	//��������
public:
	//��������
	VOID PlayOutCardSound(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount);

	//�û��¼�
public:
	//�û�״̬
	virtual VOID OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser);

	//��Ϣӳ��
protected:
	//ʱ����Ϣ
	VOID OnTimer(UINT nIDEvent);
	//��ʼ��Ϣ
	LRESULT OnMessageStart(WPARAM wParam, LPARAM lParam);
	//������Ϣ
	LRESULT OnMessageOutCard(WPARAM wParam, LPARAM lParam);
	//PASS��Ϣ
	LRESULT OnMessagePassCard(WPARAM wParam, LPARAM lParam);
	//��ʾ��Ϣ
	LRESULT OnMessageOutPrompt(WPARAM wParam, LPARAM lParam);
	//�з���Ϣ
	LRESULT OnMessageCallScore(WPARAM wParam, LPARAM lParam);
	//�Ҽ��˿�
	LRESULT OnMessageLeftHitCard(WPARAM wParam, LPARAM lParam);
	//�����˿�
	LRESULT OnMessageSortHandCard(WPARAM wParam, LPARAM lParam);
	//�鿴����
	LRESULT OnMessageLastTurnCard(WPARAM wParam, LPARAM lParam);
	//�Ϲܿ���
	LRESULT OnMessageTrusteeControl(WPARAM wParam, LPARAM lParam);
	//��������
	LRESULT OnMessageSearchCard(WPARAM wParam, LPARAM lParam);
	//��Ϸ����
	LRESULT OnMessageGameSound(WPARAM wParam, LPARAM lParam);
	//��Ϸ�ر�
	LRESULT OnMessageGameClose(WPARAM wParam, LPARAM lParam);
public:
	//������Ϣ
	LRESULT OnChatMessage(WPARAM wParam, LPARAM lParam);

	//������Ϣ
protected:
	//��������
	LRESULT OnMessageEnableHistory(WPARAM wParam, LPARAM lParam);
	//�������
	LRESULT OnMessageOutCardFinish(WPARAM wParam, LPARAM lParam);
	//�������
	LRESULT OnMessageDispatchFinish(WPARAM wParam, LPARAM lParam);
	//�������
	LRESULT OnMessageReversalFinish(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif