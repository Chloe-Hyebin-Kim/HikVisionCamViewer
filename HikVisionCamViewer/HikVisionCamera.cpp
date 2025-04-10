#include "HikVisionCamera.h"
#include <stdio.h>
#include <string.h>


const char HIKVISION_CAMID_TABLE[4][7] =
{
	"RS_CAM",
	"RT_CAM",
	"LS_CAM",
	"LT_CAM"
};

HikVisionCamera::HikVisionCamera()
{
	m_hDevHandle = MV_NULL;

	for (int i = 0; i < GZ_CAMERA_MAX; i++)
	{
		m_CameraHandle[i] = NULL;
		m_ImgBuffer[i] = NULL;
		//m_stDevInfo[i]	= NULL; 는 Initialize에서 함
		// //memcpy(&m_stDevInfo[camID], stDeviceList.pDeviceInfo[i], sizeof(MV_CC_DEVICE_INFO));
	}
	m_HandType = GZ_HANDTYPE_RIGHT;
	m_bRunning = false;
}

HikVisionCamera::~HikVisionCamera()
{
	if (m_hDevHandle)
	{
		MV_CC_DestroyHandle(m_hDevHandle);
		m_hDevHandle = MV_NULL;
	}

	ReleaseDevice();
}

void HikVisionCamera::ImageCallback(unsigned char* pData, MV_FRAME_OUT_INFO* pFrameInfo, void* pUser)
{
	stVwUserPointer* pMe = (stVwUserPointer*)pUser;

	// Image data와 timestamp를 buffer에 복사
	memcpy(pMe->ImgBuffer, pData, pFrameInfo->nWidth * pFrameInfo->nHeight);

	pMe->unTimeStamp = pFrameInfo->nDevTimeStampLow;
	pMe->bFrameGrabbed = true;
}

//int HikVisionCamera::Initialize(MV_CC_DEVICE_INFO_LIST* pstDevList)
int HikVisionCamera::InitDevice(void)
{
	int nRet = -1;

	MV_CC_DEVICE_INFO_LIST stDeviceList;
	memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

	nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE, &stDeviceList); // GigE 디바이스만 찾기
	if (MV_OK != nRet)
	{
		//MSG_ERROR(_T("MV_CC_EnumDevices fail! nRet [%x]\n"), nRet);
		return nRet;
	}

	if (!(stDeviceList.nDeviceNum > 0))
	{
		//MSG_ERROR(_T("Find No GIGE Devices!\n"));
		return nRet;
	}

	if (stDeviceList.nDeviceNum != 2 && stDeviceList.nDeviceNum != 4)
	{
		wchar_t messagebuffer[30] = { 0, };

		_stprintf(messagebuffer, L"Error : 카메라개수(%d)", stDeviceList.nDeviceNum);
		AfxMessageBox(messagebuffer);
		return nRet;
	}


	for (int i = 0; i < stDeviceList.nDeviceNum; i++)
	{
		// Initlize Camera
		void* pCamera = NULL;
		bool	bSucessInitlized = false;

		// Initlize UserPointer
		stVwUserPointer* pUserpointer = new stVwUserPointer;
		pUserpointer->ClassPointer = this;

		memset(pUserpointer->ImgBuffer, 0, CAMERA_WIDTH * CAMERA_HEIGHT);
		pUserpointer->unTimeStamp = 0;

		nRet = MV_CC_CreateHandle(&pCamera, stDeviceList.pDeviceInfo[i]); // 카메라 핸들 생성.
		if (MV_OK != nRet)
		{
			//MSG_ERROR(_T("MV_CC_CreateHandle fail! nRet [%x]\n"), nRet);
			//return FALSE;
			nRet = -1;
			break;
		}

		nRet = MV_CC_OpenDevice(pCamera); // 해당 핸들의 카메라 오픈
		if (MV_OK != nRet)
		{
			//MSG_ERROR(_T("MV_CC_OpenDevice fail! nRet [%x]\n"), nRet);
			//return FALSE;
			nRet = -1;
			break;
		}

		pUserpointer->CamIndex = i;

		if (nRet == MV_OK)
		{
			// Find Servername
			MVCC_STRINGVALUE pStringValue;
			nRet = MV_CC_GetStringValue(pCamera, "DeviceUserID", &pStringValue); // 디바이스 아이디 정보 가져오기
			if (nRet != MV_OK)
			{
				//MSG_ERROR(_T("DeviceUserID get fail! nRet [%x]\n"), nRet);
				//return FALSE;
				nRet = -1;
				break;
			}

			bSucessInitlized = true;

			int camID = 0;
			GetCamID(pCamera, camID);

			//데이터 복사
			memcpy(&m_stDevInfo[camID], stDeviceList.pDeviceInfo[i], sizeof(MV_CC_DEVICE_INFO));

			m_CameraHandle[camID] = pCamera;
			m_ImgBuffer[camID] = pUserpointer;

			nRet = MV_CC_SetIntValue(m_CameraHandle[camID], "GevSCPD", 2);  //이거 해야만 399fps 나옴
			if (MV_OK != nRet)
			{
				//return FALSE;
				nRet = -1;
				break;
			}

			nRet = MV_CC_SetImageNodeNum(m_CameraHandle[camID], 30); // 이미지 버퍼 설정
			if (MV_OK != nRet)
			{
				//return FALSE; nRet = -1; break;
			}

			nRet = MV_CC_RegisterImageCallBack(m_CameraHandle[camID], ImageCallback, pUserpointer);
			if (MV_OK != nRet)
			{
				//MSG_ERROR(_T("error: RegisterImageCallBack fail [%x]\n"), nRet);
				//return FALSE;
				nRet = -1;
				break;
			}

			if (!SetDefulatCameraSetting(camID))
			{
				//카메라값 설정 실패!, 이 카메라는 없는것으로 하자
				//bReturn = FALSE;
				bSucessInitlized = false;
				m_CameraHandle[camID] = NULL;
				m_ImgBuffer[camID] = NULL;
				//return FALSE;
				nRet = -1;
				break;
			}
		}
		else //result != RESULT_SUCCESS )
		{
			//bReturn = FALSE;
		}

		if (!bSucessInitlized)
		{
			if (pCamera)
				MV_CC_CloseDevice(pCamera);
			delete pUserpointer;
		}
	}

	return nRet;
}

void HikVisionCamera::ReleaseDevice(void)
{
	for (int i = 0; i < GZ_CAMERA_MAX; i++)
	{
		if (m_CameraHandle[i])
		{
			MV_CC_CloseDevice(m_CameraHandle[i]);
			MV_CC_DestroyHandle(m_CameraHandle[i]);

			delete m_ImgBuffer[i];

			m_CameraHandle[i] = NULL;
			m_ImgBuffer[i] = NULL;
		}
	}
}


float g_CurGamma = 0;
int	HikVisionCamera::StartDevice(eGzHandType handtype)
{
	if (m_bRunning && m_HandType != handtype)
		StopDevice();

	if (m_bRunning && m_HandType == handtype)
		return TRUE;

	m_HandType = handtype;

	{
		int camID = m_HandType == GZ_HANDTYPE_RIGHT ? 0 : 2;
		MVCC_FLOATVALUE CurExposureTime;
		MV_CC_GetFloatValue(m_CameraHandle[camID], "ExposureTime", &CurExposureTime);

		if (CurExposureTime.fCurValue == 2000.f)
		{
			MV_CC_SetFloatValue(m_CameraHandle[camID], "BlackLevel", 250.f);

			if (g_CurGamma != 0)
				MV_CC_SetFloatValue(m_CameraHandle[camID], "Gamma", g_CurGamma);
			else
				MV_CC_SetFloatValue(m_CameraHandle[camID], "Gamma", 0.75f);
		}
	}


	if (handtype == GZ_HANDTYPE_RIGHT)
	{
		if (m_CameraHandle[0] && m_CameraHandle[1])
		{
			MV_CC_SetCommandValue(m_CameraHandle[1], "GevTimestampControlReset");
			MV_CC_SetCommandValue(m_CameraHandle[0], "GevTimestampControlReset");

			if (MV_CC_StartGrabbing(m_CameraHandle[1]) != MV_OK)
			{
				return FALSE;
			}
			if (MV_CC_StartGrabbing(m_CameraHandle[0]) != MV_OK)
			{
				MV_CC_StopGrabbing(m_CameraHandle[1]);
				return FALSE;
			}
			m_bRunning = TRUE;
			return TRUE;
		}
	}
	else if (handtype == GZ_HANDTYPE_LEFT)
	{
		if (m_CameraHandle[2] && m_CameraHandle[3])
		{
			MV_CC_SetCommandValue(m_CameraHandle[3], "GevTimestampControlReset");
			MV_CC_SetCommandValue(m_CameraHandle[2], "GevTimestampControlReset");

			if (MV_CC_StartGrabbing(m_CameraHandle[3]) != MV_OK)
			{
				return FALSE;
			}
			if (MV_CC_StartGrabbing(m_CameraHandle[2]) != MV_OK)
			{
				MV_CC_StopGrabbing(m_CameraHandle[3]);
				return FALSE;
			}
			m_bRunning = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}


BOOL HikVisionCamera::StopDevice(void)
{
	if (m_HandType == GZ_HANDTYPE_RIGHT)
	{
		if (m_CameraHandle[0] && MV_CC_StopGrabbing(m_CameraHandle[0]) == MV_OK)
			m_ImgBuffer[0]->bFrameGrabbed = false;

		if (m_CameraHandle[1] && MV_CC_StopGrabbing(m_CameraHandle[1]) == MV_OK)
			m_ImgBuffer[1]->bFrameGrabbed = false;
	}
	else if (m_HandType == GZ_HANDTYPE_LEFT)
	{
		if (m_CameraHandle[2] && MV_CC_StopGrabbing(m_CameraHandle[2]) == MV_OK)
			m_ImgBuffer[2]->bFrameGrabbed = false;

		if (m_CameraHandle[3] && MV_CC_StopGrabbing(m_CameraHandle[3]) == MV_OK)
			m_ImgBuffer[3]->bFrameGrabbed = false;
	}

	m_bRunning = FALSE;
	return TRUE;
}


BOOL HikVisionCamera::lowBright_StartDevice(eGzHandType handtype)
{
	if (m_bRunning && m_HandType != handtype)
		StopDevice();

	if (m_bRunning && m_HandType == handtype)		//2024-10-29 이승훈 : 여기서는 파라메터 변경이 필요하므로 stop
		StopDevice();

	//2024-10-29 이승훈 : 파라메터 변경 (Side Only)
	{
		int CamIndex = 0;
		if (handtype == GZ_HANDTYPE_LEFT)
			CamIndex = 2;
		MVCC_FLOATVALUE CurGamma;
		MV_CC_GetFloatValue(m_CameraHandle[CamIndex], "Gamma", &CurGamma);
		g_CurGamma = CurGamma.fCurValue;

		//2024-11-29 이승훈 : 900mm용
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "ExposureTime", 150.f);
	}

	if (handtype == GZ_HANDTYPE_RIGHT)
	{
		if (m_CameraHandle[0] && m_CameraHandle[1])
		{
			MV_CC_SetCommandValue(m_CameraHandle[1], "GevTimestampControlReset");
			MV_CC_SetCommandValue(m_CameraHandle[0], "GevTimestampControlReset");

			if (MV_CC_StartGrabbing(m_CameraHandle[1]) != MV_OK)
			{
				return FALSE;
			}
			if (MV_CC_StartGrabbing(m_CameraHandle[0]) != MV_OK)
			{
				MV_CC_StopGrabbing(m_CameraHandle[1]);
				return FALSE;
			}
			m_bRunning = TRUE;
			return TRUE;
		}
	}
	else if (handtype == GZ_HANDTYPE_LEFT)
	{
		if (m_CameraHandle[2] && m_CameraHandle[3])
		{
			MV_CC_SetCommandValue(m_CameraHandle[3], "GevTimestampControlReset");
			MV_CC_SetCommandValue(m_CameraHandle[2], "GevTimestampControlReset");

			if (MV_CC_StartGrabbing(m_CameraHandle[3]) != MV_OK)
			{
				return FALSE;
			}
			if (MV_CC_StartGrabbing(m_CameraHandle[2]) != MV_OK)
			{
				MV_CC_StopGrabbing(m_CameraHandle[3]);
				return FALSE;
			}
			m_bRunning = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}


BOOL HikVisionCamera::highBright_StartDevice(eGzHandType handtype)
{
	if (m_bRunning && m_HandType != handtype)
		StopDevice();

	if (m_bRunning && m_HandType == handtype)		//2024-10-29 이승훈 : 여기서는 파라메터 변경이 필요하므로 stop
		StopDevice();

	//2024-10-29 이승훈 : 파라메터 변경 (Side Only)
	{
		int CamIndex = 0;
		if (handtype == GZ_HANDTYPE_LEFT)
			CamIndex = 2;
		MVCC_FLOATVALUE CurGamma;
		MV_CC_GetFloatValue(m_CameraHandle[CamIndex], "Gamma", &CurGamma);
		g_CurGamma = CurGamma.fCurValue;

		//2024-11-29 이승훈 : 1800mm 용
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "ExposureTime", 2000.f);
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "BlackLevel", 400.f);
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "Gamma", 0.5f);
	}

	if (handtype == GZ_HANDTYPE_RIGHT)
	{
		if (m_CameraHandle[0] && m_CameraHandle[1])
		{
			MV_CC_SetCommandValue(m_CameraHandle[1], "GevTimestampControlReset");
			MV_CC_SetCommandValue(m_CameraHandle[0], "GevTimestampControlReset");

			if (MV_CC_StartGrabbing(m_CameraHandle[1]) != MV_OK)
			{
				return FALSE;
			}
			if (MV_CC_StartGrabbing(m_CameraHandle[0]) != MV_OK)
			{
				MV_CC_StopGrabbing(m_CameraHandle[1]);
				return FALSE;
			}
			m_bRunning = TRUE;
			return TRUE;
		}
	}
	else if (handtype == GZ_HANDTYPE_LEFT)
	{
		if (m_CameraHandle[2] && m_CameraHandle[3])
		{
			MV_CC_SetCommandValue(m_CameraHandle[3], "GevTimestampControlReset");
			MV_CC_SetCommandValue(m_CameraHandle[2], "GevTimestampControlReset");

			if (MV_CC_StartGrabbing(m_CameraHandle[3]) != MV_OK)
			{
				return FALSE;
			}
			if (MV_CC_StartGrabbing(m_CameraHandle[2]) != MV_OK)
			{
				MV_CC_StopGrabbing(m_CameraHandle[3]);
				return FALSE;
			}
			m_bRunning = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}


BOOL HikVisionCamera::lowBright_StopDevice(void)
{
	if (m_HandType == GZ_HANDTYPE_RIGHT)
	{
		if (m_CameraHandle[0] && MV_CC_StopGrabbing(m_CameraHandle[0]) == MV_OK)
			m_ImgBuffer[0]->bFrameGrabbed = false;

		if (m_CameraHandle[1] && MV_CC_StopGrabbing(m_CameraHandle[1]) == MV_OK)
			m_ImgBuffer[1]->bFrameGrabbed = false;
	}
	else if (m_HandType == GZ_HANDTYPE_LEFT)
	{
		if (m_CameraHandle[2] && MV_CC_StopGrabbing(m_CameraHandle[2]) == MV_OK)
			m_ImgBuffer[2]->bFrameGrabbed = false;

		if (m_CameraHandle[3] && MV_CC_StopGrabbing(m_CameraHandle[3]) == MV_OK)
			m_ImgBuffer[3]->bFrameGrabbed = false;
	}

	//2024-10-29 이승훈 : 파라메터 변경 (Side Only)
	{
		int CamIndex = 0;
		if (m_HandType == GZ_HANDTYPE_LEFT)
			CamIndex = 2;

		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "ExposureTime", 250.f);
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "BlackLevel", 250.f);
		if (g_CurGamma != 0)
			MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "Gamma", g_CurGamma);
		else
			MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "Gamma", 0.75f);
	}

	m_bRunning = FALSE;
	return TRUE;
}

