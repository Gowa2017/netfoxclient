#ifndef _CMD_DATA_H_
#define _CMD_DATA_H_
#include "Define.h"
#include "cocos2d.h"
#include "tolua_fix.h"
#include "CCLuaEngine.h"
#include "LuaAssert.h"
#include "LuaBasicConversions.h"
#include "Integer64.h"

NS_CC_BEGIN
#ifdef __cplusplus
extern "C" {
#endif

#define AUTO_LEN		512

//网络数据处理
class CCmd_Data :public cocos2d::Ref
{
public:
	BYTE*		m_pBuffer;			//数据缓存

protected:
	WORD		m_wMain;			//主命令
	WORD		m_wSub;				//子命令
	
	WORD		m_wOpacity;		//容量
	WORD		m_wLength;		//数据实际长度
	WORD		m_wCurIndex;		//操作游标

	bool        m_bAutoLen;

public:
	//构造函数
	CCmd_Data(WORD nLenght);
	//析构函数
	virtual ~CCmd_Data();

public:
	//创建对象
	static CCmd_Data *create(int nLenght);

public:
	//设置命令
	VOID	SetCmdInfo(WORD wMain,WORD wSub);
	//填充数据
	WORD	PushByteData(BYTE* cbData,WORD wLenght);
	//获取主命令
	WORD	GetMainCmd(){return m_wMain;}
	//获取子命令
	WORD	GetSubCmd(){return m_wSub;}
	//获取数据
	BYTE * GetBufferData() { return m_pBuffer; }
	//数据长度
	WORD	GetBufferLenght() { return m_wLength; }
	//当前位置
	WORD	GetCurrentIndex(){return m_wCurIndex;}
	//设置游标
	VOID	SetCurrentIndex(WORD wIndex){if(wIndex<= m_wOpacity)m_wCurIndex = wIndex;}
	//重置游标
	VOID	ResetCurrentIndex(){m_wCurIndex = 0 ;}
};
NS_CC_END

USING_NS_CC;

//创建对象
static int toLua_Cmd_Data_create(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	CCmd_Data* tolua_ret = nullptr;
	if(argc == 2)
	{
		int nLenght = lua_tointeger(tolua_S,2);
		tolua_ret = (CCmd_Data*)CCmd_Data::create(nLenght);
	}else{
		tolua_ret = (CCmd_Data*)CCmd_Data::create(0);
		CCLOG("WARN this is cmd_data is auto! init auto is %d",AUTO_LEN);
	}
	int nID = (tolua_ret) ? tolua_ret->_ID : -1;
	int *pLuaID = (tolua_ret) ? &tolua_ret->_luaID : NULL;

	toluafix_pushusertype_ccobject(tolua_S,nID,pLuaID,(void*)tolua_ret, "cc.CCmd_Data");
	return 1;
}
//设置命令
static int toLua_Cmd_Data_setCmdInfo(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 3)
		{
			WORD wMain = (WORD)lua_tointeger(tolua_S,2);
			WORD wSub = (WORD)lua_tointeger(tolua_S,3);
			cobj->SetCmdInfo(wMain,wSub);
			//CCLOG("toLua_Cmd_Data_setCmdInfo main:%d sub:%d curLen:%d",wMain,wSub,cobj->GetCurrentIndex());
		}
	}
	return 0;
}

//填充数据
static int toLua_Cmd_Data_pushData(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S, 1, nullptr);
	if (cobj)
	{
		int argc = lua_gettop(tolua_S);
		if (argc == 3)
		{
			auto pData = lua_topointer(tolua_S, 2);
			WORD wDataLen = lua_tointeger(tolua_S, 3);
			cobj->PushByteData((BYTE*)pData, wDataLen);
			//CCLOG("toLua_Cmd_Data_pushBYTE curLen:%d",cobj->GetCurrentIndex());
		}
		else
		{
			assert(false);
		}
	}
	return 0;
}

//填充bool
static int toLua_Cmd_Data_pushBOOL(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 2)
		{
			BYTE cbValue = ((lua_toboolean(tolua_S,2)== 0)?FALSE:TRUE);
			cobj->PushByteData(&cbValue ,1);
			//CCLOG("toLua_Cmd_Data_pushBOOL curLen:%d",cobj->GetCurrentIndex());
		}
	}
	return 0;
}
//填充BYTE
static int toLua_Cmd_Data_pushBYTE(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 2)
		{
			BYTE cbValue = (BYTE)lua_tointeger(tolua_S,2);
			cobj->PushByteData(&cbValue,1);
			//CCLOG("toLua_Cmd_Data_pushBYTE curLen:%d",cobj->GetCurrentIndex());
		}
	}
	return 0;
}
//填充WORD
static int toLua_Cmd_Data_pushWORD(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 2)
		{
			WORD wValue =  (WORD)lua_tointeger(tolua_S,2);
			cobj->PushByteData((BYTE*)&wValue,2);
			//CCLOG("toLua_Cmd_Data_pushWORD curLen:%d",cobj->GetCurrentIndex());
		}
	}
	return 0;
}

//填充SHORT
static int toLua_Cmd_Data_pushSHORT(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 2)
		{
			SHORT wValue =  (SHORT)lua_tointeger(tolua_S,2);
			cobj->PushByteData((BYTE*)&wValue,2);
		}
	}
	return 0;
}

//填充int
static int toLua_Cmd_Data_pushINT(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 2)
		{
			int dwValue =  lua_tointeger(tolua_S,2);
			cobj->PushByteData((BYTE*)&dwValue,4);
			//CCLOG("toLua_Cmd_Data_pushINT curLen:%d",cobj->GetCurrentIndex());
		}
	}
	return 0;
}
//填充DWORD
static int toLua_Cmd_Data_pushDWORD(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 2)
		{
			DWORD dwValue = (DWORD)lua_tonumber(tolua_S,2);
			cobj->PushByteData((BYTE*)&dwValue,4);
			//CCLOG("toLua_Cmd_Data_pushDWORD curLen:%d",cobj->GetCurrentIndex());
		}
	}
	return 0;
}
//填充Float
static int toLua_Cmd_Data_pushFLOAT(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 2)
		{
			
			float fValue =  (float)lua_tonumber(tolua_S,2);
			cobj->PushByteData((BYTE*)&fValue,4);
			//CCLOG("toLua_Cmd_Data_pushFLOAT curLen:%d",cobj->GetCurrentIndex());
		}
	}
	return 0;
}
//填充DOUBLE
static int toLua_Cmd_Data_pushDOUBLE(lua_State* tolua_S)
{

	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 2)
		{
			DOUBLE dValue =  (DOUBLE)lua_tonumber(tolua_S,2);
			cobj->PushByteData((BYTE*)&dValue,8);
			//CCLOG("toLua_Cmd_Data_pushDOUBLE curLen:%d",cobj->GetCurrentIndex());
			return 1;
		}
	}
	return 0;
}
//填充I64
static int toLua_Cmd_Data_pushSCORE(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 2)
		{
			Integer64* data64 = (Integer64*)tolua_tousertype(tolua_S,2,nullptr);
			if(data64)
			{
				int64_t data = data64->m_val;
				cobj->PushByteData((BYTE*)&data,8);
			}
			else
			{
				int64_t data =  (int64_t)lua_tonumber(tolua_S,2);
				cobj->PushByteData((BYTE*)&data,8);
			}
			//CCLOG("toLua_Cmd_Data_pushSCORE curLen:%d",cobj->GetCurrentIndex());
		}
	}
	return 0;
}
//填充文本
static int toLua_Cmd_Data_pushSTRING(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 3)
		{
			const char* szData = lua_tostring(tolua_S,2);
			WORD wDstLen = lua_tointeger(tolua_S,3);
			do
			{
				if(!EMPTY_CHAR(szData))
				{
					WORD wSrcLen = strlen(szData); 
					TCHAR *szTmp = new TCHAR[wDstLen+1];
					memset(szTmp,0,(wDstLen+1)*2);
					char* szSrc = new char[wSrcLen+1];
					memset(szSrc,0,wSrcLen+1);
					memcpy(szSrc,szData,wSrcLen);
					ToServerString(szSrc,wSrcLen,(char*)szTmp,wDstLen*2);
					cobj->PushByteData((BYTE*)szTmp,wDstLen*2);
					CC_SAFE_DELETE(szTmp);
					CC_SAFE_DELETE(szSrc);
				}
				else
				{
					cobj->PushByteData(NULL,wDstLen*2);
				}

				//CCLOG("toLua_Cmd_Data_pushSTRING curLen:%d",cobj->GetCurrentIndex());
			}while(false);
		}
	}
	return 0;
}

