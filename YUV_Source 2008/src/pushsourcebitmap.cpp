//------------------------------------------------------------------------------
// File: PushSourceBitmap.cpp
//
// Desc: DirectShow sample code - In-memory push mode source filter
//       Provides a static bitmap as the video output stream.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------
#include "stdafx.h"


#define BITMAP_NAME TEXT("sample.bmp")

const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{
    &MEDIATYPE_Video,       // Major type
    &MEDIASUBTYPE_NULL      // Minor type
};

// Utility method to query location of installed SDK's media path
const TCHAR* DXUtil_GetDXSDKMediaPath();


/**********************************************
 *
 *  CPushPinBitmap Class
 *  
 *
 **********************************************/

CPushPinBitmap::CPushPinBitmap(HRESULT *phr, CSource *pFilter)
      : CSourceStream(NAME("SMT YUV Source"), phr, pFilter, L"Out"),
        m_rtFrameLength(FPS_24), // frames per second
		m_QueueThread()
{
	rtLastStart = 0;
	rtLastEnd = 417084; //needs to change based on the target framerate
}


CPushPinBitmap::~CPushPinBitmap()
{   
    //DbgLog((LOG_TRACE, 3, TEXT("Frames written %d"),m_iFrameNumber));

}

#define QI2(i) (riid == IID_##i) ? GetInterface((i*)this, ppv) :

STDMETHODIMP CPushPinBitmap::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	CheckPointer(ppv, E_POINTER);

	if (riid == IID_IMediaSeeking)
	{
		HRESULT hr = GetInterface((IMediaSeeking*)this, ppv);
		return hr;
	}

	return 
		QI2(IFileSourceFilterSMT)
		QI2(IMediaSeeking)
		__super::NonDelegatingQueryInterface(riid, ppv);
}

// GetMediaType: This method tells the downstream pin what types we support.

// Here is how CSourceStream deals with media types:
//
// If you support exactly one type, override GetMediaType(CMediaType*). It will then be
// called when (a) our filter proposes a media type, (b) the other filter proposes a
// type and we have to check that type.
//
// If you support > 1 type, override GetMediaType(int,CMediaType*) AND CheckMediaType.
//
// In this case we support only one type, which we obtain from the bitmap file.

HRESULT CPushPinBitmap::GetMediaType(CMediaType *pMediaType)
{
    CAutoLock cAutoLock(m_pFilter->pStateLock());
    CheckPointer(pMediaType, E_POINTER);
	return CreateVideoMediaType(1920, 1080, 16, D3DFMT_UYVY, m_rtFrameLength, pMediaType);
	//WORKS	//return CreateVideoMediaType(1920, 1080, 16, D3DFMT_UYVY, 417084, pMediaType);

	//return CreateVideoMediaType(1920, 1080, 12, D3DFMT_I420, 417084, pMediaType);
	//return CreateVideoMediaType(1920, 1080, 12, D3DFMT_IYUV, 417084, pMediaType);
}

HRESULT CPushPinBitmap::CreateVideoMediaType(int width, int height, WORD bpp, DWORD format, REFERENCE_TIME timePerFrame, CMediaType* pMediaType)
{
	VIDEOINFO* info = (VIDEOINFO*)pMediaType->AllocFormatBuffer(sizeof VIDEOINFO);
	ZeroMemory(info, sizeof VIDEOINFO);

	info->bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
	info->bmiHeader.biWidth        = width;
	info->bmiHeader.biHeight       = height;
	info->bmiHeader.biPlanes       = 1;
	info->bmiHeader.biCompression  = format;
	info->bmiHeader.biBitCount     = bpp;
	info->bmiHeader.biSizeImage    = GetBitmapSize(&info->bmiHeader);
	info->bmiHeader.biClrImportant = 0;

	SetRectEmpty(&(info->rcSource)); // we want the whole image area rendered.
	SetRectEmpty(&(info->rcTarget)); // no particular destination rectangle

	info->AvgTimePerFrame = timePerFrame;
	if (timePerFrame > 0)
		info->dwBitRate = info->bmiHeader.biSizeImage * DWORD(UNITS / timePerFrame) * 8;

	info->rcSource.right = width;
	info->rcSource.bottom = abs(height);
	info->rcTarget.right = width;
	info->rcTarget.bottom = abs(height);

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetFormatType(&FORMAT_VideoInfo);
	pMediaType->SetTemporalCompression(FALSE);

	// Work out the GUID for the subtype from the header info.
	const GUID SubTypeGUID = GetBitmapSubtype(&info->bmiHeader);
	pMediaType->SetSubtype(&SubTypeGUID);
	pMediaType->SetSampleSize(info->bmiHeader.biSizeImage);
	return S_OK;
}

