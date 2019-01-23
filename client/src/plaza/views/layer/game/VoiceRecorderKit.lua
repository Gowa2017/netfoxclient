--
-- Author: zhong
-- Date: 2016-11-24 16:36:46
--
-- 语音录音
VoiceRecorderKit = VoiceRecorderKit or {}
-- 录音间隔
VoiceRecorderKit.nInterval = 0
-- 是否在录音
VoiceRecorderKit.bRecordVoice = false

local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")
local RECORD_STATE = true
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MAX_RECORD_TIME = 14
local RECORD_SAVE_PATH = device.writablePath .. "/saverec/"

local VoiceRecorderLayer = class("VoiceRecorderLayer", cc.Layer)
--全局函数(ios/android端调用)
cc.exports.g_NativeRecord = function (msg)
    if msg == "record error" then
        local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            showToastNoFade(runScene, "访问麦克风失败或录音异常,请检查录音权限!", 2)
        end
        RECORD_STATE = false
    end
end

local scheduler = cc.Director:getInstance():getScheduler()
function VoiceRecorderLayer:ctor( parent, gameFrame )
    self.parent = parent
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/VoiceRecordLayer.csb", self )
    self.m_actAni = ExternalFun.loadTimeLine( "plaza/VoiceRecordLayer.csb" )
    self.m_actAni:gotoFrameAndPlay(0,true)
    self:runAction(self.m_actAni)

    -- 剩余时间
    self.m_txtLeftTips = csbNode:getChildByName("txt_tips")
    self.m_txtLeftTips:setString("剩余录音时间:" .. MAX_RECORD_TIME .. "秒")

    -- 移除文件
    cc.FileUtils:getInstance():removeFile(RECORD_SAVE_PATH .. "record.mp3")
    RECORD_STATE = true
    -- 权限检查
    if RECORD_STATE then
        AudioEngine.setMusicVolume(0.01)
        AudioRecorder:getInstance():startRecord("record.mp3")
    end
    self.m_nRecordTime = 0
    self._gameFrame = gameFrame
    self.m_bCancelRecord = false
    VoiceRecorderKit.bRecordVoice = true

    if nil == self.m_scheduler then
        self.m_scheduler = scheduler:scheduleScriptFunc(handler(self, self.onCountDown), 1.0, false)
    end
end

function VoiceRecorderLayer:onCountDown(dt)
    self.m_nRecordTime = self.m_nRecordTime + 1
    local left = MAX_RECORD_TIME - self.m_nRecordTime
    self.m_txtLeftTips:setString("剩余录音时间:" .. left .. "秒")
    if 0 == left then
        self:removeRecorde()
    end
end

function VoiceRecorderLayer:removeRecorde()
    VoiceRecorderKit.bRecordVoice = false
    if nil ~= self.m_scheduler then
        scheduler:unscheduleScriptEntry(self.m_scheduler)
        self.m_scheduler = nil
    end
    if self.m_nRecordTime < 1 then        
        showToast(self.parent, "录音的时间过短，请重试", 1)
        AudioRecorder:getInstance():cancelRecord()
        AudioEngine.setMusicVolume(GlobalUserItem.nMusic/100.0)
        self:removeFromParent()
        return
    end

    if RECORD_STATE then
        if not self.m_bCancelRecord then
            AudioRecorder:getInstance():endRecord()
            AudioEngine.setMusicVolume(GlobalUserItem.nMusic/100.0)
            
            local buffer = AudioRecorder:getInstance():createSendBuffer()
            if nil ~= buffer and nil ~= self._gameFrame then
                print(" 发送录音 ")
                buffer:setcmdinfo(game_cmd.MDM_GF_FRAME,game_cmd.SUB_GF_USER_VOICE)
                self._gameFrame:sendSocketData(buffer)
            end
        end
    else
        AudioRecorder:getInstance():cancelRecord()
        AudioEngine.setMusicVolume(GlobalUserItem.nMusic/100.0)
    end    
    self:removeFromParent()
end

function VoiceRecorderLayer:cancelVoiceRecord()
    VoiceRecorderKit.bRecordVoice = false
    if nil ~= self.m_scheduler then
        scheduler:unscheduleScriptEntry(self.m_scheduler)
        self.m_scheduler = nil
    end
    self.m_txtLeftTips:setString("取消录音")
    AudioRecorder:getInstance():cancelRecord()
    AudioEngine.setMusicVolume(GlobalUserItem.nMusic/100.0)
    self.m_bCancelRecord = true
    self:removeFromParent()
end

function VoiceRecorderKit.init()
    -- 配置路径
    AudioRecorder:getInstance():init(device.writablePath .. "/saverec/", device.writablePath .. "/downrec/")
end

function VoiceRecorderKit.createRecorderLayer( parent, gameFrame )
    -- 录音间隔
    local lasttime = VoiceRecorderKit.nInterval
    local curtime = os.time()
    if curtime - lasttime < 2 then
        local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            showToastNoFade(runScene, "录音间隔太短, 请稍后", 1)
        end        
        return nil
    end

    -- 权限请求
    local bRequest = cc.UserDefault:getInstance():getBoolForKey("recordpermissionreq",false)
    if false == bRequest then
        print("###2")
        -- 尝试请求
        AudioRecorder:getInstance():startRecord("record.mp3")
        AudioRecorder:getInstance():cancelRecord()
        cc.UserDefault:getInstance():setBoolForKey("recordpermissionreq",true)
        VoiceRecorderKit.nInterval = os.time()
        return nil
    end

    print("###3")
    -- 权限检查(ios端有效)
    if false == MultiPlatform:getInstance():checkRecordPermission() then
        local runScene = cc.Director:getInstance():getRunningScene()
        if nil ~= runScene then
            showToastNoFade(runScene, "当前未获得麦克风权限,无法进行语音聊天!", 2)
        end
        VoiceRecorderKit.nInterval = os.time()
        return nil
    end
    print("###4")
    VoiceRecorderKit.nInterval = os.time()
    return VoiceRecorderLayer:create(parent, gameFrame)
end
VoiceRecorderKit.init()

function VoiceRecorderKit.startPlayVoice( spath )
    AudioEngine.setMusicVolume(0.01)
    AudioEngine.setVoiceVolume(1.0)
    return AudioEngine.playVoice(spath)
end

function VoiceRecorderKit.finishPlayVoice()
    if not VoiceRecorderKit.bRecordVoice then
        AudioEngine.setMusicVolume(GlobalUserItem.nMusic/100.0)
        AudioEngine.setVoiceVolume(0)  
    end
end

return VoiceRecorderKit

--[[if nil ~= buffer then
            AudioRecorder:getInstance():saveRecordFile(buffer, function(uid, tid, spath)
                AudioEngine.setMusicVolume(0.01)
                local vid = AudioEngine.playVoice(spath)
                AudioEngine.setVoiceVolume(1.0)
                AudioRecorder:getInstance():setFinishCallBack(vid, function(voiceid, filename)
                    print("play over " .. filename)                 
                    AudioEngine.setMusicVolume(GlobalUserItem.nMusic/100.0) 
                    AudioEngine.setVoiceVolume(0)               
                end)
            end)
        end]]