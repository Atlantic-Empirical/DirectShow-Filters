//#include <appincludes.h>
//#include "../VideoPin.h"
//
////// ATL
////#include <atlbase.h>
////#define __AFX_H__
//
//HRESULT CKeystoneVideoOnePin::FrameStep()
//{
//	if (bFrameStepUnderway == true) return S_OK; //Don't bug us right now, we're busy.
//	if (bFrameStepping == true)
//	{
//		bFrameStepUnderway = true;
//		DeliverFrameStepSample();
//	}
//	else
//	{
//		SetupFrameStepping();
//	}
//	return S_OK;
//}
//
//HRESULT CKeystoneVideoOnePin::SetupFrameStepping()
//{
//	//Let's try this. It will just cause the queue to fill up and the upstream filters to pause.
//	//The second time the user calls FrameStep they will advance one frame. SEEMS GOOD.
//	bFrameStepping = true;
//	return S_OK;	
//}
//
//HRESULT CKeystoneVideoOnePin::DeliverFrameStepSample()
//{
//	HRESULT hr;
//	IMediaSample * PoppedSample = PopSampleFromQueue();
//	if (PoppedSample == NULL) return E_FAIL;
//
//	//Set time stamps
//	REFERENCE_TIME rtStart, rtEnd;
//	CRefTime crtStreamTime;
//	m_pTransformFilter->StreamTime(crtStreamTime);
//	rtStart = crtStreamTime.m_time; //+ 100000;
//	rtEnd = rtStart + m_pTransformFilter->llTargetFR_Out_ATPF;
//	PoppedSample->SetTime(&rtStart, &rtEnd);
//	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Framestep sample.")));
//
//	//Deliver
//	hr = ProcessVideoSample(PoppedSample);
//	if FAILED(hr) return hr;
//	bFrameStepUnderway = false;
//	return S_OK;
//}
//
//HRESULT CKeystoneVideoOnePin::QuitFrameStepping()
//{
//	if (bFrameStepping == false) return S_OK;
//	bFrameStepping = false;
//	HRESULT hr;
//	IMediaSample * PoppedSample = NULL;
//
//	//Deliver all samples from queue
//	short QueueLength = (short) m_FrameQueue.size();
//    for (short s = 0; s <= QueueLength; s++)
//	{
//		//Pop next sample
//		PoppedSample = PopSampleFromQueue();
//		if (PoppedSample == NULL) return E_FAIL;
//
//		//Set time stamps
//		REFERENCE_TIME rtStart, rtEnd;
//		CRefTime crtStreamTime;
//		m_pTransformFilter->StreamTime(crtStreamTime);
//		rtStart = crtStreamTime.m_time; //+ 100000;
//		rtEnd = rtStart + m_pTransformFilter->llTargetFR_Out_ATPF;
//		PoppedSample->SetTime(&rtStart, &rtEnd);
//		DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Framestep sample on quit frame stepping.")));
//
//		//Deliver
//		hr = ProcessVideoSample(PoppedSample);
//		if FAILED(hr) return hr;
//	}
//	
//	//Unpause upstream filters
//	hr = RunNavDec();
//	if (hr != S_OK) return hr;
//
//	return S_OK;
//}
//
//// Pushes a sample onto the Q - input is a populated IMediaSample structure
//// returns S_OK or E_FAIL
//HRESULT CKeystoneVideoOnePin::PushSampleOntoQueue( IMediaSample * pIMediaSample )
//{
//	// create a new complete frame to copy that then push onto the queue.
//	strucCompleteFrame * pstrucCompleteFrame = new strucCompleteFrame;
//	HRESULT hr = S_OK;
//	hr = pIMediaSample->GetTime( &pstrucCompleteFrame->rtTimeStart, &pstrucCompleteFrame->rtTimeEnd );
//	if( FAILED( hr ) )
//	{
//		// TODO: output failure message here
//		delete pstrucCompleteFrame;
//		return E_FAIL;
//	}
//	
//	BYTE * pBuffer;
//	hr = pIMediaSample->GetPointer( &pBuffer );
//	if( FAILED( hr ) )
//	{
//		// TODO: output failure message here
//		delete pstrucCompleteFrame;
//		return E_FAIL;
//	}
//
//	memcpy( pstrucCompleteFrame->bSampleBuffer, pBuffer, sizeof( SAMPLE_SIZE_MAX ) );
//
//	hr = pIMediaSample->IsDiscontinuity();
//	if( hr == S_OK )
//		pstrucCompleteFrame->bIsDiscontinuity = true;
//	else
//		pstrucCompleteFrame->bIsDiscontinuity = false;
//
//
//	hr = pIMediaSample->IsSyncPoint();
//	if( hr == S_OK )
//		pstrucCompleteFrame->bIsSyncPoint = true;
//	else
//		pstrucCompleteFrame->bIsSyncPoint = false;
//
//
//	// So we have all of our data in the structure - let's get the mutex.
//	hr = GetMutex( m_hQAccessMutex, 2000 );
//	if( FAILED( hr ) )
//	{
//		// TODO: output failure message here
//		delete pstrucCompleteFrame;
//		return E_FAIL;
//	}
//
//	//// We're ready to push this onto the frame.  Let's check the size of the queue
//	queue <int>::size_type queuelength;
//	queuelength = m_FrameQueue.size();
//	if( queuelength >= 30 )
//	{
//		// Queue is at thirty - we need to pause the upstream filters.
//		// Since we don't want to loose any samples, we'll push this here anyways.
//		if (bNavDecRunning == true)
//		{
//			hr = PauseNavDec();
//			if (FAILED(hr)) return hr;
//		}
//		m_FrameQueue.push( *pstrucCompleteFrame );
//	}
//	else
//	{
//		//Let's toss it in there.
//		m_FrameQueue.push( *pstrucCompleteFrame );
//	}
//
//	ReleaseMutex( m_hQAccessMutex );
//	delete pstrucCompleteFrame;
//	return hr;
//}
//
//// Pops a sample from the queue - input is a pointer to an EMPTY IMediaSample
//// returns S_OK or E_FAIL
//IMediaSample * CKeystoneVideoOnePin::PopSampleFromQueue()
//{
//	// Can't pop a sample from a queue that doesn't have any, can we?
//	HRESULT hr;
//	queue <int>::size_type queuelength;
//	queuelength = m_FrameQueue.size();
//	if( queuelength < 1 )
//	{
//		// Whoops.  No more samples in the queue.  
//		// TODO: IF WE'RE IN STEP MODE, UNPAUSE THE SOURCE AND DECODER FILTERS AND FILL UP THE QUEUE AGAIN.
//		hr = RunNavDec();
//		//if (FAILED(hr)) return NULL;
//		return NULL;
//	}
//
//	// regardless of the queue depth, we still want this sample.  Let's get it.
//	hr = GetMutex( m_hQAccessMutex, 2000 );
//	if( FAILED( hr ) )
//	{
//		// TODO: output failure message here
//		//return E_FAIL;
//		return NULL;
//	}
//
//	strucCompleteFrame CurrentFrame = m_FrameQueue.front();
//
//	m_FrameQueue.pop();
//
//	ReleaseMutex( m_hQAccessMutex );
//
//	//Initialize the new sample
//	IMediaSample * IMS = NULL;
//	m_pTransformFilter->m_pOutput->GetDeliveryBuffer(&IMS, NULL, NULL, 0);
//	BYTE * pBuffer;
//	hr = IMS->GetPointer( &pBuffer );
//	if( FAILED( hr ) ) return NULL;
//	memcpy( pBuffer, &CurrentFrame.bSampleBuffer, sizeof( SAMPLE_SIZE_MAX) );
//
//	// So, we have the front frame.  Let's copy it to the IMediaSample we received.
//	IMS->SetDiscontinuity( CurrentFrame.bIsDiscontinuity );
//	IMS->SetSyncPoint( CurrentFrame.bIsSyncPoint );
//
//	// Voila.  Complete frame.
//	return IMS;
//}
//
//// Gets the mutex.  
//bool CKeystoneVideoOnePin::GetMutex( HANDLE hMutex, DWORD dwMaxWaitMsec )
//{
//	// Request ownership of mutex.
//	DWORD dwWaitResult = WaitForSingleObject( hMutex, dwMaxWaitMsec );
//	bool bGotMutex = false;
//	bool bDebugMessage = false;
//	switch ( dwWaitResult ) 
//	{
//		// The thread got Mutex ownership.
//		case WAIT_OBJECT_0: 
//			bGotMutex = true;
//			break; 
//
//		// Failed to get Mutex ownership after time-out expired.
//		case WAIT_TIMEOUT: 
//			bGotMutex = false; 
//			break;
//
//		// Got ownership of the abandoned Mutex object.
//		case WAIT_ABANDONED: 
//			bGotMutex = true;
//			break; 
//
//		default:
//			bGotMutex = false; 
//			break;
//	}
//
//	return bGotMutex;
//}
//
//HRESULT CKeystoneVideoOnePin::PauseNavDec()
//{
//	//Somehow we need to stop pause/block the upstream filters from delivering samples so that
//	//Keystone can just send the 30 samples in the queue as frame steps.
//
//	////IPinFlowControl WAY - Dec seems to not support IPFC, perhaps the Nav does
//	//bNavDecRunning = false;
//	//HRESULT hr;
//
//	//IPinFlowControl * DecOutFC = NULL;
//	//hr = m_Connected->QueryInterface(IID_IPinFlowControl, (void**) &DecOutFC);
//	//if (FAILED(hr)) return hr;
//
//	//hr = DecOutFC->Block(AM_PIN_FLOW_CONTROL_BLOCK, NULL);
//	//if (FAILED(hr)) return hr;
//
//	//DecOutFC->Release();
//	//return S_OK;
//
//	////STOP WAY
//	//bNavDecRunning = false;
//	//HRESULT hr;
//	//IFilterGraph * IFG;
//	//hr = m_pTransformFilter->m_pGraph->QueryInterface(IID_IFilterGraph, (void **) &IFG);
//	//if (FAILED(hr)) return hr;
//
//	//hr = IFG->FindFilterByName(L"DVD Navigator", &Nav);
//	//if (FAILED(hr)) return hr;
//	//hr = IFG->FindFilterByName(L"NVIDIA Video Decoder", &Dec);
//	//if (FAILED(hr)) return hr;
//
//	//IMediaFilter * NavMF;
//	//IMediaFilter * DecMF;
//
//	//hr = Nav->QueryInterface(IID_IMediaFilter, (void**) &NavMF);
//	//if (FAILED(hr)) return hr;
//	//hr = Dec->QueryInterface(IID_IMediaFilter, (void**) &DecMF);
//	//if (FAILED(hr)) return hr;
//
//	//hr = NavMF->Stop();
//	//if (FAILED(hr)) return hr;
//	//hr = DecMF->Stop();
//	//if (FAILED(hr)) return hr;
//
//	//IFG->Release();
//	//NavMF->Release();
//	//DecMF->Release();
//	//return S_OK;
//	return S_OK;
//}
//
//HRESULT CKeystoneVideoOnePin::RunNavDec()
//{
//	bNavDecRunning = true;
//	IFilterGraph * IFG;
//	m_pTransformFilter->m_pGraph->QueryInterface(IID_IFilterGraph, (void **) &IFG);
//
//	IFG->FindFilterByName(L"DVD Navigator", &Nav);
//	IFG->FindFilterByName(L"nVidia Video Decoder", &Dec);
//
//	IMediaFilter * NavMF;
//	IMediaFilter * DecMF;
//
//	Nav->QueryInterface(IID_IMediaFilter, (void**) &NavMF);
//	Dec->QueryInterface(IID_IMediaFilter, (void**) &DecMF);
//
//	DWORD dwCurrentTicks = GetTickCount();
//	NavMF->Run((REFERENCE_TIME) dwCurrentTicks);
//	DecMF->Run((REFERENCE_TIME) dwCurrentTicks);
//
//	IFG->Release();
//	NavMF->Release();
//	DecMF->Release();
//	return S_OK;
//}
