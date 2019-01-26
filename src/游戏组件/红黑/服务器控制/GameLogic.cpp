#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

const BYTE CGameLogic::m_cbCardListData[CARD_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//黑桃 A - K

};
//////////////////////////////////////////////////////////////////////////

//混乱扑克
void RandCount(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE *cbCardData = new BYTE[cbBufferCount];
	CopyMemory(cbCardData,cbCardBuffer,cbBufferCount);

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);



	delete []cbCardData;
	cbCardData = NULL;

	return;
}


//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//混乱扑克
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE cbControlArea[])
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardListData)];
	CopyMemory(cbCardData,m_cbCardListData,sizeof(cbCardData));
	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	BYTE leftCards[3];
	BYTE leftCardsNum = 0;
	INT winArea = 0;
	INT cardType = 0;
	if (cbControlArea[0] == CONTROL_BLACK || cbControlArea[0] == CONTROL_RED)
	{
		winArea = cbControlArea[0];
		cardType = cbControlArea[1];
	}
	else
		cardType = cbControlArea[0];
	if (cardType == CONTROL_BAOZI)
	{
		leftCards[0] = rand()%13;
		leftCards[1] = leftCards[0] + 13;
		leftCards[2] = leftCards[0] + 26;
	}
	else if (cardType == CONTROL_TONGHUASHUN)
	{
		leftCards[0]=rand()%(CountArray(cbCardData));
		leftCardsNum = 2;
		if (leftCards[0]%13 == 0)
		{
			leftCards[1] = leftCards[0]+1;
			leftCards[2] = leftCards[0]+2;
		}
		else if (leftCards[0]%13 == 12)
		{
			leftCards[1] = leftCards[0]-1;
			leftCards[2] = leftCards[0]-2;
		}
		else 
		{
			leftCards[1] = leftCards[0]-1;
			leftCards[2] = leftCards[0]+1;
		}
	}
	else if (cardType == CONTROL_JINHUA || cardType == CONTROL_JINHUA_A)
	{
		if (cardType == CONTROL_JINHUA_A)
		{
			BYTE indexA = rand()%4;
			leftCards[0] = indexA*13;
		}
		else
			leftCards[0]=rand()%(CountArray(cbCardData));
		leftCardsNum = 12;
		BYTE tempLeftCards[12];
		BYTE tempIndex = 0;
		BYTE writeIndex = 0;
		do 
		{
			BYTE temp = leftCards[0]/13 *13+tempIndex;
			if (temp != leftCards[0])
			{
				tempLeftCards[writeIndex] = temp;
				writeIndex++;
			}
			tempIndex++;
		} while (writeIndex<leftCardsNum);
		BYTE tempCount=0;
		BYTE index = 1;
		do 
		{
			BYTE pointPosition = rand()%(leftCardsNum-tempCount);
			leftCards[index] = tempLeftCards[pointPosition];
			index++;
			tempCount++;
			tempLeftCards[pointPosition]=tempLeftCards[leftCardsNum-tempCount];
		} while (index < 3);
	}
	else if (cardType == CONTROL_DUIZI)
	{
		leftCards[0]=rand()%(CountArray(cbCardData));
		BYTE leftIndex =  leftCards[0]/13 + rand()%3 + 1;
		if (leftIndex >= 4)
			leftIndex = leftIndex - 4;
		leftCards[1] = leftCards[0]%13 + leftIndex * 13;
		leftCardsNum = 50;
		BYTE tempLeftCards[50];
		BYTE tempIndex = 0;
		BYTE writeIndex = 0;
		do
		{
			if (tempIndex != leftCards[0] && tempIndex != leftCards[1])
			{
				tempLeftCards[writeIndex] = tempIndex;
				writeIndex++;
			}
			tempIndex++;
		} while (writeIndex<leftCardsNum);
		BYTE pointPosition = rand()%leftCardsNum;
		leftCards[2] = tempLeftCards[pointPosition];
	}
	else if (cardType == CONTROL_SPECIAL)
	{
		leftCards[0] = 1;
		leftCards[1] = 15;
		leftCards[2] = 30;
	} else if (cardType == CONTROL_SINGLE)
	{
		BYTE tempLeftCards[CARD_COUNT];
		BYTE tempIndex = 0;
		do
		{
			tempLeftCards[tempIndex] = tempIndex;
			tempIndex++;
		} while (tempIndex<CARD_COUNT);
		BYTE isFind = false;
		BYTE cbTempLeftCards[CountArray(m_cbCardListData)];
		CopyMemory(cbTempLeftCards,tempLeftCards,sizeof(cbTempLeftCards));
		do 
		{
			BYTE tempCount=0;
			BYTE index = 0;
			BYTE tempSingleCardData[MAX_COUNT];
			ZeroMemory(tempLeftCards, sizeof(tempLeftCards));
			CopyMemory(tempLeftCards,cbTempLeftCards,sizeof(tempLeftCards));
			do 
			{
				BYTE pointPosition = rand()%(CARD_COUNT-tempCount);
				leftCards[index] = tempLeftCards[pointPosition];
				tempSingleCardData[index]=cbCardData[leftCards[index]];
				index++;
				tempCount++;
				tempLeftCards[pointPosition]=tempLeftCards[CARD_COUNT-tempCount];
			} while (index < MAX_COUNT);
			if (GetCardType(tempSingleCardData,MAX_COUNT) ==  CARD_SINGLE)
				isFind = true;
		} while (isFind == false);
	}
	BYTE cbFirstCardData[3],cbSecondCardData[3];
	BYTE needCardsNum = 3;
	BYTE tempIndex = 0;
	// 只设定输赢方，不设定牌型
	if (cardType == 0)
	{
		do
		{
			cbPosition=rand()%(CountArray(cbCardData)-cbRandCount);
			cbFirstCardData[tempIndex] = cbCardData[cbPosition];
			tempIndex++;
			cbRandCount++;
			cbCardData[cbPosition]=cbCardData[CountArray(cbCardData)-cbRandCount];
		} while (tempIndex<needCardsNum);
		tempIndex = 0;
		do
		{
			cbPosition=rand()%(CountArray(cbCardData)-cbRandCount);
			cbSecondCardData[tempIndex] = cbCardData[cbPosition];
			tempIndex++;
			cbRandCount++;
			cbCardData[cbPosition]=cbCardData[CountArray(cbCardData)-cbRandCount];
		} while (tempIndex<needCardsNum);
		int nResult = CompareCard(cbFirstCardData,MAX_COUNT,cbSecondCardData,MAX_COUNT);
		BYTE isNormalSort = true;
		if ((nResult==1 && winArea == CONTROL_RED) || (nResult==-1 && winArea == CONTROL_BLACK))
			isNormalSort = false;
		BYTE cardBuffIndex = 0;
		if (isNormalSort)
		{
			do
			{
				cbCardBuffer[cardBuffIndex] = cbFirstCardData[cardBuffIndex];
				cardBuffIndex++;
			} while (cardBuffIndex < needCardsNum);
			do 
			{
				cbCardBuffer[cardBuffIndex] = cbSecondCardData[cardBuffIndex-3];
				cardBuffIndex++;
			} while (cardBuffIndex < needCardsNum+3);
		}
		else
		{
			do
			{
				cbCardBuffer[cardBuffIndex] = cbSecondCardData[cardBuffIndex];
				cardBuffIndex++;
			} while (cardBuffIndex < needCardsNum);
			do 
			{
				cbCardBuffer[cardBuffIndex] = cbFirstCardData[cardBuffIndex-3];
				cardBuffIndex++;
			} while (cardBuffIndex < needCardsNum+3);
		}
	}
	else
	{
		// 有牌型，红黑赢
		do 
		{
			cbFirstCardData[tempIndex]=cbCardData[leftCards[tempIndex]];
			cbRandCount++;
			cbCardData[leftCards[tempIndex]]=cbCardData[CountArray(cbCardData)-cbRandCount];
			tempIndex++;
		} while (tempIndex < needCardsNum);
		BYTE isFind = false;
		BYTE cbRandCountTemp = cbRandCount;
		BYTE cbTempCardData[CountArray(m_cbCardListData)];
		CopyMemory(cbTempCardData,cbCardData,sizeof(cbTempCardData));
		tempIndex = 0;
		if (cardType == CONTROL_SPECIAL && winArea != 0)
		{
			// 一家235，要赢，另外一家必须是豹子
			BYTE cbSecondIndex = rand()%13;
			for (BYTE i=0;i<4;i++)
			{
				BYTE j = cbSecondIndex+13*i;
				if (j != leftCards[0] && j != leftCards[1] && j != leftCards[2])
				{
					cbSecondCardData[tempIndex] = cbCardData[j];
					tempIndex++;
				}
				if (tempIndex >= needCardsNum)
					break;
			}
		}
		else if (cardType == CARD_SINGLE)
		{
			BYTE isFind = false;
			do 
			{
				cbRandCount = cbRandCountTemp;
				ZeroMemory(cbCardData, sizeof(cbCardData));
				CopyMemory(cbCardData,cbTempCardData,sizeof(cbCardData));
				BYTE index = 0;
				do 
				{
					cbPosition=rand()%(CountArray(cbCardData)-cbRandCount);
					cbSecondCardData[index] = cbCardData[cbPosition];
					index++;
					cbRandCount++;
					cbCardData[cbPosition]=cbCardData[CountArray(cbCardData)-cbRandCount];
				} while (index < needCardsNum);
				if (GetCardType(cbSecondCardData,MAX_COUNT) ==  CARD_SINGLE)
					isFind = true;
			} while (isFind == false);
		}
		else
		{
			cbRandCount = cbRandCountTemp;
			ZeroMemory(cbCardData, sizeof(cbCardData));
			CopyMemory(cbCardData,cbTempCardData,sizeof(cbCardData));
			do
			{
				cbPosition=rand()%(CountArray(cbCardData)-cbRandCount);
				cbSecondCardData[tempIndex] = cbCardData[cbPosition];
				tempIndex++;
				cbRandCount++;
				cbCardData[cbPosition]=cbCardData[CountArray(cbCardData)-cbRandCount];
			} while (tempIndex<needCardsNum);
		}
		int nResult = CompareCard(cbFirstCardData,MAX_COUNT,cbSecondCardData,MAX_COUNT);
		BYTE isNormalSort = true;
		if ((nResult==1 && winArea == CONTROL_RED) || (nResult==-1 && winArea == CONTROL_BLACK))
			isNormalSort = false;
		BYTE cardBuffIndex = 0;
		if (isNormalSort)
		{
			do
			{
				cbCardBuffer[cardBuffIndex] = cbFirstCardData[cardBuffIndex];
				cardBuffIndex++;
			} while (cardBuffIndex < needCardsNum);
			do 
			{
				cbCardBuffer[cardBuffIndex] = cbSecondCardData[cardBuffIndex-3];
				cardBuffIndex++;
			} while (cardBuffIndex < needCardsNum+3);
		}
		else
		{
			do
			{
				cbCardBuffer[cardBuffIndex] = cbSecondCardData[cardBuffIndex];
				cardBuffIndex++;
			} while (cardBuffIndex < needCardsNum);
			do 
			{
				cbCardBuffer[cardBuffIndex] = cbFirstCardData[cardBuffIndex-3];
				cardBuffIndex++;
			} while (cardBuffIndex < needCardsNum+3);
		}
	}


	//#ifdef _DEBUG
	//	cbCardBuffer[0]=0x01;
	//	cbCardBuffer[1]=0x02;
	//
	//	cbCardBuffer[2]=0x02;
	//	cbCardBuffer[3]=0x03;
	//
	//	cbCardBuffer[4]=0x04;
	//	cbCardBuffer[5]=0x04;
	//
	//	cbCardBuffer[6]=0x05;
	//	cbCardBuffer[7]=0x05;
	//
	//#endif

	return;
}
void CGameLogic::GetCardPoint(BYTE bcCardData ,CPoint &Point,int CardX,int CardY)
{
	BYTE pbcCardData[]={0x0a,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};
	for (int i = 0;i<10;i++)
	{
		if(bcCardData==pbcCardData[i])
		{
			if(i==0)
			{
				Point.x = CardX*(i-11);
				Point.y =CardY;

			}else
			{
				Point.x =CardX*i;
				Point.y = 0;

			}

			return ;

		}
	}
	Point.x =CardX*10;
	Point.y = 1;

	return ;

}
DWORD CGameLogic::GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount)
{
	//效验参数
	ASSERT(cbCardCount==MAX_COUNT);
	if(cbCardCount!=MAX_COUNT)return 0;

	DWORD dwCardType=0;

	//对子
	if(GetCardValue(cbCardData[0])==GetCardValue(cbCardData[1]))
	{	
		return 10;
	}
	else
	{
		BYTE maxValue=0;

		BYTE cbPoint0 = GetCardValue(cbCardData[0]);
		BYTE cbPoint1 = GetCardValue(cbCardData[1]);
		if (cbPoint0 >= 10)cbPoint0 = 0;
		if (cbPoint1 >= 10)cbPoint1 = 0;

		maxValue=cbPoint0+cbPoint1;

		if (maxValue>=10)
		{
			maxValue -=10;
		}
		if (maxValue==0)
		{
			BYTE LittleValue = GetCardValue(cbCardData[0])>GetCardValue(cbCardData[1])?GetCardValue(cbCardData[1]):GetCardValue(cbCardData[0]);
			//2+8最大
			if (LittleValue==2)
			{
				return 11;
			}
			//其余不分大小
			else
			{
				return maxValue;
			}
		}
		else
		{
			return maxValue;
		}
	}


}

