// DlgCustomRule.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgCustomRule.h"


// CDlgCustomRule 对话框

BEGIN_MESSAGE_MAP(CDlgCustomRule, CDialog)
END_MESSAGE_MAP()

CDlgCustomRule::CDlgCustomRule() : CDialog(IDD_CUSTOM_RULE)
{
	//设置变量
	ZeroMemory(&m_CustomRule, sizeof(m_CustomRule));

	m_CustomRule.lStorageStartTable = 100000;
	m_CustomRule.lStorageDeductRoom = 0;
	m_CustomRule.lStorageMax1Room = 1000000;
	m_CustomRule.lStorageMul1Room = 50;
	m_CustomRule.lStorageMax2Room = 5000000;
	m_CustomRule.lStorageMul2Room = 75;
	m_CustomRule.wGameTwo = INVALID_WORD;
	m_CustomRule.wGameTwoDeduct = INVALID_WORD;
	m_CustomRule.wGameThree = INVALID_WORD;
	m_CustomRule.wGameThreeDeduct = INVALID_WORD;

	m_CustomRule.cbBetCount = 9;
	m_CustomRule.lBetScore[0] = 10;
	m_CustomRule.lBetScore[1] = 20;
	m_CustomRule.lBetScore[2] = 30;
	m_CustomRule.lBetScore[3] = 40;
	m_CustomRule.lBetScore[4] = 50;
	m_CustomRule.lBetScore[5] = 60;
	m_CustomRule.lBetScore[6] = 70;
	m_CustomRule.lBetScore[7] = 80;
	m_CustomRule.lBetScore[8] = 90;

	m_CustomRule.TheFirstPass[0] = 50;
	m_CustomRule.TheFirstPass[1] = 50;
	m_CustomRule.TheFirstPass[2] = 50;
	m_CustomRule.TheFirstPass[3] = 100;
	m_CustomRule.TheFirstPass[4] = 250;

	m_CustomRule.TheSecondPass[0] = 100;
	m_CustomRule.TheSecondPass[1] = 100;
	m_CustomRule.TheSecondPass[2] = 250;
	m_CustomRule.TheSecondPass[3] = 500;
	m_CustomRule.TheSecondPass[4] = 0;

	m_CustomRule.TheThirdPass[0] = 250;
	m_CustomRule.TheThirdPass[1] = 250;
	m_CustomRule.TheThirdPass[2] = 250;
	m_CustomRule.TheThirdPass[3] = 0;
	m_CustomRule.TheThirdPass[4] = 0;

	m_CustomRule.TheFourthPass[0] = 500;
	m_CustomRule.TheFourthPass[1] = 500;
	m_CustomRule.TheFourthPass[2] = 0;
	m_CustomRule.TheFourthPass[3] = 0;
	m_CustomRule.TheFourthPass[4] = 0;
}

CDlgCustomRule::~CDlgCustomRule()
{
}

//配置函数
BOOL CDlgCustomRule::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件范围
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_START_TABLE))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_DEDUCT_ROOM))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_MAX1_ROOM))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_MUL1_ROOM))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_MAX2_ROOM))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_STORAGE_MUL2_ROOM))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_GAME_TWO))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_GAME_TWO_DEDUCT))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_GAME_THREE))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_GAME_THREE_DEDUCT))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_1))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_2))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_3))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_4))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_5))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_6))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_7))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_8))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_BET_SCORE_9))->LimitText(9);

	//小游戏控件
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_1))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_2))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_3))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_4))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_5))->LimitText(9);

	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_6))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_7))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_8))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_9))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_10))->LimitText(9);

	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_11))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_12))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_13))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_14))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_15))->LimitText(9);

	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_16))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_17))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_18))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_19))->LimitText(9);
	((CEdit *)GetDlgItem(IDC_EDIT_SMALL_SORCE_20))->LimitText(9);

	//更新参数
	FillDataToControl();

	return FALSE;
}

//确定函数
VOID CDlgCustomRule::OnOK() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDOK, 0), 0);

	return;
}

//取消消息
VOID CDlgCustomRule::OnCancel() 
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND, MAKELONG(IDCANCEL, 0), 0);

	return;
}

//更新控件
bool CDlgCustomRule::FillDataToControl()
{
	//设置数据
	SetDlgItemInt(IDC_EDIT_STORAGE_START_TABLE, m_CustomRule.lStorageStartTable);
	SetDlgItemInt(IDC_EDIT_STORAGE_DEDUCT_ROOM, m_CustomRule.lStorageDeductRoom);
	SetDlgItemInt(IDC_EDIT_STORAGE_MAX1_ROOM, m_CustomRule.lStorageMax1Room);
	SetDlgItemInt(IDC_EDIT_STORAGE_MUL1_ROOM, m_CustomRule.lStorageMul1Room);
	SetDlgItemInt(IDC_EDIT_STORAGE_MAX2_ROOM, m_CustomRule.lStorageMax2Room);
	SetDlgItemInt(IDC_EDIT_STORAGE_MUL2_ROOM, m_CustomRule.lStorageMul2Room);
	if(m_CustomRule.wGameTwo != INVALID_WORD)
	{
		SetDlgItemInt(IDC_EDIT_GAME_TWO, m_CustomRule.wGameTwo);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_TWO, TEXT(""));
	}
	if(m_CustomRule.wGameTwoDeduct != INVALID_WORD)
	{
		SetDlgItemInt(IDC_EDIT_GAME_TWO_DEDUCT, m_CustomRule.wGameTwoDeduct);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_TWO_DEDUCT, TEXT(""));
	}
	if(m_CustomRule.wGameThree != INVALID_WORD)
	{
		SetDlgItemInt(IDC_EDIT_GAME_THREE, m_CustomRule.wGameThree);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_THREE, TEXT(""));
	}
	if(m_CustomRule.wGameThreeDeduct != INVALID_WORD)
	{
		SetDlgItemInt(IDC_EDIT_GAME_THREE_DEDUCT, m_CustomRule.wGameThreeDeduct);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_GAME_THREE_DEDUCT, TEXT(""));
	}
	SetDlgItemInt(IDC_EDIT_BET_SCORE_1, m_CustomRule.lBetScore[0]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_2, m_CustomRule.lBetScore[1]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_3, m_CustomRule.lBetScore[2]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_4, m_CustomRule.lBetScore[3]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_5, m_CustomRule.lBetScore[4]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_6, m_CustomRule.lBetScore[5]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_7, m_CustomRule.lBetScore[6]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_8, m_CustomRule.lBetScore[7]);
	SetDlgItemInt(IDC_EDIT_BET_SCORE_9, m_CustomRule.lBetScore[8]);

	//小游戏关卡
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_1, m_CustomRule.TheFirstPass[0]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_2, m_CustomRule.TheFirstPass[1]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_3, m_CustomRule.TheFirstPass[2]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_4, m_CustomRule.TheFirstPass[3]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_5, m_CustomRule.TheFirstPass[4]);

	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_6, m_CustomRule.TheSecondPass[0]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_7, m_CustomRule.TheSecondPass[1]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_8, m_CustomRule.TheSecondPass[2]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_9, m_CustomRule.TheSecondPass[3]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_10, m_CustomRule.TheSecondPass[4]);

	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_11, m_CustomRule.TheThirdPass[0]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_12, m_CustomRule.TheThirdPass[1]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_13, m_CustomRule.TheThirdPass[2]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_14, m_CustomRule.TheThirdPass[3]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_15, m_CustomRule.TheThirdPass[4]);

	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_16, m_CustomRule.TheFourthPass[0]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_17, m_CustomRule.TheFourthPass[1]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_18, m_CustomRule.TheFourthPass[2]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_19, m_CustomRule.TheFourthPass[3]);
	SetDlgItemInt(IDC_EDIT_SMALL_SORCE_20, m_CustomRule.TheFourthPass[4]);

	return true;
}

