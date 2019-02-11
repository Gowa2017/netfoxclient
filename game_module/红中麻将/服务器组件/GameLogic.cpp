#include "StdAfx.h"
#include "GameLogic.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
bool		CChiHuRight::m_bInit = false;
DWORD		CChiHuRight::m_dwRightMask[MAX_RIGHT_COUNT];

//���캯��
CChiHuRight::CChiHuRight()
{
	ZeroMemory(m_dwRight,sizeof(m_dwRight));

	if(!m_bInit)
	{
		m_bInit = true;
		for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
		{
			if(0 == i)
				m_dwRightMask[i] = 0;
			else
				m_dwRightMask[i] = (DWORD(pow(2,i-1)))<<28;
		}
	}
}

//��ֵ������
CChiHuRight & CChiHuRight::operator = (DWORD dwRight)
{
	DWORD dwOtherRight = 0;
	//��֤Ȩλ
	if(!IsValidRight(dwRight))
	{
		//��֤ȡ��Ȩλ
		ASSERT(IsValidRight(~dwRight));
		if(!IsValidRight(~dwRight)) return *this;
		dwRight = ~dwRight;
		dwOtherRight = MASK_CHI_HU_RIGHT;
	}

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000))
			m_dwRight[i] = dwRight&MASK_CHI_HU_RIGHT;
		else m_dwRight[i] = dwOtherRight;
	}

	return *this;
}

//�����
CChiHuRight & CChiHuRight::operator &= (DWORD dwRight)
{
	bool bNavigate = false;
	//��֤Ȩλ
	if(!IsValidRight(dwRight))
	{
		//��֤ȡ��Ȩλ
		ASSERT(IsValidRight(~dwRight));
		if(!IsValidRight(~dwRight)) return *this;
		//����Ȩλ
		DWORD dwHeadRight = (~dwRight)&0xF0000000;
		DWORD dwTailRight = dwRight&MASK_CHI_HU_RIGHT;
		dwRight = dwHeadRight|dwTailRight;
		bNavigate = true;
	}

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000))
		{
			m_dwRight[i] &= (dwRight&MASK_CHI_HU_RIGHT);
		}
		else if(!bNavigate)
			m_dwRight[i] = 0;
	}

	return *this;
}

//�����
CChiHuRight & CChiHuRight::operator |= (DWORD dwRight)
{
	//��֤Ȩλ
	if(!IsValidRight(dwRight)) return *this;

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000))
		{
			m_dwRight[i] |= (dwRight&MASK_CHI_HU_RIGHT);
			break;
		}
	}

	return *this;
}

//��
CChiHuRight CChiHuRight::operator & (DWORD dwRight)
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//��
CChiHuRight CChiHuRight::operator & (DWORD dwRight) const
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//��
CChiHuRight CChiHuRight::operator | (DWORD dwRight)
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//��
CChiHuRight CChiHuRight::operator | (DWORD dwRight) const
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//���
bool CChiHuRight::operator == (DWORD dwRight) const
{
	CChiHuRight chr;
	chr = dwRight;
	return (*this)==chr;
}

//���
bool CChiHuRight::operator == (const CChiHuRight chr) const
{
	for(WORD i = 0; i < CountArray(m_dwRight); i++)
	{
		if(m_dwRight[i] != chr.m_dwRight[i]) return false;
	}
	return true;
}

//�����
bool CChiHuRight::operator != (DWORD dwRight) const
{
	CChiHuRight chr;
	chr = dwRight;
	return (*this)!=chr;
}

//�����
bool CChiHuRight::operator != (const CChiHuRight chr) const
{
	return !((*this)==chr);
}

//�Ƿ�ȨλΪ��
bool CChiHuRight::IsEmpty()
{
	for(BYTE i = 0; i < CountArray(m_dwRight); i++)
		if(m_dwRight[i]) return false;
	return true;
}

//����ȨλΪ��
void CChiHuRight::SetEmpty()
{
	ZeroMemory(m_dwRight,sizeof(m_dwRight));
	return;
}

//��ȡȨλ��ֵ
BYTE CChiHuRight::GetRightData(DWORD dwRight[], BYTE cbMaxCount)
{
	ASSERT(cbMaxCount >= CountArray(m_dwRight));
	if(cbMaxCount < CountArray(m_dwRight)) return 0;

	CopyMemory(dwRight,m_dwRight,sizeof(DWORD)*CountArray(m_dwRight));
	return CountArray(m_dwRight);
}

//����Ȩλ��ֵ
bool CChiHuRight::SetRightData(const DWORD dwRight[], BYTE cbRightCount)
{
	ASSERT(cbRightCount <= CountArray(m_dwRight));
	if(cbRightCount > CountArray(m_dwRight)) return false;

	ZeroMemory(m_dwRight,sizeof(m_dwRight));
	CopyMemory(m_dwRight,dwRight,sizeof(DWORD)*cbRightCount);

	return true;
}

//����λ�Ƿ���ȷ
bool CChiHuRight::IsValidRight(DWORD dwRight)
{
	DWORD dwRightHead = dwRight & 0xF0000000;
	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
		if(m_dwRightMask[i] == dwRightHead) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////

//��̬����

//�齫����
const BYTE CGameLogic::m_cbCardDataArray[MAX_REPERTORY]=
{
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
		0x35,0x35,0x35,0x35																	//����
};

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
	m_cbMagicIndex = MAX_INDEX;
}

//��������
CGameLogic::~CGameLogic()
{
}

//�����˿�
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE cbCardDataTemp[CountArray(m_cbCardDataArray)];
	CopyMemory(cbCardDataTemp,m_cbCardDataArray,sizeof(m_cbCardDataArray));

	//�����˿�
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

bool rateHit(int rate)
{
	if (rate > 100)
	{
		rate = 100;
	}
	if (rate < 0)
	{
		rate = 0;
	}
	int rd = rand() % 101;
	if (rd >(100 - rate))
	{
		return true;
	}
	return false;
}

/*
�ӿ����repoCards�������Ʒŵ�outCards
������ƴӿ���Ƴ�
*/
int CGameLogic::GetCards(BYTE outCards[], BYTE outLen, BYTE repoCardsPrime[], BYTE repoLen, BYTE cCards[], BYTE cLen)
{
	BYTE repoCards[CountArray(m_cbCardDataArray)];
	CopyMemory(repoCards, repoCardsPrime, repoLen);
	if (cLen > outLen || outLen > repoLen)
	{
		return 0;
	}

	//repoCards��ҵ����е�cCards,�ƶ�����ʼλ��
	for (int i = 0; i < cLen; i++)
	{
		int j = i;
		for (; j < repoLen; j++)
		{
			if (repoCards[j] == cCards[i])
			{
				//���ҵ����ƽ����ŵ��ʼ�ĵط�
				BYTE tmp = repoCards[j];
				repoCards[j] = repoCards[i];
				repoCards[i] = tmp;
				break;
			}
		}
		//��ָ������û�ҵ�
		if (j == repoLen)
		{
			return 0;
		}
	}


	//��cLen<outLen, �������������
	BYTE bRandCount = cLen, bPosition = 0;
	while (bRandCount<outLen)
	{
		bPosition = rand() % (repoLen - bRandCount);
		BYTE tmp = repoCards[bRandCount];
		repoCards[bRandCount] = repoCards[bRandCount + bPosition];
		repoCards[bRandCount + bPosition] = tmp;
		bRandCount++;
	}
	//���Ƶ�outCards
	CopyMemory(outCards, repoCards, outLen);

	//ԭ������Ƴ�������
	if (repoLen > outLen)
	{
		CopyMemory(repoCardsPrime, &repoCards[outLen], (repoLen - outLen));
	}

	ZeroMemory(&repoCardsPrime[repoLen - outLen], outLen);

	return outLen;
}

//�����˿�
VOID CGameLogic::RandCardList(BYTE cbCardData[], BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	CopyMemory(cbCardDataTemp,cbCardData,sizeof(BYTE) * cbBufferCount);

	//�����˿�
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//ɾ���˿�
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		//Ч���˿�
		ASSERT(IsValidCard(cbRemoveCard[i]));
		ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard[i])]>0);

		//ɾ���˿�
		BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard[i]);
		if (cbCardIndex[cbRemoveIndex]==0)
		{
			//�������
			ASSERT(FALSE);

			//��ԭɾ��
			for (BYTE j=0;j<i;j++) 
			{
				ASSERT(IsValidCard(cbRemoveCard[j]));
				cbCardIndex[SwitchToCardIndex(cbRemoveCard[j])]++;
			}

			return false;
		}
		else 
		{
			//ɾ���˿�
			--cbCardIndex[cbRemoveIndex];
		}
	}

	return true;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard)
{
	//Ч���˿�
	ASSERT(IsValidCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)] > 0);

	//ɾ���˿�
	BYTE cbRemoveIndex = SwitchToCardIndex(cbRemoveCard);
	if (cbCardIndex[cbRemoveIndex] > 0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//ʧ��Ч��
	ASSERT(FALSE);

	return false;
}

//�����ж�
bool CGameLogic::IsMagicCard(BYTE cbCardData)
{
	if(m_cbMagicIndex != MAX_INDEX)
		return SwitchToCardIndex(cbCardData) == m_cbMagicIndex;
	return false;
}

//�����ж�
bool CGameLogic::IsHuaCard(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));

	return cbCardData >= 0x38;
}

//�����ж�
BYTE CGameLogic::IsHuaCard(BYTE cbCardIndex[MAX_INDEX])
{
	BYTE cbHuaCardCount = 0;
	for(int i = MAX_INDEX - MAX_HUA_INDEX; i < MAX_INDEX; i++)
	{
		if(cbCardIndex[i] > 0)
		{
			cbHuaCardCount += cbCardIndex[i];
		}
	}

	return cbHuaCardCount;
}

//����,������ֵ����
bool CGameLogic::SortCardList(BYTE cbCardData[MAX_COUNT], BYTE cbCardCount)
{
	//��Ŀ����
	if (cbCardCount==0||cbCardCount>MAX_COUNT) return false;

	//�������
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	BYTE cbCard1, cbCard2;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			cbCard1 = cbCardData[i];
			cbCard2 = cbCardData[i+1];
			//��������д����ƣ������������ת��
			if(INDEX_REPLACE_CARD != MAX_INDEX  && m_cbMagicIndex != INDEX_REPLACE_CARD)
			{
				if(SwitchToCardIndex(cbCard1) == INDEX_REPLACE_CARD) cbCard1 = SwitchToCardData(m_cbMagicIndex);
				if(SwitchToCardIndex(cbCard2) == INDEX_REPLACE_CARD) cbCard2 = SwitchToCardData(m_cbMagicIndex);
			}
			if (cbCard1>cbCard2)
			{
				//���ñ�־
				bSorted=false;

				//�˿�����
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	return true;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//��������
	ASSERT(cbCardCount<=MAX_COUNT);
	ASSERT(cbRemoveCount<=cbCardCount);

	//�������
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData))
		return false;
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

	//�ɹ��ж�
	if (cbDeleteCount!=cbRemoveCount) 
	{
		ASSERT(FALSE);
		return false;
	}

	//�����˿�
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) 
			cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//�����ȼ�
BYTE CGameLogic::GetUserActionRank(BYTE cbUserAction)
{
	//���Ƶȼ�
	if (cbUserAction&WIK_CHI_HU) { return 4; }

	//���Ƶȼ�
	if (cbUserAction&WIK_GANG) { return 3; }

	//���Ƶȼ�
	if (cbUserAction&WIK_PENG) { return 2; }

	//���Ƶȼ�
	if (cbUserAction&(WIK_RIGHT|WIK_CENTER|WIK_LEFT)) { return 1; }

	return 0;
}

//���Ƶȼ�
WORD CGameLogic::GetChiHuActionRank(const CChiHuRight & ChiHuRight)
{
	return 1;
}

