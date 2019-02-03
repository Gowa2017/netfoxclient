--[[
	欢迎界面
			2015_12_03 C.P
	功能：本地版本记录读取，如无记录，则解压原始大厅及附带游戏
--]]

local WelcomeScene = class("WelcomeScene", cc.BaseScene)

local ClientUpdate = require("client.src.app.controllers.ClientUpdate")
local QueryDialog = require("client.src.app.views.layer.other.QueryDialog")
local luautil = require("client.src.app.models.luautil")

--win32是否需要更新 标识
local WIN32_NEED_CHECK_UPDATE = false
--全局toast函数(ios/android端调用)
cc.exports.g_NativeToast = function (msg)
	local runScene = cc.Director:getInstance():getRunningScene()
	if nil ~= runScene then
		showToastNoFade(runScene, msg, 2)
	end
end

WelcomeScene.S_CONNECT_SERVER = 1	--连接服务器中
WelcomeScene.S_LOADED_RES = 2		--加载游戏资源
WelcomeScene.S_UNZIP_RES = 3		--解压游戏资源

--顺序处理每个url, 按顺序挑选一个可用的url
local function singleTaskCheckUrl(urllist, callback)
	assert("table" == type(urllist), "expected table, but not now")
	if ("table" ~= type(urllist)) then
		return
	end
	
	assert("function" == type(callback), "expected function, but not now")
	if ("function" ~= type(callback)) then
		return
	end
	
	--通过协程遍历检查url可用性
    local checkUrlThread = nil
	checkUrlThread = coroutine.create(function()
		local url = ""
        while #urllist > 0 do
            url = table.remove(urllist, 1)

            local finish = false
            local function onCheckFinishCallBack(datatable)
				if type(datatable) == "table" then
					local code = datatable["code"]
					if (code == 0) then
						finish = true
						coroutine.resume(checkUrlThread)
					else--请求成功，但是数据存在异常
						printf("ConfigProxy got success, but data failed! url:%s\n", url)
					end
				else--请求失败
					printf("ConfigProxy got an error! url:%s\n", url)
				end
				
				coroutine.resume(checkUrlThread)
            end

            appdf.onHttpJsionTable(url .. "/index.txt","get","",onCheckFinishCallBack)
			
			coroutine.yield()
			if finish then
				callback(0, url)
				break
			else
				if (0 == #urllist) then
					--没有完成 放弃cpu周期，交出控制权
					if not finish then
						callback(-1, "")
						break
					end
				end
			end
			
        end
    end)

    -- start check
    coroutine.resume(checkUrlThread)    
end

function WelcomeScene:ctor()
	self.super.ctor(self)
    -- 加载csb资源
	local filePath = ""
	if (BaseConfig.AGENT_IDENTIFICATION == "") then
		filePath = "base/Loading.csb"
	else
		filePath = device.writablePath .. "client/res/base/Loading_" .. BaseConfig.AGENT_IDENTIFICATION .. ".csb"
		if (not cc.FileUtils:getInstance():isFileExist(filePath)) then
			filePath = "base/Loading_" .. BaseConfig.AGENT_IDENTIFICATION .. ".csb"
			if (not cc.FileUtils:getInstance():isFileExist(filePath)) then
				filePath = "base/Loading.csb"
			end
		end		
	end	
	
    local rootLayer, csbNode = appdf.loadRootCSB(filePath,self)
	self._csbNode = csbNode
	
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(filePath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)

	--屏幕中央提示文本
	self._tips = self._csbNode:getChildByName("txtBg_1"):getChildByName("tips")
	
	--右下角提示文本
	self._txtTips = self._csbNode:getChildByName("txtTips")

	--单文件进度
	self.m_spFileBg = self._csbNode:getChildByName("barbg_0")
	self.m_fileBar = self.m_spFileBg:getChildByName("single_bar")
	self.m_fileBar:setPercent(0)
	self.m_fileThumb = self.m_fileBar:getChildByName("thumb")
	self.m_fileThumb:setPositionY(self.m_fileBar:getContentSize().height * 0.5)
	self:updateBar(self.m_fileBar, self.m_fileThumb, 0)

	--总进度
	self.m_spTotalBg = self._csbNode:getChildByName("barbg_1")
	self.m_totalBar = self.m_spTotalBg:getChildByName("mul_bar")
	self.m_totalBar:setPercent(0)
	self.m_totalThumb = self.m_totalBar:getChildByName("thumb")
	self.m_totalThumb:setPositionY(self.m_totalBar:getContentSize().height * 0.5)
	self:updateBar(self.m_totalBar, self.m_totalThumb, 0)
	
	-- 资源同步队列
	self.m_tabUpdateQueue = {}
	
	local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
	
	--无版本信息或不对应 解压自带ZIP
	local nResversion = versionProxy:getResVersion()
	--定时器
	local schduler = cc.Director:getInstance():getScheduler()
	
	
	self._scheduleHandle = 0
	
	--检查网络锁
	self._checkLock = false
	local function async_callback()
		if(self._checkLock == false) then
			self._checkLock = true
			if nil == nResversion then
				--版本同步
				--为了兼容旧版本的bin文件 所以这行代码不能加
				self:checkWebUrlInfo(function (code)
					if (code == 0) then
						self:onUnZipBase() 
						schduler:unscheduleScriptEntry(self._scheduleHandle) 
						self._scheduleHandle = 0
					else
						self._checkLock = false
					end
				end)
			else
				--版本同步
				self:checkWebUrlInfo(function (code)
					if (code == 0) then
						self:checkNewVersion()
						schduler:unscheduleScriptEntry(self._scheduleHandle)
						self._scheduleHandle = 0
					else
						self._checkLock = false
					end
				end)
			end
		end
	end
	
	self._scheduleHandle = schduler:scheduleScriptFunc(async_callback, 1, false)	
end

--进入登录界面
function  WelcomeScene:EnterClient()
	--场景切换
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {transition = "FADE", time = 1}, VIEW_LIST.LOGON_SCENE)
end

