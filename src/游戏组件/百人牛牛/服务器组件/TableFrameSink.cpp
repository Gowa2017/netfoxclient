#include "StdAfx.h"
#include "DlgCustomRule.h"
#include "TableFrameSink.h"
#include <locale>

//////////////////////////////////////////////////////////////////////////////////
//��������
#define SEND_COUNT          100                 //���ʹ���

//��������
#define INDEX_PLAYER        0                 //�м�����
#define INDEX_BANKER        1                 //ׯ������

//ʱ�䶨��
#define IDI_FREE          1                 //����ʱ��
#define IDI_PLACE_JETTON      2                 //��עʱ��
#define IDI_GAME_END        3                 //����ʱ��

#define TIME_FREE         5                 //����ʱ��
#define TIME_PLACE_JETTON     15                  //��עʱ��
#define TIME_GAME_END       20                  //����ʱ��
//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD      CTableFrameSink::m_wPlayerCount=GAME_PLAYER;        //��Ϸ����

void Debug(char *text,...)
{
  static DWORD num=0;
  char buf[1024];
  FILE *fp=NULL;
  va_list ap;
  va_start(ap,text);
  vsprintf(buf,text,ap);
  va_end(ap);
  if(num == 0)
  {
    fp=fopen("����ţţ���.log","w");
  }
  else
  {
    fp=fopen("����ţţ���.log","a");
  }
  if(fp == NULL)
  {
    return ;
  }
  num++;
  SYSTEMTIME time;
  GetLocalTime(&time);
  fprintf(fp, "%d:%s - %d/%d/%d %d:%d:%d \n", num, buf, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
  fclose(fp);
}
//////////////////////////////////////////////////////////////////////////
//���캯��
CTableFrameSink::CTableFrameSink()
{
  //��ʼ����
  ZeroMemory(m_lUserStartScore,sizeof(m_lUserStartScore));

  //����ע��
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));
  ZeroMemory(m_lbakAllJettonScore,sizeof(m_lbakAllJettonScore));
  //������ע
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));
  m_PeizhiUserArray.RemoveAll();
  //���Ʊ���
  m_cbWinSideControl=0;
  m_nSendCardCount=0;
  m_bControl=false;

  for (int i =0 ; i< MAX_OCCUPY_SEAT_COUNT;i++)
  {
		m_wOccupySeatChairID[i] = -1;
  }
  //��ҳɼ�
  ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
  ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
  ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

  //�˿���Ϣ
  ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
  ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

  //״̬����
  m_dwJettonTime=0L;
  m_bExchangeBanker=true;
  m_wAddTime=0;
  CopyMemory(m_szControlName,TEXT("����"),sizeof(m_szControlName));

  //ׯ����Ϣ
  m_ApplyUserArray.RemoveAll();
  m_wCurrentBanker=INVALID_CHAIR;
  m_wBankerTime=0;
  m_lBankerWinScore=0L;
  m_lBankerCurGameScore=0L;
  m_bEnableSysBanker=true;
  m_cbLeftCardCount=0;
  m_bContiueCard=false;
 m_bAndroidXiahzuang = false;
  //��¼����
  ZeroMemory(m_GameRecordArrary,sizeof(m_GameRecordArrary));
  //��¼����
  ZeroMemory(&m_OccUpYesAtconfig,sizeof(m_OccUpYesAtconfig));

  m_nRecordFirst=0;
  m_nRecordLast=0;
  m_dwRecordCount=0;

  //�����˿���
  m_nChipRobotCount = 0;
  m_nRobotListMaxCount =0;
  ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

  //�������
  m_hInst = NULL;
  m_pServerContro = NULL;
  m_hInst = LoadLibrary(TEXT("OxBattleServerControl.dll"));
  if(m_hInst)
  {
    typedef void * (*CREATE)();
    CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl");
    if(ServerControl)
    {
      m_pServerContro = static_cast<IServerControl*>(ServerControl());
    }
  }

  return;
}

//��������
CTableFrameSink::~CTableFrameSink()
{
}

VOID CTableFrameSink::Release()
{
  //if( m_pServerContro )
  //{
  //  delete m_pServerContro;
  //  m_pServerContro = NULL;
  //}

  if(m_hInst)
  {
    FreeLibrary(m_hInst);
    m_hInst = NULL;
  }
  delete this;
}
//�ӿڲ�ѯ
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
  QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
  return NULL;
}

//��������
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
  //��ѯ�ӿ�
  m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

  //�����ж�
  if(m_pITableFrame==NULL)
  {
    CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"),TraceLevel_Exception);
    return false;
  }

  //��ʼģʽ
  m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

  m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
  m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();

  ASSERT(m_pGameServiceOption!=NULL);
  ASSERT(m_pGameServiceAttrib!=NULL);
  //�����ļ���
  TCHAR szPath[MAX_PATH]=TEXT("");
  GetCurrentDirectory(sizeof(szPath),szPath);
  _sntprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\OxBattleConfig.ini"),szPath);

  //��������
  memcpy(m_szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));


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

  return true;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
  //����ע��
  ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));
  ZeroMemory(m_lbakAllJettonScore,sizeof(m_lbakAllJettonScore));
  //������ע
  ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

  //��ҳɼ�
  ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
  ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
  ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

  //�����˿���
  m_nChipRobotCount = 0;
  m_bControl=false;
  ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

  return;
}

//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
  if(pIServerUserItem->GetUserStatus() == US_PLAYING)
  {
    return 0L;
  }
  else
  {
    return __max(pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinTableScore, 0L);
  }
}

