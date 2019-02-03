#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "StringMessage.h"
#include "AESEncrypt.h"

//////////////////////////////////////////////////////////////////////////

//��ʱ����ʶ
#define IDI_START_GAME				200									//��ʼ��ʱ��
#define IDI_USER_ADD_SCORE			201									//��ע��ʱ��
#define IDI_USER_COMPARE_CARD		202									//ѡ�����û���ʱ��
#define IDI_DISABLE					203									//���˶�ʱ��

//ʱ���ʶ
#define TIME_START_GAME				30									//��ʼ��ʱ��
#define TIME_USER_ADD_SCORE			30									//��ע��ʱ��
#define	TIME_USER_COMPARE_CARD		30									//���ƶ�ʱ��

//��������ԭ��
unsigned char chCommonEncryption[AESENCRYPTION_LENGTH] = 
{
	0x32, 0x43, 0xf6, 0xa8, 
	0x88, 0x5a, 0x30, 0x8d, 
	0x31, 0x31, 0x98, 0xa2, 
	0xe0, 0x37, 0x07, 0x34
};

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_ADD_SCORE,OnAddScore)
	ON_MESSAGE(IDM_MIN_SCORE,OnMinScore)
	ON_MESSAGE(IDM_MAX_SCORE,OnMaxScore)
	ON_MESSAGE(IDM_CONFIRM,OnConfirmScore)
	ON_MESSAGE(IDM_CANCEL_ADD,OnCancelAdd)
	ON_MESSAGE(IDM_LOOK_CARD,OnLookCard)
	ON_MESSAGE(IDM_COMPARE_CARD,OnCompareCard)
	ON_MESSAGE(IDM_OPEN_CARD,OnOpenCard)
	ON_MESSAGE(IDM_GIVE_UP,OnGiveUp)
	ON_MESSAGE(IDM_SEND_CARD_FINISH,OnSendCardFinish)
	ON_MESSAGE(IDM_FALSH_CARD_FINISH,OnFlashCardFinish)
	ON_MESSAGE(IDM_COMPARE_USER,OnChooseCUser)
	ON_MESSAGE(IDM_CHAT_MESSAGE,OnChatMessage)
	ON_MESSAGE(IDM_GAME_SOUND,OnMessageGameSound)

	ON_MESSAGE(IDM_ADMIN_UPDATE_STORAGE, OnStorage)
	ON_MESSAGE(IDM_ADMIN_MODIFY_STORAGE, OnStorageInfo)
	ON_MESSAGE(IDM_REQUEST_QUERY_USER, OnRequestQueryUser)
	ON_MESSAGE(IDM_USER_CONTROL, OnUserControl)
	ON_MESSAGE(IDM_REQUEST_UPDATE_ROOMINFO, OnRequestUpdateRoomInfo)
	ON_MESSAGE(IDM_CLEAR_CURRENT_QUERYUSER, OnClearCurrentQueryUser)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientEngine::CGameClientEngine()
{
	//���ñ���
	m_dwCardHSpace=DEFAULT_PELS;

	//�û���Ϣ
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wWinnerUser=INVALID_CHAIR;
	m_bCheatUser = false;
	m_bPlaySound = true;

	//��ע��Ϣ
	m_lMaxScore=0L;
	m_lMaxCellScore=0L;
	m_lCellScore=0L;
	m_lCurrentTimes=1L;
	m_lUserMaxScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//������Ϣ
	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_wLostUserID[i]=INVALID_CHAIR;
		m_wViewChairID[i]=INVALID_CHAIR;
		m_bMingZhu[i]=false;
	}

	//״̬����
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_strAccounts,sizeof(m_strAccounts));
	m_cbGameCount = 0;
	
	ZeroMemory(m_chUserAESKey, sizeof(m_chUserAESKey));

	return;
}

//��������
CGameClientEngine::~CGameClientEngine()
{
}

//��ʼ����
bool CGameClientEngine::OnInitGameEngine()
{
	//ȫ�ֶ���
	CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();
	ASSERT(m_pGlobalUnits!=NULL);

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

	//��ȡ����
	m_dwCardHSpace=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);

	//��������
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;

	//���ÿؼ�
	for (int i=0;i<GAME_PLAYER;i++)
	{
		if(i!=2)
			m_GameClientView.m_CardControl[i].SetCardSpace(m_dwCardHSpace);
		else
			m_GameClientView.m_CardControl[i].SetCardSpace(20);
	}

	////��������
	CGlobalUnits *pGlobalUnits=CGlobalUnits::GetInstance();
	if (!pGlobalUnits->m_bMuteStatuts)
	{
		PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));
	}
	//pGlobalUnits->m_bAllowSound = true;

	//SetIcon(hIcon,TRUE);
	//SetIcon(hIcon,FALSE);
	//AddScoreControl(SW_SHOW);

	//���Դ���
#ifdef _DEBUG
	//TCHAR szBuffer[32];
	//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("%s��%+I64d"),TEXT("�������"),-5);
	//InsertGeneralString(szBuffer,RGB(0,0,255),true);
	//m_GameClientView.m_btStart.ShowWindow(5);
	//m_GameClientView.m_ScoreView.ResetScore();
	//LONGLONG lGameScore = 10;
	//LONGLONG lGameTax = 10;
	//for (WORD i=0;i<GAME_PLAYER;i++)
	//{	
	//	BYTE bTemp[3]={1,2,3};
	//	m_GameClientView.m_CardControl[i].SetCardData(bTemp,3);
	//	m_GameClientView.m_CardControl[i].SetDisplayHead(true);
	//	//������Ϣ
	//	m_GameClientView.m_ScoreView.SetGameTax(lGameTax,i);
	//	if (m_szAccounts[i][0]==0) m_GameClientView.m_ScoreView.SetGameScore(i,TEXT("���뿪"),lGameScore);
	//	else m_GameClientView.m_ScoreView.SetGameScore(i,m_szAccounts[i],lGameScore);
	//}

	//m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btMinScore.ShowWindow(5);
	//BYTE bTemp[3]={1,2,3};
	//m_GameClientView.m_CardControl[2].SetCardData(bTemp,3);
	//m_GameClientView.m_CardControl[2].SetPositively(true);
	//0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	//0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	//0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	//0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//���� A - K

	//BYTE bFirstTemp[3]={0x32,0x11,0x23};
	//BYTE bNextTemp[3]={0x2b,0x22,0x26};
	//if(m_GameLogic.CompareCard(bFirstTemp,bNextTemp,3))
	//	MessageBox("FirstTemp Win!");
	//else MessageBox("NextTemp Win!");
	/*m_GameClientView.m_ScoreView.SetGameScore(2,"abc",1122);
	m_GameClientView.m_ScoreView.SetGameScore(1,"abc",1221);
	m_GameClientView.m_ScoreView.SetGameScore(0,"ab122221c",122222221);
	m_GameClientView.m_ScoreView.SetGameScore(3,"67",11);
	for (WORD i=0;i<GAME_PLAYER;i++)m_GameClientView.m_ScoreView.SetGameTax(12222223,i);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
	m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
	m_GameClientView.m_btConfirm.ShowWindow(SW_SHOW);
	m_GameClientView.m_btMaxScore.ShowWindow(SW_SHOW);
	m_GameClientView.m_btMinScore.ShowWindow(SW_SHOW);
	m_GameClientView.m_btOpenCard.ShowWindow(SW_SHOW);
	*/
	//BYTE bFirstTemp[3]={0x31,0x23,0x03};

	//for(int i=0;i<GAME_PLAYER;i++)
	//{
	//	//m_GameClientView.SetUserTableScore(2,50,999990);
	//	//m_GameClientView.m_CardControl[i].SetDisplayHead(true);
	//	m_GameClientView.m_CardControl[i].SetCardData(bFirstTemp,3);
	//}
	//m_GameClientView.m_CardControl[2].SetCardColor(2);

	//m_GameClientView.m_btConfirm.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btCancel.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btMinScore.ShowWindow(SW_SHOW);
	//m_GameClientView.m_btMaxScore.ShowWindow(SW_SHOW);
	//m_GameClientView.m_GoldControl.SetGoldCount(10000L);
	//m_GameClientView.m_GoldControl.SetGoldCount(10000);
	//m_GameClientView.m_GoldControl.SetGold(44000L);
	//m_GameClientView.m_GoldControl.SetGoldCount(10);
	//m_GameClientView.m_GoldControl.ShowWindow(SW_SHOW);
	//m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);
	//m_GameClientView.SetScoreInfo(100,100);

	//m_GameClientView.m_btMinScore.ShowWindow(5);
#endif

	return true;
}

//���ÿ��
bool CGameClientEngine::OnResetGameEngine()
{
	//�û���Ϣ
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wWinnerUser=INVALID_CHAIR;

	//��ע��Ϣ
	m_lMaxScore=0L;
	m_lMaxCellScore=0L;
	m_lCellScore=0L;
	m_lCurrentTimes=1L;
	m_lUserMaxScore=0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//������Ϣ
	for (int i=0;i<GAME_PLAYER;i++)
	{
		m_wLostUserID[i]=INVALID_CHAIR;
		m_wViewChairID[i]=INVALID_CHAIR;
		m_bMingZhu[i]=false;
	}

	//״̬����
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

	return true;
}

//��Ϸ����
void CGameClientEngine::OnGameOptionSet()
{
	//��������
	CGameOption GameOption;
	GameOption.m_dwCardHSpace=m_dwCardHSpace;
	GameOption.m_bEnableSound=m_pGlobalUnits->m_bAllowSound;
	GameOption.m_bAllowLookon=m_pGlobalUnits->m_bAllowLookon;

	//��������
	if (GameOption.DoModal()==IDOK)
	{
		//�����˿�
		m_dwCardHSpace=GameOption.m_dwCardHSpace;
		for (int i=0;i<GAME_PLAYER;i++) m_GameClientView.m_CardControl[i].SetCardSpace(m_dwCardHSpace);

		//��������
		m_pGlobalUnits->m_bAllowSound=GameOption.m_bEnableSound;
		m_pGlobalUnits->m_bAllowLookon=GameOption.m_bAllowLookon;

		//��������
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
	}

	return;
}

//ʱ��ɾ��
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
	return true;
}
//ʱ����Ϣ 
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
	switch (wClockID)
	{
	case IDI_START_GAME:		//��ʼ��ʱ��
		{
			//��ֹ�ж�
			if (nElapse==0)
			{
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return false;
			}

			//����֪ͨ
			if ((nElapse<=5)&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_USER_ADD_SCORE:	//��ע��ʱ��
		{
			WORD wMeChairID=GetMeChairID();

			//��ֹ�ж�
			if (nElapse==0)
			{
				//������ע
				if ((IsLookonMode()==false)&&(wMeChairID==m_wCurrentUser)&&m_wCurrentUser!=INVALID_CHAIR)
				{
					//�Զ�����
					OnGiveUp(0,0);
				}
				return false;
			}

			//����֪ͨ
			if ((nElapse<=5)&&(wMeChairID==m_wCurrentUser)&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
				return true;
			}

			return true;
		}
	case IDI_DISABLE:			//���˶�ʱ��
		{
			if (nElapse==0)
			{
				//ɾ����ʱ��
				KillGameClock(IDI_DISABLE);
			}
			return true;
		}
	case IDI_USER_COMPARE_CARD:	//ѡ��ʱ��
		{
			//��ֹ�ж�
			if (nElapse==0)
			{
				//ɾ����ʱ��
				KillGameClock(IDI_USER_COMPARE_CARD);
				WORD wMeChairID=GetMeChairID();

				//�������
				m_GameClientView.SetCompareCard(false,NULL);

				//�������
				CMD_C_CompareCard CompareCard;
				ZeroMemory(&CompareCard,sizeof(CompareCard));

				//�����ϼ�
				for (LONGLONG i=wMeChairID-1;;i--)
				{
					if(i==-1)i=GAME_PLAYER-1;
					if(m_cbPlayStatus[i]==TRUE)
					{
						CompareCard.wCompareUser=(WORD)i;
						break;
					}
				}

				//������Ϣ
				SendSocketData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CompareCard));

				return false;
			}

			//����֪ͨ
			if ((nElapse<=5)&&(GetMeChairID()==m_wCurrentUser)&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
				return true;
			}

			return true;
		}
	}

	return false;
}

//�Թ�״̬
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	//�ؼ�����
	////if(bLookonUser)
	//{
	//	if(IsAllowLookon() 
	//		&& m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].GetLookCard())
	//		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(true);
	//	else m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(false);
	//}
	return true;
}

//������Ϣ
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			//��Ϣ����
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_ADD_SCORE:		//�û���ע
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//��Ϣ����
			return OnSubAddScore(pData,wDataSize);
		}
	case SUB_S_LOOK_CARD:		//������Ϣ
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//��Ϣ����
			return OnSubLookCard(pData,wDataSize);
		}
	case SUB_S_COMPARE_CARD:	//������Ϣ
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//��Ϣ����
			return OnSubCompareCard(pData,wDataSize);
		}
	case SUB_S_OPEN_CARD:		//������Ϣ
		{
			//m_GameClientView.StopMoveJettons();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//��Ϣ����
			return OnSubOpenCard(pData,wDataSize);
		}
	case SUB_S_GIVE_UP:			//�û�����
		{
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//��Ϣ����
			return OnSubGiveUp(pData,wDataSize);
		}
	case SUB_S_PLAYER_EXIT:		//�û�ǿ��
		{
			m_GameClientView.FinishDispatchCard();
			//��Ϣ����
			return OnSubPlayerExit(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//��Ϸ����
		{
			m_GameClientView.StopMoveJettons();
			m_GameClientView.FinishDispatchCard();
			m_GameClientView.StopCompareCard();
			m_GameClientView.StopFlashCard();

			//��Ϣ����
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_WAIT_COMPARE:	//�ȴ�����
		{
			//��Ϣ����
			if (wDataSize!=sizeof(CMD_S_WaitCompare)) return false;
			CMD_S_WaitCompare * pWaitCompare=(CMD_S_WaitCompare *)pData;	
			ASSERT(pWaitCompare->wCompareUser==m_wCurrentUser);

			WORD wMeChairID=GetMeChairID();
			if(wMeChairID!=m_wCurrentUser)
			{
				//���Ʊ���
			//	m_GameClientView.m_CardControl[m_wViewChairID[m_wCurrentUser]].SetCompareBack(true);

				SetGameClock(m_wCurrentUser,IDI_DISABLE,TIME_USER_COMPARE_CARD);
			}

			//��ʾ��־
			if(wMeChairID!=m_wCurrentUser || IsLookonMode()) m_GameClientView.SetWaitUserChoice(TRUE);

			return true;
		}
	case SUB_S_SEND_CARD:		//�û��˿�
		{
			return true;
		}
	case SUB_S_CHEAT_CARD:		//������Ϣ
		{
			//��ϢЧ��
			ASSERT(wDataSize==sizeof(CMD_S_CheatCardInfo));
			if (wDataSize!=sizeof(CMD_S_CheatCardInfo)) return false;

			CMD_S_CheatCardInfo  *cheatCard = (CMD_S_CheatCardInfo *)pData;

			//���ױ��
			m_bCheatUser = true;
			m_GameClientView.m_bCheatUser = true;

			//�����˿�
			CopyMemory(m_cbHandCardData,cheatCard->cbAllHandCardData,sizeof(m_cbHandCardData));
			

			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				IClientUserItem *pUserInfo = GetTableUserItem(i);
				m_GameLogic.SortCardList(m_cbHandCardData[i],3);
				if (NULL != pUserInfo)
				{
					m_GameClientView.m_CardControl[SwitchViewChairID(i)].SetCardData(m_cbHandCardData[i],MAX_COUNT);
					m_GameClientView.m_CardControl[SwitchViewChairID(i)].SetDisplayHead(true);
					m_GameClientView.m_CardControl[SwitchViewChairID(i)].SetCardStatus(0);
				}

			}

			return true;
		}
	case SUB_S_ADMIN_STORAGE_INFO:	//����ͻ�����Ϣ
		{
			ASSERT(wDataSize == sizeof(CMD_S_ADMIN_STORAGE_INFO));
			if(wDataSize != sizeof(CMD_S_ADMIN_STORAGE_INFO))
			{
				return false;
			}
			//�ж�Ȩ��
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				||NULL == m_GameClientView.m_hInst)
			{
				return true;
			}

			CMD_S_ADMIN_STORAGE_INFO *pStorage = (CMD_S_ADMIN_STORAGE_INFO *)pData;
			m_GameClientView.m_pClientControlDlg->SetStorageInfo(pStorage);
			return true;
		}
	case SUB_S_REQUEST_QUERY_RESULT:
		{
			ASSERT(wDataSize == sizeof(CMD_S_RequestQueryResult));
			if(wDataSize != sizeof(CMD_S_RequestQueryResult))
			{
				return false;
			}
			//�ж�Ȩ��
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				||NULL == m_GameClientView.m_hInst)
			{
				return true;
			}

			CMD_S_RequestQueryResult *pQueryResult = (CMD_S_RequestQueryResult *)pData;
			m_GameClientView.m_pClientControlDlg->RequestQueryResult(pQueryResult);

			return true;
		}
	case SUB_S_USER_CONTROL:
		{
			ASSERT(wDataSize == sizeof(CMD_S_UserControl));
			if(wDataSize != sizeof(CMD_S_UserControl))
			{
				return false;
			}
			//�ж�Ȩ��
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				||NULL == m_GameClientView.m_hInst)
			{
				return true;
			}

			CMD_S_UserControl *pUserControl = (CMD_S_UserControl *)pData;
			m_GameClientView.m_pClientControlDlg->RoomUserControlResult(pUserControl);

			return true;
		}
	case SUB_S_USER_CONTROL_COMPLETE:
		{
			ASSERT(wDataSize == sizeof(CMD_S_UserControlComplete));
			if(wDataSize != sizeof(CMD_S_UserControlComplete))
			{
				return false;
			}
			//�ж�Ȩ��
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				||NULL == m_GameClientView.m_hInst)
			{
				return true;
			}

			CMD_S_UserControlComplete *pUserControlComplete = (CMD_S_UserControlComplete *)pData;
			m_GameClientView.m_pClientControlDlg->UserControlComplete(pUserControlComplete);

			return true;
		}
	case SUB_S_OPERATION_RECORD:
		{
			ASSERT(wDataSize == sizeof(CMD_S_Operation_Record));
			if(wDataSize != sizeof(CMD_S_Operation_Record))
			{
				return false;
			}
			//�ж�Ȩ��
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				||NULL == m_GameClientView.m_hInst)
			{
				return true;
			}

			CMD_S_Operation_Record *pOperation_Record = (CMD_S_Operation_Record *)pData;
			m_GameClientView.m_pClientControlDlg->UpdateOperationRecord(pOperation_Record);

			return true;
		}
	case SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT:
		{
			ASSERT(wDataSize == sizeof(CMD_S_RequestUpdateRoomInfo_Result));
			if(wDataSize != sizeof(CMD_S_RequestUpdateRoomInfo_Result))
			{
				return false;
			}
			//�ж�Ȩ��
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
				||NULL == m_GameClientView.m_hInst)
			{
				return true;
			}

			CMD_S_RequestUpdateRoomInfo_Result *pRoomInfo_Result = (CMD_S_RequestUpdateRoomInfo_Result *)pData;
			m_GameClientView.m_pClientControlDlg->UpdateRoomInfoResult(pRoomInfo_Result);

			return true;
		}
	case SUB_S_UPDATEAESKEY:
		{
			return OnSubUpdateAESKey(pData,wDataSize);
		}
	case SUB_S_RC_TREASEURE_DEFICIENCY:
		{
			return true;
		}
	}

	return false;
}

//��Ϸ����
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//����״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			//���ÿؼ�
			if (IsLookonMode()==false&& GetMeUserItem()->GetUserStatus()!=US_READY)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
				SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
			}
			//������Դ
			for(int i =0; i < GAME_PLAYER; ++i)
			{
				IClientUserItem* pIClientUserItem = GetTableUserItem(i);
				if(pIClientUserItem == NULL) continue;

				WORD wViewChair = SwitchViewChairID(pIClientUserItem->GetChairID());
				BYTE cbGender = pIClientUserItem->GetGender();
				m_GameClientView.LoadUserImage(wViewChair,cbGender);
			}
			
			if (!bLookonUser)
			{
				//������Կ
				CopyMemory(m_chUserAESKey, pStatusFree->chUserAESKey, sizeof(m_chUserAESKey));
			}

			if (CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_hInst)
			{
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

				//���ø��¶�ʱ��
				m_GameClientView.m_pClientControlDlg->SetUpdateIDI();

				//���÷�����
				m_GameClientView.m_pClientControlDlg->SetRoomStorage(pStatusFree->lRoomStorageStart, pStatusFree->lRoomStorageCurrent);
			}

			return true;
		}
	case GAME_STATUS_PLAY:	//��Ϸ״̬
		{
			for (WORD i=0;i<GAME_PLAYER;i++)m_wViewChairID[i]=SwitchViewChairID(i);

			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;
			WORD wMeChairID=GetMeChairID();

			//��ע��Ϣ
			m_lMaxCellScore=pStatusPlay->lMaxCellScore;
			m_lCellScore=pStatusPlay->lCellScore;
			m_lCurrentTimes=pStatusPlay->lCurrentTimes;
			m_lUserMaxScore=pStatusPlay->lUserMaxScore;
//			m_GameClientView.m_GoldControl.SetGoldCount(m_lCellScore);

			//���ñ���
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			CopyMemory(m_bMingZhu,pStatusPlay->bMingZhu,sizeof(pStatusPlay->bMingZhu));
			CopyMemory(m_lTableScore,pStatusPlay->lTableScore,sizeof(pStatusPlay->lTableScore));
			CopyMemory(m_cbPlayStatus,pStatusPlay->cbPlayStatus,sizeof(pStatusPlay->cbPlayStatus));

			//�����˿�
			CopyMemory(&m_cbHandCardData[wMeChairID],&pStatusPlay->cbHandCardData,MAX_COUNT);

			//���ñ���
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.m_cbPlayStatus[m_wViewChairID[i]]=m_cbPlayStatus[i];
				m_GameClientView.m_CardControl[i].SetDisplayHead(false);
				if(m_lTableScore[i]>0L)m_GameClientView.SetUserTableScore(m_wViewChairID[i],m_lTableScore[i],m_lTableScore[i]);

				//��ȡ�û�
				IClientUserItem * pClientUserItem=GetTableUserItem(i);
				if (pClientUserItem==NULL) continue;

				//Ч��״̬
				//if(pUserData->cbUserStatus==GAME_STATUS_PLAY) ASSERT(m_cbPlayStatus[i]==TRUE);
				//����Ч��,����:�ڽ���ʱ,Ӯ�Ҷ������½���,״̬SIT

				//�û�����
				lstrcpyn(m_szAccounts[i],pClientUserItem->GetNickName(),CountArray(m_szAccounts[i]));
				m_strAccounts[i] = pClientUserItem->GetNickName();
			}

			//ֹͣ���붯��
			m_GameClientView.StopMoveJettons();

			//ׯ�ұ�־
			WORD wID=m_wViewChairID[m_wBankerUser];
			m_GameClientView.SetBankerUser(wID);

			//������Ϣ
			m_GameClientView.SetScoreInfo(m_lMaxCellScore,m_lCellScore);

			//���ý���
			//IClientUserItem * pClientUserItem = GetMeUserItem();
			//pClientUserItem->Get
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//����λ��
				WORD wViewChairID=m_wViewChairID[i];

				IClientUserItem * pClientUserItem = GetTableUserItem(i);
				if(pClientUserItem == NULL) continue;

				//�����˿�
				if((i == wMeChairID /*&& m_cbHandCardData[wMeChairID][0]!=0*/) || i!=wMeChairID)
						m_GameClientView.m_CardControl[wViewChairID].SetCardData(m_cbHandCardData[i],MAX_COUNT);

				if (m_cbPlayStatus[i]==TRUE) 
				{
					//m_GameClientView.m_CardControl[wViewChairID].SetCardData(m_cbHandCardData[i],MAX_COUNT);
					if(m_bMingZhu[i])
					{
						//if(i!=wMeChairID)	//���Լ�����
						{
							m_GameClientView.m_CardControl[wViewChairID].SetCardStatus(CARD_KANPAI);
							m_GameClientView.m_CardControl[wViewChairID].SetCardType(0);
						}
						//else				//�Լ�����
						//	m_GameClientView.m_CardControl[wViewChairID].SetDisplayHead(true);
					}
					else	//�����Ʊ���
						m_GameClientView.m_CardControl[wViewChairID].SetCardColor(INVALID_CHAIR);
				}
				else
					m_GameClientView.m_CardControl[wViewChairID].SetCardColor(1);
			}
			//������Դ
			for(int i =0; i < GAME_PLAYER; ++i)
			{
				IClientUserItem* pIClientUserItem = GetTableUserItem(i);
				if(pIClientUserItem == NULL) continue;

				WORD wViewChair = SwitchViewChairID(pIClientUserItem->GetChairID());
				BYTE cbGender = pIClientUserItem->GetGender();
				m_GameClientView.LoadUserImage(wViewChair,cbGender);
			}

			if(m_bMingZhu[wMeChairID] && (!IsLookonMode() || IsAllowLookon()))
			{
				m_GameClientView.m_CardControl[m_wViewChairID[wMeChairID]].SetDisplayHead(true);
			}

			//�ؼ�����
			m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

			//�жϿؼ�
			if(m_wCurrentUser<GAME_PLAYER)
			{
				if(!(pStatusPlay->bCompareState))
				{
					//�ؼ���Ϣ
					if(!IsLookonMode() && wMeChairID==m_wCurrentUser)
					{
						UpdataControl();
					}

					//����ʱ��
					SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
				}
				else
				{	
					if(!IsLookonMode() && wMeChairID==m_wCurrentUser)
					{
						//ѡ����ұ���
						BOOL bCompareUser[GAME_PLAYER];
						for(int i=0;i<GAME_PLAYER;i++)
						{
							if(m_cbPlayStatus[i]==TRUE && wMeChairID!=i)
							{
								bCompareUser[m_wViewChairID[i]]=TRUE;
							}
							else bCompareUser[m_wViewChairID[i]]=FALSE;
						}
						//���ü�ͷ
						m_GameClientView.SetCompareCard(true,bCompareUser);

						//�ȴ�����
						SendSocketData(SUB_C_WAIT_COMPARE,NULL,0);

						//����ʱ��
						SetGameClock(wMeChairID,IDI_USER_COMPARE_CARD,TIME_USER_COMPARE_CARD);

						//��ʾ��־
						m_GameClientView.SetWaitUserChoice(FALSE);
					}
					else 
					{
						//���Ʊ���
						m_GameClientView.m_CardControl[m_wViewChairID[m_wCurrentUser]].SetCompareBack(false);

						SetGameClock(m_wCurrentUser,IDI_DISABLE,TIME_USER_COMPARE_CARD);

						//��ʾ��־
						m_GameClientView.SetWaitUserChoice(TRUE);
					}
				}
			}
			
			if (!bLookonUser)
			{
				//������Կ
				CopyMemory(m_chUserAESKey, pStatusPlay->chUserAESKey, sizeof(m_chUserAESKey));
			}

			if (CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_hInst)
			{
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

				//���ø��¶�ʱ��
				m_GameClientView.m_pClientControlDlg->SetUpdateIDI();

				//���÷�����
				m_GameClientView.m_pClientControlDlg->SetRoomStorage(pStatusPlay->lRoomStorageStart, pStatusPlay->lRoomStorageCurrent);
			}

			return true;
		}
	}

	return false;
}

//��Ϸ��ʼ
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//�Թ۽���
	if(IsLookonMode())OnStart(0,0);

	////ֹͣ����
	m_GameClientView.StopMoveJettons();
	m_GameClientView.StopMoveNumber();

	//���ý���
	m_wBankerUser=INVALID_CHAIR;
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetScoreInfo(0,0);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L,0L);

	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_bMingZhu[i]=false;
		m_GameClientView.m_NumberControl[i].ResetControl();
		m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_CardControl[i].SetDisplayHead(false);
		m_GameClientView.m_CardControl[i].SetCardColor(INVALID_CHAIR);
		m_GameClientView.m_CardControl[i].SetLookCard(false);
		m_GameClientView.m_CardControl[i].SetCardStatus(0);
		m_GameClientView.m_CardControl[i].SetCompareCard(false);
		m_GameClientView.m_CardControl[i].SetCardType(0);
	}
	m_GameClientView.m_JettonControl.ResetControl();

	//״̬����
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//��ע��Ϣ
	m_lMaxCellScore=0L;
	m_lCellScore=0L;


	//��Ϸ����
	m_cbGameCount++;

	//������Ϣ
	m_lCellScore=pGameStart->lCellScore;
	m_lMaxCellScore=pGameStart->lMaxScore;
	m_lCurrentTimes=pGameStart->lCurrentTimes;
	m_wCurrentUser=pGameStart->wCurrentUser;
	m_wBankerUser=pGameStart->wBankerUser;
	m_lUserMaxScore=pGameStart->lUserMaxScore;
//	m_GameClientView.m_GoldControl.SetGoldCount(m_lCellScore);

	//���ñ���
	LONGLONG lTableScore=0L;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_wLostUserID[i]=INVALID_CHAIR;
		m_wViewChairID[i]=SwitchViewChairID(i);
		m_GameClientView.m_cbPlayStatus[m_wViewChairID[i]]=FALSE;

		//��ȡ�û�
		IClientUserItem * pClientUserItem=GetTableUserItem(i);
		if (pClientUserItem==NULL) continue;

		//��Ϸ��Ϣ		
		m_cbPlayStatus[i]=TRUE;
		m_GameClientView.m_cbPlayStatus[m_wViewChairID[i]]=TRUE;
		lTableScore+=m_lCellScore;
		m_lTableScore[i]=m_lCellScore;
		m_GameClientView.SetUserTableScore(m_wViewChairID[i],m_lTableScore[i],m_lTableScore[i]);

		//�û���Ϣ
		lstrcpyn(m_szAccounts[i],pClientUserItem->GetNickName(),CountArray(m_szAccounts[i]));
		m_strAccounts[i] = pClientUserItem->GetNickName();
	}
	//m_GameClientView.BeginMoveJettons();
	m_GameClientView.StopMoveJettons();

	//��������
	m_GameClientView.SetBankerUser(m_wViewChairID[m_wBankerUser]);
//	AfxMessageBox(TEXT("..."),0,0);
	m_GameClientView.SetScoreInfo(m_lMaxCellScore,m_lCellScore);

	//�ɷ��˿�
	for(WORD i=0;i<MAX_COUNT;i++)
	{
		for (WORD j=m_wBankerUser;j<m_wBankerUser+GAME_PLAYER;j++)
		{
			WORD w=j%GAME_PLAYER;
			if (m_cbPlayStatus[w]==TRUE)
			{
				m_GameClientView.DispatchUserCard(m_wViewChairID[w],0);
			}
		}
	}

	//����Ч��
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//�û�����
bool CGameClientEngine::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize!=sizeof(CMD_S_GiveUp)) return false;
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	//ɾ����ʱ��
	if(IsLookonMode() || pGiveUp->wGiveUpUser!=GetMeChairID())KillGameClock(IDI_USER_ADD_SCORE);	

	//���ñ���
	m_cbPlayStatus[pGiveUp->wGiveUpUser]=FALSE;
	m_GameClientView.m_cbPlayStatus[m_wViewChairID[pGiveUp->wGiveUpUser]]=FALSE;

	//��������
	WORD wGiveUpUser=pGiveUp->wGiveUpUser;
	WORD wViewChairID=m_wViewChairID[wGiveUpUser];

	//�˿˱��
	if (!m_bCheatUser)
	{
		m_GameClientView.m_CardControl[wViewChairID].SetCardColor(1);
	}
	m_GameClientView.m_CardControl[wViewChairID].SetCardStatus(5);

	//�������ñ�ѡ�����û�
	if(m_wCurrentUser==GetMeChairID() && m_GameClientView.GetCompareInfo())
	{
		BOOL bCompareUser[GAME_PLAYER];
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i]==TRUE && GetMeChairID()!=i)
			{
				bCompareUser[m_wViewChairID[i]]=TRUE;
			}
			else bCompareUser[m_wViewChairID[i]]=FALSE;
		}

		//���ü�ͷ
		m_GameClientView.SetCompareCard(true,bCompareUser);
	}

	//״̬����
	if ((IsLookonMode()==false)&&(pGiveUp->wGiveUpUser==GetMeChairID())) SetGameStatus(GAME_STATUS_FREE);

	//��������
	if ((IsLookonMode()==true)||(wGiveUpUser!=GetMeChairID())) PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	return true;
}

//�û���ע
bool CGameClientEngine::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_AddScore));
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;
	ASSERT(pAddScore->wCurrentUser < GAME_PLAYER);

	//ɾ����ʱ��
	if(IsLookonMode() || pAddScore->wAddScoreUser!=GetMeChairID()) KillGameClock(IDI_USER_ADD_SCORE);

	//��������
	WORD wMeChairID=GetMeChairID();
	WORD wAddScoreUser=pAddScore->wAddScoreUser;
	WORD wViewChairID=m_wViewChairID[wAddScoreUser];
	m_lCurrentTimes=pAddScore->lCurrentTimes;
	ASSERT(m_lCurrentTimes<=m_lMaxCellScore/m_lCellScore);

	//��ע����
	if ((IsLookonMode()==true)||(wAddScoreUser!=wMeChairID))
	{
		//��ע���
		m_lTableScore[pAddScore->wAddScoreUser]+=pAddScore->lAddScoreCount;

		//��ע����
		m_GameClientView.SetUserTableScore(wViewChairID,m_lTableScore[pAddScore->wAddScoreUser],pAddScore->lAddScoreCount);
		m_GameClientView.BeginMoveJettons();

		//��������
		if (m_cbPlayStatus[wAddScoreUser]==TRUE)
		{
			 PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
		}
	}

	//��ǰ�û�
	m_wCurrentUser=pAddScore->wCurrentUser;

	//�ؼ���Ϣ
	if(pAddScore->wCompareState==FALSE && (!IsLookonMode()) && wMeChairID==m_wCurrentUser)
	{
		UpdataControl();
	}

	//����ʱ��
	if(pAddScore->wCompareState==FALSE)
	{//if(IsLookonMode() || wMeChairID!=m_wCurrentUser)
		SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
	}

	return true;
}

//�û�����
bool CGameClientEngine::OnSubLookCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize!=sizeof(CMD_S_LookCard)) return false;
	CMD_S_LookCard * pLookCard=(CMD_S_LookCard *)pBuffer;

	//���¶�ʱ
	if(IsLookonMode() || m_wCurrentUser!=GetMeChairID())
	{
		////ɾ����ʱ
		//KillGameClock(IDI_USER_ADD_SCORE);
		//SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
	}
	else
	{
		//�����˿�
		CopyMemory(m_cbHandCardData[GetMeChairID()],pLookCard->cbCardData,sizeof(pLookCard->cbCardData));
		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetCardData(m_cbHandCardData[GetMeChairID()],MAX_COUNT);
		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(true);
	}

	//���ý���
	WORD wId=pLookCard->wLookCardUser;
	if(GetMeChairID()!=wId || IsLookonMode())
	{
		m_GameClientView.m_CardControl[m_wViewChairID[wId]].SetCardStatus(CARD_KANPAI);
	}

	//�����Թ�
	if(IsLookonMode() && IsAllowLookon() && m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].GetLookCard())
	{
		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetDisplayHead(true);
	}

	//��������
	if ((IsLookonMode()==true)||(wId!=GetMeChairID())) PlayGameSound(AfxGetInstanceHandle(),TEXT("CENTER_SEND_CARD"));

	return true;
}

//�û�����
bool CGameClientEngine::OnSubCompareCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize!=sizeof(CMD_S_CompareCard)) return false;
	CMD_S_CompareCard * pCompareCard=(CMD_S_CompareCard *)pBuffer;

	//ɾ��ʱ��
	KillGameClock(IDI_DISABLE);

	//ȡ���ȴ�
	m_GameClientView. SetWaitUserChoice(INVALID_CHAIR);

	//�����û�
	m_wLostUser=pCompareCard->wLostUser;
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_wLostUserID[i]==INVALID_CHAIR)			
		{
			m_wLostUserID[i] = m_wLostUser;
			break;
		}
	}

	//���ñ���
	m_cbPlayStatus[m_wLostUser]=FALSE;
	m_GameClientView.m_cbPlayStatus[m_wViewChairID[m_wLostUser]]=FALSE;

	//״̬����
	if ((IsLookonMode()==false)&&(m_wLostUser==GetMeChairID())) SetGameStatus(GAME_STATUS_FREE);

	//��ǰ�û�
	m_wCurrentUser=pCompareCard->wCurrentUser;

	//ȡ�����Ʊ���
	WORD wViewChair1=m_wViewChairID[pCompareCard->wCompareUser[0]];
	WORD wViewChair2=m_wViewChairID[pCompareCard->wCompareUser[1]];
	m_GameClientView.m_CardControl[wViewChair1].SetCompareBack(false);
	m_GameClientView.m_CardControl[wViewChair2].SetCompareBack(false);

	//��Ϸ����
	if(m_wCurrentUser==INVALID_CHAIR)m_GameClientView.StopUpdataScore(true);

	//���˶���Ч��
	//OnFlashCardFinish(0,0);

	//���ؿؼ�
	ScoreControl(SW_HIDE);
	WORD wFalshUser[]={wViewChair1,wViewChair2};

	//����Ч��
	m_GameClientView.PerformCompareCard(wFalshUser,m_wViewChairID[m_wLostUser]);

	return true;
}

//�û�����
bool CGameClientEngine::OnSubOpenCard(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize!=sizeof(CMD_S_OpenCard)) return false;
	CMD_S_OpenCard* pOpenCard=(CMD_S_OpenCard *)pBuffer;

	//ȡ���ȴ�
	m_GameClientView.SetWaitUserChoice(INVALID_CHAIR);

	//ʤ���û�
	m_wWinnerUser=pOpenCard->wWinner;

	//���ؿؼ�
	ScoreControl(SW_HIDE);
//	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);

	//��Ϸ����
	m_GameClientView.StopUpdataScore(true);

	//���˶���Ч��
	//OnFlashCardFinish(0,0);

	//����Ч��
	WORD wFalshUser[GAME_PLAYER];
	for (LONGLONG i=0;i<GAME_PLAYER;i++)if(m_cbPlayStatus[i]==TRUE)wFalshUser[i]=m_wViewChairID[i];
	m_GameClientView.bFalshCard(wFalshUser);

	return true;
}

//�û�ǿ��
bool CGameClientEngine::OnSubPlayerExit(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize!=sizeof(CMD_S_PlayerExit)) return false;
	CMD_S_PlayerExit * pPlayerExit=(CMD_S_PlayerExit *)pBuffer;

	WORD wID=pPlayerExit->wPlayerID;

	//��Ϸ��Ϣ
	ASSERT(m_cbPlayStatus[wID]==TRUE);
	m_cbPlayStatus[wID]=FALSE;
	m_GameClientView.m_cbPlayStatus[m_wViewChairID[wID]]=FALSE;

	//�û�����
	for(WORD i=0;i<32;i++)m_szAccounts[wID][i]=0;

	return true;
}

//��Ϸ����
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;
	WORD wMeChiar=GetMeChairID();

	//ɾ����ʱ��
	KillGameClock(IDI_USER_ADD_SCORE);

	//3�̻�һ�γ���
	if(m_cbGameCount == 3)
	{
		m_GameClientView.SwitchSence();
		m_cbGameCount = 0;
	}

	//����ؼ�
	AddScoreControl(SW_HIDE);
	ScoreControl(SW_HIDE);
	m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	m_GameClientView.m_btFollow.EnableWindow(FALSE);


	//��������
	LONGLONG lTax = m_lCellScore*pGameEnd->lGameTax/1000;
	WORD wWinner=INVALID_CHAIR;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//������Ϣ
		if (pGameEnd->lGameScore[i]!=0L)
		{
			if(pGameEnd->lGameScore[i]>0L) 
			{
				wWinner = i;
			}
			m_GameClientView.m_ScoreView.SetGameTax(lTax,i);
			if (m_szAccounts[i][0]==0) m_GameClientView.m_ScoreView.SetGameScore(i,TEXT("���뿪"),pGameEnd->lGameScore[i]);
			else m_GameClientView.m_ScoreView.SetGameScore(i,m_szAccounts[i],pGameEnd->lGameScore[i]);
		}
	}
	//m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//����Ч��
	m_GameClientView.StopUpdataScore(false);

	//�����뱻�����û�������������
	for (WORD j=0;j<4;j++)
	{
		WORD wUserID=pGameEnd->wCompareUser[wMeChiar][j];
		if(wUserID==INVALID_CHAIR)continue;

		WORD wViewChiar=m_wViewChairID[wUserID];
		m_GameClientView.m_CardControl[wViewChiar].SetDisplayHead(true);
		m_GameClientView.m_CardControl[wViewChiar].SetLookCard(false);
		m_GameClientView.m_CardControl[wViewChiar].SetCardColor(INVALID_CHAIR);
		m_GameClientView.m_CardControl[wViewChiar].SetCardData(pGameEnd->cbCardData[wUserID],MAX_COUNT);
	}

	//�Լ��˿�
	if(pGameEnd->wCompareUser[wMeChiar][0]!=INVALID_CHAIR || m_bMingZhu[wMeChiar])
	{
		WORD wViewChiar=m_wViewChairID[wMeChiar];
		m_GameClientView.m_CardControl[wViewChiar].SetCardData(pGameEnd->cbCardData[wMeChiar],MAX_COUNT);
		m_GameClientView.m_CardControl[wViewChiar].SetCardColor(INVALID_CHAIR);
		m_GameClientView.m_CardControl[wViewChiar].SetDisplayHead(true);
	}

	////���˶�̬�����û��˿�
	//IClientUserItem * pClientUserItem=GetTableUserItem(wMeChiar);
	//BYTE a= GetGameStatus();
	//BYTE B=pClientUserItem->GetUserStatus();
	//if(pClientUserItem!=NULL && GetGameStatus()!=GAME_STATUS_PLAY && pClientUserItem->GetUserStatus()!=US_OFFLINE)
	//{
	//	WORD wViewChiar=m_wViewChairID[wMeChiar];
	//	m_GameClientView.m_CardControl[wViewChiar].SetCardData(NULL,0);
	//}

	//���ƽ���
	if(pGameEnd->wEndState==1)
	{
		//���б����û�
		if(m_cbPlayStatus[wMeChiar]==TRUE)
		{
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==TRUE)
				{
					WORD wViewChiar=m_wViewChairID[i];
					m_GameClientView.m_CardControl[wViewChiar].SetLookCard(false);
					m_GameClientView.m_CardControl[wViewChiar].SetCardColor(INVALID_CHAIR);
					m_GameClientView.m_CardControl[wViewChiar].SetDisplayHead(true);
					m_GameClientView.m_CardControl[wViewChiar].SetCardData(pGameEnd->cbCardData[i],MAX_COUNT);
				}
			}
		}
	}

	//���ֹ�������
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( pGameEnd->lGameScore[i] == 0 ) continue;
		WORD wViewId = m_wViewChairID[i];	
		m_GameClientView.m_NumberControl[wViewId].SetScore( pGameEnd->lGameScore[i] );
	}
	m_GameClientView.BeginMoveNumber();

	//����
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		//���ǻ���
		if( pGameEnd->lGameScore[i] == 0 ) continue;

		BYTE cbCardData[MAX_COUNT];
		WORD wViewId = m_wViewChairID[i];
		CopyMemory(cbCardData, pGameEnd->cbCardData[i], sizeof(BYTE)*MAX_COUNT);
		m_GameLogic.SortCardList(cbCardData, MAX_COUNT);
		m_GameClientView.m_CardControl[wViewId].SetCardType(m_GameLogic.GetCardType(cbCardData,MAX_COUNT));
	}

	//���ճ���
	if(wWinner<GAME_PLAYER)
		m_GameClientView.SetGameEndInfo(m_wViewChairID[wWinner]);

	//��������
	if (IsLookonMode()==false)
	{
		if (pGameEnd->lGameScore[GetMeChairID()]>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//��ʼ��ť
	if (IsLookonMode()==false)
	{
		tagServerAttribute *pServerAttribute=m_pIClientKernel->GetServerAttribute();
		bool bMatchMode=((pServerAttribute->wServerType&GAME_GENRE_MATCH)!=0);
		m_GameClientView.m_btStart.ShowWindow(bMatchMode?SW_HIDE:SW_SHOW);
		//		m_GameClientView.m_btStart.SetFocus();
		if(!bMatchMode)
			SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}

	//�ɼ���ʾ�ڼ�ʱ����Ի���
	TCHAR szBuffer[64];
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("���ֽ���,�ɼ�ͳ��:"));
	if(m_pIStringMessage != NULL)
		m_pIStringMessage->InsertSystemString(szBuffer);

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//��������
		if (m_szAccounts[i][0]!=0)
		{
			_sntprintf(szBuffer,CountArray(szBuffer),TEXT("��%s��:%+I64d"),m_szAccounts[i],pGameEnd->lGameScore[i]);
			if(m_pIStringMessage != NULL)

				m_pIStringMessage->InsertNormalString(szBuffer);
			//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("�˿����ݣ�%I64d,%I64d,%I64d;��ɫ���ݣ�%I64d,%I64d,%I64d "),
			//	pGameEnd->cbCardData[i][0]&0x0f,pGameEnd->cbCardData[i][1]&0x0f,pGameEnd->cbCardData[i][2]&0x0f,
			//	(pGameEnd->cbCardData[i][0]&0xf0)>>4,(pGameEnd->cbCardData[i][1]&0xf0)>>4,
			//	(pGameEnd->cbCardData[i][2]&0xf0)>>4);
			//if(m_pIStringMessage != NULL)
			//	m_pIStringMessage->InsertSystemString(szBuffer);
		}
	}

	//״̬����
	m_GameClientView.SetWaitUserChoice(INVALID_CHAIR);
	SetGameStatus(GAME_STATUS_FREE);

	return true;
}

//������Կ
bool CGameClientEngine::OnSubUpdateAESKey(const void * pBuffer, WORD wDataSize)
{
	ASSERT (wDataSize == sizeof(CMD_S_UpdateAESKey));
	if (wDataSize != sizeof(CMD_S_UpdateAESKey))
	{
		return false;
	}

	ASSERT (!IsLookonMode());
	if (IsLookonMode())
	{
		return false;
	}

	CMD_S_UpdateAESKey *pUpdateAESKey = (CMD_S_UpdateAESKey*)pBuffer;

	CopyMemory(m_chUserAESKey, pUpdateAESKey->chUserUpdateAESKey, sizeof(m_chUserAESKey));

	return true;
}

//ת����Ϣ
void CGameClientEngine::ChangeUserInfo(BYTE bCardData[],BYTE bCardCount,CString &CardInfo)
{
	//ת����Ϣ
	for(BYTE i=0;i<bCardCount;i++)
	{
		//�˿˻�ɫ
		BYTE bCardColor = bCardData[i]&LOGIC_MASK_COLOR;
		switch(bCardColor)
		{
		case 0x00:
			{
				CardInfo+=TEXT("����");
			}
			break;
		case 0x10:
			{
				CardInfo+=TEXT("÷��");
			}
			break;
		case 0x20:
			{
				CardInfo+=TEXT("����");
			}
			break;
		case 0x30:
			{
				CardInfo+=TEXT("����");
			}
			break;
		}

		//�˿�����
		BYTE bTempCardData = bCardData[i]&LOGIC_MASK_VALUE;
		switch(bTempCardData)
		{
		case 0x01:CardInfo+=TEXT("��A��.");break;
		case 0x02:CardInfo+=TEXT("��2��.");break;
		case 0x03:CardInfo+=TEXT("��3��.");break;
		case 0x04:CardInfo+=TEXT("��4��.");break;
		case 0x05:CardInfo+=TEXT("��5��.");break;
		case 0x06:CardInfo+=TEXT("��6��.");break;
		case 0x07:CardInfo+=TEXT("��7��.");break;
		case 0x08:CardInfo+=TEXT("��8��.");break;
		case 0x09:CardInfo+=TEXT("��9��.");break;
		case 0x0a:CardInfo+=TEXT("��10��.");break;
		case 0x0b:CardInfo+=TEXT("��J��.");break;
		case 0x0c:CardInfo+=TEXT("��Q��.");break;
		case 0x0d:CardInfo+=TEXT("��K��.");break;
		}
	}

	return ;
}

//�������
void CGameClientEngine::ScoreControl(BOOL bShow)
{
//	m_GameClientView.m_btAddScore.ShowWindow(false);
	m_GameClientView.m_btCompareCard.ShowWindow(bShow);
	m_GameClientView.m_btGiveUp.ShowWindow(bShow);
	m_GameClientView.m_btLookCard.ShowWindow(bShow);
	m_GameClientView.m_btFollow.ShowWindow(bShow);
	m_GameClientView.m_btJetton_1.ShowWindow(bShow);
	m_GameClientView.m_btJetton_2.ShowWindow(bShow);
	m_GameClientView.m_btJetton_5.ShowWindow(bShow);

	if(m_lCellScore == 10)
	{
		m_GameClientView.m_btJetton_1.SetButtonImage(IDB_10,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_2.SetButtonImage(IDB_20,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_5.SetButtonImage(IDB_50,AfxGetInstanceHandle(),false,false);
	}
	else if(m_lCellScore == 100)
	{
		m_GameClientView.m_btJetton_1.SetButtonImage(IDB_100,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_2.SetButtonImage(IDB_200,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_5.SetButtonImage(IDB_500,AfxGetInstanceHandle(),false,false);
	}
	else if(m_lCellScore == 1000)
	{
		m_GameClientView.m_btJetton_1.SetButtonImage(IDB_1K,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_2.SetButtonImage(IDB_2K,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_5.SetButtonImage(IDB_5K,AfxGetInstanceHandle(),false,false);
	}
	else if(m_lCellScore == 10000)
	{
		m_GameClientView.m_btJetton_1.SetButtonImage(IDB_1W,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_2.SetButtonImage(IDB_2W,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_5.SetButtonImage(IDB_5W,AfxGetInstanceHandle(),false,false);
	}
	else if(m_lCellScore == 100000)
	{
		m_GameClientView.m_btJetton_1.SetButtonImage(IDB_10W,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_2.SetButtonImage(IDB_20W,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_5.SetButtonImage(IDB_50W,AfxGetInstanceHandle(),false,false);
	}
	else if(m_lCellScore == 1000000)
	{
		m_GameClientView.m_btJetton_1.SetButtonImage(IDB_100W,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_2.SetButtonImage(IDB_200W,AfxGetInstanceHandle(),false,false);
		m_GameClientView.m_btJetton_5.SetButtonImage(IDB_500W,AfxGetInstanceHandle(),false,false);
	}

	//�ؼ�����
	m_GameClientView.SetControlBack(bShow);

	return ;
}

//��ע����
void CGameClientEngine::AddScoreControl(BOOL bShow)
{
	return;
	//if(bShow)
	/*m_GameClientView.m_btMinScore.ShowWindow(bShow);
	m_GameClientView.m_btMaxScore.ShowWindow(bShow);
	m_GameClientView.m_btCancel.ShowWindow(bShow);
	m_GameClientView.m_btConfirm.ShowWindow(bShow);*/
//	m_GameClientView.m_GoldControl.ShowWindow(false);
	
//	m_GameClientView.m_GoldControl.m_bMingTag=m_bMingZhu[GetMeChairID()];

	m_GameClientView.RefreshGameView();

	return;
}

//�������
void CGameClientEngine::UpdataControl()
{
	ScoreControl(SW_SHOW);
	ActiveGameFrame();

	//������ť
	WORD wMeChairID = GetMeChairID();
	m_GameClientView.m_btGiveUp.EnableWindow(TRUE);

	

	//���ư�ť
	m_GameClientView.m_btLookCard.EnableWindow((m_bMingZhu[wMeChairID])?FALSE:TRUE);

	//�жϿ���
	LONGLONG lTemp=(m_bMingZhu[wMeChairID])?6:5;
	//��ǰ����
	BYTE UserCount=0;
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==TRUE)UserCount++;
	}
	//����עҪ�����б��Ƶĳ���
	if((m_lUserMaxScore-m_lTableScore[wMeChairID]) >= (m_lCellScore*(lTemp + m_lCurrentTimes)+(UserCount-1)*m_lCurrentTimes*m_lCellScore*4))
	{
		ScoreControl(SW_SHOW);
		//�����ϼ�
		for (LONGLONG j=wMeChairID-1;;j--)
		{
			if(j==-1)j=GAME_PLAYER-1;
			if(m_cbPlayStatus[j]==TRUE)break;
		}

		//��ע��ť
		m_GameClientView.m_btFollow.EnableWindow((m_lTableScore[j]==m_lCellScore)?FALSE:TRUE);

		//��ע��ť
//		m_GameClientView.m_btAddScore.EnableWindow(((m_lCurrentTimes*m_lCellScore) < m_lMaxCellScore)?FALSE:FALSE);
		if((m_lCurrentTimes*m_lCellScore) < m_lMaxCellScore)
		{

			m_GameClientView.m_btJetton_5.EnableWindow((m_lCurrentTimes<=5)?TRUE:FALSE);
			m_GameClientView.m_btJetton_2.EnableWindow((m_lCurrentTimes<=8)?TRUE:FALSE);
			m_GameClientView.m_btJetton_1.EnableWindow((m_lCurrentTimes<=9)?TRUE:FALSE);
			
		}
		else
		{
			
			m_GameClientView.m_btJetton_1.EnableWindow(FALSE);
			m_GameClientView.m_btJetton_2.EnableWindow(FALSE);
			m_GameClientView.m_btJetton_5.EnableWindow(FALSE);

		}

		//���ư�ť
		if(wMeChairID==m_wBankerUser || m_lTableScore[wMeChairID]>=2*m_lCellScore)
		{
			m_GameClientView.m_btCompareCard.EnableWindow(TRUE);
		}
		else m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	}
	else 
	{
		m_GameClientView.m_btCompareCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btGiveUp.ShowWindow(SW_SHOW);
		m_GameClientView.m_btLookCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btFollow.ShowWindow(SW_SHOW);
		m_GameClientView.m_btJetton_1.ShowWindow(SW_SHOW);
		m_GameClientView.m_btJetton_2.ShowWindow(SW_SHOW);
		m_GameClientView.m_btJetton_5.ShowWindow(SW_SHOW);

		m_GameClientView.m_btJetton_1.EnableWindow(FALSE);
		m_GameClientView.m_btJetton_2.EnableWindow(FALSE);
		m_GameClientView.m_btJetton_5.EnableWindow(FALSE);
		
		m_GameClientView.m_btFollow.EnableWindow(FALSE);
	//	m_GameClientView.m_btAddScore.EnableWindow(FALSE);
		m_GameClientView.m_btCompareCard.EnableWindow(TRUE);
		m_GameClientView.m_btLookCard.EnableWindow(TRUE);
	//	m_GameClientView.m_btOpenCard.ShowWindow(SW_SHOW);

	}

	return;
}

//��ʼ��ť
LRESULT	CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
	//ɾ����ʱ��
	if(!IsLookonMode())KillGameClock(IDI_START_GAME);

	//ֹͣ����
	m_GameClientView.StopMoveJettons();
	m_GameClientView.StopMoveNumber();

	//���ý���
	m_wBankerUser=INVALID_CHAIR;
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_GameClientView.SetScoreInfo(0,0);
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	m_GameClientView.SetUserTableScore(INVALID_CHAIR,0L,0L);

	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_bMingZhu[i]=false;
		m_GameClientView.m_NumberControl[i].ResetControl();
		m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_CardControl[i].SetDisplayHead(false);
		m_GameClientView.m_CardControl[i].SetCardColor(INVALID_CHAIR);
		m_GameClientView.m_CardControl[i].SetLookCard(false);
		m_GameClientView.m_CardControl[i].SetCardStatus(0);
		m_GameClientView.m_CardControl[i].SetCompareCard(false);
		m_GameClientView.m_CardControl[i].SetCardType(0);
	}
	m_GameClientView.m_JettonControl.ResetControl();

	//״̬����
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//��ע��Ϣ
	m_lMaxCellScore=0L;
	m_lCellScore=0L;

	//������Ϣ
	if(!IsLookonMode())SendUserReady(NULL,0);

	return 0;
}

//��ע��ť 
LRESULT	CGameClientEngine::OnAddScore(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_lCellScore>0L);

	//�жϼ�עֵ
	LONGLONG lAddScore;
	BYTE cbTemp= (BYTE)wParam;
	if(cbTemp == 0) lAddScore = m_lCellScore;
	if(cbTemp == 1) lAddScore = m_lCellScore*2;
	if(cbTemp == 2) lAddScore = m_lCellScore*5;

	OnConfirmScore(0,lAddScore);
	//����ؼ�
//	m_GameClientView.m_GoldControl.SetMaxGold(m_lMaxCellScore);
//	m_GameClientView.m_GoldControl.SetMinGold(m_lCellScore*m_lCurrentTimes);
//	m_GameClientView.m_GoldControl.SetGold(m_lCellScore*m_lCurrentTimes);
	AddScoreControl(SW_SHOW);
	ScoreControl(SW_HIDE);

	return 0;
}

//ȡ����Ϣ
LRESULT CGameClientEngine::OnCancelAdd(WPARAM wParam, LPARAM lParam)
{
	//����ؼ�
	AddScoreControl(SW_HIDE);
	ScoreControl(SW_SHOW);

	return 0;
}

//���ټ�ע
LRESULT	CGameClientEngine::OnMinScore(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_lCellScore>0L);
//	m_GameClientView.m_GoldControl.SetGold(m_lCellScore*m_lCurrentTimes);

	return 0;
}

//����ע
LRESULT	CGameClientEngine::OnMaxScore(WPARAM wParam, LPARAM lParam)
{
//	if(m_lMaxCellScore>0) m_GameClientView.m_GoldControl.SetGold(m_lMaxCellScore);

	return 0;
}

//��ע��Ϣ && ��ע��Ϣ
LRESULT CGameClientEngine::OnConfirmScore(WPARAM wParam, LPARAM lParam)
{
	//һΪ��ע
	WORD wTemp=LOWORD(wParam);
	LONGLONG lScore = (LONGLONG)lParam;

	//Ч������
	ASSERT(wTemp==1 || lScore+m_lCellScore*m_lCurrentTimes >= (m_lCellScore*m_lCurrentTimes)
		&& lScore+m_lCellScore*m_lCurrentTimes <= m_lMaxCellScore);

	//ɾ��ʱ��
	KillGameClock(IDI_USER_ADD_SCORE);

	//��ȡ����
//	LONGLONG lCurrentScore = (wTemp==0) ?(m_GameClientView.m_GoldControl.GetGold()) : (m_lCellScore*m_lCurrentTimes);

	
	LONGLONG lCurrentScore = (wTemp==0) ?(lScore+m_lCellScore*m_lCurrentTimes) : (m_lCellScore*m_lCurrentTimes);

	//��ע�ӱ�
	WORD wMeChairID=GetMeChairID();
	if(m_bMingZhu[wMeChairID]) lCurrentScore*=2;

	//Ԥ�ȴ���
	m_lTableScore[wMeChairID]+=lCurrentScore;
	m_GameClientView.SetUserTableScore(m_wViewChairID[wMeChairID],m_lTableScore[wMeChairID],lCurrentScore);
	m_GameClientView.BeginMoveJettons();

	//����ť
	AddScoreControl(SW_HIDE);

//	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
//	m_GameClientView.m_btAddScore.SetButtonImage(IDB_USERCONTROL_INVEST,AfxGetInstanceHandle(),false,false);
	ScoreControl(SW_HIDE);
//	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
//	m_GameClientView.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	m_GameClientView.m_btFollow.EnableWindow(FALSE);

	//��������
	m_GameClientView.RefreshGameView();
	PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

	//������Ϣ
	CMD_C_AddScore AddScore;
	ZeroMemory(&AddScore, sizeof(AddScore));
	AddScore.wState=0;
	AddScore.lScore=lCurrentScore;

	//���ܲ���
	AESEncrypt aes(m_chUserAESKey);
	unsigned char chTempEncryption[AESENCRYPTION_LENGTH];
	ZeroMemory(chTempEncryption, sizeof(chTempEncryption));
	CopyMemory(chTempEncryption, chCommonEncryption, sizeof(chTempEncryption));

	aes.Cipher(chTempEncryption);
	CopyMemory(AddScore.chciphertext, chTempEncryption, sizeof(chTempEncryption));

	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	return 0;
}

//������Ϣ
LRESULT	CGameClientEngine::OnLookCard(WPARAM wParam, LPARAM lParam)
{
	//for(WORD i=0;i<MAX_COUNT;i++)
	//{
	//	for (WORD j=0;j<GAME_PLAYER;j++)
	//	{
	//		WORD w=j%GAME_PLAYER;
	//		if (TRUE)
	//		{
	//	//		m_GameClientView.DispatchUserCard(j,0);
	//		}
	//	}
	//	m_GameClientView.m_NumberControl[i].SetScore( -20000000000000000 );
	//	
	//}

	//WORD IDChair[2] = {2,3};
	//m_GameClientView.PerformCompareCard(IDChair,2);

	//m_GameClientView.BeginMoveNumber();/**/
	//���ÿؼ�
	WORD wMeChairID=GetMeChairID();
	//KillGameClock(IDI_USER_ADD_SCORE);

	//���¶�ʱ
	//SetGameClock(wMeChairID,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);

	//��������
	m_bMingZhu[wMeChairID] = true;

	//������Ϣ
	SendSocketData(SUB_C_LOOK_CARD,NULL,0);
	
	//�ؼ���Ϣ
	UpdataControl();

	//��������
	PlayGameSound(AfxGetInstanceHandle(),TEXT("CENTER_SEND_CARD"));

	return 0;
}

//������Ϣ
LRESULT	CGameClientEngine::OnCompareCard(WPARAM wParam, LPARAM lParam)
{
	//ɾ����ʱ��
	KillGameClock(IDI_USER_ADD_SCORE);

	//����ؼ�
	AddScoreControl(SW_HIDE);

//	m_GameClientView.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	m_GameClientView.m_btFollow.EnableWindow(FALSE);

	//�ж���ע
	WORD wMeChairID=GetMeChairID();
	LONGLONG lCurrentScore=(m_bMingZhu[wMeChairID])?(m_lCurrentTimes*m_lCellScore*4):(m_lCurrentTimes*m_lCellScore*2);
	m_lTableScore[wMeChairID]+=lCurrentScore;
	m_GameClientView.SetUserTableScore(m_wViewChairID[wMeChairID],m_lTableScore[wMeChairID],lCurrentScore);
	m_GameClientView.BeginMoveJettons();

	//������Ϣ
	CMD_C_AddScore AddScore;
	ZeroMemory(&AddScore, sizeof(AddScore));
	AddScore.wState=TRUE;
	AddScore.lScore=lCurrentScore;

	//���ܲ���
	AESEncrypt aes(m_chUserAESKey);
	unsigned char chTempEncryption[AESENCRYPTION_LENGTH];
	ZeroMemory(chTempEncryption, sizeof(chTempEncryption));
	CopyMemory(chTempEncryption, chCommonEncryption, sizeof(chTempEncryption));

	aes.Cipher(chTempEncryption);
	CopyMemory(AddScore.chciphertext, chTempEncryption, sizeof(chTempEncryption));

	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	//��ǰ����
	BYTE UserCount=0;
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==TRUE)UserCount++;
	}

	//ׯ���ڵ�һ��û��עֻ�ܸ��ϼұ��� �� ֻʣ������
	if(m_wBankerUser==wMeChairID && (m_lTableScore[wMeChairID]-lCurrentScore)==m_lCellScore || UserCount==2)
	{
		//�������
		CMD_C_CompareCard CompareCard;

		//�����ϼ�
		for (LONGLONG i=(LONGLONG)wMeChairID-1;;i--)
		{
			if(i==-1)i=GAME_PLAYER-1;
			if(m_cbPlayStatus[i]==TRUE)
			{
				CompareCard.wCompareUser=(WORD)i;
				break;
			}
		}
		//������Ϣ
		SendSocketData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CompareCard));
		KillGameClock(IDI_USER_COMPARE_CARD);
	}
	else	//ѡ����ұ���
	{
		//���ð�ť
		m_GameClientView.m_btJetton_1.EnableWindow(FALSE);
		m_GameClientView.m_btJetton_2.EnableWindow(FALSE);
		m_GameClientView.m_btJetton_5.EnableWindow(FALSE);

		//���ñ���
		BOOL bCompareUser[GAME_PLAYER];
		for(int i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i]==TRUE && wMeChairID!=i)
			{
				bCompareUser[m_wViewChairID[i]]=TRUE;
			}
			else bCompareUser[m_wViewChairID[i]]=FALSE;
		}

		//���ü�ͷ
		m_GameClientView.SetCompareCard(true,bCompareUser);

		//���͵ȴ�"����"��Ϣ
		SendSocketData(SUB_C_WAIT_COMPARE,NULL,0);

		//����ʱ��
		SetGameClock(wMeChairID,IDI_USER_COMPARE_CARD,TIME_USER_COMPARE_CARD);

		//��ʾ��־
		m_GameClientView.SetWaitUserChoice(FALSE);
	}

	return 0;
}

//��ѡ�����û�
LRESULT CGameClientEngine::OnChooseCUser(WPARAM wParam, LPARAM lParam)
{
	//Ч������
	WORD wTemp=LOWORD(wParam);
	WORD wChairID=INVALID_CHAIR;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_wViewChairID[i]==wTemp)
		{
			wChairID=i;
			break;
		}
	}
	ASSERT(wChairID!=INVALID_CHAIR && m_cbPlayStatus[wChairID]==TRUE && wChairID!=GetMeChairID());

	//ɾ����ʱ��
	KillGameClock(IDI_USER_COMPARE_CARD);

	//�������
	m_GameClientView.SetCompareCard(false,NULL);

	//�������
	CMD_C_CompareCard CompareCard;
	ZeroMemory(&CompareCard,sizeof(CompareCard));
	CompareCard.wCompareUser=wChairID;

	//������Ϣ
	SendSocketData(SUB_C_COMPARE_CARD,&CompareCard,sizeof(CompareCard));

	return 0;
}

//������Ϣ
LRESULT	CGameClientEngine::OnOpenCard(WPARAM wParam, LPARAM lParam)
{
	//ɾ����ʱ��
	KillGameClock(IDI_USER_ADD_SCORE);

	//����ؼ�
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	//m_GameClientView.m_btOpenCard.EnableWindow(FALSE);
//	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);

	//��������
	WORD bUserCount =0;
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==TRUE) bUserCount++;
	}

	//���˱���
	if(bUserCount==2)
	{
		OnCompareCard(0,0);
		return 0;
	}

	//�ж���ע
	WORD wMeChairID=GetMeChairID();
	LONGLONG lCurrentScore=(m_bMingZhu[wMeChairID])?(m_lCurrentTimes*m_lCellScore*4):(m_lCurrentTimes*m_lCellScore*2);
	m_lTableScore[wMeChairID]+=lCurrentScore;
	m_GameClientView.SetUserTableScore(m_wViewChairID[wMeChairID],m_lTableScore[wMeChairID],lCurrentScore);
	m_GameClientView.BeginMoveJettons();

	//������Ϣ
	CMD_C_AddScore AddScore;
	ZeroMemory(&AddScore, sizeof(AddScore));
	AddScore.wState=2;
	AddScore.lScore=lCurrentScore;

	//���ܲ���
	AESEncrypt aes(m_chUserAESKey);
	unsigned char chTempEncryption[AESENCRYPTION_LENGTH];
	ZeroMemory(chTempEncryption, sizeof(chTempEncryption));
	CopyMemory(chTempEncryption, chCommonEncryption, sizeof(chTempEncryption));

	aes.Cipher(chTempEncryption);
	CopyMemory(AddScore.chciphertext, chTempEncryption, sizeof(chTempEncryption));

	SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

	//������Ϣ
	SendSocketData(SUB_C_OPEN_CARD,NULL,0);

	return 0;
}

//������Ϣ
LRESULT	CGameClientEngine::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	//ɾ��ʱ��
	KillGameClock(IDI_USER_ADD_SCORE);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

	//����ť
	ScoreControl(SW_HIDE);

//	m_GameClientView.m_btOpenCard.ShowWindow(SW_HIDE);
//	m_GameClientView.m_btAddScore.EnableWindow(FALSE);
	m_GameClientView.m_btCompareCard.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_btLookCard.EnableWindow(FALSE);
	m_GameClientView.m_btFollow.EnableWindow(FALSE);

	//�˿˱��
	if (!m_bCheatUser)
	{
		m_GameClientView.m_CardControl[m_wViewChairID[GetMeChairID()]].SetCardColor(1);
	}
	

	//������Ϣ
	SendSocketData(SUB_C_GIVE_UP,NULL,0);

	return 0;
}

//�������
LRESULT CGameClientEngine::OnSendCardFinish(WPARAM wParam, LPARAM lParam)
{
	//����ʱ��
	if(m_wCurrentUser==INVALID_CHAIR)return 0;
	SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
	//SendSocketData(SUB_C_ADD_SCORE_TIME);

	//�����û�
	WORD wMeChairID=GetMeChairID();
	if (IsLookonMode() || wMeChairID!=m_wCurrentUser)return 0;

	//���ÿؼ�
	ActiveGameFrame();
	UpdataControl();

	return 0;
}

//�������
LRESULT CGameClientEngine::OnFlashCardFinish(WPARAM wParam, LPARAM lParam)
{
	//���ƽ���
	if(m_wWinnerUser>=GAME_PLAYER)
	{
		if(m_wLostUser>GAME_PLAYER)return 0;
		ASSERT(m_wLostUser<GAME_PLAYER);

		//�����û�
		WORD wLostUser=m_wLostUser;
		WORD wMeChairID=GetMeChairID();

		//��������
		WORD wViewChairID=m_wViewChairID[wLostUser];
		WORD wMeViewChairID=m_wViewChairID[wMeChairID];

		//�����˿�
		m_GameClientView.m_CardControl[wMeViewChairID].SetCardData(m_cbHandCardData[wMeChairID],MAX_COUNT);

		//������ʾ
		for(WORD i=0;i<GAME_PLAYER;i++)
		{
			IClientUserItem *pUserInfo = GetTableUserItem(i);
			if (NULL != pUserInfo)
			{
				m_GameClientView.m_CardControl[SwitchViewChairID(i)].SetCardData(m_cbHandCardData[i],MAX_COUNT);
			}
			
		}

		//��������
		if ((!IsLookonMode()) && wLostUser==wMeChairID) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));

		//�������
		BYTE bCount=0;
		for (WORD i=0;i<GAME_PLAYER;i++)if(m_cbPlayStatus[i]==TRUE)bCount++;

		//�жϽ���
		if(bCount>1)
		{
			//�ؼ���Ϣ
			if((!IsLookonMode()) && wMeChairID==m_wCurrentUser)
			{
				UpdataControl();
			}

			//����ʱ��
			SetGameClock(m_wCurrentUser,IDI_USER_ADD_SCORE,TIME_USER_ADD_SCORE);
		}
		else 
		{
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(m_cbPlayStatus[i]==TRUE)break;
			}
			WORD wMeChairID = GetMeChairID();
			if(i==wMeChairID || wMeChairID==m_wLostUser)
			{
				//������Ϣ
				SendSocketData(SUB_C_FINISH_FLASH,NULL,0);
			}
		}
	}
	else			//���ƽ���
	{
		//ASSERT(m_wWinnerUser<GAME_PLAYER);
		if(m_wWinnerUser>GAME_PLAYER)return 0;		

		//����ʧ���û�
		for (int i=0;i<GAME_PLAYER;i++)
		{
			if(m_cbPlayStatus[i]==TRUE && i!=m_wWinnerUser)		
			{
				//�˿˱���
				WORD wViewChairID=m_wViewChairID[i];

				if (!m_bCheatUser)
				{
					m_GameClientView.m_CardControl[wViewChairID].SetCardColor(2);
				}
				

				//״̬����
				if ((!IsLookonMode())&&(i==GetMeChairID())) 
				{
					SetGameStatus(GAME_STATUS_FREE);
					//��������
					PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
				}
			}
		}
		m_wWinnerUser=INVALID_CHAIR;

		//������Ϣ
		SendSocketData(SUB_C_FINISH_FLASH,NULL,0);
	}
	
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//���˶�̬�����û��˿�
		IClientUserItem * pClientUserItem=GetTableUserItem(i);
		if(pClientUserItem!=NULL && pClientUserItem->GetUserStatus()!=US_PLAYING && pClientUserItem->GetUserStatus()!=US_OFFLINE)
		{
			WORD wViewChiar=m_wViewChairID[i];
			m_GameClientView.m_CardControl[wViewChiar].SetCardData(NULL,0);
		}
	}
	//���˶�̬�����û��˿�
	/*IClientUserItem * pClientUserItem=GetTableUserItem(GetMeChairID());
	if(pClientUserItem!=NULL && pClientUserItem->GetUserStatus()!=US_PLAYING && pClientUserItem->GetUserStatus()!=US_OFFLINE)
	{
		WORD wViewChiar=m_wViewChairID[GetMeChairID()];
		m_GameClientView.m_CardControl[wViewChiar].SetCardData(NULL,0);
	}
	else
	{
		WORD wViewChiar=m_wViewChairID[GetMeChairID()];
		m_GameClientView.m_CardControl[wViewChiar].SetCardData(NULL,0);
	}*/
	return 0;
}

//������Ϣ
LRESULT CGameClientEngine::OnChatMessage( WPARAM wParam, LPARAM lParam )
{
	//m_GameClientView.InsertAllChatMessage((LPCTSTR)wParam,int(lParam));

	return 0;
}

//��Ϸ����
LRESULT CGameClientEngine::OnMessageGameSound(WPARAM wParam, LPARAM lParam)
{
	CGlobalUnits* pGlabalUnits = CGlobalUnits::GetInstance();
	HINSTANCE hInstance=AfxGetInstanceHandle(); 
	pGlabalUnits->m_bAllowSound = !pGlabalUnits->m_bAllowSound;
	bool bSound = pGlabalUnits->m_bAllowSound;
	if(bSound == false)
	{
		StopSound();
		m_bPlaySound = false;
		m_GameClientView.m_btSound.SetButtonImage(IDB_CLOSE_SOUND,hInstance,false,false);
	}
	else
	{
		PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));
		m_GameClientView.m_btSound.SetButtonImage(IDB_OPEN_SOUND,hInstance,false,false);
		m_bPlaySound = true;
	}
	bool sound = pGlabalUnits->IsAllowGameSound();
	sound = pGlabalUnits->m_bMuteStatuts;
	return 0;
}
//�û�����
VOID CGameClientEngine::OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	if(bLookonUser) return;

	//������Դ
	WORD wViewChair = SwitchViewChairID(pIClientUserItem->GetChairID());
	BYTE cbGender = pIClientUserItem->GetGender();
	m_GameClientView.LoadUserImage(wViewChair,cbGender);

	return;
}
//�û��뿪
VOID CGameClientEngine::OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser)
{
	//ж����Դ
	if(!bLookonUser) m_GameClientView.DestroyUserImage(SwitchViewChairID(pIClientUserItem->GetChairID()));

	return;
}

//��������
bool CGameClientEngine::AllowBackGroundSound(bool bAllowSound)
{
	CGlobalUnits *pGlobalUnits=CGlobalUnits::GetInstance();
	if(bAllowSound  && !pGlobalUnits->m_bMuteStatuts)
	{
		PlayBackGroundSound(AfxGetInstanceHandle(), TEXT("BACK_GROUND"));
	}
	else if(!bAllowSound || pGlobalUnits->m_bMuteStatuts)
	{
		StopSound();
	}
	return true;
}

LRESULT CGameClientEngine::OnStorage(WPARAM wParam, LPARAM lParam)
{
	SendSocketData(SUB_C_STORAGE, (CMD_C_UpdateStorage*)wParam, sizeof(CMD_C_UpdateStorage));
	return true;
}

LRESULT CGameClientEngine::OnStorageInfo(WPARAM wParam, LPARAM lParam)
{
	SendSocketData(SUB_C_STORAGEMAXMUL, (CMD_C_ModifyStorage*)wParam, sizeof(CMD_C_ModifyStorage));
	return true;
}

LRESULT CGameClientEngine::OnRequestQueryUser(WPARAM wParam, LPARAM lParam)
{
	SendSocketData(SUB_C_REQUEST_QUERY_USER, (CMD_C_RequestQuery_User*)wParam, sizeof(CMD_C_RequestQuery_User));
	return true;
}

//�û�����
LRESULT CGameClientEngine::OnUserControl(WPARAM wParam, LPARAM lParam)
{
	SendSocketData(SUB_C_USER_CONTROL, (CMD_C_UserControl*)wParam, sizeof(CMD_C_UserControl));
	return true;
}

//������·�����Ϣ
LRESULT CGameClientEngine::OnRequestUpdateRoomInfo(WPARAM wParam, LPARAM lParam)
{
	SendSocketData(SUB_C_REQUEST_UDPATE_ROOMINFO);
	return true;
}

//��յ�ǰ��ѯ�û�
LRESULT CGameClientEngine::OnClearCurrentQueryUser(WPARAM wParam, LPARAM lParam)
{
	SendSocketData(SUB_C_CLEAR_CURRENT_QUERYUSER);
	return true;
}
//////////////////////////////////////////////////////////////////////////
