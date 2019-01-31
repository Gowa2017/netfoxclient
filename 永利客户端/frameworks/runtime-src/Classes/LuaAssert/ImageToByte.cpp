#include "ImageToByte.h"

//���캯��
CImageToByte::CImageToByte() :m_pData(NULL), m_nWidth(0), m_nHeight(0)
{

}
//��������
CImageToByte::~CImageToByte()
{
	CC_SAFE_DELETE(m_pData);
}
//��������
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
		unsigned char *data = myImage->getData();  //�������ͼƬ������ 
		m_pData = new unsigned char[len];
		memset(m_pData, 0, len);
		memcpy(m_pData, data, len);
		result = true;
	} while (false);
	CC_SAFE_DELETE(myImage);
	return result;
}
//��ȡ����
unsigned int CImageToByte::getData(int x, int y)
{
	if (m_szRes.empty())
		return 0;

	unsigned int *pixel = (unsigned int *)m_pData + ((m_nHeight - y - 1) * m_nWidth) + x;
	return *pixel;
}
//ɾ������
void CImageToByte::onCleanData()
{
	m_szRes = "";
	CC_SAFE_DELETE(m_pData);
}