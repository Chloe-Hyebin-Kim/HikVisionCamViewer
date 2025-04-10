#pragma once

#include "GzCamLinkHeader.h"


eGzCamVendor GzCamLink_GetVender(void);
int GzCamLink_InitDevice(void);
int GzCamLink_StartDevice(eGzHandType handtype);
int GzCamLink_StopDevice(void);
int GzCamLink_ReleaseDevice(void);


int GzCamLink_lowBright_StartDevice(eGzHandType handtype);
int GzCamLink_lowBright_StopDevice(void);
int GzCamLink_highBright_StartDevice(eGzHandType handtype);
int GzCamLink_highBright_StopDevice(void);


int GzCamLink_GetFeatureValueINT(const char* featureName, int CamID, int* pValue);
int GzCamLink_GetFeatureValueDWORD(const char* featureName, int CamID, DWORD* pValue);
int GzCamLink_GetFeatureValueSTR(const char* featureName, int CamID, char* pValue);
int GzCamLink_GetFeatureValueFLOAT(const char* featureName, int CamID, float* pValue);

int GzCamLink_SetFeatureValueINT(const char* featureName, int CamID, const int Value);
int GzCamLink_SetFeatureValueDWORD(const char* featureName, int CamID, const DWORD Value);
int GzCamLink_SetFeatureValueSTR(const char* featureName, int CamID, const char* pValue);
int GzCamLink_SetFeatureValueFLOAT(const char* featureName, int CamID, const float* pValue);

void* GzCamLink_GetBufferPointer(int camid);
