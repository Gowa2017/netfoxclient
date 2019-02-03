local GameServerItem = class("GameServerItem")

function GameServerItem:ctor(pData)

	self.wKindID			=0	
	self.wNodeID			=0								
	self.wSortID			=0									
	self.wServerID 			=0									
	self.wServerKind		=0
	self.wServerType		=0
	self.wServerLevel		=0               					
	self.wServerPort		=0																	

	self.lCellScore 		=0									
	self.cbEnterMember		=0
	self.lEnterScore		=0

	self.dwServerRule		=0
	self.dwOnLineCount		=0
	self.dwAndroidCount		=0
	self.dwFullCount		=0									

	self.szServerAddr 		= ""								
	self.szServerName 		= ""								

end


function GameServerItem:onInit(pData)
	if pData == nil then
		print("GameServerItem-onInit-null")
		return
	end

    local int64 = Integer64.new()

	self.wKindID = pData:readword()				--名称索引
	self.wNodeID = pData:readword()				--节点索引
	self.wSortID = pData:readword()				--排序索引
	self.wServerID = pData:readword()			--房间索引
	self.wServerKind = pData:readword()			--房间类型
	self.wServerType = pData:readword()			--房间类型
	self.wServerLevel= pData:readword()			--房间等级
	self.wServerPort = pData:readword()			--房间端口

	self.lCellScore =  pData:readscore(int64):getvalue()			--单元积分
	self.cbEnterMember = pData:readbyte() 							--进入会员
	self.lEnterScore =  pData:readscore(int64):getvalue()			--进入积分

	self.dwServerRule = pData:readdword()		--房间规则
	self.dwOnLineCount= pData:readdword()		--在线人数
	self.dwAndroidCount = pData:readdword()		--机器人数
	self.dwFullCount  = pData:readdword()		--满员人数

	self.szServerAddr = pData:readstring(32)	--房间地址
	self.szServerName = pData:readstring(32) 	--房间名称
	self.dwSurportType = pData:readdword() 		--支持类型
	self.wTableCount = pData:readword() 		--桌子数目

	--self:testlog()
	return self
end

function GameServerItem:testlog() 
	print("**************************************************")
	--dump(self, "GameServerItem", 6)
	print("**************************************************")
end

function GameServerItem:readScore(dataBuffer)
    if self._int64 == nil then
        self._int64 = Integer64.new():addTo(self)
    end
    dataBuffer:readscore(self._int64)
    return self._int64:getvalue()
end

return GameServerItem