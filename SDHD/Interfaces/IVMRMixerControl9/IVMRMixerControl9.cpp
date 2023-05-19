#include "../../utility/appincludes.h"
#include <vmr9.h>

STDMETHODIMP CKeystone::SetImageCompositor( IVMRImageCompositor9 *lpVMRImgCompositor)
{
	return S_OK;
};

STDMETHODIMP CKeystone::SetNumberOfStreams( DWORD dwMaxStreams)
{
	return S_OK;
};

STDMETHODIMP CKeystone::GetNumberOfStreams( DWORD *pdwMaxStreams)
{
	return S_OK;
};

STDMETHODIMP CKeystone::SetRenderingPrefs( DWORD dwRenderFlags)
{
	return S_OK;
};

STDMETHODIMP CKeystone::GetRenderingPrefs( DWORD *pdwRenderFlags)
{
	return S_OK;
};

STDMETHODIMP CKeystone::SetRenderingMode( DWORD Mode)
{
	return S_OK;
};

STDMETHODIMP CKeystone::GetRenderingMode( DWORD *pMode)
{
	return S_OK;
};
