#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "CardControl.h"
#include "ScoreControl.h"
#include "HistoryRecord.h"
#include "ChatEdit.h"
#include "ChatMessage.h"
//////////////////////////////////////////////////////////////////////////////////

//��ͼλ��
#define MYSELF_VIEW_ID				1									//��ͼλ��

//������Ϣ
#define IDM_START					(WM_USER+200)						//��ʼ��Ϣ
#define IDM_OUT_CARD				(WM_USER+201)						//������Ϣ
#define IDM_PASS_CARD				(WM_USER+202)						//PASS��Ϣ
#define IDM_OUT_PROMPT				(WM_USER+203)						//��ʾ��Ϣ
#define IDM_CALL_SCORE				(WM_USER+204)						//�з���Ϣ
#define	IDM_LEFT_HIT_CARD			(WM_USER+205)						//����˿�
#define IDM_SORT_HAND_CARD			(WM_USER+206)						//�����˿�
#define IDM_LAST_TURN_CARD			(WM_USER+207)						//�����˿�
#define IDM_TRUSTEE_CONTROL			(WM_USER+208)						//�йܿ���
#define IDM_GAME_SOUND				(WM_USER+209)						//��Ϸ����
#define IDM_GAME_CLOSE				(WM_USER+210)						//��Ϸ�ر�

//������Ϣ
#define IDM_DISPATCH_FINISH			(WM_USER+300)						//�������
#define IDM_REVERSAL_FINISH			(WM_USER+301)						//�������

#define IDM_SEARCH_CARD				(WM_USER+320)						//��������

#define SEARCH_MISSILE				0									//�������
#define SEARCH_BOMB					1									//����ը��
#define SEARCH_THREE_TOW_LINE		2									//�����ɻ�
#define SEARCH_DOUBLE_LINE			3									//����˫˳
#define SEARCH_SINGLE_LINE			4									//������˳
#define SEARCH_THREE_TWO_ONE		5									//��������N

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ��ͼ
class CGameClientView : public CGameFrameViewD3D
{
	//״̬����
public:
	bool							m_bCellScoreEnd;					//ͷ��״̬
	bool							m_bGameEnd;							//��Ϸ����
	bool							m_bShowScore;						//����״̬
	bool							m_bLastTurnState;					//���ֱ�־
	bool							m_bWaitCallScore;					//�ȴ��з�
	bool							m_bUserPass[GAME_PLAYER];			//������־
	bool							m_bUserCheat[GAME_PLAYER];			//���ױ�־
	tagHistoryScore	*				m_pHistoryScore[GAME_PLAYER];		//������Ϣ
	double							m_dTimeScale;						//ʱ��֡������
	BYTE							m_cbSenceIndex;						//��������
	bool							m_bPlaySound;						//��Ϸ����
	bool							m_bUserTrustee[GAME_PLAYER];		//�йܱ�־
	bool							m_bChangeTimePos;
	WORD							m_wMessageLen;						//��Ϣ����
	bool							m_bShowChatMessage;					//�鿴��Ϣ


	//��Ϸ����
protected:
	LONG							m_lCellScore;						//��Ԫ����
	BYTE							m_cbBombCount;						//ը������
	BYTE							m_cbCallScore[GAME_PLAYER];			//�з���Ϣ
	tagScoreInfo					m_ScoreInfo;						//������Ϣ

	//ׯ�ұ���
protected:
	WORD							m_wBankerUser;						//ׯ���û�
	BYTE							m_cbBankerScore;					//ׯ�ҽз�

	//�ƶ�����
protected:
	bool							m_bMoveMouse;						//�ƶ���־
	bool							m_bSwitchCard;						//������־
	bool							m_bSelectCard;						//ѡ���־
	BYTE							m_cbHoverCardItem;					//��������
	BYTE							m_cbMouseDownItem;					//�������
	BYTE							m_cbSwitchCardItem;					//��������

	//λ�ñ���
protected:
	CPoint							m_ptPass[GAME_PLAYER];				//����λ��
	CPoint							m_ptBanker[GAME_PLAYER];			//ׯ��λ��
	CPoint							m_ptCountWarn[GAME_PLAYER];			//����λ��
	CPoint							m_ptCallScore[GAME_PLAYER];			//�з�λ��
	CPoint							m_ptOrderFlag[GAME_PLAYER];			//�ȼ�λ��
	CPoint							m_ptChatBubble[GAME_PLAYER];		//����λ��
	CPoint							m_ptMachine[GAME_PLAYER];			//�й�λ��

