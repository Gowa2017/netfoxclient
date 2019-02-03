#include "StdAfx.h"
#include "Resource.h"
#include "DlgMatchWizard.h"

//////////////////////////////////////////////////////////////////////////
//��������

#define TAB_OPTION_SIGNUP1					0
#define TAB_OPTION_SIGNUP2					1
#define TAB_OPTION_RANKINGMODE				2
#define TAB_OPTION_DISTRIBUTE				3
#define TAB_OPTION_REWARD					4

//////////////////////////////////////////////////////////////////////////
//��Ϣӳ��

BEGIN_MESSAGE_MAP(CDlgMatchItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchWizardItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchSignup1, CDlgMatchItem)
	ON_CBN_SELCHANGE(IDC_COMBO_FROM_MATCH, OnCbnSelchangeComboFromMatch)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchReward, CDlgMatchItem)
	ON_BN_CLICKED(IDC_ADD_REWARD, OnBnClickedAddReward)
	ON_BN_CLICKED(IDC_MODIFY_REWARD, OnBnClickedModifyReward)	
	ON_BN_CLICKED(IDC_DELETE_REWARD, OnBnClickedDeleteReward)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_REWARD, OnLvnItemchangedListReward)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchWizardStep1, CDlgMatchWizardItem)	
	ON_CBN_SELCHANGE(IDC_COMBO_MATCH_TYPE, OnCbnSelchangeComboMatchType)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MATCH_OPTION, OnTcnSelchangeMatchOption)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgMatchWizard, CDialog)
	ON_BN_CLICKED(IDC_LAST, OnBnClickedLast)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_FINISH, OnBnClickedFinish)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//���캯��
CDlgMatchItem::CDlgMatchItem(UINT nIDTemplate): CDialog(nIDTemplate)
{
	//���ñ���		
	m_pGameMatchOption=NULL;
	m_pMatchInfoBuffer=NULL;
	m_pRewardInfoBuffer=NULL;
}

//��������
CDlgMatchItem::~CDlgMatchItem()
{
}

//���ñ���
VOID CDlgMatchItem::SetMatchInfoBuffer(CMatchInfoBuffer * pMatchInfoBuffer)
{
	m_pMatchInfoBuffer=pMatchInfoBuffer;
}

//���ý���
VOID CDlgMatchItem::SetRewardInfoBuffer(CRewardInfoBuffer * pRewardInfoBuffer)
{
	m_pRewardInfoBuffer=pRewardInfoBuffer;
}

//��������
VOID CDlgMatchItem::SetGameMatchOption(tagGameMatchOption * pGameMatchOption)
{
	m_pGameMatchOption=pGameMatchOption;
}

//������
bool CDlgMatchItem::ShowMatchItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) CreateMatchItem(rcRect,pParentWnd);

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

//��������
VOID CDlgMatchItem::CreateMatchItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//������Դ
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//��������
	BOOL bSuccess=Create(m_lpszTemplateName,pParentWnd);
	SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(NULL));
}

//���ͱ��
VOID CDlgMatchItem::OnEventMatchTypeChanged(BYTE cbMatchType)
{
}

//λ����Ϣ
VOID CDlgMatchItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchSignup1::CDlgMatchSignup1() : CDlgMatchItem(IDD_MATCH_SIGNUP1)
{
}

//��������
CDlgMatchSignup1::~CDlgMatchSignup1()
{
}

//��ʼ������
BOOL CDlgMatchSignup1::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�
	((CEdit *)GetDlgItem(IDC_EDIT_SIGNUP_FEE))->LimitText(9);

	//Ĭ��ѡ��
	CheckRadioButton(IDC_RADIO_GOLD,IDC_RADIO_MEDAL,IDC_RADIO_GOLD);
	CheckRadioButton(IDC_RADIO_SERVER,IDC_RADIO_WEBSITE,IDC_RADIO_SERVER);	

	//��������
	SetDlgItemInt(IDC_EDIT_SIGNUP_FEE,(UINT)m_pGameMatchOption->lSignupFee);

	//��������
	if (m_pGameMatchOption->cbSignupMode&SIGNUP_MODE_SIGNUP_FEE)
	{
		//����ѡ��
		((CButton *)GetDlgItem(IDC_CHECK_SIGNUP_FEE))->SetCheck(TRUE);

		//��������
		SetDlgItemInt(IDC_EDIT_SIGNUP_FEE,(int)m_pGameMatchOption->lSignupFee);

		//��������
		CheckRadioButton(IDC_RADIO_GOLD,IDC_RADIO_MEDAL,IDC_RADIO_GOLD+(m_pGameMatchOption->cbFeeType-FEE_TYPE_GOLD));

		//�ɷ�λ��
		CheckRadioButton(IDC_RADIO_SERVER,IDC_RADIO_WEBSITE,IDC_RADIO_SERVER+(m_pGameMatchOption->cbDeductArea-DEDUCT_AREA_SERVER));		
	}

	//��������
	if (m_pGameMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
	{
		//����ѡ��
		((CButton *)GetDlgItem(IDC_CHECK_MATCH_USER))->SetCheck(TRUE);		

		//��������
		CDateTimeCtrl * pEndtDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_END);
		pEndtDate->SetTime(&m_pGameMatchOption->MatchEndDate);

		//��ʼ����
		CDateTimeCtrl * pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_START);
		pStartDate->SetTime(&m_pGameMatchOption->MatchStartDate);		

		//��������
		if (m_pGameMatchOption->cbFilterType==FILTER_TYPE_SINGLE_TURN)
		{			
			SetDlgItemInt(IDC_EDIT_SINGLE_RANK,m_pGameMatchOption->wMaxRankID);
			CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_SINGLE_TURN);			
		}

		//��������
		if (m_pGameMatchOption->cbFilterType==FILTER_TYPE_TOTAL_RANKING)
		{
			SetDlgItemInt(IDC_EDIT_TOTAL_RANK,m_pGameMatchOption->wMaxRankID);
			CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_TOTAL_RANKING);			
		}		
	}
	else
	{
		//Ĭ������
		CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_SINGLE_TURN);
	}

	//��ȡ�ؼ�
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_FROM_MATCH);
	if (pComboBox!=NULL)
	{
		//���ر���
		if (m_pMatchInfoBuffer!=NULL)
		{
			//��������
			for (INT_PTR i=0;i<m_pMatchInfoBuffer->m_GameMatchOptionArray.GetCount();i++)
			{
				//��ȡ����
				tagGameMatchOption * pGameMatchOption=m_pMatchInfoBuffer->m_GameMatchOptionArray[i];
				if (pGameMatchOption->dwMatchID==m_pGameMatchOption->dwMatchID) continue;
			
				//�������
				int nIndex=pComboBox->AddString(pGameMatchOption->szMatchName);
				pComboBox->SetItemDataPtr(nIndex,pGameMatchOption);
				if (pGameMatchOption->dwMatchID==m_pGameMatchOption->dwFromMatchID)
				{
					if (m_pGameMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
					{
						pComboBox->SetCurSel(nIndex);
					}
				}
			}
		}

		//ģ��ѡ��
		OnCbnSelchangeComboFromMatch();
	}
	
	return FALSE;
}

//��������
bool CDlgMatchSignup1::SaveInputInfo()
{
	//�۳����
	if (IsDlgButtonChecked(IDC_CHECK_SIGNUP_FEE))
	{
		//���ö��
		m_pGameMatchOption->cbSignupMode|=SIGNUP_MODE_SIGNUP_FEE;
		m_pGameMatchOption->lSignupFee=GetDlgItemInt(IDC_EDIT_SIGNUP_FEE);

		//�ɷ�����
		if (IsDlgButtonChecked(IDC_RADIO_SERVER)) m_pGameMatchOption->cbDeductArea=DEDUCT_AREA_SERVER;
		if (IsDlgButtonChecked(IDC_RADIO_WEBSITE)) m_pGameMatchOption->cbDeductArea=DEDUCT_AREA_WEBSITE;

		//��������
		if (IsDlgButtonChecked(IDC_RADIO_GOLD)) m_pGameMatchOption->cbFeeType=FEE_TYPE_GOLD;
		if (IsDlgButtonChecked(IDC_RADIO_MEDAL)) m_pGameMatchOption->cbFeeType=FEE_TYPE_MEDAL;
	}
	else
	{
		m_pGameMatchOption->cbSignupMode&=~SIGNUP_MODE_SIGNUP_FEE;
	}

	//�����û�
	if (IsDlgButtonChecked(IDC_CHECK_MATCH_USER))
	{
		m_pGameMatchOption->cbSignupMode|=SIGNUP_MODE_MATCH_USER;

		//��ȡ�ؼ�
		CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_FROM_MATCH);
		if (pComboBox!=NULL && pComboBox->GetCurSel()!=LB_ERR)
		{
			tagGameMatchOption * pGameMatchOption=(tagGameMatchOption *)pComboBox->GetItemData(pComboBox->GetCurSel());
			if (pGameMatchOption!=NULL) m_pGameMatchOption->dwFromMatchID=pGameMatchOption->dwMatchID;
		}

		//��������
		CDateTimeCtrl * pEndtDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_END);
		pEndtDate->GetTime(&m_pGameMatchOption->MatchEndDate);

		//��ʼ���� 
		CDateTimeCtrl * pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_DATE_MATCH_START);
		pStartDate->GetTime(&m_pGameMatchOption->MatchStartDate);

		//��������
		if (IsDlgButtonChecked(IDC_RADIO_SINGLE_TURN))
		{			
			m_pGameMatchOption->cbFilterType=FILTER_TYPE_SINGLE_TURN;
			m_pGameMatchOption->wMaxRankID=GetDlgItemInt(IDC_EDIT_SINGLE_RANK);
		}

		//��������
		if (IsDlgButtonChecked(IDC_RADIO_TOTAL_RANKING))
		{
			m_pGameMatchOption->cbFilterType=FILTER_TYPE_TOTAL_RANKING;
			m_pGameMatchOption->wMaxRankID=GetDlgItemInt(IDC_EDIT_TOTAL_RANK);
		}
	}
	else
	{
		m_pGameMatchOption->cbSignupMode&=~SIGNUP_MODE_MATCH_USER;
	}

	//������ʽ
	if (m_pGameMatchOption->cbSignupMode==0)
	{
		AfxMessageBox(TEXT("������ʽ����Ϊ�գ�������ѡ��"));

		return false;
	}

	//�����û�
	if (m_pGameMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
	{
		if (m_pGameMatchOption->dwFromMatchID==0)
		{
			AfxMessageBox(TEXT("����ѡ�˲μ��ض�������Ϊ������ʽ������ѡ��һ��������ȷ�������û�����Դ��"));

			return false;
		}

		//���ұ���
		tagGameMatchOption * pGameMatchOption=m_pMatchInfoBuffer->SearchMatchOption(m_pGameMatchOption->dwFromMatchID);
		if (pGameMatchOption!=NULL)
		{
			if (pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
			{
				if (m_pGameMatchOption->cbFilterType!=FILTER_TYPE_SINGLE_TURN)
				{
					AfxMessageBox(TEXT("��ѡ���˼�ʱ����Ϊ�����û�����Դ��ɸѡ��ʽ����Ϊ���ֱ���������"));

					return false;
				}
			}

			if (pGameMatchOption->cbMatchType==MATCH_TYPE_LOCKTIME)
			{
				if (m_pGameMatchOption->cbFilterType!=FILTER_TYPE_TOTAL_RANKING)
				{
					AfxMessageBox(TEXT("��ѡ���˶�ʱ����Ϊ�����û�����Դ��ɸѡ��ʽ����Ϊ������������"));

					return false;
				}
			}
		}
	}

	//�����ж�
	if (m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE && m_pGameMatchOption->cbSignupMode&SIGNUP_MODE_MATCH_USER)
	{
		AfxMessageBox(TEXT("��ʱ������ѡ����������Ϊ������ʽ��������ѡ��"));

		return false;
	}

	return true;
}

//�����ؼ�
VOID CDlgMatchSignup1::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//���ͱ��
VOID CDlgMatchSignup1::OnEventMatchTypeChanged(BYTE cbMatchType)
{
	//���ÿؼ�
	CButton * pCheckButton=(CButton *)GetDlgItem(IDC_CHECK_MATCH_USER);
	if (pCheckButton!=NULL)
	{
		pCheckButton->SetCheck(FALSE);
		pCheckButton->EnableWindow(cbMatchType==MATCH_TYPE_LOCKTIME);
	}	
}

//ѡ����
void CDlgMatchSignup1::OnCbnSelchangeComboFromMatch()
{
	//��ȡ����
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_FROM_MATCH);
	if (pComboBox!=NULL && pComboBox->GetCurSel()!=LB_ERR)
	{
		tagGameMatchOption * pGameMatchOption=(tagGameMatchOption *)pComboBox->GetItemDataPtr(pComboBox->GetCurSel());

		//���ÿؼ�
		GetDlgItem(IDC_DATE_MATCH_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SINGLE_RANK)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_SINGLE_TURN)->EnableWindow(TRUE);		
		GetDlgItem(IDC_EDIT_TOTAL_RANK)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_TOTAL_RANKING)->EnableWindow(TRUE);				

		//��ʱ������
		if (pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
		{
			//���ÿؼ�
			GetDlgItem(IDC_EDIT_TOTAL_RANK)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_TOTAL_RANKING)->EnableWindow(FALSE);			

			//����ѡ��
			CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_SINGLE_TURN);
		}

		//��ʱ������
		if (pGameMatchOption->cbMatchType==MATCH_TYPE_LOCKTIME)
		{
			//���ÿؼ�
			GetDlgItem(IDC_DATE_MATCH_START)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_SINGLE_TURN)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_SINGLE_RANK)->EnableWindow(FALSE);

			//����ѡ��
			CheckRadioButton(IDC_RADIO_SINGLE_TURN,IDC_RADIO_TOTAL_RANKING,IDC_RADIO_TOTAL_RANKING);
		}		
	}	
}


//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchSignup2::CDlgMatchSignup2() : CDlgMatchItem(IDD_MATCH_SIGNUP2)
{
}

//��������
CDlgMatchSignup2::~CDlgMatchSignup2()
{
}

//��ʼ������
BOOL CDlgMatchSignup2::OnInitDialog()
{
	__super::OnInitDialog();

	//�û�����
	if (m_pGameMatchOption->cbJoinCondition&MATCH_JOINCD_EXPERIENCE)
	{
		//����ѡ��
		((CButton *)GetDlgItem(IDC_CHECK_EXPERIENCE))->SetCheck(TRUE);

		//���þ���
		SetDlgItemInt(IDC_EDIT_EXPERIENCE,m_pGameMatchOption->dwExperience);
	}

    //��Ա�ȼ�
	if (m_pGameMatchOption->cbJoinCondition&MATCH_JOINCD_MEMBER_ORDER)
	{
		//���ÿտؼ�
		((CButton *)GetDlgItem(IDC_CHECK_MEMBER_ORDER))->SetCheck(TRUE);
		CheckRadioButton(IDC_RADIO_NOAMAL,IDC_RADIO_MEMBER_V7,IDC_RADIO_NOAMAL+m_pGameMatchOption->cbMemberOrder);		
	}
	else
	{
		//Ĭ������
		CheckRadioButton(IDC_RADIO_NOAMAL,IDC_RADIO_MEMBER_V7,IDC_RADIO_NOAMAL);		
	}	

	return FALSE;
}

//��������
bool CDlgMatchSignup2::SaveInputInfo()
{
	//�û�����
	if (IsDlgButtonChecked(IDC_CHECK_EXPERIENCE))
	{
		m_pGameMatchOption->cbJoinCondition|=MATCH_JOINCD_EXPERIENCE;

		//���þ���
		m_pGameMatchOption->dwExperience=GetDlgItemInt(IDC_EDIT_EXPERIENCE);
	}
	else
	{
		m_pGameMatchOption->cbJoinCondition&=~MATCH_JOINCD_EXPERIENCE;
	}

	//��Ա�ȼ�
	if (IsDlgButtonChecked(IDC_CHECK_MEMBER_ORDER))
	{
		m_pGameMatchOption->cbJoinCondition|=MATCH_JOINCD_MEMBER_ORDER;		

		//���õȼ�	
		UINT nCheckedID=GetCheckedRadioButton(IDC_RADIO_NOAMAL,IDC_RADIO_MEMBER_V7);
		m_pGameMatchOption->cbMemberOrder=nCheckedID-IDC_RADIO_NOAMAL;
	}
	else
	{
		m_pGameMatchOption->cbJoinCondition&=~MATCH_JOINCD_MEMBER_ORDER;
	}	

	//������ʽ
	if (m_pGameMatchOption->cbSignupMode==0)
	{
		AfxMessageBox(TEXT("������ʽ����Ϊ�գ�������ѡ��"));

		return false;
	}

	return true;
}

//�����ؼ�
VOID CDlgMatchSignup2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchDistribute::CDlgMatchDistribute() : CDlgMatchItem(IDD_MATCH_DISTRIBUTE)
{
}

//��������
CDlgMatchDistribute::~CDlgMatchDistribute()
{
}

//��ʼ������
BOOL CDlgMatchDistribute::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�
	((CEdit *)GetDlgItem(IDC_DISTRIBUTE_MINCOUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_DISTRIBUTE_TIMESPACE))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_PARTAKE_MIN_GAMEUSER))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_PARTAKE_MAX_GAMEUSER))->LimitText(2);	

	//ͬ��ѡ��
	if ((m_pGameMatchOption->cbDistributeRule&DISTRIBUTE_LAST_TABLE)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_LAST_SAMETABLE))->SetCheck(TRUE);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_LAST_SAMETABLE))->SetCheck(FALSE);
	}

	//��ַѡ��
	if ((m_pGameMatchOption->cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)!=0)
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->SetCheck(TRUE);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->SetCheck(FALSE);
	}

	//��������
	if (m_pGameMatchOption->wMinDistributeUser!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_MINCOUNT,m_pGameMatchOption->wMinDistributeUser);
	}

	//������
	if (m_pGameMatchOption->wDistributeTimeSpace!=0)
	{
		SetDlgItemInt(IDC_DISTRIBUTE_TIMESPACE,m_pGameMatchOption->wDistributeTimeSpace);
	}

	//��С����
	if (m_pGameMatchOption->wMinPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_PARTAKE_MIN_GAMEUSER,m_pGameMatchOption->wMinPartakeGameUser);
	}

	//�������
	if (m_pGameMatchOption->wMaxPartakeGameUser!=0)
	{
		SetDlgItemInt(IDC_PARTAKE_MAX_GAMEUSER,m_pGameMatchOption->wMaxPartakeGameUser);
	}

	return FALSE;
}

//��������
bool CDlgMatchDistribute::SaveInputInfo()
{
	//��������
	m_pGameMatchOption->wMinDistributeUser=GetDlgItemInt(IDC_DISTRIBUTE_MINCOUNT);
	m_pGameMatchOption->wDistributeTimeSpace=GetDlgItemInt(IDC_DISTRIBUTE_TIMESPACE);
	m_pGameMatchOption->wMinPartakeGameUser=GetDlgItemInt(IDC_PARTAKE_MIN_GAMEUSER);
	m_pGameMatchOption->wMaxPartakeGameUser=GetDlgItemInt(IDC_PARTAKE_MAX_GAMEUSER);

	//ͬ��ѡ��
	if (((CButton *)GetDlgItem(IDC_ALLOW_LAST_SAMETABLE))->GetCheck()==BST_CHECKED)
	{
		m_pGameMatchOption->cbDistributeRule|=DISTRIBUTE_LAST_TABLE;
	}
	else
	{
		m_pGameMatchOption->cbDistributeRule&=~DISTRIBUTE_LAST_TABLE;
	}

	//��ַѡ��
	if (((CButton *)GetDlgItem(IDC_ALLOW_SAME_ADDRESS))->GetCheck()==BST_CHECKED)
	{
		m_pGameMatchOption->cbDistributeRule|=DISTRIBUTE_SAME_ADDRESS;
	}
	else
	{
		m_pGameMatchOption->cbDistributeRule&=~DISTRIBUTE_SAME_ADDRESS;
	}

	return true;
}

//�����ؼ�
VOID CDlgMatchDistribute::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}


//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchRankingMode::CDlgMatchRankingMode() : CDlgMatchItem(IDD_MATCH_RANKING_MODE)
{
}

//��������
CDlgMatchRankingMode::~CDlgMatchRankingMode()
{
}

//��ʼ������
BOOL CDlgMatchRankingMode::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�
	((CEdit *)GetDlgItem(IDC_EDIT_COUNT_INNINGS))->LimitText(3);

	//���ÿؼ�
	CheckRadioButton(IDC_RADIO_TOTAL_GRADES,IDC_RADIO_SPECIALLY_GRADES,IDC_RADIO_TOTAL_GRADES+(m_pGameMatchOption->cbRankingMode-RANKING_MODE_TOTAL_GRADES));

	//�ض���������
	if (IsDlgButtonChecked(IDC_RADIO_SPECIALLY_GRADES))
	{
		//ͳ�ƾ���
		SetDlgItemInt(IDC_EDIT_COUNT_INNINGS,m_pGameMatchOption->wCountInnings);

		//���ÿؼ�
		CheckRadioButton(IDC_RADIO_BEST_GRADES,IDC_RADIO_AVERAGEEX_GRADES,IDC_RADIO_BEST_GRADES+(m_pGameMatchOption->cbFilterGradesMode-FILTER_GRADES_MODE_BEST));
	}
	else
	{
		CheckRadioButton(IDC_RADIO_BEST_GRADES,IDC_RADIO_AVERAGEEX_GRADES,IDC_RADIO_BEST_GRADES);
	}

	return FALSE;
}

//��������
bool CDlgMatchRankingMode::SaveInputInfo()
{
	//������ʽ
	if (IsDlgButtonChecked(IDC_RADIO_TOTAL_GRADES)) m_pGameMatchOption->cbRankingMode=RANKING_MODE_TOTAL_GRADES;
	if (IsDlgButtonChecked(IDC_RADIO_SPECIALLY_GRADES)) m_pGameMatchOption->cbRankingMode=RANKING_MODE_SPECIAL_GRADES;

	//�ض�����
	if (m_pGameMatchOption->cbRankingMode==RANKING_MODE_SPECIAL_GRADES)
	{
		//ͳ�ƾ���
		m_pGameMatchOption->wCountInnings=GetDlgItemInt(IDC_EDIT_COUNT_INNINGS);

		//�Ϸ�У��
		if (m_pGameMatchOption->wCountInnings==0)
		{
			AfxMessageBox(TEXT("ͳ�ƾ������ò��Ϸ������������ã�"));

			return false;
		}

		//ɸѡģʽ
		if (IsDlgButtonChecked(IDC_RADIO_BEST_GRADES)) m_pGameMatchOption->cbFilterGradesMode=FILTER_GRADES_MODE_BEST;
		if (IsDlgButtonChecked(IDC_RADIO_AVERAGE_GRADES)) m_pGameMatchOption->cbFilterGradesMode=FILTER_GRADES_MODE_AVERAGE;
		if (IsDlgButtonChecked(IDC_RADIO_AVERAGEEX_GRADES)) m_pGameMatchOption->cbFilterGradesMode=FILTER_GRADES_MODE_AVERAGEEX;
	}	

	return true;
}

//�����ؼ�
VOID CDlgMatchRankingMode::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//���ͱ��
VOID CDlgMatchRankingMode::OnEventMatchTypeChanged(BYTE cbMatchType)
{
	//���ÿؼ�
	GetDlgItem(IDC_RADIO_SPECIALLY_GRADES)->EnableWindow(cbMatchType==MATCH_TYPE_LOCKTIME);
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchReward::CDlgMatchReward() : CDlgMatchItem(IDD_MATCH_REWARD)
{
}

//��������
CDlgMatchReward::~CDlgMatchReward()
{
}

//��ʼ������
BOOL CDlgMatchReward::OnInitDialog()
{
	__super::OnInitDialog();

	//��ӽ���
	if (m_pRewardInfoBuffer!=NULL)
	{
		for(INT_PTR i=0;i<m_pRewardInfoBuffer->m_MatchRewardInfoArray.GetCount();i++) 
		{
			m_RewardListControl.InsertMatchReward(m_pRewardInfoBuffer->m_MatchRewardInfoArray[i]);
		}
	}

	return FALSE;
}

//�ؼ���
VOID CDlgMatchReward::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�����б�
	DDX_Control(pDX,IDC_LIST_REWARD,m_RewardListControl);
}

//��������
bool CDlgMatchReward::SaveInputInfo()
{
	return true;
}

//�����ؼ�
VOID CDlgMatchReward::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//��������
bool CDlgMatchReward::GetMatchRewardOption(tagMatchRewardInfo & MatchRewardInfo,WORD & wStartRankID,WORD & wEndRankID)
{
	//��ȡ����
	wEndRankID=GetDlgItemInt(IDC_RANKID_END);
	wStartRankID=GetDlgItemInt(IDC_RANKID_START);	

	//��������
	if (wEndRankID==0) wEndRankID=wStartRankID;

	//�������
	MatchRewardInfo.lRewardGold=GetDlgItemInt(IDC_REWARD_GOLD);

	//����Ԫ��
	MatchRewardInfo.lRewardIngot=GetDlgItemInt(IDC_REWARD_INGOT);

	//��������
	MatchRewardInfo.dwRewardExperience=GetDlgItemInt(IDC_REWARD_EXPERIENCE);

	//����У��
	if (wStartRankID==0 || wEndRankID<wStartRankID)
	{
		AfxMessageBox(TEXT("������ı������β��Ϸ������������룡"));

		//���ý���
		GetDlgItem(IDC_RANKID_START)->SetFocus();

		return false;
	}

	return true;
}

//��������
VOID CDlgMatchReward::SetMatchRewardOption(tagMatchRewardInfo * pMatchRewardInfo)
{
	//����У��
	ASSERT(pMatchRewardInfo!=NULL);
	if (pMatchRewardInfo==NULL) return;	

	//��������
	SetDlgItemInt(IDC_RANKID_END,(UINT)pMatchRewardInfo->wRankID);
	SetDlgItemInt(IDC_RANKID_START,(UINT)pMatchRewardInfo->wRankID);	

	//��ҽ���
	SetDlgItemInt(IDC_REWARD_GOLD,(UINT)pMatchRewardInfo->lRewardGold);

	//���ƽ���
	SetDlgItemInt(IDC_REWARD_INGOT,(UINT)pMatchRewardInfo->lRewardIngot);

	//���齱��
	SetDlgItemInt(IDC_REWARD_EXPERIENCE,(UINT)pMatchRewardInfo->dwRewardExperience);
}


//��ӽ���
VOID CDlgMatchReward::OnBnClickedAddReward()
{
	//��������
	WORD wStartRankID,wEndRankID;
	tagMatchRewardInfo MatchRewardInfo;

	//��ȡ����
	if (GetMatchRewardOption(MatchRewardInfo,wStartRankID,wEndRankID)==false) return;

	for (WORD wRankID=wStartRankID; wRankID<=wEndRankID; wRankID++)
	{
		//��������
		MatchRewardInfo.wRankID=wRankID;

		//�����ж�
		if (m_pRewardInfoBuffer->SearchMatchReward(MatchRewardInfo.wRankID)!=NULL)
		{
			AfxMessageBox(TEXT("��Ǹ�������εĽ����Ѵ���,��Ҫ�޸������޸Ľ�����ť��"));

			return;
		}

		//���뽱��
		tagMatchRewardInfo * pMatchRewardInfo=NULL;
		if (pMatchRewardInfo=m_pRewardInfoBuffer->InsertMatchRewardInfo(MatchRewardInfo))
		{
			m_RewardListControl.InsertMatchReward(pMatchRewardInfo);
		}
	}
}

//�޸Ľ���
VOID CDlgMatchReward::OnBnClickedModifyReward()
{
	//��������
	WORD wStartRankID,wEndRankID;
	tagMatchRewardInfo MatchRewardInfo;

	//��ȡ����
	if (GetMatchRewardOption(MatchRewardInfo,wStartRankID,wEndRankID)==false) return;

	for (WORD wRankID=wStartRankID; wRankID<=wEndRankID; wRankID++)
	{
		//��������
		MatchRewardInfo.wRankID=wRankID;

		//�����ж�
		tagMatchRewardInfo * pMatchRewardInfo=m_pRewardInfoBuffer->SearchMatchReward(wRankID);
		if (pMatchRewardInfo==NULL)
		{
			AfxMessageBox(TEXT("��Ǹ�������εĽ���������,��Ҫ���������ӽ�����ť��"));
			return;
		}

		//��������
		CopyMemory(pMatchRewardInfo,&MatchRewardInfo,sizeof(tagMatchRewardInfo));

		//���¿ؼ�
		m_RewardListControl.UpdateMatchReward(pMatchRewardInfo);
	}
}

//ɾ������
VOID CDlgMatchReward::OnBnClickedDeleteReward()
{
	//��������
	WORD wStartRankID,wEndRankID;
	tagMatchRewardInfo MatchRewardInfo;

	//��ȡ����
	if (GetMatchRewardOption(MatchRewardInfo,wStartRankID,wEndRankID)==false) return;

	for (WORD wRankID=wStartRankID; wRankID<=wEndRankID; wRankID++)
	{
		//��������
		MatchRewardInfo.wRankID=wRankID;

		//�����ж�
		tagMatchRewardInfo * pMatchRewardInfo=m_pRewardInfoBuffer->SearchMatchReward(wRankID);
		if (pMatchRewardInfo==NULL)
		{
			AfxMessageBox(TEXT("��Ǹ�������εĽ���������,��Ҫ���������ӽ�����ť��"));
			return;
		}

		//���¿ؼ�
		m_RewardListControl.DeleteMatchReward(pMatchRewardInfo);
		m_pRewardInfoBuffer->DeleteMatchRewardInfo(pMatchRewardInfo->wRankID);
	}
}

//������
VOID CDlgMatchReward::OnLvnItemchangedListReward(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	//��������
	if (pNMLV->iItem!=LB_ERR)
	{
		//��������
		tagMatchRewardInfo * pMatchRewardInfo=(tagMatchRewardInfo *)m_RewardListControl.GetItemData(pNMLV->iItem);
		if (pMatchRewardInfo!=NULL) SetMatchRewardOption(pMatchRewardInfo);
	}

	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchLockTime::CDlgMatchLockTime() : CDlgMatchItem(IDD_MATCH_LOCKTIME)
{
}

//��������
CDlgMatchLockTime::~CDlgMatchLockTime()
{
}

//��ʼ������
BOOL CDlgMatchLockTime::OnInitDialog()
{
	__super::OnInitDialog();

	//��������
	((CEdit *)GetDlgItem(IDC_INIT_SCORE))->LimitText(8);
	((CEdit *)GetDlgItem(IDC_CULL_SCORE))->LimitText(8);
	((CEdit *)GetDlgItem(IDC_MIN_PLAYCOUNT))->LimitText(4);
	((CEdit *)GetDlgItem(IDC_MAX_PLAYCOUNT))->LimitText(4);
	((CEdit *)GetDlgItem(IDD_END_DELAY_TIME))->LimitText(8);

	//��������
	tagLockTimeMatch * pLockTimeMatch=(tagLockTimeMatch *)m_pGameMatchOption->cbMatchRule;

	//����ģʽ
	LPCTSTR pszMatchMode[]={TEXT("���濪��"), TEXT("ѭ������")};
	BYTE cbMatchMode[]={MATCH_MODE_NORMAL, MATCH_MODE_ROUND};

	//��ȡ�ؼ�
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_MODE);

	//���ÿؼ�
	for (int i=0; i<CountArray(pszMatchMode); i++)
	{
		int nIndex=pComboBox->SetItemData(pComboBox->InsertString(i,pszMatchMode[i]),cbMatchMode[i]);
		if (cbMatchMode[i]==pLockTimeMatch->cbMatchMode)
		{
			pComboBox->SetCurSel(nIndex);
		}
	}

	//Ĭ������
	if (pComboBox->GetCurSel()==-1) pComboBox->SetCurSel(0);

	//��������
	if (pLockTimeMatch->wStartUserCount!=0)
	{
		SetDlgItemInt(IDC_MIN_MATCH_USER,(UINT)pLockTimeMatch->wStartUserCount);
	}

	//��������
	if (pLockTimeMatch->wMaxSignupUser!=0)
	{
		SetDlgItemInt(IDD_MAX_SIGNUP_USER,(UINT)pLockTimeMatch->wMaxSignupUser);
	}

	//��ʼ����
	if (pLockTimeMatch->lMatchInitScore!=0)
	{
		SetDlgItemInt(IDC_INIT_SCORE,(UINT)pLockTimeMatch->lMatchInitScore);
	}

	//��̭����
	if (pLockTimeMatch->lMatchCullScore!=0)
	{
		SetDlgItemInt(IDC_CULL_SCORE,(UINT)pLockTimeMatch->lMatchCullScore);
	}

	//��Ч����
	if (pLockTimeMatch->wMinPlayCount!=0)
	{
		SetDlgItemInt(IDC_MIN_PLAYCOUNT,pLockTimeMatch->wMinPlayCount);
	}

	//������
	if (pLockTimeMatch->wMaxPlayCount!=0)
	{
		SetDlgItemInt(IDC_MAX_PLAYCOUNT,pLockTimeMatch->wMaxPlayCount);
	}

	//��������
	if (pLockTimeMatch->wEndDelayTime!=0)
	{
		SetDlgItemInt(IDD_END_DELAY_TIME,(UINT)pLockTimeMatch->wEndDelayTime);
	}
    
	//����ʱ��
	SYSTEMTIME MatchEndTime=pLockTimeMatch->MatchEndTime;
	SYSTEMTIME MatchStartTime=pLockTimeMatch->MatchStartTime;	
	SYSTEMTIME SignupEndTime=pLockTimeMatch->SignupEndTime;	
	SYSTEMTIME SignupStartTime=pLockTimeMatch->SignupStartTime;	

	//��ʼ����
	CDateTimeCtrl * pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_DATE);
	pStartDate->SetTime(&MatchStartTime);

	//��������
	CDateTimeCtrl * pEndDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_END_DATE);
	pEndDate->SetTime(&MatchEndTime);

	//��ʼʱ��
	CDateTimeCtrl * pStartTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_TIME);
	pStartTime->SetTime(&MatchStartTime);

	//����ʱ��
	CDateTimeCtrl * pEndTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_END_TIME);
	pEndTime->SetTime(&MatchEndTime);

	//��ʼ����
	CDateTimeCtrl * pSignupStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_SIGNUP_START_DATE);
	pSignupStartDate->SetTime(&SignupStartTime);

	//��ʼʱ��
	CDateTimeCtrl * pSignupStartTime=(CDateTimeCtrl *)GetDlgItem(IDC_SIGNUP_START_TIME);
	pSignupStartTime->SetTime(&SignupStartTime);

	//��ֹ����
	CDateTimeCtrl * pSignupEndDate=(CDateTimeCtrl *)GetDlgItem(IDC_SIGNUP_END_DATE);
	pSignupEndDate->SetTime(&SignupEndTime);

	//��ֹʱ��
	CDateTimeCtrl * pSignupEndTime=(CDateTimeCtrl *)GetDlgItem(IDC_SIGNUP_END_TIME);
	pSignupEndTime->SetTime(&SignupEndTime);

	//�������
	if (pLockTimeMatch->cbReviveEnabled)
	{
		//���տ�����
		if (pLockTimeMatch->cbSafeCardEnabled)
		{
			((CButton *)GetDlgItem(IDC_CHECK_SAFECARD_ENABLED))->SetCheck(TRUE);
		}

		//���ÿؼ�
		((CButton *)GetDlgItem(IDC_CHECK_REVIVE_ENABLED))->SetCheck(TRUE);

		//���ÿؼ�
		SetDlgItemInt(IDC_REVIVE_FEE,(int)pLockTimeMatch->lReviveFee);
		SetDlgItemInt(IDC_SAFECARD_FEE,(int)pLockTimeMatch->lSafeCardFee);		
		SetDlgItemInt(IDC_REVIVE_TIMES,pLockTimeMatch->cbReviveTimes);		
	}

	return FALSE;
}

