#pragma once


////////////////////////////////////////////////////////////////////////////////////////////

//���ýṹ
struct tagCustomRule
{
	LONGLONG									lStorageStartTable;			//���ӿ����ʼֵ
	LONGLONG									lStorageDeductRoom;			//ȫ��˥��ֵ
	LONGLONG									lStorageMax1Room;			//ȫ�ֿ��ⶥֵ1
	LONGLONG									lStorageMul1Room;			//ȫ��Ӯ�ְٷֱ�1
	LONGLONG									lStorageMax2Room;			//ȫ�ֿ��ⶥֵ2
	LONGLONG									lStorageMul2Room;			//ȫ��Ӯ�ְٷֱ�2
	WORD									wGameTwo;					//�ȱ�����
	WORD									wGameTwoDeduct;				//�ȱ�����
	WORD									wGameThree;					//С��������
	WORD									wGameThreeDeduct;			//С��������
	BYTE									cbBetCount;					//��ע����
	LONGLONG									lBetScore[9];				//��ע��С
	DWORD									TheFirstPass[5];			//С��Ϸ��һ�ط���
	DWORD									TheSecondPass[5];			//С��Ϸ��һ�ط���
	DWORD									TheThirdPass[5];			//С��Ϸ��һ�ط���
	DWORD									TheFourthPass[5];			//С��Ϸ��һ�ط���
};

class CDlgCustomRule : public CDialog
{
	DECLARE_MESSAGE_MAP()
	//��������
public:
	//���캯��
	CDlgCustomRule();
	//��������
	virtual ~CDlgCustomRule();

	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();
	//ȡ����Ϣ
	virtual VOID OnCancel();

	//���ܺ���
public:
	//���¿ؼ�
	bool FillDataToControl();
	//��������
	bool FillControlToData();

	//���ú���
public:
	//��ȡ����
	bool GetCustomRule(tagCustomRule & CustomRule);
	//��������
	bool SetCustomRule(tagCustomRule & CustomRule);

	//���ñ���
protected:
	tagCustomRule					m_CustomRule;						//���ýṹ
};
////////////////////////////////////////////////////////////////////////////////////////////