#ifndef _LUA_INTEGER64_H_
#define _LUA_INTEGER64_H_

#include "cocos2d.h"
#include "tolua_fix.h"
#include "CCLuaEngine.h"


NS_CC_BEGIN
class Integer64 :public cocos2d::Node 
{
public:
	int64_t		m_val;

public:
    Integer64(int64_t val):m_val(val){}
	virtual ~Integer64(){/*CCLOG("[_DEBUG]	~Integer64:%I64d",m_val);*/}
};
NS_CC_END
USING_NS_CC;

static int64_t toint64(lua_State* tolua_S ,int nIndex)
{
	int nType = lua_type(tolua_S,nIndex);
		
	int64_t data = 0;

	if(nType == LUA_TUSERDATA){
		Integer64* cobj =(Integer64*)tolua_tousertype(tolua_S,nIndex,0);
		if(cobj != nullptr){
			data = cobj->m_val;
		}
	}
	else if (nType == LUA_TNUMBER){
		data = (int64_t)lua_tonumber(tolua_S, nIndex);
	}else{
		CCLOG("[_DEBUG]	_toint64_error[nIndex:%d][nType:%d]",nIndex,nType);
	}
	return data;
}

static int new_Integer64(lua_State* tolua_S ,int64_t data)
{
	if(tolua_S)
	{
		Integer64* cobj =new Integer64(data);
		cobj->autorelease();
		int nID = (cobj) ? cobj->_ID : -1;
		int *pLuaID = (cobj) ? &cobj->_luaID : NULL;
		toluafix_pushusertype_ccobject(tolua_S,nID,pLuaID,(void*)cobj, "cc.Integer64");
		return 1;
	}
	return 0;
}

static int toLua_Integer64(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);

	if(argc == 0)
	{
		return new_Integer64(tolua_S,0);
	}
	else if (argc == 1)
	{
		int64_t tmpdata = (int64_t)lua_tonumber(tolua_S,1);
		return new_Integer64(tolua_S,tmpdata);
	}
	return 0;
}

// lua加法
static int toLua_Integer64_add(lua_State* tolua_S)
{
	int64_t data1= toint64(tolua_S,1);
	int64_t data2= toint64(tolua_S,2);
	return new_Integer64(tolua_S,data1+data2);
}

// lua减法
static int toLua_Integer64_sub(lua_State* tolua_S)
{
	int64_t data1= toint64(tolua_S,1);
	int64_t data2= toint64(tolua_S,2);
	return new_Integer64(tolua_S,data1-data2);
}

// lua乘法
static int toLua_Integer64_mul(lua_State* tolua_S)
{
	int64_t data1= toint64(tolua_S,1);
	int64_t data2= toint64(tolua_S,2);
	return new_Integer64(tolua_S,data1*data2);
}

// lua除法
static int toLua_Integer64_div(lua_State* tolua_S)
{
	int64_t data1= toint64(tolua_S,1);
	int64_t data2= toint64(tolua_S,2);
	if (data2 == 0) {
		return luaL_error(tolua_S, "div by zero");
	}
	return new_Integer64(tolua_S,data1/data2);
}

// lua取模
static int toLua_Integer64_mod(lua_State* tolua_S)
{
	int64_t data1 = toint64(tolua_S,1);
	int64_t data2 = toint64(tolua_S,2);
	if (data2 == 0) {
		return luaL_error(tolua_S, "mod by zero");
	}
	return new_Integer64(tolua_S,data1%data2);
}

// lua取负
static int toLua_Integer64_unm(lua_State* tolua_S)
{
	int64_t data1= -toint64(tolua_S,1);
	return new_Integer64(tolua_S,data1);
}

static int64_t _pow64(int64_t a, int64_t b) {
	if (b == 1) {
		return a;
	}
	int64_t a2 = a * a;
	if (b % 2 == 1) {
		return _pow64(a2, b/2) * a;
	} else {
		return _pow64(a2, b/2);
	}
}

// lua幂
static int toLua_Integer64_pow(lua_State* tolua_S)
{
	int64_t data1 = toint64(tolua_S,1);
	int64_t data2 = toint64(tolua_S,2);
	int64_t p;
	if (data2 > 0) {
		p = _pow64(data1,data2);
	} else if (data2 == 0) {
		p = 1;
	} else {
		return luaL_error(tolua_S, "pow by nagtive number %d",(int)data2);
	} 
	return new_Integer64(tolua_S,p);
}

// lua ==
static int toLua_Integer64_eq(lua_State* tolua_S)
{
	int64_t data1 = toint64(tolua_S,1);
	int64_t data2 = toint64(tolua_S,2);
	lua_pushboolean(tolua_S,data1 == data2);
	return 1;
}
// lua <
static int toLua_Integer64_lt(lua_State* tolua_S)
{
	int64_t data1 = toint64(tolua_S,1);
	int64_t data2 = toint64(tolua_S,2);
	lua_pushboolean(tolua_S,data1 < data2);
	return 1;
}
// lua<=
static int toLua_Integer64_le(lua_State* tolua_S)
{
	int64_t data1 = toint64(tolua_S,1);
	int64_t data2 = toint64(tolua_S,2);
	lua_pushboolean(tolua_S,data1 <= data2);
	return 1;
}

static int toLua_Integer64_setvalue(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		Integer64* cobjsrc =(Integer64*)tolua_tousertype(tolua_S,1,0);
		Integer64* cobjdst =(Integer64*)tolua_tousertype(tolua_S,2,0);

		if(cobjsrc != nullptr && cobjdst != nullptr){
			cobjsrc->m_val = cobjdst->m_val;
			return 0;
		}
		int64_t nData = (int64_t)lua_tonumber(tolua_S,2);
		cobjsrc->m_val = nData;
		return 0;
	}
	else
	{
		CCLOG("[error]	toLua_Integer64_setvalue arg num #%d not 2",argc);
	}

	return 0;
}
static int toLua_Integer64_getvalue(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{
		Integer64* cobjsrc =(Integer64*)tolua_tousertype(tolua_S,1,0);
		
		double data = 0;
		if(cobjsrc != nullptr ){
			data = (double)cobjsrc->m_val;
		}
		lua_pushnumber(tolua_S,data);
		return 1;
	}
	else
	{
		CCLOG("[error]	toLua_Integer64_getvalue arg num #%d not 1",argc);
	}

	return 0;
}

static int toLua_Integer64_addfun(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		Integer64* cobjsrc =(Integer64*)tolua_tousertype(tolua_S,1,0);
		Integer64* cobjop=(Integer64*)tolua_tousertype(tolua_S,2,0);
		if (cobjsrc == NULL)
		{
			CCLOG("[error]	toLua_Integer64_addfun null src");
			return 0;
		}
		if ( cobjop != NULL )
		{
			cobjsrc->m_val = cobjsrc->m_val + cobjop->m_val;
		}
		else
		{
			int64_t nData = (int64_t)lua_tonumber(tolua_S,2);
			cobjsrc->m_val = cobjsrc->m_val + nData;
		}
	}
	else
	{
		CCLOG("[error]	toLua_Integer64_addfun arg num #%d not 2",argc);
	}

	return 0;
}

static int toLua_Integer64_subfun(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		Integer64* cobjsrc =(Integer64*)tolua_tousertype(tolua_S,1,0);
		Integer64* cobjop=(Integer64*)tolua_tousertype(tolua_S,2,0);
		if (cobjsrc == NULL)
		{
			CCLOG("[error]	toLua_Integer64_subfun null src");
			return 0;
		}
		if ( cobjop != NULL )
		{
			cobjsrc->m_val = cobjsrc->m_val - cobjop->m_val;
		}
		else
		{
			int64_t nData = (int64_t)lua_tonumber(tolua_S,2);
			cobjsrc->m_val = cobjsrc->m_val - nData;
		}
	}
	else
	{
		CCLOG("[error]	toLua_Integer64_subfun arg num #%d not 2",argc);
	}

	return 0;
}

// lua文本
static int toLua_Integer64_tostringfun(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{
		Integer64* cobjsrc =(Integer64*)tolua_tousertype(tolua_S,1,0);
		
		char szData[64] = {0};
#ifdef WIN32
		sprintf(szData,"%I64d",cobjsrc->m_val);
#else
		sprintf(szData,"%lld",cobjsrc->m_val);
#endif
		lua_pushlstring(tolua_S, szData, strlen(szData));
		return 1;
	}
	return 0;
}

static int toLua_Integer64_morefun(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		bool result = false;
		Integer64* cobjsrc =(Integer64*)tolua_tousertype(tolua_S,1,0);
		Integer64* cobjop=(Integer64*)tolua_tousertype(tolua_S,2,0);
		if (cobjsrc == NULL)
		{
			CCLOG("[error]	toLua_Integer64_morefun null src");
			return 0;
		}
		if ( cobjop != NULL )
		{
			result = (cobjsrc->m_val > cobjop->m_val);
		}
		else
		{
			int64_t nData = (int64_t)lua_tonumber(tolua_S,2);
			result = (cobjsrc->m_val > nData);
		}
		lua_pushboolean(tolua_S,result);
		return 1;
	}
	return 0;
}

static int toLua_Integer64_moreexfun(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		bool result = false;
		Integer64* cobjsrc =(Integer64*)tolua_tousertype(tolua_S,1,0);
		Integer64* cobjop=(Integer64*)tolua_tousertype(tolua_S,2,0);
		if (cobjsrc == NULL)
		{
			CCLOG("[error]	toLua_Integer64_morefun null src");
			return 0;
		}
		if ( cobjop != NULL )
		{
			result = (cobjsrc->m_val >= cobjop->m_val);
		}
		else
		{
			int64_t nData = (int64_t)lua_tonumber(tolua_S,2);
			result = (cobjsrc->m_val >= nData);
		}
		lua_pushboolean(tolua_S,result);
		return 1;
	}
	return 0;
}

static int toLua_Integer64_lessfun(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		bool result = false;
		Integer64* cobjsrc =(Integer64*)tolua_tousertype(tolua_S,1,0);
		Integer64* cobjop=(Integer64*)tolua_tousertype(tolua_S,2,0);
		if (cobjsrc == NULL)
		{
			CCLOG("[error]	toLua_Integer64_morefun null src");
			return 0;
		}
		if ( cobjop != NULL )
		{
			result = (cobjsrc->m_val < cobjop->m_val);
		}
		else
		{
			int64_t nData = (int64_t)lua_tonumber(tolua_S,2);
			result = (cobjsrc->m_val < nData);
		}
		lua_pushboolean(tolua_S,result);
		return 1;
	}
	return 0;
}

static int toLua_Integer64_lessexfun(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		bool result = false;
		Integer64* cobjsrc =(Integer64*)tolua_tousertype(tolua_S,1,0);
		Integer64* cobjop=(Integer64*)tolua_tousertype(tolua_S,2,0);
		if (cobjsrc == NULL)
		{
			CCLOG("[error]	toLua_Integer64_morefun null src");
			return 0;
		}
		if ( cobjop != NULL )
		{
			result = (cobjsrc->m_val <= cobjop->m_val);
		}
		else
		{
			int64_t nData = (int64_t)lua_tonumber(tolua_S,2);
			result = (cobjsrc->m_val <= nData);
		}
		lua_pushboolean(tolua_S,result);
		return 1;
	}
	return 0;
}

// lua文本
static int toLua_Integer64_tostring(lua_State* tolua_S)
{
	static  char hex[] = "0123456789ABCDEF";
	int64_t n = toint64(tolua_S,1);
	int argc = lua_gettop(tolua_S);
	int format = argc>1?lua_tonumber(tolua_S, 2):0;
	int shift , mask;
	switch(format) {
		case 0: 
		case 10: {
			int64_t dec = (int64_t)n;
			luaL_Buffer b;
				luaL_buffinit(tolua_S , &b);
			if (dec<0) {
				luaL_addchar(&b, '-');
				dec = -dec;
			}
			int buffer[32];
			int i;
			for (i=0;i<32;i++) {
				buffer[i] = dec%10;
				dec /= 10;
				if (dec == 0)
					break;
			}
			while (i>=0) {
				luaL_addchar(&b, hex[buffer[i]]);
				--i;
			}
			luaL_pushresult(&b);
			return 1;
		}
		case 2:
			shift = 1;
			mask = 1;
			break;
		case 8:
			shift = 3;
			mask = 7;
			break;
		case 16:
			shift = 4;
			mask = 0xf;
			break;
		default:
			luaL_error(tolua_S, "Unsupport base %d",format);
			break;
		}
		int i;
		char buffer[64];
		for (i=0;i<64;i+=shift) {
			buffer[i/shift] = hex[(n>>(64-shift-i)) & mask];
		}
		lua_pushlstring(tolua_S, buffer, 64 / shift);

	return 1;
}


static void register_all_Integer64()
{
	auto engine = LuaEngine::getInstance();
	ScriptEngineManager::getInstance()->setScriptEngine(engine);
	lua_State* tolua_S = engine->getLuaStack()->getLuaState();

    tolua_usertype(tolua_S,"cc.Integer64");
	tolua_cclass(tolua_S,"Integer64","cc.Integer64","cc.Node",nullptr);
	tolua_beginmodule(tolua_S,"Integer64");
	tolua_function(tolua_S,"new",toLua_Integer64);
	tolua_function(tolua_S,"__add",toLua_Integer64_add);
	tolua_function(tolua_S,"__sub",toLua_Integer64_sub);
	tolua_function(tolua_S, "__mul", toLua_Integer64_mul);
	tolua_function(tolua_S, "__div", toLua_Integer64_div);
	tolua_function(tolua_S, "__mod", toLua_Integer64_mod);
	tolua_function(tolua_S, "__unm", toLua_Integer64_unm);
	tolua_function(tolua_S, "__pow", toLua_Integer64_pow);
	tolua_function(tolua_S, "__eq", toLua_Integer64_eq);
	tolua_function(tolua_S, "__lt", toLua_Integer64_lt);
	tolua_function(tolua_S, "__le", toLua_Integer64_le);
	tolua_function(tolua_S, "__tostring", toLua_Integer64_tostringfun);
	tolua_function(tolua_S, "getstring",toLua_Integer64_tostring);
	tolua_function(tolua_S, "setvalue",toLua_Integer64_setvalue);
	tolua_function(tolua_S, "getvalue",toLua_Integer64_getvalue);
	tolua_function(tolua_S, "add",toLua_Integer64_addfun);
	tolua_function(tolua_S, "sub",toLua_Integer64_subfun);
	tolua_function(tolua_S, "more",toLua_Integer64_morefun);
	tolua_function(tolua_S, "less",toLua_Integer64_lessfun);
	tolua_function(tolua_S, "moreex",toLua_Integer64_moreexfun);
	tolua_function(tolua_S, "lessex",toLua_Integer64_lessexfun);
	
	tolua_endmodule(tolua_S); 

	std::string typeName = typeid(cocos2d::Integer64).name();
    g_luaType[typeName] = "cc.Integer64";
    g_typeCast["Integer64"] = "cc.Integer64";
}

#endif