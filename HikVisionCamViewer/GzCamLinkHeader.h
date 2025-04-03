
#pragma once

typedef enum {
	GZ_CAMVENDOR_NONE,
	GZ_CAMVENDOR_DALSA,
	GZ_CAMVENDOR_VIEWORKS,
	GZ_CAMVENDOR_HIKVISON,
	GZ_CAMVENDOR_HIKVISON_SONY,
	GZ_CAMVENDOR_EMULATOR,
	GZ_CAMVENDOR_MAX
} eGzCamVendor;

typedef enum {
	GZ_HANDTYPE_RIGHT,	// = GZDLL_HANDTYPE_RIGHT	0x00
	GZ_HANDTYPE_LEFT,	// = GZDLL_HANDTYPE_LEFT	0x01
	GZ_HANDTYPE_MAX
} eGzHandType;

typedef enum {
	GZ_CAMPOS_SIDE,
	GZ_CAMPOS_TOP,
	GZ_CAMPOS_MAX
} eGzCamPosition;

// logical index of cameras
typedef enum {
	GZ_CAMERA_RIGHT,
	GZ_CAMERA_LEFT,
	GZ_CAMERA_TOPRIGHT,
	GZ_CAMERA_TOPLEFT,
	GZ_CAMERA_MAX
} eGzCamIndex;

typedef eGzCamVendor (*fpGetVender)(void);

// 
typedef int (*fpInitDevice)(void);

// 
typedef int (*fpStartDevice)(eGzHandType handtype);

// 
typedef int (*fpStopDevice)(void);

// 
typedef int (*fpReleaseDevice)(void);

//2024-10-28 ÀÌ½ÂÈÆ : Ä¶ ÃÖÀûÈ­ ÆÄ¶ó¸ÞÅÍ Àû¿ë
// 
typedef int (*fpCalStartDevice)(eGzHandType handtype);

// 
typedef int (*fpCalStopDevice)(void);

//2024-10-28 ÀÌ½ÂÈÆ : ¹à±â¸¦ ¾îµÓ°Ô, ¹à°Ô Âï¾î¼­ ÇÊ¿ä¿µ¿ª ÇÕ¼º
// 
typedef int (*fpHighStartDevice)(eGzHandType handtype);

// 
typedef int (*fpHighStopDevice)(void);

//
typedef int (*fpGetFeatureValueINT)(const char *featureName, int CamID, int *pValue);
typedef int (*fpGetFeatureValueDWORD)(const char *featureName, int CamID, DWORD *pValue);
typedef int (*fpGetFeatureValueSTR)(const char *featureName, int CamID, char *pValue);
typedef int (*fpGetFeatureValueFLOAT)(const char *featureName, int CamID, float *pValue);

//
typedef int (*fpSetFeatureValueINT)(const char *featureName, int CamID, const int Value);
typedef int (*fpSetFeatureValueDWORD)(const char *featureName, int CamID, const DWORD Value);
typedef int (*fpSetFeatureValueSTR)(const char *featureName, int CamID, const char *pValue);
typedef int (*fpSetFeatureValueFLOAT)(const char *featureName, int CamID, const float *pValue);

typedef void *(*fpGetBufferPointer)(int camid);

typedef struct 
{
	fpGetVender		GetVender;
	fpInitDevice	InitDevice;
	fpStartDevice	StartDevice;
	fpStopDevice	StopDevice;
	fpReleaseDevice	ReleaseDevice;
	//2024-10-28 ÀÌ½ÂÈÆ : ¹à±â¸¦ ¾îµÓ°Ô, ¹à°Ô Âï¾î¼­ ÇÊ¿ä¿µ¿ª ÇÕ¼º
	fpCalStartDevice lowBright_StartDevice;
	fpCalStopDevice lowBright_StopDevice;
	fpHighStartDevice highBright_StartDevice;
	fpHighStopDevice highBright_StopDevice;

	fpGetFeatureValueINT	GetFeatureValueINT;
	fpGetFeatureValueDWORD	GetFeatureValueDWORD;
	fpGetFeatureValueSTR	GetFeatureValueSTR;
	fpGetFeatureValueFLOAT	GetFeatureValueFLOAT;

	fpSetFeatureValueINT	SetFeatureValueINT;
	fpSetFeatureValueDWORD	SetFeatureValueDWORD;
	fpSetFeatureValueSTR	SetFeatureValueSTR;
	fpSetFeatureValueFLOAT	SetFeatureValueFLOAT;

	fpGetBufferPointer	GetBufferPointer;

} GzCamLinkPointer;


