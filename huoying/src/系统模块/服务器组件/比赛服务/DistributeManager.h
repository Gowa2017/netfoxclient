#ifndef DISTRIBUTE_MANAGER_HEAD_FILE
#define DISTRIBUTE_MANAGER_HEAD_FILE

#pragma once

#include "MatchServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//ͬ����Ϣ
struct tagSameTableInfo
{
	WORD							wPlayerCount;					    //�����Ŀ
	DWORD							wPlayerIDSet[16];					//��Ҽ���
};

//��������
struct tagDistributeInfo
{
	WORD							wDistribute;						//����ȼ�	
	DWORD							dwClientAddr;						//��ҵ�ַ
	DWORD							dwInsertStamp;						//����ʱ��
	IServerUserItem *				pIServerUserItem;					//�û��ӿ�
	VOID *                          pPertainNode;                       //�������
};

//������
struct tagDistributeNode
{
	tagDistributeNode *             pPrepDistributeNode;               //ǰһ���
	tagDistributeNode *			    pNextDistributeNode;               //��һ���
	tagDistributeInfo               DistributeInfo;                    //������Ϣ

	tagDistributeNode()
	{
		pPrepDistributeNode=NULL;
		pNextDistributeNode=NULL;
	}
	~tagDistributeNode()
	{
		pPrepDistributeNode=NULL;
		pNextDistributeNode=NULL;
	}
};  

//////////////////////////////////////////////////////////////////////////////////

//���Ͷ���
typedef CWHArray<tagSameTableInfo *> CSameTableArray;						//ͬ������
typedef CWHArray<tagDistributeInfo> CDistributeInfoArray;					//��������
typedef CMap<DWORD,DWORD,tagSameTableInfo *, tagSameTableInfo*>	CSameTableMap;	//ͬ����Ϣ

//////////////////////////////////////////////////////////////////////////////////

//����
class  CDistributeNodePool
{
	//�������
public:
	static const int BLOCK_SIZE;
	static tagDistributeNode * m_pHeadOfFreeList;

	//��������
public:
	//���캯��
	CDistributeNodePool();
	//��������
	~CDistributeNodePool();

	//���ܺ���
public:
	//������
	tagDistributeNode * AllocNode();
	//�ͷŽ��
	VOID FreeNode(void * pNode);
};

//////////////////////////////////////////////////////////////////////////////////

//�������
class CDistributeManager
{
	//��Ŀ����
protected:
	WORD							m_wNodeCount;				      //�����Ŀ
	WORD                            m_wRealCount;	                  //������Ŀ
	WORD                            m_wAndroidCount;		          //������Ŀ 

	//�洢����
protected:
	CSameTableArray					m_SameTableBuffer;				  //ͬ������
	CSameTableArray					m_SameTableActive;				  //ͬ������
	CSameTableMap					m_SameTableMap;					  //ͬ���ֵ�

	//�������
protected:
	CDistributeNodePool             m_DistributeNodePool;             //���� 
	tagDistributeNode *				m_pHeadNode;					  //ͷ�����

	//�������
protected:
	BYTE							m_cbDistributeRule;				  //�������

	//��������
public:
	//���캯��
	CDistributeManager();
	//��������
	virtual ~CDistributeManager();

	//��������
public:
	//�����Ŀ
	inline WORD  GetCount() { return m_wNodeCount; }
	//������Ŀ
	inline WORD  GetRealCount() { return m_wRealCount; };
	//������Ŀ
	inline WORD  GetAndroidCount() { return m_wAndroidCount; };
	//���ù���
	inline VOID  SetDistributeRule(BYTE cbDistributeRule) { m_cbDistributeRule=cbDistributeRule; }

	//���ܺ���
public:	
	//�Ƴ����
	VOID RemoveAll();
	//������
	BOOL InsertDistributeNode(const tagDistributeInfo & DistributeInfo);
	//�Ƴ����
	VOID RemoveDistributeNode(tagDistributeNode * pDistributeNode);
	//�Ƴ����
	VOID RemoveDistributeNode(IServerUserItem  *  pIServerUserItem);	
	//ִ�з���
	WORD PerformDistribute(CDistributeInfoArray & DistributeInfoArray,WORD wNeedCount);

	//���ܺ���
public:
	//���ҽ��
	tagDistributeNode* SearchNode(IServerUserItem  * pIServerUserItem);
	//��ȡ��Ϣ
	tagSameTableInfo * GetUserSameTableInfo(DWORD dwUserID);
	//�Ƴ���Ϣ
	VOID RemoveUserSameTableInfo(DWORD dwUserID);

	//�ڲ�����
protected:
	//�������
	tagSameTableInfo * ActiveSameTableInfo();
	//�Ƴ�����
	VOID RemoveSameTableInfo(tagSameTableInfo * pSameTableInfo);	

	//������
protected:
	//IPַͬ
	inline BOOL FilterRuleExitsIPAddr(const CDistributeInfoArray & DistributeInfoArray,DWORD dwClientAddr);	
	//��������
	inline BOOL FilterRuleIsAllAndroid(const CDistributeInfoArray & DistributeInfoArray, IServerUserItem  * const pIServerUserItem);
	//�Ͼ�ͬ��
	inline BOOL FilterRuleIsLastSameTable(const CDistributeInfoArray & DistributeInfoArray,DWORD dwUserID);
};

//////////////////////////////////////////////////////////////////////////////////

#endif