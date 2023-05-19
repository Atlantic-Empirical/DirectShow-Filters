#include "appincludes.h"
#include "Filter_iKeystone.h"
#include <atlbase.h>

STDMETHODIMP CKeystone::get_InputMediaType(CMediaType *InputMediaType  )
{
    CheckPointer(InputMediaType,E_POINTER);
	*InputMediaType = m_pVideo->CurrentMediaType();
    return NOERROR;
}

STDMETHODIMP CKeystone::get_OutputMediaType(CMediaType *OutputMediaType  )
{
    CheckPointer(OutputMediaType,E_POINTER);
	*OutputMediaType = m_pOutput->CurrentMediaType();
    return NOERROR;
}

STDMETHODIMP CKeystone::get_InputWidth(long *InputWidth)
{
    CheckPointer(InputWidth,E_POINTER);
	if(m_pVideo->IsConnected() != TRUE)
	{
		*InputWidth = 0;
	}
	else
	{
		*InputWidth = PIVI_ConnectedWidth;
	}
	return S_OK;
}

STDMETHODIMP CKeystone::get_InputHeight(long *InputHeight)
{
    CheckPointer(InputHeight,E_POINTER);
	if(m_pVideo->IsConnected() != TRUE)
	{
		*InputHeight = 0;
	}
	else
	{
		*InputHeight = PIVI_ConnectedHeight;
	}
	return S_OK;
}

STDMETHODIMP CKeystone::get_OutputWidth(long *OutputWidth)
{
	if(m_pOutput->IsConnected() != TRUE)
	{
		*OutputWidth = 0;
	}
	else
	{
		CheckPointer(OutputWidth,E_POINTER);
		CMediaType* pmt = &m_pOutput->CurrentMediaType();
		VIDEOINFOHEADER2* pvi = (VIDEOINFOHEADER2*)pmt->Format();
		*OutputWidth = pvi->rcTarget.right;
	}
	return S_OK;
}

STDMETHODIMP CKeystone::get_OutputHeight(long *OutputHeight)
{
	if(m_pOutput->IsConnected() != TRUE)
	{
		*OutputHeight = 0;
	}
	else
	{
		CheckPointer(OutputHeight,E_POINTER);
		CMediaType* pmt = &m_pOutput->CurrentMediaType();
		VIDEOINFOHEADER2* pvi = (VIDEOINFOHEADER2*)pmt->Format();
		*OutputHeight = pvi->rcTarget.bottom;
	}
	return S_OK;
}

STDMETHODIMP CKeystone::GrabSample(int in_SampleWhat, LPBYTE *out_pSample, long *out_lSampleSize, long *out_Width, long *out_Height)
{
	if (SAGR_SampleOperationUnderway == true)
	{		
		//Can only do one at a time
		return E_ACCESSDENIED;
	}

	SAGR_SampleOperationUnderway = true;
	USEROPTION_SampleWhat = in_SampleWhat;
	SAGR_GetSample = true;
	SAGR_Sample = NULL;

	if (USEROPTION_SampleWhat == 3)
	{
		SAGR_GetSample = false;
		HandleSampleGrab_LastOutput();
		goto SkipAhead;
	}

	DWORD dwSampStartTicks = GetTickCount();
	while (SAGR_Sample == NULL)
	{
		Sleep(1);
		//Just wait until the sample is created, up to three seconds then return control to app.
		if (GetTickCount() - dwSampStartTicks > 3000)
		{
			//Timeout abort
			SAGR_SampleOperationUnderway = false;
			SAGR_Sample = NULL;
			return E_ABORT;
		}
	}
	if (SAGR_Sample == 0)
	{
		//HandleSampleGrab did not succeed.
		return E_FAIL;
	}

SkipAhead:
	//Now, the sample buffer is available
	*out_pSample = SAGR_Sample;
	*out_lSampleSize = SAGR_SampleSize;
	*out_Width = SAGR_SampleWidth;
	*out_Height = SAGR_SampleHeight;

	SAGR_SampleOperationUnderway = false;
	USEROPTION_SampleWhat = NULL;
	SAGR_Sample = NULL;
	SAGR_SampleSize = 0;
	SAGR_SampleWidth = 0;
	SAGR_SampleHeight = 0;	
	return S_OK;
}

