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
//struct strucCompleteFrame
//{
//	REFERENCE_TIME		rtTimeStart;								// start reference time
//	REFERENCE_TIME		rtTimeEnd;									// end reference time
//	byte				bSampleBuffer[ SAMPLE_SIZE_MAX ];			// the buffer.
//	bool				bIsDiscontinuity;							// quick discontinuity flag
//	bool				bIsSyncPoint;								// sync point flag
//};

//END FRAME STEPPING


// ==================================================
// Implements the VIDEO input pin
// ==================================================

class CKeystoneVideoTwoPin : 
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
    CKeystoneVideoTwoPin(
        TCHAR *pObjectName,
        CKeystone *pTransformFilter,
        HRESULT * phr,
        LPCWSTR pName);
#ifdef UNICODE
    CKeystoneVideoTwoPin(
        char *pObjectName,
        CKeystone *pTransformFilter,
        HRESULT * phr,
        LPCWSTR pName);
#endif

	DECLARE_IUNKNOWN

	~CKeystoneVideoTwoPin()
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

	//IVMRVideoStreamControl
    STDMETHODIMP SetColorKey( LPDDCOLORKEY lpClrKey);
    STDMETHODIMP GetColorKey( LPDDCOLORKEY lpClrKey);
    STDMETHODIMP SetStreamActiveState( BOOL fActive);
    STDMETHODIMP GetStreamActiveState( BOOL *lpfActive);

	//3:2 Stuff
	int bRun32;
	REFERENCE_TIME rtLast32Sample_StartTime;
	REFERENCE_TIME rtLast32Sample_EndTime;
	BYTE s_buffer[720*480*2];
	void extract(bool top_field, LPBYTE src);
	REFERENCE_TIME rtLastEnd;
	REFERENCE_TIME rtLastStart;
	int iDisplayFrameNo;
	int iEncodeFrameNo;
	bool BIE;
	int sbTopFieldSource;
	int sbBottomFieldSource;
	bool bLastRFF;
	bool bNeedTop;
	int DoFieldSplitting;

	LPCTSTR ImageRecordTargPath;

	//bool bAcceptVidTwo;

	//"TYPE DETERMINATION MODE" stuff
	bool bTDM;
	int TDMCnt;
	bool tff1, tff2, tff3, tff4;
	bool rff1, rff2, rff3, rff4;
	bool tff_encoded;

	//Field/Frame properties
	HRESULT ReverseFieldOrder(LPBYTE src, int h, int w, int pitch);
	HRESULT BumpFieldsDown(LPBYTE src, int h, int w, int pitch, int Cnt, bool SetLinesBlack);
	bool bForceReverseFieldOrder;
	bool bForceBumpFieldsDown;

	HRESULT ReceiveStep2(IMediaSample * pIn);
	HRESULT ThreeTwo(IMediaSample * pIn);
	HRESULT NonThreeTwoIOS(IMediaSample * pIn, RECT * r1, int CopyLine);
	HRESULT ThreeTwoIOS(IMediaSample * pIn, RECT * r1, int CopyLine);
	HRESULT ReceiveStep5(IMediaSample * pOut, RECT * r1, bool Is32Sample, int BottomSource, int TopSource);
	HRESULT Resize(IMediaSample * pIn);

	HRESULT LogMPEGFlags(IMediaSample * pIn);

	//Frame Stepping
public:
	bool bFrameStepping;
	//bool bFrameStepUnderway;
	bool bDeliverFrameStepSample;
	bool bSendNewSegment;
	//HRESULT FrameStep();
	//HRESULT SetupFrameStepping();
	//HRESULT DeliverFrameStepSample();
	//HRESULT QuitFrameStepping();
	//HRESULT PushSampleOntoQueue( IMediaSample * pIMediaSample );
	//IMediaSample * PopSampleFromQueue();
	//bool GetMutex( HANDLE hMutex, DWORD dwMaxWaitMsec );
	//queue <strucCompleteFrame> m_FrameQueue;  // a frame Q
	//HANDLE m_hQAccessMutex;                   // this is a mutex handle to access the Q.

	//IBaseFilter * Nav;
	//IBaseFilter * Dec;

	//bool bNavDecRunning;
	//HRESULT PauseNavDec();
	//HRESULT RunNavDec();

	//Varispeed
	bool bInVarispeed;
	double MSVarispeedInterval;
	HRESULT ProcessVarispeedSample(IMediaSample * pVSSample);
	HRESULT ActivateVarispeed(double Speed);
	HRESULT DeactivateVarispeed();
	bool bInFFRW;

	//Other Custom Stuff
	//HRESULT ProcessVideoSample(IMediaSample * pIn);
	bool bNewSegmentReceived;
	bool bRestartTimeStamps;

	//LB/PS resizing
	int ResizeMode; //1=anamorphic, 2=PanScan, 3=Letterbox
	int DesResizeMode; //1=anamorphic, 2=PanScan, 3=Letterbox
	HRESULT SizeToPanScan(LPBYTE src, int h, int w);
	HRESULT SizeToLetterbox(LPBYTE orig, int h, int w, DWORD* LBColor);
	DWORD dwLBColor;

	//Gabor stuff
	bool ZoomYUY2(LPBYTE src, int w, int h, RECT r);
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

	//Jacket picture mode
	bool bInJacketPictureMode;

	//Misc
	int FrameCnt;
	int InputType;
	LPBYTE NewBuf;

	// IKsPropertySet
	STDMETHODIMP Set(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength);
	STDMETHODIMP Get(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength, ULONG* pBytesReturned);
	STDMETHODIMP QuerySupported(REFGUID PropSet, ULONG Id, ULONG* pTypeSupport);

};
