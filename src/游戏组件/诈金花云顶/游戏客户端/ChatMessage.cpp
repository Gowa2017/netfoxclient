#include "StdAfx.h"
#include ".\chatmessage.h"
#include "GameClientDlg.h"

CChatMessage::CChatMessage(void)
{
}

CChatMessage::~CChatMessage(void)
{
}

//用户聊天
bool CChatMessage::InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor)
{
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	CGameClientEngine * pGameClientEngine = (CGameClientEngine*)CGameClientEngine::GetInstance();
	
	CString strMessage=TEXT("");
	strMessage.Format(TEXT("【%s】说：%s"), pszSendUser, pszString);
	if (pGameFrameEngine!=NULL)
	{
		pGameClientEngine->OnChatMessage(WPARAM(strMessage.GetBuffer(0)),2);
	}

	__super::InsertUserChat(pszSendUser,pszString,crColor);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
