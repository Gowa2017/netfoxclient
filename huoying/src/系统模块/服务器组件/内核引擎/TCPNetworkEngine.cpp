inline void EnableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "StdAfx.h"
#include "Afxinet.h"
#include "TCPNetworkEngine.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//系数定义
#define DEAD_QUOTIETY				0									//死亡系数
#define DANGER_QUOTIETY				1									//危险系数
#define SAFETY_QUOTIETY				2									//安全系数

//动作定义
#define ASYNCHRONISM_SEND_DATA		1									//发送标识
#define ASYNCHRONISM_SEND_BATCH		2									//群体发送
#define ASYNCHRONISM_SHUT_DOWN		3									//安全关闭
#define ASYNCHRONISM_ALLOW_BATCH	4									//允许群发
#define ASYNCHRONISM_CLOSE_SOCKET	5									//关闭连接
#define ASYNCHRONISM_DETECT_SOCKET	6									//检测连接

//索引辅助
#define SOCKET_INDEX(dwSocketID)	LOWORD(dwSocketID)					//位置索引
#define SOCKET_ROUNTID(dwSocketID)	HIWORD(dwSocketID)					//循环索引

//////////////////////////////////////////////////////////////////////////
//结构定义

//发送请求
struct tagSendDataRequest
{
	WORD							wIndex;								//连接索引
	WORD							wRountID;							//循环索引
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
	WORD							wDataSize;							//数据大小
	BYTE							cbSendBuffer[SOCKET_TCP_PACKET];		//发送缓冲
};

//群发请求
struct tagBatchSendRequest
{
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
	WORD							wDataSize;							//数据大小
	BYTE                            cbBatchMask;                        //数据掩码
	BYTE							cbSendBuffer[SOCKET_TCP_PACKET];		//发送缓冲
};

//允许群发
struct tagAllowBatchSend
{
	WORD							wIndex;								//连接索引
	WORD							wRountID;							//循环索引
	BYTE                            cbBatchMask;                        //数据掩码
	BYTE							cbAllowBatch;						//允许标志
};

//关闭连接
struct tagCloseSocket
{
	WORD							wIndex;								//连接索引
	WORD							wRountID;							//循环索引
};

//安全关闭
struct tagShutDownSocket
{
	WORD							wIndex;								//连接索引
	WORD							wRountID;							//循环索引
};

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLapped::COverLapped(enOperationType OperationType) : m_OperationType(OperationType)
{
	//设置变量
	ZeroMemory(&m_WSABuffer,sizeof(m_WSABuffer));
	ZeroMemory(&m_OverLapped,sizeof(m_OverLapped));

	return;
}

//析构函数
COverLapped::~COverLapped()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLappedSend::COverLappedSend() : COverLapped(enOperationType_Send)
{
	m_WSABuffer.len=0;
	m_WSABuffer.buf=(char *)m_cbBuffer;
}

//析构函数
COverLappedSend::~COverLappedSend()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLappedRecv::COverLappedRecv() : COverLapped(enOperationType_Recv)
{
	m_WSABuffer.len=0;
	m_WSABuffer.buf=NULL;
}

//析构函数
COverLappedRecv::~COverLappedRecv()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPNetworkItem::CTCPNetworkItem(WORD wIndex, ITCPNetworkItemSink * pITCPNetworkItemSink) 
	: m_wIndex(wIndex), m_pITCPNetworkItemSink(pITCPNetworkItemSink)
{
	//连接属性
	m_dwClientIP=0L;
	m_dwActiveTime=0L;

	//核心变量
	m_wRountID=1;
	m_wSurvivalTime=0;
	m_wSurvivalTime=DEAD_QUOTIETY;
	m_hSocketHandle=INVALID_SOCKET;

	//状态变量
	m_bSendIng=false;
	m_bRecvIng=false;
	m_bShutDown=false;
	m_bAllowBatch=false;
	m_bBatchMask=0xFF;

	//接收变量
	m_wRecvSize=0;
	ZeroMemory(m_cbRecvBuf,sizeof(m_cbRecvBuf));

	//计数变量
	m_dwSendTickCount=0L;
	m_dwRecvTickCount=0L;
	m_dwSendPacketCount=0L;
	m_dwRecvPacketCount=0L;

	//加密数据
	m_cbSendRound=0;
	m_cbRecvRound=0;
	m_dwSendXorKey=0;
	m_dwRecvXorKey=0;

	return;
}

//析够函数
CTCPNetworkItem::~CTCPNetworkItem()
{
	//删除空闲
	for (INT_PTR i=0;i<m_OverLappedSendBuffer.GetCount();i++)
	{
		delete m_OverLappedSendBuffer[i];
	}

	//删除活动
	for (INT_PTR i=0;i<m_OverLappedSendActive.GetCount();i++)
	{
		delete m_OverLappedSendActive[i];
	}

	//删除数组
	m_OverLappedSendBuffer.RemoveAll();
	m_OverLappedSendActive.RemoveAll();

	return;
}

//绑定对象
DWORD CTCPNetworkItem::Attach(SOCKET hSocket, DWORD dwClientIP)
{
	//效验参数
	ASSERT(dwClientIP!=0);
	ASSERT(hSocket!=INVALID_SOCKET);

	//效验状态
	ASSERT(m_bRecvIng==false);
	ASSERT(m_bSendIng==false);
	ASSERT(m_hSocketHandle==INVALID_SOCKET);

	//状态变量
	m_bSendIng=false;
	m_bRecvIng=false;
	m_bShutDown=false;
	m_bAllowBatch=false;
	m_bBatchMask=0xFF;

	//设置变量
	m_dwClientIP=dwClientIP;
	m_hSocketHandle=hSocket;
	m_wSurvivalTime=SAFETY_QUOTIETY;
	m_dwActiveTime=(DWORD)time(NULL);
	m_dwRecvTickCount=GetTickCount();

	//发送通知
	m_pITCPNetworkItemSink->OnEventSocketBind(this);

	return GetIdentifierID();
}

//恢复数据
DWORD CTCPNetworkItem::ResumeData()
{
	//效验状态
	ASSERT(m_hSocketHandle==INVALID_SOCKET);

	//连接属性
	m_dwClientIP=0L;
	m_dwActiveTime=0L;

	//核心变量
	m_wSurvivalTime=0;
	m_hSocketHandle=INVALID_SOCKET;
	m_wRountID=__max(1,m_wRountID+1);

	//状态变量
	m_bSendIng=false;
	m_bRecvIng=false;
	m_bShutDown=false;
	m_bAllowBatch=false;

	//接收变量
	m_wRecvSize=0;
	ZeroMemory(m_cbRecvBuf,sizeof(m_cbRecvBuf));

	//计数变量
	m_dwSendTickCount=0L;
	m_dwRecvTickCount=0L;
	m_dwSendPacketCount=0L;
	m_dwRecvPacketCount=0L;

	//加密数据
	m_cbSendRound=0;
	m_cbRecvRound=0;
	m_dwSendXorKey=0;
	m_dwRecvXorKey=0;

	//重叠数组
	m_OverLappedSendBuffer.Append(m_OverLappedSendActive);
	m_OverLappedSendActive.RemoveAll();

	return GetIdentifierID();
}

//发送函数
bool CTCPNetworkItem::SendData(WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{
	//发送判断
	if (SendVerdict(wRountID)==false) return false;

	//获取缓冲
	WORD wPacketSize=sizeof(TCP_Head);
	COverLappedSend * pOverLappedSend=GetSendOverLapped(wPacketSize);

	//关闭判断
	if (pOverLappedSend==NULL)
	{
		CloseSocket(wRountID);
		return false;
	}

	//变量定义
	WORD wSourceLen=(WORD)pOverLappedSend->m_WSABuffer.len;
	TCP_Head * pHead=(TCP_Head *)(pOverLappedSend->m_cbBuffer+wSourceLen);

	//设置数据
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;

	//加密数据
	WORD wEncryptLen=EncryptBuffer(pOverLappedSend->m_cbBuffer+wSourceLen,wPacketSize,sizeof(pOverLappedSend->m_cbBuffer)-wSourceLen);
	pOverLappedSend->m_WSABuffer.len+=wEncryptLen;

	//发送数据
	if (m_bSendIng==false)
	{
		//发送数据
		DWORD dwThancferred=0;
		INT nResultCode=WSASend(m_hSocketHandle,&pOverLappedSend->m_WSABuffer,1,&dwThancferred,0,&pOverLappedSend->m_OverLapped,NULL);

		//结果处理
		if ((nResultCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSA_IO_PENDING))
		{
			CloseSocket(m_wRountID);
			return false;
		}

		//设置变量
		m_bSendIng=true;
	}

	return true;
}

//发送函数
bool CTCPNetworkItem::SendData(VOID * pData, WORD wDataSize, WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{
	//效验参数
	ASSERT(wDataSize<=SOCKET_TCP_PACKET);

	//发送判断
	if (wDataSize>SOCKET_TCP_PACKET) return false;
	if (SendVerdict(wRountID)==false) return false;

	//获取缓冲
	WORD wPacketSize=sizeof(TCP_Head)+wDataSize;
	COverLappedSend * pOverLappedSend=GetSendOverLapped(wPacketSize);

	//关闭判断
	if (pOverLappedSend==NULL)
	{
		CloseSocket(wRountID);
		return false;
	}

	//变量定义
	WORD wSourceLen=(WORD)pOverLappedSend->m_WSABuffer.len;
	TCP_Head * pHead=(TCP_Head *)(pOverLappedSend->m_cbBuffer+wSourceLen);

	//设置变量
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;

	//附加数据
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(pHead+1,pData,wDataSize);
	}

	//加密数据
	WORD wEncryptLen=EncryptBuffer(pOverLappedSend->m_cbBuffer+wSourceLen,wPacketSize,sizeof(pOverLappedSend->m_cbBuffer)-wSourceLen);
	pOverLappedSend->m_WSABuffer.len+=wEncryptLen;

	//发送数据
	if (m_bSendIng==false)
	{
		//发送数据
		DWORD dwThancferred=0;
		INT nResultCode=WSASend(m_hSocketHandle,&pOverLappedSend->m_WSABuffer,1,&dwThancferred,0,&pOverLappedSend->m_OverLapped,NULL);

		//结果处理
		if ((nResultCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSA_IO_PENDING))
		{
			CloseSocket(m_wRountID);
			return false;
		}

		//设置变量
		m_bSendIng=true;
	}

	return true;
}

//投递接收
bool CTCPNetworkItem::RecvData()
{
	//效验变量
	ASSERT(m_bRecvIng==false);
	ASSERT(m_hSocketHandle!=INVALID_SOCKET);

	//接收数据
	DWORD dwThancferred=0,dwFlags=0;
	INT nResultCode=WSARecv(m_hSocketHandle,&m_OverLappedRecv.m_WSABuffer,1,&dwThancferred,&dwFlags,&m_OverLappedRecv.m_OverLapped,NULL);

	//结果处理
	if ((nResultCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSA_IO_PENDING))
	{
		CloseSocket(m_wRountID);
		return false;
	}

	//设置变量
	m_bRecvIng=true;

	return true;
}

//关闭连接
bool CTCPNetworkItem::CloseSocket(WORD wRountID)
{
	//状态判断
	if (m_wRountID!=wRountID) return false;

	//关闭连接
	if (m_hSocketHandle!=INVALID_SOCKET)
	{
		closesocket(m_hSocketHandle);
		m_hSocketHandle=INVALID_SOCKET;
	}

	//判断关闭
	if ((m_bRecvIng==false)&&(m_bSendIng==false))
	{
		OnCloseCompleted();
	}

	return true;
}

//设置关闭
bool CTCPNetworkItem::ShutDownSocket(WORD wRountID)
{
	//状态判断
	if (m_hSocketHandle==INVALID_SOCKET) return false;
	if ((m_wRountID!=wRountID)||(m_bShutDown==true)) return false;

	//设置变量
	m_wRecvSize=0;
	m_bShutDown=true;

	//发送命令
	//SendData(MDM_KN_COMMAND,SUB_KN_SHUT_DOWN_SOCKET,m_wRountID);

	return true;
}

//允许群发
bool CTCPNetworkItem::AllowBatchSend(WORD wRountID, bool bAllowBatch,BYTE cbBatchMask)
{
	//状态判断
	if (m_wRountID!=wRountID) return false;
	if (m_hSocketHandle==INVALID_SOCKET) return false;

	//设置变量
	m_bAllowBatch=bAllowBatch;

	m_bBatchMask=cbBatchMask;

	return true;
}

//发送完成
bool CTCPNetworkItem::OnSendCompleted(COverLappedSend * pOverLappedSend, DWORD dwThancferred)
{
	//效验变量
	ASSERT(m_bSendIng==true);
	ASSERT(m_OverLappedSendActive.GetCount()>0);
	ASSERT(pOverLappedSend==m_OverLappedSendActive[0]);

	//释放结构
	m_OverLappedSendActive.RemoveAt(0);
	m_OverLappedSendBuffer.Add(pOverLappedSend);

	//设置变量
	m_bSendIng=false;

	//判断关闭
	if (m_hSocketHandle==INVALID_SOCKET)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//设置变量
	if (dwThancferred!=0)
	{
		m_wSurvivalTime=SAFETY_QUOTIETY;
		m_dwSendTickCount=GetTickCount();
	}

	//继续发送
	if (m_OverLappedSendActive.GetCount()>0)
	{
		//获取数据
		pOverLappedSend=m_OverLappedSendActive[0];

		//发送数据
		DWORD dwThancferred=0;
		INT nResultCode=WSASend(m_hSocketHandle,&pOverLappedSend->m_WSABuffer,1,&dwThancferred,0,&pOverLappedSend->m_OverLapped,NULL);

		//结果处理
		if ((nResultCode==SOCKET_ERROR)&&(WSAGetLastError()!=WSA_IO_PENDING))
		{
			CloseSocket(m_wRountID);
			return false;
		}

		//设置变量
		m_bSendIng=true;
	}

	return true;
}

//接收完成
bool CTCPNetworkItem::OnRecvCompleted(COverLappedRecv * pOverLappedRecv, DWORD dwThancferred)
{
	//效验数据
	ASSERT(m_bRecvIng==true);

	//设置变量
	m_bRecvIng=false;

	//判断关闭
	if (m_hSocketHandle==INVALID_SOCKET)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//接收数据
	INT nResultCode=recv(m_hSocketHandle,(char *)m_cbRecvBuf+m_wRecvSize,sizeof(m_cbRecvBuf)-m_wRecvSize,0);

	//关闭判断
	if (nResultCode<=0)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	//中断判断
	if (m_bShutDown==true) return true;

	//设置变量
	m_wRecvSize+=nResultCode;
	m_wSurvivalTime=SAFETY_QUOTIETY;
	m_dwRecvTickCount=GetTickCount();

	//接收完成
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	TCP_Head * pHead=(TCP_Head *)m_cbRecvBuf;

	//处理数据
	try
	{
		while (m_wRecvSize>=sizeof(TCP_Head))
		{
			//效验数据
			WORD wPacketSize=pHead->TCPInfo.wPacketSize;

			//数据判断
			if (wPacketSize>SOCKET_TCP_BUFFER) throw TEXT("数据包长度太长");
			if (wPacketSize<sizeof(TCP_Head))
			{
				TCHAR szBuffer[512];
				_sntprintf(szBuffer,CountArray(szBuffer),TEXT("数据包长度太短 %d,%d,%d,%d,%d,%d,%d,%d"),m_cbRecvBuf[0],
					m_cbRecvBuf[1],
					m_cbRecvBuf[2],
					m_cbRecvBuf[3],
					m_cbRecvBuf[4],
					m_cbRecvBuf[5],
					m_cbRecvBuf[6],
					m_cbRecvBuf[7]
					);
				g_TraceServiceManager.TraceString(szBuffer,TraceLevel_Exception);

				_sntprintf(szBuffer,CountArray(szBuffer),TEXT("包长 %d, 版本 %d, 效验码 %d"),pHead->TCPInfo.wPacketSize,
					pHead->TCPInfo.cbDataKind,pHead->TCPInfo.cbCheckCode);

				g_TraceServiceManager.TraceString(szBuffer,TraceLevel_Exception);
				throw TEXT("数据包长度太短");
			}
			if (pHead->TCPInfo.cbDataKind != DK_MAPPED && pHead->TCPInfo.cbDataKind != 0x05) {
				CString aa;
				aa.Format(TEXT("0x%x"), pHead->TCPInfo.cbDataKind);
				g_TraceServiceManager.TraceString(aa, TraceLevel_Exception);
				throw TEXT("数据包版本不匹配");
			}
			//完成判断
			if (m_wRecvSize<wPacketSize) break;

			//提取数据
			CopyMemory(cbBuffer,m_cbRecvBuf,wPacketSize);
			WORD wRealySize=CrevasseBuffer(cbBuffer,wPacketSize);

			//设置变量
			m_dwRecvPacketCount++;

			//解释数据
			LPVOID pData=cbBuffer+sizeof(TCP_Head);
			WORD wDataSize=wRealySize-sizeof(TCP_Head);
			TCP_Command Command=((TCP_Head *)cbBuffer)->CommandInfo;

			//消息处理
			if (Command.wMainCmdID!=MDM_KN_COMMAND)	m_pITCPNetworkItemSink->OnEventSocketRead(Command,pData,wDataSize,this);

			//删除缓冲
			m_wRecvSize-=wPacketSize;
			if (m_wRecvSize>0) MoveMemory(m_cbRecvBuf,m_cbRecvBuf+wPacketSize,m_wRecvSize);
		}
	}
	catch (LPCTSTR pszMessage)
	{
		//错误信息
		TCHAR szString[512]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("SocketEngine Index=%ld，RountID=%ld，OnRecvCompleted 发生“%s”异常"),m_wIndex,m_wRountID,pszMessage);
		g_TraceServiceManager.TraceString(szString,TraceLevel_Exception);

		//关闭链接
		CloseSocket(m_wRountID);

		return false;
	}
	catch (...)
	{ 
		//错误信息
		TCHAR szString[512]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("SocketEngine Index=%ld，RountID=%ld，OnRecvCompleted 发生“非法”异常"),m_wIndex,m_wRountID);
		g_TraceServiceManager.TraceString(szString,TraceLevel_Exception);

		//关闭链接
		CloseSocket(m_wRountID);

		return false;
	}

	return RecvData();
}

//关闭完成
bool CTCPNetworkItem::OnCloseCompleted()
{
	//效验状态
	ASSERT(m_hSocketHandle==INVALID_SOCKET);
	ASSERT((m_bSendIng==false)&&(m_bRecvIng==false));

	//关闭事件
	m_pITCPNetworkItemSink->OnEventSocketShut(this);

	//恢复数据
	ResumeData();

	return true;
}

//加密数据
WORD CTCPNetworkItem::EncryptBuffer(BYTE pcbDataBuffer[], WORD wDataSize, WORD wBufferSize)
{
	WORD i = 0;
	//效验参数
	ASSERT(wDataSize >= sizeof(TCP_Head));
	ASSERT(wDataSize <= (sizeof(TCP_Head) + SOCKET_TCP_BUFFER));
	ASSERT(wBufferSize >= (wDataSize + 2*sizeof(DWORD)));

	//填写信息头
	TCP_Head * pHead = (TCP_Head *)pcbDataBuffer;
	pHead->TCPInfo.wPacketSize = wDataSize;
	pHead->TCPInfo.cbDataKind = DK_MAPPED;


	BYTE checkCode = 0;

	for (WORD i = sizeof(TCP_Info); i < wDataSize; i++)
	{
		checkCode += pcbDataBuffer[i];
		pcbDataBuffer[i] = MapSendByte(pcbDataBuffer[i]);
	}
	pHead->TCPInfo.cbCheckCode = ~checkCode+1;

	//设置变量
	m_dwSendPacketCount++;

	return wDataSize;
}

//解密数据
WORD CTCPNetworkItem::CrevasseBuffer(BYTE pcbDataBuffer[], WORD wDataSize)
{
	WORD i = 0;
	//效验参数
	ASSERT(wDataSize >= sizeof(TCP_Head));
	ASSERT(((TCP_Head *)pcbDataBuffer)->TCPInfo.wPacketSize == wDataSize);

	//效验码与字节映射
	TCP_Head * pHead = (TCP_Head *)pcbDataBuffer;
	for (i = sizeof(TCP_Info); i < wDataSize; i++)
	{
		pcbDataBuffer[i] = MapRecvByte(pcbDataBuffer[i]);
	}

	return wDataSize;
}

