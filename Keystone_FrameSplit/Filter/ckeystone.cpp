#include "../Utility/appincludes.h"

#include "CKeystone.h"
#include <streams.h>
#include <measure.h>

#pragma warning ( disable : 4238 ) 

#define QI(i) (riid == __uuidof(i)) ? GetInterface((i*)this, ppv) :
#define QI2(i) (riid == IID_##i) ? GetInterface((i*)this, ppv) :

// =================================================================
// Implements the CKeystone class
// =================================================================

CKeystone::CKeystone(TCHAR     *pName,
                                   LPUNKNOWN pUnk,
                                   REFCLSID  clsid) :
    CBaseFilter(pName,pUnk,&m_csFilter, clsid),
    m_pVideoOne(NULL),
    m_pOutput(NULL),
    m_bEOSDelivered(FALSE),
    m_bQualityChanged(FALSE),
    m_bSampleSkipped(FALSE),
	m_iFrame(0)
	{
#ifdef PERF
    RegisterPerfId();
#endif //  PERF

	HRESULT hr = S_OK;
	m_pVideoOne = new CKeystoneVideoOnePin(NAME("Keystone VideoOne Pin"), this, &hr, L"VideoOne");
	m_pVideoTwo = new CKeystoneVideoTwoPin(NAME("Keystone VideoTwo Pin"), this, &hr, L"VideoTwo");
	m_pOutput = new CKeystoneOutputPin(NAME("Keystone Output Pin"), this, &hr, L"Output");

	m_OutTime = 0;
	m_InTime = 0;
	bFeedbackClicks = false;
	fActualFR_Out = 0;
	fActualFR_In = 0;
	bFilterIsLocked = false;
	bForceOutputConnectSize = false;
	bDoingYUVFile = false;
	dwSecStartTicks_Out = 0;
	dwSecStartTicks_In = 0;
	sFramesPassedThisSec_Out = 0;
	sFramesPassedThisSec_In = 0;
	llTargetFR_Out_ATPF = 0;
	llTargetFR_In_ATPF = 0;
	iDumpFrameCount = 0;
	SP_X=0;
	SP_Y=0;
	//L21_X=40;
	//L21_Y=0;
	opFrameCnt = 0;
	LB_OK = 1;
	PS_OK = 1;

	AnchorX = 0;
	AnchorY = 0;
	R1_X = 50;
	R1_Y = 50;
	R2_X = 50;
	R2_Y = 50;

	bRecordingImages = false;
	CurrentTimeFormat = TIME_FORMAT_MEDIA_TIME;

	//GOP TCs
	iMixGOPTC=0;
	InitGOPTC(m_hDC, m_hBM, m_hFONT, 100, 16);

	LastFrameOutput = (LPBYTE)malloc(829440);

	//Video xf
	bDoProcAmp = false;
	bDoColorFiltering = false;
	iWhichColorFilter = 0;
	
	lSumOfProcessingTimes=0;
	lSampleCount=0;

	bSampleOperationUnderway = false;
	SampleWhat = NULL;
	pSample = NULL;
	lSampleSize = 0;
	bGetSample = false;
	lSamp_W = 0;
	lSamp_H = 0;

	//Mixer var initialization
	//OSD Mix
	bPendingOSDMix = false;

	//Init Gabor's cc code
	CSCManual::InitCC();
	LastConnectedHeight_VidIn = 480;

	// DM
	m_iQueueRd = 0;
	m_iQueueWr = 0;
	m_aQueue = NULL;
	VTM_HaveData = 0;

	m_bPinTwoIsWaiting = FALSE;

	SECURITY_ATTRIBUTES attb;
	attb.nLength = sizeof(attb);
	attb.lpSecurityDescriptor = NULL;
	attb.bInheritHandle = FALSE;

	m_hEvent = ::CreateEvent(
	&attb,
	FALSE,			//BOOL bManualReset,
	TRUE,			//BOOL bInitialState,
	NULL);			///LPCTSTR lpName
}

#ifdef UNICODE
CKeystone::CKeystone(char     *pName,
                                   LPUNKNOWN pUnk,
                                   REFCLSID  clsid) :
    CBaseFilter(pName,pUnk,&m_csFilter, clsid),
    m_pVideoOne(NULL),
    m_pOutput(NULL),
    m_bEOSDelivered(FALSE),
    m_bQualityChanged(FALSE),
    m_bSampleSkipped(FALSE)
{
#ifdef PERF
    RegisterPerfId();
#endif //  PERF
	bFilterIsLocked = true;
}
#endif

// destructor

CKeystone::~CKeystone()
{
	::CoTaskMemFree(m_aQueue);

	CloseHandle(m_hEvent);

	// Delete the pins
    delete m_pVideoOne;
	delete m_pVideoTwo;
    delete m_pOutput;
	FreeGOPTC(m_hDC, m_hBM, m_hFONT);
}

STDMETHODIMP CKeystone::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	CheckPointer(ppv, E_POINTER);

	//IVMRVideoStreamControl
    //uuid(058d1f11-2a54-4bef-bd54-df706626b727),
    //helpstring("IVMRMixerStreamConfig Interface"),
	//IID_##i
	//__uuidof(058d1f11-2a54-4bef-bd54-df706626b727)
	//if (riid == IID_IVMRVideoStreamControl)
	//{
	//	int i = 1;
	//}

	return 
		QI2(IKeystone)
		QI2(IKeystoneProcAmp)
		QI2(IKeystoneMixer)
		QI2(IKeystoneQuality)
		QI2(ISpecifyPropertyPages)
		QI2(IVMRMixerControl9)
		QI2(IVMRFilterConfig9)
		QI2(IAMFilterMiscFlags)
		//QI(IMediaSeeking)
		//QI2(IKsPropertySet)
		__super::NonDelegatingQueryInterface(riid, ppv);
}

HRESULT CKeystone::CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut)
{
	HRESULT hr = VFW_E_TYPE_NOT_ACCEPTED;
	CMediaType mt;
	for (int i = 0; (m_pOutput->GetMediaType(i, &mt) == S_OK) && FAILED(hr); i++)
	{
		if (*mtOut == mt)
			hr = S_OK;
		mt.ResetFormatBuffer();
	}
	return hr;
}

STDMETHODIMP CKeystone::Run(REFERENCE_TIME tStart)
{
	m_mtVideoOne = m_pVideoOne->CurrentMediaType();
	m_mtVideoTwo = m_pVideoTwo->CurrentMediaType();

	if (m_mtVideoOne.formattype == FORMAT_VideoInfo2)
	{
		VIH2In = (VIDEOINFOHEADER2*)m_mtVideoOne.pbFormat;
		InIsVIH2 = true;
		lConnectedHeight_In = VIH2In->bmiHeader.biHeight;
		VIH2In->bmiHeader.biSizeImage = (VIH2In->bmiHeader.biHeight * VIH2In->bmiHeader.biWidth * 2);
		CurrentInputBufferSize = VIH2In->bmiHeader.biSizeImage; 
		llTargetFR_In_ATPF = VIH2In->AvgTimePerFrame;
	}
	else
	{
		VIHIn = (VIDEOINFOHEADER*)m_mtVideoOne.pbFormat;
		InIsVIH2 = false;
		lConnectedHeight_In = VIHIn->bmiHeader.biHeight;
		VIHIn->bmiHeader.biSizeImage = (VIHIn->bmiHeader.biHeight * VIHIn->bmiHeader.biWidth * 2);
		CurrentInputBufferSize = VIHIn->bmiHeader.biSizeImage; 
		llTargetFR_In_ATPF = VIHIn->AvgTimePerFrame;
	}

	m_mtOut = m_pOutput->CurrentMediaType();
	if (m_mtOut.formattype == FORMAT_VideoInfo2)
	{
		VIH2Out = (VIDEOINFOHEADER2*)m_mtOut.pbFormat;
		OutIsVIH2 = true;
		lConnectedHeight_Out = VIH2Out->bmiHeader.biHeight;
		VIH2Out->bmiHeader.biSizeImage = (VIH2Out->bmiHeader.biHeight * VIH2Out->bmiHeader.biWidth * 2);
		CurrentOutputBufferSize = VIH2Out->bmiHeader.biSizeImage; 
		llTargetFR_Out_ATPF = VIH2Out->AvgTimePerFrame;
		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Out ATPF= %I64d"), llTargetFR_Out_ATPF));
	}
	else
	{
		VIHOut = (VIDEOINFOHEADER*)m_mtOut.pbFormat;
		OutIsVIH2 = false;
		lConnectedHeight_Out = VIHOut->bmiHeader.biHeight;
		VIHOut->bmiHeader.biSizeImage = (VIHOut->bmiHeader.biHeight * VIHOut->bmiHeader.biWidth * 2);
		CurrentOutputBufferSize = VIHOut->bmiHeader.biSizeImage; 
		llTargetFR_Out_ATPF = VIHOut->AvgTimePerFrame;
	}	

	CAutoLock lck2(&m_csReceive);
	//llMediaTime_LastSamp_Start = 0;  //Set MediaTimes to 0 at Run, then increment by ATPF
	//llMediaTime_LastSamp_End = llTargetFR_Out_ATPF;
	llFrameCounter = 1;

	return __super::Run(tStart);
}

// return the number of pins we provide

int CKeystone::GetPinCount()
{
    return 3;
}


// return a non-addrefed CBasePin * for the user to addref if he holds onto it
// for longer than his pointer to us. We create the pins dynamically when they
// are asked for rather than in the constructor. This is because we want to
// give the derived class an oppportunity to return different pin objects

// We return the objects as and when they are needed. If either of these fails
// then we return NULL, the assumption being that the caller will realise the
// whole deal is off and destroy us - which in turn will delete everything.

CBasePin * CKeystone::GetPin(int n)
{
    HRESULT hr = S_OK;

    // Create our pins if neccessary...

    if (m_pVideoOne == NULL) 
    {
        // Create our Video Pin
        m_pVideoOne = new CKeystoneVideoOnePin(NAME("VideoOne pin"),
											this,       // Owner filter
											&hr,        // Result code
											L"VideoOne");  // Pin name

        ASSERT(SUCCEEDED(hr));

        if (m_pVideoOne == NULL) 
        {
           DbgLog((LOG_TRACE,1,TEXT("ERROR Creating VideoOne Pin!")));
            return NULL;
        }

         // The lone output pin
        m_pOutput = new CKeystoneOutputPin(NAME("Transform output pin"), this, &hr, L"Out" );   

        ASSERT(SUCCEEDED(hr));

        if (m_pOutput == NULL) 
        {
           // OUTPUT DEBUG MESSAGE HERE!!!
           DbgLog((LOG_TRACE,1,TEXT("ERROR Creating output pin!")));
            delete m_pVideoOne;
            m_pVideoOne = NULL;
            return NULL;
        }

        // And last but not least, our line21 pin...
        m_pVideoTwo = new CKeystoneVideoTwoPin( NAME( "Keystone VideoTwo Pin" ), this, &hr, L"VideoTwo" );

        ASSERT( SUCCEEDED( hr ) );

        if( m_pVideoTwo == NULL )
        {
            // OUTPUT DEBUG MESSAGE HERE!!!
           DbgLog((LOG_TRACE,1,TEXT("ERROR Creating VideoTwo Pin!")));
            delete m_pVideoOne;
            delete m_pOutput;
            m_pVideoOne = NULL;
            m_pOutput = NULL;
            return NULL;
        }
    }

    // Return the appropriate pin

    if (n == 0) 
	{
        return m_pVideoOne;
    } 
	else if (n == 1)
	{
        return m_pOutput;
    } 
	else if (n == 2)
	{
		return m_pVideoTwo;
	}
	else 
	{
        return NULL;
    }
}



//
// FindPin
//
// If Id is In or Out then return the IPin* for that pin
// creating the pin if need be.  Otherwise return NULL with an error.

STDMETHODIMP CKeystone::FindPin(LPCWSTR Id, IPin **ppPin)
{
    CheckPointer(ppPin,E_POINTER);
    ValidateReadWritePtr(ppPin,sizeof(IPin *));

    if (0==lstrcmpW(Id,L"VideoOne")) {
        *ppPin = GetPin(0);
    } else if (0==lstrcmpW(Id,L"Output")) {
        *ppPin = GetPin(1);
    } else if (0==lstrcmpW(Id,L"VideoTwo")) {
        *ppPin = GetPin(2);
    } else {
        *ppPin = NULL;
        return VFW_E_NOT_FOUND;
    }

    HRESULT hr = NOERROR;
    //  AddRef() returned pointer - but GetPin could fail if memory is low.
    if (*ppPin) {
        (*ppPin)->AddRef();
    } else {
        hr = E_OUTOFMEMORY;  // probably.  There's no pin anyway.
    }
    return hr;
}

// EndOfStream received. Default behaviour is to deliver straight
// downstream, since we have no queued data. If you overrode Receive
// and have queue data, then you need to handle this and deliver EOS after
// all queued data is sent
HRESULT CKeystone::EndOfStream(void)
{
    HRESULT hr = NOERROR;
    if (m_pOutput != NULL) {
        hr = m_pOutput->DeliverEndOfStream();
    }

    return hr;
}


