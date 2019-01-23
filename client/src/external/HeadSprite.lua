--
-- Author: zhong
-- Date: 2016-07-25 10:19:18
--
--游戏头像
local HeadSprite = class("HeadSprite", cc.Sprite)
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

--自定义头像存储规则
-- path/face/userid/custom_+customid.ry
--头像缓存规则
-- uid_custom_cusomid

local FACEDOWNLOAD_LISTENER = "face_notify_down"
local FACERESIZE_LISTENER = "face_resize_notify"
--全局通知函数
cc.exports.g_FaceDownloadListener = function (ncode, msg, filename)
	local event = cc.EventCustom:new(FACEDOWNLOAD_LISTENER)
	event.code = ncode
	event.msg = msg
	event.filename = filename

	cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

--
cc.exports.g_FaceResizeListener = function(oldpath, newpath)
	local event = cc.EventCustom:new(FACERESIZE_LISTENER)
	event.oldpath = oldpath
	event.newpath = newpath

	cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

local SYS_HEADSIZE = 96
function HeadSprite.checkData(useritem)
	useritem = useritem or {}
	useritem.dwUserID = useritem.dwUserID or 0
	useritem.dwCustomID = useritem.dwCustomID or 0
	useritem.wFaceID = useritem.wFaceID or 0
	if useritem.wFaceID > 199 then
		useritem.wFaceID = 0
	end
	useritem.cbMemberOrder = useritem.cbMemberOrder or 0

	return useritem
end

function HeadSprite:ctor( )
	self.m_spRender = nil
	self.m_downListener = nil
	self.m_resizeListener = nil

	--注册事件
	local function onEvent( event )
		if event == "exit" then
			self:onExit()
		elseif event == "enterTransitionFinish" then
			self:onEnterTransitionFinish()
        end
	end
	self:registerScriptHandler(onEvent)

	self.m_headSize = 96
	self.m_useritem = nil
	self.listener = nil
	self.m_bEnable = false
	--是否头像
	self.m_bFrameEnable = false
	--头像配置
	self.m_tabFrameParam = {}
end

--创建普通玩家头像
function HeadSprite:createNormal( useritem ,headSize)
	if nil == useritem then
		--return
	end
	useritem = HeadSprite.checkData(useritem)
	local sp = HeadSprite.new()
	sp.m_headSize = headSize
	local spRender = sp:initHeadSprite(useritem)
	if nil ~= spRender then
		sp:addChild(spRender)
		local selfSize = sp:getContentSize()
		spRender:setPosition(cc.p(selfSize.width * 0.5, selfSize.height * 0.5))
		return sp
	end
	
	return nil
end

--创建裁剪玩家头像
function HeadSprite:createClipHead( useritem, headSize, clippingfile )
	if nil == useritem then
		--return
	end
	useritem = HeadSprite.checkData(useritem)

	local sp = HeadSprite.new()
	sp.m_headSize = headSize
	local spRender = sp:initHeadSprite(useritem)
	if nil == spRender then
		return nil
	end 

	--创建裁剪
	local strClip = "head_mask.png"
	if nil ~= clippingfile then
		strClip = clippingfile
	end
	local clipSp = nil
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(strClip)
	if nil ~= frame then
		clipSp = cc.Sprite:createWithSpriteFrame(frame)
	else
		clipSp = cc.Sprite:create(strClip)
	end
	if nil ~= clipSp then
		--裁剪
		local clip = cc.ClippingNode:create()
		clip:setStencil(clipSp)
		clip:setAlphaThreshold(0)
		clip:addChild(spRender)
		local selfSize = sp:getContentSize()
		clip:setPosition(cc.p(selfSize.width * 0.5, selfSize.height * 0.5))
		sp:addChild(clip)
		return sp
	end

	return nil
end

function HeadSprite:updateHead( useritem )
	if nil == useritem then
		return
	end
	self.m_useritem = useritem

	--判断是否进入防作弊房间
	local bAntiCheat = GlobalUserItem.isAntiCheatValid(useritem.dwUserID)

	--更新头像框
	if self.m_bFrameEnable and false == bAntiCheat then
		local vipIdx = self.m_useritem.cbMemberOrder or 0

		--根据会员等级配置
		local vipIdx = self.m_useritem.cbMemberOrder or 0
		local framestr = string.format("sp_frame_%d_0.png", vipIdx)
		local deScale = 0.72

		local framefile = self.m_tabFrameParam._framefile or framestr
		local scaleRate = self.m_tabFrameParam._scaleRate or deScale
		local zorder = self.m_tabFrameParam._zorder or 1
		self:updateHeadFrame(framefile, scaleRate):setLocalZOrder(zorder)
	end

	if nil ~= useritem.dwCustomID and 0 ~= useritem.dwCustomID and false == bAntiCheat then
		--判断是否有缓存
		local framename = useritem.dwUserID .. "_custom_" .. useritem.dwCustomID .. ".ry"		
		local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(framename)
		if nil ~= frame then
			self:updateHeadByFrame(frame)
			return		
		end		
	end
	--系统头像
	local faceid = useritem.wFaceID or 0
	if true == bAntiCheat then
		faceid = 0
	end
	local str = string.format("Avatar%d.png", faceid)
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
	if nil ~= frame then
		self.m_spRender:setSpriteFrame(frame)
		self:setContentSize(self.m_spRender:getContentSize())		
	end
	self.m_fScale = self.m_headSize / SYS_HEADSIZE
	self:setScale(self.m_fScale)

	return sp
end

function HeadSprite:updateHeadByFrame(frame)
	if nil == self.m_spRender then
		self.m_spRender = cc.Sprite:createWithSpriteFrame(frame)
	else
		self.m_spRender:setSpriteFrame(frame)
	end
	print("width " .. self.m_spRender:getContentSize().width .. " height " .. self.m_spRender:getContentSize().height)
	
	self:setContentSize(self.m_spRender:getContentSize())
	self.m_fScale = self.m_headSize / SYS_HEADSIZE
	self:setScale(self.m_fScale)
end

--允许个人信息弹窗/点击头像触摸事件
function HeadSprite:registerInfoPop( bEnable, fun )
	self.m_bEnable = bEnable
	self.m_fun = fun

	if bEnable then
		--触摸事件
		self:registerTouch()
	else
		self:onExit()
	end
end

--头像框
--[[
frameparam = 
{
	--框文件
	_framefile 
	--缩放值
	_scaleRate
	--位置比例
	_posPer{}
	-- z顺序
	_zorder
}
]]
function HeadSprite:enableHeadFrame( bEnable, frameparam )
	if nil == self.m_useritem then
		return
	end
	self.m_bFrameEnable = bEnable
	local bAntiCheat = GlobalUserItem.isAntiCheatValid(self.m_useritem.dwUserID)

	if false == bEnable or bAntiCheat then
		if nil ~= self.m_spFrame then
			self.m_spFrame:removeFromParent()
			self.m_spFrame = nil
		end
		return
	end	
	local vipIdx = self.m_useritem.cbMemberOrder or 0

	--根据会员等级配置
	local vipIdx = self.m_useritem.cbMemberOrder or 0
	local framestr = string.format("sp_frame_%d_0.png", vipIdx)
	local deScale = 0.72

	frameparam = frameparam or {}
	self.m_tabFrameParam = frameparam
	local framefile = frameparam._framefile or framestr
	local scaleRate = frameparam._scaleRate or deScale
	local zorder = frameparam._zorder or 1
	self:updateHeadFrame(framefile, scaleRate):setLocalZOrder(zorder)
end

--更新头像框
function HeadSprite:updateHeadFrame(framefile, scaleRate)
	local spFrame = nil	
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(framefile)
	if nil == frame then
		spFrame = cc.Sprite:create(framefile)
		frame = (spFrame ~= nil) and spFrame:getSpriteFrame() or nil		
	end
	if nil == frame then
		return nil
	end

	if nil == self.m_spFrame then
		local selfSize = self:getContentSize()
		self.m_spFrame = cc.Sprite:createWithSpriteFrame(frame)
		local positionRate = self.m_tabFrameParam._posPer or cc.p(0.5, 0.64)
		self.m_spFrame:setPosition(selfSize.width * positionRate.x, selfSize.height * positionRate.y)
		self:addChild(self.m_spFrame)
	else
		self.m_spFrame:setSpriteFrame(frame)
	end
	self.m_spFrame:setScale(scaleRate)
	return self.m_spFrame
end

function HeadSprite:initHeadSprite( useritem )
	self.m_useritem = useritem
	local isThirdParty = useritem.bThirdPartyLogin or false	
	
	--系统头像
	local faceid = useritem.wFaceID or 0
	--判断是否进入防作弊房间
	local bAntiCheat = GlobalUserItem.isAntiCheatValid(useritem.dwUserID)
	if bAntiCheat then
		--直接使用系统头像
		faceid = 0
	elseif isThirdParty and nil ~= useritem.szThirdPartyUrl and string.len(useritem.szThirdPartyUrl) > 0 then
		local filename = string.gsub(useritem.szThirdPartyUrl, "[/.:+]", "") .. ".png"

		--判断是否有缓存或者本地文件
		local framename = filename
		local path = device.writablePath .. "face/" .. useritem.dwUserID
		local filepath = path .. "/" .. filename
		local bHave, spRender = self:haveCacheOrLocalFile(framename, filepath, false) 
		if bHave then
			return spRender
		else
			--判断是否有旧头像
			local infofile = path .. "/face.ry"
			if cc.FileUtils:getInstance():isFileExist(infofile) then
				local oldfile = cc.FileUtils:getInstance():getStringFromFile(infofile)
				local ok, datatable = pcall(function()
						return cjson.decode(oldfile)
				end)
				if ok and type(datatable) == "table" then
					for k ,v in pairs(datatable) do
						if v ~= filename then
							cc.FileUtils:getInstance():removeFile(path .. "/" .. v)
						end
					end
				end
			end	

			--网络请求
			local url = useritem.szThirdPartyUrl
			self:downloadFace(url, path, filename, function(downloadfile)
				local selffile = filename
				if selffile == downloadfile then
            		--保存头像信息
            		local infotable = {}
            		--导入新的图片信息
					table.insert(infotable, downloadfile)
            		local jsonStr = cjson.encode(infotable)
					cc.FileUtils:getInstance():writeStringToFile(jsonStr, infofile)

            		local filepath = path .. "/" .. selffile

            		--处理图片大小
            		reSizeGivenFile(filepath, filepath, "g_FaceResizeListener", SYS_HEADSIZE)
            		local function eventReSizeListener(event)
            			print("resize")
            			if nil == event.oldpath or nil == event.newpath then
            				return
            			end

            			--是否是自己文件
            			if event.newpath == filepath then
            				local sp = cc.Sprite:create(event.newpath)
            				if nil == sp then
            					return
            				end

            				--清理旧资源
            				local oldframe = cc.SpriteFrameCache:getInstance():getSpriteFrame(framename)
							if nil ~= oldframe then
								oldframe:release()
							end	

            				local customframe = cc.Sprite:create(event.newpath):getSpriteFrame()
							--缓存帧  										
							cc.SpriteFrameCache:getInstance():addSpriteFrame(customframe, framename)
							customframe:retain()
							self:updateHeadByFrame(customframe)

							--发送上传头像
							local url = yl.HTTP_URL .. "/WS/Account.ashx?action=uploadface"
							local uploader = CurlAsset:createUploader(url,filepath)
							if nil == uploader then
								return
							end
							local nres = uploader:addToFileForm("file", filepath, "image/png")
							--用户标示
							nres = uploader:addToForm("userID", useritem.dwUserID or "thrid")
							--登陆时间差
							local delta = tonumber(currentTime()) - tonumber(GlobalUserItem.LogonTime)
							print("time delta " .. delta)
							nres = uploader:addToForm("time", delta .. "")
							--客户端ip
							local ip = MultiPlatform:getInstance():getClientIpAdress() or "192.168.1.1"
							nres = uploader:addToForm("clientIP", ip)
							--机器码
							local machine = useritem.szMachine or "A501164B366ECFC9E249163873094D50"
							nres = uploader:addToForm("machineID", machine)
							--会话签名
							nres = uploader:addToForm("signature", GlobalUserItem:getSignature(delta))
							if 0 ~= nres then
								return
							end
							uploader:uploadFile(function(sender, ncode, msg)
								--showToast(self:getParent(), "上传完成", 2)
								--print(msg)
							end)
            			end							
            		end
            		self.m_resizeListener = cc.EventListenerCustom:create(FACERESIZE_LISTENER,eventReSizeListener)
					self:getEventDispatcher():addEventListenerWithFixedPriority(self.m_resizeListener, 1)
            	end
			end)			
		end
	elseif nil ~= useritem.dwCustomID and 0 ~= useritem.dwCustomID then
		--判断是否有缓存或者本地文件
		local framename = useritem.dwUserID .. "_custom_" .. useritem.dwCustomID .. ".ry"
		local filepath = device.writablePath .. "face/" .. useritem.dwUserID .. "/" .. framename

		local bHave, spRender = self:haveCacheOrLocalFile(framename, filepath, true)
		if bHave then
			return spRender
		else
			local path = device.writablePath .. "face/" .. useritem.dwUserID
			local filename = framename
			--判断是否有旧头像
			local infofile = path .. "/face.ry"
			if cc.FileUtils:getInstance():isFileExist(infofile) then
				local oldfile = cc.FileUtils:getInstance():getStringFromFile(infofile)
				local ok, datatable = pcall(function()
						return cjson.decode(oldfile)
				end)
				if ok and type(datatable) == "table" then
					for k ,v in pairs(datatable) do
						if v ~= filename then
							local oldframe = cc.SpriteFrameCache:getInstance():getSpriteFrame(v)
							if nil ~= oldframe then
								oldframe:release()
							end
							cc.FileUtils:getInstance():removeFile(path .. "/" .. v)
						end
					end
				end
			end			

			--网络请求
			local url = yl.HTTP_URL .. "/WS/UserFace.ashx?customid=" .. useritem.dwCustomID
			self:downloadFace(url, path, filename, function(downloadfile)
				--判断是否是自己头像
	        	local selffile = filename
	        	if selffile == downloadfile then
	        		--保存头像信息
	        		local infotable = {}
	        		--导入新的图片信息
					table.insert(infotable, downloadfile)
	        		local jsonStr = cjson.encode(infotable)
					cc.FileUtils:getInstance():writeStringToFile(jsonStr, infofile)

	        		local filepath = path .. "/" .. selffile
	        		local customframe = createSpriteFrameWithBMPFile(filepath)
	        		if nil ~= customframe then
	        			local oldframe = cc.SpriteFrameCache:getInstance():getSpriteFrame(framename)
						if nil ~= oldframe then
							oldframe:release()
						end	

						--缓存帧										
						cc.SpriteFrameCache:getInstance():addSpriteFrame(customframe, framename)
						customframe:retain()
						self:updateHeadByFrame(customframe)
					end
	        	end
			end)
		end
	end
	
	local str = string.format("Avatar%d.png", faceid)
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(str)
	if nil ~= frame then
		self.m_spRender = cc.Sprite:createWithSpriteFrame(frame)
		self:setContentSize(self.m_spRender:getContentSize())
	end	
	self.m_fScale = self.m_headSize / SYS_HEADSIZE
	self:setScale(self.m_fScale)

	return self.m_spRender
end

function HeadSprite:haveCacheOrLocalFile(framename, filepath, bmpfile)
	--判断是否有缓存
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(framename)
	if nil ~= frame then
		self:updateHeadByFrame(frame)
		return true, self.m_spRender
	else
		--判断是否有本地文件
		local path = filepath
		if cc.FileUtils:getInstance():isFileExist(path) then
			local customframe = nil
			if bmpfile then 
				customframe = createSpriteFrameWithBMPFile(path)
			else
				local sp = cc.Sprite:create(path)
				if nil ~= sp then
					customframe = sp:getSpriteFrame()
				end
			end
			if nil ~= customframe then
				--缓存帧
				local framename = self.m_useritem.dwUserID .. "_custom_" .. self.m_useritem.dwCustomID .. ".ry"    										
				cc.SpriteFrameCache:getInstance():addSpriteFrame(customframe, framename)
				customframe:retain()
				self:updateHeadByFrame(customframe)
				return true, self.m_spRender
			end
		end
	end
	return false
end

--下载头像
function HeadSprite:downloadFace(url, path, filename, onDownLoadSuccess)
	local downloader = CurlAsset:createDownloader("g_FaceDownloadListener",url)			
	if false == cc.FileUtils:getInstance():isDirectoryExist(path) then
		cc.FileUtils:getInstance():createDirectory(path)
	end			

	local function eventCustomListener(event)
        if nil ~= event.filename and 0 == event.code then
        	if nil ~= onDownLoadSuccess 
        		and type(onDownLoadSuccess) == "function" 
        		and nil ~= event.filename 
        		and type(event.filename) == "string" then
        		onDownLoadSuccess(event.filename)
        	end        	
        end
	end
	self.m_downListener = cc.EventListenerCustom:create(FACEDOWNLOAD_LISTENER,eventCustomListener)
	self:getEventDispatcher():addEventListenerWithFixedPriority(self.m_downListener, 1)
	downloader:downloadFile(path, filename)
end

function HeadSprite:registerTouch( )
	local function onTouchBegan( touch, event )
		return self:isVisible() and self:isAncestorVisible(self) and self.m_bEnable
	end

	local function onTouchEnded( touch, event )
		local pos = touch:getLocation()
        pos = self:convertToNodeSpace(pos)
        local rec = cc.rect(0, 0, self:getContentSize().width, self:getContentSize().height)
        if true == cc.rectContainsPoint(rec, pos) then
            if nil ~= self.m_fun then
            	self.m_fun()
            end
        end        
	end

	local listener = cc.EventListenerTouchOneByOne:create()
	self.listener = listener
    listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN )
    listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED )
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self)
end

function HeadSprite:isAncestorVisible( child )
	if nil == child then
		return true
	end
	local parent = child:getParent()
	if nil ~= parent and false == parent:isVisible() then
		return false
	end
	return self:isAncestorVisible(parent)
end

function HeadSprite:onExit( )
	if nil ~= self.listener then
		local eventDispatcher = self:getEventDispatcher()
		eventDispatcher:removeEventListener(self.listener)
		self.listener = nil
	end

	if nil ~= self.m_downListener then
		self:getEventDispatcher():removeEventListener(self.m_downListener)
		self.m_downListener = nil
	end

	if nil ~= self.m_resizeListener then
		self:getEventDispatcher():removeEventListener(self.m_resizeListener)
		self.m_resizeListener = nil
	end
end

function HeadSprite:onEnterTransitionFinish()
	if self.m_bEnable and nil == self.listener then
		self:registerTouch()
	end
end

--缓存头像
function HeadSprite.loadAllHeadFrame(  )
	if false == cc.SpriteFrameCache:getInstance():isSpriteFramesWithFileLoaded("public/im_head.plist") then
		--缓存头像
		cc.SpriteFrameCache:getInstance():addSpriteFrames("public/im_head.plist")
		--
		--手动retain所有的头像帧缓存、防止被释放
		local dict = cc.FileUtils:getInstance():getValueMapFromFile("public/im_head.plist")
		local framesDict = dict["frames"]
		if nil ~= framesDict and type(framesDict) == "table" then
			for k,v in pairs(framesDict) do
				local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(k)
				if nil ~= frame then
					frame:retain()
				end
			end
		end

		--缓存头像框
		cc.SpriteFrameCache:getInstance():addSpriteFrames("public/im_head_frame.plist")
		dict = cc.FileUtils:getInstance():getValueMapFromFile("public/im_head_frame.plist")
		framesDict = dict["frames"]
		if nil ~= framesDict and type(framesDict) == "table" then
			for k,v in pairs(framesDict) do
				local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(k)
				if nil ~= frame then
					frame:retain()
				end
			end
		end
	end
end

function HeadSprite.unloadAllHead(  )
	local dict = cc.FileUtils:getInstance():getValueMapFromFile("public/im_head_frame.plist")
	local framesDict = dict["frames"]
	if nil ~= framesDict and type(framesDict) == "table" then
		for k,v in pairs(framesDict) do
			local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(k)
			if nil ~= frame then
				frame:release()
			end
		end
	end

	cc.Director:getInstance():getTextureCache():removeUnusedTextures()
	cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()
end

--获取系统头像数量
function HeadSprite.getSysHeadCount(  )
	return 200
end

--缓存头像
HeadSprite.loadAllHeadFrame()

return HeadSprite