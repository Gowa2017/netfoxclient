#include "StdAfx.h"
#include ".\stringmessage.h"

#define IDM_CHAT_MESSAGE			WM_USER+208

CStringMessage::CStringMessage(void)
{
}

CStringMessage::~CStringMessage(void)
{
}


//�ͷŶ���
VOID CStringMessage::Release()
{
	delete this;
}
//�ӿڲ�ѯ
VOID * CStringMessage::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IStringMessage,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IStringMessage,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CStringMessage::InsertUserEnter(LPCTSTR pszUserName)
{
	CString strMessage=TEXT("");
	strMessage.Format(TEXT("%s������"),pszUserName);

	SendMessageToEngine(strMessage,0);
	return true;
}
//�뿪�¼�
bool CStringMessage::InsertUserLeave(LPCTSTR pszUserName)
{
	CString strMessage=TEXT("");
	strMessage.Format(TEXT("%s�뿪��"),pszUserName);

	SendMessageToEngine(strMessage,0);
	return true;
}
//�����¼�
bool CStringMessage::InsertUserOffLine(LPCTSTR pszUserName)
{
	CString strMessage=TEXT("");
	strMessage.Format(TEXT("%s������"),pszUserName);

	SendMessageToEngine(strMessage,0);
	return true;
}

//��ͨ��Ϣ
bool CStringMessage::InsertNormalString(LPCTSTR pszString)
{
	
	CString strMessage=TEXT("");
	strMessage+=pszString;

	SendMessageToEngine(strMessage,0);
	
	return true;
}
//ϵͳ��Ϣ
bool CStringMessage::InsertSystemString(LPCTSTR pszString)
{
	
	CString strMessage=TEXT("");
	strMessage+=pszString;
	
	SendMessageToEngine(strMessage,1);
	//AfxMessageBox(pszString);
	return true;
}
//��ʾ��Ϣ
bool CStringMessage::InsertPromptString(LPCTSTR pszString)
{
	
	CString strMessage=TEXT("");
	strMessage+=pszString;

	SendMessageToEngine(strMessage,1);
	
	return true;
}
//������Ϣ
bool CStringMessage::InsertAfficheString(LPCTSTR pszString)
{
	CString strMessage=TEXT("");
	strMessage+=pszString;

	SendMessageToEngine(strMessage,1);
	
	return true;
}

//������Ϣ
bool CStringMessage::InsertCustomString(LPCTSTR pszString, COLORREF crColor)
{
	CString strMessage=TEXT("");
	strMessage+=pszString;

	SendMessageToEngine(strMessage,0);

	return true;
}
//������Ϣ
bool CStringMessage::InsertCustomString(LPCTSTR pszString, COLORREF crColor, COLORREF crBackColor)
{
	return true;
}

//�û�����
bool CStringMessage::InsertExpression(LPCTSTR pszSendUser, LPCTSTR pszImagePath)
{
	return true;
}
//�û�����
bool CStringMessage::InsertExpression(LPCTSTR pszSendUser, LPCTSTR pszImagePath, bool bMyselfString)
{
	return true;
}
//�û�����
bool CStringMessage::InsertExpression(LPCTSTR pszSendUser, LPCTSTR pszRecvUser, LPCTSTR pszImagePath)
{
	return true;
}

//�û�����
bool CStringMessage::InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor)
{
	
	CString strMessage=TEXT("");
	strMessage.Format(TEXT("[%s]:%s"),pszSendUser,pszString);

	SendMessageToEngine(strMessage,2);

	return true;
}
//�û�����
bool CStringMessage::InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszRecvUser, LPCTSTR pszString, COLORREF crColor)
{
	return true;
}
//�û�˽��
bool CStringMessage::InsertWhisperChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString)
{
	return true;
}

//�û�����
bool CStringMessage::InsertUserTrumpet( LPCTSTR pszSendUser,LPCTSTR pszString,COLORREF crColor )
{	
	return true;
}

//�û�����
bool CStringMessage::InsertUserTyphon( LPCTSTR pszSendUser,LPCTSTR pszString,COLORREF crColor )
{
	return true;
}

void CStringMessage::SendMessageToEngine(CString strMessage,int nMessageType)
{
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();

	if (pGameFrameEngine!=NULL)
	{
		pGameFrameEngine->SendMessage(IDM_CHAT_MESSAGE,WPARAM(strMessage.GetBuffer(0)),nMessageType);
	}

}



