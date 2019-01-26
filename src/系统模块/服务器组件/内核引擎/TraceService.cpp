#include "StdAfx.h"
#include "TraceService.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//颜色定义
#define COLOR_TIME					RGB(10,10,10)						//时间颜色
#define COLOR_BACK_GROUND			RGB(230,230,230)					//背景颜色

//信息颜色
#define COLOR_INFO					RGB(64,0,0)							//信息颜色
#define COLOR_NORMAL				RGB(125,125,125)					//普通颜色
#define COLOR_WARN					RGB(255,128,0)						//警告颜色
#define COLOR_EXCEPTION				RGB(200,0,0)						//异常颜色
#define COLOR_DEBUG					RGB(0,128,128)						//调试颜色

//菜单命令
#define IDM_CLEAR_ALL				(WM_USER+201)						//删除信息
#define IDM_SELECT_ALL				(WM_USER+202)						//全部选择
#define IDM_COPY_STRING				(WM_USER+203)						//拷贝字符
#define IDM_SAVE_STRING				(WM_USER+205)						//保存信息
#define IDM_DELETE_STRING			(WM_USER+204)						//删除字符

//常量定义
#define IDT_TRACE_STRING			1									//追踪事件
#define WM_TRACE_SERVICE_MESSAGE	(WM_USER+100)						//追踪消息

//////////////////////////////////////////////////////////////////////////

//追踪消息
struct tagTraceInfo
{
	enTraceLevel					TraceLevel;							//事件等级
	TCHAR							szSring[512];						//输出消息
};

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTraceServiceControl, CRichEditCtrl)

	//系统消息
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()

	//菜单消息
	ON_COMMAND(IDM_CLEAR_ALL, OnClearAll)
	ON_COMMAND(IDM_SELECT_ALL, OnSelectAll)
	ON_COMMAND(IDM_COPY_STRING, OnCopyString)
	ON_COMMAND(IDM_SAVE_STRING, OnSaveString)
	ON_COMMAND(IDM_DELETE_STRING, OnDeleteString)

	//自定消息
	ON_MESSAGE(WM_TRACE_SERVICE_MESSAGE, OnTraceServiceMessage)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CTraceService::CTraceService()
{
}

//设置服务
bool CTraceService::SetTraceService(IUnknownEx * pIUnknownEx)
{
	return g_TraceServiceManager.SetTraceService(pIUnknownEx);
}

//获取服务
VOID * CTraceService::GetTraceService(const IID & Guid, DWORD dwQueryVer)
{
	return g_TraceServiceManager.GetTraceService(Guid,dwQueryVer);
}

//获取服务
VOID * CTraceService::GetTraceServiceManager(const IID & Guid, DWORD dwQueryVer)
{
	return g_TraceServiceManager.QueryInterface(Guid,dwQueryVer);
}

//追踪状态
bool CTraceService::IsEnableTrace(enTraceLevel TraceLevel)
{
	return g_TraceServiceManager.IsEnableTrace(TraceLevel); 
}

//追踪控制
bool CTraceService::EnableTrace(enTraceLevel TraceLevel, bool bEnableTrace)
{
	return g_TraceServiceManager.EnableTrace(TraceLevel,bEnableTrace); 
}

//追踪信息
bool CTraceService::TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	return g_TraceServiceManager.TraceString(pszString,TraceLevel); 
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTraceServiceControl::CTraceServiceControl()
{
	//设置变量
	m_lMaxLineCount=1024;
	m_lReserveLineCount=512;
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	return;
}

//析构函数
CTraceServiceControl::~CTraceServiceControl()
{
}

//接口查询
VOID * CTraceServiceControl::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITraceService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITraceService,Guid,dwQueryVer);
	return NULL;
}

//追踪信息
bool CTraceServiceControl::TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	//状态判断
	if (m_hWnd==NULL) return false;

	//变量定义
	tagTraceInfo TraceInfo;
	ZeroMemory(&TraceInfo,sizeof(TraceInfo));

	//构造数据
	TraceInfo.TraceLevel=TraceLevel;
	_sntprintf(TraceInfo.szSring,CountArray(TraceInfo.szSring),TEXT("%s\n"),pszString);

	//插入队列
	CWHDataLocker ThreadLock(m_CriticalSection);
	WORD wDataSize=sizeof(TraceInfo)-sizeof(TraceInfo.szSring)+CountStringBuffer(TraceInfo.szSring);
	m_DataQueue.InsertData(IDT_TRACE_STRING,&TraceInfo,wDataSize);

	//发送消息
	PostMessage(WM_TRACE_SERVICE_MESSAGE,TraceLevel,wDataSize);

	return true;
}

//绑定函数
VOID CTraceServiceControl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//配置控件
	InitializeService();

	return;
}

//加载消息
bool CTraceServiceControl::LoadMessage(LPCTSTR pszFileName)
{
	//效验参数
	ASSERT(pszFileName!=NULL);
	if (pszFileName==NULL) return false;

	//打开文件
	CFile File;
	if (File.Open(pszFileName,CFile::modeRead,NULL)==FALSE) return false;

	//构造数据
	EDITSTREAM EditStream;
	EditStream.pfnCallback=LoadCallBack;
	EditStream.dwCookie=(DWORD)(LONGLONG)(&File);

	//读取文件
    StreamIn(SF_RTF,EditStream);

	return true;
}

//保存消息
bool CTraceServiceControl::SaveMessage(LPCTSTR pszFileName)
{
	//变量定义
	CString strFileName=pszFileName;

	//打开文件
	if (strFileName.IsEmpty()==true)
	{
		CFileDialog FileDlg(FALSE,TEXT("RTF"),0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,TEXT("信息文件(*.RTF)|*.RTF||"),this);
		if (FileDlg.DoModal()==IDOK) strFileName=FileDlg.GetPathName();
		else return false;
	}

	//打开文件
	CFile File;
	if (File.Open(strFileName,CFile::modeWrite|CFile::modeCreate,NULL)==FALSE) return false;

	//构造数据
	EDITSTREAM EditStream;
	EditStream.pfnCallback=SaveCallBack;
	EditStream.dwCookie=(DWORD)(LONGLONG)(&File);

	//写入文件
    StreamOut(SF_RTF,EditStream);

	return true;
}

//设置参数
bool CTraceServiceControl::SetParameter(LONG lMaxLineCount, LONG lReserveLineCount)
{
	//效验参数
	ASSERT((m_lMaxLineCount==0)||(m_lMaxLineCount>m_lReserveLineCount));
	if ((m_lMaxLineCount!=0)&&(m_lMaxLineCount<=m_lReserveLineCount)) return false;

	//设置变量
	m_lMaxLineCount=lMaxLineCount;
	m_lReserveLineCount=lReserveLineCount;

	return true;
}

//配置服务
VOID CTraceServiceControl::InitializeService()
{
	//设置控件
	SetBackgroundColor(FALSE,COLOR_BACK_GROUND);

	//信息组件
	if (CTraceService::SetTraceService(QUERY_ME_INTERFACE(IUnknownEx))==false)
	{
		TraceString(TEXT("信息输出接口设置失败，系统运行信息将无法显示"),TraceLevel_Exception);
	}

	return;
}

//获取颜色
COLORREF CTraceServiceControl::GetTraceColor(enTraceLevel TraceLevel)
{
	//分析颜色
	switch (TraceLevel)
	{
	case TraceLevel_Info:		{ return COLOR_INFO; }
	case TraceLevel_Normal:		{ return COLOR_NORMAL; }
	case TraceLevel_Warning:	{ return COLOR_WARN; }
	case TraceLevel_Exception:	{ return COLOR_EXCEPTION; }
	case TraceLevel_Debug:		{ return COLOR_DEBUG; }
	default: { ASSERT(FALSE); }
	}

	return COLOR_NORMAL;
}

//插入字串
bool CTraceServiceControl::InsertString(LPCTSTR pszString, CHARFORMAT2 & CharFormat)
{
	//保存信息
	CHARRANGE CharRange;
	GetSel(CharRange);

	//变量定义
	LONG lTextLength=GetTextLengthEx(GTL_NUMCHARS);
	
	//插入消息
	SetSel(lTextLength,lTextLength);
	SetSelectionCharFormat(CharFormat);
	ReplaceSel(pszString);

	//滚动字符
	PostMessage(WM_VSCROLL,SB_BOTTOM,0);

	return true;
}

//加载回调
DWORD CALLBACK CTraceServiceControl::LoadCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	//读取文件
	*pcb=((CFile *)(LONGLONG)(dwCookie))->Read(pbBuff,cb);

	return 0;
}

