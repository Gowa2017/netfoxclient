--
-- Author: zhong
-- Date: 2016-11-10 17:47:30
--

-- 帧动画管理
local AnimationMgr = class("AnimationMgr")

AnimationMgr.PLIST_RES = 1
AnimationMgr.LOCAL_RES = 2

function AnimationMgr.getAnimationParam()
    return
    {
        -- 动画名称
        m_strName = "",
        -- 是否重置动画参数
        m_bResetParam = true,
        -- 每帧持续时间
        m_fDelay = 0,
        -- 是否恢复到第一帧
        m_bRestore = true,
    }
end

-- 加载帧动画
-- @param[plist]    帧动画plist文件
function AnimationMgr.loadAnimationFromFile( plist )
    cc.AnimationCache:getInstance():addAnimations(plist)
end

-- 加载帧动画
-- @param[format]   帧文件名字format格式
-- @param[start]    起始索引
-- @param[count]    帧数量
-- @param[key]      动画key
-- @param[resType]  资源类型( 1:plist合图、2:单图)
function AnimationMgr.loadAnimationFromFrame( format, start, count, key, resType)
    local animation = cc.Animation:create()
    if nil == animation then
        return
    end
    resType = resType or AnimationMgr.PLIST_RES

    local nBegin = start
    local nEnd = start + count

    for i = nBegin, nEnd do
        local buf = string.format(format, i)
        if AnimationMgr.LOCAL_RES == resType then
            if cc.FileUtils:getInstance():isFileExist(buf) then
                animation:addSpriteFrameWithFile(buf)
            end
        else
            local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(buf)
            if nil ~= frame then
                animation:addSpriteFrame(frame)
            end
        end        
    end

    cc.AnimationCache:getInstance():addAnimation(animation, key)
end

-- 移除动画
-- @param[key]      动画key
function AnimationMgr.removeCachedAnimation( key )
    cc.AnimationCache:getInstance():removeAnimation(key)
end

-- 播放动画
-- @param[pNode]    动画播放节点
-- @param[param]    动画参数
-- @param[callBack] 回调
function AnimationMgr.playAnimation( pNode, param, callBack)
    local animation = cc.AnimationCache:getInstance():getAnimation(param.m_strName)
    if nil == animation then
        return
    end

    -- 设置参数
    if param.m_bResetParam then
        animation:setDelayPerUnit(param.m_fDelay)
        animation:setRestoreOriginalFrame(param.m_bRestore)
    end

    local act = cc.Animate:create(animation)
    if nil == act then
        return
    end

    if nil ~= callBack then
        pNode:runAction(cc.Sequence:create(act, callBack))
    else
        pNode:runAction(act)
    end
end

-- 获取动画
-- @param[param]    动画参数
function AnimationMgr.getAnimate( param )
    local animation = cc.AnimationCache:getInstance():getAnimation(param.m_strName)
    if nil == animation then
        return nil
    end

    -- 设置参数
    if param.m_bResetParam then
        animation:setDelayPerUnit(param.m_fDelay)
        animation:setRestoreOriginalFrame(param.m_bRestore)
    end
    return cc.Animate:create(animation)
end

return AnimationMgr