local GameListAccessProxy = class("GameListAccessProxy", puremvc.Proxy)

function GameListAccessProxy:ctor()
    self.super.ctor(self)
    self.proxyName = "GameListAccessProxy"

	--��ȡ��Ϸ���ʼ�¼
	self:read()
end

--����������ò������ô��� ���� �ܹ����ô���
function GameListAccessProxy:addRef(kinid)
	for i = 1, #self._gameList do
		local item = self._gameList[i]
		if (item.kinid == kinid) then
			item.ref = item.ref + 1
			
			table.sort(self._gameList, function(a, b)
				return a.ref > b.ref
			end)
			self:write()
			
			return item.ref
		end
	end
	
	local item = {}
	item.kinid = kinid
	item.ref = 1
	
	table.insert(self._gameList, item)
	
	table.sort(self._gameList, function(a, b)
		return a.ref > b.ref
	end)	
	self:write()

	return item.ref
end

function GameListAccessProxy:read()
	local json_data = cc.UserDefault:getInstance():getStringForKey("GAME_ACCESS_DATA")
	if (json_data ~= nil and json_data ~= "") then
		local table_data = json.decode(json_data)
		self._gameList = table_data or {}
	else
		self._gameList = {}
	end	
end

function GameListAccessProxy:write()
	--ת��ΪJson���ݴ洢
	local json_data = json.encode(self._gameList)
	cc.UserDefault:getInstance():setStringForKey("GAME_ACCESS_DATA", json_data)
	cc.UserDefault:getInstance():flush()	
end

function GameListAccessProxy:onRegister()
    self.super.onRegister(self)
end

function GameListAccessProxy:onRemove()
    self.super.onRemove(self)
end

return GameListAccessProxy