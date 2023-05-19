#include "appincludes.h"
#include "Mixer.h"

HRESULT CKeystone::Pipeline_Video_ReceiveVideo(IMediaSample * pSample)
{
	HRESULT hr = 0;

	//TICK INPUT FRAME RATE
	FRRT_TickInputRate(GetTickCount());

	//PAUSING
	while (Pausing == 1)
	{
		Sleep(1);
	}

	//MEDIA TIME EVENTS
	if (MTEV_ThrowMediaTimeEvents == 1)
	{
		LONGLONG TStart = 0;
		LONGLONG TEnd = 0;
		hr = pSample->GetMediaTime(&TStart, &TEnd);
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: MediaTimes= %I64d - %I64d"), TStart, TEnd));

		switch(MTEV_TypeOfMediaTimeEvents)
		{
			case 1: //Just the first value
			{
				LONG_PTR p1 = (TStart>>31) & 0x3; //send the next two higher bits so as to support the full 33 bit length allowed in a PTS value
				LONG_PTR p2 = TStart & 0x7FFFFFFF; // take the lower 31 bits
				//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: p1 = %I32d  p2 = %I32d"), p1, p2));
				NotifyEvent(EC_KEYSTONE_MEDIATIME, p1, p2);
				break;
			}
			case 2: //
			{
			
			}
		}
	}

	//STORE RECEIVED FRAME
	if (OPTM_OptimizedLevel < 1)
	{
		LPBYTE dst = NULL;
		pSample->GetPointer(&dst);
		TMNG_LastFrameReceived_Ticks = GetTickCount();
		CopyBuffer_Rect(dst, SAGR_LastBufferReceived, PIVI_Pitch, PIVI_ConnectedWidth, PIVI_ConnectedHeight);
	}
	
	//MULTI-FRAME DUMP
	if (SAGR_MultiFrameCount > 0)
	{
		HandleMultiFrameGrabSample(pSample);
		SAGR_MultiFrameCount--;
	}

	//Sample grabber - Video Only
	if ((SAGR_GetSample == true) && (USEROPTION_SampleWhat == 0))
	{
		SAGR_GetSample = false;
		SAGR_HandleSampleGrab(pSample);
	}

	if (USEROPTION_DetectBarData == true)
	{
		//DO THX BAR DETECTION HERE
		LPBYTE buf = NULL;
		pSample->GetPointer(&buf);
		BDDT_GenerateBarData(buf, PIVI_ConnectedWidth, PIVI_ConnectedHeight);
	}

	DWORD LastTicks = 0;
	if (VARI_InVarispeed == true)
	{
		LastTicks = GetTickCount();
	}
	while (VARI_InVarispeed == true)
	{
		Sleep(1);
		DWORD CurrentTicks = GetTickCount();
		if (CurrentTicks - LastTicks > (DWORD)VARI_MSVarispeedInterval)
		{
			VARI_ProcessVarispeedSample(pSample);
			pSample->Release();
			return S_OK;
		}
	}
	if (VARI_InFFRW)
	{
		if (FFRW_Rate == 2)
		{
			if (FFRW_2X_DropFrameFlag == true)
			{
				//drop this frame
				FFRW_2X_DropFrameFlag = false; //don't drop the next frame
				pSample->Release();
				return S_OK;
			}
			else
			{
				FFRW_2X_DropFrameFlag = true; //drop the next frame
				//will the time stamping below do the trick?
			}
		}

		//Set time stamps
		REFERENCE_TIME rtS = 0;
		REFERENCE_TIME rtE = 0;
		CRefTime crtStreamTime;
		StreamTime(crtStreamTime);
		rtS = crtStreamTime + 250000;
		rtE = rtS + 333667;
		pSample->SetTime(&rtS, &rtE);
		NotifyEvent(EC_KEYSTONE_SETPRESENTATIONTIME, 2, 0);
	}

	return Pipeline_Video_ReceiveVideo_Step2(pSample);
}

HRESULT CKeystone::Pipeline_Video_ReceiveVideo_Step2(IMediaSample * pSample)
{
	HRESULT hr = S_OK;

	//CHECK FOR MEDIATYPE CHANGE
	bool NewMT = MDTY_CheckForMediaTypeChange(pSample);
	if (NewMT)
	{
		//Pass new media type to renderer
		//pSample->SetMediaType((AM_MEDIA_TYPE*) &m_mtOut);
	}
	else
	{
		pSample->SetMediaType(NULL);
	}

	////MIX GUIDES
	LPBYTE src = NULL;
	pSample->GetPointer(&src);
	//hr = GIDE_MixGuides(src, PIVI_ConnectedWidth, PIVI_ConnectedHeight, false);
	//if (FAILED(hr)) return hr;
	
	//// MIX LINE21
	//hr = Pipeline_Line21_MixLine21(src, PIVI_ConnectedWidth, PIVI_ConnectedHeight);
	//if (FAILED(hr)) return hr;

	//// MIX SUBPICTURE
	//hr = Pipeline_Subpicture_MixSubpicture(src, PIVI_ConnectedWidth, PIVI_ConnectedHeight);
	//if (FAILED(hr)) return hr;

	////Sample grabber - Video And Subpicture
	//if ((SAGR_GetSample == true) && (USEROPTION_SampleWhat == 4))
	//{
	//	SAGR_GetSample = false;
	//	SAGR_HandleSampleGrab(pSample);
	//}

	//RESIZE
	if (OPTM_OptimizedLevel < 4)
	{
		SCLE_Resize(src);
	}

    return Pipeline_Video_ThreeTwo(pSample);
}

HRESULT CKeystone::Pipeline_Video_ThreeTwo(IMediaSample * pIn)
{
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: ThreeTwo()")));
	RECT r1;

	if (m_pVideo->CurrentMediaType().formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2* inType = (VIDEOINFOHEADER2*)m_pVideo->CurrentMediaType().Format();
		CopyRect(&r1, &inType->rcTarget);
	}
	else
	{
		VIDEOINFOHEADER* inType = (VIDEOINFOHEADER*)m_pVideo->CurrentMediaType().Format();
		CopyRect(&r1, &inType->rcTarget);
	}

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
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: dwFlags: %d"), dwFlags));

   	if (SampProps.dwSampleFlags & 0x04) 
	{
		//Reset interlacing on discontinutity
		TCNE_Progressive = NULL;
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
	if (TCNE_Progressive != progressive_frame)
	{
		NotifyEvent(EC_KEYSTONE_INTERLACING, progressive_frame, 0);	
	}
	TCNE_Progressive = progressive_frame;

	dwFlags = SampProps.dwTypeSpecificFlags;
	top_field_first = (dwFlags >> 17) & 1;

	dwFlags = SampProps.dwTypeSpecificFlags;
	BYTE picture_structure = (dwFlags & 0xC0000L) >> 18;

	dwFlags = SampProps.dwTypeSpecificFlags;
	bool repeat_first_field = (dwFlags >> 20) & 1;

	dwFlags = SampProps.dwTypeSpecificFlags;
	bool progressive_sequence = (dwFlags >> 21) & 1;

	dwFlags = SampProps.dwTypeSpecificFlags;

	REFERENCE_TIME atpf = FRRT_TargetFramerate_In_ATPF;

	////DEBUGGING
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | ENCODED FRAME #%d FLAGGING = rff=%d, tff=%d, atpf=%I64d, ps=%d, pf=%d, picst=%d"), TCNE_FrameCounting_EncodeFrameNo, repeat_first_field, top_field_first, atpf, progressive_sequence, progressive_frame, picture_structure));
	//TCNE_FrameCounting_EncodeFrameNo += 1;
	////DEBUGGING

	HRESULT hr = S_OK;

	NotifyEvent(EC_KEYSTONE_PROGRESSIVESEQUENCE, progressive_sequence, 0);
	NotifyEvent(EC_KEYSTONE_FIELDORDER, TCNE_tff_encoded, 0);
	//NotifyEvent(EC_Keystone_32, 1, 0);

   	if (SampProps.dwSampleFlags & 0x04) 
	{
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: EC_Keystone_DISCONTINUITY")));
		NotifyEvent(EC_KEYSTONE_DISCONTINUITY, 0, 0);
		TCNE_tff_encoded = top_field_first;
		//bTDM = true;
		//TDMCnt = 0;
		TCNE_BufferIsEmpty = true;

		//CRefTime crtST;
		//rtLastEnd = crtST.m_time + 20000;
	}

	if (m_pVideo->bNewSegmentReceived)
	{
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: New Segment")));
		TCNE_tff_encoded = top_field_first;
		//m_pVideo->bNewSegmentReceived = false; //now done below
	}

	if (REN_Renderer == 0) goto Skip32; //do not run 3:2 if using VMR9

	// ==================================================================================================
	//    BEGIN 3:2
	// ===================================================================

	if ((r1.bottom < 500) && (USEROPTION_AllowTelecine == 1))
	{
		BumpLines = 3;

		LPBYTE src = NULL;
		pIn->GetPointer(&src);

		if (!TCNE_tff_encoded)
		{
			BumpLines=4;
		}

		if (repeat_first_field)
		{
			if (TCNE_BufferIsEmpty) //SITUATION 2
			{
				if ((TCNE_tff_encoded & !top_field_first) || (!TCNE_tff_encoded & top_field_first))
				{
					//Skip the extraction
					//This is from LOTR. End of a 30fps sequence. The first frame after is BFF, RFF.
					//Which is no good because the source content is TFF so we need a top and bottom field here.
					//Not a bottom, top, bottom.
					goto NextStep;
				}
				TCNE_extract(top_field_first, src);
				TCNE_NeedTopField = !top_field_first;
				TCNE_BufferIsEmpty = false;
				//DbgLog((LOG_TRACE, 0, TEXT("Keystone: 3:2 Situation 2.")));
				goto NextStep;
			}
			else //SITUATION 4
			{
				//Verify we're not outputting two of the same fields in a row.
				if (TCNE_NeedTopField != top_field_first)
				{
					TCNE_extract(top_field_first, src);
					TCNE_NeedTopField = !top_field_first;
					TCNE_BufferIsEmpty = false;
					goto NextStep;
				}

				TCNE_extract(top_field_first, src);
				TCNE_NeedTopField = !top_field_first;
				TCNE_BufferIsEmpty = true;
				hr = Pipeline_Video_ThreeTwoIOS(pIn, &r1, BumpLines);
				if FAILED(hr) return hr;
				//DbgLog((LOG_TRACE, 0, TEXT("Keystone: 3:2 Situation 4.")));
				goto NextStep;
			}
		}
		else
		{
			if (TCNE_BufferIsEmpty) //SITUATION 1
			{
				TCNE_BufferIsEmpty = true; //just for looks
				//DbgLog((LOG_TRACE, 0, TEXT("Keystone: 3:2 Situation 1.")));
				goto NextStep;
			}
			else //SITUATION 3
			{
				//Verify we're not outputting two of the same fields in a row.
				//untested
				if (TCNE_NeedTopField != top_field_first)
				{
					TCNE_extract(top_field_first, src);
					TCNE_NeedTopField = !top_field_first;
					TCNE_BufferIsEmpty = false;
					goto NextStep;
				}

				TCNE_extract(top_field_first, src);
				TCNE_NeedTopField = !top_field_first;

				hr = Pipeline_Video_ThreeTwoIOS(pIn, &r1, BumpLines);
				if FAILED(hr) return hr;
				TCNE_extract(!top_field_first, src);
				TCNE_NeedTopField = top_field_first;
				TCNE_BufferIsEmpty = false;
				//DbgLog((LOG_TRACE, 0, TEXT("Keystone: 3:2 Situation 3.")));
				//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Sample skipped: %d."), iEncodeFrameNo-1));
				pIn->Release();
				return hr;
			}
		}
	}
NextStep:;

Skip32:;
	return Pipeline_Video_NonThreeTwoIOS(pIn, &r1, BumpLines);
}

HRESULT CKeystone::Pipeline_Video_NonThreeTwoIOS(IMediaSample * pIn, RECT * r1, int CopyLine)
{
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: NonThreeTwoIOS()")));

	if (FRFI_ForceBumpFieldsDown == true) CopyLine += 1;

	HRESULT hr = S_OK;
    IMediaSample * pOut = NULL;
    hr = InitializeOutputSample(pIn, &pOut);
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
	//SaveBufferToFile("C:\\Temp\\src.bin", "w", src, 720*480*2, 720, 480);
	CopyBuffer_Rect_YUY2toUYVY(src, 0, 0, PIVI_Pitch, dst, 0, CopyLine, PIVO_Pitch, PIVI_ConnectedWidth, r1->bottom);
	//SaveBufferToFile("C:\\Temp\\dst.bin", "w", dst, 720*486*2, 720, 486);

	////DEBUGGING
	//Pipeline_Core_DeliverMixedSample(pOut);
	//return S_OK;
	////DEBUGGING

	return Pipeline_Video_ReceiveVideo_LastStep(pOut, r1, false);
}

HRESULT CKeystone::Pipeline_Video_ThreeTwoIOS(IMediaSample * pIn, RECT * r1, int CopyLine)
{
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: ThreeTwoIOS()")));

	if (FRFI_ForceBumpFieldsDown == true) CopyLine += 1;

	IMediaSample * pTT = NULL;
    HRESULT hr = InitializeOutputSample(pIn, &pTT);
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
	//SaveBufferToFile("C:\\Temp\\src.bin", "w", src, 720*480*2, 720, 480);
	CopyBuffer_Rect_YUY2toUYVY(TCNE_buffer, 0, 0, PIVI_Pitch, dst, 0, CopyLine, PIVI_Pitch, PIVI_ConnectedWidth, PIVI_ConnectedHeight);
	//SaveBufferToFile("C:\\Temp\\dst.bin", "w", dst, 720*486*2, 720, 486);

	return Pipeline_Video_ReceiveVideo_LastStep(pTT, r1, true);
}

HRESULT CKeystone::Pipeline_Video_ReceiveVideo_LastStep(IMediaSample * pOut, RECT * r1, bool Is32Sample)
{
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: ReceiveStep5()")));

	VIDEOINFOHEADER* outType = (VIDEOINFOHEADER*)m_pOutput->CurrentMediaType().Format();
	int dstpitch = outType->bmiHeader.biWidth * 2;
	IntersectRect(r1, &outType->rcTarget, r1);

	LPBYTE dst = NULL;
	pOut->GetPointer(&dst);

	//PROCAMP
	if (USEROPTION_DoProcAmp)
	{
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: ProcAmp")));
		ApplyBrContHueSat_UYVY(dst, r1->right, r1->bottom, r1->right * 2, USEROPTION_ProcAmpHalfFrame);
	}

	//COLOR FILTER
	if (USEROPTION_DoColorFiltering)
	{
		ApplyColorFilters_UYVY(dst, r1->right, r1->bottom, r1->right * 2, USEROPTION_WhichColorFilter);
	}

	TCNE_FrameCounting_DisplayFrameNo += 1;

	//===========================================================================================
	// TIME STAMPING
	//===========================================================================================
	//VIDEOINFOHEADER2 * v2 = (VIDEOINFOHEADER2*)m_pVideo->CurrentMediaType().Format();
	//if (v2->bmiHeader.biCompression == 1498831189) goto SkipTimeStamping;
	////Beep(1000,1);

	REFERENCE_TIME rtS = 0;
	REFERENCE_TIME rtE = 0;
	pOut->GetTime(&rtS, &rtE);
	//int Dur = rtE - rtS;
	LONGLONG ATPF = FRRT_TargetFramerate_Out_ATPF;

    CRefTime crtST;
	StreamTime(crtST);

	if (crtST.m_time > 108000000000) // The stream time is greater than three hours. This is an outside-the-box situation and implies that the graph is new and Run() has not made its way through all filters. We send NULL timestamps until we have a real stream time.
	{
		//NotifyEvent(EC_KEYSTONE_STREAMTIME, 321, 0);
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | Invalid Stream Time.")));
		//#ifdef DEBUG
		//		pOut->SetTime(0, 0);
		//#else
		//		pOut->SetTime(NULL, NULL);
		//#endif
	}
	else if (m_State != State_Running)
	{
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | !State_Running.")));
		//rtS = crtST.m_time + 1000;
		//rtE = rtS + ATPF;
		//pOut->SetTime(&rtS, &rtE);
		//pOut->SetTime(0, 0);
		//pOut->SetTime(NULL, NULL);
	}
	else
	{
		//CheckForPresentationTimeStreamTimeDiscrepency(pOutSample);
		int LE_CST = crtST.m_time - TMNG_LastSampleEndTime;
		//if (abs(LE_CST) > 10000000) //|| (LE_CST < 10000000))
		if (LE_CST > 10000000) //|| (LE_CST < 10000000))
		{
			//The difference between the current stream time and the last end has grown too large
			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | Sample time adjusted.")));
			TMNG_LastSampleEndTime = crtST.m_time + 6000;
		}
		else if (!Is32Sample)
		{
			REFERENCE_TIME rtDelta = rtE - TMNG_LastSampleEndTime;
			//DbgLog((LOG_TRACE, 0, TEXT("Keystone: rec-del delta: %I64d"), rtDelta));
			if (rtDelta > 255000)
			{
				pOut->GetTime(&TMNG_LastSampleStartTime, &TMNG_LastSampleEndTime);
				DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | Sample time restarted.")));
			}
		}

		//2) match up the previous end with the current start
		rtS = TMNG_LastSampleEndTime;
		rtE = rtS + ATPF;

		//3) if source is 24fps (417083) set proper duration on sample
		if (ATPF > 410000)
		{
			rtE = rtS + 333667;
		}

		pOut->SetTime(&rtS, &rtE);
		NotifyEvent(EC_KEYSTONE_SETPRESENTATIONTIME, 1, 0);
		TMNG_LastSampleStartTime = rtS;
		TMNG_LastSampleEndTime = rtE;

	}

	if (m_pVideo->bNewSegmentReceived)
	{
		TMNG_LastSampleStartTime = 0;
		TMNG_LastSampleEndTime = 0;
		m_pVideo->bNewSegmentReceived = false;
	}

	//===========================================================================================
	// END TIME STAMPING
	//===========================================================================================

SkipTimeStamping:

	LPBYTE src = NULL;
	pOut->GetPointer(&src);

	//FIELD SPLITTING
	if (FRFI_DoFieldSplitting == 1)
	{
		LPBYTE tmp = (LPBYTE)malloc(r1->right * r1->bottom * 2);

		int Pitch = 1440;
		LPBYTE d1 = tmp;
		int HalfFrameBytes = (r1->bottom/2) * Pitch;
		LPBYTE d2 = tmp + HalfFrameBytes;
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: D2: %d"), (d2 - d1)/1440));
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
		CopyBuffer_Rect(tmp, src, 1440, r1->right, r1->bottom);
		free(tmp);
	}

	//GABOR'S FRAME STEPPING
	while (!m_pVideo->m_bFlushing && FRST_FrameStepping)
	{
		Sleep(1);
		if(FRST_DeliverFrameStepSample)
		{
			//pSample->SetTime(NULL, NULL);
			DbgLog((LOG_TRACE, 0, TEXT("Keystone: Framestep sample.")));
			FRST_DeliverFrameStepSample = false;
			//LONGLONG rtStart = 0, rtStop = _I64_MAX;
			//pOut->SetTime(&rtStart, &rtStop);

			//Set time stamps
			REFERENCE_TIME rtS = 0;
			REFERENCE_TIME rtE = 0;
			CRefTime crtStreamTime;
			StreamTime(crtStreamTime);
			rtS = crtStreamTime + 250000;
			rtE = rtS + 333667;
			pOut->SetTime(&rtS, &rtE);

			NotifyEvent(EC_KEYSTONE_SETPRESENTATIONTIME, 7, 0);
			//Beep(100,1);
			return Pipeline_Core_Entrance(pOut, r1->right, r1->bottom);
		}
	}

	return Pipeline_Core_Entrance(pOut, r1->right, r1->bottom);
}
