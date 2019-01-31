#include "LuaAssert.h"
#include <string.h>

int code_convert(const char *from_charset, const char *to_charset, char *inbuf,
		size_t inlen, char *outbuf, size_t outlen)
{
	iconv_t cd;
#ifdef WIN32

	const char **pin = (const char **)&inbuf;
#else
	char **pin = &inbuf;
#endif
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
		return -1;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
		return -1;
	iconv_close(cd);
	return 0;
}

/* UTF-8 to wchar  */
int u2w(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("UTF-8", "UTF-16LE", inbuf, inlen, outbuf, outlen);
}
/* UTF-8 to gbk  */
int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("UTF-8", "GBK", inbuf, inlen, outbuf, outlen);
}
/* wchar to UTF-8 */
int w2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("UTF-16LE", "UTF-8", inbuf, inlen, outbuf, outlen);
}
/* wchar to gbk */
int w2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("UTF-16LE", "GBK", inbuf, inlen, outbuf, outlen);
}
/* gbk to UTF-8 */
int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("GBK", "UTF-8", inbuf, inlen, outbuf, outlen);
}

/* gbk to wchar */
int g2w(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("GBK", "UTF-16LE", inbuf, inlen, outbuf, outlen);
}

bool ToServerString(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return u2w(inbuf,  inlen, outbuf,  outlen) == 0;
}

bool ToClientString(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return w2u(inbuf,  inlen, outbuf,  outlen) == 0;
}