--
-- Author: zhong
-- Date: 2016-07-29 12:01:42
--

--[[
* 通用扩展
]]
local ExternalFun = {}

--枚举声明
function ExternalFun.declarEnum( ENSTART, ... )
	local enStart = 1;
	if nil ~= ENSTART then
		enStart = ENSTART;
	end

	local args = {...};
	local enum = {};
	for i=1,#args do
		enum[args[i]] = enStart;
		enStart = enStart + 1;
	end

	return enum;
end

function ExternalFun.declarEnumWithTable( ENSTART, keyTable )
	local enStart = 1;
	if nil ~= ENSTART then
		enStart = ENSTART;
	end

	local args = keyTable;
	local enum = {};
	for i=1,#args do
		enum[args[i]] = enStart;
		enStart = enStart + 1;
	end

	return enum;
end

function ExternalFun.guid()
	math.randomseed(os.time())
	local seed = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}
	local tb = {}
	
	--填充前16位用户机器码
	local machine = GlobalUserItem.szMachine or ""
	if (nil ~= machine and "" ~= machine) then
		machine = string.sub(machine, 0, 16)
		machine = string.upper(machine)
		for i = 1, 16 do
			table.insert(tb, string.sub(machine, i, i))
		end
	else
		for i = 1, 16 do
			table.insert(tb, seed[math.random(1, 16)])
		end
	end
	
	--填充8位时间戳
	local timestamp = string.format("%X", os.time())
	for i = 1, 8 do
		table.insert(tb, string.sub(timestamp, i, i))
	end

	--填充8位随机码
	for i = 1, 8 do
		table.insert(tb, seed[math.random(1, 16)])
	end
	
	local sid=table.concat(tb)
	return sid
end

function ExternalFun.SAFE_RELEASE( var )
	if nil ~= var then
		var:release();
	end
end

function ExternalFun.SAFE_RETAIN( var )
	if nil ~= var then
		var:retain();
	end
end

function ExternalFun.enableBtn( btn, bEnable, bHide )
	if nil == bEnable then
		bEnable = false;
	end
	if nil == bHide then
		bHide = false;
	end

	btn:setEnabled(bEnable);
	if bEnable then
		btn:setVisible(true);
		btn:setOpacity(255);
	else
		if bHide then
			btn:setVisible(false);
		else
			btn:setOpacity(125);
		end
	end
end

--格式化长整形
function ExternalFun.formatScore( llScore )
	local str = string.formatNumberThousands(llScore);
	if string.len(str) >= 4 then
		str = string.sub(str, 1, -4);
		str = (string.gsub(str, ",", ""))
		return str;
	else
		return ""
	end	
end

--无小数点 NumberThousands
function ExternalFun.numberThousands( llScore )
	local str = string.formatNumberThousands(llScore);
	if string.len(str) >= 4 then
		return string.sub(str, 1, -4)
	else
		return ""
	end	
end

local debug_mode = nil
--读取网络消息
function ExternalFun.read_datahelper( param )
	if debug_mode then
		print("read: " .. param.strkey .. " helper");
	end
	
	if nil ~= param.lentable then		
		local lentable = param.lentable;
		local depth = #lentable;

		if debug_mode then
			print("depth:" .. depth);
		end
		
		local tmpT = {};
		for i=1,depth do
			local entryLen = lentable[i];
			if debug_mode then
				print("entryLen:" .. entryLen);
			end
			
			local entryTable = {};
			for i=1,entryLen do
				local entry = param.fun();
				if debug_mode then					
					if type(entry) == "boolean" then
						print("value:".. (entry and "true" or "false"))
					else
						print("value:" .. entry);
					end
				end

				table.insert(entryTable, entry);
			end					
			table.insert(tmpT, entryTable);
		end

		return tmpT;
	else
		if debug_mode then
			local value = param.fun();
			if type(value) == "boolean" then
				print("value:".. (value and "true" or "false"))
			else
				print("value:" .. value);
			end		
			return value;
		else
			return param.fun();
		end		
	end	
end

