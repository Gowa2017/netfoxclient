#ifndef DEFINE_HEAD_FILE
#define DEFINE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//��ֵ����

//ͷ���С
#define FACE_CX						48									//ͷ����
#define FACE_CY						48									//ͷ��߶�

#define BIGFACE_CX					96									//ͷ����
#define BIGFACE_CY					96									//ͷ��߶�

//���ȶ���
#define LEN_LESS_ACCOUNTS			6									//����ʺ�
#define LEN_LESS_NICKNAME			6									//����ǳ�
#define LEN_LESS_PASSWORD			6									//�������
#define LEN_LESS_COMPELLATION		4									//��С����

//��������
#define MAX_CHAIR					200									//�������
#define MAX_TABLE					512									//�������
#define MAX_COLUMN					32									//����б�
#define MAX_BATCH					48									//�������
#define MAX_ANDROID					2048								//������
#define MAX_PROPERTY				128									//������
#define MAX_WHISPER_USER			16									//���˽��
#define MAX_BUFF					10									//���Buff

//�б���
#define MAX_KIND					256									//�������
#define MAX_SERVER					1024								//��󷿼�

//��������
#define INVALID_CHAIR				0xFFFF								//��Ч����
#define INVALID_TABLE				0xFFFF								//��Ч����
#define INVALID_SERVER				0xFFFF								//��Ч����
#define INVALID_KIND				0xFFFF								//��Ч��Ϸ

//˰�ն���
#define REVENUE_BENCHMARK			0L								    //˰�����
#define REVENUE_DENOMINATOR			1000L								//˰�շ�ĸ
#define PERSONAL_ROOM_CHAIR			8									//˽�˷������������ӵ������Ŀ
//////////////////////////////////////////////////////////////////////////////////
//ϵͳ����
#define	MAX_ACCOUNTS				5									//����˺�

//��������
#define SCORE						DOUBLE								//��������
#define SCORE_STRING				TEXT("%.2lf")						//��������

//��Ϸ״̬
#define GAME_STATUS_FREE			0									//����״̬
#define GAME_STATUS_PLAY			100									//��Ϸ״̬
#define GAME_STATUS_WAIT			200									//�ȴ�״̬

//ϵͳ����
#define LEN_USER_CHAT				128									//���쳤��
#define TIME_USER_CHAT				1L									//������
#define TRUMPET_MAX_CHAR            128									//���ȳ���

//////////////////////////////////////////////////////////////////////////////////
//��������

//�б�����
#define PRIME_TYPE					11L									//������Ŀ
#define PRIME_KIND					53L									//������Ŀ
#define PRIME_NODE					101L								//�ڵ���Ŀ
#define PRIME_PAGE					53L									//�Զ���Ŀ
#define PRIME_SERVER				1009L								//������Ŀ

//��������
#define PRIME_SERVER_USER			503L								//��������
#define PRIME_ANDROID_USER			503L								//��������
#define PRIME_PLATFORM_USER			100003L								//ƽ̨����

//��������
#define PRIME_ANDROID_PARAMETER		65									//������Ŀ

//////////////////////////////////////////////////////////////////////////////////
//���ݳ���

//��������
#define LEN_MD5						33									//��������
#define LEN_USERNOTE				32									//��ע����
#define LEN_ACCOUNTS				32									//�ʺų���
#define LEN_NICKNAME				32									//�ǳƳ���
#define LEN_PASSWORD				33									//���볤��
#define LEN_GROUP_NAME				32									//��������
#define LEN_UNDER_WRITE				32									//����ǩ��
#define LEN_REMARKS					32									//��ע��Ϣ
#define LEN_DATETIME				20									//���ڳ���
#define  ROOM_ID_LEN 7
//���ݳ���
#define LEN_QQ						16									//Q Q ����
#define LEN_IP						32									//IP����
#define LEN_EMAIL					33									//�����ʼ�
#define LEN_USER_NOTE				256									//�û���ע
#define LEN_SEAT_PHONE				33									//�̶��绰
#define LEN_MOBILE_PHONE			12									//�ƶ��绰
#define LEN_PASS_PORT_ID			19									//֤������
#define LEN_COMPELLATION			16									//��ʵ����
#define LEN_DWELLING_PLACE			128									//��ϵ��ַ
#define LEN_USER_UIN				33									//UIN����
#define LEN_WEEK					7									//���ڳ���
#define LEN_TASK_NAME				64									//��������
#define LEN_TRANS_REMARK			32									//ת�˱�ע	
#define LEN_VERIFY_CODE				8									//��֤����
#define LEN_POSTALCODE				8									//��������
#define LEN_BIRTHDAY				16									//�û�����
#define LEN_BLOODTYPE				6									//�û�Ѫ��
#define LEN_CONSTELLATION			6									//�û�����
#define LEN_PHONE_MODE				21									//�ֻ��ͺ�
#define LEN_REAL_NAME				32									//��ʵ����
#define LEN_BANK_NO					32									//���п���
#define LEN_BANK_NAME				32									//��������
#define LEN_BANK_ADDRESS 		    128									//���п�����ַ
#define LEN_AGENT_ID				20									//����ų���
#define LEN_DESC					128									//��������
#define LEN_IMAGE_NAME				32									//�ļ�������
#define LEN_ALIPAY					32									//֧�����˺�
#define LEN_WECHAT					16									//΢���˺ų���
//������ʶ
#define LEN_NETWORK_ID				13									//��������
#define LEN_MACHINE_ID				33									//���г���
#define LEN_WHERE_PARM				32									//��������

//�б�����
#define LEN_TYPE					32									//���೤��
#define LEN_KIND					32									//���ͳ���
#define LEN_NODE					32									//�ڵ㳤��
#define LEN_PAGE					32									//���Ƴ���
#define LEN_SERVER					32									//���䳤��
#define LEN_PROCESS					32									//���̳���
#define LEN_ORDER_ID				64									//������

//////////////////////////////////////////////////////////////////////////
//���Ѷ���
#define MAX_FRIEND_COUNT			1000								//������
#define MIN_FRIEND_CONTENT			20									//��С����	
#define MAX_FRIEND_TRANSFER			5									//�����	

#define FRIEND_OPERATE_REMOVE		1									//�Ƴ�����
#define FRIEND_OPERATE_MDGROUPID	2									//�޸����

//��¼����
#define LOGON_COMMAND_USERINFO      0x0001								//�û���Ϣ
#define LOGON_COMMAND_GROUPINFO		0x0002								//������Ϣ
#define LOGON_COMMAND_FACE			0x0004								//�û�ͷ��
#define LOGON_COMMAND_FRIENDS       0x0008								//�û�����
#define LOGON_COMMAND_REMARKS		0x0010								//��ע��Ϣ
#define LOGON_TIME_SPACE			1000								//ʱ����

//���鶨��
#define GROUP_MAX_COUNT				32									//������
#define GROUP_LEN_NAME				32									//��������	
#define GROUP_LEN_INFO				1021								//��Ϣ����

#define GROUP_OPERATE_ADD			1									//��ӷ���
#define GROUP_OPERATE_MODIFY		2									//�༭����
#define GROUP_OPERATE_REMOVE		3									//�Ƴ�����


//��������
#define FRIEND_INVALID_SERVERID			WORD(0xffff)						//��Ч����
#define FRIEND_INVALID_TABLEID			WORD(0xffff)						//��Ч����
#define FRIEND_INVALID_CHAIR			WORD(0xffff)						//��Ч����

//�û�״̬
#define FRIEND_US_OFFLINE					0x00								//����״̬
#define FRIEND_US_ONLINE					0x01								//����״̬
#define FRIEND_US_LEAVE						0x02								//�뿪״̬
#define FRIEND_US_BUSY						0x04								//æµ״̬
#define FRIEND_US_UNDISTRUB					0x08								//�����״̬
#define FRIEND_US_CORBET					0x10								//����״̬


//��������
#define LEN_FONT_NAME				16									//��������
#define LEN_MESSAGE_CONTENT			512									//��Ϣ����		
#define MAX_EXPRESSION_COUNT		100									//������	

//////////////////////////////////////////////////////////////////////////////////

//�û���ϵ
#define	CP_NORMAL					0									//δ֪��ϵ
#define	CP_FRIEND					1									//���ѹ�ϵ
#define	CP_DETEST					2									//����ϵ
#define CP_SHIELD					3									//��������

//////////////////////////////////////////////////////////////////////////////////

//�Ա���
#define GENDER_FEMALE				0									//Ů���Ա�
#define GENDER_MANKIND				1									//�����Ա�

//////////////////////////////////////////////////////////////////////////////////

//��Ϸģʽ
#define GAME_GENRE_GOLD				0x0001								//�������
#define GAME_GENRE_SCORE			0x0002								//��ֵ����
#define GAME_GENRE_MATCH			0x0004								//��������
#define GAME_GENRE_EDUCATE			0x0008								//ѵ������
#define GAME_GENRE_PERSONAL			0x0010								//Լս����

//��������
#define SERVER_GENRE_NORMAL         0x0001                              //��ͨ����
#define SERVER_GENRE_PASSWD         0x0002                              //��������

//����ģʽ
#define SCORE_GENRE_NORMAL			0x0100								//��ͨģʽ
#define SCORE_GENRE_POSITIVE		0x0200								//�Ǹ�ģʽ

//////////////////////////////////////////////////////////////////////////////////

//��������
#define TASK_TYPE_WIN_INNINGS		0x01								//Ӯ������
#define TASK_TYPE_SUM_INNINGS		0x02								//�ܾ�����
#define TASK_TYPE_FIRST_WIN			0x04								//��ʤ����
#define TASK_TYPE_KEEP_WIN			0x08								//��Ӯ����

//����״̬
#define TASK_STATUS_UNFINISH		0									//����״̬
#define TASK_STATUS_SUCCESS			1									//����ɹ�
#define TASK_STATUS_FAILED			2									//����ʧ��	
#define TASK_STATUS_REWARD			3									//��ȡ����

//��������
#define TASK_MAX_COUNT				128									//��������

//////////////////////////////////////////////////////////////////////////////////
//�齱����
#define MAX_LOTTERY					16									//��ཱ��

//��Ϸ����
#define LEN_GAME_DATA				1024								//��Ϸ����
#define LEN_GAME_ITEM				64									//��������
#define MAX_DATA_ITEM				16									//�������
	
//////////////////////////////////////////////////////////////////////////////////

//�û�״̬
#define US_NULL						0x00								//û��״̬
#define US_FREE						0x01								//վ��״̬
#define US_SIT						0x02								//����״̬
#define US_READY					0x03								//ͬ��״̬
#define US_LOOKON					0x04								//�Թ�״̬
#define US_PLAYING					0x05								//��Ϸ״̬
#define US_OFFLINE					0x06								//����״̬

//////////////////////////////////////////////////////////////////////////////////
//��������

//�����û�״̬
#define MUS_NULL					0x00								//û��״̬
#define MUS_OUT						0x01								//��̭״̬
#define MUS_SIGNUPED     			0x02								//����״̬
#define MUS_PLAYING					0x04								//��������

//����״̬
#define MS_FREE						0x00								//����״̬
#define MS_WAITPLAY					0x01								//�ȴ���ʼ
#define MS_MATCHING					0x02								//��������
#define MS_WAITEND					0x04								//�ȴ�����
#define MS_MATCHEND					0x08								//��������

//�ɷ�����
#define DEDUCT_AREA_SERVER			0									//����ɷ�
#define DEDUCT_AREA_WEBSITE			1									//��վ�ɷ�

//��������
#define FEE_TYPE_GOLD				0									//���ɽ��
#define FEE_TYPE_MEDAL				1									//���ɽ���	

//��������
#define MATCH_JOINCD_MEMBER_ORDER	0x01								//��Ա�ȼ�	
#define MATCH_JOINCD_EXPERIENCE		0x02								//����ȼ�

//ɸѡ��ʽ
#define FILTER_TYPE_SINGLE_TURN		0									//��������
#define FILTER_TYPE_TOTAL_RANKING	1									//��������

//����ģʽ
#define SIGNUP_MODE_SIGNUP_FEE		0x01								//����ģʽ
#define SIGNUP_MODE_MATCH_USER		0x02								//����ģʽ

//������ʽ
#define RANKING_MODE_TOTAL_GRADES   0									//������ʽ
#define RANKING_MODE_SPECIAL_GRADES 1									//������ʽ 

//ɸѡ�ɼ�
#define FILTER_GRADES_MODE_BEST		0									//���ųɼ�
#define FILTER_GRADES_MODE_AVERAGE	1									//ƽ���ɼ�
#define FILTER_GRADES_MODE_AVERAGEEX 2									//ƽ���ɼ�

//����ģʽ
#define MATCH_MODE_NORMAL			0x00								//���濪��
#define MATCH_MODE_ROUND			0x01								//ѭ������

//��������
#define MATCH_TYPE_LOCKTIME			0x00								//��ʱ����
#define MATCH_TYPE_IMMEDIATE		0x01								//��ʱ����

//////////////////////////////////////////////////////////////////////////////////

//�������
#define SRL_LOOKON					0x00000001							//�Թ۱�־
#define SRL_OFFLINE					0x00000002							//���߱�־
#define SRL_SAME_IP					0x00000004							//ͬ����־

//�������
#define SRL_ROOM_CHAT				0x00000100							//�����־
#define SRL_GAME_CHAT				0x00000200							//�����־
#define SRL_WISPER_CHAT				0x00000400							//˽�ı�־
#define SRL_HIDE_USER_INFO			0x00000800							//���ر�־

//////////////////////////////////////////////////////////////////////////////////
//�б�����

//��Ч����
#define UD_NULL						0									//��Ч����
#define UD_IMAGE					100									//ͼ������
#define UD_CUSTOM					200									//�Զ�����

//��������
#define UD_GAME_ID					1									//��Ϸ��ʶ
#define UD_USER_ID					2									//�û���ʶ
#define	UD_NICKNAME					3									//�û��ǳ�

//��չ����
#define UD_GENDER					10									//�û��Ա�
#define UD_GROUP_NAME				11									//��������
#define UD_UNDER_WRITE				12									//����ǩ��

//״̬��Ϣ
#define UD_TABLE					20									//��Ϸ����
#define UD_CHAIR					21									//���Ӻ���

//������Ϣ
#define UD_SCORE					30									//�û�����
#define UD_GRADE					31									//�û��ɼ�
#define UD_USER_MEDAL				32									//�û�����
#define UD_EXPERIENCE				33									//�û�����
#define UD_LOVELINESS				34									//�û�����
#define UD_WIN_COUNT				35									//ʤ������
#define UD_LOST_COUNT				36									//�������
#define UD_DRAW_COUNT				37									//�;�����
#define UD_FLEE_COUNT				38									//�Ӿ�����
#define UD_PLAY_COUNT				39									//�ܾ�����

//���ֱ���
#define UD_WIN_RATE					40									//�û�ʤ��
#define UD_LOST_RATE				41									//�û�����
#define UD_DRAW_RATE				42									//�û�����
#define UD_FLEE_RATE				43									//�û�����
#define UD_GAME_LEVEL				44									//��Ϸ�ȼ�

//��չ��Ϣ
#define UD_NOTE_INFO				50									//�û���ע
#define UD_LOOKON_USER				51									//�Թ��û�

//ͼ���б�
#define UD_IMAGE_FLAG				(UD_IMAGE+1)						//�û���־
#define UD_IMAGE_GENDER				(UD_IMAGE+2)						//�û��Ա�
#define UD_IMAGE_STATUS				(UD_IMAGE+3)						//�û�״̬

//////////////////////////////////////////////////////////////////////////////////
//���ݿⶨ��

#define DB_ERROR 					-1  								//����ʧ��
#define DB_SUCCESS 					0  									//����ɹ�
#define DB_NEEDMB 					18 									//����ʧ��
#define DB_PASSPORT					19									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////
//���߱�ʾ
#define PT_USE_MARK_DOUBLE_SCORE    0x0001								//˫������
#define PT_USE_MARK_FOURE_SCORE     0x0002								//�ı�����
#define PT_USE_MARK_GUARDKICK_CARD  0x0010								//���ߵ���
#define PT_USE_MARK_POSSESS         0x0020								//������� 

#define MAX_PT_MARK                 4                                   //��ʶ��Ŀ

//��Ч��Χ
#define VALID_TIME_DOUBLE_SCORE     3600                                //��Чʱ��
#define VALID_TIME_FOUR_SCORE       3600                                //��Чʱ��
#define VALID_TIME_GUARDKICK_CARD   3600                                //����ʱ��
#define VALID_TIME_POSSESS          3600                                //����ʱ��
#define VALID_TIME_KICK_BY_MANAGER  3600                                //��Ϸʱ�� 

#define PROPERTY_LEN_NAME			32									//��������
#define PROPERTY_LEN_INFO			128									//������Ϣ
//////////////////////////////////////////////////////////////////////////////////
//�豸����
#define DEVICE_TYPE_PC              0x00                                //PC
#define DEVICE_TYPE_ANDROID         0x10                                //Android
#define DEVICE_TYPE_ANDROID_COCOS2D	0x11								//Android COCOS2D
#define DEVICE_TYPE_ANDROID_U3D		0x12								//Android U3D
#define DEVICE_TYPE_ITOUCH          0x20                                //iTouch
#define DEVICE_TYPE_IPHONE          0x30                                //iPhone
#define DEVICE_TYPE_IPHONE_COCOS2D	0x31								//Android COCOS2D
#define DEVICE_TYPE_IPHONE_U3D		0x32								//Android U3D
#define DEVICE_TYPE_IPAD            0x40                                //iPad
#define DEVICE_TYPE_IPAD_COCOS2D	0x41								//Android COCOS2D
#define DEVICE_TYPE_IPAD_U3D		0x42								//Android U3D

/////////////////////////////////////////////////////////////////////////////////
//�ֻ�����

//��ͼģʽ
#define	VIEW_MODE_ALL				0x0001								//ȫ������
#define	VIEW_MODE_PART				0x0002								//���ֿ���

//��Ϣģʽ
#define VIEW_INFO_LEVEL_1			0x0010								//������Ϣ
#define VIEW_INFO_LEVEL_2			0x0020								//������Ϣ
#define VIEW_INFO_LEVEL_3			0x0040								//������Ϣ
#define VIEW_INFO_LEVEL_4			0x0080								//������Ϣ

//��������
#define RECVICE_GAME_CHAT			0x0100								//��������
#define RECVICE_ROOM_CHAT			0x0200								//��������
#define RECVICE_ROOM_WHISPER		0x0400								//����˽��

//��Ϊ��ʶ
#define BEHAVIOR_LOGON_NORMAL       0x0000                              //��ͨ��¼
#define BEHAVIOR_LOGON_IMMEDIATELY  0x1000                              //������¼

/////////////////////////////////////////////////////////////////////////////////
//������
#define RESULT_ERROR 					-1  								//�������
#define RESULT_SUCCESS 					0  									//����ɹ�
#define RESULT_FAIL 					1  									//����ʧ��

/////////////////////////////////////////////////////////////////////////////////
//�仯ԭ��
#define SCORE_REASON_WRITE              0                                   //д�ֱ仯
#define SCORE_REASON_INSURE             1                                   //���б仯
#define SCORE_REASON_PROPERTY           2                                   //���߱仯
#define SCORE_REASON_MATCH_FEE          3                                   //��������
#define SCORE_REASON_MATCH_QUIT         4                                   //��������

/////////////////////////////////////////////////////////////////////////////////

//��¼����ʧ��ԭ��
#define LOGON_FAIL_SERVER_INVALIDATION  200   

/////////////////////////////////////////////////////////////////////////////////

//��������
#define ANDROID_SIMULATE				0x01								//�໥ģ��
#define ANDROID_PASSIVITY				0x02								//�������
#define ANDROID_INITIATIVE				0x04								//�������
#define ANDROID_THEJOINT				0x08								//����ģʽ
/////////////////////////////////////////////////////////////////////////////////
//��������

//�۷�����
#define MATCH_FEE_TYPE_GOLD				0x00								//�۷�����
#define MATCH_FEE_TYPE_MEDAL			0x01								//�۷�����

//��������
#define MATCH_TYPE_LOCKTIME				0x00								//��ʱ����
#define MATCH_TYPE_IMMEDIATE			0x01								//��ʱ����

////////////////////////////////////////////////////////////////////////////////

//��������
#define CONSUME_TYPE_GOLD				0x01								//��Ϸ��
#define CONSUME_TYPE_USEER_MADEL		0x02								//Ԫ��
#define CONSUME_TYPE_CASH				0x03								//��Ϸ��
#define CONSUME_TYPE_LOVELINESS			0x04								//����ֵ

//�ȼ�����
#define IDI_HOT_KEY_BOSS			0x0100								//�ϰ��ȼ�
#define IDI_HOT_KEY_WHISPER			0x0200								//˽���ȼ�

//ʧ�ܴ�����
#define FAILURE_TYPE_ADMIN_KICK_OUT		0x01								//������Ա�������
#define FAILURE_TYPE_SYSTEM_MAINTENANCE	0x02								//ϵͳά����
#define FAILURE_TYPE_VIP_LIMIT_MIN		0x03								//��Ա�ȼ�������ͽ����Ա����
#define FAILURE_TYPE_VIP_LIMIT_MAX		0x04								//��Ա�ȼ�������߽����Ա����
#define FAILURE_TYPE_ROOM_FULL			0x05								//���䱬��
#define FAILURE_TYPE_ROOM_PASSWORD_INCORRECT 0x06							//�����������
#define FAILURE_TYPE_ROOM_COIN_NOTENOUGH 0x07							//�����Ҳ������

//˽�˷����
const int RULE_LEN = 100;//������򳤶�
const int SPECIAL_INFO_LEN = 1000;//��Է������ʱ����ʱ��һЩ����Ҫ��
const int MAX_CREATE_COUNT = 32;
//�������
#define MAXT_VOICE_LENGTH				15000								//��������

#endif