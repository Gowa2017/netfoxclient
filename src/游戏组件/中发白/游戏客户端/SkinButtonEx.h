#ifndef SKIN_BUTTONEX_HEAD_FILE
#define SKIN_BUTTONEX_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////////////

//��ť��
class CSkinButtonEx : public CSkinButton
{
	//��������
public:
	//���캯��
	CSkinButtonEx();
	//��������
	virtual ~CSkinButtonEx();

	//���غ���
protected:
	//�󶨺���
	virtual VOID PreSubclassWindow();
	//�滭����
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//�滭����
	VOID DrawButtonText(CDC * pDC, UINT uItemState);
};

//////////////////////////////////////////////////////////////////////////////////

#endif