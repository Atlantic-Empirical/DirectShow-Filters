#include "../Utility/appincludes.h"
#include "../../VideoProcessing/Mixer/Mixer.h"


HRESULT CKeystone::ReceiveVideoTwo(LPBYTE EncodeBuffer)
{
//	int iDebug = 0;
//	while ((m_iQueueWr - m_iQueueRd) >= (QUEUE_ENTS / 2))
//	{
//		iDebug++;
//	}

	if ((m_iQueueWr - m_iQueueRd) >= (QUEUE_ENTS / 2))
	{
		DWORD dwRes = WaitForSingleObject(m_hEvent, INFINITE);
	}

	if (NULL == m_aQueue)
	{
		m_aQueue = (MYBUF*)::CoTaskMemAlloc(FRAME_SIZE * QUEUE_ENTS);
		if (NULL == m_aQueue)
			return E_OUTOFMEMORY;

		for (int iBuf = 0; iBuf < QUEUE_ENTS; iBuf++)
		{
			SetFullBufferBGColorYUY2(m_aQueue[iBuf], 3840, 1920, 1080);
		}
	}

	MYBUF* pDest = &m_aQueue[m_iQueueWr & QUEUE_MASK]; 
	memcpy(pDest, EncodeBuffer, FRAME_SIZE);

	m_iQueueWr++;
	VTM_HaveData = 1;

	return S_OK;
}

HRESULT CKeystone::MixAndDeliverVideo(IMediaSample* pOut, int w, int h)
{
	if ((m_iQueueWr - m_iQueueRd) > 1)
	{
		m_iQueueRd++;
	}
//	else
//	{
//		Beep(1000,1);
//	}
	
//	if ((m_iQueueWr - m_iQueueRd) < (QUEUE_ENTS/2))
//	{
		SetEvent(m_hEvent);
//	}
	//else
	//{
	//	Beep(1000,1);	// occasionally as expected
	//}

    //DEBUGGING
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: MixAndDeliverVideo")));
	//DEBUGGING

    if (bFilterIsLocked == true) return E_FAIL;
	HRESULT hr = S_OK;

	//New Video Two Mix
	LPBYTE dst = NULL;
	pOut->GetPointer(&dst);
	if (VTM_HaveData == 1)
	{
		PBYTE cpysrc = PBYTE(&m_aQueue[m_iQueueRd & QUEUE_MASK]);

		//R1_X = Horizontal
		//R1_Y = Vertical

		//cpysrc = the YUV image
		if ((AnchorX == 0) && (AnchorY == 0))
		{
			//5050 A - source on left
			//R1_X is a percentage indicating how much of A vs B to use
			int BytesToCopy = fround(((float) R1_X / 100) * 3840, 0);
			for (int y = 0; y < 1080; y++)
			{
				memcpy_amd(dst, cpysrc, BytesToCopy); //copy half of the bytes from the line
				dst += 3840;
				cpysrc += 3840;
			}
		}
		else if ((AnchorX == 1) && (AnchorY == 1))
		{
			//5050 B - source on right
			//R1_X is a percentage indicating how much of A vs B to use
			int BytesToCopy = fround(((float) (100-R1_X) / 100) * 3840, 0);
			for (int y = 0; y < 1080; y++)
			{
				dst += 3840 - BytesToCopy;
				cpysrc += 3840 - BytesToCopy;
				memcpy_amd(dst, cpysrc, BytesToCopy); //copy half of the bytes from the line
				dst += BytesToCopy;
				cpysrc += BytesToCopy;
			}
		}
		else if ((AnchorX == 1) && (AnchorY == 0))
		{
			//CHECKER BOARD A - source top left
			for (int y = 0; y < 540; y++)
			{
				memcpy_amd(dst, cpysrc, 1920); //copy half of the bytes from the line
				dst += 3840;
				cpysrc += 3840;
			}

			cpysrc += 1920;

			for (int y = 540; y < 1080; y++)
			{
				dst += 1920;
				memcpy_amd(dst, cpysrc, 1920); //copy half of the bytes from the line
				dst += 1920;
				cpysrc += 3840;
			}
		}
		else if ((AnchorX == 0) && (AnchorY == 1))
		{
			//CHECKER BOARD B - source top right
			for (int y = 0; y < 540; y++)
			{
				//cpysrc is the YUV data
				cpysrc += 1920;
				dst += 1920;
				memcpy_amd(dst, cpysrc, 1920); //copy half of the bytes from the line
				dst += 1920;
				cpysrc += 1920;
			}
			for (int y = 540; y < 1080; y++)
			{
				memcpy_amd(dst, cpysrc, 1920); //copy half of the bytes from the line
				dst += 3840;
				cpysrc += 3840;
			}
		}
		else if ((AnchorX == 0) && (AnchorY == 2))
		{
			//5050 left-left
			for (int y = 0; y < 1080; y++)
			{
				//we want to take the left side of cpysrc and put it on the right side of dst
				//todo: make it clear that the source is on the right side now
				dst += 1920;
				memcpy_amd(dst, cpysrc, 1920); //copy half of the bytes from the line
				dst += 1920;
				cpysrc += 3840;
			}
		}
		else if ((AnchorX == 2) && (AnchorY == 0))
		{
			//5050 right-right
			for (int y = 0; y < 1080; y++)
			{
				//we want to take the right side of cpysrc and put it on the left side of dst
				//todo: make it clear that the source is on the left side now
				cpysrc += 1920;
				memcpy_amd(dst, cpysrc, 1920); //copy half of the bytes from the line
				dst += 3840;
				cpysrc += 1920;
			}
		}
		else if ((AnchorX == 3) && (AnchorY == 0))
		{
			//All A
			//DO NOTHING
		}
		else if ((AnchorX == 3) && (AnchorY == 1))
		{
			//All B
			for (int y = 0; y < 1080; y++)
			{
				memcpy_amd(dst, cpysrc, 3840);
				dst += 3840;
				cpysrc += 3840;
			}
		}
		else
		{
			//DO FULL MIX ACCORDING TO RADIALS
			//hr = CMixer::MixUYVY_VideoTwo(L21MixData.pImage, dst, AnchorX, AnchorY, R1_X, R1_Y, R2_X, R2_Y);
			//if (FAILED(hr)) return hr;
		}
	}

	//FRAME GRAB
	if (bGetSample == true)
	{
		bGetSample = false;
		HandleSampleGrab(pOut);
	}


	//	//MIX OSD
//	if (bPendingOSDMix != true) goto SkipOSDMix;
//	if ((int)OSDMixData.pImage == 0xcdcdcdcd) goto SkipOSDMix;
//	if (OSDMixData.pImage)
//	{
//		if (OSDTicker < OSDTickGoal)
//		{
//			hr = CMixer::MixUYVY_OSD(OSDMixData.pImage, OSDMixData.nW, OSDMixData.nH, dst, w, h, OSDMixData.nX, OSDMixData.nY, OSDMixData.KeyColor); 
//			if (FAILED(hr)) return hr;
//			OSDTicker += 1;
//			//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OSD Ticker. %d"), OSDTicker));
//		}
//		else
//		{
//			//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OSD Ticker Maxedout")));
//			bPendingOSDMix = false;
//		}
//	}
//SkipOSDMix:

//	//Mix Guides
//	if (bMixGuides == true)
//	{
//		hr = CMixer::MixUYVY_Guides(dst, w, h, nGuide_L, nGuide_T, nGuide_R, nGuide_B, dwGuideColor);
//		if (FAILED(hr)) return hr;
//	}
//
//	if (iShowActionTitleGuides == 1)
//	{
//		hr = CMixer::MixUYVY_ActionTitleGuides(dst, w, h, dwGuideColor);
//		if (FAILED(hr)) return hr;
//	}
//
//	//GET GOP TIMECODE
//	LONGLONG TStart = 0;
//	LONGLONG TEnd = 0;
//    pOut->GetMediaTime(&TStart, &TEnd);
//	if ((TStart>0) || (TEnd>0)) 
//	{
//		////DEBUGGING
//		//int h, m, s, f;
//		//RTToHMSF(TStart, 29.97, h, m, s, f);
//		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: SH GOP TC: %d:%d:%d;%d"), h, m, s, f));
//		////DEBUGGING
//		
//		if (TStart == LastNewTStart)
//		{
//			//Increment it
//			if ((llTargetFR_In_ATPF == 333667) || (llTargetFR_In_ATPF == 333666) || (llTargetFR_In_ATPF == 417084) || (llTargetFR_In_ATPF == 417083))
//			{
//				TStartIncr += 333667;
//			}
//			else
//			{
//				TStartIncr += llTargetFR_In_ATPF;
//			}
//			TStart += TStartIncr;
//		}
//		else
//		{
//			//New MPEG GOP TC
//			LastNewTStart = TStart;
//			TStartIncr = 0;
//		}
//
//		int FPS = 0;
//		double dFPS = 0;
//		if ((llTargetFR_In_ATPF == 333667) || (llTargetFR_In_ATPF == 333666) || (llTargetFR_In_ATPF == 417084) || (llTargetFR_In_ATPF == 417083))
//		{
//			FPS = 2997;
//			dFPS = 29.97;
//		}
//		else if (llTargetFR_In_ATPF == 400000)
//		{
//			FPS = 25;
//			dFPS = 25;
//		}
//
//		int h, m, s, f;
//		RTToHMSF(TStart, dFPS, h, m, s, f);
//
//		LONG_PTR out = (h<<24) | (m<<16) | (s<<8) | f;
//
//		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: GOP TC: %d - %d"), TStart, TEnd));
//		NotifyEvent(EC_KEYSTONE_MPEGTC, out, (LONG_PTR)FPS);
//	}
//	else
//	{
//		goto SkipGOPTCMix;
//	}
//
//	//Mix GOP Timecodes
//	if (iMixGOPTC > 0)
//	{
//		double dOut = 0;
//		if ((llTargetFR_In_ATPF == 333667) || (llTargetFR_In_ATPF == 333666) || (llTargetFR_In_ATPF == 417084) || (llTargetFR_In_ATPF == 417083))
//		{
//			dOut = 29.97;
//		}
//		else if (llTargetFR_In_ATPF == 400000)
//		{
//			dOut = 25;
//		}
//
//		//Create bmp of ref time
//		if (TStartIncr == 0)
//		{
//			RTtoBMP(m_hDC, m_hBM, m_hFONT, (REFERENCE_TIME)TStart, dOut, 0x0000ff, 0x000000);
//		}
//		else
//		{
//			RTtoBMP(m_hDC, m_hBM, m_hFONT, (REFERENCE_TIME)TStart, dOut, 0x00ff00, 0x000000);		
//		}
//
//		//Get a bitmap from the hbitmap
//		BITMAP bm;
//		GetObject(m_hBM, sizeof(bm), &bm);
//
//		//CSC
//		LPBYTE GOPTC_UYVY = (LPBYTE)malloc(bm.bmHeight * bm.bmWidth * 2); //maybe needed	
//		CSCManual::RGB32ToUYVY((LPBYTE)bm.bmBits, bm.bmWidthBytes, bm.bmWidth, bm.bmHeight, GOPTC_UYVY);
//
//		//Mix
//		CMixer::MixUYVY_OSD(GOPTC_UYVY, 100, 16, dst, w, h, 540, 30, 0x00000000);
//	}
//SkipGOPTCMix:
//
//	//TURNED OFF TO SEE IF WE CAN MAKE THE LAST-OUTPUT-FRAME GRAB WORK
//	////Sample grabber - Full Mix
//	//if ((bGetSample == true) && (SampleWhat == 3))
//	//{
//	//	bGetSample = false;
//	//	HandleSampleGrab(pOut);
//	//}
//
//	////DEBUGGING
//	//LPBYTE ConversionTarget = (LPBYTE)malloc(720 * 480 * 3);
//	//CSCManual::UYVYToRGB24(dst, 720, 480, ConversionTarget);
//	//NotifyEvent(EC_KEYSTONE_FORCEFRAMEGRAB, (LONG_PTR)ConversionTarget, 720 * 480 * 3);
//	////DEBUGGING

	//Get on with out sample initialization
	pOut->SetActualDataLength(w * h * 2);
	pOut->SetSyncPoint(TRUE);
	pOut->SetDiscontinuity(FALSE);

	VIDEOINFOHEADER2 * v2 = (VIDEOINFOHEADER2*)m_pVideoOne->CurrentMediaType().Format();
	if (v2->bmiHeader.biCompression == 1498831189) goto gtDeliverMixedSample; //If we're connected to NullVideoSource

	//Disabled 12/3/2005, still menus seem ok without this now. Probably due to the time stamping code in Receive5 in the video pin.
	////Workaround for still menus and DeckLink renderer.
	////nvDecoder sends single samples without times for still menu changes.
	////Renderer needs times, so add them.
	////DWORD dwCurrentTicks = GetTickCount();
	////int iTDiff = abs(short(dwLastSampleProcessedAt - dwCurrentTicks));
	//////DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Gap in samples detected. %d"), iTDiff));
	////if (iTDiff > 200)  //250= 1/4sec - this has to be short enough to catch quick button changes by user.
	////{
	////	//REFERENCE_TIME rtStart, rtEnd;
	////	//StreamTime(crtStreamTime);
	////	//rtStart = crtStreamTime.m_time; //+ 100000;
	////	//rtEnd = rtStart + llTargetFR_Out_ATPF;
	////	//pOut->SetTime(&rtStart, &rtEnd);
	////	////pOut->SetTime(NULL, NULL);
	////	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Gap in samples detected. %d LARGE"), iTDiff));
	////}
	////dwLastSampleProcessedAt = GetTickCount();



	//opFrameCnt += 1;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OP Frame Count: %d"), opFrameCnt));
	////HandleSampleGrab(pOut);


gtDeliverMixedSample:
	return DeliverMixedSample(pOut);
}

HRESULT CKeystone::DeliverMixedSample(IMediaSample * pOut)
{
	//DEBUGGING
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: DeliverMixedSample")));
	//DEBUGGING

	//Sleep(1000);
	//Beep(200, 1);

	HRESULT hr = S_OK;

	////DEBUGGING
	//Media time
	//LONGLONG pMedTime_Start = 0;
	//LONGLONG pMedTime_End = 0;
	//pOutSample->GetMediaTime(&pMedTime_Start, &pMedTime_End);
	//pOutSample->SetMediaTime(NULL, NULL);

	//pOutSample->SetMediaTime(&llMediaTime_LastSamp_Start, &llMediaTime_LastSamp_End);
	//llMediaTime_LastSamp_Start += llTargetFR_Out_ATPF;  //Set MediaTimes to 0 at Run, then increment by ATPF
	//llMediaTime_LastSamp_End += llTargetFR_Out_ATPF;

	////To make AVI Mux Happy
	//pOutSample->SetMediaTime(&llFrameCounter, &llFrameCounter);
	//llFrameCounter++;
	//REFERENCE_TIME pStrTime_Start = 0;
	//REFERENCE_TIME pStrTime_End = 0;
	//pOutSample->GetTime(&pStrTime_Start, &pStrTime_End);
	//pOutSample->SetTime(&pStrTime_Start, &pStrTime_End +1);
	////End AVI

	//long lPresTimeDelta = pStrTime_End - pStrTime_Start;
	//DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: Presentation time diff: %d"), lPresTimeDelta));
	//if (lPresTimeDelta <= 0)
	//{
	//	//WTF, the end time is before the start time.
	//	DbgLog((LOG_TRACE,0,TEXT("Presentation time error. End is before beginning. %d %d"), pStrTime_Start, pStrTime_End));
	//	
	//	////Maybe this will fix it.
	//	//pOutSample->SetTime(&pStrTime_Start, &pStrTime_End + 1);
	//}
	
	//REFERENCE_TIME pCurrentTime = 0;
	//this->m_pClock->GetTime(&pCurrentTime);
	//DbgLog((LOG_TRACE,0,TEXT("Current Stream Time: %Xh"), pCurrentTime));

	//REFERENCE_TIME pPrivateTime = 0;
	//this->m_pClock->GetTime(&pPrivateTime);
	//DbgLog((LOG_TRACE,0,TEXT("Private Stream Time: %Xh"), pPrivateTime));

	//DbgLog((LOG_TRACE,0,TEXT("Media Times - Start: %d  End: %d"), pMedTime_Start, pMedTime_End));
	//DbgLog((LOG_TRACE,0,TEXT("Stream Times - Start: %Xh  End: %Xh"), pStrTime_Start, pStrTime_End));

	//REFERENCE_TIME pCurrentTime = 0;
	//m_pClock->GetTime(&pCurrentTime);

	//END DEBUGGING

	//DEBUGGING
	//Beep(1000, 1);

	////DEBUGGING OUTPUT SAMPLE

	//Set presentation times

	//Deliver
	TickOutputRateTime(GetTickCount());

	//IMPORTANT IMPORTANT
	//If 32 is running set the presentation time as needed

	//if (llTargetFR_Out_ATPF > 410000)
	//{
	//	pOutSample->SetTime(&m_pVideoOne->rtLast32Sample_StartTime, &m_pVideoOne->rtLast32Sample_EndTime);
	//	DbgLog((LOG_TRACE, 0, TEXT("Presentation Times - Start: %I64d  End: %I64d"), m_pVideoOne->rtLast32Sample_StartTime, m_pVideoOne->rtLast32Sample_EndTime));

	//	//DbgLog((LOG_TRACE, 0, TEXT("Media Times - Start: %d  End: %d"), pMedTime_Start, pMedTime_End));
	//	//NotifyEvent(EC_KEYSTONE_SAMPLETIMES, m_pVideoOne->rtLast32Sample_StartTime, m_pVideoOne->rtLast32Sample_EndTime);

	//	m_pVideoOne->rtLast32Sample_StartTime += 333667;
	//	m_pVideoOne->rtLast32Sample_EndTime += 333667;
	//}

	//if (m_pVideoOne->bRun32)
	//{
	//	pOutSample->SetTime(&m_pVideoOne->rtLast32Sample_StartTime, &m_pVideoOne->rtLast32Sample_EndTime);

	//	//DbgLog((LOG_TRACE,0,TEXT("Media Times - Start: %d  End: %d"), pMedTime_Start, pMedTime_End));
	//	//NotifyEvent(EC_KEYSTONE_SAMPLETIMES, m_pVideoOne->rtLast32Sample_StartTime, m_pVideoOne->rtLast32Sample_EndTime);

	//	m_pVideoOne->rtLast32Sample_StartTime += 333667;
	//	m_pVideoOne->rtLast32Sample_EndTime += 333667;
	//}

	//DEBUGGING
	//pOutSample->SetTime(NULL, NULL);
	//DEBUGGING

	//DEBUGGING
	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;
	//pOutSample->SetTime(&rtS, &rtE);
	//DEBUGGING

	//Beep(1000, 1);
	//pOutSample->SetMediaTime(NULL, NULL);

	////DEBUGGING
	//LPBYTE src = NULL;
	//pOutSample->GetPointer(&src);
	//SaveBufferToFile("C:\\Temp\\Dump3.bin", "w", src, 691200, 720, 480);
	////DEBUGGING

	//PERFORMANCE
	//Stop the clock and log it (if PERF is defined)
	MSR_STOP(m_idTransform);
	
	////m_pClock->GetTime(&m_pVideoOne->rtSampleDelivered);
	////REFERENCE_TIME rtMSElapsed = (m_pVideoOne->rtSampleDelivered - m_pVideoOne->rtSampleReceived)/100000000;
	////StreamTime(m_pVideoOne->crtSampleDelivered);
	////REFERENCE_TIME rtMSElapsed = (m_pVideoOne->crtSampleDelivered.m_time - m_pVideoOne->crtSampleReceived.m_time);
	//m_pVideoOne->dwSampleDelivered = GetTickCount();
	//REFERENCE_TIME rtMSElapsed = (m_pVideoOne->dwSampleDelivered - m_pVideoOne->dwSampleReceived);
	//LONGLONG rtReceived = 0;
	//LONGLONG rtReceiveda = 0;
	//pOutSample->GetMediaTime(&rtReceived, &rtReceiveda);

	//m_pVideoOne->dwSampleDelivered = GetTickCount();
	//LONG a = 0;
	//a = pOutSample->GetActualDataLength();
	//REFERENCE_TIME rtMSElapsed = (m_pVideoOne->dwSampleDelivered - a);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Sample processing time: %I64d"), rtMSElapsed));

	//lSampleCount++;
	//lSumOfProcessingTimes += rtMSElapsed;
	//int Avg = lSumOfProcessingTimes/lSampleCount;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Average sample processing time: %d"), Avg));

	//PERFORMANCE

	// Begin debug



	// End debug


/*
	//Check to see if samples are running late and set times as needed
	REFERENCE_TIME rtS = 0;
	REFERENCE_TIME rtE = 0;
	pOut->GetTime(&rtS, &rtE);
	StreamTime(crtStreamTime);
	
	//REFERENCE_TIME rtDif = crtStreamTime - m_pVideoOne->rtLast32Sample_StartTime;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: DIF = %I64d"), rtDif));
	//m_pVideoOne->rtLast32Sample_StartTime = crtStreamTime;

	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: TIMES %I64d %I64d"), rtS, rtE));
	//REFERENCE_TIME rtDif = rtS - m_pVideoOne->rtLast32Sample_StartTime;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: DIF %I64d"), rtDif));
	//m_pVideoOne->rtLast32Sample_StartTime = rtS;
	//m_pVideoOne->rtLast32Sample_EndTime += rtE;

	////DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: CRT %I64d"), m_pVideoOne->crtSampleDelivered));

	REFERENCE_TIME rtDif = crtStreamTime.m_time - rtS;
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: DIF %I64d"), rtDif));
	if (rtDif > 0)
	{
		//Sample is late
		Beep(1000,1);
		//rtS = crtStreamTime; // + 100000;
		//rtE = rtS + llTargetFR_Out_ATPF;
		//pOutSample->SetTime(&rtS, &rtE);
	}
*/

	while (m_pVideoOne->bFrameStepping)
	{
		//Beep(1000,1);
		Sleep(1);
		if(m_pVideoOne->bDeliverFrameStepSample)
		{
			//pSample->SetTime(NULL, NULL);
			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Framestep sample.")));
			m_pVideoOne->bDeliverFrameStepSample = false;

			LONGLONG rtStart = 0, rtStop = _I64_MAX;
			hr = pOut->SetTime(&rtStart, &rtStop);

			//Beep(100,1);
			hr = m_pOutput->m_pInputPin->Receive(pOut);
			m_bSampleSkipped = FALSE;	// last thing no longer dropped
			pOut->Release();
			return S_OK;
		}
	}
	

	//if ((bDoingYUVFile == true) && ((int)YUVFile != 0xcdcdcdcd))
	//{
	//	LPBYTE dst = NULL;
	//	LPBYTE src = YUVFile;
	//	pOut->GetPointer(&dst);
	//	for (int i = 0; i < lConnectedHeight_In; i++)
	//	{
	//		memcpy(dst, src, 1440);
	//		src += 1440;
	//		dst += 1440;
	//	}
	//}


	////DEBUGGING
	//IMediaSample2 * IMS2 = NULL;
	//if (!FAILED(pOutSample->QueryInterface(IID_IMediaSample2, (void**) &IMS2)))
	//{
	//	AM_SAMPLE2_PROPERTIES SampProps;
	//	if (!FAILED(IMS2->GetProperties(sizeof(AM_SAMPLE2_PROPERTIES), (BYTE*)&SampProps)))
	//	{
	//		LPBYTE dst = NULL;
	//		pOutSample->GetPointer(&dst);
	//		SaveBufferToFile("C:\\Temp\\Dump.bin", "w", dst, 720 * 486 * 2, 720, 486);

	//        AM_SAMPLE2_PROPERTIES OutProps;
	//		IMS2->GetProperties(FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, tStart), (PBYTE)&OutProps);
	//		OutProps.dwTypeSpecificFlags = 917512;
	//		IMS2->SetProperties(FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, dwStreamId), (PBYTE)&OutProps);
	//		IMS2->GetProperties(sizeof(AM_SAMPLE2_PROPERTIES), (BYTE*)&SampProps);
	//		int i = 0;
	//	}
	//}
	//IMS2->Release();
	////DEBUGGING



	//set timestapms
	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;
	//pOut->GetTime(&rtS, &rtE);
	//CRefTime crtST;
	//StreamTime(crtST);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: CurrentStreamTime: %I64d"), crtST.m_time));
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: SampleStart/SampleEnd: %I64d / %I64d"), rtS, rtE));
	////rtS = 0;
	////rtE = 0;
	//pOut->SetTime(&rtS, &rtE);



	//pOut->GetTime(&rtS, &rtE);
	//
 //   CRefTime crtST;
	//StreamTime(crtST);
	//int LE_CST = crtST.m_time - rtS;

	//if (rtS != 0)
	//{
	//	int i = 1;
	//}

	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: LE_CST: %d"), LE_CST));
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: CST/MTST: %I64d %I64d"), crtST.m_time, rtS));
	////-183406000			 -48636000
	//if ((LE_CST > 10000000 && LE_CST < 500000000) || (LE_CST < -10000000 && LE_CST > -400000000))
	//{
	//	//The difference between the current stream time and the last end has grown too large
	//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: sample time adjusted.")));
	//	rtS = crtST.m_time + 6000;
	//	rtE = rtS + 417083;
	//	pOut->SetTime(&rtS, &rtE);
	//}


	//IMAGE RECORD
	if (bRecordingImages == true)
	{
		LPBYTE dst = NULL;
		pOut->GetPointer(&dst);

		CRefTime crtST;
		StreamTime(crtST);

		TCHAR buff[256];
	
		USES_CONVERSION;
		_stprintf(buff, _T("%s%d.bin"), W2A(ImageRecordTargPath), crtST.Millisecs());

		LONG TargSize = 6220800;
		LPBYTE pRGB = (LPBYTE)malloc(TargSize);
		CSCManual::UYVYToRGB24(dst, 1920, 1080, pRGB);
		SaveBufferToFile_A(buff, "wb", pRGB, TargSize);
		free(pRGB);
		Beep(1000,1);
	}


	//Beep(1000,1);
	//SetFullBufferBGColorYUY2(dst, 1440, 720, 486);
	//TellPhoenixToGetFrameGrab(pOutSample, 720, 480, 0);

	////FG OF LAST FRAME OUTPUT
	//LPBYTE dst = NULL;
	//pOutSample->GetPointer(&dst);
	//CopyVidRect(dst, LastFrameOutput, 1440, 720, lConnectedHeight_Out);

#ifdef DEBUG
	//Beep(1000,1);
#endif

	REFERENCE_TIME rtStart = m_iFrame++;
	rtStart *= 10010010;
	rtStart /= 24;
	REFERENCE_TIME rtEnd = m_iFrame;
	rtEnd *= 10010010;
	rtEnd /= 24;

	pOut->SetTime(&rtStart, &rtEnd);
	hr = m_pOutput->m_pInputPin->Receive(pOut);
	m_bSampleSkipped = FALSE;	// last thing no longer dropped
	pOut->Release();
	return S_OK;


//// S_FALSE returned from Transform is a PRIVATE agreement
//// We should return NOERROR from Receive() in this cause because returning S_FALSE
//// from Receive() means that this is the end of the stream and no more data should
//// be sent.
//if (S_FALSE == hr) {
//
//    //  Release the sample before calling notify to avoid
//    //  deadlocks if the sample holds a lock on the system
//    //  such as DirectDraw buffers do
//    pOutSample->Release();
//    m_bSampleSkipped = TRUE;
//    if (!m_bQualityChanged) {
//        NotifyEvent(EC_QUALITY_CHANGE,0,0);
//        m_bQualityChanged = TRUE;
//    }
//    return NOERROR;
//}
//}

    // release the output buffer. If the connected pin still needs it, it will have addrefed it itself.
 //   pOutSample->Release();

	//return S_OK;
}

HRESULT CKeystone::ResendSample()
{
	IMediaSample * pOutSample;

	//Video
	HRESULT hr = m_pOutput->m_pAllocator->GetBuffer(&pOutSample, NULL, NULL, 0);
    if (FAILED(hr)) return hr;
	LPBYTE dst = NULL;
	pOutSample->GetPointer(&dst);
	CopyVidRect(LastFrameOutput, dst, 1440, 720, lConnectedHeight_Out);

	//Time stamps
	REFERENCE_TIME rtStart, rtEnd;
	CRefTime crtStreamTime;
	StreamTime(crtStreamTime);
	rtStart = crtStreamTime.m_time; //+ 100000;
	rtEnd = rtStart + llTargetFR_Out_ATPF;
	pOutSample->SetTime(&rtStart, &rtEnd);

	//Deliver
	hr = m_pOutput->m_pInputPin->Receive(pOutSample);
    pOutSample->Release();
    if (FAILED(hr)) return hr;

	return S_OK;
}

// Set up our output sample
HRESULT CKeystone::InitializeOutputSample(IMediaSample *pSample, IMediaSample **ppOutSample)
{
    IMediaSample *pOutSample;

    // default - times are the same

    AM_SAMPLE2_PROPERTIES * const pProps = m_pVideoOne->SampleProps();
    DWORD dwFlags = m_bSampleSkipped ? AM_GBF_PREVFRAMESKIPPED : 0;

    // This will prevent the image renderer from switching us to DirectDraw
    // when we can't do it without skipping frames because we're not on a
    // keyframe.  If it really has to switch us, it still will, but then we
    // will have to wait for the next keyframe
    if (!(pProps->dwSampleFlags & AM_SAMPLE_SPLICEPOINT)) 
	{
		dwFlags |= AM_GBF_NOTASYNCPOINT;
    }

    ASSERT(m_pOutput->m_pAllocator != NULL);
    HRESULT hr = m_pOutput->m_pAllocator->GetBuffer(
             &pOutSample, 
			 pProps->dwSampleFlags & AM_SAMPLE_TIMEVALID ? &pProps->tStart : NULL, 
			 pProps->dwSampleFlags & AM_SAMPLE_STOPVALID ? &pProps->tStop : NULL, 
			 dwFlags
         );
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
            pOutSample->SetTime(&pProps->tStart,
                                &pProps->tStop);
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
	LONGLONG MediaStart, MediaEnd;
    if (pSample->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR) 
	{
        pOutSample->SetMediaTime(&MediaStart,&MediaEnd);
    }

	//Copy the time stamps
	REFERENCE_TIME rtS = 0;
	REFERENCE_TIME rtE = 0;
	pSample->GetTime(&rtS, &rtE);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: rtS/rtE: %I64d %I64d"), rtS, rtE));
	pOutSample->SetTime(&rtS, &rtE);

	return S_OK;
}