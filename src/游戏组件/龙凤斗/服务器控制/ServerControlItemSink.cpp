#include "StdAfx.h"
#include "servercontrolitemsink.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////

CServerControlItemSink::CServerControlItemSink(void)
{
	//���Ʊ���
	//m_cbWinSideControl=0;
	ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
	m_nSendCardCount=0;
	m_cbExcuteTimes = 0;
	m_nControlCount = 0;
	//�˿���Ϣ
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//����������
bool __cdecl CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption)
{
	//��������й���ԱȨ�� �򷵻ش���
	if( !CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) )
		return false;

	const CMD_C_AdminReq* AdminReq=static_cast<const CMD_C_AdminReq*>(pDataBuffer);
	switch(AdminReq->cbReqType)
	{
	case RQ_RESET_CONTROL:
		{
			//m_cbWinSideControl=0;
			ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
			m_cbExcuteTimes=0;
			m_nControlCount = 0;
			CMD_S_CommandResult cResult;
			cResult.cbAckType=ACK_RESET_CONTROL;
			cResult.cbResult=CR_ACCEPT;
			pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

			//������Ϣ
			ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

			break;
		}
	case RQ_SET_WIN_AREA:
		{	
			m_nControlCount = 0;
			for(int i=0; i<AREA_MAX; i++)
			{
				if(AdminReq->bWinArea[i] == 1) m_nControlCount++;
			}
			if(m_nControlCount == 1)
			{
				CopyMemory(m_bWinArea,AdminReq->bWinArea,sizeof(m_bWinArea));
				m_cbExcuteTimes=AdminReq->nControlTimes;
				CMD_S_CommandResult cResult;
				cResult.cbAckType=ACK_SET_WIN_AREA;
				cResult.cbResult=CR_ACCEPT;
				pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

				//������Ϣ
				ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);
			}
			else
			{
				CMD_S_CommandResult cResult;
				cResult.cbAckType=ACK_SET_WIN_AREA;
				cResult.cbResult=CR_REFUSAL;
				pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

				//������Ϣ
				ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);
			}
			break;
		}
	case RQ_PRINT_SYN:
		{
			CMD_S_CommandResult cResult;
			cResult.cbAckType=ACK_PRINT_SYN;
			cResult.cbResult=CR_ACCEPT;
			CopyMemory(cResult.bWinArea,m_bWinArea,sizeof(cResult.bWinArea));
			cResult.nControlTimes=m_cbExcuteTimes;
			pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
			break;

			//������Ϣ
			ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);
		}

	}

	return true;
}

