#ifndef GDIFONT_H
#define GDIFONT_H
#include "FontSprite.h"
#include <list>

class GdiFont :
	public FontSprite
{
public:
	GdiFont(const char* lpsFontName, int nFaceSize, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bUnderline = FALSE, BOOL bAntialias = TRUE);
	~GdiFont(void);

	void StaticCacheCharacter(const wchar_t* text);

public:
	// �ͷ�һ��GdiFont�������
	virtual void	Release();

	// ��Ⱦ�ı�
	virtual void	Printf( float x, float y, const wchar_t *format, ... );
	virtual void	Render(float x, float y, const wchar_t* text );
	virtual void	RenderEx( float x, float y, const wchar_t* text, float scale = 1.0f );

	// �������ȡ��ɫ
	virtual void	SetColor( DWORD dwColor, int i = -1 );
	virtual DWORD	GetColor( int i = 0 );

	// ��ȡ�ı����
	virtual SIZE	GetTextSize( const wchar_t* text );

	// ���������ȡ�ַ�
	virtual wchar_t	GetCharacterFromPos( const wchar_t* text, float pixel_x, float pixel_y );

	// �����ּ��
	virtual void	SetKerningWidth( float kerning );
	virtual void	SetKerningHeight( float kerning );

	// ��ȡ�ּ��
	virtual float	GetKerningWidth();
	virtual float	GetKerningHeight();

	// �����С
	virtual float	GetFontSize();

private:
	// �����ַ���ȡ����
	unsigned int	GetGlyphByCharacter( wchar_t c );
	inline float	GetWidthFromCharacter( wchar_t c );
	inline void		CacheCharacter(unsigned int idx, wchar_t c);

	typedef struct tagEngineFontGlyph
	{
		float		x;
		float		y;
		float		w;
		float		h;
		HTEXTURE	t;
		wchar_t		c;
	}TENGINEFONTGLYPH;

	static const unsigned int font_count = 0xFFFF;// = sizeof(wchar_t);
	static const int	tex_size = 256;

	std::list<HTEXTURE>	m_FontTextures;
	TENGINEFONTGLYPH	m_FontGlyphs[font_count];
	float				m_nFontSize;
	float				m_nKerningWidth;
	float				m_nKerningHeight;
	BOOL				m_bItalic;

	HGE*				m_pHGE;
	hgeSprite*			m_pSprite;

	// GDI�豸
	HDC					m_hMemDC;
	HBRUSH				m_hBrush;
	HFONT				m_hFont;
	HBITMAP				m_hBitmap;
	void*				m_pBuffer;

	// ��ʱ������ģ������Ϣ
	HTEXTURE			m_hTexLetter;
	POINT				m_ptLetter;
};
#endif//GDIFONT_H