function ExternalFun.readTableHelper( param )
	local templateTable = param.dTable or {}
	local strkey = param.strkey or "default"
	if nil ~= param.lentable then		
		local lentable = param.lentable;
		local depth = #lentable;

		if debug_mode then
			print("depth:" .. depth);
		end
		
		local tmpT = {};
		for i=1,depth do
			local entryLen = lentable[i];
			if debug_mode then
				print("entryLen:" .. entryLen);
			end
			
			local entryTable = {};
			for i=1,entryLen do
				local entry = ExternalFun.read_netdata(templateTable, param.buffer)
				if debug_mode then					
					dump(entry, strkey .. " ==> " .. i)
				end

				table.insert(entryTable, entry);
			end					
			table.insert(tmpT, entryTable);
		end

		return tmpT
	else
		if debug_mode then
			local value = ExternalFun.read_netdata(templateTable, param.buffer)
			dump(value,strkey )	
			return value
		else
			return ExternalFun.read_netdata(templateTable, param.buffer)
		end		
	end	
end

--[[
******
* 结构体描述
* {k = "key", t = "type", s = len, l = {}}
* k 表示字段名,对应C++结构体变量名
* t 表示字段类型,对应C++结构体变量类型
* s 针对string变量特有,描述长度
* l 针对数组特有,描述数组长度,以table形式,一维数组表示为{N},N表示数组长度,多维数组表示为{N,N},N表示数组长度
* d 针对table类型,即该字段为一个table类型,d表示该字段需要读取的table数据
* ptr 针对数组,此时s必须为实际长度

** egg
* 取数据的时候,针对一维数组,假如有字段描述为 {k = "a", t = "byte", l = {3}}
* 则表示为 变量a为一个byte型数组,长度为3
* 取第一个值的方式为 a[1][1],第二个值a[1][2],依此类推

* 取数据的时候,针对二维数组,假如有字段描述为 {k = "a", t = "byte", l = {3,3}}
* 则表示为 变量a为一个byte型二维数组,长度都为3
* 则取第一个数组的第一个数据的方式为 a[1][1], 取第二个数组的第一个数据的方式为 a[2][1]
******
]]
--读取网络消息
function ExternalFun.read_netdata( keyTable, dataBuffer )
	local cmd_table = {};

	--辅助读取int64
    local int64 = Integer64.new();
	for k,v in pairs(keyTable) do
		local keys = v;

		------
		--读取数据
		--类型
		local keyType = string.lower(keys["t"]);
		--键
		local key = keys["k"];
		--长度
		local lenT = keys["l"];
		local keyFun = nil;
		if "byte" == keyType then
			keyFun = function() return dataBuffer:readbyte(); end
		elseif "int" == keyType then
			keyFun = function() return dataBuffer:readint(); end
		elseif "word" == keyType then
			keyFun = function() return  dataBuffer:readword(); end
		elseif "dword" == keyType then
			keyFun = function() return  dataBuffer:readdword(); end
		elseif "score" == keyType then
			keyFun = function() return  dataBuffer:readscore(int64):getvalue(); end
		elseif "string" == keyType then
			if nil ~= keys["s"] then
				keyFun = function() return  dataBuffer:readstring(keys["s"]); end
			else
				keyFun = function() return  dataBuffer:readstring(); end
			end	
		elseif "tchar" == keyType then
			if nil ~= keys["s"] then
				keyFun = function() return  dataBuffer:readstring(keys["s"]); end
			else
				keyFun = function() return  dataBuffer:readstring(); end
			end	
		elseif "bool" == keyType then
			keyFun = function() return  dataBuffer:readbool(); end
		elseif "table" == keyType then
			cmd_table[key] = ExternalFun.readTableHelper({dTable = keys["d"], lentable = lenT, buffer = dataBuffer, strkey = key})
		elseif "double" == keyType then
			keyFun = function() return  dataBuffer:readdouble(); end
		elseif "float" == keyType then
			keyFun = function() return  dataBuffer:readfloat(); end
		elseif "short" == keyType then
			keyFun = function() return  dataBuffer:readshort(); end
		else
			print("read_netdata error: key==>" .. key .. "; type==>" .. keyType);
		end
		if nil ~= keyFun then
			cmd_table[key] = ExternalFun.read_datahelper({strkey = key, lentable = lenT, fun = keyFun});
		end
	end
	return cmd_table;
end

--网络消息包大小
function ExternalFun.sizeof(keyTable)
	local netdatasize = nil
	local length = 0
	netdatasize = function(keyTable)
		for i=1,#keyTable do
			local keys = keyTable[i];
			local keyType = string.lower(keys["t"]);

			--todo 数组长度计算
			local keyLen = 0;
			if "byte" == keyType or "bool" == keyType then
				keyLen = 1;
			elseif "score" == keyType or "double" == keyType then
				keyLen = 8;
			elseif "word" == keyType or "short" == keyType then
				keyLen = 2;
			elseif "dword" == keyType or "int" == keyType or "float" == keyType then
				keyLen = 4;
			elseif "string" == keyType then
				keyLen = keys["s"];
			elseif "tchar" == keyType then
				keyLen = keys["s"] * 2
			elseif "ptr" == keyType then
				keyLen = keys["s"]
			elseif "table" == keyType then
				local subKeyDataTable = keys["d"]
				netdatasize(subKeyDataTable, length)
			else
				print("error keytype==>" .. keyType);
			end
			if type(keys.l) == "table"  then
				for _,v in ipairs(keys.l) do
					keyLen = keyLen * v
				end
			end
			length = length + keyLen;
		end
	end
	
	netdatasize(keyTable, 0)
	
	return length;
end


--创建网络消息包
function ExternalFun.create_netdata(keyTable)
	return CCmd_Data:create(ExternalFun.sizeof(keyTable))
end

--导入包
function ExternalFun.req_var( module_name )
	if (nil ~= module_name) and ("string" == type(module_name)) then
		return require(module_name);
	end
end

--加载界面根节点，设置缩放达到适配
function ExternalFun.loadRootCSB( csbFile, parent )
	local rootlayer = ccui.Layout:create()
		:setContentSize(1335,750) --这个是资源设计尺寸
		:setScale(yl.WIDTH / 1335);
	if nil ~= parent then
		parent:addChild(rootlayer);
	end

	local csbnode = cc.CSLoader:createNode(csbFile);
	rootlayer:addChild(csbnode);

	return rootlayer, csbnode;
end

--加载csb资源
function ExternalFun.loadCSB( csbFile, parent )
	local csbnode = cc.CSLoader:createNode(csbFile);
	if nil ~= parent then
		parent:addChild(csbnode);
	end
	return csbnode;	
end

--加载 帧动画
function ExternalFun.loadTimeLine( csbFile )
	return cc.CSLoader:createTimeline(csbFile);	 
end

--注册node事件
function ExternalFun.registerNodeEvent( node )
	if nil == node then
		return
	end
	local function onNodeEvent( event )
		if event == "enter" and nil ~= node.onEnter then
			node:onEnter()
		elseif event == "enterTransitionFinish" 
			and nil ~= node.onEnterTransitionFinish then
			node:onEnterTransitionFinish()
		elseif event == "exitTransitionStart" 
			and nil ~= node.onExitTransitionStart then
			node:onExitTransitionStart()
		elseif event == "exit" and nil ~= node.onExit then
			node:onExit()
		elseif event == "cleanup" and nil ~= node.onCleanup then
			node:onCleanup()
		end
	end

	node:registerScriptHandler(onNodeEvent)
end

