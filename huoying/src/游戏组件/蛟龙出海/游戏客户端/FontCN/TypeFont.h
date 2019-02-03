////////////////////////////////////////////////////////////////////////////
// ʹ�� FreeType2 ��ʵ�ֺ�����ʾ��
// ΢���ƽ��
// 2007-10-12
////////////////////////////////////////////////////////////////////////////

#ifndef TYPEFONT_H
#define TYPEFONT_H

#include "FontSprite.h"
#include <vector>

// ����ʸ������֧�ֵ�lib
//#pragma comment(lib,"FontCN/freetype/objs/freetype235MT.lib")

extern "C"
{
#include "ft2build.h"
#include "freetype\freetype.h"
};

////////////////////////////////////////////////////////////////////////////
// ��������
typedef struct tagTTFontGlyph
{
	float		x;
	float		y;
	float		w;
	float		h;
	HTEXTURE	hTexture;
	void Reset()
	{
		x=0;y=0;w=0;h=0;hTexture=NULL;
	}
	void Release(HGE* pHGE)
	{
		if (hTexture)
		{
			pHGE->Texture_Free( hTexture );
			hTexture = NULL;
		}
	};
}TTFONTGLYPH;

////////////////////////////////////////////////////////////////////////////
// ����
class TypeFont :
	public FontSprite
{
public:
	TypeFont();
	virtual ~TypeFont();

	// �ͷ�һ��TypeFont�������
	virtual void	Release();

	// ��Ⱦ�ı�
	virtual void	Printf( float x, float y, const wchar_t *format, ... );
	virtual void	Render( float x, float y, const wchar_t* text );
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

	// ��������
	bool			Attach(const char* lpsTTFile, float nFontSize);

private:

	// �����ַ���ȡ����
	unsigned int			GetGlyphByCharacter( wchar_t c );
	inline float			GetWidthFromCharacter( wchar_t c );
	inline void				CacheCharacter(unsigned int idx);


	FT_Library				m_FTLibrary;
	FT_Face					m_FTFace;
	unsigned char*			m_pFontBuffer;
	HGE*					m_pHGE;
	hgeSprite*				m_pSprite;

	std::vector<TTFONTGLYPH>		m_Glyphs;

	float					m_nKerningWidth;
	float					m_nKerningHeight;
	float					m_nFontSize;

};//class TypeFont


#endif //TYPEFONT_H
