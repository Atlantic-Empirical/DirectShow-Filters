#include "appincludes.h"

STDMETHODIMP CKeystoneSubpicturePin::SetStreamActiveState(BOOL fActive)
{
	BSubpictureStreamIsActive = fActive;
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: SP Activate. %d"), fActive));
	return S_OK;
}

STDMETHODIMP CKeystoneSubpicturePin::GetStreamActiveState(BOOL *fActive)
{
	*fActive = BSubpictureStreamIsActive;
	return S_OK;
}