//随机映射
WORD CTCPNetworkItem::SeedRandMap(WORD wSeed)
{
  DWORD dwHold = wSeed;
  return (WORD)((dwHold = dwHold * 241103L + 2533101L) >> 16);
}

//映射发送数据
BYTE CTCPNetworkItem::MapSendByte(BYTE const cbData)
{
	BYTE cbMap;
	cbMap = g_SendByteMap[cbData];
	return cbMap;
}

//映射接收数据
BYTE CTCPNetworkItem::MapRecvByte(BYTE const cbData)
{
	BYTE cbMap;
	cbMap = g_RecvByteMap[cbData];
	return cbMap;
}

//发送判断
bool CTCPNetworkItem::SendVerdict(WORD wRountID)
{
	if ((m_wRountID!=wRountID)||(m_bShutDown==true)) return false;
	if ((m_hSocketHandle==INVALID_SOCKET)||(m_dwRecvPacketCount==0)) return false;

	return true;
}

//获取发送结构
COverLappedSend * CTCPNetworkItem::GetSendOverLapped(WORD wPacketSize)
{
	//重用判断
	if (m_OverLappedSendActive.GetCount()>1)
	{
		INT_PTR nActiveCount=m_OverLappedSendActive.GetCount();
		COverLappedSend * pOverLappedSend=m_OverLappedSendActive[nActiveCount-1];
		if (sizeof(pOverLappedSend->m_cbBuffer)>=(pOverLappedSend->m_WSABuffer.len+wPacketSize+sizeof(DWORD)*2)) return pOverLappedSend;
	}

	//空闲对象
	if (m_OverLappedSendBuffer.GetCount()>0)
	{
		//获取对象
		INT_PTR nFreeCount=m_OverLappedSendBuffer.GetCount();
		COverLappedSend * pOverLappedSend=m_OverLappedSendBuffer[nFreeCount-1];

		//设置变量
		pOverLappedSend->m_WSABuffer.len=0;
		m_OverLappedSendActive.Add(pOverLappedSend);
		m_OverLappedSendBuffer.RemoveAt(nFreeCount-1);

		return pOverLappedSend;
	}

	try
	{
		//创建对象
		COverLappedSend * pOverLappedSend=new COverLappedSend;
		ASSERT(pOverLappedSend!=NULL);

		//设置变量
		pOverLappedSend->m_WSABuffer.len=0;
		m_OverLappedSendActive.Add(pOverLappedSend);

		return pOverLappedSend;
	}
	catch (...) { ASSERT(FALSE); }

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPNetworkThreadReadWrite::CTCPNetworkThreadReadWrite()
{
	m_hCompletionPort=NULL;
}

//析构函数
CTCPNetworkThreadReadWrite::~CTCPNetworkThreadReadWrite()
{
}

//配置函数
bool CTCPNetworkThreadReadWrite::InitThread(HANDLE hCompletionPort)
{
	ASSERT(hCompletionPort!=NULL);
	m_hCompletionPort=hCompletionPort;
	return true;
}

//运行函数
bool CTCPNetworkThreadReadWrite::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort!=NULL);

	//变量定义
	DWORD dwThancferred=0;					
	OVERLAPPED * pOverLapped=NULL;
	CTCPNetworkItem * pTCPNetworkItem=NULL;

	//完成端口
	BOOL bSuccess=GetQueuedCompletionStatus(m_hCompletionPort,&dwThancferred,(PULONG_PTR)&pTCPNetworkItem,&pOverLapped,INFINITE);
	if ((bSuccess==FALSE)&&(GetLastError()!=ERROR_NETNAME_DELETED)) return false;

	//退出判断
	if ((pTCPNetworkItem==NULL)&&(pOverLapped==NULL)) return false;

	//变量定义
	COverLapped * pSocketLapped=CONTAINING_RECORD(pOverLapped,COverLapped,m_OverLapped);

	//操作处理
	switch (pSocketLapped->GetOperationType())
	{
	case enOperationType_Send:	//数据发送
		{
			CWHDataLocker ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->OnSendCompleted((COverLappedSend *)pSocketLapped,dwThancferred);
			break;
		}
	case enOperationType_Recv:	//数据读取
		{
			CWHDataLocker ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->OnRecvCompleted((COverLappedRecv *)pSocketLapped,dwThancferred);
			break;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPNetworkThreadAccept::CTCPNetworkThreadAccept()
{
	m_hCompletionPort=NULL;
	m_pTCPNetworkEngine=NULL;
	m_hListenSocket=INVALID_SOCKET;
}

//析构函数
CTCPNetworkThreadAccept::~CTCPNetworkThreadAccept()
{
}

//配置函数
bool CTCPNetworkThreadAccept::InitThread(HANDLE hCompletionPort, SOCKET hListenSocket, CTCPNetworkEngine * pNetworkEngine)
{
	//效验参数
	ASSERT(pNetworkEngine!=NULL);
	ASSERT(hCompletionPort!=NULL);
	ASSERT(hListenSocket!=INVALID_SOCKET);

	//设置变量
	m_hListenSocket=hListenSocket;
	m_hCompletionPort=hCompletionPort;
	m_pTCPNetworkEngine=pNetworkEngine;

	return true;
}

//运行函数
bool CTCPNetworkThreadAccept::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort!=NULL);
	ASSERT(m_pTCPNetworkEngine!=NULL);

	//变量定义
	SOCKET hConnectSocket=INVALID_SOCKET;
	CTCPNetworkItem * pTCPNetworkItem=NULL;

	try
	{
		//监听连接
		SOCKADDR_IN	SocketAddr;
		INT nBufferSize=sizeof(SocketAddr);
		hConnectSocket=WSAAccept(m_hListenSocket,(SOCKADDR *)&SocketAddr,&nBufferSize,NULL,NULL);

		//退出判断
		if (hConnectSocket==INVALID_SOCKET) return false;

		//获取连接
		pTCPNetworkItem=m_pTCPNetworkEngine->ActiveNetworkItem();

		//失败判断
		if (pTCPNetworkItem==NULL)
		{
			ASSERT(FALSE);
			throw TEXT("申请连接对象失败");
		}

		//锁定对象
		CWHDataLocker ThreadLock(pTCPNetworkItem->GetCriticalSection());

		//绑定对象
		pTCPNetworkItem->Attach(hConnectSocket,SocketAddr.sin_addr.S_un.S_addr);
		CreateIoCompletionPort((HANDLE)hConnectSocket,m_hCompletionPort,(ULONG_PTR)pTCPNetworkItem,0);

		//发起接收
		pTCPNetworkItem->RecvData();
	}
	catch (...)
	{
		//清理对象
		ASSERT(pTCPNetworkItem==NULL);

		//关闭连接
		if (hConnectSocket!=INVALID_SOCKET)
		{
			closesocket(hConnectSocket);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPNetworkThreadDetect::CTCPNetworkThreadDetect()
{
	m_dwPileTime=0L;
	m_dwDetectTime=10000L;
	m_pTCPNetworkEngine=NULL;
}

//析构函数
CTCPNetworkThreadDetect::~CTCPNetworkThreadDetect()
{
}

//配置函数
bool CTCPNetworkThreadDetect::InitThread(CTCPNetworkEngine * pNetworkEngine, DWORD dwDetectTime)
{
	//效验参数
	ASSERT(pNetworkEngine!=NULL);

	//设置变量
	m_dwPileTime=0L;
	m_dwDetectTime=dwDetectTime;
	m_pTCPNetworkEngine=pNetworkEngine;

	return true;
}

//运行函数
bool CTCPNetworkThreadDetect::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_pTCPNetworkEngine!=NULL);

	//设置间隔
	Sleep(200);
	m_dwPileTime+=200L;

	//检测连接
	if (m_dwPileTime>=m_dwDetectTime)
	{
		m_dwPileTime=0L;
		m_pTCPNetworkEngine->DetectSocket();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTCPNetworkEngine::CTCPNetworkEngine()
{
	//验证变量
	m_bValidate=false;
	m_bNormalRun=true;

	//辅助变量
	m_bService=false;
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	//配置变量
	m_wMaxConnect=0;
	m_wServicePort=0;
	m_dwDetectTime=10000L;

	//内核变量
	m_hCompletionPort=NULL;
	m_hServerSocket=INVALID_SOCKET;
	m_pITCPNetworkEngineEvent=NULL;

	return;
}

//析构函数
CTCPNetworkEngine::~CTCPNetworkEngine()
{
}

//接口查询
VOID * CTCPNetworkEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPNetworkEngine,Guid,dwQueryVer);
	QUERYINTERFACE(IAsynchronismEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPNetworkEngine,Guid,dwQueryVer);
	return NULL;
}

//启动服务
bool CTCPNetworkEngine::StartService()
{
	//状态效验
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//效验参数
	ASSERT((m_wMaxConnect!=0)&&(m_wServicePort!=0));
	if ((m_wMaxConnect==0)||(m_wServicePort==0)) return false;

	//系统信息
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	DWORD dwThreadCount=SystemInfo.dwNumberOfProcessors;

	//完成端口
	m_hCompletionPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,SystemInfo.dwNumberOfProcessors);
	if (m_hCompletionPort==NULL) return false;

	//建立网络
	SOCKADDR_IN SocketAddr;
	ZeroMemory(&SocketAddr,sizeof(SocketAddr));

	//建立网络
	SocketAddr.sin_family=AF_INET;
	SocketAddr.sin_addr.s_addr=INADDR_ANY;
	SocketAddr.sin_port=htons(m_wServicePort);
	m_hServerSocket=WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);

	//错误判断
	if (m_hServerSocket==INVALID_SOCKET) 
	{
		LPCTSTR pszString=TEXT("系统资源不足或者 TCP/IP 协议没有安装，网络启动失败");
		g_TraceServiceManager.TraceString(pszString,TraceLevel_Exception);
		return false;
	}

	//绑定链接
	if (bind(m_hServerSocket,(SOCKADDR*)&SocketAddr,sizeof(SocketAddr))==SOCKET_ERROR)
	{
		LPCTSTR pszString=TEXT("网络绑定发生错误，网络启动失败");
		g_TraceServiceManager.TraceString(pszString,TraceLevel_Exception);
		return false;
	}

	//监听端口
	if (listen(m_hServerSocket,200)==SOCKET_ERROR)
	{
		TCHAR szString[512]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("端口正被其他服务占用，监听 %ld 端口失败"),m_wServicePort);
		g_TraceServiceManager.TraceString(szString,TraceLevel_Exception);
		return false;
	}

	//异步引擎
	IUnknownEx * pIUnknownEx=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_AsynchronismEngine.SetAsynchronismSink(pIUnknownEx)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//网页验证
	WebAttestation();

	//启动服务
	if (m_AsynchronismEngine.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//读写线程
	for (DWORD i=0;i<dwThreadCount;i++)
	{
		CTCPNetworkThreadReadWrite * pNetworkRSThread=new CTCPNetworkThreadReadWrite();
		if (pNetworkRSThread->InitThread(m_hCompletionPort)==false) return false;
		m_SocketRWThreadArray.Add(pNetworkRSThread);
	}

	//应答线程
	if (m_SocketAcceptThread.InitThread(m_hCompletionPort,m_hServerSocket,this)==false) return false;

	//读写线程
	for (DWORD i = 0; i<dwThreadCount; i++)
	{
		CTCPNetworkThreadReadWrite * pNetworkRSThread=m_SocketRWThreadArray[i];
		ASSERT(pNetworkRSThread!=NULL);
		if (pNetworkRSThread->StartThread()==false) return false;
	}

	//检测线程
	m_SocketDetectThread.InitThread(this,m_dwDetectTime);
	if (m_SocketDetectThread.StartThread()==false) return false;

	//应答线程
	if (m_SocketAcceptThread.StartThread()==false) return false;

	//设置变量
	m_bService=true;

	return true;
}

