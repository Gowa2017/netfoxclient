#ifndef HISTORY_RECORD_HEAD_FILE
#define HISTORY_RECORD_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

//������־
#define AF_NULL           0                   //û�ж���
#define AF_PASS           1                   //��������
#define AF_OUT_CARD         2                   //���ƶ���

//��Ϣ����
#define WM_ENABLE_HISTORY     (WM_USER+500)             //������Ϣ

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//������Ϣ
struct tagHistoryScore
{
  SCORE            lTurnScore;               //������Ϣ
  SCORE            lCollectScore;              //������Ϣ
};

//////////////////////////////////////////////////////////////////////////

//��ʷ����
class CHistoryScore
{
  //��������
protected:
  tagHistoryScore         m_HistoryScore[GAME_PLAYER];      //������Ϣ

  //��������
public:
  //���캯��
  CHistoryScore();
  //��������
  virtual ~CHistoryScore();

  //���ܺ���
public:
  //��������
  VOID ResetData();
  //��ȡ����
  tagHistoryScore * GetHistoryScore(WORD wChairID);

  //�¼�����
public:
  //�û�����
  VOID OnEventUserEnter(WORD wChairID);
  //�û��뿪
  VOID OnEventUserLeave(WORD wChairID);
  //�û�����
  VOID OnEventUserScore(WORD wChairID, SCORE lGameScore);
};

//////////////////////////////////////////////////////////////////////////

#endif