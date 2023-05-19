#include "appincludes.h"
#include "CKeystone.h"
#include <dvdmedia.h>
#include "SubpicturePin_Allocator.h"

// constructor
CKeystoneSubpicturePin::CKeystoneSubpicturePin(
    TCHAR *pObjectName,
    CKeystone *pTransformFilter,
    HRESULT * phr,
    LPCWSTR pName)
    : CBaseInputPin(pObjectName, pTransformFilter, &pTransformFilter->m_csFilter, phr, pName)
{
    DbgLog((LOG_TRACE,2,TEXT("CKeystoneSubpicturePin::CKeystoneSubpicturePin")));
    m_pTransformFilter = pTransformFilter;
	m_allocprops.cbAlign = 1;
	m_allocprops.cbBuffer = 829440;
	m_allocprops.cbPrefix = 0;
	m_allocprops.cBuffers = 1;
	BSubpictureStreamIsActive = TRUE;
}

#ifdef UNICODE
CKeystoneSubpicturePin::CKeystoneSubpicturePin(
    CHAR *pObjectName,
    CKeystone *pTransformFilter,
    HRESULT * phr,
    LPCWSTR pName)
    : CBaseInputPin(pObjectName, pTransformFilter, &pTransformFilter->m_csFilter, phr, pName)
{
    DbgLog((LOG_TRACE,2,TEXT("CKeystoneSubpicturePin::CKeystoneSubpicturePin")));
    m_pTransformFilter = pTransformFilter;
}
#endif

#define QI2(i) (riid == IID_##i) ? GetInterface((i*)this, ppv) :

STDMETHODIMP CKeystoneSubpicturePin::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	CheckPointer(ppv, E_POINTER);

	return  
		QI2(IVMRVideoStreamControl9)
		__super::NonDelegatingQueryInterface(riid, ppv);
}


//=============================================================================================
// CONNECTION NEGOTIATION
//=============================================================================================

// provides derived filter a chance to grab extra interfaces
HRESULT CKeystoneSubpicturePin::CheckConnect(IPin *pPin)
{
    return CBaseInputPin::CheckConnect(pPin);
}


// provides derived filter a chance to release it's extra interfaces
HRESULT CKeystoneSubpicturePin::BreakConnect()
{
    //  Can't disconnect unless stopped
    ASSERT(IsStopped());
    return CBaseInputPin::BreakConnect();
}


// Let derived class know when the input pin is connected
HRESULT CKeystoneSubpicturePin::CompleteConnect(IPin *pReceivePin)
{
    return CBaseInputPin::CompleteConnect(pReceivePin);
}


// check that we can support a given media type
HRESULT CKeystoneSubpicturePin::CheckMediaType(const CMediaType* pmt)
{
	//return S_OK;
	if (pmt->subtype == MEDIASUBTYPE_ARGB4444)
	{
		return S_OK;
	}
	return VFW_E_TYPE_NOT_ACCEPTED;
}

// set the media type for this connection
HRESULT CKeystoneSubpicturePin::SetMediaType(const CMediaType* mtIn)
{
    // Set the base class media type (should always succeed)
    HRESULT hr = CBasePin::SetMediaType(mtIn);
    //if (FAILED(hr)) {
    //    return hr;
    //}

	////Setup the grab buffer
	//m_pTransformFilter->pLastSubpictureBuffer = NULL;
	return hr;
}

HRESULT  CKeystoneSubpicturePin::GetMediaType(int iPosition, CMediaType* pMediaType)
{
	HRESULT hr = VFW_S_NO_MORE_ITEMS;
	if (iPosition == 0)
	{
		CMediaType * mt = new CMediaType();
		CreateSubpictureMediaType(mt);
		pMediaType = mt;
		hr = S_OK;
	}
	return hr;
}

