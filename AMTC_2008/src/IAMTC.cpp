#include "appincludes.h"

STDMETHODIMP CAMTC::GetBuffer(LPBYTE *pSample, long *BufferSize)
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

STDMETHODIMP CAMTC::StoreBuffers(bool bUserWantsBuffersStored)
{
	bGrabBuffers = bUserWantsBuffersStored;
	return S_OK;
}

STDMETHODIMP CAMTC::FrameStep(bool bForward)
{
	//if (m_pVideo->bFrameStepUnderway) return S_OK; //we're busy
	//Gabor's new way
	if (!bFrameStepping)
	{
		bFrameStepping = true;
	}
	else
	{
		bDeliverFrameStepSample = true;
	}
	return S_OK;
}

STDMETHODIMP CAMTC::QuitFrameStepping()
{
	//Gabor's new way
	bFrameStepping = false;
	//m_pVideo->bSendNewSegment;
	//m_pVideo->bRestartTimeStamps = true;
	return S_OK;
}

STDMETHODIMP CAMTC::SetNULLTimestamps()
{
	bSetNULLTimestamps = true;
	return S_OK;
}
