#include "appincludes.h"
#include "VideoPin.h"

HRESULT CKeystone::VARI_ProcessVarispeedSample(IMediaSample * pVSSample)
{
	//Set time stamps
	//Beep(1000, 1);
	REFERENCE_TIME rtStart, rtEnd;
	CRefTime crtStreamTime;
	StreamTime(crtStreamTime);
	rtStart = crtStreamTime.m_time; //+ 100000;
	rtEnd = rtStart + FRRT_TargetFramerate_Out_ATPF;
	pVSSample->SetTime(&rtStart, &rtEnd);
	DbgLog((LOG_TRACE, 0, TEXT("Keystone: Varispeed sample.")));
	return Pipeline_Video_ReceiveVideo_Step2(pVSSample);
	//return S_OK;
}