HRESULT CKeystoneSubpicturePin::CreateSubpictureMediaType(CMediaType * pMediaType)
{
	VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)pMediaType->AllocFormatBuffer(sizeof VIDEOINFOHEADER2);
	ZeroMemory(VIH2, sizeof VIDEOINFOHEADER2);
	
	//BITMAPINFOHEADER
	VIH2->bmiHeader.biBitCount = 16;
	VIH2->bmiHeader.biClrImportant = 0;
	VIH2->bmiHeader.biClrUsed = 0;
	VIH2->bmiHeader.biCompression = 3;  //3 for ARGB4444, 0 for RGB8 //
	VIH2->bmiHeader.biHeight = 576;
	VIH2->bmiHeader.biPlanes = 1;
	VIH2->bmiHeader.biSize = 40;
	VIH2->bmiHeader.biSizeImage = 829440; //414720
	VIH2->bmiHeader.biWidth = 720;
	VIH2->bmiHeader.biXPelsPerMeter = 0;
	VIH2->bmiHeader.biYPelsPerMeter = 0;

	VIH2->rcSource.bottom = 576;
	VIH2->rcSource.right = 720;
	VIH2->rcSource.left = 0;
	VIH2->rcSource.top = 0;

	VIH2->rcTarget.bottom = 576;
	VIH2->rcTarget.right = 720;
	VIH2->rcTarget.left = 0;
	VIH2->rcTarget.top = 0;

	VIH2->AvgTimePerFrame = 0;

	VIH2->dwBitErrorRate = 0;
	VIH2->dwBitRate = 159252480; // 2bytes per pixel = 159252480, 1 = 79626240
	VIH2->dwControlFlags = 0;
	VIH2->dwCopyProtectFlags = 0;
	VIH2->dwInterlaceFlags = 0;
	VIH2->dwPictAspectRatioX = 16;
	VIH2->dwPictAspectRatioY = 9;

	//pMediaType->SetFormat((BYTE*) &VIH2, 112);
	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetFormatType(&FORMAT_VideoInfo2);
	pMediaType->SetTemporalCompression(FALSE);
	pMediaType->bFixedSizeSamples = TRUE;
	pMediaType->SetSubtype(&MEDIASUBTYPE_ARGB4444);  //MEDIASUBTYPE_AI44  MEDIASUBTYPE_ARGB4444 MEDIASUBTYPE_DVD_SUBPICTURE
	pMediaType->SetSampleSize(829440); //829440 for argb4444, 414720
	return S_OK;
}

STDMETHODIMP  CKeystoneSubpicturePin::QueryAccept(const AM_MEDIA_TYPE* pmt)
{
	return __super::QueryAccept(pmt);
}

STDMETHODIMP CKeystoneSubpicturePin::ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt)
{
	CAutoLock lock(m_pLock);

   	HRESULT hr = CheckConnect(pConnector);
	if (SUCCEEDED(hr))
	{
        CMediaType* pcmt = (CMediaType*)pmt;
		hr = CheckMediaType(pcmt);
		if (SUCCEEDED(hr))
			hr = pConnector->QueryAccept(pcmt);
		if (hr == S_OK)
		{
			if (m_Connected != NULL)
				m_Connected->Release();
			m_Connected = pConnector;
			m_Connected->AddRef();
			hr = SetMediaType(pcmt);
			if (SUCCEEDED(hr))
				hr = CompleteConnect(pConnector);

			if (FAILED(hr) && (m_Connected != NULL))
			{
				m_Connected->Release();
				m_Connected = NULL;
			}
		}
	}
	return hr;
}


//=============================================================================================
// IMemInputPin interface
//=============================================================================================

// provide EndOfStream that passes straight downstream
// (there is no queued data)
STDMETHODIMP CKeystoneSubpicturePin::EndOfStream(void)
{
    CAutoLock lck(&m_pTransformFilter->m_csReceive);
    HRESULT hr = CheckStreaming();
    //if (S_OK == hr) {
    //   hr = m_pTransformFilter->EndOfStream();
    //}
    return hr;
}


// enter flushing state. Call default handler to block Receives, then pass to overridable method in filter
STDMETHODIMP CKeystoneSubpicturePin::BeginFlush(void)
{
    CAutoLock lck(&m_pTransformFilter->m_csFilter);
    //  Are we actually doing anything?
    ASSERT(m_pTransformFilter->m_pOutput != NULL);
    if (!IsConnected() ||
        !m_pTransformFilter->m_pOutput->IsConnected()) {
        return VFW_E_NOT_CONNECTED;
    }
    HRESULT hr = CBaseInputPin::BeginFlush();
    if (FAILED(hr)) {
    	return hr;
    }

    //return m_pTransformFilter->BeginFlush();
	return hr;
}


// leave flushing state.
// Pass to overridable method in filter, then call base class
// to unblock receives (finally)
STDMETHODIMP CKeystoneSubpicturePin::EndFlush(void)
{
    CAutoLock lck(&m_pTransformFilter->m_csFilter);
    //  Are we actually doing anything?
    ASSERT(m_pTransformFilter->m_pOutput != NULL);
    if (!IsConnected() ||
        !m_pTransformFilter->m_pOutput->IsConnected()) {
        return VFW_E_NOT_CONNECTED;
    }

    //HRESULT hr = m_pTransformFilter->EndFlush();
    //if (FAILED(hr)) {
    //    return hr;
    //}

    return CBaseInputPin::EndFlush();
}


//=============================================================================================
// DATA TRAFFICING
//=============================================================================================

// here's the next block of data from the stream.
// AddRef it yourself if you need to hold it beyond the end
// of this call.
HRESULT CKeystoneSubpicturePin::Receive(IMediaSample * pSample)
{
	//Beep(1000, 1);

	HRESULT hr;

    CAutoLock lck(&m_csReceiveSubpicture);
    ASSERT(pSample);

    // check all is well with the base class
    hr = CBaseInputPin::Receive(pSample);
    if (S_OK != hr) return hr;

	REFERENCE_TIME rtS = 0;
	REFERENCE_TIME rtE = 0;
	pSample->GetTime(&rtS, &rtE);

	hr = pSample->IsSyncPoint();
    if ((hr == S_FALSE) && (rtS != 0) && (rtE != 0))
	{
		//TODO: we need to mix the last received valid data again
		//It may be doing it already because the pointer is still valid for the last
		//'real' sample so it should just keep mixing it.
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: SyncPoint SP Sample Received: FALSE.")));
		return S_OK;
	}
	else
	{
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: SyncPoint SP Sample Received: TRUE.")));
	}

	//Check for MediaType changes
	CMediaType *mSource = 0;
    if (S_OK == pSample->GetMediaType((AM_MEDIA_TYPE**)&mSource) && mSource)
	{
		m_pTransformFilter->PISI_MediaType = *mSource;
	}

	//Sample grabber - Subpicture only
	if ((m_pTransformFilter->SAGR_GetSample == true) && (m_pTransformFilter->USEROPTION_SampleWhat == 1))
	{
		m_pTransformFilter->SAGR_GetSample = false;
		m_pTransformFilter->SAGR_HandleSampleGrab(pSample);
	}

    // process the incoming data
	if (BSubpictureStreamIsActive == TRUE)
	{
		return m_pTransformFilter->Pipeline_Subpicture_ReceiveSubpicture(pSample);
	}
	else
	{
		return S_OK;
	}
}


// Check streaming status
HRESULT CKeystoneSubpicturePin::CheckStreaming()
{
    ASSERT(m_pTransformFilter->m_pOutput != NULL);
    if (!m_pTransformFilter->m_pOutput->IsConnected()) {
        return VFW_E_NOT_CONNECTED;
    } else {
        //  Shouldn't be able to get any data if we're not connected!
        ASSERT(IsConnected());

        //  we're flushing
        if (m_bFlushing) {
            return S_FALSE;
        }
        //  Don't process stuff in Stopped state
        if (IsStopped()) {
            return VFW_E_WRONG_STATE;
        }
        if (m_bRunTimeError) {
    	    return VFW_E_RUNTIME_ERROR;
        }
        return S_OK;
    }
}

// override to pass downstream
STDMETHODIMP CKeystoneSubpicturePin::NewSegment(
    REFERENCE_TIME tStart,
    REFERENCE_TIME tStop,
    double dRate)
{
    //  Save the values in the pin
    CBasePin::NewSegment(tStart, tStop, dRate);
   
	return S_OK;
}

STDMETHODIMP CKeystoneSubpicturePin::NotifyAllocator(IMemAllocator* pAllocator, BOOL bReadOnly)
{
	CAutoLock lock(m_pLock);
	HRESULT hr;

	////DEBUGGING
	//ALLOCATOR_PROPERTIES * ap = new ALLOCATOR_PROPERTIES();
	//hr = pAllocator->GetProperties(ap);
	//delete ap;
	////DEBUGGING

	hr = __super::NotifyAllocator(pAllocator, bReadOnly);
	return hr;
}

STDMETHODIMP CKeystoneSubpicturePin::GetAllocator(IMemAllocator **ppAllocator)
{
    CheckPointer(ppAllocator, E_POINTER);
    if (m_pAllocator)  
    {
        // We already have an allocator, so return that one.
        *ppAllocator = m_pAllocator;
        (*ppAllocator)->AddRef();
        return S_OK;
    }

    // No allocator yet, so propose our custom allocator. The exact code
    // here will depend on your custom allocator class definition.
    HRESULT hr = S_OK;
    CSPAllocator *pAlloc = new CSPAllocator(this, &hr);
    if (!pAlloc)
    {
        return E_OUTOFMEMORY;
    }
    if (FAILED(hr))
    {
        delete pAlloc;
        return hr;
    }
    // Return the IMemAllocator interface to the caller.
    return pAlloc->QueryInterface(IID_IMemAllocator, (void**)ppAllocator);
}

//----------------------------------------------------------------------------
// GetAllocatorRequirements: The upstream filter calls this to get our
// filter's allocator requirements. If the app has set the buffer, then
// we return those props. Otherwise, we use the default TransInPlace behavior.
//----------------------------------------------------------------------------

HRESULT CKeystoneSubpicturePin::GetAllocatorRequirements( ALLOCATOR_PROPERTIES *pProps )
{
    CheckPointer(pProps,E_POINTER);

    if (m_pPrivateAllocator)
    {
        *pProps = m_allocprops;
        return S_OK;
    }
    else
    {
		return __super::GetAllocatorRequirements(pProps);
    }
}