//���Ʊ���
WORD CGameLogic::GetChiHuTime(const CChiHuRight & ChiHuRight)
{
	WORD wFanShu = 1;//ƽ��һ��

	if(!(ChiHuRight&CHR_TIAN_HU).IsEmpty())
		wFanShu=16;
	else if(!(ChiHuRight&CHR_QI_SHOU_LISTEN).IsEmpty())//������
		wFanShu=8;
	else if(!(ChiHuRight&CHR_PENG_PENG).IsEmpty())//������
		wFanShu=3;
	else if(!(ChiHuRight&CHR_DAN_DIAN_QI_DUI).IsEmpty())//������߶�
		wFanShu=6;
	else if(!(ChiHuRight&CHR_MA_QI_DUI).IsEmpty())//���߶�
		wFanShu=5;
	else if(!(ChiHuRight&CHR_MA_QI_WANG).IsEmpty())//������
		wFanShu=10;
	else if(!(ChiHuRight&CHR_MA_QI_WZW).IsEmpty())//����������
		wFanShu=20;
	else if(!(ChiHuRight&CHR_SHI_SAN_LAN).IsEmpty())//ʮ����
		wFanShu=2;
	else if(!(ChiHuRight&CHR_QX_SHI_SAN_LAN).IsEmpty())//����ʮ����
		wFanShu=4;

	if(!(ChiHuRight&CHR_GANG_SHANG_HUA).IsEmpty())//���Ͽ���
		wFanShu *= 2;
	else if(!(ChiHuRight&CHR_QIANG_GANG_HU).IsEmpty())//����
	{
		wFanShu = 2*wFanShu+1;
	}
	
	if(!(ChiHuRight&CHR_ZI_MO).IsEmpty())
		wFanShu *= 2;


	return wFanShu;
}

//�Զ�����
BYTE CGameLogic::AutomatismOutCard(const BYTE cbCardIndex[MAX_INDEX], const BYTE cbEnjoinOutCard[MAX_COUNT], BYTE cbEnjoinOutCardCount)
{
	// �ȴ����
	if(m_cbMagicIndex != MAX_INDEX)
	{
		if(cbCardIndex[m_cbMagicIndex] > 0)
		{
			return SwitchToCardData(m_cbMagicIndex);
		}
	}

	//��������ƣ����ƴ��Լ���ģ���Ŀһ���Ͱ����������з��׵�˳��
	BYTE cbCardData = 0;
	BYTE cbOutCardIndex  = MAX_INDEX;
	BYTE cbOutCardIndexCount = 0;
	for(int i = MAX_INDEX - 7; i < MAX_INDEX - 1; i++)
	{
		if(cbCardIndex[i] > cbOutCardIndexCount)
		{
			cbOutCardIndexCount = cbCardIndex[i];
			cbOutCardIndex = i;
		}
	}

	if(cbOutCardIndex != MAX_INDEX)
	{
		cbCardData = SwitchToCardData(cbOutCardIndex);
		bool bEnjoinCard = false;
		for(int k = 0; k < cbEnjoinOutCardCount; k++)
		{
			if(cbCardData == cbEnjoinOutCard[k])
			{
				bEnjoinCard = true;
			}
		}
		if(!bEnjoinCard)
		{
			return cbCardData;
		}		
	}

	//û�����ƾʹ���ţ�1��9��˳��Ϊ��Ͳ����2��8������3��7������4��6������5
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			cbOutCardIndex  = MAX_INDEX;
			if(cbCardIndex[j * 9 + i] > 0)
			{
				cbOutCardIndex = j * 9 + i;
			}
			else if(cbCardIndex[j * 9 + (9 - i - 1)] > 0)
			{
				cbOutCardIndex = j * 9 + (9 - i - 1);
			}

			if(cbOutCardIndex != MAX_INDEX)
			{
				BYTE cbCardDataTemp = SwitchToCardData(cbOutCardIndex);
				bool bEnjoinCard = false;
				for(int k = 0; k < cbEnjoinOutCardCount; k++)
				{
					if(cbCardDataTemp == cbEnjoinOutCard[k])
					{
						bEnjoinCard = true;
					}
				}
				if(!bEnjoinCard)
				{
					return cbCardDataTemp;
				}
				else
				{
					if(cbCardData == 0)
					{
						cbCardData = cbCardDataTemp;
					}
				}
			}
		}
	}
	return cbCardData;
}

//�����ж�
BYTE CGameLogic::EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//����Ч��
	ASSERT(IsValidCard(cbCurrentCard));

	BYTE cbCurrentIndex=SwitchToCardIndex(cbCurrentCard);

	//�����ж�
	if (cbCurrentIndex == m_cbMagicIndex) return WIK_NULL;
	if (cbCurrentIndex == INDEX_REPLACE_CARD && m_cbMagicIndex>=27) return WIK_NULL;
	if (cbCurrentCard >= 0x31 && cbCurrentIndex != INDEX_REPLACE_CARD) return WIK_NULL;

	//��������
	BYTE cbExcursion[3]={0,1,2};
	BYTE cbItemKind[3]={WIK_LEFT,WIK_CENTER,WIK_RIGHT};

	//��ַ���
	BYTE cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex,cbCardIndex,sizeof(cbMagicCardIndex));

	//����в���
	BYTE cbMagicCardCount = 0;
	if(m_cbMagicIndex != MAX_INDEX)
	{
		cbMagicCardCount = cbCardIndex[m_cbMagicIndex];
		//��������д����ƣ������������ת��
		if(INDEX_REPLACE_CARD != MAX_INDEX)
		{
			cbMagicCardIndex[m_cbMagicIndex] = cbMagicCardIndex[INDEX_REPLACE_CARD];
			cbMagicCardIndex[INDEX_REPLACE_CARD] = cbMagicCardCount;
		}
	}

	//�����ж�
	BYTE cbEatKind=0,cbFirstIndex=0;
	if (cbCurrentIndex == INDEX_REPLACE_CARD) cbCurrentIndex = m_cbMagicIndex;
	for (BYTE i=0;i<CountArray(cbItemKind);i++)
	{
		BYTE cbValueIndex=cbCurrentIndex%9;
		if ((cbValueIndex>=cbExcursion[i])&&((cbValueIndex-cbExcursion[i])<=6))
		{
			//�����ж�
			cbFirstIndex=cbCurrentIndex-cbExcursion[i];

			//���Ʋ��ܰ����в���
			if(m_cbMagicIndex != MAX_INDEX &&
				m_cbMagicIndex >= cbFirstIndex && m_cbMagicIndex <= cbFirstIndex+2) continue;

			if ((cbCurrentIndex!=cbFirstIndex)&&(cbMagicCardIndex[cbFirstIndex]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+1))&&(cbMagicCardIndex[cbFirstIndex+1]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+2))&&(cbMagicCardIndex[cbFirstIndex+2]==0))
				continue;

			//��������
			cbEatKind|=cbItemKind[i];
		}
	}

	return cbEatKind;
}

//�����ж�
BYTE CGameLogic::EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//����Ч��
	ASSERT(IsValidCard(cbCurrentCard));

	//�����ж�
	if (IsMagicCard(cbCurrentCard) || IsHuaCard(cbCurrentCard)) 
		return WIK_NULL;

	//�����ж�
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//�����ж�
BYTE CGameLogic::EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//����Ч��
	ASSERT(IsValidCard(cbCurrentCard));

	//�����ж�
	if (IsMagicCard(cbCurrentCard) || IsHuaCard(cbCurrentCard)) 
		return WIK_NULL;

	//�����ж�
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]==3)?WIK_GANG:WIK_NULL;
}

//���Ʒ���
BYTE CGameLogic::AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult)
{
	//���ñ���
	BYTE cbActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//���ϸ���
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if(i == m_cbMagicIndex) continue;
		if (cbCardIndex[i]==4)
		{
			cbActionMask|=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);
		}
	}

	//��ϸ���
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		if (WeaveItem[i].cbWeaveKind==WIK_PENG)
		{
			if (cbCardIndex[SwitchToCardIndex(WeaveItem[i].cbCenterCard)]==1)
			{
				cbActionMask|=WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;
			}
		}
	}

	return cbActionMask;
}

BYTE CGameLogic::AnalyseGangCardEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbProvideCard, tagGangCardResult & GangCardResult)
{
	//���ñ���
	BYTE cbActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//���ϸ���
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if(i == m_cbMagicIndex) continue;
		if (cbCardIndex[i]==4)
		{
			cbActionMask|=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);
		}
	}

	//��ϸ���
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		if (WeaveItem[i].cbWeaveKind==WIK_PENG)
		{
			if (WeaveItem[i].cbCenterCard==cbProvideCard)//֮��ץ���ĵ��Ʋ��ܺ�����ɸ�
			{
				cbActionMask|=WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;
			}
		}
	}

	return cbActionMask;
}

//�Ժ�����
BYTE CGameLogic::AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight,bool b4HZHu/*=false*/)
{
	//��������
	BYTE cbChiHuKind=WIK_NULL;
	CAnalyseItemArray AnalyseItemArray;

	//���ñ���
	AnalyseItemArray.RemoveAll();
	ChiHuRight.SetEmpty();

	//�����˿�
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//cbCurrentCardһ����Ϊ0			!!!!!!!!!
	ASSERT(cbCurrentCard != 0);
	if(cbCurrentCard == 0) return WIK_NULL;

	//�����˿�
	if (cbCurrentCard!=0)
		cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

	if(b4HZHu && cbCardIndexTemp[31]==4)//�ĸ�����ֱ�Ӻ���
	{
		return WIK_CHI_HU;
	}

	if(m_cbQiXiaoDui && IsQiXiaoDui(cbCardIndexTemp,WeaveItem,cbWeaveCount))
		ChiHuRight |= CHR_DAN_DIAN_QI_DUI;//��С���ж�

	//�����˿�
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);

	//���Ʒ���
	if (AnalyseItemArray.GetCount()>0)
	{
		//���ͷ���
// 		for (int i=0;i<AnalyseItemArray.GetCount();i++)
// 		{
// 			//��������
// 			tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];
// 
//  			//������
//  			if(IsPengPeng(pAnalyseItem)) 
// 			{
// 				ChiHuRight |= CHR_PENG_PENG;
// 			}
// 
// 		}

		ChiHuRight |= CHR_PING_HU;
	}

	if(!ChiHuRight.IsEmpty())
		cbChiHuKind = WIK_CHI_HU;

	return cbChiHuKind;
}

//���Ʒ���
BYTE CGameLogic::AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount)
{
	//��������
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if((cbCardCount + 1) % 3 == 0)
	{
		for(BYTE i = 0; i < MAX_INDEX - MAX_HUA_INDEX; i++)
		{
			if(cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;

			for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_INDEX; j++)
			{
				BYTE cbCurrentCard = SwitchToCardData(j);
				if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbCurrentCard, chr))
					return WIK_LISTEN;
			}

			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_INDEX; j++)
		{
			BYTE cbCurrentCard = SwitchToCardData(j);
			if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr))
				return WIK_LISTEN;
		}
	}

	return WIK_NULL;
}

//���Ʒ���
BYTE CGameLogic::AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbOutCard[][28])
{
	//��������
	BYTE cbOutCount = 0;
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if((cbCardCount-2)%3==0)
	{
		for(BYTE i = 0; i < MAX_INDEX-MAX_HUA_INDEX; i++)
		{
			if(cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;
			
			bool bAdd=false;
			BYTE nCount=0;
			for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_INDEX; j++)
			{
				BYTE cbCurrentCard = SwitchToCardData(j);
				if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr))
				{
					if(bAdd==FALSE)
					{
						bAdd=true;
						cbOutCard[0][cbOutCount++] = SwitchToCardData(i);
					}
					cbOutCard[cbOutCount][nCount++]=SwitchToCardData(j);
				}
			}

			cbCardIndexTemp[i]++;
		}
	}

	return cbOutCount;
}

BYTE CGameLogic::AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE& cbOutCardCount,BYTE cbOutCardData[],BYTE cbHuCardCount[],BYTE cbHuCardData[][28])
{
	//��������
	BYTE cbOutCount = 0;
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if((cbCardCount-2)%3==0)
	{
		for(BYTE i = 0; i < MAX_INDEX-MAX_HUA_INDEX; i++)
		{
			if(cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;

			bool bAdd=false;
			BYTE nCount=0;
			for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_INDEX; j++)
			{
				BYTE cbCurrentCard = SwitchToCardData(j);
				if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr))
				{
					if(bAdd==FALSE)
					{
						bAdd=true;
						cbOutCardData[cbOutCount++] = SwitchToCardData(i);
					}
					cbHuCardData[cbOutCount-1][nCount++]=SwitchToCardData(j);
				}
			}
			if(bAdd)
				cbHuCardCount[cbOutCount-1]=nCount;

			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		BYTE cbCount=0;
		for( BYTE j = 0; j < MAX_INDEX; j++ )
		{
			BYTE cbCurrentCard = SwitchToCardData(j);
			if( WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr) )
			{
				cbHuCardData[0][cbCount++] = cbCurrentCard;
			}
		}
		cbHuCardCount[0]=cbCount;
	}

	cbOutCardCount = cbOutCount;
	return cbOutCount;
}

BYTE CGameLogic::GetHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbHuCardData[])
{
	//��������
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	BYTE cbCount = 0;
	ZeroMemory(cbHuCardData,sizeof(cbHuCardData));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if( (cbCardCount-2)%3 !=0 )
	{
		for( BYTE j = 0; j < MAX_INDEX; j++ )
		{
			BYTE cbCurrentCard = SwitchToCardData(j);
			if( WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr) )
			{
				cbHuCardData[cbCount++] = cbCurrentCard;
			}
		}
		if(cbCount>0)
			return cbCount;
	}

	return 0;
}
//�˿�ת��
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	ASSERT(cbCardIndex<MAX_INDEX);
	if(cbCardIndex < 27)
		return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
	else return (0x30|(cbCardIndex-27+1));
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));
	return ((cbCardData & MASK_COLOR) >> 4) * 9 + (cbCardData & MASK_VALUE) - 1;
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT])
{
	//ת���˿�
	BYTE cbPosition=0;
	//����
	if(m_cbMagicIndex != MAX_INDEX)
	{
		for(BYTE i = 0; i < cbCardIndex[m_cbMagicIndex]; i++)
			cbCardData[cbPosition++] = SwitchToCardData(m_cbMagicIndex);
	}
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if(i == m_cbMagicIndex && m_cbMagicIndex != INDEX_REPLACE_CARD) 
		{
			//��������д����ƣ�������ƴ������ԭ����λ��
			if(INDEX_REPLACE_CARD != MAX_INDEX)
			{
				for(BYTE j = 0; j < cbCardIndex[INDEX_REPLACE_CARD]; j++)
					cbCardData[cbPosition++] = SwitchToCardData(INDEX_REPLACE_CARD);
			}
			continue;
		}
		if(i == INDEX_REPLACE_CARD) continue;
		if (cbCardIndex[i]!=0)
		{
			for (BYTE j=0;j<cbCardIndex[i];j++)
			{
				ASSERT(cbPosition<MAX_COUNT);
				cbCardData[cbPosition++]=SwitchToCardData(i);
			}
		}
	}

	return cbPosition;
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX])
{
	//���ñ���
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//ת���˿�
	for (BYTE i=0;i<cbCardCount;i++)
	{
		ASSERT(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}


//��Ч�ж�
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	BYTE cbValue = (cbCardData & MASK_VALUE);
	BYTE cbColor = (cbCardData & MASK_COLOR) >> 4;
	return (((cbValue >= 1) && (cbValue <= 9) && (cbColor <= 2)) || ((cbValue >= 1) && (cbValue <= (7 + MAX_HUA_INDEX)) && (cbColor == 3)));
}

//�˿���Ŀ
BYTE CGameLogic::GetCardCount(const BYTE cbCardIndex[MAX_INDEX])
{
	//��Ŀͳ��
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
		cbCardCount+=cbCardIndex[i];

	return cbCardCount;
}

//��ȡ���
BYTE CGameLogic::GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//����˿�
	switch (cbWeaveKind)
	{
	case WIK_LEFT:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+2;

			return 3;
		}
	case WIK_RIGHT:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard-2;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_CENTER:	//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard-1;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard+1;

			return 3;
		}
	case WIK_PENG:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_GANG:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;
			cbCardBuffer[3]=cbCenterCard;

			return 4;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return 0;
}

bool CGameLogic::AddKindItem(tagKindItem &TempKindItem, tagKindItem KindItem[], BYTE &cbKindItemCount, bool &bMagicThree)
{
	TempKindItem.cbMagicCount =  
		(m_cbMagicIndex == TempKindItem.cbValidIndex[0] ? 1 : 0) + 
		(m_cbMagicIndex == TempKindItem.cbValidIndex[1] ? 1 : 0) +
		(m_cbMagicIndex == TempKindItem.cbValidIndex[2] ? 1 : 0) ;

	if(TempKindItem.cbMagicCount>=3) 
	{
		if(!bMagicThree)
		{
			bMagicThree = true;
			CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
			return true;
		}
		return false;
	}
	else if(TempKindItem.cbMagicCount == 2)
	{
		BYTE cbNoMagicIndex = 0;
		BYTE cbNoTempMagicIndex = 0;
		for(int i = 0; i < 3; i++)
		{
			if(TempKindItem.cbValidIndex[i] != m_cbMagicIndex)
			{
				cbNoTempMagicIndex = TempKindItem.cbValidIndex[i];
				break;
			}
		}
		bool bFind = false;
		for(int j = 0; j < cbKindItemCount; j++)
		{
			for(int i = 0; i < 3; i++)
			{
				if(KindItem[j].cbValidIndex[i] != m_cbMagicIndex)
				{
					cbNoMagicIndex = KindItem[j].cbValidIndex[i];
					break;
				}
			}
			if(cbNoMagicIndex == cbNoTempMagicIndex && cbNoMagicIndex != 0)
			{
				bFind = true;
			}
		}

		if(!bFind)
		{
			CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
			return true;
		}
		return false;
	}
	else if(TempKindItem.cbMagicCount == 1)
	{
		BYTE cbTempCardIndex[2] = {0};
		BYTE cbCardIndex[2] = {0};
		BYTE cbCardCount = 0;
		for(int i = 0; i < 3; i++)
		{
			if(TempKindItem.cbValidIndex[i] != m_cbMagicIndex)
			{
				cbTempCardIndex[cbCardCount++] = TempKindItem.cbValidIndex[i];
			}
		}
		ASSERT(cbCardCount == 2);

		for(int j = 0; j < cbKindItemCount; j++)
		{
			if(1 == KindItem[j].cbMagicCount)
			{
				cbCardCount = 0;
				for(int i = 0; i < 3; i++)
				{
					if(KindItem[j].cbValidIndex[i] != m_cbMagicIndex)
					{
						cbCardIndex[cbCardCount++] = KindItem[j].cbValidIndex[i];
					}
				}
				ASSERT(cbCardCount == 2);

				if(cbTempCardIndex[0] == cbCardIndex[0] && cbTempCardIndex[1] == cbCardIndex[1])
				{
					return false;
				}
			}
		}


		CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
		return true;
	}
	else
	{
		CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
		return true;
	}
}



