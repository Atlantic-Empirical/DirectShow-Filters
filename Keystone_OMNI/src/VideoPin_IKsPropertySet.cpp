#include "appincludes.h"

#define AM_PROPERTY_COPY_MACROVISION	0x05

// IKsPropertySet

STDMETHODIMP CKeystoneVideoPin::Set(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength)
{
	if(PropSet == AM_KSPROPSETID_CopyProt && Id == AM_PROPERTY_COPY_MACROVISION)
	{
		DbgLog((LOG_TRACE, 0, TEXT("Keystone: MacroVision level: %d"), (int) (*(LPBYTE) pPropertyData)));
		m_pTransformFilter->NotifyEvent(EC_KEYSTONE_MACROVISION, (int) (*(LPBYTE) pPropertyData), 0);
		return S_OK;
	}
	return S_OK;
}

STDMETHODIMP CKeystoneVideoPin::Get(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength, ULONG* pBytesReturned)
{
	if(PropSet == AM_KSPROPSETID_CopyProt && Id == AM_PROPERTY_COPY_MACROVISION)
	{
		return S_OK;
	}
	return S_OK;
}

STDMETHODIMP CKeystoneVideoPin::QuerySupported(REFGUID PropSet, ULONG Id, ULONG* pTypeSupport)
{
	if(PropSet == AM_KSPROPSETID_CopyProt && Id == AM_PROPERTY_COPY_MACROVISION)
	{
		*pTypeSupport = KSPROPERTY_SUPPORT_SET;
		return S_OK;
	}
	return S_OK;

}
