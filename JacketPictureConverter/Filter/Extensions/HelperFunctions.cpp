#include "../Utility/appincludes.h"

void CKeystone::LogSampleMetadata(IMediaSample * pSample)
{
	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG ===========================================================================================")));

	CMediaType *mSource = 0;
    if (S_OK == pSample->GetMediaType((AM_MEDIA_TYPE**)&mSource) && mSource)
	{
		//AM_MEDIA_TYPE
		DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG MT_majortype=%s"), (LPCTSTR)CDisp(mSource->majortype)));
		DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG MT_subtype=%s"), (LPCTSTR)CDisp(mSource->subtype)));
		DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG MT_formattype=%s"), (LPCTSTR)CDisp(mSource->formattype)));
		DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG MT_FixedSizeSamples=%d"), (int)mSource->bFixedSizeSamples));
		DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG MT_TemporalCompression=%d"), (int)mSource->bTemporalCompression));
		DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG MT_SampleSize=%d"), mSource->lSampleSize));
	
		if (mSource->formattype == FORMAT_VideoInfo2)
		{
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH2")));
	
			VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)mSource->pbFormat;
			//rcSource
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_SourceRect_Left=%d"), VIH2->rcSource.left));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_SourceRect_Top=%d"), VIH2->rcSource.top));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_SourceRect_Right=%d"), VIH2->rcSource.right));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_SourceRect_Bottom=%d"), VIH2->rcSource.bottom));
			//rcTarget
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_Targetect_Left=%d"), VIH2->rcTarget.left));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_TargetRect_Top=%d"), VIH2->rcTarget.top));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_TargetRect_Right=%d"), VIH2->rcTarget.right));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_TargetRect_Bottom=%d"), VIH2->rcTarget.bottom));

			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_Bitrate=%d"), VIH2->dwBitRate));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_BitErrorRate=%d"), VIH2->dwBitErrorRate));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_ATPF=%d"), VIH2->AvgTimePerFrame));

			//BITMAPINFOHEADER
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Size=%d"), VIH2->bmiHeader.biSize));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Width=%d"), VIH2->bmiHeader.biWidth));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Height=%d"), VIH2->bmiHeader.biHeight));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Planes=%d"), VIH2->bmiHeader.biPlanes));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Bitcount=%d"), VIH2->bmiHeader.biBitCount));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Compression=%d"), VIH2->bmiHeader.biCompression));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_SizeImage=%d"), VIH2->bmiHeader.biSizeImage));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_XPelsPerMeter=%d"), VIH2->bmiHeader.biXPelsPerMeter));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_YPelsPerMeter=%d"), VIH2->bmiHeader.biYPelsPerMeter));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_ClrUsed=%d"), VIH2->bmiHeader.biClrUsed));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_ClrImportant=%d"), VIH2->bmiHeader.biClrImportant));

			//VIH2 Specific
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH2_InterlaceFlags=%d"), VIH2->dwInterlaceFlags));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH2_CopyProtectFlags=%d"), VIH2->dwCopyProtectFlags));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH2_PictAspectRatioX=%d"), VIH2->dwPictAspectRatioX));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH2_PictAspectRatioY=%d"), VIH2->dwPictAspectRatioY));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH2_ControlFlags=%d"), VIH2->dwControlFlags));
		}
		else if (mSource->formattype == FORMAT_VideoInfo)
		{
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH")));

			VIDEOINFOHEADER * VIH = (VIDEOINFOHEADER*)mSource->pbFormat;
			//rcSource
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_SourceRect_Left=%d"), VIH->rcSource.left));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_SourceRect_Top=%d"), VIH->rcSource.top));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_SourceRect_Right=%d"), VIH->rcSource.right));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_SourceRect_Bottom=%d"), VIH->rcSource.bottom));
			//rcTarget
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_Targetect_Left=%d"), VIH->rcTarget.left));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_TargetRect_Top=%d"), VIH->rcTarget.top));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_TargetRect_Right=%d"), VIH->rcTarget.right));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_TargetRect_Bottom=%d"), VIH->rcTarget.bottom));

			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_Bitrate=%d"), VIH->dwBitRate));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_BitErrorRate=%d"), VIH->dwBitErrorRate));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG VIH_ATPF=%d"), VIH->AvgTimePerFrame));

			//BITMAPINFOHEADER
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Size=%d"), VIH->bmiHeader.biSize));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Width=%d"), VIH->bmiHeader.biWidth));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Height=%d"), VIH->bmiHeader.biHeight));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Planes=%d"), VIH->bmiHeader.biPlanes));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Bitcount=%d"), VIH->bmiHeader.biBitCount));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_Compression=%d"), VIH->bmiHeader.biCompression));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_SizeImage=%d"), VIH->bmiHeader.biSizeImage));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_XPelsPerMeter=%d"), VIH->bmiHeader.biXPelsPerMeter));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_YPelsPerMeter=%d"), VIH->bmiHeader.biYPelsPerMeter));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_ClrUsed=%d"), VIH->bmiHeader.biClrUsed));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG BMI_ClrImportant=%d"), VIH->bmiHeader.biClrImportant));
		}
	}
	else
	{
		DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA ***NO MEDIA TYPE IN THIS SAMPLE***")));
	}

	IMediaSample2 * IMS2 = NULL;
	if (SUCCEEDED(pSample->QueryInterface(IID_IMediaSample2, (void**) &IMS2)))
	{
		AM_SAMPLE2_PROPERTIES SampProps;
		if (FAILED(IMS2->GetProperties(sizeof(AM_SAMPLE2_PROPERTIES), (BYTE*)&SampProps)))
		{
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA ***NO SAMPLE2 PROPERTIES IN THIS SAMPLE***")));
		}
		else
		{
			//AM_SAMPLE2_PROPERTIES
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG S2P_cbData=%d"), SampProps.cbBuffer));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG S2P_Actual=%d"), SampProps.lActual));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG S2P_Start=%I64d"), SampProps.tStart));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG S2P_Stop=%I64d"), SampProps.tStop));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG S2P_StreamId=%d"), SampProps.dwStreamId));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG S2P_MediaType=%d"), SampProps.pMediaType));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG S2P_pbBuffer=%d"), SampProps.pbBuffer));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG S2P_cbBuffer=%d"), SampProps.cbBuffer));
		
			//TYPE_SPECIFIC_FLAGS
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG TSF_FieldMask=%d"), SampProps.dwTypeSpecificFlags & 0x03));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG TSF_InterlevedFrame=%d"), SampProps.dwTypeSpecificFlags & 0x00));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG TSF_Field1=%d"), SampProps.dwTypeSpecificFlags & 0x01));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG TSF_Field2=%d"), SampProps.dwTypeSpecificFlags & 0x02));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG TSF_Field1First=%d"), SampProps.dwTypeSpecificFlags & 0x04));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG TSF_Weave=%d"), SampProps.dwTypeSpecificFlags & 0x08));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG TSF_RepeatField=%d"), SampProps.dwTypeSpecificFlags & 0x40));

			//AM_SAMPLE_PROPERTY_FLAGS
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG SPF_SplicePoint=%d"), SampProps.dwSampleFlags & AM_SAMPLE_SPLICEPOINT));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG SPF_PreRoll=%d"), SampProps.dwSampleFlags & AM_SAMPLE_PREROLL));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG SPF_DataDiscontinuity=%d"), SampProps.dwSampleFlags & AM_SAMPLE_DATADISCONTINUITY));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG SPF_TypeChanged=%d"), SampProps.dwSampleFlags & AM_SAMPLE_TYPECHANGED));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG SPF_TimeValid=%d"), SampProps.dwSampleFlags & AM_SAMPLE_TIMEVALID));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG SPF_TimeDiscontinuity=%d"), SampProps.dwSampleFlags & AM_SAMPLE_TIMEDISCONTINUITY));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG SPF_FlushOnPause=%d"), SampProps.dwSampleFlags & AM_SAMPLE_FLUSH_ON_PAUSE));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG SPF_StopValid=%d"), SampProps.dwSampleFlags & AM_SAMPLE_STOPVALID));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG SPF_EndOfStream=%d"), SampProps.dwSampleFlags & AM_SAMPLE_ENDOFSTREAM));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG SPF_Media=%d"), SampProps.dwSampleFlags & AM_STREAM_MEDIA));
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA LOG SPF_Control=%d"), SampProps.dwSampleFlags & AM_STREAM_CONTROL));
		}
		
		IMS2->Release();
	}
	else
	{
		DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: SAMPLE METADATA ***NO IMediaSample2 IN THIS SAMPLE***")));
	}
}

