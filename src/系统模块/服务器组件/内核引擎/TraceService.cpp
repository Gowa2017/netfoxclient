#include "StdAfx.h"
#include "TraceService.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//��ɫ����
#define COLOR_TIME					RGB(10,10,10)						//ʱ����ɫ
#define COLOR_BACK_GROUND			RGB(230,230,230)					//������ɫ

//��Ϣ��ɫ
#define COLOR_INFO					RGB(64,0,0)							//��Ϣ��ɫ
#define COLOR_NORMAL				RGB(125,125,125)					//��ͨ��ɫ
#define COLOR_WARN					RGB(255,128,0)						//������ɫ
#define COLOR_EXCEPTION				RGB(200,0,0)						//�쳣��ɫ
#define COLOR_DEBUG					RGB(0,128,128)						//������ɫ

//�˵�����
#define IDM_CLEAR_ALL				(WM_USER+201)						//ɾ����Ϣ
#define IDM_SELECT_ALL				(WM_USER+202)						//ȫ��ѡ��
#define IDM_COPY_STRING				(WM_USER+203)						//�����ַ�
#define IDM_SAVE_STRING				(WM_USER+205)						//������Ϣ
#define IDM_DELETE_STRING			(WM_USER+204)						//ɾ���ַ�

//��������
#define IDT_TRACE_STRING			1									//׷���¼�
#define WM_TRACE_SERVICE_MESSAGE	(WM_USER+100)						//׷����Ϣ

//////////////////////////////////////////////////////////////////////////

//׷����Ϣ
struct tagTraceInfo
{
	enTraceLevel					TraceLevel;							//�¼��ȼ�
	TCHAR							szSring[512];						//�����Ϣ
};

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTraceServiceControl, CRichEditCtrl)

	//ϵͳ��Ϣ
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()

	//�˵���Ϣ
	ON_COMMAND(IDM_CLEAR_ALL, OnClearAll)
	ON_COMMAND(IDM_SELECT_ALL, OnSelectAll)
	ON_COMMAND(IDM_COPY_STRING, OnCopyString)
	ON_COMMAND(IDM_SAVE_STRING, OnSaveString)
	ON_COMMAND(IDM_DELETE_STRING, OnDeleteString)

	//�Զ���Ϣ
	ON_MESSAGE(WM_TRACE_SERVICE_MESSAGE, OnTraceServiceMessage)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CTraceService::CTraceService()
{
}

//���÷���
bool CTraceService::SetTraceService(IUnknownEx * pIUnknownEx)
{
	return g_TraceServiceManager.SetTraceService(pIUnknownEx);
}

//��ȡ����
VOID * CTraceService::GetTraceService(const IID & Guid, DWORD dwQueryVer)
{
	return g_TraceServiceManager.GetTraceService(Guid,dwQueryVer);
}

//��ȡ����
VOID * CTraceService::GetTraceServiceManager(const IID & Guid, DWORD dwQueryVer)
{
	return g_TraceServiceManager.QueryInterface(Guid,dwQueryVer);
}

//׷��״̬
bool CTraceService::IsEnableTrace(enTraceLevel TraceLevel)
{
	return g_TraceServiceManager.IsEnableTrace(TraceLevel); 
}

//׷�ٿ���
bool CTraceService::EnableTrace(enTraceLevel TraceLevel, bool bEnableTrace)
{
	return g_TraceServiceManager.EnableTrace(TraceLevel,bEnableTrace); 
}

//׷����Ϣ
bool CTraceService::TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	return g_TraceServiceManager.TraceString(pszString,TraceLevel); 
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CTraceServiceControl::CTraceServiceControl()
{
	//���ñ���
	m_lMaxLineCount=1024;
	m_lReserveLineCount=512;
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	return;
}

//��������
CTraceServiceControl::~CTraceServiceControl()
{
}

//�ӿڲ�ѯ
VOID * CTraceServiceControl::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITraceService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITraceService,Guid,dwQueryVer);
	return NULL;
}