//获取牌型
DWORD CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{

	//效验参数
	ASSERT(cbCardCount==MAX_COUNT);
	if(cbCardCount!=MAX_COUNT)return 0;

	DWORD dwCardType=0;
	BYTE *newCbCardData = new BYTE[cbCardCount];
	CopyMemory(newCbCardData,cbCardData,cbCardCount);
	SortCardList(newCbCardData, cbCardCount, ST_LOGIC);
	BYTE firstValue = GetCardLogicValue(newCbCardData[0]);
	BYTE secondValue = GetCardLogicValue(newCbCardData[1]);
	BYTE thirdValue = GetCardLogicValue(newCbCardData[2]);
	BYTE firstColor = GetCardColor(newCbCardData[0]);
	BYTE secondColor = GetCardColor(newCbCardData[1]);
	BYTE thirdColor = GetCardColor(newCbCardData[2]);
	if (firstValue == 2 && secondValue == 3 &&  thirdValue == 5 && firstColor != secondColor && firstColor != thirdColor && secondColor != thirdColor)
		return CARD_SPECIAL;
	else if (firstValue == secondValue && firstValue == thirdValue)
		return CARD_BAOZI;
	else if (firstColor == secondColor && firstColor == thirdColor)
	{
		if ((firstValue == 2 && secondValue == 3 && thirdValue == 14) || (firstValue == secondValue-1 && firstValue == thirdValue - 2))
			return CARD_TONGHUASHUN;
		else
			return CARD_JINHUA;
	} else if ((firstValue == 2 && secondValue == 3 && thirdValue == 14) || (firstValue == secondValue-1 && firstValue == thirdValue - 2))
	{
		return CARD_SHUNZI;
	} else if (firstValue == secondValue || secondValue == thirdValue)
		return CARD_DUIZI;
	else return CARD_SINGLE;
}


BYTE CGameLogic::GetNewCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//效验参数
	ASSERT(cbCardCount==MAX_COUNT);
	if(cbCardCount!=MAX_COUNT)return 0;
	//对子
	if(GetCardValue(cbCardData[0])==GetCardValue(cbCardData[1]))
	{	
		switch(cbCardData[0])
		{
		case 0x10:
			return 20;
		case 0x09:
			return 19;
		case 0x08:
			return 18;
		case 0x07:
			return 17;
		case 0x06:
			return 16;
		case 0x05:
			return 15;
		case 0x04:
			return 14;
		case 0x03:
			return 13;
		case 0x02:
			return 12;
		case 0x01:
			return 11;
		}
	}
	BYTE maxValue=0;

	maxValue=GetCardValue(cbCardData[0])+GetCardValue(cbCardData[1]);
	if (maxValue>=10)
	{
		maxValue -=10; 
	}
	if (maxValue==0)
	{
		BYTE LittleValue = GetCardValue(cbCardData[0])>GetCardValue(cbCardData[1])?GetCardValue(cbCardData[1]):GetCardValue(cbCardData[0]);
		//2+8最大
		if (LittleValue==2)
		{
			return 10;
		}
		//其余不分大小
		else
		{
			return 0;
		}
	}
	return maxValue;
}
//大小比较
/*
cbNextCardData>cbFirstCardData  返回1
cbNextCardData<cbFirstCardData  返回-1
cbNextCardData==cbFirstCardData 返回0
*/
int CGameLogic::CompareCard(BYTE cbFirstCardData[], BYTE cbFirstCardCount,BYTE cbNextCardData[], BYTE cbNextCardCount)
{
	BYTE pWinArea[3] = {false};
	//合法判断
	ASSERT(MAX_COUNT==cbFirstCardCount && MAX_COUNT==cbNextCardCount);
	if (!(MAX_COUNT==cbFirstCardCount && MAX_COUNT==cbNextCardCount)) return 0;

	DWORD dwCardType[2]={GetCardType(cbFirstCardData,MAX_COUNT),GetCardType(cbNextCardData,MAX_COUNT)};

	//printf("中区域的牌型===== %d \n",dwCardType[0]);
	//printf("白区域的牌型===== %d \n",dwCardType[1]);

	int nResult=0;
	if (dwCardType[0] == CARD_SPECIAL && dwCardType[1] == CARD_BAOZI)
	{
		nResult = 1;
	}
	else if (dwCardType[1] == CARD_SPECIAL && dwCardType[0] == CARD_BAOZI)
	{
		nResult = -1;
	}
	else {
		BYTE *newFirstCardData = new BYTE[cbFirstCardCount];
		CopyMemory(newFirstCardData,cbFirstCardData,cbFirstCardCount);
		SortCardList(newFirstCardData, cbFirstCardCount, ST_LOGIC);
		BYTE *newSecondCardData = new BYTE[cbNextCardCount];
		CopyMemory(newSecondCardData,cbNextCardData,cbNextCardCount);
		SortCardList(newSecondCardData, cbNextCardCount, ST_LOGIC);
		if (dwCardType[0] != dwCardType[1])
		{
			DWORD winCardType = dwCardType[0];
			BYTE *winCardData = new BYTE[MAX_COUNT];
			if (dwCardType[0] < dwCardType[1])
			{
				nResult = 1;
				winCardType = dwCardType[0];
				CopyMemory(winCardData,newFirstCardData,cbFirstCardCount);
			}
			else
			{
				nResult = -1;
				winCardType = dwCardType[1];
				CopyMemory(winCardData,newSecondCardData,cbNextCardCount);
			}
			if (winCardType <= CARD_DUIZI)
			{
				if (winCardType == CARD_DUIZI) 
				{
					if (GetCardLogicValue(winCardData[1]) >= 9)
					{
						pWinArea[1] = true;
					}
				}
				else
				{
					pWinArea[1] = true;
				}
			}

		}
		else
		{
			if (dwCardType[0] == CARD_SPECIAL)
				nResult = GetCardColor(newFirstCardData[2]) > GetCardColor(newSecondCardData[2]) ? 1: -1;
			else if (dwCardType[0] == CARD_BAOZI)
			{
				nResult = GetCardLogicValue(newFirstCardData[0]) > GetCardLogicValue(newSecondCardData[0]) ? 1: -1;
				pWinArea[1] = true;
			}
			else if (dwCardType[0] == CARD_TONGHUASHUN)
			{
				if (GetCardLogicValue(newFirstCardData[2]) == 14 && GetCardLogicValue(newFirstCardData[1]) == 3)
				{
					if (GetCardLogicValue(newSecondCardData[2] == 14 && GetCardLogicValue(newSecondCardData[1]) == 3))
					{
						nResult = GetCardColor(newFirstCardData[2]) > GetCardColor(newSecondCardData[2]) ? 1: -1;
					} else 
					{
						nResult = -1;
					}
				} else if (GetCardLogicValue(newSecondCardData[2]) == 14 && GetCardLogicValue(newSecondCardData[1]) == 3)
				{
					nResult = 1;
				} else
				{
					nResult = CompareCardCycle(newFirstCardData, newSecondCardData, MAX_COUNT-1);
				}
				pWinArea[1] = true;
			}
			else if (dwCardType[0] == CARD_JINHUA)
			{
				nResult = CompareCardCycle(newFirstCardData, newSecondCardData, 0);
				pWinArea[1] = true;
			}
			else if (dwCardType[0] == CARD_SHUNZI)
			{
				if (GetCardLogicValue(newFirstCardData[2]) == 14 && GetCardLogicValue(newFirstCardData[1]) == 3)
				{
					if (GetCardLogicValue(newSecondCardData[2] == 14 && GetCardLogicValue(newSecondCardData[1]) == 3))
					{
						nResult = GetCardColor(newFirstCardData[2]) > GetCardColor(newSecondCardData[2]) ? 1: -1;
					} else 
					{
						nResult = -1;
					}
				} else if (GetCardLogicValue(newSecondCardData[2]) == 14 && GetCardLogicValue(newSecondCardData[1]) == 3)
				{
					nResult = 1;
				} else
				{
					nResult = CompareCardCycle(newFirstCardData, newSecondCardData, MAX_COUNT-1);
				}
				pWinArea[1] = true; 
			}
			else if (dwCardType[0] == CARD_DUIZI)
			{
				BYTE duiziBigValue = 0;
				if (GetCardLogicValue(newFirstCardData[1]) == GetCardLogicValue(newSecondCardData[1]))
				{
					BYTE compareIndex = 0;
					if (GetCardLogicValue(newFirstCardData[0]) == GetCardLogicValue(newFirstCardData[1]))
						compareIndex = 2;
					if (GetCardLogicValue(newFirstCardData[compareIndex]) == GetCardLogicValue(newSecondCardData[compareIndex]))
						nResult = GetCardColor(newFirstCardData[compareIndex]) > GetCardColor(newSecondCardData[compareIndex]) ? 1: -1;
					else 
						nResult = GetCardLogicValue(newFirstCardData[compareIndex]) > GetCardLogicValue(newSecondCardData[compareIndex]) ? 1: -1;
					duiziBigValue = GetCardLogicValue(newFirstCardData[1]);

				}
				else
				{
					nResult = GetCardLogicValue(newFirstCardData[1]) > GetCardLogicValue(newSecondCardData[1]) ? 1: -1;
					duiziBigValue = __max(GetCardLogicValue(newFirstCardData[1]), GetCardLogicValue(newSecondCardData[1]));
				}
				if (duiziBigValue >= 9)
				{
					pWinArea[1] = true;
				}
			}
			else
				nResult = CompareCardCycle(newFirstCardData, newSecondCardData, 0);
		}
	}
	BYTE cbWinType = 0;
	if(nResult == 1)
	{
		pWinArea[0] = true;
		cbWinType = dwCardType[0];
	}
	else
	{
		pWinArea[2] = true;
		cbWinType = dwCardType[1];
	}
	return nResult;
}

