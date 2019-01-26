//-----------------------------------------------------------------------------
// File: mp_string.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_STRING
#define MAGIC_STRING

#include <stdio.h>

#pragma warning ( disable : 4996)

class MP_String
{
protected:
	char* theString;		// eng: pointer to string
							// rus: ��������?�� ������

	int stringLen;			// eng: string length
							// rus: ����?������
   
public:
	// eng: Constructors
	// rus: ����������?
	MP_String();
	MP_String(char s);
	MP_String (const char*);
	MP_String(const MP_String&);

	// eng: Destructor
	// rus: ����������
	virtual ~MP_String(void);

	MP_String& operator=(const MP_String&);
	MP_String& operator=(char* s);
	bool operator==(const MP_String&);
	bool operator!=(const MP_String&);
	bool operator>(const MP_String&);
	bool operator<(const MP_String&);
	char& operator[](int n);
	MP_String operator+(const MP_String& so);
	MP_String operator+(const char c);
	MP_String& operator+=(const MP_String& so);
	MP_String& operator+=(const char c);

	// eng: Loading string from stream
	// rus: ������ ������ �� ������
	void LoadFromStream(FILE* stream);

	// eng: Save string to stream
	// rus: ������ ������ ?����?
	void SaveToStream(FILE* stream);

	// eng: Save string to stream as text
	// rus: ������ ������ ?����??���� ������
	void SaveToStreamByText(FILE* stream);

	#ifndef MP_STRING_LITE
	// eng: Reversing
	// rus: ��������������
	MP_String& operator~(void);
	#endif

	// eng: String length
	// rus: ����?������
	int length(void);

	#ifndef MP_STRING_LITE
	// eng: Transformation to capital letters
	// rus: �������������� ?��������?����?
	MP_String& toUpper(void);

	// eng: Transformation to small letters
	// rus: �������������� ?�������� ����?
	MP_String& toLower(void);
	#endif

	// eng: Find symbol in string
	// rus: ����?������??������
	int find(char c);

	// eng: Find substring in string
	// rus: ����?��������??������
	int find(const char* subs);
	int find(const MP_String& so);

	// eng: String pointer
	// rus: ����?������
	const char* c_str();

	// eng: Returns substring using specified position and length
	// rus: ��������?��������?�� ������ ������ ?�������� ������??��������?������ ��������
	MP_String substr(int pos,int plen);

	// eng: Left substring
	// rus: ����� ��������?
	MP_String left(int len);

	// eng: Rigth substring
	// rus: ����? ��������?
	MP_String right(int len);

	bool empty();
};

class MP_StringW
{
protected:
	wchar_t* theString;		// eng: pointer to string
							// rus: ��������?�� ������

	int stringLen;			// eng: string length
							// rus: ����?������
   
public:
	// eng: Constructors
	// rus: ����������?
	MP_StringW();
	MP_StringW(char s);
	MP_StringW(const char* s);
	MP_StringW(const wchar_t*);
	MP_StringW(const MP_StringW&);

	// eng: Destructor
	// rus: ����������
	virtual ~MP_StringW(void);

	MP_StringW& operator=(const MP_StringW&);
	MP_StringW& operator=(const wchar_t* s);
	MP_StringW& operator=(const char* s);
	bool operator==(const MP_StringW&);
	bool operator!=(const MP_StringW&);
	bool operator>(const MP_StringW&);
	bool operator<(const MP_StringW&);
	MP_StringW operator+(const MP_StringW& so);
	MP_StringW& operator+=(const MP_StringW& so);

	// eng: Loading string from stream
	// rus: ������ ������ �� ������
	void LoadFromStream(FILE* stream);

	// eng: Save string to stream
	// rus: ������ ������ ?����?
	void SaveToStream(FILE* stream);

	#ifndef MP_STRING_LITE
	// eng: Reversing
	// rus: ��������������
	MP_StringW& operator~(void);
	#endif
	
	// eng: String length
	// rus: ����?������
	int length(void);

	#ifndef MP_STRING_LITE
	// eng: Transformation to capital letters
	// rus: �������������� ?��������?����?
	MP_StringW& toUpper(void);

	// eng: Transformation to small letters
	// rus: �������������� ?�������� ����?
	MP_StringW& toLower(void);
	#endif

	// eng: Find substring in string
	// rus: ����?��������??������
	int find(const wchar_t* subs);
	int find(const MP_StringW& so);

	// eng: String pointer
	// rus: ����?������
	const wchar_t* w_str();

	bool empty();
};


#endif
