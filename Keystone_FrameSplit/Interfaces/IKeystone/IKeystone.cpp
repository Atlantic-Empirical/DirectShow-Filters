#include "../../utility/appincludes.h"
#include "iKeystone.h"
#include <atlbase.h>

STDMETHODIMP CKeystone::get_InputOneMediaType(CMediaType *InputMediaType  )
{
    CheckPointer(InputMediaType,E_POINTER);
	*InputMediaType = m_pVideoOne->CurrentMediaType();
    return NOERROR;
} // get_InputMediaType

STDMETHODIMP CKeystone::get_InputTwoMediaType(CMediaType *InputMediaType  )
{
    CheckPointer(InputMediaType,E_POINTER);
	*InputMediaType = m_pVideoTwo->CurrentMediaType();
    return NOERROR;
} // get_InputMediaType

STDMETHODIMP CKeystone::get_OutputMediaType(CMediaType *OutputMediaType  )
{
    CheckPointer(OutputMediaType,E_POINTER);
	*OutputMediaType = m_pOutput->CurrentMediaType();
    return NOERROR;
} // get_OutputMediaType

STDMETHODIMP CKeystone::get_InputWidth(long *InputWidth)
{
    CheckPointer(InputWidth,E_POINTER);
	if(m_pVideoOne->IsConnected() != TRUE)
	{
		*InputWidth = 0;
	}
	else
	{
		CMediaType* pmt = &m_pVideoOne->CurrentMediaType();
		VIDEOINFOHEADER2* pvi = (VIDEOINFOHEADER2*)pmt->Format();
		*InputWidth = pvi->bmiHeader.biWidth;
		//DeleteMediaType((AM_MEDIA_TYPE*) pmt);
	}
	return S_OK;
} // get_InputWidth

STDMETHODIMP CKeystone::get_InputHeight(long *InputHeight)
{
    CheckPointer(InputHeight,E_POINTER);
	if(m_pVideoOne->IsConnected() != TRUE)
	{
		*InputHeight = 0;
	}
	else
	{
		CMediaType* pmt = &m_pVideoOne->CurrentMediaType();
		VIDEOINFOHEADER2* pvi = (VIDEOINFOHEADER2*)pmt->Format();
		*InputHeight = pvi->bmiHeader.biHeight;
		//DeleteMediaType(pmt);
	}
	return S_OK;
} // get_InputHeight

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
		//DeleteMediaType(pmt);
	}
	return S_OK;
} // get_OutputWidth

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
		//DeleteMediaType(pmt);
	}
	return S_OK;
} // get_OutputHeight

STDMETHODIMP CKeystone::GrabSample(int in_SampleWhat, LPBYTE *out_pSample, long *out_lSampleSize, long *out_Width, long *out_Height)
{
	if (bSampleOperationUnderway == true)
	{		
		//Can only do one at a time
		return E_ACCESSDENIED;
	}

	bSampleOperationUnderway = true;
	SampleWhat = in_SampleWhat;
	bGetSample = true;
	pSample = NULL;

	//if (SampleWhat == 3)
	//{
	//	HandleSampleGrab_LastOutput();
	//	goto SkipAhead;
	//}

	DWORD dwSampStartTicks = GetTickCount();
	while (pSample == NULL)
	{
		Sleep(1);
		//Just wait until the sample is created, up to three seconds then return control to app.
		if (GetTickCount() - dwSampStartTicks > 3000)
		{
			//Timeout abort
			bSampleOperationUnderway = false;
			pSample = NULL;
			return E_ABORT;
		}
	}
	if (pSample == 0)
	{
		//HandleSampleGrab did not succeed.
		return E_FAIL;
	}

SkipAhead:
	//Now, the sample buffer is available
	*out_pSample = pSample;
	*out_lSampleSize = lSampleSize;
	*out_Width = lSamp_W;
	*out_Height = lSamp_H;

	bSampleOperationUnderway = false;
	SampleWhat = NULL;
	pSample = NULL;
	lSampleSize = 0;
	lSamp_W = 0;
	lSamp_H = 0;	
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
		bFilterIsLocked = false;
		return S_OK;
	}
	if (*FilterKey == CLSID_Key_Development)
	{
		bFilterIsLocked = false;
		return S_OK;
	}
	if (*FilterKey == CLSID_Key_Client1)
	{
		bFilterIsLocked = false;
		return S_OK;
	}
	bFilterIsLocked = true;
	return S_OK;
} // UnlockFilter

STDMETHODIMP CKeystone::put_FeedbackClicks(bool DoClicks)
{
	////DEBUGGING
	//while (m_pVideoOne->m_cRef > 0)
	//{
	//	m_pVideoOne->Release();
	//}
 //	delete m_pVideoOne;

	//while (m_pOutput->m_cRef > 0)
	//{
	//	m_pOutput->Release();
	//}
 //	delete m_pOutput;

	//while (m_pSubpicture->m_cRef > 0)
	//{
	//	m_pSubpicture->Release();
	//}
 //	delete m_pSubpicture;

	//while (m_pVideoTwo->m_cRef > 0)
	//{
	//	m_pVideoTwo->Release();
	//}
 //	delete m_pVideoTwo;
	////DEBUGGING

	bFeedbackClicks = DoClicks;
	return S_OK;
} // put_FeedbackClicks

STDMETHODIMP CKeystone::get_FeedbackClicks(bool *DoClicks)
{
    CheckPointer(DoClicks,E_POINTER);
	if (bFeedbackClicks == true)
	{
		*DoClicks = true;
	}
	else
	{
		*DoClicks = false;
	}
	//*DoClicks = bFeedbackClicks;
	return S_OK;
} // get_FeedbackClicks

STDMETHODIMP CKeystone::Set32Status(int Do32)
{
	m_pVideoOne->bRun32 = Do32;
	return S_OK;
}

STDMETHODIMP CKeystone::FrameStep(bool bForward)
{
	//if (m_pVideoOne->bFrameStepUnderway) return S_OK; //we're busy
	//Gabor's new way
	if (!m_pVideoOne->bFrameStepping)
	{
		m_pVideoOne->bFrameStepping = true;
	}
	else
	{
		m_pVideoOne->bDeliverFrameStepSample = true;
	}
	return S_OK;
}

STDMETHODIMP CKeystone::QuitFrameStepping()
{
	//Gabor's new way
	m_pVideoOne->bFrameStepping = false;
	m_pVideoOne->bSendNewSegment;
	m_pVideoOne->bRestartTimeStamps = true;
	return S_OK;
}

STDMETHODIMP CKeystone::ActivateVarispeed(double Speed)
{
	m_pVideoOne->ActivateVarispeed(Speed);
	return S_OK;
}

STDMETHODIMP CKeystone::DeactivateVarispeed()
{
	m_pVideoOne->DeactivateVarispeed();
	return S_OK;
}

STDMETHODIMP CKeystone::SetL21State(bool bL21Active)
{
	//m_pVideoTwo->bL21StreamIsActive = bL21Active;
	return S_OK;
}

STDMETHODIMP CKeystone::ShowJacketPicture(LPCTSTR stJPPath, int X, int Y, int W, int H)
{
	//Sleep(1000);
 	CComPtr<IMediaSample> pSample;
	GetJPSample(stJPPath, X, Y, W, H, &pSample);

	////DEBUGGING
	//LPBYTE tmp = NULL;
	//pSample->GetPointer(&tmp);
	//LPBYTE ConversionTarget = 0;
	//ConversionTarget = (LPBYTE)malloc(720 * 486 * 3);
	//CSCManual::UYVYToRGB24(tmp, 720, 486, ConversionTarget);
	////CSCManual::YUY2ToRGB24(tmp, 720, 486, ConversionTarget);
	//NotifyEvent(EC_KEYSTONE_FORCEFRAMEGRAB, (LONG_PTR)ConversionTarget, 720 * 486 * 3);
	////DEBUGGING

	HRESULT hr = m_pOutput->m_pInputPin->Receive(pSample);
	return hr;
}

