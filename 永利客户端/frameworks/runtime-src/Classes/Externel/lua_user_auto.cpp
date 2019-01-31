#include "lua_user_auto.hpp"
#include "RadioBox.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#include "Externel/lua_user_auto.hpp"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"

int lua_cocos2dx_user_RadioBox_setSolidPaddingBottomY(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingBottomY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "RadioBox:setSolidPaddingBottomY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingBottomY'", nullptr);
            return 0;
        }
        cobj->setSolidPaddingBottomY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:setSolidPaddingBottomY",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingBottomY'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_setSolidPaddingRightX(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingRightX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "RadioBox:setSolidPaddingRightX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingRightX'", nullptr);
            return 0;
        }
        cobj->setSolidPaddingRightX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:setSolidPaddingRightX",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingRightX'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_setShowRadioCount(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_setShowRadioCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "RadioBox:setShowRadioCount");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_setShowRadioCount'", nullptr);
            return 0;
        }
        cobj->setShowRadioCount(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:setShowRadioCount",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_setShowRadioCount'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_addRadioButton(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_addRadioButton'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::RadioButton* arg0;

        ok &= luaval_to_object<cocos2d::ui::RadioButton>(tolua_S, 2, "ccui.RadioButton",&arg0, "RadioBox:addRadioButton");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_addRadioButton'", nullptr);
            return 0;
        }
        cobj->addRadioButton(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        cocos2d::ui::RadioButton* arg0;
        bool arg1;

        ok &= luaval_to_object<cocos2d::ui::RadioButton>(tolua_S, 2, "ccui.RadioButton",&arg0, "RadioBox:addRadioButton");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "RadioBox:addRadioButton");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_addRadioButton'", nullptr);
            return 0;
        }
        cobj->addRadioButton(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:addRadioButton",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_addRadioButton'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_startAttenuatingAutoScroll(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_startAttenuatingAutoScroll'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        cocos2d::Vec2 arg0;
        cocos2d::Vec2 arg1;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "RadioBox:startAttenuatingAutoScroll");

        ok &= luaval_to_vec2(tolua_S, 3, &arg1, "RadioBox:startAttenuatingAutoScroll");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_startAttenuatingAutoScroll'", nullptr);
            return 0;
        }
        cobj->startAttenuatingAutoScroll(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:startAttenuatingAutoScroll",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_startAttenuatingAutoScroll'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_setSolidPaddingTopY(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingTopY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "RadioBox:setSolidPaddingTopY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingTopY'", nullptr);
            return 0;
        }
        cobj->setSolidPaddingTopY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:setSolidPaddingTopY",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingTopY'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_refresh(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_refresh'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_refresh'", nullptr);
            return 0;
        }
        cobj->refresh();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:refresh",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_refresh'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_setSelectedScale(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_setSelectedScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        double arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "RadioBox:setSelectedScale");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "RadioBox:setSelectedScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_setSelectedScale'", nullptr);
            return 0;
        }
        cobj->setSelectedScale(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:setSelectedScale",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_setSelectedScale'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_addFocusEventListener(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_addFocusEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<void (cocos2d::ui::RadioButton *, int, cocos2d::ui::RadioButtonGroup::EventType)> arg0;


		LUA_FUNCTION handle = toluafix_ref_function(tolua_S, 2, 0);
		arg0 = [handle](cocos2d::ui::RadioButton * sender, int index, cocos2d::ui::RadioButtonGroup::EventType eType) {
			LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
			stack->pushObject(sender, "cc.Ref");
			stack->pushInt(index);
			stack->pushInt((int)eType);
			stack->executeFunctionByHandler(handle, 3);
			stack->clean();
		};

        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_addFocusEventListener'", nullptr);
            return 0;
        }
        cobj->addFocusEventListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:addFocusEventListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_addFocusEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_init(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_init'", nullptr);
            return 0;
        }
        bool ret = cobj->init();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:init",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_init'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_setSelectedButtonWithoutEvent(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_setSelectedButtonWithoutEvent'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            cocos2d::ui::RadioButton* arg0;
            ok &= luaval_to_object<cocos2d::ui::RadioButton>(tolua_S, 2, "ccui.RadioButton",&arg0, "RadioBox:setSelectedButtonWithoutEvent");

            if (!ok) { break; }
            cobj->setSelectedButtonWithoutEvent(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "RadioBox:setSelectedButtonWithoutEvent");

            if (!ok) { break; }
            cobj->setSelectedButtonWithoutEvent(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "RadioBox:setSelectedButtonWithoutEvent",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_setSelectedButtonWithoutEvent'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_setSolidPaddingLeftX(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingLeftX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "RadioBox:setSolidPaddingLeftX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingLeftX'", nullptr);
            return 0;
        }
        cobj->setSolidPaddingLeftX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:setSolidPaddingLeftX",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_setSolidPaddingLeftX'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_getNumberOfRadioButtons(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_getNumberOfRadioButtons'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_getNumberOfRadioButtons'", nullptr);
            return 0;
        }
        ssize_t ret = cobj->getNumberOfRadioButtons();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:getNumberOfRadioButtons",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_getNumberOfRadioButtons'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_setMarginX(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_setMarginX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "RadioBox:setMarginX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_setMarginX'", nullptr);
            return 0;
        }
        cobj->setMarginX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:setMarginX",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_setMarginX'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_setSelectedButton(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (RadioBox*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_user_RadioBox_setSelectedButton'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            cocos2d::ui::RadioButton* arg0;
            ok &= luaval_to_object<cocos2d::ui::RadioButton>(tolua_S, 2, "ccui.RadioButton",&arg0, "RadioBox:setSelectedButton");

            if (!ok) { break; }
            cobj->setSelectedButton(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "RadioBox:setSelectedButton");

            if (!ok) { break; }
            cobj->setSelectedButton(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "RadioBox:setSelectedButton",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_setSelectedButton'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_user_RadioBox_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"RadioBox",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_create'", nullptr);
            return 0;
        }
        RadioBox* ret = RadioBox::create();
        object_to_luaval<RadioBox>(tolua_S, "RadioBox",(RadioBox*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "RadioBox:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_user_RadioBox_constructor(lua_State* tolua_S)
{
    int argc = 0;
    RadioBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_user_RadioBox_constructor'", nullptr);
            return 0;
        }
        cobj = new RadioBox();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"RadioBox");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "RadioBox:RadioBox",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_user_RadioBox_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_user_RadioBox_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (RadioBox)");
    return 0;
}