HRESULT CPushPinBitmap::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest)
{
    HRESULT hr;
    CAutoLock cAutoLock(m_pFilter->pStateLock());

    CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pRequest, E_POINTER);

    // Ensure a minimum number of buffers
    pRequest->cBuffers = 8;

    //if (pRequest->cBuffers == 0)
    //{
    //    pRequest->cBuffers = 5;
    //}
    pRequest->cbBuffer = 4147200;

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pRequest, &Actual);
    if (FAILED(hr)) 
    {
        return hr;
    }

    // Is this allocator unsuitable?
    if (Actual.cbBuffer < pRequest->cbBuffer) 
    {
        return E_FAIL;
    }

    return S_OK;
}

// This is where we insert the DIB bits into the video stream.
// FillBuffer is called once for every sample in the stream.
HRESULT CPushPinBitmap::FillBuffer(IMediaSample *pSample)
{

	if (!m_QueueThread.ThreadExists())
	{
		m_QueueThread.Launch(L"F:\\Media\\Video\\YUVSample\\avacuno.iyuv");
	}

	//DbgLog((LOG_TRACE, 0, TEXT("YUVSOURCE: Enter FillBuffer.")));

	////DEBUGGING
	//REFERENCE_TIME rtStart = m_iFrameNumber * m_rtFrameLength;
 //   REFERENCE_TIME rtStop  = rtStart + m_rtFrameLength;
 //   pSample->SetTime(&rtStart, &rtStop);
 //   m_iFrameNumber++;
 //   pSample->SetSyncPoint(TRUE);	
	//Beep(1000,1);
	//LPBYTE OutFrame = NULL;
	//pSample->GetPointer(&OutFrame);
	//memset(OutFrame, 0x16, 4147200); //set frame green
	//return S_OK;
	////DEBUGGING

	//Beep(1000,1);

	HRESULT hr = S_OK;
	LPBYTE OutFrame = NULL;
	pSample->GetPointer(&OutFrame);

	//
	LPBYTE Yptr = m_QueueThread.LockFrameBuffer();
	if (NULL == Yptr)
		return E_FAIL;

	LPBYTE Uptr = Yptr + 2073600;
	LPBYTE Vptr = Uptr + 518400;
	LPBYTE ULinePtr = Uptr;
	LPBYTE VLinePtr = Vptr;
	LPBYTE Dptr = OutFrame;
	BYTE NewLineFlag = 0;

	//I420 to UYVY CSC
	for(int y = 0; y < 1080; y++) 
	{ 
		for(int x = 0; x < 3840/4; x++)
		{
			Dptr[0] = Uptr[0];
			Dptr[1] = Yptr[0];
			Yptr++;
			Dptr[2] = Vptr[0];
			Dptr[3] = Yptr[0];
			Dptr += 4;
			Yptr++;
			Uptr++;
			Vptr++;
		}

		//NOW INCREMENT
		//Note: Y incrementing handled in inner above. 
		if (NewLineFlag == 0)
		{
			//Don't increment U or V
			//So we set U and V ptrs back to the beginning of the line
			Uptr = ULinePtr;
			Vptr = VLinePtr;
			NewLineFlag = 1;
		}
		else
		{
			//Increment U and V one line
			//They have already been incremented above so there's nothing more to do.
			//Just need to set the line start pointers to the current position
			ULinePtr = Uptr;
			VLinePtr = Vptr;
			NewLineFlag = 0;
		}
	}
	//DbgLog((LOG_TRACE, 0, TEXT("YUVSOURCE: End Frame Construction.")));

	m_QueueThread.UnlockFrameBuffer();	// initiates read for the next frame.

    // Set the timestamps that will govern playback frame rate.
    // If this file is getting written out as an AVI,
    // then you'll also need to configure the AVI Mux filter to 
    // set the Average Time Per Frame for the AVI Header.
    // The current time is the sample's start

	// Issue: Why does this freeze?
	//REFERENCE_TIME rtStart = m_QueueThread.m_iFrame;
	//rtStart *= 10010010;
	//rtStart /= 24;
	//REFERENCE_TIME rtEnd = m_QueueThread.m_iFrame + 1;
	//rtEnd *= 10010010;
	//rtEnd /= 24;
	//pSample->SetTime(&rtStart, &rtEnd);

//	{
//	HRESULT: 0x80040249 (2147746377)
//	Name: VFW_E_SAMPLE_TIME_NOT_SET
//	Description: No time stamp has been set for this sample.
//
//		HRESULT hrTemp = S_OK;
//		REFERENCE_TIME rtStartRead = (-1);
//		REFERENCE_TIME rtEndRead = (-1);
//		hrTemp = pSample->GetTime(&rtStartRead, &rtEndRead);
////	}

    // Set TRUE on every sample for uncompressed frames
    pSample->SetSyncPoint(TRUE);

	//DEBUGGING
	REFERENCE_TIME rtS = 0;
	REFERENCE_TIME rtE = 0;
	pSample->GetTime(&rtS, &rtE);
	REFERENCE_TIME ScrubbingATPF = rtE - rtS;
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: IN PTs= %I64d %I64d"), rtS, rtE));
	DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ATPF=%I64d"), ScrubbingATPF));

	pSample->SetTime(&rtLastStart, &rtLastEnd);
	
	rtLastStart = rtLastStart + m_rtFrameLength;
	rtLastEnd = rtLastStart + m_rtFrameLength;

	//WORKS
	//rtLastStart = rtLastStart + 417084;
	//rtLastEnd = rtLastStart + 417084;
	//DEBUGGING

	//DbgLog((LOG_TRACE, 0, TEXT("YUVSOURCE: End FillBuffer.")));
	//Beep(1000,1);
    return S_OK;
}

HRESULT CPushPinBitmap::Notify(IBaseFilter *pSelf, Quality q)
{
    return E_FAIL;
}

// Just for debugging.
//HRESULT CPushPinBitmap::Inactive()
//{
//	HRESULT hr = CSourceStream::Inactive();
//	return hr;
//}

/**********************************************
 *
 *  CPushSourceBitmap Class
 *
 **********************************************/

CPushSourceBitmap::CPushSourceBitmap(IUnknown *pUnk, HRESULT *phr) 
			: CSource(NAME("SMT YUV Source"), pUnk, CLSID_YUV_Source)
{
    // The pin magically adds itself to our pin array.
    m_pPin = new CPushPinBitmap(phr, this);

    if (phr)
    {
        if (m_pPin == NULL)
            *phr = E_OUTOFMEMORY;
        else
            *phr = S_OK;
    }  
}


CPushSourceBitmap::~CPushSourceBitmap()
{
    delete m_pPin;
}


CUnknown * WINAPI CPushSourceBitmap::CreateInstance(IUnknown *pUnk, HRESULT *phr)
{
    CPushSourceBitmap *pNewFilter = new CPushSourceBitmap(pUnk, phr );

    if (phr)
    {
        if (pNewFilter == NULL) 
            *phr = E_OUTOFMEMORY;
        else
            *phr = S_OK;
    }

    return pNewFilter;
}

STDMETHODIMP CPushSourceBitmap::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	CheckPointer(ppv, E_POINTER);

	return 
		__super::NonDelegatingQueryInterface(riid, ppv);
}

