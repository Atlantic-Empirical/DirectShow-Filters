#ifndef __KEYSTONEBASE__
#define __KEYSTONEBASE__

#include "../Utility/appincludes.h"
#include "Pins/Video/VideoPin.h"
#include "Pins/Output/OutputPin.h"
#include "vmr9.h"
#include <atlbase.h>

class CKeystone : 
					public CBaseFilter,
					public IKeystone,
					public IKeystoneMixer,
					public IVMRMixerControl9,
					public IVMRFilterConfig9,
					public IAMFilterMiscFlags
{

public:
    DECLARE_IUNKNOWN;

    // map getpin/getpincount for base enum of pins to owner
    // override this to return more specialised pin objects

    virtual int GetPinCount();
    virtual CBasePin * GetPin(int n);
    STDMETHODIMP FindPin(LPCWSTR Id, IPin **ppPin);

    // override state changes to allow derived transform filter
    // to control streaming start/stop
    STDMETHODIMP Stop();
    STDMETHODIMP Pause();

public:

	CKeystone(TCHAR *, LPUNKNOWN, REFCLSID clsid);
#ifdef UNICODE
    CKeystone(CHAR *, LPUNKNOWN, REFCLSID clsid);
#endif
    ~CKeystone();

	//Try modifying this to see if we can trick the decoder into thinking we're the VMR9
    STDMETHODIMP QueryFilterInfo( FILTER_INFO * pInfo);

    // =================================================================
    // ----- override these bits ---------------------------------------
    // =================================================================

    // These must be supplied in a derived class
    //virtual HRESULT Transform(IMediaSample * pIn, IMediaSample *pOut);
    //virtual HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut) PURE;

    // this goes in the factory template table to create new instances
    // static CCOMObject * CreateInstance(LPUNKNOWN, HRESULT *);


    // =================================================================
    // ----- Optional Override Methods           -----------------------
    // =================================================================

    //// you can also override these if you want to know about streaming
    //virtual HRESULT StartStreaming();
    //virtual HRESULT StopStreaming();

    // Standard setup for output sample
    HRESULT InitializeOutputSample(IMediaSample *pSample, IMediaSample **ppOutSample);

    // if you override Receive, you may need to override these three too
    virtual HRESULT EndOfStream(void);
    virtual HRESULT BeginFlush(void);
    virtual HRESULT EndFlush(void);
    //virtual HRESULT NewSegment(
    //                    REFERENCE_TIME tStart,
    //                    REFERENCE_TIME tStop,
    //                    double dRate);

#ifdef PERF
    // Override to register performance measurement with a less generic string
    // You should do this to avoid confusion with other filters
    virtual void RegisterPerfId()
         {m_idTransform = MSR_REGISTER(TEXT("Transform"));}
#endif // PERF


// implementation details

protected:

#ifdef PERF
    int m_idTransform;                 // performance measuring id
#endif
    BOOL m_bEOSDelivered;              // have we sent EndOfStream
    BOOL m_bSampleSkipped;             // Did we just skip a frame
    BOOL m_bQualityChanged;            // Have we degraded?

    // critical section protecting filter state.

    CCritSec m_csFilter;

    // critical section stopping state changes (ie Stop) while we're
    // processing a sample.
    //
    // This critical section is held when processing
    // events that occur on the receive thread - Receive() and EndOfStream().
    //
    // If you want to hold both m_csReceive and m_csFilter then grab
    // m_csFilter FIRST - like CKeystone::Stop() does.

public:
    CCritSec m_csReceive;

public:
	// these hold our input and output pins

    friend class CKeystoneVideoPin;
    friend class CKeystoneOutputPin;

	CKeystoneVideoPin *m_pVideo;
    CKeystoneOutputPin *m_pOutput;

	//======================================================================
	// IMPORTED FROM OLD CADAPTER
	//======================================================================

    HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut);
	STDMETHODIMP Run(REFERENCE_TIME tStart);

	//Methods for handling incoming data
    //HRESULT ReceiveVideo(IMediaSample * pSample);
    HRESULT ReceiveSubpicture(IMediaSample * pSample);
    HRESULT ReceiveLine21(IMediaSample * pSample);
    // GD: broken out from ReceiveVideo
    HRESULT MixAndDeliverVideo(IMediaSample* pOut, int w, int h);
    //HRESULT MixAndDeliverVideo(const BYTE* src, IMediaSample* pOut, REFERENCE_TIME rtGOPTC);
	HRESULT DeliverMixedSample(IMediaSample * pOutSample);

	//Helper methods
    HRESULT HandleSampleGrab(IMediaSample *imsSample);

	//Buffer Handling
	void YUVSquare(LPBYTE dst, int x, int y, int dstpitch, int w, int h);
	void CopyUYVYRect(LPBYTE src, int srcx, int srcy, int srcpitch, LPBYTE dst, int dstx, int dsty, int dstpitch, int w, int h);
    // GD: source is const (to match MixAndDeliverVideo)
	void CopyYUY2toUYVYRect(const BYTE* src, int srcx, int srcy, int srcpitch, LPBYTE dst, int dstx, int dsty, int dstpitch, int w, int h);
	void CopyYUY2toUYVYLine(const BYTE* src, LPBYTE dst, int w);
	int RectWidth(const RECT& r);
	int RectHeight(const RECT& r);
	HRESULT SetBufferBGColor(LPBYTE dst, int dstpitch, DWORD w, DWORD h);
	HRESULT SetFullBufferBGColorYUY2(LPBYTE dst, int dstpitch, int w, int h);
	void CopyVidRect(LPBYTE src, LPBYTE dst, int pitch, int w, int h); //Just copies identical data. No change.
	HRESULT SaveBufferToFile(const char * FileName, const char * Mode, LPBYTE buf, int bufsize, int w, int h);
	HRESULT SaveBufferToFile_A(const char * FileName, const char * Mode, LPBYTE buf, int length);
	HRESULT TellPhoenixToGetFrameGrab(IMediaSample * pSample, int w, int h, int UYVY);
	HRESULT SaveBitmapRasterData(LPBYTE src);

	//MediaType Handling
	bool CheckForMTChange(IMediaSample *pIn);
	HRESULT Convert480MTTo486(CMediaType * pMT);
	int GetMTHeight(const CMediaType * pMT);
	HRESULT PutMTHeight(CMediaType * pMT, int nHeight);

    // Reveals IKeystone 
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

