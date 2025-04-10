#pragma once

#include "MvCameraControl.h"

#include "stdafx.h" // 완성하면 없어도 될듯
#include "GzCamLinkHeader.h"

#ifndef MV_NULL
#define MV_NULL    0
#endif

struct stVwUserPointer
{
	unsigned char ImgBuffer[CAMERA_WIDTH * CAMERA_HEIGHT];
	UINT64			unTimeStamp;
	void* ClassPointer;
	int			CamIndex;
	bool		bFrameGrabbed;
	//	char		strServerMacAddress[30];

	stVwUserPointer()
	{
		bFrameGrabbed = false;
		ClassPointer = NULL;
		unTimeStamp = 0;
	}
};

class HikVisionCamera
{
public:
	HikVisionCamera();
	~HikVisionCamera();

private:
	eGzHandType	m_HandType;
	bool		m_bRunning;

public:
	void* m_CameraHandle[GZ_CAMERA_MAX]; // [이명진] : 카메라 핸들.
	static int camtable[GZ_CAMERA_MAX]; // [이명진] : camtable 변수 static 멤버로 수정
	MV_CC_DEVICE_INFO m_stDevInfo[GZ_CAMERA_MAX];
	stVwUserPointer* m_ImgBuffer[GZ_CAMERA_MAX];

public:
	static void __stdcall ImageCallback(unsigned char* pData, MV_FRAME_OUT_INFO* pFrameInfo, void* pUser);  //Image capture event

	int InitDevice(void);//int Initialize(MV_CC_DEVICE_INFO_LIST* pstDevList);
	void ReleaseDevice(void);//void Release(void);

	int StartDevice(eGzHandType handtype);
	BOOL StopDevice(void);

	BOOL lowBright_StartDevice(eGzHandType handtype);
	BOOL lowBright_StopDevice(void);
	BOOL highBright_StartDevice(eGzHandType handtype);
	BOOL highBright_StopDevice(void);

	BOOL SetDefulatCameraSetting(int CamIndex);
	BOOL SetWidthCamera(int index, int nWidth);
	BOOL SetHeightCamera(int index, int nHeight);


	BOOL	SetExposureTime(int nCamID, float value);
	BOOL	GetExposureTime(int nCamID, float* value);

	BOOL	SetGain(int nCamID, float value);
	BOOL	GetGain(int nCamID, float* value);

	BOOL	SetFPS(int nCamID, float value);
	BOOL	GetFPS(int nCamID, float* value);

	BOOL	GetTemperature(int nCamID, float* value);

	int		GetMaxGain(void);
	int		GetMinGain(void);

	int		GetMaxExposure(void);
	int		GetMinExposure(void);

	int		GetServerID(int nCameraID);

	int		GetSerialNumber(int nCamID, char* value);
	int		GetMacAddress(int nCamID, char* value);

private:
	BOOL GetCamID(void* handle, int& nCamId);


#pragma region HikVisionSimpleSDK

private:
	void* m_hDevHandle;

public:
	static int GetSDKVersion();

	static int EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList);

	static bool IsDeviceAccessible(MV_CC_DEVICE_INFO* pstDevInfo, unsigned int nAccessMode);

	int Open(MV_CC_DEVICE_INFO* pstDeviceInfo);

	int CreateHandle(MV_CC_DEVICE_INFO* pstDeviceInfo);
	//int OpenDevice(MV_CC_DEVICE_INFO* pstDeviceInfo);

	int Close();

	bool IsDeviceConnected();

	int CommandExecute(IN const char* strKey);

	int RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser);

	int RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser), void* pUser);

	int RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO* pEventInfo, void* pUser), void* pUser);

	int ForceIp(unsigned int nIP, unsigned int nSubNetMask, unsigned int nDefaultGateWay);

	int SetIpConfig(unsigned int nType);

	int SetNetTransMode(unsigned int nType);

	int StartGrabbing();

	int StopGrabbing();

	int GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec);

	int FreeImageBuffer(MV_FRAME_OUT* pFrame);

	int DisplayOneFrame(MV_DISPLAY_FRAME_INFO* pDisplayInfo);

	int SetImageNodeNum(unsigned int nNum);

	int GetDeviceInfo(MV_CC_DEVICE_INFO* pstDevInfo);

	int GetOptimalPacketSize(unsigned int* pOptimalPacketSize);

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

	int ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM* pstCvtParam);

	int SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam);

	int SaveImageToFile(MV_SAVE_IMG_TO_FILE_PARAM* pstSaveFileParam);


#pragma endregion HikVisionSimpleSDK
};