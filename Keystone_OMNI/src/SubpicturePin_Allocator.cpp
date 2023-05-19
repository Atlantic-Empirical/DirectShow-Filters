#include "appincludes.h"
#include "SubpicturePin_Allocator.h"

HRESULT CSPAllocator::GetBuffer(IMediaSample **ppBuffer, REFERENCE_TIME *pStartTime, REFERENCE_TIME *pEndTime, DWORD dwFlags )
{
    UNREFERENCED_PARAMETER(pStartTime);
    UNREFERENCED_PARAMETER(pEndTime);
    UNREFERENCED_PARAMETER(dwFlags);
    CMediaSample *pSample;

    *ppBuffer = NULL;
    for (;;)
    {
        {  // scope for lock
            CAutoLock cObjectLock(this);

            /* Check we are committed */
            if (!m_bCommitted) {
                return VFW_E_NOT_COMMITTED;
            }
            pSample = (CMediaSample *) m_lFree.RemoveHead();
            if (pSample == NULL) {
                SetWaiting();
            }
        }

        /* If we didn't get a sample then wait for the list to signal */

        if (pSample) {
            break;
        }
        if (dwFlags & AM_GBF_NOWAIT) {
            return VFW_E_TIMEOUT;
        }
        ASSERT(m_hSem != NULL);
        WaitForSingleObject(m_hSem, INFINITE);
    }

    /* Addref the buffer up to one. On release
       back to zero instead of being deleted, it will requeue itself by
       calling the ReleaseBuffer member function. NOTE the owner of a
       media sample must always be derived from CBaseAllocator */


    ASSERT(pSample->m_cRef == 0);
    pSample->m_cRef = 1;
    *ppBuffer = pSample;

    return NOERROR;
}

HRESULT CSPAllocator::Alloc( )
{
    // look at the base class code to see where this came from!

    CAutoLock lck(this);

    // Check he has called SetProperties
    HRESULT hr = CBaseAllocator::Alloc();
    if (FAILED(hr)) {
        return hr;
    }

    // If the requirements haven't changed then don't reallocate
    if (hr == S_FALSE) {
        ASSERT(m_pBuffer);
        return NOERROR;
    }
    ASSERT(hr == S_OK); // we use this fact in the loop below

    // Free the old resources
    if (m_pBuffer) {
        ReallyFree();
    }

    // Compute the aligned size
    LONG lAlignedSize = m_lSize + m_lPrefix;
    if (m_lAlignment > 1) 
    {
        LONG lRemainder = lAlignedSize % m_lAlignment;
        if (lRemainder != 0) 
        {
            lAlignedSize += (m_lAlignment - lRemainder);
        }
    }

    // Create the contiguous memory block for the samples
    // making sure it's properly aligned (64K should be enough!)
    ASSERT(lAlignedSize % m_lAlignment == 0);

    m_pBuffer = (PBYTE)VirtualAlloc(NULL,
                    m_lCount * lAlignedSize,
                    MEM_COMMIT,
                    PAGE_READWRITE);

    if (m_pBuffer == NULL) {
        return E_OUTOFMEMORY;
    }

    LPBYTE pNext = m_pBuffer;
    CMediaSample *pSample;

    ASSERT(m_lAllocated == 0);

    // Create the new samples - we have allocated m_lSize bytes for each sample
    // plus m_lPrefix bytes per sample as a prefix. We set the pointer to
    // the memory after the prefix - so that GetPointer() will return a pointer
    // to m_lSize bytes.
    for (; m_lAllocated < m_lCount; m_lAllocated++, pNext += lAlignedSize) 
    {
        pSample = new CMediaSample(
                                NAME("dec-key sp sample"),
                                this,
                                &hr,
                                pNext + m_lPrefix,      // GetPointer() value
                                m_lSize);               // not including prefix

		//Tom's Hail Mary
		CMediaType * mt = new CMediaType();
		m_pPin->CreateSubpictureMediaType(mt);
		pSample->SetMediaType((AM_MEDIA_TYPE*) mt);

		//DEBUGGING
//		//VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*) pSample->GetMediaType();
//		CMediaType * mt = NULL;
//		pSample->GetMediaType((AM_MEDIA_TYPE**) &mt);
//		if (mt)
//		{
//			goto MoveOn;	
//		}
//		else
//		{
//			goto MoveOn;	
//		}
//MoveOn:
		//DEBUGGING

        //End Hail Mary

        ASSERT(SUCCEEDED(hr));
        if (pSample == NULL)
            return E_OUTOFMEMORY;

        // This CANNOT fail
        m_lFree.Add(pSample);
    }

    m_bChanged = FALSE;
    return NOERROR;
}


//----------------------------------------------------------------------------
// don't really free the memory
//----------------------------------------------------------------------------

void CSPAllocator::ReallyFree()
{
    // look at the base class code to see where this came from!

    // Should never be deleting this unless all buffers are freed

    ASSERT(m_lAllocated == m_lFree.GetCount());

    // Free up all the CMediaSamples

    CMediaSample *pSample;
    for (;;) 
    {
        pSample = m_lFree.RemoveHead();
        if (pSample != NULL) 
        {
            delete pSample;
        } 
        else 
        {
            break;
        }
    }

    m_lAllocated = 0;

    // don't free the buffer - let the app do it
}


//----------------------------------------------------------------------------
// SetProperties: Called by the upstream filter to set the allocator
// properties. The application has already allocated the buffer, so we reject 
// anything that is not compatible with that, and return the actual props.
//----------------------------------------------------------------------------

HRESULT CSPAllocator::SetProperties(ALLOCATOR_PROPERTIES *pRequest, ALLOCATOR_PROPERTIES *pActual)
{
    HRESULT hr = CMemAllocator::SetProperties(pRequest, pActual);

    if (FAILED(hr))
    {
        return hr;
    }
    
    ALLOCATOR_PROPERTIES *pRequired = &(m_pPin->m_allocprops);
    if (pRequest->cbAlign != pRequired->cbAlign)
    {
        return VFW_E_BADALIGN;
    }
    if (pRequest->cbPrefix != pRequired->cbPrefix)
    {
        return E_FAIL;
    }
    if (pRequest->cbBuffer > pRequired->cbBuffer)
    {
        return E_FAIL;
    }
    if (pRequest->cBuffers > pRequired->cBuffers)
    {
        return E_FAIL;
    }

    *pActual = *pRequired;

    m_lCount = pRequired->cBuffers;
    m_lSize = pRequired->cbBuffer;
    m_lAlignment = pRequired->cbAlign;
    m_lPrefix = pRequired->cbPrefix;

    return S_OK;
}