public:
	//MISC VARS
	REFERENCE_TIME m_OutTime;
	REFERENCE_TIME m_InTime;
	bool bFilterIsLocked;
	bool bFeedbackClicks;
	long CurrentInputBufferSize;
	long CurrentOutputBufferSize;
	CRefTime crtStreamTime;

	//Guides
	bool bMixGuides;
	int nGuide_L, nGuide_T, nGuide_R, nGuide_B;
	DWORD dwGuideColor;
	int iShowActionTitleGuides;

	//Time Stamping
	LONGLONG llMediaTime_LastSamp_Start;  //Set start MediaTimes to 0 at Run, then increment by ATPF
	LONGLONG llMediaTime_LastSamp_End;
	LONGLONG llFrameCounter;  //The docs seem to indicate that MediaTimes should actuall be frame numbers.

	//Frame rate calculation
	void TickOutputRateTime(DWORD dwCurrentTicks);
	void TickInputRateTime(DWORD dwCurrentTicks);
	HRESULT CalcActualFR_Out(int MSCount, short FrameCount);
	HRESULT CalcActualFR_In(int MSCount, short FrameCount);
	DWORD dwLastSampleProcessedAt;

	DWORD dwSecStartTicks_Out;
	short sFramesPassedThisSec_Out;
	DWORD dwSecStartTicks_In;
	short sFramesPassedThisSec_In;

    float fActualFR_In;
    float fActualFR_Out;

	REFERENCE_TIME llTargetFR_Out_ATPF;
	LONGLONG llTargetFR_In_ATPF;

	LONGLONG llActualFR_Out_ATPF;
	LONGLONG llActualFR_In_ATPF;

	short sFramesPassed_10s_Out;
	short s10sTicker_Out;
	DWORD dw10sStartTicks_Out;
	double d10sFR_Out;

	short sFramesPassed_10s_In;
	short s10sTicker_In;
	DWORD dw10sStartTicks_In;
	double d10sFR_In;

	LONG lSumOfProcessingTimes;
	LONG lSampleCount;

	//Input Pin Info
	bool InIsVIH2;
	VIDEOINFOHEADER * VIHIn;
	VIDEOINFOHEADER2 * VIH2In;
	CMediaType m_mtIn;
	long lConnectedHeight_In;
	int LastConnectedHeight_VidIn;

	//Output Pin Info
	bool OutIsVIH2;
	VIDEOINFOHEADER * VIHOut;
	VIDEOINFOHEADER2 * VIH2Out;
	CMediaType m_mtOut;
	long lConnectedHeight_Out;
	//HRESULT ReconnectOutput(CMediaType * pMTToPropose);

	//SP Pin Info
	CMediaType m_mtSP;

	//L21 Pin Info
	CMediaType m_mtL21;

	//ProcAmp Values
	bool bDoProcAmp;
	bool bProcAmpHalfFrame;
	double PA_dBrightness;
	double PA_dContrast;
	double PA_dSaturation;
	double PA_dHue;
	double PA_dGamma;
	double PA_dChroma;
	double PA_dLuma;
	double PA_dTint;
	double PA_dGain;
	double PA_dSharpness;
	
	bool bDoColorFiltering;
	int iWhichColorFilter;

	//YUV File Rendering
	STDMETHODIMP ShowUYVYFile(LPCTSTR stYUVPath, int X, int Y, int W, int H);
	STDMETHODIMP ClearUYVYFile();
	STDMETHODIMP ClearUYVYFile_A();
	STDMETHODIMP ShowUYVYBuffer(LPBYTE pUYVY, int W, int H, int X, int Y);
	LPBYTE YUVFile;
	int YUVFileLength;
	bool bDoingYUVFile;

	//Pauser
	STDMETHODIMP Pause(int nPause);
	int Pausing;

	//MIXER
	struct stMixData
	{
		LPBYTE pImage;
		int nW;
		int nH;
		int nX;
		int nY;
		int nImage_Format;
		RGBQUAD * Palette;
		COLORREF KeyColor;
	};

	//OSD Mix
	bool bPendingOSDMix;
	stMixData OSDMixData;
	int OSDTicker;
	int OSDTickGoal;

	//Subpicture Mix
	//bool bPendingSPMix;
	LPBYTE TempSPBuffer;
	stMixData SPMixData;
	int mxSub_W, mxSub_H, mxSub_X, mxSub_Y;
	int SP_X, SP_Y;
	//REFERENCE_TIME rtCurrentSPStart;
	//REFERENCE_TIME rtCurrentVidStart;

	//Line21 Mix
	//bool bPendingL21Mix;
	LPBYTE TempL21Buffer;
	stMixData L21MixData;
	int L21_X, L21_Y;

	//GOP TC
	int _tmain(int argc, _TCHAR* argv[]);
	bool InitGOPTC(HDC& hDC, HBITMAP& hBM, HFONT& hFont, int w, int h);
	void FreeGOPTC(HDC& hDC, HBITMAP& hBM, HFONT& hFont);
	void RTtoBMP(HDC hDC, HBITMAP hBM, HFONT hFont, REFERENCE_TIME rt, double fps, DWORD fgcolor = 0xffffff, DWORD bgcolor = 0x000000);
	void RTToHMSF(REFERENCE_TIME rt, double fps, int& h, int& m, int& s, int& f);
	short iMixGOPTC;
	HDC m_hDC;
	HBITMAP m_hBM;
	HFONT m_hFONT;
	LONGLONG LastNewTStart;
	LONGLONG TStartIncr;

	//END MIXER

	HRESULT SetupVideoSizing();
	int LB_OK;
	int PS_OK;

	HRESULT ResendSample();

	//Sample Vars
	bool bSampleOperationUnderway;
	int SampleWhat;
	LPBYTE pSample;
	long lSampleSize;
	bool bGetSample;
	long lSamp_W, lSamp_H;

	int opFrameCnt;

	//Multi Frame Dump
	int iDumpFrameCount;
	LPCTSTR sDumpFrameLocation;
	HRESULT HandleMultiFrameGrabSample(IMediaSample *IMS);

	//Last frame output grab
	LPBYTE LastFrameOutput;
	HRESULT HandleSampleGrab_LastOutput();

	//Force Output connect size
	bool bForceOutputConnectSize;
	int iForceOutput_W;
	int iForceOutput_H;

