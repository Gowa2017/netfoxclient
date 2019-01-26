#include "StdAfx.h"
#include "GameLogic.h"
//////////////////////////////////////////////////////////////////////////////////
//��̬����

//��������
const BYTE cbIndexCount=5;

//�˿�����
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//���� A - K
	0x4E,0x4F,
};

//const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
//{
//	0x01,0x11,0x21,0x31,0x1B,0x06,0x07,0x08,0x2A,0x0A,0x0B,0x0C,0x0D,0x02,0x12,0x27,0x28,
//	0x15,0x16,0x17,0x18,0x35,0x1A,0x05,0x1C,0x1D,0x03,0x22,0x23,0x24,0x25,0x26,0x4E,0x4F,
//	0x29,0x09,0x2B,0x2C,0x2D,0x04,0x32,0x33,0x34,0x19,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,
//	0x3D,0x13,0x14,
//};
//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
}

//��������
CGameLogic::~CGameLogic()
{
}

//��ȡ����
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//������
	switch (cbCardCount)
	{
	case 0:	//����
		{
			return CT_ERROR;
		}
	case 1: //����
		{
			return CT_SINGLE;
		}
	case 2:	//���ƻ��
		{
			//�����ж�
			if (((cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E))
				|| ((cbCardData[0]==0x4E)&&(cbCardData[1]==0x4F))) return CT_MISSILE_CARD;
			if (GetCardLogicValue(cbCardData[0])==GetCardLogicValue(cbCardData[1])) return CT_DOUBLE;

			return CT_ERROR;
		}
	}

	//�����˿�
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//�����ж�
	if (AnalyseResult.cbBlockCount[3]>0)
	{
		//�����ж�
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==4)) return CT_BOMB_CARD;
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==6)) return CT_FOUR_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[3]==1)&&(cbCardCount==8)&&(AnalyseResult.cbBlockCount[1]==2)) return CT_FOUR_TAKE_TWO;

		return CT_ERROR;
	}

	//�����ж�
	if (AnalyseResult.cbBlockCount[2]>0)
	{
		//�����ж�
		if (AnalyseResult.cbBlockCount[2]>1)
		{
			//��������
			BYTE cbCardData=AnalyseResult.cbCardData[2][0];
			BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

			//�������
			if (cbFirstLogicValue>=15) return CT_ERROR;

			//�����ж�
			for (BYTE i=1;i<AnalyseResult.cbBlockCount[2];i++)
			{
				BYTE cbCardData=AnalyseResult.cbCardData[2][i*3];
				if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
			}
		}
		else if( cbCardCount == 3 ) return CT_THREE;

		//�����ж�
		if (AnalyseResult.cbBlockCount[2]*3==cbCardCount) return CT_THREE_LINE;
		if (AnalyseResult.cbBlockCount[2]*4==cbCardCount) return CT_THREE_TAKE_ONE;
		if ((AnalyseResult.cbBlockCount[2]*5==cbCardCount)&&(AnalyseResult.cbBlockCount[1]==AnalyseResult.cbBlockCount[2])) return CT_THREE_TAKE_TWO;

		return CT_ERROR;
	}

	//��������
	if (AnalyseResult.cbBlockCount[1]>=3)
	{
		//��������
		BYTE cbCardData=AnalyseResult.cbCardData[1][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//�����ж�
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[1];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[1][i*2];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		//�����ж�
		if ((AnalyseResult.cbBlockCount[1]*2)==cbCardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//�����ж�
	if ((AnalyseResult.cbBlockCount[0]>=5)&&(AnalyseResult.cbBlockCount[0]==cbCardCount))
	{
		//��������
		BYTE cbCardData=AnalyseResult.cbCardData[0][0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//�����ж�
		for (BYTE i=1;i<AnalyseResult.cbBlockCount[0];i++)
		{
			BYTE cbCardData=AnalyseResult.cbCardData[0][i];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

//�����˿�
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//��Ŀ����
	if (cbCardCount==0) return;
	if (cbSortType==ST_CUSTOM) return;

	//ת����ֵ
	BYTE cbSortValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	

	//�������
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if ((cbSortValue[i]<cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//���ñ�־
				bSorted=false;

				//�˿�����
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;

				//����Ȩλ
				cbSwitchData=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	//��Ŀ����
	if (cbSortType==ST_COUNT)
	{
		//��������
		BYTE cbCardIndex=0;

		//�����˿�
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(&cbCardData[cbCardIndex],cbCardCount-cbCardIndex,AnalyseResult);

		//��ȡ�˿�
		for (BYTE i=0;i<CountArray(AnalyseResult.cbBlockCount);i++)
		{
			//�����˿�
			BYTE cbIndex=CountArray(AnalyseResult.cbBlockCount)-i-1;
			CopyMemory(&cbCardData[cbCardIndex],AnalyseResult.cbCardData[cbIndex],AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE));

			//��������
			cbCardIndex+=AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE);
		}
	}

	return;
}

//�����˿�
VOID CGameLogic::RandCardList(BYTE cbCardBuffer1[], BYTE cbBufferCount)
{
	BYTE cbCardBuffer[FULL_COUNT];
	ZeroMemory(cbCardBuffer,sizeof(cbCardBuffer));
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

	// ���ը����
	BYTE  cbBombNum = 0;
	int iBombRandNum = 0;
	iBombRandNum = rand()%1000;
	BYTE cbPlayerBombArray[3][4];
	ZeroMemory(cbPlayerBombArray,sizeof(cbPlayerBombArray));
	if (iBombRandNum < 5){
		cbBombNum = 8;
	} else if (iBombRandNum < 20) 
	{
		cbBombNum = 7;
	} 
	else if (iBombRandNum < 100) 
	{
		cbBombNum = 6;
	}
	else if (iBombRandNum < 300) 
	{
		cbBombNum = 5;
	}
	else if (iBombRandNum < 500) 
	{
		cbBombNum = 4;
	}
	else if (iBombRandNum < 700) 
	{
		cbBombNum = 3;
	}
	else if (iBombRandNum < 900) 
	{
		cbBombNum = 2;
	} 
	else if (iBombRandNum < 1000) 
	{
		cbBombNum = 1;
	}
	bool bIsMissile = false;
	iBombRandNum = rand()%100;
	if (iBombRandNum < 60) {
		bIsMissile = true;
		cbBombNum = cbBombNum - 1;
	}
	BYTE cbBombValueArray[13] = {3,4,5,6,7,8,9,10,11,12,13,14,15};
	BYTE cbBombValueCount = 13;
	//�����˿�
	BYTE cbPositionTemp = 0, cbRandCountTemp = 0;
	BYTE cbBombNumTemp = cbBombNum;
	BYTE cbMaxBombNumTemp = 0;
	BYTE cbMaxBombPlayerIndex = 0;
	BYTE playerMaxNum = 0,playerMinNum = 0;
	if (cbBombNum <= 2)
		playerMaxNum = 1;
	else if (cbBombNum <= 4)
		playerMaxNum = 2;
	else
		playerMaxNum = 3;

	if (cbBombNum <= 3)
		playerMinNum = 0;
	else if (cbBombNum <= 6)
		playerMinNum = 1;
	else
		playerMinNum = 2;
	BYTE playerCardNumArray[3];
	ZeroMemory(playerCardNumArray,sizeof(playerCardNumArray));
	for (BYTE i = 0; i< 3; i++)
	{
		if (cbBombNumTemp == 0)
			break;
		BYTE cbPlayerBombNum;
		if (i==2)
			cbPlayerBombNum = cbBombNumTemp;
		else
		{
			BYTE minTemp = __min(cbBombNumTemp,playerMaxNum);
			if (i == 1)
			{
				minTemp = __min(minTemp, cbBombNumTemp-playerMinNum);
			}
			BYTE cbRandomNum = rand()%(minTemp-playerMinNum+1) + playerMinNum;
			cbBombNumTemp = cbBombNumTemp - cbRandomNum;
			cbPlayerBombNum = cbRandomNum;
		}
		if (cbPlayerBombNum == 0)
			continue;
		if (cbPlayerBombNum > cbMaxBombNumTemp)
		{
			cbMaxBombNumTemp = cbPlayerBombNum;
			cbMaxBombPlayerIndex = i;
		}
		BYTE bombIndex = 0;
		do
		{
			cbPositionTemp=rand()%(cbBombValueCount-cbRandCountTemp);
			BYTE bombValue = cbBombValueArray[cbPositionTemp];
			BYTE cbBombValueSmallIndexArray[13];
			ZeroMemory(cbBombValueSmallIndexArray, sizeof(cbBombValueSmallIndexArray));
			BYTE cbBombValueCopyIndex = 0;
			// ����ȫ���Ǵ�ը��J��
			if (cbPlayerBombArray[i][0] >= 11 && bombValue >= 11)
			{
				if (cbPlayerBombNum == 1 && (cbPlayerBombArray[i][1] == 0 || cbPlayerBombArray[i][1] >= 11))
				{
					for (BYTE tempIndex=0; tempIndex < (cbBombValueCount-cbRandCountTemp); tempIndex ++)
					{
						if (cbBombValueArray[tempIndex] <11 )
						{
							cbBombValueSmallIndexArray[cbBombValueCopyIndex] = tempIndex;
							cbBombValueCopyIndex++;
						}
					}
					if (cbBombValueCopyIndex > 0)
					{
						BYTE cbBombSmallPosition = rand()%(cbBombValueCopyIndex);
						cbPositionTemp = cbBombValueSmallIndexArray[cbBombSmallPosition];
						bombValue = cbBombValueArray[cbPositionTemp];
					}
				}
			}
			if (bombIndex == 2)
			{
				// ������˳ը
				BYTE cbBombValueStraightIndexArray[13];
				ZeroMemory(cbBombValueStraightIndexArray, sizeof(cbBombValueStraightIndexArray));
				BYTE cbBombValueStraightIndex = 0;
				for (BYTE tempIndex=0; tempIndex < (cbBombValueCount-cbRandCountTemp); tempIndex ++)
				{
					BOOL isNear = false;
					if (cbPlayerBombArray[i][0] > cbPlayerBombArray[i][1])
					{
						if (cbPlayerBombArray[i][0] - cbPlayerBombArray[i][1] == 1 && (cbBombValueArray[tempIndex] == cbPlayerBombArray[i][0] + 1 || cbBombValueArray[tempIndex] == cbPlayerBombArray[i][1] - 1))
							isNear = true;
						else if (cbPlayerBombArray[i][0] - cbPlayerBombArray[i][1] == 2 && cbBombValueArray[tempIndex] == cbPlayerBombArray[i][0] - 1)
							isNear = true;
					} else {
						if (cbPlayerBombArray[i][1] - cbPlayerBombArray[i][0] == 1 && (cbBombValueArray[tempIndex] == cbPlayerBombArray[i][1] + 1 || cbBombValueArray[tempIndex] == cbPlayerBombArray[i][0] - 1))
							isNear = true;
						else if (cbPlayerBombArray[i][1] - cbPlayerBombArray[i][0] == 2 && cbBombValueArray[tempIndex] == cbPlayerBombArray[i][0] + 1)
							isNear = true;
					}
					if (isNear == false)
					{
						if (cbBombValueCopyIndex > 0)
						{
							for (BYTE tempIndex1=0; tempIndex1 < cbBombValueCopyIndex; tempIndex1++)
							{
								if (cbBombValueSmallIndexArray[tempIndex1] == tempIndex)
								{
									cbBombValueStraightIndexArray[cbBombValueStraightIndex] = tempIndex;
									cbBombValueStraightIndex++;
									break;
								}
							}
						} else {
							cbBombValueStraightIndexArray[cbBombValueStraightIndex] = tempIndex;
							cbBombValueStraightIndex++;
						}
					}
				}
				if (cbBombValueStraightIndex > 0)
				{
					BYTE cbBombSmallPosition = rand()%(cbBombValueStraightIndex);
					cbPositionTemp = cbBombValueStraightIndexArray[cbBombSmallPosition];
					bombValue = cbBombValueArray[cbPositionTemp];
				}
			}
			cbPlayerBombArray[i][bombIndex] = bombValue;
			bombIndex ++;
			cbRandCountTemp ++;
			cbPlayerBombNum --;
			playerCardNumArray[i] = playerCardNumArray[i]+4;
			cbBombValueArray[cbPositionTemp]=cbBombValueArray[cbBombValueCount-cbRandCountTemp];
		} while (cbRandCountTemp<cbBombValueCount && cbPlayerBombNum > 0);
	}
	if (bIsMissile)
	{
		//BYTE randTemp;
		//for (BYTE j = 0; j< 3; j++)
		//{
		//	randTemp = rand()%100;
		//	if (randTemp < 50)
		//	{
		//		BYTE playerBombNum = CountArray(cbPlayerBombArray[j]);
		//		cbPlayerBombArray[j][playerBombNum] = 16;
		//		playerCardNumArray[j] = playerCardNumArray[j]+2;
		//		break;
		//	}
		//}
		BYTE missilePlayerArray[2];
		BYTE missileIndexTemp = 0;
		BYTE missileIndex = 0;
		for (BYTE k=0; k<3; k++)
		{
			if (k==cbMaxBombPlayerIndex)
				continue;
			missilePlayerArray[missileIndexTemp] = k;
			missileIndexTemp++;
		}
		if (playerCardNumArray[missilePlayerArray[0]] == cbMaxBombNumTemp)
		{
			missileIndex = missilePlayerArray[1];
		}
		else if (playerCardNumArray[missilePlayerArray[1]] == cbMaxBombNumTemp)
		{
			missileIndex = missilePlayerArray[0];
		} 
		else
		{
			BYTE randTemp = rand()%100;
			if (randTemp < 50)
				missileIndex = missilePlayerArray[0];
			else
				missileIndex = missilePlayerArray[1];
		}
		cbPlayerBombArray[missileIndex][(INT)(playerCardNumArray[missileIndex]/4)] = 16;
		playerCardNumArray[missileIndex] = playerCardNumArray[missileIndex]+2;
	}
	// �������
	BYTE cbPlayerThreeArray[3][4];
	ZeroMemory(cbPlayerThreeArray,sizeof(cbPlayerThreeArray));
	BYTE cbThreeNum = rand()%3 + 2;
	cbThreeNum = __min(cbThreeNum, cbBombValueCount-cbRandCountTemp);
	BYTE cbThreeNumTemp = cbThreeNum;
	for (BYTE k = 0; k< 3; k++)
	{
		BYTE cbPlayerThreeNumMax = __min(cbThreeNumTemp, (INT)((NORMAL_COUNT-playerCardNumArray[k])/3));
		BYTE cbPlayerThreeNum = rand()%(cbPlayerThreeNumMax+1);
		if (k == 2)
			cbPlayerThreeNum = cbPlayerThreeNumMax;
		cbThreeNumTemp = cbThreeNumTemp - cbPlayerThreeNum;
		BYTE threeIndex = 0;
		while (cbRandCountTemp<cbBombValueCount && cbPlayerThreeNum > 0)
		{
			cbPositionTemp=rand()%(cbBombValueCount-cbRandCountTemp);
			BYTE threeValue = cbBombValueArray[cbPositionTemp];
			cbPlayerThreeArray[k][threeIndex] = threeValue;
			threeIndex ++;
			cbRandCountTemp ++;
			cbPlayerThreeNum --;
			playerCardNumArray[k] = playerCardNumArray[k] + 3;
			cbBombValueArray[cbPositionTemp]=cbBombValueArray[cbBombValueCount-cbRandCountTemp];
		};
	}
	BYTE allCardNum = 0;
	for (BYTE j=0; j<3; j++)
	{
		BYTE playerCardNum = 0;
		BYTE cbPlayerBombArrayLen = CountArray(cbPlayerBombArray[j]);
		for (BYTE m=0; m<cbPlayerBombArrayLen; m++)
		{
			BYTE bombValue = cbPlayerBombArray[j][m];
			if (bombValue == 0)
				break;
			if (bombValue == 16)
			{
				// ��С��
				BYTE temp1 = cbCardData[FULL_COUNT-2];
				if (temp1 == 0)
				{
					BYTE temp11 = 0;
					BYTE temp12 = 0;
				}
				cbCardBuffer[j*NORMAL_COUNT+playerCardNum] = cbCardData[FULL_COUNT-2];
				cbCardData[FULL_COUNT-2] = 0;
				playerCardNum++;
				BYTE temp2 = cbCardData[FULL_COUNT-1];
				if (temp2 == 0)
				{
					BYTE temp11 = 0;
					BYTE temp12 = 0;
				}
				cbCardBuffer[j*NORMAL_COUNT+playerCardNum] = cbCardData[FULL_COUNT-1];
				cbCardData[FULL_COUNT-1] = 0;
				playerCardNum++;
			} else {
				if (bombValue > 13)
					bombValue = bombValue - 13;
				for (BYTE n=0; n< 4; n++)
				{
					BYTE temp3 = cbCardData[13*n+bombValue-1];
					if (temp3 == 0)
					{
						BYTE temp11 = 0;
						BYTE temp12 = 0;
					}
					cbCardBuffer[j*NORMAL_COUNT+playerCardNum] = cbCardData[13*n+bombValue-1];
					cbCardData[13*n+bombValue-1] = 0;
					playerCardNum++;
				}
			}
		}
		BYTE cbPlayerThreeArrayLen = CountArray(cbPlayerThreeArray[j]);
		for (BYTE x=0; x<cbPlayerThreeArrayLen; x++)
		{
			BYTE threeValue = cbPlayerThreeArray[j][x];
			if (threeValue == 0)
				break;
			if (threeValue > 13)
				threeValue = threeValue - 13;
			BYTE noThreeIndex = rand()%4; 
			for (BYTE y=0; y< 4; y++)
			{
				if (y == noThreeIndex)
					continue;
				BYTE temp4 = cbCardData[13*y+threeValue-1];
				if (temp4 == 0)
				{
					BYTE temp11 = 0;
					BYTE temp12 = 0;
				}
				cbCardBuffer[j*NORMAL_COUNT+playerCardNum] = cbCardData[13*y+threeValue-1];
				cbCardData[13*y+threeValue-1] = 0;
				playerCardNum++;
			}
		}
		allCardNum = allCardNum + playerCardNum;
	}
	BYTE m_cbCardDataLen = CountArray(m_cbCardData);
	BYTE cbLeftCardNum = m_cbCardDataLen-allCardNum;
	BYTE cbLeftCardData[FULL_COUNT];
	ZeroMemory(cbLeftCardData,sizeof(cbLeftCardData));
	BYTE cbLeftCardNumTemp = cbLeftCardNum;
	cbLeftCardNumTemp --;
	for (BYTE z=0; z< m_cbCardDataLen; z++)
	{
		if (cbCardData[z] != 0)
		{
			cbLeftCardData[cbLeftCardNumTemp] = cbCardData[z];
			cbLeftCardNumTemp --;
		}
	}
	BYTE playerIndex = 0;
	//�����˿�
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbLeftCardNum-cbRandCount);
		for (BYTE p = playerIndex; p<3; p++)
		{
			if (playerCardNumArray[p] < NORMAL_COUNT || (p==2 && playerCardNumArray[p] < MAX_COUNT)) {
				playerIndex = p;
				break;
			}
		}
		if (p == 3)
			break;
		cbCardBuffer[playerIndex*NORMAL_COUNT+playerCardNumArray[p]] = cbLeftCardData[cbPosition];
		cbRandCount++;
		playerCardNumArray[p] ++;
		cbLeftCardData[cbPosition]=cbLeftCardData[cbLeftCardNum-cbRandCount];
	} while (cbRandCount<cbLeftCardNum);
	CopyMemory(cbCardBuffer1,cbCardBuffer,cbBufferCount);

	return;
}

//ɾ���˿�
bool CGameLogic::RemoveCardList(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//��������
	ASSERT(cbRemoveCount<=cbCardCount);

	//�������
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	ZeroMemory(cbCardData,cbCardCount*sizeof(cbCardData[0]));
	//�����˿�
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//��������
	ASSERT(cbRemoveCount<=cbCardCount);

	//�������
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	//�����˿�
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//�����˿�
VOID CGameLogic::SortOutCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//��ȡ����
	BYTE cbCardType = GetCardType(cbCardData,cbCardCount);

	if( cbCardType == CT_THREE_TAKE_ONE || cbCardType == CT_THREE_TAKE_TWO )
	{
		//������
		tagAnalyseResult AnalyseResult = {};
		AnalysebCardData( cbCardData,cbCardCount,AnalyseResult );

		cbCardCount = AnalyseResult.cbBlockCount[2]*3;
		CopyMemory( cbCardData,AnalyseResult.cbCardData[2],sizeof(BYTE)*cbCardCount );
		for( int i = CountArray(AnalyseResult.cbBlockCount)-1; i >= 0; i-- )
		{
			if( i == 2 ) continue;

			if( AnalyseResult.cbBlockCount[i] > 0 )
			{
				CopyMemory( &cbCardData[cbCardCount],AnalyseResult.cbCardData[i],
					sizeof(BYTE)*(i+1)*AnalyseResult.cbBlockCount[i] );
				cbCardCount += (i+1)*AnalyseResult.cbBlockCount[i];
			}
		}
	}
	else if( cbCardType == CT_FOUR_TAKE_ONE || cbCardType == CT_FOUR_TAKE_TWO )
	{
		//������
		tagAnalyseResult AnalyseResult = {};
		AnalysebCardData( cbCardData,cbCardCount,AnalyseResult );

		cbCardCount = AnalyseResult.cbBlockCount[3]*4;
		CopyMemory( cbCardData,AnalyseResult.cbCardData[3],sizeof(BYTE)*cbCardCount );
		for( int i = CountArray(AnalyseResult.cbBlockCount)-1; i >= 0; i-- )
		{
			if( i == 3 ) continue;

			if( AnalyseResult.cbBlockCount[i] > 0 )
			{
				CopyMemory( &cbCardData[cbCardCount],AnalyseResult.cbCardData[i],
					sizeof(BYTE)*(i+1)*AnalyseResult.cbBlockCount[i] );
				cbCardCount += (i+1)*AnalyseResult.cbBlockCount[i];
			}
		}
	}

	return;
}

//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	if(cbCardValue<=0 || cbCardValue>(MASK_VALUE&0x4f))
		return 0 ;
	ASSERT(cbCardValue>0 && cbCardValue<=(MASK_VALUE&0x4f));

	//ת����ֵ
	if (cbCardColor==0x40) return cbCardValue+2;
	return (cbCardValue<=2)?(cbCardValue+13):cbCardValue;
}

//�Ա��˿�
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//��ȡ����
	BYTE cbNextType=GetCardType(cbNextCard,cbNextCount);
	BYTE cbFirstType=GetCardType(cbFirstCard,cbFirstCount);

	//�����ж�
	if (cbFirstType==CT_MISSILE_CARD) return false;
	if (cbNextType==CT_ERROR) return false;
	if (cbNextType==CT_MISSILE_CARD) return true;

	//ը���ж�
	if ((cbFirstType!=CT_BOMB_CARD)&&(cbNextType==CT_BOMB_CARD)) return true;
	if ((cbFirstType==CT_BOMB_CARD)&&(cbNextType!=CT_BOMB_CARD)) return false;

	//�����ж�
	if ((cbFirstType!=cbNextType)||(cbFirstCount!=cbNextCount)) return false;

	//��ʼ�Ա�
	switch (cbNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
		{
			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
		{
			//�����˿�
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbCardData[2][0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbCardData[2][0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_FOUR_TAKE_ONE:
	case CT_FOUR_TAKE_TWO:
		{
			//�����˿�
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbCardData[3][0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbCardData[3][0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
	}
	
	return false;
}

//�����˿�
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex<<4)|(cbValueIndex+1);
}

//�����˿�
VOID CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//���ý��
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//�˿˷���
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1,cbCardValueTemp=0;
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		if(cbSameCount > 4)
		{
			ASSERT(FALSE);
			//���ý��
			ZeroMemory(&AnalyseResult, sizeof(AnalyseResult));
			return;
		}

		//���ý��
		BYTE cbIndex=AnalyseResult.cbBlockCount[cbSameCount-1]++;
		for (BYTE j=0;j<cbSameCount;j++) AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j]=cbCardData[i+j];

		//��������
		i+=cbSameCount-1;
	}

	return;
}

//�����ֲ�
VOID CGameLogic::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing)
{
	//���ñ���
	ZeroMemory(&Distributing,sizeof(Distributing));

	//���ñ���
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbCardData[i]==0) continue;

		//��ȡ����
		BYTE cbCardColor=GetCardColor(cbCardData[i]);
		BYTE cbCardValue=GetCardValue(cbCardData[i]);

		//�ֲ���Ϣ
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue-1][cbIndexCount]++;
		Distributing.cbDistributing[cbCardValue-1][cbCardColor>>4]++;
	}

	return;
}

