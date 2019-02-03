#include "StdAfx.h"
#include "TableFrameSink.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////////////

int					CTableFrameSink::m_IsOpen=0;									//
LONGLONG			CTableFrameSink::m_lStorageStartTable = 0;						//��ʼ���ӿ��
LONGLONG			CTableFrameSink::m_lStorageInputTable = 0;						//��ʼ���ӿ��
LONGLONG			CTableFrameSink::m_lCurStorage = 0;								//��ʼ���ӿ��
LONGLONG			CTableFrameSink::m_lRoomStockRecoverScore = 0;					//��ʼ���ӿ��
LONGLONG			CTableFrameSink::m_lStorageDeductRoom = 0;						//ȫ�ֿ��˥��
LONGLONG			CTableFrameSink::m_lStorageMax1Room = 0;						//ȫ�ֿ��ⶥ
LONGLONG			CTableFrameSink::m_lStorageMul1Room = 0;						//ȫ��ϵͳ��Ǯ����
LONGLONG			CTableFrameSink::m_lStorageMax2Room = 0;						//ȫ�ֿ��ⶥ
LONGLONG			CTableFrameSink::m_lStorageMul2Room = 0;						//ȫ��ϵͳ��Ǯ����
WORD				CTableFrameSink::m_wGameTwo = INVALID_WORD;						//�ȱ�����
WORD				CTableFrameSink::m_wGameTwoDeduct = INVALID_WORD;				//�ȱ�����
WORD				CTableFrameSink::m_wGameThree = INVALID_WORD;					//С��������
WORD				CTableFrameSink::m_wGameThreeDeduct = INVALID_WORD;				//С��������
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> CTableFrameSink::m_MapRoomUserInfo;	//���USERIDӳ�������Ϣ
CList<CString, CString&> CTableFrameSink::m_ListOperationRecord;					//�������Ƽ�¼


//////////////////////////////////////////////////////////////////////////////////
//��ʱ��ID
#define IDI_CHECK_TABLE				1					//�������
#define IDI_USER_OUT_TIME			6					//��ҳ�ʱ
#define IDI_SAMll_GAME				7					//С��Ϸ

//��ʱ��ʱ��
#define IDI_SAMll_GAME_TIME			60					//С��Ϸʱ��

//���캯��
CTableFrameSink::CTableFrameSink()
{
	m_Bonus = 0;
	m_cbLine = 0;
	m_lBet = 0;
	m_cbGameMode = GM_NULL;
	m_cbTwoMode = 0xFF;
	m_cbCurBounsGameCount = 0;
	m_cbBounsGameCount = 0;
	m_lOneGameScore = 0;
	m_lBetScore = 0;
	m_lTwoGameScore = 0;
	m_lThreeGameScore = 0;
	m_bGameStart = false;
	m_cbALLSmallSorce = 0;
	m_cbCustoms = 0;
	m_BonusCount = 0;
	m_BonusGetSorce = 0;

	ZeroMemory(SamllGameFruit, sizeof(SamllGameFruit));
	ZeroMemory(m_BonusSorce, sizeof(m_BonusSorce));
	ZeroMemory(m_BonusSorceDate, sizeof(m_BonusSorceDate));
	ZeroMemory(m_NickName, sizeof(m_NickName));

	//�������
	m_pITableFrame=NULL;
	m_pGameCustomRule=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//�������
	m_wSingleGameTwo = INVALID_WORD;
	m_wSingleGameThree = INVALID_WORD;
	m_hInst = NULL;
	//m_pServerControl = NULL;
	//�������
	m_pServerControl = new CServerControlItemSink;

	/*m_hInst = LoadLibrary(TEXT("WaterMarginServerControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerControl = static_cast<IServerControl*>(ServerControl());
		}
	}*/

	return;
}

//��������
CTableFrameSink::~CTableFrameSink()
{
	if( m_pServerControl )
	{
		delete m_pServerControl;
		m_pServerControl = NULL;
	}

	if( m_hInst )
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}

//�ӿڲ�ѯ
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	m_cbGameMode = GM_NULL;
	m_cbTwoMode = 0xFF;
	m_cbCurBounsGameCount = 0;
	m_cbBounsGameCount = 0;
	m_lOneGameScore = 0;
	m_lBetScore = 0;
	m_lTwoGameScore = 0;
	m_lThreeGameScore = 0;	
	m_cbALLSmallSorce = 0;
	m_BonusGetSorce = 0;
	return;
}

//��������
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

	//�����ж�
	if (m_pITableFrame==NULL)
	{
		CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"),TraceLevel_Exception);
		return NcaTextOut(__LINE__);
	}

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//��Ϸ����
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//�Զ�����
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);
	m_pGameCustomRule=(tagCustomRule *)m_pITableFrame->GetCustomRule();

	ASSERT(m_pGameCustomRule != NULL);

	WORD wTableID = m_pITableFrame->GetTableID();
	
	//��ʼ�����
	LONGLONG lCurrentStorageTable = m_pGameCustomRule->lStorageStartTable;
	m_lCurStorage = m_pGameCustomRule->lStorageStartTable;
	m_lStorageStartTable = m_lCurStorage;
	m_lStorageDeductRoom = /*m_pGameCustomRule->lStorageDeductRoom*/0;

	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

  	//�����ļ���
	TCHAR szPath1[MAX_PATH]=TEXT("");
	GetModuleFileName( NULL,szPath1,CountArray(szPath1));

	CString szTemp;
	szTemp = szPath1;
	int nPos = szTemp.ReverseFind(TEXT('\\'));
	_sntprintf(szPath, CountArray(szPath), TEXT("%s"), szTemp.Left(nPos));
	_sntprintf(m_szControlConfigFileName,sizeof(m_szControlConfigFileName),TEXT("%s\\GameControl\\bairen.ini"),szPath);

	_sntprintf(m_szControlRoomName, sizeof(m_szControlRoomName), _T("%d"),m_pGameServiceOption->wServerID);


	ReadConfigInformation();

	m_lStorageInputTable = m_pGameCustomRule->lStorageStartTable;
	m_lStorageMax1Room = m_pGameCustomRule->lStorageMax1Room;
	m_lStorageMul1Room = m_pGameCustomRule->lStorageMul1Room;
	m_lStorageMax2Room = m_pGameCustomRule->lStorageMax2Room;
	m_lStorageMul2Room = m_pGameCustomRule->lStorageMul2Room;
	m_wGameTwo = m_pGameCustomRule->wGameTwo;
	m_wGameTwoDeduct = m_pGameCustomRule->wGameTwoDeduct;
	m_wGameThree = m_pGameCustomRule->wGameThree;
	m_wGameThreeDeduct = m_pGameCustomRule->wGameThreeDeduct;

// 	//У������
// 	if (m_lStorageDeductRoom < 0 || m_lStorageDeductRoom > 1000)
// 	{
// 		m_lStorageDeductRoom = 0;
// 	}
// 
// 	if (m_lStorageDeductRoom > 1000)
// 	{
// 		m_lStorageDeductRoom = 1000;
// 	}

	if (m_lStorageMul1Room < 0 || m_lStorageMul1Room > 1000) 
	{
		m_lStorageMul1Room = 100;
	}

	if (m_lStorageMul2Room < 0 || m_lStorageMul2Room > 1000) 
	{
		m_lStorageMul2Room = 80;
	}

	//int nCount = 100000;
	//int nGameCount = 200;
	//LONGLONG lWinScore[200] = {0};
	//LONGLONG lScore = 0;
	//int nWinCount[200] = {0};
	//int nSHZCount[200] = {0};
	//double dControl = 40;
	//double dControlTemp = 0.1;
	//double dWin[200] = {0};
	//for (int n = 0; n < nGameCount; n++)
	//{
	//	for (int i = 0; i < nCount; i++)
	//	{
	//		lWinScore[n] += m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet] * m_cbLine;
	//		m_GameLogic.RandCardList(m_cbItemInfo,dControl);
	//		lScore = m_GameLogic.GetZhongJiangTime(m_cbItemInfo) * m_pGameServiceOption->lCellScore * m_pGameCustomRule->lBetScore[m_lBet];
	//		lWinScore[n] -= lScore;
	//		if(lScore > 0)
	//		{
	//			nWinCount[n]++;
	//		}


	//		bool bFind = false;
	//		for (int ii = 0; ii < ITEM_Y_COUNT; ii++)
	//		{
	//			for (int j = 0; j < ITEM_X_COUNT; j++)
	//			{
	//				if(m_cbItemInfo[ii][j] == CT_SHUIHUZHUAN)
	//				{
	//					nSHZCount[n]++;
	//					bFind = true;
	//					break;
	//				}
	//			}

	//			if(bFind) break;
	//		}
	//	}
	//	dControl += dControlTemp;
	//	dWin[n] = nWinCount[n] / ((double)nCount);
	//}


	//С��Ϸ����
	CopyMemory(FirstSorce, m_pGameCustomRule->TheFirstPass, sizeof(m_pGameCustomRule->TheFirstPass));
	CopyMemory(SecondSorce, m_pGameCustomRule->TheSecondPass, sizeof(m_pGameCustomRule->TheSecondPass));
	CopyMemory(ThirdSorce, m_pGameCustomRule->TheThirdPass, sizeof(m_pGameCustomRule->TheThirdPass));
	CopyMemory(FourthSorce, m_pGameCustomRule->TheFourthPass, sizeof(m_pGameCustomRule->TheFourthPass));



	return true;
}
//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{
	TCHAR OutBuf[255];
	//���ƿ��
  	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("IsOpen"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_IsOpen = _tstoi(OutBuf);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("StorageStart"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_lCurStorage = _tstoi64(OutBuf);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("StorageDeduct"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_lStorageDeductRoom = _tstoi64(OutBuf);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("AttenuationScore"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_lRoomStockRecoverScore = _tstoi64(OutBuf);

  return;
}
//��������
DOUBLE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	return 0L;
}

//���ٻ���
DOUBLE CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return 0;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	//����״̬
	m_pITableFrame->SetGameStatus(GAME_SCENE_WIAT);	
	
	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������
		{				
			if(m_bGameStart)
			{
				//���ֱ���
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

				if(pIServerUserItem != NULL)
				{			
					//�������
					ScoreInfoArray[0].lScore = m_lBetScore + (m_lOneGameScore + m_cbALLSmallSorce + m_BonusGetSorce) * 0.98;
					ScoreInfoArray[0].cbType=(ScoreInfoArray[0].lScore>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

					CString str4;
					str4.Format(_T("��Ϸ����ǰ�ʽ�::%lld"), m_Bonus);
					CTraceService::TraceString(str4, TraceLevel_Warning);

					//���Ӳʽ��(Ӯ�ְٷ�֮������ʽ��)
					if (ScoreInfoArray[0].cbType == SCORE_TYPE_WIN)
					{
						//m_Bonus = QueryeBonus();
						m_Bonus += (m_lOneGameScore + m_cbALLSmallSorce + m_BonusGetSorce) * 0.02;
//						m_pITableFrame->SetBonus(m_Bonus);
					}

					//m_Bonus = 0;//m_pITableFrame->QueryeBonus();

					CString str3;
					str3.Format(_T("��Ϸ������ʽ�::%lld"), m_Bonus);
					CTraceService::TraceString(str3, TraceLevel_Warning);

					//����˰��
					ScoreInfoArray[0].lRevenue=m_pITableFrame->CalculateRevenue(0,ScoreInfoArray[0].lScore);
					if (ScoreInfoArray[0].lRevenue>0L) ScoreInfoArray[0].lScore-=ScoreInfoArray[0].lRevenue;




					//�����˲�д���
					if(!pIServerUserItem->IsAndroidUser())
					{		
						  ReadConfigInformation();
						m_lCurStorage -= ScoreInfoArray[0].lScore;
		//���˥��
		if (ScoreInfoArray[0].lScore < -0.001 || ScoreInfoArray[0].lScore > 0.001)
		{
			m_lRoomStockRecoverScore +=  m_lCurStorage*m_lStorageDeductRoom/1000;
			m_lCurStorage -= m_lCurStorage*m_lStorageDeductRoom/1000;
		}
						//���
						WORD wTableID = m_pITableFrame->GetTableID();

						m_lCurStorage -= ScoreInfoArray[0].lScore;

						if(ScoreInfoArray[0].lScore+pIServerUserItem->GetUserScore() < 0)
						{
							CString str;
							str.Format(TEXT("��%d���� ��ҡ�%d����%s����Ҳ���,Я�����:%I64d,�ɼ�:%I64d"), m_pITableFrame->GetTableID() + 1,  pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(),pIServerUserItem->GetUserScore(),ScoreInfoArray[0].lScore );
							NcaTextOut(str, m_pGameServiceOption->szServerName);
							ScoreInfoArray[0].lScore  = -pIServerUserItem->GetUserScore();
						}
						if(m_lCurStorage < 0)
						{
							CString str;
							str.Format(TEXT("��%d���� ��ǰ��� %I64d, �÷� %I64d�� %s[%ld]�� ��ע %I64d�� Ѻ�� %I64d�� �ȱ� %I64d�� С���� %I64d"), m_pITableFrame->GetTableID() + 1, m_lCurStorage, ScoreInfoArray[0].lScore, pIServerUserItem->GetNickName(),pIServerUserItem->GetGameID(), m_lBetScore, m_lOneGameScore, m_lTwoGameScore, m_lThreeGameScore);
							NcaTextOut(str, m_pGameServiceOption->szServerName);
						}
						TCHAR szStorage[1024]=TEXT("");
						_sntprintf(szStorage,sizeof(szStorage),TEXT("%ld"),m_lCurStorage);
						WritePrivateProfileString(m_szControlRoomName,TEXT("StorageStart"),szStorage,m_szControlConfigFileName);

						_sntprintf(szStorage,sizeof(szStorage),TEXT("%ld"),m_lRoomStockRecoverScore);
						WritePrivateProfileString(m_szControlRoomName,TEXT("AttenuationScore"),szStorage,m_szControlConfigFileName);

					}



					//д�����
					m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
				}

				m_bGameStart = false;

				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				//��������
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);


				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN, ScoreInfoArray[0].lScore);

			}
			else
			{
				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
			}

			
			return true;
		}
	case GER_DISMISS:		//��Ϸ��ɢ
		{
			m_bGameStart = false;

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

			//��������
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_CONCLUDE);

			//��ȡ�û�
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(0);

			ASSERT(pIServerUserItem != NULL);

			UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

			return true;
		}
	case GER_USER_LEAVE:	//�û�ǿ��
	case GER_NETWORK_ERROR:	//�����ж�
		{
			if(m_bGameStart)
			{
				m_bGameStart = false;
				//���ֱ���
				tagScoreInfo ScoreInfoArray[GAME_PLAYER];
				ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

				if(pIServerUserItem != NULL)
				{
					//�������
					ScoreInfoArray[0].lScore=m_lBetScore+m_lOneGameScore+m_lTwoGameScore+m_lThreeGameScore;
					ScoreInfoArray[0].cbType=(ScoreInfoArray[0].lScore>=0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;

					//����˰��
					ScoreInfoArray[0].lRevenue=m_pITableFrame->CalculateRevenue(0,ScoreInfoArray[0].lScore);
					if (ScoreInfoArray[0].lRevenue>0L) ScoreInfoArray[0].lScore-=ScoreInfoArray[0].lRevenue;

					//�����˲�д���
					if(!pIServerUserItem->IsAndroidUser())
					{
						//���
						WORD wTableID = m_pITableFrame->GetTableID();
						m_lCurStorage -= ScoreInfoArray[0].lScore;
						if(m_lCurStorage < 0)
						{
							CString str;
							str.Format(TEXT("��%d���� ��ǰ��� %I64d, �÷� %I64d�� %s[%ld]�� ��ע %I64d�� Ѻ�� %I64d�� �ȱ� %I64d�� С���� %I64d"), m_pITableFrame->GetTableID() + 1, m_lCurStorage, ScoreInfoArray[0].lScore, pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), m_lBetScore, m_lOneGameScore, m_lTwoGameScore, m_lThreeGameScore);
							NcaTextOut(str, m_pGameServiceOption->szServerName);
						}
						if(ScoreInfoArray[0].lScore+pIServerUserItem->GetUserScore() < 0)
						{
							CString str;
							str.Format(TEXT("��%d���� ��ҡ�%d����%s����Ҳ���,Я�����:%I64d,�ɼ�:%I64d"), m_pITableFrame->GetTableID() + 1,  pIServerUserItem->GetGameID(), pIServerUserItem->GetNickName(),pIServerUserItem->GetUserScore(),ScoreInfoArray[0].lScore );
							NcaTextOut(str, m_pGameServiceOption->szServerName);
							ScoreInfoArray[0].lScore  = -pIServerUserItem->GetUserScore();
						}
					}
					//д�����
					m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));
				}
				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP, ScoreInfoArray[0].lScore);
			}
			else
			{
				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

				UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
			}

			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return NcaTextOut(__LINE__, m_pITableFrame->GetTableUserItem(wChairID));
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_SCENE_FREE:	//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//��Ԫ����
			StatusFree.lCellScore = m_pGameServiceOption->lCellScore;

			
			//m_Bonus = 0;//m_pITableFrame->QueryeBonus();
			
			//�ʽ��
			StatusFree.ALLbonus = m_Bonus;

			CString str4;
			str4.Format(_T("����״̬%lld"), StatusFree.ALLbonus);
			CTraceService::TraceString(str4, TraceLevel_Warning);

			//��עֵ
			StatusFree.cbBetCount = m_pGameCustomRule->cbBetCount;
			CopyMemory(StatusFree.lBetScore, m_pGameCustomRule->lBetScore, sizeof(StatusFree.lBetScore));

			//���·����û���Ϣ
			UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				//��������
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));

				//���ҵ�ǰ���ӿ��
				LONGLONG lCurrentStorageTable = m_lCurStorage;

				//��������
				StorageInfo.lCurrentStorageTable = lCurrentStorageTable;
				StorageInfo.lCurrentDeductRoom = m_lStorageDeductRoom;
				StorageInfo.lMaxStorageRoom[0] = m_lStorageMax1Room;
				StorageInfo.lMaxStorageRoom[1] = m_lStorageMax2Room;
				StorageInfo.wStorageMulRoom[0] = (WORD)m_lStorageMul1Room;
				StorageInfo.wStorageMulRoom[1] = (WORD)m_lStorageMul2Room;
				StorageInfo.wGameTwo = m_wGameTwo;
				StorageInfo.wGameTwoDeduct = m_wGameTwoDeduct;
				StorageInfo.wGameThree = m_wGameThree;
				StorageInfo.wGameThreeDeduct = m_wGameThreeDeduct;
				StorageInfo.lRoomStorageStart = m_lStorageStartTable;
				StorageInfo.lRoomStorageCurrent = m_lCurStorage;
				StorageInfo.lRoomStorageInput = m_lStorageInputTable;
				StorageInfo.lRoomStockRecoverScore = m_lRoomStockRecoverScore;


				m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GAME_SMALL_GAME:	//С��Ϸ״̬
		{
			//��������
			CMD_S_SmallStatus StatusSmall;
			ZeroMemory(&StatusSmall, sizeof(StatusSmall));

			StatusSmall.lCellScore = m_pGameServiceOption->lCellScore;
			StatusSmall.cbGameMode = m_cbGameMode;
			StatusSmall.lBet = m_lBet;
			StatusSmall.lBetCount = m_cbLine;
			StatusSmall.lOneGameScore = m_lOneGameScore;
			StatusSmall.lSmallGameScore = m_cbALLSmallSorce;
			StatusSmall.cbCustoms = m_cbCustoms;
			StatusSmall.cbBonus = m_Bonus;

			//С��Ϸʣ��ʱ��
			CTime time = CTime::GetCurrentTime();
			StatusSmall.wUseTime = (time.GetMinute() - m_tCreateTime.GetMinute()) * 60 + (time.GetSecond() - m_tCreateTime.GetSecond());

			CopyMemory(StatusSmall.cbItemInfo, m_cbItemInfo, sizeof(StatusSmall.cbItemInfo));	//������Ϣ
			CopyMemory(StatusSmall.lBetScore, m_pGameCustomRule->lBetScore, sizeof(StatusSmall.lBetScore));

			//���·����û���Ϣ
			UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
			{
				//��������
				CMD_S_ADMIN_STORAGE_INFO StorageInfo;
				ZeroMemory(&StorageInfo, sizeof(StorageInfo));

				//���ҵ�ǰ���ӿ��
				LONGLONG lCurrentStorageTable = m_lCurStorage;

				//��������
				StorageInfo.lCurrentStorageTable = lCurrentStorageTable;
				StorageInfo.lCurrentDeductRoom = m_lStorageDeductRoom;
				StorageInfo.lMaxStorageRoom[0] = m_lStorageMax1Room;
				StorageInfo.lMaxStorageRoom[1] = m_lStorageMax2Room;
				StorageInfo.wStorageMulRoom[0] = (WORD)m_lStorageMul1Room;
				StorageInfo.wStorageMulRoom[1] = (WORD)m_lStorageMul2Room;
				StorageInfo.wGameTwo = m_wGameTwo;
				StorageInfo.wGameTwoDeduct = m_wGameTwoDeduct;
				StorageInfo.wGameThree = m_wGameThree;
				StorageInfo.wGameThreeDeduct = m_wGameThreeDeduct;
				StorageInfo.lRoomStorageStart = m_lStorageStartTable;
				StorageInfo.lRoomStorageCurrent = m_lCurStorage;
				StorageInfo.lRoomStorageInput = m_lStorageInputTable;
				StorageInfo.lRoomStockRecoverScore = m_lRoomStockRecoverScore;
				m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
				m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusSmall, sizeof(StatusSmall));
		}
	}

	//�������
	ASSERT(FALSE);

	return NcaTextOut(__LINE__, m_pITableFrame->GetTableUserItem(wChairID));
}

//ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_SAMll_GAME:
	{
		//ɾ����ʱ�� ������Ϸ״̬
		//m_pITableFrame->SetGameStatus(GM_NULL);
		//m_pITableFrame->KillGameTimer(IDI_SAMll_GAME);
	}
		break;
	default:
		return NcaTextOut(__LINE__);
	}
	return NcaTextOut(__LINE__);
}

//�����¼�
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return NcaTextOut(__LINE__);
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return NcaTextOut(__LINE__);
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_ONE_START:					//Ѻ�߿�ʼ
		{

			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_OneStart));
			if (wDataSize!=sizeof(CMD_C_OneStart)) return NcaTextOut(__LINE__, pIServerUserItem);

			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_WIAT);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_WIAT) return NcaTextOut(__LINE__, pIServerUserItem);

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return NcaTextOut(__LINE__, pIServerUserItem);
			//��������
			CMD_C_OneStart * pOneStart=(CMD_C_OneStart *)pData;

			CString str4;
			str4.Format(_T("Ѻ�߿�ʼ��OneStart->lBet:%d,pOneStart->lBetCount:%d"),pOneStart->lBet,pOneStart->lBetCount);
			CTraceService::TraceString(str4, TraceLevel_Warning);

			return OnUserOneStart(pIServerUserItem, (pOneStart->lBet)-1, pOneStart->lBetCount);
												
		}
	case SUB_C_ONE_END:					//����Ѻ��
		{
			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GAME_SCENE_ONE || m_pITableFrame->GetGameStatus()==GAME_SCENE_WIAT);
			if (m_pITableFrame->GetGameStatus()!=GAME_SCENE_ONE && m_pITableFrame->GetGameStatus()!=GAME_SCENE_WIAT) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			return OnUserGameEnd(pIServerUserItem);
		}

	case SUB_C_SMALL_START:		 //С��Ϸ״̬
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_C_SmallStatus));
			if (wDataSize != sizeof(CMD_C_SmallStatus)) return NcaTextOut(__LINE__, pIServerUserItem);

			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus() == GAME_SMALL_GAME);
			if (m_pITableFrame->GetGameStatus() != GAME_SMALL_GAME) return true;
			
			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus() == US_PLAYING);
			if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

			//��������
			CMD_C_SmallStatus * pSmallStart = (CMD_C_SmallStatus *)pData;

			return OnUserSmallGameStart(pIServerUserItem, pSmallStart->cbCustoms, pSmallStart->SmallSorce);
		}
	case SUB_C_LOOK_RECORD:				//�н���¼
		{
			return OnUserWinningRecord(pIServerUserItem);
		}
	case SUB_C_THREE_END:					//����С��Ϸ
		{
			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus() == GAME_SMALL_GAME);
			if (m_pITableFrame->GetGameStatus() != GAME_SMALL_GAME) return true;

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()==US_PLAYING);
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			CString str4;
			str4.Format(_T("С��Ϸ����"));
			CTraceService::TraceString(str4, TraceLevel_Warning);

			return OnUserGameEnd(pIServerUserItem);
		}
	case SUB_C_UPDATE_TABLE_STORAGE:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UpdateStorageTable));
			if (wDataSize != sizeof(CMD_C_UpdateStorageTable)) 
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false)
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//��������
			CMD_C_UpdateStorageTable *pUpdateStorage=(CMD_C_UpdateStorageTable *)pData;

			WORD wTableID = m_pITableFrame->GetTableID();
			SCORE lStockScore = m_lCurStorage;
			m_lCurStorage = pUpdateStorage->lStorageTable;
			m_lStorageInputTable += pUpdateStorage->lStorageTable - lStockScore;

			//20��������¼
			if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				m_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("����ʱ��: %04d/%02d/%02d-%02d:%02d:%02d, �����˻�[%s],�޸ĵ�ǰ���Ϊ %I64d,��ǰ����ԭ���Ϊ %I64d"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), pUpdateStorage->lStorageTable, lStockScore);

			m_ListOperationRecord.AddTail(strOperationRecord);

			//д����־
			strOperationRecord += TEXT("\n");
			WriteInfo(strOperationRecord);

			//��������
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();

			//�����±�
			WORD wIndex = 0;
			while(posListRecord)
			{
				CString strRecord = m_ListOperationRecord.GetNext(posListRecord);

				strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
				_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
				strRecord.ReleaseBuffer();
				OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			//ˢ�·�����
			CMD_S_UpdateRoomStorage UpdateRoomStorage;
			ZeroMemory(&UpdateRoomStorage, sizeof(UpdateRoomStorage));
			UpdateRoomStorage.lRoomStorageStart = m_lStorageStartTable;
			UpdateRoomStorage.lRoomStorageCurrent = m_lCurStorage;
			UpdateRoomStorage.lRoomStorageInput = m_lStorageInputTable;
			UpdateRoomStorage.lRoomStockRecoverScore = m_lRoomStockRecoverScore;

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));

			return true;
		}
	case SUB_C_MODIFY_ROOM_CONFIG:
		{
			ASSERT(wDataSize == sizeof(CMD_C_ModifyRoomConfig));
			if (wDataSize != sizeof(CMD_C_ModifyRoomConfig))
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false)
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}
			
			CMD_C_ModifyRoomConfig *pModifyRoomConfig = (CMD_C_ModifyRoomConfig *)pData;

			if(pModifyRoomConfig->wGameTwo > 100 && pModifyRoomConfig->wGameTwo != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wGameTwoDeduct >= 100 && pModifyRoomConfig->wGameTwoDeduct != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wGameThree > 100 && pModifyRoomConfig->wGameThree != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wGameThreeDeduct >= 100 && pModifyRoomConfig->wGameThreeDeduct != INVALID_WORD ) return NcaTextOut(__LINE__, pIServerUserItem);

			if(pModifyRoomConfig->wStorageMulRoom[0] > 100) return NcaTextOut(__LINE__, pIServerUserItem);
			if(pModifyRoomConfig->wStorageMulRoom[1] > 100) return NcaTextOut(__LINE__, pIServerUserItem);			

			//��������
			m_lStorageMax1Room = pModifyRoomConfig->lMaxStorageRoom[0];
			m_lStorageMax2Room = pModifyRoomConfig->lMaxStorageRoom[1];
			m_lStorageMul1Room = (SCORE)(pModifyRoomConfig->wStorageMulRoom[0]);
			m_lStorageMul2Room = (SCORE)(pModifyRoomConfig->wStorageMulRoom[1]);
			m_lStorageDeductRoom = /*pModifyRoomConfig->lStorageDeductRoom*/0;
			m_wGameTwo = pModifyRoomConfig->wGameTwo;
			m_wGameTwoDeduct = pModifyRoomConfig->wGameTwoDeduct;
			m_wGameThree = pModifyRoomConfig->wGameThree;
			m_wGameThreeDeduct = pModifyRoomConfig->wGameThreeDeduct;

			//20��������¼
			if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				m_ListOperationRecord.RemoveHead();
			}

			CString strOperationRecord;
			CTime time = CTime::GetCurrentTime();
			strOperationRecord.Format(TEXT("����ʱ��: %04d/%02d/%02d-%02d:%02d:%02d, �����˻�[%s], �������ֵ1Ϊ %I64d,Ӯ�ָ���1Ϊ %I64d,����ֵ2Ϊ %I64d,Ӯ�ָ���2Ϊ %I64d, %s, %s"),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), m_lStorageMax1Room, m_lStorageMul1Room, m_lStorageMax2Room, m_lStorageMul2Room, GetControlDataString(2), GetControlDataString(3));

			m_ListOperationRecord.AddTail(strOperationRecord);

			//д����־
			strOperationRecord += TEXT("\n");
			WriteInfo(strOperationRecord);

			//��������
			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();

			//�����±�
			WORD wIndex = 0;
			while(posListRecord)
			{
				CString strRecord = m_ListOperationRecord.GetNext(posListRecord);

				strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
				_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
				strRecord.ReleaseBuffer();
				OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
				wIndex++;
			}

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
	case SUB_C_REQUEST_QUERY_USER:
		{
			ASSERT(wDataSize == sizeof(CMD_C_RequestQuery_User));
			if (wDataSize != sizeof(CMD_C_RequestQuery_User)) 
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			CMD_C_RequestQuery_User *pQuery_User = (CMD_C_RequestQuery_User *)pData;

			CMD_S_RequestQueryResult QueryResult;
			ZeroMemory(&QueryResult, sizeof(QueryResult));

			//�����Ѵ��ڵ��û�
			ROOMUSERINFO roomUserInfo;
			ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));
			if (m_MapRoomUserInfo.Lookup(pQuery_User->dwGameID, roomUserInfo))
			{
				//�����û���Ϣ����
				QueryResult.bFind = true;
				CopyMemory(&QueryResult.userinfo, &roomUserInfo, sizeof(QueryResult.userinfo));
			}

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

			return true;
		}
	case SUB_C_USER_CONTROL:
		{
			ASSERT(wDataSize == sizeof(CMD_C_UserControl));
			if (wDataSize != sizeof(CMD_C_UserControl)) 
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			//Ȩ���ж�
			if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			CMD_C_UserControl *pUserControl = (CMD_C_UserControl *)pData;

			//����ӳ��
			POSITION ptMapHead = m_MapRoomUserInfo.GetStartPosition();
			DWORD dwUserID = 0;

			//20��������¼
			if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
			{
				m_ListOperationRecord.RemoveHead();
			}
			
			//������¼
			CString strOperationRecord;
			CString strControlType;

			if (pUserControl->userControlInfo.controlType >= CONTINUE_3 && pUserControl->userControlInfo.controlType <= CONTINUE_CANCEL &&
				(pUserControl->userControlInfo.cbZhongJiangJian <= 100 || pUserControl->userControlInfo.cbZhongJiangJian == 255) &&
				(pUserControl->userControlInfo.cbZhongJiang <= 100 || pUserControl->userControlInfo.cbZhongJiang == 255)) //�������
			{				
				//�����Ѵ��ڵ��û�
				ROOMUSERINFO roomUserInfo;
				ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));
				if (m_MapRoomUserInfo.Lookup(pUserControl->dwGameID, roomUserInfo))
				{
					roomUserInfo.UserContorl.cbControlCount = pUserControl->userControlInfo.cbControlCount;
					roomUserInfo.UserContorl.controlType = pUserControl->userControlInfo.controlType;	
					roomUserInfo.UserContorl.cbControlData = pUserControl->userControlInfo.cbControlData;	
					roomUserInfo.UserContorl.cbZhongJiang = pUserControl->userControlInfo.cbZhongJiang;	
					roomUserInfo.UserContorl.cbZhongJiangJian = pUserControl->userControlInfo.cbZhongJiangJian;	

					if(pUserControl->userControlInfo.controlType == CONTINUE_CANCEL)
					{
						roomUserInfo.UserContorl.cbControlCount = 0;
					}
					
					m_MapRoomUserInfo.SetAt(pUserControl->dwGameID, roomUserInfo);
					GetControlTypeString(pUserControl->userControlInfo, strControlType);
					CTime time = CTime::GetCurrentTime();
					strOperationRecord.Format(TEXT("����ʱ��: %04d/%02d/%02d-%02d:%02d:%02d, �����˻�[%s], �������[%s],%s "),
						time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), roomUserInfo.szNickName, strControlType);

					//��������
					CMD_S_UserControl serverUserControl;
					ZeroMemory(&serverUserControl, sizeof(serverUserControl));
					serverUserControl.dwGameID = roomUserInfo.dwGameID;
					CopyMemory(serverUserControl.szNickName, roomUserInfo.szNickName, sizeof(serverUserControl.szNickName));
					serverUserControl.controlResult = CONTROL_SUCCEED;
					serverUserControl.UserControl = pUserControl->userControlInfo;
					serverUserControl.cbControlCount = pUserControl->userControlInfo.cbControlCount;

					//��������
					m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
					m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
				}
				else
				{
					GetControlTypeString(pUserControl->userControlInfo, strControlType);
					CTime time = CTime::GetCurrentTime();
					strOperationRecord.Format(TEXT("����ʱ��: %04d/%02d/%02d-%02d:%02d:%02d, �����˻�[%s], �������[%s],%s,ʧ�ܣ�"),
						time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), roomUserInfo.szNickName, strControlType);		
				}
			}
			else
			{
				return NcaTextOut(__LINE__, pIServerUserItem);
			}

			m_ListOperationRecord.AddTail(strOperationRecord);

			//д����־
			strOperationRecord += TEXT("\n");
			WriteInfo(strOperationRecord);

			CMD_S_Operation_Record OperationRecord;
			ZeroMemory(&OperationRecord, sizeof(OperationRecord));
			POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();
			WORD wIndex = 0;//�����±�
			try
			{
				CString strRecord;
				while(posListRecord)
				{
					strRecord = m_ListOperationRecord.GetNext(posListRecord);
					strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
					_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
					strRecord.ReleaseBuffer();
					OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
					wIndex++;
				}
			}
			catch(...)
			{
				DWORD w = GetLastError();
				ASSERT(NULL);
			}
			

			ASSERT(wIndex <= MAX_OPERATION_RECORD);

			//��������
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

			return true;
		}
		case SUB_C_REQUEST_UDPATE_ROOMUSERLIST:
			{
				//Ȩ���ж�
				if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				//����ӳ��
				POSITION ptHead = m_MapRoomUserInfo.GetStartPosition();
				DWORD dwUserID = 0;
				ROOMUSERINFO KeyUserInfo;
				ZeroMemory(&KeyUserInfo, sizeof(KeyUserInfo));

				CMD_S_UpdateRoomUserList UpdateRoomUserList;

				while(ptHead)
				{
					m_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, KeyUserInfo);

					ZeroMemory(&UpdateRoomUserList, sizeof(UpdateRoomUserList));
					CopyMemory(&(UpdateRoomUserList.roomUserInfo), &KeyUserInfo, sizeof(KeyUserInfo));
					UpdateRoomUserList.dwUserID = dwUserID;

					//��������
					m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOMUSERLIST, &UpdateRoomUserList, sizeof(UpdateRoomUserList));
					m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOMUSERLIST, &UpdateRoomUserList, sizeof(UpdateRoomUserList));
				}

				return true;
			}
		case SUB_C_REQUEST_UDPATE_ROOMSTORAGE:
			{
				//Ȩ���ж�
				if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				//ˢ�·�����
				CMD_S_UpdateRoomStorage UpdateRoomStorage;
				ZeroMemory(&UpdateRoomStorage, sizeof(UpdateRoomStorage));
				UpdateRoomStorage.lRoomStorageStart = m_lStorageStartTable;
				UpdateRoomStorage.lRoomStorageCurrent = m_lCurStorage;
				UpdateRoomStorage.lRoomStorageInput = m_lStorageInputTable;
				UpdateRoomStorage.lRoomStockRecoverScore = m_lRoomStockRecoverScore;

				//��������
				m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));
				m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_UPDATE_ROOM_STORAGE, &UpdateRoomStorage, sizeof(UpdateRoomStorage));

				return true;
			}
		case SUB_C_SINGLEMODE_CONFIRM:
			{	
				ASSERT(wDataSize == sizeof(CMD_C_SingleMode));
				if (wDataSize != sizeof(CMD_C_SingleMode)) 
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				//Ȩ���ж�
				if (CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
				{
					return NcaTextOut(__LINE__, pIServerUserItem);
				}

				
				CMD_C_SingleMode *pSingleMode = (CMD_C_SingleMode *)pData;
				WORD wTableID = pSingleMode->wTableID;

				SCORE lStockScore = m_lCurStorage;
				m_lCurStorage = pSingleMode->lTableStorage;
				m_lStorageInputTable += pSingleMode->lTableStorage - lStockScore;

				//20��������¼
				if (m_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
				{
					m_ListOperationRecord.RemoveHead();
				}

				CString strOperationRecord;
				CTime time = CTime::GetCurrentTime();
				strOperationRecord.Format(TEXT("����ʱ��: %04d/%02d/%02d-%02d:%02d:%02d, �����˻�[%s],�޸ĵ�[ %d ]�����ӿ��Ϊ %I64d,��ǰ�����ܿ��Ϊ %I64d"),
					time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), pSingleMode->wTableID + 1, pSingleMode->lTableStorage, m_lCurStorage);

				m_ListOperationRecord.AddTail(strOperationRecord);

				//д����־
				strOperationRecord += TEXT("\n");
				WriteInfo(strOperationRecord);

				//��������
				CMD_S_Operation_Record OperationRecord;
				ZeroMemory(&OperationRecord, sizeof(OperationRecord));
				POSITION posListRecord = m_ListOperationRecord.GetHeadPosition();

				//�����±�
				WORD wIndex = 0;
				while(posListRecord)
				{
					CString strRecord = m_ListOperationRecord.GetNext(posListRecord);

					strRecord.Left(CountArray(OperationRecord.szRecord[wIndex]) - 1);
					_tcscpy(OperationRecord.szRecord[wIndex], strRecord.GetBuffer());
					strRecord.ReleaseBuffer();
					OperationRecord.szRecord[wIndex][RECORD_LENGTH - 1] = 0;
					wIndex++;
				}

				ASSERT(wIndex <= MAX_OPERATION_RECORD);

				//��������
				m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
				m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

				return true;
			}
	}

	return NcaTextOut(__LINE__, pIServerUserItem);
}

