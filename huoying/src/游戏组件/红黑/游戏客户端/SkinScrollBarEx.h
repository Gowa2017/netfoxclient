
#pragma once
#include "stdafx.h"
//////////////////////////////////////////////////////////////////////////////////

//��������
class  CSkinScrollBarEx
{
	
	//��Դ����
public:
	CBitImage						m_ImageScroll;						//������Դ
	//��������
public:
	//���캯��
	CSkinScrollBarEx();
	//��������
	virtual ~CSkinScrollBarEx();

	//���ܺ���
public:
	//���ù���
	VOID InitScroolBar(CWnd * pWnd);
};

//////////////////////////////////////////////////////////////////////////////////
