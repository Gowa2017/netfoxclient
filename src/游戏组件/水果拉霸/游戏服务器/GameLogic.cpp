#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const CPoint	CGameLogic::m_ptXian[ITEM_COUNT][ITEM_X_COUNT]=
{
	CPoint(1, 0), CPoint(1, 1), CPoint(1, 2), CPoint(1, 3), CPoint(1, 4), //1
	CPoint(0, 0), CPoint(0, 1), CPoint(0, 2), CPoint(0, 3), CPoint(0, 4), //2
	CPoint(2, 0), CPoint(2, 1), CPoint(2, 2), CPoint(2, 3), CPoint(2, 4), //3
	CPoint(0, 0), CPoint(1, 1), CPoint(2, 2), CPoint(1, 3), CPoint(0, 4), //4
	CPoint(2, 0), CPoint(1, 1), CPoint(0, 2), CPoint(1, 3), CPoint(2, 4), //5
	CPoint(0, 0), CPoint(0, 1), CPoint(1, 2), CPoint(2, 3), CPoint(2, 4), //6
	CPoint(2, 0), CPoint(2, 1), CPoint(1, 2), CPoint(0, 3), CPoint(0, 4), //7
	CPoint(1, 0), CPoint(0, 1), CPoint(1, 2), CPoint(2, 3), CPoint(1, 4), //8
	CPoint(1, 0), CPoint(2, 1), CPoint(1, 2), CPoint(0, 3), CPoint(1, 4)  //9	
};

const BYTE CGameLogic::m_cbItemType[24]=
{
	CT_LU, CT_YINGQIANG, CT_ZHONGYITANG,  CT_FUTOU, CT_DADAO, CT_SHUIHUZHUAN,
	CT_SONG, CT_LU, CT_YINGQIANG, CT_FUTOU, CT_TITIANXINGDAO, CT_SHUIHUZHUAN,
	CT_LIN, CT_DADAO, CT_FUTOU, CT_LU, CT_SONG, CT_SHUIHUZHUAN,
	CT_LIN, CT_YINGQIANG,  CT_FUTOU, CT_DADAO, CT_TITIANXINGDAO, CT_SHUIHUZHUAN,
};

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//随机函数
LONGLONG CGameLogic::GetRandMax(BYTE cbIndex, LONGLONG lZhongJiang[], int nCount)
{
	LONGLONG lRandMax = 0;
	for (int i = 0; i < cbIndex && i < nCount; i++)
	{
		lRandMax += lZhongJiang[i];
	}
	return lRandMax;
}

