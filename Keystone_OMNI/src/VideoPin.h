#pragma once
#include <streams.h>

#pragma warning ( disable : 4005 ) 


// ==================================================
// Implements the VIDEO input pin
// ==================================================

class CKeystoneVideoPin : 
							public CBaseInputPin,
							public IKsPropertySet
{
    friend class CKeystone;

protected:
    CKeystone *m_pTransformFilter;
    BOOL m_bSampleSkipped;             // Did we just skip a frame
    BOOL m_bQualityChanged;            // Have we degraded?

public:
    CKeystoneVideoPin(
        TCHAR *pObjectName,
        CKeystone *pTransformFilter,
        HRESULT * phr,
        LPCWSTR pName);
#ifdef UNICODE
    CKeystoneVideoPin(
        char *pObjectName,
        CKeystone *pTransformFilter,
        HRESULT * phr,
        LPCWSTR pName);
#endif

	DECLARE_IUNKNOWN

	~CKeystoneVideoPin()
	{
	};

    STDMETHODIMP QueryId(LPWSTR * Id)
    {
        return AMGetWideString(L"Video", Id);
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

	virtual HRESULT CheckStreaming();

	// Allocator
	STDMETHODIMP NotifyAllocator(IMemAllocator* pAllocator, BOOL bReadOnly);

public:

	// ========================================================================================================
	// SMT :: VIDEO PIN :: MEDIATYPE
	// ===============================================================

	CMediaType& CurrentMediaType() { return m_mt; };

	// ========================================================================================================
	// SMT :: VIDEO PIN :: GENERAL PROPERTIES
	// ===============================================================

	bool bNewSegmentReceived;
	bool bSendNewSegment;

	// ========================================================================================================
	// INTERFACES :: IKSPROPERTYSET
	// ===============================================================

	STDMETHODIMP Set(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength);
	STDMETHODIMP Get(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength, ULONG* pBytesReturned);
	STDMETHODIMP QuerySupported(REFGUID PropSet, ULONG Id, ULONG* pTypeSupport);

};
