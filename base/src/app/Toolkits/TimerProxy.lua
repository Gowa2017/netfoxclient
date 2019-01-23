--[[
	定时器Proxy,用于处理游戏中需要定时操作的逻辑
	提供了添加和移除定时器的接口
--]]

local TimerProxy = class("TimerProxy")

local TAG = "<TimerProxy | "

local _scheduler = cc.Director:getInstance():getScheduler()

local TIMER_STATUS = {
	INIT = "timer_init";
	DELAY = "timer_delay";
	RUNNING = "timer_running";
	FINISH = "timer_finished";
}

function TimerProxy:ctor()
	-- body
	self._timers = {}

	local timer_init_, timer_delay_, timer_running_; --[[, timer_finish_--]]

	timer_init_ = function (timer, dt)
		-- body
		if timer.delay then
			timer._status = TIMER_STATUS.DELAY
			timer._elapseDelay = timer.delay
			return timer_delay_(timer, dt)
		end

		timer._status = TIMER_STATUS.RUNNING
		timer._elapse = timer.interval
		return timer_running_(timer, dt)
	end

	timer_delay_ = function (timer, dt)
		-- body
		timer._elapseDelay = timer._elapseDelay - dt
		if timer._elapseDelay > 0.0 then
			return
		end

		timer._status = TIMER_STATUS.RUNNING
		timer._elapse = timer.interval
		-- timer_running_(timer, dt)
	end

	timer_running_ = function (timer, dt)
		-- body
		timer._elapse = timer._elapse - dt
		if timer._elapse > 0.0 then
			return
		end

		timer._elapse = timer.interval
		timer._usedTimes = timer._usedTimes + 1

		if timer.callback then
			timer.callback(timer.tag)
		end

		if timer._usedTimes >= timer.times then
			timer._status = TIMER_STATUS.FINISH
			-- return timer_finish_(timer, dt)
		end
	end

	--[[
	timer_finish_ = function (timer, dt)
		-- body
		if self._timers[timer.tag] then
			self._timers[timer.tag] = nil
		end
	end
	--]]

	local function timer_validate_()
		-- body
		for k, v in pairs(self._timers) do
			if v._status == TIMER_STATUS.FINISH then
				self._timers[k] = nil
			end
		end
	end

	local function timer_update_(timer, dt)
		-- body
		if timer._status == TIMER_STATUS.INIT then
			return timer_init_(timer, dt)
		end		

		if timer._status == TIMER_STATUS.DELAY then
			return timer_delay_(timer, dt)
		end

		if timer._status == TIMER_STATUS.RUNNING then
			return timer_running_(timer, dt)
		end

		if timer._status == TIMER_STATUS.FINISH then
			-- nothing to do
			-- return timer_finish_(timer, dt)
			return
		end

		print(TAG .. "Timer_update_ - timer status error.")		
	end

	local function scheduleUpdate_(dt)
		-- body
		for _, v in pairs(self._timers) do
			-- print(TAG .. "scheduleUpdate_ - called." .. dt)
			timer_update_(v, dt)
		end

		timer_validate_()
	end
	self._schedulerEntry = _scheduler:scheduleScriptFunc(scheduleUpdate_, 0.0, false)
end

--[[
	添加定时器
	timerTag - 定时器标识，建议使用字符串进行标识
	callback - 定时器回调函数
	interval - 定时器间隔时间，默认(不填时)为0.0的间隔，即一帧执行一次
	times - 定时器重复定数，0次为无效定时器，负数次则为无限重复定时器，默认(不填时)为1次
	delay - 延时多长时间开始定时，默认(不填时)为立即开始
--]]
function TimerProxy:addTimer(timerTag, callback, interval, times, delay)
	-- body
	assert(timerTag, "timerTag is error.")
	assert(type(callback) == "function", "Timer callback error.")

	if self._timers[timerTag] then
		print(TAG .. "addTimer - timerTag is existed.")
		return false
	end

	interval = interval or 0.0
	interval = tonumber(interval)
	if interval < 0.0 then
		interval = 0.0
	end

	times = times or 1
	times = math.ceil(tonumber(times))
	if times == 0 then
		print(TAG .. "addTimer - times error with 0")
		return
	end

	if times < 0 then
		times = math.huge
	end

	delay = delay or false
	if delay ~= false then
		delay = tonumber(delay)
		if delay < 0.0 then
			delay = false
		end
	end

	self._timers[timerTag] = {
		tag = timerTag; 

		interval = interval; 
		times = times; 
		callback = callback; 
		delay = delay;

		_status = TIMER_STATUS.INIT;
		_elapse = 0;
		_usedTimes = 0;
		_elapseDelay = 0;
	}
	return true
end

--[[
	移除指定的定时器
	timerTag - 调用addTimer时传入的timerTag值
--]]
function TimerProxy:removeTimer(timerTag)
	-- body
	if self._timers[timerTag] then
		self._timers[timerTag] = nil
	end
end

--[[
	查询指定的定时器是否有效
	timerTag - 调用addTimer时传入的timerTag值
--]]
function TimerProxy:hasTimer(timerTag)
	-- body
	if self._timers[timerTag] then
		return true
	end

	return false
end

function TimerProxy:finalizer()
	-- body
	_scheduler:unscheduleScriptEntry(self._schedulerEntry)
end

cc.exports.TimerProxy = cc.exports.TimerProxy or TimerProxy:create()
return cc.exports.TimerProxy