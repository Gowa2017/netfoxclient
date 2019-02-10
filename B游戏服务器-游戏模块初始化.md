[TOC]
# 前文

在前一文章中我们说到，GameServer 内有一个服务组件单元 CServiceUnits，有它进行具体游戏服务的加载，启动，关闭，初始化等等。

在服务单元进行启动的过程中，其会调用内核级别的 dll 文件，建立相关的调度引擎，时间引擎，网络引擎，数据库引擎等服务对象。然而，调度引擎才是我们的核心，我们所有的事件信息都要发送至调度引擎，由调度引擎进行分发。

最终，落到我们指定的内存对象上去。

# 调度引擎的启动

单独针对调度引擎而言，其过程也没有什么特别的。

## CreateServiceDLL()    

```cpp
	//调度引擎
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_AttemperEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}
```
## InitializeService
设置一下调度引擎的钩子函数，


```cpp
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;

```
## StartKernelService

```cpp
	if (m_AttemperEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

```

对于调度引擎而言，其启动的时候，还会启动一个异步引擎，开启一个异步线程。所有的信息都是异步提交到队列中，然后由异步线程轮询进行处理，分发到不同的地方去。

## CAttemperEngine::StartService()

调度引擎会启动一个异步引擎，同时将异步引擎的钩子函数设置为自身。也就是说，调度引擎会异步提交消息到队列中，异步引擎的处理线程又会将消息转交给调度引擎处理，也就是做一个排队而已了。


```cpp
bool CAttemperEngine::StartService()
{
    // 效验参数
    ASSERT((m_pITCPNetworkEngine != NULL) && (m_pIAttemperEngineSink != NULL));
    if ((m_pITCPNetworkEngine == NULL) || (m_pIAttemperEngineSink == NULL))
        return false;

    // 注册对象
    IUnknownEx * pIAsynchronismEngineSink = QUERY_ME_INTERFACE(IUnknownEx);
    if (m_AsynchronismEngine.SetAsynchronismSink(pIAsynchronismEngineSink) == false)
    {
        ASSERT(FALSE);
        return false;
    }

    // 异步引擎
    if (m_AsynchronismEngine.StartService() == false)
        return false;

    return true;
}

```


### CAsynchronismThread::OnEventThreadStrat()
异步线程启动时，会调用异步线程的钩子函数（前已经设置为调度引擎），因此，实际上会调用调度引擎的  OnAsynchronismEngineStart() 方法。

```cpp
bool CAsynchronismThread::OnEventThreadStrat()
{
    // 事件通知
    ASSERT(m_pIAsynchronismEngineSink != NULL);
    bool bSuccess = m_pIAsynchronismEngineSink->OnAsynchronismEngineStart();

    // 设置变量
    CAsynchronismEngine * pAsynchronismEngine = CONTAINING_RECORD(this, CAsynchronismEngine, m_AsynchronismThread);
    pAsynchronismEngine->m_bService = true;

    return bSuccess;
}

```

### CAsynchronismThread::OnEventThreadRun()

异步线程会轮训消息队列，然后将消息转交给钩子函数，也就是调度引擎。

```cpp
bool CAsynchronismThread::OnEventThreadRun()
{
    // 效验参数
    ASSERT(m_hCompletionPort != NULL);
    ASSERT(m_pIAsynchronismEngineSink != NULL);

    // 变量定义
    DWORD dwThancferred = 0;
    OVERLAPPED * pOverLapped = NULL;
    CAsynchronismEngine * pAsynchronismEngine = NULL;

    // 完成端口
    if (GetQueuedCompletionStatus(m_hCompletionPort, &dwThancferred, (PULONG_PTR)&pAsynchronismEngine, &pOverLapped, INFINITE))
    {
        // 退出判断
        if (pAsynchronismEngine == NULL)
            return false;

        // 队列锁定
        CWHDataLocker ThreadLock(pAsynchronismEngine->m_CriticalSection);

        // 提取数据
        tagDataHead DataHead;
        pAsynchronismEngine->m_DataQueue.DistillData(DataHead, m_cbBuffer, sizeof(m_cbBuffer));

        // 队列解锁
        ThreadLock.UnLock();

        // 数据处理
        try
        {
            m_pIAsynchronismEngineSink->OnAsynchronismEngineData(DataHead.wIdentifier, m_cbBuffer, DataHead.wDataSize);
        }
        catch (...)
        {
            // 构造信息
            TCHAR szDescribe[256] = TEXT("");
            _sntprintf(szDescribe, CountArray(szDescribe), TEXT("CAsynchronismEngine::OnAsynchronismEngineData [ wIdentifier=%d wDataSize=%ld ]"),
                DataHead.wIdentifier, DataHead.wDataSize);

            // 输出信息
            g_TraceServiceManager.TraceString(szDescribe, TraceLevel_Exception);
        }

        return true;
    }

    return false;
}

```

