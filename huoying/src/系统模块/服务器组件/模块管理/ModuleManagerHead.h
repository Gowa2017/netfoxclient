#ifndef MODULE_MANAGER_HEAD_HEAD_FILE
#define MODULE_MANAGER_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"

//�������
#include "..\..\�������\�������\ServiceCoreHead.h"
#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef MODULE_MANAGER_CLASS
	#ifdef  MODULE_MANAGER_DLL
		#define MODULE_MANAGER_CLASS _declspec(dllexport)
	#else
		#define MODULE_MANAGER_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManager.dll")			//��� DLL ����
#else
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManagerD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//ģ����Ϣ
struct tagGameModuleInfo
{
	//�汾��Ϣ
	DWORD							dwClientVersion;					//��Ϸ�汾
	DWORD							dwServerVersion;					//����汾
	DWORD							dwNativeVersion;					//���ذ汾

	//��������
	WORD							wGameID;							//ģ���ʶ
	TCHAR							szDataBaseAddr[15];					//���ݿ���
	TCHAR							szDataBaseName[32];					//���ݿ���

	//ģ������
	TCHAR							szGameName[LEN_KIND];				//��Ϸ����
	TCHAR							szServerDLLName[LEN_PROCESS];		//��������
	TCHAR							szClientEXEName[LEN_PROCESS];		//��������
};

//������Ϣ
struct tagGameServerInfo
{
	//��������
	WORD							wGameID;							//ģ���ʶ
	WORD							wServerID;							//�������

	//�ҽ�����
	WORD							wKindID;							//�ҽ�����
	WORD							wNodeID;							//�ҽӽڵ�
	WORD							wSortID;							//���б�ʶ

	//�汾��Ϣ
	DWORD							dwClientVersion;					//��Ϸ�汾
	DWORD							dwServerVersion;					//����汾
	DWORD							dwNativeVersion;					//���ذ汾

	//ģ������
	TCHAR							szGameName[LEN_KIND];				//��Ϸ����
	TCHAR							szServerDLLName[LEN_PROCESS];		//��������
	TCHAR							szClientEXEName[LEN_PROCESS];		//��������

	//˰������
	SCORE							lCellScore;							//��λ����
	WORD							wRevenueRatio;						//˰�ձ���
	SCORE							lServiceScore;						//�������

	//��������
	SCORE							lRestrictScore;						//���ƻ���
	SCORE							lMinTableScore;						//��ͻ���
	SCORE							lMinEnterScore;						//��ͻ���
	SCORE							lMaxEnterScore;						//��߻���

	//��Ա����
	BYTE							cbMinEnterMember;					//��ͻ�Ա
	BYTE							cbMaxEnterMember;					//��߻�Ա

	//��������
	DWORD							dwServerRule;						//�������
	DWORD							dwAttachUserRight;					//����Ȩ��

	//��������
	WORD							wMaxPlayer;							//�����Ŀ
	WORD							wTableCount;						//������Ŀ
	WORD							wServerPort;						//����˿�
	WORD                            wServerKind;                        //������� 
	WORD							wServerType;						//��������
	WORD							wServerLevel;						//����ȼ�
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR                           szServerPasswd[LEN_PASSWORD];       //�������� 

	//��������
	BYTE							cbDistributeRule;					//�������
	WORD							wMinDistributeUser;					//��������
	WORD							wDistributeTimeSpace;				//������
	WORD							wDistributeDrawCount;				//�������
	WORD							wMinPartakeGameUser;				//��������
	WORD							wMaxPartakeGameUser;				//�������


	//��������
	TCHAR							szDataBaseName[32];					//���ݿ���
	TCHAR							szDataBaseAddr[32];					//��ַ��Ϣ

	//��������
	BYTE							cbCustomRule[1024];					//�Զ�����
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//��������
};


//������Ϣ
struct tagCreateMatchInfo
{
	//��ʶ��Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������

	//������Ϣ	
	BYTE							cbMatchType;						//��������
	TCHAR							szMatchName[32];					//��������
	TCHAR							szMatchDate[64];					//����ʱ��

	//������Ϣ
	BYTE							cbFeeType;							//��������
	BYTE							cbDeductArea;						//�ɷ�����
	LONGLONG						lSignupFee;							//��������	
	BYTE							cbSignupMode;						//������ʽ
	BYTE							cbJoinCondition;					//��������
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	DWORD							dwExperience;						//��Ҿ���
	DWORD							dwFromMatchID;						//������ʶ	
	BYTE							cbFilterType;						//ɸѡ��ʽ
	WORD							wMaxRankID;							//�������
	SYSTEMTIME						MatchEndDate;						//��������
	SYSTEMTIME						MatchStartDate;						//��ʼ����

	//������ʽ
	BYTE							cbRankingMode;						//������ʽ	
	WORD							wCountInnings;						//ͳ�ƾ���
	BYTE							cbFilterGradesMode;					//ɸѡ��ʽ

	//��������
	BYTE							cbDistributeRule;					//��������
	WORD							wMinDistributeUser;					//��������
	WORD							wDistributeTimeSpace;				//������	
	WORD							wMinPartakeGameUser;				//��������
	WORD							wMaxPartakeGameUser;				//�������

	//������Ϣ
	TCHAR							szRewardGold[512];					//��ҽ���
	TCHAR							szRewardIngot[512];					//Ԫ������
	TCHAR							szRewardExperience[512];			//���齱��

	//��չ����
	BYTE							cbMatchRule[512];					//��������
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//��������
};

//������Ϣ
struct tagGameServerCreate
{
	//��������
	WORD							wGameID;							//ģ���ʶ
	WORD							wServerID;							//�������

	//�ҽ�����
	WORD							wKindID;							//�ҽ�����
	WORD							wNodeID;							//�ҽӽڵ�
	WORD							wSortID;							//���б�ʶ

	//˰������
	SCORE							lCellScore;							//��λ����
	WORD							wRevenueRatio;						//˰�ձ���
	SCORE							lServiceScore;						//�������

	//��������
	SCORE							lRestrictScore;						//���ƻ���
	SCORE							lMinTableScore;						//��ͻ���
	SCORE							lMinEnterScore;						//��ͻ���
	SCORE							lMaxEnterScore;						//��߻���

	//��Ա����
	BYTE							cbMinEnterMember;					//��ͻ�Ա
	BYTE							cbMaxEnterMember;					//��߻�Ա

	//��������
	DWORD							dwServerRule;						//�������
	DWORD							dwAttachUserRight;					//����Ȩ��

	//��������
	WORD							wMaxPlayer;							//�����Ŀ
	WORD							wTableCount;						//������Ŀ
	WORD							wServerPort;						//����˿�
	WORD                            wServerKind;                        //������� 
	WORD							wServerType;						//��������
	WORD							wServerLevel;						//����ȼ�
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR                           szServerPasswd[LEN_PASSWORD];       //�������� 

	//��������
	BYTE							cbDistributeRule;					//�������
	WORD							wMinDistributeUser;					//��������	
	WORD							wDistributeTimeSpace;				//������
	WORD							wDistributeDrawCount;				//�������
	WORD							wMinPartakeGameUser;				//��������
	WORD							wMaxPartakeGameUser;				//�������

	//��������
	TCHAR							szDataBaseName[32];					//���ݿ���
	TCHAR							szDataBaseAddr[32];					//��ַ��Ϣ

	//��������
	BYTE							cbCustomRule[1024];					//�Զ�����
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//��������
};

//////////////////////////////////////////////////////////////////////////////////

//���ò���
struct tagModuleInitParameter
{
	tagGameServiceAttrib			GameServiceAttrib;					//��������
	tagGameServiceOption			GameServiceOption;					//��������
	tagGameMatchOption				GameMatchOption;						//������Ϣ
	tagPersonalRoomOption		PersonalRoomOption;						//˽�˷���������
	tagPersonalRoomOptionRightAndFee  m_pCreateRoomRightAndFee;//Ȩ�����������

};

//////////////////////////////////////////////////////////////////////////////////

//�����ļ�
#ifndef MODULE_MANAGER_DLL 
	
	#include "ListControl.h"
	#include "DlgServerItem.h"
	#include "DlgServerMatch.h"
	#include "DlgServerWizard.h"
	
	#include "ModuleListControl.h"
	#include "ModuleDBParameter.h"
	#include "ModuleInfoManager.h"

	#include "ServerCustomRule.h"
	#include "ServerListControl.h"
	#include "ServerInfoManager.h"

#endif

//////////////////////////////////////////////////////////////////////////////////

#endif