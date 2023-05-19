#include "../../CKeystone.h"
#include <dvdmedia.h>
#include "../../../Utility/appincludes.h"

// =================================================================
// Implements the CKeystoneOutputPin class
// =================================================================

// constructor

CKeystoneOutputPin::CKeystoneOutputPin(
    TCHAR *pObjectName,
    CKeystone *pTransformFilter,
    HRESULT * phr,
    LPCWSTR pPinName)
    : CBaseOutputPin(pObjectName, pTransformFilter, &pTransformFilter->m_csFilter, phr, pPinName),
      m_pPosition(NULL)
{
    DbgLog((LOG_TRACE,2,TEXT("CKeystoneOutputPin::CKeystoneOutputPin")));
    m_pTransformFilter = pTransformFilter;
}

#ifdef UNICODE
CKeystoneOutputPin::CKeystoneOutputPin(
    CHAR *pObjectName,
    CKeystone *pTransformFilter,
    HRESULT * phr,
    LPCWSTR pPinName)
    : CBaseOutputPin(pObjectName, pTransformFilter, &pTransformFilter->m_csFilter, phr, pPinName),
      m_pPosition(NULL)
{
    DbgLog((LOG_TRACE,2,TEXT("CKeystoneOutputPin::CKeystoneOutputPin")));
    m_pTransformFilter = pTransformFilter;

}
#endif

// destructor

CKeystoneOutputPin::~CKeystoneOutputPin()
{
    DbgLog((LOG_TRACE,2,TEXT("CKeystoneOutputPin::~CKeystoneOutputPin")));

    if (m_pPosition) m_pPosition->Release();
}


// overriden to expose IMediaPosition and IMediaSeeking control interfaces

STDMETHODIMP
CKeystoneOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    CheckPointer(ppv,E_POINTER);
    ValidateReadWritePtr(ppv,sizeof(PVOID));
    *ppv = NULL;

    if (riid == IID_IMediaPosition || riid == IID_IMediaSeeking) {

        // we should have an input pin by now

        ASSERT(m_pTransformFilter->m_pVideo != NULL);

        if (m_pPosition == NULL) {

            HRESULT hr = CreatePosPassThru(
                             GetOwner(),
                             FALSE,
                             (IPin *)m_pTransformFilter->m_pVideo,
                             &m_pPosition);
            if (FAILED(hr)) {
                return hr;
            }
        }
        return m_pPosition->QueryInterface(riid, ppv);
    } else {
        return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv);
    }
}


// provides derived filter a chance to grab extra interfaces

HRESULT
CKeystoneOutputPin::CheckConnect(IPin *pPin)
{
    // we should have an input connection first

    ASSERT(m_pTransformFilter->m_pVideo != NULL);
    if ((m_pTransformFilter->m_pVideo->IsConnected() == FALSE)) {
	    return E_UNEXPECTED;
    }

    //HRESULT hr = m_pTransformFilter->CheckConnect(PINDIR_OUTPUT,pPin);
    //if (FAILED(hr)) {
	   // return hr;
    //}
    return CBaseOutputPin::CheckConnect(pPin);
}


// provides derived filter a chance to release it's extra interfaces

HRESULT CKeystoneOutputPin::BreakConnect()
{
    //  Can't disconnect unless stopped
    ASSERT(IsStopped());
   // m_pTransformFilter->BreakConnect(PINDIR_OUTPUT);
    return CBaseOutputPin::BreakConnect();
}


// Let derived class know when the output pin is connected

HRESULT CKeystoneOutputPin::CompleteConnect(IPin *pReceivePin)
{
    //HRESULT hr = m_pTransformFilter->CompleteConnect(PINDIR_OUTPUT,pReceivePin);
    //if (FAILED(hr)) {
    //    return hr;
    //}

	//CMediaType * mt = NULL;
	//HRESULT hr = pReceivePin->ConnectionMediaType((AM_MEDIA_TYPE*) &mt);

	//Added by TRPF on 070528 - This relies on an illegitimate cheat.
	IPin * pRendererPin = NULL;
	HRESULT hr = ConnectedTo(&pRendererPin);
	if (FAILED(hr)) return hr;
	PIN_INFO pPI;
	hr = pRendererPin->QueryPinInfo(&pPI);
	if (FAILED(hr)) return hr;
    IBaseFilter * pDL = pPI.pFilter;
	pPI.pFilter->Release();
	m_pRenderer = (CBaseRenderer*) ((ULONG)pDL - 12); //can't find any better way to do this. we're assuming that the class pointer will always be 12 bytes before the interface pointer. Through limited testing this seems to be true. According to research there isn't a proper way to get a class pointer from an interface pointer.

	return CBaseOutputPin::CompleteConnect(pReceivePin);
}


