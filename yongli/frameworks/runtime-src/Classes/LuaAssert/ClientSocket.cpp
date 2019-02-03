#include "ClientSocket.h"



CClientSocket::CClientSocket(int nHandler)
{
	pISocketServer = GetMCKernel()->CreateSocket(nHandler);
}
CClientSocket::~CClientSocket()
{
	CCLOG("[_DEBUG]	CClientSocket::~CClientSocket");
}

int CClientSocket::sendData(CCmd_Data* pData)
{
	return SendSocketData(pData->GetMainCmd(),pData->GetSubCmd(),pData->GetBufferData(),pData->GetBufferLenght())?1:0;
}

bool CClientSocket::Connect(const char* szUrl, unsigned short wPort,unsigned char* pValidate /*= nullptr*/)
{
	if(pISocketServer)
		return pISocketServer->Connect(szUrl,wPort,pValidate);
	return false;
}
bool CClientSocket::SendSocketData(unsigned short wMain, unsigned short wSub, const void* pData/* = nullptr*/, unsigned short wDataSize/* = 0*/)
{
	if(pISocketServer)
		return pISocketServer->SendSocketData(wMain,wSub,pData,wDataSize);
	return false;
}
void CClientSocket::StopServer()
{
	if(pISocketServer)
		pISocketServer->StopServer();
}
bool CClientSocket::IsServer()
{
	if(pISocketServer)
		return pISocketServer->IsServer();
	return false;
}

void CClientSocket::SetHeartBeatKeep(bool bKeep)
{
	if(pISocketServer)
		pISocketServer->SetHeartBeatKeep(bKeep);
}
void CClientSocket::SetDelayTime(long time)
{
	if(pISocketServer)
		pISocketServer->SetDelayTime(time);
}

void CClientSocket::SetWaitTime(long time)
{
	if(pISocketServer)
		pISocketServer->SetWaitTime(time);
}