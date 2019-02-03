#ifndef _LUA_IMAGE_TO_BYTE_
#define _LUA_IMAGE_TO_BYTE_

#include "cocos2d.h"
#include <string>
USING_NS_CC;
using namespace std;
class CImageToByte:public cocos2d::Node
{
protected:
	int  m_nWidth;					//ͼƬ���
	int  m_nHeight;					//ͼƬ�߶�
	std::string m_szRes;			//������Ϣ
	unsigned char*  m_pData;		//���ݻ���
public:
	//���캯��
	CImageToByte();
	//��������
	virtual ~CImageToByte();

public:
	//��������
	bool onLoadData(const char* respath);
	//ɾ������
	void onCleanData();
	//��ȡ����
	unsigned int getData(int x, int y);
};


#endif