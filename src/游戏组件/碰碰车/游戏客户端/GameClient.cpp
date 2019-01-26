#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientView.h"
#include "GameClientEngine.h"
#include "GameFrameWindow.h"

//////////////////////////////////////////////////////////////////////////////////

//�������
CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientApp::CGameClientApp()
{
}

//��������
CGameClientApp::~CGameClientApp()
{
}


////���ú���
//BOOL CGameClientApp::InitInstance()
//{
//  CWinApp::InitInstance();
//
//  ////�ڲ���ʼ��
//  //AfxOleInit();
//  //AfxInitRichEdit2();
//  //InitCommonControls();
//  //AfxEnableControlContainer();
//
//  //try
//  //{
//  //  //��������
//  //  ASSERT(CGlobalUnits::GetInstance()!=NULL);
//  //  CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
//
//  //  //ȫ�ֵ�Ԫ
//  //  bool bSuccess=pGlobalUnits->InitGlobalUnits();
//  //  if (bSuccess==false) throw TEXT("��Ϸ������ʼ��ʧ��");
//
//  //  //��������
//  //  m_pGameFrameWnd=GetGameFrameWnd();
//  //  if (m_pGameFrameWnd==NULL) throw TEXT("��Ϸ��ܴ��ڴ���ʧ��");
//
//
//  //  //��������
//  //  m_pGameFrameEngine=GetGameFrameEngine(VERSION_FRAME_SDK);
//  //  if (m_pGameFrameEngine==NULL) throw TEXT("��Ϸ������󴴽�ʧ��");
//
//
//  //  //�������
//  //  m_pMainWnd=m_pGameFrameWnd;
//  //  m_pGameFrameWnd->Create(NULL,NULL,WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,CRect(0,0,0,0));
//
//
//  //  //��������
//  //  m_D3DSound.CreateD3DSound(m_pMainWnd->GetSafeHwnd());
//
//  //  //��������
//  //  if (m_GameFrameService.CreateGameFrameService()==false) throw TEXT("�ں����洴��ʧ��");
//
//  //  //���ӹ㳡
//  //  IClientKernel * pIClientKernel=pGlobalUnits->m_ClientKernelModule.GetInterface();
//  //  if ((pIClientKernel!=NULL)&&(pIClientKernel->IsSingleMode()==false)) pIClientKernel->CreateConnect();
//
//  //  return TRUE;
//  //}
//  //catch (LPCTSTR pszMesssage)
//  //{
//  //  //������Ϣ
//  //  CString strBuffer;
//  //  strBuffer.Format(TEXT("���� [ %s ] ��Ϸ��������ʧ�ܡ�"),pszMesssage);
//
//  //  //��ʾ��Ϣ
//  //  AfxMessageBox(pszMesssage);
//
//  //  //ɾ������
//  //  SafeRelease(m_pGameFrameWnd);
//  //  SafeRelease(m_pGameFrameEngine);
//
//  //  return FALSE;
//  //}
//
//  return TRUE;
//}


////�˳�����
//BOOL CGameClientApp::ExitInstance()
//{
//  ////ɾ������
//  //if ( m_pGameFrameEngine != NULL )
//  //{
//  //  m_pGameFrameEngine->Release();
//  //  m_pGameFrameEngine = NULL;
//  //}
//
//  ////��������
//  //ASSERT(CGlobalUnits::GetInstance()!=NULL);
//  //CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
//
//  ////�ر�����
//  //IClientKernel * pIClientKernel=pGlobalUnits->m_ClientKernelModule.GetInterface();
//  //if ((pIClientKernel!=NULL)&&(pIClientKernel->IsSingleMode()==false)) pIClientKernel->IntermitConnect();
//
//  ////ע�����
//  //pGlobalUnits->UnInitGlobalUnits();
//
//  return CWinApp::ExitInstance();
//}


//��������
CGameFrameEngine * CGameClientApp::GetGameFrameEngine(DWORD dwSDKVersion)
{
  //�汾���
  if(InterfaceVersionCompare(VERSION_FRAME_SDK,dwSDKVersion)==false)
  {
    ASSERT(FALSE);
    return NULL;
  }

  //��������
  return new CGameClientEngine;
}

////�������
//CGameFrameWindow * CGameClientApp::GetGameFrameWindow()
//{
//  return new CGameFrameWindow;
//}

//CGameFrameWnd * CGameClientApp::GetFrameWnd()
//{
//  return m_pGameFrameWnd;
//}

//�������
CGameFrameWnd * CGameClientApp::GetGameFrameWnd()
{
  return (CGameFrameWnd *)new CGameFrameWindow();
  //return new CGameFrameWnd;
}

//////////////////////////////////////////////////////////////////////////////////
