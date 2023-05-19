#include <appincludes.h>
#include <ddraw.h>

STDMETHODIMP CKeystoneVideoPin::SetColorKey( LPDDCOLORKEY lpClrKey)
{
	return S_OK;
};

STDMETHODIMP CKeystoneVideoPin::GetColorKey( LPDDCOLORKEY lpClrKey)
{
	return S_OK;
};

STDMETHODIMP CKeystoneVideoPin::SetStreamActiveState( BOOL fActive)
{
	return S_OK;
};

STDMETHODIMP CKeystoneVideoPin::GetStreamActiveState( BOOL *lpfActive)
{
	return S_OK;
};
