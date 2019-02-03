#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "GoldControl.h"
#include "HistoryRecord.h"
#include "ClientControl.h"
#include "SpeClientControl.h"
#include "GameLogic.h"
//////////////////////////////////////////////////////////////////////////

//��Ϣ����
#define IDM_HINT_OX         WM_USER+99              //��ʾ��Ϣ
#define IDM_START         WM_USER+100             //��ʼ��Ϣ
#define IDM_READY         WM_USER+101             //̯����Ϣ
#define IDM_OX            WM_USER+102             //ţţ��Ϣ
#define IDM_MAX_SCORE       WM_USER+103             //����ע
#define IDM_SEND_CARD_FINISH    WM_USER+104             //�������
#define IDM_SHOW_VALUE        WM_USER+105             //������Ϣ
#define IDM_SHOW_INFO       WM_USER+106             //��ʾ��Ϣ
#define IDM_OX_ENABLE       WM_USER+107             //ţţ��ť
#define IDM_SORT          WM_USER+108             //�˿˷���
#define IDM_RESORT          WM_USER+109             //������Ϣ
#define IDM_ADD_SCORE       WM_USER+110             //��ע��Ϣ
#define IDM_SHORTCUT_KEY      WM_USER+111             //��ݼ�
#define IDM_BANKER          WM_USER+112             //��ׯ��Ϣ
#define IDM_CLICK_CARD        WM_USER+114             //�����Ϣ
#define IDM_YU_YIN          (WM_USER+209)           //��������

#define MY_VIEW_CHAIRID       2                 //����λ��
#define DRAW_HEIGHT         34                  //�滭�߶�

/////////////////////////////////////////////////////////////////////////
//�ṹ����

//����״̬
struct tagJettonStatus
{
  //������Ϣ
  WORD              wMoveCount;             //�ƶ�����
  WORD              wMoveIndex;             //�ƶ�����

  //������Ϣ
  CPoint              ptFrom;               //����λ��
  CPoint              ptDest;               //Ŀ��λ��
  CPoint              ptCourse;             //����λ��
  SCORE            lGold;                //������Ŀ

  //�ƶ���ʽ
  INT               iMoveType;              //�ƶ���ʽ
};

//��������
struct tagSendCardItem
{
  WORD              wChairID;             //�����û�
  BYTE              cbCardData;             //��������
};

//����˵��
typedef CWHArray<tagSendCardItem,tagSendCardItem &> CSendCardItemArray;

//////////////////////////////////////////////////////////////////////////

//��Ϸ��ͼ
class CGameClientView : public CGameFrameViewGDI
{
  //���ݱ���
protected:
  WORD              m_wBankerUser;            //ׯ���û�
  SCORE            m_lTableScore[GAME_PLAYER];     //��ע��Ŀ
  BYTE              m_bOxValue[GAME_PLAYER];      //ţţ����
  BYTE              m_bOpenCardUser[GAME_PLAYER];   //̯���û�
  BYTE              m_bCallUser;            //�ȴ���ׯ
  BYTE                            m_cbDynamicJoin;                    //��̬����
  bool              m_bInvest;              //�ȴ���ע
  bool              m_bShowSocre[GAME_PLAYER];      //��ʾ����
  bool              m_bUserAction;            //�ȴ���־
  bool              m_bJettonAction;          //�ƶ���ʾ
  tagJettonStatus         m_JettonStatus[GAME_PLAYER];    //������Ϣ
  SCORE            m_lMoveGold[GAME_PLAYER];     //������Ϣ
  BYTE              m_cbShootData[GAME_PLAYER];     //��������
  BYTE              m_cbShootCount;


  //��������
public:
  tagHistoryScore *       m_pHistoryScore[GAME_PLAYER];   //������Ϣ
  bool              m_bShowScore;           //����״̬
  bool              m_bShow;              //��ʾ����
  bool              m_bLookOnUser;            //�Թ۱�־
  bool              m_bOpenStatus;            //�ȴ�̯��
  bool              m_bMeOpenCard;            //�Ƿ�̯��

  //��������
protected:
  CPoint              m_SendCardPos;            //����λ��
  WORD              m_wSendCount;           //�ƶ�����
  WORD              m_wSendIndex;           //�ƶ�����
  CPoint              m_SendEndingPos[GAME_PLAYER];   //����λ��
  CPoint              m_SendStartPos;           //ʼ��λ��
  CSendCardItemArray        m_SendCardItemArray;        //��������

  //����
public:
  HINSTANCE           m_hInst;
  IClientControlDlg*        m_pClientControlDlg;
  CButton             m_btOpenAdmin;            //ϵͳ����
  //ISpeClientControlDlg*     m_pSpeClientControlDlg;
  //HINSTANCE           m_hSpeInst;

  //��ť�ؼ�
public:
  CSkinButton           m_btHintOx;             //��ʾ��ť
  CSkinButton           m_btStart;              //��ʼ��ť
  //CSkinButton         m_btReSort;             //���Ű�ť
  CSkinButton           m_btOpenCard;           //̯�ư�ť
  CSkinButton           m_btOx;               //ţţ��ť
  //CSkinButton         m_btShortcut;           //��ݼ���ť
  CSkinButton           m_btOneScore;           //һ�ְ�ť
  CSkinButton           m_btTwoScore;           //���ְ�ť
  CSkinButton           m_btThreeScore;           //���ְ�ť
  CSkinButton           m_btFourScore;            //�ķְ�ť
  CSkinButton           m_btBanker;             //ׯ�Ұ�ť
  CSkinButton           m_btIdler;              //�мҰ�ť
  CSkinButton           m_btScore;              //���ְ�ť
  CSkinButton           m_btYuYin;              //������ť

  //�ؼ�����
public:
  //CScoreView            m_ScoreView;            //�ɼ�����
  CCardControl          m_CardControl[GAME_PLAYER];     //�û��˿�
  CCardControl          m_CardControlOx[GAME_PLAYER];   //�û��˿�

#ifdef VIDEO_GAME
  //�ؼ�����
protected:
  CVideoServiceControl      m_DlgVideoService[GAME_PLAYER];   //��Ƶ����
#endif

protected:
  CDFontEx            m_FontEx;             //��������
  CDFontEx            m_FontCardValue;          //��������

protected:
  CGameLogic            m_GameLogic;

  //λ����Ϣ
protected:
  CPoint              m_PointBanker[GAME_PLAYER];     //ׯ��λ��
  CPoint              m_ptJeton[GAME_PLAYER];       //���λ��
  CPoint              m_ptValue[GAME_PLAYER];       //����λ��
  CPoint              m_ptOpenTag[GAME_PLAYER];     //̯��λ��
  CPoint              m_ptCard[GAME_PLAYER];        //���λ��
  CSize             m_sizeHistory;            //���ִ�С

  //�������
protected:
  CPngImage           m_ImageCard;            //�˿���Դ
  CPngImage           m_ImageViewFill;            //������Դ
  CPngImage           m_ImageViewCenter;            //���б���

  CBitImage           m_ImageJeton;           //������Դ
  CBitImage           m_ImageOpenCard;          //̯����Դ

  CPngImage           m_ImageScore;           //������Դ

  CPngImage           m_ImageOxValueZero;         //ţֵ��Դ
  CPngImage           m_ImageDoulbeOx;          //ţţ��Դ
  CPngImage           m_ImageNumber;            //������Դ
  CPngImage           m_ImageOxValue;           //ţֵ��Դ
  CPngImage           m_ImageWaitCall;          //�ȴ���Դ
  CPngImage             m_ImageTimeBack;          //ʱ�䱳��
  CPngImage             m_ImageTimeNumber;          //ʱ������
  CPngImage           m_ImageBanker;            //ׯ����Դ
  CPngImage           m_PngUserBack;            //��ҵ׿�
  CPngImage           m_PngHistoryScore;          //��ʷ����

  CPngImage           m_PngAutoOpen;            //�Զ�����
  CPngImage           m_PngWaitBet;           //�ȴ���ע
  CPngImage           m_PngWaitOpen;            //�ȴ�����
  CPngImage           m_PngOpenTag;           //���Ʊ�ʾ
  CPngImage                       m_PngReady;                         //׼����ʶ
  CPngImage           m_PngFrameLR;           //�����Դ
  CPngImage           m_PngFrameTB;           //�����Դ
  CPngImage           m_PngVirtualLine;         //������Դ

  CPngImage           m_PngOxAddBack;           //+������
  CPngImage           m_PngOxAddNum;            //+������
  CPngImage           m_PngOxAddType;           //+������

