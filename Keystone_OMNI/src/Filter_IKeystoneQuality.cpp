#include "appincludes.h"
#include "Filter_IKeystoneQuality.h"

STDMETHODIMP CKeystone::get_TargetFR_Out(float *pTargetFR)
{
    CheckPointer(pTargetFR,E_POINTER);
	if (FRRT_TargetFramerate_Out_ATPF == 0)
	{
		*pTargetFR = 0;
	}
	else
	{
		float TenM = 10000000;
		float fFR = TenM / FRRT_TargetFramerate_Out_ATPF;
		*pTargetFR = fround(fFR, 3);
	}
	return NOERROR;
}

STDMETHODIMP CKeystone::get_TargetFR_Out_ATPF(LONGLONG *pTargetFR_ATPF)
{
    CheckPointer(pTargetFR_ATPF,E_POINTER);
    *pTargetFR_ATPF = FRRT_TargetFramerate_Out_ATPF;
    return NOERROR;

}

STDMETHODIMP CKeystone::get_TargetFR_In(float *pTargetFR)
{
    CheckPointer(pTargetFR,E_POINTER);

	if (FRRT_TargetFramerate_In_ATPF == 0)
	{
		*pTargetFR = 0;
	}
	else
	{
		float TenM = 10000000;
		float fFR = TenM / FRRT_TargetFramerate_In_ATPF;
		*pTargetFR = fround(fFR, 3);
	}
	return NOERROR;
}

STDMETHODIMP CKeystone::get_TargetFR_In_ATPF(LONGLONG *pTargetFR_ATPF)
{
    CheckPointer(pTargetFR_ATPF,E_POINTER);
    *pTargetFR_ATPF = FRRT_TargetFramerate_In_ATPF;
    return NOERROR;
}

STDMETHODIMP CKeystone::get_ActualFR_Out(float *pActualFR)
{
    CheckPointer(pActualFR,E_POINTER);
    *pActualFR = FRRT_ActualFramerate_Out;
    return NOERROR;
}

STDMETHODIMP CKeystone::get_ActualFR_Out_ATPF(LONGLONG *pActualFR_ATPF)
{
    CheckPointer(pActualFR_ATPF,E_POINTER);
	if (FRRT_ActualFramerate_Out == 0)
	{
		*pActualFR_ATPF = 0;
	}
	else
	{
		*pActualFR_ATPF = iround(10000000 / FRRT_ActualFramerate_Out);
	}
    return NOERROR;
}

STDMETHODIMP CKeystone::get_ActualFR_In(float *pActualFR)
{
	//TODO: 2) the filter needs to be calculating ActualFR and putting it in dActualFR
    CheckPointer(pActualFR,E_POINTER);
    *pActualFR = FRRT_ActualFramerate_In;
    return NOERROR;
}

STDMETHODIMP CKeystone::get_ActualFR_In_ATPF(LONGLONG *pActualFR_ATPF)
{
    CheckPointer(pActualFR_ATPF,E_POINTER);
	if (FRRT_ActualFramerate_In == 0)
	{
		*pActualFR_ATPF = 0;
	}
	else
	{
		*pActualFR_ATPF = iround(10000000 / FRRT_ActualFramerate_In);
	}
    return NOERROR;
}

STDMETHODIMP CKeystone::get_Jitter_In(LONGLONG *InputJitter)
{
	return S_OK;
}

STDMETHODIMP CKeystone::get_Jitter_Out(LONGLONG *OutputJitter)
{
	return S_OK;
}
