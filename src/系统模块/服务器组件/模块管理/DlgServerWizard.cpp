#include "StdAfx.h"
#include "Resource.h"
#include "DlgServerWizard.h"
#include "ModuleDBParameter.h"
#include ".\dlgserverwizard.h"

//////////////////////////////////////////////////////////////////////////////////

//页面定义
#define ITEM_SERVER_OPTION_1		0									//配置选择
#define ITEM_SERVER_OPTION_2		1									//配置选择
#define ITEM_SERVER_OPTION_3		2									//配置选择
#define ITEM_SERVER_OPTION_CUSTOM	4								//定制配置
#define ITEM_SERVER_OPTION_5		3								//配置选择

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerOptionItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerOptionItem1, CDlgServerOptionItem)
	ON_CBN_SELCHANGE(IDC_SERVER_TYPE, OnSelchangeServerType)
	ON_CBN_SELCHANGE(IDC_SERVER_KIND, OnSelchangeServerKind)
	ON_CBN_SELCHANGE(IDC_PESONAL_SQL_TYPE, OnSelchangePersonalSqlType)
	ON_BN_CLICKED(IDC_REVENUE_RADIO, OnBnClickedRevenueRadio)
	ON_BN_CLICKED(IDC_SERVICE_RADIO, OnBnClickedServiceRadio)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItem2, CDlgServerOptionItem)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItem3, CDlgServerOptionItem)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItemCustom, CDlgServerOptionItem)
	ON_WM_SETFOCUS()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerWizardItem1, CDlgServerWizardItem)
	ON_NOTIFY(NM_DBLCLK, IDC_MODULE_LIST, OnNMDblclkModuleList)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardItem2, CDlgServerWizardItem)
	ON_WM_SETFOCUS()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CTRL, OnTcnSelchangeTabCtrl)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerOptionItem5, CDlgServerOptionItem)
	ON_BN_CLICKED(IDC_RADIO_JOIN_GAME, OnBnClickedJoinGameRadio)
	ON_BN_CLICKED(IDC_RADIO_NOT_JOIN_GAME, OnBnClickedNotJoinRadio)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CDlgServerWizard, CDialog)
	ON_BN_CLICKED(IDC_LAST, OnBnClickedLast)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_FINISH, OnBnClickedFinish)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerOptionItem::CDlgServerOptionItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//设置变量
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pPersonalRoomOption = NULL;
	m_pCreateRoomRightAndFee = NULL;
	m_bIsHasPersonalRoom = false;

	HINSTANCE hInstLibrary = NULL;
#ifdef _DEBUG
	hInstLibrary = LoadLibrary(TEXT("PersonalRoomServiceD.dll"));
#else
	hInstLibrary = LoadLibrary(TEXT("PersonalRoomService.dll"));
#endif
	if (hInstLibrary == NULL)
	{
		FreeLibrary(hInstLibrary); 
	}
	else
	{
		m_bIsHasPersonalRoom = true;
	}

	return;
}

//析构函数
CDlgServerOptionItem::~CDlgServerOptionItem()
{
}

//保存数据
bool CDlgServerOptionItem::SaveItemData()
{
	//保存数据
	if ((m_hWnd!=NULL)&&(SaveInputInfo()==false))
	{
		return false;
	}

	return true;
}

//显示配置
bool CDlgServerOptionItem::ShowOptionItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//创建窗口
	if (m_hWnd==NULL) 
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//创建窗口
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//设置资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//显示窗口
	ShowWindow(SW_SHOW);

	return true;
}

//确定函数
VOID CDlgServerOptionItem::OnOK() 
{ 
	//投递消息
	m_pDlgServerWizard->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgServerOptionItem::OnCancel() 
{ 
	//投递消息
	m_pDlgServerWizard->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//位置消息
VOID CDlgServerOptionItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerOptionItem1::CDlgServerOptionItem1() : CDlgServerOptionItem(IDD_SERVER_OPTION_1)
{
}

//析构函数
CDlgServerOptionItem1::~CDlgServerOptionItem1()
{
}

//初始化函数
BOOL CDlgServerOptionItem1::OnInitDialog()
{
	__super::OnInitDialog();

	//构造控件
	InitCtrlWindow();

	//变量定义
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	CComboBox * pServerKind=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);

	//房间名字
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		SetDlgItemText(IDC_SERVER_NAME,m_pGameServiceAttrib->szGameName);
	}
	else SetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName);

	//桌子数目
	if (m_pGameServiceOption->wTableCount!=0)
	{
		SetDlgItemInt(IDC_TABLE_COUNT,m_pGameServiceOption->wTableCount);
	}
	else if (m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		SetDlgItemInt(IDC_TABLE_COUNT,1);
	}
	else 
	{
		SetDlgItemInt(IDC_TABLE_COUNT,60);
	}

	//房间等级
	if (m_pGameServiceOption->wServerLevel>0)
	{
		SetDlgItemInt(IDC_SERVER_LEVEL,m_pGameServiceOption->wServerLevel);
	}

	//服务端口
	if (m_pGameServiceOption->wServerPort!=0)
	{
		SetDlgItemInt(IDC_SERVER_PORT,m_pGameServiceOption->wServerPort);
	}

	//数据库名
	if (m_pGameServiceOption->szDataBaseName[0]!=0)
	{
		SetDlgItemText(IDC_DATABASE_NAME,m_pGameServiceOption->szDataBaseName);
	}
	else SetDlgItemText(IDC_DATABASE_NAME,m_pGameServiceAttrib->szDataBaseName);

	//连接地址
	if (m_pGameServiceOption->szDataBaseAddr[0]!=0)
	{
		DWORD dwDataBaseAddr=inet_addr(CT2CA(m_pGameServiceOption->szDataBaseAddr));
		((CIPAddressCtrl *)GetDlgItem(IDC_DATABASE_ADDR))->SetAddress(htonl(dwDataBaseAddr));
	}

	//游戏类型
	for (INT i=0;i<pServerType->GetCount();i++)
	{
		if (pServerType->GetItemData(i)==m_pGameServiceOption->wServerType)
		{
			pServerType->SetCurSel(i);
		}
	}

	//房间密码
	if (m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)
	{
		GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(true);
		SetDlgItemText(IDC_SERVER_PASSWD,m_pGameServiceOption->szServerPasswd);
	}
	else
	{
		GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(false);
		SetDlgItemText(IDC_SERVER_PASSWD,TEXT(""));
	}

	//房间类型
	for (INT i=0;i<pServerKind->GetCount();i++)
	{
		if (pServerKind->GetItemData(i)==m_pGameServiceOption->wServerKind)
		{
			pServerKind->SetCurSel(i);
		}
	}
	
	TCHAR str[32];
	_sntprintf(str,sizeof(str),TEXT("%.2f"),m_pGameServiceOption->lCellScore);
	//房间配置
	//if (m_pGameServiceOption->lCellScore!=0) SetDlgItemInt(IDC_CELL_SCORE,(LONG)m_pGameServiceOption->lCellScore);
	if (m_pGameServiceOption->lCellScore >= 0.0f )SetDlgItemText(IDC_CELL_SCORE,str);
	if (m_pGameServiceOption->wMaxPlayer!=0) SetDlgItemInt(IDC_MAX_PLAYER,(LONG)m_pGameServiceOption->wMaxPlayer);
	//if (m_pGameServiceOption->lRestrictScore!=0) SetDlgItemInt(IDC_RESTRICT_SCORE,(LONG)m_pGameServiceOption->lRestrictScore);
	if (m_pGameServiceOption->lRestrictScore >= 0.0f ) 
	{
		_sntprintf(str,sizeof(str),TEXT("%.2f"),m_pGameServiceOption->lRestrictScore);
		SetDlgItemText(IDC_RESTRICT_SCORE,str);

	}
	if (m_pGameServiceOption->wRevenueRatio!=0)
	{
		OnBnClickedRevenueRadio();
		SetDlgItemInt(IDC_SERVICE_REVENUE,(LONG)m_pGameServiceOption->wRevenueRatio);
		((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_SERVICE_RADIO))->SetCheck(BST_UNCHECKED);
		
	}
	else if(m_pGameServiceOption->lServiceScore != 0)
	{
		OnBnClickedServiceRadio();
		SetDlgItemInt(IDC_SERVICE_REVENUE,(LONG)m_pGameServiceOption->lServiceScore);
		((CButton*)GetDlgItem(IDC_SERVICE_RADIO))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->SetCheck(BST_UNCHECKED);
	}

	//百人游戏无底分配置
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		GetDlgItem(IDC_CELL_SCORE)->EnableWindow(FALSE);
	}

	//挂接配置
	WORD wGameKindID=m_pGameServiceOption->wKindID;
	WORD wAttribKindID=m_pGameServiceAttrib->wKindID;
	if (m_pGameServiceOption->wNodeID!=0) SetDlgItemInt(IDC_NODE_ID,m_pGameServiceOption->wNodeID);
	if (m_pGameServiceOption->wSortID!=0) SetDlgItemInt(IDC_SORT_ID,m_pGameServiceOption->wSortID);
	if ((wGameKindID!=0)&&(wGameKindID!=wAttribKindID))	SetDlgItemInt(IDC_KIND_ID,m_pGameServiceOption->wKindID);

	//默认选择
	if (pServerType->GetCurSel()==LB_ERR)
	{
		//获取名字
		TCHAR szDataBase[32]=TEXT("");
		GetDlgItemText(IDC_DATABASE_NAME,szDataBase,CountArray(szDataBase));

		//变量定义
		WORD wServerType=0;
		bool bGoldDataBase=(lstrcmpi(szDataBase,szTreasureDB)==0);
		bool bDefaultDataBase=(lstrcmpi(szDataBase,m_pGameServiceAttrib->szDataBaseName)==0);

		//类型定义
		if ((wServerType==0)&&(bGoldDataBase==true)) wServerType=GAME_GENRE_GOLD;
		if ((wServerType==0)&&(bGoldDataBase==false)&&(bDefaultDataBase==true)) wServerType=GAME_GENRE_SCORE;

		//分析类型
		if ((m_pGameServiceAttrib->wSupporType&wServerType)!=0L)
		{
			for (INT i=0;i<pServerType->GetCount();i++)
			{
				if (pServerType->GetItemData(i)==wServerType)
				{
					pServerType->SetCurSel(i);
					break;
				}
			}
		}
	}

	//约战房数据库类型
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_PESONAL_SQL_TYPE);

	if(m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		if(pComboBox)
		{
			pComboBox->EnableWindow(TRUE);
		}
	}
	else
	{
		if(pComboBox)
		{
			pComboBox->EnableWindow(FALSE);
		}
	}


	return TRUE;
}

