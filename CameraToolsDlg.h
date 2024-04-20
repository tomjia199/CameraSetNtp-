
// CameraToolsDlg.h: 头文件
//

#pragma once

#include "Utility.h"

//#define WM_DISPLAY_CHANGE1 WM_USER+101
#define WM_DISPLAY_CHANGE2 WM_USER+102

// CCameraToolsDlg 对话框
class CCameraToolsDlg : public CDialogEx
{
// 构造
public:
	CCameraToolsDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERATOOLS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedOpenfile();
	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSetntpButton();
};
