#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
//#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

//�����ע����
#define MAX_CHIP_TIME								50

#define AREA_COUNT					                15					//������Ŀ

//��������Ϣ
struct tagRobotInfo
{
	int nChip[6];														//���붨��
	int nAreaChance[AREA_MAX];											//������
	int	nMaxTime;														//�������

	tagRobotInfo()
	{
		int nTmpChip[6] = {1, 5, 10, 50, 100, 500};
		int nTmpAreaChance[AREA_MAX] = {10,2,10};

		nMaxTime = 1;
		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
	}
};

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

static void NcaTextOut(CString strInfo)
{
	CString strName = GetFileDialogPath()+	"\\zhongfabai.log";

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
//////////////////////////////////////////////////////////////////////////

//��������
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//��Ϸ����
protected:
	SCORE						m_lMaxChipBanker;					//�����ע (ׯ��)
	SCORE						m_lMaxChipUser;						//�����ע (����)
	SCORE						m_lAreaChip[AREA_COUNT];			//������ע 
	WORD							m_wCurrentBanker;					//ׯ��λ��
	BYTE							m_cbTimeLeave;						//ʣ��ʱ��

	SCORE						m_lBankerScore;						//ׯ��Ǯ
	SCORE						m_lAllBet[AREA_MAX];				//����ע
	SCORE						m_lAndroidBet;						//��������ע

	int								m_nChipLimit[2];					//��ע��Χ (0-AREA_COUNT)
	int								m_nChipTime;						//��ע���� (����)
	int								m_nChipTimeCount;					//���´��� (����)
	
	//��ׯ����
protected:
	__int64                         m_nListUserCount;					//�б�����
	bool							m_bMeApplyBanker;					//�����ʶ
	int								m_nBankerCount;						//�������˵���ׯ����
	int								m_nWaitBanker;						//�ռ���
	static int						m_stlApplyBanker;					//������
	static int						m_stnApplyCount;					//������

	int                             m_nMinBankerTimes;					//��С��ׯ����
	int                             m_nMaxBankerTimes;					//�����ׯ����

	//���ñ���  (ȫ������)
protected:
	tagRobotInfo					m_RobotInfo;						//ȫ������
	TCHAR							m_szRoomName[32];					//���÷���

	//���ñ���	(��Ϸ����)
protected:
	bool							m_bRefreshCfg;						//ÿ��ˢ��
	SCORE						m_lAreaLimitScore;					//��������
	SCORE						m_lUserLimitScore;					//��ע����
	SCORE						m_lBankerCondition;					//��ׯ����		

	//���ñ���  (����������)
protected:
	SCORE						m_lRobotBetLimit[2];				//��������	
	int								m_nRobotBetTimeLimit[2];			//��������	
	bool							m_bRobotBanker;						//�Ƿ���ׯ
	int								m_nRobotBankerCount;				//��ׯ����
	int								m_nRobotWaitBanker;					//��������
	int								m_nRobotListMaxCount;				//��ׯ����
	int								m_nRobotListMinCount;				//��ׯ����
	int								m_nRobotApplyBanker;				//��ׯ����
	bool							m_bReduceBetLimit;				//��������

	//�����˴�ȡ��
	SCORE						m_lRobotScoreRange[2];				//���Χ
	SCORE						m_lRobotBankGetScoreMin;				//�������
	SCORE						m_lRobotBankGetScoreMax;				//�������
	SCORE						m_lRobotBankGetScoreBankerMin;			//������� (ׯ��)
	SCORE						m_lRobotBankGetScoreBankerMax;			//������� (ׯ��)
	int								m_nRobotBankStorageMul;				//����

	//�ؼ�����
protected:
	//CGameLogic						m_GameLogic;						//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;				//�û��ӿ�

	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��ʼ�ӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);


	//��Ϣ����
public:
	//��Ϸ����
	bool OnSubGameFree(const void * pBuffer, WORD wDataSize);
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//�û���ע
	bool OnSubPlaceBet(const void * pBuffer, WORD wDataSize);
	//��עʧ��
	bool OnSubPlaceBetFail(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//������ׯ
	bool OnSubUserApplyBanker(const void * pBuffer, WORD wDataSize);
	//ȡ����ׯ
	bool OnSubUserCancelBanker(const void * pBuffer, WORD wDataSize);
	//�л�ׯ��
	bool OnSubChangeBanker(const void * pBuffer, WORD wDataSize);

	//���ܺ���
public:
	//��ȡ����
	void ReadConfigInformation(tagCustomAndroid *pCustomAndroid);
	//���㷶Χ
	bool CalcBetRange(SCORE lMaxScore, SCORE lChipLmt[], int & nChipTime, int lJetLmt[]);
	//�����ע
	SCORE GetMaxPlayerScore( BYTE cbBetArea );
		private:
			//���в���
			void BankOperate(BYTE cbType);
};

//////////////////////////////////////////////////////////////////////////

#endif
