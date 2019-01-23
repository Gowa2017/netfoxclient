--
-- Author: zhong
-- Date: 2016-07-01 19:33:58
--
--
--[[
* 裁剪文本
]]

local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")

local ClipText = class("ClipText", --[[ccui.Layout]]function ( ... )
	local la = ccui.Layout:create()
	la._setAnchorPoint = la.setAnchorPoint --重写方法
	return la
end)

local m_tableTxtConfig = {}
------
--var
emClipType = 
{
	"kClipRunAct",
	"kClipReplace"
};
ClipText.emClipType = ExternalFun.declarEnumWithTable(0, emClipType);
------

function ClipText:createClipText( visibleSize, str, fontName, fontSize, szType )
	local text = ClipText.new();
	if nil ~= text then
		text.m_szVisibleSize = visibleSize;
		text.m_emClipType = szType;
		text:initText(str, fontName, fontSize);
	end
	return text;
end

function ClipText:setAnchorPoint( anchor )
	self:_setAnchorPoint(anchor);
	self:setTextAnchorPoint(anchor);
end

function ClipText:setTextAnchorPoint( anchor )
	local pNode = self.m_text;
	if nil ~= pNode then
		pNode:setAnchorPoint(anchor);
		self:updateMoveAction(pNode);
		local m_szVisibleSize = self.m_szVisibleSize;
		if ClipText.emClipType.kClipRunAct == self.m_emClipType then
			if pNode:getContentSize().width >= self.m_szVisibleSize.width then
				self:moveStr();
			end
		else
			pNode:setPosition(m_szVisibleSize.width * anchor.x, m_szVisibleSize.height * anchor.y);
		end
	end
end

function ClipText:setClipMode( mode , replaceStr)
	self.m_emClipType = mode;
	self.m_replaceStr = replaceStr or "...";
	local pNode = self.m_text;

	if nil == pNode then
		return;
	end

	local anchor = pNode:getAnchorPoint();
	if pNode:getContentSize().width >= self.m_szVisibleSize.width then
		if ClipText.emClipType.kClipRunAct == self.m_emClipType then
			self:moveStr();
		else
			self:parseStr(self.m_strText);
		end
	end
end

function ClipText:setTextPosition( pos )
	if nil ~= self.m_text then
		self.m_text:setPosition(pos);
		self:updateMoveAction(self.m_text);
	end
end

function ClipText:setString( str )
	self.m_strText = str;
	if nil == self.m_text then
		return;
	end
	local m_text = self.m_text;
	m_text:setString(str);
	self:updateMoveAction(m_text);

	local anchor = m_text:getAnchorPoint();
	if m_text:getContentSize().width >= self.m_szVisibleSize.width then
		if ClipText.emClipType.kClipRunAct == self.m_emClipType then
			self:moveStr();
		else
			self:parseStr(self.m_strText);
		end
	end
end

function ClipText:getString()
	return self.m_strText
end

function ClipText:getClipText()
	return self.m_text
end

function ClipText:setTextColor( color )
	if nil == self.m_text then
		return;
	end

	self.m_text:setColor(color);
end

function ClipText:setTextFontSize( fSize )
	if nil == self.m_text then
		return;
	end

	self.m_nFontSize = fSize;
	self.m_text:setFontSize(fSize);
end

function ClipText:initText( str, fontName, fontSize )
	--裁剪
	self:setClippingEnabled(true);
	self:setContentSize(self.m_szVisibleSize);

	--注册事件
	local function onLayoutEvent( event )
		if event == "exit" then
			self:onExit();
        elseif event == "enterTransitionFinish" then
        	self:onEnterTransitionFinish();
        end
	end
	self:registerScriptHandler(onLayoutEvent);

	fontName = fontName or "fonts/round_body.ttf";
	self.m_strFontName = fontName;
	fontSize = fontSize or 20;
	self.m_nFontSize = fontSize;

	self.m_strText = str;	
	self.m_emClipType = self.m_emClipType or ClipText.emClipType.kClipReplace;
	
	local text = ccui.Text:create(str, fontName, fontSize)
				 :addTo(self);
	self.m_text = text;

	--更新移动动作
	self:updateMoveAction(text);
	--设置锚点
	self:setTextAnchorPoint(self:getAnchorPoint());

	if text:getContentSize().width >= self.m_szVisibleSize.width then
		if ClipText.emClipType.kClipReplace == self.m_emClipType then
			self:parseStr(str);
		else
			self:moveStr();
		end
	end
end

function ClipText:registerTouch(  )
	local function onTouchBegan( touch, event )
		local pos = touch:getLocation();
		local child = self.m_text;
		if nil ~= child then
			pos = child:convertToNodeSpace(pos);
			local rect = cc.rect(0,0,child:getContentSize().width,child:getContentSize().height);

			if cc.rectContainsPoint(rect, pos) then
				return true;
			end
			return false;
		end
	end

	local function onTouchEnded( touch, event )
		local child = self.m_text;
		if nil ~= child then
			local runningAct = child:getNumberOfRunningActions();
			if 0 == runningAct then
				local childSize = child:getContentSize();
				local m_szVisibleSize = self.m_szVisibleSize;
				if (childSize.width >= m_szVisibleSize.width) and (ClipText.emClipType.kClipRunAct == self.m_emClipType) then
					child:stopAllActions();
					local anchor = child:getAnchorPoint();
					child:setPosition(cc.p(m_szVisibleSize.width * anchor.x, m_szVisibleSize.height * anchor.y));
					if nil ~= self.m_actMoveAct then
						child:runAction(self.m_actMoveAct);
					end					
				end
			end
		end
	end

	local listener = cc.EventListenerTouchOneByOne:create();
	self.listener = listener;
    listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN );
    listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED );
    local eventDispatcher = self:getEventDispatcher();
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self);
end

