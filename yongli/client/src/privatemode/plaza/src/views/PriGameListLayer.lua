--[[
	 MXM Լ����Ϸ�б�
	 Date: 2017-09-21
--]]

-- ˽�˷�ģʽ ��Ϸ�б�


local PriGameListLayer = class("PriGameListLayer", cc.load("mvc").ViewBase)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local priGameListView = appdf.req(PriRoom.MODULE.PLAZAMODULE .."views.ManageGameListLayer")

local priRoomCreateOxNew = appdf.req(PriRoom.MODULE.GAMEMODULE .."yule.oxnew.src.PriRoomCreateLayer")
local priRoomCreateLand = appdf.req(PriRoom.MODULE.GAMEMODULE .."qipai.land.src.PriRoomCreateLayer")

local BTN_EXIT = 100


function PriGameListLayer:ctor( scene )
	ExternalFun.registerNodeEvent(self)
	--GlobalUserItem.nCurRoomIndex = -1
	
	self._scene = scene
    -- ����csb��Դ
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PriGameListLayer.csb", self)
	self._rootlayer = rootLayer
	
	 local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
	--����
	self._return = csbNode:getChildByName("Button_return")
	:setTag(BTN_EXIT)
	self._return:addTouchEventListener(btncallback)
	
	self._sceneLayer = display.newLayer()
		:setContentSize(yl.WIDTH,yl.HEIGHT)
		:addTo(self)
	
	--��Ϸ�����淨����
	self._gameRuleLayer	= csbNode:getChildByName("Game_Rules")
	self._gameRuleLayer:setPosition(230,5)
	
	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
	priGameListView:create(appConfigProxy._gameList)
				:addTo(self._sceneLayer)
	

	PriRoom:getInstance():exitRoom()
	PriRoom:getInstance():enterRoom(scene)
	
	local targetView = nil
	--��ʼ����Ϸ
	if GlobalUserItem.nCurGameKind == 27 or GlobalUserItem.nCurGameKind == 28  or GlobalUserItem.nCurGameKind == 36   then
		--����ţţ
		targetView = priRoomCreateOxNew:create(scene)
				:addTo(self._gameRuleLayer)
				PriRoom:getInstance():setViewFrame(targetView)
	elseif GlobalUserItem.nCurGameKind == 200 then
		--������		
		targetView =priRoomCreateLand:create(scene)
				:addTo(self._gameRuleLayer)
				PriRoom:getInstance():setViewFrame(targetView)
	end	
	
	--������Ϣ
	local listener = cc.EventListenerCustom:create(yl.RY_SELECT_PRIVATEROOM_NOTIFY,function (event)
		local kindID = event.obj
		PriRoom:getInstance():exitRoom()
		PriRoom:getInstance():enterRoom(scene)
		
		if (nil ~= targetView) then
			self._gameRuleLayer:removeChild(targetView)
		end
		
		if kindID == 27 then
			--����ţţ
			targetView = priRoomCreateOxNew:create(scene)
					:addTo(self._gameRuleLayer)
			PriRoom:getInstance():setViewFrame(targetView)
		elseif kindID == 200 then
			--������		
			targetView =priRoomCreateLand:create(scene)
				:addTo(self._gameRuleLayer)
			PriRoom:getInstance():setViewFrame(targetView)
		end
				
	end)
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self)--]]
	
end

function PriGameListLayer:onButtonClickedEvent( tag, sender )
	
    if BTN_EXIT == tag then
		self._scene:onKeyBack()

		if GlobalUserItem.nCurGameKind == 36 or GlobalUserItem.nCurGameKind == 28 then
			GlobalUserItem.nCurGameKind = 27
		end
		self._scene:loadPriGameList(GlobalUserItem.nCurGameKind)

	end  
end

function PriGameListLayer:onEnterTransitionFinish()
end

return PriGameListLayer