[TOC]

# 简介

由于不是非常的了解 MFC，所以就从最基础的，最基本的开始。

# GameServer::InitInstance()

此方法，会进行初始化，同时持有一个对话框对象 GameServerDlg，并进行显示：

```cpp

//启动函数
BOOL CGameServerApp::InitInstance()
{
	__super::InitInstance();

	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szProduct);

	//显示窗口
	CGameServerDlg GameServerDlg;
	m_pMainWnd=&GameServerDlg;
	GameServerDlg.DoModal();

	return FALSE;
}
```

这其实就和常规的 GUI 变成没有什么区别。初始化对话框，然后对话框响应我们的操作，进行对应的方法调用。


# GameServerDlg 

对话框实现了 IServiceUnitsSink 接口，持有一个进行后台服务的组件 m_ServiceUnits。

```cpp
class CGameServerDlg : public CDialog, public IServiceUnitsSink
{
	//组件变量
protected:
	CServiceUnits					m_ServiceUnits;						//服务单元
	CTraceServiceControl			m_TraceServiceControl;				//追踪窗口

	//组件变量
protected:
	CModuleDBParameter				m_ModuleDBParameter;				//模块参数

	//配置参数
protected:
	bool							m_bAutoControl;						//自动控制
	bool							m_bOptionSuccess;					//配置标志
	tagModuleInitParameter			m_ModuleInitParameter;				//配置参数

	//函数定义
public:
	//构造函数
	CGameServerDlg();
	//析构函数
	virtual ~CGameServerDlg();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual VOID OnOK();
	//取消函数
	virtual VOID OnCancel();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//服务接口
public:
	//服务状态
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);

	//辅助函数
protected:
	//更新图标
	VOID UpdateServerLogo(LPCTSTR pszServerDLL);
	//更新标题
	VOID UpdateServerTitle(enServiceStatus ServiceStatus);
	//更新状态
	VOID UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter);

	//服务控制
protected:
	//启动房间
	bool StartServerService(WORD wServerID);
	//获取连接
	bool GetPlatformDBParameter(tagDataBaseParameter & DataBaseParameter);	

	//按钮消息
protected:
	//启动服务
	VOID OnBnClickedStartService();
	//停止服务
	VOID OnBnClickedStopService();
	//打开房间
	VOID OnBnClickedOpenServer();
	//创建房间
	VOID OnBnClickedCreateServer();
	//配置房间
	VOID OnBnClickedOptionServer();
	//配置比赛
	VOID OnBnClickedOptionMatch();

	//消息映射
public:
	//关闭询问
	BOOL OnQueryEndSession();
	//命令处理
	LRESULT OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
//////////////////////////////////////////////////////////////////////////////////
```

初始化的时候，把 m_ServiceUnits 的接口（因为 GameServer 已实现了 IServiceUnitsSink 接口） 设置为对话框。

也就是说，当我们的服务组件发生了状态变化的时候，会调用 GameServerDlg 实现的这个接口，更新显示一些信息出来。

```cpp
BOOL CGameServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("游戏服务器 -- [ 停止 ]"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//设置组件
	m_ServiceUnits.SetServiceUnitsSink(this);

	//命令处理
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]!=0) PostMessage(WM_PROCESS_CMD_LINE,0,(LPARAM)pszCmdLine);

	return TRUE;
}
```

# 服务相关

GameServerDlg 将其显示的按钮与几个事件进行了绑定。

```cpp
BEGIN_MESSAGE_MAP(CGameServerDlg, CDialog)

	//系统消息
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_OPEN_SERVER, OnBnClickedOpenServer) // 打开房间列表
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService) // 停止服务
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService) // 启动服务
	ON_BN_CLICKED(IDC_CREATE_SERVER, OnBnClickedCreateServer) // 创建房间
	ON_BN_CLICKED(IDC_OPTION_SERVER, OnBnClickedOptionServer) // 配置房间
	ON_BN_CLICKED(IDC_OPTION_MATCH, OnBnClickedOptionMatch) // 配置比赛

	//自定消息
	ON_MESSAGE(WM_PROCESS_CMD_LINE,OnMessageProcessCmdLine)

END_MESSAGE_MAP()
```

我们的操作（建立了房间服务器后）一般就是，打开房间列表，选择一个房间，然后启动服务。


## CGameServerDlg::OnBnClickedOpenServer()

打开房间列表时，会首先从 ServerParameter.ini 获取数据库信息，进行连接。接着打开一个 CDlgServerItem 对话框供我们选择。

