#ifndef DLG_MATCH_WIZARD_HEAD_FILE
#define DLG_MATCH_WIZARD_HEAD_FILE

#pragma once

//�����ļ�
#include "MatchInfoManager.h"
#include "MatchListControl.h"

//////////////////////////////////////////////////////////////////////////

//��˵��
class CDlgMatchItem;
class CDlgMatchWizard;
class CDlgMatchWizardStep1;
class CDlgMatchWizardStep2;

//////////////////////////////////////////////////////////////////////////

//����������
class MODULE_MANAGER_CLASS CDlgMatchItem : public CDialog
{
	friend class CDlgMatchWizardStep1;
	friend class CDlgMatchWizardStep2;

	//��������
protected:			
	CMatchInfoBuffer *				m_pMatchInfoBuffer;					//ģ����Ϣ
	CRewardInfoBuffer *				m_pRewardInfoBuffer;				//��������	
	tagGameMatchOption *			m_pGameMatchOption;					//��������

	//��������
protected:
	//���캯��
	CDlgMatchItem(UINT nIDTemplate);
	//��������
	virtual ~CDlgMatchItem();

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK() { return; }
	//ȡ����Ϣ
	virtual VOID OnCancel() { return; }

	//�ӿں���
protected:	
	//��������
	virtual bool SaveInputInfo()=NULL;	
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//���ú���
protected:	
	//���ñ���
	VOID SetMatchInfoBuffer(CMatchInfoBuffer * pMatchInfoBuffer);
	//���ý���
	VOID SetRewardInfoBuffer(CRewardInfoBuffer * pRewardInfoBuffer);
	//��������
	VOID SetGameMatchOption(tagGameMatchOption * pGameMatchOption);
	//��ȡ����
	tagGameMatchOption * GetGameMatchOption() { return m_pGameMatchOption; }

	//���ܺ���
private:
	//��ʾ����
	bool ShowMatchItem(const CRect & rcRect, CWnd * pParentWnd); 
	//��������
	VOID CreateMatchItem(const CRect & rcRect, CWnd * pParentWnd);

	//�¼�����
public:
	//���ͱ��
	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);

	//��Ϣ����
private:
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CDlgMatchSignup1 : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//��������
public:
	//���캯��
	CDlgMatchSignup1();
	//��������
	virtual ~CDlgMatchSignup1();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//�¼�����
public:
	//���ͱ��
	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);

	//��Ϣ����
protected:
	//ѡ����
	void OnCbnSelchangeComboFromMatch();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CDlgMatchSignup2 : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//��������
public:
	//���캯��
	CDlgMatchSignup2();
	//��������
	virtual ~CDlgMatchSignup2();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//�������
class MODULE_MANAGER_CLASS CDlgMatchDistribute : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//��������
public:
	//���캯��
	CDlgMatchDistribute();
	//��������
	virtual ~CDlgMatchDistribute();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//������ʽ
class MODULE_MANAGER_CLASS CDlgMatchRankingMode : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//��������
public:
	//���캯��
	CDlgMatchRankingMode();
	//��������
	virtual ~CDlgMatchRankingMode();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//�¼�����
public:
	//���ͱ��
	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);
};


//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CDlgMatchReward : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//�ؼ�����
protected:
	CRewardListControl			m_RewardListControl;			//�б�ؼ�

	//��������
public:
	//���캯��
	CDlgMatchReward();
	//��������
	virtual ~CDlgMatchReward();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//��������
protected:
	//��������
	VOID SetMatchRewardOption(tagMatchRewardInfo * pMatchRewardInfo);
	//��ȡ����
	bool GetMatchRewardOption(tagMatchRewardInfo & MatchRewardInfo,WORD & wStartRankID,WORD & wEndRankID);	

	//��Ϣ����
public:
	//��ӽ���
	VOID OnBnClickedAddReward();
	//�޸Ľ���
	VOID OnBnClickedModifyReward();
	//ɾ������
	VOID OnBnClickedDeleteReward();

	//�ؼ���Ϣ
protected:
	//������
	VOID OnLvnItemchangedListReward(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

//��ʱ����
class MODULE_MANAGER_CLASS CDlgMatchLockTime : public CDlgMatchItem
{
	//��������
public:
	//���캯��
	CDlgMatchLockTime();
	//��������
	virtual ~CDlgMatchLockTime();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//��ʱ����
class MODULE_MANAGER_CLASS CDlgMatchImmediate : public CDlgMatchItem
{
	//��������
public:
	//���캯��
	CDlgMatchImmediate();
	//��������
	virtual ~CDlgMatchImmediate();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//�򵼻�����
class MODULE_MANAGER_CLASS CDlgMatchWizardItem : public CDialog
{
	//��Ԫ����
	friend class CDlgMatchWizard;

	//״̬����
private:
	bool							m_bSaveData;						//�����־

	//��������
protected:
	CDlgMatchWizard *				m_pDlgMatchWizard;					//��ָ��
	tagGameMatchOption *			m_pGameMatchOption;					//��������
	CMatchInfoBuffer *				m_pMatchInfoBuffer;					//ģ����Ϣ
	CRewardInfoBuffer *				m_pRewardInfoBuffer;				//��������	

	//��������
protected:
	//���캯��
	CDlgMatchWizardItem(UINT nIDTemplate);
	//��������
	virtual ~CDlgMatchWizardItem();

	//���غ���
protected:
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();

	//�ӿں���
protected:
	//��������
	virtual bool SaveInputInfo()=NULL;
	//��������
	virtual VOID OnMatchItemActive()=NULL;
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//���ܺ���
private:
	//��������
	bool SaveItemData();
	//������
	bool ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd);

	//��Ϣ����
private:
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//������
class MODULE_MANAGER_CLASS CDlgMatchWizardStep1 : public CDlgMatchWizardItem
{
	//��������
protected:
	BYTE							m_cbItemIndex;					//��������

	//�ؼ�����
protected:	
	CDlgMatchItem *					m_pDlgMatchItem[5];				//��������
	CDlgMatchReward					m_DlgMatchReward;				//��������
	CDlgMatchSignup1				m_DlgMatchSignup1;				//��������
	CDlgMatchSignup2				m_DlgMatchSignup2;				//��������
	CDlgMatchDistribute				m_DlgMatchDistribute;			//��������	
	CDlgMatchRankingMode			m_DlgMatchRankingMode;			//��������

	//��������
public:
	//���캯��
	CDlgMatchWizardStep1();
	//��������
	virtual ~CDlgMatchWizardStep1();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//��������
	virtual VOID OnMatchItemActive();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//��������
protected:
	//��������
	VOID InitBaseOption();
	//��������
	bool SaveBaseOption();
	//��������
	VOID ActiveOptionItem(BYTE cbItemIndex);

	//��Ϣ����
protected:
	//ѡ����
	void OnCbnSelchangeComboMatchType();	
	//ѡ����
	void OnTcnSelchangeMatchOption(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

//������
class MODULE_MANAGER_CLASS CDlgMatchWizardStep2 : public CDlgMatchWizardItem
{
	//��������
protected:
	CDlgMatchItem *				m_pDlgMatchItem;				//��������
	CDlgMatchLockTime			m_DlgMatchLockTime;				//��ʱ��
	CDlgMatchImmediate			m_DlgMatchImmediate;			//��ʱ��

	//��������
public:
	//���캯��
	CDlgMatchWizardStep2();
	//��������
	virtual ~CDlgMatchWizardStep2();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();

	//���غ���
public:
	//��������
	virtual bool SaveInputInfo();
	//�����¼�
	virtual VOID OnMatchItemActive();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);	
};

//////////////////////////////////////////////////////////////////////////

//������
class MODULE_MANAGER_CLASS CDlgMatchWizard : public CDialog
{
	//��Ԫ����
	friend class CDlgMatchWizardStep1;
	friend class CDlgMatchWizardStep2;

	//��������
protected:
	WORD							m_wActiveIndex;						//�����
	CDlgMatchWizardItem *			m_pWizardItem[2];					//��ָ��

	//���ñ���
protected:	
	WORD							m_wKindID;							//���ͱ�ʶ
	CMatchInfoBuffer				m_MatchInfoBuffer;					//ģ����Ϣ
	CRewardInfoBuffer				m_RewardInfoBuffer;					//��������	
	tagGameMatchOption				m_GameMatchOption;					//��������		
	
	//�ؼ�����
protected:
	CDlgMatchWizardStep1			m_MatchWizardStep1;					//���ò���
	CDlgMatchWizardStep2			m_MatchWizardStep2;					//���ò���

	//��������
public:
	//���캯��
	CDlgMatchWizard();
	//��������
	virtual ~CDlgMatchWizard();

	//���غ���
public:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();

	//���ú���
public:
	//��������
	VOID SetGameKindID(WORD wKindID) { m_wKindID=wKindID; }
	//��ȡ����
	VOID GetGameMatchOption(tagGameMatchOption & GameMatchOption);	
	//��������
	VOID SetGameMatchOption(tagGameMatchOption & GameMatchOption);
	//����

	//�ڲ�����
private:	
	//������
	bool ActiveWizardItem(WORD wIndex);	
	//����ת��
	VOID TransferMatchReward(CString & strRewardGold,CString & strRewardMedal,CString & szRewardExperience);

	//��Ϣӳ��
public:
	//��һ��
	VOID OnBnClickedLast();
	//��һ��
	VOID OnBnClickedNext();
	//��ɰ�ť
	VOID OnBnClickedFinish();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif