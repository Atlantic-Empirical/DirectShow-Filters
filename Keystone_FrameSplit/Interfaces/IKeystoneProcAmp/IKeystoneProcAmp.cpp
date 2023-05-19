#include "../../utility/appincludes.h"
#include "iKeystoneProcAmp.h"

STDMETHODIMP CKeystone::put_Brightness(double dBrightness)
{
	PA_dBrightness = dBrightness;
	CProcAmp::SetBrightness(dBrightness);
	return S_OK;
}

STDMETHODIMP CKeystone::get_Brightness(double *dBrightness)
{
	*dBrightness = PA_dBrightness;
	return S_OK;
}

STDMETHODIMP CKeystone::put_Contrast(double dContrast)
{
	PA_dContrast = dContrast;
	CProcAmp::SetContrast(dContrast);
	return S_OK;
}

STDMETHODIMP CKeystone::get_Contrast(double *dContrast)
{
	*dContrast = PA_dContrast;
	return S_OK;
}

STDMETHODIMP CKeystone::put_Hue(double dHue)
{
	PA_dHue = dHue;
	CProcAmp::SetHue(dHue);
	return S_OK;
}

STDMETHODIMP CKeystone::get_Hue(double *dHue)
{
	*dHue = PA_dHue;
	return S_OK;
}

STDMETHODIMP CKeystone::put_Saturation(double dSaturation)
{
	PA_dSaturation = dSaturation;
	CProcAmp::SetSaturation(dSaturation);
	return S_OK;
}

STDMETHODIMP CKeystone::get_Saturation(double *dSaturation)
{
	*dSaturation = PA_dSaturation;
	return S_OK;
}

STDMETHODIMP CKeystone::ToggleProcAmp(bool bToggleProcAmp, bool bHalfFrame)
{
	bDoProcAmp = bToggleProcAmp;
	bProcAmpHalfFrame = bHalfFrame;
	return S_OK;
}

STDMETHODIMP CKeystone::ToggleColorFilter(bool bDoColorFilter, int iUseWhichFilter)
{
	bDoColorFiltering = bDoColorFilter;
	iWhichColorFilter = iUseWhichFilter;
	return S_OK;
}
