//-----------------------------------------------------------------------------
// File: platform_win.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_PLATFORM
#define MAGIC_PARTICLES_PLATFORM

#include <windows.h>
#include "mp.h"

class MP_Platform_WINDOWS : public MP_Platform
{
protected:
	WIN32_FIND_DATAW fd;
	HANDLE hFindFile;

public:
	MP_Platform_WINDOWS();
	virtual ~MP_Platform_WINDOWS();

	// eng: Returns time in milliseconds
	// rus: ���������� ����� ?������������
	virtual unsigned long GetTick();

	// eng: Returns path to folder with emitters
	// rus: ���������� ���� ?����??ptc-������?
	virtual const wchar_t* GetPathToPTC();

	// eng: Returns path to folder with textures
	// rus: ���������� ���� ?����??����������
	virtual const wchar_t* GetPathToTexture();

	// eng: Returns path to folder which could be used by wrapper to store temporary files
	// rus: ���������� ���� ?��������?����?
	virtual const wchar_t* GetPathToTemp();

	// eng: Finds the first ptc-file in emitters folder
	// rus: ����?������?ptc-����??����??����������
	virtual const wchar_t* GetFirstFile();

	// eng: Finds the next ptc-file in emitters folder
	// rus: ����?���������� ptc-����??����??����������
	virtual const wchar_t* GetNextFile();

	// eng: Converts wchar_t string into utf8
	// rus: ������������ ������ ���� wchar_t ?������ ���� utf8
	virtual const char* wchar_to_utf8(const wchar_t* str);

	// eng: Converts utf8 string into wchar_t
	// rus: ������������ ������ ���� utf8 ?������ ���� wchar_t
	virtual const wchar_t* utf8_to_wchar(const char* str);
};

#endif