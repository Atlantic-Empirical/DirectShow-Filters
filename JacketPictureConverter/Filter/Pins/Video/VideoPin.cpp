#include "../../CKeystone.h"
#include <dvdmedia.h>
#include "../../../VideoProcessing/Mixer/Mixer.h"
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
	bFrameStepping = false;
	bInVarispeed = false;
	bRestartTimeStamps = false;
	bForceReverseFieldOrder = false;
	bInJacketPictureMode = false;
	BIE = true;
	bForceBumpFieldsDown = false;
	bTDM = false;
	bInFFRW = false;
	TDMCnt = 0;
	tff_encoded = false;
	bProgressive = false;
	iDisplayFrameNo = 0;
	iEncodeFrameNo = 0;
	bNeedTop = true;
	DoFieldSplitting = 0;
	bRun32 = 1;
	FrameCnt = 0;
	dwLBColor = 0x00800080;
	DesResizeMode = 1;

	for (LPBYTE i = s_buffer; i < s_buffer + 691200; i += 4)
	{
		//BLACK
		memset(i	, 0x00, 1);
		memset(i + 1, 0x80, 1);
		memset(i + 2, 0x00, 1);
		memset(i + 3, 0x80, 1);
	}

	//Init Gabor's resizing code
	//TODO: de and reinit this each time the video size changes on input pin
	ZoomInit(720, 480);
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
				//VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)pmt->pbFormat;
				//if ((VIH2->bmiHeader.biHeight < 1000) && (VIH2->bmiHeader.biWidth < 1000))
				//{
				//	hr = S_OK;
				//}

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
// DATA TRAFFICING
//=============================================================================================

// here's the next block of data from the stream. AddRef it yourself if you need to hold it beyond 
// the end of this call.

