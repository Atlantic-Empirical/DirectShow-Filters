
//------------------------------------------------------------------------------
// File: PushSource.H
//
// Desc: DirectShow sample code - In-memory push mode source filter
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// UNITS = 10 ^ 7  
// UNITS / 30 = 30 fps;
// UNITS / 20 = 20 fps, etc7
const REFERENCE_TIME FPS_50 = UNITS / 50;
const REFERENCE_TIME FPS_30 = UNITS / 30;
const REFERENCE_TIME FPS_2997 = UNITS / 29.97;
const REFERENCE_TIME FPS_24 = UNITS / 24;
const REFERENCE_TIME FPS_23976 = UNITS / 23.976;
const REFERENCE_TIME FPS_20 = UNITS / 20;
const REFERENCE_TIME FPS_15 = UNITS / 15;
const REFERENCE_TIME FPS_10 = UNITS / 10;
const REFERENCE_TIME FPS_5  = UNITS / 5;
const REFERENCE_TIME FPS_4  = UNITS / 4;
const REFERENCE_TIME FPS_3  = UNITS / 3;
const REFERENCE_TIME FPS_2  = UNITS / 2;
const REFERENCE_TIME FPS_1  = UNITS / 1;

// Filter name strings
#define g_wszPushBitmap     L"SMT YUV Source"


/**********************************************
 *
 *  Class declarations
 *
 **********************************************/

class CPushPinBitmap :	public CSourceStream, 
						public IFileSourceFilterSMT,
						public IMediaSeeking
{
DECLARE_IUNKNOWN;

public:
	CReadQueue	m_QueueThread;

protected:
    CCritSec	m_cSharedState;           // Protects our internal state
    CImageDisplay	m_Display;            // Figures out our media type for us

    //int	m_iFrameNumber;							// Where we are
	REFERENCE_TIME	m_rtFrameLength;	// ATPF
    //const REFERENCE_TIME	m_rtFrameLength;	// ATPF
	
public:

    CPushPinBitmap(HRESULT *phr, CSource *pFilter);
    ~CPushPinBitmap();

	//void * memcpy_amd(void *dest, const void *src, size_t n);

	//TRPF
	REFERENCE_TIME rtLastStart;
	REFERENCE_TIME rtLastEnd;

    // Override the version that offers exactly one media type
    HRESULT GetMediaType(CMediaType *pMediaType);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest);
    HRESULT FillBuffer(IMediaSample *pSample);
    HRESULT CreateVideoMediaType(int width, int height, WORD bpp, DWORD format, REFERENCE_TIME timePerFrame, CMediaType* pMediaType);

    // Quality control
	// Not implemented because we aren't going in real time.
	// If the file-writing filter slows the graph down, we just do nothing, which means
	// wait until we're unblocked. No frames are ever dropped.
    STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q);

	//IFileSourceFilterSMT Interface
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);
	STDMETHODIMP Load(LPCOLESTR pszFileName, const AM_MEDIA_TYPE *pmt);
	STDMETHODIMP GetCurFile(LPOLESTR *ppszFileName, AM_MEDIA_TYPE *pmt);
	STDMETHODIMP SetATPF(REFERENCE_TIME TargetATPF);

	//IMediaSeeking
	GUID CurrentTimeFormat; //holds the current time format
	STDMETHODIMP GetCapabilities(DWORD * pCapabilities);
	STDMETHODIMP CheckCapabilities(DWORD * pCapabilities);
	STDMETHODIMP IsFormatSupported(const GUID * pFormat);
	STDMETHODIMP QueryPreferredFormat(GUID * pFormat);
	STDMETHODIMP GetTimeFormat(GUID * pFormat);
	STDMETHODIMP IsUsingTimeFormat(const GUID * pFormat);
	STDMETHODIMP SetTimeFormat(const GUID * pFormat);
	STDMETHODIMP GetDuration(LONGLONG *pDuration);
	STDMETHODIMP GetStopPosition(LONGLONG *pStop);
	STDMETHODIMP GetCurrentPosition(LONGLONG *pCurrent);
	STDMETHODIMP ConvertTimeFormat(LONGLONG * pTarget, const GUID * pTargetFormat, LONGLONG Source, const GUID * pSourceFormat);
	STDMETHODIMP SetPositions(LONGLONG * pCurrent, DWORD dwCurrentFlags, LONGLONG * pStop, DWORD dwStopFlags);
	STDMETHODIMP GetPositions(LONGLONG * pCurrent, LONGLONG * pStop);
	STDMETHODIMP GetAvailable(LONGLONG * pEarliest, LONGLONG * pLatest);
	STDMETHODIMP SetRate(double dRate);
	STDMETHODIMP GetRate(double * pdRate);
	STDMETHODIMP GetPreroll(LONGLONG * pllPreroll);

	// SourceStream
	//HRESULT		Inactive(void);
};


class CPushSourceBitmap :	public CSource
{
private:
    // Constructor is private because you have to use CreateInstance
    CPushSourceBitmap(IUnknown *pUnk, HRESULT *phr);
    ~CPushSourceBitmap();

public:
	friend class CPushPinBitmap;
    CPushPinBitmap *m_pPin;

	static CUnknown * WINAPI CreateInstance(IUnknown *pUnk, HRESULT *phr);  

	DECLARE_IUNKNOWN;

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

};