//�����¼�
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
  //��ׯ�ҷ����ڿ���ʱ��䶯ʱ(��ׯ�ҽ����˴�ȡ��)У��ׯ�ҵ���ׯ����
  if(wChairID == m_wCurrentBanker && m_pITableFrame->GetGameStatus()==GAME_SCENE_FREE)
  {
    ChangeBanker(false);
  }

  return true;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
  //�����������
  srand(GetTickCount());

  //���Ϳ����Ϣ
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //��ȡ�û�
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem == NULL)
    {
      continue;
    }

    if(CUserRight::IsGameCheatUser(m_pITableFrame->GetTableUserItem(i)->GetUserRight()))
    {
      CString strInfo;
      strInfo.Format(TEXT("��ǰ��棺%.2lf"), m_lStorageCurrent);

      m_pITableFrame->SendGameMessage(pIServerUserItem,strInfo,SMT_CHAT);
    }
  }

  CString strStorage;
  CTime Time(CTime::GetCurrentTime());
  strStorage.Format(TEXT(" ����: %s | ʱ��: %d-%d-%d %d:%d:%d | ���: %.2lf \n"), m_pGameServiceOption->szServerName, Time.GetYear(), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond(), m_lStorageCurrent);
  WriteInfo(TEXT("����ţţ�����־.log"), strStorage);

  //��������
  CMD_S_GameStart GameStart;
  ZeroMemory(&GameStart,sizeof(GameStart));

  //��ȡׯ��
  IServerUserItem *pIBankerServerUserItem=NULL;
  if(INVALID_CHAIR!=m_wCurrentBanker)
  {
    pIBankerServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
    m_lBankerScore=pIBankerServerUserItem->GetUserScore();
  }

  //���ñ���
  GameStart.cbTimeLeave=m_nPlaceJettonTime;
  GameStart.wBankerUser=m_wCurrentBanker;
  GameStart.lBottomPourImpose = m_lBottomPourImpose;
  if(pIBankerServerUserItem!=NULL)
  {
    GameStart.lBankerScore=m_lBankerScore;
  }
  GameStart.bContiueCard=m_bContiueCard;

  //��ע����������
  int nChipRobotCount = 0;
  for(int i = 0; i < GAME_PLAYER; i++)
  {
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
    {
      nChipRobotCount++;
    }
  }

  GameStart.nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);

  nChipRobotCount = 0;
  for(int i = 0; i < m_ApplyUserArray.GetCount(); i++)
  {
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray.GetAt(i));
    if(pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
    {
      nChipRobotCount++;
    }
  }

  if(nChipRobotCount > 0)
  {
    GameStart.nAndriodApplyCount=nChipRobotCount-1;
  }

  //�����˿���
  m_nChipRobotCount = 0;
  ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

  //�Թ����
  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

  //��Ϸ���
  for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
  {
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(pIServerUserItem==NULL)
    {
      continue;
    }
    int iTimer = 10;

    //���û���
    GameStart.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*iTimer);

    m_pITableFrame->SendTableData(wChairID,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
  }

  return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
  switch(cbReason)
  {
    case GER_NORMAL:    //�������
    {
      //��Ϸ����
      if(m_pServerContro != NULL && m_pServerContro->NeedControl())
      {
		BYTE cbControlTybe =  m_pServerContro->GetControlTybe();
		BYTE bWinArea[CONTROL_AREA]= {false};
		bool bControlLose = false;
		if (cbControlTybe == CS_BANKER_WIN)
		{
			for(BYTE Index=0; Index<AREA_COUNT; Index++)
			{
				if(m_lbakAllJettonScore[Index+1] > 0.001)
				{
					bWinArea[Index] = false;
				}
				else
				{
					if (rand()%100 < 50 )
					{
						bWinArea[Index] = true;
					}
					else
					{
						bWinArea[Index] = false;
					}
				}
			}
		}
		if (cbControlTybe == CS_BANKER_LOSE)
		{
			for(BYTE Index=0; Index<AREA_COUNT; Index++)
			{
				if(m_lbakAllJettonScore[Index+1] > 0.001)
				{
					bWinArea[Index] = true;
					bControlLose = true;
				}
				else
				{
					if (rand()%100 < 50 )
					{
						bWinArea[Index] = true;
					}
					else
					{
						bWinArea[Index] = false;
					}
				}
			}
		}
		m_pServerContro->SetControlArea(bWinArea);
		if (cbControlTybe == CS_BET_AREA)
		{
			DispatchTableCard();
			m_pServerContro->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lbakAllJettonScore);
		}
		else if (cbControlTybe == CS_BANKER_WIN)
		{
			BYTE cbAreaWinCount = 0;
			for(BYTE Index=0; Index<AREA_COUNT; Index++)
			{
				if (bWinArea[Index] == true)
				{
					cbAreaWinCount++;
				}
			}

			int nCount = 0;
			bool bOk = false;
			//�ɷ��˿�
			do 
			{	
				DispatchTableCard();
				for( BYTE cbIndex=0; cbIndex<MAX_CARDGROUP; cbIndex++)
				{
					BYTE cbFirstCardType=m_GameLogic.GetCardType(m_cbTableCardArray[cbIndex], 5);
					if (cbControlTybe == CS_BANKER_WIN)
					{
						if (cbFirstCardType >= 11)
						{
							bOk = true;
							nCount ++;
						}
					}
				}
				if (nCount <= cbAreaWinCount)
				{
					nCount = 0;
					bOk = false;
				}

			} while (bOk == false);
			m_pServerContro->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lbakAllJettonScore);
		}
		else if (cbControlTybe == CS_BANKER_LOSE)
		{

			if (bControlLose == true)
			{
				//�˿�����
				BYTE cbCardListData[54]=
				{
					0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D, //���� A - K
					0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D, //÷�� A - K
					0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D, //���� A - K
					0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D, //���� A - K
					0x41,0x42,
				};
				int nUserIndex = 0;
				do 
				{
					BYTE cbCard[5] = {0};
					if (nUserIndex == 0)
					{
						m_GameLogic.RandCardListLose(cbCard,5,cbCardListData,54);
						for(int j =0;j<5;j++)
						{
							if (cbCard[j] > 100)
							{
								int nn = 0;
							}
						}
						if (m_GameLogic.GetCardType(cbCard, 5) < 10)
						{
							//ȫ����ע
							CopyMemory(m_cbTableCardArray[nUserIndex],cbCard,sizeof(m_cbTableCardArray[nUserIndex]));
							m_GameLogic.RemoveGoodCardData(cbCard,5,cbCardListData,54);
							nUserIndex++;
						}
					}
					if (nUserIndex != 0)
					{	
						m_GameLogic.RandCardListLose(cbCard,5,cbCardListData,54-nUserIndex*5);
						for(int j =0;j<5;j++)
						{
							if (cbCard[j] > 100)
							{
								int nn = 0;
							}
						}
						if (m_lbakAllJettonScore[nUserIndex] < 0.001)
						{
							//ȫ����ע
							CopyMemory(m_cbTableCardArray[nUserIndex],cbCard,sizeof(m_cbTableCardArray[nUserIndex]));
							m_GameLogic.RemoveGoodCardData(cbCard,5,cbCardListData,54-nUserIndex*5);
							nUserIndex++;
						}
						else
						{
							BYTE bMultiple = 0;
							if (m_GameLogic.CompareCard(m_cbTableCardArray[0], 5,cbCard, 5, bMultiple) == 1 && m_GameLogic.GetCardType(cbCard, 5) <= 10)
							{
								//ȫ����ע
								CopyMemory(m_cbTableCardArray[nUserIndex],cbCard,sizeof(m_cbTableCardArray[nUserIndex]));
								m_GameLogic.RemoveGoodCardData(cbCard,5,cbCardListData,54-nUserIndex*5);
								nUserIndex++;
							}
						}

					}
					//if (nUserIndex == 2)
					//{
					//	m_GameLogic.RandCardListLose(cbCard,5,cbCardListData,44);
					//	if (m_lAllJettonScore[nUserIndex] < 0.001)
					//	{
					//		//ȫ����ע
					//		CopyMemory(m_cbTableCardArray[nUserIndex],cbCard,sizeof(m_cbTableCardArray[nUserIndex]));
					//		m_GameLogic.RemoveGoodCardData(cbCard,5,cbCardListData,54);
					//		nUserIndex++;
					//	}
					//	else
					//	{
					//		BYTE bMultiple = 0;
					//		if (m_GameLogic.CompareCard(m_cbTableCardArray[0], 5,cbCard, 5, bMultiple) == 1 && m_GameLogic.GetCardType(cbCard, 5) <= 10)
					//		{
					//			//ȫ����ע
					//			CopyMemory(m_cbTableCardArray[nUserIndex],cbCard,sizeof(m_cbTableCardArray[nUserIndex]));
					//			m_GameLogic.RemoveGoodCardData(cbCard,5,cbCardListData,54);
					//			nUserIndex++;
					//		}
					//	}
					//}
					//if (nUserIndex == 3)
					//{
					//	m_GameLogic.RandCardListLose(cbCard,5,cbCardListData,39);
					//	if (m_lAllJettonScore[nUserIndex] < 0.001)
					//	{
					//		//ȫ����ע
					//		CopyMemory(m_cbTableCardArray[nUserIndex],cbCard,sizeof(m_cbTableCardArray[nUserIndex]));
					//		m_GameLogic.RemoveGoodCardData(cbCard,5,cbCardListData,54);
					//		nUserIndex++;
					//	}
					//	else
					//	{
					//		BYTE bMultiple = 0;
					//		if (m_GameLogic.CompareCard(m_cbTableCardArray[0], 5,cbCard, 5, bMultiple) == 1 && m_GameLogic.GetCardType(cbCard, 5) <= 10)
					//		{
					//			//ȫ����ע
					//			CopyMemory(m_cbTableCardArray[nUserIndex],cbCard,sizeof(m_cbTableCardArray[nUserIndex]));
					//			m_GameLogic.RemoveGoodCardData(cbCard,5,cbCardListData,54);
					//			nUserIndex++;
					//		}
					//	}
					//}
					//if (nUserIndex == 4)
					//{
					//	m_GameLogic.RandCardListLose(cbCard,5,cbCardListData,34);
					//	if (m_lAllJettonScore[nUserIndex] < 0.001)
					//	{
					//		//ȫ����ע
					//		CopyMemory(m_cbTableCardArray[nUserIndex],cbCard,sizeof(m_cbTableCardArray[nUserIndex]));
					//		m_GameLogic.RemoveGoodCardData(cbCard,5,cbCardListData,54);
					//		nUserIndex++;
					//	}
					//	else
					//	{
					//		BYTE bMultiple = 0;
					//		if (m_GameLogic.CompareCard(m_cbTableCardArray[0], 5,cbCard, 5, bMultiple) == 1 && m_GameLogic.GetCardType(cbCard, 5) <= 10)
					//		{
					//			//ȫ����ע
					//			CopyMemory(m_cbTableCardArray[nUserIndex],cbCard,sizeof(m_cbTableCardArray[nUserIndex]));
					//			m_GameLogic.RemoveGoodCardData(cbCard,5,cbCardListData,54);
					//			nUserIndex++;
					//		}
					//	}
					//}
				} while (nUserIndex != 5 );
			}
			else
			{
				DispatchTableCard();
				m_pServerContro->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lbakAllJettonScore);
			}
		}

        //��ȡ����Ŀ������
        m_bControl=true;
        
        m_pServerContro->CompleteControl();
      }
      else
      {
	    //�ɷ��˿�
	    DispatchTableCard();
        StorageOptimize();
      }

      //�������
      SCORE lBankerWinScore=CalculateScore();

      //��������
      m_wBankerTime++;

      //������Ϣ
      CMD_S_GameEnd GameEnd;
      ZeroMemory(&GameEnd,sizeof(GameEnd));

      //ׯ����Ϣ
      GameEnd.nBankerTime = m_wBankerTime;
      GameEnd.lBankerTotallScore=m_lBankerWinScore;
      GameEnd.lBankerScore=lBankerWinScore;
      GameEnd.bcFirstCard = m_bcFirstPostCard;

      //�˿���Ϣ
      CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
      GameEnd.cbLeftCardCount=m_cbLeftCardCount;

	  for (int m = 0;m < 5;m++)
	  {
	  }
	  for(int i =0;i<5;i++)
	  {
		  for (int j = 0;j<5;j++)
		  {
			  if(i == j)continue;
			  if(m_cbTableCardArray[m][i] == m_cbTableCardArray[m][j])
			  {
				  int n  = 0;
			  }
		  }
	  }
      //���ͻ���
      GameEnd.cbTimeLeave=m_nGameEndTime;
      for(WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex)
      {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
        if(pIServerUserItem == NULL)
        {
          continue;
        }

        //���óɼ�
        GameEnd.lUserScore=m_lUserWinScore[wUserIndex];

        //��������
        GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

        //����˰��
        if(m_lUserRevenue[wUserIndex]>0)
        {
          GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
        }
        else if(m_wCurrentBanker!=INVALID_CHAIR)
        {
          GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
        }
        else
        {
          GameEnd.lRevenue=0;
        }

        //������Ϣ
        m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
        m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
      }

      return true;
    }
    case GER_USER_LEAVE:    //�û��뿪
    case GER_NETWORK_ERROR:
    {
      //�м��ж�
      if(m_wCurrentBanker!=wChairID)
      {
        //��������
        SCORE lRevenue=0;

        //�Ƿ���
        if(m_pITableFrame->GetGameStatus() == GAME_SCENE_PLACE_JETTON)
        {
          //������ע
          for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
          {
            if(m_lUserJettonScore[nAreaIndex][wChairID] != 0)
            {
              CMD_S_PlaceJettonFail PlaceJettonFail;
              ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
              PlaceJettonFail.lJettonArea=nAreaIndex;
              PlaceJettonFail.lPlaceScore=m_lUserJettonScore[nAreaIndex][wChairID];
              PlaceJettonFail.wPlaceUser=wChairID;

              //��Ϸ���
              for(WORD i=0; i<GAME_PLAYER; ++i)
              {
                IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
                if(pIServerUserItem==NULL)
                {
                  continue;
                }

                m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
              }

              m_lAllJettonScore[nAreaIndex] -= m_lUserJettonScore[nAreaIndex][wChairID];
              m_lUserJettonScore[nAreaIndex][wChairID] = 0;
            }
          }
        }
        else
        {
          //д�����
          if(m_lUserWinScore[wChairID]!=0L)
          {
            tagScoreInfo ScoreInfo[GAME_PLAYER];
            ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
            //�����
            if(!pIServerUserItem->IsAndroidUser())
            {
              //m_lStorageCurrent -= (m_lUserWinScore[wChairID]+m_lUserRevenue[wChairID]);
            }
            ScoreInfo[wChairID].lScore = m_lUserWinScore[wChairID];
            ScoreInfo[wChairID].cbType=m_lUserWinScore[wChairID]>0?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
            ScoreInfo[wChairID].lRevenue = m_lUserRevenue[wChairID];
            m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
            m_lUserWinScore[wChairID] = 0;
          }
          //�����ע
          for(int nAreaIndex = 1; nAreaIndex <= AREA_COUNT; nAreaIndex++)
          {
            m_lUserJettonScore[nAreaIndex][wChairID] = 0;
          }
        }

        return true;
      }

      //״̬�ж�
      if(m_pITableFrame->GetGameStatus()!=GAME_SCENE_GAME_END)
      {
        //��ʾ��Ϣ
        TCHAR szTipMsg[128];
        _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("����ׯ��[ %s ]ǿ�ˣ���Ϸ��ǰ������"),pIServerUserItem->GetNickName());

        //������Ϣ
        SendGameMessage(INVALID_CHAIR,szTipMsg);

        //����״̬
        m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

        //����ʱ��
        m_dwJettonTime=(DWORD)time(NULL);
        m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
        m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);

        //��Ϸ����
        if(m_pServerContro != NULL && m_pServerContro->NeedControl())
        {
          //��ȡ����Ŀ������
          m_pServerContro->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
          m_pServerContro->CompleteControl();
        }

        //�������
        CalculateScore();

        //������Ϣ
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd,sizeof(GameEnd));

        //ׯ����Ϣ
        GameEnd.nBankerTime = m_wBankerTime;
        GameEnd.lBankerTotallScore=m_lBankerWinScore;
        GameEnd.lBankerScore=m_lUserWinScore[m_wCurrentBanker];

        //�˿���Ϣ
        CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
        GameEnd.cbLeftCardCount=m_cbLeftCardCount;

        //���ͻ���
        GameEnd.cbTimeLeave=m_nGameEndTime;
        for(WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex)
        {
          IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
          if(pIServerUserItem == NULL)
          {
            continue;
          }

          //���óɼ�
          GameEnd.lUserScore=m_lUserWinScore[wUserIndex];

          //��������
          GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

          //����˰��
          if(m_lUserRevenue[wUserIndex]>0)
          {
            GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
          }
          else if(m_wCurrentBanker!=INVALID_CHAIR)
          {
            GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
          }
          else
          {
            GameEnd.lRevenue=0;
          }

          //������Ϣ
          m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
          m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
        }
      }

      //�۳�����
      tagScoreInfo ScoreInfo[GAME_PLAYER];
      ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
      ScoreInfo[m_wCurrentBanker].lScore =m_lUserWinScore[m_wCurrentBanker];
      ScoreInfo[m_wCurrentBanker].lRevenue = m_lUserRevenue[m_wCurrentBanker];
      ScoreInfo[m_wCurrentBanker].cbType = (m_lUserWinScore[m_wCurrentBanker]>0?SCORE_TYPE_WIN:SCORE_TYPE_LOSE);
      m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

      //�����
      IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
      if(pIServerUserItem != NULL && !pIServerUserItem->IsAndroidUser())
      {
        //m_lStorageCurrent -= (m_lUserWinScore[m_wCurrentBanker]+m_lUserRevenue[m_wCurrentBanker]);
      }

      m_lUserWinScore[m_wCurrentBanker] = 0;

      //�л�ׯ��
      ChangeBanker(true);

      return true;
    }
  }
  ASSERT(FALSE);
  return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
  switch(cbGameStatus)
  {
    case GAME_SCENE_FREE:     //����״̬
    {
      //���ͼ�¼
      SendGameRecord(pIServerUserItem);

	  //��Ϸ����
	  BYTE cbArea[4];
	  ZeroMemory(cbArea,sizeof(cbArea));
	  BYTE cbControlStyle = 0;
	  if(m_pServerContro != NULL && m_pServerContro->NeedControl())
	  {
		  m_pServerContro->GetControlArea(cbArea,cbControlStyle);
	  }


      //��������
      CMD_S_StatusFree StatusFree;
      ZeroMemory(&StatusFree,sizeof(StatusFree));

      //������Ϣ
      StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
      StatusFree.lAreaLimitScore = m_lAreaLimitScore;
      StatusFree.bGenreEducate = m_pGameServiceOption->wServerType&GAME_GENRE_EDUCATE;
      //ׯ����Ϣ
      StatusFree.bEnableSysBanker=m_bEnableSysBanker;
      StatusFree.wBankerUser=m_wCurrentBanker;
      StatusFree.cbBankerTime=m_wBankerTime;
      StatusFree.lBankerWinScore=m_lBankerWinScore;
      if(m_wCurrentBanker!=INVALID_CHAIR)
      {
        IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
        StatusFree.lBankerScore=pIServerUserItem->GetUserScore();
      }
      StatusFree.nEndGameMul = m_nEndGameMul;

      //�����Ϣ
      if(pIServerUserItem->GetUserStatus()!=US_LOOKON)
      {
        StatusFree.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*10);
      }
      //��������
      CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));

      //ȫ����Ϣ
      DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
      StatusFree.cbTimeLeave=(BYTE)(m_nFreeTime-__min(dwPassTime,(DWORD)m_nFreeTime));
      //Ȩ���ж�
	  StatusFree.bControlLimits = false;
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
         StatusFree.bControlLimits = true;
      }
	  StatusFree.lBottomPourImpose = m_lBottomPourImpose;
	  //����������
	  if(pIServerUserItem->IsAndroidUser())
	  {
		  tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
		  ASSERT(pCustomConfig);

		  CopyMemory(&StatusFree.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
	  }


	  CopyMemory(&StatusFree.occupyseatConfig, &m_OccUpYesAtconfig, sizeof(tagOccUpYesAtconfig));
	  //ȫ����ע
	  CopyMemory(StatusFree.wOccupySeatChairID,m_wOccupySeatChairID,sizeof(StatusFree.wOccupySeatChairID));


	  CopyMemory(StatusFree.cbArea,cbArea,sizeof(StatusFree.cbArea));
		StatusFree.cbControlStyle = cbControlStyle;
      //���ͳ���
      bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

      //������ʾ
      TCHAR szTipMsg[128];
      _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%.2lf,��������Ϊ��%.2lf,�������Ϊ��%.2lf"),m_lApplyBankerCondition,
                 m_lAreaLimitScore,m_lUserLimitScore);

      m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);

	  SendPeizhiUser(pIServerUserItem);
      //����������
      SendApplyUser(pIServerUserItem);

      //���¿����Ϣ
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
        CMD_S_UpdateStorage updateStorage;
        ZeroMemory(&updateStorage, sizeof(updateStorage));

        updateStorage.cbReqType = RQ_REFRESH_STORAGE;
        updateStorage.lStorageStart = m_lStorageStart;
        updateStorage.lStorageDeduct = m_nStorageDeduct;
        updateStorage.lStorageCurrent = m_lStorageCurrent;
        updateStorage.lStorageMax1 = m_lStorageMax1;
        updateStorage.lStorageMul1 = m_lStorageMul1;
        updateStorage.lStorageMax2 = m_lStorageMax2;
        updateStorage.lStorageMul2 = m_lStorageMul2;

        m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
      }

      return bSuccess;
    }
    case GAME_SCENE_PLACE_JETTON:   //��Ϸ״̬
    case GAME_SCENE_GAME_END:     //����״̬
    {

		//��Ϸ����
		BYTE cbArea[4];
		ZeroMemory(cbArea,sizeof(cbArea));
		BYTE cbControlStyle = 0;
		if(m_pServerContro != NULL && m_pServerContro->NeedControl())
		{
			m_pServerContro->GetControlArea(cbArea,cbControlStyle);
		}

      //���ͼ�¼
      SendGameRecord(pIServerUserItem);

      //��������
      CMD_S_StatusPlay StatusPlay= {0};

      //ȫ����ע
      CopyMemory(StatusPlay.lAllJettonScore,m_lAllJettonScore,sizeof(StatusPlay.lAllJettonScore));

      //�����ע
      if(pIServerUserItem->GetUserStatus()!=US_LOOKON)
      {
        for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
        {
          StatusPlay.lUserJettonScore[nAreaIndex] = m_lUserJettonScore[nAreaIndex][wChairID];
        }

        //�����ע
        StatusPlay.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*10);
      }

      //������Ϣ
      StatusPlay.lApplyBankerCondition=m_lApplyBankerCondition;
      StatusPlay.lAreaLimitScore=m_lAreaLimitScore;
      StatusPlay.bGenreEducate = m_pGameServiceOption->wServerType&GAME_GENRE_EDUCATE;
      //ׯ����Ϣ
      StatusPlay.bEnableSysBanker=m_bEnableSysBanker;
      StatusPlay.wBankerUser=m_wCurrentBanker;
      StatusPlay.cbBankerTime=m_wBankerTime;
      StatusPlay.lBankerWinScore=m_lBankerWinScore;
      if(m_wCurrentBanker!=INVALID_CHAIR)
      {
        StatusPlay.lBankerScore=m_lBankerScore;
      }
      StatusPlay.nEndGameMul = m_nEndGameMul;
	  StatusPlay.lBottomPourImpose = m_lBottomPourImpose;
	        //Ȩ���ж�
	  StatusPlay.bControlLimits = false;
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
         StatusPlay.bControlLimits = true;
      }


      //ȫ����Ϣ
      DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
      int nTotalTime = (cbGameStatus==GAME_SCENE_PLACE_JETTON?m_nPlaceJettonTime:m_nGameEndTime);
      StatusPlay.cbTimeLeave=(BYTE)(nTotalTime-__min(dwPassTime,(DWORD)nTotalTime));
      StatusPlay.cbGameStatus=m_pITableFrame->GetGameStatus();

      //��������
      CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));

      //�����ж�
      if(cbGameStatus==GAME_SCENE_GAME_END)
      {
        //���óɼ�
        StatusPlay.lEndUserScore=m_lUserWinScore[wChairID];

        //��������
        StatusPlay.lEndUserReturnScore=m_lUserReturnScore[wChairID];

        //����˰��
        if(m_lUserRevenue[wChairID]>0)
        {
          StatusPlay.lEndRevenue=m_lUserRevenue[wChairID];
        }
        else if(m_wCurrentBanker!=INVALID_CHAIR)
        {
          StatusPlay.lEndRevenue=m_lUserRevenue[m_wCurrentBanker];
        }
        else
        {
          StatusPlay.lEndRevenue=0;
        }

        //ׯ�ҳɼ�
        StatusPlay.lEndBankerScore=m_lBankerCurGameScore;

        //�˿���Ϣ
        CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
      }

      //����������
      if(pIServerUserItem->IsAndroidUser())
      {
        tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
        ASSERT(pCustomConfig);

        CopyMemory(&StatusPlay.CustomAndroid, &pCustomConfig->CustomAndroid, sizeof(tagCustomAndroid));
      }

	  CopyMemory(&StatusPlay.occupyseatConfig, &m_OccUpYesAtconfig, sizeof(tagOccUpYesAtconfig));
	  //ȫ����ע
	  CopyMemory(StatusPlay.wOccupySeatChairID,m_wOccupySeatChairID,sizeof(StatusPlay.wOccupySeatChairID));

	  CopyMemory(StatusPlay.cbArea,cbArea,sizeof(StatusPlay.cbArea));
	  StatusPlay.cbControlStyle = cbControlStyle;

      //���ͳ���
      bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));

      //������ʾ
      TCHAR szTipMsg[128];
      _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%.2lf,��������Ϊ��%.2lf,�������Ϊ��%.2lf"),m_lApplyBankerCondition,
                 m_lAreaLimitScore,m_lUserLimitScore);

      m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
	  SendPeizhiUser(pIServerUserItem);
      //����������
      SendApplyUser(pIServerUserItem);

      //���¿����Ϣ
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
        CMD_S_UpdateStorage updateStorage;
        ZeroMemory(&updateStorage, sizeof(updateStorage));

        updateStorage.cbReqType = RQ_REFRESH_STORAGE;
        updateStorage.lStorageStart = m_lStorageStart;
        updateStorage.lStorageDeduct = m_nStorageDeduct;
        updateStorage.lStorageCurrent = m_lStorageCurrent;
        updateStorage.lStorageMax1 = m_lStorageMax1;
        updateStorage.lStorageMul1 = m_lStorageMul1;
        updateStorage.lStorageMax2 = m_lStorageMax2;
        updateStorage.lStorageMul2 = m_lStorageMul2;

        m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
      }

      //���������ע��Ϣ
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
        SendUserBetInfo(pIServerUserItem);
      }

      return bSuccess;
    }
  }

  return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
  switch(wTimerID)
  {
    case IDI_FREE:    //����ʱ��
    {
      //��ʼ��Ϸ
      m_pITableFrame->StartGame();

      //����ʱ��
      m_dwJettonTime=(DWORD)time(NULL);
      m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_nPlaceJettonTime*1000,1,0L);

      ////�ɷ��˿�
      //DispatchTableCard();

      //����״̬
      m_pITableFrame->SetGameStatus(GAME_SCENE_PLACE_JETTON);

      return true;
    }
    case IDI_PLACE_JETTON:    //��עʱ��
    {
      //״̬�ж�(��ֹǿ���ظ�����)
      if(m_pITableFrame->GetGameStatus()!=GAME_SCENE_GAME_END)
      {
        //����״̬
        m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

        //������Ϸ
        OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

        //����ʱ��
        m_dwJettonTime=(DWORD)time(NULL);
        m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);
      }

      return true;
    }
    case IDI_GAME_END:      //������Ϸ
    {
      //д�����
      tagScoreInfo ScoreInfo[GAME_PLAYER];
      ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

	  			//ϵͳ��ׯ
	  SCORE TempStartScore=0;
		bool bSystemBanker = false;
		if ( m_wCurrentBanker == INVALID_CHAIR )
		{
			bSystemBanker = true;
		}
		else
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			if ( pServerUserItem != NULL )	
				bSystemBanker = pServerUserItem->IsAndroidUser();
		}


      for(WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID)
      {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
        if(pIServerUserItem == NULL||(QueryBuckleServiceCharge(wUserChairID)==false))
        {
          continue;
        }

        //ʤ������
        BYTE ScoreKind=(m_lUserWinScore[wUserChairID]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
        ScoreInfo[wUserChairID].lScore = m_lUserWinScore[wUserChairID];
        ScoreInfo[wUserChairID].lRevenue = m_lUserRevenue[wUserChairID];
        ScoreInfo[wUserChairID].cbType = ScoreKind;

        ////�����
        //if(!pIServerUserItem->IsAndroidUser())
        //{
        //  m_lStorageCurrent -= (m_lUserWinScore[wUserChairID]+m_lUserRevenue[wUserChairID]);
        //}

		if(bSystemBanker && !pIServerUserItem->IsAndroidUser())				//ϵͳ��ׯ
			TempStartScore += m_lUserWinScore[wUserChairID];
		else if(bSystemBanker == false && pIServerUserItem->IsAndroidUser())			//�����ׯ
			TempStartScore -= m_lUserWinScore[wUserChairID];
		//������ҵ��պ�����Ӯ
		SCORE lTdTotalScore = pIServerUserItem->GetTdTotalScore();
		SCORE lTotalScore = pIServerUserItem->GetTotalScore();

		pIServerUserItem->SetTotalScore(lTotalScore +  m_lUserWinScore[wUserChairID]);
		pIServerUserItem->SetTdTotalScore(lTdTotalScore +  m_lUserWinScore[wUserChairID]);
      }

      //д�����
      m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

      for(WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID)
      {
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
			if(pIServerUserItem == NULL)
			{
				continue;
			}
	  		for (int i =0;i<MAX_OCCUPY_SEAT_COUNT;i++)
			{
				if(m_wOccupySeatChairID[i] == pIServerUserItem->GetChairID() && pIServerUserItem->GetUserScore() < m_OccUpYesAtconfig.lOccupySeatFree)
				{
					m_wOccupySeatChairID[i] = -1;
					break;
				}
			}
	  }
		
		CMD_S_UpdateOccupySeat UpdateOccupySeat;
		ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
		//UpdateOccupySeat.wQuitOccupySeatChairID = pIServerUserItem->GetChairID();
		//ȫ����ע
		CopyMemory(&UpdateOccupySeat.tabWOccupySeatChairID,&m_wOccupySeatChairID,sizeof(UpdateOccupySeat.tabWOccupySeatChairID));

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

	  ReadConfigInformation();

	  for(WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID)
	  {
		  IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
		  if(pIServerUserItem == NULL)
			  continue;
		  if(!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
			  continue;
		  for(WORD i = 0; i < GAME_PLAYER; ++i)
		  {
			  IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
			  if(pUserItem == NULL)
				  continue;
			   if (pUserItem->IsAndroidUser())
				continue;
			  CMD_S_UpAlllosewin UpAlllosewin;
			  ZeroMemory(&UpAlllosewin, sizeof(UpAlllosewin));
			  UpAlllosewin.dwGameID = pUserItem->GetGameID();
			  UpAlllosewin.lTdTotalScore = pUserItem->GetTdTotalScore();
			  UpAlllosewin.lTotalScore = pUserItem->GetTotalScore();
			  UpAlllosewin.lScore = pUserItem->GetUserInfo()->lScore;
			  m_pITableFrame->SendTableData(wUserChairID, SUB_S_UPALLLOSEWIN, &UpAlllosewin, sizeof(UpAlllosewin));
			  m_pITableFrame->SendLookonData(wUserChairID, SUB_S_UPALLLOSEWIN, &UpAlllosewin, sizeof(UpAlllosewin));
		  }
	  }
      SCORE lTempDeduct=0;
      lTempDeduct=m_nStorageDeduct;
      bool bDeduct=NeedDeductStorage();
      lTempDeduct=bDeduct?lTempDeduct:0;
	  if(m_bControl == false)
	  {
		m_lStorageCurrent -= TempStartScore;
		//���˥��
		if (TempStartScore < -0.001 || TempStartScore > 0.001)
		{
			m_lAttenuationScore +=  m_lStorageCurrent*m_nStorageDeduct/1000;
			m_lStorageCurrent -= m_lStorageCurrent*m_nStorageDeduct/1000;
		}
	  }
		TCHAR szStorage[1024]=TEXT("");
		_sntprintf(szStorage,sizeof(szStorage),TEXT("%.2lf"),m_lStorageCurrent);
		WritePrivateProfileString(m_szControlRoomName,TEXT("StorageStart"),szStorage,m_szControlConfigFileName);

		_sntprintf(szStorage,sizeof(szStorage),TEXT("%.2lf"),m_lAttenuationScore);
		WritePrivateProfileString(m_szControlRoomName,TEXT("AttenuationScore"),szStorage,m_szControlConfigFileName);

      //������Ϸ
      m_pITableFrame->ConcludeGame(GAME_SCENE_FREE);

	  if (m_bAndroidXiahzuang)
	  {
		  if (m_wCurrentBanker != INVALID_CHAIR)
		  {
			  IServerUserItem * pControlUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			  if(pControlUserItem != NULL &&pControlUserItem->IsAndroidUser())
			  {
				 OnUserCancelBanker(pControlUserItem);
				 m_bAndroidXiahzuang = false;
			  }	
		  }
	  }	
	m_bAndroidXiahzuang =false;
      //�л�ׯ��
      ChangeBanker(false);

      //����ʱ��
      m_dwJettonTime=(DWORD)time(NULL);
      m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,0L);

	  //��Ϸ����
	  BYTE cbControl = 0;
	  if(m_pServerContro != NULL && m_pServerContro->NeedControl())
	  {
		cbControl = 1;
	  }

      //������Ϣ
      CMD_S_GameFree GameFree;
      ZeroMemory(&GameFree,sizeof(GameFree));
      GameFree.cbTimeLeave=m_nFreeTime;
      GameFree.lStorageStart=m_lStorageCurrent;
      GameFree.nListUserCount=m_ApplyUserArray.GetCount()-1;
	  GameFree.cbControl = cbControl;
      m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
      m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));



      //���¿����Ϣ
      for(WORD wUserID = 0; wUserID < GAME_PLAYER; ++wUserID)
      {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserID);
        if(pIServerUserItem == NULL)
        {
          continue;
        }
        if(!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
        {
          continue;
        }

        CMD_S_UpdateStorage updateStorage;
        ZeroMemory(&updateStorage, sizeof(updateStorage));

        updateStorage.cbReqType = RQ_REFRESH_STORAGE;
        updateStorage.lStorageStart = m_lStorageStart;
        updateStorage.lStorageDeduct = m_nStorageDeduct;
        updateStorage.lStorageCurrent = m_lStorageCurrent;
        updateStorage.lStorageMax1 = m_lStorageMax1;
        updateStorage.lStorageMul1 = m_lStorageMul1;
        updateStorage.lStorageMax2 = m_lStorageMax2;
        updateStorage.lStorageMul2 = m_lStorageMul2;

        m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
      }

      return true;
    }
  }

  return false;
}

