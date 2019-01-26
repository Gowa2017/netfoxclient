#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

//��������
#define INVALID_ITEM				0xFFFF								//��Ч����

//���Զ���
#define MAX_CARD_COUNT				3									//�˿���Ŀ
#define SPACE_CARD_DATA				255									//����˿�

//��ֵ����
#define	CARD_MASK_COLOR				0xF0								//��ɫ����
#define	CARD_MASK_VALUE				0x0F								//��ֵ����

//��ඨ��
#define DEF_X_DISTANCE				25									//Ĭ�ϼ��
#define DEF_Y_DISTANCE				17									//Ĭ�ϼ��
#define DEF_SHOOT_DISTANCE			20									//Ĭ�ϼ��


//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//X ���з�ʽ
enum enXCollocateMode 
{ 
	enXLeft,						//�����
	enXCenter,						//�ж���
	enXRight,						//�Ҷ���
};

//Y ���з�ʽ
enum enYCollocateMode 
{ 
	enYTop,							//�϶���
	enYCenter,						//�ж���
	enYBottom,						//�¶���
};

//////////////////////////////////////////////////////////////////////////

//�˿˽ṹ
struct tagCardItem
{
	bool							bShoot;								//�����־
	BYTE							cbCardData;							//�˿�����
};

//////////////////////////////////////////////////////////////////////////

//�˿˿ؼ�
class CCardControl
{
	//״̬����
protected:
	bool							m_bDisplayItem;						//��ʾ��־
	bool							m_bShowCardControl;					//��ʾ�˿�

	//�˿�����
protected:
	WORD							m_wCardCount;						//�˿���Ŀ
	tagCardItem						m_CardItemArray[MAX_CARD_COUNT];	//�˿�����

	//λ�ñ���
protected:
	CPoint							m_BenchmarkPos;						//��׼λ��

	//��Դ����
protected:
	CSize							m_CardSize;							//�˿˴�С
	CPngImage						m_ImageCard;						//ͼƬ��Դ

	//��������
public:
	//���캯��
	CCardControl();
	//��������
	virtual ~CCardControl();

	//��������
public:
	//��������
	void Create(CWnd* pWnd);

	//�˿˿���
public:
	//�����˿�
	bool SetCardData(WORD wCardCount);
	//�����˿�
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);

	//��ȡ�˿�
public:
	//�˿���Դ
	CPngImage* GetCardImage() { return &m_ImageCard; }
	//�˿���Ŀ
	WORD GetCardCount() { return m_wCardCount; }
	//��ȡ�˿�
	WORD GetCardData(BYTE cbCardData[], WORD wBufferCount);
	//��ȡ�˿�
	WORD GetShootCard(BYTE cbCardData[], WORD wBufferCount);
	//��ȡ�˿�
	WORD GetCardData(tagCardItem CardItemArray[], WORD wBufferCount);

	//״̬����
public:
	//������ʾ
	VOID SetDisplayFlag(bool bDisplayItem) { m_bDisplayItem = bDisplayItem; }
	//��ʾ�˿�
	void ShowCardControl(bool bShow){ m_bShowCardControl = bShow;}

	//�ؼ�����
public:
	//��׼λ��
	VOID SetBenchmarkPos(INT nXPos, INT nYPos);
	//��ȡλ��
	CPoint GetBenchmarkPos();

	//�¼�����
public:
	//�滭�˿�
	VOID DrawCardControl(CDC * pDC);
	//�滭�˿�
	VOID DrawCard(CDC * pDC, BYTE cbCardData, int nPosX, int nPosY);
};

//////////////////////////////////////////////////////////////////////////

#endif