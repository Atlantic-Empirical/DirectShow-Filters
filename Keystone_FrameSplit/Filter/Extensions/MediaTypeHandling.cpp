#include "../Utility/appincludes.h"
#include <dvdmedia.h>

//Older version
bool CKeystone::CheckForMTChange(IMediaSample *pIn)
{
	CMediaType *mSource = 0;
    if (S_OK == pIn->GetMediaType((AM_MEDIA_TYPE**)&mSource) && mSource)
	{
		//The mediatype has changed on the input pin.
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: MediaType has changed on the input pin.")));
		if (bFeedbackClicks == true){
			Beep(1000, 1); //Click on media type change.
		}

		//VIDEOINFOHEADER2 *vih2in = (VIDEOINFOHEADER2*)mSource->Format();
		VIH2In = (VIDEOINFOHEADER2*)mSource->Format();
		//VIH2In->bmiHeader.biCompression = vih2in->bmiHeader.biCompression;

		SetupVideoSizing();
	
		////Check dwInterlaceFlags to see if source is interlaced
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: dwInterlaceFlags: %d"), VIH2In->dwInterlaceFlags));
		//if (VIH2In->dwInterlaceFlags & AMINTERLACE_IsInterlaced)
		//{
		//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Interlaced")));
		//}
		//else
		//{
		//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Progressive")));
		//}

		//DEBUGGING
		////Trying to figure out if we want anamorphic, letterbox, or pan and scan
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: X: %d"), VIH2In->dwPictAspectRatioX));
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Y: %d"), VIH2In->dwPictAspectRatioY));

		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Source left: %d"), VIH2In->rcSource.left));
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Source right: %d"), VIH2In->rcSource.right));
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Source top: %d"), VIH2In->rcSource.top));
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Source bottom: %d"), VIH2In->rcSource.bottom));

		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Target left: %d"), VIH2In->rcTarget.left));
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Target right: %d"), VIH2In->rcTarget.right));
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Target top: %d"), VIH2In->rcTarget.top));
		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Target bottom: %d"), VIH2In->rcTarget.bottom));

		//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: New incoming dwControlFlags: %d"), VIH2In->dwControlFlags));
		//DEBUGGING

		//Check for video height change
		if (VIH2In->bmiHeader.biHeight != LastConnectedHeight_VidIn)
		{
			//Video standard change
			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Video height change. New height: %d"), VIH2In->bmiHeader.biHeight));
			m_pVideoOne->ZoomDeInit();
			m_pVideoOne->ZoomInit(720, VIH2In->bmiHeader.biHeight);
			LastConnectedHeight_VidIn = VIH2In->bmiHeader.biHeight;
		}

		//We're now doing this in Phoenix
		////Check for video standard change (PAL-NTSC)
		//if (vih2in->rcSource.bottom != lConnectedHeight_In)
		//{
		//	//This is the end of us! Phoenix will destroy and create in our image.
		//	//NotifyEvent(EC_VIDEO_SIZE_CHANGED, 32778, vih2in->rcSource.bottom);

		//	//IMediaControl * IMS;
		//	//m_pGraph->QueryInterface(IID_IMediaControl, (void **) &IMS);
		//	//HRESULT hr = IMS->Stop();
		//	/*if (hr == S_OK)
		//	{
		//		NotifyEvent(EC_VIDEO_SIZE_CHANGED, 32778, vih2in->rcSource.bottom);
		//		return true;
		//	}
		//	else
		//	{
		//		return false;
		//	}*/
		//}

		//Check for ATPF Change
		LONGLONG llTmpATPF = VIH2In->AvgTimePerFrame;
		if (llTmpATPF != llTargetFR_In_ATPF)
		{
			//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: New incoming ATPF: %d"), llTmpATPF));
			llTargetFR_In_ATPF = llTmpATPF;

			if ((llTargetFR_In_ATPF == 333667) || (llTargetFR_In_ATPF == 333666))
			{
				if (OutIsVIH2)
				{
					VIH2Out->AvgTimePerFrame = 333667;
				}
				else
				{
					VIHOut->AvgTimePerFrame = 333667;
				}
				llTargetFR_In_ATPF = 333667;
				llTargetFR_Out_ATPF = 333667;

				//Turn off 3:2
				//m_pVideoOne->TurnOff32();

				//Bump Fields Down
				//m_pVideoOne->bBumpFieldsDown = true;
			}
				
			if (llTargetFR_In_ATPF == 400000)
			{
				if (OutIsVIH2)
				{
					VIH2Out->AvgTimePerFrame = 400000;
				}
				else
				{
					VIHOut->AvgTimePerFrame = 333667;
				}
				llTargetFR_In_ATPF = 400000;
				llTargetFR_Out_ATPF = 400000;

				//Turn off 3:2
				//m_pVideoOne->TurnOff32();

				//Bump Fields Down - PAL content should always be interlaced
				//m_pVideoOne->bBumpFieldsDown = true;

			}

			if ((llTargetFR_In_ATPF == 417084) || (llTargetFR_In_ATPF == 417083))
			{
				//For now it seems that the renderer will freak out if it gets dynamically changed ATPF
				if (OutIsVIH2)
				{
					VIH2Out->AvgTimePerFrame = 333667; //llTargetFR_In_ATPF
				}
				else
				{
					VIHOut->AvgTimePerFrame = 333667;
				}
				llTargetFR_In_ATPF = 417084;
				llTargetFR_Out_ATPF = 417084;

				//Turn-on 3:2
				//m_pVideoOne->TurnOn32(pIn);
			}

			//417083/4 = 23.976
			//400000   = 25
			//333666/7 = 29.97
		}

		////Check Field Order
		//IMediaSample2 * IMS2 = NULL;
		//if (FAILED(pIn->QueryInterface(IID_IMediaSample2, (void**) &IMS2)))
		//{
		//	return S_FALSE;
		//}
		//AM_SAMPLE2_PROPERTIES SampProps;
		//if (FAILED(IMS2->GetProperties(sizeof(AM_SAMPLE2_PROPERTIES), (BYTE*)&SampProps)))
		//{
		//	return S_FALSE;
		//}
		//IMS2->Release();
		//if (SampProps.dwTypeSpecificFlags & AM_VIDEO_FLAG_FIELD1FIRST)
		//{
		//	//TFF
		//	bTopFieldFirst = true;
		//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: TFF.")));
		//	NotifyEvent(EC_KEYSTONE_FIELDORDER, 1, 1);
		//}
		//else
		//{
		//	//BFF
		//	bTopFieldFirst = false;
		//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: BFF.")));
		//	NotifyEvent(EC_KEYSTONE_FIELDORDER, 1, 0);
		//}
		//if (SampProps.dwTypeSpecificFlags & 0x4000L)
		//{
		//	//Frame is interlaced
		//	m_pVideoOne->bBumpFieldsDown = true;
		//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Frame is interlaced. %d"), SampProps.dwTypeSpecificFlags));
		//}
		//else
		//{
		//	//Frame is progressive
		//	m_pVideoOne->bBumpFieldsDown = false;
		//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Frame is progressive. %d"), SampProps.dwTypeSpecificFlags));
		//}


		//Save the new input pin MediaType
		m_mtVideoOne = m_pVideoOne->CurrentMediaType();

		//We've got a new MT
		return true;
	}
	else
	{
		//No change to the MT
		return false;
	}
}

int CKeystone::GetMTHeight(const CMediaType * pMT)
{
	if (pMT->formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 * VIH2In = (VIDEOINFOHEADER2*)pMT->pbFormat;
		return VIH2In->bmiHeader.biHeight;
	}
	else
	{
		VIDEOINFOHEADER * VIHIn = (VIDEOINFOHEADER*)pMT->pbFormat;
		return VIHIn->bmiHeader.biHeight;
	}
}

HRESULT PutMTHeight(CMediaType * pMT, int nHeight)
{
	if (pMT->formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 * VIH2In = (VIDEOINFOHEADER2*)pMT->pbFormat;
		VIH2In->bmiHeader.biHeight = nHeight;
		VIH2In->rcSource.bottom = nHeight;
		VIH2In->rcTarget.bottom = nHeight;
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

HRESULT CKeystone::Convert480MTTo486(CMediaType * pMT)
{
	if (pMT->formattype == FORMAT_VideoInfo2) 
	{
		VIDEOINFOHEADER2* inType = (VIDEOINFOHEADER2*)pMT->Format();
		if ((inType->bmiHeader.biHeight == 480))
		{
			inType->rcSource.bottom = 486;
			inType->rcTarget.bottom = 486;
			inType->bmiHeader.biHeight = 486;
			return S_OK;
		}
	}
	else if (pMT->formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER* inType = (VIDEOINFOHEADER*)pMT->Format();
		if ((inType->bmiHeader.biHeight == 480))
		{
			inType->rcSource.bottom = 486;
			inType->rcTarget.bottom = 486;
			inType->bmiHeader.biHeight = 486;
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CKeystone::SetupVideoSizing()
{
	if ((LB_OK==0) && (PS_OK==0))
	{
		m_pVideoOne->ResizeMode = 1;
		return S_OK;
	}

	//Check for current AR mode
	//int ResizeMode; //1=anamorphic, 2=PanScan, 3=Letterbox
	DWORD dwCF =  VIH2In->dwControlFlags;
	int X = VIH2In->dwPictAspectRatioX;
	int Y = VIH2In->dwPictAspectRatioY;
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: NewAR. DesResizeMode: %d  ResizeMode: %d  dwCF: %d  X: %d  Y: %d  L: %d  R: %d"), m_pVideoOne->DesResizeMode, m_pVideoOne->ResizeMode, dwCF, X, Y, VIH2In->rcSource.left, VIH2In->rcSource.right));

	if (VIH2In->rcSource.left == 90) X = 16;

    switch(m_pVideoOne->DesResizeMode)
    {
        case 1: //Anamorphic
        {
			m_pVideoOne->ResizeMode = 1;
			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - no change")));
			break;
		}

        case 2: //Panscan
        {
CasePanscan:
			if (X==4) //Source is 4x3
			{
				m_pVideoOne->ResizeMode = 1;
				DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - no change")));				
				break;
			}
			else
			{
				if (PS_OK==1)
				{
					m_pVideoOne->ResizeMode = 2;
					DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - panscanned")));
					break;
				}
				else
				{
					goto CaseLetterbox;
				}
			}
		}

		case 3: //Letterbox
		{
CaseLetterbox:
			if (X==4) //Source is 4x3
			{
				m_pVideoOne->ResizeMode = 1;
				DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - no change")));				
				break;
			}
			else
			{
				if (LB_OK==1)
				{
					m_pVideoOne->ResizeMode = 3;
					DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - letterboxed")));
					break;
				}
				else
				{
					goto CasePanscan;
				}
			}
		}
	}

	return S_OK;

	//if (dwCF==0)
	//{
	////Nav is NOT calling for padding
	//	if ((X==16) && (Y==9))
	//	{
	//	//Source is anamorphic
	//		if (m_pVideoOne->DesResizeMode == 1)
	//		{
	//		//User has selected anamorphic
	//			m_pVideoOne->ResizeMode = 1;
	//			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - no change")));
	//		}
	//		else if (m_pVideoOne->DesResizeMode == 2)
	//		{
	//		//User has selected panscan
	//			m_pVideoOne->ResizeMode = 2;
	//			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - panscanned")));
	//		}
	//		else if (m_pVideoOne->DesResizeMode == 3)
	//		{
	//		//User has selected letterbox
	//			//Confirmed that we land here when emulator AR is lb, source is ana, flag is ps=true, lb=false
	//			//so we should always panscan here?
	//			m_pVideoOne->ResizeMode = 2;
	//			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - panscanned")));
	//		}
	//	}
	//	else if ((X==4) && (Y==3))
	//	{
	//	//Source is NOT anamorphic
	//		if (m_pVideoOne->DesResizeMode == 1)
	//		{
	//		//User has selected anamorphic
	//			m_pVideoOne->ResizeMode = 1;
	//			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - no change")));
	//		}
	//		else if (m_pVideoOne->DesResizeMode == 2)
	//		{
	//		//User has selected panscan
	//			if ((VIH2In->rcSource.left == 90) & (VIH2In->rcSource.right == 630))
	//			{
	//				//we'll arrive here if the source is 16x9 and the user has selected panscan
	//				m_pVideoOne->ResizeMode = 2; //PANSCAN
	//				DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - panscanned")));
	//			}
	//			else
	//			{
	//				//we'll arrive here if the source is 4x3 and the user has selected panscan
	//				m_pVideoOne->ResizeMode = 1; //ANAMORPHIC
	//				DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - no change")));
	//			}
	//		}
	//		else if (m_pVideoOne->DesResizeMode == 3)
	//		{
	//		//User has selected letterbox
	//			if ((VIH2In->rcSource.left == 90) & (VIH2In->rcSource.right == 630))
	//			{
	//				//we'll arrive here if the source is 16x9 and the user has selected panscan
	//				m_pVideoOne->ResizeMode = 2; //PANSCAN
	//				DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - panscanned")));
	//			}
	//			else
	//			{
	//				//we'll arrive here if the source is 4x3 and the user has selected panscan
	//				m_pVideoOne->ResizeMode = 1; //ANAMORPHIC
	//				DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - no change")));
	//			}
	//		}
	//	}
	//}
	//else if (dwCF==3)
	//{
	////Nav IS calling for padding to 4x3
	//	if ((X==16) && (Y==9))
	//	{
	//	//Source is anamorphic
	//		if (m_pVideoOne->DesResizeMode == 1)
	//		{
	//		//User has selected anamorphic
	//			m_pVideoOne->ResizeMode = 1;
	//			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - no change")));
	//		}
	//		else if (m_pVideoOne->DesResizeMode == 2)
	//		{
	//		//User has selected panscan
	//			m_pVideoOne->ResizeMode = 2;
	//			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - panscanned")));
	//		}
	//		else if (m_pVideoOne->DesResizeMode == 3)
	//		{
	//		//User has selected letterbox
	//			m_pVideoOne->ResizeMode = 3;
	//			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - letterboxed")));				
	//		}
	//	}
	//	else if ((X==4) && (Y==3))
	//	{
	//	//Source is NOT anamorphic
	//		if (m_pVideoOne->DesResizeMode == 1)
	//		{
	//		//User has selected anamorphic
	//			m_pVideoOne->ResizeMode = 1;
	//			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - no change")));
	//		}
	//		else if (m_pVideoOne->DesResizeMode == 2)
	//		{
	//		//User has selected panscan
	//			m_pVideoOne->ResizeMode = 1;
	//			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - no change")));				
	//		}
	//		else if (m_pVideoOne->DesResizeMode == 3)
	//		{
	//		//User has selected letterbox
	//			m_pVideoOne->ResizeMode = 1;
	//			DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: OUTPUT - no change")));			
	//		}
	//	}
	//}

	//m_pVideoOne->ResizeMode = 3; //LETTERBOX
	//if ((VIH2In->rcSource.left == 90) & (VIH2In->rcSource.right == 630))


	////OLD WAY
	//if ((dwCF==3) && (X==16) && (Y==9))
	//{
	//	//We'll arrive here if the source is 16x9 and the user has selected letterbox
	//	m_pVideoOne->ResizeMode = 3; //LETTERBOX
	//}
	//else if ((dwCF==0) && (X==4) && (Y==3))
	//{
	//	//This is meant to prevent stretching of pan-scan content
	//	if ((VIH2In->rcSource.left == 90) & (VIH2In->rcSource.right == 630))
	//	{
	//		//we'll arrive here if the source is 16x9 and the user has selected panscan
//			m_pVideoOne->ResizeMode = 2; //PANSCAN
	//	}
	//	else
	//	{
	//		//we'll arrive here if the source is 4x3 and the user has selected panscan
	//		m_pVideoOne->ResizeMode = 1; //ANAMORPHIC
	//	}
	//}




	//else if ((dwCF==0) && (X==16) && (Y==9))
	//{
	//	m_pVideoOne->ResizeMode = 1;
	//}
	//else
	//{
	//	m_pVideoOne->ResizeMode = 1;
	//}

	//return S_OK;
}