//��Ϸ��Ϣ
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
  switch(wSubCmdID)
  {
    case SUB_C_PLACE_JETTON:    //�û���ע
    {
      //Ч������
      ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
      if(wDataSize!=sizeof(CMD_C_PlaceJetton))
      {
        return false;
      }

      //�û�Ч��
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      //��Ϣ����
      CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pData;
      return OnUserPlaceJetton(pUserData->wChairID,pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
    }
    case SUB_C_APPLY_BANKER:    //������ׯ
    {
      //�û�Ч��
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus==US_LOOKON)
      {
        return true;
      }

      return OnUserApplyBanker(pIServerUserItem);
    }
    case SUB_C_CANCEL_BANKER:   //ȡ����ׯ
    {
      //�û�Ч��
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus==US_LOOKON)
      {
        return true;
      }

      return OnUserCancelBanker(pIServerUserItem);
    }
    case SUB_C_CONTINUE_CARD:   //��������
    {
      //�û�Ч��
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus==US_LOOKON)
      {
        return true;
      }
      if(pUserData->wChairID!=m_wCurrentBanker)
      {
        return true;
      }
      if(m_cbLeftCardCount < 8)
      {
        return true;
      }

      //���ñ���
      m_bContiueCard=true;

      //������Ϣ
      SendGameMessage(pUserData->wChairID,TEXT("���óɹ�����һ�ֽ��������ƣ�"));

      return true;
    }
	case SUB_C_SUPERROB_BANKER:
	{
		return true;
	}
	case SUB_C_OCCUPYSEAT:
	{
		ASSERT(wDataSize==sizeof(CMD_C_OccupySeat));
		if(wDataSize!=sizeof(CMD_C_OccupySeat))
			return false;

		//��Ϣ����
		CMD_C_OccupySeat * pOccupySeat=(CMD_C_OccupySeat *)pData;

		if(m_OccUpYesAtconfig.occupyseatType == OCCUPYSEAT_VIPTYPE)
		{

		}
		else if(m_OccUpYesAtconfig.occupyseatType == OCCUPYSEAT_CONSUMETYPE)
		{
			//CMD_S_OccupySeat_Fail OccupySeat_Fail;
			//ZeroMemory(&OccupySeat_Fail,sizeof(OccupySeat_Fail));
			//OccupySeat_Fail.cbAlreadyOccupySeatIndex = pOccupySeat->cbOccupySeatIndex;
			//OccupySeat_Fail.wAlreadyOccupySeatChairID = pOccupySeat->wOccupySeatChairID;
			//m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));
			//m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OCCUPYSEAT_FAIL, &OccupySeat_Fail, sizeof(OccupySeat_Fail));
			//return true;
		}
		else
		{

		}
		CMD_S_OccupySeat OccupySeat;
		ZeroMemory(&OccupySeat, sizeof(OccupySeat));
		OccupySeat.wOccupySeatChairID = pOccupySeat->wOccupySeatChairID;
		OccupySeat.cbOccupySeatIndex = pOccupySeat->cbOccupySeatIndex;
		m_wOccupySeatChairID[OccupySeat.cbOccupySeatIndex] = pOccupySeat->wOccupySeatChairID;
		//ȫ����ע
		CopyMemory(&OccupySeat.tabWOccupySeatChairID,&m_wOccupySeatChairID,sizeof(OccupySeat.tabWOccupySeatChairID));

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OCCUPYSEAT, &OccupySeat, sizeof(OccupySeat));

		return true;
	}
	case SUB_C_QUIT_OCCUPYSEAT:
	{
		for (int i =0;i<MAX_OCCUPY_SEAT_COUNT;i++)
		{
			if(m_wOccupySeatChairID[i] == pIServerUserItem->GetChairID())
			{
				m_wOccupySeatChairID[i] = -1;
				break;
			}
		}
		
		CMD_S_UpdateOccupySeat UpdateOccupySeat;
		ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
		UpdateOccupySeat.wQuitOccupySeatChairID = pIServerUserItem->GetChairID();
		//ȫ����ע
		CopyMemory(&UpdateOccupySeat.tabWOccupySeatChairID,&m_wOccupySeatChairID,sizeof(UpdateOccupySeat.tabWOccupySeatChairID));

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

		return true;
	}
    case SUB_C_AMDIN_COMMAND:
    {
      ASSERT(wDataSize==sizeof(CMD_C_AdminReq)); 
      if(wDataSize!=sizeof(CMD_C_AdminReq))
      {
        return false;
      }

      //Ȩ���ж�
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
      {
        return false;
      }

      if(m_pServerContro == NULL)
      {
        return false;
      }
      CopyMemory(m_szControlName,pIServerUserItem->GetNickName(),sizeof(m_szControlName));

      return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);
    }
    case SUB_C_UPDATE_STORAGE:    //���¿��
    {
      ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
      if(wDataSize!=sizeof(CMD_C_UpdateStorage))
      {
        return false;
      }

      //Ȩ���ж�
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
      {
        return false;
      }

      //��Ϣ����
      CMD_C_UpdateStorage * pUpdateStorage=(CMD_C_UpdateStorage *)pData;
      if(pUpdateStorage->cbReqType==RQ_SET_STORAGE)
      {
        m_nStorageDeduct = pUpdateStorage->lStorageDeduct;
        m_lStorageCurrent = pUpdateStorage->lStorageCurrent;
        m_lStorageMax1 = pUpdateStorage->lStorageMax1;
        m_lStorageMul1 = pUpdateStorage->lStorageMul1;
        m_lStorageMax2 = pUpdateStorage->lStorageMax2;
        m_lStorageMul2 = pUpdateStorage->lStorageMul2;

        //��¼��Ϣ
        CString strControlInfo, str;
        str.Format(TEXT("%s"), TEXT("�޸Ŀ�����ã�"));
        CTime Time(CTime::GetCurrentTime());
        strControlInfo.Format(TEXT("����: %s | ����: %u | ʱ��: %d-%d-%d %d:%d:%d\n�������˺�: %s | ������ID: %u\n%s\r\n"),
                              m_pGameServiceOption->szServerName, m_pITableFrame->GetTableID()+1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
                              Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

        WriteInfo(TEXT("����ţţ������Ϣ.log"),strControlInfo);
      }

      for(WORD wUserID = 0; wUserID < GAME_PLAYER; wUserID++)
      {
        IServerUserItem *pIServerUserItemSend = m_pITableFrame->GetTableUserItem(wUserID);
        if(pIServerUserItemSend == NULL)
        {
          continue;
        }
        if(!CUserRight::IsGameCheatUser(pIServerUserItemSend->GetUserRight()))
        {
          continue;
        }

        if(RQ_REFRESH_STORAGE == pUpdateStorage->cbReqType && pIServerUserItem->GetChairID() != wUserID)
        {
          continue;
        }

        CMD_S_UpdateStorage updateStorage;
        ZeroMemory(&updateStorage, sizeof(updateStorage));

        if(RQ_SET_STORAGE == pUpdateStorage->cbReqType && pIServerUserItem->GetChairID() == wUserID)
        {
          updateStorage.cbReqType = RQ_SET_STORAGE;
        }
        else
        {
          updateStorage.cbReqType = RQ_REFRESH_STORAGE;
        }

        updateStorage.lStorageStart = m_lStorageStart;
        updateStorage.lStorageDeduct = m_nStorageDeduct;
        updateStorage.lStorageCurrent = m_lStorageCurrent;
        updateStorage.lStorageMax1 = m_lStorageMax1;
        updateStorage.lStorageMul1 = m_lStorageMul1;
        updateStorage.lStorageMax2 = m_lStorageMax2;
        updateStorage.lStorageMul2 = m_lStorageMul2;

        m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
      }

      return true;
    }
	case SUB_C_PEIZHI_USER:
	{
		ASSERT(wDataSize==sizeof(CMD_C_peizhiVec));
		if(wDataSize!=sizeof(CMD_C_peizhiVec)) return false;
		//��Ϣ����
		CMD_C_peizhiVec * pPeizhiVec=(CMD_C_peizhiVec *)pData;
		if(pPeizhiVec == NULL)return false;
		//��������
		IServerUserItem * pUserItem=m_pITableFrame->SearchUserItemGameID(pPeizhiVec->dwGameID);
		if(pUserItem!= NULL)
		{
			bool bOK = false;
			for(int i =0;i < m_PeizhiUserArray.GetCount();i++)
			{
				if(m_PeizhiUserArray[i] ==  pPeizhiVec->dwGameID)
				{
					bOK = true;
					break;
				}
			}
			if(bOK == false)
			{
				m_PeizhiUserArray.Add(pPeizhiVec->dwGameID);
				CMD_S_peizhiVec peizhiVec;
				ZeroMemory(&peizhiVec,sizeof(peizhiVec));
				peizhiVec.dwGameID = pPeizhiVec->dwGameID;
				peizhiVec.lScore = pUserItem->GetUserInfo()->lScore;
				for(int i =0;i<GAME_PLAYER;i++)
				{
					IServerUserItem * pControlUserItem=m_pITableFrame->GetTableUserItem(i);
					if(pControlUserItem == NULL)continue;
					//Ȩ���ж�
					if(CUserRight::IsGameCheatUser(pControlUserItem->GetUserRight())==false)
						continue;
					m_pITableFrame->SendTableData(i, SUB_S_PEIZHI_USER, &peizhiVec, sizeof(peizhiVec));
				}
			}
		}
		return true;
	}
	case SUB_C_DelPeizhi:
		{
			ASSERT(wDataSize==sizeof(CMD_C_DelPeizhi));
			if(wDataSize!=sizeof(CMD_C_DelPeizhi)) return false;

			CMD_C_DelPeizhi * pDelPeizhi=(CMD_C_DelPeizhi *)pData;
			if(pDelPeizhi == NULL)return false;
			bool bOK = false;
			//�������
			for (WORD i=0; i<m_PeizhiUserArray.GetCount(); ++i)
			{
				//��ȡ���
				DWORD dwGameID=m_PeizhiUserArray[i];

				//��������
				if (dwGameID != pDelPeizhi->dwGameID) continue;

				//ɾ�����
				m_PeizhiUserArray.RemoveAt(i);
				bOK= true;
				break;
			}
			if(bOK)
			{
				CMD_S_DelPeizhi DelPeizhi;
				ZeroMemory(&DelPeizhi,sizeof(DelPeizhi));
				DelPeizhi.dwGameID = pDelPeizhi->dwGameID;
				for(int i =0;i<GAME_PLAYER;i++)
				{
					IServerUserItem * pControlUserItem=m_pITableFrame->GetTableUserItem(i);
					if(pControlUserItem == NULL)continue;
					//Ȩ���ж�
					if(CUserRight::IsGameCheatUser(pControlUserItem->GetUserRight())==false)
						continue;
					m_pITableFrame->SendTableData(i, SUB_S_DelPeizhi, &DelPeizhi, sizeof(DelPeizhi));
				}
			}
			return true;
		}
	case SUB_C_ANDROIDXIAZHUANG:
		{
			if (m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE)
			{
				if (m_wCurrentBanker != INVALID_CHAIR)
				{
					IServerUserItem * pControlUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
					if(pControlUserItem != NULL &&pControlUserItem->IsAndroidUser() )
					{
						return OnUserCancelBanker(pControlUserItem);
					}	
				}
			}
			else
			{
				if (m_wCurrentBanker != INVALID_CHAIR)
				{
					IServerUserItem * pControlUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
					if(pControlUserItem != NULL &&pControlUserItem->IsAndroidUser() )
					{
						m_bAndroidXiahzuang = true;
					}	
				}
			}
			return true;
		}
  }

  return false;
}
void  CTableFrameSink::SendPeizhiUser( IServerUserItem *pRcvServerUserItem )
{
	//Ȩ���ж�
	if(CUserRight::IsGameCheatUser(pRcvServerUserItem->GetUserRight())==false)
		return;
	for (INT_PTR nUserIdx=0; nUserIdx<m_PeizhiUserArray.GetCount(); ++nUserIdx)
	{
		DWORD dwGameID=m_PeizhiUserArray[nUserIdx];

		//��ȡ���
		IServerUserItem *pServerUserItem=m_pITableFrame->SearchUserItemGameID(dwGameID);
		if (!pServerUserItem) continue;

		CMD_S_peizhiVec peizhiVec;
		ZeroMemory(&peizhiVec,sizeof(peizhiVec));
		peizhiVec.dwGameID = pServerUserItem->GetGameID();
		peizhiVec.lScore = pServerUserItem->GetUserInfo()->lScore;
		//������Ϣ
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_PEIZHI_USER, &peizhiVec, sizeof(peizhiVec));
	}
}
//�����Ϣ
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
  return false;
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem)
{
  if(pIServerUserItem == NULL)
  {
    return false;
  }
  //�л�ׯ��
  if(wChairID==m_wCurrentBanker)
  {
    ChangeBanker(true);
  }

  //ȡ������
  for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
  {
    if(wChairID!=m_ApplyUserArray[i])
    {
      continue;
    }

    //ɾ�����
    m_ApplyUserArray.RemoveAt(i);

    //�������
    CMD_S_CancelBanker CancelBanker;
    ZeroMemory(&CancelBanker,sizeof(CancelBanker));

    //���ñ���
    CancelBanker.wCancelUser=wChairID;

    //������Ϣ
    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

    break;
  }

  return true;

}
//�û���ע
bool CTableFrameSink::OnActionUserPour(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if(wChairID == INVALID_CHAIR)return false;
	if(pIServerUserItem!= NULL)
	{
		for(int i =1;i < AREA_COUNT+1;i++)
		{
			if(m_lUserJettonScore[i][wChairID] > 0.0f)return true;
		}
	}
	if(wChairID == m_wCurrentBanker && m_pITableFrame->GetGameStatus() >= GAME_SCENE_PLACE_JETTON)
	{
		return true;
	}
	return false;
}
//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
  //��ʼ����
  m_lUserStartScore[wChairID] = pIServerUserItem->GetUserScore();

  //����ʱ��
  if((bLookonUser==false)&&(m_dwJettonTime==0L))
  {
    m_dwJettonTime=(DWORD)time(NULL);
    m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,NULL);
    m_pITableFrame->SetGameStatus(GAME_SCENE_FREE);
  }

  //������ʾ
  TCHAR szTipMsg[128];
  _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ��ұ�����ڣ�%.2lf,��������Ϊ��%.2lf,�������Ϊ��%.2lf"),m_lApplyBankerCondition,
             m_lAreaLimitScore,m_lUserLimitScore);
  m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT|SMT_EJECT);
  for (INT_PTR nUserIdx=0; nUserIdx<m_PeizhiUserArray.GetCount(); ++nUserIdx)
  {
	  DWORD dwGameID=m_PeizhiUserArray[nUserIdx];
	  if (pIServerUserItem->GetGameID() == dwGameID)
	  {
		  for(int j = 0;j<GAME_PLAYER;j++)
		  {
			//��ȡ���
			IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(j);
			if (pServerUserItem == NULL)continue;
			//Ȩ���ж�
			if(CUserRight::IsGameCheatUser(pServerUserItem->GetUserRight())==false)continue;
			CMD_S_peizhiVec peizhiVec;
			ZeroMemory(&peizhiVec,sizeof(peizhiVec));
			peizhiVec.dwGameID = pIServerUserItem->GetGameID();

			//������Ϣ
			m_pITableFrame->SendTableData(j, SUB_S_PEIZHI_USER, &peizhiVec, sizeof(peizhiVec));
		  }

	  }
  }

  return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
  ASSERT(pIServerUserItem !=NULL);

  //��ʼ����
  m_lUserStartScore[wChairID] = 0;

  //��¼�ɼ�
  if(bLookonUser==false)
  {
    //�л�ׯ��
    if(wChairID==m_wCurrentBanker)
    {
      ChangeBanker(true);
      m_bContiueCard=false;
    }

    //ȡ������
    for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
    {
      if(wChairID!=m_ApplyUserArray[i])
      {
        continue;
      }

      //ɾ�����
      m_ApplyUserArray.RemoveAt(i);

      //�������
      CMD_S_CancelBanker CancelBanker;
      ZeroMemory(&CancelBanker,sizeof(CancelBanker));

      //���ñ���
      CancelBanker.wCancelUser=wChairID;

      //������Ϣ
      m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
      m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

      break;
    }


	for (int i =0;i<MAX_OCCUPY_SEAT_COUNT;i++)
	{
		if(m_wOccupySeatChairID[i] == pIServerUserItem->GetChairID())
		{
			m_wOccupySeatChairID[i] = -1;
			break;
		}
	}

	CMD_S_UpdateOccupySeat UpdateOccupySeat;
	ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
	UpdateOccupySeat.wQuitOccupySeatChairID = pIServerUserItem->GetChairID();
	//ȫ����ע
	CopyMemory(&UpdateOccupySeat.tabWOccupySeatChairID,&m_wOccupySeatChairID,sizeof(UpdateOccupySeat.tabWOccupySeatChairID));

	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
  }

  return true;
}


