local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local loginCMD = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
--[[
	包含 PromoterInputLayer QrCodeLayer SelectHeadLayer UserInfoLayer 三个类
]]

--二维码界面
local SHARE_QRCODE = 1
local SAVE_QECODE = 2
local QrCodeLayer = class("QrCodeLayer", cc.Layer)
function QrCodeLayer:ctor()
	--注册触摸事件
	ExternalFun.registerTouchEvent(self, true)

	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("userinfo/QrCodeLayer.csb", self)
	self.m_spBg = csbNode:getChildByName("qr_sp_bg")
	local bgsize = self.m_spBg:getContentSize()

	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender:getTag(), sender)
		end
	end

	self.m_qrContent = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
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

-- 代理填写界面
local TAG_MASK = 101
local BT_SURE = 102
local BT_S_WECHAT = 103 	--分享到微信
local BT_S_CYCLE = 104 		--分享到朋友圈
local BT_S_CONTACT = 105 	--分享到短信
local BT_COPY = 106 		--复制粘贴板
local BT_QR_ENTER = 107 	--点击显示二维码

local PromoterInputLayer = class("PromoterInputLayer", cc.Layer)
function PromoterInputLayer:ctor( viewparent )
	self.m_parent = viewparent
	local noSpreader = GlobalUserItem.szSpreaderAccount == ""

	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("userinfo/PromoterInputLayer.csb", self)
	self.m_spBg = csbNode:getChildByName("qr_sp_bg")
	self.m_spBg:setTouchEnabled(true)
    self.m_spBg:setSwallowTouches(true)

	local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    -- 编辑框
    local tmp = self.m_spBg:getChildByName("sp_uinfoedit_bg")
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, 60),"blank.png",UI_TEX_TYPE_PLIST)
        :setPosition(tmp:getPosition())
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setPlaceHolder("请输入推广员ID")
        :setMaxLength(32)
        :setPlaceholderFontColor(cc.c3b(51, 45, 106))
        :setFontColor(cc.c3b(51, 45, 106))
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
    self.m_spBg:addChild(editbox)
    editbox:setContentSize(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10))
    editbox:setPlaceholderFont("fonts/round_body.ttf", 30)
    editbox:setFont("fonts/round_body.ttf", 30)
    editbox:registerScriptEditBoxHandler(editHanlder)
    self.m_bEditChat = false
    self.m_editID = editbox
    editbox:setEnabled(noSpreader)

    -- 邀请奖励
    local atlas = self.m_spBg:getChildByName("atlas_sendcount")
    local count = GlobalUserItem.nInviteSend or 0
    atlas:setString(count .. "")

    -- 确定按钮
    local btn = self.m_spBg:getChildByName("btn_sure")
    btn:setTag(BT_SURE)
    btn:addTouchEventListener( touchFunC )
    btn:setEnabled(noSpreader)
    if not noSpreader then
    	editbox:setPlaceHolder(GlobalUserItem.szSpreaderAccount)
    	btn:setOpacity(125)
    end

    -- 关闭按钮
    btn = self.m_spBg:getChildByName("btn_close")
    btn:setTag(TAG_MASK)
    btn:addTouchEventListener( touchFunC )

    -- 微信分享按钮
    btn = self.m_spBg:getChildByName("btn_wechat")
    btn:setTag(BT_S_WECHAT)
    btn:addTouchEventListener( touchFunC )

    -- 朋友圈
    btn = self.m_spBg:getChildByName("btn_cycle")
    btn:setTag(BT_S_CYCLE)
    btn:addTouchEventListener( touchFunC )

    -- 通讯录
    btn = self.m_spBg:getChildByName("btn_contact")
    btn:setTag(BT_S_CONTACT)
    btn:addTouchEventListener( touchFunC )

    -- 复制
    btn = self.m_spBg:getChildByName("btn_copy")
    btn:setTag(BT_COPY)
    btn:addTouchEventListener( touchFunC )

    -- 二维码
    btn = self.m_spBg:getChildByName("btn_qrcode")
    btn:setTag(BT_QR_ENTER)
    btn:addTouchEventListener( touchFunC )

    self.m_qrContent = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
	local qr = QrNode:createQrNode(self.m_qrContent, 184, 5, 1)
    self.m_spBg:addChild(qr)
    qr:setPosition(btn:getPosition())
	self.m_qrNode = qr
end