BOOL HikVisionCamera::highBright_StopDevice(void)
{
	if (m_HandType == GZ_HANDTYPE_RIGHT)
	{
		if (m_CameraHandle[0] && MV_CC_StopGrabbing(m_CameraHandle[0]) == MV_OK)
			m_ImgBuffer[0]->bFrameGrabbed = false;

		if (m_CameraHandle[1] && MV_CC_StopGrabbing(m_CameraHandle[1]) == MV_OK)
			m_ImgBuffer[1]->bFrameGrabbed = false;
	}
	else if (m_HandType == GZ_HANDTYPE_LEFT)
	{
		if (m_CameraHandle[2] && MV_CC_StopGrabbing(m_CameraHandle[2]) == MV_OK)
			m_ImgBuffer[2]->bFrameGrabbed = false;

		if (m_CameraHandle[3] && MV_CC_StopGrabbing(m_CameraHandle[3]) == MV_OK)
			m_ImgBuffer[3]->bFrameGrabbed = false;
	}

	//2024-10-29 이승훈 : 파라메터 변경 (Side Only)
	{
		int CamIndex = 0;
		if (m_HandType == GZ_HANDTYPE_LEFT)
			CamIndex = 2;

		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "ExposureTime", 250.f);
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "BlackLevel", 250.f);
		if (g_CurGamma != 0)
			MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "Gamma", g_CurGamma);
		else
			MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "Gamma", 0.75f);
	}

	m_bRunning = FALSE;
	return TRUE;
}






BOOL HikVisionCamera::SetDefulatCameraSetting(int CamIndex)
{
	UINT unPixelSize = 0;

	MVCC_INTVALUE nWidth;
	MVCC_INTVALUE nHeight;

	MV_CC_GetWidth(m_CameraHandle[CamIndex], &nWidth);
	MV_CC_GetHeight(m_CameraHandle[CamIndex], &nHeight);


	//// Set Width
	MVCC_INTVALUE nInputWidth;
	nInputWidth.nCurValue = CAMERA_WIDTH;

	if (FALSE == SetWidthCamera(CamIndex, nInputWidth.nCurValue))
	{
		return FALSE;
	}

	//// Set Height
	MVCC_INTVALUE nInputHeight;
	nInputHeight.nCurValue = CAMERA_HEIGHT;

	if (FALSE == SetHeightCamera(CamIndex, nInputHeight.nCurValue))
	{
		return FALSE;
	}

	MV_CC_GetWidth(m_CameraHandle[CamIndex], &nInputWidth);
	MV_CC_GetHeight(m_CameraHandle[CamIndex], &nInputHeight);

	CString strVendor;
	strVendor = m_stDevInfo[CamIndex].SpecialInfo.stGigEInfo.chManufacturerName;

	unsigned int sdkversion = 0;
	sdkversion = MV_CC_GetSDKVersion();

	if (strVendor.Compare(_T("Golfzon2")) == 0)
	{
		if (sdkversion != 50594304)
		{
			CString strVesionError;

			strVesionError.Format(_T("cam[%d] SDK Version : %s(%x), vendor : %s"), CamIndex, sdkversion == 33685510 ? _T("v2.2.3") : sdkversion == 50594304 ? _T("v3.3.1") : _T("Unknown"), sdkversion, strVendor);
			AfxMessageBox(strVesionError);
			AfxMessageBox(_T("Please install sdk 3.3.1"));
			return FALSE;
		}
	}

	MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "ExposureTime", 400.0f);

	if (strVendor.Compare(_T("Golfzon2")) == 0)
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "ExposureTime", 250.f);

	if (CamIndex == 0 || CamIndex == 2)	// rr, ll
	{
		// Gain Selector를 변경
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "Gain", 2.5f);

		if (strVendor.Compare(_T("Golfzon2")) == 0)
			MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "Gain", 0.f);
		//LineInverter 1
		//TriggerMode : off
		// TriggerDelay 0

		MV_CC_SetEnumValue(m_CameraHandle[CamIndex], "TriggerMode", MV_TRIGGER_MODE_OFF);
		MV_CC_SetEnumValue(m_CameraHandle[CamIndex], "TriggerSource", 2);
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "TriggerDelay", 0.0f);

		//Keeworks 17.10.25 Kei
		MV_CC_SetEnumValue(m_CameraHandle[CamIndex], "LineSelector", 1);  //스트로브 동작시 스트로브 신호 출력 라인 설정 : Line1
		MV_CC_SetEnumValue(m_CameraHandle[CamIndex], "LineMode", 8);   //라인의 모드를 스트로브로 설정한다 : 8번 -> Strobe Mode
		MV_CC_SetBoolValue(m_CameraHandle[CamIndex], "LineInverter", 1);  //LineInverter Enable(마스터 카메라에만 적용)
		MV_CC_SetBoolValue(m_CameraHandle[CamIndex], "StrobeEnable", 1);  //Strobe Enable (스트로브 출력을 실행)

		MV_CC_SetEnumValue(m_CameraHandle[CamIndex], "LineSelector", 2);
		MV_CC_SetIntValue(m_CameraHandle[CamIndex], "LineDebouncerTime", 60);

		MV_CC_SetBoolValue(m_CameraHandle[CamIndex], "AcquisitionFrameRateEnable", 1);  //Enable (카메라 Frame Rate를 지정된 값으로 고정)
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "AcquisitionFrameRate", 400.0f);

		if (CamIndex == 0)
			MV_CC_SetBoolValue(m_CameraHandle[CamIndex], "ReverseX", false);
		else
			MV_CC_SetBoolValue(m_CameraHandle[CamIndex], "ReverseX", true);
	}
	if (CamIndex == 1 || CamIndex == 3) // rt, lt
	{
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "AcquisitionFrameRate", 400.0f);
		// Gain Selector를 변경
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "Gain", 6.0f);

		if (strVendor.Compare(_T("Golfzon2")) == 0)
			MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "Gain", 0.f);

		MV_CC_SetBoolValue(m_CameraHandle[CamIndex], "GammaEnable", 1); //1:enable 2:disable
		MV_CC_SetEnumValue(m_CameraHandle[CamIndex], "GammaSelector", 1); //1:User 2:sRGB
		//MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "Gamma", 0.8);

		//LineInverter 1
		//TriggerMode : on
		// TriggerDelay : 100

		MV_CC_SetEnumValue(m_CameraHandle[CamIndex], "TriggerMode", MV_TRIGGER_MODE_ON);
		// TriggerDelay
		//Keeworks 2017.10.25 Kei
		MV_CC_SetEnumValue(m_CameraHandle[CamIndex], "TriggerSource", 2);  //트리거 입력 포트 : 2 -> Line2
		MV_CC_SetFloatValue(m_CameraHandle[CamIndex], "TriggerDelay", 0.0f);
		//Keeworks 17.10.25 Kei
		MV_CC_SetEnumValue(m_CameraHandle[CamIndex], "LineSelector", 1);  //스트로브 동작시 스트로브 신호 출력 라인 설정 : Line1
		MV_CC_SetEnumValue(m_CameraHandle[CamIndex], "LineMode", 8);   //라인의 모드를 스트로브로 설정한다 : 8번 -> Strobe Mode
		MV_CC_SetBoolValue(m_CameraHandle[CamIndex], "LineInverter", 0);  //LineInverter Disable(Slave 카메라에만 적용)
		MV_CC_SetBoolValue(m_CameraHandle[CamIndex], "StrobeEnable", 1);  //Strobe Enable (스트로브 출력을 실행)

		MV_CC_SetEnumValue(m_CameraHandle[CamIndex], "LineSelector", 2);  //Line2
		MV_CC_SetIntValue(m_CameraHandle[CamIndex], "LineDebouncerTime", 60);

		MV_CC_SetBoolValue(m_CameraHandle[CamIndex], "AcquisitionFrameRateEnable", 0);  //Disable (카메라 Frame Rate를 지정된 값으로 고정) //테스트 후 적용 필요

		if (CamIndex == 1)
			MV_CC_SetBoolValue(m_CameraHandle[CamIndex], "ReverseX", false);
		else
			MV_CC_SetBoolValue(m_CameraHandle[CamIndex], "ReverseX", true);
	}

	MV_CC_SetCommandValue(m_CameraHandle[CamIndex], "GevTimestampControlReset");

	return TRUE;
}

BOOL HikVisionCamera::SetWidthCamera(int index, int nWidth)
{
	MVCC_INTVALUE CurrWidth;

	if (MV_OK != MV_CC_GetWidth(m_CameraHandle[index], &CurrWidth))
	{
		// ERROR
		return FALSE;
	}

	if (nWidth != CurrWidth.nCurValue)
	{
		MV_CC_SetWidth(m_CameraHandle[index], nWidth);


		if (nWidth % 4 != 0) // 이미지 너비가 4의 배수 인지 체크
		{
			AfxMessageBox(_T("Width must be a multiple of 4!"));
			return FALSE;
		}
	}
	return TRUE;
}

BOOL HikVisionCamera::SetHeightCamera(int index, int nHeight)
{
	MVCC_INTVALUE CurrHeight;
	if (MV_OK != MV_CC_GetHeight(m_CameraHandle[index], &CurrHeight))
	{
		// ERROR
		return FALSE;
	}

	if (nHeight != CurrHeight.nCurValue)
	{
		MV_CC_SetHeight(m_CameraHandle[index], nHeight);

		if (nHeight % 2 != 0) // 이미지 세로가 2의 배수 인지 체크
		{
			AfxMessageBox(_T("Height must be a multiple of 2!"));
			return FALSE;
		}
	}

	return TRUE;
}

BOOL HikVisionCamera::GetCamID(void* handle, int& nCamId)
{
	int nRet = MV_OK;
	MVCC_STRINGVALUE pStringValue;
	nRet = MV_CC_GetStringValue(handle, "DeviceUserID", &pStringValue); // 디바이스 아이디 정보 가져오기
	if (nRet != MV_OK)
	{
		return FALSE;
	}

	BOOL bCamFound = FALSE;
	for (int iCamOrder = 0; iCamOrder < 4/*m_nNumCameras*/; iCamOrder++)
	{
		if (!strcmp(pStringValue.chCurValue, HIKVISION_CAMID_TABLE[iCamOrder]))
		{
			//m_AcqDevice[iCamOrder] = pVwCamera;
			nCamId = iCamOrder;
			bCamFound = TRUE;
		}
	}
	return bCamFound;
}


BOOL	HikVisionCamera::SetExposureTime(int nCamID, float value)
{
	if (m_CameraHandle[nCamID])
	{
		MV_CC_SetFloatValue(m_CameraHandle[nCamID], "ExposureTime", value);
		return TRUE;
	}

	return FALSE;
}

BOOL	HikVisionCamera::GetExposureTime(int nCamID, float* value)
{
	if (m_CameraHandle[nCamID])
	{
		MVCC_FLOATVALUE CurExposureTime;
		MV_CC_GetFloatValue(m_CameraHandle[nCamID], "ExposureTime", &CurExposureTime);
		*value = CurExposureTime.fCurValue;
		return TRUE;
	}
	return FALSE;
}

BOOL	HikVisionCamera::SetGain(int nCamID, float value)
{
	/*1~3.5*/

	if (m_CameraHandle[nCamID])
	{
		float gainvalue = (float)value / 10;
		MV_CC_SetFloatValue(m_CameraHandle[nCamID], "Gain", gainvalue);
		return TRUE;
	}
	return FALSE;
}

BOOL	HikVisionCamera::GetGain(int nCamID, float* value)
{
	if (m_CameraHandle[nCamID])
	{
		MVCC_FLOATVALUE CurGain;
		MV_CC_GetFloatValue(m_CameraHandle[nCamID], "Gain", &CurGain);
		*value = CurGain.fCurValue * 10;
		return TRUE;
	}

	return FALSE;
}

BOOL	HikVisionCamera::SetFPS(int nCamID, float value)
{
	if (m_CameraHandle[nCamID])
	{
		MV_CC_SetFloatValue(m_CameraHandle[nCamID], "AcquisitionFrameRate", value);
		return TRUE;
	}
	return FALSE;
}

BOOL	HikVisionCamera::GetFPS(int nCamID, float* value)
{
	if (m_CameraHandle[nCamID])
	{
		MVCC_FLOATVALUE CurAcquisitionFrameRate;
		MV_CC_GetFloatValue(m_CameraHandle[nCamID], "AcquisitionFrameRate", &CurAcquisitionFrameRate);
		*value = CurAcquisitionFrameRate.fCurValue;
		return TRUE;
	}
	return FALSE;
}
BOOL	HikVisionCamera::GetTemperature(int nCamID, float* value)
{
	double dValue;
	if (m_CameraHandle[nCamID])
	{
		MVCC_FLOATVALUE CurDeviceTemperature;
		MV_CC_GetFloatValue(m_CameraHandle[nCamID], "DeviceTemperature", &CurDeviceTemperature);
		*value = CurDeviceTemperature.fCurValue;

		return TRUE;
	}

	return FALSE;
}


int		HikVisionCamera::GetMaxGain(void)
{
	return 100;
}

int		HikVisionCamera::GetMinGain(void)
{
	return 10;
}

int		HikVisionCamera::GetMaxExposure(void)
{
	return 2000;
}
int		HikVisionCamera::GetMinExposure(void)
{
	return 180;	//2024-11-29 이승훈 : 캘리브레이션을 위해 min값 수정
}

int		HikVisionCamera::GetSerialNumber(int nCamID, char* value)
{
	strcpy(value, "NULL");
	if (m_CameraHandle[nCamID] && value)
	{
		MVCC_STRINGVALUE pStringValue;
		MV_CC_GetStringValue(m_CameraHandle[nCamID], "DeviceID", &pStringValue); // 디바이스 아이디 정보 가져오기

		strcpy(value, pStringValue.chCurValue);

		return TRUE;
	}

	return FALSE;
}

int		HikVisionCamera::GetMacAddress(int nCamID, char* value)
{
	strcpy(value, "00:00:00:00:00:00");
	if (m_CameraHandle[nCamID] && value)
	{
		MVCC_INTVALUE  pIntValue;
		MV_CC_GetIntValue(m_CameraHandle[nCamID], "GevMACAddress", &pIntValue); // 디바이스 아이디 정보 가져오기

		long macvalue = pIntValue.nCurValue;

		char buffer[30] = { 0, };
		memset(buffer, 0, sizeof(buffer));

		sprintf(buffer, "C42F%08X", macvalue);
		memset(value, 0, sizeof(char) * 30);

		for (int i = 0; i < 6; i++)
		{
			value[i * 3 + 0] = buffer[i * 2 + 0];
			value[i * 3 + 1] = buffer[i * 2 + 1];
			value[i * 3 + 2] = ':';
		}
		value[17] = 0;

		return TRUE;
	}

	return FALSE;
}

int HikVisionCamera::GetServerID(int nCameraID)
{
	//	int nServerID = -1;
	//	char strServerName[20];
	//	int nNumCameras;
	//	char strServerUserName[20];
	//	VwGigE* pvwGigE = NULL;
	//	RESULT	result = RESULT_ERROR;
	//
	//	int bIsLocal = false;
	//
	//	if(m_pvwGigE != NULL)
	//	{
	//		pvwGigE = m_pvwGigE;
	//		bIsLocal = false;
	//	}
	//	else
	//	{
	//		pvwGigE = new VwGigE;
	//		bIsLocal = true;
	//	}
	//
	//	pvwGigE->Open();
	//
	//	UINT numCameras;
	//	pvwGigE->GetNumCameras(&numCameras);
	//
	//	for (int i = 0; i < numCameras; i++)
	//	{
	//		VwCamera *pCamera = NULL;
	//		OBJECT_INFO objectInfo;
	//		result = pvwGigE->OpenCamera((UINT)i/*m_radioDevice*/,
	//								&pCamera,
	//								2/*m_imagebuffernumber*/,
	//								0,
	//								0,
	//								0,
	//								&objectInfo,
	//								NULL );
	//
	//		if( result != RESULT_SUCCESS )
	//		{
	//			continue;
	//		}
	//
	//
	//		int camID = 0;
	//		GetCamID(pCamera, camID);
	//		if (camID == nCameraID)
	//		{
	//			nServerID = i;
	//			pCamera->CloseCamera();
	//			break;
	//		}
	//
	//		if ( pCamera->CloseCamera() == RESULT_SUCCESS )
	//		{
	//			pCamera = NULL;
	//		}
	//	}
	//
	//	if( bIsLocal == true && pvwGigE )
	//	{
	//		pvwGigE->Close();
	//		delete pvwGigE;
	//		pvwGigE = NULL;
	//	}
	//
	//	return nServerID;
	return 1;
}