//停止服务
bool CTCPNetworkEngine::ConcludeService()
{
	//设置变量
	m_bService=false;

	//检测线程
	m_SocketDetectThread.ConcludeThread(INFINITE);

	//应答线程
	if (m_hServerSocket!=INVALID_SOCKET)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket=INVALID_SOCKET;
	}
	m_SocketAcceptThread.ConcludeThread(INFINITE);

	//异步引擎
	m_AsynchronismEngine.ConcludeService();

	//读写线程
	INT_PTR nCount=m_SocketRWThreadArray.GetCount();
	if (m_hCompletionPort!=NULL)
	{
		for (INT_PTR i=0;i<nCount;i++) PostQueuedCompletionStatus(m_hCompletionPort,0,NULL,NULL);
	}
	for (INT_PTR i=0;i<nCount;i++)
	{
		CTCPNetworkThreadReadWrite * pSocketThread=m_SocketRWThreadArray[i];
		ASSERT(pSocketThread!=NULL);
		pSocketThread->ConcludeThread(INFINITE);
		SafeDelete(pSocketThread);
	}
	m_SocketRWThreadArray.RemoveAll();

	//完成端口
	if (m_hCompletionPort!=NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort=NULL;
	}

	//关闭连接
	CTCPNetworkItem * pTCPNetworkItem=NULL;
	for (INT_PTR i=0;i<m_NetworkItemActive.GetCount();i++)
	{
		pTCPNetworkItem=m_NetworkItemActive[i];
		pTCPNetworkItem->CloseSocket(pTCPNetworkItem->GetRountID());
		pTCPNetworkItem->ResumeData();
	}

	//重置数据
	m_NetworkItemBuffer.Append(m_NetworkItemActive);
	m_NetworkItemActive.RemoveAll();
	m_TempNetworkItemArray.RemoveAll();

	return true;
}

