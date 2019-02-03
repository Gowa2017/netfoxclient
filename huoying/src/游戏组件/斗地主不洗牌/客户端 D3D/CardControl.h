#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////
//�궨��

//�Ʊ�����
#define CARD_LAND					0									//��������
#define CARD_BOOR					1									//ũ�����

//�����Ͷ���
#define CARD_TYPE_NOMAL				0									//������
#define CARD_TYPE_SMALL				1									//С����
#define CARD_TYPE_LEFT				2									//�����
#define CARD_TYPE_RIGHT				3									//�ұ���
#define CARD_TYPE_LOUT				4
#define CARD_TYPE_ROUT				5
#define	CARD_TYPE_OUT				6									//

//��������
#define INVALID_ITEM				0xFF								//��Ч����

//���Զ���
#define MAX_CARD_COUNT				20									//�˿���Ŀ
#define SPACE_CARD_DATA				255									//����˿�

//��ֵ����
#define	CARD_MASK_COLOR				0xF0								//��ɫ����
#define	CARD_MASK_VALUE				0x0F								//��ֵ����

//��ඨ��
#define DEF_X_DISTANCE				22									//Ĭ�ϼ��
#define DEF_Y_DISTANCE				18									//Ĭ�ϼ��
#define DEF_SHOOT_DISTANCE			20									//Ĭ�ϼ��

//��ඨ��
#define DEF_X_DISTANCE_SMALL		18									//Ĭ�ϼ��
#define DEF_Y_DISTANCE_SMALL		17									//Ĭ�ϼ��
#define DEF_SHOOT_DISTANCE_SMALL	20									//Ĭ�ϼ��

//������Ϣ
#define IDM_OUT_CARD_FINISH			(WM_USER+400)						//�������

//////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////

//�˿˽ṹ
struct tagCardItem
{
	bool							bShoot;								//�����־
	BYTE							cbCardData;							//�˿�����
};

//////////////////////////////////////////////////////////////////////////////////

//�˿���Դ
class CCardResource
{
	//��Ԫ����
	friend class CCardControl;

	//�˿���Դ
protected:
	CSize							m_CardSize;							//�˿˴�С
	CSize							m_CardSizeSmall;					//�˿˴�С
	CSize							m_CardSizeLeft;						//
	CSize							m_CardSizeRight;					//
	CSize							m_CardSizeLeftOut;					//
	CSize							m_CardSizeRightOut;					//
	CSize							m_CardSizeOut;						//

	//�˿���Դ
protected:
	CD3DSprite						m_TextureCard;						//ͼƬ��Դ
	CD3DSprite						m_TextureSmallCard;					//ͼƬ��Դ
	CD3DSprite						m_TextureLeftCard;					//ͼƬ��Դ
	CD3DSprite						m_TextureRightCard;					//ͼƬ��Դ
	CD3DSprite						m_TextureLeftOutCard;				//ͼƬ��Դ
	CD3DSprite						m_TextureRightOutCard;				//ͼƬ��Դ
	CD3DSprite						m_TextureOutCard;					//ͼƬ��Դ


	//��������
public:
	//���캯��
	CCardResource();
	//��������
	virtual ~CCardResource();

	//��Դ����
public:
	//������Դ
	VOID Initialize(CD3DDevice * pD3DDevice);
	//��ȡ��С
	VOID GetCardSize(BYTE cbCardType, CSize & CardSize);
	//��ȡ��Դ
	VOID GetCardTexture(BYTE cbCardType, CD3DSprite * * pD3DTexture);
	//����������Դ
	VOID LoadHandCard(CD3DDevice * pD3DDevice);
	//�ͷ�������Դ
	VOID DestroyHandCard(CD3DDevice * pD3DDevice);
};

//////////////////////////////////////////////////////////////////////////////////

//�˿˿ؼ�
class CCardControl
{
	//���Ա���
protected:
	bool							m_bSmallMode;						//С��ģʽ
	bool							m_bHorizontal;						//��ʾ����
	bool							m_bPositively;						//��Ӧ��־
	bool							m_bDisplayItem;						//��ʾ��־
	BYTE							m_cbBackGround;						//������־
	BYTE							m_cbCardType;						//������
	bool							m_bDrawOrder;						//����˳��

	//ѡ�����
protected:
	BYTE							m_cbHeadIndex;						//��ʼ����
	BYTE							m_cbTailIndex;						//��������

	//���Ʊ���
protected:
	bool							m_bOutCard;							//���Ʊ�־
	WORD							m_wOutCardIndex;					//��������
	CLapseCount						m_OutCardLapseCount;				//���ż���

	//�������
protected:
	INT							m_nXDistance;						//������
	INT							m_nYDistance;						//������
	INT							m_nShootDistance;					//����߶�
	
	//�ָ�����
protected:
	INT							m_nFirstPart;
	INT							m_nLastPart;

	//λ�ñ���
public:
	CPoint							m_DispatchPos;						//�ɷ�λ��
	CPoint							m_BenchmarkPos;						//��׼λ��
	enXCollocateMode				m_XCollocateMode;					//��ʾģʽ
	enYCollocateMode				m_YCollocateMode;					//��ʾģʽ