STDMETHODIMP CKeystone::ShowBitmap(LPBYTE pBMP, int W, int H, int X, int Y, int Format)
{
	//Sleep(1000);
 	CComPtr<IMediaSample> pSample;
	GetBMPSample(pBMP, X, Y, W, H, &pSample);

	////DEBUGGING
	//LPBYTE tmp = NULL;
	//pSample->GetPointer(&tmp);
	//LPBYTE ConversionTarget = 0;
	//ConversionTarget = (LPBYTE)malloc(720 * 486 * 3);
	//CSCManual::UYVYToRGB24(tmp, 720, 486, ConversionTarget);
	////CSCManual::YUY2ToRGB24(tmp, 720, 486, ConversionTarget);
	//NotifyEvent(EC_KEYSTONE_FORCEFRAMEGRAB, (LONG_PTR)ConversionTarget, 720 * 486 * 3);
	////DEBUGGING

	HRESULT hr = m_pOutput->m_pInputPin->Receive(pSample);
	//pSample->Release();
	return S_OK;
}

STDMETHODIMP CKeystone::SaveNextXFrames(int Count, LPCTSTR stDumpPath)
{
	iDumpFrameCount = Count;
	sDumpFrameLocation = stDumpPath;
	DWORD dwSampStartTicks = GetTickCount();
	DWORD dwEndTicks = dwSampStartTicks + (99 * Count);
	DWORD dwCurrentTicks = dwSampStartTicks;
	while ((iDumpFrameCount > 0) & (dwCurrentTicks < dwEndTicks))
	{
		dwCurrentTicks = GetTickCount();
		Sleep(1000);
	}
	int FramesOutput = (Count - iDumpFrameCount);
	iDumpFrameCount = 0;
	return FramesOutput;
}

STDMETHODIMP CKeystone::ForceOutputConnectSize(int W, int H )
{
	bForceOutputConnectSize = true;
	iForceOutput_W = W;
	iForceOutput_H = H;
	return S_OK;
}

STDMETHODIMP CKeystone::ShowUYVYFile(LPCTSTR stYUVPath, int X, int Y, int W, int H)
{
	FILE* f = _tfopen(stYUVPath, _T("rb"));
	if(!f) return E_FAIL;
	YUVFileLength = W*H*2;
	YUVFile = (LPBYTE)malloc(YUVFileLength);
	int BytesRead = fread(YUVFile, 1, YUVFileLength, f);
	if (BytesRead < YUVFileLength)
	{
		//Not enough bytes read
		return E_FAIL;
	}
	bDoingYUVFile = true;
	return S_OK;
}

STDMETHODIMP CKeystone::ClearUYVYFile()
{
	free(YUVFile);
	bDoingYUVFile = false;
	return S_OK;
}

STDMETHODIMP CKeystone::ClearUYVYFile_A()
{
	bDoingYUVFile = false;
	return S_OK;
}

STDMETHODIMP CKeystone::ShowUYVYBuffer(LPBYTE pUYVY, int W, int H, int X, int Y)
{
	YUVFileLength = W*H*2;
	YUVFile = pUYVY;
	bDoingYUVFile = true;
	return S_OK;
}

STDMETHODIMP CKeystone::Pause(int nPause)
{
	Pausing = nPause;
	return S_OK;
}

STDMETHODIMP CKeystone::ActivateFFRW()
{
	m_pVideoOne->bInFFRW = true;
	return S_OK;
}

STDMETHODIMP CKeystone::DeactivateFFRW()
{
	m_pVideoOne->bInFFRW = false;
	return S_OK;
}

STDMETHODIMP CKeystone::ResendLastSamp()
{
	ResendSample();
	return S_OK;
}

STDMETHODIMP CKeystone::StartRecording(LPCWSTR pszFileName)
{
	bRecordingImages = true;
	HRESULT hr = ::SHStrDupW(pszFileName, &ImageRecordTargPath);
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
	bRecordingImages = false;
	return S_OK;
}
