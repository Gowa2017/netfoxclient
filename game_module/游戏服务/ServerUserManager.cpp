#include "StdAfx.h"
#include "ServerUserManager.h"

//////////////////////////////////////////////////////////////////////////////////
const TCHAR g_szGameDataSpe[]=TEXT(":;");

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerUserItem::CServerUserItem()
{
	//���ӱ���
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//�йܱ���
	m_lTrusteeScore=0L;
	m_lRestrictScore=0L;
	m_lFrozenedScore=0L;
	m_lFrozenedScore=0L;

	//��¼��Ϣ
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//�ͻ�����
	m_bMobileUser=false;
	m_bAndroidUser=false;

	//����ӿ�
	m_pIServerUserItemSink=NULL;
	m_pIMatchUserItemSink=NULL;

	//ϵͳ����
	m_wBindIndex=INVALID_WORD;
	m_dwClientAddr=INADDR_NONE;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//������Ϣ
	m_dwSignUpTime=0;
	m_cbMatchStatus=0;
	m_pMatchData=NULL;

	//��������
	m_bTrusteeUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));
	ZeroMemory(m_szUserGameData,sizeof(m_szUserGameData));

	//���Ա���
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wLastTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	//��¼��Ϣ
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	//�ֻ�����
	m_wMobileUserRule=0;
	m_wMobileUserRule |= VIEW_MODE_PART;
	m_wMobileUserRule |= VIEW_INFO_LEVEL_1;
	m_wDeskPos=0;

	//��������
	m_UserGameDataMap.InitHashTable(17);

	return;
}

//��������
CServerUserItem::~CServerUserItem()
{
	//�������
	INT wKey=0;
	tagGameDataItem * pGameDataItem=NULL;
	POSITION Position=m_UserGameDataMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserGameDataMap.GetNextAssoc(Position,wKey,pGameDataItem);
		SafeDelete(pGameDataItem);
	}
	m_UserGameDataMap.RemoveAll();
}

//�ӿڲ�ѯ
VOID * CServerUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserItem,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserItem,Guid,dwQueryVer);
	return NULL;
}

//�û�ʤ��
WORD CServerUserItem::GetUserWinRate()
{
	//����ʤ��
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwWinCount*10000L/dwPlayCount);

	return 0;
}

//�û�����
WORD CServerUserItem::GetUserLostRate()
{
	//��������
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwLostCount*10000L/dwPlayCount);

	return 0;
}

//�û�����
WORD CServerUserItem::GetUserDrawRate()
{
	//�������
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwDrawCount*10000L/dwPlayCount);

	return 0;
}

//�û�����
WORD CServerUserItem::GetUserFleeRate()
{
	//��������
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwFleeCount*10000L/dwPlayCount);

	return 0;
}

//��Ϸ����
LPCTSTR CServerUserItem::GetUserGameData()
{
	CString strMainItem=TEXT("");

	//��������
	INT wKey=0;
	tagGameDataItem * pGameDataItem=NULL;
	POSITION Position=m_UserGameDataMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserGameDataMap.GetNextAssoc(Position,wKey,pGameDataItem);
		if (pGameDataItem!=NULL)
		{
			strMainItem.AppendFormat(TEXT("%d:%s;"),pGameDataItem->nKey,pGameDataItem->szValue);
		}
	}

	lstrcpyn(m_szUserGameData,strMainItem,strMainItem.GetLength()+1);

	return m_szUserGameData;
}

//��Ϸ����
VOID CServerUserItem::GetUserGameData(INT nKey, LPTSTR pszValue, INT nMaxCount)
{
	ASSERT(nMaxCount<=LEN_GAME_ITEM);
	if (nMaxCount>LEN_GAME_ITEM) return;

	tagGameDataItem *pGameDataItem=NULL;
	m_UserGameDataMap.Lookup(nKey,pGameDataItem);
	lstrcpyn(pszValue,pGameDataItem->szValue,nMaxCount);
}

