#include "StdAfx.h"
#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include ".\sensitivewordsfilter.h"
#include <locale>


int CToken::isChinese(character *_char)
{        
	if((unsigned short)_char->val_word.high > 127        
		&& (unsigned short)_char->val_word.low > 127)        
		return CH;        
	return EN;        
}        

int CToken::isChinese(LPCTSTR buf,int pos,int size)        
{        
	if(pos+1 > size)        
		return NULL;        
	//  	if((unsigned int)buf[pos]>=MAX_CH)
	//  		return NULL;
	if((unsigned int)buf[pos] > 127 && (unsigned int)buf[pos+1] > 127)        
		return CH;  
	if (buf[pos]<0 && (unsigned int)buf[pos+1] < 127)
		return CH;
	return EN;        
}        

//////////////////////////////////////////////////////////////////////////
CToken *tokenArray::insertToken(unsigned short code,character &_char)        
{        
	std::vector<CToken*>::iterator it = alltokens.begin();        
	std::vector<CToken*>::iterator end = alltokens.end();        
	for( ; it != end; ++it)        
		if( (*it)->getCode() == code)        
			break;        
	if(it != alltokens.end())        
		return *it;        
	else        
	{        
		CToken *tok = new CToken(code,_char);        
		alltokens.push_back(tok);
		nNewCount++;
		return tok;        
	}   

	//ʵ��Ӧ���У������Ҫ����������    
}        

CToken *tokenArray::getToken(unsigned short code)        
{      
	//ʵ��Ӧ����ʹ�ö��ֲ���   
	std::vector<CToken*>::iterator it = alltokens.begin();        
	std::vector<CToken*>::iterator end = alltokens.end();        
	for( ; it != end; ++it)        
		if( (*it)->getCode() == code)        
			return *it;        
	return NULL;        
}        

tokenArray* CToken::getChild(character &_char)        
{        
	if(isChinese(&_char))        
		return &word_children;        
	else        
		return &byte_children;        
}           

CToken* CToken::addChild(unsigned short code,character &_char)        
{        
	if(isChinese(&_char))        
		return word_children.insertToken(code,_char);        
	else        
		return byte_children.insertToken(code,_char);        
}        


//////////////////////////////////////////////////////////////////////////
void CFilterTree::BuildWord(LPCTSTR msg)        
{
}        
    
//�������
CString CFilterTree::Filtrate(LPCTSTR msg)        
{
	//��������
	CString strChat=TEXT("");

	//������֤
	ASSERT(msg!=NULL);
	if(msg==NULL)return strChat; 

	//��ȡ����
	int size = (int)lstrlen(msg);        
	if(size <= 0)return strChat;

	//�������
	strChat=msg;
	for (INT_PTR i=0;i<m_ForbidArray.GetCount();i++)
	{
		strChat.Replace(m_ForbidArray[i],TEXT("*"));
	}

	//
	TCHAR *pLimit[] = { TEXT("25qp"),TEXT("25�ѣ�"),TEXT("�����ѣ�"),TEXT("25q��"),TEXT("25��p")};
	for (INT_PTR i=0;i<CountArray(pLimit);i++)
	{
		strChat.Replace(pLimit[i],TEXT("foxuc"));
	}
	   
	return strChat;          
}

//�������˱�
void CFilterTree::BuildTree()        
{        
	INT_PTR nCount=m_ForbidArray.GetCount();
	for(INT_PTR i = 0; i < nCount; ++i)        
	{       
		BuildWord(m_ForbidArray[i]);           
	}        
}

//��ӽ���
bool CSensitiveWordsFilter::AddSensitiveWords(LPCTSTR pSensitiveWords)
{
	//��ȡ����
	int nlen = lstrlen(pSensitiveWords);

	//����ռ�
	LPTSTR pWords=new TCHAR[nlen+1];
	ZeroMemory(pWords, sizeof(TCHAR)*(nlen+1));

	//��������
	lstrcpyn(pWords,pSensitiveWords,nlen+1);
	
	//��Ӽ�¼
	m_FilterTree.m_ForbidArray.Add(pWords);

	return true;
}

//������
void CSensitiveWordsFilter::FinishAdd()
{
	m_FilterTree.BuildTree();
}

//�������д�
void CSensitiveWordsFilter::Filtrate(LPCTSTR pMsg, LPTSTR pszFiltered, int nMaxLen)
{
	int nLen=0;
	CString lpOut=Filtrate(pMsg,nLen); 
	lstrcpyn(pszFiltered,lpOut,nMaxLen);

	return ;
}

//�������д�
CString  CSensitiveWordsFilter::Filtrate(LPCTSTR pMsg, int &nLenght)
{
	CString ret= m_FilterTree.Filtrate(pMsg);
	nLenght=(int)ret.GetLength();
    return ret;
}

CFilterTree::~CFilterTree()
{   
	Reset();
}

//����
void CFilterTree::Reset()
{
	for (int i=0; i<m_ForbidArray.GetCount(); i++)
	{
		SafeDeleteArray(m_ForbidArray.GetAt(i));
	}
	m_ForbidArray.RemoveAll();
}

//��ʼ��
void CSensitiveWordsFilter::ResetSensitiveWordArray()
{
	m_FilterTree.Reset();
}