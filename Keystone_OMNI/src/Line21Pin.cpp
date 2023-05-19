#include "appincludes.h"
//#include "CKeystone.h"
//#include <dvdmedia.h>

// =================================================================
// Implements the CKeystoneLine21Pin class
// =================================================================

// constructor
CKeystoneLine21Pin::CKeystoneLine21Pin(
    TCHAR *pObjectName,
    CKeystone *pTransformFilter,
    HRESULT * phr,
    LPCWSTR pName)
    : CBaseInputPin(pObjectName, pTransformFilter, &pTransformFilter->m_csFilter, phr, pName)
{
    DbgLog((LOG_TRACE,2,TEXT("CKeystoneLine21Pin::CKeystoneLine21Pin")));
    m_pTransformFilter = pTransformFilter;
	bL21StreamIsActive = false;
	LastFrameReceived_RT = 0;
}

#ifdef UNICODE
CKeystoneLine21Pin::CKeystoneLine21Pin(
    CHAR *pObjectName,
    CKeystone *pTransformFilter,
    HRESULT * phr,
    LPCWSTR pName)
    : CBaseInputPin(pObjectName, pTransformFilter, &pTransformFilter->m_csFilter, phr, pName)
{
    DbgLog((LOG_TRACE,2,TEXT("CKeystoneLine21Pin::CKeystoneLine21Pin")));
    m_pTransformFilter = pTransformFilter;
	bL21StreamIsActive = false;
}
#endif

CKeystoneLine21Pin::~CKeystoneLine21Pin()
{
}


#define QI2(i) (riid == IID_##i) ? GetInterface((i*)this, ppv) :

STDMETHODIMP CKeystoneLine21Pin::NonDelegatingQueryInterface(REFIID riid, void** ppv)
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
HRESULT CKeystoneLine21Pin::CheckConnect(IPin *pPin)
{
    return CBaseInputPin::CheckConnect(pPin);
}


// provides derived filter a chance to release it's extra interfaces
HRESULT CKeystoneLine21Pin::BreakConnect()
{
    //  Can't disconnect unless stopped
    ASSERT(IsStopped());
    return CBaseInputPin::BreakConnect();
}


// Let derived class know when the input pin is connected
HRESULT CKeystoneLine21Pin::CompleteConnect(IPin *pReceivePin)
{
    return CBaseInputPin::CompleteConnect(pReceivePin);
}


// check that we can support a given media type
HRESULT CKeystoneLine21Pin::CheckMediaType(const CMediaType* pmt)
{
	if (pmt->subtype == MEDIASUBTYPE_RGB8)
	{
		return S_OK;
	}
	else
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
}


// set the media type for this connection
HRESULT CKeystoneLine21Pin::SetMediaType(const CMediaType* mtIn)
{
    // Set the base class media type (should always succeed)
    HRESULT hr = CBasePin::SetMediaType(mtIn);
	//if (FAILED(hr)) {
 //       return hr;
 //   }

	////Setup the L21 grab buffer
	//m_pTransformFilter->pLastL21Buffer = NULL;

	return hr;
}

HRESULT  CKeystoneLine21Pin::GetMediaType(int iPosition, CMediaType* pMediaType)
{
	HRESULT hr = VFW_S_NO_MORE_ITEMS;
	if (iPosition == 0)
	{
		hr = CreateLine21MediaType(pMediaType);
	}
	return hr;
}

HRESULT CKeystoneLine21Pin::CreateLine21MediaType(CMediaType * pMediaType)
{
	VIDEOINFOHEADER * VIH = (VIDEOINFOHEADER*)pMediaType->AllocFormatBuffer(sizeof VIDEOINFOHEADER2);
	ZeroMemory(VIH, sizeof VIDEOINFOHEADER);
	
	//BITMAPINFOHEADER
	VIH->bmiHeader.biBitCount = 8;  //16
	VIH->bmiHeader.biClrImportant = 0;
	VIH->bmiHeader.biClrUsed = 256;
	VIH->bmiHeader.biCompression = 0; //3 for ARGB4444
	VIH->bmiHeader.biHeight = 480;
	VIH->bmiHeader.biPlanes = 1;
	VIH->bmiHeader.biSize = 40;
	VIH->bmiHeader.biSizeImage = 307200;
	VIH->bmiHeader.biWidth = 640;
	VIH->bmiHeader.biXPelsPerMeter = 0;
	VIH->bmiHeader.biYPelsPerMeter = 0;

	VIH->rcSource.bottom = 480;
	VIH->rcSource.right = 640;
	VIH->rcSource.left = 0;
	VIH->rcSource.top = 0;

	VIH->rcTarget.bottom = 480;
	VIH->rcTarget.right = 640;
	VIH->rcTarget.left = 0;
	VIH->rcTarget.top = 0;

	VIH->AvgTimePerFrame = 333667;
	VIH->dwBitErrorRate = 0;
	VIH->dwBitRate = 73654272;

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetFormatType(&FORMAT_VideoInfo);
	pMediaType->SetTemporalCompression(FALSE);
	pMediaType->bFixedSizeSamples = true;
	pMediaType->SetSubtype(&MEDIASUBTYPE_RGB8);
	pMediaType->SetSampleSize(307200);

	return S_OK;
}

STDMETHODIMP  CKeystoneLine21Pin::QueryAccept(const AM_MEDIA_TYPE* pmt)
{
	return __super::QueryAccept(pmt);
}

STDMETHODIMP CKeystoneLine21Pin::ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt)
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
STDMETHODIMP CKeystoneLine21Pin::EndOfStream(void)
{
    CAutoLock lck(&m_pTransformFilter->m_csReceive);
    HRESULT hr = CheckStreaming();
    return hr;
}


// enter flushing state. Call default handler to block Receives, then
// pass to overridable method in filter
STDMETHODIMP CKeystoneLine21Pin::BeginFlush(void)
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

    return hr;
}


// leave flushing state.
// Pass to overridable method in filter, then call base class
// to unblock receives (finally)
STDMETHODIMP CKeystoneLine21Pin::EndFlush(void)
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

//This confirms that there are non pink pixels in the L21 sample before using it for a frame grab.
bool CKeystoneLine21Pin::L21ByteCheck(IMediaSample * pSample)
{
	LPBYTE buf = NULL;
	pSample->GetPointer(&buf);
	for (int i = 0; i < 102400; i++, buf++)
	{
		if (buf[0] != 0xfd)
		{
			return true;
		}
	}
	return false;
}

//=============================================================================================
// DATA TRAFFICKING
//=============================================================================================

// here's the next block of data from the stream.
// AddRef it yourself if you need to hold it beyond the end
// of this call.
HRESULT CKeystoneLine21Pin::Receive(IMediaSample * pSample)
{
	//Beep(1000, 1);

	//DEBUGGING - DUMP SAMPLES
	//To save samples
	//if (NULL != m_pTransformFilter->pLastL21Buffer)
	//{
	//	free(m_pTransformFilter->pLastL21Buffer);
	//}
	//m_pTransformFilter->pLastL21Buffer = NULL;
	//m_pTransformFilter->pLastL21Buffer = (LPBYTE)malloc(829440);
	//pSample->GetPointer(&m_pTransformFilter->pLastL21Buffer);
	//END DEBUGGING - DUMP SAMPLES

    HRESULT hr;

	CAutoLock lck(&m_csReceiveLine21);
    ASSERT(pSample);

    // check all is well with the base class
    hr = CBaseInputPin::Receive(pSample);
    if (S_OK != hr) return hr;

	//Check for MediaType changes
	CMediaType *mSource = 0;
    if (S_OK == pSample->GetMediaType((AM_MEDIA_TYPE**)&mSource) && mSource)
	{
		m_pTransformFilter->PILI_MediaType = *mSource;
	}

	hr = pSample->IsSyncPoint(); 
	if (S_OK == hr)
	{
		//Sample grabber - Line21 only
		if ((m_pTransformFilter->SAGR_GetSample == true) && (m_pTransformFilter->USEROPTION_SampleWhat == 2))
		{
			DbgLog((LOG_TRACE, 0, TEXT("Keystone: Attempting L21 ByteCheck")));
			if (L21ByteCheck(pSample) == true)
			{
				m_pTransformFilter->SAGR_GetSample = false;
				m_pTransformFilter->SAGR_HandleSampleGrab(pSample);
			}
		}
	}

	if (bL21StreamIsActive)
	{
		m_pTransformFilter->Pipeline_Line21_ReceiveLine21(pSample);
		return S_OK;
	}
	else
	{
		m_pTransformFilter->L21_IsInitialized = true;
		return S_OK;
	}
}

// Check streaming status
HRESULT CKeystoneLine21Pin::CheckStreaming()
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
STDMETHODIMP CKeystoneLine21Pin::NewSegment(
    REFERENCE_TIME tStart,
    REFERENCE_TIME tStop,
    double dRate)
{
    CBasePin::NewSegment(tStart, tStop, dRate);
	return S_OK;
}

STDMETHODIMP CKeystoneLine21Pin::NotifyAllocator(IMemAllocator* pAllocator, BOOL bReadOnly)
{
	return __super::NotifyAllocator(pAllocator, bReadOnly);
}
