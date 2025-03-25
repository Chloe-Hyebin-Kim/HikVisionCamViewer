// HikVisionCamViewerDlg.cpp: 구현 파일

#include "stdafx.h"
#include "HikVisionCamViewer.h"
#include "HikVisionCamViewerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About
#pragma region CAboutDlg

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CHikVisionCamViewerDlg dialog

#pragma endregion CAboutDlg




CHikVisionCamViewerDlg::CHikVisionCamViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CHikVisionCamViewerDlg::IDD, pParent)
	, m_pcMyCamera(NULL)
	, m_i32DeviceCombo(0)
	, m_bOpenDevice(FALSE)
	, m_bStartGrabbing(FALSE)
	, m_hGrabThread(NULL)
	, m_bThreadState(FALSE)
	, m_i32TriggerMode(MV_TRIGGER_MODE_OFF)
	, m_i32TriggerSource(MV_TRIGGER_SOURCE_SOFTWARE)
	, m_f64ExposureEdit(0)
	, m_f64GainEdit(0)
	, m_f64FrameRateEdit(0)
	, m_bSoftWareTriggerCheck(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHikVisionCamViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_BTN_CAMSEARCH, m_btnCameraSearch);
	//DDX_Control(pDX, IDC_BTN_CAMOPEN, m_btnCameraOpen);
	//DDX_Control(pDX, IDC_BTN_CAMCLOSE, m_btnCameraClose);

	//DDX_Control(pDX, IDC_BTN_CAMSTART, m_btnCameraStart);
	//DDX_Control(pDX, IDC_BTN_CAMPAUSE, m_btnCameraPause);
	//DDX_Control(pDX, IDC_BTN_CAMSTOP, m_btnCameraStop);

	//DDX_Control(pDX, IDC_RADIO_CONTINUS, m_btnContinusRadio);
	//DDX_Control(pDX, IDC_RADIO_TRIGGER, m_btnTriggerRadio);

	DDX_Control(pDX, IDC_DEVICE_COMBO, m_cbCameraList);
	DDX_CBIndex(pDX, IDC_DEVICE_COMBO, m_i32DeviceCombo);

	DDX_Text(pDX, IDC_EXPOSURE_EDIT, m_f64ExposureEdit);
	DDX_Text(pDX, IDC_GAIN_EDIT, m_f64GainEdit);
	DDX_Text(pDX, IDC_FRAME_RATE_EDIT, m_f64FrameRateEdit);

	DDX_Check(pDX, IDC_SOFTWARE_TRIGGER_CHECK, m_bSoftWareTriggerCheck);
}

BEGIN_MESSAGE_MAP(CHikVisionCamViewerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	// }}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_BTN_CAMSEARCH, &CHikVisionCamViewerDlg::OnBnClickedmCameraSearch)

	ON_BN_CLICKED(IDC_BTN_CAMOPEN, &CHikVisionCamViewerDlg::OnBnClickedCamOpen)
	ON_BN_CLICKED(IDC_BTN_CAMCLOSE, &CHikVisionCamViewerDlg::OnBnClickedCamClose)

	ON_BN_CLICKED(IDC_BTN_CAMSTART, &CHikVisionCamViewerDlg::OnBnClickedCamStart)	//ON_BN_CLICKED(IDC_START_GRABBING_BUTTON, &CBasicDemoDlg::OnBnClickedCamStart)
	ON_BN_CLICKED(IDC_BTN_CAMPAUSE, &CHikVisionCamViewerDlg::OnBnClickedCamPause)
	ON_BN_CLICKED(IDC_BTN_CAMSTOP, &CHikVisionCamViewerDlg::OnBnClickedCamStop)	//ON_BN_CLICKED(IDC_STOP_GRABBING_BUTTON, &CBasicDemoDlg::OnBnClickedCamStop)

	ON_BN_CLICKED(IDC_BTN_GET_PARAMETER, &CHikVisionCamViewerDlg::OnBnClickedGetParameterButton)
	ON_BN_CLICKED(IDC_BTN_SET_PARAMETER, &CHikVisionCamViewerDlg::OnBnClickedSetParameterButton)

	ON_BN_CLICKED(IDC_RADIO_CONTINUS, &CHikVisionCamViewerDlg::OnBnClickedContinusModeRadio)
	ON_BN_CLICKED(IDC_RADIO_TRIGGER, &CHikVisionCamViewerDlg::OnBnClickedTriggerModeRadio)
	ON_BN_CLICKED(IDC_SOFTWARE_TRIGGER_CHECK, &CHikVisionCamViewerDlg::OnBnClickedSoftwareTriggerCheck)
	ON_BN_CLICKED(IDC_SOFTWARE_ONCE_BUTTON, &CHikVisionCamViewerDlg::OnBnClickedSoftwareOnceButton)

	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CHikVisionCamViewerDlg message map handling



//Grabbing thread
unsigned int __stdcall GrabThread(void* pUser)
{
	if (pUser)
	{
		CHikVisionCamViewerDlg* pCam = (CHikVisionCamViewerDlg*)pUser;

		pCam->GrabThreadProcess();

		return 0;
	}

	return -1;
}


BOOL CHikVisionCamViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);

		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	DisplayWindowInitial();//Display Window Initialization

	InitializeCriticalSection(&m_hSaveImageMux);

	return TRUE; // return TRUE  unless you set the focus to a control
}

void CHikVisionCamViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CHikVisionCamViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CHikVisionCamViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHikVisionCamViewerDlg::ShowErrorMsg(CString csMessage, int i32ErrorNum)
{
	CString errorMsg;
	if (i32ErrorNum == 0)
	{
		errorMsg.Format(_T("%s"), csMessage);
	}
	else
	{
		errorMsg.Format(_T("%s: Error = %x: "), csMessage, i32ErrorNum);
	}

	switch (i32ErrorNum)
	{
	case MV_E_HANDLE:           errorMsg += "Error or invalid handle ";                                         break;
	case MV_E_SUPPORT:          errorMsg += "Not supported function ";                                          break;
	case MV_E_BUFOVER:          errorMsg += "Cache is full ";                                                   break;
	case MV_E_CALLORDER:        errorMsg += "Function calling order error ";                                    break;
	case MV_E_PARAMETER:        errorMsg += "Incorrect parameter ";                                             break;
	case MV_E_RESOURCE:         errorMsg += "Applying resource failed ";                                        break;
	case MV_E_NODATA:           errorMsg += "No data ";                                                         break;
	case MV_E_PRECONDITION:     errorMsg += "Precondition error, or running environment changed ";              break;
	case MV_E_VERSION:          errorMsg += "Version mismatches ";                                              break;
	case MV_E_NOENOUGH_BUF:     errorMsg += "Insufficient memory ";                                             break;
	case MV_E_ABNORMAL_IMAGE:   errorMsg += "Abnormal image, maybe incomplete image because of lost packet ";   break;
	case MV_E_UNKNOW:           errorMsg += "Unknown error ";                                                   break;
	case MV_E_GC_GENERIC:       errorMsg += "General error ";                                                   break;
	case MV_E_GC_ACCESS:        errorMsg += "Node accessing condition error ";                                  break;
	case MV_E_ACCESS_DENIED:	errorMsg += "No permission ";                                                   break;
	case MV_E_BUSY:             errorMsg += "Device is busy, or network disconnected ";                         break;
	case MV_E_NETER:            errorMsg += "Network error ";                                                   break;
	}

	MessageBox(errorMsg, TEXT("PROMPT"), MB_OK | MB_ICONWARNING);
}

