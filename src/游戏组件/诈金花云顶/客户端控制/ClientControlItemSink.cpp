#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"

#define TIME_ROOMINFO							500	    //500����ˢ��һ��
#define IDI_REQUEST_UPDATE_ROOMINFO				111		//����ʱ��

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_STORAGE, OnUpdateStorage)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_STORAGE, OnModifyStorage)
	ON_BN_CLICKED(IDC_BT_CONTINUE_WIN, OnContinueControlWin)
	ON_BN_CLICKED(IDC_BT_CONTINUE_LOST, OnContinueControlLost)
	ON_BN_CLICKED(IDC_BT_CONTINUE_CANCEL, OnContinueControlCancel)
	ON_BN_CLICKED(IDC_BTN_USER_BET_QUERY, OnBnClickedBtnUserBetQuery)
	ON_EN_SETFOCUS(IDC_EDIT_USER_ID, OnEnSetfocusEditUserId)
	ON_BN_CLICKED(IDC_RADIO_GAMEID, OnQueryUserGameID)
	ON_BN_CLICKED(IDC_RADIO_NICKNAME, OnQueryUserNickName)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
	: IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
	ZeroMemory(m_lMaxRoomStorage,sizeof(m_lMaxRoomStorage));
	ZeroMemory(m_wRoomStorageMul,sizeof(m_wRoomStorageMul));
	m_lRoomStorageCurrent = 0;
	m_lRoomStorageDeduct = 0;

	m_dwQueryUserGameID = INVALID_WORD;
	ZeroMemory(m_szQueryUserNickName, sizeof(m_szQueryUserNickName));
	m_QueryType = QUERY_INVALID;
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	IClientControlDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_UPDATE_STORAGE, m_btUpdateStorage);
	DDX_Control(pDX, IDC_BUTTON_MODIFY_STORAGE, m_btModifyStorage);
	DDX_Control(pDX, IDC_BT_CONTINUE_WIN, m_btContinueWin);
	DDX_Control(pDX, IDC_BT_CONTINUE_LOST, m_btContinueLost);
	DDX_Control(pDX, IDC_BT_CONTINUE_CANCEL, m_btContinueCancel);
	DDX_Control(pDX, IDC_BTN_USER_BET_QUERY, m_btQueryUser);
	DDX_Control(pDX, IDC_EDIT_ROOMCURRENT_STORAGE, m_editCurrentStorage);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_DEDUCT, m_editStorageDeduct);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MAX1, m_editStorageMax1);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MUL1, m_editStorageMul1);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MAX2, m_editStorageMax2);
	DDX_Control(pDX, IDC_EDIT_ROOMSTORAGE_MUL2, m_editStorageMul2);
	DDX_Control(pDX, IDC_EDIT_USER_ID, m_editUserID);
	DDX_Control(pDX, IDC_EDIT_CONTROL_COUNT, m_editControlCount);
	DDX_Control(pDX, IDC_RICHEDIT_USERDESCRIPTION, m_richEditUserDescription);
	DDX_Control(pDX, IDC_RICHEDIT_USERCONTROL, m_richEditUserControl);
	DDX_Control(pDX, IDC_RICHEDIT_OPERATION_RECORD, m_richEditOperationRecord);
}

//��ʼ��
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	SetWindowText(TEXT("���ƹ�����"));

	SetDlgItemText(IDC_BUTTON_UPDATE_STORAGE,TEXT("���¿��"));
	SetDlgItemText(IDC_BUTTON_MODIFY_STORAGE,TEXT("�޸�����"));

	//���Ʒ�Χ
	m_editStorageDeduct.LimitText(3);
	m_editStorageMul1.LimitText(2);
	m_editStorageMul2.LimitText(2);
	m_editCurrentStorage.LimitText(13);
	m_editStorageMax1.LimitText(13);
	m_editStorageMax2.LimitText(13);
	m_editControlCount.LimitText(1);

	SetDlgItemText(IDC_EDIT_USER_ID, TEXT("���������ID���ǳ�"));
	GetDlgItem(IDC_EDIT_USER_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_USER_BET_QUERY)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

	m_richEditUserDescription.EnableWindow(FALSE);
	m_richEditUserControl.EnableWindow(FALSE);
	m_richEditOperationRecord.EnableWindow(FALSE);

	return TRUE; 
}

//������ʼ���
void CClientControlItemSinkDlg::SetRoomStorage(LONGLONG lRoomStartStorage, LONGLONG lRoomCurrentStorage)
{
	CString strBuf;
	strBuf.Format(SCORE_STRING, lRoomStartStorage);
	GetDlgItem(IDC_STATIC_ROOMSTART_STORAGE)->SetWindowText(strBuf);

	strBuf.Format(SCORE_STRING, lRoomCurrentStorage);
	GetDlgItem(IDC_STATIC_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);

	//strBuf.Format(SCORE_STRING, lRoomCurrentStorage);
	//GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
}

void CClientControlItemSinkDlg::SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo)
{
	CString strBuf;
	//������ʼ���
	strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageStart);
	GetDlgItem(IDC_STATIC_ROOMSTART_STORAGE)->SetWindowText(strBuf);
	//��ǰ���
	strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageCurrent);
	GetDlgItem(IDC_STATIC_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
	//��ǰ���
	//strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageCurrent);
	//GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
	//��ǰ˥��
	strBuf.Format(SCORE_STRING, pStorageInfo->lRoomStorageDeduct);
	GetDlgItem(IDC_EDIT_ROOMSTORAGE_DEDUCT)->SetWindowText(strBuf);
	//�������1
	strBuf.Format(SCORE_STRING, pStorageInfo->lMaxRoomStorage[0]);
	GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX1)->SetWindowText(strBuf);
	//�·ָ���
	strBuf.Format(TEXT("%d"), pStorageInfo->wRoomStorageMul[0]);
	GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL1)->SetWindowText(strBuf);
	//�������2
	strBuf.Format(SCORE_STRING, pStorageInfo->lMaxRoomStorage[1]);
	GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX2)->SetWindowText(strBuf);
	//�·ָ���
	strBuf.Format(TEXT("%d"), pStorageInfo->wRoomStorageMul[1]);
	GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL2)->SetWindowText(strBuf);
	

	//������ֵ
	m_lRoomStorageCurrent = pStorageInfo->lRoomStorageCurrent;
	m_lRoomStorageDeduct = pStorageInfo->lRoomStorageDeduct;
	CopyMemory(m_lMaxRoomStorage, pStorageInfo->lMaxRoomStorage,sizeof(m_lMaxRoomStorage));
	CopyMemory(m_wRoomStorageMul, pStorageInfo->wRoomStorageMul,sizeof(m_wRoomStorageMul));
}

HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_STORAGEINFO || pWnd->GetDlgCtrlID() == IDC_STATIC_ROOMSTART_STORAGE
	 || pWnd->GetDlgCtrlID() == IDC_STATIC_ROOMCURRENT_STORAGE)
	{
		pDC->SetTextColor(RGB(255,10,10));
	}
	return hbr;
}

//�޸Ŀ������
void CClientControlItemSinkDlg::OnModifyStorage()
{
	CMD_C_ModifyStorage modifyStorage;
	ZeroMemory(&modifyStorage, sizeof(modifyStorage));

	CString strlMaxStorage0;
	CString strlMaxStorage1;
	GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MAX1, strlMaxStorage0);
	GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MAX2, strlMaxStorage1);

	CString strMul0;
	CString strMul1;
	GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MUL1, strMul0);
	GetDlgItemText(IDC_EDIT_ROOMSTORAGE_MUL2, strMul1);

	//У���Ƿ�Ϊ��ֵ
	if(strlMaxStorage0 == TEXT("") || strlMaxStorage1 == TEXT("")
		|| strMul0 == TEXT("") || strMul1 == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("�������ֵ�����������ã�"));
		//��ԭԭ��ֵ
		CString strBuf;
		strBuf.Format(TEXT("%d"), m_lMaxRoomStorage[0]);
		GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX1)->SetWindowText(strBuf);
		//�·ָ���
		strBuf.Format(TEXT("%d"),m_wRoomStorageMul[0]);
		GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL1)->SetWindowText(strBuf);
		//�������2
		strBuf.Format(TEXT("%I64d"),m_lMaxRoomStorage[1]);
		GetDlgItem(IDC_EDIT_ROOMSTORAGE_MAX2)->SetWindowText(strBuf);
		//�·ָ���
		strBuf.Format(TEXT("%d"),m_wRoomStorageMul[1]);
		GetDlgItem(IDC_EDIT_ROOMSTORAGE_MUL2)->SetWindowText(strBuf);
		return;		
	}

	strlMaxStorage0.TrimLeft();
	strlMaxStorage0.TrimRight();
	strlMaxStorage1.TrimLeft();
	strlMaxStorage1.TrimRight();
	LPTSTR lpsz1 = new TCHAR[strlMaxStorage0.GetLength()+1];
	_tcscpy(lpsz1,strlMaxStorage0);
	modifyStorage.lMaxRoomStorage[0] = _ttoi64(lpsz1);
	delete lpsz1;

	LPTSTR lpsz2 = new TCHAR[strlMaxStorage1.GetLength()+1];
	_tcscpy(lpsz2,strlMaxStorage1);
	modifyStorage.lMaxRoomStorage[1] = _ttoi64(lpsz2);
	delete lpsz2;

	modifyStorage.wRoomStorageMul[0] = GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL1);
	modifyStorage.wRoomStorageMul[1] = GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_MUL2);

	//У��Ϸ���
	if(modifyStorage.wRoomStorageMul[0] <0 || modifyStorage.wRoomStorageMul[0] >100 || 
		modifyStorage.wRoomStorageMul[1] <0 || modifyStorage.wRoomStorageMul[1] >100)
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("Ӯ�ָ��ʷ�Χ0-100"));
		return;		
	}
	if( modifyStorage.lMaxRoomStorage[0]<=0 ||  modifyStorage.lMaxRoomStorage[0]>=modifyStorage.lMaxRoomStorage[1] )
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("�������Ӧ�ô���0������ֵ2Ӧ�ô�������ֵ1�����������ã�"));
		return;
	}
	else if ( modifyStorage.wRoomStorageMul[0]<=0 ||  modifyStorage.wRoomStorageMul[0]>=modifyStorage.wRoomStorageMul[1] )
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("Ӯ�ָ���Ӧ�ô���0��Ӯ�ָ���ֵ2Ӧ�ô���Ӯ�ָ���ֵ1"));
		return;
	}
	else
	{
		CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
		if (pGameFrameEngine!=NULL)
		{
			pGameFrameEngine->SendMessage(IDM_ADMIN_MODIFY_STORAGE, (WPARAM)&modifyStorage, 0);
		}
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("�޸ĳɹ�"));

		CopyMemory(m_wRoomStorageMul,modifyStorage.wRoomStorageMul,sizeof(m_wRoomStorageMul));
		CopyMemory(m_lMaxRoomStorage,modifyStorage.lMaxRoomStorage,sizeof(m_lMaxRoomStorage));
	}
}