//读取bool
static int toLua_Cmd_Data_readBOOL(lua_State* tolua_S)
{
	//CCLOG("toLua_Cmd_Data_readBOOL");
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		WORD wCurIndex = cobj->GetCurrentIndex();
		if( wCurIndex < cobj->GetBufferLenght())
		{
			lua_pushboolean(tolua_S, cobj->m_pBuffer[wCurIndex]==0?0:1);
			cobj->SetCurrentIndex(wCurIndex+1);
			return 1;
		}
	}
	return 0;
}
//读取byte
static int toLua_Cmd_Data_readBYTE(lua_State* tolua_S)
{
	//CCLOG("toLua_Cmd_Data_readBYTE");
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		WORD wCurIndex = cobj->GetCurrentIndex();
		if( wCurIndex < cobj->GetBufferLenght())
		{
			lua_pushinteger(tolua_S, (0x000000FF&cobj->m_pBuffer[wCurIndex]));
			cobj->SetCurrentIndex(wCurIndex+1);
			return 1;
		}

	}
	return 0;
}
//读取word
static int toLua_Cmd_Data_readWORD(lua_State* tolua_S)
{
	//CCLOG("toLua_Cmd_Data_readWORD");
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		WORD wCurIndex = cobj->GetCurrentIndex();
		if( wCurIndex+2 <= cobj->GetBufferLenght())
		{
			BYTE tmp[2] = {0};
            memcpy(tmp, (void*)(cobj->m_pBuffer+wCurIndex), 2);
			lua_pushinteger(tolua_S, *(WORD*)(tmp));
			cobj->SetCurrentIndex(wCurIndex+2);
			return 1;
		}
	}
	return 0;
}

//读取short
static int toLua_Cmd_Data_readSHORT(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		SHORT wCurIndex = cobj->GetCurrentIndex();
		if( wCurIndex+2 <= cobj->GetBufferLenght())
		{
			BYTE tmp[2] = {0};
            memcpy(tmp, (void*)(cobj->m_pBuffer+wCurIndex), 2);
			lua_pushinteger(tolua_S, *(SHORT*)(tmp));
			cobj->SetCurrentIndex(wCurIndex+2);
			return 1;
		}
	}
	return 0;
}

//读取int
static int toLua_Cmd_Data_readINT(lua_State* tolua_S)
{
	//CCLOG("toLua_Cmd_Data_readINT");
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		WORD wIndex = cobj->GetCurrentIndex();
		if(wIndex + 4 <= cobj->GetBufferLenght())
		{
			BYTE tmp[4] = {0};
            memcpy(tmp, (void*)(cobj->m_pBuffer+wIndex), 4);
			lua_pushinteger(tolua_S, *(int*)(tmp));
			cobj->SetCurrentIndex(wIndex+4);
			return 1;
		}
	}
	return 0;
}
//读取DWORD
static int toLua_Cmd_Data_readDWORD(lua_State* tolua_S)
{
	//CCLOG("toLua_Cmd_Data_readDWORD");
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		WORD wIndex = cobj->GetCurrentIndex();
		if(wIndex + 4 <= cobj->GetBufferLenght())
		{
			BYTE tmp[4] = {0};
            memcpy(tmp, (void*)(cobj->m_pBuffer+wIndex), 4);
            lua_pushnumber(tolua_S,*(DWORD*)(tmp));
			cobj->SetCurrentIndex(wIndex+4);
			return 1;
		}
	}
	return 0;
}
//读取float
static int toLua_Cmd_Data_readFLOAT(lua_State* tolua_S)
{
	//CCLOG("toLua_Cmd_Data_readFLOAT");
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		WORD wIndex = cobj->GetCurrentIndex();
		if(wIndex + 4 <= cobj->GetBufferLenght())
		{
			BYTE tmp[4] = {0};
            memcpy(tmp, (void*)(cobj->m_pBuffer+wIndex), 4);
			lua_pushnumber(tolua_S, *(float*)(tmp));
			cobj->SetCurrentIndex(wIndex+4);
			return 1;
		}
	}
	return 0;
}
//读取double
static int toLua_Cmd_Data_readDOUBLE(lua_State* tolua_S)
{
	//CCLOG("toLua_Cmd_Data_readDOUBLE");
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		WORD wIndex = cobj->GetCurrentIndex();

		if(wIndex + 8 <= cobj->GetBufferLenght())
		{
			BYTE cbBuffer[8] = {0};
			memcpy(cbBuffer,cobj->m_pBuffer+wIndex,8);
			double dData = *((double*)cbBuffer);
			lua_pushnumber(tolua_S, dData);
			cobj->SetCurrentIndex(wIndex+8);
			return 1;
		}

	}
	return 0;
}
//读取score
static int toLua_Cmd_Data_readSCORE(lua_State* tolua_S)
{
	//CCLOG("toLua_Cmd_Data_readSCORE");
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	Integer64* data64	= (Integer64*)tolua_tousertype(tolua_S,2,nullptr);
	do
	{
		if(cobj == NULL)
		{
			CCLOG("toLua_Cmd_Data_readSCORE error cobj is null");
			break;
		}
		if(data64 == NULL)
		{
			CCLOG("toLua_Cmd_Data_readSCORE error data64 is null");
			break;
		}
		WORD wIndex = cobj->GetCurrentIndex();

		if(wIndex + 8 <= cobj->GetBufferLenght())
		{
			BYTE cbData[8] = {0};
			memcpy(cbData,cobj->m_pBuffer+wIndex,8);
			data64->m_val = *((int64_t*)cbData);
			cobj->SetCurrentIndex(wIndex+8);
			return 1;
		}
		CCLOG("toLua_Cmd_Data_readSCORE error wIndex is longer");
	}while(false);

	return 0;
}
//读取string
static int toLua_Cmd_Data_readSTRING(lua_State* tolua_S)
{
	//CCLOG("toLua_Cmd_Data_readSTRING");
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		int argc = lua_gettop(tolua_S);
		if(argc == 1)
		{
			WORD wIndex =  cobj->GetCurrentIndex();
			
			if(wIndex<cobj->GetBufferLenght())
			{
				WORD wMaxLen = cobj->GetBufferLenght() - wIndex;
				char*	pszDstData = new char[1024];
				TCHAR*	pszSrcData = new TCHAR[wMaxLen+1];
				
				memset(pszDstData,0,1024);
				memset(pszSrcData,0,wMaxLen*2+2);

				memcpy(pszSrcData,cobj->m_pBuffer+wIndex,wMaxLen*2);

				ToClientString((char*)pszSrcData,wMaxLen*2,pszDstData,1024);
				lua_pushstring(tolua_S,pszDstData);
				CC_SAFE_DELETE(pszDstData);
				CC_SAFE_DELETE(pszSrcData);
				cobj->SetCurrentIndex(wIndex+wMaxLen*2);
				return 1;
			}else{
				CCLOG("readstring error curLen:%d maxlen:%d",wIndex,cobj->GetBufferLenght());
			}
		}
		if(argc == 2)
		{
			WORD wIndex =  cobj->GetCurrentIndex();
			WORD wMaxLen = (WORD)lua_tointeger(tolua_S,2);
			if(wMaxLen>0 && wIndex + wMaxLen*2 <= cobj->GetBufferLenght())
			{
				char*	pszDstData = new char[1024];
				TCHAR*	pszSrcData = new TCHAR[wMaxLen+1];
				
				memset(pszDstData,0,1024);
				memset(pszSrcData,0,wMaxLen*2+2);

				memcpy(pszSrcData,cobj->m_pBuffer+wIndex,wMaxLen*2);

				ToClientString((char*)pszSrcData,wMaxLen*2,pszDstData,1024);
				lua_pushstring(tolua_S,pszDstData);
				CC_SAFE_DELETE(pszDstData);
				CC_SAFE_DELETE(pszSrcData);
				cobj->SetCurrentIndex(wIndex+wMaxLen*2);
				return 1;
			}else{
				CCLOG("readstring error readlen:%d curLen:%d maxlen:%d",wMaxLen,wIndex,cobj->GetBufferLenght());
			}
		}

	}
	return 0;
}
//获取主命令
static int toLua_Cmd_Data_getMain(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		lua_pushinteger(tolua_S,cobj->GetMainCmd());
		return 1;
	}
	return 0;
}
//获取子命令
static int toLua_Cmd_Data_getSub(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		lua_pushinteger(tolua_S,cobj->GetSubCmd());
		return 1;
	}
	return 0;
}
//获取长度
static int toLua_Cmd_Data_len(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		lua_pushinteger(tolua_S,cobj->GetBufferLenght());
		return 1;
	}
	return 0;
}

