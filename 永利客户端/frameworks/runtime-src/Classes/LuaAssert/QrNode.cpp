//
//  QrNode.cpp
//  GameProject
//
//  Created by zhong on 16/10/09.
//
//

#include "QrNode.h"
#include "ui/CocosGUI.h"
#if CC_ENABLE_SCRIPT_BINDING
#include "CCLuaEngine.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#endif

static const std::string QR_NODE_NAME = "_@@_qr_node_name_@@_";
QrNode::~QrNode()
{
    m_vecQrFileChild.clear();
}

QrNode* QrNode::createQrNode(const std::string &content,
                             const int &nNodeSize,
                             const int &nQrSize /*= 5*/,
                             const int &nLevel /*= 0*/,
                             const int &nVersion /*= 0*/,
                             const bool &bAutoExtent /*= true*/,
                             const int &nMaskingNo /*= -1*/)
{
    QrNode *qr = new QrNode();
    if (nullptr != qr && qr->initQrNode(content, nNodeSize, nQrSize, nLevel, nVersion, bAutoExtent, nMaskingNo))
    {
        qr->autorelease();
        return qr;
    }
    CC_SAFE_DELETE(qr);
    return nullptr;
}

void onCaptureScreenArea(const std::function<void(bool, const std::string&)>& afterCaptured, const std::string& filename, const Rect &area)
{
    static bool startedCapture = false;
    
    if (startedCapture)
    {
        CCLOG("Screen capture is already working");
        if (afterCaptured)
        {
            afterCaptured(false, filename);
        }
        return;
    }
    else
    {
        startedCapture = true;
    }
    
    int width = area.size.width;
    int height = area.size.height;
    
    bool succeed = false;
    std::string outputFile = "";
    
    do
    {
        std::shared_ptr<GLubyte> buffer(new GLubyte[width * height * 4], [](GLubyte* p){ CC_SAFE_DELETE_ARRAY(p); });
        if (!buffer)
        {
            break;
        }
        
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(area.origin.x, area.origin.y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.get());
        
        std::shared_ptr<GLubyte> flippedBuffer(new GLubyte[width * height * 4], [](GLubyte* p) { CC_SAFE_DELETE_ARRAY(p); });
        if (!flippedBuffer)
        {
            break;
        }
        
        for (int row = 0; row < height; ++row)
        {
            memcpy(flippedBuffer.get() + (height - row - 1) * width * 4, buffer.get() + row * width * 4, width * 4);
        }
        
        Image* image = new (std::nothrow) Image;
        if (image)
        {
            image->initWithRawData(flippedBuffer.get(), width * height * 4, width, height, 8);
            if (FileUtils::getInstance()->isAbsolutePath(filename))
            {
                outputFile = filename;
            }
            else
            {
                CCASSERT(filename.find("/") == std::string::npos, "The existence of a relative path is not guaranteed!");
                outputFile = FileUtils::getInstance()->getWritablePath() + filename;
            }
            
            // Save image in AsyncTaskPool::TaskType::TASK_IO thread, and call afterCaptured in mainThread
            static bool succeedSaveToFile = false;
            std::function<void(void*)> mainThread = [afterCaptured, outputFile](void* param)
            {
                if (afterCaptured)
                {
                    afterCaptured(succeedSaveToFile, outputFile);
                }
                startedCapture = false;
            };
            
            AsyncTaskPool::getInstance()->enqueue(AsyncTaskPool::TaskType::TASK_IO, mainThread, (void*)NULL, [image, outputFile]()
                                                  {
                                                      succeedSaveToFile = image->saveToFile(outputFile);
                                                      delete image;
                                                  });
        }
        else
        {
            CCLOG("Malloc Image memory failed!");
            if (afterCaptured)
            {
                afterCaptured(succeed, outputFile);
            }
            startedCapture = false;
        }
    } while (0);
}

/*
 * Capture screen interface
 */
static EventListenerCustom* s_captureScreenListener;
static CustomCommand s_captureScreenCommand;
void captureScreenArea(const std::function<void(bool, const std::string&)>& afterCaptured, const std::string& filename, const Rect &area)
{
    if (s_captureScreenListener)
    {
        CCLOG("Warning: CaptureScreen has been called already, don't call more than once in one frame.");
        return;
    }
    s_captureScreenCommand.init(std::numeric_limits<float>::max());
    s_captureScreenCommand.func = std::bind(onCaptureScreenArea, afterCaptured, filename, area);
    s_captureScreenListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(Director::EVENT_AFTER_DRAW, [](EventCustom *event) {
        auto director = Director::getInstance();
        director->getEventDispatcher()->removeEventListener((EventListener*)(s_captureScreenListener));
        s_captureScreenListener = nullptr;
        director->getRenderer()->addCommand(&s_captureScreenCommand);
        director->getRenderer()->render();
    });
}

bool QrNode::saveQrToFile(const std::string &filename, const int &nLuaCallFunC)
{
    if (0 == nLuaCallFunC)
    {
        return false;
    }
    auto size = this->getContentSize();
    auto qr = this->drawQr();
    auto node = Node::create();
    node->addChild(qr);
    qr->setPositionY(m_nQrOriginSize);
    node->setScale(m_fScaleRate);
    
    auto render = RenderTexture::create(size.width, size.height);
    render->retain();
    render->beginWithClear(0, 0, 0, 0);
    node->visit();
    render->end();
    Director::getInstance()->getRenderer()->render();
    return render->saveToFile(filename, true, [=](RenderTexture* render, const std::string& fullpath)
                              {
#if CC_ENABLE_SCRIPT_BINDING
                                  lua_State* tolua_S=LuaEngine::getInstance()->getLuaStack()->getLuaState();
                                  toluafix_get_function_by_refid(tolua_S, nLuaCallFunC);
                                  int result = 0;
                                  if (lua_isfunction(tolua_S, -1))
                                  {
                                      lua_pushstring(tolua_S, fullpath.c_str());
                                      result = LuaEngine::getInstance()->getLuaStack()->executeFunctionByHandler(nLuaCallFunC, 1);
                                  }
                                  if (result)
                                  {
                                      CCLOG("saveQrToFile-luacallback-handler-false:%d",nLuaCallFunC);
                                  }
#endif
                                  render->release();
                              });
}

bool QrNode::initQrNode(const std::string &content,
                        const int &nNodeSize,
                        const int &nQrSize /*= 5*/,
                        const int &nLevel /*= 0*/,
                        const int &nVersion /*= 0*/,
                        const bool &bAutoExtent /*= true*/,
                        const int &nMaskingNo /*= -1*/)
{
    bool bRes = false;
    do
    {
        m_strContent = content;
        m_nQrSize = nQrSize;
        m_nNodeSize = nNodeSize;
        if (m_QREncode.EncodeData(nLevel, nVersion, bAutoExtent, nMaskingNo, (char*)content.c_str()))
        {
            Node *qrNode = this->drawQr();
            if (nullptr != qrNode)
            {          
                m_nQrOriginSize = (m_QREncode.m_nSymbleSize + QR_MARGIN * 2) * m_nQrSize;
                m_fScaleRate = (float)nNodeSize / m_nQrOriginSize;
                
                this->setScale(m_fScaleRate);
                this->setContentSize(Size(m_fScaleRate * m_nQrOriginSize, m_fScaleRate * m_nQrOriginSize));
                this->addChild(qrNode);
                qrNode->setName(QR_NODE_NAME);
                qrNode->setPosition(-m_nQrOriginSize * 0.5, m_nQrOriginSize * 0.5);
                bRes = true;
            }
        }
    } while (false);
    return bRes;
}

Node* QrNode::drawQr()
{
    int nSize = m_nQrSize;
    DrawNode *pQRNode = DrawNode::create();
    for (int i = 0; i < m_QREncode.m_nSymbleSize; ++i)
    {
        for (int j = 0; j < m_QREncode.m_nSymbleSize; ++j)
        {
            if (m_QREncode.m_byModuleData[i][j] == 1)
            {
                pQRNode->drawSolidRect(Vec2((i + QR_MARGIN) * nSize, (j + QR_MARGIN) * nSize), Vec2(((i + QR_MARGIN) + 1)*nSize, ((j + QR_MARGIN) + 1)*nSize), Color4F(0, 0, 0, 1));
            }
            else
            {
                pQRNode->drawSolidRect(Vec2((i + QR_MARGIN)*nSize, (j + QR_MARGIN)*nSize), Vec2(((i + QR_MARGIN) + 1)*nSize, ((j + QR_MARGIN) + 1)*nSize), Color4F(1, 1, 1, 1));
            }
        }
    }
    pQRNode->drawSolidRect(Vec2(0, 0), Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN * 2) * nSize, (QR_MARGIN) * nSize), Color4F(1, 1, 1, 1));
    pQRNode->drawSolidRect(Vec2(0, 0), Vec2((QR_MARGIN) * nSize, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2) * nSize), Color4F(1, 1, 1, 1));
    pQRNode->drawSolidRect(Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN) * nSize, 0), Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN * 2) * nSize, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2) * nSize), Color4F(1, 1, 1, 1));
    pQRNode->drawSolidRect(Vec2(0, (m_QREncode.m_nSymbleSize + QR_MARGIN) * nSize), Vec2((m_QREncode.m_nSymbleSize + QR_MARGIN * 2) * nSize, (m_QREncode.m_nSymbleSize + QR_MARGIN * 2) * nSize), Color4F(1, 1, 1, 1));
    pQRNode->setScaleY(-1);        

    return pQRNode;
}

#if CC_ENABLE_SCRIPT_BINDING
static int toLua_QrNode_createQrNode(lua_State *tolua_S)
{
    int argc = lua_gettop(tolua_S);
    QrNode *obj = nullptr;
    std::string content = "";
    int nodesize = 0;
    int qrsize = 5;
    int level = 0;
    int version = 0;
    bool autoextent = true;
    int maskingno = -1;
    if (argc >= 3)
    {
        content = lua_tostring(tolua_S, 2);
        nodesize = (int)lua_tonumber(tolua_S, 3);
        if (argc >= 4)
        {
            qrsize = (int)lua_tonumber(tolua_S, 4);
        }
        if (argc >= 5)
        {
            level = (int)lua_tonumber(tolua_S, 5);
        }
        if (argc >= 6)
        {
            version = (int)lua_tonumber(tolua_S, 6);
        }
        if (argc >= 7)
        {
            autoextent = (lua_toboolean(tolua_S, 7) == 1);
        }
        if (argc >= 8)
        {
            maskingno = (int)lua_tonumber(tolua_S, 8);
        }
        obj = QrNode::createQrNode(content, nodesize, qrsize, level, version, autoextent, maskingno);
    }
    int nID = (nullptr != obj) ? obj->_ID : -1;
    int *pLuaID = (nullptr != obj) ? &obj->_luaID : nullptr;
    toluafix_pushusertype_ccobject(tolua_S,nID,pLuaID,(void*)obj, "cc.QrNode");
    return 1;
}

static int toLua_QrNode_saveQrToFile(lua_State *tolua_S)
{
    bool bRes = false;
    int argc = lua_gettop(tolua_S);
    if (argc == 3)
    {
        QrNode *obj = (QrNode*)tolua_tousertype(tolua_S, 1, nullptr);
        if (nullptr != obj)
        {
            std::string filename = lua_tostring(tolua_S, 2);
            int handler = toluafix_ref_function(tolua_S, 3, 0);
            bRes = obj->saveQrToFile(filename, handler);
        }
    }
    lua_pushboolean(tolua_S, bRes);
    return 1;
}

static int toLua_captureScreenWithArea(lua_State *tolua_S)
{
    bool bRes = false;
    int argc = lua_gettop(tolua_S);
    if (argc == 3)
    {
        cocos2d::Rect arg0;
        bRes &= luaval_to_rect(tolua_S, 1, &arg0, "captureScreenWithArea");
        std::string filename = lua_tostring(tolua_S, 2);
        int handler = toluafix_ref_function(tolua_S, 3, 0);
        
        if (0 != handler)
        {
            captureScreenArea([=](bool ok, const std::string& savepath)
                              {
                                  toluafix_get_function_by_refid(tolua_S, handler);
                                  int result = 0;
                                  if (lua_isfunction(tolua_S, -1))
                                  {
                                      lua_pushboolean(tolua_S, ok);
                                      lua_pushstring(tolua_S, savepath.c_str());
                                      result = LuaEngine::getInstance()->getLuaStack()->executeFunctionByHandler(handler, 2);
                                  }
                                  if (result)
                                  {
                                      CCLOG("saveQrToFile-luacallback-handler-false:%d",handler);
                                  }
                              }, filename, arg0);
        }
    }
    lua_pushboolean(tolua_S, bRes);
    return 1;
}
#endif

int register_all_QrNode()
{
#if CC_ENABLE_SCRIPT_BINDING
    lua_State* tolua_S = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    
    tolua_usertype(tolua_S, "cc.QrNode");
    tolua_cclass(tolua_S,"QrNode","cc.QrNode","cc.Node",nullptr);
    
    tolua_beginmodule(tolua_S,"QrNode");
    tolua_function(tolua_S,"createQrNode",toLua_QrNode_createQrNode);
    tolua_function(tolua_S,"saveQrToFile",toLua_QrNode_saveQrToFile);
    tolua_endmodule(tolua_S);
    
    //全局函数
    lua_register(tolua_S,"captureScreenWithArea",toLua_captureScreenWithArea);
    std::string typeName = typeid(QrNode).name();
    g_luaType[typeName] = "cc.QrNode";
    g_typeCast["QrNode"] = "cc.QrNode";
#endif
    return 1;
}