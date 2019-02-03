
// Fish_ConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fish_Config.h"
#include "Fish_ConfigDlg.h"
#include "afxdialogex.h"
#include<atlconv.h>
#include<string>
#include<vector>
using std::string;
using std::vector;
#define PIPE_NAME_S		L"\\\\.\\pipe\\Pipe_FishConfig_S"
#define PIPE_NAME_C		L"\\\\.\\pipe\\Pipe_FishConfig_C"

#define IDI_LOAD_FANGSHUI	1001
#define kucunpeizhi 1005
#define IDI_fasong 1006
#define IDI_baifenbi 1007
static int aaaaaa = 0;;
static bool ppp = true;
static bool ppp6 = true;
static bool ppp1 = true;
static bool ppp2 = true, ppp3 = true, ppp4 = true, panduan = true;
static bool panduan2 = false;
static SCORE cha = 0, he = 0;
static DWORD  ceshi2 = 0;
static bool var1 = true;
static bool var2 = true, var3 = true,var4=true;
BEGIN_MESSAGE_MAP(CFish_ConfigDlg, CDialogEx)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FISH, OnNMDblclkList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FISH, OnNMClickList)
	ON_NOTIFY(LVN_BEGINSCROLL, IDC_LIST_FISH, OnLvnBeginScrollList)
	
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_STOCKMIN, OnNMDblclkList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_STOCKMIN, OnNMClickList)
	ON_NOTIFY(LVN_BEGINSCROLL, IDC_LIST_STOCKMIN, OnLvnBeginScrollList)

	ON_NOTIFY(NM_DBLCLK, IDC_LIST_STOCKMAX, OnNMDblclkList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_STOCKMAX, OnNMClickList)
	ON_NOTIFY(LVN_BEGINSCROLL, IDC_LIST_STOCKMAX, OnLvnBeginScrollList)


	ON_NOTIFY(NM_DBLCLK, IDC_LIST_USWIN, OnNMDblclkList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_USWIN, OnNMClickList)
	ON_NOTIFY(LVN_BEGINSCROLL, IDC_LIST_USWIN, OnLvnBeginScrollList)

	ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BUTTONQUERY, OnBnClickedButtonquery)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BTN_ADD2, OnBnClickedBtnAdd2)
	
	ON_BN_CLICKED(IDC_BUTTON_DELETE2, OnBnClickedButtonDelete2)
	ON_BN_CLICKED(IDC_BUTTONQUERY2, OnBnClickedButtonquery2)

	ON_BN_CLICKED(IDC_BTN_tianjia, OnBnClickedBtnAdd3)
	ON_BN_CLICKED(IDC_BTN_shanchu, OnBnClickedButtonDelete3)
	ON_BN_CLICKED(IDC_BTN_LOAD, OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CFish_ConfigDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_FSSTART, &CFish_ConfigDlg::OnBnClickedBtnFsStart)
	ON_BN_CLICKED(IDC_BTN_FSSTOP, &CFish_ConfigDlg::OnBnClickedBtnFsStop)
	ON_BN_CLICKED(IDC_BUTTON_queidng, &CFish_ConfigDlg::OnBnClickedButtonqueidng)
	ON_BN_CLICKED(IDC_BUTTON_queidng2, &CFish_ConfigDlg::OnBnClickedButtonqueidng2)

	ON_BN_CLICKED(IDC_BUTTON_jiqiren1, &CFish_ConfigDlg::OnBnClickedButtonjiqiren1)
	ON_BN_CLICKED(IDC_BUTTON_jiqiren2, &CFish_ConfigDlg::OnBnClickedButtonjiqiren2)
	ON_BN_CLICKED(IDC_BUTTON_likaiqueding, &CFish_ConfigDlg::OnBnClickedButtonlikaiqueding)
	ON_BN_CLICKED(IDC_BTN_fuzhi, &CFish_ConfigDlg::OnBnClickedBtnfuzhi)
	ON_BN_CLICKED(IDC_BUTTON_xiugai, &CFish_ConfigDlg::OnBnClickedButtonxiugai)
	ON_BN_CLICKED(IDC_BTN_qiyong2, &CFish_ConfigDlg::OnBnClickedBtnqiyong2)
	ON_BN_CLICKED(IDC_BTN_qiyong3, &CFish_ConfigDlg::OnBnClickedBtnqiyong3)
	ON_BN_CLICKED(IDC_BTN_qujianqueding, &CFish_ConfigDlg::OnBnClickedBtnqujianqueding)
	ON_BN_CLICKED(IDC_BUTTON_queidng3, &CFish_ConfigDlg::OnBnClickedButtonqueidng3)
END_MESSAGE_MAP()


// CFish_ConfigDlg 对话框

CFish_ConfigDlg::CFish_ConfigDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CFish_ConfigDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pActiveList=nullptr;
}

void CFish_ConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FISH, m_ListFish);
	DDX_Control(pDX, IDC_EDIT_TEST, m_EditTest);
	DDX_Control(pDX, IDC_LIST_STOCKMIN, m_ListStockMin);
	DDX_Control(pDX, IDC_LIST_STOCKMAX, m_ListStockMax);
	DDX_Control(pDX, IDC_EDIT_STOCK_MIN, m_EditStockMin);
	DDX_Control(pDX, IDC_EDIT_STOCK_MAX, m_EditStockMax);
	DDX_Control(pDX, IDC_EDIT_STOCK_MAX2,m_zhengtigailv);
	DDX_Control(pDX, IDC_EDIT_EXCHANGE_US, m_EditExChangeUser);
	DDX_Control(pDX, IDC_EDIT_EXCHANGE_FISH, m_EditExChangeFish);
	DDX_Control(pDX, IDC_EDIT_EXCHANGE_COUNT, m_EditExchangeCount);
	DDX_Control(pDX, IDC_EDIT_MIN_BULLET, m_EditMinBullet);
	DDX_Control(pDX, IDC_EDIT_MAX_BULLET, m_EditMaxBullet);
	DDX_Control(pDX, IDC_LIST_TESHUYU, m_ListTeShuYu);
	DDX_Control(pDX, IDC_LIST_HBMD, m_ListHBMD);
	DDX_Control(pDX, IDC_LIST_USWIN, m_UserWinLimit);
	DDX_Control(pDX, IDC_LIST_USWIN2, m_ListFangShui);
	DDX_Control(pDX, IDC_EDIT_GAMEID, m_EditGameID);
	DDX_Control(pDX, IDC_EDIT_GAILV, m_EditBuHuoGaiLv);
	DDX_Control(pDX, IDC_EDIT_COUNT, m_EditBuHuoCount);
	DDX_Control(pDX, IDC_EDIT_GAMEID2, m_EditGameID2);
	DDX_Control(pDX, IDC_EDIT_REVENUE, m_EditRevenue);
	DDX_Control(pDX, IDC_EDIT_REVENUEBFB, m_EditRevenuebfb);
	DDX_Control(pDX, IDC_STATIC_FANGSHUI, m_StaticFangShui);
	DDX_Control(pDX, IDC_select8, m_currentkongzhi);
	DDX_Control(pDX, IDC_select9, m_currentkongzhi1);
	DDX_Control(pDX, IDC_select10, m_currentkongzhi2);
	DDX_Control(pDX, IDC_select11, m_currentkongzhi3);
	DDX_Control(pDX, IDC_BTN_FSSTART, m_ButtonFSStart);
	DDX_Control(pDX, IDC_BTN_FSSTOP, m_ButtonFSStop);
	DDX_Control(pDX, IDC_BUTTON_xiugai, m_buttonqidong);

	DDX_Control(pDX, IDC_EDIT_FANGSHUI_EDU, m_EditFangSHuiEdu);
	DDX_Control(pDX, IDC_EDIT1,m_EDITPPG);
	DDX_Control(pDX, IDC_EDIT_FANGSHUI_GAILV, m_EditFangShuiGaiLv);
	DDX_Control(pDX, IDC_EDIT_FANGSHUI_TIME, m_EditFangShuiTime);
	DDX_Control(pDX, IDC_EDIT9, m_Editfangshuixiaoyu);
	DDX_Control(pDX, IDC_EDIT8, m_Editfangshuidayu);
	DDX_Control(pDX, IDC_shijian, m_bodongshijian);
	DDX_Control(pDX, IDC_hard, m_hard);
	DDX_Control(pDX, IDC_easy, m_easy);
	DDX_Control(pDX, IDC_youxiid, m_youxiID1);
	DDX_Control(pDX, IDC_youxiid1, m_youxiid2);
	DDX_Control(pDX, IDC_qujianmax, K_qujianmax);
	DDX_Control(pDX, IDC_qujianmin, K_qujianmin);
	/*DDX_Control(pDX, IDC_zuowei1, m_yizihao2);*/
	//DDX_Control(pDX, IDC_zuowei, m_yizihao1);
	DDX_Control(pDX, IDC_LIST_IPhao, m_iphaoma);
	DDX_Control(pDX, IDC_renshu1, m_zaixianrenshu);
	DDX_Control(pDX, IDC_LIST_IPhao2, m_iphaoma1);
	DDX_Control(pDX, IDC_BTN_tianjia, m_gl2);
	DDX_Control(pDX, IDC_BTN_shanchu, m_gl2);
	DDX_Control(pDX, IDC_IPhao, m_gl2);
	DDX_Control(pDX, IDC_Gailv2, m_gl2);
	DDX_Control(pDX, IDC_shijian3, m_bodongmax);
	DDX_Control(pDX, IDC_EDIT_p1, m_pao1);
	DDX_Control(pDX, IDC_EDIT_p2, m_pao2);
	DDX_Control(pDX, IDC_EDIT_p3, m_pao3);
	DDX_Control(pDX, IDC_EDIT_p4, m_pao4);
	DDX_Control(pDX, IDC_EDIT_p5, m_pao5);
	DDX_Control(pDX, IDC_EDIT_p6, m_pao6);
	DDX_Control(pDX, IDC_EDIT_p7, m_pao7);
	DDX_Control(pDX, IDC_EDIT_p8, m_pao8);
	DDX_Control(pDX, IDC_EDIT_p9, m_pao9);
	DDX_Control(pDX, IDC_EDIT_p10, m_pao10);
	DDX_Control(pDX, IDC_EDIT_ti, m_tichu);
	DDX_Control(pDX, IDC_EDIT_qijiren, m_jiqiren);
	DDX_Control(pDX, IDC_EDIT_qijiren2, m_jiqiren2);
	DDX_Control(pDX, IDC_EDIT_qijiren3, m_jiqiren3);
	DDX_Control(pDX, IDC_EDIT_minlikaishijian, m_minleave);
	DDX_Control(pDX, IDC_EDIT_maxlikaishijian, m_maxleave);
	DDX_Control(pDX, IDC_renshu5, m_winPercentage);
	DDX_Control(pDX, IDC_renshu6, m_losePercentage);
	DDX_Control(pDX, IDC_EDIT_p11, m_weizhi1);
	DDX_Control(pDX, IDC_EDIT_p12, m_weizhi2);
	DDX_Control(pDX, IDC_EDIT_p13, m_weizhi3);
	DDX_Control(pDX, IDC_EDIT_p14, m_weizhi4);
	DDX_Control(pDX, IDC_EDIT_p15, m_weizhi5);
	DDX_Control(pDX, IDC_EDIT_p16, m_weizhi6);
	DDX_Control(pDX, IDC_EDIT_p17, m_weizhi7);
	DDX_Control(pDX, IDC_EDIT_p18, m_weizhi8);
	DDX_Control(pDX, IDC_STATICX, m_ZhongWen);
	DDX_Control(pDX, IDC_STATICZ, m_ZhongWen1);



}

// CFish_ConfigDlg 消息处理程序
BOOL CFish_ConfigDlg::OnInitDialog()
{
	
	CDialogEx::OnInitDialog();
	panduan = true;
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_SHOW);
	CenterWindow();
	// TODO:  在此添加额外的初始化代码
	m_EditTest.ShowWindow(SW_HIDE);
	m_ListFish.SetExtendedStyle(m_ListFish.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListFish.InsertColumn(0, _T("ID"), LVCFMT_CENTER, 50);
	m_ListFish.InsertColumn(1, _T("最小倍数"), LVCFMT_CENTER, 60);
	m_ListFish.InsertColumn(2, _T("最大倍数"), LVCFMT_CENTER, 60);
	m_ListFish.InsertColumn(3, _T("移动速度"), LVCFMT_CENTER, 60);
	m_ListFish.InsertColumn(4, _T("打中概率"), LVCFMT_CENTER, 60);
	
	m_ListStockMin.SetExtendedStyle(m_ListStockMin.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListStockMin.InsertColumn(0, _T("小鱼库存"), LVCFMT_CENTER, 85);
	m_ListStockMin.InsertColumn(1, _T("相应概率"), LVCFMT_CENTER, 70);
	m_ListStockMax.SetExtendedStyle(m_ListStockMax.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListStockMax.InsertColumn(0, _T("大鱼库存"), LVCFMT_CENTER, 85);
	m_ListStockMax.InsertColumn(1, _T("相应概率"), LVCFMT_CENTER, 70);

	m_ListTeShuYu.SetExtendedStyle(m_ListTeShuYu.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListTeShuYu.InsertColumn(0, _T("GameID"), LVCFMT_CENTER, 60);
	m_ListTeShuYu.InsertColumn(1, _T("KIND"), LVCFMT_CENTER, 50);
	m_ListTeShuYu.InsertColumn(2, _T("数量"), LVCFMT_CENTER, 50);
	m_ListTeShuYu.InsertColumn(3, _T("概率"), LVCFMT_CENTER, 50);

	m_ListHBMD.SetExtendedStyle(m_ListTeShuYu.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListHBMD.InsertColumn(0, _T("GameID"), LVCFMT_CENTER, 80);
	m_ListHBMD.InsertColumn(1, _T("黑白名单"), LVCFMT_CENTER, 100);
	
	m_UserWinLimit.SetExtendedStyle(m_UserWinLimit.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_UserWinLimit.InsertColumn(0, _T("输赢档位"), LVCFMT_CENTER, 80);
	m_UserWinLimit.InsertColumn(1, _T("对应概率"), LVCFMT_CENTER, 70);

	m_ListFangShui.SetExtendedStyle(m_UserWinLimit.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListFangShui.InsertColumn(0, _T("放水时间                 放水总分  小鱼库存   大鱼库存"), LVCFMT_CENTER, 450);

	m_iphaoma.SetExtendedStyle(m_UserWinLimit.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_iphaoma.InsertColumn(0, _T("IP号"), LVCFMT_CENTER, 80);
	m_iphaoma.InsertColumn(1, _T("GameID"), LVCFMT_CENTER, 80);

	m_iphaoma1.SetExtendedStyle(m_UserWinLimit.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_iphaoma1.InsertColumn(0, _T("IP号"), LVCFMT_CENTER, 80);
	m_iphaoma1.InsertColumn(1, _T("概率"), LVCFMT_CENTER, 80);
	PipeSendData(SUB_C_ip, 0, 0);
	PipeSendData(SUB_C_ADMIN_CONFIG, 0, 0);
	PipeSendData(SUB_C_ADMIN_FANGSHUI, 0, 0);
	
	SetTimer(IDI_LOAD_FANGSHUI, 1000, NULL);
	SetTimer(kucunpeizhi, 10000, NULL);
	SetTimer(IDI_fasong, 1000, NULL);
	SetTimer(IDI_baifenbi, 1500, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

///消息解释
BOOL CFish_ConfigDlg::PreTranslateMessage(MSG * pMsg)
{
	///按键过虑
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	///按键过虑
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		
		if (m_pActiveList!=nullptr && m_EditTest.m_hWnd != NULL && m_EditTest.IsWindowVisible())
		{
			m_EditTest.ShowWindow(0);
			
			if (m_ListRow != -1)
			{
				CString text;
				m_EditTest.GetWindowText(text);
				m_pActiveList->SetItemText(m_ListRow, m_ListCol, text);
			}
			m_pActiveList=nullptr;
			m_ListCol = m_ListRow = -1;
		}
		return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}

void CFish_ConfigDlg::OnTimer(UINT nTimeID)
{
	if (nTimeID == IDI_LOAD_FANGSHUI)
	{
		PipeSendData(SUB_C_ADMIN_FANGSHUI, 0, 0);
		return ;
	}
	if (nTimeID == kucunpeizhi)
	{
		PipeSendData(SUB_gengxin, 0, 0);
		return;
	}	
	if (nTimeID == IDI_fasong)
	{
		PipeSendData(SUB_C_ipfasong, 0, 0);
		KillTimer(IDI_fasong);
	}
	if (nTimeID == IDI_baifenbi)
	{
		PipeSendData(SUB_huoqu, 0, 0);
		KillTimer(IDI_baifenbi);

	}
	return __super::OnTimer(nTimeID);
}
///发送数据
bool CFish_ConfigDlg::PipeSendData(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	theApp.PipeSendData(wSubCmdID, pData, wDataSize);
	return true;
}
bool  CFish_ConfigDlg::PipeSendDataTren(VOID *pData, WORD wDatasize)
{
	theApp.pipeSendDataTren(pData, wDatasize);
	return true;

}
bool CFish_ConfigDlg::PipeMsgEvent(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if (wSubCmdID == SUB_S_ADMIN_CONFIG)
	{

		//ASSERT(wDataSize == sizeof(CMD_S_ADMIN_CONFIG));
		if (!(wDataSize == sizeof(CMD_S_ADMIN_CONFIG))) { return false; }
		CMD_S_ADMIN_CONFIG * pAdmin = (CMD_S_ADMIN_CONFIG *)pData;
		m_ListStockMin.DeleteAllItems();
		m_ListStockMax.DeleteAllItems();

		CString strBuffer;
		CString strBuffer1;
		CString strBuffer2;
		CString strBuffer3;
		CString strBuffer4;
		CString strBuffer5;
		CString strBuffer6;
		CString strBuffer7;
		CString strBuffer8;
		CString strBuffer9;
		CString strBuffer10;
		CString strbuffer8;
		CString strbuffer9;
		int nIndex = 0;
		strBuffer9.Format(TEXT("%d"), pAdmin->qijiren[0]);
		m_jiqiren.SetWindowText(strBuffer9);
		strBuffer8.Format(TEXT("%d"), pAdmin->qijiren[1]);
		m_jiqiren2.SetWindowText(strBuffer8);
		strBuffer7.Format(TEXT("%.3f"), pAdmin->qijirengailv);
		m_jiqiren3.SetWindowText(strBuffer7);
		strBuffer10.Format(TEXT("%d"), pAdmin->jiqirenleave[0]);
		m_minleave.SetWindowText(strBuffer10);

		strBuffer1.Format(TEXT("%d"), pAdmin->jiqirenleave[1]);

		m_maxleave.SetWindowText(strBuffer1);




		strBuffer.Format(TEXT("%d"), pAdmin->exchange_ratio_userscore_);
		m_EditExChangeUser.SetWindowText(strBuffer);

		strBuffer1.Format(TEXT("%d"), pAdmin->exchange_ratio_fishscore_);
		m_EditExChangeFish.SetWindowText(strBuffer1);

		strBuffer2.Format(TEXT("%d"), pAdmin->exchange_fishscore_count_);
		m_EditExchangeCount.SetWindowText(strBuffer2);

		
		strBuffer3.Format(TEXT("%d"), pAdmin->bullet_multiple[0]);
		m_EditMinBullet.SetWindowText(strBuffer3);
		
		strBuffer4.Format(TEXT("%d"), pAdmin->bullet_multiple[1]);
		m_EditMaxBullet.SetWindowText(strBuffer4);

		strBuffer5.Format(TEXT("%lld"), pAdmin->stock_score[0]);
		m_EditStockMin.SetWindowText(strBuffer5);

		strBuffer7.Format(TEXT("%lld"), pAdmin->stock_score[1]);
		m_EditStockMax.SetWindowText(strBuffer7);
		for (int i = 0; i < strBuffer5.GetLength(); i++)
		{
			switch (strBuffer5[i])
			{
			case 48:
				strbuffer8.Append(TEXT("0"));
				break;
			case 49:
				strbuffer8.Append(TEXT("1"));
				break;
			case 50:
				strbuffer8.Append(TEXT("2"));
				break;
			case 51:
				strbuffer8.Append(TEXT("3"));
				break;
			case 52:
				strbuffer8.Append(TEXT("4"));
				break;
			case 53:
				strbuffer8.Append(TEXT("5"));
				break;
			case 54:
				strbuffer8.Append(TEXT("6"));
				break;
			case 55:
				strbuffer8.Append(TEXT("7"));
				break;
			case 56:
				strbuffer8.Append(TEXT("8"));
				break;
			case 57:
				strbuffer8.Append(TEXT("9"));
				break;
			default:
				break;
			}
		}
		if (strBuffer5.GetLength() > 8)
		{
			strbuffer8.Insert(strBuffer5.GetLength() - 8, TEXT("亿"));
			strbuffer8.Insert(strBuffer5.GetLength() - 3, TEXT("万"));
		}
		else if (strBuffer5.GetLength() > 5)
		{
			strbuffer8.Insert(strBuffer5.GetLength() - 4, TEXT("万"));
		}

		m_ZhongWen.SetWindowText(strbuffer8);
		for (int i = 0; i < strBuffer7.GetLength(); i++)
		{
			switch (strBuffer7[i])
			{
			case 48:
				strbuffer9.Append(TEXT("0"));
				break;
			case 49:
				strbuffer9.Append(TEXT("1"));
				break;
			case 50:
				strbuffer9.Append(TEXT("2"));
				break;
			case 51:
				strbuffer9.Append(TEXT("3"));
				break;
			case 52:
				strbuffer9.Append(TEXT("4"));
				break;
			case 53:
				strbuffer9.Append(TEXT("5"));
				break;
			case 54:
				strbuffer9.Append(TEXT("6"));
				break;
			case 55:
				strbuffer9.Append(TEXT("7"));
				break;
			case 56:
				strbuffer9.Append(TEXT("8"));
				break;
			case 57:
				strbuffer9.Append(TEXT("9"));
				break;
			default:
				break;
			}
		}
		if (strBuffer7.GetLength() > 8)
		{
			strbuffer9.Insert(strBuffer7.GetLength() - 8, TEXT("亿"));
			strbuffer9.Insert(strBuffer7.GetLength() - 3, TEXT("万"));
		}
		else if (strBuffer7.GetLength() > 5)
		{
			strbuffer9.Insert(strBuffer7.GetLength() - 4, TEXT("万"));
		}
		m_ZhongWen1.SetWindowText(strbuffer9);
		strBuffer6.Format(TEXT("%.3f"), pAdmin->paogailv[0]);
		m_pao1.SetWindowText(strBuffer6);

		strBuffer6.Format(TEXT("%.3f"), pAdmin->paogailv[1]);
		m_pao2.SetWindowText(strBuffer6);

		strBuffer6.Format(TEXT("%.3f"), pAdmin->paogailv[2]);
		m_pao3.SetWindowText(strBuffer6);

		strBuffer6.Format(TEXT("%.3f"), pAdmin->paogailv[3]);
		m_pao4.SetWindowText(strBuffer6);

		strBuffer6.Format(TEXT("%.3f"), pAdmin->paogailv[4]);
		m_pao5.SetWindowText(strBuffer6);

		strBuffer6.Format(TEXT("%.3f"), pAdmin->paogailv[5]);
		m_pao6.SetWindowText(strBuffer6);

		strBuffer6.Format(TEXT("%.3f"), pAdmin->paogailv[6]);
		m_pao7.SetWindowText(strBuffer6);

		strBuffer6.Format(TEXT("%.3f"), pAdmin->paogailv[7]);
		m_pao8.SetWindowText(strBuffer6);

		strBuffer6.Format(TEXT("%.3f"), pAdmin->paogailv[8]);
		m_pao9.SetWindowText(strBuffer6);

		strBuffer6.Format(TEXT("%.3f"), pAdmin->paogailv[9]);
		m_pao10.SetWindowText(strBuffer6);

		strBuffer3.Format(TEXT("%.3f"), pAdmin->zuoweigailv[0]);
		m_weizhi1.SetWindowText(strBuffer3);

		strBuffer3.Format(TEXT("%.3f"), pAdmin->zuoweigailv[1]);
		m_weizhi2.SetWindowText(strBuffer3);

		strBuffer3.Format(TEXT("%.3f"), pAdmin->zuoweigailv[2]);
		m_weizhi3.SetWindowText(strBuffer3);

		strBuffer3.Format(TEXT("%.3f"), pAdmin->zuoweigailv[3]);
		m_weizhi4.SetWindowText(strBuffer3);

		strBuffer3.Format(TEXT("%.3f"), pAdmin->zuoweigailv[4]);
		m_weizhi5.SetWindowText(strBuffer3);

		strBuffer3.Format(TEXT("%.3f"), pAdmin->zuoweigailv[5]);
		m_weizhi6.SetWindowText(strBuffer3);

		strBuffer3.Format(TEXT("%.3f"), pAdmin->zuoweigailv[6]);
		m_weizhi7.SetWindowText(strBuffer3);

		strBuffer3.Format(TEXT("%.3f"), pAdmin->zuoweigailv[7]);
		m_weizhi8.SetWindowText(strBuffer3);

		strBuffer8.Format(TEXT("%.3f"), pAdmin->zhengtigailv);
		m_zhengtigailv.SetWindowText(strBuffer8);

		strBuffer9.Format(TEXT("%lld"), pAdmin->revenue_score);
		m_EditRevenue.SetWindowText(strBuffer9);

		strBuffer10.Format(TEXT("%.3f"), pAdmin->total_return_rate_);
		m_EditRevenuebfb.SetWindowText(strBuffer10);
		
		strBuffer.Format(TEXT("%d"), pAdmin->bodongshijian);
		m_bodongshijian.SetWindowText(strBuffer);

		strBuffer1.Format(TEXT("%.3f"), pAdmin->hard);
		m_hard.SetWindowText(strBuffer1);

		strBuffer2.Format(TEXT("%.3f"), pAdmin->easy);
		m_easy.SetWindowText(strBuffer2);

		strBuffer3.Format(TEXT("%d"), pAdmin->bodongmax);
		m_bodongmax.SetWindowText(strBuffer3);

		for (int i = 0; i < 12; i++)
		{
			strBuffer4.Format(TEXT("%d"), pAdmin->stock_crucial_score_[i]);
			nIndex = m_ListStockMax.InsertItem(i, strBuffer4);
			strBuffer5.Format(TEXT("%.3f"), pAdmin->stock_increase_probability_[i]);
			m_ListStockMax.SetItemText(nIndex, 1, strBuffer5);

			strBuffer6.Format(TEXT("%d"), pAdmin->stock_crucial_score_small[i]);
			nIndex = m_ListStockMin.InsertItem(i, strBuffer6);

			strBuffer7.Format(TEXT("%.3f"), pAdmin->stock_increase_probability_small[i]);
			m_ListStockMin.SetItemText(nIndex, 1, strBuffer7);
		}


		m_ListFish.DeleteAllItems();
		for (int i = 0; i < FISH_KIND_COUNT; i++)
		{
			strBuffer8.Format(TEXT("%02d号鱼"), i + 1);
			nIndex = m_ListFish.InsertItem(i, strBuffer8);

			strBuffer9.Format(TEXT("%d"), pAdmin->nFishMinMultiple[i]);
			m_ListFish.SetItemText(nIndex, 1, strBuffer9);

			strBuffer10.Format(TEXT("%d"), pAdmin->nFishMaxMultiple[i]);
			m_ListFish.SetItemText(nIndex, 2, strBuffer10);

			strBuffer1.Format(TEXT("%d"), pAdmin->nFishSpeed[i]);
			m_ListFish.SetItemText(nIndex, 3, strBuffer1);

			strBuffer.Format(TEXT("%.3f"), pAdmin->fFishGailv[i]);
			m_ListFish.SetItemText(nIndex, 4, strBuffer);
		}

		

		m_UserWinLimit.DeleteAllItems();
		for (int i = 0; i < 20; i++)
		{
			strBuffer.Format(TEXT("%d"), pAdmin->win_score_limit_[i]);
			nIndex = m_UserWinLimit.InsertItem(i, strBuffer);
			strBuffer2.Format(TEXT("%.3f"), pAdmin->win_decrease_probability_[i]);
			m_UserWinLimit.SetItemText(nIndex, 1, strBuffer2);
		}

	}
	else if (wSubCmdID == SUB_S_HEIBAI_LIST)
	{
		//ASSERT(wDataSize == sizeof(CMD_S_WhiteBlackList));
		if (wDataSize != sizeof(CMD_S_WhiteBlackList)) return false;
		CMD_S_WhiteBlackList* pWhiteBlackList = static_cast<CMD_S_WhiteBlackList*>(pData);


		m_ListHBMD.DeleteAllItems();
		CString strBuffer;
		int nIndex = 0;
		
		
		for (int i = 0; i<30; i++)
		{
			if (pWhiteBlackList->WhiteList[i] == 0) continue;
			strBuffer.Format(TEXT("%d"), pWhiteBlackList->WhiteList[i]);
			nIndex = m_ListHBMD.InsertItem(i, strBuffer);
			m_ListHBMD.SetItemText(nIndex, 1, TEXT("白名单"));
		}
		for (int i = 0; i<30; i++)
		{
			if (pWhiteBlackList->BlackList[i] == 0) continue;
			strBuffer.Format(TEXT("%d"), pWhiteBlackList->BlackList[i]);
			nIndex = m_ListHBMD.InsertItem(i, strBuffer);
			m_ListHBMD.SetItemText(nIndex, 1, TEXT("黑名单"));
		}
	}
	else if (wSubCmdID == sub_iptianjiashanchu)
	{
		CMD_S_a *wangz = static_cast<CMD_S_a *>(pData);
		m_iphaoma1.DeleteAllItems();
		CString strbuffer;
		CString xiaz;
		int nIndex = 0;
		for (int i = 0; i < 30; i++)
		{
			if (wangz->abc[i] == 0)continue;
			in_addr a;
			a.s_addr = wangz->abc[i];
			char * fx = inet_ntoa(a);
			strbuffer.Format(TEXT("%s"), CA2T(fx));
			nIndex = m_iphaoma1.InsertItem(i, strbuffer);
			 xiaz.Format(TEXT("%f"), wangz->gailv[i]);
			 m_iphaoma1.SetItemText(nIndex, 1, xiaz);
		}
	}
	else if (wSubCmdID == SUB_fanhuiiDhao)
	{
		
		userIDD* kickuser = static_cast<userIDD*>(pData);
		userIDD kickuser1 = *kickuser;
		PipeSendDataTren(&kickuser1, sizeof(userIDD));
	}
	else if (wSubCmdID == SUB_gengxin1)
	{
		CString strBuffer5;
		CString strBuffer7;
		CString strbuffer8;
		CString strbuffer9;
		CMD_gengxin * a = static_cast<CMD_gengxin*> (pData);

		strBuffer5.Format(TEXT("%lld"), a->daxiaoyu[0]);
		m_EditStockMin.SetWindowText(strBuffer5);
		for (int i = 0; i < strBuffer5.GetLength(); i++)
		{
			switch (strBuffer5[i])
			{
			case 48:
				strbuffer8.Append(TEXT("0"));
				break;
			case 49:
				strbuffer8.Append(TEXT("1"));
				break;
			case 50:
				strbuffer8.Append(TEXT("2"));
				break;
			case 51:
				strbuffer8.Append(TEXT("3"));
				break;
			case 52:
				strbuffer8.Append(TEXT("4"));
				break;
			case 53:
				strbuffer8.Append(TEXT("5"));
				break;
			case 54:
				strbuffer8.Append(TEXT("6"));
				break;
			case 55:
				strbuffer8.Append(TEXT("7"));
				break;
			case 56:
				strbuffer8.Append(TEXT("8"));
				break;
			case 57:
				strbuffer8.Append(TEXT("9"));
				break;
			default:
				break;
			}
		}
		if (strBuffer5.GetLength() > 8)
		{
			strbuffer8.Insert(strBuffer5.GetLength() - 8, TEXT("亿"));
			strbuffer8.Insert(strBuffer5.GetLength() - 3, TEXT("万"));
		}
		else if (strBuffer5.GetLength() > 5)
		{
			strbuffer8.Insert(strBuffer5.GetLength() - 4, TEXT("万"));
		}
		
		m_ZhongWen.SetWindowText(strbuffer8);
		strBuffer7.Format(TEXT("%lld"), a->daxiaoyu[1]);
		m_EditStockMax.SetWindowText(strBuffer7);
		for (int i = 0; i < strBuffer7.GetLength(); i++)
		{
			switch (strBuffer7[i])
			{
			case 48:
				strbuffer9.Append(TEXT("0"));
				break;
			case 49:
				strbuffer9.Append(TEXT("1"));
				break;
			case 50:
				strbuffer9.Append(TEXT("2"));
				break;
			case 51:
				strbuffer9.Append(TEXT("3"));
				break;
			case 52:
				strbuffer9.Append(TEXT("4"));
				break;
			case 53:
				strbuffer9.Append(TEXT("5"));
				break;
			case 54:
				strbuffer9.Append(TEXT("6"));
				break;
			case 55:
				strbuffer9.Append(TEXT("7"));
				break;
			case 56:
				strbuffer9.Append(TEXT("8"));
				break;
			case 57:
				strbuffer9.Append(TEXT("9"));
				break;
			default:
				break;
			}
		}
		if (strBuffer7.GetLength() > 8)
		{
			strbuffer9.Insert(strBuffer7.GetLength() - 8, TEXT("亿"));
			strbuffer9.Insert(strBuffer7.GetLength() - 3, TEXT("万"));
		}
		else if (strBuffer7.GetLength() > 5)
		{
			strbuffer9.Insert(strBuffer7.GetLength() - 4, TEXT("万"));
		}
		m_ZhongWen1.SetWindowText(strbuffer9);

	}
	else if (wSubCmdID == SUB_iphao)
	{   
		ASSERT(wDataSize == sizeof(iphaoshuzu));
		if (wDataSize != sizeof(iphaoshuzu)) return false;
		iphaoshuzu * wangz = static_cast<iphaoshuzu*>(pData);
		m_iphaoma.DeleteAllItems();
		CString strbuffer;
		int nIndex = 0;
		for (int i = 0; i < 30; i++)
		{
			if (wangz->gameid[i] == 0)continue;
			in_addr a;
			a.s_addr = wangz->iphao[i];
			char * fx = inet_ntoa(a);
			strbuffer.Format(TEXT("%s"), CA2T(fx));
			nIndex = m_iphaoma.InsertItem(i, strbuffer);
			strbuffer.Format(TEXT("%d"), wangz->gameid[i]);
			m_iphaoma.SetItemText(nIndex, 1, strbuffer);

		}

	}
	else if (wSubCmdID == SUB_Smorenbaifenbi)
	{
		winlose *a23 = static_cast<winlose*>(pData);
		CString strbuffer;
		strbuffer.Format(TEXT("%lld"), a23->qujianmax);
		K_qujianmax.SetWindowText(strbuffer);
		strbuffer.Format(TEXT("%lld"), a23->qujianmin);
		K_qujianmin.SetWindowText(strbuffer);
		if (a23->var233[0])
		{
			m_buttonqidong.SetWindowText(L"禁用");
			var1 = false; 
			strbuffer.Format(TEXT("%.3f"), a23->win233);
			m_winPercentage.SetWindowText(strbuffer);
			strbuffer.Format(TEXT("%.3f"), a23->lose233);
			m_losePercentage.SetWindowText(strbuffer);
			m_winPercentage.EnableWindow(false);
			m_losePercentage.EnableWindow(false);
		}
		else
		{
			m_buttonqidong.SetWindowText(L"启用");
			var1 = true;
			strbuffer.Format(TEXT("%.3f"), a23->win233);
			m_winPercentage.SetWindowText(strbuffer);
			strbuffer.Format(TEXT("%.3f"), a23->lose233);
			m_losePercentage.SetWindowText(strbuffer);
			m_winPercentage.EnableWindow(true);
			m_losePercentage.EnableWindow(true);
		}
		var2= a23->var233[1];
		var3 = a23->var233[2];
		var4 = a23->var233[3];
		if (!var2)
		{
			var2 = false;
			GetDlgItem(IDC_BTN_qiyong2)->SetWindowText(L"禁用");
			m_UserWinLimit.EnableWindow(false);

		}
		else
		{
			var2 = true;
			GetDlgItem(IDC_BTN_qiyong2)->SetWindowText(L"启用");

			m_UserWinLimit.EnableWindow(true);
		}

		if (!var3)
		{
			GetDlgItem(IDC_BTN_qiyong3)->SetWindowText(L"禁用");

			m_ListStockMin.EnableWindow(false);
			m_ListStockMax.EnableWindow(false);
		
			var3 = false;
		}
		else
		{
			GetDlgItem(IDC_BTN_qiyong3)->SetWindowText(L"启用");
			var3 = true;
			m_ListStockMin.EnableWindow(true);
			m_ListStockMax.EnableWindow(true);
		}
		if (var4)
		{
			GetDlgItem(IDC_BTN_qujianqueding)->SetWindowText(L"禁用");
			var4 = false;
			K_qujianmax.EnableWindow(false);
			K_qujianmin.EnableWindow(false);
		}
		else
		{
			GetDlgItem(IDC_BTN_qujianqueding)->SetWindowText(L"启用");
			var4 = true;
			K_qujianmax.EnableWindow(true);
			K_qujianmin.EnableWindow(true);
		}


	}
	else if (wSubCmdID == SUB_S_FISHTESHU_LIST)
	{
		ASSERT(wDataSize == sizeof(CMD_S_SpecialList));
		if (wDataSize != sizeof(CMD_S_SpecialList)) return false;
		CMD_S_SpecialList* pSpecialList = static_cast<CMD_S_SpecialList*>(pData);

		m_ListTeShuYu.DeleteAllItems();
		CString strBuffer;
		int nIndex = 0;
		for (int i = 0; i<30; i++)
		{
			if (pSpecialList->fish20gameid[i] == 0) continue;
			strBuffer.Format(TEXT("%d"), pSpecialList->fish20gameid[i]);
			nIndex = m_ListTeShuYu.InsertItem(i, strBuffer);

			strBuffer.Format(TEXT("%d"), pSpecialList->fish20gameidkind[i]);
			m_ListTeShuYu.SetItemText(nIndex, 1, TEXT("企鹅"));

			strBuffer.Format(TEXT("%d"), pSpecialList->fish20count[i]);
			m_ListTeShuYu.SetItemText(nIndex, 2, strBuffer);

			strBuffer.Format(TEXT("%d"), (int)(pSpecialList->fish20probability[i] * 1000));
			m_ListTeShuYu.SetItemText(nIndex, 3, strBuffer);
		}

		for (int i = 0; i<30; i++)
		{
			if (pSpecialList->fish21gameid[i] == 0) continue;
			strBuffer.Format(TEXT("%d"), pSpecialList->fish21gameid[i]);
			nIndex = m_ListTeShuYu.InsertItem(i, strBuffer);

			strBuffer.Format(TEXT("%d"), pSpecialList->fish21gameidkind[i]);
			m_ListTeShuYu.SetItemText(nIndex, 1, TEXT("李逵"));

			strBuffer.Format(TEXT("%d"), pSpecialList->fish21count[i]);
			m_ListTeShuYu.SetItemText(nIndex, 2, strBuffer);

			strBuffer.Format(TEXT("%d"), (int)(pSpecialList->fish21probability[i] * 1000));
			m_ListTeShuYu.SetItemText(nIndex, 3, strBuffer);
		}

		for (int i = 0; i<30; i++)
		{
			if (pSpecialList->fishjinsha[i] == 0) continue;
			strBuffer.Format(TEXT("%d"), pSpecialList->fishjinsha[i]);
			nIndex = m_ListTeShuYu.InsertItem(i, strBuffer);

			strBuffer.Format(TEXT("%d"), pSpecialList->fishjinshakind[i]);
			m_ListTeShuYu.SetItemText(nIndex, 1, TEXT("金鲨"));

			strBuffer.Format(TEXT("%d"), pSpecialList->fishjinshacount[i]);
			m_ListTeShuYu.SetItemText(nIndex, 2, strBuffer);

			strBuffer.Format(TEXT("%d"), (int)(pSpecialList->fishjinshaprobability[i] * 1000));
			m_ListTeShuYu.SetItemText(nIndex, 3, strBuffer);

		}
	}
	else if (wSubCmdID == SUB_chuansong)
	{   
		TCHAR * pString = static_cast<TCHAR*>(pData);
		jilu.push_back(pString);

	}
	else if (wSubCmdID == SUB_S_ADMIN_FANGSHUI)
	{
		//ASSERT(wDataSize == sizeof(CMD_S_ADMIN_FANGSHUI));
		if (wDataSize != sizeof(CMD_S_ADMIN_FANGSHUI)) return false;
		CMD_S_ADMIN_FANGSHUI* pAdminFangShui = static_cast<CMD_S_ADMIN_FANGSHUI*>(pData);
		CString strBuffer;
		CString strBuffer1;

		strBuffer.Format(TEXT("%d"), pAdminFangShui->stock_fangshui_edu);
		if (!m_EditFangSHuiEdu.isFocus() && ppp)	{
			m_EditFangSHuiEdu.SetWindowText(strBuffer);
			if (ppp)
				ppp = false;
		}

		strBuffer.Format(TEXT("%d"), pAdminFangShui->fangshuixiaoyukucun);
		if (!m_Editfangshuixiaoyu.isFocus() && ppp3)	{
			m_Editfangshuixiaoyu.SetWindowText(strBuffer);
			ppp3 = false;
		}
		

		strBuffer.Format(TEXT("%d"), pAdminFangShui->fangshuidayukucun);
		if (!m_Editfangshuidayu.isFocus() && ppp4){
			m_Editfangshuidayu.SetWindowText(strBuffer);
			ppp4 = false;
		}

		strBuffer.Format(TEXT("%d"), pAdminFangShui->youxiid1[0]);
		m_youxiID1.SetWindowText(strBuffer);
		strBuffer.Format(TEXT("%d"), pAdminFangShui->youxiid1[1]);
		m_youxiid2.SetWindowText(strBuffer);
		
		/*strBuffer.Format(TEXT("%d"), pAdminFangShui->yizihao1[0]);
		m_yizihao1.SetWindowText(strBuffer);

		strBuffer.Format(TEXT("%d"), pAdminFangShui->yizihao1[1]);
		m_yizihao2.SetWindowText(strBuffer);*/

		strBuffer.Format(TEXT("%d"), pAdminFangShui->zaixianwanjia);
		m_zaixianrenshu.SetWindowText(strBuffer);

		strBuffer.Format(TEXT("%.3f"), 3.0);
		if (!m_EditFangShuiGaiLv.isFocus() && ppp2)	{
			m_EditFangShuiGaiLv.SetWindowText(strBuffer);
			ppp2 = false;

		}
		///在这
		strBuffer.Format(TEXT("%d"), pAdminFangShui->fangshui_time / 24);
		int  xf=pAdminFangShui->fangshui_time / 24;
		if (!m_EditFangShuiTime.isFocus() && ppp1)	{
			m_EditFangShuiTime.SetWindowText(strBuffer);
			ppp1 = false;
		}

			strBuffer.Format(TEXT("%d"), pAdminFangShui->meitian_edu);
				m_EDITPPG.SetWindowText(strBuffer);

			strBuffer.Format(TEXT("%d"), pAdminFangShui->fangshuidayukucun);
			if (!m_Editfangshuidayu.isFocus())
				m_Editfangshuidayu.SetWindowText(strBuffer);

			strBuffer.Format(TEXT("%d"), pAdminFangShui->fangshuixiaoyukucun);
			if (!m_EDITPPG.isFocus())
				m_Editfangshuixiaoyu.SetWindowText(strBuffer);

		SCORE fangshui_score = pAdminFangShui->fangshui_score;
		DWORD fangshui_count_time = pAdminFangShui->fangshui_count_time;
		
		SCORE lastfangshui_score = pAdminFangShui->lastfangshui_score;
		DWORD lastfangshui_count_time = pAdminFangShui->lastfangshui_count_time;
	
		DWORD nextfangshui_time = pAdminFangShui->nextfangshui_time;
		DWORD Begin_time = pAdminFangShui->fangshui_Begin;
		CTime sTime = CTime(nextfangshui_time);
		DWORD PPG = 0;	

		if (panduan)
		{
			if (jilu.size() > 0){
				m_ListFangShui.DeleteAllItems();
				for (int i = 0; i < jilu.size(); i++)
				{

					/*const char *f=*/
					strBuffer = jilu[i];
					m_ListFangShui.InsertItem(aaaaaa++, strBuffer);

				}
				panduan = false;
				jilu.clear();
			}
		}

		if (pAdminFangShui->bFangShuiStart)
		{
			m_ButtonFSStart.EnableWindow(FALSE);
			m_ButtonFSStop.EnableWindow(TRUE);
			m_EditFangSHuiEdu.EnableWindow(FALSE);
			m_EditFangShuiGaiLv.EnableWindow(FALSE);
			m_EditFangShuiTime.EnableWindow(FALSE);
			m_EDITPPG.EnableWindow(FALSE);
			m_Editfangshuixiaoyu.EnableWindow(FALSE);
			m_Editfangshuidayu.EnableWindow(FALSE);
	/*		m_yizihao1.EnableWindow(FALSE);
			m_yizihao2.EnableWindow(FALSE);*/
			m_youxiID1.EnableWindow(FALSE);
			m_youxiid2.EnableWindow(FALSE);
			m_zaixianrenshu.EnableWindow(FALSE);
			if (aaaaaa == 0)
			{
				m_ListFangShui.DeleteAllItems();
				aaaaaa++;
			/*	FILE *wenjian2 = fopen("GameModule//jilu1.txt", "a+");*/
				//if (pAdminFangShui->jiluyong > 0)
				//{
				//	
				//
				//	/*if (jilu.size() > 170)
				//	{
				//	FILE *wenjian2 = fopen("GameModule//jilu1.txt", "w");
				//	fclose(wenjian2);
				//	}*/
				//	/*fclose(wenjian2);*/
				//	
					for (int i = 0; i < jilu.size(); i++)
					{
										
						/*const char *f=*/
						strBuffer = jilu[i];
						 m_ListFangShui.InsertItem(aaaaaa++, strBuffer);

					}
					jilu.clear();
				//}
			}
			else if ((DWORD)(Begin_time + fangshui_count_time+20) == nextfangshui_time)
			{
				CString str1;
				strBuffer.Format(TEXT("%d.%d  %d:%02ld分%02ld秒     %d分 "),  sTime.GetMonth(), sTime.GetDay(),
						sTime.GetHour(), sTime.GetMinute(), sTime.GetSecond(), fangshui_score);
					std::string x1 = CT2CA(strBuffer);
					cha = (SCORE)(pAdminFangShui->fangshuixiaoyukucun1 - pAdminFangShui->fangshuixiaoyukucun);
					he = (SCORE)(pAdminFangShui->fangshuidayukucun1 - pAdminFangShui->fangshuidayukucun);
					str1.Format(TEXT("%I64d分   %I64d分\n"), cha, he);
					
					m_ListFangShui.InsertItem(aaaaaa++, strBuffer+str1);

				/*	FILE *wenjian = fopen("GameModule//jilu1.txt", "a+");
					fprintf(wenjian, "%s", x1);
					fprintf(wenjian, "%d分   ", pAdminFangShui->fangshuixiaoyukucun1 - pAdminFangShui->fangshuixiaoyukucun);
					fprintf(wenjian, "%d分\n", pAdminFangShui->fangshuidayukucun1 - pAdminFangShui->fangshuidayukucun);
					fclose(wenjian);*/
					//jilu.push_back(x1);
					//strBuffer.Format(TEXT("%d"), fangshui_score);
					//m_ListFangShui.SetItemText(nIndex, 1, strBuffer);
				}


			
		}
		else
		{
			m_ButtonFSStart.EnableWindow(TRUE);
			m_ButtonFSStop.EnableWindow(FALSE);
			m_EditFangSHuiEdu.EnableWindow(TRUE);
			m_EditFangShuiGaiLv.EnableWindow(TRUE);
			m_EditFangShuiTime.EnableWindow(TRUE);
			m_EDITPPG.EnableWindow(FALSE);
			m_Editfangshuixiaoyu.EnableWindow(FALSE);
			m_Editfangshuidayu.EnableWindow(FALSE);
			/*m_yizihao1.EnableWindow(FALSE);
			m_yizihao2.EnableWindow(FALSE);*/
			m_youxiID1.EnableWindow(FALSE);
			m_youxiid2.EnableWindow(FALSE);
			m_zaixianrenshu.EnableWindow(FALSE);
		}
		
		if (!var1)
			m_currentkongzhi.SetWindowText(L"输赢百分比概率");
		else 
			m_currentkongzhi.SetWindowText(L"");
		if (!var2)
			m_currentkongzhi1.SetWindowText(L"- 赢分档位概率");
		else
			m_currentkongzhi1.SetWindowText(L"");
		if (!var4)
			m_currentkongzhi3.SetWindowText(L"*区间概率");
		else
			m_currentkongzhi3.SetWindowText(L"");

		if (!var3)
			m_currentkongzhi2.SetWindowText(L"*大小鱼库存档位");
		else
			m_currentkongzhi2.SetWindowText(L"");
		strBuffer.Format(TEXT("\n\t放了:%I64d分\n\t放水大鱼库存:%I64d\n\t放水小鱼库存:%I64d\n\t当前放水持续时间:%d秒\n(服务器的时间)下次放水时间:%d时/%d分/%d秒"),
			fangshui_score, pAdminFangShui->fangshuidayukucun, pAdminFangShui->fangshuixiaoyukucun, fangshui_count_time, sTime.GetHour(), sTime.GetMinute(), sTime.GetSecond());
		
		m_StaticFangShui.SetWindowText(strBuffer);

		
	}
	return true;
}

void CFish_ConfigDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CListCtrl* pThisList=nullptr;
	if (pNMItemActivate->hdr.idFrom == IDC_LIST_FISH)	pThisList = &m_ListFish;
	if (pNMItemActivate->hdr.idFrom == IDC_LIST_STOCKMIN)	pThisList = &m_ListStockMin;
	if (pNMItemActivate->hdr.idFrom == IDC_LIST_STOCKMAX)	pThisList = &m_ListStockMax;
	if (pNMItemActivate->hdr.idFrom == IDC_LIST_USWIN)	pThisList = &m_UserWinLimit;
	m_pActiveList = pThisList;

	CRect rc;
	CString strTemp;
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	m_ListRow = pNMListView->iItem;
	m_ListCol = pNMListView->iSubItem;
	if (pNMListView->iItem == -1)    //选择空白处，添加一行，并设置焦点为最后一行，第二列
	{
		return ;
		m_ListRow = pThisList->GetItemCount();
		strTemp.Format(_T("%d"), m_ListRow + 1);
		pThisList->InsertItem(m_ListRow, strTemp);
		pThisList->SetItemState(m_ListRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		pThisList->EnsureVisible(m_ListRow, FALSE);
		//m_ListCol = 1;  
	}
	if (m_ListCol != 0 || (m_ListCol == 0 && pNMItemActivate->hdr.idFrom != IDC_LIST_FISH)) // 选择子项  
	{
		pThisList->GetSubItemRect(m_ListRow, m_ListCol, LVIR_LABEL, rc);
		m_EditTest.SetParent(pThisList);
		m_EditTest.MoveWindow(rc);
		m_EditTest.SetWindowTextW(pThisList->GetItemText(m_ListRow, m_ListCol));
		m_EditTest.ShowWindow(SW_SHOW);
		m_EditTest.SetFocus();//设置Edit焦点  
		m_EditTest.ShowCaret();//显示光标  
		m_EditTest.SetSel(0, -1);//全选  
	}

	*pResult = 0;
}

void CFish_ConfigDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if (m_pActiveList != nullptr && m_EditTest.m_hWnd != NULL && m_EditTest.IsWindowVisible())
	{
		m_EditTest.ShowWindow(0);
		if (m_ListRow != -1)
		{
			CString text;
			m_EditTest.GetWindowText(text);
			m_pActiveList->SetItemText(m_ListRow, m_ListCol, text);
		}
		m_pActiveList = nullptr;
	}
	m_ListRow = pNMItemActivate->iItem;
	m_ListCol = pNMItemActivate->iSubItem;
	*pResult = 0;
}

void CFish_ConfigDlg::OnLvnBeginScrollList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 此功能要求 Internet Explorer 5.5 或更高版本。
	// 符号 _WIN32_IE 必须是 >= 0x0560。
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);

	if (m_EditTest.m_hWnd != NULL && m_EditTest.IsWindowVisible())
	{
		m_EditTest.ShowWindow(0);
	}
	m_ListCol = m_ListRow = -1;

	*pResult = 0;
}

void CFish_ConfigDlg::OnBnClickedBtnAdd()
{
	CString strGameID=TEXT("");
	GetDlgItem(IDC_EDIT_GAMEID)->GetWindowText(strGameID);
	CString strGaiLv = TEXT("");
	GetDlgItem(IDC_EDIT_GAILV)->GetWindowText(strGaiLv);
	CString strCount = TEXT(""); 
	GetDlgItem(IDC_EDIT_COUNT)->GetWindowText(strCount);


	DWORD game_id = _tstol(strGameID);
	int catch_count = _tstoi(strCount);
	double catch_probability = _ttof(strGaiLv);
	catch_probability/=1000.0;
	if (((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck() == 1)
	{
		CMD_C_Fish20Config Fish20Config;

		Fish20Config.game_id = game_id;
		Fish20Config.catch_count = catch_count;
		Fish20Config.catch_probability = catch_probability;
		PipeSendData(SUB_C_FISH20_CONFIG, &Fish20Config, sizeof(Fish20Config));
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck() == 1)
	{
		CMD_C_Fish20Config fish21_config;
		fish21_config.game_id = game_id;
		fish21_config.catch_count = catch_count;
		fish21_config.catch_probability = catch_probability;
		PipeSendData(SUB_C_FISH21_CONFIG, &fish21_config, sizeof(fish21_config));
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO3))->GetCheck() == 1)
	{
		CMD_C_Fish20Config fishjinsha_config;
		fishjinsha_config.game_id = game_id;
		fishjinsha_config.catch_count = catch_count;
		fishjinsha_config.catch_probability = catch_probability;
		PipeSendData(SUB_C_FISHJINSHA_CONFIG, &fishjinsha_config, sizeof(fishjinsha_config));
	}
	OnBnClickedButtonquery();
}

void CFish_ConfigDlg::OnBnClickedButtonquery()
{
	CMD_C_ChackList chack_list;
	chack_list.operate_code = 2;
	PipeSendData(SUB_C_FISHTESHU_LIST, &chack_list, sizeof(chack_list));
}

void CFish_ConfigDlg::OnBnClickedButtonDelete()
{
	CString str;
	int z = m_ListTeShuYu.GetItemCount();
	for (int i = 0; i<z; i++)
	{
		if (m_ListTeShuYu.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED /*|| m_ListTeShuYu.GetCheck(i)*/)
		{
			CMD_C_UserFilter UserFilter;
			UserFilter.operate_code = 3;
			DWORD game_id = _tstol(m_ListTeShuYu.GetItemText(i, 0));
 			UserFilter.game_id = game_id;
			PipeSendData(SUB_C_USER_FILTER, &UserFilter, sizeof(UserFilter));
			break;
		}
	}

	OnBnClickedButtonquery();
}

void CFish_ConfigDlg::OnBnClickedBtnAdd2()
{
	CString strGameID = TEXT("");
	GetDlgItem(IDC_EDIT_GAMEID2)->GetWindowText(strGameID);

	DWORD game_id = _tstol(strGameID);
	if (((CButton *)GetDlgItem(IDC_RADIO4))->GetCheck() == 1)
	{
		CMD_C_UserFilter user_filter;
		user_filter.game_id = game_id;
		user_filter.operate_code = 1;
		PipeSendData(SUB_C_USER_FILTER, &user_filter, sizeof(user_filter));
	}
	else if (((CButton *)GetDlgItem(IDC_RADIO5))->GetCheck() == 1)
	{
		CMD_C_UserFilter user_filter;
		user_filter.game_id = game_id;
		user_filter.operate_code = 0;
		PipeSendData(SUB_C_USER_FILTER, &user_filter, sizeof(user_filter));
	}
	OnBnClickedButtonquery2();
}

void CFish_ConfigDlg::OnBnClickedButtonDelete2()
{
	CString str;
	int z = m_ListHBMD.GetItemCount();
	for (int i = 0; i<z; i++)
	{
		if (m_ListHBMD.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			CMD_C_UserFilter UserFilter;
			UserFilter.operate_code = 2;
			DWORD game_id = _tstol(m_ListHBMD.GetItemText(i, 0));
			UserFilter.game_id = game_id;
			PipeSendData(SUB_C_USER_FILTER, &UserFilter, sizeof(UserFilter));
			break;
		}
	}
	OnBnClickedButtonquery2();
}

void CFish_ConfigDlg::OnBnClickedBtnAdd3()
{
	CString strGameIP = TEXT("");
	CString strgailv = TEXT("");
	tianjiashanchu wangz = {};
	GetDlgItem(IDC_IPhao)->GetWindowText(strGameIP);
	GetDlgItem(IDC_Gailv2)->GetWindowText(strgailv);
	wangz.gailv = _ttof(strgailv);
		string cd=CT2CA(strGameIP);
		
		wangz.abc=inet_addr(cd.c_str());
		
		PipeSendData(SUB_C_ip1, &wangz, sizeof(wangz));
}
void CFish_ConfigDlg::OnBnClickedButtonDelete3(){
	CString strGameIP = TEXT("");
	int z = m_iphaoma1.GetItemCount();
	for (int i = 0; i<z; i++)
	{
		if (m_iphaoma1.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			
			auto game_ip = m_iphaoma1.GetItemText(i, 0);
			string w=CT2CA(game_ip);
			auto xw=inet_addr(w.c_str());
			PipeSendData(SUB_C_ip2, &xw, sizeof(DWORD));
			break;
		}
	}
}

void CFish_ConfigDlg::OnBnClickedButtonquery2()
{
	CMD_C_ChackList chack_list;
	chack_list.operate_code = 2;
	PipeSendData(SUB_C_HEIBAI_LIST, &chack_list, sizeof(chack_list));
}


void CFish_ConfigDlg::OnBnClickedBtnLoad()
{
	//CDialogEx::OnOK();
	PipeSendData(SUB_C_ip, 0, 0);
	PipeSendData(SUB_C_ADMIN_CONFIG, 0, 0);
}


void CFish_ConfigDlg::OnBnClickedBtnSave()
{
	CMD_S_ADMIN_CONFIG AdminConfig;
	CString strBuffer;
	CString strBuffer1;
	CString strBuffer2;
	CString strBuffer3;
	CString strBuffer4;
	CString strBuffer5;
	CString strBuffer6;
	CString strBuffer7;
	m_EditRevenuebfb.GetWindowText(strBuffer);
	double total_return_rate_ = _ttof(strBuffer);						//返还百分比
	m_EditRevenue.GetWindowText(strBuffer);
	SCORE revenue_score = _ttoll(strBuffer);								//游戏抽水
	m_EditExChangeUser.GetWindowText(strBuffer);
	int exchange_ratio_userscore_ = _ttoi(strBuffer);					//金币兑换比例
	m_EditExChangeFish.GetWindowText(strBuffer);
	int exchange_ratio_fishscore_ = _ttoi(strBuffer);					//鱼币兑换比例
	m_EditExchangeCount.GetWindowText(strBuffer);
	int exchange_fishscore_count_ = _ttoi(strBuffer);					//单次兑换数量
	m_EditMinBullet.GetWindowText(strBuffer);
	int bullet_multiple_min = _ttoi(strBuffer);							//子弹最小值
	m_EditMaxBullet.GetWindowText(strBuffer);
	int bullet_multiple_max = _ttoi(strBuffer);							//子弹最大值
	m_EditStockMin.GetWindowText(strBuffer);
	int stock_score_min = _ttoi(strBuffer);		//库存最小值
	m_EditStockMax.GetWindowText(strBuffer);
	int stock_score_max = _ttoi(strBuffer);								//库存最大值
	

	m_easy.GetWindowText(strBuffer);
	double t_easy = _ttof(strBuffer);

	m_hard.GetWindowText(strBuffer);
	double t_hard = _ttof(strBuffer);

	m_bodongshijian.GetWindowText(strBuffer);
	int time_a = _ttoi(strBuffer);

	m_bodongmax.GetWindowText(strBuffer);
	int time_b = _ttoi(strBuffer);

	m_zhengtigailv.GetWindowText(strBuffer);     //整体概率
	double stock_zhengti = _ttof(strBuffer);

	AdminConfig.total_return_rate_ = total_return_rate_;
	AdminConfig.revenue_score = revenue_score;
	AdminConfig.exchange_ratio_userscore_ = exchange_ratio_userscore_;
	AdminConfig.exchange_ratio_fishscore_ = exchange_ratio_fishscore_;
	AdminConfig.exchange_fishscore_count_ = exchange_fishscore_count_;
	AdminConfig.bullet_multiple[0] = bullet_multiple_min;
	AdminConfig.bullet_multiple[1] = bullet_multiple_max;
	AdminConfig.stock_score[0] = stock_score_min;
	AdminConfig.stock_score[1] = stock_score_max;
	AdminConfig.zhengtigailv = stock_zhengti;
	AdminConfig.bodongshijian = time_a;
	AdminConfig.hard = t_hard;
	AdminConfig.easy = t_easy;
	AdminConfig.bodongmax = time_b;

	


	for (int i = 0; i < 12; i++)
	{
	strBuffer6 = m_ListStockMax.GetItemText(i, 0);
		AdminConfig.stock_crucial_score_[i] = _ttoi(strBuffer6);

		strBuffer5 = m_ListStockMax.GetItemText(i, 1);
		AdminConfig.stock_increase_probability_[i] = _ttof(strBuffer5);

		strBuffer4 = m_ListStockMin.GetItemText(i, 0);
		AdminConfig.stock_crucial_score_small[i] = _ttoi(strBuffer4);

		strBuffer3 = m_ListStockMin.GetItemText(i, 1);
		AdminConfig.stock_increase_probability_small[i] = _ttof(strBuffer3);
	}

	for (int i = 0; i < FISH_KIND_COUNT; i++)
	{
		strBuffer2 = m_ListFish.GetItemText(i, 1);
		AdminConfig.nFishMinMultiple[i] = _ttoi(strBuffer2);

		strBuffer1 = m_ListFish.GetItemText(i, 2);
		AdminConfig.nFishMaxMultiple[i] = _ttoi(strBuffer1);

		strBuffer6 = m_ListFish.GetItemText(i, 3);
		AdminConfig.nFishSpeed[i] = _ttoi(strBuffer6);

		strBuffer = m_ListFish.GetItemText(i, 4);
		AdminConfig.fFishGailv[i] = _ttof(strBuffer);
	}
	

	for (int i = 0; i < 20; i++)
	{
		strBuffer5 = m_UserWinLimit.GetItemText(i, 0);
		AdminConfig.win_score_limit_[i] = _ttoi(strBuffer5);

		strBuffer4 = m_UserWinLimit.GetItemText(i, 1);
		AdminConfig.win_decrease_probability_[i] = _ttof(strBuffer4);
	}
	PipeSendData(SUB_C_ADMIN_CONFIG, &AdminConfig, sizeof(AdminConfig));
}


void CFish_ConfigDlg::OnBnClickedBtnFsStart()
{
	CString strBuffer;

	CMD_S_ADMIN_FANGSHUI AdminFangShui;
	panduan2 = true;
	AdminFangShui = {};

	//m_Editfangshuidayu.GetWindowText(strBuffer);
	//AdminFangShui.fangshuidayukucun = _ttol(strBuffer);				//放水大鱼库存

	//m_Editfangshuixiaoyu.GetWindowText(strBuffer);
	//AdminFangShui.fangshuixiaoyukucun = _ttol(strBuffer);			//放水小鱼库存



	m_EditFangSHuiEdu.GetWindowText(strBuffer);
	AdminFangShui.stock_fangshui_edu = _ttoll(strBuffer);			//放水额度



	strBuffer.Format(TEXT("%d"), AdminFangShui.stock_fangshui_edu);
	m_EditFangSHuiEdu.SetWindowText(strBuffer);

	m_EditFangShuiGaiLv.GetWindowText(strBuffer);
	AdminFangShui.stock_fangshui_probability = _ttof(strBuffer);	//放水概率

	m_EditFangShuiTime.GetWindowText(strBuffer);
	AdminFangShui.fangshui_time = _tcstoul(strBuffer,NULL,10);					//放水时间
	LPCTSTR app = L"天数至少为1天!";
	if (AdminFangShui.fangshui_time > 0)
		//每天额度
		AdminFangShui.meitian_edu = AdminFangShui.stock_fangshui_edu / AdminFangShui.fangshui_time;
	else
	{
		AdminFangShui.fangshui_time = 0;

		MessageBox(app);
	}

	CTime bbbbbbf = CTime::GetCurrentTime().GetTime();
	//AdminFangShui.stock_fangshui_edu = AdminFangShui.stock_fangshui_edu - AdminFangShui.meitian_edu;

	AdminFangShui.fangshui_time = AdminFangShui.fangshui_time * 24 - bbbbbbf.GetHour();
	AdminFangShui.bFangShuiStart=true;
	if (AdminFangShui.stock_fangshui_edu <= 0 || AdminFangShui.fangshui_time>=10000)
		AdminFangShui.bFangShuiStart = false;
	PipeSendData(SUB_C_ADMIN_FANGSHUI, &AdminFangShui, sizeof(AdminFangShui));
}


void CFish_ConfigDlg::OnBnClickedBtnFsStop()
{
	CString strBuffer;
	if (panduan2)
	{
		m_ListFangShui.DeleteAllItems();
		panduan2 = false;
	}
	aaaaaa=0;
	CMD_S_ADMIN_FANGSHUI AdminFangShui;

	m_EditFangSHuiEdu.GetWindowText(strBuffer);
	AdminFangShui.stock_fangshui_edu = _ttol(strBuffer);			//放水额度

	m_Editfangshuidayu.GetWindowText(strBuffer);
	AdminFangShui.fangshuidayukucun = _ttol(strBuffer);				//放水大鱼库存

	m_Editfangshuixiaoyu.GetWindowText(strBuffer);
	AdminFangShui.fangshuixiaoyukucun = _ttol(strBuffer);			//放水小鱼库存

	m_EditFangShuiGaiLv.GetWindowText(strBuffer);
	AdminFangShui.stock_fangshui_probability = _ttof(strBuffer);	//放水概率

	m_EditFangShuiTime.GetWindowText(strBuffer);
	AdminFangShui.fangshui_time = _ttol(strBuffer);					//放水时间

	m_EDITPPG.GetWindowText(strBuffer);   //每天额度
	AdminFangShui.meitian_edu = _ttol(strBuffer);

	AdminFangShui.bFangShuiStart = false;
	PipeSendData(SUB_C_ADMIN_FANGSHUI, &AdminFangShui, sizeof(AdminFangShui));
}







void CFish_ConfigDlg::OnBnClickedButtonqueidng()
{
	CString strBuffer6;
	pao10086 abc = {};
	double p1[10];
	m_pao1.GetWindowText(strBuffer6);
	 p1[0] = _ttof(strBuffer6);

	m_pao2.GetWindowText(strBuffer6);
	p1[1] = _ttof(strBuffer6);

	m_pao3.GetWindowText(strBuffer6);
	p1[2] = _ttof(strBuffer6);

	m_pao4.GetWindowText(strBuffer6);
	p1[3] = _ttof(strBuffer6);

	m_pao5.GetWindowText(strBuffer6);
	p1[4] = _ttof(strBuffer6);

	m_pao6.GetWindowText(strBuffer6);
	p1[5] = _ttof(strBuffer6);

	m_pao7.GetWindowText(strBuffer6);
	p1[6] = _ttof(strBuffer6);

	m_pao8.GetWindowText(strBuffer6);
	p1[7] = _ttof(strBuffer6);

	m_pao9.GetWindowText(strBuffer6);
	p1[8] = _ttof(strBuffer6);

	m_pao10.GetWindowText(strBuffer6);
	p1[9] = _ttof(strBuffer6);
	
	for (int i = 0; i < 10; i++)
	{
		abc.paogailv[i] = p1[i];

	}
	PipeSendData(SUB_pao,&abc,sizeof(abc));

}


void CFish_ConfigDlg::OnBnClickedButtonqueidng2()
{
	CString strbuffer7;
	m_tichu.GetWindowText(strbuffer7);
	
	DWORD abc = _tstol(strbuffer7);
	PipeSendData(sub_tichu, &abc, sizeof(abc));


}





void CFish_ConfigDlg::OnBnClickedButtonjiqiren1()
{
	CString strbuffer;
	qijiren abc;
	m_jiqiren.GetWindowText(strbuffer);
	abc.time1[0] = _tstol(strbuffer);
	m_jiqiren2.GetWindowText(strbuffer);
	abc.time1[1] = _tstol(strbuffer);
	PipeSendData(SUB_jiqiren, &abc, sizeof(abc));
}


void CFish_ConfigDlg::OnBnClickedButtonjiqiren2()
{
	CString strbuffer;
	double abc;
	m_jiqiren3.GetWindowText(strbuffer);

	abc = _ttof(strbuffer);
	if (abc <= 0)
	{
		LPCTSTR app = L"概率不能为0!";
		MessageBox(app);
		return;
	}
	else
	PipeSendData(SUB_jiqirengailv, &abc, sizeof(abc));
	// TODO:  在此添加控件通知处理程序代码
}


void CFish_ConfigDlg::OnBnClickedButtonlikaiqueding()
{
	CString  strbuffer;
	qijiren abc;
	m_minleave.GetWindowText(strbuffer);
	abc.time1[0] = _tstol(strbuffer);
	m_maxleave.GetWindowText(strbuffer);
	abc.time1[1] = _tstol(strbuffer);

	PipeSendData(SUB_jiqirenjinchu, &abc, sizeof(abc));

}


void CFish_ConfigDlg::OnBnClickedBtnfuzhi()
{
	CString strGameIP = TEXT("");
	int z = m_iphaoma.GetItemCount();
	for (int i = 0; i<z; i++)
	{
		if (m_iphaoma.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{

			auto game_ip = m_iphaoma.GetItemText(i, 0);
			GetDlgItem(IDC_IPhao)->SetWindowText(game_ip);
			/*string w = CT2CA(game_ip);
			auto xw = inet_addr(w.c_str());
			PipeSendData(SUB_C_ip2, &xw, sizeof(DWORD));*/
			break;
		}
	}
}

void CFish_ConfigDlg::OnBnClickedButtonxiugai()
{
	CString str1 = L"";
	CString str2 = L"";
	m_winPercentage.GetWindowText(str1);
	m_losePercentage.GetWindowText(str2);
	winlose abc = {};
	
	abc.win233 = _ttof(str1);
	abc.lose233 = _ttof(str2);
	if (abc.win233 + abc.lose233 > 1){ MessageBox(L"不能超过100%"); return; }
	if (var1)
	{
		m_buttonqidong.SetWindowText(L"禁用");
		m_winPercentage.EnableWindow(false);
		m_losePercentage.EnableWindow(false);
		abc.var233[0] = true;
		var1 = false;
	}
	else
	{
		m_buttonqidong.SetWindowText(L"启用");
		m_winPercentage.EnableWindow(true);
		m_losePercentage.EnableWindow(true);
		abc.var233[0] = false;
		var1 = true;
	}
	PipeSendData(SUB_tiaozheng, &abc, sizeof(abc));
	
}


void CFish_ConfigDlg::OnBnClickedBtnqiyong2()
{
	if (var2)
	{
		var2 = false;
		GetDlgItem(IDC_BTN_qiyong2)->SetWindowText(L"禁用");
		m_UserWinLimit.EnableWindow(false);
		
	}
	else
	{
		var2 = true;
		GetDlgItem(IDC_BTN_qiyong2)->SetWindowText(L"启用");

		m_UserWinLimit.EnableWindow(true);
	}
	PipeSendData(SUB_winlose, &var2, sizeof(bool));

}


void CFish_ConfigDlg::OnBnClickedBtnqiyong3()
{
	if (var3)
	{
		GetDlgItem(IDC_BTN_qiyong3)->SetWindowText(L"禁用");

		m_ListStockMin.EnableWindow(false);
		m_ListStockMax.EnableWindow(false);
		var3 = false;
	}
	else
	{
		GetDlgItem(IDC_BTN_qiyong3)->SetWindowText(L"启用");
		var3 = true;
		m_ListStockMin.EnableWindow(true);
		m_ListStockMax.EnableWindow(true);
	}
	PipeSendData(SUB_kucun, &var3,sizeof(bool));
}




void CFish_ConfigDlg::OnBnClickedBtnqujianqueding()
{
	CString afg;
	qujian arg2;
	K_qujianmax.GetWindowText(afg);
	arg2.qujianmax = _ttoll(afg);
	K_qujianmin.GetWindowText(afg);
	arg2.qujianmin = _ttoll(afg);
	arg2.panduan = var4;
	if (var4)
	{
		var4 = false;
		GetDlgItem(IDC_BTN_qujianqueding)->SetWindowText(L"禁用");
		K_qujianmax.EnableWindow(false);
		K_qujianmin.EnableWindow(false);
	}
	else
	{	
		var4 = true;
		GetDlgItem(IDC_BTN_qujianqueding)->SetWindowText(L"启用");
		K_qujianmax.EnableWindow(true);
		K_qujianmin.EnableWindow(true);
	}
	PipeSendData(SUB_C_qujian, &arg2, sizeof(arg2));
	// TODO:  在此添加控件通知处理程序代码
}


void CFish_ConfigDlg::OnBnClickedButtonqueidng3()
{
	CString var1;
	double p1[8];
	m_weizhi1.GetWindowText(var1);
	p1[0] = _ttof(var1);
	m_weizhi2.GetWindowText(var1);
	p1[1] = _ttof(var1);
	m_weizhi3.GetWindowText(var1);
	p1[2] = _ttof(var1);
	m_weizhi4.GetWindowText(var1);
	p1[3] = _ttof(var1);
	m_weizhi5.GetWindowText(var1);
	p1[4] = _ttof(var1);
	m_weizhi6.GetWindowText(var1);
	p1[5] = _ttof(var1);
	m_weizhi7.GetWindowText(var1);
	p1[6] = _ttof(var1);
	m_weizhi8.GetWindowText(var1);
	p1[7] = _ttof(var1);
	PipeSendData(SUB_weizhigailv, p1, sizeof(p1));

}
