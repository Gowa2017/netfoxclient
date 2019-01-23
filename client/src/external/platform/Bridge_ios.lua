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

return Bridge_ios