//׷����Ϣ
bool CTraceServiceControl::TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	//״̬�ж�
	if (m_hWnd==NULL) return false;

	//��������
	tagTraceInfo TraceInfo;
	ZeroMemory(&TraceInfo,sizeof(TraceInfo));

	//��������
	TraceInfo.TraceLevel=TraceLevel;
	_sntprintf(TraceInfo.szSring,CountArray(TraceInfo.szSring),TEXT("%s\n"),pszString);

	//�������
	CWHDataLocker ThreadLock(m_CriticalSection);
	WORD wDataSize=sizeof(TraceInfo)-sizeof(TraceInfo.szSring)+CountStringBuffer(TraceInfo.szSring);
	m_DataQueue.InsertData(IDT_TRACE_STRING,&TraceInfo,wDataSize);

	//������Ϣ
	PostMessage(WM_TRACE_SERVICE_MESSAGE,TraceLevel,wDataSize);

	return true;
}

//�󶨺���
VOID CTraceServiceControl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//���ÿؼ�
	InitializeService();

	return;
}

//������Ϣ
bool CTraceServiceControl::LoadMessage(LPCTSTR pszFileName)
{
	//Ч�����
	ASSERT(pszFileName!=NULL);
	if (pszFileName==NULL) return false;

	//���ļ�
	CFile File;
	if (File.Open(pszFileName,CFile::modeRead,NULL)==FALSE) return false;

	//��������
	EDITSTREAM EditStream;
	EditStream.pfnCallback=LoadCallBack;
	EditStream.dwCookie=(DWORD)(LONGLONG)(&File);

	//��ȡ�ļ�
    StreamIn(SF_RTF,EditStream);

	return true;
}

//������Ϣ
bool CTraceServiceControl::SaveMessage(LPCTSTR pszFileName)
{
	//��������
	CString strFileName=pszFileName;

	//���ļ�
	if (strFileName.IsEmpty()==true)
	{
		CFileDialog FileDlg(FALSE,TEXT("RTF"),0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,TEXT("��Ϣ�ļ�(*.RTF)|*.RTF||"),this);
		if (FileDlg.DoModal()==IDOK) strFileName=FileDlg.GetPathName();
		else return false;
	}

	//���ļ�
	CFile File;
	if (File.Open(strFileName,CFile::modeWrite|CFile::modeCreate,NULL)==FALSE) return false;

	//��������
	EDITSTREAM EditStream;
	EditStream.pfnCallback=SaveCallBack;
	EditStream.dwCookie=(DWORD)(LONGLONG)(&File);

	//д���ļ�
    StreamOut(SF_RTF,EditStream);

	return true;
}

//���ò���
bool CTraceServiceControl::SetParameter(LONG lMaxLineCount, LONG lReserveLineCount)
{
	//Ч�����
	ASSERT((m_lMaxLineCount==0)||(m_lMaxLineCount>m_lReserveLineCount));
	if ((m_lMaxLineCount!=0)&&(m_lMaxLineCount<=m_lReserveLineCount)) return false;

	//���ñ���
	m_lMaxLineCount=lMaxLineCount;
	m_lReserveLineCount=lReserveLineCount;

	return true;
}

//���÷���
VOID CTraceServiceControl::InitializeService()
{
	//���ÿؼ�
	SetBackgroundColor(FALSE,COLOR_BACK_GROUND);

	//��Ϣ���
	if (CTraceService::SetTraceService(QUERY_ME_INTERFACE(IUnknownEx))==false)
	{
		TraceString(TEXT("��Ϣ����ӿ�����ʧ�ܣ�ϵͳ������Ϣ���޷���ʾ"),TraceLevel_Exception);
	}

	return;
}

//��ȡ��ɫ
COLORREF CTraceServiceControl::GetTraceColor(enTraceLevel TraceLevel)
{
	//������ɫ
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

//�����ִ�
bool CTraceServiceControl::InsertString(LPCTSTR pszString, CHARFORMAT2 & CharFormat)
{
	//������Ϣ
	CHARRANGE CharRange;
	GetSel(CharRange);

	//��������
	LONG lTextLength=GetTextLengthEx(GTL_NUMCHARS);
	
	//������Ϣ
	SetSel(lTextLength,lTextLength);
	SetSelectionCharFormat(CharFormat);
	ReplaceSel(pszString);

	//�����ַ�
	PostMessage(WM_VSCROLL,SB_BOTTOM,0);

	return true;
}

//���ػص�
DWORD CALLBACK CTraceServiceControl::LoadCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	//��ȡ�ļ�
	*pcb=((CFile *)(LONGLONG)(dwCookie))->Read(pbBuff,cb);

	return 0;
}

//����ص�
DWORD CALLBACK CTraceServiceControl::SaveCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	//д���ļ�
	((CFile *)(LONGLONG)(dwCookie))->Write(pbBuff,*pcb);

	return 0;
}

//ɾ����Ϣ
VOID CTraceServiceControl::OnClearAll()
{
	//ɾ����Ϣ
	SetSel(0L,-1L);
	ReplaceSel(NULL);

	return;
}

//ȫ��ѡ��
VOID CTraceServiceControl::OnSelectAll()
{
	//ȫ��ѡ��
	SetSel(0L,-1L);

	return;
}

//�����ַ�
VOID CTraceServiceControl::OnCopyString()
{
	//�����ַ�
	Copy();

	return;
}

//������Ϣ
VOID CTraceServiceControl::OnSaveString()
{
	//������Ϣ
	SaveMessage(NULL);

	return;
}

//ɾ���ַ�
VOID CTraceServiceControl::OnDeleteString()
{
	//ɾ���ַ�
	ReplaceSel(TEXT(""));

	return;
}

//������Ϣ
INT CTraceServiceControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//���ÿؼ�
	InitializeService();

	return 0;
}

//�Ҽ���Ϣ
VOID CTraceServiceControl::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//���ý���
	SetFocus();

	//����˵�
	CMenu FunctionMenu;
	FunctionMenu.CreatePopupMenu();

	//�����˵�
	FunctionMenu.AppendMenu(MF_STRING,IDM_COPY_STRING,TEXT("����(&C)     Ctrl+C"));
	FunctionMenu.AppendMenu(MF_STRING,IDM_SELECT_ALL,TEXT("ȫѡ(&A)     Ctrl+A"));
	FunctionMenu.AppendMenu(MF_STRING,IDM_DELETE_STRING,TEXT("ɾ��(&D)"));
	FunctionMenu.AppendMenu(MF_STRING,IDM_CLEAR_ALL,TEXT("�����Ϣ"));
	FunctionMenu.AppendMenu(MF_SEPARATOR);
	FunctionMenu.AppendMenu(MF_STRING,IDM_SAVE_STRING,TEXT("������Ϣ..."));

	//��ֹ�˵�
	UINT nEnableMask=((GetSelectionType()!=SEL_EMPTY)?0:MF_GRAYED);
	FunctionMenu.EnableMenuItem(IDM_COPY_STRING,MF_BYCOMMAND|nEnableMask);
	FunctionMenu.EnableMenuItem(IDM_DELETE_STRING,MF_BYCOMMAND|nEnableMask);

	//��ʾ�˵�
	ClientToScreen(&Point);
	FunctionMenu.TrackPopupMenu(TPM_RIGHTBUTTON,Point.x,Point.y,this);

	return;
}

//׷����Ϣ
LRESULT CTraceServiceControl::OnTraceServiceMessage(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����
	tagDataHead DataHead;
	CWHDataLocker ThreadLock(m_CriticalSection);
	m_DataQueue.DistillData(DataHead,m_cbBuffer,sizeof(m_cbBuffer));

	//Ч������
	ASSERT(DataHead.wIdentifier==IDT_TRACE_STRING);
	if (DataHead.wIdentifier!=IDT_TRACE_STRING) return 0;

	//����ж�
	INT nLineCount=GetLineCount();
	if ((m_lMaxLineCount>0)&&(nLineCount>=m_lMaxLineCount))
	{
		//�����Ϣ
		ASSERT(m_lMaxLineCount>m_lReserveLineCount);
		SetSel(0,LineIndex(nLineCount-m_lReserveLineCount));
		ReplaceSel(TEXT(""));

		//���ù��
		LONG lTextLength=GetTextLengthEx(GTL_NUMCHARS);
		SetSel(lTextLength,lTextLength);
	}

	//��ȡʱ��
	SYSTEMTIME SystemTime;
	TCHAR szTimeBuffer[64];
	GetLocalTime(&SystemTime);
	_sntprintf(szTimeBuffer,CountArray(szTimeBuffer),TEXT("�� %04d-%02d-%02d %02d:%02d:%02d ��"),
		SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

	//����ʱ��
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat,sizeof(CharFormat));
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_COLOR;
	CharFormat.crTextColor=COLOR_TIME;
	InsertString(szTimeBuffer,CharFormat);

	//������Ϣ
	tagTraceInfo * pTraceInfo=(tagTraceInfo *)m_cbBuffer;
	CharFormat.crTextColor=GetTraceColor(pTraceInfo->TraceLevel);
	InsertString(pTraceInfo->szSring,CharFormat);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
