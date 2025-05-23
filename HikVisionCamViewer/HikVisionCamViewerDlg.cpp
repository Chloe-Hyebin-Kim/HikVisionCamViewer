﻿// HikVisionCamViewerDlg.cpp: 구현 파일

#include "stdafx.h"
#include "HikVisionCamViewer.h"
#include "HikVisionCamViewerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum CamVendor
{
	CamVendor_NONE = 0,
	CamVendor_HKVision = 1,
	CamVendor_VWorks = 2,
};

//typedef enum {
//	GZ_CAMVENDOR_NONE,
//	GZ_CAMVENDOR_HIKVISION,
//	GZ_CAMVENDOR_HIKVISION_SONY,
//	GZ_CAMVENDOR_VIEWORKS,
//	GZ_CAMVENDOR_EMULATOR,
//	GZ_CAMVENDOR_MAX
//} eGzCamVendor;


//int GzCamDetect(void)
//{
//	int nRet = MV_OK;
//
//	MV_CC_DEVICE_INFO_LIST stDeviceList;
//	memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
//
//
//	nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE, &stDeviceList); // GigE 디바이스만 찾기
//	if (MV_OK != nRet)
//	{
//		printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
//		return GZ_CAMVENDOR_MAX;
//	}
//
//	CString strVendor;
//
//	if (stDeviceList.nDeviceNum > 0)
//	{
//		for (int i = 0; i < 1/*stDeviceList.nDeviceNum*/; i++)
//		{
//			if (stDeviceList.pDeviceInfo[i]->nTLayerType == MV_GIGE_DEVICE)
//			{
//				strVendor = stDeviceList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chManufacturerName;
//			}
//		}
//	}
//	else
//	{
//		printf("Find No GIGE Devices!\n");
//		return GZ_CAMVENDOR_MAX;
//	}
//
//	if (strVendor.Compare(_T("VIEWORKS")) == 0)
//		return GZ_CAMVENDOR_VIEWORKS;
//	else if (strVendor.Compare(_T("Golfzon")) == 0)
//		return GZ_CAMVENDOR_HIKVISION;
//	else if (strVendor.Compare(_T("Golfzon2")) == 0)
//		return GZ_CAMVENDOR_HIKVISION_SONY;
//	else
//		return GZ_CAMVENDOR_MAX;
//}

int CamVendorSelector(void)
{
	int CamVendor = 0;
	{
		HMODULE hCamDetect = LoadLibrary(_T("NGSCamDetecter.dll"));

		if (hCamDetect)
		{
			typedef int (*fpCamDetect)(void);
			fpCamDetect GzCamDetect = (fpCamDetect)GetProcAddress(hCamDetect, "GzCamDetect");
			CamVendor = GzCamDetect();
			FreeLibrary(hCamDetect);
		}
		else
		{
			return CamVendor_VWorks;
		}

		if (CamVendor == 1 || CamVendor == 2)
			return CamVendor_HKVision;

		if (CamVendor == 3)
			return CamVendor_VWorks;
	}

	return CamVendor_VWorks;
}

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
	, m_ui32ValidCamNum(0)
	, m_bOpenDevice(FALSE)
	, m_bStartGrabbing(FALSE)
	, m_i32TriggerMode(MV_TRIGGER_MODE_OFF)
	, m_i32CurCameraIndex(-1)
	, m_i32ZoomInIndex(-1)
	, m_i32SaveIndex(0)

	, m_i32DeviceCombo(0)
	, m_i32TriggerSource(MV_TRIGGER_SOURCE_SOFTWARE)
	, m_f64ExposureEdit(0)
	, m_f64GainEdit(0)
	, m_f64FrameRateEdit(0)
	, m_bSoftWareTriggerCheck(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

	for (int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		m_bCamCheckArr[i] = FALSE;
		m_hwndDisplayArr[i] = NULL;
		m_pcMyCameraArr[i] = NULL;
		m_hGrabThreadArr[i] = NULL;
		m_pSaveImageBufArr[i] = NULL;
		m_ui32SaveImageBufSizeArr[i] = 0;
		memset(&(m_stImageInfoArr[i]), 0, sizeof(MV_FRAME_OUT_INFO_EX));
	}
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

	CDialog::DoDataExchange(pDX);
	for (int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		DDX_Check(pDX, IDC_CAM1_CHECK + i, m_bCamCheckArr[i]);
	}

	DDX_Control(pDX, IDC_OUTPUT_INFO_LIST, m_ctrlListBoxInfo);
}

BEGIN_MESSAGE_MAP(CHikVisionCamViewerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	// }}AFX_MSG_MAP
	ON_WM_TIMER()

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

	ON_BN_CLICKED(IDC_BTN_JPG, &CHikVisionCamViewerDlg::OnBnClickedSaveJpgButton)
	ON_BN_CLICKED(IDC_BTN_PNG, &CHikVisionCamViewerDlg::OnBnClickedSavePngButton)

	ON_WM_CLOSE()

	//ON_LBN_DBLCLK(IDC_OUTPUT_INFO_LIST, &CMultipleCameraDlg::OnLbnDblclkOutputInfoList)
	//ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CHikVisionCamViewerDlg message map handling


//Working thread
unsigned int    __stdcall   WorkThread(void* pUser)
{
	if (pUser)
	{
		CHikVisionCamViewerDlg* pCam = (CHikVisionCamViewerDlg*)pUser;
		if (NULL == pCam)
		{
			return -1;
		}
		int nCurCameraIndex = pCam->m_i32CurCameraIndex;
		pCam->m_i32CurCameraIndex = -1;
		pCam->ThreadFunc(nCurCameraIndex);

		return 0;
	}

	return -1;
}


int CHikVisionCamViewerDlg::ThreadFunc(int nCurCameraIndex)
{
	//TODO:  콜백 기반 구조로 변경 ( 현재 타이밍 제어를 하며 직접 MV_CC_GetImageBuffer를 반복 호출 하는 구조임)
	if (m_pcMyCameraArr[nCurCameraIndex])
	{
		MV_FRAME_OUT stImageOut = { 0 };
		MV_DISPLAY_FRAME_INFO stDisplayInfo = { 0 };
		while (m_bStartGrabbing)
		{
			//주의!!!!! MV_CC_GetImageBuffer는 MV_CC_FreeImageBuffer와 반드시 쌍으로 사용해야 함!!!
			int nRet = m_pcMyCameraArr[nCurCameraIndex]->GetImageBuffer(&stImageOut, 1000);//SDK에서 스트림 버퍼를 자동 관리
			if (nRet == MV_OK)
			{
				EnterCriticalSection(&m_hSaveImageMuxArr[nCurCameraIndex]);
				if (NULL == m_pSaveImageBufArr[nCurCameraIndex] || stImageOut.stFrameInfo.nFrameLen > m_ui32SaveImageBufSizeArr[nCurCameraIndex])
				{
					if (m_pSaveImageBufArr[nCurCameraIndex])
					{
						free(m_pSaveImageBufArr[nCurCameraIndex]);
						m_pSaveImageBufArr[nCurCameraIndex] = NULL;
					}

					m_pSaveImageBufArr[nCurCameraIndex] = (unsigned char*)malloc(sizeof(unsigned char) * stImageOut.stFrameInfo.nFrameLen);
					if (m_pSaveImageBufArr[nCurCameraIndex] == NULL)
					{
						EnterCriticalSection(&m_hSaveImageMuxArr[nCurCameraIndex]);
						return 0;
					}
					m_ui32SaveImageBufSizeArr[nCurCameraIndex] = stImageOut.stFrameInfo.nFrameLen;
				}
				memcpy(m_pSaveImageBufArr[nCurCameraIndex], stImageOut.pBufAddr, stImageOut.stFrameInfo.nFrameLen);
				memcpy(&(m_stImageInfoArr[nCurCameraIndex]), &(stImageOut.stFrameInfo), sizeof(MV_FRAME_OUT_INFO_EX));
				LeaveCriticalSection(&m_hSaveImageMuxArr[nCurCameraIndex]);

				stDisplayInfo.hWnd = m_hwndDisplayArr[nCurCameraIndex];
				stDisplayInfo.pData = stImageOut.pBufAddr;
				stDisplayInfo.nDataLen = stImageOut.stFrameInfo.nFrameLen;
				stDisplayInfo.nWidth = stImageOut.stFrameInfo.nWidth;
				stDisplayInfo.nHeight = stImageOut.stFrameInfo.nHeight;
				stDisplayInfo.enPixelType = stImageOut.stFrameInfo.enPixelType;

				nRet = m_pcMyCameraArr[nCurCameraIndex]->DisplayOneFrame(&stDisplayInfo);
				if (MV_OK != nRet)
				{
					PrintMessage("Display one frame fail! DevIndex[%d], nRet[%#x]\r\n", nCurCameraIndex + 1, nRet);
				}

				//주의!!!!! MV_CC_GetImageBuffer는 MV_CC_FreeImageBuffer와 반드시 쌍으로 사용해야 함!!!
				nRet = m_pcMyCameraArr[nCurCameraIndex]->FreeImageBuffer(&stImageOut);
				if (MV_OK != nRet)
				{
					PrintMessage("Free image buffer fail! DevIndex[%d], nRet[%#x]\r\n", nCurCameraIndex + 1, nRet);
				}
			}
			else
			{
				if (MV_TRIGGER_MODE_ON == m_i32TriggerMode)
				{
					Sleep(5);
				}
			}
		}
	}

	return MV_OK;
}

void CHikVisionCamViewerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == WM_TIMER_GRAB_INFO)
	{
		int nRet = 0;
		unsigned int nLostFrame = 0;
		unsigned int nFrameCount = 0;
		MV_MATCH_INFO_NET_DETECT stMatchInfoNetDetect = { 0 };
		MV_MATCH_INFO_USB_DETECT stMatchInfoUSBDetect = { 0 };

		for (int i = 0; i < MAX_DEVICE_NUM; i++)
		{
			if (m_pcMyCameraArr[i])
			{
				MV_CC_DEVICE_INFO stDevInfo = { 0 };
				m_pcMyCameraArr[i]->GetDeviceInfo(&stDevInfo);

				if (stDevInfo.nTLayerType == MV_GIGE_DEVICE)
				{
					nRet = m_pcMyCameraArr[i]->GetGevAllMatchInfo(&stMatchInfoNetDetect);
					nLostFrame = stMatchInfoNetDetect.nLostFrameCount;
					nFrameCount = stMatchInfoNetDetect.nNetRecvFrameCount;
				}
				else if (stDevInfo.nTLayerType == MV_USB_DEVICE)
				{
					nRet = m_pcMyCameraArr[i]->GetU3VAllMatchInfo(&stMatchInfoUSBDetect);
					nLostFrame = stMatchInfoUSBDetect.nErrorFrameCount;
					nFrameCount = stMatchInfoUSBDetect.nReceivedFrameCount;
				}
				else
				{
					return;
				}

				if (MV_OK == nRet)
				{
					char chFrameCount[128] = { 0 };
					wchar_t strFrameCount[128] = { 0 };

					sprintf_s(chFrameCount, 128, "FrameCount:%d,LostFrame:%d", nFrameCount, nLostFrame);
					MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(chFrameCount), -1, strFrameCount, 128);
					//GetDlgItem(IDC_FRAME_COUNT1_STATIC + i)->SetWindowText(strFrameCount);
				}
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
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

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	//Select CamVendor
	int CamVendor = CamVendor_NONE;
	HMODULE hCamDetect = LoadLibrary(_T("NGSCamDetecter.dll"));//HikVision의 EnumDevices 함수가 있는 dll 로딩
	if (hCamDetect)
	{
		typedef int (*fpCamDetect)(void);//함수포인터
		fpCamDetect GzCamDetect = (fpCamDetect)GetProcAddress(hCamDetect, "GzCamDetect"); //NGSCamDetecter.dll에서 GzCamDetect이름을 가진 함수 주소를 가져옴
		CamVendor = GzCamDetect();
		FreeLibrary(hCamDetect);
	}
	else
	{
		CamVendor = CamVendor_VWorks;
	}

	//Load Vendor Library "~.dll"
	m_hCamLib = 0;

	if (CamVendor == 1 || CamVendor == 2)
	{
		CamVendor = CamVendor_HKVision;

		m_hCamLib = LoadLibrary(L"NGSCamLinkDLLHV.dll");
		if (!m_hCamLib)
		{
			memset(&m_CamLinkPtr, 0, sizeof(m_CamLinkPtr));

			MessageBox(L"NGSCamLinkDLLHV.dll을 찾을 수 없습니다.");
			exit(0);
			return false;
		}
	}
	else if (CamVendor == 3)
	{
		CamVendor = CamVendor_VWorks;

		m_hCamLib = LoadLibrary(L"NGSCamLinkDLLVW.dll");
		if (!m_hCamLib)
		{
			memset(&m_CamLinkPtr, 0, sizeof(m_CamLinkPtr));

			MessageBox(L"NGSCamLinkDLLVW.dll을 찾을 수 없습니다.");
			exit(0);
			return false;
		}
	}
	else if (CamVendor == CamVendor_NONE)
	{
		MessageBox(L"연결된 카메라를 찾을 수 없습니다.");
		exit(0);
		return false;
	}
	else
	{
		MessageBox(L"NGSCamDetecter.dll을 찾을 수 없습니다.");
		exit(0);
		return false;
	}

	//
	m_CamLinkPtr.GetVender = (fpGetVender)GetProcAddress(m_hCamLib, "GzCamLink_GetVender");
	m_CamLinkPtr.InitDevice = (fpInitDevice)GetProcAddress(m_hCamLib, "GzCamLink_InitDevice");
	m_CamLinkPtr.StartDevice = (fpStartDevice)GetProcAddress(m_hCamLib, "GzCamLink_StartDevice");
	m_CamLinkPtr.StopDevice = (fpStopDevice)GetProcAddress(m_hCamLib, "GzCamLink_StopDevice");
	m_CamLinkPtr.ReleaseDevice = (fpReleaseDevice)GetProcAddress(m_hCamLib, "GzCamLink_ReleaseDevice");

	m_CamLinkPtr.GetFeatureValueINT = (fpGetFeatureValueINT)GetProcAddress(m_hCamLib, "GzCamLink_GetFeatureValueINT");
	m_CamLinkPtr.GetFeatureValueDWORD = (fpGetFeatureValueDWORD)GetProcAddress(m_hCamLib, "GzCamLink_GetFeatureValueDWORD");
	m_CamLinkPtr.GetFeatureValueSTR = (fpGetFeatureValueSTR)GetProcAddress(m_hCamLib, "GzCamLink_GetFeatureValueSTR");
	m_CamLinkPtr.GetFeatureValueFLOAT = (fpGetFeatureValueFLOAT)GetProcAddress(m_hCamLib, "GzCamLink_GetFeatureValueFLOAT");
	m_CamLinkPtr.SetFeatureValueINT = (fpSetFeatureValueINT)GetProcAddress(m_hCamLib, "GzCamLink_SetFeatureValueINT");
	m_CamLinkPtr.SetFeatureValueDWORD = (fpSetFeatureValueDWORD)GetProcAddress(m_hCamLib, "GzCamLink_SetFeatureValueDWORD");
	m_CamLinkPtr.SetFeatureValueSTR = (fpSetFeatureValueSTR)GetProcAddress(m_hCamLib, "GzCamLink_SetFeatureValueSTR");
	m_CamLinkPtr.SetFeatureValueFLOAT = (fpSetFeatureValueFLOAT)GetProcAddress(m_hCamLib, "GzCamLink_SetFeatureValueFLOAT");

	m_CamLinkPtr.GetBufferPointer = (fpGetBufferPointer)GetProcAddress(m_hCamLib, "GzCamLink_GetBufferPointer");



	//DisplayWindowInitial();
	OnBnClickedmCameraSearch();
	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		m_hwndDisplayArr[i] = GetDlgItem(IDC_DISPLAY1_STATIC + i)->GetSafeHwnd();
		GetDlgItem(IDC_DISPLAY1_STATIC + i)->GetWindowRect(&m_hwndRectArr[i]);
		ScreenToClient(&m_hwndRectArr[i]);
		InitializeCriticalSection(&m_hSaveImageMuxArr[i]);
	}

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

void CHikVisionCamViewerDlg::OnClose()
{
	KillTimer(WM_TIMER_GRAB_INFO);
	m_bStartGrabbing = FALSE;

	for (int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_hGrabThreadArr[i])
		{
			WaitForSingleObject(m_hGrabThreadArr[i], INFINITE);
			CloseHandle(m_hGrabThreadArr[i]);
			m_hGrabThreadArr[i] = NULL;
		}

		if (m_pcMyCameraArr[i])
		{
			delete(m_pcMyCameraArr[i]);
			m_pcMyCameraArr[i] = NULL;
		}
		DeleteCriticalSection(&m_hSaveImageMuxArr[i]);
	}

	m_bOpenDevice = FALSE;
	CDialog::OnClose();
}