//���¿��
void CClientControlItemSinkDlg::OnUpdateStorage()
{
	CString str;
	CMD_C_UpdateStorage UpdateStorage;
	ZeroMemory(&UpdateStorage, sizeof(UpdateStorage));

	CString strStorageCurrent;
	GetDlgItemText(IDC_EDIT_ROOMCURRENT_STORAGE, strStorageCurrent);
	CString strStorageDudect;
	GetDlgItemText(IDC_EDIT_ROOMSTORAGE_DEDUCT, strStorageDudect);

	//У���Ƿ�Ϊ��ֵ
	if(strStorageCurrent == TEXT("") || strStorageDudect == TEXT(""))
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("�������ֵ�����������ã�"));
		CString strBuf;
		//��ǰ���
		strBuf.Format(TEXT("%I64d"), m_lRoomStorageCurrent);
		GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);
		//��ǰ˥��
		strBuf.Format(TEXT("%I64d"), m_lRoomStorageDeduct);
		GetDlgItem(IDC_EDIT_ROOMSTORAGE_DEDUCT)->SetWindowText(strBuf);

		return;		
	}
	strStorageCurrent.TrimLeft();
	strStorageCurrent.TrimRight();
	LPTSTR lpsz = new TCHAR[strStorageCurrent.GetLength()+1];
	_tcscpy(lpsz,strStorageCurrent);
	UpdateStorage.lRoomStorageCurrent = _ttoi64(lpsz);
	UpdateStorage.lRoomStorageDeduct = (SCORE)GetDlgItemInt(IDC_EDIT_ROOMSTORAGE_DEDUCT);	
	delete lpsz;
	//У��Ϸ���
	if(!(UpdateStorage.lRoomStorageDeduct >= 0 && UpdateStorage.lRoomStorageDeduct < 1000))
	{
		GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(TEXT("���˥����Χ0-1000"));
		return;
	}
	CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
	if (pGameFrameEngine != NULL) 
	{
		pGameFrameEngine->SendMessage(IDM_ADMIN_UPDATE_STORAGE, (WPARAM)&UpdateStorage, 0);	
	}
	str=TEXT("����Ѹ��£�");
	m_lRoomStorageCurrent = UpdateStorage.lRoomStorageCurrent;
	m_lRoomStorageDeduct = UpdateStorage.lRoomStorageDeduct;
	GetDlgItem(IDC_STATIC_STORAGEINFO)->SetWindowText(str);
	GetDlgItem(IDC_EDIT_ROOMCURRENT_STORAGE)->SetWindowText(TEXT(""));
}

BOOL CClientControlItemSinkDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE)
		{
			return true;
		}
	}
	return false;
}

void CClientControlItemSinkDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void CClientControlItemSinkDlg::OnEnSetfocusEditUserId()
{
	SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
	m_richEditUserDescription.CleanScreen();
	m_richEditUserControl.CleanScreen();
	SetDlgItemText(IDC_EDIT_CONTROL_COUNT,TEXT(""));
	GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

	m_dwQueryUserGameID = INVALID_WORD;
	ZeroMemory(m_szQueryUserNickName, sizeof(m_szQueryUserNickName));

	CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
	if (pGameFrameEngine != NULL)
	{
		pGameFrameEngine->SendMessage(IDM_CLEAR_CURRENT_QUERYUSER, 0, 0);
	}
}

void CClientControlItemSinkDlg::OnQueryUserGameID()
{
	CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
	if (pGameFrameEngine != NULL)
	{
		pGameFrameEngine->SendMessage(IDM_CLEAR_CURRENT_QUERYUSER, 0, 0);
	}

	SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
	m_richEditUserDescription.CleanScreen();
	m_richEditUserControl.CleanScreen();
	SetDlgItemText(IDC_EDIT_CONTROL_COUNT,TEXT(""));
	GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

	m_QueryType = QUERY_USER_GAMEID;

	GetDlgItem(IDC_EDIT_USER_ID)->EnableWindow(TRUE);
	SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));

	//����༭������,����ES_NUMBER���
	DWORD dwStyle = m_editUserID.GetStyle();
	dwStyle |= ES_NUMBER;
	SetWindowLong(m_editUserID.m_hWnd, GWL_STYLE, dwStyle);
}

void CClientControlItemSinkDlg::OnQueryUserNickName()
{
	CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
	if (pGameFrameEngine != NULL)
	{
		pGameFrameEngine->SendMessage(IDM_CLEAR_CURRENT_QUERYUSER, 0, 0);
	}

	SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));
	m_richEditUserDescription.CleanScreen();
	m_richEditUserControl.CleanScreen();
	SetDlgItemText(IDC_EDIT_CONTROL_COUNT,TEXT(""));
	GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

	m_QueryType = QUERY_USER_NICKNAME;

	GetDlgItem(IDC_EDIT_USER_ID)->EnableWindow(TRUE);
	SetDlgItemText(IDC_EDIT_USER_ID, TEXT(""));

	//����༭������,ɾ��ES_NUMBER���
	DWORD dwStyle = m_editUserID.GetStyle();
	dwStyle &= ~ES_NUMBER;
	SetWindowLong(m_editUserID.m_hWnd, GWL_STYLE, dwStyle);
}

void CClientControlItemSinkDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case IDI_REQUEST_UPDATE_ROOMINFO:
		{
			CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
			if (pGameFrameEngine != NULL)
			{
				pGameFrameEngine->SendMessage(IDM_REQUEST_UPDATE_ROOMINFO, 0, 0);
			}

			return;
		}
	}
	__super::OnTimer(nIDEvent);
}

void CClientControlItemSinkDlg::OnContinueControlWin()
{
	//��ȡ���ƾ���
	CString strCount = TEXT("");
	GetDlgItemText(IDC_EDIT_CONTROL_COUNT, strCount);
	WORD wControlCount = StrToInt(strCount);
	if(wControlCount <= 0 || wControlCount > 5)
	{
		CInformation information;
		information.ShowMessageBox(TEXT("��ܰ��ʾ"), TEXT("���ƾ�����ΧΪ1��5"), MB_OK, 5);
		return;
	}

	ASSERT(!((m_dwQueryUserGameID == INVALID_WORD) && (m_szQueryUserNickName[0] == 0)));

	CMD_C_UserControl UserControl;
	ZeroMemory(&UserControl, sizeof(UserControl));
	UserControl.dwGameID = m_dwQueryUserGameID;
	CopyMemory(UserControl.szNickName, m_szQueryUserNickName, sizeof(m_szQueryUserNickName));
	UserControl.userControlInfo.bCancelControl = false;
	UserControl.userControlInfo.cbControlCount = wControlCount;
	UserControl.userControlInfo.control_type = CONTINUE_WIN;

	CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
	if (pGameFrameEngine != NULL) 
	{
		pGameFrameEngine->SendMessage(IDM_USER_CONTROL, (WPARAM)&UserControl, 0);
	}
}

void CClientControlItemSinkDlg::OnContinueControlLost()
{
	//��ȡ���ƾ���
	CString strCount = TEXT("");
	GetDlgItemText(IDC_EDIT_CONTROL_COUNT, strCount);
	WORD wControlCount = StrToInt(strCount);
	if(wControlCount <= 0 || wControlCount > 5)
	{
		CInformation information;
		information.ShowMessageBox(TEXT("��ܰ��ʾ"), TEXT("���ƾ�����ΧΪ1��5"), MB_OK, 5);
		return;
	}

	ASSERT(!((m_dwQueryUserGameID == INVALID_WORD) && (m_szQueryUserNickName[0] == 0)));

	CMD_C_UserControl UserControl;
	ZeroMemory(&UserControl, sizeof(UserControl));
	UserControl.dwGameID = m_dwQueryUserGameID;
	CopyMemory(UserControl.szNickName, m_szQueryUserNickName, sizeof(m_szQueryUserNickName));
	UserControl.userControlInfo.bCancelControl = false;
	UserControl.userControlInfo.cbControlCount = wControlCount;
	UserControl.userControlInfo.control_type = CONTINUE_LOST;

	CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
	if (pGameFrameEngine != NULL) 
	{
		pGameFrameEngine->SendMessage(IDM_USER_CONTROL, (WPARAM)&UserControl, 0);
	}
}

void CClientControlItemSinkDlg::OnContinueControlCancel()
{
	ASSERT(!((m_dwQueryUserGameID == INVALID_WORD) && (m_szQueryUserNickName[0] == 0)));

	CMD_C_UserControl UserControl;
	ZeroMemory(&UserControl, sizeof(UserControl));
	UserControl.dwGameID = m_dwQueryUserGameID;
	CopyMemory(UserControl.szNickName, m_szQueryUserNickName, sizeof(m_szQueryUserNickName));
	UserControl.userControlInfo.bCancelControl = true;
	UserControl.userControlInfo.control_type = CONTINUE_CANCEL;

	CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
	if (pGameFrameEngine != NULL) 
	{
		pGameFrameEngine->SendMessage(IDM_USER_CONTROL, (WPARAM)&UserControl, 0);
	}

	GetDlgItem(IDC_EDIT_CONTROL_COUNT)->SetWindowText(TEXT(""));
}

void CClientControlItemSinkDlg::OnBnClickedBtnUserBetQuery()
{
	m_richEditUserControl.CleanScreen();

	if (m_QueryType == QUERY_INVALID)
	{
		m_richEditUserControl.InsertString(TEXT("����ȷѡ���ѯ������Ȼ�����������룡"), RGB(255,10,10));

		return;
	}

	//��ȡ�û�ID
	CString strUser = TEXT("");
	GetDlgItemText(IDC_EDIT_USER_ID, strUser);

	if(strUser == TEXT(""))
	{
		CInformation information;
		information.ShowMessageBox(TEXT("��ܰ��ʾ"), TEXT("����ȷ�������ID���ǳ�"), MB_OK, 5);
		return;
	}

	CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
	ASSERT(pGameFrameEngine != NULL);
	//ȥ���ո�
	CString szStr = strUser;
	szStr.TrimLeft();
	szStr.TrimRight();

	CMD_C_RequestQuery_User	QueryUser;
	ZeroMemory(&QueryUser, sizeof(QueryUser));

	ASSERT(m_QueryType != QUERY_INVALID);

	//��ѯGAMEID
	if (m_QueryType == QUERY_USER_GAMEID)
	{
		QueryUser.dwGameID = StrToInt(szStr);
		m_dwQueryUserGameID = StrToInt(szStr);	
	}
	else if (m_QueryType == QUERY_USER_NICKNAME)
	{
		CopyMemory(QueryUser.szNickName, szStr, sizeof(QueryUser.szNickName));
		CopyMemory(m_szQueryUserNickName, szStr, sizeof(m_szQueryUserNickName));
	}
	
	if (pGameFrameEngine != NULL) 
	{
		pGameFrameEngine->SendMessage(IDM_REQUEST_QUERY_USER, (WPARAM)&QueryUser, 0);	
	}
}

//��ѯ�û����
void CClientControlItemSinkDlg::RequestQueryResult(CMD_S_RequestQueryResult *pQueryResult)
{
	//�����Ļ
	m_richEditUserDescription.CleanScreen();
	m_richEditUserControl.CleanScreen();
	GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

	if (pQueryResult->bFind == false)
	{
		m_richEditUserDescription.InsertString(TEXT("��ѯ��������ҵ���Ϣ�����ܸ������δ��ȥ����Ϸ�������δ�����κ�һ�����ӣ�"),RGB(255,10,10));
		return;
	}
	else  //��ѯ��Ŀ�����
	{
		CString strUserInfo;
		CString strUserStatus;
		CString strGameStatus;
		CString strSatisfyControl;
		CString strAndroid = (pQueryResult->userinfo.bAndroid == true ? TEXT("Ϊ������") : TEXT("Ϊ�������"));
		bool bEnableControl = false;
		GetGameStatusString(pQueryResult, strGameStatus);
		GetUserStatusString(pQueryResult, strUserStatus);
		GetSatisfyControlString(pQueryResult, strSatisfyControl, bEnableControl);

		if (pQueryResult->userinfo.wChairID != INVALID_CHAIR && pQueryResult->userinfo.wTableID != INVALID_TABLE)
		{
			strUserInfo.Format(TEXT("��ѯ��ҡ�%s�� %s,���ڵ�%d�����ӣ���%d�����ӽ�����Ϸ���û�״̬Ϊ%s,��Ϸ״̬Ϊ%s,%s ! "), pQueryResult->userinfo.szNickName, 
				strAndroid, pQueryResult->userinfo.wTableID, pQueryResult->userinfo.wChairID, strUserStatus, strGameStatus, strSatisfyControl);

			m_richEditUserDescription.InsertString(strUserInfo, RGB(255,10,10));

			if (bEnableControl)
			{
				GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(TRUE);
				GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(TRUE);
				GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(TRUE);
				GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(TRUE);
			}
		}
		else
		{
			strUserInfo.Format(TEXT("��ѯ��ҡ�%s�� %s �����κ����ӣ����������������"), pQueryResult->userinfo.szNickName, strAndroid);

			m_richEditUserDescription.InsertString(strUserInfo, RGB(255,10,10));
			return;
		}
	}
}

//��ȡ�û�״̬
void CClientControlItemSinkDlg::GetUserStatusString(CMD_S_RequestQueryResult *pQueryResult, CString &userStatus)
{
	ASSERT(pQueryResult->bFind);

	switch (pQueryResult->userinfo.cbUserStatus)
	{
	case US_NULL:
		{
			userStatus = TEXT("û��");
			return;
		}
	case US_FREE:
		{
			userStatus = TEXT("վ��");
			return;
		}
	case US_SIT:
		{
			userStatus = TEXT("����");
			return;
		}
	case US_READY:
		{
			userStatus = TEXT("ͬ��");
			return;
		}
	case US_LOOKON:
		{
			userStatus = TEXT("�Թ�");
			return;
		}
	case US_PLAYING:
		{
			userStatus = TEXT("��Ϸ");
			return;
		}
	case US_OFFLINE:
		{
			userStatus = TEXT("����");
			return;
		}
	ASSERT(FALSE);
	}
}

//��ȡ�û�״̬
void CClientControlItemSinkDlg::GetGameStatusString(CMD_S_RequestQueryResult *pQueryResult, CString &gameStatus)
{
	ASSERT(pQueryResult->bFind);

	switch (pQueryResult->userinfo.cbGameStatus)
	{
	case GAME_STATUS_FREE:
		{
			gameStatus = TEXT("����");
			return;
		}
	case GAME_STATUS_PLAY:
		{
			gameStatus = TEXT("��Ϸ����");
			return;
		}
		ASSERT(FALSE);
	}
}

//��ȡ�Ƿ��������
void CClientControlItemSinkDlg::GetSatisfyControlString(CMD_S_RequestQueryResult *pQueryResult, CString &satisfyControl, bool &bEnableControl)
{
	ASSERT(pQueryResult->bFind);

	if (pQueryResult->userinfo.wChairID == INVALID_CHAIR || pQueryResult->userinfo.wTableID == INVALID_TABLE)
	{
		satisfyControl = TEXT("���������");
		bEnableControl = FALSE;
		return;
	}

	if (pQueryResult->userinfo.cbUserStatus == US_SIT || pQueryResult->userinfo.cbUserStatus == US_READY || pQueryResult->userinfo.cbUserStatus == US_PLAYING)
	{
		if (pQueryResult->userinfo.cbGameStatus == GAME_STATUS_FREE)
		{
			satisfyControl = TEXT("�������,�þֿ�ʼ����");
			bEnableControl = TRUE;
			return;
		}
		else
		{
			satisfyControl = TEXT("������ƣ��¾ֿ�ʼ����");
			bEnableControl = TRUE;
			return;
		}
	}
	else
	{
		satisfyControl = TEXT("���������");
		bEnableControl = FALSE;
		return;
	}
}

