--[[
 	下载更新
 		2016_04_27 C.P
 	功能：下载替换文件资源
]]--
local ClientUpdate = class("ClientUpdate")

-- url:下载list地址 
-- wirtepath:保存路径
-- curfile:当前list路径
function ClientUpdate:ctor(newfileurl,savepath,curfile,downurl)
	self._newfileurl = newfileurl
	self._savepath = savepath
	self._curfile = curfile
	self._downUrl = downurl
	print("curfile:"..curfile)
end

--开始更新 
--listener回调目标
--listener:updateProgress(sub,msg)
--listener:updateResult(result,msg)
function ClientUpdate:upDateClient(updateProcess, updateResult)
	--监听
	self._updateProgress = updateProcess
	self._updateResult = updateResult
	
	self._downList={}			--下载列表

	--创建文件夹
	if not createDirectory(self._savepath) then
		self._updateResult(false,"创建文件夹失败！")
		return
	end

	--获取当前文件列表
	local szCurHotFixData = cc.FileUtils:getInstance():getStringFromFile(self._curfile)
	local oldlist = {}
	if  szCurHotFixData ~= nil and #szCurHotFixData > 0 then
		local curMD5List = cjson.decode(szCurHotFixData)
		oldlist = curMD5List["listdata"] or {}
	end
	
	local hotFixUpdateProxy = AppFacade:getInstance():retrieveProxy("HotFixUpdateProxy")
	local updateFiles = hotFixUpdateProxy:getUpdateFiles()

	appdf.onHttpJsionTable(self._newfileurl,"GET","",function(jsondata,response)
		--记录新的list
		local fileUtil = cc.FileUtils:getInstance()
		self._response = response
		if jsondata then
			local newlist = jsondata["listdata"]
			if nil == newlist and nil ~= self._listener.updateResult then
				self._updateResult(false,"获取服务器列表失败！")
				return
			end
				--删除判断
			for k,v in pairs(oldlist) do
				local oldpath = v["path"]
				local oldname = v["name"]
				if  oldpath then
					local bdel = true
					for newk,newv in pairs(newlist) do
						if oldpath == newv["path"] and oldname == newv["name"] then
							bdel = false
							break
						end
					end
					--删除文件
					if bdel == true then
						print("removefile:"..self._savepath..oldpath..oldname)
						fileUtil:removeFile(self._savepath..oldpath..oldname)
					end
				end
			end
			--下载判断
			for k ,v in pairs(newlist) do
				local newpath = v["path"]
				if newpath then
					local needupdate = true
					local newname = v["name"]
					local newmd5 = v["md5"]
					for oldk,oldv in pairs(oldlist) do
						local oldpath = oldv["path"]
						local oldname = oldv["name"]
						local oldmd5 = oldv["md5"]
						
						if oldpath == newpath and newname == oldname then 
							if newmd5 == oldmd5 then
								needupdate = false
							end
							break
						end
					end
					--保存到下载列队
					if needupdate == true then
						print(newname.."-needupdate")
						table.insert(self._downList , {newpath,newname})
						
						--记录更新的Lua文件
						local ext = appdf.getextension(newname)
						if (ext) then
							if (ext == "luac" or ext == "lua") then
								local luarefpath = string.gsub(newpath, "/", ".")
								table.insert(updateFiles, luarefpath .. appdf.stripextension(newname))
							end
						end
					end
				end
				
			end
			print("update_count:"..#self._downList)
			--开始下载
			if (#self._downList == 0) then
				cc.FileUtils:getInstance():writeStringToFile(self._response,self._curfile)
				self._updateResult(true,"")
			else
				self:UpdateFile()
			end
		end
	end)

end

--下载
function ClientUpdate:UpdateFile()
	if not self._downList  then 
		self._updateResult(false,"下载信息损坏！")
		return
	end 
	
	local fullurl = ""
	local filename = ""
	local dstpath = ""
	local delm = "{0}"
	
	--拼接要下载的所有文件
	for _,v in pairs(self._downList) do
		fullurl = fullurl .. (self._downUrl..v[1]..v[2] .. "{0}")
		dstpath = dstpath .. (self._savepath..v[1] .. "{0}")
	end
	
	--下载多文件  适合热更新散文件使用
	local failedDown = {}
--[[	fullurl = "http://www.765qp.com/download/MBzy/debug//client/res/Shop/rmb_score.fnt"
	dstpath = "E:/ouyery/RY/YundingRy/client/client/res/Shop/"--]]
	downMulFileAsync(fullurl,dstpath,function (code, parm)
		--下载回调
		if code == appdf.DOWN_PRO_INFO then --进度信息
			self._updateProgress(-1,"", tonumber(parm))
		elseif code == appdf.DOWN_PRO_SUB_INFO then
			self._updateProgress(tonumber(parm),"", -1)
		elseif code == appdf.DOWN_COMPELETED then --下载完毕
			--cc.FileUtils:getInstance():writeStringToFile(self._response,self._curfile)
			if (#failedDown ~= 0) then
				for _, v in pairs(failedDown) do
					showToast(cc.Director:getInstance():getRunningScene(), "下载" .. v .. "失败！", 1)
				end
				self._updateResult(false,"下载" .. parm .. "失败,请重试！")
				--self._updateResult(false,"下载" .. "失败,请重试！")
				--end
			else
				--更新本地热更新比较文件
				cc.FileUtils:getInstance():writeStringToFile(self._response,self._curfile)
				--更新UI
				self._updateProgress(100,"", 100)
				--通知完成
				self._updateResult(true,"")
			end
		elseif code == appdf.DOWN_STOPED then --下载中断
			--print("下载中断,code:" .. code .. " ## parm:" .. parm) --失败信息
		else	
			table.insert(failedDown, parm)
		end			
	end, 1)	
end

--下载
function ClientUpdate:UpdateFullFile()
	if not self._downList  then 
		self._updateResult(false,"下载信息损坏！")
		return
	end

	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")

	local url = appConfigProxy._updateUrl
	local saveurl = device.writablePath
	
	downFileAsync(url .. "client.zip", saveurl, nil, function (code, parm)
		--下载回调
		if code == appdf.DOWN_PRO_INFO then --进度信息
			self._updateProgress(100,"", tonumber(parm))
		elseif code == appdf.DOWN_COMPELETED then --下载完毕
			--更新UI
			self._updateProgress(100,"", 100)
			
			unZipAsync(saveurl .. "client.zip",saveurl,function(code, parm)
				if (code == appdf.UNZIP_COMPELETED) then
					--通知完成
					self._updateResult(true,"")
					--更新本地热更新比较文件
					cc.FileUtils:getInstance():writeStringToFile(self._response,self._curfile)					
				else
					self._updateResult(false,"下载" .. "失败,请重试！")
				end
			end)				
		else	
			self._updateResult(false,"下载" .. "失败,请重试！")
		end			
	end, 1)	
end

return ClientUpdate