bool CHikVisionCamViewerDlg::PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
	if (NULL == pstMVDevInfo)
	{
		printf("The Pointer of pstMVDevInfo is NULL!\n");
		return false;
	}

	if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
	{
		int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
		int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
		int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
		int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
		//Print current ip and user defined name
		printf("CurrentIp: %d.%d.%d.%d\n", nIp1, nIp2, nIp3, nIp4);
		printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
	}
	else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
	{
		printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
	}
	else
	{
		printf("Not support.\n");
	}
	return true;
}

void CHikVisionCamViewerDlg::PrintMessage(const char* pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	char   szInfo[512] = { 0 };
	vsprintf_s(szInfo, 512, pszFormat, args);
	va_end(args);
	m_ctrlListBoxInfo.AddString(CA2T(szInfo));
	m_ctrlListBoxInfo.SetTopIndex(m_ctrlListBoxInfo.GetCount() - 1);
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

void CHikVisionCamViewerDlg::EnableControls(bool bIsCameraReady /*=true*/)
{
	GetDlgItem(IDC_BTN_CAMSEARCH)->EnableWindow(m_bOpenDevice ? FALSE : TRUE);
	GetDlgItem(IDC_BTN_CAMOPEN)->EnableWindow(m_bOpenDevice ? FALSE : TRUE);
	GetDlgItem(IDC_BTN_CAMCLOSE)->EnableWindow(m_bOpenDevice ? TRUE : FALSE);

	GetDlgItem(IDC_BTN_CAMSTART)->EnableWindow(m_bStartGrabbing ? FALSE : m_bOpenDevice);
	GetDlgItem(IDC_BTN_CAMSTOP)->EnableWindow(m_bStartGrabbing ? m_bOpenDevice : FALSE);
	//GetDlgItem(IDC_BTN_CAMPAUSE)->EnableWindow((m_bStartGrabbing && bIsCameraReady) ? true : false);//CameraPause
	//GetDlgItem(IDC_BTN_CAMPAUSE)->EnableWindow((m_bOpenDevice && bIsCameraReady) ? true : false);	//CameraPause
	GetDlgItem(IDC_BTN_CAMPAUSE)->EnableWindow(m_bStartGrabbing ? m_bOpenDevice : FALSE);

	GetDlgItem(IDC_EXPOSURE_EDIT)->EnableWindow(m_bOpenDevice ? m_bOpenDevice : FALSE);
	GetDlgItem(IDC_FRAME_RATE_EDIT)->EnableWindow(m_bOpenDevice ? m_bOpenDevice : FALSE);
	GetDlgItem(IDC_GAIN_EDIT)->EnableWindow(m_bOpenDevice ? m_bOpenDevice : FALSE);

	GetDlgItem(IDC_RADIO_CONTINUS)->EnableWindow(m_bOpenDevice ? m_bOpenDevice : FALSE);
	GetDlgItem(IDC_RADIO_TRIGGER)->EnableWindow(m_bOpenDevice ? m_bOpenDevice : FALSE);

	GetDlgItem(IDC_BTN_PNG)->EnableWindow(m_bStartGrabbing ? m_bOpenDevice : FALSE);
	GetDlgItem(IDC_BTN_JPG)->EnableWindow(m_bStartGrabbing ? m_bOpenDevice : FALSE);

	GetDlgItem(IDC_BTN_GET_PARAMETER)->EnableWindow(m_bOpenDevice ? true : false);
	GetDlgItem(IDC_BTN_SET_PARAMETER)->EnableWindow(m_bOpenDevice ? true : false);

	GetDlgItem(IDC_SOFTWARE_TRIGGER_CHECK)->EnableWindow(m_bOpenDevice ? true : false);

	//GetDlgItem(IDC_SOFTWARE_MODE_BUTTON)->EnableWindow(m_i32TriggerMode ? m_bOpenDevice : FALSE);  //?
	//GetDlgItem(IDC_HARDWARE_MODE_BUTTON)->EnableWindow(m_i32TriggerMode ? m_bOpenDevice : FALSE);  //?

	GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(m_i32TriggerSource == MV_TRIGGER_SOURCE_SOFTWARE && m_i32TriggerMode == MV_TRIGGER_MODE_ON ? m_bOpenDevice : FALSE);
	//GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow((m_bStartGrabbing && m_bSoftWareTriggerCheck && ((CButton*)GetDlgItem(IDC_RADIO_TRIGGER))->GetCheck()) ? TRUE : FALSE);
}

void CHikVisionCamViewerDlg::DisplayWindowInitial()
{
	OnBnClickedmCameraSearch();

	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		m_hwndDisplayArr[i] = GetDlgItem(IDC_DISPLAY1_STATIC + i)->GetSafeHwnd();
		if (m_hwndDisplayArr[i])
		{
			GetDlgItem(IDC_DISPLAY1_STATIC + i)->GetWindowRect(&m_hwndRectArr[i]);
			ScreenToClient(&m_hwndRectArr[i]);
			InitializeCriticalSection(&m_hSaveImageMuxArr[i]);
		}
	}
}

