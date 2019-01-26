#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

const BYTE CGameLogic::m_cbCardListData[CARD_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//���� A - K
		
};
//////////////////////////////////////////////////////////////////////////

//�����˿�
void RandCount(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE *cbCardData = new BYTE[cbBufferCount];
	CopyMemory(cbCardData,cbCardBuffer,cbBufferCount);

	//�����˿�
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


//���캯��
CGameLogic::CGameLogic()
{
}

//��������
CGameLogic::~CGameLogic()
{
}

//�����˿�
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardListData)];
	CopyMemory(cbCardData,m_cbCardListData,sizeof(m_cbCardListData));

	//�����˿�
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(CountArray(cbCardData)-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[CountArray(cbCardData)-cbRandCount];
		//cbPosition++;
	} while (cbRandCount<cbBufferCount);


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
	//Ч�����
	ASSERT(cbCardCount==MAX_COUNT);
	if(cbCardCount!=MAX_COUNT)return 0;

	DWORD dwCardType=0;

	//����
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
			//2+8���
			if (LittleValue==2)
			{
				return 11;
			}
			//���಻�ִ�С
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

//��ȡ����
DWORD CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{

	//Ч�����
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
	//Ч�����
	ASSERT(cbCardCount==MAX_COUNT);
	if(cbCardCount!=MAX_COUNT)return 0;
	//����
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
		//2+8���
		if (LittleValue==2)
		{
			return 10;
		}
		//���಻�ִ�С
		else
		{
			return 0;
		}
	}
	return maxValue;
}
//��С�Ƚ�
/*
cbNextCardData>cbFirstCardData  ����1
cbNextCardData<cbFirstCardData  ����-1
cbNextCardData==cbFirstCardData ����0
*/
BYTE CGameLogic::CompareCard(const BYTE cbFirstCardData[], BYTE cbFirstCardCount,const BYTE cbNextCardData[], BYTE cbNextCardCount, BYTE pWinArea[])
{
	//�Ϸ��ж�
	ASSERT(MAX_COUNT==cbFirstCardCount && MAX_COUNT==cbNextCardCount);
	if (!(MAX_COUNT==cbFirstCardCount && MAX_COUNT==cbNextCardCount)) return 0;

	DWORD dwCardType[2]={GetCardType(cbFirstCardData,MAX_COUNT),GetCardType(cbNextCardData,MAX_COUNT)};

	//printf("�����������===== %d \n",dwCardType[0]);
	//printf("�����������===== %d \n",dwCardType[1]);

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
						pWinArea[winCardType] = true;
						pWinArea[1] = true;
					}
				}
				else
				{
					pWinArea[winCardType] = true;
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
				pWinArea[dwCardType[0]] = true;
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
				pWinArea[dwCardType[0]] = true;
				pWinArea[1] = true;
			}
			else if (dwCardType[0] == CARD_JINHUA)
			{
				nResult = CompareCardCycle(newFirstCardData, newSecondCardData, 0);
				pWinArea[dwCardType[0]] = true;
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
				pWinArea[dwCardType[0]] = true;
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
					pWinArea[dwCardType[0]] = true;
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
	return cbWinType;

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

//�߼���С
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE bCardValue=GetCardValue(cbCardData);

	//ת����ֵ
	return (bCardValue==1)?(bCardValue+13):bCardValue;
}

//�����˿�(С����)
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//��Ŀ����
	if (cbCardCount==0) return;

	//ת����ֵ
	BYTE cbSortValue[CARD_COUNT];
	if (ST_VALUE==cbSortType)
	{
		for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardValue(cbCardData[i]);	
	}
	else 
	{
		for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	
	}

	//�������
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
				//����λ��
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