STDMETHODIMP CKeystone::UnlockFilter(GUID *FilterKey)
{
    CheckPointer(FilterKey,E_POINTER);
	//TODO: 5) Make UnlockFilter with GUID functional. Should be easy, mostly done.
	//It would be better if there were a way to just compare GUIDs
	//This method needs to compare the provided FilterKey GUID to a list/array of GUIDs defined
	//in DL_Adapter_uids.h
	if (*FilterKey == CLSID_Key_Master)
	{
		LOCK_FilterIsLocked = false;
		return S_OK;
	}
	if (*FilterKey == CLSID_Key_Development)
	{
		LOCK_FilterIsLocked = false;
		return S_OK;
	}
	if (*FilterKey == CLSID_Key_Client1)
	{
		LOCK_FilterIsLocked = false;
		return S_OK;
	}
	if (*FilterKey == CLSID_Key_Bolinas)
	{
		LOCK_FilterIsLocked = false;
		return S_OK;
	}
	LOCK_FilterIsLocked = true;
	return S_OK;
}

STDMETHODIMP CKeystone::put_FeedbackClicks(bool DoClicks)
{
	USEROPTION_FeedbackClicks = DoClicks;
	return S_OK;
}

STDMETHODIMP CKeystone::get_FeedbackClicks(bool *DoClicks)
{
    CheckPointer(DoClicks,E_POINTER);
	if (USEROPTION_FeedbackClicks == true)
	{
		*DoClicks = true;
	}
	else
	{
		*DoClicks = false;
	}
	return S_OK;
}

STDMETHODIMP CKeystone::Set32Status(int Do32)
{
	USEROPTION_AllowTelecine = Do32;
	return S_OK;
}

STDMETHODIMP CKeystone::FrameStep(bool bForward)
{
	if (!FRST_FrameStepping)
	{
		FRST_FrameStepping = true;
	}
	else
	{
		FRST_DeliverFrameStepSample = true;
	}
	return S_OK;
}

STDMETHODIMP CKeystone::QuitFrameStepping()
{
	FRST_FrameStepping = false;
	m_pVideo->bSendNewSegment;
	//bRestartTimeStamps = true;
	return S_OK;
}

STDMETHODIMP CKeystone::ActivateVarispeed(double Speed)
{
	VARI_InVarispeed = true;
	VARI_MSVarispeedInterval = (((int)FRRT_TargetFramerate_In_ATPF) * Speed) / 10000;
	VARI_MSVarispeedInterval = fround(VARI_MSVarispeedInterval, 0);
	return S_OK;
}

STDMETHODIMP CKeystone::DeactivateVarispeed()
{
	VARI_InVarispeed = false;
	VARI_MSVarispeedInterval = FRRT_TargetFramerate_In_ATPF;
	return S_OK;
}

STDMETHODIMP CKeystone::SetL21State(bool bL21Active)
{
	if (L21_IsInitialized == false)
	{
		return S_FALSE;
	}
	m_pLine21->bL21StreamIsActive = bL21Active;
	return S_OK;
}

STDMETHODIMP CKeystone::ShowJacketPicture(LPCTSTR stJPPath, int X, int Y, int W, int H)
{
 	CComPtr<IMediaSample> pSample;
	GetJPSample(stJPPath, X, Y, W, H, &pSample);
	HRESULT hr = m_pOutput->m_pInputPin->Receive(pSample);
	return hr;
}

