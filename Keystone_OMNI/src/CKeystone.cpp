#include "appincludes.h"
#include "CKeystone.h"
#include <streams.h>
#include <measure.h>

#pragma warning ( disable : 4238 ) 

//#include "SentinelKeystypes.h"  /*  Header file for this License  */    
//#include "SentinelKeys.h"         /*  Header file for the Sentinel Keys client library  */
#include "SentinelKeysLicense.h"  /*  Header file for this License  */    


#define QI(i) (riid == __uuidof(i)) ? GetInterface((i*)this, ppv) :
#define QI2(i) (riid == IID_##i) ? GetInterface((i*)this, ppv) :

// =================================================================
// Implements the CKeystone class
// =================================================================

CKeystone::CKeystone(TCHAR *pName, LPUNKNOWN pUnk, REFCLSID  clsid) : 
	CBaseFilter(pName,pUnk,&m_csFilter, clsid),
    m_pVideo(NULL),
    m_pOutput(NULL),
    m_bEOSDelivered(FALSE),
    m_bQualityChanged(FALSE),
    m_bSampleSkipped(FALSE)

	{

#ifdef PERF
    RegisterPerfId();
#endif //  PERF

	HRESULT hr = S_OK;
	m_pVideo = new CKeystoneVideoPin(NAME("Keystone Video Pin"), this, &hr, L"Video");
	m_pOutput = new CKeystoneOutputPin(NAME("Keystone Output Pin"), this, &hr, L"Output");
	m_pSubpicture = new CKeystoneSubpicturePin(NAME("Keystone Subpicture Pin"), this, &hr, L"Subpicture");
	m_pLine21 = new CKeystoneLine21Pin(NAME("Keystone Line21 Pin"), this, &hr, L"Line21");

	// DONGLE CHECK TO SEE IF WE'RE IN TRIAL MODE
	DeveloperID = 0x972961FA;
	DWORD LICENSEID_ = 0x9FB9;
	flags = 32;

	//LOCK_TRIALOVERRIDE = false;

	//BOLINAS - UNLOCKED KEYSTONE
	LOCK_TRIALMODE = false;
	LOCK_FrameCount = INT_MAX;
	//status = SFNTGetLicense(DeveloperID, SOFTWARE_KEY, LICENSEID_, flags, &licHandle);
	//if (status != SP_SUCCESS)
	//{
	//	LOCK_TRIALMODE = true;
	//	LOCK_FrameCount = 30 * 60 * LOCK_TimeoutMinutes;
	//}
	//else
	//{
	//	//CHECK FOR EXPIRATION
	//	if (FAILED(IsDongleExpired(licHandle, DeveloperID)))
	//	{
	//		LOCK_TRIALMODE = true;
	//		LOCK_FrameCount = 30 * 60 * LOCK_TimeoutMinutes;
	//	}
	//	else
	//	{
	//		LOCK_TRIALMODE = false;
	//		LOCK_FrameCount = INT_MAX;
	//	}
	//}
	//BOLINAS - UNLOCKED KEYSTONE

	//MISCELANEOUS
	USEROPTION_FeedbackClicks = false;
	USEROPTION_ClientType = 0;
 	LOCK_FilterIsLocked = true;
	YVRD_DoYUVStillRender = false;
	//opFrameCnt = 0;
	//lSumOfProcessingTimes=0;

	//RENDERER
	REN_Renderer = 2;

	//FRAMERATE
	FRRT_ActualFramerate_In = 0;
	FRRT_ActualFramerate_Out = 0;
	FRRT_OneSecondInterval_StartTicks_In = 0;
	FRRT_OneSecondInterval_StartTicks_Out = 0;
	FRRT_FramesPassed_1s_In = 0;
	FRRT_FramesPassed_1s_Out = 0;
	FRRT_TargetFramerate_In_ATPF = 0;
	FRRT_TargetFramerate_Out_ATPF = 0;
	FRRT_ActualFramerate_In_ATPF = 0;
	FRRT_ActualFramerate_Out_ATPF = 0;

	//SUBPICTURE
	USEROPTION_SubpictureLocation_X=0;
	USEROPTION_SubpictureLocation_Y=0;
	USEROPTION_DoHighContrastSubpicture = 0;

	//FRAME DUMPING
	SAGR_DumpPath = _T("D:\\Temp"); //set default dump location
	SAGR_MultiFrameCount = 0;
	//lSampleCount=0;
	SAGR_SampleOperationUnderway = false;
	USEROPTION_SampleWhat = NULL;
	SAGR_Sample = NULL;
	SAGR_SampleSize = 0;
	SAGR_GetSample = false;
	SAGR_SampleWidth = 0;
	SAGR_SampleHeight = 0;
	SAGR_RecordingImages = false;

	//LINE21
	USEROPTION_Line21Position_X=40;
	USEROPTION_Line21Position_Y=0;
	L21_IsInitialized = false;
	//Line21_SyncLock = false;
	//Line21_AcceptSample = true;

	//TIMESTAMPING
	TIST_LastSampleProcessedAt = 0;
	TMNG_LastSampleStartTime = 0;
	TMNG_LastSampleEndTime = 0;
	USEROPTION_SetNULLTimestamps = false;
	RunWasCalled = false;

	//SCALING
	SCLE_LB_OK = 1;
	SCLE_PS_OK = 1;
	SCLE_BarColor = 0x00800080;
	SCLE_DesiredResizeMode = 1;
	SCLE_ActiveResizeMode = 1;

	//BAR DATA
	USEROPTION_DetectBarData = false;
	BDDT_top_bar = 0;
	BDDT_bottom_bar = 0;
	BDDT_left_bar = 0;
	BDDT_right_bar = 0;
	BDDT_burn_demarcation = false;
	BDDT_luma_tolerance = 20;
	BDDT_chroma_tolerance = 10;
	BDDT_weight = 0;

	//GOP TIMECODES
	USEROPTION_MixGOPTimecode = 0;
	USEROPTION_MixGOPTimecode_RedIFrames = 0;
	GPTC_Init(GPTC_BurnIn_hDC, GPTC_BurnIn_hBM, GPTC_BurnIn_hFONT, 100, 16);

	//FRAME STORAGE
	SAGR_LastBufferOutput = (LPBYTE)malloc(829440);
	SAGR_LastBufferReceived = (LPBYTE)malloc(829440);
	SAGR_LastBufferReceived_Scaled = (LPBYTE)malloc(829440);

	//PROCAMP
	USEROPTION_DoProcAmp = false;
	USEROPTION_DoColorFiltering = false;
	USEROPTION_WhichColorFilter = 0;
	
	//OSD
	USEROPTION_DoOSDMix = false;

	//Init Gabor's cc code
	CSCManual::InitCC();

	//MEDIATYPES
	PIVI_LastConnectedHeight = 480;
	USEROPTION_ForceOutputConnectSize = false;

	//TELECINE
	USEROPTION_AllowTelecine = 1;
	TCNE_tff_encoded = false;
	TCNE_Progressive = false;
	TCNE_FrameCounting_DisplayFrameNo = 0;
	TCNE_FrameCounting_EncodeFrameNo = 0;
	TCNE_NeedTopField = true;
	TCNE_BufferIsEmpty = true;
	for (LPBYTE i = TCNE_buffer; i < TCNE_buffer + 691200; i += 4)
	{
		//BLACK
		memset(i	, 0x00, 1);
		memset(i + 1, 0x80, 1);
		memset(i + 2, 0x00, 1);
		memset(i + 3, 0x80, 1);
	}

	//FRAME STEPPING
	FRST_FrameStepping = false;

	//VARISPEED
	VARI_InVarispeed = false;
	VARI_InFFRW = false;
	FFRW_2X_DropFrameFlag = false;

	//FRAME-FIELD
	FRFI_ForceBumpFieldsDown = false;
	FRFI_DoFieldSplitting = 0;

	//GUIDES
	GIDE_ActionTitleSafeColor = 0x00992277;

	//OUTPUT PIN
	USEROPTION_ForceOutput_W = 720;
	USEROPTION_ForceOutput_H = 486;
	
	//OPTIMIZATION LEVEL
	OPTM_OptimizedLevel = 0; //no optmimization (4 = full optimization)

	//ORPHENED
	//bRestartTimeStamps = false;
	//bForceReverseFieldOrder = false;
	//bInJacketPictureMode = false;
	//bTDM = false;
	//TDMCnt = 0;

}