--注册touch事件
function ExternalFun.registerTouchEvent( node, bSwallow )
	if nil == node then
		return false
	end
	local function onNodeEvent( event )
		if event == "enter" and nil ~= node.onEnter then
			node:onEnter()
		elseif event == "enterTransitionFinish" then
			--注册触摸
			local function onTouchBegan( touch, event )
				if nil == node.onTouchBegan then
					return false
				end
				return node:onTouchBegan(touch, event)
			end

			local function onTouchMoved(touch, event)
				if nil ~= node.onTouchMoved then
					node:onTouchMoved(touch, event)
				end
			end

			local function onTouchEnded( touch, event )
				if nil ~= node.onTouchEnded then
					node:onTouchEnded(touch, event)
				end       
			end

			local listener = cc.EventListenerTouchOneByOne:create()
			bSwallow = bSwallow or false
			listener:setSwallowTouches(bSwallow)
			node._listener = listener
		    listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN )
		    listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
		    listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED )
		    local eventDispatcher = node:getEventDispatcher()
		    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, node)

			if nil ~= node.onEnterTransitionFinish then
				node:onEnterTransitionFinish()
			end
		elseif event == "exitTransitionStart" 
			and nil ~= node.onExitTransitionStart then
			node:onExitTransitionStart()
		elseif event == "exit" and nil ~= node.onExit then	
			if nil ~= node._listener then
				local eventDispatcher = node:getEventDispatcher()
				eventDispatcher:removeEventListener(node._listener)
			end			

			if nil ~= node.onExit then
				node:onExit()
			end
		elseif event == "cleanup" and nil ~= node.onCleanup then
			node:onCleanup()
		end
	end
	node:registerScriptHandler(onNodeEvent)
	return true
end

local filterLexicon = {}
--加载屏蔽词库
function ExternalFun.loadLexicon( )
	local startTime = os.clock()
	local str = cc.FileUtils:getInstance():getStringFromFile("public/badwords.txt")

	if "{" ~= string.sub(str, 1, 1) or "}" ~= string.sub(str, -1, -1) then
		print("[WARN] load lexicon error!!!")
		return
	end
	str = "return" .. str
	local fuc = loadstring(str)
	
	if nil ~= fuc and type(fuc) == "function" then
		filterLexicon = fuc()
	end
	local endTime = os.clock()
	print("load time ==> " .. endTime - startTime)
end
ExternalFun.loadLexicon( )

--判断是否包含过滤词
function ExternalFun.isContainBadWords( str )
	local startTime = os.clock()

	print("origin ==> " .. str)
	--特殊字符过滤
	str = string.gsub(str, "[%w '|/?·`,;.~!@#$%^&*()-_。，、+]", "")
	print("gsub ==> " .. str)
	--是否直接为敏感字符
	local res = filterLexicon[str]
	--是否包含
	for k,v in pairs(filterLexicon)	do
		local b,e = string.find(str, k)
		if nil ~= b or nil ~= e then
			res = true
			break
		end
	end

	local endTime = os.clock()
	print("excute time ==> " .. endTime - startTime)

	return res ~= nil
end

