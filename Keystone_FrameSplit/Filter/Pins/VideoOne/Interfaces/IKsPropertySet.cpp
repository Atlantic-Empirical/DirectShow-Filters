//#include "../VideoPin.h" //include the header for the input pin
#include <appincludes.h>

#define AM_PROPERTY_COPY_MACROVISION	0x05

// IKsPropertySet

STDMETHODIMP CKeystoneVideoOnePin::Set(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength)
{
	if(PropSet == AM_KSPROPSETID_CopyProt && Id == AM_PROPERTY_COPY_MACROVISION)
	{
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: MacroVision level: %d"), (int) (*(LPBYTE) pPropertyData)));
		m_pTransformFilter->NotifyEvent(EC_KEYSTONE_MACROVISION, (int) (*(LPBYTE) pPropertyData), 0);
		return S_OK;
	}
	return S_OK;
}

STDMETHODIMP CKeystoneVideoOnePin::Get(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength, ULONG* pBytesReturned)
{
	if(PropSet == AM_KSPROPSETID_CopyProt && Id == AM_PROPERTY_COPY_MACROVISION)
	{
		return S_OK;
	}
	return S_OK;
}

STDMETHODIMP CKeystoneVideoOnePin::QuerySupported(REFGUID PropSet, ULONG Id, ULONG* pTypeSupport)
{
	if(PropSet == AM_KSPROPSETID_CopyProt && Id == AM_PROPERTY_COPY_MACROVISION)
	{
		*pTypeSupport = KSPROPERTY_SUPPORT_SET;
		return S_OK;
	}
	return S_OK;

}

////#include <atlbase.h>

//if(CComQIPtr<IKsPropertySet> pKsPS = m_pTransformFilter->m_pOutput->GetConnected())
//{
//	if(PropSet == AM_KSPROPSETID_CopyProt && Id == AM_PROPERTY_COPY_MACROVISION
//	/*&& DataLength == 4 && *(DWORD*)pPropertyData*/)
//	{
//		//TRACE(_T("Oops, no-no-no, no macrovision please\n"));
//		return S_OK;
//	}

//	return pKsPS->Set(PropSet, Id, pInstanceData, InstanceLength, pPropertyData, DataLength);
//}

//return E_UNEXPECTED;


//if(CComQIPtr<IKsPropertySet> pKsPS = m_pTransformFilter->m_pOutput->GetConnected())
//{
//	return pKsPS->Get(PropSet, Id, pInstanceData, InstanceLength, pPropertyData, DataLength, pBytesReturned);
//}
//
//return E_UNEXPECTED;	


//*pTypeSupport = KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET;


//if(CComQIPtr<IKsPropertySet> pKsPS = m_pTransformFilter->m_pOutput->GetConnected())
//{
//	return pKsPS->QuerySupported(PropSet, Id, pTypeSupport);
//}
//
//return E_UNEXPECTED;

//// 0E8A0A40-6AEF-11D0-9ED0-00A024CA19B3
//OUR_GUID_ENTRY(AM_KSPROPSETID_CopyProt,
//0x0E8A0A40, 0x6AEF, 0x11D0, 0x9E, 0xD0, 0x00, 0xA0, 0x24, 0xCA, 0x19, 0xB3)

//#ifndef STATIC_IID_IKsPropertySet
//#define STATIC_IID_IKsPropertySet\
//    0x31EFAC30L, 0x515C, 0x11d0, 0xA9, 0xAA, 0x00, 0xAA, 0x00, 0x61, 0xBE, 0x93
//#endif // STATIC_IID_IKsPropertySet
 
 
