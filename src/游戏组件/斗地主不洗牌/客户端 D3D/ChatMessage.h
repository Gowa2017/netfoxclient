#pragma once

#include "Stdafx.h"
class CChatMessage : public CRichEditMessage
{
public:
	CChatMessage(void);
	virtual ~CChatMessage(void);

	//重写函数
public:
	//用户聊天
	virtual bool InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor);
};

//////////////////////////////////////////////////////////////////////////////////////