HRESULT CKeystoneVideoPin::Receive(IMediaSample * pSample)
{
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Receive()")));
	
	//m_pTransformFilter->LogSampleMetadata(pSample);

	//Beep(1000, 1);
	m_pTransformFilter->NotifyEvent(EC_KEYSTONE_FRAMERECEIVED, 0, 0);

	//pSample->Release();
	//return S_OK;

	//DEBUGGING
	
	//FrameCnt += 1;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: VP Frame Count: %d"), FrameCnt));

	//HRESULT hr = S_OK;
 //   IMediaSample * pOut = NULL;
 //   hr = m_pTransformFilter->InitializeOutputSample(pSample, &pOut);
	//if (FAILED(hr)) return hr;
	//LPBYTE src1 = NULL;
	//pSample->GetPointer(&src1);
	//LPBYTE dst1 = NULL;
	//pOut->GetPointer(&dst1);
	//m_pTransformFilter->CopyYUY2toUYVYRect(src1, 0, 0, 1440, dst1, 0, 3, 1440, 720, 480);
	//return m_pTransformFilter->DeliverMixedSample(pOut);

	//LogMPEGFlags(pSample);


	//m_pTransformFilter->TellPhoenixToGetFrameGrab(pSample, 720, 480, 0);

	//REFERENCE_TIME pEnd = 0;
	//pSample->GetTime(&m_pTransformFilter->rtCurrentVidStart, &pEnd);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Current Vid Pres Time: %I64d"), rtCurrentVidStart));

	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;
	//pSample->GetTime(&rtS, &rtE);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: receive %I64d %I64d"), rtS, rtE));
	//Beep(1000, 1);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Video Pin - Receive")));

	////CHECK SAMPLE TIME
	//REFERENCE_TIME pStrTime_Start = 0;
	//REFERENCE_TIME pStrTime_End = 0;
	//pSample->GetTime(&pStrTime_Start, &pStrTime_End);
	//m_pTransformFilter->StreamTime(m_pTransformFilter->crtStreamTime);
	////DbgLog((LOG_TRACE,0,TEXT("Current Stream Time: %Xh  Sample Start Time: %Xh"), crtStreamTime.m_time, pStrTime_Start));
	//if (pStrTime_Start < m_pTransformFilter->crtStreamTime.m_time)
	//{
	//	////DbgLog((LOG_TRACE,0,TEXT("Stream Times - Start: %Xh  End: %Xh"), pStrTime_Start, pStrTime_End));
	//	//pStrTime_Start = crtStreamTime.m_time; //+ 100000;
	//	//pStrTime_End = pStrTime_Start + llTargetFR_Out_ATPF;
	//	//pOutSample->SetTime(&pStrTime_Start, &pStrTime_End);
	//	////DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: Sample time set to null.")));
	//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: Sample late.")));
	//}
	//else
	//{
	//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: Sample on time.")));
	//}
	////DEBUGGING

	//FLUSHING
	if(m_bFlushing) return S_OK;

	//TICK INPUT FRAME RATE
	m_pTransformFilter->TickInputRateTime(GetTickCount());

	//PAUSING
	while (m_pTransformFilter->Pausing == 1)
	{
		Sleep(1);
	}

	//STORE RECEIVED FRAME
	LPBYTE dst = NULL;
	pSample->GetPointer(&dst);
	LastFrameReceived_Ticks = GetTickCount();
	m_pTransformFilter->CopyVidRect(dst, m_pTransformFilter->LastFrameReceived, 1440, 720, m_pTransformFilter->lConnectedHeight_In);

	//MULTI-FRAME DUMP
	if (m_pTransformFilter->iDumpFrameCount > 0)
	{
		m_pTransformFilter->HandleMultiFrameGrabSample(pSample);
		m_pTransformFilter->iDumpFrameCount--;
	}

	//Sample grabber - Video Only
	if ((m_pTransformFilter->bGetSample == true) && (m_pTransformFilter->SampleWhat == 0))
	{
		m_pTransformFilter->bGetSample = false;
		m_pTransformFilter->HandleSampleGrab(pSample);
	}

	//while (bFrameStepping)
	//{
	//	//Beep(1000,1);
	//	Sleep(1);
	//	if(bDeliverFrameStepSample)
	//	{
	//		//pSample->SetTime(NULL, NULL);
	//		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Framestep sample.")));
	//		bDeliverFrameStepSample = false;

	//		LONGLONG rtStart = 0, rtStop = _I64_MAX;
	//		hr = pIn->SetTime(&rtStart, &rtStop);

	//		//Beep(100,1);
	//		return ReceiveStep2(pSample);
	//		//return S_OK;
	//	}
	//}

	DWORD LastTicks = 0;
	if (bInVarispeed == true)
	{
		LastTicks = GetTickCount();
	}
	while (bInVarispeed)
	{
		Sleep(1);
		DWORD CurrentTicks = GetTickCount();
		if (CurrentTicks - LastTicks > (DWORD)MSVarispeedInterval)
		{
			ProcessVarispeedSample(pSample);
			pSample->Release();
			return S_OK;
		}
	}

	if (bInFFRW)
	{
		//ProcessVarispeedSample(pSample);

		//Set time stamps
		REFERENCE_TIME rtS = 0;
		REFERENCE_TIME rtE = 0;
		//pSample->GetTime(&rtS, &rtE);
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: TIMES %I64d %I64d"), rtS, rtE));
		m_pTransformFilter->StreamTime(m_pTransformFilter->crtStreamTime);
		rtS = m_pTransformFilter->crtStreamTime + 250000;
		rtE = rtS + 333667;
		pSample->SetTime(&rtS, &rtE);
	}

	return ReceiveStep2(pSample);
}

HRESULT CKeystoneVideoPin::ReceiveStep2(IMediaSample * pIn)
{
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ReceiveStep2()")));

	HRESULT hr = S_OK;

	//CHECK FOR MEDIATYPE CHANGE
	bool NewMT = m_pTransformFilter->CheckForMTChange(pIn);
	if (NewMT)
	{
		//Pass new media type to renderer
		//pSample->SetMediaType((AM_MEDIA_TYPE*) &m_pTransformFilter->m_mtOut);
	}
	else
	{
		pIn->SetMediaType(NULL);
	}

	//LOCK AND BASEFILTER
    CAutoLock lck(&m_pTransformFilter->m_csReceive);
    ASSERT(pIn);

    hr = CBaseInputPin::Receive(pIn);
    if (S_OK != hr) return hr;

	////DEBUGGING
	//DWORD dwCF =  m_pTransformFilter->VIH2In->dwControlFlags;
	//int X = m_pTransformFilter->VIH2In->dwPictAspectRatioX;
	//int Y = m_pTransformFilter->VIH2In->dwPictAspectRatioY;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ResizeMode: %d  AR: %d x %d  CF: %d"), ResizeMode, X, Y, dwCF));
	////DEBUGGING

	//TODO: resize according to progressive_frame - do fields if interlaced
	//RESIZE
	LPBYTE src = NULL;
	pIn->GetPointer(&src);
	Resize(src);

    return ThreeTwo(pIn);
}

HRESULT CKeystoneVideoPin::ThreeTwo(IMediaSample * pIn)
{
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ThreeTwo()")));
	RECT r1;

	if (this->CurrentMediaType().formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2* inType = (VIDEOINFOHEADER2*)this->CurrentMediaType().Format();
		CopyRect(&r1, &inType->rcTarget);
	}
	else
	{
		VIDEOINFOHEADER* inType = (VIDEOINFOHEADER*)this->CurrentMediaType().Format();
		CopyRect(&r1, &inType->rcTarget);
	}

	//VIDEOINFOHEADER2* inType = (VIDEOINFOHEADER2*)this->CurrentMediaType().Format();


	bool top_field_first = true;
	int BumpLines = 0;

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

   	if (SampProps.dwSampleFlags & 0x04) 
	{
		//Reset interlacing on discontinutity
		bProgressive = NULL;
	}

	bool progressive_frame = (dwFlags >> 14) & 1;
	if (progressive_frame == 0)
	{
		progressive_frame = 1;
	}
	else
	{
		progressive_frame = 0;
	}
	if (bProgressive != progressive_frame)
	{
		m_pTransformFilter->NotifyEvent(EC_KEYSTONE_INTERLACING, progressive_frame, 0);	
	}
	bProgressive = progressive_frame;

	dwFlags = SampProps.dwTypeSpecificFlags;
	top_field_first = (dwFlags >> 17) & 1;

	dwFlags = SampProps.dwTypeSpecificFlags;
	BYTE picture_structure = (dwFlags & 0xC0000L) >> 18;

	dwFlags = SampProps.dwTypeSpecificFlags;
	bool repeat_first_field = (dwFlags >> 20) & 1;

	dwFlags = SampProps.dwTypeSpecificFlags;
	bool progressive_sequence = (dwFlags >> 21) & 1;

	dwFlags = SampProps.dwTypeSpecificFlags;

	REFERENCE_TIME atpf = m_pTransformFilter->llTargetFR_In_ATPF;

	//DEBUGGING
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: --")));
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ENCODE FRAME: %d, rff=%d, tff=%d, atpf=%I64d, ps=%d, pf=%d, picst=%d"), iEncodeFrameNo, repeat_first_field, top_field_first, atpf, progressive_sequence, progressive_frame, picture_structure));
	iEncodeFrameNo += 1;
	//DEBUGGING

	HRESULT hr = S_OK;

	m_pTransformFilter->NotifyEvent(EC_KEYSTONE_PROGRESSIVESEQUENCE, progressive_sequence, 0);
	m_pTransformFilter->NotifyEvent(EC_KEYSTONE_FIELDORDER, tff_encoded, 0);
	//m_pTransformFilter->NotifyEvent(EC_KEYSTONE_32, 1, 0);

   	if (SampProps.dwSampleFlags & 0x04) 
	{
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: EC_KEYSTONE_DISCONTINUITY")));
		m_pTransformFilter->NotifyEvent(EC_KEYSTONE_DISCONTINUITY, 0, 0);
		tff_encoded = top_field_first;
		bTDM = true;
		TDMCnt = 0;
		BIE = true;

	 //   CRefTime crtST;
		//rtLastEnd = crtST.m_time + 20000;
	}

	if (bNewSegmentReceived)
	{
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: New Segment")));
		tff_encoded = top_field_first;
		bNewSegmentReceived = false;
	}

	if ((r1.bottom < 500) && (bRun32 == 1))
	{
		BumpLines = 3;

		LPBYTE src = NULL;
		pIn->GetPointer(&src);

		//DEBUGGING
		
		//if (BIE)
		//{
		//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: BIE = true.")));
		//}
		//else
		//{
		//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: BIE = false.")));
		//}

		//DEBUGGING

		//// derive number_of_frame_center_offsets
		//int number_of_frame_center_offsets = 0;
		////0-2 = first field
		////3 = second field
		////This might explain the need for the 4 line bump, study it.
		//if(progressive_sequence)
		//{
		//	if(repeat_first_field)
		//	{
		//		if(top_field_first)
		//			number_of_frame_center_offsets = 3;
		//		else
		//			number_of_frame_center_offsets = 2;
		//	}
		//	else
		//	{
		//		number_of_frame_center_offsets = 1;
		//	}
		//}
		//else
		//{
		//	if(picture_structure!=3) //frame_picture
		//	{
		//		number_of_frame_center_offsets = 1;
		//	}
		//	else
		//	{
		//		if(repeat_first_field)
		//			number_of_frame_center_offsets = 3;
		//		else
		//			number_of_frame_center_offsets = 2;
		//	}
		//}

		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: NFCOs: %d"), number_of_frame_center_offsets));

		//if ((!bLastRFF) && (!repeat_first_field))
		//{
		//	BIE = true;			
		//}
		//bLastRFF = repeat_first_field;

		if (!tff_encoded)
		{
			BumpLines=4;
		}

		if (repeat_first_field)
		{
			if (BIE) //SITUATION 2
			{
				if ((tff_encoded & !top_field_first) || (!tff_encoded & top_field_first))
				{
					//Skip the extraction
					//This is from LOTR. End of a 30fps sequence. The first frame after is BFF, RFF.
					//Which is no good because the source content is TFF so we need a top and bottom field here.
					//Not a bottom, top, bottom.
					goto NextStep;
				}
				extract(top_field_first, src);
				bNeedTop = !top_field_first;
				BIE = false;
				//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: 3:2 Situation 2.")));
				goto NextStep;
			}
			else //SITUATION 4
			{
				//Verify we're not outputting two of the same fields in a row.
				if (bNeedTop != top_field_first)
				{
					extract(top_field_first, src);
					bNeedTop = !top_field_first;
					BIE = false;
					goto NextStep;
				}

				extract(top_field_first, src);
				bNeedTop = !top_field_first;
				BIE = true;
				hr = ThreeTwoIOS(pIn, &r1, BumpLines);
				if FAILED(hr) return hr;
				//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: 3:2 Situation 4.")));
				goto NextStep;
			}
		}
		else
		{
			if (BIE) //SITUATION 1
			{
				BIE = true; //just for looks
				//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: 3:2 Situation 1.")));
				goto NextStep;
			}
			else //SITUATION 3
			{
				//Verify we're not outputting two of the same fields in a row.
				//untested
				if (bNeedTop != top_field_first)
				{
					extract(top_field_first, src);
					bNeedTop = !top_field_first;
					BIE = false;
					goto NextStep;
				}

				extract(top_field_first, src);
				bNeedTop = !top_field_first;

				hr = ThreeTwoIOS(pIn, &r1, BumpLines);
				if FAILED(hr) return hr;
				extract(!top_field_first, src);
				bNeedTop = top_field_first;
				BIE = false;
				//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: 3:2 Situation 3.")));
				//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Sample skipped: %d."), iEncodeFrameNo-1));
				pIn->Release();
				return hr;
			}
		}
	}
NextStep:;
	return NonThreeTwoIOS(pIn, &r1, BumpLines);
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

HRESULT CKeystoneVideoPin::NonThreeTwoIOS(IMediaSample * pIn, RECT * r1, int CopyLine)
{
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: NonThreeTwoIOS()")));

	if (bForceBumpFieldsDown == true) CopyLine += 1;

	HRESULT hr = S_OK;
    IMediaSample * pOut = NULL;
    hr = m_pTransformFilter->InitializeOutputSample(pIn, &pOut);
    if (FAILED(hr)) return hr;

	LPBYTE dst = NULL;
	pOut->GetPointer(&dst);

	LPBYTE src = NULL;
	pIn->GetPointer(&src);

	if (r1->bottom < 500)
	{
		//Set first 4 and last six lines black
		for (LPBYTE i = dst; i < dst + 5760; i += 4)
		{
			//UYVY BLACK
			memset(i	, 0x80, 1);
			memset(i + 1, 0x00, 1);
			memset(i + 2, 0x80, 1);
			memset(i + 3, 0x00, 1);
		}
		for (LPBYTE i = dst + 691200; i < dst + 699840; i += 4)
		{
			memset(i	, 0x80, 1);
			memset(i + 1, 0x00, 1);
			memset(i + 2, 0x80, 1);
			memset(i + 3, 0x00, 1);
		}
	}

	//CSC
	//m_pTransformFilter->SaveBufferToFile("C:\\Temp\\src.bin", "w", src, 720*480*2, 720, 480);
	m_pTransformFilter->CopyYUY2toUYVYRect(src, 0, 0, 1440, dst, 0, CopyLine, 1440, 720, r1->bottom);
	//m_pTransformFilter->SaveBufferToFile("C:\\Temp\\dst.bin", "w", dst, 720*486*2, 720, 486);

	return ReceiveStep5(pOut, r1, false, sbBottomFieldSource, sbTopFieldSource);
}

HRESULT CKeystoneVideoPin::ThreeTwoIOS(IMediaSample * pIn, RECT * r1, int CopyLine)
{
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ThreeTwoIOS()")));

	if (bForceBumpFieldsDown == true) CopyLine += 1;

	IMediaSample * pTT = NULL;
    HRESULT hr = m_pTransformFilter->InitializeOutputSample(pIn, &pTT);
    if (FAILED(hr)) return hr;

	LPBYTE dst = NULL;
	pTT->GetPointer(&dst);

	//Set first 4 and last six lines black
	for (LPBYTE i = dst; i < dst + 4320; i += 4)
	{
		memset(i	, 0x80, 1);
		memset(i + 1, 0x00, 1);
		memset(i + 2, 0x80, 1);
		memset(i + 3, 0x00, 1);
	}
	for (LPBYTE i = dst + 691200; i < dst + 699840; i += 4)
	{
		memset(i	, 0x80, 1);
		memset(i + 1, 0x00, 1);
		memset(i + 2, 0x80, 1);
		memset(i + 3, 0x00, 1);
	}

	//CSC
	//m_pTransformFilter->SaveBufferToFile("C:\\Temp\\src.bin", "w", src, 720*480*2, 720, 480);
	m_pTransformFilter->CopyYUY2toUYVYRect(s_buffer, 0, 0, 1440, dst, 0, CopyLine, 1440, 720, 480);
	//m_pTransformFilter->SaveBufferToFile("C:\\Temp\\dst.bin", "w", dst, 720*486*2, 720, 486);

	return ReceiveStep5(pTT, r1, true, sbBottomFieldSource, sbTopFieldSource);
}

HRESULT CKeystoneVideoPin::ReceiveStep5(IMediaSample * pOut, RECT * r1, bool Is32Sample, int BottomSource, int TopSource)
{
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ReceiveStep5()")));
	int srcpitch = r1->right * 2;

	VIDEOINFOHEADER* outType = (VIDEOINFOHEADER*)m_pTransformFilter->m_pOutput->CurrentMediaType().Format();
	int dstpitch = outType->bmiHeader.biWidth * 2;
	IntersectRect(r1, &outType->rcTarget, r1);

	LPBYTE dst = NULL;
	pOut->GetPointer(&dst);

	//DEBUGGING
	//BumpFieldsDown(dst, 480, 720, 1440, 1, false);
	//DEBUGGING

	//PROCAMP
	if (m_pTransformFilter->bDoProcAmp)
	{
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ProcAmp")));
		m_pTransformFilter->ApplyBrContHueSat_UYVY(dst, r1->right, r1->bottom, r1->right * 2, m_pTransformFilter->bProcAmpHalfFrame);
		//Later change the format argument so it is set dynamically to the correct format, for now we know that UYVY is ok.
		//CProcAmp::ProcAmp(dst, r1.right, r1.bottom, IF_UYVY, iBrightness, iContrast, iSaturation, iHue, iGamma, iChroma, iLuma, iTint, iGain, iSharpness);
	}

	//COLOR FILTER
	if (m_pTransformFilter->bDoColorFiltering)
	{
		m_pTransformFilter->ApplyColorFilters_UYVY(dst, r1->right, r1->bottom, r1->right * 2, m_pTransformFilter->iWhichColorFilter);
	}

	//DEBUGGING
	// TopField: %d, BottomField: %d" , TopSource, BottomSource
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: display frame= %d, 32Samp= %d"), iDisplayFrameNo-1, Is32Sample));
	iDisplayFrameNo += 1;
	//DEBUGGING

	VIDEOINFOHEADER2 * v2 = (VIDEOINFOHEADER2*)this->CurrentMediaType().Format();
	if (v2->bmiHeader.biCompression == 1498831189) goto SkipTimeStamping;


	////===========================================================================================
	//// NEW TIME STAMPING
	////===========================================================================================

	////1) Determine target duration
	//LONGLONG ATPF = m_pTransformFilter->llTargetFR_Out_ATPF;

	////2) Check last end too see if it's non-sensical in comparison with the current stream time. 
	////If yes, adjust lastEnd to a more sensible value.
 //   CRefTime crtST;
	//m_pTransformFilter->StreamTime(crtST);
	////int LE_CST = crtST.m_time - rtLastEnd;
	////LE_CST = abs(LE_CST);
	////DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: LE_CST=%I32d"), LE_CST));
	////if (LE_CST > 10000000) //10m = 1s of tolerance
	////{
	////	//The difference between the current stream time and the last end has grown too large
	////	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: sample time adjusted.")));
	////	rtLastEnd = crtST.m_time + 30000; //this value can be adjusted to acheieve desired result
	////}

	//LONGLONG LE_CST = crtST.m_time - rtLastEnd;

	////IF THE CST AND THE LE ARE TOO FAR APART WE NEED TO:
	////	Restart the stamping to a time in the future.

	//if ((LE_CST > 10000000) || (LE_CST < -10000000))
	//{
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: The difference between the current stream time and the last end has grown too large")));
	//	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: LE_CST %I32d"), LE_CST));
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Sample times restarted.")));
	//	rtLastEnd = crtST.m_time + 5000000; // 1/2 a second
	//	//rtLastEnd = crtST.m_time + 6000;
	//}

	////3) Match up the last end with the current start and set duration of sample
	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;
	//rtS = rtLastEnd;
	//rtE = rtS + ATPF;

	////3) Set, Store current time stamps as last time stamps
	//pOut->SetTime(&rtS, &rtE);
	//rtLastStart = rtS;
	//rtLastEnd = rtE;

	////===========================================================================================
	//// END NEW TIME STAMPING
	////===========================================================================================


	//===========================================================================================
	// TIME STAMPING
	//===========================================================================================

	//SET TIME STAMPS DURING 3:2 AND WHEN THERE IS A LARGE GAP 
	REFERENCE_TIME rtS = 0;
	REFERENCE_TIME rtE = 0;
	pOut->GetTime(&rtS, &rtE);
	//int Dur = rtE - rtS;
	LONGLONG ATPF = m_pTransformFilter->llTargetFR_Out_ATPF;
	//if (Dur == 0) Dur = m_pTransformFilter->llTargetFR_Out_ATPF;
	//if (Dur > 410000) Dur = 333667;
	//if ((Dur == 400000) && (m_pTransformFilter->lConnectedHeight_In == 480)) Dur = 333667; //For null video source

    CRefTime crtST;
	m_pTransformFilter->StreamTime(crtST);

	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: STREAM TIME=%I64d"), crtST.m_time));
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: CUR START=%I64d"), rtS));
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: CUR END=%I64d"), rtE));
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: LAST START=%I64d"), rtLastStart));
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: LAST END=%I64d"), rtLastEnd));

	int LE_CST = crtST.m_time - rtLastEnd;
	if (LE_CST > 10000000) //|| (LE_CST < 10000000))
	{
		//The difference between the current stream time and the last end has grown too large
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: sample time adjusted.")));
		rtLastEnd = crtST.m_time + 6000;
	}
	else if (!Is32Sample)
	{
		REFERENCE_TIME rtDelta = rtE - rtLastEnd;
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: rec-del delta: %I64d"), rtDelta));
		if (rtDelta > 255000)
		{
			pOut->GetTime(&rtLastStart, &rtLastEnd);
			////rtLastEnd = rtS;  //Shu-Ping's idea

			//// CHECK FOR A BIG GAP BETWEEN THE START TIME AND THE CURRENT STREAM TIME
			//int CST_CS = crtST.m_time - rtLastStart;

			////IF THE CST AND THE current start ARE TOO FAR APART WE NEED TO:
			////	Restart the stamping to a time in the future.
			//if ((CST_CS > 10000000) || (CST_CS < -10000000))
			//{
			//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: CST CS too far apart(%I32d). Resetting.  ======================================================================="), CST_CS));
			//	//rtLastEnd = crtST.m_time + 6000; //+ ATPF;
			//}

			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: sample time restarted.")));
		}
	}

	//2) match up the previous end with the current start
	rtS = rtLastEnd;
	rtE = rtS + ATPF;

	//3) if source is 24fps (417083) set proper duration on sample
	if (ATPF > 410000)
	{
		rtE = rtS + 333667;
	}

	//////FINALLY, CHECK FOR A BIG GAP BETWEEN THE START TIME AND THE CURRENT STREAM TIME
	//int CST_CS = crtST.m_time - rtS;

	////IF THE CST AND THE current start ARE TOO FAR APART WE NEED TO:
	////	Restart the stamping to a time in the future.
	//if ((CST_CS > 10000000) || (CST_CS < -10000000))
	//{
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: CST CS too far apart. Resetting.  =======================================================================")));
	//	rtS = crtST.m_time; //+ ATPF;
	//	rtE = rtS +  ATPF;
	//}

	pOut->SetTime(&rtS, &rtE); //REENABLE ME!!!
	rtLastStart = rtS;
	rtLastEnd = rtE;
	// DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: SAVED LAST END=%I64d"), rtLastEnd));

	//===========================================================================================
	// END TIME STAMPING
	//===========================================================================================

SkipTimeStamping:
	LPBYTE src = NULL;
	pOut->GetPointer(&src);

	////DEBUGGING
	//if (r1->bottom < 500)
	//{
	//	BumpFieldsDown(src, r1->bottom, r1->right, r1->right * 2, 3, true); 

	//	//Last n Lines
	//	for (LPBYTE i = src + 691200; i < src + 699840; i += 4)
	//	{
	//		//BLACK
	//		memset(i	, 0x80, 1);	//U: Blue - Luma
	//		memset(i + 1, 0x00, 1);	//Y: Luma
	//		memset(i + 2, 0x80, 1);	//V: Red - Luma
	//		memset(i + 3, 0x00, 1);	//Y: Luma

	//		////MAGENTA
	//		//memset(i	, 0xC0, 1);	//U: Blue - Luma
	//		//memset(i + 1, 0x00, 1);	//Y: Luma
	//		//memset(i + 2, 0xC0, 1);	//V: Red - Luma
	//		//memset(i + 3, 0x00, 1);	//Y: Luma
	//	}
	//}

	//FIELD SPLITTING
	if (DoFieldSplitting == 1)
	{
		LPBYTE tmp = (LPBYTE)malloc(r1->right * r1->bottom * 2);

		int Pitch = 1440;
		LPBYTE d1 = tmp;
		int HalfFrameBytes = (r1->bottom/2) * Pitch;
		LPBYTE d2 = tmp + HalfFrameBytes;
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: D2: %d"), (d2 - d1)/1440));
		LPBYTE s = src;
		for (int i = 0; i < r1->bottom; i+=2)
		{
			memcpy(d1, s, 1440);
			s+=Pitch;
			memcpy(d2, s, 1440);
			s+=Pitch;
			d1 += Pitch;
			d2 += Pitch;
		}
		m_pTransformFilter->CopyVidRect(tmp, src, 1440, r1->right, r1->bottom);
		free(tmp);
	}

	//GABOR'S FRAME STEPPING
	while (!m_bFlushing && bFrameStepping)
	{
		Sleep(1);
		if(bDeliverFrameStepSample)
		{
			//pSample->SetTime(NULL, NULL);
			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Framestep sample.")));
			bDeliverFrameStepSample = false;
			LONGLONG rtStart = 0, rtStop = _I64_MAX;
			pOut->SetTime(&rtStart, &rtStop);
			//Beep(100,1);
			return m_pTransformFilter->MixAndDeliverVideo(pOut, r1->right, r1->bottom);
			//m_pTransformFilter->DeliverMixedSample(pOut);
			//m_bSampleSkipped = FALSE;	// last thing no longer dropped
			//pOut->Release();
			//return S_OK;
		}
	}
	
	return m_pTransformFilter->MixAndDeliverVideo(pOut, r1->right, r1->bottom);
}

//HRESULT CKeystoneVideoPin::Resize(IMediaSample * pIn)
HRESULT CKeystoneVideoPin::Resize(LPBYTE src)
{
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Resize: %d"), ResizeMode));

	//ResizeMode = 1; //do nothing

	if (ResizeMode == 2)
	{
		//Size 16x9 content to panscan
		//LPBYTE src = NULL;
		//pIn->GetPointer(&src);
		ZoomYUY2(src, m_pTransformFilter->VIH2In->bmiHeader.biWidth, m_pTransformFilter->VIH2In->bmiHeader.biHeight, m_pTransformFilter->VIH2In->rcTarget);
	}
	else if (ResizeMode == 3)
	{
		//size 16x9 content to letterbox
		//LPBYTE src = NULL;
		//pIn->GetPointer(&src);

		int h = m_pTransformFilter->VIH2In->bmiHeader.biHeight;
		int w = m_pTransformFilter->VIH2In->bmiHeader.biWidth;
		int pitch = 1440;

		//prep
		LPBYTE tmp = (LPBYTE)malloc(h * pitch);
		m_pTransformFilter->CopyVidRect(src, tmp, pitch, w, h);

		//set bg color for desired lb color
		char* from = (char*)&dwLBColor;
		char dest[] = "0000";
		swab(from, dest, 4);

		LPBYTE tOrig = src;
		for(int j = 0; j < h; j++, tOrig += pitch) 
		{ 
			WORD* s = (WORD*) tOrig; 
			for(int i = 0; i < w; i += 2, s += 2) 
			{
				s[0] = (WORD) ((WORD)(dest[3]<<8) | (BYTE)(dest[2])); 
				s[1] = (WORD) ((WORD)(dest[1]<<8) | (BYTE)(dest[0]));
			} 
		}

		//set write address to appropriate starting position after the top LB
		BYTE adj = 0;
		if (h==576) //PAL
		{
			adj = 72;
		}
		else //NTSC
		{
			adj = 60;
		}
		LPBYTE TmpWrite = src + (adj * pitch);
		LPBYTE TmpRead = tmp;

		ShrinkV(TmpWrite, h-(adj*2), TmpRead, h, w, true);
		free(tmp);
	}
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



