#ifndef GOLD_CONTROL_HEAD_FILE
#define GOLD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//��ҿ���
class CGoldControl : public CWnd
{
  //��������
protected:
  SCORE                m_lMaxGold;           //�����
  SCORE                m_lGoldCell[7];         //��Ԫ���
  CPoint                m_BasicPoint;         //����λ��

  //λ�ñ���
protected:
  int                 m_nWidth;           //�ؼ����
  int                 m_nCellCount;         //��Ԫ��Ŀ
  int                 m_AppendWidth;          //���ӿ��

  //��Դ����
protected:
  CBitImage             m_ImageLeft;          //������Դ
  CBitImage             m_ImageMid;           //������Դ
  CBitImage             m_ImageRight;         //������Դ

  //��������
public:
  //���캯��
  CGoldControl();
  //��������
  virtual ~CGoldControl();

  //���ܺ���
public:
  //��ȡ���
  SCORE GetGold();
  //���ý��
  void SetGold(SCORE lGold);
  //������ע
  void SetMaxGold(SCORE lMaxGold);
  //����λ��
  void SetBasicPoint(int nXPos, int nYPos);

  //�ڲ�����
private:
  //�����ؼ�
  void RectifyControl();
  //�滭���
  void DrawGoldCell(CDC * pDC, int nXPos, int nYPox, SCORE lGold);

  //��Ϣӳ��
protected:
  //�ػ�����
  afx_msg void OnPaint();
  //���������Ϣ
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  //�Ҽ�������Ϣ
  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  //���ù��
  afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

  DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

#endif
