local AppConfigProxy = class("AppConfigProxy", puremvc.Proxy)

function AppConfigProxy:ctor()
    self.super.ctor(self)
    self.proxyName = "AppConfigProxy"

	--��Ϸ��Ϣ
	self._gameList = {}
	--���µ�ַ
	self._updateUrl = ""
	--����������ȡ��������Ϣ
	self._serverConfig = {}
	--��˿���
	self._appStoreSwitch = 0
	--����
	self._updatePackage = ""
	--��¼����
	self._logonNotice = {}
	--��������
	self._hallNotice = {}
end

function AppConfigProxy:loadData()
    self:sendNotification("AppConfigLoadFinished")
end

function AppConfigProxy:onRegister()
    self.super.onRegister(self)
    self:loadData()
end

--���󹫸沢���浽Ӧ��������
function AppConfigProxy:requestNotice()
	--���ô�����������
	self._hallNotice = {}
	local url = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx?action=GetMobileRollNotice"         	
	appdf.onHttpJsionTable(url ,"GET","",function(jstable,jsdata)
		if type(jstable) == "table" then
			local data = jstable["data"]
			local msg = jstable["msg"]
			if type(data) == "table" then
				local valid = data["valid"]
				if nil ~= valid and true == valid then
					local list = data["notice"]
					if type(list)  == "table" then
						local listSize = #list
						for i = 1, listSize do
							list[i].content = string.gsub(list[i].content or "","[\r\n]", "")
							table.insert(self._hallNotice, list[i])
						end
					end
				end
			end
			
			self:sendNotification(GAME_COMMAMD.HALL_GONGGAO_RESULT, self._hallNotice, msg)
		end
	end)
end

function AppConfigProxy:onRemove()
    self.super.onRemove(self)
end

return AppConfigProxy