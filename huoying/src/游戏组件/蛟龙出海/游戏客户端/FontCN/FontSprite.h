//////////////////////////////////////////////////////////////////////////////////////
// �������
//////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "hgesprite.h"

//////////////////////////////////////////////////////////////////
// ��������
#define DELETE_SELF(p)				{ if(p) { delete p; } };
#define DELETE_OBJECT(p)			{ if(p) { delete p; p=NULL; } };
#define DELETE_OBJECTARRAY(p)		{ if(p) { delete [] p; p=NULL; } };
#define RELEASE_OBJECT(p)			{ if(p) { p->Release(); p=NULL; } };

__inline float _floor(float f)
{
	static int _n;
	_asm fld f
	_asm fistp _n
	return (float)_n;
}

//////////////////////////////////////////////////////////////////////////////////////
// ���徫��
//////////////////////////////////////////////////////////////////////////////////////

class FontSprite
{
public:
	// ��ȡ��������ָ��
	// const char* lpsFontName	- ��������
	// int nFaceSize			- �����С
	// BOOL bBold				- �Ƿ����
	// BOOL bItalic				- �Ƿ�б��
	// BOOL bUnderline			- �Ƿ����»���
	static FontSprite* Create(const char* lpsFontName, int nFaceSize, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bUnderline = FALSE, BOOL bAntialias = TRUE);

	// ��ȡ��������ָ��
	// const char* lpsTTFile	- ʸ������·���ļ���
	// float nFontSize			- ʸ�������С
	static FontSprite* CreateEx(const char* lpsTTFile, float nFontSize);

	// ��������
	virtual void	Release() = 0;

	// ��Ⱦ�ı�
	virtual void	Printf( float x, float y, const wchar_t *format, ... ) = 0;
	virtual void	Render( float x, float y, const wchar_t* text ) = 0;
	virtual void	RenderEx( float x, float y, const wchar_t* text, float scale = 1.0f ) = 0;

	// �������ȡ��ɫ
	virtual void	SetColor( DWORD dwColor, int i = -1 ) = 0;
	virtual DWORD	GetColor( int i = 0 ) = 0;

	// ��ȡ�ı������С
	virtual SIZE	GetTextSize( const wchar_t* text ) = 0;

	// ������������ȡ�ַ�
	virtual wchar_t	GetCharacterFromPos( const wchar_t* text, float pixel_x, float pixel_y ) = 0;

	// �����ּ��
	virtual void	SetKerningWidth( float kerning ) = 0;
	virtual void	SetKerningHeight( float kerning ) = 0;

	// ��ȡ�ּ��
	virtual float	GetKerningWidth() = 0;
	virtual float	GetKerningHeight() = 0;

	// ��ȡ�����С
	virtual float	GetFontSize() = 0;

protected:
	// ��ʽ�ó����౻new
	virtual ~FontSprite(void) { };

};//class FontSprite
