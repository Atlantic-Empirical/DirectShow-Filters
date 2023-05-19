#pragma once
#include "appincludes.h"
#include "VideoPin.h"
#include "SubpicturePin.h"
#include "Line21Pin.h"
#include "OutputPin.h"
#include "SentinelKeystypes.h"  /*  Header file for this License  */    
#include "vmr9.h"
#include "ProcAmp.h"
#include "ColorFilters.h"
#include <atlbase.h>
#include <queue>
using std::queue;	// using the STL queue

class CKeystone : 
					public CBaseFilter,
					public IKeystone,
					public ISpecifyPropertyPages,
					public IKeystoneProcAmp,
					public IKeystoneQuality,
					public IKeystoneMixer,
					public IVMRMixerControl9,
					public IVMRFilterConfig9,
					public IAMFilterMiscFlags,
					public IMediaSeeking,
					public CProcAmp,
					public CColorFilters
{

public:
    DECLARE_IUNKNOWN;

    virtual int GetPinCount();
    virtual CBasePin * GetPin(int n);
    STDMETHODIMP FindPin(LPCWSTR Id, IPin **ppPin);

    STDMETHODIMP Stop();
    STDMETHODIMP Pause();

public:

	CKeystone(TCHAR *, LPUNKNOWN, REFCLSID clsid);
#ifdef UNICODE
    CKeystone(CHAR *, LPUNKNOWN, REFCLSID clsid);
#endif
	~CKeystone();

    STDMETHODIMP QueryFilterInfo( FILTER_INFO * pInfo);

	HRESULT InitializeOutputSample(IMediaSample *pSample, IMediaSample **ppOutSample);

    // if you override Receive, you may need to override these three too
    virtual HRESULT EndOfStream(void);
    virtual HRESULT BeginFlush(void);
    virtual HRESULT EndFlush(void);

#ifdef PERF
	// Override to register performance measurement with a less generic string
    // You should do this to avoid confusion with other filters
    virtual void RegisterPerfId() {m_idTransform = MSR_REGISTER(TEXT("Transform"));}
#endif // PERF

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

	// ========================================================================================================
	// DSHOW
	// ===============================================================

	// these hold our input and output pins
    friend class CKeystoneVideoPin;
    friend class CKeystoneOutputPin;
    friend class CKeystoneSubpicturePin;
    friend class CKeystoneLine21Pin;

	CKeystoneVideoPin *m_pVideo;
    CKeystoneOutputPin *m_pOutput;
    CKeystoneSubpicturePin *m_pSubpicture;
    CKeystoneLine21Pin *m_pLine21;

	STDMETHODIMP Run(REFERENCE_TIME tStart);

	// ========================================================================================================
	// SMT :: PIPELINE - CORE
	// ===============================================================

    HRESULT Pipeline_Core_Entrance(IMediaSample* pOut, int w, int h);
	HRESULT Pipeline_Core_DeliverMixedSample(IMediaSample * pOutSample);
	HRESULT Pipeline_Core_ReSizeMixSendSample(int Unconditional, bool DoYUVCSC);

	// ========================================================================================================
	// SMT :: PIPELINE - VIDEO
	// ===============================================================

	HRESULT Pipeline_Video_ReceiveVideo(IMediaSample * pSample);
	HRESULT Pipeline_Video_ReceiveVideo_Step2(IMediaSample * pSample);
	HRESULT Pipeline_Video_ThreeTwo(IMediaSample * pIn);
	HRESULT Pipeline_Video_ThreeTwoIOS(IMediaSample * pIn, RECT * r1, int CopyLine);
	HRESULT Pipeline_Video_NonThreeTwoIOS(IMediaSample * pIn, RECT * r1, int CopyLine);
	HRESULT Pipeline_Video_ReceiveVideo_LastStep(IMediaSample * pOut, RECT * r1, bool Is32Sample);

	// ========================================================================================================
	// SMT :: PIPELINE - SUBPICTURES
	// ===============================================================

	HRESULT Pipeline_Subpicture_ReceiveSubpicture(IMediaSample * pSample);
	HRESULT Pipeline_Subpicture_MixSubpicture(LPBYTE dst, int w, int h);

	// ========================================================================================================
	// SMT :: PIPELINE - LINE21
	// ===============================================================

    HRESULT Pipeline_Line21_ReceiveLine21(IMediaSample * pSample);
	HRESULT Pipeline_Line21_MixLine21(LPBYTE dst, int w, int h);

	// ========================================================================================================
	// SMT :: VIDEO :: TELECINE & FRAME STRUCTURE
	// Prefix = TCNE 
	// ===============================================================

	void TCNE_extract(bool top_field, LPBYTE src);

	int USEROPTION_AllowTelecine;
	//(O) REFERENCE_TIME rtLast32Sample_StartTime;
	//(O)REFERENCE_TIME rtLast32Sample_EndTime;
	BYTE TCNE_buffer[720*480*2];
	int TCNE_FrameCounting_DisplayFrameNo;
	int TCNE_FrameCounting_EncodeFrameNo;
	bool TCNE_BufferIsEmpty;
	//(O) int sbTopFieldSource;
	//(O) int sbBottomFieldSource;
	//bool bLastRFF;
	bool TCNE_NeedTopField;
	
	//"TYPE DETERMINATION MODE" stuff
	//(O) bool bTDM;
	//(O) int TDMCnt;
	//(O) bool tff1, tff2, tff3, tff4;
	//(O) bool rff1, rff2, rff3, rff4;
	bool TCNE_tff_encoded;
	bool TCNE_Progressive;

	// ========================================================================================================
	// SMT :: VIDEO :: FRAME/FIELD
	// Prefix = FRFI
	// ===============================================================

	//(O) HRESULT FRFI_ReverseFieldOrder_imp(LPBYTE src, int h, int w, int pitch);
	//(O) HRESULT FRFI_BumpFieldsDown_imp(LPBYTE src, int h, int w, int pitch, int Cnt, bool SetLinesBlack);

	//(O) bool bForceReverseFieldOrder;
	bool FRFI_ForceBumpFieldsDown;
	int FRFI_DoFieldSplitting;

	// ========================================================================================================
	// SMT :: VIDEO :: FRAME STEPPING
	// Prefix = FRST
	// ===============================================================
	
	bool FRST_FrameStepping;
	bool FRST_DeliverFrameStepSample;

	// ========================================================================================================
	// SMT :: VIDEO :: VARISPEED
	// Prefix = VARI
	// ===============================================================

	HRESULT VARI_ProcessVarispeedSample(IMediaSample * pVSSample);

	bool VARI_InVarispeed;
	double VARI_MSVarispeedInterval;
	bool VARI_InFFRW;
	int FFRW_Rate;
	bool FFRW_2X_DropFrameFlag; //used to drop every other frame when running at 2X

	// ========================================================================================================
	// SMT :: VIDEO :: SCALING
	// Prefix = SCLE
	// ===============================================================

	bool SCLE_ZoomInit(int w, int h);
	void SCLE_ZoomDeInit();
	bool SCLE_ZoomYUY2(LPBYTE src, int w, int h, RECT r);
	void SCLE_Resize1D(BYTE* d, int dw, BYTE* s, int sw, int step);
	bool SCLE_ShrinkV(LPBYTE dst, int dh, LPBYTE src, int sh, int w, BOOL bInterlaced);
	bool SCLE_ShrinkV(LPBYTE dst, int dh, int dstpitch, LPBYTE src, int sh, int srcpitch, int wbytes);
	HRESULT SCLE_SetupVideoSizing();
	HRESULT SCLE_Resize(LPBYTE src);
	//(O) HRESULT SizeToPanScan(LPBYTE src, int h, int w);
	//(O) HRESULT SizeToLetterbox(LPBYTE orig, int h, int w, DWORD* LBColor);

	int SCLE_ActiveResizeMode; //1=anamorphic, 2=PanScan, 3=Letterbox
	int SCLE_DesiredResizeMode; //1=anamorphic, 2=PanScan, 3=Letterbox
	DWORD SCLE_BarColor;

	int SCLE_LB_OK;
	int SCLE_PS_OK;

	// ========================================================================================================
	// SMT :: VIDEO :: SAMPLE TIMING
	// Prefix = TMNG
	// ===============================================================

	//(O) bool bRestartTimeStamps;
	DWORD TMNG_LastFrameReceived_Ticks;
	//(O) REFERENCE_TIME rtSampleReceived;
	//(O) REFERENCE_TIME rtSampleDelivered;
	//(O) CRefTime crtSampleReceived;
	//(O) CRefTime crtSampleDelivered;
	//(O) DWORD dwSampleReceived;
	//(O) DWORD dwSampleDelivered;
	REFERENCE_TIME TMNG_LastSampleEndTime;
	REFERENCE_TIME TMNG_LastSampleStartTime;
	bool RunWasCalled;
	HRESULT CheckForPresentationTimeStreamTimeDiscrepency(IMediaSample * pOutSample);


	// ========================================================================================================
	// SMT :: VIDEO :: LOGGING
	// ===============================================================

	HRESULT LogMPEGFlags(IMediaSample * pIn);
	void LogSampleMetadata(IMediaSample * pSample);

	//(O) bool bInJacketPictureMode;

	// ========================================================================================================
	// SMT :: SUBPICTURE
	// Prefix = SUB
	// ===============================================================

	stMixData SUB_ActiveMixData;
	int SUB_LastSub_Width, SUB_LastSub_Height, SUB_LastSub_X, SUB_LastSub_Y;
	int USEROPTION_SubpictureLocation_X, USEROPTION_SubpictureLocation_Y;
	int USEROPTION_DoHighContrastSubpicture;

	// ========================================================================================================
	// SMT :: LINE-21
	// Prefix = L21
	// ===============================================================

	HRESULT L21_AddBufferToLine21Queue(LPBYTE pL21Buffer);
	LPBYTE L21_PopSampleFromLine21Queue();
	bool L21_GetLine21QueueMutex( HANDLE hMutex, DWORD dwMaxWaitMsec );

	stMixData L21_ActiveMixData;
	int USEROPTION_Line21Position_X, USEROPTION_Line21Position_Y;
	bool L21_IsInitialized;
	//(O) bool Line21_SyncLock;
	//(O) bool Line21_AcceptSample;
	queue <LPBYTE> L21_SampleQueue;
	HANDLE L21_SampleQueue_AccessMutex;                   // this is a mutex handle to access the Q.

	// ========================================================================================================
	// SMT :: MIXING :: ON-SCREEN DISPLAY 
	// Prefix = OSD
	// ===============================================================

	bool USEROPTION_DoOSDMix;

	HRESULT OSD_MixOnScreenDisplay(LPBYTE dst, int w, int h);

	stMixData OSD_ActiveMixData;
	int OSD_Ticker;
	int OSD_TickGoal;

	// ========================================================================================================
	// SMT :: MIXING :: GUIDES
	// Prefix = GIDE
	// ===============================================================

	HRESULT GIDE_MixGuides(LPBYTE dst, int w, int h, bool bUYVY);

	bool USEROPTION_MixGuides;
	int USEROPTION_ShowActionTitleGuides;

	int GIDE_Guide_L, GIDE_Guide_T, GIDE_Guide_R, GIDE_Guide_B;
	DWORD GIDE_GuideColor;
	DWORD GIDE_ActionTitleSafeColor;

	// ========================================================================================================
	// SMT :: MIXING :: GOP TIMECODE
	// Prefix = GPTC
	// ===============================================================

	short USEROPTION_MixGOPTimecode;
	short USEROPTION_MixGOPTimecode_RedIFrames;

	HRESULT GPTC_MixGOPTimecode(LPBYTE dst, IMediaSample * pOut, int w, int h);
	int _tmain(int argc, _TCHAR* argv[]);
	bool GPTC_Init(HDC& hDC, HBITMAP& hBM, HFONT& hFont, int w, int h);
	void GPTC_Free(HDC& hDC, HBITMAP& hBM, HFONT& hFont);
	void GPTC_RTtoBMP(HDC hDC, HBITMAP hBM, HFONT hFont, REFERENCE_TIME rt, double fps, DWORD fgcolor = 0xffffff, DWORD bgcolor = 0x000000);

	HDC GPTC_BurnIn_hDC;
	HBITMAP GPTC_BurnIn_hBM;
	HFONT GPTC_BurnIn_hFONT;

	LONGLONG GPTC_LastGOPHeaderTimecodeValue;
	LONGLONG GPTC_ATPFSinceLastGOPHeader;

	// ========================================================================================================
	// SMT :: MIXING :: TRIAL WARNING
	// Prefix = TRWA
	// ===============================================================

	HRESULT TRWA_MixTrialWarning(LPBYTE dst, IMediaSample * pOut, int w, int h);
	bool TRWA_InitTrialWarningMixing(HDC& hDC, HBITMAP& hBM, HFONT& hFont, int w, int h);
	void TRWA_FreeTrialWarningMixing(HDC& hDC, HBITMAP& hBM, HFONT& hFont);

	HDC TRWA_BurnIn_hDC;
	HBITMAP TRWA_BurnIn_hBM;
	HFONT TRWA_BurnIn_hFONT;

	// ========================================================================================================
	// SMT :: MIXING :: TICKS
	// Prefix = TCKS
	// ===============================================================

	void TCKS_BurnInTicks(LPBYTE dst, int w, int h);

	HDC TCKS_BurnIn_hDC;
	HBITMAP TCKS_BurnIn_hBM;
	HFONT TCKS_BurnIn_hFONT;

	// ========================================================================================================
	// SMT :: BUFFER COPIES / COLOR SPACE CONVERSION
	// ===============================================================

	//(O) void YUVSquare(LPBYTE dst, int x, int y, int dstpitch, int w, int h);
	void CopyBuffer_Rect(LPBYTE src, LPBYTE dst, int pitch, int w, int h); //Just copies identical data. No change.
	void CopyBuffer_Rect_UYVY(LPBYTE src, int srcx, int srcy, int srcpitch, LPBYTE dst, int dstx, int dsty, int dstpitch, int w, int h);
	void CopyBuffer_Rect_YUY2toUYVY(const BYTE* src, int srcx, int srcy, int srcpitch, LPBYTE dst, int dstx, int dsty, int dstpitch, int w, int h);
	void CopyBuffer_Line_YUY2toUYVY(const BYTE* src, LPBYTE dst, int w);
	HRESULT SetBufferColor_YUY2(LPBYTE dst, int dstpitch, int w, int h);

	// ========================================================================================================
	// SMT :: INCOMING MEDIATIME EVENTS
	// Prefix = MTEV
	// ===============================================================

	int MTEV_ThrowMediaTimeEvents;
	int MTEV_TypeOfMediaTimeEvents;

	// ========================================================================================================
	// SMT :: SAMPLE GRABBING
	// Prefix = SAGR
	// ===============================================================

	HRESULT SAGR_HandleSampleGrab(IMediaSample *imsSample);
	HRESULT SAGR_TellPhoenixToGetFrameGrab(IMediaSample * pSample, int w, int h, int UYVY);
	HRESULT HandleMultiFrameGrabSample(IMediaSample *IMS);
	HRESULT HandleSampleGrab_LastOutput();

	int USEROPTION_SampleWhat;

	bool SAGR_SampleOperationUnderway;
	LPBYTE SAGR_Sample;
	long SAGR_SampleSize;
	bool SAGR_GetSample;
	long SAGR_SampleWidth, SAGR_SampleHeight;
	LPCTSTR SAGR_DumpPath;

	int SAGR_MultiFrameCount;

	LPBYTE SAGR_LastBufferReceived;
	LPBYTE SAGR_LastBufferReceived_Scaled;
	LPBYTE SAGR_LastBufferOutput;

	//FROM KEYHD
	bool SAGR_RecordingImages;
	LPWSTR SAGR_ImageRecordTargPath;


	// ========================================================================================================
	// SMT :: BUFFER SAVE
	// Prefix = BUSA
	// ===============================================================

	HRESULT BUSA_SaveBufferToFile(const char * FileName, const char * Mode, LPBYTE buf, int bufsize, int w, int h);
	HRESULT BUSA_SaveBufferToFile_A(const char * FileName, const char * Mode, LPBYTE buf, int length);
	HRESULT BUSA_SaveBitmapRasterData(LPBYTE src);

	// ========================================================================================================
	// SMT :: BAR DATA DETECTION
	// Prefix = BDDT
	// ===============================================================

	HRESULT BDDT_GenerateBarData(LPBYTE buf, int w, int h);

	bool USEROPTION_DetectBarData;

	double BDDT_top_bar;
	double BDDT_bottom_bar;
	double BDDT_left_bar;
	double BDDT_right_bar;
	bool BDDT_burn_demarcation;
	int BDDT_luma_tolerance; //up from 16  'max = 20
	int BDDT_chroma_tolerance; //max = 10
	double BDDT_weight; //used to calculate the average value

	// ========================================================================================================
	// SMT :: MEDIATYPE
	// Prefix = MDTY
	// ===============================================================

	bool MDTY_CheckForMediaTypeChange(IMediaSample *pIn);
	//(O) HRESULT MDTY_ConvertMediaType_480to486(CMediaType * pMT);
	//(O) int MDTY_GetMediaTypeHeight(const CMediaType * pMT);
	//(O) HRESULT PutMTHeight(CMediaType * pMT, int nHeight);

	// ========================================================================================================
	// SMT :: GENERAL USER OPTIONS
	// ===============================================================

	bool USEROPTION_FeedbackClicks;
	int USEROPTION_ClientType;

	// ========================================================================================================
	// SMT :: UTILITY METHODS
	// Prefix = UTMT
	// ===============================================================

	//(O) int UTMT_GetRectWidth(const RECT& r);
	//(O) int UTMT_GetRectHeight(const RECT& r);
	void UTMT_REFERENCE_TIMEtoHMSF(REFERENCE_TIME rt, double fps, int& h, int& m, int& s, int& f);
	void UTMT_TEXTtoBMP(HDC hDC, HBITMAP hBM, HFONT hFont, const char * str, DWORD fgcolor, DWORD bgcolor);

	// ========================================================================================================
	// SMT :: SAMPLE RESENDING
	// Prefix = SPRS
	// ===============================================================

	IMediaSample * SPRS_LastSampleSent;
	HRESULT SPRS_CloneMediaSample(IMediaSample *pSource, IMediaSample *pDest);

	// ========================================================================================================
	// SMT :: GENERAL PLAYBACK META
	// ===============================================================

	int Pausing;

	// ========================================================================================================
	// SMT :: FRAMERATE CALCULATION
	// Prefix = FRRT
	// ===============================================================

	void FRRT_TickInputRate(DWORD dwCurrentTicks);
	void FRRT_TickOutputRate(DWORD dwCurrentTicks);

	HRESULT FRRT_CalculateInputFramerate(int MSCount, short FrameCount);
	HRESULT FRRT_CalcOutputFramerate(int MSCount, short FrameCount);

	REFERENCE_TIME FRRT_TargetFramerate_In_ATPF;
	REFERENCE_TIME FRRT_TargetFramerate_Out_ATPF;

	REFERENCE_TIME FRRT_ActualFramerate_In_ATPF;
	REFERENCE_TIME FRRT_ActualFramerate_Out_ATPF;

	float FRRT_ActualFramerate_In;
    float FRRT_ActualFramerate_Out;

	DWORD FRRT_OneSecondInterval_StartTicks_In;
	DWORD FRRT_OneSecondInterval_StartTicks_Out;

	short FRRT_FramesPassed_1s_In;
	short FRRT_FramesPassed_1s_Out;

	DWORD FRRT_TenSecondInterval_StartTicks_In;
	DWORD FRRT_TenSecondInterval_StartTicks_Out;

	short FRRT_FramesPassed_10s_In;
	short FRRT_FramesPassed_10s_Out;

	// ========================================================================================================
	// SMT :: TIMESTAMPING
	// Prefix = TIST
	// ===============================================================

	bool USEROPTION_SetNULLTimestamps;

	//(O) REFERENCE_TIME m_OutTime;
	//(O) REFERENCE_TIME m_InTime;
	//(O) LONGLONG llMediaTime_LastSamp_Start;  //Set start MediaTimes to 0 at Run, then increment by ATPF
	//(O) LONGLONG llMediaTime_LastSamp_End;
	//(O) LONGLONG llFrameCounter;  //The docs seem to indicate that MediaTimes should actuall be frame numbers.

	DWORD TIST_LastSampleProcessedAt; //Used for gap detection between samples.

	//(O) LONG lSumOfProcessingTimes;
	//(O) LONG lSampleCount;

	//USED BY THE FRAME DROP DETECTION JUST AFTER SENDING SAMPLE DOWNSTREAM
	REFERENCE_TIME rtLastStart;
	REFERENCE_TIME rtLastEnd;

		// ========================================================================================================
	// SMT :: LOCKING & DONGLING
	// Prefix = LOCK
	// ===============================================================

	bool LOCK_FilterIsLocked;
	bool LOCK_TRIALMODE;
	//bool LOCK_TRIALOVERRIDE;
	int LOCK_FrameCount;
	int static const LOCK_TimeoutMinutes = 5;

	/* Sentinel Keys Key Information */
	DWORD DeveloperID;          /* Developer ID of Sentinel Keys key */
	DWORD    flags;                /* flags for License  */
	HANDLE   licHandle;            /* Handle for License  */
	/* Variables for the Sentinel Keys API */
	DWORD     status;              /* The Sentinel Keys API return codes */

	HRESULT IsDongleExpired(SP_HANDLE licHandle, SP_DWORD devID);

	// ========================================================================================================
	// SMT :: PIN INFO
	// Prefix = PIxx
	// ===============================================================

	//VIDEO INPUT
	void HandleVideoPinConnected(CMediaType* mtIn);

	CMediaType PIVI_MediaType;
	VIDEOINFOHEADER * PIVI_VIH;
	VIDEOINFOHEADER2 * PIVI_VIH2;
	bool PIVI_FormatIsVIH2;
	long PIVI_ConnectedHeight;
	long PIVI_ConnectedWidth;
	int PIVI_LastConnectedHeight;
	long PIVI_BufferSize;
	int PIVI_Pitch;

	//OUTPUT
	void HandleOutputPinConnected(CMediaType* mtIn);

	CMediaType PIVO_MediaType;
	VIDEOINFOHEADER * PIVO_VIH;
	VIDEOINFOHEADER2 * PIVO_VIH2;
	bool PIVO_FormatIsVIH2;
	long PIVO_ConnectedHeight;
	long PIVO_ConnectedWidth;
	long PIVO_BufferSize;
	REFERENCE_TIME PIVO_ATPF;
	int PIVO_Pitch;

	bool USEROPTION_ForceOutputConnectSize;
	int USEROPTION_ForceOutput_W;
	int USEROPTION_ForceOutput_H;

	//SUBPICTURE
	CMediaType PISI_MediaType;
	//(O) LPBYTE pLastSubpictureBuffer;

	//LINE-21
	CMediaType PILI_MediaType;
	//(O) LPBYTE pLastL21Buffer;

	// ========================================================================================================
	// SMT :: PROCAMP
	// Prefix = PAMP
	// ===============================================================

	bool USEROPTION_DoProcAmp;
	bool USEROPTION_ProcAmpHalfFrame;
	bool USEROPTION_DoColorFiltering;
	int USEROPTION_WhichColorFilter;

	double PAMP_Brightness;
	double PAMP_Contrast;
	double PAMP_Saturation;
	double PAMP_Hue;
	double PAMP_Gamma;
	double PAMP_Chroma;
	double PAMP_Luma;
	double PAMP_Tint;
	double PAMP_Gain;
	double PAMP_Sharpness;
	
	// ========================================================================================================
	// SMT :: YUV DATA STILL IMAGE RENDER
	// Prefix = YVRD
	// ===============================================================

	LPBYTE YVRD_YUVBuffer;
	int YVRD_YUVBufferSize;
	bool YVRD_DoYUVStillRender;

	// ========================================================================================================
	// SMT :: BMP DATA STILL IMAGE RENDER
	// Prefix = BMRD
	// ===============================================================

	HRESULT BMRD_GetBMPSample(LPBYTE pBMP, int x, int y, int w, int h, IMediaSample** ppOutSample);
	HRESULT BMRD_SetBMPtoBuffer(LPBYTE pBMP, int w, int h, LPBYTE targetBuffer);

	// ========================================================================================================
	// SMT :: DVD JACKET PICTURE
	// ===============================================================

	HRESULT GetJPSample(LPCTSTR fn, int x, int y, int w, int h, IMediaSample** ppOutSample);

	// ========================================================================================================
	// SMT :: OPTIMIZED LEVEL
	// Prefix = OPTM
	// ===============================================================
	
	//NOTES: optimization level only applies to features/functionality that default on.
	//Features that are user activated are never adjusted by optimization level.

	int OPTM_OptimizedLevel;

	// ========================================================================================================
	// SMT :: RENDERER SELECTION
	// ===============================================================

	int REN_Renderer;
	//matches eAVMode
	// 0 = VMR
	// 1 = Intensity
	// 2 = Decklink

	// ========================================================================================================
	// INTERFACES :: SHARED
	// ===============================================================

    // Reveals Interfaces
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);
    STDMETHODIMP GetPages(CAUUID* pPages);

	// ========================================================================================================
	// INTERFACES :: IKEYSTONE
	// ===============================================================

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
	STDMETHODIMP ShowUYVYFile(LPCTSTR stYUVPath, int X, int Y, int W, int H);
	STDMETHODIMP ClearUYVYFile();
	STDMETHODIMP ClearUYVYFile_A();
	STDMETHODIMP ShowUYVYBuffer(LPBYTE pUYVY, int W, int H, int X, int Y);
	STDMETHODIMP ActivateFFRW(int nRate);
	STDMETHODIMP DeactivateFFRW();
	STDMETHODIMP Pause(int nPause);
	STDMETHODIMP ResendLastSamp(int Unconditional);
	STDMETHODIMP SetNULLTimestamps();
	STDMETHODIMP SetProperty(GUID gProp, int iValue);
	STDMETHODIMP StartRecording(LPCWSTR pszFileName);
	STDMETHODIMP StopRecording();
	STDMETHODIMP SetTrialOverride(bool bOverride);
	STDMETHODIMP SendMediaTimeEvents(int bActive, int iType);
	STDMETHODIMP SetOptimizedLevel(int iOptimizedLevel);
	STDMETHODIMP SetRenderer(int iRenderer);

	// ========================================================================================================
	// INTERFACES :: IKEYSTONE_QUALITY
	// ===============================================================

    STDMETHODIMP get_TargetFR_Out(float *TargetFR);
    STDMETHODIMP get_TargetFR_Out_ATPF(LONGLONG *TargetFR_ATPF);
    STDMETHODIMP get_TargetFR_In(float *TargetFR);
    STDMETHODIMP get_TargetFR_In_ATPF(LONGLONG *TargetFR_ATPF);
    STDMETHODIMP get_ActualFR_Out(float *ActualFR);
    STDMETHODIMP get_ActualFR_Out_ATPF(LONGLONG *ActualFR_ATPF);
    STDMETHODIMP get_ActualFR_In(float *ActualFR);
    STDMETHODIMP get_ActualFR_In_ATPF(LONGLONG *ActualFR_ATPF);
    STDMETHODIMP get_Jitter_In(LONGLONG *InputJitter);
    STDMETHODIMP get_Jitter_Out(LONGLONG *OutputJitter);

	// ========================================================================================================
	// INTERFACES :: IKEYSTONE_MIXER
	// ===============================================================

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
	STDMETHODIMP BurnGOPTCs(int iBurnGOPTCs, int iBlinkRed);
	STDMETHODIMP SetActionTitleGuides(int iShowGuides, int Red, int Blue, int Green);
	STDMETHODIMP FieldSplit(int DoFieldSplit);
	STDMETHODIMP HighContrastSP(int DoHighContrastSP);
	STDMETHODIMP SetARFlags(int PS, int LB);
	STDMETHODIMP SetActionTitleSafeColor(int Red, int Green, int Blue);
	STDMETHODIMP BarDataConfig(int DetectBarData, int BurnGuides, int Luma_Tolerance, int Chroma_Tolerance);
	STDMETHODIMP BarDataReset();

	// ========================================================================================================
	// INTERFACES :: IKEYSTONE_PROCAMP
	// ===============================================================

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

	// ========================================================================================================
	// INTERFACES :: IVMRMIXERCONTROL9
	// ===============================================================

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

	// ========================================================================================================
	// INTERFACES :: IVMRFILTERCONFIG9
	// ===============================================================

	STDMETHODIMP SetImageCompositor( IVMRImageCompositor9 *lpVMRImgCompositor);
    STDMETHODIMP SetNumberOfStreams( DWORD dwMaxStreams);
    STDMETHODIMP GetNumberOfStreams( DWORD *pdwMaxStreams);
    STDMETHODIMP SetRenderingPrefs( DWORD dwRenderFlags);
    STDMETHODIMP GetRenderingPrefs( DWORD *pdwRenderFlags);
    STDMETHODIMP SetRenderingMode( DWORD Mode);
    STDMETHODIMP GetRenderingMode( DWORD *pMode);

	// ========================================================================================================
	// INTERFACES :: IAMFILTERMISCFLAGS
	// ===============================================================

	ULONG STDMETHODCALLTYPE GetMiscFlags( void );

	// ========================================================================================================
	// INTERFACES :: IMediaSeeking
	// ===============================================================
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

};