```cpp
VOID CGameServerDlg::OnBnClickedOpenServer()
{
	//变量定义
	tagDataBaseParameter DataBaseParameter;
	ZeroMemory(&DataBaseParameter,sizeof(DataBaseParameter));

	//设置参数
	GetPlatformDBParameter(DataBaseParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(DataBaseParameter);

	//配置房间
	CDlgServerItem DlgServerItem;
	if (DlgServerItem.OpenGameServer()==false) return;

	//更新状态
	UpdateParameterStatus(DlgServerItem.m_ModuleInitParameter);

	//加载私人房间配置
	if (DlgServerItem.m_ModuleInitParameter.GameServiceOption.wServerType == GAME_GENRE_PERSONAL)
	{
		DlgServerItem.LoadPersonalRoomPatameter(DlgServerItem.m_ModuleInitParameter.GameServiceOption.wKindID);
	}
	m_ModuleInitParameter.PersonalRoomOption = DlgServerItem.m_ModuleInitParameter.PersonalRoomOption;
	m_ModuleInitParameter.m_pCreateRoomRightAndFee = DlgServerItem.m_ModuleInitParameter.m_pCreateRoomRightAndFee;


	return;
}
```

当我们选择某一个房间时，就会回调 GameServerDlg 的方法进行更新相关的配置参数：

## CGameServerDlg::UpdateParameterStatus
```cpp
VOID CGameServerDlg::UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter)
{
	//设置变量
	m_bOptionSuccess=true;
	m_ModuleInitParameter=ModuleInitParameter;

	//更新标题
	UpdateServerTitle(ServiceStatus_Stop);
	UpdateServerLogo(ModuleInitParameter.GameServiceAttrib.szServerDLLName);

	//设置按钮
	GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
	GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(TRUE);

	//不是比赛就过滤
	if((ModuleInitParameter.GameServiceOption.wServerType&GAME_GENRE_MATCH)!=0)
	{
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(FALSE);
	}

	//设置控件
	SetDlgItemText(IDC_GAME_NAME,m_ModuleInitParameter.GameServiceAttrib.szGameName);
	SetDlgItemText(IDC_SERVER_NAME,m_ModuleInitParameter.GameServiceOption.szServerName);

	//监听端口
	if (m_ModuleInitParameter.GameServiceOption.wServerPort==0)
	{
		SetDlgItemText(IDC_SERVER_PORT,TEXT("自动适配"));
	}
	else
	{
		SetDlgItemInt(IDC_SERVER_PORT,m_ModuleInitParameter.GameServiceOption.wServerPort);
	}

	//设置模块
	LPCTSTR pszServerDLLName=m_ModuleInitParameter.GameServiceAttrib.szServerDLLName;
	m_ServiceUnits.CollocateService(pszServerDLLName,m_ModuleInitParameter.GameServiceOption);

	//构造提示
	TCHAR szString[256]=TEXT("");
	LPCTSTR pszServerName=m_ModuleInitParameter.GameServiceOption.szServerName;
	_sntprintf(szString,CountArray(szString),TEXT("[ %s ] 房间参数加载成功"),pszServerName);

	//输出信息
	CTraceService::TraceString(szString,TraceLevel_Normal);

	return;
}
```

重要的是 `m_ServiceUnits.CollocateService(pszServerDLLName,m_ModuleInitParameter.GameServiceOption);` 这一句，给后台服务组件传递了一个 dll 名称，及相关的参数。这个就是我们的子游戏逻辑所在。

## CServiceUnits::CollocateService

```cpp
bool CServiceUnits::CollocateService(LPCTSTR pszGameModule, tagGameServiceOption & GameServiceOption)
{
	//效验状态
	ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//配置模块
	m_GameServiceOption=GameServiceOption;
	m_GameServiceManager.SetModuleCreateInfo(pszGameModule,GAME_SERVICE_CREATE_NAME);

	return true;
}
```

从最开始的 ServiceUnits.h 中我们可以看到， m_GameServiceManager 是一个内核组件。

```cpp
public:
	CTimerEngineHelper				m_TimerEngine;						//时间引擎
	CAttemperEngineHelper			m_AttemperEngine;					//调度引擎
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//网络引擎
	CTCPSocketServiceHelper			m_TCPSocketService;					//网络服务
	CGameServiceManagerHelper		m_GameServiceManager;				//游戏模块
	CMatchServiceManagerHelper		m_MatchServiceManager;				//比赛管理
```

关于 Helper 类，有点蛋疼，是用宏的方式进行声明定义的，在*全局定义/Module.h* 中，定义了这么一个宏：

```cpp
//组件辅助类宏
#define DECLARE_MODULE_DYNAMIC(OBJECT_NAME)																	\
class C##OBJECT_NAME##Helper : public CTempldateHelper<I##OBJECT_NAME>										\
{																											\
public:																										\
	C##OBJECT_NAME##Helper() : CTempldateHelper<I##OBJECT_NAME>(IID_I##OBJECT_NAME,VER_I##OBJECT_NAME) { }	\
};

//组件辅助类宏
#define DECLARE_MODULE_HELPER(OBJECT_NAME,MODULE_DLL_NAME,CREATE_FUNCTION_NAME)								\
class C##OBJECT_NAME##Helper : public CTempldateHelper<I##OBJECT_NAME>										\
{																											\
public:																										\
	C##OBJECT_NAME##Helper() : CTempldateHelper<I##OBJECT_NAME>(IID_I##OBJECT_NAME,							\
		VER_I##OBJECT_NAME,MODULE_DLL_NAME,CREATE_FUNCTION_NAME) { }										\
};

```

在游戏服务项目，*GameServiceHead.h* 中声明了使用了这个宏：