function ClipText:onExit(  )
	local eventDispatcher = self:getEventDispatcher();
	eventDispatcher:removeEventListener(self.listener);

	ExternalFun.SAFE_RETAIN(self.m_actMoveAct);
end

function ClipText:onEnterTransitionFinish(  )
	self:registerTouch();
end

function ClipText:updateMoveAction( text )
	ExternalFun.SAFE_RELEASE(self.m_actMoveAct);
	local m_szVisibleSize = self.m_szVisibleSize;

	local anchor = text:getAnchorPoint();
	local textSize = text:getContentSize();
	local pos = cc.p(-textSize.width, text:getPositionY());
	local moveBy = cc.MoveTo:create(5.0 + (textSize.width / m_szVisibleSize.width) * 2.0, pos);
    local delay = cc.DelayTime:create(1.5);
    local back = cc.CallFunc:create(function ( ... )
    	text:setPosition(cc.p(text:getContentSize().width * anchor.x + m_szVisibleSize.width, m_szVisibleSize.height * anchor.y));
    end);
    local seq = cc.Sequence:create(moveBy, delay, back);
    local repeatAct = cc.Repeat:create(seq, 3);
    local backOrigin = cc.MoveTo:create(3.0, cc.p(m_szVisibleSize.width * anchor.x, m_szVisibleSize.height * anchor.y));
    self.m_actMoveAct = cc.Sequence:create(repeatAct, backOrigin);

	ExternalFun.SAFE_RETAIN(self.m_actMoveAct);
end

function ClipText:moveStr(  )
	local m_szVisibleSize = self.m_szVisibleSize
	local text = self.m_text;
	if nil == text then
		return;
	end

	local anchor = text:getAnchorPoint();
	text:stopAllActions();
	local pos = cc.p(text:getContentSize().width * anchor.x + m_szVisibleSize.width, m_szVisibleSize.height * anchor.y);
	text:setPosition(pos);
	text:runAction(self.m_actMoveAct);
end

function ClipText:parseStr( str )
	local nTotal = self.m_szVisibleSize.width - self.m_nFontSize;

	local config = ClipText.getFontConfig(self.m_strFontName, self.m_nFontSize)
	
	local pStr = self:stringEllipsis(str, config, nTotal, self.m_replaceStr);
	self.m_text:setString(pStr);
end

--依据宽度截断字符
function ClipText:stringEllipsis(szText, config, maxWidth, replaceStr)
	replaceStr = replaceStr or "...";
	--当前计算宽度
	local width = 0
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
			if width <= maxWidth - 3*config.upperEnSize.width then
				width = width + config.cnSize.width
				i = i + 3
			else
				bOK = true
				break
			end
		elseif	byte ~= 32 then --区分大小写和数字
			if width <= maxWidth - 3*config.upperEnSize.width then
				if string.byte('A') <= byte and byte <= string.byte('Z') then
					width = width + config.upperEnSize.width
				elseif string.byte('a') <= byte and byte <= string.byte('z') then
					width = width + config.lowerEnSize.width
				else
					width = width + config.numSize.width
				end				
				i = i + 1
			else
				bOK = true
				break
			end
		else
			i = i + 1
		end
	end
	 
 	if i ~= 1 then
		szResult = string.sub(szText, 1, i-1)
		if(bOK) then
			szResult = szResult.. replaceStr
		end
	end
	return szResult
end

--获取配置记录
function ClipText.getFontConfig(fontfile, fontsize)
	local strKey = string.format("%s-%d", fontfile, fontsize)
	local config = m_tableTxtConfig[strKey]
	if nil == config then
		config = {}
		local tmpEN = cc.LabelTTF:create("A", fontfile, fontsize)
		local tmpCN = cc.LabelTTF:create("网", fontfile, fontsize)
		local tmpen = cc.LabelTTF:create("a", fontfile, fontsize)
		local tmpNu = cc.LabelTTF:create("2", fontfile, fontsize)
		config.upperEnSize = tmpEN:getContentSize()
		config.cnSize = tmpCN:getContentSize()
		config.lowerEnSize = tmpen:getContentSize()
		config.numSize = tmpNu:getContentSize()

		m_tableTxtConfig[strKey] = config
	end
	return config
end

--计算字符串长度
function ClipText.getLabelWidth(szText, fontfile, fontsize)
	local begin = 1
	local szLen = string.len(szText)
	local gsubSize = 1
	local szWidth = 0
	local config = ClipText.getFontConfig(fontfile, fontsize)

	while true do
		if begin > szLen then
			break
		end

		local cur = string.sub(szText,begin,begin + 1)
		local byte = string.byte(cur)
		if byte == nil then
			break
		end
		if byte > 128 then
			szWidth = szWidth + config.cnSize.width
			gsubSize = 3
		elseif	byte ~= 32 then --区分大小写和数字
			if string.byte('A') <= byte and byte <= string.byte('Z') then
				szWidth = szWidth + config.upperEnSize.width
			elseif string.byte('a') <= byte and byte <= string.byte('z') then
				szWidth = szWidth + config.lowerEnSize.width
			else
				szWidth = szWidth + config.numSize.width
			end
			gsubSize = 1
		else
			gsubSize = 1
		end
		begin = begin + gsubSize
	end
	return szWidth, config
end

return ClipText