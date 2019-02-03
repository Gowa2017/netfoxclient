local RankingListLayer = class("RankingListLayer", function(scene)
		local rankingListLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return rankingListLayer
end)
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")

--继续游戏
RankingListLayer.BT_CONTINUE = 101

-- 进入场景而且过渡动画结束时候触发。
function RankingListLayer:onEnterTransitionFinish()
	if 0 == #self._rankList then
		self._scene:showPopWait()
		appdf.onHttpJsionTable(BaseConfig.WEB_HTTP_URL .. "/WS/PhoneRank.ashx","GET","action=getscorerank&pageindex=1&pagesize=10&userid="..GlobalUserItem.dwUserID,function(jstable,jsdata)
			self._scene:dismissPopWait()
			dump(jstable, "jstable", 5)
			if type(jstable) == "table" then
				for i = 1, #jstable do
					if i == 1 then
						self._myRank.szNickName = jstable[i]["NickName"]
						self._myRank.lScore = jstable[i]["Score"]
						self._myRank.rank = jstable[i]["Rank"]..""
						self._myRank.lv = jstable[i]["Experience"]
					else
						local item = {}
						item.szNickName = jstable[i]["NickName"]
						item.lScore = jstable[i]["Score"]..""
						item.wFaceID = tonumber(jstable[i]["FaceID"])
						item.lv = jstable[i]["Experience"]
						item.cbMemberOrder = tonumber(jstable[i]["MemberOrder"])
						item.dBeans = tonumber(jstable[i]["Currency"])
						item.lIngot = tonumber(jstable[i]["UserMedal"])
						item.dwGameID = tonumber(jstable[i]["GameID"])
						item.dwUserID = tonumber(jstable[i]["UserID"])
						item.szSign = jstable[i]["szSign"] or "此人很懒，没有签名"
						item.szIpAddress = jstable[i]["ip"]
						table.insert(self._rankList,item)
					end
				end
				GlobalUserItem.tabRankCache["rankMyInfo"] = self._myRank
				GlobalUserItem.tabRankCache["rankList"] = self._rankList
				self:onUpdateShow()
			else
				showToast(self,"抱歉，获取排行榜信息失败！",2,cc.c3b(250,0,0))
			end
		end)
	else
		self:onUpdateShow()
	end	
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function RankingListLayer:onExitTransitionStart()
    return self
end

