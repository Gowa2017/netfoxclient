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
function ClientUpdate:upDateClient(listener)

	--监听
	self._listener = listener
	self._downList={}			--下载列表

	local this = self
	local result = false
	local msg = nil

	while(not result)
	do
		-- --清空临时文件夹
		-- if not removeDirectory(self._savepath) then
		-- 	msg = "删除临时文件夹失败！"
		-- 	break
		-- end

		--创建文件夹
		if not createDirectory(self._savepath) then
			msg = "创建文件夹失败！"
			break
		end

		--获取当前文件列表
		local szCurList = cc.FileUtils:getInstance():getStringFromFile(self._curfile)
		local oldlist = {}
		if  szCurList ~= nil and #szCurList > 0 then
			local curMD5List = cjson.decode(szCurList)
			oldlist = curMD5List["listdata"]
		end

		if not oldlist then
			oldlist = {}
		end

		appdf.onHttpJsionTable(self._newfileurl,"GET","",function(jsondata,response)
				--记录新的list
				local fileUtil = cc.FileUtils:getInstance()
				this._response = response
				if jsondata then
					local newlist = jsondata["listdata"]
					if nil == newlist and nil ~= this._listener.updateResult then
						this._listener:updateResult(false,"获取服务器列表失败！")
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
								table.insert(this._downList , {newpath,newname})
							end
						end
						
					end
					print("update_count:"..#this._downList)
					--开始下载
					if #this._downList >0 then
						this._retryCount = 3
						this._downIndex = 1
						this:UpdateFile()
					else
						cc.FileUtils:getInstance():writeStringToFile(self._response,self._curfile)
						this._listener:updateResult(true,"")
					end
				else
					this._listener:updateResult(false,"获取服务器列表失败！")
				end
			end)

		result = true
	end
	if not result then
		listener:updateResult(false,msg)
	end

end

--下载
function ClientUpdate:UpdateFile()
	if not self._downIndex or not self._downList  then 
		self._listener:updateResult(false,"下载信息损坏！")
		return
	end 
	--列表完成
	if self._downIndex == (#self._downList + 1) then
		--更新本地MD5
		cc.FileUtils:getInstance():writeStringToFile(self._response,self._curfile)
		--通知完成
		self._listener:updateResult(true,"")
		return 
	end
	--下载参数
	local this = self
	local fileinfo = self._downList[self._downIndex]
	local url 
	local dstpath

	url = self._downUrl..fileinfo[1]..fileinfo[2]
	dstpath = self._savepath..fileinfo[1]

	--print("from ==> " .. url .. " download " .. fileinfo[2])
	--调用C++下载
	downFileAsync(url,fileinfo[2],dstpath,function(main,sub)
			--下载回调
			if main == appdf.DOWN_PRO_INFO then --进度信息
				this._listener:updateProgress(sub,fileinfo[2], (this._downIndex / (#self._downList + 1)) * 100)
			elseif main == appdf.DOWN_COMPELETED then --下载完毕
				self._retryCount = 3;
				this._downIndex = this._downIndex +1
				this._listener:runAction(cc.CallFunc:create(function()
						this:UpdateFile()
					end))
			else
				if sub == 28 and self._retryCount > 0 then
					self._retryCount = self._retryCount - 1
					this._listener:runAction(cc.CallFunc:create(function()
						this:UpdateFile()
					end))
				else
					this._listener:updateResult(false,"下载失败,main:" .. main .. " ## sub:" .. sub) --失败信息
				end
			end
		end)
end

return ClientUpdate