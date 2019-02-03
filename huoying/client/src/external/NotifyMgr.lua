--
-- Author: zhong
-- Date: 2016-09-02 11:22:28
--
--[[
Tips
1. 相同的main,sub值,需要注册多个通知,可以设置不同的name
2. 消息处理函数返回false,可以将消息缓存至未处理列表,复用
]]

--网络通知管理
local NotifyMgr = class("NotifyMgr")
--实现单例
NotifyMgr._instance = nil

local MAX_CACHE_NOTIFY = 20
function NotifyMgr:getInstance()
    if nil == NotifyMgr._instance then
        print("new instance")
        NotifyMgr._instance = NotifyMgr:create()
    end
    return NotifyMgr._instance
end

function NotifyMgr:ctor()
    --通知管理
    self.m_tabNofityMgr = {}
    --通知列表
    self.m_tabListNotify = {}
    --未读通知列表
    self.m_tabUnreadNotify = {}
    --暂停通知列表
    self.m_tabSleepNotify = {}
    --通知id索引
    self.m_nNotifyId = 0
end

--清理
function NotifyMgr:clear()
    --通知列表
    self.m_tabListNotify = {}
    --未读通知列表
    self.m_tabUnreadNotify = {}
    --暂停通知列表
    self.m_tabSleepNotify = {}
    --通知id索引
    self.m_nNotifyId = 0
end

--
function NotifyMgr:createNotify(main, sub)
    return
    {
        main = main,
        sub = sub,
        name = "",
        fun = nil,
        pause = false,
        group = ""
    }
end

--红点name
local DOT_NODE_NAME = "___ry_notify_dot_name___"
--通知红点
function NotifyMgr:showNotify(node, msg, pos)
    if nil == node then
        return
    end 

    local dot = node:getChildByName(DOT_NODE_NAME)
    if nil == dot then
        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_dot.png")
        if nil ~= frame then
            dot = cc.Sprite:createWithSpriteFrame(frame)
            dot:setName(DOT_NODE_NAME)
            node:addChild(dot)

            local nodesize = node:getContentSize()
            local dotsize = dot:getContentSize()
            pos = pos or cc.p(nodesize.width - dotsize.width * 0.5, nodesize.height - dotsize.height * 0.5 )
            dot:setPosition(pos)            
        end
    end

    if nil ~= dot then
        --前一个不相同通知
        if nil ~= dot.msg and dot.msg._id ~= msg._id then
            self:markRead(dot.msg)
        end
        dot.msg = msg
        dot:setVisible(true)   
    end
end
--通知红点
function NotifyMgr:hideNotify(node,markread)
    if nil == node then
        return
    end
    markread = markread or false
    local dot = node:getChildByName(DOT_NODE_NAME)
    if nil ~= dot then
        dot:setVisible(false)
        if true== markread and nil ~= dot.msg then
            self:markRead(dot.msg)
        end
    end
end

--添加通知
--[[
notify = 
{
    main =          
    sub = 
    name =          --用于区别同main,sub消息
    fun =           --回调
    pause =         --消息是否激活
    autoread =      --是否自动标记已读
    group =         --消息组
}
]]
function NotifyMgr:registerNotify(notify) 
    notify.main = notify.main or 0
    notify.sub = notify.sub or 0
    notify.name = notify.name or ""
    notify.group = notify.group or ""
    notify.fun = notify.fun or nil
    local pause = notify.pause or false
    notify.active = not pause
    notify.autoread = notify.autoread or false

    local mNotify = self:getManagedNotify(main, sub, name)
    if nil == mNotify then
        print("register main ==> " .. notify.main .. " sub ==> " .. notify.sub .. " name ==> " .. notify.name)
        table.insert(self.m_tabNofityMgr, notify)
    else
        print("update main ==> " .. notify.main .. " sub ==> " .. notify.sub .. " name ==> " .. notify.name)
        mNotify = notify
    end
end

--移除通知
function NotifyMgr:unregisterNotify(main, sub, name)
    local idx = nil
    for k,v in pairs(self.m_tabNofityMgr) do
        if v.main == main and v.sub == sub and v.name == name then
            idx = k
            print("unregister main ==> " .. main .. " sub ==> " .. sub .. " name ==> " .. name)
        end
    end
    if nil ~= idx then
        table.remove(self.m_tabNofityMgr, idx)
    end
end

--标记暂停
function NotifyMgr:pauseNotify(main, sub, name)
    local notify = self:getManagedNotify(main, sub, name)
    if nil ~= notify then
        notify.active = false
    end
end

--标记激活
function NotifyMgr:resumeNotify(main, sub, name)
    local notify = self:getManagedNotify(main, sub, name)
    if nil ~= notify then
        notify.active = true
    end
end

--标记通知已读
function NotifyMgr:markRead(msg)
    print("mark read ==> ".. msg._id)
    --异常处理
    msg._id = msg._id or -1
    if -1 == msg._id then
        msg._bread = true
    end
    
    self.m_tabUnreadNotify = {}
    for k,v in pairs(self.m_tabListNotify) do
        if v._id == msg._id then
            self.m_tabListNotify[k] = nil
        else
            table.insert(self.m_tabUnreadNotify, v)
        end
    end
end

--获取未读通知列表
function NotifyMgr:getUnreadNotifyList()
    --dump(self.m_tabUnreadNotify, "self.m_tabUnreadNotify", 6)
    return self.m_tabUnreadNotify
end

--更新未读通知列表
function NotifyMgr:updateUnreadList()
    self.m_tabUnreadNotify = {}
    for k,v in pairs(self.m_tabListNotify) do
        if nil ~= v._bread and false == v._bread then
            table.insert(self.m_tabUnreadNotify, v)
        else
            self.m_tabListNotify[k] = nil
        end
    end
end

--处理通知
function NotifyMgr:excuteSleepNotfiy()
    for k,v in pairs(self.m_tabSleepNotify) do
        local notify = v.notify
        local msg = v.msg
        if nil ~= notify and type(notify.fun) == "function" and nil ~= msg then
            table.insert(self.m_tabListNotify, msg)
            notify.fun(msg)            
        end
    end
    self.m_tabSleepNotify = {}
    self:updateUnreadList()
end

--处理通知
function NotifyMgr:excute(main, sub, param)
    for k,v in pairs(self.m_tabNofityMgr) do
        if v.main == main and v.sub == sub then
            --dump(v, "v")
            local msg = 
            {
                _id     = self:getNotifyId(),
                _bread  = false,
                main    = main,
                sub     = sub,
                name    = v.name,
                param   = param,
                group   = v.group
            }       
            if true == v.active then
                local bHandled = nil
                --活动状态
                if type(v.fun) == "function" then
                    bHandled = v.fun(msg)
                end
                bHandled = bHandled or false

                if false == bHandled then
                    table.insert(self.m_tabListNotify, msg)    
                    self:updateUnreadList()
                end                            
            else
                print("cache notify: main ==> " .. main .. " sub ==> " .. sub)
                table.insert(self.m_tabSleepNotify, {notify = v, msg = msg})
                --控制容量
                if #self.m_tabSleepNotify > MAX_CACHE_NOTIFY then
                    table.remove(self.m_tabSleepNotify, 1)
                end
            end
        end
    end
end

--判断是否在通知列表
function NotifyMgr:getManagedNotify(main, sub, name)
    for k,v in pairs(self.m_tabNofityMgr) do
        if v.main == main and v.sub == sub and v.name == name then
            return v
        end
    end
    return nil
end

--获取通知id(标记管理)
function NotifyMgr:getNotifyId()
    local tmp = self.m_nNotifyId
    self.m_nNotifyId = self.m_nNotifyId + 1
    if self.m_nNotifyId > yl.MAX_INT then
        self.m_nNotifyId = 0
    end
    return tmp
end

return NotifyMgr