## CAttemperEngine::OnAsynchronismEngineStart()

在调度引擎收到异步启动的消息时，也会去调用其钩子函数的启动方法。

```cpp
bool CAttemperEngine::OnAsynchronismEngineStart()
{
    // 效验参数
    ASSERT(m_pIAttemperEngineSink != NULL);
    if (m_pIAttemperEngineSink == NULL)
        return false;

    // 启动通知
    if (m_pIAttemperEngineSink->OnAttemperEngineStart(QUERY_ME_INTERFACE(IUnknownEx)) == false)
    {
        ASSERT(FALSE);
        return false;
    }

    return true;
}

```

### CAttemperEngineSink::OnAttemperEngineStart

在这里，才是真正的启动业务逻辑，配置了桌子，属性等。

```cpp
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定信息
	m_pAndroidParameter=new tagBindParameter[MAX_ANDROID];
	ZeroMemory(m_pAndroidParameter,sizeof(tagBindParameter)*MAX_ANDROID);

	//绑定信息
	m_pNormalParameter=new tagBindParameter[m_pGameServiceOption->wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pGameServiceOption->wMaxPlayer);

	//配置比赛
	InitMatchServiceManager();

	//配置约战房
	InitPersonalRoomServiceManager();

	//配置机器
	if (InitAndroidUser()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//配置桌子
	if (InitTableFrameArray()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置接口
	if(m_pIMatchServiceManager!=NULL)
	{
		if (m_ServerUserManager.SetServerUserItemSink(m_pIMatchServiceManager->GetServerUserItemSink())==false)
		{
			ASSERT(FALSE);
			return false;
		}

	}
	else	if(m_pIPersonalRoomServiceManager!=NULL)//设置约战房间接口
	{
		if (m_ServerUserManager.SetServerUserItemSink(m_pIPersonalRoomServiceManager->GetServerUserItemSink())==false)
		{
			ASSERT(FALSE);
			return false;
		}

	}
	else
	{
		if (m_ServerUserManager.SetServerUserItemSink(QUERY_ME_INTERFACE(IServerUserItemSink))==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//启动机器
	if (m_AndroidUserManager.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置接口
	m_AndroidUserManager.SetMainServiceFrame(QUERY_ME_INTERFACE(IMainServiceFrame));

	//设置规则
	m_DistributeManage.SetDistributeRule(m_pGameServiceOption->cbDistributeRule);

	//设置时间
	m_pITimerEngine->SetTimer(IDI_GAME_SERVICE_PULSE,1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_DBCORRESPOND_NOTIFY,TIME_DBCORRESPOND_NOTIFY*1000L,TIMES_INFINITY,NULL);

#ifdef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,15*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,5*1000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,TIME_LOAD_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,TIME_SEND_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
#endif

	//延时加载敏感词
	m_pITimerEngine->SetTimer(IDI_LOAD_SENSITIVE_WORD,TIME_LOAD_SENSITIVE_WORD*1000L,TIMES_INFINITY,NULL);

	//防作弊模式
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_ALLOW)!=0)
	{
		m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pGameServiceOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	}

	//机器人参赛
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		m_pITimerEngine->SetTimer(IDI_ANDROID_JION_MATCH,m_pGameServiceOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	}

	//加载机器
	if (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule) || 
		CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule) )
	{
#ifdef _DEBUG			
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,10000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ANDROID,TIME_DISTRIBUTE_ANDROID*1000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,TIME_LOAD_ANDROID_USER*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ANDROID,TIME_DISTRIBUTE_ANDROID*1000L,TIMES_INFINITY,NULL);
#endif
	}

	return true;
}


```

我们重点看一下桌子的初始化。

### CAttemperEngineSink::InitTableFrameArray()

桌子对象持有了我们服务单元的 时间引擎，内核数据库引擎及记录数据库引擎。每张桌子的配置，其实是一样的。

```cpp
bool CAttemperEngineSink::InitTableFrameArray()
{
	//桌子参数
	tagTableFrameParameter TableFrameParameter;
	ZeroMemory(&TableFrameParameter,sizeof(TableFrameParameter));

	//内核组件
	TableFrameParameter.pITimerEngine=m_pITimerEngine;
	TableFrameParameter.pIKernelDataBaseEngine=m_pIKernelDataBaseEngine;
	TableFrameParameter.pIRecordDataBaseEngine=m_pIRecordDataBaseEngine;

	//服务组件
	TableFrameParameter.pIMainServiceFrame=this;
	TableFrameParameter.pIAndroidUserManager=&m_AndroidUserManager;
	TableFrameParameter.pIGameServiceManager=m_pIGameServiceManager;
	TableFrameParameter.pITCPSocketService = m_pITCPSocketService;

	//配置参数
	TableFrameParameter.pGameParameter=m_pGameParameter;
	TableFrameParameter.pGameMatchOption=m_pGameMatchOption;
	TableFrameParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption=m_pGameServiceOption;

	//桌子容器
	m_TableFrameArray.SetSize(m_pGameServiceOption->wTableCount);
	ZeroMemory(m_TableFrameArray.GetData(),m_pGameServiceOption->wTableCount*sizeof(CTableFrame *));

	//创建桌子
	for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//创建对象
		m_TableFrameArray[i]=new CTableFrame;

		//配置桌子
		if (m_TableFrameArray[i]->InitializationFrame(i,TableFrameParameter)==false)
		{
			return false;
		}

		//绑定桌子
		if(m_pIMatchServiceManager!=NULL)
		{
			m_pIMatchServiceManager->BindTableFrame(QUERY_OBJECT_PTR_INTERFACE((m_TableFrameArray[i]),ITableFrame),i);
		}

		//约战服务绑定桌子
		if(m_pIPersonalRoomServiceManager!=NULL)
		{
			m_pIPersonalRoomServiceManager->BindTableFrame(QUERY_OBJECT_PTR_INTERFACE((m_TableFrameArray[i]),ITableFrame),i);
		}
	}	

	return true;
}
```

### CTableFrame::InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)

我们在调度引擎的回调函数中，初始化桌子的相关参数，然后进行初始化。在这里，我们关注一下 pIGameServiceManager 。这个其实就是我们游戏模块的 Dll ，其提供了桌子处理相关逻辑的接口方法。

```cpp
bool CTableFrame::InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//设置变量
	m_wTableID=wTableID;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//配置参数
	m_pGameParameter=TableFrameParameter.pGameParameter;
	m_pGameMatchOption=TableFrameParameter.pGameMatchOption;
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;

	//组件接口
	m_pITimerEngine=TableFrameParameter.pITimerEngine;
	m_pIMainServiceFrame=TableFrameParameter.pIMainServiceFrame;
	m_pIAndroidUserManager=TableFrameParameter.pIAndroidUserManager;
	m_pIKernelDataBaseEngine=TableFrameParameter.pIKernelDataBaseEngine;
	m_pIRecordDataBaseEngine=TableFrameParameter.pIRecordDataBaseEngine;
	m_pITCPSocketService = TableFrameParameter.pITCPSocketService;

	//创建桌子
	IGameServiceManager * pIGameServiceManager=TableFrameParameter.pIGameServiceManager;
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);

	//错误判断
	if (m_pITableFrameSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}
	
	//设置变量
	m_lCellScore=m_pGameServiceOption->lCellScore;

	//设置桌子
	IUnknownEx * pITableFrame=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->Initialization(pITableFrame)==false) return false;

	//设置标识
	m_bTableInitFinish=true;

	//扩展接口
	m_pITableUserAction=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserAction);
	m_pITableUserRequest=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserRequest);

	//读取配置
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//构造路径
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf(szIniFile,CountArray(szIniFile),TEXT("%s\\PersonalTable.ini"),szWorkDir);

	//读取配置
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	m_PlayerWaitTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("PlayerWaitTime"),120);
	m_TableOwnerWaitTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("TableOwnerWaitTime"),1800);
	m_GameStartOverTime = IniData.ReadInt(m_pGameServiceAttrib->szGameName,TEXT("GameStartOverTime"),900);

	return true;
}

```

