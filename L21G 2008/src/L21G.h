
// Define the filter's CLSID
// {A2957546-A38D-44b9-834E-096AF622EC3D}
static const GUID CLSID_L21G = 
{ 0xa2957546, 0xa38d, 0x44b9, { 0x83, 0x4e, 0x9, 0x6a, 0xf6, 0x22, 0xec, 0x3d } };

static const WCHAR g_wszName[] = L"SMT L21G";   // A name for the filter 

class CL21G : 
				public CTransformFilter, 
				public IL21G
{
public:
	DECLARE_IUNKNOWN;

	CL21G(LPUNKNOWN pUnk, HRESULT *phr) : CTransformFilter(NAME("Seq. L21G"), pUnk, CLSID_L21G) {}

    // Overridden CTransformFilter methods
    HRESULT CheckInputType(const CMediaType *mtIn);
    HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProp);
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
    HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);

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
	HRESULT  SaveBufferToFile(const char * FileName, const char * Mode, LPBYTE buf, int bufsize);

	//Interface
	STDMETHODIMP GetBuffer(LPBYTE *pSample, long *BufferSize);

};