//��Ϸ����
VOID CServerUserItem::SetUserGameData(LPCTSTR pszValue, INT nMaxCount)
{
	ASSERT(nMaxCount<=LEN_GAME_DATA);
	if (nMaxCount>LEN_GAME_DATA) return;

	//��Ч���ж�
	if (IsValidGameData(pszValue,g_szGameDataSpe)==false)
		return;

	//������Ϸ����
	ZeroMemory(m_szUserGameData,sizeof(m_szUserGameData));
	lstrcpyn(m_szUserGameData,pszValue,nMaxCount);

	//���¹���Map
	InitUserGameDataMap();

	return;
}

//��Ϸ����
VOID CServerUserItem::SetUserGameData(INT nKey, LPCTSTR pszValue, INT nMaxCount)
{
	ASSERT(nMaxCount<=LEN_GAME_ITEM);
	if (nMaxCount>LEN_GAME_ITEM) return;

	tagGameDataItem *pGameDataItem=NULL;
	m_UserGameDataMap.Lookup(nKey,pGameDataItem);

	if (pGameDataItem!=NULL)
	{
		lstrcpyn(pGameDataItem->szValue,pszValue,nMaxCount);
	}
	else
	{
		try
		{
			pGameDataItem=new tagGameDataItem;

			pGameDataItem->nKey=nKey;
			lstrcpyn(pGameDataItem->szValue,pszValue,nMaxCount);

			m_UserGameDataMap[nKey]=pGameDataItem;
		}
		catch (...)
		{
			ASSERT(FALSE);
			return;
		}
	}
}

//��Ϸ����
VOID CServerUserItem::WriteUserGameData()
{
	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemGameData(this,0);

	return;
}

//�Ա��ʺ�
bool CServerUserItem::ContrastNickName(LPCTSTR pszNickName)
{
	//Ч�����
	ASSERT(pszNickName!=NULL);
	if (pszNickName==NULL) return false;

	//���ȶԱ�
	INT nContrastLen=lstrlen(pszNickName);
	INT nSourceLen=lstrlen(m_UserInfo.szNickName);

	//�ַ��Ա�
	if (nContrastLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszNickName,nContrastLen,m_UserInfo.szNickName,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//�Ա�����
bool CServerUserItem::ContrastLogonPass(LPCTSTR pszPassword)
{
	//Ч�����
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	//���ȶԱ�
	INT nTargetLen=lstrlen(pszPassword);
	INT nSourceLen=lstrlen(m_szLogonPass);

	//����Ա�
	if (nTargetLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszPassword,nTargetLen,m_szLogonPass,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//����״̬
bool CServerUserItem::SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)
{
	//Ч��״̬
	//ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//��¼��Ϣ
	WORD wOldTableID=m_UserInfo.wTableID;
	WORD wOldChairID=m_UserInfo.wChairID;

	//���ñ���
	m_UserInfo.wTableID=wTableID;
	m_UserInfo.wChairID=wChairID;
	m_UserInfo.cbUserStatus=cbUserStatus;
	if(cbUserStatus==US_PLAYING)m_UserInfo.wLastTableID=wTableID;

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemStatus(this,wOldTableID,wOldChairID);

	return true;
}

//����״̬
VOID CServerUserItem::SetUserMatchStatus(BYTE cbMatchStatus)
{
	BYTE cbOldMatchStatus=m_cbMatchStatus;

	m_cbMatchStatus=cbMatchStatus;

	//����״̬
	/*ASSERT(m_pIMatchUserItemSink!=NULL);
	if (m_pIMatchUserItemSink!=NULL) m_pIMatchUserItemSink->OnEventMatchUserStatus(this,cbOldMatchStatus,cbMatchStatus);*/
}

//д�����
bool CServerUserItem::WriteUserScore(SCORE lScore, SCORE lGrade, SCORE lRevenue, SCORE lIngot, BYTE cbScoreType, DWORD dwPlayTimeCount,DWORD dwWinExperience)
{
	//Ч��״̬
	ASSERT((m_UserInfo.dwUserID!=0L)&&(cbScoreType!=SCORE_TYPE_NULL));
	if ((m_UserInfo.dwUserID==0L)||(cbScoreType==SCORE_TYPE_NULL)) return false;

	//�޸�ʤ��
	switch (cbScoreType)
	{
	case SCORE_TYPE_WIN:	//ʤ��
		{
			m_UserInfo.dwWinCount++;
			m_RecordInfo.dwWinCount++;
			m_VariationInfo.dwWinCount++;
			break;
		}
	case SCORE_TYPE_LOSE:	//���
		{
			m_UserInfo.dwLostCount++;
			m_RecordInfo.dwLostCount++;
			m_VariationInfo.dwLostCount++;
			break;
		}
	case SCORE_TYPE_DRAW:	//�;�
		{
			m_UserInfo.dwDrawCount++;
			m_RecordInfo.dwDrawCount++;
			m_VariationInfo.dwDrawCount++;
			break;
		}
	case SCORE_TYPE_FLEE:	//�Ӿ�
		{
			m_UserInfo.dwFleeCount++;
			m_RecordInfo.dwFleeCount++;
			m_VariationInfo.dwFleeCount++;
			break;
		}
	}

	//���ñ�־
	m_bModifyScore=true;

	//���û���
	m_UserInfo.lScore+=lScore;
	m_UserInfo.lGrade+=lGrade;
	m_UserInfo.lIngot+=lIngot;
	m_UserInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;	

	//��ʷ����
	m_UserInfo.lIntegralCount+=lScore;
	m_RecordInfo.lIntegralCount+=lScore;
	m_VariationInfo.lIntegralCount+=lScore;

	//��¼��Ϣ
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lGrade+=lGrade;
	m_RecordInfo.lRevenue+=lRevenue;
	m_RecordInfo.lIngot+=lIngot;
	m_RecordInfo.dwPlayTimeCount+=dwPlayTimeCount;
	m_RecordInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;

	//�����Ϣ
	m_VariationInfo.lIngot+=lIngot;
	m_VariationInfo.dwPlayTimeCount+=dwPlayTimeCount;
	m_VariationInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;
	
	//�������
	m_VariationInfo.lScore+=lScore;
	m_VariationInfo.lGrade+=lGrade;
	m_VariationInfo.lRevenue+=lRevenue;	

	//�йܻ���
	if (m_lRestrictScore>0L)
	{
		//��������
		SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

		//�йܵ���
		if (lTotalScore>m_lRestrictScore)
		{
			m_UserInfo.lScore=m_lRestrictScore;
			m_lTrusteeScore=lTotalScore-m_lRestrictScore;
		}
		else
		{
			m_lTrusteeScore=0L;
			m_UserInfo.lScore=lTotalScore;
		}
	}

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this,SCORE_REASON_WRITE);

	return true;
}

//��ȡ����
bool CServerUserItem::SetUserTaskReward(SCORE lScore, SCORE lIngot)
{
	m_UserInfo.lScore=lScore;
	m_UserInfo.lIngot=lIngot;

	return true;
}

//��ѯ��¼
bool CServerUserItem::QueryRecordInfo(tagVariationInfo & RecordInfo)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//���ñ���
	RecordInfo=m_RecordInfo;

	return true;
}

//��ȡ���
bool CServerUserItem::DistillVariation(tagVariationInfo & VariationInfo)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//�޸��ж�
	if (m_bModifyScore==true)
	{
		//���ñ���
		VariationInfo=m_VariationInfo;
	}
	else
	{
		//���ñ���
		ZeroMemory(&VariationInfo,sizeof(VariationInfo));
	}

	//��ԭ����
	m_bModifyScore=false;
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	return true;
}

//�������
bool CServerUserItem::FrozenedUserScore(SCORE lScore)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//Ч�����
	ASSERT((lScore>=0L)&&((m_UserInfo.lScore+m_lTrusteeScore)>=lScore));
	if ((lScore<0L)||((m_UserInfo.lScore+m_lTrusteeScore)<lScore)) return false;

	//���ñ���
	m_lFrozenedScore+=lScore;

	//�����û�
	if (m_lTrusteeScore<lScore)
	{
		//��������
		lScore-=m_lTrusteeScore;
		m_UserInfo.lScore-=lScore;
		m_lTrusteeScore=0L;
	}
	else
	{
		//�й�����
		m_lTrusteeScore-=lScore;
	}

	return true;
}