function RankingListLayer:ctor(scene, preTag)
	
	local this = self

	self._scene = scene
	--上一个页面
	self.m_preTag = preTag
	
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
		end
	end)
	self._myRank = GlobalUserItem.tabRankCache["rankMyInfo"] or {name = GlobalUserItem.szNickName,lScore = "0",rank = "0"}
	self._rankList = GlobalUserItem.tabRankCache["rankList"] or {}

	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(667,yl.HEIGHT-51)
        self:addChild(sp)
    end

    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(667,324)
        self:addChild(sp)
    end

    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_2.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(667,324)
        self:addChild(sp)
    end
    
	display.newSprite("Rank/biaoti1.png")
		:move(667,yl.HEIGHT - 41)
		:addTo(self)

	ccui.Button:create("base/bt_return_0.png","base/bt_return_1.png")
		:move(75,yl.HEIGHT-51)
		:addTo(self)
		:addTouchEventListener(function(ref, type)
       		 	if type == ccui.TouchEventType.ended then
					this._scene:onKeyBack()
				end
			end)
    display.newSprite("Rank/dikuang6.png")
    	:move(667,555)
    	:addTo(self)

    local y = 542
	--头像
	--[[local texture = display.loadImage("face.png")
	local facex = (GlobalUserItem.cbGender == yl.GENDER_MANKIND and 1 or 0)
	self._myFace = display.newSprite(texture)
		:setTextureRect(cc.rect(facex*200,0,200,200))
		:setScale(46.00/200.00)
		:move(420,y)
		:addTo(self)
	display.newSprite("Rank/dikuang7.png")
		:move(420,y)
		:addTo(self)]]

	local head = PopupInfoHead:createClipHead(GlobalUserItem, 56)
	head:setPosition(400, 542)
	self:addChild(head)
	head:setIsGamePop(false)
	--根据会员等级确定头像框
	head:enableHeadFrame(true)
	head:enableInfoPop(true, cc.p(420, 120), cc.p(0, 1))
	self._myFace = head

    local testen = cc.Label:createWithSystemFont("A","Arial", 24)
    self._enSize = testen:getContentSize().width
    local testcn = cc.Label:createWithSystemFont("游","Arial", 24)
    self._cnSize = testcn:getContentSize().width

	self._myName = cc.Label:createWithTTF(string.stringEllipsis(GlobalUserItem.szNickName,self._enSize,self._cnSize,165),"base/fonts/round_body.ttf",24)
		:move(460,y)
		:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
		:setAnchorPoint(cc.p(0,0.5))
		:addTo(self)

	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender:getTag(), sender);
		end
	end
	ccui.Button:create("Rank/anniu3.png","Rank/anniu4.png")
		:move(1100,y)	
		:setTag(RankingListLayer.BT_CONTINUE)	
		:addTo(self)
		:addTouchEventListener(btnEvent)

	display.newSprite("Rank/biaoti3.png")
		:move(680,y)
		:addTo(self)

	self._myRankFlag = display.newSprite("Rank/tubiao8.png")
		:move(240,y)
		:addTo(self)

	-- self._myRankNum = cc.LabelAtlas:_create("10", "Rank/shuzi2.png", 24, 34 , string.byte("0")) 
	-- 		:move(240,y)
	-- 		:setAnchorPoint(cc.p(0.5,0.5))
	-- 		:addTo(self)

	self._myScore = cc.LabelAtlas:_create("0", "Rank/shuzi3.png", 19, 24, string.byte("/")) 
			:move(740,y-13)
			:setAnchorPoint(cc.p(0,0))
			:addTo(self)

	--游戏列表
	self._listView = cc.TableView:create(cc.size(1147, 430))
	self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
	self._listView:setPosition(cc.p(95,60))
	self._listView:setDelegate()
	self._listView:addTo(self)
	self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)

	self._listView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	self._listView:registerScriptHandler(self.tableCellAtIndex, cc.TABLECELL_SIZE_AT_INDEX)
	self._listView:registerScriptHandler(self.tableCellTouched, cc.TABLECELL_TOUCHED)
	self._listView:registerScriptHandler(self.numberOfCellsInTableView, cc.NUMBER_OF_CELLS_IN_TABLEVIEW)

end

function RankingListLayer:onUpdateShow()

	local rank = tonumber(self._myRank.rank)
	if rank > 0 and rank < 21 then
		self._myRankFlag:setTexture("Rank/tubiao9.png")
	else
	 	self._myRankFlag:setTexture("Rank/tubiao8.png")
	end 

	self._myScore:setString(string.formatNumberThousands(self._myRank.lScore,true,"/"))
	self._listView:reloadData()
end

function RankingListLayer:onButtonClickedEvent( tag, sender )
	ExternalFun.playClickEffect()
	if RankingListLayer.BT_CONTINUE == tag then
		local enterInfo = self._scene:getEnterGameInfo()
		if nil ~= enterInfo then		
			--回到游戏界面
			GlobalUserItem.setCurGameKind(tonumber(enterInfo._KindID))
			GlobalUserItem.szCurGameName = enterInfo._KindName

			--判断上一个页面是否是房间列表
			if nil ~= self.m_preTag and yl.SCENE_GAMELIST ~= self.m_preTag then
				self._scene:onKeyBack()
			else				
				self._scene:onChangeShowMode(yl.SCENE_ROOMLIST)
			end
		else
			--返回
			self._scene:onKeyBack()
		end
	end
end

---------------------------------------------------------------------

--子视图大小
function RankingListLayer.cellSizeForTable(view, idx)
  	return 1147 , 70
end