//�����˿�
bool CGameLogic::AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray)
{
	//������Ŀ
	BYTE cbCardCount=GetCardCount(cbCardIndex);

	//Ч����Ŀ
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0))
		return false;

	//��������
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[27*2+28+16];
	ZeroMemory(KindItem,sizeof(KindItem));
	tagKindItem TempKindItem;
	ZeroMemory(&TempKindItem,sizeof(TempKindItem));
	bool bMagicThree=false;

	//�����ж�
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==MAX_WEAVE);

	//�����ж�
	if (cbLessKindItem==0)
	{
		//Ч�����
		ASSERT((cbCardCount==2)&&(cbWeaveCount==MAX_WEAVE));

		//�����ж�
		for (BYTE i=0;i<MAX_INDEX;i++)
		{
			if (cbCardIndex[i]==2 || (m_cbMagicIndex != MAX_INDEX && i != m_cbMagicIndex && cbCardIndex[m_cbMagicIndex]+cbCardIndex[i]==2))
			{
				//��������
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

				//���ý��
				for (BYTE j=0;j<cbWeaveCount;j++)
				{
					AnalyseItem.cbWeaveKind[j]=WeaveItem[j].cbWeaveKind;
					AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
					CopyMemory(AnalyseItem.cbCardData[j],WeaveItem[j].cbCardData,sizeof(WeaveItem[j].cbCardData));
				}
				if(cbCardIndex[i] < 2 || i == m_cbMagicIndex)
					AnalyseItem.bMagicEye = true;
				else AnalyseItem.bMagicEye = false;
				AnalyseItem.cbCardEye=cbCardIndex[i]==0?SwitchToCardData(cbCardIndex[m_cbMagicIndex]):SwitchToCardData(i);

				//������
				AnalyseItemArray.Add(AnalyseItem);

				return true;
			}
		}

		return false;
	}

	//��ַ���
	BYTE cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex,cbCardIndex,sizeof(cbMagicCardIndex));

	//����в���
	BYTE cbMagicCardCount = 0;
	BYTE cbTempMagicCount = 0;

	if(m_cbMagicIndex != MAX_INDEX)
	{
		cbMagicCardCount = cbCardIndex[m_cbMagicIndex];
		//��������д����ƣ������������ת��
		if(INDEX_REPLACE_CARD != MAX_INDEX)
		{
			cbMagicCardIndex[m_cbMagicIndex] = cbMagicCardIndex[INDEX_REPLACE_CARD];
			cbMagicCardIndex[INDEX_REPLACE_CARD] = cbMagicCardCount;
		}
	}

	if (cbCardCount>=3)
	{
		for (BYTE i=0;i<MAX_INDEX-MAX_HUA_INDEX;i++)
		{
			//ͬ���ж�
			//����ǲ���,���Ҳ�����С��3,�򲻽������
			if(cbMagicCardIndex[i] >= 3 || (cbMagicCardIndex[i]+cbMagicCardCount >= 3 &&
				((INDEX_REPLACE_CARD!=MAX_INDEX && i != INDEX_REPLACE_CARD) || (INDEX_REPLACE_CARD==MAX_INDEX && i != m_cbMagicIndex)))
				)
			{
				int nTempIndex = cbMagicCardIndex[i];
				do
				{
					ASSERT(cbKindItemCount < CountArray(KindItem));
					BYTE cbIndex = i;
					BYTE cbCenterCard = SwitchToCardData(i);
					//����ǲ����Ҳ����д�����,�򻻳ɴ�����
					if(i == m_cbMagicIndex && INDEX_REPLACE_CARD != MAX_INDEX)
					{
						cbIndex = INDEX_REPLACE_CARD;
						cbCenterCard = SwitchToCardData(INDEX_REPLACE_CARD);
					}
					TempKindItem.cbWeaveKind=WIK_PENG;
					TempKindItem.cbCenterCard=cbCenterCard;
					TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex;
					TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex;
					TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex;
					AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);


					//��ǰ������δ�������� �Ҳ��������Ϊ0 
					if(nTempIndex>=3 && cbMagicCardCount >0)
					{
						--nTempIndex;
						//1��������֮���
						TempKindItem.cbWeaveKind=WIK_PENG;
						TempKindItem.cbCenterCard=cbCenterCard;
						TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex;
						TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex;
						TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex;
						AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);

						//����������֮���
						if(cbMagicCardCount>1)
						{
							TempKindItem.cbWeaveKind=WIK_PENG;
							TempKindItem.cbCenterCard=cbCenterCard;
							TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex;
							TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex;
							TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex;
							AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);
						}

						++nTempIndex;
					}

					//����ǲ���,���˳�
					if(i == INDEX_REPLACE_CARD || ((i == m_cbMagicIndex) && INDEX_REPLACE_CARD == MAX_INDEX))
						break;

					nTempIndex -= 3;
					//����պô���ȫ�������˳�
					if(nTempIndex == 0) break;

				}while(nTempIndex+cbMagicCardCount >= 3);
			}

			//�����ж�
			if ((i<(MAX_INDEX-MAX_HUA_INDEX-9))&&((i%9)<7))
			{
				//ֻҪ������������3��˳���������������ڵ���3,��������
				if(cbMagicCardCount+cbMagicCardIndex[i]+cbMagicCardIndex[i+1]+cbMagicCardIndex[i+2] >= 3)
				{
					BYTE cbIndex[3] = { cbMagicCardIndex[i],cbMagicCardIndex[i+1],cbMagicCardIndex[i+2] };

					if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) continue;

					int nMagicCountTemp;
					nMagicCountTemp = cbMagicCardCount;

					BYTE cbValidIndex[3];
					while(nMagicCountTemp+cbIndex[0]+cbIndex[1]+cbIndex[2] >= 3)
					{
						for(BYTE j = 0; j < CountArray(cbIndex); j++)
						{
							if(cbIndex[j] > 0) 
							{
								cbIndex[j]--;
								cbValidIndex[j] = ((i+j==m_cbMagicIndex) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j;
							}
							else 
							{
								nMagicCountTemp--;
								cbValidIndex[j] = m_cbMagicIndex;														
							}
						}
						if(nMagicCountTemp >= 0)
						{
							ASSERT(cbKindItemCount < CountArray(KindItem));
							TempKindItem.cbWeaveKind=WIK_LEFT;
							TempKindItem.cbCenterCard=SwitchToCardData(i);
							CopyMemory(TempKindItem.cbValidIndex,cbValidIndex,sizeof(cbValidIndex));
							AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);
						}
						else break;
					}
				}
			}
		}
	}

	//��Ϸ���
	if (cbKindItemCount>=cbLessKindItem)
	{
		ASSERT(27*2+28+16 >= cbKindItemCount);
		//��������
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//��������
		BYTE cbIndex[MAX_WEAVE];
		for(BYTE i = 0; i < CountArray(cbIndex); i++)
			cbIndex[i] = i;

		tagKindItem * pKindItem[MAX_WEAVE];
		ZeroMemory(&pKindItem,sizeof(pKindItem));

		tagKindItem KindItemTemp[CountArray(KindItem)];

		//��ʼ���
		do
		{
			//����ĸ�����еĻ��ƴ������ϵĻ��Ƹ�������������
			cbTempMagicCount = 0;
			for(int i=0;i<cbLessKindItem;i++) cbTempMagicCount +=  KindItem[cbIndex[i]].cbMagicCount; 
			if(cbTempMagicCount <= cbMagicCardCount)
			{
				//���ñ���
				CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
				CopyMemory(KindItemTemp,KindItem,sizeof(KindItem));
	
				for (BYTE i=0;i<cbLessKindItem;i++)
					pKindItem[i]=&KindItemTemp[cbIndex[i]];


				//�����ж�
				bool bEnoughCard=true;

				for (BYTE i=0;i<cbLessKindItem*3;i++)
				{
					//�����ж�
					BYTE cbCardIndex=pKindItem[i/3]->cbValidIndex[i%3]; 
					if (cbCardIndexTemp[cbCardIndex]==0)
					{
						if(m_cbMagicIndex != MAX_INDEX && cbCardIndexTemp[m_cbMagicIndex] > 0)
						{											
							pKindItem[i/3]->cbValidIndex[i%3] = m_cbMagicIndex;
							cbCardIndexTemp[m_cbMagicIndex]--;
						}
						else
						{
							bEnoughCard=false;
							break;
						}
					}
					else cbCardIndexTemp[cbCardIndex]--;
				}

				//�����ж�
				if (bEnoughCard==true)
				{
					//�����ж�
					BYTE cbCardEye=0;
					bool bMagicEye = false;
					if(GetCardCount(cbCardIndexTemp) == 2)
					{
						if(m_cbMagicIndex != MAX_INDEX && cbCardIndexTemp[m_cbMagicIndex]==2)
						{
							cbCardEye = SwitchToCardData(m_cbMagicIndex);
							bMagicEye = true;
						}
						else
						{
							for (BYTE i=0;i<MAX_INDEX;i++)
							{
								if (cbCardIndexTemp[i]==2)
								{
									cbCardEye=SwitchToCardData(i);
									if(m_cbMagicIndex != MAX_INDEX && i == m_cbMagicIndex) 
									{
										bMagicEye = true;
									}
									break;
								}
								else if(i!=m_cbMagicIndex && m_cbMagicIndex != MAX_INDEX && cbCardIndexTemp[i]+cbCardIndexTemp[m_cbMagicIndex]==2)
								{
									cbCardEye = SwitchToCardData(i);
									bMagicEye = true;
									break;
								}
							}
						}
					}

					//�������
					if (cbCardEye!=0)
					{
						//��������
						tagAnalyseItem AnalyseItem;
						ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

						//�������
						for (BYTE i=0;i<cbWeaveCount;i++)
						{
							AnalyseItem.cbWeaveKind[i]=WeaveItem[i].cbWeaveKind;
							AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
							GetWeaveCard(WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,AnalyseItem.cbCardData[i]);
						}

						//��������
						for (BYTE i=0;i<cbLessKindItem;i++) 
						{
							AnalyseItem.cbWeaveKind[i+cbWeaveCount]=pKindItem[i]->cbWeaveKind;
							AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
							AnalyseItem.cbCardData[cbWeaveCount+i][0] = SwitchToCardData(pKindItem[i]->cbValidIndex[0]);
							AnalyseItem.cbCardData[cbWeaveCount+i][1] = SwitchToCardData(pKindItem[i]->cbValidIndex[1]);
							AnalyseItem.cbCardData[cbWeaveCount+i][2] = SwitchToCardData(pKindItem[i]->cbValidIndex[2]);
						}

						//��������
						AnalyseItem.cbCardEye=cbCardEye;
						AnalyseItem.bMagicEye = bMagicEye;

						//������
						AnalyseItemArray.Add(AnalyseItem);
					}
				}
			}

			//��������
			if (cbIndex[cbLessKindItem-1]==(cbKindItemCount-1))
			{
				BYTE i= cbLessKindItem - 1;

				for (;i>0;i--)
				{
					if ((cbIndex[i-1]+1)!=cbIndex[i])
					{
						BYTE cbNewIndex=cbIndex[i-1];
						for (BYTE j=(i-1);j<cbLessKindItem;j++) 
							cbIndex[j]=cbNewIndex+j-i+2;
						break;
					}
				}
				if (i==0) break;
			}
			else
				cbIndex[cbLessKindItem-1]++;

		} while (true);
	}

	return (AnalyseItemArray.GetCount()>0);
}

