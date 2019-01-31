#ifndef _LUA_ASSERT_H_
#define _LUA_ASSERT_H_
#include <iconv.h>

#ifdef __cplusplus
extern "C" {
#endif


int code_convert(const char *from_charset, const char *to_charset, char *inbuf,
size_t inlen, char *outbuf, size_t outlen);

/* UTF-8 to wchar  */
int u2w(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

/* UTF-8 to gbk  */
int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

/* wchar to UTF-8 */
int w2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

/* wchar to gbk */
int w2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

/* gbk to UTF-8 */
int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

/* gbk to wchar */
int g2w(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

bool ToServerString(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

bool ToClientString(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

#ifdef __cplusplus
 }
#endif

#endif