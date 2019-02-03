#pragma once

#include <windows.h>
#include <string>
#include <functional>

class PipeServer
{
	HANDLE						m_hClientPipe;
	HANDLE						m_hServerPipe;
	HANDLE						m_lclientpipe;
	HANDLE						m_lserverpipe;
public:
	std::tr1::function<bool(WORD, WORD, VOID *, WORD)>							m_func;
	std::tr1::function<bool(VOID *, WORD)> m_func1;
public:
	PipeServer(void);
	~PipeServer(void);
public:
	bool PipeSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData = 0, WORD wDataSize = 0);
	bool PipeSendData1(WORD wMainCmdID, WORD wSubCmdID, VOID * pData = 0, WORD wDataSize = 0);
	bool PipSendTiren(VOID * pData = 0, WORD wDataSize = 0);
	bool	StartPipe();
	bool	StartPipe1();
	bool	StopPipe();
	bool	StopPipe1();
};