	//��ը����
protected:
	bool							m_bCartoonBomb;						//������־
	WORD							m_wCartoonBombIndex;				//֡������
	CLapseCount						m_CartoonBombLapseCount;			//���ż���

	//�ɻ�����
protected:
	bool							m_bCartoonPlan;						//������־
	WORD							m_wCartoonPlanIndex;				//֡������
	CPoint							m_ptCartoonPlan;					//�ɻ�λ��
	CLapseCount						m_CartoonPlanLapseCount;			//���ż���

	//�������
protected:
	bool							m_bCartoonRocket;					//������־
	WORD							m_wCartoonRocketIndex;				//֡������
	CPoint							m_ptCartoonRocket;					//���λ��
	CLapseCount						m_CartoonRocketLapseCount;			//���ż���

	//˳�Ӷ���
protected:
	bool							m_bCartoonLine;						//������־
	WORD							m_wCartoonLineIndex;				//֡������
	CLapseCount						m_CartoonLineLapseCount;			//���ż���

	//���ƶ���
protected:
	bool							m_bReversalCard;					//������־
	WORD							m_wReversalCardIndex;				//֡������
	CLapseCount						m_ReversalCardLapseCount;			//���ż���

	//ׯ�Ҷ���
protected:
	WORD							m_wBankerMovieIndex;				//֡������
	CLapseCount						m_BankerMovieLapseCount;			//���ż���

	//���ƶ���
protected:
	bool							m_bRiffleCard;						//������־
	bool							m_bDispatchCard;					//������־
	WORD							m_wDispatchCardIndex;				//֡������
	CLapseCount						m_DispatchCardLapseCount;			//���ż���

	//��������
protected:
	WORD							m_wStartUser;						//��ʼ�û�
	BYTE							m_cbHandCardData[GAME_PLAYER][NORMAL_COUNT];	//��������

	//��������
protected:
	bool							m_bCountWarn[GAME_PLAYER];			//������־
	WORD							m_wCountWarnIndex[GAME_PLAYER];		//֡������
	CLapseCount						m_CountWarnLapseCount[GAME_PLAYER];	//���ż���

	//��Ϸ��ť
public:
	CVirtualButton					m_btScore;							//���ְ�ť
	CVirtualButton					m_btStart;							//��ʼ��ť
	CVirtualButton					m_btOutCard;						//���ư�ť
	CVirtualButton					m_btPassCard;						//PASS��ť
	CVirtualButton					m_btOutPrompt;						//��ʾ��ť
	CVirtualButton					m_btPlayAgain;						//����һ��
	CVirtualButton					m_btReturn;							//���ش���
	CVirtualButton					m_btClose;							//�رհ�ť
	CVirtualButton					m_btMin;							//��С��ť
	CVirtualButton					m_btSound;							//������ť
	CVirtualButton					m_btHistoryChat;					//�����¼

	//���ܰ�ť
public:
	CVirtualButton					m_btCancelTrustee;					//ȡ���й�
	CVirtualButton					m_btTrustee;						//�йܰ�ť
	CVirtualButton					m_btLastTurn;						//���ְ�ť
	CVirtualButton					m_btSortCard;						//����ť
//	CVirtualButton					m_btExpression;						//���鰴ť
//	CVirtualButton					m_btVoice;							//������ť
	CVirtualButton					m_btEnter;							//�س���ť
//	CVirtualButton					m_btSwitchSence;					//�л�����

	//�зְ�ť
public:
	CVirtualButton					m_btCallScore1;						//�зְ�ť
	CVirtualButton					m_btCallScore2;						//�зְ�ť
	CVirtualButton					m_btCallScore3;						//�зְ�ť
	CVirtualButton					m_btCallScoreNone;					//�зְ�ť

	//������Դ
protected:
	CD3DTexture						m_TextureBack;						//������Դ
	CD3DTexture						m_TextureBackButtom;				//������Դ
	CD3DTexture						m_TextureUserPlayer;				//�û����
	CD3DTexture						m_TextureUserPlayerGame;			//�û����
	CD3DTexture						m_TextureUserMySelf;				//�û����
	CD3DTexture						m_TextureGameScoreBack;				//���㱳��
	CD3DTexture						m_TextureChatBubble;				//��������

	//��Դ����
protected:
	CD3DSprite						m_TextureBankerFlag[GAME_PLAYER];	//ׯ�ұ�־
	CD3DTexture						m_TextureCellNumber;				//��Ϣ����
	CD3DTexture						m_TextureBankerCardBack;			//��Դ����
	CD3DTexture						m_TextureRemnantCardNumber;			//��Դ����
	CD3DTexture						m_TextureTimeBack;					//ʱ�䱳��
	CD3DTexture						m_TextureTimeNumber;				//ʱ������
	CD3DFontEx						m_FontChat;							//��������
	CD3DTexture						m_TextureCardCountFrame1;			//���ƿ��
	CD3DTexture						m_TextureCardCountFrame2;			//���ƿ��
	CD3DTexture						m_TextureChatBack;					//���챳��	
	CFont							m_MyFont;

	//��̬��Դ
protected:
	CD3DTexture						m_TextureUserPass;					//������־
	CD3DTexture						m_TextureLastTurn;					//���ֱ�־
	CD3DTexture						m_TextureCallScore;					//�û��з�
	CD3DTexture						m_TextureCartoonBomb;				//״̬��Ϣ
	CD3DTexture						m_TextureCartoonPlan;				//�ɻ�����
	CD3DTexture						m_TextureMemberOrder;				//�ɻ�����
	CD3DTexture						m_TextureCartoonRocket;				//�������
	CD3DTexture						m_TextureRocket;					//�������
	CD3DTexture						m_TextureRocketLight;				//�������
	CD3DTexture						m_TextureDispatchCard;				//��Դ����
	CD3DTexture						m_TextureHistoryScore;				//��Դ����
	CD3DTexture						m_TextureReversalCard;				//��Դ����
	CD3DTexture						m_TextureWaitCallScore;				//�ȴ��з�
	CD3DTexture						m_TextureCartoonTime;				//ʱ�䶯��
	CD3DTexture						m_TextureCartoonLine;				//˳�Ӷ���
	CD3DTexture						m_TextureLine;						//˳�Ӷ���
	CD3DTexture						m_TextureWarn[5];					//��������
	CD3DTexture						m_TextureTimeLight;					//ʱ�䱳��
	CD3DTexture						m_TextureMachine;					//�йܱ�־

	

	//�ؼ�����
public:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	CScoreControl					m_ScoreControl;						//���ֿؼ�

	//�������
public:
	CEdit							m_ChatInput;							//��������
	CString							m_strChat[GAME_PLAYER];					//��������
	WORD							m_ChatChair[GAME_PLAYER];				//�����û�
	INT								m_nChatStrlen[GAME_PLAYER];
	DWORD							m_dwChatTime;
	CChatMessage					m_ChatMessage;

	//�˿˿ؼ�
public:
	CCardControl					m_BackCardControl;					//�����˿�
	CCardControl					m_HandCardControl[GAME_PLAYER];		//�û��˿�
	CCardControl					m_UserCardControl[GAME_PLAYER];		//�˿���ͼ
	CCardControl					m_ChetCardContol[GAME_PLAYER];
protected:
	CBrush							m_brush;							//������ˢ

	//��������
public:
	//���캯��
	CGameClientView();
	//��������
	virtual ~CGameClientView();

public:
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//���غ���
private:
	//���ý���
	virtual VOID ResetGameView();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//�豸�¼�
public:
	//��ʧ�豸
	virtual VOID OnLostDevice(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);
	//�����豸
	virtual VOID OnResetDevice(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);

	//���溯��
protected:
	//��������
	virtual VOID CartoonMovie();
	//�����豸
	virtual VOID InitGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);
	//�滭����
	virtual VOID DrawGameView(CD3DDevice * pD3DDevice, INT nWidth, INT nHeight);

	//ׯ������
public:
	//����ׯ��
	VOID SetBankerUser(WORD wBankerUser);
	//���ñ���
	VOID SetBankerScore(BYTE cbBankerScore);
	//��ʾ����
	VOID ShowBankerCard(BYTE cbCardData[3]);

	//���ܺ���
public:
	//��Ԫ����
	VOID SetCellScore(LONG lCellScore);
	//ը������
	VOID SetBombCount(BYTE cbBombCount);
	//���ý���
	VOID ShowGameScoreInfo(tagScoreInfo & ScoreInfo,bool bGameEnd);
	// �滭����
	void DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// �滭����
	void DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// �滭����
	void DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat = DT_LEFT);
	//�滭ʱ��
	void DrawUserTimerEx(CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, WORD wUserClock);
	//��ʾ����
	void ShowChatMessage(LPCTSTR strMessage,LPCTSTR UserName);
	//���ý�����λ��
	void SetScoreControlPos(INT nXOffset,INT nYOffset);
	//����Ч��
	bool EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex);
	//�鿴����
	void OnHideChatMessage();
	// ��Ӷ���
	CString AddComma( LONGLONG lScore , bool bPlus = false);


	//���Ͷ���
public:
	//�ɻ�����
	VOID ShowCartoonPlan(bool bCartoon);
	//��ը����
	VOID ShowCartoonBomb(bool bCartoon);
	//�������
	VOID ShowCartoonRocket(bool bCartoon);
	//˳�Ӷ���
	VOID ShowCartoonLine(bool bCartoon,BYTE cbCardType);

	//���ƺ���
public:
	//�û�����
	VOID UserValidCard(WORD wChairID, BYTE cbCardData[MAX_COUNT], BYTE cbCardCount = 17);

	//״̬����
public:
	//��������
	VOID SetLastTurnState(bool bLastTurnState);
	//�ȴ��з�
	VOID SetWaitCallScore(bool bWaitCallScore);
	//���÷���
	VOID SetUserPassState(WORD wChairID, bool bUserPass);
	//���þ���
	VOID SetUserCountWarn(WORD wChairID, bool bCountWarn);
	//���ýз�
	VOID SetUserCallScore(WORD wChairID, BYTE cbCallScore);
	//���û���
	VOID SetHistoryScore(WORD wChairID, tagHistoryScore * pHistoryScore);
	//���ó���
	VOID SetUserAvatar(bool bCellScoreEnd);
	//���ñ���
	VOID SetTimeScale(WORD wTime);
	//�й�״̬
	VOID SetUserTrustee(WORD wChairID,bool bTrustee){m_bUserTrustee[wChairID] = bTrustee;}
	//���¿�ʼ
	VOID SetChangeTimePos(bool bChange){m_bChangeTimePos = bChange;}

	//���ƺ���
public:
	//���ƶ���
	VOID StopDispatchCard();
	//���ƶ���
	VOID ShowDispatchCard(BYTE cbCardData[NORMAL_COUNT], WORD wStartUser);

	//��ť��Ϣ
public:
	//��ʼ��ť
	VOID OnBnClickedStart();
	//���ְ�ť
	VOID OnBnClickedScore();
	//���ư�ť
	VOID OnBnClickedOutCard();
	//����ť
	VOID OnBnClickedSortCard();
	//���ְ�ť
	VOID OnBnClickedLastTurn();
	//�йܰ�ť
	VOID OnBnClickedTrustee();
	//�йܰ�ť
	VOID OnBnClickedCancelTrustee();
	//PASS��ť
	VOID OnBnClickedPassCard();
	//��ʾ��ť
	VOID OnBnClickedOutPrompt();
	//һ�ְ�ť
	VOID OnBnClickedCallScore1();
	//���ְ�ť
	VOID OnBnClickedCallScore2();
	//���ְ�ť
	VOID OnBnClickedCallScore3();
	//������ť
	VOID OnBnClickedCallScoreNone();
	//���鰴ť
	VOID OnBnClickedExpression();
	//�س���ť
	VOID OnBnClickedEnter();
	//������ť
	VOID OnBnClickedVoice();
	//�л�������ť
	VOID OnBnClickedSwitchSence();

	//���غ���
public:
	//��Ϣ����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//��Ϣ����
public:
	//�����Ϣ
	VOID OnRButtonUp(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	VOID OnLButtonUp(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	VOID OnMouseMove(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	VOID OnLButtonDblClk(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//�ؼ���ɫ
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();


	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif