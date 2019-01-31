//
//  CircleBy.cpp
//  GameProject
//
//  Created by 汤淼 on 16/9/22.
//
//

#include "CircleBy.h"
#if CC_ENABLE_SCRIPT_BINDING
#include "CCLuaEngine.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#endif

CirCleBy *CirCleBy::create(float t, cocos2d::Vec2 circleCenter, float radius)
{
    
    CirCleBy *ret = new CirCleBy();
    ret->init(t,  circleCenter,  radius);
    ret->autorelease();
    
    return ret;
    
}


bool CirCleBy::init(float t, cocos2d::Vec2 circleCenter, float radius)
{
    
    if (ActionInterval::initWithDuration(t))
    {
        
        
        m_circleCenter = circleCenter;
        m_raduis = radius;
        
        m_radian = 2.0 *M_PI/(t/Director::getInstance()->getAnimationInterval());
        
        m_moveTimes=1;
        return true;
 
    }
    
    return false;
}

void CirCleBy::update(float dt)
{
    float radian = m_radian * m_moveTimes ; //每次偏转的弧度 = 单位弧度 * 运动次数
    
    float x = m_raduis * sin(radian);
    float y = m_raduis * cos(radian);
    
    Vec2 newPos = Vec2(x + m_circleCenter.x, y + m_circleCenter.y);
    
    if (_target)
    {
        _target->setPosition(newPos);
    }
    
    m_moveTimes += 1;
}


#if CC_ENABLE_SCRIPT_BINDING

static int tolua_CircleBy_create(lua_State *tolua_S)
{
    int argc = lua_gettop(tolua_S);
    CirCleBy *obj = nullptr;
    if (3 == argc-1)
    {   
        
        double arg0 = lua_tonumber(tolua_S, 2);
        cocos2d::Vec2 arg1;
        luaval_to_vec2(tolua_S,3, &arg1, "tolua_CircleBy_create");
        double arg2 = lua_tonumber(tolua_S, 4);
        obj = CirCleBy::create(arg0,arg1,arg2);
    }

    int nID = (nullptr != obj) ? obj->_ID : -1;
    int *pLuaID = (nullptr != obj) ? &obj->_luaID : nullptr;
    toluafix_pushusertype_ccobject(tolua_S,nID,pLuaID,(void*)obj, "cc.CircleBy");

    return 1;

}


int register_all_Circleasset()
{
#if CC_ENABLE_SCRIPT_BINDING
    lua_State* tolua_S = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    
    tolua_usertype(tolua_S, "cc.CircleBy");
    tolua_cclass(tolua_S,"CirCleBy","cc.CircleBy","cc.ActionInterval",nullptr);
    
    tolua_beginmodule(tolua_S,"CirCleBy");
  
    tolua_function(tolua_S,"create",tolua_CircleBy_create);
  
    tolua_endmodule(tolua_S);
    
    std::string typeName = typeid(CirCleBy).name();
    g_luaType[typeName] = "cc.CircleBy";
    g_typeCast["CirCleBy"] = "cc.CircleBy";
#endif
    return 1;
}

#endif