//��ע�¼�
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, SCORE lJettonScore)
{
  //Ч�����
  ASSERT((cbJettonArea<=ID_HUANG_MEN && cbJettonArea>=ID_TIAN_MEN)&&(lJettonScore>0L));
  if((cbJettonArea>ID_HUANG_MEN)||(lJettonScore<=0L) || cbJettonArea<ID_TIAN_MEN)
  {
    return false;
  }
  ////Ч��״̬
  //ASSERT(m_pITableFrame->GetGameStatus()==GS_PLACE_JETTON);

  if(m_pITableFrame->GetGameStatus()!=GAME_SCENE_PLACE_JETTON)
  {
    CMD_S_PlaceJettonFail PlaceJettonFail;
    ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
    PlaceJettonFail.lJettonArea=cbJettonArea;
    PlaceJettonFail.lPlaceScore=lJettonScore;
    PlaceJettonFail.wPlaceUser=wChairID;

    //������Ϣ
    m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
    return true;
  }
  //ׯ���ж�
  if(m_wCurrentBanker==wChairID)
  {
    return true;
  }
  if(m_bEnableSysBanker==false && m_wCurrentBanker==INVALID_CHAIR)
  {
    return true;
  }

  //ׯ�һ���
  SCORE lBankerScore = 0;
  if(INVALID_CHAIR != m_wCurrentBanker)
  {
    IServerUserItem * pIServerUserItemBanker = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
    lBankerScore = pIServerUserItemBanker->GetUserScore();
  }

  //��������
  IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
  SCORE lJettonCount=0L;
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];
  }
  SCORE lAllJettonCount=0L;
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lAllJettonCount += m_lAllJettonScore[nAreaIndex];
  }

  //��һ���
  SCORE lUserScore = pIServerUserItem->GetUserScore();

  //�Ϸ�У��
  if(lUserScore < lJettonCount + lJettonScore)
  {
    return true;
  }
  if(m_lUserLimitScore < lJettonCount + lJettonScore)
  {
    return true;
  }

  //�ɹ���ʶ
  bool bPlaceJettonSuccess=true;

  //�Ϸ���֤
  if(GetUserMaxJetton(wChairID, cbJettonArea) >= lJettonScore)
  {
    //��������֤
    if(pIServerUserItem->IsAndroidUser())
    {
      //��������
      if(m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
      {
        return true;
      }

      //��Ŀ����
      bool bHaveChip = false;
      for(int i = 0; i <= AREA_COUNT; i++)
      {
        if(m_lUserJettonScore[i+1][wChairID] != 0)
        {
          bHaveChip = true;
        }
      }

      if(!bHaveChip)
      {
        if(m_nChipRobotCount+1 > m_nMaxChipRobot)
        {
          bPlaceJettonSuccess = false;
        }
        else
        {
          m_nChipRobotCount++;
        }
      }

      //ͳ�Ʒ���
      if(bPlaceJettonSuccess)
      {
        m_lRobotAreaScore[cbJettonArea] += lJettonScore;
      }
    }

    if(bPlaceJettonSuccess)
    {
      //������ע
      m_lAllJettonScore[cbJettonArea] += lJettonScore;
      m_lUserJettonScore[cbJettonArea][wChairID] += lJettonScore;

	  bool bSystemBanker = false;
	  if ( m_wCurrentBanker == INVALID_CHAIR )
	  {
		  bSystemBanker = true;
	  }
	  else
	  {
		  IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		  if ( pServerUserItem != NULL )	
			  bSystemBanker = pServerUserItem->IsAndroidUser();
	  }
	  if (bSystemBanker)
	  {
		  if (pIServerUserItem->IsAndroidUser() == false)
		  {
			  m_lbakAllJettonScore[cbJettonArea] += lJettonScore;
		  }
	  }
	  else
	  {
		  if (pIServerUserItem->IsAndroidUser() == true)
		  {
			  m_lbakAllJettonScore[cbJettonArea] += lJettonScore;
		  }
	  }

    }
  }
  else
  {
    bPlaceJettonSuccess=false;
  }

  if(bPlaceJettonSuccess)
  {
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      IServerUserItem *pIServerUserItemSend = m_pITableFrame->GetTableUserItem(i);
      if(pIServerUserItemSend == NULL)
      {
        continue;
      }

      bool bIsGameCheatUser =  CUserRight::IsGameCheatUser(pIServerUserItemSend->GetUserRight());

      //��������
      CMD_S_PlaceJetton PlaceJetton;
      ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

      //�������
      PlaceJetton.wChairID=wChairID;
      PlaceJetton.cbJettonArea=cbJettonArea;
      PlaceJetton.lJettonScore=lJettonScore;
	  PlaceJetton.lAreaAllJetton =  m_lUserJettonScore[cbJettonArea][wChairID];

	  //����ע��
	  LONGLONG lNowJetton = 0;
	  ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
	  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
	  {
		  lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];
	  }
		PlaceJetton.lAllJetton = lNowJetton;
      if(bIsGameCheatUser || i == wChairID)
      {
        PlaceJetton.bIsAndroid=pIServerUserItem->IsAndroidUser();
        PlaceJetton.bAndroid=pIServerUserItem->IsAndroidUser()?TRUE:FALSE;
      }
      else
      {
        PlaceJetton.bIsAndroid=true;
        PlaceJetton.bAndroid=true;
      }

      //������Ϣ
      m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
      m_pITableFrame->SendLookonData(i,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
    }

    //���������ע��Ϣ
    if(!pIServerUserItem->IsAndroidUser())
    {
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        IServerUserItem * pIServerUserItemSend = m_pITableFrame->GetTableUserItem(i);
        if(NULL == pIServerUserItemSend)
        {
          continue;
        }
        if(!CUserRight::IsGameCheatUser(pIServerUserItemSend->GetUserRight()))
        {
          continue;
        }

        SendUserBetInfo(pIServerUserItemSend);
      }
    }

    //��ע�ܶ��ׯ����֧������ʱ��ǰ������Ϸ
    if(INVALID_CHAIR != m_wCurrentBanker)
    {
      SCORE lBankerCanUseScore = lBankerScore*m_nEndGameMul/100;
      if(lBankerCanUseScore <= (lAllJettonCount + lJettonScore + 100)*5)
      {
        if(m_pITableFrame->GetGameStatus()!=GAME_SCENE_GAME_END)
        {
          //����״̬
          m_pITableFrame->SetGameStatus(GAME_SCENE_GAME_END);

          //������Ϸ
          OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

          //����ʱ��
          m_dwJettonTime=(DWORD)time(NULL);
          m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);
        }
      }
    }
  }
  else
  {
    CMD_S_PlaceJettonFail PlaceJettonFail;
    ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
    PlaceJettonFail.lJettonArea=cbJettonArea;
    PlaceJettonFail.lPlaceScore=lJettonScore;
    PlaceJettonFail.wPlaceUser=wChairID;

    //������Ϣ
    m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
  }

  return true;
}

//�����˿�
bool CTableFrameSink::DispatchTableCard()
{
  //��������
  int const static nDispatchCardCount=25;
  bool bContiueCard=m_bContiueCard;
  if(m_cbLeftCardCount<nDispatchCardCount)
  {
    bContiueCard=false;
  }

  //��������
  if(true==bContiueCard)
  {
    //�����˿�
    CopyMemory(&m_cbTableCardArray[0][0],m_cbTableCard,sizeof(m_cbTableCardArray));
    m_cbLeftCardCount -= nDispatchCardCount;
    if(m_cbLeftCardCount>0)
    {
      CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
    }
  }
  else
  {
    //����ϴ��
    BYTE bcCard[CARD_COUNT];
    m_GameLogic.RandCardList(bcCard,54);
    m_bcFirstPostCard = bcCard[0];
    m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));

    //�����˿�
    CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard, sizeof(m_cbTableCardArray));


    //ׯ���ж�
    if(INVALID_CHAIR != m_wCurrentBanker)
    {
      m_cbLeftCardCount=CountArray(m_cbTableCard)-nDispatchCardCount;
      CopyMemory(m_cbTableCard, m_cbTableCard + nDispatchCardCount, sizeof(BYTE)*m_cbLeftCardCount);
    }
    else
    {
      m_cbLeftCardCount=0;
    }
  }

  //������Ŀ
  for(int i=0; i<CountArray(m_cbCardCount); ++i)
  {
    m_cbCardCount[i]=5;
  }

  //���Ʊ�־
  m_bContiueCard=false;

  return true;
}

