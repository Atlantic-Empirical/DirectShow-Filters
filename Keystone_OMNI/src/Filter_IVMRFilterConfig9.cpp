#include "appincludes.h"
#include <vmr9.h>

STDMETHODIMP CKeystone::SetAlpha( DWORD dwStreamID, float Alpha)
{
	return S_OK;
};
STDMETHODIMP CKeystone::GetAlpha(  DWORD dwStreamID, float *pAlpha)
{
	return S_OK;
};
STDMETHODIMP CKeystone::SetZOrder( DWORD dwStreamID, DWORD dwZ)
{
	return S_OK;
};
STDMETHODIMP CKeystone::GetZOrder( DWORD dwStreamID, DWORD *pZ)
{
	return S_OK;
};
STDMETHODIMP CKeystone::SetOutputRect( DWORD dwStreamID, const VMR9NormalizedRect *pRect)
{
	return S_OK;
};
STDMETHODIMP CKeystone::GetOutputRect( DWORD dwStreamID, VMR9NormalizedRect *pRect)
{
	return S_OK;
};
STDMETHODIMP CKeystone::SetBackgroundClr( COLORREF ClrBkg)
{
	return S_OK;
};
STDMETHODIMP CKeystone::GetBackgroundClr( COLORREF *lpClrBkg)
{
	return S_OK;
};
STDMETHODIMP CKeystone::SetMixingPrefs( DWORD dwMixerPrefs)
{
	return S_OK;
};
STDMETHODIMP CKeystone::GetMixingPrefs( DWORD *pdwMixerPrefs)
{
	//DWORD OutPrefs = MixerPref9_NoDecimation || 0x00002000;
	return S_OK;
};
STDMETHODIMP CKeystone::SetProcAmpControl( DWORD dwStreamID, VMR9ProcAmpControl *lpClrControl)
{
	return S_OK;
};
STDMETHODIMP CKeystone::GetProcAmpControl( DWORD dwStreamID, VMR9ProcAmpControl *lpClrControl)
{
	return S_OK;
};
STDMETHODIMP CKeystone::GetProcAmpControlRange( DWORD dwStreamID, VMR9ProcAmpControlRange *lpClrControl)
{
	return S_OK;
};