void CHikVisionCamViewerDlg::EnableControls(bool bIsCameraReady)
{
	GetDlgItem(IDC_BTN_CAMOPEN)->EnableWindow(m_bOpenDevice ? false : (bIsCameraReady ? true : false));
	GetDlgItem(IDC_BTN_CAMCLOSE)->EnableWindow((m_bOpenDevice && bIsCameraReady) ? true : false);

	GetDlgItem(IDC_BTN_CAMSTART)->EnableWindow((m_bStartGrabbing && bIsCameraReady) ? false : (m_bOpenDevice ? true : false));
	GetDlgItem(IDC_BTN_CAMPAUSE)->EnableWindow((m_bStartGrabbing && bIsCameraReady) ? true : false);//CameraPause
	//GetDlgItem(IDC_BTN_CAMPAUSE)->EnableWindow((m_bOpenDevice && bIsCameraReady) ? true : false);	//CameraPause
	GetDlgItem(IDC_BTN_CAMSTOP)->EnableWindow(m_bStartGrabbing ? true : false);

	GetDlgItem(IDC_EXPOSURE_EDIT)->EnableWindow(m_bOpenDevice ? true : false);
	GetDlgItem(IDC_FRAME_RATE_EDIT)->EnableWindow(m_bOpenDevice ? true : false);
	GetDlgItem(IDC_GAIN_EDIT)->EnableWindow(m_bOpenDevice ? true : false);

	GetDlgItem(IDC_BTN_GET_PARAMETER)->EnableWindow(m_bOpenDevice ? true : false);
	GetDlgItem(IDC_BTN_SET_PARAMETER)->EnableWindow(m_bOpenDevice ? true : false);

	GetDlgItem(IDC_RADIO_CONTINUS)->EnableWindow(m_bOpenDevice ? true : false);
	GetDlgItem(IDC_RADIO_TRIGGER)->EnableWindow(m_bOpenDevice ? true : false);
	GetDlgItem(IDC_SOFTWARE_TRIGGER_CHECK)->EnableWindow(m_bOpenDevice ? true : false);
	GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow((m_bStartGrabbing && m_bSoftWareTriggerCheck && ((CButton*)GetDlgItem(IDC_RADIO_TRIGGER))->GetCheck()) ? TRUE : FALSE);

	//GetDlgItem(IDC_SAVE_BMP_BUTTON)->EnableWindow(m_bStartGrabbing ? true : false);
	//GetDlgItem(IDC_SAVE_TIFF_BUTTON)->EnableWindow(m_bStartGrabbing ? true : false);
	//GetDlgItem(IDC_SAVE_PNG_BUTTON)->EnableWindow(m_bStartGrabbing ? true : false);
	//GetDlgItem(IDC_SAVE_JPG_BUTTON)->EnableWindow(m_bStartGrabbing ? true : false);
}

void CHikVisionCamViewerDlg::DisplayWindowInitial()
{
	CWnd* pWnd = GetDlgItem(IDC_DISPLAY_STATIC);
	if (pWnd)
	{
		m_hwndDisplay = pWnd->GetSafeHwnd();//Display Handle
		if (m_hwndDisplay)
		{
			EnableControls(FALSE);
		}
	}
}

int CHikVisionCamViewerDlg::CloseDevice()
{
	m_bThreadState = FALSE;
	if (m_hGrabThread)
	{
		WaitForSingleObject(m_hGrabThread, INFINITE);
		CloseHandle(m_hGrabThread);
		m_hGrabThread = NULL;
	}

	if (m_pcMyCamera)
	{
		m_pcMyCamera->Close();
		delete m_pcMyCamera;
		m_pcMyCamera = NULL;
	}

	m_bStartGrabbing = false;
	m_bOpenDevice = false;

	if (m_pSaveImageBuf)
	{
		free(m_pSaveImageBuf);
		m_pSaveImageBuf = NULL;
	}
	m_ui32SaveImageBufSize = 0;

	return MV_OK;
}

bool CHikVisionCamViewerDlg::RemoveCustomPixelFormats(enum MvGvspPixelType enPixelFormat)
{
	int nResult = enPixelFormat & MV_GVSP_PIX_CUSTOM;
	if (MV_GVSP_PIX_CUSTOM == nResult)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CHikVisionCamViewerDlg::OnClose()
{
	PostQuitMessage(0);
	CloseDevice();

	DeleteCriticalSection(&m_hSaveImageMux);
	CDialog::OnClose();
}



int CHikVisionCamViewerDlg::GetTriggerMode()
{
	MVCC_ENUMVALUE stEnumValue = { 0 };

	int i32Ret = m_pcMyCamera->GetEnumValue("TriggerMode", &stEnumValue);
	if (MV_OK != i32Ret)
	{
		return i32Ret;
	}

	m_i32TriggerMode = stEnumValue.nCurValue;

	if (MV_TRIGGER_MODE_ON == m_i32TriggerMode)
	{
		OnBnClickedTriggerModeRadio();
		//ShowErrorMsg(TEXT("[MV_TRIGGER_MODE_ON] OnBnClickedTriggerModeRadio"), 0);
	}
	else
	{
		m_i32TriggerMode = MV_TRIGGER_MODE_OFF;
		OnBnClickedContinusModeRadio();
		//ShowErrorMsg(TEXT("[MV_TRIGGER_MODE_OFF] OnBnClickedContinusModeRadio"), 0);
	}

	return MV_OK;
}

int CHikVisionCamViewerDlg::SetTriggerMode()
{
	return m_pcMyCamera->SetEnumValue("TriggerMode", m_i32TriggerMode);
}

int CHikVisionCamViewerDlg::GetExposureTime()
{
	MVCC_FLOATVALUE stFloatValue = { 0 };

	int i32Ret = m_pcMyCamera->GetFloatValue("ExposureTime", &stFloatValue);
	if (MV_OK != i32Ret)
	{
		return i32Ret;
	}

	m_f64ExposureEdit = stFloatValue.fCurValue;

	return MV_OK;
}

int CHikVisionCamViewerDlg::SetExposureTime()
{
	//Adjust these two exposure mode to allow exposure time effective
	int i32Ret = m_pcMyCamera->SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
	if (MV_OK != i32Ret)
	{
		return i32Ret;
	}

	m_pcMyCamera->SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);

	return m_pcMyCamera->SetFloatValue("ExposureTime", (float)m_f64ExposureEdit);
}