/*
// ������������
*/

//������
bool CGameLogic::IsPengPeng(const tagAnalyseItem *pAnalyseItem)
{
	for(BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if(pAnalyseItem->cbWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))
			return false;
	}
	return true;
}

//�Ƿ�����ϵ��
bool CGameLogic::IsMaQi(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount,CChiHuRight &ChiHuRight)
{
	if(cbWeaveCount!=0)
		return false;

	BYTE cbGang = 0;
	BYTE cbMagicCount = m_cbMagicIndex != MAX_INDEX ? cbCardIndex[m_cbMagicIndex] : 0;
	//��������
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if(cbCardIndex[i] != 0 && i != m_cbMagicIndex)
		{			
			if(cbCardIndex[i] % 2 == 1)
			{
				if(cbMagicCount >= 1)
				{
					cbMagicCount--;
				}
				else
				{
					return false;//�зǶ��ӣ�����
				}
			}

			cbGang+= cbCardIndex[i] / 4;
		}
	}

	if(cbGang >= 2)//����������4�ţ�������
	{
		ChiHuRight |= CHR_MA_QI_WZW;
		return true;
	}
	else if(cbGang == 1)//��һ��4�ţ�������
	{
		ChiHuRight |= CHR_MA_QI_WANG;
		return true;
	}
	else//���߶�
	{
		ChiHuRight |=CHR_MA_QI_DUI;
		return true;
	}

	return false;
}
//ʮ����ϵ��
bool CGameLogic::IsShiSanLan(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount,CChiHuRight &ChiHuRight)
{
	//����ж�
	if (cbWeaveCount!=0) return false;

	for(int i=0;i<MAX_INDEX;i++)
	{
		if(cbCardIndex[i]>=2)//�������ظ���
			return false;
	}

	for(int j=0;j<3;j++)
	{
		for(int i=0;i<9-2;i++)
		{
			int index = j*9+i;
			if(cbCardIndex[index]+cbCardIndex[index+1]+cbCardIndex[index+2]>1)
			{
				//if(cbCardIndex[index+1]>0 || cbCardIndex[index+2]>0)//�������>=3
					return false;
			}
		}
	}

	for(int i=27;i<MAX_INDEX;i++)//������
	{
		if(cbCardIndex[i]==0)//û�а������з��ƣ�ʮ����
		{
			ChiHuRight |= CHR_SHI_SAN_LAN;
			return true;
		}
	}

	//���з��ƶ��У�����ʮ����
	ChiHuRight |= CHR_QX_SHI_SAN_LAN;
	return true;
}