```cpp
DECLARE_MODULE_DYNAMIC(GameServiceManager)
DECLARE_MODULE_DYNAMIC(AndroidUserItemSink)
```

作用是什么呢？就是根据名称，来生成一个通用的组件。

## Module.h

这个是一个全局的定义，我们来看一下其生成通用组件的形式，后面组件的加载对这个的依赖比较重。

```cpp
//组件模板辅助模板

//组件创建函数
typedef VOID * (ModuleCreateProc)(REFGUID Gudi, DWORD dwInterfaceVer);

//组件辅助类模板
template <typename IModeluInterface> class CTempldateHelper
{
	//接口属性
public:
	REFGUID							m_Guid;								//接口标识
	const DWORD						m_dwVersion;						//接口版本

	//组件属性
public:
	CHAR							m_szCreateProc[64];					//创建函数
	TCHAR							m_szModuleDllName[MAX_PATH];		//组件名字

	//内核变量
public:
	HINSTANCE						m_hDllInstance;						//DLL 句柄
	IModeluInterface *				m_pIModeluInterface;				//模块接口

	//辅助变量
public:
	TCHAR							m_szDescribe[128];					//错误描述

	//函数定义
public:
	//构造函数
	CTempldateHelper(REFGUID Guid, DWORD dwVersion);
	//构造函数
	CTempldateHelper(REFGUID Guid, DWORD dwVersion, LPCTSTR pszModuleDll, LPCSTR pszCreateProc);
	//析构函数
	virtual ~CTempldateHelper();

	//管理函数
public:
	//释放组件
	bool CloseInstance();
	//创建函数
	bool CreateInstance();

	//配置函数
public:
	//创建信息
	VOID SetModuleCreateInfo(LPCTSTR pszModuleDllName, LPCSTR pszCreateProc);

	//辅助函数
public:
	//获取错误
	inline LPCTSTR GetErrorDescribe() const;
	//指针重载
	inline IModeluInterface * operator->() const;
	//获取接口
	inline IModeluInterface * GetInterface() const;
};
```

在我们调用 `m_GameServiceManager.SetModuleCreateInfo(pszGameModule,GAME_SERVICE_CREATE_NAME);` 时，就设置了这个类将要建立的模块名称和名字。（GAME_SERVICE_CREATE_NAME = “CreateGameServiceManager”）

## CServiceUnits::StartService()

这里开始启动我们的服务。

```cpp
bool CServiceUnits::StartService()
{
	//效验状态
	ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//设置状态
	SetServiceStatus(ServiceStatus_Config);

	//创建窗口
	if (m_hWnd==NULL)
	{
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),100);
	}

	//创建模块
	if (CreateServiceDLL()==false)
	{
		ConcludeService();
		return false;
	}

	//调整参数
	if (RectifyServiceParameter()==false)
	{
		ConcludeService();
		return false;
	}

	//配置服务
	if (InitializeService()==false)
	{
		ConcludeService();
		return false;
	}

	//启动内核
	if (StartKernelService()==false)
	{
		ConcludeService();
		return false;
	}		

	//加载配置
	SendControlPacket(CT_LOAD_SERVICE_CONFIG,NULL,0);

	return true;
}

```

可以看到，分为几个步骤：加载 dll， 调整参数，配置服务，启动内核等等。

### CreateServiceDLL

```cpp
bool CServiceUnits::CreateServiceDLL()
{
	//时间引擎
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TimerEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//调度引擎
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_AttemperEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//网络组件
	if ((m_TCPSocketService.GetInterface()==NULL)&&(m_TCPSocketService.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPSocketService.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//网络引擎
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPNetworkEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//数据组件
	if ((m_KernelDataBaseEngine.GetInterface()==NULL)&&(m_KernelDataBaseEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_KernelDataBaseEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//数据组件
	if ((m_RecordDataBaseEngine.GetInterface()==NULL)&&(m_RecordDataBaseEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_RecordDataBaseEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//游戏模块
	if ((m_GameServiceManager.GetInterface()==NULL)&&(m_GameServiceManager.CreateInstance()==false))
	{
		CTraceService::TraceString(m_GameServiceManager.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//比赛模块
	if ((m_GameServiceOption.wServerType&GAME_GENRE_MATCH)!=0)
	{
		if ((m_MatchServiceManager.GetInterface()==NULL)&&(m_MatchServiceManager.CreateInstance()==false))
		{
			CTraceService::TraceString(m_MatchServiceManager.GetErrorDescribe(),TraceLevel_Exception);
			return false;
		}
	}

	return true;
}

```

可以看到，全是一些 helper 类进行获取接口的验证操作。我们简单来看一下 m_GameServiceManager.GetInterface。 前面提到，这个帮助类是在  Module.h 中用模板生成的，所以模板内的方法其都拥有。

1. 调用 m_GameServiceManager.GetInterface 来验证模块是否已加载，如果没有
2. 调用 CreateInstance 建立。

#### CTempldateHelper<IModeluInterface>::CreateInstance

```cpp
bool CTempldateHelper<IModeluInterface>::CreateInstance()
{
	//释放组件
	CloseInstance();

	//创建组件
	try
	{
		//效验参数
		ASSERT(m_szCreateProc[0]!=0);
		ASSERT(m_szModuleDllName[0]!=0);

		//加载模块
		m_hDllInstance=AfxLoadLibrary(m_szModuleDllName);
		if (m_hDllInstance==NULL) 
		{
			_sntprintf(m_szDescribe,CountArray(m_szDescribe),TEXT("“%s”模块加载失败"),m_szModuleDllName);
			return false;
		}

		//寻找函数
		ModuleCreateProc * CreateProc=(ModuleCreateProc *)GetProcAddress(m_hDllInstance,m_szCreateProc);
		if (CreateProc==NULL) 
		{
			_sntprintf(m_szDescribe,CountArray(m_szDescribe),TEXT("找不到组件创建函数“%s”"),m_szCreateProc);
			return false;
		}

		//创建组件
		m_pIModeluInterface=(IModeluInterface *)CreateProc(m_Guid,m_dwVersion);
		if (m_pIModeluInterface==NULL) 
		{
			_sntprintf(m_szDescribe,CountArray(m_szDescribe),TEXT("调用函数“%s”生成对象失败"),m_szCreateProc);
			return false;
		}
	}
	catch (LPCTSTR pszError)
	{
		_sntprintf(m_szDescribe,CountArray(m_szDescribe),TEXT("由于“%s”，组件创建失败"),pszError);
		return false;
	}
	catch (...)	
	{ 
		_sntprintf(m_szDescribe,CountArray(m_szDescribe),TEXT("组件创建函数“%s”产生未知异常错误，组件创建失败"),m_szCreateProc);
		return false;
	}

	return true;
}
```

哈哈，现在找到地方了。在前面的过程中我们说到：

在我们调用 `m_GameServiceManager.SetModuleCreateInfo(pszGameModule,GAME_SERVICE_CREATE_NAME);` 时，就设置了这个类将要建立的模块名称和名字。（`GAME_SERVICE_CREATE_NAME` = “CreateGameServiceManager”）

现在，我们的动态加载了我们指定的  dll 名称，同时给予其一个创建函数，找到这个建立函数的地址，进行调用，获得一个返回地址。我们的组件只需要实现 CreateGameServiceManager 这么一个导出函数就可以被调用了。


### 内核组件


```cpp
public:
	CTimerEngineHelper				m_TimerEngine;						//时间引擎
	CAttemperEngineHelper			m_AttemperEngine;					//调度引擎
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//网络引擎
	CTCPSocketServiceHelper			m_TCPSocketService;					//网络服务
	CGameServiceManagerHelper		m_GameServiceManager;				//游戏模块
	CMatchServiceManagerHelper		m_MatchServiceManager;				//比赛管理
```

几大组件，其中时间、调度、网络、网络服务组件都是呼叫内核 dll 来建立的。

在 KernelEngineHead.h 中有声明：

```cpp
DECLARE_MODULE_HELPER(DataBase,KERNEL_ENGINE_DLL_NAME,"CreateDataBase")
DECLARE_MODULE_HELPER(TimerEngine,KERNEL_ENGINE_DLL_NAME,"CreateTimerEngine")
DECLARE_MODULE_HELPER(DataBaseEngine,KERNEL_ENGINE_DLL_NAME,"CreateDataBaseEngine")
DECLARE_MODULE_HELPER(AttemperEngine,KERNEL_ENGINE_DLL_NAME,"CreateAttemperEngine")
DECLARE_MODULE_HELPER(TCPSocketService,KERNEL_ENGINE_DLL_NAME,"CreateTCPSocketService")
DECLARE_MODULE_HELPER(WEBSocketService,KERNEL_ENGINE_DLL_NAME,"CreateWEBSocketService")
DECLARE_MODULE_HELPER(TCPNetworkEngine,KERNEL_ENGINE_DLL_NAME,"CreateTCPNetworkEngine")
DECLARE_MODULE_HELPER(UDPNetworkEngine,KERNEL_ENGINE_DLL_NAME,"CreateUDPNetworkEngine")
DECLARE_MODULE_HELPER(AsynchronismEngine,KERNEL_ENGINE_DLL_NAME,"CreateAsynchronismEngine")
DECLARE_MODULE_HELPER(TraceServiceManager,KERNEL_ENGINE_DLL_NAME,"CreateTraceServiceManager")
```

宏的定义如下，在根据名字来生成模板类的时候，这个模板类还实现了对应的命名接口。

如 AttemperEngine 生成的辅助类就会实现 IAttemperEngine 接口。

```cpp
#define DECLARE_MODULE_HELPER(OBJECT_NAME,MODULE_DLL_NAME,CREATE_FUNCTION_NAME)								\
class C##OBJECT_NAME##Helper : public CTempldateHelper<I##OBJECT_NAME>										\
{																											\
public:																										\
	C##OBJECT_NAME##Helper() : CTempldateHelper<I##OBJECT_NAME>(IID_I##OBJECT_NAME,							\
		VER_I##OBJECT_NAME,MODULE_DLL_NAME,CREATE_FUNCTION_NAME) { }										\
};

```


