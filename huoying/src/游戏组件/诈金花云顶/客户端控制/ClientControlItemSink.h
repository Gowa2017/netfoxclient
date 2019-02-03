#pragma once
#include "../��Ϸ�ͻ���/ClientControl.h"

typedef enum
{
	QUERY_USER_GAMEID,
	QUERY_USER_NICKNAME,
	QUERY_INVALID
}QUERY_TYPE;

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	SCORE				m_lMaxRoomStorage[2];						//�������
	WORD					m_wRoomStorageMul[2];						//Ӯ�ָ���
	SCORE				m_lRoomStorageCurrent;						//�����ֵ
	SCORE				m_lRoomStorageDeduct;						//���˥��

    DWORD					m_dwQueryUserGameID;						//��ѯGAMEID
	TCHAR					m_szQueryUserNickName[LEN_NICKNAME];	//��ѯ�û��ǳ�
	QUERY_TYPE				m_QueryType;							//��ѯ�û�������

public:
	//�ؼ�����
	CSkinEdit				m_editCurrentStorage;
	CSkinEdit				m_editStorageDeduct;
	CSkinEdit				m_editStorageMax1;
	CSkinEdit				m_editStorageMul1;
	CSkinEdit				m_editStorageMax2;
	CSkinEdit				m_editStorageMul2;
	CSkinEdit				m_editUserID;
	CSkinEdit				m_editControlCount;
	CSkinButton				m_btUpdateStorage;
	CSkinButton				m_btModifyStorage;
	CSkinButton				m_btContinueWin;
	CSkinButton				m_btContinueLost;
	CSkinButton				m_btContinueCancel;
	CSkinButton				m_btQueryUser;
	CSkinRichEdit			m_richEditUserDescription;
	CSkinRichEdit			m_richEditUserControl;
	CSkinRichEdit			m_richEditOperationRecord;

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClientControlItemSinkDlg();

// �Ի�������
	enum { IDD = IDD_CLIENT_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	//���÷�����
	virtual void SetRoomStorage(LONGLONG lRoomStartStorage, LONGLONG lRoomCurrentStorage);
	virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInfo);
	//��ѯ�û����
	virtual void RequestQueryResult(CMD_S_RequestQueryResult *pQueryResult);
	//�����û����ƽ��
	virtual void RoomUserControlResult(CMD_S_UserControl *pUserControl);
	//�û��������
	virtual void UserControlComplete(CMD_S_UserControlComplete *pUserControlComplete);
	//������¼
	virtual void UpdateOperationRecord(CMD_S_Operation_Record *pOperation_Record);
	//���ø��¶�ʱ��
	virtual void SetUpdateIDI(); 
	//���·�����Ϣ
	virtual void UpdateRoomInfoResult(CMD_S_RequestUpdateRoomInfo_Result *RoomInfo_Result);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void  OnCancel();

	//�޸Ŀ������
	afx_msg void OnModifyStorage();
	//���¿��
	afx_msg void OnUpdateStorage();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnSetfocusEditUserId();
	afx_msg void OnContinueControlWin();
	afx_msg void OnContinueControlLost();
	afx_msg void OnContinueControlCancel();
	afx_msg void OnBnClickedBtnUserBetQuery();
	afx_msg void OnQueryUserGameID();
	afx_msg void OnQueryUserNickName();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

//���ܺ���
public:
	//��ȡ�û�״̬
	void GetUserStatusString(CMD_S_RequestQueryResult *pQueryResult, CString &userStatus);
	//��ȡ��Ϸ״̬
	void GetGameStatusString(CMD_S_RequestQueryResult *pQueryResult, CString &gameStatus);
	//��ȡ�Ƿ��������
	void GetSatisfyControlString(CMD_S_RequestQueryResult *pQueryResult, CString &satisfyControl, bool &bEnableControl);
	//��ȡ��������
	void GetControlTypeString(CONTROL_TYPE &controlType, CString &controlTypestr);
};
