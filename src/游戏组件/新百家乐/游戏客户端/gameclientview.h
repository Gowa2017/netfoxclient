#ifndef GAME_CLIENT_VIEW_HEAD_FILE
#define GAME_CLIENT_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "RecordGameList.h"
#include "GameLogic.h"
#include "DlgViewChart.h"
#include "ClientControl.h"
#include "MessageTipDlg.h"
#include "SkinListCtrl.h"
#include "MyEdit.h"
#include "BankTipsDlg.h"
#include "SkinButtonEx.h"
//////////////////////////////////////////////////////////////////////////

//���붨��
#define BET_COUNT					6									//������Ŀ
#define JETTON_RADII				68									//����뾶

//��Ϣ����
#define IDM_PALY_BET				WM_USER+200							//��ס��Ϣ
#define IDM_APPLY_BANKER			WM_USER+201							//������Ϣ
#define IDM_PLAY_SOUND				WM_USER+202							//������Ϣ
#define IDM_GAME_OPTIONS			WM_USER+203							//��Ϸ������Ϣ	
#define IDM_CHAT_MESSAGE			WM_USER+208							//��Ϸ������Ϣ	
#define WM_SET_CAPTION				WM_USER+209
#define IDM_GAME_SOUND				WM_USER+210							//��Ϸ����

//��������
#define INDEX_PLAYER				0									//�м�����
#define INDEX_BANKER				1									//ׯ������


//·��ˮƽ����
#define NORMAL_RECORD_COL_COUNT      27								// ����ģʽ�¼�¼��·������
#define ZOOMED_RECORD_COL_COUNT      40								// ȫ��ģʽ�¼�¼��·������

//��¼��Ŀ
#define MAX_RECORD_COL_COUNT            80                         // ����¼��·������
#define MAX_RECORD_SUM					250                        // ����¼·������
//////////////////////////////////////////////////////////////////////////
//�ṹ����

//������Ϣ
struct tagBetInfo
{
	int								nXPos;								//����λ��
	int								nYPos;								//����λ��
	BYTE							cbBetIndex;							//��������
};

//ѡȡ������Ϣ
struct tagFlashInfo
{
	BYTE							cbFlashArea;					//��ʾ����
	int								nFlashAreaAlpha;				//��ʾ͸����
	bool							bFlashAreaAlpha;				//�Ƿ���ʾ
};

enum enFlexMode
{
	enFlexNULL,
	enFlexBetTip,
	enFlexDealCrad,
	enFlexGameEnd,
};

//��������
typedef CWHArray<tagBetInfo,tagBetInfo&> CBetInfoArray;

struct tagRecordResult
{
	int resultIndex[6];
};

typedef CWHArray<tagRecordResult> RECORDRESULT;

//////////////////////////////////////////////////////////////////////////

//��Ϸ��ͼ
class CGameClientView : public CGameFrameViewGDI
{
	//������Ϣ
protected:
	LONGLONG						m_lMeMaxScore;						//�����ע
    LONGLONG						m_lAreaLimitScore;					//��������


	//��ע��
protected:
	LONGLONG						m_lAllBet[AREA_MAX];				//����ע
	LONGLONG						m_lPlayBet[AREA_MAX];				//�����ע
	LONGLONG						m_lAllPlayBet[GAME_PLAYER][AREA_MAX];//���������ע

	//λ����Ϣ
protected:
	CWHArray<CRect>					m_ArrayBetArea[AREA_MAX];			//��ע����
	CPoint							m_ptBetFrame[AREA_MAX];				//��ע��ʾλ��
	CSize							m_sizeWin;							//���ڴ�С
	BYTE							m_cbJetArea;						//��ע����
	bool							m_bFlashResult;						//��ʾ���

	//�˿���Ϣ
protected:	
	BYTE							m_cbCardCount[2];					//�˿���Ŀ
    BYTE							m_cbTableCardArray[2][3];			//�����˿�

	//��������
protected:
	int								m_nDealMove;						//���Ʋ���
	int								m_nDealIndex;						//��������
	CPoint							m_ptDispatchCard;					//����λ��
	CCardControl					m_DispatchCard;						//�����˿�

	//��������
protected:
	enFlexMode						m_enFlexMode;						//��������
	int								m_nFlexMove;						//��������
	bool							m_bFlexShow;						//��ʾ
	CPoint							m_ptFlexBeing;						//�ƶ�λ��
	CPoint							m_ptFlexMove;						//�ƶ�λ��
	CPngImage*						m_pImageFlex;						//�ƶ�ͼƬ

	//��ʷ��Ϣ
protected:
	LONGLONG						m_lMeStatisticScore;				//��Ϸ�ɼ�
	tagClientGameRecord				m_GameRecordArrary[MAX_RECORD_SUM]; //��Ϸ��¼
	int								m_nRecordFirst;						//��ʼ��¼
	int								m_nRecordLast;						//����¼
	int								m_nRecordHead;						//

	//״̬����
protected:
	LONG							m_nWinCount;						//Ӯ����
	LONG							m_nLoseCount;						//������
	BYTE							m_cbGameStatus;						//��Ϸ״̬
	WORD							m_wMeChairID;						//�ҵ�λ��
	LONGLONG						m_lCurrentBet;						//��ǰ����
	CString							m_strDispatchCardTips;				//������ʾ
	bool							m_bShowChangeBanker;				//�ֻ�ׯ��
	bool							m_bNeedSetGameRecord;				//�������
	bool							m_bEnableBet;

	//������
protected:
	CWHArray<BYTE>					m_ArrayFlashArea;					//ʤ�����
	int								m_nFlashAreaAlpha;					//ʤ����ʾ
	bool							m_bFlashAreaAlpha;					//ʤ����ʾ
	CWHArray<tagFlashInfo>			m_ArrayCurrentFlash;				//ѡ������

	//�ײ���Ϸ��ʷ��¼
protected:
	tagClientGameRecord				m_LefeGameRecordArray[MAX_RECORD_SUM];		//·����¼
	tagClientGameRecord				m_RightGameRecordArray[MAX_RECORD_SUM];		//�����¼
	int								m_LefeGameRecordCount;				//·����Ŀ
	int								m_RightGameRecordCount;				//��¼��Ŀ
	bool							m_bFillTrace[6][MAX_RECORD_SUM];		//����ʶ
	CPngImage						m_PngPlayerFlag;					//�м�ͼƬ
	CPngImage						m_PngPlayerFlagTowPair;
	CPngImage						m_PngPlayerEXFlag;					//�м�ͼƬ
	CPngImage						m_PngPlayerEXFlagTowPair;
	CPngImage						m_PngBankerFlag;					//ׯ��ͼƬ
	CPngImage						m_PngBankerFlagTowPair;
	CPngImage						m_PngBankerEXFlag;					//ׯ��ͼƬ
	CPngImage						m_PngBankerEXFlagTowPair;
	CPngImage						m_PngTieFlag;						//ƽ��ͼƬ
	CPngImage						m_PngTieEXFlag;						//ƽ��ͼƬ
	CPngImage						m_PngTwopairFlag;					//ƽ��ͼƬ
	int								m_nScoreHead;						//�ɼ�λ��
	int								m_nHeadCol;							//��¼����
	int								m_nTotalCol;						//��¼����

	RECORDRESULT					m_RecordResult;
	//ׯ����Ϣ
protected:	
	WORD							m_wBankerUser;						//��ǰׯ��
	WORD							m_wBankerTime;						//��ׯ����
	LONGLONG						m_lBankerScore;						//ׯ�һ���
	LONGLONG						m_lBankerWinScore;					//ׯ�ҳɼ�	
	LONGLONG						m_lBankerWinTempScore;				//ׯ�ҳɼ�[��ʱ����]
	bool							m_bEnableSysBanker;					//ϵͳ��ׯ	

