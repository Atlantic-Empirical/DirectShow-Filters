#pragma once
#include <streams.h>
#include <mpconfig.h>
#include <vmr9.h>

// ==================================================
// Implements the Line21 input pin
// ==================================================

class CKeystoneLine21Pin : public CBaseInputPin, IVMRVideoStreamControl9
{
    friend class CKeystone;

protected:
    CKeystone *m_pTransformFilter;
    CCritSec m_csReceiveLine21;

public:
    CKeystoneLine21Pin (
        TCHAR *pObjectName,
        CKeystone *pTransformFilter,
        HRESULT * phr,
        LPCWSTR pName);
#ifdef UNICODE
    CKeystoneLine21Pin (
        char *pObjectName,
        CKeystone *pTransformFilter,
        HRESULT * phr,
        LPCWSTR pName);
#endif

	~CKeystoneLine21Pin();

	DECLARE_IUNKNOWN

    STDMETHODIMP QueryId(LPWSTR * Id)
    {
        return AMGetWideString(L"Line21", Id);
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
	HRESULT CreateLine21MediaType(CMediaType* pMediaType);
	STDMETHODIMP QueryAccept(const AM_MEDIA_TYPE* pmt);
	STDMETHODIMP ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt);
	STDMETHODIMP NotifyAllocator(IMemAllocator* pAllocator, BOOL bReadOnly);
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    // IMemInputPin

    STDMETHODIMP Receive(IMediaSample * pSample);
    STDMETHODIMP EndOfStream(void);
    STDMETHODIMP BeginFlush(void);
    STDMETHODIMP EndFlush(void);
    STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

    virtual HRESULT CheckStreaming();

public:

	// ========================================================================================================
	// SMT :: LINE-21 PIN :: MEDIATYPE
	// ===============================================================

    CMediaType& CurrentMediaType() { return m_mt; };

	// ========================================================================================================
	// SMT :: LINE-21 PIN :: GENERAL PROPERTIES
	// ===============================================================

	bool bL21StreamIsActive;
	bool L21ByteCheck(IMediaSample * pSample);
	REFERENCE_TIME LastFrameReceived_RT;

	// ========================================================================================================
	// INTERFACES :: IMIXERPINCONFIG
	// ===============================================================

	STDMETHODIMP SetRelativePosition( DWORD dwLeft, DWORD dwTop, DWORD dwRight, DWORD dwBottom );
	STDMETHODIMP GetRelativePosition( DWORD *pdwLeft, DWORD *pdwTop, DWORD *pdwRight, DWORD *pdwBottom );
	STDMETHODIMP SetZOrder( DWORD dwZOrder );
	STDMETHODIMP GetZOrder( DWORD *pdwZOrder );
	STDMETHODIMP SetColorKey( COLORKEY *pColorKey );
	STDMETHODIMP GetColorKey( COLORKEY *pColorKey, DWORD *pColor );
	STDMETHODIMP SetBlendingParameter( DWORD dwBlendingParameter );
	STDMETHODIMP GetBlendingParameter( DWORD *pdwBlendingParameter );
	STDMETHODIMP SetAspectRatioMode( AM_ASPECT_RATIO_MODE amAspectRatioMode );
	STDMETHODIMP GetAspectRatioMode( AM_ASPECT_RATIO_MODE* pamAspectRatioMode );
	STDMETHODIMP SetStreamTransparent( BOOL bStreamTransparent );
	STDMETHODIMP GetStreamTransparent( BOOL *pbStreamTransparent );

	// ========================================================================================================
	// INTERFACES :: IVMRVIDEOSTREAMCONTROL9
	// ===============================================================

	STDMETHODIMP SetStreamActiveState(BOOL fActive);
	STDMETHODIMP GetStreamActiveState(BOOL *fActive);

};
