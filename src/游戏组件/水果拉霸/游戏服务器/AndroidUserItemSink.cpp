#include "Stdafx.h"
#include "AndroidUserItemSink.h"
//////////////////////////////////////////////////////////////////////////

//��Ϸʱ��
#define IDI_START_GAME				200									//���ƶ�ʱ��
#define IDI_USER_OPERATE			201									//��ʼ��ʱ��
#define IDI_USER_BANK				202									//��ʼ��ʱ��

#define TIME_USER_START_GAME		5									//��ʼʱ��
#define TIME_USER_OPERATE			30									//����ʱ��
//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	return;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
void *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);

	return NULL;
}

//��ʼ�ӿ�
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem)!=NULL);
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);

	//�����ж�
	if (m_pIAndroidUserItem==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//���ýӿ�
bool  CAndroidUserItemSink::RepositionSink()
{
	return true;
}

//ʱ����Ϣ
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:		//��ʼ��ʱ��
		{			
			//��������
			CMD_C_OneStart OneStart;
			OneStart.lBetCount = 1;
			OneStart.lBet = 1;	
			//������Ϣ
			m_pIAndroidUserItem->SendUserReady(NULL, NULL);
			m_pIAndroidUserItem->SendSocketData(SUB_C_ONE_START, &OneStart, sizeof(OneStart));

			return true;
		}
	case IDI_USER_OPERATE:		//��ʼ��ʱ��
		{			
			if (m_cbGameMode == GM_SMALL)
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_THREE_END);
			}
			else
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_ONE_END);
			}
			return true;
		}
	case IDI_USER_BANK:		//��ȡ�ʱ��
		{			
			m_pIAndroidUserItem->KillGameTimer(nTimerID);

			//��������
			IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			LONGLONG lRobotScore = pUserItem->GetUserScore();	
			tagAndroidParameter *pAndroidParameter = m_pIAndroidUserItem->GetAndroidParameter();

			if (lRobotScore < pAndroidParameter->lTakeMinScore / 2)
			{
				LONGLONG lScore=pAndroidParameter->lTakeMinScore + (rand()%(LONGLONG)__max(pAndroidParameter->lTakeMaxScore - pAndroidParameter->lTakeMinScore, 1));
				if (lScore > 0)
				{
					m_pIAndroidUserItem->PerformTakeScore(lScore);
				}
			}
			else if (lRobotScore > pAndroidParameter->lTakeMaxScore * 2)
			{
				LONGLONG lScore=rand()%(LONGLONG)__max(lRobotScore - pAndroidParameter->lTakeMinScore, 1);
				if (lScore > 0)
				{
					m_pIAndroidUserItem->PerformSaveScore(lScore);
				}
			}

			return true;
		}	
	}
	return false;
}

//��Ϸ��Ϣ
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_SMALL_START:
		{
			return true;
		}
	case SUB_S_GAME_CONCLUDE:	//��Ϸ����
		{
			return OnSubGameConclude(pData,wDataSize);
		}	
	}
	return true;

}

//��Ϸ��Ϣ
bool  CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:		//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			//��������
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			//��ʼʱ��
			UINT nElapse= 2 + (rand() % TIME_USER_START_GAME);
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
			return true;
		}
	case GAME_SMALL_GAME:		//С��Ϸ״̬
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_S_SmallStatus));
			if (wDataSize != sizeof(CMD_S_SmallStatus)) return false;
			//��������
			CMD_S_SmallStatus * pStatusTwo = (CMD_S_SmallStatus *)pData;
			m_cbGameMode = GM_SMALL;
			//��ʼʱ��
			UINT nElapse= 5 + (rand() % TIME_USER_OPERATE);
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_OPERATE,nElapse);
			return true;
		}
	}

	return true;
}

//�û�����
void  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
void  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
void  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
void  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//��������
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	m_cbGameMode = pGameStart->cbGameMode;
	if (pGameStart->cbGameMode == GM_SMALL)
	{
		//��ʼʱ��
		UINT nElapse= 10 + (rand() % TIME_USER_OPERATE);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_OPERATE,nElapse);
	}
	else
	{
		//��ʼʱ��
		UINT nElapse= 5 + (rand() % 2);
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_OPERATE,nElapse);
	}

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubTwoStart(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameTwoStart));
	if (wDataSize!=sizeof(CMD_S_GameTwoStart)) return false;

	//��������
	CMD_S_GameTwoStart * pTwoStart=(CMD_S_GameTwoStart *)pData;

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubThreeStart(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameThreeStart));
	if (wDataSize!=sizeof(CMD_S_GameThreeStart)) return false;

	//��������
	CMD_S_GameThreeStart * pGameThreeStart=(CMD_S_GameThreeStart *)pData;


	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubThreeKaiJian(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameThreeKaiJiang));
	if (wDataSize!=sizeof(CMD_S_GameThreeKaiJiang)) return false;

	//��������
	CMD_S_GameThreeKaiJiang * pGameThreeKaiJiang=(CMD_S_GameThreeKaiJiang *)pData;

	

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubThreeEnd(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==0);
	if (wDataSize!=0) return false;


	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameConclude(VOID * pData, WORD wDataSize)
{
	//��ʼʱ��
	UINT nElapse= 2 + (rand() % TIME_USER_START_GAME);
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

	if((rand() % 10) < 3)
	{
		UINT nElapseTemp = 2 + (rand() % 2);
		if(nElapse > nElapseTemp)
		{
			m_pIAndroidUserItem->SetGameTimer(IDI_USER_BANK,nElapse);
		}
	}
	return true;
}

//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
