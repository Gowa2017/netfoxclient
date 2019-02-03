#include "StdAfx.h"
#include "MatchListControl.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CMatchListControl::CMatchListControl()
{
}

//��������
CMatchListControl::~CMatchListControl()
{
}

//�����б�
VOID CMatchListControl::InitListControl()
{
	//��������
	INT nColIndex=0;

	//�����б�
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_CENTER,80);
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_CENTER,200);

	return;
}

//��������
INT CMatchListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//��������
	tagGameMatchOption * pGameMatchOption1=(tagGameMatchOption *)lParam1;
	tagGameMatchOption * pGameMatchOption2=(tagGameMatchOption *)lParam2;

	//��������
	switch (wColumnIndex)
	{
	case 0:		//��������
		{
			return pGameMatchOption1->cbMatchType>pGameMatchOption2->cbMatchType?SORT_POSITION_AFTER:SORT_POSITION_FRONT;			
		}
	case 1:		//��������
		{
			return lstrcmp(pGameMatchOption1->szMatchName,pGameMatchOption2->szMatchName);
		}	
	}

	return 0;
}

//��ȡ��ɫ
VOID CMatchListControl::GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//��������
	ASSERT(lItemParam!=NULL);

	//������ɫ
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

	return;
}

//�����б�
bool CMatchListControl::InsertMatchOption(tagGameMatchOption * pGameMatchOption)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameMatchOption;

	//�����ж�
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pGameMatchOption);
			LPCTSTR pszDescribe=GetDescribeString(pGameMatchOption,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameMatchOption);
		}
		else
		{
			//�ַ�����
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameMatchOption,i),0,0,0,0);
		}
	}

	return true;
}

//�����б�
bool CMatchListControl::UpdateMatchOption(tagGameMatchOption * pGameMatchOption)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameMatchOption;

	//�����ж�
	INT nUpdateItem=FindItem(&FindInfo);
	if (nUpdateItem==LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		//�ַ�����
		SetItem(nUpdateItem,i,LVIF_TEXT,GetDescribeString(pGameMatchOption,i),0,0,0,0);
	}

	//��������
	RedrawItems(nUpdateItem,nUpdateItem);

	return true;
}

//��������
WORD CMatchListControl::GetInsertIndex(tagGameMatchOption * pGameMatchOption)
{
	//��������
	INT nItemCount=GetItemCount();
	tagGameMatchOption * pGameMatchTemp=NULL;

	//��ȡλ��
	for (INT i=0;i<nItemCount;i++)
	{
		//��ȡ����
		pGameMatchTemp=(tagGameMatchOption *)GetItemData(i);

		//��������
		if (lstrcmp(pGameMatchOption->szMatchName,pGameMatchTemp->szMatchName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//�����ַ�
LPCTSTR CMatchListControl::GetDescribeString(tagGameMatchOption * pGameMatchOption, WORD wColumnIndex)
{
	//��������
	static TCHAR szDescribe[128]=TEXT("");

	//�����ַ�
	switch (wColumnIndex)
	{
	case 0:		//��������
		{
			//��������
			LPCTSTR pszMatchType[] = {TEXT("��ʱ��"),TEXT("��ʱ��")};

			//�ж�����
			if(pGameMatchOption->cbMatchType<CountArray(pszMatchType))
			{
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%s"),pszMatchType[pGameMatchOption->cbMatchType]);
			}

			return szDescribe;
		}
	case 1:		//��������
		{
			return pGameMatchOption->szMatchName;
		}	
	//case 2:		//������ʽ
	//	{
	//		if(pGameMatchOption->cbSignupMode==SIGNUP_MODE_SIGNUP_FEE)
	//		{
	//			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("�۳����"));
	//		}
	//		else if(pGameMatchOption->cbSignupMode==SIGNUP_MODE_MATCH_USER)
	//		{
	//			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("��ת��վ"));
	//		}

	//		return szDescribe;
	//	}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////


//���캯��
CRewardListControl::CRewardListControl()
{
}

//��������
CRewardListControl::~CRewardListControl()
{
}

//�����б�
VOID CRewardListControl::InitListControl()
{
	//��������
	INT nColIndex=0;

	//�����б�
	InsertColumn(nColIndex++,TEXT("��    ��"),LVCFMT_LEFT,120);
	InsertColumn(nColIndex++,TEXT("�������"),LVCFMT_LEFT,100);
	InsertColumn(nColIndex++,TEXT("����Ԫ��"),LVCFMT_LEFT,100);
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_LEFT,100);

	return;
}

//��������
INT CRewardListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//��������
	tagMatchRewardInfo * pMatchRewardInfo1=(tagMatchRewardInfo *)lParam1;
	tagMatchRewardInfo * pMatchRewardInfo2=(tagMatchRewardInfo *)lParam2;

	//��������
	switch (wColumnIndex)
	{
	case 0:		//��������
		{
			return pMatchRewardInfo1->wRankID>pMatchRewardInfo2->wRankID;
		}
	case 1:		//�������
		{
			return pMatchRewardInfo1->lRewardGold>pMatchRewardInfo2->lRewardGold;			
		}
	case 2:		//����Ԫ��
		{
			return pMatchRewardInfo1->lRewardIngot>pMatchRewardInfo2->lRewardIngot;			
		}
	case 3:		//��������
		{
			return pMatchRewardInfo1->dwRewardExperience>pMatchRewardInfo2->dwRewardExperience;	;
		}
	}

	return 0;
}

//��ȡ��ɫ
VOID CRewardListControl::GetListItemColor(LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//��������
	ASSERT(lItemParam!=NULL);

	//������ɫ
	ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
	ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

	return;
}

//�����б�
bool CRewardListControl::InsertMatchReward(tagMatchRewardInfo * pMatchRewardInfo)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardInfo;

	//�����ж�
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pMatchRewardInfo);
			LPCTSTR pszDescribe=GetDescribeString(pMatchRewardInfo,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pMatchRewardInfo);
		}
		else
		{
			//�ַ�����
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pMatchRewardInfo,i),0,0,0,0);
		}
	}

	return true;
}

//�����б�
bool CRewardListControl::UpdateMatchReward(tagMatchRewardInfo * pMatchRewardInfo)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardInfo;

	//�����ж�
	INT nUpdateItem=FindItem(&FindInfo);
	if (nUpdateItem==LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		//�ַ�����
		SetItem(nUpdateItem,i,LVIF_TEXT,GetDescribeString(pMatchRewardInfo,i),0,0,0,0);
	}

	//��������
	RedrawItems(nUpdateItem,nUpdateItem);

	return true;
}

//ɾ���б�
bool CRewardListControl::DeleteMatchReward(tagMatchRewardInfo * pMatchRewardInfo)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pMatchRewardInfo;

	//�����ж�
	INT nDeleteItem=FindItem(&FindInfo);
	if (nDeleteItem==LB_ERR) return true;

	//ɾ������
	DeleteItem(nDeleteItem);

	return true;
}

//��������
WORD CRewardListControl::GetInsertIndex(tagMatchRewardInfo * pMatchRewardInfo)
{
	//��������
	INT nItemCount=GetItemCount();
	tagMatchRewardInfo * pMatchRewardTemp=NULL;

	//��ȡλ��
	for (INT i=0;i<nItemCount;i++)
	{
		//��ȡ����
		pMatchRewardTemp=(tagMatchRewardInfo *)GetItemData(i);

		//��������
		if (pMatchRewardInfo->wRankID < pMatchRewardTemp->wRankID)
		{
			return i;
		}
	}

	return nItemCount;
}


//�����ַ�
LPCTSTR CRewardListControl::GetDescribeString(tagMatchRewardInfo * pMatchRewardInfo, WORD wColumnIndex)
{
	//��������
	static TCHAR szDescribe[128]=TEXT("");

	//�����ַ�
	switch (wColumnIndex)
	{
	case 0:		//��������
		{
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("��%02d��"),pMatchRewardInfo->wRankID);

			return szDescribe;
		}
	case 1:		//��ҽ���
		{
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%.2f"),pMatchRewardInfo->lRewardGold);

			return szDescribe;
		}
	case 2:		//Ԫ������
		{
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%.2f"),pMatchRewardInfo->lRewardIngot);

			return szDescribe;
		}
	case 3:		//���齱��
		{
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("%d"),pMatchRewardInfo->dwRewardExperience);

			return szDescribe;
		}	
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
