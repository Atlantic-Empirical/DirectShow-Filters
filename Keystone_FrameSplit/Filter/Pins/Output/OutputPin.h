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

    // Grab and release extra interfaces if required

    HRESULT CheckConnect(IPin *pPin);
    HRESULT BreakConnect();
    HRESULT CompleteConnect(IPin *pReceivePin);
	STDMETHODIMP ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt);

    // check that we can support this output type
    HRESULT CheckMediaType(const CMediaType* mtOut);

    // set the connection media type
    HRESULT SetMediaType(const CMediaType *pmt);

	//
	HRESULT	Inactive(void);

    // called from CBaseOutputPin during connection to ask for
    // the count and size of buffers we need.
    HRESULT DecideBufferSize(
                IMemAllocator * pAlloc,
                ALLOCATOR_PROPERTIES *pProp);

    // inherited from IQualityControl via CBasePin
    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

    HRESULT AlterQuality(Quality q);

	//Moved from old CKeystoneOutputPin
	IMemAllocator* GetAllocator();
    HRESULT GetMediaType(int iPosition,CMediaType *pMediaType);
	HRESULT CreateVideoMediaType(int width, int height, WORD bpp, DWORD format, REFERENCE_TIME timePerFrame, CMediaType* pMediaType);

    // Media type
public:
    CMediaType& CurrentMediaType() { return m_mt; };
};
