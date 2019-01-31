#ifndef __AESENCRYPT_HEAD_FILE__
#define __AESENCRYPT_HEAD_FILE__

#include "tolua_fix.h"
#include "CCLuaEngine.h"
#include "LuaBasicConversions.h"
#include <string>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

int  register_all_AESEncrypt();

class AESEncrypt  
{
public:
	AESEncrypt(unsigned char* key);
	virtual ~AESEncrypt();
	unsigned char* Cipher(unsigned char* input);
	unsigned char* InvCipher(unsigned char* input);
	void* Cipher(void* input, int length=0);
	void* InvCipher(void* input, int length);

private:
	unsigned char Sbox[256];
	unsigned char InvSbox[256];
	unsigned char w[11][4][4];

	void KeyExpansion(unsigned char* key, unsigned char w[][4][4]);
	unsigned char FFmul(unsigned char a, unsigned char b);

	void SubBytes(unsigned char state[][4]);
	void ShiftRows(unsigned char state[][4]);
	void MixColumns(unsigned char state[][4]);
	void AddRoundKey(unsigned char state[][4], unsigned char k[][4]);

	void InvSubBytes(unsigned char state[][4]);
	void InvShiftRows(unsigned char state[][4]);
	void InvMixColumns(unsigned char state[][4]);
};
#endif // __AESENCRYPT_HEAD_FILE__