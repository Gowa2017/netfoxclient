#include "Stdafx.h"
#include <Afxdllx.h>
#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
static AFX_EXTENSION_MODULE	KernelEngineDLL={NULL,NULL};

//ȫ�ֺ���
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(KernelEngineDLL,hInstance)) return 0;
		new CDynLinkLibrary(KernelEngineDLL);

		//���� COM
		CoInitialize(NULL);

		//��������
		WSADATA WSAData;
		if (WSAStartup(MAKEWORD(2,2),&WSAData)!=0) return FALSE;
	}
	else if (dwReason==DLL_THREAD_ATTACH)
	{
		//���� COM
		CoInitialize(NULL);
	}
	else if (dwReason==DLL_THREAD_DETACH)
	{
		//�ͷ� COM
		CoUninitialize();
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		//�ͷ�����
		WSACleanup();

		//�ͷ� COM
		CoUninitialize();

		AfxTermExtensionModule(KernelEngineDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
