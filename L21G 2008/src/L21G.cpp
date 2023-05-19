#include "appincludes.h"

#define QI(i) (riid == __uuidof(i)) ? GetInterface((i*)this, ppv) :
#define QI2(i) (riid == IID_##i) ? GetInterface((i*)this, ppv) :

STDMETHODIMP CL21G::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	CheckPointer(ppv, E_POINTER);

	return 
		QI2(IL21G)
		__super::NonDelegatingQueryInterface(riid, ppv);
}


HRESULT CL21G::CheckInputType(const CMediaType *pmt)
{
	if ((pmt->majortype == MEDIATYPE_AUXLine21Data) && (pmt->subtype == MEDIASUBTYPE_Line21_GOPPacket))
	{
	    return S_OK;
	}
	return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT CL21G::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
	if ((mtIn->majortype == MEDIATYPE_AUXLine21Data) && (mtIn->subtype == MEDIASUBTYPE_Line21_GOPPacket))
	{
		if ((mtOut->majortype == MEDIATYPE_AUXLine21Data) && (mtOut->subtype == MEDIASUBTYPE_Line21_GOPPacket))
		{
		    return S_OK;
		}
	}
	return VFW_E_TYPE_NOT_ACCEPTED;
}


HRESULT CL21G::GetMediaType(int iPosition, CMediaType *pMediaType)
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
		//the input and output are the same
		m_pInput->ConnectionMediaType(pMediaType);
        return S_OK;
    }
    return VFW_S_NO_MORE_ITEMS;
}

HRESULT CL21G::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProp)
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

HRESULT CL21G::SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt)
{
    if (direction == PINDIR_INPUT)
    {

	}
    else   // output pin
    {

	}

    return S_OK;
}

HRESULT  CL21G::SaveBufferToFile(const char * FileName, const char * Mode, LPBYTE buf, int bufsize)
{
	FILE * Out = fopen(FileName, Mode);
	fwrite(buf, bufsize, 1, Out);
	fclose(Out);
	return S_OK;
}

HRESULT CL21G::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	LPBYTE src = NULL;
	LPBYTE dst = NULL;
	pIn->GetPointer(&src);
	pOut->GetPointer(&dst);

	//Copy the buffer over into the output sample
	LastBufferSize = pIn->GetActualDataLength();
	memcpy(dst, src, LastBufferSize);
	
	//store the buffer, punk
	if ((LastBuffer) && ((int)LastBuffer != 0xcdcdcdcd)) 	free(LastBuffer);
	LastBuffer = (LPBYTE) malloc(LastBufferSize);
	memcpy(LastBuffer, src, LastBufferSize);
	//SaveBufferToFile("C:\\Temp\\L21Dump.bin", "w", LastBuffer, LastBufferSize);
	NotifyEvent(EC_L21Buffer, LastBufferSize, 0);

	ASSERT(pOut->GetSize() >= LastBufferSize);
    pOut->SetActualDataLength(LastBufferSize);

	//Beep(1000, 1);

    return S_OK;
}


// COM stuff


//----------------------------------------------------------------------------
// CL21G::CreateInstance
//
// Static method that returns a new instance of our filter.
// Note: The DirectShow class factory object needs this method.
//
// pUnk: Pointer to the controlling IUnknown (usually NULL)
// pHR:  Set this to an error code, if an error occurs
//-----------------------------------------------------------------------------

CUnknown * WINAPI CL21G::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHR) 
{
    CL21G *pFilter = new CL21G(pUnk, pHR );
    if (pFilter == NULL) 
    {
        *pHR = E_OUTOFMEMORY;
    }
    return pFilter;
} 


// The next bunch of structures define information for the class factory.

AMOVIESETUP_FILTER FilterInfo =
{
    &CLSID_L21G,     // CLSID
    g_wszName,          // Name
    MERIT_DO_NOT_USE,   // Merit
    0,                  // Number of AMOVIESETUP_PIN structs
    NULL                // Pin registration information.
};


CFactoryTemplate g_Templates[1] = 
{
    { 
      g_wszName,                // Name
      &CLSID_L21G,           // CLSID
      CL21G::CreateInstance, // Method to create an instance of MyComponent
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