//�ⶳ����
bool CServerUserItem::UnFrozenedUserScore(SCORE lScore)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//Ч�����
	ASSERT((lScore>=0L)&&(m_lFrozenedScore>=lScore));
	if ((lScore<0L)||(m_lFrozenedScore<lScore)) return false;

	//���ñ���
	m_lFrozenedScore-=lScore;
	m_UserInfo.lScore+=lScore;

	//�йܻ���
	if (m_lRestrictScore>0L)
	{
		//��������
		SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

		//�йܵ���
		if (lTotalScore>m_lRestrictScore)
		{
			m_UserInfo.lScore=m_lRestrictScore;
			m_lTrusteeScore=lTotalScore-m_lRestrictScore;
		}
		else
		{
			m_lTrusteeScore=0L;
			m_UserInfo.lScore=lTotalScore;
		}
	}

	return true;
}

//�޸���Ϣ
bool CServerUserItem::ModifyUserProperty(SCORE lScore,LONG lLoveLiness)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//���û���
	m_UserInfo.lScore+=lScore;
	m_UserInfo.lLoveLiness+=lLoveLiness;

	//��¼��Ϣ
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lLoveLiness+=lLoveLiness;

	//�����Ϣ
	if (lLoveLiness!=0L)
	{
		m_VariationInfo.lLoveLiness+=lLoveLiness;
	}

	//�йܻ���
	if ((m_lRestrictScore>0L)&&(lScore!=0L))
	{
		//��������
		SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

		//�йܵ���
		if (lTotalScore>m_lRestrictScore)
		{
			m_UserInfo.lScore=m_lRestrictScore;
			m_lTrusteeScore=lTotalScore-m_lRestrictScore;
		}
		else
		{
			m_lTrusteeScore=0L;
			m_UserInfo.lScore=lTotalScore;
		}
	}

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, SCORE_REASON_PROPERTY);

	return true;
}

//�����
bool CServerUserItem::DetachBindStatus()
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//�û�����
	m_bClientReady=false;

	//��������
	m_wBindIndex=INVALID_WORD;

	return true;
}

//���в���
bool CServerUserItem::ModifyUserInsure(SCORE lScore, SCORE lInsure, SCORE lRevenue)
{
	//ƽ��Ч��
	/*ASSERT((m_bAndroidUser==true)||(((lScore+lInsure+lRevenue)==0L)&&(lRevenue>=0L)));
	if ((m_bAndroidUser==false)&&(((lScore+lInsure+lRevenue)!=0L)||(lRevenue<0L))) return false;*/

	//Ч��״̬
	/*ASSERT((m_UserInfo.dwUserID!=0L)&&((m_UserInfo.lScore+m_lTrusteeScore+lScore)>=0L));
	if ((m_UserInfo.dwUserID==0L)||((m_UserInfo.lScore+m_lTrusteeScore+lScore)<0L)) return false;*/

	//���û���
	m_UserInfo.lScore+=lScore;
	m_UserInfo.lInsure+=lInsure;

	//��¼��Ϣ
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lInsure+=lInsure;
	m_RecordInfo.lRevenue+=lRevenue;

	//�йܻ���
	if (m_lRestrictScore>0L)
	{
		//��������
		SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

		//�йܵ���
		if (lTotalScore>m_lRestrictScore)
		{
			m_UserInfo.lScore=m_lRestrictScore;
			m_lTrusteeScore=lTotalScore-m_lRestrictScore;
		}
		else
		{
			m_lTrusteeScore=0L;
			m_UserInfo.lScore=lTotalScore;
		}
	}

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, SCORE_REASON_INSURE);

	return true;
}

//���ò���
bool CServerUserItem::SetUserParameter(DWORD dwClientAddr, WORD wBindIndex, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser, bool bClientReady)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//�û�����
	m_bAndroidUser=bAndroidUser;
	m_bClientReady=bClientReady;

	//��������
	m_wBindIndex=wBindIndex;
	m_dwClientAddr=dwClientAddr;
	lstrcpyn(m_szMachineID,szMachineID,CountArray(m_szMachineID));

	return true;
}

//�޸�Ȩ��
VOID CServerUserItem::ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind)
{
	//���Ȩ��
	m_dwUserRight |= dwAddRight;

	//ɾ��Ȩ��
	m_dwUserRight &= ~dwRemoveRight;

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemRight(this,dwAddRight,dwRemoveRight,cbRightKind);

	return;
}

//��������
VOID CServerUserItem::ResetUserItem()
{
	//���ӱ���
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//�йܱ���
	m_lTrusteeScore=0L;
	m_lRestrictScore=0L;
	m_lFrozenedScore=0L;
	m_lFrozenedScore=0L;

	//��¼��Ϣ
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//ϵͳ����
	m_wBindIndex=INVALID_WORD;
	m_dwClientAddr=INADDR_ANY;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//�ͻ�����
	m_bMobileUser=false;
	m_bAndroidUser=false;

	//������Ϣ
	m_dwSignUpTime=0;
	m_cbMatchStatus=0;
	m_pMatchData=NULL;

	//��������
	m_bTrusteeUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));
	ZeroMemory(m_szUserGameData,sizeof(m_szUserGameData));

	//���Ա���
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wLastTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	//��¼��Ϣ
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	//�ֻ�����
	m_wMobileUserRule=0;
	m_wMobileUserRule |= VIEW_MODE_PART;
	m_wMobileUserRule |= VIEW_INFO_LEVEL_1;
	m_wDeskPos=0;

	//�ӿڱ���
	m_pIServerUserItemSink=NULL;
	m_pIMatchUserItemSink=NULL;

	//�������
	INT wKey=0;
	tagGameDataItem * pGameDataItem=NULL;
	POSITION Position=m_UserGameDataMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserGameDataMap.GetNextAssoc(Position,wKey,pGameDataItem);
		SafeDelete(pGameDataItem);
	}
	m_UserGameDataMap.RemoveAll();

	return;
}

//������Ϸ����
VOID CServerUserItem::InitUserGameDataMap()
{
	//�������
	INT wKey=0;
	tagGameDataItem * pGameDataItem=NULL;
	POSITION Position=m_UserGameDataMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserGameDataMap.GetNextAssoc(Position,wKey,pGameDataItem);
		SafeDelete(pGameDataItem);
	}
	m_UserGameDataMap.RemoveAll();

	//��Ϸ���ݸ�ʽ�� Key1:Value1;Key2:Value2;Key3:Value3;Key4:Value4;
	CStringArray SubItemArray;
	SplitString(m_szUserGameData,g_szGameDataSpe,SubItemArray);

	INT nSubItemCount=(INT)SubItemArray.GetCount();
	ASSERT(nSubItemCount%2==0);
	if(nSubItemCount%2!=0) return;

	TCHAR szSubItem[32]=TEXT("");
	for (INT i=nSubItemCount-1; i>=0; i-=2)
	{
		_sntprintf(szSubItem,CountArray(szSubItem),TEXT("%s"),SubItemArray.GetAt(i-1));
	
		bool bPureDigit=IsPureDigitString(szSubItem);
		if (bPureDigit)
		{
			INT nKey=_ttoi(szSubItem);
			CString pszValue=SubItemArray.GetAt(i);

			tagGameDataItem *pNewDataItem=new tagGameDataItem;
			pNewDataItem->nKey=nKey;
			lstrcpyn(pNewDataItem->szValue,pszValue,CountArray(pNewDataItem->szValue));

			//���뼯��
			m_UserGameDataMap[nKey]=pNewDataItem;
		}
	}

	return;
}

//�ַ������
VOID CServerUserItem::SplitString(CString strSource, CString strSpliter, CStringArray& ResultArray)
{
	////����У��
	//ASSERT(strSource.GetLength()>0 && strSpliter.GetLength()>0);
	//if (strSource.GetLength()<=0 || strSpliter.GetLength()<=0) return;

	//�������
	ResultArray.RemoveAll();

	int nPostion=0;
	while (1)
	{
		CString strToken=strSource.Tokenize(strSpliter,nPostion);
		if (strToken.IsEmpty())
			break;
		ResultArray.Add(strToken);
	}
}

