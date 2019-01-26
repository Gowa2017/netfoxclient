#include "StdAfx.h"
#include "servercontrolitemsink.h"
#include <locale>
//////////////////////////////////////////////////////////////////////////

CServerControlItemSink::CServerControlItemSink(void)
{
	//控制变量
	//m_cbWinSideControl=0;
	ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
	m_nSendCardCount=0;
	m_cbExcuteTimes = 0;
	m_nControlCount = 0;
	//扑克信息
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//服务器控制
bool __cdecl CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption)
{
	//如果不具有管理员权限 则返回错误
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

			//控制信息
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

				//控制信息
				ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);
			}
			else
			{
				CMD_S_CommandResult cResult;
				cResult.cbAckType=ACK_SET_WIN_AREA;
				cResult.cbResult=CR_REFUSAL;
				pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

				//控制信息
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

			//控制信息
			ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);
		}

	}

	return true;
}

//需要控制
bool __cdecl CServerControlItemSink::NeedControl()
{
	if (m_nControlCount>0 && m_cbExcuteTimes > 0)
	{
		return true;
	}
	return false;
	
}
//开始控制
void  __cdecl CServerControlItemSink::GetControlArea(BYTE Area[], BYTE &cbControlTimes)
{
	for(BYTE cbIndex =0; cbIndex<AREA_MAX; cbIndex++)
	{
		Area[cbIndex] = m_bWinArea[cbIndex];
	}
	cbControlTimes = m_cbExcuteTimes;
}
//返回控制区域
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

//申请结果
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
				case 0:str.Format(TEXT("无受控区域"));break;
				case 1:str.Format(TEXT("执行控制成功,胜利区域:闲对子,执行次数:%d"),m_cbExcuteTimes);break;
				case 2:str.Format(TEXT("执行控制成功,胜利区域:  闲  ,执行次数:%d"),m_cbExcuteTimes);break;
				case 3:str.Format(TEXT("执行控制成功,胜利区域:闲天王,执行次数:%d"),m_cbExcuteTimes);break;
				case 4:str.Format(TEXT("执行控制成功,胜利区域:  平  ,执行次数:%d"),m_cbExcuteTimes);break;
				case 5:str.Format(TEXT("执行控制成功,胜利区域:同点平,执行次数:%d"),m_cbExcuteTimes);break;
				case 6:str.Format(TEXT("执行控制成功,胜利区域:庄对子,执行次数:%d"),m_cbExcuteTimes);break;
				case 7:str.Format(TEXT("执行控制成功,胜利区域:  庄  ,执行次数:%d"),m_cbExcuteTimes);break;
				case 8:str.Format(TEXT("执行控制成功,胜利区域:庄天王,执行次数:%d"),m_cbExcuteTimes);break;
				default:break;
				}
			}
			else
			{
				str.Format(TEXT("执行控制失败!"));
			}
			break;
		}
	case ACK_RESET_CONTROL:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str=TEXT("取消控制!");
			}
			break;
		}
	case ACK_PRINT_SYN:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str=TEXT("服务器同步成功!");
			}
			else
			{
				str.Format(TEXT("服务器同步失败!"));
			}
			break;
		}

	default: break;
	}
	
	//记录信息
	CString strControlInfo;
	CTime Time(CTime::GetCurrentTime());
	strControlInfo.Format(TEXT("房间: %s | 桌号: %u | 时间: %d-%d-%d %d:%d:%d\n控制人账号: %s | 控制人ID: %u\n%s\r\n"),
		pGameServiceOption->szServerName, pITableFrame->GetTableID()+1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
		Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

	WriteInfo(TEXT("新百家乐控制信息.log"),strControlInfo);

	return;
}

//记录信息
VOID CServerControlItemSink::WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString )
{
	//设置语言区域
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

	//还原区域设定
	setlocale( LC_CTYPE, old_locale );
	free( old_locale );

	return;
}

bool CServerControlItemSink::DispatchCard()
{
	//随机扑克
	m_GameLogic.RandCardList(m_cbTableCardArray[0],sizeof(m_cbTableCardArray)/sizeof(m_cbTableCardArray[0][0]));

	m_cbCardCount[INDEX_PLAYER] = 1;
	m_cbCardCount[INDEX_BANKER] = 1;

	return true;
}

void CServerControlItemSink::DeduceWinner(BYTE* pWinArea)
{
	//计算牌点
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
