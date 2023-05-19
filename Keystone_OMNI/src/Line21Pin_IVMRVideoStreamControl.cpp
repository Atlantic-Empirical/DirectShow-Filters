#include "appincludes.h"
#include <ddraw.h>

#pragma warning(disable:4800)

STDMETHODIMP CKeystoneLine21Pin::SetStreamActiveState( BOOL fActive)
{
	bL21StreamIsActive = (bool) fActive;
	return S_OK;
};

STDMETHODIMP CKeystoneLine21Pin::GetStreamActiveState( BOOL *lpfActive)
{
	*lpfActive = (BOOL) bL21StreamIsActive;
	return S_OK;
};

