#ifndef CMD_HTTPSERVER_HEAD_FILE
#define CMD_HTTPSERVER_HEAD_FILE

#pragma pack(1)

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"

//////////////////////////////////////////////////////////////////////////////////

//��̨��ɢ����
#define  SUB_CS_C_CLEARUSER				1									//�������
//��ѯ������Ϣ
#define SUB_CS_C_HTTP_QUERY_ONLINE		2									//��ѯ�û�����
//��վ��ȡ������������
#define SUB_CS_C_HTTP_QUERY_DUMMY_ONLINE 3									//��ѯ��������
//��վ�޸���������ʱ����б��
#define SUB_CS_C_HTTP_CHANGE_DUMMY_ONLINE 5									//��վ�޸���������ʱ����б��
//ǿ���û�����
#define SUB_CS_C_HTTP_FORCE_OFFLINE		6									//ǿ���û�����

#pragma pack()

#endif