#pragma once

#include "Stdafx.h"
class CChatMessage : public CRichEditMessage
{
public:
	CChatMessage(void);
	virtual ~CChatMessage(void);

	//��д����
public:
	//�û�����
	virtual bool InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor);
};

//////////////////////////////////////////////////////////////////////////////////////