//�Ƿ�Ϊ�������ַ���
bool CServerUserItem::IsPureDigitString(CString strSource)
{
	for (INT i=0; i< strSource.GetLength(); i++)
	{
		TCHAR chSplit=strSource[i];
		if (chSplit<TEXT('0') || chSplit>TEXT('9'))
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return true;
}

//��Ч���ж�
bool CServerUserItem::IsValidGameData(CString strSource, CString strSpliter)
{
	////����У��
	//ASSERT(strSource.GetLength()>0 && strSpliter.GetLength()>0);
	//if (strSource.GetLength()<=0 || strSpliter.GetLength()<=0) return false;

	CStringArray SubItemArray;
	SplitString(strSource,strSpliter,SubItemArray);

	INT nSubItemCount=(INT)SubItemArray.GetCount();
	ASSERT(nSubItemCount%2==0);
	if(nSubItemCount%2!=0) return false;

	TCHAR szSubItem[LEN_GAME_ITEM]=TEXT("");
	for (INT i=0; i<nSubItemCount; i+=2)
	{
		_sntprintf(szSubItem,CountArray(szSubItem),TEXT("%s"),SubItemArray.GetAt(i));

		bool bPureDigit=IsPureDigitString(szSubItem);
		bool bChString=SubItemArray.GetAt(i+1).GetLength()>0;

		if (bPureDigit==false || bChString==false)
			return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerUserManager::CServerUserManager()
{
	//����ӿ�
	m_pIServerUserItemSink=NULL;
	m_pIMatchUserItemSink=NULL;

	//��������
	m_UserIDMap.InitHashTable(PRIME_SERVER_USER);

	return;
}

//��������
CServerUserManager::~CServerUserManager()
{
	//�ͷ��û�
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++) m_UserItemStore[i]->Release();
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++) m_UserItemArray[i]->Release();

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CServerUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserManager,Guid,dwQueryVer);
	return NULL;
}

//���ýӿ�
bool CServerUserManager::SetServerUserItemSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink)!=NULL);
		m_pIServerUserItemSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink);

		//�ɹ��ж�
		if (m_pIServerUserItemSink==NULL) return false;
	}
	else m_pIServerUserItemSink=NULL;

	return true;
}

//���ýӿ�
bool CServerUserManager::SetMatchUserItemSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IMatchUserItemSink)!=NULL);
		m_pIMatchUserItemSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IMatchUserItemSink);

		//�ɹ��ж�
		if (m_pIMatchUserItemSink==NULL) return false;
	}
	else m_pIMatchUserItemSink=NULL;

	return true;
}

//ö���û�
IServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//�����û�
IServerUserItem * CServerUserManager::SearchUserItem(DWORD dwUserID)
{
	return m_UserIDMap[dwUserID];
}

//��������
DWORD CServerUserManager::GetAndroidCount()
{
	//��������
	DWORD dwAndroidCount=0;
	CServerUserItem * pServerUserItem=NULL;

	//�����û�
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if(pServerUserItem->IsAndroidUser() == true) dwAndroidCount++;
	}

	return dwAndroidCount;
}

//�����û�
IServerUserItem * CServerUserManager::SearchUserItem(LPCTSTR pszNickName)
{
	//��������
	CServerUserItem * pServerUserItem=NULL;

	//�����û�
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if (pServerUserItem->ContrastNickName(pszNickName)==true) return pServerUserItem;
	}

	return NULL;
}

//ɾ���û�
bool CServerUserManager::DeleteUserItem()
{
	//�洢����
	m_UserItemStore.Append(m_UserItemArray);

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();

	return true;
}