//保存输入
bool CDlgServerOptionItem1::SaveInputInfo()
{
	//变量定义
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	CComboBox * pServerKind=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);
	CIPAddressCtrl * pIPAddressCtrl=(CIPAddressCtrl *)GetDlgItem(IDC_DATABASE_ADDR);

	//挂接属性
	m_pGameServiceOption->wKindID=GetDlgItemInt(IDC_KIND_ID);
	m_pGameServiceOption->wNodeID=GetDlgItemInt(IDC_NODE_ID);
	m_pGameServiceOption->wSortID=GetDlgItemInt(IDC_SORT_ID);

	//属性配置
	m_pGameServiceOption->wMaxPlayer=GetDlgItemInt(IDC_MAX_PLAYER);

	CString str;
	GetDlgItemText(IDC_CELL_SCORE,str);

	int nLength = str.GetLength();
	int nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
	char* VoicePath = new char[ nBytes + 1];
	memset(VoicePath,0,nLength + 1);
	WideCharToMultiByte(CP_OEMCP, 0, str, nLength, VoicePath, nBytes, NULL, NULL); 
	VoicePath[nBytes] = 0; 

	m_pGameServiceOption->lCellScore = (SCORE)atof(VoicePath);
	delete VoicePath;
	VoicePath = NULL;

	GetDlgItemText(IDC_RESTRICT_SCORE,str);

	nLength = str.GetLength();
	nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
	VoicePath = new char[ nBytes + 1];
	memset(VoicePath,0,nLength + 1);
	WideCharToMultiByte(CP_OEMCP, 0, str, nLength, VoicePath, nBytes, NULL, NULL); 
	VoicePath[nBytes] = 0; 

	m_pGameServiceOption->lRestrictScore = (SCORE)atof(VoicePath);
	delete VoicePath;
	VoicePath = NULL;

	//m_pGameServiceOption->lCellScore=GetDlgItemInt(IDC_CELL_SCORE);
	m_pGameServiceOption->wServerLevel=GetDlgItemInt(IDC_SERVER_LEVEL);
	
	
	//m_pGameServiceOption->lRestrictScore=GetDlgItemInt(IDC_RESTRICT_SCORE);
	if(((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->wRevenueRatio=GetDlgItemInt(IDC_SERVICE_REVENUE);
		m_pGameServiceOption->lServiceScore=0L;
	}
	else
	{
		m_pGameServiceOption->lServiceScore=GetDlgItemInt(IDC_SERVICE_REVENUE);
		m_pGameServiceOption->wRevenueRatio=0L;
	}

	//选择控件
	m_pGameServiceOption->wServerType=(WORD)pServerType->GetItemData(pServerType->GetCurSel());
	m_pGameServiceOption->wServerKind=(WORD)pServerKind->GetItemData(pServerKind->GetCurSel());

	//房间属性
	m_pGameServiceOption->wTableCount=GetDlgItemInt(IDC_TABLE_COUNT);
	m_pGameServiceOption->wServerPort=GetDlgItemInt(IDC_SERVER_PORT);
	GetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName,CountArray(m_pGameServiceOption->szServerName));
	GetDlgItemText(IDC_DATABASE_NAME,m_pGameServiceOption->szDataBaseName,CountArray(m_pGameServiceOption->szDataBaseName));
	GetDlgItemText(IDC_SERVER_PASSWD,m_pGameServiceOption->szServerPasswd,CountArray(m_pGameServiceOption->szServerPasswd));

	//连接地址
	DWORD dwDataBaseAddr=INADDR_NONE;
	pIPAddressCtrl->GetAddress(dwDataBaseAddr);
	_sntprintf(m_pGameServiceOption->szDataBaseAddr,CountArray(m_pGameServiceOption->szDataBaseAddr),TEXT("%d.%d.%d.%d"),
		*(((BYTE *)&dwDataBaseAddr)+3),*(((BYTE *)&dwDataBaseAddr)+2),*(((BYTE *)&dwDataBaseAddr)+1),*((BYTE *)&dwDataBaseAddr));

	//房间名字
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		AfxMessageBox(TEXT("游戏房间名字不能为空，请输入游戏房间名字"),MB_ICONERROR);
		return false;
	}

	//类别判断
	if((m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)!=0 && (m_pGameServiceOption->szServerPasswd[0]==0))
	{
		AfxMessageBox(TEXT("游戏房间密码不能为空，请输入游戏房间密码"),MB_ICONERROR);
		return false;
	}

	//类型判断
	if (m_pGameServiceOption->wServerType==0)
	{
		AfxMessageBox(TEXT("游戏房间类型不能为空，请重新选择游戏房间类型"),MB_ICONERROR);
		return false;
	}

	//数据库名
	if (m_pGameServiceOption->szDataBaseName[0]==0)
	{
		AfxMessageBox(TEXT("游戏数据库名不能为空，请输入游戏数据库名"),MB_ICONERROR);
		return false;
	}

	//连接地址
	if (INVALID_IP_ADDRESS(dwDataBaseAddr))
	{
		AfxMessageBox(TEXT("游戏数据库地址格式不正确，请重新输入游戏数据库地址"),MB_ICONERROR);
		return false;
	}

	//桌子数目
	if ((m_pGameServiceOption->wTableCount==0)||(m_pGameServiceOption->wTableCount>MAX_TABLE))
	{
		AfxMessageBox(TEXT("游戏桌子数目无效，请重新输入游戏桌子数目，有效数值范围为 1 - 512"),MB_ICONERROR);
		return false;
	}

	//如果是约战房 默认勾选即时写分，支持pc
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,(TRUE));
		CServerRule::SetSuportMobile(m_pGameServiceOption->dwServerRule, (TRUE));
		CServerRule::SetSuportPC(m_pGameServiceOption->dwServerRule, (FALSE));
	}	
	else
	{
		//默认支持pc 和 手游，如果是约战房（又叫做私人房）则不支持pc（在保存时会改变）
		CServerRule::SetSuportMobile(m_pGameServiceOption->dwServerRule, (TRUE));
		CServerRule::SetSuportPC(m_pGameServiceOption->dwServerRule, (TRUE));
	}

	return true;
}

//调整控件
VOID CDlgServerOptionItem1::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//构造控件
VOID CDlgServerOptionItem1::InitCtrlWindow()
{
	//列表配置
	((CEdit *)GetDlgItem(IDC_KIND_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_NODE_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SORT_ID))->LimitText(5);

	//属性配置
	((CEdit *)GetDlgItem(IDC_MAX_PLAYER))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_CELL_SCORE))->LimitText(6);
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->LimitText(3);
	((CButton*)GetDlgItem(IDC_REVENUE_RADIO))->SetCheck(BST_CHECKED);

	//常规配置
	((CEdit *)GetDlgItem(IDC_TABLE_COUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_SERVER_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SERVER_NAME))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_DATABASE_NAME))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_SERVER_LEVEL))->LimitText(4);

	//变量定义
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	WORD wServerType[5]={GAME_GENRE_GOLD,GAME_GENRE_SCORE,GAME_GENRE_MATCH,GAME_GENRE_EDUCATE,GAME_GENRE_PERSONAL};
	LPCTSTR pszServerType[5]={TEXT("财富类型"),TEXT("点值类型"),TEXT("比赛类型"),TEXT("练习类型"),TEXT("约战类型")};

	//如果是约战房
	if (m_bIsHasPersonalRoom)
	{
		//构造类型
		for (INT i=0;i<CountArray(wServerType);i++)
		{
			if ((m_pGameServiceAttrib->wSupporType&wServerType[i])==0) continue;
			pComboBox->SetItemData(pComboBox->AddString(pszServerType[i]),wServerType[i]);
		}
	}
	else
	{
		//构造类型
		for (INT i=0;i<CountArray(wServerType) - 1;i++)
		{
			if ((m_pGameServiceAttrib->wSupporType&wServerType[i])==0) continue;
			pComboBox->SetItemData(pComboBox->AddString(pszServerType[i]),wServerType[i]);
		}
	}

	//变量定义
	pComboBox=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);
	WORD wServerKind[2]={SERVER_GENRE_NORMAL,SERVER_GENRE_PASSWD};
	LPCTSTR pszServerKind[2]={TEXT("普通房间"),TEXT("密码房间")};

	//构造类型
	for (INT i=0;i<CountArray(wServerKind);i++)
	{
		pComboBox->SetItemData(pComboBox->InsertString(i,pszServerKind[i]),wServerKind[i]);
	}
	if(pComboBox->GetCurSel()==LB_ERR) pComboBox->SetCurSel(0);

	////约战房数据库类型
	pComboBox=(CComboBox *)GetDlgItem(IDC_PESONAL_SQL_TYPE);
	LPCTSTR pszPersonalSqlType[2]={TEXT("金币库"),TEXT("积分库")};

	if (!m_bIsHasPersonalRoom)
	{
		OutputDebugString(TEXT("ptdt *** 数据库选项："));
		SetDlgItemText(IDC_STATIC_SQL_OPTION, TEXT("数据库选项："));
		//((CStatic *)GetDlgItem(IDC_STATIC))->SetWindowText(TEXT("数据库选项"));
	}
	else
	{
		((CStatic *)GetDlgItem(IDC_STATIC_SQL_OPTION))->SetWindowText(TEXT("约战类型数据库选项："));
	}

	////构造类型
	for (INT i=0; i<2; i++)
	{
		pComboBox->SetItemData(pComboBox->InsertString(i, pszPersonalSqlType[i]),i);
	}

	return;
}

//更新数据
VOID CDlgServerOptionItem1::UpdateDataBaseName()
{
	//变量定义
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);

	//是否私人房数据类型
	CComboBox * pPersonalSqlType=(CComboBox *)GetDlgItem(IDC_PESONAL_SQL_TYPE);
	if(pPersonalSqlType)
	{
		pPersonalSqlType->EnableWindow(FALSE);
	}

	//数据调整
	switch ((WORD)pServerType->GetItemData(pServerType->GetCurSel()))
	{
	case GAME_GENRE_GOLD:	//财富类型
		{
			SetDlgItemText(IDC_DATABASE_NAME,szTreasureDB);
			break;
		}
	case GAME_GENRE_SCORE:	//点值类型
		{
			SetDlgItemText(IDC_DATABASE_NAME,m_pGameServiceAttrib->szDataBaseName);
			break;
		}
	case GAME_GENRE_MATCH:	//比赛类型
		{
			SetDlgItemText(IDC_DATABASE_NAME,szGameMatchDB);
			break;
		}
	case GAME_GENRE_PERSONAL://约战类型
		{
			if(pPersonalSqlType)
			{
				pPersonalSqlType->EnableWindow(TRUE);
			}
		}
	default:
		{
			//获取名字
			TCHAR szDataBase[32]=TEXT("");
			GetDlgItemText(IDC_DATABASE_NAME,szDataBase,CountArray(szDataBase));

			//名字判断
			if (lstrcmpi(szDataBase,szTreasureDB)==0) SetDlgItemText(IDC_DATABASE_NAME,TEXT(""));
			if (lstrcmpi(szDataBase,m_pGameServiceAttrib->szDataBaseName)==0) SetDlgItemText(IDC_DATABASE_NAME,TEXT(""));

			break;
		}
	}

	return;
}

//选择改变
VOID CDlgServerOptionItem1::OnSelchangeServerType()
{
	//更新数据
	UpdateDataBaseName();

	return;
}