--utf8字符串分割为单个字符
function ExternalFun.utf8StringSplit( str )
	local strTable = {}
	for uchar in string.gfind(str, "[%z\1-\127\194-\244][\128-\191]*") do
		strTable[#strTable+1] = uchar
	end
	return strTable
end

function ExternalFun.replaceAll(src, regex, replacement)
	return string.gsub(src, regex, replacement)
end

function ExternalFun.cleanZero(s)
	-- 如果传入的是空串则继续返回空串
    if"" == s then    
        return ""
    end

    -- 字符串中存在多个'零'在一起的时候只读出一个'零'，并省略多余的单位
    
    local regex1 = {"零仟", "零佰", "零拾"}
    local regex2 = {"零亿", "零万", "零元"}
    local regex3 = {"亿", "万", "元"}
    local regex4 = {"零角", "零分"}
    
    -- 第一轮转换把 "零仟", 零佰","零拾"等字符串替换成一个"零"
    for i = 1, 3 do    
        s = ExternalFun.replaceAll(s, regex1[i], "零")
    end

    -- 第二轮转换考虑 "零亿","零万","零元"等情况
    -- "亿","万","元"这些单位有些情况是不能省的，需要保留下来
    for i = 1, 3 do
        -- 当第一轮转换过后有可能有很多个零叠在一起
        -- 要把很多个重复的零变成一个零
        s = ExternalFun.replaceAll(s, "零零零", "零")
        s = ExternalFun.replaceAll(s, "零零", "零")
        s = ExternalFun.replaceAll(s, regex2[i], regex3[i])
    end

    -- 第三轮转换把"零角","零分"字符串省略
    for i = 1, 2 do
        s = ExternalFun.replaceAll(s, regex4[i], "")
    end

    -- 当"万"到"亿"之间全部是"零"的时候，忽略"亿万"单位，只保留一个"亿"
    s = ExternalFun.replaceAll(s, "亿万", "亿")
    
    --去掉单位
    --s = ExternalFun.replaceAll(s, "元", "")
    return s
end

--人民币阿拉伯数字转大写
function ExternalFun.numberTransiform(strCount)
	local big_num = {"零","壹","贰","叁","肆","伍","陆","柒","捌","玖"}
	local big_mt = {__index = function(o) 
		return ""
	end}
	
	setmetatable(big_num,big_mt)
	local zUnit = {"元", "拾", "佰", "仟", "万",
                  --拾万位到千万位
                  "拾", "佰", "仟",
                  --亿万位到万亿位
                  "亿", "拾", "佰", "仟", "万",}
	local dUnit = {"分", "角"}
    local unit_mt = {__index = function() return "" end }
    setmetatable(zUnit,unit_mt)
	setmetatable(dUnit,unit_mt)
    local tmp_str = ""
	local nInteger, nDecimal = math.modf(tonumber(strCount))
	
	if (nInteger > 0) then
		local strInteger = string.gsub(string.format("%0.2f", nInteger), "([%d]*)([%p])([%d]*).*", "%1")
		local zLen = string.len(strInteger)
		for i = 1, zLen do
			tmp_str = tmp_str .. big_num[string.byte(strInteger, i) - 47] .. zUnit[zLen - i + 1]
		end
	end
	
	if (nDecimal > 0.000001) then
		local strDecimal = string.gsub(string.format("%0.2f", nDecimal), "([%d]*)([%p])([%d]*).*", "%3")
		zLen = string.len(strDecimal)
		for i = 1, zLen do
			tmp_str = tmp_str .. big_num[string.byte(strDecimal, i) - 47] .. dUnit[zLen - i + 1]
		end
	end
	
    return ExternalFun.cleanZero(tmp_str)
end

--播放音效 (根据性别不同播放不同的音效)
function ExternalFun.playSoundEffect( path, useritem )
	local sound_path = path
	if nil == useritem then
		sound_path = "sound_res/" .. path
	else
		-- 0:女/1:男
		local gender = useritem.cbGender
		sound_path = string.format("sound_res/%d/%s", gender,path)
	end
	if GlobalUserItem.bSoundAble then
		AudioEngine.playEffect(sound_path,false)
	end	
end

function ExternalFun.playClickEffect( )
	if GlobalUserItem.bSoundAble then
		AudioEngine.playEffect(cc.FileUtils:getInstance():fullPathForFilename("sound/Click.wav"),false)
	end
end

--播放背景音乐
function ExternalFun.playBackgroudAudio( bgfile )
	local strfile = bgfile
	if nil == bgfile then
		strfile = "backgroud.wav"
	end
	strfile = "sound_res/" .. strfile
	if GlobalUserItem.bVoiceAble then
		AudioEngine.playMusic(strfile,true)
	end	
end

--播放大厅背景音乐
function ExternalFun.playPlazzBackgroudAudio( )
	if GlobalUserItem.bVoiceAble then
		AudioEngine.playMusic(cc.FileUtils:getInstance():fullPathForFilename("sound/plazaMusic.mp3"),true)
	end
end

--中文长度计算(同步pc,中文长度为2)
function ExternalFun.stringLen(szText)
	local len = 0
	local i = 1
	while true do
		local cur = string.sub(szText,i,i)
		local byte = string.byte(cur)
		if byte == nil then
			break
		end
		if byte > 128 then
			i = i + 3
			len = len + 2
		else
			i = i + 1
			len = len + 1
		end
	end
	return len
end

--webview 可见设置(已知在4s设备上设置可见会引发bug)
function ExternalFun.visibleWebView(webview, visible)
	if nil == webview then
		return
	end

	local target = cc.Application:getInstance():getTargetPlatform()
	if target == cc.PLATFORM_OS_IPHONE or target == cc.PLATFORM_OS_IPAD then
		local size = cc.Director:getInstance():getOpenGLView():getFrameSize()
		local con = math.max(size.width, size.height)
		if con ~= 960 then
	        webview:setVisible(visible)
	        return true
	    end
	else
		webview:setVisible(visible)
		return true
	end
	return false
end

-- 过滤emoji表情
-- 编码为 226 的emoji字符,不确定是否是某个中文字符
-- [%z\48-\57\64-\126\226-\233][\128-\191] 正则匹配式去除了226
function ExternalFun.filterEmoji(str)
	local newstr = ""
	print(string.byte(str))
	for unchar in string.gfind(str, "[%z\25-\57\64-\126\227-\240][\128-\191]*") do
		newstr = newstr .. unchar
	end
	print(newstr)
	return newstr
end

-- 判断是否包含emoji
-- 编码为 226 的emoji字符,不确定是否是某个中文字符
function ExternalFun.isContainEmoji(str)
	if nil ~= containEmoji then
		return containEmoji(str)
	end
	local origincount = string.utf8len(str)
	print("origin " .. origincount)
	local count = 0
	for unchar in string.gfind(str, "[%z\25-\57\64-\126\227-\240][\128-\191]*") do
		--[[print(string.len(unchar))
		print(string.byte(unchar))]]
		if string.len(unchar) < 4 then
			count = count + 1
		end		
	end
	print("newcount " .. count)
	return count ~= origincount
end

local TouchFilter = class("TouchFilter", function(showTime, autohide, msg)
		return display.newLayer(cc.c4b(0, 0, 0, 0))
	end)
function TouchFilter:ctor(showTime, autohide, msg)
	ExternalFun.registerTouchEvent(self, true)
	showTime = showTime or 2
	self.m_msgTime = showTime
	if autohide then			
		self:runAction(cc.Sequence:create(cc.DelayTime:create(showTime), cc.RemoveSelf:create(true)))
	end	
	self.m_filterMsg = msg
end

function TouchFilter:onTouchBegan(touch, event)
	return self:isVisible()
end

function TouchFilter:onTouchEnded(touch, event)
	print("TouchFilter:onTouchEnded")
	if type(self.m_filterMsg) == "string" and "" ~= self.m_filterMsg then
		showToast(self, self.m_filterMsg, self.m_msgTime)
	end
end

local TOUCH_FILTER_NAME = "__touch_filter_node_name__"
--触摸过滤
function ExternalFun.popupTouchFilter( showTime, autohide, msg, parent )
	local filter = TouchFilter:create(showTime, autohide, msg)
	local runScene = parent or cc.Director:getInstance():getRunningScene()
	if nil ~= runScene then
		local lastfilter = runScene:getChildByName(TOUCH_FILTER_NAME)
		if nil ~= lastfilter then
			lastfilter:stopAllActions()
			lastfilter:removeFromParent()
		end
		if nil ~= filter then
			filter:setName(TOUCH_FILTER_NAME)
			runScene:addChild(filter)
		end
	end
end

function ExternalFun.dismissTouchFilter()
	local runScene = cc.Director:getInstance():getRunningScene()
	if nil ~= runScene then
		local filter = runScene:getChildByName(TOUCH_FILTER_NAME)
		if nil ~= filter then
			filter:stopAllActions()
			filter:removeFromParent()
		end
	end
end

-- eg: 10000 转 1.0万
function ExternalFun.formatScoreText(score)
	local scorestr = ExternalFun.formatScore(score)
	if score < 10000 then
		return scorestr
	end

	if score < 100000000 then
		scorestr = string.format("%.2f万", score / 10000)
		return scorestr
	end
	scorestr = string.format("%.2f亿", score / 100000000)
	return scorestr
end

-- eg: 10000 转 1.0,万
function ExternalFun.formatScoreUnit(score)
	local scorestr = ExternalFun.formatScore(score)
	if score < 10000 then
		return scorestr
	end

	if score < 100000000 then
		scorestr = string.format("%.2f", score / 10000)
		return scorestr, "wan"
	end
	scorestr = string.format("%.2f", score / 100000000)
	return scorestr, "yi"
	
end

-- eg: 1 转 1.0  10000 转 1.0万
function ExternalFun.formatScoreFloatText(score)
	local scorestr = ExternalFun.formatScore(score)
	if score < 10000 then
		scorestr = string.format("%.2f", score)
		return scorestr
	end

	if score < 100000000 then
		scorestr = string.format("%.2f万", score / 10000)
		return scorestr
	end
	scorestr = string.format("%.2f亿", score / 100000000)
	return scorestr
end

-- 随机ip地址
local external_ip_long = 
{
	{ 607649792, 608174079 }, -- 36.56.0.0-36.63.255.255
    { 1038614528, 1039007743 }, -- 61.232.0.0-61.237.255.255
    { 1783627776, 1784676351 }, -- 106.80.0.0-106.95.255.255
    { 2035023872, 2035154943 }, -- 121.76.0.0-121.77.255.255
    { 2078801920, 2079064063 }, -- 123.232.0.0-123.235.255.255
    { -1950089216, -1948778497 }, -- 139.196.0.0-139.215.255.255
    { -1425539072, -1425014785 }, -- 171.8.0.0-171.15.255.255
    { -1236271104, -1235419137 }, -- 182.80.0.0-182.92.255.255
    { -770113536, -768606209 }, -- 210.25.0.0-210.47.255.255
    { -569376768, -564133889 }, -- 222.16.0.0-222.95.255.255
}
function ExternalFun.random_longip()
	local rand_key = math.random(1, 10)
	local bengin_long = external_ip_long[rand_key][1] or 0
	local end_long = external_ip_long[rand_key][2] or 0
	return math.random(bengin_long, end_long)
end

function ExternalFun.long2ip( value )
	if not value then
		return {p=0,m=0,s=0,b=0}
	end
	if nil == bit then
		print("not support bit module")
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

function ExternalFun.getChildRecursiveByName(node, name)
	local child = node:getChildByName(name)
	if nil ~= child then
		return child
	end
	local children = node:getChildren()
	print("childrenNumber:",#children)
	for i=1, #children do
		local result = ExternalFun.getChildRecursiveByName(children[i], name)
		if nil ~= result then
			return result
		end
	end
	return nil
end

--查找指定名称控件，返回第一个找到的控件
function ExternalFun.seekWigetByName(node, name)
	local child = node:getChildByName(name)
	if nil ~= child then
		return child
	end
	
	local children = node:getChildren()
	for i=1, #children do
		local result = ExternalFun.seekWigetByName(children[i], name)
		if nil ~= result then
			return result
		end
	end
	
	return nil
end

--查找指定名称控件，返回找到的控件列表
function ExternalFun.seekWigetByNameEx(node, name)
	local children = node:getChildren()
	
	local children_count = #children
	
	local findChild = {}
	
	--递归遍历子节点
	if (children_count ~= 0) then
		for i=1, children_count do
			local result = ExternalFun.seekWigetByNameEx(children[i], name)
			if 0 == #result then
				local childName = children[i]:getName()
				if (childName == name) then
					table.insert(findChild, children[i])
				end
			else
				for j = 1, #result do
					table.insert(findChild, result[j])
				end
			end
		end
	end
	
	return findChild
end

--查找指定Tag控件，返回第一个找到的控件
function ExternalFun.seekWigetByTag(node, tag)
	local child = node:getChildByTag(tag)
	if nil ~= child then
		return child
	end
	
	local children = node:getChildren()
	for i=1, #children do
		local result = ExternalFun.seekWigetByTag(children[i], tag)
		if nil ~= result then
			return result
		end
	end
	
	return nil
end

--查找指定名称控件，返回找到的控件列表
function ExternalFun.seekWigetByTagEx(node, tag)
	local children = node:getChildren()
	
	local children_count = #children
	
	local findChild = {}
	
	--递归遍历子节点
	if (children_count ~= 0) then
		for i=1, children_count do
			local result = ExternalFun.seekWigetByTagEx(children[i], tag)
			if 0 == #result then
				local childTag = children[i]:getTag()
				if (childTag == tag) then
					table.insert(findChild, children[i])
				end
			else
				for j = 1, #result do
					table.insert(findChild, result[j])
				end
			end
		end
	end
	
	return findChild
end

return ExternalFun