//保存回调
DWORD CALLBACK CTraceServiceControl::SaveCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	//写入文件
	((CFile *)(LONGLONG)(dwCookie))->Write(pbBuff,*pcb);

	return 0;
}

//删除信息
VOID CTraceServiceControl::OnClearAll()
{
	//删除信息
	SetSel(0L,-1L);
	ReplaceSel(NULL);

	return;
}

//全部选择
VOID CTraceServiceControl::OnSelectAll()
{
	//全部选择
	SetSel(0L,-1L);

	return;
}

//拷贝字符
VOID CTraceServiceControl::OnCopyString()
{
	//拷贝字符
	Copy();

	return;
}

//保存信息
VOID CTraceServiceControl::OnSaveString()
{
	//保存信息
	SaveMessage(NULL);

	return;
}

//删除字符
VOID CTraceServiceControl::OnDeleteString()
{
	//删除字符
	ReplaceSel(TEXT(""));

	return;
}

//建立消息
INT CTraceServiceControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//配置控件
	InitializeService();

	return 0;
}

//右键消息
VOID CTraceServiceControl::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//设置焦点
	SetFocus();

	//构造菜单
	CMenu FunctionMenu;
	FunctionMenu.CreatePopupMenu();

	//建立菜单
	FunctionMenu.AppendMenu(MF_STRING,IDM_COPY_STRING,TEXT("复制(&C)     Ctrl+C"));
	FunctionMenu.AppendMenu(MF_STRING,IDM_SELECT_ALL,TEXT("全选(&A)     Ctrl+A"));
	FunctionMenu.AppendMenu(MF_STRING,IDM_DELETE_STRING,TEXT("删除(&D)"));
	FunctionMenu.AppendMenu(MF_STRING,IDM_CLEAR_ALL,TEXT("清除信息"));
	FunctionMenu.AppendMenu(MF_SEPARATOR);
	FunctionMenu.AppendMenu(MF_STRING,IDM_SAVE_STRING,TEXT("保存信息..."));

	//禁止菜单
	UINT nEnableMask=((GetSelectionType()!=SEL_EMPTY)?0:MF_GRAYED);
	FunctionMenu.EnableMenuItem(IDM_COPY_STRING,MF_BYCOMMAND|nEnableMask);
	FunctionMenu.EnableMenuItem(IDM_DELETE_STRING,MF_BYCOMMAND|nEnableMask);

	//显示菜单
	ClientToScreen(&Point);
	FunctionMenu.TrackPopupMenu(TPM_RIGHTBUTTON,Point.x,Point.y,this);

	return;
}

//追踪消息
LRESULT CTraceServiceControl::OnTraceServiceMessage(WPARAM wParam, LPARAM lParam)
{
	//获取数据
	tagDataHead DataHead;
	CWHDataLocker ThreadLock(m_CriticalSection);
	m_DataQueue.DistillData(DataHead,m_cbBuffer,sizeof(m_cbBuffer));

	//效验数据
	ASSERT(DataHead.wIdentifier==IDT_TRACE_STRING);
	if (DataHead.wIdentifier!=IDT_TRACE_STRING) return 0;

	//溢出判断
	INT nLineCount=GetLineCount();
	if ((m_lMaxLineCount>0)&&(nLineCount>=m_lMaxLineCount))
	{
		//清除消息
		ASSERT(m_lMaxLineCount>m_lReserveLineCount);
		SetSel(0,LineIndex(nLineCount-m_lReserveLineCount));
		ReplaceSel(TEXT(""));

		//设置光标
		LONG lTextLength=GetTextLengthEx(GTL_NUMCHARS);
		SetSel(lTextLength,lTextLength);
	}

	//获取时间
	SYSTEMTIME SystemTime;
	TCHAR szTimeBuffer[64];
	GetLocalTime(&SystemTime);
	_sntprintf(szTimeBuffer,CountArray(szTimeBuffer),TEXT("【 %04d-%02d-%02d %02d:%02d:%02d 】"),
		SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

	//插入时间
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat,sizeof(CharFormat));
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_COLOR;
	CharFormat.crTextColor=COLOR_TIME;
	InsertString(szTimeBuffer,CharFormat);

	//插入消息
	tagTraceInfo * pTraceInfo=(tagTraceInfo *)m_cbBuffer;
	CharFormat.crTextColor=GetTraceColor(pTraceInfo->TraceLevel);
	InsertString(pTraceInfo->szSring,CharFormat);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
