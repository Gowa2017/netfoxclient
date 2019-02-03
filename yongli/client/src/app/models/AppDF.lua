--[[
	常用定义
]]
appdf = appdf or {}

--屏幕高宽
appdf.WIDTH									= 1334
appdf.HEIGHT								= 750
appdf.g_scaleY                              = display.height / appdf.HEIGHT    --Y坐标的缩放比例值 added ycc

appdf.CLIENT_SRC                            = "client.src."
appdf.GAME_SRC                              = "game."
--扩展目录
appdf.EXTERNAL_SRC							= "client.src.external."
--通用定义目录
appdf.HEADER_SRC							= "client.src.header."
--下载信息
appdf.DOWN_PRO_INFO							= 1 									--下载进度
appdf.DOWN_COMPELETED						= 2 									--下载结果
appdf.DOWN_ERROR_PATH						= 3 									--路径出错
appdf.DOWN_ERROR_CREATEFILE					= 4 									--文件创建出错
appdf.DOWN_ERROR_CREATEURL					= 5 									--创建连接失败
appdf.DOWN_ERROR_NET		 				= 6 									--网络错误
appdf.DOWN_FILE_ERROR						= 7	
appdf.DOWN_PRO_SUB_INFO		 				= 8
appdf.DOWN_STOPED		 				= 9

--解压信息
appdf.UNZIP_COMPELETED = 1						--下载完成
appdf.UNZIP_ERROR_PATH = 2						--目标文件不存在
appdf.UNZIP_FILE_FAILED = 3						--目标文件不存在
appdf.UNZIP_ERROR_CREATEFILE = 4				--创建文件失败

appdf.BASE_GAME = 
{	
    {kind = 406,version = "100"},
    {kind = 6,version = "100"},
--[[	{kind = 408,version = "100"},
	--{kind = 36, version = "100"},
	{kind = 28, version = "100"},
	{kind = 200, version = "100"}--]]
}

function appdf.req(path)
    if path and type(path) == "string" then
        return require(path)
    else
        print("require paht unknow")
    end
    
end


function appdf.isIpAddress(ip)
 if not ip then return false end
 local a,b,c,d=ip:match("^(%d%d?%d?)%.(%d%d?%d?)%.(%d%d?%d?)%.(%d%d?%d?)$")
 a=tonumber(a)
 b=tonumber(b)
 c=tonumber(c)
 d=tonumber(d)
 if not a or not b or not c or not d then return false end
 if a<0 or 255<a then return false end
 if b<0 or 255<b then return false end
 if c<0 or 255<c then return false end
 if d<0 or 255<d then return false end
 return true
