#include "appincludes.h"
#include <atlimage.h>

HRESULT CKeystone::HandleSampleGrab(IMediaSample *IMS)
{
	//Set iSampleSize
	lSampleSize = IMS->GetSize();

	//Get mediatype->Subtype from IMS
	GUID subtype;
	GUID formattype;
	LPBYTE pbFormat;

	//CMediaType *mtSample = 0;
	switch (SampleWhat)
	{
		case 3: //full mix
		{
			subtype = m_mtOut.subtype;
			formattype = m_mtOut.formattype;
			pbFormat = m_mtOut.pbFormat;
			break;
		}
		case 0: //Video only
		{
			subtype = m_mtIn.subtype;
			formattype = m_mtIn.formattype;
			pbFormat = m_mtIn.pbFormat;
			break;
		}
		case 1: //Subpicture only
		{
			subtype = m_mtSP.subtype;
			formattype = m_mtSP.formattype;
			pbFormat = m_mtSP.pbFormat;
			break;
		}
		case 2: //line21 only
		{
			subtype = m_mtL21.subtype;
			formattype = m_mtL21.formattype;
			pbFormat = m_mtL21.pbFormat;
			break;
		}
		case 4: //video and subpicture
		{
			subtype = m_mtOut.subtype;
			formattype = m_mtOut.formattype;
			pbFormat = m_mtOut.pbFormat;
			break;
		}
	}

	//Set iSamp_W, iSamp_H;
	int srcx, srcy;
	if (formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2* VIH2 = (VIDEOINFOHEADER2*)pbFormat;
		lSamp_W = VIH2->bmiHeader.biWidth;
		lSamp_H = VIH2->bmiHeader.biHeight;
		srcx = VIH2->rcTarget.left;
		srcy = VIH2->rcTarget.top;
	}
	else if (formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER* VIH = (VIDEOINFOHEADER*)pbFormat;
		lSamp_W = VIH->bmiHeader.biWidth;
		lSamp_H = VIH->bmiHeader.biHeight;
		srcx = VIH->rcTarget.left;
		srcy = VIH->rcTarget.top;
	}
	else
	{
		VIDEOINFO* VI = (VIDEOINFO*)pbFormat;
		lSamp_W = VI->bmiHeader.biWidth;
		lSamp_H = VI->bmiHeader.biHeight;
		srcx = VI->rcTarget.left;
		srcy = VI->rcTarget.top;
	}

	//bug fix, something below didn't like non-zero values
	if (SampleWhat == 2)
	{
		srcx = 0;
		srcy = 0;
	}

	//Make a copy of the buffer to a place where it won't be destroyed when the IMS is destroyed

	//Setup Source buffer metadata
	LPBYTE src = NULL;
	IMS->GetPointer(&src);
	int srcpitch = 0;

	if (SampleWhat == 2)
	{
		srcpitch = lSamp_W; //Line21 Samples are RGB8 which is one byte per pixel
	}
	else
	{
		srcpitch = lSamp_W * 2;
	}

	src += srcx * 2 + srcy * srcpitch;

	//Create destination buffer
	LPBYTE dst = NULL;
	dst = (LPBYTE)malloc(lSampleSize);
	LPBYTE dstOrig = dst;

	//Setup destination buffer metadata
	int dstx = srcx;
	int dsty = srcy;
	int dstpitch = srcpitch;
	dst += dstx * 2 + dsty * dstpitch;

	//int ActualSize = 0;

	for (int i = 0; i < lSamp_H; i++)
	{
		memcpy(dst, src, srcpitch);
		//memcpy(dst, src, lSamp_W * 2);
		//ActualSize += lSamp_W * 2;
		src += srcpitch;
		dst += dstpitch;
	}
	
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: Actual dst Size: %d"), ActualSize));

	//Convert the data into RGB24 in yet a new buffer.
	LPBYTE ConversionTarget = 0;
	ConversionTarget = (LPBYTE)malloc(lSamp_W * lSamp_H * 3);
	//ConversionTarget = (LPBYTE)malloc(lSamp_W * lSamp_H * 2); //For debugging - Pass just UYVY bits

	if (subtype == MEDIASUBTYPE_RGB24)
	{
		//TODO: copy the data straight across from dst into Conversion target.
		pSample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_UYVY)
	{
		CSCManual::UYVYToRGB24(dstOrig, lSamp_W, lSamp_H, ConversionTarget);
		pSample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_YUY2)
	{
		CSCManual::YUY2ToRGB24(dstOrig, lSamp_W, lSamp_H, ConversionTarget);
		pSample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_RGB32)
	{
		CSCManual::RGB32ToRGB24(dstOrig, lSamp_W, lSamp_H, ConversionTarget);
		pSample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_ARGB4444) //subpicture
	{
		CSCManual::ARGB4444ToRGB24(dstOrig, lSamp_W, lSamp_H, ConversionTarget);
		pSample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_RGB8) //line21
	{
		VIDEOINFO * VI = (VIDEOINFO*) m_mtL21.Format();
		CSCManual::RGB8ToRGB24(dstOrig, lSamp_W, lSamp_H, ConversionTarget, (DWORD*) COLORS(VI));
		pSample = ConversionTarget;
	}
	else
	{
		pSample = 0;		
	}

	//Destroy intermediate buffer.
	//free(dst); 

	//pSample -= lSampleSize;

	lSampleSize = lSamp_W * lSamp_H * 3; //For RGB
	//lSampleSize = lSamp_W * lSamp_H * 2; //For debugging (UYVY bytes)

	//DEBUGGING
	NotifyEvent(EC_KEYSTONE_FORCEFRAMEGRAB, (LONG_PTR)pSample, lSamp_W * lSamp_H * 3);
	//DEBUGGING

	//is dst not freed?
	free(dst);
	//is conversiontarget never freed? managed code would have to call back to say when
	//it is done with it.
	return S_OK;
}