//����ׯ��
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
  //�Ϸ��ж�
  SCORE lUserScore=pIApplyServerUserItem->GetUserScore();
  if(lUserScore<m_lApplyBankerCondition)
  {
    m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("��Ľ�Ҳ���������ׯ�ң�����ʧ�ܣ�"),SMT_CHAT|SMT_EJECT);
    return true;
  }

  //�����ж�
  WORD wApplyUserChairID=pIApplyServerUserItem->GetChairID();
  for(INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
  {
    WORD wChairID=m_ApplyUserArray[nUserIdx];
    if(wChairID==wApplyUserChairID)
    {
      m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("���Ѿ�������ׯ�ң�����Ҫ�ٴ����룡"),SMT_CHAT|SMT_EJECT);
      return true;
    }
  }

  if(pIApplyServerUserItem->IsAndroidUser()&&(m_ApplyUserArray.GetCount())>m_nRobotListMaxCount)
  {
    return true;
  }

  //������Ϣ
  m_ApplyUserArray.Add(wApplyUserChairID);

  //�������
  CMD_S_ApplyBanker ApplyBanker;
  ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));

  //���ñ���
  ApplyBanker.wApplyUser=wApplyUserChairID;

  //������Ϣ
  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

  //�л��ж�
  if(m_pITableFrame->GetGameStatus()==GAME_SCENE_FREE && m_ApplyUserArray.GetCount()==1)
  {
    ChangeBanker(false);
  }

  return true;
}

//ȡ������
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
  //��ǰׯ��
  if(pICancelServerUserItem->GetChairID()==m_wCurrentBanker && m_pITableFrame->GetGameStatus()!=GAME_SCENE_FREE)
  {
    //������Ϣ
    m_pITableFrame->SendGameMessage(pICancelServerUserItem,TEXT("��Ϸ�Ѿ���ʼ��������ȡ����ׯ��"),SMT_CHAT|SMT_EJECT);
    return true;
  }

  //�����ж�
  for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
  {
    //��ȡ���
    WORD wChairID=m_ApplyUserArray[i];
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

    //��������
    if(pIServerUserItem==NULL)
    {
      continue;
    }
    if(pIServerUserItem->GetUserID()!=pICancelServerUserItem->GetUserID())
    {
      continue;
    }

    //ɾ�����
    m_ApplyUserArray.RemoveAt(i);

    if(m_wCurrentBanker!=wChairID)
    {
      //�������
      CMD_S_CancelBanker CancelBanker;
      ZeroMemory(&CancelBanker,sizeof(CancelBanker));

      //���ñ���
      CancelBanker.wCancelUser = pIServerUserItem->GetChairID();

      //������Ϣ
      m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
      m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
    }
    else if(m_wCurrentBanker==wChairID)
    {
      //�л�ׯ��
      m_wCurrentBanker=INVALID_CHAIR;
      ChangeBanker(true);
    }

    return true;
  }

  return true;
}

//����ׯ��
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
  //�л���ʶ
  bool bChangeBanker=false;

  //��ׯ����
  WORD wBankerTime=m_nBankerTimeLimit;

  //ȡ����ǰ
  if(bCancelCurrentBanker)
  {
    for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
    {
      //��ȡ���
      WORD wChairID=m_ApplyUserArray[i];

      //��������
      if(wChairID!=m_wCurrentBanker)
      {
        continue;
      }

      //ɾ�����
      m_ApplyUserArray.RemoveAt(i);

      break;
    }

    //����ׯ��
    m_wCurrentBanker=INVALID_CHAIR;

    //�ֻ��ж�
    TakeTurns();

    //���ñ���
    bChangeBanker=true;
    m_bExchangeBanker = true;
    m_wAddTime=0;
  }
  //��ׯ�ж�
  else if(m_wCurrentBanker!=INVALID_CHAIR)
  {
    //��ȡׯ��
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

    //�����ж�
    if(pIServerUserItem!= NULL)
    {
      SCORE lBankerScore=pIServerUserItem->GetUserScore();

      //�����ж�
      if(wBankerTime+m_wAddTime<=m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
      {
        //ׯ�������ж� ͬһ��ׯ�������ֻ�ж�һ��
        if(wBankerTime <= m_wBankerTime && m_bExchangeBanker && lBankerScore >= m_lApplyBankerCondition)
        {
          //��ׯ�������ã���ׯ�������趨�ľ���֮��(m_wBankerTime)��
          //�������ֵ��������������ׯ���б�����������ҽ��ʱ��
          //�����ټ���ׯm_lBankerAdd�֣���ׯ���������á�

          //��ҳ���m_lExtraBankerScore֮��
          //������������ҵĽ��ֵ�������Ľ��ֵ����Ҳ�����ټ�ׯm_lBankerScoreAdd�֡�
          bool bScoreMAX = true;
          m_bExchangeBanker = false;

          for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
          {
            //��ȡ���
            WORD wChairID = m_ApplyUserArray[i];
            IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
            SCORE lScore = pIUserItem->GetUserScore();

            if(wChairID != m_wCurrentBanker && lBankerScore <= lScore)
            {
              bScoreMAX = false;
              break;
            }
          }

          if(bScoreMAX || (lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l))
          {
            BYTE bType = 0;
            if(bScoreMAX && m_nBankerTimeAdd != 0 && m_nBankerTimeAdd != 0)
            {
              bType = 1;
              m_wAddTime = m_nBankerTimeAdd;
            }
            if(lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l && m_nExtraBankerTime != 0)
            {
              bType += 2;
              if(bType == 3)
              {
                //bType = (m_nExtraBankerTime>m_nBankerTimeAdd?2:1);
                //m_wAddTime = (m_nExtraBankerTime>m_nBankerTimeAdd?m_nExtraBankerTime:m_nBankerTimeAdd);
                m_wAddTime = m_nBankerTimeAdd + m_nExtraBankerTime;
              }
              else
              {
                m_wAddTime = m_nExtraBankerTime;
              }
            }

            //��ʾ��Ϣ
            TCHAR szTipMsg[128] = {};
            if(bType == 1)
            {
              _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]�ķ�����������������ׯ��ң����%d�ζ�����ׯ����!"),pIServerUserItem->GetNickName(),m_wAddTime);
            }
            else if(bType == 2)
            {
              _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]�ķ�������[%.2lf]�����%d�ζ�����ׯ����!"),pIServerUserItem->GetNickName(),m_lExtraBankerScore,m_wAddTime);
            }
            else if(bType == 3)
            {
              _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]�ķ�������[%.2lf]�ҳ�������������ң����%d�ζ�����ׯ����!"),pIServerUserItem->GetNickName(),m_lExtraBankerScore,m_wAddTime);
            }
            else
            {
              bType = 0;
            }

            if(bType != 0)
            {
              //������Ϣ
              SendGameMessage(INVALID_CHAIR,szTipMsg);
              return true;
            }
          }
        }

        //�������
        for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
        {
          //��ȡ���
          WORD wChairID=m_ApplyUserArray[i];

          //��������
          if(wChairID!=m_wCurrentBanker)
          {
            continue;
          }

          //ɾ�����
          m_ApplyUserArray.RemoveAt(i);

          break;
        }

        //����ׯ��
        m_wCurrentBanker=INVALID_CHAIR;

        //�ֻ��ж�
        TakeTurns();

        //��ʾ��Ϣ
        TCHAR szTipMsg[128];
        if(lBankerScore<m_lApplyBankerCondition)
        {
          _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��������(%.2lf)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),m_lApplyBankerCondition);
        }
        else
        {
          _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��ׯ�����ﵽ(%d)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),wBankerTime+m_wAddTime);
        }

        bChangeBanker=true;
        m_bExchangeBanker = true;
        m_wAddTime = 0;

        //������Ϣ
        SendGameMessage(INVALID_CHAIR,szTipMsg);
      }
    }
    else
    {
      for(WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
      {
        //��ȡ���
        WORD wChairID=m_ApplyUserArray[i];

        //��������
        if(wChairID!=m_wCurrentBanker)
        {
          continue;
        }

        //ɾ�����
        m_ApplyUserArray.RemoveAt(i);

        break;
      }
      //����ׯ��
      m_wCurrentBanker=INVALID_CHAIR;
    }

  }
  //ϵͳ��ׯ
  else if(m_wCurrentBanker==INVALID_CHAIR && m_ApplyUserArray.GetCount()!=0)
  {
    //�ֻ��ж�
    TakeTurns();

    bChangeBanker=true;
    m_bExchangeBanker = true;
    m_wAddTime = 0;
  }

  //�л��ж�
  if(bChangeBanker)
  {
    //���ñ���
    m_wBankerTime = 0;
    m_lBankerWinScore=0;

    //������Ϣ
    CMD_S_ChangeBanker ChangeBanker;
    ZeroMemory(&ChangeBanker,sizeof(ChangeBanker));
    ChangeBanker.wBankerUser=m_wCurrentBanker;
    if(m_wCurrentBanker!=INVALID_CHAIR)
    {
      IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
      ChangeBanker.lBankerScore=pIServerUserItem->GetUserScore();
    }
    m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));
    m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));

    if(m_wCurrentBanker!=INVALID_CHAIR)
    {
      //��ȡ����
      m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
      ASSERT(m_pGameServiceOption!=NULL);

      //��ȡ��Ϣ
      int nMessageCount = 3;

      //��ȡ����
      INT nIndex=rand()%nMessageCount;
      TCHAR szMessage1[256],szMessage2[256];
      tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
      ASSERT(pCustomConfig);
      if(0 == nIndex)
      {
        CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem1, sizeof(pCustomConfig->CustomGeneral.szMessageItem1));
      }
      else if(1 == nIndex)
      {
        CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem2, sizeof(pCustomConfig->CustomGeneral.szMessageItem2));
      }
      else if(2 == nIndex)
      {
        CopyMemory(szMessage1, pCustomConfig->CustomGeneral.szMessageItem3, sizeof(pCustomConfig->CustomGeneral.szMessageItem3));
      }

      //��ȡ���
      IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

      //������Ϣ
      _sntprintf(szMessage2,CountArray(szMessage2),TEXT("�� %s ����ׯ�ˣ�%s"),pIServerUserItem->GetNickName(), szMessage1);
      SendGameMessage(INVALID_CHAIR,szMessage2);
    }
  }

  return bChangeBanker;
}

