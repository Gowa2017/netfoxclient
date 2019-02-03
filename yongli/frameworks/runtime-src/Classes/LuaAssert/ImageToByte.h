#ifndef _LUA_IMAGE_TO_BYTE_
#define _LUA_IMAGE_TO_BYTE_

#include "cocos2d.h"
#include <string>
USING_NS_CC;
using namespace std;
class CImageToByte:public cocos2d::Node
{
protected:
	int  m_nWidth;					//图片宽度
	int  m_nHeight;					//图片高度
	std::string m_szRes;			//加载信息
	unsigned char*  m_pData;		//数据缓存
public:
	//构造函数
	CImageToByte();
	//析构函数
	virtual ~CImageToByte();

public:
	//加载数据
	bool onLoadData(const char* respath);
	//删除数据
	void onCleanData();
	//获取数据
	unsigned int getData(int x, int y);
};


#endif