--进入登录界面
--1 连接服务器 2 游戏加载中 3解压游戏中
function  WelcomeScene:setStatusTips(var)
	if (var == WelcomeScene.S_CONNECT_SERVER) then
		self._tips:setString("服务器连接中,请稍后...")
		local tipCount = 0
		local action = cc.RepeatForever:create(cc.Sequence:create(cc.DelayTime:create(1),
		cc.CallFunc:create(function()
			local count = tipCount % 3
			if (count == 0) then
				self._tips:setString("服务器连接中,请稍后.")
			elseif (count == 1) then
				self._tips:setString("服务器连接中,请稍后..")
			elseif (count == 2) then
				self._tips:setString("服务器连接中,请稍后...")
			end
			tipCount = tipCount + 1
		end)))
		self._tips:stopAllActions()
		self._tips:runAction(action)
	elseif(var == WelcomeScene.S_LOADED_RES) then
		self._tips:setString("资源加载中,请稍后...")
		local tipCount = 0
		local action = cc.RepeatForever:create(cc.Sequence:create(cc.DelayTime:create(1),
		cc.CallFunc:create(function()
			local count = tipCount % 3
			if (count == 0) then
				self._tips:setString("资源加载中,请稍后.")
			elseif (count == 1) then
				self._tips:setString("资源加载中,请稍后..")
			elseif (count == 2) then
				self._tips:setString("资源加载中,请稍后...")
			end
			tipCount = tipCount + 1
		end)))
		self._tips:stopAllActions()
		self._tips:runAction(action)
	elseif(var == WelcomeScene.S_UNZIP_RES) then
		self._tips:setString("初始化游戏资源,请稍后...")
		local tipCount = 0
		local action = cc.RepeatForever:create(cc.Sequence:create(cc.DelayTime:create(1),
		cc.CallFunc:create(function()
			local count = tipCount % 3
			if (count == 0) then
				self._tips:setString("初始化游戏资源,请稍后.")
			elseif (count == 1) then
				self._tips:setString("初始化游戏资源,请稍后..")
			elseif (count == 2) then
				self._tips:setString("初始化游戏资源,请稍后...")
			end
			tipCount = tipCount + 1
		end)))
		self._tips:stopAllActions()
		self._tips:runAction(action)		
	else
		self._tips:stopAllActions()
		self._tips:setVisible(false)
	end
end