//获取Buffer数据
static int toLua_Cmd_Data_buffer(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S, 1, nullptr);
	if (cobj)
	{
		lua_pushlightuserdata(tolua_S, cobj->GetBufferData());
		return 1;
	}
	return 0;
}

//获取长度
static int toLua_Cmd_Data_curlen(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		lua_pushinteger(tolua_S,cobj->GetCurrentIndex());
		return 1;
	}
	return 0;	
}
//重置游标
static int toLua_Cmd_Data_resetRead(lua_State* tolua_S)
{
	CCmd_Data* cobj = (CCmd_Data*)tolua_tousertype(tolua_S,1,nullptr);
	if (cobj) 
	{
		cobj->ResetCurrentIndex();
		return 1;
	}
	return 0;
}
//Lua注册
static int register_all_cmd_data()
{
	auto engine = LuaEngine::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(engine);
    lua_State* tolua_S = engine->getLuaStack()->getLuaState();

	tolua_usertype(tolua_S,"cc.CCmd_Data");
	tolua_cclass(tolua_S,"CCmd_Data","cc.CCmd_Data","cc.Node",nullptr);
	tolua_beginmodule(tolua_S,"CCmd_Data");
	tolua_function(tolua_S,"create",toLua_Cmd_Data_create);
	tolua_function(tolua_S,"setcmdinfo",toLua_Cmd_Data_setCmdInfo);
	tolua_function(tolua_S,"getmain",toLua_Cmd_Data_getMain);
	tolua_function(tolua_S,"getsub",toLua_Cmd_Data_getSub);
	tolua_function(tolua_S,"getlen",toLua_Cmd_Data_len);
	tolua_function(tolua_S,"getbuffer", toLua_Cmd_Data_buffer);
	tolua_function(tolua_S,"getcurlen",toLua_Cmd_Data_curlen);
	tolua_function(tolua_S,"pushbool",toLua_Cmd_Data_pushBOOL);
	tolua_function(tolua_S,"pushData",toLua_Cmd_Data_pushData);
	tolua_function(tolua_S,"pushbyte",toLua_Cmd_Data_pushBYTE);
	tolua_function(tolua_S,"pushword",toLua_Cmd_Data_pushWORD);
	tolua_function(tolua_S,"pushshort",toLua_Cmd_Data_pushSHORT);
	tolua_function(tolua_S,"pushint",toLua_Cmd_Data_pushINT);
	tolua_function(tolua_S,"pushdword",toLua_Cmd_Data_pushDWORD);
	tolua_function(tolua_S,"pushfloat",toLua_Cmd_Data_pushFLOAT);
	tolua_function(tolua_S,"pushdouble",toLua_Cmd_Data_pushDOUBLE);
	tolua_function(tolua_S,"pushscore",toLua_Cmd_Data_pushSCORE);
	tolua_function(tolua_S,"pushstring",toLua_Cmd_Data_pushSTRING);
	tolua_function(tolua_S,"resetread",toLua_Cmd_Data_resetRead);
	tolua_function(tolua_S,"readbool",toLua_Cmd_Data_readBOOL);
	tolua_function(tolua_S,"readbyte",toLua_Cmd_Data_readBYTE);
	tolua_function(tolua_S,"readword",toLua_Cmd_Data_readWORD);
	tolua_function(tolua_S,"readshort",toLua_Cmd_Data_readSHORT);
	tolua_function(tolua_S,"readint",toLua_Cmd_Data_readINT);
	tolua_function(tolua_S,"readdword",toLua_Cmd_Data_readDWORD);
	tolua_function(tolua_S,"readfloat",toLua_Cmd_Data_readFLOAT);
	tolua_function(tolua_S,"readdouble",toLua_Cmd_Data_readDOUBLE);
	tolua_function(tolua_S,"readscore",toLua_Cmd_Data_readSCORE);
	tolua_function(tolua_S,"readstring",toLua_Cmd_Data_readSTRING);
	tolua_endmodule(tolua_S);

	std::string typeName = typeid(cocos2d::CCmd_Data).name();
    g_luaType[typeName] = "cc.CCmd_Data";
    g_typeCast["CCmd_Data"] = "cc.CCmd_Data";
	return 1;
}

#ifdef __cplusplus
}
#endif

#endif