//��С����
bool CGameLogic::IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount)
{
	//����ж�
	if (cbWeaveCount!=0) return false;

	//������Ŀ
	BYTE cbReplaceCount = 0;

	//���㵥��
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		BYTE cbCardCount=cbCardIndex[i];

		//���ƹ���
		if( i == m_cbMagicIndex ) continue;

		//����ͳ��
		if( cbCardCount == 1 || cbCardCount == 3 ) 	cbReplaceCount++;

	}

	//���Ʋ���
	if( m_cbMagicIndex != MAX_INDEX && cbReplaceCount > cbCardIndex[m_cbMagicIndex] ||
		m_cbMagicIndex == MAX_INDEX && cbReplaceCount > 0 )
		return false;

	return true;
}

//����
bool CGameLogic::IsJiHu(const tagAnalyseItem *pAnalyseItem)
{
	bool bPeng = false,bLian = false;
	for(BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if(pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG))
			bPeng = true;
		else bLian = true;
	}

	return bPeng&&bLian;
}

//ƽ��
bool CGameLogic::IsPingHu(const tagAnalyseItem *pAnalyseItem)
{
	//������
	for(BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if(pAnalyseItem->cbWeaveKind[i]&(WIK_PENG|WIK_GANG)) return false;
	}

	return true;
}

//��һɫ
bool CGameLogic::IsQingYiSe(const tagAnalyseItem * pAnalyseItem, bool &bQuanFan)
{
	//����У��
	if(pAnalyseItem==NULL) return false;

	//��������
	BYTE  cbCardColor=pAnalyseItem->cbCardEye&MASK_COLOR;
	for(BYTE i=0;i<MAX_WEAVE;i++)
	{
		if((pAnalyseItem->cbCenterCard[i]&MASK_COLOR) != cbCardColor) 
		{
			return false;
		}
	}

	if(0x30 == cbCardColor)
	{
		bQuanFan = true;
	}
	else 
	{
		bQuanFan = false;
	}

	return true;
}

//��һɫ
bool CGameLogic::IsHunYiSe(const tagAnalyseItem * pAnalyseItem)
{
	//����У��
	if(pAnalyseItem==NULL) return false;

	//��������
	BYTE  cbCardColor=(pAnalyseItem->cbCardEye&MASK_COLOR)>>4;
	ASSERT(cbCardColor >= 0 && cbCardColor <= 3);
	BYTE cbColorCount[4] = {0};
	cbColorCount[cbCardColor] = 1;
	for(BYTE i=0;i<MAX_WEAVE;i++)
	{
		cbCardColor=((pAnalyseItem->cbCenterCard[i])&MASK_COLOR)>>4;
		ASSERT(cbCardColor >= 0 && cbCardColor <= 3);
		if(0 == cbColorCount[cbCardColor])
		{
			cbColorCount[cbCardColor] = 1;
		}
	}

	if(cbColorCount[0] + cbColorCount[1] + cbColorCount[2] == 1 && cbColorCount[3] == 1)
	{
		return true;
	}

	return false;
}





//////////////////////////////////////////////////////////////////////////////////
