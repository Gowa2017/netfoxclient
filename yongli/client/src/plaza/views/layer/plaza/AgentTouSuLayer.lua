--[[
	银行界面
	2017_08_25 MXM
]]

local AgentTouSuLayer = class("AgentTouSuLayer", cc.BaseLayer)

local PopWait = appdf.req(appdf.CLIENT_SRC.."app.views.layer.other.PopWait")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

--开通银行
function AgentTouSuLayer:ctor(agentid)
	self.super.ctor(self)
    -- 加载csb资源
	local csbPath = "Shop/AgentRecharge/AgentRechargeTouSu.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	
	self._agentid = agentid
	
	--是否有上传图片
	self._strUploadImage = ""
end

function AgentTouSuLayer:showToast(msg)
	local curScene = cc.Director:getInstance():getRunningScene()
	showToast(curScene, msg, 1)
end

function AgentTouSuLayer:initUI()
	local imgBg = self._csbNode:getChildByName("bg")
	
     --返回按钮
    local btn = imgBg:getChildByName("btn_close")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
    --确定按钮
    btn = imgBg:getChildByName("btn_agent_tousu")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
    --上传按钮
    btn = imgBg:getChildByName("btn_agent_upload")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))	
		
	local text_field_bg_1 = imgBg:getChildByName("text_field_bg_1")
	local text_field_1 = text_field_bg_1:getChildByName("text_field_1")	

	text_field_size = text_field_1:getContentSize()

	self.m_edit1 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入代理账号...")
		:addTo(text_field_1)
		
	if (self._agentid ~= nil or self._agentid ~= "") then
		self.m_edit1:setText(self._agentid)
	end
		
		
	local text_field_bg_2 = imgBg:getChildByName("text_field_bg_2")
	local text_field_2 = text_field_bg_2:getChildByName("text_field_2")	
	
	text_field_size = text_field_2:getContentSize()
	
	self.m_edit2 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(150)
		:setInputMode(cc.EDITBOX_INPUT_MODE_ANY)
		:setPlaceHolder("投诉原因,15个汉字以上...")
		:addTo(text_field_2)
		
		
	local text_field_bg_3 = imgBg:getChildByName("text_field_bg_3")
	local text_field_3 = text_field_bg_3:getChildByName("text_field_3")	
	
	text_field_size = text_field_3:getContentSize()
	
	self.m_edit3 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(150)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请留下联系方式...")
		:addTo(text_field_3)
		
	self:attach("M2V_ShowToast", function (e, msg)
		self:showToast(msg)
	end)	
		
	-- 添加点击监听事件 遮罩
    local mask = self._csbNode:getChildByName("mask")
	local Layoutlistener = cc.EventListenerTouchOneByOne:create()
	Layoutlistener:setSwallowTouches(true)
	Layoutlistener:registerScriptHandler(function(touch, event)
		return self:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	Layoutlistener:registerScriptHandler(function(touch, event)
--[[		local pos = touch:getLocation();
		local pBg = self._csbNode:getChildByName("bg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOP_TOUSU_DAILI})
		end--]]
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
end

function AgentTouSuLayer:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)

	self._csbNodeAni:play("openAni", false)
end

function AgentTouSuLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)

	self._csbNodeAni:play("closeAni", false)
end

--按键监听
function AgentTouSuLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	
	local senderName = sender:getName()
	
	if senderName == "btn_close" then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOP_TOUSU_DAILI})
	elseif senderName == "btn_agent_tousu" then
		local desc = self.m_edit2:getText()
		local userWx = self.m_edit3:getText()
		local agentWx = self.m_edit1:getText()
		
		if (self:checkParm(desc, userWx, agentWx, self._strUploadImage)) then
			self:emit("V2M_TouShuAgent", GlobalUserItem.dwUserID, userWx, agentWx, desc, self._strUploadImage)
		end
	elseif senderName == "btn_agent_upload" then
		local function async_callback( param )
			if type(param) == "string" then
				if cc.FileUtils:getInstance():isFileExist(param) then
					self._strUploadImage = param
				else
					showToast(self, "图像打开失败！", 2)
				end
			end
		end		
		MultiPlatform:getInstance():triggerPickImg(async_callback, false)		
	end
end


function AgentTouSuLayer:checkParm(desc, userWx, agentWx, imageName)
	--代理微信
	local check_agentWx = agentWx or ""
	len = ExternalFun.stringLen(check_agentWx)
	if len <= 0 then
		showToast(self,"代理号不能为空，请重新输入！",2,cc.c4b(250,255,255,255));
        return false
    end
	
	--我的微信
	local check_userWx = userWx or ""
	len = ExternalFun.stringLen(check_userWx)
	if len <= 0 then
		showToast(self,"我的微信号不能为空，请重新输入！",2,cc.c4b(250,255,255,255));
        return false
    end	
	
	-- 中文描述 个数
	local lenInByte = string.len(desc)
	local chinese_count = 0
	local index = 0
	while index < lenInByte do
		index = index + 1
		
		local curByte = string.byte(desc, index)
		local byteCount = 1;
		if curByte>0 and curByte<=127 then
			byteCount = 1
		elseif curByte>=192 and curByte<223 then
			byteCount = 2
			chinese_count = chinese_count + 1
		elseif curByte>=224 and curByte<239 then
			byteCount = 3
			chinese_count = chinese_count + 1
		elseif curByte>=240 and curByte<=247 then
			byteCount = 4
			chinese_count = chinese_count + 1
		end
		 
		local char = string.sub(desc, index, index+byteCount-1)
		index = index + byteCount -1
	end
	
	if (chinese_count <15) then
		showToast(self,"请输入至少15个汉字描述！",2,cc.c4b(250,255,255,255));
		return false
	end
	
--[[	local extFileName = appdf.strippath(imageName) or ""
	local check_imageName = appdf.stripextension(extFileName) or ""
	len = ExternalFun.stringLen(check_imageName)
	if len <= 0 then
		showToast(self,"上传图片不能为空，请选择图片！",2,cc.c4b(250,255,255,255));
        return false
    end--]]
	
	return true
end

--显示等待
function AgentTouSuLayer:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function AgentTouSuLayer:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end

return AgentTouSuLayer