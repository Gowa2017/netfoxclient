#pragma once
#include "../��Ϸ�ͻ���/ClientControl.h"
#include "SkinListCtrl.h"
#include "afxcmn.h"

// CClientControlItemSinkDlg �Ի���
#ifndef _UNICODE
#define myprintf  _snprintf
#define mystrcpy  strcpy
#define mystrlen  strlen
#define myscanf   _snscanf
#define myLPSTR   LPCSTR
#define myatoi      atoi
#define myatoi64    _atoi64
#else
#define myprintf  swprintf
#define mystrcpy  wcscpy
#define mystrlen  wcslen
#define myscanf   _snwscanf
#define myLPSTR   LPWSTR
#define myatoi      _wtoi
#define myatoi64  _wtoi64
#endif
class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
  DECLARE_DYNAMIC(CClientControlItemSinkDlg)

public:
  CClientControlItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
  virtual ~CClientControlItemSinkDlg();
  LONGLONG            m_lStorageStart;            //�����ֵ
  LONGLONG            m_lStorageDeduct;           //���˥��

// �Ի�������
  enum { IDD = IDD_CLIENT_CONTROL_EX };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

  DECLARE_MESSAGE_MAP()

public:
  //���¿���
  virtual void  UpdateControl(CMD_S_ControlReturns* pControlReturns);

protected:
  //��Ϣ����
  void PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes);
  //���ý���
  void ReSetAdminWnd();

public:
  //��ʼ��
  virtual BOOL OnInitDialog();
  //������ɫ
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  //ȡ������
  afx_msg void OnBnClickedButtonReset();
  //���ֿ���
  afx_msg void OnBnClickedButtonSyn();
  //��������
  afx_msg void OnBnClickedButtonOk();
  //������ע
  virtual void __cdecl UpdateUserBet(bool bReSet);
  //ȡ���ر�
  afx_msg void OnBnClickedButtonCancel();
  afx_msg void  OnFreshStorage();
  afx_msg void  OnFreshDeuct();
  //���¿��
  virtual bool __cdecl UpdateStorage(const void * pBuffer);
  //CSkinListCtrlEx m_SkinListCtrl;
  CSkinListCtrlEx           m_listUserBet;          //�б�ؼ�
  CListCtrl           m_listUserBetAll;       //�б�ؼ�
  LONGLONG            m_lQueryGameID;         //��ѯID
  void SetStorageCurrentValue(LONGLONG lvalue);

  void ClearClist();
  virtual     void ClearText();
  void UpdateClistCtrl(LONGLONG m_lAllPlayBet[GAME_PLAYER][AREA_COUNT],BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair,IClientUserItem* pUserData);
private:
  LONGLONG                        m_areAtotalBet[AREA_COUNT];
  IClientUserItem* m_pUserData;
public:


  LONGLONG m_lcurrentStorage;
  LONGLONG m_lStorageMax1;
  LONGLONG m_lStorageMax2;
  LONGLONG m_lStorageMul1;
  LONGLONG m_lStorageMul2;
  afx_msg void OnStnClickedStaticInfo();
  afx_msg void OnBnClickedAreaFreshsto();
  afx_msg void OnBnClickedQuery();
  afx_msg void OnBnClickedBtReset();
  afx_msg void OnBnClickedBtCurset();

  afx_msg void OnEnSetfocusEditUserId();
  afx_msg void OnBnClickedTotal();

};