--解压自带ZIP
function WelcomeScene:onUnZipBase()
	self:setStatusTips(WelcomeScene.S_UNZIP_RES)
	print("onUnZipBase")
	if self._unZip == nil then --大厅解压
		-- 状态提示
		self._txtTips:setString("解压客户端文件，请稍候...")
		self._unZip = 0
		--解压
		local dst = device.writablePath
--[[appdf.UNZIP_COMPELETED = 1						--下载完成
appdf.UNZIP_ERROR_PATH = 2						--目标文件不存在
appdf.UNZIP_FILE_FAILED = 3						--目标文件不存在
appdf.UNZIP_ERROR_CREATEFILE = 4				--创建文件失败--]]
		local client_package = cc.FileUtils:getInstance():fullPathForFilename("client.zip")
		unZipAsync(client_package,dst,function(code, parm)
				if (code == appdf.UNZIP_COMPELETED) then
					self:onUnZipBase()
				else					--忽略解压错误
					self:onUnZipBase()
				end
			end)
	elseif self._unZip == 0 then --默认游戏解压
		self._unZip = 1
		self._txtTips:setString("解压游戏文件，请稍候...")
		--解压
		local dst = device.writablePath
		unZipAsync(cc.FileUtils:getInstance():fullPathForFilename("game.zip"),dst,function(code, parm)
				if (code == appdf.UNZIP_COMPELETED) then
					self:onUnZipBase()
				else
					--忽略解压错误
					self:onUnZipBase()
				end
			end)
	else 			-- 解压完成
		self._unZip = nil
		
		local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
		--设置资源版本号
		versionProxy:setResVersion(BaseConfig.BASE_C_RESVERSION)
		--设置沙盒资源版本号
		versionProxy:setVersion(BaseConfig.BASE_C_VERSION)
		--更新状态
		self._txtTips:setString("解压完成！")

		--版本同步
	    self:checkNewVersion()
		return	
	end

end

--同步版本
function WelcomeScene:checkNewVersion()	
	local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")		
	
	self:setStatusTips(WelcomeScene.S_CONNECT_SERVER)

	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	--数据解析
	local vcallback = function(datatable)
	 	local succeed = false
	 	local msg = "网络获取失败！"
	 	if type(datatable) == "table" then	 		
            local databuffer = datatable["data"]
            if databuffer then
            	--dump(databuffer, "databuffer", 6)
                --返回结果
	 		    succeed = databuffer["valid"]
	 		    --提示文字
	 		    local tips = datatable["msg"]
	 		    if tips and tips ~= cjson.null then
	 			    msg = tips
				else
					msg = ""
	 		    end
	 		    --获取信息
	 		    if succeed == true then	 
	 		    	appConfigProxy._serverConfig = databuffer
 				    --下载地址
					appConfigProxy._updateUrl = databuffer["downloadurl"]
					
 				    if true == BaseConfig.APPSTORE_VERSION then
						local ret, lang = appdf.getAppleLanguages()
						lang = lang or ""
						if (string.find(lang, "zh")) then
							appConfigProxy._appStoreSwitch = tonumber(databuffer["appstore"])
						else
							appConfigProxy._appStoreSwitch = 1
						end
 				    end
					
					appConfigProxy._updatePackage = databuffer["packagename"]
 				    --大厅版本
 				    self._newVersion = tonumber(databuffer["clientversion"])          						--test zhong  0
 				    --大厅资源版本
 				    self._newResVersion = tonumber(databuffer["resversion"])
 				    --苹果大厅更新地址
 				    self._iosUpdateUrl = databuffer["ios_url"]	 
					
				    --游戏列表
 				    local rows = databuffer["gamelist"]
 				    appConfigProxy._gameList = {}
 				    for k,v in pairs(rows) do
 					    local gameinfo = {}
						gameinfo._KindType = tonumber(v["KindType"])
 					    gameinfo._KindID = v["KindID"]
 					    gameinfo._KindName = string.lower(v["ModuleName"]) .. "."
 					    gameinfo._Module = string.gsub(gameinfo._KindName, "[.]", "/")
 					    gameinfo._KindVersion = v["ClientVersion"]
 					    gameinfo._ServerResVersion = tonumber(v["ResVersion"])
 					    gameinfo._Type = gameinfo._Module
 					    --检查本地文件是否存在
 					    local path = device.writablePath .. "game/" .. gameinfo._Module
 					    gameinfo._Active = cc.FileUtils:getInstance():isDirectoryExist(path)
 					    local e = string.find(gameinfo._KindName, "[.]")
 					    if e then
 					    	gameinfo._Type = string.sub(gameinfo._KindName,1,e - 1)
 					    end
 					    -- 排序
 					    gameinfo._SortId = tonumber(v["SortID"]) or 0

 					    table.insert(appConfigProxy._gameList, gameinfo)
 				    end

 				    table.sort(appConfigProxy._gameList, function(a, b)
 				    	return a._SortId > b._SortId
 				    end)
	 		    end
            end	 		
	 	end
	 	self._txtTips:setString("")
		self:checkNewVersionCallBack(succeed,msg)
	end
	
	appdf.onHttpJsionTable(BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx","get", "action=getgamelist&agentAcc=" .. BaseConfig.AGENT_IDENTIFICATION, vcallback)
end

--检查网站有效性，无效自动切换，更新网站地址
function WelcomeScene:checkWebUrlInfo(func)
	assert(type(func) == "function", "expected function, but not now")
	if (type(func) ~= "function") then
		return
	end
	
	local function callback(code, url)
		if (code == 0) then
			--更新网站全局变量
			BaseConfig.WEB_HTTP_URL = url
			--记录最后一次可用的网站
			cc.UserDefault:getInstance():setStringForKey("LAST_WEBURI", url)
			cc.UserDefault:getInstance():flush()

			func(code)
			
			appdf.onHttpJsionTable(url .. "/WS/MobileInterface.ashx","get","action=getserverconfig",function(datatable, resp)
				local succeed = false
				local msg = "网络获取失败！"
				if type(datatable) == "table" then	 		
					local databuffer = datatable["data"]
					if databuffer then
						--dump(databuffer, "databuffer", 6)
						--返回结果
						succeed = databuffer["valid"]
						--提示文字
						local tips = datatable["msg"]
						if tips and tips ~= cjson.null then
							msg = tips
						end
						--获取信息
						if succeed == true then
							local uriinfo = databuffer["str1"]
							if (nil ~= uriinfo and "" ~= uriinfo) then
								cc.UserDefault:getInstance():setStringForKey("WEBURI_INFO", uriinfo)
								cc.UserDefault:getInstance():flush()
							end
						end
					end
				end
			end)
		else
			func(code)
			printf("没有一个网站可以使用，网站全挂")
		end
	end
	
	local webUrlInfo = cc.UserDefault:getInstance():getStringForKey("WEBURI_INFO")
	if (nil ~= webUrlInfo and "" ~= webUrlInfo) then
		local urlArray = luautil.split(webUrlInfo, "|")
		--获取最后一次正常的网站，并插入到检查列表中
		local lastWebUrl = cc.UserDefault:getInstance():getStringForKey("LAST_WEBURI")
		if (nil ~= lastWebUrl) then
			table.insert(urlArray, 1, lastWebUrl)
		end
		
		singleTaskCheckUrl(urlArray, callback)
	else
		--未使用过任何网站的缓存数据，使用预设网站
		singleTaskCheckUrl({BaseConfig.WEB_HTTP_URL}, callback)
	end
end

--服务器版本返回
function WelcomeScene:checkNewVersionCallBack(result,msg)
	local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")		
    --获取失败
    if not result then
	    self._txtTips:setString("")
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.QUERY_DIALOG_LAYER})
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {msg.."\n是否重试？",function(ok)
			if ok == true then
				self:checkNewVersion()
			else
				os.exit(0)
			end
		end, false, 2}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)			
    else
		--更新回调
		local function updateCallBack(needupdate)
			if (not needupdate) then
				printf("无需更新，进入登录，当前资源版本号：%d", self._newResVersion)
				if (appConfigProxy._appStoreSwitch == 0) then
					
					versionProxy:setResVersion(self._newResVersion)
				end
				--进入登录界面
				self._txtTips:setString("OK")
				self:setStatusTips(WelcomeScene.S_LOADED_RES)
				self:EnterClient()
			end
		end
		
	    --升级判断
	    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
		
		if cc.PLATFORM_OS_WINDOWS == targetPlatform then
			if WIN32_NEED_CHECK_UPDATE then
				self:updateClient(updateCallBack)
			else
				updateCallBack(false)
			end
		else
			if (appConfigProxy._appStoreSwitch == 0) then
				self:updateClient(updateCallBack)
			else
				updateCallBack(false)
			end
		end
    end
