#pragma once
#include "Stdafx.h"
#define IDC_CTROL_QUERY           2023+2
class CCtrolListCtrl:public CListCtrl
{
public:
  CCtrolListCtrl(void);
  ~CCtrolListCtrl(void);
  DECLARE_MESSAGE_MAP()
  void OnClickQuery();
};
