#include "StdAfx.h"
#include "AndroidServiceManager.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidService::CAndroidService()
{
}

//��������
CAndroidService::~CAndroidService()
{
}

//�ӿڲ�ѯ
VOID *  CAndroidService::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(IAndroidService,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(IAndroidService,Guid,dwQueryVer);
  return NULL;
}

//����������
VOID*  CAndroidService::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
  //��������
  CAndroidUserItemSink * pAndroidUserItemSink=NULL;

  try
  {
    //��������
    pAndroidUserItemSink=new CAndroidUserItemSink();
    if(pAndroidUserItemSink==NULL)
    {
      throw TEXT("����ʧ��");
    }

    //��ѯ�ӿ�
    VOID * pObject=pAndroidUserItemSink->QueryInterface(Guid,dwQueryVer);
    if(pObject==NULL)
    {
      throw TEXT("�ӿڲ�ѯʧ��");
    }

    return pObject;
  }
  catch(...) {}

  //ɾ������
  SafeDelete(pAndroidUserItemSink);

  return NULL;
}

//////////////////////////////////////////////////////////////////////////

//��������
DECLARE_CREATE_MODULE(AndroidService)

//////////////////////////////////////////////////////////////////////////