end


--升级大厅
function WelcomeScene:updateClient(callback)
	local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")	
	--当前网络资源版本号
	local curOnlineV = self._newVersion
	--当前包资源版本号
	local curPackageV = BaseConfig.BASE_C_VERSION
	--当前沙盒资源版本号
	local curSanBoxV = tonumber(versionProxy:getVersion()) or 0
	
	--网络资源版本号 大于 包资源版本号 更新APP
	if curOnlineV > curPackageV then
		if device.platform == "ios" and (type(self._iosUpdateUrl) ~= "string" or self._iosUpdateUrl == "") then
			print("ios update fail, url is nil or empty")
			callback(false)
			return
		else
			self._txtTips:setString("")
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.QUERY_DIALOG_LAYER})
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"有新的版本，是否现在下载升级？",function(ok)
				if ok == true then                    	
					self:upDateBaseApp()
					callback(true)
				else
					os.exit(0)
					callback(false)
				end			
			end, false, 2}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)
			return
		end			
	end
	
	--包资源版本号 大于 沙盒资源版本号 解压资源覆盖沙盒
	if curPackageV > curSanBoxV then
		local client_package = cc.FileUtils:getInstance():fullPathForFilename("client.zip")
		unZipAsync(client_package,device.writablePath,function(code, parm)
			if (code == appdf.UNZIP_COMPELETED) then
				local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
				--设置资源版本号
				versionProxy:setResVersion(BaseConfig.BASE_C_RESVERSION)
				--设置沙盒资源版本号
				versionProxy:setVersion(BaseConfig.BASE_C_VERSION)
				--设置需要重启标识
				local hotFixUpdateProxy = AppFacade:getInstance():retrieveProxy("HotFixUpdateProxy")
				local updateFiles = hotFixUpdateProxy:getUpdateFiles()
				table.insert(updateFiles, "needRestart")
				
				--网络资源版本号 大于 沙盒资源版本号
				if  self._newResVersion > versionProxy:getResVersion() then
					-- 更新配置
					local updateConfig = {}
					updateConfig.isClient = true
					updateConfig.newfileurl = appConfigProxy._updateUrl.."/client/res/filemd5List.json"
					updateConfig.downurl = appConfigProxy._updateUrl .. "/"
					updateConfig.dst = device.writablePath
					
					if cc.PLATFORM_OS_WINDOWS == targetPlatform then
						updateConfig.dst = device.writablePath .. "download/client/"
					end	
					
					updateConfig.src = device.writablePath.."client/res/filemd5List.json"
					table.insert(self.m_tabUpdateQueue, updateConfig)
					
					self:goUpdate()
					
					callback(true)
				else
					callback(false)
				end
			else
				printf("解压包资源失败，请检查")
				callback(false)
				return
			end
		end)
	else
		--网络资源版本号 大于 沙盒资源版本号
		if  self._newResVersion > versionProxy:getResVersion() then
			-- 更新配置
			local updateConfig = {}
			updateConfig.isClient = true
			updateConfig.newfileurl = appConfigProxy._updateUrl.."/client/res/filemd5List.json"
			updateConfig.downurl = appConfigProxy._updateUrl .. "/"
			updateConfig.dst = device.writablePath
			
			if cc.PLATFORM_OS_WINDOWS == targetPlatform then
				updateConfig.dst = device.writablePath .. "download/client/"
			end	
			
			updateConfig.src = device.writablePath.."client/res/filemd5List.json"
			table.insert(self.m_tabUpdateQueue, updateConfig)
			
			self:goUpdate()
			
			callback(true)
		else
			callback(false)
		end
	end
end

