#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

#define MAX_COUNT					3									//�����Ŀ
#define	DRAW						2									//�;�����

//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//�˿�����
#define CT_SINGLE					1									//��������
#define CT_DOUBLE					2									//��������
#define	CT_SHUN_ZI					3									//˳������
#define CT_JIN_HUA					4									//������
#define	CT_SHUN_JIN					5									//˳������
#define	CT_BAO_ZI					6									//��������
#define CT_SPECIAL					7									//��������

//////////////////////////////////////////////////////////////////////////

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
	CString strName;
	strName.Format(TEXT("%s\\ZaJinHua-%s.log"), GetFileDialogPath(), strRoomName);

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

//��Ϸ�߼���
class CGameLogic
{
	//��������
private:
	static BYTE						m_cbCardListData[52];				//�˿˶���

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//���ƺ���
public:
	//�����˿�
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//�����˿�
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//���ܺ���
public:
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	//�Ա��˿�
	BYTE CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount);
};

//////////////////////////////////////////////////////////////////////////

#endif