这些内核组件的接口，在头文件 *KernelEngineHead.h* 中进行了定义：

```cpp
//调度引擎
interface IAttemperEngine : public IServiceModule
{
	//配置接口
public:
	//网络接口
	virtual bool SetNetworkEngine(IUnknownEx * pIUnknownEx)=NULL;
	//回调接口
	virtual bool SetAttemperEngineSink(IUnknownEx * pIUnknownEx)=NULL;

	//控制事件
public:
	//自定事件
	virtual bool OnEventCustom(WORD wRequestID, VOID * pData, WORD wDataSize)=NULL;
	//控制事件
	virtual bool OnEventControl(WORD wControlID, VOID * pData, WORD wDataSize)=NULL;
};
```
## CServiceUnits::InitializeService()

下一个步骤，是进行各组件的配置工作。


```cpp
bool CServiceUnits::InitializeService()
{
	//设置参数
	m_InitParameter.SetServerParameter(&m_GameServiceAttrib,&m_GameServiceOption);

	//加载参数
	m_InitParameter.LoadInitParameter();

	//配置参数
	m_GameParameter.dwMedalRate=1L;
	m_GameParameter.dwRevenueRate=1L;
	m_GameParameter.dwExchangeRate=100L;

	//连接信息
	LPCTSTR pszDataBaseAddr=m_GameServiceOption.szDataBaseAddr;
	LPCTSTR pszDataBaseName=m_GameServiceOption.szDataBaseName;
	if (LoadDataBaseParameter(pszDataBaseAddr,pszDataBaseName,m_DataBaseParameter)==false) return false;

	//组件接口  
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface(); // 调度引擎模块接口地址
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface(); // 网络引擎模块接口地址
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);  // 调度引擎钩子

	//数据引擎
	IUnknownEx * pIDataBaseEngineRecordSink[CountArray(m_RecordDataBaseSink)];
	IUnknownEx * pIDataBaseEngineKernelSink[CountArray(m_KernelDataBaseSink)];
	for (WORD i=0;i<CountArray(pIDataBaseEngineRecordSink);i++) pIDataBaseEngineRecordSink[i]=QUERY_OBJECT_INTERFACE(m_RecordDataBaseSink[i],IUnknownEx);
	for (WORD i=0;i<CountArray(pIDataBaseEngineKernelSink);i++) pIDataBaseEngineKernelSink[i]=QUERY_OBJECT_INTERFACE(m_KernelDataBaseSink[i],IUnknownEx);

	//绑定接口  
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false; // 设置调度引擎的钩子
	if (m_RecordDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineRecordSink,CountArray(pIDataBaseEngineRecordSink))==false) return false;
	if (m_KernelDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineKernelSink,CountArray(pIDataBaseEngineKernelSink))==false) return false;

	//查询接口
	IGameServiceCustomTime * pIGameServiceSustomTime=QUERY_OBJECT_PTR_INTERFACE(m_GameServiceManager.GetInterface(),IGameServiceCustomTime);

	//时间引擎
	if (pIGameServiceSustomTime!=NULL)
	{
		//单元时间
		DWORD dwTimeCell=dwTimeCell=pIGameServiceSustomTime->GetTimerEngineTimeCell();
		if (dwTimeCell>TIME_CELL) dwTimeCell=TIME_CELL;

		//设置时间
		m_TimerEngine->SetTimeCell(dwTimeCell);
	}

	//内核组件 
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false; //把定时引擎的事件处理指定为调度引擎地址
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false; // 设置调度引擎的网络处理引擎
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false; //网络引擎事件交给调度引擎处理。

	//协调服务
	if (m_TCPSocketService->SetServiceID(NETWORK_CORRESPOND)==false) return false;
	if (m_TCPSocketService->SetTCPSocketEvent(pIAttemperEngine)==false) return false; // 协调服务交给调度引擎处理。

	//数据协调
	m_DBCorrespondManager.InitDBCorrespondManager(m_KernelDataBaseEngine.GetInterface());

	//创建比赛
	if(m_MatchServiceManager.GetInterface()!=NULL)
	{
		if(m_MatchServiceManager->CreateGameMatch(m_GameMatchOption.cbMatchType)==false) return false;
	}

	//调度回调
	m_AttemperEngineSink.m_pInitParameter=&m_InitParameter;
	m_AttemperEngineSink.m_pGameParameter=&m_GameParameter;
	m_AttemperEngineSink.m_pGameMatchOption=&m_GameMatchOption;
	m_AttemperEngineSink.m_pGameServiceAttrib=&m_GameServiceAttrib;
	m_AttemperEngineSink.m_pGameServiceOption=&m_GameServiceOption;	

	//调度回调
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIAttemperEngine=m_AttemperEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketService=m_TCPSocketService.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_pIGameServiceSustomTime=pIGameServiceSustomTime;
	m_AttemperEngineSink.m_pIGameServiceManager=m_GameServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIRecordDataBaseEngine=m_RecordDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIKernelDataBaseEngine=m_KernelDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIMatchServiceManager=m_MatchServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);

	//数据回调
	for (INT i=0;i<CountArray(m_RecordDataBaseSink);i++)
	{
		m_RecordDataBaseSink[i].m_pInitParameter=&m_InitParameter;
		m_RecordDataBaseSink[i].m_pGameParameter=&m_GameParameter;
		m_RecordDataBaseSink[i].m_pDataBaseParameter=&m_DataBaseParameter;
		m_RecordDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_RecordDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_RecordDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_RecordDataBaseSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
	}

	//数据回调
	for (INT i=0;i<CountArray(m_KernelDataBaseSink);i++)
	{
		m_KernelDataBaseSink[i].m_pInitParameter=&m_InitParameter;
		m_KernelDataBaseSink[i].m_pGameParameter=&m_GameParameter;
		m_KernelDataBaseSink[i].m_pDataBaseParameter=&m_DataBaseParameter;
		m_KernelDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_KernelDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_KernelDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_KernelDataBaseSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
		m_KernelDataBaseSink[i].m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);
	}

	//配置网络
	m_TCPNetworkEngine->SetServiceParameter(m_GameServiceOption.wServerPort,m_GameServiceOption.wMaxPlayer,szCompilation);

	return true;
}

```

综上，我们来整理一下这个过程：

1. 配置参数，获取数据库连接。
2. 获取各内核组件的模块接口地址。
3. 获取调度引擎钩子函数地址
4. 将网络事件，数据库事件的处理丢给调度引擎。
5. 调度引擎将具体处理事件的功能转交给钩子函数。

# 总结及总体架构

每个游戏服务器都有一个游戏单元。

游戏单元持有各个组件，包括调度引擎，定时器引擎，数据库引擎，网络引擎，网络服务等，游戏管理服务等。

将所有事件都集中到调度引擎，由调度引擎来进行分发处理。

# 网络服务

在 系统模块/服务器组件/游戏服务器/ServiceUnits.cpp 中，我们启动了一个网络引擎，用来监听端口，提供网络服务。

```cpp
bool CServiceUnits::StartNetworkService()
{
	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}
```

其本质是就简单的获取套接字，监听，绑定，然后开几个线程来进行处理：

```cpp
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

	//建立读写线程
	for (DWORD i=0;i<dwThreadCount;i++)
	{
		CTCPNetworkThreadReadWrite * pNetworkRSThread=new CTCPNetworkThreadReadWrite();
		if (pNetworkRSThread->InitThread(m_hCompletionPort)==false) return false;
		m_SocketRWThreadArray.Add(pNetworkRSThread);
	}

	//建立应答线程
	if (m_SocketAcceptThread.InitThread(m_hCompletionPort,m_hServerSocket,this)==false) return false;

	//启动读写线程
	for (DWORD i = 0; i<dwThreadCount; i++)
	{
		CTCPNetworkThreadReadWrite * pNetworkRSThread=m_SocketRWThreadArray[i];
		ASSERT(pNetworkRSThread!=NULL);
		if (pNetworkRSThread->StartThread()==false) return false;
	}

	//启动检测线程
	m_SocketDetectThread.InitThread(this,m_dwDetectTime);
	if (m_SocketDetectThread.StartThread()==false) return false;

	//启动应答线程
	if (m_SocketAcceptThread.StartThread()==false) return false;

	//设置变量
	m_bService=true;

	return true;
}
```

可以看到，其开了几个线程来处理呢，dwThreadCount 个线程来进行读写，一个检测线程，一个应答线程。其中，应答线程的处理器被设置为 this，也就是网络引擎本身。

那么数据是如何交给我们的游戏逻辑呢？

我们知道，之前我们已经被网络引擎的处理丢给了调度引擎，那么，这个过程是怎么样的呢？

首先，网络引擎监听一个套接字，当其上有连接建立时，会响应连接的建立，之后就在连接建立的套接字上进行数据的读写。

## 连接建立

这个是由应答线程完成的：CTCPNetworkThreadAccept::OnEventThreadRun()。**当有连接到达时，派生出一个新的套接字，同时将套接字绑定到相应的 TCPNetworkItem**。

```cpp
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

```

而数据的读写，则是由读写线程完成。

##  CTCPNetworkThreadReadWrite::OnEventThreadRun()

简单的将数据封装成了收和发两种类型。

```cpp
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

```

获取到数据包后，解析为一个 TCPNetworkItem 进行数据的递交。事实上，在获取一个连接的时候，也把这个连接的处理器进行了指定：

```cpp
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
				//创建对象 ,指定处理器为 this ，也就是网络引擎本身。
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

```

## CTCPNetworkItem::OnRecvCompleted(COverLappedRecv * pOverLappedRecv, DWORD dwThancferred)


此方法，验证数据的完整性，然后交给钩子函数去处理，在上面说到，钩子函数是由网络引擎实现，所以也是由网络引擎进行处理。


```cpp
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

```

## CTCPNetworkEngine::OnEventSocketRead(TCP_Command Command, VOID * pData, WORD wDataSize, CTCPNetworkItem * pTCPNetworkItem)


