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
	/*** Camera Init ***/
	afx_msg void OnBnClickedmCameraSearch();		//IDC_BTN_CAMSEARCH

	afx_msg void OnBnClickedCamOpen();				//IDC_BTN_CAMOPEN
	afx_msg void OnBnClickedCamClose();				//IDC_BTN_CAMCLOSE

	/*** Camera Control ***/
	afx_msg void OnBnClickedCamStart();				//IDC_BTN_CAMSTART
	afx_msg void OnBnClickedCamPause();				//IDC_BTN_CAMPAUSE
	afx_msg void OnBnClickedCamStop();				//IDC_BTN_CAMSTOP

	/*** Parameters Get and Set ***/
	afx_msg void OnBnClickedGetParameterButton();	// Get Parameter
	afx_msg void OnBnClickedSetParameterButton();	// Exit from upper right corner

	/*** Image Acquisition ***/
	afx_msg void OnBnClickedContinusModeRadio();
	afx_msg void OnBnClickedTriggerModeRadio();
	afx_msg void OnBnClickedSoftwareTriggerCheck();
	afx_msg void OnBnClickedSoftwareOnceButton();//Software Trigger Execute Once

	afx_msg void OnClose();

private:
	/*** Error Log ***/
	void ShowErrorMsg(CString csMessage, int i32ErrorNum);

	/*** Window initialization ***/
	void DisplayWindowInitial();
	void EnableControls(bool bIsCameraReady);

	int CloseDevice();

	/*** Parameters Get and Set ***/
	int SetTriggerMode();
	int GetTriggerMode();
	int GetExposureTime();
	int SetExposureTime();
	int GetGain();
	int SetGain();
	int GetFrameRate();
	int SetFrameRate();
	int GetTriggerSource();
	int SetTriggerSource();

	bool RemoveCustomPixelFormats(enum MvGvspPixelType enPixelFormat);

public:
	int GrabThreadProcess();

private:
	//CButton
	//CButton m_btnCameraOpen;	//IDC_BTN_CAMOPEN
	//CButton m_btnCameraClose;	//IDC_BTN_CAMCLOSE

	//CButton m_btnCameraStart;	//IDC_BTN_CAMSTART
	//CButton m_btnCameraPause;	//IDC_BTN_CAMPAUSE
	//CButton m_btnCameraStop;	//IDC_BTN_CAMSTOP

	//CButton m_btnContinusRadio;//IDC_RADIO_CONTINUS
	//CButton m_btnTriggerRadio;//IDC_RADIO_TRIGGER

	//Enumerated device
	//CButton m_btnCameraSearch;	//IDC_BTN_CAMSEARCH
	CComboBox m_cbCameraList;	//IDC_DEVICE_COMBO

private:
	int m_i32DeviceCombo; //m_cbCameraList - Enumerated device

	bool m_bOpenDevice;
	bool m_bStartGrabbing;
	bool m_bThreadState;

	int m_i32TriggerMode;
	int m_i32TriggerSource;
	double m_f64ExposureEdit;
	double m_f64GainEdit;
	double m_f64FrameRateEdit;

	BOOL m_bSoftWareTriggerCheck;

	HWND m_hwndDisplay; //Window display Handle

	HikVisionCamera* m_pcMyCamera; // HikVisionCamera packed commonly used interface
	MV_CC_DEVICE_INFO_LIST m_stDevList; //Device Information List ( Online Device Number,Support up to 256 devices)

	void* m_hGrabThread; //Grab thread handle

	CRITICAL_SECTION m_hSaveImageMux;
	MV_FRAME_OUT_INFO_EX m_stImageInfo;//Output Frame Information
	unsigned char* m_pSaveImageBuf;
	unsigned int m_ui32SaveImageBufSize;
};
