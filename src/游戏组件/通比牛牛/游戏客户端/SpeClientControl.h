#pragma once

#define IDM_ADMIN_COMMDN WM_USER+1000
#define IDM_SPE_COMMDN WM_USER+1001

//��Ϸ���ƻ���
class ISpeClientControlDlg : public CDialog
{
public:
  ISpeClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent) {}
  virtual ~ISpeClientControlDlg(void) {}

  //���¿���
  virtual void __cdecl OnAllowControl(bool bEnable) = NULL;
  //������Ϣ
  virtual bool __cdecl ReqResult(const void * pBuffer)=NULL;
};