//��������
bool CDlgMatchLockTime::SaveInputInfo()
{
	//��������
	tagLockTimeMatch * pLockTimeMatch=(tagLockTimeMatch *)m_pGameMatchOption->cbMatchRule;

	//��ȡ�ؼ�
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_MODE);

	//��������
	pLockTimeMatch->lMatchInitScore=GetDlgItemInt(IDC_INIT_SCORE);
	pLockTimeMatch->lMatchCullScore=GetDlgItemInt(IDC_CULL_SCORE);
	pLockTimeMatch->wMinPlayCount=GetDlgItemInt(IDC_MIN_PLAYCOUNT);
	pLockTimeMatch->wMaxPlayCount=GetDlgItemInt(IDC_MAX_PLAYCOUNT);
	pLockTimeMatch->wStartUserCount=GetDlgItemInt(IDC_MIN_MATCH_USER);
	pLockTimeMatch->wMaxSignupUser=GetDlgItemInt(IDD_MAX_SIGNUP_USER);
	pLockTimeMatch->wEndDelayTime=GetDlgItemInt(IDD_END_DELAY_TIME);	
	pLockTimeMatch->cbMatchMode=(BYTE)(pComboBox->GetItemData(pComboBox->GetCurSel()));

	//��������
	if (pLockTimeMatch->wStartUserCount==0)
	{
		AfxMessageBox(TEXT("�����������ò��Ϸ������������룡"));

		GetDlgItem(IDC_MIN_MATCH_USER)->SetFocus();

		return false;
	}

	//��������
	if (pLockTimeMatch->wMaxSignupUser<pLockTimeMatch->wStartUserCount)
	{
		AfxMessageBox(TEXT("���������������ڿ������������������룡"));

		GetDlgItem(IDD_MAX_SIGNUP_USER)->SetFocus();

		return false;
	}

	//��ʼ����
	if (pLockTimeMatch->lMatchInitScore==0)
	{
		AfxMessageBox(TEXT("��ʼ�������ò��Ϸ������������룡"));

		GetDlgItem(IDC_INIT_SCORE)->SetFocus();

		return false;
	}

	//��Ч����
	if (pLockTimeMatch->wMinPlayCount==0)
	{
		AfxMessageBox(TEXT("��Ч�������ò��Ϸ������������룡"));

		GetDlgItem(IDC_MIN_PLAYCOUNT)->SetFocus();

		return false;
	}

	
	//������
	if (pLockTimeMatch->wMaxPlayCount!=0 && pLockTimeMatch->wMaxPlayCount<pLockTimeMatch->wMinPlayCount)
	{
		AfxMessageBox(TEXT("���������ò��Ϸ������������룡"));

		GetDlgItem(IDC_MIN_PLAYCOUNT)->SetFocus();

		return false;
	}
	
    
	//ʱ�䶨��
	CTimeSpan DateSpad,TimeSpad;
	CTime StartTime,StartDate,EndTime,EndDate;		
	CDateTimeCtrl * pStartDate,* pEndDate,* pStartTime,* pEndTime;
    
	//��ʼ����
	pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_DATE);
	pStartDate->GetTime(StartDate);

	//��������
	pEndDate=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_END_DATE);
	pEndDate->GetTime(EndDate);

	//��ʼʱ��
	pStartTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_START_TIME);
	pStartTime->GetTime(StartTime);

	//����ʱ��
	pEndTime=(CDateTimeCtrl *)GetDlgItem(IDC_MATCH_END_TIME);
	pEndTime->GetTime(EndTime);

	//����У��
	DateSpad=EndDate-StartDate;
	if (DateSpad.GetTimeSpan() < 0)
	{
		AfxMessageBox(TEXT("���������������ò��Ϸ������������룡"));

		GetDlgItem(IDC_MATCH_END_DATE)->SetFocus();

		return false;
	}

	//ʱ��У��
	TimeSpad=EndTime-StartTime;
	if (TimeSpad.GetTimeSpan() < 0)
	{
		AfxMessageBox(TEXT("��������ʱ�����ò��Ϸ������������룡"));

		GetDlgItem(IDC_MATCH_END_TIME)->SetFocus();

		return false;
	}

	//�ϲ�ʱ��
	CTime EndDateTime(EndDate.GetYear(),EndDate.GetMonth(),EndDate.GetDay(),EndTime.GetHour(),EndTime.GetMinute(),EndTime.GetSecond());
	CTime StartDateTime(StartDate.GetYear(),StartDate.GetMonth(),StartDate.GetDay(),StartTime.GetHour(),StartTime.GetMinute(),StartTime.GetSecond());	

	//����ʱ��
	EndDateTime.GetAsSystemTime(pLockTimeMatch->MatchEndTime);
	StartDateTime.GetAsSystemTime(pLockTimeMatch->MatchStartTime);
	
	//��ʼ����
	pStartDate=(CDateTimeCtrl *)GetDlgItem(IDC_SIGNUP_START_DATE);
	pStartDate->GetTime(StartDate);

	//��������
	pEndDate=(CDateTimeCtrl *)GetDlgItem(IDC_SIGNUP_END_DATE);
	pEndDate->GetTime(EndDate);

	//��ʼʱ��
	pStartTime=(CDateTimeCtrl *)GetDlgItem(IDC_SIGNUP_START_TIME);
	pStartTime->GetTime(StartTime);

	//����ʱ��
	pEndTime=(CDateTimeCtrl *)GetDlgItem(IDC_SIGNUP_END_TIME);
	pEndTime->GetTime(EndTime);

	//����У��
	DateSpad=EndDate-StartDate;
	if (DateSpad.GetTimeSpan() < 0)
	{
		AfxMessageBox(TEXT("������ֹ�������ò��Ϸ������������룡"));

		GetDlgItem(IDC_SIGNUP_END_DATE)->SetFocus();

		return false;
	}

	//ʱ��У��
	TimeSpad=EndTime-StartTime;
	if (TimeSpad.GetTimeSpan() < 0)
	{
		AfxMessageBox(TEXT("������ֹʱ�����ò��Ϸ������������룡"));

		GetDlgItem(IDC_SIGNUP_END_TIME)->SetFocus();

		return false;
	}

	//�ϲ�ʱ��
	CTime SignupEndDateTime(EndDate.GetYear(),EndDate.GetMonth(),EndDate.GetDay(),EndTime.GetHour(),EndTime.GetMinute(),EndTime.GetSecond());
	CTime SignuStartDateTime(StartDate.GetYear(),StartDate.GetMonth(),StartDate.GetDay(),StartTime.GetHour(),StartTime.GetMinute(),StartTime.GetSecond());	

	//����ʱ��
	SignupEndDateTime.GetAsSystemTime(pLockTimeMatch->SignupEndTime);
	SignuStartDateTime.GetAsSystemTime(pLockTimeMatch->SignupStartTime);

	//��������
	if (IsDlgButtonChecked(IDC_CHECK_REVIVE_ENABLED))
	{
		//���ñ���
		pLockTimeMatch->cbReviveEnabled=TRUE;		

		//���տ�����
		if (IsDlgButtonChecked(IDC_CHECK_SAFECARD_ENABLED)) 
		{
			pLockTimeMatch->cbSafeCardEnabled=TRUE;
		}
		else
		{
			pLockTimeMatch->cbSafeCardEnabled=FALSE;
		}

		//�������
		pLockTimeMatch->lReviveFee=GetDlgItemInt(IDC_REVIVE_FEE);
		pLockTimeMatch->lSafeCardFee=GetDlgItemInt(IDC_SAFECARD_FEE);		
		pLockTimeMatch->cbReviveTimes=GetDlgItemInt(IDC_REVIVE_TIMES);
	}
	else
	{
		pLockTimeMatch->cbReviveEnabled=FALSE;
		pLockTimeMatch->cbSafeCardEnabled=FALSE;
	}

	return true;
}

//�����ؼ�
VOID CDlgMatchLockTime::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchImmediate::CDlgMatchImmediate(): CDlgMatchItem(IDD_MATCH_IMMEDIATE)
{
}

//��������
CDlgMatchImmediate::~CDlgMatchImmediate()
{
}

//��ʼ������
BOOL CDlgMatchImmediate::OnInitDialog()
{
	__super::OnInitDialog();

	//��������
	((CEdit *)GetDlgItem(IDC_START_USERCOUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_ANDROID_USERCOUNT))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_INITIAL_BASE))->LimitText(8);
	((CEdit *)GetDlgItem(IDC_INITIAL_SCORE))->LimitText(8);
	((CEdit *)GetDlgItem(IDC_PLAY_COUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_SWITCH_TABLE_COUNT))->LimitText(3);

	//��������
	tagImmediateMatch * pImmediateMatch=(tagImmediateMatch *)m_pGameMatchOption->cbMatchRule;

	//��������
	if (pImmediateMatch->wStartUserCount!=0)
	{
		SetDlgItemInt(IDC_START_USERCOUNT,pImmediateMatch->wStartUserCount);
	}

	//��������
	if (pImmediateMatch->wAndroidUserCount!=0)
	{
		SetDlgItemInt(IDC_ANDROID_USERCOUNT,pImmediateMatch->wAndroidUserCount);
	}

	//�����ȴ�
	if (pImmediateMatch->wAndroidDelaySignupTime!=0)
	{
		SetDlgItemInt(IDC_ANDROID_DELAYTIME,pImmediateMatch->wAndroidDelaySignupTime);
	}

	//��Ϸ�׷�
	if (pImmediateMatch->lInitalBase!=0)
	{
		SetDlgItemInt(IDC_INITIAL_BASE,(UINT)pImmediateMatch->lInitalBase);
	}

	//��ʼ�׷�
	if (pImmediateMatch->lInitalScore!=0)
	{
		SetDlgItemInt(IDC_INITIAL_SCORE,(UINT)pImmediateMatch->lInitalScore);
	}

	//��Ϸ����
	if (pImmediateMatch->wPlayCount!=0)
	{
		SetDlgItemInt(IDC_PLAY_COUNT,pImmediateMatch->wPlayCount);
	}

	//��������
	if (pImmediateMatch->cbSwitchTableCount!=0)
	{
		SetDlgItemInt(IDC_SWITCH_TABLE_COUNT,pImmediateMatch->cbSwitchTableCount);
	}

	return TRUE;
}

//��������
bool CDlgMatchImmediate::SaveInputInfo()
{
	//��������
	tagImmediateMatch * pImmediateMatch=(tagImmediateMatch *)m_pGameMatchOption->cbMatchRule;

	//��������
	pImmediateMatch->wPlayCount=GetDlgItemInt(IDC_PLAY_COUNT);	
	pImmediateMatch->lInitalBase=GetDlgItemInt(IDC_INITIAL_BASE);
	pImmediateMatch->lInitalScore=GetDlgItemInt(IDC_INITIAL_SCORE);
	pImmediateMatch->wStartUserCount=GetDlgItemInt(IDC_START_USERCOUNT);
	pImmediateMatch->wAndroidUserCount=GetDlgItemInt(IDC_ANDROID_USERCOUNT);	
	pImmediateMatch->cbSwitchTableCount=GetDlgItemInt(IDC_SWITCH_TABLE_COUNT);
	pImmediateMatch->wAndroidDelaySignupTime=GetDlgItemInt(IDC_ANDROID_DELAYTIME);

	//��������
	if (pImmediateMatch->wStartUserCount==0)
	{
		AfxMessageBox(TEXT("�����������ò��Ϸ������������룡"));

		//���ý���
		GetDlgItem(IDC_START_USERCOUNT)->SetFocus();

		return false;
	}

	//��Ϸ�׷�
	if (pImmediateMatch->lInitalBase==0)
	{
		AfxMessageBox(TEXT("��Ϸ�׷����ò��Ϸ������������룡"));

		//���ý���
		GetDlgItem(IDC_INITIAL_BASE)->SetFocus();

		return false;
	}

	//��ʼ�׷�
	if (pImmediateMatch->lInitalScore==0)
	{
		AfxMessageBox(TEXT("��ʼ�׷����ò��Ϸ������������룡"));

		//���ý���
		GetDlgItem(IDC_INITIAL_SCORE)->SetFocus();

		return false;
	}

	//��Ϸ����
	if (pImmediateMatch->wPlayCount==0)
	{
		AfxMessageBox(TEXT("��Ϸ�������ò��Ϸ������������룡"));

		//���ý���
		GetDlgItem(IDC_PLAY_COUNT)->SetFocus();

		return false;
	}

	return true;
}

//�����ؼ�
VOID CDlgMatchImmediate::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchWizardItem::CDlgMatchWizardItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//���ñ���
	m_bSaveData=true;

	//���ñ���
	m_pDlgMatchWizard=NULL;
	m_pGameMatchOption=NULL;
	m_pMatchInfoBuffer=NULL;
	m_pRewardInfoBuffer=NULL;

	return;
}

//��������
CDlgMatchWizardItem::~CDlgMatchWizardItem()
{
}

//��������
bool CDlgMatchWizardItem::SaveItemData()
{
	//��������
	if ((m_hWnd!=NULL)&&(m_bSaveData==false))
	{
		//��������
		m_bSaveData=SaveInputInfo();

		//����ж�
		if (m_bSaveData==false) return false;
	}

	return true;
}

//������
bool CDlgMatchWizardItem::ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//��������
	if (m_hWnd==NULL) 
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//��������
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//������Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//���ñ���
	m_bSaveData=false;

	//��������
	OnMatchItemActive();

	//��ʾ����
	ShowWindow(SW_SHOW);

	return true;
}

//ȷ������
VOID CDlgMatchWizardItem::OnOK() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//ȡ����Ϣ
VOID CDlgMatchWizardItem::OnCancel() 
{ 
	//Ͷ����Ϣ
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//λ����Ϣ
VOID CDlgMatchWizardItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchWizardStep1::CDlgMatchWizardStep1() : CDlgMatchWizardItem(IDD_MATCH_WIZARD_1)
{
	//��������
	m_cbItemIndex=0xff;
	ZeroMemory(m_pDlgMatchItem,sizeof(m_pDlgMatchItem));

	return;
}

//��������
CDlgMatchWizardStep1::~CDlgMatchWizardStep1()
{
}

//�ؼ���
VOID CDlgMatchWizardStep1::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//��ʼ������
BOOL CDlgMatchWizardStep1::OnInitDialog()
{
	__super::OnInitDialog();

	//��ʼ������
	InitBaseOption();

	//��ȡ�ؼ�
	CTabCtrl * pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_MATCH_OPTION);

	//��������
	pTabCtrl->InsertItem(TAB_OPTION_SIGNUP1,TEXT("������ʽ"));
	pTabCtrl->InsertItem(TAB_OPTION_SIGNUP2,TEXT("��������"));
	pTabCtrl->InsertItem(TAB_OPTION_RANKINGMODE,TEXT("������ʽ"));
	pTabCtrl->InsertItem(TAB_OPTION_DISTRIBUTE,TEXT("��������"));	
	pTabCtrl->InsertItem(TAB_OPTION_REWARD,TEXT("��������"));	

	//���ô�С
	pTabCtrl->SetItemSize(CSize(120,25));

	//�������
	m_pDlgMatchItem[0]=&m_DlgMatchSignup1;
	m_pDlgMatchItem[1]=&m_DlgMatchSignup2;
	m_pDlgMatchItem[2]=&m_DlgMatchRankingMode;
	m_pDlgMatchItem[3]=&m_DlgMatchDistribute;	
	m_pDlgMatchItem[4]=&m_DlgMatchReward;	

	//��������
	CRect rcCreate(0,0,0,0);

	//��������
	for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
	{
		//ָ���ж�
		if (m_pDlgMatchItem[i]==NULL) continue;

		//���ÿؼ�		
		m_pDlgMatchItem[i]->SetGameMatchOption(m_pGameMatchOption);
		m_pDlgMatchItem[i]->SetMatchInfoBuffer(m_pMatchInfoBuffer);
		m_pDlgMatchItem[i]->SetRewardInfoBuffer(m_pRewardInfoBuffer);

		//��������		
		m_pDlgMatchItem[i]->CreateMatchItem(rcCreate,pTabCtrl);
	}

	//��������
	ActiveOptionItem(TAB_OPTION_SIGNUP1);

	return TRUE;
}

//��������
bool CDlgMatchWizardStep1::SaveInputInfo()
{
	//��������
	if (SaveBaseOption()==false) return false;

	//��������
	for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
	{
		if (m_pDlgMatchItem[i]->SaveInputInfo() == false) return false;
	}

	return true;
}

//��������
VOID CDlgMatchWizardStep1::OnMatchItemActive()
{
	return;
}

//�����ؼ�
VOID CDlgMatchWizardStep1::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//��������
VOID CDlgMatchWizardStep1::InitBaseOption()
{
	//��������
	((CEdit *)GetDlgItem(IDC_EDIT_MATCH_NAME))->LimitText(32);		

	//��������
	SetDlgItemText(IDC_EDIT_MATCH_NAME,m_pGameMatchOption->szMatchName);

	//��������
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_TYPE);
	if (pComboBox!=NULL)
	{
		//��������
		BYTE cbMatchType[]={MATCH_TYPE_LOCKTIME,MATCH_TYPE_IMMEDIATE};
		LPCTSTR pszMatchType[]={TEXT("��ʱ������"),TEXT("��ʱ������")};

		//��������
		for (INT i=0;i<CountArray(cbMatchType);i++)
		{
			int nIndex=pComboBox->AddString(pszMatchType[i]);
			pComboBox->SetItemData(nIndex,cbMatchType[i]);
			if (m_pGameMatchOption->cbMatchType==cbMatchType[i])
			{
				pComboBox->SetCurSel(nIndex);
			}
		}

		//Ĭ��ѡ��
		if (pComboBox->GetCurSel()==CB_ERR) pComboBox->SetCurSel(0);
		if (m_pGameMatchOption->dwMatchID!=0) pComboBox->EnableWindow(FALSE);
	}

	//��������
	//pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_RELATED_MATCH);
	//if (pComboBox!=NULL)
	//{
	//	//Ĭ������
	//	pComboBox->EnableWindow(FALSE);

	//	//��ʱ��
	//	if (m_pGameMatchOption->cbMatchType==MATCH_TYPE_LOCKTIME)
	//	{
	//		//���ر���
	//		if (m_pMatchInfoBuffer!=NULL)
	//		{
	//			//��������
	//			tagGameMatchOption * pGameMatchOption=NULL;

	//			for(INT_PTR i=0;i<m_pMatchInfoBuffer->m_GameMatchOptionArray.GetCount();i++)
	//			{
	//				//��ȡ����
	//				pGameMatchOption=m_pMatchInfoBuffer->m_GameMatchOptionArray[i];
	//				if (pGameMatchOption->cbMatchType!=MATCH_TYPE_LOCKTIME) continue;
	//				if (pGameMatchOption->dwMatchID==m_pGameMatchOption->dwMatchID) continue;

	//				//��������
	//				bool bBreakLoop=false;
	//				tagGameMatchOption * pGameMatchOption1=NULL;
	//				for(INT_PTR j=0;j<m_pMatchInfoBuffer->m_GameMatchOptionArray.GetCount();j++)
	//				{
	//					//��ȡ����
	//					pGameMatchOption1=m_pMatchInfoBuffer->m_GameMatchOptionArray[j];
	//					if (pGameMatchOption->cbMatchType!=MATCH_TYPE_LOCKTIME) continue;

	//					if ((pGameMatchOption1->dwRelatedMatchID==pGameMatchOption->dwMatchID) && 
	//						(pGameMatchOption1->dwMatchID!=m_pGameMatchOption->dwMatchID))
	//					{
	//						bBreakLoop=true;
	//						break;
	//					}
	//				}

	//				//�˳��ж�
	//				if (bBreakLoop==true) continue;
	//			
	//				//�Ѿ�����
	//				if (m_pGameMatchOption->dwMatchID!=0 && pGameMatchOption->dwRelatedMatchID!=0)
	//				{
	//					//��������
	//					bBreakLoop=false;
	//					pGameMatchOption1=pGameMatchOption;

	//					do
	//					{							
	//						if (pGameMatchOption1->dwRelatedMatchID==m_pGameMatchOption->dwMatchID)
	//						{
	//							bBreakLoop=true;
	//							break;
	//						}								
	//					}while (pGameMatchOption1=m_pMatchInfoBuffer->SearchMatchOption(pGameMatchOption1->dwRelatedMatchID));	

	//					//�˳��ж�
	//					if (bBreakLoop==true) continue;
	//				}				

	//				//�������
	//				int nIndex=pComboBox->AddString(pGameMatchOption->szMatchName);
	//				pComboBox->SetItemData(nIndex,pGameMatchOption->dwMatchID);
	//				if (pGameMatchOption->dwMatchID==m_pGameMatchOption->dwRelatedMatchID)
	//				{
	//					pComboBox->SetCurSel(nIndex);
	//				}
	//			}
	//		}
	//	}
	//}	
}

//��������
bool CDlgMatchWizardStep1::SaveBaseOption()
{
	//������Ϣ	
	GetDlgItemText(IDC_EDIT_MATCH_NAME,m_pGameMatchOption->szMatchName,CountArray(m_pGameMatchOption->szMatchName));

	//��֤����
	if (m_pGameMatchOption->szMatchName[0]==0)
	{
		AfxMessageBox(TEXT("�������Ʋ���Ϊ�գ����������룡"));

		//���ý���
		GetDlgItem(IDC_EDIT_MATCH_NAME)->SetFocus();

		return false;
	}

	//��������
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_TYPE);
	m_pGameMatchOption->cbMatchType=(BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());	

	return true;
}

//��������
VOID CDlgMatchWizardStep1::ActiveOptionItem(BYTE cbItemIndex)
{
	//�ж�����
	if (m_cbItemIndex==cbItemIndex) return;

	//��������
	m_cbItemIndex=cbItemIndex;

	//�ؼ�����
	CRect rcTabCtrl;
	GetDlgItem(IDC_MATCH_OPTION)->GetClientRect(&rcTabCtrl);

	//���ÿؼ�
	if (m_pDlgMatchItem[m_cbItemIndex]->m_hWnd!=NULL)
	{
		m_pDlgMatchItem[m_cbItemIndex]->SetWindowPos(NULL,5,30,rcTabCtrl.Width()-10,rcTabCtrl.Height()-30-5,SWP_NOZORDER|SWP_SHOWWINDOW);
	}

	//���ÿؼ�
	for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
	{
		if (i==m_cbItemIndex) continue;
		if (m_pDlgMatchItem[i]->m_hWnd!=NULL) m_pDlgMatchItem[i]->ShowWindow(SW_HIDE);
	}
}

//ѡ����
void CDlgMatchWizardStep1::OnCbnSelchangeComboMatchType()
{
	//��ȡ����
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_COMBO_MATCH_TYPE);
	if (pComboBox!=NULL && pComboBox->GetCurSel()!=LB_ERR)
	{
		//��������
		BYTE cbMatchType=(BYTE)pComboBox->GetItemData(pComboBox->GetCurSel());

		//�¼�֪ͨ
		for (int i=0; i<CountArray(m_pDlgMatchItem); i++)
		{
			m_pDlgMatchItem[i]->OnEventMatchTypeChanged(cbMatchType);
		}
	}
}

