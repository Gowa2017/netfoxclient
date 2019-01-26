#include "StdAfx.h"
#include "GameClient.h"
#include "ScoreControl.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////////////

//�ؼ�����
#define IDC_BT_CLOSE				100									//�����˿�
#define IDC_BT_PLAY_AGAIN			101									//
#define IDC_BT_RETURN				102									//
#define IDC_SCORE_CLOSE				103									//

//������Ŀ
#define SCORE_CARTOON_COUNT			4									//������Ŀ

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CScoreControl::CScoreControl()
{
	//���ñ���
	m_cbAlphaIndex=0;
	m_cbCartoonIndex=0;
	m_bLbuttonDown = false;

	//���ñ���
	ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));

	return;
}

//��������
CScoreControl::~CScoreControl()
{
}

//������Ϣ
VOID CScoreControl::OnWindowMovie()
{

	return;
}

//������Ϣ
VOID CScoreControl::OnWindowCreate(CD3DDevice * pD3DDevice)
{
	//��������
	m_FontScore.CreateFont(pD3DDevice,16,600,TEXT("����"));

	//�رհ�ť
	CRect rcCreate(0,0,0,0);

	m_btClose.ActiveWindow(rcCreate,WS_VISIBLE,IDC_SCORE_CLOSE,GetVirtualEngine(),this);
	m_btClose.SetButtonImage(pD3DDevice,TEXT("BT_SCORE_CLOSE"),TEXT("PNG"),AfxGetInstanceHandle());

	//������Դ
	m_TextureGameScore.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("GAME_SCORE"),TEXT("PNG"));
	m_TextureWinLoseFlag.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("WIN_LOSE_FLAG"),TEXT("PNG"));
	m_TextureChunTian.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("CHUN_TIAN"),TEXT("PNG"));
	m_TextureFanChunTian.LoadImage(pD3DDevice,AfxGetInstanceHandle(),TEXT("FAN_CHUN_TIAN"),TEXT("PNG"));
	//��ȡ��С
	CSize SizeGameScore;
	SizeGameScore.SetSize(m_TextureGameScore.GetWidth(),m_TextureGameScore.GetHeight());

	//�ƶ�����
	SetWindowPos(0,0,SizeGameScore.cx,SizeGameScore.cy,0);

	m_btClose.SetWindowPos(SizeGameScore.cx-125,52,0,0,SWP_NOSIZE);

	//���ý���
	/*if(m_ScoreInfo.pGameClientEngine->IsLookonMode() == true)
	{
		m_btPlayAgain.ShowWindow(false);
	}
	else
	{
		m_btPlayAgain.ShowWindow(true);
	}*/
	
	return;
}

//������Ϣ
VOID CScoreControl::OnWindowDestory(CD3DDevice * pD3DDevice)
{
	//ɾ������
	m_FontScore.DeleteFont();

	//������Դ
	if (m_TextureGameScore.IsNull()==false) m_TextureGameScore.Destory();
	if (m_TextureWinLoseFlag.IsNull()==false) m_TextureWinLoseFlag.Destory();
	if (m_TextureChunTian.IsNull()==false) m_TextureChunTian.Destory();
	if (m_TextureFanChunTian.IsNull()==false) m_TextureFanChunTian.Destory();

	return;
}

//����¼�
VOID CScoreControl::OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos)
{
	/*if(uMessage == WM_LBUTTONDOWN)
	{
		m_ptLOld.x = nXMousePos;
		m_ptLOld.y = nYMousePos;
		m_bLbuttonDown = true;
		SetCapture(m_ScoreInfo.pGameClientEngine->m_GameClientView.m_hWnd);
	}
	if(uMessage == WM_LBUTTONUP)
	{
		m_bLbuttonDown = false;
		ReleaseCapture();
	}
	if(uMessage == WM_MOUSEMOVE && m_bLbuttonDown == true)
	{
		m_ptLNew.x = nXMousePos;
		m_ptLNew.y = nYMousePos;
		m_ScoreInfo.pGameClientEngine->MoveSoreControl(m_ptLNew.x-m_ptLOld.x,m_ptLNew.y-m_ptLOld.y);
	}*/
	return;
}

