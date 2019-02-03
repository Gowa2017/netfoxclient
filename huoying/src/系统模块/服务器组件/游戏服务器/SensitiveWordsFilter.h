#ifndef SENSITIVE_WORDS_FILTER_HEAD_FILE
#define SENSITIVE_WORDS_FILTER_HEAD_FILE
#pragma once

#include "Stdafx.h"
#include <vector>        
#include <algorithm>        
#include "..\..\ȫ�ֶ���\array.h"
#include "..\..\ȫ�ֶ���\macro.h"

//////////////////////////////////////////////////////////////////////////
#define MAX_CH 65535 //���������ַ���        
#define CH 2      
#define EN 1      

static int nNewCount=0;

//////////////////////////////////////////////////////////////////////////
//�ַ���ֵ,val_byte����m_pAnsiArray�ַ���val_word���������ַ�        
typedef union         
{        
	CHAR val_byte;        
	struct        
	{        
		CHAR high;        
		CHAR low;        
	}val_word;        
}character;        


class CToken;        

//////////////////////////////////////////////////////////////////////////
//���ڴ���ӽڵ�      
class tokenArray        
{        
public:        

	CToken *getToken(unsigned short code);        
	CToken *insertToken(unsigned short code,character &_char);        
     
	std::vector<CToken*> alltokens;        
};        


//һ���ַ�������������Ҳ������Ӣ�ĵ�        
class CToken        
{        
public:        
	CToken(unsigned short code,character &_char)        
		:code(code),_character(_char),isend(false)        
	{}        

	~CToken()
	{
		for (size_t j=0; j<byte_children.alltokens.size();j++)
		{
			delete(byte_children.alltokens[j]);
			nNewCount--;
		}
		byte_children.alltokens.clear();
		for (size_t j=0; j<word_children.alltokens.size();j++)
		{
			delete(word_children.alltokens[j]);
			nNewCount--;
		}
		word_children.alltokens.clear();

	}
	character* getCharacter()        
	{        
		return &_character;        
	}        

	unsigned short getCode()        
	{        
		return code;        
	}        

	void setEnd()        
	{        
		isend = true;        
	}        

	bool isEnd()//��ǰ�ַ��Ƿ���һ�������Ľ�β        
	{        
		return isend;        
	}        

	static int isChinese(character *_char);
	static int isChinese(LPCTSTR buf,int pos,int size);
	tokenArray* getChild(character &_char);        

	CToken* addChild(unsigned short code,character &_char);        

public:        
	unsigned short code;//�ַ��ı���        
	character _character;        
	tokenArray byte_children;//�����ӽڵ�,(���ģ�      
	tokenArray word_children;//�����ӽڵ�,(Ӣ��)        

	bool isend;//�Ƿ���һ���ʵĴ�β        
};   

//////////////////////////////////////////////////////////////////////////
typedef CWHArray<LPCTSTR > CForbidWordArray;

//////////////////////////////////////////////////////////////////////////
//���ʹ�����        
class CFilterTree        
{
public:
	CForbidWordArray		m_ForbidArray;

	//��������
public:        
	CFilterTree()        
	{  
	}   

	~CFilterTree();	   

	//���ܺ���
public:
	//�������˱�
	void BuildTree();
	//Ԥ����һ����        
	void BuildWord(LPCTSTR msg);
	//�������ڹ���        
	CString Filtrate(LPCTSTR msg);
	//����
	void Reset();
};      

//////////////////////////////////////////////////////////////////////////
//�������д�
class CSensitiveWordsFilter
{
	//��������
protected:
	CFilterTree			m_FilterTree;                         //�ؼ�����
	//CToken				m_Token;

	//��������
public:
	//���캯��
	CSensitiveWordsFilter(){}
	//��������
	virtual ~CSensitiveWordsFilter(){}

	//���ܺ���
public:
	//��ʼ��
	void ResetSensitiveWordArray();
	//��ӽ���
	bool AddSensitiveWords(LPCTSTR pSensitiveWords);
	//������
	void FinishAdd();
	//�������д�
	CString Filtrate(LPCTSTR pMsg, int &nLenght);
	//�������д�
	void Filtrate(LPCTSTR pMsg, LPTSTR pszFiltered, int nMaxLen);
};

//////////////////////////////////////////////////////////////////////////
#endif