STDMETHODIMP CKeystone::ShowBitmap(LPBYTE pBMP, int W, int H, int X, int Y, int Format)
{
	HRESULT hr;
	hr = BMRD_SetBMPtoBuffer(pBMP, W, H, SAGR_LastBufferReceived);
	if (FAILED(hr)) return hr;
	hr = Pipeline_Core_ReSizeMixSendSample(1, false);
	return hr;

	////ATTEMPT ONE ==================================================
	//CComPtr<IMediaSample> pSample;
	//BMRD_GetBMPSample(pBMP, X, Y, W, H, &pSample);

	//CRefTime crtStreamTime;

	////Set Time Stamps
	//REFERENCE_TIME rtStart, rtEnd;
	//StreamTime(crtStreamTime);
	//rtStart = crtStreamTime.m_time + 250000;
	//rtEnd = rtStart + FRRT_TargetFramerate_Out_ATPF;
	//pSample->SetTime(&rtStart, &rtEnd);
	////pSample->SetTime(NULL, NULL);
	//HRESULT hr = m_pOutput->m_pInputPin->Receive(pSample);

	////ATTEMPT TWO ==================================================
	//CComPtr<IMediaSample> pSample1;
	//BMRD_GetBMPSample(pBMP, X, Y, W, H, &pSample1);

	////Set Time Stamps
	//StreamTime(crtStreamTime);
	//rtStart = crtStreamTime.m_time + 250000;
	//rtEnd = rtStart + FRRT_TargetFramerate_Out_ATPF;
	//pSample1->SetTime(&rtStart, &rtEnd);

	//hr = m_pOutput->m_pInputPin->Receive(pSample1);

	////ATTEMPT THREE ==================================================
	//CComPtr<IMediaSample> pSample2;
	//BMRD_GetBMPSample(pBMP, X, Y, W, H, &pSample2);
	//
	////Set Time Stamps
	//StreamTime(crtStreamTime);
	//rtStart = crtStreamTime.m_time + 250000;
	//rtEnd = rtStart + FRRT_TargetFramerate_Out_ATPF;
	//pSample2->SetTime(&rtStart, &rtEnd);

	//hr = m_pOutput->m_pInputPin->Receive(pSample2);

	return S_OK;
}

STDMETHODIMP CKeystone::SaveNextXFrames(int Count, LPCTSTR stDumpPath)
{
	SAGR_MultiFrameCount = Count;
	SAGR_DumpPath = stDumpPath;
	DWORD dwSampStartTicks = GetTickCount();
	DWORD dwEndTicks = dwSampStartTicks + (99 * Count);
	DWORD dwCurrentTicks = dwSampStartTicks;
	while ((SAGR_MultiFrameCount > 0) & (dwCurrentTicks < dwEndTicks))
	{
		dwCurrentTicks = GetTickCount();
		Sleep(1000);
	}
	int FramesOutput = (Count - SAGR_MultiFrameCount);
	SAGR_MultiFrameCount = 0;
	return FramesOutput;
}

STDMETHODIMP CKeystone::ForceOutputConnectSize(int W, int H )
{
	USEROPTION_ForceOutputConnectSize = true;
	USEROPTION_ForceOutput_W = W;
	USEROPTION_ForceOutput_H = H;
	return S_OK;
}

STDMETHODIMP CKeystone::ShowUYVYFile(LPCTSTR stYUVPath, int X, int Y, int W, int H)
{
	FILE* f = _tfopen(stYUVPath, _T("rb"));
	if(!f) return E_FAIL;
	YVRD_YUVBufferSize = W*H*2;
	YVRD_YUVBuffer = (LPBYTE)malloc(YVRD_YUVBufferSize);
	int BytesRead = fread(YVRD_YUVBuffer, 1, YVRD_YUVBufferSize, f);
	if (BytesRead < YVRD_YUVBufferSize)
	{
		//Not enough bytes read
		return E_FAIL;
	}
	YVRD_DoYUVStillRender = true;
	return S_OK;
}

STDMETHODIMP CKeystone::ClearUYVYFile()
{
	free(YVRD_YUVBuffer);
	YVRD_DoYUVStillRender = false;
	return S_OK;
}

STDMETHODIMP CKeystone::ClearUYVYFile_A()
{
	YVRD_DoYUVStillRender = false;
	return S_OK;
}

STDMETHODIMP CKeystone::ShowUYVYBuffer(LPBYTE pUYVY, int W, int H, int X, int Y)
{
	YVRD_YUVBufferSize = W*H*2;
	YVRD_YUVBuffer = pUYVY;
	YVRD_DoYUVStillRender = true;
	return S_OK;
}

STDMETHODIMP CKeystone::Pause(int nPause)
{
	Pausing = nPause;
	return S_OK;
}

STDMETHODIMP CKeystone::ActivateFFRW(int nRate)
{
	FFRW_Rate = nRate;
	VARI_InFFRW = true;
	return S_OK;
}

STDMETHODIMP CKeystone::DeactivateFFRW()
{
	VARI_InFFRW = false;
	SCLE_SetupVideoSizing();
	return S_OK;
}

STDMETHODIMP CKeystone::ResendLastSamp(int Unconditional)
{
	return Pipeline_Core_ReSizeMixSendSample(Unconditional, true);
}

STDMETHODIMP CKeystone::SetNULLTimestamps()
{
	USEROPTION_SetNULLTimestamps = true;
	return S_OK;
}

STDMETHODIMP CKeystone::SetProperty(GUID gProp, int iValue)
{
	//if (gProp == CLSID_Property_TrialOverride)
	//{
	//	if (iValue == 32778)
	//	{
	//		LOCK_TRIALOVERRIDE = true;					
	//	}
	//	else
	//	{
	//		LOCK_TRIALOVERRIDE = false;			
	//	}
	//}
	if (gProp == CLSID_Property_TrialMode)
	{
		if (LOCK_TRIALMODE != true)
		{
			LOCK_TRIALMODE = true;
			LOCK_FrameCount = 30 * 60 * LOCK_TimeoutMinutes;
		}
	}
	return S_OK;
}

STDMETHODIMP CKeystone::StartRecording(LPCWSTR pszFileName)
{
	SAGR_RecordingImages = true;

	//USES_CONVERSION;
	HRESULT hr = ::SHStrDupW(pszFileName, &SAGR_ImageRecordTargPath);
	//ImageRecordTargPath = W2A(pszFileName);
	//DbgLog((LOG_TRACE, 0, TEXT(ImageRecordTargPath));
	return S_OK;

	////Sleep(1000);
 //	CComPtr<IMediaSample> pSample;
	//GetJPSample(stJPPath, X, Y, W, H, &pSample);

	//////DEBUGGING
	////LPBYTE tmp = NULL;
	////pSample->GetPointer(&tmp);
	////LPBYTE ConversionTarget = 0;
	////ConversionTarget = (LPBYTE)malloc(720 * 486 * 3);
	////CSCManual::UYVYToRGB24(tmp, 720, 486, ConversionTarget);
	//////CSCManual::YUY2ToRGB24(tmp, 720, 486, ConversionTarget);
	////NotifyEvent(EC_KEYSTONE_FORCEFRAMEGRAB, (LONG_PTR)ConversionTarget, 720 * 486 * 3);
	//////DEBUGGING

	//HRESULT hr = m_pOutput->m_pInputPin->Receive(pSample);
	//return hr;
}

STDMETHODIMP CKeystone::StopRecording()
{
	SAGR_RecordingImages = false;
	return S_OK;
}

STDMETHODIMP CKeystone::SetTrialOverride(bool bOverride)
{
	//LOCK_TRIALOVERRIDE = bOverride;
	return S_OK;
}
   
STDMETHODIMP CKeystone::SendMediaTimeEvents(int bActive, int iType)
{
	MTEV_ThrowMediaTimeEvents = bActive;	
	MTEV_TypeOfMediaTimeEvents = iType;
	return S_OK;
}

STDMETHODIMP CKeystone::SetOptimizedLevel(int iOptimizedLevel)
{
	OPTM_OptimizedLevel = iOptimizedLevel;	
	return S_OK;
}

STDMETHODIMP CKeystone::SetRenderer(int iRenderer)
{
	REN_Renderer = iRenderer; // 0 = VMR9, 1 = INTENSITY, 2 = DECKLINK
	return S_OK;
}
