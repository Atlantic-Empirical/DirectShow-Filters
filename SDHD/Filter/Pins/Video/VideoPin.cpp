#include "../../CKeystone.h"
#include <dvdmedia.h>
#include <string>

// =================================================================
// Implements the CKeystoneVideoPin class
// =================================================================

// constructor
CKeystoneVideoPin::CKeystoneVideoPin(
    TCHAR *pObjectName,
    CKeystone *pTransformFilter,
    HRESULT * phr,
    LPCWSTR pName)
    : CBaseInputPin(pObjectName, pTransformFilter, &pTransformFilter->m_csFilter, phr, pName)
{
    DbgLog((LOG_TRACE,2,TEXT("CKeystoneVideoPin::CKeystoneVideoPin")));
    m_pTransformFilter = pTransformFilter;
	FrameCnt = 0;

	//Init Gabor's resizing code
	//TODO: de and reinit this each time the video size changes on input pin
	ZoomInit(1920, 1080);
	ResizeMode = 1;
}

#ifdef UNICODE
CKeystoneVideoPin::CKeystoneVideoPin(
    CHAR *pObjectName,
    CKeystone *pTransformFilter,
    HRESULT * phr,
    LPCWSTR pName)
    : CBaseInputPin(pObjectName, pTransformFilter, &pTransformFilter->m_csFilter, phr, pName)
{
    DbgLog((LOG_TRACE,2,TEXT("CKeystoneVideoPin::CKeystoneVideoPin")));
    m_pTransformFilter = pTransformFilter;
	bRun32 = false;
	s32Counter = 0;
	p32BufferA = NULL;
	p32BufferB = NULL;
	b32TopFieldFirst = false;
	bFrameStepping = false;
	bInVarispeed = false;
	bNavDecRunning = true;
	bFrameStepUnderway = false;
	bRestartTimeStamps = false;

	//FrameStepping:
	m_hQAccessMutex = CreateMutex( NULL, FALSE, NULL );	// a mutex to make sure our queue is threadsafe
}
#endif

#define QI2(i) (riid == IID_##i) ? GetInterface((i*)this, ppv) :

STDMETHODIMP CKeystoneVideoPin::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	CheckPointer(ppv, E_POINTER);

	return 
		//QI2(IVMRVideoStreamControl)
		QI2(IKsPropertySet)
		__super::NonDelegatingQueryInterface(riid, ppv);
}


//=============================================================================================
// CONNECTION NEGOTIATION
//=============================================================================================

// provides derived filter a chance to grab extra interfaces
HRESULT CKeystoneVideoPin::CheckConnect(IPin *pPin)
{
    return CBaseInputPin::CheckConnect(pPin);
}

// provides derived filter a chance to release it's extra interfaces
HRESULT CKeystoneVideoPin::BreakConnect()
{
    //  Can't disconnect unless stopped
    ASSERT(IsStopped());
    //m_pTransformFilter->BreakConnect(PINDIR_INPUT);
    return CBaseInputPin::BreakConnect();
}

// Let derived class know when the input pin is connected
HRESULT CKeystoneVideoPin::CompleteConnect(IPin *pReceivePin)
{
    return CBaseInputPin::CompleteConnect(pReceivePin);
}

// check that we can support a given media type
HRESULT CKeystoneVideoPin::CheckMediaType(const CMediaType* pmt)
{
	HRESULT hr = VFW_E_TYPE_NOT_ACCEPTED;
	if (pmt->majortype == MEDIATYPE_Video)
	{
		if (pmt->subtype == MEDIASUBTYPE_YUY2)
		{
			if ((pmt->formattype == FORMAT_VideoInfo2) || (pmt->formattype == FORMAT_VideoInfo))
			{
				void* info = pmt->Format();
				if (info != NULL)
                    hr = S_OK;
			}
		}
	}
	return hr;
}

// set the media type for this connection
HRESULT CKeystoneVideoPin::SetMediaType(const CMediaType* mtIn)
{
    // Set the base class media type (should always succeed)
    HRESULT hr = CBasePin::SetMediaType(mtIn);
    if (FAILED(hr)) {
        return hr;
    }
	return hr;
}

HRESULT CKeystoneVideoPin::GetMediaType(int iPosition, CMediaType* pMediaType)
{
	//TODO: make this call getvideomediatype
	HRESULT hr = VFW_S_NO_MORE_ITEMS;
	if (iPosition == 0)
	{
		pMediaType->majortype = MEDIATYPE_Video;
		pMediaType->subtype = MEDIASUBTYPE_YUY2;
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP  CKeystoneVideoPin::QueryAccept(const AM_MEDIA_TYPE* pmt)
{
	return __super::QueryAccept(pmt);
}

STDMETHODIMP CKeystoneVideoPin::ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt)
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

// provide EndOfStream that passes straight downstream (there is no queued data)
STDMETHODIMP CKeystoneVideoPin::EndOfStream(void)
{
    CAutoLock lck(&m_pTransformFilter->m_csReceive);
    HRESULT hr = CheckStreaming();
    if (S_OK == hr) {
       hr = m_pTransformFilter->EndOfStream();
    }
    return hr;
}

// enter flushing state. Call default handler to block Receives, then pass to overridable method in filter
STDMETHODIMP CKeystoneVideoPin::BeginFlush(void)
{
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Video Pin - Begin Flush")));
    CAutoLock lck(&m_pTransformFilter->m_csFilter);
    //  Are we actually doing anything?
    ASSERT(m_pTransformFilter->m_pOutput != NULL);
    if (!IsConnected() || !m_pTransformFilter->m_pOutput->IsConnected()) {
        return VFW_E_NOT_CONNECTED;
    }
    HRESULT hr = CBaseInputPin::BeginFlush();
    if (FAILED(hr)) {
    	return hr;
    }

	hr = m_pTransformFilter->BeginFlush();
	if (FAILED(hr)) {
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: BF HR: %d"), hr));
        return hr;
    }

    return hr;
}

// leave flushing state. Pass to overridable method in filter, then call base class to 
// unblock receives (finally)
STDMETHODIMP CKeystoneVideoPin::EndFlush(void)
{
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Video Pin - End Flush")));
    CAutoLock lck(&m_pTransformFilter->m_csFilter);
    //  Are we actually doing anything?
    ASSERT(m_pTransformFilter->m_pOutput != NULL);
    if (!IsConnected() || !m_pTransformFilter->m_pOutput->IsConnected()) {
        return VFW_E_NOT_CONNECTED;
    }

    HRESULT hr = m_pTransformFilter->EndFlush();
    if (FAILED(hr)) {
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: EF HR: %d"), hr));
        return hr;
    }

	//bNewSegmentReceived = true;
    return CBaseInputPin::EndFlush();
}

// override to pass downstream
STDMETHODIMP CKeystoneVideoPin::NewSegment( REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	CAutoLock lck2(&m_pTransformFilter->m_csReceive);
	
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: NS %I64d %I64d %.3f"), tStart, tStop, dRate));

	m_pTransformFilter->m_OutTime = tStart;
	m_pTransformFilter->m_InTime = tStart;

	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: New Segment")));
	bNewSegmentReceived = true;
 
	if (m_pTransformFilter->m_pOutput != NULL) {
        HRESULT hr = m_pTransformFilter->m_pOutput->DeliverNewSegment(tStart, tStop, dRate);
		if (FAILED(hr)) {
			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: NS HR: %d"), hr));
		}
		return hr;
    }
    return S_OK;

	////Old way
	////  Save the values in the pin
 //   CBasePin::NewSegment(tStart, tStop, dRate);
 //   
	////Overridden in derived filter so this shouldn't matter
	//return S_OK;
	////return m_pTransformFilter->NewSegment(tStart, tStop, dRate);
}

//=============================================================================================
// DATA TRAFFICKING
//=============================================================================================

// here's the next block of data from the stream. AddRef it yourself if you need to hold it beyond 
// the end of this call.

HRESULT CKeystoneVideoPin::Receive(IMediaSample * pSample)
{
	//just rezise and call initializeoutputsample then delivermixedsample
	HRESULT hr = E_FAIL;

	//get pointer to the incoming buffer
	LPBYTE src = NULL;
	hr = pSample->GetPointer(&src);
	if (FAILED(hr)) return hr;

	//initialize the output sample
	IMediaSample * pOut = NULL;
	hr = m_pTransformFilter->InitializeOutputSample(pSample, &pOut);
	if (FAILED(hr)) return hr;

	//get a pointer to the output buffer
	LPBYTE dst = NULL;
	pOut->GetPointer(&dst);

	//resize from the source (SD) buffer to the outgoing (HD) buffer
	hr = ResizeToHD(src, dst);
	if (FAILED(hr)) return hr;

	//NOTE: this probably still needs YUV2->UYVY conversion
	//m_pTransformFilter->CopyYUY2toUYVYRect(tb, 0, 0, 3840, dst, 0, 0, 3840, 1920, 1080);

	//deliver the sample
	return m_pTransformFilter->DeliverMixedSample(pOut);
}

HRESULT CKeystoneVideoPin::ResizeToHD(LPBYTE src, LPBYTE TargetBuffer)
{
	//This must call ZoomYUY2 to resize NTSC or PAL to HD
	ZoomYUY2(src, 1920, 1080, m_pTransformFilter->VIH2In->rcTarget, TargetBuffer);
	//check for valid TargetBuffer pointer here
	return S_OK;
}

HRESULT CKeystoneVideoPin::LogMPEGFlags(IMediaSample * pIn)
{
	//MPEG FLAGS
	IMediaSample2 * IMS2 = NULL;
	if (FAILED(pIn->QueryInterface(IID_IMediaSample2, (void**) &IMS2)))
	{
		return S_FALSE;
	}
	AM_SAMPLE2_PROPERTIES SampProps;
	if (FAILED(IMS2->GetProperties(sizeof(AM_SAMPLE2_PROPERTIES), (BYTE*)&SampProps)))
	{
		return S_FALSE;
	}
	IMS2->Release();

	DWORD dwFlags = SampProps.dwTypeSpecificFlags;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: dwFlags: %d"), dwFlags));
	bool progressive_frame = (dwFlags >> 14) & 1;
	if (progressive_frame == 0)
	{
		progressive_frame = 1;
	}
	else
	{
		progressive_frame = 0;
	}

	dwFlags = SampProps.dwTypeSpecificFlags;
	BYTE top_field_first = (dwFlags >> 17) & 1;

	dwFlags = SampProps.dwTypeSpecificFlags;
	BYTE picture_structure = (dwFlags & 0xC0000L) >> 18;

	dwFlags = SampProps.dwTypeSpecificFlags;
	bool repeat_first_field = (dwFlags >> 20) & 1;

	dwFlags = SampProps.dwTypeSpecificFlags;
	bool progressive_sequence = (dwFlags >> 21) & 1;

	dwFlags = SampProps.dwTypeSpecificFlags;

	REFERENCE_TIME atpf = m_pTransformFilter->llTargetFR_In_ATPF;

	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: --")));
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ENCODE FRAME: %d, rff=%d, tff=%d, atpf=%I64d, ps=%d, pf=%d, picst=%d"), iEncodeFrameNo, repeat_first_field, top_field_first, atpf, progressive_sequence, progressive_frame, picture_structure));
	return S_OK;
}


// Check streaming status
HRESULT CKeystoneVideoPin::CheckStreaming()
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

STDMETHODIMP CKeystoneVideoPin::NotifyAllocator(IMemAllocator* pAllocator, BOOL bReadOnly)
{
	CAutoLock lock(m_pLock);
	HRESULT hr = __super::NotifyAllocator(pAllocator, bReadOnly);
	return hr;
}

