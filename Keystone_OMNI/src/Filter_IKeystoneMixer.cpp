#include "appincludes.h"
#include "Filter_IKeystoneMixer.h"

//OSD Mixer
STDMETHODIMP CKeystone::put_OSD(LPBYTE pMixImage, int W, int H, int X, int Y, int Format, COLORREF KeyColor, int DurationSecs)
{
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: New OSD.")));
	USEROPTION_DoOSDMix = false;
	OSD_Ticker = 0;
	OSD_TickGoal = DurationSecs;

	OSD_ActiveMixData.nH = H;
	OSD_ActiveMixData.nW = W;
	OSD_ActiveMixData.nX = X;
	OSD_ActiveMixData.nY = Y;
	OSD_ActiveMixData.nImage_Format = Format;
	OSD_ActiveMixData.KeyColor = 0xFE80FE80; //KeyColor;

	OSD_ActiveMixData.pImage = (LPBYTE)malloc(W*H*2);
	CSCManual::RGB24ToUYVY(pMixImage, W, H, OSD_ActiveMixData.pImage);

	USEROPTION_DoOSDMix = true;
	return S_OK;
}

STDMETHODIMP CKeystone::ClearOSD()
{
	USEROPTION_DoOSDMix = false;
	return S_OK;
}

STDMETHODIMP CKeystone::GrabLastL21(LPBYTE * pL21Sample)
{
	return S_OK;	
}

STDMETHODIMP CKeystone::GrabLastSubpicture(LPBYTE * pSubpictureSample)
{
	return S_OK;	
}

STDMETHODIMP CKeystone::SetGuides(int Right,int Top, int Left,int Bottom, int Red, int Blue, int Green)
{
	GIDE_Guide_L = Left;
	GIDE_Guide_T = Top;
	GIDE_Guide_R = Right;
	GIDE_Guide_B = Bottom;
	GIDE_GuideColor = CSCManual::RGBToYUV(Blue, Green, Red);
	USEROPTION_MixGuides = true;
	return S_OK;
}

STDMETHODIMP CKeystone::ClearGuides()
{
	USEROPTION_MixGuides = false;
	return S_OK;
}

STDMETHODIMP CKeystone::SetSPPlacement(int X, int Y)
{
	USEROPTION_SubpictureLocation_X = X;
	USEROPTION_SubpictureLocation_Y = Y;
	return S_OK;
}

STDMETHODIMP CKeystone::SetResizeMode(int Mode)
{
	SCLE_ActiveResizeMode = Mode;
	SCLE_DesiredResizeMode = Mode;
	return S_OK;
}

STDMETHODIMP CKeystone::SetLBColor(int Red, int Green, int Blue)
{
	SCLE_BarColor = CSCManual::RGBToYUV(Blue, Green, Red);
	return S_OK;
}

STDMETHODIMP CKeystone::SetL21Placement(int X, int Y)
{
	USEROPTION_Line21Position_X = X;
	USEROPTION_Line21Position_Y = Y;
	return S_OK;
}

STDMETHODIMP CKeystone::ReverseFieldOrder(bool bReverseIt)
{
	//bForceReverseFieldOrder = bReverseIt;
	return S_OK;
}

STDMETHODIMP CKeystone::SetJacketPicMode(bool bJackPicMode)
{
	//bInJacketPictureMode = bJackPicMode;
	return S_OK;
}

STDMETHODIMP CKeystone::BumpFieldsDown(int bBumpFields)
{
	if (bBumpFields == 0)
	{
		FRFI_ForceBumpFieldsDown = false;
	}
	else
	{
		FRFI_ForceBumpFieldsDown = true;
	}
	return S_OK;
}

STDMETHODIMP CKeystone::BurnGOPTCs(int iBurnGOPTCs, int iBlinkRed)
{
	USEROPTION_MixGOPTimecode = iBurnGOPTCs;
	USEROPTION_MixGOPTimecode_RedIFrames = iBlinkRed;
	return S_OK;
}

STDMETHODIMP CKeystone::SetActionTitleGuides(int iShowGuides, int Red, int Blue, int Green)
{
	USEROPTION_ShowActionTitleGuides = iShowGuides;
	GIDE_GuideColor = CSCManual::RGBToYUV(Blue, Green, Red);
	return S_OK;
}

STDMETHODIMP CKeystone::FieldSplit(int DoFieldSplit)
{
	FRFI_DoFieldSplitting = DoFieldSplit;
	return S_OK;
}

STDMETHODIMP CKeystone::HighContrastSP(int DoHighContrastSP)
{
	USEROPTION_DoHighContrastSubpicture = DoHighContrastSP;
	HRESULT hr = BeginFlush();
	if (hr == S_OK)
	{
		hr = EndFlush();
		if (hr == S_OK)
		{
			return S_OK;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		int i = 1;
		return 1;
	}
	return S_OK;
}

STDMETHODIMP CKeystone::SetARFlags(int PS, int LB)
{
	SCLE_LB_OK = LB;
	SCLE_PS_OK = PS;
	SCLE_SetupVideoSizing();
	return S_OK;
}

STDMETHODIMP CKeystone::SetActionTitleSafeColor(int Red, int Green, int Blue)
{
	GIDE_ActionTitleSafeColor = CSCManual::RGBToYUV(Blue, Green, Red);
	return S_OK;
}

STDMETHODIMP CKeystone::BarDataConfig(int DetectBarData, int BurnGuides, int Luma_Tolerance, int Chroma_Tolerance)
{
	USEROPTION_DetectBarData = (DetectBarData>0);
	BDDT_burn_demarcation = (BurnGuides>0);
	BDDT_luma_tolerance = Luma_Tolerance;
	if (BDDT_luma_tolerance>20) BDDT_luma_tolerance = 20;
	BDDT_chroma_tolerance = Chroma_Tolerance;
	if (BDDT_chroma_tolerance>10) BDDT_chroma_tolerance = 10;
	return S_OK;
}

STDMETHODIMP CKeystone::BarDataReset()
{
	BDDT_top_bar = 0;
	BDDT_bottom_bar = 0;
	BDDT_left_bar = 0;
	BDDT_right_bar = 0;
	BDDT_weight = 1;
	return S_OK;
}
