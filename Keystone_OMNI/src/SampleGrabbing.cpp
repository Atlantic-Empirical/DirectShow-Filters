#include "appincludes.h"
#include <atlimage.h>

HRESULT CKeystone::SAGR_HandleSampleGrab(IMediaSample *IMS)
{
	//Set iSampleSize
	SAGR_SampleSize = IMS->GetSize();

	//Get mediatype->Subtype from IMS
	GUID subtype;
	GUID formattype;
	LPBYTE pbFormat;

	//CMediaType *mtSample = 0;
	switch (USEROPTION_SampleWhat)
	{
		case 3: //full mix
		{
			subtype = PIVO_MediaType.subtype;
			formattype = PIVO_MediaType.formattype;
			pbFormat = PIVO_MediaType.pbFormat;
			break;
		}
		case 0: //Video only
		{
			subtype = PIVI_MediaType.subtype;
			formattype = PIVI_MediaType.formattype;
			pbFormat = PIVI_MediaType.pbFormat;
			break;
		}
		case 1: //Subpicture only
		{
			subtype = PISI_MediaType.subtype;
			formattype = PISI_MediaType.formattype;
			pbFormat = PISI_MediaType.pbFormat;
			break;
		}
		case 2: //line21 only
		{
			subtype = PILI_MediaType.subtype;
			formattype = PILI_MediaType.formattype;
			pbFormat = PILI_MediaType.pbFormat;
			break;
		}
		case 4: //video and subpicture
		{
			subtype = PIVO_MediaType.subtype;
			formattype = PIVO_MediaType.formattype;
			pbFormat = PIVO_MediaType.pbFormat;
			break;
		}
	}

	//Set iSamp_W, iSamp_H;
	int srcx, srcy;
	if (formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2* VIH2 = (VIDEOINFOHEADER2*)pbFormat;
		SAGR_SampleWidth = VIH2->bmiHeader.biWidth;
		SAGR_SampleHeight = VIH2->bmiHeader.biHeight;
		srcx = VIH2->rcTarget.left;
		srcy = VIH2->rcTarget.top;
	}
	else if (formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER* VIH = (VIDEOINFOHEADER*)pbFormat;
		SAGR_SampleWidth = VIH->bmiHeader.biWidth;
		SAGR_SampleHeight = VIH->bmiHeader.biHeight;
		srcx = VIH->rcTarget.left;
		srcy = VIH->rcTarget.top;
	}
	else
	{
		VIDEOINFO* VI = (VIDEOINFO*)pbFormat;
		SAGR_SampleWidth = VI->bmiHeader.biWidth;
		SAGR_SampleHeight = VI->bmiHeader.biHeight;
		srcx = VI->rcTarget.left;
		srcy = VI->rcTarget.top;
	}

	//bug fix, something below didn't like non-zero values
	if (USEROPTION_SampleWhat == 2)
	{
		srcx = 0;
		srcy = 0;
	}

	//Make a copy of the buffer to a place where it won't be destroyed when the IMS is destroyed

	//Setup Source buffer metadata
	LPBYTE src = NULL;
	IMS->GetPointer(&src);
	int srcpitch = 0;

	if (USEROPTION_SampleWhat == 2)
	{
		srcpitch = SAGR_SampleWidth; //Line21 Samples are RGB8 which is one byte per pixel
	}
	else
	{
		srcpitch = SAGR_SampleWidth * 2;
	}

	src += srcx * 2 + srcy * srcpitch;

	//Create destination buffer
	LPBYTE dst = NULL;
	dst = (LPBYTE)malloc(SAGR_SampleSize);
	LPBYTE dstOrig = dst;

	//Setup destination buffer metadata
	int dstx = srcx;
	int dsty = srcy;
	int dstpitch = srcpitch;
	dst += dstx * 2 + dsty * dstpitch;

	//int ActualSize = 0;

	for (int i = 0; i < SAGR_SampleHeight; i++)
	{
		memcpy(dst, src, srcpitch);
		//memcpy(dst, src, SAGR_SampleWidth * 2);
		//ActualSize += SAGR_SampleWidth * 2;
		src += srcpitch;
		dst += dstpitch;
	}
	
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Actual dst Size: %d"), ActualSize));

	//Convert the data into RGB24 in yet a new buffer.
	LPBYTE ConversionTarget = 0;
	ConversionTarget = (LPBYTE)malloc(SAGR_SampleWidth * SAGR_SampleHeight * 3);
	//ConversionTarget = (LPBYTE)malloc(SAGR_SampleWidth * SAGR_SampleHeight * 2); //For debugging - Pass just UYVY bits

	if (subtype == MEDIASUBTYPE_RGB24)
	{
		//TODO: copy the data straight across from dst into Conversion target.
		SAGR_Sample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_UYVY)
	{
		CSCManual::UYVYToRGB24(dstOrig, SAGR_SampleWidth, SAGR_SampleHeight, ConversionTarget);
		SAGR_Sample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_YUY2)
	{
		CSCManual::YUY2ToRGB24(dstOrig, SAGR_SampleWidth, SAGR_SampleHeight, ConversionTarget);
		SAGR_Sample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_RGB32)
	{
		CSCManual::RGB32ToRGB24(dstOrig, SAGR_SampleWidth, SAGR_SampleHeight, ConversionTarget);
		SAGR_Sample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_ARGB4444) //subpicture
	{
		CSCManual::ARGB4444ToRGB24(dstOrig, SAGR_SampleWidth, SAGR_SampleHeight, ConversionTarget);
		SAGR_Sample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_RGB8) //line21
	{
		VIDEOINFO * VI = (VIDEOINFO*) PILI_MediaType.Format();
		CSCManual::RGB8ToRGB24(dstOrig, SAGR_SampleWidth, SAGR_SampleHeight, ConversionTarget, (DWORD*) COLORS(VI));
		SAGR_Sample = ConversionTarget;
	}
	else
	{
		SAGR_Sample = 0;		
	}

	//Destroy intermediate buffer.
	//free(dst); 

	//SAGR_Sample -= lSampleSize;

	SAGR_SampleSize = SAGR_SampleWidth * SAGR_SampleHeight * 3; //For RGB
	//lSampleSize = SAGR_SampleWidth * SAGR_SampleHeight * 2; //For debugging (UYVY bytes)

	//DEBUGGING
	NotifyEvent(EC_KEYSTONE_FORCEFRAMEGRAB, (LONG_PTR)SAGR_Sample, SAGR_SampleWidth * SAGR_SampleHeight * 3);
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
	_stprintf(buff, _T("%s\\%d.bin"), SAGR_DumpPath, SAGR_MultiFrameCount);

	LONG TargSize = PIVI_VIH2->bmiHeader.biWidth * PIVI_VIH2->bmiHeader.biHeight * 3;
	LPBYTE pRGB = (LPBYTE)malloc(TargSize);
	//CSCManual::UYVYToRGB24(tBuf, 720, 480, pRGB);
	CSCManual::YUY2ToRGB24(tBuf, 720, 480, pRGB);
	BUSA_SaveBufferToFile_A(buff, "wb", pRGB, TargSize);

	free(pRGB);
	return S_OK;
}


HRESULT CKeystone::HandleSampleGrab_LastOutput()
{
	//Set iSampleSize
	SAGR_SampleSize = 1440 * abs(PIVO_ConnectedHeight);

	//Get mediatype->Subtype from IMS
	GUID subtype = PIVO_MediaType.subtype;
	GUID formattype = PIVO_MediaType.formattype;
	LPBYTE pbFormat = PIVO_MediaType.pbFormat;

	//Set iSamp_W, iSamp_H;
	int srcx, srcy;
	if (formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2* VIH2 = (VIDEOINFOHEADER2*)pbFormat;
		SAGR_SampleWidth = VIH2->bmiHeader.biWidth;
		SAGR_SampleHeight = VIH2->bmiHeader.biHeight;
		srcx = VIH2->rcTarget.left;
		srcy = VIH2->rcTarget.top;
	}
	else if (formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER* VIH = (VIDEOINFOHEADER*)pbFormat;
		SAGR_SampleWidth = VIH->bmiHeader.biWidth;
		SAGR_SampleHeight = VIH->bmiHeader.biHeight;
		srcx = VIH->rcTarget.left;
		srcy = VIH->rcTarget.top;
	}
	else
	{
		VIDEOINFO* VI = (VIDEOINFO*)pbFormat;
		SAGR_SampleWidth = VI->bmiHeader.biWidth;
		SAGR_SampleHeight = VI->bmiHeader.biHeight;
		srcx = VI->rcTarget.left;
		srcy = VI->rcTarget.top;
	}

	//Convert the data into RGB24 in yet a new buffer.
	LPBYTE ConversionTarget = 0;
	ConversionTarget = (LPBYTE)malloc(SAGR_SampleWidth * abs(SAGR_SampleHeight) * 3);
	//ConversionTarget = (LPBYTE)malloc(SAGR_SampleWidth * SAGR_SampleHeight * 2); //For debugging - Pass just UYVY bits

	if (subtype == MEDIASUBTYPE_RGB24)
	{
		//TODO: copy the data straight across from dst into Conversion target.
		SAGR_Sample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_UYVY)
	{
		CSCManual::UYVYToRGB24(SAGR_LastBufferOutput, SAGR_SampleWidth, SAGR_SampleHeight, ConversionTarget);
		SAGR_Sample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_YUY2)
	{
		CSCManual::YUY2ToRGB24(SAGR_LastBufferOutput, SAGR_SampleWidth, SAGR_SampleHeight, ConversionTarget);
		SAGR_Sample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_RGB32)
	{
		CSCManual::RGB32ToRGB24(SAGR_LastBufferOutput, SAGR_SampleWidth, SAGR_SampleHeight, ConversionTarget);
		SAGR_Sample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_ARGB4444) //subpicture
	{
		CSCManual::ARGB4444ToRGB24(SAGR_LastBufferOutput, SAGR_SampleWidth, SAGR_SampleHeight, ConversionTarget);
		SAGR_Sample = ConversionTarget;
	}
	else if (subtype == MEDIASUBTYPE_RGB8) //line21
	{
		VIDEOINFO * VI = (VIDEOINFO*) PILI_MediaType.Format();
		CSCManual::RGB8ToRGB24(SAGR_LastBufferOutput, SAGR_SampleWidth, SAGR_SampleHeight, ConversionTarget, (DWORD*) COLORS(VI));
		SAGR_Sample = ConversionTarget;
	}
	else
	{
		SAGR_Sample = 0;		
	}

	//Destroy intermediate buffer.
	//free(dst); 

	//pSample -= lSampleSize;

	SAGR_SampleSize = SAGR_SampleWidth * abs(SAGR_SampleHeight) * 3; //For RGB
	//lSampleSize = SAGR_SampleWidth * SAGR_SampleHeight * 2; //For debugging (UYVY bytes)
	return S_OK;
}



HRESULT CKeystone::SAGR_TellPhoenixToGetFrameGrab(IMediaSample * pSample, int w, int h, int UYVY)
{
	LPBYTE tmp = NULL;
	pSample->GetPointer(&tmp);
	LPBYTE ConversionTarget = 0;
	ConversionTarget = (LPBYTE)malloc(w*h*3);
	if (UYVY==1)
	{
		CSCManual::UYVYToRGB24(tmp, w, h, ConversionTarget);
	}
	else
	{
		CSCManual::YUY2ToRGB24(tmp, w, h, ConversionTarget);
	}
	NotifyEvent(EC_KEYSTONE_FORCEFRAMEGRAB, (LONG_PTR)ConversionTarget, w*h*3);

	//Currently we are not freeing ConversionTarget so it's a leak.
	//Just don't use this function too much.

	return S_OK;
}
