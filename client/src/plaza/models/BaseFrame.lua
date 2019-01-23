local BaseFrame = class("BaseFrame")

function BaseFrame:ctor(view,callback)
	self._viewFrame = view
	self._threadid  = nil
	self._socket    = nil
	self._callBack = callback
	-- 游戏长连接
	self._gameFrame = nil
	self.m_tabCacheMsg = {}
end

function BaseFrame:setCallBack(callback)
	self._callBack = callback
end

function BaseFrame:setViewFrame(viewFrame)
	self._viewFrame = viewFrame
end

function BaseFrame:setSocketEvent(socketEvent)
	self._socketEvent = socketEvent	
end

function BaseFrame:getViewFrame()
	return self._viewFrame
end

function BaseFrame:isSocketServer()
	return self._socket ~= nil and self._threadid ~=nil
end

local targetPlatform = cc.Application:getInstance():getTargetPlatform()
--网络错误
function BaseFrame:onSocketError(pData)
	if self._threadid == nil then
		return
	end
	local cachemsg = cjson.encode(self.m_tabCacheMsg)
	if nil ~= cachemsg then
		if cc.PLATFORM_OS_WINDOWS == targetPlatform then
			LogAsset:getInstance():logData(cachemsg or "",true)
		else
			buglyReportLuaException(cachemsg or "", debug.traceback())
		end	
	end
	
	self:onCloseSocket()

	if  self._callBack ~= nil then
		if not pData then
			self._callBack(-1,"网络断开！")
		elseif type(pData) == "string" then
			self._callBack(-1,pData)
		else
			local errorcode = pData:readword()
			if errorcode == nil then
				self._callBack(-1,"网络断开！")
			elseif errorcode == 6 then
				self._callBack(-1,"长时间无响应，网络断开！")
			elseif errorcode == 3 then
				self._callBack(-1,"网络连接超时, 请重试!")
				-- 切换地址
				if nil ~= yl.SERVER_LIST[yl.CURRENT_INDEX] then
					yl.LOGONSERVER = yl.SERVER_LIST[yl.CURRENT_INDEX]
				end
				yl.CURRENT_INDEX = yl.CURRENT_INDEX + 1
				if yl.CURRENT_INDEX > yl.TOTAL_COUNT then
					yl.CURRENT_INDEX = 1
				end
			else
				self._callBack(-1,"网络错误，code："..errorcode)			
			end
		end
	end

end

--启动网络
function  BaseFrame:onCreateSocket(szUrl,nPort)
	--已存在连接
	if self._socket ~= nil then
		return false
	end
	--创建连接
	local this = self
	self._szServerUrl = szUrl 
	self._nServerPort = nPort
	self._SocketFun = function(pData)
			this:onSocketCallBack(pData)
		end
	self._socket = CClientSocket:createSocket(self._SocketFun)
	self._socket:setwaittime(0)
	if self._socket:connectSocket(self._szServerUrl,self._nServerPort,yl.VALIDATE) == true then
		self._threadid = 0
		return true
	else --创建失败
		self:onCloseSocket() 
		return false
	end
end

--网络消息回调
function BaseFrame:onSocketCallBack(pData)
	--无效数据
	if  pData == nil then 
		return
	end
	if not self._callBack then
		print("base frame no callback")
		self:onCloseSocket()
		return
	end
	
	-- 连接命令
	local main = pData:getmain()
	local sub =pData:getsub()
	print("onSocketCallBack main:"..main.."#sub:"..sub)	
	if main == yl.MAIN_SOCKET_INFO then 		--网络状态
		if sub == yl.SUB_SOCKET_CONNECT then
			self._threadid = 1	
			self:onConnectCompeleted()
		elseif sub == yl.SUB_SOCKET_ERROR then	--网络错误
			if self._threadid then
				self:onSocketError(pData)
			else
				self:onCloseSocket()
			end			
		else
			self:onCloseSocket()
		end
	else
		if 1 == self._threadid then--网络数据
			self:onSocketEvent(main,sub,pData)
		end
	end
end

--关闭网络
function BaseFrame:onCloseSocket()
	if self._socket then
		self._socket:relaseSocket()
		self._socket = nil
	end
	if self._threadid then
		self._threadid = nil
	end
	self._SocketFun = nil
end

--发送数据
function BaseFrame:sendSocketData(pData)
	if self._socket == nil then
		self._callBack(-1)
		return false
	end
	local tabCache = {}
	tabCache["main"] = pData:getmain()
	tabCache["sub"] = pData:getsub()
	tabCache["len"] = pData:getlen()
	tabCache["kindid"] = GlobalUserItem.nCurGameKind
	table.insert( self.m_tabCacheMsg, tabCache )
	if #self.m_tabCacheMsg > 5 then
		table.remove(self.m_tabCacheMsg, 1)
	end
	if not self._socket:sendData(pData) then
		self:onSocketError("发送数据失败！")
		return false
	end
	return true
end

--连接OK
function BaseFrame:onConnectCompeleted()
	print("warn BaseFrame-onConnectResult-"..result)
end

--网络信息(短连接)
function BaseFrame:onSocketEvent(main,sub,pData)
	print("warn BaseFrame-onSocketData-"..main.."-"..sub)
end

--网络消息(长连接)
function BaseFrame:onGameSocketEvent(main,sub,pData)
	print("warn BaseFrame-onGameSocketEvent-"..main.."-"..sub)
end

return BaseFrame