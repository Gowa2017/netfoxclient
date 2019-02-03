#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "cocos2d.h"

typedef int Lua_CallBack;

/**
@brief    The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by Director.
*/
class  AppDelegate : private cocos2d::Application, public cocos2d::Node
{
	enum AppStatus
	{
		APP_NONE =			0,			//初始状态
		APP_FOCUS =			1,			//其他顶层activity在覆盖cocos2d activity
		APP_UNFOCUS =		2,		//cocos2d activity跳转到其他层activity(暂未实现)
		APP_FOREGROUND =	3,		//进入前台
		APP_BACKGROUND =	4,		//进入后台
	};
static	AppDelegate*	m_instance;

	Node*				m_pClientKernel;
	// Lua_CallBack*       m_SocketEventListener;

	Lua_CallBack        m_BackgroundCallBack;
public:
	Node*			    m_ImageToByte;
	std::unordered_map<std::string, cocos2d::Texture2D*> m_cachedBmpTex;

public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();

    /**
    @brief    Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();

	void GlobalUpdate(float dt);

	static AppDelegate* getAppInstance(){ return m_instance; }

	Node* getClientKernel(){ return m_pClientKernel; }

	// void setSocketEventListener(Node* node){m_SocketEventListener = node;}
	// Node* getSocketEventListener(){return m_SocketEventListener;}

	void setBackGroundListener(Lua_CallBack callback){ m_BackgroundCallBack = callback; }
	Lua_CallBack getBackGroundListener(){ return m_BackgroundCallBack; }
	AppStatus				m_eAppStatus;
};

#endif  // __APP_DELEGATE_H__

