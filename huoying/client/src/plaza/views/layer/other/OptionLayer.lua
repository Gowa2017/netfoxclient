--[[
	设置界面
	2015_12_03 C.P
	功能：音乐音量震动等
]]

local OptionLayer = class("OptionLayer", function(scene)
		local optionLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return optionLayer
end)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local g_var = ExternalFun.req_var
local WebViewLayer = appdf.CLIENT_SRC .. "plaza.views.layer.plaza.WebViewLayer"
appdf.req(appdf.CLIENT_SRC.."plaza.models.FriendMgr")
local NotifyMgr = appdf.req(appdf.EXTERNAL_SRC .. "NotifyMgr")
local ModifyFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ModifyFrame")

OptionLayer.CBT_SILENCE 	= 1
OptionLayer.CBT_SOUND   	= 2
OptionLayer.BT_EXIT			= 7

OptionLayer.BT_QUESTION		= 8
OptionLayer.BT_COMMIT		= 9
OptionLayer.BT_MODIFY		= 10
OptionLayer.BT_EXCHANGE		= 11
OptionLayer.BT_LOCK         = 12
OptionLayer.BT_UNLOCK       = 13

OptionLayer.PRO_WIDTH		= yl.WIDTH

function OptionLayer:ctor(scene)
	self._scene = scene
	self:setContentSize(yl.WIDTH,yl.HEIGHT)
	local this = self

    local cbtlistener = function (sender,eventType)
    	this:onSelectedEvent(sender:getTag(),sender,eventType)
    end
	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

	local areaWidth = yl.WIDTH
	local areaHeight = yl.HEIGHT

	--显示背景
	display.newSprite("Option/bg_option.png")
		:move(yl.WIDTH/2,yl.HEIGHT/2)
		:addTo(self)

	--上方背景
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,yl.HEIGHT-51)
        self:addChild(sp)
    end
	--标题
	display.newSprite("Option/title_option.png")
		:move(areaWidth/2,yl.HEIGHT-51)
		:addTo(self)
	--返回
	ccui.Button:create("bt_return_0.png","bt_return_1.png")
    	:move(75,yl.HEIGHT-51)
    	:setTag(OptionLayer.BT_EXIT)
    	:addTo(self)
    	:addTouchEventListener(btcallback)

	--音效开关
	display.newSprite("Option/frame_option_0.png")
		:move(1000,510)
		:addTo(self)
	display.newSprite("Option/text_sound.png")
		:move(837,510)
		:addTo(self)
	self._cbtSilence = ccui.CheckBox:create("Option/bt_option_switch_0.png","","Option/bt_option_switch_1.png","","")
		:move(1175,510)
		:setSelected(GlobalUserItem.bSoundAble)
		:addTo(self)
		:setTag(self.CBT_SOUND)
	self._cbtSilence:addEventListener(cbtlistener)

	--音乐开关
	display.newSprite("Option/frame_option_0.png")
		:move(330,510)
		:addTo(self)
	display.newSprite("Option/text_music.png")
		:move(170,510)
		:addTo(self)
	self._cbtSound = ccui.CheckBox:create("Option/bt_option_switch_0.png","","Option/bt_option_switch_1.png","","")
		:move(500,510)
		:setSelected(GlobalUserItem.bVoiceAble)
		:addTo(self)
		:setTag(self.CBT_SILENCE)
	self._cbtSound:addEventListener(cbtlistener)

	--常见问题
	display.newSprite("Option/frame_option_0.png")
		:move(330,338)
		:addTo(self)
	display.newSprite("Option/text_question.png")
		:move(170,338)
		:addTo(self)
	ccui.Button:create("Option/bt_option_check_0.png","Option/bt_option_check_1.png")
		:move(500,338)
		:setTag(OptionLayer.BT_QUESTION)
		:addTo(self)
		:addTouchEventListener(btcallback)

	--游戏反馈
	display.newSprite("Option/frame_option_0.png")
		:move(1000,338)
		:addTo(self)
	display.newSprite("Option/text_feedback.png")
		:move(837,338)
		:addTo(self)
	ccui.Button:create("Option/bt_option_commit_0.png","Option/bt_option_commit_1.png")
		:move(1173,338)
		:setTag(OptionLayer.BT_COMMIT)
		:addTo(self)
		:addTouchEventListener(btcallback)
	
	--帐号信息
	display.newSprite("Option/frame_option_1.png")
		:move(yl.WIDTH/2,166)
		:addTo(self)
	display.newSprite("Option/text_account.png")
		:move(240,166)
		:addTo(self)

	local testen = cc.Label:createWithSystemFont("A","Arial", 24)
    self._enSize = testen:getContentSize().width
    local testcn = cc.Label:createWithSystemFont("游","Arial", 24)
    self._cnSize = testcn:getContentSize().width
	self._nickname = cc.Label:createWithTTF(string.stringEllipsis(GlobalUserItem.szNickName,self._enSize,self._cnSize,200), "fonts/round_body.ttf", 24)
        :move(435,166)
        :setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
        :setAnchorPoint(cc.p(0.5,0.5))
       	:setWidth(210)
       	:setHeight(25)
       	:setLineBreakWithoutSpace(false)
       	:setTextColor(cc.c4b(240,240,240,255))
       	:addTo(self)

    -- 锁定
    if 1 == GlobalUserItem.cbLockMachine then
        self.m_btnLock = ccui.Button:create("Option/btn_unlockmachine_0.png","Option/btn_unlockmachine_1.png","Option/btn_unlockmachine_0.png")
        self.m_btnLock:setTag(OptionLayer.BT_UNLOCK)
    else
        self.m_btnLock = ccui.Button:create("Option/btn_lockmachine_0.png","Option/btn_lockmachine_1.png","Option/btn_lockmachine_0.png")
        self.m_btnLock:setTag(OptionLayer.BT_LOCK)
    end    
    self.m_btnLock:move(631,166)        
        :addTo(self)
        :addTouchEventListener(btcallback)

    --修改密码
    ccui.Button:create("Option/bt_option_modify_0.png","Option/bt_option_modify_1.png")
		:move(861,166)
		:setTag(OptionLayer.BT_MODIFY)
		:addTo(self)
		:addTouchEventListener(btcallback)
	--切换帐号
    ccui.Button:create("Option/bt_option_change_0.png","Option/bt_option_change_1.png")
		:move(1091,166)
		:setTag(OptionLayer.BT_EXCHANGE)
		:addTo(self)
		:addTouchEventListener(btcallback)

    local mgr = self._scene:getApp():getVersionMgr()
    -- 版本号
    cc.Label:createWithTTF("版本号:" .. appdf.BASE_C_VERSION .. "." .. mgr:getResVersion(), "fonts/round_body.ttf", 24)
        :move(yl.WIDTH,0)
        :setAnchorPoint(cc.p(1,0))
        :addTo(self)