这样，每当有游戏数据过来的时候，就会调用我们的游戏模块定义的桌子数据处理接口，来处理相关逻辑。

### 桌子处理逻辑接口

这里还没完，最终还是落到了我们游戏模块的接口初始化方法上来。以百家乐为例来说：

```cpp
void * CGameServiceManager::CreateTableFrameSink(const IID & Guid, DWORD dwQueryVer)
{
	//建立对象
	CTableFrameSink * pTableFrameSink=NULL;
	try
	{
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("创建失败");
		void * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pTableFrameSink);

	return NULL;
}


bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame == NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	//读取配置
	memcpy(m_szGameRoomName, m_pGameServiceOption->szServerName, sizeof(m_szGameRoomName));

	//设置文件名
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	_sntprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\BaccaratNewConfig.ini"),szPath);

	ReadConfigInformation();	

	return true;
}


```

这样一个服务就启动起来，并进行了初始化了。
## CAttemperEngine::OnAsynchronismEngineData

当调度引擎收到异步数据的时候，会根据数据类型进行分发。我们关注一下来自于网络的事件。SOCKET 属于网络连接类型消息，而 NETWORK 归属于网络数据类型。

```cpp
bool CAttemperEngine::OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
    // 效验参数
    ASSERT(m_pITCPNetworkEngine != NULL);
    ASSERT(m_pIAttemperEngineSink != NULL);

    // 内核事件
    switch (wIdentifier)
    {
        case EVENT_TIMER:				// 时间事件
        {
            // 大小断言
            ASSERT(wDataSize == sizeof(NTY_TimerEvent));
            if (wDataSize != sizeof(NTY_TimerEvent))
                return false;

            // 处理消息
            NTY_TimerEvent * pTimerEvent = (NTY_TimerEvent *)pData;
            m_pIAttemperEngineSink->OnEventTimer(pTimerEvent->dwTimerID, pTimerEvent->dwBindParameter);

            return true;
        }
        case EVENT_CONTROL:				// 控制消息
        {
            // 大小断言
            ASSERT(wDataSize >= sizeof(NTY_ControlEvent));
            if (wDataSize < sizeof(NTY_ControlEvent))
                return false;

            // 处理消息
            NTY_ControlEvent * pControlEvent = (NTY_ControlEvent *)pData;
            m_pIAttemperEngineSink->OnEventControl(pControlEvent->wControlID, pControlEvent + 1, wDataSize - sizeof(NTY_ControlEvent));

            return true;
        }
        case EVENT_DATABASE:			// 数据库事件
        {
            // 大小断言
            ASSERT(wDataSize >= sizeof(NTY_DataBaseEvent));
            if (wDataSize < sizeof(NTY_DataBaseEvent))
                return false;

            // 处理消息
            NTY_DataBaseEvent * pDataBaseEvent = (NTY_DataBaseEvent *)pData;
            m_pIAttemperEngineSink->OnEventDataBase(pDataBaseEvent->wRequestID, pDataBaseEvent->dwContextID, pDataBaseEvent + 1, wDataSize - sizeof(NTY_DataBaseEvent));

            return true;
        }
        case EVENT_TCP_SOCKET_READ:		// 读取事件
        {
            // 变量定义
            NTY_TCPSocketReadEvent * pReadEvent = (NTY_TCPSocketReadEvent *)pData;

            // 大小断言
            ASSERT(wDataSize >= sizeof(NTY_TCPSocketReadEvent));
            ASSERT(wDataSize == (sizeof(NTY_TCPSocketReadEvent) + pReadEvent->wDataSize));

            // 大小效验
            if (wDataSize < sizeof(NTY_TCPSocketReadEvent))
                return false;
            if (wDataSize != (sizeof(NTY_TCPSocketReadEvent) + pReadEvent->wDataSize))
                return false;

            // 处理消息
            m_pIAttemperEngineSink->OnEventTCPSocketRead(pReadEvent->wServiceID, pReadEvent->Command, pReadEvent + 1, pReadEvent->wDataSize);

            return true;
        }
        case EVENT_TCP_SOCKET_SHUT:	// 关闭事件
        {
            // 大小断言
            ASSERT(wDataSize == sizeof(NTY_TCPSocketShutEvent));
            if (wDataSize != sizeof(NTY_TCPSocketShutEvent))
                return false;

            // 处理消息
            NTY_TCPSocketShutEvent * pCloseEvent = (NTY_TCPSocketShutEvent *)pData;
            m_pIAttemperEngineSink->OnEventTCPSocketShut(pCloseEvent->wServiceID, pCloseEvent->cbShutReason);

            return true;
        }
        case EVENT_TCP_SOCKET_LINK:	// 连接事件
        {
            // 大小断言
            ASSERT(wDataSize == sizeof(NTY_TCPSocketLinkEvent));
            if (wDataSize != sizeof(NTY_TCPSocketLinkEvent))
                return false;

            // 处理消息
            NTY_TCPSocketLinkEvent * pConnectEvent = (NTY_TCPSocketLinkEvent *)pData;
            m_pIAttemperEngineSink->OnEventTCPSocketLink(pConnectEvent->wServiceID, pConnectEvent->nErrorCode);

            return true;
        }
        case EVENT_TCP_NETWORK_ACCEPT:	// 应答事件
        {
            // 大小断言
            ASSERT(wDataSize == sizeof(NTY_TCPNetworkAcceptEvent));
            if (wDataSize != sizeof(NTY_TCPNetworkAcceptEvent))
                return false;

            // 变量定义
            bool bSuccess = false;
            NTY_TCPNetworkAcceptEvent * pAcceptEvent = (NTY_TCPNetworkAcceptEvent *)pData;

            // 处理消息
            try
            {
                bSuccess = m_pIAttemperEngineSink->OnEventTCPNetworkBind(pAcceptEvent->dwClientAddr, pAcceptEvent->dwSocketID);
            }
            catch (...) {}

            // 失败处理
            if (bSuccess == false) m_pITCPNetworkEngine->CloseSocket(pAcceptEvent->dwSocketID);

            return true;
        }
        case EVENT_TCP_NETWORK_READ:	// 读取事件
        {
            // 效验大小
            NTY_TCPNetworkReadEvent * pReadEvent = (NTY_TCPNetworkReadEvent *)pData;

            // 大小断言
            ASSERT(wDataSize >= sizeof(NTY_TCPNetworkReadEvent));
            ASSERT(wDataSize == (sizeof(NTY_TCPNetworkReadEvent) + pReadEvent->wDataSize));

            // 大小效验
            if (wDataSize < sizeof(NTY_TCPNetworkReadEvent))
            {
                m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
                return false;
            }

            // 大小效验
            if (wDataSize != (sizeof(NTY_TCPNetworkReadEvent) + pReadEvent->wDataSize))
            {
                m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
                return false;
            }

            // 处理消息
            bool bSuccess = false;
            try
            {
                bSuccess = m_pIAttemperEngineSink->OnEventTCPNetworkRead(pReadEvent->Command, pReadEvent + 1, pReadEvent->wDataSize, pReadEvent->dwSocketID);
            }
            catch (...) {}

            // 失败处理
            if (bSuccess == false) m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);

            return true;
        }
        case EVENT_TCP_NETWORK_SHUT:	// 关闭事件
        {
            // 大小断言
            ASSERT(wDataSize == sizeof(NTY_TCPNetworkShutEvent));
            if (wDataSize != sizeof(NTY_TCPNetworkShutEvent))
                return false;

            // 处理消息
            NTY_TCPNetworkShutEvent * pCloseEvent = (NTY_TCPNetworkShutEvent *)pData;
            m_pIAttemperEngineSink->OnEventTCPNetworkShut(pCloseEvent->dwClientAddr, pCloseEvent->dwActiveTime, pCloseEvent->dwSocketID);

            return true;
        }
    }

    // 其他事件
    return m_pIAttemperEngineSink->OnEventAttemperData(wIdentifier, pData, wDataSize);
}

```

