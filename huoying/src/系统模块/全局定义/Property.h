#ifndef PROPERTY_HEAD_FILE
#define PROPERTY_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//���з�Χ issue area
#define PT_ISSUE_AREA_PLATFORM		0x01								//�������ߣ���������ʹ�ã�
#define PT_ISSUE_AREA_SERVER		0x02								//������ߣ��ڷ������ʹ�ã�
#define PT_ISSUE_AREA_GAME			0x04								//��Ϸ���ߣ�������Ϸʱ����ʹ�ã�

//ʹ�÷�Χ service area
#define PT_SERVICE_AREA_MESELF		0x0001								//�Լ���Χ
#define PT_SERVICE_AREA_PLAYER		0x0002								//��ҷ�Χ
#define PT_SERVICE_AREA_LOOKON		0x0004								//�Թ۷�Χ

//���߹�������
enum PROP_KIND
{
	PROP_KIND_ALL=					0,		//����
	PROP_KIND_GIFT=					1,		//����
	PROP_KIND_GEM=					2,		//��ʯ
	PROP_KIND_DOOUBLE=				3,		//����˫��
	PROP_KIND_DEFENSE=				4,		//�����
	PROP_KIND_DEFENSE_TICK=			5,		//���߿�
	PROP_KIND_VIP=					6,		//vip��
	PROP_KIND_TRUMPET=				7,		//������
	PROP_KIND_TYPHON=				8,		//С����
	PROP_KIND_NEGATIVE_SCORE_CLEAR=	9,		//��������
	PROP_KIND_ESCAPE_CLEAR=			10,		//��������
	PROP_KIND_MENOY=				12,		//���
	PROP_KIND_LOVELINESS=			13		//����
};



//��������
#define PT_TYPE_ERROR               0                                   //�������� 
#define PT_TYPE_PROPERTY            1	                                //��������  
#define PT_TYPE_PRESENT             2                                   //��������

//���߶���
#define PROPERTY_ID_CAR				1									//��������
#define PROPERTY_ID_CLAP			2									//��������
#define PROPERTY_ID_KISS			3									//��������
#define PROPERTY_ID_BEER			4									//ơ������
#define PROPERTY_ID_CAKE			5									//��������
#define PROPERTY_ID_RING			6									//�������
#define PROPERTY_ID_SMOKE			7									//��������
#define PROPERTY_ID_VILLA			8									//��������
#define PROPERTY_ID_FLOWER			9									//�ʻ�����
#define PROPERTY_ID_BEAT			10									//��������
#define PROPERTY_ID_BOMB			11									//ը������
#define PROPERTY_ID_BRICK			12									//שͷ����
#define PROPERTY_ID_EGG				13									//��������


#define PROPERTY_ID_CRYSTAL			101									//ˮ��
#define PROPERTY_ID_AGATE			102									//���
#define PROPERTY_ID_BOULDER			103									//��ʯ
#define PROPERTY_ID_GEM				104									//��ʯ
#define PROPERTY_ID_DIAMOND			105									//��ʯ

#define PROPERTY_ID_DOUBLE_1HOUR	201									//1Сʱ˫�����ֿ�
#define PROPERTY_ID_DOUBLE_3HOUR	202									//3Сʱ˫�����ֿ�
#define PROPERTY_ID_DOUBLE_1DAY		203									//1��˫�����ֿ�
#define PROPERTY_ID_DOUBLE_1WEEK	204									//1��˫�����ֿ�
#define PROPERTY_ID_AMULET_1HOUR	205									//1Сʱ����
#define PROPERTY_ID_AMULET_3HOUR	206									//3Сʱ����
#define PROPERTY_ID_AMULET_1DAY		207									//1�커��
#define PROPERTY_ID_AMULET_1WEEK	208									//1�ܻ���
#define PROPERTY_ID_GUARDKICK_1HOUR 209									//1Сʱ���ߵ���
#define PROPERTY_ID_GUARDKICK_3HOUR 210									//3Сʱ���ߵ���
#define PROPERTY_ID_GUARDKICK_1DAY  211									//1����ߵ���
#define PROPERTY_ID_GUARDKICK_1WEEK 212									//1�ܷ��ߵ���

#define PROPERTY_ID_TYPHON			306									//���ȵ���
#define PROPERTY_ID_TRUMPET			307									//���ȵ���
#define PROPERTY_ID_SCORE_CLEAR     308									//��������
#define PROPERTY_ID_ESCAPE_CLEAR    309									//��������

#define PROPERTY_ID_TWO_CARD	    14				                    //���ֵ��� 
#define PROPERTY_ID_FOUR_CARD		15			                        //���ֵ���  
#define PROPERTY_ID_GUARDKICK_CARD  20									//���ߵ���
#define PROPERTY_ID_POSSESS			21									//�������
#define PROPERTY_ID_BLUERING_CARD	22									//�������
#define PROPERTY_ID_YELLOWRING_CARD	23									//�������
#define PROPERTY_ID_WHITERING_CARD	24  								//������� 
#define PROPERTY_ID_REDRING_CARD	25									//�������
#define PROPERTY_ID_VIPROOM_CARD	26									//VIP������

//////////////////////////////////////////////////////////////////////////////////


//������Ϣ
struct tagPropertyInfo
{
	//������Ϣ
	WORD							wIndex;								//���߱�ʶ
	WORD							wKind;								//��������
	WORD							wDiscount;							//��Ա�ۿ�
	WORD							wUseArea;							//������Χ
	WORD							wServiceArea;						//����Χ
	WORD							wRecommend;							//�Ƽ���ʶ
	WORD							wScoreMultiple;						//���ܱ���
	BYTE							cbSuportMobile;						//֧���ֻ�

	//���ۼ۸�
	SCORE							lPropertyGold;						//���߽��
	DOUBLE							dPropertyCash;						//���߼۸�
	SCORE							lPropertyUserMedal;					//���߽��
	SCORE							lPropertyLoveLiness;				//���߽��

	//��������
	SCORE							lSendLoveLiness;					//��������
	SCORE							lRecvLoveLiness;					//��������
	SCORE							lUseResultsGold;					//��ý��
	
	TCHAR							szName[PROPERTY_LEN_NAME];			//��������
	TCHAR							szRegulationsInfo[PROPERTY_LEN_INFO];//ʹ����Ϣ
};


//��������
struct tagPropertyAttrib
{
	WORD							wIndex;								//���߱�ʶ
	WORD                            wPropertyType;                      //��������
	WORD							wServiceArea;						//ʹ�÷�Χ
	TCHAR                           szMeasuringunit[8];					//������λ 
	TCHAR							szPropertyName[32];					//��������
	TCHAR							szRegulationsInfo[256];				//ʹ����Ϣ
};


//������������
struct tagBackpackProperty
{
	int								nCount;								//���߸���
	tagPropertyInfo					Property;							//��������
};

//����Buff״̬
struct tagPropertyBuff
{
	DWORD	dwKind;														//��������
	DWORD	dwScoreMultiple;											//������2�����ֿ���4�����ֿ���
	time_t	tUseTime;													//ʹ��ʱ�̵�ʱ��
	DWORD	UseResultsValidTime;										//����ʱ��
	TCHAR	szName[16];													//����
};

//���͵ĵ���
struct SendPresent
{
	DWORD							dwUserID;							//������
	DWORD							dwRecvUserID;						//���߸�˭
	DWORD							dwPropID;
	WORD							wPropCount;
	time_t							tSendTime;							//���͵�ʱ��
	TCHAR							szPropName[16];						//��������
};
//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif