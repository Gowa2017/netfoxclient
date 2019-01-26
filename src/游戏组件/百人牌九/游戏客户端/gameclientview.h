#ifndef GAME_CLIENT_VIEW_HEAD_FILE
#define GAME_CLIENT_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "RecordGameList.h"
#include "ApplyUserList.h"
#include "ClientControl.h"
#include "../��Ϸ������/GameLogic.h"
#include "MessageTipDlg.h"
#include "SkinListCtrl.h"
#include "MyEdit.h"
#include "BankTipsDlg.h"
#include "SkinButtonEx.h"

//////////////////////////////////////////////////////////////////////////

//���붨��
#define JETTON_COUNT        7                 //������Ŀ
#define JETTON_RADII        68                  //����뾶

//��Ϣ����
#define IDM_PLACE_JETTON      WM_USER+200             //��ס��Ϣ
#define IDM_APPLY_BANKER      WM_USER+201             //������Ϣ
#define IDM_CONTINUE_CARD     WM_USER+202             //��������
#define IDM_AUTO_OPEN_CARD      WM_USER+203             //��ť��ʶ
#define IDM_OPEN_CARD       WM_USER+204
#define IDM_CHAT_MESSAGE      WM_USER+205             //��Ϸ������Ϣ
#define IDM_GAME_OPTIONS      WM_USER+206             //��Ϸ������Ϣ
#define IDM_GAME_SOUND        WM_USER+207             //��Ϸ����
#define WM_SET_CAPTION        WM_USER+208             //���ñ���


//��������
#define INDEX_PLAYER        0                 //�м�����
#define INDEX_BANKER        1                 //ׯ������

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//��ʷ��¼
#define MAX_SCORE_HISTORY     65                  //��ʷ����

#define NORMAL_MAX_FALG_COUNT       15              //����ģʽ��ʶ����          
#define ZOOMED_MAX_FALG_COUNT       20              //���ģʽ��ʶ����

//������Ϣ
struct tagJettonInfo
{
  int               nXPos;                //����λ��
  int               nYPos;                //����λ��
  BYTE              cbJettonIndex;            //��������

  BOOL              bShow;                //�Ƿ���ʾ
};

//�������
enum enOperateResult
{
  enOperateResult_NULL,
  enOperateResult_Win,
  enOperateResult_Lost
};

//��¼��Ϣ
struct tagClientGameRecord
{
  enOperateResult         enOperateShunMen;         //������ʶ
  enOperateResult         enOperateDuiMen;          //������ʶ
  enOperateResult         enOperateDaoMen;          //������ʶ
  bool              bWinShunMen;            //˳��ʤ��
  bool              bWinDuiMen;             //����ʤ��
  bool              bWinDaoMen;             //����ʤ��
};

//������ʾ
enum enDispatchCardTip
{
  enDispatchCardTip_NULL,
  enDispatchCardTip_Continue,                     //��������
  enDispatchCardTip_Dispatch                      //����ϴ��
};

//�ͻ��˻������·�
struct tagAndroidBet
{
  BYTE              cbJettonArea;           //��������
  LONGLONG            lJettonScore;           //��ע��Ŀ
  WORD              wChairID;             //���λ��
  int               nLeftTime;              //ʣ��ʱ�� (100msΪ��λ)
};

//��������
typedef CWHArray<tagJettonInfo,tagJettonInfo&> CJettonInfoArray;

//������
class CGameClientEngine;
//////////////////////////////////////////////////////////////////////////

//��Ϸ��ͼ
class CGameClientView : public CGameFrameViewGDI
{
  //������Ϣ
protected:
  LONGLONG            m_lMeMaxScore;            //�����ע
  LONGLONG            m_lAreaLimitScore;          //��������
public:
  LONGLONG            m_TempScore;

  //��ע��Ϣ
protected:
  LONGLONG            m_lUserJettonScore[AREA_COUNT+1]; //������ע
public:
  LONGLONG            m_lAllJettonScore[AREA_COUNT+1];  //ȫ����ע
  INT               m_nEndGameMul;            //��ǰ���ưٷֱ�

  //λ����Ϣ
protected:
  int               m_nWinFlagsExcursionX;        //ƫ��λ��
  int               m_nWinFlagsExcursionY;        //ƫ��λ��
  int               m_nScoreHead;           //�ɼ�λ��
  CRect             m_rcShunMen;            //�м�����
  CRect             m_rcJiaoL;              //����������
  CRect             m_rcQiao;             //��������
  CRect             m_rcDuiMen;             //ƽ����
  CRect             m_rcDaoMen;             //ͬ��ƽ����
  CRect             m_rcJiaoR;              //ׯ������
  int               m_OpenCardIndex;          //�_�����
  int               m_PostCardIndex;          //�l�����
  int               m_PostStartIndex;         //������ʼλ��
  CRect               m_RectUserList;           //�б�λ��
  CRect               m_RectChartDisplay;         //����λ��

  //�˿���Ϣ
public:
  BYTE              m_cbTableCardArray[4][2];     //�����˿�
  bool              m_blMoveFinish;
  BYTE              m_bcfirstShowCard;
  BYTE              m_bcShowCount;
  bool              m_blAutoOpenCard;         //���Զ�

  //��ʷ��Ϣ
protected:
  LONGLONG            m_lMeStatisticScore;        //��Ϸ�ɼ�
  tagClientGameRecord       m_GameRecordArrary[MAX_SCORE_HISTORY];//��Ϸ��¼
  int               m_nRecordFirst;           //��ʼ��¼
  int               m_nRecordLast;            //����¼

  //״̬����
public:
  WORD              m_wMeChairID;           //�ҵ�λ��
  BYTE              m_cbAreaFlash;            //ʤ�����
  LONGLONG            m_lCurrentJetton;         //��ǰ����
  bool              m_bShowChangeBanker;        //�ֻ�ׯ��
  bool              m_bNeedSetGameRecord;       //�������
  bool              m_bWinShunMen;            //ʤ����ʶ
  bool              m_bWinDuiMen;           //ʤ����ʶ
  bool              m_bWinDaoMen;           //ʤ����ʶ
  bool              m_bFlashResult;           //��ʾ���
  bool              m_bShowGameResult;          //��ʾ���
  enDispatchCardTip       m_enDispatchCardTip;        //������ʾ
  bool                            m_bPlaceJettonEnd;                  //��ע����
  CString             m_strRoomName;            //������

  //ׯ����Ϣ
protected:
  WORD              m_wBankerUser;            //��ǰׯ��
  WORD              m_wBankerTime;            //��ׯ����
  LONGLONG            m_lBankerScore;           //ׯ�һ���
  LONGLONG            m_lBankerWinScore;          //ׯ�ҳɼ�
  LONGLONG            m_lTmpBankerWinScore;       //ׯ�ҳɼ�
  bool              m_bEnableSysBanker;         //ϵͳ��ׯ

  //���ֳɼ�
public:
  LONGLONG            m_lMeCurGameScore;          //�ҵĳɼ�
  LONGLONG            m_lMeCurGameReturnScore;      //�ҵĳɼ�
  LONGLONG            m_lBankerCurGameScore;        //ׯ�ҳɼ�
  LONGLONG            m_lGameRevenue;           //��Ϸ˰��

  //���ױ���
public:
  bool              m_bShowCheatInfo;         //��ʾ����
  BYTE              m_bCheatCard[4][2];         //�����˿�
  int               m_nWinInfo[3];            //��Ӯ���

  //���ݱ���
protected:
  CPoint              m_PointJetton[AREA_COUNT];      //����λ��
  CPoint              m_PointJettonNumber[AREA_COUNT];  //����λ��
  CJettonInfoArray        m_JettonInfoArray[AREA_COUNT];    //��������

  //�ؼ�����
public:
  CSkinButtonEx         m_btJetton100;            //���밴ť
  CSkinButtonEx         m_btJetton1000;           //���밴ť
  CSkinButtonEx         m_btJetton10000;          //���밴ť
  CSkinButtonEx         m_btJetton100000;         //���밴ť
  CSkinButtonEx         m_btJetton500000;         //���밴ť
  CSkinButtonEx         m_btJetton1000000;          //���밴ť
  CSkinButtonEx         m_btJetton5000000;          //���밴ť
  CSkinButton           m_btApplyBanker;          //����ׯ��
  CSkinButton           m_btCancelBanker;         //ȡ��ׯ��
  CSkinButton           m_btScoreMoveL;           //�ƶ��ɼ�
  CSkinButton           m_btScoreMoveR;           //�ƶ��ɼ�
  CSkinButton           m_btAutoOpenCard;         //�Զ�����
  CSkinButton           m_btOpenCard;           //�ֶ�����
  CSkinButton           m_btContinueCard;         //��������
  CSkinButton           m_btUp;               //ׯ���б�ť
  CSkinButton           m_btDown;             //ׯ���б�ť
  CSkinButton           m_btOption;             //���ð�ť
  CSkinButton           m_btMin;              //��С����ť
  CSkinButton           m_btClose;              //�رհ�ť
  CSkinButton           m_btMax;
  CSkinButton           m_btRestore;
  CSkinButton           m_btSound;              //������ť
  CSkinButton           m_btBank;             //���а�ť

  CSkinButton           m_btSendChat;           //���Ͱ�ť


  //CDlgBank            m_DlgBank;              //���пؼ�
  CSkinButton           m_btBankerStorage;          //��ť
  CSkinButton           m_btBankerDraw;           //ȡ�ť
  bool              m_blCanStore;                       //�Ƿ��ܱ���

  CButton             m_btOpenAdmin;
  //�ؼ�����
public:
  CGameLogic            m_GameLogic;
  INT               m_nShowValleyIndex;         //��ʾ��ׯ�б�
  CWHArray<WORD>          m_ValleysList;            //��ׯ�б�
  //CApplyUser            m_ApplyUser;            //�����б�
  CGameRecord           m_GameRecord;           //��¼�б�
  CCardControl          m_CardControl[5];         //�˿˿ؼ�
  CGameClientEngine         *m_pGameClientDlg;          //����ָ��
  CSkinListCtrlEx                 m_UserList;             //�б�ؼ�
  CMyEdit             m_ChatDisplay;            //��ʾ�ؼ�
  CEdit             m_ChatInput;            //����ؼ�
  CBrush              m_brush;              //������ˢ

  //����
public:
  HINSTANCE           m_hInst;
  IClientControlDlg*        m_pClientControlDlg;

  //�������
protected:
  CBitImage           m_ImageViewFill;          //����λͼ
  CPngImage           m_ImageViewBack;          //����λͼ
  CPngImage           m_ImageGameFrame[8];        //��Ϸ���
  CPngImage           m_ImageWinFlags;          //��־λͼ
  CPngImageEx           m_ImageJettonView;          //������ͼ
  CPngImageEx           m_ImageScoreNumber;         //������ͼ
  CPngImageEx           m_ImageMeScoreNumber;       //������ͼ
  CPngImage           m_ImageTimeFlag;          //ʱ���ʶ
  CPngImage           m_ImageDispatchCardTip;       //������ʾ
  CPngImage           m_ImageUserBack;          //��Ϸ����б�
  CPngImage           m_ImageChatBack;          //��Ϸ��Ϣ��
  CPngImage           m_ImageWaitValleys;         //�ȴ���ׯ
  CPngImage           m_ImageTimeNumber;          //ʱ������
  CPngImage           m_ImageTimeBack;          //ʱ�䱳��

  CPngImage           m_ImageBankerInfo;
  CPngImage           m_ImagePlayerInfo;

  CPngImage           m_ImageHistoryMid;
  CPngImage           m_ImageHistoryFront;
  CPngImage           m_ImageHistoryBack;
  //�߿���Դ
protected:
  CPngImageEx           m_ImageFrameShunMen;        //�߿�ͼƬ
  CPngImageEx           m_ImageFrameJiaoL;          //�߿�ͼƬ
  CPngImageEx           m_ImageFrameQiao;         //�߿�ͼƬ
  CPngImageEx           m_ImageFrameDuiMen;         //�߿�ͼƬ
  CPngImageEx           m_ImageFrameDaoMen;         //�߿�ͼƬ
  CPngImageEx           m_ImageFrameJiaoR;          //�߿�ͼƬ

  CPngImage           m_ImageMeBanker;          //�л�ׯ��
  CPngImage           m_ImageChangeBanker;        //�л�ׯ��
  CPngImage           m_ImageNoBanker;          //�л�ׯ��

  //������Դ
protected:
  CPngImageEx           m_ImageGameEnd;           //�ɼ�ͼƬ



  CDFontEx            m_DFontExT;
  CRgn              m_JettonRegion[AREA_COUNT];     //��ʾ����


  DWORD             m_dwChatTime;           //����ʱ��

  CString             m_strCaption;

  //��������
public:
  //���캯��
  CGameClientView();
  //��������
  virtual ~CGameClientView();

public:
  //��Ϣ����
  virtual BOOL PreTranslateMessage(MSG * pMsg);

  //�̳к���
private:
  //���ý���
  virtual VOID ResetGameView();
  //�����ؼ�
  virtual VOID RectifyControl(INT nWidth, INT nHeight);
  //�滭����
  virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);
  //WIN7֧��
  virtual bool RealizeWIN7()
  {
    return true;
  }

  //���ú���
public:
  //���÷���
  void SetRoomName(CString strRoomName)
  {
    m_strRoomName = strRoomName;
  }
  //������Ϣ
  void SetMeMaxScore(LONGLONG lMeMaxScore);
  //�ҵ�λ��
  void SetMeChairID(WORD dwMeUserID);
  //������ע
  void SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount);
  //ׯ����Ϣ
  void SetBankerInfo(DWORD dwBankerUserID, LONGLONG lBankerScore);
  //ׯ�ҳɼ�
  void SetBankerScore(WORD wBankerTime, LONGLONG lWinScore)
  {
    m_wBankerTime=wBankerTime;
    m_lTmpBankerWinScore=lWinScore;
  }
  //���ֳɼ�
  void SetCurGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore, LONGLONG lGameRevenue);
  //���ó���
  void PlaceUserJetton(BYTE cbViewIndex, LONGLONG lScoreCount);
  //��������
  void SetAreaLimitScore(LONGLONG lAreaLimitScore);
  //�����˿�
  void SetCardInfo(BYTE cbTableCardArray[4][2]);
  //���ó���
  void SetCurrentJetton(LONGLONG lCurrentJetton);
  //��ʷ��¼
  void SetGameHistory(bool bWinShunMen, bool bWinDaoMen, bool bWinDuiMen);
  //�ֻ�ׯ��
  void ShowChangeBanker(bool bChangeBanker);
  //�ɼ�����
  void SetGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore);
  //����ϵͳ��ׯ
  void EnableSysBanker(bool bEnableSysBanker)
  {
    m_bEnableSysBanker=bEnableSysBanker;
  }
  //������ʾ
  void SetDispatchCardTip(enDispatchCardTip DispatchCardTip);

  void SetFirstShowCard(BYTE bcCard);
  //������������
  void SetCheatInfo(BYTE bCardData[], int nWinInfo[]);
  //ȡ������
  void ClearAreaFlash()
  {
    m_cbAreaFlash = 0xFF;
  }
  //������ͼ
  void RefreshGameView();
  //���°�ť
  VOID UpdateButtonContron();

  //��������
public:
  //��ʼ����
  void DispatchCard();
  //��������
  void FinishDispatchCard(bool bNotScene);
  //���ñ�ը
  bool SetBombEffect(bool bBombEffect, WORD wAreaIndex);
  //��ֹ����
  void StopAnimal();
  //���а�ť
  void OnButtonGoBanker();

  //��������
public:
  //�����ע
  LONGLONG GetUserMaxJetton(BYTE cbArea = 0xFF);

  //�滭����
protected:
  //��˸�߿�
  void FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC);

  //���溯��
public:
  //�������
  void CleanUserJetton();
  //����ʤ��
  void SetWinnerSide(bool bWinShunMen, bool bWinDuiMen, bool bWinDaoMen, bool bSet);
  //��������
  void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, CRect rcRect, UINT nDrawFormat);
  //�滭����
  void DrawMeJettonNumber(CDC *pDC);
  //�滭ׯ��
  void DrawBankerInfo(CDC *pDC,int nWidth,int nHeight);
  //�滭���
  void DrawMeInfo(CDC *pDC,int nWidth,int nHeight);
  //�����˿�
  void DrawCheatCard(int nWidth, int nHeight, CDC *pDC);
  //�������
  void AllowControl(BYTE cbStatus);
  //�滭���
  void DrawGameFrame(CDC *pDC, int nWidth, int nHeight);
  // �滭��ׯ�б�
  void DrawBankerList(CDC *pDC, int nWidth, int nHeight);
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
  //������Ϣ
  void InsertGameEndInfo();

  //��������
public:
  //��ǰ����
  inline LONGLONG GetCurrentJetton()
  {
    return m_lCurrentJetton;
  }
  void SetMoveCardTimer();


  // �Ƿ������ģʽ
  bool IsZoomedMode();

  // ����ȫ���滭�ķ�϶������ƽ�Ƶ�����
  INT CalFrameGap();

  //�ڲ�����
private:
  //��ȡ����
  BYTE GetJettonArea(CPoint MousePoint);
  //�滭����
  void DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore = false);
  //�滭����
  void DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos);
  //�滭����
  void DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, INT nFormat=-1);
  //�滭����
  void DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, COLORREF crText, COLORREF crFrame, INT nFormat=-1);
  //�滭��ʶ
  void DrawWinFlags(CDC * pDC);
  //��ʾ���
  void ShowGameResult(CDC *pDC, int nWidth, int nHeight);
  //͸���滭
  bool DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha);
  //�ƶ�Ӯ��
  void DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen);
  // �滭ʱ��
  void DrawTime(CDC * pDC, WORD wUserTime,INT nXPos, INT nYPos);
  // �滭����
  void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, LONGLONG lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
  // �滭����
  void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
  // �滭����
  void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat = DT_LEFT);

  //��ť��Ϣ
protected:
  //��ׯ��ť
  afx_msg void OnApplyBanker();
  //��ׯ��ť
  afx_msg void OnCancelBanker();
  //�ƶ���ť
  afx_msg void OnScoreMoveL();
  //�ƶ���ť
  afx_msg void OnScoreMoveR();
  //�ֹ�����
  afx_msg void OnOpenCard();
  //�Զ�����
  afx_msg void OnAutoOpenCard();
  //��ҳ��ť
  afx_msg void OnUp();
  //��ҳ��ť
  afx_msg void OnDown();
  //������Ϣ
  afx_msg void OnSendMessage();

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
  afx_msg void OnLButtonUp(UINT nFlags, CPoint Point);
  //�����Ϣ
  afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
  //����
  afx_msg LRESULT OnSetCaption(WPARAM wParam, LPARAM lParam);
  //�ؼ�����
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //����ƶ�
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  //�ؼ���ɫ
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  //�����ؼ����UP��Ϣ
  //LRESULT OnViLBtUp(WPARAM wParam, LPARAM lParam);
  //����ȡ��
  afx_msg void OnBankDraw();
  //���д��
  afx_msg void OnBankStorage();
  //����Ա����
  afx_msg void OpenAdminWnd();

  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);


  //����Ч��
  bool EfficacyUserChat(LPCTSTR pszChatString, WORD wExpressionIndex);

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