//��ť�¼�
VOID CScoreControl::OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos)
{
	
	switch(uButtonID)
	{
	case IDC_SCORE_CLOSE:				//�رմ���
		{
			CloseControl();
			return;
		}
	case IDC_BT_PLAY_AGAIN:			//����һ��
		{
			m_ScoreInfo.pGameClientEngine->PlayAgain();
			CloseControl();
		}
		return;
	case IDC_BT_RETURN:				//���ش���
		{
			m_ScoreInfo.pGameClientEngine->ExitGame();
		}
		return;
	}

	
	return;
}

//�滭����
VOID CScoreControl::OnEventDrawWindow(CD3DDevice * pD3DDevice, INT nXOriginPos, INT nYOriginPos)
{
	
	//�滭����
	m_TextureGameScore.DrawImage(pD3DDevice,nXOriginPos,nYOriginPos);


	CRect rcScore;
	INT nWidth =m_TextureGameScore.GetWidth();
	INT nHeight = m_TextureGameScore.GetHeight();
	//�滭��Ϣ
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//�滭�û�
		LPCTSTR pszNickName=m_ScoreInfo.szNickName[i];

		//����
		CRect crNickName(nWidth/2-105-48+nXOriginPos,nHeight/2-96+10+65*i+nYOriginPos+12,nWidth/2-105-48+100+nXOriginPos,nHeight/2-96+i*65+40+nYOriginPos+12);
		if(i == m_ScoreInfo.wBankerUser)
			m_FontScore.DrawText(pszNickName, crNickName, D3DCOLOR_XRGB(255,0,0), DT_LEFT|DT_TOP|DT_END_ELLIPSIS);
		else
			m_FontScore.DrawText(pszNickName, crNickName, D3DCOLOR_XRGB(27,146,0), DT_LEFT|DT_TOP|DT_END_ELLIPSIS);

		//��Ӯ��־
		CSize SizeWinLoseFlag;
		BYTE Ximage = m_ScoreInfo.lGameScore[i]>0?0:1;
		SizeWinLoseFlag.SetSize(m_TextureWinLoseFlag.GetWidth()/3,m_TextureWinLoseFlag.GetHeight());
		m_TextureWinLoseFlag.DrawImage(pD3DDevice,nWidth/2-215+nXOriginPos,nHeight/2-84+65*i+nYOriginPos,SizeWinLoseFlag.cx,SizeWinLoseFlag.cy,Ximage*SizeWinLoseFlag.cx,0);
		
		//�÷�
		CRect rcGameScore(nWidth/2-40+nXOriginPos,nHeight/2-83-20+i*65+nYOriginPos+16,nWidth/2-40+140+nXOriginPos,nHeight/2-53-20+i*65+nYOriginPos+16);
		CRect rcCollectScore(nWidth/2-40+nXOriginPos,nHeight/2-60-20+i*65+nYOriginPos+16,nWidth/2-40+140+nXOriginPos,nHeight/2-30-20+i*65+nYOriginPos+16);
		TCHAR czGameScore[30];
		TCHAR czCollectScore[30];
		_sntprintf(czGameScore,CountArray(czGameScore),TEXT("�÷֣�%d"),m_ScoreInfo.lGameScore[i]);
		_sntprintf(czCollectScore,CountArray(czCollectScore),TEXT("�ۼƣ�%d"),m_ScoreInfo.lCollectScore[i]);
		m_FontScore.DrawText(czGameScore, rcGameScore, D3DCOLOR_XRGB(149,80,8), DT_LEFT|DT_TOP|DT_END_ELLIPSIS);
		m_FontScore.DrawText(czCollectScore, rcCollectScore, D3DCOLOR_XRGB(149,80,8), DT_LEFT|DT_TOP|DT_END_ELLIPSIS);

		//ը��
		CRect crBombCount(nWidth/2+125+nXOriginPos,nHeight/2-96+65*i+nYOriginPos+18,nWidth/2+125+100+nXOriginPos,nHeight/2-96+i*65+50+nYOriginPos+18);
		TCHAR czBombCount[30];
		_sntprintf(czBombCount,CountArray(czBombCount),TEXT("ը���� %d ��"),m_ScoreInfo.cbEachBombCount[i]);
		m_FontScore.DrawText(czBombCount, crBombCount, D3DCOLOR_XRGB(149,80,8), DT_LEFT|DT_TOP|DT_END_ELLIPSIS);

	}
	//���췴�����־
	if(m_ScoreInfo.bChunTian == FALSE && m_ScoreInfo.bFanChunTian == FALSE)
		return;
	CSize SizeChunTian;
	SizeChunTian.SetSize(m_TextureChunTian.GetWidth(),m_TextureChunTian.GetHeight());
	CSize SizeFanChunTian;
	SizeFanChunTian.SetSize(m_TextureFanChunTian.GetWidth(),m_TextureFanChunTian.GetHeight());
	//�滭����
	if(m_ScoreInfo.bChunTian == TRUE)
	{
		ASSERT(m_ScoreInfo.bFanChunTian == FALSE);
		m_TextureChunTian.DrawImage(pD3DDevice,nWidth/2-215+nXOriginPos,nHeight - 95+nYOriginPos,SizeChunTian.cx,SizeChunTian.cy,0,0);
	}
	else if(m_ScoreInfo.bFanChunTian == TRUE)
	{
		ASSERT(m_ScoreInfo.bChunTian == FALSE);
		m_TextureFanChunTian.DrawImage(pD3DDevice,nWidth/2-215+nXOriginPos,nHeight - 95+nYOriginPos,SizeFanChunTian.cx,SizeFanChunTian.cy,0,0);
	}
	return;
}

