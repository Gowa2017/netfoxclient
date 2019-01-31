#include "ImageToByte.h"

//构造函数
CImageToByte::CImageToByte() :m_pData(NULL), m_nWidth(0), m_nHeight(0)
{

}
//析构函数
CImageToByte::~CImageToByte()
{
	CC_SAFE_DELETE(m_pData);
}
//加载数据
bool CImageToByte::onLoadData(const char* respath)
{
	bool result = false;
	Image *myImage = nullptr;
	do{
		if (!m_szRes.empty())
		{
			if (m_szRes.compare(respath) == 0)
			{
				CCLOG("[CImageToByte] has init %s", respath);
				break;
			}
		}

		myImage = new Image();
		myImage->initWithImageFile(respath);

		m_nWidth = myImage->getWidth();
		m_nHeight = myImage->getHeight();

		if (m_nWidth == 0 || m_nHeight == 0)
		{
			CCLOG("[_DEBUG] CImageToByte error width-%d height-%d", m_nWidth, m_nHeight);
			break;
		}

		m_szRes = respath;
		CC_SAFE_DELETE(m_pData);

		int len = myImage->getDataLen();
		unsigned char *data = myImage->getData();  //这里就是图片数据了 
		m_pData = new unsigned char[len];
		memset(m_pData, 0, len);
		memcpy(m_pData, data, len);
		result = true;
	} while (false);
	CC_SAFE_DELETE(myImage);
	return result;
}
//获取数据
unsigned int CImageToByte::getData(int x, int y)
{
	if (m_szRes.empty())
		return 0;

	unsigned int *pixel = (unsigned int *)m_pData + ((m_nHeight - y - 1) * m_nWidth) + x;
	return *pixel;
}
//删除数据
void CImageToByte::onCleanData()
{
	m_szRes = "";
	CC_SAFE_DELETE(m_pData);
}