int CGameLogic::CompareCardCycle(const BYTE firstCardData[], const BYTE secondCardData[], BYTE maxIndex)
{
	int nResult=0;
	BYTE index = 2;
	do 
	{
		if (GetCardLogicValue(firstCardData[index]) == GetCardLogicValue(secondCardData[index]))
		{
			index--;
		} else
		{
			if (GetCardLogicValue(firstCardData[index]) > GetCardLogicValue(secondCardData[index]))
				nResult = 1;
			else
				nResult = -1;
			break;
		}
	} while (index >= maxIndex);
	if (nResult == 0)
		nResult = GetCardColor(firstCardData[2]) > GetCardColor(secondCardData[2]) ? 1: -1;
	return nResult;
}


//逻辑大小
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE bCardValue=GetCardValue(cbCardData);

	//转换数值
	return (bCardValue==1)?(bCardValue+13):bCardValue;
}

//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//数目过虑
	if (cbCardCount==0) return;

	//转换数值
	BYTE cbSortValue[CARD_COUNT];
	if (ST_VALUE==cbSortType)
	{
		for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardValue(cbCardData[i]);	
	}
	else 
	{
		for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	
	}

	//排序操作
	bool bSorted=true;
	BYTE cbThreeCount,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if ((cbSortValue[i]>cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]>cbCardData[i+1])))
			{
				//交换位置
				cbThreeCount=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbThreeCount;
				cbThreeCount=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbThreeCount;
				bSorted=false;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	return;
}

//////////////////////////////////////////////////////////////////////////