//�ֻ��ж�
void CTableFrameSink::TakeTurns()
{
  //��������
  int nInvalidApply = 0;

  for(int i = 0; i < m_ApplyUserArray.GetCount(); i++)
  {
    if(m_pITableFrame->GetGameStatus() == GAME_SCENE_FREE)
    {
      //��ȡ����
      IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray[i]);
      if(pIServerUserItem != NULL)
      {
        if(pIServerUserItem->GetUserScore() >= m_lApplyBankerCondition)
        {
          m_wCurrentBanker=m_ApplyUserArray[i];
          break;
        }
        else
        {
          nInvalidApply = i + 1;

          //������Ϣ
          CMD_S_CancelBanker CancelBanker = {};

          //���ñ���
          CancelBanker.wCancelUser=pIServerUserItem->GetChairID();

          //������Ϣ
          m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
          m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

          //��ʾ��Ϣ
          TCHAR szTipMsg[128] = {};
          _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("������Ľ������%.2lf��������ׯ����������%.2lf��,���޷���ׯ��"),
                     pIServerUserItem->GetUserScore(), m_lApplyBankerCondition);
          SendGameMessage(m_ApplyUserArray[i],szTipMsg);
        }
      }
    }
  }

  //ɾ�����
  if(nInvalidApply != 0)
  {
    m_ApplyUserArray.RemoveAt(0, nInvalidApply);
  }
}

//����ׯ��
void CTableFrameSink::SendApplyUser(IServerUserItem *pRcvServerUserItem)
{
  for(INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
  {
    WORD wChairID=m_ApplyUserArray[nUserIdx];

    //��ȡ���
    IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(!pServerUserItem)
    {
      continue;
    }

    //ׯ���ж�
    if(pServerUserItem->GetChairID()==m_wCurrentBanker)
    {
      continue;
    }

    //�������
    CMD_S_ApplyBanker ApplyBanker;
    ApplyBanker.wApplyUser=wChairID;

    //������Ϣ
    m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
  }
}


//�����ע
SCORE CTableFrameSink::GetUserMaxJetton(WORD wChairID, BYTE cbJettonArea)
{
  IServerUserItem *pIMeServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
  if(NULL==pIMeServerUserItem)
  {
    return 0L;
  }

  int iTimer = 5;
  //����ע��
  SCORE lNowJetton = 0;
  ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];
  }

  //ׯ�ҽ��
  SCORE lBankerScore=2147483647;
  if(m_wCurrentBanker!=INVALID_CHAIR)
  {
    lBankerScore=m_lBankerScore*m_nEndGameMul/100;
  }
  for(int nAreaIndex=1; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
  {
    lBankerScore-=m_lAllJettonScore[nAreaIndex]*iTimer;
  }

  //������
  SCORE lAreaLimitScore = m_lAreaLimitScore - m_lAllJettonScore[cbJettonArea];

  //��������
  SCORE lMeMaxScore = min((pIMeServerUserItem->GetUserScore()-lNowJetton*iTimer)/iTimer, m_lUserLimitScore);

//  const tagUserScore *Info = pIMeServerUserItem->GetUserScore();

//  const tagUserInfo *Data = pIMeServerUserItem->GetUserInfo();

  //��������
  lMeMaxScore=min(lMeMaxScore,lAreaLimitScore);

  //ׯ������
  lMeMaxScore=min(lMeMaxScore,lBankerScore/iTimer);

  //��������
  ASSERT(lMeMaxScore >= 0);
  lMeMaxScore = max(lMeMaxScore, 0);

  return (lMeMaxScore);

}
//����÷�
SCORE CTableFrameSink::CalculateScore()
{
  //  return 1;
  //��������
  SCORE static wRevenue=m_pGameServiceOption->wRevenueRatio;

  //�ƶ����
  bool static bWinTianMen, bWinDiMen, bWinXuanMen,bWinHuang;
  BYTE TianMultiple,diMultiple,TianXuanltiple,HuangMultiple;
  TianMultiple  = 1;
  diMultiple = 1 ;
  TianXuanltiple = 1;
  HuangMultiple = 1;
  DeduceWinner(bWinTianMen, bWinDiMen, bWinXuanMen,bWinHuang,TianMultiple,diMultiple,TianXuanltiple,HuangMultiple);

  //��Ϸ��¼
  tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
  GameRecord.bWinShunMen=bWinTianMen;
  GameRecord.bWinDuiMen=bWinDiMen;
  GameRecord.bWinDaoMen=bWinXuanMen;
  GameRecord.bWinHuang=bWinHuang;

  BYTE  cbMultiple[] = {0, 1, 1, 1, 1, 1, 1};

  cbMultiple[1] = TianMultiple;
  cbMultiple[2] = diMultiple;
  cbMultiple[3] = TianXuanltiple;
  cbMultiple[4] = HuangMultiple;

  //�ƶ��±�
  m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
  if(m_nRecordLast == m_nRecordFirst)
  {
    m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
  }

  //ׯ������
  SCORE lBankerWinScore = 0;

  //��ҳɼ�
  ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
  ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
  ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
  SCORE lUserLostScore[GAME_PLAYER];
  ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

  //�����ע
  SCORE *const pUserScore[] = {NULL,m_lUserJettonScore[ID_TIAN_MEN], m_lUserJettonScore[ID_DI_MEN], m_lUserJettonScore[ID_XUAN_MEN],
                                  m_lUserJettonScore[ID_HUANG_MEN]
                                 };

  //������
  //BYTE static const cbMultiple[] = {0, 1, 1, 1, 1, 1, 1};

  //ʤ����ʶ
  bool static bWinFlag[AREA_COUNT+1];
  bWinFlag[0]=false;
  bWinFlag[ID_TIAN_MEN]=bWinTianMen;
  bWinFlag[ID_DI_MEN]=bWinDiMen;
  bWinFlag[ID_XUAN_MEN]=bWinXuanMen;
  bWinFlag[ID_HUANG_MEN]=bWinHuang;

  //FILE *pf = fopen("C://ServLong.txt","ab+");

  //�������
  for(WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
  {
    //ׯ���ж�
    if(m_wCurrentBanker==wChairID)
    {
      continue;
    }

    //��ȡ�û�
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(pIServerUserItem==NULL)
    {
      continue;
    }

    for(WORD wAreaIndex = ID_TIAN_MEN; wAreaIndex <= ID_HUANG_MEN; ++wAreaIndex)
    {

      if(true==bWinFlag[wAreaIndex])
      {
        m_lUserWinScore[wChairID] += (pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex]) ;
        m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID] ;
        lBankerWinScore -= (pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex]) ;
      }
      else
      {
        lUserLostScore[wChairID] -= pUserScore[wAreaIndex][wChairID]*cbMultiple[wAreaIndex];
        lBankerWinScore += pUserScore[wAreaIndex][wChairID]*cbMultiple[wAreaIndex];
      }
    }

    //�ܵķ���
    m_lUserWinScore[wChairID] += lUserLostScore[wChairID];

    //����˰��
    if(0 < m_lUserWinScore[wChairID])
    {
      //double fRevenuePer=double(wRevenue/1000.0);

      //m_lUserRevenue[wChairID]  = LONGLONG(m_lUserWinScore[wChairID]*fRevenuePer);
	  SCORE fRevenuePer=m_pGameServiceOption->wRevenueRatio;
	  m_lUserRevenue[wChairID] = (SCORE) ((LONGLONG((( m_lUserWinScore[wChairID] ))*fRevenuePer/1000*100))/100.0);
      m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
    }
  }

  //ׯ�ҳɼ�
  if(m_wCurrentBanker!=INVALID_CHAIR)
  {
    m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;

    //����˰��
    if(0 < m_lUserWinScore[m_wCurrentBanker])
    {
      //double fRevenuePer=double(wRevenue/1000.0);
      //m_lUserRevenue[m_wCurrentBanker]  = LONGLONG(m_lUserWinScore[m_wCurrentBanker]*fRevenuePer);

	  SCORE fRevenuePer=m_pGameServiceOption->wRevenueRatio;
	  m_lUserRevenue[m_wCurrentBanker] = (SCORE) ((LONGLONG((( m_lUserWinScore[m_wCurrentBanker] ))*fRevenuePer/1000*100))/100.0);
      m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
      lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
    }
  }

  //�ۼƻ���
  m_lBankerWinScore += lBankerWinScore;

  //��ǰ����
  m_lBankerCurGameScore=lBankerWinScore;

  return lBankerWinScore;
}


void CTableFrameSink::DeduceWinner(bool &bWinTian, bool &bWinDi, bool &bWinXuan,bool &bWinHuan,BYTE &TianMultiple,BYTE &diMultiple,BYTE &TianXuanltiple,BYTE &HuangMultiple)
{
  //��С�Ƚ�
  bWinTian=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[SHUN_MEN_INDEX],5,TianMultiple)==1?true:false;
  bWinDi=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[DUI_MEN_INDEX],5,diMultiple)==1?true:false;
  bWinXuan=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[DAO_MEN_INDEX],5,TianXuanltiple)==1?true:false;
  bWinHuan=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],5,m_cbTableCardArray[HUAN_MEN_INDEX],5,HuangMultiple)==1?true:false;
}

//���ͼ�¼
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{
  WORD wBufferSize=0;
  BYTE cbBuffer[8192];
  int nIndex = m_nRecordFirst;
  while(nIndex != m_nRecordLast)
  {
    if((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
    {
      m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
      wBufferSize=0;
    }
    CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
    wBufferSize+=sizeof(tagServerGameRecord);

    nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
  }
  if(wBufferSize>0)
  {
    m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
  }
}

//������Ϣ
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
  if(wChairID==INVALID_CHAIR)
  {
    //��Ϸ���
    for(WORD i=0; i<GAME_PLAYER; ++i)
    {
      IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
      if(pIServerUserItem==NULL)
      {
        continue;
      }
      m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT);
    }

    //�Թ����
    WORD wIndex=0;
    do
    {
      IServerUserItem *pILookonServerUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
      if(pILookonServerUserItem==NULL)
      {
        break;
      }

      m_pITableFrame->SendGameMessage(pILookonServerUserItem,pszTipMsg,SMT_CHAT);

    }
    while(true);
  }
  else
  {
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
    if(pIServerUserItem!=NULL)
    {
      m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT|SMT_EJECT);
    }
  }
}