//更新数据
bool CDlgCustomRule::FillControlToData()
{
	//设置数据
	m_CustomRule.lStorageStartTable = (LONGLONG)GetDlgItemInt(IDC_EDIT_STORAGE_START_TABLE);
	m_CustomRule.lStorageDeductRoom = (LONGLONG)GetDlgItemInt(IDC_EDIT_STORAGE_DEDUCT_ROOM);
	m_CustomRule.lStorageMax1Room = (LONGLONG)GetDlgItemInt(IDC_EDIT_STORAGE_MAX1_ROOM);
	m_CustomRule.lStorageMul1Room = (LONGLONG)GetDlgItemInt(IDC_EDIT_STORAGE_MUL1_ROOM);
	m_CustomRule.lStorageMax2Room = (LONGLONG)GetDlgItemInt(IDC_EDIT_STORAGE_MAX2_ROOM);
	m_CustomRule.lStorageMul2Room = (LONGLONG)GetDlgItemInt(IDC_EDIT_STORAGE_MUL2_ROOM);
	
	CString strGameTwo;
	CString strGameTwoDeduct;
	CString strGameThree;
	CString strGameThreeDeduct;
	GetDlgItemText(IDC_EDIT_GAME_TWO, strGameTwo);
	GetDlgItemText(IDC_EDIT_GAME_TWO_DEDUCT, strGameTwoDeduct);
	GetDlgItemText(IDC_EDIT_GAME_THREE, strGameThree);
	GetDlgItemText(IDC_EDIT_GAME_THREE_DEDUCT, strGameThreeDeduct);
	if(strGameTwoDeduct == TEXT(""))
	{
		m_CustomRule.wGameTwoDeduct = INVALID_WORD;
	}
	else
	{
		m_CustomRule.wGameTwoDeduct = GetDlgItemInt(IDC_EDIT_GAME_TWO_DEDUCT);
	}
	if(strGameTwo == TEXT(""))
	{
		m_CustomRule.wGameTwo = INVALID_WORD;
		m_CustomRule.wGameTwoDeduct = INVALID_WORD;
	}
	else
	{
		m_CustomRule.wGameTwo = GetDlgItemInt(IDC_EDIT_GAME_TWO);
	}

	if(strGameThreeDeduct == TEXT(""))
	{
		m_CustomRule.wGameThreeDeduct = INVALID_WORD;
	}
	else
	{
		m_CustomRule.wGameThreeDeduct = GetDlgItemInt(IDC_EDIT_GAME_THREE_DEDUCT);
	}	
	if(strGameThree == TEXT(""))
	{
		m_CustomRule.wGameThree = INVALID_WORD;
		m_CustomRule.wGameThreeDeduct = INVALID_WORD;
	}
	else
	{
		m_CustomRule.wGameThree = GetDlgItemInt(IDC_EDIT_GAME_THREE);
	}		

	if (m_CustomRule.lStorageMax1Room > m_CustomRule.lStorageMax2Room)
	{
		AfxMessageBox(TEXT("库存封顶值1应小于库存封顶值2，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lStorageMul1Room > m_CustomRule.lStorageMul2Room)
	{
		AfxMessageBox(TEXT("赢分概率1应小于赢分概率2，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lStorageMul1Room > m_CustomRule.lStorageMul2Room)
	{
		AfxMessageBox(TEXT("赢分概率1应小于赢分概率2，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	if (m_CustomRule.lStorageMul1Room > m_CustomRule.lStorageMul2Room)
	{
		AfxMessageBox(TEXT("赢分概率1应小于赢分概率2，请重新设置！"), MB_ICONSTOP);
		return false;
	}

	//校验合法性
	if (!(m_CustomRule.wGameTwo >= 0 && m_CustomRule.wGameTwo <= 100 || m_CustomRule.wGameTwo == INVALID_WORD))
	{
		AfxMessageBox(TEXT("比倍中奖概率范围0-100 或 空"));
		return false;
	}

	//校验合法性
	if (!(m_CustomRule.wGameTwoDeduct >= 0 && m_CustomRule.wGameTwoDeduct < 100 || m_CustomRule.wGameTwoDeduct == INVALID_WORD))
	{
		AfxMessageBox(TEXT("比倍中奖概率衰减值范围0-99 或 空"));
		return false;
	}

	//校验合法性
	if (!(m_CustomRule.wGameThree >= 0 && m_CustomRule.wGameThree <= 100 || m_CustomRule.wGameThree == INVALID_WORD))
	{
		AfxMessageBox(TEXT("小玛丽中奖概率范围0-100 或 空"));
		return false;
	}

	//校验合法性
	if (!(m_CustomRule.wGameThreeDeduct >= 0 && m_CustomRule.wGameThreeDeduct < 100 || m_CustomRule.wGameThreeDeduct == INVALID_WORD))
	{
		AfxMessageBox(TEXT("小玛丽中奖概率衰减值范围0-99 或 空"));
		return false;
	}

	m_CustomRule.cbBetCount = 0;
	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (LONGLONG)GetDlgItemInt(IDC_EDIT_BET_SCORE_1);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (LONGLONG)GetDlgItemInt(IDC_EDIT_BET_SCORE_2);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (LONGLONG)GetDlgItemInt(IDC_EDIT_BET_SCORE_3);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (LONGLONG)GetDlgItemInt(IDC_EDIT_BET_SCORE_4);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (LONGLONG)GetDlgItemInt(IDC_EDIT_BET_SCORE_5);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_6);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_7);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_8);
	if(m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	m_CustomRule.lBetScore[m_CustomRule.cbBetCount] = (SCORE)GetDlgItemInt(IDC_EDIT_BET_SCORE_9);
	if (m_CustomRule.lBetScore[m_CustomRule.cbBetCount] > 0)
	{
		m_CustomRule.cbBetCount++;
	}

	//小游戏
	m_CustomRule.TheFirstPass[0] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_1);
	m_CustomRule.TheFirstPass[1] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_2);
	m_CustomRule.TheFirstPass[2] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_3);
	m_CustomRule.TheFirstPass[3] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_4);
	m_CustomRule.TheFirstPass[4] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_5);

	m_CustomRule.TheSecondPass[0] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_6);
	m_CustomRule.TheSecondPass[1] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_7);
	m_CustomRule.TheSecondPass[2] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_8);
	m_CustomRule.TheSecondPass[3] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_9);
	m_CustomRule.TheSecondPass[4] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_10);

	m_CustomRule.TheThirdPass[0] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_11);
	m_CustomRule.TheThirdPass[1] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_12);
	m_CustomRule.TheThirdPass[2] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_13);
	m_CustomRule.TheThirdPass[3] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_14);
	m_CustomRule.TheThirdPass[4] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_15);

	m_CustomRule.TheFourthPass[0] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_16);
	m_CustomRule.TheFourthPass[1] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_17);
	m_CustomRule.TheFourthPass[2] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_18);
	m_CustomRule.TheFourthPass[3] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_19); 
	m_CustomRule.TheFourthPass[4] = (LONGLONG)GetDlgItemInt(IDC_EDIT_SMALL_SORCE_20);

	if (m_CustomRule.cbBetCount == 0)
	{
		AfxMessageBox(TEXT("下注值不能全为空，请重新设置！"), MB_ICONSTOP);
		return false;
	}
	else
	{
		for(int i = 0; i < m_CustomRule.cbBetCount - 1; i++)
		{
			for(int j = i + 1; j < m_CustomRule.cbBetCount; j++)
			{
				if(m_CustomRule.lBetScore[i] == m_CustomRule.lBetScore[j] && m_CustomRule.lBetScore[i] != 0)
				{
					AfxMessageBox(TEXT("下注值有重复，请重新设置！"), MB_ICONSTOP);
					return false;
				}
			}
		}
	}

	//下注排序
	LONGLONG lTempJteeon;
	for(int i = 0; i < m_CustomRule.cbBetCount - 1; i++)
	{
		for(int j = 0; j < m_CustomRule.cbBetCount - 1 - i; j++)
		{
			if(m_CustomRule.lBetScore[j] > m_CustomRule.lBetScore[j + 1])
			{
				lTempJteeon = m_CustomRule.lBetScore[j];
				m_CustomRule.lBetScore[j] = m_CustomRule.lBetScore[j + 1];
				m_CustomRule.lBetScore[j + 1] = lTempJteeon;
			}
		}
	}

	return true;
}

//读取配置
bool CDlgCustomRule::GetCustomRule(tagCustomRule & CustomRule)
{
	//读取参数
	if (FillControlToData() == true)
	{
		CustomRule = m_CustomRule;
		return true;
	}

	return false;
}

//设置配置
bool CDlgCustomRule::SetCustomRule(tagCustomRule & CustomRule)
{
	//设置变量
	m_CustomRule = CustomRule;

	//更新参数
	if (m_hWnd != NULL) 
	{
		FillDataToControl();
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
