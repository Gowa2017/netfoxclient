
// Fish_ConfigDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Fish_Config.h"

class CMyEdit;
// CFish_ConfigDlg �Ի���
class CFish_ConfigDlg : public CDialogEx
{
	HICON						m_hIcon;

	int							m_ListRow;
	int							m_ListCol;
	CListCtrl*					m_pActiveList;
	CListCtrl					m_ListFish;							//�����б�
	CListCtrl					m_ListTeShuYu;						//�������
	CListCtrl					m_ListHBMD;							//�ڰ�����
	CListCtrl					m_UserWinLimit;						//Ӯ������
	CListCtrl					m_ListStockMin;						//�����Сֵ
	CListCtrl					m_ListStockMax;						//������ֵ
	CListCtrl					m_ListFangShui;						//��ˮ��¼
	CListCtrl                    m_iphaoma;              // IP��
	CListCtrl                    m_iphaoma1;             //ip��1

	CMyEdit						m_EditTest; 
	CMyEdit						m_yuyingdeqian; 
	CMyEdit						m_EditStockMin;						//�����Сֵ
	CMyEdit						m_EditStockMax;						//������ֵ
	CMyEdit						m_EditGameID;
	CMyEdit                     m_zhengtigailv;     //�������
	CMyEdit						m_EditBuHuoGaiLv;
	CMyEdit						m_EditBuHuoCount;
	CMyEdit						m_EditGameID2;
	CMyEdit						m_EditRevenuebfb;
	CMyEdit						m_EditRevenue;
	CMyEdit						m_EditExChangeUser;					//��Ҷһ�����
	CMyEdit						m_EditExChangeFish;					//��Ҷһ�����
	CMyEdit						m_EditExchangeCount;				//���ζһ�����
	CMyEdit						m_EditMinBullet;					//�ӵ���Сֵ
	CMyEdit						m_EditMaxBullet;					//�ӵ����ֵ
	CMyEdit						m_EditFangSHuiEdu;					//��ˮ���
	CMyEdit						m_EditFangShuiGaiLv;				//��ˮ����
	CMyEdit						m_EditFangShuiTime;					//��ˮʱ��
	CMyEdit						m_EDITPPG;                          //ÿ���ܶ�
	CMyEdit						m_Editfangshuixiaoyu;                //��ˮС��
	CMyEdit						m_Editfangshuidayu;					//��ˮ����
	CMyEdit                      m_bodongshijian;                 //����ʱ��
	CMyEdit                       m_bodongmax;               //���ֵ
	CMyEdit                     m_hard;                   //�����Ѷ�
	CMyEdit                     m_easy;                //�����׶� 
	CMyEdit                      K_qujianmax;      
	CMyEdit                    K_qujianmin;
	//CMyEdit                     m_yizihao1;           //�������Ӻ�1
	//CMyEdit                     m_yizihao2;             //
	CMyEdit                         m_youxiID1;          //��ϷID1
	CMyEdit                   m_youxiid2;           //��ϷID2
	CMyEdit                     m_zaixianrenshu;   //���������
	CMyEdit                    m_gl2;              //����2
	CMyEdit                   m_tiaozhengID;             //������ID��
	CMyEdit                     m_pao1;         //��1
	CMyEdit                     m_pao2;
	CMyEdit                     m_pao3;
	CMyEdit                     m_pao4;
	CMyEdit                     m_pao5;
	CMyEdit                     m_pao6;
	CMyEdit                     m_pao7;
	CMyEdit                     m_pao8;
	CMyEdit                     m_pao9;
	CMyEdit                     m_pao10;
	CMyEdit                     m_tichu;
	CMyEdit                     m_jiqiren;
	CMyEdit                    m_jiqiren2;
	CMyEdit                    m_jiqiren3;
	CMyEdit						m_maxleave;
	CMyEdit						m_minleave;
	CMyEdit						m_winPercentage;
	CMyEdit						m_losePercentage;
	CMyEdit                     m_currentkongzhi;
	CMyEdit						m_currentkongzhi1;
	CMyEdit						m_currentkongzhi2;
	CMyEdit						m_currentkongzhi3;
	CMyEdit						m_weizhi1;
	CMyEdit						m_weizhi2;
	CMyEdit						m_weizhi3;
	CMyEdit						m_weizhi4;
	CMyEdit						m_weizhi5;
	CMyEdit						m_weizhi6;
	CMyEdit						m_weizhi7;
	CMyEdit						m_weizhi8;
	CMyEdit                     m_ZhongWen;
	CMyEdit						m_ZhongWen1;

	CButton						m_ButtonFSStart;
	CButton						m_ButtonFSStop;
	CButton						m_buttonqidong;
	CStatic						m_StaticFangShui;					//�Զ���ˮ˵��
// ����
public:
	// �Ի�������
	enum { IDD = IDD_FISH_CONFIG_DIALOG };

	CFish_ConfigDlg(CWnd* pParent = NULL);	// ��׼���캯��

	bool PipeSendData(WORD wSubCmdID, VOID * pData, WORD wDataSize);
 
	bool PipeSendDataTren(VOID *Pdata, WORD wdatasize);

	bool PipeMsgEvent(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	void jiluxiaoxi();
	

// ʵ��
protected:
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	// DDX/DDV ֧��
	virtual void DoDataExchange(CDataExchange* pDX);	
	///��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	afx_msg void OnTimer(UINT nTimeID);

	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnBeginScrollList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedBtnAdd();

	afx_msg void OnBnClickedButtonquery();

	afx_msg void OnBnClickedButtonDelete();

	afx_msg void OnBnClickedBtnAdd2();

	afx_msg void OnBnClickedButtonquery2();

	afx_msg void OnBnClickedButtonDelete2();

	afx_msg void OnBnClickedBtnLoad();

	afx_msg void OnBnClickedBtnSave();

	afx_msg void OnBnClickedBtnFsStart();

	afx_msg void OnBnClickedBtnFsStop();

	afx_msg void OnBnClickedBtnAdd3();

	afx_msg void OnBnClickedButtonDelete3();
	DECLARE_MESSAGE_MAP()
	


public:
	afx_msg void OnEnChangeEditMaxBullet();
	afx_msg void OnBnClickedButtonqueidng();
	afx_msg void OnBnClickedButtonqueidng2();
	afx_msg void OnBnClickedButtonjiqiren1();
	afx_msg void OnBnClickedButtonjiqiren2();
	afx_msg void OnBnClickedButtonlikaiqueding();
	afx_msg void OnBnClickedBtnfuzhi();
	afx_msg void OnBnClickedBtnLoad2();
	afx_msg void OnBnClickedButtonxiugai();
	afx_msg void OnBnClickedBtnqiyong2();
	afx_msg void OnBnClickedBtnqiyong3();
	afx_msg void OnBnClickedBtnqujianqueding();
	afx_msg void OnBnClickedButtonqueidng3();
};
