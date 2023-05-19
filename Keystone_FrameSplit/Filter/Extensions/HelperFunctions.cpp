#include "../Utility/appincludes.h"

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
//	CMediaType mtIn = m_pVideoOne->CurrentMediaType();
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