//��Ҫ����
bool __cdecl CServerControlItemSink::NeedControl()
{
	if (m_nControlCount>0 && m_cbExcuteTimes > 0)
	{
		return true;
	}
	return false;
	
}
//��ʼ����
void  __cdecl CServerControlItemSink::GetControlArea(BYTE Area[], BYTE &cbControlTimes)
{
	for(BYTE cbIndex =0; cbIndex<AREA_MAX; cbIndex++)
	{
		Area[cbIndex] = m_bWinArea[cbIndex];
	}
	cbControlTimes = m_cbExcuteTimes;
}
//���ؿ�������
bool __cdecl CServerControlItemSink::ControlResult(BYTE	cbTableCardArray[], BYTE cbCardCount[])
{
	bool bFindCard = false;	
	if (m_nControlCount == 1)
	{
		BYTE cbControlArea = -1;
		for (int i=0; i<AREA_MAX;i++)
		{
			if (m_bWinArea[i] == 1) cbControlArea = i;
		}

		do 
		{
			DispatchCard();	

			BYTE cbWinArea[AREA_MAX] = {FALSE};
			DeduceWinner(cbWinArea);

			if (cbWinArea[cbControlArea] == true)	bFindCard = true;

		} while (!bFindCard);

		if (bFindCard == true)
		{
			CopyMemory(cbCardCount,m_cbCardCount,sizeof(BYTE)*1);
			CopyMemory(cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
		}
	}

	m_cbExcuteTimes--;
	return true;
}

//������
VOID CServerControlItemSink::ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption)
{
	const CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
	CString str;
	switch(pResult->cbAckType)
	{
	case ACK_SET_WIN_AREA:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				switch(m_cbWinSideControl)
				{
				case 0:str.Format(TEXT("���ܿ�����"));break;
				case 1:str.Format(TEXT("ִ�п��Ƴɹ�,ʤ������:�ж���,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 2:str.Format(TEXT("ִ�п��Ƴɹ�,ʤ������:  ��  ,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 3:str.Format(TEXT("ִ�п��Ƴɹ�,ʤ������:������,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 4:str.Format(TEXT("ִ�п��Ƴɹ�,ʤ������:  ƽ  ,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 5:str.Format(TEXT("ִ�п��Ƴɹ�,ʤ������:ͬ��ƽ,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 6:str.Format(TEXT("ִ�п��Ƴɹ�,ʤ������:ׯ����,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 7:str.Format(TEXT("ִ�п��Ƴɹ�,ʤ������:  ׯ  ,ִ�д���:%d"),m_cbExcuteTimes);break;
				case 8:str.Format(TEXT("ִ�п��Ƴɹ�,ʤ������:ׯ����,ִ�д���:%d"),m_cbExcuteTimes);break;
				default:break;
				}
			}
			else
			{
				str.Format(TEXT("ִ�п���ʧ��!"));
			}
			break;
		}
	case ACK_RESET_CONTROL:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str=TEXT("ȡ������!");
			}
			break;
		}
	case ACK_PRINT_SYN:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str=TEXT("������ͬ���ɹ�!");
			}
			else
			{
				str.Format(TEXT("������ͬ��ʧ��!"));
			}
			break;
		}

	default: break;
	}
	
	//��¼��Ϣ
	CString strControlInfo;
	CTime Time(CTime::GetCurrentTime());
	strControlInfo.Format(TEXT("����: %s | ����: %u | ʱ��: %d-%d-%d %d:%d:%d\n�������˺�: %s | ������ID: %u\n%s\r\n"),
		pGameServiceOption->szServerName, pITableFrame->GetTableID()+1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
		Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

	WriteInfo(TEXT("�°ټ��ֿ�����Ϣ.log"),strControlInfo);

	return;
}

//��¼��Ϣ
VOID CServerControlItemSink::WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString )
{
	//������������
	char* old_locale = _strdup( setlocale(LC_CTYPE,NULL) );
	setlocale( LC_CTYPE, "chs" );

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("%s"), pszFileName);
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
	if ( bOpen )
	{	
		myFile.SeekToEnd();
		myFile.WriteString( pszString );
		myFile.Flush();
		myFile.Close();
	}

	//��ԭ�����趨
	setlocale( LC_CTYPE, old_locale );
	free( old_locale );

	return;
}

bool CServerControlItemSink::DispatchCard()
{
	//����˿�
	m_GameLogic.RandCardList(m_cbTableCardArray[0],sizeof(m_cbTableCardArray)/sizeof(m_cbTableCardArray[0][0]));

	m_cbCardCount[INDEX_PLAYER] = 1;
	m_cbCardCount[INDEX_BANKER] = 1;

	return true;
}

void CServerControlItemSink::DeduceWinner(BYTE* pWinArea)
{
	//�����Ƶ�
	//BYTE cbPlayerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER],m_cbCardCount[INDEX_PLAYER]);
	//pWinArea[cbPlayerCount+3]  = true;
	//BYTE cbBankerCount = m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);
	//pWinArea[cbBankerCount+3]  = true;

	int nWin=m_GameLogic.CompareCard(m_cbTableCardArray[0],1,m_cbTableCardArray[1],1);

	if(nWin == -1)
		pWinArea[0] = true;
	else if(nWin == 1)
		pWinArea[2] = true;
	else if(nWin == 0)
		pWinArea[1] = true;
}
