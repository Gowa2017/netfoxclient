local BaseMediator = import(".BaseMediator")
local WelcomeSceneMediator = class("WelcomeSceneMediator", BaseMediator)

--单个协程顺序处理每个url(会比较慢，不容易暴露网站地址)，按顺序挑选一个可用的url
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

--多个协程并行处理每个url(会比较快，容易暴露网站地址),返回最快响应的url
local function mutiTaskCheckUrl(urllist, callback)
	assert("table" == type(urllist), "expected table, but not now")
	if ("table" ~= type(urllist)) then
		return
	end
	
	assert("function" == type(callback), "expected function, but not now")
	if ("function" ~= type(callback)) then
		return
	end
	
	--通过协程函数
	local url = ""
	local isCallSuccess = false
	local retCount = 0
	local function checkUrl(conhandle, url)
		local corn = conhandle
		local function onCheckFinishCallBack(datatable)
			if type(datatable) == "table" then
				local code = datatable["code"]
				if (code == 0) then
					finish = true
					coroutine.resume(corn)
				else--请求成功，但是数据存在异常
					printf("ConfigProxy got success, but data failed! url:%s\n", url)
				end
			else--请求失败
				printf("ConfigProxy got an error! url:%s\n", url)
			end
			
			coroutine.resume(corn)
		end

		appdf.onHttpJsionTable(url .. "/index.txt","get","",onCheckFinishCallBack)
		
		coroutine.yield()

		if (isCallSuccess) then
			return
		end
		
		if finish then
			isCallSuccess = true
			callback(0, url)
			return
		else
			if not finish then
				retCount = retCount + 1
				if (retCount == #urllist) then
					callback(-1, "")
				end
				return
			end
		end
    end
	
	-- start check
	for _, v in pairs(urllist) do
		local conhandle = nil
		conhandle = coroutine.create(checkUrl, conhandle, v)
		coroutine.resume(conhandle, conhandle, v)    
	end
end

function WelcomeSceneMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --该mediator关心的消息
    return	{
			}
end

function WelcomeSceneMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)
    local name = notification:getName()
    local data = notification:getBody()
    local type = notification:getType()
    --deal something
end

function WelcomeSceneMediator:onRegister()
    self.super.onRegister(self)
	
	--注册UI监听事件
	self.viewComponent:attach("V2M_CheckVersion", handler(self, self.onCheckVersion))
	self.viewComponent:attach("V2M_UpdatePackage", handler(self, self.onUpdatePackage))
	
	
	--self:sendNotification(GAME_COMMAMD.POP_VIEW)
end

function WelcomeSceneMediator:onCheckVersion()
	local versionProxy = self.facade:retrieveProxy("VersionProxy")
	local nResversion = tonumber(versionProxy:getResVersion())
	if nil == nResversion then
		--解压包自带的游戏资源
		self:onUnZipBase()        
	else
		--检查网站是否可用
		self:checkWebUrlInfo(function ()
			self:httpGetVersion()
		end)
	end
end

function WelcomeSceneMediator:onUpdatePackage()
	
	local appConfigProxy = self.facade:retrieveProxy("AppConfigProxy")
	
	if device.platform == "android" then
		local this = self
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
						self.m_progressLayer:setVisible(false)

						--安装apk						
						local args = {filepath}
						sigs = "(Ljava/lang/String;)V"
		   				ok,ret = luaj.callStaticMethod(className, "installClient",args, sigs)
		   				if ok then
		   					os.exit(0)
		   				end
					else
						print("下载失败,code:"..code)
						QueryDialog:create("下载失败," .. "\n是否重试？",function(bReTry)
							if bReTry == true then
								this:upDateBaseApp()
							else
								os.exit(0)
							end
						end)
						:setCanTouchOutside(false)
						:addTo(self)
					end
				end, 5)
		else
			os.exit(0)
   		end	    
	elseif device.platform == "ios" then
		local luaoc = require "cocos.cocos2d.luaoc"
		local ok,ret  = luaoc.callStaticMethod("AppController","updateBaseClient",{url = self._iosUpdateUrl})
	    if not ok then
	        print("luaoc error:" .. ret)        
	    end
	elseif device.platform == "windows" then
		
		local url = appConfigProxy._updateUrl.."/kygame.apk"	
		
		downFileAsync(url,"c:\\", nil, function(code,parm)
				--下载回调
				if code == appdf.DOWN_PRO_INFO then --进度信息
					self:updateBar(self.m_fileBar, self.m_fileThumb, tonumber(parm))
				elseif code == appdf.DOWN_COMPELETED then --下载完毕
					self._txtTips:setString("下载完成")
					self.m_progressLayer:setVisible(false)
				else
					print("下载失败,code:"..code)
					QueryDialog:create("下载失败," .. "\n是否重试？",function(bReTry)
						if bReTry == true then
							this:upDateBaseApp()
						else
							os.exit(0)
						end
					end)
					:setCanTouchOutside(false)
					:addTo(self)
				end
			end, 5)
	end
end

function WelcomeSceneMediator:onUnZipBase()
	local dst = device.writablePath
	local versionProxy = self.facade:retrieveProxy("VersionProxy")
	local con_unzip = nil
	con_unzip = coroutine.create(function()
		--解压资源
		local view = self:getViewComponent()
		view:emit("M2V_UpdateCenterTipStatues", "S_UNZIP_RES")
		view:emit("M2V_UpdateTipStatues", "解压文件，请稍候...")
		
		local dst = device.writablePath
		unZipAsync(cc.FileUtils:getInstance():fullPathForFilename("client.zip"),dst,function(result)
				coroutine.resume(con_unzip)
			end)
		coroutine.yield()
		unZipAsync(cc.FileUtils:getInstance():fullPathForFilename("game.zip"),dst,function(result)
				coroutine.resume(con_unzip)
			end)
		coroutine.yield()

		view:emit("M2V_UpdateTipStatues", "解压完成！")

		--初始化大厅应用版本号
		versionProxy:setVersion(BaseConfig.BASE_C_VERSION, 0)
		--初始化大厅的版本号
		versionProxy:setResVersion(BaseConfig.BASE_C_RESVERSION, 0)
		--初始化游戏版本号
		for k ,v in pairs(appdf.BASE_GAME) do
			versionProxy:setResVersion(v.version,v.kind)
		end
		
		--检查网站是否可用
		self:checkWebUrlInfo(function ()
			self:httpGetVersion()
		end)
    end)
	coroutine.resume(con_unzip)
end

--同步版本
function WelcomeSceneMediator:httpGetVersion()
	local view = self:getViewComponent()
	view:emit("M2V_UpdateTipStatues", "获取服务器信息...")
	view:emit("M2V_UpdateCenterTipStatues", "S_CONNECT_SERVER")
	
	--获取数据
	local appConfigProxy = self.facade:retrieveProxy("AppConfigProxy")
	local versionProxy = self.facade:retrieveProxy("VersionProxy")
	
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
	 		    end
	 		    --获取信息
	 		    if succeed == true then
					view:emit("M2V_UpdateTipStatues", "OK")
					appConfigProxy._serverConfig = databuffer		     
 				    --热更新下载地址
					appConfigProxy._updateUrl = databuffer["downloadurl"]						--test zhong "http://172.16.4.140/download/"
 				    --ios appstore 版下载路径
					if true == BaseConfig.APPSTORE_VERSION then
 				    	appConfigProxy._updateUrl = appConfigProxy._updateUrl .. "/appstore/"
						appConfigProxy._appStoreSwitch = tonumber(databuffer["appstore"])
 				    end
					
					appConfigProxy._updatePackage = databuffer["packagename"]
					
 				    --游戏列表
 				    local rows = databuffer["gamelist"]
 				    appConfigProxy._gameList = {}
 				    for k,v in pairs(rows) do
 					    local gameinfo = {}
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

 				    table.sort( appConfigProxy._gameList, function(a, b)
 				    	return a._SortId > b._SortId
 				    end)					
					
					
 				    --大厅版本
 				    local remoteAppVersion = tonumber(databuffer["clientversion"])          						--test zhong  0
 				    --大厅资源版本
 				    local remoteResVersion = tonumber(databuffer["resversion"])
 				    --苹果大厅更新跳转网页地址
 				    local iosUpdateUrl = databuffer["ios_url"]

					--判断应用版本号
					local curAppVersion = versionProxy:getVersion()
					if remoteAppVersion then
						if remoteAppVersion > curAppVersion then
							-- 更新配置
							local config = {}
							config.remoteFile = appConfigProxy._updateUrl.."/client.apk"
							config.iosUpdateUrl = iosUpdateUrl
							view:emit("M2V_UpdateCenterTipStatues", "S_UPDATE_PACKAGE_RES")
							view:emit("M2V_NeedUpdatePackage", config)
							return
						end
					end
					--判断资源版本号
					local curResVersion = versionProxy:getResVersion()
					if remoteResVersion then
						if remoteResVersion > curResVersion then
							-- 更新配置
		 				    local config = {}
							
					 		config.newfileurl = appConfigProxy._updateUrl.."/client/res/filemd5List.json"
							config.downurl = appConfigProxy._updateUrl .. "/"
							
							local targetPlatform = cc.Application:getInstance():getTargetPlatform()
							if cc.PLATFORM_OS_WINDOWS == targetPlatform then
								config.dst = device.writablePath .. "download/client/"
							else
								config.dst = device.writablePath
							end
							
							config.src = device.writablePath.."client/res/filemd5List.json"
							view:emit("M2V_UpdateCenterTipStatues", "S_UPDATE_HOTFIX_RES")
							view:emit("M2V_NeedHotFix", config)
							return
						end
					end
					
					--没有更新 直接进入 下一个场景
					--重置大厅与游戏
					for k ,v in pairs(package.loaded) do
						if k ~= nil then 
							if type(k) == "string" then
								if string.find(k,"plaza.") ~= nil or string.find(k,"game.") ~= nil then
									print("package kill:"..k) 
									package.loaded[k] = nil
								end
							end
						end
					end	
					self:sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.LOGON)
				else
					view:emit("M2V_UpdateTipStatues", "FAILED")
					view:emit("M2V_CheckVersionResult", false, "数据异常")	
	 		    end
			else
				view:emit("M2V_UpdateTipStatues", "FAILED")
				view:emit("M2V_CheckVersionResult", false, "数据异常")	
            end
		else
			view:emit("M2V_UpdateTipStatues", "FAILED")
			view:emit("M2V_CheckVersionResult", false, "数据异常")	
	 	end
		
		
	end

	appdf.onHttpJsionTable(BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx","get","action=getgamelist",vcallback)
end

--检查网站有效性，无效自动切换，更新网站地址
function WelcomeSceneMediator:checkWebUrlInfo(func)
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

			func()
			
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
			printf("没有一个网站可以使用，网站全挂")
		end
	end
	
	local webUrlInfo = cc.UserDefault:getInstance():getStringForKey("WEBURI_INFO")
	if (nil ~= webUrlInfo and "" ~= webUrlInfo) then
		local urlArray = string.split(webUrlInfo, "|")
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

return WelcomeSceneMediator