#pragma region HikVisionSimpleSDK


int HikVisionCamera::CommandExecute(IN const char* strKey)
{
	return MV_CC_SetCommandValue(m_hDevHandle, strKey);
}

int HikVisionCamera::RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser), void* pUser)
{
	return MV_CC_RegisterExceptionCallBack(m_hDevHandle, cbException, pUser);
}

int HikVisionCamera::RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO* pEventInfo, void* pUser), void* pUser)
{
	//return MV_CC_RegisterEventCallBackEx(m_hDevHandle, pEventName, cbEvent, pUser);

	int nRet = MV_CC_RegisterEventCallBackEx(m_hDevHandle, pEventName, cbEvent, pUser);
	if (MV_OK != nRet)
	{
		printf("Register Event CallBack fail[%x]\n", nRet);
	}

	return nRet;
}

int HikVisionCamera::ForceIp(unsigned int nIP, unsigned int nSubNetMask, unsigned int nDefaultGateWay)
{
	return MV_GIGE_ForceIpEx(m_hDevHandle, nIP, nSubNetMask, nDefaultGateWay);
}

int HikVisionCamera::SetIpConfig(unsigned int nType)
{
	return MV_GIGE_SetIpConfig(m_hDevHandle, nType);
}

int HikVisionCamera::SetNetTransMode(unsigned int nType)
{
	return MV_GIGE_SetNetTransMode(m_hDevHandle, nType);
}


int HikVisionCamera::GetSDKVersion()
{
	return MV_CC_GetSDKVersion();
}

int HikVisionCamera::EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList)
{
	return MV_CC_EnumDevices(nTLayerType, pstDevList);
}

bool HikVisionCamera::IsDeviceAccessible(MV_CC_DEVICE_INFO* pstDevInfo, unsigned int nAccessMode)
{
	return MV_CC_IsDeviceAccessible(pstDevInfo, nAccessMode);
}



int HikVisionCamera::CreateHandle(MV_CC_DEVICE_INFO* pstDeviceInfo)
{
	int nRet = -1;
	m_hDevHandle = NULL;

	//Enumerate all the corresponding devices of the specified transport protocols within the subnet.
	unsigned int nTLayerType = MV_GIGE_DEVICE | MV_USB_DEVICE;
	MV_CC_DEVICE_INFO_LIST m_stDevList = { 0 };
	int nRet = MV_CC_EnumDevices(nTLayerType, &m_stDevList);
	if (MV_OK != nRet)
	{
		printf("error: EnumDevices fail [%x]\n", nRet);
		return;
	}

	int i = 0;
	if (m_stDevList.nDeviceNum == 0)
	{
		printf("no camera found!\n");
		return;
	}

	//Select the searched first online device, create a device handle
	int nDeviceIndex = 0;
	MV_CC_DEVICE_INFO pstDeviceInfo = { 0 };
	memcpy(&pstDeviceInfo, m_stDevList.pDeviceInfo[nDeviceIndex], sizeof(MV_CC_DEVICE_INFO));
	nRet = MV_CC_CreateHandle(&m_hDevHandle, pstDeviceInfo);
	if (MV_OK != nRet)
	{
		printf("error: CreateHandle fail [%x]\n", nRet);
		return;
	}
}


int HikVisionCamera::Open(MV_CC_DEVICE_INFO* pstDeviceInfo)
{
	if (MV_NULL == pstDeviceInfo)
	{
		return MV_E_PARAMETER;
	}

	if (m_hDevHandle)
	{
		return MV_E_CALLORDER;
	}

	int nRet = MV_CC_CreateHandle(&m_hDevHandle, pstDeviceInfo);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	nRet = MV_CC_OpenDevice(m_hDevHandle);
	if (MV_OK != nRet)
	{
		MV_CC_DestroyHandle(m_hDevHandle);
		m_hDevHandle = MV_NULL;
	}

	return nRet;
}

int HikVisionCamera::Close()
{
	if (MV_NULL == m_hDevHandle)
	{
		return MV_E_HANDLE;
	}

	MV_CC_CloseDevice(m_hDevHandle);

	int nRet = MV_CC_DestroyHandle(m_hDevHandle);
	m_hDevHandle = MV_NULL;

	return nRet;
}

bool HikVisionCamera::IsDeviceConnected()
{
	return MV_CC_IsDeviceConnected(m_hDevHandle);
}

int HikVisionCamera::RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser)
{
	return MV_CC_RegisterImageCallBackEx(m_hDevHandle, cbOutput, pUser);
}

int HikVisionCamera::StartGrabbing()
{
	return MV_CC_StartGrabbing(m_hDevHandle);
}

int HikVisionCamera::StopGrabbing()
{
	return MV_CC_StopGrabbing(m_hDevHandle);
}

int HikVisionCamera::GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec)
{
	return MV_CC_GetImageBuffer(m_hDevHandle, pFrame, nMsec);
}

int HikVisionCamera::FreeImageBuffer(MV_FRAME_OUT* pFrame)
{
	return MV_CC_FreeImageBuffer(m_hDevHandle, pFrame);
}

int HikVisionCamera::DisplayOneFrame(MV_DISPLAY_FRAME_INFO* pDisplayInfo)
{
	return MV_CC_DisplayOneFrame(m_hDevHandle, pDisplayInfo);
}

int HikVisionCamera::SetImageNodeNum(unsigned int nNum)
{
	return MV_CC_SetImageNodeNum(m_hDevHandle, nNum);
}

int HikVisionCamera::GetDeviceInfo(MV_CC_DEVICE_INFO* pstDevInfo)
{
	return MV_CC_GetDeviceInfo(m_hDevHandle, pstDevInfo);
}

int HikVisionCamera::GetGevAllMatchInfo(MV_MATCH_INFO_NET_DETECT* pMatchInfoNetDetect)
{
	if (MV_NULL == pMatchInfoNetDetect)
	{
		return MV_E_PARAMETER;
	}

	MV_CC_DEVICE_INFO stDevInfo = { 0 };
	GetDeviceInfo(&stDevInfo);
	if (stDevInfo.nTLayerType != MV_GIGE_DEVICE)
	{
		return MV_E_SUPPORT;
	}

	MV_ALL_MATCH_INFO struMatchInfo = { 0 };

	struMatchInfo.nType = MV_MATCH_TYPE_NET_DETECT;
	struMatchInfo.pInfo = pMatchInfoNetDetect;
	struMatchInfo.nInfoSize = sizeof(MV_MATCH_INFO_NET_DETECT);
	memset(struMatchInfo.pInfo, 0, sizeof(MV_MATCH_INFO_NET_DETECT));

	return MV_CC_GetAllMatchInfo(m_hDevHandle, &struMatchInfo);
}

int HikVisionCamera::GetU3VAllMatchInfo(MV_MATCH_INFO_USB_DETECT* pMatchInfoUSBDetect)
{
	if (MV_NULL == pMatchInfoUSBDetect)
	{
		return MV_E_PARAMETER;
	}

	MV_CC_DEVICE_INFO stDevInfo = { 0 };
	GetDeviceInfo(&stDevInfo);
	if (stDevInfo.nTLayerType != MV_USB_DEVICE)
	{
		return MV_E_SUPPORT;
	}

	MV_ALL_MATCH_INFO struMatchInfo = { 0 };

	struMatchInfo.nType = MV_MATCH_TYPE_USB_DETECT;
	struMatchInfo.pInfo = pMatchInfoUSBDetect;
	struMatchInfo.nInfoSize = sizeof(MV_MATCH_INFO_USB_DETECT);
	memset(struMatchInfo.pInfo, 0, sizeof(MV_MATCH_INFO_USB_DETECT));

	return MV_CC_GetAllMatchInfo(m_hDevHandle, &struMatchInfo);
}

int HikVisionCamera::GetIntValue(IN const char* strKey, OUT MVCC_INTVALUE_EX* pIntValue)
{
	return MV_CC_GetIntValueEx(m_hDevHandle, strKey, pIntValue);
}

int HikVisionCamera::SetIntValue(IN const char* strKey, IN int64_t nValue)
{
	return MV_CC_SetIntValueEx(m_hDevHandle, strKey, nValue);
}

int HikVisionCamera::GetEnumValue(IN const char* strKey, OUT MVCC_ENUMVALUE* pEnumValue)
{
	return MV_CC_GetEnumValue(m_hDevHandle, strKey, pEnumValue);
}

int HikVisionCamera::SetEnumValue(IN const char* strKey, IN unsigned int nValue)
{
	return MV_CC_SetEnumValue(m_hDevHandle, strKey, nValue);
}

int HikVisionCamera::SetEnumValueByString(IN const char* strKey, IN const char* sValue)
{
	return MV_CC_SetEnumValueByString(m_hDevHandle, strKey, sValue);
}

int HikVisionCamera::GetFloatValue(IN const char* strKey, OUT MVCC_FLOATVALUE* pFloatValue)
{
	return MV_CC_GetFloatValue(m_hDevHandle, strKey, pFloatValue);
}

int HikVisionCamera::SetFloatValue(IN const char* strKey, IN float fValue)
{
	return MV_CC_SetFloatValue(m_hDevHandle, strKey, fValue);
}

int HikVisionCamera::GetBoolValue(IN const char* strKey, OUT bool* pbValue)
{
	return MV_CC_GetBoolValue(m_hDevHandle, strKey, pbValue);
}

int HikVisionCamera::SetBoolValue(IN const char* strKey, IN bool bValue)
{
	return MV_CC_SetBoolValue(m_hDevHandle, strKey, bValue);
}

int HikVisionCamera::GetStringValue(IN const char* strKey, MVCC_STRINGVALUE* pStringValue)
{
	return MV_CC_GetStringValue(m_hDevHandle, strKey, pStringValue);
}

int HikVisionCamera::SetStringValue(IN const char* strKey, IN const char* strValue)
{
	return MV_CC_SetStringValue(m_hDevHandle, strKey, strValue);
}

int HikVisionCamera::GetOptimalPacketSize(unsigned int* pOptimalPacketSize)
{
	if (MV_NULL == pOptimalPacketSize)
	{
		return MV_E_PARAMETER;
	}

	int nRet = MV_CC_GetOptimalPacketSize(m_hDevHandle);
	if (nRet < MV_OK)
	{
		return nRet;
	}

	*pOptimalPacketSize = (unsigned int)nRet;

	return MV_OK;
}

int HikVisionCamera::ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM* pstCvtParam)
{
	return MV_CC_ConvertPixelType(m_hDevHandle, pstCvtParam);
}

int HikVisionCamera::SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam)
{
	return MV_CC_SaveImageEx2(m_hDevHandle, pstParam);
}

int HikVisionCamera::SaveImageToFile(MV_SAVE_IMG_TO_FILE_PARAM* pstSaveFileParam)
{
	return MV_CC_SaveImageToFile(m_hDevHandle, pstSaveFileParam);
}

#pragma endregion HikVisionSimpleSDK