int lua_register_cocos2dx_user_RadioBox(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"RadioBox");
    tolua_cclass(tolua_S,"RadioBox","RadioBox","ccui.ScrollView",nullptr);

    tolua_beginmodule(tolua_S,"RadioBox");
        tolua_function(tolua_S,"new",lua_cocos2dx_user_RadioBox_constructor);
        tolua_function(tolua_S,"setSolidPaddingBottomY",lua_cocos2dx_user_RadioBox_setSolidPaddingBottomY);
        tolua_function(tolua_S,"setSolidPaddingRightX",lua_cocos2dx_user_RadioBox_setSolidPaddingRightX);
        tolua_function(tolua_S,"setShowRadioCount",lua_cocos2dx_user_RadioBox_setShowRadioCount);
        tolua_function(tolua_S,"addRadioButton",lua_cocos2dx_user_RadioBox_addRadioButton);
        tolua_function(tolua_S,"startAttenuatingAutoScroll",lua_cocos2dx_user_RadioBox_startAttenuatingAutoScroll);
        tolua_function(tolua_S,"setSolidPaddingTopY",lua_cocos2dx_user_RadioBox_setSolidPaddingTopY);
        tolua_function(tolua_S,"refresh",lua_cocos2dx_user_RadioBox_refresh);
        tolua_function(tolua_S,"setSelectedScale",lua_cocos2dx_user_RadioBox_setSelectedScale);
        tolua_function(tolua_S,"addFocusEventListener",lua_cocos2dx_user_RadioBox_addFocusEventListener);
        tolua_function(tolua_S,"init",lua_cocos2dx_user_RadioBox_init);
        tolua_function(tolua_S,"setSelectedButtonWithoutEvent",lua_cocos2dx_user_RadioBox_setSelectedButtonWithoutEvent);
        tolua_function(tolua_S,"setSolidPaddingLeftX",lua_cocos2dx_user_RadioBox_setSolidPaddingLeftX);
        tolua_function(tolua_S,"getNumberOfRadioButtons",lua_cocos2dx_user_RadioBox_getNumberOfRadioButtons);
        tolua_function(tolua_S,"setMarginX",lua_cocos2dx_user_RadioBox_setMarginX);
        tolua_function(tolua_S,"setSelectedButton",lua_cocos2dx_user_RadioBox_setSelectedButton);
        tolua_function(tolua_S,"create", lua_cocos2dx_user_RadioBox_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(RadioBox).name();
    g_luaType[typeName] = "RadioBox";
    g_typeCast["RadioBox"] = "RadioBox";
    return 1;
}
TOLUA_API int register_all_cocos2dx_user(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"cc",0);
	tolua_beginmodule(tolua_S,"cc");

	lua_register_cocos2dx_user_RadioBox(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