此方法会将数据根据套接字ID，投递到网络引擎事件处理器。在之前点启动服务的过程中，把这个事件处理器设置成了调度引擎。

```cpp
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

```
## CAttemperEngine::OnEventTCPNetworkRead(DWORD dwSocketID, TCP_Command Command, VOID * pData, WORD wDataSize)

调度函数经过一些判断后，将数据异步进行投递。
```cpp
bool CAttemperEngine::OnEventTCPNetworkRead(DWORD dwSocketID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT((wDataSize+sizeof(NTY_TCPNetworkReadEvent))<=MAX_ASYNCHRONISM_DATA);
	if ((wDataSize+sizeof(NTY_TCPNetworkReadEvent))>MAX_ASYNCHRONISM_DATA) return false;

	//缓冲锁定
	CWHDataLocker ThreadLock(m_CriticalLocker);
	NTY_TCPNetworkReadEvent * pReadEvent=(NTY_TCPNetworkReadEvent *)m_cbBuffer;
	
	//构造数据
	pReadEvent->Command=Command;
	pReadEvent->wDataSize=wDataSize;
	pReadEvent->dwSocketID=dwSocketID;

	//附加数据
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(m_cbBuffer+sizeof(NTY_TCPNetworkReadEvent),pData,wDataSize);
	}

	//投递数据
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_NETWORK_READ,m_cbBuffer,sizeof(NTY_TCPNetworkReadEvent)+wDataSize);
}
```

这些事件又回到了调度引擎本身，由调度引擎有空的时候进行处理。

## CAttemperEngine::OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)

调度引擎最终会将事件进行分类，交由不同的钩子函数进行处理。
```cpp
bool CAttemperEngine::OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(m_pITCPNetworkEngine!=NULL);
	ASSERT(m_pIAttemperEngineSink!=NULL);

	//内核事件
	switch (wIdentifier)
	{
	case EVENT_TIMER:				//时间事件
		{
			//大小断言
			ASSERT(wDataSize==sizeof(NTY_TimerEvent));
			if (wDataSize!=sizeof(NTY_TimerEvent)) return false;

			//处理消息
			NTY_TimerEvent * pTimerEvent=(NTY_TimerEvent *)pData;
			m_pIAttemperEngineSink->OnEventTimer(pTimerEvent->dwTimerID,pTimerEvent->dwBindParameter);

			return true;
		}
	case EVENT_CONTROL:				//控制消息
		{
			//大小断言
			ASSERT(wDataSize>=sizeof(NTY_ControlEvent));
			if (wDataSize<sizeof(NTY_ControlEvent)) return false;

			//处理消息
			NTY_ControlEvent * pControlEvent=(NTY_ControlEvent *)pData;
			m_pIAttemperEngineSink->OnEventControl(pControlEvent->wControlID,pControlEvent+1,wDataSize-sizeof(NTY_ControlEvent));

			return true;
		}
	case EVENT_DATABASE:			//数据库事件
		{
			//大小断言
			ASSERT(wDataSize>=sizeof(NTY_DataBaseEvent));
			if (wDataSize<sizeof(NTY_DataBaseEvent)) return false;

			//处理消息
			NTY_DataBaseEvent * pDataBaseEvent=(NTY_DataBaseEvent *)pData;
			m_pIAttemperEngineSink->OnEventDataBase(pDataBaseEvent->wRequestID,pDataBaseEvent->dwContextID,pDataBaseEvent+1,wDataSize-sizeof(NTY_DataBaseEvent));

			return true;
		}
	case EVENT_TCP_SOCKET_READ:		//读取事件
		{
			//变量定义
			NTY_TCPSocketReadEvent * pReadEvent=(NTY_TCPSocketReadEvent *)pData;

			//大小断言
			ASSERT(wDataSize>=sizeof(NTY_TCPSocketReadEvent));
			ASSERT(wDataSize==(sizeof(NTY_TCPSocketReadEvent)+pReadEvent->wDataSize));

			//大小效验
			if (wDataSize<sizeof(NTY_TCPSocketReadEvent)) return false;
			if (wDataSize!=(sizeof(NTY_TCPSocketReadEvent)+pReadEvent->wDataSize)) return false;

			//处理消息
			m_pIAttemperEngineSink->OnEventTCPSocketRead(pReadEvent->wServiceID,pReadEvent->Command,pReadEvent+1,pReadEvent->wDataSize);

			return true;
		}
	case EVENT_TCP_SOCKET_SHUT:	//关闭事件
		{
			//大小断言
			ASSERT(wDataSize==sizeof(NTY_TCPSocketShutEvent));
			if (wDataSize!=sizeof(NTY_TCPSocketShutEvent)) return false;

			//处理消息
			NTY_TCPSocketShutEvent * pCloseEvent=(NTY_TCPSocketShutEvent *)pData;
			m_pIAttemperEngineSink->OnEventTCPSocketShut(pCloseEvent->wServiceID,pCloseEvent->cbShutReason);

			return true;
		}
	case EVENT_TCP_SOCKET_LINK:	//连接事件
		{
			//大小断言
			ASSERT(wDataSize==sizeof(NTY_TCPSocketLinkEvent));
			if (wDataSize!=sizeof(NTY_TCPSocketLinkEvent)) return false;

			//处理消息
			NTY_TCPSocketLinkEvent * pConnectEvent=(NTY_TCPSocketLinkEvent *)pData;
			m_pIAttemperEngineSink->OnEventTCPSocketLink(pConnectEvent->wServiceID,pConnectEvent->nErrorCode);

			return true;
		}
	case EVENT_TCP_NETWORK_ACCEPT:	//应答事件
		{
			//大小断言
			ASSERT(wDataSize==sizeof(NTY_TCPNetworkAcceptEvent));
			if (wDataSize!=sizeof(NTY_TCPNetworkAcceptEvent)) return false;

			//变量定义
			bool bSuccess=false;
			NTY_TCPNetworkAcceptEvent * pAcceptEvent=(NTY_TCPNetworkAcceptEvent *)pData;

			//处理消息
			try
			{ 
				bSuccess=m_pIAttemperEngineSink->OnEventTCPNetworkBind(pAcceptEvent->dwClientAddr,pAcceptEvent->dwSocketID);
			}
			catch (...)	{ }

			//失败处理
			if (bSuccess==false) m_pITCPNetworkEngine->CloseSocket(pAcceptEvent->dwSocketID);

			return true;
		}
	case EVENT_TCP_NETWORK_READ:	//读取事件
		{
			//效验大小
			NTY_TCPNetworkReadEvent * pReadEvent=(NTY_TCPNetworkReadEvent *)pData;

			//大小断言
			ASSERT(wDataSize>=sizeof(NTY_TCPNetworkReadEvent));
			ASSERT(wDataSize==(sizeof(NTY_TCPNetworkReadEvent)+pReadEvent->wDataSize));

			//大小效验
			if (wDataSize<sizeof(NTY_TCPNetworkReadEvent))
			{
				m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
				return false;
			}

			//大小效验
			if (wDataSize!=(sizeof(NTY_TCPNetworkReadEvent)+pReadEvent->wDataSize))
			{
				m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
				return false;
			}

			//处理消息
			bool bSuccess=false;
			try
			{ 
				bSuccess=m_pIAttemperEngineSink->OnEventTCPNetworkRead(pReadEvent->Command,pReadEvent+1,pReadEvent->wDataSize,pReadEvent->dwSocketID);
			}
			catch (...)	{ }

			//失败处理
			if (bSuccess==false) m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);

			return true;
		}
	case EVENT_TCP_NETWORK_SHUT:	//关闭事件
		{
			//大小断言
			ASSERT(wDataSize==sizeof(NTY_TCPNetworkShutEvent));
			if (wDataSize!=sizeof(NTY_TCPNetworkShutEvent)) return false;

			//处理消息
			NTY_TCPNetworkShutEvent * pCloseEvent=(NTY_TCPNetworkShutEvent *)pData;
			m_pIAttemperEngineSink->OnEventTCPNetworkShut(pCloseEvent->dwClientAddr,pCloseEvent->dwActiveTime,pCloseEvent->dwSocketID);

			return true;
		}
	}

	//其他事件
	return m_pIAttemperEngineSink->OnEventAttemperData(wIdentifier,pData,wDataSize); 
}

```

