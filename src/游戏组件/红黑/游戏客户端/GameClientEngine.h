#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//��Ϸ�Ի���
class CGameClientEngine : public CGameFrameEngine
{
	//��Ԫ����
	friend class CGameClientView;

	//������Ϣ
protected:
	LONGLONG						m_lPlayBetScore;					//��������ע
	LONGLONG						m_lPlayFreeSocre;					//������ɻ���
	LONGLONG						m_lInitUserScore[GAME_PLAYER];		//ԭʼ����

	//��ע��
protected:
	LONGLONG						m_lAllBet[AREA_MAX];				//����ע
	LONGLONG						m_lPlayBet[AREA_MAX];				//�����ע
	LONGLONG						m_lPlayScore[AREA_MAX];				//�����Ӯ

	//ׯ����Ϣ
protected:
	WORD							m_wBankerUser;						//��ǰׯ��
	LONGLONG						m_lBankerScore;						//ׯ�һ���
	LONGLONG						m_lBankerWinScore;					//ׯ��Ӯ��
	WORD							m_wBankerTime;						//ׯ�Ҿ���
	bool							m_bEnableSysBanker;					//ϵͳ��ׯ	

	//״̬����
protected:
	bool							m_bMeApplyBanker;					//�����ʶ
	bool							m_bBackGroundSound;					//��������
	bool							m_bPlaySound;

	//������Ϣ
protected:
	LONGLONG						m_lAreaLimitScore;					//��������
	LONGLONG						m_lApplyBankerCondition;			//��������

	//�ؼ�����
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	CGameClientView					m_GameClientView;					//��Ϸ��ͼ

	//������Դ
protected:
	DWORD							m_dwBackID;							//��������ID

	//��������
	CWHArray<CMD_S_PlaceBet,CMD_S_PlaceBet> m_PlaceBetArray;

	//��������
public:
	//���캯��
	CGameClientEngine();
	//��������
	virtual ~CGameClientEngine();

	//����̳�
private:
	//��ʼ����
	virtual bool OnInitGameEngine();
	//���ÿ��
	virtual bool OnResetGameEngine();
	//�Թ�״̬
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//ʱ��ɾ��
	virtual bool OnEventGameClockKill(WORD wChairID);
	//ʱ����Ϣ
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);
	//������Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��Ϸ����
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);
	//��������
	virtual bool AllowBackGroundSound(bool bAllowSound);

	//�û��¼�
public:
	//�û�����
	virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser);
	//�û��뿪
	virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser);
	//�û�����
	virtual VOID OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser);
	//�û�״̬
	virtual VOID OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser);
	//�û���λ
	virtual VOID OnEventUserSegment(IClientUserItem * pIClientUserItem, bool bLookonUser){};
	//�û�ͷ��
	virtual VOID OnEventCustomFace(IClientUserItem * pIClientUserItem, bool bLookonUser){};

	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameFree(const void * pBuffer, WORD wDataSize);
	//�û���ע
	bool OnSubPlaceBet(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//������ׯ
	bool OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize);
	//ȡ����ׯ
	bool OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize);
	//�л�ׯ��
	bool OnSubChangeBanker(const void * pBuffer, WORD wDataSize);
	//��Ϸ��¼
	bool OnSubGameRecord(const void * pBuffer, WORD wDataSize);
	//��עʧ��
	bool OnSubPlaceBetFail(const void * pBuffer, WORD wDataSize);
	//������
	bool OnSubReqResult(const void * pBuffer, WORD wDataSize);
	//���¿��
	bool OnSubUpdateStorage(const void * pBuffer, WORD wDataSize);
	//������ע
	bool OnSubSendUserBetInfo(const void * pBuffer, WORD wDataSize);
	//�����û��б�
	void UpdateUserList();

	//���ܺ���
protected:
	//���¿���
	void UpdateButtonContron();	

	//��Ϣӳ��
protected:
	//��ע��Ϣ
	LRESULT OnPlayBet(WPARAM wParam, LPARAM lParam);
	//������Ϣ
	LRESULT OnApplyBanker(WPARAM wParam, LPARAM lParam);
	//������Ϣ
	LRESULT OnPlaySound(WPARAM wParam, LPARAM lParam);
	//����Ա����
	LRESULT OnAdminCommand(WPARAM wParam, LPARAM lParam);
	//���¿��
	LRESULT OnUpdateStorage(WPARAM wParam, LPARAM lParam);
	//����
	LRESULT  OnGameSetting( WPARAM wParam, LPARAM lParam );
	//����
	LRESULT  OnChatMessage( WPARAM wParam, LPARAM lParam );
	//��Ϸ����
	LRESULT OnMessageGameSound(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
};

//////////////////////////////////////////////////////////////////////////
