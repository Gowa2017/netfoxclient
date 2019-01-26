#ifndef SCORE_VIEW_HEAD_FILE
#define SCORE_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagScoreViewInfo
{
  WORD              wGameScore;             //��Ϸ�÷�
};

//////////////////////////////////////////////////////////////////////////

//������ͼ��
class CScoreView : public CWnd
{
  //������Ϣ
protected:
  SCORE              m_lTax[GAME_PLAYER];        //��Ϸ˰��
  SCORE              m_lScore[GAME_PLAYER];        //��Ϸ�÷�
  TCHAR             m_szUserName[GAME_PLAYER][LEN_ACCOUNTS];//�û�����

  //��Դ����
protected:
  CBitImage           m_ImageBack;            //����ͼ��
  CBitImage           m_ImageWinLose;           //ʤ����־

  //��������
public:
  //���캯��
  CScoreView();
  //��������
  virtual ~CScoreView();

  //���ܺ���
public:
  //���û���
  void SetGameScore(WORD wChairID, LPCTSTR pszUserName, SCORE lScore);
  //����˰��
  void SetGameTax(SCORE lTax,WORD wChairID);
  //��������
  void ResetScore();

  //��Ϣ����
protected:
  //�ػ�����
  afx_msg void OnPaint();
  //������Ϣ
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //�����Ϣ
  afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);

  DECLARE_MESSAGE_MAP()
public:
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnMove(int x, int y);
};

//////////////////////////////////////////////////////////////////////////

#endif
