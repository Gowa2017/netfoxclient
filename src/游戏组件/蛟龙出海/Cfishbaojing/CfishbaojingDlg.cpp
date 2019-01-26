
// CfishbaojingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Cfishbaojing.h"
#include "CfishbaojingDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CFISHa23 对话框


// CCfishbaojingDlg 对话框

static int count1 = 0;

CCfishbaojingDlg::CCfishbaojingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCfishbaojingDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	count1 = 0;
}

void CCfishbaojingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_baojing);
	DDX_Control(pDX, IDC_LIST2, m_zongku);
	DDX_Control(pDX, IDC_shangxian2, m_EditTest);
	DDX_Control(pDX, IDSC2, RHVAR);
	DDX_Control(pDX, IDC_shangxian3, m_bulletcount);
}


///消息解释
BOOL CCfishbaojingDlg::PreTranslateMessage(MSG * pMsg)
{
	///按键过虑
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	///按键过虑
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{

			m_EditTest.ShowWindow(0);

			if (m_ListRow != -1)
			{
				CString text;
				m_EditTest.GetWindowText(text);
				m_zongku.SetItemText(m_ListRow, m_ListCol, text);
			}
			m_ListCol = m_ListRow = -1;
			return TRUE;
	}
		
	return __super::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CCfishbaojingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDBlclkList)
//	ON_NOTIFY(NM_CLICK, IDC_LIST2,OnNMClickList)
	ON_BN_CLICKED(IDSC, &CCfishbaojingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDSZ, &CCfishbaojingDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK2, &CCfishbaojingDlg::OnBnClickedOk2)
	ON_BN_CLICKED(IDSC2, &CCfishbaojingDlg::OnBnClickedSc2)
	ON_BN_CLICKED(IDOK3, &CCfishbaojingDlg::OnBnClickedOk3)
	ON_BN_CLICKED(IDlangchao, &CCfishbaojingDlg::OnBnClickedlangchao)
	ON_BN_CLICKED(IDOK5, &CCfishbaojingDlg::OnBnClickedOk5)
	ON_BN_CLICKED(IDOK6, &CCfishbaojingDlg::OnBnClickedOk6)
	ON_BN_CLICKED(IDOK7, &CCfishbaojingDlg::OnBnClickedOk7)
	ON_BN_CLICKED(IDheibaiqueding, &CCfishbaojingDlg::OnBnClickedheibaiqueding)
END_MESSAGE_MAP()


// CCfishbaojingDlg 消息处理程序

BOOL CCfishbaojingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	m_baojing.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_baojing.InsertColumn(0, _T(""), LVCFMT_CENTER, 500);
	m_zongku.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_zongku.InsertColumn(0, _T("总库存"), LVCFMT_CENTER, 85);
	m_zongku.InsertColumn(1, _T("相应概率"), LVCFMT_CENTER, 70);	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	PipeSendData(SUB_zongkucun, 0, 0);
	PipeSendData(SUB_baojingjilu, 0, 0);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


