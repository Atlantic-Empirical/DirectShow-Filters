#include <appincludes.h>
#include "../VideoTwoPin.h"

HRESULT CKeystoneVideoTwoPin::ProcessVarispeedSample(IMediaSample * pVSSample)
{
	//Set time stamps
	//Beep(1000, 1);
	REFERENCE_TIME rtStart, rtEnd;
	CRefTime crtStreamTime;
	m_pTransformFilter->StreamTime(crtStreamTime);
	rtStart = crtStreamTime.m_time; //+ 100000;
	rtEnd = rtStart + m_pTransformFilter->llTargetFR_Out_ATPF;
	pVSSample->SetTime(&rtStart, &rtEnd);
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Varispeed sample.")));
	return ReceiveStep2(pVSSample);
	//return S_OK;
}

HRESULT CKeystoneVideoTwoPin::ActivateVarispeed(double Speed)
{
	bInVarispeed = true;
	MSVarispeedInterval = (((int)m_pTransformFilter->llTargetFR_In_ATPF) * Speed) / 10000;
	MSVarispeedInterval = fround(MSVarispeedInterval, 0);
	return S_OK;
}

HRESULT CKeystoneVideoTwoPin::DeactivateVarispeed()
{
	bInVarispeed = false;
	MSVarispeedInterval = m_pTransformFilter->llTargetFR_In_ATPF;
	return S_OK;
}
