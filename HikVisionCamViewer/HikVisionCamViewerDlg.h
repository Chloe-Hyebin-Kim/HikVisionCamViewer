// HikVisionCamViewerDlg.h:  header file
#pragma once

#include "HikVisionCamera.h"

// CHikVisionCamViewerDlg dialog
class CHikVisionCamViewerDlg : public CDialog
{
	/*** Construction ***/
public:
	CHikVisionCamViewerDlg(CWnd* pParent = nullptr);	// Standard constructor

/*** Dialog Data ***/
	enum { IDD = IDD_HIKVISIONCAMVIEWER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

/*** Implementation ***/
protected:
	HICON m_hIcon;

protected:
	/*** Generated message map functions ***/
	virtual BOOL OnInitDialog();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

public:
	/*** Camera Control ***/
	afx_msg void OnBnClickedCamStart();				//IDC_BTN_CAMSTART
	afx_msg void OnBnClickedCamPause();				//IDC_BTN_CAMPAUSE
	afx_msg void OnBnClickedCamStop();				//IDC_BTN_CAMSTOP
	afx_msg void OnBnClickedmCameraSearch();		//IDC_BTN_CAMSEARCH


	/*** Parameters Get and Set ***/
	afx_msg void OnBnClickedGetParameterButton();	// Get Parameter
	afx_msg void OnBnClickedSetParameterButton();	// Exit from upper right corner

private:
	/*** Error Log ***/
	void ShowErrorMsg(CString csMessage, int nErrorNum);

	/*** Window initialization ***/
	void DisplayWindowInitial();
	void EnableControls(bool bIsCameraReady);

	//int CloseDevice();

	/*** Parameters Get and Set ***/
	int SetTriggerMode();							// Set Trigger Mode
	int GetTriggerMode();
	int GetExposureTime();							// Set Exposure Time
	int SetExposureTime();
	int GetGain();									// Set Gain
	int SetGain();
	int GetFrameRate();								// Set Frame Rate
	int SetFrameRate();
	int GetTriggerSource();							// Set Trigger Source
	int SetTriggerSource();

private:
	//CButton
	CButton m_btnCameraStart;	//IDC_BTN_CAMSTART
	CButton m_btnCameraPause;	//IDC_BTN_CAMPAUSE
	CButton m_btnCameraStop;	//IDC_BTN_CAMSTOP

	//Enumerated device
	CButton m_btnCameraSearch;	//IDC_BTN_CAMSEARCH
	CComboBox m_cbCameraList;	//IDC_DEVICE_COMBO

private:
	bool m_bCameraStarted;
	bool m_bOpenDevice;

	bool m_bSoftWareTriggerCheck;

	int m_i32DeviceCombo; //m_cbCameraList
	int m_i32TriggerMode;
	int m_i32TriggerSource;
	double m_f64ExposureEdit;
	double m_f64GainEdit;
	double m_f64FrameRateEdit;

private:
	HikVisionCamera* m_pcMyCamera; // HikVisionCamera packed commonly used interface

	HWND m_hwndDisplay;//Display Handle

	CRITICAL_SECTION m_hSaveImageMux;
	MV_CC_DEVICE_INFO_LIST m_stDevList;
};
