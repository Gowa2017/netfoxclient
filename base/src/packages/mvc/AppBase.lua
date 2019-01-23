
local AppBase = class("AppBase")

function AppBase:ctor(configs)
    self.configs_ = {
        viewsRoot  = "base.src.app.views",
        modelsRoot = "base.src.app.models",
        defaultSceneName = "WelcomeScene",
    }

    for k, v in pairs(configs or {}) do
        self.configs_[k] = v
    end

    if type(self.configs_.viewsRoot) ~= "table" then
        self.configs_.viewsRoot = {self.configs_.viewsRoot}
    end
    if type(self.configs_.modelsRoot) ~= "table" then
        self.configs_.modelsRoot = {self.configs_.modelsRoot}
    end

    if false == isDebug() then
        dump(self.configs_, "AppBase configs")
    end

    if CC_SHOW_FPS then
        cc.Director:getInstance():setDisplayStats(true)
    end

    -- event
    self:onCreate()
end

function AppBase:run(initSceneName)
    initSceneName = initSceneName or self.configs_.defaultSceneName
    self:enterScene(initSceneName)
end

function AppBase:enterScene(sceneName, transition, time, more)
    local view = self:createView(sceneName)
    view:showWithScene(transition, time, more)
    return view
end

function AppBase:enterSceneEx(sceneName, transition, time, more)
    local view = self:createViewEx(sceneName)
    view:showWithScene(transition, time, more)
    return view
end

function AppBase:createViewEx(name)

    local packageName = name
    local status, view = xpcall(function()
            return require(packageName)
        end, function(msg)
            if true == isDebug() then
                --确保先提示第一个错误
                if true == cc.exports.bHandlePopErrorMsg then
                    cc.exports.bHandlePopErrorMsg = false
                    nativeMessageBox(msg, "lua_src_error")
                end        
            end
            LogAsset:getInstance():logData(msg,true)
            if not string.find(msg, string.format("'%s' not found:", packageName)) then
                print("load view error: ", msg)            
            end
    end)
    local t = type(view)
    if status and (t == "table" or t == "userdata") then
        return view:create(self, name)
    end
    error(string.format("AppBase:createView() - not found view \"%s\" ",name) ,0)
end
function AppBase:createView(name)
    for _, root in ipairs(self.configs_.viewsRoot) do
        local packageName = string.format("%s.%s", root, name)
        local status, view = xpcall(function()
                return require(packageName)
            end, function(msg)
                if true == isDebug() then
                    --确保先提示第一个错误
                    if true == cc.exports.bHandlePopErrorMsg then
                        cc.exports.bHandlePopErrorMsg = false
                        nativeMessageBox(msg, "lua_src_error")
                    end        
                end
                LogAsset:getInstance():logData(msg,true)
                       
                if not string.find(msg, string.format("'%s' not found:", packageName)) then
                    print("load view error: ", msg)
                end
        end)
        local t = type(view)
        if status and (t == "table" or t == "userdata") then
            return view:create(self, name)
        end
    end
    error(string.format("AppBase:createView() - not found view \"%s\" in search paths \"%s\"",name, table.concat(self.configs_.viewsRoot, ",")), 0)
end

function AppBase:onCreate()
end

return AppBase