// enter flush state. Receives already blocked
// must override this if you have queued data or a worker thread
HRESULT CKeystone::BeginFlush(void)
{
    HRESULT hr = NOERROR;
    if (m_pOutput != NULL) {
		// block receives -- done by caller (CBaseInputPin::BeginFlush)

		// discard queued data -- we have no queued data

		// free anyone blocked on receive - not possible in this filter

		// call downstream
		hr = m_pOutput->DeliverBeginFlush();
    }
	return hr;
}


// leave flush state. must override this if you have queued data
// or a worker thread
HRESULT CKeystone::EndFlush(void)
{
    // sync with pushing thread -- we have no worker thread

    // ensure no more data to go downstream -- we have no queued data

    // call EndFlush on downstream pins
    ASSERT (m_pOutput != NULL);
    return m_pOutput->DeliverEndFlush();

    // caller (the input pin's method) will unblock Receives
}


// override these so that the derived filter can catch them
STDMETHODIMP CKeystone::Stop()
{
//	_asm {int 3}
//	return __super::Stop();

    CAutoLock lck1(&m_csFilter);
    if (m_State == State_Stopped) {
        return NOERROR;
    }

    // Succeed the Stop if we are not completely connected
    ASSERT(m_pVideoOne == NULL || m_pOutput != NULL);
    if (m_pVideoOne == NULL || m_pVideoOne->IsConnected() == FALSE || m_pOutput->IsConnected() == FALSE) {
        m_State = State_Stopped;
        m_bEOSDelivered = FALSE;
        return NOERROR;
	}

    ASSERT(m_pVideoOne);
    ASSERT(m_pOutput);
	ASSERT(m_pVideoTwo);

    // decommit the input pins before locking or we can deadlock
    m_pVideoOne->Inactive();
	m_pVideoTwo->Inactive();

    // synchronize with Receive calls
    CAutoLock lck2(&m_csReceive);
    m_pOutput->Inactive();

    // allow a class derived from CKeystone
    // to know about starting and stopping streaming

    //HRESULT hr = StopStreaming();
    //if (SUCCEEDED(hr)) {
	// complete the state transition
	m_State = State_Stopped;
	m_bEOSDelivered = FALSE;
    //}
    return S_OK;
}

STDMETHODIMP CKeystone::Pause()
{
    CAutoLock lck(&m_csFilter);
    HRESULT hr = NOERROR;

	//hr = m_pOutput->m_pInputPin->Receive((IMediaSample*) &pSample);

    if (m_State == State_Paused) {
        // (This space left deliberately blank)
    }

    // If we have no input pin or it isn't yet connected then when we are
    // asked to pause we deliver an end of stream to the downstream filter.
    // This makes sure that it doesn't sit there forever waiting for
    // samples which we cannot ever deliver without an input connection.

    else if (m_pVideoOne == NULL || m_pVideoOne->IsConnected() == FALSE) {
        if (m_pOutput && m_bEOSDelivered == FALSE) {
            m_pOutput->DeliverEndOfStream();
            m_bEOSDelivered = TRUE;
        }
        m_State = State_Paused;
    }

    // We may have an input connection but no output connection
    // However, if we have an input pin we do have an output pin

    else if (m_pOutput->IsConnected() == FALSE) {
        m_State = State_Paused;
    }

    else {
	if (m_State == State_Stopped) {
	    // allow a class derived from CKeystone
	    // to know about starting and stopping streaming
            CAutoLock lck2(&m_csReceive);
	    //hr = StartStreaming();
		hr = S_OK;
	}
	if (SUCCEEDED(hr)) {
	    hr = CBaseFilter::Pause();
	}
    }

    m_bSampleSkipped = FALSE;
    m_bQualityChanged = FALSE;
    return hr;
}

STDMETHODIMP CKeystone::QueryFilterInfo(FILTER_INFO * pInfo)
{
    CheckPointer(pInfo,E_POINTER);
    ValidateReadWritePtr(pInfo,sizeof(FILTER_INFO));

	//Orig
    if (m_pName) {
        lstrcpynW(pInfo->achName, m_pName, sizeof(pInfo->achName)/sizeof(WCHAR));
    } else {
        pInfo->achName[0] = L'\0';
    }

	////To deliver a different name in FILTER_INFO
	//m_pName = L"Video Mixing Renderer 9";
	//lstrcpynW(pInfo->achName, m_pName, sizeof(pInfo->achName)/sizeof(WCHAR));

	pInfo->pGraph = m_pGraph;
    if (m_pGraph)
        m_pGraph->AddRef();
    return NOERROR;
}

// the following removes a very large number of level 4 warnings from the microsoft
// compiler output, which are not useful at all in this case.
#pragma warning(disable:4514)
