#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"


static CString GetFileDialogPath()
{
	CString strFileDlgPath;
	TCHAR szModuleDirectory[MAX_PATH];	//ģ��Ŀ¼
	GetModuleFileName(AfxGetInstanceHandle(),szModuleDirectory,sizeof(szModuleDirectory));
	int nModuleLen=lstrlen(szModuleDirectory);
	int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE")) + 1;
	if (nModuleLen<=nProcessLen) 
		return TEXT("");
	szModuleDirectory[nModuleLen-nProcessLen]=0;
	strFileDlgPath = szModuleDirectory;
	return strFileDlgPath;
}

static void NcaTextOut(CString strInfo, CString strRoomName)
{
	CreateDirectory(GetFileDialogPath() + TEXT("\\Log"), NULL);
	CString strName;
	strName.Format(TEXT("%s\\Log\\ˮ������[%s].log"), GetFileDialogPath(), strRoomName);

	CTime time = CTime::GetCurrentTime() ;
	CString strTime ;
	strTime.Format( TEXT( "%d-%d-%d %d:%d:%d" ) ,
		time.GetYear() ,
		time.GetMonth() ,
		time.GetDay() ,
		time.GetHour() ,
		time.GetMinute() ,
		time.GetSecond()
		);
	CString strMsg;
	strMsg.Format(TEXT("%s,%s\r\n"),strTime,strInfo);

	CFile fLog;
	if(fLog.Open( strName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite ))
	{

		fLog.SeekToEnd(); 	
		int strLength=strMsg.GetLength();
#ifdef _UNICODE
		BYTE bom[2] = {0xff, 0xfe};
		fLog.Write(bom,sizeof(BYTE)*2);
		strLength*=2;
#endif
		fLog.Write((LPCTSTR)strMsg,strLength);
		fLog.Close();
	}

}
static bool NcaTextOut(UINT uLine, IServerUserItem * pISendUserItem = NULL, CString str = TEXT(""), bool bRet = false)
{
	CString strInfo;
	strInfo.Format(TEXT("LINE=%d"), uLine);
	CreateDirectory(GetFileDialogPath() + TEXT("\\SGLBLog"), NULL);

	CTime time = CTime::GetCurrentTime() ;

	CString strName;
	strName.Format(TEXT("%s\\SGLBLog\\SGLB%04d%02d%02d.log"), 
		GetFileDialogPath(),
		time.GetYear(),
		time.GetMonth(),
		time.GetDay()
		);

	CString strTime ;
	strTime.Format( TEXT( "%04d-%02d-%02d %02d:%02d:%02d" ) ,
		time.GetYear() ,
		time.GetMonth() ,
		time.GetDay() ,
		time.GetHour() ,
		time.GetMinute() ,
		time.GetSecond()
		);
	CString strMsg;
	strMsg.Format(TEXT("%s,%s,"),strTime,strInfo);

	//�Ƿ������
	if (pISendUserItem != NULL)
	{
		strTime.Format(TEXT("NickName=%s,UserID=%ld,AndroidUser=%d"), pISendUserItem->GetNickName(), pISendUserItem->GetUserID(), pISendUserItem->IsAndroidUser());
		strMsg += strTime;
	}

	//��Ϣ
	if (!str.IsEmpty())
	{
		strMsg += str;
	}

	strMsg += TEXT("\r\n");

	CFile fLog;
	if(fLog.Open( strName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite ))
	{

		fLog.SeekToEnd(); 	
		int strLength=strMsg.GetLength();
#ifdef _UNICODE
		BYTE bom[2] = {0xff, 0xfe};
		fLog.Write(bom,sizeof(BYTE)*2);
		strLength*=2;
#endif
		fLog.Write((LPCTSTR)strMsg,strLength);
		fLog.Close();
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const CPoint				m_ptXian[ITEM_COUNT][ITEM_X_COUNT];			//�˿�����
	static const BYTE				m_cbItemType[24];

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ƺ���
public:
	//�������
	LONGLONG GetRandMax(BYTE cbIndex, LONGLONG lZhongJiang[], int nCount);
	//�������
	LONGLONG LLRand(LONGLONG lMaxCount);
	//�����˿�
	VOID RandCardList(BYTE cbCardBuffer[ITEM_Y_COUNT][ITEM_X_COUNT], double dControl);
	//ȡ���˿�
	VOID GetCardData(BYTE cbCardBuffer[ITEM_COUNT][ITEM_X_COUNT], double dControl);
	//�����˿�
	VOID RandCardListEx(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//ȡ���˿�
	int RandCardListThree(BYTE &cbIndex, BYTE cbCardBuffer[4], double dControl);


	//�߼�����
public:
	//ȡ���н�
	BYTE GetZhongJiangInfo(BYTE cbItemInfo[][ITEM_X_COUNT], CPoint ptZhongJiang[][ITEM_X_COUNT]);
	//ȡ���н�
	BYTE GetZhongJiangInfo(BYTE cbIndex, BYTE cbItemInfo[][ITEM_X_COUNT]);
	//ȡ���н�
	int GetZhongJiangTime(SCORE m_cbLine, BYTE cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT] ,INT index);
	//ȡ���н�
	int GetZhongJiangTime(BYTE cbIndex, BYTE cbItemInfo[][ITEM_X_COUNT]);
	//ȫ�̽�
	int GetQuanPanJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//����С����
	BYTE GetEnterThree(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//ȡ���н�
	LONGLONG GetThreeZhongJiangInfo(BYTE cbItemInfo[], BYTE& cbZhongPos);
	//ȡ���н�
	bool GetThreeZhongJiangInfo(BYTE cbIndex, BYTE cbItemInfo[]);

	//�ڲ�����
public:
	//�����н�
	BYTE GetZhongJiangXian(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT], CPoint ptZhongJiang[ITEM_X_COUNT]);
	//�����н�
	int GetZhongJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT]);
	//����С����
	BYTE GetEnterThree(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT]);
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif