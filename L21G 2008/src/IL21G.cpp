#include "appincludes.h"

STDMETHODIMP CL21G::GetBuffer(LPBYTE *pSample, long *BufferSize)
{
	if (opUnderway == true)
	{		
		//Can only do one at a time
		return E_ACCESSDENIED;
	}

	opUnderway = true;

	while (LastBuffer == NULL)
	{
		//Just wait until the sample is created		
	}
	if (LastBuffer == 0)
	{
		//HandleSampleGrab did not succeed.
		return E_FAIL;
	}

	//Now, the sample buffer is available
	*pSample = LastBuffer;
	*BufferSize = LastBufferSize;

	opUnderway = false;
	return S_OK;
}