int CHikVisionCamViewerDlg::GetGain()
{
	MVCC_FLOATVALUE stFloatValue = { 0 };

	int i32Ret = m_pcMyCamera->GetFloatValue("Gain", &stFloatValue);
	if (MV_OK != i32Ret)
	{
		return i32Ret;
	}
	m_f64GainEdit = stFloatValue.fCurValue;

	return MV_OK;
}

int CHikVisionCamViewerDlg::SetGain()
{
	// Set Gain after Auto Gain is turned off, this failure does not need to return
	m_pcMyCamera->SetEnumValue("GainAuto", 0);

	return m_pcMyCamera->SetFloatValue("Gain", (float)m_f64GainEdit);
}

int CHikVisionCamViewerDlg::GetFrameRate()
{
	MVCC_FLOATVALUE stFloatValue = { 0 };

	int i32Ret = m_pcMyCamera->GetFloatValue("ResultingFrameRate", &stFloatValue);
	if (MV_OK != i32Ret)
	{
		return i32Ret;
	}
	m_f64FrameRateEdit = stFloatValue.fCurValue;

	return MV_OK;
}

int CHikVisionCamViewerDlg::SetFrameRate()
{
	int i32Ret = m_pcMyCamera->SetBoolValue("AcquisitionFrameRateEnable", true);
	if (MV_OK != i32Ret)
	{
		return i32Ret;
	}

	return m_pcMyCamera->SetFloatValue("AcquisitionFrameRate", (float)m_f64FrameRateEdit);
}

int CHikVisionCamViewerDlg::GetTriggerSource()
{
	MVCC_ENUMVALUE stEnumValue = { 0 };

	int i32Ret = m_pcMyCamera->GetEnumValue("TriggerSource", &stEnumValue);
	if (MV_OK != i32Ret)
	{
		return i32Ret;
	}

	if ((unsigned int)MV_TRIGGER_SOURCE_SOFTWARE == stEnumValue.nCurValue)
	{
		m_bSoftWareTriggerCheck = TRUE;
	}
	else
	{
		m_bSoftWareTriggerCheck = FALSE;
	}

	return MV_OK;
}

int CHikVisionCamViewerDlg::SetTriggerSource()
{
	int i32Ret = MV_OK;
	if (m_bSoftWareTriggerCheck)
	{
		m_i32TriggerSource = MV_TRIGGER_SOURCE_SOFTWARE;
		i32Ret = m_pcMyCamera->SetEnumValue("TriggerSource", m_i32TriggerSource);
		if (MV_OK != i32Ret)
		{
			ShowErrorMsg(TEXT("Set Software Trigger FAIL!"), i32Ret);
			return i32Ret;
		}
		//GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(TRUE);
	}
	else
	{
		m_i32TriggerSource = MV_TRIGGER_SOURCE_LINE0;
		i32Ret = m_pcMyCamera->SetEnumValue("TriggerSource", m_i32TriggerSource);
		if (MV_OK != i32Ret)
		{
			ShowErrorMsg(TEXT("Set Hardware Trigger FAIL!"), i32Ret);
			return i32Ret;
		}
		//GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(FALSE);
	}

	return i32Ret;
}



int CHikVisionCamViewerDlg::GrabThreadProcess()
{
	MV_FRAME_OUT stImageInfo = { 0 };
	MV_DISPLAY_FRAME_INFO stDisplayInfo = { 0 };
	int i32Ret = MV_OK;

	while (m_bThreadState)
	{
		i32Ret = m_pcMyCamera->GetImageBuffer(&stImageInfo, 1000);
		if (i32Ret == MV_OK)
		{
			EnterCriticalSection(&m_hSaveImageMux);
			if (NULL == m_pSaveImageBuf || stImageInfo.stFrameInfo.nFrameLen > m_ui32SaveImageBufSize)
			{
				if (m_pSaveImageBuf)
				{
					free(m_pSaveImageBuf);
					m_pSaveImageBuf = NULL;
				}

				m_pSaveImageBuf = (unsigned char*)malloc(sizeof(unsigned char) * stImageInfo.stFrameInfo.nFrameLen);
				if (m_pSaveImageBuf == NULL)
				{
					LeaveCriticalSection(&m_hSaveImageMux);
					return 0;
				}
				m_ui32SaveImageBufSize = stImageInfo.stFrameInfo.nFrameLen;
			}
			memcpy(m_pSaveImageBuf, stImageInfo.pBufAddr, stImageInfo.stFrameInfo.nFrameLen);
			memcpy(&m_stImageInfo, &(stImageInfo.stFrameInfo), sizeof(MV_FRAME_OUT_INFO_EX));
			LeaveCriticalSection(&m_hSaveImageMux);

			if (RemoveCustomPixelFormats(stImageInfo.stFrameInfo.enPixelType))
			{
				m_pcMyCamera->FreeImageBuffer(&stImageInfo);
				continue;
			}

			stDisplayInfo.hWnd = m_hwndDisplay;
			stDisplayInfo.pData = stImageInfo.pBufAddr;
			stDisplayInfo.nDataLen = stImageInfo.stFrameInfo.nFrameLen;
			stDisplayInfo.nWidth = stImageInfo.stFrameInfo.nWidth;
			stDisplayInfo.nHeight = stImageInfo.stFrameInfo.nHeight;
			stDisplayInfo.enPixelType = stImageInfo.stFrameInfo.enPixelType;
			m_pcMyCamera->DisplayOneFrame(&stDisplayInfo);

			m_pcMyCamera->FreeImageBuffer(&stImageInfo);
		}
		else
		{
			if (MV_TRIGGER_MODE_ON == m_i32TriggerMode)
			{
				Sleep(5);
			}
		}
	}

	return MV_OK;
}


