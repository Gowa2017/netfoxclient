#pragma once
#include "StringMessage.h"

//��Ļ����
#define LESS_SCREEN_CY        740                 //��С�߶�
#define LESS_SCREEN_CX        1024                //��С���

class CGameFrameWindow : public CFrameWnd, public IGameFrameWnd, public IUserEventSink
{
  //����ӿ�
protected:
  IClientKernel *         m_pIClientKernel;         //�ں˽ӿ�
  IGameFrameView *        m_pIGameFrameView;          //��ͼ�ӿ�
  IGameFrameService *       m_pIGameFrameService;       //��ܷ���


  //���ܿؼ�
public:
  CGameFrameControl       m_GameFrameControl;         //���ƿ��
  CStringMessage          m_StringMessage;          //��Ϣ����
private:
  CSize             m_CurWindowSize;

public:
  CGameFrameWindow(void);
  ~CGameFrameWindow(void);

  //�����ӿ�
public:
  //�ͷŶ���
  virtual VOID Release()
  {
    delete this;
  }
  //�ӿڲ�ѯ
  virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

  //���غ���
protected:
  //��Ϣ����
  virtual BOOL PreTranslateMessage(MSG * pMsg);
  //�����
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

  //�������
protected:
  //�����ؼ�
  VOID RectifyControl(INT nWidth, INT nHeight);
  //�����С
  void ReSetDlgSize(int cx,int cy);

  //���ڿ���
public:
  //��Ϸ����
  virtual bool ShowGameRule();
  //��󴰿�
  virtual bool MaxSizeWindow();
  //��ԭ����
  virtual bool RestoreWindow();

  //���ƽӿ�
public:
  //��������
  virtual bool AllowGameSound(bool bAllowSound);
  //�Թۿ���
  virtual bool AllowGameLookon(DWORD dwUserID, bool bAllowLookon);
  //���ƽӿ�
  virtual bool OnGameOptionChange();

  //�û��¼�
public:
  //�û�����
  virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser);
  //�û��뿪
  virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser);
  //�û�����
  virtual VOID OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser);
  //�û�״̬
  virtual VOID OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser);
  //�û�ͷ��
  virtual VOID OnEventCustomFace(IClientUserItem * pIClientUserItem, bool bLookonUser);
  //�û�����
  virtual VOID OnEventUserAttrib(IClientUserItem * pIClientUserItem, bool bLookonUser);


  //��Ϣ����
protected:
  //�滭����
  BOOL OnEraseBkgnd(CDC * pDC);
  //λ����Ϣ
  VOID OnSize(UINT nType, INT cx, INT cy);
  //������Ϣ
  INT OnCreate(LPCREATESTRUCT lpCreateStruct);
  //�����Ϣ
  VOID OnLButtonDown(UINT nFlags, CPoint Point);
  //�����Ϣ
  VOID OnLButtonDblClk(UINT nFlags, CPoint Point);
  //�ı���Ϣ
  VOID OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

  afx_msg LRESULT OnNcHitTest(CPoint point);

  //�Զ���Ϣ
protected:
  //������Ϣ
  LRESULT OnSetTextMesage(WPARAM wParam, LPARAM lParam);

  DECLARE_MESSAGE_MAP()
};
