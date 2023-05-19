#include "../Utility/appincludes.h"
#include "../../VideoProcessing/Mixer/Mixer.h"

//HRESULT CKeystone::ReceiveVideo(IMediaSample * pIn)
//{
//	//Beep(1000,1);
//	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ReceiveVideo")));
//    IMediaSample * pOut = NULL;
//    HRESULT hr = InitializeOutputSample(pIn, &pOut);
//    if (FAILED(hr)) {
//        return hr;
//    }
//
//	LPBYTE src = NULL;
//	pIn->GetPointer(&src);
//
//    return MixAndDeliverVideo(src, pOut);
//}

HRESULT CKeystone::ReceiveSubpicture(IMediaSample * pSample)
{
	//DEBUGGING
	//Beep(1000, 1);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ReceiveSubpicture")));
	//TellPhoenixToGetFrameGrab(pSample, 720, 480, 0);
	//DEBUGGING

	if ((SPMixData.pImage) && ((int)SPMixData.pImage != 0xcdcdcdcd)) 	free(SPMixData.pImage);
	CMediaType *mSource = 0;
	VIDEOINFOHEADER2 * VIH2;
	if (S_OK == pSample->GetMediaType((AM_MEDIA_TYPE**)&mSource) && mSource)
	{
		VIH2 = (VIDEOINFOHEADER2*) mSource->Format();
		SPMixData.nH = VIH2->rcTarget.bottom;
		SPMixData.nW = VIH2->rcTarget.right;
		SPMixData.nX = VIH2->rcTarget.left;
		SPMixData.nY = VIH2->rcTarget.top;
		mxSub_W = VIH2->rcTarget.right;
		mxSub_H = VIH2->rcTarget.bottom;
		mxSub_X = VIH2->rcTarget.left;
		mxSub_Y	= VIH2->rcTarget.top;
	}
	else
	{
		SPMixData.nH = mxSub_H;
		SPMixData.nW = mxSub_W;
		SPMixData.nX = mxSub_X;
		SPMixData.nY = mxSub_Y;
	}

	//REFERENCE_TIME pEnd = 0;
	//pSample->GetTime(&rtCurrentSPStart, &pEnd);

	SPMixData.nImage_Format = CKeystone::IF_ARGB4444;

	pSample->GetPointer(&TempSPBuffer);

	//DEBUGGING
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Sub address: %d"), &TempSPBuffer));
	//SaveBufferToFile("C:\\Temp\\Dump.bin", "w", TempSPBuffer, SPMixData.nW * SPMixData.nH * 2, SPMixData.nW, SPMixData.nH);
	//DEBUGGING

	//SPMixData.pImage = (LPBYTE)malloc(SPMixData.nW * SPMixData.nH * 2);
	//CSCManual::ARGB4444ToUYVY(TempSPBuffer, SPMixData.nW, SPMixData.nH, SPMixData.pImage);
	//CopyVidRect(TempSPBuffer, SPMixData.pImage, 1440, 720, 480);

	//if ((int)SPMixData.pImage == 0xcdcdcdcd)
	//{
	//	SPMixData.pImage = (LPBYTE)malloc(SPMixData.nW * SPMixData.nH * 3);
	//}
	//Is this line a memory leak if I never delete the created buffer?
	SPMixData.pImage = (LPBYTE)malloc(SPMixData.nW * SPMixData.nH * 3);
	CSCManual::ARGB4444ToAAUYVY(TempSPBuffer, SPMixData.nW, SPMixData.nH, SPMixData.pImage);

	//bPendingSPMix = true;
	return S_OK;
}

HRESULT CKeystone::ReceiveLine21(IMediaSample * pSample)
{
	//DEBUGGING
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ReceiveLine21")));
	
	//CMediaType *mSource = 0;
	//VIDEOINFOHEADER *VIHL21 = NULL;
	//if (S_OK == pSample->GetMediaType((AM_MEDIA_TYPE**)&mSource) && mSource)
	//{
	//	VIHL21 = (VIDEOINFOHEADER*)mSource->Format();
	//}

	//Beep(1000,1);

	//DEBUGGING

	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: L21 - 1")));

	if ((L21MixData.pImage) && ((int)L21MixData.pImage != 0xcdcdcdcd)) 	free(L21MixData.pImage);

	L21MixData.nH = 640;
	L21MixData.nW = 480;
	L21MixData.nImage_Format = CKeystone::IF_RGB8;
	L21MixData.nX = 0;
	L21MixData.nY = 0;

    //Get palette
	CMediaType *mtL21 = 0;
	if (S_OK != pSample->GetMediaType((AM_MEDIA_TYPE**)&mtL21) && mtL21) 
	{
		return E_FAIL;
	}

	if (mtL21 == 0x00000000) 
	{
		//pSample->GetPointer(&TempL21Buffer);
		//L21MixData.pImage = NULL;
		//return S_OK;
	}
	else
	{
		VIDEOINFO * VI = (VIDEOINFO*) mtL21->Format();
		L21MixData.Palette = COLORS(VI);
	}

	if (((unsigned int)L21MixData.Palette == 0xcdcdcdcd) || ((unsigned int)L21MixData.Palette == 0))
	{
		return S_OK;
	}

	pSample->GetPointer(&TempL21Buffer);
    L21MixData.pImage = (LPBYTE)malloc(614400);
	HRESULT hr = CSCManual::RGB8ToUYVY(TempL21Buffer, 640, 480, L21MixData.pImage, (DWORD*) L21MixData.Palette);

	//bPendingL21Mix = true;
	return S_OK;
}

HRESULT CKeystone::MixAndDeliverVideo(IMediaSample* pOut, int w, int h)
{
    //DEBUGGING
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: MixAndDeliverVideo")));
	//DEBUGGING

    if (bFilterIsLocked == true) return E_FAIL;
	HRESULT hr = S_OK;

	if (h == 480) h = 486;

	LPBYTE dst = NULL;
	pOut->GetPointer(&dst);

	hr = MixSubpicture(dst, w, h);
	if (FAILED(hr)) return hr;

	//Sample grabber - Video And Subpicture
	if ((bGetSample == true) && (SampleWhat == 4))
	{
		bGetSample = false;
		HandleSampleGrab(pOut);
	}

	hr = MixOnScreenDisplay(dst, w, h);
	if (FAILED(hr)) return hr;

	hr = MixLine21(dst, w, h);
	if (FAILED(hr)) return hr;

	hr = MixGuides(dst, w, h);
	if (FAILED(hr)) return hr;

	hr = MixGOPTimecode(dst, pOut, w, h);
	if (FAILED(hr)) return hr;

	//Get on with out sample initialization
	pOut->SetActualDataLength(w * h * 2);
	pOut->SetSyncPoint(TRUE);
	pOut->SetDiscontinuity(FALSE);

	VIDEOINFOHEADER2 * v2 = (VIDEOINFOHEADER2*)m_pVideo->CurrentMediaType().Format();
	if (v2->bmiHeader.biCompression == 1498831189) goto gtDeliverMixedSample; //If we're connected to NullVideoSource

	//NEEDED FOR SYSTEM JACKET PICTURE ONLY
	DWORD dwCurrentTicks = GetTickCount();
	int iTDiff = abs(short(dwLastSampleProcessedAt - dwCurrentTicks));
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Gap in samples: %d"), iTDiff));
	if (iTDiff > 82)  //250= 1/4sec - this has to be short enough to catch quick button changes by user.
	{ 
		//Beep(1000, 1);
		REFERENCE_TIME rtStart, rtEnd;
		StreamTime(crtStreamTime);
		rtStart = crtStreamTime.m_time + 100000;
		rtEnd = rtStart + llTargetFR_Out_ATPF;
		pOut->SetTime(&rtStart, &rtEnd);
		//pOut->SetTime(NULL, NULL);
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: LARGE gap in samples detected. %d"), iTDiff));
	}
	dwLastSampleProcessedAt = GetTickCount();


	//opFrameCnt += 1;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OP Frame Count: %d"), opFrameCnt));
	////HandleSampleGrab(pOut);

gtDeliverMixedSample:
	return DeliverMixedSample(pOut);
}

HRESULT CKeystone::MixSubpicture(LPBYTE dst, int w, int h)
{
	return S_OK;
}

HRESULT CKeystone::MixOnScreenDisplay(LPBYTE dst, int w, int h)
{
	HRESULT hr = S_OK;
	if (bPendingOSDMix != true) goto SkipOSDMix;
	if ((int)OSDMixData.pImage == 0xcdcdcdcd) goto SkipOSDMix;
	if (OSDMixData.pImage)
	{
		if (OSDTicker < OSDTickGoal)
		{
			hr = CMixer::MixUYVY_OSD(OSDMixData.pImage, OSDMixData.nW, OSDMixData.nH, dst, w, h, OSDMixData.nX, OSDMixData.nY, OSDMixData.KeyColor); 
			if (FAILED(hr)) return hr;
			OSDTicker += 1;
			//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OSD Ticker. %d"), OSDTicker));
		}
		else
		{
			//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OSD Ticker Maxedout")));
			bPendingOSDMix = false;
		}
	}
SkipOSDMix:
	return hr;
}

HRESULT CKeystone::MixLine21(LPBYTE dst, int w, int h)
{
	return S_OK;
}

HRESULT CKeystone::MixGuides(LPBYTE dst, int w, int h)
{
	HRESULT hr = S_OK;
	if (bMixGuides == true)
	{
		hr = CMixer::MixUYVY_Guides(dst, w, h, nGuide_L, nGuide_T, nGuide_R, nGuide_B, dwGuideColor);
		if (FAILED(hr)) return hr;
	}

	if (iShowActionTitleGuides == 1)
	{
		hr = CMixer::MixUYVY_ActionTitleGuides(dst, w, h, dwActionTitleSafeColor);
		if (FAILED(hr)) return hr;
	}
	return hr;
}

HRESULT CKeystone::MixGOPTimecode(LPBYTE dst, IMediaSample * pOut, int w, int h)
{
	HRESULT hr = S_OK;
	//GET GOP TIMECODE
	LONGLONG TStart = 0;
	LONGLONG TEnd = 0;
    pOut->GetMediaTime(&TStart, &TEnd);
	if ((TStart>0) || (TEnd>0)) 
	{
		////DEBUGGING
		//int h, m, s, f;
		//RTToHMSF(TStart, 29.97, h, m, s, f);
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: SH GOP TC: %d:%d:%d;%d"), h, m, s, f));
		////DEBUGGING
		
		if (TStart == LastNewTStart)
		{
			//Increment it
			if ((llTargetFR_In_ATPF == 333667) || (llTargetFR_In_ATPF == 333666) || (llTargetFR_In_ATPF == 417084) || (llTargetFR_In_ATPF == 417083))
			{
				TStartIncr += 333667;
			}
			else
			{
				TStartIncr += llTargetFR_In_ATPF;
			}
			TStart += TStartIncr;
		}
		else
		{
			//New MPEG GOP TC
			LastNewTStart = TStart;
			TStartIncr = 0;
		}

		int FPS = 0;
		double dFPS = 0;
		if ((llTargetFR_In_ATPF == 333667) || (llTargetFR_In_ATPF == 333666) || (llTargetFR_In_ATPF == 417084) || (llTargetFR_In_ATPF == 417083))
		{
			FPS = 2997;
			dFPS = 29.97;
		}
		else if (llTargetFR_In_ATPF == 400000)
		{
			FPS = 25;
			dFPS = 25;
		}

		int h, m, s, f;
		RTToHMSF(TStart, dFPS, h, m, s, f);

		LONG_PTR out = (h<<24) | (m<<16) | (s<<8) | f;

		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: GOP TC: %d - %d"), TStart, TEnd));
		NotifyEvent(EC_KEYSTONE_MPEGTC, out, (LONG_PTR)FPS);
	}
	else
	{
		goto SkipGOPTCMix;
	}

	//Mix GOP Timecodes
	if (iMixGOPTC > 0)
	{
		double dOut = 0;
		if ((llTargetFR_In_ATPF == 333667) || (llTargetFR_In_ATPF == 333666) || (llTargetFR_In_ATPF == 417084) || (llTargetFR_In_ATPF == 417083))
		{
			dOut = 29.97;
		}
		else if (llTargetFR_In_ATPF == 400000)
		{
			dOut = 25;
		}

		//Create bmp of ref time
		if ((TStartIncr == 0) && (iMixGOPTC_IFrameRed == 1))
		{
			//red
			RTtoBMP(m_hDC, m_hBM, m_hFONT, (REFERENCE_TIME)TStart, dOut, 0x0000ff, 0x000000);
		}
		else
		{
			//green
			RTtoBMP(m_hDC, m_hBM, m_hFONT, (REFERENCE_TIME)TStart, dOut, 0x00ff00, 0x000000);		
		}

		//Get a bitmap from the hbitmap
		BITMAP bm;
		GetObject(m_hBM, sizeof(bm), &bm);

		//CSC
		LPBYTE GOPTC_UYVY = (LPBYTE)malloc(bm.bmHeight * bm.bmWidth * 2); //maybe needed	
		CSCManual::RGB32ToUYVY((LPBYTE)bm.bmBits, bm.bmWidthBytes, bm.bmWidth, bm.bmHeight, GOPTC_UYVY);

		//Mix
		CMixer::MixUYVY_OSD(GOPTC_UYVY, 100, 16, dst, w, h, 540, 30, 0x00000000);
		free(GOPTC_UYVY);
	}
SkipGOPTCMix:
	return hr;
}

HRESULT CKeystone::DeliverMixedSample(IMediaSample * pOutSample)
{
	//DEBUGGING
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: DeliverMixedSample")));
	//DEBUGGING

	HRESULT hr = S_OK;

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
	//	pOutSample->SetTime(&m_pVideo->rtLast32Sample_StartTime, &m_pVideo->rtLast32Sample_EndTime);
	//	DbgLog((LOG_TRACE, 0, TEXT("Presentation Times - Start: %I64d  End: %I64d"), m_pVideo->rtLast32Sample_StartTime, m_pVideo->rtLast32Sample_EndTime));

	//	//DbgLog((LOG_TRACE, 0, TEXT("Media Times - Start: %d  End: %d"), pMedTime_Start, pMedTime_End));
	//	//NotifyEvent(EC_KEYSTONE_SAMPLETIMES, m_pVideo->rtLast32Sample_StartTime, m_pVideo->rtLast32Sample_EndTime);

	//	m_pVideo->rtLast32Sample_StartTime += 333667;
	//	m_pVideo->rtLast32Sample_EndTime += 333667;
	//}

	//if (m_pVideo->bRun32)
	//{
	//	pOutSample->SetTime(&m_pVideo->rtLast32Sample_StartTime, &m_pVideo->rtLast32Sample_EndTime);

	//	//DbgLog((LOG_TRACE,0,TEXT("Media Times - Start: %d  End: %d"), pMedTime_Start, pMedTime_End));
	//	//NotifyEvent(EC_KEYSTONE_SAMPLETIMES, m_pVideo->rtLast32Sample_StartTime, m_pVideo->rtLast32Sample_EndTime);

	//	m_pVideo->rtLast32Sample_StartTime += 333667;
	//	m_pVideo->rtLast32Sample_EndTime += 333667;
	//}

	//DEBUGGING
	//pOutSample->SetTime(NULL, NULL);
	//DEBUGGING

	//DEBUGGING
	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;
	//pOutSample->SetTime(&rtS, &rtE);
	//DEBUGGING

	////DEBUGGING
	//LPBYTE src = NULL;
	//pOutSample->GetPointer(&src);
	//SaveBufferToFile("C:\\Temp\\Dump3.bin", "w", src, 691200, 720, 480);
	////DEBUGGING

	//PERFORMANCE
	//Stop the clock and log it (if PERF is defined)
	MSR_STOP(m_idTransform);
	
	////m_pClock->GetTime(&m_pVideo->rtSampleDelivered);
	////REFERENCE_TIME rtMSElapsed = (m_pVideo->rtSampleDelivered - m_pVideo->rtSampleReceived)/100000000;
	////StreamTime(m_pVideo->crtSampleDelivered);
	////REFERENCE_TIME rtMSElapsed = (m_pVideo->crtSampleDelivered.m_time - m_pVideo->crtSampleReceived.m_time);
	//m_pVideo->dwSampleDelivered = GetTickCount();
	//REFERENCE_TIME rtMSElapsed = (m_pVideo->dwSampleDelivered - m_pVideo->dwSampleReceived);
	//LONGLONG rtReceived = 0;
	//LONGLONG rtReceiveda = 0;
	//pOutSample->GetMediaTime(&rtReceived, &rtReceiveda);

	//m_pVideo->dwSampleDelivered = GetTickCount();
	//LONG a = 0;
	//a = pOutSample->GetActualDataLength();
	//REFERENCE_TIME rtMSElapsed = (m_pVideo->dwSampleDelivered - a);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Sample processing time: %I64d"), rtMSElapsed));

	//lSampleCount++;
	//lSumOfProcessingTimes += rtMSElapsed;
	//int Avg = lSumOfProcessingTimes/lSampleCount;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Average sample processing time: %d"), Avg));

	//PERFORMANCE

	////Check to see if samples are running late and set times as needed
	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;

	//pOutSample->GetTime(&rtS, &rtE);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: TIMES %I64d %I64d"), rtS, rtE));

	//StreamTime(crtStreamTime);
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: CRT %I64d"), m_pVideo->crtSampleDelivered));
	//
	//REFERENCE_TIME rtDif = rtS - crtStreamTime;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: DIF %I64d"), rtDif));

	//if (rtDif < 0)
	//{
	//	//Sample is late
	//	Beep(1000,5);
	//	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: DIF %I64d"), rtDif));
	//	rtS = crtStreamTime; // + 100000;
	//	rtE = rtS + llTargetFR_Out_ATPF;
	//	pOutSample->SetTime(&rtS, &rtE);
	//}



	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;
	//pOutSample->GetTime(&rtS, &rtE);
	////DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: TIMES %I64d %I64d"), rtS, rtE));
	//StreamTime(crtStreamTime);
	//rtS = crtStreamTime + 250000;
	//rtE = rtS + 333667;
	//pOutSample->SetTime(&rtS, &rtE);




	//while (m_pVideo->bFrameStepping)
	//{
	//	//Beep(1000,1);
	//	Sleep(1);
	//	if(m_pVideo->bDeliverFrameStepSample)
	//	{
	//		//pSample->SetTime(NULL, NULL);
	//		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Framestep sample.")));
	//		m_pVideo->bDeliverFrameStepSample = false;

	//		LONGLONG rtStart = 0, rtStop = _I64_MAX;
	//		hr = pOutSample->SetTime(&rtStart, &rtStop);

	//		//Beep(100,1);
	//		hr = m_pOutput->m_pInputPin->Receive(pOutSample);
	//		m_bSampleSkipped = FALSE;	// last thing no longer dropped
	//		pOutSample->Release();
	//		return S_OK;
	//	}
	//}
	

	if ((bDoingYUVFile == true) && ((int)YUVFile != 0xcdcdcdcd))
	{
		LPBYTE dst = NULL;
		LPBYTE src = YUVFile;
		pOutSample->GetPointer(&dst);
		for (int i = 0; i < lConnectedHeight_In; i++)
		{
			memcpy(dst, src, 1440);
			src += 1440;
			dst += 1440;
		}
	}


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

	////DEBUGGING
	//TCHAR buff[256];
	//DWORD dwCurrentTicks = GetTickCount();
	//_stprintf(buff, _T("C:\\Temp\\Dump_NormalDelivery_UYVY_%d.bin"), dwCurrentTicks);
	//SaveBufferToFile(buff, "w", dst, 720 * 486 * 2, 720, 486);
	////DEBUGGING

	//Beep(1000,1);
	//SetFullBufferBGColorYUY2(dst, 1440, 720, 486);
	//TellPhoenixToGetFrameGrab(pOutSample, 720, 480, 0);

	//FG OF LAST FRAME OUTPUT
	LPBYTE dst = NULL;
	pOutSample->GetPointer(&dst);
	CopyVidRect(dst, LastFrameOutput, 1440, 720, lConnectedHeight_Out);

	////DEBUGGING
	//CRefTime crtST;
	//REFERENCE_TIME rtS = 0;
	//REFERENCE_TIME rtE = 0;
	//StreamTime(crtST);
	//pOutSample->GetTime(&rtS, &rtE);
	//DbgLog((LOG_TRACE, 0, TEXT("	          KEYSTONE ST: %I64d"), crtST.m_time));
	//DbgLog((LOG_TRACE, 0, TEXT("	       KEYSTONE TIMES: %I64d -- %I64d"), rtS, rtE));
	//REFERENCE_TIME rtDif = crtST - rtS;
	//DbgLog((LOG_TRACE, 0, TEXT("		     KEYSTONE DIF: %I64d"), rtDif));
	////pOutSample->SetTime(NULL, NULL);
	////pOutSample->SetTime(0, 0);
	////DEBUGGING

	//FRAME DROP DETECTION
	CRefTime crtST;
	REFERENCE_TIME rtS = 0;
	REFERENCE_TIME rtE = 0;
	pOutSample->GetTime(&rtS, &rtE);
	StreamTime(crtST);
	
	//Difference between current stream time and the sample's start time.
	REFERENCE_TIME StartTime_StreamTime_Delta = rtS - crtST.m_time;
	if (StartTime_StreamTime_Delta < 0) //Should be a positive delta. This means we have time to burn.
	{
		//We're late
		//Beep(1000,1);
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: WE ARE LATE!!! StartDelta= %I64d"), StartTime_StreamTime_Delta));
		NotifyEvent(EC_KEYSTONE_FRAMEDROPPED, StartTime_StreamTime_Delta, 0);
		pOutSample->Release();
		return S_OK;
	}
	//END FRAME DROP DETECTION

	//DEBUGGING
	//pOutSample->SetTime(NULL, NULL);
	//DEBUGGING

	//LogSampleMetadata(pOutSample);
	//Beep(1000, 1);

	if (bSetNULLTimestamps == true)
	{
		//pOutSample->SetTime(NULL, NULL);
	}

	hr = m_pOutput->m_pInputPin->Receive(pOutSample);
	m_bSampleSkipped = FALSE;	// last thing no longer dropped
    pOutSample->Release();

	//Beep(1000, 1);

	NotifyEvent(EC_KEYSTONE_FRAMEDELIVERED, GetTickCount(), 0);
	
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

HRESULT CKeystone::ReSizeMixSendSample(int Unconditional)
{
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ReSizeMixSendSample()")));

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
	if (((int)LastFrameReceived == 0xcdcdcdcd) || ((int)LastFrameReceived == 0))
	{
		//Beep(1000, 1);
		return S_FALSE;
	}

	HRESULT hr;
	
	//GET CBASERENDERER
	//IPin * pRendererPin = NULL;
	//hr = m_pOutput->ConnectedTo(&pRendererPin);
	//if (FAILED(hr)) return hr;
	//PIN_INFO pPI;
	//hr = pRendererPin->QueryPinInfo(&pPI);
	//if (FAILED(hr)) return hr;
	//IBaseFilter * pDL = pPI.pFilter;
	//CBaseRenderer * pRen = (CBaseRenderer*) ((ULONG)pDL -12);

	//OLD WAY - DOES A SEARCH FOR THE RENDERER - DO NOT DELETE - THIS IS A LEGITIMATE WAY TO DO THIS (COMPARED WITH THE OTHER TWO)
	//IBaseFilter * pDL = NULL;
	//hr = m_pGraph->FindFilterByName(L"DeckLink Video Renderer", &pDL);
	//
	//if (FAILED(hr)) //probably hr == VFW_E_NOT_FOUND
	//{
	//	//See if we're using the VMR9
	//	hr = m_pGraph->FindFilterByName(L"Video Mixing Renderer 9", &pDL);		
	//	if (FAILED(hr)) return hr;
	//}
	//CBaseRenderer * pRen = (CBaseRenderer*) ((ULONG)pDL -12);

	//OLD WAY - RELIED ON A MODIFICATION TO THE BASE CLASSES. MOVE m_pRenderer to Public in CRendererInputPin
	//CRendererInputPin * pRenIn = NULL;
	//pRenIn = (CRendererInputPin*) m_pOutput->m_pInputPin;
	//HRESULT ReceiveCanBlock = pRenIn->ReceiveCanBlock();
	//CBaseRenderer * pRen = NULL;
	//pRen = pRenIn->m_pRenderer; //Had to move m_pRenderer to "Public" in the base class to facilitate this.

	BOOL IsStreaming = m_pOutput->m_pRenderer->IsStreaming();
	BOOL IsEOS = m_pOutput->m_pRenderer->IsEndOfStream();
	BOOL IsReady = m_pOutput->m_pRenderer->CheckReady();
	BOOL HasSample = m_pOutput->m_pRenderer->HaveCurrentSample();

	if ((!IsStreaming) || (IsEOS) || (!IsReady) || (HasSample))
	{
		//Beep(1000,1);
		return S_FALSE;
	}

	//CComPtr<IMediaSample> pSample;
	IMediaSample * pSample = NULL;
    hr = InitializeOutputSample(NULL, &pSample);
    if (FAILED(hr)) return hr;

	LPBYTE dst = NULL;
	hr = pSample->GetPointer(&dst);

	//DIMENSIONS
	int w = 720;
	int h = lConnectedHeight_In;

	//RESIZE
	CopyVidRect(LastFrameReceived, LastFrameReceived_Resized, 1440, w, h);
	m_pVideo->Resize(LastFrameReceived_Resized);

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
		CopyYUY2toUYVYRect(LastFrameReceived_Resized, 0, 0, 1440, dst, 0, 3, 1440, w, h);
	}
	else
	{
		CopyYUY2toUYVYRect(LastFrameReceived_Resized, 0, 0, 1440, dst, 0, 0, 1440, w, h);
	}

	//NTSC - ADJUST TO OUTPUT SIZE
	if (h == 480) h = 486;
 
	//MIXING
	hr = MixSubpicture(dst, w, h);
	if (FAILED(hr)) return hr;

	hr = MixGuides(dst, w, h);
	if (FAILED(hr)) return hr;

	//DEBUGGING
	//BurnInTicks(dst, w, h);
	//DEBUGGING

	pSample->SetTime(NULL, NULL);

	//CONFIRM THAT A FRAME HAS NOT BEEN RECEIVED SINCE WE DETECTED A DISCONTINUITY
	DWORD TicksDelta = GetTickCount() - m_pVideo->LastFrameReceived_Ticks;
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: DELTA %I32d"), TicksDelta));
	if ((TicksDelta < 500) && (Unconditional == 0))
	{
		//Beep(1000,1);
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Bailing out of ReSizeMixSendSample()")));
		pSample->Release();
		return S_OK; //Bail out, a new frame arrived	
	}

	//DEBUGGING
	REFERENCE_TIME rtS = 0;
	REFERENCE_TIME rtE = 0;
	pSample->GetTime(&rtS, &rtE);
	DbgLog((LOG_TRACE, 0, TEXT("		KEYSTONE TIMES: (RS) %I64d -- %I64d"), rtS, rtE));
	//DEBUGGING

	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ResendLastSample Receive()")));
	hr = m_pOutput->m_pInputPin->Receive(pSample);
	pSample->Release();
	if (FAILED(hr))
	{
		////DEBUGGING
		//CRefTime crtST;
		//StreamTime(crtST);

		//TCHAR buff[256];
		//_stprintf(buff, _T("C:\\Temp\\Testing\\%I32d_%I64d_%I32d.bin"), hr, crtST.Millisecs(), (int)dst);

		//LONG TargSize = 1049760;
		//LPBYTE pRGB = (LPBYTE)malloc(TargSize);
		//CSCManual::YUY2ToRGB24(LastFrameReceived, 720, 486, pRGB);
		//SaveBufferToFile_A(buff, "wb", pRGB, TargSize);
		//free(pRGB);
		////DEBUGGING
	}
	else
	{
		////DEBUGGING
		//CRefTime crtST;
		//StreamTime(crtST);

		//TCHAR buff[256];
		//_stprintf(buff, _T("C:\\Temp\\Testing\\%I64d_%I32d.bin"), crtST.Millisecs(), (int)dst);

		//LONG TargSize = 1049760;
		//LPBYTE pRGB = (LPBYTE)malloc(TargSize);
		//CSCManual::YUY2ToRGB24(LastFrameReceived, 720, 486, pRGB);
		//SaveBufferToFile_A(buff, "wb", pRGB, TargSize);
		//free(pRGB);
		////DEBUGGING
	}
	//return S_OK;
	return hr;
}

HRESULT CKeystone::InitializeOutputSample(IMediaSample *pSample, IMediaSample **ppOutSample)
{
    IMediaSample *pOutSample;
	//CComPtr<IMediaSample> pOutSample = NULL;

    // default - times are the same

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
	if (pSample != NULL)
	{
		LONGLONG MediaStart, MediaEnd;
		if (pSample->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR) 
		{
			pOutSample->SetMediaTime(&MediaStart,&MediaEnd);
		}
	}

		//if (pSample->GetActualDataLength() == 691200)
	//{
	//	pOutSample->SetActualDataLength(699840);
	//}
	//else
	//{
	//	pOutSample->SetActualDataLength(829440);
	//}

	//if (pSample->IsSyncPoint() == S_OK)
	//{
	//	pOutSample->SetSyncPoint(TRUE);
	//}
	//else
	//{
	//	pOutSample->SetSyncPoint(FALSE);
	//}

	//if (pSample->IsDiscontinuity() == S_OK)
	//{
	//	pOutSample->SetDiscontinuity(TRUE);
	//}
	//else
	//{
	//	pOutSample->SetDiscontinuity(FALSE);
	//}

	return S_OK;
}

HRESULT CKeystone::ResendSample()
{
	return S_OK;
}