//ɾ���û�
bool CServerUserManager::DeleteUserItem(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()==US_NULL));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetUserStatus()!=US_NULL)) return false;

	//��������
	CServerUserItem * pTempUserItem=NULL;
	DWORD dwUserID=pIServerUserItem->GetUserID();

	//Ѱ�Ҷ���
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//��ȡ�û�
		pTempUserItem=m_UserItemArray[i];
		if (pIServerUserItem!=pTempUserItem) continue;

		//���ö���
		pTempUserItem->ResetUserItem();

		//ɾ������
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�����û�
bool CServerUserManager::InsertUserItem(IServerUserItem * * pIServerUserResult, tagUserInfo & UserInfo,tagUserInfoPlus & UserInfoPlus)
{
	//��������
	CServerUserItem * pServerUserItem=NULL;

	//��ȡָ��
	if (m_UserItemStore.GetCount()>0)
	{
		INT_PTR nItemPostion=m_UserItemStore.GetCount()-1;
		pServerUserItem=m_UserItemStore[nItemPostion];
		m_UserItemStore.RemoveAt(nItemPostion);

		//���ö���
		pServerUserItem->ResetUserItem();		
	}
	else
	{
		try
		{
			pServerUserItem=new CServerUserItem;
		}
		catch (...)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//���ýӿ�
	pServerUserItem->m_pIServerUserItemSink=m_pIServerUserItemSink;
	pServerUserItem->m_pIMatchUserItemSink=m_pIMatchUserItemSink;

	//ԭʼ����
	pServerUserItem->m_ScoreFormer.lScore=UserInfo.lScore;
	pServerUserItem->m_ScoreFormer.dwWinCount=UserInfo.dwWinCount;
	pServerUserItem->m_ScoreFormer.dwLostCount=UserInfo.dwLostCount;
	pServerUserItem->m_ScoreFormer.dwDrawCount=UserInfo.dwDrawCount;
	pServerUserItem->m_ScoreFormer.dwFleeCount=UserInfo.dwFleeCount;
	pServerUserItem->m_ScoreFormer.dwExperience=UserInfo.dwExperience;
	pServerUserItem->m_ScoreFormer.lIntegralCount = UserInfo.lIntegralCount;

	//���Ա���
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));
	ZeroMemory(&pServerUserItem->m_RecordInfo,sizeof(pServerUserItem->m_RecordInfo));
	ZeroMemory(&pServerUserItem->m_VariationInfo,sizeof(pServerUserItem->m_VariationInfo));

	//��¼��Ϣ
	pServerUserItem->m_dwLogonTime=UserInfoPlus.dwLogonTime;
	pServerUserItem->m_dwInoutIndex=UserInfoPlus.dwInoutIndex;

	//�û�Ȩ��
	pServerUserItem->m_dwUserRight=UserInfoPlus.dwUserRight;
	pServerUserItem->m_dwMasterRight=UserInfoPlus.dwMasterRight;
	pServerUserItem->m_lRestrictScore=UserInfoPlus.lRestrictScore;

	//������Ϣ
	pServerUserItem->m_wBindIndex=UserInfoPlus.wBindIndex;
	pServerUserItem->m_dwClientAddr=UserInfoPlus.dwClientAddr;
	lstrcpyn(pServerUserItem->m_szMachineID,UserInfoPlus.szMachineID,CountArray(pServerUserItem->m_szMachineID));

	//��������
	pServerUserItem->m_bClientReady=false;
	pServerUserItem->m_bModifyScore=false;
	pServerUserItem->m_bTrusteeUser=false;
	pServerUserItem->m_bMobileUser=UserInfoPlus.bMobileUser;
	pServerUserItem->m_bAndroidUser=UserInfoPlus.bAndroidUser;
	lstrcpyn(pServerUserItem->m_szLogonPass,UserInfoPlus.szPassword,CountArray(pServerUserItem->m_szLogonPass));
	lstrcpyn(pServerUserItem->m_szUserGameData,UserInfoPlus.szUserGameData,CountArray(pServerUserItem->m_szUserGameData));
	if (pServerUserItem->IsValidGameData(pServerUserItem->m_szUserGameData,g_szGameDataSpe)==true)
	{
		pServerUserItem->InitUserGameDataMap();
	}

	//�йܵ���
	if ((UserInfoPlus.lRestrictScore>0L)&&(UserInfo.lScore>UserInfoPlus.lRestrictScore))
	{
		pServerUserItem->m_UserInfo.lScore=UserInfoPlus.lRestrictScore;
		pServerUserItem->m_lTrusteeScore=UserInfo.lScore-UserInfoPlus.lRestrictScore;
	}

	//�����û�
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	//���ñ���
	*pIServerUserResult=pServerUserItem;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