HRESULT CKeystone::CalcActualFR_Out(int MSCount, short FrameCount)
{
	if (FrameCount == 0)
	{
		fActualFR_Out = 0;
		return S_OK;
	}
	float fMSPerFrame = (float)MSCount/FrameCount;
	fActualFR_Out = (float)1000/fMSPerFrame;
	if (fActualFR_Out == 0)
	{
		DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: Actual output frame rate is zero.")));
	}
	else
	{
		//DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: Actual output frame rate is: %.3f"), fActualFR_Out));
	}
	
	return S_OK;
}

HRESULT CKeystone::CalcActualFR_In(int MSCount, short FrameCount)
{
	if (FrameCount == 0)
	{
		fActualFR_In = 0;
		return S_OK;
	}
	float fMSPerFrame = (float)MSCount/FrameCount;
	fActualFR_In = (float)1000/fMSPerFrame;
	if (fActualFR_In == 0)
	{
		DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: Actual input frame rate is zero.")));
	}
	else
	{
		//DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: Actual input frame rate is: %.3f"), fActualFR_In));
	}
	return S_OK;
}

void CKeystone::TickOutputRateTime(DWORD dwCurrentTicks)
{
	//TODO: (IANIER) Calculate ActualFR
	//Please verify that I did this correctly:
	//Main questions: Would this give accurate values? 
	//Does this significantly decrease performace of the filter?
	//The values currently being created seem a little bit low.
	//DWORD dwCurrentTicks = GetTickCount();
	int iTDiff = abs(short(dwSecStartTicks_Out - dwCurrentTicks)); 
	//if (iTDiff > 2000) goto LeaveCalcActualFR;

	int i10Diff = abs(short(dw10sStartTicks_Out - dwCurrentTicks));
	if (i10Diff > 10000)
	{
		//Ten secs have gone by
		if (sFramesPassed_10s_Out == 0)
		{
			d10sFR_Out = 0;
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: No frames passed in 10 seconds.")));
			goto Restart10s;
		}
		float fMSPerFrame = (float)i10Diff/sFramesPassed_10s_Out;
		float f10s = (float)1000/fMSPerFrame;
		//if (f10s == 0)
		//{
		//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: 10s framerate is zero.")));
		//}
		//else
		//{
		//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: 10s output framerate is: %.3f"), f10s));
		//}

Restart10s:
		dw10sStartTicks_Out = GetTickCount();
		sFramesPassed_10s_Out = 0;
	}
	else
	{
		sFramesPassed_10s_Out++;
	}

	if (iTDiff > 1000)
	{
		CalcActualFR_Out(iTDiff, sFramesPassedThisSec_Out);
		sFramesPassedThisSec_Out = 0;
		dwSecStartTicks_Out = GetTickCount();
	}
	else
	{
		sFramesPassedThisSec_Out++;
	}
};

void CKeystone::TickInputRateTime(DWORD dwCurrentTicks)
{
	int i10Diff = abs(short(dw10sStartTicks_In - dwCurrentTicks));
	if (i10Diff > 10000)
	{
		//Ten secs have gone by
		if (sFramesPassed_10s_In == 0)
		{
			d10sFR_In = 0;
			DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: No frames passed in 10 seconds.")));
			goto Restart10s;
		}
		float fMSPerFrame = (float)i10Diff/sFramesPassed_10s_In;
		float f10s = (float)1000/fMSPerFrame;
		//if (f10s == 0)
		//{
		//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: 10s framerate is zero.")));
		//}
		//else
		//{
		//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: 10s input framerate is: %.3f"), f10s));
		//}

Restart10s:
		dw10sStartTicks_In = GetTickCount();
		sFramesPassed_10s_In = 0;
	}
	else
	{
		sFramesPassed_10s_In++;
	}

	int iTDiff = abs(short(dwSecStartTicks_In - dwCurrentTicks)); 
	if (iTDiff > 1000)
	{
		CalcActualFR_In(iTDiff, sFramesPassedThisSec_In);
		sFramesPassedThisSec_In = 0;
		dwSecStartTicks_In = GetTickCount();
	}
	else
	{
		sFramesPassedThisSec_In++;
	}
};




//HRESULT CKeystone::ReconnectOutput(CMediaType * pMTToPropose)
//{
//	CKeystoneOutputPin * asd = (CKeystoneOutputPin*) m_pOutput;
//	asd->ReconnectOutputPin(pMTToPropose);
//	return S_OK;
//}

//HRESULT CKeystone::ReconnectOutput()
//{
//	HRESULT hr;
//
//	//if(!m_fReconnectOutput) return S_OK;
//
//	CMediaType mtIn = m_pVideo->CurrentMediaType();
//	CMediaType mtOut = m_pOutput->CurrentMediaType();
//
//	VIDEOINFOHEADER2* vihIn = (VIDEOINFOHEADER2*)mtIn.Format();
//	VIDEOINFOHEADER2* vihOut = (VIDEOINFOHEADER2*)mtOut.Format();
//
//	SetRect(&vihOut->rcSource, 0, 0, vihIn->rcSource.right, vihIn->rcSource.bottom);
//	SetRect(&vihOut->rcTarget, 0, 0, vihIn->rcTarget.right, vihIn->rcTarget.bottom);
//	vihOut->dwPictAspectRatioX = vihIn->dwPictAspectRatioX;
//	vihOut->dwPictAspectRatioY = vihIn->dwPictAspectRatioY;
//	vihOut->bmiHeader.biWidth = vihIn->bmiHeader.biWidth;
//	vihOut->bmiHeader.biHeight = vihIn->bmiHeader.biHeight;
//	vihOut->bmiHeader.biSizeImage = vihIn->bmiHeader.biSizeImage;
//
//	hr = m_pOutput->GetConnected()->QueryAccept(&mtOut);
//	ASSERT(SUCCEEDED(hr));
//
//	IMediaSample* pOut = NULL;
//	HRESULT hr1 = m_pOutput->GetConnected()->ReceiveConnection(m_pOutput, &mtOut);
//	HRESULT hr2 = m_pOutput->GetDeliveryBuffer(&pOut, NULL, NULL, 0);
//	if(FAILED(hr1 || hr2 )) return E_FAIL;
//
//	AM_MEDIA_TYPE* pmt;
//	if(SUCCEEDED(pOut->GetMediaType(&pmt)) && pmt)
//	{
//		CMediaType mt = *pmt;
//		m_pOutput->SetMediaType(&mt);
//		DeleteMediaType(pmt);
//	}
//
//	pOut->Release();
//
//	// some renderers don't send this
//	NotifyEvent(EC_VIDEO_SIZE_CHANGED, MAKELPARAM(vihIn->rcTarget.right, vihIn->rcTarget.bottom), 0);
//
//	//m_fReconnectOutput = false;
//
//	return S_OK;
//}