#ifdef UNICODE
CKeystone::CKeystone(char     *pName,
                                   LPUNKNOWN pUnk,
                                   REFCLSID  clsid) :
    CBaseFilter(pName,pUnk,&m_csFilter, clsid),
    m_pVideo(NULL),
    m_pOutput(NULL),
    m_bEOSDelivered(FALSE),
    m_bQualityChanged(FALSE),
    m_bSampleSkipped(FALSE)
{
#ifdef PERF
    RegisterPerfId();
#endif //  PERF
	LOCK_FilterIsLocked = true;
}
#endif

// destructor

CKeystone::~CKeystone()
{
	// Delete the pins
    delete m_pVideo;
    delete m_pOutput;
	delete m_pSubpicture;
	delete m_pLine21;

	GPTC_Free(GPTC_BurnIn_hDC, GPTC_BurnIn_hBM, GPTC_BurnIn_hFONT);
	TRWA_FreeTrialWarningMixing(TRWA_BurnIn_hDC, TRWA_BurnIn_hBM, TRWA_BurnIn_hFONT);

	free(SAGR_LastBufferOutput);
	free(SAGR_LastBufferReceived);
	free(SAGR_LastBufferReceived_Scaled);

	SCLE_ZoomDeInit();

}

STDMETHODIMP CKeystone::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	CheckPointer(ppv, E_POINTER);

	return 
		QI2(IKeystone)
		QI2(IKeystoneProcAmp)
		QI2(IKeystoneMixer)
		QI2(IKeystoneQuality)
		QI2(ISpecifyPropertyPages)
		QI2(IVMRMixerControl9)
		QI2(IVMRFilterConfig9)
		QI2(IAMFilterMiscFlags)
		__super::NonDelegatingQueryInterface(riid, ppv);
}

STDMETHODIMP CKeystone::Run(REFERENCE_TIME tStart)
{
 	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | CKeystone::Run(%I64d)"), tStart));

	LONG_PTR outST = (LONG_PTR)lround(tStart/10000000);
	NotifyEvent(EC_KEYSTONE_RUN, outST, 0);

	RunWasCalled = true;
	//Beep(500, 1000);

	PISI_MediaType = m_pSubpicture->CurrentMediaType();
	PILI_MediaType = m_pLine21->CurrentMediaType();

	//VIDEO PIN HANDLED in VideoPin_Connected.cpp
	//OUTPUT PIN HANDLED in OutputPin_Connected.cpp

	//CAutoLock lck2(&m_csReceive);
	return __super::Run(tStart);
}

// return the number of pins we provide
int CKeystone::GetPinCount()
{
    return 4;
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

    if (m_pVideo == NULL) 
    {
        // Create our Video Pin
        m_pVideo = new CKeystoneVideoPin(NAME("Video pin"),
											this,       // Owner filter
											&hr,        // Result code
											L"Video");  // Pin name

        ASSERT(SUCCEEDED(hr));

        if (m_pVideo == NULL) 
        {
           DbgLog((LOG_TRACE,1,TEXT("ERROR Creating Video Pin!")));
            return NULL;
        }

         // The lone output pin
        m_pOutput = new CKeystoneOutputPin(NAME("Transform output pin"), this, &hr, L"Out" );   

        ASSERT(SUCCEEDED(hr));

        if (m_pOutput == NULL) 
        {
           // OUTPUT DEBUG MESSAGE HERE!!!
           DbgLog((LOG_TRACE,1,TEXT("ERROR Creating output pin!")));
            delete m_pVideo;
            m_pVideo = NULL;
            return NULL;
        }

        // Our subpicture pin
        m_pSubpicture = new CKeystoneSubpicturePin(NAME( "Keystone Subpicture Pin"), this, &hr, L"Subpicture" );

        ASSERT( SUCCEEDED( hr ) );

        if( m_pSubpicture == NULL )
        {
            // OUTPUT DEBUG MESSAGE HERE!!!
            DbgLog((LOG_TRACE,1,TEXT("ERROR Creating subpicture pin!")));
            delete m_pVideo;
            delete m_pOutput;
            m_pVideo = NULL;
            m_pOutput = NULL;
            return NULL;
        }

        // And last but not least, our line21 pin...
        m_pLine21 = new CKeystoneLine21Pin( NAME( "Keystone Line21 Pin" ), this, &hr, L"Line 21" );

        ASSERT( SUCCEEDED( hr ) );

        if( m_pLine21 == NULL )
        {
            // OUTPUT DEBUG MESSAGE HERE!!!
           DbgLog((LOG_TRACE,1,TEXT("ERROR Creating Line21 Pin!")));
            delete m_pVideo;
            delete m_pOutput;
            delete m_pSubpicture;
            m_pVideo = NULL;
            m_pOutput = NULL;
            m_pSubpicture = NULL;
            return NULL;
        }
    }

    // Return the appropriate pin

    if (n == 0) 
	{
        return m_pVideo;
    } 
	else if (n == 1) 
	{
        return m_pOutput;
    } 
	else if (n == 2)
	{
		return m_pSubpicture;
	}
	else if (n == 3)
	{
		return m_pLine21;
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

    if (0==lstrcmpW(Id,L"Video")) {
        *ppPin = GetPin(0);
    } else if (0==lstrcmpW(Id,L"Output")) {
        *ppPin = GetPin(1);
    } else if (0==lstrcmpW(Id,L"Subpicture")) {
        *ppPin = GetPin(2);
    } else if (0==lstrcmpW(Id,L"Line21")) {
        *ppPin = GetPin(3);
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
 	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | CKeystone::EndOfStream()")));
	NotifyEvent(EC_KEYSTONE_ENDOFSTREAM, 0, 0);

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
 	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | CKeystone::BeginFlush()")));
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
 	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | CKeystone::EndFlush()")));
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
 	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | CKeystone::Stop()")));
    CAutoLock lck1(&m_csFilter);
    if (m_State == State_Stopped) {
        return NOERROR;
    }

    // Succeed the Stop if we are not completely connected
    ASSERT(m_pVideo == NULL || m_pOutput != NULL);
    if (m_pVideo == NULL || m_pVideo->IsConnected() == FALSE || m_pOutput->IsConnected() == FALSE) {
        m_State = State_Stopped;
        m_bEOSDelivered = FALSE;
        return NOERROR;
	}

    ASSERT(m_pVideo);
    ASSERT(m_pOutput);
	ASSERT(m_pLine21);
	ASSERT(m_pSubpicture);

    // decommit the input pins before locking or we can deadlock
    m_pVideo->Inactive();
	m_pLine21->Inactive();
	m_pSubpicture->Inactive();

    // synchronize with Receive calls
    //CAutoLock lck2(&m_csReceive);
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
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE OMNI | CKeystone::Pause()")));

    CAutoLock lck(&m_csFilter);
    HRESULT hr = NOERROR;

    if (m_State == State_Paused) 
	{
        // (This space left deliberately blank)
    }

    // If we have no input pin or it isn't yet connected then when we are
    // asked to pause we deliver an end of stream to the downstream filter.
    // This makes sure that it doesn't sit there forever waiting for
    // samples which we cannot ever deliver without an input connection.
    else if (m_pVideo == NULL || m_pVideo->IsConnected() == FALSE) 
	{
        if (m_pOutput && m_bEOSDelivered == FALSE) 
		{
            m_pOutput->DeliverEndOfStream();
            m_bEOSDelivered = TRUE;
        }
        m_State = State_Paused;
    }

    // We may have an input connection but no output connection
    // However, if we have an input pin we do have an output pin
    else if (m_pOutput->IsConnected() == FALSE) 
	{
        //return VFW_E_NOT_CONNECTED;
    }
    else 
	{
		//if (m_State == State_Stopped) 
		//{
		//	//CAutoLock lck2(&m_csReceive);
		//	hr = S_OK;
		//}
		//if (SUCCEEDED(hr)) 
		//{
		//}
		hr = CBaseFilter::Pause();
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

HRESULT CKeystone::IsDongleExpired(SP_HANDLE licHandle, SP_DWORD devID)
{
    SP_STATUS           status;
    SP_DWORD            qrySize, respSize;
    SP_DWORD            flags = 0;
    int                 qryTableSize;
    int                 i = 1, j = 0;
    unsigned char*      response;
    SP_FEATURE_INFO     featureInfo;

    /* Get flags */
    flags = SP_STANDALONE_MODE;
    
    /* Calculate the number of queries in the query table. This query table is defined
    * in the header SentinelKeysLicense.h, which is generated by Sentinel Keys Toolkit*/

    qryTableSize = sizeof( QUERY_SP_EXPIRE_AES_SP_SIMPLE_QUERY );
    qrySize = sizeof(QUERY_SP_EXPIRE_AES_SP_SIMPLE_QUERY[j]);
    
    response = (unsigned char*)malloc(qrySize+1);
    respSize = qrySize;

    for (i=1; i<=SP_NUM_OF_QR; i++) 
    { 
           /* This will check the "Expiration Date"
           * The API will return eror if date has expired */

           status = SFNTQueryFeature(licHandle,
                                    SP_EXPIRE_AES,
                                    SP_SIMPLE_QUERY,
                                    QUERY_SP_EXPIRE_AES_SP_SIMPLE_QUERY[j],
                                    qrySize,
                                    response,
                                    respSize);

            /* If query fails, release the license and return */
            if( status != SP_SUCCESS)
            {
                //printf( "\nQuery to key failed. Return code: %d.", status );
                SFNTReleaseLicense(licHandle);
                free(response);
                return S_OK; //NOTE: EXPIRE is not set in the dongle. It does not expire. And must be permitted to run.
            }

            /* Now check whether the response returned is a valid one, 
            * by comparing with the response table defined in sentinelkeyslicense.h. */
            for(SP_DWORD q = 0; q < qrySize; q++) 
            {
                if(response[q] != RESPONSE_SP_EXPIRE_AES_SP_SIMPLE_QUERY[j][q])
                {
                    //printf( "\nInvalid Query Response.\n");
                    SFNTReleaseLicense(licHandle);
                    free(response); 
                    return E_FAIL;
                }
            }

        j++;

    } /* end of for loop */ 

    //* Check the number of days left for application to expire */
    memset(&featureInfo,0,sizeof(featureInfo));
    status = SFNTGetFeatureInfo(licHandle, SP_EXPIRE_AES, &featureInfo);
    if( status != SP_SUCCESS)
    {
        //printf( "\n SFNTGetFeatureInfo failed. Return code: %d.", status );
        SFNTReleaseLicense(licHandle);
        free(response);
        return E_FAIL;
    }
    else
    {
        SP_DATE_TIME dateTime;
        memset(&dateTime,0,sizeof(dateTime));
        memcpy(&dateTime,&(featureInfo.timeControl.stopTime), sizeof(dateTime));
        //printf("\n Your application would expire on (format - Date YYYY\\MM\\DD\\ Time - HH:MM:SS) :%ld\\%d\\%d Time %d:%d:%d  (GMT)",
        //                                    dateTime.year, dateTime.month, dateTime.dayOfMonth,
        //                                    dateTime.hour, dateTime.minute, dateTime.second);      
		SFNTReleaseLicense(licHandle);
		free(response);
		return S_OK;
    }
}
