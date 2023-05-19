#include "appincludes.h"

void CKeystone::LogSampleMetadata(IMediaSample * pSample)
{
	DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG ===========================================================================================")));

	CMediaType *mSource = 0;
    if (S_OK == pSample->GetMediaType((AM_MEDIA_TYPE**)&mSource) && mSource)
	{
		//AM_MEDIA_TYPE
		DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG MT_majortype=%s"), (LPCTSTR)CDisp(mSource->majortype)));
		DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG MT_subtype=%s"), (LPCTSTR)CDisp(mSource->subtype)));
		DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG MT_formattype=%s"), (LPCTSTR)CDisp(mSource->formattype)));
		DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG MT_FixedSizeSamples=%d"), (int)mSource->bFixedSizeSamples));
		DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG MT_TemporalCompression=%d"), (int)mSource->bTemporalCompression));
		DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG MT_SampleSize=%d"), mSource->lSampleSize));
	
		if (mSource->formattype == FORMAT_VideoInfo2)
		{
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH2")));
	
			VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)mSource->pbFormat;
			//rcSource
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_SourceRect_Left=%d"), VIH2->rcSource.left));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_SourceRect_Top=%d"), VIH2->rcSource.top));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_SourceRect_Right=%d"), VIH2->rcSource.right));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_SourceRect_Bottom=%d"), VIH2->rcSource.bottom));
			//rcTarget
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_Targetect_Left=%d"), VIH2->rcTarget.left));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_TargetRect_Top=%d"), VIH2->rcTarget.top));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_TargetRect_Right=%d"), VIH2->rcTarget.right));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_TargetRect_Bottom=%d"), VIH2->rcTarget.bottom));

			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_Bitrate=%d"), VIH2->dwBitRate));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_BitErrorRate=%d"), VIH2->dwBitErrorRate));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_ATPF=%d"), VIH2->AvgTimePerFrame));

			//BITMAPINFOHEADER
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Size=%d"), VIH2->bmiHeader.biSize));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Width=%d"), VIH2->bmiHeader.biWidth));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Height=%d"), VIH2->bmiHeader.biHeight));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Planes=%d"), VIH2->bmiHeader.biPlanes));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Bitcount=%d"), VIH2->bmiHeader.biBitCount));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Compression=%d"), VIH2->bmiHeader.biCompression));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_SizeImage=%d"), VIH2->bmiHeader.biSizeImage));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_XPelsPerMeter=%d"), VIH2->bmiHeader.biXPelsPerMeter));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_YPelsPerMeter=%d"), VIH2->bmiHeader.biYPelsPerMeter));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_ClrUsed=%d"), VIH2->bmiHeader.biClrUsed));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_ClrImportant=%d"), VIH2->bmiHeader.biClrImportant));

			//VIH2 Specific
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH2_InterlaceFlags=%d"), VIH2->dwInterlaceFlags));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH2_CopyProtectFlags=%d"), VIH2->dwCopyProtectFlags));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH2_PictAspectRatioX=%d"), VIH2->dwPictAspectRatioX));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH2_PictAspectRatioY=%d"), VIH2->dwPictAspectRatioY));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH2_ControlFlags=%d"), VIH2->dwControlFlags));
		}
		else if (mSource->formattype == FORMAT_VideoInfo)
		{
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH")));

			VIDEOINFOHEADER * VIH = (VIDEOINFOHEADER*)mSource->pbFormat;
			//rcSource
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_SourceRect_Left=%d"), VIH->rcSource.left));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_SourceRect_Top=%d"), VIH->rcSource.top));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_SourceRect_Right=%d"), VIH->rcSource.right));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_SourceRect_Bottom=%d"), VIH->rcSource.bottom));
			//rcTarget
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_Targetect_Left=%d"), VIH->rcTarget.left));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_TargetRect_Top=%d"), VIH->rcTarget.top));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_TargetRect_Right=%d"), VIH->rcTarget.right));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_TargetRect_Bottom=%d"), VIH->rcTarget.bottom));

			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_Bitrate=%d"), VIH->dwBitRate));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_BitErrorRate=%d"), VIH->dwBitErrorRate));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG VIH_ATPF=%d"), VIH->AvgTimePerFrame));

			//BITMAPINFOHEADER
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Size=%d"), VIH->bmiHeader.biSize));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Width=%d"), VIH->bmiHeader.biWidth));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Height=%d"), VIH->bmiHeader.biHeight));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Planes=%d"), VIH->bmiHeader.biPlanes));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Bitcount=%d"), VIH->bmiHeader.biBitCount));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_Compression=%d"), VIH->bmiHeader.biCompression));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_SizeImage=%d"), VIH->bmiHeader.biSizeImage));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_XPelsPerMeter=%d"), VIH->bmiHeader.biXPelsPerMeter));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_YPelsPerMeter=%d"), VIH->bmiHeader.biYPelsPerMeter));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_ClrUsed=%d"), VIH->bmiHeader.biClrUsed));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG BMI_ClrImportant=%d"), VIH->bmiHeader.biClrImportant));
		}
	}
	else
	{
		DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA ***NO MEDIA TYPE IN THIS SAMPLE***")));
	}

	IMediaSample2 * IMS2 = NULL;
	if (SUCCEEDED(pSample->QueryInterface(IID_IMediaSample2, (void**) &IMS2)))
	{
		AM_SAMPLE2_PROPERTIES SampProps;
		if (FAILED(IMS2->GetProperties(sizeof(AM_SAMPLE2_PROPERTIES), (BYTE*)&SampProps)))
		{
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA ***NO SAMPLE2 PROPERTIES IN THIS SAMPLE***")));
		}
		else
		{
			//AM_SAMPLE2_PROPERTIES
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG S2P_cbData=%d"), SampProps.cbBuffer));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG S2P_Actual=%d"), SampProps.lActual));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG S2P_Start=%I64d"), SampProps.tStart));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG S2P_Stop=%I64d"), SampProps.tStop));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG S2P_StreamId=%d"), SampProps.dwStreamId));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG S2P_MediaType=%d"), SampProps.pMediaType));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG S2P_pbBuffer=%d"), SampProps.pbBuffer));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG S2P_cbBuffer=%d"), SampProps.cbBuffer));
		
			//TYPE_SPECIFIC_FLAGS
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG TSF_FieldMask=%d"), SampProps.dwTypeSpecificFlags & 0x03));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG TSF_InterlevedFrame=%d"), SampProps.dwTypeSpecificFlags & 0x00));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG TSF_Field1=%d"), SampProps.dwTypeSpecificFlags & 0x01));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG TSF_Field2=%d"), SampProps.dwTypeSpecificFlags & 0x02));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG TSF_Field1First=%d"), SampProps.dwTypeSpecificFlags & 0x04));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG TSF_Weave=%d"), SampProps.dwTypeSpecificFlags & 0x08));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG TSF_RepeatField=%d"), SampProps.dwTypeSpecificFlags & 0x40));

			//AM_SAMPLE_PROPERTY_FLAGS
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG SPF_SplicePoint=%d"), SampProps.dwSampleFlags & AM_SAMPLE_SPLICEPOINT));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG SPF_PreRoll=%d"), SampProps.dwSampleFlags & AM_SAMPLE_PREROLL));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG SPF_DataDiscontinuity=%d"), SampProps.dwSampleFlags & AM_SAMPLE_DATADISCONTINUITY));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG SPF_TypeChanged=%d"), SampProps.dwSampleFlags & AM_SAMPLE_TYPECHANGED));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG SPF_TimeValid=%d"), SampProps.dwSampleFlags & AM_SAMPLE_TIMEVALID));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG SPF_TimeDiscontinuity=%d"), SampProps.dwSampleFlags & AM_SAMPLE_TIMEDISCONTINUITY));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG SPF_FlushOnPause=%d"), SampProps.dwSampleFlags & AM_SAMPLE_FLUSH_ON_PAUSE));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG SPF_StopValid=%d"), SampProps.dwSampleFlags & AM_SAMPLE_STOPVALID));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG SPF_EndOfStream=%d"), SampProps.dwSampleFlags & AM_SAMPLE_ENDOFSTREAM));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG SPF_Media=%d"), SampProps.dwSampleFlags & AM_STREAM_MEDIA));
			DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA LOG SPF_Control=%d"), SampProps.dwSampleFlags & AM_STREAM_CONTROL));
		}
		
		IMS2->Release();
	}
	else
	{
		DbgLog((LOG_TRACE,0,TEXT("Keystone: SAMPLE METADATA ***NO IMediaSample2 IN THIS SAMPLE***")));
	}
}



HRESULT CKeystone::LogMPEGFlags(IMediaSample * pIn)
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
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: dwFlags: %d"), dwFlags));
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

	REFERENCE_TIME atpf = FRRT_TargetFramerate_In_ATPF;

	return S_OK;
}

