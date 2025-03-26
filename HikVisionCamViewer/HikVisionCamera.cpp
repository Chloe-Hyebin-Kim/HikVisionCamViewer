#include "HikVisionCamera.h"


HikVisionCamera::HikVisionCamera()
{
	m_hDevHandle = MV_NULL;
}

HikVisionCamera::~HikVisionCamera()
{
	if (m_hDevHandle)
	{
		MV_CC_DestroyHandle(m_hDevHandle);
		m_hDevHandle = MV_NULL;
	}
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

int HikVisionCamera::CommandExecute(IN const char* strKey)
{
	return MV_CC_SetCommandValue(m_hDevHandle, strKey);
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

int HikVisionCamera::RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser), void* pUser)
{
	return MV_CC_RegisterExceptionCallBack(m_hDevHandle, cbException, pUser);
}

int HikVisionCamera::RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO* pEventInfo, void* pUser), void* pUser)
{
	return MV_CC_RegisterEventCallBackEx(m_hDevHandle, pEventName, cbEvent, pUser);
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