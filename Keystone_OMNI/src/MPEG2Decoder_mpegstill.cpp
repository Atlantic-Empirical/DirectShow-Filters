#include "appincludes.h"
#include "CKeystone.h"
#include "MPEG2Decoder_libmpeg2.h"
#include <atlbase.h>

#pragma warning (disable:4245) 

static void __declspec(naked) __cdecl yuvtoyuy2row_MMX(BYTE* dst, BYTE* srcy, BYTE* srcu, BYTE* srcv, DWORD width)
{
	__asm {
		push	ebp
		push	edi
		push	esi
		push	ebx

		mov		edi, [esp+20] // dst
		mov		ebp, [esp+24] // srcy
		mov		ebx, [esp+28] // srcu
		mov		esi, [esp+32] // srcv
		mov		ecx, [esp+36] // width

		shr		ecx, 3

yuvtoyuy2row_loop:

		movd		mm0, [ebx]
		punpcklbw	mm0, [esi]

		movq		mm1, [ebp]
		movq		mm2, mm1
		punpcklbw	mm1, mm0
		punpckhbw	mm2, mm0

		movq		[edi], mm1
		movq		[edi+8], mm2

		add		ebp, 8
		add		ebx, 4
		add		esi, 4
        add		edi, 16

		dec		ecx
		jnz		yuvtoyuy2row_loop

		pop		ebx
		pop		esi
		pop		edi
		pop		ebp
		ret
	};

	__asm emms;

}

static void __declspec(naked) __cdecl yuvtoyuy2row_avg_MMX(BYTE* dst, BYTE* srcy, BYTE* srcu, BYTE* srcv, DWORD width, DWORD pitchuv)
{
	static const __int64 mask = 0x7f7f7f7f7f7f7f7fi64;

	__asm {
		push	ebp
		push	edi
		push	esi
		push	ebx

		movq	mm7, mask

		mov		edi, [esp+20] // dst
		mov		ebp, [esp+24] // srcy
		mov		ebx, [esp+28] // srcu
		mov		esi, [esp+32] // srcv
		mov		ecx, [esp+36] // width
		mov		eax, [esp+40] // pitchuv

		shr		ecx, 3

yuvtoyuy2row_avg_loop:

		movd		mm0, [ebx]
		punpcklbw	mm0, [esi]
		movq		mm1, mm0

		movd		mm2, [ebx + eax]
		punpcklbw	mm2, [esi + eax]
		movq		mm3, mm2

		// (x+y)>>1 == (x&y)+((x^y)>>1)

		pand		mm0, mm2
		pxor		mm1, mm3
		psrlq		mm1, 1
		pand		mm1, mm7
		paddb		mm0, mm1

		movq		mm1, [ebp]
		movq		mm2, mm1
		punpcklbw	mm1, mm0
		punpckhbw	mm2, mm0

		movq		[edi], mm1
		movq		[edi+8], mm2

		add		ebp, 8
		add		ebx, 4
		add		esi, 4
        add		edi, 16

		dec		ecx
		jnz		yuvtoyuy2row_avg_loop

		pop		ebx
		pop		esi
		pop		edi
		pop		ebp
		ret
	};
	
	__asm emms;

}

//These c versions of above mmx can be used instead by making change in BitBlt... below
//static void yuvtoyuy2row_c(BYTE* dst, BYTE* srcy, BYTE* srcu, BYTE* srcv, DWORD width)
//{
//	WORD* dstw = (WORD*)dst;
//	for(; width > 1; width -= 2)
//	{
//		*dstw++ = (*srcu++<<8)|*srcy++;
//		*dstw++ = (*srcv++<<8)|*srcy++;
//	}
//}
//
//static void yuvtoyuy2row_avg_c(BYTE* dst, BYTE* srcy, BYTE* srcu, BYTE* srcv, DWORD width, DWORD pitchuv)
//{
//	WORD* dstw = (WORD*)dst;
//	for(; width > 1; width -= 2, srcu++, srcv++)
//	{
//		*dstw++ = (((srcu[0]+srcu[pitchuv])>>1)<<8)|*srcy++;
//		*dstw++ = (((srcv[0]+srcv[pitchuv])>>1)<<8)|*srcy++;
//	}
//}