end


function OptionLayer:onSelectedEvent(tag,sender,eventType)
	if tag == OptionLayer.CBT_SILENCE then
		GlobalUserItem.setVoiceAble(eventType == 0)
		--背景音乐
        ExternalFun.playPlazzBackgroudAudio()
	elseif tag == OptionLayer.CBT_SOUND then
		GlobalUserItem.setSoundAble(eventType == 0)
	end
end

--按键监听
function OptionLayer:onButtonClickedEvent(tag,sender)
	if tag ~= OptionLayer.BT_EXCHANGE and tag ~= OptionLayer.BT_EXIT then
		if GlobalUserItem.isAngentAccount() then
			return
		end
	end	
	
	if tag == OptionLayer.BT_EXCHANGE then
        self._scene:ExitClient()
	elseif tag == OptionLayer.BT_EXIT then
		self._scene:onKeyBack()
	elseif tag == OptionLayer.BT_QUESTION then
		self._scene:onChangeShowMode(yl.SCENE_FAQ)
	elseif tag == OptionLayer.BT_MODIFY then
        if self._scene._gameFrame:isSocketServer() then
            showToast(self,"当前页面无法使用此功能！",1)
            return
        end
		self._scene:onChangeShowMode(yl.SCENE_MODIFY)
	elseif tag == OptionLayer.BT_COMMIT then
		self._scene:onChangeShowMode(yl.SCENE_FEEDBACK)
    elseif tag == OptionLayer.BT_LOCK then
        print("锁定机器")
        self:showLockMachineLayer(self)
    elseif tag == OptionLayer.BT_UNLOCK then
        print("解锁机器")
        self:showLockMachineLayer(self)
	end