void CDlgMatchWizardStep1::OnTcnSelchangeMatchOption(NMHDR *pNMHDR, LRESULT *pResult)
{
	//��ȡ�ؼ�
	CTabCtrl * pTabCtrl=(CTabCtrl *)GetDlgItem(IDC_MATCH_OPTION);
	ActiveOptionItem(pTabCtrl->GetCurSel());	
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchWizardStep2::CDlgMatchWizardStep2() : CDlgMatchWizardItem(IDD_MATCH_WIZARD_2)
{
	//���ñ���
	m_pDlgMatchItem=NULL;
}

//��������
CDlgMatchWizardStep2::~CDlgMatchWizardStep2()
{
}

//�ؼ���
VOID CDlgMatchWizardStep2::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//��ʼ������
BOOL CDlgMatchWizardStep2::OnInitDialog()
{
	__super::OnInitDialog();	
	
	return FALSE;
}

//��������
bool CDlgMatchWizardStep2::SaveInputInfo()
{
	//��������
	if (m_pDlgMatchItem->SaveInputInfo() == false) return false;

	return true;
}

//��������
VOID CDlgMatchWizardStep2::OnMatchItemActive()
{
	//���ش���
	if (m_pDlgMatchItem!=NULL) m_pDlgMatchItem->ShowWindow(SW_HIDE);

	//��ȡλ��
	CRect rcItemRect;
	GetDlgItem(IDC_MATCH_OPTION_FRAME)->GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//ѡ������
	if (m_pGameMatchOption->cbMatchType==MATCH_TYPE_LOCKTIME)
	{
		m_pDlgMatchItem=&m_DlgMatchLockTime;
	}
	else
	{
		m_pDlgMatchItem=&m_DlgMatchImmediate;
	}

	//���ñ���
	m_pDlgMatchItem->SetGameMatchOption(m_pGameMatchOption);

	//��ʾ����
	rcItemRect.DeflateRect(2,12,2,2);
	m_pDlgMatchItem->ShowMatchItem(rcItemRect,this);
}

//�����ؼ�
VOID CDlgMatchWizardStep2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgMatchWizard::CDlgMatchWizard() : CDialog(IDD_MATCH_WIZARD)
{
	//���ñ���
	m_wActiveIndex=INVALID_WORD;
	m_pWizardItem[0]=&m_MatchWizardStep1;
	m_pWizardItem[1]=&m_MatchWizardStep2;

	//������Ϣ
	m_RewardInfoBuffer.ResetMatchRewardInfo();
	ZeroMemory(&m_GameMatchOption,sizeof(m_GameMatchOption));
	m_GameMatchOption.cbSignupMode = SIGNUP_MODE_SIGNUP_FEE;
	m_GameMatchOption.cbJoinCondition = MATCH_JOINCD_MEMBER_ORDER;	
	m_GameMatchOption.cbDistributeRule =DISTRIBUTE_ALLOW|DISTRIBUTE_LAST_TABLE;

	return;
}

//��������
CDlgMatchWizard::~CDlgMatchWizard()
{
}

//��ʼ������
BOOL CDlgMatchWizard::OnInitDialog()
{
	__super::OnInitDialog();

	//��������
	CMatchInfoManager MatchInfoManager;

	//��ȡ������
	TCHAR szServiceMachine[LEN_MACHINE_ID];
	CWHService::GetMachineID(szServiceMachine);

	//��ȡ��Ϣ
	MatchInfoManager.LoadGameMatchOption(m_wKindID,szServiceMachine,m_MatchInfoBuffer);

	//��ȡ����
	if (m_GameMatchOption.dwMatchID!=0)
	{
		MatchInfoManager.LoadGameMatchReward(m_GameMatchOption.dwMatchID,m_RewardInfoBuffer);
	}	

	//������
	ActiveWizardItem(0);

	return FALSE;
}

//ȷ������
VOID CDlgMatchWizard::OnOK()
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


//��ȡ����
VOID CDlgMatchWizard::GetGameMatchOption(tagGameMatchOption & GameMatchOption)
{
	//���ñ���
	GameMatchOption=m_GameMatchOption;
}

//��������
VOID CDlgMatchWizard::SetGameMatchOption(tagGameMatchOption & GameMatchOption)
{
	//���ñ���
	m_GameMatchOption=GameMatchOption;
}

//������
bool CDlgMatchWizard::ActiveWizardItem(WORD wIndex)
{
	//�ж�״̬
	if (m_wActiveIndex==wIndex)
	{
		m_pWizardItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//�������
	CDlgMatchWizardItem * pItemWizard=NULL;
	if (m_wActiveIndex<CountArray(m_pWizardItem))
	{
		//���ñ���
		pItemWizard=m_pWizardItem[m_wActiveIndex];

		//��������
		if ((wIndex>m_wActiveIndex)&&(m_pWizardItem[m_wActiveIndex]->SaveItemData()==false)) return false;
	}

	//��ȡλ��
	CRect rcItemRect;
	GetDlgItem(IDC_MATCH_FRAME)->GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//��������
	m_pWizardItem[wIndex]->m_pDlgMatchWizard=this;
	m_pWizardItem[wIndex]->m_pGameMatchOption=&m_GameMatchOption;
	m_pWizardItem[wIndex]->m_pMatchInfoBuffer=&m_MatchInfoBuffer;
	m_pWizardItem[wIndex]->m_pRewardInfoBuffer=&m_RewardInfoBuffer;

	//��������
	m_wActiveIndex=wIndex;
	m_pWizardItem[m_wActiveIndex]->ShowWizardItem(rcItemRect,this);

	//��������
	m_pWizardItem[m_wActiveIndex]->SetFocus();
	if (pItemWizard!=NULL) pItemWizard->ShowWindow(SW_HIDE);

	//�������
	CButton * pButtonLast=(CButton * )GetDlgItem(IDC_LAST);
	CButton * pButtonNext=(CButton * )GetDlgItem(IDC_NEXT);
	CButton * pButtonFinish=(CButton * )GetDlgItem(IDC_FINISH);

	//���Ƚ���
	pButtonFinish->EnableWindow((m_wActiveIndex>0)?TRUE:FALSE);
	pButtonNext->EnableWindow(((m_wActiveIndex+1)<CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonLast->EnableWindow((m_wActiveIndex>0)?TRUE:FALSE);

	//�������
	TCHAR szTitle[128]=TEXT("");
	_sntprintf(szTitle,CountArray(szTitle),TEXT("���������� --- [ ���� %d ]"),m_wActiveIndex+1);
	
	//���ñ���
	SetWindowText(szTitle);

	return true;
}

//����ת��
VOID CDlgMatchWizard::TransferMatchReward(CString & strRewardGold,CString & strRewardIngot,CString & szRewardExperience)
{
	//���ò���
	strRewardGold.Empty();
	strRewardIngot.Empty();
	szRewardExperience.Empty();

	//��������
	CString strRewardItem;
	tagMatchRewardInfo * pMatchRewardInfo=NULL;

	//ת������
	for (WORD wRandID=1; wRandID<=m_RewardInfoBuffer.GetMatchRewardCount(); wRandID++)
	{
		//������Ϣ
		pMatchRewardInfo=m_RewardInfoBuffer.SearchMatchReward(wRandID);

		//��ҽ���
		strRewardItem.Format(TEXT("%.2f|"),pMatchRewardInfo->lRewardGold);
		strRewardGold.Append(strRewardItem);

		//Ԫ������
		strRewardItem.Format(TEXT("%.2f|"),pMatchRewardInfo->lRewardIngot);
		strRewardIngot.Append(strRewardItem);

		//���齱��
		strRewardItem.Format(TEXT("%d|"),pMatchRewardInfo->dwRewardExperience);
		szRewardExperience.Append(strRewardItem);
	}
}

//��һ��
VOID CDlgMatchWizard::OnBnClickedLast()
{
	//Ч�����
	ASSERT(m_wActiveIndex>0);
	ASSERT(m_wActiveIndex<CountArray(m_pWizardItem));

	//�л�ҳ��
	ActiveWizardItem(m_wActiveIndex-1);

	return;
}

//��һ��
VOID CDlgMatchWizard::OnBnClickedNext()
{
	//Ч�����
	ASSERT((m_wActiveIndex+1)<CountArray(m_pWizardItem));

	//����ҳ��
	ActiveWizardItem(m_wActiveIndex+1);

	return;
}

//��ɰ�ť
VOID CDlgMatchWizard::OnBnClickedFinish()
{
	//��������
	ASSERT(m_wActiveIndex>=1);
	if (m_pWizardItem[m_wActiveIndex]->SaveItemData()==false) return;

	//��������
	for (WORD i=0;i<CountArray(m_pWizardItem);i++)
	{
		if ((m_pWizardItem[i]!=NULL)&&(m_pWizardItem[i]->SaveItemData()==false))
		{
			ActiveWizardItem(i);
			return;
		}
	}

	//������Ϣ
	tagCreateMatchInfo CreateMatchInfo;
	ZeroMemory(&CreateMatchInfo,sizeof(CreateMatchInfo));

	//������Ϣ
	CreateMatchInfo.wKindID=m_wKindID;
	CreateMatchInfo.dwMatchID=m_GameMatchOption.dwMatchID;
	CreateMatchInfo.lMatchNo=m_GameMatchOption.lMatchNo;	
	CreateMatchInfo.cbMatchType=m_GameMatchOption.cbMatchType;	

	//������Ϣ
	CreateMatchInfo.cbFeeType=m_GameMatchOption.cbFeeType;	
	CreateMatchInfo.lSignupFee=m_GameMatchOption.lSignupFee;
	CreateMatchInfo.cbDeductArea=m_GameMatchOption.cbDeductArea;
	CreateMatchInfo.cbSignupMode=m_GameMatchOption.cbSignupMode;	
	CreateMatchInfo.cbJoinCondition=m_GameMatchOption.cbJoinCondition;	
	CreateMatchInfo.cbMemberOrder=m_GameMatchOption.cbMemberOrder;	
	CreateMatchInfo.dwExperience=m_GameMatchOption.dwExperience;	
	CreateMatchInfo.dwFromMatchID=m_GameMatchOption.dwFromMatchID;	
	CreateMatchInfo.cbFilterType=m_GameMatchOption.cbFilterType;
	CreateMatchInfo.wMaxRankID=m_GameMatchOption.wMaxRankID;	
	CreateMatchInfo.MatchEndDate=m_GameMatchOption.MatchEndDate;
	CreateMatchInfo.MatchStartDate=m_GameMatchOption.MatchStartDate;

	//������ʽ
	CreateMatchInfo.cbRankingMode=m_GameMatchOption.cbRankingMode;
	CreateMatchInfo.wCountInnings=m_GameMatchOption.wCountInnings;
	CreateMatchInfo.cbFilterGradesMode=m_GameMatchOption.cbFilterGradesMode;

	//������Ϣ
	lstrcpyn(CreateMatchInfo.szMatchName,m_GameMatchOption.szMatchName,CountArray(CreateMatchInfo.szMatchName));

	//��������
	CreateMatchInfo.cbDistributeRule=m_GameMatchOption.cbDistributeRule;
	CreateMatchInfo.wMinDistributeUser=m_GameMatchOption.wMinDistributeUser;
	CreateMatchInfo.wDistributeTimeSpace=m_GameMatchOption.wDistributeTimeSpace;
	CreateMatchInfo.wMinPartakeGameUser=m_GameMatchOption.wMinPartakeGameUser;
	CreateMatchInfo.wMaxPartakeGameUser=m_GameMatchOption.wMaxPartakeGameUser;

	//ת������
	CString strRewardGold,strRewardIngot,strRewardExperience;
	TransferMatchReward(strRewardGold,strRewardIngot,strRewardExperience);

	//��������
	lstrcpyn(CreateMatchInfo.szRewardGold,strRewardGold,CountArray(CreateMatchInfo.szRewardGold));
	lstrcpyn(CreateMatchInfo.szRewardIngot,strRewardIngot,CountArray(CreateMatchInfo.szRewardIngot));
	lstrcpyn(CreateMatchInfo.szRewardExperience,strRewardExperience,CountArray(CreateMatchInfo.szRewardExperience));

	//��չ����
	CWHService::GetMachineID(CreateMatchInfo.szServiceMachine);
	CopyMemory(&CreateMatchInfo.cbMatchRule,&m_GameMatchOption.cbMatchRule,sizeof(CreateMatchInfo.cbMatchRule));

	//����ʱ��
	if (CreateMatchInfo.cbMatchType==MATCH_TYPE_IMMEDIATE)
	{
		lstrcpyn(CreateMatchInfo.szMatchDate,TEXT("ȫ��24Сʱѭ������"),CountArray(CreateMatchInfo.szMatchDate));
	}

	//����ʱ��
	if (CreateMatchInfo.cbMatchType==MATCH_TYPE_LOCKTIME)
	{
		tagLockTimeMatch * pLockTimeMatch=(tagLockTimeMatch *)m_GameMatchOption.cbMatchRule;
		_sntprintf(CreateMatchInfo.szMatchDate,CountArray(CreateMatchInfo.szMatchDate),TEXT("%4d-%02d-%02d -- %04d-%02d-%02d"),pLockTimeMatch->MatchStartTime.wYear,pLockTimeMatch->MatchStartTime.wMonth,pLockTimeMatch->MatchStartTime.wDay,pLockTimeMatch->MatchEndTime.wYear,pLockTimeMatch->MatchEndTime.wMonth,pLockTimeMatch->MatchEndTime.wDay);
	}

	//��������
	tagGameMatchOption GameMatchResult;
	CMatchInfoManager MatchInfoManager;
	
	//�������
	if (CreateMatchInfo.dwMatchID==0)
	{
		if (MatchInfoManager.InsertGameMatchOption(&CreateMatchInfo,GameMatchResult)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ���������ɹ�"),TraceLevel_Normal);
	}

	//�޸ı���
	if (CreateMatchInfo.dwMatchID!=0)
	{
		//�޸ķ���
		if (MatchInfoManager.ModifyGameMatchOption(&CreateMatchInfo,GameMatchResult)==false) return;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ�������óɹ�"),TraceLevel_Normal);
	}

	//���ñ�ʶ
	m_GameMatchOption.dwMatchID=GameMatchResult.dwMatchID;

	//��������
	m_GameMatchOption.wRewardCount=GameMatchResult.wRewardCount;
	CopyMemory(m_GameMatchOption.MatchRewardInfo,GameMatchResult.MatchRewardInfo,sizeof(m_GameMatchOption.MatchRewardInfo));

	//�رմ���
	EndDialog(IDOK);

	return;
}

//////////////////////////////////////////////////////////////////////////