//#define TINY_BLOCK_COPY 64       // upper limit for movsd type copy
//// The smallest copy uses the X86 "movsd" instruction, in an optimized
//// form which is an "unrolled loop".
//
//#define IN_CACHE_COPY 64 * 1024  // upper limit for movq/movq copy w/SW prefetch
//// Next is a copy that uses the MMX registers to copy 8 bytes at a time,
//// also using the "unrolled loop" optimization.   This code uses
//// the software prefetch instruction to get the data into the cache.
//
//#define UNCACHED_COPY 197 * 1024 // upper limit for movq/movntq w/SW prefetch
//// For larger blocks, which will spill beyond the cache, it's faster to
//// use the Streaming Store instruction MOVNTQ.   This write instruction
//// bypasses the cache and writes straight to main memory.  This code also
//// uses the software prefetch instruction to pre-read the data.
//// USE 64 * 1024 FOR THIS VALUE IF YOU'RE ALWAYS FILLING A "CLEAN CACHE"
//
//#define BLOCK_PREFETCH_COPY  infinity // no limit for movq/movntq w/block prefetch 
//#define CACHEBLOCK 80h // number of 64-byte blocks (cache lines) for block prefetch
//// For the largest size blocks, a special technique called Block Prefetch
//// can be used to accelerate the read operations.   Block Prefetch reads
//// one address per cache line, for a series of cache lines, in a short loop.
//// This is faster than using software prefetch.  The technique is great for
//// getting maximum read bandwidth, especially in DDR memory systems.
//
//// Inline assembly syntax for use with Visual C++
//
//void * CPushPinBitmap::memcpy_amd(void *dest, const void *src, size_t n)
//{
//  __asm {
//
//	mov		ecx, [n]		; number of bytes to copy
//	mov		edi, [dest]		; destination
//	mov		esi, [src]		; source
//	mov		ebx, ecx		; keep a copy of count
//
//	cld
//	cmp		ecx, TINY_BLOCK_COPY
//	jb		$memcpy_ic_3	; tiny? skip mmx copy
//
//	cmp		ecx, 32*1024		; don't align between 32k-64k because
//	jbe		$memcpy_do_align	;  it appears to be slower
//	cmp		ecx, 64*1024
//	jbe		$memcpy_align_done
//$memcpy_do_align:
//	mov		ecx, 8			; a trick that's faster than rep movsb...
//	sub		ecx, edi		; align destination to qword
//	and		ecx, 111b		; get the low bits
//	sub		ebx, ecx		; update copy count
//	neg		ecx				; set up to jump into the array
//	add		ecx, offset $memcpy_align_done
//	jmp		ecx				; jump to array of movsb's
//
//align 4
//	movsb
//	movsb
//	movsb
//	movsb
//	movsb
//	movsb
//	movsb
//	movsb
//
//$memcpy_align_done:			; destination is dword aligned
//	mov		ecx, ebx		; number of bytes left to copy
//	shr		ecx, 6			; get 64-byte block count
//	jz		$memcpy_ic_2	; finish the last few bytes
//
//	cmp		ecx, IN_CACHE_COPY/64	; too big 4 cache? use uncached copy
//	jae		$memcpy_uc_test
//
//// This is small block copy that uses the MMX registers to copy 8 bytes
//// at a time.  It uses the "unrolled loop" optimization, and also uses
//// the software prefetch instruction to get the data into the cache.
//align 16
//$memcpy_ic_1:			; 64-byte block copies, in-cache copy
//
//	prefetchnta [esi + (200*64/34+192)]		; start reading ahead
//
//	movq	mm0, [esi+0]	; read 64 bits
//	movq	mm1, [esi+8]
//	movq	[edi+0], mm0	; write 64 bits
//	movq	[edi+8], mm1	;    note:  the normal movq writes the
//	movq	mm2, [esi+16]	;    data to cache; a cache line will be
//	movq	mm3, [esi+24]	;    allocated as needed, to store the data
//	movq	[edi+16], mm2
//	movq	[edi+24], mm3
//	movq	mm0, [esi+32]
//	movq	mm1, [esi+40]
//	movq	[edi+32], mm0
//	movq	[edi+40], mm1
//	movq	mm2, [esi+48]
//	movq	mm3, [esi+56]
//	movq	[edi+48], mm2
//	movq	[edi+56], mm3
//
//	add		esi, 64			; update source pointer
//	add		edi, 64			; update destination pointer
//	dec		ecx				; count down
//	jnz		$memcpy_ic_1	; last 64-byte block?
//
//$memcpy_ic_2:
//	mov		ecx, ebx		; has valid low 6 bits of the byte count
//$memcpy_ic_3:
//	shr		ecx, 2			; dword count
//	and		ecx, 1111b		; only look at the "remainder" bits
//	neg		ecx				; set up to jump into the array
//	add		ecx, offset $memcpy_last_few
//	jmp		ecx				; jump to array of movsd's
//
//$memcpy_uc_test:
//	cmp		ecx, UNCACHED_COPY/64	; big enough? use block prefetch copy
//	jae		$memcpy_bp_1
//
//$memcpy_64_test:
//	or		ecx, ecx		; tail end of block prefetch will jump here
//	jz		$memcpy_ic_2	; no more 64-byte blocks left
//
//// For larger blocks, which will spill beyond the cache, it's faster to
//// use the Streaming Store instruction MOVNTQ.   This write instruction
//// bypasses the cache and writes straight to main memory.  This code also
//// uses the software prefetch instruction to pre-read the data.
//align 16
//$memcpy_uc_1:				; 64-byte blocks, uncached copy
//
//	prefetchnta [esi + (200*64/34+192)]		; start reading ahead
//
//	movq	mm0,[esi+0]		; read 64 bits
//	add		edi,64			; update destination pointer
//	movq	mm1,[esi+8]
//	add		esi,64			; update source pointer
//	movq	mm2,[esi-48]
//	movntq	[edi-64], mm0	; write 64 bits, bypassing the cache
//	movq	mm0,[esi-40]	;    note: movntq also prevents the CPU
//	movntq	[edi-56], mm1	;    from READING the destination address
//	movq	mm1,[esi-32]	;    into the cache, only to be over-written
//	movntq	[edi-48], mm2	;    so that also helps performance
//	movq	mm2,[esi-24]
//	movntq	[edi-40], mm0
//	movq	mm0,[esi-16]
//	movntq	[edi-32], mm1
//	movq	mm1,[esi-8]
//	movntq	[edi-24], mm2
//	movntq	[edi-16], mm0
//	dec		ecx
//	movntq	[edi-8], mm1
//	jnz		$memcpy_uc_1	; last 64-byte block?
//
//	jmp		$memcpy_ic_2		; almost done
//
//// For the largest size blocks, a special technique called Block Prefetch
//// can be used to accelerate the read operations.   Block Prefetch reads
//// one address per cache line, for a series of cache lines, in a short loop.
//// This is faster than using software prefetch, in this case.
//// The technique is great for getting maximum read bandwidth,
//// especially in DDR memory systems.
//$memcpy_bp_1:			; large blocks, block prefetch copy
//
//	cmp		ecx, CACHEBLOCK			; big enough to run another prefetch loop?
//	jl		$memcpy_64_test			; no, back to regular uncached copy
//
//	mov		eax, CACHEBLOCK / 2		; block prefetch loop, unrolled 2X
//	add		esi, CACHEBLOCK * 64	; move to the top of the block
//align 16
//$memcpy_bp_2:
//	mov		edx, [esi-64]		; grab one address per cache line
//	mov		edx, [esi-128]		; grab one address per cache line
//	sub		esi, 128			; go reverse order
//	dec		eax					; count down the cache lines
//	jnz		$memcpy_bp_2		; keep grabbing more lines into cache
//
//	mov		eax, CACHEBLOCK		; now that it's in cache, do the copy
//align 16
//$memcpy_bp_3:
//	movq	mm0, [esi   ]		; read 64 bits
//	movq	mm1, [esi+ 8]
//	movq	mm2, [esi+16]
//	movq	mm3, [esi+24]
//	movq	mm4, [esi+32]
//	movq	mm5, [esi+40]
//	movq	mm6, [esi+48]
//	movq	mm7, [esi+56]
//	add		esi, 64				; update source pointer
//	movntq	[edi   ], mm0		; write 64 bits, bypassing cache
//	movntq	[edi+ 8], mm1		;    note: movntq also prevents the CPU
//	movntq	[edi+16], mm2		;    from READING the destination address 
//	movntq	[edi+24], mm3		;    into the cache, only to be over-written,
//	movntq	[edi+32], mm4		;    so that also helps performance
//	movntq	[edi+40], mm5
//	movntq	[edi+48], mm6
//	movntq	[edi+56], mm7
//	add		edi, 64				; update dest pointer
//
//	dec		eax					; count down
//
//	jnz		$memcpy_bp_3		; keep copying
//	sub		ecx, CACHEBLOCK		; update the 64-byte block count
//	jmp		$memcpy_bp_1		; keep processing chunks
//
//// The smallest copy uses the X86 "movsd" instruction, in an optimized
//// form which is an "unrolled loop".   Then it handles the last few bytes.
//align 4
//	movsd
//	movsd			; perform last 1-15 dword copies
//	movsd
//	movsd
//	movsd
//	movsd
//	movsd
//	movsd
//	movsd
//	movsd			; perform last 1-7 dword copies
//	movsd
//	movsd
//	movsd
//	movsd
//	movsd
//	movsd
//
//$memcpy_last_few:		; dword aligned from before movsd's
//	mov		ecx, ebx	; has valid low 2 bits of the byte count
//	and		ecx, 11b	; the last few cows must come home
//	jz		$memcpy_final	; no more, let's leave
//	rep		movsb		; the last 1, 2, or 3 bytes
//
//$memcpy_final: 
//	emms				; clean up the MMX state
//	sfence				; flush the write buffer
//	mov		eax, [dest]	; ret value = destination pointer
//
//    }
//}
