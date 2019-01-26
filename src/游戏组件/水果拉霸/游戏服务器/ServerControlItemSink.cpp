#include "StdAfx.h"
#include "servercontrolitemsink.h"

//////////////////////////////////////////////////////////////////////////

//ÆË¿ËÊý¾Ý
const CPoint	CServerControlItemSink::m_ptXian[ITEM_COUNT][ITEM_X_COUNT]=
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
//
CServerControlItemSink::CServerControlItemSink(void)
{
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//·þÎñÆ÷¿ØÖÆ
bool __cdecl CServerControlItemSink::ServerControl(USERCONTROL &UserContorl, BYTE cbCardData[][ITEM_X_COUNT])
{
	BYTE cbCardDataTemp[ITEM_X_COUNT * ITEM_Y_COUNT] = {0};
	for (int i = 0; i< ITEM_Y_COUNT; i++)
	{
		for (int j = 0; j< ITEM_X_COUNT; j++)
		{
			cbCardData[i][j] = 0xFF;
		}
	}
	BYTE cbCardCount = GetCardType(UserContorl, cbCardDataTemp);
	if(cbCardCount > 0)
	{
		if(cbCardCount == ITEM_X_COUNT * ITEM_Y_COUNT)
		{
			for (int i = 0; i< ITEM_Y_COUNT; i++)
			{
				for (int j = 0; j< ITEM_X_COUNT; j++)
				{
					cbCardData[i][j] = cbCardDataTemp[i * ITEM_Y_COUNT + j];
				}
			}
		}
		else if(cbCardCount >= 3 && cbCardCount <= 5)
		{
			int nXian = 0;
			if (cbCardCount == 3) nXian = rand() % 3;
			if (cbCardCount == 4) nXian = rand() % 2;
			for (int i = 0; i < cbCardCount; i++)
			{
				cbCardData[/*m_ptXian[nXian][i].x*/1][m_ptXian[nXian][i].y] = cbCardDataTemp[i];
			}
			//²¹¿Õ
			BYTE cbType = 0;
			for (int y = 0; y< ITEM_Y_COUNT; y++)
			{
				for (int x = 0; x< ITEM_X_COUNT; x++)
				{
					if(cbCardData[y][x] == 0xFF)
					{		
						cbType = rand() % 8;
						for (int i = 0; i < 8; i++)
						{
							cbType++;
							if(cbType == 8)
							{
								cbType = 0;
							}
							//×ó±ß
							if(x > 0)
							{
								//×óÉÏ±ß
								if(y > 0)
								{
									if(cbCardData[y - 1][x - 1] == cbType)
									{
										continue;
									}
								}

								//×ó±ß
								if(cbCardData[y][x - 1] == cbType)
								{
									continue;
								}

								//×óÏÂ±ß
								if(y < ITEM_Y_COUNT - 1)
								{
									if(cbCardData[y + 1][x - 1] == cbType)
									{
										continue;
									}
								}
							}
							break;
						}
						cbCardData[y][x] = cbType;
					}
				}
			}
		}
		else
		{
			ASSERT(NULL);
			return false;
		}
		return true;
	}
	ASSERT(NULL);
	return false;
}

int CServerControlItemSink::GetCardType(USERCONTROL &UserContorl, BYTE cbCardData[ITEM_X_COUNT * ITEM_Y_COUNT])
{
	int nCardCount = 0;
	switch(UserContorl.controlType)
	{
	case CONTINUE_3:
		{
			nCardCount = 3;
			break;
		}
	case CONTINUE_4:
		{
			nCardCount = 4;
			break;
		}
	case CONTINUE_5:
		{
			nCardCount = 5;
			break;
		}
	case CONTINUE_ALL:
	case CONTINUE_LOST:
		{
			nCardCount = ITEM_X_COUNT * ITEM_Y_COUNT;
			break;
		}
	}
	switch(UserContorl.cbControlData)
	{
	case 0:
		{
			//str = TEXT("Ïã½¶");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_FUTOU;
			}
			break;
		}
	case 1:
		{
			//str = TEXT("Î÷¹Ï");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_YINGQIANG;
			}
			break;
		}
	case 2:
		{
			//str = TEXT("Ã¢¹û");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_DADAO;
			}
			break;
		}
	case 3:
		{
			//str = TEXT("ÆÏÌÑ");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_LU;
			}
			break;
		}
	case 4:
		{
			//str = TEXT("²¤ÂÜ");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_LIN;
			}
			break;
		}
	case 5:
		{
			//str = TEXT("Áåîõ");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_SONG;
			}
			break;
		}
	case 6:
		{
			//str = TEXT("Ó£ÌÒ");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_TITIANXINGDAO;
			}
			break;
		}
	case 7:
		{
			//str = TEXT("¾Æ°É");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_ZHONGYITANG;
			}
			break;
		}
	case 8:
		{
			//str = TEXT("Àæ×Ó");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_SHUIHUZHUAN;
			}
			break;
		}
	case 9:
		{
			//str = TEXT("7×Ö");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_SHUIHUZHUAN2;
			}
			break;
		}
	case 10:
		{
			//str = TEXT("ÉÈ±´");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_SHENBEN;
			}
			break;
		}
	}

	return nCardCount;
}