### CAttemperEngineSink::OnEventTCPNetworkRead

这里也不处理数据，只是做数据的分发。

```cpp
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GR_USER:		//用户命令
		{
			return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_LOGON:		//登录命令
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_GAME:		//游戏命令
		{
			bool bReturnVal = OnTCPNetworkMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			if (!bReturnVal)
			{
				TCHAR szInfo[260] = {0};
				wsprintf(szInfo, TEXT("游戏命令返回 false   wMainCmdID = %d  wSubCmdID = %d "),  Command.wMainCmdID, Command.wSubCmdID);
				CTraceService::TraceString(szInfo, TraceLevel_Normal);
			}
			return bReturnVal;
		}
	case MDM_GF_FRAME:		//框架命令
		{
			bool bReturnVal =  OnTCPNetworkMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
			if (!bReturnVal)
			{
				CTraceService::TraceString(TEXT("MDM_GF_FRAME 框架命令返回 false"),TraceLevel_Normal);
			}
			return bReturnVal;
		}
	case MDM_GR_INSURE:		//银行命令
		{
			return OnTCPNetworkMainInsure(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_TASK:		//任务命令
		{
			return OnTCPNetworkMainTask(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_EXCHANGE:	//兑换命令
		{
			return OnTCPNetworkMainExchange(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MANAGE:		//管理命令
		{
			return OnTCPNetworkMainManage(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MATCH:		//比赛命令
		{
			return OnTCPNetworkMainMatch(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MEMBER:	//会员命令
		{
			return OnTCPNetworkMainMember(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_PROPERTY:	//道具命令
		{
			return OnTCPNetworkMainProperty(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_BASEENSURE:	//低保命令
		{
			return OnTCPNetworkMainBaseEnsure(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_CHECKIN: //签到命令
		{
			return OnTCPNetworkMainCheckIn(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_PERSONAL_TABLE:
		{
			return OnTCPNetworkMainPersonalTable(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}

	return false;

```

### CAttemperEngineSink::OnTCPNetworkMainGame

对于游戏中的数据，最终，是根据消息中的桌子ID来转交给对应的桌子对象处理。

```cpp
bool CAttemperEngineSink::OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

```

### CTableFrame::OnEventSocketGame

最终真正处理数据的，还是我们在子游戏模块内定义的数据处理接口。


```cpp
bool CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//消息处理
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

```

随便以一个游戏为例，比如百家乐，我们来看一下其处理方法。

### CTableFrameSink::OnGameMessage

```cpp
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_C_PlaceBet));
			if (wDataSize!=sizeof(CMD_C_PlaceBet)) return false;

			//用户效验
			//if ( pIServerUserItem->GetUserStatus() != US_PLAYING ) return true;

			//消息处理
			CMD_C_PlaceBet * pPlaceBet = (CMD_C_PlaceBet *)pData;
			return OnUserPlayBet(pIServerUserItem->GetChairID(), pPlaceBet->cbBetArea, pPlaceBet->lBetScore);
		}
	case SUB_C_APPLY_BANKER:		//申请做庄
		{
			//用户效验
			if ( pIServerUserItem->GetUserStatus() == US_LOOKON ) return true;

			return OnUserApplyBanker(pIServerUserItem);	
		}
	case SUB_C_CANCEL_BANKER:		//取消做庄
		{
			//用户效验
			if ( pIServerUserItem->GetUserStatus() == US_LOOKON ) return true;

			return OnUserCancelBanker(pIServerUserItem);	
		}
	case SUB_C_AMDIN_COMMAND:
		{
			ASSERT(wDataSize == sizeof(CMD_C_AdminReq));
			if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

			if ( m_pServerContro == NULL)
				return false;
			CopyMemory(m_szControlName,pIServerUserItem->GetNickName(),sizeof(m_szControlName));

			return m_pServerContro->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame, m_pGameServiceOption);
		}

	case SUB_C_UPDATE_STORAGE:		//更新库存
		{
			ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
			if(wDataSize!=sizeof(CMD_C_UpdateStorage)) return false;

			//权限判断
			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			//消息处理
			CMD_C_UpdateStorage * pUpdateStorage=(CMD_C_UpdateStorage *)pData;
			if (pUpdateStorage->cbReqType==RQ_SET_STORAGE)
			{
				LONGLONG lbeforeStorage = m_lStorageCurrent;
				m_lStorageDeduct = pUpdateStorage->lStorageDeduct;
				m_lStorageCurrent = pUpdateStorage->lStorageCurrent;
				m_lStorageMax1 = pUpdateStorage->lStorageMax1;
				m_lStorageMul1 = pUpdateStorage->lStorageMul1;
				m_lStorageMax2 = pUpdateStorage->lStorageMax2;
				m_lStorageMul2 = pUpdateStorage->lStorageMul2;

				//记录信息
				CString strControlInfo, str;
				str.Format(TEXT("%s"), TEXT("修改库存设置！"));
				CTime Time(CTime::GetCurrentTime());
				strControlInfo.Format(TEXT("房间: %s | 桌号: %u | 时间: %d-%d-%d %d:%d:%d\n控制人账号: %s | 控制人ID: %u\n%s, 库存由[%I64d]修改为[%I64d]\n\n"),
					m_pGameServiceOption->szServerName, m_pITableFrame->GetTableID()+1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
					Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str,
					lbeforeStorage, m_lStorageCurrent);

				WriteInfo(TEXT("新百家乐控制信息.log"),strControlInfo);
			}
			
			for(WORD wUserID = 0; wUserID < GAME_PLAYER; wUserID++)
			{
				IServerUserItem *pIServerUserItemSend = m_pITableFrame->GetTableUserItem(wUserID);
				if ( pIServerUserItemSend == NULL ) continue;
				if( !CUserRight::IsGameCheatUser( pIServerUserItemSend->GetUserRight() ) ) continue;
				
				if (pIServerUserItem->IsAndroidUser())
				{
					continue;
				}

				if(RQ_REFRESH_STORAGE == pUpdateStorage->cbReqType && pIServerUserItem->GetChairID() != wUserID) continue;

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
				updateStorage.lStorageDeduct = m_lStorageDeduct;
				updateStorage.lStorageCurrent = m_lStorageCurrent;
				updateStorage.lStorageMax1 = m_lStorageMax1;
				updateStorage.lStorageMul1 = m_lStorageMul1;
				updateStorage.lStorageMax2 = m_lStorageMax2;
				updateStorage.lStorageMul2 = m_lStorageMul2;

				m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_UPDATE_STORAGE,&updateStorage,sizeof(updateStorage));
			}

			return true;
		}
	case SUB_C_SUPERROB_BANKER:
		{
			//用户效验
			if ( pIServerUserItem->GetUserStatus() == US_LOOKON ) 
			{
				return true;
			}

			//校验条件
			if (m_superbankerConfig.superbankerType == SUPERBANKER_VIPTYPE)
			{
				ASSERT (pIServerUserItem->GetMemberOrder() >= GetMemberOrderIndex(m_superbankerConfig.enVipIndex));

				if (!(pIServerUserItem->GetMemberOrder() >= GetMemberOrderIndex(m_superbankerConfig.enVipIndex)))
				{
					return true;
				}
			}
			else if (m_superbankerConfig.superbankerType == SUPERBANKER_CONSUMETYPE)
			{
				ASSERT (pIServerUserItem->GetUserScore() > m_superbankerConfig.lSuperBankerConsume);

				if (!(pIServerUserItem->GetUserScore() > m_superbankerConfig.lSuperBankerConsume))
				{
					return true;
				}
			}

			//已有超级抢庄玩家
			if (m_wCurSuperRobBankerUser != INVALID_CHAIR)
			{
				CMD_S_SuperRobBanker SuperRobBanker;
				ZeroMemory(&SuperRobBanker, sizeof(SuperRobBanker));
				SuperRobBanker.bSucceed = false;
				SuperRobBanker.wCurSuperRobBankerUser = m_wCurSuperRobBankerUser;
				SuperRobBanker.wApplySuperRobUser = pIServerUserItem->GetChairID();

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SUPERROB_BANKER, &SuperRobBanker, sizeof(SuperRobBanker));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SUPERROB_BANKER, &SuperRobBanker, sizeof(SuperRobBanker));
			}
			else
			{
				CMD_S_SuperRobBanker SuperRobBanker;
				ZeroMemory(&SuperRobBanker, sizeof(SuperRobBanker));
				SuperRobBanker.bSucceed = true;
				SuperRobBanker.wCurSuperRobBankerUser = pIServerUserItem->GetChairID();
				SuperRobBanker.wApplySuperRobUser = pIServerUserItem->GetChairID();
				m_wCurSuperRobBankerUser = pIServerUserItem->GetChairID();
				
				//超级抢庄消耗类型
				if (m_superbankerConfig.superbankerType == SUPERBANKER_CONSUMETYPE)
				{
					tagScoreInfo ScoreInfo;
					ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
					ScoreInfo.cbType = SCORE_TYPE_SERVICE;
					ScoreInfo.lRevenue = 0;
					ScoreInfo.lScore = -m_superbankerConfig.lSuperBankerConsume;
					m_pITableFrame->WriteUserScore(pIServerUserItem->GetChairID(), ScoreInfo);
				}

				m_ApplyUserArray.Add(INVALID_CHAIR);
				//改变上庄列表顺序
				for (WORD i=m_ApplyUserArray.GetCount()-1; i>0; i--)
				{
					m_ApplyUserArray[i] = m_ApplyUserArray[i-1];
				}

				m_ApplyUserArray[0] = pIServerUserItem->GetChairID();


				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SUPERROB_BANKER, &SuperRobBanker, sizeof(SuperRobBanker));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SUPERROB_BANKER, &SuperRobBanker, sizeof(SuperRobBanker));
			}
		
			return true;
		}
	case SUB_C_OCCUPYSEAT:
		{
			//效验数据
			ASSERT (wDataSize == sizeof(CMD_C_OccupySeat));
			if (wDataSize != sizeof(CMD_C_OccupySeat))
			{
				return true;
			}

			//消息处理
			CMD_C_OccupySeat *pOccupySeat = (CMD_C_OccupySeat *)pData;
			return OnUserOccupySeat(pOccupySeat->wOccupySeatChairID, pOccupySeat->cbOccupySeatIndex);
		}
	case SUB_C_QUIT_OCCUPYSEAT:
		{	
			bool binvalid = false;
			//校验数据
			for (WORD i=0; i<MAX_OCCUPY_SEAT_COUNT; i++)
			{
				if (m_wOccupySeatChairIDArray[i] == pIServerUserItem->GetChairID())
				{
					binvalid = true;

					//重置无效
					m_wOccupySeatChairIDArray[i] = INVALID_CHAIR;

					break;
				}
			}

			ASSERT (binvalid == true);

			CMD_S_UpdateOccupySeat UpdateOccupySeat;
			ZeroMemory(&UpdateOccupySeat, sizeof(UpdateOccupySeat));
			CopyMemory(UpdateOccupySeat.wOccupySeatChairIDArray, m_wOccupySeatChairIDArray, sizeof(m_wOccupySeatChairIDArray));
			UpdateOccupySeat.wQuitOccupySeatChairID = pIServerUserItem->GetChairID();

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_OCCUPYSEAT, &UpdateOccupySeat, sizeof(UpdateOccupySeat));

			return true;
		}
	}

	return false;
}


```