function PromoterInputLayer:onButtonClickedEvent( tag, sender )
	if BT_SURE == tag then
		local txt = string.gsub(self.m_editID:getText(), "[.]", "")
		if txt == "" then
			showToast(self, "推广员ID不能为空!", 1)
			return
		end
		txt = tonumber(txt)
		if nil == txt then
			showToast(self, "请输入合法的推广员ID!", 1)
			return
		end
		self.m_parent:showPopWait()
		self.m_parent._modifyFrame:onBindSpreader(txt)
		self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5), cc.CallFunc:create(function()
			self:removeFromParent()
		end)))
	elseif BT_S_WECHAT == tag then
        local function sharecall( isok )
            if type(isok) == "string" and isok == "true" then
                showToast(self, "分享完成", 1)
            end
        end
        local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
        MultiPlatform:getInstance():shareToTarget(yl.ThirdParty.WECHAT, sharecall, yl.SocialShare.title, yl.SocialShare.content, url)
	elseif BT_S_CYCLE == tag then
		local function sharecall( isok )
            if type(isok) == "string" and isok == "true" then
                showToast(self, "分享完成", 1)
            end
        end
        local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
        MultiPlatform:getInstance():shareToTarget(yl.ThirdParty.WECHAT_CIRCLE, sharecall, yl.SocialShare.title, yl.SocialShare.content, url)
	elseif BT_S_CONTACT == tag then
		local function sharecall( isok )
            if type(isok) == "string" and isok == "true" then
                showToast(self, "分享完成", 1)
            end
        end
        local url = GlobalUserItem.szSpreaderURL or yl.HTTP_URL
        MultiPlatform:getInstance():shareToTarget(yl.ThirdParty.SMS, sharecall, yl.SocialShare.title, url)
	elseif BT_COPY == tag then
		local url = GlobalUserItem.szSpreaderURL or yl.HTTP_URL
		local res, msg = MultiPlatform:getInstance():copyToClipboard(url)
		if true == res then
			showToast(self, "复制到剪贴板成功!", 1)
		else
			if type(msg) == "string" then
				showToast(self, msg, 1, cc.c3b(250,0,0))
			end
		end
	elseif BT_QR_ENTER == tag then
		local qrlayer = QrCodeLayer:create()
		self.m_parent:addChild(qrlayer)
		self:removeFromParent()
	elseif TAG_MASK == tag then
		if false == self.m_bEditChat then
			self:removeFromParent()
		end		
		if true == self.m_bEditChat then
	        self.m_bEditChat = false
	    end
	end
end

function PromoterInputLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditChat = string.len(src) ~= 0
    elseif event == "changed" then
        self.m_bEditChat = string.len(src) ~= 0
    end
end

------
--头像选择界面
local SelectHeadLayer = class("SelectHeadLayer", cc.Layer)

SelectHeadLayer.BTN_LOCAL = 101
SelectHeadLayer.BTN_SYS = 102
function SelectHeadLayer:ctor( viewparent )
	self.m_parent = viewparent
	--注册触摸事件
	ExternalFun.registerTouchEvent(self, true)

	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("userinfo/SelectHeadLayer.csb", self)

	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender:getTag(), sender)
		end
	end
	------
	--选择方式
	self.m_spSelectType = csbNode:getChildByName("select_type")
	--本地图片
	local btn = self.m_spSelectType:getChildByName("local_btn")
	btn:setTag(SelectHeadLayer.BTN_LOCAL)
	btn:addTouchEventListener(btnEvent)
	--系统头像
	btn = self.m_spSelectType:getChildByName("system_btn")
	btn:setTag(SelectHeadLayer.BTN_SYS)
	btn:addTouchEventListener(btnEvent)
	------

	------
	--系统头像列表
	self.m_spSysSelect = csbNode:getChildByName("head_list")
	self.m_spSysSelect:setVisible(false)
	self.m_tableView = nil
	self.m_tabSystemHead = {}
	self.m_vecTouchBegin = {x = 0, y = 0}
	--------------------------------
end

function SelectHeadLayer:refreshSystemHeadList(  )
	if nil == self.m_tableView then
		local content = self.m_spSysSelect:getChildByName("content")
		local m_tableView = cc.TableView:create(content:getContentSize())
		m_tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
		m_tableView:setPosition(content:getPosition())
		m_tableView:setDelegate()
		m_tableView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
		m_tableView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
		m_tableView:registerScriptHandler(self.numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)
		self.m_spSysSelect:addChild(m_tableView)
		self.m_tableView = m_tableView
		content:removeFromParent()
		self.m_tableView:reloadData()
	end
end

function SelectHeadLayer:onButtonClickedEvent( tag, sender )
	self.m_spSelectType:setVisible(false)
	if SelectHeadLayer.BTN_LOCAL == tag then		
		local function callback( param )
			if type(param) == "string" then
				print("lua call back " .. param)
				if cc.FileUtils:getInstance():isFileExist(param) then
					--发送上传头像
					local url = yl.HTTP_URL .. "/WS/Account.ashx?action=uploadface"
					local uploader = CurlAsset:createUploader(url,param)
					if nil == uploader then
						showToast(self, "自定义头像上传异常", 2)
						return
					end
					local nres = uploader:addToFileForm("file", param, "image/png")
					--用户标示
					nres = uploader:addToForm("userID", GlobalUserItem.dwUserID)
					--登陆时间差
					local delta = tonumber(currentTime()) - tonumber(GlobalUserItem.LogonTime)
					print("time delta " .. delta)
					nres = uploader:addToForm("time", delta .. "")
					--客户端ip
					local ip = MultiPlatform:getInstance():getClientIpAdress() or "192.168.1.1"
					nres = uploader:addToForm("clientIP", ip)
					--机器码
					local machine = GlobalUserItem.szMachine or "A501164B366ECFC9E249163873094D50"
					nres = uploader:addToForm("machineID", machine)
					--会话签名
					nres = uploader:addToForm("signature", GlobalUserItem:getSignature(delta))
					if 0 ~= nres then
						showToast(self, "上传表单提交异常,error code ==> " .. nres, 2)
						return
					end
					self.m_parent:showPopWait()
					uploader:uploadFile(function(sender, ncode, msg)
						self:onUploadFaceResult(sender, ncode, msg, param)
					end)
				end
			end
		end
		MultiPlatform:getInstance():triggerPickImg(callback, true)
	elseif SelectHeadLayer.BTN_SYS == tag then
		self.m_spSysSelect:setVisible(true)
		self:refreshSystemHeadList()
	end
end

function SelectHeadLayer:onUploadFaceResult(sender, ncode, msg, param)
	self.m_parent:dismissPopWait()
	if 0 == ncode then
		if type(msg) == "string" then
			print("msg ==> " .. msg)
		end
		local ok, datatable = pcall(function()
				return cjson.decode(msg)
		end)
		if ok then
			dump(datatable, "datatable")
			if nil ~= datatable.code and 0 == datatable.code then
				local msgdata = datatable.data
				dump(msgdata, "msgdata")
				if nil ~= msgdata and type(msgdata) == "table" then			
					local valid = msgdata.valid
					if valid then
						cc.Director:getInstance():getTextureCache():removeTextureForKey(param)	    									
						local sp = cc.Sprite:create(param)
						if nil ~= sp then
							GlobalUserItem.dwCustomID = tonumber(msgdata.CustomID)
							local frame = sp:getSpriteFrame()
							local framename = GlobalUserItem.dwUserID .. "_custom_" .. GlobalUserItem.dwCustomID .. ".ry"
							local oldframe = cc.SpriteFrameCache:getInstance():getSpriteFrame(framename)
							if nil ~= oldframe then
								oldframe:release()
							end	
							cc.SpriteFrameCache:getInstance():removeSpriteFrameByName(framename)

							cc.SpriteFrameCache:getInstance():addSpriteFrame(frame, framename)
							frame:retain()
							
							if nil ~= self.m_parent and nil ~= self.m_parent.onFaceResultSuccess then
								self.m_parent:onFaceResultSuccess()
							end
							showToast(self, "自定义头像上传成功", 2)
						end		    				
					else
						if type(msg.msg) == "string" then
							showToast(self, msg.msg, 2)
						end			    							
					end
				end
			end
		else
			showToast(self, "自定义头像上传异常", 2)
		end
		return					
	end
	showToast(self, "自定义头像上传异常, error code ==> " .. ncode, 2)
end

function SelectHeadLayer:onTouchBegan(touch, event)
	self.m_vecTouchBegin = {x = touch:getLocation().x, y = touch:getLocation().y}
	return self:isVisible()
end

function SelectHeadLayer:onTouchEnded( touch, event )
	local pos = touch:getLocation()
	if math.abs(pos.x - self.m_vecTouchBegin.x) > 30 
		or math.abs(pos.y - self.m_vecTouchBegin.y) > 30 then
		self.m_vecTouchBegin = {x = 0, y = 0}

		return
	end
	
	local m_spBg = nil
	if self.m_spSelectType:isVisible() then
		m_spBg = self.m_spSelectType
	elseif self.m_spSysSelect:isVisible() then
		m_spBg = self.m_spSysSelect

		local touchHead = false
		local touchSp = nil
		for i,v in pairs(self.m_tabSystemHead) do
			local tmppos = v:convertToNodeSpace(pos)
			local headRect = cc.rect(0, 0, v:getContentSize().width, v:getContentSize().height)
			if true == cc.rectContainsPoint(headRect, tmppos) then
		        touchHead = true
		        touchSp = v
		        break
		    end
		end

		if true == touchHead and nil ~= touchSp then
			local tag = touchSp:getTag()
			print("touch head " .. tag)
			if nil ~= self.m_parent and nil ~= self.m_parent.sendModifySystemFace then
				self.m_parent:sendModifySystemFace(tag)
			end
			return
		end
	end
	if nil == m_spBg then
		self:removeFromParent()
		return
	end

    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        self:removeFromParent()
    end
end

--tableview
function SelectHeadLayer.cellSizeForTable( view, idx )
	return 940,95
end

function SelectHeadLayer.numberOfCellsInTableView( view )
	--一行10个，200个
	return 20
end

function SelectHeadLayer:tableCellAtIndex( view, idx )
	local cell = view:dequeueCell()
	idx = 19 - idx
	--[[if nil ~= cell and nil ~= cell:getChildByName("head_item_view") then
		cell:removeChildByName("head_item_view", true)	
	end]]
	
	if nil == cell then
		cell = cc.TableViewCell:new()
		local item = self:groupSysHead(idx, view)
		item:setPosition(view:getViewSize().width * 0.5, 0)
		item:setName("head_item_view")
		item:setTag(idx)
		cell:addChild(item)
	else
		local item = cell:getChildByName("head_item_view")
		item:setTag(idx)
		self:updateCellItem(item, idx, view)
	end	

	return cell
end

local xTable = {-450}
function SelectHeadLayer:groupSysHead( idx, view )
	local item = cc.Node:create()
	local xStart = -450
	local str = ""
	local head = nil
	local frame = nil
	local tag = 0
	for i = 0, 9 do
		head = nil
		tag = idx * 10 + i
		str = string.format("Avatar%d.png", tag)
		frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
		if nil ~= frame then
			head = cc.Sprite:createWithSpriteFrame(frame)
		end

		if nil ~= head then
			item:addChild(head)
			head:setTag(tag)
			local xPos = xStart + i * 100
			head:setPosition(xPos, 45)
			head:setScale(80 / 96)
			head:setName("head_" .. i)

			self.m_tabSystemHead[tag] = head

			local frame = nil
			if tag == GlobalUserItem.wFaceID then
				frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_select_bg.png")
			else
				frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_normal_frame.png")
			end
			if nil ~= frame then
				local spFrame = cc.Sprite:createWithSpriteFrame(frame)
				spFrame:setPosition(xPos, 45)
				item:addChild(spFrame, -1)
				spFrame:setName("frame_" .. i)
			end
		end
	end
	return item
