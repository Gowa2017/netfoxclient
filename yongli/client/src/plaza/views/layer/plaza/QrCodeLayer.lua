--二维码界面
local SHARE_QRCODE = 1
local SAVE_QECODE = 2
local QrCodeLayer = class("QrCodeLayer", cc.Layer)
function QrCodeLayer:ctor()
	--注册触摸事件
	ExternalFun.registerTouchEvent(self, true)

	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("Userinfo/QrCodeLayer.csb", self)
	self.m_spBg = csbNode:getChildByName("qr_sp_bg")
	local bgsize = self.m_spBg:getContentSize()

	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender:getTag(), sender)
		end
	end

	self.m_qrContent = GlobalUserItem.getShareUrl()
	local qr = QrNode:createQrNode(self.m_qrContent, 500, 5, 1)
    self.m_spBg:addChild(qr)
    qr:setPosition(bgsize.width * 0.5, bgsize.height * 0.5)
	self.m_qrNode = qr
	local head = HeadSprite:createClipHead(GlobalUserItem, 40)
	head:enableHeadFrame(true)
	self.m_qrNode:addChild(head)

	--分享
	local btn = self.m_spBg:getChildByName("btn_share")
	btn:setTag(SHARE_QRCODE)
	btn:addTouchEventListener(btnEvent)
	--存储
	btn = self.m_spBg:getChildByName("btn_save")
	btn:setTag(SAVE_QECODE)
	btn:addTouchEventListener(btnEvent)

	local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
	local xRate = framesize.width / yl.WIDTH 
	local yRate = framesize.height / yl.HEIGHT
	local screenPos = self.m_spBg:convertToWorldSpace(cc.p(bgsize.width * 0.5, bgsize.height * 0.5))
	local framePosX = (screenPos.x - 250) * xRate
	local framePosY = (screenPos.y - 250) * yRate
	local frameWidth = 500 * xRate
	local frameHeight = 500 * yRate
	self.m_qrFrameRect = cc.rect(framePosX, framePosY, frameWidth, frameHeight)
	self.m_screenPos = screenPos
	self.m_xRate = xRate
	self.m_yRate = yRate
end

function QrCodeLayer:onButtonClickedEvent( tag, sender )
	ExternalFun.playClickEffect()
	ExternalFun.popupTouchFilter(0, false)
	if tag == SHARE_QRCODE then
		captureScreenWithArea(self.m_qrFrameRect, "qr_code.png", function(ok, savepath)
			ExternalFun.dismissTouchFilter()
			if ok then
				MultiPlatform:getInstance():customShare(function(isok)
		         			end, "我的推广码", "分享我的推广码", self.m_qrContent, savepath, "true")
			end
		end)
	elseif tag == SAVE_QECODE then
		captureScreenWithArea(self.m_qrFrameRect, "qr_code.png", function(ok, savepath)			
			if ok then	
				if true == MultiPlatform:getInstance():saveImgToSystemGallery(savepath, "qr_code.png") then
					showToast(self, "您的推广码二维码图片已保存至系统相册", 1)
				end
			end
			self:runAction(cc.Sequence:create(cc.DelayTime:create(3), cc.CallFunc:create(function()
				ExternalFun.dismissTouchFilter()
			end)))
		end)
	end
end

function QrCodeLayer:onTouchBegan(touch, event)
    return self:isVisible()
end

function QrCodeLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:removeFromParent()
    end
end

function QrCodeLayer:onExit()
	self:stopAllActions()
end

return QrCodeLayer