function WelcomeScene:upDateBaseApp()
	local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")		
	
	self.m_spTotalBg:setVisible(false)
	self.m_spFileBg:setVisible(true)

	if device.platform == "android" then
		local argsJson 
		local url = ""
		print("debug .. => " .. DEBUG)
		local dst_package_name = nil
		
		if (BaseConfig.AGENT_IDENTIFICATION == nil or BaseConfig.AGENT_IDENTIFICATION == "") then
			if isDebug() then
				if (nil ~= appConfigProxy._updatePackage and "" ~= appConfigProxy._updatePackage) then
					dst_package_name = appConfigProxy._updatePackage .. "_debug.apk"
					url = appConfigProxy._updateUrl.."/" .. dst_package_name
				else
					dst_package_name = "game_debug.apk"
					url = appConfigProxy._updateUrl.."/" ..dst_package_name
				end
			else	
				if (nil ~= appConfigProxy._updatePackage and "" ~= appConfigProxy._updatePackage) then
					dst_package_name = appConfigProxy._updatePackage .. ".apk"
					url = appConfigProxy._updateUrl.."/" .. dst_package_name
				else
					dst_package_name = "game.apk"
					url = appConfigProxy._updateUrl.."/" ..dst_package_name
				end 
			end
		else
			if isDebug() then
				dst_package_name = BaseConfig.AGENT_IDENTIFICATION .. "_debug.apk"
				url = appConfigProxy._updateUrl.."/" .. dst_package_name
			else	
				dst_package_name = BaseConfig.AGENT_IDENTIFICATION .. ".apk"
				url = appConfigProxy._updateUrl.."/" .. dst_package_name
			end
		end

	    --调用C++下载
	    local luaj = require "cocos.cocos2d.luaj"
		local className = "org/cocos2dx/lua/AppActivity"

	    local sigs = "()Ljava/lang/String;"
   		local ok,ret = luaj.callStaticMethod(className,"getSDCardDocPath",{},sigs)
   		if ok then
   			local dstpath = ret .. "/update/"
   			local filepath = dstpath .. dst_package_name
		    if cc.FileUtils:getInstance():isFileExist(filepath) then
		    	cc.FileUtils:getInstance():removeFile(filepath)
		    end
		    if false == cc.FileUtils:getInstance():isDirectoryExist(dstpath) then
		    	cc.FileUtils:getInstance():createDirectory(dstpath)
		    end
		    self:updateBar(self.m_fileBar, self.m_fileThumb, 0)
			
			downFileAsync(url,dstpath, nil, function(code,parm)
					--下载回调
					if code == appdf.DOWN_PRO_INFO then --进度信息
						self:updateBar(self.m_fileBar, self.m_fileThumb, tonumber(parm))
					elseif code == appdf.DOWN_COMPELETED then --下载完毕
						self._txtTips:setString("下载完成")
						self.m_spTotalBg:setVisible(false)
						self.m_spFileBg:setVisible(false)
						--安装apk						
						local args = {filepath}
						sigs = "(Ljava/lang/String;)V"
		   				ok,ret = luaj.callStaticMethod(className, "installClient",args, sigs)
		   				if ok then
		   					os.exit(0)
		   				end
					else
						AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.QUERY_DIALOG_LAYER})
						AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"下载失败," .. "\n是否重试？",function(ok)
							if ok == true then                    	
								self:upDateBaseApp()				    
							else
								os.exit(0)
							end			
						end, false, 2}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)		
					end
				end, 5)
		else
			os.exit(0)
   		end	    
	elseif device.platform == "ios" then
		local luaoc = require "cocos.cocos2d.luaoc"
		local ok,ret  = luaoc.callStaticMethod("AppController","updateBaseClient",{url = self._iosUpdateUrl})
		if ok then
			os.exit(0)
		else
	        print("luaoc error:" .. ret)        
	    end
	elseif device.platform == "windows" then
		
		local url = ""
	
		if (BaseConfig.AGENT_IDENTIFICATION == nil or BaseConfig.AGENT_IDENTIFICATION == "") then
			if isDebug() then
				if (nil ~= appConfigProxy._updatePackage and "" ~= appConfigProxy._updatePackage) then
					dst_package_name = appConfigProxy._updatePackage .. "_debug.apk"
					url = appConfigProxy._updateUrl.."/" .. dst_package_name
				else
					dst_package_name = "game_debug.apk"
					url = appConfigProxy._updateUrl.."/" ..dst_package_name
				end
			else	
				if (nil ~= appConfigProxy._updatePackage and "" ~= appConfigProxy._updatePackage) then
					dst_package_name = appConfigProxy._updatePackage .. ".apk"
					url = appConfigProxy._updateUrl.."/" .. dst_package_name
				else
					dst_package_name = "game.apk"
					url = appConfigProxy._updateUrl.."/" ..dst_package_name
				end 
			end
		else
			if isDebug() then
				dst_package_name = BaseConfig.AGENT_IDENTIFICATION .. "_debug.apk"
				url = appConfigProxy._updateUrl.."/" .. dst_package_name
			else	
				dst_package_name = BaseConfig.AGENT_IDENTIFICATION .. ".apk"
				url = appConfigProxy._updateUrl.."/" .. dst_package_name
			end
		end
		
		downFileAsync(url,cc.FileUtils:getInstance():getWritablePath(), nil, function(code,parm)
				--下载回调
				if code == appdf.DOWN_PRO_INFO then --进度信息
					self:updateBar(self.m_fileBar, self.m_fileThumb, tonumber(parm))
				elseif code == appdf.DOWN_COMPELETED then --下载完毕
					self._txtTips:setString("下载完成")
					self.m_spTotalBg:setVisible(false)
					self.m_spFileBg:setVisible(false)
				else
					print("下载失败,code:"..code)
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.QUERY_DIALOG_LAYER})
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"下载失败," .. "\n是否重试？",function(ok)
						if ok == true then                    	
							self:upDateBaseApp()				    
						else
							os.exit(0)
						end			
					end, false, 2}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)		
				end
			end, 5)
	end