//��������
BYTE CGameLogic::SearchOutCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, 
							   tagSearchCardResult *pSearchCardResult )
{
	//���ý��
	ASSERT( pSearchCardResult != NULL );
	if( pSearchCardResult == NULL ) return 0;

	ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));

	//��������
	BYTE cbResultCount = 0;
	tagSearchCardResult tmpSearchCardResult = {};

	//�����˿�
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//��ȡ����
	BYTE cbTurnOutType=GetCardType(cbTurnCardData,cbTurnCardCount);

	//���Ʒ���
	switch (cbTurnOutType)
	{
	case CT_ERROR:					//��������
		{
			//��ȡ��������һ��
			ASSERT( pSearchCardResult );
			if( !pSearchCardResult ) return 0;

			//�Ƿ�һ�ֳ���
			if( GetCardType(cbCardData,cbCardCount) != CT_ERROR )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = cbCardCount;
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],cbCardData,
					sizeof(BYTE)*cbCardCount );
				cbResultCount++;
			}

			//�����С�Ʋ��ǵ��ƣ�����ȡ
			BYTE cbSameCount = 0;
			if( cbCardCount > 1 && GetCardValue(cbCardData[cbCardCount-1]) == GetCardValue(cbCardData[cbCardCount-2]) )
			{
				cbSameCount = 1;
				pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[cbCardCount-1];
				BYTE cbCardValue = GetCardValue(cbCardData[cbCardCount-1]);
				for( int i = cbCardCount-2; i >= 0; i-- )
				{
					if( GetCardValue(cbCardData[i]) == cbCardValue )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbSameCount++] = cbCardData[i];
					}
					else break;
				}

				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCount;
				cbResultCount++;
			}

			//����
			BYTE cbTmpCount = 0;
			if( cbSameCount != 1 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,1,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//����
			if( cbSameCount != 2 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,2,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//����
			if( cbSameCount != 3 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,3,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//����һ��
			cbTmpCount = SearchTakeCardType( cbCardData,cbCardCount,0,3,1,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//����һ��
			cbTmpCount = SearchTakeCardType( cbCardData,cbCardCount,0,3,2,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//����
			cbTmpCount = SearchLineCardType( cbCardData,cbCardCount,0,1,0,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//����
			cbTmpCount = SearchLineCardType( cbCardData,cbCardCount,0,2,0,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			//����
			cbTmpCount = SearchLineCardType( cbCardData,cbCardCount,0,3,0,&tmpSearchCardResult );
			if( cbTmpCount > 0 )
			{
				pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
					sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
				cbResultCount++;
			}

			////�ɻ�
			//cbTmpCount = SearchThreeTwoLine( cbCardData,cbCardCount,&tmpSearchCardResult );
			//if( cbTmpCount > 0 )
			//{
			//	pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
			//	CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
			//		sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
			//	cbResultCount++;
			//}

			//ը��
			if( cbSameCount != 4 )
			{
				cbTmpCount = SearchSameCard( cbCardData,cbCardCount,0,4,&tmpSearchCardResult );
				if( cbTmpCount > 0 )
				{
					pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[0];
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[0],
						sizeof(BYTE)*tmpSearchCardResult.cbCardCount[0] );
					cbResultCount++;
				}
			}

			//�������
			if ((cbCardCount>=2)&&(cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E))
			{
				//���ý��
				pSearchCardResult->cbCardCount[cbResultCount] = 2;
				pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[0];
				pSearchCardResult->cbResultCard[cbResultCount][1] = cbCardData[1];

				cbResultCount++;
			}

			pSearchCardResult->cbSearchCount = cbResultCount;
			return cbResultCount;
		}
	case CT_SINGLE:					//��������
	case CT_DOUBLE:					//��������
	case CT_THREE:					//��������
		{
			//��������
			BYTE cbReferCard=cbTurnCardData[0];
			BYTE cbSameCount = 1;
			if( cbTurnOutType == CT_DOUBLE ) cbSameCount = 2;
			else if( cbTurnOutType == CT_THREE ) cbSameCount = 3;

			//������ͬ��
			cbResultCount = SearchSameCard( cbCardData,cbCardCount,cbReferCard,cbSameCount,pSearchCardResult );

			break;
		}
	case CT_SINGLE_LINE:		//��������
	case CT_DOUBLE_LINE:		//��������
	case CT_THREE_LINE:				//��������
		{
			//��������
			BYTE cbBlockCount = 1;
			if( cbTurnOutType == CT_DOUBLE_LINE ) cbBlockCount = 2;
			else if( cbTurnOutType == CT_THREE_LINE ) cbBlockCount = 3;

			BYTE cbLineCount = cbTurnCardCount/cbBlockCount;

			//��������
			cbResultCount = SearchLineCardType( cbCardData,cbCardCount,cbTurnCardData[0],cbBlockCount,cbLineCount,pSearchCardResult );

			break;
		}
	case CT_THREE_TAKE_ONE:	//����һ��
	case CT_THREE_TAKE_TWO:	//����һ��
		{
			//Ч������
			if( cbCardCount < cbTurnCardCount ) break;

			//���������һ��������
			if( cbTurnCardCount == 4 || cbTurnCardCount == 5 )
			{
				BYTE cbTakeCardCount = cbTurnOutType==CT_THREE_TAKE_ONE?1:2;

				//������������
				cbResultCount = SearchTakeCardType( cbCardData,cbCardCount,cbTurnCardData[2],3,cbTakeCardCount,pSearchCardResult );
			}
			else
			{
				//��������
				BYTE cbBlockCount = 3;
				BYTE cbLineCount = cbTurnCardCount/(cbTurnOutType==CT_THREE_TAKE_ONE?4:5);
				BYTE cbTakeCardCount = cbTurnOutType==CT_THREE_TAKE_ONE?1:2;

				//��������
				BYTE cbTmpTurnCard[MAX_COUNT] = {};
				CopyMemory( cbTmpTurnCard,cbTurnCardData,sizeof(BYTE)*cbTurnCardCount );
				SortOutCardList( cbTmpTurnCard,cbTurnCardCount );
				cbResultCount = SearchLineCardType( cbCardData,cbCardCount,cbTmpTurnCard[0],cbBlockCount,cbLineCount,pSearchCardResult );

				//��ȡ����
				bool bAllDistill = true;
				for( BYTE i = 0; i < cbResultCount; i++ )
				{
					BYTE cbResultIndex = cbResultCount-i-1;

					//��������
					BYTE cbTmpCardData[MAX_COUNT];
					BYTE cbTmpCardCount = cbCardCount;

					//ɾ������
					CopyMemory( cbTmpCardData,cbCardData,sizeof(BYTE)*cbCardCount );
					VERIFY( RemoveCard( pSearchCardResult->cbResultCard[cbResultIndex],pSearchCardResult->cbCardCount[cbResultIndex],
						cbTmpCardData,cbTmpCardCount ) );
					cbTmpCardCount -= pSearchCardResult->cbCardCount[cbResultIndex];

					//������
					tagAnalyseResult  TmpResult = {};
					AnalysebCardData( cbTmpCardData,cbTmpCardCount,TmpResult );

					//��ȡ��
					BYTE cbDistillCard[MAX_COUNT] = {};
					BYTE cbDistillCount = 0;
					for( BYTE j = cbTakeCardCount-1; j < CountArray(TmpResult.cbBlockCount); j++ )
					{
						if( TmpResult.cbBlockCount[j] > 0 )
						{
							if( j+1 == cbTakeCardCount && TmpResult.cbBlockCount[j] >= cbLineCount )
							{
								BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
								CopyMemory( cbDistillCard,&TmpResult.cbCardData[j][(cbTmpBlockCount-cbLineCount)*(j+1)],
									sizeof(BYTE)*(j+1)*cbLineCount );
								cbDistillCount = (j+1)*cbLineCount;
								break;
							}
							else
							{
								for( BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++ )
								{
									BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
									CopyMemory( &cbDistillCard[cbDistillCount],&TmpResult.cbCardData[j][(cbTmpBlockCount-k-1)*(j+1)],
										sizeof(BYTE)*cbTakeCardCount );
									cbDistillCount += cbTakeCardCount;

									//��ȡ���
									if( cbDistillCount == cbTakeCardCount*cbLineCount ) break;
								}
							}
						}

						//��ȡ���
						if( cbDistillCount == cbTakeCardCount*cbLineCount ) break;
					}

					//��ȡ���
					if( cbDistillCount == cbTakeCardCount*cbLineCount )
					{
						//���ƴ���
						BYTE cbCount = pSearchCardResult->cbCardCount[cbResultIndex];
						CopyMemory( &pSearchCardResult->cbResultCard[cbResultIndex][cbCount],cbDistillCard,
							sizeof(BYTE)*cbDistillCount );
						pSearchCardResult->cbCardCount[cbResultIndex] += cbDistillCount;
					}
					//����ɾ������
					else
					{
						bAllDistill = false;
						pSearchCardResult->cbCardCount[cbResultIndex] = 0;
					}
				}

				//�������
				if( !bAllDistill )
				{
					pSearchCardResult->cbSearchCount = cbResultCount;
					cbResultCount = 0;
					for( BYTE i = 0; i < pSearchCardResult->cbSearchCount; i++ )
					{
						if( pSearchCardResult->cbCardCount[i] != 0 )
						{
							tmpSearchCardResult.cbCardCount[cbResultCount] = pSearchCardResult->cbCardCount[i];
							CopyMemory( tmpSearchCardResult.cbResultCard[cbResultCount],pSearchCardResult->cbResultCard[i],
								sizeof(BYTE)*pSearchCardResult->cbCardCount[i] );
							cbResultCount++;
						}
					}
					tmpSearchCardResult.cbSearchCount = cbResultCount;
					CopyMemory( pSearchCardResult,&tmpSearchCardResult,sizeof(tagSearchCardResult) );
				}
			}

			break;
		}
	case CT_FOUR_TAKE_ONE:		//�Ĵ�����
	case CT_FOUR_TAKE_TWO:		//�Ĵ���˫
		{
			BYTE cbTakeCount = cbTurnOutType==CT_FOUR_TAKE_ONE?1:2;

			BYTE cbTmpTurnCard[MAX_COUNT] = {};
			CopyMemory( cbTmpTurnCard,cbTurnCardData,sizeof(BYTE)*cbTurnCardCount );
			SortOutCardList( cbTmpTurnCard,cbTurnCardCount );

			//��������
			cbResultCount = SearchTakeCardType( cbCardData,cbCardCount,cbTmpTurnCard[0],4,cbTakeCount,pSearchCardResult );

			break;
		}
	}

	//����ը��
	if ((cbCardCount>=4)&&(cbTurnOutType!=CT_MISSILE_CARD))
	{
		//��������
		BYTE cbReferCard = 0;
		if (cbTurnOutType==CT_BOMB_CARD) cbReferCard=cbTurnCardData[0];

		//����ը��
		BYTE cbTmpResultCount = SearchSameCard( cbCardData,cbCardCount,cbReferCard,4,&tmpSearchCardResult );
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSearchCardResult.cbCardCount[i];
			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSearchCardResult.cbResultCard[i],
				sizeof(BYTE)*tmpSearchCardResult.cbCardCount[i] );
			cbResultCount++;
		}
	}

	//�������
	if (cbTurnOutType!=CT_MISSILE_CARD&&(cbCardCount>=2)&&(cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E))
	{
		//���ý��
		pSearchCardResult->cbCardCount[cbResultCount] = 2;
		pSearchCardResult->cbResultCard[cbResultCount][0] = cbCardData[0];
		pSearchCardResult->cbResultCard[cbResultCount][1] = cbCardData[1];

		cbResultCount++;
	}

	pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//ͬ������
BYTE CGameLogic::SearchSameCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount,
								tagSearchCardResult *pSearchCardResult )
{
	//���ý��
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//�����˿�
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//�����˿�
	tagAnalyseResult AnalyseResult = {};
	AnalysebCardData( cbCardData,cbCardCount,AnalyseResult );

	BYTE cbReferLogicValue = cbReferCard==0?0:GetCardLogicValue(cbReferCard);
	BYTE cbBlockIndex = cbSameCardCount-1;
	do
	{
		for( BYTE i = 0; i < AnalyseResult.cbBlockCount[cbBlockIndex]; i++ )
		{
			BYTE cbIndex = (AnalyseResult.cbBlockCount[cbBlockIndex]-i-1)*(cbBlockIndex+1);
			if( GetCardLogicValue(AnalyseResult.cbCardData[cbBlockIndex][cbIndex]) > cbReferLogicValue )
			{
				if( pSearchCardResult == NULL ) return 1;

				ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

				//�����˿�
				CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],&AnalyseResult.cbCardData[cbBlockIndex][cbIndex],
					cbSameCardCount*sizeof(BYTE) );
				pSearchCardResult->cbCardCount[cbResultCount] = cbSameCardCount;

				cbResultCount++;
			}
		}

		cbBlockIndex++;
	}while( cbBlockIndex < CountArray(AnalyseResult.cbBlockCount) );

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//������������(����һ���Ĵ�һ��)
BYTE CGameLogic::SearchTakeCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount, 
									tagSearchCardResult *pSearchCardResult )
{
	//���ý��
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//Ч��
	ASSERT( cbSameCount == 3 || cbSameCount == 4 );
	ASSERT( cbTakeCardCount == 1 || cbTakeCardCount == 2 );
	if( cbSameCount != 3 && cbSameCount != 4 )
		return cbResultCount;
	if( cbTakeCardCount != 1 && cbTakeCardCount != 2 )
		return cbResultCount;

	//�����ж�
	if( cbSameCount == 4 && cbHandCardCount<cbSameCount+cbTakeCardCount*2 ||
		cbHandCardCount < cbSameCount+cbTakeCardCount )
		return cbResultCount;

	//�����˿�
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//����ͬ��
	tagSearchCardResult SameCardResult = {};
	BYTE cbSameCardResultCount = SearchSameCard( cbCardData,cbCardCount,cbReferCard,cbSameCount,&SameCardResult );

	if( cbSameCardResultCount > 0 )
	{
		//�����˿�
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

		//��Ҫ����
		BYTE cbNeedCount = cbSameCount+cbTakeCardCount;
		if( cbSameCount == 4 ) cbNeedCount += cbTakeCardCount;

		//��ȡ����
		for( BYTE i = 0; i < cbSameCardResultCount; i++ )
		{
			bool bMerge = false;

			for( BYTE j = cbTakeCardCount-1; j < CountArray(AnalyseResult.cbBlockCount); j++ )
			{
				for( BYTE k = 0; k < AnalyseResult.cbBlockCount[j]; k++ )
				{
					//��С����
					BYTE cbIndex = (AnalyseResult.cbBlockCount[j]-k-1)*(j+1);

					//������ͬ��
					if( GetCardValue(SameCardResult.cbResultCard[i][0]) ==
						GetCardValue(AnalyseResult.cbCardData[j][cbIndex]) )
						continue;

					//���ƴ���
					BYTE cbCount = SameCardResult.cbCardCount[i];
					CopyMemory( &SameCardResult.cbResultCard[i][cbCount],&AnalyseResult.cbCardData[j][cbIndex],
						sizeof(BYTE)*cbTakeCardCount );
					SameCardResult.cbCardCount[i] += cbTakeCardCount;

					if( SameCardResult.cbCardCount[i] < cbNeedCount ) continue;

					if( pSearchCardResult == NULL ) return 1;

					//���ƽ��
					CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],SameCardResult.cbResultCard[i],
						sizeof(BYTE)*SameCardResult.cbCardCount[i] );
					pSearchCardResult->cbCardCount[cbResultCount] = SameCardResult.cbCardCount[i];
					cbResultCount++;

					bMerge = true;

					//��һ���
					break;
				}

				if( bMerge ) break;
			}
		}
	}

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//��������
BYTE CGameLogic::SearchLineCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
									tagSearchCardResult *pSearchCardResult )
{
	//���ý��
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));
	BYTE cbResultCount = 0;

	//�������
	BYTE cbLessLineCount = 0;
	if( cbLineCount == 0 )
	{
		if( cbBlockCount == 1 )
			cbLessLineCount = 5;
		else if( cbBlockCount == 2 )
			cbLessLineCount = 3;
		else cbLessLineCount = 2;
	}
	else cbLessLineCount = cbLineCount;

	BYTE cbReferIndex = 2;
	if( cbReferCard != 0 )
	{
		ASSERT( GetCardLogicValue(cbReferCard)-cbLessLineCount>=2 );
		cbReferIndex = GetCardLogicValue(cbReferCard)-cbLessLineCount+1;
	}
	//����A
	if( cbReferIndex+cbLessLineCount > 14 ) return cbResultCount;

	//�����ж�
	if( cbHandCardCount < cbLessLineCount*cbBlockCount ) return cbResultCount;

	//�����˿�
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//�����˿�
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//�����˿�
	tagDistributing Distributing = {};
	AnalysebDistributing(cbCardData,cbCardCount,Distributing);

	//����˳��
	BYTE cbTmpLinkCount = 0;
	for (BYTE cbValueIndex=cbReferIndex;cbValueIndex<13;cbValueIndex++)
	{
		//�����ж�
		if ( Distributing.cbDistributing[cbValueIndex][cbIndexCount]<cbBlockCount )
		{
			if( cbTmpLinkCount < cbLessLineCount )
			{
				cbTmpLinkCount=0;
				continue;
			}
			else cbValueIndex--;
		}
		else 
		{
			cbTmpLinkCount++;
			//Ѱ�����
			if( cbLineCount == 0 ) continue;
		}

		if( cbTmpLinkCount >= cbLessLineCount )
		{
			if( pSearchCardResult == NULL ) return 1;

			ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

			//�����˿�
			BYTE cbCount = 0;
			for( BYTE cbIndex = cbValueIndex+1-cbTmpLinkCount; cbIndex <= cbValueIndex; cbIndex++ )
			{
				BYTE cbTmpCount = 0;
				for (BYTE cbColorIndex=0;cbColorIndex<4;cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++]=MakeCardData(cbIndex,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) break;
					}
					if( cbTmpCount == cbBlockCount ) break;
				}
			}

			//���ñ���
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;

			if( cbLineCount != 0 )
			{
				cbTmpLinkCount--;
			}
			else 
			{
				cbTmpLinkCount = 0;
			}
		}
	}

	//����˳��
	if( cbTmpLinkCount >= cbLessLineCount-1 && cbValueIndex == 13 )
	{
		if( Distributing.cbDistributing[0][cbIndexCount] >= cbBlockCount ||
			cbTmpLinkCount >= cbLessLineCount )
		{
			if( pSearchCardResult == NULL ) return 1;

			ASSERT( cbResultCount < CountArray(pSearchCardResult->cbCardCount) );

			//�����˿�
			BYTE cbCount = 0;
			BYTE cbTmpCount = 0;
			for( BYTE cbIndex = cbValueIndex-cbTmpLinkCount; cbIndex < 13; cbIndex++ )
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex=0;cbColorIndex<4;cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[cbIndex][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++]=MakeCardData(cbIndex,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) break;
					}
					if( cbTmpCount == cbBlockCount ) break;
				}
			}
			//����A
			if( Distributing.cbDistributing[0][cbIndexCount] >= cbBlockCount )
			{
				cbTmpCount = 0;
				for (BYTE cbColorIndex=0;cbColorIndex<4;cbColorIndex++)
				{
					for( BYTE cbColorCount = 0; cbColorCount < Distributing.cbDistributing[0][3-cbColorIndex]; cbColorCount++ )
					{
						pSearchCardResult->cbResultCard[cbResultCount][cbCount++]=MakeCardData(0,3-cbColorIndex);

						if( ++cbTmpCount == cbBlockCount ) break;
					}
					if( cbTmpCount == cbBlockCount ) break;
				}
			}

			//���ñ���
			pSearchCardResult->cbCardCount[cbResultCount] = cbCount;
			cbResultCount++;
		}
	}

	if( pSearchCardResult )
		pSearchCardResult->cbSearchCount = cbResultCount;
	return cbResultCount;
}

//�����ɻ�
BYTE CGameLogic::SearchThreeTwoLine( const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult )
{
	//���ý��
	if( pSearchCardResult )
		ZeroMemory(pSearchCardResult,sizeof(tagSearchCardResult));

	//��������
	tagSearchCardResult tmpSearchResult = {};
	tagSearchCardResult tmpSingleWing = {};
	tagSearchCardResult tmpDoubleWing = {};
	BYTE cbTmpResultCount = 0;

	//��������
	cbTmpResultCount = SearchLineCardType( cbHandCardData,cbHandCardCount,0,3,0,&tmpSearchResult );

	if( cbTmpResultCount > 0 )
	{
		//��ȡ����
		for( BYTE i = 0; i < cbTmpResultCount; i++ )
		{
			//��������
			BYTE cbTmpCardData[MAX_COUNT];
			BYTE cbTmpCardCount = cbHandCardCount;

			//������
			if( cbHandCardCount-tmpSearchResult.cbCardCount[i] < tmpSearchResult.cbCardCount[i]/3 )
			{
				BYTE cbNeedDelCount = 3;
				while( cbHandCardCount+cbNeedDelCount-tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i]-cbNeedDelCount)/3 )
					cbNeedDelCount += 3;
				//��������
				if( (tmpSearchResult.cbCardCount[i]-cbNeedDelCount)/3 < 2 )
				{
					//�ϳ�����
					continue;
				}

				//�������
				RemoveCard( tmpSearchResult.cbResultCard[i],cbNeedDelCount,tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i] );
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;
			}

			if( pSearchCardResult == NULL ) return 1;

			//ɾ������
			CopyMemory( cbTmpCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount );
			VERIFY( RemoveCard( tmpSearchResult.cbResultCard[i],tmpSearchResult.cbCardCount[i],
				cbTmpCardData,cbTmpCardCount ) );
			cbTmpCardCount -= tmpSearchResult.cbCardCount[i];

			//��Ϸɻ�
			BYTE cbNeedCount = tmpSearchResult.cbCardCount[i]/3;
			ASSERT( cbNeedCount <= cbTmpCardCount );

			BYTE cbResultCount = tmpSingleWing.cbSearchCount++;
			CopyMemory( tmpSingleWing.cbResultCard[cbResultCount],tmpSearchResult.cbResultCard[i],
				sizeof(BYTE)*tmpSearchResult.cbCardCount[i] );
			CopyMemory( &tmpSingleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
				&cbTmpCardData[cbTmpCardCount-cbNeedCount],sizeof(BYTE)*cbNeedCount );
			tmpSingleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i]+cbNeedCount;

			//���������
			if( cbTmpCardCount < tmpSearchResult.cbCardCount[i]/3*2 )
			{
				BYTE cbNeedDelCount = 3;
				while( cbTmpCardCount+cbNeedDelCount-tmpSearchResult.cbCardCount[i] < (tmpSearchResult.cbCardCount[i]-cbNeedDelCount)/3*2 )
					cbNeedDelCount += 3;
				//��������
				if( (tmpSearchResult.cbCardCount[i]-cbNeedDelCount)/3 < 2 )
				{
					//�ϳ�����
					continue;
				}

				//�������
				RemoveCard( tmpSearchResult.cbResultCard[i],cbNeedDelCount,tmpSearchResult.cbResultCard[i],
					tmpSearchResult.cbCardCount[i] );
				tmpSearchResult.cbCardCount[i] -= cbNeedDelCount;

				//����ɾ������
				CopyMemory( cbTmpCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount );
				VERIFY( RemoveCard( tmpSearchResult.cbResultCard[i],tmpSearchResult.cbCardCount[i],
					cbTmpCardData,cbTmpCardCount ) );
				cbTmpCardCount = cbHandCardCount-tmpSearchResult.cbCardCount[i];
			}

			//������
			tagAnalyseResult  TmpResult = {};
			AnalysebCardData( cbTmpCardData,cbTmpCardCount,TmpResult );

			//��ȡ���
			BYTE cbDistillCard[MAX_COUNT] = {};
			BYTE cbDistillCount = 0;
			BYTE cbLineCount = tmpSearchResult.cbCardCount[i]/3;
			for( BYTE j = 1; j < CountArray(TmpResult.cbBlockCount); j++ )
			{
				if( TmpResult.cbBlockCount[j] > 0 )
				{
					if( j+1 == 2 && TmpResult.cbBlockCount[j] >= cbLineCount )
					{
						BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
						CopyMemory( cbDistillCard,&TmpResult.cbCardData[j][(cbTmpBlockCount-cbLineCount)*(j+1)],
							sizeof(BYTE)*(j+1)*cbLineCount );
						cbDistillCount = (j+1)*cbLineCount;
						break;
					}
					else
					{
						for( BYTE k = 0; k < TmpResult.cbBlockCount[j]; k++ )
						{
							BYTE cbTmpBlockCount = TmpResult.cbBlockCount[j];
							CopyMemory( &cbDistillCard[cbDistillCount],&TmpResult.cbCardData[j][(cbTmpBlockCount-k-1)*(j+1)],
								sizeof(BYTE)*2 );
							cbDistillCount += 2;

							//��ȡ���
							if( cbDistillCount == 2*cbLineCount ) break;
						}
					}
				}

				//��ȡ���
				if( cbDistillCount == 2*cbLineCount ) break;
			}

			//��ȡ���
			if( cbDistillCount == 2*cbLineCount )
			{
				//���Ƴ��
				cbResultCount = tmpDoubleWing.cbSearchCount++;
				CopyMemory( tmpDoubleWing.cbResultCard[cbResultCount],tmpSearchResult.cbResultCard[i],
					sizeof(BYTE)*tmpSearchResult.cbCardCount[i] );
				CopyMemory( &tmpDoubleWing.cbResultCard[cbResultCount][tmpSearchResult.cbCardCount[i]],
					cbDistillCard,sizeof(BYTE)*cbDistillCount );
				tmpDoubleWing.cbCardCount[i] = tmpSearchResult.cbCardCount[i]+cbDistillCount;
			}
		}

		//���ƽ��
		for( BYTE i = 0; i < tmpDoubleWing.cbSearchCount; i++ )
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpDoubleWing.cbResultCard[i],
				sizeof(BYTE)*tmpDoubleWing.cbCardCount[i] );
			pSearchCardResult->cbCardCount[cbResultCount] = tmpDoubleWing.cbCardCount[i];
		}
		for( BYTE i = 0; i < tmpSingleWing.cbSearchCount; i++ )
		{
			BYTE cbResultCount = pSearchCardResult->cbSearchCount++;

			CopyMemory( pSearchCardResult->cbResultCard[cbResultCount],tmpSingleWing.cbResultCard[i],
				sizeof(BYTE)*tmpSingleWing.cbCardCount[i] );
			pSearchCardResult->cbCardCount[cbResultCount] = tmpSingleWing.cbCardCount[i];
		}
	}

	return pSearchCardResult==NULL?0:pSearchCardResult->cbSearchCount;
}

//////////////////////////////////////////////////////////////////////////////////
