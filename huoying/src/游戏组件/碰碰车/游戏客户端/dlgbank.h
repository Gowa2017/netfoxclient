#pragma once

void MakeString(CString &strNum,LONGLONG lNumber);
void MakeStringToNum(CString str,LONGLONG &Num);
// CDlgBank dialog
#define  WM_CREADED           WM_USER+1
class CDlgBank : public CSkinDialog
{
  //��������
public:
  bool              m_bBankStorage;         //�洢��ʶ
  IClientUserItem         *m_pMeUserData;         //�û���Ϣ
  LONGLONG            m_lGameGold;          //������Ŀ
  LONGLONG            m_lStorageGold;         //�洢��Ŀ
  LONGLONG            m_lInCount;           //������Ϸ��
  LONGLONG            m_OrInCount;                    //
  bool              m_blCanStore;                   //�ܴ�
  bool                            m_blCanGetOnlyFree;             //��������ȡ

  //�ؼ�����
protected:
  CSkinButton           m_btOK;             //ȷ����ť
  CSkinButton           m_btCancel;           //ȡ����ť
  CSkinButton           m_btFresh;            //ˢ�°�ť
  CEdit             m_Edit;
  CStatic             m_Static;
  bool              m_blUsingPassWord;
  CPngImage           m_ImageFrame;         //
  CPngImage           m_ImageNumber;

  //�ӿڱ���
protected:
  IClientKernel         * m_pIClientKernel;       //�ں˽ӿ�


public:
  CDlgBank(CWnd* pParent = NULL);   // standard constructor
  virtual ~CDlgBank();

  //��������
public:
  //���½���
  void UpdateView();

  //���غ���
protected:
  //�ؼ���
  virtual void DoDataExchange(CDataExchange* pDX);
  //��ʼ������
  virtual BOOL OnInitDialog();
  //ȷ����ť
  afx_msg void OnBnClickedOk();
  //�����Ϣ
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  //�����Ϣ
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  //������Ϣ
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //ʱ����Ϣ
  afx_msg void OnTimer(UINT nIDEvent);
  //������Ϣ
  afx_msg void OnEnChangeInCount();

  afx_msg void OnEnRadioStore();

  afx_msg void OnEnRadioGet();
  //��ѡ
  afx_msg void OnBnClickedCheck();
  virtual void OnCancel();
  //�滭��Ϣ
  virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);



  int OnCreate(LPCREATESTRUCT lpCreateStruct);

  afx_msg LRESULT  OnCreateEd(WPARAM wparam,LPARAM lparam);

  //�ӿں���
public:
  //��������
  void SetBankerActionType(bool bStorage);

  void SetPostPoint(CPoint Point);

  void ShowItem();

  void HideStorage();

  //���ܺ���
public:
  //�Ƿ�ѡ��
  bool IsButtonSelected(UINT uButtonID);
  //ѡ�а�ť
  void SetButtonSelected(UINT uButtonID, bool bSelected);
  //�������
  void SetSendInfo(IClientKernel *pClientKernel,IClientUserItem*pMeUserDatas);
  //������Ϣ
  void SendSocketData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize);
  void OnCancelEvent();
  //�滭����
  VOID DrawNumberString(CDC * pDC, SCORE lScore, INT nXPos, INT nYPos);

  DECLARE_MESSAGE_MAP()
  DECLARE_DYNAMIC(CDlgBank)
};
