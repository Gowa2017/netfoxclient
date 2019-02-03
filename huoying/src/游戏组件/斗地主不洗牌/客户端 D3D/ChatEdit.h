#pragma once
#include "Stdafx.h"
#include "SkinVerticleScrollbar.h"

//////////////////////////////////////////////////////////////////////////////////

class CChatEdit : public CRichEditCtrl
{
	//DECLARE_DYNAMIC(CChatEdit)

	//��Ա����
private:
	CFont							m_font;								//�ı�����
	COLORREF						m_crTextColor;						//�ı���ɫ
	CPngImage						m_PngBack;							//

	//�������
protected:
	CSkinVerticleScrollbar			m_SkinVerticleScrollbar;			//��������

	//��������
public:
	//���캯��
	CChatEdit();
	//��������
	~CChatEdit();

	//���ܺ���
public:
	//��ʼ������
	VOID Initialization();
	//�����ؼ�
	VOID RectifyControl(INT nWidth, INT nHeigth);
	//����������ɫ
	VOID SetTextColor(COLORREF rcTextColor);
	//��������
	VOID SetTextFont(const LOGFONT &lfTextFont);
	//��������
	VOID SetHideWindow(bool bHide);
	//��ȡ������ɫ
	COLORREF GetTextColor(void) {return m_crTextColor;}
	//��ȡ��ǰ����
	BOOL GetTextFont(LOGFONT &lfTextFont);

	VOID DrawScrollBar();

protected:
	virtual void PreSubclassWindow();

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnEnVscroll();
//	afx_msg void OnPaint();
//	afx_msg void OnNcPaint();
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

//////////////////////////////////////////////////////////////////////////////////