//�����û����ƽ��
void CClientControlItemSinkDlg::RoomUserControlResult(CMD_S_UserControl *pUserControl)
{
	m_richEditUserControl.CleanScreen();

	//���ƽ��
	CString strControlInfo;
	CString strControlType;

	switch(pUserControl->controlResult)
	{
	case CONTROL_SUCCEED:
		{
			GetControlTypeString(pUserControl->controlType, strControlType);

			if (pUserControl->controlType == CONTINUE_WIN || pUserControl->controlType == CONTINUE_LOST)
			{
				strControlInfo.Format(TEXT("��ҡ�%s�����Ƴɹ���%s, ���ƾ��� %d �� ��"), pUserControl->szNickName, strControlType, pUserControl->cbControlCount);
			}

			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
			break;
		}
	case CONTROL_FAIL:
		{
			if (pUserControl->controlType == CONTINUE_WIN || pUserControl->controlType == CONTINUE_LOST)
			{
				strControlInfo.Format(TEXT("��ҡ�%s������ʧ�ܣ������״̬������Ϸ״̬��������Ƶ����� ��"), pUserControl->szNickName);
			}

			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
			break;
		}
	case CONTROL_CANCEL_SUCCEED:
		{
			strControlInfo.Format(TEXT("��ҡ�%s��ȡ�����Ƴɹ� ��"), pUserControl->szNickName);

			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
			break;
		}
	case CONTROL_CANCEL_INVALID:
		{
			strControlInfo.Format(TEXT("��ҡ�%s��ȡ������ʧ�ܣ�����Ҳ����ڿ��ƻ��߿�������� ��"), pUserControl->szNickName);

			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
			break;
		}
	ASSERT(FALSE);
	}
	
}

//�û��������
void CClientControlItemSinkDlg::UserControlComplete(CMD_S_UserControlComplete *pUserControlComplete)
{
	m_richEditUserControl.CleanScreen();

	ASSERT(pUserControlComplete->controlType != CONTINUE_CANCEL);
	
	//���ƽ��
	CString strControlInfo;
	CString strControlType;

	switch(pUserControlComplete->controlType)
	{
	case CONTINUE_WIN:
		{
			GetControlTypeString(pUserControlComplete->controlType, strControlType);

			strControlInfo.Format(TEXT("��ҡ�%s��%s, ʣ����ƾ���Ϊ %d �� ��"), pUserControlComplete->szNickName, strControlType, pUserControlComplete->cbRemainControlCount);
			
			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
			break;
		}
	case CONTINUE_LOST:
		{
			GetControlTypeString(pUserControlComplete->controlType, strControlType);

			strControlInfo.Format(TEXT("��ҡ�%s��%s, ʣ����ƾ���Ϊ %d �� ��"), pUserControlComplete->szNickName, strControlType, pUserControlComplete->cbRemainControlCount);

			m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
			break;
		}
	ASSERT(FALSE);
	}
}

//������¼
void CClientControlItemSinkDlg::UpdateOperationRecord(CMD_S_Operation_Record *pOperation_Record)
{
	m_richEditOperationRecord.CleanScreen();
	
	m_richEditOperationRecord.EnableWindow(TRUE);
	//��������
	TCHAR szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];
	ZeroMemory(szRecord, sizeof(szRecord));
	CopyMemory(szRecord, pOperation_Record->szRecord, sizeof(szRecord));
	WORD wRecordCount = 1;
	CString strCount;

	for (WORD i=0; i<MAX_OPERATION_RECORD; i++)
	{
		for (WORD j=0; j<RECORD_LENGTH; j++)
		{
			if (szRecord[i][j] ==0)
			{
				break;
			}

			if (szRecord[i][j] !=0)
			{
				strCount.Format(TEXT("�ڡ�%d����-%s"), wRecordCount, szRecord[i]);

				m_richEditOperationRecord.InsertString(strCount, RGB(255,10,10));
				m_richEditOperationRecord.InsertString(TEXT("\n"), RGB(255,10,10));

				wRecordCount++;
				break;
			}
		}
	}
}

//���ø��¶�ʱ��
void CClientControlItemSinkDlg::SetUpdateIDI()
{
	//���ø��¶�ʱ��
	SetTimer(IDI_REQUEST_UPDATE_ROOMINFO, TIME_ROOMINFO, NULL);
}

//���·�����Ϣ
void CClientControlItemSinkDlg::UpdateRoomInfoResult(CMD_S_RequestUpdateRoomInfo_Result *RoomInfo_Result)
{
	CString strBuf;
	strBuf.Format(SCORE_STRING, RoomInfo_Result->lRoomStorageCurrent);
	GetDlgItem(IDC_STATIC_ROOMCURRENT_STORAGE)->SetWindowText(strBuf);

	if (m_dwQueryUserGameID == INVALID_WORD && m_szQueryUserNickName[0] == 0)
	{
		CGameFrameEngine * pGameFrameEngine = CGameFrameEngine::GetInstance();
		if (pGameFrameEngine != NULL)
		{
			pGameFrameEngine->SendMessage(IDM_CLEAR_CURRENT_QUERYUSER, 0, 0);
		}

		return;
	}

	if (RoomInfo_Result->currentqueryuserinfo.dwGameID == 0 &&
		RoomInfo_Result->currentqueryuserinfo.wChairID == 0 &&
		RoomInfo_Result->currentqueryuserinfo.wTableID == 0)
	{
		return;
	}
	
	GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(FALSE);

	CMD_S_RequestQueryResult QueryResult;
	ZeroMemory(&QueryResult, sizeof(QueryResult));
	QueryResult.bFind = true;
	CopyMemory(&(QueryResult.userinfo), &(RoomInfo_Result->currentqueryuserinfo), sizeof(QueryResult.userinfo));
	
	static CString strDuplicate = TEXT("");
	CString strUserInfo;
	CString strUserStatus;
	CString strGameStatus;
	CString strSatisfyControl;
	CString strAndroid = (QueryResult.userinfo.bAndroid == true ? TEXT("Ϊ������") : TEXT("Ϊ�������"));
	bool bEnableControl = false;
	GetGameStatusString(&QueryResult, strGameStatus);
	GetUserStatusString(&QueryResult, strUserStatus);
	GetSatisfyControlString(&QueryResult, strSatisfyControl, bEnableControl);

	if (QueryResult.userinfo.wChairID != INVALID_CHAIR && QueryResult.userinfo.wTableID != INVALID_TABLE)
	{
		strUserInfo.Format(TEXT("��ѯ��ҡ�%s�� %s,���ڵ�%d�����ӣ���%d�����ӽ�����Ϸ���û�״̬Ϊ%s,��Ϸ״̬Ϊ%s,%s ! "), QueryResult.userinfo.szNickName, 
			strAndroid, QueryResult.userinfo.wTableID, QueryResult.userinfo.wChairID, strUserStatus, strGameStatus, strSatisfyControl);

		if (strDuplicate != strUserInfo)
		{
			m_richEditUserDescription.CleanScreen();
			m_richEditUserDescription.InsertString(strUserInfo, RGB(255,10,10));
			strDuplicate = strUserInfo;
		}

		if (bEnableControl)
		{
			GetDlgItem(IDC_EDIT_CONTROL_COUNT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BT_CONTINUE_WIN)->EnableWindow(TRUE);
			GetDlgItem(IDC_BT_CONTINUE_LOST)->EnableWindow(TRUE);
			GetDlgItem(IDC_BT_CONTINUE_CANCEL)->EnableWindow(TRUE);
		}
	}
	else
	{
		strUserInfo.Format(TEXT("��ѯ��ҡ�%s�� %s �����κ����ӣ����������������"), QueryResult.userinfo.szNickName, strAndroid);
		
		if (strDuplicate != strUserInfo)
		{
			m_richEditUserDescription.CleanScreen();
			m_richEditUserDescription.InsertString(strUserInfo, RGB(255,10,10));
			strDuplicate = strUserInfo;
		}
		
	}
	
	//���ڿ���
	if (RoomInfo_Result->bExistControl == true)
	{
		m_richEditUserControl.CleanScreen();

		ASSERT(RoomInfo_Result->currentusercontrol.control_type != CONTINUE_CANCEL);

		//���ƽ��
		CString strControlInfo;
		CString strControlType;

		switch(RoomInfo_Result->currentusercontrol.control_type)
		{
		case CONTINUE_WIN:
			{
				GetControlTypeString(RoomInfo_Result->currentusercontrol.control_type, strControlType);

				strControlInfo.Format(TEXT("��ҡ�%s��%s, ʣ����ƾ���Ϊ %d �� ��"), QueryResult.userinfo.szNickName, strControlType, RoomInfo_Result->currentusercontrol.cbControlCount);

				m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
				break;
			}
		case CONTINUE_LOST:
			{
				GetControlTypeString(RoomInfo_Result->currentusercontrol.control_type, strControlType);

				strControlInfo.Format(TEXT("��ҡ�%s��%s, ʣ����ƾ���Ϊ %d �� ��"), QueryResult.userinfo.szNickName, strControlType, RoomInfo_Result->currentusercontrol.cbControlCount);

				m_richEditUserControl.InsertString(strControlInfo, RGB(255,10,10));
				break;
			}
			ASSERT(FALSE);
		}
	}
}

//��ȡ��������
void CClientControlItemSinkDlg::GetControlTypeString(CONTROL_TYPE &controlType, CString &controlTypestr)
{
	switch(controlType)
	{
	case CONTINUE_WIN:
		{
			controlTypestr = TEXT("��������Ϊ��Ӯ");
			break;
		}
	case CONTINUE_LOST:
		{
			controlTypestr = TEXT("��������Ϊ����");
			break;
		}
	case CONTINUE_CANCEL:
		{
			controlTypestr = TEXT("��������Ϊȡ������");
			break;
		}
	}
}