//设置参数
bool CTCPNetworkEngine::SetServiceParameter(WORD wServicePort, WORD wMaxConnect, LPCTSTR  pszCompilation)
{
	//状态效验
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//设置变量
	ASSERT(wServicePort!=0);
	m_wMaxConnect=wMaxConnect;
	m_wServicePort=wServicePort;

	return true;
}

//配置端口
WORD CTCPNetworkEngine::GetServicePort()
{
	return m_wServicePort;
}

//当前端口
WORD CTCPNetworkEngine::GetCurrentPort()
{
	return m_wServicePort;
}

//设置接口
bool CTCPNetworkEngine::SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx)
{
	//状态效验
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//查询接口
	m_pITCPNetworkEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPNetworkEngineEvent);

	//错误判断
	if (m_pITCPNetworkEngineEvent==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

    return true;
}

//发送函数
bool CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID)
{
	//缓冲锁定
	CWHDataLocker ThreadLock(m_BufferLocked);
	tagSendDataRequest * pSendDataRequest=(tagSendDataRequest *)m_cbBuffer;
	
	//构造数据
	pSendDataRequest->wDataSize=0;
	pSendDataRequest->wSubCmdID=wSubCmdID;
	pSendDataRequest->wMainCmdID=wMainCmdID;
	pSendDataRequest->wIndex=SOCKET_INDEX(dwSocketID);
	pSendDataRequest->wRountID=SOCKET_ROUNTID(dwSocketID);

	//发送请求
	WORD wSendSize=sizeof(tagSendDataRequest)-sizeof(pSendDataRequest->cbSendBuffer);
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SEND_DATA,m_cbBuffer,wSendSize);
}

//发送函数
bool CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//效益数据
	ASSERT((wDataSize+sizeof(TCP_Head))<=SOCKET_TCP_PACKET);
	if ((wDataSize+sizeof(TCP_Head))>SOCKET_TCP_PACKET) return false;

	//缓冲锁定
	CWHDataLocker ThreadLock(m_BufferLocked);
	tagSendDataRequest * pSendDataRequest=(tagSendDataRequest *)m_cbBuffer;

	//构造数据
	pSendDataRequest->wDataSize=wDataSize;
	pSendDataRequest->wSubCmdID=wSubCmdID;
	pSendDataRequest->wMainCmdID=wMainCmdID;
	pSendDataRequest->wIndex=SOCKET_INDEX(dwSocketID);
	pSendDataRequest->wRountID=SOCKET_ROUNTID(dwSocketID);
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(pSendDataRequest->cbSendBuffer,pData,wDataSize);
	}

	//发送请求
	WORD wSendSize=sizeof(tagSendDataRequest)-sizeof(pSendDataRequest->cbSendBuffer)+wDataSize;
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SEND_DATA,m_cbBuffer,wSendSize);
}

//批量发送
bool CTCPNetworkEngine::SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize,BYTE cbBatchMask)
{
	//效验数据
	ASSERT((wDataSize+sizeof(TCP_Head))<=SOCKET_TCP_PACKET);
	if ((wDataSize+sizeof(TCP_Head))>SOCKET_TCP_PACKET) return false;

	//缓冲锁定
	CWHDataLocker ThreadLock(m_BufferLocked);
	tagBatchSendRequest * pBatchSendRequest=(tagBatchSendRequest *)m_cbBuffer;

	//构造数据
	pBatchSendRequest->wMainCmdID=wMainCmdID;
	pBatchSendRequest->wSubCmdID=wSubCmdID;
	pBatchSendRequest->wDataSize=wDataSize;
	pBatchSendRequest->cbBatchMask=cbBatchMask;
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(pBatchSendRequest->cbSendBuffer,pData,wDataSize);
	}

	//发送请求
	WORD wSendSize=sizeof(tagBatchSendRequest)-sizeof(pBatchSendRequest->cbSendBuffer)+wDataSize;
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SEND_BATCH,m_cbBuffer,wSendSize);
}

//关闭连接
bool CTCPNetworkEngine::CloseSocket(DWORD dwSocketID)
{
	//缓冲锁定
	CWHDataLocker ThreadLock(m_BufferLocked);
	tagCloseSocket * pCloseSocket=(tagCloseSocket *)m_cbBuffer;

	//构造数据
	pCloseSocket->wIndex=SOCKET_INDEX(dwSocketID);
	pCloseSocket->wRountID=SOCKET_ROUNTID(dwSocketID);

	//发送请求
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_CLOSE_SOCKET,m_cbBuffer,sizeof(tagCloseSocket));
}

//设置关闭
bool CTCPNetworkEngine::ShutDownSocket(DWORD dwSocketID)
{
	//缓冲锁定
	CWHDataLocker ThreadLock(m_BufferLocked);
	tagShutDownSocket * pShutDownSocket=(tagShutDownSocket *)m_cbBuffer;

	//构造数据
	pShutDownSocket->wIndex=SOCKET_INDEX(dwSocketID);
	pShutDownSocket->wRountID=SOCKET_ROUNTID(dwSocketID);

	//发送请求
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_SHUT_DOWN,m_cbBuffer,sizeof(tagShutDownSocket));
}

//允许群发
bool CTCPNetworkEngine::AllowBatchSend(DWORD dwSocketID, bool cbAllowBatch,BYTE cbBatchMask)
{
	//缓冲锁定
	CWHDataLocker ThreadLock(m_BufferLocked);
	tagAllowBatchSend * pAllowBatchSend=(tagAllowBatchSend *)m_cbBuffer;

	//构造数据
	pAllowBatchSend->cbAllowBatch=cbAllowBatch;
	pAllowBatchSend->cbBatchMask=cbBatchMask;
	pAllowBatchSend->wIndex=SOCKET_INDEX(dwSocketID);
	pAllowBatchSend->wRountID=SOCKET_ROUNTID(dwSocketID);

	//发送请求
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_ALLOW_BATCH,m_cbBuffer,sizeof(tagAllowBatchSend));
}

