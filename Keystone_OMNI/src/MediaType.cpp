#include "appincludes.h"
#include <dvdmedia.h>

bool CKeystone::MDTY_CheckForMediaTypeChange(IMediaSample *pIn)
{
	CMediaType *mSource = 0;
    if (S_OK == pIn->GetMediaType((AM_MEDIA_TYPE**)&mSource) && mSource)
	{
		//The mediatype has changed on the input pin.
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: MediaType has changed on the input pin.")));
		if (USEROPTION_FeedbackClicks == true){
			Beep(1000, 1); //Click on media type change.
		}

		//VIDEOINFOHEADER2 *PIVI_VIH2 = (VIDEOINFOHEADER2*)mSource->Format();
		PIVI_VIH2 = (VIDEOINFOHEADER2*)mSource->Format();
		//PIVI_VIH2->bmiHeader.biCompression = PIVI_VIH2->bmiHeader.biCompression;

		SCLE_SetupVideoSizing();
	
		////Check dwInterlaceFlags to see if source is interlaced
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: dwInterlaceFlags: %d"), PIVI_VIH2->dwInterlaceFlags));
		//if (PIVI_VIH2->dwInterlaceFlags & AMINTERLACE_IsInterlaced)
		//{
		//	DbgLog((LOG_TRACE, 0, TEXT("Keystone: Interlaced")));
		//}
		//else
		//{
		//	DbgLog((LOG_TRACE, 0, TEXT("Keystone: Progressive")));
		//}

		//DEBUGGING
		////Trying to figure out if we want anamorphic, letterbox, or pan and scan
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: X: %d"), PIVI_VIH2->dwPictAspectRatioX));
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Y: %d"), PIVI_VIH2->dwPictAspectRatioY));

		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Source left: %d"), PIVI_VIH2->rcSource.left));
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Source right: %d"), PIVI_VIH2->rcSource.right));
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Source top: %d"), PIVI_VIH2->rcSource.top));
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Source bottom: %d"), PIVI_VIH2->rcSource.bottom));

		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Target left: %d"), PIVI_VIH2->rcTarget.left));
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Target right: %d"), PIVI_VIH2->rcTarget.right));
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Target top: %d"), PIVI_VIH2->rcTarget.top));
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Target bottom: %d"), PIVI_VIH2->rcTarget.bottom));

		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: New incoming dwControlFlags: %d"), PIVI_VIH2->dwControlFlags));
		//DEBUGGING

		//Check for video height change
		if (PIVI_VIH2->bmiHeader.biHeight != PIVI_LastConnectedHeight)
		{
			//Video standard change
			DbgLog((LOG_TRACE, 0, TEXT("Keystone: Video height change. New height: %d"), PIVI_VIH2->bmiHeader.biHeight));
			SCLE_ZoomDeInit();
			SCLE_ZoomInit(720, PIVI_VIH2->bmiHeader.biHeight);
			PIVI_LastConnectedHeight = PIVI_VIH2->bmiHeader.biHeight;
		}

		//Check for ATPF Change
		LONGLONG llTmpATPF = PIVI_VIH2->AvgTimePerFrame;
		if (llTmpATPF != FRRT_TargetFramerate_In_ATPF)
		{
			//DbgLog((LOG_TRACE, 0, TEXT("Keystone: New incoming ATPF: %d"), llTmpATPF));
			FRRT_TargetFramerate_In_ATPF = llTmpATPF;

			if ((FRRT_TargetFramerate_In_ATPF == 333667) || (FRRT_TargetFramerate_In_ATPF == 333666))
			{
				if (PIVO_FormatIsVIH2)
				{
					PIVO_VIH2->AvgTimePerFrame = 333667;
				}
				else
				{
					PIVO_VIH->AvgTimePerFrame = 333667;
				}
				FRRT_TargetFramerate_In_ATPF = 333667;
				FRRT_TargetFramerate_Out_ATPF = 333667;

				//Turn off 3:2
				//TurnOff32();

				//Bump Fields Down
				//bBumpFieldsDown = true;
			}
				
			if (FRRT_TargetFramerate_In_ATPF == 400000)
			{
				if (PIVO_FormatIsVIH2)
				{
					PIVO_VIH2->AvgTimePerFrame = 400000;
				}
				else
				{
					PIVO_VIH->AvgTimePerFrame = 333667;
				}
				FRRT_TargetFramerate_In_ATPF = 400000;
				FRRT_TargetFramerate_Out_ATPF = 400000;

				//Turn off 3:2
				//m_pVideo->TurnOff32();

				//Bump Fields Down - PAL content should always be interlaced
				//bBumpFieldsDown = true;

			}

			if ((FRRT_TargetFramerate_In_ATPF == 417084) || (FRRT_TargetFramerate_In_ATPF == 417083))
			{
				//For now it seems that the renderer will freak out if it gets dynamically changed ATPF
				if (PIVO_FormatIsVIH2)
				{
					PIVO_VIH2->AvgTimePerFrame = 333667; //llTargetFR_In_ATPF
				}
				else
				{
					PIVO_VIH->AvgTimePerFrame = 333667;
				}
				FRRT_TargetFramerate_In_ATPF = 417084;
				FRRT_TargetFramerate_Out_ATPF = 417084;

				//Turn-on 3:2
				//m_pVideo->TurnOn32(pIn);
			}

			//417083/4 = 23.976
			//400000   = 25
			//333666/7 = 29.97
		}

		//Save the new input pin MediaType
		PIVI_MediaType = m_pVideo->CurrentMediaType();

		//We've got a new MT
		return true;
	}
	else
	{
		//No change to the MT
		return false;
	}
}