public:
	typedef enum _ImageFormat
	{
		IF_RGB24 =  0,
		IF_RGB32 = 1,
		IF_YUY2 = 2,
		IF_UYVY = 3,
		IF_ARGB4444 = 4,
		IF_RGB8 = 5
	} ImageFormat;

public:
	typedef enum _eSampleFrom
	{
		VideoIn =  0,
		SubpictureIn = 1,
		Line21In = 2,
		Output = 3,
	} eSampleFrom;


	//==============================================================================
	//Interfaces
	//==============================================================================

	// IKeystone
    STDMETHODIMP get_InputMediaType(CMediaType *InputMediaType);
    STDMETHODIMP get_OutputMediaType(CMediaType *OutputMediaType);
    STDMETHODIMP get_InputWidth(long *InputWidth);
    STDMETHODIMP get_InputHeight(long *InputHeight);
    STDMETHODIMP get_OutputWidth(long *InputWidth);
    STDMETHODIMP get_OutputHeight(long *InputHeight);
    STDMETHODIMP GrabSample(int SampleWhat, LPBYTE * pSample, long *lSampleSizeSampleSize, long *lWidth, long *lHeight);
    STDMETHODIMP UnlockFilter(GUID *FilterKey);
    STDMETHODIMP put_FeedbackClicks(bool DoClicks);
    STDMETHODIMP get_FeedbackClicks(bool *DoClicks);
	STDMETHODIMP Set32Status(int Do32);
	STDMETHODIMP FrameStep(bool bForward);
	STDMETHODIMP QuitFrameStepping();
	STDMETHODIMP ActivateVarispeed(double Speed);
	STDMETHODIMP DeactivateVarispeed();
	STDMETHODIMP SetL21State(bool bL21Active);
	STDMETHODIMP ShowJacketPicture(LPCTSTR stJPPath, int X, int Y, int W, int H);
	STDMETHODIMP ShowBitmap(LPBYTE pBMP, int W, int H, int X, int Y, int Format);
	STDMETHODIMP SaveNextXFrames(int Count, LPCTSTR stJPPath);
	STDMETHODIMP ForceOutputConnectSize(int W, int H );
	STDMETHODIMP ActivateFFRW();
	STDMETHODIMP DeactivateFFRW();
	STDMETHODIMP ResendLastSamp();

	//IKeystoneMixer
	STDMETHODIMP put_OSD(LPBYTE pMixImage, int W, int H, int X, int Y, int Format, COLORREF KeyColor, int DurationSecs);
	STDMETHODIMP ClearOSD();
	STDMETHODIMP GrabLastL21(LPBYTE * pSubpictureSample);
	STDMETHODIMP GrabLastSubpicture(LPBYTE * pSubpictureSample);
    STDMETHODIMP SetGuides(int Left,int Top, int Right,int Bottom, int Red, int Blue, int Green);
    STDMETHODIMP ClearGuides();
	STDMETHODIMP SetSPPlacement(int X, int Y);
	STDMETHODIMP SetResizeMode(int Mode);
	STDMETHODIMP SetLBColor(int Red, int Green, int Blue);
	STDMETHODIMP SetL21Placement(int X, int Y);
	STDMETHODIMP ReverseFieldOrder(bool bReverseIt);
	STDMETHODIMP SetJacketPicMode(bool bJackPicMode);
	STDMETHODIMP BumpFieldsDown(int bBumpFields);
	STDMETHODIMP BurnGOPTCs(int iBurnGOPTCs);
	STDMETHODIMP SetActionTitleGuides(int iShowGuides, int Red, int Blue, int Green);
	STDMETHODIMP FieldSplit(int DoFieldSplit);
	STDMETHODIMP HighContrastSP(int DoHighContrastSP);
	STDMETHODIMP SetARFlags(int PS, int LB);

	LPBYTE pLastL21Buffer;
	LPBYTE pLastSubpictureBuffer;

	//IKeystoneProcAmp
	STDMETHODIMP put_Brightness(double dBrightness);
	STDMETHODIMP get_Brightness(double *dBrightness);
	STDMETHODIMP put_Contrast(double dContrast);
	STDMETHODIMP get_Contrast(double *dContrast);
	STDMETHODIMP put_Hue(double dHue);
	STDMETHODIMP get_Hue(double *dHue);
	STDMETHODIMP put_Saturation(double dSaturation);
	STDMETHODIMP get_Saturation(double *dSaturation);
	STDMETHODIMP ToggleProcAmp(bool bToggleProcAmp, bool bHalfFrame);
	STDMETHODIMP ToggleColorFilter(bool bDoColorFilter, int iUseWhichFilter);

	//IVMRMixerControl9
	STDMETHODIMP SetAlpha( DWORD dwStreamID, float Alpha);
	STDMETHODIMP GetAlpha(  DWORD dwStreamID, float *pAlpha);
	STDMETHODIMP SetZOrder( DWORD dwStreamID, DWORD dwZ);
	STDMETHODIMP GetZOrder( DWORD dwStreamID, DWORD *pZ);
	STDMETHODIMP SetOutputRect( DWORD dwStreamID, const VMR9NormalizedRect *pRect);
	STDMETHODIMP GetOutputRect( DWORD dwStreamID, VMR9NormalizedRect *pRect);
	STDMETHODIMP SetBackgroundClr( COLORREF ClrBkg);
	STDMETHODIMP GetBackgroundClr( COLORREF *lpClrBkg);
	STDMETHODIMP SetMixingPrefs( DWORD dwMixerPrefs);
	STDMETHODIMP GetMixingPrefs( DWORD *pdwMixerPrefs);
	STDMETHODIMP SetProcAmpControl( DWORD dwStreamID, VMR9ProcAmpControl *lpClrControl);
	STDMETHODIMP GetProcAmpControl( DWORD dwStreamID, VMR9ProcAmpControl *lpClrControl);
	STDMETHODIMP GetProcAmpControlRange( DWORD dwStreamID, VMR9ProcAmpControlRange *lpClrControl);

	//IVMRFilterConfig9
	STDMETHODIMP SetImageCompositor( IVMRImageCompositor9 *lpVMRImgCompositor);
    STDMETHODIMP SetNumberOfStreams( DWORD dwMaxStreams);
    STDMETHODIMP GetNumberOfStreams( DWORD *pdwMaxStreams);
    STDMETHODIMP SetRenderingPrefs( DWORD dwRenderFlags);
    STDMETHODIMP GetRenderingPrefs( DWORD *pdwRenderFlags);
    STDMETHODIMP SetRenderingMode( DWORD Mode);
    STDMETHODIMP GetRenderingMode( DWORD *pMode);

	//IAMFilterMiscFlags
	ULONG STDMETHODCALLTYPE GetMiscFlags( void);

	//IBasicVideo
	STDMETHODIMP get_AvgTimePerFrame( REFTIME *pAvgTimePerFrame);
	STDMETHODIMP get_BitRate( long *pBitRate);
    STDMETHODIMP get_BitErrorRate( long *pBitErrorRate);
    STDMETHODIMP get_VideoWidth( long *pVideoWidth);
    STDMETHODIMP get_VideoHeight(long *pVideoHeight);
    STDMETHODIMP put_SourceLeft( long SourceLeft);
    STDMETHODIMP get_SourceLeft( long *pSourceLeft);
    STDMETHODIMP put_SourceWidth( long SourceWidth);
    STDMETHODIMP get_SourceWidth( long *pSourceWidth);
    STDMETHODIMP put_SourceTop( long SourceTop);
    STDMETHODIMP get_SourceTop( long *pSourceTop);
    STDMETHODIMP put_SourceHeight( long SourceHeight);
    STDMETHODIMP get_SourceHeight( long *pSourceHeight);
    STDMETHODIMP put_DestinationLeft( long DestinationLeft);
    STDMETHODIMP get_DestinationLeft( long *pDestinationLeft);
    STDMETHODIMP put_DestinationWidth( long DestinationWidth);
    STDMETHODIMP get_DestinationWidth( long *pDestinationWidth);
    STDMETHODIMP put_DestinationTop( long DestinationTop);
    STDMETHODIMP get_DestinationTop( long *pDestinationTop);
    STDMETHODIMP put_DestinationHeight( long DestinationHeight);
    STDMETHODIMP get_DestinationHeight( long *pDestinationHeight);
    STDMETHODIMP SetSourcePosition(long Left, long Top, long Width, long Height);
    STDMETHODIMP GetSourcePosition( long *pLeft, long *pTop, long *pWidth, long *pHeight);
    STDMETHODIMP SetDefaultSourcePosition( void);
    STDMETHODIMP SetDestinationPosition( long Left, long Top, long Width, long Height);
    STDMETHODIMP GetDestinationPosition( long *pLeft, long *pTop, long *pWidth, long *pHeight);
    STDMETHODIMP SetDefaultDestinationPosition( void);
    STDMETHODIMP GetVideoSize( long *pWidth,long *pHeight);
    STDMETHODIMP GetVideoPaletteEntries( long StartIndex, long Entries, long *pRetrieved, long *pPalette);
    STDMETHODIMP GetCurrentImage( long *pBufferSize, long *pDIBImage);
    STDMETHODIMP IsUsingDefaultSource( void);
    STDMETHODIMP IsUsingDefaultDestination( void);

	//// IKsPropertySet
	//STDMETHODIMP Set(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength);
	//STDMETHODIMP Get(REFGUID PropSet, ULONG Id, LPVOID pInstanceData, ULONG InstanceLength, LPVOID pPropertyData, ULONG DataLength, ULONG* pBytesReturned);
	//STDMETHODIMP QuerySupported(REFGUID PropSet, ULONG Id, ULONG* pTypeSupport);

};

#endif /* __KEYSTONEBASE__ */


