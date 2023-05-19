#pragma once
#include <streams.h>
#include "vmr9.h"
#include "SubpicturePin_Allocator.h"

// ==================================================
// Implements the Subpicture input pin
// ==================================================

class CKeystoneSubpicturePin : 
								public CBaseInputPin,
								public IVMRVideoStreamControl9
{
    friend class CKeystone;
	friend class SPAllocator;

public:
	//Custom Allocator Stuff
	CSPAllocator * m_pPrivateAllocator;
    ALLOCATOR_PROPERTIES m_allocprops;
	BYTE * m_pBuffer;

    // we override this to tell whoever's upstream of us what kind of
    // properties we're going to demand to have
    //
	STDMETHODIMP GetAllocatorRequirements( ALLOCATOR_PROPERTIES *pProps );
	STDMETHODIMP GetAllocator(IMemAllocator **ppAllocator);

protected:
    CKeystone *m_pTransformFilter;
    CCritSec m_csReceiveSubpicture;

public:
    CKeystoneSubpicturePin(
        TCHAR *pObjectName,
        CKeystone *pTransformFilter,
        HRESULT * phr,
        LPCWSTR pName);
#ifdef UNICODE
    CKeystoneSubpicturePin(
        char *pObjectName,
        CKeystone *pTransformFilter,
        HRESULT * phr,
        LPCWSTR pName);
#endif

	DECLARE_IUNKNOWN

    STDMETHODIMP QueryId(LPWSTR * Id)
    {
        return AMGetWideString(L"Subpicture", Id);
    }

	// ========================================================================================================
	// DIRECTSHOW
	// ===============================================================

	// CBaseInputPin
    HRESULT CheckConnect(IPin *pPin);
    HRESULT BreakConnect();
    HRESULT CompleteConnect(IPin *pReceivePin);
    HRESULT CheckMediaType(const CMediaType* mtIn);
    HRESULT SetMediaType(const CMediaType* mt);
	HRESULT GetMediaType(int iPosition, CMediaType* pMediaType);
	STDMETHODIMP QueryAccept(const AM_MEDIA_TYPE* pmt);
	STDMETHODIMP ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt);
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // IMemInputPin
    STDMETHODIMP Receive(IMediaSample * pSample);
    STDMETHODIMP EndOfStream(void);
    STDMETHODIMP BeginFlush(void);
    STDMETHODIMP EndFlush(void);
    STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
	STDMETHODIMP NotifyAllocator(IMemAllocator* pAllocator, BOOL bReadOnly);

	virtual HRESULT CheckStreaming();

public:

	// ========================================================================================================
	// SMT :: SUBPICTURE PIN :: MEDIATYPE
	// ===============================================================

	HRESULT CreateSubpictureMediaType(CMediaType* pMediaType);
    CMediaType& CurrentMediaType() { return m_mt; };

	// ========================================================================================================
	// SMT :: SUBPICTURE PIN ::GENERAL PROPERTIES
	// ===============================================================

	BOOL BSubpictureStreamIsActive;

	// ========================================================================================================
	// INTERFACES :: IVMRSTREAMINGCONTROL9
	// ===============================================================

	STDMETHODIMP SetStreamActiveState(BOOL fActive);
	STDMETHODIMP GetStreamActiveState(BOOL *fActive);

};
