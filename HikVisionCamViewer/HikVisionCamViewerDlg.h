// HikVisionCamViewerDlg.h: 헤더 파일
#pragma once

#include "HikVisionCamera.h"

// CHikVisionCamViewerDlg 대화 상자
class CHikVisionCamViewerDlg : public CDialog
{
	// 생성입니다.
public:
	CHikVisionCamViewerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_HIKVISIONCAMVIEWER_DIALOG };

public:

private:
	void ShowErrorMsg(CString csMessage, int nErrorNum);

private:
	bool m_bCameraStarted;

	//CButton
	CButton m_btnCameraStart;	//IDC_BUTTON1
	CButton m_btnCameraPause;	//IDC_BUTTON2
	CButton m_btnCameraStop;	//IDC_BUTTON3

	CButton m_cbCameraSearch;	//IDC_BUTTON4
	CComboBox m_cbCameraList;	//IDC_DEVICE_COMBO

	//CComboBox
	//CComboBox m_cbDeviceCombo;
	int m_i32DeviceCombo;


private:
	MV_CC_DEVICE_INFO_LIST  m_stDevList;
	HikVisionCamera* m_pcMyCamera; // HikVisionCamera packed commonly used interface

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCamStart();				//IDC_BUTTON1
	afx_msg void OnBnClickedCamPause();				//IDC_BUTTON2
	afx_msg void OnBnClickedCamStop();				//IDC_BUTTON3
	afx_msg void OnBnClickedmCameraSearch();		//IDC_BUTTON4
};