end
-- 字符分割
function appdf.split(str, flag)
	local tab = {}
	while true do

		local n = string.find(str, flag)
		if n then
			local first = string.sub(str, 1, n-1) 
			str = string.sub(str, n+1, #str) 
			table.insert(tab, first)
		else
			table.insert(tab, str)
			break
		end
	end
	return tab
end

--依据宽度截断字符
function appdf.stringEllipsis(szText, sizeE,sizeCN,maxWidth)
	--当前计算宽度
	local width = 0
	--截断位置
	local lastpos = 0
	--截断结果
	local szResult = "..."
	--完成判断
	local bOK = false
	 
	local i = 1
	 
	while true do
		local cur = string.sub(szText,i,i)
		local byte = string.byte(cur)
		if byte == nil then
			break
		end
		if byte > 128 then
			if width +sizeCN <= maxWidth - 3*sizeE then
				width = width +sizeCN
				 i = i + 3
				 lastpos = i+2
			else
				bOK = true
				break
			end
		elseif	byte ~= 32 then
			if width +sizeE <= maxWidth - 3*sizeE then
				width = width +sizeE
				i = i + 1
				lastpos = i
			else
				bOK = true
				break
			end
		else
			i = i + 1
			lastpos = i
		end
	end
	 
	 	if lastpos ~= 0 then
			szResult = string.sub(szText, 1, lastpos)
			if(bOK) then
				szResult = szResult.."..."
			end
		end
		return szResult
end

--打印table
function appdf.printTable(dataBuffer)
	if not dataBuffer then
		print("printTable:dataBuffer is nil!")
		return
	end
	if type(dataBuffer) ~= "table" then
		print("printTable:dataBuffer is not table!")
		return
	end
	for k ,v in pairs(dataBuffer) do
		local typeinfo = type(v) 
		if typeinfo == "table" then
			appdf.printTable(v)
		elseif typeinfo == "userdata" then
			print("key["..k.."]value[userdata]")
		elseif typeinfo == "boolean" then
			print("key["..k.."]value["..(v and "true" or "false").."]")
		else
			print("key["..k.."]value["..v.."]")
		end
	end

end

--HTTP获取json
function appdf.onHttpJsionTable(url,methon,params,callback)
	print("appdf.onHttpJsionTable:"..url)
	local xhr = cc.XMLHttpRequest:new()
	xhr.responseType = cc.XMLHTTPREQUEST_RESPONSE_JSON
	local bPost = ((methon == "POST") or (methon == "post"))

	--模式判断
	if not bPost then
		if params ~= nil and params ~= "" then
			xhr:open(methon, url.."?"..params)
		else
			xhr:open(methon, url)
		end
	else
		xhr:open(methon, url)
	end
	--HTTP回调函数
	local function onJsionTable()
		local datatable 
		local response
		local ok
	    if xhr.readyState == 4 and (xhr.status >= 200 and xhr.status < 207) then
	   		response  = xhr.response -- 获得响应数据
	   		if response then
	   		    ok, datatable = pcall(function()
			       return cjson.decode(response)
			    end)
			    if not ok then
			    	print("onHttpJsionTable_cjson_error")
			    	datatable = nil
			    end
		    end
	    else
	    	print("onJsionTable http fail readyState:"..xhr.readyState.."#status:"..xhr.status)
	    end
	    if type(callback) == "function" then
	    	callback(datatable,response)
	    end	    
	end
	xhr:registerScriptHandler(onJsionTable)
	if not bPost then
		xhr:send()
	else
		xhr:send(params)
	end
	return true
end

--短连通讯
function appdf.onSendData(url, port, data, recvCallBack)
	--复制到本地（防止释放）
	--连接后最大允许未活动时间（超过该时间未活动的通讯连接将会被断开）单位秒
	data:retain()
	local freeTimeMaxRemain = 15
	local curTimeRemain = freeTimeMaxRemain
	--定时器
	local scheduler = cc.Director:getInstance():getScheduler()
	local schedulerHandle = nil
	--通讯句柄
	local socket = nil
	
	local threadid = nil
	
	--关闭网络
	local function onCloseSocket()
		if socket then
			socket:relaseSocket()
			socket = nil
			data:release()
		end
		
		threadid = nil
		
		if (schedulerHandle) then
			scheduler:unscheduleScriptEntry(schedulerHandle)
			schedulerHandle = nil
		end
	end
	
	--网络错误
	local function onSocketError(pData)
		if threadid == nil then
			return
		end

		if  recvCallBack ~= nil then
			if not pData then
				recvCallBack({}, {code = -1, msg = "网络断开"})
			elseif type(pData) == "string" then
				recvCallBack({}, {code = -1, msg = pData})
			else
				local errorcode = pData:readword()
				if errorcode == nil then
					recvCallBack({}, {code = -1, msg = "网络断开！"})
				elseif errorcode == 6 then
					recvCallBack({}, {code = -1, msg = "长时间无响应，网络断开！"})
				elseif errorcode == 3 then
					recvCallBack({}, {code = -1, msg = "网络连接超时, 请重试!"})
				else
					recvCallBack({}, {code = -1, msg = "网络错误，code："..errorcode})			
				end
			end
		end
	end
	
	
	--网络事件回调
	local function onEventCallBack(pData)
		--无效数据
		if  pData == nil then 
			return
		end
		if not recvCallBack then
			onCloseSocket()
			return
		end
		
		-- 连接命令
		local main = pData:getmain()
		local sub =pData:getsub()

		if main == yl.MAIN_SOCKET_INFO then 		--网络状态
			if sub == yl.SUB_SOCKET_CONNECT then
				threadid = 1	
				--self:onConnectCompeleted()
				if not socket:sendData(data) then
					onSocketError("发送数据失败！")
					onCloseSocket()
					return
				end
				
				schedulerHandle = scheduler:scheduleScriptFunc(function(dt)
					curTimeRemain = curTimeRemain - dt
					if (curTimeRemain <= 0) then
						onCloseSocket()
					end
				end, 1, false)
				
			elseif sub == yl.SUB_SOCKET_ERROR then	--网络错误
				if threadid then
					onSocketError(pData)
					onCloseSocket()
				else
					onCloseSocket()
				end			
			else
				onCloseSocket()
			end
		else
			if 1 == threadid then--网络数据
				recvCallBack({main = main, sub = sub, data = pData}, {code = 0, msg = ""})
				curTimeRemain = freeTimeMaxRemain
			end
		end
	end
	
	--创建网络
	socket = CClientSocket:createSocket(onEventCallBack)
	
	if socket == nil then
		recvCallBack({}, {code = -1, msg = "Socket 未创建"})
		data:release()
		return false
	end
	
	socket:setwaittime(0)
	if socket:connectSocket(url,port, yl.VALIDATE) == true then
		threadid = 0
		return true
	else --创建失败
		onCloseSocket() 
		return false
	end
	
	return true
end

--创建版本
function appdf.ValuetoVersion(value)
	if not value then
		return {p=0,m=0,s=0,b=0}
	end
	local tmp 
	if type(value) ~= "number" then
		tmp = tonumber(value)
	else
		tmp = value
	end
	return
	{
		p = bit.rshift(bit.band(tmp,0xFF000000),24),
		m = bit.rshift(bit.band(tmp,0x00FF0000),16),
		s = bit.rshift(bit.band(tmp,0x0000FF00),8),
		b = bit.band(tmp,0x000000FF)
	}
end

--创建颜色
function appdf.ValueToColor( r,g,b )
	r = r or 255
	g = g or 255
	b = b or 255
	if type(r) ~= "number" then
		r = 255
	end
	if type(g) ~= "number" then
		g = 255
	end
	if type(b) ~= "number" then
		b = 255
	end

	local c = 0
	c = bit.lshift(bit.band(0, 255),24)
	c = bit.bor(c, bit.lshift(bit.band(r, 255),16))
	c = bit.bor(c, bit.lshift(bit.band(g, 255),8))
	c = bit.bor(c, bit.band(b, 255))

	return c
end

--版本值
function appdf.VersionValue(p,m,s,b)

	local v = 0
	if p ~= nil then
		v = bit.bor(v,bit.lshift(p,24))
	end
	if m ~= nil then
		v = bit.bor(v,bit.lshift(m,16))
	end
	if s ~= nil then
		v = bit.bor(v,bit.lshift(s,8))
	end
	if b ~= nil then
		v = bit.bor(v,b)
	end

	return v
end

---根據名稱取node
function appdf.getNodeByName(node,name)
	local curNode = node:getChildByName(name)
	if curNode then
		return curNode
	else
		local  nodeTab = node:getChildren()
		if #nodeTab>0 then		
			for i=1,#nodeTab do
				local  result = appdf.getNodeByName(nodeTab[i],name)
				if result then					
					return result
				end 
			end
		end

	end
end

--获取路径  
function appdf.stripfilename(filename)  
    return string.match(filename, "(.+)/[^/]*%.%w+$") --*nix system  
    --return string.match(filename, “(.+)\\[^\\]*%.%w+$”) — windows  
end  
  
--获取文件名  
function appdf.strippath(filename)  
    return string.match(filename, ".+/([^/]*%.%w+)$") -- *nix system  
    --return string.match(filename, “.+\\([^\\]*%.%w+)$”) — *nix system  
end  
  
--去除扩展名  
function appdf.stripextension(filename)  
    local idx = filename:match(".+()%.%w+$")  
    if(idx) then  
        return filename:sub(1, idx-1)  
    else  
        return filename  
    end  
end  
  
--获取扩展名  
function appdf.getextension(filename)  
    return filename:match(".+%.(%w+)$")  
end  

--Emali验证
function appdf.checkEmail(str)
    if string.len(str or "") < 6 then return false end
    local b,e = string.find(str or "", '@')
    local bstr = ""
    local estr = ""
    if b then
        bstr = string.sub(str, 1, b-1)
        estr = string.sub(str, e+1, -1)
    else
        return false
    end


    -- check the string before '@'
    local p1,p2 = string.find(bstr, "[%w_]+")
    if (p1 ~= 1) or (p2 ~= string.len(bstr)) then return false end


    -- check the string after '@'
    if string.find(estr, "^[%.]+") then return false end
    if string.find(estr, "%.[%.]+") then return false end
    if string.find(estr, "@") then return false end
    if string.find(estr, "%s") then return false end --空白符
    if string.find(estr, "[%.]+$") then return false end


    _,count = string.gsub(estr, "%.", "")
    if (count < 1 ) or (count > 3) then
        return false
    end


    return true
end

--手机号验证
function appdf.checkPhone(var)
	return string.match(var,"[1][3,4,5,7,8]%d%d%d%d%d%d%d%d%d") == var
end

--加载界面根节点，设置缩放达到适配
function appdf.loadRootCSB( csbFile, parent )
	local rootlayer = ccui.Layout:create()
		:setContentSize(1335,750) --这个是资源设计尺寸
		:setScale(appdf.WIDTH / 1335);
	if nil ~= parent then
		parent:addChild(rootlayer);
	end

	local csbnode = cc.CSLoader:createNode(csbFile);
	rootlayer:addChild(csbnode);

	return rootlayer, csbnode;
end

--加载csb资源
function appdf.loadCSB( csbFile, parent )
	local csbnode = cc.CSLoader:createNode(csbFile);
	if nil ~= parent then
		parent:addChild(csbnode);
	end
	return csbnode;	
end

--加载 帧动画
function appdf.loadTimeLine( csbFile )
	return cc.CSLoader:createTimeline(csbFile);	 
end

function appdf.loadImage(plistFileName, imageFilename, callback)
	local textureCache = cc.Director:getInstance():getTextureCache()
	local spriteFrameCache = cc.SpriteFrameCache:getInstance()	
    if not callback then
        textureCache:addImage(imageFilename)
		spriteFrameCache:addSpriteFrames(plistFileName, imageFilename)
    else
        textureCache:addImageAsync(imageFilename, function ()
			spriteFrameCache:addSpriteFrames(plistFileName, imageFilename)
			callback()
		end)
    end
end

-- 苹果系统语言
function appdf.getAppleLanguages()
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_MAC == targetPlatform) then
		local args = {}
		local luaoc = require "cocos.cocos2d.luaoc"
		local ok, ret = luaoc.callStaticMethod("AppController", "getAppleLanguages", args)
		if not ok then
			local msg = "luaoc error:" .. ret
			print(msg)
			return false, msg
		else
			return ok, ret
		end
	else
		return true, "zh"
	end
end 