static bool BitBltFromI420ToYUY2(int w, int h, BYTE* dst, int dstpitch, BYTE* srcy, BYTE* srcu, BYTE* srcv, int srcpitch)
{
	if(w<=0 || h<=0 || (w&1) || (h&1))
		return false;

	if(srcpitch == 0) srcpitch = w;

	do
	{
		yuvtoyuy2row_MMX(dst, srcy, srcu, srcv, w);
		yuvtoyuy2row_avg_MMX(dst + dstpitch, srcy + srcpitch, srcu, srcv, w, srcpitch/2);

		dst += 2*dstpitch;
		srcy += srcpitch*2;
		srcu += srcpitch/2;
		srcv += srcpitch/2;
	}
	while((h -= 2) > 2);

	yuvtoyuy2row_MMX(dst, srcy, srcu, srcv, w);
	yuvtoyuy2row_MMX(dst + dstpitch, srcy + srcpitch, srcu, srcv, w);

	__asm emms;

	return true;
}

static bool ExtractBIH(const AM_MEDIA_TYPE* pmt, BITMAPINFOHEADER* bih)
{
	if(pmt)
	{
		if(pmt->formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)pmt->pbFormat;
			memcpy(bih, &vih->bmiHeader, sizeof(BITMAPINFOHEADER));
		}
		else if(pmt->formattype == FORMAT_VideoInfo2)
		{
			VIDEOINFOHEADER2* vih = (VIDEOINFOHEADER2*)pmt->pbFormat;
			memcpy(bih, &vih->bmiHeader, sizeof(BITMAPINFOHEADER));
		}
		else if(pmt->formattype == FORMAT_MPEGVideo)
		{
			VIDEOINFOHEADER* vih = &((MPEG1VIDEOINFO*)pmt->pbFormat)->hdr;
			memcpy(bih, &vih->bmiHeader, sizeof(BITMAPINFOHEADER));
		}
		else if(pmt->formattype == FORMAT_MPEG2_VIDEO)
		{
			VIDEOINFOHEADER2* vih = &((MPEG2VIDEOINFO*)pmt->pbFormat)->hdr;
			memcpy(bih, &vih->bmiHeader, sizeof(BITMAPINFOHEADER));
		}

		return(true);
	}
	
	return(false);
}

size_t nextpack(FILE* f)
{
	DWORD dw = ~0;

	while(!feof(f))
	{
		dw = (dw << 8) | (BYTE)fgetc(f);

		if(dw == 0x000001ba)
		{
			BYTE b = (BYTE)fgetc(f);

			if((b&0xc4) != 0x44) // mpeg2?
				return 0;

			fseek(f, 8, 1);
			fseek(f, fgetc(f)&7, 1);
		}
		else if((dw&~0xf) == 0x000001e0)
		{
			size_t size = 0;

			size = fgetc(f);
			size = (size << 8) | fgetc(f);

			for(int i = 0; i < 16 && fgetc(f) == 0xff; i++) 
				size--;

			long pos = ftell(f);

			fgetc(f);

			int hrdsize = fgetc(f);
			size -= 3 + hrdsize;
			fseek(f, hrdsize, 1);

			return size;
		}
	}

	return 0;
}