//随机函数
LONGLONG CGameLogic::LLRand(LONGLONG lMaxCount)
{
	if (lMaxCount <= 0)
	{
		return 0;
	}
	return (LONGLONG)(lMaxCount*rand()/(RAND_MAX + 1.0));
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[ITEM_Y_COUNT][ITEM_X_COUNT], double dControl)
{
	dControl /= 105;//0.47 
	for (int i = 0; i < ITEM_Y_COUNT; i++)
	{
		for (int j = 0; j < ITEM_X_COUNT; j++)
		{
			cbCardBuffer[i][j] = 0xFF;
		}
	}
	BYTE cbCardData[ITEM_COUNT][ITEM_X_COUNT];
	GetCardData(cbCardData, dControl);
	BYTE cbShuiHuZhuan = rand() % 10;
	for (int i = 0; i < ITEM_COUNT; i++)
	{
		if (cbCardData[i][0] != 0xFF)
		{
			if(rand() % 2 == 0)
			{
				for (int j = 0; j < ITEM_X_COUNT; j++)
				{
					if (cbCardData[i][j] != 0xFF)
					{
						if (cbCardBuffer[m_ptXian[i][j].x][m_ptXian[i][j].y] == 0xFF)
						{
							cbCardBuffer[m_ptXian[i][j].x][m_ptXian[i][j].y] = cbCardData[i][j];
						}						
						else if (cbCardBuffer[m_ptXian[i][j].x][m_ptXian[i][j].y] != 0xFF && cbCardBuffer[m_ptXian[i][j].x][m_ptXian[i][j].y] != cbCardData[i][j] && cbShuiHuZhuan > 0)
						{
							cbCardBuffer[m_ptXian[i][j].x][m_ptXian[i][j].y] = CT_SHENBEN;
							cbShuiHuZhuan--;
						}
					}					
				}
			}
			else
			{
				for (int j = 0; j < ITEM_X_COUNT; j++)
				{
					if (cbCardData[i][j] != 0xFF)
					{
						if (cbCardBuffer[m_ptXian[i][ITEM_X_COUNT - j - 1].x][m_ptXian[i][ITEM_X_COUNT - j - 1].y] == 0xFF)
						{
							cbCardBuffer[m_ptXian[i][ITEM_X_COUNT - j - 1].x][m_ptXian[i][ITEM_X_COUNT - j - 1].y] = cbCardData[i][j];
						}
						else if (cbCardBuffer[m_ptXian[i][ITEM_X_COUNT - j - 1].x][m_ptXian[i][ITEM_X_COUNT - j - 1].y] != 0xFF && cbCardBuffer[m_ptXian[i][ITEM_X_COUNT - j - 1].x][m_ptXian[i][ITEM_X_COUNT - j - 1].y] != cbCardData[i][j] && cbShuiHuZhuan > 0)
						{
							cbCardBuffer[m_ptXian[i][ITEM_X_COUNT - j - 1].x][m_ptXian[i][ITEM_X_COUNT - j - 1].y] = CT_SHENBEN;
							cbShuiHuZhuan--;
						}
					}					
				}
			}
		}
	}

	CPoint ptOff[6] = {
		CPoint(-1, -1), CPoint(-1, 0), CPoint(-1, 1),
		CPoint(1, -1), CPoint(1, 0), CPoint(1, 1)};

	for (int i = 0; i < ITEM_Y_COUNT; i++)
	{
		for (int j = 0; j < ITEM_X_COUNT; j++)
		{
			if(cbCardBuffer[i][j] == 0xFF)
			{
				cbCardBuffer[i][j] = CT_FUTOU;
				BYTE cbCardType[12] = {0};
				BYTE cbCardCount = 0;
				for (int k = 0; k < 10; k++)
				{
					bool bFind = false;
					for (int l = 0; l < 6; l++)
					{
						int nY = i + ptOff[l].y;
						int nX = j + ptOff[l].x;
						if (nY >= 0 && nY < ITEM_Y_COUNT && nX >= 0 && nX < ITEM_X_COUNT)
						{
							if (cbCardBuffer[nY][nX] == k)
							{
								bFind = true;
								break;
							}
						}
					}
					if (!bFind)
					{
						cbCardType[cbCardCount++] = k;
					}
				}
				if (cbCardCount > 0)
				{
					cbCardBuffer[i][j] = cbCardType[rand() % cbCardCount];
				}
			}
		}
	}

	return;
}

//取得扑克
VOID CGameLogic::GetCardData(BYTE cbCardBuffer[ITEM_COUNT][ITEM_X_COUNT], double dControl)
{
	//LONGLONG lZhongJiang[9][3] = 
	//{
	//	500000,	20000,	5000,
	//	333333,	10000,	2500,
	//	200000,	6666,	1666,
	//	142850,	5000,	1000,
	//	100000,	3333,	625,
	//	66666,	2500,	500,
	//	50000,	1666,	250,
	//	20000,	500,	100,
	//	0,		0,		50
	//};

	LONGLONG lZhongJiang[9][3] = 
	{
		84000,		33600,		8400,
		56000,		16800,		4200,
		33600,		11200,		2800,
		24000,		8400,		1680,
		16800,		5600,		1050,
		11200,		4200,		840,
		8400,		2100,		420,
		3360,		840,		168,
		0,			0,			84
	};

	LONGLONG lNoZhongJianCount = 0;
	LONGLONG lTotalCount = 0;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			lNoZhongJianCount += lZhongJiang[i][j];
			lZhongJiang[i][j] = (LONGLONG)(lZhongJiang[i][j] / 3 * dControl);
			lTotalCount += lZhongJiang[i][j];
		}
	}

	lTotalCount += lNoZhongJianCount;

	for (int i = 0; i < ITEM_COUNT; i++)
	{
		for (int j = 0; j < ITEM_X_COUNT; j++)
		{
			cbCardBuffer[i][j] = 0xFF;
		}
	}

	int nMaxCount = __min(ITEM_COUNT, 2 + (dControl * 8));
	for (int i = 0; i < nMaxCount; i++)
	{		
		LONGLONG lRand = LLRand(lTotalCount);
		if (lRand >= lNoZhongJianCount)
		{
			BYTE cbIndex = 0;

			if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))			//中三铁斧
			{
				for (int j = 0; j < 3; j++)
				{
					cbCardBuffer[i][j] = CT_FUTOU;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中四铁斧
			{
				for (int j = 0; j < 4; j++)
				{
					cbCardBuffer[i][j] = CT_FUTOU;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中五铁斧
			{
				for (int j = 0; j < 5; j++)
				{
					cbCardBuffer[i][j] = CT_FUTOU;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中三银枪
			{
				for (int j = 0; j < 3; j++)
				{
					cbCardBuffer[i][j] = CT_YINGQIANG;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中四银枪
			{
				for (int j = 0; j < 4; j++)
				{
					cbCardBuffer[i][j] = CT_YINGQIANG;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中五银枪
			{
				for (int j = 0; j < 5; j++)
				{
					cbCardBuffer[i][j] = CT_YINGQIANG;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中三金刀
			{
				for (int j = 0; j < 3; j++)
				{
					cbCardBuffer[i][j] = CT_DADAO;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中四金刀
			{
				for (int j = 0; j < 4; j++)
				{
					cbCardBuffer[i][j] = CT_DADAO;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中五金刀
			{
				for (int j = 0; j < 5; j++)
				{
					cbCardBuffer[i][j] = CT_DADAO;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中三鲁智深
			{
				for (int j = 0; j < 3; j++)
				{
					cbCardBuffer[i][j] = CT_LU;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中四鲁智深
			{
				for (int j = 0; j < 4; j++)
				{
					cbCardBuffer[i][j] = CT_LU;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中五鲁智深
			{
				for (int j = 0; j < 5; j++)
				{
					cbCardBuffer[i][j] = CT_LU;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中三林冲
			{
				for (int j = 0; j < 3; j++)
				{
					cbCardBuffer[i][j] = CT_LIN;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中四林冲
			{
				for (int j = 0; j < 4; j++)
				{
					cbCardBuffer[i][j] = CT_LIN;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中五林冲
			{
				for (int j = 0; j < 5; j++)
				{
					cbCardBuffer[i][j] = CT_LIN;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中三宋江
			{
				for (int j = 0; j < 3; j++)
				{
					cbCardBuffer[i][j] = CT_SONG;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中四宋江
			{
				for (int j = 0; j < 4; j++)
				{
					cbCardBuffer[i][j] = CT_SONG;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中五宋江
			{
				for (int j = 0; j < 5; j++)
				{
					cbCardBuffer[i][j] = CT_SONG;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中三替天行道
			{
				for (int j = 0; j < 3; j++)
				{
					cbCardBuffer[i][j] = CT_TITIANXINGDAO;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中四替天行道
			{
				for (int j = 0; j < 4; j++)
				{
					cbCardBuffer[i][j] = CT_TITIANXINGDAO;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中五替天行道
			{
				for (int j = 0; j < 5; j++)
				{
					cbCardBuffer[i][j] = CT_TITIANXINGDAO;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中三忠义堂
			{
				for (int j = 0; j < 3; j++)
				{
					cbCardBuffer[i][j] = CT_ZHONGYITANG;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中四忠义堂
			{
				for (int j = 0; j < 4; j++)
				{
					cbCardBuffer[i][j] = CT_ZHONGYITANG;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中五忠义堂
			{
				for (int j = 0; j < 5; j++)
				{
					cbCardBuffer[i][j] = CT_ZHONGYITANG;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中三水浒传
			{
				for (int j = 0; j < 3; j++)
				{
					cbCardBuffer[i][j] = CT_SHUIHUZHUAN;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中四水浒传
			{
				for (int j = 0; j < 4; j++)
				{
					cbCardBuffer[i][j] = CT_SHUIHUZHUAN;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中五水浒传
			{
				for (int j = 0; j < 5; j++)
				{
					cbCardBuffer[i][j] = CT_SHUIHUZHUAN;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中7字
			{
				for (int j = 0; j < 3; j++)
				{
					cbCardBuffer[i][j] = CT_SHUIHUZHUAN2;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中7字
			{
				for (int j = 0; j < 4; j++)
				{
					cbCardBuffer[i][j] = CT_SHUIHUZHUAN2;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中7字
			{
				for (int j = 0; j < 5; j++)
				{
					cbCardBuffer[i][j] = CT_SHUIHUZHUAN2;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中扇贝
			{
				for (int j = 0; j < 3; j++)
				{
					cbCardBuffer[i][j] = CT_SHENBEN;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中扇贝
			{
				for (int j = 0; j < 4; j++)
				{
					cbCardBuffer[i][j] = CT_SHENBEN;
				}
				continue;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndex, &lZhongJiang[0][0], sizeof(lZhongJiang) / sizeof(LONGLONG)))		//中扇贝
			{
				for (int j = 0; j < 5; j++)
				{
					cbCardBuffer[i][j] = CT_SHENBEN;
				}
				continue;
			}
		}
	}
}

//混乱扑克
VOID CGameLogic::RandCardListEx(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	for (int i = 0; i < cbBufferCount; i++)
	{
		BYTE cbRand = rand() % 100;
		if (cbRand > 80)
		{
			cbCardBuffer[i] = CT_FUTOU;
		}
		else if (cbRand > 65)
		{
			cbCardBuffer[i] = CT_YINGQIANG;
		}
		else if (cbRand > 54)
		{
			cbCardBuffer[i] = CT_DADAO;
		}
		else if (cbRand > 46)
		{
			cbCardBuffer[i] = CT_LU;
		}
		else if (cbRand > 32)
		{
			cbCardBuffer[i] = CT_LIN;
		}
		else if (cbRand > 20)
		{
			cbCardBuffer[i] = CT_SONG;
		}
		else if (cbRand > 10)
		{
			cbCardBuffer[i] = CT_TITIANXINGDAO;
		}
		else if (cbRand > 4)
		{
			cbCardBuffer[i] = CT_ZHONGYITANG;
		}
		else
		{
			cbCardBuffer[i] =CT_FUTOU;
		}			
	}
	return;
}

//取得扑克
int CGameLogic::RandCardListThree(BYTE &cbIndex, BYTE cbCardBuffer[4], double dControl)
{
	dControl /= 108;
	LONGLONG lZhongJiang[8] = 
	{
		50000, 20000, 10000, 5000, 2500, 1428, 1000, 500
	};

	LONGLONG lNoZhongJianCount = 0;
	LONGLONG lTotalCount = 0;
	for (int i = 0; i < 8; i++)
	{
		lNoZhongJianCount += lZhongJiang[i];
		lZhongJiang[i] = (LONGLONG)(lZhongJiang[i] / 8 * dControl);
		lTotalCount += lZhongJiang[i];
	}

	lTotalCount += lNoZhongJianCount;

	for (int i = 0; i < 4; i++)
	{
		//cbCardBuffer[i] = 0xFF;

		LONGLONG lRand = LLRand(lTotalCount);
		if (lRand >= lNoZhongJianCount)
		{
			BYTE cbIndexTemp = 0;

			if (lRand < lNoZhongJianCount + GetRandMax(++cbIndexTemp, lZhongJiang, sizeof(lZhongJiang) / sizeof(LONGLONG))
				&& cbCardBuffer[i]==CT_FUTOU)			//中铁斧
			{				
				BYTE cbItemType[4] = {3, 9, 14, 20};
				cbIndex = cbItemType[rand() % 4];
				return 2;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndexTemp, lZhongJiang, sizeof(lZhongJiang) / sizeof(LONGLONG))
				&& cbCardBuffer[i]==CT_YINGQIANG)		//中银枪
			{
				BYTE cbItemType[3] = {1, 8, 19};
				cbIndex = cbItemType[rand() % 3];
				return 5;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndexTemp, lZhongJiang, sizeof(lZhongJiang) / sizeof(LONGLONG))
				&& cbCardBuffer[i]==CT_DADAO)		//中大刀
			{
				BYTE cbItemType[3] = {4, 13, 21};
				cbIndex = cbItemType[rand() % 3];
				return 10;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndexTemp, lZhongJiang, sizeof(lZhongJiang) / sizeof(LONGLONG))
				&& cbCardBuffer[i]==CT_LU)		//中鲁
			{
				BYTE cbItemType[3] = {0, 7, 15};
				cbIndex = cbItemType[rand() % 3];
				return 20;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndexTemp, lZhongJiang, sizeof(lZhongJiang) / sizeof(LONGLONG))
				&& cbCardBuffer[i]==CT_LIN)		//中林
			{
				BYTE cbItemType[2] = {12, 18};
				cbIndex = cbItemType[rand() % 2];
				return 50;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndexTemp, lZhongJiang, sizeof(lZhongJiang) / sizeof(LONGLONG))
				&& cbCardBuffer[i]==CT_SONG)		//中宋
			{
				BYTE cbItemType[2] = {6, 16};
				cbIndex = cbItemType[rand() % 2];
				return 70;
			}
			else if (lRand < lNoZhongJianCount + GetRandMax(++cbIndexTemp, lZhongJiang, sizeof(lZhongJiang) / sizeof(LONGLONG))
				&& cbCardBuffer[i]==CT_TITIANXINGDAO)		//中替天行道
			{
				BYTE cbItemType[2] = {10, 22};
				cbIndex = cbItemType[rand() % 2];
				return 100;
			}
			else if(cbCardBuffer[i]==CT_ZHONGYITANG)
			{
				cbIndex = 2;
				return 200;
			}
		}
		else
		{
			BYTE cbItemType[4] = {5, 11, 17, 23};
			cbIndex = cbItemType[rand() % 4];
			return 0;
		}
	}

	BYTE cbItemType[4] = {5, 11, 17, 23};
	cbIndex = cbItemType[rand() % 4];
	return 0;
}

//取得中奖
BYTE CGameLogic::GetZhongJiangInfo(BYTE cbItemInfo[][ITEM_X_COUNT], CPoint ptZhongJiang[][ITEM_X_COUNT])
{
	BYTE cbZhongJiangCount = 0;
	for (int i = 0; i < ITEM_COUNT; i++)
	{
		cbZhongJiangCount += GetZhongJiangXian(cbItemInfo, m_ptXian[i], ptZhongJiang[i]);
	}
	
	return cbZhongJiangCount;
}

//取得中奖
BYTE CGameLogic::GetZhongJiangInfo(BYTE cbIndex, BYTE cbItemInfo[][ITEM_X_COUNT])
{
	CPoint ptZhongJiang[ITEM_X_COUNT];
	return GetZhongJiangXian(cbItemInfo, m_ptXian[cbIndex], ptZhongJiang);
}

//取得中奖
int CGameLogic::GetZhongJiangTime(SCORE cbLine,BYTE cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT] ,INT index)
{
	int nTime = 0;

	BYTE cbZhongJiangCount = 0;
	for (int i = 0; i < cbLine; i++)
	{
		nTime += GetZhongJiangTime(cbItemInfo, m_ptXian[i]);
	}

	//全盘奖
	///nTime += GetQuanPanJiangTime(cbItemInfo);

	return nTime;
}

//取得中奖
int CGameLogic::GetZhongJiangTime(BYTE cbIndex, BYTE cbItemInfo[][ITEM_X_COUNT])
{
	return GetZhongJiangTime(cbItemInfo, m_ptXian[cbIndex]);
}

//全盘奖
int CGameLogic::GetQuanPanJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT])
{
	int nTime = 0;
	bool bSingle = true;
	CPoint ptFirstIndex(0xFF, 0xFF);

	for (int i = 0; i < ITEM_Y_COUNT; i++)
	{
		for (int j = 0; j < ITEM_X_COUNT; j++)
		{
			if (ptFirstIndex.x == 0xFF)
			{
				ptFirstIndex.SetPoint(i, j);
			}
			else if (cbItemInfo[ptFirstIndex.x][ptFirstIndex.y] != cbItemInfo[i][j])
			{					
				if(cbItemInfo[ptFirstIndex.x][ptFirstIndex.y] / 3 != cbItemInfo[i][j] / 3 ||
					cbItemInfo[ptFirstIndex.x][ptFirstIndex.y] >= CT_TITIANXINGDAO || cbItemInfo[i][j] >= CT_TITIANXINGDAO)
				{
					return 0;
				}
				bSingle = false;					
			}
		}
	}

	ASSERT(ptFirstIndex.x != 0xFF);

	if (!bSingle)
	{
		ASSERT(cbItemInfo[ptFirstIndex.x][ptFirstIndex.y] / 3 < 2);
		switch (cbItemInfo[ptFirstIndex.x][ptFirstIndex.y] / 3)
		{
		case 0:
			{
				nTime = 15;
				break;
			}
		case 1:
			{
				nTime = 50;
				break;
			}
		default:
			{
				return 0;
			}
		}
	}
	else
	{
		switch (cbItemInfo[ptFirstIndex.x][ptFirstIndex.y])
		{
		case CT_FUTOU:
			{
				nTime = 50;
				break;
			}
		case CT_YINGQIANG:
			{
				nTime = 100;
				break;
			}
		case CT_DADAO:
			{
				nTime = 150;
				break;
			}
		case CT_LU:
			{
				nTime = 250;
				break;
			}
		case CT_LIN:
			{
				nTime = 400;
				break;
			}
		case CT_SONG:
			{
				nTime = 500;
				break;
			}
		case CT_TITIANXINGDAO:
			{
				nTime = 1000;
				break;
			}
		case CT_ZHONGYITANG:
			{
				nTime = 2500;
				break;
			}
		case CT_SHUIHUZHUAN:
			{
				nTime = 5000;
				break;
			}
		default:
			{
				return 0;
			}
		}
	}

	return nTime;
}

//进入小玛丽
BYTE CGameLogic::GetEnterThree(BYTE cbItemInfo[][ITEM_X_COUNT])
{
	BYTE cbBounsGameCount = 0;
	for (int i = 0; i < ITEM_COUNT; i++)
	{
		cbBounsGameCount += GetEnterThree(cbItemInfo, m_ptXian[i]);
	}

	return cbBounsGameCount;
}

//取得中奖
LONGLONG CGameLogic::GetThreeZhongJiangInfo(BYTE cbItemInfo[], BYTE& cbZhongPos)
{
	LONGLONG lScore = 0;

	cbZhongPos = 0xFF;
	bool bLeftLink = true;
	bool bRightLink = true;
	int nLeftBaseLinkCount = 1;
	int nRightBaseLinkCount = 1;


	//中奖线 1
	for (int i = 1; i < 4; i++)
	{
		//左到右基本奖
		if (cbItemInfo[0] == cbItemInfo[i] && bLeftLink)
		{
			nLeftBaseLinkCount++;
		}
		else
		{
			bLeftLink = false;
		}

		//右到左基本奖
		if (cbItemInfo[3] == cbItemInfo[4-i-1] && bRightLink)
		{
			nRightBaseLinkCount++;
		}
		else
		{
			bRightLink = false;
		}
	}

	if (nRightBaseLinkCount == 4)
	{
		lScore = 500;
		cbZhongPos = 1;
	}

	if (nLeftBaseLinkCount == 3)
	{
		lScore = 20;
		cbZhongPos = 0;
	}
	else if (nRightBaseLinkCount == 3)
	{
		lScore = 20;
		cbZhongPos = 2;
	}	

	return lScore;
}

//取得中奖
bool CGameLogic::GetThreeZhongJiangInfo(BYTE cbIndex, BYTE cbItemInfo[])
{
	ASSERT(cbIndex < 24);
	for (int i = 0; i < 4; i++)
	{
		if (cbItemInfo[i] == m_cbItemType[cbIndex] && m_cbItemType[cbIndex] != CT_SHUIHUZHUAN)
		{
			return true;
		}
	}

	return false;
}

//单线中奖
BYTE CGameLogic::GetZhongJiangXian(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT], CPoint ptZhongJiang[ITEM_X_COUNT])
{
	bool bLeftLink = true;
	bool bRightLink = true;
	int nLeftBaseLinkCount = 0;
	int nRightBaseLinkCount = 0;

	BYTE cbLeftFirstIndex = 0;
	BYTE cbRightFirstIndex = ITEM_X_COUNT - 1;

	for (int i = 0; i < ITEM_X_COUNT; i++)
	{
		ptZhongJiang[i].SetPoint(0xFF, 0xFF);
		if (cbItemInfo[ptXian[i].x][ptXian[i].y] != CT_SHUIHUZHUAN && bLeftLink)
		{
			cbLeftFirstIndex = i;
			bLeftLink = false;
		}

		if (cbItemInfo[ptXian[ITEM_X_COUNT - i - 1].x][ptXian[ITEM_X_COUNT - i - 1].y] != CT_SHUIHUZHUAN && bRightLink)
		{
			cbRightFirstIndex = ITEM_X_COUNT - i - 1;
			bRightLink = false;
		}
	}

	bLeftLink = true;
	bRightLink = true;

	//中奖线
	for (int i = 0; i < ITEM_X_COUNT; i++)
	{
		//左到右基本奖
		if ((cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == cbItemInfo[ptXian[i].x][ptXian[i].y] || cbItemInfo[ptXian[i].x][ptXian[i].y] == CT_SHUIHUZHUAN) && bLeftLink)
		{
			nLeftBaseLinkCount++;
		}
		else
		{
			bLeftLink = false;
		}

		//右到左基本奖
		if ((cbItemInfo[ptXian[cbRightFirstIndex].x][ptXian[cbRightFirstIndex].y] == cbItemInfo[ptXian[ITEM_X_COUNT - i - 1].x][ptXian[ITEM_X_COUNT - i - 1].y] || cbItemInfo[ptXian[ITEM_X_COUNT - i - 1].x][ptXian[ITEM_X_COUNT - i - 1].y] == CT_SHUIHUZHUAN) && bRightLink)
		{
			nRightBaseLinkCount++;
		}
		else
		{
			bRightLink = false;
		}
	}

	int nLinkCount = 0;
	if (nLeftBaseLinkCount >= 3)
	{
		for (int i = 0; i < nLeftBaseLinkCount; i++)
		{
			ptZhongJiang[i].SetPoint(ptXian[i].x, ptXian[i].y);
			
		}	
		nLinkCount += nLeftBaseLinkCount;
	}
	
	if (nRightBaseLinkCount >= 3)
	{
		for (int i = 0; i < nRightBaseLinkCount; i++)
		{
			ptZhongJiang[ITEM_X_COUNT - 1 - i].SetPoint(ptXian[ITEM_X_COUNT - 1 - i].x, ptXian[ITEM_X_COUNT - 1 - i].y);
			
		}

		nLinkCount += nRightBaseLinkCount;
	}

	return __min(5, nLinkCount);
}

//单线中奖
int CGameLogic::GetZhongJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT])
{
	int nTime = 0;

	bool bLeftLink = true;
	bool bRightLink = true;
	int nLeftBaseLinkCount = 0;
	//int nRightBaseLinkCount = 0;

	BYTE cbLeftFirstIndex = 0;
	//BYTE cbRightFirstIndex = ITEM_X_COUNT - 1;

	for (int i = 0; i < ITEM_X_COUNT; i++)
	{
		if (cbItemInfo[ptXian[i].x][ptXian[i].y] != CT_SHENBEN && bLeftLink)
		{
			cbLeftFirstIndex = i;
			bLeftLink = false;
		}

		//if (cbItemInfo[ptXian[ITEM_X_COUNT - i - 1].x][ptXian[ITEM_X_COUNT - i - 1].y] != CT_SHUIHUZHUAN && bRightLink)
		//{
		//	cbRightFirstIndex = ITEM_X_COUNT - i - 1;
		//	bRightLink = false;
		//}
	}


	bLeftLink = true;
	bRightLink = true;

	//中奖线
	for (int i = 0; i < ITEM_X_COUNT; i++)
	{
		//左到右基本奖
		if ((cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == cbItemInfo[ptXian[i].x][ptXian[i].y] || cbItemInfo[ptXian[i].x][ptXian[i].y] == CT_SHENBEN) && bLeftLink)
		{
			nLeftBaseLinkCount++;
		}
		else
		{
			bLeftLink = false;
		}
		////右到左基本奖
		//if ((cbItemInfo[ptXian[cbRightFirstIndex].x][ptXian[cbRightFirstIndex].y] == cbItemInfo[ptXian[ITEM_X_COUNT - i - 1].x][ptXian[ITEM_X_COUNT - i - 1].y] || cbItemInfo[ptXian[ITEM_X_COUNT - i - 1].x][ptXian[ITEM_X_COUNT - i - 1].y] == CT_SHUIHUZHUAN) && bRightLink)
		//{
		//	nRightBaseLinkCount++;
		//}
		//else
		//{
		//	bRightLink = false;
		//}
	}


	if (nLeftBaseLinkCount == 5)		//全盘奖 | 五连	
	{
		//中奖类型
		switch(cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y])
		{
		case CT_FUTOU:
			{
				nTime += 75;
				break;
			}
		case CT_YINGQIANG:
			{
				nTime += 85;
				break;
			}
		case CT_DADAO:
			{
				nTime += 250;
				break;
			}
		case CT_LU:
			{
				nTime += 400;
				break;
			}
		case CT_LIN	:
			{
				nTime += 550;
				break;
			}
		case CT_SONG:
			{
				nTime += 650;
				break;
			}
		case CT_TITIANXINGDAO:
			{
				nTime += 800;
				break;
			}
		case CT_ZHONGYITANG:
			{
				nTime += 1250;
				break;
			}
		case CT_SHUIHUZHUAN:
			{
				nTime += 400;
				break;
			}
		case CT_SHUIHUZHUAN2:
		{
				nTime += 1750;
				break;
		}
		case CT_SHENBEN:
		{
				nTime += 1000;
				break;
		}
		}
	}
	else 
	{
		if (nLeftBaseLinkCount == 2)
		{
			//中奖类型
			if (cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_FUTOU)
			{
				nTime += 1;
			}
		}
		else if (nLeftBaseLinkCount == 3 || nLeftBaseLinkCount == 4)
		{
			//中奖类型
			if(cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_FUTOU)
			{
				nTime += nLeftBaseLinkCount == 3 ? 3 : 10;
			}
			else if(cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_YINGQIANG)
			{
				nTime += nLeftBaseLinkCount == 3 ? 3 : 10;
			}
			else if(cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_DADAO)
			{
				nTime += nLeftBaseLinkCount == 3 ? 15 : 40;
			}
			else if(cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_LU)
			{
				nTime += nLeftBaseLinkCount == 3 ? 25 : 50;
			}
			else if(cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_LIN)
			{
				nTime += nLeftBaseLinkCount == 3 ? 30 : 70;
			}
			else if(cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_SONG)
			{
				nTime += nLeftBaseLinkCount == 3 ? 35 : 80;
			}
			else if(cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_TITIANXINGDAO)
			{
				nTime += nLeftBaseLinkCount == 3 ? 45 : 100;
			}
			else if(cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_ZHONGYITANG)
			{
				nTime += nLeftBaseLinkCount == 3 ? 75 : 175;
			}
			else if(cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_SHUIHUZHUAN)
			{
				nTime += nLeftBaseLinkCount == 3 ? 25 : 50;
			}
			else if (cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_SHUIHUZHUAN2)
			{
				nTime += nLeftBaseLinkCount == 3 ? 100 : 200;
			}
			else if (cbItemInfo[ptXian[cbLeftFirstIndex].x][ptXian[cbLeftFirstIndex].y] == CT_SHENBEN)
			{
				nTime += nLeftBaseLinkCount == 3 ? 0 : 0;
			}
		}
		//if (nRightBaseLinkCount == 3 || nRightBaseLinkCount == 4)
		//{
		//	//中奖类型
		//	if(cbItemInfo[ptXian[cbRightFirstIndex].x][ptXian[cbRightFirstIndex].y] == CT_FUTOU)
		//	{
		//		nTime += nRightBaseLinkCount == 3 ? 2 : 5;
		//	}
		//	else if(cbItemInfo[ptXian[cbRightFirstIndex].x][ptXian[cbRightFirstIndex].y] == CT_YINGQIANG)
		//	{
		//		nTime += nRightBaseLinkCount == 3 ? 3 : 10;
		//	}
		//	else if(cbItemInfo[ptXian[cbRightFirstIndex].x][ptXian[cbRightFirstIndex].y] == CT_DADAO)
		//	{
		//		nTime += nRightBaseLinkCount == 3 ? 5 : 15;
		//	}
		//	else if(cbItemInfo[ptXian[cbRightFirstIndex].x][ptXian[cbRightFirstIndex].y] == CT_LU)
		//	{
		//		nTime += nRightBaseLinkCount == 3 ? 7 : 20;
		//	}
		//	else if(cbItemInfo[ptXian[cbRightFirstIndex].x][ptXian[cbRightFirstIndex].y] == CT_LIN)
		//	{
		//		nTime += nRightBaseLinkCount == 3 ? 10 : 30;
		//	}
		//	else if(cbItemInfo[ptXian[cbRightFirstIndex].x][ptXian[cbRightFirstIndex].y] == CT_SONG)
		//	{
		//		nTime += nRightBaseLinkCount == 3 ? 15 : 40;
		//	}
		//	else if(cbItemInfo[ptXian[cbRightFirstIndex].x][ptXian[cbRightFirstIndex].y] == CT_TITIANXINGDAO)
		//	{
		//		nTime += nRightBaseLinkCount == 3 ? 20 : 80;
		//	}
		//	else if(cbItemInfo[ptXian[cbRightFirstIndex].x][ptXian[cbRightFirstIndex].y] == CT_ZHONGYITANG)
		//	{
		//		nTime += nRightBaseLinkCount == 3 ? 50 : 200;
		//	}
		//	else if(cbItemInfo[ptXian[cbRightFirstIndex].x][ptXian[cbRightFirstIndex].y] == CT_SHUIHUZHUAN)
		//	{
		//		nTime += nRightBaseLinkCount == 3 ? 0 : 0;
		//	}
		//}
	}

	return nTime;
}

//进入小玛丽
BYTE CGameLogic::GetEnterThree(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT])
{
	bool bLeftLink = true;
	bool bRightLink = true;
	int nLeftBaseLinkCount = 0;
	int nRightBaseLinkCount = 0;

	//中奖线
	for (int i = 0; i < ITEM_X_COUNT; i++)
	{
		//左到右基本奖
		if (cbItemInfo[ptXian[i].x][ptXian[i].y] == CT_SHUIHUZHUAN && bLeftLink)
		{
			nLeftBaseLinkCount++;
		}
		else
		{
			bLeftLink = false;
		}

		//右到左基本奖
		if (cbItemInfo[ptXian[ITEM_X_COUNT - i - 1].x][ptXian[ITEM_X_COUNT - i - 1].y] == CT_SHUIHUZHUAN && bRightLink)
		{
			nRightBaseLinkCount++;
		}
		else
		{
			bRightLink = false;
		}
	}

	if (nLeftBaseLinkCount == 3 || nRightBaseLinkCount == 3)
	{
		return 1;	
	}
	else if (nLeftBaseLinkCount == 4 || nRightBaseLinkCount == 4)
	{
		return 2;
	}
	else if (nRightBaseLinkCount == 5)
	{
		return 3;
	}

	return 0;
}