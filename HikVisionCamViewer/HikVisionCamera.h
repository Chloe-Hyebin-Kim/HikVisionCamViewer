#pragma once

#include "MvCameraControl.h"

#ifndef MV_NULL
#define MV_NULL    0
#endif

class HikVisionCamera
{
public:
	HikVisionCamera();
	~HikVisionCamera();

private:
	void* m_hDevHandle;

public:
	static int GetSDKVersion();

	static int EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList);

	static bool IsDeviceAccessible(MV_CC_DEVICE_INFO* pstDevInfo, unsigned int nAccessMode);

	int Open(MV_CC_DEVICE_INFO* pstDeviceInfo);

	//int CreateHandle(MV_CC_DEVICE_INFO* pstDeviceInfo);
	//int OpenDevice(MV_CC_DEVICE_INFO* pstDeviceInfo);

	int Close();

	bool IsDeviceConnected();

	int RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser);

	int StartGrabbing();

	int StopGrabbing();

	int GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec);

	int FreeImageBuffer(MV_FRAME_OUT* pFrame);

	int DisplayOneFrame(MV_DISPLAY_FRAME_INFO* pDisplayInfo);

	int SetImageNodeNum(unsigned int nNum);

	int GetDeviceInfo(MV_CC_DEVICE_INFO* pstDevInfo);

	int GetGevAllMatchInfo(MV_MATCH_INFO_NET_DETECT* pMatchInfoNetDetect);

	int GetU3VAllMatchInfo(MV_MATCH_INFO_USB_DETECT* pMatchInfoUSBDetect);

	int GetIntValue(IN const char* strKey, OUT MVCC_INTVALUE_EX* pIntValue);
	int SetIntValue(IN const char* strKey, IN int64_t nValue);

	int GetEnumValue(IN const char* strKey, OUT MVCC_ENUMVALUE* pEnumValue);
	int SetEnumValue(IN const char* strKey, IN unsigned int nValue);
	int SetEnumValueByString(IN const char* strKey, IN const char* sValue);

	int GetFloatValue(IN const char* strKey, OUT MVCC_FLOATVALUE* pFloatValue);
	int SetFloatValue(IN const char* strKey, IN float fValue);

	int GetBoolValue(IN const char* strKey, OUT bool* pbValue);
	int SetBoolValue(IN const char* strKey, IN bool bValue);

	int GetStringValue(IN const char* strKey, MVCC_STRINGVALUE* pStringValue);
	int SetStringValue(IN const char* strKey, IN const char* strValue);

	int CommandExecute(IN const char* strKey);

	int GetOptimalPacketSize(unsigned int* pOptimalPacketSize);

	int RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser), void* pUser);

	int RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO* pEventInfo, void* pUser), void* pUser);

	int ForceIp(unsigned int nIP, unsigned int nSubNetMask, unsigned int nDefaultGateWay);

	int SetIpConfig(unsigned int nType);

	int SetNetTransMode(unsigned int nType);

	int ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM* pstCvtParam);

	int SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam);

	int SaveImageToFile(MV_SAVE_IMG_TO_FILE_PARAM* pstSaveFileParam);
};