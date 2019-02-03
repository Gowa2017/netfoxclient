#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//�˿�����
BYTE CGameLogic::m_cbCardListData[52]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//���� A - K
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
}

//��������
CGameLogic::~CGameLogic()
{
}

//��ȡ����
BYTE CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	ASSERT(cbCardCount==MAX_COUNT);

	if (cbCardCount==MAX_COUNT)
	{
		//��������
		bool cbSameColor=true,bLineCard=true;
		BYTE cbFirstColor=GetCardColor(cbCardData[0]);
		BYTE cbFirstValue=GetCardLogicValue(cbCardData[0]);

		//���η���
		for (BYTE i=1;i<cbCardCount;i++)
		{
			//���ݷ���
			if (GetCardColor(cbCardData[i])!=cbFirstColor) cbSameColor=false;
			if (cbFirstValue!=(GetCardLogicValue(cbCardData[i])+i)) bLineCard=false;

			//�����ж�
			if ((cbSameColor==false)&&(bLineCard==false)) break;
		}

		//����A32
		if(!bLineCard)
		{
			bool bOne=false,bTwo=false,bThree=false;
			for(BYTE i=0;i<MAX_COUNT;i++)
			{
				if(GetCardValue(cbCardData[i])==1)		bOne=true;
				else if(GetCardValue(cbCardData[i])==2)	bTwo=true;
				else if(GetCardValue(cbCardData[i])==3)	bThree=true;
			}
			if(bOne && bTwo && bThree)bLineCard=true;
		}

		//˳������
		if ((cbSameColor)&&(bLineCard)) return CT_SHUN_JIN;

		//˳������
		if ((!cbSameColor)&&(bLineCard)) return CT_SHUN_ZI;

		//������
		if((cbSameColor)&&(!bLineCard)) return CT_JIN_HUA;

		//���η���
		bool bDouble=false,bPanther=true;

		//���Ʒ���
		for (BYTE i=0;i<cbCardCount-1;i++)
		{
			for (BYTE j=i+1;j<cbCardCount;j++)
			{
				if (GetCardLogicValue(cbCardData[i])==GetCardLogicValue(cbCardData[j])) 
				{
					bDouble=true;
					break;
				}
			}
			if(bDouble)break;
		}

		//����(����)����
		for (BYTE i=1;i<cbCardCount;i++)
		{
			if (bPanther && cbFirstValue!=GetCardLogicValue(cbCardData[i])) bPanther=false;
		}

		//���Ӻͱ����ж�
		if (bDouble==true) return (bPanther)?CT_BAO_ZI:CT_DOUBLE;

		//����235
		bool bTwo=false,bThree=false,bFive=false;
		for (BYTE i=0;i<cbCardCount;i++)
		{
			if(GetCardValue(cbCardData[i])==2)	bTwo=true;
			else if(GetCardValue(cbCardData[i])==3)bThree=true;
			else if(GetCardValue(cbCardData[i])==5)bFive=true;			
		}	
		if (bTwo && bThree && bFive) return CT_SPECIAL;
	}

	return CT_SINGLE;
}

//�����˿�
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//ת����ֵ
	BYTE cbLogicValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbLogicValue[i]=GetCardLogicValue(cbCardData[i]);	

	//�������
	bool bSorted=true;
	BYTE cbTempData,bLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((cbLogicValue[i]<cbLogicValue[i+1])||
				((cbLogicValue[i]==cbLogicValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//����λ��
				cbTempData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbTempData;
				cbTempData=cbLogicValue[i];
				cbLogicValue[i]=cbLogicValue[i+1];
				cbLogicValue[i+1]=cbTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

////�����˿�
//void CGameLogic::RandCardList(BYTE cbAllCardBuffer[],BYTE cbCardBuffer[], BYTE cbBufferCount)
//{
//	//CopyMemory(cbCardBuffer,m_cbCardListData,cbBufferCount);
//
//	//����׼��
//	BYTE cbCardData[CountArray(m_cbCardListData)];
//	CopyMemory(cbCardData,m_cbCardListData,sizeof(m_cbCardListData));
//
//	//�����˿�
//	BYTE bRandCount=0,bPosition=0;
//	do
//	{
//		bPosition=rand()%(CountArray(m_cbCardListData)-bRandCount);
//		if (bRandCount < GAME_PLAYER*MAX_COUNT)
//		{
//			cbCardBuffer[bRandCount]=cbCardData[bPosition];
//		}	
//		
//		cbAllCardBuffer[bRandCount]=cbCardData[bPosition];
//		bRandCount++;
//		cbCardData[bPosition]=cbCardData[CountArray(m_cbCardListData)-bRandCount];
//		
//	} while (bRandCount<cbBufferCount);
//
//	return;
//}

//�����˿�
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//CopyMemory(cbCardBuffer,m_cbCardListData,cbBufferCount);
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardListData)];
	CopyMemory(cbCardData,m_cbCardListData,sizeof(m_cbCardListData));

	//�����˿�
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(CountArray(m_cbCardListData)-bRandCount);
		cbCardBuffer[bRandCount++]=cbCardData[bPosition];
		cbCardData[bPosition]=cbCardData[CountArray(m_cbCardListData)-bRandCount];
	}
	while(bRandCount<cbBufferCount);

	return;
}

//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE bCardColor=GetCardColor(cbCardData);
	BYTE bCardValue=GetCardValue(cbCardData);

	//ת����ֵ
	return (bCardValue==1)?(bCardValue+13):bCardValue;
}

