#pragma once
#include <streams.h>
//#include <vmr9.h>
//#include <ddraw.h>

#pragma warning ( disable : 4005 ) 

//FRAME STEPPING

// Includes (these should go in the .h file)
#include <queue>
#include <iostream>
using std::queue;	// using the STL queue
typedef std::basic_string< TCHAR > String;
#define SAMPLE_SIZE_MAX	691200	//Make buffer that should be 699840 - you probably already have this, but it's for the structure below.
								//should be 691200 for NTSC and 829440 for PAL

// This is a structure we're going to use with our queue.  Basically, it's all the information we need 
// to take from the IMediaSample, so we can create a new one when we need it.
struct strucCompleteFrame
{
	REFERENCE_TIME		rtTimeStart;								// start reference time
	REFERENCE_TIME		rtTimeEnd;									// end reference time
	byte				bSampleBuffer[ SAMPLE_SIZE_MAX ];			// the buffer.
	bool				bIsDiscontinuity;							// quick discontinuity flag
	bool				bIsSyncPoint;								// sync point flag
};

//END FRAME STEPPING


// ==================================================
// Implements the VIDEO input pin
// ==================================================

class CKeystoneVideoPin : 
							public CBaseInputPin,
							public IKsPropertySet//,
							//public IVMRVideoStreamControl
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
		ZoomDeInit();
	};

    STDMETHODIMP QueryId(LPWSTR * Id)
    {
        return AMGetWideString(L"Video", Id);
    }

    // Grab and release extra interfaces if required

    HRESULT CheckConnect(IPin *pPin);
    HRESULT BreakConnect();
    HRESULT CompleteConnect(IPin *pReceivePin);

    // check that we can support this output type
    HRESULT CheckMediaType(const CMediaType* mtIn);

    // set the connection media type
    HRESULT SetMediaType(const CMediaType* mt);

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	//Moved from CAdapterInputPin
	HRESULT GetMediaType(int iPosition, CMediaType* pMediaType);
	STDMETHODIMP QueryAccept(const AM_MEDIA_TYPE* pmt);
	STDMETHODIMP ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt);
	STDMETHODIMP NotifyAllocator(IMemAllocator* pAllocator, BOOL bReadOnly);

    // --- IMemInputPin -----

    // here's the next block of data from the stream.
    // AddRef it yourself if you need to hold it beyond the end
    // of this call.
    STDMETHODIMP Receive(IMediaSample * pSample);

    // provide EndOfStream that passes straight downstream
    // (there is no queued data)
    STDMETHODIMP EndOfStream(void);

    // passes it to CKeystone::BeginFlush
    STDMETHODIMP BeginFlush(void);

    // passes it to CKeystone::EndFlush
    STDMETHODIMP EndFlush(void);

    STDMETHODIMP NewSegment(
                        REFERENCE_TIME tStart,
                        REFERENCE_TIME tStop,
                        double dRate);

    // Check if it's OK to process samples
    virtual HRESULT CheckStreaming();

    // Media type
public:
    CMediaType& CurrentMediaType() { return m_mt; };

	HRESULT ResizeToHD(LPBYTE src, LPBYTE TargetBuffer);
	HRESULT LogMPEGFlags(IMediaSample * pIn);

	//Other Custom Stuff
	bool bNewSegmentReceived;
	bool bRestartTimeStamps;

	//LB/PS resizing
	int ResizeMode; //1=anamorphic, 2=PanScan, 3=Letterbox
	int DesResizeMode; //1=anamorphic, 2=PanScan, 3=Letterbox
	HRESULT SizeToPanScan(LPBYTE src, int h, int w);
	HRESULT SizeToLetterbox(LPBYTE orig, int h, int w, DWORD* LBColor);

	//Gabor stuff
	bool ZoomYUY2(LPBYTE src, int w, int h, RECT r, LPBYTE TargetBuffer);
	void Resize1D(BYTE* d, int dw, BYTE* s, int sw, int step);
	bool ZoomInit(int w, int h);
	void ZoomDeInit();
	bool ShrinkV(LPBYTE dst, int dh, LPBYTE src, int sh, int w, BOOL bInterlaced);
	bool ShrinkV(LPBYTE dst, int dh, int dstpitch, LPBYTE src, int sh, int srcpitch, int wbytes);

	//Sample processing time
	REFERENCE_TIME rtSampleReceived;
	REFERENCE_TIME rtSampleDelivered;
	CRefTime crtSampleReceived;
	CRefTime crtSampleDelivered;
	DWORD dwSampleReceived;
	DWORD dwSampleDelivered;

	//Misc
	int FrameCnt;

	// IKsPropertySet
	STDMETHODIMP Set(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength);
	STDMETHODIMP Get(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength, ULONG* pBytesReturned);
	STDMETHODIMP QuerySupported(REFGUID PropSet, ULONG Id, ULONG* pTypeSupport);

	//IVMRVideoStreamControl
    STDMETHODIMP SetColorKey( LPDDCOLORKEY lpClrKey);
    STDMETHODIMP GetColorKey( LPDDCOLORKEY lpClrKey);
    STDMETHODIMP SetStreamActiveState( BOOL fActive);
    STDMETHODIMP GetStreamActiveState( BOOL *lpfActive);

};
