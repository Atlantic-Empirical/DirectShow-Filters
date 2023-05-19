#include "../../utility/appincludes.h"
#include "IKeystoneQuality.h"

STDMETHODIMP CKeystone::get_TargetFR_Out(float *pTargetFR)
{
    CheckPointer(pTargetFR,E_POINTER);
	//AvgTimePerFrame = 10,000,000 / FrameRate

	if (llTargetFR_Out_ATPF == 0)
	{
		*pTargetFR = 0;
	}
	else
	{
		float TenM = 10000000;
		float fFR = TenM / llTargetFR_Out_ATPF;
		*pTargetFR = fround(fFR, 3);
	}
	return NOERROR;
} // get_TargetFR

STDMETHODIMP CKeystone::get_TargetFR_Out_ATPF(LONGLONG *pTargetFR_ATPF)
{
	//OLD WAY
    CheckPointer(pTargetFR_ATPF,E_POINTER);
    *pTargetFR_ATPF = llTargetFR_Out_ATPF;

	////NEW WAY
	//CMediaType *mSource = 0;
	//HRESULT hr = m_pOutput->ConnectionMediaType(((AM_MEDIA_TYPE*) mSource));
	//if (FAILED(hr)) return hr;
	//VIDEOINFOHEADER2 *vih2 = (VIDEOINFOHEADER2*)mSource->Format();
	//*pTargetFR_ATPF = vih2->AvgTimePerFrame;
    return NOERROR;

} // get_TargetFR_ATPF

STDMETHODIMP CKeystone::get_TargetFR_In(float *pTargetFR)
{
    CheckPointer(pTargetFR,E_POINTER);
	//AvgTimePerFrame = 10,000,000 / FrameRate

	if (llTargetFR_In_ATPF == 0)
	{
		*pTargetFR = 0;
	}
	else
	{
		float TenM = 10000000;
		float fFR = TenM / llTargetFR_In_ATPF;
		*pTargetFR = fround(fFR, 3);
	}
	return NOERROR;
} // get_TargetFR

STDMETHODIMP CKeystone::get_TargetFR_In_ATPF(LONGLONG *pTargetFR_ATPF)
{
    CheckPointer(pTargetFR_ATPF,E_POINTER);
    *pTargetFR_ATPF = llTargetFR_In_ATPF;
    return NOERROR;

} // get_TargetFR_ATPF

STDMETHODIMP CKeystone::get_ActualFR_Out(float *pActualFR)
{
    CheckPointer(pActualFR,E_POINTER);
    *pActualFR = fActualFR_Out;
    return NOERROR;

} // get_ActualFR

STDMETHODIMP CKeystone::get_ActualFR_Out_ATPF(LONGLONG *pActualFR_ATPF)
{
    CheckPointer(pActualFR_ATPF,E_POINTER);
	if (fActualFR_Out == 0)
	{
		*pActualFR_ATPF = 0;
	}
	else
	{
		*pActualFR_ATPF = iround(10000000 / fActualFR_Out);
	}
    return NOERROR;
} // get_ActualFR_ATPF

STDMETHODIMP CKeystone::get_ActualFR_In(float *pActualFR)
{
	//TODO: 2) the filter needs to be calculating ActualFR and putting it in dActualFR
    CheckPointer(pActualFR,E_POINTER);
    *pActualFR = fActualFR_In;
    return NOERROR;

} // get_ActualFR

STDMETHODIMP CKeystone::get_ActualFR_In_ATPF(LONGLONG *pActualFR_ATPF)
{
    CheckPointer(pActualFR_ATPF,E_POINTER);
	if (fActualFR_In == 0)
	{
		*pActualFR_ATPF = 0;
	}
	else
	{
		*pActualFR_ATPF = iround(10000000 / fActualFR_In);
	}
    return NOERROR;
} // get_ActualFR_ATPF

STDMETHODIMP CKeystone::get_Jitter_In(LONGLONG *InputJitter)
{
	return S_OK;
}

STDMETHODIMP CKeystone::get_Jitter_Out(LONGLONG *OutputJitter)
{
	return S_OK;
}
