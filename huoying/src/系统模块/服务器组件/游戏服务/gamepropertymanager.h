#ifndef GAME_PROPERTY_MANAGER_HEAD_FILE
#define GAME_PROPERTY_MANAGER_HEAD_FILE

#pragma once
#include <map>
#include <vector>
#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//���Ͷ���
typedef CWHArray<tagPropertyInfo>	CPropertyInfoArray;							//��������
typedef std::map<DWORD, std::vector<tagPropertyBuff> > CPropertyBuffMAP;		//��ҵ���Buff	

//////////////////////////////////////////////////////////////////////////////////

//���߹���
class GAME_SERVICE_CLASS CGamePropertyManager
{
	//��������
protected:
	CPropertyInfoArray				m_PropertyInfoArray;				//��������
	static CPropertyBuffMAP			m_PropertyBuffMap;					//����Buff

	//��������
public:
	//���캯��
	CGamePropertyManager();
	//��������
	virtual ~CGamePropertyManager();

	//���ú���
public:
	//���õ���
	bool SetGamePropertyInfo(tagPropertyInfo PropertyInfo[], WORD wPropertyCount);
	bool SetGamePropertyBuff(const DWORD dwUserID, const tagPropertyBuff PropertyBuff[], const WORD wBuffCount);
	bool ClearUserBuff(const DWORD dwUserID);

	//���ܺ���
public:
	//���ҵ���
	tagPropertyInfo * SearchPropertyItem(WORD wPropertyIndex);
	static tagPropertyBuff* SearchValidPropertyBuff(const DWORD dwUserID, const PROP_KIND Kind);
};

//////////////////////////////////////////////////////////////////////////////////

#endif