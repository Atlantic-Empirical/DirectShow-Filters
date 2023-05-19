#include "appincludes.h"
#include "Mixer.h"

HRESULT CKeystone::Pipeline_Core_Entrance(IMediaSample* pOut, int w, int h)
{
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: MixAndDeliverVideo")));

    if (LOCK_FilterIsLocked == true) return E_FAIL;
	HRESULT hr = S_OK;

	if (h == 480) h = 486;

	LPBYTE dst = NULL;
	pOut->GetPointer(&dst);

	// MIX SUBPICTURE
	hr = Pipeline_Subpicture_MixSubpicture(dst, w, h);
	if (FAILED(hr)) return hr;

	//Sample grabber - Video And Subpicture
	if ((SAGR_GetSample == true) && (USEROPTION_SampleWhat == 4))
	{
		SAGR_GetSample = false;
		SAGR_HandleSampleGrab(pOut);
	}

	// MIX OSD
	hr = OSD_MixOnScreenDisplay(dst, w, h);
	if (FAILED(hr)) return hr;

	// MIX LINE21
	hr = Pipeline_Line21_MixLine21(dst, w, h);
	if (FAILED(hr)) return hr;

	//MIX GUIDES
	hr = GIDE_MixGuides(dst, PIVI_ConnectedWidth, PIVI_ConnectedHeight, false);
	if (FAILED(hr)) return hr;

	// MIX GOP TIMECODE
	hr = GPTC_MixGOPTimecode(dst, pOut, w, h);
	if (FAILED(hr)) return hr;

	// TRIAL MODE
	if (LOCK_TRIALMODE == true)
	{
		hr = TRWA_MixTrialWarning(dst, pOut, w, h);
		if (FAILED(hr)) return hr;
		//if (LOCK_TRIALOVERRIDE == false)
		//{
		//}
	}

	pOut->SetActualDataLength(w * h * 2);
	pOut->SetSyncPoint(TRUE);
	pOut->SetDiscontinuity(FALSE);

	VIDEOINFOHEADER2 * v2 = (VIDEOINFOHEADER2*)m_pVideo->CurrentMediaType().Format();
	if (v2->bmiHeader.biCompression == 1498831189) goto gtDeliverMixedSample; //If we're connected to NullVideoSource

	////NEEDED FOR SYSTEM JACKET PICTURE ONLY
	//DWORD dwCurrentTicks = GetTickCount();
	//int iTDiff = abs(short(TIST_LastSampleProcessedAt - dwCurrentTicks));
	////DbgLog((LOG_TRACE, 0, TEXT("Keystone: Gap in samples: %d"), iTDiff));
	//if (iTDiff > 82)  //250= 1/4sec - this has to be short enough to catch quick button changes by user.
	//{ 
	//	//Beep(1000, 1);
	//	REFERENCE_TIME rtStart, rtEnd;
	//	CRefTime crtStreamTime;
	//	StreamTime(crtStreamTime);
	//	rtStart = crtStreamTime.m_time + 100000;
	//	rtEnd = rtStart + FRRT_TargetFramerate_Out_ATPF;
	//	NotifyEvent(EC_KEYSTONE_SETPRESENTATIONTIME, 3, 0);
	//	//pOut->SetTime(&rtStart, &rtEnd);
	//	pOut->SetTime(NULL, NULL);
	//	DbgLog((LOG_TRACE, 0, TEXT("Keystone: LARGE gap in samples detected. %d"), iTDiff));
	//}
	//TIST_LastSampleProcessedAt = GetTickCount();

gtDeliverMixedSample:
	return Pipeline_Core_DeliverMixedSample(pOut);
}

HRESULT CKeystone::Pipeline_Core_DeliverMixedSample(IMediaSample * pOutSample)
{
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Pipeline_Core_DeliverMixedSample")));

	HRESULT hr = S_OK;

	//IMAGE RECORD
	if (SAGR_RecordingImages == true)
	{
		LPBYTE dst = NULL;
		pOutSample->GetPointer(&dst);

		CRefTime crtST;
		StreamTime(crtST);

		TCHAR buff[256];
		//_stprintf(buff, _T("C:\\ImageDump\\%I64d.bin"), crtST.Millisecs());
	
		USES_CONVERSION;
		_stprintf(buff, _T("%s%d.bin"), W2A(SAGR_ImageRecordTargPath), crtST.Millisecs());

		LONG TargSize = PIVI_ConnectedHeight * PIVI_ConnectedWidth * 3;
		LPBYTE pRGB = (LPBYTE)malloc(TargSize);
		CSCManual::UYVYToRGB24(dst, PIVI_ConnectedWidth, PIVI_ConnectedHeight, pRGB);
		BUSA_SaveBufferToFile_A(buff, "wb", pRGB, TargSize);
		free(pRGB);
		Beep(1000,1);
		//const char * FileName, const char * Mode, LPBYTE buf, int bufsize, int w, int h
		//m_pTransformFilter->SaveBufferToFile(m_pTransformFilter->ImageRecordTargPath, "w", src, 4147200, 1920, 1080);
	}

	FRRT_TickOutputRate(GetTickCount());

	MSR_STOP(m_idTransform);
	
	if ((YVRD_DoYUVStillRender == true) && ((int)YVRD_YUVBuffer != 0xcdcdcdcd))
	{
		LPBYTE dst = NULL;
		LPBYTE src = YVRD_YUVBuffer;
		pOutSample->GetPointer(&dst);
		for (int i = 0; i < PIVI_ConnectedHeight; i++)
		{
			memcpy(dst, src, PIVI_Pitch);
			src += PIVI_Pitch;
			dst += PIVI_Pitch;
		}
	}

	//FG OF LAST FRAME OUTPUT
	if (OPTM_OptimizedLevel < 1)
	{
		LPBYTE dst = NULL;
		pOutSample->GetPointer(&dst);
		CopyBuffer_Rect(dst, SAGR_LastBufferOutput, PIVO_Pitch, PIVO_ConnectedWidth, PIVO_ConnectedHeight);
	}

	//////Set NULL timestamps if the user wants
	////if (USEROPTION_SetNULLTimestamps == true)
	////{
	////	NotifyEvent(EC_KEYSTONE_SETPRESENTATIONTIME, 5, 0);
	//	pOutSample->SetTime(NULL, NULL);
	////}
	
	//CLONE THE SAMPLE FOR RESEND
	SPRS_CloneMediaSample(pOutSample, SPRS_LastSampleSent);

	// LOGGING
	REFERENCE_TIME rtS = 0;
	REFERENCE_TIME rtE = 0;
	pOutSample->GetTime(&rtS, &rtE);
	NotifyEvent(EC_KEYSTONE_PRESENTATIONTIMES, rtS, rtE);
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | rtS/rtE @Send: %I64d %I64d"), rtS, rtE));
	// LOGGING
	//Beep(1000,1);
 	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | SEND")));
	LOCK_FrameCount --;
	hr = m_pOutput->m_pInputPin->Receive(pOutSample);
	m_bSampleSkipped = FALSE;	// last thing no longer dropped
    pOutSample->Release();
	NotifyEvent(EC_KEYSTONE_DELIVER, 0, 0);

	//NotifyEvent(EC_KEYSTONE_FRAMEDELIVERED, GetTickCount(), 0);
	

	//DROP FRAME DETECTION & TIME STAMP CHECKS FROM KEYHD
	//Implemented with the advice of James Clough from BMD

	CRefTime crtST;
	StreamTime(crtST);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE HD: Sample Delivered. Start/End= %I64d/%I64d"), rtS, rtE));
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | StreamTime= %I64d"), crtST.m_time));

	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;
	pOutSample->GetTime(&rtS, &rtE);
	StreamTime(crtST);
	
	//DEBUGGING
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone_Omni: m_tStart= %I64d"), m_tStart));
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone_Omni: GraphTime= %I64d"), ?));
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone_Omni: StreamTime= %I64d"), crtST.m_time));
	//DEBUGGING

	//Difference between current stream time and the sample's start time.
	REFERENCE_TIME StartTime_StreamTime_Delta = rtS - crtST.m_time;
	if (StartTime_StreamTime_Delta < 0) //Should be a positive delta. This means we have time to burn.
	{
		//We're late
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | WE ARE LATE - StartDelta= %I64d"), StartTime_StreamTime_Delta));
		NotifyEvent(EC_KEYSTONE_FRAMEDROPPED, StartTime_StreamTime_Delta, 0);
	}

	//Sample duration should match negotiated out atpf
	REFERENCE_TIME SampleDuration = rtE - rtS;
	if (SampleDuration != this->PIVO_ATPF)
	{
		//Sample duration mismatch detected.
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | INVALID SAMPLE DURATION - Duration= %I64d"), SampleDuration));
		NotifyEvent(EC_KEYSTONE_WRONGDURATION, SampleDuration, 0);
	}

	//Samples should be butt-to-butt
	if ((rtS != rtLastEnd) && (rtLastEnd != 0))
	{
		//Discontinuity has occurrred.
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | SAMPLES NOT ADJACENT - LastEnd/NewStart= %I64d/%I64d"), rtLastEnd, rtS));
		NotifyEvent(EC_KEYSTONE_SAMPLESNOTADJACENT, rtLastEnd, rtS);
	}

	//Finally, save these times.
	rtLastStart = rtS;
	rtLastEnd = rtE;
	//END OF FRAMEDROP DETECTION

	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | END OF PIPELINE")));
	DbgLog((LOG_TRACE, 0, TEXT("=================================================================================")));
	DbgLog((LOG_TRACE, 0, TEXT("")));

	return hr;
}

HRESULT CKeystone::CheckForPresentationTimeStreamTimeDiscrepency(IMediaSample * pOutSample)
{
	REFERENCE_TIME rtS = 0;
	REFERENCE_TIME rtE = 0;
	pOutSample->GetTime(&rtS, &rtE);

	int Dur = rtE - rtS;

	CRefTime crtST;
	StreamTime(crtST);
	int LE_CST = crtST.m_time - TMNG_LastSampleEndTime;
	if (abs(LE_CST) > 10000000)
	{
		//The difference between the current stream time and the last end has grown too large
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: sample time adjusted.")));
		NotifyEvent(EC_KEYSTONE_SETPRESENTATIONTIME, 327, 0);
		TMNG_LastSampleEndTime = crtST.m_time + 6000;
	}
	//else if (!Is32Sample)
	//{
	//	REFERENCE_TIME rtDelta = rtE - rtLastEnd;
	//	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: rec-del delta: %I64d"), rtDelta));
	//	if (rtDelta > 255000)
	//	{
	//		pOutSample->GetTime(&rtLastStart, &rtLastEnd);
	//		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: sample time restarted.")));
	//	}	
	//}
	else if (Dur == 333665)
	{
		//to deal with MC VC-1 using this kooky atpf
		Dur += 1;
	}

	//2) match up the previous end with the current start
	rtS = TMNG_LastSampleEndTime;
	rtE = rtS + Dur;

	////3) if source is 24fps (417083) set proper duration on sample
	//if (Dur > 410000)
	//{
	//	rtE = rtS + 333667;
	//}

	//DbgLog((LOG_TRACE, 0, TEXT("KEY HD: rtS= %I64d  rtE=%I64d"), rtS, rtE));
	pOutSample->SetTime(&rtS, &rtE);
	NotifyEvent(EC_KEYSTONE_SETPRESENTATIONTIME, 723, 0);
	TMNG_LastSampleStartTime = rtS;
	TMNG_LastSampleEndTime = rtE;

	return S_OK;
}

HRESULT CKeystone::Pipeline_Core_ReSizeMixSendSample(int Unconditional, bool DoYUVCSC)
{
	DbgLog((LOG_TRACE, 0, TEXT("Keystone: Pipeline_Core_ReSizeMixSendSample()")));

	//Beep(1000,1);
	if (m_pOutput->m_pAllocator == NULL)
	{
		//Beep(1000, 1);
		return S_FALSE;
	}
	if ((m_State == State_Paused) || (m_State == State_Stopped))
	{
		//Beep(1000, 1);
		return S_FALSE;
	}
	if (((int)SAGR_LastBufferReceived == 0xcdcdcdcd) || ((int)SAGR_LastBufferReceived == 0))
	{
		//Beep(1000, 1);
		return S_FALSE;
	}

	HRESULT hr;
	
	if (REN_Renderer == 2)
	{
		BOOL IsStreaming = m_pOutput->m_pRenderer->IsStreaming();
		BOOL IsEOS = m_pOutput->m_pRenderer->IsEndOfStream();
		BOOL IsReady = m_pOutput->m_pRenderer->CheckReady();
		BOOL HasSample;
		
		HasSample = m_pOutput->m_pRenderer->HaveCurrentSample();

		if ((!IsStreaming) || (IsEOS) || (!IsReady) || (HasSample))
		{
			//Beep(1000,1);
			return S_FALSE;
		}
	}
	//else
	//{
	//	HasSample = FALSE;
	//}

	//CComPtr<IMediaSample> pSample;
	IMediaSample * pSample = NULL;
    hr = InitializeOutputSample(NULL, &pSample);
    if (FAILED(hr)) return hr;

	LPBYTE dst = NULL;
	hr = pSample->GetPointer(&dst);

	//DIMENSIONS
	int w = 720;
	int h = PIVI_ConnectedHeight;

	//RESIZE
	CopyBuffer_Rect(SAGR_LastBufferReceived, SAGR_LastBufferReceived_Scaled, PIVI_Pitch, w, h);
	SCLE_Resize(SAGR_LastBufferReceived_Scaled);

	int BumpLines = 0;

	//COPY LAST BUFFER RECEIVED TO OUTPUT BUFFER
	if (h == 480)
	{
		//NTSC - SET FIRST THREE AND LAST THREE LINES BLACK
		for (LPBYTE i = dst; i < dst + 4320; i += 4)
		{
			//UYVY BLACK
			memset(i	, 0x80, 1);
			memset(i + 1, 0x00, 1);
			memset(i + 2, 0x80, 1);
			memset(i + 3, 0x00, 1);
		}
		for (LPBYTE i = dst + 691200 + 4320; i < dst + 699840; i += 4)
		{
			memset(i	, 0x80, 1);
			memset(i + 1, 0x00, 1);
			memset(i + 2, 0x80, 1);
			memset(i + 3, 0x00, 1);
		}

		BumpLines = 3;
		if (!TCNE_tff_encoded)
		{
			BumpLines=4;
		}

		if (DoYUVCSC == true)
		{
			CopyBuffer_Rect_YUY2toUYVY(SAGR_LastBufferReceived_Scaled, 0, 0, PIVI_Pitch, dst, 0, BumpLines, PIVI_Pitch, w, h);
		}
		else
		{
			CopyBuffer_Rect(SAGR_LastBufferReceived_Scaled, dst, PIVI_Pitch, w, h);
		}
	}
	else
	{
		if (DoYUVCSC == true)
		{
			CopyBuffer_Rect_YUY2toUYVY(SAGR_LastBufferReceived_Scaled, 0, 0, PIVI_Pitch, dst, 0, 0, PIVI_Pitch, w, h);
		}
		else
		{
			CopyBuffer_Rect(SAGR_LastBufferReceived_Scaled, dst, PIVI_Pitch, w, h);
		}
	}

	//NTSC - ADJUST TO OUTPUT SIZE
	if (h == 480) h = 486;
 
	//MIX GUIDES
	hr = GIDE_MixGuides(dst, PIVI_ConnectedWidth, PIVI_ConnectedHeight, false);
	if (FAILED(hr)) return hr;

	// MIX SUBPICTURE
	hr = Pipeline_Subpicture_MixSubpicture(dst, w, h);
	if (FAILED(hr)) return hr;

	// TRIAL MODE
	if (LOCK_TRIALMODE == true)
	{
		hr = TRWA_MixTrialWarning(dst, pSample, w, h);
		if (FAILED(hr)) return hr;
		//if (LOCK_TRIALOVERRIDE == false)
		//{
		//}
	}


	//DEBUGGING
	//BurnInTicks(dst, w, h);
	//DEBUGGING

	pSample->SetTime(NULL, NULL);

	//CONFIRM THAT A FRAME HAS NOT BEEN RECEIVED SINCE WE DETECTED A DISCONTINUITY
	DWORD TicksDelta = GetTickCount() - TMNG_LastFrameReceived_Ticks;
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: DELTA %I32d"), TicksDelta));
	if ((TicksDelta < 500) && (Unconditional == 0))
	{
		//Beep(1000,1);
		DbgLog((LOG_TRACE, 0, TEXT("Keystone: Bailing out of ReSizeMixSendSample()")));
		pSample->Release();
		return S_OK; //Bail out, a new frame arrived	
	}

	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: ResendLastSample Receive()")));
	LOCK_FrameCount --;
	hr = m_pOutput->m_pInputPin->Receive(pSample);
	pSample->Release();
	return hr; 
}

HRESULT CKeystone::InitializeOutputSample(IMediaSample *pSample, IMediaSample **ppOutSample)
{
    IMediaSample *pOutSample;

    AM_SAMPLE2_PROPERTIES * const pProps = m_pVideo->SampleProps();
    DWORD dwFlags = m_bSampleSkipped ? AM_GBF_PREVFRAMESKIPPED : 0;

    // This will prevent the image renderer from switching us to DirectDraw
    // when we can't do it without skipping frames because we're not on a
    // keyframe.  If it really has to switch us, it still will, but then we
    // will have to wait for the next keyframe
    if (!(pProps->dwSampleFlags & AM_SAMPLE_SPLICEPOINT)) 
	{
		dwFlags |= AM_GBF_NOTASYNCPOINT;
    }

	////DEBUGING
	//BOOL IsStreaming = m_pOutput->m_pRenderer->IsStreaming();
	//if (IsStreaming == TRUE)
	//{
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | IsStreaming: TRUE")));
	//}
	//else
	//{
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | IsStreaming: FALSE")));
	//}
	//BOOL IsEOS = m_pOutput->m_pRenderer->IsEndOfStream();
	//if (IsEOS == TRUE)
	//{
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | IsEOS: TRUE")));
	//}
	//else
	//{
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | IsEOS: FALSE")));
	//}
	//BOOL IsReady = m_pOutput->m_pRenderer->CheckReady();
	//if (IsReady == TRUE)
	//{
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | IsReady: TRUE")));
	//}
	//else
	//{
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | IsReady: FALSE")));
	//}
	//BOOL HasSample = m_pOutput->m_pRenderer->HaveCurrentSample();
	//if (HasSample == TRUE)
	//{
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | HasSample: TRUE")));
	//}
	//else
	//{
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | HasSample: FALSE")));
	//}
	////DEBUGGING

    ASSERT(m_pOutput->m_pAllocator != NULL);
    HRESULT hr = m_pOutput->m_pAllocator->GetBuffer(
             &pOutSample, 
			 pProps->dwSampleFlags & AM_SAMPLE_TIMEVALID ? &pProps->tStart : NULL, 
			 pProps->dwSampleFlags & AM_SAMPLE_STOPVALID ? &pProps->tStop : NULL, 
			 dwFlags
         );
    //(*ppOutSample = pOutSample)->AddRef();
    *ppOutSample = pOutSample;
    if (FAILED(hr)) return hr;

    ASSERT(pOutSample);
    IMediaSample2 *pOutSample2;
    if (SUCCEEDED(pOutSample->QueryInterface(IID_IMediaSample2, (void **)&pOutSample2))) 
	{
        /*  Modify it */
        AM_SAMPLE2_PROPERTIES OutProps;
        EXECUTE_ASSERT(SUCCEEDED(pOutSample2->GetProperties(
            FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, tStart), (PBYTE)&OutProps)
        ));
        OutProps.dwTypeSpecificFlags = pProps->dwTypeSpecificFlags;
        OutProps.dwSampleFlags = (OutProps.dwSampleFlags & AM_SAMPLE_TYPECHANGED) | (pProps->dwSampleFlags & ~AM_SAMPLE_TYPECHANGED);
        OutProps.tStart = pProps->tStart;
        OutProps.tStop  = pProps->tStop;
        OutProps.cbData = FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, dwStreamId);
        hr = pOutSample2->SetProperties(FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, dwStreamId), (PBYTE)&OutProps);
        if (pProps->dwSampleFlags & AM_SAMPLE_DATADISCONTINUITY) 
		{
            m_bSampleSkipped = FALSE;
        }
        pOutSample2->Release();
    } 
	else 
	{
        if (pProps->dwSampleFlags & AM_SAMPLE_TIMEVALID) {
            pOutSample->SetTime(&pProps->tStart, &pProps->tStop);
        }
        if (pProps->dwSampleFlags & AM_SAMPLE_SPLICEPOINT) {
            pOutSample->SetSyncPoint(TRUE);
        }
        if (pProps->dwSampleFlags & AM_SAMPLE_DATADISCONTINUITY) {
            pOutSample->SetDiscontinuity(TRUE);
            m_bSampleSkipped = FALSE;
        }

		// Copy the media times
		LONGLONG MediaStart, MediaEnd;
		if (pSample->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR) 
		{
            pOutSample->SetMediaTime(&MediaStart,&MediaEnd);
        }
    }

	if (pSample != NULL)
	{
		LONGLONG MediaStart, MediaEnd;
		if (pSample->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR) 
		{
			pOutSample->SetMediaTime(&MediaStart,&MediaEnd);
		}
	}
	return S_OK;
}

HRESULT CKeystone::SPRS_CloneMediaSample(IMediaSample *pSource, IMediaSample *pDest)
{
	//CheckPointer(pSource,E_POINTER);
	//CheckPointer(pDest,E_POINTER);

	////// Copy the sample data

	////BYTE *pSourceBuffer, *pDestBuffer;
	////long lSourceSize = pSource->GetActualDataLength();

	////#ifdef DEBUG
	////	long lDestSize = pDest->GetSize();
	////	ASSERT(lDestSize >= lSourceSize);
	////#endif

	////pSource->GetPointer(&pSourceBuffer);
	////pDest->GetPointer(&pDestBuffer);

	////CopyMemory((PVOID) pDestBuffer,(PVOID) pSourceBuffer,lSourceSize);

	//// Copy the sample times

	//REFERENCE_TIME TimeStart, TimeEnd;
	//if(NOERROR == pSource->GetTime(&TimeStart, &TimeEnd))
	//{
	//	pDest->SetTime(&TimeStart, &TimeEnd);
	//}

	//LONGLONG MediaStart, MediaEnd;
	//if(pSource->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR)
	//{
	//	pDest->SetMediaTime(&MediaStart,&MediaEnd);
	//}

	//// Copy the media type
	//AM_MEDIA_TYPE *pMediaType;
	//pSource->GetMediaType(&pMediaType);
	//pDest->SetMediaType(pMediaType);
	//DeleteMediaType(pMediaType);

	//// Copy the actual data length
	//long lDataLength = pSource->GetActualDataLength();
	//pDest->SetActualDataLength(lDataLength);

	////Copy preroll
	//pDest->SetPreroll(pSource->IsPreroll());

	////Copy SyncPoint
	//pDest->SetSyncPoint(pSource->IsSyncPoint());

	////Copy Discontinuity
	//pDest->SetDiscontinuity(pSource->IsDiscontinuity());

	////Copy Properties
	//CComQIPtr<IMediaSample2> pSample2Src(pSource);
	//CComQIPtr<IMediaSample2> pSample2Dest(pDest);

	//if (pSample2Src && pSample2Dest)
	//{
	//	//AM_SAMPLE2_PROPERTIES Properties = {0};
	//	//if (S_OK == pSample2Src->GetProperties(sizeof (Properties), (BYTE*) &Properties))
	//	//{
	//	//	HRESULT hr = pSample2Dest->SetProperties(Properties.cbData, (BYTE*)&Properties);
	//	//	if (S_OK != hr)
	//	//	{
	//	//		DbgLog((LOG_TRACE, 0, TEXT("copy IMediaSample2 failed 0x%x"), hr));
	//	//	}
	//	//}

	//	AM_SAMPLE2_PROPERTIES * const pProps = m_pVideo->SampleProps();
	//	HRESULT hr;
	//	if (SUCCEEDED(pDest->QueryInterface(IID_IMediaSample2, (void **)&pDest))) 
	//	{
	//		/*  Modify it */
	//		AM_SAMPLE2_PROPERTIES OutProps;
	//		EXECUTE_ASSERT
	//		(
	//			SUCCEEDED
	//			(
	//				pSample2Dest->GetProperties(FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, tStart), (PBYTE)&OutProps)
	//			)
	//		);
	//		OutProps.dwTypeSpecificFlags = pProps->dwTypeSpecificFlags;
	//		OutProps.dwSampleFlags = (OutProps.dwSampleFlags & AM_SAMPLE_TYPECHANGED) | (pProps->dwSampleFlags & ~AM_SAMPLE_TYPECHANGED);
	//		OutProps.tStart = pProps->tStart;
	//		OutProps.tStop  = pProps->tStop;
	//		OutProps.cbData = FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, dwStreamId);
	//		hr = pSample2Dest->SetProperties(FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, dwStreamId), (PBYTE)&OutProps);
	//		if (pProps->dwSampleFlags & AM_SAMPLE_DATADISCONTINUITY) 
	//		{
	//			m_bSampleSkipped = FALSE;
	//		}
	//		pDest->Release();
	//	} 
	//	else 
	//	{
	//		if (pProps->dwSampleFlags & AM_SAMPLE_TIMEVALID) {
	//			pDest->SetTime(&pProps->tStart, &pProps->tStop);
	//		}
	//		if (pProps->dwSampleFlags & AM_SAMPLE_SPLICEPOINT) {
	//			pDest->SetSyncPoint(TRUE);
	//		}
	//		if (pProps->dwSampleFlags & AM_SAMPLE_DATADISCONTINUITY) {
	//			pDest->SetDiscontinuity(TRUE);
	//			m_bSampleSkipped = FALSE;
	//		}
	//	}
	//}

	return NOERROR;
} 
