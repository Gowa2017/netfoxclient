local MusicRecordProxy = class("MusicRecordProxy", puremvc.Proxy)

function MusicRecordProxy:ctor()
    self.super.ctor(self)
    self.proxyName = "MusicRecordProxy"

	--�Ѳ��ŵ������б�
	self._musicList = {}
end

--����������ò������ô��� ���� �ܹ����ô���
function MusicRecordProxy:addRef(path)
	for i = 1, #self._musicList do
		local item = self._musicList[i]
		if (item.path == path) then
			item.ref = item.ref + 1
			return item.ref
		end
	end
	
	local item = {}
	item.path = path
	item.ref = 1
	
	table.insert(self._musicList, item)
	
	return item.ref
end

function MusicRecordProxy:onRegister()
    self.super.onRegister(self)
end

function MusicRecordProxy:onRemove()
    self.super.onRemove(self)
end

return MusicRecordProxy