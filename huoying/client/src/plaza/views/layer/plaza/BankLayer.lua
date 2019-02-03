
local BankLayer = class("BankLayer", function(scene)
		local bankLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return bankLayer
end)

local BankFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BankFrame")
local PopWait = appdf.req(appdf.BASE_SRC.."app.views.layer.other.PopWait")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

BankLayer.BT_TAKE 			= 2
BankLayer.BT_SAVE 			= 3
BankLayer.BT_TRANSFER		= 4
BankLayer.CBT_SAVEORTAKE 	= 5
BankLayer.CBT_TRANSFER 		= 6
BankLayer.EDIT_TRANSFER_DST = 9
BankLayer.CBT_BY_ID			= 10
BankLayer.CBT_BY_NAME		= 11
BankLayer.BT_EXIT			= 12
BankLayer.BT_FORGET			= 15
BankLayer.BT_CHECK			= 16
BankLayer.EDIT_SAVEORTAKE	= 17
BankLayer.BT_CLOSE			= 18
BankLayer.BT_ENABLE			= 19
BankLayer.BT_BACK  			= 20

--开通银行
BankLayer.BT_ENABLE_RETURN	= 30
BankLayer.BT_ENABLE_BACK	= 31
BankLayer.BT_ENABLE_CONFIRM = 32

function BankLayer:ctor(scene, gameFrame)
	ExternalFun.registerNodeEvent(self)
		
	self._scene = scene

	self:setContentSize(yl.WIDTH,yl.HEIGHT)

	local this = self
    --银行配置信息
    self.m_tabBankConfigInfo = {}

	local editHanlder = function(event,editbox)
		this:onEditEvent(event,editbox)
	end

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    local cbtlistener = function (sender,eventType)
    	this:onSelectedEvent(sender,eventType)
    end

    --网络回调
    local  bankCallBack = function(result,message)
		this:onBankCallBack(result,message)
	end

	--网络处理
	self._bankFrame = BankFrame:create(self,bankCallBack)
    self._bankFrame._gameFrame = gameFrame
    if nil ~= gameFrame then
        gameFrame._shotFrame = self._bankFrame
    end

	local areaWidth = yl.WIDTH
	local areaHeight = yl.HEIGHT

	--显示标题
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(areaWidth/2,700)
        self:addChild(sp)
    end
	display.newSprite("Bank/title_bank.png")
		:move(areaWidth/2,700)
		:addTo(self)

	--返回按钮
	ccui.Button:create("bt_return_0.png","bt_return_1.png")
    	:move(75,yl.HEIGHT-51)
    	:setTag(BankLayer.BT_EXIT)
    	:addTo(self)
    	:addTouchEventListener(btcallback)

    --背景框
    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(areaWidth/2,325)
        self:addChild(sp)
    end

	self._cbtSaveTake = ccui.CheckBox:create("Bank/tag_bank_take_0.png","Bank/tag_bank_take_0.png","Bank/tag_bank_take_1.png","","")
		:move(yl.WIDTH/2,585)
		:setAnchorPoint(cc.p(1.0,0.5))
		:setSelected(true)
		:addTo(self)
		:setTag(self.CBT_SAVEORTAKE)
	self._cbtSaveTake:addEventListener(cbtlistener)
    self._cbtSaveTake:setVisible(false)
    self._cbtSaveTake:setEnabled(false)

	--银行赠送
	self._cbtTransfer = ccui.CheckBox:create("Bank/tag_bank_present_0.png","Bank/tag_bank_present_0.png","Bank/tag_bank_present_1.png","","")
		:setAnchorPoint(cc.p(0,0.5))
		:move(yl.WIDTH/2,585)
		:setSelected(false)
		:addTo(self)
		:setTag(self.CBT_TRANSFER)
	self._cbtTransfer:addEventListener(cbtlistener)
	self._cbtTransfer:setVisible(false)
	self._cbtTransfer:setEnabled(false)

	--转换区域
	self._takesaveArea = display.newLayer()
		:setContentSize(1250,520)
		:move(42,26)
		:addTo(self)

	display.newSprite("Bank/bank_frame_0.png")
		:move(1250/2,430)
		:addTo(self._takesaveArea)
	--携带金币
	display.newSprite("Bank/icon_bank_take.png")
		:move(300,425)
		:addTo(self._takesaveArea)
	display.newSprite("Bank/text_bank_gold.png")
		:move(455,443)
		:addTo(self._takesaveArea)
	self._txtScore = cc.LabelAtlas:_create(string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/"), "Bank/bank_num_1.png", 19, 24, string.byte("/")) 
    		:move(380,396)
    		:setAnchorPoint(cc.p(0,0.5))
    		:addTo(self._takesaveArea)

    --银行金币
	display.newSprite("Bank/icon_bank_save.png")
		:move(785,425)
		:addTo(self._takesaveArea)
	display.newSprite("Bank/text_bank_save.png")
		:move(915,443)
		:addTo(self._takesaveArea)

	self._txtInsure = cc.LabelAtlas:_create(string.formatNumberThousands(GlobalUserItem.lUserInsure,true,"/"), "Bank/bank_num_1.png", 19, 24, string.byte("/")) 
    		:move(853,396)
    		:setAnchorPoint(cc.p(0,0.5))
    		:addTo(self._takesaveArea)

    --其他功能
    self._txtSaveTakeChs = cc.LabelAtlas:_create("", "Bank/bank_num_0.png", 30, 32, string.byte("0")) 
    		:move(898,305)
    		:setAnchorPoint(cc.p(0,0.5))
    		:addTo(self._takesaveArea)
    --[[ccui.Button:create("Bank/bt_bank_forget.png","")
    	:move(958,210)
    	:setTag(BankLayer.BT_FORGET)
    	:addTo(self._takesaveArea)
    	:addTouchEventListener(btcallback)]]
    ccui.Button:create("Bank/bt_bank_check.png","")
    	:move(990,117)
    	:setTag(BankLayer.BT_CHECK)
    	:addTo(self._takesaveArea)
    	:addTouchEventListener(btcallback)
    self._notifyText = cc.Label:createWithTTF("提示：存入游戏币免手续费，取出将扣除 的手续费。存款无需输入银行密码。", "fonts/round_body.ttf", 24)
								:addTo(self._takesaveArea)
								:setTextColor(cc.c4b(136,164,224,255))
								:move(1250/2,38)

    --存款按钮
	ccui.Button:create("Bank/bt_bank_save_0.png", "Bank/bt_bank_save_1.png")
		:move(490,117)
		:setTag(BankLayer.BT_SAVE)
		:addTo(self._takesaveArea)
        :addTouchEventListener(btcallback)
    --取款按钮
   	ccui.Button:create("Bank/bt_bank_take_0.png", "Bank/bt_bank_take_1.png")
		:move(763,117)
		:setTag(BankLayer.BT_TAKE)
		:addTo(self._takesaveArea)
        :addTouchEventListener(btcallback)

    display.newSprite("Bank/text_bank_take.png")
    	:move(305,305)
    	:addTo(self._takesaveArea)
    display.newSprite("Bank/text_bank_password.png")
    	:move(305,210)
    	:addTo(self._takesaveArea)

    --金额大写提示
    self.m_textNumber = ClipText:createClipText(cc.size(350,24), "", "fonts/round_body.ttf", 24)
    self:addChild(self.m_textNumber)
    self.m_textNumber:setPosition(930,335)
    self.m_textNumber:setAnchorPoint(cc.p(0,0.5))

	--金额输入
	self.edit_Score = ccui.EditBox:create(cc.size(492,69), ccui.Scale9Sprite:create("Bank/bank_frame_1.png"))
		:move(635,305)
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(13)
		:setFontColor(cc.c4b(255,255,255,255))
		:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
		:setTag(BankLayer.EDIT_SAVEORTAKE)
		:setPlaceHolder("请输入操作金额")
		:addTo(self._takesaveArea)
	self.edit_Score:registerScriptEditBoxHandler(editHanlder)

	--密码输入	
	self.edit_Password = ccui.EditBox:create(cc.size(492,69), ccui.Scale9Sprite:create("Bank/bank_frame_1.png"))
		:move(635,210)
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setFontColor(cc.c4b(195,199,239,255))
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("存款不需要密码")
		:addTo(self._takesaveArea)

	--赠送界面
	self._transferArea = display.newLayer()
		:setContentSize(1250,520)
		:addTo(self)
		:move(43,25)
		:setVisible(false)

    display.newSprite("Bank/text_bank_preAccount.png")
    	:move(300,425)
    	:addTo(self._transferArea)
    display.newSprite("Bank/text_bank_preGold.png")
    	:move(300,335)
    	:addTo(self._transferArea)
    display.newSprite("Bank/text_bank_password.png")
    	:move(300,240)
    	:addTo(self._transferArea)

   	ccui.Button:create("Bank/bt_bank_present_0.png", "Bank/bt_bank_present_1.png")
		:move(625,117)
		:setTag(BankLayer.BT_TRANSFER)
		:addTo(self._transferArea)
        :addTouchEventListener(btcallback)
	
	self.edit_TransferDst = ccui.EditBox:create(cc.size(492,69), ccui.Scale9Sprite:create("Bank/bank_frame_1.png"))
		:move(625,425)
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setFontColor(cc.c4b(195,199,239,255))
		:setTag(BankLayer.EDIT_TRANSFER_DST)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入赠送的ID")
		:addTo(self._transferArea)

	self.edit_TransferScore = ccui.EditBox:create(cc.size(492,69), ccui.Scale9Sprite:create("Bank/bank_frame_1.png"))
		:move(625,335)
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(13)
		:setFontColor(cc.c4b(255,255,255,255))
		:setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
		:setPlaceHolder("请输入操作金额")
		:addTo(self._transferArea)
	self.edit_TransferScore:registerScriptEditBoxHandler(editHanlder)

	--密码输入	
	self.edit_TransferPassword = ccui.EditBox:create(cc.size(492,69), ccui.Scale9Sprite:create("Bank/bank_frame_1.png"))
		:move(625,240)
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setFontColor(cc.c4b(195,199,239,255))
		:setPlaceHolder("请输入银行密码")
		:addTo(self._transferArea)

	--其他功能
    self._txtPresentChs = cc.LabelAtlas:_create("", "Bank/bank_num_0.png", 30, 32, string.byte("0")) 
    		:move(898,305)
    		:setAnchorPoint(cc.p(0,0.5))
    		:addTo(self._transferArea)
    --[[ccui.Button:create("Bank/bt_bank_forget.png","")
    	:move(958,240)
    	:setTag(BankLayer.BT_FORGET)
    	:addTo(self._transferArea)
    	:addTouchEventListener(btcallback)]]
    ccui.Button:create("Bank/bt_bank_check.png","")
    	:move(990,117)
    	:setTag(BankLayer.BT_CHECK)
    	:addTo(self._transferArea)
    	:addTouchEventListener(btcallback)
    self._notifyTextPresent = cc.Label:createWithTTF("提示：存入游戏币免手续费，取出将扣除 的手续费。存款无需输入银行密码。", "fonts/round_body.ttf", 24)
								:addTo(self._transferArea)
								:setTextColor(cc.c4b(136,164,224,255))
								:move(1250/2,38)

	--提示区域
	self._notifyLayer = ccui.Layout:create()
		:setContentSize(yl.WIDTH,yl.HEIGHT)
		:move(0,0)
		:addTo(self)
		:setVisible(false)
    self._notifyLayer:setTouchEnabled(true)
    self._notifyLayer:setSwallowTouches(true)

	display.newSprite("General/frame_0.png")
    	:move(yl.WIDTH/2,yl.HEIGHT/2)
    	:addTo(self._notifyLayer)

    ccui.Button:create("General/bt_close_0.png","General/bt_close_1.png")
    	:move(1070,560)
    	:setTag(BankLayer.BT_CLOSE)
    	:addTo(self._notifyLayer)
    	:addTouchEventListener(btcallback)

    display.newSprite("General/title_general.png")
    	:move(yl.WIDTH/2,530)
    	:addTo(self._notifyLayer)

    cc.Label:createWithTTF("初次使用，请先开通银行！", "fonts/round_body.ttf", 24)
			:addTo(self._notifyLayer)
			:setTextColor(cc.c4b(255,255,255,255))
			:move(yl.WIDTH/2,430)

	ccui.Button:create("General/bt_cancel_0.png","General/bt_cancel_1.png")
    	:move(529,239)
    	:setTag(BankLayer.BT_CLOSE)
    	:addTo(self._notifyLayer)
    	:addTouchEventListener(btcallback)

    ccui.Button:create("General/bt_confirm_0.png","General/bt_confirm_1.png")
    	:move(809,239)
    	:setTag(BankLayer.BT_ENABLE)
    	:addTo(self._notifyLayer)
    	:addTouchEventListener(btcallback)

    if 0 == GlobalUserItem.cbInsureEnabled then
        self:initEnableBankLayer()
    end
end

function BankLayer:initEnableBankLayer()
    local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    local areaWidth = yl.WIDTH

    --开通区域
    self._enableLayer = ccui.Layout:create()
        :setContentSize(yl.WIDTH,yl.HEIGHT)
        :move(0,0)
        :addTo(self)
    self._enableLayer:setTouchEnabled(true)
    self._enableLayer:setSwallowTouches(true)
    
    --显示标题
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(areaWidth/2,700)
        self._enableLayer:addChild(sp)
    end
    display.newSprite("Bank/title_bank_enable.png")
        :move(areaWidth/2,700)
        :addTo(self._enableLayer)
    --返回按钮
    ccui.Button:create("bt_return_0.png","bt_return_1.png")
        :move(75,yl.HEIGHT-51)
        :setTag(BankLayer.BT_ENABLE_RETURN)
        :addTo(self._enableLayer)
        :addTouchEventListener(btcallback)
    --背景框
    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(areaWidth/2,325)
        self._enableLayer:addChild(sp)
    end
    --银行密码提示
    display.newSprite("Bank/text_setpass_enable.png")
        :move(340,425)
        :addTo(self._enableLayer)
    display.newSprite("Bank/text_confirm_enable.png")
        :move(340,305)
        :addTo(self._enableLayer)

    --密码输入  
    self.edit_EnablePassword = ccui.EditBox:create(cc.size(492,69), "Bank/bank_frame_1.png")
        :move(710,425)
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setFontColor(cc.c4b(195,199,239,255))
        :setPlaceHolder("请输入您的银行密码")
        :addTo(self._enableLayer)
    --密码确认  
    self.edit_EnablePassConfirm = ccui.EditBox:create(cc.size(492,69), "Bank/bank_frame_1.png")
        :move(710,305)
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setFontColor(cc.c4b(195,199,239,255))
        :setPlaceHolder("请输入您的银行密码")
        :addTo(self._enableLayer)

    ccui.Button:create("Bank/bt_bank_enable_0.png","Bank/bt_bank_enable_1.png")
        :move(yl.WIDTH/2,150)
        :setTag(BankLayer.BT_ENABLE_CONFIRM)
        :addTo(self._enableLayer)
        :addTouchEventListener(btcallback)
end

function BankLayer:onFlushBank()
	self:showPopWait()
	self._bankFrame:onFlushBank()
end

--按键监听
function BankLayer:onButtonClickedEvent(tag,sender)
	if tag == BankLayer.BT_TAKE then
		self:onTakeScore()
	elseif tag == BankLayer.BT_SAVE then
		self:onSaveScore()
	elseif tag == BankLayer.BT_TRANSFER then
		self:onTransferScore()
	elseif tag == BankLayer.BT_EXIT then
		self._scene:onKeyBack()
	elseif tag == BankLayer.BT_CLOSE then
		self._notifyLayer:setVisible(false)
	elseif tag == BankLayer.BT_ENABLE then
		self._notifyLayer:setVisible(false)
        if nil ~= self._enableLayer then
            self._enableLayer:runAction(cc.MoveTo:create(0.3,cc.p(0,0)))
        end		
	elseif tag == BankLayer.BT_ENABLE_RETURN then
        self._scene:onKeyBack()
        --[[if nil ~= self._enableLayer then
            self._enableLayer:runAction(cc.MoveTo:create(0.3,cc.p(yl.WIDTH,0)))
        end]]
	elseif tag == BankLayer.BT_ENABLE_CONFIRM then
		self:onEnableBank()
	elseif tag == BankLayer.BT_CHECK then
		self:getParent():getParent():onChangeShowMode(yl.SCENE_BANKRECORD)
	end
end

--输入框监听
function BankLayer:onEditEvent(event,editbox)
	if event == "changed" then
		local src = editbox:getText()

		local dst =  src --string.gsub(src,"([^0-9])","")		
		--editbox:setText(dst)

		local ndst = tonumber(dst)
		if type(ndst) == "number" and ndst < 9999999999999 then
			self.m_textNumber:setString(ExternalFun.numberTransiform(dst))
		else
			self.m_textNumber:setString("")
		end
	elseif event == "return" then
		local src = editbox:getText()
		local numstr = self.m_textNumber:getString()
		if src ~= numstr then
			local dst =  string.gsub(src,"([^0-9])","")
			local ndst = tonumber(dst)
			if type(ndst) == "number" and ndst < 9999999999999 then
				self.m_textNumber:setString(ExternalFun.numberTransiform(dst))
			else
				self.m_textNumber:setString("")
			end
            editbox:setText(dst)
		end	
	end
end


function BankLayer:onSelectedEvent(sender,eventType)
	local tag = sender:getTag()

	if tag == BankLayer.CBT_SAVEORTAKE or tag == BankLayer.CBT_TRANSFER then
		local transfermode = (tag == BankLayer.CBT_TRANSFER)
		self._cbtTransfer:setSelected(transfermode)
		self._cbtSaveTake:setSelected(not transfermode)
		self._transferArea:setVisible(transfermode)
		self._takesaveArea:setVisible(not transfermode)

		self.edit_Score:setText("")
		self.edit_TransferScore:setText("")
		self.m_textNumber:setString("")

        --手续费
        local str = string.format("提示:存入游戏币免手续费,取出将扣除%d‰的手续费。存款无需输入银行密码。", self.m_tabBankConfigInfo.wRevenueTake)
		--调整位置
		if transfermode then 
			self.m_textNumber:setPosition(930, 365)
            str = string.format("提示:普通玩家游戏币赠送需扣除%d‰的手续费。", self.m_tabBankConfigInfo.wRevenueTransfer)
            if 0 ~= GlobalUserItem.cbMemberOrder then
                local vipConfig = GlobalUserItem.MemberList[GlobalUserItem.cbMemberOrder]
                str = str .. vipConfig._name .. "扣除" .. vipConfig._insure .. "‰手续费。"
            end
            self._notifyTextPresent:setString(str)
		else
			self.m_textNumber:setPosition(930, 330)
            self._notifyText:setString(str)
		end        
	elseif tag == BankLayer.CBT_BY_ID or tag == BankLayer.CBT_BY_NAME then
		local byID = (tag == BankLayer.CBT_BY_ID)
		self.edit_TransferDst:setText("")
		self.cbt_TransferByID:setSelected(byID)
		self.cbt_TransferByName:setSelected(not byID)

		local szRes = (byID and "bnak_word_targetid.png" or "bnak_word_targetname.png")
		self._labelTarget:setTexture(szRes)
	end
end

--开通银行
function BankLayer:onEnableBank()
	
	--参数判断
	local szPass = self.edit_EnablePassword:getText()
	local szPassConfirm = self.edit_EnablePassConfirm:getText()

	if #szPass < 1 then 
		showToast(self,"请输入银行密码！",2)
		return
	end
	if #szPass < 6 then
		showToast(self,"密码必须大于6个字符，请重新输入！",2)
		return
	end

	if #szPassConfirm < 1 then 
		showToast(self,"请在确认栏输入银行密码！",2)
		return
	end
	if #szPassConfirm < 6 then
		showToast(self,"确认栏密码必须大于6个字符，请重新输入！",2)
		return
	end

	if szPass ~= szPassConfirm then
		showToast(self,"设置栏和确认栏的密码不相同，请重新输入！",2)
        return
	end

    -- 与帐号不同
    if string.lower(szPass) == string.lower(GlobalUserItem.szAccount) then
        showToast(self,"密码不能与帐号相同，请重新输入！",2)
        return
    end

    -- 银行不同登陆
    if string.lower(szPass) == string.lower(GlobalUserItem.szPassword) then
        showToast(self, "银行密码不能与登录密码一致!", 2)
        return
    end
    
    --[[-- 首位为字母
    if 1 ~= string.find(szPass, "%a") then
        showToast(self,"密码首位必须为字母，请重新输入！",2)
        return
    end]]

	self:showPopWait()
	self._bankFrame:onEnableBank(szPass)
end

--取款操作
function BankLayer:onTakeScore()

	if GlobalUserItem.cbInsureEnabled==0 then
		self._notifyLayer:setVisible(true)
		return
	end

	--参数判断
	local szScore =  string.gsub(self.edit_Score:getText(),"([^0-9])","")
    szScore = string.gsub(szScore, "[.]", "")
	local szPass = self.edit_Password:getText()
    if #szScore < 1 then 
        showToast(self,"请输入操作金额！",2)
        return
    end

	local lOperateScore = tonumber(szScore)
	if lOperateScore < 1 then
		showToast(self,"请输入正确金额！",2)
		return
	end

    if lOperateScore > GlobalUserItem.lUserInsure then
        showToast(self,"您银行游戏币的数目余额不足,请重新输入游戏币数量！",2)
        return
    end

	if #szPass < 1 then 
		showToast(self,"请输入银行密码！",2)
		return
	end
	if #szPass <6 then
		showToast(self,"密码必须大于6个字符，请重新输入！",2)
		return
	end

	self:showPopWait()
	self._bankFrame:onTakeScore(lOperateScore,szPass)
end

--存款
function BankLayer:onSaveScore()
	if GlobalUserItem.cbInsureEnabled==0 then
		self._notifyLayer:setVisible(true)
		return
	end

	--参数判断
	local szScore =  string.gsub(self.edit_Score:getText(),"([^0-9])","")	
    szScore = string.gsub(szScore, "[.]", "")
	if #szScore < 1 then 
		showToast(self,"请输入操作金额！",2)
		return
	end
	
	local lOperateScore = tonumber(szScore)
	
	if lOperateScore<1 then
		showToast(self,"请输入正确金额！",2)
		return
	end

    if lOperateScore > GlobalUserItem.lUserScore then
        showToast(self,"您所携带游戏币的数目余额不足,请重新输入游戏币数量!",2)
        return
    end

	self:showPopWait()

	self._bankFrame:onSaveScore(lOperateScore)
end

function BankLayer:onTransferScore()
	if GlobalUserItem.cbInsureEnabled==0 then
		self._notifyLayer:setVisible(true)
		return
	end

	--参数判断
	local szScore =  string.gsub(self.edit_TransferScore:getText(),"([^0-9])","")
	local szPass = self.edit_TransferPassword:getText()
	local szTarget = self.edit_TransferDst:getText()
	local byID = 1--self.cbt_TransferByID:isSelected() and 1 or 0;

	if #szScore < 1 then 
		showToast(self,"请输入操作金额！",2)
		return
	end

	local lOperateScore = tonumber(szScore)
	if lOperateScore<1 then
		showToast(self,"请输入正确金额！",2)
		return
	end

	if #szPass < 1 then 
		showToast(self,"请输入钱包密码！",2)
		return
	end
	if #szPass <6 then
		showToast(self,"密码必须大于6个字符，请重新输入！",2)
		return
	end

	if #szTarget < 1 then 
		showToast(self,"请输入赠送用户ID！",2)
		return
	end

	self:showPopWait()
	self._bankFrame:onTransferScore(lOperateScore,szTarget,szPass,byID)
end

--操作结果
function BankLayer:onBankCallBack(result,message)

	self:dismissPopWait()
	if  message ~= nil and message ~= "" then
		showToast(self._scene,message,2)
	end

	if result == 2 then
		if GlobalUserItem.cbInsureEnabled~=0 then
			showToast(self,"银行开通成功！",2)
            if nil ~= self._enableLayer then
                self._enableLayer:runAction(cc.MoveTo:create(0.3,cc.p(yl.WIDTH,0)))
            end	
            self:showPopWait()
            self._bankFrame:sendGetBankInfo()
		end
	end

    if result == BankFrame.OP_ENABLE_BANK_GAME then
        showToast(self,"银行开通成功！",2)
        if nil ~= self._enableLayer then
            self._enableLayer:runAction(cc.MoveTo:create(0.3,cc.p(yl.WIDTH,0)))
        end 
        self:showPopWait()
        self._bankFrame:onGetBankInfo()
    end

	if result == 1 then
		self._txtScore:setString(string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/"))
		self._txtInsure:setString(string.formatNumberThousands(GlobalUserItem.lUserInsure,true,"/"))
		self.edit_TransferDst:setText("")
		self.edit_TransferScore:setText("")
		self.edit_TransferPassword:setText("")
		self.edit_Score:setText("")
		self.edit_Password:setText("")
		self.m_textNumber:setString("")
		--更新大厅
		self:getParent():getParent()._gold:setString(string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/"))

        if self._bankFrame._oprateCode == BankFrame.OP_SEND_SCORE then
            -- 转账凭证
            self:showCerLayer(self._bankFrame._tabTarget)
        end
	end

	if result == self._bankFrame.OP_GET_BANKINFO then
		local enableTransfer = (0 == message.cbEnjoinTransfer)
		self._cbtTransfer:setEnabled(enableTransfer)
		self._cbtTransfer:setVisible(enableTransfer)
        self._cbtSaveTake:setEnabled(enableTransfer)
        self._cbtSaveTake:setVisible(enableTransfer)

        self.m_tabBankConfigInfo = message
		--取款收费比例
		local str = string.format("提示:存入游戏币免手续费,取出将扣除%d‰的手续费。存款无需输入银行密码。", message.wRevenueTake)
        self._notifyText:setString(str)

        self._txtInsure:setString(string.formatNumberThousands(GlobalUserItem.lUserInsure,true,"/"))
        self._txtScore:setString(string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/"))
	end
end

--显示等待
function BankLayer:showPopWait()
	self._scene:showPopWait()
end

--关闭等待
function BankLayer:dismissPopWait()
	self._scene:dismissPopWait()
end

function BankLayer:onEnterTransitionFinish( )
    if 1 == GlobalUserItem.cbInsureEnabled then
        self:showPopWait()
        self._bankFrame:onGetBankInfo()
    end	
end

function BankLayer:onExit()
    if self._bankFrame:isSocketServer() then
        self._bankFrame:onCloseSocket()
    end
    if nil ~= self._bankFrame._gameFrame then
        self._bankFrame._gameFrame._shotFrame = nil
        self._bankFrame._gameFrame = nil
    end
end

local TAG_MASK = 101
local BTN_SHARE = 102
local BTN_SAVEPIC = 103
-- 显示凭证
function BankLayer:showCerLayer( tabData )
    if type(tabData) ~= "table" then
        return
    end
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("Bank/BankCerLayer.csb", self)
    local stamp = tabData.opTime or os.time()

    local hide = function()
        local scale1 = cc.ScaleTo:create(0.2, 0.0001)
        local call1 = cc.CallFunc:create(function()
            rootLayer:removeFromParent()
        end)
        csbNode.m_imageBg:runAction(cc.Sequence:create(scale1, call1))
    end
    local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
    -- 截图分享
    local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
    local area = cc.rect(0, 0, framesize.width, framesize.height)

    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            local tag = ref:getTag()
            if TAG_MASK == tag then
                hide()
            elseif BTN_SHARE == tag then
                ExternalFun.popupTouchFilter(0, false)
                captureScreenWithArea(area, "ce_code.png", function(ok, savepath)
                    ExternalFun.dismissTouchFilter()
                    if ok then
                        MultiPlatform:getInstance():customShare(function(isok)
                                    end, "转账凭证", "分享我的转账凭证", url, savepath, "true")
                    end
                end)
            elseif BTN_SAVEPIC == tag then
                ExternalFun.popupTouchFilter(0, false)
                captureScreenWithArea(area, "ce_code.png", function(ok, savepath)         
                    if ok then  
                        if true == MultiPlatform:getInstance():saveImgToSystemGallery(savepath, stamp .. "ce_code.png") then
                            showToast(self, "您的转账凭证图片已保存至系统相册", 1)
                        end
                    end
                    self:runAction(cc.Sequence:create(cc.DelayTime:create(3), cc.CallFunc:create(function()
                        ExternalFun.dismissTouchFilter()
                    end)))
                end)
            end           
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 底板
    local image_bg = csbNode:getChildByName("image_bg")
    image_bg:setTouchEnabled(true)
    image_bg:setSwallowTouches(true)
    image_bg:setScale(0.00001)
    csbNode.m_imageBg = image_bg

    -- 赠送人昵称
    local sendnick = ClipText:createClipText(cc.size(210, 30), GlobalUserItem.szAccount, nil, 30)
    sendnick:setTextColor(cc.c3b(79, 212, 253))
    sendnick:setAnchorPoint(cc.p(0, 0.5))
    sendnick:setPosition(cc.p(260, 450))
    image_bg:addChild(sendnick)

    -- 赠送人ID
    local sendid = image_bg:getChildByName("txt_senduid")
    sendid:setString(GlobalUserItem.dwGameID .. "")

    -- 接收人昵称
    local recnick = ClipText:createClipText(cc.size(210, 30), tabData.opTargetAcconts or "", nil, 30)
    recnick:setTextColor(cc.c3b(79, 212, 253))
    recnick:setAnchorPoint(cc.p(0, 0.5))
    recnick:setPosition(cc.p(810, 450))
    image_bg:addChild(recnick)

    -- 接收人ID
    local recid = image_bg:getChildByName("txt_recuid")
    local reuid = tabData.opTargetID or 0
    recid:setString(reuid .. "")

    -- 赠送游戏币
    local sendcount = image_bg:getChildByName("atlas_sendnum")
    local count = tabData.opScore or 0
    sendcount:setString("" .. count)

    -- 大写
    local szcount = image_bg:getChildByName("txt_sendnum")
    local szstr = ""
    if count < 9999999999999 then
        szstr = ExternalFun.numberTransiform(count)
    end
    szcount:setString(szstr)

    -- 日期
    local txtdate = image_bg:getChildByName("txt_date")    
    local tt = os.date("*t", stamp)
    txtdate:setString(string.format("%d.%02d.%02d-%02d:%02d:%02d", tt.year, tt.month, tt.day, tt.hour, tt.min, tt.sec))

    -- 凭证
    local cer = image_bg:getChildByName("txt_cerno")
    cer:setString(md5(stamp))

    -- 分享
    local btn = image_bg:getChildByName("btn_share")
    btn:setTag(BTN_SHARE)
    btn:addTouchEventListener( touchFunC )

    -- 保存
    btn = image_bg:getChildByName("btn_save")
    btn:setTag(BTN_SAVEPIC)
    btn:addTouchEventListener( touchFunC )

    -- 加载动画
    image_bg:runAction(cc.ScaleTo:create(0.2, 1.0))
end

return BankLayer