#include "Stdafx.h"
#include <Afxdllx.h>
#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
static AFX_EXTENSION_MODULE	KernelEngineDLL={NULL,NULL};

//全局函数
extern "C" INT APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(KernelEngineDLL,hInstance)) return 0;
		new CDynLinkLibrary(KernelEngineDLL);

		//加载 COM
		CoInitialize(NULL);

		//加载网络
		WSADATA WSAData;
		if (WSAStartup(MAKEWORD(2,2),&WSAData)!=0) return FALSE;
	}
	else if (dwReason==DLL_THREAD_ATTACH)
	{
		//加载 COM
		CoInitialize(NULL);
	}
	else if (dwReason==DLL_THREAD_DETACH)
	{
		//释放 COM
		CoUninitialize();
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		//释放网络
		WSACleanup();

		//释放 COM
		CoUninitialize();

		AfxTermExtensionModule(KernelEngineDLL);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