int CHikVisionCamViewerDlg::CloseDevice()
{
	KillTimer(WM_TIMER_GRAB_INFO);
	m_bStartGrabbing = FALSE;
	for (int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_hGrabThreadArr[i])
		{
			WaitForSingleObject(m_hGrabThreadArr[i], INFINITE);
			CloseHandle(m_hGrabThreadArr[i]);
			m_hGrabThreadArr[i] = NULL;
		}
	}

	int nRet = MV_OK;
	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_pcMyCameraArr[i])
		{
			nRet = m_pcMyCameraArr[i]->Close();
			if (MV_OK != nRet)
			{
				PrintMessage("Close device fail! DevIndex[%d], nRet[%#x]\r\n", i + 1, nRet);
			}

			delete(m_pcMyCameraArr[i]);
			m_pcMyCameraArr[i] = NULL;
		}

		if (i < m_ui32ValidCamNum)
		{
			GetDlgItem(IDC_CAM1_CHECK + i)->EnableWindow(TRUE);
			//GetDlgItem(IDC_FRAME_COUNT1_STATIC + i)->SetWindowText(L"FrameCount:0,LostFrame:0");
		}

		if (m_pSaveImageBufArr[i])
		{
			free(m_pSaveImageBufArr[i]);
			m_pSaveImageBufArr[i] = NULL;
		}
		m_ui32SaveImageBufSizeArr[i] = 0;
	}

	m_bOpenDevice = FALSE;

	if (m_i32ZoomInIndex != -1)
	{
		CPoint point;
		OnLButtonDblClk(0, point);
	}

	if (m_i32SaveIndex != 0)
	{
		m_i32SaveIndex = 0;
	}

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


void CHikVisionCamViewerDlg::OnBnClickedmCameraSearch()
{
	//Clear Device List Information
	m_cbCameraList.ResetContent();

	//Device Information List Initialization
	memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

	// Enumerate all devices within subnet
	int i32Ret = HikVisionCamera::EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);  // get device list (MV_CC_EnumDevices)
	if ((MV_OK != i32Ret) || (m_stDevList.nDeviceNum == 0))
	{
		PrintMessage("[OnBnClickedmCameraSearch] Find any device!\r\n");
		return;
	}

	PrintMessage("[OnBnClickedmCameraSearch] Find %d devices!\r\n", m_stDevList.nDeviceNum);
	m_ui32ValidCamNum = 0;

	//Add value to the information list box and display
	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		char chDeviceName[256] = { 0 };

		if (i < m_stDevList.nDeviceNum)
		{
			//TODO: 장치 접근 가능 여부 확인 ( MV_CC_IsDeviceAccessible)

			MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];

			if (MV_GIGE_DEVICE == pDeviceInfo->nTLayerType)
			{
				if (strcmp("", (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName)) != 0)
				{
					int nIp1 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
					int nIp2 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
					int nIp3 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
					int nIp4 = (m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

					wchar_t* pUserName = NULL;
					DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, NULL, 0);
					pUserName = new wchar_t[dwLenUserName];
					MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, pUserName, dwLenUserName);

					CString strMsg;
					strMsg.Format(_T("[%d : %s]  (GigE: %d.%d.%d.%d)"), i, pUserName, nIp1, nIp2, nIp3, nIp4);
					//strMsg.Format(_T("[%d : %s]  (GigE: %d.%d.%d.%d)"), i, pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName, nIp1, nIp2, nIp3, nIp4);
					m_cbCameraList.AddString(strMsg);

					if (pUserName)
					{
						delete[] pUserName;
						pUserName = NULL;
					}

					sprintf_s(chDeviceName, 256, "[%s]  (%d : %d.%d.%d.%d)", pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName, i, nIp1, nIp2, nIp3, nIp4);
				}
				else
				{
					sprintf_s(chDeviceName, 256, "%s (%s)", pDeviceInfo->SpecialInfo.stGigEInfo.chModelName, pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber);
				}
			}
			else if (MV_USB_DEVICE == pDeviceInfo->nTLayerType)
			{
				if (strcmp("", (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName)) != 0)
				{
					sprintf_s(chDeviceName, 256, "[%s]", pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
				}
				else
				{
					sprintf_s(chDeviceName, 256, "%s (%s)", pDeviceInfo->SpecialInfo.stUsb3VInfo.chModelName, pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
				}
			}

			PrintMessage("[OnBnClickedmCameraSearch] Find devices! - %s\r\n", pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName);

			GetDlgItem(IDC_CAM1_CHECK + i)->EnableWindow(TRUE);
			m_ui32ValidCamNum++;
		}
		else
		{
			sprintf_s(chDeviceName, 256, "Cam%d", i + 1);
			GetDlgItem(IDC_CAM1_CHECK + i)->EnableWindow(FALSE);
		}


		wchar_t strUserName[128] = { 0 };
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(chDeviceName), -1, strUserName, 128);
		GetDlgItem(IDC_CAM1_CHECK + i)->SetWindowText(strUserName);
		m_bCamCheckArr[i] = FALSE;
	}

	UpdateData(FALSE);

	EnableControls(true);
}

void CHikVisionCamViewerDlg::OnBnClickedCamOpen()
{
	if (true == m_bOpenDevice)
	{
		PrintMessage("[OnBnClickedCamOpen]  All device is closed!\r\n");
		return;
	}

	UpdateData(TRUE);
	BOOL bHaveCheck = FALSE;

	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_bCamCheckArr[i])
		{
			bHaveCheck = TRUE;
			if (NULL == m_pcMyCameraArr[i])
			{
				m_pcMyCameraArr[i] = new HikVisionCamera;//m_hDevHandle
			}

			int i32Ret = m_pcMyCameraArr[i]->Open(m_stDevList.pDeviceInfo[i]);//장치 핸들 생성 및 API를 사용하여 장치 연결 ( MV_CC_CreateHandle, MV_CC_OpenDevice)
			if (MV_OK != i32Ret)
			{
				//MV_CC_DestroyHandle
				delete(m_pcMyCameraArr[i]);
				m_pcMyCameraArr[i] = NULL;

				PrintMessage("[OnBnClickedCamOpen] Open device failed! DevIndex[%d], i32Ret[%#x]\r\n", i + 1, i32Ret);
				ShowErrorMsg(TEXT("Open FAIL!"), i32Ret);
				continue;
			}
			else
			{
				//TODO: 콜백 등록...

				m_bOpenDevice = TRUE;

				PrintMessage("[OnBnClickedCamOpen] Open device! - %s\r\n", m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName);

				if (MV_GIGE_DEVICE == m_stDevList.pDeviceInfo[i]->nTLayerType)
				{
					//Detection network optimal package size(It only works for the GigE camera)
					unsigned int ui32PacketSize = 0;
					i32Ret = m_pcMyCameraArr[i]->GetOptimalPacketSize(&ui32PacketSize);

					if (ui32PacketSize > 0)
					{
						i32Ret = m_pcMyCameraArr[i]->SetIntValue("GevSCPSPacketSize", ui32PacketSize);
						if (MV_OK != i32Ret)
						{
							PrintMessage("[OnBnClickedCamOpen] Set Packet Size fail! DevIndex[%d], i32Ret[%#x]\r\n", i + 1, i32Ret);
						}
					}
					else
					{
						PrintMessage("[OnBnClickedCamOpen] Get Packet Size fail! DevIndex[%d], i32Ret[%#x]\r\n", i + 1, i32Ret);
					}
				}
			}
		}
	}

	if (TRUE == m_bOpenDevice)
	{
		OnBnClickedGetParameterButton();//?

		OnBnClickedContinusModeRadio();
		EnableControls();
	}
	else
	{
		if (FALSE == bHaveCheck)
		{
			PrintMessage("Unchecked device!\r\n");
			ShowErrorMsg(TEXT("Unchecked device!"), 0);
		}
		else
		{
			PrintMessage("No device opened successfully!\r\n");
		}
	}

	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (i < m_ui32ValidCamNum)
		{
			GetDlgItem(IDC_CAM1_CHECK + i)->EnableWindow(!m_bOpenDevice);
		}
	}
}

