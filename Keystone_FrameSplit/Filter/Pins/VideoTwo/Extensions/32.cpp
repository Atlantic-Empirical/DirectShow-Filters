#include <appincludes.h>

//HRESULT CKeystoneVideoOnePin::Run32(IMediaSample* pIn)
//{
//	//New SMT telecine routine for MPEG24->NTSC29.97 using YUY2 color space
//
//	//Each sample coming in will be RFF or not
//	//Each sample coming in will be TFF or not
//
//	//IMediaSample2 * IMS2 = NULL;
//	//if (FAILED(pIn->QueryInterface(IID_IMediaSample2, (void**) &IMS2)))
//	//{
//	//	return S_FALSE;
//	//}
//	//AM_SAMPLE2_PROPERTIES SampProps;
//	//if (FAILED(IMS2->GetProperties(sizeof(AM_SAMPLE2_PROPERTIES), (BYTE*)&SampProps)))
//	//{
//	//	return S_FALSE;
//	//}
//	//IMS2->Release();
//	//if (SampProps.dwTypeSpecificFlags & 0x20000L)
//	//{
//	//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: TFF.")));
//	//}
//	//else
//	//{
//	//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: BFF.")));
//	//}
//
//	//if (SampProps.dwTypeSpecificFlags & 0x100000L)
//	//{
//	//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: RFF-Y.")));
//	//}
//	//else
//	//{
//	//	DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: RFF-N.")));
//	//}
//
//	//REMEMBER THAT WE'RE STILL WORKING WITH A 480 BUFFER HERE.
//
//	//1=1
//	//2=2
//	//3=2+3
//	//4=3+4
//	//5=4
//
//	//6=5
//	//7=6
//	//8=6+7
//	//9=7+8
//	//10=8
//
//	LPBYTE src = NULL;
//	pIn->GetPointer(&src);
//
//	//On frame 1 - increment counter, deliver input sample
//	if ((s32Counter == 0))
//	{
//		//First frame: just deliver it.
//		s32Counter += 1;
//		goto Exit32;
//	}
//
//	//Second frame: copy top half, deliver input sample
//	if (s32Counter == 1)
//	{
//		//Create bufferA
//		p32BufferA = (LPBYTE)malloc(m_pTransformFilter->CurrentOutputBufferSize);  //Make buffer that should be 699840
//
//		//Fill buffer with black
//		m_pTransformFilter->SetBufferBGColor(p32BufferA, 1440, 720, 480);
//
//		//Populate first half of bufferA
//		Populate32Buffer(true, src, p32BufferA);
//
//		s32Counter += 1;
//		goto Exit32;
//	}
//
//	//Third frame: fill bottom half of bufferA, top half of bufferB, don't deliver input sample.
//	if (s32Counter == 2)
//	{
//		//Create bufferB
//		p32BufferB = (LPBYTE)malloc(m_pTransformFilter->CurrentOutputBufferSize);  //Make buffer that should be 699840
//
//		//Fill buffer with black
//		m_pTransformFilter->SetBufferBGColor(p32BufferB, 1440, 720, 480);
//
//		//Populate second half of bufferA
//		Populate32Buffer(false, src, p32BufferA);
//
//		//Deliver bufferA sample
//        // GD: add pIn param
//		Deliver32Sample(pIn, p32BufferA, 699840);
//		free(p32BufferA);
//		p32BufferA = NULL;
//
//		//Populate the first half of bufferB
//		Populate32Buffer(true, src, p32BufferB);
//
//		//Increment counter
//		s32Counter += 1;
//
//		//Discard the whole sample #3 becuse it is used as two separate frames
//		return S_FALSE; //Tell calling method not to deliver the current sample.
//	}
//
//	//Fourth frame: copy bottom half of bufferB, deliver bufferB, deliver whole frame
//	if (s32Counter == 3)
//	{
//		Populate32Buffer(false, src, p32BufferB);
//
//		//Deliver bufferB sample
//        // GD: add pIn param
//		Deliver32Sample(pIn, p32BufferB, 699840);
//		free(p32BufferB);
//		p32BufferB = NULL;
//
//		//Reset counter
//		s32Counter = 0;
//		goto Exit32;
//	}
//Exit32:
//	return S_OK;
//}

//HRESULT CKeystoneVideoOnePin::Populate32Buffer(bool Odd, LPBYTE src, LPBYTE dst)
//{
//	//long d = 699840; //Number of bytes in dst buffer (extra 8640 for six lines)
//	//long s = 691200; //Number of bytes in src buffer
//	//short p = 1440; //Pitch
//
//	//So, what we need to do is copy the lines, in order into the proper addresses of the dst buffer.
//	LPBYTE ReadAddress = src;
//	LPBYTE WriteAddress = dst;
//
//	if (Odd == true)
//	{
//		ReadAddress = src + 1440;
//		WriteAddress = dst + 1440;
//	}
//	
//	for (int nScanLines = 0; nScanLines < 480; nScanLines += 2)
//	{
//		memcpy(WriteAddress, ReadAddress, 1440);
//		WriteAddress += 2880;
//		ReadAddress += 2880;
//	}
//	return S_OK;
//}

//HRESULT CKeystoneVideoOnePin::Deliver32Sample(IMediaSample* pIn, LPBYTE Buffer, int nBytes)
//{
//	HRESULT hr;
//	IMediaSample * sample;  // GD: I would use a smart pointer here...
//    hr = m_pTransformFilter->InitializeOutputSample(pIn, &sample);
//    if (SUCCEEDED(hr))
//    {
//		//DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: 3:2 Sample.")));
//        hr = m_pTransformFilter->MixAndDeliverVideo(Buffer, sample);
//        sample->Release();
//    }
//	return hr;
//}

//HRESULT CKeystoneVideoOnePin::TurnOn32(IMediaSample * pIn)
//{
//	bRun32 = true;
//	this->s_buffer_empty = true;
//	//Beep(200, 1);
//	s32Counter = 0;
//	pIn->GetTime(&rtLast32Sample_StartTime, &rtLast32Sample_EndTime);
//	//DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: 3:2 activated.")));
//	m_pTransformFilter->NotifyEvent(EC_KEYSTONE_32, 1, 0);
//	return S_OK;
//}
//
//HRESULT CKeystoneVideoOnePin::TurnOff32()
//{
//	bRun32 = false;
//	//Beep(1500, 1);
//	//this->s_buffer_empty = true;
//	s32Counter = 0;
//	if((p32BufferA)!= NULL)
//	{
//		free(p32BufferA);
//		p32BufferA = NULL;
//	}
//	if((p32BufferB)!= NULL)
//	{
//		free(p32BufferB);
//		p32BufferB = NULL;
//	}
//	rtLast32Sample_StartTime = 0;
//	rtLast32Sample_EndTime = 0;
//	//DbgLog((LOG_TRACE,0,TEXT("KEYSTONE: 3:2 de-activated.")));
//	m_pTransformFilter->NotifyEvent(EC_KEYSTONE_32, 0, 0);
//	return S_OK;
//}


//Old way
//HRESULT CKeystoneVideoOnePin::Run32(IMediaSample* pIn)
//{
//	//REMEMBER THAT WE'RE STILL WORKING WITH A 480 BUFFER HERE.
//
//	//1=1
//	//2=2
//	//3=2+3
//	//4=3+4
//	//5=4
//
//	//6=5
//	//7=6
//	//8=6+7
//	//9=7+8
//	//10=8
//
//	LPBYTE src = NULL;
//	pIn->GetPointer(&src);
//
//	//On frame 1 - increment counter, deliver input sample
//	if ((s32Counter == 0))
//	{
//		//First frame: just deliver it.
//		s32Counter += 1;
//		goto Exit32;
//	}
//
//	//Second frame: copy top half, deliver input sample
//	if (s32Counter == 1)
//	{
//		//Create bufferA
//		p32BufferA = (LPBYTE)malloc(m_pTransformFilter->CurrentOutputBufferSize);  //Make buffer that should be 699840
//
//		//Fill buffer with black
//		m_pTransformFilter->SetBufferBGColor(p32BufferA, 1440, 720, 480);
//
//		//Populate first half of bufferA
//		Populate32Buffer(true, src, p32BufferA);
//
//		s32Counter += 1;
//		goto Exit32;
//	}
//
//	//Third frame: fill bottom half of bufferA, top half of bufferB, don't deliver input sample.
//	if (s32Counter == 2)
//	{
//		//Create bufferB
//		p32BufferB = (LPBYTE)malloc(m_pTransformFilter->CurrentOutputBufferSize);  //Make buffer that should be 699840
//
//		//Fill buffer with black
//		m_pTransformFilter->SetBufferBGColor(p32BufferB, 1440, 720, 480);
//
//		//Populate second half of bufferA
//		Populate32Buffer(false, src, p32BufferA);
//
//		//Deliver bufferA sample
//        // GD: add pIn param
//		Deliver32Sample(pIn, p32BufferA, 699840);
//		free(p32BufferA);
//		p32BufferA = NULL;
//
//		//Populate the first half of bufferB
//		Populate32Buffer(true, src, p32BufferB);
//
//		//Increment counter
//		s32Counter += 1;
//
//		//Discard the whole sample #3 becuse it is used as two separate frames
//		return S_FALSE; //Tell calling method not to send the current sample.
//	}
//
//	//Fourth frame: copy bottom half of bufferB, deliver bufferB, deliver whole frame
//	if (s32Counter == 3)
//	{
//		Populate32Buffer(false, src, p32BufferB);
//
//		//Deliver bufferB sample
//        // GD: add pIn param
//		Deliver32Sample(pIn, p32BufferB, 699840);
//		free(p32BufferB);
//		p32BufferB = NULL;
//
//		//Reset counter
//		s32Counter = 0;
//		goto Exit32;
//	}
//Exit32:
//	return S_OK;
//}