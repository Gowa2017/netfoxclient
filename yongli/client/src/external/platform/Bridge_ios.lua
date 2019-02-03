--
-- Author: zhong
-- Date: 2016-07-29 17:45:46
--
local Bridge_ios = {}

local luaoc = require "cocos.cocos2d.luaoc"
local BRIDGE_CLASS = "AppController"

--获取设备id
function Bridge_ios.getMachineId()
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"getUUID")
    if not ok then
        print("luaj error:" .. ret)
        return "A501164B366ECFC9E249163873094D50"
    else
        print("The ret is:" .. ret)
        return md5(ret)
    end
end

--获取设备ip
function Bridge_ios.getClientIpAdress()
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"getHostAdress")
    if not ok then
        print("luaj error:" .. ret)
        return "192.168.1.1"
    else
        print("The ret is:" .. ret)
        return ret
    end
end

--选择图片
function Bridge_ios.triggerPickImg( callback, needClip )
	needClip = needClip or false
    local args = { scriptHandler = callback, needClip = needClip }
    if nil == callback or type(callback) ~= "function" then
        print("user default callback fun")

        local function callbackLua(param)
            if type(param) == "string" then
                print(param)
            end        
        end
        args = { scriptHandler = callback, needClip = needClip }
    end    
    
    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"pickImg",args)
    if not ok then
        print("luaoc error:" .. ret)       
    end
end

--配置支付、登陆相关
function Bridge_ios.thirdPartyConfig(thirdparty, configTab)
    configTab._nidx = thirdparty
    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"thirdPartyConfig",configTab)
    if not ok then
        print("luaoc error:" .. ret)        
    end
end

function Bridge_ios.configSocial(socialTab)
    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"socialShareConfig",socialTab)
    if not ok then
        print("luaoc error:" .. ret)        
    end
end

--第三方登陆
function Bridge_ios.thirdPartyLogin(thirdparty, callback)
    local args = { _nidx = thirdparty, scriptHandler = callback }
    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"thirdLogin",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

--分享
function Bridge_ios.startShare(callback)
    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"startShare",{scriptHandler = callback})
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

function Bridge_ios.registerActiveStatusCallBack(callback)
    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"registerActiveStatusCallBack",{scriptHandler = callback})
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

--自定义分享
function Bridge_ios.customShare( title,content,url,img,imgOnly,callback )
    local t = 
    {
        title = title,
        content = content,
        url = url,
        img = img,
        imageOnly = imgOnly,
        scriptHandler = callback,
    }
    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"customShare",t)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

-- 分享到指定平台
function Bridge_ios.shareToTarget( target, title, content, url, img, imgOnly, callback )
    local t = 
    {
        target = target,
        title = title,
        content = content,
        url = url,
        img = img,
        imageOnly = imgOnly,
        scriptHandler = callback,
    }
    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"shareToTarget",t)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

--第三方支付
function Bridge_ios.thirdPartyPay(thirdparty, payparamTab, callback)
    payparamTab._nidx = thirdparty
    payparamTab.scriptHandler = callback
    payparamTab.info = cjson.encode(payparamTab.info)

    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"thirdPartyPay",payparamTab)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

--获取竣付通支付列表
function Bridge_ios.getPayList(token, callback)
    local paramtab = {token = token, scriptHandler = callback}
    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"getPayList",paramtab)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

function Bridge_ios.isPlatformInstalled(thirdparty)
    local paramtab = { _nidx = thirdparty }
    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"isPlatformInstalled",paramtab)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_ios.saveImgToSystemGallery(filepath, filename)
    local args = { _filepath = filepath, _filename = filename }
    local ok,ret  = luaoc.callStaticMethod(BRIDGE_CLASS,"saveImgToSystemGallery",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_ios.checkRecordPermission()
    local args = { }
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"isHaveRecordPermission",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_ios.requestLocation( callback )
    local paramtab = {scriptHandler = callback}
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"requestLocation",paramtab)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_ios.metersBetweenLocation( loParam )
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"metersBetweenLocation",loParam)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_ios.requestContact( callback )
    local paramtab = {scriptHandler = callback}
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"requestContact", paramtab)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_ios.openBrowser(url)
    local paramtab = {url = url}
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"openBrowser", paramtab)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_ios.copyToClipboard( msg )
    local paramtab = {msg = msg}
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"copyToClipboard", paramtab)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return 0, msg   
    else 
        print(ret)
        return ret
    end
end


function Bridge_ios.isOpenLocation()
    local args = { }
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"isOpenLocation",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_ios.presentServiceViewController( userid,signature,time,url, headurl)
    local args = 
    { 
                    userid = userid,
                    signature = signature,
                    time = time,
                    url = url,
					headurl = headurl
    }
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"presentServiceViewController",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

--获取电池当前的状态，共有4种状态
function Bridge_ios.getBatteryState()
    local args = { }
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"getBatteryState",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

--获取精准电池电量
function Bridge_ios.getCurrentBatteryLevel()
    local args = { }
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"getCurrentBatteryLevel",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

--获取精准电池电量,返回 float 类型
function Bridge_ios.getBatteryLevel()
    local args = { }
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"getBatteryLevel",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

--WIFI信号强度，返回 int 类型
function Bridge_ios.getSignalStrength()
    local args = { }
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"getSignalStrength",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

--网络信号类型，返回 string 参数："无网络"、"2G"、3G"、"4G"、"wifi"、"无法获取当前网络"
function Bridge_ios.getNetWorkStates()
    local args = { }
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"getNetWorkStates",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

--设备是否允许远程通知，返回 bool  
function Bridge_ios.isOpenRemoteNotification()
    local args = { }
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"isOpenRemoteNotification",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

--设置极光推送别名,传入参数 ：userid ，用于给该“userid”的用户 单独推送 远程通知消息 
function Bridge_ios.setAlias( account )
    local tstr = string.urlencode(account)
    local args = { tstr }
    local args = { account = account }
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"setAlias",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

--删除极光推送别名，退出登录时，要删除 极光推送别名，这样才能切换通知用户
function Bridge_ios.deleteAlias()
    local args = { }
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"deleteAlias",args)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

--极光推送 回调函数 注册， callback 传入回调函数名
function Bridge_ios.registerRemoteNotificationCallBack( callback )
    local paramtab = {scriptHandler = callback}
    local ok,ret = luaoc.callStaticMethod(BRIDGE_CLASS,"registerRemoteNotificationCallBack",paramtab)
    if not ok then
        local msg = "luaoc error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

--删除极光推送别名，退出登录时，要删除 极光推送别名，这样才能切换通知用户
function Bridge_ios.openQQ()
    cc.Application:getInstance():openURL("mqq://")
    return true   
end

return Bridge_ios