void CHikVisionCamViewerDlg::OnBnClickedCamClose()
{
	CloseDevice();

	//EnableControls(true);
	EnableControls();
	Invalidate();
}


void CHikVisionCamViewerDlg::OnBnClickedCamStart()
{
	if (false == m_bOpenDevice)
	{
		AfxMessageBox(_T("The camera hasn't been opened yet."));
		return;
	}

	if (true == m_bStartGrabbing)
	{
		AfxMessageBox(_T("The camera is already running."));
		return;
	}

	int nRet = MV_OK;

	SetTimer(WM_TIMER_GRAB_INFO, 300, NULL);
	for (int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_pcMyCameraArr[i])
		{
			memset(&(m_stImageInfoArr[i]), 0, sizeof(MV_FRAME_OUT_INFO_EX));

			nRet = m_pcMyCameraArr[i]->StartGrabbing();
			if (MV_OK != nRet)
			{
				PrintMessage("Start grabbing fail! DevIndex[%d], nRet[%#x]\r\n", i + 1, nRet);
				AfxMessageBox(_T("Start grabbing thread FAIL! DevIndex[%d]"), i + 1);
			}

			m_bStartGrabbing = TRUE;

			unsigned int nCount = 0;
			while (-1 != m_i32CurCameraIndex)
			{
				nCount++;
				if (nCount > 50)
				{
					return;
				}

				Sleep(2);
			}

			unsigned int nThreadID = 0;
			m_i32CurCameraIndex = i;
			m_hGrabThreadArr[i] = (void*)_beginthreadex(NULL, 0, WorkThread, this, 0, &nThreadID);

			if (NULL == m_hGrabThreadArr[i])
			{
				PrintMessage("Create grab thread fail! DevIndex[%d]\r\n", i + 1);
				AfxMessageBox(_T("Start grabbing thread FAIL! DevIndex[%d]"), i + 1);
			}
		}
	}

	EnableControls();
}

void CHikVisionCamViewerDlg::OnBnClickedCamPause()
{
	AfxMessageBox(_T("Camera has been paused."));
	return;
}

void CHikVisionCamViewerDlg::OnBnClickedCamStop()
{
	if (FALSE == m_bOpenDevice || FALSE == m_bStartGrabbing)
	{
		return;
	}

	KillTimer(WM_TIMER_GRAB_INFO);

	int nRet = MV_OK;
	m_bStartGrabbing = FALSE;

	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_pcMyCameraArr[i])
		{
			nRet = m_pcMyCameraArr[i]->StopGrabbing();
			if (MV_OK != nRet)
			{
				PrintMessage("Stop grabbing fail! DevIndex[%d], nRet[%#x]\r\n", i + 1, nRet);
			}
		}

		if (m_hGrabThreadArr[i])
		{
			WaitForSingleObject(m_hGrabThreadArr[i], INFINITE);
			CloseHandle(m_hGrabThreadArr[i]);
			m_hGrabThreadArr[i] = NULL;
		}
	}

	EnableControls();
}



void CHikVisionCamViewerDlg::OnBnClickedContinusModeRadio()
{
	((CButton*)GetDlgItem(IDC_RADIO_CONTINUS))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_TRIGGER))->SetCheck(FALSE);

	((CButton*)GetDlgItem(IDC_SOFTWARE_TRIGGER_CHECK))->EnableWindow(FALSE);
	//GetDlgItem(IDC_SOFTWARE_MODE_BUTTON)->EnableWindow(FALSE);
	//GetDlgItem(IDC_HARDWARE_MODE_BUTTON)->EnableWindow(FALSE);

	m_i32TriggerMode = MV_TRIGGER_MODE_OFF;
	PrintMessage("[OnBnClickedContinusModeRadio] TriggerMode : MV_TRIGGER_MODE_OFF \r\n");
	SetTriggerMode();

	GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(FALSE);
}

void CHikVisionCamViewerDlg::OnBnClickedTriggerModeRadio()
{
	//UpdateData(TRUE);

	((CButton*)GetDlgItem(IDC_RADIO_CONTINUS))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_TRIGGER))->SetCheck(TRUE);

	((CButton*)GetDlgItem(IDC_SOFTWARE_TRIGGER_CHECK))->EnableWindow(TRUE);
	//GetDlgItem(IDC_SOFTWARE_MODE_BUTTON)->EnableWindow(TRUE);
	//GetDlgItem(IDC_HARDWARE_MODE_BUTTON)->EnableWindow(TRUE);

	m_i32TriggerMode = MV_TRIGGER_MODE_ON;
	PrintMessage("[OnBnClickedTriggerModeRadio] TriggerMode : MV_TRIGGER_MODE_ON \r\n");
	SetTriggerMode();

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

	m_i32TriggerSource = MV_TRIGGER_SOURCE_SOFTWARE;

	//Set trigger mode as software trigger
	SetTriggerSource();
	EnableControls();
}

void CHikVisionCamViewerDlg::OnBnClickedSoftwareOnceButton()
{
	if (FALSE == m_bStartGrabbing)
	{
		PrintMessage("Please start grabbing first!\r\n");
		return;
	}

	int nRet = MV_OK;
	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_pcMyCameraArr[i])
		{
			nRet = m_pcMyCameraArr[i]->CommandExecute("TriggerSoftware");//MV_CC_SetCommandValue
			if (MV_OK != nRet)
			{
				PrintMessage("[OnBnClickedSoftwareOnceButton] Soft trigger fail! DevIndex[%d], nRet[%#x]\r\n", i + 1, nRet);
			}
			else
			{
				PrintMessage("[OnBnClickedSoftwareOnceButton] Trigger Software succeed! %s [%d]\r\n", m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName, i + 1);
			}
		}
	}
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

	SetExposureTime();
	SetGain();
	SetFrameRate();
}

void CHikVisionCamViewerDlg::OnBnClickedSaveJpgButton()
{
	int nRet = SaveImage(MV_Image_Jpeg);
	if (MV_OK != nRet)
	{
		ShowErrorMsg(TEXT("Save jpg fail"), nRet);
		return;
	}
	ShowErrorMsg(TEXT("Save jpg succeed"), nRet);
}

void CHikVisionCamViewerDlg::OnBnClickedSavePngButton()
{
	int nRet = SaveImage(MV_Image_Png);
	if (MV_OK != nRet)
	{
		ShowErrorMsg(TEXT("Save png fail"), nRet);
		return;
	}
	ShowErrorMsg(TEXT("Save png succeed"), nRet);
}

int CHikVisionCamViewerDlg::SaveImage(MV_SAVE_IAMGE_TYPE enSaveImageType)
{
	if (FALSE == m_bStartGrabbing)
	{
		return -1;
	}

	MV_SAVE_IMG_TO_FILE_PARAM stSaveFileParam;
	memset(&stSaveFileParam, 0, sizeof(MV_SAVE_IMG_TO_FILE_PARAM));

	++m_i32SaveIndex;

	for (int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_pcMyCameraArr[i])
		{
			EnterCriticalSection(&m_hSaveImageMuxArr[i]);
			if (m_pSaveImageBufArr[i] == NULL || m_stImageInfoArr[i].enPixelType == 0)
			{
				PrintMessage("Save Image fail! No data! DevIndex[%d]\r\n", i + 1);
				LeaveCriticalSection(&m_hSaveImageMuxArr[i]);
				continue;
			}

			stSaveFileParam.enImageType = enSaveImageType; //Image format to save
			stSaveFileParam.enPixelType = m_stImageInfoArr[i].enPixelType;  // Camera pixel type
			stSaveFileParam.nWidth = m_stImageInfoArr[i].nWidth;         // Width
			stSaveFileParam.nHeight = m_stImageInfoArr[i].nHeight;          // Height
			stSaveFileParam.nDataLen = m_stImageInfoArr[i].nFrameLen;
			stSaveFileParam.pData = m_pSaveImageBufArr[i];
			stSaveFileParam.iMethodValue = 0;

			// jpg image nQuality range is (50-99], png image nQuality range is [0-9]
			if (MV_Image_Jpeg == stSaveFileParam.enImageType)
			{
				stSaveFileParam.nQuality = 80;
				sprintf_s(stSaveFileParam.pImagePath, 256, "%s_Image%d_w%d_h%d_fn%03d.jpg", m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName, m_i32SaveIndex, stSaveFileParam.nWidth, stSaveFileParam.nHeight, m_stImageInfoArr[i].nFrameNum);
			}
			else if (MV_Image_Png == stSaveFileParam.enImageType)
			{
				stSaveFileParam.nQuality = 8;
				sprintf_s(stSaveFileParam.pImagePath, 256, "%s_Image%d_w%d_h%d_fn%03d.png", m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName, m_i32SaveIndex, stSaveFileParam.nWidth, stSaveFileParam.nHeight, m_stImageInfoArr[i].nFrameNum);
			}

			int nRet = m_pcMyCameraArr[i]->SaveImageToFile(&stSaveFileParam);
			LeaveCriticalSection(&m_hSaveImageMuxArr[i]);

			if (MV_OK != nRet)
			{
				PrintMessage("Save Image fail! DevIndex[%d], nRet[%#x]\r\n", i + 1, nRet);
			}
			else
			{
				PrintMessage("Save Image succeed! %s_%s_w%d_h%d_fn%03d\r\n", m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName, __TIME__, stSaveFileParam.nWidth, stSaveFileParam.nHeight, m_stImageInfoArr[i].nFrameNum);
			}
		}
	}

	return MV_OK;
}

#pragma region Parameters

int CHikVisionCamViewerDlg::GetTriggerMode()
{
	MVCC_ENUMVALUE stEnumValue = { 0 };

	int i32Ret = m_pcMyCameraArr[0]->GetEnumValue("TriggerMode", &stEnumValue);//MV_CC_GetEnumValue
	if (MV_OK != i32Ret)
	{
		return i32Ret;
	}

	m_i32TriggerMode = stEnumValue.nCurValue;

	if (MV_TRIGGER_MODE_ON == m_i32TriggerMode)
	{
		OnBnClickedTriggerModeRadio();
	}
	else
	{
		m_i32TriggerMode = MV_TRIGGER_MODE_OFF;

		OnBnClickedContinusModeRadio();
	}

	return MV_OK;
}

void CHikVisionCamViewerDlg::SetTriggerMode()
{
	int nRet = MV_OK;
	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_pcMyCameraArr[i])
		{
			nRet = m_pcMyCameraArr[i]->SetEnumValue("TriggerMode", m_i32TriggerMode);
			if (MV_OK != nRet)
			{
				PrintMessage("Set Trigger mode fail! DevIndex[%d], TriggerMode[%d], nRet[%#x]\r\n", i + 1, m_i32TriggerMode, nRet);
			}
			else
			{
				if (MV_TRIGGER_MODE_ON == m_i32TriggerMode)
				{
					PrintMessage("Set Trigger mode succeed!! [ %s ] TriggerMode : MV_TRIGGER_MODE_ON\r\n", m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName);
				}
				else
				{
					PrintMessage("Set Trigger mode succeed!! [ %s ] TriggerMode : MV_TRIGGER_MODE_OFF\r\n", m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName);
				}
			}
		}
	}
}

