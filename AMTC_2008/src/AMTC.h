// Define the filter's CLSID
// {A6512CF0-A47B-45ba-A054-0DB0D4BB87F7}
static const GUID CLSID_AMTC = 
{ 0xa6512cf0, 0xa47b, 0x45ba, { 0xa0, 0x54, 0xd, 0xb0, 0xd4, 0xbb, 0x87, 0xf7 } };

static const WCHAR g_wszName[] = L"SMT AMTC";   // A name for the filter 

class CAMTC : 
				public CTransformFilter,
				public IAMTC
{
public:
	DECLARE_IUNKNOWN;

	CAMTC(LPUNKNOWN pUnk, HRESULT *phr) : CTransformFilter(NAME("SMT AMTC"), pUnk, CLSID_AMTC) 
	{
		bGrabBuffers = false;
		bSetNULLTimestamps = false;
	}

    // Overridden CTransformFilter methods
    HRESULT CheckInputType(const CMediaType *mtIn);
    HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProp);
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
    HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);
	virtual HRESULT NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
	HRESULT DeliverSample(IMediaSample * pIn, IMediaSample * pOut);

    // Override this so we can grab the video format
    HRESULT SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt);

    // Static object-creation method (for the class factory)
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr); 

	//we're offering an interface
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	//Sequoyan stuff
	LPBYTE LastBuffer;
	long LastBufferSize;
	bool opUnderway;
	bool bGrabBuffers;

	bool bSetNULLTimestamps;

	//Time Stamping
	LONGLONG llMediaTime_LastSamp_Start;  //Set start MediaTimes to 0 at Run, then increment by ATPF
	LONGLONG llMediaTime_LastSamp_End;
	CRefTime crtSampleDelivered;

	//Interface
	STDMETHODIMP GetBuffer(LPBYTE *pSample, long *BufferSize);
	STDMETHODIMP StoreBuffers(bool bUserWantsBuffersStored);
	STDMETHODIMP QuitFrameStepping();
	STDMETHODIMP FrameStep(bool bForward);
	STDMETHODIMP SetNULLTimestamps();

	//FrameStepping
	bool bFrameStepping;
	bool bDeliverFrameStepSample;
};