	//���ֳɼ�
public:
	LONGLONG						m_lPlayScore[AREA_MAX];				//�����Ӯ
	LONGLONG						m_lPlayAllScore;					//�������Ӯ
	LONGLONG						m_lRevenue;							//����˰��
	LONGLONG						m_lBankerCurGameScore;				//����ׯ�ҵ÷�

	//��������
public:
	int								m_nWinShowArea;						//Ӯ��������
	int								m_nWinShowIndex[3];					//Ӯ��������

	//���ݱ���
protected:
	CPoint							m_ptBetNumber[AREA_MAX];			//����λ��
	CPoint							m_ptAllBetNumber[AREA_MAX];			//����λ��
	CBetInfoArray					m_BetInfoArray[AREA_MAX];			//��������
	CRect							m_rcXian;							//�м�����
	CRect							m_rcPing;							//ƽ����
	CRect							m_rcZhuang;							//ׯ������
	CRect							m_rcXianTian;						//����������
	CRect							m_rcZhuangTian;						//ׯ��������
	CRect							m_rcTongDui;						//ͬ��ƽ����
	CRect							m_rcXianDui;							//�ж�������
	CRect							m_rcZhuangDui;							//ׯ��������
	CRect						    m_RectUserList;						//�б�λ��
	CRect						    m_RectChartDisplay;					//����λ��
	
	

	//�ؼ�����
public:
	CSkinButton						m_btBet100;							//���밴ť
	CSkinButton						m_btBet1000;						//���밴ť
	CSkinButton						m_btBet10000;						//���밴ť
	CSkinButton						m_btBet100000;						//���밴ť
	CSkinButton						m_btBet1000000;						//���밴ť
	CSkinButton						m_btBet5000000;						//���밴ť
	CSkinButton						m_btBet10000000;					//���밴ť
	CSkinButton						m_btViewChart;						//�鿴·��
	
	CSkinButtonEx						m_btApplyBanker;					//����ׯ��
	CSkinButtonEx						m_btCancelBanker;					//ȡ��ׯ��

	CSkinButton						m_btScoreMoveL;						//�ƶ��ɼ�
	CSkinButton						m_btScoreMoveR;						//�ƶ��ɼ�

	CSkinButton						m_btValleysUp;						//��ť
	CSkinButton						m_btValleysDown;					//��ť
	CSkinButton						m_btOption;							//���ð�ť
	CSkinButton						m_btMin;							//��С����ť
	CSkinButton						m_btClose;							//�رհ�ť
	CSkinButton						m_btMax;							//��󻯰�ť
	CSkinButton						m_btRestore;						//�ָ���ť
	CSkinButton						m_btBank;							//���а�ť
	CSkinButton						m_btSound;							//������ť
	CSkinButton						m_btSend;							//��������

	CButton							m_btOpenAdmin;


	//������Դ
public:
	CDFontEx						m_FontValleysListOne;				//��ׯ�б�����һ
	CDFontEx						m_FontValleysListTwo;				//��ׯ�б������
	CDFontEx						m_FontCaption;
	CString							m_strCaption;

	//�ؼ�����
public:
	INT								m_nShowValleyIndex;					//��ʾ��ׯ�б�
	CWHArray<WORD>					m_ValleysList;						//��ׯ�б�			
	CCardControl					m_CardControl[2];					//�˿˿ؼ�	
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	CDlgViewChart					m_DlgViewChart;						//�鿴·��
	CSkinListCtrlEx                 m_UserList;							//�б�ؼ�
	CMyEdit							m_ChatDisplay;						//��ʾ�ؼ�
	CEdit							m_ChatInput;						//��������
	CBrush							m_brush;							//��ˢ����
	DWORD							m_dwChatTime;

	//�������
protected:
	CBitImage						m_ImageViewFill;
	CPngImage						m_ImageHistoryMid;
	CPngImage						m_ImageHistoryFront;
	CPngImage						m_ImageHistoryBack;

	CPngImage						m_ImageViewBack;					//����λͼ
	CPngImage						m_ImageChipInfo;					//������ע��Ϣ
	CPngImage						m_ImagePlayerResultInfo;               
	CPngImage						m_ImageBankerResultInfo;                 //����ׯ����Ϣ

	CPngImage						m_ImageViewBackJB;					//����λͼ
	CPngImage						m_ImageWinFlags;					//��־λͼ
	CPngImage						m_ImageBetView;						//������ͼ
	CPngImage						m_ImageScoreBack;					//������ͼ
	CPngImage						m_ImageScoreNumber;					//������ͼ
	CPngImage						m_ImageMeScoreBack;					//������ͼ
	CPngImage						m_ImageMeScoreNumber;				//������ͼ
	CPngImage						m_ImgaeGameTimeCount;				//�������򿪳����Ĵ���


	//�߿���Դ
protected:
	CPngImage						m_ImageFrame[AREA_MAX];				//�߿�ͼƬ
	CPngImage						m_ImageGameFrame[8];				//��Ϸ���	
	CPngImage						m_ImageUserBack;					//��Ϸ����б�
	CPngImage						m_ImageChatBack;					//��Ϸ��Ϣ��

	CPngImage						m_ImageMeBanker;					//�л�ׯ��
	CPngImage						m_ImageChangeBanker;				//�л�ׯ��
	CPngImage						m_ImageNoBanker;					//�л�ׯ��

	//��������
protected:
	CPngImage						m_ImageBetTip;						//��ע��ʾ
	CPngImage						m_ImageGameEnd;						//�ɼ�����
	CPngImage						m_ImageDealBack;					//���Ʊ���

	//������Դ
protected:
	CPngImage						m_ImageGamePoint;					//����ͼƬ
	CPngImage						m_ImageGameEndMyScore;				//��ҽ�������
	CPngImage						m_ImageGameEndAllScore;				//��ҽ�������
	CPngImage						m_ImageGameEndPoint;				//�����������

	CPngImage						m_ImageWinType;						//��Ӯ����
	CPngImage						m_ImageWinXian;						//����
	CPngImage						m_ImageWinZhuang;					//ׯ��


	//�������
protected:
	CPngImage						m_ImageBrandBoxRight;				//�ƺ���

	CPngImage						m_ImageTimeNumber;					//ʱ������
	CPngImage						m_ImageTimeBack;					//ʱ�䱳��
	CPngImage						m_ImageTimeType;					//ʱ���ʶ
	CPngImage						m_ImageTimeFlag;					//ʱ���ʶ

	CPngImage						m_ImagePlayLeft;					//�����Ϣ��
	CPngImage						m_ImagePlayMiddle;					//�����Ϣ��
	CPngImage						m_ImagePlayRight;					//�����Ϣ��

	CPngImage						m_ImageScoreInfo;					//������Ϣ
	CPngImage						m_ImageBankerInfo;					//ׯ����Ϣ
	CPngImage						m_ImageWaitValleys;					//�ȴ���ׯ
	CPngImage						m_ImageWaitFirst;					//��һ�ȴ�
	
public:
	bool							m_blCanStore;                       //�Ƿ��ܱ���
	CSkinButton						m_btBankerStorage;					//��ť
	CSkinButton						m_btBankerDraw;						//ȡ�ť

	//����
public:
	HINSTANCE						m_hControlInst;
	IClientControlDlg*				m_pClientControlDlg;

	//��������
public:
	//���캯��
	CGameClientView();
	//��������
	virtual ~CGameClientView();

	//���ƽӿ�
public:
	//���ý���
	virtual VOID ResetGameView();

	//�̳к���
private:
	//�����ؼ�
	virtual VOID RectifyControl(int nWidth, int nHeight);
	//�滭����
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);
	//WIN7֧��
	virtual bool RealizeWIN7() { return true; }

public:
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//���ú���
public:
	//����״̬
	VOID SetGameStatus(BYTE cbGameStatus);
	//�ҵ�λ��
	void SetMeChairID(WORD wMeChairID);
	//���������ע
	void SetPlayBetScore(LONGLONG lPlayBetScore);
	//��������
	void SetAreaLimitScore(LONGLONG lAreaLimitScore);
	//ׯ����Ϣ
	void SetBankerInfo(WORD wBankerUser, LONGLONG lBankerScore);
	//ׯ����Ϣ
	void SetBankerInfo(WORD wBankerUser, LONGLONG lBankerScore, LONGLONG lBankerWinScore, WORD wBankerTime);
	//ׯ����Ϣ
	void SetBankerOverInfo(LONGLONG lBankerWinScore, WORD wBankerTime);
	//����ϵͳ�Ƿ���ׯ
	void SetEnableSysBanker(bool bEnableSysBanker);
	//������ע
	void SetPlayBet(BYTE cbViewIndex, LONGLONG lBetCount);
	//ȫ����ע
	void SetAllBet(BYTE cbViewIndex, LONGLONG lBetCount);
	//���ó���
	void AddChip(BYTE cbViewIndex, LONGLONG lScoreCount);
	//���ֳɼ�
	void SetCurGameScore(LONGLONG lPlayScore[AREA_MAX], LONGLONG lPlayAllScore, LONGLONG lRevenue);
	//�����˿�
	void SetCardInfo(BYTE cbCardCount[2], BYTE cbTableCardArray[2][3]);
	//���ó���
	void SetCurrentBet(LONGLONG lCurrentBet);
	//��ʷ��¼
	void SetGameHistory(enOperateResult OperateResult, BYTE cbPlayerCount, BYTE cbBankerCount, BYTE cbKingWinner, bool bPlayerTwoPair, bool bBankerTwoPair);
	//�ֻ�ׯ��
	void ShowChangeBanker( bool bChangeBanker );
	//�滭��ע����
	void JettonAreaFrame(CDC *pDC);
	//������Ϣ
	void InsertAllChatMessage(LPCTSTR pszString,int nMessageType) ;
	//ϵͳ��Ϣ
	void InsertSystemMessage(LPCTSTR pszString);
	//������Ϣ
	void InsertChatMessage(LPCTSTR pszString);
	//������Ϣ
	void InsertNormalMessage(LPCTSTR pszString);
	//��Ϣ����
	void InsertMessage(LPCTSTR pszString,COLORREF color=RGB(193,167,108));
	//����Ч��
	bool EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex);
	//������Ϣ
	void InsertGameEndInfo();
	//��ʷ��¼
	void SetBottomRecord(const tagClientGameRecord &ClientGameRecord);
	//��¼����
	void SetRecordCol();
	//���°�ť
	VOID UpdateButtonContron();
	// �Ƿ������ģʽ
	bool IsZoomedMode();
	//������ͼ
	void RefreshGameView();

	//��������
public:	
	//��ʼ����
	void DispatchCard();
	//��������
	void FinishDispatchCard(bool bScene = false);
	//��������
	void FlexAnimation(enFlexMode nFlexMode, bool bShow, bool bMove = true);
	//��˸����
	void FlashAnimation( bool bBegin );


	//�滭����
protected:
	// �滭������Ϣ
	void DrawTopInfo(CDC *pDC, int nWidth, int nHeight);
	// �滭�ײ���Ϣ
	void DrawBottomInfo(CDC *pDC, int nWidth, int nHeight);
	//��ʾ��Ӯ
	void DrawWinType(CDC *pDC, int nWidth, int nHeight, int nBeginX, int nBeginY );
	//��ʾ���
	void DrawGameOver(CDC *pDC, int nWidth, int nHeight, int nBeginX, int nBeginY );
	// ��˸�߿�
	void DrawFlashFrame(CDC *pDC, int nWidth, int nHeight);
	//�滭���
	void DrawGameFrame(CDC *pDC, int nWidth, int nHeight);

	//��������
public:
	//�����ע
	LONGLONG GetMaxPlayerScore(BYTE cbBetArea);

	//���溯��
public:
	//�������
	void CleanUserBet();
	//������ʾ
	void SetDispatchCardTips();

	// ����ȫ���滭�ķ�϶������ƽ�Ƶ�����
	INT CalFrameGap();

	// �������һ����¼���ڵ�λ�õ�����Ŀ
	INT CalLastRecordHorCount();

	//��������
public:
	//��ǰ����
	inline LONGLONG GetCurrentBet() { return m_lCurrentBet; }	

	//�ڲ�����
private:
	//��ȡ����
	BYTE GetBetArea(CPoint MousePoint);
	//�ƶ�Ӯ��
	void DeduceWinner(BYTE* pWinArea);
	//�ƶ�Ӯ��
	void DeduceWinner(BYTE &cbWinner, BYTE &cbKingWinner, bool &bPlayerTwoPair, bool &bBankerTwoPair);
	// ��Ӷ���
	CString AddComma( LONGLONG lScore , bool bPlus = false);
	// ɾ������
	LONGLONG DeleteComma( CString strScore );

	//�滭���亯��
private:
	//��������
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//�滭�ַ�
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat);
	//��������
	void DrawTextString(CDC * pDC, CDFontEx* pFont, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//�滭�ַ�
	void DrawTextString(CDC * pDC, CDFontEx* pFont, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat);
	// �滭����
	void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, LONGLONG lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// �滭����
	void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	// �滭����
	void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat = DT_LEFT);
	// �滭ʱ��
	void DrawTime(CDC * pDC, WORD wUserTime,INT nXPos, INT nYPos);

	//��ť��Ϣ
protected:
	//���밴ť
	afx_msg void OnBetButton100();
	//���밴ť
	afx_msg void OnBetButton1000();
	//���밴ť
	afx_msg void OnBetButton10000();
	//���밴ť
	afx_msg void OnBetButton100000();
	//���밴ť
	afx_msg void OnBetButton1000000();
	//���밴ť
	afx_msg void OnBetButton5000000();
	//���밴ť
	afx_msg void OnBetButton10000000();
	//�鿴·��
	afx_msg void OnViewChart();
	//��ׯ��ť
	afx_msg void OnApplyBanker();
	//��ׯ��ť
	afx_msg void OnCancelBanker();
	//�ƶ���ť
	afx_msg void OnScoreMoveL();
	//�ƶ���ť
	afx_msg void OnScoreMoveR();
	//�Ϸ�ҳ
	afx_msg void OnValleysUp();
	//�·�ҳ
	afx_msg void OnValleysDown();
	//����Ա����
	afx_msg void OpenAdminWnd();
	//���д��
	afx_msg void OnBankStorage();
	//����ȡ��
	afx_msg void OnBankDraw();
	//��С��ť	
	afx_msg void OnButtonMin();
	//���ð�ť	
	afx_msg void OnButtonGameOption();
	//�رհ�ť	
	afx_msg void OnButtonClose();
	//���а�ť
	afx_msg void OnButtonGoBanker();

	//��Ϣӳ��
protected:
	//��ʱ����Ϣ
	afx_msg void OnTimer(UINT nIDEvent);
	//��������
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//�����Ϣ
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);	
	//�����Ϣ
	afx_msg void OnRButtonDown(UINT nFlags, CPoint Point);	
	//�����Ϣ
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//����
	afx_msg LRESULT OnSetCaption(WPARAM wParam, LPARAM lParam);
	//�ؼ�����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//�ؼ���ɫ
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSize( UINT nType, int cx, int cy );

	afx_msg void OnLButtonDblClk(UINT nFlags,CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