int CHikVisionCamViewerDlg::GetExposureTime()
{
	MVCC_FLOATVALUE stFloatValue = { 0 };

	int i32Ret = m_pcMyCameraArr[0]->GetFloatValue("ExposureTime", &stFloatValue);
	if (MV_OK != i32Ret)
	{
		return i32Ret;
	}

	m_f64ExposureEdit = stFloatValue.fCurValue;

	return MV_OK;
}

void CHikVisionCamViewerDlg::SetExposureTime()
{
	//Adjust these two exposure mode to allow exposure time effective
	int nRet = MV_OK;
	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_pcMyCameraArr[i])
		{
			nRet = m_pcMyCameraArr[i]->SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
			if (MV_OK != nRet)
			{
				PrintMessage("Set Exposure Time fail! DevIndex[%d], ExposureTime[%f], nRet[%#x]\r\n", i + 1, (float)m_f64ExposureEdit, nRet);
			}
			else
			{
				PrintMessage("Set Exposure Time succeed!! [ %s ] ExposureTime : %f\r\n", m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName, (float)m_f64ExposureEdit);
			}
			m_pcMyCameraArr[i]->SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
		}
	}
}

int CHikVisionCamViewerDlg::GetGain()
{
	MVCC_FLOATVALUE stFloatValue = { 0 };

	int i32Ret = m_pcMyCameraArr[0]->GetFloatValue("Gain", &stFloatValue);
	if (MV_OK != i32Ret)
	{
		return i32Ret;
	}
	m_f64GainEdit = stFloatValue.fCurValue;

	return MV_OK;
}

void CHikVisionCamViewerDlg::SetGain()
{
	// Set Gain after Auto Gain is turned off, this failure does not need to return
	int nRet = MV_OK;
	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_pcMyCameraArr[i])
		{
			nRet = m_pcMyCameraArr[i]->SetEnumValue("GainAuto", 0); //m_pcMyCameraArr[i]->SetEnumValue("GainMode", 0);
			if (MV_OK != nRet)
			{
				PrintMessage("Set Gain fail! DevIndex[%d], Gain[%f], nRet[%#x]\r\n", i + 1, (float)m_f64GainEdit, nRet);
			}
			else
			{
				PrintMessage("Set Gain succeed!! [ %s ] Gain : %f\r\n", m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName, (float)m_f64GainEdit);
			}
		}
	}
}

int CHikVisionCamViewerDlg::GetFrameRate()
{
	MVCC_FLOATVALUE stFloatValue = { 0 };

	int i32Ret = m_pcMyCameraArr[0]->GetFloatValue("ResultingFrameRate", &stFloatValue);
	if (MV_OK != i32Ret)
	{
		return i32Ret;
	}
	m_f64FrameRateEdit = stFloatValue.fCurValue;

	return MV_OK;
}

void CHikVisionCamViewerDlg::SetFrameRate()
{
	int nRet = MV_OK;
	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_pcMyCameraArr[i])
		{
			nRet = m_pcMyCameraArr[i]->SetBoolValue("AcquisitionFrameRateEnable", true);
			if (MV_OK != nRet)
			{
				PrintMessage("Set Acquisition Frame Rate fail! DevIndex[%d], FrameRate[%f], nRet[%#x]\r\n", i + 1, (float)m_f64FrameRateEdit, nRet);
			}
			else
			{
				PrintMessage("Set Acquisition Frame Rate succeed!! [ %s ] FrameRate : %f\r\n", m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.chUserDefinedName, (float)m_f64FrameRateEdit);
			}
		}
	}
}

int CHikVisionCamViewerDlg::GetTriggerSource()
{
	MVCC_ENUMVALUE stEnumValue = { 0 };

	int i32Ret = m_pcMyCameraArr[0]->GetEnumValue("TriggerSource", &stEnumValue);
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

void CHikVisionCamViewerDlg::SetTriggerSource()
{
	/*int nRet = MV_OK;
	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_pcMyCameraArr[i])
		{
			nRet = m_pcMyCameraArr[i]->SetEnumValue("TriggerSource", m_i32TriggerSource);
			if (MV_OK != nRet)
			{
				PrintMessage("Set Trigger source fail! DevIndex[%d], TriggerSource[%d], nRet[%#x]\r\n", i + 1, m_i32TriggerSource, nRet);
			}
		}
	}*/


	int i32Ret = MV_OK;
	for (unsigned int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_pcMyCameraArr[i])
		{
			if (m_bSoftWareTriggerCheck)
			{
				//m_i32TriggerSource = MV_TRIGGER_SOURCE_SOFTWARE;

				i32Ret = m_pcMyCameraArr[i]->SetEnumValue("TriggerSource", m_i32TriggerSource);
				if (MV_OK != i32Ret)
				{
					PrintMessage("Set Trigger source fail! DevIndex[%d], TriggerSource[%d], nRet[%#x]\r\n", i + 1, m_i32TriggerSource, i32Ret);
					ShowErrorMsg(TEXT("Set Software Trigger FAIL!"), i32Ret);
				}

				GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(TRUE);
			}
			else
			{
				m_i32TriggerSource = MV_TRIGGER_SOURCE_LINE0;
				i32Ret = m_pcMyCameraArr[i]->SetEnumValue("TriggerSource", m_i32TriggerSource);
				if (MV_OK != i32Ret)
				{
					ShowErrorMsg(TEXT("Set Hardware Trigger FAIL!"), i32Ret);
				}

				GetDlgItem(IDC_SOFTWARE_ONCE_BUTTON)->EnableWindow(FALSE);
			}
		}
	}
}

#pragma endregion Parameters