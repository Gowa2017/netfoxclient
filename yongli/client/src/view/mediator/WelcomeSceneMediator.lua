local BaseMediator = import(".BaseMediator")
local WelcomeSceneMediator = class("WelcomeSceneMediator", BaseMediator)

--����Э��˳����ÿ��url(��Ƚ����������ױ�¶��վ��ַ)����˳����ѡһ�����õ�url
local function singleTaskCheckUrl(urllist, callback)
	assert("table" == type(urllist), "expected table, but not now")
	if ("table" ~= type(urllist)) then
		return
	end
	
	assert("function" == type(callback), "expected function, but not now")
	if ("function" ~= type(callback)) then
		return
	end
	
	--ͨ��Э�̱������url������
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
					else--����ɹ����������ݴ����쳣
						printf("ConfigProxy got success, but data failed! url:%s\n", url)
					end
				else--����ʧ��
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
					--û����� ����cpu���ڣ���������Ȩ
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

--���Э�̲��д���ÿ��url(��ȽϿ죬���ױ�¶��վ��ַ),���������Ӧ��url
local function mutiTaskCheckUrl(urllist, callback)
	assert("table" == type(urllist), "expected table, but not now")
	if ("table" ~= type(urllist)) then
		return
	end
	
	assert("function" == type(callback), "expected function, but not now")
	if ("function" ~= type(callback)) then
		return
	end
	
	--ͨ��Э�̺���
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
				else--����ɹ����������ݴ����쳣
					printf("ConfigProxy got success, but data failed! url:%s\n", url)
				end
			else--����ʧ��
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
    --��mediator���ĵ���Ϣ
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
	
	--ע��UI�����¼�
	self.viewComponent:attach("V2M_CheckVersion", handler(self, self.onCheckVersion))
	self.viewComponent:attach("V2M_UpdatePackage", handler(self, self.onUpdatePackage))
	
	
	--self:sendNotification(GAME_COMMAMD.POP_VIEW)
end

function WelcomeSceneMediator:onCheckVersion()
	local versionProxy = self.facade:retrieveProxy("VersionProxy")
	local nResversion = tonumber(versionProxy:getResVersion())
	if nil == nResversion then
		--��ѹ���Դ�����Ϸ��Դ
		self:onUnZipBase()        
	else
		--�����վ�Ƿ����
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

	    --����C++����
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
					--���ػص�
					if code == appdf.DOWN_PRO_INFO then --������Ϣ
						self:updateBar(self.m_fileBar, self.m_fileThumb, tonumber(parm))
					elseif code == appdf.DOWN_COMPELETED then --�������
						self._txtTips:setString("�������")
						self.m_progressLayer:setVisible(false)

						--��װapk						
						local args = {filepath}
						sigs = "(Ljava/lang/String;)V"
		   				ok,ret = luaj.callStaticMethod(className, "installClient",args, sigs)
		   				if ok then
		   					os.exit(0)
		   				end
					else
						print("����ʧ��,code:"..code)
						QueryDialog:create("����ʧ��," .. "\n�Ƿ����ԣ�",function(bReTry)
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
				--���ػص�
				if code == appdf.DOWN_PRO_INFO then --������Ϣ
					self:updateBar(self.m_fileBar, self.m_fileThumb, tonumber(parm))
				elseif code == appdf.DOWN_COMPELETED then --�������
					self._txtTips:setString("�������")
					self.m_progressLayer:setVisible(false)
				else
					print("����ʧ��,code:"..code)
					QueryDialog:create("����ʧ��," .. "\n�Ƿ����ԣ�",function(bReTry)
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
		--��ѹ��Դ
		local view = self:getViewComponent()
		view:emit("M2V_UpdateCenterTipStatues", "S_UNZIP_RES")
		view:emit("M2V_UpdateTipStatues", "��ѹ�ļ������Ժ�...")
		
		local dst = device.writablePath
		unZipAsync(cc.FileUtils:getInstance():fullPathForFilename("client.zip"),dst,function(result)
				coroutine.resume(con_unzip)
			end)
		coroutine.yield()
		unZipAsync(cc.FileUtils:getInstance():fullPathForFilename("game.zip"),dst,function(result)
				coroutine.resume(con_unzip)
			end)
		coroutine.yield()

		view:emit("M2V_UpdateTipStatues", "��ѹ��ɣ�")

		--��ʼ������Ӧ�ð汾��
		versionProxy:setVersion(BaseConfig.BASE_C_VERSION, 0)
		--��ʼ�������İ汾��
		versionProxy:setResVersion(BaseConfig.BASE_C_RESVERSION, 0)
		--��ʼ����Ϸ�汾��
		for k ,v in pairs(appdf.BASE_GAME) do
			versionProxy:setResVersion(v.version,v.kind)
		end
		
		--�����վ�Ƿ����
		self:checkWebUrlInfo(function ()
			self:httpGetVersion()
		end)
    end)
	coroutine.resume(con_unzip)
end

--ͬ���汾
function WelcomeSceneMediator:httpGetVersion()
	local view = self:getViewComponent()
	view:emit("M2V_UpdateTipStatues", "��ȡ��������Ϣ...")
	view:emit("M2V_UpdateCenterTipStatues", "S_CONNECT_SERVER")
	
	--��ȡ����
	local appConfigProxy = self.facade:retrieveProxy("AppConfigProxy")
	local versionProxy = self.facade:retrieveProxy("VersionProxy")
	
	--���ݽ���
	local vcallback = function(datatable)
	 	local succeed = false
	 	local msg = "�����ȡʧ�ܣ�"
	 	if type(datatable) == "table" then	 		
            local databuffer = datatable["data"]
            if databuffer then
            	--dump(databuffer, "databuffer", 6)
                --���ؽ��
	 		    succeed = databuffer["valid"]
	 		    --��ʾ����
	 		    local tips = datatable["msg"]
	 		    if tips and tips ~= cjson.null then
	 			    msg = tips
	 		    end
	 		    --��ȡ��Ϣ
	 		    if succeed == true then
					view:emit("M2V_UpdateTipStatues", "OK")
					appConfigProxy._serverConfig = databuffer		     
 				    --�ȸ������ص�ַ
					appConfigProxy._updateUrl = databuffer["downloadurl"]						--test zhong "http://172.16.4.140/download/"
 				    --ios appstore ������·��
					if true == BaseConfig.APPSTORE_VERSION then
 				    	appConfigProxy._updateUrl = appConfigProxy._updateUrl .. "/appstore/"
						appConfigProxy._appStoreSwitch = tonumber(databuffer["appstore"])
 				    end
					
					appConfigProxy._updatePackage = databuffer["packagename"]
					
 				    --��Ϸ�б�
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
 					    --��鱾���ļ��Ƿ����
 					    local path = device.writablePath .. "game/" .. gameinfo._Module
 					    gameinfo._Active = cc.FileUtils:getInstance():isDirectoryExist(path)
 					    local e = string.find(gameinfo._KindName, "[.]")
 					    if e then
 					    	gameinfo._Type = string.sub(gameinfo._KindName,1,e - 1)
 					    end
 					    -- ����
 					    gameinfo._SortId = tonumber(v["SortID"]) or 0

 					    table.insert(appConfigProxy._gameList, gameinfo)
 				    end

 				    table.sort( appConfigProxy._gameList, function(a, b)
 				    	return a._SortId > b._SortId
 				    end)					
					
					
 				    --�����汾
 				    local remoteAppVersion = tonumber(databuffer["clientversion"])          						--test zhong  0
 				    --������Դ�汾
 				    local remoteResVersion = tonumber(databuffer["resversion"])
 				    --ƻ������������ת��ҳ��ַ
 				    local iosUpdateUrl = databuffer["ios_url"]

					--�ж�Ӧ�ð汾��
					local curAppVersion = versionProxy:getVersion()
					if remoteAppVersion then
						if remoteAppVersion > curAppVersion then
							-- ��������
							local config = {}
							config.remoteFile = appConfigProxy._updateUrl.."/client.apk"
							config.iosUpdateUrl = iosUpdateUrl
							view:emit("M2V_UpdateCenterTipStatues", "S_UPDATE_PACKAGE_RES")
							view:emit("M2V_NeedUpdatePackage", config)
							return
						end
					end
					--�ж���Դ�汾��
					local curResVersion = versionProxy:getResVersion()
					if remoteResVersion then
						if remoteResVersion > curResVersion then
							-- ��������
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
					
					--û�и��� ֱ�ӽ��� ��һ������
					--���ô�������Ϸ
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
					view:emit("M2V_CheckVersionResult", false, "�����쳣")	
	 		    end
			else
				view:emit("M2V_UpdateTipStatues", "FAILED")
				view:emit("M2V_CheckVersionResult", false, "�����쳣")	
            end
		else
			view:emit("M2V_UpdateTipStatues", "FAILED")
			view:emit("M2V_CheckVersionResult", false, "�����쳣")	
	 	end
		
		
	end

	appdf.onHttpJsionTable(BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx","get","action=getgamelist",vcallback)
end

--�����վ��Ч�ԣ���Ч�Զ��л���������վ��ַ
function WelcomeSceneMediator:checkWebUrlInfo(func)
	assert(type(func) == "function", "expected function, but not now")
	if (type(func) ~= "function") then
		return
	end
	
	local function callback(code, url)
		if (code == 0) then
			--������վȫ�ֱ���
			BaseConfig.WEB_HTTP_URL = url
			--��¼���һ�ο��õ���վ
			cc.UserDefault:getInstance():setStringForKey("LAST_WEBURI", url)
			cc.UserDefault:getInstance():flush()

			func()
			
			appdf.onHttpJsionTable(url .. "/WS/MobileInterface.ashx","get","action=getserverconfig",function(datatable, resp)
				local succeed = false
				local msg = "�����ȡʧ�ܣ�"
				if type(datatable) == "table" then	 		
					local databuffer = datatable["data"]
					if databuffer then
						--dump(databuffer, "databuffer", 6)
						--���ؽ��
						succeed = databuffer["valid"]
						--��ʾ����
						local tips = datatable["msg"]
						if tips and tips ~= cjson.null then
							msg = tips
						end
						--��ȡ��Ϣ
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
			printf("û��һ����վ����ʹ�ã���վȫ��")
		end
	end
	
	local webUrlInfo = cc.UserDefault:getInstance():getStringForKey("WEBURI_INFO")
	if (nil ~= webUrlInfo and "" ~= webUrlInfo) then
		local urlArray = string.split(webUrlInfo, "|")
		--��ȡ���һ����������վ�������뵽����б���
		local lastWebUrl = cc.UserDefault:getInstance():getStringForKey("LAST_WEBURI")
		if (nil ~= lastWebUrl) then
			table.insert(urlArray, 1, lastWebUrl)
		end
		
		singleTaskCheckUrl(urlArray, callback)
	else
		--δʹ�ù��κ���վ�Ļ������ݣ�ʹ��Ԥ����վ
		singleTaskCheckUrl({BaseConfig.WEB_HTTP_URL}, callback)
	end
end

return WelcomeSceneMediator