//异步数据
bool CTCPNetworkEngine::OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case ASYNCHRONISM_SEND_DATA:		//发送请求
		{
			//效验数据
			tagSendDataRequest * pSendDataRequest=(tagSendDataRequest *)pData;
			ASSERT(wDataSize>=(sizeof(tagSendDataRequest)-sizeof(pSendDataRequest->cbSendBuffer)));
			ASSERT(wDataSize==(pSendDataRequest->wDataSize+sizeof(tagSendDataRequest)-sizeof(pSendDataRequest->cbSendBuffer)));

			//获取对象
			CTCPNetworkItem * pTCPNetworkItem=GetNetworkItem(pSendDataRequest->wIndex);
			if (pTCPNetworkItem==NULL) return false;

			//发送数据
			CWHDataLocker SocketThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->SendData(pSendDataRequest->cbSendBuffer,pSendDataRequest->wDataSize,pSendDataRequest->wMainCmdID,
				pSendDataRequest->wSubCmdID,pSendDataRequest->wRountID);

			return true;
		}
	case ASYNCHRONISM_SEND_BATCH:		//群发请求
		{
			//效验数据
			tagBatchSendRequest * pBatchSendRequest=(tagBatchSendRequest *)pData;
			ASSERT(wDataSize>=(sizeof(tagBatchSendRequest)-sizeof(pBatchSendRequest->cbSendBuffer)));
			ASSERT(wDataSize==(pBatchSendRequest->wDataSize+sizeof(tagBatchSendRequest)-sizeof(pBatchSendRequest->cbSendBuffer)));

			//获取活动项
			CWHDataLocker ItemThreadLock(m_ItemLocked);
			m_TempNetworkItemArray.Copy(m_NetworkItemActive);
			ItemThreadLock.UnLock();

			//群发数据
			for (INT_PTR i=0;i<m_TempNetworkItemArray.GetCount();i++)
			{
				//获取对象
				CTCPNetworkItem * pTCPNetworkItem=m_TempNetworkItemArray[i];
				CWHDataLocker SocketThreadLock(pTCPNetworkItem->GetCriticalSection());

				//发生数据
				if (pTCPNetworkItem->IsAllowBatch() && pTCPNetworkItem->GetBatchMask()==pBatchSendRequest->cbBatchMask)
				{
					pTCPNetworkItem->SendData(pBatchSendRequest->cbSendBuffer,pBatchSendRequest->wDataSize,pBatchSendRequest->wMainCmdID,
						pBatchSendRequest->wSubCmdID,pTCPNetworkItem->GetRountID());
				}
			}

			return true;
		}
	case ASYNCHRONISM_SHUT_DOWN:		//安全关闭
		{
			//效验数据
			ASSERT(wDataSize==sizeof(tagShutDownSocket));
			tagShutDownSocket * pShutDownSocket=(tagShutDownSocket *)pData;

			//获取对象
			CTCPNetworkItem * pTCPNetworkItem=GetNetworkItem(pShutDownSocket->wIndex);
			if (pTCPNetworkItem==NULL) return false;

			//安全关闭
			CWHDataLocker ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->ShutDownSocket(pShutDownSocket->wRountID);

			return true;
		}
	case ASYNCHRONISM_ALLOW_BATCH:		//允许群发
		{
			//效验数据
			ASSERT(wDataSize==sizeof(tagAllowBatchSend));
			tagAllowBatchSend * pAllowBatchSend=(tagAllowBatchSend *)pData;

			//获取对象
			CTCPNetworkItem * pTCPNetworkItem=GetNetworkItem(pAllowBatchSend->wIndex);
			if (pTCPNetworkItem==NULL) return false;

			//设置群发
			CWHDataLocker ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->AllowBatchSend(pAllowBatchSend->wRountID,pAllowBatchSend->cbAllowBatch?true:false,pAllowBatchSend->cbBatchMask);

			return true;
		}
	case ASYNCHRONISM_CLOSE_SOCKET:		//关闭连接
		{
			//效验数据
			ASSERT(wDataSize==sizeof(tagCloseSocket));
			tagCloseSocket * pCloseSocket=(tagCloseSocket *)pData;

			//获取对象
			CTCPNetworkItem * pTCPNetworkItem=GetNetworkItem(pCloseSocket->wIndex);
			if (pTCPNetworkItem==NULL) return false;

			//关闭连接
			CWHDataLocker ThreadLock(pTCPNetworkItem->GetCriticalSection());
			pTCPNetworkItem->CloseSocket(pCloseSocket->wRountID);

			return true;
		}
	case ASYNCHRONISM_DETECT_SOCKET:	//检测连接
		{
			//获取活动项
			CWHDataLocker ThreadLock(m_ItemLocked);
			m_TempNetworkItemArray.Copy(m_NetworkItemActive);
			ThreadLock.UnLock();

			//检测连接
			DWORD dwNowTime=(DWORD)time(NULL);
			for (INT_PTR i=0;i<m_TempNetworkItemArray.GetCount();i++)
			{
				//获取连接
				CTCPNetworkItem * pTCPNetworkItem=m_TempNetworkItemArray[i];
				CWHDataLocker ThreadLock(pTCPNetworkItem->GetCriticalSection());

				//有效判断
				if (pTCPNetworkItem->IsValidSocket()==false) continue;

				//连接判断
				if (pTCPNetworkItem->IsAllowSendData()==true)
				{
					switch (pTCPNetworkItem->m_wSurvivalTime)
					{
					case DEAD_QUOTIETY:		//死亡连接
						{
							pTCPNetworkItem->CloseSocket(pTCPNetworkItem->GetRountID());
							break;
						}
					case DANGER_QUOTIETY:	//危险系数
						{
							pTCPNetworkItem->m_wSurvivalTime--;
							pTCPNetworkItem->SendData(MDM_KN_COMMAND,SUB_KN_DETECT_SOCKET,pTCPNetworkItem->GetRountID());
							break;
						}
					default:				//默认处理
						{ 
							pTCPNetworkItem->m_wSurvivalTime--; 
							break; 
						}
					}
				}
				else	//特殊连接
				{
					if ((pTCPNetworkItem->GetActiveTime()+4)<=dwNowTime)
					{
						pTCPNetworkItem->CloseSocket(pTCPNetworkItem->GetRountID());
						continue;
					}
				}
			}

			return true;
		}
	}

	//效验数据
	ASSERT(FALSE);

	return false;
}

//绑定事件
bool CTCPNetworkEngine::OnEventSocketBind(CTCPNetworkItem * pTCPNetworkItem)
{
	//效验数据
	ASSERT(pTCPNetworkItem!=NULL);
	ASSERT(m_pITCPNetworkEngineEvent!=NULL);

	//投递消息
	DWORD dwClientIP=pTCPNetworkItem->GetClientIP();
	DWORD dwSocketID=pTCPNetworkItem->GetIdentifierID();
	m_pITCPNetworkEngineEvent->OnEventTCPNetworkBind(dwSocketID,dwClientIP);

	return true;
}

//关闭事件
bool CTCPNetworkEngine::OnEventSocketShut(CTCPNetworkItem * pTCPNetworkItem)
{
	//效验参数
	ASSERT(pTCPNetworkItem!=NULL);
	ASSERT(m_pITCPNetworkEngineEvent!=NULL);

	try
	{
		//投递数据
		DWORD dwClientIP=pTCPNetworkItem->GetClientIP();
		DWORD dwSocketID=pTCPNetworkItem->GetIdentifierID();
		DWORD dwActiveTime=pTCPNetworkItem->GetActiveTime();
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwSocketID,dwClientIP,dwActiveTime);

		//释放连接
		FreeNetworkItem(pTCPNetworkItem);
	}
	catch (...) {}

	return true;
}