--子视图数目
function RankingListLayer.numberOfCellsInTableView(view)
	return #view:getParent()._rankList
end
	
--获取子视图
function RankingListLayer.tableCellAtIndex(view, idx)	
	
	local cell = view:dequeueCell()
	
	local item = view:getParent()._rankList[idx+1]

	if not cell then
		local cy = 35
		cell = cc.TableViewCell:new()
		display.newSprite("Rank/dikuang5.png")
			:addTo(cell)
			:move(1147/2, cy)
			:setTag(1)

		display.newSprite("Rank/tubiao1.png")
			:move(60,cy)
			:setTag(2)
			:addTo(cell)

		--名次数字
		cc.LabelAtlas:_create("10", "Rank/shuzi2.png", 24, 34 , string.byte("0")) 
			:move(60,cy)
			:setTag(3)
			:setAnchorPoint(cc.p(0.5,0.5))
			:addTo(cell)

		display.newSprite("Rank/tubiao7.png")
			:addTo(cell)
			:move(200,cy)
		display.newSprite("Rank/biaoti2.png")
			:addTo(cell)
			:move(240,cy)

		cc.LabelAtlas:_create("0", "Rank/shuzi1.png", 16, 20, string.byte("0")) 
			:move(260,cy)
			:setTag(7)
			:setAnchorPoint(cc.p(0,0.5))
			:addTo(cell)
		--昵称
		cc.Label:createWithTTF("游戏玩家","base/fonts/round_body.ttf",24)
			:move(305,cy)
			:setTag(5)
			:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
			:setAnchorPoint(cc.p(0,0.5))
			:addTo(cell)
		--金币标识
		display.newSprite("Rank/biaoti4.png")
			:move(750,cy)
			:addTo(cell)

		--金币
		cc.LabelAtlas:_create("0", "Rank/shuzi3.png", 19, 24, string.byte("/")) 
			:move(800,cy)
			:setTag(6)
			:setAnchorPoint(cc.p(0,0.5))
			:addTo(cell)
	end
	if cell:getChildByName("cell_face") then
		cell:getChildByName("cell_face"):updateHead(item)
	else
		--头像
		local head = PopupInfoHead:createClipHead(item, 36)
		head:setPosition(130,35)
		head:setIsGamePop(false)
		head:enableHeadFrame(true)
		head:enableInfoPop(true, cc.p(397, 152), cc.p(0, 0.5))
		cell:addChild(head)
		head:setName("cell_face")
	end

	local rankidx = (idx+1)..""
	if  rankidx ~= view:getParent()._myRank.rank then
		cell:getChildByTag(1):setTexture("Rank/dikuang5.png")
	else
		cell:getChildByTag(1):setTexture("Rank/dikuang4.png")
	end

	if idx == 0 then
		cell:getChildByTag(2):setTexture("Rank/tubiao1.png")
		cell:getChildByTag(2):setVisible(true)
		cell:getChildByTag(3):setVisible(false)
	elseif idx == 1 then
		cell:getChildByTag(2):setTexture("Rank/tubiao2.png")
		cell:getChildByTag(2):setVisible(true)
		cell:getChildByTag(3):setVisible(false)
	elseif idx == 2 then 
		cell:getChildByTag(2):setTexture("Rank/tubiao3.png")
		cell:getChildByTag(2):setVisible(true)
		cell:getChildByTag(3):setVisible(false)
	else
		cell:getChildByTag(2):setVisible(false)
		cell:getChildByTag(3):setString((idx+1).."")
		cell:getChildByTag(3):setVisible(true)
	end
	cell:getChildByTag(7):setString(item.lv)
	cell:getChildByTag(5):setString(item.szNickName)
	cell:getChildByTag(6):setString(string.formatNumberThousands(item.lScore,true,"/"))

	return cell
end

function RankingListLayer.tableCellTouched(view, cell)
	if nil ~= cell then
		local face = cell:getChildByName("cell_face")
		if nil ~= face then
			face:onTouchHead()
		end
	end
end
---------------------------------------------------------------------
return RankingListLayer