//���ش���
VOID CScoreControl::CloseControl()
{
	//�رմ���
	if (IsWindowActive()==true)
	{
		//�رմ���
		//DeleteWindow();
		
		//��������
		ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));

		//DeleteWindow();
		ShowWindow(false);

	}

	return;
}

//���û���
VOID CScoreControl::SetScoreInfo(tagScoreInfo & ScoreInfo)
{
	//���ñ���
	m_ScoreInfo=ScoreInfo;

	return;
}

// �滭����
void CScoreControl::DrawNumber( CD3DDevice * pD3DDevice, CD3DTexture* ImageNumber, TCHAR * szImageNum, SCORE lOutNum, INT nXPos, INT nYPos, UINT uFormat /*= DT_LEFT*/ )
{
	TCHAR szOutNum[128] = {0};
	_sntprintf(szOutNum,CountArray(szOutNum),SCORE_STRING,lOutNum);

	// ������Դ
	INT nNumberHeight=ImageNumber->GetHeight();
	INT nNumberWidth=ImageNumber->GetWidth()/lstrlen(szImageNum);

	if ( uFormat == DT_CENTER )
	{
		nXPos -= (INT)(((DOUBLE)(lstrlen(szOutNum)) / 2.0) * nNumberWidth);
	}
	else if ( uFormat == DT_RIGHT )
	{
		nXPos -= lstrlen(szOutNum) * nNumberWidth;
	}

	for ( INT i = 0; i < lstrlen(szOutNum); ++i )
	{
		for ( INT j = 0; j < lstrlen(szImageNum); ++j )
		{
			if ( szOutNum[i] == szImageNum[j] && szOutNum[i] != '\0' )
			{
				ImageNumber->DrawImage(pD3DDevice, nXPos, nYPos, nNumberWidth, nNumberHeight, j * nNumberWidth, 0, nNumberWidth, nNumberHeight);
				nXPos += nNumberWidth;
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