void CHikVisionCamViewerDlg::OnBnClickedmCameraSearch()
{
	CString strMsg;

	//Clear Device List Information
	m_cbCameraList.ResetContent();

	//Device Information List Initialization
	memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

	// Enumerate all devices within subnet
	int i32Ret = HikVisionCamera::EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);
	if (MV_OK != i32Ret)
	{
		return;
	}

	//Add value to the information list box and display
	for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++)
	{
		MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
		if (NULL == pDeviceInfo)
		{
			continue;
		}

		wchar_t* pUserName = NULL;
		if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE)
		{
			int nIp1 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
			int nIp2 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
			int nIp3 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
			int nIp4 = (m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

			if (strcmp("", (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName)) != 0)
			{
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
			}
			else
			{
				char strUserName[256] = { 0 };
				sprintf_s(strUserName, 256, "%s %s (%s)", pDeviceInfo->SpecialInfo.stGigEInfo.chManufacturerName, pDeviceInfo->SpecialInfo.stGigEInfo.chModelName, pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber);
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
			}

			strMsg.Format(_T("[%d]GigE:    %s  (%d.%d.%d.%d)"), i, pUserName, nIp1, nIp2, nIp3, nIp4);
		}
		else if (pDeviceInfo->nTLayerType == MV_USB_DEVICE)
		{
			if (strcmp("", (char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName) != 0)
			{
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
			}
			else
			{
				char strUserName[256] = { 0 };
				sprintf_s(strUserName, 256, "%s %s (%s)", pDeviceInfo->SpecialInfo.stUsb3VInfo.chManufacturerName, pDeviceInfo->SpecialInfo.stUsb3VInfo.chModelName, pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
				DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
				pUserName = new wchar_t[dwLenUserName];
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
			}

			strMsg.Format(_T("[%d]UsbV3:  %s"), i, pUserName);
		}
		else
		{
			ShowErrorMsg(TEXT("Unknown device enumerated"), 0);
		}

		m_cbCameraList.AddString(strMsg);

		if (pUserName)
		{
			delete[] pUserName;
			pUserName = NULL;
		}
	}

	if (0 == m_stDevList.nDeviceNum)
	{
		ShowErrorMsg(TEXT("No device"), 0);
		return;
	}

	m_cbCameraList.SetCurSel(0);
	EnableControls(true);//	GetDlgItem(IDC_BTN_CAMOPEN)->EnableWindow(TRUE); //m_btnCameraStart.EnableWindow(TRUE);
}

void CHikVisionCamViewerDlg::OnBnClickedCamOpen()
{
	if (true == m_bOpenDevice || NULL != m_pcMyCamera)
	{
		return;
	}

	UpdateData(TRUE);

	int nIndex = m_i32DeviceCombo;
	if ((nIndex < 0) | (nIndex >= MV_MAX_DEVICE_NUM))
	{
		ShowErrorMsg(TEXT("Please select device"), 0);
		return;
	}

	if (NULL == m_stDevList.pDeviceInfo[nIndex])
	{
		ShowErrorMsg(TEXT("Device does not exist"), 0);
		return;
	}

	m_pcMyCamera = new HikVisionCamera;
	if (NULL == m_pcMyCamera)
	{
		return;
	}

	int i32Ret = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[nIndex]);
	if (MV_OK != i32Ret)
	{
		delete m_pcMyCamera;
		m_pcMyCamera = NULL;
		ShowErrorMsg(TEXT("Open FAIL!"), i32Ret);
		return;
	}


	// Detection network optimal package size(It only works for the GigE camera)
	if (m_stDevList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
	{
		unsigned int nPacketSize = 0;
		i32Ret = m_pcMyCamera->GetOptimalPacketSize(&nPacketSize);
		if (i32Ret == MV_OK)
		{
			i32Ret = m_pcMyCamera->SetIntValue("GevSCPSPacketSize", nPacketSize);
			if (i32Ret != MV_OK)
			{
				ShowErrorMsg(TEXT("Warning: Set Packet Size FAIL!!"), i32Ret);
			}
		}
		else
		{
			ShowErrorMsg(TEXT("Warning: Get Packet Size FAIL!!"), i32Ret);
		}
	}

	m_bOpenDevice = true;
	EnableControls(true);

	OnBnClickedGetParameterButton();
}

void CHikVisionCamViewerDlg::OnBnClickedCamClose()
{
	CloseDevice();
	EnableControls(true);
}


void CHikVisionCamViewerDlg::OnBnClickedContinusModeRadio()
{
	((CButton*)GetDlgItem(IDC_RADIO_CONTINUS))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_TRIGGER))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SOFTWARE_TRIGGER_CHECK))->EnableWindow(FALSE);

	m_i32TriggerMode = MV_TRIGGER_MODE_OFF;
	int nRet = SetTriggerMode();
	if (MV_OK != nRet)
	{
		return;
	}
	GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(FALSE);
}

void CHikVisionCamViewerDlg::OnBnClickedTriggerModeRadio()
{
	UpdateData(TRUE);

	((CButton*)GetDlgItem(IDC_RADIO_CONTINUS))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_TRIGGER))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_SOFTWARE_TRIGGER_CHECK))->EnableWindow(TRUE);

	m_i32TriggerMode = MV_TRIGGER_MODE_ON;
	int nRet = SetTriggerMode();
	if (MV_OK != nRet)
	{
		ShowErrorMsg(TEXT("Set Trigger Mode Fail"), nRet);
		return;
	}

	if (m_bStartGrabbing == TRUE)
	{
		if (TRUE == m_bSoftWareTriggerCheck)
		{
			GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(TRUE);
		}
	}
}

void CHikVisionCamViewerDlg::OnBnClickedSoftwareTriggerCheck()
{
	UpdateData(TRUE);

	int nRet = SetTriggerSource();
	if (nRet != MV_OK)
	{
		return;
	}
}

void CHikVisionCamViewerDlg::OnBnClickedSoftwareOnceButton()
{
	if (TRUE != m_bStartGrabbing)
	{
		return;
	}

	m_pcMyCamera->CommandExecute("TriggerSoftware");
}

