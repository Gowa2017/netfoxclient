--[[
	银行界面
	2017_08_25 MXM
]]

local AgentShengQingConfirmLayer = class("AgentShengQingConfirmLayer", cc.BaseLayer)

local PopWait = appdf.req(appdf.CLIENT_SRC.."app.views.layer.other.PopWait")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")

--开通银行
function AgentShengQingConfirmLayer:ctor(data)
	self.super.ctor(self)
    -- 加载csb资源
	local csbPath = "Shop/AgentRecharge/AgentRechargeShengQingConfirm.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	self.data = data
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
end

function AgentShengQingConfirmLayer:initUI()
	local imgBg = self._csbNode:getChildByName("bg")
	
     --返回按钮
    local btn = imgBg:getChildByName("btn_close")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
    --确定按钮
    btn = imgBg:getChildByName("btn_sumit_sq")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	self._text_1_0 = imgBg:getChildByName("Text_1_0")
	self._text_1_0:setString(self.data[1] or "")
	self._text_2_0 = imgBg:getChildByName("Text_2_0")
	self._text_2_0:setString(self.data[2] or "")
	self._text_3_0 = imgBg:getChildByName("Text_3_0")
	self._text_3_0:setString(self.data[3] or "")
	self._text_4_0 = imgBg:getChildByName("Text_4_0")
	self._text_4_0:setString(self.data[4] or "")
	self._text_5_0 = imgBg:getChildByName("Text_5_0")
	self._text_5_0:setString(self.data[5] or "")
	self._text_6_0 = imgBg:getChildByName("Text_6_0")
	self._text_6_0:setString(self.data[6] or "")
	
	self:attach("M2V_ShowToast", function (e, msg)
		showToast(self, msg, 2)
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
		local pos = touch:getLocation();
		local pBg = self._csbNode:getChildByName("bg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOP_SHENGQING_DAILI_COMFIRM})
			
			--回馈参数
			local text1 = self._text_1_0:getString()
			local text2 = self._text_2_0:getString()
			local text3 = self._text_3_0:getString()
			local text4 = self._text_4_0:getString()
			local text5 = self._text_5_0:getString()
			local text6 = self._text_6_0:getString()
			
			local backinfo = {text1, text2, text3, text4, text5, text6}
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {backinfo}}, VIEW_LIST.SHOP_SHENGQING_DAILI)
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
end

function AgentShengQingConfirmLayer:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)

	self._csbNodeAni:play("openAni", false)
end

function AgentShengQingConfirmLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)

	self._csbNodeAni:play("closeAni", false)
end

--按键监听
function AgentShengQingConfirmLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()


	local senderName = sender:getName()
	
	if senderName == "btn_close" then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOP_SHENGQING_DAILI_COMFIRM})
		
		--回馈参数
		local text1 = self._text_1_0:getString()
		local text2 = self._text_2_0:getString()
		local text3 = self._text_3_0:getString()
		local text4 = self._text_4_0:getString()
		local text5 = self._text_5_0:getString()
		local text6 = self._text_6_0:getString()
		
		local backinfo = {text1, text2, text3, text4, text5, text6}
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {backinfo}}, VIEW_LIST.SHOP_SHENGQING_DAILI)
		
	elseif senderName == "btn_sumit_sq" then
		--回馈参数
		local text1 = self._text_1_0:getString()
		local text2 = self._text_2_0:getString()
		local text3 = self._text_3_0:getString()
		local text4 = self._text_4_0:getString()
		local text5 = self._text_5_0:getString()
		local text6 = self._text_6_0:getString()
		
		
		local ClientData = ExternalFun.create_netdata(logincmd.CMD_MB_ShenQingDaili)
		ClientData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_SHENGQING_AGENT)
		ClientData:pushdword(GlobalUserItem.dwUserID)
		ClientData:pushstring(text1,yl.LEN_ACCOUNTS)
		ClientData:pushstring(text2,yl.LEN_AGENT_ID)
		ClientData:pushstring(text3,yl.LEN_ALIPAY)
		ClientData:pushstring(text4,yl.LEN_WECHAT)
		ClientData:pushstring(text5,yl.LEN_QQ)
		ClientData:pushstring(text6,yl.LEN_DESC)
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"正在提交申请，请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)
		appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, ClientData, function (Datatable, Responce)
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
			if (Responce.code == 0) then
				if Datatable.sub == logincmd.SUB_GP_OPERATE_SUCCESS then
					local pData = Datatable.data
					local lResultCode = pData:readint()
					local szDescribe = pData:readstring()
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOP_SHENGQING_DAILI_COMFIRM})
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {szDescribe,nil, true, 1}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)	
				elseif Datatable.sub == logincmd.SUB_GP_OPERATE_FAILURE then
					local pData = Datatable.data
					local lResultCode = pData:readint()
					local szDescribe = pData:readstring()
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {szDescribe,nil, true, 1}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)		
				end
			else
				showToast(self, "网络连接失败！", 2)
			end
		end)		
	end
end

--显示等待
function AgentShengQingConfirmLayer:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function AgentShengQingConfirmLayer:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end

return AgentShengQingConfirmLayer