HRESULT CKeystone::GetJPSample(LPCTSTR fn, int x, int y, int w, int h, IMediaSample** ppOutSample)
{
	ASSERT(*ppOutSample == NULL);

	FILE* f = _tfopen(fn, _T("rb"));
	if(!f) return E_FAIL;

	CMpeg2Dec dec;

	static BYTE buff[65536];

	bool ps = false;
	fread(buff, 1, 4, f);
	if(*(DWORD*)buff == 0xba010000)
		ps = true;

	while(1)
	{
		mpeg2_state_t state = dec.mpeg2_parse();

		__asm emms;

		//DbgLog((LOG_TRACE, 0, TEXT("State: %d"), state));
		//DbgLog((LOG_TRACE, 0, TEXT("PS %d"), (int)ps));

		switch(state)
		{
		case STATE_BUFFER:
			{
				size_t size = !ps ? sizeof(buff) : nextpack(f);
				if(!size) {fclose(f); return E_FAIL;}
				size = fread(buff, 1, size, f);
				dec.mpeg2_buffer(buff, buff + size);
			}
			break;
		case STATE_INVALID:
			// hmmm
			break;
		case STATE_GOP:
			break;
		case STATE_SEQUENCE:
			// atf = 10i64 * dec.m_info.m_sequence->frame_period / 27;
			break;
		case STATE_PICTURE:
			break;
		case STATE_SLICE:
			//break;
		case STATE_END:
			{
				mpeg2_picture_t* picture = dec.m_info.m_display_picture;
				mpeg2_picture_t* picture_2nd = dec.m_info.m_display_picture_2nd;
				mpeg2_fbuf_t* fbuf = dec.m_info.m_display_fbuf;

				if(picture && fbuf && !(picture->flags&PIC_FLAG_SKIP))
				{
					int srcw = dec.m_info.m_sequence->picture_width;
					int srch = dec.m_info.m_sequence->picture_height;
					int srcpitch = dec.m_info.m_sequence->width;

					//SaveBufferToFile_A("C:\\Temp\\fbuf0.bin", "w", fbuf->buf[0], srcw*srch);
					//SaveBufferToFile_A("C:\\Temp\\fbuf1.bin", "w", fbuf->buf[1], (srcw*srch)/4);
					//SaveBufferToFile_A("C:\\Temp\\fbuf2.bin", "w", fbuf->buf[2], (srcw*srch)/4);

					HRESULT hr;

					CComPtr<IMediaSample> pSample;
			
					ASSERT(m_pOutput->m_pAllocator != NULL);
					hr = m_pOutput->m_pAllocator->GetBuffer(&pSample, NULL, NULL, 0);
					if(SUCCEEDED(hr))
					{
						(*ppOutSample = pSample)->AddRef();

						const CMediaType& mt = m_pOutput->CurrentMediaType();
						BITMAPINFOHEADER bih;
						ExtractBIH(&mt, &bih);

						int dstw = bih.biWidth;
						int dsth = abs(bih.biHeight);
						int dstpitch = bih.biWidth*2;

						//int dstw = w;
						//int dsth = h;
						//int dstpitch = w*2;

						BYTE* dst = NULL;
						hr = pSample->GetPointer(&dst);

						//memset(fbuf->buf[0], 0x80, srcw*srch);
						//memset(fbuf->buf[1], 0x80, (srcw*srch)/4);
						//memset(fbuf->buf[2], 0x80, (srcw*srch)/4);

						SetBufferColor_YUY2(dst, 1440, w, dsth);

						//BitBltFromI420ToYUY2(int w, int h, BYTE* dst, int dstpitch, BYTE* srcy, BYTE* srcu, BYTE* srcv, int srcpitch)

						BitBltFromI420ToYUY2(
							srcw, 
							srch, 
							dst + y*dstpitch + x*2, 
							dstpitch, 
							fbuf->buf[0], 
							fbuf->buf[1], 
							fbuf->buf[2], 
							srcpitch
						);

						//BitBltFromI420ToYUY2(
						//	min(dstw, srcw - x), 
						//	min(dsth, srch - y), 
						//	dst + y*dstpitch + x*2, 
						//	dstpitch, 
						//	fbuf->buf[0], 
						//	fbuf->buf[1], 
						//	fbuf->buf[2], 
						//	srcpitch
						//);

						////DEBUGGING
						//LPBYTE ConversionTarget = 0;
						//ConversionTarget = (LPBYTE)malloc(720 * 486 * 3);
						////CSCManual::UYVYToRGB24(tmp, 720, 486, ConversionTarget);
						//CSCManual::YUY2ToRGB24(dst, 720, 486, ConversionTarget);
						//NotifyEvent(EC_Keystone_FORCEFRAMEGRAB, (LONG_PTR)ConversionTarget, 720 * 486 * 3);
						////DEBUGGING

						CSCManual::YUY2ToUYVY(dst, dstw, dsth);

						LONGLONG rtStart = 0, rtStop = _I64_MAX;
						hr = pSample->SetTime(NULL, NULL);
						//hr = pSample->SetTime(&rtStart, &rtStop);
						hr = pSample->SetSyncPoint(TRUE);
						hr = pSample->SetDiscontinuity(TRUE);
						hr = pSample->SetActualDataLength(dstpitch*dsth);

						hr = S_OK;
					}

					fclose(f);
					return hr;
				}
			}
			break;
		}
	}
}