//�Ա��˿�
BYTE CGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount)
{
	//���ñ���
	BYTE FirstData[MAX_COUNT],NextData[MAX_COUNT];
	CopyMemory(FirstData,cbFirstData,sizeof(FirstData));
	CopyMemory(NextData,cbNextData,sizeof(NextData));

	//��С����
	SortCardList(FirstData,cbCardCount);
	SortCardList(NextData,cbCardCount);

	//��ȡ����
	BYTE cbNextType=GetCardType(NextData,cbCardCount);
	BYTE cbFirstType=GetCardType(FirstData,cbCardCount);

	//�����������
	if((cbNextType+cbFirstType)==(CT_SPECIAL+CT_BAO_ZI))return (BYTE)(cbFirstType>cbNextType);

	//��ԭ��������
	if(cbNextType==CT_SPECIAL)cbNextType=CT_SINGLE;
	if(cbFirstType==CT_SPECIAL)cbFirstType=CT_SINGLE;

	//�����ж�
	if (cbFirstType!=cbNextType) return (cbFirstType>cbNextType)?1:0;

	//������
	switch(cbFirstType)
	{
	case CT_BAO_ZI:			//����
	case CT_SINGLE:			//����
	case CT_JIN_HUA:		//��
		{
			//�Ա���ֵ
			for (BYTE i=0;i<cbCardCount;i++)
			{
				BYTE cbNextValue=GetCardLogicValue(NextData[i]);
				BYTE cbFirstValue=GetCardLogicValue(FirstData[i]);
				if (cbFirstValue!=cbNextValue) return (cbFirstValue>cbNextValue)?1:0;
			}
			return DRAW;
		}
	case CT_SHUN_ZI:		//˳��
	case CT_SHUN_JIN:		//˳�� 432>A32
		{		
			BYTE cbNextValue=GetCardLogicValue(NextData[0]);
			BYTE cbFirstValue=GetCardLogicValue(FirstData[0]);

			//����A32
			if(cbNextValue==14 && GetCardLogicValue(NextData[cbCardCount-1])==2)
			{
				cbNextValue=3;
			}
			if(cbFirstValue==14 && GetCardLogicValue(FirstData[cbCardCount-1])==2)
			{
				cbFirstValue=3;
			}

			//�Ա���ֵ
			if (cbFirstValue!=cbNextValue) return (cbFirstValue>cbNextValue)?1:0;;
			return DRAW;
		}
	case CT_DOUBLE:			//����
		{
			BYTE cbNextValue=GetCardLogicValue(NextData[0]);
			BYTE cbFirstValue=GetCardLogicValue(FirstData[0]);

			//���Ҷ���/����
			BYTE bNextDouble=0,bNextSingle=0;
			BYTE bFirstDouble=0,bFirstSingle=0;
			if(cbNextValue==GetCardLogicValue(NextData[1]))
			{
				bNextDouble=cbNextValue;
				bNextSingle=GetCardLogicValue(NextData[cbCardCount-1]);
			}
			else
			{
				bNextDouble=GetCardLogicValue(NextData[cbCardCount-1]);
				bNextSingle=cbNextValue;
			}
			if(cbFirstValue==GetCardLogicValue(FirstData[1]))
			{
				bFirstDouble=cbFirstValue;
				bFirstSingle=GetCardLogicValue(FirstData[cbCardCount-1]);
			}
			else 
			{
				bFirstDouble=GetCardLogicValue(FirstData[cbCardCount-1]);
				bFirstSingle=cbFirstValue;
			}

			if (bNextDouble!=bFirstDouble)return (bFirstDouble>bNextDouble)?1:0;
			if (bNextSingle!=bFirstSingle)return (bFirstSingle>bNextSingle)?1:0;
			return DRAW;
		}
	}

	return DRAW;
}
//�����˿�
void CGameLogic::RandCardList2(BYTE cbCardBuffer[], BYTE cbBufferCount,BYTE cbAllCardData[52],BYTE cbCardCount)
{
	//����׼��
	BYTE cbCardData[52];
	CopyMemory(cbCardData,cbAllCardData,sizeof(cbCardData));

	//�����˿�
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(CountArray(cbCardData)-cbRandCount - (52-cbCardCount));
		if (cbCardData[cbPosition] == 0x00)
		{
			int nn  = 0;
		}
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[CountArray(cbCardData)-cbRandCount- (52-cbCardCount)];
	}
	while(cbRandCount<cbBufferCount);

	return;
}
bool CGameLogic::RemoveGoodCardData(BYTE cbGoodcardData[3], BYTE cbGoodCardCount, BYTE cbCardData[52], BYTE cbCardCount) 
{
	//��������
	ASSERT(cbGoodCardCount<=cbCardCount);
	if(cbGoodCardCount>cbCardCount)
		return false ;

	//�������
	BYTE cbDeleteCount=0,cbTempCardData[52],cbTempCardData2[52] = {0};
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i=0;i<cbGoodCardCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbGoodcardData[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	ASSERT(cbDeleteCount==cbGoodCardCount) ;
	if (cbDeleteCount!=cbGoodCardCount) return false;

	//�����˿�
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) 
			cbTempCardData2[cbCardPos++]=cbTempCardData[i];
	}
	CopyMemory(cbCardData,cbTempCardData2,52);
	return true;
}
//////////////////////////////////////////////////////////////////////////