//int CKeystone::MDTY_GetMediaTypeHeight(const CMediaType * pMT)
//{
//	if (pMT->formattype == FORMAT_VideoInfo2)
//	{
//		VIDEOINFOHEADER2 * PIVI_VIH2 = (VIDEOINFOHEADER2*)pMT->pbFormat;
//		return PIVI_VIH2->bmiHeader.biHeight;
//	}
//	else
//	{
//		VIDEOINFOHEADER * VIHIn = (VIDEOINFOHEADER*)pMT->pbFormat;
//		return VIHIn->bmiHeader.biHeight;
//	}
//}

HRESULT PutMTHeight(CMediaType * pMT, int nHeight)
{
	if (pMT->formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 * PIVI_VIH2 = (VIDEOINFOHEADER2*)pMT->pbFormat;
		PIVI_VIH2->bmiHeader.biHeight = nHeight;
		PIVI_VIH2->rcSource.bottom = nHeight;
		PIVI_VIH2->rcTarget.bottom = nHeight;
	}
	else
	{
		VIDEOINFOHEADER * VIHIn = (VIDEOINFOHEADER*)pMT->pbFormat;
		VIHIn->bmiHeader.biHeight = nHeight;
		VIHIn->rcSource.bottom = nHeight;
		VIHIn->rcTarget.bottom = nHeight;
	}
	return S_OK;
}

//HRESULT CKeystone::MDTY_ConvertMediaType_480to486(CMediaType * pMT)
//{
//	if (pMT->formattype == FORMAT_VideoInfo2) 
//	{
//		VIDEOINFOHEADER2* inType = (VIDEOINFOHEADER2*)pMT->Format();
//		if ((inType->bmiHeader.biHeight == 480))
//		{
//			inType->rcSource.bottom = 486;
//			inType->rcTarget.bottom = 486;
//			inType->bmiHeader.biHeight = 486;
//			return S_OK;
//		}
//	}
//	else if (pMT->formattype == FORMAT_VideoInfo)
//	{
//		VIDEOINFOHEADER* inType = (VIDEOINFOHEADER*)pMT->Format();
//		if ((inType->bmiHeader.biHeight == 480))
//		{
//			inType->rcSource.bottom = 486;
//			inType->rcTarget.bottom = 486;
//			inType->bmiHeader.biHeight = 486;
//			return S_OK;
//		}
//	}
//	return E_FAIL;
//}
//
