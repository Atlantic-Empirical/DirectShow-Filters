#pragma once
#include <streams.h>

// ==================================================
// Implements the output pin
// ==================================================

class CKeystoneOutputPin : public CBaseOutputPin
{
    friend class CKeystone;

protected:
    CKeystone *m_pTransformFilter;

public:

    // implement IMediaPosition by passing upstream
    IUnknown * m_pPosition;

    CKeystoneOutputPin(
        TCHAR *pObjectName,
        CKeystone *pTransformFilter,
        HRESULT * phr,
        LPCWSTR pName);
#ifdef UNICODE
    CKeystoneOutputPin(
        CHAR *pObjectName,
        CKeystone *pTransformFilter,
        HRESULT * phr,
        LPCWSTR pName);
#endif
    ~CKeystoneOutputPin();

    // override to expose IMediaPosition
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

    // --- CBaseOutputPin ------------

    STDMETHODIMP QueryId(LPWSTR * Id)
    {
        return AMGetWideString(L"Output", Id);
    }

	// ========================================================================================================
	// DIRECTSHOW
	// ===============================================================

	// CBaseOutputPin
    HRESULT CheckConnect(IPin *pPin);
    HRESULT BreakConnect();
    HRESULT CompleteConnect(IPin *pReceivePin);
	STDMETHODIMP ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt);
    HRESULT CheckMediaType(const CMediaType* mtOut);
    HRESULT SetMediaType(const CMediaType *pmt);
    HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES *pProp);

    // IQualityControl
    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);
    HRESULT AlterQuality(Quality q);

	// Allocator
	IMemAllocator* GetAllocator();
    //  This just saves the allocator being used on the output pin
    //  Also called by input pin's GetAllocator()
    void SetAllocator(IMemAllocator * pAllocator);
	HRESULT GetMediaType(int iPosition,CMediaType *pMediaType);

public:

	// ========================================================================================================
	// SMT :: OUTPUT PIN :: MEDIATYPE
	// ===============================================================

	CMediaType& CurrentMediaType() { return m_mt; };
	//HRESULT CreateVideoMediaType(int width, int height, WORD bpp, DWORD format, REFERENCE_TIME timePerFrame, CMediaType* pMediaType);

	// ========================================================================================================
	// SMT :: OUTPUT PIN :: GENERAL PROPERTIES
	// ===============================================================

	CBaseRenderer * m_pRenderer;

};