end

local TAG_MASK = 101
local BTN_CLOSE = 102
function OptionLayer:showLockMachineLayer( parent )
    if nil == parent then
        return
    end
    --网络回调
    local modifyCallBack = function(result,message)
        self:onModifyCallBack(result,message)
    end
    --网络处理
    self._modifyFrame = ModifyFrame:create(self,modifyCallBack)

    -- 加载csb资源
    local csbNode = ExternalFun.loadCSB("Option/LockMachineLayer.csb", parent )

    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local tag = ref:getTag()
            if TAG_MASK == tag or BTN_CLOSE == tag then
                csbNode:removeFromParent()
            elseif OptionLayer.BT_LOCK == tag then
                local txt = csbNode.m_editbox:getText()
                if txt == "" then
                    showToast(self, "密码不能为空!", 2)
                    return 
                end
                self._modifyFrame:onBindingMachine(1, txt)
                csbNode:removeFromParent()
            elseif OptionLayer.BT_UNLOCK == tag then
                local txt = csbNode.m_editbox:getText()
                if txt == "" then
                    showToast(self, "密码不能为空!", 2)
                    return 
                end
                self._modifyFrame:onBindingMachine(0, txt)
                csbNode:removeFromParent()
            end
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    local image_bg = csbNode:getChildByName("image_bg")
    image_bg:setSwallowTouches(true)

    -- 输入
    local tmp = image_bg:getChildByName("sp_lockmachine_bankpw")
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10),"blank.png",UI_TEX_TYPE_PLIST)
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(30)
        :setPlaceholderFontSize(30)
        :setMaxLength(32)
        :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("请输入密码")
    image_bg:addChild(editbox)
    csbNode.m_editbox = editbox

    -- 锁定/解锁
    local btn = image_bg:getChildByName("btn_lock")
    btn:setTag(OptionLayer.BT_LOCK)
    btn:addTouchEventListener( touchFunC )
    local normal = "Option/btn_lockmachine_0.png"
    local disable = "Option/btn_lockmachine_0.png"
    local press = "Option/btn_lockmachine_1.png"
    if 1 == GlobalUserItem.cbLockMachine then
        btn:setTag(OptionLayer.BT_UNLOCK)
        normal = "Option/btn_unlockmachine_0.png"
        disable = "Option/btn_unlockmachine_0.png"
        press = "Option/btn_unlockmachine_1.png"
    end
    btn:loadTextureDisabled(disable)
    btn:loadTextureNormal(normal)
    btn:loadTexturePressed(press)  

    btn = image_bg:getChildByName("btn_cancel")
    btn:setTag(BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )

    -- 关闭
    btn = image_bg:getChildByName("btn_close")
    btn:setTag(BTN_CLOSE)
    btn:addTouchEventListener( touchFunC )
end

function OptionLayer:onModifyCallBack(result, tips)
    if type(tips) == "string" and "" ~= tips then
        showToast(self, tips, 2)
    end 

    local normal = "Option/btn_lockmachine_0.png"
    local disable = "Option/btn_lockmachine_0.png"
    local press = "Option/btn_lockmachine_1.png"
    if self._modifyFrame.BIND_MACHINE == result then
        if 0 == GlobalUserItem.cbLockMachine then
            GlobalUserItem.cbLockMachine = 1
            self.m_btnLock:setTag(OptionLayer.BT_UNLOCK)
            normal = "Option/btn_unlockmachine_0.png"
            disable = "Option/btn_unlockmachine_0.png"
            press = "Option/btn_unlockmachine_1.png"
        else
            GlobalUserItem.cbLockMachine = 0
            self.m_btnLock:setTag(OptionLayer.BT_LOCK)
        end
    end   
    self.m_btnLock:loadTextureDisabled(disable)
    self.m_btnLock:loadTextureNormal(normal)
    self.m_btnLock:loadTexturePressed(press)  
end

return OptionLayer