//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if (m_pITableFrame->GetGameStatus() != GAME_SCENE_FREE)
	{
		m_cbGameMode = GM_NULL;
		OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);
	}
	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true; 
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʷ����
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserEnter(wChairID);


		//���·����û���Ϣ
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��ʷ����
	if (bLookonUser==false)
	{
		ASSERT(wChairID!=INVALID_CHAIR);
		m_HistoryScore.OnEventUserLeave(wChairID);

		//���·����û���Ϣ
		UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

		m_BonusCount = 0;
		ZeroMemory(m_BonusSorce, sizeof(m_BonusSorce));
		ZeroMemory(m_BonusSorceDate, sizeof(m_BonusSorceDate));
		ZeroMemory(m_NickName, sizeof(m_NickName));
	}

	return true;
}

//�û�ͬ��
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	return true;
}

//Ѻ�߿�ʼ
bool CTableFrameSink::OnUserOneStart(IServerUserItem * pIServerUserItem, LONGLONG lBet, LONGLONG lBetCount)
{
	if(lBet < 0 || lBet > m_pGameCustomRule->cbBetCount) return NcaTextOut(__LINE__, pIServerUserItem);


	ASSERT(pIServerUserItem->GetUserScore() >=m_pGameCustomRule->lBetScore[lBet] * lBetCount);
	if (pIServerUserItem->GetUserScore() < m_pGameCustomRule->lBetScore[lBet] * lBetCount) return NcaTextOut(__LINE__, pIServerUserItem);

	m_lBet = lBet;
	m_cbLine = lBetCount;
	m_bGameStart = true;
	//����״̬
	m_pITableFrame->SetGameStatus(GAME_SCENE_ONE);	

	WORD wTableID = m_pITableFrame->GetTableID();

	//m_cbTwoMode = 0xFF;	
	//m_wSingleGameTwo = m_wGameTwo;
	//m_wSingleGameThree = m_wGameThree;

	if(m_lCurStorage > 0 && !pIServerUserItem->IsAndroidUser())
	{
		SCORE lRecoverScore = m_lCurStorage*m_lStorageDeductRoom/1000;
		m_lCurStorage -= lRecoverScore;
		m_lRoomStockRecoverScore += lRecoverScore;
	}

	if(!pIServerUserItem->IsAndroidUser())
	{
		CString str;
		str.Format(TEXT("��%d���� ��ǰ��� %I64d  %s[%ld]"), m_pITableFrame->GetTableID() + 1, m_lCurStorage, pIServerUserItem->GetNickName(),pIServerUserItem->GetGameID());
		NcaTextOut(str, m_pGameServiceOption->szServerName);
	}

	//���·����û���Ϣ
	UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

	//���¿���
	ROOMUSERINFO roomUserInfo;
	bool bWin = UpdateControl(roomUserInfo);

	//roomUserInfo.UserContorl.controlType = CONTINUE_3;
	//roomUserInfo.UserContorl.cbControlCount = 10;
	//roomUserInfo.UserContorl.cbControlData = 8;
	//bWin = true;

	//�����˿�
	double dControl = m_lStorageMulRoom;

	m_lBetScore = -m_pGameCustomRule->lBetScore[m_lBet] * m_cbLine;

	bool bControl = false;
	if(bWin && m_pServerControl != NULL)
	{		
		if(m_pServerControl->ServerControl(roomUserInfo.UserContorl, m_cbItemInfo))
		{
			m_lOneGameScore = m_GameLogic.GetZhongJiangTime(m_cbLine,m_cbItemInfo,2) * m_pGameCustomRule->lBetScore[(LONGLONG)m_lBet];
			bControl = true;
		}
	}
	if(!bControl)
	{ 
		m_GameLogic.RandCardList(m_cbItemInfo,dControl);
		m_lOneGameScore = m_GameLogic.GetZhongJiangTime(m_cbLine,m_cbItemInfo,2) * m_pGameCustomRule->lBetScore[m_lBet];

		while(m_lCurStorage - m_lOneGameScore < 0 && m_lOneGameScore > 0 && !pIServerUserItem->IsAndroidUser())
		{
			dControl /= 1.2;
			m_GameLogic.RandCardList(m_cbItemInfo,dControl);
			m_lOneGameScore = m_GameLogic.GetZhongJiangTime(m_cbLine, m_cbItemInfo, 2) * m_pGameCustomRule->lBetScore[m_lBet];
		}
	} 

	CString str;
	str.Format(_T("Ѻ������%lld:ѹ�߷�%lld:��Ѻ�߷�%lld:Ѻ�ߵ÷�%lld "), m_cbLine, m_lBet+1, m_lBetScore, m_lOneGameScore);
	CTraceService::TraceString(str, TraceLevel_Warning);

	//�ж��Ƿ��н�(���Ѻע)
	if (m_cbLine==9 && m_lBet==8)
	{
		bool Winning = false;
		Winning = DetermineTheWinning(m_cbItemInfo);
		if (Winning)
		{
			m_BonusGetSorce = WinningSocre(pIServerUserItem);
		}
	}
	//��������7 ����С��Ϸ
	bool IsSmallGame = false;
	IsSmallGame = EntranceSmallGame(m_cbItemInfo);

	IsSmallGame = false;//�ر�С��Ϸ
	if (IsSmallGame)
	{
		m_pITableFrame->SetGameStatus(GAME_SMALL_GAME);
		m_cbGameMode = GM_SMALL;

		//���·����û���Ϣ
		UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
	}
	else
	{
		m_cbGameMode = GM_NULL;
	}

	//�������
	CMD_S_GameStart GameStart;
	GameStart.lScore = m_lOneGameScore;
	GameStart.cbGameMode = m_cbGameMode;
	GameStart.cbBouns = m_Bonus;
	CopyMemory(GameStart.cbItemInfo, m_cbItemInfo, sizeof(GameStart.cbItemInfo));

	//BYTE m_cbItemInfoTemp[ITEM_Y_COUNT][ITEM_X_COUNT];	//������Ϣ
	//for (int i = 0; i < ITEM_Y_COUNT; i++)
	//{
	//	for (int j = 0; j < ITEM_X_COUNT; j++)
	//	{
	//		m_cbItemInfoTemp[i][j] = 1;
	//	}
	//}
	//CopyMemory(GameStart.cbItemInfo, m_cbItemInfoTemp, sizeof(GameStart.cbItemInfo));

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));


	if (m_cbGameMode == GM_SMALL)
	//{
	//	OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);
	//}
	//else
	{
		//��ȡС��Ϸ����
		OnUserSmallGameStartBefore();

		CMD_C_SmallStart SmallGameStart;
		SmallGameStart.cbGameMode = m_cbGameMode;
		CopyMemory(SmallGameStart.SamllGameFruitData, SamllGameFruit, sizeof(SmallGameStart.SamllGameFruitData));


		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SMALL_START, &SmallGameStart, sizeof(SmallGameStart));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SMALL_START, &SmallGameStart, sizeof(SmallGameStart));
	}
	return true;
}

//�Ƿ����С��Ϸ
bool CTableFrameSink::EntranceSmallGame(BYTE cbCardBuffer[ITEM_Y_COUNT][ITEM_X_COUNT])
{
	BYTE  SameLine = 0;
	BYTE  cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT];	//������Ϣ
	CopyMemory(cbItemInfo, cbCardBuffer, sizeof(cbItemInfo));
	
	//�Ƿ��������
	bool OneBonus = false;
	bool TwoBonus = false;
	bool ThereBonus = false;
	bool FourBonus = false;
	bool FiveBonus = false;

	//��������7 ����С��Ϸ
	for (BYTE i = 0; i < ITEM_X_COUNT; i++)
	{
		if (i == 0)
		{
			for (BYTE j = 0; j < ITEM_Y_COUNT; j++)
			{
				if (cbItemInfo[j][i] == CT_SHUIHUZHUAN)
				{
					OneBonus = true;
					break;
				}
			}
		}
		if (i == 1)
		{
			for (BYTE j = 0; j < ITEM_Y_COUNT; j++)
			{
				if (cbItemInfo[j][i] == CT_SHUIHUZHUAN)
				{
					TwoBonus = true;
					break;
				}
			}
		}
		if (i == 2)
		{
			for (BYTE j = 0; j < ITEM_Y_COUNT; j++)
			{
				if (cbItemInfo[j][i] == CT_SHUIHUZHUAN)
				{
					ThereBonus = true;
					break;
				}
			}
		}
		if (i == 3)
		{
			for (BYTE j = 0; j < ITEM_Y_COUNT; j++)
			{
				if (cbItemInfo[j][i] == CT_SHUIHUZHUAN)
				{
					FourBonus = true;
					break;
				}
			}
		}
		if (i == 4)
		{
			for (BYTE j = 0; j < ITEM_Y_COUNT; j++)
			{
				if (cbItemInfo[j][i] == CT_SHUIHUZHUAN)
				{
					FiveBonus = true;
					break;
				}
			}
		}
	}

	if (OneBonus && TwoBonus && ThereBonus)
	{
		return true;
	}
	else if (TwoBonus && ThereBonus && FourBonus)
	{
		return true;
	}
	else if (ThereBonus && FourBonus && FiveBonus)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//С��Ϸ
