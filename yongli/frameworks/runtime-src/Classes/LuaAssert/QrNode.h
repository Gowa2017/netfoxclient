//
//  QrNode.hpp
//  GameProject
//
//  Created by zhong on 16/10/09.
//
//

#ifndef QrNode_h
#define QrNode_h
#include <stdio.h>
#include "cocos2d.h"
#include "QR_Encode.h"
USING_NS_CC;

int register_all_QrNode();

class QrNode : public Node
{
public:
    ~QrNode();
    /*
     * @brief 创建 二维码 node
     * @param[content] 二维码内容
     * @param[nNodeSize] node 尺寸
     * @param[nQrSize] 点大小
     * @param[nLevel] 二维码level (容错率)
     * @param[nVersion] 二维码版本
     * @param[bAutoExtent] 自动扩展二维码版本
     * @param[nMaskinNo] 屏蔽模式
     */
    static QrNode* createQrNode(const std::string &content,
                                const int &nNodeSize,
                                const int &nQrSize = 5,
                                const int &nLevel = 0,
                                const int &nVersion = 0,
                                const bool &bAutoExtent = true,
                                const int &nMaskingNo = -1);
    
    /*
     * @brief 二维码图片存储
     * @param[filename] 文件名
     * @param[nLuaCallFunC] lua回调函数
     * @return 是否存储成功
     */
    bool saveQrToFile( const std::string &filename, const int &nLuaCallFunC);
    
private:
    bool initQrNode(const std::string &content,
                    const int &nNodeSize,
                    const int &nQrSize = 5,
                    const int &nLevel = 0,
                    const int &nVersion = 0,
                    const bool &bAutoExtent = true,
                    const int &nMaskingNo = -1);

    Node* drawQr();
private:
    int m_nQrSize;
    int m_nNodeSize;
    float m_fScaleRate;
    int m_nQrOriginSize;

    std::string m_strContent;
    CQR_Encode m_QREncode;
    
    Vector<Node*> m_vecQrFileChild;
};

#endif /* QrNode_hpp */
