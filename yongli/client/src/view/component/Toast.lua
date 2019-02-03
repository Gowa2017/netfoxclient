cc.exports.TOAST_LAYER_INDEX = 9999

function showToast(view,message,delaytime,color)
	if (view == nil) or  (message == nil) then
		return
	end
	
	delaytime = delaytime or 1
	if (delaytime < 1) then
		delaytime = 1
	end

	local msgtype = type(message)
	if msgtype == "userdata" or msgtype == "table" then
		return
	end

	if message == "" then
		return
	end
	local showMessage = message
	
	local bg = nil--view:getChildByName("toast_bg")
	local lab = nil
	if bg then
		bg:stopAllActions()
		lab = bg:getChildByName("toast_lab")
		if nil == lab then
			lab = cc.Label:createWithTTF(showMessage, "base/fonts/round_body.ttf", 24, cc.size(930,0))
			lab:setName("toast_lab")
			
			lab:setTextColor(not color and cc.c4b(255,255,255,255) or color)
			lab:addTo(bg)
		end
		lab:stopAllActions()
		bg:setOpacity(255)
		lab:setOpacity(255)
		bg:runAction(cc.Sequence:create(cc.DelayTime:create(delaytime), 
			cc.Spawn:create(cc.FadeTo:create(0.5,0),cc.CallFunc:create(function()
				lab:runAction(cc.FadeTo:create(0.5,0))
				end)), 
			cc.RemoveSelf:create(true)))		
	else
		bg = ccui.ImageView:create("General/frame_1.png")
		--bg:setOpacity(0)
		
		bg:setAnchorPoint(cc.p(0.5, 0))
		bg:move(appdf.WIDTH/2,appdf.HEIGHT)
		bg:addTo(view)
		local curIndex = cc.exports.TOAST_LAYER_INDEX - 1
		if (curIndex < 0) then
			curIndex = 9999
		end
		cc.exports.TOAST_LAYER_INDEX = curIndex
		
		bg:setLocalZOrder(curIndex)
		--bg:setName("toast_bg")
		bg:setScale9Enabled(true)

		lab = cc.Label:createWithTTF(showMessage, "base/fonts/round_body.ttf", 24, cc.size(930,0))
		lab:setName("toast_lab")
		
		--获取文字区域大小
		local txtContentSize = lab:getContentSize()
		
		local bgContentSize = bg:getContentSize();
		if (bgContentSize.height < txtContentSize.height) then
			bgContentSize = cc.size(appdf.WIDTH, txtContentSize.height + 20)
		else
			bgContentSize = cc.size(appdf.WIDTH, bgContentSize.height + 4)
		end

		bg:setContentSize(bgContentSize)
		
		local moveAction = cc.MoveBy:create(0.3, cc.p(0, -bgContentSize.height))
		local moveReverseAction = moveAction:reverse()
		bg:runAction(cc.Sequence:create(moveAction, 
			cc.DelayTime:create(delaytime), 
			moveReverseAction, 
			cc.RemoveSelf:create(true)))
		
		lab:setTextColor(not color and cc.c4b(255,255,255,255) or color)
		lab:addTo(bg)
	end

	if nil ~= lab and nil ~= bg then
		local bgContentSize = bg:getContentSize()
		lab:setString(showMessage)
		lab:setTextColor(not color and cc.c4b(255,255,255,255) or color)
		local labSize = lab:getContentSize()		

		if labSize.height < 30 then
			lab:setHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
		else
			lab:setHorizontalAlignment(cc.TEXT_ALIGNMENT_LEFT)
		end
		lab:move(appdf.WIDTH * 0.5, bgContentSize.height * 0.5)
	end
end

--不渐变形式toast(fadeto方法，在安卓调用的时候会导致底图不显示)
function showToastNoFade(context,message,delaytime,color)
	if (context == nil) or  (message == nil) or (delaytime<1) then
		return
	end

	local msgtype = type(message)
	if msgtype == "userdata" or msgtype == "table" then
		return
	end

	if message == "" then
		return
	end
	local showMessage = message	
	
	local bg = context:getChildByName("toast_bg")
	local lab = nil
	if bg then
		bg:stopAllActions()
		lab = bg:getChildByName("toast_lab")
	else
		bg = ccui.ImageView:create("client/General/frame_1.png")
		bg:move(appdf.WIDTH/2,appdf.HEIGHT/2)
		bg:addTo(context)
		bg:setName("toast_bg")
		bg:setScale9Enabled(true)		

		lab = cc.Label:createWithTTF(showMessage, "base/fonts/round_body.ttf", 24, cc.size(930,0))
		lab:setName("toast_lab")
		
		lab:setTextColor(not color and cc.c4b(255,255,255,255) or color)
		lab:addTo(bg)
	end

	if nil ~= lab and nil ~= bg then
		lab:setString(showMessage)
		lab:setTextColor(not color and cc.c4b(255,255,255,255) or color)
		bg:runAction(cc.Sequence:create(cc.DelayTime:create(delaytime), cc.RemoveSelf:create(true)))

		local labSize = lab:getContentSize()
		if labSize.height < 30 then
			lab:setHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
			bg:setContentSize(cc.size(appdf.WIDTH, 64))
		else
			lab:setHorizontalAlignment(cc.TEXT_ALIGNMENT_LEFT)
			bg:setContentSize(cc.size(appdf.WIDTH, 64 + labSize.height))		
		end
		lab:move(appdf.WIDTH * 0.5, bg:getContentSize().height * 0.5)
	end
end