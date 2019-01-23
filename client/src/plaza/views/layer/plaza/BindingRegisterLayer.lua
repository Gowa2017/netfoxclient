--
-- Author: zhong
-- Date: 2016-10-31 15:49:33
--

local ModifyFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ModifyFrame")
local NotifyMgr = appdf.req(appdf.EXTERNAL_SRC .. "NotifyMgr")
local BindingRegisterLayer = class("BindingRegisterLayer",function(scene)
        local lay =  display.newLayer()
    return lay
end)

BindingRegisterLayer.BT_REGISTER = 1
BindingRegisterLayer.BT_RETURN   = 2
BindingRegisterLayer.BT_AGREEMENT= 3
BindingRegisterLayer.CBT_AGREEMENT = 4

BindingRegisterLayer.bAgreement = false
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

function BindingRegisterLayer:ctor(scene)
    local this = self
    self._scene = scene

    local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    --网络回调
    local modifyCallBack = function(result,message)
        this:onModifyCallBack(result,message)
    end
    --网络处理
    self._modifyFrame = ModifyFrame:create(self,modifyCallBack)

    --Top背景
    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,yl.HEIGHT-51)
        self:addChild(sp)
    end

    --Top标题
    display.newSprite("Regist/title_regist.png")
        :move(yl.WIDTH/2,yl.HEIGHT-51)
        :addTo(self)

    --Top返回
    ccui.Button:create("bt_return_0.png","bt_return_1.png")
        :setTag(BindingRegisterLayer.BT_RETURN)
        :move(75,yl.HEIGHT-51)
        :addTo(self)
        :addTouchEventListener(btcallback)

    --注册背景框
    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,320)
        self:addChild(sp)
    end

    --帐号提示
    display.newSprite("Regist/icon_regist_tip.png")
        :move(293,540)
        :addTo(self)
    display.newSprite("Regist/text_regist_account.png")
        :move(392,540)
        :addTo(self)

    --账号输入
    self.edit_Account = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Regist/text_field_regist.png"))
        :move(730,540)
        :setAnchorPoint(cc.p(0.5,0.5))
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(31)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("6-31位字符")
        :addTo(self)

    --密码提示
    display.newSprite("Regist/icon_regist_tip.png")
        :move(293,450)
        :addTo(self)
    display.newSprite("Regist/text_regist_password.png")
        :move(392,450)
        :addTo(self)

    --密码输入  
    self.edit_Password = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Regist/text_field_regist.png"))
        :move(730,450)
        :setAnchorPoint(cc.p(0.5,0.5))
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(26)
        :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("6-26位英文字母，数字，下划线组合")
        :addTo(self)

    --确认密码提示
    display.newSprite("Regist/icon_regist_tip.png")
        :move(293,358)
        :addTo(self)
    display.newSprite("Regist/text_regist_confirm.png")
        :move(392,358)
        :addTo(self)

    --确认密码输入    
    self.edit_RePassword = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Regist/text_field_regist.png"))
        :move(730,358)
        :setAnchorPoint(cc.p(0.5,0.5))
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(26)
        :setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
        :setPlaceHolder("6-26位英文字母，数字，下划线组合")
        :addTo(self)

    --推广员
    display.newSprite("Regist/text_regist_tuiguang.png")
        :move(392,268)
        :addTo(self)

    --推广员   
    self.edit_Spreader = ccui.EditBox:create(cc.size(490,67), ccui.Scale9Sprite:create("Regist/text_field_regist.png"))
        :move(730,268)
        :setAnchorPoint(cc.p(0.5,0.5))
        :setFontName("fonts/round_body.ttf")
        :setPlaceholderFontName("fonts/round_body.ttf")
        :setFontSize(24)
        :setPlaceholderFontSize(24)
        :setMaxLength(32)
        :setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)--setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
        :setPlaceHolder("请输入推广员帐号")
        :addTo(self)

    --条款协议
    self.cbt_Agreement = ccui.CheckBox:create("Regist/choose_regist_0.png","","Regist/choose_regist_1.png","","")
        :move(510,183)
        :setSelected(BindingRegisterLayer.bAgreement)
        :setTag(BindingRegisterLayer.CBT_AGREEMENT)
        :addTo(self)

    --显示协议
    ccui.Button:create("Regist/bt_regist_agreement.png","")
        :setTag(BindingRegisterLayer.BT_AGREEMENT)
        :move(780,181)
        :addTo(self)
        :addTouchEventListener(btcallback)

    --注册按钮
    ccui.Button:create("Regist/bt_regist_0.png","")
        :setTag(BindingRegisterLayer.BT_REGISTER)
        :move(yl.WIDTH/2,93)
        :addTo(self)
        :addTouchEventListener(btcallback)

    --条款界面
    self._serviceView = nil
end


function BindingRegisterLayer:onButtonClickedEvent(tag,ref)
    if tag == BindingRegisterLayer.BT_RETURN then
        self._scene:onKeyBack()
    elseif tag == BindingRegisterLayer.BT_AGREEMENT then
        if self._serviceView == nil then
            self._serviceView = ServiceLayer:create()
                :move(yl.WIDTH,0)
                :addTo(self._backLayer)
        else
            self._serviceView:stopAllActions()
        end
        self._serviceView:runAction(cc.MoveTo:create(0.3,cc.p(0,0)))
    elseif tag == BindingRegisterLayer.BT_REGISTER then
        local szAccount = string.gsub(self.edit_Account:getText(), " ", "")
        local szPassword = string.gsub(self.edit_Password:getText(), " ", "")
        local szRePassword = string.gsub(self.edit_RePassword:getText(), " ", "")

        local len = ExternalFun.stringLen(szAccount)--#szAccount
        if len < 6 or len > 31 then
            showToast(self,"游戏帐号必须为6~31个字符，请重新输入！",2,cc.c4b(250,0,0,255));
            return
        end

        --判断emoji
        if ExternalFun.isContainEmoji(szAccount) then
            showToast(self, "帐号包含非法字符,请重试", 2)
            return
        end

        --判断是否有非法字符
        if true == ExternalFun.isContainBadWords(szAccount) then
            showToast(self, "帐号中包含敏感字符,不能注册", 2)
            return
        end

        len = ExternalFun.stringLen(szPassword)
        if len < 6 or len > 26 then
            showToast(self,"密码必须为6~26个字符，请重新输入！",2,cc.c4b(250,0,0,255));
            return
        end 

        if szPassword ~= szRePassword then
            showToast(self,"二次输入密码不一致，请重新输入！",2,cc.c4b(250,0,0,255));
            return
        end

        -- 与帐号不同
        if string.lower(szPassword) == string.lower(szAccount) then
            showToast(self,"密码不能与帐号相同，请重新输入！",2,cc.c4b(250,0,0,255));
            return
        end

        --[[-- 首位为字母
        if 1 ~= string.find(szPassword, "%a") then
            showToast(self,"密码首位必须为字母，请重新输入！",2,cc.c4b(250,0,0,255));
            return
        end]]

        local bAgreement = self:getChildByTag(BindingRegisterLayer.CBT_AGREEMENT):isSelected()
        if bAgreement == false then
            showToast(self,"请先阅读并同意《游戏中心服务条款》！",2,cc.c4b(250,0,0,255));
            return
        end        
                
        local szSpreader = string.gsub(self.edit_Spreader:getText(), " ", "")
        self._scene:showPopWait()
        self._modifyFrame:onAccountRegisterBinding(szAccount,md5(szPassword),szSpreader)
        self.szAccount = szAccount
        self.szPassword = szPassword
    end
end

function BindingRegisterLayer:setAgreement(bAgree)
    self.cbt_Agreement:setSelected(bAgree)
end

--操作结果
function BindingRegisterLayer:onModifyCallBack(result,message)
    print("======== BindingRegisterLayer::onModifyCallBack ========")

    self._scene:dismissPopWait()
    if  message ~= nil and message ~= "" then
        showToast(self,message,2);
    end

    if result == 2 then
        self._scene:showPopWait()
        GlobalUserItem.setBindingAccount()
        GlobalUserItem.szPassword = self.szPassword
        GlobalUserItem.szAccount = self.szAccount
        --保存数据
        GlobalUserItem.onSaveAccountConfig()

        self:runAction(cc.Sequence:create(cc.DelayTime:create(1.0), cc.CallFunc:create(function ()
            self._scene:dismissPopWait()
                  
            --重新登录
            GlobalUserItem.nCurRoomIndex = -1
            self:getParent():getParent():getApp():enterSceneEx(appdf.CLIENT_SRC.."plaza.views.LogonScene","FADE",1)
            GlobalUserItem.reSetData()
            --读取配置
            GlobalUserItem.LoadData()
            --断开好友服务器
            FriendMgr:getInstance():reSetAndDisconnect()
            --通知管理
            NotifyMgr:getInstance():clear()
            end)))
    end
end

return BindingRegisterLayer