#include "appincludes.h"
#include "CKeystone.h"
#include <atlbase.h>

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

HRESULT CKeystone::BMRD_GetBMPSample(LPBYTE pBMP, int x, int y, int w, int h, IMediaSample** ppOutSample)
{
	ASSERT(*ppOutSample == NULL);

	HRESULT hr = E_FAIL;

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

		BYTE* dst = NULL;
		hr = pSample->GetPointer(&dst);

		//LPBYTE UYVYBuff = (LPBYTE)malloc(w*h*2);
		hr = CSCManual::RGB24ToUYVY(pBMP, w, h, dst);

		//SET LAST SIX LINES TO BLACK
		if (h == 480)
		{
			hr = pSample->GetPointer(&dst);
			dst += 691200;
			for (LPBYTE i = dst; i < dst + (1440 * 6); i += 4)
			{
				memset(i	, 0x80, 1);	//U: Blue - Luma
				memset(i + 1, 0x00, 1);	//Y: Luma
				memset(i + 2, 0x80, 1);	//V: Red - Luma
				memset(i + 3, 0x00, 1);	//Y: Luma
			}
		}

		hr = pSample->SetTime(NULL, NULL);
		hr = pSample->SetSyncPoint(TRUE);
		hr = pSample->SetDiscontinuity(FALSE);
		hr = pSample->SetActualDataLength(dstw*dsth*2);
		hr = pSample->SetMediaTime(NULL, NULL);

		hr = S_OK;
	}
	return S_OK;
}

HRESULT CKeystone::BMRD_SetBMPtoBuffer(LPBYTE pBMP, int w, int h, LPBYTE targetBuffer)
{
	HRESULT hr;
	hr = CSCManual::RGB24ToUYVY(pBMP, w, h, targetBuffer);
	if (FAILED(hr)) return hr;

	//SET LAST SIX LINES TO BLACK
	if (h == 480)
	{
		targetBuffer += 691200;
		for (LPBYTE i = targetBuffer; i < targetBuffer + (1440 * 6); i += 4)
		{
			memset(i	, 0x80, 1);	//U: Blue - Luma
			memset(i + 1, 0x00, 1);	//Y: Luma
			memset(i + 2, 0x80, 1);	//V: Red - Luma
			memset(i + 3, 0x00, 1);	//Y: Luma
		}
	}
	return S_OK;
}
