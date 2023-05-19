#include "CKeystone.h"
#include <dvdmedia.h>
#include "appincludes.h"

// =================================================================
// Implements the CKeystoneOutputPin class
// =================================================================

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
STDMETHODIMP CKeystoneOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
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
HRESULT CKeystoneOutputPin::CheckConnect(IPin *pPin)
{
    // we should have an input connection first
    ASSERT(m_pTransformFilter->m_pVideo != NULL);
    if ((m_pTransformFilter->m_pVideo->IsConnected() == FALSE)) {
	    return E_UNEXPECTED;
    }
    return CBaseOutputPin::CheckConnect(pPin);
}

// provides derived filter a chance to release it's extra interfaces
HRESULT CKeystoneOutputPin::BreakConnect()
{
    //  Can't disconnect unless stopped
    ASSERT(IsStopped());
    return CBaseOutputPin::BreakConnect();
}

// Let derived class know when the output pin is connected
HRESULT CKeystoneOutputPin::CompleteConnect(IPin *pReceivePin)
{
	//Added by TRPF on 070528 - This relies on an illegitimate cheat.
	//Need to get a pointer to the CBaseRenderer in the renderer filter.
	//This is needed to get access to methods on CBaseRenderer that were needed to
	//implement the fix for that shitty Post Logic issue.
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

	//	//TENTATIVELY WORKS WITH BOTH VMR9 AND DLV
	//CheckPointer(pmtOut,E_POINTER);

 //   // Check for the subtypes we support
 //   const GUID *SubType = pmtOut->Subtype();
 //   if (SubType == NULL)
 //       return E_INVALIDARG;

	//if (*SubType != MEDIASUBTYPE_UYVY)
 //   {
 //       return E_INVALIDARG;
 //   }

	////// Get the format area of the media type
 ////   VIDEOINFO *pvi = (VIDEOINFO *) pmtOut->Format();

 ////   if(pvi == NULL)
 ////       return E_INVALIDARG;

 ////   //// Check the image size. As my default ball is 10 pixels big
 ////   //// look for at least a 20x20 image. This is an arbitary size constraint,
 ////   //// but it avoids balls that are bigger than the picture...

 ////   if((pvi->bmiHeader.biWidth < 20) || ( abs(pvi->bmiHeader.biHeight) < 20))
 ////   {
 ////       return E_INVALIDARG;
 ////   }

 ////   // Check if the image width & height have changed
 ////   if(pvi->bmiHeader.biWidth != 1920 || 
 ////      abs(pvi->bmiHeader.biHeight) != 1080)
 ////   {
 ////       // If the image width/height is changed, fail CheckMediaType() to force
 ////       // the renderer to resize the image.
 ////       return E_INVALIDARG;
 ////   }


	//// Get the format area of the media type
	//int w = 0;
	//int h = 0;
	//if (pmtOut->formattype == FORMAT_VideoInfo)
	//{
	//    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pmtOut->Format();
	//	if(pvi == NULL)
	//		return E_INVALIDARG;
	//		w = pvi->bmiHeader.biWidth;
	//		h = pvi->bmiHeader.biHeight;
	//}
	//else if (pmtOut->formattype == FORMAT_VideoInfo2)
	//{
	//    VIDEOINFOHEADER2 *pvi = (VIDEOINFOHEADER2 *) pmtOut->Format();
	//	if(pvi == NULL)
	//		return E_INVALIDARG;
	//		w = pvi->bmiHeader.biWidth;
	//		h = pvi->bmiHeader.biHeight;
	//		//return E_INVALIDARG;
	//}
	//else
	//{
	//    VIDEOINFO *pvi = (VIDEOINFO *) pmtOut->Format();
	//	if(pvi == NULL)
	//		return E_INVALIDARG;
	//		w = pvi->bmiHeader.biWidth;
	//		h = pvi->bmiHeader.biHeight;
	//}
	//
	//if((w < 20) || ( abs(h) < 20))
 //   {
 //       return E_INVALIDARG;
 //   }

 //   // Check if the image width & height have changed
	////TODO: this should use the h & w from the input connected type

	////if(w != 1920 || abs(h) != 1080)
	//if(w != m_pTransformFilter->PIVI_ConnectedWidth || abs(h) != m_pTransformFilter->PIVI_ConnectedHeight)
 //   {
 //       // If the image width/height is changed, fail CheckMediaType() to force
 //       // the renderer to resize the image.
 //       return E_INVALIDARG;
 //   }

 //   return S_OK;  // This format is acceptable.





	//WORKS WITH PHOENIX
    // must have selected input first
    ASSERT(m_pTransformFilter->m_pVideo != NULL);
    if ((m_pTransformFilter->m_pVideo->IsConnected() == FALSE)) {
	        return E_INVALIDARG;
    }

	//VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)pmtOut->pbFormat;
	//if ((VIH2->rcSource.right > 720) || (VIH2->rcSource.bottom > 576))
	//{
	//	return VFW_E_TYPE_NOT_ACCEPTED;
	//}

	//CHECK PROPOSED MEDIATYPE INFORMATION
	int w = 0;
	int h = 0;
	if (pmtOut->formattype == FORMAT_VideoInfo)
	{
	    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pmtOut->Format();
		if(pvi == NULL) return E_INVALIDARG;
		w = pvi->bmiHeader.biWidth;
		h = pvi->bmiHeader.biHeight;
	}
	else if (pmtOut->formattype == FORMAT_VideoInfo2)
	{
	    VIDEOINFOHEADER2 *pvi = (VIDEOINFOHEADER2 *) pmtOut->Format();
		if(pvi == NULL) return E_INVALIDARG;
		w = pvi->bmiHeader.biWidth;
		h = pvi->bmiHeader.biHeight;
	}
	else
	{
	    VIDEOINFO *pvi = (VIDEOINFO *) pmtOut->Format();
		if(pvi == NULL) return E_INVALIDARG;
		w = pvi->bmiHeader.biWidth;
		h = pvi->bmiHeader.biHeight;
	}
	
	//if((w < 1) || ( abs(h) < 1))
 //   {
 //       return VFW_E_TYPE_NOT_ACCEPTED;
 //   }

	//FORCE OUTPUT TYPE TO SAME FRAME SIZE AS INPUT

	//WIDTH MUST MATCH
	if (abs(w) != m_pTransformFilter->PIVI_ConnectedWidth)
	{
        return VFW_E_TYPE_NOT_ACCEPTED;
	}

	//HEIGHT MUST MATCH UNLESS WE'RE DOING 480->486
	if (m_pTransformFilter->PIVI_ConnectedHeight != 480)
	{
		if (abs(h) != m_pTransformFilter->PIVI_ConnectedHeight)
		{
			return VFW_E_TYPE_NOT_ACCEPTED;
		}
	}

    return S_OK;
}

// called after we have agreed a media type to actually set it in which case
// we run the CheckTransform function to get the output format type again
HRESULT CKeystoneOutputPin::SetMediaType(const CMediaType* pmtOut)
{
    HRESULT hr = NOERROR;
    ASSERT(m_pTransformFilter->m_pVideo != NULL);
    ASSERT(m_pTransformFilter->m_pVideo->CurrentMediaType().IsValid());

    // Set the base class media type (should always succeed)
    hr = CBasePin::SetMediaType(pmtOut);
    if (FAILED(hr)) {
        return hr;
    }

	m_pTransformFilter->HandleOutputPinConnected(&this->CurrentMediaType());


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

	return hr;
}


// ALLOCATOR

///* Save the allocator pointer in the output pin
void CKeystoneOutputPin::SetAllocator(IMemAllocator * pAllocator)
{
    pAllocator->AddRef();
    if (m_pAllocator) {
        m_pAllocator->Release();
    }
    m_pAllocator = pAllocator;
}

IMemAllocator* CKeystoneOutputPin::GetAllocator()
{
	return m_pAllocator;
}

// pass the buffer size decision through to the main transform class
HRESULT CKeystoneOutputPin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES* pProp)
{
	//////FROM TEARDOWN
	////TENTATIVELY WORKS WITH BOTH VMR9 AND DLV
	//CheckPointer(pAlloc,E_POINTER);
 //   CheckPointer(pProp,E_POINTER);

 //   //CAutoLock cAutoLock(m_pTransformFilter->pStateLock());
 //   HRESULT hr = NOERROR;

 //   //VIDEOINFO *pvi = (VIDEOINFO *) m_mt.Format();
	//int ImageSize = 0;
	//if (m_mt.formattype == FORMAT_VideoInfo)
	//{
	//    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) m_mt.Format();
	//	if(pvi == NULL)
	//		return E_INVALIDARG;
	//		ImageSize = pvi->bmiHeader.biSizeImage;
	//}
	//else if (m_mt.formattype == FORMAT_VideoInfo2)
	//{
	//    VIDEOINFOHEADER2 *pvi = (VIDEOINFOHEADER2 *) m_mt.Format();
	//	if(pvi == NULL)
	//		return E_INVALIDARG;
	//		ImageSize = pvi->bmiHeader.biSizeImage;
	//}
	//else
	//{
	//    VIDEOINFO *pvi = (VIDEOINFO *) m_mt.Format();
	//	if(pvi == NULL)
	//		return E_INVALIDARG;
	//		ImageSize = pvi->bmiHeader.biSizeImage;
	//}

 //   pProp->cBuffers = 8;
 //   //pProp->cBuffers = 30;
	//pProp->cbBuffer = ImageSize;
	////pProp->cbBuffer = 4147200;
	////pProp->cbAlign = 16;

 //   ASSERT(pProp->cbBuffer);

 //   // Ask the allocator to reserve us some sample memory, NOTE the function
 //   // can succeed (that is return NOERROR) but still not have allocated the
 //   // memory that we requested, so we must check we got whatever we wanted

 //   ALLOCATOR_PROPERTIES Actual;
 //   hr = pAlloc->SetProperties(pProp,&Actual);
 //   if(FAILED(hr))
 //   {
 //       return hr;
 //   }

 //   // Is this allocator unsuitable

 //   if(Actual.cbBuffer < pProp->cbBuffer)
 //   {
 //       return E_FAIL;
 //   }

 //   // Make sure that we have only 1 buffer (we erase the ball in the
 //   // old buffer to save having to zero a 200k+ buffer every time
 //   // we draw a frame)

 //   //ASSERT(Actual.cBuffers == 1);
 //   return NOERROR;


	//OLD - WORKS WITH PHOENIX
	pProp->cBuffers = 8;
	pProp->cbBuffer = m_pTransformFilter->PIVO_BufferSize;
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
STDMETHODIMP CKeystoneOutputPin::Notify(IBaseFilter * pSender, Quality q)
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
	////Beep(1000, 1000); //Debugging, 1s beep when quality message is received.
	//TCHAR * Type = TEXT("Famine");
	//if (q.Type == 1)
	//{
	//	Type = TEXT("Flood");
	//}
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone Quality Message. Type: %s Proportion: %d Late: %d TimeStamp: %d"), Type, q.Proportion, q.Late, q.TimeStamp));
	//m_pTransformFilter->NotifyEvent(EC_QUALITY_CHANGE_Keystone, q.Proportion, (LONG_PTR)q.Late);
	//UNREFERENCED_PARAMETER(q);
    return S_FALSE; //send it upstream
    //return S_OK;
}

HRESULT CKeystoneOutputPin::GetMediaType(int iPosition, CMediaType* pmt)
{
	CheckPointer(pmt,E_POINTER);

	if (!m_pTransformFilter->m_pVideo->IsConnected())
	{
        return VFW_S_NO_MORE_ITEMS;
	}

	if(iPosition < 0)
    {
        return E_INVALIDARG;
    }
    if(iPosition > 0)
    {
        return VFW_S_NO_MORE_ITEMS;
    }

	//HRESULT hr = VFW_S_NO_MORE_ITEMS;
	int h,w;
	REFERENCE_TIME ft;

	h = m_pTransformFilter->PIVI_ConnectedHeight;
	w = m_pTransformFilter->PIVI_ConnectedWidth;
	ft = m_pTransformFilter->FRRT_TargetFramerate_In_ATPF;

	if (h == 480)
	{
		h = 486;
		ft = (REFERENCE_TIME) 333667;
	}
	else if (h == 576)
	{
		ft = (REFERENCE_TIME) 400000;
	}
	else if ((ft > 417000) && (ft < 418000))
	{
		ft = 417083;
	}

	if ((m_pTransformFilter->REN_Renderer == 1) || (m_pTransformFilter->REN_Renderer == 2)) //DECKLINK/INTENSITY
	{
		VIDEOINFOHEADER * VIH2 = (VIDEOINFOHEADER*)pmt->AllocFormatBuffer(sizeof VIDEOINFOHEADER);
		//VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)pmt->AllocFormatBuffer(sizeof VIDEOINFOHEADER2);
		ZeroMemory(VIH2, sizeof VIDEOINFOHEADER);
		
		//BITMAPINFOHEADER
		VIH2->bmiHeader.biBitCount = 16;
		VIH2->bmiHeader.biClrImportant = 0;
		VIH2->bmiHeader.biClrUsed = 0;
		VIH2->bmiHeader.biCompression = D3DFMT_UYVY;
		VIH2->bmiHeader.biHeight = h;
		VIH2->bmiHeader.biWidth = w;
		VIH2->bmiHeader.biPlanes = 1;
		VIH2->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		VIH2->bmiHeader.biSizeImage = GetBitmapSize(&VIH2->bmiHeader);
		VIH2->bmiHeader.biXPelsPerMeter = 0;
		VIH2->bmiHeader.biYPelsPerMeter = 0;

		VIH2->rcSource.bottom = abs(h);
		VIH2->rcSource.right = abs(w);
		VIH2->rcSource.left = 0;
		VIH2->rcSource.top = 0;

		VIH2->rcTarget.bottom = abs(h);
		VIH2->rcTarget.right = abs(w);
		VIH2->rcTarget.left = 0;
		VIH2->rcTarget.top = 0;

		VIH2->AvgTimePerFrame = ft;
		if (ft > 0)
			VIH2->dwBitRate = VIH2->bmiHeader.biSizeImage * DWORD(UNITS / ft) * 8;

		VIH2->dwBitErrorRate = 0;
		VIH2->dwBitRate = 0;

		//VIH2->dwControlFlags = 0;
		//VIH2->dwCopyProtectFlags = 0;
		//VIH2->dwInterlaceFlags = 0;
		//VIH2->dwPictAspectRatioX = 0;
		//VIH2->dwPictAspectRatioY = 0;

		pmt->SetType(&MEDIATYPE_Video);
		//pmt->SetFormatType(&FORMAT_VideoInfo2);
		pmt->SetFormatType(&FORMAT_VideoInfo);
		pmt->SetTemporalCompression(FALSE);
		pmt->SetSampleSize(VIH2->bmiHeader.biSizeImage);
		pmt->bFixedSizeSamples = TRUE;

		// Work out the GUID for the subtype from the header info.
		const GUID SubTypeGUID = GetBitmapSubtype(&VIH2->bmiHeader);
		pmt->SetSubtype(&SubTypeGUID);

		return S_OK;
	}
	else if (m_pTransformFilter->REN_Renderer == 0) //VIDEO MIXING RENDERER 9
	{
		//VIDEOINFOHEADER * VIH2 = (VIDEOINFOHEADER*)pmt->AllocFormatBuffer(sizeof VIDEOINFOHEADER);
		VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)pmt->AllocFormatBuffer(sizeof VIDEOINFOHEADER2);
		ZeroMemory(VIH2, sizeof VIDEOINFOHEADER);
		
		//BITMAPINFOHEADER
		VIH2->bmiHeader.biBitCount = 16;
		VIH2->bmiHeader.biClrImportant = 0;
		VIH2->bmiHeader.biClrUsed = 0;
		VIH2->bmiHeader.biCompression = D3DFMT_UYVY;
		VIH2->bmiHeader.biHeight = h - (2*h);
		//VIH2->bmiHeader.biHeight = h;
		VIH2->bmiHeader.biWidth = w;
		VIH2->bmiHeader.biPlanes = 1;
		VIH2->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		VIH2->bmiHeader.biSizeImage = GetBitmapSize(&VIH2->bmiHeader);
		VIH2->bmiHeader.biXPelsPerMeter = 0;
		VIH2->bmiHeader.biYPelsPerMeter = 0;

		VIH2->rcSource.bottom = abs(h);
		VIH2->rcSource.right = abs(w);
		VIH2->rcSource.left = 0;
		VIH2->rcSource.top = 0;

		VIH2->rcTarget.bottom = abs(h);
		VIH2->rcTarget.right = abs(w);
		VIH2->rcTarget.left = 0;
		VIH2->rcTarget.top = 0;

		VIH2->AvgTimePerFrame = ft;
		if (ft > 0)
			VIH2->dwBitRate = VIH2->bmiHeader.biSizeImage * DWORD(UNITS / ft) * 8;

		VIH2->dwBitErrorRate = 0;
		VIH2->dwBitRate = 0;

		VIH2->dwControlFlags = 0;
		VIH2->dwCopyProtectFlags = 0;
		VIH2->dwInterlaceFlags = 0;
		VIH2->dwPictAspectRatioX = 16;
		VIH2->dwPictAspectRatioY = 9;

		pmt->SetType(&MEDIATYPE_Video);
		pmt->SetFormatType(&FORMAT_VideoInfo2);
		//pmt->SetFormatType(&FORMAT_VideoInfo);
		pmt->SetTemporalCompression(FALSE);
		pmt->SetSampleSize(VIH2->bmiHeader.biSizeImage);
		pmt->bFixedSizeSamples = TRUE;

		// Work out the GUID for the subtype from the header info.
		const GUID SubTypeGUID = GetBitmapSubtype(&VIH2->bmiHeader);
		pmt->SetSubtype(&SubTypeGUID);

		return S_OK;


		//VIDEOINFO *pvi = (VIDEOINFO *) pmt->AllocFormatBuffer(sizeof(VIDEOINFO));
		//if(NULL == pvi) return(E_OUTOFMEMORY);

		//ZeroMemory(pvi, sizeof(VIDEOINFO));
		//pvi->bmiHeader.biCompression = D3DFMT_UYVY;
		//pvi->bmiHeader.biBitCount    = 16;
		//pvi->bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
		//pvi->bmiHeader.biWidth      = w;
		//pvi->bmiHeader.biHeight     = h - (2*h);
		//pvi->bmiHeader.biPlanes     = 1;
		//pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader);
		//pvi->bmiHeader.biClrImportant = 0;

		//SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
		//SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

		//pmt->SetType(&MEDIATYPE_Video);
		//pmt->SetFormatType(&FORMAT_VideoInfo);
		//pmt->SetTemporalCompression(FALSE);

		//// Work out the GUID for the subtype from the header info.
		//const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
		//pmt->SetSubtype(&SubTypeGUID);
		//pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);

		//return NOERROR;
	}
	else
	{
        return VFW_S_NO_MORE_ITEMS;
	}


	//if (iPosition == 0) //DECKLINK VIDEO RENDERER
	//{
	//	VIDEOINFOHEADER * VIH2 = (VIDEOINFOHEADER*)pmt->AllocFormatBuffer(sizeof VIDEOINFOHEADER);
	//	//VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)pmt->AllocFormatBuffer(sizeof VIDEOINFOHEADER2);
	//	ZeroMemory(VIH2, sizeof VIDEOINFOHEADER);
	//	
	//	//BITMAPINFOHEADER
	//	VIH2->bmiHeader.biBitCount = 16;
	//	VIH2->bmiHeader.biClrImportant = 0;
	//	VIH2->bmiHeader.biClrUsed = 0;
	//	VIH2->bmiHeader.biCompression = D3DFMT_UYVY;
	//	VIH2->bmiHeader.biHeight = h;
	//	VIH2->bmiHeader.biWidth = w;
	//	VIH2->bmiHeader.biPlanes = 1;
	//	VIH2->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//	VIH2->bmiHeader.biSizeImage = GetBitmapSize(&VIH2->bmiHeader);
	//	VIH2->bmiHeader.biXPelsPerMeter = 0;
	//	VIH2->bmiHeader.biYPelsPerMeter = 0;

	//	VIH2->rcSource.bottom = abs(h);
	//	VIH2->rcSource.right = abs(w);
	//	VIH2->rcSource.left = 0;
	//	VIH2->rcSource.top = 0;

	//	VIH2->rcTarget.bottom = abs(h);
	//	VIH2->rcTarget.right = abs(w);
	//	VIH2->rcTarget.left = 0;
	//	VIH2->rcTarget.top = 0;

	//	VIH2->AvgTimePerFrame = ft;
	//	if (ft > 0)
	//		VIH2->dwBitRate = VIH2->bmiHeader.biSizeImage * DWORD(UNITS / ft) * 8;

	//	VIH2->dwBitErrorRate = 0;
	//	VIH2->dwBitRate = 0;

	//	//VIH2->dwControlFlags = 0;
	//	//VIH2->dwCopyProtectFlags = 0;
	//	//VIH2->dwInterlaceFlags = 0;
	//	//VIH2->dwPictAspectRatioX = 0;
	//	//VIH2->dwPictAspectRatioY = 0;

	//	pmt->SetType(&MEDIATYPE_Video);
	//	//pmt->SetFormatType(&FORMAT_VideoInfo2);
	//	pmt->SetFormatType(&FORMAT_VideoInfo);
	//	pmt->SetTemporalCompression(FALSE);
	//	pmt->SetSampleSize(VIH2->bmiHeader.biSizeImage);
	//	pmt->bFixedSizeSamples = TRUE;

	//	// Work out the GUID for the subtype from the header info.
	//	const GUID SubTypeGUID = GetBitmapSubtype(&VIH2->bmiHeader);
	//	pmt->SetSubtype(&SubTypeGUID);

	//	return S_OK;
	//}
	//else if (iPosition == 1) //VIDEO MIXING RENDERER 9
	//{
		//VIDEOINFO *pvi = (VIDEOINFO *) pmt->AllocFormatBuffer(sizeof(VIDEOINFO));
		//if(NULL == pvi) return(E_OUTOFMEMORY);

		//ZeroMemory(pvi, sizeof(VIDEOINFO));
		//pvi->bmiHeader.biCompression = D3DFMT_UYVY;
		//pvi->bmiHeader.biBitCount    = 16;
		//pvi->bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
		//pvi->bmiHeader.biWidth      = w;
		//pvi->bmiHeader.biHeight     = h - (2*h);
		//pvi->bmiHeader.biPlanes     = 1;
		//pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader);
		//pvi->bmiHeader.biClrImportant = 0;

		//SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
		//SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

		//pmt->SetType(&MEDIATYPE_Video);
		//pmt->SetFormatType(&FORMAT_VideoInfo);
		//pmt->SetTemporalCompression(FALSE);

		//// Work out the GUID for the subtype from the header info.
		//const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
		//pmt->SetSubtype(&SubTypeGUID);
		//pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);

		//return NOERROR;
	//}

	////OLD WAY - WORKS WITH PHOENIX
	//HRESULT hr = VFW_S_NO_MORE_ITEMS;
	//if ((iPosition == 0) && m_pTransformFilter->m_pVideo->IsConnected())
	//{
	//	int h,w;
	//	REFERENCE_TIME ft;
	//	if (m_pTransformFilter->m_pVideo->CurrentMediaType().formattype == FORMAT_VideoInfo2)
	//	{
	//		VIDEOINFOHEADER2* inType = (VIDEOINFOHEADER2*)m_pTransformFilter->m_pVideo->CurrentMediaType().Format();
	//		h = inType->bmiHeader.biHeight;
	//		w = inType->bmiHeader.biWidth;
	//		ft = inType->AvgTimePerFrame;
	//	}
	//	else
	//	{
	//		VIDEOINFOHEADER* inType = (VIDEOINFOHEADER*)m_pTransformFilter->m_pVideo->CurrentMediaType().Format();
	//		h = inType->bmiHeader.biHeight;
	//		w = inType->bmiHeader.biWidth;
	//		ft = inType->AvgTimePerFrame;
	//	}

	//	if (m_pTransformFilter->USEROPTION_ForceOutputConnectSize)
	//	{
	//		w = m_pTransformFilter->USEROPTION_ForceOutput_W;
	//		h = m_pTransformFilter->USEROPTION_ForceOutput_H;
	//		if (h == 480)
	//		{
	//			ft = (REFERENCE_TIME) 333667;
	//		}
	//		else
	//		{
	//			ft = (REFERENCE_TIME) 400000;
	//		}
	//	}
	//	else
	//	{
	//		if (h == 480)
	//		{
	//			h = 486;
	//			ft = (REFERENCE_TIME) 333667;
	//		}
	//		else if (h == 576)
	//		{
	//			ft = (REFERENCE_TIME) 400000;
	//		}
	//	}

	//	CreateVideoMediaType(w, h, 16, D3DFMT_UYVY, ft, pmt);
	//	hr = S_OK;
	//} 
	//return hr;
}

STDMETHODIMP CKeystoneOutputPin::ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt)
{
	return __super::ReceiveConnection(pConnector, pmt);
}
