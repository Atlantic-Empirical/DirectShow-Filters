#pragma once

#include <streams.h>
//#include "..\SubpicturePin.h"

class CSPAllocator : public CMemAllocator
{
    friend class CKeystoneSubpicturePin;
    friend class CKeystone;

protected:

    // our pin who created us
    //
    CKeystoneSubpicturePin * m_pPin;

public:

    CSPAllocator( CKeystoneSubpicturePin * pParent, HRESULT *phr ) : CMemAllocator( TEXT("CSPAllocator\0"), NULL, phr ) , m_pPin( pParent )
	{
	
	};

    ~CSPAllocator( )
    {
        // wipe out m_pBuffer before we try to delete it. It's not an allocated
        // buffer, and the default destructor will try to free it!
        m_pBuffer = NULL;
    }

    HRESULT Alloc( );

    void ReallyFree();

    // Override this to reject anything that does not match the actual buffer
    // that was created by the application
    STDMETHODIMP SetProperties(ALLOCATOR_PROPERTIES *pRequest, ALLOCATOR_PROPERTIES *pActual);
	STDMETHODIMP GetBuffer(IMediaSample **ppBuffer, REFERENCE_TIME *pStartTime, REFERENCE_TIME *pEndTime, DWORD dwFlags);

};