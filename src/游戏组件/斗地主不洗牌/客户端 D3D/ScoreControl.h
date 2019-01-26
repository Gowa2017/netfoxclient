#ifndef SCORE_CONTROL_HEAD_FILE
#define SCORE_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"


class CGameClientEngine;
//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagScoreInfo
{
	//ը����Ϣ
	BYTE							cbBombCount;							//ը������
	BYTE							cbEachBombCount[GAME_PLAYER];			//ը������

	//��Ϸ�ɼ�
	SCORE							lCellScore;								//��Ԫ����
	SCORE							lGameScore[GAME_PLAYER];				//��Ϸ�ɼ�
	SCORE							lCollectScore[GAME_PLAYER];				//���ܳɼ�

	//�����Ϣ
	WORD							wMeChairID;								//�Լ�λ��
	WORD							wBankerUser;							//ׯ���û�
	BYTE							cbBankerScore;							//�з���Ŀ

	//�����־
	BYTE							bChunTian;							//�����־
	BYTE							bFanChunTian;						//�����־

	//�û���Ϣ
	WORD							wFaceID[GAME_PLAYER];					//ͷ���ʶ
	DWORD							dwUserID[GAME_PLAYER];					//�û���ʶ
	TCHAR							szNickName[GAME_PLAYER][LEN_NICKNAME];	//�û��ǳ�

	CGameClientEngine*				pGameClientEngine;
};

//////////////////////////////////////////////////////////////////////////////////

//���ֿؼ�
class CScoreControl : public CVirtualWindow
{
	//��������
protected:
	tagScoreInfo					m_ScoreInfo;						//������Ϣ

	//�Ҷȶ���
protected:
	BYTE							m_cbAlphaIndex;						//͸������
	CLapseCount						m_AlphaLapseCount;					//���ż���

	//�ɼ�����
protected:
	BYTE							m_cbCartoonIndex;					//��������
	CLapseCount						m_CartoonLapseCount;				//���ż���

	//�ؼ�����
protected:
	CVirtualButton					m_btClose;							//�رհ�ť
//	CVirtualButton					m_btPlayAgain;						//����һ��
//	CVirtualButton					m_btReturn;							//���ش���

	//��Դ����
protected:
	CD3DFontEx						m_FontScore;						//��������
	CD3DTexture						m_TextureGameScore;					//������ͼ
//	CD3DTexture						m_TextureBombNumber;				//ը������
	CD3DTexture						m_TextureWinLoseFlag;				//��Ӯ��־
	CD3DTexture						m_TextureChunTian;					//�����־
	CD3DTexture						m_TextureFanChunTian;				//�������־


	//״̬����
protected:
	CPoint							m_ptLOld;							//�����������
	CPoint							m_ptLNew;							//�����������
	bool							m_bLbuttonDown;						//������±�־


	//��������
public:
	//���캯��
	CScoreControl();
	//��������
	virtual ~CScoreControl();

	//ϵͳ�¼�
protected:
	//������Ϣ
	virtual VOID OnWindowMovie();
	//������Ϣ
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);
	//������Ϣ
	virtual VOID OnWindowDestory(CD3DDevice * pD3DDevice);

	//���غ���
protected:
	//����¼�
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos);
	//��ť�¼�
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos);
	//�滭����
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos);

	//���ܺ���
public:
	//���ش���
	VOID CloseControl();
	//���û���
	VOID SetScoreInfo(tagScoreInfo & ScoreInfo);
	// �滭����
	void DrawNumber(CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif