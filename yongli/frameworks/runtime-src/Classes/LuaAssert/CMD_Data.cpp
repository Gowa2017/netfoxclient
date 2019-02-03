#include "CMD_Data.h"
#include "LuaAssert.h"

//构造函数
CCmd_Data::CCmd_Data(WORD nLenght):m_wMain(0),m_wSub(0),m_wCurIndex(0)
{
	if(nLenght>0)
	{
		m_wOpacity = nLenght;
		m_wLength = nLenght;
		m_pBuffer = new BYTE[nLenght];
		memset(m_pBuffer,0,nLenght);
		m_bAutoLen = false;
	}
	else
	{
		//默认分配大小
		m_wOpacity = 0;
		m_wLength = 0;
		m_pBuffer = NULL;
		//memset(m_pBuffer,0,AUTO_LEN);
		m_bAutoLen = true;
	}
}
//析构函数
CCmd_Data::~CCmd_Data()
{
	CC_SAFE_DELETE(m_pBuffer);
}
//创建对象
CCmd_Data* CCmd_Data::create(int nLenght)
{
	CCmd_Data* data = new CCmd_Data(nLenght);
	data->autorelease();
	return data;
}
//设置命令
VOID CCmd_Data::SetCmdInfo(WORD wMain,WORD wSub)
{
	m_wMain = wMain;
	m_wSub = wSub;
}
//填充数据
WORD CCmd_Data::PushByteData(BYTE* cbData,WORD wLenght)
{
	do
	{
		//非法过滤
		if(wLenght == 0&&cbData == NULL)
		{
			CCLOG("[_DEBUG]	pushByteData-error-null-input");
			break;
		}

		//重新分配大小
		if(m_wCurIndex+wLenght > m_wOpacity || m_pBuffer == NULL)
		{

			WORD wNewLen = (m_wOpacity + wLenght) * 2;
			BYTE* pNewData = new BYTE[wNewLen];
			memset(pNewData, 0, wNewLen);
			
			if (m_pBuffer)
			{
				memcpy(pNewData, m_pBuffer, m_wOpacity);
				CC_SAFE_DELETE(m_pBuffer);
			}

			m_wOpacity = wNewLen;

			m_pBuffer = pNewData;

			//非自动分配 强行扩展数据长度
			if (m_bAutoLen == false)
			{
				m_wLength += wLenght;
			}
		}

		//自动分配 更新数据长度
		if (m_bAutoLen)
			m_wLength += wLenght;

		//填充数据
		if(cbData != NULL)
			memcpy(m_pBuffer + m_wCurIndex,cbData,wLenght);
		//游标更新
		m_wCurIndex += wLenght;

	}while(false);

	return m_wCurIndex;
}