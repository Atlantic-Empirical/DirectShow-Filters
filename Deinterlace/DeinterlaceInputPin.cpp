#include "stdafx.h"
#include "DeinterlaceInputPin.h"

CDeinterlaceInputPin::CDeinterlaceInputPin( 
                                            TCHAR* pObjectName,
                                            CDeinterlaceFilter* pFilter,
                                            HRESULT* phr,
                                            LPCWSTR pName
                                            ) :
    CTransformInputPin(pObjectName, pFilter, phr, pName)
{
    DbgLog((LOG_TRACE, 2, TEXT("CDeinterlaceInputPin::CDeinterlaceInputPin")));

    m_pDeinterlaceFilter = pFilter;
}

////////////////////////////////////////////////////////////////////////////
// GetAllocator
// 
// If the downstream filter has one then offer that (even if our own output
// pin is not using it yet.  If the upstream filter chooses it then we will
// tell our output pin to ReceiveAllocator).
// Else if our output pin is using an allocator then offer that.
//     ( This could mean offering the upstream filter his own allocator,
//       it could mean offerring our own
//     ) or it could mean offering the one from downstream
// Else fail to offer any allocator at all.
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CDeinterlaceInputPin::GetAllocator(IMemAllocator ** ppAllocator)
{
    if (!IsConnected())
    {
        return VFW_E_NO_ALLOCATOR;
    }

    HRESULT hr = CTransformInputPin::GetAllocator(ppAllocator);

    if (SUCCEEDED(hr)) 
    {
        ALLOCATOR_PROPERTIES Props, Actual;
        (*ppAllocator)->GetProperties(&Props);
        if (Props.cBuffers < MAX_FRAMES_IN_HISTORY+1)
        {
            Props.cBuffers = MAX_FRAMES_IN_HISTORY+1;
        }
        if (Props.cbAlign < 8)
        {
            Props.cbAlign = 8;
        }
        if (Props.cbBuffer < (long)CurrentMediaType().GetSampleSize())
        {
            Props.cbBuffer = (long)CurrentMediaType().GetSampleSize();
        }

        hr = (*ppAllocator)->SetProperties(&Props,&Actual);
        if (SUCCEEDED(hr) && Actual.cBuffers < MAX_FRAMES_IN_HISTORY+1)
        {
            hr = E_FAIL;
        }
    }

    return hr;

}

/////////////////////////////////////////////////////////////////////////////
// NotifyAllocator
// 
// Get told which allocator the upstream output pin is actually going to use 
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDeinterlaceInputPin::NotifyAllocator(IMemAllocator * pAllocator, BOOL bReadOnly)
{
    HRESULT hr = CTransformInputPin::NotifyAllocator(pAllocator, bReadOnly);
    if (SUCCEEDED(hr)) 
    {
        ALLOCATOR_PROPERTIES Props;
        m_pAllocator->GetProperties(&Props);
        if (Props.cBuffers < MAX_FRAMES_IN_HISTORY+1)
        {
            hr = E_FAIL;
        }
    }

    return hr;

}

/////////////////////////////////////////////////////////////////////////////
// GetAllocatorRequirements
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDeinterlaceInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps)
{
    DbgLog((LOG_TRACE, 2, TEXT("CDeinterlaceInputPin::GetAllocatorRequirements")));

    pProps->cbAlign = 8;
    pProps->cbBuffer = 0;
    pProps->cbPrefix = 0;
    pProps->cBuffers = MAX_FRAMES_IN_HISTORY+4;  // JBC 9/20/01 we drop frames if too few buffers
    
    return NOERROR;
}