//选择改变
VOID CDlgServerOptionItem1::OnSelchangeServerKind()
{
	//变量定义
	CComboBox * pServerKind=(CComboBox *)GetDlgItem(IDC_SERVER_KIND);

	//数据调整
	int nCurSelIndex = pServerKind->GetCurSel();
	switch (pServerKind->GetItemData(nCurSelIndex))
	{
	case SERVER_GENRE_NORMAL:	//普通房间
		{
			GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(false);
			break;
		}
	case SERVER_GENRE_PASSWD:	//密码类型
		{
			GetDlgItem(IDC_SERVER_PASSWD)->EnableWindow(true);
			break;
		}
	}

	return;
}

//约战选择改变
VOID CDlgServerOptionItem1::OnSelchangePersonalSqlType()
{
	//变量定义
	CComboBox * pPersonalSqlType=(CComboBox *)GetDlgItem(IDC_PESONAL_SQL_TYPE);

	//数据调整
	int nCurSelIndex = pPersonalSqlType->GetCurSel();
	switch (pPersonalSqlType->GetItemData(nCurSelIndex))
	{
	case 0:	//普通房间
		{
			SetDlgItemText(IDC_DATABASE_NAME,szTreasureDB);
			break;
		}
	case 1:	//密码类型
		{
			SetDlgItemText(IDC_DATABASE_NAME,szGameScoreDB);
			break;
		}
	}

	return;
}
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerOptionItem2::CDlgServerOptionItem2() : CDlgServerOptionItem(IDD_SERVER_OPTION_2)
{
}

//析构函数
CDlgServerOptionItem2::~CDlgServerOptionItem2()
{
}

//初始化函数
BOOL CDlgServerOptionItem2::OnInitDialog()
{
	__super::OnInitDialog();

	//构造控件
	InitCtrlWindow();

	//变量定义
	CComboBox * pMinMember=(CComboBox *)GetDlgItem(IDC_MIN_ENTER_MEMBER);
	CComboBox * pMaxMember=(CComboBox *)GetDlgItem(IDC_MAX_ENTER_MEMBER);

	//房间配置
	//if (m_pGameServiceOption->lMinEnterScore!=0) SetDlgItemInt(IDC_MIN_ENTER_SCORE,(LONG)m_pGameServiceOption->lMinEnterScore);
	//if (m_pGameServiceOption->lMaxEnterScore!=0) SetDlgItemInt(IDC_MAX_ENTER_SCORE,(LONG)m_pGameServiceOption->lMaxEnterScore);
	//if (m_pGameServiceOption->lMinTableScore!=0) SetDlgItemInt(IDC_MIN_TABLE_SCORE,(LONG)m_pGameServiceOption->lMinTableScore);
	TCHAR str[32];

	//房间配置
	if (m_pGameServiceOption->lMinEnterScore>=0.0f) 
	{
		_sntprintf(str,sizeof(str),TEXT("%.2f"),m_pGameServiceOption->lMinEnterScore);
		SetDlgItemText(IDC_MIN_ENTER_SCORE,str);
	}
	if (m_pGameServiceOption->lMaxEnterScore>=0.0f) 
	{
		_sntprintf(str,sizeof(str),TEXT("%.2f"),m_pGameServiceOption->lMaxEnterScore);
		SetDlgItemText(IDC_MAX_ENTER_SCORE,str);
	}
	if (m_pGameServiceOption->lMinTableScore>=0.0f ) 
	{
		_sntprintf(str,sizeof(str),TEXT("%.2f"),m_pGameServiceOption->lMinTableScore);
		SetDlgItemText(IDC_MIN_TABLE_SCORE,str);
	}

	//会员限制
	//if (m_pGameServiceOption->cbMaxEnterMember!=0)
	{
		//最低会员
		for (INT i=0;i<pMinMember->GetCount();i++)
		{
			if (pMinMember->GetItemData(i)==m_pGameServiceOption->cbMinEnterMember)
			{
				pMinMember->SetCurSel(i);
				break;
			}
		}

		//最高会员
		for (INT i=0;i<pMaxMember->GetCount();i++)
		{
			if (pMaxMember->GetItemData(i)==m_pGameServiceOption->cbMaxEnterMember)
			{
				pMaxMember->SetCurSel(i);
				break;
			}
		}
	}

	//禁止公聊
	bool bForfendGameChat=CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_CHAT))->SetCheck((bForfendGameChat==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//禁止公聊
	bool bForfendRoomChat=CServerRule::IsForfendRoomChat(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_ROOM_CHAT))->SetCheck((bForfendRoomChat==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//禁止私聊
	bool bForfendWisperChat=CServerRule::IsForfendWisperChat(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_WISPER_CHAT))->SetCheck((bForfendWisperChat==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//禁止私聊
	bool bForfendWisperOnGame=CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_WISPER_ON_GAME))->SetCheck((bForfendWisperOnGame==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//禁止进入
	bool bForfendRoomEnter=CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_ROOM_ENTER))->SetCheck((bForfendRoomEnter==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//禁止进入
	bool bForfendGameEnter=CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_ENTER))->SetCheck((bForfendGameEnter==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//禁止旁观
	bool bForfendGameLookon=CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_LOOKON))->SetCheck((bForfendGameLookon==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//禁止规则
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR) CServerRule::SetForfendGameRule(m_pGameServiceOption->dwServerRule, true);
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR) ((CButton *)GetDlgItem(IDC_FORFEND_GAME_RULE))->EnableWindow(FALSE);
	bool bForfendGameRule=CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_GAME_RULE))->SetCheck((bForfendGameRule==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//禁止锁桌
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR) CServerRule::SetForfendLockTable(m_pGameServiceOption->dwServerRule, true);
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR) ((CButton *)GetDlgItem(IDC_FORFEND_LOCK_TABLE))->EnableWindow(FALSE);
	bool bForfendLockTable=CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_LOCK_TABLE))->SetCheck((bForfendLockTable==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//记录积分
	bool bRecordGameScore=CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_RECORD_GAME_SCORE))->SetCheck((bRecordGameScore==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//记录过程
	//bool bRecordGameTrack=CServerRule::IsRecordGameTrack(m_pGameServiceOption->dwServerRule);
	//((CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK))->SetCheck((bRecordGameTrack==TRUE)?BST_CHECKED:BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK))->SetCheck(BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK))->EnableWindow(FALSE);

	//所有财富游戏，每局即时写分，记录每局成绩，默认为直接勾选灰色状态
	if(m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
	{
		CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,true);
		GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE)->EnableWindow(FALSE);
	}

	//即时写分
	bool bImmediateWriteScore=CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE))->SetCheck((bImmediateWriteScore==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//动态底分
	//bool bDynamicCellScore=CServerRule::IsDynamicCellScore(m_pGameServiceOption->dwServerRule);
	//((CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE))->SetCheck((bDynamicCellScore==TRUE)?BST_CHECKED:BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE))->SetCheck(BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE))->EnableWindow(FALSE);

	//隐藏信息
	bool bAllowAvertCheatMode=CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_ALLOW_AVERT_CHEAT_MODE))->SetCheck((bAllowAvertCheatMode==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//禁止存钱
	bool bForfendSaveInRoom=CServerRule::IsForfendSaveInRoom(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_SAVE_IN_ROOM))->SetCheck((bForfendSaveInRoom==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//禁止存钱
	bool bForfendSaveInGame=CServerRule::IsForfendSaveInGame(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_FORFEND_SAVE_IN_GAME))->SetCheck((bForfendSaveInGame==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//允许占位
	bool bAllowAndroidSimulate=CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);
	((CButton *)GetDlgItem(IDC_ALLOW_ANDROID_SIMULATE))->SetCheck((bAllowAndroidSimulate==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//手机支持
	bool bSuportMobile = CServerRule::IsSuportMobile(m_pGameServiceOption->dwServerRule);
	((CButton*)GetDlgItem(IDC_SERVER_SUPORT_MOBILE))->SetCheck((bSuportMobile == TRUE) ? BST_CHECKED : BST_UNCHECKED);

	//pc支持
	bool bSuportPC = CServerRule::IsSuportPC(m_pGameServiceOption->dwServerRule);
	((CButton*)GetDlgItem(IDC_SERVER_SUPORT_PC))->SetCheck((bSuportPC == TRUE) ? BST_CHECKED : BST_UNCHECKED);

	//如果是约战房 不允许支持pc
	if (m_pGameServiceOption->wServerType == GAME_GENRE_PERSONAL)
	{
		((CButton*)GetDlgItem(IDC_SERVER_SUPORT_PC))->SetCheck(false);
		((CButton*)GetDlgItem(IDC_SERVER_SUPORT_PC))->EnableWindow(false);
		((CButton*)GetDlgItem(IDC_SERVER_SUPORT_MOBILE))->SetCheck(true);
		((CButton*)GetDlgItem(IDC_SERVER_SUPORT_MOBILE))->EnableWindow(false);
	}
	else
	{
		//非约战房，如果支持pc和支持收机都没勾选，则默认为都勾选
		if(!bSuportMobile && !bSuportPC)
		{
			((CButton*)GetDlgItem(IDC_SERVER_SUPORT_PC))->SetCheck(true);
			((CButton*)GetDlgItem(IDC_SERVER_SUPORT_MOBILE))->SetCheck(true);
		}
	}

	//动态加入
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE)
	{
		bool bAllowDynamicJoin=CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule);
		((CButton *)GetDlgItem(IDC_ALLOW_DYNAMIC_JOIN))->SetCheck((bAllowDynamicJoin==TRUE)?BST_CHECKED:BST_UNCHECKED);
	}

	//允许机器
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE)
	{
		bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
		((CButton *)GetDlgItem(IDC_ALLOW_ANDROID_ATTEND))->SetCheck((bAllowAndroidAttend==TRUE)?BST_CHECKED:BST_UNCHECKED);
	}

	//断线代打
	if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE)
	{
		bool bAllowOffLineTrustee=CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule);
		((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->SetCheck((bAllowOffLineTrustee==TRUE)?BST_CHECKED:BST_UNCHECKED);
	}
	//((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->SetCheck(BST_UNCHECKED);
	//((CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE))->EnableWindow(FALSE);

	return TRUE;
}

//保存输入
bool CDlgServerOptionItem2::SaveInputInfo()
{
	//变量定义
	CComboBox * pMinMember=(CComboBox *)GetDlgItem(IDC_MIN_ENTER_MEMBER);
	CComboBox * pMaxMember=(CComboBox *)GetDlgItem(IDC_MAX_ENTER_MEMBER);

	//积分限制
	m_pGameServiceOption->cbMinEnterMember=0;
	m_pGameServiceOption->cbMaxEnterMember=0;
	
	CString str;
	GetDlgItemText(IDC_MIN_ENTER_SCORE,str);

	int nLength = str.GetLength();
	int nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
	char* VoicePath = new char[ nBytes + 1];
	memset(VoicePath,0,nLength + 1);
	WideCharToMultiByte(CP_OEMCP, 0, str, nLength, VoicePath, nBytes, NULL, NULL); 
	VoicePath[nBytes] = 0; 

	m_pGameServiceOption->lMinEnterScore = (SCORE)atof(VoicePath);
	delete VoicePath;
	VoicePath = NULL;

	GetDlgItemText(IDC_MAX_ENTER_SCORE,str);

	nLength = str.GetLength();
	nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
	VoicePath = new char[ nBytes + 1];
	memset(VoicePath,0,nLength + 1);
	WideCharToMultiByte(CP_OEMCP, 0, str, nLength, VoicePath, nBytes, NULL, NULL); 
	VoicePath[nBytes] = 0; 

	m_pGameServiceOption->lMaxEnterScore= (SCORE)atof(VoicePath);
	delete VoicePath;
	VoicePath = NULL;

	GetDlgItemText(IDC_MIN_TABLE_SCORE,str);

	nLength = str.GetLength();
	nBytes = WideCharToMultiByte(CP_ACP,0,str,nLength,NULL,0,NULL,NULL);
	VoicePath = new char[ nBytes + 1];
	memset(VoicePath,0,nLength + 1);
	WideCharToMultiByte(CP_OEMCP, 0, str, nLength, VoicePath, nBytes, NULL, NULL); 
	VoicePath[nBytes] = 0; 

	m_pGameServiceOption->lMinTableScore= (SCORE)atof(VoicePath);
	delete VoicePath;
	VoicePath = NULL;

	//m_pGameServiceOption->lMinEnterScore=GetDlgItemInt(IDC_MIN_ENTER_SCORE);
	//m_pGameServiceOption->lMaxEnterScore=GetDlgItemInt(IDC_MAX_ENTER_SCORE);
	//m_pGameServiceOption->lMinTableScore=GetDlgItemInt(IDC_MIN_TABLE_SCORE);

	//会员限制
	if (pMinMember->GetCurSel()!=LB_ERR)
	{
		INT nCurSelect=pMinMember->GetCurSel();
		m_pGameServiceOption->cbMinEnterMember=(BYTE)pMinMember->GetItemData(nCurSelect);
	}

	//最高会员
	if (pMaxMember->GetCurSel()!=LB_ERR)
	{
		INT nCurSelect=pMaxMember->GetCurSel();
		m_pGameServiceOption->cbMaxEnterMember=(BYTE)pMaxMember->GetItemData(nCurSelect);
	}

	//参数效验
	if ((m_pGameServiceOption->lMaxEnterScore!=0L)&&(m_pGameServiceOption->lMaxEnterScore<m_pGameServiceOption->lMinEnterScore))
	{
		AfxMessageBox(TEXT("进入房间最低成绩比最高成绩还高，将会造成任何玩家都不能进入"),MB_ICONERROR);
		return false;
	}

	//参数效验
	if ((m_pGameServiceOption->cbMaxEnterMember!=0L)&&(m_pGameServiceOption->cbMaxEnterMember<m_pGameServiceOption->cbMinEnterMember))
	{
		AfxMessageBox(TEXT("进入房间最低会员级别比最高会员级别还高，将会造成任何玩家都不能进入"),MB_ICONERROR);
		return false;
	}

	//禁止公聊
	CButton * pForfendGameChat=(CButton *)GetDlgItem(IDC_FORFEND_GAME_CHAT);
	CServerRule::SetForfendGameChat(m_pGameServiceOption->dwServerRule,(pForfendGameChat->GetCheck()==BST_CHECKED));

	//禁止公聊
	CButton * pForfendRoomChat=(CButton *)GetDlgItem(IDC_FORFEND_ROOM_CHAT);
	CServerRule::SetForfendRoomChat(m_pGameServiceOption->dwServerRule,(pForfendRoomChat->GetCheck()==BST_CHECKED));

	//禁止私聊
	CButton * pForfendWisperChat=(CButton *)GetDlgItem(IDC_FORFEND_WISPER_CHAT);
	CServerRule::SetForfendWisperChat(m_pGameServiceOption->dwServerRule,(pForfendWisperChat->GetCheck()==BST_CHECKED));

	//禁止私聊
	CButton * pForfendWisperOnGame=(CButton *)GetDlgItem(IDC_FORFEND_WISPER_ON_GAME);
	CServerRule::SetForfendWisperOnGame(m_pGameServiceOption->dwServerRule,(pForfendWisperOnGame->GetCheck()==BST_CHECKED));

	//禁止进入
	CButton * pForfendRoomEnter=(CButton *)GetDlgItem(IDC_FORFEND_ROOM_ENTER);
	CServerRule::SetForfendRoomEnter(m_pGameServiceOption->dwServerRule,(pForfendRoomEnter->GetCheck()==BST_CHECKED));

	//禁止进入
	CButton * pForfendGameEnter=(CButton *)GetDlgItem(IDC_FORFEND_GAME_ENTER);
	CServerRule::SetForfendGameEnter(m_pGameServiceOption->dwServerRule,(pForfendGameEnter->GetCheck()==BST_CHECKED));

	//禁止旁观
	CButton * pForfendGameLookon=(CButton *)GetDlgItem(IDC_FORFEND_GAME_LOOKON);
	CServerRule::SetForfendGameLookon(m_pGameServiceOption->dwServerRule,(pForfendGameLookon->GetCheck()==BST_CHECKED));

	//禁止规则
	CButton * pForfendGameRule=(CButton *)GetDlgItem(IDC_FORFEND_GAME_RULE);
	CServerRule::SetForfendGameRule(m_pGameServiceOption->dwServerRule,(pForfendGameRule->GetCheck()==BST_CHECKED));

	//禁止锁桌
	CButton * pForfendLockTable=(CButton *)GetDlgItem(IDC_FORFEND_LOCK_TABLE);
	CServerRule::SetForfendLockTable(m_pGameServiceOption->dwServerRule,(pForfendLockTable->GetCheck()==BST_CHECKED));

	//记录积分
	CButton * pRecordGameScore=(CButton *)GetDlgItem(IDC_RECORD_GAME_SCORE);
	CServerRule::SetRecordGameScore(m_pGameServiceOption->dwServerRule,(pRecordGameScore->GetCheck()==BST_CHECKED));

	//记录过程
	CButton * pRecordGameTrack=(CButton *)GetDlgItem(IDC_RECORD_GAME_TRACK);
	CServerRule::SetRecordGameTrack(m_pGameServiceOption->dwServerRule,(pRecordGameTrack->GetCheck()==BST_CHECKED));

	//即时写分
	CButton * pImmediateWriteScore=(CButton *)GetDlgItem(IDC_IMMEDIATE_WRITE_SCORE);
	CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,(pImmediateWriteScore->GetCheck()==BST_CHECKED));

	//手机支持
	CButton* pSuportMobile = (CButton*)GetDlgItem(IDC_SERVER_SUPORT_MOBILE);
	CServerRule::SetSuportMobile(m_pGameServiceOption->dwServerRule, (pSuportMobile->GetCheck()==BST_CHECKED));

	//pc支持
	CButton* pSuportPC = (CButton*)GetDlgItem(IDC_SERVER_SUPORT_PC);
	CServerRule::SetSuportPC(m_pGameServiceOption->dwServerRule, (pSuportPC->GetCheck()==BST_CHECKED));

	//动态底分
	CButton * pDynamicCellScore=(CButton *)GetDlgItem(IDC_DYNAMIC_CELL_SCORE);
	CServerRule::SetDynamicCellScore(m_pGameServiceOption->dwServerRule,(pDynamicCellScore->GetCheck()==BST_CHECKED));

	//隐藏信息
	CButton * pAvertCheatMode=(CButton *)GetDlgItem(IDC_ALLOW_AVERT_CHEAT_MODE);
	CServerRule::SetAllowAvertCheatMode(m_pGameServiceOption->dwServerRule,(pAvertCheatMode->GetCheck()==BST_CHECKED));

	//禁止存钱
	CButton * pForfendSaveInRoom=(CButton *)GetDlgItem(IDC_FORFEND_SAVE_IN_ROOM);
	CServerRule::SetForfendSaveInRoom(m_pGameServiceOption->dwServerRule,(pForfendSaveInRoom->GetCheck()==BST_CHECKED));

	//禁止存钱
	CButton * pForfendSaveInGame=(CButton *)GetDlgItem(IDC_FORFEND_SAVE_IN_GAME);
	CServerRule::SetForfendSaveInGame(m_pGameServiceOption->dwServerRule,(pForfendSaveInGame->GetCheck()==BST_CHECKED));

	//允许占位
	CButton * pAllowAndroidSimulate=(CButton *)GetDlgItem(IDC_ALLOW_ANDROID_SIMULATE);
	CServerRule::SetAllowAndroidSimulate(m_pGameServiceOption->dwServerRule,(pAllowAndroidSimulate->GetCheck()==BST_CHECKED));

	//动态加入
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE)
	{
		CButton * pAllowDynamicJoin=(CButton *)GetDlgItem(IDC_ALLOW_DYNAMIC_JOIN);
		CServerRule::SetAllowDynamicJoin(m_pGameServiceOption->dwServerRule,(pAllowDynamicJoin->GetCheck()==BST_CHECKED));
	}

	//允许陪玩
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE)
	{
		CButton * pAllowAndroidUser=(CButton *)GetDlgItem(IDC_ALLOW_ANDROID_ATTEND);
		CServerRule::SetAllowAndroidAttend(m_pGameServiceOption->dwServerRule,(pAllowAndroidUser->GetCheck()==BST_CHECKED));
	}

	//断线代打
	if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE)
	{
		CButton * pAllowOffLineTrustee=(CButton *)GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE);
		CServerRule::SetAllowOffLineTrustee(m_pGameServiceOption->dwServerRule,(pAllowOffLineTrustee->GetCheck()==BST_CHECKED));
	}

	return true;
}

//调整控件
VOID CDlgServerOptionItem2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//构造控件
VOID CDlgServerOptionItem2::InitCtrlWindow()
{
	//条件限制
	((CEdit *)GetDlgItem(IDC_MIN_ENTER_SCORE))->LimitText(18);
	((CEdit *)GetDlgItem(IDC_MAX_ENTER_SCORE))->LimitText(18);
	((CEdit *)GetDlgItem(IDC_MIN_TABLE_SCORE))->LimitText(18);

	//会员配置
	CComboBox * pMinMember=(CComboBox *)GetDlgItem(IDC_MIN_ENTER_MEMBER);
	CComboBox * pMaxMember=(CComboBox *)GetDlgItem(IDC_MAX_ENTER_MEMBER);
	LPCTSTR pszMember[]={TEXT("所有用户"),TEXT("VIP1"),TEXT("VIP2"),TEXT("VIP3"),TEXT("VIP4"),TEXT("VIP5")};

	//会员信息
	for (INT i=0;i<CountArray(pszMember);i++)
	{
		pMinMember->SetItemData(pMinMember->AddString(pszMember[i]),i);
		pMaxMember->SetItemData(pMaxMember->AddString(pszMember[i]),i);
	}

	//控件禁用
	GetDlgItem(IDC_ALLOW_DYNAMIC_JOIN)->EnableWindow((m_pGameServiceAttrib->cbDynamicJoin==TRUE)?TRUE:FALSE);
	GetDlgItem(IDC_ALLOW_ANDROID_ATTEND)->EnableWindow((m_pGameServiceAttrib->cbAndroidUser==TRUE)?TRUE:FALSE);
	GetDlgItem(IDC_ALLOW_OFFLINE_TRUSTEE)->EnableWindow((m_pGameServiceAttrib->cbOffLineTrustee==TRUE)?TRUE:FALSE);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerOptionItem3::CDlgServerOptionItem3() : CDlgServerOptionItem(IDD_SERVER_OPTION_3)
{
}

//析构函数
CDlgServerOptionItem3::~CDlgServerOptionItem3()
{
}

//初始化函数
BOOL CDlgServerOptionItem3::OnInitDialog()
{
	__super::OnInitDialog();

	//最少人数
	if (m_pGameServiceOption->wMinDistributeUser!=0)
	{
		SetDlgItemInt(IDC_MIN_DISTRIBUTE_USER,m_pGameServiceOption->wMinDistributeUser);
	}

	//分组间隔
	if (m_pGameServiceOption->wDistributeTimeSpace!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_TIME_SPACE,m_pGameServiceOption->wDistributeTimeSpace);
	}

	//分组局数
	if (m_pGameServiceOption->wDistributeDrawCount!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_DRAW_COUNT,m_pGameServiceOption->wDistributeDrawCount);
	}

	//最少人数
	if (m_pGameServiceOption->wMinPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_START_MIN_USER,m_pGameServiceOption->wMinPartakeGameUser);
	}

	//最大人数
	if (m_pGameServiceOption->wMaxPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_START_MAX_USER,m_pGameServiceOption->wMaxPartakeGameUser);
	}

	//允许分组
	bool bDistributeAllow=((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_ALLOW)!=0);
	((CButton *)GetDlgItem(IDC_DISTRIBUTE_ALLOW))->SetCheck((bDistributeAllow==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//同桌选项
	bool bDistributeLastTable=((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_LAST_TABLE)!=0);
	((CButton *)GetDlgItem(IDC_DISTRIBUTE_LAST_TABLE))->SetCheck((bDistributeLastTable==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//地址选项
	bool bDistributeSameAddress=((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)!=0);
	((CButton *)GetDlgItem(IDC_DISTRIBUTE_SAME_ADDRESS))->SetCheck((bDistributeSameAddress==TRUE)?BST_CHECKED:BST_UNCHECKED);

	return TRUE;
}

//保存输入
bool CDlgServerOptionItem3::SaveInputInfo()
{
	//属性配置
	m_pGameServiceOption->wMinDistributeUser=GetDlgItemInt(IDC_MIN_DISTRIBUTE_USER);
	m_pGameServiceOption->wDistributeTimeSpace=GetDlgItemInt(IDC_DISTRIBUTE_TIME_SPACE);
	m_pGameServiceOption->wDistributeDrawCount=GetDlgItemInt(IDC_DISTRIBUTE_DRAW_COUNT);
	m_pGameServiceOption->wMinPartakeGameUser=GetDlgItemInt(IDC_DISTRIBUTE_START_MIN_USER);
	m_pGameServiceOption->wMaxPartakeGameUser=GetDlgItemInt(IDC_DISTRIBUTE_START_MAX_USER);

	//允许分组
	if (((CButton *)GetDlgItem(IDC_DISTRIBUTE_ALLOW))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_ALLOW;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_ALLOW;
	}

	//同桌选项
	if (((CButton *)GetDlgItem(IDC_DISTRIBUTE_LAST_TABLE))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_LAST_TABLE;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_LAST_TABLE;
	}

	//地址选项
	if (((CButton *)GetDlgItem(IDC_DISTRIBUTE_SAME_ADDRESS))->GetCheck()==BST_CHECKED)
	{
		m_pGameServiceOption->cbDistributeRule|=DISTRIBUTE_SAME_ADDRESS;
	}
	else
	{
		m_pGameServiceOption->cbDistributeRule&=~DISTRIBUTE_SAME_ADDRESS;
	}

	return true;
}

//调整控件
VOID CDlgServerOptionItem3::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////////
//私人房参数配置
//构造函数
CDlgServerOptionItem5::CDlgServerOptionItem5() : CDlgServerOptionItem(IDD_SERVER_OPTION_5)
{
}

//析构函数
CDlgServerOptionItem5::~CDlgServerOptionItem5()
{
}

//初始化函数
BOOL CDlgServerOptionItem5::OnInitDialog()
{
	__super::OnInitDialog();



	SetDlgItemInt(IDC_EDIT_MAX_ROOM, m_pPersonalRoomOption->wCanCreateCount);

	if (m_pPersonalRoomOption->dwTimeAfterBeginCount > 0)
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_BEGIN, m_pPersonalRoomOption->dwTimeAfterBeginCount);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_BEGIN, 1440);
	}

	if (m_pPersonalRoomOption->dwTimeOffLineCount > 0)
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_OFFLINE, m_pPersonalRoomOption->dwTimeOffLineCount);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_OFFLINE, 1440);
	}

	if (m_pPersonalRoomOption->dwTimeNotBeginGame > 0)
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_BEFORE_BEGIN, m_pPersonalRoomOption->dwTimeNotBeginGame);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_BEFORE_BEGIN, 1440);
	}

	if (m_pPersonalRoomOption->dwTimeAfterCreateRoom > 0)
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_CREATE, m_pPersonalRoomOption->dwTimeAfterCreateRoom);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_CREATE, 1440);
	}


	((CButton *)GetDlgItem(IDC_RADIO_COST_BEAN))->SetCheck(!m_pPersonalRoomOption->cbCardOrBean);
	((CButton *)GetDlgItem(IDC_RADIO_COST_ROOM_CARD))->SetCheck(m_pPersonalRoomOption->cbCardOrBean);

	((CButton *)GetDlgItem(IDC_RADIO_JOIN_GAME))->SetCheck(m_pPersonalRoomOption->cbIsJoinGame);
	((CButton *)GetDlgItem(IDC_RADIO_NOT_JOIN_GAME))->SetCheck(!m_pPersonalRoomOption->cbIsJoinGame);
	SetDlgItemInt(IDC_PERSONAL_ROOM_TAX, m_pPersonalRoomOption->lPersonalRoomTax);
	if(m_pPersonalRoomOption->cbIsJoinGame)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_MAX_ROOM))->EnableWindow(false);
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_EDIT_MAX_ROOM))->EnableWindow(true);
	}
	SetDlgItemInt(IDC_EDIT_FEE_BEAN_OR_ROOMCARD, m_pPersonalRoomOption->lFeeCardOrBeanCount);

	//创建房间等级
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_0))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_1))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_2))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_3))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_4))->SetCheck(false);
	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_5))->SetCheck(false);
	switch (m_pCreateRoomRightAndFee->cbCreateRight)
	{
	case 0:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_0))->SetCheck(TRUE);
			break;
		}
	case 1:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_1))->SetCheck(TRUE);
			break;
		}
	case 2:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_2))->SetCheck(TRUE);
			break;
		}
	case 3:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_3))->SetCheck(TRUE);
			break;
		}
	case 4:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_4))->SetCheck(TRUE);
			break;
		}
	case 5:
		{
			((CButton *)GetDlgItem(IDC_RADIO_LEVEL_5))->SetCheck(TRUE);
			break;
		}
	}

	//房间费用配置
	for (int i = 0; i < FEE_OPTION_COUNT; i++)
	{
		SetDlgItemInt(IDC_EDIT_TURN_NUM + i * 3, m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTurnLimit);
		SetDlgItemInt(IDC_EDIT_CREATE_FEE + i * 3, m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPayFee);
		SetDlgItemInt(IDC_EDIT_ROOM_INISCORE + i * 3, m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwIniScore);
		if (i == 0)
		{
			if (m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTurnLimit == 0 || m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwIniScore == 0 
				|| m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPayFee  == 0)
			{
				SetDlgItemInt(IDC_EDIT_TURN_NUM + i * 3, 5);
				SetDlgItemInt(IDC_EDIT_CREATE_FEE + i * 3, 1);
				SetDlgItemInt(IDC_EDIT_ROOM_INISCORE + i * 3, 1000);
			}
		}
	}




	return TRUE;
}

//保存输入
bool CDlgServerOptionItem5::SaveInputInfo()
{

	//属性配置
	m_pPersonalRoomOption->wCanCreateCount=GetDlgItemInt(IDC_EDIT_MAX_ROOM);
	m_pPersonalRoomOption->dwTimeAfterBeginCount=GetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_BEGIN);
	m_pPersonalRoomOption->dwTimeOffLineCount=GetDlgItemInt(IDC_EDIT_DISSUME_TIME_OFFLINE);
	m_pPersonalRoomOption->dwTimeNotBeginGame=GetDlgItemInt(IDC_EDIT_DISSUME_TIME_BEFORE_BEGIN);
	m_pPersonalRoomOption->dwTimeAfterCreateRoom=GetDlgItemInt(IDC_EDIT_DISSUME_TIME_AFTER_CREATE);
	m_pPersonalRoomOption->lFeeCardOrBeanCount   =   GetDlgItemInt(IDC_EDIT_FEE_BEAN_OR_ROOMCARD);
	m_pPersonalRoomOption->lPersonalRoomTax = GetDlgItemInt(IDC_PERSONAL_ROOM_TAX);


	//0 表示 消耗游戏豆  1 表示 消耗房卡	
	if (((CButton *)GetDlgItem(IDC_RADIO_COST_BEAN))->GetCheck()==BST_CHECKED)
	{
		m_pPersonalRoomOption->cbCardOrBean=0;
	}
	else
	{
		m_pPersonalRoomOption->cbCardOrBean=1;
	}

	if (((CButton *)GetDlgItem(IDC_RADIO_COST_ROOM_CARD))->GetCheck()==BST_CHECKED)
	{
		m_pPersonalRoomOption->cbCardOrBean=1;
	}
	else
	{
		m_pPersonalRoomOption->cbCardOrBean=0;
	}

	// 0 表示不参与游戏  1 标识参与游戏
	if (((CButton *)GetDlgItem(IDC_RADIO_JOIN_GAME))->GetCheck()==BST_CHECKED)
	{
		m_pPersonalRoomOption->cbIsJoinGame = 1;
	}
	else
	{
		m_pPersonalRoomOption->cbIsJoinGame = 0;
	}

	if (((CButton *)GetDlgItem(IDC_RADIO_NOT_JOIN_GAME))->GetCheck()==BST_CHECKED)
	{
		m_pPersonalRoomOption->cbIsJoinGame = 0;
	}
	else
	{
		m_pPersonalRoomOption->cbIsJoinGame = 1;
	}



	//权限判断
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_0))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 0;
	}

	//权限判断
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_1))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 1;
	}

	//权限判断
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_2))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 2;
	}

	//权限判断
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_3))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 3;
	}

	//权限判断
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_4))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 4;
	}

	//权限判断
	if(	((CButton *)GetDlgItem(IDC_RADIO_LEVEL_5))->GetCheck()==BST_CHECKED)
	{
		m_pCreateRoomRightAndFee->cbCreateRight = 5;
	}

	//房间费用配置
	for (int i = 0; i < FEE_OPTION_COUNT; i++)
	{
		m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPlayTurnLimit = GetDlgItemInt(IDC_EDIT_TURN_NUM + i * 3);
		m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwPayFee = GetDlgItemInt(IDC_EDIT_CREATE_FEE + i * 3);
		m_pCreateRoomRightAndFee->stCreateRoomPayFee[i].dwIniScore = GetDlgItemInt(IDC_EDIT_ROOM_INISCORE + i * 3);
	}

	return true;
}

