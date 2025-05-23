#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"
#include "framework.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS    // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.
#include <afxdisp.h>        // MFC

//#include "afxdialogex.h"

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>               // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                 // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxcontrolbars.h>     // MFC

#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <ctype.h>



#define WM_TIMER_GRAB_INFO 1
#define MAX_DEVICE_NUM 4

#define CAMERA_WIDTH	640
#define CAMERA_HEIGHT	480