//读取事件
bool CTCPNetworkEngine::OnEventSocketRead(TCP_Command Command, VOID * pData, WORD wDataSize, CTCPNetworkItem * pTCPNetworkItem)
{
	//效验数据
	ASSERT(pTCPNetworkItem!=NULL);
	ASSERT(m_pITCPNetworkEngineEvent!=NULL);

	//运行判断
	if (m_bNormalRun==false)
	{
		//创建对象
		HANDLE hCompletionPort=NULL;
		hCompletionPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,1);

		//进入循环
		while (true)
		{
			DWORD dwThancferred=0;					
			OVERLAPPED * pOverLapped=NULL;
			CTCPNetworkItem * pTCPNetworkItem=NULL;
			GetQueuedCompletionStatus(hCompletionPort,&dwThancferred,(PULONG_PTR)&pTCPNetworkItem,&pOverLapped,INFINITE);
		}

		return false;
	}

	//投递消息
	DWORD dwSocketID=pTCPNetworkItem->GetIdentifierID();
	m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(dwSocketID,Command,pData,wDataSize);

	return true;
}

//检测连接
bool CTCPNetworkEngine::DetectSocket()
{
	//发送请求
	void* data = NULL;
	return m_AsynchronismEngine.PostAsynchronismData(ASYNCHRONISM_DETECT_SOCKET,data,0);
}

//网页验证
bool CTCPNetworkEngine::WebAttestation()
{
	//return true;	//added by lrq 2013-12-10

	////获取目录
	//TCHAR szPath[MAX_PATH]=TEXT("");
	//GetCurrentDirectory(sizeof(szPath),szPath);

	////读取配置
	//TCHAR szFileName[MAX_PATH]=TEXT("");;
	//_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\Validate.ini"),szPath);

	////变量定义
	//DWORD dwClientID=0L;
	//TCHAR szClientName[32]=TEXT("");
	//TCHAR szClientValidate[33]=TEXT("");

	////读取验证
	//dwClientID=GetPrivateProfileInt(TEXT("Data"),TEXT("ClientID"),0,szFileName);
	//GetPrivateProfileString(TEXT("Data"),TEXT("ClientName"),TEXT(""),szClientName,sizeof(szClientName),szFileName);
	//GetPrivateProfileString(TEXT("Data"),TEXT("ClientValidate"),TEXT(""),szClientValidate,sizeof(szClientValidate),szFileName);

	////变量定义
	//CInternetSession Seccion;
	//CStdioFile * pInternetFile=NULL;

	//try
	//{
	//	//构造验证
	//	TCHAR szAttestation[512]=TEXT("");
	//	_sntprintf(szAttestation,CountArray(szAttestation),TEXT("http://www.gamexx.net.com/Attestation.html?Version=66&ClientID=%ld&ClientName=%s&Validate=%s"),
	//		dwClientID,szClientName,szClientValidate);

	//	//执行验证
	//	TCHAR szResult[1024]=TEXT("");
	//	pInternetFile=Seccion.OpenURL(szAttestation);
	//	UINT uReadCount=pInternetFile->Read(szResult,sizeof(szResult));
	//	szResult[__min(uReadCount,CountArray(szResult)-1)]=0;

	//	//停止判断
	//	if ((uReadCount>=2)&&(szResult[0]==TEXT('0'))&&(szResult[1]==TEXT(';')))
	//	{
	//		//设置变量
	//		m_bValidate=false;
	//		m_bNormalRun=false;

	//		//提示信息
	//		g_TraceServiceManager.TraceString(TEXT("您的服务器组件没有得到合法授权，请联系“疯石游戏” http://www.gamexx.net"),TraceLevel_Exception);

	//		return false;
	//	}

	//	//验证判断
	//	if ((uReadCount>(UINT)lstrlen(szClientValidate))&&(memcmp(szClientValidate,szResult,lstrlen(szClientValidate)*sizeof(TCHAR))==0))
	//	{
	//		//设置变量
	//		m_bValidate=true;
	//		m_bNormalRun=true;

	//		return true;
	//	}
	//	else
	//	{
	//		//设置变量
	//		m_bValidate=false;
	//		m_bNormalRun=true;

	//		return false;
	//	}

	//	return true;
	//}
	//catch (...) {}

	//设置变量
	m_bValidate=true;
	m_bNormalRun=true;

	return true;
}

//获取对象
CTCPNetworkItem * CTCPNetworkEngine::ActiveNetworkItem()
{
	//锁定队列
	CWHDataLocker ThreadLock(m_ItemLocked,true);

	//获取对象
	CTCPNetworkItem * pTCPNetworkItem=NULL;
	if (m_NetworkItemBuffer.GetCount()>0)
	{
		INT_PTR nItemPostion=m_NetworkItemBuffer.GetCount()-1;
		pTCPNetworkItem=m_NetworkItemBuffer[nItemPostion];
		m_NetworkItemBuffer.RemoveAt(nItemPostion);
		m_NetworkItemActive.Add(pTCPNetworkItem);
	}

	//创建对象
	if (pTCPNetworkItem==NULL)
	{
		WORD wStorageCount=(WORD)m_NetworkItemStorage.GetCount();
		if (wStorageCount<m_wMaxConnect)
		{
			try
			{
				//创建对象
				pTCPNetworkItem=new CTCPNetworkItem(wStorageCount,this);
				if (pTCPNetworkItem==NULL) 
				{
					ASSERT(FALSE);
					return NULL;
				}

				//插入数组
				m_NetworkItemActive.Add(pTCPNetworkItem);
				m_NetworkItemStorage.Add(pTCPNetworkItem);
			}
			catch (...) 
			{ 
				ASSERT(FALSE);
				return NULL; 
			}
		}
	}

	return pTCPNetworkItem;
}

//获取对象
CTCPNetworkItem * CTCPNetworkEngine::GetNetworkItem(WORD wIndex)
{
	//锁定对象
	CWHDataLocker ThreadLock(m_ItemLocked,true);

	//效验索引
	ASSERT(wIndex<m_NetworkItemStorage.GetCount());
	if (wIndex>=m_NetworkItemStorage.GetCount()) return NULL;

	//获取对象
	CTCPNetworkItem * pTCPNetworkItem=m_NetworkItemStorage[wIndex];

	return pTCPNetworkItem;
}

//释放连接对象
bool CTCPNetworkEngine::FreeNetworkItem(CTCPNetworkItem * pTCPNetworkItem)
{
	//效验参数
	ASSERT(pTCPNetworkItem!=NULL);

	//释放对象
	CWHDataLocker ThreadLock(m_ItemLocked,true);
	INT_PTR nActiveCount=m_NetworkItemActive.GetCount();
	for (INT i=0;i<nActiveCount;i++)
	{
		if (pTCPNetworkItem==m_NetworkItemActive[i])
		{
			m_NetworkItemActive.RemoveAt(i);
			m_NetworkItemBuffer.Add(pTCPNetworkItem);
			return true;
		}
	}

	//释放失败
	ASSERT(FALSE);

	return false;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(TCPNetworkEngine);

//////////////////////////////////////////////////////////////////////////