//调整控件
VOID CDlgServerOptionItem5::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//调整控件
VOID CDlgServerOptionItem5::OnBnClickedJoinGameRadio()
{
	((CEdit*)GetDlgItem(IDC_EDIT_MAX_ROOM))->EnableWindow(false);
	return;
}

//调整控件
VOID CDlgServerOptionItem5::OnBnClickedNotJoinRadio()
{
	((CEdit*)GetDlgItem(IDC_EDIT_MAX_ROOM))->EnableWindow(true);
	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerOptionItemCustom::CDlgServerOptionItemCustom() : CDlgServerOptionItem(IDD_SERVER_OPTION_4)
{
	//设置变量
	m_hCustomRule=NULL;
	m_pIGameServiceCustomRule=NULL;

	return;
}

//析构函数
CDlgServerOptionItemCustom::~CDlgServerOptionItemCustom()
{
}

//初始化函数
BOOL CDlgServerOptionItemCustom::OnInitDialog()
{
	__super::OnInitDialog();

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//设置变量
	m_pIGameServiceCustomRule=m_pDlgServerWizard->m_pIGameServiceCustomRule;

	//创建窗口
	WORD wCustonSize=sizeof(m_pGameServiceOption->cbCustomRule);
	m_hCustomRule=m_pIGameServiceCustomRule->CreateCustomRule(this,rcClient,m_pGameServiceOption->cbCustomRule,wCustonSize);

	return TRUE;
}

//保存输入
bool CDlgServerOptionItemCustom::SaveInputInfo()
{
	//保存输入
	if (m_hCustomRule!=NULL)
	{
		//保存设置
		WORD wCustonSize=sizeof(m_pGameServiceOption->cbCustomRule);
		bool bSuccess=m_pIGameServiceCustomRule->SaveCustomRule(m_pGameServiceOption->cbCustomRule,wCustonSize);

		return bSuccess;
	}

	return true;
}

//调整控件
VOID CDlgServerOptionItemCustom::RectifyControl(INT nWidth, INT nHeight)
{
	//调整位置
	if (m_hCustomRule!=NULL)
	{
		::SetWindowPos(m_hCustomRule,NULL,0,0,nWidth,nHeight,SWP_NOMOVE|SWP_NOZORDER);
	}

	return;
}

//消耗消息
VOID CDlgServerOptionItemCustom::OnNcDestroy()
{
	//关闭窗口
	if (m_hCustomRule!=NULL)
	{
		::DestroyWindow(m_hCustomRule);
	}

	//设置变量
	m_hCustomRule=NULL;
	m_pIGameServiceCustomRule=NULL;

	__super::OnNcDestroy();
}

//焦点消息
VOID CDlgServerOptionItemCustom::OnSetFocus(CWnd * pNewWnd)
{
	__super::OnSetFocus(pNewWnd);

	//设置焦点
	if (m_hCustomRule!=NULL)
	{
		::SetFocus(m_hCustomRule);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerWizardItem::CDlgServerWizardItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//设置变量
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pPersonalRoomOption = NULL;
	m_pCreateRoomRightAndFee = NULL;//权限与费用配置
	return;
}

//析构函数
CDlgServerWizardItem::~CDlgServerWizardItem()
{
}

//保存数据
bool CDlgServerWizardItem::SaveItemData()
{
	//保存数据
	if ((m_hWnd!=NULL)&&(SaveInputInfo()==false))
	{
		return false;
	}

	return true;
}

//创建向导
bool CDlgServerWizardItem::ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//创建窗口
	if (m_hWnd==NULL) 
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//创建窗口
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//设置资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//显示窗口
	ShowWindow(SW_SHOW);

	return true;
}

//确定函数
VOID CDlgServerWizardItem::OnOK()
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgServerWizardItem::OnCancel()
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//位置消息
VOID CDlgServerWizardItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerWizardItem1::CDlgServerWizardItem1() : CDlgServerWizardItem(IDD_SERVER_WIZARD_1)
{
}

//析构函数
CDlgServerWizardItem1::~CDlgServerWizardItem1()
{
}

//控件绑定
VOID CDlgServerWizardItem1::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROMPT, m_StaticPrompt);
	DDX_Control(pDX, IDC_MODULE_LIST, m_ModuleListControl);
}

//初始化函数
BOOL CDlgServerWizardItem1::OnInitDialog()
{
	__super::OnInitDialog();

	//加载列表
	LoadDBModuleItem();

	return TRUE;
}

//保存输入
bool CDlgServerWizardItem1::SaveInputInfo()
{
	//变量定义
	tagGameModuleInfo * pGameModuleInfo=NULL;
	POSITION Position=m_ModuleListControl.GetFirstSelectedItemPosition();
	
	//获取选择
	if (Position!=NULL)
	{
		INT nListItem=m_ModuleListControl.GetNextSelectedItem(Position);
		pGameModuleInfo=(tagGameModuleInfo *)m_ModuleListControl.GetItemData(nListItem);
	}

	//选择判断
	if (pGameModuleInfo==NULL)
	{
		AfxMessageBox(TEXT("请您先从游戏列表中选择游戏组件"),MB_ICONERROR);
		return false;
	}

	//游戏模块
	m_GameServiceManager.CloseInstance();
	m_GameServiceManager.SetModuleCreateInfo(pGameModuleInfo->szServerDLLName,GAME_SERVICE_CREATE_NAME);

	//创建判断
	if (pGameModuleInfo->dwNativeVersion==0L)
	{
		//构造提示
		TCHAR szString[512]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("[ %s ] 服务组件还没有安装，请先安装服务组件"),pGameModuleInfo->szGameName);

		//提示消息
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//更新判断
	if (pGameModuleInfo->dwNativeVersion!=pGameModuleInfo->dwServerVersion)
	{
		//构造提示
		TCHAR szString[512]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("[ %s ] 服务组件更新了，是否还继续创建房间吗？"),pGameModuleInfo->szGameName);

		//提示消息
		if (AfxMessageBox(szString,MB_ICONWARNING|MB_YESNO|MB_DEFBUTTON2)!=IDYES) return false;
	}

	//加载模块
	if (m_GameServiceManager.CreateInstance()==false)
	{
		//构造提示
		TCHAR szString[512]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("[ %s ] 服务组件加载失败，创建游戏房间失败"),pGameModuleInfo->szGameName);

		//提示消息
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//配置模块
	m_pDlgServerWizard->SetWizardParameter(m_GameServiceManager.GetInterface(),NULL);

	//默认设置
	lstrcpyn(m_pGameServiceOption->szDataBaseName,pGameModuleInfo->szDataBaseName,CountArray(m_pGameServiceOption->szDataBaseName));
	lstrcpyn(m_pGameServiceOption->szDataBaseAddr,pGameModuleInfo->szDataBaseAddr,CountArray(m_pGameServiceOption->szDataBaseAddr));

	return true;
}

//调整控件
VOID CDlgServerWizardItem1::RectifyControl(INT nWidth, INT nHeight)
{
	//调整提示
	if (m_StaticPrompt.m_hWnd!=NULL)
	{
		m_StaticPrompt.SetWindowPos(NULL,5,8,nWidth-10,12,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	//调整列表
	if (m_ModuleListControl.m_hWnd!=NULL)
	{
		m_ModuleListControl.SetWindowPos(NULL,5,28,nWidth-10,nHeight-28,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	return;
}

//加载列表
bool CDlgServerWizardItem1::LoadDBModuleItem()
{
	//加载信息
	if (m_ModuleInfoManager.LoadGameModuleInfo(m_ModuleInfoBuffer)==true)
	{
		//重置列表
		m_ModuleListControl.DeleteAllItems();

		//变量定义
		POSITION Position=m_ModuleInfoBuffer.m_GameModuleInfoMap.GetStartPosition();

		//枚举模块
		while (Position!=NULL)
		{
			//获取对象
			WORD wModuleID=0L;
			tagGameModuleInfo * pGameModuleInfo=NULL;
			m_ModuleInfoBuffer.m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);

			//插入列表
			ASSERT(pGameModuleInfo!=NULL);
			if (pGameModuleInfo!=NULL) m_ModuleListControl.InsertModuleInfo(pGameModuleInfo);
		}
	}

	return false;
}

//双击列表
VOID CDlgServerWizardItem1::OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//加载配置
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		//变量定义
		INT nListItem=((NMITEMACTIVATE *)pNMHDR)->iItem;
		tagGameModuleInfo * pGameModuleInfo=(tagGameModuleInfo *)m_ModuleListControl.GetItemData(nListItem);

		//投递消息
		if (pGameModuleInfo->dwNativeVersion!=0L)
		{
			GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDC_NEXT,0),0);
		}

		return;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerWizardItem2::CDlgServerWizardItem2() : CDlgServerWizardItem(IDD_SERVER_WIZARD_2)
{
	//设置变量
	m_wItemCount=0;
	m_wActiveIndex=INVALID_WORD;
	ZeroMemory(m_pOptionItem,sizeof(m_pOptionItem));

	return;
}

//析构函数
CDlgServerWizardItem2::~CDlgServerWizardItem2()
{
}

//控件绑定
VOID CDlgServerWizardItem2::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CTRL, m_TabCtrl);
}

//初始化函数
BOOL CDlgServerWizardItem2::OnInitDialog()
{
	__super::OnInitDialog();

	//设置变量
	m_wItemCount=0;
	m_wActiveIndex=INVALID_WORD;

	//基本配置
	m_pOptionItem[m_wItemCount++]=&m_ServerOptionItem1;
	m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_1,TEXT("基本配置"));

	//房间选项
	m_pOptionItem[m_wItemCount++]=&m_ServerOptionItem2;
	m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_2,TEXT("房间选项"));

	//附加权限
	m_pOptionItem[m_wItemCount++]=&m_ServerOptionItem3;
	m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_3,TEXT("分组选项"));

	//私人房配置
	if(m_ServerOptionItem1.m_bIsHasPersonalRoom)
	{
		m_pOptionItem[m_wItemCount++]=&m_ServerOptionItem5;
		CDlgServerOptionItem5 temp;
		m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_5,TEXT("私人房配置"));
	}

	//服务定制
	if (m_pDlgServerWizard->m_pIGameServiceCustomRule!=NULL)
	{
		m_pOptionItem[m_wItemCount++]=&m_ServerOptionItemCustom;
		m_TabCtrl.InsertItem(ITEM_SERVER_OPTION_CUSTOM,TEXT("服务定制"));
	}

	//激活子项
	ActiveOptionItem(0);

	return TRUE;
}

//保存输入
bool CDlgServerWizardItem2::SaveInputInfo()
{
	//保存输入
	for (INT i=0;i<m_wItemCount;i++)
	{
		if (m_pOptionItem[i]->SaveItemData()==false)
		{
			ActiveOptionItem(i);
			return false;
		}
	}

	return true;
}

