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
	, m_bSoftWareTriggerCheck(FALSE)
	, m_i32TriggerMode(MV_TRIGGER_MODE_OFF)
	, m_i32TriggerSource(MV_TRIGGER_SOURCE_SOFTWARE)
	, m_f64ExposureEdit(0)
	, m_f64GainEdit(0)
	, m_f64FrameRateEdit(0)
{
	m_bCameraStarted = false;
	m_bOpenDevice = false;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHikVisionCamViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BTN_CAMSTART, m_btnCameraStart);//BTNCAMERASTART
	DDX_Control(pDX, IDC_BTN_CAMPAUSE, m_btnCameraPause);
	DDX_Control(pDX, IDC_BTN_CAMSTOP, m_btnCameraStop);

	DDX_Control(pDX, IDC_BTN_CAMSEARCH, m_btnCameraSearch);
	DDX_Control(pDX, IDC_DEVICE_COMBO, m_cbCameraList);
	DDX_CBIndex(pDX, IDC_DEVICE_COMBO, m_i32DeviceCombo);
}

BEGIN_MESSAGE_MAP(CHikVisionCamViewerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	// }}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_BTN_CAMSTART, &CHikVisionCamViewerDlg::OnBnClickedCamStart)
	ON_BN_CLICKED(IDC_BTN_CAMPAUSE, &CHikVisionCamViewerDlg::OnBnClickedCamPause)
	ON_BN_CLICKED(IDC_BTN_CAMSTOP, &CHikVisionCamViewerDlg::OnBnClickedCamStop)

	ON_BN_CLICKED(IDC_BTN_CAMSEARCH, &CHikVisionCamViewerDlg::OnBnClickedmCameraSearch)

	ON_BN_CLICKED(IDC_BTN_GET_PARAMETER, &CHikVisionCamViewerDlg::OnBnClickedGetParameterButton)
	ON_BN_CLICKED(IDC_BTN_SET_PARAMETER, &CHikVisionCamViewerDlg::OnBnClickedSetParameterButton)



END_MESSAGE_MAP()

// CHikVisionCamViewerDlg 메시지 처리기

BOOL CHikVisionCamViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	DisplayWindowInitial();

	InitializeCriticalSection(&m_hSaveImageMux);

	m_bCameraStarted = false;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CHikVisionCamViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CHikVisionCamViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHikVisionCamViewerDlg::EnableControls(bool bIsCameraReady)
{
	GetDlgItem(IDC_BTN_CAMSTART)->EnableWindow(m_bOpenDevice ? FALSE : (bIsCameraReady ? TRUE : FALSE));		//CameraStart
	GetDlgItem(IDC_BTN_CAMPAUSE)->EnableWindow((m_bOpenDevice && bIsCameraReady) ? TRUE : FALSE);			//CameraPause
	GetDlgItem(IDC_BTN_CAMSTOP)->EnableWindow((m_bOpenDevice && bIsCameraReady) ? TRUE : FALSE);			//CameraStop

//GetDlgItem(IDC_START_GRABBING_BUTTON)->EnableWindow((m_bStartGrabbing && bIsCameraReady) ? FALSE : (m_bOpenDevice ? TRUE : FALSE));
//GetDlgItem(IDC_STOP_GRABBING_BUTTON)->EnableWindow(m_bStartGrabbing ? TRUE : FALSE);
//GetDlgItem(IDC_SOFTWARE_TRIGGER_CHECK)->EnableWindow(m_bOpenDevice ? TRUE : FALSE);
//GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow((m_bStartGrabbing && m_bSoftWareTriggerCheck && ((CButton*)GetDlgItem(IDC_TRIGGER_MODE_RADIO))->GetCheck()) ? TRUE : FALSE);
//GetDlgItem(IDC_SAVE_BMP_BUTTON)->EnableWindow(m_bStartGrabbing ? TRUE : FALSE);
//GetDlgItem(IDC_SAVE_TIFF_BUTTON)->EnableWindow(m_bStartGrabbing ? TRUE : FALSE);
//GetDlgItem(IDC_SAVE_PNG_BUTTON)->EnableWindow(m_bStartGrabbing ? TRUE : FALSE);
//GetDlgItem(IDC_SAVE_JPG_BUTTON)->EnableWindow(m_bStartGrabbing ? TRUE : FALSE);

	GetDlgItem(IDC_EXPOSURE_EDIT)->EnableWindow(m_bOpenDevice ? TRUE : FALSE);
	GetDlgItem(IDC_FRAME_RATE_EDIT)->EnableWindow(m_bOpenDevice ? TRUE : FALSE);
	GetDlgItem(IDC_GAIN_EDIT)->EnableWindow(m_bOpenDevice ? TRUE : FALSE);

	GetDlgItem(IDC_BTN_GET_PARAMETER)->EnableWindow(m_bOpenDevice ? TRUE : FALSE);
	GetDlgItem(IDC_BTN_SET_PARAMETER)->EnableWindow(m_bOpenDevice ? TRUE : FALSE);

	//GetDlgItem(IDC_CONTINUS_MODE_RADIO)->EnableWindow(m_bOpenDevice ? TRUE : FALSE);
	//GetDlgItem(IDC_TRIGGER_MODE_RADIO)->EnableWindow(m_bOpenDevice ? TRUE : FALSE);
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
void CHikVisionCamViewerDlg::ShowErrorMsg(CString csMessage, int nErrorNum)
{
	CString errorMsg;
	if (nErrorNum == 0)
	{
		errorMsg.Format(_T("%s"), csMessage);
	}
	else
	{
		errorMsg.Format(_T("%s: Error = %x: "), csMessage, nErrorNum);
	}

	switch (nErrorNum)
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




int CHikVisionCamViewerDlg::GetTriggerMode()
{
	MVCC_ENUMVALUE stEnumValue = { 0 };

	int nRet = m_pcMyCamera->GetEnumValue("TriggerMode", &stEnumValue);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	m_i32TriggerMode = stEnumValue.nCurValue;

	if (MV_TRIGGER_MODE_ON == m_i32TriggerMode)
	{
		//OnBnClickedTriggerModeRadio();
		ShowErrorMsg(TEXT("[MV_TRIGGER_MODE_ON] OnBnClickedTriggerModeRadio"), 0);
	}
	else
	{
		m_i32TriggerMode = MV_TRIGGER_MODE_OFF;
		//OnBnClickedContinusModeRadio();
		ShowErrorMsg(TEXT("[MV_TRIGGER_MODE_OFF] OnBnClickedContinusModeRadio"), 0);
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

	int nRet = m_pcMyCamera->GetFloatValue("ExposureTime", &stFloatValue);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	m_f64ExposureEdit = stFloatValue.fCurValue;

	return MV_OK;
}

int CHikVisionCamViewerDlg::SetExposureTime()
{
	//Adjust these two exposure mode to allow exposure time effective
	int nRet = m_pcMyCamera->SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	m_pcMyCamera->SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);

	return m_pcMyCamera->SetFloatValue("ExposureTime", (float)m_f64ExposureEdit);
}

int CHikVisionCamViewerDlg::GetGain()
{
	MVCC_FLOATVALUE stFloatValue = { 0 };

	int nRet = m_pcMyCamera->GetFloatValue("Gain", &stFloatValue);
	if (MV_OK != nRet)
	{
		return nRet;
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

	int nRet = m_pcMyCamera->GetFloatValue("ResultingFrameRate", &stFloatValue);
	if (MV_OK != nRet)
	{
		return nRet;
	}
	m_f64FrameRateEdit = stFloatValue.fCurValue;

	return MV_OK;
}

int CHikVisionCamViewerDlg::SetFrameRate()
{
	int nRet = m_pcMyCamera->SetBoolValue("AcquisitionFrameRateEnable", true);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	return m_pcMyCamera->SetFloatValue("AcquisitionFrameRate", (float)m_f64FrameRateEdit);
}

int CHikVisionCamViewerDlg::GetTriggerSource()
{
	MVCC_ENUMVALUE stEnumValue = { 0 };

	int nRet = m_pcMyCamera->GetEnumValue("TriggerSource", &stEnumValue);
	if (MV_OK != nRet)
	{
		return nRet;
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
	int nRet = MV_OK;
	if (m_bSoftWareTriggerCheck)
	{
		m_i32TriggerSource = MV_TRIGGER_SOURCE_SOFTWARE;
		nRet = m_pcMyCamera->SetEnumValue("TriggerSource", m_i32TriggerSource);
		if (MV_OK != nRet)
		{
			ShowErrorMsg(TEXT("Set Software Trigger Fail"), nRet);
			return nRet;
		}
		//GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(TRUE);
	}
	else
	{
		m_i32TriggerSource = MV_TRIGGER_SOURCE_LINE0;
		nRet = m_pcMyCamera->SetEnumValue("TriggerSource", m_i32TriggerSource);
		if (MV_OK != nRet)
		{
			ShowErrorMsg(TEXT("Set Hardware Trigger Fail"), nRet);
			return nRet;
		}
		//GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(FALSE);
	}

	return nRet;
}

















void CHikVisionCamViewerDlg::OnBnClickedmCameraSearch()
{
	CString strMsg;

	//Clear Device List Information
	m_cbCameraList.ResetContent();

	//Device Information List Initialization
	memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

	// Enumerate all devices within subnet
	int nRet = HikVisionCamera::EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);
	if (MV_OK != nRet)
	{
		return;
	}

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
				sprintf_s(strUserName, 256, "%s %s (%s)", pDeviceInfo->SpecialInfo.stGigEInfo.chManufacturerName,
					pDeviceInfo->SpecialInfo.stGigEInfo.chModelName,
					pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber);
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
				sprintf_s(strUserName, 256, "%s %s (%s)", pDeviceInfo->SpecialInfo.stUsb3VInfo.chManufacturerName,
					pDeviceInfo->SpecialInfo.stUsb3VInfo.chModelName,
					pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
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

	m_cbCameraList.SetCurSel(0);

	if (0 == m_stDevList.nDeviceNum)
	{
		ShowErrorMsg(TEXT("No device"), 0);
		return;
	}
	m_btnCameraStart.EnableWindow(TRUE);
}

void CHikVisionCamViewerDlg::OnBnClickedCamStart()
{
	if (TRUE == m_bOpenDevice || NULL != m_pcMyCamera)
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

	int nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[nIndex]);
	if (MV_OK != nRet)
	{
		delete m_pcMyCamera;
		m_pcMyCamera = NULL;
		ShowErrorMsg(TEXT("Open Fail"), nRet);
		return;
	}

	/////////////////////
	if (false == m_bCameraStarted)
	{
		m_bCameraStarted = true;
	}
	else// if (m_bCameraStarted)
	{
		AfxMessageBox(_T("Camera already started."));
		return;
	}

	/////////////////////

	// Detection network optimal package size(It only works for the GigE camera)
	if (m_stDevList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
	{
		unsigned int nPacketSize = 0;
		nRet = m_pcMyCamera->GetOptimalPacketSize(&nPacketSize);
		if (nRet == MV_OK)
		{
			nRet = m_pcMyCamera->SetIntValue("GevSCPSPacketSize", nPacketSize);
			if (nRet != MV_OK)
			{
				ShowErrorMsg(TEXT("Warning: Set Packet Size fail!"), nRet);
			}
		}
		else
		{
			ShowErrorMsg(TEXT("Warning: Get Packet Size fail!"), nRet);
		}
	}

	m_bOpenDevice = TRUE;
	EnableControls(TRUE);
	OnBnClickedGetParameterButton(); // ch:삿혤꽝鑒 | en:Get Parameter
}

void CHikVisionCamViewerDlg::OnBnClickedCamPause()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_bCameraStarted)
	{
		m_bCameraStarted = false;
	}
}

void CHikVisionCamViewerDlg::OnBnClickedCamStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CHikVisionCamViewerDlg::OnBnClickedGetParameterButton()
{
	int nRet = GetTriggerMode();
	if (nRet != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Trigger Mode Fail"), nRet);
	}

	nRet = GetExposureTime();
	if (nRet != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Exposure Time Fail"), nRet);
	}

	nRet = GetGain();
	if (nRet != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Gain Fail"), nRet);
	}

	nRet = GetFrameRate();
	if (nRet != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Frame Rate Fail"), nRet);
	}

	nRet = GetTriggerSource();
	if (nRet != MV_OK)
	{
		ShowErrorMsg(TEXT("Get Trigger Source Fail"), nRet);
	}

	UpdateData(FALSE);
}

void CHikVisionCamViewerDlg::OnBnClickedSetParameterButton()
{
	UpdateData(TRUE);

	bool bIsSetSucceed = true;

	int nRet = SetExposureTime();
	if (nRet != MV_OK)
	{
		bIsSetSucceed = false;
		ShowErrorMsg(TEXT("Set Exposure Time Fail"), nRet);
	}

	nRet = SetGain();
	if (nRet != MV_OK)
	{
		bIsSetSucceed = false;
		ShowErrorMsg(TEXT("Set Gain Fail"), nRet);
	}

	nRet = SetFrameRate();
	if (nRet != MV_OK)
	{
		bIsSetSucceed = false;
		ShowErrorMsg(TEXT("Set Frame Rate Fail"), nRet);
	}

	if (true == bIsSetSucceed)
	{
		ShowErrorMsg(TEXT("Set Parameter Succeed"), nRet);
	}
}