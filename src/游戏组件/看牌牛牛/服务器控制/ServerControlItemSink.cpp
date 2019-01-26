#include "StdAfx.h"
#include "servercontrolitemsink.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//
CServerControlItemSink::CServerControlItemSink(void)
{
}

CServerControlItemSink::~CServerControlItemSink(void)
{

}

bool __cdecl CServerControlItemSink::ControlResult(BYTE cbControlCardData[GAME_PLAYER][MAX_COUNT], USERROSTER userroster[GAME_PLAYER])
{
  BYTE bCardData[MAX_COUNT];
  BYTE bHandCardData[GAME_PLAYER][MAX_COUNT];
  CopyMemory(bCardData,cbControlCardData[0],sizeof(bCardData));

  for(BYTE i=0; i<GAME_PLAYER; i++)
  {
    CopyMemory(bHandCardData[i],cbControlCardData[i],sizeof(BYTE)*MAX_COUNT);
  }

  //最大牌型
  BYTE bMaxUser=0;
  for(BYTE i=1; i<GAME_PLAYER; i++)
  {
    if(cbControlCardData[i][0] == 0)
    {
      continue;
    }

    bool bFistOx=m_GameLogic.GetOxCard(bCardData,MAX_COUNT);
    bool bNextOx=m_GameLogic.GetOxCard(cbControlCardData[i],MAX_COUNT);


    if(m_GameLogic.CompareCard(bCardData,cbControlCardData[i],MAX_COUNT,bFistOx,bNextOx) == false)
    {
      CopyMemory(bCardData,cbControlCardData[i],sizeof(bCardData));
      bMaxUser=i;
    }
  }

  //最小牌型
  BYTE bMinUser = 0;
  CopyMemory(bCardData,cbControlCardData[0],sizeof(bCardData));
  for(BYTE i=1; i<GAME_PLAYER; i++)
  {
    if(cbControlCardData[i][0] == 0)
    {
      continue;
    }

    bool bFistOx=m_GameLogic.GetOxCard(bCardData,MAX_COUNT);
    bool bNextOx=m_GameLogic.GetOxCard(cbControlCardData[i],MAX_COUNT);


    if(m_GameLogic.CompareCard(bCardData,cbControlCardData[i],MAX_COUNT,bFistOx,bNextOx) == true)
    {
      CopyMemory(bCardData,cbControlCardData[i],sizeof(bCardData));
      bMinUser=i;
    }
  }

  bool bExistWhite = false;
  bool bExistBlack = false;
  bool bExistInvalid = false;

  WORD wWhiteChairID = INVALID_CHAIR;
  WORD wBlackChairID = INVALID_CHAIR;
  WORD wInvalidChairID = INVALID_CHAIR;

  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    if(userroster[i] == WHITE_ROSTER)
    {
      bExistWhite = true;
      wWhiteChairID = i;
    }
    else if(userroster[i] == BLACK_ROSTER)
    {
      bExistBlack = true;
      wBlackChairID = i;
    }
    else if(userroster[i] == INVALID_ROSTER)
    {
      bExistInvalid = true;
      wInvalidChairID = i;
    }
  }

  BYTE cbSwapCard[MAX_COUNT];
  ZeroMemory(cbSwapCard, sizeof(cbSwapCard));

  //存在白名单
  if(bExistWhite == true)
  {
    //最大的牌不为白名单
    if(bMaxUser != wWhiteChairID)
    {
      CopyMemory(cbSwapCard, cbControlCardData[wWhiteChairID], sizeof(BYTE) * MAX_COUNT);
      CopyMemory(cbControlCardData[wWhiteChairID], cbControlCardData[bMaxUser], sizeof(BYTE) * MAX_COUNT);
      CopyMemory(cbControlCardData[bMaxUser], cbSwapCard, sizeof(BYTE) * MAX_COUNT);
    }

  }
  else  //不存在白名单
  {
    //存在黑名单
    if(bExistBlack == true)
    {
      //最小的牌不为黑名单
      if(bMinUser != wBlackChairID)
      {
        CopyMemory(cbSwapCard, cbControlCardData[wBlackChairID], sizeof(BYTE) * MAX_COUNT);
        CopyMemory(cbControlCardData[wBlackChairID], cbControlCardData[bMinUser], sizeof(BYTE) * MAX_COUNT);
        CopyMemory(cbControlCardData[bMinUser], cbSwapCard, sizeof(BYTE) * MAX_COUNT);
      }
    }

  }

  return true;
}