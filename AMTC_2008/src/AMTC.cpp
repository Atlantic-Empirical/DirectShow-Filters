#include "appincludes.h"

#define QI(i) (riid == __uuidof(i)) ? GetInterface((i*)this, ppv) :
#define QI2(i) (riid == IID_##i) ? GetInterface((i*)this, ppv) :

STDMETHODIMP CAMTC::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	CheckPointer(ppv, E_POINTER);

	return 
		QI2(IAMTC)
		__super::NonDelegatingQueryInterface(riid, ppv);
}

HRESULT CAMTC::CheckInputType(const CMediaType *pmt)
{
	WAVEFORMATEXTENSIBLE *pwfx = (WAVEFORMATEXTENSIBLE*)pmt->Format();
	if (pwfx == 0x00)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	if (pwfx->Format.wFormatTag == WAVE_FORMAT_DOLBY_AC3_SPDIF)
	{
	    return S_OK;
	}

	//added 070703 to accomodate PacTV graph
	//re-added 081019 for use in skunkworks
	if (pwfx->Format.wFormatTag == WAVE_FORMAT_PCM)
	{
	    return S_OK;
	}

	return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT CAMTC::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
	WAVEFORMATEXTENSIBLE *pWFXIn = (WAVEFORMATEXTENSIBLE*)mtIn->Format();
	if (
		  (pWFXIn->Format.wFormatTag == WAVE_FORMAT_DOLBY_AC3_SPDIF) //From nv Aud Dec
			|| 
		  (pWFXIn->Format.wFormatTag == WAVE_FORMAT_PCM) //from ffdshow aud dec, for PacTV graph, or for nVidia audio decoder for Skunkworks M2TS graph
		)
	{
		WAVEFORMATEXTENSIBLE *pWFXOut = (WAVEFORMATEXTENSIBLE*)mtOut->Format();
		if (pWFXOut == 0x00000000)
		{
			return VFW_E_TYPE_NOT_ACCEPTED;
		}
		if (pWFXOut->Format.wFormatTag == WAVE_FORMAT_PCM) //For DL Aud Ren
		{
		    return S_OK;
		}
	}
	return VFW_E_TYPE_NOT_ACCEPTED;
}


HRESULT CAMTC::GetMediaType(int iPosition, CMediaType *pMediaType)
{
    // The output pin calls this method only if the input pin is connected.
    ASSERT(m_pInput->IsConnected());

    // There is only one output type that we want, which is the input type.

    if (iPosition < 0)
    {
        return E_INVALIDARG;
    }
    else if (iPosition == 0)
    {
		//We want the input type but change the wFormatTag to WAVE_FORMAT_PCM so the DL Aud Ren will be happy.
		m_pInput->ConnectionMediaType(pMediaType);
		WAVEFORMATEXTENSIBLE *pWFXOut = (WAVEFORMATEXTENSIBLE*)pMediaType->Format();
		pWFXOut->Format.wFormatTag = WAVE_FORMAT_PCM;
        return S_OK;
    }
    return VFW_S_NO_MORE_ITEMS;
}

HRESULT CAMTC::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProp)
{
    // Make sure the input pin connected.
    if (!m_pInput->IsConnected()) 
    {
        return E_UNEXPECTED;
    }

    // Our strategy here is to use the upstream allocator as the guideline, but
    // also defer to the downstream filter's request when it's compatible with us.

    // First, find the upstream allocator...
    ALLOCATOR_PROPERTIES InputProps;

    IMemAllocator *pAllocInput = 0;
    HRESULT hr = m_pInput->GetAllocator(&pAllocInput);

    if (FAILED(hr))
    {
        return hr;
    }

    // ... now get the properters

    hr = pAllocInput->GetProperties(&InputProps);
    pAllocInput->Release();

    if (FAILED(hr)) 
    {
        return hr;
    }

    // Buffer alignment should be non-zero [zero alignment makes no sense!]
    if (pProp->cbAlign == 0)
    {
        pProp->cbAlign = 1;
    }

    // Number of buffers must be non-zero
    if (pProp->cbBuffer == 0)
    {
        pProp->cBuffers = 1;
    }

    // For buffer size, find the maximum of the upstream size and 
    // the downstream filter's request.
    pProp->cbBuffer = max(InputProps.cbBuffer, pProp->cbBuffer);
   
    // Now set the properties on the allocator that was given to us,
    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pProp, &Actual);
    if (FAILED(hr)) 
    {
        return hr;
    }
    
    // Even if SetProperties succeeds, the actual properties might be
    // different than what we asked for. We check the result, but we only
    // look at the properties that we care about. The downstream filter
    // will look at them when NotifyAllocator is called.
    
    if (InputProps.cbBuffer > Actual.cbBuffer) 
    {
        return E_FAIL;
    }
    
    return S_OK;
}

// override to pass downstream
HRESULT CAMTC::NewSegment( REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	DbgLog((LOG_TRACE, 0, TEXT("AMTC: New Segment")));
 	return __super::NewSegment(tStart, tStop, dRate);
}

HRESULT CAMTC::SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt)
{
    if (direction == PINDIR_INPUT)
    {

	}
    else   // output pin
    {

	}
    return S_OK;
}

HRESULT CAMTC::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	////Gabor's version of frame stepping
	//while (bFrameStepping)
	//{
	//	//Sleep(1);
	//	//if(bDeliverFrameStepSample)
	//	//{
	//	//	//pSample->SetTime(NULL, NULL);
	//	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Framestep sample.")));
	//	//	bDeliverFrameStepSample = false;
	//	//	return DeliverSample(pIn, pOut);
	//	//}
	//}
	return DeliverSample(pIn, pOut);
}

HRESULT CAMTC::DeliverSample(IMediaSample * pIn, IMediaSample * pOut)
{
	//Beep(1000,1);
	DbgLog((LOG_TRACE, 0, TEXT("AMTC | DeliverSample()")));

	////DEBUGGING
	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;
	//pIn->GetTime(&rtS, &rtE);
	//DbgLog((LOG_TRACE, 0, TEXT("AMTC | rtS/rtE @Receive: %I64d %I64d"), rtS, rtE));
	////NotifyEvent(EC_AMTC_SAMPLETIMES, rtStart, rtEnd);
	////DEBUGGING

	LPBYTE src = NULL;
	LPBYTE dst = NULL;
	pIn->GetPointer(&src);
	pOut->GetPointer(&dst);

	//Copy the buffer over into the output sample
	LastBufferSize = pIn->GetActualDataLength();
	memcpy(dst, src, LastBufferSize);

	if (bGrabBuffers)
	{
		//store the buffer, punk
		if ((LastBuffer) && ((int)LastBuffer != 0xcdcdcdcd)) 	free(LastBuffer);
		LastBuffer = (LPBYTE) malloc(LastBufferSize);
		memcpy(LastBuffer, src, LastBufferSize);
		NotifyEvent(EC_AMTCBuffer, LastBufferSize, 0);
	}

	ASSERT(pOut->GetSize() >= LastBufferSize);
    pOut->SetActualDataLength(LastBufferSize);

	////DEBUGGING
	//CRefTime crtST;
	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;
	//StreamTime(crtST);
	//pOut->GetTime(&rtS, &rtE);
	//DbgLog((LOG_TRACE, 0, TEXT("	          AMTC ST: %I64d"), crtST.m_time));
	//DbgLog((LOG_TRACE, 0, TEXT("	       AMTC TIMES: %I64d -- %I64d"), rtS, rtE));
	//REFERENCE_TIME rtDif = crtST - rtS;
	//DbgLog((LOG_TRACE, 0, TEXT("		     AMTC DIF: %I64d"), rtDif));
	////DEBUGGING

	if (bSetNULLTimestamps == true)
	{
		//pOut->SetTime(NULL, NULL);	
	}

	//DEBUGGING
	//pOut->SetTime(NULL, NULL);	
	//DEBUGGING

    return S_OK;
}


// COM stuff


//----------------------------------------------------------------------------
// CAMTC::CreateInstance
//
// Static method that returns a new instance of our filter.
// Note: The DirectShow class factory object needs this method.
//
// pUnk: Pointer to the controlling IUnknown (usually NULL)
// pHR:  Set this to an error code, if an error occurs
//-----------------------------------------------------------------------------

CUnknown * WINAPI CAMTC::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHR) 
{
    CAMTC *pFilter = new CAMTC(pUnk, pHR );
    if (pFilter == NULL) 
    {
        *pHR = E_OUTOFMEMORY;
    }
    return pFilter;
} 


// The next bunch of structures define information for the class factory.

AMOVIESETUP_FILTER FilterInfo =
{
    &CLSID_AMTC,     // CLSID
    g_wszName,          // Name
    MERIT_DO_NOT_USE,   // Merit
    0,                  // Number of AMOVIESETUP_PIN structs
    NULL                // Pin registration information.
};


CFactoryTemplate g_Templates[1] = 
{
    { 
      g_wszName,                // Name
      &CLSID_AMTC,           // CLSID
      CAMTC::CreateInstance, // Method to create an instance of MyComponent
      NULL,                     // Initialization function
      &FilterInfo               // Set-up information (for filters)
    }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);    


// Functions needed by the DLL, for registration.

STDAPI DllRegisterServer(void)
{
    return AMovieDllRegisterServer2(TRUE);
}


STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);
}