我们的调度引擎钩子函数是由游戏服务器的 CAttemperEngineSink 实现的。

## CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)

简单的将命令类型进行一下分类，然后调用不同的方法来处理了。
```cpp
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (wDataSize == 866)
	{
		TCHAR szMessage[260]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("会发生异常的消息: Command.wMainCmdID =%d   Command.wSubCmdID = %d"), Command.wMainCmdID, Command.wSubCmdID);
		CTraceService::TraceString(szMessage,TraceLevel_Normal);
	}
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
				//输出信息
				TCHAR szBuffer[128]=TEXT("");
				_sntprintf(szBuffer,CountArray(szBuffer),TEXT("MDM_GF_GAME 游戏命令返回 [ %ld ]"),Command.wSubCmdID);
				//输出信息
				CTraceService::TraceString(szBuffer,TraceLevel_Normal);

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
}


```

如果属于游戏命令，那么就交由游戏逻辑进行处理。

## CAttemperEngineSink::OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)

看代码，其是由每个桌子服务管理器进行处理的。
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

将事件的处理，交给每个桌子

## CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)

桌子就会去调用其钩子函数就行处理。

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

# 调度引擎钩子的初始化

在前面的环节中，当我们在 ServiceUnits::InitailServices 方法中的时候，对调度函数的钩子进行了初始化，传递给他一些加载游戏逻辑的参数。

比如游戏管理器：

```cpp
m_AttemperEngineSink.m_pIGameServiceManager=m_GameServiceManager.GetInterface();
```

获取了子游戏 dll 文件中， GameServiceManager 的模块地址。

在初始化桌子的时候就会用到这个服务组件：

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
	}	

	return true;
}

```

所以在我们的组件中，肯定是实现了  ITableFrameSink 这个接口的。

这样我们就可以把游戏的消息处理，交给我们自定义的接口了。