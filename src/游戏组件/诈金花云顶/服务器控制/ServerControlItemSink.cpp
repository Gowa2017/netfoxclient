#include "StdAfx.h"
#include "servercontrolitemsink.h"

//
CServerControlItemSink::CServerControlItemSink(void)
{
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//服务器控制
bool __cdecl CServerControlItemSink::ServerControl( BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], ITableFrame * pITableFrame, WORD wPlayerCount)
{
	CString strInfo = TEXT("\n");
	for( int i = 0; i < wPlayerCount; ++i )
	{
		IServerUserItem* pTableUserItem = pITableFrame->GetTableUserItem(i);
		if( pTableUserItem == NULL )
			continue;

		strInfo += pTableUserItem->GetNickName();
		strInfo += TEXT("\n");

		for( int j = 0; j < MAX_COUNT; ++j )
		{
			strInfo += GetCradInfo(cbHandCardData[i][j]);
		}
		strInfo += TEXT("\n");
	}

	for( int i = 0; i < wPlayerCount; ++i )
	{
		IServerUserItem*  pTableUserItem = pITableFrame->GetTableUserItem(i);
		if( pTableUserItem == NULL )
			continue;

		if( CUserRight::IsGameCheatUser(pTableUserItem->GetUserRight()) )
		{
			////聊天框显示
			//pITableFrame->SendGameMessage(pTableUserItem, strInfo, SMT_CHAT);

			//明牌显示
			CMD_S_CheatCardInfo cheatCard;
			CopyMemory(cheatCard.cbAllHandCardData,cbHandCardData,sizeof(cheatCard));

			pITableFrame->SendTableData(i,SUB_S_CHEAT_CARD,&cheatCard,sizeof(cheatCard));
		}
	}

	int nLookonCount = 0;
	IServerUserItem* pLookonUserItem = pITableFrame->EnumLookonUserItem(nLookonCount);
	while( pLookonUserItem )
	{
		//if( CUserRight::IsGameCheatUser(pLookonUserItem->GetUserRight()) )
		//	pITableFrame->SendGameMessage(pLookonUserItem, strInfo, SMT_CHAT);

		nLookonCount++;
		pLookonUserItem = pITableFrame->EnumLookonUserItem(nLookonCount);
	}

	return true;
}

//获取牌信息
CString CServerControlItemSink::GetCradInfo( BYTE cbCardData )
{
	CString strInfo;
	if( (cbCardData&LOGIC_MASK_COLOR) == 0x00 )
		strInfo += TEXT("[方块 ");
	else if( (cbCardData&LOGIC_MASK_COLOR) == 0x10 )
		strInfo += TEXT("[梅花 ");
	else if( (cbCardData&LOGIC_MASK_COLOR) == 0x20 )
		strInfo += TEXT("[红桃 ");
	else if( (cbCardData&LOGIC_MASK_COLOR) == 0x30 )
		strInfo += TEXT("[黑桃 ");

	if( (cbCardData&LOGIC_MASK_VALUE) == 0x01 )
		strInfo += TEXT("A] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x02 )
		strInfo += TEXT("2] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x03 )
		strInfo += TEXT("3] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x04 )
		strInfo += TEXT("4] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x05 )
		strInfo += TEXT("5] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x06 )
		strInfo += TEXT("6] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x07 )
		strInfo += TEXT("7] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x08 )
		strInfo += TEXT("8] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x09 )
		strInfo += TEXT("9] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0A )
		strInfo += TEXT("10] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0B )
		strInfo += TEXT("J] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0C )
		strInfo += TEXT("Q] ");
	else if( (cbCardData&LOGIC_MASK_VALUE) == 0x0D )
		strInfo += TEXT("K] ");

	return strInfo;
}

//返回控制区域
bool __cdecl CServerControlItemSink::ControlResult(BYTE cbControlCardData[GAME_PLAYER][MAX_COUNT], ROOMUSERCONTROL Keyroomusercontrol)
{
	ASSERT(Keyroomusercontrol.roomUserInfo.wChairID < GAME_PLAYER);

	//变量定义
	BYTE bCardData[MAX_COUNT];
	BYTE bHandCardData[GAME_PLAYER][MAX_COUNT];
	ZeroMemory(bCardData, sizeof(bCardData));
	ZeroMemory(bHandCardData, sizeof(bHandCardData));

	//最大玩家
	BYTE bMaxUser = INVALID_BYTE;
	//最小玩家
	BYTE bMinUser = INVALID_BYTE;

	//控制胜利
	if (Keyroomusercontrol.userControl.control_type == CONTINUE_WIN)
	{
		for(BYTE i=0;i<GAME_PLAYER;i++)
		{
			CopyMemory(bHandCardData[i], cbControlCardData[i], sizeof(BYTE)*MAX_COUNT);

			if(INVALID_BYTE == bMaxUser)
			{
				if(cbControlCardData[i][0] != 0)
				{
					bMaxUser = i;
					CopyMemory(bCardData,cbControlCardData[i],sizeof(bCardData));
				}
			}
		}

		//最大牌型
		for(BYTE i=bMaxUser+1;i<GAME_PLAYER;i++)
		{
			if(cbControlCardData[i][0] == 0)continue;

			if(m_GameLogic.CompareCard(bCardData,cbControlCardData[i],MAX_COUNT) == false)
			{
				CopyMemory(bCardData,cbControlCardData[i],sizeof(bCardData));
				bMaxUser=i;
			}
		}

		CopyMemory(cbControlCardData[Keyroomusercontrol.roomUserInfo.wChairID], bCardData, sizeof(bCardData));

		if(Keyroomusercontrol.roomUserInfo.wChairID != bMaxUser)
		{
			CopyMemory(cbControlCardData[bMaxUser], bHandCardData[Keyroomusercontrol.roomUserInfo.wChairID], sizeof(bHandCardData[i]));
		}

		return true;
	}
	else if (Keyroomusercontrol.userControl.control_type == CONTINUE_LOST)
	{	
		for(BYTE i=0;i<GAME_PLAYER;i++)
		{
			CopyMemory(bHandCardData[i], cbControlCardData[i], sizeof(BYTE)*MAX_COUNT);

			if(INVALID_BYTE == bMinUser)
			{
				if(cbControlCardData[i][0] != 0)
				{
					bMinUser = i;
					CopyMemory(bCardData,cbControlCardData[i],sizeof(bCardData));
				}
			}
		}

		//最小牌型
		for(BYTE i=bMinUser+1;i<GAME_PLAYER;i++)
		{
			if(cbControlCardData[i][0] == 0)continue;

			if(m_GameLogic.CompareCard(cbControlCardData[i],bCardData,MAX_COUNT) == false)
			{
				CopyMemory(bCardData,cbControlCardData[i],sizeof(bCardData));
				bMinUser=i;
			}
		}

		CopyMemory(cbControlCardData[Keyroomusercontrol.roomUserInfo.wChairID], bCardData, sizeof(bCardData));

		if(Keyroomusercontrol.roomUserInfo.wChairID != bMinUser)
		{
			CopyMemory(cbControlCardData[bMinUser], bHandCardData[Keyroomusercontrol.roomUserInfo.wChairID], sizeof(bHandCardData[i]));
		}

		return true;
	}

	ASSERT(FALSE);

	return false;
}