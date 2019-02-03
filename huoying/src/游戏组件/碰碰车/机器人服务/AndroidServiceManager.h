#ifndef ANDROID_SERVICE_HEAD_FILE
#define ANDROID_SERVICE_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//��Ϸ������������
class CAndroidService : public IAndroidService
{
  //��������
public:
  //���캯��
  CAndroidService(VOID);
  //��������
  virtual ~CAndroidService(VOID);

  //�����ӿ�
public:
  //�ͷŶ���
  virtual VOID  Release()
  {
    delete this;
  }
  //�ӿڲ�ѯ
  virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

  //���ܽӿ�
public:
  //����������
  virtual VOID*  CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer);
};

//////////////////////////////////////////////////////////////////////////

#endif