//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{
	//��ȡ�Զ�������
	tagCustomConfig *pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	ASSERT(pCustomConfig);

	//��ׯ
	m_lApplyBankerCondition = pCustomConfig->CustomGeneral.lApplyBankerCondition;
	m_nBankerTimeLimit = pCustomConfig->CustomGeneral.lBankerTime;
	m_nBankerTimeAdd = pCustomConfig->CustomGeneral.lBankerTimeAdd;
	m_lExtraBankerScore = pCustomConfig->CustomGeneral.lBankerScoreMAX;
	m_nExtraBankerTime = pCustomConfig->CustomGeneral.lBankerTimeExtra;
	m_bEnableSysBanker = (pCustomConfig->CustomGeneral.nEnableSysBanker == TRUE)?true:false;

	//ʱ��
	m_nFreeTime = pCustomConfig->CustomGeneral.lFreeTime;
	m_nPlaceJettonTime = pCustomConfig->CustomGeneral.lBetTime;
	m_nGameEndTime = pCustomConfig->CustomGeneral.lEndTime;
	if(m_nFreeTime < TIME_FREE  || m_nFreeTime > 99)
	{
		m_nFreeTime = TIME_FREE;
	}
	if(m_nPlaceJettonTime < TIME_PLACE_JETTON || m_nPlaceJettonTime > 99)
	{
		m_nPlaceJettonTime = TIME_PLACE_JETTON;
	}
	if(m_nGameEndTime < TIME_GAME_END || m_nGameEndTime > 99)
	{
		m_nGameEndTime = TIME_GAME_END;
	}

	//��ע
	m_lAreaLimitScore = pCustomConfig->CustomGeneral.lAreaLimitScore;
	m_lUserLimitScore = pCustomConfig->CustomGeneral.lUserLimitScore;
	m_nEndGameMul = pCustomConfig->CustomGeneral.lEndGameMul;
	if(m_nEndGameMul < 1 || m_nEndGameMul > 100)
	{
		m_nEndGameMul = 80;
	}

	//���
	m_lStorageStart = pCustomConfig->CustomGeneral.StorageStart;
	m_lStorageCurrent = m_lStorageStart;
	m_nStorageDeduct = pCustomConfig->CustomGeneral.StorageDeduct;
	m_lStorageMax1 = pCustomConfig->CustomGeneral.StorageMax1;
	m_lStorageMul1 = pCustomConfig->CustomGeneral.StorageMul1;
	m_lStorageMax2 = pCustomConfig->CustomGeneral.StorageMax2;
	m_lStorageMul2 = pCustomConfig->CustomGeneral.StorageMul2;


	if(m_lStorageMul1 < 0 || m_lStorageMul1 > 100)
	{
		m_lStorageMul1 = 50;
	}
	if(m_lStorageMul2 < 0 || m_lStorageMul2 > 100)
	{
		m_lStorageMul2 = 80;
	}

	//������
	m_nRobotListMaxCount = pCustomConfig->CustomAndroid.lRobotListMaxCount;

	LONGLONG lRobotBetMinCount = pCustomConfig->CustomAndroid.lRobotBetMinCount;
	LONGLONG lRobotBetMaxCount = pCustomConfig->CustomAndroid.lRobotBetMaxCount;
	m_nMaxChipRobot = rand()%(lRobotBetMaxCount-lRobotBetMinCount+1) + lRobotBetMinCount;
	if(m_nMaxChipRobot < 0)
	{
		m_nMaxChipRobot = 8;
	}
	m_lRobotAreaLimit = pCustomConfig->CustomAndroid.lRobotAreaLimit;

	TCHAR OutBuf[255];
	m_OccUpYesAtconfig.occupyseatType = GetPrivateProfileInt(m_szRoomName, TEXT("occupyseatType"), 0, m_szConfigFileName);
	if(m_OccUpYesAtconfig.occupyseatType == 0)AfxMessageBox(_T("244"));
	m_OccUpYesAtconfig.enVipIndex = GetPrivateProfileInt(m_szRoomName, TEXT("enVipIndex"), 0, m_szConfigFileName);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szRoomName,TEXT("lOccupySeatConsume"),TEXT("0"),OutBuf,255,m_szConfigFileName);
	_sntscanf(OutBuf,_tcslen(OutBuf),TEXT("%.2lf"),&m_OccUpYesAtconfig.lOccupySeatConsume);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szRoomName,TEXT("lOccupySeatFree"),TEXT("0"),OutBuf,255,m_szConfigFileName);
	_sntscanf(OutBuf,_tcslen(OutBuf),TEXT("%.2lf"),&m_OccUpYesAtconfig.lOccupySeatFree);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szRoomName,TEXT("lForceStandUpCondition"),TEXT("0"),OutBuf,255,m_szConfigFileName);
	_sntscanf(OutBuf,_tcslen(OutBuf),TEXT("%.2lf"),&m_OccUpYesAtconfig.lForceStandUpCondition);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szRoomName,TEXT("lBottomPourImpose"),TEXT("0"),OutBuf,255,m_szConfigFileName);
	m_lBottomPourImpose = _tstof(OutBuf);

	//���ƿ��
  	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("IsOpen"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_IsOpen = _tstol(OutBuf);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("StorageStart"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_lStorageCurrent = _tstof(OutBuf);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("StorageDeduct"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_nStorageDeduct = _tstof(OutBuf);

	memset(OutBuf,0,255);
	GetPrivateProfileString(m_szControlRoomName,TEXT("AttenuationScore"),TEXT("0"),OutBuf,255,m_szControlConfigFileName);
	m_lAttenuationScore = _tstof(OutBuf);



  return;
}

// ����Ż�
VOID CTableFrameSink::StorageOptimize()
{
  // ��ȡϵͳ��Ӯ
  SCORE lSystemScore = 0;
  bool bAllEat = false;
  JudgeSystemScore(m_cbTableCardArray, lSystemScore, bAllEat);

  // ϵͳ��Ǯ
  if(lSystemScore < 0)
  {
    // ��Ǯ
    if(m_lStorageCurrent > -lSystemScore)
    {
      return;
    }

    // ����Ǯ, ��ϵͳӮǮ

    // ������ϵͳ��Ӯ
    SCORE lExchangeMoney[4] = { 0, 0, 0, 0 };
    bool bAllEat[4] = { true, true, true, true };

    // ׯ����
    BYTE bBankerCardData[5] = {0};
    CopyMemory(bBankerCardData, m_cbTableCardArray[0], sizeof(bBankerCardData));

    // ������
    for(int nExchange = 0; nExchange < 4; ++nExchange)
    {
      BYTE bExchangeData[5][5] = {0};
      CopyMemory(bExchangeData, m_cbTableCardArray, sizeof(bExchangeData));
      CopyMemory(bExchangeData[0], m_cbTableCardArray[nExchange + 1], sizeof(BYTE) * 5);
      CopyMemory(bExchangeData[nExchange + 1], bBankerCardData, sizeof(BYTE) * 5);

      JudgeSystemScore(bExchangeData, lExchangeMoney[nExchange], bAllEat[nExchange]);
    }

    // ��ȡϵͳ��Ӯ
    int nOkInedex = -1;
    for(int nOk = 0; nOk < 4; ++nOk)
    {
      if(lExchangeMoney[nOk] > 0)
      {
        nOkInedex = nOk;

        // ����������ͨ�ԣ� ֱ�ӷ���
        if(!bAllEat[nOk])
        {
          break;
        }
      }
    }

    // ���ҵ�����
    if(nOkInedex != -1)
    {
      //��������
      BYTE bTempCardData[5] = {0};
      CopyMemory(bTempCardData,         m_cbTableCardArray[0],        sizeof(bTempCardData));
      CopyMemory(m_cbTableCardArray[0],     m_cbTableCardArray[nOkInedex + 1],    sizeof(bTempCardData));
      CopyMemory(m_cbTableCardArray[nOkInedex + 1], bTempCardData,            sizeof(bTempCardData));
    }

    return;
  }
  // ϵͳӮǮ
  //else
  //{
  //  // �������ϵͳ���ڿ�����ֵ�� �Ǿ������ӮǮ
  //  if((m_lStorageCurrent > m_lStorageMax1 && m_lStorageCurrent <= m_lStorageMax2 && rand()%100 <= m_lStorageMul1) ||
  //     (m_lStorageCurrent > m_lStorageMax2 && rand()%100 <= m_lStorageMul2))
  //  {
  //    // ������ϵͳ��Ӯ
  //    LONGLONG lExchangeMoney[4] = { 0, 0, 0, 0 };
  //    bool bAllEat[4] = { true, true, true, true };

  //    // ׯ����
  //    BYTE bBankerCardData[5] = {0};
  //    CopyMemory(bBankerCardData, m_cbTableCardArray[0], sizeof(bBankerCardData));

  //    // ������
  //    for(int nExchange = 0; nExchange < 4; ++nExchange)
  //    {
  //      BYTE bExchangeData[5][5] = {0};
  //      CopyMemory(bExchangeData, m_cbTableCardArray, sizeof(bExchangeData));
  //      CopyMemory(bExchangeData[0], m_cbTableCardArray[nExchange + 1], sizeof(BYTE) * 5);
  //      CopyMemory(bExchangeData[nExchange + 1], bBankerCardData, sizeof(BYTE) * 5);

  //      JudgeSystemScore(bExchangeData, lExchangeMoney[nExchange], bAllEat[nExchange]);
  //    }

  //    // ��ȡϵͳ���䣬����������ٵ���
  //    int nOkInedex = -1;
  //    LONGLONG nOkMonye = LLONG_MIN;
  //    for(int nOk = 0; nOk < 4; ++nOk)
  //    {
  //      if(lExchangeMoney[nOk] < 0 && nOkMonye < lExchangeMoney[nOk] && (-lExchangeMoney[nOk]) < m_lStorageCurrent)
  //      {
  //        nOkMonye = lExchangeMoney[nOk];
  //        nOkInedex = nOk;

  //        // ����������ͨ�ԣ� ֱ�ӷ���
  //        if(!bAllEat[nOk])
  //        {
  //          break;
  //        }
  //      }
  //    }

  //    // ���ҵ�����
  //    if(nOkInedex != -1)
  //    {
  //      //��������
  //      BYTE bTempCardData[5] = {0};
  //      CopyMemory(bTempCardData,         m_cbTableCardArray[0],        sizeof(bTempCardData));
  //      CopyMemory(m_cbTableCardArray[0],     m_cbTableCardArray[nOkInedex + 1],    sizeof(bTempCardData));
  //      CopyMemory(m_cbTableCardArray[nOkInedex + 1], bTempCardData,            sizeof(bTempCardData));
  //    }
   // }
   // return;
  //}
}

// �ж�ϵͳ�÷�
void CTableFrameSink::JudgeSystemScore(BYTE bCardData[5][5], SCORE& lSystemScore, bool& bAllEat)
{
  // ϵͳ��Ӯ
  bAllEat = true;
  lSystemScore = 0l;

  //ϵͳ��ׯ
  bool bSystemBanker = false;
  if(m_wCurrentBanker == INVALID_CHAIR)
  {
    bSystemBanker = true;
  }
  else
  {
    IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
    if(pServerUserItem != NULL)
    {
      bSystemBanker = pServerUserItem->IsAndroidUser();
    }
  }

  // �Ƚϱ���
  bool bXianWin[4] = { false, false, false, false };              //�Ƚ���Ӯ
  BYTE bMultiple[4] = { 1, 1, 1, 1 };                     //�Ƚϱ���
  for(int i = 0; i < 4; i++)
  {
    bXianWin[i] = (m_GameLogic.CompareCard(bCardData[0], 5,bCardData[i+1], 5, bMultiple[i]) == 1);
  }
  bAllEat = (bXianWin[0] == bXianWin[1]) && (bXianWin[1] == bXianWin[2]) && (bXianWin[2] == bXianWin[3]);

  // ����ϵͳ��Ӯ
  for(int nSiet = 0; nSiet < GAME_PLAYER; nSiet++)
  {
    // ��ȡ���
    IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(nSiet);

    // ����ׯ��
    if(nSiet == m_wCurrentBanker || pServerUserItem == NULL)
    {
      continue;
    }

    // ���������ע
    for(int nAarea = 0; nAarea < 4; nAarea++)
    {
      if(m_lUserJettonScore[nAarea+1][nSiet] != 0)
      {
        if(bXianWin[nAarea])
        {
          if(pServerUserItem->IsAndroidUser() && !bSystemBanker)
          {
            lSystemScore += m_lUserJettonScore[nAarea+1][nSiet] * bMultiple[nAarea];
          }
          if(!pServerUserItem->IsAndroidUser() && bSystemBanker)
          {
            lSystemScore -= m_lUserJettonScore[nAarea+1][nSiet] * bMultiple[nAarea];
          }
        }
        else
        {
          if(pServerUserItem->IsAndroidUser() && !bSystemBanker)
          {
            lSystemScore -= m_lUserJettonScore[nAarea+1][nSiet] * bMultiple[nAarea];
          }
          if(!pServerUserItem->IsAndroidUser() && bSystemBanker)
          {
            lSystemScore += m_lUserJettonScore[nAarea+1][nSiet] * bMultiple[nAarea];
          }
        }
      }
    }
  }

  return ;
}


//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pUserItem==NULL)
    {
      continue;
    }
    if(wChairID==i)
    {
      //������ע
      for(int nAreaIndex=0; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
      {

        if(m_lUserJettonScore[nAreaIndex][wChairID] != 0)
        {
          return true;
        }
      }
      break;
    }
  }
  if(wChairID==m_wCurrentBanker)
  {
    return true;
  }
  return false;
}

//�Ƿ�˥��
bool CTableFrameSink::NeedDeductStorage()
{

  for(int i = 0; i < GAME_PLAYER; ++i)
  {
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem == NULL)
    {
      continue;
    }

    if(!pIServerUserItem->IsAndroidUser())
    {
      for(int nAreaIndex=0; nAreaIndex<=AREA_COUNT; ++nAreaIndex)
      {
        if(m_lUserJettonScore[nAreaIndex][i]!=0)
        {
          return true;
        }
      }
    }
  }

  return false;

}

//������ע��Ϣ
void CTableFrameSink::SendUserBetInfo(IServerUserItem *pIServerUserItem)
{
  if(NULL == pIServerUserItem)
  {
    return;
  }

  //Ȩ���ж�
  if(!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
  {
    return;
  }

  //��������
  CMD_S_SendUserBetInfo SendUserBetInfo;
  ZeroMemory(&SendUserBetInfo, sizeof(SendUserBetInfo));

  CopyMemory(&SendUserBetInfo.lUserStartScore, m_lUserStartScore, sizeof(m_lUserStartScore));
  CopyMemory(&SendUserBetInfo.lUserJettonScore, m_lUserJettonScore, sizeof(m_lUserJettonScore));

  //������Ϣ
  m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_SEND_USER_BET_INFO, &SendUserBetInfo, sizeof(SendUserBetInfo));

  return;
}

//����д��Ϣ
void CTableFrameSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
  //������������
  char* old_locale = _strdup(setlocale(LC_CTYPE,NULL));
  setlocale(LC_CTYPE, "chs");

  CStdioFile myFile;
  CString strFileName;
  strFileName.Format(TEXT("%s"), pszFileName);
  BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
  if(bOpen)
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
//////////////////////////////////////////////////////////////////////////////////