	//�˿�����
protected:
	BYTE							m_cbCardCount;						//�˿���Ŀ
	BYTE							m_cbShowCount;						//��ʾ��Ŀ
	CPoint							m_CardItemPoint[MAX_CARD_COUNT];	//�˿�λ��
	tagCardItem						m_CardItemArray[MAX_CARD_COUNT];	//�˿�����

	//�߼�����
protected:
	CGameLogic						m_GameLogic;						//�߼�����

	//��Դ����
public:
	static CCardResource			m_CardResource;						//�˿���Դ


	//��������
public:
	//���캯��
	CCardControl();
	//��������
	virtual ~CCardControl();

	//�����˿�
public:
	//�����˿�
	bool SetCardData(BYTE cbCardCount);
	//�����˿�
	bool SetCardData(BYTE cbCardData[], BYTE cbCardCount);

	//�����˿�
public:
	//�����˿�
	bool SetShootArea(BYTE cbHeadIndex, BYTE cbTailIndex);
	//�����˿�
	bool SetShootCard(BYTE cbCardData[], BYTE cbCardCount);
	//������ͬ����
	bool SetShootSameCard(BYTE cbCardData);
	//����˳��
	bool SetShootOrderCard( BYTE cbCardOne, BYTE cbCardTwo );

	//�ɷ��˿�
public:
	//�Ƿ����
	bool IsOutCard() { return m_bOutCard; }
	//���ó���
	bool ShowOutCard(BYTE cbCardData[], BYTE cbCardCount);

	//�˿˿���
public:
	//ɾ���˿�
	bool RemoveShootItem();
	//�ƶ��˿�
	bool MoveCardItem(BYTE cbTargerItem);
	//ѡ���˿�
	bool SetSelectIndex(BYTE cbStartIndex, BYTE cbEndIndex);
	//�����˿�
	bool SwitchCardItem(BYTE cbSourceItem, BYTE cbTargerItem);


	//��ȡ�˿�
public:
	//��ȡ�˿�
	tagCardItem * GetCardFromIndex(BYTE cbIndex);
	//��ȡ�˿�
	tagCardItem * GetCardFromPoint(CPoint & MousePoint);

	//��ȡ�˿�
public:
	//�˿���Ŀ
	BYTE GetCardCount() { return m_cbCardCount; }
	//��ȡ�˿�
	BYTE GetCardData(BYTE cbCardData[], BYTE cbBufferCount);
	//��ȡ�˿�
	BYTE GetShootCard(BYTE cbCardData[], BYTE cbBufferCount);

	//״̬��ѯ
public:
	//��ѯ����
	bool GetDirection() { return m_bHorizontal; }
	//��ѯ��Ӧ
	bool GetPositively() { return m_bPositively; }
	//��ѯ��ʾ
	bool GetDisplayItem() { return m_bDisplayItem; }

	//״̬����
public:
	//����ģʽ
	VOID SetSmallMode(bool bSmallMode) { m_bSmallMode=bSmallMode; }
	//���÷���
	VOID SetDirection(bool bHorizontal) { m_bHorizontal=bHorizontal; }
	//������Ӧ
	VOID SetPositively(bool bPositively) { m_bPositively=bPositively; }
	//������ʾ
	VOID SetDisplayItem(bool bDisplayItem) { m_bDisplayItem=bDisplayItem; }

	//�˿˿���
public:
	//������ʾ
	VOID SetShowCount(BYTE cbShowCount);
	//���ñ���
	VOID SetBackGround(BYTE cbBackGround);
	//���þ���
	VOID SetCardDistance(INT nXDistance, INT nYDistance, INT nShootDistance);
	//����������
	VOID SetCardType(BYTE cbCardType) {m_cbCardType = cbCardType;}
	//���÷ָ�
	VOID Setpartition(INT nFirstPart,INT nLastPart);
	//����˳��
	VOID SetDrawOrder(bool bOrder) {m_bDrawOrder = bOrder;}
	//���ƺ���
public:
	//��ȡ����
	VOID GetCenterPoint(CPoint & CenterPoint);
	//�ɷ�λ��
	VOID SetDispatchPos(INT nXPos, INT nYPos);
	//��׼λ��
	VOID SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);

	//�¼�����
public:
	//ִ�ж���
	VOID CartoonMovie();
	//�����Ϣ
	bool OnEventSetCursor(CPoint Point);
	//�滭�˿�
	VOID DrawCardControl(CD3DDevice * pD3DDevice);

	//�ڲ�����
public:
	//��ȡ��С
	VOID GetControlSize(CSize & ControlSize);
	//��ȡԭ��
	VOID GetOriginPoint(CPoint & OriginPoint);
	//�����л�
	BYTE SwitchCardPoint(CPoint & MousePoint);

	//��̬����
public:
	//������Դ
	static VOID LoadCardResource(CD3DDevice * pD3DDevice);
	//����������Դ
	static VOID LoadHandCard(CD3DDevice * pD3DDevice);
	//�ͷ�������Դ
	static VOID DestroyHandCard(CD3DDevice * pD3DDevice);
};

//////////////////////////////////////////////////////////////////////////////////

#endif