// check a given transform - must have selected input type first

HRESULT CKeystoneOutputPin::CheckMediaType(const CMediaType* pmtOut)
{
    // must have selected input first
    ASSERT(m_pTransformFilter->m_pVideo != NULL);
    if ((m_pTransformFilter->m_pVideo->IsConnected() == FALSE)) {
	        return E_INVALIDARG;
    }

	VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)pmtOut->pbFormat;
	if ((VIH2->rcSource.right > 720) || (VIH2->rcSource.bottom > 576))
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

    return m_pTransformFilter->CheckTransform(&m_pTransformFilter->m_pVideo->CurrentMediaType(), pmtOut);
}


// called after we have agreed a media type to actually set it in which case
// we run the CheckTransform function to get the output format type again

HRESULT CKeystoneOutputPin::SetMediaType(const CMediaType* pmtOut)
{
    HRESULT hr = NOERROR;
    ASSERT(m_pTransformFilter->m_pVideo != NULL);
    ASSERT(m_pTransformFilter->m_pVideo->CurrentMediaType().IsValid());

	VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)pmtOut->pbFormat;
	m_pTransformFilter->llTargetFR_Out_ATPF = VIH2->AvgTimePerFrame;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: (In pin) Out ATPF= %I64d"), VIH2->AvgTimePerFrame));

    // Set the base class media type (should always succeed)
    hr = CBasePin::SetMediaType(pmtOut);
    if (FAILED(hr)) {
        return hr;
    }

	////Just out of curiosity, let's find out about the newly connected filter.
	//if (this->IsConnected())
	//{

	//	//1) Get IPin for downstream pin
	//	IPin * pDownstreamPin = NULL;
	//	this->m_pInputPin->QueryInterface(IID_IPin, (void**) &pDownstreamPin);
 //       
	//	//2) Query IPin for Pin Info
	//	PIN_INFO PinInfo;
	//	pDownstreamPin->QueryPinInfo(&PinInfo);
	//	IBaseFilter * pDownstreamFilter = PinInfo.pFilter;
 //              
	//	//3) Use the returned IBaseFilter to call QueryFilterInfo
	//	FILTER_INFO FilterInfo;
	//	FilterInfo.pGraph = NULL;
	//	pDownstreamFilter->QueryFilterInfo(&FilterInfo);
 //       
	//	//4) What did we find?
	//	DbgLog((LOG_TRACE,0,TEXT("Downstream filter name: %S\n"), FilterInfo.achName));
	//	//DbgLog((LOG_TRACE,0,TEXT("Keystone Quality Message. Type: %s Proportion: %d Late: %d TimeStamp: %d"), Type, q.Proportion, q.Late, q.TimeStamp));
	//	delete &PinInfo;
	//	delete &FilterInfo;
	//}

#ifdef DEBUG
    if (FAILED(m_pTransformFilter->CheckTransform(&m_pTransformFilter->
					m_pVideo->CurrentMediaType(),pmtOut))) {
	DbgLog((LOG_ERROR,0,TEXT("*** This filter is accepting an output media type")));
	DbgLog((LOG_ERROR,0,TEXT("    that it can't currently transform to.  I hope")));
	DbgLog((LOG_ERROR,0,TEXT("    it's smart enough to reconnect its input.")));
    }
#endif

    //return m_pTransformFilter->SetMediaType(PINDIR_OUTPUT,pmtOut);

	return hr;
}


// pass the buffer size decision through to the main transform class

HRESULT
CKeystoneOutputPin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES* pProp)
{
	VIDEOINFOHEADER* info = (VIDEOINFOHEADER*)CurrentMediaType().Format();

	pProp->cBuffers = 8;
	pProp->cbBuffer = info->bmiHeader.biSizeImage;
	pProp->cbAlign = 1;
	pProp->cbPrefix = 0;

    // Ask the allocator to reserve us the memory
	ALLOCATOR_PROPERTIES actual;
	ZeroMemory(&actual, sizeof actual);

	HRESULT hr = pAlloc->SetProperties(pProp, &actual);
    if (SUCCEEDED(hr))
	{
		if ((actual.cbBuffer < pProp->cbBuffer) ||
			(actual.cBuffers < pProp->cBuffers))
			hr = E_FAIL;
	}
	return hr;
}

// Override this if you can do something constructive to act on the
// quality message.  Consider passing it upstream as well

// Pass the quality mesage on upstream.

STDMETHODIMP
CKeystoneOutputPin::Notify(IBaseFilter * pSender, Quality q)
{
    UNREFERENCED_PARAMETER(pSender);
    ValidateReadPtr(pSender,sizeof(IBaseFilter));

    // First see if we want to handle this ourselves
    HRESULT hr = AlterQuality(q);
    if (hr!=S_FALSE) {
        return hr;        // either S_OK or a failure
    }

    // S_FALSE means we pass the message on.
    // Find the quality sink for our input pin and send it there

    ASSERT(m_pTransformFilter->m_pVideo != NULL);

    return m_pTransformFilter->m_pVideo->PassNotify(q);

} // Notify

// Return S_FALSE to mean "pass the note on upstream"
// Return NOERROR (Same as S_OK)
// to mean "I've done something about it, don't pass it on"
HRESULT CKeystoneOutputPin::AlterQuality(Quality q)
{
	//Beep(1000, 1000); //Debugging, 1s beep when quality message is received.
	TCHAR * Type = TEXT("Famine");
	if (q.Type == 1)
	{
		Type = TEXT("Flood");
	}
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone Quality Message. Type: %s Proportion: %d Late: %d TimeStamp: %d"), Type, q.Proportion, q.Late, q.TimeStamp));
	//m_pTransformFilter->NotifyEvent(EC_QUALITY_CHANGE_KEYSTONE, q.Proportion, (LONG_PTR)q.Late);
	UNREFERENCED_PARAMETER(q);
    return S_FALSE;
    //return S_OK;
}

IMemAllocator* CKeystoneOutputPin::GetAllocator()
{
	return m_pAllocator;
}

HRESULT CKeystoneOutputPin::GetMediaType(int iPosition, CMediaType* pmt)
{
	HRESULT hr = VFW_S_NO_MORE_ITEMS;
	if ((iPosition == 0) && m_pTransformFilter->m_pVideo->IsConnected())
	{
		int h,w;
		REFERENCE_TIME ft;
		if (m_pTransformFilter->m_pVideo->CurrentMediaType().formattype == FORMAT_VideoInfo2)
		{
			VIDEOINFOHEADER2* inType = (VIDEOINFOHEADER2*)m_pTransformFilter->m_pVideo->CurrentMediaType().Format();
			h = inType->bmiHeader.biHeight;
			w = inType->bmiHeader.biWidth;
		}
		else
		{
			VIDEOINFOHEADER* inType = (VIDEOINFOHEADER*)m_pTransformFilter->m_pVideo->CurrentMediaType().Format();
			h = inType->bmiHeader.biHeight;
			w = inType->bmiHeader.biWidth;
		}

		if (m_pTransformFilter->bForceOutputConnectSize)
		{
			w = m_pTransformFilter->iForceOutput_W;
			h = m_pTransformFilter->iForceOutput_H;
			if (h == 480)
			{
				ft = (REFERENCE_TIME) 333667;
			}
			else
			{
				ft = (REFERENCE_TIME) 400000;
			}
		}
		else
		{
			if (h == 480)
			{
				h = 486;
				ft = (REFERENCE_TIME) 333667;
			}
			else
			{
				h = 576;
				ft = (REFERENCE_TIME) 400000;
			}
		}

		//w = 720;
		//h = 486;

		CreateVideoMediaType(w, h, 16, D3DFMT_UYVY, ft, pmt);
		hr = S_OK;
	} 
	return hr;
}

HRESULT CKeystoneOutputPin::CreateVideoMediaType(int width, int height, WORD bpp, DWORD format, REFERENCE_TIME timePerFrame, CMediaType* pMediaType)
{
	//VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)pMediaType->AllocFormatBuffer(sizeof VIDEOINFOHEADER2);
	//ZeroMemory(VIH2, sizeof VIDEOINFOHEADER2);
	//
	////BITMAPINFOHEADER
	//VIH2->bmiHeader.biBitCount = bpp;
	//VIH2->bmiHeader.biClrImportant = 0;
	//VIH2->bmiHeader.biClrUsed = 0;
	//VIH2->bmiHeader.biCompression = format;
	//VIH2->bmiHeader.biHeight = height;
	//VIH2->bmiHeader.biPlanes = 1;
	//VIH2->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//VIH2->bmiHeader.biSizeImage = GetBitmapSize(&VIH2->bmiHeader);
	//VIH2->bmiHeader.biWidth = width;
	//VIH2->bmiHeader.biXPelsPerMeter = 0;
	//VIH2->bmiHeader.biYPelsPerMeter = 0;

	//VIH2->rcSource.bottom = abs(height);
	//VIH2->rcSource.right = abs(width);
	//VIH2->rcSource.left = 0;
	//VIH2->rcSource.top = 0;

	//VIH2->rcTarget.bottom = abs(height);
	//VIH2->rcTarget.right = abs(width);
	//VIH2->rcTarget.left = 0;
	//VIH2->rcTarget.top = 0;

	//VIH2->AvgTimePerFrame = timePerFrame;

	//VIH2->dwBitErrorRate = 0;
	//VIH2->dwBitRate = 0;
	//VIH2->dwControlFlags = 0;
	//VIH2->dwCopyProtectFlags = 0;
	//VIH2->dwInterlaceFlags = 0;
	//VIH2->dwPictAspectRatioX = 0;
	//VIH2->dwPictAspectRatioY = 0;

	//pMediaType->SetType(&MEDIATYPE_Video);
	//pMediaType->SetFormatType(&FORMAT_VideoInfo2);
	//pMediaType->SetTemporalCompression(FALSE);
	//pMediaType->bFixedSizeSamples = true;

	//// Work out the GUID for the subtype from the header info.
	//const GUID SubTypeGUID = GetBitmapSubtype(&VIH2->bmiHeader);
	//pMediaType->SetSubtype(&SubTypeGUID);
	//pMediaType->SetSampleSize(VIH2->bmiHeader.biSizeImage);

	//return S_OK;

	//Ianier version
	VIDEOINFO* info = (VIDEOINFO*)pMediaType->AllocFormatBuffer(sizeof VIDEOINFO);
	ZeroMemory(info, sizeof VIDEOINFO);

	info->bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
	info->bmiHeader.biWidth        = width;
	info->bmiHeader.biHeight       = height;
	info->bmiHeader.biPlanes       = 1;
	info->bmiHeader.biCompression  = format;
	info->bmiHeader.biBitCount     = bpp;
	info->bmiHeader.biSizeImage    = GetBitmapSize(&info->bmiHeader);
	info->bmiHeader.biClrImportant = 0;

	SetRectEmpty(&(info->rcSource)); // we want the whole image area rendered.
	SetRectEmpty(&(info->rcTarget)); // no particular destination rectangle

	info->AvgTimePerFrame = timePerFrame;
	if (timePerFrame > 0)
		info->dwBitRate = info->bmiHeader.biSizeImage * DWORD(UNITS / timePerFrame) * 8;

	info->rcSource.right = width;
	info->rcSource.bottom = abs(height);
	info->rcTarget.right = width;
	info->rcTarget.bottom = abs(height);

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetFormatType(&FORMAT_VideoInfo);
	pMediaType->SetTemporalCompression(FALSE);

	// Work out the GUID for the subtype from the header info.
	const GUID SubTypeGUID = GetBitmapSubtype(&info->bmiHeader);
	pMediaType->SetSubtype(&SubTypeGUID);
	pMediaType->SetSampleSize(info->bmiHeader.biSizeImage);
	return S_OK;
}

STDMETHODIMP CKeystoneOutputPin::ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt)
{
	return __super::ReceiveConnection(pConnector, pmt);
}
