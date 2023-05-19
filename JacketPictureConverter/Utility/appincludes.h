#pragma once

#include <windows.h>
//#include <assert.h>
//#include <mmreg.h>
//#include <windowsx.h>
#include <commctrl.h>
//#include <olectl.h>
//#include <memory.h>
//#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <initguid.h>
//#include <olectl.h>
//#include <olectlid.h>
#include <tchar.h>

// DirectX
//#include <dshow.h>
#include <streams.h>
#include <d3d9.h>
#include <dvdmedia.h>

// returns the current module instance
HINSTANCE GetModuleInstance();

// some useful macros

//#define BoolCheck(x, hr) if (!(x)) throw hr;
#define OleCheck(x) if (FAILED(hr = (x))) throw hr;

// app specific
#include "dshowutility.h"
#include "Keystone_uids.h"
#include "round.h"
#include "../Interfaces/IKeystone/IKeystone.h"
#include "../Filter/CKeystone.h"

//VIDEO PROCESSING
#include "../VideoProcessing/CSC/CSC_IPP.h"
#include "../VideoProcessing/CSC/CSC_Manual.h"
#include "../VideoProcessing/ProcAmp/ProcAmp.h"
//#include "../VideoProcessing/Mixer/Mixer.h"

#define EC_QUALITY_CHANGE_KEYSTONE      0x58
#define EC_KEYSTONE_32				    0x59
#define EC_KEYSTONE_SAMPLETIMES			0x60
#define EC_KEYSTONE_FIELDORDER		    0x61
#define EC_KEYSTONE_MPEGTC			    0x62
#define EC_KEYSTONE_INTERLACING			0x63
#define EC_KEYSTONE_FORCEFRAMEGRAB		0x64
#define EC_KEYSTONE_PROGRESSIVESEQUENCE	0x65
#define EC_KEYSTONE_DISCONTINUITY		0x66
#define EC_KEYSTONE_MACROVISION			0x67
#define EC_KEYSTONE_FRAMEDELIVERED		0x68
#define EC_KEYSTONE_FRAMERECEIVED		0x69
#define EC_KEYSTONE_FRAMEDROPPED		0x200
