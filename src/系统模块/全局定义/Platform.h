#ifndef PLATFORM_HEAD_FILE
#define PLATFORM_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//�����ļ�

//�����ļ�
#include "Macro.h"
#include "Define.h"

//�ṹ�ļ�
#include "Struct.h"
#include "Packet.h"
#include "Property.h"

//ģ���ļ�
#include "Array.h"
#include "Module.h"
#include "PacketAide.h"
#include "ServerRule.h"
#include "RightDefine.h"

//ȫ�ֺ���
#include "GlobalFunction.h"

//////////////////////////////////////////////////////////////////////////////////

//����汾
#define VERSION_FRAME				PROCESS_VERSION(7,0,1)				//��ܰ汾
#define VERSION_PLAZA				PROCESS_VERSION(7,0,1)				//�����汾
#define VERSION_MOBILE_ANDROID		PROCESS_VERSION(7,0,1)				//�ֻ��汾
#define VERSION_MOBILE_IOS			PROCESS_VERSION(7,0,1)				//�ֻ��汾

//�汾����
#define VERSION_EFFICACY			0									//Ч��汾
#define VERSION_FRAME_SDK			INTERFACE_VERSION(7,1)				//��ܰ汾

//////////////////////////////////////////////////////////////////////////////////
//�����汾

#ifndef _DEBUG

//ƽ̨����
const TCHAR szProduct[]=TEXT("��������ƽ̨");							//��Ʒ����
const TCHAR szPlazaClass[]=TEXT("RYQPGamePlaza");						//�㳡����
const TCHAR szProductKey[]=TEXT("RYQPGamePlatform");					//��Ʒ����

//��ַ����
const TCHAR szCookieUrl[]=TEXT("http://103.26.78.231");					//��¼��ַ
const TCHAR szLogonServer[]=TEXT("103.26.78.231");						//��¼��ַ
const TCHAR szPlatformLink[]=TEXT("http://103.26.78.231");				//ƽ̨��վ
const TCHAR szValidateKey[]=TEXT("RYSyncLoginKey");						//��֤��Կ
const TCHAR szValidateLink[]=TEXT("SyncLogin.aspx?userid=%d&time=%d&signature=%s&url=/"); //��֤��ַ 

#else

//////////////////////////////////////////////////////////////////////////////////
//�ڲ�汾

//ƽ̨����
const TCHAR szProduct[]=TEXT("��������ƽ̨");							//��Ʒ����
const TCHAR szPlazaClass[]=TEXT("WHRYGamePlaza");						//�㳡����
const TCHAR szProductKey[]=TEXT("WHRYGamePlatform");					//��Ʒ����

//��ַ����
const TCHAR szCookieUrl[]=TEXT("http://103.26.78.231");					//��¼��ַ
const TCHAR szLogonServer[]=TEXT("103.26.78.231");						//��¼��ַ
const TCHAR szPlatformLink[]=TEXT("http://103.26.78.231");				//ƽ̨��վ
const TCHAR szValidateKey[]=TEXT("RYSyncLoginKey");						//��֤��Կ
const TCHAR szValidateLink[]=TEXT("SyncLogin.aspx?userid=%d&time=%d&signature=%s&url=/"); //��֤��ַ 

#endif

//////////////////////////////////////////////////////////////////////////////////

//���ݿ���
const TCHAR szPlatformDB[]=TEXT("RYPlatformDB");						//ƽ̨���ݿ�
const TCHAR szAccountsDB[]=TEXT("RYAccountsDB");						//�û����ݿ�
const TCHAR szTreasureDB[]=TEXT("RYTreasureDB");						//�Ƹ����ݿ�
const TCHAR szGameMatchDB[]=TEXT("RYGameMatchDB");						//�������ݿ�
const TCHAR szExerciseDB[]=TEXT("RYEducateDB"); 						//��ϰ���ݿ�
const TCHAR szGameScoreDB[]=TEXT("RYGameScoreDB");						//��ϰ���ݿ�

//////////////////////////////////////////////////////////////////////////////////

//��Ȩ��Ϣ
const TCHAR szCompilation[]=TEXT("B3D44854-9C2F-4C78-807F-8C08E940166D");

//////////////////////////////////////////////////////////////////////////////////

#endif