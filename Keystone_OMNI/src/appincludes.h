#pragma once

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <initguid.h>
#include <tchar.h>

// DirectX
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

#include "CommonStructures.h"

#include "Filter_IKeystone.h"
#include "Filter_IKeystoneQuality.h"
#include "Filter_IKeystoneProcAmp.h"
#include "Filter_IKeystoneMixer.h"
#include "PropPage_Keystone.h"
#include "CKeystone.h"

//VIDEO PROCESSING
#include "CSC_Manual.h"
#include "ProcAmp.h"

#define EC_KEYSTONE_QUALITY_CHANGE			0x58
#define EC_KEYSTONE_32						0x59
#define EC_KEYSTONE_SAMPLETIMES				0x60
#define EC_KEYSTONE_FIELDORDER				0x61
#define EC_KEYSTONE_MPEGTC					0x62
#define EC_KEYSTONE_INTERLACING				0x63
#define EC_KEYSTONE_FORCEFRAMEGRAB			0x64
#define EC_KEYSTONE_PROGRESSIVESEQUENCE		0x65
#define EC_KEYSTONE_DISCONTINUITY			0x66
#define EC_KEYSTONE_MACROVISION				0x67
#define EC_KEYSTONE_FRAMEDELIVERED			0x68
#define EC_KEYSTONE_FRAMERECEIVED			0x69
#define EC_KEYSTONE_FRAMEDROPPED			0x70
#define EC_KEYSTONE_WRONGDURATION			0x71
#define EC_KEYSTONE_SAMPLESNOTADJACENT		0x72
#define EC_KEYSTONE_ENDOFSTREAM				0x73
#define EC_KEYSTONE_MEDIATIME				0x74
#define EC_KEYSTONE_BARDATA_TOP_BOTTOM		0x75
#define EC_KEYSTONE_BARDATA_LEFT_RIGHT		0x76
#define EC_KEYSTONE_BARDATA_FRAME_TOO_DARK	0x77
#define EC_KEYSTONE_PRESENTATIONTIMES		0x78
#define EC_KEYSTONE_RUNNOTCALLED			0x79
#define EC_KEYSTONE_STREAMTIME				0x80
#define EC_KEYSTONE_SETPRESENTATIONTIME		0x81
//#define EC_KEYSTONE_RECEIVE					0x82
#define EC_KEYSTONE_DELIVER					0x83
#define EC_KEYSTONE_RUN						0x84
#define EC_KEYSTONE_TRIAL_FRAMECOUNT		0x85