  //�ʽ𶯻�
public:
  WORD              m_wBonusCartoonIndex;
  CPngImage                       m_PngBonus;                         //�ʽ𶯻�
  CPngImageEx                     m_PngBonusNum;                      //�ʽ𶯻�
  bool              m_bShowBonusCartoon;
  bool              m_bBonus[GAME_PLAYER];
  CPoint              m_ptBonus[GAME_PLAYER];
  SCORE            m_lBonus;

  //��ʾ����
protected:
  CToolTipCtrl          m_ToolTipCtrl;            //��ʾ�ؼ�

  //��������
public:
  //���캯��
  CGameClientView();
  //��������
  virtual ~CGameClientView();
  //��Ϣ����
  virtual BOOL PreTranslateMessage(MSG * pMsg);

  //���غ���
protected:
  //�����
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

  //�̳к���
private:
  //���ý���
  virtual VOID ResetGameView();
  //�����ؼ�
  virtual VOID RectifyControl(int nWidth, int nHeight);
  //�滭����
  virtual VOID DrawGameView(CDC * pDC, int nWidth, int nHeight);


  //WIN7֧��
public:
  virtual bool RealizeWIN7()
  {
    return true;
  }

  //���ܺ���
public:
  //�滭ʱ��
  void DrawUserTimerEx(CDC * pDC, int nXPos, int nYPos, WORD wTime, WORD wTimerArea=99);
  //��ʾ��Ϣ
  void SetUserAction(bool bWait);
  //�����ж�
  bool IsDispatchCard();
  //��ɷ���
  void FinishDispatchCard();
  //�����˿�
  void DispatchUserCard(WORD wChairID, BYTE cbCardData);
  //������ע
  void SetUserTableScore(WORD wChairID, SCORE lTableScore,bool bMove=false);
  //ׯ�ұ�־
  void SetBankerUser(WORD wBankerUser);
  //�ȴ���ׯ
  void SetWaitCall(BYTE bCallUser);
  //�ȴ���־
  void SetWaitInvest(bool Invest);
  //���ö�̬�����־
  void SetUserDynamicJoin(BYTE cbDynamicJoin)
  {
    m_cbDynamicJoin=cbDynamicJoin;
  };
  //������Ϣ
  //void SetScoreInfo(SCORE lTurnMaxScore,SCORE lTurnLessScore);
  //��ʾ����
  void DisplayTypeEx(bool bShow);
  //���õ���
  void SetUserOxValue(WORD wChiarID,BYTE bValue);
  //��ȡ����
  BYTE GetValue(WORD wChiarID)
  {
    return m_bOxValue[wChiarID];
  }
  //��ʾ̯��
  void ShowOpenCard(WORD wChiarID,BOOL bShow=TRUE);
  //��ʾ����
  void ShowScore(WORD wChiarID,bool bShow);
  //��������
  void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
  //��������
  void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect);
  //���ְ�ť
  VOID OnBnClickedScore();
  //���û���
  bool SetHistoryScore(WORD wChairID,tagHistoryScore * pHistoryScore);
  //���ƶ���
  void DrawMoveAnte(WORD wViewChairID, SCORE lTableScore);
  //�ƶ�����
  bool MoveJetton();
  //�����û�ͼ��
  void DrawUserReadyEx(CDC *pDC,int nXPos,int nYPos);
  //����Ա����
  afx_msg void OpenAdminWnd();
  //������ͼ
  void RefreshGameView();
  //������ͼ
  void RefreshGameView(CRect &rect);
  //��ʾ���Ƶ���
  void SetShootCardData(BYTE cbCardData[MAX_COUNT],BYTE cbCardCount);
  //�滭����
  void DrawNumberString(CDC * pDC, SCORE lNumber, INT nXPos, INT nYPos, BYTE bPngType);
  // �滭����
  void DrawNumber(CDC * pDC, CPngImageEx* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_RIGHT);
  // �滭����
  void DrawNumber(CDC * pDC, CPngImageEx* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat = DT_RIGHT);
  //������չ
  void CGameClientView::RectifyControlEx();
  //��ʼ�ʽ𶯻�
  void StartBonusCartoon();
  //���ö�����־
  void SetBonusFlag(WORD wViewChairID, bool bBonus);
  //���òʽ�
  void SetBonus(SCORE lBonus)
  {
    if(lBonus != m_lBonus)
    {
      m_lBonus = lBonus;
    }
  }
  //��Ϣӳ��
protected:
  //��������
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //��ʱ����Ϣ
  afx_msg void OnTimer(UINT nIDEvent);

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