end

--开始下载
function WelcomeScene:goUpdate( )
	self.m_spTotalBg:setVisible(true)
	self.m_spFileBg:setVisible(true)

	local config = self.m_tabUpdateQueue[1]
	if nil == config then
		self.m_spTotalBg:setVisible(false)
		self.m_spFileBg:setVisible(false)
		self._txtTips:setString("OK")
		self:EnterClient()
	else
		self.clientUpdate =	ClientUpdate:create(config.newfileurl, config.dst, config.src, config.downurl)
		self.clientUpdate:upDateClient(handler(self, self.updateProgress), handler(self, self.updateResult))
	end	
end

--下载进度
function WelcomeScene:updateProgress(sub, msg, mainpersent)
	if (sub ~= -1) then
		self:updateBar(self.m_fileBar, self.m_fileThumb, sub)
	end
	
	if (mainpersent ~= -1) then
		self:updateBar(self.m_totalBar, self.m_totalThumb, mainpersent)
	end
end

--下载结果
function WelcomeScene:updateResult(result,msg)
	local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")			
	
	if result == true then
		self:updateBar(self.m_fileBar, self.m_fileThumb, 0)
		self:updateBar(self.m_totalBar, self.m_totalThumb, 0)

		local config = self.m_tabUpdateQueue[1]
		if nil ~= config then
			if true == config.isClient then
				--设置沙盒资源版本号
				versionProxy:setResVersion(self._newResVersion)	
			else
				versionProxy:setResVersion(config._ServerResVersion, config._KindID)
				for k,v in pairs(appConfigProxy._gameList) do
					if v._KindID == config._KindID then
						v._Active = true
					end
				end
			end
			table.remove(self.m_tabUpdateQueue, 1)
			self:goUpdate()
		else
			--进入登录界面
			self._txtTips:setString("OK")
			self:EnterClient()
		end
	else
		self.m_spTotalBg:setVisible(false)
		self.m_spFileBg:setVisible(false)
		self:updateBar(self.m_fileBar, self.m_fileThumb, 0)
		self:updateBar(self.m_totalBar, self.m_totalThumb, 0)

		--重试询问
		self._txtTips:setString("")

		local function actionCallBack(bRetry)
			if bRetry == true then
				self.m_spTotalBg:setVisible(true)
				self.m_spFileBg:setVisible(true)				
				self.clientUpdate:UpdateFile()
			else
				os.exit(0)
			end
		end
		
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {msg.."\n是否重试？", actionCallBack, false, 2}}, VIEW_LIST.QUERY_DIALOG_LAYER)
	end
end

function WelcomeScene:updateBar(bar, thumb, percent)
	if nil == bar or nil == thumb then
		return
	end
	local text_tip = bar:getChildByName("text_tip")
	if nil ~= text_tip then
		local str = string.format("%d%%", percent)
		text_tip:setString(str)
	end

	bar:setPercent(percent)
	local size = bar:getVirtualRendererSize()
	thumb:setPositionX(size.width * percent / 100)
end

return WelcomeScene