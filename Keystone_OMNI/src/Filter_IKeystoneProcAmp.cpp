#include "appincludes.h"
#include "Filter_iKeystoneProcAmp.h"

STDMETHODIMP CKeystone::put_Brightness(double dBrightness)
{
	PAMP_Brightness = dBrightness;
	CProcAmp::SetBrightness(dBrightness);
	return S_OK;
}

STDMETHODIMP CKeystone::get_Brightness(double *dBrightness)
{
	*dBrightness = PAMP_Brightness;
	return S_OK;
}

STDMETHODIMP CKeystone::put_Contrast(double dContrast)
{
	PAMP_Contrast = dContrast;
	CProcAmp::SetContrast(dContrast);
	return S_OK;
}

STDMETHODIMP CKeystone::get_Contrast(double *dContrast)
{
	*dContrast = PAMP_Contrast;
	return S_OK;
}

STDMETHODIMP CKeystone::put_Hue(double dHue)
{
	PAMP_Hue = dHue;
	CProcAmp::SetHue(dHue);
	return S_OK;
}

STDMETHODIMP CKeystone::get_Hue(double *dHue)
{
	*dHue = PAMP_Hue;
	return S_OK;
}

STDMETHODIMP CKeystone::put_Saturation(double dSaturation)
{
	PAMP_Saturation = dSaturation;
	CProcAmp::SetSaturation(dSaturation);
	return S_OK;
}

STDMETHODIMP CKeystone::get_Saturation(double *dSaturation)
{
	*dSaturation = PAMP_Saturation;
	return S_OK;
}

STDMETHODIMP CKeystone::ToggleProcAmp(bool bToggleProcAmp, bool bHalfFrame)
{
	USEROPTION_DoProcAmp = bToggleProcAmp;
	USEROPTION_ProcAmpHalfFrame = bHalfFrame;
	return S_OK;
}

STDMETHODIMP CKeystone::ToggleColorFilter(bool bDoColorFilter, int iUseWhichFilter)
{
	USEROPTION_DoColorFiltering = bDoColorFilter;
	USEROPTION_WhichColorFilter = iUseWhichFilter;
	return S_OK;
}
