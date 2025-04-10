#include "stdafx.h"
#include "HikvisionDLL.h"

#include "HikVisionCamera.h"

HikVisionCamera g_HikVision;


eGzCamVendor GzCamLink_GetVender(void)
{
	return GZ_CAMVENDOR_HIKVISON;
}


int GzCamLink_InitDevice(void)
{
	// already initlized
	return g_HikVision.InitDevice();
}

int GzCamLink_StartDevice(eGzHandType handtype)
{
	return g_HikVision.StartDevice(handtype);
}

int GzCamLink_StopDevice(void)
{
	return g_HikVision.StopDevice();
}

int GzCamLink_ReleaseDevice(void)
{
	g_HikVision.ReleaseDevice();
	return TRUE;
}


int GzCamLink_lowBright_StartDevice(eGzHandType handtype)
{
	return g_HikVision.lowBright_StartDevice(handtype);
}

int GzCamLink_lowBright_StopDevice(void)
{
	return g_HikVision.lowBright_StopDevice();
}

int GzCamLink_highBright_StartDevice(eGzHandType handtype)
{
	return g_HikVision.highBright_StartDevice(handtype);
}

int GzCamLink_highBright_StopDevice(void)
{
	return g_HikVision.highBright_StopDevice();
}


int GzCamLink_GetFeatureValueINT(const char* featureName, int CamID, int* pValue)
{
	if (!strcmp(featureName, "MaxGain"))
	{
		*pValue = g_HikVision.GetMaxGain();
		return TRUE;
	}

	if (!strcmp(featureName, "MinGain"))
	{
		*pValue = g_HikVision.GetMinGain();
		return TRUE;
	}

	if (!strcmp(featureName, "MaxExposure"))
	{
		*pValue = g_HikVision.GetMaxExposure();
		return TRUE;
	}

	if (!strcmp(featureName, "MinExposure"))
	{
		*pValue = g_HikVision.GetMinExposure();
		return TRUE;
	}

	if (!strcmp(featureName, "FrameGrapped"))
	{
		if (g_HikVision.m_ImgBuffer[CamID])
		{
			*pValue = g_HikVision.m_ImgBuffer[CamID]->bFrameGrabbed;
			return TRUE;
		}
		else
			return FALSE;
	}

	return FALSE;
}

int GzCamLink_GetFeatureValueDWORD(const char* featureName, int CamID, DWORD* pValue)
{
	return FALSE;
}

int GzCamLink_GetFeatureValueSTR(const char* featureName, int CamID, char* pValue)
{
	if (!strcmp(featureName, "SerialNumber"))
	{
		return g_HikVision.GetSerialNumber(CamID, pValue);
	}
	if (!strcmp(featureName, "MACAddress"))
	{
		return g_HikVision.GetMacAddress(CamID, pValue);
	}
	return FALSE;
}

int GzCamLink_GetFeatureValueFLOAT(const char* featureName, int CamID, float* pValue)
{
	if (!strcmp(featureName, "Gain"))
	{
		return g_HikVision.GetGain(CamID, pValue);
	}

	if (!strcmp(featureName, "ExposureTime"))
	{
		return g_HikVision.GetExposureTime(CamID, pValue);
	}

	if (!strcmp(featureName, "Temperature"))
	{
		return g_HikVision.GetTemperature(CamID, pValue);
	}

	if (!strcmp(featureName, "FPS"))
	{
		return g_HikVision.GetFPS(CamID, pValue);
	}

	return FALSE;
}

int GzCamLink_SetFeatureValueINT(const char* featureName, int CamID, const int Value)
{
	// Vieworks 에서는 사용하지 않음.

	/*if(!strcmp(featureName, "AdjustCameraOffset"))
	{
		return true;
	}*/
	return FALSE;
}

int GzCamLink_SetFeatureValueDWORD(const char* featureName, int CamID, const DWORD Value)
{
	return FALSE;
}

int GzCamLink_SetFeatureValueSTR(const char* featureName, int CamID, const char* pValue)
{
	return FALSE;
}

int GzCamLink_SetFeatureValueFLOAT(const char* featureName, int CamID, const float* pValue)
{
	if (!strcmp(featureName, "Gain"))
	{
		g_HikVision.SetGain(CamID, *pValue);
		return TRUE;
	}

	if (!strcmp(featureName, "ExposureTime"))
	{
		g_HikVision.SetExposureTime(CamID, *pValue);
		return TRUE;
	}

	return FALSE;
}

void* GzCamLink_GetBufferPointer(int camid)
{
	if (g_HikVision.m_ImgBuffer[camid])
	{
		return g_HikVision.m_ImgBuffer[camid]->ImgBuffer;
	}
	return NULL;
}

int GzCamLink_GetServerID(int camid)
{
	return g_HikVision.GetServerID(camid);
}