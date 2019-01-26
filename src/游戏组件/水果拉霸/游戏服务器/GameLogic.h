#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"


static CString GetFileDialogPath()
{
	CString strFileDlgPath;
	TCHAR szModuleDirectory[MAX_PATH];	//模块目录
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
	strName.Format(TEXT("%s\\Log\\水果拉霸[%s].log"), GetFileDialogPath(), strRoomName);

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

	//是否机器人
	if (pISendUserItem != NULL)
	{
		strTime.Format(TEXT("NickName=%s,UserID=%ld,AndroidUser=%d"), pISendUserItem->GetNickName(), pISendUserItem->GetUserID(), pISendUserItem->IsAndroidUser());
		strMsg += strTime;
	}

	//信息
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

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const CPoint				m_ptXian[ITEM_COUNT][ITEM_X_COUNT];			//扑克数据
	static const BYTE				m_cbItemType[24];

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//控制函数
public:
	//随机函数
	LONGLONG GetRandMax(BYTE cbIndex, LONGLONG lZhongJiang[], int nCount);
	//随机函数
	LONGLONG LLRand(LONGLONG lMaxCount);
	//混乱扑克
	VOID RandCardList(BYTE cbCardBuffer[ITEM_Y_COUNT][ITEM_X_COUNT], double dControl);
	//取得扑克
	VOID GetCardData(BYTE cbCardBuffer[ITEM_COUNT][ITEM_X_COUNT], double dControl);
	//混乱扑克
	VOID RandCardListEx(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//取得扑克
	int RandCardListThree(BYTE &cbIndex, BYTE cbCardBuffer[4], double dControl);


	//逻辑函数
public:
	//取得中奖
	BYTE GetZhongJiangInfo(BYTE cbItemInfo[][ITEM_X_COUNT], CPoint ptZhongJiang[][ITEM_X_COUNT]);
	//取得中奖
	BYTE GetZhongJiangInfo(BYTE cbIndex, BYTE cbItemInfo[][ITEM_X_COUNT]);
	//取得中奖
	int GetZhongJiangTime(SCORE m_cbLine, BYTE cbItemInfo[ITEM_Y_COUNT][ITEM_X_COUNT] ,INT index);
	//取得中奖
	int GetZhongJiangTime(BYTE cbIndex, BYTE cbItemInfo[][ITEM_X_COUNT]);
	//全盘奖
	int GetQuanPanJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//进入小玛丽
	BYTE GetEnterThree(BYTE cbItemInfo[][ITEM_X_COUNT]);
	//取得中奖
	LONGLONG GetThreeZhongJiangInfo(BYTE cbItemInfo[], BYTE& cbZhongPos);
	//取得中奖
	bool GetThreeZhongJiangInfo(BYTE cbIndex, BYTE cbItemInfo[]);

	//内部函数
public:
	//单线中奖
	BYTE GetZhongJiangXian(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT], CPoint ptZhongJiang[ITEM_X_COUNT]);
	//单线中奖
	int GetZhongJiangTime(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT]);
	//进入小玛丽
	BYTE GetEnterThree(BYTE cbItemInfo[][ITEM_X_COUNT], const CPoint ptXian[ITEM_X_COUNT]);
	
};

//////////////////////////////////////////////////////////////////////////////////

#endif