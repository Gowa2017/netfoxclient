#pragma once

//��Ϸ���ƻ���
class IServerControl
{
public:
  IServerControl(void) {};
  virtual ~IServerControl(void) {};

public:

  virtual bool __cdecl ControlResult(BYTE cbControlCardData[GAME_PLAYER][MAX_COUNT], USERROSTER userroster[GAME_PLAYER]) = NULL;
};