bool CCfishbaojingDlg::PipeSendData(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{

	theApp.PipeSendData(wSubCmdID, pData, wDataSize);
	return true;

}
bool CCfishbaojingDlg::PipeMsgEvent(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if (wSubCmdID == SUB_baojing)
	{
		TCHAR * pString = static_cast<TCHAR*>(pData);
		m_baojing.InsertItem(count1++,pString);
		
	}
	else if (wSubCmdID == SUB_zongKC)
	{
		CMD_S_CONFIG *AdminConfig = static_cast<CMD_S_CONFIG*>(pData);
		CString strBuffer;
		CString strBuffer2;
		m_zongku.DeleteAllItems();
		int nIndex = 0;
		for (int i = 0; i < 21; i++)
		{
			strBuffer.Format(TEXT("%lld"), AdminConfig->stock_crucial_score_All[i]);
			nIndex = m_zongku.InsertItem(i, strBuffer);
			strBuffer2.Format(TEXT("%.3f"), AdminConfig->stock_increase_probability_All[i]);
			m_zongku.SetItemText(nIndex, 1, strBuffer2);
			
		}

		RHVar = AdminConfig->RHRHvar3;
		if (RHVar)
		{
			m_zongku.EnableWindow(false);
			RHVAR.SetWindowText(L"禁用");
			
		}
		else
		{
			RHVAR.SetWindowText(L"启用");
			m_zongku.EnableWindow(true);

		}
		strBuffer2.Format(TEXT("%.3f"), AdminConfig->heibaigailv[0]);
		GetDlgItem(IDC_Black)->SetWindowText(strBuffer2);

		strBuffer2.Format(TEXT("%.3f"), AdminConfig->heibaigailv[1]);
		GetDlgItem(IDC_white)->SetWindowText (strBuffer2);

		strBuffer2.Format(TEXT("%d"),AdminConfig->BulletCount);
		m_bulletcount.SetWindowText(strBuffer2);

		strBuffer2.Format(TEXT("%d"), AdminConfig->LangchaoTime);
		GetDlgItem(IDC_langchao)->SetWindowText(strBuffer2);

		strBuffer2.Format(TEXT("%d"), AdminConfig->BulletTimer);
		GetDlgItem(IDC_chaojipaotime)->SetWindowText(strBuffer2);

		strBuffer2.Format(TEXT("%d"), AdminConfig->Bullet_Count);
		GetDlgItem(IDC_chaojipaocount)->SetWindowText(strBuffer2);

		strBuffer2.Format(TEXT("%.3f"), AdminConfig->Bullet_gailv);
		GetDlgItem(IDC_chaojipao)->SetWindowText(strBuffer2);


	}
	
	return true;
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCfishbaojingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCfishbaojingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//void CCfishbaojingDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
//	int							m_ListRow;
//	int							m_ListCol;
//	m_ListRow = pNMItemActivate->iItem;//列表项的索引
//	m_ListCol = pNMItemActivate->iSubItem;//列表子项的索引
//	m_EditTest.ShowWindow(0);
//		if (m_ListRow != -1)
//		{
//			CString text;
//			m_EditTest.GetWindowText(text);
//			m_zongku.SetItemText(m_ListRow, m_ListCol, text);
//		}
//	
//}

void CCfishbaojingDlg::OnDBlclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;

	CRect rc;
	CString strTemp;

	m_ListRow = pNMListView->iItem;//列表项的索引
	m_ListCol = pNMListView->iSubItem;//列表子项的索引
	m_zongku.GetSubItemRect(m_ListRow, m_ListCol, LVIR_LABEL, rc);
	if (pNMListView->iItem == -1)    //选择空白处，添加一行，并设置焦点为最后一行，第二列
	{
		return;
		m_ListRow = m_zongku.GetItemCount();
		strTemp.Format(_T("%d"), m_ListRow + 1);
		//插入列表子项
		m_zongku.InsertItem(m_ListRow, strTemp);
		//设置子项状态
		m_zongku.SetItemState(m_ListRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		//设置滚动
		m_zongku.EnsureVisible(m_ListRow, FALSE);
	}
	else
	{
		m_EditTest.SetParent(&m_zongku);
		m_EditTest.MoveWindow(rc);
		m_EditTest.SetWindowTextW(m_zongku.GetItemText(m_ListRow, m_ListCol));
		m_EditTest.ShowWindow(SW_SHOW);
		m_EditTest.SetFocus();//设置Edit焦点  
		m_EditTest.ShowCaret();//显示光标   
		m_EditTest.SetSel(0, -1);//全选  
	}
	
}
void CCfishbaojingDlg::OnBnClickedOk()
{
	CString str;
	int z = m_baojing.GetItemCount();
	for (int i = 0; i<z; i++)
	{
		if (m_baojing.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED )
		{

			m_baojing.DeleteItem(i);
			//COLORREF  clrNewBkColor;
			//clrNewBkColor = RGB(49, 106, 197);
			m_baojing.SetFocus(); //选中为焦点
			m_baojing.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			////m_baojing.EnsureVisible(0, FALSE);
			//m_baojing.SetSelectionMark(1);
			//m_baojing.SetFocus();
			PipeSendData(SUB_shanchujilu, &i, 4);
			break;
		}
	}
	

}


void CCfishbaojingDlg::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	CString text;
	GetDlgItem(IDC_EDIT233)->GetWindowText(text);
	SCORE i=_ttoi(text);
	PipeSendData(SUB_shezhi, &i, sizeof(SCORE));
}





void CCfishbaojingDlg::OnBnClickedOk2()
{
	m_baojing.DeleteAllItems();
	count1 = 0; 
	PipeSendData(SUB_baojingjilu, 0, 0);

}


void CCfishbaojingDlg::OnBnClickedSc2()
{

	CString str;
	CMD_S_CONFIG AdminConfig;
	RHVar = !RHVar;
	AdminConfig.RHRHvar3 = RHVar;
	if (RHVar)
	{
		for (int i = 0; i < 21; i++)
		{
			str = m_zongku.GetItemText(i, 0);
			AdminConfig.stock_crucial_score_All[i] = _ttoll(str);

			str = m_zongku.GetItemText(i, 1);
			AdminConfig.stock_increase_probability_All[i] = _ttof(str);

		}
		RHVAR.SetWindowText(L"禁用");
		m_zongku.EnableWindow(false);
		PipeSendData(SUB_zongkucunxiugai, &AdminConfig, sizeof(CMD_S_CONFIG));

	}
	else
	{
		RHVAR.SetWindowText(L"启用");
		m_zongku.EnableWindow(true);
		PipeSendData(SUB_zongkucunxiugai, &AdminConfig, sizeof(CMD_S_CONFIG));

	}
}


void CCfishbaojingDlg::OnBnClickedOk3()
{
	CString text;
	m_bulletcount.GetWindowText(text);
	int count = 0;
	count=_ttoi(text);
	if (count <= 0)
		MessageBox(L"子弹数量至少为1");
	else
		PipeSendData(SUB_C_BulletCount,&count, sizeof(int));


}


void CCfishbaojingDlg::OnBnClickedlangchao()
{
	CString text;
	GetDlgItem(IDC_langchao)->GetWindowText(text);
	int time = 0;
	time = _ttoi(text);
	PipeSendData(SUB_C_LangChaoTime, &time, sizeof(int));
}


void CCfishbaojingDlg::OnBnClickedOk5()
{
	CString text;
	GetDlgItem(IDC_chaojipao)->GetWindowText(text);
	double gailv = 0;
	gailv = _ttof(text);
	PipeSendData(SUB_C_SuPerGaiLv, &gailv, sizeof(double));
}



void CCfishbaojingDlg::OnBnClickedOk6()
{
	CString text;
	GetDlgItem(IDC_chaojipaotime)->GetWindowText(text);
	DWORD time = 0;
	time = _tcstoul(text, NULL, 10);
	PipeSendData(SUB_C_SuperTimer, &time, sizeof(DWORD));
}



void CCfishbaojingDlg::OnBnClickedOk7()
{
	CString text;
	GetDlgItem(IDC_chaojipaocount)->GetWindowText(text);
	DWORD count = 0;
	count = _tcstoul(text, NULL, 10);
	PipeSendData(SUB_C_FISH_Count, &count, sizeof(int));
}


void CCfishbaojingDlg::OnBnClickedheibaiqueding()
{
	CString text;
	double count[2] = { 0.5, 1.5 };
	GetDlgItem(IDC_white)->GetWindowText(text);
	count[1] = _ttof(text);
	GetDlgItem(IDC_Black)->GetWindowText(text);
	count[0] = _ttof(text);

	PipeSendData(SUB_C_heibaigailv, &count, sizeof(count));

}
