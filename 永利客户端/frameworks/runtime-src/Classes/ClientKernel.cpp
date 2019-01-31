#include "ClientKernel.h"
#include "CCLuaEngine.h"
#include "CMD_Data.h"


CClientKernel::CClientKernel()
{
}

CClientKernel::~CClientKernel()
{
}

bool CClientKernel::OnInit()
{
	return true;
}
void CClientKernel::LogOut(const char *message)
{
	CCLOG("[Kernel LogOut]: %s",message);
}
//֪ͨ
bool CClientKernel::OnMessageHandler(int nHandler,WORD wMain,WORD wSub)
{
	bool result = false;
	do
	{
		if(nHandler != 0)
		{
			lua_State* tolua_S=LuaEngine::getInstance()->getLuaStack()->getLuaState();
			toluafix_get_function_by_refid(tolua_S, nHandler);
			if (lua_isfunction(tolua_S, -1))
			{
				lua_pushinteger(tolua_S, wMain);
				lua_pushinteger(tolua_S, wSub);
				int result = LuaEngine::getInstance()->getLuaStack()->executeFunctionByHandler(nHandler, 2)!=0;
			}
			else
			{
				CCLOG("OnUnZipEvent-luacallback-handler-false:%d",nHandler);
			}
		}
	}while(false);
	return result == 0;
}

bool CClientKernel::OnSocketConnectEvent(int nHandler,WORD wMain,WORD wSub,BYTE* pBuffer,WORD wSize)
{
	bool result = false;
	do{	
		CCmd_Data* pData = CCmd_Data::create(0);
		pData->SetCmdInfo(0,1);

		result = OnCallLuaSocketCallBack(nHandler,pData);
	}while(false);
	return result;
}

bool CClientKernel::OnSocketDataEvent(int nHandler,WORD wMain,WORD wSub,BYTE* pBuffer,WORD wSize)
{
	bool result = false;
	do
	{
		CCmd_Data* pData = CCmd_Data::create(wSize);
		pData->SetCmdInfo(wMain,wSub);
		if(wSize > 0 && pBuffer != NULL)
		{
			pData->PushByteData(pBuffer,wSize);
			pData->ResetCurrentIndex();
		}
		result = OnCallLuaSocketCallBack(nHandler,pData);
	}while(false);
	return result;
}

bool CClientKernel::OnSocketErrorEvent(int nHandler,WORD wMain,WORD wSub,BYTE* pBuffer,WORD wSize)
{
	bool result = false;
	do
	{
		CCmd_Data* pData = CCmd_Data::create(2);
		pData->SetCmdInfo(0,2);
		pData->PushByteData((BYTE*)&wMain,2);
		pData->ResetCurrentIndex();
		result = OnCallLuaSocketCallBack(nHandler,pData);

	}while(false);
	return result;
}

bool CClientKernel::OnSocketCloseEvent(int nHandler,WORD wMain,WORD wSub,BYTE* pBuffer,WORD wSize)
{
	bool result = false;
	do
	{
		CCmd_Data* pData = CCmd_Data::create(0);
		pData->SetCmdInfo(0,3);
		result = OnCallLuaSocketCallBack(nHandler,pData);
	}while(false);
	return result;
}

bool CClientKernel::OnCallLuaSocketCallBack(int nHandler,Ref* pData)
{

	bool result = false;
	do
	{
		CCmd_Data* pCmdData = (CCmd_Data*)pData;

		if(nHandler != 0 && pCmdData != NULL)
		{
			lua_State* tolua_S=LuaEngine::getInstance()->getLuaStack()->getLuaState();
			toluafix_get_function_by_refid(tolua_S, nHandler);
			if (lua_isfunction(tolua_S, -1))
			{
				//����
				int nID = (pData) ? pCmdData->_ID : -1;
				int *pLuaID = (pData) ? &pCmdData->_luaID : NULL;
				toluafix_pushusertype_ccobject(tolua_S,nID,pLuaID,(void*)pData, "cc.CCmd_Data");

				LuaEngine::getInstance()->getLuaStack()->executeFunctionByHandler(nHandler, 1);
			}
			else 
                        {
				CCLOG("OnCallLuaSocketCallBack-false:%d",nHandler);
			}
		}
	}while(false);
	return result == 0;
}


void CClientKernel::GlobalUpdate(float dt)
{
	GetMCKernel()->OnMainLoop(this, 100);
}

void CClientKernel::OnMessageRespon(CMessage* message)
{
	if(message)
	{
		switch (message->wType)
		{
		case MSG_SOCKET_CONNECT:
			OnSocketConnectEvent(message->nHandler,message->wMain,message->wSub,message->pData,message->wSize);
			 break;
		case MSG_SOCKET_DATA:
			 OnSocketDataEvent(message->nHandler,message->wMain,message->wSub,message->pData,message->wSize);
			  break;
		case MSG_SOCKET_ERROR:
			 OnSocketErrorEvent(message->nHandler,message->wMain,message->wSub,message->pData,message->wSize);
			  break;
		case MSG_SOCKET_CLOSED:
			 OnSocketCloseEvent(message->nHandler,message->wMain,message->wSub,message->pData,message->wSize);
			 break;
		default:
			CCLOG("[_DEBUG]	unkown_message:[type:%d][main:%d][sub:%d][size:%d]",message->wType,message->wMain,message->wSub,message->wSize);
			break;
		}
	}

}