void CHikVisionCamViewerDlg::OnBnClickedCamStart()
{
	if (false == m_bOpenDevice)
	{
		AfxMessageBox(_T("The camera hasn't been opened yet."));
		return;
	}

	if (NULL == m_pcMyCamera)
	{
		AfxMessageBox(_T("The camera hasn't been find. Faaaailllll"));
		return;
	}

	if (true == m_bStartGrabbing)
	{
		AfxMessageBox(_T("The camera is already running."));
		return;
	}

	memset(&m_stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
	m_bThreadState = true;

	unsigned int ui32ThreadID = 0;
	m_hGrabThread = (void*)_beginthreadex(NULL, 0, GrabThread, this, 0, &ui32ThreadID);
	if (NULL == m_hGrabThread)
	{
		m_bThreadState = FALSE;
		ShowErrorMsg(TEXT("Create thread FAIL!"), 0);
		return;
	}

	int i32Ret = m_pcMyCamera->StartGrabbing();
	if (MV_OK != i32Ret)
	{
		m_bThreadState = FALSE;
		ShowErrorMsg(TEXT("Start grabbing FAIL!"), i32Ret);
		return;
	}

	m_bStartGrabbing = true;
	EnableControls(true);
}

void CHikVisionCamViewerDlg::OnBnClickedCamPause()
{
	AfxMessageBox(_T("Camera has been paused."));
	return;

	/*if (false == m_bCameraStarted)
	{
		AfxMessageBox(_T("Camera already has been paused."));
		return;
	}

	m_bCameraStarted = false;*/
}

void CHikVisionCamViewerDlg::OnBnClickedCamStop()
{
	if (false == m_bOpenDevice)
	{
		AfxMessageBox(_T("The camera has already been stopped."));
		return;
	}

	if (NULL == m_pcMyCamera)
	{
		AfxMessageBox(_T("The camera hasn't been found. Faaaailllll"));
		return;
	}

	if (false == m_bStartGrabbing)
	{
		AfxMessageBox(_T("The camera is already running."));
		return;
	}

	m_bThreadState = false;

	if (m_hGrabThread)
	{
		WaitForSingleObject(m_hGrabThread, INFINITE);
		CloseHandle(m_hGrabThread);
		m_hGrabThread = NULL;
	}

	int i32Ret = m_pcMyCamera->StopGrabbing();
	if (MV_OK != i32Ret)
	{
		ShowErrorMsg(TEXT("Stop grabbing FAIL!"), i32Ret);
		return;
	}

	m_bStartGrabbing = false;
	EnableControls(true);
}

void CHikVisionCamViewerDlg::OnBnClickedGetParameterButton()
{
	int i32Ret = GetTriggerMode();
	if (i32Ret != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Trigger Mode FAIL!"), i32Ret);
	}

	i32Ret = GetExposureTime();
	if (i32Ret != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Exposure Time FAIL!"), i32Ret);
	}

	i32Ret = GetGain();
	if (i32Ret != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Gain FAIL!"), i32Ret);
	}

	i32Ret = GetFrameRate();
	if (i32Ret != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Frame Rate FAIL!"), i32Ret);
	}

	i32Ret = GetTriggerSource();
	if (i32Ret != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Trigger Source FAIL!"), i32Ret);
	}

	UpdateData(FALSE);
}

void CHikVisionCamViewerDlg::OnBnClickedSetParameterButton()
{
	UpdateData(TRUE);

	bool bIsSetSucceed = true;

	int i32Ret = SetExposureTime();
	if (i32Ret != MV_OK)
	{
		bIsSetSucceed = false;
		ShowErrorMsg(TEXT("Set Exposure Time FAIL!"), i32Ret);
	}

	i32Ret = SetGain();
	if (i32Ret != MV_OK)
	{
		bIsSetSucceed = false;
		ShowErrorMsg(TEXT("Set Gain FAIL!"), i32Ret);
	}

	i32Ret = SetFrameRate();
	if (i32Ret != MV_OK)
	{
		bIsSetSucceed = false;
		ShowErrorMsg(TEXT("Set Frame Rate FAIL!"), i32Ret);
	}

	if (true == bIsSetSucceed)
	{
		ShowErrorMsg(TEXT("Set Parameter Succeed"), i32Ret);
	}
}