end

function SelectHeadLayer:updateCellItem( item, idx, view )
	local tag = 0
	local frame = nil
	local str = ""
	for i = 0, 9 do
		head = nil
		tag = idx * 10 + i
		frame = nil
		str = ""

		local head = item:getChildByName("head_" .. i)
		if nil ~= head then
			str = string.format("Avatar%d.png", tag)
			frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
			if nil ~= frame then
				head:setSpriteFrame(frame)
				head:setTag(tag)

				self.m_tabSystemHead[tag] = head
			end
		end
		
		local spFrame = item:getChildByName("frame_" .. i)
		if nil ~= spFrame then
			frame = nil
			if tag == GlobalUserItem.wFaceID then
				frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_select_bg.png")
			else
				frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_normal_frame.png")
			end

			if nil ~= frame then
				spFrame:setSpriteFrame(frame)
			end
		end
	end
end

------
--个人信息界面
local UserInfoLayer = class("UserInfoLayer", function(scene)
		local userInfoLayer =  display.newLayer()
    return userInfoLayer
end)

local Bank = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.BankLayer")
local ModifyFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ModifyFrame")
local bGender = false
local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")


UserInfoLayer.BT_MODIFY_INFO 	= 1
UserInfoLayer.BT_BANK			= 2
UserInfoLayer.BT_MODIFY_PASS	= 3
UserInfoLayer.BT_ADD			= 4
UserInfoLayer.BT_EXIT			= 5

UserInfoLayer.BT_BINDING 		= 6
UserInfoLayer.BT_VIP 			= 7
UserInfoLayer.BT_NICKEDIT 		= 8
UserInfoLayer.BT_SIGNEDIT 		= 9
UserInfoLayer.BT_TAKE 			= 10
UserInfoLayer.BT_RECHARGE 		= 11
UserInfoLayer.BT_EXCHANGE 		= 12
UserInfoLayer.BT_AGENT 			= 16
UserInfoLayer.BT_CONFIRM		= 15

UserInfoLayer.CBT_MAN			= 13
UserInfoLayer.CBT_WOMAN			= 14
UserInfoLayer.LAY_SELHEAD		= 17

UserInfoLayer.BT_BAG			= 18
UserInfoLayer.BT_QRCODE 		= 19
UserInfoLayer.BT_PROMOTER 		= 20

function UserInfoLayer:ctor(scene)
	ExternalFun.registerNodeEvent(self)

	self._scene = scene

	local this = self
	self:setContentSize(yl.WIDTH,yl.HEIGHT) 

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    local cbtlistener = function (sender,eventType)
    	this:onSelectedEvent(sender:getTag(),sender,eventType)
    end

    self._bankNotify = function ()
    	this._txtScore:setString(string.formatNumberThousands(GlobalUserItem.lUserScore))
    end

	--网络回调
    local modifyCallBack = function(result,message)
		this:onModifyCallBack(result,message)
	end
    --网络处理
	self._modifyFrame = ModifyFrame:create(self,modifyCallBack)

    bGender = (GlobalUserItem.cbGender == yl.GENDER_MANKIND and true or false)

	--上方背景
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
	if nil ~= frame then
		local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,yl.HEIGHT-51)
        self:addChild(sp)
	end
	--标题
	display.newSprite("Information/title_info.png")
		:move(yl.WIDTH/2,yl.HEIGHT-51)
		:addTo(self)
	--返回
	ccui.Button:create("bt_return_0.png","bt_return_1.png")
    	:move(75,yl.HEIGHT-51)
    	:setTag(UserInfoLayer.BT_EXIT)
    	:addTo(self)
    	:addTouchEventListener(btcallback)
    --背包
    --[[ccui.Button:create("Information/btn_ubag_0.png","Information/btn_ubag_1.png")
    	:move(yl.WIDTH-90,yl.HEIGHT-51)
    	:setTag(UserInfoLayer.BT_BAG)
    	:addTo(self)
    	:addTouchEventListener(btcallback)]]
    -- 推广员
    --[[self.m_btnPromoter = ccui.Button:create("Information/btn_promoter_0.png","Information/btn_promoter_1.png","Information/btn_promoter_0.png")
    self.m_btnPromoter:move(yl.WIDTH-90,yl.HEIGHT-51)
    self.m_btnPromoter:setTag(UserInfoLayer.BT_PROMOTER)
    self.m_btnPromoter:addTo(self)
    self.m_btnPromoter:addTouchEventListener(btcallback)]]

    --下方背景
	frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,310)
        self:addChild(sp)
    end

	--头像
	--[[local texture = display.loadImage("face.png")
	local facex = (GlobalUserItem.cbGender == yl.GENDER_MANKIND and 1 or 0)
	self._head = display.newSprite(texture)
		:setTextureRect(cc.rect(facex*200,0,200,200))
		:move(210,485)
		:setScale(142.00/200.00)
		:addTo(self)]]
	local head = HeadSprite:createClipHead(GlobalUserItem, 142)
	head:setPosition(210, 445)
	head:enableHeadFrame(true)
	head:registerInfoPop(true, function( )
		self:onClickUserHead()		
	end)
	self:addChild(head)
	self._head = head

	--[[display.newSprite("Information/head_info_frame.png")
		:move(210,485)
		:addTo(self)]]

	--ID
	display.newSprite("Information/text_info_id.png")
		:move(150,328)
		:addTo(self)
	local textId = string.format("%d", GlobalUserItem.dwGameID)
	self._txtID = cc.Label:createWithTTF(textId, "fonts/round_body.ttf", 25)
		:setTextColor(cc.c4b(176,204,243,255))
		:setAnchorPoint(cc.p(0,0.5))
		:setDimensions(345,60)
		:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
		:setHorizontalAlignment(cc.TEXT_ALIGNMENT_LEFT)
		:move(190,328)
		:addTo(self)

	--等级
	display.newSprite("Information/icon_info_level.png")
		:move(175,280+15)
		:addTo(self)
	display.newSprite("Information/text_info_level.png")
		:move(215,278+15)
		:addTo(self)

	self._level = cc.LabelAtlas:_create(GlobalUserItem.wCurrLevelID.."", "Information/num_info_level.png", 16, 20, string.byte("0")) 
    		:move(235,278+15)
    		:setAnchorPoint(cc.p(0, 0.5))
    		:addTo(self)

    --进度条
    display.newSprite("Information/frame_info_level.png")
		:move(203,240+15)
		:addTo(self)
    --进度条
    self._levelpro = display.newSprite("Information/progress_info_level.png")
    	:setAnchorPoint(cc.p(0,0.5))
		:move(203-218/2,240+15)
		:addTo(self)
	
    --等级信息
    display.newSprite("Information/info_frame_4.png")
		:move(210,170)
		:addTo(self)

	if GlobalUserItem.dwUpgradeExperience >0 then
		local scalex = GlobalUserItem.dwExperience/GlobalUserItem.dwUpgradeExperience
		if scalex > 1 then
			scalex = 1
		end
		self._levelpro:setTextureRect(cc.rect(0,0,218*scalex,20))
	else
		self._levelpro:setTextureRect(cc.rect(0,0,1,20))
	end

	local nextexp = GlobalUserItem.dwUpgradeExperience - GlobalUserItem.dwExperience
	nextexp = (nextexp < 0) and 0 or nextexp
	cc.Label:createWithTTF("下次升级还需要"..(nextexp).."经验","fonts/round_body.ttf", 18)
		:move(210,140+45)
		:setTextColor(cc.c3b(250,250,0))
		:addTo(self)
	
	display.newSprite("Information/icon_info_up.png")
		:move(95,100+40)
		:addTo(self)
	local gold = GlobalUserItem.lUpgradeRewardGold 
	local szgold
	if gold > 9999 then
		szgold = string.format("%0.2f万",gold/10000) 
	else
		szgold = gold..""
	end
	cc.Label:createWithTTF("奖励：","fonts/round_body.ttf", 18)
		:move(210-50,115+40)
		:setTextColor(cc.c3b(250,250,0))
		:addTo(self)
	cc.Label:createWithTTF("+ "..szgold.." 游戏币","fonts/round_body.ttf", 18)
		:move(165+20,115+40)
		:setAnchorPoint(cc.p(0,0.5))
		:setTextColor(cc.c3b(250,250,0))
		:addTo(self)
	cc.Label:createWithTTF("+ "..GlobalUserItem.lUpgradeRewardIngot.." 元宝","fonts/round_body.ttf", 18)
		:move(165+20,120+10)
		:setAnchorPoint(cc.p(0,0.5))
		:setTextColor(cc.c3b(250,250,0))
		:addTo(self)

	--信息背景
	display.newSprite("Information/info_frame_1.png")
		:move(850,310)
		:addTo(self)

	--信息提示
	display.newSprite("Information/text_info_account.png")
		:move(533,537)
		:addTo(self)
	display.newSprite("Information/text_info_gender.png")
		:move(533,466)
		:addTo(self)
	display.newSprite("Information/text_info_nickname.png")
		:move(533,387)
		:addTo(self)
	display.newSprite("Information/text_info_sign.png")
		:move(503,310)
		:addTo(self)
	display.newSprite("Information/text_info_gold.png")
		:move(518,232)
		:addTo(self)
	display.newSprite("Information/text_info_bean.png")
		:move(518,155)
		:addTo(self)
	display.newSprite("Information/text_info_ingot.png")
		:move(533,80)
		:addTo(self)

	self._txtScore = ClipText:createClipText(cc.size(345,60), string.formatNumberThousands(GlobalUserItem.lUserScore,true), "fonts/round_body.ttf", 25)
	self._txtScore:setTextColor(cc.c4b(176,204,243,255))
	self._txtScore:setAnchorPoint(cc.p(0,0.5))
	self._txtScore:setPosition(590, 232)
	self:addChild(self._txtScore)

	self._txtBean = ClipText:createClipText(cc.size(345,60), string.format("%.2f", GlobalUserItem.dUserBeans), "fonts/round_body.ttf", 25)
	self._txtBean:setTextColor(cc.c4b(176,204,243,255))
	self._txtBean:setAnchorPoint(cc.p(0,0.5))
	self._txtBean:setPosition(590, 155)
	self:addChild(self._txtBean)

	self._txtIngot = ClipText:createClipText(cc.size(345,60), string.formatNumberThousands(GlobalUserItem.lUserIngot,true), "fonts/round_body.ttf", 25)
	self._txtIngot:setTextColor(cc.c4b(176,204,243,255))
	self._txtIngot:setAnchorPoint(cc.p(0,0.5))
	self._txtIngot:setPosition(590, 80)
	self:addChild(self._txtIngot)

	local account = GlobalUserItem.szAccount
	--微信登陆显示昵称
	if GlobalUserItem.bWeChat then
		account = GlobalUserItem.szNickName
	end
	local accountClip = ClipText:createClipText(cc.size(200, 30), account, "fonts/round_body.ttf", 25)
	accountClip:setTextColor(cc.c4b(176,204,243,255))
	accountClip:setAnchorPoint(cc.p(0,0.5))
	accountClip:setPosition(590,537)
	self:addChild(accountClip)

    if GlobalUserItem.bVisitor == true and false == GlobalUserItem.getBindingAccount() then
    	--绑定帐号
		ccui.Button:create("Information/bt_info_binding_0.png","Information/bt_info_binding_1.png")
	    	:move(860,537)
	    	:setTag(UserInfoLayer.BT_BINDING)
	    	:addTo(self)
	    	:addTouchEventListener(btcallback)
    end

    local vip = GlobalUserItem.cbMemberOrder or 0
    if 0 == vip then
    	--开通VIP
		ccui.Button:create("Information/bt_info_vip_0.png","Information/bt_info_vip_1.png")
	    	:move(1064,537)
	    	:setTag(UserInfoLayer.BT_VIP)
	    	:addTo(self)
	    	:addTouchEventListener(btcallback)
	else
		local sp_vip = cc.Sprite:create("Information/atlas_vipnumber.png")
		if nil ~= sp_vip then
			--sp_vip:setScale(1.5)
			sp_vip:setPosition(accountClip:getClipText():getContentSize().width + 610, 535)
			self:addChild(sp_vip)
			sp_vip:setTextureRect(cc.rect(28*vip,0,28,26))
		end
    end    

    if GlobalUserItem.bIsAngentAccount then
    	--我的代理
		ccui.Button:create("Information/bt_agent_0.png","Information/bt_agent_1.png")
	    	:move(1064,466)
	    	:setTag(UserInfoLayer.BT_AGENT)
	    	:addTo(self)
	    	:addTouchEventListener(btcallback)
    end   

    -- 我的推广码
    ccui.Button:create("Information/btn_qrcode_0.png","Information/btn_qrcode_1.png")
	    	:move(210,50)
	    	:setTag(UserInfoLayer.BT_QRCODE)
	    	:addTo(self)
	    	:addTouchEventListener(btcallback)

    --性别选择
    self._cbtMan = ccui.CheckBox:create("Information/checkbox_info_0.png","","Information/checkbox_info_1.png","","")
		:move(615,466)
		:setSelected(bGender)
		:addTo(self)
		:setTag(self.CBT_MAN)
	self._cbtMan:addEventListener(cbtlistener)

    self._cbtWoman = ccui.CheckBox:create("Information/checkbox_info_0.png","","Information/checkbox_info_1.png","","")
		:move(715,466)
		:setSelected(not bGender)
		:addTo(self)
		:setTag(self.CBT_WOMAN)
	self._cbtWoman:addEventListener(cbtlistener)

	display.newSprite("Information/icon_info_1.png")
		:move(654,466)
		:addTo(self)
	display.newSprite("Information/icon_info_0.png")
		:move(759,466)
		:addTo(self)

	display.newSprite("Information/info_frame_2.png")
		:move(710,387)
		:addTo(self)
	display.newSprite("Information/info_frame_3.png")
		:move(910,310)
		:addTo(self)
	display.newSprite("Information/icon_info_edit.png")
		:move(798,387)
		:addTo(self)
	display.newSprite("Information/icon_info_edit.png")
		:move(1198,310)
		:addTo(self)

	local editHanlder = function ( name, sender )
		self:onEditEvent(name, sender)
	end
	--输入框
	self.edit_Nickname = ccui.EditBox:create(cc.size(180,40), "blank.png")
		:move(688,387)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入您的游戏昵称")
		:setFontColor(cc.c4b(254,164,107,255))
		:setText(GlobalUserItem.szNickName)		
		:addTo(self)
	self.edit_Nickname:registerScriptEditBoxHandler(editHanlder)
	self.edit_Nickname:setName("edit_nickname")
	self.m_szNick = GlobalUserItem.szNickName

	self.edit_Sign = ccui.EditBox:create(cc.size(570,40), "blank.png")
		:move(884,310)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入您的个人签名")
		:setFontColor(cc.c4b(254,164,107,255))
		:setText(GlobalUserItem.szSign)		
		:addTo(self)
	self.edit_Sign:registerScriptEditBoxHandler(editHanlder)
	self.edit_Sign:setName("edit_sign")	
	self.m_szSign = GlobalUserItem.szSign

	--确认修改
	--[[ccui.Button:create("Information/bt_info_edit_0.png","Information/bt_info_edit_1.png")
    	:move(210,86)
    	:setTag(UserInfoLayer.BT_CONFIRM)
    	:addTo(self)
    	:addTouchEventListener(btcallback)]]

    --取款按钮
    ccui.Button:create("Information/bt_info_take_0.png","Information/bt_info_take_1.png")
    	:move(1045,232)
    	:setTag(UserInfoLayer.BT_TAKE)
    	:addTo(self)
    	:addTouchEventListener(btcallback)

    ccui.Button:create("Information/bt_info_recharge_0.png","Information/bt_info_recharge_1.png")
    	:move(1045,155)
    	:setTag(UserInfoLayer.BT_RECHARGE)
    	:addTo(self)
    	:addTouchEventListener(btcallback)

    ccui.Button:create("Information/bt_info_exchange_0.png","Information/bt_info_exchange_1.png")
    	:move(1045,80)
    	:setTag(UserInfoLayer.BT_EXCHANGE)
    	:addTo(self)
    	:addTouchEventListener(btcallback)
end


function UserInfoLayer:onButtonClickedEvent(tag,ref)
	if tag == UserInfoLayer.BT_EXIT then
		self._scene:onKeyBack()
	elseif tag == UserInfoLayer.BT_TAKE then
		if GlobalUserItem.isAngentAccount() then
			return
		end
		self._scene:onChangeShowMode(yl.SCENE_BANK)
	elseif tag == UserInfoLayer.BT_AGENT then
		self._scene:onChangeShowMode(yl.SCENE_AGENT)
	elseif tag == UserInfoLayer.BT_BINDING then
		if GlobalUserItem.isAngentAccount() then
			return
		end
		self:getParent():getParent():onChangeShowMode(yl.SCENE_BINDING)
	elseif tag == UserInfoLayer.BT_CONFIRM then
		if GlobalUserItem.isAngentAccount() then
			return
		end

		local szNickname = string.gsub(self.edit_Nickname:getText(), " ", "")

		--判断昵称长度
		if ExternalFun.stringLen(szNickname) < 6 then
			showToast(self, "游戏昵称必须大于6位以上,请重新输入!", 2)
			return
		end

		--判断是否有非法字符
		if true == ExternalFun.isContainBadWords(szNickname) then
			showToast(self, "昵称中包含敏感字符,请重试", 2)
			return
		end

		local szSign = string.gsub(self.edit_Sign:getText(), " ", "")

		--判断是否有非法字符
		if true == ExternalFun.isContainBadWords(szSign) then
			showToast(self, "个性签名中包含敏感字符,请重试", 2)
			return
		end

		if szNickname ~= GlobalUserItem.szNickName or szSign ~= GlobalUserItem.szSign then
			self:showPopWait()
			self._modifyFrame:onModifyUserInfo(GlobalUserItem.cbGender,szNickname,szSign)
		end
	elseif tag == UserInfoLayer.BT_VIP then
		if GlobalUserItem.isAngentAccount() then
			return
		end

		self:getParent():getParent():onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_VIP)
	elseif tag == UserInfoLayer.BT_RECHARGE then
		if GlobalUserItem.isAngentAccount() then
			return
		end

		self._scene:onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_BEAN)
	elseif tag == UserInfoLayer.BT_EXCHANGE then
		if GlobalUserItem.isAngentAccount() then
			return
		end

		self._scene:onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_ENTITY)
	elseif tag == UserInfoLayer.BT_BAG then
		if GlobalUserItem.isAngentAccount() then
			return
		end

		self._scene:onChangeShowMode(yl.SCENE_BAG)
	elseif tag == UserInfoLayer.BT_QRCODE then
		--local qrlayer = QrCodeLayer:create(self)
		--self:addChild(qrlayer)
		local prolayer = PromoterInputLayer:create(self)
		self:addChild(prolayer)
	elseif tag == UserInfoLayer.BT_PROMOTER then
		--local prolayer = PromoterInputLayer:create(self)
		--self:addChild(prolayer)
	end
end

function UserInfoLayer:onClickUserHead( )
	if GlobalUserItem.isAngentAccount() then
		return
	end
	if GlobalUserItem.notEditAble() then
		return
	end

	local tmp = SelectHeadLayer:create(self)
	tmp:setTag(UserInfoLayer.LAY_SELHEAD)
	self:addChild(tmp)
end

function UserInfoLayer:onSelectedEvent(tag,sender,eventType)	
	if GlobalUserItem.isAngentAccount() then
		sender:setSelected(not sender:isSelected())
		return
	end
	if GlobalUserItem.notEditAble() then
		sender:setSelected(not sender:isSelected())
		return
	end

	local szNickname = string.gsub(self.edit_Nickname:getText(), " ", "")
	local szSign = string.gsub(self.edit_Sign:getText(), " ", "")
	if tag == UserInfoLayer.CBT_MAN then
		if GlobalUserItem.cbGender == yl.GENDER_MANKIND then
			sender:setSelected(not sender:isSelected())
			return
		end
		if bGender ~= true then
			local cbGender = yl.GENDER_MANKIND
			self:showPopWait()
			self._modifyFrame:onModifyUserInfo(cbGender, szNickname, szSign)
		end		
	end

	if tag == UserInfoLayer.CBT_WOMAN then
		if GlobalUserItem.cbGender == yl.GENDER_FEMALE then
			sender:setSelected(not sender:isSelected())
			return
		end
		
		if bGender ~= false then
			local cbGender = yl.GENDER_FEMALE
			self:showPopWait()
			self._modifyFrame:onModifyUserInfo(cbGender, szNickname, szSign)
		end
	end	
end

function UserInfoLayer:onEditEvent(name, editbox)
	if "return" == name then
		if "edit_sign" == editbox:getName() then
			if GlobalUserItem.isAngentAccount() then
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end
			if GlobalUserItem.notEditAble() then
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end

			local szSign = string.gsub(self.edit_Sign:getText(), " ", "")
			--判断长度
			if ExternalFun.stringLen(szSign) < 1 then
				showToast(self, "个性签名不能为空", 2)
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end

			--判断emoji
			if ExternalFun.isContainEmoji(szSign) then
				showToast(self, "个性签名中包含非法字符,请重试", 2)
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end

			--判断是否有非法字符
			if true == ExternalFun.isContainBadWords(szSign) then
				showToast(self, "个性签名中包含敏感字符,请重试", 2)
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end
			self.m_szSign = szSign
			if szSign == GlobalUserItem.szSign then
				return
			end
		elseif "edit_nickname" == editbox:getName() then
			if GlobalUserItem.isAngentAccount() then
				self.edit_Nickname:setText(GlobalUserItem.szNickName)
				return
			end

			if GlobalUserItem.notEditAble() then
				self.edit_Nickname:setText(GlobalUserItem.szNickName)
				return
			end

			local szNickname = string.gsub(self.edit_Nickname:getText(), " ", "")
			--判断长度
			if ExternalFun.stringLen(szNickname) < 6 then
				showToast(self, "游戏昵称必须大于6位以上,请重新输入!", 2)
				self.edit_Nickname:setText(GlobalUserItem.szNickName)
				return
			end

			--判断emoji
			if ExternalFun.isContainEmoji(szNickname) then
				showToast(self, "昵称中包含非法字符,请重试", 2)
				self.edit_Sign:setText(GlobalUserItem.szSign)
				return
			end

			--判断是否有非法字符
			if true == ExternalFun.isContainBadWords(szNickname) then
				showToast(self, "昵称中包含敏感字符,请重试", 2)
				self.edit_Nickname:setText(GlobalUserItem.szNickName)
				return
			end
			self.m_szNick = szNickname
			if szNickname == GlobalUserItem.szNickName then
				return
			end
		end

		if self.m_szNick == "" or self.m_szSign == "" then
			return
		end
		self:showPopWait()
		self._modifyFrame:onModifyUserInfo(GlobalUserItem.cbGender,self.m_szNick,self.m_szSign)
	end
end

function UserInfoLayer:onExit( )
	if nil ~= self.edit_Sign then
		self.edit_Sign:unregisterScriptEditBoxHandler()
	end

	if nil ~= self.edit_Nickname then
		self.edit_Nickname:unregisterScriptEditBoxHandler()
	end

	if self._modifyFrame:isSocketServer() then
		self._modifyFrame:onCloseSocket()
	end
end

function UserInfoLayer:onEnterTransitionFinish()
	self:showPopWait()
	self._modifyFrame:onQueryUserInfo()
end

function UserInfoLayer:sendModifySystemFace( wFaceId )
	self:showPopWait()
	self._modifyFrame:onModifySystemHead(wFaceId)
end

--操作结果
function UserInfoLayer:onModifyCallBack(result,message)
	self:dismissPopWait()
	if  message ~= nil and message ~= "" then
		showToast(self,message,2);
	end
	if -1 == result then
		return
	end

	bGender = (GlobalUserItem.cbGender == yl.GENDER_MANKIND and true or false)
	self._cbtMan:setSelected(bGender)
	self._cbtWoman:setSelected(not bGender)

	if yl.SUB_GP_USER_FACE_INFO == result then
		self:onFaceResultSuccess()
		self:removeChildByTag(UserInfoLayer.LAY_SELHEAD, true)
	elseif loginCMD.SUB_GP_USER_INDIVIDUAL == result then
		-- 推广员按钮
		--local noSpreader = GlobalUserItem.szSpreaderAccount == ""
		--self.m_btnPromoter:setVisible(noSpreader)
		--self.m_btnPromoter:setEnabled(noSpreader)
	elseif self._modifyFrame.INPUT_SPREADER == result then
		--local noSpreader = GlobalUserItem.szSpreaderAccount == ""
		--self.m_btnPromoter:setVisible(noSpreader)
		--self.m_btnPromoter:setEnabled(noSpreader)
	end	
end

function UserInfoLayer:onKeyBack(  )
	if nil ~= self._scene._popWait then
		showToast(self, "当前操作不可返回", 2)
	end
    return true
end

--显示等待
function UserInfoLayer:showPopWait()
	self._scene:showPopWait()
end

--关闭等待
function UserInfoLayer:dismissPopWait()
	self._scene:dismissPopWait()
end

--头像更改结果
function UserInfoLayer:onFaceResultSuccess()
	--更新头像
	self._head:updateHead(GlobalUserItem)
	--通知
	local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
    eventListener.obj = yl.RY_MSG_USERHEAD
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

return UserInfoLayer