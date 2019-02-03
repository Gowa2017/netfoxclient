#pragma once
#include "../ÓÎÏ··þÎñÆ÷/ServerControl.h"
#include "GameLogic.h"

class CServerControlItemSink : public IServerControl
{
public:
  CGameLogic            m_GameLogic;

  CServerControlItemSink(void);
  virtual ~CServerControlItemSink(void);

public:

  virtual bool __cdecl ControlResult(BYTE cbControlCardData[GAME_PLAYER][MAX_COUNT], USERROSTER userroster[GAME_PLAYER]);
};
