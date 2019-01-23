--
-- Author: zhong
-- Date: 2016-07-29 17:45:30
--
local Bridge_android = {}
local BRIDGE_CLASS = "org/cocos2dx/lua/AppActivity"
local luaj = require "cocos.cocos2d.luaj"

--获取设备id
function Bridge_android.getMachineId()
    local sigs = "()Ljava/lang/String;"    
    local ok,ret = luaj.callStaticMethod(BRIDGE_CLASS,"getUUID",{},sigs)
    if not ok then
        print("luaj error:" .. ret)
        return "A501164B366ECFC9E249163873094D50"
    else
        print("The ret is:" .. ret)
        return md5(ret)
    end
end

--获取设备ip
function Bridge_android.getClientIpAdress()
    local sigs = "()Ljava/lang/String;"
    local ok,ret = luaj.callStaticMethod(BRIDGE_CLASS,"getHostAdress",{},sigs)
    if not ok then
        print("luaj error:" .. ret)
        return "192.168.1.1"
    else
        print("The ret is:" .. ret)
        return ret
    end
end

--获取外部存储可写文档目录
function Bridge_android.getExtralDocPath()
    local sigs = "()Ljava/lang/String;"
    local ok,ret = luaj.callStaticMethod(BRIDGE_CLASS,"getSDCardDocPath",{},sigs)
    if not ok then
        print("luaj error:" .. ret)
        return device.writablePath
    else
        print("The ret is:" .. ret)
        return ret
    end
end

--选择图片
function Bridge_android.triggerPickImg(callback, needClip)
    needClip = needClip or false
    local args = { callback, needClip }
    if nil == callback or type(callback) ~= "function" then
    	print("user default callback fun")

    	local function callbackLua(param)
	        if type(param) == "string" then
	        	print(param)
	        end        
	    end
    	args = { callbackLua, needClip }
    end    
    
    local sigs = "(IZ)V"
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"pickImg",args,sigs)
    if not ok then
        print("luaj error:" .. ret)       
    end
end

--配置支付、登陆相关
function Bridge_android.thirdPartyConfig(thirdparty, configTab)
    local args = {thirdparty, cjson.encode(configTab)}
    local sigs = "(ILjava/lang/String;)V"
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"thirdPartyConfig",args,sigs)
    if not ok then
        print("luaj error:" .. ret)        
    end
end

function Bridge_android.configSocial(socialTab)
    local title = socialTab.title
    local content = socialTab.content
    local url = socialTab.url

    local args = {title, content, url}
    local sigs = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"socialShareConfig",args,sigs)
    if not ok then
        print("luaj error:" .. ret)        
    end
end

--第三方登陆
function Bridge_android.thirdPartyLogin(thirdparty, callback)
    local args = { thirdparty, callback }
    local sigs = "(II)V"
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"thirdLogin",args,sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

--分享
function Bridge_android.startShare(callback)
    local sigs = "(I)V"
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"startShare",{callback},sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

--自定义分享
function Bridge_android.customShare( title, content,url, img, imgOnly, callback )
    local sigs = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V"
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"customShare",{title, content, url, img, imgOnly, callback},sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

-- 分享到指定平台
function Bridge_android.shareToTarget( target, title, content, url, img, imgOnly, callback )
    local sigs = "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V"
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"shareToTarget",{target, title, content, url, img, imgOnly, callback},sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

--第三方支付
function Bridge_android.thirdPartyPay(thirdparty, payparamTab, callback)
    local args = { thirdparty, cjson.encode(payparamTab), callback }
    local sigs = "(ILjava/lang/String;I)V"
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"thirdPartyPay",args,sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

--获取竣付通支付列表
function Bridge_android.getPayList(token, callback)
    local args = { token, callback }
    local sigs = "(Ljava/lang/String;I)V" 
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"getPayList",args,sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

function Bridge_android.isPlatformInstalled(thirdparty)
    local args = { thirdparty }
    local sigs = "(I)Z" 
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"isPlatformInstalled",args,sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_android.saveImgToSystemGallery(filepath, filename)
    local args = { filepath, filename }
    local sigs = "(Ljava/lang/String;Ljava/lang/String;)Z" 
    local ok,ret = luaj.callStaticMethod(BRIDGE_CLASS,"saveImgToSystemGallery",args,sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_android.checkRecordPermission()
    local args = { }
    local sigs = "()Z" 
    local ok,ret = luaj.callStaticMethod(BRIDGE_CLASS,"isHaveRecordPermission",args,sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_android.requestLocation( callback )
    local args = { callback }
    local sigs = "(I)V" 
    local ok,ret = luaj.callStaticMethod(BRIDGE_CLASS,"requestLocation", args, sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_android.metersBetweenLocation( loParam )
    local args = { cjson.encode(loParam) }
    local sigs = "(Ljava/lang/String;)Ljava/lang/String;"
    local ok,ret = luaj.callStaticMethod(BRIDGE_CLASS,"metersBetweenLocation", args, sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_android.requestContact( callback )
    local args = { callback }
    local sigs = "(I)V" 
    local ok,ret = luaj.callStaticMethod(BRIDGE_CLASS,"requestContact", args, sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return ret
    end
end

function Bridge_android.openBrowser(url)
    local args = { url }
    local sigs = "(Ljava/lang/String;)V" 
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"openBrowser",args,sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return false, msg   
    else  
        return true     
    end
end

function Bridge_android.copyToClipboard( msg )
    local args = { msg }
    local sigs = "(Ljava/lang/String;)Z" 
    local ok,ret  = luaj.callStaticMethod(BRIDGE_CLASS,"copyToClipboard",args,sigs)
    if not ok then
        local msg = "luaj error:" .. ret
        print(msg)  
        return 0, msg   
    else  
        return ret     
    end
end

return Bridge_android