//调整控件
VOID CDlgServerWizardItem2::RectifyControl(INT nWidth, INT nHeight)
{
	//调整选择
	if (m_TabCtrl.m_hWnd!=NULL)
	{
		m_TabCtrl.SetWindowPos(NULL,5,5,nWidth-10,nHeight-5,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	//调整选择
	if ((m_TabCtrl.m_hWnd!=NULL)&&(m_wActiveIndex!=INVALID_WORD))
	{
		//获取位置
		CRect rcItemRect;
		m_TabCtrl.GetWindowRect(&rcItemRect);

		//计算位置
		m_TabCtrl.ScreenToClient(&rcItemRect);
		m_TabCtrl.AdjustRect(FALSE,&rcItemRect);

		//移动位置
		m_pOptionItem[m_wActiveIndex]->MoveWindow(&rcItemRect);
	}

	return;
}

//激活向导
bool CDlgServerWizardItem2::ActiveOptionItem(WORD wIndex)
{
	//判断状态
	if (m_wActiveIndex==wIndex)
	{
		m_pOptionItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//保存旧项
	CDlgServerOptionItem * pItemOption=NULL;
	if (m_wActiveIndex<m_wItemCount) pItemOption=m_pOptionItem[m_wActiveIndex];

	//获取位置
	CRect rcItemRect;
	m_TabCtrl.GetWindowRect(&rcItemRect);

	//计算位置
	m_TabCtrl.ScreenToClient(&rcItemRect);
	m_TabCtrl.AdjustRect(FALSE,&rcItemRect);

	//设置新项
	m_pOptionItem[wIndex]->m_pDlgServerWizard=m_pDlgServerWizard;
	m_pOptionItem[wIndex]->m_pGameServiceAttrib=m_pGameServiceAttrib;
	m_pOptionItem[wIndex]->m_pGameServiceOption=m_pGameServiceOption;
	m_pOptionItem[wIndex]->m_pPersonalRoomOption=m_pPersonalRoomOption;
	m_pOptionItem[wIndex]->m_pCreateRoomRightAndFee=m_pCreateRoomRightAndFee;

	//创建新项
	m_wActiveIndex=wIndex;
	m_pOptionItem[m_wActiveIndex]->ShowOptionItem(rcItemRect,&m_TabCtrl);

	//激活新项
	m_pOptionItem[m_wActiveIndex]->SetFocus();
	if (pItemOption!=NULL) pItemOption->ShowWindow(SW_HIDE);

	//设置选择
	if (m_TabCtrl.GetCurSel()!=wIndex) m_TabCtrl.SetCurSel(wIndex);

	return true;
}

//焦点消息
VOID CDlgServerWizardItem2::OnSetFocus(CWnd * pNewWnd)
{
	__super::OnSetFocus(pNewWnd);

	//设置焦点
	if ((m_wActiveIndex!=INVALID_WORD)&&(m_pOptionItem[m_wActiveIndex]->m_hWnd!=NULL))
	{
		m_pOptionItem[m_wActiveIndex]->SetFocus();
	}

	return;
}

//选择改变
VOID CDlgServerWizardItem2::OnTcnSelchangeTabCtrl(NMHDR * pNMHDR, LRESULT * pResult)
{
	//设置页面
	switch (m_TabCtrl.GetCurSel())
	{
	case ITEM_SERVER_OPTION_1:{ ActiveOptionItem(0); break; }
	case ITEM_SERVER_OPTION_2:{ ActiveOptionItem(1); break; }
	case ITEM_SERVER_OPTION_3:{ ActiveOptionItem(2); break; }
	case ITEM_SERVER_OPTION_5:{ ActiveOptionItem(ITEM_SERVER_OPTION_5); break; }
	case ITEM_SERVER_OPTION_CUSTOM:{ ActiveOptionItem(ITEM_SERVER_OPTION_CUSTOM ); break; }
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerWizard::CDlgServerWizard() : CDialog(IDD_SERVER_WIZARD_MAIN)
{
	//设置变量
	m_wActiveIndex=INVALID_WORD;
	m_pWizardItem[0]=&m_ServerWizardItem1;
	m_pWizardItem[1]=&m_ServerWizardItem2;

	//接口变量
	m_pIGameServiceManager=NULL;
	m_pIGameServiceCustomRule=NULL;

	//配置信息
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//析构函数
CDlgServerWizard::~CDlgServerWizard()
{
}

//初始化函数
BOOL CDlgServerWizard::OnInitDialog()
{
	__super::OnInitDialog();

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//激活向导
	ActiveWizardItem((m_pIGameServiceManager==NULL)?0:1);

	//组件属性
	if (m_pIGameServiceManager!=NULL)
	{
		SetDlgItemInt(IDC_GAME_ID,m_ModuleInitParameter.GameServiceAttrib.wKindID);
		SetDlgItemText(IDC_GAME_NAME,m_ModuleInitParameter.GameServiceAttrib.szGameName);
		SetDlgItemInt(IDC_CHAIR_COUNT,m_ModuleInitParameter.GameServiceAttrib.wChairCount);
		SetDlgItemText(IDC_DATABASE_NAME,m_ModuleInitParameter.GameServiceAttrib.szDataBaseName);
		SetDlgItemText(IDC_CLIENT_EXE_NAME,m_ModuleInitParameter.GameServiceAttrib.szClientEXEName);
		SetDlgItemText(IDC_SERVICE_DLL_NAME,m_ModuleInitParameter.GameServiceAttrib.szServerDLLName);
	}

	return FALSE;
}

//确定函数
VOID CDlgServerWizard::OnOK()
{
	if ((m_wActiveIndex+1)<CountArray(m_pWizardItem))
	{
		OnBnClickedNext();
	}
	else
	{
		OnBnClickedFinish();
	}

	return;
}

//创建房间
bool CDlgServerWizard::CreateGameServer()
{
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//配置房间
	if (DoModal()==IDOK)
	{
		return true;
	}

	return false;
}

//设置接口
VOID CDlgServerWizard::SetWizardParameter(IGameServiceManager * pIGameServiceManager, tagGameServiceOption * pGameServiceOption)
{
	//销毁子项
	if (pGameServiceOption==NULL)
	{
		for (WORD i=1;i<CountArray(m_pWizardItem);i++)
		{
			if ((m_pWizardItem[i]!=NULL)&&(m_pWizardItem[i]->m_hWnd!=NULL))
			{
				m_pWizardItem[i]->DestroyWindow();
			}
		}
	}

	//设置变量
	m_pIGameServiceCustomRule=NULL;
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	//获取属性
	ASSERT(pIGameServiceManager!=NULL);
	pIGameServiceManager->GetServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//设置接口
	m_pIGameServiceManager=pIGameServiceManager;
	m_pIGameServiceCustomRule=QUERY_OBJECT_PTR_INTERFACE(m_pIGameServiceManager,IGameServiceCustomRule);

	//组件属性
	if (m_hWnd!=NULL)
	{
		SetDlgItemInt(IDC_GAME_ID,m_ModuleInitParameter.GameServiceAttrib.wKindID);
		SetDlgItemText(IDC_GAME_NAME,m_ModuleInitParameter.GameServiceAttrib.szGameName);
		SetDlgItemInt(IDC_CHAIR_COUNT,m_ModuleInitParameter.GameServiceAttrib.wChairCount);
		SetDlgItemText(IDC_DATABASE_NAME,m_ModuleInitParameter.GameServiceAttrib.szDataBaseName);
		SetDlgItemText(IDC_CLIENT_EXE_NAME,m_ModuleInitParameter.GameServiceAttrib.szClientEXEName);
		SetDlgItemText(IDC_SERVICE_DLL_NAME,m_ModuleInitParameter.GameServiceAttrib.szServerDLLName);
	}

	//设置规则
	if (pGameServiceOption==NULL)
	{
		//自定规则
		if (m_pIGameServiceCustomRule!=NULL)
		{
			WORD wCustomSize=sizeof(m_ModuleInitParameter.GameServiceOption.cbCustomRule);
			m_pIGameServiceCustomRule->DefaultCustomRule(m_ModuleInitParameter.GameServiceOption.cbCustomRule,wCustomSize);
		}

		//调整参数
		ASSERT(m_pIGameServiceManager!=NULL);
		m_pIGameServiceManager->RectifyParameter(m_ModuleInitParameter.GameServiceOption);
	}
	else
	{
		//拷贝规则
		CopyMemory(&m_ModuleInitParameter.GameServiceOption,pGameServiceOption,sizeof(tagGameServiceOption));
	}

	return;
}

//设置私人房间参数
void CDlgServerWizard::SetPersonalRoomParameter(tagPersonalRoomOption PersonalRoomOption, tagPersonalRoomOptionRightAndFee PersonalRoomOptionRightAndFee)
{
	m_ModuleInitParameter.PersonalRoomOption = PersonalRoomOption;
	m_ModuleInitParameter.m_pCreateRoomRightAndFee = PersonalRoomOptionRightAndFee;
}

//激活向导
bool CDlgServerWizard::ActiveWizardItem(WORD wIndex)
{
	//判断状态
	if (m_wActiveIndex==wIndex)
	{
		m_pWizardItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//保存旧项
	CDlgServerWizardItem * pItemWizard=NULL;
	if (m_wActiveIndex<CountArray(m_pWizardItem))
	{
		//设置变量
		pItemWizard=m_pWizardItem[m_wActiveIndex];

		//保存数据
		if ((wIndex>m_wActiveIndex)&&(m_pWizardItem[m_wActiveIndex]->SaveItemData()==false)) return false;
	}

	//获取位置
	CRect rcItemRect;
	GetDlgItem(IDC_ITEM_FRAME)->GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//设置新项
	m_pWizardItem[wIndex]->m_pDlgServerWizard=this;
	m_pWizardItem[wIndex]->m_pGameServiceAttrib=&m_ModuleInitParameter.GameServiceAttrib;
	m_pWizardItem[wIndex]->m_pGameServiceOption=&m_ModuleInitParameter.GameServiceOption;
	m_pWizardItem[wIndex]->m_pPersonalRoomOption=&m_ModuleInitParameter.PersonalRoomOption;
	m_pWizardItem[wIndex]->m_pCreateRoomRightAndFee=&m_ModuleInitParameter.m_pCreateRoomRightAndFee;

	//创建新项
	m_wActiveIndex=wIndex;
	m_pWizardItem[m_wActiveIndex]->ShowWizardItem(rcItemRect,this);

	//激活新项
	m_pWizardItem[m_wActiveIndex]->SetFocus();
	if (pItemWizard!=NULL) pItemWizard->ShowWindow(SW_HIDE);

	//界面变量
	CButton * pButtonLast=(CButton * )GetDlgItem(IDC_LAST);
	CButton * pButtonNext=(CButton * )GetDlgItem(IDC_NEXT);
	CButton * pButtonFinish=(CButton * )GetDlgItem(IDC_FINISH);

	//进度界面
	pButtonNext->EnableWindow(((m_wActiveIndex+1)<CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonFinish->EnableWindow(((m_wActiveIndex+1)==CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonLast->EnableWindow(((m_wActiveIndex>1)||((m_ModuleInitParameter.GameServiceOption.wServerID==0)&&(m_wActiveIndex>0)))?TRUE:FALSE);

	//构造标题
	TCHAR szTitle[128]=TEXT("");
	_sntprintf(szTitle,CountArray(szTitle),TEXT("房间配置向导 --- [ 步骤 %d ]"),m_wActiveIndex+1);
	
	//设置标题
	SetWindowText(szTitle);

	return true;
}

//上一步
VOID CDlgServerWizard::OnBnClickedLast()
{
	//效验参数
	ASSERT(m_wActiveIndex>0);
	ASSERT(m_wActiveIndex<CountArray(m_pWizardItem));

	//切换页面
	ActiveWizardItem(m_wActiveIndex-1);

	return;
}

//下一步
VOID CDlgServerWizard::OnBnClickedNext()
{
	//效验参数
	ASSERT((m_wActiveIndex+1)<CountArray(m_pWizardItem));

	//激活页面
	ActiveWizardItem(m_wActiveIndex+1);

	return;
}

//完成按钮
VOID CDlgServerWizard::OnBnClickedFinish()
{
	//保存设置
	ASSERT(m_wActiveIndex>=1);
	if (m_pWizardItem[m_wActiveIndex]->SaveItemData()==false) return;

	//房间信息
	tagGameServerCreate GameServerCreate;
	ZeroMemory(&GameServerCreate,sizeof(GameServerCreate));

	//参数调整
	ASSERT(m_pIGameServiceManager!=NULL);
	m_pIGameServiceManager->RectifyParameter(m_ModuleInitParameter.GameServiceOption);

	//索引变量
	GameServerCreate.wGameID=m_ModuleInitParameter.GameServiceAttrib.wKindID;
	GameServerCreate.wServerID=m_ModuleInitParameter.GameServiceOption.wServerID;

	//挂接属性
	GameServerCreate.wKindID=m_ModuleInitParameter.GameServiceAttrib.wKindID;
	GameServerCreate.wNodeID=m_ModuleInitParameter.GameServiceOption.wNodeID;
	GameServerCreate.wSortID=m_ModuleInitParameter.GameServiceOption.wSortID;

	//税收配置
	GameServerCreate.lCellScore=m_ModuleInitParameter.GameServiceOption.lCellScore;
	GameServerCreate.wRevenueRatio=m_ModuleInitParameter.GameServiceOption.wRevenueRatio;
	GameServerCreate.lServiceScore=m_ModuleInitParameter.GameServiceOption.lServiceScore;

	//限制配置
	GameServerCreate.lRestrictScore=m_ModuleInitParameter.GameServiceOption.lRestrictScore;
	GameServerCreate.lMinTableScore=m_ModuleInitParameter.GameServiceOption.lMinTableScore;
	GameServerCreate.lMinEnterScore=m_ModuleInitParameter.GameServiceOption.lMinEnterScore;
	GameServerCreate.lMaxEnterScore=m_ModuleInitParameter.GameServiceOption.lMaxEnterScore;

	//会员限制
	GameServerCreate.cbMinEnterMember=m_ModuleInitParameter.GameServiceOption.cbMinEnterMember;
	GameServerCreate.cbMaxEnterMember=m_ModuleInitParameter.GameServiceOption.cbMaxEnterMember;

	//房间配置
	GameServerCreate.dwServerRule=m_ModuleInitParameter.GameServiceOption.dwServerRule;
	GameServerCreate.dwAttachUserRight=m_ModuleInitParameter.GameServiceOption.dwAttachUserRight;

	//房间属性
	GameServerCreate.wMaxPlayer=m_ModuleInitParameter.GameServiceOption.wMaxPlayer;
	GameServerCreate.wTableCount=m_ModuleInitParameter.GameServiceOption.wTableCount;
	GameServerCreate.wServerType=m_ModuleInitParameter.GameServiceOption.wServerType;
	GameServerCreate.wServerKind=m_ModuleInitParameter.GameServiceOption.wServerKind;
	GameServerCreate.wServerPort=m_ModuleInitParameter.GameServiceOption.wServerPort;
	GameServerCreate.wServerLevel=m_ModuleInitParameter.GameServiceOption.wServerLevel;
	lstrcpyn(GameServerCreate.szServerName,m_ModuleInitParameter.GameServiceOption.szServerName,CountArray(GameServerCreate.szServerName));
	lstrcpyn(GameServerCreate.szServerPasswd,m_ModuleInitParameter.GameServiceOption.szServerPasswd,CountArray(GameServerCreate.szServerPasswd));

	//分组属性
	GameServerCreate.cbDistributeRule=m_ModuleInitParameter.GameServiceOption.cbDistributeRule;
	GameServerCreate.wMinDistributeUser=m_ModuleInitParameter.GameServiceOption.wMinDistributeUser;
	GameServerCreate.wDistributeTimeSpace=m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace;
	GameServerCreate.wDistributeDrawCount=m_ModuleInitParameter.GameServiceOption.wDistributeDrawCount;
	GameServerCreate.wMinPartakeGameUser=m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser;
	GameServerCreate.wMaxPartakeGameUser=m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser;

	//连接信息
	lstrcpyn(GameServerCreate.szDataBaseName,m_ModuleInitParameter.GameServiceOption.szDataBaseName,CountArray(GameServerCreate.szDataBaseName));
	lstrcpyn(GameServerCreate.szDataBaseAddr,m_ModuleInitParameter.GameServiceOption.szDataBaseAddr,CountArray(GameServerCreate.szDataBaseAddr));

	//扩展配置
	CWHService::GetMachineID(GameServerCreate.szServiceMachine);
	CopyMemory(GameServerCreate.cbCustomRule,m_ModuleInitParameter.GameServiceOption.cbCustomRule,sizeof(GameServerCreate.cbCustomRule));

	//变量定义
	tagGameServerInfo GameServerResult;
	CServerInfoManager ServerInfoManager;
	
	//插入房间
	if (GameServerCreate.wServerID==0)
	{
		//插入私人房间信息
		ServerInfoManager.SetPersonalRoomOption(m_ModuleInitParameter.PersonalRoomOption, m_ModuleInitParameter.m_pCreateRoomRightAndFee);

		if (ServerInfoManager.InsertGameServer(&GameServerCreate,GameServerResult)==false) return;

		//提示消息
		CTraceService::TraceString(TEXT("游戏房间创建成功"),TraceLevel_Normal);
	}

	//修改房间
	if (GameServerCreate.wServerID!=0)
	{

		//修改私人房间信息
		ServerInfoManager.SetPersonalRoomOption(m_ModuleInitParameter.PersonalRoomOption, m_ModuleInitParameter.m_pCreateRoomRightAndFee);

		//修改房间
		if (ServerInfoManager.ModifyGameServer(&GameServerCreate,GameServerResult)==false) return;

		//提示消息
		CTraceService::TraceString(TEXT("游戏房间配置成功"),TraceLevel_Normal);
	}

	//索引变量
	m_ModuleInitParameter.GameServiceOption.wServerID=GameServerResult.wServerID;

	//挂接属性
	m_ModuleInitParameter.GameServiceOption.wKindID=GameServerResult.wKindID;
	m_ModuleInitParameter.GameServiceOption.wNodeID=GameServerResult.wNodeID;
	m_ModuleInitParameter.GameServiceOption.wSortID=GameServerResult.wSortID;

	//税收配置
	m_ModuleInitParameter.GameServiceOption.lCellScore=GameServerResult.lCellScore;
	m_ModuleInitParameter.GameServiceOption.wRevenueRatio=m_ModuleInitParameter.GameServiceOption.wRevenueRatio;
	m_ModuleInitParameter.GameServiceOption.lServiceScore=m_ModuleInitParameter.GameServiceOption.lServiceScore;

	//限制配置
	m_ModuleInitParameter.GameServiceOption.lRestrictScore=GameServerResult.lRestrictScore;
	m_ModuleInitParameter.GameServiceOption.lMinTableScore=GameServerResult.lMinTableScore;
	m_ModuleInitParameter.GameServiceOption.lMinEnterScore=GameServerResult.lMinEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterScore=GameServerResult.lMaxEnterScore;

	//会员限制
	m_ModuleInitParameter.GameServiceOption.cbMaxEnterMember=GameServerResult.cbMaxEnterMember;
	m_ModuleInitParameter.GameServiceOption.cbMaxEnterMember=GameServerResult.cbMaxEnterMember;

	//房间配置
	m_ModuleInitParameter.GameServiceOption.dwServerRule=GameServerResult.dwServerRule;
	m_ModuleInitParameter.GameServiceOption.dwAttachUserRight=GameServerResult.dwAttachUserRight;

	//房间属性
	m_ModuleInitParameter.GameServiceOption.wMaxPlayer=GameServerResult.wMaxPlayer;
	m_ModuleInitParameter.GameServiceOption.wTableCount=GameServerResult.wTableCount;
	m_ModuleInitParameter.GameServiceOption.wServerType=GameServerResult.wServerType;
	m_ModuleInitParameter.GameServiceOption.wServerPort=GameServerResult.wServerPort;
	m_ModuleInitParameter.GameServiceOption.wServerLevel=GameServerResult.wServerLevel;
	lstrcpyn(m_ModuleInitParameter.GameServiceOption.szServerName,GameServerResult.szServerName,LEN_SERVER);

	//分组属性
	m_ModuleInitParameter.GameServiceOption.cbDistributeRule=GameServerResult.cbDistributeRule;
	m_ModuleInitParameter.GameServiceOption.wMinDistributeUser=GameServerResult.wMinDistributeUser;
	m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace=GameServerResult.wDistributeTimeSpace;
	m_ModuleInitParameter.GameServiceOption.wDistributeDrawCount=GameServerResult.wDistributeDrawCount;
	m_ModuleInitParameter.GameServiceOption.wMinPartakeGameUser=GameServerResult.wMinPartakeGameUser;
	m_ModuleInitParameter.GameServiceOption.wMaxPartakeGameUser=GameServerResult.wMaxPartakeGameUser;

	//扩展配置
	UINT uCustomRuleSize=sizeof(m_ModuleInitParameter.GameServiceOption.cbCustomRule);
	CopyMemory(m_ModuleInitParameter.GameServiceOption.cbCustomRule,GameServerResult.cbCustomRule,uCustomRuleSize);

	//关闭窗口
	EndDialog(IDOK);

	return;
}

//////////////////////////////////////////////////////////////////////////////////

void CDlgServerOptionItem1::OnBnClickedRevenueRadio()
{
	//
	GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("税收比例："));
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->SetWindowText(TEXT(""));
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->LimitText(3);
}

void CDlgServerOptionItem1::OnBnClickedServiceRadio()
{
	//
	GetDlgItem(IDC_STATIC_REVENUE_SERVICE)->SetWindowText(TEXT("服务费数："));
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->SetWindowText(TEXT(""));
	((CEdit *)GetDlgItem(IDC_SERVICE_REVENUE))->LimitText(9);
}
