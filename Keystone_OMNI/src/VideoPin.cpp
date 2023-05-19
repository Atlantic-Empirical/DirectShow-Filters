#include "appincludes.h"
//#include "CKeystone.h"
//#include <dvdmedia.h>
//#include "Mixer.h"
//#include <string>

// =================================================================
// Implements the CKeystoneVideoPin class
// =================================================================

// constructor
CKeystoneVideoPin::CKeystoneVideoPin( TCHAR *pObjectName, CKeystone *pTransformFilter, HRESULT * phr, LPCWSTR pName)
    : CBaseInputPin(pObjectName, pTransformFilter, &pTransformFilter->m_csFilter, phr, pName)
{
    DbgLog((LOG_TRACE,2,TEXT("CKeystoneVideoPin::CKeystoneVideoPin")));
    m_pTransformFilter = pTransformFilter;
}

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
	////FROM TEARDOWN
	//HRESULT hr = VFW_E_TYPE_NOT_ACCEPTED;
	//if (pmt->majortype == MEDIATYPE_Video)
	//{
	//	if (pmt->subtype == MEDIASUBTYPE_UYVY)
	//	{
	//		if ((pmt->formattype == FORMAT_VideoInfo2) || (pmt->formattype == FORMAT_VideoInfo))
	//		{
	//			VIDEOINFOHEADER2 * VIH2In = (VIDEOINFOHEADER2*)pmt->Format();
	//			void* info = pmt->Format();
	//			if (info != NULL)
 //                   hr = S_OK;
	//		}
	//	}
	//}
	//return hr;

	//OLD - WORKS WITH PHOENIX
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
	m_pTransformFilter->HandleVideoPinConnected(&this->CurrentMediaType());
	return hr;
}

HRESULT CKeystoneVideoPin::GetMediaType(int iPosition, CMediaType* pMediaType)
{
	////FROM TEARDOWN
	//HRESULT hr = VFW_S_NO_MORE_ITEMS;
	//if (iPosition == 0)
	//{
	//	pMediaType->majortype = MEDIATYPE_Video;
	//	pMediaType->subtype = MEDIASUBTYPE_UYVY;
	//	hr = S_OK;
	//}
	//return hr;

	//WORKS WITH PHOENIX
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
 	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | CKeystoneVideoPin::EndOfStream()")));
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
 	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | CKeystoneVideoPin::BeginFlush()")));
	CAutoLock lck(&m_pTransformFilter->m_csFilter);
    //  Are we actually doing anything?
    ASSERT(m_pTransformFilter->m_pOutput != NULL);
    if (!IsConnected() || !m_pTransformFilter->m_pOutput->IsConnected()) 
	{
        return VFW_E_NOT_CONNECTED;
    }
    HRESULT hr = CBaseInputPin::BeginFlush();
    if (FAILED(hr)) 
	{
    	return hr;
    }

	hr = m_pTransformFilter->BeginFlush();
	if (FAILED(hr)) {
        return hr;
    }

    return hr;
}

// leave flushing state. Pass to overridable method in filter, then call base class to 
// unblock receives (finally)
STDMETHODIMP CKeystoneVideoPin::EndFlush(void)
{
 	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | CKeystoneVideoPin::EndFlush()")));

	CAutoLock lck(&m_pTransformFilter->m_csFilter);
    //  Are we actually doing anything?
    ASSERT(m_pTransformFilter->m_pOutput != NULL);
    if (!IsConnected() || !m_pTransformFilter->m_pOutput->IsConnected()) {
        return VFW_E_NOT_CONNECTED;
    }

    HRESULT hr = m_pTransformFilter->EndFlush();
    if (FAILED(hr)) {
		DbgLog((LOG_TRACE, 0, TEXT("Keystone: EF HR: %d"), hr));
        return hr;
    }

	//bNewSegmentReceived = true;
    return CBaseInputPin::EndFlush();
}

// override to pass downstream
STDMETHODIMP CKeystoneVideoPin::NewSegment( REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI: CKeystoneVideoPin::NewSegment() %I64d %I64d %.3f"), tStart, tStop, dRate));
	//CAutoLock lck2(&m_pTransformFilter->m_csReceive);
	//  Save the values in the pin
	CBasePin::NewSegment(tStart, tStop, dRate);


	//m_pTransformFilter->m_OutTime = tStart;
	//m_pTransformFilter->m_InTime = tStart;

	bNewSegmentReceived = true;
 
	if (m_pTransformFilter->m_pOutput != NULL) {
        HRESULT hr = m_pTransformFilter->m_pOutput->DeliverNewSegment(tStart, tStop, dRate);
		if (FAILED(hr)) {
			DbgLog((LOG_TRACE, 0, TEXT("Keystone: NS HR: %d"), hr));
		}
		return hr;
    }
    return S_OK;
}

//=============================================================================================
// DATA TRAFFICING
//=============================================================================================

// here's the next block of data from the stream. AddRef it yourself if you need to hold it beyond 
// the end of this call.

HRESULT CKeystoneVideoPin::Receive(IMediaSample * pSample)
{
 	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | CKeystoneVideoPin::Receive()")));
	//Beep(1000, 1);

	//TRIAL FRAME COUNT
	if (m_pTransformFilter->LOCK_FrameCount <= 0)
	{
		m_pTransformFilter->NotifyEvent(EC_KEYSTONE_TRIAL_FRAMECOUNT, 0, 0);
		while (true);
		{
			Sleep(1);
		}
	}

	//LOCK AND BASEFILTER
    CAutoLock lck(&m_pTransformFilter->m_csReceive);
    ASSERT(pSample);

    HRESULT hr = CBaseInputPin::Receive(pSample);
    if (S_OK != hr) return hr;
	if(m_bFlushing) return S_OK;

	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI: Receive()")));
	m_pTransformFilter->NotifyEvent(EC_KEYSTONE_FRAMERECEIVED, 0, 0);

	//DEBUGGING
	REFERENCE_TIME rtS = 0;
	REFERENCE_TIME rtE = 0;
	pSample->GetTime(&rtS, &rtE);
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | rtS/rtE @Receive: %I64d %I64d"), rtS, rtE));
	CRefTime crtST;
	m_pTransformFilter->StreamTime(crtST);
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | Stream time: %I64d"), crtST.m_time));
	//DEBUGGING

	////DEBUGGING
	//m_pTransformFilter->LogSampleMetadata(pSample);
	////DEBUGGING


	////DEBUGGING - FOR SCRUBBING
	//CRefTime crtST;
	//m_pTransformFilter->StreamTime(crtST);
	//Quality q;
	//q.Type = Famine;
	//q.Proportion = 4000;
	//q.Late = 1000000;
	//q.TimeStamp = crtST.m_time;
	//PassNotify(q);
	////DEBUGGING


	//CRefTime crtST;
	//m_pTransformFilter->StreamTime(crtST);
	//if (crtST.m_time > 108000000000) // The stream time is greater than three hours. This is an outside-the-box situation and implies that the graph is new and Run() has not made its way through all filters. We send NULL timestamps until we have a real stream time.
	//{
	//	LONG_PTR outST = (LONG_PTR)lround(crtST.m_time/1000);
	//	m_pTransformFilter->NotifyEvent(EC_KEYSTONE_STREAMTIME, outST, 0);
	//	pSample->Release();
	//	return S_OK;
	//}

	////DEBUGGING
	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;
	//pSample->GetTime(&rtS, &rtE);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI| rtS/rtE @Receive: %I64d %I64d"), rtS, rtE));
	////DEBUGGING

	////DEBUGGING
	//return S_OK;
	////DEBUGGING

	//// DEBUGGING
	//REFERENCE_TIME rtS1 = 0;
	//REFERENCE_TIME rtE1 = 0;
	//pSample->GetTime(&rtS1, &rtE1);
	//CRefTime crtST;
	//m_pTransformFilter->StreamTime(crtST);
	//LONG_PTR outST = (LONG_PTR)lround(crtST.m_time/1000);
	//m_pTransformFilter->NotifyEvent(EC_KEYSTONE_PRESENTATIONTIMES, rtS1, rtE1);
	//m_pTransformFilter->NotifyEvent(EC_KEYSTONE_STREAMTIME, outST, 0);
	//// DEBUGGING

	
	////DEBUGGING - DIRECT DELIVERY OF SAMPLE
 //   IMediaSample * pOut = NULL;
 //   hr = m_pTransformFilter->InitializeOutputSample(pSample, &pOut);
 //   if (FAILED(hr)) return hr;
	//LPBYTE dst = NULL;
	//LPBYTE src = NULL;
	//pOut->GetPointer(&dst);
	//pSample->GetPointer(&src);
	//m_pTransformFilter->CopyBuffer_Rect_YUY2toUYVY(src, 0, 0, m_pTransformFilter->PIVI_Pitch, dst, 0, 0, m_pTransformFilter->PIVO_Pitch, m_pTransformFilter->PIVI_ConnectedWidth, m_pTransformFilter->PIVI_ConnectedHeight);
	//return m_pTransformFilter->Pipeline_Core_DeliverMixedSample(pOut);
	////DEBUGGING - DIRECT DELIVERY OF SAMPLE

	return m_pTransformFilter->Pipeline_Video_ReceiveVideo(pSample);
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

//=============================================================================================
// Allocator Stuff
//=============================================================================================

STDMETHODIMP CKeystoneVideoPin::NotifyAllocator(IMemAllocator* pAllocator, BOOL bReadOnly)
{
	//FROM TEARDOWN
    CheckPointer(pAllocator,E_POINTER);
    ValidateReadPtr(pAllocator,sizeof(IMemAllocator));
    CAutoLock cObjectLock(m_pLock);

	ALLOCATOR_PROPERTIES * ap = new ALLOCATOR_PROPERTIES();
	HRESULT hr = pAllocator->GetProperties(ap);
	delete ap;

    IMemAllocator *pOldAllocator = m_pAllocator;
    pAllocator->AddRef();
    m_pAllocator = pAllocator;

    if (pOldAllocator != NULL) {
        pOldAllocator->Release();
    }

    // the readonly flag indicates whether samples from this allocator should
    // be regarded as readonly - if true, then inplace transforms will not be
    // allowed.
    m_bReadOnly = (BYTE)bReadOnly;
    //return NOERROR;

	CAutoLock lock(m_pLock);
	hr = __super::NotifyAllocator(pAllocator, bReadOnly);
	return hr;
}

