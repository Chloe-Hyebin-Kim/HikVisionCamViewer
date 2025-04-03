// HikVisionCamViewerDlg.h:  header file
#pragma once

#include "HikVisionCamera.h"

#include "GzCamLinkHeader.h"

#define WM_TIMER_GRAB_INFO 1
#define MAX_DEVICE_NUM 4

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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
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

	afx_msg void OnBnClickedSaveJpgButton();
	afx_msg void OnBnClickedSavePngButton();

	afx_msg void OnClose();

private:
	/*** Log ***/
	void ShowErrorMsg(CString csMessage, int i32ErrorNum);
	void PrintMessage(const char* pszFormat, ...);
	bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);


	/*** Window initialization ***/
	void DisplayWindowInitial();
	void EnableControls(bool bIsCameraReady = true);

	int CloseDevice();

	/*** Parameters Get and Set ***/
	int GetTriggerMode();
	void SetTriggerMode();
	int GetExposureTime();
	void SetExposureTime();
	int GetGain();
	void SetGain();
	int GetFrameRate();
	void SetFrameRate();
	int GetTriggerSource();
	void SetTriggerSource();

	int SaveImage(MV_SAVE_IAMGE_TYPE enSaveImageType);

	bool RemoveCustomPixelFormats(enum MvGvspPixelType enPixelFormat);

public:
	int ThreadFunc(int nCurCameraIndex);

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
	CListBox m_ctrlListBoxInfo;

private:
	int m_i32ZoomInIndex;
	int m_i32SaveIndex;

	int m_i32DeviceCombo; //m_cbCameraList - Enumerated device

	bool m_bOpenDevice;
	bool m_bStartGrabbing;

	BOOL m_bSoftWareTriggerCheck;

	int m_i32TriggerMode;
	int m_i32TriggerSource;
	double m_f64ExposureEdit;
	double m_f64GainEdit;
	double m_f64FrameRateEdit;


	void* m_hGrabThreadArr[MAX_DEVICE_NUM]; //Grab thread handle

	MV_CC_DEVICE_INFO_LIST m_stDevList; //Device Information List ( Online Device Number,Support up to 256 devices)

	CRITICAL_SECTION m_hSaveImageMuxArr[MAX_DEVICE_NUM];
	MV_FRAME_OUT_INFO_EX m_stImageInfoArr[MAX_DEVICE_NUM]; //Output Frame Information
	unsigned char* m_pSaveImageBufArr[MAX_DEVICE_NUM];
	unsigned int m_ui32SaveImageBufSizeArr[MAX_DEVICE_NUM];

	BOOL m_bCamCheckArr[MAX_DEVICE_NUM];
	unsigned int m_ui32ValidCamNum;

public:
	int m_i32CurCameraIndex;
	HikVisionCamera* m_pcMyCameraArr[MAX_DEVICE_NUM]; // HikVisionCamera packed commonly used interface
	HWND m_hwndDisplayArr[MAX_DEVICE_NUM]; //Window display Handle
	CRect m_hwndRectArr[MAX_DEVICE_NUM];


	/////////////////////////////////////////////////////
	//LoadLibrary
	GzCamLinkPointer m_CamLinkPtr;
	HMODULE m_hCamLib;
};
