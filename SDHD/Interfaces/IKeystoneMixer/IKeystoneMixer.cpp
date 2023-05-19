#include "../../utility/appincludes.h"
#include "IKeystoneMixer.h"

//OSD Mixer
STDMETHODIMP CKeystone::put_OSD(LPBYTE pMixImage, int W, int H, int X, int Y, int Format, COLORREF KeyColor, int DurationSecs)
{
	//TODO: IANIER - How to convert DWORD_PTR into LPBYTE
	//The point here is to pass a pointer to an HDC that contains the image to be mixed onto the 
	//video. The reason that MixImage_Pointer is an LPBYTE is because that's what IMediaSample::GetPointer
	//provides so the mixing function will be getting that type of pointer value for the line21
	//and subpicture data that it will be mixing. But the OSD pointer seems to be a different
	//type of address, much larger - a normal pointer. While the IMediaSample::GetPointer value
	//is a number between 0 and 255. So what's going on and how do I address it?

	//reject new OSD data if an OSD operation is currently pending
	//if (bPendingOSDMix == true)
	//{
	//	return 1;
	//}

	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: New OSD.")));
	bPendingOSDMix = false;
	OSDTicker = 0;
	OSDTickGoal = DurationSecs;

	OSDMixData.nH = H;
	OSDMixData.nW = W;
	OSDMixData.nX = X;
	OSDMixData.nY = Y;
	OSDMixData.nImage_Format = Format;
	OSDMixData.KeyColor = KeyColor;

	//DEBUGGING
	//SaveBufferToFile("C:\\Temp\\Dump_RGB.bin", "w", pMixImage, OSDMixData.nW * OSDMixData.nH * 3, OSDMixData.nW, OSDMixData.nH);
	//DEBUGGING

	OSDMixData.pImage = (LPBYTE)malloc(W*H*2);
	CSCManual::RGB24ToUYVY(pMixImage, W, H, OSDMixData.pImage);

	//DEBUGGING
	//SaveBufferToFile("C:\\Temp\\Dump_UYVY.bin", "w", OSDMixData.pImage, OSDMixData.nW * OSDMixData.nH * 2, OSDMixData.nW, OSDMixData.nH);
	//DEBUGGING

	bPendingOSDMix = true;
	return S_OK;

	//COLORREFF = 0x00bbggrr
	//See RGB(R,G,B)
	//See also GetRValue, GetGValue, GetBValue

}

STDMETHODIMP CKeystone::ClearOSD()
{
	bPendingOSDMix = false;
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
	nGuide_L = Left;
	nGuide_T = Top;
	nGuide_R = Right;
	nGuide_B = Bottom;
	bMixGuides = true;
	dwGuideColor = CSCManual::RGBToYUV(Blue, Green, Red);
	return S_OK;
}

STDMETHODIMP CKeystone::ClearGuides()
{
	bMixGuides = false;
	return S_OK;
}

STDMETHODIMP CKeystone::SetSPPlacement(int X, int Y)
{
	SP_X = X;
	SP_Y = Y;
	return S_OK;
}

STDMETHODIMP CKeystone::SetResizeMode(int Mode)
{
	m_pVideo->ResizeMode = Mode;
	m_pVideo->DesResizeMode = Mode;
	return S_OK;
}

STDMETHODIMP CKeystone::SetLBColor(int Red, int Green, int Blue)
{
	//m_pVideo->dwLBColor = CSCManual::RGBToYUV(Blue, Green, Red);
	return S_OK;
}

STDMETHODIMP CKeystone::SetL21Placement(int X, int Y)
{
	L21_X = X;
	L21_Y = Y;
	return S_OK;
}

STDMETHODIMP CKeystone::ReverseFieldOrder(bool bReverseIt)
{
	//m_pVideo->bForceReverseFieldOrder = bReverseIt;
	return S_OK;
}

STDMETHODIMP CKeystone::SetJacketPicMode(bool bJackPicMode)
{
	//m_pVideo->bInJacketPictureMode = bJackPicMode;
	return S_OK;
}

STDMETHODIMP CKeystone::BumpFieldsDown(int bBumpFields)
{
	//if (bBumpFields == 0)
	//{
	//	m_pVideo->bForceBumpFieldsDown = false;
	//}
	//else
	//{
	//	m_pVideo->bForceBumpFieldsDown = true;
	//}
	return S_OK;
}

STDMETHODIMP CKeystone::BurnGOPTCs(int iBurnGOPTCs)
{
	iMixGOPTC = iBurnGOPTCs;
	return S_OK;
}

STDMETHODIMP CKeystone::SetActionTitleGuides(int iShowGuides, int Red, int Blue, int Green)
{
	iShowActionTitleGuides = iShowGuides;
	dwGuideColor = CSCManual::RGBToYUV(Blue, Green, Red);
	return S_OK;
}

STDMETHODIMP CKeystone::FieldSplit(int DoFieldSplit)
{
	//m_pVideo->DoFieldSplitting = DoFieldSplit;
	return S_OK;
}

STDMETHODIMP CKeystone::HighContrastSP(int DoHighContrastSP)
{
	//m_pSubpicture->DoHighContrastSP = DoHighContrastSP;
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
	LB_OK = LB;
	PS_OK = PS;
	SetupVideoSizing();
	//ResendSample();

	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: SetARFlags - PS: %d  LB: %d"), PS, LB));
	return S_OK;
}