bool CTableFrameSink::OnUserSmallGameStart(IServerUserItem * pIServerUserItem, BYTE cbCustoms,LONGLONG  SmallSorce)
{
	ASSERT(cbCustoms);
	if (cbCustoms < 0 || cbCustoms > 4) return NcaTextOut(__LINE__, pIServerUserItem);

	if (SmallSorce)
	{
		m_cbALLSmallSorce += SmallSorce;
	}
	//С��Ϸ�ؿ�
	m_cbCustoms = cbCustoms;

	return true;
}

//����bonus����С��Ϸ
bool  CTableFrameSink::OnUserSmallGameStartBefore()
{
	//���ö�ʱ��
	m_tCreateTime = CTime::GetCurrentTime();

	//������
	if (m_pGameServiceOption->wServerLevel == 2)
	{
		for (BYTE i = 0; i < 4; i++)
		{
			if (i == 0)
			{
				//��һ�ط��� ��ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = FirstSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 1)
			{
				//�ڶ��ط���  һ��ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = SecondSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 2)
			{
				//�����ط��� ����ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = ThirdSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 3)
			{
				//���Ĺط��� ����ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = FourthSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
		}
	}
	//�м���
	else if (m_pGameServiceOption->wServerLevel == 3)
	{
		for (BYTE i = 0; i < 4; i++)
		{
			if (i == 0)
			{
				//��һ�ط��� ��ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = FirstSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 1)
			{
				//�ڶ��ط���  һ��ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = SecondSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 2)
			{
				//�����ط��� ����ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = ThirdSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 3)
			{
				//���Ĺط��� ����ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = FourthSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
		}
	}
	//�߼���
	else if (m_pGameServiceOption->wServerLevel == 4)
	{
		for (BYTE i = 0; i < 4; i++)
		{
			if (i == 0)
			{
				//��һ�ط��� ��ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = FirstSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 1)
			{
				//�ڶ��ط���  һ��ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = SecondSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 2)
			{
				//�����ط��� ����ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = ThirdSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i==3)
			{
				//���Ĺط��� ����ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = FourthSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
		}
	}
	//�����
	else if (m_pGameServiceOption->wServerLevel == 5)
	{
		for (BYTE i = 0; i < 4; i++)
		{
			if (i == 0)
			{
				//��һ�ط��� ��ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = FirstSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 1)
			{
				//�ڶ��ط���  һ��ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = SecondSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 2)
			{
				//�����ط��� ����ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = ThirdSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 3)
			{
				//���Ĺط��� ����ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = FourthSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
		}
	}
	//�߼���
	else
	{
		for (BYTE i = 0; i < 4; i++)
		{
			if (i == 0)
			{
				//��һ�ط��� ��ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = FirstSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 1)
			{
				//�ڶ��ط���  һ��ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = SecondSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 2)
			{
				//�����ط��� ����ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = ThirdSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
			else if (i == 3)
			{
				//���Ĺط��� ����ը��
				bool   HaveTakeOut[5] = { 0 };
				for (BYTE j = 0; j < 5; j++)
				{
					BYTE TakeCount = rand() % 5;
					if (HaveTakeOut[TakeCount] == false)
					{
						HaveTakeOut[TakeCount] = true;
						SamllGameFruit[i][j] = FourthSorce[TakeCount];
					}
					else
					{
						j--;
					}
				}
			}
		}
	}
	return true;
}

//��Ϸ����
bool CTableFrameSink::OnUserGameEnd(IServerUserItem * pIServerUserItem)
{
	m_cbGameMode = GM_NULL;
	OnEventGameConclude(0, m_pITableFrame->GetTableUserItem(0), GER_NORMAL);

	return true;
}

//�ж��Ƿ��н�
bool CTableFrameSink::DetermineTheWinning(BYTE cbCardBuffer[ITEM_Y_COUNT][ITEM_X_COUNT])
{
	BYTE HaveSevenCount = 0;
	//����߻�ý���
	for (int i = 0; i < ITEM_Y_COUNT; i++)
	{
		for (int j = 0; j < ITEM_X_COUNT; j++)
		{
			if (cbCardBuffer[i][j] == CT_SHUIHUZHUAN2)
			{
				HaveSevenCount += 1;
			}
		}
	}
	if (HaveSevenCount > 4)
	{
		return true;
	}
	return false;
}

//�н��÷�
LONGLONG CTableFrameSink::WinningSocre(IServerUserItem * pIServerUserItem)
{
	//����¼ʮ������
	if (m_BonusCount <= 10)
	{
		//���ص÷�
		SYSTEMTIME BonusSorcest = { 0 };
		GetLocalTime(&BonusSorcest);
		m_BonusSorceDate[m_BonusCount] = BonusSorcest;
		m_BonusSorce[m_BonusCount] = m_Bonus * 0.2;
		lstrcpyn(m_NickName[m_BonusCount], pIServerUserItem->GetNickName(), CountArray(m_NickName[m_BonusCount]));
		m_BonusCount++;

		//���ÿ۳�����֮��Ľ���
		m_Bonus -= m_BonusSorce[m_BonusCount - 1];
		return m_BonusSorce[m_BonusCount - 1];
	}
	else
	{	
		BYTE index = (m_BonusCount % 10) - 1; 
		m_BonusCount++;
		//���ص÷�
		SYSTEMTIME BonusSorcest = { 0 };
		GetLocalTime(&BonusSorcest);
		m_BonusSorceDate[index] = BonusSorcest;
		m_BonusSorce[index] = m_Bonus * 0.2;
		lstrcpyn(m_NickName[index], pIServerUserItem->GetNickName(), CountArray(m_NickName[index]));

		//���ÿ۳�����֮��Ľ���
		m_Bonus -= m_BonusSorce[index];
		return m_BonusSorce[index];
	}
}

//���ؽ��ؼ�¼
bool CTableFrameSink::OnUserWinningRecord(IServerUserItem * pIServerUserItem)
{

	CMD_S_WinningRecord  WinningRecord = { 0 };
	WinningRecord.BonusCount = (m_BonusCount >10 ? 10 : m_BonusCount);
	CopyMemory(WinningRecord.NickName, m_NickName, sizeof(WinningRecord.NickName));
	CopyMemory(WinningRecord.BonusSorce, m_BonusSorce, sizeof(WinningRecord.BonusSorce));
	CopyMemory(WinningRecord.BonusSorceDate, m_BonusSorceDate, sizeof(WinningRecord.BonusSorceDate));
	m_pITableFrame->SendTableData(0, SUB_S_THREE_KAI_JIAN, &WinningRecord, sizeof(WinningRecord));
	m_pITableFrame->SendLookonData(0, SUB_S_THREE_KAI_JIAN, &WinningRecord, sizeof(WinningRecord));

	////����
	//CMD_S_WinningRecord  WinningRecord;
	//WinningRecord.BonusCount = 10;
	//SCORE m_BonusSorce[10] = { 1,2,3, 4, 5, 6, 7, 8, 9 ,10};
	//SYSTEMTIME	  m_BonusSorceDate[10];
	//TCHAR		  m_NickName[10][LEN_NICKNAME];
	//SYSTEMTIME    BonusSorcest = { 0 };
	//GetLocalTime(&BonusSorcest);
	//for (BYTE i = 0; i < 10; i++)
	//{
	//	m_BonusSorceDate[i] = BonusSorcest;
	//	lstrcpyn(m_NickName[i], pIServerUserItem->GetNickName(), CountArray(m_NickName[m_BonusCount]));
	//}
	//CopyMemory(WinningRecord.NickName, m_NickName, sizeof(WinningRecord.NickName));
	//CopyMemory(WinningRecord.BonusSorce, m_BonusSorce, sizeof(m_BonusSorce));
	//CopyMemory(WinningRecord.BonusSorceDate, m_BonusSorceDate, sizeof(m_BonusSorceDate));
	//m_pITableFrame->SendTableData(0, SUB_S_THREE_KAI_JIAN, &WinningRecord, sizeof(WinningRecord));
	//m_pITableFrame->SendLookonData(0, SUB_S_THREE_KAI_JIAN, &WinningRecord, sizeof(WinningRecord));

	return true;
}

void CTableFrameSink::SetGameBaseScore(DOUBLE lBaseScore)
{
	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SET_BASESCORE,&lBaseScore,sizeof(lBaseScore));
}

//д��־�ļ�
void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{
	static CCriticalSection g_criSection;
	g_criSection.Lock();//������ʹ�õ���Դ����
	try
	{
		//������������
		char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
		setlocale(LC_CTYPE, "chs");

		CStdioFile myFile;
		CreateDirectory(GetFileDialogPath() + TEXT("\\Log"), NULL);
		CString strFileName;
		strFileName.Format(TEXT("%s\\Log\\ˮ������[%s]����.log"), GetFileDialogPath(), m_pGameServiceOption->szServerName);
		BOOL bOpen = myFile.Open(strFileName, CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate);

		if (bOpen)
		{	
			myFile.SeekToEnd();
			myFile.WriteString(pszString);
			myFile.Flush();
			myFile.Close();
		}

		//��ԭ�����趨
		setlocale(LC_CTYPE, old_locale);
		free(old_locale);
	}
	catch(...)
	{
		DWORD w = GetLastError();
		ASSERT(NULL);
	}
	g_criSection.Unlock(); //��ʹ�õ���Դ����
}

//���·����û���Ϣ
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction, LONGLONG lGameScore)
{
	if(pIServerUserItem->IsAndroidUser()) return;

	//��������
	ROOMUSERINFO roomUserInfo;
	ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

	roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
	CopyMemory(roomUserInfo.szNickName, pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
	roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
	roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

	roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

	//�û����º�����
	if (userAction == USER_SITDOWN || userAction == USER_RECONNECT)
	{
		roomUserInfo.wChairID = pIServerUserItem->GetChairID();
		roomUserInfo.wTableID = pIServerUserItem->GetTableID();
	}
	else if (userAction == USER_STANDUP || userAction == USER_OFFLINE)
	{
		roomUserInfo.wChairID = INVALID_CHAIR;
		roomUserInfo.wTableID = INVALID_TABLE;
	}

	//�����Ѵ��ڵ��û�
	ROOMUSERINFO lastroomUserInfo;
	ZeroMemory(&lastroomUserInfo, sizeof(lastroomUserInfo));
	if (m_MapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), lastroomUserInfo))
	{
		roomUserInfo.lGameScore = lastroomUserInfo.lGameScore + lGameScore;
		roomUserInfo.UserContorl = lastroomUserInfo.UserContorl;
	}

	m_MapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
}

//���¿���
bool CTableFrameSink::UpdateControl(ROOMUSERINFO &roomUserInfo)
{
	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(0);

	m_lStorageMulRoom = 0;
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL)
	{		
		return NcaTextOut(__LINE__, pIServerUserItem);
	}
	
	//�����Ѵ��ڵ��û�	
	if (m_MapRoomUserInfo.Lookup(pIServerUserItem->GetGameID(), roomUserInfo))
	{		
		if(roomUserInfo.UserContorl.cbControlCount > 0)
		{
			roomUserInfo.UserContorl.cbControlCount--;
			m_MapRoomUserInfo.SetAt(pIServerUserItem->GetGameID(), roomUserInfo);
			if(roomUserInfo.UserContorl.controlType != CONTINUE_LOST)
			{
				if (m_lCurStorage > m_lStorageMax2Room)
				{
					m_lStorageMulRoom = m_lStorageMul2Room * 2;
				}
				else 
				{
					m_lStorageMulRoom = m_lStorageMul1Room * 2;
				}
				return true;
			}
			else
			{
				m_lStorageMulRoom = 0;
			}
			return NcaTextOut(__LINE__, pIServerUserItem);
		}
	}

	if (m_lCurStorage > m_lStorageMax2Room)
	{
		m_lStorageMulRoom = m_lStorageMul2Room;
	}
	else if (m_lCurStorage > 0)
	{
		if(m_lStorageMax1Room>m_lCurStorage && m_lStorageMax1Room!=0)	
			m_lStorageMulRoom = m_lStorageMul1Room*m_lCurStorage/m_lStorageMax1Room;
		else 
			m_lStorageMulRoom = m_lStorageMul1Room;
			
	}

	return NcaTextOut(__LINE__, pIServerUserItem);
}

//��ȡ��������
void CTableFrameSink::GetControlTypeString(USERCONTROL &UserControl, CString &controlTypestr)
{
	
	switch(UserControl.controlType)
	{
	case CONTINUE_3:
		{
			controlTypestr.Format(TEXT("��������Ϊ %d�� 3�� %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_4:
		{
			controlTypestr.Format(TEXT("��������Ϊ  %d��4�� %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_5:
		{
			controlTypestr.Format(TEXT("��������Ϊ %d�� 5�� %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_ALL:
		{
			controlTypestr.Format(TEXT("��������Ϊ %d�� ȫ�� %s"), UserControl.cbControlCount, GetControlDataString(UserControl));
			break;
		}
	case CONTINUE_LOST:
		{
			controlTypestr.Format(TEXT("��������Ϊ %d�ֲ���"), UserControl.cbControlCount);
			break;
		}
	case CONTINUE_CANCEL:
		{
			controlTypestr = TEXT("��������Ϊȡ������");
			break;
		}
	}
}

//��ȡ��������
CString CTableFrameSink::GetControlDataString(USERCONTROL &UserControl)
{
	CString str;

	switch(UserControl.cbControlData)
	{
	case 0:
		{
			str = TEXT("�㽶");
			break;
		}
	case 1:
		{
			str = TEXT("����");
			break;
		}
	case 2:
		{
			str = TEXT("â��");
			break;
		}
	case 3:
		{
			str = TEXT("����");
			break;
		}
	case 4:
		{
			str = TEXT("����");
			break;
		}
	case 5:
		{
			str = TEXT("����");
			break;
		}
	case 6:
		{
			str = TEXT("ӣ��");
			break;
		}
	case 7:
		{
			str = TEXT("�ư�");
			break;
		}
	case 8:
		{
			str = TEXT("����");
			break;
		}
	case 9:
		{
			str = TEXT("7��");
			break;
		}
	case 10:
		{
			str = TEXT("�ȱ�");
			break;
		}
	}

	CString strTemp;
	if(UserControl.cbZhongJiang == 255)
	{
		if(UserControl.cbControlData == 8)
		{
			str += TEXT("��������С����");
		}
		else
		{
			str += TEXT("�������Ʊȱ�");
		}
	}
	else
	{
		if(UserControl.cbControlData == 8)
		{
			strTemp.Format(TEXT("��С�����н���%d"), UserControl.cbZhongJiang);
		}
		else
		{
			strTemp.Format(TEXT("���ȱ��н���%d"), UserControl.cbZhongJiang);
		}

		str += strTemp;
		if(UserControl.cbZhongJiangJian == 255)
		{
			str += TEXT("����2��������н�");
		}
		else
		{
			strTemp.Format(TEXT("��˥������%d"), UserControl.cbZhongJiangJian);
			str += strTemp;
		}
	}
	return str;
}

//��ȡ��������
CString CTableFrameSink::GetControlDataString(BYTE cbGameMode)
{
	CString str;

	if(cbGameMode == 2)
	{
		if(m_wGameTwo == INVALID_WORD)
		{
			str = TEXT("�����Ʊȱ�");
		}
		else
		{
			if(m_wGameTwoDeduct == INVALID_WORD)
			{
				str.Format(TEXT("�ȱ��н�����Ϊ %d �ڶ����𲻿���"), m_wGameTwo);
			}
			else
			{
				str.Format(TEXT("�ȱ��н�����Ϊ %d ˥��ֵΪ %d"), m_wGameTwo, m_wGameTwoDeduct);
			}
		}
	}
	else if(cbGameMode == 3)
	{
		if(m_wGameThree == INVALID_WORD)
		{
			str = TEXT("������С����");
		}
		else
		{
			if(m_wGameThreeDeduct == INVALID_WORD)
			{
				str.Format(TEXT("С�����н�����Ϊ %d �ڶ����𲻿���"), m_wGameThree);
			}
			else
			{
				str.Format(TEXT("С�����н�����Ϊ %d ˥��ֵΪ %d"), m_wGameThree, m_wGameThreeDeduct);
			}
		}
	}
	return str;
}
//////////////////////////////////////////////////////////////////////////////////
