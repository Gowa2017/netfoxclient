#ifndef _LUA_CLIENT_KERNEL_H_
#define _LUA_CLIENT_KERNEL_H_

#include "Define.h"
#include "cocos2d.h"
#include "MobileClientKernel.h"
USING_NS_CC;
class CClientKernel: public cocos2d::Node,public IMessageRespon,public ILog
{
public:
	//ππ‘Ï∫Ø ˝
	CClientKernel();
public:
	//Œˆππ∫Ø ˝
	virtual ~CClientKernel();
public:
	//≥ı ºªØ
	bool OnInit();

public:
	bool OnMessageHandler(int nHandler,WORD wMain, WORD wSub);

public:
	// Luaªÿµ˜
	bool OnCallLuaSocketCallBack(int nHandler,Ref* data);

public:
	//¡¨Ω” ¬º˛
	bool OnSocketConnectEvent(int nHandler,WORD wMain,WORD wSub,BYTE* pBuffer,WORD wSize);
	// ˝æ› ¬º˛
	bool OnSocketDataEvent(int nHandler,WORD wMain,WORD wSub,BYTE* pBuffer,WORD wSize);
	//¥ÌŒÛ ¬º˛
	bool OnSocketErrorEvent(int nHandler,WORD wMain,WORD wSub,BYTE* pBuffer,WORD wSize);
	//πÿ±’ ¬º˛
	bool OnSocketCloseEvent(int nHandler,WORD wMain,WORD wSub,BYTE* pBuffer,WORD wSize);
public:
	//»´æ÷∏¸–¬
	void GlobalUpdate(float dt);

public:
	virtual void OnMessageRespon(CMessage* message);

	virtual void LogOut(const char *message);
};



#endif