HRESULT CKeystone::HandleMultiFrameGrabSample(IMediaSample *IMS)
{
	LPBYTE tBuf = NULL;
	IMS->GetPointer(&tBuf);

	TCHAR buff[256];
	_stprintf(buff, _T("%s\\%d.bin"), sDumpFrameLocation, iDumpFrameCount);

	LONG TargSize = VIH2In->bmiHeader.biWidth * VIH2In->bmiHeader.biHeight * 3;
	LPBYTE pRGB = (LPBYTE)malloc(TargSize);
	CSCManual::YUY2ToRGB24(tBuf, 720, 480, pRGB);
	SaveBufferToFile_A(buff, "wb", pRGB, TargSize);

	free(pRGB);
	return S_OK;
}


HRESULT CKeystone::HandleSampleGrab_LastOutput()
{
	//Set iSampleSize
	lSampleSize = 1440 * lConnectedHeight_Out;

	//Get mediatype->Subtype from IMS
	GUID subtype = m_mtOut.subtype;
	GUID formattype = m_mtOut.formattype;
	LPBYTE pbFormat = m_mtOut.pbFormat;

	//Set iSamp_W, iSamp_H;
	int srcx, srcy;
	if (formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2* VIH2 = (VIDEOINFOHEADER2*)pbFormat;
		lSamp_W = VIH2->bmiHeader.biWidth;
		lSamp_H = VIH2->bmiHeader.biHeight;
		srcx = VIH2->rcTarget.left;
		srcy = VIH2->rcTarget.top;
	}
	else if (formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER* VIH = (VIDEOINFOHEADER*)pbFormat;
		lSamp_W = VIH->bmiHeader.biWidth;
		lSamp_H = VIH->bmiHeader.biHeight;
		srcx = VIH->rcTarget.left;
		srcy = VIH->rcTarget.top;
	}
	else
	{
		VIDEOINFO* VI = (VIDEOINFO*)pbFormat;
		lSamp_W = VI->bmiHeader.biWidth;
		lSamp_H = VI->bmiHeader.biHeight;
		srcx = VI->rcTarget.left;
		srcy = VI->rcTarget.top;
	}

	//Convert the data into RGB24 in yet a new buffer.
	LPBYTE ConversionTarget = 0;
	ConversionTarget = (LPBYTE)malloc(lSamp_W * lSamp_H * 3);
	//ConversionTarget = (LPBYTE)malloc(lSamp_W * lSamp_H * 2); //For debugging - Pass just UYVY bits

	if (subtype == MEDIASUBTYPE_RGB24)
	{
		//TODO: copy the data straight across from dst into Conversion target.
		pSample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_UYVY)
	{
		CSCManual::UYVYToRGB24(LastFrameOutput, lSamp_W, lSamp_H, ConversionTarget);
		pSample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_YUY2)
	{
		CSCManual::YUY2ToRGB24(LastFrameOutput, lSamp_W, lSamp_H, ConversionTarget);
		pSample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_RGB32)
	{
		CSCManual::RGB32ToRGB24(LastFrameOutput, lSamp_W, lSamp_H, ConversionTarget);
		pSample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_ARGB4444) //subpicture
	{
		CSCManual::ARGB4444ToRGB24(LastFrameOutput, lSamp_W, lSamp_H, ConversionTarget);
		pSample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_RGB8) //line21
	{
		VIDEOINFO * VI = (VIDEOINFO*) m_mtL21.Format();
		CSCManual::RGB8ToRGB24(LastFrameOutput, lSamp_W, lSamp_H, ConversionTarget, (DWORD*) COLORS(VI));
		pSample = ConversionTarget;
	}
	else
	{
		pSample = 0;		
	}

	//Destroy intermediate buffer.
	//free(dst); 

	//pSample -= lSampleSize;

	lSampleSize = lSamp_W * lSamp_H * 3; //For RGB
	//lSampleSize = lSamp_W * lSamp_H * 2; //For debugging (UYVY bytes)
	return S_OK;
}
