
// Fish_ConfigDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Fish_Config.h"

class CMyEdit;
// CFish_ConfigDlg 对话框
class CFish_ConfigDlg : public CDialogEx
{
	HICON						m_hIcon;

	int							m_ListRow;
	int							m_ListCol;
	CListCtrl*					m_pActiveList;
	CListCtrl					m_ListFish;							//鱼类列表
	CListCtrl					m_ListTeShuYu;						//特殊鱼表
	CListCtrl					m_ListHBMD;							//黑白名单
	CListCtrl					m_UserWinLimit;						//赢分限制
	CListCtrl					m_ListStockMin;						//库存最小值
	CListCtrl					m_ListStockMax;						//库存最大值
	CListCtrl					m_ListFangShui;						//放水记录
	CListCtrl                    m_iphaoma;              // IP号
	CListCtrl                    m_iphaoma1;             //ip号1

	CMyEdit						m_EditTest; 
	CMyEdit						m_yuyingdeqian; 
	CMyEdit						m_EditStockMin;						//库存最小值
	CMyEdit						m_EditStockMax;						//库存最大值
	CMyEdit						m_EditGameID;
	CMyEdit                     m_zhengtigailv;     //整体概率
	CMyEdit						m_EditBuHuoGaiLv;
	CMyEdit						m_EditBuHuoCount;
	CMyEdit						m_EditGameID2;
	CMyEdit						m_EditRevenuebfb;
	CMyEdit						m_EditRevenue;
	CMyEdit						m_EditExChangeUser;					//金币兑换比例
	CMyEdit						m_EditExChangeFish;					//鱼币兑换比例
	CMyEdit						m_EditExchangeCount;				//单次兑换数量
	CMyEdit						m_EditMinBullet;					//子弹最小值
	CMyEdit						m_EditMaxBullet;					//子弹最大值
	CMyEdit						m_EditFangSHuiEdu;					//放水额度
	CMyEdit						m_EditFangShuiGaiLv;				//放水概率
	CMyEdit						m_EditFangShuiTime;					//放水时间
	CMyEdit						m_EDITPPG;                          //每天总额
	CMyEdit						m_Editfangshuixiaoyu;                //放水小鱼
	CMyEdit						m_Editfangshuidayu;					//放水大鱼
	CMyEdit                      m_bodongshijian;                 //波动时间
	CMyEdit                       m_bodongmax;               //最大值
	CMyEdit                     m_hard;                   //波动难度
	CMyEdit                     m_easy;                //波动易度 
	CMyEdit                      K_qujianmax;      
	CMyEdit                    K_qujianmin;
	//CMyEdit                     m_yizihao1;           //波动椅子号1
	//CMyEdit                     m_yizihao2;             //
	CMyEdit                         m_youxiID1;          //游戏ID1
	CMyEdit                   m_youxiid2;           //游戏ID2
	CMyEdit                     m_zaixianrenshu;   //在线玩家数
	CMyEdit                    m_gl2;              //概率2
	CMyEdit                   m_tiaozhengID;             //调整的ID号
	CMyEdit                     m_pao1;         //炮1
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
	CStatic						m_StaticFangShui;					//自动放水说明
// 构造
public:
	// 对话框数据
	enum { IDD = IDD_FISH_CONFIG_DIALOG };

	CFish_ConfigDlg(CWnd* pParent = NULL);	// 标准构造函数

	bool PipeSendData(WORD wSubCmdID, VOID * pData, WORD wDataSize);
 
	bool PipeSendDataTren(VOID *Pdata, WORD wdatasize);

	bool PipeMsgEvent(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	void jiluxiaoxi();
	

// 实现
protected:
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	// DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);	
	///消息解释
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
